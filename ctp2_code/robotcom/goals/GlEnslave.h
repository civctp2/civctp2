
#pragma once

#ifndef __GOAL_ENSLAVE_H__
#define __GOAL_ENSLAVE_H__ 1

#include "ArmyGoal.h"

class AiMain;
class CivArchive; 
class ForeignAgent;
class ForeignCity; 
enum GOAL_TYPE;
class BSetID; 



class GoalEnslaveFlat { 

protected:
    sint32 m_place_holder_flat; 

public:
	GoalEnslaveFlat(); 
	void Serialize(CivArchive &archive); 
};


class GoalEnslavePtr { 
protected:
    sint32 foreign_player; 

    BSetID m_his_agent_id;
    BSetID m_his_city_id;
	AiMain *m_ai;

public:

	GoalEnslavePtr(); 
    ~GoalEnslavePtr(); 
	void Serialize(CivArchive &archive);
	void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
};



class GoalEnslave :  public ArmyGoal, public GoalEnslaveFlat, 
    public GoalEnslavePtr { 

public:

    GoalEnslave(); 
    GoalEnslave( AiMain *init_ai, BSetID &id, ForeignAgent *him); 
    GoalEnslave( AiMain *init_ai, BSetID &id, ForeignCity *his_city); 

    GoalEnslave(AiMain *ai, CivArchive &archive); 
    void Init(); 

    void Serialize(AiMain *ai, CivArchive &archive); 
    BOOL Validate(AiMain *ai); 
    void HookUp(AiMain *ai); 
    
    GOAL_TYPE GetType () const;
	void Display_Goal_Type(AiMain *ai);
    
    BOOL GetPos(MapPointData &pos);
    BOOL IsCity();  
    void CleanUp(); 

	void Set_Invalid();
	double Compute_Raw_Priority(AiMain *ai);

    BOOL FindPathToTask(AiMain *ai, ArmyAgent *the_army,
      MapPointData &dest_pos);

    void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army,
		ArmyAgent *the_transport, SUB_TASK_TYPE sub_task);
	ForeignAgent *GetAgentTarget(AiMain *ai);
	ForeignCity *GetCityTarget(AiMain *ai);
	BOOL WithinRange(AiMain *ai, Agent *agent,
					 const SUB_TASK_TYPE & sub_task);
};


#endif __GOAL_ENSLAVE_H__
