 












#include "c3.h"
#include "globals.h"
#include "c3types.h"
#include "c3errors.h"

#include "IMapPointData.h"


#include "FlatPtr.h"
#include "CivArchive.h"



#include "common.h"
#include "linked_list.h"
#include "Strategic_AI_Forward.h"
#include "Unitflow.h"
#include "Squad.h"
#include "Goal.h"



#include "semi_dynamic_array.h"
#include "list_array.h"
#include "Pool.h"


#include "Plan.h"
#include "DynArr.h"

#include "AiMain.h"

#include "Scheduler.h"
#include "Planner.h"

#include "Squad_Strength.h"



ZEROMEM(PlanFlat); 


FLATSERIALIZE(PlanFlat); 



extern Pool<Squad> * g_squad_pool;

Plan::Plan()
{ 

	


	Init();







}

void Plan::Init()
{




	the_squad = NULL;
	the_goal = NULL;


	

	m_needed_strength.Init();
}

Plan::Plan(AiMain *ai,CivArchive &archive)
{ 
	


    Serialize(ai, archive); 
} 







Plan::~Plan()
{ 
























	


} 







































void Plan::Serialize(AiMain *ai, CivArchive &archive)
{
	sint32 squad_id;
	sint32 goal_id;

    CHECKSERIALIZE

    PlanFlat::Serialize(archive); 


	


    m_needed_strength.Serialize(archive); 

	
    if (archive.IsStoring())
	{





		if (the_squad != NULL)
			archive << (sint32) the_squad->squad_ID;
		else archive << (sint32) -1;

		if (the_goal != NULL)
			archive << (sint32) the_goal->goal_ID;
		else archive << (sint32) -1;

	} 
	else
	{











		
		archive >> squad_id;

		
		if (squad_id == -1) the_squad = NULL;
		else
		{
			
			the_squad = ai->m_planner->Look_Up_Squad_In_Load_Table(squad_id);
		}

		
		archive >> goal_id;

		
        if (UNASSIGNED_ID == goal_id) { 
            the_goal = NULL;
        }
		else
		{
			
			the_goal = ai->m_planner->Look_Up_Goal_In_Load_Table(goal_id);
		}

	} 

}






























































BOOL Plan::Plan_Is_Needed_And_Valid() const
{ 

    if (used)
        return FALSE; 

    if (NULL == the_squad) 
        return FALSE; 

    if (the_squad->already_committed)
        return FALSE; 

    if (the_squad->my_agents.count < 1) 
        return FALSE; 

    if (NULL == the_goal) { 
        return FALSE; 
    }

    if (!the_goal->CanBeExecuted()) { 
        return FALSE; 
    } 

    return TRUE; 
}	
















BOOL Plan::AccumulateEnoughStrength(AiMain *ai)
{
	
	squad_ptr the_new_squad;			
	BOOL recruited = false;				
	int excess;
	

	
	if (the_goal->what_goal == GOAL_TYPE_RETREAT)
		{
			the_goal->Detach_Squad();
		}

	
    if (NULL == the_goal->attacking_squad) 
	{
		int squad_pool_index;			


		
		
		the_new_squad = 
			g_squad_pool->Get_Next_Pointer(squad_pool_index);
		the_new_squad->Init(ai->m_planner);
		the_new_squad->squad_pool_index = squad_pool_index;

		
		the_new_squad->Set_Goal (the_goal); 

		
		ai->m_planner->Add_Squad(the_new_squad);

		
		the_new_squad->Recruit_Best_Strength(ai, the_goal, the_squad);
	
		
		recruited = TRUE; 

    } 

	
	else 
	{  
        
 		
		recruited = 
			the_goal->attacking_squad->Recruit_Best_Strength(ai, 
											the_goal, the_squad); 
    } 

	

	return the_goal->Is_Satisfied(&excess);
}
