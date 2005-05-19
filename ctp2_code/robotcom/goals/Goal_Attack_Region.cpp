






























































#include "c3.h"
#include "civarchive.h"
#include "IMapPointData.h"


#include "Agent.h"
#include "ArmyAgent.h"
#include "CityAgent.h"


#include "ic3GameState.h"
#include "aimain.h"


#include "Goal_Attack_Region.h"

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

Goal_Attack_Region::Goal_Attack_Region()
{
	
    
	removal_time = REMOVE_WHEN_SORTED;
	what_goal = GOAL_TYPE_ATTACK_REGION; 
	
	the_class = MILITARY_GOAL_CLASS;

    return; 
}

Goal_Attack_Region::Goal_Attack_Region
(
	AiMain *init_ai,					
	const double v,						
	const MapPointData &pos				
) 
: ArmyGoal(init_ai, v, pos)
{
	
    
	removal_time = REMOVE_WHEN_SORTED;
	what_goal = GOAL_TYPE_ATTACK_REGION; 

	
	the_class = MILITARY_GOAL_CLASS;

	return; 
} 	

Goal_Attack_Region::Goal_Attack_Region
(
	AiMain *ai,					
	CivArchive &archive
) 			
										
{
	
	the_class = MILITARY_GOAL_CLASS;

    Serialize(ai, archive); 
}


Goal_Attack_Region::~Goal_Attack_Region()
{ 
	return; 
} 

void Goal_Attack_Region::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE

	
	
	ArmyGoal::Serialize(ai, archive); 

	return; 
}


BOOL Goal_Attack_Region::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}


GOAL_TYPE Goal_Attack_Region::GetType () const
{
	return GOAL_TYPE_ATTACK_REGION; 
}







void Goal_Attack_Region::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_ATTACK_REGION"));
#endif DBGAILOG
}













void Goal_Attack_Region::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		AILOG((wstr, " Target: (%d, %d, %d)", m_pos->x, m_pos->y, m_pos->z));
#endif DBGAILOG
}













int Goal_Attack_Region::Get_Totally_Complete(AiMain *ai)
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






























































double Goal_Attack_Region::Compute_Raw_Priority(AiMain *ai)
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

	
	raw_priority = m_value + ai->m_planner->the_AIP.attack_region_priority;
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

















int Goal_Attack_Region::Is_Unit_Appropriate
(
	AiMain *ai,							
	Agent * unit_in_question			
)
{

	
	AGENT_STATE agent_state;			
	AGENT_TYPE  agent_type;				
	ArmyAgent * the_army;				
	CityAgent * the_city;				
	

	
	agent_type = unit_in_question->GetType();
	switch (agent_type)
	{
	case AGENT_TYPE_ARMY:
	{

		
		the_army = (ArmyAgent *) unit_in_question;

		
		
		agent_state = the_army->GetState();
		switch (agent_state) 
		{ 
		case AGENT_STATE_MARKED_SETTLE:
		case AGENT_STATE_MARKED_DEAD:            
		case AGENT_STATE_MARKED_GARRISON:
		case AGENT_STATE_AT_GOAL:
			return false;
		default:
			break;
		}

        if (the_army->IsWounded()) return false; 

		if (the_army->Get_Army_Attack_Strength(ai) <= 0.1) return false;

		return true;


	} 
	break;
	case AGENT_TYPE_CITY:
	{
		
		the_city = (CityAgent *) unit_in_question;

		
		if (the_city->GetQueueLen() < 1)
			return true; 
		else return false;
	}
	default:
		return false;
	}
}






void Goal_Attack_Region::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army)
{
    the_army->SetState(AGENT_STATE_LOOKING_FOR_WORK); 
}

Goal_Result Goal_Attack_Region::CheckCompletion(const sint32 build_count)
{
    
    return GOAL_IN_PROGRESS; 
}

