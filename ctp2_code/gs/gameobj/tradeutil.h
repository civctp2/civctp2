//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Trade utilities
// Id           : $Id:$
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
//
//----------------------------------------------------------------------------

#ifndef TRADEUTIL_H__
#define TRADEUTIL_H__

#include "ConstDB.h"

class Unit;

sint32 tradeutil_GetTradeValue(const sint32 owner, Unit &destination, sint32 resource);
sint32 tradeutil_GetAccurateTradeDistance(Unit &source, Unit &destination);
sint32 tradeutil_GetTradeDistance(Unit &source, Unit &destination);
double inline tradeutil_GetNetTradeCosts(double costs){return(costs * g_theConstDB->GetCaravanCoef() * 0.1) + 0.5;}

#endif
