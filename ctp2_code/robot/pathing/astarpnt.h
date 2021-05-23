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
// None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added support for other compilers than MSVC6
// - queue index used
//
//----------------------------------------------------------------------------
//
/// \file   astarpnt.h
/// \brief  Point properties for the A-star pathfinding algorithm

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef ASTAR_POINT__
#define ASTAR_POINT__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class AstarPoint;

enum ASTAR_ENTRY_TYPE
{
	ASTAR_BLOCKED,
	ASTAR_RETRY_DIRECTION,
	ASTAR_CAN_ENTER,
	// ASTAR_ENTRY_TYPE_RESERVED

	// When having more than 4 types, k_ASTAR_ENTRY_MASK has to be updated
};

// Some bit masks to access properties of a point
#define k_ASTAR_ENTRY_MASK    0x00000003u // see ASTAR_ENTRY_TYPE
#define k_ASTAR_ZOC_MASK      0x00000004u
#define k_ASTAR_EXPANDED_MASK 0x00000008u
// Reserved                   0xfffffff0u

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "ctp2_inttypes.h"      // sintN, uintN
#include "MapPoint.h"           // MapPoint

//----------------------------------------------------------------------------
// Declarations
//----------------------------------------------------------------------------

class AstarPoint
{
public:
	AstarPoint() :
		m_flags       (0x00000000),
		m_pos         (),
		m_past_cost   (0.0f),
		m_entry_cost  (0.0f),
		m_future_cost (0.0f),
		m_total_cost  (0.0f),
		m_queue_idx   (-1),
		m_parent      (NULL),
		m_next        (NULL)
	{}

	/// Determine whether a route through this point costs less than a route
	/// through another point.
	/// \param rhs The other point
	/// \remarks   The route cost is - partly - an estimation.
	bool operator < (const AstarPoint & rhs) const
	{
		double cost = m_total_cost;
		double otherCost = rhs.m_total_cost;
		if (cost < otherCost) {
			return true;
		}
		if ((cost - otherCost) > 0.000001) {
			return false;
		}

		cost = m_past_cost + m_entry_cost;
		otherCost = rhs.m_past_cost + rhs.m_entry_cost;
		if (cost < otherCost) {
			return true;
		}
		if ((cost - otherCost) > 0.000001) {
			return false;
		}

		cost = m_entry_cost;
		otherCost = rhs.m_entry_cost;
		if (cost < otherCost) {
			return true;
		}
		if ((cost - otherCost) > 0.000001) {
			return false;
		}

		// Somewhat silly tie breaker for points with the same cost
		return (m_pos.x < rhs.m_pos.x) || ((m_pos.x == rhs.m_pos.x) && (m_pos.y < rhs.m_pos.y));
	}

	bool Identical(const AstarPoint & rhs) const;
	void Clear();

	void GetPos(MapPoint & pos) const { pos = m_pos; }

	sint32 GetPriorityQueueIndex() const { return m_queue_idx; }
	void SetPriorityQueueIndex(const sint32 new_idx) {
		Assert(new_idx < SHRT_MAX);
		m_queue_idx = sint16(new_idx);
	}

	void SetEntry(const ASTAR_ENTRY_TYPE e)
	{
		m_flags &= ~k_ASTAR_ENTRY_MASK;
		m_flags |= e;
	}

	ASTAR_ENTRY_TYPE GetEntry() const
	{
		return ASTAR_ENTRY_TYPE(m_flags & k_ASTAR_ENTRY_MASK);
	}

	void SetZoc(const bool zoc)
	{
		if (zoc) {
			m_flags |= k_ASTAR_ZOC_MASK;
		} else {
			m_flags &= ~k_ASTAR_ZOC_MASK;
		}
	}

	bool GetZoc() const
	{
		return m_flags & k_ASTAR_ZOC_MASK;
	}

	void SetExpanded(const bool expanded)
	{
		if (expanded) {
			m_flags |= k_ASTAR_EXPANDED_MASK;
		} else {
			m_flags &= ~k_ASTAR_EXPANDED_MASK;
		}
	}

	bool GetExpanded() const
	{
		return m_flags & k_ASTAR_EXPANDED_MASK;
	}

private:
	/// Some properties of this point
	uint32 m_flags;
	/// Position on the map
	MapPoint m_pos;
	/// Cost to reach this point from the starting point (lowest found so far)
	float m_past_cost;
	/// Cost to enter this point from an adjacent point
	float m_entry_cost;
	/// Estimated cost to reach the destination from here
	float m_future_cost;
	/// Estimated cost from the starting point to the destination via this point
	float m_total_cost;

	sint32 m_queue_idx;
	AstarPoint *m_parent;
	AstarPoint *m_next;

	friend class RobotAstar2;
	friend class Astar;
	friend class UnitAstar;
	friend class AVLHeap;
	friend class Path;
};

#endif
