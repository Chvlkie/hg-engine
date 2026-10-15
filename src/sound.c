#include "config.h"
#include "debug.h"
#include "types.h"

#include "sound.h"
#include "system.h"

extern SysTask *LONG_CALL CreateSysTask(SysTaskFunc func, void *data, int priority);
extern void LONG_CALL DestroySysTask(SysTask *task);

// Keep small cries on the original path. Larger wave archives get an on-demand
// sound heap backed by general-purpose heap 3, rather than the shared sound heap.
#define CRY_INLINE_WAVE_ARC_LIMIT 0x5000
#define CRY_WAVE_BACKING_HEAP 3
#define CRY_WAVE_HEAP_OVERHEAD 0x400

typedef struct CryWaveHeapOwner {
    void *backingHeap;
    void *memory;
    void *soundHeap;
    SysTask *cleanupTask;
} CryWaveHeapOwner;

static void DisposeCryWaveHeap(void *memory, u32 size UNUSED,
                               u32 userData1 UNUSED, u32 userData2 UNUSED)
{
    CryWaveHeapOwner *owner = memory;
    if (owner->cleanupTask != NULL) {
        DestroySysTask(owner->cleanupTask);
        owner->cleanupTask = NULL;
    }
    if (owner->soundHeap != NULL) {
        // Native destruction invalidates wave data, detaches bank links and
        // waits for ARM7 sound commands before we return the backing memory.
        NNS_SndHeapDestroy(owner->soundHeap);
        owner->soundHeap = NULL;
    }
    if (owner->memory != NULL) {
        u32 interrupts = OS_DisableInterrupts();
        NNS_FndFreeToExpHeap(owner->backingHeap, owner->memory);
        OS_RestoreInterrupts(interrupts);
        owner->memory = NULL;
    }
}

static void ReleaseFinishedCryWave(SysTask *task UNUSED, void *data)
{
    // PLAYER_PV is player 0; handles 1 and 8 are its two playback slots.
    // Wait for both so a cached waveform shared by them cannot be freed early.
    if (NNS_SndPlayerCountPlayingSeqByPlayerNo(0) == 0) {
        DisposeCryWaveHeap(data, 0, 0, 0);
    }
}

static int LoadCryWaveArc(u32 waveArcIndex, u32 loadFlag, void *heap,
                          BOOL bSetAddr, SNDWaveArc **waveArc,
                          const NNSSndArcWaveArcInfo *info)
{
    u32 fileSize = NNS_SndArcGetFileSize(info->fileId);
    if (heap == NULL || !(loadFlag & NNS_SND_ARC_LOAD_WAVE)
        || (info->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD)
        || fileSize <= CRY_INLINE_WAVE_ARC_LIMIT
        || NNS_SndArcGetFileAddress(info->fileId) != NULL) {
        return NNSi_SndArcLoadWaveArc(waveArcIndex, loadFlag, heap, bSetAddr, waveArc);
    }

    // This small allocation belongs to the original cry heap. Its disposal
    // callback also covers interruption, failed starts and sound-heap resets.
    CryWaveHeapOwner *owner = NNS_SndHeapAlloc(heap, sizeof(*owner),
                                            DisposeCryWaveHeap, 0, 0);
    if (owner == NULL) {
        return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE;
    }
    owner->backingHeap = NULL;
    owner->memory = NULL;
    owner->soundHeap = NULL;
    owner->cleanupTask = NULL;

    if (CRY_WAVE_BACKING_HEAP >= sHeapInfo.totalNumHeaps
        || fileSize > 0xFFFFFFFFu - CRY_WAVE_HEAP_OVERHEAD) {
        return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE;
    }
    u32 heapIndex = sHeapInfo.heapIdxs[CRY_WAVE_BACKING_HEAP];
    if (heapIndex >= sHeapInfo.maxHeaps
        || sHeapInfo.heapHandles[heapIndex] == NULL) {
        return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE;
    }
    owner->backingHeap = sHeapInfo.heapHandles[heapIndex];
    u32 heapSize = fileSize + CRY_WAVE_HEAP_OVERHEAD;
    u32 interrupts = OS_DisableInterrupts();
    // Raw SDK allocation returns NULL on exhaustion instead of invoking the
    // game's allocation-failure reset. Pair it with the raw SDK free above.
    owner->memory = NNS_FndAllocFromExpHeapEx(owner->backingHeap, heapSize, 32);
    OS_RestoreInterrupts(interrupts);
    if (owner->memory == NULL) {
        return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE;
    }
    owner->soundHeap = NNS_SndHeapCreate(owner->memory, heapSize);
    if (owner->soundHeap == NULL) {
        DisposeCryWaveHeap(owner, 0, 0, 0);
        return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE;
    }

    // Retain the native loader's cache flushing, file-address registration and
    // wave disposal callbacks inside the temporary sound heap.
    int result = NNSi_SndArcLoadWaveArc(waveArcIndex, loadFlag,
                                      owner->soundHeap, bSetAddr, waveArc);
    if (result == NNS_SND_ARC_LOAD_SUCCESS) {
        // A player heap may stay allocated long after its sequence stops.
        // Release the general-purpose backing RAM on playback completion,
        // rather than waiting for that player's next cry or a sound reset.
        owner->cleanupTask = CreateSysTask(ReleaseFinishedCryWave, owner, 0);
        if (owner->cleanupTask == NULL) {
            result = NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE;
        }
    }
    if (result != NNS_SND_ARC_LOAD_SUCCESS) {
        DisposeCryWaveHeap(owner, 0, 0, 0);
        *waveArc = NULL;
    }
    return result;
}

BOOL LONG_CALL GF_Snd_LoadSeq(int seqNo)
{
    BOOL ret;
    struct SND_WORK *work;

    work = GetSoundDataPointer();
    ret = NNS_SndArcLoadSeq(seqNo, work->heap);
    GF_SndHeapGetFreeSize();

#ifdef DEBUG_SOUND_SSEQ_LOADS
    if (!ret) {
        u8 buf[200];
        sprintf(buf, "[GF_Snd_LoadSeq] Failed to load song %d.  There are 0x%x bytes left in the sound heap.\n", seqNo, SoundHeapFreeSize);
        debugsyscall(buf);
    } else {
        u8 buf[200];
        sprintf(buf, "[GF_Snd_LoadSeq] Loaded song %d.  There are 0x%x bytes left in the sound heap.\n", seqNo, SoundHeapFreeSize);
        debugsyscall(buf);
    }
#endif // DEBUG_SOUND_SSEQ_LOADS

    return ret;
}

BOOL GF_Snd_LoadSeqEx(int seqNo, u32 loadFlag)
{
    BOOL ret;
    struct SND_WORK *work;

    work = GetSoundDataPointer();
    ret = NNS_SndArcLoadSeqEx(seqNo, loadFlag, work->heap);
    GF_SndHeapGetFreeSize();

#ifdef DEBUG_SOUND_SSEQ_LOADS
    if (!ret) {
        u8 buf[200];
        sprintf(buf, "[GF_Snd_LoadSeqEx] Failed to load song %d.  There are 0x%x bytes left in the sound heap.\n", seqNo, SoundHeapFreeSize);
        debugsyscall(buf);
    } else {
        u8 buf[200];
        sprintf(buf, "[GF_Snd_LoadSeqEx] Loaded song %d.  There are 0x%x bytes left in the sound heap (EX).\n", seqNo, SoundHeapFreeSize);
        debugsyscall(buf);
    }
#endif // DEBUG_SOUND_SSEQ_LOADS

    return ret;
}

#ifdef DEBUG_SOUND_SBNK_LOADS

const u8 *NNS_SND_ARC_LOAD_ERROR_STRINGS[] = {
    "NNS_SND_ARC_LOAD_SUCCESS",
    "NNS_SND_ARC_LOAD_ERROR_INVALID_GROUP_NO",
    "NNS_SND_ARC_LOAD_ERROR_INVALID_SEQ_NO",
    "NNS_SND_ARC_LOAD_ERROR_INVALID_SEQARC_NO",
    "NNS_SND_ARC_LOAD_ERROR_INVALID_BANK_NO",
    "NNS_SND_ARC_LOAD_ERROR_INVALID_WAVEARC_NO",
    "NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_SEQ",
    "NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_SEQARC",
    "NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_BANK",
    "NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE"
};

#endif // DEBUG_SOUND_SBNK_LOADS

int LONG_CALL NNSi_SndArcLoadBank(int bankNo, u32 loadFlag, void *heap, BOOL bSetAddr, struct SNDBankData **pData)
{
    const NNSSndArcBankInfo *bankInfo;
    const NNSSndArcWaveArcInfo *waveArcInfo;
    SNDBankData *bank = NULL;
    SNDWaveArc *waveArc = NULL;
    int result;
    int i;
    BOOL loadingNewCry = 0, hasLoadedCry = 0;

    // Get bank information
    if (bankNo >= CRY_PSEUDOBANK_START || (bankNo < 495 && bankNo > 1)) // assume all cry banks are loading cries
    {
        bankInfo = NNS_SndArcGetBankInfo(1);
        loadingNewCry = 1;
#ifdef DEBUG_SOUND_SBNK_LOADS
        u8 buf[200];
        sprintf(buf, "[NNSi_SndArcLoadBank] Cry load detected for bank %d (Index %d).\n", bankNo, (bankNo >= CRY_PSEUDOBANK_START) ? (bankNo - (CRY_PSEUDOBANK_START - 544)) : bankNo);
        debugsyscall(buf);
#endif // DEBUG_SOUND_SBNK_LOADS
    } else {
        bankInfo = NNS_SndArcGetBankInfo(bankNo);
    }

#ifdef DEBUG_SOUND_SBNK_LOADS
    if (bankInfo == NULL) {
        u8 buf[200];
        GF_SndHeapGetFreeSize();
        sprintf(buf, "[NNSi_SndArcLoadBank] Failed to load bank %d.  There are 0x%x bytes left in the sound heap.\n", bankNo, SoundHeapFreeSize);
        debugsyscall(buf);
    }
#endif // DEBUG_SOUND_SBNK_LOADS

    if (bankInfo == NULL) {
        return NNS_SND_ARC_LOAD_ERROR_INVALID_BANK_NO;
    }

    // If necessary to load
    if (loadFlag & NNS_SND_ARC_LOAD_BANK) {
        bank = LoadBank(bankInfo->fileId, heap, bSetAddr);
        if (bank == NULL) {
            return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_BANK;
        }
    } else {
        bank = (SNDBankData *)NNS_SndArcGetFileAddress(bankInfo->fileId);
    }

    // Load waveform data
    for (i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM; i++) {
        u32 waveArcIndex = bankInfo->waveArcNo[i];
        if (loadingNewCry && !hasLoadedCry) {
            waveArcIndex = bankNo;
            hasLoadedCry = 1;
        }

        if (waveArcIndex == NNS_SND_ARC_INVALID_WAVEARC_NO) {
            continue;
        }

        // Get waveform archive information
        waveArcInfo = NNS_SndArcGetWaveArcInfo(waveArcIndex);

        if (waveArcInfo == NULL) {
#ifdef DEBUG_SOUND_SBNK_LOADS
            u8 buf[200];
            GF_SndHeapGetFreeSize();
            sprintf(buf, "[NNSi_SndArcLoadBank] Failed to load waveArc %d using NNS_SndArcGetWaveArcInfo.  There are 0x%x bytes left in the sound heap.\n", waveArcIndex, SoundHeapFreeSize);
            debugsyscall(buf);
#endif // DEBUG_SOUND_SBNK_LOADS

            return NNS_SND_ARC_LOAD_ERROR_INVALID_WAVEARC_NO;
        }

        // Loading waveform archives
        if (loadingNewCry) {
            result = LoadCryWaveArc(waveArcIndex, loadFlag, heap, bSetAddr, &waveArc, waveArcInfo);
        } else {
            result = NNSi_SndArcLoadWaveArc(waveArcIndex, loadFlag, heap, bSetAddr, &waveArc);
        }

#ifdef DEBUG_SOUND_SBNK_LOADS

        if (result != NNS_SND_ARC_LOAD_SUCCESS) {
            u8 buf[200];
            GF_SndHeapGetFreeSize();
            sprintf(buf, "[NNSi_SndArcLoadBank] Failed to load waveArc %d (%s). There are 0x%x bytes left in the sound heap.\n", waveArcIndex, NNS_SND_ARC_LOAD_ERROR_STRINGS[result], SoundHeapFreeSize);
            debugsyscall(buf);
            return result;
        }

#else

        if (result != NNS_SND_ARC_LOAD_SUCCESS) {
            return result;
        }

#endif // DEBUG_SOUND_SBNK_LOADS

        if (waveArcInfo->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD) {
            // Individual waveform loading
            if (loadFlag & NNS_SND_ARC_LOAD_WAVE) {
                if (!LoadSingleWaves(waveArc, bank, i, waveArcInfo->fileId, heap)) {
#ifdef DEBUG_SOUND_SBNK_LOADS
                    {
                        u8 buf[200];
                        GF_SndHeapGetFreeSize();
                        sprintf(buf, "[NNSi_SndArcLoadBank] Failed to load waves for waveArc id %d using LoadSingleWaves.  There are 0x%x bytes left in the sound heap.\n", waveArcIndex, SoundHeapFreeSize);
                        debugsyscall(buf);
                    }
#endif // DEBUG_SOUND_SBNK_LOADS

                    return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE;
                }
            }
        }

        // Associate waveforms with banks
        if (bank != NULL && waveArc != NULL) {
            SND_AssignWaveArc(bank, i, waveArc);

#ifdef DEBUG_SOUND_SBNK_LOADS
            {
                u8 buf[200];
                GF_SndHeapGetFreeSize();
                sprintf(buf, "[NNSi_SndArcLoadBank] Loaded waveArc id %d fully and assigned it to in-progress loaded bank %d.  There are 0x%x bytes left in the sound heap.\n", waveArcIndex, bankNo, SoundHeapFreeSize);
                debugsyscall(buf);
            }
#endif // DEBUG_SOUND_SBNK_LOADS
        }
    }

    if (pData != NULL) {
        *pData = bank;
    }

#ifdef DEBUG_SOUND_SBNK_LOADS
    {
        u8 buf[200];
        GF_SndHeapGetFreeSize();
        sprintf(buf, "[NNSi_SndArcLoadBank] Loaded bank %d.  There are 0x%x bytes left in the sound heap.\n", bankNo, SoundHeapFreeSize);
        debugsyscall(buf);
    }
#endif // DEBUG_SOUND_SBNK_LOADS

    return NNS_SND_ARC_LOAD_SUCCESS;
}
