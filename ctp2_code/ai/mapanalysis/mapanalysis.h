













#pragma once
#ifndef __MAP_ANALYSIS_H__
#define __MAP_ANALYSIS_H__

#include "MapGrid.h"
#include "CityData.h"
#include "BoundingRect.h"
#include "Bit_Table.h"
#include <vector>

#ifdef _DEBUG
	typedef std::vector<MapGrid<sint32>, dbgallocator< MapGrid<sint32> > > MapGridVector;
	typedef std::vector<BoundingRect, dbgallocator<BoundingRect> > BoundingRectVector;
	typedef std::vector<sint32, dbgallocator<sint32> > Sint32Vector;
	typedef std::vector<sint16, dbgallocator<sint16> > Sint16Vector;
	typedef std::vector<uint32, dbgallocator<sint16> > Uint32Vector;
	typedef std::vector<MapPoint, dbgallocator<MapPoint> > MapPointVector;
	typedef std::vector<double, dbgallocator<double> > DoubleVector;
#else
	typedef std::vector<MapGrid<sint32> > MapGridVector;
	typedef std::vector<BoundingRect> BoundingRectVector;
	typedef std::vector<sint32> Sint32Vector;
	typedef std::vector<sint16> Sint16Vector;
	typedef std::vector<uint32> Uint32Vector;
	typedef std::vector<MapPoint> MapPointVector;
	typedef std::vector<double> DoubleVector;
#endif

class MapAnalysis {

public:

	
	static MapAnalysis & GetMapAnalysis();

	
	void Resize( const PLAYER_INDEX & maxPlayerId,
				 const sint16 & xSize, 
				 const sint16 & ySize, 
				 const sint16 & resolution );

	
	void ResizeContinents();

	
	void BeginTurn();

	
	sint32 GetThreat(const PLAYER_INDEX & player, const MapPoint & pos ) const;

	
	sint32 GetMaxThreat(const PLAYER_INDEX & player) const;

	
	sint32 GetPower(const PLAYER_INDEX & player, const MapPoint & pos ) const;

	
	sint32 GetMaxPower(const PLAYER_INDEX & player) const;

	
	sint32 GetAlliedValue(const PLAYER_INDEX & player, const MapPoint & pos) const;

	
	sint32 GetMaxAlliedValue(const PLAYER_INDEX & player) const;
	
	
	sint32 GetEnemyValue(const PLAYER_INDEX & player, const MapPoint & pos) const;

	
	sint32 GetMaxEnemyValue(const PLAYER_INDEX & player) const;

	
	void DebugLog() const;

	
	double GetProductionRank(const CityData *city, const bool & all_players) const;
	double GetGrowthRank(const CityData *city, const bool & all_players) const;
	double GetCommerceRank(const CityData *city, const bool & all_players) const;
	double GetHappinessRank(const CityData *city) const;
	double GetThreatRank(const CityData *city) const;
	double GetPowerRank(const CityData *city) const;

	
	void UpdateBoundingRectangle(const Army & army);

	
	void UpdateBoundingRectangle(const Unit & city);

	
	const BoundingRect & GetBoundingRectangle(const PLAYER_INDEX & player) const;

	
	const MapPoint & GetEmpireCenter(const PLAYER_INDEX player) const;

	
	const MapPoint & GetNearestForeigner(const PLAYER_INDEX player, const MapPoint & pos) const;

	sint32 TotalThreat(const PLAYER_INDEX & playerId) const {
		return m_threatGrid[playerId].GetTotalValue();
	}

	sint32 TotalValue(const PLAYER_INDEX & playerId) const {
		return m_valueGrid[playerId].GetTotalValue();
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

	
	sint16 GetTradeAtRisk(const PLAYER_INDEX & playerId, const MapPoint & pos ) const {
		
		return (sint32) (m_tradeAtRiskGrid[playerId].GetGridValue(pos) / 1000);
	}

	
	sint32 TotalTradeAtRisk(const PLAYER_INDEX & playerId) const {
		
		return (sint32) (m_tradeAtRiskGrid[playerId].GetTotalValue() / 1000);
	}

	
	sint16 GetMaxTradeAtRisk(const PLAYER_INDEX & player) const {
		
		return (sint32) (m_tradeAtRiskGrid[player].GetMaxGridValue() / 1000);
	}

	
	sint16 GetPiracyLoss(const PLAYER_INDEX & playerId, const MapPoint & pos ) const {
		return m_piracyLossGrid[playerId].GetGridValue(pos);
	}

	
	sint32 TotalPiracyLoss(const PLAYER_INDEX & playerId) const {
		return m_piracyLossGrid[playerId].GetTotalValue();
	}

	
	sint16 GetMaxPiracyLoss(const PLAYER_INDEX & player) const {
		return m_piracyLossGrid[player].GetMaxGridValue();
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

	
	sint16 AverageSettledContinentSize(const PLAYER_INDEX playerId) const;

	
	sint16 GetTotalPopulation(const PLAYER_INDEX playerId = -1) const;

	
	sint16 GetLandArea(const PLAYER_INDEX playerId = -1) const;

	
	double GetPopulationPercent(const PLAYER_INDEX playerId) const;

	
	double GetLandPercent(const PLAYER_INDEX playerId) const;

	
	void ComputeAllianceSize(const PLAYER_INDEX playerId, PLAYER_INDEX & leaderId, double & population, double & land) const;

	
	double GetProductionHandicapRatio(const PLAYER_INDEX playerId) const;

	
	double GetGoldHandicapRatio(const PLAYER_INDEX playerId) const;

	
	double GetScienceHandicapRatio(const PLAYER_INDEX playerId) const;

private:
	static MapAnalysis s_mapAnalysis;

	
	void AddPiracyIncome( const PLAYER_INDEX playerId, 
						  const PLAYER_INDEX victimId, 
						  const sint16 route_value );

	
	void ComputeHandicapRatios();

	
	MapAnalysis();

	
	MapGridVector m_threatGrid;

	
	MapGridVector m_valueGrid;

	
	Sint32Vector m_totalTrade;

	
	Sint32Vector m_projectedScience;

	
	MapGridVector m_tradeAtRiskGrid;

	
	MapGridVector m_piracyLossGrid;

	
	Sint16Vector m_piracyIncomeMatrix;

	
	Sint32Vector m_minCityProduction;
	Sint32Vector m_maxCityProduction;
	sint32 m_minCityProductionAll;
	sint32 m_maxCityProductionAll;

	Sint32Vector m_minCityGrowth;
	Sint32Vector m_maxCityGrowth;
	sint32 m_minCityGrowthAll;
	sint32 m_maxCityGrowthAll;
	
	Sint32Vector m_minCityGold;
	Sint32Vector m_maxCityGold;
	sint32 m_minCityGoldAll;
	sint32 m_maxCityGoldAll;

	Sint32Vector m_minCityHappiness;
	Sint32Vector m_maxCityHappiness;

	Sint32Vector m_minCityThreat;
	Sint32Vector m_maxCityThreat;

	
	BoundingRectVector m_empireBoundingRect;
	
	
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

#endif __MAP_ANALYSIS_H__
