//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : A-star pathfinding
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added includes to use MapPoint_List - Calvitix
// - Added avoidList Param and a IsInAvoidList method  - Calvitix
//----------------------------------------------------------------------------

#pragma once
#ifndef ASTAR_H
    #define ASTAR_H 1

    #include "AstarPnt.h"
    #include "PriorityQueue.h"

class MapPoint;
class AstarPoint;

const float k_ASTAR_BIG = 7654321.0f;

#if !defined (ACTIVISION_ORIGINAL)

    #if defined(_MSC_VER) && (_MSC_VER > 1000)
        #pragma warning(disable: 4786)
    #endif
    #include "Bit_Table.h"
    #if defined(_MSC_VER) && (_MSC_VER < 1300)
        #include <list-fixed>
    #else
        #include <list>
    #endif
    #include <utility>
    #include "MapPoint.h"
    #include "c3debugstl.h"

typedef std::list < MapPoint, dbgallocator < MapPoint > > MapPoint_List;
#endif


class Astar
{


    DAPriorityQueue < AstarPoint > m_priority_queue;

protected:
    void DecayOrtho(AstarPoint * parent, AstarPoint * point,
    float & new_entry_cost);

    virtual sint32 EntryCost(const MapPoint & prev, const MapPoint & pos,
    float & cost, BOOL & is_zoc, ASTAR_ENTRY_TYPE & entry) = 0;

    virtual sint32 GetMaxDir(MapPoint & pos) const = 0;

    virtual void RecalcEntryCost(AstarPoint * parent,
    AstarPoint * node, float & new_entery_cost,
    BOOL & new_is_zoc, ASTAR_ENTRY_TYPE & entry);


    virtual sint32 InitPoint(AstarPoint * parent, AstarPoint * point,
    const MapPoint & pos, const float pc, const MapPoint & dest);

    sint32 Cleanup(const MapPoint & dest, Path & a_path, float & total_cost,
    const sint32 isunit, AstarPoint * best,
    AstarPoint * cost_tree);

#if !defined (ACTIVISION_ORIGINAL)

	/** Check if the mapPoint pos is in the avoidList (List of point that are forbidden for the path) -Calvitix*/
	static BOOL IsInAvoidList(const MapPoint & pos, const MapPoint_List & avoidList);
#endif

public:

    BOOL m_pretty_path;

    Astar()
    {

        m_pretty_path = false;
    }

    virtual float EstimateFutureCost(const MapPoint & pos, const MapPoint & dest);

#if defined (ACTIVISION_ORIGINAL)
    sint32 FindPath(const MapPoint & start, const MapPoint & dest,
    Path & a_path, float & total_cost, const sint32 isunit,
    const sint32 cutoff, sint32 & nodes_opened);
#else
    sint32 FindPath(const MapPoint & start, const MapPoint & dest,
    Path & a_path, float & total_cost, const sint32 isunit,
    const sint32 cutoff, sint32 & nodes_opened, const MapPoint_List & avoidList);
#endif

};


extern void Astar_Init();
extern void Astar_Cleanup();

#endif


