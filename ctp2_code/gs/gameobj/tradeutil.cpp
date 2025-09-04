//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - Devided caravan costs by 10 so that we don't have to update const.txt.
//   This allows better mod compatibility. - May 7th 2005 Martin G�hmann
// - Removed .NET warnings - May 7th 2005 Martin G�hmann
// - Standardized trade route cost calculation. - June 5th 2005 Martin G�hmann
// - Added note that trade pact deals shouldn't be hardcoded 6.13.2007
// - Added some functionality from the old const database. (5-Aug-2007 Martin G�hmann)
// - Corrected meridian calculation. (29-Oct-2007 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "tradeutil.h"

#include <cmath>                    // std::round
#include "World.h"
#include "ResourceRecord.h"
#include "Unit.h"
#include "citydata.h"
#include "TradeAstar.h"
#include "Path.h"
#include "AgreementMatrix.h"
#include "ConstRecord.h"
#include "World.h"

extern TradeAstar g_theTradeAstar;

sint32 tradeutil_GetTradeValue(const sint32 owner, const Unit & destination, sint32 resource)
    {
    Assert(destination.IsValid());
    if(!destination.IsValid()) return 0;
    
    Assert(resource >= 0 && resource < g_theResourceDB->NumRecords());
    if(resource < 0 || resource >= g_theResourceDB->NumRecords())
	return 0;

    double baseValue = g_theWorld->GetGoodValue(resource); // good value varies between MIN_GOOD_VALUE and MAX_GOOD_VALUE (Const.txt) depening on its frequency on the map
    double baseValue2 = baseValue * (10.0*g_theConstDB->Get(0)->GetTradeCoef() - 10) / (100 - 10.0*g_theConstDB->Get(0)->GetTradeCoef()); // TradeCoef is the factor that the VPC (valuePerCaravan) should be increased in case a good needs not just 1 caravan but 10 caravans for creating the trade route, i.e. if TradeCoef= 1 the dependency is just linear (as before), if TradeCoef= 1.002 then the VPC is about 2 times for a good that needs 2 Caravans to trade (in comparison as if the good would only need 1 Caravan to trade)

    sint32 distToGood= destination.GetCityData()->GetDistanceToGood(resource);
    double tmpValue= baseValue * distToGood + baseValue2 * distToGood * distToGood; // linear + quadratic dependency
    sint32 totalValue= static_cast<sint32>(std::min<double>(tmpValue, std::numeric_limits<int>::max())); // limit totalValue to max value of sint32 before casting
    
    PLAYER_INDEX const  tradePartner = destination.GetOwner();
    if (owner == tradePartner) // reduce value to good in case of domestic trade (good is within territory then)
	{
	totalValue *= g_theConstDB->Get(0)->GetDomesticTradeReduction();
	}

    if ((owner != tradePartner)	&& AgreementMatrix::s_agreements.HasAgreement(owner, tradePartner, PROPOSAL_TREATY_TRADE_PACT))
	{
	totalValue = (sint32) (totalValue * g_theConstDB->Get(0)->GetTradePactCoef());
	}

    return static_cast<sint32>(std::max<double>(totalValue, 1.0)); // ensure that the trade value is >= 1
    }

sint32 tradeutil_GetTradeDistance(const Unit &source, const Unit &destination)
{
	Path    path;
	float   cost;

#ifdef USE_PRECISETRADEROUTECALC
// used to be tradeutil_GetAccurateTradeDistance which is exact but takes much longer to compute due to use of astar
	if (g_theTradeAstar.FindPath // comparable to TradeRouteData::GeneratePath cost calculation
            (source.GetOwner(), source.RetPos(), destination.RetPos(), path, cost, FALSE)
       )
    {
        return static_cast<sint32>(std::round(std::max<double>(tradeutil_GetNetTradeCosts(cost), 1.0)));
    }

	return DISTANCE_UNKNOWN;

#else // not USE_PRECISETRADEROUTECALC
// used to be tradeutil_GetTradeDistance which is a very rough calculation but is quick due to avoiding astar
	cost = 5 * // used to be g_theWorld->CalcTerrainFreightCost(source.RetPos()) * which was just returning 5 as a const in orig code: https://github.com/civctp2/civctp2/blob/d614fbdf705db334a4b45037bbcc735142d22016/ctp2_code/gs/world/wldgen.cpp#L1984-L1990
	    static_cast<double>(source.RetPos().NormalizedDistance(destination.RetPos()));

	return static_cast<sint32>(std::max<double>(tradeutil_GetNetTradeCosts(cost), 1.0));

#endif // USE_PRECISETRADEROUTECALC
}

// Maybe move the following to worldutils
void constutil_y2meridian(const sint32 y, sint32 &k)
{
	k = 0;

	sint32 relMapHeight = (y * 100) / g_theWorld->GetHeight();

	if     (relMapHeight < g_theConstDB->Get(0)->GetMeridianA())
	{
		k = 0;
	}
	else if(relMapHeight < g_theConstDB->Get(0)->GetMeridianB())
	{
		k = 1;
	}
	else if(relMapHeight < g_theConstDB->Get(0)->GetMeridianC())
	{
		k = 2;
	}
	else if(relMapHeight < g_theConstDB->Get(0)->GetMeridianD())
	{
		k = 3;
	}
	else if(relMapHeight < g_theConstDB->Get(0)->GetMeridianE())
	{
		k = 4;
	}
	else if(relMapHeight < g_theConstDB->Get(0)->GetMeridianF())
	{
		k = 5;
	}
	else
	{
		k = 6;
	}
}

void constutil_GetMapSizeMapPoint(MAPSIZE size, MapPoint &mapPoint)
{
	// Can be removed, when the map size only depends on the map database
	switch (size) {
	case MAPSIZE_SMALL:
		mapPoint.x = static_cast<sint32>(g_theConstDB->Get(0)->GetMapSizeSmall(0));
		mapPoint.y = static_cast<sint32>(g_theConstDB->Get(0)->GetMapSizeSmall(1));
#if !defined(_SMALL_MAPPOINTS)
		mapPoint.z = static_cast<sint32>(g_theConstDB->Get(0)->GetMapSizeSmall(2));
#endif
		break;
	case MAPSIZE_MEDIUM:
		mapPoint.x = static_cast<sint32>(g_theConstDB->Get(0)->GetMapSizeMedium(0));
		mapPoint.y = static_cast<sint32>(g_theConstDB->Get(0)->GetMapSizeMedium(1));
#if !defined(_SMALL_MAPPOINTS)
		mapPoint.z = static_cast<sint32>(g_theConstDB->Get(0)->GetMapSizeMedium(2));
#endif
		break;
	case MAPSIZE_LARGE:
		mapPoint.x = static_cast<sint32>(g_theConstDB->Get(0)->GetMapSizeLarge(0));
		mapPoint.y = static_cast<sint32>(g_theConstDB->Get(0)->GetMapSizeLarge(1));
#if !defined(_SMALL_MAPPOINTS)
		mapPoint.z = static_cast<sint32>(g_theConstDB->Get(0)->GetMapSizeLarge(2));
#endif
		break;
	case MAPSIZE_GIGANTIC:
		mapPoint.x = static_cast<sint32>(g_theConstDB->Get(0)->GetMapSizeGigantic(0));
		mapPoint.y = static_cast<sint32>(g_theConstDB->Get(0)->GetMapSizeGigantic(1));
#if !defined(_SMALL_MAPPOINTS)
		mapPoint.z = static_cast<sint32>(g_theConstDB->Get(0)->GetMapSizeGigantic(2));
#endif
		break;
	default :
		Assert(FALSE);
	}
}
