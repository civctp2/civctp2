#pragma once

#ifndef __GOAL_PERIMETER_H__
#define __GOAL_PERIMETER_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE;
struct MapPointData;
class CivArchive;

class Goal_Perimeter : public ArmyGoal {

public:

    Goal_Perimeter();
	Goal_Perimeter
	(
		AiMain *init_ai,
		const double v,
		const MapPointData &pos
	);
    Goal_Perimeter	(AiMain *ai,CivArchive &archive
	);
	~Goal_Perimeter();
    void Serialize(AiMain *ai,CivArchive &archive);
    BOOL Validate(AiMain *ai);


    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);







	void Display_Goal_Details(AiMain *ai);




















	double Compute_Raw_Priority(AiMain *ai);

	void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army);

    Goal_Result CheckCompletion(const sint32 build_count);

};

#endif __GOAL_PERIMETER_H__
