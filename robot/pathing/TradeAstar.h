
#pragma once
#ifndef __TRADE_ASTAR_H__
#define __TRADE_ASTAR_H__ 1

#include "gstypes.h"
#include "Astar.h"

class TradeAstar : public Astar { 

    PLAYER_INDEX m_owner; 

	sint32 EntryCost(const MapPoint &prev, const MapPoint &pos,                           
                            float &cost, BOOL &is_zoc, 
							ASTAR_ENTRY_TYPE &entry);

    sint32 GetMaxDir(MapPoint &pos) const;

public:

    sint32 FindPath(const PLAYER_INDEX owner, const MapPoint &start, 
		const MapPoint &dest, Path &a_path, float &total_cost, 
		const sint32 isunit);

     
};

#endif
