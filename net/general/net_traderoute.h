









#pragma once
#ifndef _NET_TRADEROUTE_H_
#define _NET_TRADEROUTE_H_

#include "net_packet.h"

class TradeRouteData;

class NetTradeRoute : public Packetizer {
public:
	NetTradeRoute(TradeRouteData*, bool newRoute);
	NetTradeRoute() {}

	void Packetize(uint8* buf, uint16& size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);

private:
	TradeRouteData* m_routeData;
	bool m_newRoute;
};

#endif

