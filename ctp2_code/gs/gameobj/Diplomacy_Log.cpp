

#include "c3.h"
#include "Diplomacy_Log.h"

#include "MapPoint.h"
#include "ArmyData.h"
#include "Order.h"

#include "SelItem.h"
extern SelectedItem *g_selected_item; 




#include "TurnCnt.h"
extern TurnCount *g_turn; 

#include "player.h"
extern Player **g_player; 

#include "AgreementTypes.h"
#include "DiplomaticTypes.h"

Diplomacy_Log::Diplomacy_Log()
{
    strcpy(m_filename,"logs\\diplomacy_log.txt");
    m_player_bit_mask = 0xffffffff;
    FILE *fout = fopen(m_filename, "w"); 
    Assert(fout); 
    if (!fout) return; 

    fprintf (fout, "diplomacy log\n"); 
    fclose (fout); 

    m_regard_stack_idx=0; 
}

Diplomacy_Log::~Diplomacy_Log()
{
}

void Diplomacy_Log::LogAllPlayers()
{
    m_player_bit_mask = 0xffffffff;

}

void Diplomacy_Log::UnlogAllPlayers()
{
    m_player_bit_mask = 0;
}

void Diplomacy_Log::LogPlayer(const sint32 player_idx)
{
    m_player_bit_mask |= (1 << player_idx);

}

void Diplomacy_Log::UnlogPlayer(const sint32 player_idx)
{
    m_player_bit_mask &= ~(1 << player_idx);
}

BOOL Diplomacy_Log::IsPlayerLogged(const sint32 player_idx)
{
    
    if (player_idx == -1) return FALSE;

    Assert(-1 <= player_idx); 
    Assert(player_idx < k_MAX_PLAYERS); 
    if ((player_idx < -1) || (k_MAX_PLAYERS <= player_idx)) return FALSE; 

    return (m_player_bit_mask & (1 << player_idx)) != 0; 
} 

void Diplomacy_Log::BeginRound()
{
    if (m_player_bit_mask == 0)  return; 

    FILE *fout = fopen(m_filename, "a"); 
    Assert(fout); 
    if (!fout) return; 

    fprintf (fout, "***************************ROUND %d ***********************\n", g_turn->GetRound());
    fclose(fout); 
}

void Diplomacy_Log::BeginTurn()
{
    sint32 player_idx = g_selected_item->GetCurPlayer();
    
    if (IsPlayerLogged(player_idx)) { 
        FILE *fout = fopen(m_filename, "a"); 
        Assert(fout); 
        if (!fout) return; 

        
        fprintf (fout, "----------------Begin Turn %d:%d--------------------\n", g_turn->GetRound(), player_idx); 
      
        if (Player::IsThisPlayerARobot(player_idx)) { 

            sint32 i; 
            
            

            fprintf (fout, "Player %d regards---\n", player_idx); 
            
            for (i=0; i<k_MAX_PLAYERS; i++) { 
                if (i == player_idx )continue; 
                if (g_player[i] == NULL) continue; 
				if (g_player[i]->m_isDead) continue; 

                
                
            } 

            fprintf(fout, "\n"); 
        }
        fclose(fout); 
    } 
}

void Diplomacy_Log::EndTurn()
{
    
    sint32 player_idx = g_selected_item->GetCurPlayer();
    if (IsPlayerLogged(player_idx)) { 

        FILE *fout = fopen(m_filename, "a"); 
        Assert(fout); 
        if (!fout) return; 

  
        if (Player::IsThisPlayerARobot(player_idx)) { 

            sint32 i; 
   
            fprintf (fout, "Player %d regards---\n", player_idx); 
            
            for (i=0; i<k_MAX_PLAYERS; i++) { 
                if (i == player_idx )continue; 
                if (g_player[i] == NULL) continue; 
				if (g_player[i]->m_isDead) continue; 

                
                
            } 

            fprintf(fout, "\n"); 

            
            
        }

        fprintf (fout, ".....................End Turn %d:%d...................\n", g_turn->GetRound(), player_idx); 

        fclose(fout); 
    } 
}

void Diplomacy_Log::LogStr(const sint32 player_idx, char *str)
{ 
    if (!IsPlayerLogged(player_idx)) return; 

    FILE *fout= fopen(m_filename, "a");
    Assert(fout); 
    if (!fout) return; 

    fprintf (fout, "%s", str); 
    fclose (fout); 
}

void Diplomacy_Log::LogHostileAction(const UNIT_ORDER_TYPE order_type, const sint32 target_player, 
    const sint32 target_id,  const sint32 owner, const uint32 unit_id, const MapPoint &pos)
{
    if (!IsPlayerLogged(target_player) && !IsPlayerLogged(owner)) return; 

    char order_str[80];
    switch(order_type) { 
    	case UNIT_ORDER_INVESTIGATE_CITY: strcpy (order_str, "UNIT_ORDER_INVESTIGATE_CITY"); break; 
        case UNIT_ORDER_NULLIFY_WALLS: strcpy (order_str, "UNIT_ORDER_NULLIFY_WALLS"); break; 
        case UNIT_ORDER_STEAL_TECHNOLOGY:strcpy (order_str, "UNIT_ORDER_STEAL_TECHNOLOGY"); break; 
        case UNIT_ORDER_ASSASSINATE: strcpy (order_str, "UNIT_ORDER_ASSASSINATE"); break; 
        case UNIT_ORDER_INVESTIGATE_READINESS:strcpy (order_str, "UNIT_ORDER_INVESTIGATE_READINESS"); break; 
        case UNIT_ORDER_FRANCHISE: strcpy (order_str, "UNIT_ORDER_FRANCHISE"); break; 
        case UNIT_ORDER_SUE_FRANCHISE: strcpy (order_str, "UNIT_ORDER_SUE_FRANCHISE"); break; 
        case UNIT_ORDER_CAUSE_UNHAPPINESS: strcpy (order_str, "UNIT_ORDER_CAUSE_UNHAPPINESS"); break; 
        case UNIT_ORDER_PLANT_NUKE: strcpy (order_str, "UNIT_ORDER_PLANT_NUKE"); break; 
        case UNIT_ORDER_SLAVE_RAID: strcpy (order_str, "UNIT_ORDER_SLAVE_RAID"); break; 
        case UNIT_ORDER_UNDERGROUND_RAILWAY: strcpy (order_str, "case UNIT_ORDER_UNDERGROUND_RAILWAY"); break; 
        case UNIT_ORDER_INCITE_UPRISING: strcpy (order_str, "UNIT_ORDER_INCITE_UPRISING"); break; 
        case UNIT_ORDER_BIO_INFECT: strcpy (order_str, "UNIT_ORDER_BIO_INFECT"); break; 
        case UNIT_ORDER_NANO_INFECT: strcpy (order_str, "UNIT_ORDER_NANO_INFECT"); break; 
        case UNIT_ORDER_CONVERT: strcpy (order_str, "UNIT_ORDER_CONVERT"); break; 
        case UNIT_ORDER_INDULGENCE: strcpy (order_str, "UNIT_ORDER_INDULGENCE"); break; 
        case UNIT_ORDER_SOOTHSAY: strcpy (order_str, "UNIT_ORDER_SOOTHSAY"); break; 
        case UNIT_ORDER_INJOIN: strcpy (order_str, "UNIT_ORDER_INJOIN"); break; 
        case UNIT_ORDER_INCITE_REVOLUTION: strcpy (order_str, "UNIT_ORDER_INCITE_REVOLUTION"); break; 
        case UNIT_ORDER_CREATE_PARK: strcpy (order_str, "UNIT_ORDER_CREATE_PARK"); break; 
        case UNIT_ORDER_BOMBARD: strcpy (order_str, "UNIT_ORDER_BOMBARD"); break; 
        case UNIT_ORDER_EXPEL: strcpy (order_str, "UNIT_ORDER_EXPEL"); break; 
        case UNIT_ORDER_ENSLAVE_SETTLER: strcpy (order_str, "UNIT_ORDER_ENSLAVE_SETTLER"); break; 
        case UNIT_ORDER_FINISH_ATTACK: strcpy (order_str, "UNIT_ORDER_FINISH_ATTACK"); break; 
        case UNIT_ORDER_SUE: strcpy (order_str, "UNIT_ORDER_SUE"); break; 
        case UNIT_ORDER_PILLAGE: strcpy (order_str, "UNIT_ORDER_PILLAGE"); break; 
        case UNIT_ORDER_INTERCEPT_TRADE: strcpy (order_str, "UNIT_ORDER_INTERCEPT_TRADE"); break; 
        case UNIT_ORDER_INFORM_AI_CAPTURE_CITY: strcpy (order_str, "UNIT_ORDER_INFORM_AI_CAPTURE_CITY"); break; 
        default:
            { BOOL unknown_order_type=0; 
                Assert(unknown_order_type); 
            }
            break; 
    }

    FILE *fout = fopen(m_filename, "a"); 
    Assert(fout); 
    if (!fout) return; 

    fprintf (fout, "player %d did %s to player %d at %d %d\n", 
        owner, order_str, target_player, pos.x, pos.y); 

    fclose (fout); 
}

void Diplomacy_Log::LogRegard(const sint32 the_regarder, const sint32 the_regardee)
{
     if (!IsPlayerLogged(the_regarder) && !IsPlayerLogged(the_regardee)) return; 

     if (!Player::IsThisPlayerARobot(the_regarder)) return; 


     FILE *fout = fopen(m_filename, "a"); 
     Assert(fout); 
     if (!fout) return; 

     
     
     
     

     fclose (fout); 
}

void Diplomacy_Log::LogMakeAgreement(const sint32 owner, const sint32 recipient, 
     const sint32 thirdParty, const AGREEMENT_TYPE agreement)
{

    if (!IsPlayerLogged(owner) && !IsPlayerLogged(recipient) && !IsPlayerLogged(thirdParty)) return;

    FILE *fout = fopen(m_filename, "a"); 
    Assert(fout); 
    if (!fout) return; 

    char astr[80]; 
    switch (agreement) { 
    case AGREEMENT_TYPE_DEMAND_ADVANCE: strcpy (astr, "AGREEMENT_TYPE_DEMAND_ADVANCE"); break; 
	case AGREEMENT_TYPE_DEMAND_CITY: strcpy (astr, "AGREEMENT_TYPE_DEMAND_CITY");break; 
	case AGREEMENT_TYPE_DEMAND_MAP: strcpy (astr, "AGREEMENT_TYPE_DEMAND_MAP");break; 
	case AGREEMENT_TYPE_DEMAND_GOLD: strcpy (astr, "AGREEMENT_TYPE_DEMAND_GOLD");break; 
	case AGREEMENT_TYPE_DEMAND_STOP_TRADE: strcpy (astr, "AGREEMENT_TYPE_DEMAND_STOP_TRADE");break; 
	case AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS: strcpy (astr, "AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS"); break; 
    case AGREEMENT_TYPE_REDUCE_POLLUTION: strcpy (astr, "AGREEMENT_TYPE_REDUCE_POLLUTION"); break; 
	case AGREEMENT_TYPE_CEASE_FIRE: strcpy (astr, "AGREEMENT_TYPE_CEASE_FIRE");break; 
    case AGREEMENT_TYPE_PACT_CAPTURE_CITY: strcpy (astr, "AGREEMENT_TYPE_PACT_CAPTURE_CITY");break; 
    case AGREEMENT_TYPE_PACT_END_POLLUTION: strcpy (astr, "AGREEMENT_TYPE_PACT_END_POLLUTION");break; 
    case AGREEMENT_TYPE_DEMAND_ATTACK_ENEMY: strcpy (astr, "AGREEMENT_TYPE_DEMAND_ATTACK_ENEMY");break; 
	case AGREEMENT_TYPE_OFFER_ADVANCE: strcpy (astr, "AGREEMENT_TYPE_OFFER_ADVANCE");break; 
	case AGREEMENT_TYPE_OFFER_CITY: strcpy (astr, "AGREEMENT_TYPE_OFFER_CITY");break; 
	case AGREEMENT_TYPE_OFFER_MAP: strcpy (astr, "AGREEMENT_TYPE_OFFER_MAP");break; 
	case AGREEMENT_TYPE_OFFER_GOLD: strcpy (astr, "AGREEMENT_TYPE_OFFER_GOLD");break; 
	case AGREEMENT_TYPE_EXCHANGE_ADVANCE: strcpy (astr, "AGREEMENT_TYPE_EXCHANGE_ADVANCE");break; 
	case AGREEMENT_TYPE_EXCHANGE_CITY: strcpy (astr, "AGREEMENT_TYPE_EXCHANGE_CITY");break; 
	case AGREEMENT_TYPE_EXCHANGE_MAP: strcpy (astr, "AGREEMENT_TYPE_EXCHANGE_MAP");break; 
	case AGREEMENT_TYPE_NO_PIRACY: strcpy (astr, "AGREEMENT_TYPE_NO_PIRACY"); break; 
    default :
        {
        sint32 unknown_agreement_type=0; 
        Assert(unknown_agreement_type); 
        }
        break; 
    } 

    if (-1 == thirdParty) { 
        fprintf (fout, "player %d makes agreement with %d that %s\n", owner, recipient, astr); 
    } else { 
        fprintf (fout, "player %d makes agreement with %d that %s about\n", owner, recipient, astr, thirdParty); 
    }

    fclose(fout); 
}

void Diplomacy_Log::LogBrokenAgreement(const sint32 owner, const sint32 recipient, 
     const sint32 thirdParty, const AGREEMENT_TYPE agreement)
{
    if (!IsPlayerLogged(owner) && !IsPlayerLogged(recipient) && !IsPlayerLogged(thirdParty)) return;

    FILE *fout = fopen(m_filename, "a"); 
    Assert(fout); 
    if (!fout) return; 

    char astr[80]; 
    switch (agreement) { 
    case AGREEMENT_TYPE_DEMAND_ADVANCE: strcpy (astr, "AGREEMENT_TYPE_DEMAND_ADVANCE"); break; 
	case AGREEMENT_TYPE_DEMAND_CITY: strcpy (astr, "AGREEMENT_TYPE_DEMAND_CITY");break; 
	case AGREEMENT_TYPE_DEMAND_MAP: strcpy (astr, "AGREEMENT_TYPE_DEMAND_MAP");break; 
	case AGREEMENT_TYPE_DEMAND_GOLD: strcpy (astr, "AGREEMENT_TYPE_DEMAND_GOLD");break; 
	case AGREEMENT_TYPE_DEMAND_STOP_TRADE: strcpy (astr, "AGREEMENT_TYPE_DEMAND_STOP_TRADE");break; 
	case AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS: strcpy (astr, "AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS"); break; 
    case AGREEMENT_TYPE_REDUCE_POLLUTION: strcpy (astr, "AGREEMENT_TYPE_REDUCE_POLLUTION"); break; 
	case AGREEMENT_TYPE_CEASE_FIRE: strcpy (astr, "AGREEMENT_TYPE_CEASE_FIRE");break; 
    case AGREEMENT_TYPE_PACT_CAPTURE_CITY: strcpy (astr, "AGREEMENT_TYPE_PACT_CAPTURE_CITY");break; 
    case AGREEMENT_TYPE_PACT_END_POLLUTION: strcpy (astr, "AGREEMENT_TYPE_PACT_END_POLLUTION");break; 
    case AGREEMENT_TYPE_DEMAND_ATTACK_ENEMY: strcpy (astr, "AGREEMENT_TYPE_DEMAND_ATTACK_ENEMY");break; 
	case AGREEMENT_TYPE_OFFER_ADVANCE: strcpy (astr, "AGREEMENT_TYPE_OFFER_ADVANCE");break; 
	case AGREEMENT_TYPE_OFFER_CITY: strcpy (astr, "AGREEMENT_TYPE_OFFER_CITY");break; 
	case AGREEMENT_TYPE_OFFER_MAP: strcpy (astr, "AGREEMENT_TYPE_OFFER_MAP");break; 
	case AGREEMENT_TYPE_OFFER_GOLD: strcpy (astr, "AGREEMENT_TYPE_OFFER_GOLD");break; 
	case AGREEMENT_TYPE_EXCHANGE_ADVANCE: strcpy (astr, "AGREEMENT_TYPE_EXCHANGE_ADVANCE");break; 
	case AGREEMENT_TYPE_EXCHANGE_CITY: strcpy (astr, "AGREEMENT_TYPE_EXCHANGE_CITY");break; 
	case AGREEMENT_TYPE_EXCHANGE_MAP: strcpy (astr, "AGREEMENT_TYPE_EXCHANGE_MAP");break; 
	case AGREEMENT_TYPE_NO_PIRACY: strcpy (astr, "AGREEMENT_TYPE_NO_PIRACY"); break; 
    default :
        {
        sint32 unknown_agreement_type=0; 
        Assert(unknown_agreement_type); 
        }
        break; 
    } 

    if (-1 == thirdParty) { 
        fprintf (fout, "sombody? has broken player %d agreement with %d that %s\n", owner, recipient, astr); 
    } else { 
        fprintf (fout, "sombody? has broken player %d agreement with %d that %s about\n", owner, recipient, astr, thirdParty); 
    }

    fclose(fout); 
}


void Diplomacy_Log::Request2String(REQUEST_TYPE request, char astr[80])
{
    switch (request) {
    case REQUEST_TYPE_GREETING: strcpy (astr, "REQUEST_TYPE_GREETING"); break; 
	case REQUEST_TYPE_DEMAND_ADVANCE: strcpy (astr, "REQUEST_TYPE_DEMAND_ADVANCE"); break; 
	case REQUEST_TYPE_DEMAND_CITY: strcpy (astr, "REQUEST_TYPE_DEMAND_CITY"); break; 
	case REQUEST_TYPE_DEMAND_MAP: strcpy (astr, "REQUEST_TYPE_DEMAND_MAP"); break; 
	case REQUEST_TYPE_DEMAND_GOLD: strcpy (astr, "REQUEST_TYPE_DEMAND_GOLD"); break; 
	case REQUEST_TYPE_DEMAND_STOP_TRADE: strcpy (astr, "REQUEST_TYPE_DEMAND_STOP_TRADE"); break; 
	case REQUEST_TYPE_DEMAND_ATTACK_ENEMY: strcpy (astr, "REQUEST_TYPE_DEMAND_ATTACK_ENEMY"); break; 
	case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS: strcpy (astr, "REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS"); break; 
	case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION: strcpy (astr, "REQUEST_TYPE_DEMAND_REDUCE_POLLUTION"); break; 
	case REQUEST_TYPE_OFFER_ADVANCE: strcpy (astr, "REQUEST_TYPE_OFFER_ADVANCE"); break; 
	case REQUEST_TYPE_OFFER_CITY: strcpy (astr, "REQUEST_TYPE_OFFER_CITY"); break; 
	case REQUEST_TYPE_OFFER_MAP: strcpy (astr, "REQUEST_TYPE_OFFER_MAP"); break; 
	case REQUEST_TYPE_OFFER_GOLD: strcpy (astr, "REQUEST_TYPE_OFFER_GOLD"); break; 
	case REQUEST_TYPE_OFFER_CEASE_FIRE: strcpy (astr, "REQUEST_TYPE_OFFER_CEASE_FIRE"); break; 
	case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE: strcpy (astr, "REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE"); break; 
	case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY: strcpy (astr, "REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY"); break; 
    case REQUEST_TYPE_OFFER_PACT_END_POLLUTION: strcpy (astr, "REQUEST_TYPE_OFFER_PACT_END_POLLUTION"); break; 
	case REQUEST_TYPE_EXCHANGE_ADVANCE: strcpy (astr, "REQUEST_TYPE_EXCHANGE_ADVANCE"); break; 
    case REQUEST_TYPE_EXCHANGE_CITY: strcpy (astr, "REQUEST_TYPE_EXCHANGE_CITY"); break; 
    case REQUEST_TYPE_EXCHANGE_MAP: strcpy (astr, "REQUEST_TYPE_EXCHANGE_MAP"); break; 
    case REQUEST_TYPE_DEMAND_NO_PIRACY: strcpy (astr, "REQUEST_TYPE_DEMAND_NO_PIRACY"); break; 
    default:
        { 
            sint32 unknown_request=0; 
            Assert(unknown_request); 
        } 
    } 
}

void Diplomacy_Log::LogRequestCreated(const sint32 owner, const sint32 recipient, 
    const REQUEST_TYPE request)
{
    if (!IsPlayerLogged(owner) && !IsPlayerLogged(recipient)) return;     

    FILE *fout = fopen(m_filename, "a"); 
    Assert(fout); 
    if (!fout) return; 
    
    char astr[80];
    Request2String(request, astr); 

    fprintf (fout, "player %d requests that player %d that %s\n", owner, recipient, astr); 

    fclose (fout); 
}

void Diplomacy_Log::LogTone(const sint32 sender, const sint32 reciever, const ATTITUDE_TYPE attitude)
{

    if (!IsPlayerLogged(sender) && !IsPlayerLogged(reciever)) return; 

    char astr[80]; 

    switch (attitude) { 
    case ATTITUDE_TYPE_STRONG_HOSTILE: strcpy (astr, "ATTITUDE_TYPE_STRONG_HOSTILE"); break;
	case ATTITUDE_TYPE_WEAK_HOSTILE:  strcpy (astr, "ATTITUDE_TYPE_WEAK_HOSTILE"); break;
	case ATTITUDE_TYPE_NEUTRAL:  strcpy (astr, "ATTITUDE_TYPE_NEUTRAL"); break;
	case ATTITUDE_TYPE_WEAK_FRIENDLY:  strcpy (astr, "ATTITUDE_TYPE_WEAK_FRIENDLY"); break;
	case ATTITUDE_TYPE_STRONG_FRIENDLY:  strcpy (astr, "ATTITUDE_TYPE_STRONG_FRIENDLY"); break;
    default:
        { 
            sint32 unknown_attitude=0; 
            Assert(unknown_attitude); 
        }
    } 

    
    FILE *fout = fopen(m_filename, "a"); 
    Assert(fout); 
    if (!fout) return; 

    fprintf (fout, "player's %d tone to player %d is %s\n", sender, reciever, astr); 

    fclose (fout); 

}

void Diplomacy_Log::LogEnact(const sint32 sender, const sint32 reciever, const REQUEST_TYPE request)
{

    if (!IsPlayerLogged(sender) && !IsPlayerLogged(reciever)) return; 

    char astr[80]; 

    FILE *fout = fopen(m_filename, "a"); 
    Assert(fout); 
    if (!fout) return; 

    Request2String(request, astr); 
    
    fprintf (fout, "player's %d accepts player %d request of %s\n", reciever, sender, astr); 

    fclose (fout); 
}


void Diplomacy_Log::LogReject(const sint32 sender, const sint32 reciever, const REQUEST_TYPE request)
{

    if (!IsPlayerLogged(sender) && !IsPlayerLogged(reciever)) return; 

    char astr[80]; 

    FILE *fout = fopen(m_filename, "a"); 
    Assert(fout); 
    if (!fout) return; 

    Request2String(request, astr); 
    
    fprintf (fout, "player's %d rejects player %d request of %s\n", reciever, sender, astr); 

    fclose (fout); 

}


void Diplomacy_Log::PushRegardReqest(const sint32 regarder, const sint32 regardee)
{
    
    

    if (k_REGARD_STACK_MAX <= m_regard_stack_idx)
        return; 

    m_regard_stack[m_regard_stack_idx][0] = regarder; 
    m_regard_stack[m_regard_stack_idx][1] = regardee; 
    m_regard_stack_idx++;
}

void Diplomacy_Log::PopRegardRequest()
{
    
    if (m_regard_stack_idx <= 0)
        return; 

    m_regard_stack_idx--;
    LogRegard(m_regard_stack[m_regard_stack_idx][0], 
        m_regard_stack[m_regard_stack_idx][1]); 

}



