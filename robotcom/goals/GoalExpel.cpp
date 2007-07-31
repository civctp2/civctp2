









































































































































#include "c3.h"
#include "IC3UnitDB.h"
#include "DynArr.h"
#include "CivArchive.h"
#include "IMapPointData.h"


#include "IC3GameState.h"
#include "IC3World.h"

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
#include "Foreigner.h"
#include "ScienceAgent.h"


#include "GoalExpel.h"

#include "AILOG.h"







BOOL GoalExpel::PretestConstruct(
	AiMain *ai,					
	ForeignAgent * agent
)
{
	
	sint32 unit_type = agent->GetTopUnitType();

	
	if (!ai->m_foreigner[agent->GetPlayerIndex()])
		return FALSE;

	
	if (ai->m_foreigner[agent->GetPlayerIndex()]->IsAtColdWar()) 
		{ 
			BOOL can_be_expelled = ai->m_unitDB->IsCanBeExpelled(unit_type);
			BOOL can_be_sued =  ai->m_unitDB->IsCanBeSued(unit_type);
			return (can_be_expelled || can_be_sued);
		}
	else 
		return FALSE;
}














GoalExpel::GoalExpel()
{
	
	the_class = MILITARY_GOAL_CLASS;

	m_foreign_player = -1;
	what_goal = GOAL_TYPE_EXPEL;
	the_class = MILITARY_GOAL_CLASS;
}













GoalExpel::GoalExpel
(
	AiMain *init_ai,					
	ForeignAgent *the_target,			
	MapPointData pos					
) 
: ArmyGoal(init_ai, 1000000, pos)
{
	
	the_class = MILITARY_GOAL_CLASS;

	raw_priority = BAD_UTILITY;
	what_goal = GOAL_TYPE_EXPEL;
	the_class = MILITARY_GOAL_CLASS;

	
	removal_time = REMOVE_WHEN_SORTED;

	m_target_id = BSetID(the_target->GetID().GetVal());

    m_foreign_player = the_target->GetPlayerIndex(); 

	
	the_target->GetXYPos(*m_XYpos);

	m_ai = init_ai; 
} 	












GoalExpel::GoalExpel(AiMain *ai, CivArchive &archive)
{
	
	the_class = MILITARY_GOAL_CLASS;

	what_goal = GOAL_TYPE_EXPEL; 
	the_class = MILITARY_GOAL_CLASS;

    m_foreign_player=-1;
	raw_priority = BAD_UTILITY;
	m_ai = ai; 

    Serialize(ai, archive); 
}













GoalExpel::~GoalExpel()
{ 
} 













void GoalExpel::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE

    ArmyGoal::Serialize(ai, archive); 
	uint32 v;

    if (archive.IsStoring()) { 
         archive << m_foreign_player; 
         archive.PutSINT32(m_target_id.GetVal()); 
    } else { 
        archive >> m_foreign_player; 
        v = archive.GetSINT32(); 
        m_target_id = BSetID(v);
    } 
}

BOOL GoalExpel::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}













void GoalExpel::HookUp(AiMain *ai)
{ 
    Assert(0 <= m_foreign_player); 
    Assert(m_foreign_player < k_MAX_PLAYERS); 

    Assert(ai->m_foreigner[m_foreign_player]); 

    BSet<ForeignAgent> *vc = ai->m_foreigner[m_foreign_player]->GetVisibleUnits();
    BSetID h_id; 
    ForeignAgent *him=NULL; 
    for (him = vc->First(h_id); vc->Last(); him =  vc->Next(h_id)) { 
        if (him->GetID() == m_target_id) {  
			
            return; 
        }
    }

    {
        BOOL COULD_NOT_FIND_EXPEL_GOAL_TARGET=0;
        Assert(COULD_NOT_FIND_EXPEL_GOAL_TARGET);
    }
}















GOAL_TYPE GoalExpel::GetType () const
{
	return what_goal;
}













void GoalExpel::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_EXPEL"));
#endif DBGAILOG
}














void GoalExpel::Compute_Needed_Troop_Flow(AiMain *ai)
{
	
	
	

	


	return;
}















double GoalExpel::Compute_Matching_Value
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
		
		avg_utility = BAD_UTILITY;

	
	same_squad_goal_combo = (the_squad->last_goal_type is what_goal);

	
	if (same_squad_goal_combo)
	{
		
		avg_utility += ai->m_planner->the_AIP.persistence_modifier;

	} 


	
	return (avg_utility);
}

















int GoalExpel::Is_Unit_Appropriate
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
			return false;

		
		
		agent_state = the_army->GetState();
		switch (agent_state) 
		{ 
		case AGENT_STATE_MARKED_SETTLE:
		case AGENT_STATE_MARKED_DEAD:            
		case AGENT_STATE_MARKED_GARRISON:
		case AGENT_STATE_AT_GOAL:
			return false;
		default:
			return true;
		}
		break;
	} 
	break;
	case AGENT_TYPE_CITY:
	{
		
		the_city = (CityAgent *) unit_in_question;
        if (the_city->GetQueueLen() < 1)  
			return true;
		else
			return false;
		break;
	}
	default:
		return false;
	}
}
























BOOL GoalExpel::WithinRange
(
    AiMain *ai, 
	Agent *agent,
	const SUB_TASK_TYPE & sub_task
)
{

	ArmyAgent *army_agent = (ArmyAgent *) agent;

	
	switch (agent->GetType())
	{
	case AGENT_TYPE_ARMY:
		
		BOOL close_enough;

		
		if (sub_task == SUB_TASK_CARGO_TO_BOARD ||
			sub_task == SUB_TASK_TRANSPORT_TO_BOARD)
			return army_agent->AtEndOfPath(ai);
		
		
		close_enough = (army_agent->GetRemainingPathPoints(ai) <= 1);
		
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























void GoalExpel::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army, 
							   ArmyAgent *the_transport, SUB_TASK_TYPE sub_task)
{
	BOOL succeeded;

	if (SUB_TASK_CARGO_TO_BOARD == sub_task)
	{
		the_army->TryToBoardTransport(ai, the_transport);
		return;
	}

	
	if (sub_task == SUB_TASK_GOAL)
	{
		succeeded = the_army->ActionTryToExpel(ai, *m_pos);

		if (the_army->GetState() != AGENT_STATE_MARKED_DEAD)
			{
				the_army->SetState(AGENT_STATE_LOOKING_FOR_WORK); 
			}
		Set_Totally_Complete(succeeded); 
	}
}













BOOL GoalExpel::PretestBid(AiMain *ai, ArmyAgent *actor)
{
	
	if (actor->PretestBid(ai, GetType(), *m_pos) <= BAD_UTILITY)
			return FALSE;

	sint32 unit_type;
	BOOL can_doit = FALSE;

	
	ForeignAgent *target = GetTarget(ai);
	BOOL can_be_expelled = FALSE;
	BOOL can_be_sued = FALSE;
	if (target != NULL)
		{
			unit_type = target->GetTopUnitType();
			can_be_expelled = ai->m_unitDB->IsCanBeExpelled(unit_type);
			can_be_sued =  ai->m_unitDB->IsCanBeSued(unit_type);
		}
	else
		{
			BOOL NO_TARGET_FOUND_FOR_EXPEL_SUE_GOAL = FALSE;
			Assert(NO_TARGET_FOUND_FOR_EXPEL_SUE_GOAL);
			return FALSE;
		}

	
	Assert(can_be_expelled || can_be_sued);
	if (!can_be_expelled && !can_be_sued)
		return FALSE;

	
    for (int i=0; i < actor->GetUnitCount(); i++)
		{ 
			unit_type = actor->GetUnitTypeByNumber(i); 

			
			if (ai->m_unitDB->IsNuclearAttack(unit_type))
				return false;

			
			if (ai->m_unitDB->IsSettler(unit_type))
				return false;

			
			can_doit = (can_doit || 
						(can_be_expelled && 
						 ai->m_unitDB->GetAttack(unit_type) > 0) ||
						(can_be_sued &&
						 ai->m_unitDB->CanSue(unit_type)));
		} 

	
    if (actor->GotCargo()) { 
        return false; 
    } 

	return can_doit;
}













double GoalExpel::Action_Bid(AiMain *ai, Agent *agent)
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
    double build_time;
    if (army_agent->BidDistance(ai, FALSE, *m_pos, move_point_cost, rounds, build_time) < 0.0) { 
        return BAD_UTILITY; 
    }

	
	if (ai->m_my_player_id == PLAYER_INDEX_VANDALS &&
		rounds > ai->m_planner->the_AIP.wander_rounds_from_target)
		return BAD_UTILITY;

	
	
	

	
	double high_movement_weight = .75;
	double low_defense_weight = .25;
	double bonuses = 0.0;

	
	sint32 best_defense;
	sint32 my_defense  = army_agent->GetBestDefense(ai);
	if (ai->m_science_agent->GetBestLandDefense(best_defense))
		bonuses += (1.0 - (my_defense / best_defense))
			* ai->m_planner->the_AIP.max_misc_bid_bonus
			* low_defense_weight;
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














void GoalExpel::Set_Invalid() 
{
	invalid_goal = true;

	
	ForeignAgent *target = GetTarget(m_ai);
	if (target != NULL)
	{
		
		target->RemoveExpelGoal(m_ai, this->GetID());

		
		target = NULL;

	} 
		
	Goal::Set_Invalid();
}
















double GoalExpel::Compute_Raw_Priority(AiMain *ai)
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
	
	raw_priority = ai->m_planner->the_AIP.expel_priority;
	raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
		-1, 
		-1, 
		 1, 
		 1, 
		-1, 
		 1, 
		 1); 

	
	return raw_priority;
}












Goal_Result GoalExpel::BuildTaskSolution(AiMain *ai, CityAgent *the_city,
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
			return GOAL_IN_PROGRESS;
		}
		return GOAL_INAPPROPRIATE;
    } else { 
    	
		
		return GOAL_INAPPROPRIATE;
	}
}













ForeignAgent *GoalExpel::GetTarget(AiMain *ai) 
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





