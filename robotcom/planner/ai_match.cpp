
























#include "c3.h"
#include "globals.h"
#include "c3types.h"
#include "c3errors.h"

#include "IMapPointData.h"

#include "AiMain.h"

#include "Agent.h"
#include "ArmyAgent.h"
#include "CityAgent.h"

#include "common.h"


#include "linked_list.h"




#include "AILog.h"
#include "dr_debug.h"
















#include "Agent.h"
#include "AI_Match.h"




























































































  
  






































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



















































































































