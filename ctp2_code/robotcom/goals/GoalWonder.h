
#pragma once

#ifndef __GOAL_WONDER_H__
#define __GOAL_WONDER_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE; 
class CivArchive; 

class AiMain; 
class CityAgent; 

class GoalWonderFlat { 
protected:
    sint32 m_idx_wonder; 
public:

    ~GoalWonderFlat(); 
    void Serialize(CivArchive &archive); 
}; 

class GoalWonder : public ArmyGoal , public GoalWonderFlat{ 

public:


    GoalWonder(); 
	GoalWonder(const sint32 idx); 	
    GoalWonder(AiMain *ai,CivArchive &archive); 
	~GoalWonder(); 
    void Serialize(AiMain *ai,CivArchive &archive); 
    BOOL Validate(AiMain *ai); 



    GOAL_TYPE GetType () const;
    sint32 GetIdxWonder() const;

	
	
	
	
	
	
	void Display_Goal_Type(AiMain *ai);

    Goal_Result BuildTaskSolution(AiMain *ai, CityAgent *the_city, 
        Plan* the_plan);
    Goal_Result CheckCompletion(const sint32 build_count);
	double Compute_Raw_Priority(AiMain *ai);
}; 

#endif __GOAL_WONDER_H__
