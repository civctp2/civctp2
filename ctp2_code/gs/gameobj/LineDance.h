
#pragma once
#ifndef __LINEDANCE_H__
#define __LINEDANCE_H__

#include "Unit.h"

class CellUnitList;

#define k_MAX_LINE_SIZE 9
#define k_RANGED_ROUNDS 10

class LineDance {
private:
	Unit m_attackers[k_MAX_LINE_SIZE];
	Unit m_defenders[k_MAX_LINE_SIZE];
	sint32 m_numAttackers;
	sint32 m_numDefenders;
	sint32 m_startAttackersRangeSlot;
	sint32 m_startDefendersRangeSlot;
	BOOL m_attackerIsDead[k_MAX_LINE_SIZE];
	BOOL m_defenderIsDead[k_MAX_LINE_SIZE];
	BOOL m_attackerUsedRangeFire[k_MAX_LINE_SIZE];
	CellUnitList *m_dead_attackers;
	CellUnitList *m_dead_defenders;

	void SortAttackers(CellUnitList &attackers);
	void SortDefenders(CellUnitList &defenders);
	
	void RemoveDeadDefender(sint32 index);
	void RemoveDeadAttacker(sint32 index);

	void DoRangedAttackerAttacks();
	void DoRangedDefenderAttacks();

	void DoAssaults();

public:
	LineDance(CellUnitList &attackers, CellUnitList &defenders);
	~LineDance();

	BOOL Resolve(CellUnitList &dead_attackers, CellUnitList &dead_defenders);

	sint32 GetNumAttackers() { return m_numAttackers; }
	sint32 GetNumDefenders() { return m_numDefenders; }
	Unit &GetAttacker(sint32 index);
	Unit &GetDefender(sint32 index);

	
};

		
#endif
