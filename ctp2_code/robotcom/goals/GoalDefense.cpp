


























































































































































#include "c3.h"
#include "globals.h"
#include "IC3World.h"
#include "DynArr.h"
#include "CivArchive.h"
#include "IMapPointData.h"


#include "IC3GameState.h"
#include "AiMain.h"
#include "AiMap.h"


#include "dr_debug.h"
#include "matrix.h"
#include "Grassfire.h"
#include "Strategic_Map.h"


#include "FzOut.h"
extern double fz_min_force_matching_percent;
extern double fz_max_force_matching_percent;


#include "BSet.h"
#include "ArmyAgent.h"
#include "CityAgent.h"
#include "ForeignAgent.h"
#include "ForeignCity.h"
#include "Foreigner.h"

#include "Squad_Strength.h"


#include "GoalDefense.h"

#include "AILOG.h"

#define LOG_GOAL_DEFENSE true


GoalDefense::GoalDefense()
{
	
	the_class = DEFENSE_GOAL_CLASS;

    m_city_id = new BSetID; 
    m_city_agent = NULL;
	what_goal = GOAL_TYPE_DEFENSE; 
	the_class = DEFENSE_GOAL_CLASS;
}










GoalDefense::GoalDefense
(
	AiMain *init_ai,					
	CityAgent *the_city,				
	MapPointData pos					
) 

: ArmyGoal(init_ai, 900, pos)
{
	
	the_class = MILITARY_GOAL_CLASS;

	
	
	
	
	removal_time = DONT_REMOVE;
	what_goal = GOAL_TYPE_DEFENSE; 
	the_class = DEFENSE_GOAL_CLASS;

	m_city_id = new BSetID;
    m_city_agent = the_city;
    *m_city_id = m_city_agent->GetID(); 

#ifdef _DEBUG
        CityAgent *tmp=NULL; 
        tmp = init_ai->m_city_set->Find(*m_city_id); 
        Assert(tmp); 
#endif 
	
        
    Assert(m_XYpos); 
	the_city->GetXYPos(init_ai, *m_XYpos);
    Assert(m_XYpos); 
} 	

GoalDefense::GoalDefense(AiMain *ai, CivArchive &archive)
{
	
	the_class = MILITARY_GOAL_CLASS;

    m_city_id = new BSetID; 
    m_city_agent = NULL; 
    Serialize(ai, archive); 
}


GoalDefense::~GoalDefense()
{ 
    NDELETE(m_city_id); 
} 

void GoalDefense::Serialize(AiMain *ai, CivArchive &archive)
{
    CHECKSERIALIZE

    ArmyGoal::Serialize(ai, archive); 

    CityAgent *tmp=NULL; 
    if (archive.IsStoring()) { 

        archive.PutUINT32(m_city_id->GetVal()); 
    } else { 
        m_city_id = new BSetID(); 
        *m_city_id = archive.GetUINT32(); 
    }  
}

void GoalDefense::HookUp(AiMain *ai)
{
    Assert(m_city_id); 
    m_city_agent = ai->m_city_set->Find(*m_city_id); 
    Assert(m_city_agent);
	
}

BOOL GoalDefense::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}



GOAL_TYPE GoalDefense::GetType () const
{
	return GOAL_TYPE_DEFENSE; 
}








void GoalDefense::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_DEFENSE"));
#endif DBGAILOG
}












void GoalDefense::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		AILOG((wstr, " Target: (%d, %d, %d)", m_pos->x, m_pos->y, m_pos->z));
#endif DBGAILOG
}
















int GoalDefense::Is_Satisfied
(
	int *excess						
)
{
	if (attacking_squad == NULL) 
		return FALSE;

	
	
	
	if (m_current_needed_strength->IsEnough(m_current_attacking_strength, 999))
		return TRUE;

	return FALSE;
}











































































































































































 







int GoalDefense::Closest_Enemy_Distance(AiMain *ai, sint32 &attack_threat)
{
	
    double distance;                    
    double closest_distance;            
    sint32 i;
    BSetID h_id;						
    ForeignAgent *him= NULL;			
    BSet<ForeignAgent> *vu=NULL;		
	MapPointData his_pos;				
	sint32 tmp_threat;
	
	attack_threat = 0;

    
    if (ai->m_planner->the_strategic_map->Get_Threat(*m_XYpos) < 1.0)
        return(ABSURDLY_BIG_NUMBER);

	
	closest_distance = 2.0 * (ai->m_world->GetYHeight() + 2.0 * ai->m_world->GetXWidth());
    closest_distance *= closest_distance;

    
    for (i=0; i<k_MAX_PLAYERS; i++) { 
        
        if (ai->m_foreigner[i] == FALSE)
            continue;

        
        if (ai->m_foreigner[i]->IsAtHotWar() == FALSE &&
            ai->m_foreigner[i]->IsAtColdWar() == FALSE)
            continue;

        
        vu = ai->m_foreigner[i]->GetVisibleUnits();

        
        for (him = vu->First(h_id); vu->Last(); him =  vu->Next(h_id)) { 
            him->GetXYPos(his_pos);
            distance = ai->m_world->Get_XY_Euclidean_Distance_Squared(*m_XYpos, his_pos);
            if (distance < closest_distance) {
                closest_distance = distance;

				
				tmp_threat = (long) him->Estimate_Army_Strength(ai);
				if (tmp_threat > attack_threat)
					attack_threat = tmp_threat;
            }
        }
    }

    return ((int) (sqrt(closest_distance) + 0.5));
}















double GoalDefense::Compute_Raw_Priority(AiMain *ai)
{
	sint32 city_size;					
	double population_bonus = 0.0;		
	double proximity_bonus = 0.0;		
	double captured_bonus = 0.0;		

#ifdef _DEBUG
    MapPointData *size; 
    size = ai->m_map->GetSize();

    Assert(m_pos); 
    Assert(m_XYpos); 

    Assert(0 <= m_XYpos->x); 
    Assert(m_XYpos->x < size->x*2); 
    Assert(0 <= m_XYpos->y); 
    Assert(m_XYpos->y < size->y); 
    Assert(0 <= m_XYpos->z); 
    Assert(m_XYpos->z < size->z); 

    Assert(0 <= m_pos->x); 
    Assert(m_pos->x < size->x); 
    Assert(0 <= m_pos->y); 
    Assert(m_pos->y < size->y); 
    Assert(0 <= m_pos->z); 
    Assert(m_pos->z < size->z); 
#endif
   
	
	if (m_city_agent == NULL)
		return BAD_UTILITY;

	
	city_size = m_city_agent->GetNumCitizens();

	
	population_bonus = city_size * ai->m_planner->the_AIP.per_capita_modifier;

    
    
    
    
	sint32 attack_threat;
    int pt = 3 * ai->m_planner->the_strategic_map->threat_map_resolution;
    int prox = Closest_Enemy_Distance(ai, attack_threat);
    if (prox < pt) {
        proximity_bonus = (((double)(pt - prox) / (double)pt) * 10.0 *
                           ai->m_planner->the_AIP.threat_proximity_modifier);

		
		
		
		

		
		
		
		
    }

	if (m_city_agent->WasRecentlyCaptured(ai))
		captured_bonus = ai->m_planner->the_AIP.captured_city_modifier;
	
	
	raw_priority = ai->m_planner->the_AIP.defend_priority;
	raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
		 1, 
		 0, 
		 1, 
		 0, 
		 0, 
		 0, 
		 1); 

	raw_priority += proximity_bonus;
	raw_priority += captured_bonus;
	raw_priority += population_bonus;

	
	return raw_priority;
}






void GoalDefense::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army,
					   ArmyAgent *the_transport, SUB_TASK_TYPE sub_task)
{
	if (SUB_TASK_CARGO_TO_BOARD == sub_task)
	{
		the_army->TryToBoardTransport(ai, the_transport);
		return;
	}
	
	
	if (sub_task != SUB_TASK_GOAL)
		return;

	
	if (the_army->GetState() != AGENT_STATE_MARKED_DEAD)
		{
			CityAgent *the_city;

			
			the_army->SetState(AGENT_STATE_AT_GOAL); 

			
			if (the_army->IsInCity(ai, the_city))
				{
					
					Assert(the_city);
					
					sint32 num_defenders = the_city->GetNumDefenders();
					double attack = the_city->GetDefendersAttack();
					double defense = the_city->GetDefendersDefense();
					num_defenders += the_army->GetNUnits();
					attack += the_army->GetAttackStrength();
					defense += the_army->GetDefenseStrength();

					
					
					
					if (defense > m_min_needed_strength->GetDefense() * 1.30 &&
						num_defenders > 1 &&
						the_army->GetNUnits() > 1)
						{
							#ifdef DBGAILOG
							
							if (ai->AI_VERBOSITY >= LOG_DETAILS)
							AILOG((wstr, " City over-defended, breaking up army @ %x",the_army));
							#endif DBGAILOG
							the_army->ActionUngroupArmy(ai);
							
							
						}
						else
							
							the_city->
								ResetDefenseStrength(num_defenders, -1, attack, defense);
				}
			else
				{
					
					
					
				}
		}


	
	
	int excess;
	if (Is_Satisfied(&excess))
		dont_allocate = true;
}










void GoalDefense::Compute_Needed_Troop_Flow(AiMain *ai)
{
	
	sint32 required_defenders;			
	float attack;						
	float min_required_attack;
	float min_required_defense;
	

	
	

	
	Assert(m_city_agent);

	attack = float(m_city_agent->GetDefenseRequired(ai));

	
	double match_ratio = 
		AddMinMaxRandomRange(ai, 
							 ai->m_planner->the_AIP.min_defense_matching_force_ratio,
							 fz_min_force_matching_percent,
							 fz_max_force_matching_percent);

	
	min_required_defense = float(attack * match_ratio);
	min_required_attack = 1.0;

	
	m_min_needed_strength->SetDefense(min_required_defense);

	
	m_current_needed_strength->SetDefense(min_required_defense);

	
	m_min_needed_strength->SetAttack(min_required_attack);

	
	m_current_needed_strength->SetAttack(min_required_attack);

	
	required_defenders =  ai->m_planner->the_AIP.num_city_defenders;

	
	if (required_defenders < 1) 
		required_defenders = 1;

	
	m_min_needed_strength->SetUnitCount(required_defenders);

	
	m_current_needed_strength->SetUnitCount(required_defenders);
}














BOOL GoalDefense::FindPathToTask(AiMain *ai, ArmyAgent *the_army,
     ArmyAgent *the_transport, MapPointData &goal_pos, 
     const SUB_TASK_TYPE sub_task, MapPointData &dest_pos) 
{


     PLAYER_INDEX enemy_owner; 
     sint32 unit_num; 
     sint32 unit_type[k_MAX_ARMY_SIZE];
     sint32 unit_hp[k_MAX_ARMY_SIZE];
     BOOL is_entrenched; 

     BOOL ret = ai->m_world->GetUnitData(&goal_pos, &enemy_owner, &unit_num, unit_type, 
        unit_hp, &is_entrenched);

    double terrain_bonus;
    double fort_bonus;
    double wall_bonus;
    
    MapPointData tmp, good_pos, *size; 
    tmp.z = goal_pos.z; 
    terrain_bonus = -1000.0; 
    double good_terrain = -1000.0;
    BOOL searching = TRUE; 
    sint16 x; 
    sint16 y; 
    MapPointData start_pos; 
    the_army->GetPos(ai, start_pos); 

	 
     
     if (!(start_pos == goal_pos) &&
		 (unit_num + the_army->GetUnitCount()) > k_MAX_ARMY_SIZE) { 
		#ifdef DBGAILOG
		
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, " Not enough room in city, " ));
		#endif DBGAILOG


         size = ai->m_map->GetSize(); 
         
         for (x=-1; x<=1; x++) { 
             for (y=-1; y<=1; y++) { 

                 if ((x == 0) && (y == 0)) continue;  

                 tmp.x = goal_pos.x + x; 
                 tmp.y = goal_pos.y + y; 
                 if (tmp.x < 0) continue; 
                 if (size->x <= tmp.x) continue; 
                 if (tmp.y < 0) continue; 
                 if (size->y <= tmp.y) continue; 

                 if ((start_pos.x != tmp.x) ||
                     (start_pos.y != tmp.y)) { 
                     if (!the_army->CanEnter(ai, tmp)) continue; 

                     
                     if (ai->m_world->GetUnitData(&tmp, &enemy_owner, &unit_num, unit_type, 
                         unit_hp, &is_entrenched)) { 
                         if (enemy_owner != ai->m_my_player_id) continue; 
                         if ((unit_num + the_army->GetUnitCount()) > k_MAX_ARMY_SIZE) continue; 
                     }
                 }
                 
                 ai->m_world->GetTerrainCombatBonus (&tmp, &terrain_bonus, 
                    &fort_bonus, &wall_bonus);
                 if (terrain_bonus > good_terrain) { 
                     good_pos.x = tmp.x; 
                     good_pos.y = tmp.y; 
                     good_pos.z = tmp.z; 
                     good_terrain = terrain_bonus; 
                     searching = FALSE; 
                 }
             }
         } 

         if (searching) { 
	 		#ifdef DBGAILOG
			
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
				AILOG((wstr, " No new target found -- Failing.\n" ));
			#endif DBGAILOG

             return FALSE; 
         }
 		#ifdef DBGAILOG
		
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, " No new target (%d, %d).\n",good_pos.x, good_pos.y));
		#endif DBGAILOG

     } else { 
         good_pos = goal_pos; 
     } 

     
     return ArmyGoal::FindPathToTask(ai, the_army, the_transport, good_pos, 
         sub_task, dest_pos);
}





