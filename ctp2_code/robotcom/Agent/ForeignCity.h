

#pragma once

#ifndef __FOREIGN_CITY_H__
#define __FOREIGN_CITY_H__ 1

#include "Agent.h"

struct MapPointData; 

class AiMain;


class ForeignCity : public Agent { 

    MapPointData *m_pos;

    PLAYER_INDEX m_player_index; 

	
	BOOL seige_goal_set;	
    BOOL m_has_enslave_goal; 
    BOOL m_been_nano_attacked; 

	sint32 enslave_me_goal_ID;

	
	sint32 m_convertMeGoalID;

	
	sint32 m_harassMeGoalID;

	AiMain		*m_ai;		

public:

  
    ForeignCity(); 
    ForeignCity(IC3CivArchive *archive); 
    ForeignCity(AiMain *ai, BSetID &id, PLAYER_INDEX pi);
    ~ForeignCity(); 
    void Serialize(IC3CivArchive *archive);
    void Init(); 
    void Hook(AiMain *ai); 

    void SetPos(const MapPointData &pos); 
    void GetPos(MapPointData &my_pos); 
    void GetXYPos(AiMain *ai, PLAYER_INDEX owner, MapPointData &my_pos); 

    PLAYER_INDEX GetPlayerIndex() const { return m_player_index; } 

    double GetAttackStrength() const;                   
    void GetDefenseStrength(AiMain *ai, float& attack, float& defend) const;
    sint32 GetNumDefenders(AiMain *ai); 
    double GetTargetValue() const; 

	double ProjectedHappiness(AiMain *ai) const;
	double ProjectedProduction(AiMain *ai) const;
	double ProjectedFood(AiMain *ai) const;

	
	
	void Remove_Enslave_Goal(AiMain *ai);

	
	
	

	void Set_Seige_Status(BOOL new_seige_status);
	BOOL Get_Seige_Status();

    double PretestEnslave(); 
    void SetHasEnslaveGoal(AiMain *ai, const BOOL b);	
    void SetEnslaveGoalID(AiMain *ai, sint32 goal_ID);	
    sint32 GetEnslaveGoalID() { return enslave_me_goal_ID; }
    void RemoveEnslaveGoal(); 

    double EstimateUtility(AiMain *ai, ArmyGoal *goal_obj);
    double Estimate_Attack_Strength(AiMain *ai);

	sint32 GetGoodCount(AiMain *ai, sint32 resource);
    void GetCityNumTradeRoutes(AiMain *ai, sint32 &in_num, sint32 &in_max, 
        sint32 &out_num, sint32 &out_max);


    sint32 GetPopulation(AiMain *ai); 
    sint32 GetProduction(AiMain *ai); 
    sint32 GetFood(AiMain *ai); 
    sint32 GetGold(AiMain *ai); 

    void SetBeenNanoAttacked() { m_been_nano_attacked = TRUE; } 
    BOOL HasBeenNanoAttacked() const { return m_been_nano_attacked; }
 
}; 

#endif __FOREIGN_CITY_H__
