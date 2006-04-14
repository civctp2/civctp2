//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : City data
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
// _SLOW_BUT_SAFE
// - Define 2 other symbols (PROJECTED_CHECK_START and PROJECTED_CHECK_END) 
//   when set. But the defined symbols are never used, so this doesn't do
//   anything at all. This makes preprocessing and compilation slower, but
//   should be safe.
//
// USE_LOGGING
// - Enable logging when set, even when not building a debug version. 
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - ComputeSettleValue modified to prevent AI from trying to settle on places it cant
// 
//     
//----------------------------------------------------------------------------













#include "c3.h"
#include "settlemap.h"


#include "gfx_options.h"
#include "World.h"		            // g_theWorld
#include "citydata.h"

#include "CityInfluenceIterator.h"
#include "CitySizeRecord.h"
#include "Cell.h"
#include <vector>
#include <utility> 
#include "mapanalysis.h"
#include "StrategyRecord.h"
#include "Diplomat.h"

#include "boundingrect.h"

namespace
{
    double const    VALUE_NEAR_EDGE_OF_WORLD    = -1.0;
}

SettleMap SettleMap::s_settleMap;


MapGrid<double>::MapGridArray MapGrid<double>::s_scratch;


SettleMap::SettleMap()
:
    m_settleValues	(),
    m_invalidCells	()
{
}

double SettleMap::ComputeSettleValue(const MapPoint & pos) const
{
	sint32 score = 0;
	RadiusIterator it(pos, k_minimum_settle_city_size);

	for (it.Start(); !it.End(); it.Next()) 
	{
		const Cell * cell = g_theWorld->GetCell(it.Pos());
		score += cell->GetScore();
	}

//EMOD if the cell has a score of zero, the AI won't attempt at all 4-10-2006
	if (g_theWorld->GetCell(pos)->GetScore() == 0) {
		score = 0;
	}
//EMOD to allow for AI notto settle some places
//	If Unit Get SettleType 
//		GetCell(pos) != settletype
//		score = 0;
//EMOD
//  sint32 i;
//	const UnitRecord *rec = g_theUnitDB->Get(unit_type); 
//	for(i = 0; i < rec->GetNumCanSettleOn(); i++) {
//		if(!rec->GetCanSettleOnIndex(i) == g_theWorld->GetCell(pos)->GetTerrain()) {
//			score = 0;
//		}
//	}
// end EMOD
//
//	if !(rec->GetSettleLand() && g_theWorld->IsLand(pos))
//		score = 0; 
//	else if (rec->GetSettleMountain() && g_theWorld->IsMountain(pos))
//		score = 0; 
//	else if (rec->GetSettleWater() && g_theWorld->IsWater(pos))
//		score = 0; 
//	else if (rec->GetSettleSpace() && g_theWorld->IsSpace(pos))
//		score = 0;
//end EMOD
	return score;
}




void SettleMap::Cleanup()
{
    m_settleValues.Clear();
    m_invalidCells.Resize(0, 0, false);
}


void SettleMap::Initialize()
{
	sint32 x_size = g_theWorld->GetWidth();
	sint32 y_size = g_theWorld->GetHeight();
	MapPoint rc_pos;
	MapPoint xy_pos;

	m_settleValues.Clear();
	m_settleValues.Resize(x_size, y_size, 1); 
	m_invalidCells.Resize(x_size, y_size, 0);

	
	for (rc_pos.x=0; rc_pos.x < x_size; rc_pos.x++)
	{
		for (rc_pos.y=0; rc_pos.y < y_size; rc_pos.y++)
		{
			xy_pos.rc2xy(rc_pos, *g_theWorld->GetSize());
	        
            double value    = VALUE_NEAR_EDGE_OF_WORLD;

			if ( ( g_theWorld->IsYwrap() ||
				   ( (xy_pos.y >= k_minimum_settle_city_size) &&
				     (xy_pos.y <= (y_size - k_minimum_settle_city_size))
                   )
                 ) 
                 &&
                 (  g_theWorld->IsXwrap() ||
					( (xy_pos.x >= k_minimum_settle_city_size) &&
					  (xy_pos.x <= ((x_size * 2) - k_minimum_settle_city_size)) 
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


void SettleMap::HandleCityGrowth(const Unit & city)
{
	
	MapPoint pos = city.RetPos();
	CityData *citydata = city.GetCityData();
	Assert(citydata);
	PLAYER_INDEX playerId = city.GetOwner();

	
	sint32 radius = g_theCitySizeDB->Get(citydata->GetSizeIndex())->GetIntRadius();
	radius += radius + 1;
	
	RadiusIterator it(pos, radius);
	for (it.Start(); !it.End(); it.Next()) 
	{
		pos = it.Pos();
		m_invalidCells.Set(pos.x, pos.y, TRUE);
	}

	
	const StrategyRecord & strategy = Diplomat::GetDiplomat(playerId).GetCurrentStrategy();
	sint32 min_settle_distance = 0;
	strategy.GetMinSettleDistance(min_settle_distance);

	RadiusIterator settleIt(pos, min_settle_distance);
	for (settleIt.Start(); !settleIt.End(); settleIt.Next()) 
	{
		pos = settleIt.Pos();
		sint32 const new_value = ComputeSettleValue(pos) * 0.5;
		m_settleValues.SetGridValue(pos, new_value);
	}
	
	MapAnalysis::GetMapAnalysis().UpdateBoundingRectangle(city);
}


void SettleMap::GetSettleTargets(const PLAYER_INDEX &playerId, 
								 const bool & settle_water,
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

	for (sint32 i = 0; rect.Get(i, xy_pos, rows, cols); i++)
	{
		rc_pos.xy2rc(xy_pos, *g_theWorld->GetSize());

		settle_target.m_value = m_settleValues.GetGridValue(rc_pos);
		settle_target.m_pos = rc_pos;

#ifdef _DEBUG
		if (g_graphicsOptions->IsCellTextOn()) {
			char buf[16];
			sprintf(buf,"*%4.0f*",settle_target.m_value );
			g_graphicsOptions->AddTextToCell(rc_pos, buf, 255);
		}
#endif _DEBUG

		
		if (!CanSettlePos(rc_pos))
			continue;

		if (settle_target.m_value <= settle_threshold)
		{

#ifdef _DEBUG
			if (g_graphicsOptions->IsCellTextOn()) {
				char buf[16];
				sprintf(buf,"(%4.0f)",settle_target.m_value );
				g_graphicsOptions->AddTextToCell(rc_pos, buf, 255);
			}
#endif _DEBUG

			continue;
		}

		
		if (!settle_water && g_theWorld->IsWater(rc_pos))
			continue;

#ifdef _DEBUG
		if (g_graphicsOptions->IsCellTextOn()) {
			char buf[16];
			sprintf(buf,"%4.0f",settle_target.m_value );
			g_graphicsOptions->AddTextToCell(rc_pos, buf, 255);
		}
#endif _DEBUG

		targets.push_back(settle_target);
	}

    Assert(!targets.empty());
    if (targets.empty())
    {
        return;
    }

	targets.sort(std::greater<SettleTarget>());

	
	sint16 max_water_cont = g_theWorld->GetMaxWaterContinent() - g_theWorld->GetMinWaterContinent();
	sint16 max_land_cont = g_theWorld->GetMaxLandContinent() - g_theWorld->GetMinLandContinent();

	
	std::vector<sint16> water_continent_count(max_water_cont, 0);
	std::vector<sint16> land_continent_count(max_land_cont, 0);
	BOOL is_land;
	sint32 cont;

	
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
               if (MapPoint::GetSquaredDistance(iter->m_pos, tmp_iter->m_pos) <
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
