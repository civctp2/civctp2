















































































































#include "c3.h"


#include "CivArchive.h"
#include "IMapPointData.h"
#include "IC3World.h"


#include "IC3GameState.h"
#include "AiMain.h"
#include "AiMap.h"


#include "dr_debug.h"
#include "matrix.h"
#include "Grassfire.h"
#include "Strategic_Map.h"


#include "FzOut.h"

extern double fz_settle_value_coef;
extern double fz_settle_bonus_space; 
extern double fz_settle_bonus_water; 
extern double fz_settle_bonus_island;


#include "Agent.h"
#include "ArmyAgent.h"
#include "CityAgent.h"
#include "cont.h"

#include "GlSettl.h"

#include "AILog.h"

#define LOG_GOALSETTLE true

GoalSettle::GoalSettle()
{
	
	removal_time = REMOVE_WHEN_SORTED;
	what_goal = GOAL_TYPE_SETTLE; 

	
	the_class = SETTLER_GOAL_CLASS;

    Assert(m_XYpos); 

}


GoalSettle::GoalSettle
(
	AiMain *ai,					
	const double v,						
	MapPointData &pos				
) 
: ArmyGoal(ai, v, pos)
{
    

	removal_time = REMOVE_WHEN_SORTED;
	what_goal = GOAL_TYPE_SETTLE; 

	
	the_class = SETTLER_GOAL_CLASS;

    Assert(m_XYpos); 

    if (SPACE_Z == pos.z) { 
        m_environment = SETTLE_ENVIRONMENT_SPACE; 
    } else { 
        if (ai->m_world->IsMoveTypeWater(&pos) ||  ai->m_world->IsMoveTypeShallowWater(&pos)) { 
            m_environment = SETTLE_ENVIRONMENT_WATER; 
        } else { 
           
            sint32 dest_cont; 
            BOOL dest_is_land; 
            BOOL b; 
 	        b = ai->m_continents->GetContinent(ai, pos, dest_cont, dest_is_land);
            Assert(b); 
		    double pop = ai->m_continents->GetLandSettlers(dest_cont) + 
    		    ai->m_continents->GetLandCities(dest_cont); 

            if (pop < 1 ) { 
                m_environment = SETTLE_ENVIRONMENT_ISLAND; 
            } else { 
                m_environment = SETTLE_ENVIRONMENT_LAND; 
            } 
        }
    } 

} 	

GoalSettle::GoalSettle
(
	AiMain *ai,							
	CivArchive &archive
) 

										
{
	what_goal = GOAL_TYPE_SETTLE; 

	
	the_class = SETTLER_GOAL_CLASS;

    Serialize(ai, archive); 

    Assert(m_XYpos); 
}


GoalSettle::~GoalSettle()
{ 
	return; 
} 

void GoalSettle::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE

	
	
	ArmyGoal::Serialize(ai, archive); 

    Assert(m_XYpos); 

}


BOOL GoalSettle::Validate(AiMain *ai)
{
    BOOL r =  ArmyGoal::Validate(ai); 

    Assert(m_XYpos); 

    return r; 
}



GOAL_TYPE GoalSettle::GetType () const
{
        Assert(m_XYpos); 

	return GOAL_TYPE_SETTLE; 
}







void GoalSettle::Display_Goal_Type(AiMain *ai)
{
    Assert(m_XYpos); 
#ifdef DBGAILOG

	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_SETTLE"));
#endif DBGAILOG
}













void GoalSettle::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		AILOG((wstr, " Target: (%d, %d, %d)", m_pos->x, m_pos->y, m_pos->z));
#endif DBGAILOG
}





 

void GoalSettle::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army, 
    ArmyAgent *the_transport, SUB_TASK_TYPE sub_task)
{
    if (SUB_TASK_GOAL == sub_task) { 
        Set_Totally_Complete(TRUE); 
        the_army->ActionSettle(ai);	

		
		
		


    }
	if (SUB_TASK_CARGO_TO_BOARD == sub_task)
	{
		the_army->TryToBoardTransport(ai, the_transport);
	}
}











double GoalSettle::Compute_Raw_Priority(AiMain *ai)
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

	
	if (ai->m_my_player_id == PLAYER_INDEX_VANDALS)
		return BAD_UTILITY;

    double env_bonus; 
    switch (m_environment) { 
    case SETTLE_ENVIRONMENT_LAND: env_bonus = 0.0; break;
    case SETTLE_ENVIRONMENT_SPACE: env_bonus = fz_settle_bonus_space; break;
    case SETTLE_ENVIRONMENT_WATER: env_bonus = fz_settle_bonus_water;  break; 
    case SETTLE_ENVIRONMENT_ISLAND: env_bonus = fz_settle_bonus_island; break ; 
    default:
        INSANE(SETTLE_ENVIRONMENT_UNKNOWN); 
    } 

	
	int distance_to_us = 
		ai->m_planner->the_strategic_map->Get_Distance_To_Our_Empire(*m_XYpos);

    
	raw_priority = ai->m_planner->the_AIP.settle_priority;

	
	raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
		-1, 
		-1, 
		 0, 
		 1, 
		 0, 
		 1, 
		 1); 

	
	
    raw_priority += m_value * fz_settle_value_coef;

   	raw_priority += env_bonus; 

	
	return raw_priority;
}









