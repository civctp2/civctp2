








#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _TRADEPOOL_H_
#define _TRADEPOOL_H_

#include "ObjPool.h"

#include "TradeRoute.h"

class TradeRouteData;
enum ROUTE_TYPE;
class aui_Surface;
class TradeDynamicArray;

class TradePool : public ObjPool
{
    
	

	
	

	
	
	
	TradeDynamicArray *m_all_routes;
	
	

	friend class NetTradeRoute;
	friend class Network;

public:
	TradeRouteData* AccessTradeRoute(const TradeRoute id)
	{
		return (TradeRouteData*) Access(id);
	}

	TradeRouteData* GetTradeRoute(const TradeRoute id) const
	{
		return (TradeRouteData*) Get(id);
	}

	TradePool();
	TradePool(CivArchive &archive);
	~TradePool();

	TradeRoute Create(Unit sourceCity, Unit destCity, PLAYER_INDEX owner,
					  ROUTE_TYPE sType, sint32 sResource,
					  PLAYER_INDEX paying_for,
					  sint32 gold_in_return);
	void Remove(TradeRoute route);
	TradeRoute GetRouteIndex(sint32 index);

	void Draw(aui_Surface* surface);
	void Serialize(CivArchive &archive);
	void RecreateActors();

	
	sint32 GetSingleGoodValue(sint32 resource, sint32 nth_good);


	const TradeDynamicArray &GetAllRoutes();
	TradeDynamicArray *AccessAllRoutes() { return m_all_routes; }

	sint32 GetGoldValue(sint32 resource, sint32 num);
	void BreakOffTrade(PLAYER_INDEX pl1, PLAYER_INDEX pl2);
};

extern TradePool* g_theTradePool;
#else

class TradePool;

#endif
