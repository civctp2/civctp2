//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : AI and automated governor handling.
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
// - Added AI attack, defense, ranged, land bombard, sea bombard, and air bombard
//   player power grids to the mapanalysis. (30-Apr-2008 Martin G�hmann)
// - Fixed AI city rank calculation. (9-Nov-2009 Martin G�hmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __MAP_ANALYSIS_H__
#define __MAP_ANALYSIS_H__

#include <vector>

class MapAnalysis;

#include "mapgrid.h"
#include "citydata.h"
#include "bit_table.h"

class MapAnalysis
{
public:

    typedef std::vector<MapGrid<sint32> > MapGridVector;
    typedef std::vector<sint32> Sint32Vector;
    typedef std::vector<sint16> Sint16Vector;
    typedef std::vector<uint32> Uint32Vector;
    typedef std::vector<MapPoint> MapPointVector;
    typedef std::vector<double> DoubleVector;

	static MapAnalysis & GetMapAnalysis();

	~MapAnalysis();

	void Resize( const PLAYER_INDEX & maxPlayerId,
				 const sint16 & xSize,
				 const sint16 & ySize,
				 const sint16 & resolution );

	void ResizeContinents();

	void BeginTurn();
	void RecalcCityRanks(size_t player);

	//     The enemy grids
	sint32    GetEnemyGrid        (const MapGridVector & gridVector, const size_t & player, const MapPoint & pos) const;
	sint32 GetMaxEnemyGrid        (const MapGridVector & gridVector, const size_t & player) const;

	sint32 GetThreat              (const size_t & player, const MapPoint & pos) const { return GetEnemyGrid(m_threatGrid,      player, pos); };
	sint32 GetEnemyAttack         (const size_t & player, const MapPoint & pos) const { return GetEnemyGrid(m_attackGrid,      player, pos); };
	sint32 GetEnemyDefense        (const size_t & player, const MapPoint & pos) const { return GetEnemyGrid(m_defenseGrid,     player, pos); };
	sint32 GetEnemyRanged         (const size_t & player, const MapPoint & pos) const { return GetEnemyGrid(m_rangedGrid,      player, pos); };
	sint32 GetEnemyBombardLand    (const size_t & player, const MapPoint & pos) const { return GetEnemyGrid(m_bombardLandGrid, player, pos); };
	sint32 GetEnemyBombardSea     (const size_t & player, const MapPoint & pos) const { return GetEnemyGrid(m_bombardSeaGrid,  player, pos); };
	sint32 GetEnemyBombardAir     (const size_t & player, const MapPoint & pos) const { return GetEnemyGrid(m_bombardAirGrid,  player, pos); };
	sint32 GetEnemyCityValue      (const size_t & player, const MapPoint & pos) const { return GetEnemyGrid(m_cityValueGrid,   player, pos); };
	sint32 GetEnemyValue          (const size_t & player, const MapPoint & pos) const { return GetEnemyGrid(m_valueGrid,       player, pos); };

	sint32 GetMaxThreat           (const size_t & player) const { return GetMaxEnemyGrid(m_threatGrid,      player); };
	sint32 GetMaxEnemyAttack      (const size_t & player) const { return GetMaxEnemyGrid(m_attackGrid,      player); };
	sint32 GetMaxEnemyDefense     (const size_t & player) const { return GetMaxEnemyGrid(m_defenseGrid,     player); };
	sint32 GetMaxEnemyRanged      (const size_t & player) const { return GetMaxEnemyGrid(m_rangedGrid,      player); };
	sint32 GetMaxEnemyBombardLand (const size_t & player) const { return GetMaxEnemyGrid(m_bombardLandGrid, player); };
	sint32 GetMaxEnemyBombardSea  (const size_t & player) const { return GetMaxEnemyGrid(m_bombardSeaGrid,  player); };
	sint32 GetMaxEnemyBombardAir  (const size_t & player) const { return GetMaxEnemyGrid(m_bombardAirGrid,  player); };
	sint32 GetMaxEnemyValue       (const size_t & player) const { return GetMaxEnemyGrid(m_valueGrid,       player); };
	sint32 GetMaxEnemyCityValue   (const size_t & player) const { return GetMaxEnemyGrid(m_cityValueGrid,   player); };

	//     The allied grids
	sint32    GetAlliedGrid       (const MapGridVector & gridVector, const size_t & player, const MapPoint & pos) const;
	sint32 GetMaxAlliedGrid       (const MapGridVector & gridVector, const size_t & player) const;

	sint32 GetPower               (const size_t & player, const MapPoint & pos) const { return GetAlliedGrid(m_threatGrid,      player, pos); };
	sint32 GetAlliedAttack        (const size_t & player, const MapPoint & pos) const { return GetAlliedGrid(m_attackGrid,      player, pos); };
	sint32 GetAlliedDefense       (const size_t & player, const MapPoint & pos) const { return GetAlliedGrid(m_defenseGrid,     player, pos); };
	sint32 GetAlliedRanged        (const size_t & player, const MapPoint & pos) const { return GetAlliedGrid(m_rangedGrid,      player, pos); };
	sint32 GetAlliedBombardLand   (const size_t & player, const MapPoint & pos) const { return GetAlliedGrid(m_bombardLandGrid, player, pos); };
	sint32 GetAlliedBombardSea    (const size_t & player, const MapPoint & pos) const { return GetAlliedGrid(m_bombardSeaGrid,  player, pos); };
	sint32 GetAlliedBombardAir    (const size_t & player, const MapPoint & pos) const { return GetAlliedGrid(m_bombardAirGrid,  player, pos); };
	sint32 GetAlliedCityValue     (const size_t & player, const MapPoint & pos) const { return GetAlliedGrid(m_cityValueGrid,   player, pos); };
	sint32 GetAlliedValue         (const size_t & player, const MapPoint & pos) const { return GetAlliedGrid(m_valueGrid,       player, pos); };

	sint32 GetMaxPower            (const size_t & player) const { return GetMaxAlliedGrid(m_threatGrid,      player); };
	sint32 GetMaxAlliedAttack     (const size_t & player) const { return GetMaxAlliedGrid(m_attackGrid,      player); };
	sint32 GetMaxAlliedDefense    (const size_t & player) const { return GetMaxAlliedGrid(m_defenseGrid,     player); };
	sint32 GetMaxAlliedRanged     (const size_t & player) const { return GetMaxAlliedGrid(m_rangedGrid,      player); };
	sint32 GetMaxAlliedBombardLand(const size_t & player) const { return GetMaxAlliedGrid(m_bombardLandGrid, player); };
	sint32 GetMaxAlliedBombardSea (const size_t & player) const { return GetMaxAlliedGrid(m_bombardSeaGrid,  player); };
	sint32 GetMaxAlliedBombardAir (const size_t & player) const { return GetMaxAlliedGrid(m_bombardAirGrid,  player); };
	sint32 GetMaxAlliedCityValue  (const size_t & player) const { return GetMaxAlliedGrid(m_cityValueGrid,   player); };
	sint32 GetMaxAlliedValue      (const size_t & player) const { return GetMaxAlliedGrid(m_valueGrid,       player); };

	void DebugLog() const;

	double GetProductionRank(const CityData *city) const;
	double GetGrowthRank    (const CityData *city) const;
	double GetCommerceRank  (const CityData *city) const;
	double GetHappinessRank (const CityData *city) const;
	double GetThreatRank    (const CityData *city) const;
	double GetPowerRank     (const CityData *city) const;

	const MapPoint & GetEmpireCenter(const PLAYER_INDEX player) const;

	const MapPoint & GetNearestForeigner(const PLAYER_INDEX player, const MapPoint & pos) const;

	sint32 TotalThreat(const PLAYER_INDEX & playerId) const
	{
		return static_cast<sint32>(m_threatGrid[playerId].GetTotalValue());
	}

	sint32 TotalValue(const PLAYER_INDEX & playerId) const
	{
		return static_cast<sint32>(m_valueGrid[playerId].GetTotalValue());
	}

	sint32 TotalCityValue(const PLAYER_INDEX & playerId) const
	{
		return static_cast<sint32>(m_cityValueGrid[playerId].GetTotalValue());
	}

	sint32 GetTotalTrade(const PLAYER_INDEX & playerId) const
	{
		if (m_totalTrade.size() <= 0)
			return 0;
		return m_totalTrade[playerId];
	}

	sint32 GetProjectedScience(const PLAYER_INDEX & playerId) const
	{
		return m_projectedScience[playerId];
	}

	sint32 GetTradeAtRisk(const PLAYER_INDEX & playerId, const MapPoint & pos ) const
	{
		return static_cast<sint32>(m_tradeAtRiskGrid[playerId].GetGridValue(pos) / 1000);
	}

	sint32 TotalTradeAtRisk(const PLAYER_INDEX & playerId) const
	{
		return static_cast<sint32>(m_tradeAtRiskGrid[playerId].GetTotalValue() / 1000);
	}

	sint32 GetMaxTradeAtRisk(const PLAYER_INDEX & player) const
	{
		return static_cast<sint32>(m_tradeAtRiskGrid[player].GetMaxGridValue() / 1000);
	}

	sint32 GetPiracyLoss(const PLAYER_INDEX & playerId, const MapPoint & pos ) const
	{
		return static_cast<sint32>(m_piracyLossGrid[playerId].GetGridValue(pos));
	}

	sint32 TotalPiracyLoss(const PLAYER_INDEX & playerId) const
	{
		return static_cast<sint32>(m_piracyLossGrid[playerId].GetTotalValue());
	}

	sint32 GetMaxPiracyLoss(const PLAYER_INDEX & player) const
	{
		return static_cast<sint32>(m_piracyLossGrid[player].GetMaxGridValue());
	}

	double CityAtRiskRatio(const Unit city, const PLAYER_INDEX opponentId) const;

	sint32 MostAtRiskCity(const PLAYER_INDEX & playerId,
	                      Unit & city,
	                      const PLAYER_INDEX & opponentId) const;

	sint32 AtRiskCitiesValue(const PLAYER_INDEX & playerId,
	                         const PLAYER_INDEX & opponentId = -1) const;

	sint32 GetPiracyIncomeByPlayer(const PLAYER_INDEX playerId,
	                               const PLAYER_INDEX victimId) const;

	bool ShareContinent(const PLAYER_INDEX playerId,
	                    const PLAYER_INDEX foreignerId) const;

	bool PlayerCanEnter(const PLAYER_INDEX playerId,
	                    const MapPoint & pos ) const;

	sint16 GetNuclearWeaponsCount(const PLAYER_INDEX playerId) const;
	sint16 GetNanoWeaponsCount(const PLAYER_INDEX playerId) const;
	sint16 GetBioWeaponsCount(const PLAYER_INDEX playerId) const;

	void SetNuclearWeaponsCount(const PLAYER_INDEX playerId, const sint32 value);
	void SetBioWeaponsCount(const PLAYER_INDEX playerId, const sint32 value);
	void SetNanoWeaponsCount(const PLAYER_INDEX playerId, const sint32 value);

	sint16 GetSpecialAttackers(const PLAYER_INDEX playerId) const;

	sint32 AverageSettledContinentSize(const PLAYER_INDEX playerId) const;

	sint16 GetTotalPopulation(const PLAYER_INDEX playerId = -1) const;

	sint16 GetLandArea(const PLAYER_INDEX playerId = -1) const;

	double GetPopulationPercent(const PLAYER_INDEX playerId) const;

	double GetLandPercent(const PLAYER_INDEX playerId) const;

	void ComputeAllianceSize(const PLAYER_INDEX playerId, PLAYER_INDEX & leaderId, double & population, double & land) const;

	double GetProductionHandicapRatio(const PLAYER_INDEX playerId) const;

	double GetGoldHandicapRatio(const PLAYER_INDEX playerId) const;

	double GetScienceHandicapRatio(const PLAYER_INDEX playerId) const;

	void Cleanup();

	void CalcEmpireCenter(const size_t playerId);

private:
	static MapAnalysis s_mapAnalysis;

	void AddPiracyIncome( const size_t playerId,
	                      const size_t victimId,
	                      const sint16 route_value );

	void ComputeHandicapRatios();

	MapAnalysis();

	MapGridVector m_threatGrid;
	MapGridVector m_attackGrid;
	MapGridVector m_defenseGrid;
	MapGridVector m_rangedGrid;
	MapGridVector m_bombardLandGrid;
	MapGridVector m_bombardSeaGrid;
	MapGridVector m_bombardAirGrid;
	MapGridVector m_cityValueGrid;
	MapGridVector m_valueGrid;

	Sint32Vector m_totalTrade;

	Sint32Vector m_projectedScience;

	MapGridVector m_tradeAtRiskGrid;

	MapGridVector m_piracyLossGrid;

	Sint16Vector m_piracyIncomeMatrix;

	sint32 m_minCityProduction;
	sint32 m_minCityGrowth;
	sint32 m_minCityGold;
	sint32 m_minCityHappiness;

	sint32 m_maxCityProduction;
	sint32 m_maxCityGrowth;
	sint32 m_maxCityGold;
	sint32 m_maxCityHappiness;

	// Should be replaced by simple integers, too
	Sint32Vector m_minCityThreat;
	Sint32Vector m_maxCityThreat;

	MapPointVector m_empireCenter;

	Bit_Table m_cityOnContinent;
	Bit_Table m_armyOnContinent;

	Uint32Vector m_movementTypeUnion;

	Sint16Vector m_nuclearWeapons;
	Sint16Vector m_bioWeapons;
	Sint16Vector m_nanoWeapons;
	Sint16Vector m_specialAttackers;

	sint16 m_worldPopulation;
	Sint16Vector m_totalPopulation;
	Sint16Vector m_landArea;

	Sint32Vector m_continentSize;

	DoubleVector m_productionHandicapRatio;
	DoubleVector m_goldHandicapRatio;
	DoubleVector m_scienceHandicapRatio;
};

#endif // __MAP_ANALYSIS_H__
