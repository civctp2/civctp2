





































































































#include "c3.h"
#include "CivArchive.h"
#include "IMapPointData.h"
#include "IC3Rand.h"


#include "Agent.h"
#include "ArmyAgent.h"
#include "CityAgent.h"


#include "IC3GameState.h"
#include "AiMain.h"


#include "Goal_Patrol.h"

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

Goal_Patrol::Goal_Patrol()
{
	
	the_class = PROTECT_GOAL_CLASS;

	
	removal_time = REMOVE_WHEN_SORTED;


	what_goal = GOAL_TYPE_PATROL; 
    return; 
}

Goal_Patrol::Goal_Patrol
(
	AiMain *init_ai,					
	const double v,						
	const MapPointData &pos				
) 
: ArmyGoal(init_ai, v, pos)
{
	
	the_class = PROTECT_GOAL_CLASS;

	
	removal_time = REMOVE_WHEN_SORTED;



	what_goal = GOAL_TYPE_PATROL; 

	return; 
} 	

Goal_Patrol::Goal_Patrol
(
	AiMain *ai,					
	CivArchive &archive
) 			
										
{
	
	the_class = PROTECT_GOAL_CLASS;

    Serialize(ai, archive); 
}


Goal_Patrol::~Goal_Patrol()
{ 
	return; 
} 

void Goal_Patrol::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE

	
	
	ArmyGoal::Serialize(ai, archive); 

	return; 
}


BOOL Goal_Patrol::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}


GOAL_TYPE Goal_Patrol::GetType () const
{
	return GOAL_TYPE_PATROL; 
}







void Goal_Patrol::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_PATROL"));
#endif DBGAILOG
}













void Goal_Patrol::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		AILOG((wstr, " Target: (%d, %d, %d)", m_pos->x, m_pos->y, m_pos->z));
#endif DBGAILOG
}













int Goal_Patrol::Get_Totally_Complete(AiMain *ai)
{
	

	int i;								
	int completely_there_count = 0;		
										
    Agent *agent = NULL;				
    ArmyAgent *aa=NULL;
	MapPointData army_pos;				
	

	
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

	return m_is_completed;
}



































































double Goal_Patrol::Compute_Raw_Priority(AiMain *ai)
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

	
	raw_priority = m_value + ai->m_planner->the_AIP.patrol_priority;
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
















Goal_Result Goal_Patrol::CheckCompletion(const sint32 build_count)
{
    
    return GOAL_IN_PROGRESS; 
}


