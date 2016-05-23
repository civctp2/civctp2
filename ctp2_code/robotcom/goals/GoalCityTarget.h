#pragma once










#ifndef __GOAL_CITY_TARGET_H__
#define __GOAL_CITY_TARGET_H__ 1

#include "ArmyGoal.h"

#include "dynarr.h"

enum GOAL_TYPE;
struct MapPointData;
class CivArchive;
class GoalCityTarget;

class GoalCityTarget : public ArmyGoal {
public:

	static BOOL PretestDipState(AiMain *ai, const MapPointData & pos, const GOAL_TYPE & type);
	static BOOL IsTargetType(AiMain *ai, MapPointData & pos, const GOAL_TYPE & type);
	static void SetupCityTargetGoals(AiMain *ai);




	GoalCityTarget
	(
		AiMain *init_ai,
		const MapPointData &target,
		const GOAL_TYPE &type,
		const BSetID &cityid,
		const PLAYER_INDEX foreign_player
	);
    GoalCityTarget(AiMain *ai, CivArchive &archive);
	void Init();
	~GoalCityTarget();
	BOOL operator==(const GoalCityTarget & ref) const;
	BOOL operator!=(const GoalCityTarget & ref) const {return !operator==(ref);}
	void DelPointers() {};

    void Serialize(AiMain *ai,CivArchive &archive);
    BOOL Validate(AiMain *ai);


    GOAL_TYPE GetType () const;
	const PLAYER_INDEX & GetPlayerIndex() const;
	ForeignCity *GetTarget(AiMain *ai);

	void Display_Goal_Type(AiMain *ai);
	void Display_Goal_Details(AiMain *ai);
	void GetDebugName(char *str);

	void Compute_Needed_Troop_Flow(AiMain *ai);
	double Compute_Matching_Value(AiMain *ai, squad_ptr the_squad);
	virtual int Is_Unit_Appropriate(AiMain *ai,	Agent * unit_in_question);

	void ExclusiveWinner(AiMain *ai, const MapPointData & target, const GOAL_TYPE &type);
	BOOL WithinRange( AiMain *ai, Agent *agent,
					  const SUB_TASK_TYPE & sub_task );
	void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army, ArmyAgent *the_transport, SUB_TASK_TYPE sub_task);

	Goal_Result Execute_Task(AiMain *ai, Plan *the_plan);

	Goal_Result Execute_Task(AiMain *ai);
	BOOL UnitTypeCapable(AiMain *ai, sint32 unit_type);
	BOOL PretestBid(AiMain *ai, ArmyAgent *actor);
	double Action_Bid(AiMain *ai, Agent *agent);
	double Compute_Raw_Priority(AiMain *ai);
	Goal_Result BuildTaskSolution(AiMain *ai, CityAgent *the_city, Plan *the_plan);

	private:

	PLAYER_INDEX m_playerIndex;
	PLAYER_INDEX m_foreignPlayer;
	BSetID m_targetId;
	AiMain *m_ai;

	sint32 m_which_unit;

	BOOL m_rallying;

    GoalCityTarget();
};

#endif __GOAL_CITY_TARGET_H__
