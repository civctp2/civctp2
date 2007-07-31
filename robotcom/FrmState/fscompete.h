

#pragma once

#ifndef __FS_COMPETE_H__
#define __FS_COMPETE_H__ 1

#include "FrameState.h"


class GoalSettle;

class FSCompeteForAllMovement : public FrameState { 

public:

    FSCompeteForAllMovement(); 

    void Serialize(CivArchive &archive); 

    BOOL Execute(AiMain *ai, sint32 &branch, FILE *fout);

    void FindAllGoalPos(AiMain *ai, FILE *fout); 

    
    void InsertCityBuildGoal(AiMain *ai);

    BOOL PretestFreight(AiMain *ai);
    void InsertFreightGoal(AiMain *ai);

    void InsertEnslaveGoals(AiMain *ai); 


	
	
	
	
    void Insert_Attack_Goals(AiMain *ai);


	
	
	
	
	void Insert_Seige_Goals(AiMain *ai);

	
	
	
	
	

	
	
	
	
	
	
	void Insert_Perimeter_And_Patrol_Goals(AiMain *ai);


	
	
	
	
	void Insert_Explore_Goals(AiMain *ai);

	void Insert_Sally_Goals(AiMain *ai);

    BOOL PretestSettle(AiMain *ai) const; 
    BOOL PretestExplore(AiMain *ai) const;

	void CalcAllBids(AiMain *ai, FILE *fout);
	BOOL NextFrameState(AiMain *ai, sint32 &branch);
    void ExpandGoals(AiMain *ai);

    void InsertWonderGoal(AiMain *ai);
    
    void FlattenPersistentSettlGoals(AiMain *ai);

 
};



#endif __FS_WAR_H__
