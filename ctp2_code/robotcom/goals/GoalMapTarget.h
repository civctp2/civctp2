
#pragma once










#ifndef __GOAL_MAP_TARGET_H__
#define __GOAL_MAP_TARGET_H__ 1

#include "ArmyGoal.h"


#include "DynArr.h"

enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 
class GoalMapTarget;

class GoalMapTarget : public ArmyGoal { 
public:

	
	static BOOL IsTargetType(AiMain *ai, MapPointData & pos, const GOAL_TYPE & type);
	static void SetupMapTargetGoals(AiMain *ai);
	static void AddNewWanderGoals(AiMain *ai, const int points);
	static void AddNewTransportGoal(AiMain *ai, ArmyAgent *transport);

	

	
	GoalMapTarget
	(
		AiMain *init_ai,					
		const MapPointData &target,			
		const GOAL_TYPE &type			
	);
    GoalMapTarget(AiMain *ai, CivArchive &archive); 
    GoalMapTarget();  

	void Init();

	~GoalMapTarget(); 
	
	BOOL operator==(const GoalMapTarget & ref) const;
	BOOL operator!=(const GoalMapTarget & ref) const {return !operator==(ref);}
	GoalMapTarget& operator=(const GoalMapTarget &ref);

    void Serialize(AiMain *ai,CivArchive &archive); 
    BOOL Validate(AiMain *ai); 

	
	void AddTarget(const MapPointData & target);

	BOOL GetPos(MapPointData &pos);
    GOAL_TYPE GetType () const;
	const uint32 & GetMoveType() const;

	void Display_Goal_Type(AiMain *ai);
	void Display_Goal_Details(AiMain *ai);
	void GetDebugName(char *str);
	
	void Compute_Needed_Troop_Flow(AiMain *ai);
	double Compute_Matching_Value(AiMain *ai, squad_ptr the_squad);
	virtual int Is_Unit_Appropriate(AiMain *ai,	Agent * unit_in_question);

	BOOL FindPathToTask(AiMain *ai, ArmyAgent *the_army,
						ArmyAgent *the_transport, MapPointData &goal_pos, 
						const SUB_TASK_TYPE sub_task, MapPointData &dest_pos); 

	BOOL WithinRange( AiMain *ai, Agent *agent, 
					  const SUB_TASK_TYPE & sub_task );
	void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army, ArmyAgent *the_transport, SUB_TASK_TYPE sub_task);

	Goal_Result Execute_Task(AiMain *ai);
	BOOL PretestBid(AiMain *ai, ArmyAgent *actor);
	double Action_Bid(AiMain *ai, Agent *agent);
	double Compute_Raw_Priority(AiMain *ai);
	Goal_Result BuildTaskSolution(AiMain *ai, CityAgent *the_city, Plan *the_plan);

	private:
	
	DynamicArray<MapPointData> m_targets;
	uint32 m_moveType; 
	
	AiMain *m_ai;
}; 

#endif __GOAL_MAP_TARGET_H__
