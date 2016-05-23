//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : World distance
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
// _DEBUG
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "PQueue.h"
#include "Cell.h"
#include "player.h"
#include "UnitDynArr.h"
#include "WorldDistance.h"
#include "UnitData.h"
#include "citydata.h"
#include "Happy.h"
#include "directions.h"

extern Player **g_player;

void World::FindCityDistances(sint32 player, const MapPoint &start)
{
	Assert( k_MAX_PLAYERS <= 32 );
	Assert( (player >= 0) && (player < k_MAX_PLAYERS) );
	uint32 playerDirtyBit = 1<<player;

#ifndef _DEBUG
	if (!(m_capitolDistanceDirtyFlags & playerDirtyBit))
		return;
#endif

	if(!m_distanceQueue) {
		m_distanceQueue = new PQueue<DistItem>(32);
	}

	ClearScratch();

	m_distanceQueue->Clear();

	m_map[start.x][start.y]->m_search_count = 1;

	sint32 numCities = g_player[player]->m_all_cities->Num();
	CDMove(0, start.x, start.y, player, numCities);

	while(numCities > 0) {
		DistItem item;
		if(m_distanceQueue->RemoveTop(item)) {
			CDMove(item.m_value - sint32(m_map[item.x][item.y]->m_move_cost),
				   item.x, item.y,
				   player, numCities);
		} else {
			Assert(FALSE);
			break;
		}
	}

	ClearScratch();
	m_capitolDistanceDirtyFlags &= ~playerDirtyBit;
}

void World::CDMove(sint32 costSoFar, const sint32 x, const sint32 y,
				   sint32 player, sint32 &numCitiesToVisit)
{
	Cell *cell = m_map[x][y];
	cell->m_search_count = costSoFar + sint32(cell->m_move_cost);

	if(cell->GetCity().m_id != 0 && cell->GetCity().GetOwner() == player)
	{
		numCitiesToVisit--;
		Happy * happy =	cell->GetCity().GetData()->GetCityData()->GetHappy();







		happy->SetCostToCapitol(cell->m_search_count);

		if(numCitiesToVisit <= 0)
			return;
	}

	sint32 d;
	MapPoint next, start(x, y);
	DistItem item;
	for(d = 0; d < sint32(NOWHERE); d++) {
		if(start.GetNeighborPosition(WORLD_DIRECTION(d), next)) {
			Cell *nextCell = m_map[next.x][next.y];
			if(nextCell->m_search_count == 0) {
				item.m_value = cell->m_search_count + sint32(nextCell->m_move_cost);
				item.x = next.x;
				item.y = next.y;
				m_distanceQueue->Insert(item);
				nextCell->m_search_count = 0x80000000;
			}
		}

		if(numCitiesToVisit <= 0) {
			return;
		}
	}
}

void World::FindDistances(sint32 player, const MapPoint &start, sint32 numHits,
						  FindDistanceCellCallback *cb, void *cookie)
{
	if(!m_distanceQueue) {
		m_distanceQueue = new PQueue<DistItem>(32);
	}

	ClearScratch();

	m_distanceQueue->Clear();

	m_map[start.x][start.y]->m_search_count = 1;

	FDMove(0, start.x, start.y, player, numHits, cb, cookie);

	while(numHits > 0) {
		DistItem item;
		if(m_distanceQueue->RemoveTop(item)) {
			FDMove(item.m_value - sint32(m_map[item.x][item.y]->m_move_cost),
				   item.x, item.y,
				   player, numHits, cb, cookie);
		} else {

			Assert(FALSE);
			break;
		}
	}

	ClearScratch();
}

void World::FDMove(sint32 costSoFar, const sint32 x, const sint32 y,
				   sint32 player, sint32 &numHitsNeeded, FindDistanceCellCallback *cb,
				   void *cookie)
{
	Cell *cell = m_map[x][y];
	cell->m_search_count = costSoFar + sint32(cell->m_move_cost);

	if(cb(MapPoint(x,y), cell, cookie)) {

		numHitsNeeded--;


		if(numHitsNeeded <= 0)
			return;
	}

	sint32 d;
	MapPoint next, start(x, y);
	DistItem item;
	for(d = 0; d < sint32(NOWHERE); d++) {
		if(start.GetNeighborPosition(WORLD_DIRECTION(d), next)) {
			Cell *nextCell = m_map[next.x][next.y];
			if(nextCell->m_search_count == 0) {
				item.m_value = cell->m_search_count + sint32(nextCell->m_move_cost);
				item.x = next.x;
				item.y = next.y;
				m_distanceQueue->Insert(item);
				nextCell->m_search_count = 0x80000000;
			}
		}

		if(numHitsNeeded <= 0) {
			return;
		}
	}
}
