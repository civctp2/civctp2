//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Trade utilities
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
// CTP1_TRADE
// - Creates an executable with trade like in CTP1. Currently broken.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Standardized trade route cost calculation. - June 5th 2005 Martin G�hmann
// - Do not report (ENQUEUE) a traderoute twice upon construction.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "TradeRouteData.h"

#include <algorithm>
#include <cmath>                    // std::round
#include "TradeRoute.h"
#include "World.h"
#include "player.h"
#include "Path.h"
#include "TradeAstar.h"
#include "Cell.h"
#include "CityRadius.h"
#include "TerrainRecord.h"
#include "pixelutils.h"
#include "colorset.h"               // g_colorSet
#include "network.h"
#include "ResourceRecord.h"
#include "UnitData.h"
#include "radarmap.h"
#include "tradeutil.h"
#include "Vision.h"                 // for using m_vision

extern TradeAstar g_theTradeAstar;

TradeRouteData::TradeRouteData
(
    const TradeRoute    route,
    const Unit          source,
    const Unit          dest,
    const PLAYER_INDEX  owner,
    const ROUTE_TYPE    sourceType,
    const sint32        sourceResource,
    PLAYER_INDEX        paying_for,
    sint32              gold_in_return
)
:
    GameObj                         (route.m_id),
    CityRadiusCallback              (),
    m_transportCost                 (0),
    m_owner                         (owner),
    m_payingFor                     (paying_for),
    m_piratingArmy                  (),
    m_sourceRouteType               (sourceType),
    m_sourceResource                (sourceResource),
    m_crossesWater                  (false),
    m_isActive                      (0),
    m_color                         (g_colorSet->GetColor(COLOR_YELLOW)),
    m_seenBy                        (0),
    m_selectedIndex                 (0),
    m_valid                         (false),
    m_accumilatedTimesPirated       (0),
    m_pirate                        (PLAYER_UNASSIGNED),
    m_gold_in_return                (gold_in_return),
    m_path_selection_state          (k_TRADEROUTE_NO_PATH),
    m_sourceCity                    (source),
    m_destinationCity               (dest),
    m_piratedLastTime               (-1),
    m_path                          (),
    m_wayPoints                     (),
    m_selectedPath                  (),
    m_selectedWayPoints             (),
    m_setPath                       (),
    m_setWayPoints                  (),
    m_astarPath                     (new Path()),
    m_dontAdjustPointsWhenKilled    (false)
{
	std::fill(m_passesThrough, m_passesThrough + k_MAX_PLAYERS, FALSE);
	MapPoint sPos   (m_sourceCity.RetPos());
	MapPoint dPos   (m_destinationCity.RetPos());

	AddWayPoint(sPos);
	AddWayPoint(dPos);

	DPRINTF(k_DBG_GAMESTATE,
			("Creating route from city @ (%d,%d) to city @ (%d,%d)\n",
			 sPos.x, sPos.y, dPos.x, dPos.y));

	m_valid = GeneratePath(); // determins cost of trade-units
	m_transportCost = tradeutil_GetTradeDistance(m_sourceCity, m_destinationCity); // overwrite transport cost from GeneratePath with that from tradeutil_GetTradeDistance to conform with all other cost estimations before creation, namely in governor.cpp (AI) and trademanager (human), which use tradeutil_GetTradeDistance due to significant speed-up

	AddSeenByBit(owner); // route seen by owner in any case, currently paying_for == owner see https://github.com/civctp2/civctp2/blob/67954be9d07cfe5944dec129d0a5f21114a9682b/ctp2_code/gs/gameobj/Player.cpp#L3071
	AddSeenByBit(m_destinationCity->GetOwner()); // route seen by receiver in any case, currently paying_for != m_destinationCity->GetOwner() see https://github.com/civctp2/civctp2/blob/67954be9d07cfe5944dec129d0a5f21114a9682b/ctp2_code/gs/gameobj/Player.cpp#L3071

	DPRINTF(k_DBG_GAMESTATE, ("Created Trade Route from %s to %s, cost=%d, valid=%i\n",
	                          m_sourceCity->GetCityData()->GetName(),
	                          m_destinationCity->GetCityData()->GetName(),
	                          m_transportCost
	                         )
	       );
}

TradeRouteData::TradeRouteData(CivArchive &archive) : GameObj(0)
{
	m_astarPath = new Path;
	m_dontAdjustPointsWhenKilled = false;
	Serialize(archive);
}

TradeRouteData::TradeRouteData(TradeRouteData* copyme, uint32 new_id)
    : GameObj(new_id)
{
	*this = *copyme;
	m_astarPath = new Path;
	m_id = new_id;
}

TradeRouteData::TradeRouteData(const TradeRoute route)
	: GameObj(route.m_id)
{
	m_astarPath = new Path;
}

TradeRouteData::~TradeRouteData()
{
	delete m_astarPath;
}

void TradeRouteData::AddSeenByBit(sint32 player)
{
	if(player >= 0 && player < k_MAX_PLAYERS)
	{
		m_seenBy |= (1 << player);
	}
}

void TradeRouteData::RemoveSeenByBit(sint32 player)
{
	if(player >= 0 && player < k_MAX_PLAYERS)
	{
		m_seenBy &= ~(1 << player);
	}
}

bool TradeRouteData::SeenBy(sint32 player) const
{
	return m_seenBy & (1 << player);
}

uint32 TradeRouteData::SeenByBits() const
{
	return m_seenBy;
}

void TradeRouteData::RedrawRadarMapAlongRoute()
{
	sint32 const    num = m_path.Num();
	for (sint32 i = 0; i < num; i++)
	{
		if(g_radarMap)
			g_radarMap->RedrawTile(m_path[i]);
	}
}

void TradeRouteData::RevealTradeRouteStateIfInVision()
{
	TradeRoute route(m_id);
	sint32 const    num = m_path.Num();
	for (sint32 i = 0; i < num; i++)
	{
		for(sint32 p = 0; p < k_MAX_PLAYERS; p++)
		{
			// If point i is in vision of player p then AddSeenByBit
			if(g_player && g_player[p] && g_player[p]->m_vision && g_player[p]->m_vision->IsVisible(m_path[i]))
			{
				// Check pointers, critical when game is closed or reloaded; also reaveal trade route cities for owner
				if(IsActive())
				{
					if(!SeenBy(p))
					{
						AddSeenByBit(p);
					}

					g_player[p]->m_vision->RevealTradeRouteCities(route);
					g_player[p]->m_vision->RevealTradeRouteTiles(route);
				}
				else
				{
					if(SeenBy(p))
					{
						RemoveSeenByBit(p);
					}
				}
			}
		}
	}
}

void TradeRouteData::RemoveFromCells()
{
	TradeRoute route(m_id);
	sint32 const    num = m_path.Num();
	for (sint32 i = 0; i < num; i++)
	{
		if(g_theWorld)
			g_theWorld->GetCell(m_path[i])->DelTradeRoute(route);
		if(g_radarMap)
			g_radarMap->RedrawTile(m_path[i]);
	}
}

void TradeRouteData::GetSourceResource(ROUTE_TYPE &type, sint32 &resource) const
{
	type        = m_sourceRouteType;
	resource    = m_sourceResource;
}

void TradeRouteData::CheckSquareForCity(MapPoint const & pos)
{
	Unit city = g_theWorld->GetCell(pos)->GetCity();

	if (city.IsValid())
	{
		m_passesThrough[city.GetOwner()] = TRUE;
	}
}

void TradeRouteData::CityRadiusFunc(const MapPoint &pos)
{
	CheckSquareForCity(pos);
}

BOOL TradeRouteData::PassesThrough(sint32 p) const
{
	return m_passesThrough[p];
}

void TradeRouteData::ClearPath()
{
	m_path.Clear();
	m_wayPoints.Clear();
}

void TradeRouteData::ClearSelectedPath()
{
	m_selectedPath.Clear();
	m_selectedWayPoints.Clear();
}

void TradeRouteData::AddWayPoint(MapPoint pos)
{
	m_wayPoints.Insert(pos);
}

bool TradeRouteData::GeneratePath()
{
	float            cost    = 0.0;  // float expected by g_theTradeAstar.FindPath
	double  transportCost    = 0.0;  // double because tradeutil_GetNetTradeCosts is double based
	sint32 const    nwp     = m_wayPoints.Num();

	for (sint32 wp = 0; wp < nwp - 1; ++wp)
	{
		if (wp == 0)
		{
			if (!g_theTradeAstar.FindPath // similar to tradeutil_GetAccurateTradeDistance
			        ( m_payingFor, m_wayPoints[wp], m_wayPoints[wp + 1],
			         *m_astarPath, cost, FALSE
			        )
			   )
			{
				return false;
			}
		}
		else
		{
			Path    partialAstarPath;
			if (g_theTradeAstar.FindPath
			        (m_payingFor, m_wayPoints[wp], m_wayPoints[wp + 1],
			         partialAstarPath, cost, FALSE
			        )
			   )
			{
				m_astarPath->Concat(partialAstarPath);
			}
			else
			{
				return false;
			}
		}

		transportCost += cost;
	}

	m_transportCost = static_cast<sint32>(std::round(std::max<double>(tradeutil_GetNetTradeCosts(transportCost), 1.0))); // similar (identical?) to tradeutil_GetAccurateTradeDistance 

	m_path.Insert(m_wayPoints[0]);
	MapPoint pnt;
	for (sint32 p = 1; p < m_astarPath->Num(); p++)
	{
		WORLD_DIRECTION d;
		m_astarPath->GetCurrentDir(d);
		sint32 r = m_path[p-1].GetNeighborPosition(d, pnt);
		Assert(r);

		if (r)
		{
			m_path.Insert(pnt);
			g_theWorld->GetCell(pnt)->AddTradeRoute(m_id);
			if (g_theWorld->IsWater(pnt))
			{
				m_crossesWater = true;
			}
		}

		m_astarPath->IncDir();
	}

	ENQUEUE();
	return true;
}

/* unused
void TradeRouteData::SetPath(DynamicArray<MapPoint> &fullpath,
                             DynamicArray<MapPoint> &waypoints)
{
	m_setPath.Clear();
	for (int p = 0; p < fullpath.Num(); p++)
	{
		m_setPath.Insert(fullpath[p]);
	}
}

void TradeRouteData::BeginTurn()
{
	if (m_setPath.Num() > 0)
	{
		RemoveFromCells();
		m_path.Clear();
		m_wayPoints.Clear();
		for (int p = 0; p < m_setPath.Num(); p++)
		{
			m_path.Insert(m_setPath[p]);
		}
		for (int wp = 0; wp < m_setWayPoints.Num(); wp++)
		{
			m_wayPoints.Insert(m_setWayPoints[wp]);
		}
		m_setPath.Clear();
		m_setWayPoints.Clear();
	}
}
*/

void TradeRouteData::ReturnPath
(
    const PLAYER_INDEX          owner,
    DynamicArray<MapPoint> &    waypoints,
    DynamicArray<MapPoint> &    fullpath,
    double &                    cost
)
{
	fullpath.Clear();
	Path partialAstarPath;
	float partialcost = 0;
	cost = 0.0;

	m_astarPath->Clear();

	int     nwp = waypoints.Num();
	sint32  r;

	for (int wp = 0; wp < nwp - 1; wp++)
	{
		if (wp == 0)
		{
			r = g_theTradeAstar.FindPath(owner, waypoints[wp], waypoints[wp + 1],
			                             *m_astarPath, partialcost, FALSE);
			Assert(r);
		} else {
			r = g_theTradeAstar.FindPath(owner, waypoints[wp], waypoints[wp + 1],
								  partialAstarPath, partialcost, FALSE);
			Assert(r);
			m_astarPath->Concat(partialAstarPath);
		}
		cost += partialcost;
	}

	fullpath.Insert(waypoints[0]);
	MapPoint pnt;
	m_crossesWater = false;
	for (int p = 1; p < m_astarPath->Num(); p++) {
		WORLD_DIRECTION d;
		m_astarPath->GetCurrentDir(d);
		sint32 r = fullpath[p-1].GetNeighborPosition(d, pnt);
		Assert(r);
		if(r) {
			fullpath.Insert(pnt);
			if(g_theWorld->IsWater(pnt)) {
				m_crossesWater = true;
			}
		}

		m_astarPath->IncDir();
	}
}

BOOL TradeRouteData::CrossesWater() const
{
	return m_crossesWater;
}

void TradeRouteData::Serialize(CivArchive &archive)
{
	if(archive.IsStoring())
	{
		archive << m_id;
		archive.PutDOUBLE((double)m_transportCost); // save sint32 as double (just to preserve the alignment, needed since m_transportCost was changed from double to sint32)

		archive << m_owner;
		archive.PutSINT32(m_sourceRouteType);
		archive << m_sourceResource;
		archive.Store((uint8 *)m_passesThrough, sizeof(m_passesThrough)) ;
		archive.PutSINT32(m_crossesWater) ;
		archive.PutSINT8(m_isActive);
		archive.PutUINT32(m_color);
		archive.PutUINT32(m_seenBy);
		archive.PutSINT32(m_selectedIndex);
		m_piratingArmy.Serialize(archive);
		archive.PutSINT8(m_valid);
		archive.PutUINT16(m_accumilatedTimesPirated);

		archive.PutSINT8(m_pirate);
		archive << m_payingFor;
		archive << m_gold_in_return;

		m_sourceCity.Serialize(archive);
		m_destinationCity.Serialize(archive);
		archive << m_piratedLastTime;
		m_path.Serialize(archive);
		m_wayPoints.Serialize(archive) ;
		m_selectedPath.Serialize(archive);
		m_selectedWayPoints.Serialize(archive);
		m_astarPath->Serialize(archive);

		m_setPath.Serialize(archive); // unused
		m_setWayPoints.Serialize(archive); // unused

		uint8 hasChild;
		hasChild = m_lesser != NULL;
		archive << hasChild;
		if (m_lesser)
		{
			((TradeRouteData *)(m_lesser))->Serialize(archive) ;
		}

		hasChild = m_greater != NULL;
		archive << hasChild;
		if (m_greater)
			((TradeRouteData *)(m_greater))->Serialize(archive) ;

	}
	else
	{
		archive >> m_id;
		m_transportCost = (sint32) archive.GetDOUBLE (); // load sint32 from double (just to preserve the alignment, needed since m_transportCost was changed from double to sint32)
		archive >> m_owner;
		m_sourceRouteType = (ROUTE_TYPE)(archive.GetSINT32()) ;
		archive >> m_sourceResource;
		archive.Load((uint8 *)m_passesThrough, sizeof(m_passesThrough)) ;
		m_crossesWater = (BOOL)(archive.GetSINT32()) ;
		m_isActive = archive.GetSINT8();
		m_color = archive.GetUINT32();
		m_seenBy = archive.GetUINT32();
		m_selectedIndex = archive.GetSINT32();
		m_piratingArmy.Serialize(archive);
		m_valid = archive.GetSINT8(); // Split former BOOL m_valid, so that we can save something in the gap
		m_accumilatedTimesPirated = archive.GetUINT16();

		m_pirate = archive.GetSINT8();
		archive >> m_payingFor;
		archive >> m_gold_in_return;

		m_sourceCity.Serialize(archive);
		m_destinationCity.Serialize(archive);
		archive >> m_piratedLastTime;
		m_path.Serialize(archive);
		m_wayPoints.Serialize(archive) ;
		m_selectedPath.Serialize(archive);
		m_selectedWayPoints.Serialize(archive);
		m_astarPath->Serialize(archive);
		m_setPath.Serialize(archive); // unused
		m_setWayPoints.Serialize(archive); // unused

		uint8 hasChild;
		archive >> hasChild;
		if (hasChild)
		{
			m_lesser = new TradeRouteData(archive);
		}
		else
		{
			m_lesser = NULL;
		}

		archive >> hasChild;
		if (hasChild)
			m_greater = new TradeRouteData(archive);
		else
			m_greater = NULL;
	}
}

StringId TradeRouteData::GetResourceName() const
{
	return g_theResourceDB->Get(m_sourceResource)->GetName();
#if 0
	sint32 terrain_type;
	sint32 good;

	terrain_type = m_sourceResource % k_BASE_TERRAIN_TYPES;
	good = m_sourceResource / k_BASE_TERRAIN_TYPES;
	return g_theTerrainDB->GetGoodName(terrain_type, good);
#endif
}

BOOL TradeRouteData::InitSelectedData()
{
	if (m_selectedPath.Num()) return FALSE;

	m_selectedPath = m_path;

	m_selectedWayPoints = m_wayPoints;

	return TRUE;
}

BOOL TradeRouteData::IsSelectedPathSame()
{
	if (m_path.Num() != m_selectedPath.Num()) return FALSE;

	for ( sint32 i = 0 ; i < m_path.Num() ; i++ )
		if (m_path[i] != m_selectedPath[i]) return FALSE;

	return TRUE;
}

sint32 TradeRouteData::AddSelectedWayPoint(const MapPoint &pos)
{
	for (sint32 wp = 0; wp < m_selectedWayPoints.Num(); ++wp)
	{
		if (pos == m_selectedWayPoints[wp])
		{
			m_selectedIndex = wp;
			return m_selectedIndex;
		}
	}

	sint32 wpIndex  = 0;
	for (int i = 0; i < m_selectedPath.Num(); ++i)
	{
		if (pos == m_selectedPath[i])
		{
			m_selectedWayPoints.InsertBefore(pos, wpIndex);
			break;
		}
		else if (m_selectedPath[i] == m_selectedWayPoints[wpIndex])
		{
			++wpIndex;
		}
	}

	m_selectedIndex = wpIndex;
	return m_selectedIndex;
}

void TradeRouteData::GenerateSelectedPath(const MapPoint &pos)
{
	if ((m_selectedIndex == 0) || (m_selectedIndex == m_selectedWayPoints.Num()))
		return;

	DynamicArray<MapPoint> tempWp = m_selectedWayPoints;
	tempWp[m_selectedIndex] = pos;

	DynamicArray<MapPoint> tempPath;
	double cost;

	ReturnPath(m_owner, tempWp, tempPath, cost);

	m_selectedWayPoints = tempWp;
	m_selectedPath      = tempPath;
}

BOOL TradeRouteData::IsPosInSelectedPath(const MapPoint &pos)
{
	for ( sint32 i = 0 ; i < m_selectedPath.Num() ; i++ )
		if (pos == m_selectedPath[i]) return TRUE;

	return FALSE;
}

BOOL TradeRouteData::IsPosInPath(const MapPoint &pos)
{
	for ( sint32 i = 0 ; i < m_path.Num() ; i++ )
		if (pos == m_path[i]) return TRUE;

	return FALSE;
}

void TradeRouteData::UpdateSelectedCellData(TradeRoute &route)
{
	for ( sint32 i = 0 ; i < m_selectedPath.Num() ; i++ )
		g_theWorld->GetCell(m_selectedPath[i])->AddTradeRoute(route);
}

void TradeRouteData::ClearSelectedCellData(TradeRoute &route)
{
	for ( sint32 i = 0 ; i < m_selectedPath.Num() ; i++ )
		g_theWorld->GetCell(m_selectedPath[i])->DelTradeRoute(route);
}

void TradeRouteData::SetSource(Unit source)
{
	m_sourceCity = source;
	ENQUEUE();
}

void TradeRouteData::SetDestination(Unit dest)
{
	m_destinationCity = dest;
	ENQUEUE();
}

/* deprecated, cost set by GeneratePath() and should not be changed any more
void TradeRouteData::SetCost(sint32 cost)
{
	m_transportCost = std::max<sint32>(cost, 1);
	m_path.Insert(m_wayPoints[0]);
	ENQUEUE();
}
*/

sint32 TradeRouteData::GetCost() const
{
#ifndef CTP1_TRADE
	return m_transportCost;
#else
	return 1;
#endif
}

void TradeRouteData::DontAdjustPointsWhenKilled()
{
	m_dontAdjustPointsWhenKilled = true;
}

sint32 TradeRouteData::GetValue() const
{
	if(m_sourceRouteType != ROUTE_TYPE_RESOURCE)
		return 0;

	return tradeutil_GetTradeValue(m_owner, m_destinationCity, m_sourceResource);
}

void TradeRouteData::SetPiratingArmy(Army &a)
{
	if(m_isActive)
		m_piratingArmy = a;
	else
		m_piratingArmy = 0; // deactivated route cannot be pirated
}

Army TradeRouteData::GetPiratingArmy()
{
	return m_piratingArmy;
}

bool TradeRouteData::IsBeingPirated()
{
	if(m_isActive)
		return m_piratingArmy.IsValid();
	else
		return false; // deactivated route cannot be pirated
}
