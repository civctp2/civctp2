
#pragma once

#ifndef __ARMY_GOAL_H__
#define __ARMY_GOAL_H__ 1



#include "strategic_ai_forward.h"





#include "common.h"
#include "linked_list.h"



#include "unitflow.h"
#include "goal.h"


enum GOAL_TYPE; 
struct MapPointData; 
class CivArchive; 



class AiMain; 

enum SUB_TASK_TYPE { 
    SUB_TASK_GOAL, 
    SUB_TASK_TRANSPORT_TO_BOARD, 
    SUB_TASK_CARGO_TO_BOARD,
	SUB_TASK_RALLYING
};












class ArmyGoal : public Goal { 

protected:
    double m_value;						
    sint8 m_satisfied;					

	

public:
	MapPointData *m_pos;				
	MapPointData *m_XYpos;				
	sint32 m_target_owner;				

    void Init();

public:



	
	
	
	
	
	
    ArmyGoal
	(
	); 	

	
	
	
	
	
	
   


  
	

	
	
	
	
	
	
	ArmyGoal
	(
		AiMain *init_ai,				
		const double v,					
		const MapPointData &pos			
	);

	
	
	
	
	
	


	



	
	
	
	
	
	
	
	
	virtual ~ArmyGoal();
    
    virtual void Serialize(AiMain *ai, CivArchive &archive); 
    virtual BOOL Validate(AiMain *ai); 

	

	static BOOL PretestUnitTypes( AiMain *ai, ArmyAgent *army, GOAL_TYPE goal_type);

	

	
	
	
	
    virtual BOOL GetPos(MapPointData &pos);
 

	
	
	
	
	double GetValue() const { return m_value; } 



	
	
	
	
    virtual GOAL_TYPE GetType () const = 0;
 

	
	
	
	
	virtual void SetSatisfaction (const BOOL s);
 

	
	
	
	
	virtual BOOL IsSatisfied();



	
	
	
	
	virtual void GetGoal(double &value,  GOAL_TYPE &gt, BOOL &is_sat) const;


	
	
	
	
	
	
	virtual void Classify_Goal();


	
	
	
	
	
	
	virtual void Display_Goal_Type(AiMain *ai);

	
	
	
	
	
	
	virtual void GetDebugName(char *str);

	
	
	
	
	
	
	void Display_Goal_Details(AiMain *ai);


	
	
	
	
	
	
	
	
	virtual int Is_Satisfied
	(
		int *excess						
	);

	
	
	
	
	
	
	
	
	
	
	virtual BOOL Execute_Incrementally();


	
	
	
	
	
	
	
	
	virtual double Distance_From_Squad_To_Goal
	(
        AiMain *ai,
		squad_ptr the_squad				
	);


	
	
	
	
	
	
	
	virtual void Compute_Needed_Troop_Flow(AiMain *ai);


	
	
	
	
	
	
	
	
	virtual double Compute_Matching_Value
	(
		AiMain *ai,						
		squad_ptr the_squad				
	);


	
	
	
	
	
	
	
	
	
	virtual double Compute_Raw_Priority(AiMain *ai);
	
	
	
	
	
	
	
	
	virtual double Compute_Raw_Priority(AiMain *ai, 
		sint32 threat_sign, 
		sint32 danger_sign,  
		sint32 value_sign,
		sint32 power_sign,
		sint32 distance_to_home_sign,
		sint32 distance_to_enemy_sign,
		sint32 chokepoint_sign);


	
	
	
	
	
	
	
	
	
	
	virtual int Is_Unit_Appropriate
	(
		AiMain *ai,						
		Agent * unit_in_question		
	);


    virtual void InitStrength(); 
    virtual void EstimateRecruitmentStrength(AiMain *ai, 
        Agent *donor_agent, double &str_score); 

	
	
	
	
	
	
	
	virtual void Commit_Unit
	(
		Agent * unit_in_question		
	);


	
	
	
	
	
	
	virtual Goal_Result Execute_Task(AiMain *ai, Plan *the_plan);
    BOOL IsExecutionHalted(AiMain *ai, Goal_Result &result); 
    virtual Goal_Result CheckCompletion(const sint32 build_count);

    virtual Goal_Result BuildTaskSolution(AiMain *ai, CityAgent *the_city, 
        Plan *the_plan);

    virtual Goal_Result GotoTaskSolution(AiMain *ai, const sint32 army_idx, 
        ArmyAgent *the_army, const sint32 transport_idx, ArmyAgent *the_transport, 
        MapPointData &dest_pos, const SUB_TASK_TYPE sub_task);

    virtual BOOL FindPathToTask(AiMain *ai, ArmyAgent *the_army,
        ArmyAgent *the_transport, MapPointData &goal_pos, 
        const SUB_TASK_TYPE sub_task, MapPointData &dest_pos); 

	virtual BOOL WithinRange(AiMain *ai, Agent *agent, 
							 const SUB_TASK_TYPE & sub_task);

    virtual BOOL FollowPathToTask(AiMain *ai, ArmyAgent *the_army, 
        ArmyAgent *the_transport, const SUB_TASK_TYPE sub_task, 
        MapPointData &dest_pos);    

    virtual void ArrivedAtTask(AiMain *ai, ArmyAgent *the_army, 
        ArmyAgent *the_transport, SUB_TASK_TYPE sub_task);
    virtual void CantReachTaskThisTurn(AiMain *ai, ArmyAgent *the_army);

	BOOL IsBeachAssault(AiMain *ai, const MapPointData &dest_pos);
	BOOL RetargetEmptyBeach(AiMain *ai, const MapPointData &dest_pos, const sint16 &continent, const uint32 &depth);

    virtual BOOL CantFollowPathToTask(AiMain *ai, ArmyAgent *the_army, const SUB_TASK_TYPE sub_task);

    virtual Goal_Result TryTransport(AiMain *ai, const sint32 dont_check, ArmyAgent *the_army, 
        MapPointData &dest_pos);

    virtual Agent * FindTranportInSquad(AiMain *ai, const sint32 dont_check, 
        ArmyAgent *the_army, MapPointData &dest_pos, sint32 &best_transport_idx); 

    virtual Goal_Result LoadTransport(AiMain *ai,
        const sint32 army_idx, ArmyAgent *the_army, 
        const sint32 transport_idx, ArmyAgent *the_transport, 
        MapPointData &dest_pos);

	
	virtual double Action_Bid(AiMain *ai, Agent *agent);

    virtual double GetDefenseStrength();
    virtual double GetAttackStrength();

	double AddMinMaxRandomRange(AiMain *ai,
								const double &value,
								const double &min_percent,
								const double &max_percent);
}; 






#endif __ARMY_GOAL_H__
