//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - Added comments
//
//----------------------------------------------------------------------------
//
/// \file   AstarPnt.cpp
/// \brief  Point properties for the A-star pathfinding algorithm

#include "c3.h"
#include "astarpnt.h"

/// Determine whether this point is identical to another one.
/// \param rhs The other point
/// \remarks   Identical is defined as having the same map location.
bool AstarPoint::Identical(const AstarPoint &rhs) const
{
    return m_pos == rhs.m_pos;
}

/// Mark a point as unused.
/// \remarks Not really useful, except for debugging.
void AstarPoint::Clear()
{
    m_flags = 0xcdcdcdcdu;
    m_parent = (AstarPoint *) 0xcdcdcdcd; 
    m_next = (AstarPoint *) 0xcdcdcdcd; 
    m_pos.x = (sint16) 0xcdcd; 
    m_past_cost = (float) 0xcdcdcdcd; 
    m_entry_cost = (float) 0xcdcdcdcd; 
    m_future_cost = (float) 0xcdcdcdcd; 
    m_total_cost = (float) 0xcdcdcdcd;
}
