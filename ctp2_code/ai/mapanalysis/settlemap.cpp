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
//   question. (May 20th 2006 Martin G�hmann)
// - Corrected delete operator for array.
// - Improved settle radius determination on city growth.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "settlemap.h"

#include "boundingrect.h"
#include "Cell.h"
#include "citydata.h"
#include "CityInfluenceIterator.h"
#include "CitySizeRecord.h"
#include "Diplomat.h"
#include "gfx_options.h"
#include "mapanalysis.h"
#include "StrategyRecord.h"
#include "TerrainRecord.h"
#include "UnitRecord.h"
#include <utility>
#include <vector>
#include "World.h"		            // g_theWorld

namespace
{
    double const    VALUE_NEAR_EDGE_OF_WORLD    = -1.0;
}

SettleMap SettleMap::s_settleMap;

template<>
MapGrid<double>::MapGridArray MapGrid<double>::s_scratch = {};

SettleMap::SettleMap()
:
    m_settleValues	(),
    m_invalidCells	()
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
double SettleMap::ComputeSettleValue(const MapPoint & pos) const
{
	sint32 score = 0;
	RadiusIterator it(pos, k_minimum_settle_city_size);

	for (it.Start(); !it.End(); it.Next())
	{
		const Cell * cell = g_theWorld->GetCell(it.Pos());
		if (!cell->GetCityOwner())
		{
			score += cell->GetScore();
		}
	}

	return score;
}




void SettleMap::Cleanup()
{
    m_settleValues.Cleanup();
    m_invalidCells.Resize(0, 0, false);
}

void SettleMap::Initialize()
{
	size_t const    x_size  = g_theWorld->GetWidth();
	size_t const    y_size  = g_theWorld->GetHeight();
	MapPoint rc_pos;
	MapPoint xy_pos;

	m_settleValues.Clear();
	m_settleValues.Resize(x_size, y_size, 1);
	m_invalidCells.Resize(x_size, y_size, 0);

	for (rc_pos.x = 0; static_cast<size_t>(rc_pos.x) < x_size; rc_pos.x++)
	{
		for (rc_pos.y = 0; static_cast<size_t>(rc_pos.y) < y_size; rc_pos.y++)
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
				value = ComputeSettleValue(rc_pos);
			}

			m_settleValues.AddValue(rc_pos, value);
		}
	}
}

//void SettleMap::HandleCityStarvation(const Unit & city)

/// Update the settle target information when a city grows
/// \param city The city
void SettleMap::HandleCityGrowth(const Unit & city)
{
	Assert(city.IsValid());
	MapPoint        cityPos     = city.RetPos();
	CityData *      citydata    = city.GetCityData();
	PLAYER_INDEX    playerId    = city.GetOwner();
	sint32          radius      = g_theCitySizeDB->Get(citydata->GetSizeIndex())->GetIntRadius();
	/// @todo Check functionality. Using "2 * current radius + 1" looks arbitrary.
	/// This does not account for future growth (city spacing may become too tight), but it also
	/// prevents any overlap.
	radius += radius + 1;

	// Mark tiles near to the grown city as not worth to settle at all
	RadiusIterator it(cityPos, radius);
	for (it.Start(); !it.End(); it.Next())
	{
		MapPoint    clearPos = it.Pos();
		m_invalidCells.Set(clearPos.x, clearPos.y, TRUE);
	}

	const StrategyRecord & strategy = Diplomat::GetDiplomat(playerId).GetCurrentStrategy();
	sint32                  min_settle_distance = 0;
	strategy.GetMinSettleDistance(min_settle_distance);

	// Mark tiles further away as having only half the usual value
	/// \todo Optimise using CircleIterator, to skip computing values for the already invalidated tiles.
	RadiusIterator settleIt(cityPos, min_settle_distance);
	for (settleIt.Start(); !settleIt.End(); settleIt.Next())
	{
		MapPoint        claimPos    = settleIt.Pos();
		double const    new_value   = ComputeSettleValue(claimPos) * 0.5;
		m_settleValues.SetGridValue(claimPos, new_value);
	}

	MapAnalysis::GetMapAnalysis().UpdateBoundingRectangle(city);
}

bool SettleMap::HasSettleTargets(const PLAYER_INDEX &playerId, bool isWater) const
{
	BoundingRect rect =
		MapAnalysis::GetMapAnalysis().GetBoundingRectangle(playerId);

	if (!rect.IsValid())
		return false;

	rect.Expand( 10 );

	MapPoint xy_pos(0,0);
	MapPoint rc_pos(0,0);
	SettleTarget settle_target;

	sint16 rows = rect.GetMaxRows();
	sint16 cols = rect.GetMaxCols();

	sint32 settle_threshold = 0;
	(void) Diplomat::GetDiplomat(playerId).GetCurrentStrategy().GetMinSettleScore(settle_threshold);

	for(sint32 i = 0; rect.Get(i, xy_pos, rows, cols); i++)
	{
#if defined(USE_WORLDSIZE_CLASS)
		rc_pos.xy2rc(xy_pos, g_theWorld->GetSize());
#else
		rc_pos.xy2rc(xy_pos, *g_theWorld->GetSize());
#endif

		if(!CanSettlePos(rc_pos))
			continue;

		if(m_settleValues.GetGridValue(rc_pos) <= settle_threshold)
		{
			continue;
		}

		if(g_theWorld->IsWater(rc_pos) == isWater)
		{
			return true;
		}
	}

	return false;
}
/*
// Some experimental code that seems to make it worse for the AI
void SettleMap::GetSettleTargets(const PLAYER_INDEX &playerId,
								 SettleMap::SettleTargetList & targets) const
{
	BoundingRect rect =
		MapAnalysis::GetMapAnalysis().GetBoundingRectangle(playerId);

	if (!rect.IsValid())
		return;

	rect.Expand( 10 );

	MapPoint xy_pos(0,0);
	MapPoint rc_pos(0,0);
	SettleTarget settle_target;

	sint16 rows = rect.GetMaxRows();
	sint16 cols = rect.GetMaxCols();

	sint32 settle_threshold = 0;
	(void) Diplomat::GetDiplomat(playerId).GetCurrentStrategy().GetMinSettleScore(settle_threshold);

	Player *player_ptr = g_player[playerId];
	Assert(player_ptr);
	if(!player_ptr)
		return;

	sint32 numTerrain = g_theTerrainDB->NumRecords();
	bool* settleTerrainTypes = new bool[numTerrain];
	std::fill(settleTerrainTypes, settleTerrainTypes + numTerrain, false);

	bool noSettleUnits = true;
	Unit unit;
	sint32 i;

	Assert(player_ptr->m_all_units);
	for(i = 0; i < player_ptr->m_all_units->Num(); ++i)
	{
		unit = player_ptr->m_all_units->Access(i);
		const UnitRecord* rec = player_ptr->m_all_units->Access(i).GetDBRec();

		Assert(rec);
		if(!rec) continue;

		if(rec->GetNumCanSettleOn() > 0)
		{
			for(sint32 j = 0; j < rec->GetNumCanSettleOn(); ++j){
				settleTerrainTypes[rec->GetCanSettleOnIndex(j)] = true;
				noSettleUnits = false;
			}
		}
		else
		{
			for(sint32 j = 0; j < numTerrain; ++j)
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

	if(noSettleUnits)
	{
		delete [] settleTerrainTypes;
		return;
	}

	for(i = 0; rect.Get(i, xy_pos, rows, cols); i++)
	{
#if defined(USE_WORLDSIZE_CLASS)
		rc_pos.xy2rc(xy_pos, g_theWorld->GetSize());
#else
		rc_pos.xy2rc(xy_pos, *g_theWorld->GetSize());
#endif

		settle_target.m_value = m_settleValues.GetGridValue(rc_pos);
		settle_target.m_pos = rc_pos;

		if(g_graphicsOptions->IsCellTextOn())
		{
			char buf[16];
			sprintf(buf,"*%4.0f*",settle_target.m_value);
			g_graphicsOptions->AddTextToCell(rc_pos, buf, 255);
		}

		if(!CanSettlePos(rc_pos))
			continue;

		if(settle_target.m_value <= settle_threshold)
		{
			if (g_graphicsOptions->IsCellTextOn())
			{
				char buf[16];
				sprintf(buf,"(%4.0f)",settle_target.m_value);
				g_graphicsOptions->AddTextToCell(rc_pos, buf, 255);
			}

			continue;
		}

		if(!settleTerrainTypes[g_theWorld->GetTerrainType(rc_pos)])
			continue;

		if(g_graphicsOptions->IsCellTextOn())
		{
			char buf[16];
			sprintf(buf,"%4.0f",settle_target.m_value);
			g_graphicsOptions->AddTextToCell(rc_pos, buf, 255);
		}

		targets.push_back(settle_target);
	}

	delete [] settleTerrainTypes;

	if (targets.empty())
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

	++iter;

	while(iter != targets.end())
	{
		tmp_iter = targets.begin();
		while(tmp_iter != iter)
		{
			if
			  (
			    MapPoint::GetSquaredDistance(iter->m_pos, tmp_iter->m_pos) <
			                   (min_settle_distance * min_settle_distance)
			  )
			{
				if(iter->m_value >= tmp_iter->m_value)
				{
					tmp_iter = targets.erase(tmp_iter);
				}
				else
				{
					iter = targets.erase(iter);
					--iter;
					break;
				}
			}
			else
			{
				++tmp_iter;
			}
		}

		++iter;
	}

	iter  = targets.begin();
	++iter;

	while (iter != targets.end())
	{
		g_theWorld->GetContinent(iter->m_pos, cont, is_land);

#ifdef _DEBUG
                  if (is_land) {Assert(cont < max_land_cont);}
                  else {Assert(cont < max_water_cont);}
#endif _DEBUG
		Assert(cont >= 0);

		if ((is_land && (land_continent_count[cont] >= k_targets_per_continent)) ||
			(!is_land && (water_continent_count[cont] >= k_targets_per_continent)))
		{
			iter = targets.erase(iter);
		}
		else
		{
			if (is_land)
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

			++iter;
		}
	}

	targets.sort(std::greater<SettleTarget>()); // Should have been sorted already
}*/

void SettleMap::GetSettleTargets(const PLAYER_INDEX &playerId,
								 SettleMap::SettleTargetList & targets) const
{
	BoundingRect rect =
		MapAnalysis::GetMapAnalysis().GetBoundingRectangle(playerId);

	if (!rect.IsValid())
		return;

	rect.Expand( 10 );

	MapPoint xy_pos(0,0);
	MapPoint rc_pos(0,0);
	SettleTarget settle_target;

	sint16 rows = rect.GetMaxRows();
	sint16 cols = rect.GetMaxCols();

	sint32 settle_threshold = 0;
	(void) Diplomat::GetDiplomat(playerId).GetCurrentStrategy().GetMinSettleScore(settle_threshold);

	Player *player_ptr = g_player[playerId];
	Assert(player_ptr);
	if(!player_ptr)
		return;

	sint32 numTerrain = g_theTerrainDB->NumRecords();
	bool* settleTerrainTypes = new bool[numTerrain];
	std::fill(settleTerrainTypes, settleTerrainTypes + numTerrain, false);

	bool noSettleUnits = true;
	Unit unit;
	sint32 i;

	Assert(player_ptr->m_all_units);
	for(i = 0; i < player_ptr->m_all_units->Num(); ++i)
	{
		unit = player_ptr->m_all_units->Access(i);
		const UnitRecord* rec = player_ptr->m_all_units->Access(i).GetDBRec();

		Assert(rec);
		if(!rec) continue;

		if(rec->GetNumCanSettleOn() > 0)
		{
			for(sint32 j = 0; j < rec->GetNumCanSettleOn(); ++j){
				settleTerrainTypes[rec->GetCanSettleOnIndex(j)] = true;
				noSettleUnits = false;
			}
		}
		else
		{
			for(sint32 j = 0; j < numTerrain; ++j)
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

	if(noSettleUnits)
	{
		delete [] settleTerrainTypes;
		return;
	}

	for(i = 0; rect.Get(i, xy_pos, rows, cols); i++)
	{
#if defined(USE_WORLDSIZE_CLASS)
		rc_pos.xy2rc(xy_pos, g_theWorld->GetSize());
#else
		rc_pos.xy2rc(xy_pos, *g_theWorld->GetSize());
#endif

		settle_target.m_value = m_settleValues.GetGridValue(rc_pos);
		settle_target.m_pos = rc_pos;

		if(g_graphicsOptions->IsCellTextOn())
		{
			char buf[16];
			sprintf(buf,"*%4.0f*",settle_target.m_value);
			g_graphicsOptions->AddTextToCell(rc_pos, buf, 255);
		}

		if(!CanSettlePos(rc_pos))
			continue;

		if(settle_target.m_value <= settle_threshold)
		{
			if (g_graphicsOptions->IsCellTextOn())
			{
				char buf[16];
				sprintf(buf,"(%4.0f)",settle_target.m_value);
				g_graphicsOptions->AddTextToCell(rc_pos, buf, 255);
			}

			continue;
		}

		if(!settleTerrainTypes[g_theWorld->GetTerrainType(rc_pos)])
			continue;

		if(g_graphicsOptions->IsCellTextOn())
		{
			char buf[16];
			sprintf(buf,"%4.0f",settle_target.m_value);
			g_graphicsOptions->AddTextToCell(rc_pos, buf, 255);
		}

		targets.push_back(settle_target);
	}

	delete [] settleTerrainTypes;

	if (targets.empty())
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
	++iter;

	while (iter != targets.end())
	{
		g_theWorld->GetContinent(iter->m_pos, cont, is_land);

#ifdef _DEBUG
                  if (is_land) {Assert(cont < max_land_cont);}
                  else {Assert(cont < max_water_cont);}
#endif _DEBUG
		Assert(cont >= 0);

		if ((is_land && (land_continent_count[cont] >= k_targets_per_continent)) ||
			(!is_land && (water_continent_count[cont] >= k_targets_per_continent)))
		{
			iter = targets.erase(iter);
		}
		else
		{
			for (tmp_iter = targets.begin(); tmp_iter != iter; ++tmp_iter)
			{
				if(MapPoint::GetSquaredDistance(iter->m_pos, tmp_iter->m_pos) <
				   (min_settle_distance * min_settle_distance)
				  )
				{
					break;
				}
			}

			if (tmp_iter == iter)
			{
				++iter;
			}
			else
			{
				iter = targets.erase(iter);
				continue;
			}

			if (is_land)
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

bool SettleMap::CanSettlePos(const MapPoint & rc_pos) const
{
    return !m_invalidCells.Get(rc_pos.x, rc_pos.y);
}

void SettleMap::SetCanSettlePos(const MapPoint & rc_pos, const bool can_settle)
{
    m_invalidCells.Set(rc_pos.x, rc_pos.y, !can_settle);
}

double SettleMap::GetValue(const MapPoint &rc_pos) const
{
    return m_settleValues.GetGridValue(rc_pos);
}
