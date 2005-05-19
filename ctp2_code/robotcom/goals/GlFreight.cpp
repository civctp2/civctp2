#include "c3.h"

#include "civarchive.h"
#include "FlatPtr.h"

#include "aimain.h"

#include "glfreight.h"

#include "ailog.h"
#include "Plan.h"
#include "ArmyAgent.h"
#include "CityAgent.h"

GoalFreightFlat::~GoalFreightFlat()
{
    return; 
} 

GoalFreight::GoalFreight
(
	sint32 fc, 
	double u
) 
{    
	removal_time = REMOVE_WHEN_EXECUTED;
	what_goal = GOAL_TYPE_CONSTRUCT_FREIGHT;
	the_class = CITY_GOAL_CLASS;
	m_freight_cost = fc;
    m_value = u;
}

GoalFreight::~GoalFreight()
{
    return; 
} 

ZEROMEM(GoalFreightFlat); 
FLATSERIALIZE(GoalFreightFlat); 

GoalFreight::GoalFreight(
  AiMain *ai, CivArchive &archive)
{

    Serialize(ai, archive); 
} 

void GoalFreight::Serialize(AiMain *ai, CivArchive &archive)
{
    CHECKSERIALIZE
        
    ArmyGoal::Serialize(ai, archive); 
	GoalFreightFlat::Serialize(archive); 
}


BOOL GoalFreight::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}

GOAL_TYPE GoalFreight::GetType() const
{
   return GOAL_TYPE_CONSTRUCT_FREIGHT;
}







void GoalFreight::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_CONSTRUCT_FREIGHT"));
#endif DBGAILOG
}






Goal_Result GoalFreight::BuildTaskSolution(AiMain *ai, CityAgent *the_city, Plan* the_plan)

{
    Assert(the_city);
    Assert(the_plan); 
    double utility = the_plan->GetMatchingValue(); 

    the_city->EnqueueBuildFreight(ai, utility);

	
	the_city->current_goal_type = what_goal;

    return GOAL_IN_PROGRESS; 

}

Goal_Result GoalFreight::CheckCompletion(const sint32 build_count)
{
    if (0 < build_count) {
        return GOAL_COMPLETE; 
    } else { 
        return GOAL_FAILED; 
    } 
}










double GoalFreight::Compute_Raw_Priority(AiMain *ai)
{
	raw_priority = ai->m_planner->the_AIP.fzfreight_priority_modifier
		* (double) m_value;

	
	return raw_priority;
}




