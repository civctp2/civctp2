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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Incorrect check repaired.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "GrabItem.h"

GrabItem	*g_grabbedItem = NULL;


GrabItem::GrabItem()
{
	m_isGrabbed = FALSE;
	m_grabbedItem = NULL;
	m_grabbedItemType = GRABITEMTYPE_NONE;
}

GrabItem::~GrabItem()
{
	
}

//----------------------------------------------------------------------------
//
// Name       : GrabItem::Init
//
// Description: (Re)initialise the global grab item.
//
// Parameters : -
//
// Globals    : g_grabbedItem
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

void GrabItem::Init(void)
{
	delete g_grabbedItem;

	g_grabbedItem = new GrabItem;
}

void GrabItem::Cleanup(void)
{
	if (g_grabbedItem) {
		delete g_grabbedItem;
		g_grabbedItem = NULL;
	}
}

void GrabItem::SetGrabbedItem(Unit *unit)
{
	m_grabbedItemType = GRABITEMTYPE_UNIT;
	m_grabbedItem = (void *)unit;
}

void GrabItem::GetGrabbedItem(Unit **unit)
{
	Assert(m_grabbedItemType == GRABITEMTYPE_UNIT);
	if (m_grabbedItemType != GRABITEMTYPE_UNIT) {
		*unit = NULL;
		return;
	}

	*unit = (Unit *)m_grabbedItem;
}

void GrabItem::SetGrabbedItem(TradeRoute *route)
{
	m_grabbedItemType = GRABITEMTYPE_TRADEROUTE;
	m_grabbedItem = (void *)route;
}

void GrabItem::GetGrabbedItem(TradeRoute **route)
{
	Assert(m_grabbedItemType == GRABITEMTYPE_TRADEROUTE);
	if (m_grabbedItemType != GRABITEMTYPE_TRADEROUTE) {
		*route = NULL;
		return;
	}

	*route = (TradeRoute *)m_grabbedItem;
}
