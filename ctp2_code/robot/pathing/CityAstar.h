
#pragma once
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
					  Path & new_path,
					  float & total_cost);

private:
	bool m_pathRoad;
};

#endif
