
#pragma once
#ifndef __C3TRADE_H__
#define __C3TRADE_H__

#include "IC3Trade.h"
#include "TradeOffer.h"

class C3TradeOffer : public IC3TradeOffer
{
private:
	ULONG m_refCount;
	
	PLAYER_INDEX m_aiPlayer;
	TradeOffer m_offer;
	
public:
	C3TradeOffer(const TradeOffer &offer, PLAYER_INDEX aiPlayer);
	C3TradeOffer(CivArchive &archive);
	~C3TradeOffer();
	void Serialize(CivArchive &archive);

	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	
	STDMETHODIMP_(uint32) GetID();
	STDMETHODIMP_(PLAYER_INDEX) GetOwner();
	STDMETHODIMP_(uint32) GetFromCity();
	STDMETHODIMP_(ROUTE_TYPE) GetOfferType();
	STDMETHODIMP_(sint32) GetOfferResource();
	STDMETHODIMP_(sint32) GetAskingGold();

	STDMETHODIMP_(void) Accept(uint32 destCity);
};

#endif

	
