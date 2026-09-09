#ifndef POKEGEAR_H
#define POKEGEAR_H

#include "types.h"
#include "task.h"
#include "save.h"

typedef struct PokegearArgs {
    u8 kind;        // 0x00
    u8 unk01;       // One of the variables inside FieldSystem with offset 0x110 (currently unk_10C[1];)
    u16 mapMusicID; // 0x02 (surf-overriden)
    u8 unk04;       // related to unk_var2 from UnkFieldSystemSub_114
    u8 unk05;
    u8 unk06;
    u8 playerGender; // 0x07
    int x;           // 0x08
    int y;           // 0x0c
    u16 mapID;       // 0x10
    u16 mapHeader;   // 0x12
    BOOL unk_14;
    int mapCursorX; // 0x18
    int mapCursorY; // 0x1C
    u16 unk_20;
    u8 matrixXCoord;         // 0x22
    u8 matrixYCoord;         // 0x23
    SaveData* saveData;      // 0x24
    void* menuInputStatePtr; // 0x28

} PokegearArgs;

void LONG_CALL sub_02092D8C(FieldSystem* sys, PokegearArgs* ptr);
void LONG_CALL PokegearTownMap_LaunchApp_Impl(FieldSystem* fieldSystem, PokegearArgs* args);
#endif