


#include "c3.h"
#include "CivArchive.h"
#include "IMapPointData.h"


#include "IC3GameState.h"
#include "AiMain.h"

#include "GoalWonder.h"
#include "CityAgent.h"

GoalWonderFlat::~GoalWonderFlat()
{ 
    return; 
} 

GoalWonder::GoalWonder()
{        
	
	the_class = CITY_GOAL_CLASS;

	removal_time = REMOVE_WHEN_FULFILLED;
	what_goal = GOAL_TYPE_CONSTRUCT_WONDER; 
}

GoalWonder::GoalWonder(const sint32 idx)
{
	
	the_class = CITY_GOAL_CLASS;

    m_idx_wonder = idx;
	removal_time = REMOVE_WHEN_FULFILLED;
	what_goal = GOAL_TYPE_CONSTRUCT_WONDER; 
    m_value = 100.0 - idx; 
}

GoalWonder::GoalWonder(AiMain *ai,CivArchive &archive)
{
	
	the_class = CITY_GOAL_CLASS;

	what_goal = GOAL_TYPE_CONSTRUCT_WONDER; 
    Serialize(ai, archive); 
}

GoalWonder::~GoalWonder() 
{
    return; 
}

void GoalWonderFlat::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive.Store((uint8*) this, sizeof(*this)); 
    } else { 
        archive.Load((uint8*) this, sizeof(*this)); 
    } 
}

void GoalWonder::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE

    ArmyGoal::Serialize(ai, archive); 
    GoalWonderFlat::Serialize(archive);    
}

BOOL GoalWonder::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}

GOAL_TYPE GoalWonder::GetType () const
{
    return GOAL_TYPE_CONSTRUCT_WONDER;
}

sint32 GoalWonder::GetIdxWonder() const
{
    return m_idx_wonder; 
}








void GoalWonder::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_CONSTRUCT_WONDER"));
#endif DBGAILOG
}





Goal_Result GoalWonder::BuildTaskSolution(AiMain *ai, CityAgent *the_city, Plan* the_plan)

{
    Assert(the_city);
    Assert(the_plan); 
    double utility = the_plan->GetMatchingValue(); 

	
	the_city->current_goal_type = what_goal;

    return the_city->EnqueueBuildWonder(ai, m_idx_wonder, utility);

}

Goal_Result GoalWonder::CheckCompletion(const sint32 build_count)
{
    if (0 < build_count) {
        return GOAL_COMPLETE; 
    } else { 
        return GOAL_FAILED; 
    } 
}










double GoalWonder::Compute_Raw_Priority(AiMain *ai)
{
	raw_priority = ai->m_planner->the_AIP.fzwonder_priority_modifier
		* (double) m_value;

	
	return raw_priority;
}




