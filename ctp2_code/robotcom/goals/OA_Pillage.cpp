
#include "c3.h"

#include "globals.h"
#include "C3World.h"
#include "C3Rand.h"
#include "C3Player.h"
#include "AgreementTypes.h"
#include "MapPoint.h"

#include "OA_Pillage.h"

#include "AiMain.h"
#include "BSet.h"
#include "ArmyAgent.h"
#include "Foreigner.h"


extern double fz_auto_pillage_chance; 

Opportunity_Action_Pillage::Opportunity_Action_Pillage()
{
}

Opportunity_Action_Pillage::~Opportunity_Action_Pillage ()
{
}

void Opportunity_Action_Pillage::Serialize (CivArchive &archive)
{
}

void Opportunity_Action_Pillage::Execute(AiMain *ai)
{

    ArmyAgent *nth_army=NULL; 
    BSetID b_id;
    MapPointData pos; 
    PLAYER_INDEX start_owner; 
    PLAYER_INDEX dest_owner; 
    BOOL do_pirate; 
    BOOL is_unknown_id;
    double cur_movement;
    sint32 num_routes; 
    sint32 idx_route; 

    for (nth_army = ai->m_army_set->First(b_id); ai->m_army_set->Last(); nth_army = ai->m_army_set->Next(b_id)) { 

        if (fz_auto_pillage_chance < ai->m_rand->Next(100)) continue; 

        ai->m_player->GetArmyCurMinMovementPoints(nth_army->m_id.GetVal(), &is_unknown_id, &cur_movement); 

        if (cur_movement < 100) continue; 

        nth_army->GetPos(ai, pos); 
        
        num_routes =  ai->m_world->GetNumTradeRoutes (pos);
        do_pirate = (0 < num_routes); 
        for (idx_route=0; do_pirate && idx_route<num_routes; idx_route++) { 
	        start_owner =  ai->m_world->GetTradeRouteSource (pos, idx_route);
            Assert(0 <= start_owner); 
            Assert(start_owner <= 32); 
            if (start_owner == ai->m_my_player_id)  do_pirate = FALSE; 
            if (do_pirate && !ai->m_foreigner[start_owner]->IsAtColdWar()) do_pirate = FALSE;
            if (do_pirate && !ai->m_foreigner[start_owner]->HaveContact()) do_pirate = FALSE;
            if (do_pirate &&  ai->m_player->HaveActiveAgreement(start_owner, AGREEMENT_TYPE_NO_PIRACY)) do_pirate=FALSE; 

	        dest_owner = ai->m_world->GetTradeRouteDestination(pos, idx_route);
            Assert(0 <= dest_owner); 
            Assert(dest_owner <= 32); 
            if (dest_owner == ai->m_my_player_id) do_pirate = FALSE;
            if (do_pirate && !ai->m_foreigner[dest_owner]->IsAtColdWar()) do_pirate = FALSE;
            if (do_pirate && !ai->m_foreigner[dest_owner]->HaveContact()) do_pirate = FALSE;
            if (do_pirate &&  ai->m_player->HaveActiveAgreement(dest_owner, AGREEMENT_TYPE_NO_PIRACY)) do_pirate=FALSE; 

        }

        if (do_pirate) { 
            if (ai->m_player->CanPirate(nth_army->m_id.GetVal(), &is_unknown_id, &pos)) { 
                ai->m_player->Pirate(nth_army->m_id.GetVal(), &is_unknown_id, &pos);
            }
        }
    

        if (ai->m_world->GetNumImprovements(pos) > 0) {
			start_owner = ai->m_world->GetCellOwner(&pos);
        
            if ((0 <= start_owner) && (start_owner <= k_MAX_PLAYERS)) { 
                if (ai->m_foreigner[start_owner]) { 
                    if (start_owner != ai->m_my_player_id) { 
                        if (ai->m_foreigner[start_owner]->IsAtHotWar()) { 
                            if (ai->m_player->CanPillage(nth_army->m_id.GetVal(), &is_unknown_id, &pos)) { 
                                ai->m_player->Pillage(nth_army->m_id.GetVal(), &is_unknown_id, &pos);
                            }
                        } 
                    } 
                }
            }
        } 
    } 
}


