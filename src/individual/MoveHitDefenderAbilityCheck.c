#include "../../include/battle.h"
#include "../../include/debug.h"
#include "../../include/pokemon.h"
#include "../../include/types.h"
#include "../../include/constants/ability.h"
#include "../../include/constants/battle_script_constants.h"
#include "../../include/constants/hold_item_effects.h"
#include "../../include/constants/item.h"
#include "../../include/constants/move_effects.h"
#include "../../include/constants/moves.h"
#include "../../include/constants/species.h"
#include "../../include/constants/weather_numbers.h"
#include "../../include/constants/battle_message_constants.h"
#include "../../include/constants/file.h"

/**
 *  @brief check if a move should activate the defender's ability and run a subscript
 *
 *  @param bw battle work structure; void * because we haven't defined the battle work structure
 *  @param sp global battle structure
 *  @param seq_no battle subscript to run
 *  @return TRUE to load the battle subscript in *seq_no and run it; FALSE otherwise
 */
BOOL MoveHitDefenderAbilityCheckInternal(void *bw, struct BattleStruct *sp, int *seq_no)
{
    BOOL ret = FALSE;
    u32 move_pos;

    if (sp->defence_client == 0xFF) {
        return ret;
    }

    if (CheckSubstitute(sp, sp->defence_client) == TRUE) {
        return ret;
    }

    switch (GetBattlerAbility(sp, sp->defence_client)) {
        case ABILITY_STATIC:
            if ((sp->battlemon[sp->attack_client].hp)
                && (sp->battlemon[sp->attack_client].condition == 0)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage))
                && (IsContactBeingMade(bw, sp))
                && (BattleRand(bw) % 10 < 3)) {
                sp->addeffect_type = ADD_STATUS_ABILITY;
                sp->state_client = sp->attack_client;
                sp->battlerIdTemp = sp->defence_client;
                seq_no[0] = SUB_SEQ_APPLY_PARALYSIS;
                ret = TRUE;
            }
            break;
        case ABILITY_COLOR_CHANGE:
            {
                if (GetBattlerAbility(sp, sp->attack_client) == ABILITY_SHEER_FORCE && sp->battlemon[sp->attack_client].sheer_force_flag == 1) { // sheer force doesn't let color change activate
                    return FALSE;
                }

                u8 movetype;

                movetype = GetAdjustedMoveType(sp, sp->attack_client, sp->current_move_index); // new normalize checks

                if ((sp->battlemon[sp->defence_client].hp)
                    && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                    && (sp->current_move_index != MOVE_STRUGGLE)
                    && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                        (sp->oneSelfFlag[sp->defence_client].special_damage))
                    && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                    && (sp->moveTbl[sp->current_move_index].power)
                    && (BattlePokemonParamGet(sp, sp->defence_client, BATTLE_MON_DATA_TYPE1, NULL) != movetype)
                    && (BattlePokemonParamGet(sp, sp->defence_client, BATTLE_MON_DATA_TYPE2, NULL) != movetype)
                    && (sp->battlemon[sp->defence_client].condition2 & STATUS2_SUBSTITUTE) == 0
                    && (sp->multiHitCount <= 1)) // don't activate until the last hit of a multi-hit move
                {
                    seq_no[0] = SUB_SEQ_COLOR_CHANGE;
                    sp->msg_work = movetype;
                    ret = TRUE;
                }
            }
            break;
        case ABILITY_ROUGH_SKIN:
        case ABILITY_IRON_BARBS:
            if ((sp->battlemon[sp->attack_client].hp)
                && (GetBattlerAbility(sp, sp->attack_client) != ABILITY_MAGIC_GUARD)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage))
                && (IsContactBeingMade(bw, sp))) {
                sp->hp_calc_work = BattleDamageDivide(sp->battlemon[sp->attack_client].maxhp * -1, 8);
                sp->battlerIdTemp = sp->attack_client;
                seq_no[0] = SUB_SEQ_ROUGH_SKIN;
                ret = TRUE;
            }
            break;
        case ABILITY_EFFECT_SPORE:
            if ((sp->battlemon[sp->attack_client].hp)
                && (sp->battlemon[sp->attack_client].condition == 0)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage))
                && (IsContactBeingMade(bw, sp))
                && (BattleRand(bw) % 10 < 3)) {
                switch (BattleRand(bw) % 3) {
                    case 0:
                    default:
                        seq_no[0] = SUB_SEQ_APPLY_POISON;
                        break;
                    case 1:
                        seq_no[0] = SUB_SEQ_APPLY_PARALYSIS;
                        break;
                    case 2:
                        seq_no[0] = SUB_SEQ_APPLY_SLEEP;
                        break;
                }
                sp->addeffect_type = ADD_STATUS_ABILITY;
                sp->state_client = sp->attack_client;
                sp->battlerIdTemp = sp->defence_client;
                ret = TRUE;
            }
            break;
        case ABILITY_POISON_POINT:
            if ((sp->battlemon[sp->attack_client].hp)
                && (sp->battlemon[sp->attack_client].condition == 0)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage))
                && (IsContactBeingMade(bw, sp))
                && (BattleRand(bw) % 10 < 3)) {
                sp->addeffect_type = ADD_STATUS_ABILITY;
                sp->state_client = sp->attack_client;
                sp->battlerIdTemp = sp->defence_client;
                seq_no[0] = SUB_SEQ_APPLY_POISON;
                ret = TRUE;
            }
            break;
        case ABILITY_FLAME_BODY:
            if ((sp->battlemon[sp->attack_client].hp)
                && (sp->battlemon[sp->attack_client].condition == 0)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage))
                && (IsContactBeingMade(bw, sp))
                && (BattleRand(bw) % 10 < 3)) {
                sp->addeffect_type = ADD_STATUS_ABILITY;
                sp->state_client = sp->attack_client;
                sp->battlerIdTemp = sp->defence_client;
                seq_no[0] = SUB_SEQ_APPLY_BURN;
                ret = TRUE;
            }
            break;
        case ABILITY_CUTE_CHARM:
            if ((sp->battlemon[sp->attack_client].hp)
                && ((sp->battlemon[sp->attack_client].condition2 & STATUS2_ATTRACT) == 0)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage))
                && (IsContactBeingMade(bw, sp))
                && (sp->battlemon[sp->defence_client].hp)
                && (BattleRand(bw) % 10 < 3)) {
                sp->addeffect_type = ADD_STATUS_ABILITY;
                sp->state_client = sp->attack_client;
                sp->battlerIdTemp = sp->defence_client;
                seq_no[0] = SUB_SEQ_APPLY_ATTRACT;
                ret = TRUE;
            }
            break;
        case ABILITY_AFTERMATH:
            if ((sp->defence_client == sp->fainting_client)
                && (GetBattlerAbility(sp, sp->attack_client) != ABILITY_MAGIC_GUARD)
                && (CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, 0, ABILITY_DAMP) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && (sp->battlemon[sp->attack_client].hp)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && (IsContactBeingMade(bw, sp))) {
                sp->hp_calc_work = BattleDamageDivide(sp->battlemon[sp->attack_client].maxhp * -1, 4);
                sp->battlerIdTemp = sp->attack_client;
                seq_no[0] = SUB_SEQ_HANDLE_AFTERMATH;
                ret = TRUE;
            }
            break;
        case ABILITY_INNARDS_OUT:
            if ((sp->defence_client == sp->fainting_client)
                && (GetBattlerAbility(sp, sp->attack_client) != ABILITY_MAGIC_GUARD)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && (sp->battlemon[sp->attack_client].hp)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0))
            {
                sp->hp_calc_work = sp->damage;
                sp->battlerIdTemp = sp->attack_client;
                seq_no[0] = SUB_SEQ_HANDLE_INNARDS_OUT_MESSAGE;
                ret = TRUE;
            }
            break;
        // handle rattled
        case ABILITY_RATTLED:
            if ((sp->battlemon[sp->defence_client].hp)
                && (sp->battlemon[sp->defence_client].states[STAT_SPEED] < 12)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage)))
            {
                u8 movetype;

                movetype = GetAdjustedMoveType(sp, sp->attack_client, sp->current_move_index); // new normalize checks

                if ((movetype == TYPE_DARK) || (movetype == TYPE_GHOST) || (movetype == TYPE_BUG))
                {
                    sp->addeffect_param = ADD_STATUS_EFF_BOOST_STATS_SPEED_UP;
                    sp->addeffect_type = ADD_EFFECT_ABILITY;
                    sp->state_client = sp->defence_client;
                    sp->battlerIdTemp = sp->defence_client;
                    seq_no[0] = SUB_SEQ_BOOST_STATS;
                    ret = TRUE;
                }
            }
        break;
        // handle berserk
        case ABILITY_BERSERK:
            if
            (
                (sp->battlemon[sp->defence_client].hp)
                && (sp->battlemon[sp->defence_client].states[STAT_SPATK] < 12)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) || (sp->oneSelfFlag[sp->defence_client].special_damage))
                // berserk doesn't activate if the Pokémon gets attacked by a sheer force boosted move
                && !((GetBattlerAbility(sp, sp->attack_client) == ABILITY_SHEER_FORCE) && (sp->battlemon[sp->attack_client].sheer_force_flag == 1))
                // berserk doesn't activate until the last hit of a multi-hit move
                && (sp->multiHitCount <= 1)
                && (sp->battlemon[sp->defence_client].hp <= (s32)(sp->battlemon[sp->defence_client].maxhp / 2))
                && (
                    // checks if the pokémon has gone below half HP from the current damage instance
                    // physical_damage and special_damage contain the relevant damage value that was just dealt, but the value is negative
                    ((sp->battlemon[sp->defence_client].hp - (sp->oneSelfFlag[sp->defence_client].physical_damage)) > (s32)sp->battlemon[sp->defence_client].maxhp / 2) ||
                    ((sp->battlemon[sp->defence_client].hp - (sp->oneSelfFlag[sp->defence_client].special_damage)) > (s32)sp->battlemon[sp->defence_client].maxhp / 2)
                   )
            )
            {
                sp->addeffect_param = ADD_STATUS_EFF_BOOST_STATS_SP_ATK_UP;
                sp->addeffect_type = ADD_EFFECT_ABILITY;
                sp->state_client = sp->defence_client;
                sp->battlerIdTemp = sp->defence_client;
                seq_no[0] = SUB_SEQ_BOOST_STATS;
                ret = TRUE;
            }
        break;
        case ABILITY_ELECTROMORPHOSIS:
            if (/*(sp->battlemon[sp->defence_client].hp) // mon notably does not need to be alive for this ability to proc
                && */((sp->oneSelfFlag[sp->defence_client].physical_damage) || (sp->oneSelfFlag[sp->defence_client].special_damage)))
            {
                sp->addeffect_param = ADD_STATUS_EFF_CHARGE;
                sp->addeffect_type = ADD_EFFECT_ABILITY;
                sp->state_client = sp->defence_client;
                seq_no[0] = SUB_SEQ_HANDLE_CHARGE_BOOST;
                ret = TRUE;
            }
        break;
        case ABILITY_WIND_POWER:
            // tailwind activation handled in SwitchInAbilityCheck
            if (/*(sp->battlemon[sp->defence_client].hp) // mon notably does not need to be alive for this ability to proc
                && */((sp->oneSelfFlag[sp->defence_client].physical_damage) || (sp->oneSelfFlag[sp->defence_client].special_damage))
                && (IsMoveWindMove(sp->current_move_index)))
            {
                sp->addeffect_param = ADD_STATUS_EFF_CHARGE;
                sp->addeffect_type = ADD_EFFECT_ABILITY;
                sp->battlerIdTemp = sp->defence_client;
                sp->waza_work = sp->current_move_index;
                seq_no[0] = SUB_SEQ_HANDLE_CHARGE_BOOST;
                ret = TRUE;
            }
        break;
        case ABILITY_STAMINA:
            if ((sp->battlemon[sp->defence_client].hp)
                && (sp->battlemon[sp->defence_client].states[STAT_DEFENSE] < 12)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage)))
            {
                sp->addeffect_param = ADD_STATUS_EFF_BOOST_STATS_DEFENSE_UP;
                sp->addeffect_type = ADD_EFFECT_ABILITY;
                sp->state_client = sp->defence_client;
                sp->battlerIdTemp = sp->defence_client;
                seq_no[0] = SUB_SEQ_BOOST_STATS;
                ret = TRUE;
            }
            break;
        case ABILITY_GOOEY:
        case ABILITY_TANGLING_HAIR:
            if ((sp->battlemon[sp->attack_client].states[STAT_SPEED] > 0)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && (IsContactBeingMade(bw, sp))
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage)))
            {
                sp->addeffect_param = ADD_STATUS_EFF_BOOST_STATS_SPEED_DOWN;
                sp->addeffect_type = ADD_EFFECT_PRINT_WORK_ABILITY;
                sp->state_client = sp->attack_client;
                sp->battlerIdTemp = sp->defence_client;
                seq_no[0] = SUB_SEQ_BOOST_STATS;
                ret = TRUE;
            }
            break;
        case ABILITY_MUMMY:
            FALLTHROUGH;
        case ABILITY_LINGERING_AROMA:
            if (sp->battlemon[sp->attack_client].ability == sp->battlemon[sp->defence_client].ability) {
                break;
            }
            if (((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && (IsContactBeingMade(bw, sp))
                && (!AbilityCantSupress(sp->battlemon[sp->attack_client].ability))
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage)))
            {
                sp->addeffect_type = ADD_EFFECT_ABILITY;
                sp->battlerIdTemp = sp->attack_client;
                sp->battlemon[sp->attack_client].ability = GetBattlerAbility(sp, sp->defence_client); // spread defender ability to attacker
                seq_no[0] = SUB_SEQ_HANDLE_MUMMY_MESSAGE;
                ret = TRUE;
            }
            break;
        case ABILITY_WATER_COMPACTION:
            if ((sp->battlemon[sp->defence_client].hp)
                && (sp->battlemon[sp->defence_client].states[STAT_DEFENSE] < 12)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage)))
            {
                u8 movetype;

                movetype = GetAdjustedMoveType(sp, sp->attack_client, sp->current_move_index); // new normalize checks

                if (movetype == TYPE_WATER)
                {
                    if(sp->battlemon[sp->defence_client].states[STAT_DEFENSE] < 11)
                    {
                        sp->addeffect_param = ADD_STATUS_EFF_BOOST_STATS_DEFENSE_UP_2;
                        sp->addeffect_type = ADD_EFFECT_ABILITY;
                        sp->state_client = sp->defence_client;
                        sp->battlerIdTemp = sp->defence_client;
                        seq_no[0] = SUB_SEQ_BOOST_STATS;
                        ret = TRUE;
                    }
                    else
                    {
                        sp->addeffect_param = ADD_STATUS_EFF_BOOST_STATS_DEFENSE_UP;
                        sp->addeffect_type = ADD_EFFECT_ABILITY;
                        sp->state_client = sp->defence_client;
                        sp->battlerIdTemp = sp->defence_client;
                        seq_no[0] = SUB_SEQ_BOOST_STATS;
                        ret = TRUE;
                    }
                }
            }
            break;
        case ABILITY_JUSTIFIED:
            if ((sp->battlemon[sp->defence_client].hp)
                && (sp->battlemon[sp->defence_client].states[STAT_SPEED] < 12)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage)))
            {
                u8 movetype;

                movetype = GetAdjustedMoveType(sp, sp->attack_client, sp->current_move_index); // new normalize checks

                if (movetype == TYPE_DARK)
                {
                    sp->addeffect_param = ADD_STATUS_EFF_BOOST_STATS_ATTACK_UP;
                    sp->addeffect_type = ADD_EFFECT_ABILITY;
                    sp->state_client = sp->defence_client;
                    sp->battlerIdTemp = sp->defence_client;
                    seq_no[0] = SUB_SEQ_BOOST_STATS;
                    ret = TRUE;
                }
            }
            break;
        case ABILITY_WEAK_ARMOR:
            if ((sp->battlemon[sp->defence_client].hp)
                && ((sp->battlemon[sp->defence_client].states[STAT_DEFENSE] > 0)
                ||  (sp->battlemon[sp->defence_client].states[STAT_SPEED] < 12))
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && (sp->oneSelfFlag[sp->defence_client].physical_damage))
            {
                sp->state_client = sp->defence_client;
                sp->battlerIdTemp = sp->defence_client;
                sp->addeffect_type = ADD_EFFECT_ABILITY;
                seq_no[0] = SUB_SEQ_HANDLE_WEAK_ARMOR;
                ret = TRUE;
            }
            break;
        // case ABILITY_DEFIANT:
        //     if ((sp->battlemon[sp->defence_client].hp != 0)
        //      && (sp->oneSelfFlag[sp->state_client].defiant_flag)
        //      && (sp->battlemon[sp->defence_client].states[STAT_ATTACK] < 12)
        //      && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
        //      && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
        //      && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0))
        //     {
        //         sp->oneSelfFlag[sp->state_client].defiant_flag = 0;
        //         sp->state_client = sp->defence_client;
        //         sp->battlerIdTemp = sp->defence_client;
        //         sp->addeffect_type = ADD_EFFECT_ABILITY;
        //         seq_no[0] = SUB_SEQ_HANDLE_DEFIANT;
        //         ret = TRUE;
        //     }
        //     break;
        // // handle competitive; copypaste from above with different sub_seq
        // case ABILITY_COMPETITIVE:
        //     if ((sp->battlemon[sp->defence_client].hp != 0)
        //      && (sp->oneSelfFlag[sp->state_client].defiant_flag)
        //      && (sp->battlemon[sp->defence_client].states[STAT_SPATK] < 12)
        //      && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
        //      && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
        //      && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0))
        //     {
        //         sp->oneSelfFlag[sp->state_client].defiant_flag = 0;
        //         sp->state_client = sp->defence_client;
        //         sp->battlerIdTemp = sp->defence_client;
        //         sp->addeffect_type = ADD_EFFECT_ABILITY;
        //         seq_no[0] = SUB_SEQ_HANDLE_COMPETITIVE;
        //         ret = TRUE;
        //     }
        //     break;
        // handle pickpocket - steal attacker's item if it can
        case ABILITY_PICKPOCKET:
            if (sp->battlemon[sp->defence_client].hp != 0
             && (sp->battlemon[sp->attack_client].condition == 0)
             && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
             && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
             && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
             && ((sp->oneSelfFlag[sp->defence_client].physical_damage)
              || (sp->oneSelfFlag[sp->defence_client].special_damage))
             && IsContactBeingMade(bw, sp)
             && sp->moveTbl[sp->current_move_index].power != 0
             // can not steal an item if you already have one
             && sp->battlemon[sp->defence_client].item == ITEM_NONE
             // if the attacker has its species-specific item or the target would get its item, then pickpocket can not activate
             && CanTrickHeldItem(sp, sp->attack_client, sp->defence_client)
             // pickpocket doesn't activate if attacked by sheer force
             && !(GetBattlerAbility(sp, sp->attack_client) == ABILITY_SHEER_FORCE && sp->battlemon[sp->attack_client].sheer_force_flag == 1)
             // does not hit until the last hit of a multi-strike move
             && (sp->multiHitCount <= 1))
            {
                seq_no[0] = SUB_SEQ_HANDLE_PICKPOCKET_DEF;
                ret = TRUE;
            }
            break;
        // handle cursed body - disable the last used move by the pokemon.  disabling is handled here, script just displays the message
        case ABILITY_CURSED_BODY:
            move_pos = BattleMon_GetMoveIndex(&sp->battlemon[sp->attack_client], sp->current_move_index);
            if (sp->battlemon[sp->attack_client].hp != 0
             && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
             && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
             && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
             && ((sp->oneSelfFlag[sp->defence_client].physical_damage)
              || (sp->oneSelfFlag[sp->defence_client].special_damage))
             && sp->battlemon[sp->attack_client].moveeffect.disabledMove == 0
             && move_pos != 4 // is a valid move the mon knows
             && sp->battlemon[sp->attack_client].pp[move_pos] != 0 // pp is nonzero
             && sp->current_move_index != 0 // a move has already been used
             && sp->moveTbl[sp->current_move_index].power != 0
             && BattleRand(bw) % 10 < 3)
            {
                sp->waza_work = sp->current_move_index;
                sp->battlemon[sp->attack_client].moveeffect.disabledMove = sp->waza_work;
                sp->battlemon[sp->attack_client].moveeffect.disabledTurns = 4; // cursed body disables for 4 turns every time
                sp->addeffect_type = ADD_EFFECT_ABILITY;
                seq_no[0] = SUB_SEQ_HANDLE_CURSED_BODY;
                ret = TRUE;
            }
            break;
        case ABILITY_DISGUISE:
            if ((sp->battlemon[sp->defence_client].species == SPECIES_MIMIKYU)
             && (sp->battlemon[sp->defence_client].hp)
             && (sp->battlemon[sp->defence_client].form_no == 0)
             && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
             && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
             && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
             && ((sp->waza_status_flag & MOVE_STATUS_FLAG_MISS) == 0) // if move was successful
             && (sp->moveTbl[sp->current_move_index].power) // if move has power
            )
            {
                BattleFormChange(sp->defence_client, 1, bw, sp, TRUE);
                sp->battlerIdTemp = sp->defence_client;
                sp->battlemon[sp->defence_client].form_no = 1;
                seq_no[0] = SUB_SEQ_HANDLE_DISGUISE_ICE_FACE;
                ret = TRUE;
            }
            break;
        case ABILITY_ICE_FACE:
            if ((sp->battlemon[sp->defence_client].species == SPECIES_EISCUE)
             && (sp->battlemon[sp->defence_client].hp)
             && (sp->battlemon[sp->defence_client].form_no == 0)
             && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
             && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
             && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
             && ((sp->waza_status_flag & MOVE_STATUS_FLAG_MISS) == 0) // if move was successful
             && (sp->moveTbl[sp->current_move_index].power != 0)
             && (GetMoveSplit(sp, sp->current_move_index) == SPLIT_PHYSICAL)
            )
            {
                BattleFormChange(sp->defence_client, 1, bw, sp, TRUE);
                sp->battlerIdTemp = sp->defence_client;
                sp->battlemon[sp->defence_client].form_no = 1;
                seq_no[0] = SUB_SEQ_HANDLE_DISGUISE_ICE_FACE;
                ret = TRUE;
            }
            break;
        case ABILITY_THERMAL_EXCHANGE:
            if ((sp->battlemon[sp->defence_client].hp)
                && (sp->battlemon[sp->defence_client].states[STAT_ATTACK] < 12)
                && ((sp->battlemon[sp->defence_client].condition2 & STATUS2_SUBSTITUTE) == 0)
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) ||
                    (sp->oneSelfFlag[sp->defence_client].special_damage)))
            {
                u8 movetype;

                movetype = GetAdjustedMoveType(sp, sp->attack_client, sp->current_move_index); // new normalize checks

                if(movetype == TYPE_FIRE)
                {
                    if(sp->battlemon[sp->defence_client].states[STAT_ATTACK] < 12)
                    {
                        sp->addeffect_param = ADD_STATUS_EFF_BOOST_STATS_ATTACK_UP;
                        sp->addeffect_type = ADD_EFFECT_ABILITY;
                        sp->state_client = sp->defence_client;
                        sp->battlerIdTemp = sp->defence_client;
                        seq_no[0] = SUB_SEQ_BOOST_STATS;
                        ret = TRUE;
                    }
                }
            }
            break;
        case ABILITY_ANGER_SHELL:
            if ((sp->battlemon[sp->defence_client].hp)
                && ((sp->battlemon[sp->defence_client].states[STAT_ATTACK] < 12)
                ||  (sp->battlemon[sp->defence_client].states[STAT_SPATK] < 12)
                ||  (sp->battlemon[sp->defence_client].states[STAT_SPEED] < 12)
                ||  (sp->battlemon[sp->defence_client].states[STAT_DEFENSE] > 0)
                ||  (sp->battlemon[sp->defence_client].states[STAT_SPDEF] > 0))
                && ((sp->waza_status_flag & WAZA_STATUS_FLAG_NO_OUT) == 0)
                && ((sp->server_status_flag & SERVER_STATUS_FLAG_x20) == 0)
                && ((sp->server_status_flag2 & SERVER_STATUS_FLAG2_U_TURN) == 0)
                && ((sp->oneSelfFlag[sp->defence_client].physical_damage) || (sp->oneSelfFlag[sp->defence_client].special_damage))
                // anger shell doesn't activate if the Pokémon gets attacked by a sheer force boosted move
                && !((GetBattlerAbility(sp, sp->attack_client) == ABILITY_SHEER_FORCE) && (sp->battlemon[sp->attack_client].sheer_force_flag == 1))
                // anger shell doesn't activate until the last hit of a multi-hit move
                && (sp->multiHitCount <= 1)
                && (sp->battlemon[sp->defence_client].hp <= (s32)(sp->battlemon[sp->defence_client].maxhp / 2))
                && (
                // checks if the pokémon has gone below half HP from the current damage instance
                // physical_damage and special_damage contain the relevant damage value that was just dealt, but the value is negative
                ((sp->battlemon[sp->defence_client].hp - (sp->oneSelfFlag[sp->defence_client].physical_damage)) > (s32)sp->battlemon[sp->defence_client].maxhp / 2) ||
                ((sp->battlemon[sp->defence_client].hp - (sp->oneSelfFlag[sp->defence_client].special_damage)) > (s32)sp->battlemon[sp->defence_client].maxhp / 2)))
            {
                sp->addeffect_type = ADD_EFFECT_ABILITY;
                sp->state_client = sp->defence_client;
                sp->battlerIdTemp = sp->defence_client;
                seq_no[0] = SUB_SEQ_HANDLE_ANGER_SHELL;
                ret = TRUE;
            }
            break;
        default:
            break;
    }

    return ret;
}
