#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __STRENGTH_H__
#define __STRENGTH_H__

class Strengths;

enum STRENGTH_CAT {
	STRENGTH_CAT_NONE = -1,
	STRENGTH_CAT_KNOWLEDGE,
	STRENGTH_CAT_MILITARY,
	STRENGTH_CAT_POLLUTION,
	STRENGTH_CAT_TRADE,
	STRENGTH_CAT_GOLD,
	STRENGTH_CAT_POPULATION,
	STRENGTH_CAT_CITIES,
	STRENGTH_CAT_GEOGRAPHICAL,
	STRENGTH_CAT_SPACE,
	STRENGTH_CAT_UNDERSEA,
	STRENGTH_CAT_UNITS,
	STRENGTH_CAT_BUILDINGS,
	STRENGTH_CAT_WONDERS,
	STRENGTH_CAT_PRODUCTION,
	STRENGTH_CAT_MAX
};

#include "civarchive.h"     // CivArchive
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
	sint32 GetStrength(STRENGTH_CAT category) const;
	sint32 GetTurnStrength(STRENGTH_CAT category, sint32 turn) const;

	sint32 GetTotalUnitCost() const;
	sint32 GetTotalBuildingCost() const;
	sint32 GetTotalWonderCost() const;
	sint32 GetTotalProduction() const;
};

#endif
