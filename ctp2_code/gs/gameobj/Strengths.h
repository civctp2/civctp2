#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __STRENGTH_H__
#define __STRENGTH_H__

#include "ctp2_enums.h"

class Strengths;

#include "CivArchive.h"     // CivArchive
#include "SimpleDynArr.h"

class Strengths {
private:
	sint32 m_owner;

	SimpleDynamicArray<sint32> m_strengthRecords[STRENGTH_CAT_MAX];

	friend class NetStrengths;
	friend class NetFullStrengths;
	friend class Network;

public:
	Strengths(sint32 owner);
	Strengths(CivArchive &archive);

	~Strengths();
	void Serialize(CivArchive &archive);

	void Calculate();
	sint32 GetStrength(STRENGTH_CAT category);
	sint32 GetTurnStrength(STRENGTH_CAT category, sint32 turn);

	sint32 GetTotalUnitCost();
	sint32 GetTotalBuildingCost();
	sint32 GetTotalWonderCost();
	sint32 GetTotalProduction();
};

#endif
