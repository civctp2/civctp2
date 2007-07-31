

#pragma once

#ifndef __GOAL_ATTACK_H__
#define __GOAL_ATTACK_H__ 1




#include "ArmyGoal.h"

enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 

class GoalAttack : public ArmyGoal { 

public:

    BSetID *the_bastard_id; 
	ForeignAgent * the_bastard;
    sint32 m_foreign_player; 
	BOOL rallying;					
										


    GoalAttack(); 
	GoalAttack(
		AiMain *ai,
        ForeignAgent *init_the_bastard,	
		const MapPointData &pos
	); 	

    GoalAttack(AiMain *ai, CivArchive &archive); 
    void Init(); 

	~GoalAttack(); 
    void Serialize(AiMain *ai, CivArchive &archive); 
    void HookUp(AiMain *ai); 
    

    BOOL Validate(AiMain *ai); 


    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);

	
	
	
	
	
	
	void GoalAttack::Display_Goal_Details(AiMain *ai);

	
	
	
	
	
	


	
	
	
	
	
	
	void Set_Invalid();

	
	
	
	
	
	
	
	void Compute_Needed_Troop_Flow(AiMain *ai);

	
	
	
	
	
	
	
	
	
	double Compute_Raw_Priority(AiMain *ai);

    
    Goal_Result Execute_Task(AiMain *ai, Plan *the_plan);

    void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army);

    double GetDefenseStrength(); 
    double GetAttackStrength(); 

}; 

#endif __GOAL_ATTACK_H__
