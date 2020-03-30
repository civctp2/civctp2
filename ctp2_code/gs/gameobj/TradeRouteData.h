//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Trade Route Data
// Id           : $Id$
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
// - Added SetSourceResource method to correct the trade route index if
//   the size of the Resource database was increased. - June 4th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _TRADEROUTEDATA_H_
#define _TRADEROUTEDATA_H_

class TradeRouteData;

#include "GameObj.h"
#include "Unit.h"
#include "TradeRoute.h"
#include "dynarr.h"
#include "MapPoint.h"
#include "CityRadius.h"
#include "Army.h"

#define k_TRADEROUTE_NO_PATH		0
#define k_TRADEROUTE_ORIGINAL_PATH	1
#define k_TRADEROUTE_SELECTED_PATH	2

class Path;

class TradeRouteData : public GameObj,
                       public CityRadiusCallback
{
private:

	sint32                 m_transportCost;
	PLAYER_INDEX           m_owner;
	PLAYER_INDEX           m_payingFor;
	Army                   m_piratingArmy;

	ROUTE_TYPE             m_sourceRouteType;
	sint32                 m_sourceResource;
	BOOL                   m_passesThrough[k_MAX_PLAYERS];
	BOOL                   m_crossesWater;
	sint8                  m_isActive;

	uint32                 m_color;
	uint32                 m_seenBy;

	sint32                 m_selectedIndex;

	bool                   m_valid;
	sint16                 m_accumilatedTimesPirated;
	sint8                  m_pirate;

	sint32                 m_gold_in_return;

	sint32                 m_path_selection_state;

	Unit                   m_sourceCity;
	Unit                   m_destinationCity;
	sint32                 m_piratedLastTime;

	DynamicArray<MapPoint> m_path;
	DynamicArray<MapPoint> m_wayPoints;
	DynamicArray<MapPoint> m_selectedPath;
	DynamicArray<MapPoint> m_selectedWayPoints;

	DynamicArray<MapPoint> m_setPath;      // unused
	DynamicArray<MapPoint> m_setWayPoints; // unused

	Path *m_astarPath;

	bool m_dontAdjustPointsWhenKilled;

	void CheckSquareForCity(MapPoint const & pos);

	friend class NetTradeRoute;

public:
	TradeRouteData(const TradeRoute route,
	               const Unit source,
	               const Unit dest,
	               const PLAYER_INDEX owner,
	               const ROUTE_TYPE sourceType,
	               const sint32 sourceResource,
	               PLAYER_INDEX paying_for,
	               sint32 gold_in_return);
	TradeRouteData(const TradeRoute route);
	TradeRouteData(CivArchive &archive);
	TradeRouteData(TradeRouteData* copyme, uint32 new_id);
	~TradeRouteData();

	Unit GetSource() const { return m_sourceCity; }
	Unit GetDestination() const { return m_destinationCity; }
	PLAYER_INDEX GetOwner() const { return m_owner; }
	PLAYER_INDEX GetPayingFor() const { return m_payingFor; }

	uint32 GetColor() const { return m_color; }
	void SetColor( uint32 color ) { m_color = color; }

	void AddSeenByBit(sint32 player);
	void RemoveSeenByBit(sint32 player);
	bool SeenBy(sint32 player) const;
	uint32 SeenByBits() const;
	void RedrawRadarMapAlongRoute();
	void RevealTradeRouteStateIfInVision();

	void SetSource(Unit source);
	void SetDestination(Unit dest);

	void      SetLastTimePirated(sint32 currentTurn)       {        m_piratedLastTime = currentTurn; }
	sint32    GetLastTimePirated()                   const { return m_piratedLastTime; }
	void IncreaseAccumulatedTimePirated()                  {        m_accumilatedTimesPirated++; }
	void    ResetAccumulatedTimePirated()                  {        m_accumilatedTimesPirated = 0; }
	uint16    GetAccumulatedTimePirated()            const { return m_accumilatedTimesPirated; }
	void      SetPirate(sint8 pirate)                      {        m_pirate = pirate; }
	sint8     GetPirate()                            const { return m_pirate; }

	sint32 GetCost() const;
	// void SetCost(sint32 cost); // deprecated, cost set by GeneratePath() and should not be changed any more

	const DynamicArray<MapPoint>* GetPath() const { return &m_path; }
	const DynamicArray<MapPoint>* GetSelectedPath() const { return &m_selectedPath; }

	void GetSourceResource(ROUTE_TYPE &type, sint32 &resource) const;
	void SetSourceResource(sint32 resource){ m_sourceResource = resource; }
	void RemoveFromCells();

	BOOL PassesThrough(sint32 player) const;
	void CityRadiusFunc(const MapPoint &pos);

	BOOL CrossesWater() const;

	void ClearPath();
	void AddWayPoint(MapPoint pos);
	void ReturnPath(const PLAYER_INDEX owner, DynamicArray<MapPoint> &waypoints,
	                DynamicArray<MapPoint> &fullpath,
	                double &cost);
/* unused
	void SetPath(DynamicArray<MapPoint> &fullpath, DynamicArray<MapPoint> &waypoints);
	void BeginTurn();
*/

	void ClearSelectedPath();
	void GenerateSelectedPath(const MapPoint &pos);
	sint32 AddSelectedWayPoint(const MapPoint &pos);
	BOOL InitSelectedData();
	BOOL IsSelectedPathSame();
	BOOL IsPosInSelectedPath(const MapPoint &pos);
	BOOL IsPosInPath(const MapPoint &pos);
	void UpdateSelectedCellData(TradeRoute &route);
	void ClearSelectedCellData(TradeRoute &route);
	sint32 GetPathSelectionState() const { return m_path_selection_state; }
	void SetPathSelectionState(sint32 state) { m_path_selection_state = state; }

	bool IsActive() const { return m_isActive == 1; }
	void Activate() { m_isActive = 1; }
	void Deactivate() { m_isActive = 0; m_piratingArmy = 0; } // deactivated route cannot be pirated
	void Remove(sint8 cause) { m_isActive = -cause; } // store cause as negative value
	CAUSE_KILL_TRADE_ROUTE IsRemoved() const { return (m_isActive < 0 ? static_cast<CAUSE_KILL_TRADE_ROUTE>(-m_isActive) : CAUSE_KILL_TRADE_ROUTE_UNKNOWN); } // report cause as positive value

	bool IsValid() const { return m_valid; }

	sint32 GetGoldInReturn() const { return m_gold_in_return; }

	StringId GetResourceName() const;
	void Serialize(CivArchive &archive);

	void DontAdjustPointsWhenKilled();
	bool GetDontAdjustPoints() const { return m_dontAdjustPointsWhenKilled; }

	sint32 GetValue() const;

	void SetPiratingArmy(Army &a);
	Army GetPiratingArmy();
	bool IsBeingPirated();

private:
	bool GeneratePath();
};

#endif
