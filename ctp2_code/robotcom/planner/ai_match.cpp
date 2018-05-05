#include "ctp/c3.h"
#include "user-robotcom/shared/globals.h"
#include "ctp/c3types.h"
#include "ctp/ctp2_utils/c3errors.h"

#include "gs/outcom/IMapPointData.h"

#include "aimain.h"

#include "robotcom/Agent/Agent.h"
#include "robotcom/Agent/ArmyAgent.h"
#include "robotcom/Agent/CityAgent.h"

#include "common.h"

#include "robotcom/planner/linked_list.h"




#include "robotcom/planner/ailog.h"
#include "robotcom/planner/dr_debug.h"
















#include "robotcom/Agent/Agent.h"
#include "robotcom/planner/ai_match.h"




































































































































void AI_Match_Team_Unit_Strength_Recalculate (Planner_ptr the_team)
{









}

















void AI_Match_Init (void)
{










}














void AI_Match_Close (void)
{

}













void AI_Match_Team_Init (Planner_ptr the_team)
{










}














void AI_Match_Team_Close (Planner_ptr the_team)
{

}


















int AI_Match_Get_Unit_Strength
(
	AiMain *ai,
	Agent * the_unit
)
{

	int int_strength;
    ArmyAgent *aa=NULL;
    CityAgent *ca=NULL;





	switch (the_unit->GetType())
	{

	case AGENT_TYPE_CITY:

		ca = (CityAgent *) the_unit;

		int_strength = (int) ca->Estimate_City_Strength(ai);
		break;

	case AGENT_TYPE_ARMY:

		aa = (ArmyAgent *) the_unit;

		int_strength = (int) aa->Estimate_Army_Strength(ai);
		break;

	default:
		Assert(false);
	}

	return (int_strength);
}
