//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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
// _MSC_VER		
// - When defined, allows Microsoft C++ extensions.
// - When not defined, generates standard C++.
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks 
//       between #else and #endif are modified Apolyton code. The blocks 
//       between #if and #else are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragma once marked as Microsoft specific
// - queue index used
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __ASTAR_POINT__
#define __ASTAR_POINT__ 1

#include "gstypes.h"
#include "MapPoint.h"

class AstarPoint;

enum ASTAR_ENTRY_TYPE { 
	ASTAR_BLOCKED,
	ASTAR_RETRY_DIRECTION,
	ASTAR_CAN_ENTER
};

#define k_ASTAR_ENTRY_MASK 0x03

class AstarPoint {
	
	
    uint32 m_flags; 
	MapPoint m_pos; 
	float m_past_cost, m_entry_cost, m_future_cost, m_total_cost; 
	
    sint32 m_queue_idx; 
    AstarPoint *m_parent; 
    AstarPoint *m_next; 

public:


	friend class RobotAstar; 
	friend class RobotAstar2; 
    friend class Astar; 
    friend class UnitAstar; 
    friend class AstarQueue; 
    friend class AVLHeap; 
    friend class Path;

    friend sint32 count_point(AstarPoint *node, const MapPoint  &pos);
    friend sint32 check_for_dups(AstarPoint *cost_tree, AstarPoint *node);



    AstarPoint() { 
        m_parent = NULL;
        m_next = NULL; 
        m_flags = 0; 
        SetExpanded(FALSE); 
    }

	
	
	
	
	inline sint32 AstarPoint::operator<(const AstarPoint &rhs) const
	{ 
		if (m_total_cost < rhs.m_total_cost) {
			return TRUE;
		}
	   
	    if ((m_total_cost - rhs.m_total_cost) > 0.000001) {
	        return FALSE;
		}

	    if (m_pos.x != rhs.m_pos.x) {
			return(m_pos.x < rhs.m_pos.x);
		}
	    return(m_pos.y < rhs.m_pos.y);
	}


    sint32 Identical(const AstarPoint &rhs) const;
    void Clear(); 
    
    inline void GetPos(MapPoint &pos) { pos = m_pos; }

	
	
	
	
	
	
    inline sint32 GetPriorityQueueIndex() const { return m_queue_idx; } 
    inline void SetPriorityQueueIndex(const sint32 new_idx) {
        Assert(new_idx < SHRT_MAX); 
        m_queue_idx = sint16(new_idx); 
    } 

    inline void SetEntry(const ASTAR_ENTRY_TYPE e)
    {
        m_flags &= ~0x3; 
        m_flags |= e;
    }

    inline ASTAR_ENTRY_TYPE GetEntry() const 
    { 
        return ASTAR_ENTRY_TYPE(m_flags & k_ASTAR_ENTRY_MASK);
    }

    inline void SetZoc(const BOOL z)
    {
        if (z) { 
            m_flags |= (0x1 << 2);  
        } else { 
            m_flags &= ~(0x1 << 2);  
        }
    }

    inline BOOL GetZoc() const
    { 
        return BOOL(m_flags & 0x4);
    } 

    inline void SetExpanded(const BOOL e)
    {
        if (e) { 
            m_flags |= (0x1 << 3);  
        } else { 
            m_flags &= ~(0x1 << 3);  
        }
    }

    inline BOOL GetExpanded() const
    {
        return BOOL(m_flags & 0x8);  
    }


};

#endif
