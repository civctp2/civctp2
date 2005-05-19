









#include "c3.h"
#include "network.h"
#include "net_tradeoffer.h"
#include "net_util.h"

#include "TradeOfferPool.h"
#include "TradeOfferData.h"
#include "TradeOffer.h"
#include "UnitPool.h"

#include "player.h"
extern Player **g_player;
extern UnitPool *g_theUnitPool;

NetTradeOffer::NetTradeOffer(TradeOfferData* data) : m_offerData(data)
{
}

void NetTradeOffer::Packetize(uint8* buf, uint16 &size)
{
	buf[0] = k_PACKET_TRADE_OFFER_ID >> 8;
	buf[1] = k_PACKET_TRADE_OFFER_ID & 0xff;

	size = 2;
	PUSHLONG(m_offerData->m_id);
	PUSHLONG(m_offerData->m_owner);
	PUSHLONG(m_offerData->m_fromCity.m_id);
	PUSHBYTE(m_offerData->m_offerType);
	PUSHLONG(m_offerData->m_offerResource);
	PUSHBYTE(m_offerData->m_askingType);
	PUSHLONG(m_offerData->m_askingResource);
	PUSHLONG(m_offerData->m_toCity.m_id);
}

void NetTradeOffer::Unpacketize(uint16 id, uint8* buf, uint16 size)
{
	sint32 pos;
	sint32 fromCity;
	sint32 toCity;

	Assert(MAKE_CIV3_ID(buf[0], buf[1]) == k_PACKET_TRADE_OFFER_ID);
	TradeOffer offer(getlong(&buf[2]));

	g_network.CheckReceivedObject((uint32)offer);

	if(!g_theTradeOfferPool->IsValid(offer)) {
		m_offerData = new TradeOfferData(offer);
	} else {
		m_offerData = g_theTradeOfferPool->AccessTradeOffer(offer);
	}

	pos = 6;
	PULLLONG(m_offerData->m_owner);
	PULLLONG(fromCity);
	m_offerData->m_fromCity = Unit(fromCity);
	PULLBYTETYPE(m_offerData->m_offerType, ROUTE_TYPE);
	PULLLONG(m_offerData->m_offerResource);
	PULLBYTETYPE(m_offerData->m_askingType, ROUTE_TYPE);
	PULLLONG(m_offerData->m_askingResource);
	PULLLONG(toCity);
	m_offerData->m_toCity = Unit(toCity);

	if(!g_theTradeOfferPool->IsValid(offer)) {
		g_theTradeOfferPool->HackSetKey(((uint32)offer & k_ID_KEY_MASK) + 1);
		g_theTradeOfferPool->Insert(m_offerData);
		g_theTradeOfferPool->AddFromNetwork(offer);
	}
}

