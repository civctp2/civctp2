//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Trade utilities
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
// - Devided caravan costs by 10 so that we don't have to update const.txt.
//   This allows better mod compatibility. - May 7th 2005 Martin G�hmann
// - Removed .NET warnings - May 7th 2005 Martin G�hmann
// - Standardized trade route cost calculation. - June 5th 2005 Martin G�hmann
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "tradeutil.h"
#include "World.h"
#include "ResourceRecord.h"
#include "Unit.h"
#include "citydata.h"
#include "TradeAstar.h"
#include "Path.h"
#include "AgreementMatrix.h"

extern TradeAstar g_theTradeAstar;

sint32 tradeutil_GetTradeValue(const sint32 owner, const Unit &destination, sint32 resource)
{
	Assert(destination.IsValid());
	if(!destination.IsValid()) return 0;

	Assert(resource >= 0 && resource < g_theResourceDB->NumRecords());
	if(resource < 0 || resource >= g_theResourceDB->NumRecords())
		return 0;

	double baseValue = g_theWorld->GetGoodValue(resource);
	double distance = static_cast<double>(const_cast<Unit &>(destination).CD()->GetDistanceToGood(resource));
	sint32 totalValue = sint32(baseValue * distance);


	bool trade_pact =
		AgreementMatrix::s_agreements.HasAgreement(owner, destination.GetOwner(), PROPOSAL_TREATY_TRADE_PACT);
	if (trade_pact) {
		totalValue = (sint32) (totalValue * 1.05);
	}

	return totalValue;
}

sint32 tradeutil_GetAccurateTradeDistance(Unit &source, Unit &destination)
{

	Path path;
	float cost;

	sint32 r = g_theTradeAstar.FindPath(source.GetOwner(), source.RetPos(),
										destination.RetPos(), path,
										cost, FALSE);




	cost = tradeutil_GetNetTradeCosts(cost);

	if(cost < 1)
		cost = 1;

	return static_cast<sint32>(cost);
}

sint32 tradeutil_GetTradeDistance(Unit &source, Unit &destination)
{
	Path path;
	double cost;

#if 0




	sint32 r = g_theTradeAstar.FindPath(source.GetOwner(), source.RetPos(),
										destination.RetPos(), path,
										cost, FALSE);
#endif

	cost = static_cast<double>(source.RetPos().NormalizedDistance(destination.RetPos()));

	cost *= g_theWorld->CalcTerrainFreightCost(source.RetPos());




	cost = tradeutil_GetNetTradeCosts(cost);
	if(cost < 1)
		cost = 1;

	return static_cast<sint32>(cost);
}
