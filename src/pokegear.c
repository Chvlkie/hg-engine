#include "../include/types.h"
#include "../include/task.h"
#include "../include/memory.h"
#include "../include/pokegear.h"

PokegearArgs* ReplacePokegearWithTownMap(FieldSystem* fieldSystem) {
    PokegearArgs* args = sys_AllocMemoryLo(11, sizeof(PokegearArgs));
    MI_CpuFill8(args, 0, sizeof(PokegearArgs));
    sub_02092D8C(fieldSystem, args);
    args->kind = 1;
    PokegearTownMap_LaunchApp_Impl(fieldSystem, args);
    return args;
}