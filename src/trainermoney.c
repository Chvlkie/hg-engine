#include "../include/types.h"
#include "../include/constants/trainerclass.h"

struct TrainerMoney
{
    u16 class;
    u16 multiplier;
};

struct TrainerMoney PrizeMoney[] = // 0x34C04 in Overlay 12; 2 bytes for trainer class ID, 2 bytes for modifier; 516 bytes in total in vanilla
{
    { .class = TRAINERCLASS_PLAYER_M, .multiplier = 0 },
    { .class = TRAINERCLASS_PLAYER_F, .multiplier = 0 },
    { .class = TRAINERCLASS_YOUNGSTER, .multiplier = 4 },
    { .class = TRAINERCLASS_LASS, .multiplier = 4 },
    { .class = TRAINERCLASS_CAMPER, .multiplier = 4 },
    { .class = TRAINERCLASS_PICNICKER, .multiplier = 4 },
    { .class = TRAINERCLASS_BUG_CATCHER, .multiplier = 4 },
    { .class = TRAINERCLASS_AROMA_LADY, .multiplier = 8 },
    { .class = TRAINERCLASS_TWINS, .multiplier = 4 },
    { .class = TRAINERCLASS_HIKER, .multiplier = 8 },
    { .class = TRAINERCLASS_BATTLE_GIRL, .multiplier = 4 },
    { .class = TRAINERCLASS_FISHERMAN, .multiplier = 8 },
    { .class = TRAINERCLASS_CYCLIST_M, .multiplier = 8 },
    { .class = TRAINERCLASS_CYCLIST_F, .multiplier = 8 },
    { .class = TRAINERCLASS_BLACK_BELT, .multiplier = 6 },
    { .class = TRAINERCLASS_ARTIST, .multiplier = 12 },
    { .class = TRAINERCLASS_PKMN_BREEDER_M, .multiplier = 12 },
    { .class = TRAINERCLASS_PKMN_BREEDER_F, .multiplier = 12 },
    { .class = TRAINERCLASS_COWGIRL, .multiplier = 4 },
    { .class = TRAINERCLASS_JOGGER, .multiplier = 8 },
    { .class = TRAINERCLASS_POKEFAN_M, .multiplier = 16 },
    { .class = TRAINERCLASS_POKEFAN, .multiplier = 16 },
    { .class = TRAINERCLASS_POKE_KID, .multiplier = 2 },
    { .class = TRAINERCLASS_RIVAL, .multiplier = 16 },
    { .class = TRAINERCLASS_ACE_TRAINER_M, .multiplier = 15 },
    { .class = TRAINERCLASS_ACE_TRAINER_F, .multiplier = 15 },
    { .class = TRAINERCLASS_WAITRESS, .multiplier = 8 },
    { .class = TRAINERCLASS_VETERAN, .multiplier = 20 },
    { .class = TRAINERCLASS_NINJA_BOY, .multiplier = 2 },
    { .class = TRAINERCLASS_DRAGON_TAMER, .multiplier = 8 },
    { .class = TRAINERCLASS_BIRD_KEEPER_GS, .multiplier = 8 },
    { .class = TRAINERCLASS_JUGGLER, .multiplier = 8 },
    { .class = TRAINERCLASS_RICH_BOY, .multiplier = 40 },
    { .class = TRAINERCLASS_LADY, .multiplier = 40 },
    { .class = TRAINERCLASS_GENTLEMAN, .multiplier = 50 },
    { .class = TRAINERCLASS_SOCIALITE, .multiplier = 50 },
    { .class = TRAINERCLASS_BEAUTY, .multiplier = 14 },
    { .class = TRAINERCLASS_COLLECTOR, .multiplier = 16 },
    { .class = TRAINERCLASS_POLICEMAN, .multiplier = 10 },
    { .class = TRAINERCLASS_PKMN_RANGER_M, .multiplier = 15 },
    { .class = TRAINERCLASS_PKMN_RANGER_F, .multiplier = 15 },
    { .class = TRAINERCLASS_SCIENTIST, .multiplier = 12 },
    { .class = TRAINERCLASS_SWIMMER_M, .multiplier = 4 },
    { .class = TRAINERCLASS_SWIMMER_F, .multiplier = 4 },
    { .class = TRAINERCLASS_TUBER_M, .multiplier = 1 },
    { .class = TRAINERCLASS_TUBER_F, .multiplier = 1 },
    { .class = TRAINERCLASS_SAILOR, .multiplier = 8 },
    { .class = TRAINERCLASS_KIMONO_GIRL, .multiplier = 30 },
    { .class = TRAINERCLASS_RUIN_MANIAC, .multiplier = 12 },
    { .class = TRAINERCLASS_PSYCHIC_M, .multiplier = 8 },
    { .class = TRAINERCLASS_PSYCHIC_F, .multiplier = 8 },
    { .class = TRAINERCLASS_PI, .multiplier = 30 },
    { .class = TRAINERCLASS_GUITARIST, .multiplier = 6 },
    { .class = TRAINERCLASS_ACE_TRAINER_M_GS, .multiplier = 15 },
    { .class = TRAINERCLASS_ACE_TRAINER_F_GS, .multiplier = 15 },
    { .class = TRAINERCLASS_TEAM_ROCKET, .multiplier = 10 },
    { .class = TRAINERCLASS_SKIER, .multiplier = 8 },
    { .class = TRAINERCLASS_ROUGHNECK, .multiplier = 6 },
    { .class = TRAINERCLASS_CLOWN, .multiplier = 6 },
    { .class = TRAINERCLASS_ENGINEER, .multiplier = 10 },
    { .class = TRAINERCLASS_SCHOOL_KID_M, .multiplier = 5 },
    { .class = TRAINERCLASS_SCHOOL_KID_F, .multiplier = 5 },
    { .class = TRAINERCLASS_TEAM_ROCKET_F, .multiplier = 10 },
    { .class = TRAINERCLASS_BURGLAR, .multiplier = 4 },
    { .class = TRAINERCLASS_FIREBREATHER, .multiplier = 8 },
    { .class = TRAINERCLASS_BIKER, .multiplier = 4 },
    { .class = TRAINERCLASS_LEADER_BROCK, .multiplier = 30 },
    { .class = TRAINERCLASS_LEADER_MISTY, .multiplier = 30 },
    { .class = TRAINERCLASS_POKE_MANIAC, .multiplier = 16 },
    { .class = TRAINERCLASS_LEADER_LT_SURGE, .multiplier = 30 },
    { .class = TRAINERCLASS_LEADER_ERIKA, .multiplier = 30 },
    { .class = TRAINERCLASS_RANCHER, .multiplier = 10 },
    { .class = TRAINERCLASS_LEADER_BLAINE, .multiplier = 30 },
    { .class = TRAINERCLASS_LEADER_KOGA, .multiplier = 30 },
    { .class = TRAINERCLASS_LEADER_SABRINA, .multiplier = 30 },
    { .class = TRAINERCLASS_LEADER_GIOVANNI, .multiplier = 30 },
    { .class = TRAINERCLASS_TEACHER, .multiplier = 12 },
    { .class = TRAINERCLASS_SUPER_NERD, .multiplier = 12 },
    { .class = TRAINERCLASS_GAMBLER, .multiplier = 12 },
    { .class = TRAINERCLASS_MEDIUM, .multiplier = 12 },
    { .class = TRAINERCLASS_PARASOL_LADY, .multiplier = 8 },
    { .class = TRAINERCLASS_WAITER, .multiplier = 8 },
    { .class = TRAINERCLASS_CHAMPION, .multiplier = 50 },
    { .class = TRAINERCLASS_CAMERAMAN, .multiplier = 8 },
    { .class = TRAINERCLASS_REPORTER, .multiplier = 10 },
    { .class = TRAINERCLASS_IDOL, .multiplier = 18 },
    { .class = TRAINERCLASS_ELITE_FOUR_LORELEI, .multiplier = 30 },
    { .class = TRAINERCLASS_ELITE_FOUR_BRUNO, .multiplier = 30 },
    { .class = TRAINERCLASS_ELITE_FOUR_AGATHA, .multiplier = 30 },
    { .class = TRAINERCLASS_PKMN_TRAINER_RED, .multiplier = 16 },
    { .class = TRAINERCLASS_PKMN_TRAINER_CHERYL, .multiplier = 30 },
    { .class = TRAINERCLASS_PKMN_TRAINER_RILEY, .multiplier = 30 },
    { .class = TRAINERCLASS_PKMN_TRAINER_BUCK, .multiplier = 30 },
    { .class = TRAINERCLASS_PKMN_TRAINER_MIRA, .multiplier = 30 },
    { .class = TRAINERCLASS_PKMN_TRAINER_MARLEY, .multiplier = 30 },
    { .class = TRAINERCLASS_PKMN_TRAINER_FTR_LUCAS, .multiplier = 25 },
    { .class = TRAINERCLASS_PKMN_TRAINER_FTR_DAWN, .multiplier = 25 },
    { .class = TRAINERCLASS_TOWER_TYCOON, .multiplier = 0 },
    { .class = TRAINERCLASS_PKMN_TRAINER_GREEN, .multiplier = 16 },
    { .class = TRAINERCLASS_HALL_MATRON, .multiplier = 0 },
    { .class = TRAINERCLASS_FACTORY_HEAD, .multiplier = 0 },
    { .class = TRAINERCLASS_ARCADE_STAR, .multiplier = 0 },
    { .class = TRAINERCLASS_CASTLE_VALET, .multiplier = 0 },
    { .class = TRAINERCLASS_TEAM_ROCKET_JESSIE_JAMES, .multiplier = 15 },
    { .class = TRAINERCLASS_TEAM_ROCKET_JESSIE, .multiplier = 10 },
    { .class = TRAINERCLASS_TEAM_ROCKET_JAMES, .multiplier = 10 },
    { .class = TRAINERCLASS_LEADER_14, .multiplier = 30 },
    { .class = TRAINERCLASS_LEADER_15, .multiplier = 30 },
    { .class = TRAINERCLASS_PKMN_TRAINER_TBC, .multiplier = 50 },
    { .class = TRAINERCLASS_LEADER_16, .multiplier = 40 },
    { .class = TRAINERCLASS_ELDER, .multiplier = 30 },
    { .class = TRAINERCLASS_ELITE_FOUR_LANCE, .multiplier = 30 },
    { .class = TRAINERCLASS_SCIENTIST_GS, .multiplier = 8 },
    { .class = TRAINERCLASS_EXECUTIVE_ARIANA, .multiplier = 20 },
    { .class = TRAINERCLASS_BOARDER, .multiplier = 8 },
    { .class = TRAINERCLASS_EXECUTIVE_ARCHER, .multiplier = 20 },
    { .class = TRAINERCLASS_EXECUTIVE_ORION, .multiplier = 10 },
    { .class = TRAINERCLASS_EXECUTIVE_MARGOT, .multiplier = 10 },
    { .class = TRAINERCLASS_PASSERBY, .multiplier = 25 },
    { .class = TRAINERCLASS_MYSTERY_MAN, .multiplier = 30 },
    { .class = TRAINERCLASS_DOUBLE_TEAM, .multiplier = 30 },
    { .class = TRAINERCLASS_YOUNG_COUPLE, .multiplier = 16 },
    { .class = TRAINERCLASS_PKMN_TRAINER_LANCE, .multiplier = 0 },
    { .class = TRAINERCLASS_ROCKET_BOSS, .multiplier = 45 },
    { .class = TRAINERCLASS_PKMN_TRAINER_LUCAS_DP, .multiplier = 0 },
    { .class = TRAINERCLASS_PKMN_TRAINER_DAWN_DP, .multiplier = 0 },
    { .class = TRAINERCLASS_PKMN_TRAINER_LUCAS_PT, .multiplier = 0 },
    { .class = TRAINERCLASS_PKMN_TRAINER_DAWN_PT, .multiplier = 0 },
    { .class = TRAINERCLASS_BIRD_KEEPER, .multiplier = 8 },
};