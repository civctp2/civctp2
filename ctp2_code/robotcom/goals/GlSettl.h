

#pragma once

#ifndef __GOAL_SETTLE_H__
#define __GOAL_SETTLE_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 
class GoalTransport; 

enum SETTLE_ENVIRONMENT { 
    SETTLE_ENVIRONMENT_WATER, 
    SETTLE_ENVIRONMENT_SPACE, 
    SETTLE_ENVIRONMENT_LAND, 
    SETTLE_ENVIRONMENT_ISLAND
};
    
class GoalSettle : public ArmyGoal { 

    SETTLE_ENVIRONMENT m_environment; 

public:


    GoalSettle(); 
	GoalSettle
	(
		AiMain *init_ai,					
		const double v,						
        MapPointData &pos			
	); 	

    GoalSettle(
        AiMain *ai, 
        CivArchive &archive); 
	~GoalSettle(); 
    void Serialize(AiMain *ai,CivArchive &archive); 
    BOOL Validate(AiMain *ai); 


    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);
	void Display_Goal_Details(AiMain *ai);

	
	
	
	
	
	
	
	
	
	double Compute_Raw_Priority(AiMain *ai);

    void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army, 
        ArmyAgent *the_transport, SUB_TASK_TYPE sub_task);

    SETTLE_ENVIRONMENT GetSettleEnvironment() { return m_environment; } 

    friend GoalTransport; 

}; 

#endif __GOAL_SETTLE_H__
