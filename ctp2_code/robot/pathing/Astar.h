#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef ASTAR_H
#define ASTAR_H 1

#include "Path.h"
#include "astarpnt.h"
#include "priorityqueue.h"

class MapPoint;
class AstarPoint;

const float k_ASTAR_BIG = 7654321.0f;

class Astar {

    DAPriorityQueue<AstarPoint> m_priority_queue;

protected:
    void DecayOrtho(AstarPoint *parent, AstarPoint *point,
        float &new_entry_cost);

    virtual sint32 EntryCost(const MapPoint &prev, const MapPoint &pos,
        float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry) = 0;

    virtual sint32 GetMaxDir(MapPoint &pos) const = 0;

    virtual void RecalcEntryCost(AstarPoint *parent,
        AstarPoint *node, float &new_entery_cost,
        BOOL &new_is_zoc, ASTAR_ENTRY_TYPE &entry);

    virtual sint32 InitPoint(AstarPoint *parent, AstarPoint *point,
        const MapPoint &pos, const float pc, const MapPoint &dest);

      sint32 Cleanup (const MapPoint &dest, Path &a_path, float &total_cost,
                   const sint32 isunit, AstarPoint *best,
                   AstarPoint *cost_tree);

public:

    BOOL m_pretty_path;

	Astar()
	{

		m_pretty_path = false;
	}
	virtual ~Astar() { };

    virtual float EstimateFutureCost(const MapPoint &pos, const MapPoint &dest);

    sint32 FindPath(const MapPoint &start, const MapPoint &dest,
                      Path &a_path, float &total_cost, const sint32 isunit,
                      const sint32 cutoff, sint32 &nodes_opened);

};

extern void Astar_Init();
extern void Astar_Cleanup();

#endif
