
#include "c3.h"
#include "tradeutil.h"
#include "World.h"
#include "ResourceRecord.h"
#include "Unit.h"
#include "CityData.h"
#include "TradeAstar.h"
#include "Path.h"
#include "AgreementMatrix.h"
#include "constdb.h"

extern TradeAstar g_theTradeAstar; 

sint32 tradeutil_GetTradeValue(const sint32 owner, Unit &destination, sint32 resource)
{
	Assert(destination.IsValid());
	if(!destination.IsValid()) return 0;

	Assert(resource >= 0 && resource < g_theResourceDB->NumRecords());
	if(resource < 0 || resource >= g_theResourceDB->NumRecords())
		return 0;


	double baseValue = g_theWorld->GetGoodValue(resource);
	double distance = double(destination.CD()->GetDistanceToGood(resource));
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

	

	
	cost = (float)((int)((cost * g_theConstDB->GetCaravanCoef()) + 0.5));
	
	
	if(cost < 1)
		cost = 1;

	return sint32(cost);
}

sint32 tradeutil_GetTradeDistance(Unit &source, Unit &destination)
{
	Path path;
	float cost;

#if 0
	
	
	
	
	sint32 r = g_theTradeAstar.FindPath(source.GetOwner(), source.RetPos(),
										destination.RetPos(), path,
										cost, FALSE);
#endif

	cost = source.RetPos().NormalizedDistance(destination.RetPos());

	
	cost *= g_theWorld->CalcTerrainFreightCost(source.RetPos());

	
	
	
	cost = cost * g_theConstDB->GetCaravanCoef() + .5;
	if(cost < 1)
		cost = 1;

	return sint32(cost);
}
