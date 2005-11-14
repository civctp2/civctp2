//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Trade utilities
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
// - Standardized trade route cost calculation. - June 5th 2005 Martin Gühmann
// - Do not report (ENQUEUE) a traderoute twice upon construction.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "TradeRouteData.h"

#include <algorithm>
#include "TradeRoute.h"
#include "XY_Coordinates.h"
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
#include "ConstDB.h"
#include "UnitData.h"
#include "radarmap.h"
#include "tradeutil.h"



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
	m_transportCost                 (0.0),
	m_owner                         (owner),
	m_payingFor                     (paying_for),
	m_piratingArmy                  (),
	m_sourceRouteType               (sourceType),
	m_sourceResource                (sourceResource),
	m_crossesWater                  (false),
	m_isActive                      (false),
	m_color                         (g_colorSet->GetColor(COLOR_YELLOW)),
	m_outline                       (g_colorSet->GetColor(COLOR_BLACK)),
	m_selectedIndex                 (0),
	m_valid                         (false),
	m_gold_in_return                (gold_in_return),
	m_path_selection_state          (k_TRADEROUTE_NO_PATH),	
	m_sourceCity                    (source),
	m_destinationCity               (dest),
	m_recip                         (),
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


	m_valid = GeneratePath();

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
	m_dontAdjustPointsWhenKilled = FALSE;
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
	: GAMEOBJ(route.m_id)
{
	m_astarPath = new Path;
}

TradeRouteData::~TradeRouteData()
{
	delete m_astarPath;
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
			g_radarMap->RedrawTile(&m_path[i]);
	}
}

void TradeRouteData::GetSourceResource(ROUTE_TYPE &type, sint32 &resource) const
{
	type        = m_sourceRouteType;
	resource    = m_sourceResource;
}

TradeRoute TradeRouteData::GetRecip() const
{
	return m_recip;
}

TradeRoute TradeRouteData::AccessRecip()
{
	return m_recip;
}

void TradeRouteData::SetRecip(TradeRoute route)
{
	
	Assert(FALSE);

	m_recip = route;
	ENQUEUE();
}

void TradeRouteData::CheckSquareForCity(MapPoint pos)
{
	if(pos.x < 0) pos.x += (sint16)g_theWorld->GetWidth();
	if(pos.x >= g_theWorld->GetWidth()) pos.x -= (sint16)g_theWorld->GetWidth();
	if(pos.y < 0) pos.y += (sint16)g_theWorld->GetHeight();
	if(pos.y >= g_theWorld->GetHeight()) pos.y -= (sint16)g_theWorld->GetHeight();

	Unit city = g_theWorld->GetCell(pos)->GetCity();
	if(city.m_id != (0)) {
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
	float           cost    = 0.0;
	sint32 const    nwp     = m_wayPoints.Num();

	for (sint32 wp = 0; wp < nwp - 1; ++wp) 
    {
		if (wp == 0) 
        {
			if (!g_theTradeAstar.FindPath
                    (m_payingFor, m_wayPoints[wp], m_wayPoints[wp + 1],
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

		m_transportCost += cost;
	}

	m_transportCost = (float)((int)tradeutil_GetNetTradeCosts(m_transportCost));
	
	if(m_transportCost < 1.0)
		m_transportCost = 1.0;

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
			g_radarMap->RedrawTile(&pnt);
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
	
void TradeRouteData::SetPath(DynamicArray<MapPoint> &fullpath,
                             DynamicArray<MapPoint> &waypoints)
{
	sint32 p;
	m_setPath.Clear();
	for(p = 0; p < fullpath.Num(); p++) {
		m_setPath.Insert(fullpath[p]);
	}
}

void TradeRouteData::BeginTurn()
{
	sint32 p;
	if(m_setPath.Num() > 0) {
		RemoveFromCells();
		m_path.Clear();
		m_wayPoints.Clear();
		for(p = 0; p < m_setPath.Num(); p++) {
			m_path.Insert(m_setPath[p]);
		}
		for(p = 0; p < m_setWayPoints.Num(); p++) {
			m_wayPoints.Insert(m_setWayPoints[p]);
		}
		m_setPath.Clear();
		m_setWayPoints.Clear();
	}
}

void TradeRouteData::ReturnPath(const PLAYER_INDEX owner, 
    DynamicArray<MapPoint> &waypoints, DynamicArray<MapPoint> &fullpath,
	double &cost)
{
	fullpath.Clear();
	Path partialAstarPath;
	float partialcost = 0;
	cost = 0;

	m_astarPath->Clear();

	sint32 wp, nwp = waypoints.Num();
	sint32 r;

	for(wp = 0; wp < nwp - 1; wp++) {
		if(wp == 0) {
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

	sint32 p;
	fullpath.Insert(waypoints[0]);
	MapPoint pnt;
	m_crossesWater = FALSE;
	for(p = 1; p < m_astarPath->Num(); p++) {
		WORLD_DIRECTION d;
		m_astarPath->GetCurrentDir(d);
		sint32 r = fullpath[p-1].GetNeighborPosition(d, pnt);
		Assert(r);
		if(r) {
			fullpath.Insert(pnt);
			if(g_theWorld->IsWater(pnt)) {
				m_crossesWater = TRUE;
			}
		}

		m_astarPath->IncDir();
	}
}

BOOL
TradeRouteData::CrossesWater() const
{
	return m_crossesWater;
}

void
TradeRouteData::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive << m_id;
		archive << m_transportCost;
		archive << m_owner;
		archive.PutSINT32(m_sourceRouteType);
		archive << m_sourceResource;
		archive.Store((uint8 *)m_passesThrough, sizeof(m_passesThrough)) ;
		archive.PutSINT32(m_crossesWater) ;
		archive.PutSINT8(m_isActive);
		archive.PutUINT32(m_color);
		archive.PutUINT32(m_outline);
		archive.PutSINT32(m_selectedIndex);
		archive.PutSINT32(m_path_selection_state);
		archive.PutSINT32(m_valid);
		archive << m_payingFor;
		archive << m_gold_in_return;

		m_sourceCity.Serialize(archive);
		m_destinationCity.Serialize(archive);
		m_recip.Serialize(archive);
		m_path.Serialize(archive);
		m_wayPoints.Serialize(archive) ;
		m_selectedPath.Serialize(archive);
		m_selectedWayPoints.Serialize(archive);
		m_astarPath->Serialize(archive);

		m_setPath.Serialize(archive);
		m_setWayPoints.Serialize(archive);

		uint8 hasChild;
		hasChild = m_lesser != NULL;
		archive << hasChild;
		if (m_lesser) {
			((TradeRouteData *)(m_lesser))->Serialize(archive) ;
		}

		hasChild = m_greater != NULL;
		archive << hasChild;
		if (m_greater)
			((TradeRouteData *)(m_greater))->Serialize(archive) ;

	} else {
		archive >> m_id;
		archive >> m_transportCost;
		archive >> m_owner;
		m_sourceRouteType = (ROUTE_TYPE)(archive.GetSINT32()) ;
		archive >> m_sourceResource;
		archive.Load((uint8 *)m_passesThrough, sizeof(m_passesThrough)) ;
		m_crossesWater = (BOOL)(archive.GetSINT32()) ;
		m_isActive = (BOOL)(archive.GetSINT8());
		m_color = archive.GetUINT32();
		m_outline = archive.GetUINT32();
		m_selectedIndex = archive.GetSINT32();
		m_path_selection_state = archive.GetSINT32();
		m_valid = archive.GetSINT32();
		archive >> m_payingFor;
		archive >> m_gold_in_return;

		m_sourceCity.Serialize(archive);
		m_destinationCity.Serialize(archive);
		m_recip.Serialize(archive);
		m_path.Serialize(archive);
		m_wayPoints.Serialize(archive) ;
		m_selectedPath.Serialize(archive);
		m_selectedWayPoints.Serialize(archive);
		m_astarPath->Serialize(archive);
		m_setPath.Serialize(archive);
		m_setWayPoints.Serialize(archive);

		uint8 hasChild;
		archive >> hasChild;
		if (hasChild) {
			m_lesser = new TradeRouteData(archive);
		} else {
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
	sint32 i = 0;
	sint32 wpIndex = 0;
	BOOL posIsWayPoint = FALSE;

	
	for ( i = 0 ; i < m_selectedWayPoints.Num() ; i++ )
		if (pos == m_selectedWayPoints[i]) 
		{
			posIsWayPoint = TRUE;
			wpIndex = i;
		}

	
	if (!posIsWayPoint)
	{
		
		for ( i = 0 ; i < m_selectedPath.Num() ; i++ )
		{
			
			if (pos == m_selectedPath[i])
			{
				
				m_selectedWayPoints.InsertBefore(pos,wpIndex);
				break;
			}

			
			if (m_selectedPath[i] == m_selectedWayPoints[wpIndex]) wpIndex++;
		}
	}

	
	return m_selectedIndex = wpIndex;
}

void TradeRouteData::GenerateSelectedPath(const MapPoint &pos)
{
	if ((m_selectedIndex == 0) || (m_selectedIndex == m_selectedWayPoints.Num())) return;

	
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

void TradeRouteData::SetCost(double cost)
{ 
	m_transportCost = cost; 
	ENQUEUE(); 
}

double TradeRouteData::GetCost() const
{
#ifndef CTP1_TRADE
	return m_transportCost;
#else
	return 1.0;
#endif
}

void TradeRouteData::DontAdjustPointsWhenKilled()
{
	m_dontAdjustPointsWhenKilled = TRUE;
}

sint32 TradeRouteData::GetValue() const
{
	if(m_sourceRouteType != ROUTE_TYPE_RESOURCE)
		return 0;

	
	double baseValue = g_theWorld->GetGoodValue(m_sourceResource);
	double distance = double(m_destinationCity->GetCityData()->GetDistanceToGood(m_sourceResource));
	return sint32(baseValue * distance);
}

void TradeRouteData::SetPiratingArmy(Army &a)
{
	m_piratingArmy = a;
}

Army TradeRouteData::GetPiratingArmy()
{
	return m_piratingArmy;
}

bool TradeRouteData::IsBeingPirated()
{
	return m_piratingArmy.IsValid();
}

