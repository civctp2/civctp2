//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : A-star pathfinding for City
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added owner argument to FindRoadPath function.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __CITY_ASTAR_H__
#define __CITY_ASTAR_H__ 1

#include "gstypes.h"
#include "Astar.h"

class CityAstar : public Astar {

    PLAYER_INDEX m_owner;
    uint32 m_alliance_mask;

	sint32 EntryCost(const MapPoint &prev, const MapPoint &pos,
                            float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry);
    sint32 GetMaxDir(MapPoint &pos) const;

public:

    void FindCityDist(PLAYER_INDEX owner, const MapPoint &start, const MapPoint &dest,
      float &cost);

	bool FindRoadPath(const MapPoint & start,
					  const MapPoint & dest,
					  PLAYER_INDEX owner,
					  Path & new_path,
					  float & total_cost);

private:
	bool m_pathRoad;
};

#endif
