//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Readiness modifiers
// Id           : $Id:$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// HAVE_PRAGMA_ONCE
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Fixed gold support cost so that it does not break the save game format. (April 29th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _READINESS_H_
#define _READINESS_H_

class MilitaryReadiness;

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

//----------------------------------------------------------------------------
// Do not change anything in the types or order of the following variable 
// declarations. Doing so will break reading in of save files.
// See the Serialize implementation for more details.
//----------------------------------------------------------------------------

	double m_delta;
	double m_hp_modifier;
	double m_cost;
	double m_percent_last_turn;
	READINESS_LEVEL m_readinessLevel;
	BOOL m_ignore_unsupport;
	sint32 m_owner;
	sint32 m_turnStarted;
	sint32 m_costGold; //EMOD modify MilitaryReadiness::Serialize if you want to have this but you don't have a getter method anyway so why have it at all?

//----------------------------------------------------------------------------
// Adding anything below will also break reading in of save files.
//----------------------------------------------------------------------------

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
	void SupportUnitGold(const Unit &u, sint32 gov); //EMOD
	double GetSupportCost(const Unit &u);
	sint32 GetSupportCostGold(const Unit &u); //EMOD
	void UnsupportUnit(const Unit &u, sint32 gov); 
	void KillUnitsOverBudget(sint32 gov, DynamicArray<Army> &m_all_armies, sint32 mil_total);
	void RecalcCost();
	//void RecalcCostGold(); //EMOD
	sint32 TotalUnitGoldSupport(); //EMOD

	void Serialize(CivArchive &archive);

	sint32 GetTurnStarted() const { return m_turnStarted; }
	sint32 GetTurnsToNewReadiness();
};

#endif
