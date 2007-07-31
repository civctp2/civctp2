












#ifndef __Scheduler_h__
#define __Scheduler_h__



#include "Strategic_AI_Forward.h"


#define PLANS_IN_RED_BLACK_TREE




template <class DATA_TYPE> class list_array;
template <class DATA_TYPE> class Pool;
template <class DATA_TYPE, class KEY_TYPE> class Red_Black_Tree;









#define TIME_SLICE_DONE 1
#define TIME_SLICE_NOT_DONE 0



#define MAX_PRECOMPUTE_MATCHES 10000000



#define INITAL_PLAN_POOL_SIZE 200


#define MAX_PLAN_IDS (100000)




#define MAX_PLANS_IN_ONE_PASS_LOOP 4000



#define UNLIMITED_GOALS -999999


#define MIN_GOOD_MATCH_VALUE (0.0)


#define INITIAL_GOAL_CLASS_SIZE 20


#define INITIAL_SQUAD_CLASS_SIZE 20


#define INITIAL_GOOD_GOAL_CLASSES_FOR_SQUAD 5
#define INITIAL_GOOD_SQUAD_CLASSES_FOR_GOAL 10

#define SUPER_DEBUG_SPEED true










enum NEEDS_AUXILIARY
{ 
	NO_AUXILIARY, 
	ACCUMULATE_AUXILIARY
};










class Plan_Sort_Key
{
public:
	double value;
	int ID;

	int operator>(Plan_Sort_Key& that)
	{
		if (value > that.value) return true;
		if ((value == that.value) and (ID > that.ID)) return true;
		return false;
	}

	int operator<(Plan_Sort_Key& that)
	{
		if (value < that.value) return true;
		if ((value == that.value) and (ID < that.ID)) return true;
		return false;
	}
};














class Scheduler_Flat_Data
{


protected:

	

	int unique_goal_ID;					

	int goals_inited;					
	int squads_inited;					
	int precompute_inited;				
	int schedule_matching_done;			
	int schedule_squad_fix_done;		


	int pos_in_sorted_goal_list;		

#ifdef PLANS_IN_SORTED_ARRAY	
	int pos_in_match_list;				
#endif 

	int precompute_squad_number;		
										
										

	int allocated_troops;				

	int unique_plan_ID;					

	int total_goal_count;				
										

	int passes_thru_main_loop;			
										
										



public:

	int max_priority;					
	double min_priority;				

	int send_groups_done;				
	int subscheduling_inited;			
	int troop_allocation_done;			
	int exec_goal_count[GOAL_TYPE_MAX];	
	
	int last_best_goal_of_type[GOAL_TYPE_MAX];	
												
												
												
												
												

    Scheduler_Flat_Data(); 
    void Serialize(CivArchive &archive); 
};

















class Scheduler : public Scheduler_Flat_Data
{

protected:

	
	list_array<SQUAD_CLASS> * appropriate_squad_classes[MAX_GOAL_CLASSES];

	
	sorted_array<Goal_ptr,double> * sorted_goals_of_class[MAX_GOAL_CLASSES];

	
	list_array<Goal_ptr> * pruned_goals_of_class[MAX_GOAL_CLASSES];

	
	list_array<GOAL_CLASS> * appropriate_goal_classes[MAX_SQUAD_CLASSES];

	
	list_array<Squad *> * squads_of_class[MAX_SQUAD_CLASSES];


	
	
	
	linked_list_ptr new_goals;			

	
	linked_list_ptr goals_to_delete;	

	
	
	int goal_count[GOAL_TYPE_MAX];		

	
	
	Planner_ptr the_team;				

	
	AiMain *ai;							

	uint32	m_updateTime;				

#ifdef SUPER_DEBUG_SPEED
	int super_debug_speed;				
#endif 



public:

#ifdef PLANS_IN_SORTED_ARRAY
	
	
	sorted_array<Plan *, double> *the_matches;
#endif

#ifdef PLANS_IN_RED_BLACK_TREE
	
	Red_Black_Tree<Plan *,Plan_Sort_Key> * match_tree;
#endif

	
	#define k_UPDATE_FREQ		100
	BOOL TimeToUpdateGraphics() 
	{ 
		if (GetTickCount() > m_updateTime) { 
			m_updateTime = GetTickCount() + k_UPDATE_FREQ; 
			return TRUE;
		} else 
			return FALSE; 
	}

	
	
	
	
	
	
	
	
	
	int Scheduler::Allocate_Troops_And_Execute_Tasks
	(
		AiMain *ai						
	);

	
	
	
	
	
	
	void Init();


	
	
	
	
	
	
	Scheduler
	(
		AiMain *init_ai,				
		Planner_ptr i_team 				
	);

	
	
	
	
	
	
	Scheduler
	(
		AiMain *init_ai,				
		Planner_ptr i_team,				
		CivArchive &archive				
	);

	
	
	
	
	
	
	~Scheduler();

	
	
	
	
	
	
    void Load(CivArchive &archive); 


	
	
	
	
	
	
    void Store(CivArchive &archive); 


	
	
	
	
	
	
    void Serialize(CivArchive &archive); 
    BOOL Validate(AiMain *ai); 

	
	
	
	
	
	
	
    void Associate_Squad_Class_With_Goal_Class
	(
		SQUAD_CLASS the_squad_type,		
		GOAL_CLASS the_goal_class		
	); 



	
	
	
	
	
	
	
	
	
	
    void Reclassify_Squad
	(
		Squad *the_squad,				
		SQUAD_CLASS old_squad_type		
	); 



	
	
	
	
	
	
	void Planning_Status_Reset();


	
	
	
	
	
	
	int Ungroup_Squads();

	
	
	
	
	
	
	int Update_Squads();


	
	
	
	
	
	void Clean_Up();


	
	
	
	
	
	
	
	
	
	void Allocate_Remaining_Troops(AiMain *ai);


	
	
	
	
	
	void Add_New_Goal
	(
	  Goal_ptr my_new_goal
	);


	
	
	
	
	
	void Remove_Goal
	(
	  Goal_ptr bad_goal
	);


	
	
	
	
	
	void Remove_Goal_And_Add_To_Delete_List
	(
	  Goal_ptr bad_goal
	);


	
	
	
	
	
	
	
	Goal_ptr Return_Goal_By_Number
	(
	  int n
	);


	
	
	
	
	
	
	void Scheduler::Update_Goals();
	

	
	
	
	
	
	
	
	
	int Add_New_Matches_For_Squad
	(
		squad_ptr the_squad,
		Goal_ptr ignore_goal = NULL		
	);


	
	
	
	
	
	
	void Scheduler::Add_Single_Plan(Agent *the_agent, Goal *the_goal);
	

	
	
	
	
	
	
	void Assign_Unique_ID
	(
	  Goal_ptr my_goal
	);




protected:


	
	
	
	
	
	
	int Prepare_Goals(AiMain *ai);


	
	
	
	
	
	
	
	void Prepare_Cities(AiMain *ai);



	
	
	
	
	
	
	
	
	
	int Prepare_Squads(AiMain *ai);



	
	
	
	
	
	
	
	
	bool Add_Single_Goal_To_Valid_Goals
	(
		Goal_ptr the_goal				
	);


    void LogGoalCount(AiMain *ai); 

	
	
	
	
	
	
	bool Add_New_Match_For_Goal_And_Squad
	(
		Goal_ptr the_goal,					
		squad_ptr the_squad					
	);


	
	
	
	
	
	
	
	
	int Add_New_Matches_For_Goal
	(
		Goal_ptr the_goal				
	);

	
	
	
	
	
	
	int Init_Match_Lists();


	
	
	
	
	
	
	
	
	
	void Generate_Matches_And_Valid_Goal_List();


	
	
	
	
	
	
	void Transfer_Troops_To_Goal
	(
		Goal_ptr my_goal				
	);


	
	
	
	
	
	
	
	Goal_Result Execute_One_Task
	(
		Plan *the_plan,				
		sint32 &troops_used_up, 	
        const NEEDS_AUXILIARY retry,
		squad_ptr original_squad	
	);


	
	
	
	
	
	
	
	void Match_Resources_One_Pass();


	
	
	
	
	
	
	
	
	
	
	Goal_Result ExecuteWithTransport(AiMain *ai, Plan *needs_help, sint32 &troops_used_up);


	
	
	
	
	
	
	bool Free_Undercommitted_Troops();
	BOOL Free_Undercommitted_Goal(AiMain *ai, Goal_ptr goal_to_free);


	
	
	
	
	
	
	
	int Match_Resources();

public:
    void DumpGoalToMap(const GOAL_TYPE show_me);

#ifdef SUPER_DEBUG_SPEED
	void SetSuperDebugSpeed(BOOL on) { super_debug_speed = on; }
#endif 

};








#endif 
