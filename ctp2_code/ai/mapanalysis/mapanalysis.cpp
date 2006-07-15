//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Map analysis
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
// - Relaxed an Assert to prevent lots of popups with e.g. the CTC mod.
// - Corrected the Distance To Empire Center computation
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Standartized code (May 21st 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "mapanalysis.h"

#include "Army.h"
#include "ArmyData.h"
#include "Unit.h"
#include "UnitData.h"
#include "UnitRecord.h"
#include "citydata.h"
#include "player.h"
#include "dynarr.h"
#include "UnitDynArr.h"
#include "ArmyPool.h"
#include "UnitPool.h"
#include "c3debug.h"
#include "CitySizeRecord.h"
#include "World.h"
#include "TradeRouteData.h"
#include "Cell.h"
#include "Strengths.h"
#include "AgreementMatrix.h"
#include "Diplomat.h"

#include "Scheduler.h"
#include <vector>
using namespace std;


MapGrid < sint32 >::MapGridArray MapGrid < sint32 >::s_scratch;


MapAnalysis MapAnalysis::s_mapAnalysis;


MapAnalysis & MapAnalysis::GetMapAnalysis()
{ 
	return s_mapAnalysis; 
}


void MapAnalysis::Resize(const PLAYER_INDEX & maxPlayerId,
const sint16 & xSize,
const sint16 & ySize,
const sint16 & resolution)
{
    sint32 old_size = m_threatGrid.size();


    m_threatGrid.resize(maxPlayerId);
    m_valueGrid.resize(maxPlayerId);
    m_tradeAtRiskGrid.resize(maxPlayerId);
    m_piracyLossGrid.resize(maxPlayerId);
    m_empireBoundingRect.resize(maxPlayerId);
    m_empireCenter.resize(maxPlayerId);


    for (sint16 player = 0; player < maxPlayerId; player++)
    {
        m_threatGrid[player].Resize(xSize, ySize, resolution);
        m_valueGrid[player].Resize(xSize, ySize, resolution);
        m_tradeAtRiskGrid[player].Resize(xSize, ySize, resolution);
        m_piracyLossGrid[player].Resize(xSize, ySize, resolution);

        if (player >= old_size)
            m_empireBoundingRect[player] = BoundingRect();
    }


	
    m_piracyIncomeMatrix.resize(maxPlayerId * maxPlayerId, 0);

    m_minCityProduction.resize(maxPlayerId);
    m_maxCityProduction.resize(maxPlayerId);
    m_minCityGrowth.resize(maxPlayerId);
    m_maxCityGrowth.resize(maxPlayerId);
    m_minCityGold.resize(maxPlayerId);
    m_maxCityGold.resize(maxPlayerId);
    m_minCityHappiness.resize(maxPlayerId);
    m_maxCityHappiness.resize(maxPlayerId);
    m_minCityThreat.resize(maxPlayerId);
    m_maxCityThreat.resize(maxPlayerId);
    m_nuclearWeapons.resize(maxPlayerId);
    m_bioWeapons.resize(maxPlayerId);
    m_nanoWeapons.resize(maxPlayerId);
    m_specialAttackers.resize(maxPlayerId);
    m_continentSize.resize(maxPlayerId);
    m_totalPopulation.resize(maxPlayerId);
    m_landArea.resize(maxPlayerId);
    m_totalTrade.resize(maxPlayerId);
    m_projectedScience.resize(maxPlayerId);
    m_productionHandicapRatio.resize(maxPlayerId);
    m_goldHandicapRatio.resize(maxPlayerId);
    m_scienceHandicapRatio.resize(maxPlayerId);

    ResizeContinents();

    m_movementTypeUnion.resize(maxPlayerId);
}


void MapAnalysis::ResizeContinents()
{
    sint32 maxLandCont = g_theWorld->GetMaxLandContinent() -
    g_theWorld->GetMinLandContinent();
    sint32 maxPlayerId = m_threatGrid.size();
    m_cityOnContinent.Resize(maxPlayerId, maxLandCont, FALSE);
    m_armyOnContinent.Resize(maxPlayerId, maxLandCont, FALSE);
}


void MapAnalysis::AddPiracyIncome(const PLAYER_INDEX playerId,
const PLAYER_INDEX victimId,
const sint16 route_value)
{

    sint32 index = (victimId * m_piracyLossGrid.size()) + playerId;
    Assert(index >= 0);
    Assert(static_cast<size_t>(index) < m_piracyIncomeMatrix.size());

    m_piracyIncomeMatrix[index] += route_value;
}


void MapAnalysis::BeginTurn()
{
    MapPoint pos;
    Army army;
    Unit city;
    Unit unit;
    sint32 num_armies;
    sint32 num_units;
    sint32 num_cities;
    Player * player_ptr;
    sint32 hitpoints;
    sint32 defense_strength;
    sint32 attack_strength;
    sint32 ranged_strength;
    sint32 total_value;
    sint32 defense_count;
    sint32 ranged_count;
    sint32 prod;
    sint32 food;
    sint32 gold;

    sint32 happiness;
    sint32 threat;

    sint32 i, j, k;

    m_minCityProductionAll = 99999;
    m_minCityGrowthAll = 99999;
    m_minCityGoldAll = 99999;

    m_maxCityProductionAll = -99999;
    m_maxCityGrowthAll = -99999;
    m_maxCityGoldAll = -99999;

    TradeDynamicArray * trade_routes;
    Army pirate_army;
    sint32 route_value;

    BOOL is_land;
    sint32 cont;


    m_piracyIncomeMatrix.assign(m_piracyIncomeMatrix.size(), 0);


    m_cityOnContinent.Reset(FALSE);
    m_armyOnContinent.Reset(FALSE);

    m_worldPopulation = 0;


    ComputeHandicapRatios();


	
	size_t player;
    for (player = 0; player < m_threatGrid.size(); ++player)
    {
        m_threatGrid[player].Clear();
        m_valueGrid[player].Clear();
        m_tradeAtRiskGrid[player].Clear();
        m_piracyLossGrid[player].Clear();


        player_ptr = g_player[player];

        m_minCityProduction[player] = 99999;
        m_minCityGrowth[player] = 99999;
        m_minCityGold[player] = 99999;
        m_minCityHappiness[player] = 99999;
        m_minCityThreat[player] = 99999;

        m_maxCityProduction[player] = -99999;
        m_maxCityGrowth[player] = -99999;
        m_maxCityGold[player] = -99999;
        m_maxCityHappiness[player] = -99999;
        m_maxCityThreat[player] = -99999;

        m_movementTypeUnion[player] = 0x0;

        m_nuclearWeapons[player] = 0;
        m_bioWeapons[player] = 0;
        m_nanoWeapons[player] = 0;
        m_specialAttackers[player] = 0;
        m_continentSize[player] = 0;

        m_totalPopulation[player] = 0;
        m_landArea[player] = 0;
        m_totalTrade[player] = 0;


        if (player_ptr == NULL)
            continue;


		
        m_projectedScience[player] =
        player_ptr->m_advances->GetProjectedScience();

        m_totalPopulation[player] = static_cast<sint16>(player_ptr->GetTotalPopulation());
        m_landArea[player] = static_cast<sint16>(player_ptr->GetLandArea());
        m_worldPopulation += m_totalPopulation[player];


		
        num_units = player_ptr->m_all_units->Num();
        for (i = 0; i < num_units; i++)
        {
            unit = player_ptr->m_all_units->Access(i);
            Assert(g_theUnitPool->IsValid(unit));

            if (unit.GetDBRec()->GetNumSpecialAttacks() > 0)
                m_specialAttackers[player] ++;
        }

        num_armies = player_ptr->m_all_armies->Num();
        for (i = 0; i < num_armies; i++)
        {
            army = player_ptr->m_all_armies->Access(i);
            Assert(g_theArmyPool->IsValid(army));
            army->GetPos(pos);

            army->GetArmyStrength(hitpoints,
            defense_count,
            ranged_count,
            attack_strength,
            defense_strength,
            ranged_strength,
            total_value);
			Assert(total_value >= 0);

            m_nuclearWeapons[player] += army->CountNuclearUnits();
            m_bioWeapons[player] += army->CountBioUnits();
            m_nanoWeapons[player] += army->CountNanoUnits();


            if (m_empireBoundingRect[player].IsValid() == false)
			{
				UpdateBoundingRectangle(army);	
			}


            m_threatGrid[player].AddValue(pos, attack_strength + ranged_strength);


            m_valueGrid[player].AddValue(pos, total_value);


            g_theWorld->GetContinent(pos, cont, is_land);
            bool is_military = (attack_strength > 0 || ranged_strength > 0) &&
            !army->IsCivilian();
            if (is_land && is_military)
			{
				m_armyOnContinent.Set(player, cont, TRUE);
			}


            m_movementTypeUnion[player] |= army.GetMovementType();

            Assert(m_threatGrid[player].GetTotalValue() >= 0.0);
            Assert(m_valueGrid[player].GetTotalValue() >= 0);
            Assert(m_nuclearWeapons[player] >= 0);
            Assert(m_bioWeapons[player] >= 0);
            Assert(m_nanoWeapons[player] >= 0);
            Assert(m_specialAttackers[player] >= 0);
            Assert(m_continentSize[player] >= 0);
            Assert(m_totalPopulation[player] >= 0);
            Assert(m_landArea[player] >= 0);
            Assert(m_totalTrade[player] >= 0);
        }


        num_cities = player_ptr->m_all_cities->Num();
        for (i = 0; i < num_cities; i++)
        {
            city = player_ptr->m_all_cities->Access(i);
            Assert(g_theUnitPool->IsValid(city));
            city.GetPos(pos);
            total_value = city->GetCityData()->GetValue();


            m_totalTrade[player] += city->GetCityData()->GetGoldFromTradeRoutes();


            m_valueGrid[player].AddValue(pos, total_value);


            trade_routes = city.CD()->GetTradeSourceList();
            Assert(trade_routes != NULL);


            for (j = 0; j < trade_routes->Num(); j++)
            {
                route_value = trade_routes->Get(j)->GetValue();
                pirate_army = trade_routes->Get(j)->GetPiratingArmy();
                if (pirate_army.m_id != 0 && g_theArmyPool->IsValid(pirate_army))
                {

                    m_piracyLossGrid[player].
                    AddValue(pirate_army->RetPos(), route_value);


                    AddPiracyIncome(pirate_army.GetOwner(), player, static_cast<sint16>(route_value));


                    m_valueGrid[pirate_army.GetOwner()].
                    AddValue(pirate_army->RetPos(), route_value);
                }


				
				
                const DynamicArray < MapPoint > * path = trade_routes->Get(j)->GetPath();
                sint32 per_cell_value =
                (sint32)(((double)route_value / path->Num()) * 1000.0);
                for (k = 0; k < path->Num(); k++)
                {
                    m_tradeAtRiskGrid[player].AddValue(path->Get(k),
                    per_cell_value);
                }
            }


			
			

            prod = city->GetCityData()->GetGrossCityProduction();
            if (prod < m_minCityProduction[player])
                m_minCityProduction[player] = prod;
            if (prod < m_minCityProductionAll)
                m_minCityProductionAll = prod;
            if (prod > m_maxCityProduction[player])
                m_maxCityProduction[player] = prod;
            if (prod > m_maxCityProductionAll)
                m_maxCityProductionAll = prod;

            food = city->GetCityData()->GetGrowthRate();
            if (food < m_minCityGrowth[player])
                m_minCityGrowth[player] = food;
            if (food < m_minCityGrowthAll)
                m_minCityGrowthAll = food;
            if (food > m_maxCityGrowth[player])
                m_maxCityGrowth[player] = food;
            if (food > m_maxCityGrowthAll)
                m_maxCityGrowthAll = food;

            gold = city->GetCityData()->GetGrossCityGold();
            if (gold < m_minCityGold[player])
                m_minCityGold[player] = gold;
            if (gold < m_minCityGoldAll)
                m_minCityGoldAll = gold;
            if (gold > m_maxCityGold[player])
                m_maxCityGold[player] = gold;
            if (gold > m_maxCityGoldAll)
                m_maxCityGoldAll = gold;

            happiness = (sint32)city->GetCityData()->GetHappiness();
            if (happiness < m_minCityHappiness[player])
                m_minCityHappiness[player] = happiness;
            if (happiness > m_maxCityHappiness[player])
                m_maxCityHappiness[player] = happiness;


            g_theWorld->GetContinent(pos, cont, is_land);
            if (is_land)
            {
                m_cityOnContinent.Set(player, cont, TRUE);


                m_continentSize[player] += g_theWorld->GetLandContinentSize(cont) / num_cities;
            }

        }
    }


    DPRINTF(k_DBG_MAPANALYSIS, ("BEFORE RELAX:\n"));
    DebugLog();


    const sint8 cycles = 1;
    const float coef = 0.8f;
    for (player = 0; player < m_threatGrid.size(); player++)
    {
        m_threatGrid[player].Relax(cycles, coef);
        m_valueGrid[player].Relax(cycles, coef);


        player_ptr = g_player[player];


        if (player_ptr == NULL)
            continue;

        num_cities = player_ptr->m_all_cities->Num();
        for (i = 0; i < num_cities; i++)
        {
            city = player_ptr->m_all_cities->Access(i);
            threat = GetThreat(player, city.RetPos());
            if (threat < m_minCityThreat[player])
                m_minCityThreat[player] = threat;

            if (threat > m_maxCityThreat[player])
                m_maxCityThreat[player] = threat;
        }
    }


    DPRINTF(k_DBG_MAPANALYSIS, ("RELAXED:\n"));
    DebugLog();
}



sint32 MapAnalysis::GetThreat(const PLAYER_INDEX & player, const MapPoint & pos) const
{
    sint32 threat = 0;

    for (sint32 opponent = m_threatGrid.size() - 1; opponent >= 0; opponent--)
    {


		
		
        if (    (player == opponent) 
             || !Scheduler::CachedHasContactWithExceptSelf(player, opponent)
           )
        {
            continue;
        }

		
        if (!Scheduler::CachedIsNeutralRegard(player, opponent))
            threat += m_threatGrid[opponent].GetGridValue(pos);
    }
    return threat;
}


sint32 MapAnalysis::GetMaxThreat(const PLAYER_INDEX & player) const
{
    sint32 threat = 0;

    for (sint32 opponent = m_threatGrid.size() - 1; opponent >= 0; opponent--)
    {
        if (opponent != player &&
			!Scheduler::CachedIsNeutralRegard(player, opponent)) {
			threat += m_threatGrid[opponent].GetMaxGridValue();
		}
    }
    return threat;
}


sint32 MapAnalysis::GetPower(const PLAYER_INDEX & player, const MapPoint & pos) const
{
    sint32 power = 0;

    for (sint32 ally = m_threatGrid.size() - 1; ally >= 0; ally--)
    {

        if (Scheduler::CachedIsAllyRegard(player, ally))
		{
			power += m_threatGrid[ally].GetGridValue(pos);
		}
    }
    return power;
}


sint32 MapAnalysis::GetMaxPower(const PLAYER_INDEX & player) const
{
    sint32 power = 0;

    for (sint32 ally = m_threatGrid.size() - 1; ally >= 0; ally--)
    {
        if (Scheduler::CachedIsAllyRegard(player, ally))
		{
			power += m_threatGrid[ally].GetMaxGridValue();
		}
    }
    return power;
}


sint32 MapAnalysis::GetAlliedValue(const PLAYER_INDEX & player, const MapPoint & pos) const
{
    sint32 value = 0;


    for (sint16 ally = m_threatGrid.size() - 1; ally >= 0; ally--)
    {

        if (Scheduler::CachedIsAllyRegard(player, ally))
		{
			value += m_valueGrid[ally].GetGridValue(pos);
		}
    }
    return value;
}


sint32 MapAnalysis::GetMaxAlliedValue(const PLAYER_INDEX & player) const
{
    sint32 value = 0;

    for (sint16 ally = m_threatGrid.size() - 1; ally >= 0; ally--)
    {

        if (Scheduler::CachedIsAllyRegard(player, ally))
            value += m_threatGrid[ally].GetMaxGridValue();
    }
    return value;
}


sint32 MapAnalysis::GetEnemyValue(const PLAYER_INDEX & player, const MapPoint & pos) const
{
    sint32 value = 0;

    for (sint16 opponent = m_valueGrid.size() - 1; opponent >= 0; opponent--)
    {
        if (!Scheduler::CachedIsNeutralRegard(player, opponent))
            value += m_valueGrid[opponent].GetGridValue(pos);
    }
    return value;
}


sint32 MapAnalysis::GetMaxEnemyValue(const PLAYER_INDEX & player) const
{
    sint32 value = 0;

    for (sint16 opponent = m_threatGrid.size() - 1; opponent >= 0; opponent--)
    {
        if (!Scheduler::CachedIsNeutralRegard(player, opponent))
            value += m_threatGrid[opponent].GetMaxGridValue();
    }
    return value;
}


double MapAnalysis::GetProductionRank(const CityData * city, const bool & all_players) const
{
    Assert(city);
    sint32 prod = city->GetGrossCityProduction();
    PLAYER_INDEX owner = city->GetOwner();

    if (all_players || (m_maxCityProduction[owner] - m_minCityProduction[owner]) <= 0)
    {
        if ((m_maxCityProductionAll - m_minCityProductionAll) > 0)
            return ((double)(prod - m_minCityProductionAll) / (double)(m_maxCityProductionAll - m_minCityProductionAll));
    }
    else
		return ((double)(prod - m_minCityProduction[owner]) / (double) (m_maxCityProduction[owner] - m_minCityProduction[owner]));

    return 0.0;
}

double MapAnalysis::GetGrowthRank(const CityData * city, const bool & all_players) const
{
    Assert(city);
    sint32 food = city->GetGrowthRate();
    PLAYER_INDEX owner = city->GetOwner();


    if (all_players || (m_maxCityGrowth[owner] - m_minCityGrowth[owner]) <= 0)
    {
        if ((m_maxCityGrowthAll - m_minCityGrowthAll) > 0)
            return ((double)(food - m_minCityGrowthAll) / (double)(m_maxCityGrowthAll - m_minCityGrowthAll));
    }
	else
	{
		return ((double)(food - m_minCityGrowth[owner]) / (double) (m_maxCityGrowth[owner] - m_minCityGrowth[owner]));
	}

    return 0.0;
}

double MapAnalysis::GetCommerceRank(const CityData * city, const bool & all_players) const
{
    Assert(city);
    sint32 commerce = city->GetGrossCityGold();
    PLAYER_INDEX owner = city->GetOwner();


    if (all_players || (m_maxCityGold[owner] - m_minCityGold[owner]) <= 0)
    {
        if ((m_maxCityGoldAll - m_minCityGoldAll) > 0)
            return ((double)(commerce - m_minCityGoldAll) / (double)(m_maxCityGoldAll - m_minCityGoldAll));
    }
	else 
	{
		return ((double)(commerce - m_minCityGold[owner]) / (double) (m_maxCityGold[owner] - m_minCityGold[owner]));
	}

    return 0.0;
}

double MapAnalysis::GetHappinessRank(const CityData * city) const
{
    Assert(city);
    double happiness = city->GetHappiness();
    PLAYER_INDEX owner = city->GetOwner();

    if ((m_maxCityHappiness[owner] - m_minCityHappiness[owner]) > 0)
        return ((double)(happiness - m_minCityHappiness[owner]) /
        (double)(m_maxCityHappiness[owner] - m_minCityHappiness[owner]));

    return 1.0;
}


double MapAnalysis::GetThreatRank(const CityData * city) const
{
    Assert(city);
    PLAYER_INDEX owner = city->GetOwner();
    sint32 threat = GetThreat(owner, city->GetHomeCity().RetPos());

    if ((m_maxCityThreat[owner] - m_minCityThreat[owner]) > 0)
        return ((double)(threat - m_minCityThreat[owner]) /
        (double)(m_maxCityThreat[owner] - m_minCityThreat[owner]));

    return 0.0;
}


double MapAnalysis::GetPowerRank(const CityData * city) const
{
    Assert(city);
    PLAYER_INDEX owner = city->GetOwner();
    double power = GetPower(owner, city->GetHomeCity().RetPos());
    double max_power = GetMaxPower(owner);

    if (max_power > 0.0)
        return (power / max_power);

    return 0.0;
}



void MapAnalysis::UpdateBoundingRectangle(const Army & army)
{

    const PLAYER_INDEX player = army.GetOwner();


    MapPoint xy_pos(0, 0);
    MapPoint rc_pos;
    army.GetPos(rc_pos);
    xy_pos.rc2xy(rc_pos, * g_theWorld->GetSize());


    MapPoint xy_map_size(g_theWorld->GetWidth() * 2, g_theWorld->GetHeight());

    if (!m_empireBoundingRect[player].IsValid())
    {
        m_empireBoundingRect[player].Initialize
			(xy_pos, 2, xy_map_size, g_theWorld->IsXwrap(), g_theWorld->IsYwrap());
    }


    BoundingRect armyRect(xy_pos, 2, xy_map_size, g_theWorld->IsXwrap(), g_theWorld->IsYwrap());


    bool added = m_empireBoundingRect[player].Add(armyRect);
    Assert(added);


    m_empireCenter[player].xy2rc(m_empireBoundingRect[player].GetCenter(), * g_theWorld->GetSize());
    DPRINTF(k_DBG_MAPANALYSIS, ("Empire Center for player %d :  rc(%3d,%3d)   \n",
		player,
		m_empireCenter[player].x,
		m_empireCenter[player].y));

}


void MapAnalysis::UpdateBoundingRectangle(const Unit & city)
{

    const PLAYER_INDEX player = city.GetOwner();


    MapPoint xy_center(0, 0);
    xy_center.rc2xy(city.RetPos(), * g_theWorld->GetSize());


    MapPoint xy_map_size(g_theWorld->GetWidth() * 2, g_theWorld->GetHeight());


    if (!m_empireBoundingRect[player].IsValid())
    {
        m_empireBoundingRect[player].Initialize
			(xy_center, 0, xy_map_size, g_theWorld->IsXwrap(), g_theWorld->IsYwrap());
    }


    sint32 city_size = city->GetCityData()->GetSizeIndex();
    if (city_size >= g_theCitySizeDB->NumRecords())
	{
		city_size = g_theCitySizeDB->NumRecords() - 1;
	}
    sint16 xy_radius = (sint16)g_theCitySizeDB->Get(city_size)->GetIntRadius();


    if ((xy_radius % 2) != 0)
        xy_radius++;


    BoundingRect cityRect(xy_center, xy_radius, xy_map_size, 
						  g_theWorld->IsXwrap(), g_theWorld->IsYwrap()
						 );


    bool added = m_empireBoundingRect[player].Add(cityRect);
    Assert(added);


    m_empireCenter[player].xy2rc(m_empireBoundingRect[player].GetCenter(), * g_theWorld->GetSize());
    DPRINTF(k_DBG_SCHEDULER, ("Empire Center for player %d :  rc(%3d,%3d)   \n",
		player,
		m_empireCenter[player].x,
		m_empireCenter[player].y));

}


const BoundingRect & MapAnalysis::GetBoundingRectangle(const PLAYER_INDEX & player) const
{ return m_empireBoundingRect[player]; }


const MapPoint & MapAnalysis::GetEmpireCenter(const PLAYER_INDEX player) const
{ return m_empireCenter[player]; }


const MapPoint & MapAnalysis::GetNearestForeigner(const PLAYER_INDEX player, const MapPoint & pos) const
{
    sint32 min_squared_distance = (g_theWorld->GetWidth() * g_theWorld->GetHeight());
    min_squared_distance *= min_squared_distance;
    size_t	closest_player = 1;

    for (size_t i = 0; i < m_empireCenter.size(); i++)
    {
        if (i == static_cast<size_t const>(player))
            continue;

		if (m_empireCenter[i].x != 0 && m_empireCenter[i].y != 0)
		{
            sint32 tmp_squared_distance = MapPoint::GetSquaredDistance(m_empireCenter[i], pos);
            if (tmp_squared_distance < min_squared_distance)
            {
                min_squared_distance = tmp_squared_distance;
                closest_player = i;
            }
		}
    }
    Assert(closest_player < m_empireCenter.size());
    return m_empireCenter[closest_player];
}


double MapAnalysis::CityAtRiskRatio(const Unit city, const PLAYER_INDEX opponentId) const
{
    MapPoint pos;
    Assert(g_theUnitPool->IsValid(city));
    city.GetPos(pos);
    PLAYER_INDEX playerId = city.GetOwner();
    double ratio = 0.0;


    Player * player_ptr = g_player[playerId];


    if (player_ptr == NULL)
        return 0.0;


    if (opponentId != -1 && g_player[opponentId] == NULL)
        return 0.0;

    sint32 player_threat;
    sint32 opponent_threat;

    player_threat = m_threatGrid[playerId].GetGridValue(pos);

    if (opponentId != -1)
        opponent_threat = m_threatGrid[opponentId].GetGridValue(pos);
    else
		
        opponent_threat = GetThreat(playerId, pos);

    if (player_threat > 0)
		{
			
			
			ratio = ((double) opponent_threat / (double) player_threat) * 0.2;
		}
    else if (opponent_threat <= 0)
		{
			
			ratio  = 0.0;
		}
	else 
		{
			
			ratio = 1.0;
		}


    if (ratio > 1.0)
        ratio = 1.0;
    else if (ratio < 0.0)
        ratio = 0.0;

    return ratio;
}


sint32 MapAnalysis::MostAtRiskCity(const PLAYER_INDEX & playerId,
Unit & city,
const PLAYER_INDEX & opponentId) const
{

    double most_at_risk_value = 0.0;
    double ratio = 0.0;


    Player * player_ptr = g_player[playerId];


    if (player_ptr == NULL)
        return 0;


    if (opponentId != -1 && g_player[opponentId] == NULL)
        return 0;

    Unit tmp_city;
    sint32 num_cities = player_ptr->m_all_cities->Num();
    for (sint32 i = 0; i < num_cities; i++)
    {
        tmp_city = player_ptr->m_all_cities->Access(i);
        Assert(g_theUnitPool->IsValid(tmp_city));

        ratio = CityAtRiskRatio(tmp_city, opponentId);

        if (tmp_city->GetCityData()->GetValue() * ratio > most_at_risk_value)
        {
            most_at_risk_value = tmp_city->GetCityData()->GetValue() * ratio;
            city.m_id = tmp_city.m_id;
        }
    }


    return static_cast<sint32>(ratio * 100.0);
}


sint32 MapAnalysis::AtRiskCitiesValue(const PLAYER_INDEX & playerId,
const PLAYER_INDEX & opponentId) const
{

    Unit city;
    double at_risk_value = 0.0;
    double ratio;


    Player * player_ptr = g_player[playerId];


    if (player_ptr == NULL)
        return 0;


    if (opponentId != -1 && g_player[opponentId] == NULL)
        return 0;

    sint32 opponent_nukes =
    GetNuclearWeaponsCount(opponentId);


//  bool no_nuke_defense = (GetNuclearWeaponsCount(playerId) == 0);

    sint32 num_cities = player_ptr->m_all_cities->Num();
    for (sint32 i = 0; i < num_cities; i++)
    {
        city = player_ptr->m_all_cities->Access(i);
        Assert(g_theUnitPool->IsValid(city));


		
        if (i < opponent_nukes && !city.GetCityData()->SafeFromNukes())
		{
			ratio = 1.0;
		}
		else
		{
			ratio = CityAtRiskRatio(city, opponentId);
		}

        at_risk_value += city->GetCityData()->GetValue() * ratio;
    }

    if (m_valueGrid[playerId].GetTotalValue() > 0.0)

        return (sint32)(((double)at_risk_value / m_valueGrid[playerId].GetTotalValue()) * 100.0);
    else
		
        return 100;
}


sint32 MapAnalysis::GetPiracyIncomeByPlayer(const PLAYER_INDEX playerId,
const PLAYER_INDEX victimId) const
{
    sint32 index = (victimId * m_piracyLossGrid.size()) + playerId;
    Assert(index >= 0);
    Assert(static_cast<size_t>(index) < m_piracyIncomeMatrix.size());

    return m_piracyIncomeMatrix[index];
}


bool MapAnalysis::ShareContinent(const PLAYER_INDEX playerId,
const PLAYER_INDEX foreignerId) const
{
    sint32 maxLandCont = g_theWorld->GetMaxLandContinent() -
    g_theWorld->GetMinLandContinent();


	
    for (sint32 cont = 0; cont < maxLandCont; cont++)
    {
        if ((m_cityOnContinent.Get(playerId, cont)) &&
        (m_armyOnContinent.Get(foreignerId, cont) ||
        m_cityOnContinent.Get(foreignerId, cont)))
		{
			return true;
		}
    }
    return false;
}


bool MapAnalysis::PlayerCanEnter(const PLAYER_INDEX playerId,
const MapPoint & pos) const
{
    if (g_theWorld->GetCell(pos)->CanEnter(m_movementTypeUnion[playerId]))
        return true;
    else
        return false;
}


sint16 MapAnalysis::GetNuclearWeaponsCount(const PLAYER_INDEX playerId) const
{
	return m_nuclearWeapons[playerId];
}


sint16 MapAnalysis::GetBioWeaponsCount(const PLAYER_INDEX playerId) const
{
	return m_bioWeapons[playerId];
}


sint16 MapAnalysis::GetNanoWeaponsCount(const PLAYER_INDEX playerId) const
{
	return m_nanoWeapons[playerId];
}


void MapAnalysis::SetNuclearWeaponsCount(const PLAYER_INDEX playerId, const sint32 value)
{
	m_nuclearWeapons[playerId] = static_cast<sint16>(value);
}


void MapAnalysis::SetBioWeaponsCount(const PLAYER_INDEX playerId, const sint32 value)
{
	m_bioWeapons[playerId] = static_cast<sint16>(value);
}


void MapAnalysis::SetNanoWeaponsCount(const PLAYER_INDEX playerId, const sint32 value)
{
	m_nanoWeapons[playerId] = static_cast<sint16>(value);
}


sint16 MapAnalysis::GetSpecialAttackers(const PLAYER_INDEX playerId) const
{
	return m_specialAttackers[playerId];
}



sint32 MapAnalysis::AverageSettledContinentSize(const PLAYER_INDEX playerId) const
{
	return m_continentSize[playerId];
}


sint16 MapAnalysis::GetTotalPopulation(const PLAYER_INDEX playerId) const
{
    if (playerId == -1)
        return m_worldPopulation;
    else
        return m_totalPopulation[playerId];
}


sint16 MapAnalysis::GetLandArea(const PLAYER_INDEX playerId) const
{
	return m_landArea[playerId];	
}


double MapAnalysis::GetPopulationPercent(const PLAYER_INDEX playerId) const
{
    Assert(m_worldPopulation);
    return (double)m_totalPopulation[playerId] /
    m_worldPopulation;
}


double MapAnalysis::GetLandPercent(const PLAYER_INDEX playerId) const
{
    Assert(g_theWorld);
    return (double)m_landArea[playerId] /
    (g_theWorld->GetWidth() * g_theWorld->GetHeight());
}


void MapAnalysis::ComputeAllianceSize(const PLAYER_INDEX playerId, PLAYER_INDEX & leaderId, double & population, double & land) const
{
    sint32 alliance_population = 0;
    sint32 alliance_land = 0;
    population = 0.0;
    land = 0.0;
    leaderId = playerId;
    sint32 max_population = m_totalPopulation[playerId];
    sint32 allies = 0;
    for 
    (
        PLAYER_INDEX foreignerId = 0; 
        static_cast<size_t>(foreignerId) < m_threatGrid.size(); 
        ++foreignerId
    )
    {
        if (g_player[foreignerId] == NULL)
            continue;

        if (foreignerId == playerId ||
        AgreementMatrix::s_agreements.HasAgreement(playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE))
        {
            alliance_population += m_totalPopulation[foreignerId];
            alliance_land += m_landArea[foreignerId];
            allies++;


					
            if ((g_player[foreignerId]->m_playerType != PLAYER_TYPE_ROBOT) &&
            (m_totalPopulation[foreignerId] > max_population))
            {
                max_population = m_totalPopulation[foreignerId];
                leaderId = foreignerId;
            }
        }
    }

    if (m_worldPopulation)
    {
        Assert(g_theWorld);
        population = (double)alliance_population / m_worldPopulation;
        land = (double)alliance_land / (g_theWorld->GetWidth() * g_theWorld->GetHeight());
    }


    if (allies <= 1)
        leaderId = -1;
}

void MapAnalysis::ComputeHandicapRatios()
{
    sint32 max_production_strength = -1;
    sint32 min_production_strength = -1;
    sint32 max_science_strength = -1;
    sint32 min_science_strength = -1;
    sint32 max_gold_strength = -1;
    sint32 min_gold_strength = -1;

    sint32 strValue;


    size_t player;
    for (player = 0; player < m_threatGrid.size(); player++)
    {

        if (g_player[player] == NULL)
            continue;


        if (g_player[player]->GetPlayerType() == PLAYER_TYPE_ROBOT)
            continue;


        strValue = g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_MILITARY);
        strValue += g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_BUILDINGS);
        strValue += g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_WONDERS);
        strValue += g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_PRODUCTION);

        if (strValue < min_production_strength || min_production_strength == -1)
            min_production_strength = strValue;

        if (strValue > max_production_strength || max_production_strength == -1)
            max_production_strength = strValue;


        strValue = g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_KNOWLEDGE);

        if (strValue < min_science_strength || min_science_strength == -1)
            min_science_strength = strValue;

        if (strValue > max_science_strength || max_science_strength == -1)
            max_science_strength = strValue;


        strValue = g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_GOLD);

        if (strValue < min_gold_strength || min_gold_strength == -1)
            min_gold_strength = strValue;

        if (strValue > max_gold_strength || max_gold_strength == -1)
            max_gold_strength = strValue;
    }


    for (player = 0; player < m_threatGrid.size(); player++)
    {

        m_productionHandicapRatio[player] = 1.0;
        m_goldHandicapRatio[player] = 1.0;
        m_scienceHandicapRatio[player] = 1.0;


        if (g_player[player] == NULL)
            continue;


        if (g_player[player]->GetPlayerType() != PLAYER_TYPE_ROBOT)
            continue;


        strValue = g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_MILITARY);
        strValue += g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_BUILDINGS);
        strValue += g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_WONDERS);
        strValue += g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_PRODUCTION);

        if (strValue < min_production_strength && min_production_strength > 0)
		{
			
			m_productionHandicapRatio[player] = (double) strValue / min_production_strength;
		}
        else if (strValue > max_production_strength && max_production_strength > 0)
		{
			
			m_productionHandicapRatio[player] = (double) strValue / max_production_strength;
		}
		else
		{
			
			m_productionHandicapRatio[player] = 1.0;
		}


        strValue = g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_GOLD);

        if (strValue < min_gold_strength && min_gold_strength > 0)
		{
			
			m_goldHandicapRatio[player] = (double) strValue / min_gold_strength;
		}
        else if (strValue > max_gold_strength && max_gold_strength > 0)
		{
			
			m_goldHandicapRatio[player] = (double) strValue / max_gold_strength;
		}
		else
		{
			
			m_goldHandicapRatio[player] = 1.0;
		}


        strValue = g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_KNOWLEDGE);

        if (strValue < min_science_strength && min_science_strength > 0)
		{
			
			m_scienceHandicapRatio[player] = (double) strValue / min_science_strength;
		}
        else if (strValue > max_science_strength && max_science_strength > 0)
		{
			
			m_scienceHandicapRatio[player] = (double) strValue / max_science_strength;
		}
		else
		{

			m_scienceHandicapRatio[player] = 1.0;
    }

}

}

double MapAnalysis::GetProductionHandicapRatio(const PLAYER_INDEX playerId) const
{
    Assert(playerId >= 0);
    Assert(static_cast<size_t>(playerId) < m_productionHandicapRatio.size());
    return m_productionHandicapRatio[playerId];
}

double MapAnalysis::GetGoldHandicapRatio(const PLAYER_INDEX playerId) const
{
    Assert(playerId >= 0);
    Assert(static_cast<size_t>(playerId) < m_goldHandicapRatio.size());
    return m_goldHandicapRatio[playerId];
}


double MapAnalysis::GetScienceHandicapRatio(const PLAYER_INDEX playerId) const
{
    Assert(playerId >= 0);
    Assert(static_cast<size_t>(playerId) < m_scienceHandicapRatio.size());
    return m_scienceHandicapRatio[playerId];
}


void MapAnalysis::DebugLog() const
{
    for (size_t player = 0; player < m_threatGrid.size(); ++player)
    {
        if (m_threatGrid[player].GetMaxValue() > 0)
        {
            DPRINTF(k_DBG_MAPANALYSIS, ("THREAT Map (player = %d): \n", player));
            DPRINTF(k_DBG_MAPANALYSIS, ("%s",
            m_threatGrid[player].GetDebugString().c_str()));

        }
        if (m_valueGrid[player].GetMaxValue() > 0)
        {
            DPRINTF(k_DBG_MAPANALYSIS, ("VALUE Map (player = %d): \n", player));
            DPRINTF(k_DBG_MAPANALYSIS, ("%s",
            m_valueGrid[player].GetDebugString().c_str()));
        }
    }
}






MapAnalysis::MapAnalysis()
{

}

MapAnalysis::~MapAnalysis()
{
    MapGridVector().swap(m_threatGrid);
    MapGridVector().swap(m_valueGrid);
    MapGridVector().swap(m_tradeAtRiskGrid);
    MapGridVector().swap(m_piracyLossGrid);
    BoundingRectVector().swap(m_empireBoundingRect);
    MapPointVector().swap(m_empireCenter);
    Sint16Vector().swap(m_piracyIncomeMatrix);
    Sint32Vector().swap(m_minCityProduction);
    Sint32Vector().swap(m_maxCityProduction);
    Sint32Vector().swap(m_minCityGrowth);
    Sint32Vector().swap(m_maxCityGrowth);
    Sint32Vector().swap(m_minCityGold);
    Sint32Vector().swap(m_maxCityGold);
    Sint32Vector().swap(m_minCityHappiness);
    Sint32Vector().swap(m_maxCityHappiness);
    Sint32Vector().swap(m_minCityThreat);
    Sint32Vector().swap(m_maxCityThreat);
    Sint16Vector().swap(m_nuclearWeapons);
    Sint16Vector().swap(m_bioWeapons);
    Sint16Vector().swap(m_nanoWeapons);
    Sint16Vector().swap(m_specialAttackers);
    Sint32Vector().swap(m_continentSize);
    Sint16Vector().swap(m_totalPopulation);
    Sint16Vector().swap(m_landArea);
    Sint32Vector().swap(m_totalTrade);
    Sint32Vector().swap(m_projectedScience);
    DoubleVector().swap(m_productionHandicapRatio);
    DoubleVector().swap(m_goldHandicapRatio);
    DoubleVector().swap(m_scienceHandicapRatio);
    Uint32Vector().swap(m_movementTypeUnion);
}
