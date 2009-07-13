//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : City influence handling.
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
// - Corrected wrap detection.
// - Improved iterator class structure.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "CityInfluenceIterator.h"

#include "MapPoint.h"

#include "CityRadius.h"
#include "World.h"
#include "Cell.h"
#include "CitySizeRecord.h"

#include "directions.h"
#include "UnitData.h"

#include "network.h"

#include "tiledmap.h"

namespace
{

//----------------------------------------------------------------------------
//
// Name       : ClearScratch
//
// Description: Apply SetScratch(0) to a block of tiles around a location.
//
// Parameters : a_Center    : center of the block
//              a_Radius    : radius of the block
//
// Globals    : g_theWorld
//
// Returns    : -
//
// Remark(s)  : The block is a square, tilted diagonally.
//
//----------------------------------------------------------------------------
void ClearScratch(MapPoint const & a_Center, size_t const a_Radius)
{
	SquareIterator  it(a_Center, a_Radius);

	for (it.Start(); !it.End(); it.Next())
	{
		g_theWorld->GetCell(it.Pos())->SetScratch(0);
	}
};

bool ExpandBorders(const MapPoint &center, MapPoint curPos, sint32 player, sint32 squaredRadius)
{
	Cell *cell = g_theWorld->GetCell(curPos);
	Assert(cell);

	if (cell->GetScratch() != 0) return false;

	cell->SetScratch(1);

	if(cell->GetOwner() >= 0 && cell->GetOwner() != player) return false;
	if(UnitData::GetDistance(center, curPos, 0) > squaredRadius) return false;

	cell->SetOwner(player);

	if(player >= 0)
		g_network.Block(player);
	g_network.Enqueue(cell, curPos.x, curPos.y);
	if(player >= 0)
		g_network.Unblock(player);

	MapPoint nextPos;

	bool redrawMe = false;

	if(curPos.GetNeighborPosition(NORTHEAST, nextPos))
		if(!ExpandBorders(center, nextPos, player, squaredRadius)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(SOUTHEAST, nextPos))
		if(!ExpandBorders(center, nextPos, player, squaredRadius)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(SOUTHWEST, nextPos))
		if(!ExpandBorders(center, nextPos, player, squaredRadius)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(NORTHWEST, nextPos))
		if(!ExpandBorders(center, nextPos, player, squaredRadius)) {
			redrawMe = true;
		}

	if(redrawMe)
	{
		g_tiledMap->RedrawTile(&curPos);
	}

	return true;
}

bool ExpandInfluence(Unit &city, const MapPoint &centerPos, MapPoint curPos,
					 const CitySizeRecord *rec)
{
	Cell *cell = g_theWorld->GetCell(curPos);

	if(cell->GetScratch() != 0) return false;
	if(cell->GetCityOwner().m_id != 0 && cell->GetCityOwner().m_id != city.m_id) return false;
	if(UnitData::GetDistance(centerPos, curPos, 0) > rec->GetSquaredRadius()) return false;
	
	cell->SetCityOwner(city);
	sint32 newOwner = -1;
	if (city.m_id == 0x0)
		cell->SetOwner(-1);
	else {
		cell->SetOwner(city.GetOwner());
		newOwner = city.GetOwner();
	}
	cell->SetScratch(1);

	if(newOwner >= 0)
	{
		g_network.Block(newOwner);
		g_network.Enqueue(cell, curPos.x, curPos.y);
		g_network.Unblock(newOwner);
	}

	MapPoint nextPos;

	bool redrawMe = false;

	if(curPos.GetNeighborPosition(NORTHEAST, nextPos))
		if(!ExpandInfluence(city, centerPos, nextPos, rec)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(SOUTHEAST, nextPos))
		if(!ExpandInfluence(city, centerPos, nextPos, rec)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(SOUTHWEST, nextPos))
		if(!ExpandInfluence(city, centerPos, nextPos, rec)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(NORTHWEST, nextPos))
		if(!ExpandInfluence(city, centerPos, nextPos, rec)) {
			redrawMe = true;
		}

	if(redrawMe && g_tiledMap)
	{
		g_tiledMap->RedrawTile(&curPos);
	}

	return true;
}

//----------------------------------------------------------------------------
//
// Name       : RadiusFromIndex
//
// Description: Determine the city radius
//
// Parameters : sizeIndex   : city size index
//
// Globals    : g_theCitySizeDB
//
// Returns    : radius belonging to the give sizeIndex
//
// Remark(s)  : - Assumption: the city size database exists.
//              - When the sizeIndex is larger than the number of entries in 
//                the city size database, the maximum available entry is used.
//              - When no valid entry is found, 0 is returned.
//              
//
//----------------------------------------------------------------------------
sint32 RadiusFromIndex(sint32 sizeIndex)
{
	Assert(g_theCitySizeDB && (g_theCitySizeDB->NumRecords() > 0));

	if (sizeIndex >= g_theCitySizeDB->NumRecords())
	{
		sizeIndex = g_theCitySizeDB->NumRecords() - 1;
	}

	CitySizeRecord const *  citySizeData    = g_theCitySizeDB->Get(sizeIndex);

	return citySizeData ? citySizeData->GetIntRadius() : 0;
}

}; // namespace

//----------------------------------------------------------------------------
//
// Name       : CityInfluenceIterator::CityInfluenceIterator
//
// Description: Constructor
//
// Parameters : center          : location of city
//              size            : size of city (as index in city size database)
//
// Globals    : g_theWorld
//              g_theCitySizeDB
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
CityInfluenceIterator::CityInfluenceIterator(MapPoint const & center, sint32 size)
:	RadiusIterator  (center, RadiusFromIndex(size)),
    m_cityId        (g_theWorld->GetCell(center)->GetCityOwner().m_id)
{
}

//----------------------------------------------------------------------------
//
// Name       : CityInfluenceIterator::IsIncluded
//
// Description: Determine whether the current prospect is a valid location for
//              the iterator.
//
// Parameters : - 
//              size           : size of city (as index in city size database)
//
// Globals    : g_theWorld
//              g_theCitySizeDB
//
// Returns    : bool            : valid location
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool CityInfluenceIterator::IsIncluded()
{
	return (m_testXY.IsValid() &&
	        (g_theWorld->GetCell(m_testXY.GetRC())->GetCityOwner().m_id == m_cityId)
	       );
}

//----------------------------------------------------------------------------
//
// Name       : GenerateCityInfluence
//
// Description: -
//
// Parameters : cpos                : location of city
//              size                : size of city
//
// Globals    : g_theWorld
//              g_tiledMap
//              g_theCitySizeDB
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void GenerateCityInfluence(const MapPoint &cpos, sint32 size)
{
	Assert(g_theWorld->GetCell(cpos)->HasCity());
	Unit					city		= g_theWorld->GetCell(cpos)->GetCity();
	sint32 const			intRadius	= RadiusFromIndex(size);
	ClearScratch(cpos, size);
	g_theWorld->GetCell(cpos)->SetScratch(1);

	MapPoint				cur;
	CitySizeRecord const *  rec         = g_theCitySizeDB->Get(size);
	if(cpos.GetNeighborPosition(NORTHEAST, cur)) ExpandInfluence(city, cpos, cur, rec);
	if(cpos.GetNeighborPosition(SOUTHEAST, cur)) ExpandInfluence(city, cpos, cur, rec);
	if(cpos.GetNeighborPosition(SOUTHWEST, cur)) ExpandInfluence(city, cpos, cur, rec);
	if(cpos.GetNeighborPosition(NORTHWEST, cur)) ExpandInfluence(city, cpos, cur, rec);

	if (g_tiledMap)
	{
		g_tiledMap->RedrawTile(&cpos);
	}
}

void GenerateBorders(const MapPoint &cpos, sint32 player, sint32 intRadius, sint32 squaredRadius)
{
	ClearScratch(cpos, intRadius);

	MapPoint cur;

	Cell *cell = g_theWorld->GetCell(cpos);
	cell->SetOwner(player);
	cell->SetScratch(1);

	g_network.Block(player);
	g_network.Enqueue(cell, cpos.x, cpos.y);
	g_network.Unblock(player);

	if(cpos.GetNeighborPosition(NORTHEAST, cur)) ExpandBorders(cpos, cur, player, squaredRadius);
	if(cpos.GetNeighborPosition(SOUTHEAST, cur)) ExpandBorders(cpos, cur, player, squaredRadius);
	if(cpos.GetNeighborPosition(SOUTHWEST, cur)) ExpandBorders(cpos, cur, player, squaredRadius);
	if(cpos.GetNeighborPosition(NORTHWEST, cur)) ExpandBorders(cpos, cur, player, squaredRadius);

	if (g_tiledMap)
	{
		g_tiledMap->RedrawTile(&cpos);
	}
}
