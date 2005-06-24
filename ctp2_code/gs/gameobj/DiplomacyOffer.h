#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __DIPLOMACY_DATA_H__
#define __DIPLOMACY_DATA_H__

#include "Unit.h"
#include "GameObj.h"
#include "ID.h"
#include "TradeRouteData.h"

class CivArchive;

class TradeOfferData : public GAMEOBJ {
private:
	
	Unit m_fromCity;         
	ROUTE_TYPE m_offerType;  
	sint32 m_offerResource;  
	ROUTE_TYPE m_askingType; 
	sint32 m_askingResource; 
	

	friend class NetTradeOffer;

public:
	TradeOfferData(const ID id,
				   const Unit city, 
				   const ROUTE_TYPE offerType, const sint32 offerResource,
				   const ROUTE_TYPE askingType, const sint32 askingResource)
		: GAMEOBJ(id.m_id)
	{
		m_fromCity = city;
		m_offerType = offerType;
		m_offerResource = offerResource;
		m_askingType = askingType;
		m_askingResource = askingResource;
	}

	TradeOfferData(const ID id) : GAMEOBJ(id.m_id) {}

	TradeOfferData(CivArchive &archive) : GAMEOBJ(0) { Serialize(archive); };

	
	Unit GetFromCity() const { return m_fromCity; }
	ROUTE_TYPE GetOfferType() const { return m_offerType; }
	sint32 GetOfferResource() const { return m_offerResource; }
	ROUTE_TYPE GetAskingType() const { return m_askingType; }
	sint32 GetAskingResource() const { return m_askingResource; }

	
	
	void Serialize(CivArchive &archive);
};

#endif

