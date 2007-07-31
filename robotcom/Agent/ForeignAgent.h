

#pragma once

#ifndef __FOREIGN_AGENT_H__
#define __FOREIGN_AGENT_H__ 1

#include "Agent.h"

enum ARMY_TARGET_TYPE { 
    ARMY_TARGET_TYPE_MILITARY,
    ARMY_TARGET_TYPE_ENSLAVE, 
    ARMY_TARGET_TYPE_RUSTLE, 
    ARMY_TARGET_TYPE_EXPEL, 
    ARMY_TARGET_TYPE_UNKNOWN
};



#define FOREIGNAGENT_DEFAULT_THREAT (10.0)

class AiMain; 
struct MapPointData; 

class ForeignAgent : public Agent { 

	PLAYER_INDEX m_player_index;

    sint32 m_top_unit_type; 
    sint32 m_unit_num;
    double m_defense_strength; 
    double m_attack_strength; 
    double m_target_value; 
    sint32 m_has_peeked; 

	AiMain *ai;

    MapPointData *m_pos; 
    MapPointData *m_XYpos; 

    BOOL m_has_enslave_goal; 
	sint32 enslave_me_goal_ID;

	
	sint32 m_bombardMeGoalID; 
	sint32 m_rustleMeGoalID; 
	sint32 m_expelMeGoalID; 
	sint32 m_sallyMeGoalID; 
    sint32 m_lastSeen;      

public:
	Goal * attack_me_goal;	

public:

    ForeignAgent();
    ForeignAgent(IC3CivArchive *archive); 

    ForeignAgent(PLAYER_INDEX player_index, AiMain *init_ai, BSetID &id);
    void Init();

    ~ForeignAgent(); 

    void Serialize(IC3CivArchive *archive);
	void Hook(AiMain *ai); 

    sint32 GetPlayerIndex() const { return m_player_index; }

    void SetCurPos (const MapPointData &pos); 
    void GetPos(MapPointData &my_pos);

    void SetUnitNum(const sint32 unit_num); 
    sint32 GetUnitNum(); 
    
    void SetCurXYPos ( const MapPointData &pos ); 
    void GetXYPos(MapPointData &my_pos);
    
    void SetTopUnit( sint32 top_unit_type); 
    sint32 GetTopUnitType();     
    
    void CalcDefense();
    void CalcThreat();
    void CalcTarget();
    double GetAttackStrength() const { return m_attack_strength; }  
    double GetDefenseStrength() const { return m_defense_strength; } 
    double GetTargetValue() const;
	double Get_Threat_Value();

    double GetMaxMovePoints(); 

    double PretestEnslave(); 
    void SetHasEnslaveGoal(const BOOL b); 
    void SetEnslaveGoalID(sint32 goal_ID); 
    sint32 GetEnslaveGoalID() { return enslave_me_goal_ID; }
    void RemoveEnslaveGoal(); 

    BOOL IsLoneSettler(AiMain *ai); 
    BOOL IsStealthy(AiMain *ai); 
	BOOL IsSpaceAttack(AiMain *ai);
	BOOL IsSpaceLand(AiMain *ai);

	void SetupSpecialGoals(AiMain *ai);
	void SetupSallyGoal(AiMain *ai);
	void RemoveBombardGoal(AiMain *ai, const sint32 &goalID);
	void RemoveRustleGoal(AiMain *ai, const sint32 &goalID);
	void RemoveExpelGoal(AiMain *ai, const sint32 &goalID);
	void RemoveSallyGoal(AiMain *ai, const sint32 &goalID);
    sint32 GetRustleGoalID() { return m_rustleMeGoalID; }
    sint32 GetExpelGoalID() { return m_expelMeGoalID; }
    sint32 GetSallyGoalID() { return m_sallyMeGoalID; }


	
	
	
	
	
	
	double Estimate_Army_Strength(AiMain *ai);

	
	
	
	
	
	
	double Estimate_Army_Attack_Strength(AiMain *ai);

	
	
	
	
	
	
	void Get_Army_Attack_And_Defense_Strengths
	(
		AiMain *ai, 
		float & attack, 
		float & defend
	);

    ARMY_TARGET_TYPE GetTargetType();
    sint32 GetLastSeen();
    void SetLastSeen(const sint32 &round);
}; 

#endif __FOREIGN_AGENT_H__
