









#pragma once

#ifndef __GOAL_CHOKEPOINT_H__
#define __GOAL_CHOKEPOINT_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 

class Goal_Chokepoint : public ArmyGoal { 

public:


    Goal_Chokepoint(); 
	Goal_Chokepoint
	(
		AiMain *init_ai,					
		const double v,						
		const MapPointData &pos				
	); 	
    Goal_Chokepoint	(AiMain *ai,CivArchive &archive
	); 
	~Goal_Chokepoint(); 
    void Serialize(AiMain *ai,CivArchive &archive); 
    BOOL Validate(AiMain *ai); 



    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);

	
	
	
	
	
	
	void Display_Goal_Details(AiMain *ai);

	
	
	
	
	
	
	int Get_Totally_Complete(AiMain *ai);

	
	
	
	
	
	
	
	
	double Compute_Matching_Value
	(
		AiMain *ai,						
		squad_ptr the_squad				
	);


	
	
	
	
	
	
	
	
	
	double Compute_Raw_Priority(AiMain *ai);

    void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army);
    Goal_Result CheckCompletion(const sint32 build_count);

}; 

#endif __GOAL_CHOKEPOINT_H__
