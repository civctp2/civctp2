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
// - PRINT_COSTS
//   If defined the total expected costs will be printed onto each tile,
//   while pathing and if /debugcells has been enabled via the chat window
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

// Define PRINT_COSTS to see the costs at the tiles the A* algorithm is checking
//#define PRINT_COSTS 1

const float k_ASTAR_BIG = 7654321.0f;

class Astar
{
private:
	// pre-allocated queue for performance
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

	bool InitPoint(AstarPoint *parent, AstarPoint *point,
	    const MapPoint &pos, const float pc, const MapPoint &dest);

	bool Cleanup(const MapPoint &dest,
	             Path &a_path,
	             float &total_cost,
	             const bool isunit,
	             AstarPoint *best);

#ifdef PRINT_COSTS
	void PrintCosts(MapPoint pos, uint8 magnitude, float costs);
	void ResetPrintedCosts();
#endif

	sint32 m_maxSquaredDistance;

public:

	Astar()
	: m_maxSquaredDistance (-1)
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
