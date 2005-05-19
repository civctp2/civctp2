

























































































































































































































































































































































#include "c3.h"
#include "globals.h"
#include "ic3world.h"
#include "ic3UnitDB.h"

#include "dynarr.h"
#include "civarchive.h"
#include "IMapPointData.h"


#include "ic3GameState.h"
#include "ic3player.h"
#include "ic3Rand.h"

#include "aimain.h"
#include "AiMap.h"
#include "airndcnt.h"
#include "Agreement.h"
#include "Cont.h"


#include "dr_debug.h"
#include "matrix.h"
#include "grassfire.h"
#include "strategic_map.h"


#include "order.h"


#include "FzOut.h"


#include "bset.h"
#include "ArmyAgent.h"
#include "CityAgent.h"
#include "ForeignAgent.h"
#include "Foreigner.h"
#include "scienceagent.h"


#include "GoalMapTarget.h"

#include "ailog.h"







void GoalMapTarget::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE

    ArmyGoal::Serialize(ai, archive); 
	sint32 index, count;
	sint16 v;
	MapPointData point;

	
    if (archive.IsStoring()) 
		{ 
			v = (sint16) what_goal;
			archive << v;
			archive << m_moveType;
			archive << (sint32) m_targets.Num();
			for (index=0; index < m_targets.Num(); index++)
				{
					archive << m_targets[index].x;
					archive << m_targets[index].y;
					archive << m_targets[index].z;
				}
		} 
	else 
		{ 
			archive >> v;
			what_goal = (GOAL_TYPE) v;
			archive >> m_moveType;
			archive >> (sint32) count;
			for (index=0; index < count; index++)
			 {
				 archive >> point.x;
				 archive >> point.y;
				 archive >> point.z;
				 m_targets.InsertFlat(point);
			 } 
			
			if (what_goal != GOAL_TYPE_WANDER)
				ai->m_planner->the_strategic_map->
					SetAreaGoal(point,what_goal,this);
			else
					
					ai->m_wanderGoalTargets->InsertFlat(this);
		}
}

BOOL GoalMapTarget::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}













inline BOOL GoalMapTarget::IsTargetType(
	AiMain *ai,
	MapPointData & pos, 
	const GOAL_TYPE & type
)
{
	BOOL ASSERT_MAP_TARGET_INVALID = 0;
	PLAYER_INDEX src_owner;
	PLAYER_INDEX dst_owner;
	uint32 route_idx;
	uint32 num_routes;
	BOOL check_for_stealth; 

    MapPointData XYpos; 
	ai->m_player->Norm2XY(pos, XYpos); 

	
	
	if (ai->my_player_index == PLAYER_INDEX_VANDALS &&
		type != GOAL_TYPE_PILLAGE &&
		type != GOAL_TYPE_PIRATE)
		return FALSE;

	
	
	if (ai->m_world->IsCellOwned(&pos) &&
		(type == GOAL_TYPE_CHOKEPOINT ||
		 type == GOAL_TYPE_GOODY_HUT))
		{
			
        dst_owner = ai->m_world->GetCellOwner(&pos);
        Assert((0 <= dst_owner) &&
            (dst_owner < k_MAX_PLAYERS));

        if ((ai->m_foreigner[dst_owner] != NULL) && 
            (ai->m_player->HaveActiveAgreement(dst_owner, AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS )))
				
				return FALSE;
            
		}

	switch (type) {
		
		case GOAL_TYPE_CHOKEPOINT:
			
			if (pos.z != GROUND_Z)
				return FALSE;

			if (ai->m_world->IsWater(&pos))
				return FALSE; 

			
			if (ai->m_planner->the_strategic_map->Get_Threat(XYpos) < 1.0) {
				return FALSE; 
			}

			
			if (5.0 < ai->m_planner->the_strategic_map->Get_Distance_To_Our_Empire(XYpos)) {
				return FALSE; 
			}
			
			
			if (ai->m_world->GetIsChokePoint(pos)) 
				{ 
					return TRUE; 
				} 
				
			
			num_routes = ai->m_world->GetNumTradeRoutes(pos);
			for (route_idx = 0; route_idx < num_routes; route_idx++)
				{
					src_owner = 
						ai->m_world->GetTradeRouteSource(pos, route_idx);
					dst_owner =
						ai->m_world->GetTradeRouteDestination(pos,  route_idx);
					if (src_owner == ai->my_player_index ||
						dst_owner == ai->my_player_index)
						return TRUE;
				}

			
			return FALSE; 

			break;









		case GOAL_TYPE_PIRATE:
			
			num_routes =
				ai->m_world->GetNumTradeRoutes(pos);
			
			
			if (num_routes <= 0)
				return FALSE;

			
			for (route_idx = 0; route_idx < num_routes; route_idx++)
				{
					src_owner = ai->m_world->GetTradeRouteSource(pos, route_idx);
					dst_owner = ai->m_world->GetTradeRouteDestination(pos, route_idx);
					
					if (src_owner == ai->my_player_index ||
						dst_owner == ai->my_player_index)
						return FALSE;

					
					if (!ai->m_foreigner[src_owner] ||
						!ai->m_foreigner[dst_owner])
						return FALSE;

					
					if (!ai->m_foreigner[src_owner]->HaveContact() ||
						!ai->m_foreigner[dst_owner]->HaveContact())
						return FALSE;

					
					if (!ai->m_foreigner[src_owner]->IsAtColdWar() ||
						!ai->m_foreigner[dst_owner]->IsAtColdWar())
						return FALSE;

					
					if (ai->m_player->HaveActiveAgreement(src_owner,AGREEMENT_TYPE_NO_PIRACY) ||
						ai->m_player->HaveActiveAgreement(dst_owner,AGREEMENT_TYPE_NO_PIRACY))
						return FALSE;
				}
			return TRUE;
			break;
		case GOAL_TYPE_PILLAGE:
			
			if (ai->m_world->GetNumImprovements(pos) > 0)
				{
					PLAYER_INDEX imp_owner;
					imp_owner = ai->m_world->GetCellOwner(&pos);
                    if ((imp_owner == ai->my_player_index) ||
						imp_owner <0 ||
						k_MAX_PLAYERS <= imp_owner) 
                        return FALSE; 

						Assert(ai->m_foreigner[imp_owner]);
					
						if (ai->m_foreigner[imp_owner])
						{
							
							
							if 	(ai->m_foreigner[imp_owner]->HaveContact() &&
								 ai->m_foreigner[imp_owner]->IsAtHotWar())
								return TRUE;
						}
				}
			return FALSE;
			break;
				
		case GOAL_TYPE_COUNTER_STEALTH:
			
			check_for_stealth = (ai->m_map->GetStealthAttacks().Find(pos)>=0);			   if (check_for_stealth)
				return check_for_stealth;
			break;

		case GOAL_TYPE_BONUS_FOOD:
			
			
			
			return FALSE;
			
			
			break;

		case GOAL_TYPE_GOODY_HUT:
			
			
			return (ai->m_world->IsGoodyHut(pos));
			break;
			
		case GOAL_TYPE_PROBE_WORMHOLE:
			
			return (ai->m_world->IsWormholeVisible(ai->my_player_index));

					default:
			return FALSE;
	} 
	
	return FALSE;
}













void GoalMapTarget::SetupMapTargetGoals(
	AiMain *ai
)
{
	
	

	
	MapPointData *map_size;				
	MapPointData target_pos;			
	GoalMapTarget *tmp_goal = NULL;
	GOAL_TYPE tmp_type;
	PLAYER_INDEX owner;
	uint32 army_id;
	sint32 unit_type;
	sint32 num_units;
	uint32 city_id;
	BOOL unit_visible;
	BOOL city_visible;
	BOOL mine_visible;
    BOOL can_be_expelled; 
	

	
	map_size = ai->m_map->GetSize();

	
	target_pos.z = GROUND_Z;

	
	double density = ai->m_planner->the_AIP.wander_density;
	uint32 desired_count = (uint32) floor(map_size->x * map_size->y * density);
	uint32 count = ai->m_wanderGoalTargets->Num();
	if (ai->my_player_index == PLAYER_INDEX_VANDALS && count < desired_count)
		{
			GoalMapTarget::AddNewWanderGoals(ai, desired_count - count);
		}

	
	
	if (ai->m_world->IsWormholeVisible(ai->my_player_index))
		{
			MapPointData wormhole_pos;
			ai->m_world->GetWormholePos(wormhole_pos);
			
			tmp_goal = 
				new GoalMapTarget(ai, wormhole_pos, GOAL_TYPE_PROBE_WORMHOLE);
		}


	

	
	for (target_pos.y = 0; target_pos.y < map_size->y; target_pos.y++)
	{
		
		for (target_pos.x = 0; target_pos.x < map_size->x; target_pos.x++)
		{
            
            
            if (ai->m_world->IsCityHere(&target_pos) ||
                !ai->m_world->IsExplored(&target_pos))
                continue;

			
			
			
			ai->m_world->GetCellContents(&target_pos, 
				&owner, 
				&army_id,
				&unit_type,
				&num_units,
				&city_id,
				&unit_visible,
				&city_visible,
				&mine_visible, 
                &can_be_expelled);
			
			if (city_id != 0 && owner != ai->my_player_index)
				continue;
			
			
			if (army_id != 0 && owner != ai->my_player_index)
				if (!can_be_expelled)
					continue;

			
			
            
			tmp_type = GOAL_TYPE_MAX;

			
			
			
			
			if (IsTargetType(ai, target_pos,GOAL_TYPE_PIRATE))
				tmp_type = GOAL_TYPE_PIRATE;
			else if (IsTargetType(ai, target_pos,GOAL_TYPE_PILLAGE))
				tmp_type = GOAL_TYPE_PILLAGE;
			else if (IsTargetType(ai, target_pos,GOAL_TYPE_COUNTER_STEALTH))
				tmp_type = GOAL_TYPE_COUNTER_STEALTH;
			
			
			
			else if (IsTargetType(ai, target_pos,GOAL_TYPE_CHOKEPOINT))
				tmp_type = GOAL_TYPE_CHOKEPOINT;
			else if (IsTargetType(ai, target_pos,GOAL_TYPE_GOODY_HUT))
				tmp_type = GOAL_TYPE_GOODY_HUT;

			
			if (tmp_type >= GOAL_TYPE_MAX)
				continue;

			
			tmp_goal = ai->m_planner->the_strategic_map->
					GetAreaGoal(target_pos, tmp_type);
					
			
			if (tmp_goal == NULL)
				tmp_goal = new GoalMapTarget(ai, target_pos, tmp_type);
			else
				tmp_goal->AddTarget(target_pos);
		} 
	} 

	
	ai->m_map->ClearStealthAttacks();
}









void GoalMapTarget::AddNewWanderGoals(AiMain *ai, const int points)
{
	Assert(ai);

	MapPointData pos;
	MapPointData *map_size = ai->m_map->GetSize();
    BOOL is_land;
    sint32 pos_cont; 

	
	

	pos.z = 0; 

	Assert(points < (map_size->x * map_size->y)); 
	GoalMapTarget *goal;
	int misses = 0;
	
	for (int count = 0; count < points && misses < 100*points; )
		{
			pos.x = sint16(ai->m_rand->Next((sint32) map_size->x));
			pos.y = sint16(ai->m_rand->Next((sint32) map_size->y));

			
			

	        sint32 player_id;
	        uint32 a_id;
            sint32 top_unit_type; 
	        sint32 unit_num;
            uint32 c_id;
            BOOL unitVis;
            BOOL cityVis;
            BOOL mineVis;
	        sint32 WANDER_GOAL_hey_theres_is_an_army_here=0; 
            BOOL can_be_expelled; 

            ai->m_world->GetCellContents(&pos, &player_id, 
		        &a_id, &top_unit_type, &unit_num, &c_id, &unitVis, 
                &cityVis, &mineVis, &can_be_expelled);

            if ((a_id != 0) && !can_be_expelled) {  
                if (ai->m_my_player_id != player_id) { 
                    continue;  
                } 
            } else if (c_id != 0) { 
                if (ai->m_my_player_id != player_id) { 
                    continue;  
                } 
            }


			ai->m_continents->GetContinent(ai, pos, pos_cont, is_land);
			if (is_land == TRUE)
				{
					goal = new GoalMapTarget(ai, pos, GOAL_TYPE_WANDER);
					Assert(goal);
					count++;
				}
			else
				
				misses++;
		}

	
	Assert(misses < 100*points);
}



















































































































































































































GoalMapTarget::GoalMapTarget()
{
	
    Init(); 
}













GoalMapTarget::GoalMapTarget
(
	AiMain *ai,							
	const MapPointData &target,			
	const GOAL_TYPE &type			
) 
: ArmyGoal(ai, 1000000, target)
{
	BOOL ASSERT_MAP_TARGET_INVALID = 0;

    Init(); 

	
	what_goal = type;

	switch (what_goal)
		{



			case GOAL_TYPE_PILLAGE:
			case GOAL_TYPE_PIRATE:
			case GOAL_TYPE_COUNTER_STEALTH:
				the_class = MILITARY_GOAL_CLASS;
				break;
			case GOAL_TYPE_BONUS_FOOD:
				the_class = BONUS_FOOD_GOAL_CLASS;
				break;
			case GOAL_TYPE_CHOKEPOINT:
				the_class = PROTECT_GOAL_CLASS;
				break;
			case GOAL_TYPE_WANDER:
				the_class = DEFAULT_GOAL_CLASS;
				break;
			case GOAL_TYPE_TRANSPORT:
				the_class = TRANSPORT_HACK_GOAL_CLASS;
				break;
			case GOAL_TYPE_PROBE_WORMHOLE:
				the_class = WORMHOLE_PROBE_GOAL_CLASS;
				break;
			case GOAL_TYPE_GOODY_HUT:
				the_class = EXPLORE_GOAL_CLASS;
				break;
			default:
				Assert(ASSERT_MAP_TARGET_INVALID);
		}

	
	if (what_goal == GOAL_TYPE_WANDER ||
		what_goal == GOAL_TYPE_TRANSPORT)
		removal_time = REMOVE_WHEN_FULFILLED;
	else
		removal_time = REMOVE_WHEN_SORTED;

	
	m_ai = ai;

	
	AddTarget(target);

	
	ai->m_world->GetMoveType((MapPointData*) &target, &m_moveType);
	
	
	ai->m_planner->Add_Goal(this);






#ifdef _DEBUG
    {

        MapPointData pos = target; 
	    sint32 player_id;
	    uint32 a_id;
        sint32 top_unit_type; 
	    sint32 unit_num;
        uint32 c_id;
        BOOL unitVis;
        BOOL cityVis;
        BOOL mineVis;
	    sint32 GOAL_hey_theres_is_an_army_here=0; 
        BOOL can_be_expelled; 

        ai->m_world->GetCellContents(&pos, &player_id, 
			&a_id, &top_unit_type, &unit_num, &c_id, &unitVis, 
            &cityVis, &mineVis, &can_be_expelled);

        if ((a_id != 0) && !can_be_expelled) {  
            if (ai->m_my_player_id != player_id) { 
                Assert(GOAL_hey_theres_is_an_army_here); 

                
                 ai->m_world->GetCellContents(&pos, &player_id, 
        			&a_id, &top_unit_type, &unit_num, &c_id, &unitVis, &cityVis, &mineVis, 
                    &can_be_expelled);
            } 
        } else if (c_id != 0) { 
            if (ai->m_my_player_id != player_id) { 
				Assert(GOAL_hey_theres_is_an_army_here);

                 
                 ai->m_world->GetCellContents(&pos, &player_id, 
        			&a_id, &top_unit_type, &unit_num, &c_id, &unitVis, &cityVis, &mineVis, 
                    &can_be_expelled);

            } 
        }

    }
#endif _DEBUG











	
	sint32 id = this->GetID();
	Assert(id != UNASSIGNED_ID);

	
	if (what_goal != GOAL_TYPE_WANDER)
		ai->m_planner->the_strategic_map->SetAreaGoal(target,what_goal,this);
	else
		
		ai->m_wanderGoalTargets->InsertFlat(this);
} 	












GoalMapTarget::GoalMapTarget(AiMain *ai, CivArchive &archive)
{
    Init(); 

	Serialize(ai, archive); 

	
	m_ai = ai;
}













inline void GoalMapTarget::Init()
{
	what_goal = GOAL_TYPE_MAX;
    removal_time = REMOVE_WHEN_SORTED; 
	raw_priority = BAD_UTILITY;
	
	
}













GoalMapTarget::~GoalMapTarget()
{ 
	
	sint32 index;	

	if (what_goal != GOAL_TYPE_WANDER)
		{
			
			GetPos(*m_pos);
			m_ai->m_planner->the_strategic_map->
				SetAreaGoal(*m_pos, what_goal, NULL);
		}
	else
		{
			
			index = m_ai->m_wanderGoalTargets->Find(this);
			Assert(index >= 0);
			if (index >= 0) 
				m_ai->m_wanderGoalTargets->DelIndexFlat(index);
		}
} 




 








BOOL GoalMapTarget::operator==(const GoalMapTarget & ref) const
{
	
	return (GetID() == ref.GetID());
}












GoalMapTarget& GoalMapTarget::operator=(const GoalMapTarget &ref)
{
	
	Assert(goal_ID == UNASSIGNED_ID); 

	
	goal_ID = ref.goal_ID;
	m_targets = ref.m_targets;
	m_moveType = ref.m_moveType;
	m_ai = ref.m_ai;

	return *this;
}










void GoalMapTarget::AddTarget(const MapPointData & target)
{
	BOOL ASSERT_MAP_TARGET_INVALID = 0;

	switch (what_goal)
		{
			case GOAL_TYPE_CHOKEPOINT:
			case GOAL_TYPE_BONUS_FOOD:
			case GOAL_TYPE_COUNTER_STEALTH:

			case GOAL_TYPE_PILLAGE:
			case GOAL_TYPE_GOODY_HUT:
			case GOAL_TYPE_PIRATE:
			case GOAL_TYPE_WANDER:
			case GOAL_TYPE_TRANSPORT:
			case GOAL_TYPE_PROBE_WORMHOLE:
				
				m_targets.InsertFlat(target);
				break;
			default:
				Assert(ASSERT_MAP_TARGET_INVALID);
				break;
		}
}













GOAL_TYPE GoalMapTarget::GetType () const
{
	return what_goal;
}
















BOOL GoalMapTarget::GetPos(MapPointData &pos)
{
	
	

	if (m_targets.Num() > 0)
		{
			pos = m_targets[0];
			return TRUE;
		}
	return FALSE;
}














const uint32 & GoalMapTarget::GetMoveType () const
{
	return m_moveType;
}













void GoalMapTarget::Display_Goal_Type(AiMain *ai)
{
	BOOL ASSERT_MAP_TARGET_INVALID = 0;
	char position[20];
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		{
			switch (GetType())
				{
					case GOAL_TYPE_TRANSPORT:
						AILOG(( wstr,  "GOAL_TYPE_TRANSPORT"));
						break;
					case GOAL_TYPE_WANDER:
						AILOG(( wstr,  "GOAL_TYPE_WANDER"));
						break;
					case GOAL_TYPE_CHOKEPOINT:
						AILOG(( wstr,  "GOAL_TYPE_CHOKE_POINT"));
						break;



					case GOAL_TYPE_PILLAGE:
						AILOG(( wstr,  "GOAL_TYPE_PILLAGE"));
						break;
					case GOAL_TYPE_PIRATE:
						AILOG(( wstr,  "GOAL_TYPE_PIRATE"));
						break;
					case GOAL_TYPE_COUNTER_STEALTH:
						AILOG(( wstr,  "GOAL_TYPE_COUNTER_STEALTH"));
						break;
					case GOAL_TYPE_BONUS_FOOD:
						AILOG(( wstr,  "GOAL_TYPE_BONUS_FOOD"));
						break;
					case GOAL_TYPE_GOODY_HUT:
						AILOG(( wstr,  "GOAL_TYPE_GOODY_HUT"));
						break;
					case GOAL_TYPE_PROBE_WORMHOLE:
						AILOG(( wstr,  "GOAL_TYPE_PROBE_WORMHOLE"));
						break;
					default:
						AILOG(( wstr,  "INVALID??"));
						Assert(ASSERT_MAP_TARGET_INVALID);
						break;
				}
				GetPos(*m_pos);
				sprintf(position,"(%d,%d,%d)",m_pos->x,m_pos->y,m_pos->z);
				AILOG(( wstr,  position));
		}
	
}













void GoalMapTarget::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG
	GetPos(*m_pos);
	
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		AILOG((wstr, " Target: (%d, %d, %d)", m_pos->x, m_pos->y, m_pos->z));
#endif DBGAILOG
}













void GoalMapTarget::GetDebugName(char *str)
{
	switch (GetType())
		{
			case GOAL_TYPE_WANDER:
				sprintf(str, "wander");
				break;
			case GOAL_TYPE_TRANSPORT:
				sprintf(str, "transport");
				break;
			case GOAL_TYPE_CHOKEPOINT:
				sprintf(str, "choke");
				break;



			case GOAL_TYPE_PILLAGE:
				sprintf(str, "pillage");
				break;
			case GOAL_TYPE_PIRATE:
				sprintf(str, "pirate");
				break;
			case GOAL_TYPE_COUNTER_STEALTH:
				sprintf(str, "nostealth");
				break;
			case GOAL_TYPE_BONUS_FOOD:
				sprintf(str, "herd");
				break;
			case GOAL_TYPE_GOODY_HUT:
				sprintf(str, "goodies");
				break;
			case GOAL_TYPE_PROBE_WORMHOLE:
				sprintf(str, "probe");
				break;			
			default:
				sprintf(str, "map_target?");
				break;
		}
}














void GoalMapTarget::Compute_Needed_Troop_Flow(AiMain *ai)
{
	
	
	

	
	


	return;
}















double GoalMapTarget::Compute_Matching_Value
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

















int GoalMapTarget::Is_Unit_Appropriate
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

		
		
		agent_state = the_army->GetState();
		switch (agent_state) 
		{ 
		case AGENT_STATE_MARKED_SETTLE:
		case AGENT_STATE_MARKED_DEAD:            
		case AGENT_STATE_MARKED_GARRISON:
			return FALSE;
			break;	
		}

		if (the_army->IsWounded()) return FALSE; 
		 
		
		if (!PretestBid(ai, the_army))
			return FALSE;

		
		
		
		
		if (the_army->IsInCity(ai, the_city) &&
			ArmyGoal::PretestUnitTypes(ai, the_army, GOAL_TYPE_DEFENSE))
			{
				Assert(the_city);
				sint32 units_in_city = (sint32) the_city->GetGarrisonCount();
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

		return TRUE;
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


























 
BOOL GoalMapTarget::FindPathToTask(AiMain *ai, ArmyAgent *the_army,
     ArmyAgent *the_transport, MapPointData &goal_pos, 
     const SUB_TASK_TYPE sub_task, MapPointData &dest_pos) 
{

	
	
	BOOL check_rail_launchers = FALSE;
    sint32 pathtime;
	sint32 max_rounds = 1; 
	dest_pos = goal_pos; 
    sint32 cutoff=200000;
    sint32 nodes_opened; 

	if (what_goal == GOAL_TYPE_COUNTER_STEALTH && 
		sub_task == SUB_TASK_GOAL)
		return the_army->FindPathRoundTrip(ai, goal_pos, 
										   pathtime, max_rounds,
										   check_rail_launchers, 
                                           cutoff, nodes_opened); 
	else
		return ArmyGoal::FindPathToTask(ai,the_army,
										the_transport,
										goal_pos,
										sub_task,
										dest_pos);
} 









BOOL GoalMapTarget::WithinRange
(
    AiMain *ai, 
	Agent *agent,
	const SUB_TASK_TYPE & sub_task
)
{
	BOOL close_enough = FALSE;
	ArmyAgent *army_agent = (ArmyAgent *)agent;

	if (agent->GetType() != AGENT_TYPE_ARMY)
		return FALSE;

	
	if (sub_task == SUB_TASK_CARGO_TO_BOARD ||
		sub_task == SUB_TASK_TRANSPORT_TO_BOARD)
		return army_agent->AtEndOfPath(ai);

	
	
	
	
	
	
	if (GetType() == GOAL_TYPE_COUNTER_STEALTH)
		{
			
			
			return (army_agent->GetRemainingPathPoints(ai)==0);
		}

	
	
	return army_agent->AtEndOfPath(ai);
}























void GoalMapTarget::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army,
								   ArmyAgent *the_transport, SUB_TASK_TYPE sub_task)

{
	BOOL ASSERT_MAP_TARGET_INVALID = 0;
	MapPointData pos;
	BOOL succeeded = FALSE;

	

	if (SUB_TASK_CARGO_TO_BOARD == sub_task)
	{
		the_army->TryToBoardTransport(ai, the_transport);
	}

	
	if (sub_task != SUB_TASK_GOAL)
		return;

	PLAYER_INDEX imp_owner;
    PLAYER_INDEX start_owner; 
    PLAYER_INDEX dest_owner; 
    BOOL do_pirate; 
    sint32 num_routes; 
    sint32 idx_route; 

	
	for (int index=0; index < m_targets.Num(); index++)
		{
			pos = m_targets[index];
			imp_owner = ai->m_world->GetCellOwner(&pos);

			switch (GetType()) {







					case GOAL_TYPE_PILLAGE:
						
						
						if (imp_owner < 0 || 
							imp_owner >= k_MAX_PLAYERS ||
							ai->m_foreigner[imp_owner] == NULL) 
							
							continue;

						
						Assert(ai->m_foreigner[imp_owner]->IsAtHotWar());
						if 	(!ai->m_foreigner[imp_owner]->HaveContact() ||
							 !ai->m_foreigner[imp_owner]->IsAtHotWar())
							succeeded = FALSE;
						else
							succeeded = the_army->ActionTryToPillage(ai, pos);

						break;
					case GOAL_TYPE_PIRATE:
						

                        num_routes =  ai->m_world->GetNumTradeRoutes (pos);
                        do_pirate = (0 < num_routes); 
                        for (idx_route=0; do_pirate && idx_route<num_routes; idx_route++) { 

	                        start_owner =  ai->m_world->GetTradeRouteSource (pos, idx_route);
							Assert(start_owner > 0);
							Assert(start_owner <= k_MAX_PLAYERS);

                            if (start_owner == ai->m_my_player_id)  do_pirate=FALSE;   
                            if (do_pirate && !ai->m_foreigner[start_owner]->IsAtColdWar()) do_pirate=FALSE; 
                            if (do_pirate && !ai->m_foreigner[start_owner]->HaveContact()) do_pirate=FALSE;
                            if (do_pirate &&  ai->m_player->HaveActiveAgreement(start_owner,AGREEMENT_TYPE_NO_PIRACY)) do_pirate=FALSE; 

	                        dest_owner = ai->m_world->GetTradeRouteDestination(pos, idx_route);
							Assert(dest_owner > 0);
							Assert(dest_owner <= k_MAX_PLAYERS);

                            if (dest_owner == ai->m_my_player_id) do_pirate=FALSE;
                            if (do_pirate && !ai->m_foreigner[dest_owner]->IsAtColdWar()) do_pirate=FALSE;
                            if (do_pirate && !ai->m_foreigner[dest_owner]->HaveContact()) do_pirate=FALSE;
                            if (do_pirate &&  ai->m_player->HaveActiveAgreement(dest_owner, AGREEMENT_TYPE_NO_PIRACY)) do_pirate=FALSE; 

                        }

                        if (do_pirate) { 
                            succeeded = the_army->ActionTryToPirate(ai, pos);
                        } else { 
                            succeeded = FALSE; 
                        }

						break;
					case GOAL_TYPE_COUNTER_STEALTH:
						
						succeeded = TRUE;
						break;
					case GOAL_TYPE_BONUS_FOOD:
						
						succeeded = TRUE;
						break;
					case GOAL_TYPE_GOODY_HUT:
						
						succeeded = TRUE;
						break;
					case GOAL_TYPE_CHOKEPOINT:
						
						succeeded = TRUE;
						break;
					case GOAL_TYPE_WANDER:
						
						succeeded = TRUE;
						break;
					case GOAL_TYPE_TRANSPORT:
						
						succeeded = TRUE;
						break;
					case GOAL_TYPE_PROBE_WORMHOLE:
						
						succeeded = TRUE;
						break;
				break;
					default:
						Assert(ASSERT_MAP_TARGET_INVALID);
				}

			
			if (succeeded == TRUE)
				{
					
					m_targets.DelIndexFlat(index);
					
					
					succeeded = (m_targets.Num() == 0);
				}
		}

	
	if (the_army->GetState() != AGENT_STATE_MARKED_DEAD)
	{
		if (GetType() == GOAL_TYPE_BONUS_FOOD ||
			GetType() == GOAL_TYPE_CHOKEPOINT)
			
			the_army->SetState(AGENT_STATE_AT_GOAL); 
		else
			the_army->SetState(AGENT_STATE_LOOKING_FOR_WORK); 
	}
	
	
	Set_Totally_Complete(TRUE); 
}













BOOL GoalMapTarget::PretestBid(AiMain *ai, ArmyAgent *agent)
{
	GetPos(*m_pos);
	BOOL is_land;
	sint32 goal_cont;

	
	if (agent->PretestBid(ai, GetType(), *m_pos) <= BAD_UTILITY)
		return FALSE;
			
	sint32 unit_type;
	BOOL can_doit = FALSE;
	BOOL ASSERT_MAP_TARGET_INVALID = 0;
    BOOL wrong_movetype = FALSE;
	sint32 road_type;

	
	if (GetType() == GOAL_TYPE_PROBE_WORMHOLE)
	{
		can_doit = agent->CanProbeWormhole(ai);
		can_doit = can_doit &&
			(ai->m_player->GetStage() < 0);
		return can_doit;
	}

	
	
	
	

	
    if (agent->GotCargo()) { 
        return FALSE;
    } 

	
	if (GetType() == GOAL_TYPE_CHOKEPOINT &&
		agent->HasNoZOC(ai))
		return FALSE;

	
	ai->m_continents->GetContinent(ai, *m_pos, goal_cont, is_land);
	if ((GetType() == GOAL_TYPE_WANDER) &&
		(is_land) &&
		(agent->GetContinent() != goal_cont) &&
		(!agent->CanMoveWater(ai)))
		return FALSE;

	
	if (GetType() == GOAL_TYPE_WANDER)
		{
			MapPointData dest_pos; 
			agent->GetPos(ai, dest_pos); 
			double tile_count = ai->m_map->WrappedDistance(*m_pos, dest_pos);
			if (tile_count > 20) 
				return FALSE;
		}

	
    for (int i=0; i < agent->GetUnitCount(); i++)
		{ 
			unit_type = agent->GetUnitTypeByNumber(i); 

			
			if (ai->m_unitDB->IsNuclearAttack(unit_type))
				return FALSE;

			
			
			
			

			
			if (ai->m_unitDB->IsSettler(unit_type) &&
				GetType() != GOAL_TYPE_WANDER)
				return FALSE;

			switch (GetType())
				{
					case GOAL_TYPE_WANDER:
						
						can_doit = (ai->my_player_index == 0);
						break;
					case GOAL_TYPE_COUNTER_STEALTH:
						
						can_doit = TRUE;
						break;
					case GOAL_TYPE_GOODY_HUT:
						
						can_doit = can_doit ||
							(!ai->m_unitDB->CanMoveAir(unit_type) &&
							 !ai->m_unitDB->CanMoveSpace(unit_type));
						break;
					case GOAL_TYPE_BONUS_FOOD:
						
						can_doit = can_doit ||
							(ai->m_unitDB->GetBonusFood(unit_type) > 0);
						break;




					case GOAL_TYPE_CHOKEPOINT:
					case GOAL_TYPE_PIRATE:
						can_doit = can_doit || 
							(ai->m_unitDB->GetAttack(unit_type) > 0 &&
							!ai->m_unitDB->CanMoveAir(unit_type) &&
							!ai->m_unitDB->CanMoveSpace(unit_type));
						break;
					case GOAL_TYPE_PILLAGE:
						
						if (ai->m_world->GetRoad(m_pos, &road_type) && 
							ai->m_world->IsMoveTypeMountain(m_pos))
							return FALSE;
						
						
						can_doit = can_doit || 
							(ai->m_unitDB->GetAttack(unit_type) > 0 &&
							!ai->m_unitDB->CanMoveAir(unit_type) &&
							!ai->m_unitDB->CanMoveSpace(unit_type));
						break;
					default:
						Assert(FALSE);

				}
		} 

	return can_doit;
}













double GoalMapTarget::Action_Bid(AiMain *ai, Agent *agent)
{
	ArmyAgent *army_agent = (ArmyAgent *)agent;
	CityAgent *city_agent = (CityAgent *)agent;
	BOOL ASSERT_MAP_TARGET_INVALID = 0;

	
	if (agent->GetType() == AGENT_TYPE_CITY)
		
		return city_agent->FindBestBuildUtility(ai,this,army_agent);
	
   	
	
	
    if (army_agent->IsWounded()) return BAD_UTILITY; 

	if (!PretestBid(ai, army_agent))
		return BAD_UTILITY;

	
	if (GetType() == GOAL_TYPE_TRANSPORT)
		return Compute_Raw_Priority(ai);
	
	
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

	
	if (ai->my_player_index == PLAYER_INDEX_VANDALS &&
		rounds > ai->m_planner->the_AIP.wander_rounds_from_target &&
		GetType() != GOAL_TYPE_WANDER)
		return BAD_UTILITY;
	
	
	double high_movement_weight;
	double low_defense_weight;
	double high_defense_weight;
	double high_attack_weight;
	double bonus = 0.0;
	
	
	switch (GetType())
	{
		case GOAL_TYPE_PILLAGE:
		case GOAL_TYPE_PIRATE:
		case GOAL_TYPE_PROBE_WORMHOLE:
			
			high_movement_weight = 0.75;
			low_defense_weight = 0.25;
			high_defense_weight = 0.0;
			high_attack_weight = 0.0;
			break;
		case GOAL_TYPE_WANDER:

		case GOAL_TYPE_COUNTER_STEALTH:
		case GOAL_TYPE_BONUS_FOOD:
		case GOAL_TYPE_GOODY_HUT:
			
			high_movement_weight = 1.0;
			low_defense_weight = 0.0;
			high_defense_weight = 0.0;
			high_attack_weight = 0.0;
			break;
		case GOAL_TYPE_CHOKEPOINT:
			
			high_movement_weight = 0.0;
			low_defense_weight = 0.0;
			high_defense_weight = 0.75;
			high_attack_weight = 0.25;
			break;
		default:
			Assert(ASSERT_MAP_TARGET_INVALID);
	}
	
	
	sint32 best_defense;
	sint32 my_defense  = army_agent->GetBestDefense(ai);
	if (ai->m_science_agent->GetBestLandDefense(best_defense))
		{
			bonus += (1.0 - (my_defense / best_defense))
				* ai->m_planner->the_AIP.max_misc_bid_bonus
				* low_defense_weight;
		}
	else
		; 

	
	sint32 best_move;
	double my_move = army_agent->GetBestMove(ai);
	if (ai->m_science_agent->GetBestLandMove(best_move))
		bonus += (my_move / best_move) 
			* ai->m_planner->the_AIP.max_misc_bid_bonus
			* high_movement_weight;
	else
		; 

	
	if (ai->m_science_agent->GetBestLandDefense(best_defense))
		{
			bonus += (my_defense / best_defense)  
				* ai->m_planner->the_AIP.max_misc_bid_bonus
				* high_defense_weight;
		}
	else
		; 

	
	sint32 best_attack;
	sint32 my_attack  = army_agent->GetBestAttack(ai);
	if (ai->m_science_agent->GetBestLandAttack(best_attack))
		{
			if(best_attack == 0) {
				
				best_attack = 1;
			}
			bonus += (my_attack / best_attack)  
				* ai->m_planner->the_AIP.max_misc_bid_bonus
				* high_attack_weight;
		}
	else
		; 

	
	double time_term;
    MapPointData dest_pos; 
    army_agent->GetPos(ai, dest_pos); 
	double tile_count = ai->m_map->WrappedDistance(*m_pos, dest_pos);
	time_term = ( rounds * rounds 
		* ai->m_planner->the_AIP.distance_from_unit_priority_modifier )
		- tile_count;

	return bonus + time_term + Compute_Raw_Priority(ai); 
}	




	










double GoalMapTarget::Compute_Raw_Priority(AiMain *ai)
{
	GetPos(*m_pos);
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
	BOOL ASSERT_MAP_TARGET_INVALID = 0;
	sint32 cont_idx;
	BOOL is_land;

	
	
	switch (what_goal)
	{
		
		case GOAL_TYPE_TRANSPORT:
			return 666.0; 
			break;
		case GOAL_TYPE_WANDER:
			
			ai->m_continents->GetContinent(ai, *m_pos, 
				cont_idx, is_land);
			
			
			if (!is_land)
				return BAD_UTILITY;
			
			raw_priority = ai->m_planner->the_AIP.wander_priority * 
				ai->m_continents->GetLandArmies(cont_idx);
			return raw_priority;
			break;
		case GOAL_TYPE_CHOKEPOINT:
			raw_priority = ai->m_planner->the_AIP.chokepoint_priority;
			raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
				 1, 
				-1, 
				 1, 
				 1, 
				-1, 
				 1, 
				 1); 
			break;
		case GOAL_TYPE_PILLAGE:
			raw_priority = ai->m_planner->the_AIP.pillage_priority;
			raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
				-1, 
				-1, 
				-1, 
				 1, 
				 1, 
				-1, 
				 1); 
			break;
		case GOAL_TYPE_PIRATE:
			raw_priority = ai->m_planner->the_AIP.pirate_priority;
			raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
				-1, 
				-1, 
				-1, 
				 1, 
				 1, 
				-1, 
				 1); 
			break;
		case GOAL_TYPE_COUNTER_STEALTH:
			raw_priority = ai->m_planner->the_AIP.counter_stealth_priority;
			raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
				-1, 
				 0, 
				 1, 
				 1, 
				 0, 
				 0, 
				 1); 
			break;
		case GOAL_TYPE_BONUS_FOOD:
			
			raw_priority = ai->m_planner->the_AIP.bonus_food_priority;
			raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
				-1, 
				-1, 
				 1, 
				 1, 
				-1, 
				 1, 
				 1); 
			break;
		case GOAL_TYPE_GOODY_HUT:
			raw_priority = ai->m_planner->the_AIP.goody_hut_priority;
			raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
				-1, 
				-1, 
				 1, 
				 1, 
				-1, 
				-1, 
				 1); 
			break;











		case GOAL_TYPE_PROBE_WORMHOLE:
			raw_priority = ai->m_planner->the_AIP.probe_wormhole_priority;
			raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
				-1, 
				-1, 
				 0, 
				 1, 
				-1, 
				 1, 
				 0); 
			break;
		default:
			Assert(ASSERT_MAP_TARGET_INVALID);
			break;
	}

	
	return raw_priority;
}












Goal_Result GoalMapTarget::BuildTaskSolution(AiMain *ai, CityAgent *the_city,
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





