

#pragma once










#ifndef __GOAL_BUILD_SUP_H__
#define __GOAL_BUILD_SUP_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 
class ForeignCity; 

class Goal_Build_Supplemental : public ArmyGoal 
{ 

	sint32 which_unit;					
	double my_priority;					

public:


    Goal_Build_Supplemental(); 
    Goal_Build_Supplemental(AiMain *ai,CivArchive &archive); 
	Goal_Build_Supplemental
	(
		AiMain *init_ai,				
		sint32 init_which_unit,			
		double init_priority			
	); 	
   
     
       
	~Goal_Build_Supplemental(); 
    void Serialize(AiMain *ai,CivArchive &archive); 
    BOOL Validate(AiMain *ai); 



    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);

	
	
	
	
	
	
	void Display_Goal_Details(AiMain *ai);


	
	
	
	
	
	
	
	void Compute_Needed_Troop_Flow();


	
	
	
	
	
	
	
	
	double Compute_Matching_Value
	(
		AiMain *ai,						
		squad_ptr the_squad				
	);


	
	
	
	
	
	
	
	
	
	
	
	
	int Is_Unit_Appropriate
	(
		AiMain *ai,						
		Agent * unit_in_question		
	);


	
	
	
	
	
	
	Goal_Result Execute_Task(AiMain *ai, Plan *the_plan);

	
	
	
	
	
	
	
	
	
	double Compute_Raw_Priority(AiMain *ai);


}; 

#endif __GOAL_BUILD_SUP_H__
