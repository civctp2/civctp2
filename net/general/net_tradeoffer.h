









#pragma once
#ifndef _NET_TRADEOFFER_H_
#define _NET_TRADEOFFER_H_

#include "net_packet.h"

class TradeOfferData;

class NetTradeOffer : public Packetizer {
public:
	NetTradeOffer(TradeOfferData*);
	NetTradeOffer() {}

	void Packetize(uint8* buf, uint16& size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);

private:
	TradeOfferData* m_offerData;
};

#endif

