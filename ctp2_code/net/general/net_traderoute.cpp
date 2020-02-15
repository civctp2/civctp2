#include "c3.h"
#include "network.h"
#include "net_traderoute.h"
#include "net_util.h"

#include "TradeRouteData.h"
#include "TradePool.h"

#include "director.h"
#include "World.h"
#include "Cell.h"

#include "TradeDynArr.h"

#include "trademanager.h"

extern Director *g_director;
extern World *g_theWorld;

NetTradeRoute::NetTradeRoute(TradeRouteData* data, bool newRoute) :
	m_routeData(data),
	m_newRoute(newRoute)
{
}

void NetTradeRoute::Packetize(uint8 *buf, uint16 & size)
{
	sint32 i;

	buf[0] = k_PACKET_TRADE_ROUTE_ID >> 8;
	buf[1] = k_PACKET_TRADE_ROUTE_ID & 0xff;

	size = 2;

	PUSHLONG(m_routeData->m_id);

	PUSHBYTE(m_newRoute);

	PUSHBYTE((uint8)m_routeData->m_owner);
	PUSHBYTE((uint8)m_routeData->m_payingFor);

	PUSHLONG((uint32)(m_routeData->m_transportCost * 10000.0));
	PUSHBYTE((uint8)m_routeData->m_sourceRouteType);
	PUSHLONG(m_routeData->m_sourceResource);

	uint32 passesThrough = 0;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		passesThrough |= (m_routeData->m_passesThrough[i] << i);
	}
	PUSHLONG(passesThrough);
	PUSHBYTE(m_routeData->m_crossesWater);
	PUSHBYTE(m_routeData->m_isActive);
	PUSHLONG(m_routeData->m_color);
	PUSHLONG(m_routeData->m_seenBy);
	PUSHLONG(m_routeData->m_gold_in_return);

	PUSHLONG((uint32)(m_routeData->m_sourceCity));
	PUSHLONG((uint32)(m_routeData->m_destinationCity));
	PUSHLONG((uint32)(m_routeData->m_piratedLastTime));
	PUSHLONG((uint32)(m_routeData->m_piratingArmy.m_id));
	PUSHSHORT((uint16)(m_routeData->m_accumilatedTimesPirated));
	PUSHBYTE(m_routeData->m_valid);
	PUSHBYTE(m_routeData->m_pirate);

	PUSHSHORT((uint16)(m_routeData->m_path.Num()));
	for(i = 0; i < m_routeData->m_path.Num(); i++) {
		PUSHSHORT((uint16)m_routeData->m_path[i].x);
		PUSHSHORT((uint16)m_routeData->m_path[i].y);
	}
}

void NetTradeRoute::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos;
	uint32 transportCost;
	uint32 passesThrough;
	uint32 sourceCityID;
	uint32 destCityID;
	uint16 numWp;
	sint32 i;

	Assert(MAKE_CIV3_ID(buf[0], buf[1]) == k_PACKET_TRADE_ROUTE_ID);

	TradeRoute route(getlong(&buf[2]));

	g_network.CheckReceivedObject((uint32)route);

	if(!g_theTradePool->IsValid(route)) {
		m_routeData = new TradeRouteData(route);
	} else {
		m_routeData = g_theTradePool->AccessTradeRoute(route);
	}
	pos = 6;
	uint8 newRoute;
	PULLBYTE(newRoute);
	m_newRoute = newRoute != 0;

	PULLBYTETYPE(m_routeData->m_owner, PLAYER_INDEX);
	PULLBYTETYPE(m_routeData->m_payingFor, PLAYER_INDEX);

	PULLLONG(transportCost);
	m_routeData->m_transportCost = transportCost / 10000.;
	PULLBYTETYPE(m_routeData->m_sourceRouteType, ROUTE_TYPE);
	PULLLONG(m_routeData->m_sourceResource);

	PULLLONG(passesThrough);
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		m_routeData->m_passesThrough[i] = (passesThrough & (1 << i)) ? TRUE : FALSE;
	}
	PULLBYTE(m_routeData->m_crossesWater);
	PULLBYTE(m_routeData->m_isActive);
	PULLLONG(m_routeData->m_color);
	PULLLONG(m_routeData->m_seenBy);

	PULLLONG(m_routeData->m_gold_in_return);

	PULLLONG(sourceCityID);
	PULLLONG(destCityID);

	if(!g_theTradePool->IsValid(m_routeData->m_id)) {
		m_routeData->m_sourceCity = Unit(sourceCityID);
		m_routeData->m_destinationCity = Unit(destCityID);
	} else {
		if(m_routeData->m_sourceCity.m_id != sourceCityID) {
			m_routeData->m_sourceCity.DelTradeRoute(route);
			m_routeData->m_sourceCity = Unit(sourceCityID);
			m_routeData->m_sourceCity.AddTradeRoute(route);
		}

		if(m_routeData->m_destinationCity.m_id != destCityID) {
			m_routeData->m_destinationCity.DelTradeRoute(route);
			m_routeData->m_destinationCity = Unit(destCityID);
			m_routeData->m_destinationCity.AddTradeRoute(route);
		}
	}

	PULLLONG(m_routeData->m_piratedLastTime);
	PULLLONG(m_routeData->m_piratingArmy.m_id);
	PULLSHORT(m_routeData->m_accumilatedTimesPirated);
	PULLBYTE(m_routeData->m_valid);
	PULLBYTE(m_routeData->m_pirate);

	PULLSHORT(numWp);

	m_routeData->RemoveFromCells();

	m_routeData->m_path.Clear();
	PLAYER_INDEX owner;
	for(i = 0; i < numWp; i++) {
		MapPoint pnt;
		PULLSHORTTYPE(pnt.x, sint16);
		PULLSHORTTYPE(pnt.y, sint16);
		m_routeData->m_path.Insert(pnt);
        owner = m_routeData->m_sourceCity.GetOwner();

		g_theWorld->GetCell(pnt)->AddTradeRoute(route);
	}

	if(!g_theTradePool->IsValid(route)) {
		g_theTradePool->HackSetKey(((uint32)m_routeData->m_id & k_ID_KEY_MASK) + 1);
		g_theTradePool->Insert(m_routeData);
		m_routeData->m_sourceCity.AddTradeRoute(route, !m_newRoute);
		m_routeData->m_destinationCity.AddTradeRoute(route, !m_newRoute);
		g_theTradePool->m_all_routes->Insert(route);
		g_director->TradeActorCreate(route);
	}

	TradeManager::Notify();
}
