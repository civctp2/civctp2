#include "c3.h"
#include "TradePool.h"
#include "citylayer.h"
#include "TradeRouteData.h"
#include "director.h"
#include "TerrainRecord.h"
#include "pixelutils.h"
#include "colorset.h"           // g_colorSet
#include "TradeDynArr.h"
#include "profileDB.h"
#include "ResourceRecord.h"
#include "Globals.h"
#include "Events.h"
#include "GameEventUser.h"
#include "SelItem.h"            // g_selected_item


class aui_Surface;

TradePool::TradePool() : ObjPool(k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE)
{
	m_all_routes = new TradeDynamicArray;
}

TradePool::TradePool(CivArchive &archive) : ObjPool(k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE)
{
	m_all_routes = new TradeDynamicArray;
	Serialize(archive);
}

TradePool::~TradePool()
{
	sint32 i;
	for(i = m_all_routes->Num() - 1; i >= 0; i--) {
		m_all_routes->Access(i).Kill(CAUSE_KILL_TRADE_ROUTE_UNKNOWN);
	}
	delete m_all_routes;
}

TradeRoute TradePool::Create(Unit sourceCity,
							 Unit destCity,
							 PLAYER_INDEX owner,
							 ROUTE_TYPE sType,
							 sint32 sResource,
							 PLAYER_INDEX paying_for,
							 sint32 gold_in_return)
{
	TradeRouteData* newData;
	TradeRoute newRoute(NewKey(k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE));

	newData = new TradeRouteData(newRoute, sourceCity, destCity, owner, sType, sResource, paying_for, gold_in_return); // also sets actual transport cost
	if(!newData->IsValid()) {

		delete newData;
		return TradeRoute();
	}

	Insert(newData);
	newRoute.Activate(); // route activation should happen before AddTradeRoute, just in case
	newRoute.RevealTradeRouteStateIfInVision(); // reveal trade route state to players where route is in vision, must be after Activate()

	sourceCity.AddTradeRoute(newRoute); // activates trade route in case it is not
	destCity.AddTradeRoute(newRoute); // activates trade route in case it is not
	m_all_routes->Insert(newRoute);
	g_director->TradeActorCreate(newRoute);
	sourceCity.RecalculateResources();
	newRoute.RedrawRadarMapAlongRoute(); // must be called after TradeRouteData is set and route activation

	return newRoute;
}

void TradePool::Remove(TradeRoute route)
{
	m_all_routes->Del(route);
	Del(route);
}


TradeRoute TradePool::GetRouteIndex(sint32 index)
{
	Assert(index >= 0 && index < m_all_routes->Num());

	return m_all_routes->Access(index);
}

void TradePool::Draw(aui_Surface* surface)
{
	if(!g_theProfileDB->GetShowTradeRoutes())
		return;

	sint32 num = m_all_routes->Num();

    for (sint32 i = 0; i < num; i++) {
		TradeRoute route = m_all_routes->Access(i);

		if(route.SeenBy(g_selected_item->GetVisiblePlayer())){
		    DrawTradeRoute(surface, (DynamicArray<MapPoint>*)m_all_routes->Access(i).GetPath(),
			g_colorSet->GetPlayerColor(route.GetOwner()),
			g_colorSet->GetColor(COLOR_BLACK));
		    }

#if 0

		if (!m_all_routes->Access(i).IsSelectedPathSame())
		{

			if (m_all_routes->Access(i).GetPathSelectionState() == k_TRADEROUTE_SELECTED_PATH)
			{

			if(route.SeenBy(g_selected_item->GetVisiblePlayer())){
				DrawTradeRoute(surface, (DynamicArray<MapPoint>*)m_all_routes->Access(i).GetSelectedPath(),
					g_colorSet->GetColor(COLOR_SELECT_1),
					g_colorSet->GetColor(COLOR_BLACK));
			    }
			}
			else
			{

			if(route.SeenBy(g_selected_item->GetVisiblePlayer())){
				DrawTradeRoute(surface, (DynamicArray<MapPoint>*)m_all_routes->Access(i).GetSelectedPath(),
					g_colorSet->GetColor(COLOR_RED),
					g_colorSet->GetColor(COLOR_BLACK));
			    }
			}
		}
#endif
	}
}

void TradePool::Serialize(CivArchive &archive)
{
	TradeRouteData *tradeData;
	sint32	i,
			count = 0 ;

#define TRADEPOOL_MAGIC 0xFEDBACFE

    CHECKSERIALIZE

	if(archive.IsStoring()) {
		archive.PerformMagic(TRADEPOOL_MAGIC) ;
		ObjPool::Serialize(archive);

		for (i=0; i<k_OBJ_POOL_TABLE_SIZE; i++)
			if(m_table[i])
				count++;

		archive<<count;
		for(i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++) {
			if(m_table[i])
				((TradeRouteData*)(m_table[i]))->Serialize(archive);
		}
		m_all_routes->Serialize(archive);
	} else {
		archive.TestMagic(TRADEPOOL_MAGIC) ;
		ObjPool::Serialize(archive);

		archive>>count;
		for (i=0; i<count; i++) {
			tradeData = new TradeRouteData(archive);
			Insert(tradeData);
		}
		m_all_routes->Serialize(archive);
	}
}

void TradePool::RecreateActors()
{
	sint32 i;
	for(i = 0; i < m_all_routes->Num(); i++) {
		g_director->TradeActorCreate(m_all_routes->Access(i));
	}
}

sint32 TradePool::GetGoldValue(sint32 resource, sint32 n)
{
	return ((n * (n+1)) / 2) * g_theResourceDB->Get(resource)->GetGold();

#if 0
	sint32 terrain = resource % k_BASE_TERRAIN_TYPES;
 	sint32 good = resource / k_BASE_TERRAIN_TYPES;
 	sint32 gold = (n+1) *
		g_theTerrainDB->Get(terrain)->GetGood(good)->GetGoodGoldValue();
	return gold;
#endif
}

sint32 TradePool::GetSingleGoodValue(sint32 resource, sint32 nth_good)
{
	return (((nth_good * (nth_good + 1) / 2)
			- ((nth_good-1) * (nth_good) / 2 ))
			* g_theResourceDB->Get(resource)->GetGold() );
}

const TradeDynamicArray &TradePool::GetAllRoutes()
{
	return *m_all_routes;
}

void TradePool::BreakOffTrade(PLAYER_INDEX attack_owner, PLAYER_INDEX defense_owner, CAUSE_KILL_TRADE_ROUTE cause)
{
	sint32 i;

	for(i = 0; i < m_all_routes->Num(); i++) {
		TradeRoute route = m_all_routes->Access(i);
		if((route.GetSource().GetOwner() == attack_owner &&
			route.GetDestination().GetOwner() == defense_owner) ||
		   (route.GetDestination().GetOwner() == attack_owner &&
			route.GetSource().GetOwner() == defense_owner)) {

			g_gevManager->Pause();
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillTradeRoute,
				GEA_TradeRoute, route.m_id,
				GEA_Int, cause,
				GEA_End);
			g_gevManager->Resume();
		}
	}
}
