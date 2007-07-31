

#pragma once

#ifndef __GOAL_RECRUIT_DEFENSE_H__
#define __GOAL_RECRUIT_DEFENSE_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 
class CityAgent; 

class GoalDefense : public ArmyGoal { 

    BSetID *m_city_id; 
    CityAgent *m_city_agent; 

public:


    GoalDefense(); 

	GoalDefense
	(
		AiMain *init_ai,					
		CityAgent *the_city,				
		MapPointData pos					
	); 	
    GoalDefense(AiMain *ai, CivArchive &archive); 
	~GoalDefense(); 
    void Serialize(AiMain *ai, CivArchive &archive); 
    BOOL Validate(AiMain *ai); 
    void HookUp(AiMain *ai);



    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);
	void Display_Goal_Details(AiMain *ai);
	int Is_Satisfied(int *excess);

    CityAgent *GetCityAgent() { return m_city_agent; } 

	
	
	
	
	
	
	int Closest_Enemy_Distance(AiMain *ai, sint32 &attack_threat);


	
	
	
	
	
	
	
	
	
	double Compute_Raw_Priority(AiMain *ai);

    void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army,
					   ArmyAgent *the_transport, SUB_TASK_TYPE sub_task);

	
	
	
	
	
	
	int Get_Totally_Complete(AiMain *ai) 
	{
		return false;
	}

	
	
	
	
	
	
    void Set_Totally_Complete(const BOOL c) 
	{
		m_is_completed = false;
	}

	
	
	
	
	
	
	
	void Compute_Needed_Troop_Flow(AiMain *ai);

	BOOL FindPathToTask(AiMain *ai, ArmyAgent *the_army,
     ArmyAgent *the_transport, MapPointData &goal_pos, 
     const SUB_TASK_TYPE sub_task, MapPointData &dest_pos);


}; 

#endif __GOAL_RECRUIT_DEFENSE_H__
