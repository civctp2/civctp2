







#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _TRADEOFFER_H_
#define _TRADEOFFER_H_

#include "ID.h"
#include "Unit.h"
#include "TradeOfferData.h"

class TradeOffer : public ID {
public:
	TradeOffer () : ID() { return; } ; 
	TradeOffer (sint32 val) : ID (val) { return; }; 
	TradeOffer (uint32 val) : ID (val) { return; }; 
#ifdef WIN32
	TradeOffer (const int val) : ID (val) { return; }; 
#endif

	void KillOffer();
	void Kill() { KillOffer(); }
	void RemoveAllReferences();

	const TradeOfferData* GetData() const;
	TradeOfferData* AccessData();

	PLAYER_INDEX GetOwner() const { return GetData()->GetOwner(); }
	Unit GetFromCity() const { return GetData()->GetFromCity(); }
	Unit GetToCity() const { return GetData()->GetToCity(); }
	ROUTE_TYPE GetOfferType() const { return GetData()->GetOfferType(); }
	sint32 GetOfferResource() const { return GetData()->GetOfferResource(); }
	ROUTE_TYPE GetAskingType() const { return GetData()->GetAskingType(); }
	sint32 GetAskingResource() const { return GetData()->GetAskingResource(); }

	BOOL Accept(PLAYER_INDEX player, const Unit &sourceCity, Unit &destCity);

	void Castrate() {};
};

#endif
