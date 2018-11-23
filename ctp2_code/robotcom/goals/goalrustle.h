#pragma once










#ifndef __GOAL_RUSTLE_H__
#define __GOAL_RUSTLE_H__ 1

#include "ArmyGoal.h"

enum GOAL_TYPE;
struct MapPointData;
class CivArchive;
class ForeignCity;

class GoalRustle : public ArmyGoal {

    sint32 m_foreign_player;
    BSetID m_target_id;
	AiMain *m_ai;

public:

	static BOOL PretestConstruct(AiMain *ai, ForeignAgent * agent);
    GoalRustle();
	GoalRustle
	(
		AiMain *init_ai,
		ForeignAgent *target,
		MapPointData pos
	);
    GoalRustle(AiMain *ai, CivArchive &archive);
	~GoalRustle();
    void Serialize(AiMain *ai,CivArchive &archive);
    BOOL Validate(AiMain *ai);
    void HookUp(AiMain *ai);

    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);

	void Set_Invalid();

    ForeignAgent *GetTarget(AiMain *ai);
	void Compute_Needed_Troop_Flow(AiMain *ai);
	double Compute_Matching_Value(AiMain *ai, squad_ptr the_squad);
	virtual int Is_Unit_Appropriate(AiMain *ai,	Agent * unit_in_question);

	BOOL WithinRange( AiMain *ai, Agent *agent,
					  const SUB_TASK_TYPE & sub_task );
	void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army,ArmyAgent *the_transport, SUB_TASK_TYPE sub_task);

	Goal_Result Execute_Task(AiMain *ai);
	BOOL PretestBid(AiMain *ai, ArmyAgent *actor);
	double Action_Bid(AiMain *ai, Agent *agent);
	double Compute_Raw_Priority(AiMain *ai);
	Goal_Result BuildTaskSolution(AiMain *ai, CityAgent *the_city, Plan *the_plan);
};

#endif __GOAL_RUSTLE_H__
