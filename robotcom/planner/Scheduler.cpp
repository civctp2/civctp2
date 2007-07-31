










































































































































































































































































































































































































































































































































































































































































































#include "c3.h"
#include "globals.h"
#include "c3types.h"
#include "c3errors.h"
#include "CivArchive.h"
#include <Limits>  

#include "dr_debug.h"

#include "AiMain.h"


#include "ArmyAgent.h"
#include "CityAgent.h"
#include "InstBidList.h"


#include "common.h"

#include "linked_list.h"		
#include "semi_dynamic_array.h"
#include "sorted_array.h"
#include "list_array.h"


#include "Pool.h"
#include "Pointer_Pool.h"

#include "Red_Black_Tree.h"


#include "unitflow.h"
#include "AIP.h"
#include "squad.h"


#include "Goal.h"
#include "GlSettl.h"

#include "Scheduler.h"
#include "Planner.h"

#include "AILog.h"
#include "WallClock.h"
extern Wall_Clock *g_wall_clock;

#include "FlatPtr.h"

#define LOG_SCHEDULER

extern Pointer_Pool<Plan> * g_plan_pool;	



#define DBGAILOG 1




ZEROMEM(Scheduler_Flat_Data); 
FLATSERIALIZE(Scheduler_Flat_Data); 


#define MATCHES_CHUNK_SIZE 200





extern Pool<Squad> * g_squad_pool;



typedef Red_Black_Tree_Node<Plan *,Plan_Sort_Key> Xred_black_node_class;


Pointer_Pool<Xred_black_node_class> * g_Red_Black_Tree_Node_Pool; 


int g_Red_Black_Tree_Node_Pool_reference_count = 0;

static Plan_Sort_Key a_plan_sort_key;























int Scheduler::Allocate_Troops_And_Execute_Tasks
(
	AiMain *ai						
)
{

CHECKCLOCK
    
    Prepare_Goals(ai);
CHECKCLOCK

	
	Prepare_Squads(ai);
CHECKCLOCK

	
	Init_Match_Lists();
CHECKCLOCK

	
	Generate_Matches_And_Valid_Goal_List();
CHECKCLOCK

	#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		
		
	}
	#endif
    #endif DBGAILOG


	
	
	
	
    
	Match_Resources();

	
CHECKCLOCK

	#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		
		
	}
	#endif
    #endif DBGAILOG

	
    if (ai->m_i_am_dead) return false; 

	
	Update_Squads();
CHECKCLOCK

	
	
	Clean_Up();
CHECKCLOCK

	
	Planning_Status_Reset();
CHECKCLOCK

	
	return true;


}













void Scheduler::Init()
{
	
	int i;
	sorted_array<Goal_ptr,double> *a_goal_class;	
	list_array<GOAL_CLASS> *good_classes;	
											
	list_array<SQUAD_CLASS> *good_squad_classes;	
	list_array<Goal_ptr> *pruned_goal_class;	
	list_array<Squad *> *a_squad_class;		
	

	
	if (g_Red_Black_Tree_Node_Pool is NULL)
	{
		
		g_Red_Black_Tree_Node_Pool =  
			new Pointer_Pool<Xred_black_node_class>(INITAL_PLAN_POOL_SIZE);

	} 

	
	g_Red_Black_Tree_Node_Pool_reference_count++;

	
    new_goals = NULL;
	goals_to_delete = NULL;

#ifdef PLANS_IN_SORTED_ARRAY	
	the_matches = NULL;
#endif 

#ifdef PLANS_IN_RED_BLACK_TREE
	match_tree = NULL;
#endif 

	
	unique_goal_ID = 0;

	
	unique_plan_ID = 0;

#ifdef PLANS_IN_SORTED_ARRAY	
	
	pos_in_match_list = -1;
#endif 


#ifdef SUPER_DEBUG_SPEED
	
	super_debug_speed = false;
#endif 

	
	for (i = 0; i < MAX_GOAL_CLASSES; i++)
	{
		
		a_goal_class = 
			new sorted_array<Goal_ptr,double>(INITIAL_GOAL_CLASS_SIZE);

		
		sorted_goals_of_class[i] = a_goal_class;

		
		pruned_goal_class = 
			new list_array<Goal_ptr>(INITIAL_GOAL_CLASS_SIZE);

		
		pruned_goals_of_class[i] = pruned_goal_class;

		
		good_squad_classes =
			new list_array<SQUAD_CLASS>(INITIAL_GOOD_SQUAD_CLASSES_FOR_GOAL);

		
		appropriate_squad_classes[i] = good_squad_classes;

	} 

	
	for (i = 0; i < MAX_SQUAD_CLASSES; i++)
	{
		
		good_classes = 
			new list_array<GOAL_CLASS> (INITIAL_GOOD_GOAL_CLASSES_FOR_SQUAD);

		
		appropriate_goal_classes[i] = good_classes;

		
		a_squad_class =
			new list_array<Squad *>(INITIAL_SQUAD_CLASS_SIZE);

		
		squads_of_class[i] = a_squad_class;

	} 

    for (i=0; i<GOAL_TYPE_MAX; i++) { 
        goal_count[i] = 0; 
    } 

	
	
	m_updateTime = 0;
}













Scheduler::Scheduler
(
	AiMain *init_ai,					
	Planner_ptr i_team 					
)
{
	
	Init();

#ifdef PLANS_IN_SORTED_ARRAY	
	
	the_matches = new sorted_array<Plan *, double>(MATCHES_CHUNK_SIZE, 
													MATCHES_CHUNK_SIZE);
#endif 

	
	a_plan_sort_key.value = -DBL_MAX;
	a_plan_sort_key.ID = -1;

#ifdef PLANS_IN_RED_BLACK_TREE
	match_tree = new Red_Black_Tree<Plan *,Plan_Sort_Key>
		( 
			a_plan_sort_key,			
			NULL,						
			g_Red_Black_Tree_Node_Pool	
		);
#endif 

	
	new_goals = new linked_list;

	
	goals_to_delete = new linked_list;

	
	the_team = i_team;

	
	ai = init_ai,						

	
	Planning_Status_Reset();

}













Scheduler::Scheduler
(
	AiMain *init_ai,					
	Planner_ptr i_team,					
	CivArchive &archive					
)
{
	
	Init();

	
	a_plan_sort_key.value = -DBL_MAX;
	a_plan_sort_key.ID = -1;

#ifdef PLANS_IN_RED_BLACK_TREE
	match_tree = new Red_Black_Tree<Plan *,Plan_Sort_Key>
		( 
			a_plan_sort_key,			
			NULL,						
			g_Red_Black_Tree_Node_Pool	
		);
#endif 

	
	the_team = i_team;

	
	ai = init_ai,						

	
    Serialize(archive); 
}













Scheduler::~Scheduler()
{
	
	int i;
	sorted_array<Goal_ptr,double> *a_goal_class;	
	list_array<GOAL_CLASS> *good_classes;	
											
	list_array<Goal_ptr> *pruned_goal_class;	
	list_array<SQUAD_CLASS> *good_squad_classes;	
	list_array<Squad *> *a_squad_class;		
	

	
	
	
	delete new_goals;
	new_goals = NULL;

	
	
	Clean_Up();

    

	
#ifdef PLANS_IN_SORTED_ARRAY	
    if (the_matches isnt NULL) { 
        delete the_matches;
        the_matches = NULL; 
    } 
#endif 

#ifdef PLANS_IN_RED_BLACK_TREE
	if (match_tree isnt NULL) 
	{
		delete match_tree;
		match_tree = NULL;
	} 
#endif 

	delete goals_to_delete;
    goals_to_delete = NULL; 

	
	for (i = 0; i < MAX_GOAL_CLASSES; i++)
	{
		
		a_goal_class = sorted_goals_of_class[i];
		good_squad_classes = appropriate_squad_classes[i];
		pruned_goal_class = pruned_goals_of_class[i];
			
		
		delete a_goal_class;
        a_goal_class = NULL; 

		delete good_squad_classes;
        good_squad_classes = NULL; 

		delete pruned_goal_class;
        pruned_goal_class = NULL; 

	} 

	
	for (i = 0; i < MAX_SQUAD_CLASSES; i++)
	{
		
		good_classes = appropriate_goal_classes[i];
		a_squad_class = squads_of_class[i];

		
		delete good_classes;
        good_classes = NULL;

		delete a_squad_class;
        a_squad_class = NULL; 

	} 

	
	g_Red_Black_Tree_Node_Pool_reference_count--;

	
	if ((g_Red_Black_Tree_Node_Pool_reference_count is 0) and
		(g_Red_Black_Tree_Node_Pool isnt NULL))
	{
		
		delete g_Red_Black_Tree_Node_Pool;
		g_Red_Black_Tree_Node_Pool = NULL;

	} 
	
}














void Scheduler::Load(CivArchive &archive)
{
	
	int i;								
	Goal * the_goal;					
	sint32 goal_id;						
	
	
	

	
    sint32 is_new_goal; 
    is_new_goal = archive.GetSINT32(); 
    if (is_new_goal) { 
        new_goals = new linked_list;
        sint32 num_new_goals;					
        num_new_goals = archive.GetSINT32();  
        for (i = 0; i < num_new_goals; i++) { 
		    archive >> goal_id;  
		    the_goal = the_team->Look_Up_Goal_In_Load_Table(goal_id); 
            Assert(the_goal); 
		    the_goal->new_goal_cell_ptr = new_goals->Add_Data((data_ptr) the_goal ); 
	    } 
    } else {
        is_new_goal = NULL; 
    } 


    sint32 is_del_goal; 
    is_del_goal = archive.GetSINT32(); 
    if (is_del_goal) { 
        goals_to_delete = new linked_list;
        sint32 num_del_goals;					
        num_del_goals = archive.GetSINT32();  
        for (i = 0; i < num_del_goals; i++) { 
		    archive >> goal_id;  
		    the_goal = the_team->Look_Up_Goal_In_Load_Table(goal_id); 
            Assert(the_goal); 
		    goals_to_delete->Add_Data((data_ptr) the_goal ); 
	    } 
    } else {
        is_del_goal = NULL; 
    } 



#ifdef PLANS_IN_SORTED_ARRAY	























#endif 


    archive.Load((uint8*)goal_count, GOAL_TYPE_MAX * sizeof(int)); 
}













void Scheduler::Store(CivArchive &archive)
{
	
	int i;								
	Goal * the_goal;					
	sint32 goal_id;						
#ifdef PLANS_IN_SORTED_ARRAY	
	Plan * the_plan;					
#endif
	

	

	
    if (new_goals) { 
        archive.PutSINT32(1); 
	    archive.PutSINT32(new_goals->count); 

	    
	    for (i = new_goals->count - 1; i >= 0; i--)
	    {
		    
		    the_goal = (Goal *) new_goals->Return_Data_By_Number( i );

		    
		    goal_id = the_goal->goal_ID;

		    
		    archive << goal_id;

	    } 
    } else { 
        archive.PutSINT32(0); 
    } 

    if(goals_to_delete) {
        archive.PutSINT32(1); 
	    
	    archive.PutSINT32(goals_to_delete->count); 

	    
	    for (i = goals_to_delete->count - 1; i >= 0; i--)
	    {
		    
		    the_goal = (Goal *) goals_to_delete->Return_Data_By_Number( i );

		    
		    goal_id = the_goal->goal_ID;

		    
		    archive << goal_id;

	    } 
    } else { 
        archive.PutSINT32(0); 
    } 

	
	

#ifdef PLANS_IN_SORTED_ARRAY	
	
    if (the_matches) { 
        archive.PutSINT32(1); 
	    archive << (sint32) the_matches->count; 

	    
	    for (i = the_matches->count - 1; i >= 0; i--)
	    {
		    
		    the_plan = (Plan *) the_matches->Return_Data_By_Number( i );

		    
		    the_plan->Serialize(ai, archive);

	    } 
    } else { 
        archive.PutSINT32(0); 
    } 
#endif 

    archive.Store((uint8*)goal_count, GOAL_TYPE_MAX * sizeof(int)); 

} 












void Scheduler::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    Scheduler_Flat_Data::Serialize(archive); 

	
    if (archive.IsStoring())
	{
		
		Store(archive);

	} 
	else
	{
		
		Load(archive);

	} 

}






BOOL Scheduler::Validate(AiMain *ai)
{
    sint32 i; 
    BOOL r; 
    Goal *the_goal=NULL, 
        *the_plan_goal=NULL; 

    if (new_goals) { 
        for (i=0; i<new_goals->count; i++) {
            the_goal = (Goal *)new_goals->Return_Data_By_Number(i); 
            Assert(the_goal);
            r = the_goal->Validate(ai); 
            if (!r) return FALSE; 

		 	the_plan_goal = the_team->Look_Up_Goal_In_Load_Table(the_goal->goal_ID); 
            Assert(the_goal == the_plan_goal); 
		    if (the_goal != the_plan_goal) return FALSE; 
        } 
    } 
    return TRUE;
}









void Scheduler::Associate_Squad_Class_With_Goal_Class
(
	SQUAD_CLASS the_squad_type,		
	GOAL_CLASS the_goal_class		
)
{
	
	
	appropriate_goal_classes[the_squad_type]->Append_Data(the_goal_class);
	appropriate_squad_classes[the_goal_class]->Append_Data(the_squad_type);
}


















void Scheduler::Reclassify_Squad
(
	Squad *the_squad,				
	SQUAD_CLASS old_squad_type		
)
{
	
	squads_of_class[the_squad->squad_type]->Append_Data(
		the_squad);

	

}













void Scheduler::Planning_Status_Reset()

{

	
	troop_allocation_done = FALSE;

	
	goals_inited = FALSE;

	
	squads_inited = FALSE;

	
	subscheduling_inited = FALSE;

	


	
	precompute_inited = FALSE;

	
	schedule_matching_done = FALSE;

	
	schedule_squad_fix_done = FALSE;

	
	send_groups_done = FALSE;

	
	allocated_troops = 0;

}













int Scheduler::Ungroup_Squads()

{
	int i;
	squad_ptr the_squad;

	
    #ifdef DBGAILOG
    #ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		{
			AILOG(( wstr,  "Ungrouping all squads\n"));
		}
    #endif
    #endif DBGAILOG

	if(the_team->my_squads) {
		for (i = 0; i < the_team->my_squads->count; i++)
			{
				
				the_squad = (squad_ptr)
					the_team->my_squads->Return_Data_By_Number( i );
				the_squad->UngroupAllArmies(ai);
			}
	}
	return TIME_SLICE_DONE;
}














int Scheduler::Update_Squads()

{
	
	int i;
	squad_ptr the_squad;				
	



    
    ArmyAgent *aa; 
    BSetID b_id; 
    for (aa = ai->m_army_set->First(b_id); ai->m_army_set->Last(); aa = ai->m_army_set->Next(b_id)) { 
        aa->m_has_been_executed= FALSE; 
    } 

	
    CityAgent *ca; 
    for (ca = ai->m_city_set->First(b_id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(b_id)) 
	{ 
        ca->m_has_been_executed= FALSE; 
    } 

	
	if (schedule_squad_fix_done)
	{
		return TIME_SLICE_NOT_DONE;
	}

#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		AILOG(( wstr,  "Updating Squads\n"));
	}
	#endif
#endif DBGAILOG

	
	
	for (i = the_team->my_squads->count - 1; i >= 0; i--)
	{
		
		the_squad = (squad_ptr)
		the_team->my_squads->Return_Data_By_Number( i);

		
		if (the_squad->my_agents.count <= 0)
		{

#ifdef DBGAILOG
			#ifdef LOG_SCHEDULER
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
			{
				#ifdef _DEBUG
				AILOG(( wstr,  "Deleting SQUAD %d.\n", the_squad->squad_ID));
				#else
				AILOG(( wstr,  "Deleting SQUAD.\n"));
				#endif 
			}
			#endif
#endif DBGAILOG

			
			the_team->my_squads->Delete_Element(i);

			
			the_squad->Clean_Up();

			
			g_squad_pool->Release_Pointer(the_squad->squad_pool_index);

		} 

	} 

	
	schedule_squad_fix_done = TRUE;

	return TIME_SLICE_DONE;

}












void Scheduler::Clean_Up()

{
	
	int i,j=0;
	Plan_ptr the_match;
	Goal_ptr the_goal=NULL;
	sorted_array<Goal_ptr,double> *a_goal_class;	
	


#ifdef VALIDATE_ON
     Validate(ai); 
#endif VALIDATE_ON


#ifdef DBGAILOG
#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		AILOG(( wstr,  "Scheduler CLEAN-UP\n"));
	}
#endif
#endif DBGAILOG

	
	for (i = 0; i < MAX_GOAL_CLASSES; i++)
	{
		
		a_goal_class = sorted_goals_of_class[i];

		
		for (j = a_goal_class->count - 1; j >=0; j--)
		{
			
			the_goal = a_goal_class->Return_Data_By_Number(j);

			
			the_goal->Detach_Squad();

			if (the_goal->removal_time == REMOVE_WHEN_SORTED &&
				!the_goal->marked_for_deletion)
			{
				Remove_Goal_And_Add_To_Delete_List(the_goal);
			}

		} 

	} 


#ifdef VALIDATE_ON
     Validate(ai); 
#endif VALIDATE_ON

	
	
	
	if (
#ifdef PLANS_IN_RED_BLACK_TREE
		(match_tree isnt NULL)

#ifdef PLANS_IN_SORTED_ARRAY	
		and 
#endif 

#endif 

#ifdef PLANS_IN_SORTED_ARRAY	
		(the_matches isnt NULL)
#endif 
		)
	{
		
#ifdef PLANS_IN_SORTED_ARRAY	
		for (i = 0; i < the_matches->count; i++)
		{
			
			the_match = (Plan_ptr) the_matches->Return_Data_By_Number(i);
#endif 

#ifdef PLANS_IN_RED_BLACK_TREE
#ifndef PLANS_IN_SORTED_ARRAY 
		
		match_tree->Initialize_Reverse_Order_Tree_Traversal();

		
		the_match = match_tree->Return_Next_Reverse_Order_Element();

		
		while (not match_tree->Is_Traversal_Done())
		{
#endif 
#endif 

#ifdef DBGAILOG
			#ifdef LOG_SCHEDULER
			if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
			{
				AILOG(( wstr,  "Free up the Match @%x.\n", (long int) the_match));
			}
			#endif
#endif DBGAILOG

			
			
			
			
				
				
			g_plan_pool->Release_Pointer(the_match);

#ifdef GRAPHICS_CALLBACK
			
			if (!(i%k_CALLBACK_CHECK_FREQUENCY) && TimeToUpdateGraphics()) {
				ai->m_gs->ProcessGraphicsCallback();
			}
#endif
			

#ifdef PLANS_IN_RED_BLACK_TREE
			
			the_match = match_tree->Return_Next_Reverse_Order_Element();
#endif 

		} 
		  

		
#ifdef PLANS_IN_SORTED_ARRAY	
		the_matches->Empty();
#endif 

#ifdef PLANS_IN_RED_BLACK_TREE
		match_tree->Release_All_Nodes();
#endif 

	} 


#ifdef VALIDATE_ON
     Validate(ai); 
#endif VALIDATE_ON


#ifdef VALIDATE_ON
     Validate(ai); 
#endif VALIDATE_ON


#ifdef VALIDATE_ON
#ifdef _DEBUG

if (goals_to_delete) {
    Goal *check_me; 

    for (i=0; i<(goals_to_delete->count-1); i++) { 
        check_me = (Goal_ptr) goals_to_delete->Return_Data_By_Number(i);
        for (j=(i+1); j<goals_to_delete->count; j++) { 
            Assert(check_me != (Goal_ptr) goals_to_delete->Return_Data_By_Number(j));
        }
    }
}
#endif _DEBUG
#endif VALIDATE_ON

#ifdef VALIDATE_ON
    the_team->my_goals.Validate(); 
#endif VALIDATE_ON

	
	if ( goals_to_delete isnt NULL)
	{
		
		for (i = 0; i < goals_to_delete->count; i++)
		{
#ifdef VALIDATE_ON
    the_team->my_goals.Validate(); 
#endif VALIDATE_ON
			
			the_goal = (Goal_ptr) goals_to_delete->Return_Data_By_Number(i);
#ifdef VALIDATE_ON
    the_team->my_goals.Validate(); 
#endif VALIDATE_ON

#ifdef DBGAILOG

			#ifdef LOG_SCHEDULER
			if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
			{
				AILOG(( wstr,  "Deleting goal @%x\n", (long int) the_goal));
			}
			#endif
#endif DBGAILOG

			
			if (the_goal isnt NULL)
			{
				
				
#ifdef VALIDATE_ON
    the_team->my_goals.Validate(); 
#endif VALIDATE_ON
                the_goal->Set_Invalid();
#ifdef VALIDATE_ON
    the_team->my_goals.Validate(); 
#endif VALIDATE_ON
			










#ifdef VALIDATE_ON
    the_team->my_goals.Validate(); 
#endif VALIDATE_ON
                the_team->my_goals.Remove_Data_By_Unique_Key(the_goal->goal_ID);
#ifdef VALIDATE_ON
    the_team->my_goals.Validate(); 
#endif VALIDATE_ON
				delete the_goal;
                the_goal = NULL; 

			} 

			else
			{
#ifdef DBGAILOG
				#ifdef LOG_SCHEDULER
				if (ai->AI_VERBOSITY >= LOG_DETAILS)
				{
					AILOG(( wstr,  "the_goal list contains NULL goal\n"));
				}
				#endif
#endif DBGAILOG
			}
#ifdef VALIDATE_ON
     Validate(ai); 
#endif VALIDATE_ON
		} 

		
		goals_to_delete->Empty();

	} 

	
	troop_allocation_done = TRUE;

	
	for (i = 0; i < MAX_GOAL_CLASSES; i++)
	{
		
		sorted_goals_of_class[i]->Empty();
		pruned_goals_of_class[i]->Empty();
			
	} 

	
	for (i = 0; i < MAX_SQUAD_CLASSES; i++)
	{
		
		squads_of_class[i]->Empty();
			
	} 

}












void Scheduler::Add_New_Goal
(
  Goal_ptr my_new_goal
)
{

    Assert(((ArmyGoal*)my_new_goal)->m_XYpos);

	
	Assign_Unique_ID(my_new_goal);

	
	
	if ((goals_inited) 
		and (not troop_allocation_done))
	{
		
		my_new_goal->Classify_Goal();

		
		my_new_goal->Compute_Raw_Priority(ai);

		
		my_new_goal->Compute_Needed_Troop_Flow(ai);

		
		Add_Single_Goal_To_Valid_Goals(my_new_goal);

		
		Add_New_Matches_For_Goal(my_new_goal);

		
		
		if ((my_new_goal->removal_time isnt REMOVE_WHEN_SORTED)
			and (my_new_goal->removal_time isnt REMOVE_WHEN_EXECUTED))
		{
			
			my_new_goal->new_goal_cell_ptr = new_goals->Add_Data( my_new_goal);

		} 

	} 

	
	
	else
	{
		
		my_new_goal->new_goal_cell_ptr = new_goals->Add_Data( my_new_goal);

	} 

#ifdef DBGAILOG
#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
	{
		AILOG(( wstr, "SCHEDULER: Added "));

		
		my_new_goal->Display_Goal_Type(ai);
		my_new_goal->Display_Goal_Details(ai);

		AILOG(( wstr, " Goal # %d   (@%x)\n", my_new_goal->goal_ID, (int) my_new_goal));
	} 
#endif
#endif DBGAILOG

}












void Scheduler::Remove_Goal
(
  Goal_ptr bad_goal
)
{
	
	if (bad_goal->new_goal_cell_ptr != NULL)
	{
#ifdef DBGAILOG
#ifdef LOG_SCHEDULER
		if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		{
			AILOG(( wstr, "SCHEDULER: Removed "));

			
			bad_goal->Display_Goal_Type(ai);
			bad_goal->Display_Goal_Details(ai);

			AILOG(( wstr, " Goal # %d   (@%x)\n", bad_goal->goal_ID, (int) bad_goal));
		} 
#endif
#endif DBGAILOG

		
		new_goals->Remove_Cell( bad_goal->new_goal_cell_ptr);

		
		bad_goal->new_goal_cell_ptr = NULL;

	} 

}












void Scheduler::Remove_Goal_And_Add_To_Delete_List
(
  Goal_ptr bad_goal
)
{
    Assert(bad_goal); 

#ifdef VALIDATE_ON
    Validate(ai); 
#endif VALIDATE_ON

#ifdef _DEBUG
    sint32 bad_id = bad_goal->GetID(); 
    Assert(0 <= bad_id); 
#endif 
    
    Assert(bad_goal); 

	
	
	if (bad_goal->marked_for_deletion) 
	{
#ifdef DBGAILOG
		#ifdef LOG_SCHEDULER
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
		{
			AILOG(( wstr, "SCHEDULER: Asked to DELETE goal multiple times "));

			
			bad_goal->Display_Goal_Type(ai);
			bad_goal->Display_Goal_Details(ai);

			AILOG(( wstr, " Goal # %d   (@%x)\n", bad_goal->goal_ID, (int) bad_goal));
		} 
		#endif
#endif DBGAILOG

#ifdef VALIDATE_ON
	Validate(ai); 
#endif

		return;
	}

	
	bad_goal->marked_for_deletion = true;

	
	Remove_Goal(bad_goal);

#ifdef VALIDATE_ON
#ifdef _DEBUG
    sint32 i;
    Goal *the_goal; 
    for (i=0; i<new_goals->count; i++) { 
       the_goal = (Goal *) new_goals->Return_Data_By_Number( i );
       Assert(bad_id != the_goal->GetID()); 
    }
#endif
#endif


#ifdef DBGAILOG
#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
	{
		AILOG(( wstr, "SCHEDULER: Adding to delete list "));

		
		bad_goal->Display_Goal_Type(ai);
		bad_goal->Display_Goal_Details(ai);

		AILOG(( wstr, " Goal # %d   (@%x)\n", bad_goal->goal_ID, (int) bad_goal));
	} 
#endif
#endif DBGAILOG


	
	goals_to_delete->Add_Data( bad_goal);

#ifdef VALIDATE_ON
	Validate(ai); 
#endif VALIDATE_ON

}














Goal_ptr Scheduler::Return_Goal_By_Number
(
  int n
)
{
	
	Goal_ptr nth_goal;
	

	
	nth_goal = (Goal_ptr) new_goals->Return_Data_By_Number(n);

	return nth_goal;
}













void Scheduler::Update_Goals()
{
	
	int i;								
	Goal_ptr the_goal;
	int complete;						
	

#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		AILOG(( wstr,  "\n\nUpdating new goals %d\n", new_goals->count));
	}
	#endif
#endif DBGAILOG

	
	for (i = new_goals->count-1; i >= 0; i --)
	{
#ifdef GRAPHICS_CALLBACK
		
		if (!(i%k_CALLBACK_CHECK_FREQUENCY) && TimeToUpdateGraphics()) {
			ai->m_gs->ProcessGraphicsCallback();
		}
#endif

		
		the_goal = (Goal_ptr)
			new_goals->Return_Data_By_Number(  i );

		
		if (the_goal->removal_time is REMOVE_WHEN_FULFILLED)
		{
			
			complete = the_goal->Get_Totally_Complete(ai);

			
			if (complete)
			{
#ifdef DBGAILOG
				#ifdef LOG_SCHEDULER
				if (ai->AI_VERBOSITY >= LOG_HEADERS)
				{
					AILOG(( wstr,  "COMPLETE GOAL (#%d) ", the_goal->goal_ID));
					the_goal->Display_Goal_Type(ai);
					the_goal->Display_Goal_Details(ai);
					AILOG(( wstr,  "\n\n"));
				}
				#endif
#endif DBGAILOG

				
                if (the_goal->attacking_squad) { 
                    Squad *s = the_goal->attacking_squad; 
                    Assert(s); 
                    s->Detach_Goal(); 
                }
				Remove_Goal_And_Add_To_Delete_List(the_goal);

			} 

		} 

	} 

}















int Scheduler::Add_New_Matches_For_Squad
(
	squad_ptr the_squad,
	Goal_ptr ignore_goal				
)

{
	
	int j, k;
	Goal_ptr the_goal=NULL;
	Plan_ptr the_match=NULL;
	int matches_handled = 0;			
	double beat_this_priority;			
										
	GOAL_CLASS goal_class_for_squad;	
	list_array<Goal_ptr> *pruned_goal_class;	
	

	

	if (the_squad->Still_Can_Act() < 1) return matches_handled; 

	
	if ((not the_squad->already_committed) and (the_squad->Still_Can_Act() > 0))
	{
		
		
		if (the_squad->my_strategic_goal isnt NULL)
			beat_this_priority = the_squad->my_strategic_goal->raw_priority;
		
		else beat_this_priority = BAD_UTILITY;

		
		for (j = 0; j < appropriate_goal_classes[the_squad->squad_type]->count; j++)
		{
			
			goal_class_for_squad = 
				appropriate_goal_classes[the_squad->squad_type]->Return_Data_By_Number(j);

			
			pruned_goal_class = pruned_goals_of_class[goal_class_for_squad];

			
			
			for (k = 0; k < pruned_goal_class->count; k++)
			{
				
				the_goal = pruned_goal_class->Return_Data_By_Number(k);

				Assert(the_goal); 

				
				if ((the_goal is ignore_goal) ||
					 the_goal->Get_Totally_Complete(ai) ||
					!the_goal->CanBeExecuted())
					continue; 

				
				
				
				if (beat_this_priority > the_goal->raw_priority)
					
					break;

				
				if (Add_New_Match_For_Goal_And_Squad(the_goal, the_squad))
					matches_handled++;

			} 

		} 

	} 


	
	return matches_handled;
}













void Scheduler::Add_Single_Plan(Agent *the_agent, Goal *the_goal)
{

	
	if (the_agent->the_squad is NULL) return;

	
	the_goal->Classify_Goal();

	
	the_goal->Compute_Raw_Priority(ai);

	
	the_goal->Compute_Needed_Troop_Flow(ai);

	
	Add_New_Match_For_Goal_And_Squad(the_goal, the_agent->the_squad);

#ifdef DBGAILOG
#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
	{
		AILOG(( wstr, "\nSCHEDULER: Added single PLAN.\n    "));

		
		the_goal->Display_Goal_Type(ai);
		the_goal->Display_Goal_Details(ai);

		AILOG(( wstr, " Goal # %d   (@%x)  Squad #%d\n", 
			the_goal->goal_ID, (int) the_goal, 
			the_agent->the_squad->squad_ID));

		the_agent->the_squad->Display_Composition(ai);


	} 
#endif
#endif DBGAILOG

	
	
	goals_to_delete->Add_Data( the_goal);

}













void Scheduler::Assign_Unique_ID
(
  Goal_ptr the_goal
)
{
	
	if (the_goal->goal_ID is UNASSIGNED_ID)
	{
		
		the_goal->goal_ID = unique_goal_ID;

		
		unique_goal_ID++;

	} 
   
}



















int Scheduler::Prepare_Goals(AiMain *ai)


{
	
	int i, j;
	Goal_ptr the_goal;
	double goals_raw_priority;
	sorted_array<Goal_ptr,double> *a_goal_class;	
	bool added_OK;					
	

	
	if (goals_inited)
	{
		return TIME_SLICE_NOT_DONE;
	} 

#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		AILOG(( wstr,  "\n\nPreparing Goals %d\n", new_goals->count));
	}
	#endif
#endif DBGAILOG

	
	min_priority = A_ZILLION;
	max_priority = A_REALLY_BIG_NEGATIVE_REAL_NUMBER;

	
	for ( i = 0 ; i < GOAL_TYPE_MAX ; i++)
	{
		
		goal_count[i] = 0;

		
		exec_goal_count[i] = 0;

		
		last_best_goal_of_type[i] = 0;

	} 


	
	
	total_goal_count = new_goals->count;

	
	
	for (i = new_goals->count-1; i >= 0; i --)
	{
#ifdef GRAPHICS_CALLBACK
		
		if (!(i%k_CALLBACK_CHECK_FREQUENCY) && TimeToUpdateGraphics()) {
			ai->m_gs->ProcessGraphicsCallback();
		}
#endif
		
		the_goal = (Goal_ptr)
			new_goals->Return_Data_By_Number(  i );

		
		the_goal->Classify_Goal();


		
		
		the_goal->ever_appropriate = false;

		
		the_goal->survived_pruning = false;


		
		the_goal->raw_priority = (float) 0;

		
		if (the_goal->what_goal isnt GOAL_TYPE_NULL)
		{
			
			goals_raw_priority = the_goal->Compute_Raw_Priority(ai);

			
			if (goals_raw_priority > (BAD_UTILITY + 0.5))
			{
				
				if (the_goal->what_goal isnt GOAL_TYPE_NULL)
				{
					
					the_goal->Compute_Needed_Troop_Flow(ai);


					
					sorted_goals_of_class[the_goal->the_class]->Add_Data(
						the_goal, goals_raw_priority);
	#ifdef DBGAILOG
					#ifdef LOG_SCHEDULER
					if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
					{
						AILOG(( wstr,  
							"Adding goal into goal class %d sorted list (#%d) ", 
							the_goal->the_class, the_goal->goal_ID));
						the_goal->Display_Goal_Type(ai);
						the_goal->Display_Goal_Details(ai);
						AILOG(( wstr,  "\n"));
					}
					#endif
	#endif DBGAILOG

				} 

			} 

		} 

		
		
		the_goal->dont_allocate = false;

		
		if ((REMOVE_WHEN_SORTED == the_goal->removal_time) || 
            (REMOVE_WHEN_EXECUTED == the_goal->removal_time))
		{
			
			Remove_Goal(the_goal);

		} 

	} 

	
#ifdef DBGAILOG
#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		AILOG((wstr, "\nALL GOALS\n\n"));
#endif
#endif

	
	for (j = 0; j < MAX_GOAL_CLASSES; j++)
	{

#ifdef DBGAILOG
#ifdef LOG_SCHEDULER
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "\nCLASS %d\n\n", j));
#endif
#endif

		
		a_goal_class = sorted_goals_of_class[j];
		
		
		for (i=a_goal_class->count-1; i>=0; i--)
		{
			
			the_goal = (Goal_ptr) a_goal_class->Return_Data_By_Number(i);

			
			added_OK = Add_Single_Goal_To_Valid_Goals(the_goal);

			
			if (added_OK)
			{
				
				
				last_best_goal_of_type[the_goal->what_goal] = i;

#ifdef DBGAILOG
#ifdef LOG_SCHEDULER
				
				if (ai->AI_VERBOSITY >= LOG_DETAILS)
				{
					
					the_goal->Display_Goal_Type(ai);
					the_goal->Display_Goal_Details(ai);

					AILOG(( wstr,  "    VALID GOAL #%d %d........P: %f\n",
						the_goal->goal_ID, the_goal->what_goal,
						the_goal->raw_priority)); 

				} 

				
				
#endif
#endif
			} 

		} 

	} 

	
	goals_inited = TRUE;

	return TIME_SLICE_DONE;
}














void Scheduler::Prepare_Cities(AiMain *ai)
{
	
    CityAgent *the_city = NULL; 
    BSetID id; 
	

	
	for ( the_city = ai->m_city_set->First(id); 
		  ai->m_city_set->Last(); 
		  the_city = ai->m_city_set->Next(id) )
	{
		
		
		if (the_city->GetQueueLen() < 1)
		{
			
			the_city->current_goal_type = GOAL_TYPE_NULL;

		} 
		
		
		
		else
		{
			
			
			

			
			
			
			if (the_city->current_goal_type != GOAL_TYPE_DEFENSE)
			{
				
				exec_goal_count[the_city->current_goal_type]++;
			}
		} 


	} 

}
















int Scheduler::Prepare_Squads(AiMain *ai)

{
	
	int i;
	squad_ptr the_squad;

	

	
	if (squads_inited)
	{
		return TIME_SLICE_NOT_DONE;
	} 

#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		AILOG(( wstr,  "\n\nPreparing all squads for scheduling\n"));
	}
	#endif
#endif DBGAILOG

	
	
	Prepare_Cities(ai);

	sint32 total_transports = 0;
	double total_attack = 0.0;
	double total_defense = 0.0;

	
	for (i = 0; i < the_team->my_squads->count; i++)
	{
		
		the_squad = (squad_ptr)
			the_team->my_squads->Return_Data_By_Number( i );

		
        the_squad->auxiliary_candidate = TRUE; 

		
		total_transports += the_squad->GetUsableTransport(ai);

		
		total_defense += the_squad->GetUsableDefense(ai);

		
		total_attack += the_squad->GetUsableAttack(ai);

		
		the_squad->Compute_Location();

#ifdef _DEBUG
		
		the_squad->matched_to_any_goal = false;
#endif


		
		the_squad->already_committed = FALSE;

		
		the_squad->resend = TRUE;

		
		the_squad->ignore_goals.Empty();

		
		squads_of_class[the_squad->squad_type]->Append_Data(
			the_squad);

#ifdef DBGAILOG
		#ifdef LOG_SCHEDULER
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
		{
			if (the_squad->my_strategic_goal is NULL)
			{
				#ifdef _DEBUG
				AILOG(( wstr,  "\nSQUAD %d...U: %d\n",
				the_squad->squad_ID,
				the_squad->my_agents.count));
				#else
				AILOG(( wstr,  "\nSQUAD ...U: %d\n",
				the_squad->my_agents.count));
				#endif 

				the_squad->Display_Squad_Type(ai);
			}

			else
			{
				#ifdef _DEBUG
				AILOG(( wstr,  "\nSQUAD %d...U: %d   ",
				the_squad->squad_ID,
				the_squad->my_agents.count));
				#else
				AILOG(( wstr,  "\nSQUAD ...U: %d   ",
				the_squad->my_agents.count));
				#endif 

				the_squad->Display_Squad_Type(ai);

				
#ifdef DBGAILOG
				the_squad->my_strategic_goal->Display_Goal_Type(ai);
				the_squad->my_strategic_goal->Display_Goal_Details(ai);
#endif DBGAILOG

				AILOG(( wstr, "\n" ));
			}

			#ifdef LOG_SCHEDULER
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
			{
				
				the_squad->Display_Composition(ai);
			}
			#endif


		}

		#endif
#endif DBGAILOG


	} 

	
	squads_inited = TRUE;

	the_team->SetUsableTransport(total_transports);
	the_team->SetUsableDefense(total_defense);
	the_team->SetUsableAttack(total_attack);

	return TIME_SLICE_DONE;

}















bool Scheduler::Add_Single_Goal_To_Valid_Goals
(
	Goal_ptr the_goal					
)
{
	
	int unsatisfied_found = FALSE;
	int satisfied_found = FALSE;
	int satisfied;
	

    Assert(the_goal);  
    Assert(GOAL_TYPE_NULL < the_goal->what_goal); 
    Assert(GOAL_TYPE_MAX > the_goal->what_goal); 

	
	if ((the_team->the_AIP.max_eval_goals_of_type[the_goal->what_goal] 
			isnt UNLIMITED_GOALS)
		and
		(goal_count[the_goal->what_goal] 
			>= the_team->the_AIP.max_eval_goals_of_type[the_goal->what_goal])
		)
	{

		
		if (the_goal->attacking_squad isnt NULL)
		{
			
			the_goal->attacking_squad->Detach_Goal();

		} 

	} 

	else
	{

		
		goal_count[the_goal->what_goal]++;

		




		satisfied = false;

		
		if (satisfied)
		{
			
			if (the_goal->attacking_squad isnt NULL)
				the_goal->attacking_squad->already_committed = TRUE;

			


		} 
		else
		{
			
			unsatisfied_found = TRUE;

			
			
			
			if (the_goal->attacking_squad isnt NULL)
			{
				the_goal->attacking_squad->Detach_Goal();
			}

			


			
			
			pruned_goals_of_class[the_goal->the_class]->Append_Data(the_goal);

			
			the_goal->survived_pruning = true;

			
			return true;

		} 

	} 

	
	return false;
}













bool Scheduler::Add_New_Match_For_Goal_And_Squad
(
	Goal_ptr the_goal,					
	squad_ptr the_squad					
)
{
	
	double temp, matching_value;
	Plan_ptr the_match=NULL;
	int i;
	Goal_ptr ignore_me_goal;			
#ifdef PLANS_IN_SORTED_ARRAY
	int where_in_sortlist;				
#endif 

#ifdef PLANS_IN_RED_BLACK_TREE
	Red_Black_Tree_Node<Plan *,Plan_Sort_Key> *where_in_match_tree; 
#endif 


	

	Assert(the_goal);
	Assert(the_squad);
#ifdef GRAPHICS_CALLBACK
	
	static uint32 anmfgas_iterations = 0;

	anmfgas_iterations++;

	
	if (!(anmfgas_iterations % k_CALLBACK_CHECK_FREQUENCY) && TimeToUpdateGraphics()) {
		ai->m_gs->ProcessGraphicsCallback();
	}
#endif

	
	
	if (the_goal->attacking_squad is the_squad) return false;

	
	if ((the_goal->what_goal != GOAL_TYPE_DEFENSE) &&
		the_goal->m_current_needed_strength->GetAttack() >
		the_team->GetUsableAttack())
		{
#ifdef DBGAILOG
#ifdef LOG_SCHEDULER
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
				{
					AILOG(( wstr,  " \tNot enough usable attack to add matches for goal: %d -- ", the_goal->goal_ID));
					AILOG(( wstr,  " \t  Total team attack strength = %5.0f, needed = %5.0f ", 
						the_team->GetUsableAttack(),
						the_goal->m_current_needed_strength->GetAttack()));
					the_goal->Display_Goal_Type(ai);
					AILOG(( wstr,  " \n"));
				}
#endif
#endif DBGAILOG
			return FALSE;
		}

	
	if ((not the_squad->already_committed) and (the_squad->Still_Can_Act() > 0))
	{
		
		if ((exec_goal_count[the_goal->what_goal] 
			 >= the_team->the_AIP.max_execute_goals_of_type[the_goal->what_goal]) &&
			(the_team->the_AIP.max_execute_goals_of_type[the_goal->what_goal] 
			 != UNLIMITED_GOALS))
			{
				#ifdef DBGAILOG
				#ifdef LOG_SCHEDULER
				if (ai->AI_VERBOSITY >= LOG_DETAILS)
					{
						AILOG(( wstr, "Add_New_Matches_For_Goal_And_Squad: "));
						AILOG(( wstr, "MAX EXECUTABLE GOALS OF TYPE EXCEEDED\n"));
						the_goal->Display_Goal_Type(ai);
						AILOG(( wstr,  " \n"));
					}
				#endif
				#endif DBGAILOG

				return FALSE;
			}

		
		for (i = 0; i < the_squad->ignore_goals.count; i++)
		{
			
			ignore_me_goal = (Goal_ptr) the_squad->ignore_goals.Return_Data_By_Number(i);

			
			if (the_goal is ignore_me_goal) 
				{
					#ifdef DBGAILOG
					#ifdef LOG_SCHEDULER
					if (ai->AI_VERBOSITY >= LOG_DETAILS)
						{
							AILOG(( wstr, "Add_New_Matches_For_Goal_And_Squad: "));
							AILOG(( wstr, "Goal is ignorable; not added.\n"));
							the_goal->Display_Goal_Type(ai);
							AILOG(( wstr,  " \n"));
						}
					#endif
					#endif DBGAILOG
					return FALSE;
				}

		} 

		

		temp = (float)
			the_goal->Compute_Matching_Value(ai, the_squad );

		
		matching_value = temp;

		
		if (matching_value > (BAD_UTILITY + 0.5))
		{
			
			
			matching_value = floor(matching_value) + ((double) unique_plan_ID/(double)MAX_PLAN_IDS);

			
			if (matching_value < min_priority)
				min_priority = matching_value;

			if (matching_value > max_priority)
				max_priority = (int) matching_value;

			
			the_match = g_plan_pool->Get_Next_Pointer();


			
			the_match->Init();

			
			the_match->the_squad = the_squad;

			the_match->the_goal = the_goal;
			the_match->matching_value = matching_value;

			
			the_match->used = FALSE; 

			
			a_plan_sort_key.value = matching_value;
			a_plan_sort_key.ID = unique_plan_ID;




			
			unique_plan_ID++;

			
#ifdef PLANS_IN_SORTED_ARRAY	
			where_in_sortlist =
				the_matches->Add_Data( the_match, matching_value);
#endif 

#ifdef PLANS_IN_RED_BLACK_TREE
			where_in_match_tree =
				match_tree->Insert(the_match, a_plan_sort_key);

			
			match_tree->Choose_Highest_Traversal_Element(where_in_match_tree);
#endif 

#ifdef PLANS_IN_SORTED_ARRAY	
			
			
			
			where_in_sortlist++;

			
			
            if (where_in_sortlist > pos_in_match_list) { 
				#ifdef DO_WALLCLOCK
					if (g_wall_clock &&
						where_in_sortlist > 0 &&
						pos_in_match_list > 0) 
					{ 
						g_wall_clock->AddNewMatches(where_in_sortlist - pos_in_match_list,
							(sint32) the_match->the_goal->what_goal);
					}
				#endif
				#ifdef DBGAILOG
				#ifdef LOG_SCHEDULER
				if (ai->AI_VERBOSITY >= LOG_DETAILS &&
					where_in_sortlist - pos_in_match_list > 10)
				{
					AILOG(( wstr,  " \tMoved position in sort list up by %d for goal: %d\n", 
						where_in_sortlist - pos_in_match_list, 
						the_match->the_goal->goal_ID));
					the_match->the_goal->Display_Goal_Type(ai);
					AILOG(( wstr,  " \n"));
				}
				#endif
				#endif DBGAILOG

				pos_in_match_list = where_in_sortlist;
            } else { 
                pos_in_match_list++; 
            } 
#endif 

			
			
			the_match->the_goal->ever_appropriate = true;

#ifdef _DEBUG
			the_squad->matched_to_any_goal = true;
#endif

#ifdef DBGAILOG
			#ifdef LOG_SCHEDULER
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
			{
				AILOG(( wstr,  "ALLOCATED match @%x   P:%g  Squad: %d  Goal: %d ", 
					(long int) the_match, matching_value, the_squad->squad_ID,
					the_match->the_goal->goal_ID));

				the_match->the_goal->Display_Goal_Type(ai);
				the_match->the_goal->Display_Goal_Details(ai);

				AILOG(( wstr, "\n"));
			}
			#endif
#endif DBGAILOG

			
			return true;

		} 

	} 

	
	return false;
}















int Scheduler::Add_New_Matches_For_Goal
(
	Goal_ptr the_goal					
)

{
	
	int  j, k;
	squad_ptr the_squad=NULL;
	int matches_handled = 0;			
	SQUAD_CLASS squad_class_for_goal;	
										
	list_array<Squad *> *a_squad_class;		
	

	
	for (j = 0; j < appropriate_squad_classes[the_goal->the_class]->count; j++)
	{
		
		squad_class_for_goal = 
			appropriate_squad_classes[the_goal->the_class]->Return_Data_By_Number(j);

		
		a_squad_class = squads_of_class[squad_class_for_goal];

		
		
		for (k = 0; k < a_squad_class->count; k++)
		{
			
			the_squad = a_squad_class->Return_Data_By_Number(k);

			Assert(the_squad); 

			
			if (Add_New_Match_For_Goal_And_Squad(the_goal, the_squad))
				matches_handled++;

		} 

	} 

	
	return matches_handled;
}













int Scheduler::Init_Match_Lists()

{
	
	int matches_count;                  
	

	
	if (precompute_inited)
	{
		return TIME_SLICE_NOT_DONE;
	}

#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		AILOG(( wstr,  "Initializing the match lists\n\n"));
	}
	#endif
#endif DBGAILOG

	
	matches_count = the_team->my_squads->count * total_goal_count;

	
	if (matches_count is 0)
	{

		
		precompute_inited = TRUE;

		
		return TIME_SLICE_DONE;
	}

	
	unique_plan_ID = 0;

#ifdef PLANS_IN_SORTED_ARRAY	
	
	the_matches->Make_Room_For_N_Elements( matches_count);
#endif 

	
	precompute_squad_number = 0;

	
	precompute_inited = TRUE;

    
    the_team->is_there_new_goals = FALSE; 
    the_team->is_there_new_squads = FALSE; 

    

#ifdef PLANS_IN_RED_BLACK_TREE
		if (match_tree is NULL)
#endif 

#ifdef PLANS_IN_SORTED_ARRAY	
		if (the_matches is NULL)
#endif 

	{
		
		
		
		schedule_matching_done = TRUE;
	}

	
	return TIME_SLICE_DONE;

}

















void Scheduler::Generate_Matches_And_Valid_Goal_List()

{
	
	int i, j;
	squad_ptr the_squad=NULL;
	Goal_ptr the_goal=NULL;
	sorted_array<Goal_ptr,double> *a_goal_class;	
	int matches_handled;				
	

	
	if (subscheduling_inited)
	{
		return;
	} 

#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		AILOG(( wstr,  "Generate_Matches_And_Valid_Goal_List\n\n"));
	}
	#endif
#endif DBGAILOG

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

	
	for (i = 0; i < the_team->my_squads->count; i++)
	{
		
		the_squad = (squad_ptr) the_team->my_squads->Return_Data_By_Number(i);

		
		Add_New_Matches_For_Squad(the_squad);

	} 

	
	
	
	
	
	
	
	
	
	for (j = 0; j < MAX_GOAL_CLASSES; j++)
	{
		
		a_goal_class = sorted_goals_of_class[j];
		
		
		for (i=a_goal_class->count-1; i>=0; i--)
		{
			
			the_goal = (Goal_ptr) a_goal_class->Return_Data_By_Number(i);

			
			if (the_goal->survived_pruning)
			{
				
				if (not the_goal->ever_appropriate)
				{
					
					
					goal_count[the_goal->what_goal]--;

				} 

			} 

			
			
			else
			{
				
				if (goal_count[the_goal->what_goal]
					< the_team->the_AIP.max_eval_goals_of_type[the_goal->what_goal])
				{
					
					matches_handled = Add_New_Matches_For_Goal(the_goal);

					
					if (matches_handled > 0)
					{
						
						
						Add_Single_Goal_To_Valid_Goals(the_goal);

					} 

				} 

			} 

		} 

	} 


#ifdef PLANS_IN_SORTED_ARRAY	
	
	pos_in_match_list = the_matches->count - 1;
#endif 

#ifdef PLANS_IN_RED_BLACK_TREE
	
	match_tree->Initialize_Reverse_Order_Tree_Traversal();
#endif 

#ifdef PLANS_IN_SORTED_ARRAY	
	#ifdef DO_WALLCLOCK
		if (g_wall_clock &&
			pos_in_match_list > 0) 
		{ 
			Plan *the_match;
			for (int i = 0; i < the_matches->count; i++)
			{
				the_match = the_matches->Return_Data_By_Number(i);
				g_wall_clock->AddTotalMatches(1, (sint32) the_match->the_goal->what_goal);
			}
		}
	#endif
#endif 

	
	subscheduling_inited = TRUE;

	

#ifdef _DEBUG
	
	for (j = 0; j < the_team->my_squads->count; j++)
	{
		
		the_squad = (squad_ptr) the_team->my_squads->Return_Data_By_Number(j);

		
#ifdef DBGAILOG
		#ifdef LOG_SCHEDULER
		if ((ai->AI_VERBOSITY >= LOG_DETAILS) 
			and (not the_squad->matched_to_any_goal))
		{
			AILOG(( wstr,  "----->NO MATCHES FOR Squad: %d\n", 
				the_squad->squad_ID));
		}
		#endif 
#endif 
	}
#endif 



#ifdef DO_WALLCLOCK
  LogGoalCount(ai);
#endif

}





sint32 g_log_goal_count; 

void Scheduler::LogGoalCount(AiMain *ai)
{
    
    sint32 i;
    FILE *fout=NULL; 

if (ai->m_my_player_id != 2) return; 

    if (!g_log_goal_count) { 
        fout = fopen("logs\\goalcount.txt", "w"); 
        Assert(fout); 
        fprintf (fout, "GOAL_TYPE_NULL, ");
        fprintf (fout, "GOAL_TYPE_DEFENSE, ");
	    fprintf (fout, "GOAL_TYPE_SEIGE, ");
	    fprintf (fout, "GOAL_TYPE_ATTACK_REGION, ");
        fprintf (fout, "GOAL_TYPE_ATTACK, ");
        fprintf (fout, "GOAL_TYPE_SETTLE, ");
        fprintf (fout, "GOAL_TYPE_EXPLORE, ");
        fprintf (fout, "GOAL_TYPE_CONSTRUCT_BUILDING, "); 
        fprintf (fout, "GOAL_TYPE_CONSTRUCT_FREIGHT, ");
	    fprintf (fout, "GOAL_TYPE_CONSTRUCT_WONDER, ");
        fprintf (fout, "GOAL_TYPE_TRANSPORT, ");
        fprintf (fout, "GOAL_TYPE_ENSLAVE,  ");
	    fprintf (fout, "GOAL_TYPE_BUILD_SUPPLEMENTAL, ");
	    fprintf (fout, "GOAL_TYPE_BUILD_LIST, ");
	    fprintf (fout, "GOAL_TYPE_PERIMETER, ");
	    fprintf (fout, "GOAL_TYPE_CHOKEPOINT, ");
	    fprintf (fout, "GOAL_TYPE_PATROL, ");
	    fprintf (fout, "GOAL_TYPE_CONVERT_CITY, ");
        fprintf (fout, "GOAL_TYPE_BIOTERROR_CITY, "); 
        fprintf (fout, "GOAL_TYPE_NANOATTACK, "); 
	    fprintf (fout, "GOAL_TYPE_HARASS_CITY, ");
	    fprintf (fout, "GOAL_TYPE_BOMBARD, ");
	    fprintf (fout, "GOAL_TYPE_RUSTLE, ");
	    fprintf (fout, "GOAL_TYPE_EXPEL, ");
	    fprintf (fout, "GOAL_TYPE_RETREAT, ");
        fprintf (fout, "GOAL_TYPE_HURT_ARMY,  ");

	    fprintf (fout, "GOAL_TYPE_PILLAGE, ");
	    fprintf (fout, "GOAL_TYPE_PIRATE, ");
	    fprintf (fout, "GOAL_TYPE_COUNTER_STEALTH, ");
	    fprintf (fout, "GOAL_TYPE_BONUS_FOOD, ");
	    fprintf (fout, "GOAL_TYPE_GOODY_HUT, ");
	    fprintf (fout, "GOAL_TYPE_PLANT_NUKE, ");
	    fprintf (fout, "GOAL_TYPE_MAKE_PARK, ");
	    fprintf (fout, "GOAL_TYPE_CAUSE_UNHAPPINESS, ");
	    fprintf (fout, "GOAL_TYPE_ESTABLISH_EMBASSY, ");
	    fprintf (fout, "GOAL_TYPE_FRANCHISING, ");
	    fprintf (fout, "GOAL_TYPE_STEAL_TECHNOLOGY, ");
	    fprintf (fout, "GOAL_TYPE_INCITE_REVOLUTION, ");
	    fprintf (fout, "GOAL_TYPE_INJOIN, ");
	    fprintf (fout, "GOAL_TYPE_ASSASINATE_RULER, ");
	    fprintf (fout, "GOAL_TYPE_UNDERGROUND_RAILWAY, ");
	    fprintf (fout, "GOAL_TYPE_NUKE_CITY, ");
	    fprintf (fout, "GOAL_TYPE_WANDER, ");
	    fprintf (fout, "GOAL_TYPE_SALLY, ");
	    fprintf (fout, "GOAL_TYPE_MAX,  ");
        fprintf(fout, "total\n");
        g_log_goal_count=1; 
    } else { 
        fout = fopen("logs\\goalcount.txt", "a"); 
        Assert(fout); 
    }

    sint32 j;

    sint32 *count = new sint32 [GOAL_TYPE_MAX]; 
    for (j=0; j<GOAL_TYPE_MAX; j++) { 
        count[j] = 0; 
    } 

    sint32 total = 0; 
    sorted_array<Goal_ptr,double> * a_goal_class; 

    Goal * the_goal=NULL;
    for (i = 0; i < MAX_GOAL_CLASSES; i++)	{
		a_goal_class = sorted_goals_of_class[i];		
        for (j=0; j<a_goal_class->count; j++) { 
			the_goal = (Goal*) a_goal_class->Return_Data_By_Number(j);
            count[the_goal->what_goal]++; 
            total++; 
        }
    }

    for (i = 0; i < GOAL_TYPE_MAX; i++)	{
        fprintf (fout, "%d, ", count[i]); 
    } 

    fprintf (fout, "%d\n", total); 

    fclose(fout); 
    delete[] count; 
    count = NULL; 
}

#if 0 


void Scheduler::LogGoalCount(AiMain *ai)
{
    
    sint32 i;
    FILE *fout=NULL; 

if (ai->m_my_player_id != 2) return; 

    if (!g_log_goal_count) { 
        fout = fopen("logs\\goalcount.txt", "w"); 
        Assert(fout); 
        fprintf(fout, "DEFAULT_GOAL_CLASS, ");
	    fprintf(fout, "SETTLER_GOAL_CLASS, ");
	    fprintf(fout, "CITY_GOAL_CLASS, ");
	    fprintf(fout, "EXPLORE_GOAL_CLASS, ");
	    fprintf(fout, "BOMBARD_GOAL_CLASS, ");
        fprintf(fout, "SEIGE_GOAL_CLASS, ");
	    fprintf(fout, "PROTECT_GOAL_CLASS, ");
	    fprintf(fout, "DEFENSE_GOAL_CLASS, ");
	    fprintf(fout, "MILITARY_GOAL_CLASS, ");
	    fprintf(fout, "SLAVER_GOAL_CLASS, ");
	    fprintf(fout, "GOD_GOAL_CLASS, ");
	    fprintf(fout, "ESPIONAGE_GOAL_CLASS, ");
	    fprintf(fout, "DEFUSE_MINES_GOAL_CLASS, ");
	    fprintf(fout, "BONUS_FOOD_GOAL_CLASS, ");
	    fprintf(fout, "MEDIA_GOAL_CLASS, ");
	    fprintf(fout, "DIPLOMATIC_GOAL_CLASS, ");
	    fprintf(fout, "BUSINESS_GOAL_CLASS, ");
	    fprintf(fout, "LAW_GOAL_CLASS, ");
	    fprintf(fout, "ABOLITIONIST_GOAL_CLASS,  ");
	    fprintf(fout, "DESTROY_CITY_GOAL_CLASS, ");
	    fprintf(fout, "PROMISCUOUS_GOAL_CLASS, ");
        fprintf(fout, "total\n");
        g_log_goal_count=1; 
    } else { 
        fout = fopen("logs\\goalcount.txt", "a"); 
        Assert(fout); 
    }

    sint32 j;

    sint32 count = new sint32 [GOAL_TYPE_MAX]; 
    for (j=0; j<GOAL_TYPE_MAX; j++) { 
        count[j] = 0; 
    } 

    sint32 total = 0; 
    sorted_array<Goal_ptr,double> * a_goal_class; 


    for (j = 0; j < MAX_GOAL_CLASSES; j++)
	{
		a_goal_class = sorted_goals_of_class[j];
		
		
		fprintf (fout, "%d, ", a_goal_class->count); 
        total += a_goal_class->count; 
    }

    fprintf (fout, "%d\n", total); 

    fclose(fout); 
}

#endif




void Scheduler::DumpGoalToMap(const GOAL_TYPE show_me) 
{ 
    Goal_ptr a_goal=NULL; 
    GoalSettle *s_goal=NULL; 
    MapPointData pos; 
    sint32 i;



    if (new_goals)  { 
	    for (i = 0; i < new_goals->count; i++ ){
		    a_goal = (Goal_ptr) new_goals->Return_Data_By_Number(i);
            if (a_goal->what_goal == show_me) { 
                switch (show_me) { 
                case GOAL_TYPE_SETTLE: 
                    s_goal = (GoalSettle *)a_goal; 
                    s_goal->GetPos( pos); 

                    break; 
                default:
                    INSANE(DUMP_UNKNOWN_GOAL);
                    break; 
                } 
            }
        }
    }       
}














Goal_Result Scheduler::Execute_One_Task
(
    Plan *the_plan, 	
	sint32 &troops_used_up,					
    const NEEDS_AUXILIARY retry,
	squad_ptr original_squad	
)
{
	
	squad_ptr the_squad=NULL;
	Goal_Result task_result;
	Goal_Result aux_result = GOAL_CANT_REACH_TARGET;
	int squad_agent_count = 0;			
	int complete;						
	sint32 prev_usable_transport;		
	double prev_usable_defense;		
	double prev_usable_attack;		
	sint32 used_transport;				
	double used_defense;				
	double used_attack;				
	

    Goal* the_goal = the_plan->the_goal;

	
	the_squad = the_goal->attacking_squad;

	
	Assert(the_squad isnt NULL);

	
	squad_agent_count = the_squad->Still_Can_Act();

#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
	{
		AILOG(( wstr,  "\n\nExecute_One_Task: Using Squad %d for ", 
				the_squad->squad_ID));
		AILOG(( wstr,  "Usable agent count = %d\n", squad_agent_count));

		
		the_goal->Display_Goal_Type(ai);
		the_goal->Display_Goal_Details(ai);

		AILOG(( wstr,  "\n"));
		
		
		the_squad->Display_Composition(ai);
	}
	#endif
#endif DBGAILOG

	
	troops_used_up = 0;

	
#ifdef _DEBUG
    Assert(the_squad); 
    Assert(the_squad->my_strategic_goal); 
    Assert(the_plan); 
    the_squad->DumpGoal(ai, the_squad->my_strategic_goal->what_goal, the_plan->matching_value); 
#endif

#ifdef SUPER_DEBUG_SPEED
    if (super_debug_speed)
		return GOAL_COMPLETE;
#endif 


#ifdef DO_WALLCLOCK
    if (ACCUMULATE_AUXILIARY == retry) { 
        if (g_wall_clock) { 

        }
    }
#endif

	
	if (the_squad->GetUnitCount() < 1)
		task_result = GOAL_INAPPROPRIATE;
	else
		{
			
			prev_usable_transport = the_squad->GetUsableTransport(ai);

			
			prev_usable_defense = the_squad->GetUsableDefense(ai);

			
			prev_usable_attack = the_squad->GetUsableAttack(ai);

			
			
			task_result = the_goal->Execute_Task(ai, the_plan);
			
			

			
			used_transport = prev_usable_transport - the_squad->GetUsableTransport(ai);

			
			used_defense = prev_usable_defense - the_squad->GetUsableDefense(ai);

			
			used_attack = prev_usable_attack - the_squad->GetUsableAttack(ai);

			
			the_team->SetUsableTransport(
				the_team->GetUsableTransport() - used_transport);

			
			the_team->SetUsableDefense(
				the_team->GetUsableDefense() - used_defense);

			
			the_team->SetUsableAttack(
				the_team->GetUsableAttack() - used_attack);
		}


#ifdef DO_WALLCLOCK
    if (ACCUMULATE_AUXILIARY == retry) { 
        if (g_wall_clock) { 

        }
    }
#endif
    
    if (ai->m_i_am_dead){ 
        return task_result; 	
    } 

    
	if (the_goal->single_plan_only)
	{
		return task_result;

	} 

    the_squad->Remove_Dead_Agents(); 

	
	the_squad->Recompute_Squad_Class(ai); 

	
	if (the_squad->my_strategic_goal isnt NULL)
	{
		
		
		the_squad->last_goal_type = the_squad->my_strategic_goal->what_goal;

		
		complete = the_squad->my_strategic_goal->Get_Totally_Complete(ai);

		
		
		
		
		if (!the_squad->my_strategic_goal->Execute_Incrementally() &&
			task_result != GOAL_INAPPROPRIATE &&
			task_result != GOAL_CANT_REACH_TARGET)
			the_squad->my_strategic_goal->dont_allocate = true;

		
		if (complete or (task_result == GOAL_COMPLETE) or
			(REMOVE_WHEN_EXECUTED == the_squad->my_strategic_goal->GetRemovalTime())) 
		{ 

			
			if (complete)
			{
#ifdef DBGAILOG
				#ifdef LOG_SCHEDULER
				if (ai->AI_VERBOSITY >= LOG_DETAILS)
				{
					AILOG(( wstr,  "\nExecute_One_Task: COMPLETE GOAL (#%d) ", 
									the_squad->my_strategic_goal->goal_ID));
					the_squad->my_strategic_goal->Display_Goal_Type(ai);
					the_squad->my_strategic_goal->Display_Goal_Details(ai);
					AILOG(( wstr,  "\n\n"));
				}
				#endif
#endif DBGAILOG

				
				Remove_Goal_And_Add_To_Delete_List(the_squad->my_strategic_goal);

				
				the_squad->Detach_Goal();

			} 
		
			
			
			
			
			if (task_result != GOAL_COMPLETE ||
				task_result != GOAL_IN_PROGRESS)
			{
				if (complete)
					task_result = GOAL_COMPLETE;
				else
					task_result = GOAL_IN_PROGRESS;
			}

		} 

    } 

	
	switch (task_result)
	{
	
	
	case GOAL_FAILED:
		{
#ifdef DBGAILOG
			#ifdef LOG_SCHEDULER
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
			{
				AILOG(( wstr,  "GOAL_FAILED\n"));
			}
			#endif
#endif DBGAILOG

			
			the_squad->Detach_Goal();

			
			troops_used_up = squad_agent_count;

		} 

		break;

    case GOAL_CANT_REACH_TARGET: 
		
		
        if (ACCUMULATE_AUXILIARY == retry &&
			the_squad->the_team->GetUsableTransport() > 0) { 
			
			

#ifdef DO_WALLCLOCK
if (g_wall_clock) {  

}
#endif
            
			aux_result = ExecuteWithTransport(ai, the_plan, troops_used_up);

#ifdef DO_WALLCLOCK
if (g_wall_clock) {     

}    
#endif
			if (aux_result == GOAL_COMPLETE || aux_result == GOAL_IN_PROGRESS)
				break; 
			
        } 
        
		
#ifdef DBGAILOG
			#ifdef LOG_SCHEDULER
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
			{
				AILOG(( wstr,  "\n GOAL_CANT_REACH_TARGET\n"));
			}
			#endif
#endif DBGAILOG

	case GOAL_INAPPROPRIATE:
		{
#ifdef DBGAILOG
			#ifdef LOG_SCHEDULER
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
			{
				AILOG(( wstr,  "\n GOAL_INAPPROPRIATE\n"));
			}
			#endif
#endif DBGAILOG

			
			the_squad->Detach_Goal();

			
			
			if (original_squad isnt NULL)
			{
				
				
				original_squad->Take_All_Agents(ai, the_squad);
				#ifdef _DEBUG
				if (ai->AI_VERBOSITY >= LOG_DETAILS)
					{
						AILOG(( wstr,  "Putting agents back into squad #%d",
								original_squad->squad_ID));
						AILOG(( wstr,  " from squad #%d.\n",
								the_squad->squad_ID));
					}
				#endif _DEBUG

				
				original_squad->
					ignore_goals.Add_Data( (data_ptr) the_goal );

				#ifdef _DEBUG
				if (ai->AI_VERBOSITY >= LOG_DETAILS)
					{
						AILOG(( wstr,  "Adding goal %d to the ignore_goals ",
								the_goal->GetID()));
						AILOG(( wstr,  " of squad %d\n",
								original_squad->squad_ID));
					}
				#endif _DEBUG


			} 
			else
			{
				#ifdef _DEBUG
				if (ai->AI_VERBOSITY >= LOG_DETAILS)
					{
						AILOG(( wstr,  "Adding goal %d to the ignore_goals ",
								the_goal->GetID()));
						AILOG(( wstr,  " of squad %d\n",
								the_squad->squad_ID));
					}
				#endif _DEBUG

				
				the_squad->ignore_goals.Add_Data( (data_ptr) the_goal );

				
				
				
				


    			Add_New_Matches_For_Squad(the_squad, the_goal);
				the_team->is_there_new_goals = FALSE; 
				the_team->is_there_new_squads = FALSE; 


			} 

			

		} 

		break;

	
	
	case GOAL_COMPLETE:
		{
#ifdef DBGAILOG
			#ifdef LOG_SCHEDULER
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
			{
				AILOG(( wstr,  "GOAL_COMPLETE\n"));
			}
			#endif
#endif DBGAILOG

			
			the_squad->Detach_Goal();

			
			
			
			troops_used_up = squad_agent_count - the_squad->Still_Can_Act();

			
			
                if (original_squad isnt NULL)
			    {
				    
				    






				    original_squad->Take_All_Agents(ai, the_squad);

			    } 
			    else
			    {
				    
				    
				    
				    if (the_team->is_there_new_goals) 
				    { 
    				    Add_New_Matches_For_Squad(the_squad);
					    the_team->is_there_new_goals = FALSE; 
					    the_team->is_there_new_squads = FALSE; 


				    } 


			    } 
            
		} 

		break;

	case GOAL_IN_PROGRESS:
		{
#ifdef DBGAILOG
			#ifdef LOG_SCHEDULER
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
			{
				AILOG(( wstr,  "\nGOAL_IN_PROGRESS\n"));
			}
			#endif
#endif DBGAILOG

			
			troops_used_up = squad_agent_count;
			

            
            the_squad->already_committed = TRUE;
		}
	break;

	default:
		break;
	} 

	if (aux_result == GOAL_COMPLETE || aux_result == GOAL_IN_PROGRESS)
			return aux_result;
	else
		return task_result;
}
















void Scheduler::Match_Resources_One_Pass()

{
	
	Plan_ptr next_best_plan = NULL;			
	Plan_ptr next_best_plan2 = NULL;			
	Plan_ptr last_best_plan = NULL;			
	Plan_ptr tmp_best_plan = NULL;			
	sint32 troops_used_up;				
	squad_ptr original_squad=NULL;		
	int old_agents_count;				
	int new_agents_count;			
	int goal_type;						
	int matches_handled = 0;			
	Goal_Result result = GOAL_INAPPROPRIATE;
	




	
	#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		AILOG(( wstr,  "\nScheduler::Match_Resources_One_Pass\n\n"));
	}
	#endif
	#endif DBGAILOG

	
	#ifdef _DEBUG
	sint32 finite_loop=0; 
	#endif
	
	sint32 iterations = 0;

	
	while (
#ifdef PLANS_IN_SORTED_ARRAY	
		(pos_in_match_list >= 0) and
#endif 

#ifdef PLANS_IN_RED_BLACK_TREE
		(not match_tree->Is_Traversal_Done()) and
#endif 

	    (the_team->my_agents.count > allocated_troops)

		and (passes_thru_main_loop < MAX_PLANS_IN_ONE_PASS_LOOP)
		)
	{
		iterations++;

#ifdef GRAPHICS_CALLBACK
		
		if (!(iterations % k_CALLBACK_CHECK_FREQUENCY) && TimeToUpdateGraphics()) {
			ai->m_gs->ProcessGraphicsCallback();
		}
#endif
		
		passes_thru_main_loop++;

		
		

		
		

		
		
		

		if (passes_thru_main_loop >= MAX_PLANS_IN_ONE_PASS_LOOP - 1)
		{
			#ifdef _DEBUG
			AILOG(( wstr,  "\npass_thru_main_loop >= MAX_PLANS_IN_ONE_PASS_LOOP - 1\n"));
			#endif
			














































			
			break;
		}

		
		matches_handled++;

#ifdef PLANS_IN_SORTED_ARRAY	
		
		next_best_plan = (Plan_ptr) 
			the_matches->Return_Data_By_Number(pos_in_match_list);

		next_best_plan2 = next_best_plan;
#endif 

#ifdef PLANS_IN_RED_BLACK_TREE
		next_best_plan = match_tree->Return_Next_Reverse_Order_Element();
#endif 





#ifdef PLANS_IN_RED_BLACK_TREE
#ifdef PLANS_IN_SORTED_ARRAY	
		
		
		Assert(next_best_plan == next_best_plan2);

#endif 
#endif 

		
		
		if (next_best_plan isnt NULL)
		{
			if (next_best_plan->Plan_Is_Needed_And_Valid())
			{
				
				
				
				
				
				if (last_best_plan &&
					(next_best_plan->the_goal->goal_ID != 
					last_best_plan->the_goal->goal_ID) &&
					last_best_plan->the_goal->what_goal == GOAL_TYPE_SALLY)
					{
						Goal_ptr last_goal = last_best_plan->the_goal;
						if (last_goal->CanBeExecuted() &&
							!last_goal->Execute_Incrementally() &&
							last_goal->Is_Goal_Undercommitted() &&
							last_goal->attacking_squad != NULL)
							{
								if (last_goal->attacking_squad->my_agents.count >0)
									Free_Undercommitted_Goal(ai, last_goal);
							}
						
					}

				
				last_best_plan = next_best_plan;

				
				#ifdef DBGAILOG
				#ifdef LOG_SCHEDULER
				if (ai->AI_VERBOSITY >= LOG_DETAILS)
				{
					AILOG(( wstr,  "\n***Using PLAN %f  Goal %d ", 
						next_best_plan->matching_value, 
						next_best_plan->the_goal->goal_ID));
					next_best_plan->the_goal->Display_Goal_Type(ai);
					next_best_plan->the_goal->Display_Goal_Details(ai);
					AILOG(( wstr,  " <---> Squad %d\n", next_best_plan->the_squad->squad_ID));

				}
				#endif
				#endif DBGAILOG

				
				if (next_best_plan->the_goal->attacking_squad is NULL)
				{
					
					original_squad = next_best_plan->the_squad;

					
					old_agents_count = 0;
				}
				else 
				{
					original_squad = NULL;

					
					
					old_agents_count = 
						next_best_plan->the_goal->attacking_squad->Still_Can_Act();
				} 

				
				
				next_best_plan->the_goal->m_current_needed_strength->SetTransports(0);

				
				goal_type = next_best_plan->the_goal->what_goal;

				
				
				if (	(old_agents_count is 0) and
						(exec_goal_count[goal_type]
							>= the_team->the_AIP.max_execute_goals_of_type[goal_type]) and
						(the_team->the_AIP.max_execute_goals_of_type[goal_type] 
							isnt UNLIMITED_GOALS)
					)
				{
					
					#ifdef DBGAILOG
					#ifdef LOG_SCHEDULER
					if (ai->AI_VERBOSITY >= LOG_DETAILS)
					{
						AILOG(( wstr,  "\n++++++MAX EXECUTABLE GOALS OF TYPE EXCEEDED!!!!\n"));

					}
					#endif
					#endif DBGAILOG

					
					
					
					

				} 
				
				else if (next_best_plan->AccumulateEnoughStrength(ai) ||
						 next_best_plan->the_goal->Execute_Incrementally() ||
						 ai->my_player_index == 0)
				{
					
					Assert(next_best_plan->the_goal->attacking_squad);

					
					new_agents_count = next_best_plan->the_goal->attacking_squad->GetUnitCount();
                
					
					
					result = Execute_One_Task(next_best_plan, troops_used_up, 
											  ACCUMULATE_AUXILIARY, original_squad);
					if (ai->m_i_am_dead) { 
						return; 	
					} 


					
					allocated_troops += troops_used_up;

					
					
					
					
					if (!next_best_plan->the_goal->Execute_Incrementally() &&
						ai->my_player_index != 0)
						{
							
							
							if ((result == GOAL_COMPLETE ||
								 result == GOAL_IN_PROGRESS ) &&
								old_agents_count == 0 &&
								new_agents_count != 0 )
								{
									
									exec_goal_count[next_best_plan->the_goal->what_goal]++;
								} 
						}
            
				} 
				#ifdef DBGAILOG
				#ifdef LOG_SCHEDULER
				else if (ai->AI_VERBOSITY >= LOG_DETAILS)
				{
					AILOG(( wstr,  "\nStill trying to accumulate enough strength...\n"));
				}
				#endif
				#endif DBGAILOG

			} 

		} 
#ifdef PLANS_IN_SORTED_ARRAY	
		
		pos_in_match_list--;
#endif 

	} 
	  


	#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		AILOG(( wstr, "\nEnd allocated_troops = %d.\n", 
			allocated_troops));

#ifdef PLANS_IN_SORTED_ARRAY	
		AILOG((wstr, "THE_MATCHES: %d\n\n",
			the_matches->count));
#endif 

#ifdef PLANS_IN_RED_BLACK_TREE
		AILOG((wstr, "MATCH_TREE: %d\n\n",
			match_tree->count));
#endif 

		AILOG((wstr, "MATCHES HANDLED: %d\n\n",
			matches_handled));

		AILOG(( wstr, "\nScheduler::Match_Resources_One_Pass: EXITING\n\n"));
	}
	#endif
	#endif DBGAILOG
}

















Goal_Result Scheduler::ExecuteWithTransport(AiMain *ai, Plan *needs_help, sint32 &troops_used_up)
{
    Assert(needs_help); 
	Assert(needs_help->the_goal); 
	Assert(needs_help->the_goal->attacking_squad);

	sint32 needed_cargo;
	Goal_Result result = GOAL_INAPPROPRIATE;
	Squad *squad_needs_help = NULL;
    Squad *donor_squad = NULL;
    Agent *agent=NULL; 
	ArmyAgent *donor_agent=NULL;
	sint32 full_slots, empty_slots;
	sint32 donor_idx;

	
	if (!needs_help->the_goal ||
		!needs_help->the_goal->attacking_squad)
		return result;

	
	squad_needs_help = needs_help->the_goal->attacking_squad;

	
	sint32 transports_in_squad = squad_needs_help->GetTransportUnitsCount(ai);
		
	
	needed_cargo = squad_needs_help->m_squad_strength.GetUnitCount() -
		squad_needs_help->m_squad_strength.GetTransport() - 
		transports_in_squad;

	
	if (needed_cargo <= 0)
		return GOAL_INAPPROPRIATE;

    
    sint32 squad_idx; 
    sint32 squad_num = the_team->my_squads->count; 

	double max_score = 0;
	double tmp_score;
	ArmyAgent *best_transport = NULL;
	Squad *best_squad = NULL;
	
	BOOL transport_found = TRUE;

	
	
	while (needed_cargo > squad_needs_help->m_squad_strength.GetTransport() &&
		   transport_found)
		{
			
			transport_found = FALSE;
			max_score = BAD_UTILITY;

			
			for ( squad_idx=0; squad_idx < squad_num; squad_idx++) 
				{ 
					
					donor_squad = (Squad*)the_team->my_squads->
						Return_Data_By_Number(squad_idx); 

					
					if (donor_squad->m_squad_strength.GetTransport() <= 0)
						continue;

					
					
					
					
					if (donor_squad->auxiliary_candidate == FALSE ||
						donor_squad->already_committed ||
						donor_squad->my_agents.count < 1)
						continue;

					
					if (squad_needs_help == donor_squad)
						continue;

					
					
					
					for (donor_idx = donor_squad->my_agents.count - 1; 
						 donor_idx >= 0; donor_idx--) 
						{ 
							agent = (Agent*)donor_squad->my_agents.
								Return_Data_By_Number(donor_idx);
							Assert(agent);
							
							if (!agent || agent->GetType() == AGENT_TYPE_CITY)
								continue;

							donor_agent = (ArmyAgent *)agent;

							donor_agent->
								GetNumCargo(ai, full_slots, empty_slots); 

							
							
							if (full_slots > 0)
								continue;

							if (empty_slots > 0)
								{
									
									
									tmp_score = squad_needs_help->
										ScoreTransportUtility(ai, donor_agent);
						
									
									
									if (tmp_score > max_score)
										{
											max_score = tmp_score;
											best_transport = donor_agent;
											best_squad = donor_squad;
										}
								} 
						} 
				} 

			if (max_score > BAD_UTILITY)
				{
					
					
					
					best_squad->auxiliary_candidate = FALSE;
					transport_found = TRUE;
					
#ifdef DBGAILOG
					if (ai->AI_VERBOSITY >= LOG_DETAILS)
						{
							
							AILOG(( wstr,  
									" Moving agent from donor squad %d into squad %d.",
									best_squad->squad_ID, 
									squad_needs_help->squad_ID));
							best_transport->Log_Type(ai);
							AILOG(( wstr,  "\n"));
						}
#endif DBGAILOG
					best_squad->Remove_Agent(best_transport);

					
					best_squad->Recompute_Squad_Class( ai );
					squad_needs_help->Add_Agent(ai, best_transport);

					best_transport = NULL;
					best_squad = NULL;
				} 

	} 

	
	if (needed_cargo <= squad_needs_help->m_squad_strength.GetTransport())
		result = Execute_One_Task(needs_help, troops_used_up, NO_AUXILIARY, NULL); 
	else
		
		result = GOAL_INAPPROPRIATE;
	return result;
}



















bool Scheduler::Free_Undercommitted_Troops()
{
	
	int i, j;							
	int undercommitted_count = 0;		
	double lowest_priority;				
										
	Goal_ptr goal_to_free=NULL;			
	double a_goals_priority;			
    Goal_ptr a_goal=NULL;				
	list_array<Goal_ptr> *pruned_goal_class;	
	

	
	lowest_priority = A_REALLY_BIG_POSITIVE_REAL_NUMBER;

	
	
	


	
	for (i = 0; i < MAX_GOAL_CLASSES; i++)
	{
		
		pruned_goal_class = pruned_goals_of_class[i];

		
		for (j = pruned_goal_class->count - 1; j >=0; j--)
		{
			
			a_goal = pruned_goal_class->Return_Data_By_Number(j);

			
			if (a_goal->CanBeExecuted())
			{
				

				if (!a_goal->Execute_Incrementally() 
					&& (a_goal->Is_Goal_Undercommitted()))
				{
					
					undercommitted_count++;

					
					if (a_goal->attacking_squad isnt NULL)
					{
						
						if (a_goal->attacking_squad->my_agents.count > 0)
						{
							
							a_goals_priority = a_goal->Compute_Raw_Priority(ai);

							
							if (a_goals_priority < lowest_priority)
							{
								
								goal_to_free = a_goal;

								
								lowest_priority = a_goals_priority;

							} 

						} 

					} 

				} 

			} 

		} 

	} 

	
	
	if ((undercommitted_count < 2) or (goal_to_free is NULL))
		return true;

	return (Free_Undercommitted_Goal(ai, goal_to_free) == TRUE);
}












BOOL Scheduler::Free_Undercommitted_Goal(AiMain *ai, Goal_ptr goal_to_free)
{
	Squad * free_squad=NULL;			

#ifdef PLANS_IN_SORTED_ARRAY	
	int old_match_list_pos;				
#endif 

    #ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
	{
		AILOG(( wstr,  "\nFreeing undercommitted goal: "));

		
		goal_to_free->Display_Goal_Type(ai);
		goal_to_free->Display_Goal_Details(ai);

		AILOG(( wstr,  "...\n\n"));
	}
	#endif
    #endif DBGAILOG

	
	goal_to_free->dont_allocate = true;

	
	
	

	
	free_squad = goal_to_free->attacking_squad;

	
	free_squad->Detach_Goal();

#ifdef PLANS_IN_SORTED_ARRAY	
	
	old_match_list_pos = pos_in_match_list;
#endif 

	
	Add_New_Matches_For_Squad(free_squad, goal_to_free);
	the_team->is_there_new_goals = FALSE; 
	the_team->is_there_new_squads = FALSE;

#ifdef PLANS_IN_SORTED_ARRAY	
	
	if (old_match_list_pos isnt pos_in_match_list)
		pos_in_match_list--;
#endif 

	
	return FALSE;
}













int Scheduler::Match_Resources()

{
	
	bool done = false;
	

#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		AILOG(( wstr,  "\nScheduler::Match_Resources.\n"));
	}
	#endif
#endif DBGAILOG

	
	allocated_troops = 0;

	
	passes_thru_main_loop = 0;

	
	
	while ((not done) and (passes_thru_main_loop < MAX_PLANS_IN_ONE_PASS_LOOP))
	{

		
		
		
		Match_Resources_One_Pass();

		
		done = Free_Undercommitted_Troops();
	} 

#ifdef DBGAILOG
	#ifdef LOG_SCHEDULER
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		AILOG(( wstr,  "\nScheduler::Match_Resources: EXITTING\n\n"));
	}
	#endif
#endif DBGAILOG

	return true;
}












