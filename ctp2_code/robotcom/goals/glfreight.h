#pragma once

#ifndef __GOAL_FREIGHT_H__
#define __GOAL_FREIGHT_H__ 1

#include "ArmyGoal.h"

class CivArchive;

class GoalFreightFlat {
protected:
	sint32 m_freight_cost;
public:
    GoalFreightFlat();
    ~GoalFreightFlat();
	void Serialize(CivArchive &archive);
};

enum Goal_Result;
class Plan;

class GoalFreight : public ArmyGoal,  public GoalFreightFlat {

public:
	GoalFreight
	(

		sint32 fc,
		double u
	);

    GoalFreight(
        AiMain *ai,
        CivArchive &archive);
    ~GoalFreight();
	void Serialize(AiMain *ai, CivArchive &archive);
    BOOL Validate(AiMain *ai);

    GOAL_TYPE GetType() const;
	void Display_Goal_Type(AiMain *ai);
    double GetUtility() const { return m_value; }

    Goal_Result BuildTaskSolution(AiMain *ai, CityAgent *the_city, Plan* the_plan);
    Goal_Result CheckCompletion(const sint32 build_count);
	double Compute_Raw_Priority(AiMain *ai);
};

#endif __GOAL_FREIGHT_H__
