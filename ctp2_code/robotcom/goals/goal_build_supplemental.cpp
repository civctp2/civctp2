#include "c3.h"
#include "dynarr.h"
#include "civarchive.h"
#include "IMapPointData.h"
#include "ic3UnitDB.h"

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

#include "goal_build_supplemental.h"

#include "ailog.h"

#define LOG_GOAL_BUILD_SUPPLEMENTAL true

















Goal_Build_Supplemental::Goal_Build_Supplemental()
{

	the_class = CITY_GOAL_CLASS;

	what_goal = GOAL_TYPE_BUILD_SUPPLEMENTAL;
    which_unit = -1;
    my_priority = BAD_UTILITY;
	raw_priority = BAD_UTILITY;
}













Goal_Build_Supplemental::Goal_Build_Supplemental
(
	AiMain *init_ai,
	sint32 init_which_unit,
	double init_priority
)
{

	the_class = CITY_GOAL_CLASS;

	removal_time = REMOVE_WHEN_SORTED;
	what_goal = GOAL_TYPE_BUILD_SUPPLEMENTAL;

	m_value = init_priority;
	my_priority = init_priority;

    which_unit = init_which_unit;

	Compute_Raw_Priority(init_ai);
}













Goal_Build_Supplemental::~Goal_Build_Supplemental()
{
}













Goal_Build_Supplemental::Goal_Build_Supplemental(AiMain *ai,CivArchive &archive)
{

	the_class = CITY_GOAL_CLASS;

    Serialize(ai, archive);
}













void Goal_Build_Supplemental::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE

    ArmyGoal::Serialize(ai, archive);

    if (archive.IsStoring()) {
		archive << which_unit;
		archive << raw_priority;
		archive << my_priority;
    } else {
		archive >> which_unit;
		archive >> raw_priority;
		archive >> my_priority;
    }
}






BOOL Goal_Build_Supplemental::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai);
}













GOAL_TYPE Goal_Build_Supplemental::GetType () const
{
	return GOAL_TYPE_BUILD_SUPPLEMENTAL;
}













void Goal_Build_Supplemental::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG

	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_BUILD_SUPPLEMENTAL"));
#endif DBGAILOG
}













void Goal_Build_Supplemental::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG

	char * to_build_type_name;

	if (ai->AI_VERBOSITY < LOG_DETAILS) return;

	to_build_type_name = ai->m_unitDB->Unittype_Int_To_String(which_unit);

	if (to_build_type_name != NULL)
		AILOG((wstr, "   %s ", to_build_type_name));
#endif DBGAILOG
}














void Goal_Build_Supplemental::Compute_Needed_Troop_Flow()
{


}















double Goal_Build_Supplemental::Compute_Matching_Value
(
	AiMain *ai,
	squad_ptr the_squad
)
{
    Agent *agent=NULL;

    if (the_squad->my_agents.count <= 0) {
        return BAD_UTILITY;
    }

    sint32 i;

	for (i = 0; i < the_squad->my_agents.count; i++)
	{

		agent = (Agent *) the_squad->my_agents.Return_Data_By_Number( i );

        if (agent->GetType() == AGENT_TYPE_CITY)
			{
				CityAgent *ca = (CityAgent *) agent;
				if (ca->PretestContructUnit(ai, which_unit))
					return raw_priority;
			}

	}

    return BAD_UTILITY;











}



















int Goal_Build_Supplemental::Is_Unit_Appropriate
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

		if (the_city->GetQueueLen() < 1)
		{
			return true;
		}

	}

	return false;

}













Goal_Result Goal_Build_Supplemental::Execute_Task(AiMain *ai, Plan *the_plan)
{

	int i;
    CityAgent *the_city=NULL;
	sint32 agent_count;
	sint32 failed_unit_count = 0;


	if (attacking_squad is NULL)
	{
#ifdef DBGAILOG
		#ifdef LOG_GOAL_BUILD_SUPPLEMENTAL
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Goal_Build_Supplemental::Execute_Task: Exiting - squad NULL\n"));
		#endif
#endif DBGAILOG

		return GOAL_FAILED;

	}




	agent_count = attacking_squad->my_agents.count;

	Assert(agent_count <= 1);

	for (i=0; i < agent_count; i++)
	{

		the_city = (CityAgent *) attacking_squad->my_agents.Return_Data_By_Number(i);

		if (the_city->PretestContructUnit(ai, which_unit)){

			the_city->EnqueueArmy(ai, which_unit, raw_priority );

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
















double Goal_Build_Supplemental::Compute_Raw_Priority(AiMain *ai)
{

	raw_priority = my_priority + ai->m_planner->the_AIP.build_troops_priority;

	return raw_priority;
}
