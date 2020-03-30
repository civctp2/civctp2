#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _TRADEROUTE_H_
#define _TRADEROUTE_H_

class TradeRoute;

enum CAUSE_KILL_TRADE_ROUTE
{
	CAUSE_KILL_TRADE_ROUTE_UNKNOWN,               //  0
	CAUSE_KILL_TRADE_ROUTE_SENDER_KILLED,         //  1
	CAUSE_KILL_TRADE_ROUTE_RECIPIENT_KILLED,      //  2
	CAUSE_KILL_TRADE_ROUTE_PIRATED,               //  3
	CAUSE_KILL_TRADE_ROUTE_CITY_CHANGED_OWNER,    //  4
	CAUSE_KILL_TRADE_ROUTE_CITY_DIED,             //  5
	CAUSE_KILL_TRADE_ROUTE_DIPLOMATIC_AGREEMENT,  //  6
	CAUSE_KILL_TRADE_ROUTE_CHANGED_DESTINATION,   //  7
	CAUSE_KILL_TRADE_ROUTE_NO_MORE_CARAVANS,      //  8
	CAUSE_KILL_TRADE_ROUTE_NO_INITIAL_CARAVANS,   //  9
	CAUSE_KILL_TRADE_ROUTE_WAR,                   // 10
	CAUSE_KILL_TRADE_ROUTE_EMBARGO,               // 11
	CAUSE_KILL_TRADE_ROUTE_RESET,                 // 12
	CAUSE_KILL_TRADE_ROUTE_BETTER_OFFER,          // 13
};

enum ROUTE_TYPE
{
	ROUTE_TYPE_RESOURCE,
	ROUTE_TYPE_FOOD,
	ROUTE_TYPE_GOLD,
	ROUTE_TYPE_SLAVE,
};

#include "DB.h"     // StringId
#include "ID.h"     // ID
#include "player.h" // PLAYER_INDEX

template <class T> class DynamicArray;
class TradeRouteData;
class Unit;
class MapPoint;

class TradeRoute : public ID
{
public:
	TradeRoute () : ID() { ; } ;
	TradeRoute (sint32 val) : ID (val) { ; };
	TradeRoute (uint32 val) : ID (val) { ; };

	TradeRouteData *operator -> () const { return AccessData(); }
	bool IsValid() const;

	void KillRoute(CAUSE_KILL_TRADE_ROUTE cause);
	void Kill(CAUSE_KILL_TRADE_ROUTE cause) { KillRoute(cause); }

	void RemoveAllReferences(CAUSE_KILL_TRADE_ROUTE cause);

	const TradeRouteData* GetData() const;
	TradeRouteData* AccessData() const;

	Unit GetSource() const;
	Unit GetDestination() const;

	PLAYER_INDEX GetOwner() const;
	PLAYER_INDEX GetPayingFor() const;

	void GetSourceResource(ROUTE_TYPE &type, sint32 &resource) const;

	BOOL PassesThrough(sint32 player) const;

	BOOL CrossesWater() const;

	uint32 GetColor() const;
	void SetColor( uint32 color );

	void AddSeenByBit(sint32 player);
	void RemoveSeenByBit(sint32 player);
	bool SeenBy(sint32 player) const;
	void RedrawRadarMapAlongRoute();
	void RevealTradeRouteStateIfInVision();

	void ClearSelectedPath();
	void GenerateSelectedPath(const MapPoint &pos);
	sint32 AddSelectedWayPoint(const MapPoint &pos);
	BOOL InitSelectedData();
	BOOL IsSelectedPathSame();
	BOOL IsPosInSelectedPath(const MapPoint &pos);
	BOOL IsPosInPath(const MapPoint &pos);
	void ClearSelectedCellData(TradeRoute route);
	void UpdateSelectedCellData(TradeRoute route);
	sint32 GetPathSelectionState() const;
	void SetPathSelectionState(sint32 state);

	sint32 GetCost() const;
	const DynamicArray<MapPoint>* GetPath() const;
	const DynamicArray<MapPoint>* GetSelectedPath() const;

	BOOL IsActive() const;
	void Activate();
	void Deactivate();
	void RemoveUnseenRoute();

	sint32 GetGoldInReturn() const;

	StringId GetResourceName() const;

	void ReturnPath(const PLAYER_INDEX owner, DynamicArray<MapPoint> &waypoints,
	                DynamicArray<MapPoint> &fullpath,
	                double &cost);
/* unused
	void SetPath(DynamicArray<MapPoint> &fullpath,DynamicArray<MapPoint> &waypoints);
	void BeginTurn();
*/

	void DontAdjustPointsWhenKilled();
};

#endif
