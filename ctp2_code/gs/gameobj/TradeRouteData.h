#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _TRADEROUTEDATA_H_
#define _TRADEROUTEDATA_H_

#include "ctp2_enums.h"
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

class TradeRouteData : public GAMEOBJ,
					   public CityRadiusCallback
{
private:
    
	
	double m_transportCost;
	PLAYER_INDEX m_owner;
	PLAYER_INDEX m_payingFor;
	Army m_piratingArmy;

	ROUTE_TYPE m_sourceRouteType;
	sint32 m_sourceResource; 
	BOOL m_passesThrough[k_MAX_PLAYERS];
	BOOL m_crossesWater;
	BOOL m_isActive;

	
	uint32	m_color;
	uint32	m_outline;

	
	sint32	m_selectedIndex;


	BOOL m_valid;

	sint32 m_gold_in_return;

	
	sint32	m_path_selection_state;	

	
	

	
	
	Unit m_sourceCity;
	Unit m_destinationCity;
	TradeRoute m_recip;

	DynamicArray<MapPoint> m_path;
	DynamicArray<MapPoint> m_wayPoints;
	DynamicArray<MapPoint> m_selectedPath;
	DynamicArray<MapPoint> m_selectedWayPoints;

	DynamicArray<MapPoint> m_setPath;
	DynamicArray<MapPoint> m_setWayPoints;

	Path *m_astarPath;
	
	

	
	BOOL m_dontAdjustPointsWhenKilled;

	void CheckSquareForCity(MapPoint pos);

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
	uint32 GetOutlineColor() const { return m_outline; }
	void SetColor( uint32 color ) { m_color = color; }
	void SetOutlineColor( uint32 color ) { m_outline = color; }

	void SetSource(Unit source);
	void SetDestination(Unit dest);

	TradeRoute GetRecip() const;
	TradeRoute AccessRecip();
	void SetRecip(TradeRoute route);

	double GetCost() const;
	void SetCost(double cost);

	const DynamicArray<MapPoint>* GetPath() const { return &m_path; }
	const DynamicArray<MapPoint>* GetSelectedPath() const { return &m_selectedPath; }

	void GetSourceResource(ROUTE_TYPE &type, sint32 &resource) const;
	void RemoveFromCells();

	BOOL PassesThrough(sint32 player) const;
	void CityRadiusFunc(const MapPoint &pos);

	BOOL CrossesWater() const;

	
	void ClearPath();
	void AddWayPoint(MapPoint pos);
	BOOL GeneratePath(const PLAYER_INDEX owner);
	void ReturnPath(const PLAYER_INDEX owner, DynamicArray<MapPoint> &waypoints,
					DynamicArray<MapPoint> &fullpath,
					double &cost);
	void SetPath(DynamicArray<MapPoint> &fullpath,
				 DynamicArray<MapPoint> &waypoints);
	void BeginTurn();

	
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

	BOOL IsActive() const { return m_isActive; }
	void Activate() { m_isActive = TRUE; }
	void Deactivate() { m_isActive = FALSE; }

	BOOL IsValid() const { return m_valid; }

	sint32 GetGoldInReturn() const { return m_gold_in_return; }

	StringId GetResourceName() const;
	void Serialize(CivArchive &archive);

	void DontAdjustPointsWhenKilled();
	BOOL GetDontAdjustPoints() const { return m_dontAdjustPointsWhenKilled; }

	sint32 GetValue() const;

	void SetPiratingArmy(Army &a);
	Army GetPiratingArmy();
	bool IsBeingPirated();
};

#else
class TradeRouteData;
enum ROUTE_TYPE;
#endif
