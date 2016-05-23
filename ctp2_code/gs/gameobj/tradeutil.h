//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Trade utilities
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
// - Added tradeutil_GetNetTradeCosts function for trade route costs
//   standardization. - June 5th 2005 Martin Gühmann
// - Added some functionality from the old const database. (5-Aug-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifndef TRADEUTIL_H__
#define TRADEUTIL_H__

#include "ConstRecord.h"
#include "ctp2_inttypes.h"
#include "Unit.h"
#include "Globals.h"         // MAPSIZE

sint32 const    DISTANCE_UNKNOWN    = 10000;

sint32 tradeutil_GetTradeValue(const sint32 owner, Unit const & destination, sint32 resource);
sint32 tradeutil_GetAccurateTradeDistance(Unit &source, Unit &destination);
sint32 tradeutil_GetTradeDistance(Unit &source, Unit &destination);
double inline tradeutil_GetNetTradeCosts(double costs){return(costs * g_theConstDB->Get(0)->GetCaravanCoef() * 0.1) + 0.5;}

void constutil_y2meridian(const sint32 y, sint32 &k);
void constutil_GetMapSizeMapPoint(MAPSIZE size, MapPoint &mapPoint);
#endif
