#pragma once

#ifndef __GOAL_BUILDING_H__
#define __GOAL_BUILDING_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE;
struct MapPointData;
class CivArchive;

class AiMain;
class CityAgent;

class GoalBuildingFlat {
protected:
    uint32 m_home_id;
    sint32 m_building_type;
public:

    ~GoalBuildingFlat();
    void Serialize(CivArchive &archive);
};

class GoalBuilding : public ArmyGoal , public GoalBuildingFlat{

public:

    GoalBuilding();
	GoalBuilding
	(

		const double v,
		const sint32 bt,
		const uint32 id
	);
    GoalBuilding(AiMain *ai, CivArchive &archive);
	~GoalBuilding();
    void Serialize(AiMain *ai, CivArchive &archive);
    BOOL Validate(AiMain *ai);


    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);
	void Display_Goal_Details(AiMain *ai);
    sint32 GetBuildingType() const;
    uint32 GetHomeId() const { return m_home_id; }












	int Is_Unit_Appropriate
	(
		AiMain *ai,
		Agent * unit_in_question
	);

    Goal_Result BuildTaskSolution(AiMain *ai, CityAgent *the_city, Plan* the_plan);
    Goal_Result CheckCompletion(const sint32 build_count);

	double Compute_Raw_Priority(AiMain *ai);
};

#endif __GOAL_BUILDING_H__
