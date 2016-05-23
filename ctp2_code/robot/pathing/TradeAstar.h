//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : A-star Trade route pathing algorithm
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - None
//
//----------------------------------------------------------------------------

#pragma once
#ifndef __TRADE_ASTAR_H__
#define __TRADE_ASTAR_H__ 1

#include "gstypes.h"
#include "Astar.h"

class TradeAstar : public Astar {

    PLAYER_INDEX m_owner;

	bool EntryCost(const MapPoint &prev, const MapPoint &pos,
                            float &cost, bool &is_zoc,
							ASTAR_ENTRY_TYPE &entry);

    sint32 GetMaxDir(MapPoint &pos) const;

public:

    bool FindPath(const PLAYER_INDEX owner, const MapPoint &start,
		const MapPoint &dest, Path &a_path, float &total_cost,
		const bool isunit);
};

#endif
