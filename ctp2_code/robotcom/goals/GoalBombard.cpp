
































































































































































































































#include "c3.h"
#include "DynArr.h"
#include "CivArchive.h"
#include "IMapPointData.h"
#include "IC3Player.h"
#include "IC3World.h"
#include "IC3UnitDB.h"
#include "MoveFlags.h"


#include "IC3GameState.h"
#include "AiMain.h"
#include "AiMap.h"
#include "airndcnt.h"


#include "dr_debug.h"
#include "matrix.h"
#include "Grassfire.h"
#include "Strategic_Map.h"


#include "FzOut.h"


#include "BSet.h"
#include "ArmyAgent.h"
#include "CityAgent.h"
#include "ForeignAgent.h"
#include "ForeignCity.h"
#include "Foreigner.h"
#include "ScienceAgent.h"


#include "GoalBombard.h"

#include "AILOG.h"









GoalBombard::GoalBombard()
{
	
	the_class = MILITARY_GOAL_CLASS;

	m_foreign_player = -1;
	what_goal = GOAL_TYPE_BOMBARD;
	the_class = BOMBARD_GOAL_CLASS;
	removal_time = REMOVE_WHEN_SORTED;
}











GoalBombard::GoalBombard
(
	AiMain *init_ai,					
	ForeignAgent *the_target,			
	MapPointData pos					
) 
: ArmyGoal(init_ai, 1000000, pos)
{
	
	the_class = MILITARY_GOAL_CLASS;

	raw_priority = BAD_UTILITY;
	what_goal = GOAL_TYPE_BOMBARD;
	the_class = BOMBARD_GOAL_CLASS;

	
	removal_time = REMOVE_WHEN_SORTED;

	m_target_id = the_target->GetID();
	m_target_type = AGENT_TYPE_ARMY;

	
	the_target->GetXYPos(*m_XYpos);

    m_foreign_player = the_target->GetPlayerIndex();
	
	m_ai = init_ai; 

	
	m_rallying = TRUE; 
} 	













GoalBombard::GoalBombard
(
	AiMain *init_ai,					
	ForeignCity *the_target,			
	MapPointData pos					
) 
: ArmyGoal(init_ai, 1000000, pos)
{
	
	the_class = MILITARY_GOAL_CLASS;

	raw_priority = BAD_UTILITY;
	what_goal = GOAL_TYPE_BOMBARD;
	the_class = BOMBARD_GOAL_CLASS;

	
	removal_time = REMOVE_WHEN_SORTED;

	
	m_target_id = the_target->GetID();
	m_target_type = AGENT_TYPE_CITY;

	
	the_target->GetXYPos(init_ai, 
						 the_target->GetPlayerIndex(),
						 *m_XYpos);

    m_foreign_player = the_target->GetPlayerIndex();
	
	m_ai = init_ai; 

	
	m_rallying = TRUE; 
} 	












GoalBombard::GoalBombard(AiMain *ai, CivArchive &archive)
{
	
	the_class = MILITARY_GOAL_CLASS;

	what_goal = GOAL_TYPE_BOMBARD; 
	the_class = BOMBARD_GOAL_CLASS;
    m_foreign_player=-1;
	raw_priority = BAD_UTILITY;
	m_ai = ai; 

	
	m_rallying = TRUE; 

    Serialize(ai, archive); 
}













GoalBombard::~GoalBombard()
{ 
} 













void GoalBombard::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE

    ArmyGoal::Serialize(ai, archive); 
	uint32 v;

    if (archive.IsStoring()) { 
         archive << m_foreign_player; 
         archive.PutSINT32(m_target_id.GetVal()); 
         archive.PutSINT32(m_target_type);
    } else { 
        archive >> m_foreign_player; 
        v = archive.GetSINT32(); 
        m_target_id = v;
        v = archive.GetSINT32(); 
        m_target_type = (AGENT_TYPE) v;
    } 
}

BOOL GoalBombard::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}













void GoalBombard::HookUp(AiMain *ai)
{ 
    Assert(0 <= m_foreign_player); 
    Assert(m_foreign_player < k_MAX_PLAYERS); 

    Assert(ai->m_foreigner[m_foreign_player]); 

	
	if (!ai->m_foreigner[m_foreign_player])
		return;

	#ifdef _DEBUG
	
	if (m_target_type == AGENT_TYPE_ARMY)
		{
			BSet<ForeignAgent> *vc = 
				ai->m_foreigner[m_foreign_player]->GetVisibleUnits();
			BSetID h_id; 
			ForeignAgent *him=NULL; 
			for (him = vc->First(h_id); vc->Last(); him =  vc->Next(h_id)) { 
				if (him->GetID() == m_target_id) {  
					
					
					return; 
				}
			}
		}
	
	if (m_target_type == AGENT_TYPE_CITY)
		{
			BSet<ForeignCity> *vc = 
				ai->m_foreigner[m_foreign_player]->GetKnownCities();
			BSetID h_id; 
			ForeignCity *him=NULL; 
			for (him = vc->First(h_id); vc->Last(); him =  vc->Next(h_id)) { 
				if (him->GetID() == m_target_id) {  
					
					
					return; 
				}
			}
		}

    {
        BOOL COULD_NOT_FIND_BOMBARD_GOAL_TARGET=0;
        Assert(COULD_NOT_FIND_BOMBARD_GOAL_TARGET);
    }
	#endif _DEBUG
}















GOAL_TYPE GoalBombard::GetType () const
{
	return GOAL_TYPE_BOMBARD; 
}













void GoalBombard::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_BOMBARD"));
#endif DBGAILOG
}













void GoalBombard::Display_Goal_Details(AiMain *ai)
{
	#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
	{
		if (m_target_type == AGENT_TYPE_CITY)
		{
			AILOG((wstr, " City: (%d, %d, %d)", m_pos->x, m_pos->y, m_pos->z))
		}
		else
		{
			AILOG((wstr, " Army: (%d, %d, %d)", m_pos->x, m_pos->y, m_pos->z))
		}
	}
	#endif DBGAILOG
}














void GoalBombard::CurrentTargetStrength(double &attack, 
										double &defense, 
										double &active_defense,
										double &bombard,
										double &counter_bombard)
{
    PLAYER_INDEX enemy_owner; 
    sint32 unit_type[k_MAX_ARMY_SIZE];
    sint32 unit_hp[k_MAX_ARMY_SIZE];
    BOOL is_entrenched; 
    sint32 unit_idx; 
	uint32 movement_type;
	double ranged;
	uint32 attacking_squad_move_types;
	sint32 defend_unit_count;

	
	attack = 0.0;
	defense = 0.0;
	active_defense = 0.0;
	bombard = 0.0;
	counter_bombard = 0.0;
    
	Assert(m_ai);
	if (!m_ai)
		return; 

	Assert(attacking_squad);
	if (!attacking_squad)
		return; 

	
	attacking_squad_move_types =
		attacking_squad->GetMoveTypes(m_ai);

	
    BOOL get_unit_data_ok = 
		m_ai->m_world->GetUnitData(m_pos, 
								 &enemy_owner, 
								 &defend_unit_count, 
								 unit_type, 
								 unit_hp, 
								 &is_entrenched);

	
	if (!get_unit_data_ok)
		return; 

	
    for (unit_idx=0; unit_idx<defend_unit_count; unit_idx++) 
		{
			
			movement_type = 
				m_ai->m_unitDB->GetMovementType(unit_type[unit_idx]);

			
			defense += 	m_ai->m_unitDB->GetDefense(unit_type[unit_idx]) 
						* unit_hp[unit_idx];

			
			if (movement_type & attacking_squad_move_types)
				{
					attack += 
						m_ai->m_unitDB->GetAttack(unit_type[unit_idx]) 
						* unit_hp[unit_idx];
				}

			
			if ((movement_type & k_BIT_MOVEMENT_TYPE_AIR) &&
				m_ai->m_unitDB->IsAirAttack(unit_type[unit_idx]))
				{
					attack += 
						m_ai->m_unitDB->GetAttack(unit_type[unit_idx]) 
						* unit_hp[unit_idx];
				}

			
			if (m_ai->m_unitDB->CanCounterBombard(unit_type[unit_idx]))
				{
					counter_bombard += ranged;
				}

			
			if (m_ai->m_unitDB->GetActiveDefenseRange(unit_type[unit_idx]))
				{
					
					active_defense += 
						m_ai->m_unitDB->GetAttack(unit_type[unit_idx]);
				}

			
			ranged = m_ai->m_unitDB->GetZBRangeAttack(unit_type[unit_idx]);
			if (ranged > 0.0)
				{
					
					if (m_ai->m_unitDB->IsLandBombard(unit_type[unit_idx]) &&
						(attacking_squad_move_types & 
						 k_BIT_MOVEMENT_TYPE_LAND))
						{
							bombard += ranged;
							continue; 
						}
					
					
					if (m_ai->m_unitDB->IsLandBombard(unit_type[unit_idx]) &&
						(attacking_squad_move_types & 
						 k_BIT_MOVEMENT_TYPE_MOUNTAIN))
						{
							bombard += ranged;
							continue;
						}

					
					if (m_ai->m_unitDB->IsWaterBombard(unit_type[unit_idx]) &&
						(attacking_squad_move_types & 
						 ( k_BIT_MOVEMENT_TYPE_WATER | 
						   k_BIT_MOVEMENT_TYPE_SHALLOW_WATER)))
						{
							bombard += ranged;
							continue;
						}

					
					if (m_ai->m_unitDB->IsAirBombard(unit_type[unit_idx]) &&
						(attacking_squad_move_types & 
						 k_BIT_MOVEMENT_TYPE_AIR))
						{
							bombard += ranged;
							continue;
						}

					
					if (m_ai->m_unitDB->IsSpaceBombard(unit_type[unit_idx]) &&
						(attacking_squad_move_types & 
						 k_BIT_MOVEMENT_TYPE_SPACE))
						{
							bombard += ranged;
							continue;
						}
				} 
		} 

	
	double terrain_bonus;
	double fort_bonus;
	double wall_bonus;
	
	m_ai->m_world->GetTerrainCombatBonus (m_pos, &terrain_bonus, 
		&fort_bonus, &wall_bonus);
	
	
	double entrenchment_bonus; 
	if (is_entrenched) { 
		entrenchment_bonus =  m_ai->m_unitDB->GetEntrenchmentBonus(); 
	} else { 
		entrenchment_bonus = 0.0; 
	}
	
	defense +=
		defense * entrenchment_bonus + 
		defense * terrain_bonus + 
		defense * fort_bonus +
		wall_bonus * defend_unit_count; 
	Assert(defense < 99999.0);
}





















int GoalBombard::Is_Satisfied
(
	int *excess						
)
{
	Assert(attacking_squad);
	if (attacking_squad == NULL) 
		return FALSE;

	
	

	Assert(m_ai);
	if (!m_ai)
		return TRUE; 

	
	double attack;
	double defense;
	double active_defense;
	double bombard;
	double counter_bombard;

	CurrentTargetStrength(attack, defense, active_defense, bombard, counter_bombard);

	
	double min_required_defense = 
		(attack 
		    * m_ai->m_planner->the_AIP.min_defense_matching_force_ratio);

	
	double min_required_active_defense = 
		( min(attack, active_defense) 
			* m_ai->m_planner->the_AIP.min_defense_matching_force_ratio);
	
	
	double min_required_bombard = 
		(bombard 
			* m_ai->m_planner->the_AIP.min_defense_matching_force_ratio);

	
	double min_required_attack = 
		(defense 
			* m_ai->m_planner->the_AIP.min_defense_matching_force_ratio 
			* 0.3);  

	
	uint32 attacking_squad_move_types = attacking_squad->GetMoveTypes(m_ai);
	if (attacking_squad_move_types & k_BIT_MOVEMENT_TYPE_AIR)
	{
		
		if (active_defense || counter_bombard)
		{
			if (m_current_attacking_strength->GetDefense() < 
				 active_defense + counter_bombard)
				 return FALSE;
		}

		
		
		return TRUE;
	}
	else
	{
		
		if (m_current_attacking_strength->GetDefense() < min_required_defense)
			return FALSE;

		
		if ((m_current_attacking_strength->GetRangedStrength() < min_required_bombard) &&
			(m_current_attacking_strength->GetRangedStrength() < min_required_attack))
			return FALSE;
	}
	
	return TRUE;
}













void GoalBombard::Set_Invalid() 
{
	invalid_goal = true;

	
	if (m_target_type == AGENT_TYPE_ARMY)
		{
			
			ForeignAgent *target = GetTarget(m_ai);
			if (target != NULL)
				{
					
					target->RemoveBombardGoal(m_ai, this->GetID());

					
					target = NULL;

				} 
		}
	
	Goal::Set_Invalid();
}














void GoalBombard::Compute_Needed_Troop_Flow(AiMain *ai)
{
	
	BOOL ASSERT_CITY_TARGET_INVALID = 0;
    float  min_required_strength;
	
    
	
	
	min_required_strength = 0.0f;
	m_min_needed_strength->SetDefense(min_required_strength);
	m_current_needed_strength->SetDefense(min_required_strength);

	m_min_needed_strength->SetAttack(min_required_strength);
	m_current_needed_strength->SetAttack(min_required_strength);
	
	m_min_needed_strength->SetUnitCount(1); 
	m_current_needed_strength->SetUnitCount(1);

	m_min_needed_strength->SetRangeUnitCount(0); 
	m_current_needed_strength->SetRangeUnitCount(0); 

	m_min_needed_strength->SetRangeStrength(0); 
	m_current_needed_strength->SetRangeStrength(0);

	return;
}













double GoalBombard::Compute_Matching_Value
(
	AiMain *ai,							
	squad_ptr the_squad				
)
{
	
	double utility;					
	double summed_utility = 0.0;	
	double avg_utility;				
	int unit_count;					
	ArmyAgent *tmpAgent;			
	BOOL same_squad_goal_combo;		
	

	
	
	unit_count = the_squad->GetUnitCount();

	
	if (unit_count <= 0) 
		return BAD_UTILITY;

	
	sint32 found_agents = 0;
	for (int i = 0; i< unit_count; i++)
		{
			tmpAgent = (ArmyAgent *) the_squad->GetUnitByNumber(i);
			utility = Action_Bid(ai, tmpAgent);
			if (utility > BAD_UTILITY + 0.5)
			{
				summed_utility += utility;
				found_agents++;
			}

	} 

	
	if (found_agents > 0)
	{
		
		avg_utility = summed_utility / found_agents;
	}
	else
		
		return BAD_UTILITY;

	
	same_squad_goal_combo = (the_squad->last_goal_type is what_goal);

	
	if (same_squad_goal_combo)
	{
		
		avg_utility += ai->m_planner->the_AIP.persistence_modifier;

	} 

	
	return (avg_utility);
}
















int GoalBombard::Is_Unit_Appropriate
(
	AiMain *ai,							
	Agent * unit_in_question			
)
{

	
	AGENT_STATE agent_state;			
	AGENT_TYPE  agent_type;				
	ArmyAgent * the_army;				
	CityAgent * the_city;				
	

	
	if (!ArmyGoal::Is_Unit_Appropriate(ai, unit_in_question))
		return FALSE;

	
	agent_type = unit_in_question->GetType();
	switch (agent_type)
	{
	case AGENT_TYPE_ARMY:
	{
		
		the_army = (ArmyAgent *) unit_in_question;

		
		if (!PretestBid(ai, the_army))
			return FALSE;

		Assert(attacking_squad);
		if (!attacking_squad)
			return FALSE; 

		
		
		if (attacking_squad->GetUnitCount() > 1 &&
		   (attacking_squad->GetMoveTypes(ai) & the_army->GetMoveIntersection(ai)) 
				== 0x0)
			return FALSE;

		
		
		agent_state = the_army->GetState();
		switch (agent_state) 
		{ 
		case AGENT_STATE_MARKED_SETTLE:
		case AGENT_STATE_MARKED_DEAD:            
		case AGENT_STATE_MARKED_GARRISON:
		case AGENT_STATE_AT_GOAL:
			return FALSE;
		default:
			return TRUE;
		}
		break;
	} 
	break;
	case AGENT_TYPE_CITY:
	{
		
		the_city = (CityAgent *) unit_in_question;
        if (the_city->GetQueueLen() > 0)  
			return TRUE;
		else
			return FALSE;
		break;
	}
	default:
		return FALSE;
	}
}

























BOOL GoalBombard::WithinRange
(
    AiMain *ai, 
	Agent *agent,
	const SUB_TASK_TYPE & sub_task
)
{
	ArmyAgent *army_agent;

	
	switch (agent->GetType())
	{
	case AGENT_TYPE_ARMY:
		
		BOOL close_enough;

		
		army_agent = (ArmyAgent *) agent;
		if (army_agent->GetRemainingPathPoints(ai) <= 1)
			return true;

		
		if (sub_task == SUB_TASK_CARGO_TO_BOARD ||
			sub_task == SUB_TASK_TRANSPORT_TO_BOARD)
			return army_agent->AtEndOfPath(ai);

		close_enough = army_agent->CanBombardTarget(ai, *m_pos);
		
		return close_enough;
		break;
	case AGENT_TYPE_CITY:
		break; 
	case AGENT_TYPE_UNKNOWN:
	default:
		INSANE(DISTANCE_TO_SQUAD_GOAL_UNKNOWN); 
		
		break;
	} 
	return false;
}























void GoalBombard::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army,
								ArmyAgent *the_transport, SUB_TASK_TYPE sub_task)
{
	BOOL succeeded;
	MapPointData dummy_pos; 
	
    
	if (SUB_TASK_CARGO_TO_BOARD == sub_task)
	{
		the_army->TryToBoardTransport(ai, the_transport);
		return;
	}
	
	
	if (sub_task != SUB_TASK_GOAL)
		return;
	succeeded = TRUE;
	sint32 bombard_loop = 0;
	while (succeeded)
		{
			succeeded = the_army->ActionTryToBombard(ai, dummy_pos,this);
			Assert(bombard_loop < 50);
			bombard_loop++;
		}
	
	

	if (the_army->GetState() != AGENT_STATE_MARKED_DEAD)
		{
			
			
			the_army->ActionTryToBackAway(ai);
			the_army->SetState(AGENT_STATE_LOOKING_FOR_WORK); 
		}
	Set_Totally_Complete(succeeded); 
}













BOOL GoalBombard::PretestBid(AiMain *ai, ArmyAgent *the_army)
{
	



	if (the_army->PretestBid(ai, GetType(), *m_pos) == BAD_UTILITY)
			return FALSE;

    sint32 unit_type;
	BOOL can_bombard = false;
	BOOL bomb_land = ai->m_world->IsMoveTypeLand (m_pos);
	BOOL bomb_mountain = ai->m_world->IsMoveTypeMountain (m_pos);
	BOOL bomb_water = ai->m_world->IsMoveTypeWater (m_pos);
	BOOL bomb_space = ai->m_world->IsMoveTypeSpace (m_pos);
	

	
    for (int i=0; i < the_army->GetUnitCount(); i++)
		{ 
			unit_type = the_army->GetUnitTypeByNumber(i); 
			
			if (ai->m_unitDB->IsSettler(unit_type))
				return false;

			
			can_bombard = can_bombard || 
					( bomb_land && ai->m_unitDB->IsLandBombard(unit_type)) ||
					( bomb_mountain && ai->m_unitDB->IsMountainBombard(unit_type)) ||
					( bomb_water && ai->m_unitDB->IsWaterBombard(unit_type)) ||
					( bomb_space && ai->m_unitDB->IsSpaceBombard(unit_type));
		} 

	
    if (the_army->GotCargo()) { 
        return false; 
    } 

	return can_bombard;
}













double GoalBombard::Action_Bid(AiMain *ai, Agent *agent)
{
	ArmyAgent *army_agent = (ArmyAgent *)agent;
	CityAgent *city_agent = (CityAgent *)agent;

	
	if (agent->GetType() == AGENT_TYPE_CITY)
		
		return city_agent->FindBestBuildUtility(ai,this,army_agent);

   	

	
		
	
	if (!PretestBid(ai, army_agent))
		return BAD_UTILITY;

	
	if (army_agent->GetState() == AGENT_STATE_UNBUILT) 
	{ 
        if (army_agent->GetHomeCityAgent()->GetQueueLen() > 0)  
            return BAD_UTILITY; 
    }

	double move_point_cost;
    sint32 rounds; 
    
    
    
    MapPointData army_pos; 
    army_agent->GetPos(ai, army_pos); 
    MapPointData ignore_space = *m_pos;
    ignore_space.z = army_pos.z; 

    double build_time;
    if (army_agent->BidDistance(ai, FALSE, ignore_space, move_point_cost, rounds, build_time) < 0.0) { 
        return BAD_UTILITY; 
    }

	
	if (ai->m_my_player_id == PLAYER_INDEX_VANDALS &&
		rounds > ai->m_planner->the_AIP.wander_rounds_from_target)
		return BAD_UTILITY;

	
	double high_movement_weight = 0.5;
	double high_attack_weight = 0.5;
	double bonuses = 0.0;

	
	sint32 best_attack;
	sint32 my_attack  = army_agent->GetBestAttack(ai);
	if (ai->m_science_agent->GetBestLandAttack(best_attack))
		{
			bonuses += (my_attack / best_attack)  
				* ai->m_planner->the_AIP.max_misc_bid_bonus
				* high_attack_weight;
		}
	else
		; 
	
	
	sint32 best_move;
	double my_move = army_agent->GetBestMove(ai);
	if (ai->m_science_agent->GetBestLandMove(best_move))
		bonuses += (my_move / best_move) 
			* ai->m_planner->the_AIP.max_misc_bid_bonus
			* high_movement_weight;
	else
		; 

	
	double time_term;
    MapPointData dest_pos; 
    army_agent->GetPos(ai, dest_pos); 
	double tile_count = ai->m_map->WrappedDistance(*m_pos, dest_pos);
	time_term = ( rounds * rounds 
		* ai->m_planner->the_AIP.distance_from_unit_priority_modifier )
		- tile_count;
	
	return bonuses + time_term + Compute_Raw_Priority(ai); 
}
















double GoalBombard::Compute_Raw_Priority(AiMain *ai)
{
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
	
	raw_priority = ai->m_planner->the_AIP.bombard_priority;

	raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
		 1, 
		 0, 
		 1, 
		 1, 
		-1, 
		-1, 
		 1); 

	
	return raw_priority;
}












Goal_Result GoalBombard::BuildTaskSolution(AiMain *ai, CityAgent *the_city,
    Plan *the_plan)
{ 

	
	
	
	
    ArmyAgent *unbuilt_army; 
	the_city->FindBestBuildUtility(ai, this, unbuilt_army);

    double utility = the_plan->GetMatchingValue();  

	if (unbuilt_army) {
        sint32 unit_idx = unbuilt_army->GetUnbuiltType(); 
		
		if (the_city->PretestContructUnit(ai, unit_idx))
		{
			the_city->EnqueueArmy(ai, unit_idx, utility);

			
			the_city->current_goal_type = what_goal;
			the_class = BOMBARD_GOAL_CLASS;

			return GOAL_IN_PROGRESS;
		}
		return GOAL_INAPPROPRIATE;
    } else { 
    	
		
		return GOAL_INAPPROPRIATE;
	}

}













ForeignAgent *GoalBombard::GetTarget(AiMain *ai) 
{ 
    if (m_target_id.GetVal()) { 
		
		if (!ai->m_foreigner) return NULL;
		if (!ai->m_foreigner[m_foreign_player]) return NULL;
        BSet<ForeignAgent> *vua = ai->m_foreigner[m_foreign_player]->GetVisibleUnits();
        BSetID h_id; 
        ForeignAgent *his_agent=NULL; 
        for (his_agent = vua->First(h_id); vua->Last(); his_agent =  vua->Next(h_id)) { 
            if (his_agent->GetID().GetVal() == m_target_id.GetVal()) {  
                return his_agent;
            }
		}
	}

	return NULL;
} 





