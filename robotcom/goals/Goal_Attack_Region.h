









#pragma once

#ifndef __GOAL_ATTACK_REGION_H__
#define __GOAL_ATTACK_REGION_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 

class Goal_Attack_Region : public ArmyGoal { 

public:


    Goal_Attack_Region(); 
	Goal_Attack_Region
	(
		AiMain *init_ai,					
		const double v,						
		const MapPointData &pos				
	); 	
    Goal_Attack_Region	(AiMain *ai,CivArchive &archive
	); 
	~Goal_Attack_Region(); 
    void Serialize(AiMain *ai,CivArchive &archive); 
    BOOL Validate(AiMain *ai); 



    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);

	
	
	
	
	
	
	void Display_Goal_Details(AiMain *ai);

	
	
	
	
	
	
	int Get_Totally_Complete(AiMain *ai);


	
	
	
	
	
	
	
	
	
	double Compute_Raw_Priority(AiMain *ai);


	
	
	
	
	
	
	
	
	
	
	int Is_Unit_Appropriate
	(
		AiMain *ai,						
		Agent * unit_in_question			
	);

    void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army);

    Goal_Result CheckCompletion(const sint32 build_count);

}; 

#endif __GOAL_ATTACK_REGION_H__
