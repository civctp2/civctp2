
#pragma once










#ifndef __GOAL_BOMBARD_H__
#define __GOAL_BOMBARD_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 
class ForeignCity; 

class GoalBombard : public ArmyGoal { 

    sint32 m_foreign_player; 
    BSetID m_target_id;					
	AGENT_TYPE m_target_type;			
	AiMain *m_ai;
	BOOL m_rallying;

public:

	
    GoalBombard(); 
	GoalBombard
	(
		AiMain *init_ai,					
		ForeignAgent *target,				
		MapPointData pos					
	); 	     
	GoalBombard
	(
		AiMain *init_ai,					
		ForeignCity *target,				
		MapPointData pos					
	); 	     
    GoalBombard(AiMain *ai, CivArchive &archive); 
	~GoalBombard(); 
    void Serialize(AiMain *ai,CivArchive &archive); 
    BOOL Validate(AiMain *ai); 
    void HookUp(AiMain *ai);

	
    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);
	void Display_Goal_Details(AiMain *ai);

	void CurrentTargetStrength(double &attack, 
		double &defense, 
		double &active_defense, 
		double &bombard,
		double &counter_bombard);
	int Is_Satisfied(int *excess); 

    ForeignAgent *GetTarget(AiMain *ai);
	void Compute_Needed_Troop_Flow(AiMain *ai);
	double Compute_Matching_Value(AiMain *ai, squad_ptr the_squad);
	virtual int Is_Unit_Appropriate(AiMain *ai,	Agent * unit_in_question);

	BOOL WithinRange( AiMain *ai, Agent *agent,
					  const SUB_TASK_TYPE & sub_task );
	void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army, ArmyAgent *the_transport, SUB_TASK_TYPE sub_task);

	void Set_Invalid();
	Goal_Result Execute_Task(AiMain *ai);
	BOOL PretestBid(AiMain *ai, ArmyAgent *actor);
	double Action_Bid(AiMain *ai, Agent *agent);
	double Compute_Raw_Priority(AiMain *ai);
	Goal_Result BuildTaskSolution(AiMain *ai, CityAgent *the_city, Plan *the_plan);
}; 

#endif __GOAL_BOMBARD_H__
