

#pragma once










#ifndef __GOAL_RETREAT_H__
#define __GOAL_RETREAT_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 
class ForeignCity; 

class GoalRetreat : public ArmyGoal { 

    BSetID m_target_id; 
	AiMain *m_ai;

public:


    GoalRetreat(); 
	GoalRetreat
	(
        AiMain *init_ai,					
        CityAgent *the_city,				
        MapPointData pos					
	); 	     
    GoalRetreat(AiMain *ai,CivArchive &archive); 
	~GoalRetreat(); 
    void Serialize(AiMain *ai,CivArchive &archive); 
    BOOL Validate(AiMain *ai); 
    void HookUp(AiMain *ai);



    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);
    void Display_Goal_Details(AiMain *ai);
	void Set_Invalid();

	
	
	
	
	
	
	
	void Compute_Needed_Troop_Flow(AiMain *ai);


	
	
	
	
	
	
	
	
	
	
	int Is_Unit_Appropriate
	(
		AiMain *ai,							
		Agent * unit_in_question			
	);


    
    
    
    
    
    
    
    
    double Compute_Matching_Value
    (
         AiMain *ai,                    
         squad_ptr the_squad            
    );


    
    
    
    
    
    double Action_Bid
    (
        AiMain *ai,
        Agent *agent
    );

	













	
	
	
	
	
	
	
	
	
	double Compute_Raw_Priority(AiMain *ai);

    
    
    
    

    CityAgent *GetTarget(AiMain *ai) ;

	void ArrivedAtTask(AiMain *ai, 
		ArmyAgent *the_army, 
		ArmyAgent *the_transport, 
		SUB_TASK_TYPE sub_task);

    BOOL FindPathToTask(AiMain *ai, ArmyAgent *the_army,
        ArmyAgent *the_transport, MapPointData &goal_pos, 
        const SUB_TASK_TYPE sub_task, MapPointData &dest_pos);

}; 

#endif __GOAL_RETREAT_H__
