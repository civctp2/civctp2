




































































#include "c3.h"
#include "IC3UnitDB.h"
#include "DynArr.h"
#include "CivArchive.h"
#include "IMapPointData.h"


#include "IC3GameState.h"
#include "IC3World.h"
#include "IC3Player.h"

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


#include "GoalSally.h"

#include "AILOG.h"








GoalSally::GoalSally()
{
	
	the_class = MILITARY_GOAL_CLASS;

	m_foreign_player = -1;
	what_goal = GOAL_TYPE_SALLY;
	the_class = MILITARY_GOAL_CLASS;
}













GoalSally::GoalSally
(
	AiMain *init_ai,					
	ForeignAgent *the_target,			
	MapPointData pos					
) 
: ArmyGoal(init_ai, 1000000, pos)
{
	
	the_class = MILITARY_GOAL_CLASS;

	raw_priority = BAD_UTILITY;
	what_goal = GOAL_TYPE_SALLY;
	the_class = MILITARY_GOAL_CLASS;

	
	removal_time = REMOVE_WHEN_SORTED;

	m_target_id = BSetID(the_target->GetID().GetVal());

    m_foreign_player = the_target->GetPlayerIndex(); 

	
	the_target->GetXYPos(*m_XYpos);

	m_ai = init_ai; 
} 	












GoalSally::GoalSally(AiMain *ai, CivArchive &archive)
{
	
	the_class = MILITARY_GOAL_CLASS;

	what_goal = GOAL_TYPE_SALLY; 
	the_class = MILITARY_GOAL_CLASS;

    m_foreign_player=-1;
	raw_priority = BAD_UTILITY;
	m_ai = ai; 

    Serialize(ai, archive); 
}













GoalSally::~GoalSally()
{ 
} 













void GoalSally::Serialize(AiMain *ai,CivArchive &archive)
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

BOOL GoalSally::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}













void GoalSally::HookUp(AiMain *ai)
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
        BOOL COULD_NOT_FIND_SALLY_GOAL_TARGET=0;
        Assert(COULD_NOT_FIND_SALLY_GOAL_TARGET);
    }
}















GOAL_TYPE GoalSally::GetType () const
{
	return what_goal;
}













void GoalSally::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_SALLY"));
#endif DBGAILOG
}














void GoalSally::Compute_Needed_Troop_Flow(AiMain *ai)
{
	
	float attack;						
    sint32 defend_unit_count; 
	float defend;						
    float ranged; 
    sint32 ranged_unit_count;
	float min_required_strength;
	

	




    BOOL ret = ComputeEnemyStrengthAt(ai, *m_pos, attack, defend_unit_count, defend, 
        ranged_unit_count, ranged);

    Assert(ret); 

	
	
	
	
	
	m_min_needed_strength->SetDefense(0.0);
	m_current_needed_strength->SetDefense(0.0);

	
	
	

	
	min_required_strength = float(defend * ai->m_planner->the_AIP.min_attack_matching_force_ratio);
	m_min_needed_strength->SetAttack(min_required_strength);
	m_current_needed_strength->SetAttack(min_required_strength);

    m_min_needed_strength->SetUnitCount(1); 
    m_current_needed_strength->SetUnitCount(1);

    m_min_needed_strength->SetRangeUnitCount(ranged_unit_count); 
    m_current_needed_strength->SetRangeUnitCount(ranged_unit_count); 

    m_min_needed_strength->SetRangeStrength(ranged); 
    m_current_needed_strength->SetRangeStrength(ranged); 

}















double GoalSally::Compute_Matching_Value
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

	
	return (avg_utility);
}

















int GoalSally::Is_Unit_Appropriate
(
	AiMain *ai,							
	Agent * unit_in_question			
)
{

	
	AGENT_STATE agent_state;			
	AGENT_TYPE  agent_type;				
	ArmyAgent * the_army;				
	CityAgent * the_city;				
	

	
	agent_type = unit_in_question->GetType();
	switch (agent_type)
	{
	case AGENT_TYPE_ARMY:
	{
		
		the_army = (ArmyAgent *) unit_in_question;

		
		if (!PretestBid(ai, the_army))
			return FALSE;

        if (the_army->IsWounded()) return false; 

		
		
		
		if (the_army->IsInCity(ai, the_city) &&
			ArmyGoal::PretestUnitTypes(ai, the_army, GOAL_TYPE_DEFENSE))
			{
				sint32 units_in_city = (sint32) the_city->GetGarrisonCount();
				Assert(the_city);
				MapPointData pos;
				the_city->GetPos(pos);
				
				
				if (!(pos == *m_pos))
				{
					if (units_in_city - the_army->GetNUnits() < 
						ai->m_planner->the_AIP.num_city_defenders ||
						units_in_city - the_army->GetNUnits() <
						the_city->GetMinSlaveGarrison())
					return FALSE;
				}
			}

		
		
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
        if (the_city->GetQueueLen() > 0)  
			return true;
		else
			return false;
		break;
	}
	default:
		return false;
	}
}




































BOOL GoalSally::FollowPathToTask(AiMain *ai, ArmyAgent *first_army, 
    ArmyAgent *second_army, const SUB_TASK_TYPE sub_task, 
    MapPointData &dest_pos)
{ 
	Assert(first_army->the_squad);

	if (first_army->the_squad && sub_task == SUB_TASK_GOAL)
		
		first_army->the_squad->GroupWithArmy(ai, first_army);

	
	
	ForeignAgent *target = GetTarget(ai);
	MapPointData XYpos;
	MapPointData pos;
	first_army->GetPos(ai,pos);
	ai->m_player->Norm2XY(pos, XYpos); 
	double threat = ai->m_planner->the_strategic_map->Get_Threat(XYpos);
	if (target->GetAttackStrength() < threat)
		{
			#ifdef DBGAILOG
			
			if (ai->AI_VERBOSITY >= LOG_MEDIUM)
			AILOG(( wstr,  "\nFollowPathToTask: Chicken out, target does not account entire threat.\n"));
			#endif DBGAILOG
			return FALSE;
		}

	return ArmyGoal::FollowPathToTask(ai, first_army, second_army, sub_task, dest_pos);
}






















BOOL GoalSally::PretestBid(AiMain *ai, ArmyAgent *army_agent)
{
	MapPointData pos;
	
	if (army_agent->
		PretestBid(ai, GetType(), *m_pos) <= BAD_UTILITY)
		return FALSE;

	double move_point_cost;
    sint32 rounds; 
    double build_time;
    if (army_agent->
		BidDistance(ai, FALSE, *m_pos, move_point_cost, rounds, build_time) < 0.0) 
		{ 
			
			return FALSE; 
		}

	
	army_agent->GetPos(ai,pos);
	double cur_move_points = 
		army_agent->GetMovePoints();
	double max_move_points =
		army_agent->GetMaxMovePoints();
	double est_rounds =
		move_point_cost / ( max_move_points - cur_move_points);

	
	
	
	if (!ai->m_world->IsCityHere(&pos) ||
		move_point_cost > 0.5 ||
		build_time > 0 )
		return FALSE;

	sint32 unit_type;
	BOOL can_sally = false;

	
    for (int i=0; i < army_agent->GetUnitCount(); i++)
		{ 
			unit_type = army_agent->GetUnitTypeByNumber(i); 

			
			if (ai->m_unitDB->IsNuclearAttack(unit_type))
				return false;

			
			if (ai->m_unitDB->IsSettler(unit_type))
				return false;

			
			can_sally = can_sally || 
				(ai->m_unitDB->GetAttack(unit_type) > 0);
		} 

	
    if (army_agent->GotCargo()) { 
        return false; 
    } 

	return can_sally;
}













double GoalSally::Action_Bid(AiMain *ai, Agent *agent)
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

	
	double bonuses = 0.0;
	sint32 best_attack;
	sint32 my_attack  = army_agent->GetBestAttack(ai);
	if (ai->m_science_agent->GetBestLandAttack(best_attack))
		bonuses += (1.0 - (my_attack / best_attack))
			* ai->m_planner->the_AIP.max_misc_bid_bonus;
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














void GoalSally::Set_Invalid() 
{
	invalid_goal = true;

	
	ForeignAgent *target = GetTarget(m_ai);
	if (target != NULL)
	{
		
		target->RemoveSallyGoal(m_ai, this->GetID());

		
		target = NULL;

	} 
		
	Goal::Set_Invalid();
}
















double GoalSally::Compute_Raw_Priority(AiMain *ai)
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
	
	raw_priority = ai->m_planner->the_AIP.sally_priority;
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












Goal_Result GoalSally::BuildTaskSolution(AiMain *ai, CityAgent *the_city,
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













ForeignAgent *GoalSally::GetTarget(AiMain *ai) 
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





