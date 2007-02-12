//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// CTP1_TRADE
// - Creates an executable with trade like in CTP1. Currently broken.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragma once commented out
// - Enable reading of files created with the Activision 1.1 patch.
// - Prevent crash when settling in the Alexander scenario.
// - Added GetUtilisationRatio function.
// - Prevented cities to revolt twice in the same turn. By kaan.
// - Added NeedMoreFood function to calculate whether a city should
//   build food tile improvements, by Martin Gühmann.
// - Removed some unused functions and added a lot new ones to be able to
//   estimate resource production better without the need of recalculation
//   of everything. - April 4th 2005 Martin Gühmann
// - Added m_turnsNextPop to track city growth - PFT 29 mar 05
// - Removed private member m_turnsNextPop as there is no need to save it
//   as its calculation is cheap and in addition there was no get method
//   or anything else that requires to save it. - April 23rd 2005 Martin Gühmann
// - Replaced some member names for clarity. - Aug 6th 2005 Martin Gühmann
// - Removed a bunch of unused and incomplete methods. - Aug 6th 2005 Martin Gühmann
// - Added new code as preparation for resource calculation redesign.- Aug 6th 2005 Martin Gühmann
// - Added code for new city resource calculation. (Aug 12th 2005 Martin Gühmann)
// - Removed CITY_TILE_SIZE and k_CITY_RADIUS they aren't used. (Aug 12th 2005 Martin Gühmann)
// - Removed more unused methods. (Aug 12th 2005 Martin Gühmann)
// - Added city style specific happiness bonus method. (Oct 7th 2005 Martin Gühmann)
// - Added HasNeededGood method by E
// - Added HasEitherGood method by E
// - Added HasTileImpInRadius method by E 4-7-2006 bool if a city has imp in radius
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __CITY_DATA_H__
#define __CITY_DATA_H__ 1

class CityData;

#include "Unit.h"

#include "BldQue.h"
#include "HappyTracker.h"           // HAPPY_REASON


#include "UnitDynArr.h"
#include "TradeDynArr.h"
#include "Resources.h"
#include "GWRecord.h"

#include "CityRadius.h"

#include "MapPoint.h"

#include "CitySizeRecord.h"

#define k_CITYDATA_VERSION_MAJOR    0
#define k_CITYDATA_VERSION_MINOR    0

class CivArchive;
class Happy;
class SlicObject;
class Cell;


#define k_PEOPLE_PER_POPULATION 10000
//#define NEW_RESOURCE_PROCESS 1


struct TerrainValue;

class CityData : public CityRadiusCallback 
{
private:

//----------------------------------------------------------------------------
// Do not change anything in the types or order of the following variable 
// declarations. Doing so will break reading in of save files.
// See the Serialize implementation for more details.
//----------------------------------------------------------------------------

	PLAYER_INDEX m_owner;
	uint32 m_slaveBits;
	sint32 m_accumulated_food;
	sint32 m_shieldstore;
	sint32 m_shieldstore_at_begin_turn;
	sint32 m_build_category_at_begin_turn;
	sint32 m_net_gold;
	sint32 m_gold_lost_to_crime;
	sint32 m_gross_gold;
	sint32 m_goldFromTradeRoutes;
	sint32 m_goldLostToPiracy;
	sint32 m_science;
	sint32 m_luxury;
	CITY_ATTITUDE m_city_attitude;
	
	sint32 m_collected_production_this_turn; // Will be unused
	sint32 m_gross_production;
	sint32 m_net_production;
	sint32 m_production_lost_to_crime;

	uint64 m_built_improvements;
	uint64 m_builtWonders;

	double m_food_delta;
	double m_gross_food;
	double m_net_food;
	double m_food_lost_to_crime;
	double m_food_consumed_this_turn;

	
	sint32 m_total_pollution;
	sint32 m_cityPopulationPollution;
	sint32 m_cityIndustrialPollution;
	sint32 m_foodVatPollution;
	sint32 m_cityPollutionCleaner;
	BOOL m_contribute_materials;
	BOOL m_contribute_military;

	
	
	BOOL m_capturedThisTurn;

	sint32 m_spied_upon;
	BOOL m_walls_nullified;
	sint32 m_franchise_owner;
	sint32 m_franchiseTurnsRemaining;
	sint32 m_watchfulTurns;
	sint32 m_bioInfectionTurns;
	sint32 m_bioInfectedBy;
	sint32 m_nanoInfectionTurns;
	sint32 m_nanoInfectedBy;
	sint32 m_convertedTo;
	sint32 m_convertedGold;
	CONVERTED_BY m_convertedBy;
	BOOL	m_terrainWasPolluted,
			m_happinessAttacked,
			m_terrainImprovementWasBuilt,
			m_improvementWasBuilt;

	BOOL m_isInjoined;
	sint32 m_injoinedBy;


	sint32 m_airportLastUsed;

	
	sint32 m_founder;

	sint32 m_wages_paid;

	sint32 m_pw_from_infrastructure;
	sint32 m_gold_from_capitalization;
	BOOL m_buildInfrastructure;
	BOOL m_buildCapitalization;

	BOOL m_paidForBuyFront;

	UPRISING_CAUSE m_doUprising;

	sint32 m_turnFounded;

	sint32 m_productionLostToFranchise;

	bool m_probeRecoveredHere;

	sint16 m_lastCelebrationMsg;
	uint8 m_alreadySoldABuilding;

	
	
	
	
	sint32 m_population;
	sint32 m_partialPopulation;
	sint16 m_numSpecialists[POP_MAX];
	sint32 m_specialistDBIndex[POP_MAX];
	sint32 m_sizeIndex;
	sint32 m_workerFullUtilizationIndex;
	sint32 m_workerPartialUtilizationIndex;

	
	BOOL m_useGovernor;
	sint32 m_buildListSequenceIndex;
	sint32 m_garrisonOtherCities;
	BOOL m_garrisonComplete;

	
	sint8 m_currentGarrison;
	sint8 m_neededGarrison;
	double m_currentGarrisonStrength;
	double m_neededGarrisonStrength;

	
	sint32 m_sellBuilding;
	bool m_buyFront;

	sint32 m_max_food_from_terrain;
	sint32 m_max_prod_from_terrain;
	sint32 m_max_gold_from_terrain;

	sint32 m_growth_rate;
	double m_overcrowdingCoeff;

	sint32 m_starvation_turns;

	
	sint32 m_cityStyle;

	MapPoint m_pos;

	
	BOOL m_is_rioting;

//----------------------------------------------------------------------------
// Changing the order below this line should not break anything.
//----------------------------------------------------------------------------
	
	Unit m_home_city;
	uint8 m_min_turns_revolt; // Number of revolt risk free turns.
	BuildQueue m_build_queue;
	
	TradeDynamicArray m_tradeSourceList;
	TradeDynamicArray m_tradeDestinationList;
#ifdef CTP1_TRADE
	Resources m_resources;
	Resources m_localResources;
#else
	Resources m_collectingResources;
	Resources m_sellingResources;
	Resources m_buyingResources;
#endif
	Happy     *m_happy;
	
	MBCHAR    m_name[k_MAX_NAME_LEN] ;

	sint32    *m_distanceToGood;

	
	double    m_defensiveBonus;
	
	sint32    *m_ringFood;
	sint32    *m_ringProd;
	sint32    *m_ringGold;
	sint32    *m_ringSizes;
	
#if defined(NEW_RESOURCE_PROCESS)
	double    *m_farmersEff;
	double    *m_laborersEff;
	double    *m_merchantsEff;
	double    *m_scientistsEff;

	double    m_max_processed_terrain_food;
	double    m_max_processed_terrain_prod;
	double    m_max_processed_terrain_gold;
	double    m_max_processed_terrain_scie;

	double    m_grossFoodCrimeLoss;
	double    m_grossProdCrimeLoss;
	double    m_grossGoldCrimeLoss;
	double    m_grossScieCrimeLoss;

	double    m_grossProdBioinfectionLoss;
	double    m_grossProdFranchiseLoss;
	double    m_grossGoldConversionLoss;

	double    m_foodFromOnePop;
	double    m_prodFromOnePop;
	double    m_goldFromOnePop;
	double    m_scieFromOnePop;

	double    m_crimeFoodLossOfOnePop;
	double    m_crimeProdLossOfOnePop;
	double    m_crimeGoldLossOfOnePop;
	double    m_crimeScieLossOfOnePop;

	double    m_bioinfectionProdLossOfOnePop;
	double    m_franchiseProdLossOfOnePop;
	double    m_conversionGoldLossOfOnePop;

	sint32    m_productionLostToBioinfection;
	sint32    m_max_scie_from_terrain;
	double    m_gross_science;
	double    m_science_lost_to_crime;
#endif

	RADIUS_OP m_cityRadiusOp;
	UnitDynamicArray *m_killList;
	sint32    m_radiusNewOwner;
	sint32    m_tilecount, m_whichtile;
	SlicObject *m_tempGoodAdder;
	sint32    m_tempGood, m_tempGoodCount;
	bool      m_sentInefficientMessageAlready;
	
	

	
	
#ifdef _DEBUG
	bool m_ignore_happiness; 
#endif

	friend class NetPop;
	friend class NetCity;
	friend class NetCity2;
	friend class NetCityName;
	friend class NetCityResources;
	friend class World;
	friend class Happy; 
	friend class NetHappy;
	friend class NetCityBuildQueue;
	friend class C3Player; 
	friend class C3Population; 

#ifdef _PLAYTEST
	friend class CreateImprovementCommand;
#endif

public:

	CityData(CivArchive &archive);
	CityData(PLAYER_INDEX o, Unit hc, const MapPoint &center_pos);
	CityData(CityData *copy);
	virtual ~CityData();

	void Copy(CityData *copy);
	void Serialize(CivArchive &archive);

	void Initialize(sint32 settlerType);
	void NetworkInitialize();

	void PrepareToRemove(const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy);

	uint64 GetImprovements() const { return m_built_improvements; };
	Unit GetHomeCity() const { return m_home_city; };
	sint32 GetOwner() const { return m_owner; }
	void ResetCityOwner(sint32 owner);

	sint32 GetStoredCityFood() const { return m_accumulated_food; }
	sint32 GetNetCityFood() const { return sint32(m_food_delta); }
	sint32 GetGrossCityFood() const { return sint32(m_gross_food); }

	
	sint32 GetStarvationTurns() const {return m_starvation_turns;}

	double GetProducedFood() const { return m_net_food; }
	double GetConsumedFood() const { return m_food_consumed_this_turn; }

	
	void SetShieldstore (sint32 s);
	void AddShields(sint32 s);

#if !defined(NEW_RESOURCE_PROCESS)
	sint32 ComputeGrossProduction( double workday_per_person, sint32 collected_production, sint32 & crime_loss, sint32 & franchise_loss, bool considerOnlyFromTerrain = false ) const;
	sint32 ProcessProduction(bool projectedOnly, sint32 &grossProduction, sint32 &collectedProduction, sint32 &crimeLoss, sint32 &franchiseLoss, bool considerOnlyFromTerrain = false) const;
	sint32 ProcessProduction(bool projectedOnly);
#endif
	
	double ProjectMilitaryContribution();
	sint32 GetStoredCityProduction() const { return m_shieldstore; }
	sint32 GetNetCityProduction() const { return m_net_production; }
	sint32 GetGrossCityProduction() const { return m_gross_production; }

	
	sint32 ComputeMaterialsPaid(double percent_terrain);

	void PayFederalProduction(double percent_military,
	                          sint32 &mil_paid, 
	                          double percent_terrain, 
	                          sint32 &mat_paid);
	void PayFederalProductionAbs(sint32 mil_paid, 
	                             double percent_mat, 
	                             sint32 &mat_paid);

	void ShieldSupportTroops();
	void AddShieldsToBuilding();

	void SetMaterialContribution(bool on) { m_contribute_materials = on;}
	bool GetMaterialContribution() const { return m_contribute_materials != FALSE;}

	void SetMilitaryContribution(bool on) { m_contribute_military = on; }
	bool GetMilitaryContribution() { return m_contribute_military != FALSE; }

	
	void IncrementBuildQueue();
	sint32 LoadQueue(const MBCHAR *file);
	sint32 SaveQueue(const MBCHAR *file);

	
	BuildQueue *GetBuildQueue(void) { return &m_build_queue; }



	void   CollectResources();

#if !defined(NEW_RESOURCE_PROCESS)
	sint32 ProcessFood();
	void   ProcessFood(double &foodLostToCrime, double &producedFood, double &grossFood, bool considerOnlyFromTerrain = false) const;
	double ProcessFinalFood(double &foodLossToCrime, double &grossFood) const;
#endif
	void   EatFood();
	bool   FoodSupportTroops();

	sint32 GetBuildingOvercrowdingBonus();
	sint32 GetBuildingMaxPopIncrease();
	void   CalculateGrowthRate();
	double CalculateGrossGrowthRate(double &overcrowdingCoeff, double &baseRate, sint32 bonusFood = 0);
	bool   GrowOrStarve();
	double GetFoodRequired() const;
	double GetFoodRequired(sint32 popCount) const;
	double GetFoodRequiredPerCitizen() const;
	bool   NeedMoreFood(sint32 foodBonus, sint32 &foodMissing, bool considerOnlyFromTerrain = false);
	sint32 HowMuchMoreFoodNeeded(sint32 bonusFood = 0, bool onlyGrwoth = true, bool considerOnlyFromTerrain = false);

	sint32 GetAccumulatedFood() { return m_accumulated_food; }
	sint32 SubtractAccumulatedFood(sint32 amount);



	void SetTerrainEmptyRow(const sint32 row_len, sint32 &n, DynamicArray<TerrainValue> &val);
	void GotoRowStart(const MapPoint &center, const WORLD_DIRECTION d1, 
	                  const WORLD_DIRECTION d2, const sint32 row_len, 
	                  sint32 &n, DynamicArray<TerrainValue> &val);
	void GetOpenTerrainValuesRow(sint32 len, MapPoint &pos, sint32 &n, DynamicArray<TerrainValue> &val);
	void GetOpenTerrainValues(const MapPoint &center, sint32 &n, DynamicArray<TerrainValue> &val);
	bool CityCanHavePopAt(MapPoint &pos) const;

	void UpdateSprite(void);
	
	bool IsWatchful() const;
	void SetWatchful();
	void ModifySpecialAttackChance(UNIT_ORDER_TYPE attack, double &chance);

	void RemoveOneSlave(PLAYER_INDEX p);
	bool AdjustedBestTile(const double foodCoef, const double productionCoef, const double resourceCoef, MapPoint &bestPos);

#if !defined(NEW_RESOURCE_PROCESS)
	void CollectOtherTrade(const bool projectedOnly, bool changeResources = true); // changeResources check must be implemented
#endif
	void CheckTopTen();
	sint32 SupportBuildings(bool projectedOnly);
	sint32 GetSupportBuildingsCost() const;

	void AddTradeRoute(TradeRoute &route, bool fromNetwork);
	void DelTradeRoute(TradeRoute route);
	sint32 IsUsedInTradeRoute(const MapPoint &qpos);
	sint32 CalculateTradeRoutes(bool projectedOnly);
	sint32 CalculateGoldFromResources();
	void AddTradeResource(ROUTE_TYPE type, sint32 resource);
#ifdef CTP1_TRADE
	sint32 GetResourceCount(sint32 resource);
	sint32 GetLocalResourceCount(sint32 resource);
#endif

	sint32 CalcWages(sint32 wage) const;
	sint32 GetWagesNeeded(const sint32 & wages_per_person) const;
	sint32 GetWagesNeeded();

	sint32 GetGoldFromTradeRoutes() const { return m_goldFromTradeRoutes; }
	sint32 GetGoldLostToPiracy() const { return m_goldLostToPiracy; }
	TradeDynamicArray* GetTradeSourceList() { return (&m_tradeSourceList); }
	TradeDynamicArray* GetTradeDestinationList() { return (&m_tradeDestinationList); }

	bool BreakOneSourceRoute(ROUTE_TYPE type, sint32 resource);

#if !defined(NEW_RESOURCE_PROCESS)
	void CollectGold(sint32 &gold, sint32 &convertedGold, sint32 &crimeLost, bool considerOnlyFromTerrain = false) const;
	void ProcessGold(sint32 &gold, bool considerOnlyFromTerrain = false) const;
	void ApplyGoldCoeff(sint32 &gold) const;
	void CalcGoldLoss(const bool projectedOnly, sint32 &gold, sint32 &convertedGold, sint32 &crimeLost) const;
#endif

	sint32 GetNetCityGold() const { return m_net_gold; } 
	sint32 GetGrossCityGold() const { return m_gross_gold; } 



	void ResetConquestDistress(double new_distress);


	void ImprovementHealUnitsInCity() const;
	void ImprovementRefuelUnitsInCity() const;
	bool ImprovementCanRefuel(const Unit &u) const;
	void AddRawMaterials(sint32 amount);
	sint32 GetRawMaterials() const;
	void UseRawMaterials(sint32 amt);
	bool IsConnected(MapPoint &point, uint8* array = NULL, sint32 w = 0, sint32 h = 0);



	void CalcPollution(void) ;
	void DoLocalPollution();





	void CheatBuildFirstItem();

	void InitBeginTurnVariables();
	void DoTurnCounters();
	void TryToBuild();
	bool BeginTurn(); 
	void EndTurn();

	sint32 GetNumPop() const; 






	bool BuildUnit(sint32 type);
	bool BuildImprovement(sint32 type);
	bool BuildWonder(sint32 type);
	bool BuildEndGame(sint32 type);

	void AddWonder(sint32 type);
	bool ChangeCurrentlyBuildingItem(sint32 category, sint32 item_type);

	uint64 GetBuiltWonders() const { return m_builtWonders; }

	void DestroyCapitol(); 
	void SetCapitol();
	void DestroyImprovement(sint32 type);
	void NewGovernment(sint32 government_type);

	double GetDefendersBonus() const;
	double GetDefendersBonusNoWalls() const;

	// Modified by kaan to address bug # 12

	void NoRevoltCountdown();

	bool ShouldRevolt(const sint32 inciteBonus);
	void Revolt(sint32 &playerToJoin, bool causeIsExternal = false);
	void TeleportUnits(const MapPoint &pos,  bool &revealed_foreign_units, 
	                         bool &revealed_unexplored, sint32 foreigner);
	void StopTradingWith(const PLAYER_INDEX bannedRecipient);
	Happy *GetHappy(void) { return (m_happy); }
	
	MBCHAR *GetName(void) { return (m_name); }
	void SetName(const MBCHAR *name);

	void GetPop(sint32 &p)const { p = m_population; }

	void GetConsumptionStats(sint32 &foodConsumed) const {
		foodConsumed = (sint32)(m_food_consumed_this_turn);
	}

	
	sint32 GetTradeCrime() const { return m_gold_lost_to_crime; }
	sint32 GetProdCrime() const { return m_production_lost_to_crime; }
	void GetFoodCrime( sint32 &foodCrime ) const {
		foodCrime = (sint32)(m_food_lost_to_crime);
	}

	bool GetIsRioting(void) const { return m_is_rioting != FALSE; }

	void CalcHappiness(sint32 &virtualGoldSpent, bool isFirstPass);
	void CheckRiot();
	double GetHappiness() const;
	double GetHappySize() const;
	double GetHappyPollution() const;
	double GetHappyConquestDistress() const;
	double GetHappyEmpireDist() const;
	double GetHappyEnemyAction() const;
	double GetHappyPeace() const;
	double GetHappyWorkday() const;
	double GetHappyWages() const;
	double GetHappyRations() const;
	double GetHappyMartialLaw() const;
	double GetHappyPopEntertainment() const;
	double GetHappyImprovement() const;
	double GetHappyWonders() const;
	double GetHappyCrime() const;
	bool IsCelebratingHappiness(void) const;

	double GetImprovementCrimeMod() const;
	sint32 GetImprovementPeaceMod() const;
	void AddHappyTimer(sint32 turns, double adjust, HAPPY_REASON reason);
	sint32 GetPollution() const;
	sint32 GetPopulationPollution() const;
	sint32 GetProductionPollution() const;
	void CityToPark(sint32 agressor);

	void CityRadiusFunc(const MapPoint &pos);
	void GetNuked(UnitDynamicArray &killList);

	bool SafeFromNukes() const;

	bool HasAirport() const;
	void UseAirport();
	sint32 AirportLastUsed() const;
	bool HasCityWalls() const;
	bool HasForceField() const;

	bool HasBeenSpiedUpon() const;
	void SetSpiedUpon();

	void CityNullifyWalls();
	bool IsCapitol() const;

	void MakeFranchise(sint32 player);
	sint32 GetFranchiseOwner() const { return m_franchise_owner;}
	sint32 GetFranchiseTurnsRemaining() const;
	void SetFranchiseTurnsRemaining(sint32 turns);

#ifdef _DEBUG
	void SetIgnoreHappiness(bool v) { m_ignore_happiness = v; }
#endif


	void DoUprising(UPRISING_CAUSE cause);
	void FinishUprising(Army &sa, UPRISING_CAUSE cause);
	void CleanupUprising(Army &sa);

	void Plague(sint32 player);
	void BioInfect( sint32 player );
	void NanoInfect( sint32 player );
	bool IsBioImmune() const;
	bool IsNanoImmune() const;
	void SpreadBioTerror();
	void SpreadNanoTerror();

	bool IsNanoInfected() const { return m_nanoInfectionTurns > 0; }
	bool IsBioInfected() const { return m_bioInfectionTurns > 0; }
	bool IsFranchised() const { return m_franchise_owner >= 0; }
	bool IsConverted() const { return m_convertedTo >= 0; }
	sint32 GetConvertedTo(void) const { return (m_convertedTo) ; }
	sint32 GetConvertedGold(void) const { return (m_convertedGold) ; }

	void ConvertTo(sint32 player, CONVERTED_BY by);
	double TheologicalModifier() const;
	void Unconvert(bool makeUnhappy = true);
	sint32 IsConvertedTo() const { return m_convertedTo; }

	bool HasResource(sint32 resource) const;
	// Added by E for resources Oct 2005
	bool HasNeededGood(sint32 resource) const;
	void AddGoodToCity(sint32 good);           //EMOD to add goods outside of citydata 6.6.2006
#ifdef CTP1_TRADE
	const Resources *GetResources() const { return &m_resources; }
	const Resources *GetLocalResources() const { return &m_localResources; }
#else
	const Resources *GetCollectingResources() const { return &m_collectingResources; }
	const Resources *GetBuyingResources() const { return &m_buyingResources; }
#endif

	bool IsLocalResource( sint32 resource ) const;
	bool HasTileImpInRadius(sint32 tileimp, MapPoint const & cityPos) const; //EMOD 4-7-2006 bool if a city has imp in radius

	
	
	bool GetResourceTradeRoute(sint32 resource, TradeRoute & route) const;

	
	bool IsSellingResourceTo(sint32 resource, Unit & destination) const;

	sint32 GetOvertimeCost();
	bool BuyFront();
	bool AlreadyBoughtFront() { return m_buyFront; }
	void RemoveFront();
	void BuildWhat() const;
	sint32 HowMuchLonger() const;
	sint32 HowMuchLonger( sint32 productionRemaining ) const;
	void SellBuilding(sint32 which, bool byChoice = true);
	sint32 SellingBuilding() { return m_sellBuilding; }

	double GetMovementCost() const;
	void SetRoad() const;

#ifdef CTP1_TRADE
	sint32 GetResourceCount();
#endif

	void SetSize(sint32 size);

	
	void SetImprovements(uint64 improvements) { m_built_improvements = improvements; }
	void SetWonders(uint64 wonders) { m_builtWonders = wonders; }
	sint32 GetNumTradeRoutes() const;

	void AddTradedResources(Resources &resources);
#ifdef CTP1_TRADE
	void LocalResources(Resources &resources);
#endif

	sint32 GetCombatUnits() const;
	void IndicateImprovementBuilt(void) { m_improvementWasBuilt = TRUE ; }
	BOOL WasImprovementBuilt(void) const { return (m_improvementWasBuilt) ; }
	void IndicateTerrainImprovementBuilt(void) { m_terrainImprovementWasBuilt=TRUE ; }
	BOOL WasTerrainImprovementBuilt(void) const { return (m_terrainImprovementWasBuilt) ; }
	void IndicateHappinessAttacked(void) { m_happinessAttacked = TRUE ; }
	bool WasHappinessAttacked(void) const;
	void IndicateTerrainPolluted(void) { m_terrainWasPolluted = TRUE ; }
	BOOL WasTerrainPolluted(void) const { return (m_terrainWasPolluted) ; }
	sint32 GetScience(void) const { return (m_science) ; }
	sint32 GetScienceFromCommerce(void) const;
	bool CanBuildUnit(sint32 type) const;
	bool CanBuildBuilding(sint32 type) const;

	bool CanBuildWonder(sint32 type) const;
	void Injoin(sint32 player);
	bool IsInjoined() const;
	sint32 InjoinedBy() const { return m_injoinedBy; }

	bool HaveImprovement(const sint32 type) const;

	void RecalculateResources();
	sint32 CountTradeWith(PLAYER_INDEX player) const;

	double IsProtectedFromSlavery() const;
	void NotifyAdvance(AdvanceType advance);

	void ContributeScience(double incomePercent,
	                       double &addscience, double &subgold);
	sint32 TurnsToNextPop();//PFT 29 mar 05. Fixed to compute what it says.
	sint32 FreeSlaves();

	void AddEndGameObject(sint32 type);

	bool SendSlaveTo(Unit &dest);
	void SetFullHappinessTurns(sint32 turns);

	sint32 GetHappinessFromPops();
#if !defined(NEW_RESOURCE_PROCESS)
	sint32 GetScienceFromPops(bool considerOnlyFromTerrain = false) const;
#endif

	sint32 GetIncomingTrade() const;
	sint32 GetOutgoingTrade() const;
	void FinishBuilding();
	void DestroyRandomBuilding();
	void AddConversionUnhappiness(sint32 who);

	void InsertCapitalization();
	void InsertInfrastructure();
	void BuildCapitalization();
	bool CanBuildCapitalization() const;
	void BuildInfrastructure();
	bool CanBuildInfrastructure() const;
	bool IsBuildingCapitalization() { return m_buildCapitalization != FALSE; }
	bool IsBuildingInfrastructure() { return m_buildInfrastructure != FALSE; }
	void StopInfrastructureCapitalization();
	void EliminateNukes();

	void KillAllTradeRoutes();

	void RemoveEndGameObjects();

	bool NeedToDoUprising() const;
	void CheckForSlaveUprising();

	void Disband();

	void AddSlaveBit(sint32 player);
	uint32 GetSlaveBits();

	sint32 GetTurnFounded() const { return m_turnFounded; }

	void AddGoods(SlicObject *obj);
	sint32 GetGoodCountInRadius(sint32 good);

	sint32 GetBioInfectedBy( void ) const { return m_bioInfectedBy; }
	sint32 GetNanoInfectedBy( void ) const { return m_nanoInfectedBy; }
	sint32 GetProductionLostToFranchise( void ) const { return m_productionLostToFranchise; }

	void SetProbeRecoveredHere(bool recovered){ m_probeRecoveredHere = recovered; }
	bool GetProbeRecoveredHere() { return m_probeRecoveredHere; }

	bool HasSleepingUnits() const;

	bool IsProtectedFromConversion();
	sint32 GetGoldFromCapitalization() { return m_gold_from_capitalization; }

	sint32 CityGrowthCoefficient();
	void DestroyWonder(sint32 which);

	bool CapturedThisTurn() const { return m_capturedThisTurn != FALSE; }
	void SetSentInefficientMessage() { m_sentInefficientMessageAlready = true; }

	void BuildFront();

	
	sint32 GetValue() const;

	sint32 GetSizeIndex() const { return m_sizeIndex; }
	void MakeCitizen(PopDBIndex type, const MapPoint &center_pos,
	                 sint32 origOwner);

	
#if !defined(NEW_RESOURCE_PROCESS)
	void	GetFullAndPartialRadii(sint32 &fullRadius, sint32 &partRadius) const;
	double	GetUtilisationRatio(uint32 const squaredDistance) const;
#endif

	sint32 PopCount() const;
	sint32 SpecialistCount(POP_TYPE type) const;
	sint32 SlaveCount() const;
	sint32 WorkerCount() const;
	sint32 ScientistCount() const;
	sint32 FarmerCount() const;
	sint32 EntertainerCount() const;
	sint32 MerchantCount() const;
	sint32 LaborerCount() const;
	

#if defined(NEW_RESOURCE_PROCESS)
	void ComputeSizeIndexes(const sint32 & workers, sint32 & size_index) const;
#else
	void ComputeSizeIndexes(const sint32 & workers, sint32 & size_index, sint32 & full_index, sint32 & partial_index) const;
#endif

	
	sint32 GetBestSpecialist(const POP_TYPE & type) const;

	void AdjustSizeIndices();
	void ChangeSpecialists(POP_TYPE type, sint32 delta);
	void ChangePopulation(sint32 delta);
	void FindBestSpecialists();

	
	bool GetUseGovernor() const;
	void SetUseGovernor(const bool &value);

	sint32 GetBuildListSequenceIndex() const;
	void SetBuildListSequenceIndex(const sint32 &value);

	sint32 GetGarrisonOtherCities() const;
	void SetGarrisonOtherCities(const sint32 &value);

	sint32 GetGarrisonComplete() const;
	void SetGarrisonComplete(const sint32 &value);

	
	sint8 GetCurrentGarrison() const;
	void SetCurrentGarrison(const sint8 & value);

	
	sint8 GetNeededGarrison() const;
	void SetNeededGarrison(const sint8 & value);

	
	double GetCurrentGarrisonStrength() const;
	void SetCurrentGarrisonStrength(const double & value);

	
	double GetNeededGarrisonStrength() const;
	void SetNeededGarrisonStrength(const double & value);

	double GetVisionRadius() const;

	void InsertBuildItem(sint32 index, uint32 category, sint32 type);

	void AddSellBuilding(sint32 building);
	bool PayForBuyFront();
	void AddBuyFront();

	sint32 GetMaxFoodFromTerrain() { return m_max_food_from_terrain; }
	sint32 GetMaxProductionFromTerrain() { return m_max_prod_from_terrain; }
	sint32 GetMaxCommerceFromTerrain() { return m_max_gold_from_terrain; }

	sint32 GetGrowthRate() const { return m_growth_rate; }
	double GetOvercrowdingCoefficient() const { return m_overcrowdingCoeff; }

	sint32 GetPartialPopulation() const { return m_partialPopulation; }

	void AddImprovement(sint32 type);

	static bool FindGoodDistancesCallback(const MapPoint &pos, Cell *cell, void *cookie);
	void FindGoodDistances();
	sint32 GetDistanceToGood(sint32 good);

	void RemoveBorders();

	void ResetStarvationTurns();
	sint32 GetStarvationProtection();

	double GetOffenseBonus(const Unit &defender);
	uint64 GetEffectiveBuildings() const;

	sint32 GetDesiredSpriteIndex(bool justTryLand = false);

	
	bool IsACopy();

	void   DoSupport(bool projectedOnly);
	sint32 GetSupport() const;
#if !defined(NEW_RESOURCE_PROCESS)
	void   SplitScience(bool projectedOnly);
	void   SplitScience(bool projectedOnly, sint32 &gold, sint32 &science, bool considerOnlyFromTerrain = false) const;
#endif
	sint32 GetProjectedScience();
	sint32 GetFounder() const;

	sint32 ComputeProductionLosses(sint32 gross_production, sint32 &crime_loss, sint32 &franchise_loss) const;

	double GetOvercrowding(const CitySizeRecord *rec){ 
		return rec->GetBaseOvercrowding() + GetBuildingOvercrowdingBonus(); 
	};

	sint32 GetMaxPop(const CitySizeRecord *rec){ 
		return rec->GetBaseMaxPop() + GetBuildingMaxPopIncrease(); 
	};

	sint32 CrimeLoss(sint32 gross) const;
	double CrimeLoss(double gross) const;

	sint32 GetCityStyle() const;
	void   SetCityStyle(sint32 style);
	sint32 GetRing(MapPoint pos) const;
	sint32 GetFoodFromRing(sint32 ring) const;
	sint32 GetProdFromRing(sint32 ring) const;
	sint32 GetGoldFromRing(sint32 ring) const;
	sint32 GetRingSize(sint32 ring) const;
#if defined(NEW_RESOURCE_PROCESS)
	sint32 GetWorkingPeopleInRing(sint32 ring) const;
	sint32 TilesForWorking() const {return GetRingSize(-1);}

	void ProcessResources();
	void CalculateResources();
	void PayResources();
	void AddCapitalizationAndTryToBuild();
	void ComputeSpecialistsEffects();

	double ProcessFood(sint32 food) const;
	double ProcessProd(sint32 prod) const;
	double ProcessGold(sint32 gold) const;
	double ProcessScie(sint32 scinece) const;
	void ApplyFoodCoeff(double &food) const;
	void ApplyProdCoeff(double &prod) const;
	void ApplyGoldCoeff(double &gold) const;
	void ApplyGoldCoeff(sint32 &gold) const;
	void ApplyKnowledgeCoeff(double &science) const;

	double BioinfectionLoss(double prod) const;
	double FranchiseLoss(double prod) const;
	double ConversionLoss(double gold) const;

	double FoodFromOnePop(double &crimeLoss) const;
	double ProdFromOnePop(double &crimeLoss, double &bioinfectionLoss, double &franchiseLoss) const;
	double GoldFromOnePop(double &crimeLoss, double &conversionLoss) const;
	double ScieFromOnePop(double &crimeLoss) const;

	void ResourceFractions(double &foodFraction, double &prodFraction, double goldFraction, sint32 workingPeople) const;
	sint32 GetUnemployedPeople() const;

	double GetMaxProcessFood() const { return m_max_processed_terrain_food - m_grossFoodCrimeLoss; }
	double GetMaxProcessProd() const { return m_max_processed_terrain_prod - m_grossProdCrimeLoss - m_grossProdBioinfectionLoss - m_grossProdFranchiseLoss; }
	double GetMaxProcessGold() const { return m_max_processed_terrain_gold - m_grossGoldCrimeLoss - m_grossGoldConversionLoss;}
	double GetMaxProcessScie() const { return m_max_processed_terrain_scie - m_grossScieCrimeLoss; }
	void GetSpecialistsEffect(sint32 ring, double &farmersEff, double &laborersEff, double &merchantsEff, double &scientistsEff) const;
	double GetFarmersEffect(sint32 ring) const { return m_farmersEff[ring];}
	double GetLaborersEffect(sint32 ring) const { return m_laborersEff[ring];}
	double GetMerchantsEffect(sint32 ring) const { return m_merchantsEff[ring];}
	double GetScientistsEffect(sint32 ring) const { return m_scientistsEff[ring];}

#endif

	sint32 StyleHappinessIncr() const;
	sint32 GoodHappinessIncr() const;              //EMOD 4-27-2006
	bool CanCollectGood(sint32 good) const;        //EMOD 4-27-2006 
	bool IsBuildingOperational(sint32 type) const; //EMOD 5-15-2006
	sint32 SectarianHappiness() const;             //EMOD 5-24-2006
//	sint32 AddGoodToCity (sint32 good) const;      //EMOD 5-31-2006
	sint32 TileImpHappinessIncr() const;		   //EMOD 10-05-2006
//Religion Bools for Civ4 style Religion MAPICONS
    /// @todo Generalise the next block of functions to something which has the
    ///       religion number as parameter (HasReligion(int religion)). 
    ///       Fixing the number of religions in the code as method names is bad 
    ///       style. Just imagine we would have had BuildUnit1(), BuildUnit2(), 
    ///       etc. as methods.
	bool IsReligion1() const;
	bool IsReligion2() const;
	bool IsReligion3() const;
	bool IsReligion4() const;
	bool IsReligion5() const;
	bool IsReligion6() const;
	bool IsReligion7() const;
	bool IsReligion8() const;
	bool IsReligion9() const;
	bool IsReligion10() const;

	
	
	
	
	void ProcessAllResources();

private:
	bool    IsBankrupting(void) const;
	bool    PayWages(bool projectedOnly);
};

uint32 CityData_CityData_GetVersion(void);
#endif


