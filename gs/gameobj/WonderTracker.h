
#pragma once
#ifndef __WONDER_TRACKER_H__
#define __WONDER_TRACKER_H__

class CivArchive;
class Unit;

#include "gstypes.h"

class WonderTracker {
private:
	uint64 m_builtWonders;
	
	
    uint64 m_buildingWonders[k_MAX_PLAYERS];
	uint32 m_globeSatFlags;

public:
	WonderTracker();
	WonderTracker(CivArchive &archive) { Serialize(archive); }
	void Serialize(CivArchive &archive);

	uint64 GetBuiltWonders() const { return m_builtWonders; }

	BOOL HasWonderBeenBuilt(sint32 which);
	PLAYER_INDEX WhoOwnsWonder(sint32 which);
	void AddBuilt(sint32 which);
	void SetBuiltWonders(uint64 built);
	BOOL GetCityWithWonder(sint32 which, Unit &city);

    void SetBuildingWonder(sint32 which, PLAYER_INDEX who);
    void ClearBuildingWonder(sint32 which, PLAYER_INDEX who);
    BOOL IsBuildingWonder(sint32 which, PLAYER_INDEX who);

	
	void RecomputeIsBuilding(const PLAYER_INDEX who);

	
	uint32 GlobeSatFlags() { return m_globeSatFlags; }
	void SetGlobeSatFlags(uint32 flags) { m_globeSatFlags = flags; }
};

extern WonderTracker *g_theWonderTracker;

#endif
