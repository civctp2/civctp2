#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __EXCLUSIONS_H__
#define __EXCLUSIONS_H__

class CivArchive;

class Exclusions
{
private:
	sint32 m_numUnits;
	sint32 m_numBuildings;
	sint32 m_numWonders;

	sint32 *m_units;
	sint32 *m_buildings;
	sint32 *m_wonders;
	friend class NetExclusions;

public:
	Exclusions();
	Exclusions(CivArchive &archive) { Serialize(archive); }
	~Exclusions();

	void Serialize(CivArchive &archive);

	sint32 IsUnitExcluded(sint32 type) { return m_units[type]; }
	sint32 IsBuildingExcluded(sint32 type) { return m_buildings[type]; }
	sint32 IsWonderExcluded(sint32 type) { return m_wonders[type]; }

	void ExcludeUnit(sint32 type, sint32 exclude) { m_units[type] = exclude; }
	void ExcludeBuilding(sint32 type, sint32 exclude) { m_buildings[type] = exclude; }
	void ExcludeWonder(sint32 type, sint32 exclude) { m_wonders[type] = exclude; }
};

extern Exclusions *g_exclusions;
#endif
