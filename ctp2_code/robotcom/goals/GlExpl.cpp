
































































































































#include "c3.h"
#include "CivArchive.h"
#include "IMapPointData.h"
#include "IC3Player.h"


#include "Agent.h"
#include "ArmyAgent.h"
#include "CityAgent.h"


#include "IC3GameState.h"
#include "AiMain.h"


#include "GlExpl.h"

#include "AILOG.h"


#include "dr_debug.h"
#include "matrix.h"
#include "Grassfire.h"
#include "Strategic_Map.h"


#include "Agent.h"
#include "ArmyAgent.h"
#include "CityAgent.h"


#include "FzOut.h"
#include "AiMap.h"
#include "ScienceAgent.h"

#include "IC3World.h"


GoalExplore::GoalExplore()
{
	
	
	removal_time = REMOVE_WHEN_SORTED;
	what_goal = GOAL_TYPE_EXPLORE;

	
	the_class = EXPLORE_GOAL_CLASS;

    return; 
}

GoalExplore::GoalExplore
(
	AiMain *init_ai,					
	const double v,						
	const MapPointData &pos				
) 
: ArmyGoal(init_ai, v, pos)
{
	
	
	removal_time = REMOVE_WHEN_SORTED;
	what_goal = GOAL_TYPE_EXPLORE;

	
	the_class = EXPLORE_GOAL_CLASS;


	return; 
} 	

GoalExplore::GoalExplore
(
	AiMain *ai, 
	CivArchive &archive
) 			
										
{
	what_goal = GOAL_TYPE_EXPLORE;

	
	the_class = EXPLORE_GOAL_CLASS;

    Serialize(ai, archive); 
}


GoalExplore::~GoalExplore()
{ 
	return; 
} 

void GoalExplore::Serialize(AiMain *ai, CivArchive &archive)
{
    CHECKSERIALIZE

	
	
	ArmyGoal::Serialize(ai, archive); 

	return; 
}


BOOL GoalExplore::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}


GOAL_TYPE GoalExplore::GetType () const
{
	return GOAL_TYPE_EXPLORE; 
}







void GoalExplore::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_EXPLORE"));
#endif DBGAILOG
}













void GoalExplore::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG((wstr, " Target: (%d, %d, %d)", m_pos->x, m_pos->y, m_pos->z));
#endif DBGAILOG
}













int GoalExplore::Get_Totally_Complete(AiMain *ai)
{
	return m_is_completed;





















































}
















































double GoalExplore::Compute_Raw_Priority(AiMain *ai)
{
	int num_cities;						

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

	
	num_cities = ai->m_planner->the_strategic_map->Get_Our_City_Count();
  
	
	raw_priority = ai->m_planner->the_AIP.explore_priority;

	
	if (num_cities > 0)
	{
	
	raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
		-1, 
		-1, 
		 0, 
		 0, 
		 0, 
		-1, 
		 1); 
	}
	else
	{
	
	raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
		-1, 
		-1, 
		 1, 
		 1, 
		 0, 
		-1, 
		 1); 
	}

	
	return raw_priority;

}
















































































































































































void GoalExplore::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army)
{
    Set_Totally_Complete(TRUE); 
    the_army->SetState(AGENT_STATE_LOOKING_FOR_WORK); 
    attacking_squad->Detach_Goal(); 
}









Goal_Result GoalExplore::CheckCompletion(const sint32 build_count)
{

    Assert(attacking_squad); 
    
    return GOAL_IN_PROGRESS; 
}































