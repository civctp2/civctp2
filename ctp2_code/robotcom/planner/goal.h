













#ifndef __Goal_h__
#define __Goal_h__



#include "strategic_ai_forward.h"









#define UNASSIGNED_ID (-1)








typedef enum _removal_time_enum 
{
	REMOVE_WHEN_SORTED,
	REMOVE_WHEN_FULFILLED,
    REMOVE_WHEN_EXECUTED, 
	DONT_REMOVE
} removal_time_enum;




typedef class Goal * Goal_ptr;

enum Goal_Result
{
	GOAL_FAILED,
	GOAL_INAPPROPRIATE,
    GOAL_CANT_REACH_TARGET, 
	GOAL_IN_PROGRESS,
	GOAL_COMPLETE
};






class Goal_Flat_Data
{


public:
	int goal_ID;						

	GOAL_TYPE what_goal;				

	GOAL_CLASS the_class;				

	double raw_priority;				

    sint16 m_farthest_agent_to_target_dist;

	removal_time_enum removal_time;		



	unsigned dont_allocate : 1;					
	unsigned invalid_goal : 1;					
	unsigned single_plan_only : 1;				
	unsigned ever_appropriate : 1;				
										
	unsigned survived_pruning : 1;				
	unsigned marked_for_deletion : 1;			
    unsigned m_is_completed : 1; 



	Goal_Flat_Data(); 

	void Serialize(CivArchive &archive); 

};















class AiMain;
class Squad_Strength; 

class Goal : public Goal_Flat_Data
{



protected:



public:
    sint32 my_squad_id; 

    squad_ptr attacking_squad;			

    Squad_Strength *m_min_needed_strength; 
    Squad_Strength *m_current_needed_strength;
    Squad_Strength *m_current_attacking_strength; 

	

	list_cell_ptr new_goal_cell_ptr;	





protected:





public:


	
	
	
	
	
	
	
	void Init_Goal();



	
	
	
	
	
	
	Goal
	(
	);



	
	
	
	
	
	
	Goal
	(	
		AiMain *ai,						
		CivArchive &archive
	);



	
	
	
	
	
	
	
	
	virtual ~Goal();


	
	
	
	
	
	
    void Load(AiMain *ai, CivArchive &archive); 


	
	
	
	
	
	
    void Store(AiMain *ai, CivArchive &archive); 


	
	
	
	
	
	
	void Serialize(AiMain *ai, CivArchive &archive); 

    virtual BOOL Validate(AiMain *ai); 
    virtual void HookUp(AiMain *ai); 

	
	
	
	
	
	
	virtual int Is_Goal_Undercommitted();


	
	
	
	
	
	
	virtual void Classify_Goal() = 0;

	
	
	
	
	
	
	
	virtual void Display_Goal_Type(AiMain *ai) = 0;

	
	
	
	
	
	
	virtual void GetDebugName(char *str) = 0;

	
	
	
	
	
	
	virtual void Display_Goal_Details(AiMain *ai);


	
	
	
	
	
	
	
	
	virtual int Is_Satisfied
	(
		int *excess						
	) 
	{
		return false;
	}

	
	
	
	
	
	
	
	
	
	
	virtual BOOL Execute_Incrementally() = 0;


	
	
	
	
	
	
	
	
	virtual double Distance_From_Squad_To_Goal
	(
        AiMain *ai, 
		squad_ptr the_squad				
	) = 0;


	
	
	
	
	
	
	
	virtual void Compute_Needed_Troop_Flow(AiMain *ai) = 0;


	
	
	
	
	
	
	
	
	virtual double Compute_Matching_Value
	(
		AiMain *ai,						
		squad_ptr the_squad				
	) = 0;


	
	
	
	
	
	
	
	
	
	virtual double Compute_Raw_Priority(AiMain *ai) = 0;


	
	
	
	
	
	
	
	
	
	
	virtual int Is_Unit_Appropriate
	(
		AiMain *ai,						
		Agent * unit_in_question		
	) = 0;


	
	
	
	
	
	
	
	virtual void Commit_Unit
	(
		Agent * unit_in_question		
	) = 0;


	
	
	
	
	
	
	virtual Goal_Result Execute_Task(AiMain *ai, Plan *the_match) = 0;


	
	
	
	
	
	
	virtual int Get_Totally_Complete(AiMain *ai) 
	{
		return m_is_completed;
	}

    virtual void Set_Totally_Complete(const BOOL c) 
	{
		m_is_completed = c;
	}

	
	
	
	
	
	
	virtual void Set_Invalid(); 
	void Detach_Squad();

	
	
	
	
	
	
	int Get_Invalid() 
	{
		return invalid_goal;
	}

    removal_time_enum GetRemovalTime() const { return removal_time; } 

    int GetID() const { return goal_ID; } 

    virtual void CleanUp(); 

    virtual void EstimateRecruitmentStrength(AiMain *ai, 
        Agent *donor_agent, double &str_score); 


    BOOL CanBeExecuted() const ; 
};







#endif 
