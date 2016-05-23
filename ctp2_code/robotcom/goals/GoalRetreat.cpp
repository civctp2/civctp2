#include "c3.h"
#include "globals.h"
#include "dynarr.h"
#include "civarchive.h"
#include "IMapPointData.h"

#include "ic3GameState.h"
#include "ic3world.h"
#include "ic3player.h"

#include "aimain.h"
#include "AiMap.h"

#include "dr_debug.h"
#include "matrix.h"
#include "grassfire.h"
#include "strategic_map.h"

#include "FzOut.h"

#include "bset.h"
#include "ArmyAgent.h"
#include "CityAgent.h"
#include "ForeignCity.h"
#include "Foreigner.h"
#include "scienceagent.h"

#include "GoalRetreat.h"

#include "ailog.h"

#define LOG_GOAL_RETREAT true








GoalRetreat::GoalRetreat()
{


	the_class = SEIGE_GOAL_CLASS;

	what_goal = GOAL_TYPE_RETREAT;
}











GoalRetreat::GoalRetreat
(
	AiMain *init_ai,
	CityAgent *the_city,
	MapPointData pos
)
: ArmyGoal(init_ai, 1000000, pos)
{

	the_class = DEFAULT_GOAL_CLASS;

	removal_time = DONT_REMOVE;
	what_goal = GOAL_TYPE_RETREAT;

	m_target_id = BSetID(the_city->GetID().GetVal());

	m_ai = init_ai;
}












GoalRetreat::GoalRetreat(AiMain *ai,CivArchive &archive)
{

	the_class = DEFAULT_GOAL_CLASS;
	removal_time = DONT_REMOVE;

	what_goal = GOAL_TYPE_RETREAT;
	m_ai = ai;

    Serialize(ai, archive);
}













GoalRetreat::~GoalRetreat()
{
}













void GoalRetreat::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE

    ArmyGoal::Serialize(ai, archive);
	uint32 v;

    if (archive.IsStoring()) {
         archive.PutSINT32(m_target_id.GetVal());
    } else {
        v = archive.GetSINT32();
        m_target_id = BSetID(v);
    }
}

BOOL GoalRetreat::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai);
}













void GoalRetreat::HookUp(AiMain *ai)
{
}
















GOAL_TYPE GoalRetreat::GetType () const
{
	return GOAL_TYPE_RETREAT;
}













void GoalRetreat::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG

	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_RETREAT"));
#endif DBGAILOG
}













void GoalRetreat::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG

	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		AILOG((wstr, " Target: (%d, %d, %d)", m_pos->x, m_pos->y, m_pos->z));
#endif DBGAILOG
}













void GoalRetreat::Compute_Needed_Troop_Flow(AiMain *ai)
{
  	m_min_needed_strength->SetDefense(10000000.0);
	m_current_needed_strength->SetDefense(1000000.0);

	return;
}









double GoalRetreat::Compute_Matching_Value
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
















int GoalRetreat::Is_Unit_Appropriate
(
	AiMain *ai,
	Agent * unit_in_question
)
{

	AGENT_STATE agent_state;
	AGENT_TYPE  agent_type;
	ArmyAgent * the_army;


	agent_type = unit_in_question->GetType();

    if (agent_type != AGENT_TYPE_ARMY)
        return(FALSE);

    the_army = (ArmyAgent *) unit_in_question;








    agent_state = the_army->GetState();
    switch (agent_state) {
      case AGENT_STATE_UNBUILT:
      case AGENT_STATE_BUILDING:
      case AGENT_STATE_MARKED_SETTLE:
      case AGENT_STATE_MARKED_DEAD:
      case AGENT_STATE_MARKED_GARRISON:
	  case AGENT_STATE_AT_GOAL:
		  return(FALSE);
      default:
          return(TRUE);
    }

    return(TRUE);
}
































































































double GoalRetreat::Action_Bid(AiMain *ai, Agent *agent)
{

    if (agent->GetType() != AGENT_TYPE_ARMY)
        return(BAD_UTILITY);

	ArmyAgent *army_agent = (ArmyAgent *)agent;
	AGENT_STATE agent_state;

    agent_state = army_agent->GetState();
    switch (agent_state) {
      case AGENT_STATE_UNBUILT:
      case AGENT_STATE_BUILDING:
      case AGENT_STATE_MARKED_SETTLE:
      case AGENT_STATE_MARKED_DEAD:
      case AGENT_STATE_MARKED_GARRISON:
	  case AGENT_STATE_AT_GOAL:
          return(BAD_UTILITY);
    }

    if (!army_agent->IsParatroop()) {
        return BAD_UTILITY;
    }

	if (army_agent->IsStealthy(ai))
		return BAD_UTILITY;

	Assert(0 < ai->m_city_set->GetNum());

    MapPointData army_XYpos;
    army_agent->GetXYPos(ai, army_XYpos);
    double area_threat = ai->m_planner->the_strategic_map->Get_Threat(army_XYpos);

    MapPointData army_pos;
    army_agent->GetPos(ai, army_pos);
    if (army_pos.z != m_pos->z)  {
        return BAD_UTILITY;
    }

	double move_point_cost;
    sint32 rounds;
    double build_time;

    if (army_agent->BidDistance(ai,
								FALSE,
								*m_pos,
								move_point_cost,
								rounds,
								build_time) <	(BAD_UTILITY + 0.5) )
		{
			return BAD_UTILITY;
		}


    double outnumber_me = 2.0;
    double threat_max = 100.0;
    double raw_threat = area_threat -  outnumber_me * army_agent->GetBestDefense(ai);
    double bonuses;
    if (threat_max < raw_threat) {
        bonuses = ai->m_planner->the_AIP.max_misc_bid_bonus;
    } else if (raw_threat < -threat_max) {
        bonuses = -ai->m_planner->the_AIP.max_misc_bid_bonus;
    } else {
        bonuses = ai->m_planner->the_AIP.max_misc_bid_bonus * raw_threat /threat_max;
    }


































    CityAgent *target_city=NULL;
    if (army_agent->IsMigratingParatroop()) {

        target_city = ai->m_city_set->Find(m_target_id);
        Assert(target_city);

        if (target_city->HasAirport()) {
            bonuses += 3.5 * ai->m_planner->the_AIP.max_misc_bid_bonus;
        }
    }
	else

		return BAD_UTILITY;







	MapPointData dest_pos;
    army_agent->GetPos(ai, dest_pos);
	double tile_count = ai->m_map->WrappedDistance(*m_pos, dest_pos);
	double time_term = ( rounds * rounds
		* ai->m_planner->the_AIP.distance_from_unit_priority_modifier )
		- tile_count;

	return Compute_Raw_Priority(ai) + bonuses + time_term;
}














void GoalRetreat::Set_Invalid()
{
	invalid_goal = true;

	CityAgent *target = GetTarget(m_ai);
	if (target != NULL)
	{


	}

	Goal::Set_Invalid();
}
















double GoalRetreat::Compute_Raw_Priority(AiMain *ai)
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

	raw_priority = ai->m_planner->the_AIP.retreat_priority;
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












CityAgent *GoalRetreat::GetTarget(AiMain *ai)
{

	int i;
	Agent *an_agent;
    CityAgent *city_agent;
	AGENT_TYPE  agent_type;

    if (m_target_id.GetVal() == 0)
        return(NULL);

    for (i=0; i < ai->m_planner->my_agents.count; i++) {

        an_agent = (Agent *) ai->m_planner->my_agents.Return_Data_By_Number(i);
        agent_type = an_agent->GetType();
        if (agent_type != AGENT_TYPE_ARMY)
            continue;
        city_agent = (CityAgent *) an_agent;
        if (city_agent->GetID().GetVal() == m_target_id.GetVal())
            return(city_agent);
    }

    return(NULL);
}












void GoalRetreat::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army,
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
			the_army->SetState(AGENT_STATE_AT_GOAL);

	the_army->DisbandObsolete(ai);
}









BOOL GoalRetreat::FindPathToTask(AiMain *ai, ArmyAgent *the_army,
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

     if ((unit_num + the_army->GetUnitCount()) > k_MAX_ARMY_SIZE) {

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
             return FALSE;
         }
     } else {
         good_pos = goal_pos;
     }

     return ArmyGoal::FindPathToTask(ai, the_army, the_transport, good_pos,
         sub_task, dest_pos);

}
