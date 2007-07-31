
#pragma once

#ifndef __BFS_H__
#define __BFS_H__ 1

#include "AstarPnt.h"
#include "PriorityQueue.h"

class AstarPoint; 
class MapPoint; 

class BestFirstSearch { 

    DAPriorityQueue<AstarPoint> m_priority_queue; 

public:
 
    BestFirstSearch();
    ~BestFirstSearch();

    BOOL InitPoint(const sint32 player_idx, AstarPoint *parent, AstarPoint *&node, MapPoint &pos, 
        const double past_cost, const double max_cost);
    sint32 FindNumCitiesAtHeight(const sint32 player_idx, const sint32 z_height);
	void FindMoveCostToCitiesZ(const sint32 player_idx, const sint32 z_height, const double max_cost,
        const double min_cost);

    void CalcDistToCapitol(const sint32 player_idx);

}; 

#endif __BFS_H__
