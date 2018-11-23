#include "c3.h"
#include "civarchive.h"
#include "IMapPointData.h"
#include "ic3Rand.h"

#include "Agent.h"
#include "ArmyAgent.h"
#include "CityAgent.h"

#include "ic3GameState.h"
#include "aimain.h"

#include "Goal_Chokepoint.h"

#include "ailog.h"

#include "dr_debug.h"
#include "matrix.h"
#include "grassfire.h"
#include "strategic_map.h"

#include "Agent.h"
#include "ArmyAgent.h"
#include "CityAgent.h"

#include "FzOut.h"
#include "AiMap.h"

Goal_Chokepoint::Goal_Chokepoint()
{
    Assert(m_XYpos);

	the_class = PROTECT_GOAL_CLASS;

	removal_time = REMOVE_WHEN_EXECUTED;
	what_goal = GOAL_TYPE_CHOKEPOINT;
}

Goal_Chokepoint::Goal_Chokepoint
(
	AiMain *init_ai,
	const double v,
	const MapPointData &pos
)
: ArmyGoal(init_ai, v, pos)
{

	the_class = PROTECT_GOAL_CLASS;

    Assert(m_XYpos);

	removal_time = REMOVE_WHEN_EXECUTED;
	what_goal = GOAL_TYPE_CHOKEPOINT;
}

Goal_Chokepoint::Goal_Chokepoint
(
	AiMain *ai,
	CivArchive &archive
)

{

	the_class = PROTECT_GOAL_CLASS;

    Serialize(ai, archive);
    Assert(m_XYpos);

}

Goal_Chokepoint::~Goal_Chokepoint()
{
	return;
}

void Goal_Chokepoint::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE


	ArmyGoal::Serialize(ai, archive);
}

BOOL Goal_Chokepoint::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai);
}

GOAL_TYPE Goal_Chokepoint::GetType () const
{
	return GOAL_TYPE_CHOKEPOINT;
}







void Goal_Chokepoint::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG

    Assert(m_XYpos);
    if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_CHOKEPOINT"));
#endif DBGAILOG
}













void Goal_Chokepoint::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG

    Assert(m_XYpos);
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		AILOG((wstr, " Target: (%d, %d, %d)", m_pos->x, m_pos->y, m_pos->z));
#endif DBGAILOG
}













int Goal_Chokepoint::Get_Totally_Complete(AiMain *ai)
{

	int i;
	int completely_there_count = 0;

    Agent *agent = NULL;
    ArmyAgent *aa=NULL;
	MapPointData army_pos;

    Assert(m_XYpos);

	if (attacking_squad == NULL) return false;

	for (i=0; i < attacking_squad->my_agents.count; i++)
	{

		agent = (Agent *) attacking_squad->my_agents.Return_Data_By_Number(i);

		if (agent->GetType() == AGENT_TYPE_ARMY)
		{

			aa = (ArmyAgent *) agent;

            if (aa->GetState() == AGENT_STATE_MARKED_DEAD) {
                continue;
            }

			aa->GetPos(ai, army_pos);

			if ((m_pos->x == army_pos.x)  &&
				(m_pos->y == army_pos.y)  &&
				(m_pos->z == army_pos.z)  )
			{

				completely_there_count++;

			}

		}

	}

    Set_Totally_Complete(completely_there_count == attacking_squad->my_agents.count);

    Assert(m_XYpos);

	return m_is_completed;
}















double Goal_Chokepoint::Compute_Matching_Value
(
	AiMain *ai,
	squad_ptr the_squad
)
{

	double utility;
	bool same_squad_goal_combo = false;

    Assert(m_XYpos);

	utility = the_squad->Compute_Utility( this )
		+ ai->m_rand->Next((sint32) ai->m_planner->the_AIP.random_modifier);

	same_squad_goal_combo = (the_squad->last_goal_type is what_goal);

	if (same_squad_goal_combo)
	{

		utility += ai->m_planner->the_AIP.persistence_modifier;

	}

    Assert(m_XYpos);

	return ((double) utility);
}
















double Goal_Chokepoint::Compute_Raw_Priority(AiMain *ai)
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


    Assert(m_XYpos);

	raw_priority = m_value + ai->m_planner->the_AIP.chokepoint_priority;
	raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
		 1,
		-1,
		 1,
		 1,
		-1,
		-1,
		 1);

	return raw_priority;
}
















void Goal_Chokepoint::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army)
{
    Assert(m_XYpos);

    the_army->SetState(AGENT_STATE_LOOKING_FOR_WORK);
}

Goal_Result Goal_Chokepoint::CheckCompletion(const sint32 build_count)
{
    Assert(m_XYpos);

    Assert(m_XYpos);
    return GOAL_IN_PROGRESS;
}
