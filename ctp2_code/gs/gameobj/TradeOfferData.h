//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Trade Offer data
// Id           : $Id:$
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
// - None
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _TRADEOFFERDATA_H_
#define _TRADEOFFERDATA_H_

#include "Unit.h"
#include "GameObj.h"
#include "ID.h"
#include "TradeRouteData.h"

class CivArchive;

class TradeOfferData : public GameObj {
private:
	
	PLAYER_INDEX m_owner;
	Unit m_fromCity;         
	ROUTE_TYPE m_offerType;  
	sint32 m_offerResource;  
	ROUTE_TYPE m_askingType; 
	sint32 m_askingResource; 
	Unit m_toCity;           
	

	friend class NetTradeOffer;

public:
	TradeOfferData(const ID id,
				   const Unit city, 
				   const ROUTE_TYPE offerType, const sint32 offerResource,
				   const ROUTE_TYPE askingType, const sint32 askingResource,
				   const Unit toCity)
		: GameObj(id.m_id)
	{
		m_owner = city.GetOwner();
		m_fromCity = city;
		m_offerType = offerType;
		m_offerResource = offerResource;
		m_askingType = askingType;
		Assert(m_askingType == ROUTE_TYPE_GOLD);
		m_askingResource = askingResource;
		m_toCity = toCity;
	}

	TradeOfferData(const ID id) : GameObj(id.m_id) {}

	TradeOfferData(CivArchive &archive) : GameObj(0) { Serialize(archive); };

	
	PLAYER_INDEX GetOwner() const { return m_owner; }
	Unit GetFromCity() const { return m_fromCity; }
	Unit GetToCity() const { return m_toCity; }

	ROUTE_TYPE GetOfferType() const { return m_offerType; }
	sint32 GetOfferResource() const { return m_offerResource; }
	ROUTE_TYPE GetAskingType() const { return m_askingType; }
	sint32 GetAskingResource() const { return m_askingResource; }

	BOOL Accept(PLAYER_INDEX player, const Unit &sourceCity, Unit const &destCity);

	
	
	void Serialize(CivArchive &archive);
};

#endif
