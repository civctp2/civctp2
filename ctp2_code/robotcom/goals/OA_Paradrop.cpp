
#include "C3.h"
#include "globals.h"
#include "IC3Player.h"
#include "IC3UnitDB.h"

#include "OA_Paradrop.h"

#include "ArmyAgent.h"
#include "AiMain.h"

#include "Foreigner.h"
#include "ForeignCity.h"
#include "BSet.h"

#include "AiMap.h"

Opportunity_Action_Paradrop::Opportunity_Action_Paradrop ()
{
    m_target_list = new DynamicArray<ParadropTarget>(8);
    m_trooper_list = new DynamicArray<ArmyAgent*>(8); 
}

Opportunity_Action_Paradrop::~Opportunity_Action_Paradrop ()
{
    delete m_target_list; 
    m_target_list = NULL; 
    delete m_trooper_list; 
    m_trooper_list = NULL; 
}


void Opportunity_Action_Paradrop::Serialize (CivArchive &archive)
{
    
    
}

void Opportunity_Action_Paradrop::Execute(AiMain *ai)
{
    if (m_trooper_list->Num() < 1) return; 

    FindTargets(ai); 

    if (m_target_list->Num() < 1) return; 

    AssignParatroopersToTargets(ai); 

    DropTroopers(ai);
}

void Opportunity_Action_Paradrop::FindTargets(AiMain *ai)
{
    sint32 enemy_idx; 

    m_target_list->Clear(); 

    BSet<ForeignCity> * his_cities=NULL; 
    ForeignCity *nth_city=NULL; 
    BSetID h_id; 

    ParadropTarget tmp_target; 

    tmp_target.m_count=0;

    for (enemy_idx=0; enemy_idx<k_MAX_PLAYERS; enemy_idx++) { 
        if (NULL == ai->m_foreigner[enemy_idx]) continue;
        if (!ai->m_foreigner[enemy_idx]->IsAtHotWar()) continue;

        his_cities = ai->m_foreigner[enemy_idx]->GetKnownCities(); 

        for (nth_city = his_cities->First(h_id); his_cities->Last(); nth_city = his_cities->Next(h_id)) { 

            if (0 < nth_city->GetNumDefenders(ai)) continue;
            
            nth_city->GetPos(tmp_target.m_pos); 
            tmp_target.m_value = 100 +  nth_city->GetProduction(ai); 

            m_target_list->InsertFlat(tmp_target);             
        }
    }
}

void Opportunity_Action_Paradrop::AssignParatroopersToTargets(AiMain *ai)
{    
    sint32 trooper_idx, trooper_num; 
    sint32 target_idx, target_num; 
    sint32 d = ai->m_player->GetParadropMaxDistance();

    target_num = m_target_list->Num(); 
    trooper_num = m_trooper_list->Num();
    ParadropTarget *nth_target=NULL; 
    MapPointData army_pos; 
    for (trooper_idx = 0; trooper_idx<trooper_num; trooper_idx++) { 

        if (!m_trooper_list->Get(trooper_idx)->CanParadropNow(ai)) continue; 

        for (target_idx=0; target_idx<target_num; target_idx++) { 

            nth_target = &m_target_list->Access(target_idx);

            if (9 <= nth_target->m_count) continue; 


            m_trooper_list->Get(trooper_idx)->GetPos(ai, army_pos);             

            if (ai->m_map->WrappedDistance(army_pos, nth_target->m_pos) <= d) { 
                nth_target->m_assigned[nth_target->m_count] = m_trooper_list->Get(trooper_idx); 
                nth_target->m_count++; 
            } 
        }
    }
}

void Opportunity_Action_Paradrop::DropTroopers(AiMain *ai)
{
    sint32 target_idx, target_num; 
    sint32 assigned_trooper_idx; 

    target_num = m_target_list->Num();
    ParadropTarget *nth_target=NULL; 
    BSetID troop_id; 

    BOOL is_unknown_id;
    BOOL all_dropped;
    BOOL some_dropped;
    BOOL all_died;
    BOOL revealed_foreign_units;
    BOOL revealed_unexplored;

    for (target_idx=target_num-1; 0 <= target_idx; target_idx--) { 

        nth_target = &m_target_list->Access(target_idx);

        for (assigned_trooper_idx=nth_target->m_count-1; 0 <= assigned_trooper_idx; assigned_trooper_idx--) { 

           if (!nth_target->m_assigned[assigned_trooper_idx]->CanParadropNow(ai)) continue; 

            troop_id = nth_target->m_assigned[assigned_trooper_idx]->GetID(); 
            
            ai->m_player->Paradrop(&is_unknown_id, troop_id.GetVal(), &(nth_target->m_pos), 
                &all_dropped, &some_dropped, &all_died, &revealed_foreign_units, &revealed_unexplored);
        } 
    }
}

void Opportunity_Action_Paradrop::RegisterAddArmy (ArmyAgent *add_me)
{
	if (!add_me) return;

    if (!add_me->IsParatroop()) return; 

    m_trooper_list->InsertFlat(add_me);
}

void Opportunity_Action_Paradrop::RegisterMyRemoveArmy (const ArmyAgent *he_died)
{
    if (!he_died->IsParatroop()) return; 

    sint32 troop_num = m_trooper_list->Num(); 
    sint32 troop_idx; 

    for (troop_idx=0; troop_idx<troop_num; troop_idx++) { 
        if (m_trooper_list->Get(troop_idx) == he_died) { 
            m_trooper_list->DelIndexFlat(troop_idx); 
            return; 
        } 
    } 

    INSANE(OA_PARADROP_ARMY_NOT_FOUND); 
}

void Opportunity_Action_Paradrop::HookParatroops(AiMain *ai)
{ 

    BSetID h_id; 
    ArmyAgent *nth_army; 

    for (nth_army = ai->m_army_set->First(h_id); ai->m_army_set->Last(); nth_army = ai->m_army_set->Next(h_id)) { 
        RegisterAddArmy (nth_army);
    }

} 


