








#pragma once
#ifndef _READINESS_H_
#define _READINESS_H_

enum READINESS_LEVEL {
	READINESS_LEVEL_PEACE,
	READINESS_LEVEL_ALERT,
	READINESS_LEVEL_WAR
};

class CivArchive;
class Unit; 
template <class T> class DynamicArray;
class Army;

class MilitaryReadiness
{
private:
	
	
    double m_delta; 
    double m_hp_modifier; 
    double m_cost; 
    double m_percent_last_turn; 
	READINESS_LEVEL m_readinessLevel;
    BOOL m_ignore_unsupport; 
	sint32 m_owner;
	sint32 m_turnStarted;
    sint32 m_pad; 
	
	

	
	
	

	
	
	friend class NetReadiness;

public:
	MilitaryReadiness(sint32 o);
    
    void SetLevel(sint32 gov, DynamicArray<Army> &m_all_armies, 
              READINESS_LEVEL level, BOOL immediate = FALSE);
	READINESS_LEVEL GetLevel() const { return m_readinessLevel; }
    double GetCost() const { return m_cost; }

	void BeginTurn(sint32 gov);
	double GetSupportModifier(sint32 gov_type)const;
	double GetSpecialForcesSupportModifier(sint32 gov_type) const;
	double GetHPModifier()const;
    double GetReadyHP(sint32 gov, READINESS_LEVEL level);

    void SetPecentLastTurn(double &v) { m_percent_last_turn = v; }
    double GetPecentLastTurn() const { return m_percent_last_turn; } 

    void SupportUnit(const Unit &u, sint32 gov); 
	double GetSupportCost(const Unit &u);
    void UnsupportUnit(const Unit &u, sint32 gov); 
    void KillUnitsOverBudget(sint32 gov, DynamicArray<Army> &m_all_armies, sint32 mil_total);
	void RecalcCost();

    void Serialize(CivArchive &archive);

	sint32 GetTurnStarted() const { return m_turnStarted; }
	sint32 GetTurnsToNewReadiness();
};

#endif
