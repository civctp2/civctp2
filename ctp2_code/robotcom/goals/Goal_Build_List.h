

#pragma once










#ifndef __GOAL_BUILD_LIST_H__
#define __GOAL_BUILD_LIST_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 
class ForeignCity; 

class Goal_Build_List : public ArmyGoal 
{ 

	sint32 which_building;				
	double my_priority;					
	BOOL m_isWonder;					
	BOOL m_isEndGameObject;				
	BOOL m_isCapitalization;			
	BSetID m_myCityID;					

public:


    Goal_Build_List(); 
    Goal_Build_List(AiMain *ai,CivArchive &archive); 
	Goal_Build_List
	(
		AiMain *init_ai,				
		sint32 init_which_building,		
		double init_priority,			
		BOOL is_wonder,					
		BOOL is_end_game_object,		
		BOOL is_capitalization,			
		CityAgent *my_city				
	); 	
   
     
       
	~Goal_Build_List(); 
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

#endif __GOAL_BUILD_LIST_H__
