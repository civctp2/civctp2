









#pragma once
#ifndef _TRADEOFFERPOOL_H_
#define _TRADEOFFERPOOL_H_

#include "ObjPool.h"

#include "TradeOffer.h"

class CivArchive;
template <class T> class DynamicArray;

class TradeOfferPool : public ObjPool
{
private:
	
	DynamicArray<TradeOffer> *m_all_offers;
	

	
public:
	TradeOfferPool();
	TradeOfferPool(CivArchive &archive);
	~TradeOfferPool();

	TradeOfferData* AccessTradeOffer(const TradeOffer id)
	{
		return (TradeOfferData*)Access(id);
	}

	TradeOfferData* GetTradeOffer(const TradeOffer id)
	{
		return (TradeOfferData*)Get(id);
	}

	TradeOffer Create(Unit fromCity,
					  ROUTE_TYPE offerType,  sint32 offerResource,
					  ROUTE_TYPE askingType, sint32 askingResource,
					  Unit toCity);
	void Remove(TradeOffer offer);
	
	sint32 GetNumTradeOffers();
	TradeOffer GetTradeOffer(sint32 index);
	void ReRegisterOffers();

	void RemoveTradeOffersFromCity(Unit &city);
	void Serialize(CivArchive &archive);
	void AddFromNetwork(const TradeOffer &offer);
};

extern TradeOfferPool *g_theTradeOfferPool;
#endif
