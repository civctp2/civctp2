
#include "C3.h"

#include "globals.h"
#include "IC3Player.h"
#include "AiMain.h"

#include "CityAgent.h"
#include "OA_NoTrade.h"

#include "Foreigner.h"



Opportunity_Action_Dont_Trade_With_Enemy::Opportunity_Action_Dont_Trade_With_Enemy()
{
}

Opportunity_Action_Dont_Trade_With_Enemy::~Opportunity_Action_Dont_Trade_With_Enemy()
{
}

void Opportunity_Action_Dont_Trade_With_Enemy::Serialize (CivArchive &archive)
{
}

void Opportunity_Action_Dont_Trade_With_Enemy::Execute(AiMain *ai)
{

    BSetID b_id; 
    sint32 player_idx;
    BOOL is_unknown_id;

    sint32 goods_num, goods_idx; 
    
    CityAgent *nth_city=NULL; 
    BOOL found=FALSE; 

    for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) { 

        if (ai->m_foreigner[player_idx]==NULL) continue; 
        if (!ai->m_foreigner[player_idx]->IsAtHotWar()) continue; 

        for (nth_city = ai->m_city_set->First(b_id); ai->m_city_set->Last(); nth_city = ai->m_city_set->Next(b_id)) { 
    
          

           found = FALSE; 
           for (goods_idx=0; goods_idx<goods_num; goods_idx++) { 
               if (the_goods[goods_idx]->GetDestOwner() == player_idx) { 
                   the_goods[goods_idx]->CutRoute(); 
                   found=TRUE; 
               } 
           } 
           
           if (found) { 
               ai->m_player->CancelCityRoutesToPlayer(b_id.GetVal(), &is_unknown_id, player_idx);
               Assert(is_unknown_id == FALSE); 
           }
        }
    }
}

