#pragma once

#ifndef __RISKRECORD_H__
#define __RISKRECORD_H__

class CivArchive;
class Token;
enum C3DIR;
#include "Rec.h"

class RiskRecord : public Record
{
public:
	RiskRecord();
	RiskRecord(CivArchive &archive);
	void Serialize(CivArchive &archive);

	
	double m_barbarianHutChance;
	sint32 m_maxHutBarbarians;
	double m_goldChance;
	sint32 m_minGold;
	sint32 m_maxGold;
	double m_advanceChance;
	sint32 m_maxAdvancePrerequisites;
	double m_unitChance;
	sint32 m_maxUnitPrerequisites;
	double m_cityChance;
	double m_settlerChance;

	
	double m_barbarianChance;
	sint32 m_firstBarbarianTurn;
	sint32 m_lastBarbarianTurn;
	sint32 m_minimumBarbarianDistance;
	sint32 m_barbarianUnitRankMax;
	sint32 m_maxSpontaneousBarbarians;
	sint32 m_barbarianUnitRankMin;
	
};
#endif
