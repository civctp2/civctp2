












































































































#include "c3.h"
#include "CivArchive.h"
#include "IMapPointData.h"
#include "IC3Rand.H"


#include "Agent.h"
#include "ArmyAgent.h"
#include "CityAgent.h"


#include "IC3GameState.h"
#include "AiMain.h"


#include "Goal_Perimeter.h"

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


Goal_Perimeter::Goal_Perimeter()
{
	
	the_class = PROTECT_GOAL_CLASS;

	
    
	removal_time = REMOVE_WHEN_SORTED;
	what_goal = GOAL_TYPE_PERIMETER; 
    return; 
}

Goal_Perimeter::Goal_Perimeter
(
	AiMain *init_ai,					
	const double v,						
	const MapPointData &pos				
) 
: ArmyGoal(init_ai, v, pos)
{
	
	the_class = PROTECT_GOAL_CLASS;

	
    
	removal_time = REMOVE_WHEN_SORTED;
	what_goal = GOAL_TYPE_PERIMETER; 

	return; 
} 	

Goal_Perimeter::Goal_Perimeter
(
	AiMain *ai,					
	CivArchive &archive
) 			
										
{
	
	the_class = PROTECT_GOAL_CLASS;

    Serialize(ai, archive); 
}


Goal_Perimeter::~Goal_Perimeter()
{ 
	return; 
} 

void Goal_Perimeter::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE

	
	
	ArmyGoal::Serialize(ai, archive); 

	return; 
}


BOOL Goal_Perimeter::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}


GOAL_TYPE Goal_Perimeter::GetType () const
{
	return GOAL_TYPE_PERIMETER; 
}







void Goal_Perimeter::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_PERIMETER"));
#endif DBGAILOG
}













void Goal_Perimeter::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		AILOG((wstr, " Target: (%d, %d, %d)", m_pos->x, m_pos->y, m_pos->z));
#endif DBGAILOG
}




































































































































double Goal_Perimeter::Compute_Raw_Priority(AiMain *ai)
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


	
	raw_priority = m_value + ai->m_planner->the_AIP.perimeter_priority;
	raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
		 1, 
		-1, 
		 1, 
		-1, 
		-1, 
		 1, 
		 1); 

	
	return raw_priority;

}















void Goal_Perimeter::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army)
{
    the_army->SetState(AGENT_STATE_LOOKING_FOR_WORK); 
}

Goal_Result Goal_Perimeter::CheckCompletion(const sint32 build_count)
{
    
    return GOAL_IN_PROGRESS; 
}

