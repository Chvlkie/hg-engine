#include "../../include/bag.h"
#include "../../include/battle.h"
#include "../../include/config.h"
#include "../../include/constants/ability.h"
#include "../../include/constants/file.h"
#include "../../include/constants/game.h"
#include "../../include/constants/generated/learnsets.h"
#include "../../include/constants/hold_item_effects.h"
#include "../../include/constants/item.h"
#include "../../include/constants/moves.h"
#include "../../include/constants/species.h"
#include "../../include/constants/weather_numbers.h"
#include "../../include/debug.h"
#include "../../include/message.h"
#include "../../include/party_menu.h"
#include "../../include/pokemon.h"
#include "../../include/rtc.h"
#include "../../include/save.h"
#include "../../include/script.h"
#include "../../include/types.h"
#include "../../include/window.h"

#define TUTOR_BUG_BITE      0
#define TUTOR_MEGA_KICK     1
#define TUTOR_MEGA_PUNCH    2
#define TUTOR_STEEL_BEAM    3
#define TUTOR_ROCK_BLAST    4
#define TUTOR_COVET         5
#define TUTOR_COSMIC_POWER  6
#define TUTOR_SOFTBOILED    7
#define TUTOR_MIMIC         8
#define TUTOR_ELECTROWEB    9
#define TUTOR_SUBSTITUTE    10
#define TUTOR_ANCIENT_POWER 11
#define TUTOR_LASER_FOCUS   12
#define TUTOR_LAVA_PLUME    13
#define TUTOR_FREEZE_DRY    14
#define TUTOR_METEOR_BEAM   15
#define TUTOR_BATTLE_F_1    16
#define TUTOR_BATTLE_F_2    17

typedef struct TutorMove {
    u16 move;
    u8 cost;
    u8 tutorNpc;
} TutorMove;

TutorMove sTutorMoves[] = {
    { MOVE_BUG_BITE, 0, TUTOR_BUG_BITE },
    { MOVE_MEGA_KICK, 0, TUTOR_MEGA_KICK },
    { MOVE_MEGA_PUNCH, 0, TUTOR_MEGA_PUNCH },
    { MOVE_STEEL_BEAM, 0, TUTOR_STEEL_BEAM },
    { MOVE_ROCK_BLAST, 0, TUTOR_ROCK_BLAST },
    { MOVE_COVET, 0, TUTOR_COVET },
    { MOVE_COSMIC_POWER, 0, TUTOR_COSMIC_POWER },
    { MOVE_SOFT_BOILED, 0, TUTOR_SOFTBOILED },
    { MOVE_MIMIC, 0, TUTOR_MIMIC },
    { MOVE_ELECTROWEB, 0, TUTOR_ELECTROWEB },
    { MOVE_SUBSTITUTE, 0, TUTOR_SUBSTITUTE },
    { MOVE_ANCIENT_POWER, 0, TUTOR_ANCIENT_POWER },
    { MOVE_LASER_FOCUS, 0, TUTOR_LASER_FOCUS },
    { MOVE_LAVA_PLUME, 0, TUTOR_LAVA_PLUME },
    { MOVE_FREEZE_DRY, 0, TUTOR_FREEZE_DRY },
    { MOVE_METEOR_BEAM, 0, TUTOR_METEOR_BEAM },
    { MOVE_HELPING_HAND, 40, TUTOR_BATTLE_F_2 },
    { MOVE_ENDEAVOR, 64, TUTOR_BATTLE_F_2 },
    { MOVE_OUTRAGE, 48, TUTOR_BATTLE_F_2 },
    { MOVE_SHADOW_BALL, 40, TUTOR_BATTLE_F_2 },
    { MOVE_SYNTHESIS, 40, TUTOR_BATTLE_F_2 },
    { MOVE_SIGNAL_BEAM, 40, TUTOR_BATTLE_F_2 },
    { MOVE_ZEN_HEADBUTT, 64, TUTOR_BATTLE_F_2 },
    { MOVE_VACUUM_WAVE, 48, TUTOR_BATTLE_F_2 },
    { MOVE_EARTH_POWER, 40, TUTOR_BATTLE_F_2 },
    { MOVE_GUNK_SHOT, 32, TUTOR_BATTLE_F_2 },
    { MOVE_TWISTER, 40, TUTOR_BATTLE_F_2 },
    { MOVE_SEED_BOMB, 40, TUTOR_BATTLE_F_2 },
    { MOVE_IRON_DEFENSE, 40, TUTOR_BATTLE_F_2 },
    { MOVE_MAGNET_RISE, 40, TUTOR_BATTLE_F_2 },
    { MOVE_LAST_RESORT, 48, TUTOR_BATTLE_F_2 },
    { MOVE_BOUNCE, 32, TUTOR_BATTLE_F_2 },
    { MOVE_TRICK, 48, TUTOR_BATTLE_F_2 },
    { MOVE_HEAT_WAVE, 48, TUTOR_BATTLE_F_2 },
    { MOVE_KNOCK_OFF, 40, TUTOR_BATTLE_F_2 },
    { MOVE_SUCKER_PUNCH, 40, TUTOR_BATTLE_F_2 },
    { MOVE_SWIFT, 40, TUTOR_BATTLE_F_2 },
    { MOVE_UPROAR, 48, TUTOR_BATTLE_F_2 },
    { MOVE_SUPER_FANG, 40, TUTOR_BATTLE_F_2 },
    { MOVE_PAIN_SPLIT, 64, TUTOR_BATTLE_F_2 },
    { MOVE_STRING_SHOT, 32, TUTOR_BATTLE_F_2 },
    { MOVE_TAILWIND, 48, TUTOR_BATTLE_F_2 },
    { MOVE_GRAVITY, 32, TUTOR_BATTLE_F_2 },
    { MOVE_WORRY_SEED, 32, TUTOR_BATTLE_F_2 },
    { MOVE_MAGIC_COAT, 32, TUTOR_BATTLE_F_2 },
    { MOVE_ROLE_PLAY, 48, TUTOR_BATTLE_F_2 },
    { MOVE_HEAL_BELL, 48, TUTOR_BATTLE_F_2 },
    { MOVE_LOW_KICK, 32, TUTOR_BATTLE_F_2 },
    { MOVE_SKY_ATTACK, 64, TUTOR_BATTLE_F_2 },
    { MOVE_BLOCK, 32, TUTOR_BATTLE_F_2 },
    { MOVE_CHARGE_BEAM, 32, TUTOR_BATTLE_F_2 },
    { MOVE_HEADBUTT, 0, TUTOR_BATTLE_F_2 },
};

u16 LONG_CALL GetLearnableTutorMoves(struct PartyPokemon *mon, u32 moveTutorNpc, u8 dest[])
{
    int i;
    u16 currentMoves[MAX_MON_MOVES];
    for (i = 0; i < MAX_MON_MOVES; i++) {
        currentMoves[i] = GetMonData(mon, MON_DATA_MOVE1 + i, NULL);
    }

    u32 species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    u32 form = GetMonData(mon, MON_DATA_FORM, NULL);
    u32 tutorLearnset[TUTOR_LEARNSETS_BITFIELD_COUNT];

    ArchiveDataLoadOfs(tutorLearnset, ARC_CODE_ADDONS, CODE_ADDON_TUTOR_LEARNSETS, PokeOtherFormMonsNoGet(species, form) * TUTOR_LEARNSETS_BITFIELD_COUNT * sizeof(u32), TUTOR_LEARNSETS_BITFIELD_COUNT * sizeof(u32));

    u16 numLearnableMoves = 0;
    for (u32 j = 0; j < NELEMS(sTutorMoves); j++) {
        BOOL canLearnMove = (tutorLearnset[j / 32] >> (j % 32)) & 1;
        if (canLearnMove && moveTutorNpc == sTutorMoves[j].tutorNpc) {
            for (i = 0; i < MAX_MON_MOVES; i++) {
                if (currentMoves[i] == sTutorMoves[j].move) {
                    break;
                }
            }
            if (i < MAX_MON_MOVES) {
                continue;
            }
            if (dest != NULL) {
                dest[numLearnableMoves] = j;
            }
            numLearnableMoves++;
        }
    }

    return numLearnableMoves;
}
