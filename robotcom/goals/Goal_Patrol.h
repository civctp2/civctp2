









#pragma once

#ifndef __GOAL_PATROL_H__
#define __GOAL_PATROL_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 

class Goal_Patrol : public ArmyGoal { 

public:


    Goal_Patrol(); 
	Goal_Patrol
	(
		AiMain *init_ai,					
		const double v,						
		const MapPointData &pos				
	); 	
    Goal_Patrol	(AiMain *ai,CivArchive &archive
	); 
	~Goal_Patrol(); 
    void Serialize(AiMain *ai,CivArchive &archive); 
    BOOL Validate(AiMain *ai); 



    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);

	
	
	
	
	
	
	void Display_Goal_Details(AiMain *ai);


	
	
	
	
	
	
	int Get_Totally_Complete(AiMain *ai);


	
	
	
	
	
	
	
	
	
	double Compute_Raw_Priority(AiMain *ai);

    Goal_Result CheckCompletion(const sint32 build_count);

}; 

#endif __GOAL_PATROL_H__
