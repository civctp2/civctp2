
#pragma once

#ifndef __GOAL_EXPLORE_H__
#define __GOAL_EXPLORE_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 

class GoalExplore : public ArmyGoal { 

public:


    GoalExplore(); 
	GoalExplore
	(
		AiMain *init_ai,					
		const double v,						
		const MapPointData &pos				
	); 	
    GoalExplore	(AiMain *ai, CivArchive &archive); 
	~GoalExplore(); 
    void Serialize(AiMain *ai, CivArchive &archive); 
    BOOL Validate(AiMain *ai); 



    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);

	
	
	
	
	
	
	void Display_Goal_Details(AiMain *ai);

	
	
	
	
	
	
	int Get_Totally_Complete(AiMain *ai);


	
	
	
	
	
	
	
	
	
	double Compute_Raw_Priority(AiMain *ai);
    
    BOOL FindPathToTask(AiMain *ai, ArmyAgent *the_army,
        MapPointData &dest_pos); 

    void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army);

    Goal_Result CheckCompletion(const sint32 build_count);

}; 

#endif __GOAL_EXPLORE_H__
