//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The SettleMap
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
// - Generate debug version when set.
//
// USE_WORLD_SIZE_CLASS
// - Decoupled WorldSize from MapPoint class.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Modified GetSettleTargets so that no settle targets are found that cannot
//   be settled, because the player does not has any units for the tile in
//   question. (May 20th 2006 Martin Gühmann)
// - Corrected delete operator for array.
// - Improved settle radius determination on city growth.
// - Added player argument to AddTextToCell, so that we can select whose
//   stuff is shown. (30-Dec-2018 Martin Gühmann)
// - The AI starts to consider settle targets when a settler is almost
//   finished. (31-Dec-2018 Martin Gühmann)
// - Removed m_invalidCells, cells not suited for settling have a value of
//   zero. (03-Jan-2018 Martin Gühmann)
// - Invalid cells are owned by a city or adjacent to those. Do not use for
//   the settle map that is used for all civs the settle distance preferences
//   for one civ. The preferences are only used when selecting a settle
//   target or for checking whether there are targets. (03-Jan-2018 Martin Gühmann)
// - If a city has a population size of zero than all cells owned by this
//   city are treated as without a city owner. This frees the cells if a city
//   is being removed. When we are updating the cells, the city is officially
//   still on the map. (03-Jan-2018 Martin Gühmann)
// - The settle map is now also updated after city shrinking. (03-Jan-2018 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "settlemap.h"

#include <functional>

#include "Cell.h"
#include "citydata.h"
#include "CityInfluenceIterator.h"
#include "CitySizeRecord.h"
#include "Diplomat.h"
#include "gfx_options.h"
#include "StrategyRecord.h"
#include "TerrainRecord.h"
#include "UnitRecord.h"
#include <utility>
#include <vector>
#include "World.h"                  // g_theWorld

namespace
{
	double const    VALUE_NEAR_EDGE_OF_WORLD    = -1.0;
}

SettleMap SettleMap::s_settleMap;

template<>
MapGrid<double>::MapGridArray MapGrid<double>::s_scratch = {};

SettleMap::SettleMap()
:
	m_settleValues()
{
}

//----------------------------------------------------------------------------
//
// Name       : SettleMap::ComputeSettleValue
//
// Description: Calculates a settling score for the given position, which is
//              independent of unit abilities to settle at a certain location.
//              The settle score is the sum of scores of tiles in a radius of
//              two tiles around the given position.
//
// Parameters : const MapPoint & pos: The position for that the settle
//                                    score should be computed.
//
// Globals    : g_theWorld: The game world
//
// Returns    : double:     The settle score for the given map position
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double SettleMap::ComputeSettleValue(const MapPoint & pos, const Unit &city) const
{
	// Cannot be settled if this is already claimed by a city
	if(g_theWorld->GetCell(pos)->GetCityOwner().IsValid()
	&& g_theWorld->GetCell(pos)->GetCityOwner() != city)
		return 0.0;

	// Get a square containing 9 cells.
	// So that we can check all the neighbors.
	SquareIterator itNext(pos, 1);

	// Invalidate if a neighbor is owned by a city
	for(itNext.Start(); !itNext.End(); itNext.Next())
	{
		const Cell * cell = g_theWorld->GetCell(itNext.Pos());
		if(cell->GetCityOwner().IsValid() && cell->GetCityOwner() != city)
		{
			return 0.0;
		}
	}

	sint32 score = 0;
	RadiusIterator it(pos, k_minimum_settle_city_size);

	for(it.Start(); !it.End(); it.Next())
	{
		const Cell * cell = g_theWorld->GetCell(it.Pos());
		if(!cell->GetCityOwner() || cell->GetCityOwner() == city)
		{
			score += cell->GetScore();
		}
	}

	return score;
}

void SettleMap::Cleanup()
{
	m_settleValues.Cleanup();
}

void SettleMap::Initialize()
{
	size_t const    x_size  = g_theWorld->GetWidth();
	size_t const    y_size  = g_theWorld->GetHeight();
	MapPoint rc_pos;
	MapPoint xy_pos;

	m_settleValues.Clear();
	m_settleValues.Resize(x_size, y_size, 1);

	for(rc_pos.x = 0; static_cast<size_t>(rc_pos.x) < x_size; rc_pos.x++)
	{
		for(rc_pos.y = 0; static_cast<size_t>(rc_pos.y) < y_size; rc_pos.y++)
		{
#if defined(USE_WORLDSIZE_CLASS)
			xy_pos.rc2xy(rc_pos, g_theWorld->GetSize());
#else
			xy_pos.rc2xy(rc_pos, *g_theWorld->GetSize());
#endif
			double value    = VALUE_NEAR_EDGE_OF_WORLD;

			if ( ( g_theWorld->IsYwrap() ||
			       ( (xy_pos.y >= k_minimum_settle_city_size) &&
			         (xy_pos.y + k_minimum_settle_city_size <= y_size)
			       )
			     )
			     &&
			     (  g_theWorld->IsXwrap() ||
			        ( (xy_pos.x >= k_minimum_settle_city_size) &&
			          (xy_pos.x + k_minimum_settle_city_size <= (x_size * 2))
			        )
			     )
			   )
			{
				value = ComputeSettleValue(rc_pos, Unit());
			}

			m_settleValues.AddValue(rc_pos, value);
		}
	}
}

//void SettleMap::HandleCityStarvation(const Unit & city)

/// Update the settle target information when a city grows
/// \param city The city
void SettleMap::HandleCityGrowth(const Unit & city, sint32 oldSizeIndex)
{
	Assert(city.IsValid());
	MapPoint        cityPos   = city.RetPos();
	CityData *      citydata  = city.GetCityData();
	sint32          sizeIndex = citydata->GetSizeIndex() >= oldSizeIndex ? citydata->GetSizeIndex() : oldSizeIndex;
	sint32          radius    = g_theCitySizeDB->Get(sizeIndex)->GetIntRadius();

	// Make the radius one bigger to recalculate the 
	// values outside of the adjacent cells
	radius++;

	Unit ignoreCity = city.PopCount() > 0 ? Unit() : city;

	// Recalculate the settle values around the city
	SquareIterator settleIt(cityPos, radius);
	for(settleIt.Start(); !settleIt.End(); settleIt.Next())
	{
		MapPoint        claimPos    = settleIt.Pos();
		double const    new_value   = ComputeSettleValue(claimPos, ignoreCity);
		m_settleValues.SetGridValue(claimPos, new_value);
	}
}

bool SettleMap::HasSettleTargets(const PLAYER_INDEX &playerId, bool isWater) const
{
	Player *player_ptr = g_player[playerId];
	Assert(player_ptr);
	if(!player_ptr)
		return false;

	SettleTarget settle_target;

	sint32 settle_threshold         = 0;
	sint32 min_settle_distance      = 0;
	const StrategyRecord & strategy = Diplomat::GetDiplomat(playerId).GetCurrentStrategy();

	(void)strategy.GetMinSettleScore   (settle_threshold);
	(void)strategy.GetMinSettleDistance(min_settle_distance);

	MapPoint rc_pos;
	for(rc_pos.x = 0; rc_pos.x < g_theWorld->GetWidth(); rc_pos.x++)
	{
		for(rc_pos.y = 0; rc_pos.y < g_theWorld->GetHeight(); rc_pos.y++)
		{
			if(!CanSettlePos(rc_pos))
				continue;

			if(m_settleValues.GetGridValue(rc_pos) <= settle_threshold)
			{
				continue;
			}

			if( g_theWorld->IsWater(rc_pos) == isWater
			&& !HasCityInSettleDistance(rc_pos, min_settle_distance)
			  )
			{
				return true;
			}
		}
	}

	return false;
}

void SettleMap::GetSettleTargets(const PLAYER_INDEX &playerId,
                                 SettleMap::SettleTargetList & targets) const
{
	Player *player_ptr = g_player[playerId];
	Assert(player_ptr);
	if(!player_ptr)
		return;

	SettleTarget settle_target;

	sint32 settle_threshold = 0;
	(void) Diplomat::GetDiplomat(playerId).GetCurrentStrategy().GetMinSettleScore(settle_threshold);

	sint32 numTerrain = g_theTerrainDB->NumRecords();
	bool* settleTerrainTypes = new bool[numTerrain];
	std::fill(settleTerrainTypes, settleTerrainTypes + numTerrain, false);

	bool noSettleUnits = true;

	Assert(player_ptr->m_all_units);
	for(sint32 i = 0; i < player_ptr->m_all_units->Num(); ++i)
	{
		const UnitRecord* rec = player_ptr->m_all_units->Access(i).GetDBRec();

		CanUnitRecordSettle(rec, settleTerrainTypes, noSettleUnits);
	}

	if(noSettleUnits && !g_graphicsOptions->IsCellTextOn())
	{
		delete [] settleTerrainTypes;
		return;
	}

	MapPoint rc_pos;
	for(rc_pos.x = 0; rc_pos.x < g_theWorld->GetWidth(); rc_pos.x++)
	{
		for(rc_pos.y = 0; rc_pos.y < g_theWorld->GetHeight(); rc_pos.y++)
		{
			settle_target.m_value = m_settleValues.GetGridValue(rc_pos);
			settle_target.m_pos = rc_pos;

			if(g_graphicsOptions->IsCellTextOn())
			{
				char buf[16];
				sprintf(buf, "*%4.0f*", settle_target.m_value);
				g_graphicsOptions->AddTextToCell(rc_pos, buf, 255, playerId);
			}

			if(!CanSettlePos(rc_pos))
				continue;

			if(settle_target.m_value <= settle_threshold)
			{
				if(g_graphicsOptions->IsCellTextOn())
				{
					char buf[16];
					sprintf(buf, "(%4.0f)", settle_target.m_value);
					g_graphicsOptions->AddTextToCell(rc_pos, buf, 255, playerId);
				}

				continue;
			}

			if(g_graphicsOptions->IsCellTextOn())
			{
				char buf[16];
				sprintf(buf, "%4.0f", settle_target.m_value);
				g_graphicsOptions->AddTextToCell(rc_pos, buf, 200, playerId);
			}

			if(!settleTerrainTypes[g_theWorld->GetTerrainType(rc_pos)])
				continue;

			if(!noSettleUnits) // The condition should be superflous
			{
				targets.push_back(settle_target);
			}
		}
	}

	delete [] settleTerrainTypes;

	if(targets.empty())
	{
		return;
	}

	targets.sort(std::greater<SettleTarget>());

	sint16 max_water_cont = g_theWorld->GetMaxWaterContinent() - g_theWorld->GetMinWaterContinent();
	sint16 max_land_cont  = g_theWorld->GetMaxLandContinent () - g_theWorld->GetMinLandContinent ();

	std::vector<sint16> water_continent_count(max_water_cont, 0);
	std::vector<sint16>  land_continent_count(max_land_cont,  0);
	bool   is_land;
	sint16 cont;

	const StrategyRecord & strategy = Diplomat::GetDiplomat(playerId).GetCurrentStrategy();
	sint32 min_settle_distance = 0;
	(void) strategy.GetMinSettleDistance(min_settle_distance);

	SettleMap::SettleTargetList::iterator iter  = targets.begin();
	SettleMap::SettleTargetList::iterator tmp_iter;

	settle_target = *iter;  // non-emptyness verified before

	while(iter != targets.end())
	{
		g_theWorld->GetContinent(iter->m_pos, cont, is_land);

#ifdef _DEBUG
		if (is_land) {Assert(cont < max_land_cont);}
		else {Assert(cont < max_water_cont);}
#endif _DEBUG
		Assert(cont >= 0);

		if(
		      ( is_land && ( land_continent_count[cont] >= k_targets_per_continent))
		   || (!is_land && (water_continent_count[cont] >= k_targets_per_continent))
		  )
		{
			iter = targets.erase(iter);
		}
		else
		{
			if(HasCityInSettleDistance(iter->m_pos, min_settle_distance))
			{
				iter = targets.erase(iter);
				continue;
			}

			for(tmp_iter = targets.begin(); tmp_iter != iter; ++tmp_iter)
			{
				if(MapPoint::GetSquaredDistance(iter->m_pos, tmp_iter->m_pos) <
				   (min_settle_distance * min_settle_distance)
				  )
				{
					break;
				}
			}

			if(tmp_iter == iter)
			{
				++iter;
			}
			else
			{
				iter = targets.erase(iter);
				continue;
			}

			if(is_land)
			{
				if (land_continent_count[cont] == 0)
				{
					iter->m_value += 2;
				}
				land_continent_count[cont]++;
			}
			else
			{
				if (water_continent_count[cont] == 0)
				{
					iter->m_value += 2;
				}
				water_continent_count[cont]++;
			}
		}
	}

	targets.sort(std::greater<SettleTarget>());
}

bool SettleMap::HasCityInSettleDistance(const MapPoint & pos, sint32 min_settle_distance) const
{
	RadiusIterator it(pos, min_settle_distance);

	for(it.Start(); !it.End(); it.Next())
	{
		const Cell * cell = g_theWorld->GetCell(it.Pos());
	//	if(g_player[playerId]->IsExplored(it.Pos()) && cell->HasCity()) // Logically correct, but not so nice
		if(cell->HasCity())
		{
			return true;
		}
	}

	return false;
}

void SettleMap::CanUnitRecordSettle(const UnitRecord* rec, bool* settleTerrainTypes, bool &noSettleUnits) const
{
	Assert(rec);
	if(!rec) return;

	if(rec->GetNumCanSettleOn() > 0)
	{
		for (sint32 j = 0; j < rec->GetNumCanSettleOn(); ++j)
		{
			settleTerrainTypes[rec->GetCanSettleOnIndex(j)] = true;
			noSettleUnits = false;
		}
	}
	else
	{
		for(sint32 j = 0; j < g_theTerrainDB->NumRecords(); ++j)
		{
			const TerrainRecord* trec = g_theTerrainDB->Get(j);
			if(trec->GetMovementTypeLand()                                        && rec->GetSettleLand()
			|| trec->GetMovementTypeMountain()                                    && rec->GetSettleMountain()
			||(trec->GetMovementTypeSea() || trec->GetMovementTypeShallowWater()) && rec->GetSettleWater()
			|| trec->GetMovementTypeSpace()                                       && rec->GetSettleSpace()
			){
				settleTerrainTypes[j] = true;
				noSettleUnits = false;
			}
		}
	}
}

bool SettleMap::CanSettlePos(const MapPoint & rc_pos) const
{
	return m_settleValues.GetGridValue(rc_pos) > 0.0;
}

double SettleMap::GetValue(const MapPoint &rc_pos) const
{
	return m_settleValues.GetGridValue(rc_pos);
}
