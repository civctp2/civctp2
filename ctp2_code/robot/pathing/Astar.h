//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : A-star pathfinding
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

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef ASTAR_H
#define ASTAR_H 1

#include "astarpnt.h"
#include "priorityqueue.h"

class MapPoint;
class AstarPoint;

const float k_ASTAR_BIG = 7654321.0f;

class Astar
{
	DAPriorityQueue<AstarPoint> m_priority_queue;

protected:
	void DecayOrtho(AstarPoint *parent, AstarPoint *point,
	    float &new_entry_cost);

	virtual bool EntryCost(const MapPoint &prev, const MapPoint &pos,
	    float &cost, bool &is_zoc, ASTAR_ENTRY_TYPE &entry) = 0;

	virtual sint32 GetMaxDir(MapPoint &pos) const = 0;

	virtual void RecalcEntryCost(AstarPoint *parent,
	    AstarPoint *node, float &new_entery_cost,
	    bool &new_is_zoc, ASTAR_ENTRY_TYPE &entry);

	virtual bool InitPoint(AstarPoint *parent, AstarPoint *point,
	    const MapPoint &pos, const float pc, const MapPoint &dest);

	bool Cleanup(const MapPoint &dest,
	             Path &a_path,
	             float &total_cost,
	             const bool isunit,
	             AstarPoint *best,
	             AstarPoint *cost_tree);

	sint32 m_maxSquaredDistance;

public:

	bool m_pretty_path;

	Astar()
	: m_maxSquaredDistance (-1),
	  m_pretty_path        (false)
	{
	};

	virtual ~Astar() { };

	virtual float EstimateFutureCost(const MapPoint &pos, const MapPoint &dest);

	bool FindPath(const MapPoint &start,
	              const MapPoint &dest,
	              Path &a_path,
	              float &total_cost,
	              const bool isunit,
	              const sint32 cutoff,
	              sint32 &nodes_opened);
};

extern void Astar_Init();
extern void Astar_Cleanup();

#endif
