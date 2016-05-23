#include "c3.h"
#include "dynarr.h"
#include "civarchive.h"
#include "IMapPointData.h"
#include "ic3BlgDB.h"

#include "ic3GameState.h"
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

#include "Goal_Build_List.h"

#include "ailog.h"

#define LOG_GOAL_BUILD_LIST true

















Goal_Build_List::Goal_Build_List()
{

	the_class = CITY_GOAL_CLASS;

	what_goal = GOAL_TYPE_BUILD_LIST;
    which_building = -1;
	m_isWonder = FALSE;
	m_isEndGameObject = FALSE;
	m_isCapitalization = FALSE;
    my_priority = BAD_UTILITY;
	raw_priority = BAD_UTILITY;
}













Goal_Build_List::Goal_Build_List
(
	AiMain *init_ai,
	sint32 init_which_building,
	double init_priority,
	BOOL is_wonder,
	BOOL is_end_game_object,
	BOOL is_capitalization,
	CityAgent *my_city
)
{

	the_class = CITY_GOAL_CLASS;

	if (is_wonder)
		removal_time = REMOVE_WHEN_EXECUTED;
	else
		removal_time = REMOVE_WHEN_SORTED;

	what_goal = GOAL_TYPE_BUILD_LIST;

	m_value = init_priority;
	my_priority = init_priority;

    which_building = init_which_building;

	m_isWonder = is_wonder;

	if (m_isWonder)
	{
		Assert(my_city);
		if (my_city)
		{
			m_myCityID = my_city->GetID();
		}
	}

	m_isEndGameObject = is_end_game_object;
	m_isCapitalization = is_capitalization;

	Compute_Raw_Priority(init_ai);
}












Goal_Build_List::~Goal_Build_List()
{
}













Goal_Build_List::Goal_Build_List(AiMain *ai,CivArchive &archive)
{

	the_class = CITY_GOAL_CLASS;

    Serialize(ai, archive);
}













void Goal_Build_List::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE

    ArmyGoal::Serialize(ai, archive);
	sint32 val;

    if (archive.IsStoring()) {
		archive << which_building;
		archive << raw_priority;
		archive << my_priority;
		archive << (sint32) m_isWonder;
		archive << (sint32) m_isEndGameObject;
		archive << (sint32) m_isCapitalization;
		archive << m_myCityID.GetVal();
    } else {
		archive >> which_building;
		archive >> raw_priority;
		archive >> my_priority;
		archive >> val;
		m_isWonder = val;
		archive >> val;
		m_isEndGameObject = val;
		archive >> val;
		m_isCapitalization = val;
		m_myCityID = val;
    }
}






BOOL Goal_Build_List::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai);
}













GOAL_TYPE Goal_Build_List::GetType () const
{
	return GOAL_TYPE_BUILD_LIST;
}













void Goal_Build_List::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG

	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_BUILD_LIST"));
#endif DBGAILOG
}













void Goal_Build_List::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG

	char * to_build_type_name;

	if (ai->AI_VERBOSITY < LOG_DETAILS) return;

	to_build_type_name = ai->m_buildingDB->Improvementtype_Int_To_String(which_building);

	if (to_build_type_name != NULL)
		AILOG((wstr, "   %s ", to_build_type_name));

	if (m_isWonder == TRUE)
		AILOG((wstr, "(city id = %d) ", m_myCityID.GetVal()));
#endif DBGAILOG
}














void Goal_Build_List::Compute_Needed_Troop_Flow()
{


}















double Goal_Build_List::Compute_Matching_Value
(
	AiMain *ai,
	squad_ptr the_squad
)
{

	if (the_squad->Contains_A_City())
	{
		return raw_priority;
	}
	else
	{
		return A_REALLY_BIG_NEGATIVE_REAL_NUMBER;
	}
}



















int Goal_Build_List::Is_Unit_Appropriate
(
	AiMain *ai,
	Agent * unit_in_question
)
{

	AGENT_TYPE  agent_type;
	CityAgent * the_city;


	agent_type = unit_in_question->GetType();

	if (agent_type == AGENT_TYPE_CITY)
	{

		the_city = (CityAgent *) unit_in_question;

		if (m_isWonder &&
			the_city->PretestContructWonder(ai, which_building))
		{

			if (the_city->GetID() != m_myCityID)
				return false;

			return true;
		}

		if (m_isEndGameObject &&
			the_city->PretestConstructEndGameObject(ai, which_building))
			return true;

		if (m_isCapitalization &&
			the_city->PretestConstructCapitalization(ai))
			return true;

		if (the_city->PretestContructBuilding(ai, which_building))
		{

			if (the_city->Already_Has_One(which_building))
				return false;

			return true;
		}

	}

	return false;
}













Goal_Result Goal_Build_List::Execute_Task(AiMain *ai, Plan *the_plan)
{

	int i;
    ArmyAgent *aa=NULL;
    CityAgent *the_city=NULL;
    BSetID tmpid;
	sint32 agent_count;
	sint32 failed_unit_count = 0;


	if (attacking_squad is NULL)
	{
#ifdef DBGAILOG
		#ifdef LOG_GOAL_BUILD_LIST
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Goal_Build_List::Execute_Task: Exiting - squad NULL\n"));
		#endif
#endif DBGAILOG

		return GOAL_FAILED;

	}




	agent_count = attacking_squad->my_agents.count;

	Assert(agent_count <= 1);

	for (i=0; i < agent_count; i++)
	{

		the_city = (CityAgent *) attacking_squad->my_agents.Return_Data_By_Number(i);

		if (the_city->GetQueueLen() < 1)
		{
			if (m_isWonder) {
					the_city->EnqueueBuildWonder(ai,
												 which_building,
												 raw_priority);
            } else if (m_isEndGameObject) {
					the_city->EnqueueBuildEndGameObject(ai,
														which_building,
														raw_priority);
				}
			else if (m_isCapitalization)
				{
					the_city->BuildCapitalization(ai);
				}
			else
			{
				the_city->EnqueueBuildBuilding(ai, which_building, raw_priority);
			}

			the_city->current_goal_type = what_goal;

		}

		else
		{

			failed_unit_count++;

		}

	}

	if (failed_unit_count == agent_count)
	{

		return GOAL_INAPPROPRIATE;

	}

	return GOAL_IN_PROGRESS;
}
















double Goal_Build_List::Compute_Raw_Priority(AiMain *ai)
{

	if (m_isWonder)
		raw_priority = my_priority +
			ai->m_planner->the_AIP.fzwonder_priority_modifier;
	else if (m_isEndGameObject)
		raw_priority = my_priority +
			ai->m_planner->the_AIP.build_end_game_object_priority;
	else if (m_isCapitalization)
		raw_priority = my_priority +
			ai->m_planner->the_AIP.build_capitalization_priority;
	else
		raw_priority = my_priority +
			ai->m_planner->the_AIP.build_buildings_priority;

	return raw_priority;
}
