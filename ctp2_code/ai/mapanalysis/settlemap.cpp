#include "c3.h"

#include "gfx_options.h"

#include "settlemap.h"
#include "World.h"
extern World *g_theWorld;
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

SettleMap SettleMap::s_settleMap;

/* Removed since static data removed - JJB
#ifdef __GNUC__
// Force instantiation of static template data
static template class MapGrid<double>;
template<>
#endif
std::valarray<double> MapGrid<double>::s_scratch;
*/

SettleMap::SettleMap()
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
	MapPoint rc_pos(0,0);
	MapPoint xy_pos(0,0);
	double value;
	m_settleValues.Clear();

	m_settleValues.Resize( x_size, y_size, 1 );

	m_invalidCells.Resize( x_size, y_size, 0 );

	for (rc_pos.x=0; rc_pos.x < x_size; rc_pos.x++)
		{
			for (rc_pos.y=0; rc_pos.y < y_size; rc_pos.y++)
				{

					xy_pos.rc2xy(rc_pos, *g_theWorld->GetSize());
					if ( (g_theWorld->IsYwrap() == FALSE ) &&
						 ( xy_pos.y < k_minimum_settle_city_size ||
						   xy_pos.y > (y_size - k_minimum_settle_city_size) ))
						{

							value = -1;
						}
					else if ((g_theWorld->IsXwrap() == FALSE) &&
							 ( xy_pos.x < k_minimum_settle_city_size ||
							   xy_pos.x > ((x_size * 2) - k_minimum_settle_city_size) ))
						{

							value = -1;
						}
					else
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
	PLAYER_INDEX playerId = city.GetOwner();
	Assert(citydata);

	sint16 radius = (sint16) g_theCitySizeDB->Get(citydata->GetSizeIndex())->GetIntRadius();

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
		sint32 const new_value = sint32(ComputeSettleValue(pos) * 0.5);
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

	if (rect.IsValid() == false)
		return;





	rect.Expand( 10 );

	MapPoint xy_pos(0,0);
	MapPoint rc_pos(0,0);
	SettleTarget settle_target;









	sint16 rows = rect.GetMaxRows();
	sint16 cols = rect.GetMaxCols();




















































	sint32 settle_threshold = 0;
	if (Diplomat::GetDiplomat(playerId).GetCurrentStrategy().GetMinSettleScore())
	{
		Diplomat::GetDiplomat(playerId).GetCurrentStrategy().GetMinSettleScore(settle_threshold);
	}

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
#endif // _DEBUG

			if ( CanSettlePos(rc_pos) == false)
				continue;

			if ( settle_target.m_value <= settle_threshold)
			{

#ifdef _DEBUG
				if (g_graphicsOptions->IsCellTextOn()) {
					char buf[16];
					sprintf(buf,"(%4.0f)",settle_target.m_value );
					g_graphicsOptions->AddTextToCell(rc_pos, buf, 255);
				}
#endif // _DEBUG

				continue;
			}

			if ( (settle_water == false) &&
				 (g_theWorld->IsWater(rc_pos) == TRUE) )
				continue;

#ifdef _DEBUG
			if (g_graphicsOptions->IsCellTextOn()) {
				char buf[16];
				sprintf(buf,"%4.0f",settle_target.m_value );
				g_graphicsOptions->AddTextToCell(rc_pos, buf, 255);
			}
#endif // _DEBUG

			targets.push_back(settle_target);
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
	strategy.GetMinSettleDistance(min_settle_distance);

	SettleMap::SettleTargetList::iterator iter;
	SettleMap::SettleTargetList::iterator tmp_iter;
	iter = targets.begin();
	settle_target = *iter;
	iter++;

	while( iter != targets.end() )
		{

			g_theWorld->GetContinent(iter->m_pos, cont, is_land);

#ifdef _DEBUG
                      if (is_land) {Assert(cont < max_land_cont);}
                      else {Assert(cont < max_water_cont);}
#endif // _DEBUG
			Assert(cont >= 0);

			if ((is_land == TRUE &&
				 (land_continent_count[cont] >= k_targets_per_continent)) ||
				(is_land == FALSE &&
				 (water_continent_count[cont] >= k_targets_per_continent)))
				{

					iter = targets.erase(iter);
				}
			else
				{

					tmp_iter = targets.begin();
					while (tmp_iter != iter)
					{

                       if (MapPoint::GetSquaredDistance(iter->m_pos, tmp_iter->m_pos) <
                           (min_settle_distance * min_settle_distance) )
					   {
						   break;
					   }
					   tmp_iter++;
					}

					if (tmp_iter == iter)
						iter++;
					else {

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
	bool can_settle = true;






	if (m_invalidCells.Get(rc_pos.x, rc_pos.y) != FALSE)
		can_settle = false;

	return can_settle;
}

void SettleMap::SetCanSettlePos(const MapPoint & rc_pos, const bool can_settle)
{
	m_invalidCells.Set(rc_pos.x, rc_pos.y, (can_settle?FALSE:TRUE));
}


double SettleMap::GetValue(const MapPoint &rc_pos) const
{
	return m_settleValues.GetGridValue(rc_pos);
}
