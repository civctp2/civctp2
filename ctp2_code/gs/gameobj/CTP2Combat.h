//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Combat handling
// Id           : $Id$
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
// TEST_APP
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Veteran effect added.
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - Added a new combat option (28-Feb-2009 Maq)
//
//----------------------------------------------------------------------------

#ifndef CTP2_COMBAT_H__
#define CTP2_COMBAT_H__

#ifdef TEST_APP

typedef long sint32;
typedef unsigned long uint32;
#else

#include "GameEventManager.h"
#include "ConstRecord.h"	// g_theConstDB
#include "Unit.h"
#include "MapPoint.h"
#include "battle.h"
#include "dynarr.h"

#define k_COMBAT_WIDTH 4
#define k_COMBAT_HEIGHT 7

#endif

enum UNIT_TYPE {
	UNIT_TYPE_GROUND,
	UNIT_TYPE_NAVAL,
	UNIT_TYPE_AIR,
	UNIT_TYPE_FLANKER,
	UNIT_TYPE_NONCOMBAT,
};

#define k_AIR_COL 2
#define k_RANGED_COL 1
#define k_FRONT_COL 0
#define k_NONCOMBAT_COL 3

#define k_NON_FLANKER_PRIORITY 10000

class Army;
class Battle; 
class BattleEvent; 

class CombatUnit {
private:
	double m_offense, m_defense,
		m_strength, m_armor,
		m_ranged, m_hp;
	bool m_isVeteran;
	UNIT_TYPE m_type;
	sint32 m_preferredCol;
	sint32 m_priority;

	bool m_valid;

public:
	bool m_alreadyExploded;
	bool m_alreadyAttacked;
#ifdef TEST_APP
	uint32 m_id;
#else
	Unit m_unit;
#endif

	CombatUnit() { m_valid = false; }

#ifdef TEST_APP
	CombatUnit(double offense, double defense,
			   double strength, double armor,
			   double ranged, double hp,
			   UNIT_TYPE type, 
			   bool const isVeteran = false
			  );
#else
	CombatUnit(double offense, double defense,
			   double strength, double armor,
			   double ranged, double hp,
			   Unit &u);
#endif

	double GetOffense() const
	{
		return (m_isVeteran) 
			   ? m_offense + (m_offense * g_theConstDB->Get(0)->GetVeteranCoef() * 0.01)
			   : m_offense;
	}
	double GetDefense() { return m_defense; }
	double GetStrength() { return m_strength; }
	double GetArmor() { return m_armor; }
	double GetRangedAttack() const
	{
		return (m_isVeteran) 
			   ? m_ranged + (m_ranged * g_theConstDB->Get(0)->GetVeteranCoef() * 0.01)
			   : m_ranged;
	}
	UNIT_TYPE GetCombatType() { return m_type; }
	sint32 GetPreferredCol() { return m_preferredCol; }
	void SetPreferredCol(sint32 col) { m_preferredCol = col; }

	sint32 GetPriority() { return m_priority; }
	void SetPriority(sint32 pri) { m_priority = pri; }

	double GetHP() { return m_hp; }
	void DeductHP(double amt) 
#ifndef TEST_APP
		;
#else
	{ 
		m_hp -= amt; 
		if(m_hp < 0.001) {
			m_hp = 0;
		}
			
	}
#endif

	bool IsValid() { return m_valid; }
	bool IsAlive() { return m_hp > 0.001; }
	bool IsActive() { return IsValid() && IsAlive(); }

	void AddKill();

	void Invalidate() { m_valid = false; }
};

class CombatField {
private:
	CombatUnit **m_field;
	sint32 m_width, m_height;
	bool m_isOffense;

public:
	CombatField(sint32 width, sint32 height, bool isOffense);
	
	
	~CombatField();

#ifndef TEST_APP
	void FillFrom(CellUnitList &units);
	void ReportUnits(Battle *battle, BattleEvent *event, bool initial);
#endif

	void StartRound();

	CombatUnit &GetUnit(sint32 x, sint32 y);
	void SetUnit(sint32 x, sint32 y, CombatUnit *u);
	bool FindTargetForRangedAttackFrom(sint32 x, sint32 y, sint32 *dx, sint32 *dy);
	bool FindTargetForAttackFrom(sint32 x, sint32 y, sint32 *dx, sint32 *dy, bool isFlanker);
	sint32 NumAlive();

	void Move();
	void Sort();
	void InsertInPreferredCol(CombatUnit *u);
	
	sint32 CountColumn(sint32 col);
	void MoveRowForward(sint32 row);
	void MoveUnitsToFront(sint32 num);
	void FindOpponents(CombatField &opposite);
	void FillHoles();

#ifdef TEST_APP
	void Dump(bool flip);
#endif

};

class CTP2Combat
{
public:
#ifdef TEST_APP
	CTP2Combat(sint32 width, sint32 height, FILE *input);
#else
	CTP2Combat(sint32 width, sint32 height, CellUnitList &attackers, CellUnitList &defenders);

	
	~CTP2Combat();
#endif

	
	bool ResolveOneRound();
	bool IsDone();

	sint32 GetNumLivingAttackers() { return m_attackers.NumAlive(); }
	sint32 GetNumLivingDefenders() { return m_defenders.NumAlive(); }

	void Retreat();
	bool IsRetreated() { return m_retreated; }

#ifndef TEST_APP
	sint32 GetAttacker() { return m_attacker; }
	sint32 GetDefender() { return m_defender; }
	Army GetAttackerArmy();
	MapPoint GetDefenderLocation() { return m_defenderPos; }
	
	Battle *GetBattle() { return m_battle; }

	
	void KillBattle() { delete m_battle; m_battle = NULL; }
	void ClearBattle() { m_battle = NULL; } 
#endif

	void AddDeadUnit(Unit &u) { m_deadUnits.Insert(u); }
	void KillUnits(GAME_EVENT_INSERT priority);

private:
	CombatField m_attackers, m_defenders;
	sint32 m_width, m_height;
	sint32 m_round;
	bool m_retreated, m_retreating, m_noAttacksPossible;
	sint32 m_attacker, m_defender;
#ifndef TEST_APP
	uint32 m_army_id;
	MapPoint m_defenderPos;
	Battle *m_battle;
	sint32 m_roundsSinceUpdate;
	DynamicArray<Unit> m_deadUnits;
#endif

#ifdef TEST_APP
	void DumpState();
#endif

	void ExecuteRangedAttack(CombatField *attacker, sint32 attX, sint32 attY, 
							 CombatField *defender, sint32 defX, sint32 defY);
	void DoRangedAttacks(CombatField *attacker, CombatField *defender);
	void DoRangedAttacks();
	void DoRangedCounterAttacks();

	void ExecuteAttack(CombatField *attacker, sint32 attX, sint32 attY, 
					   CombatField *defender, sint32 defX, sint32 defY);
	void ExecuteCounterAttack(CombatField *attacker, sint32 attX, sint32 attY, 
					   CombatField *defender, sint32 defX, sint32 defY);
	void DoAttacks(CombatField *attacker, CombatField *defender);
	void DoCounterAttacks(CombatField *attacker, CombatField *defender);
	void DoAttacks();
	void DoCounterAttacks();
	void DoMovement();

	void Balance();
};

#ifndef TEST_APP
extern CTP2Combat *g_theCurrentBattle;
#endif

#endif

