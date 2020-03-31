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
// - Added GetCityLandAttackBonus, GetCityAirAttackBonus and
//   GetCitySeaAttackBonus for battleview window. (07-Mar-2009 Maq)
// - Added functions to find total that each specialist type gives to a city,
//   after crime and other modifiers. (28-Mar-2009 Maq)
// - Changed science formula to deduct crime after the government coefficient
//   like all other resources. (22-Jul-2009 Maq)
// - Added methods to find food and production before deductions. (22-Jul-2009 Maq)
// - Added stuff for reimplementing switch production penalty. (22-Jul-2009 Maq)
// - Reorderd the member variables and converted m_name to a pointer to reduce
//   the needed memory. (13-Jan-2019 Martin Gühmann)
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

//struct TerrainValue;

class CityData : public CityRadiusCallback
{
private:

//----------------------------------------------------------------------------
// Sort the member variables by size to save memory
// Before sorting the size was 1248 bytes
// After sorting, converting BOOL to bool, and revoing
// m_name as buffer: 672 bytes
//----------------------------------------------------------------------------

	// 8 bit
	double            m_food_delta;
	double            m_gross_food;
	double            m_net_food;
	double            m_food_lost_to_crime;
	double            m_food_consumed_this_turn;
	double            m_currentGarrisonStrength;
	double            m_neededGarrisonStrength;
	double            m_gross_food_before_bonuses;
	double            m_overcrowdingCoeff;
	double            m_defensiveBonus;
	double            m_bonusFoodCoeff;
	double            m_bonusFood;
	double            m_bonusProdCoeff;
	double            m_bonusProd;
	double            m_bonusGoldCoeff;
	double            m_bonusGold;
	double            m_bonusScieCoeff;
	double            m_bonusScie;

#if defined(NEW_RESOURCE_PROCESS)
	double            m_max_processed_terrain_food;
	double            m_max_processed_terrain_prod;
	double            m_max_processed_terrain_gold;
	double            m_max_processed_terrain_scie;

	double            m_grossFoodCrimeLoss;
	double            m_grossProdCrimeLoss;
	double            m_grossGoldCrimeLoss;
	double            m_grossScieCrimeLoss;

	double            m_grossProdBioinfectionLoss;
	double            m_grossProdFranchiseLoss;
	double            m_grossGoldConversionLoss;

	double            m_foodFromOnePop;
	double            m_prodFromOnePop;
	double            m_goldFromOnePop;
	double            m_scieFromOnePop;

	double            m_crimeFoodLossOfOnePop;
	double            m_crimeProdLossOfOnePop;
	double            m_crimeGoldLossOfOnePop;
	double            m_crimeScieLossOfOnePop;

	double            m_bioinfectionProdLossOfOnePop;
	double            m_franchiseProdLossOfOnePop;
	double            m_conversionGoldLossOfOnePop;

	double            m_gross_science;
	double            m_science_lost_to_crime;
#endif

	// 8 Bit
	uint64            m_built_improvements;
	uint64            m_builtWonders;

	// Pointers 4 byte in a 32 bit program, 8 byte in a 64 bit program
	Happy            *m_happy;
	MBCHAR           *m_name;
	sint32           *m_distanceToGood;
	sint32           *m_ringFood;
	sint32           *m_ringProd;
	sint32           *m_ringGold;
	sint32           *m_ringSizes;
	SlicObject       *m_tempGoodAdder;
	UnitDynamicArray *m_killList;
#if defined(NEW_RESOURCE_PROCESS)
	double           *m_farmersEff;
	double           *m_laborersEff;
	double           *m_merchantsEff;
	double           *m_scientistsEff;

	// 4 Bit
	sint32    m_productionLostToBioinfection;
	sint32    m_max_scie_from_terrain;
#endif

	// 4 Bit
	RADIUS_OP         m_cityRadiusOp;
	UPRISING_CAUSE    m_doUprising;
	CONVERTED_BY      m_convertedBy;
	CITY_ATTITUDE     m_city_attitude;
	MapPoint          m_pos;
	Unit              m_home_city;
	PLAYER_INDEX      m_owner;
	uint32            m_slaveBits;

	sint32            m_accumulated_food;
	sint32            m_shieldstore;
	sint32            m_shieldstore_at_begin_turn;
	sint32            m_build_category_at_begin_turn;
	sint32            m_net_gold;
	sint32            m_gold_lost_to_crime;
	sint32            m_gross_gold;
	sint32            m_goldFromTradeRoutes;
	sint32            m_goldLostToPiracy;
	sint32            m_goldFromTransitRoutes;
	sint32            m_science;
	sint32            m_luxury;
	sint32            m_collected_production_this_turn;  // Will be unused
	sint32            m_gross_production;
	sint32            m_net_production;
	sint32            m_production_lost_to_crime;
	sint32            m_total_pollution;
	sint32            m_cityPopulationPollution;
	sint32            m_cityIndustrialPollution;
	sint32            m_cityPollutionCleaner;
	sint32            m_spied_upon;                      // A counter
	sint32            m_franchise_owner;
	sint32            m_franchiseTurnsRemaining;
	sint32            m_watchfulTurns;
	sint32            m_bioInfectionTurns;
	sint32            m_bioInfectedBy;
	sint32            m_nanoInfectionTurns;
	sint32            m_nanoInfectedBy;
	sint32            m_convertedTo;
	sint32            m_convertedGold;
	sint32            m_injoinedBy;
	sint32            m_airportLastUsed;
	sint32            m_founder;
	sint32            m_wages_paid;                      // The wages that were paid
	sint32            m_pw_from_infrastructure;
	sint32            m_gold_from_capitalization;
	sint32            m_turnFounded;
	sint32            m_productionLostToFranchise;
	sint32            m_population;
	sint32            m_partialPopulation;
	sint32            m_sizeIndex;
	sint32            m_workerFullUtilizationIndex;
	sint32            m_workerPartialUtilizationIndex;
	sint32            m_buildListSequenceIndex;
	sint32            m_sellBuilding;                    // The building that will be sold
	sint32            m_max_food_from_terrain;
	sint32            m_max_prod_from_terrain;
	sint32            m_max_gold_from_terrain;
	sint32            m_growth_rate;
	sint32            m_starvation_turns;
	sint32            m_cityStyle;
	sint32            m_build_category_before_load_queue;
	sint32            m_scie_lost_to_crime;
	sint32            m_gross_prod_before_bonuses;
	sint32            m_happinessAttackedBy;
	sint32            m_radiusNewOwner;
	sint32            m_tilecount;
	sint32            m_whichtile;
	sint32            m_tempGood;
	sint32            m_tempGoodCount;
	sint32            m_culture;   //emod
	sint32            m_secthappy; //emod

	// 2 bit
	sint16            m_lastCelebrationMsg;

	// 1 bit
	uint8             m_min_turns_revolt;        // Number of revolt risk free turns.
	sint8             m_currentGarrison;
	sint8             m_neededGarrison;
	bool              m_alreadySoldABuilding;
	bool              m_probeRecoveredHere;
	bool              m_buyFront;
	bool              m_sentInefficientMessageAlready;
	bool              m_garrisonOtherCities;
	bool              m_contribute_materials;
	bool              m_contribute_military;
	bool              m_capturedThisTurn;
	bool              m_walls_nullified;         // Not used?
	bool              m_terrainWasPolluted;
	bool              m_terrainImprovementWasBuilt;
	bool              m_improvementWasBuilt;
	bool              m_isInjoined;
	bool              m_buildInfrastructure; // Unused, can be replace in size of BOOL for saving
	bool              m_buildCapitalization; // Unused, can be replace in size of BOOL for saving
	bool              m_paidForBuyFront;
	bool              m_garrisonComplete;
	bool              m_useGovernor;
	bool              m_is_rioting;

	BuildQueue        m_build_queue;

	TradeDynamicArray m_tradeSourceList;
	TradeDynamicArray m_tradeDestinationList;
#ifdef CTP1_TRADE
	Resources         m_resources;
	Resources         m_localResources;
#else
	Resources         m_collectingResources;
	Resources         m_sellingResources;
	Resources         m_buyingResources;
#endif
	sint16            m_numSpecialists[POP_MAX];
	sint32            m_specialistDBIndex[POP_MAX];

//----------------------------------------------------------------------------
// End member variable section
//----------------------------------------------------------------------------

	friend class NetCity;
	friend class NetCity2;
	friend class NetCityName;
	friend class NetCityResources;
	friend class World;
	friend class Happy;
	friend class NetHappy;
	friend class NetCityBuildQueue;

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
	bool HasEffectiveBuilding(sint32 building) const{ return (GetEffectiveBuildings() & (static_cast<uint64>(1) << static_cast<uint64>(building))) != 0; };
	bool HasBuilding(sint32 building) const{ return (m_built_improvements & (static_cast<uint64>(1) << static_cast<uint64>(building))) != 0; };
	Unit GetHomeCity() const { return m_home_city; };
	sint32 GetOwner() const { return m_owner; }
	void ResetCityOwner(sint32 owner);

	sint32 GetStoredCityFood() const { return m_accumulated_food; }
	sint32 GetNetCityFood() const { return sint32(m_food_delta); }
	sint32 GetGrossCityFood() const { return sint32(m_gross_food); }
	sint32 GetGrossCityFoodBeforeBonuses() const { return sint32(m_gross_food_before_bonuses); }
	sint32 GetGrossCityProdBeforeBonuses() const { return sint32(m_gross_prod_before_bonuses); }

	sint32 GetStarvationTurns() const {return m_starvation_turns;}

	double GetProducedFood() const { return m_net_food; }
	double GetConsumedFood() const { return m_food_consumed_this_turn; }

	void SetShieldstore (sint32 s);
	void AddShields(sint32 s);

#if !defined(NEW_RESOURCE_PROCESS)
	sint32 ProcessProduction(bool projectedOnly, sint32 &grossProduction, sint32 &collectedProduction, sint32 &crimeLoss, sint32 &franchiseLoss, bool considerOnlyFromTerrain = false); // const;
	sint32 ProcessProduction(bool projectedOnly);
#endif

	double ProjectMilitaryContribution();
	sint32 GetStoredCityProduction() const { return m_shieldstore; }
	sint32 GetBuildCategoryAtBeginTurn() const { return m_build_category_at_begin_turn; }
	sint32 GetBuildCategoryBeforeLoadQueue() const { return m_build_category_before_load_queue; }
	void SetBuildCategoryAtBeginTurn(sint32 cat) { m_build_category_at_begin_turn = cat; }
	void CheckSwitchProductionPenalty(sint32 newCat);
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
	bool GetMaterialContribution() const { return m_contribute_materials;}

	void SetMilitaryContribution(bool on) { m_contribute_military = on; }
	bool GetMilitaryContribution() { return m_contribute_military; }

	sint32 LoadQueue(const MBCHAR *file);
	sint32 SaveQueue(const MBCHAR *file);

	BuildQueue *GetBuildQueue(void) { return &m_build_queue; }


	void   CollectResources();
	void   CollectResourcesFinally();

        sint32 FoodFromTrade();
        sint32 ProdFromTrade();
        sint32 GoldFromTrade();

#if !defined(NEW_RESOURCE_PROCESS)
	sint32 ProcessFood();
	void   ProcessFood(double &foodLostToCrime, double &producedFood, double &grossFood, bool considerOnlyFromTerrain = false) const;
	double ProcessFinalFood(double &foodLossToCrime, double &grossFood) const;

	void   CalculateBonusFood();
	void   CalculateCoeffFood();
	void   CalculateBonusProd();
	void   CalculateCoeffProd();
	void   CalculateBonusGold();
	void   CalculateCoeffGold();
	void   CalculateBonusScie();
	void   CalculateCoeffScie();
#endif

	sint32 GetSpecialistsResources(POP_TYPE pop) const;
	void   EatFood();
	bool   FoodSupportTroops();

	sint32 GetBuildingOvercrowdingBonus() const;
	sint32 GetBuildingMaxPopIncrease() const;
	sint32 GetPossibleBuildingMaxPopIncrease() const;
	sint32 GetAllBuildingMaxPopIncrease() const;
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

	void UpdateSprite(void);

	bool IsWatchful() const;
	void SetWatchful();
	void ModifySpecialAttackChance(UNIT_ORDER_TYPE attack, double &chance);

	void RemoveOneSlave(PLAYER_INDEX p);

#if !defined(NEW_RESOURCE_PROCESS)
	void CollectOtherTrade(const bool projectedOnly);
#endif
	void CheckTopTen();
	sint32 SupportBuildings(bool projectedOnly);
	sint32 GetSupportCityCost() const;
	sint32 GetSupportBuildingsCost() const;

	void AddTradeRoute(TradeRoute &route, bool fromNetwork);
	void DelTradeRoute(TradeRoute route);
	sint32 IsUsedInTradeRoute(const MapPoint &qpos);
	void CalculateTradeRoutes(bool projectedOnly);
	sint32 CalculateGoldFromResources();
	void AddTradeResource(ROUTE_TYPE type, sint32 resource);
#ifdef CTP1_TRADE
	sint32 GetResourceCount(sint32 resource);
	sint32 GetLocalResourceCount(sint32 resource);
#endif

	sint32 CalcWages() const;
	sint32 GetWagesNeeded(const sint32 & wages_per_person) const;
	sint32 GetWagesNeeded();

	sint32 GetGoldFromTradeRoutes() const { return m_goldFromTradeRoutes; }
	sint32 GetGoldLostToPiracy() const { return m_goldLostToPiracy; }
	sint32 GetGoldFromTransitRoutes() const { return m_goldFromTransitRoutes; }
	TradeDynamicArray* GetTradeSourceList() { return (&m_tradeSourceList); }
	TradeDynamicArray* GetTradeDestinationList() { return (&m_tradeDestinationList); }

	bool BreakOneSourceRoute(ROUTE_TYPE type, sint32 resource);

#if !defined(NEW_RESOURCE_PROCESS)
	void CollectGold(sint32 &gold, sint32 &convertedGold, sint32 &crimeLost, bool considerOnlyFromTerrain = false) const;
	void ProcessGold(sint32 &gold, bool considerOnlyFromTerrain = false) const;
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
	bool HasCityWonder(sint32 wonder) const { return (m_builtWonders & (static_cast<uint64>(1) << static_cast<uint64>(wonder))) != 0; }

	void DestroyCapitol();
	void DestroyOnePerCiv(); //emod destroy small wonders
	void SetCapitol();
	void DestroyImprovement(sint32 type);
	void NewGovernment(sint32 government_type);

	double GetDefendersBonus() const;
	double GetCityLandAttackBonus() const;
	double GetCityAirAttackBonus() const;
	double GetCitySeaAttackBonus() const;
	double GetDefendersBonusNoWalls() const;

	// Modified by kaan to address bug # 12

	void NoRevoltCountdown();

	bool ShouldRevolt(const sint32 inciteBonus);
	void Revolt(sint32 &playerToJoin, bool causeIsExternal = false);
	void TeleportUnits(const MapPoint &pos,  bool &revealed_foreign_units, sint32 foreigner);
	void StopTradingWith(const PLAYER_INDEX bannedRecipient);
	Happy *GetHappy(void) { return (m_happy); }

	MBCHAR *GetName(void) { return (m_name); }
	void SetName(const MBCHAR *name);

	void GetPop(sint32 &p)const { p = m_population; }

	void GetConsumptionStats(sint32 &foodConsumed) const {
		foodConsumed = (sint32)(m_food_consumed_this_turn);
	}

	sint32 GetScienceCrime() const { return m_scie_lost_to_crime; }
	sint32 GetTradeCrime() const { return m_gold_lost_to_crime; }
	sint32 GetProdCrime() const { return m_production_lost_to_crime; }
	void GetFoodCrime( sint32 &foodCrime ) const {
		foodCrime = (sint32)(m_food_lost_to_crime);
	}

	bool GetIsRioting(void) const { return m_is_rioting; }

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
	void RemoveFranchise();
	sint32 GetFranchiseOwner() const { return m_franchise_owner;}
	sint32 GetFranchiseTurnsRemaining() const;
	void SetFranchiseTurnsRemaining(sint32 turns);

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
	bool HasTileImpInRadius(sint32 tileimp) const; //EMOD 4-7-2006 bool if a city has imp in radius
	bool HasAnyTileImpInRadiusAndIsExploredBy(const sint32 player) const;

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
	void IndicateImprovementBuilt(void) { m_improvementWasBuilt = true; }
	bool WasImprovementBuilt(void) const { return m_improvementWasBuilt; }
	void IndicateTerrainImprovementBuilt(void) { m_terrainImprovementWasBuilt = true; }
	bool WasTerrainImprovementBuilt(void) const { return m_terrainImprovementWasBuilt; }
	bool WasHappinessAttacked(void) const;
	void HappinessAttackedBy(sint32 player);
	sint32 GetHappinessAttackedBy() const { return m_happinessAttackedBy;}
	void IndicateTerrainPolluted(void) { m_terrainWasPolluted = true; }
	bool WasTerrainPolluted(void) const { return m_terrainWasPolluted; }
	sint32 GetScience(void) const { return m_science; }
	sint32 GetScienceFromCommerce(void) const;
	bool CanBuildUnit(sint32 type) const;
	bool CanBuildBuilding(sint32 type) const;

	bool CanBuildWonder(sint32 type) const;
	void Injoin(sint32 player);
	bool IsInjoined() const;
	sint32 InjoinedBy() const { return m_injoinedBy; }

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

	sint32 GetHappinessFromPops() const;

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
	bool IsBuildingCapitalization() const;
	bool IsBuildingInfrastructure() const;
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

	bool CapturedThisTurn() const { return m_capturedThisTurn != false; }
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

	bool GetGarrisonOtherCities() const;
	void SetGarrisonOtherCities(const bool &value);

	bool GetGarrisonComplete() const;
	void SetGarrisonComplete(const bool &value);

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
	void   SplitScience(bool projectedOnly, sint32 &gold, sint32 &science, sint32 &scieCrime, bool considerOnlyFromTerrain = false) const;
#endif
	sint32 GetProjectedScience();
	sint32 GetFounder() const;

	sint32 ComputeProductionLosses(sint32 gross_production, sint32 &crime_loss, sint32 &franchise_loss) const;

	double GetOvercrowding(const CitySizeRecord *rec) const
	{
		return rec->GetBaseOvercrowding() + GetBuildingOvercrowdingBonus();
	};

	sint32 GetMaxPop() const
	{
		return GetMaxPop(g_theCitySizeDB->Get(m_sizeIndex));
	};

	sint32 GetMaxPop(const CitySizeRecord *rec) const
	{
		return rec->GetBaseMaxPop() + GetBuildingMaxPopIncrease();
	};

	sint32 GetOverallMaxPop() const
	{
		return g_theCitySizeDB->Get(m_sizeIndex)->GetBaseMaxPop() + GetAllBuildingMaxPopIncrease();
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
	sint32 GoodHappinessIncr() const;									//EMOD 4-27-2006
	bool CanCollectGood(sint32 good) const;        						//EMOD 4-27-2006
	bool IsBuildingOperational(sint32 type) const; 						//EMOD 5-15-2006
	sint32 SectarianHappiness() const; //{ return m_secthappy; }      		//EMOD 5-24-2006
//	sint32 AddGoodToCity (sint32 good) const;      						//EMOD 5-31-2006
	sint32 TileImpHappinessIncr() const;		   						//EMOD 10-05-2006
	sint32 ProcessSectarianHappiness(sint32 newsecthappy, sint32 owner, sint32 citystyle) const; 			//EMOD 4-9-2007

	sint32 ConsumeEnergy();
	sint32 ProduceEnergy();

	sint32 GetNumCityBuildings() const;
	sint32 GetNumCityWonders() const;

	void RiotCasualties();
	void InsurgentSpawn();
	void CityGovernmentModifiers();
	void Militia();

	void ProcessAllResources();
	bool HasReligionIcon() const;
	bool IsReligious() const;

	bool CityIsOnTradeRoute();
	void GiveTradeRouteGold();
	bool HasSpecialIcon() const;
	void AddCitySlum();
	void AddCityExpansion();
	sint32 GetNumUrbanTile(const MapPoint pos) const;
	sint32 GetSlumTileAvailable(const MapPoint pos) const;
	sint32 GetUrbanTileAvailable(const MapPoint pos) const;
	bool IsCoastal() const;

	void PreResourceCalculation();

	double  GetBonusFoodCoeff() const { return m_bonusFoodCoeff; };
	double  GetBonusProdCoeff() const { return m_bonusProdCoeff + g_player[m_owner]->GetWorkdayPerPerson(); };
	double  GetBonusGoldCoeff() const { return m_bonusGoldCoeff; };
	double  GetBonusScieCoeff() const { return m_bonusScieCoeff; };

private:
	void    SetNameLocal(const MBCHAR *name);
	bool    IsBankrupting(void) const;
	bool    PayWages(bool projectedOnly);

#if 0
	void PrintSizeOfClass()
	{
		DPRINTF(k_DBG_AI, ("\n"));
		DPRINTF(k_DBG_AI, ("Size of CityData class:\n"));
		DPRINTF(k_DBG_AI, ("CityData: %d\n", sizeof(CityData)));
		DPRINTF(k_DBG_AI, ("m_owner: %d\n", sizeof(m_owner)));
		DPRINTF(k_DBG_AI, ("m_slaveBits: %d\n", sizeof(m_slaveBits)));
		DPRINTF(k_DBG_AI, ("m_accumulated_food: %d\n", sizeof(m_accumulated_food)));
		DPRINTF(k_DBG_AI, ("m_shieldstore: %d\n", sizeof(m_shieldstore)));
		DPRINTF(k_DBG_AI, ("m_shieldstore_at_begin_turn: %d\n", sizeof(m_shieldstore_at_begin_turn)));
		DPRINTF(k_DBG_AI, ("m_build_category_at_begin_turn: %d\n", sizeof(m_build_category_at_begin_turn)));
		DPRINTF(k_DBG_AI, ("m_net_gold: %d\n", sizeof(m_net_gold)));
		DPRINTF(k_DBG_AI, ("m_gold_lost_to_crime: %d\n", sizeof(m_gold_lost_to_crime)));
		DPRINTF(k_DBG_AI, ("m_gross_gold: %d\n", sizeof(m_gross_gold)));
		DPRINTF(k_DBG_AI, ("m_goldFromTradeRoutes: %d\n", sizeof(m_goldFromTradeRoutes)));
		DPRINTF(k_DBG_AI, ("m_goldLostToPiracy: %d\n", sizeof(m_goldLostToPiracy)));
		DPRINTF(k_DBG_AI, ("m_goldFromTransitRoutes: %d\n", sizeof(m_goldFromTransitRoutes)));
		DPRINTF(k_DBG_AI, ("m_science: %d\n", sizeof(m_science)));
		DPRINTF(k_DBG_AI, ("m_luxury: %d\n", sizeof(m_luxury)));
		DPRINTF(k_DBG_AI, ("m_city_attitude: %d\n", sizeof(m_city_attitude)));
		DPRINTF(k_DBG_AI, ("m_collected_production_this_turn: %d\n", sizeof(m_collected_production_this_turn)));
		DPRINTF(k_DBG_AI, ("m_gross_production: %d\n", sizeof(m_gross_production)));
		DPRINTF(k_DBG_AI, ("m_net_production: %d\n", sizeof(m_net_production)));
		DPRINTF(k_DBG_AI, ("m_production_lost_to_crime: %d\n", sizeof(m_production_lost_to_crime)));
		DPRINTF(k_DBG_AI, ("m_built_improvements: %d\n", sizeof(m_built_improvements)));
		DPRINTF(k_DBG_AI, ("m_builtWonders: %d\n", sizeof(m_builtWonders)));
		DPRINTF(k_DBG_AI, ("m_food_delta: %d\n", sizeof(m_food_delta)));
		DPRINTF(k_DBG_AI, ("m_gross_food: %d\n", sizeof(m_gross_food)));
		DPRINTF(k_DBG_AI, ("m_net_food: %d\n", sizeof(m_net_food)));
		DPRINTF(k_DBG_AI, ("m_food_lost_to_crime: %d\n", sizeof(m_food_lost_to_crime)));
		DPRINTF(k_DBG_AI, ("m_food_consumed_this_turn: %d\n", sizeof(m_food_consumed_this_turn)));
		DPRINTF(k_DBG_AI, ("m_total_pollution: %d\n", sizeof(m_total_pollution)));
		DPRINTF(k_DBG_AI, ("m_cityPopulationPollution: %d\n", sizeof(m_cityPopulationPollution)));
		DPRINTF(k_DBG_AI, ("m_cityIndustrialPollution: %d\n", sizeof(m_cityIndustrialPollution)));
		DPRINTF(k_DBG_AI, ("m_cityPollutionCleaner: %d\n", sizeof(m_cityPollutionCleaner)));
		DPRINTF(k_DBG_AI, ("m_contribute_materials: %d\n", sizeof(m_contribute_materials)));
		DPRINTF(k_DBG_AI, ("m_contribute_military: %d\n", sizeof(m_contribute_military)));
		DPRINTF(k_DBG_AI, ("m_capturedThisTurn: %d\n", sizeof(m_capturedThisTurn)));
		DPRINTF(k_DBG_AI, ("m_spied_upon: %d\n", sizeof(m_spied_upon)));
		DPRINTF(k_DBG_AI, ("m_walls_nullified: %d\n", sizeof(m_walls_nullified)));
		DPRINTF(k_DBG_AI, ("m_franchise_owner: %d\n", sizeof(m_franchise_owner)));
		DPRINTF(k_DBG_AI, ("m_franchiseTurnsRemaining: %d\n", sizeof(m_franchiseTurnsRemaining)));
		DPRINTF(k_DBG_AI, ("m_watchfulTurns: %d\n", sizeof(m_watchfulTurns)));
		DPRINTF(k_DBG_AI, ("m_bioInfectionTurns: %d\n", sizeof(m_bioInfectionTurns)));
		DPRINTF(k_DBG_AI, ("m_bioInfectedBy: %d\n", sizeof(m_bioInfectedBy)));
		DPRINTF(k_DBG_AI, ("m_nanoInfectionTurns: %d\n", sizeof(m_nanoInfectionTurns)));
		DPRINTF(k_DBG_AI, ("m_nanoInfectedBy: %d\n", sizeof(m_nanoInfectedBy)));
		DPRINTF(k_DBG_AI, ("m_convertedTo: %d\n", sizeof(m_convertedTo)));
		DPRINTF(k_DBG_AI, ("m_convertedGold: %d\n", sizeof(m_convertedGold)));
		DPRINTF(k_DBG_AI, ("m_convertedBy: %d\n", sizeof(m_convertedBy)));
		DPRINTF(k_DBG_AI, ("m_terrainWasPolluted: %d\n", sizeof(m_terrainWasPolluted)));
		DPRINTF(k_DBG_AI, ("m_terrainImprovementWasBuilt: %d\n", sizeof(m_terrainImprovementWasBuilt)));
		DPRINTF(k_DBG_AI, ("m_improvementWasBuilt: %d\n", sizeof(m_improvementWasBuilt)));
		DPRINTF(k_DBG_AI, ("m_isInjoined: %d\n", sizeof(m_isInjoined)));
		DPRINTF(k_DBG_AI, ("m_injoinedBy: %d\n", sizeof(m_injoinedBy)));
		DPRINTF(k_DBG_AI, ("m_airportLastUsed: %d\n", sizeof(m_airportLastUsed)));
		DPRINTF(k_DBG_AI, ("m_founder: %d\n", sizeof(m_founder)));
		DPRINTF(k_DBG_AI, ("m_wages_paid: %d\n", sizeof(m_wages_paid)));
		DPRINTF(k_DBG_AI, ("m_pw_from_infrastructure: %d\n", sizeof(m_pw_from_infrastructure)));
		DPRINTF(k_DBG_AI, ("m_gold_from_capitalization: %d\n", sizeof(m_gold_from_capitalization)));
		DPRINTF(k_DBG_AI, ("m_buildInfrastructure: %d\n", sizeof(m_buildInfrastructure)));
		DPRINTF(k_DBG_AI, ("m_buildCapitalization: %d\n", sizeof(m_buildCapitalization)));
		DPRINTF(k_DBG_AI, ("m_paidForBuyFront: %d\n", sizeof(m_paidForBuyFront)));
		DPRINTF(k_DBG_AI, ("m_doUprising: %d\n", sizeof(m_doUprising)));
		DPRINTF(k_DBG_AI, ("m_turnFounded: %d\n", sizeof(m_turnFounded)));
		DPRINTF(k_DBG_AI, ("m_productionLostToFranchise: %d\n", sizeof(m_productionLostToFranchise)));
		DPRINTF(k_DBG_AI, ("m_probeRecoveredHere: %d\n", sizeof(m_probeRecoveredHere)));
		DPRINTF(k_DBG_AI, ("m_lastCelebrationMsg: %d\n", sizeof(m_lastCelebrationMsg)));
		DPRINTF(k_DBG_AI, ("m_alreadySoldABuilding: %d\n", sizeof(m_alreadySoldABuilding)));
		DPRINTF(k_DBG_AI, ("m_population: %d\n", sizeof(m_population)));
		DPRINTF(k_DBG_AI, ("m_partialPopulation: %d\n", sizeof(m_partialPopulation)));
		DPRINTF(k_DBG_AI, ("m_numSpecialists: %d\n", sizeof(m_numSpecialists)));
		DPRINTF(k_DBG_AI, ("m_specialistDBIndex: %d\n", sizeof(m_specialistDBIndex)));
		DPRINTF(k_DBG_AI, ("m_sizeIndex: %d\n", sizeof(m_sizeIndex)));
		DPRINTF(k_DBG_AI, ("m_workerFullUtilizationIndex: %d\n", sizeof(m_workerFullUtilizationIndex)));
		DPRINTF(k_DBG_AI, ("m_workerPartialUtilizationIndex: %d\n", sizeof(m_workerPartialUtilizationIndex)));
		DPRINTF(k_DBG_AI, ("m_useGovernor: %d\n", sizeof(m_useGovernor)));
		DPRINTF(k_DBG_AI, ("m_buildListSequenceIndex: %d\n", sizeof(m_buildListSequenceIndex)));
		DPRINTF(k_DBG_AI, ("m_garrisonOtherCities: %d\n", sizeof(m_garrisonOtherCities)));
		DPRINTF(k_DBG_AI, ("m_garrisonComplete: %d\n", sizeof(m_garrisonComplete)));
		DPRINTF(k_DBG_AI, ("m_currentGarrison: %d\n", sizeof(m_currentGarrison)));
		DPRINTF(k_DBG_AI, ("m_neededGarrison: %d\n", sizeof(m_neededGarrison)));
		DPRINTF(k_DBG_AI, ("m_currentGarrisonStrength: %d\n", sizeof(m_currentGarrisonStrength)));
		DPRINTF(k_DBG_AI, ("m_neededGarrisonStrength: %d\n", sizeof(m_neededGarrisonStrength)));
		DPRINTF(k_DBG_AI, ("m_sellBuilding: %d\n", sizeof(m_sellBuilding)));
		DPRINTF(k_DBG_AI, ("m_buyFront: %d\n", sizeof(m_buyFront)));
		DPRINTF(k_DBG_AI, ("m_max_food_from_terrain: %d\n", sizeof(m_max_food_from_terrain)));
		DPRINTF(k_DBG_AI, ("m_max_prod_from_terrain: %d\n", sizeof(m_max_prod_from_terrain)));
		DPRINTF(k_DBG_AI, ("m_max_gold_from_terrain: %d\n", sizeof(m_max_gold_from_terrain)));
		DPRINTF(k_DBG_AI, ("m_growth_rate: %d\n", sizeof(m_growth_rate)));
		DPRINTF(k_DBG_AI, ("m_overcrowdingCoeff: %d\n", sizeof(m_overcrowdingCoeff)));
		DPRINTF(k_DBG_AI, ("m_starvation_turns: %d\n", sizeof(m_starvation_turns)));
		DPRINTF(k_DBG_AI, ("m_cityStyle: %d\n", sizeof(m_cityStyle)));
		DPRINTF(k_DBG_AI, ("m_pos: %d\n", sizeof(m_pos)));
		DPRINTF(k_DBG_AI, ("m_is_rioting: %d\n", sizeof(m_is_rioting)));
		DPRINTF(k_DBG_AI, ("m_build_category_before_load_queue: %d\n", sizeof(m_build_category_before_load_queue)));
		DPRINTF(k_DBG_AI, ("m_scie_lost_to_crime: %d\n", sizeof(m_scie_lost_to_crime)));
		DPRINTF(k_DBG_AI, ("m_gross_food_before_bonuses: %d\n", sizeof(m_gross_food_before_bonuses)));
		DPRINTF(k_DBG_AI, ("m_gross_prod_before_bonuses: %d\n", sizeof(m_gross_prod_before_bonuses)));
		DPRINTF(k_DBG_AI, ("m_happinessAttackedBy: %d\n", sizeof(m_happinessAttackedBy)));
		DPRINTF(k_DBG_AI, ("m_home_city: %d\n", sizeof(m_home_city)));
		DPRINTF(k_DBG_AI, ("m_min_turns_revolt: %d\n", sizeof(m_min_turns_revolt)));
		DPRINTF(k_DBG_AI, ("m_build_queue: %d\n", sizeof(m_build_queue)));
		DPRINTF(k_DBG_AI, ("m_tradeSourceList: %d\n", sizeof(m_tradeSourceList)));
		DPRINTF(k_DBG_AI, ("m_tradeDestinationList: %d\n", sizeof(m_tradeDestinationList)));
		DPRINTF(k_DBG_AI, ("m_collectingResources: %d\n", sizeof(m_collectingResources)));
		DPRINTF(k_DBG_AI, ("m_sellingResources: %d\n", sizeof(m_sellingResources)));
		DPRINTF(k_DBG_AI, ("m_buyingResources: %d\n", sizeof(m_buyingResources)));
		DPRINTF(k_DBG_AI, ("m_happy: %d\n", sizeof(m_happy)));
		DPRINTF(k_DBG_AI, ("m_name: %d\n", sizeof(m_name)));
		DPRINTF(k_DBG_AI, ("m_distanceToGood: %d\n", sizeof(m_distanceToGood)));
		DPRINTF(k_DBG_AI, ("m_defensiveBonus: %d\n", sizeof(m_defensiveBonus)));
		DPRINTF(k_DBG_AI, ("m_ringFood: %d\n", sizeof(m_ringFood)));
		DPRINTF(k_DBG_AI, ("m_ringProd: %d\n", sizeof(m_ringProd)));
		DPRINTF(k_DBG_AI, ("m_ringGold: %d\n", sizeof(m_ringGold)));
		DPRINTF(k_DBG_AI, ("m_ringSizes: %d\n", sizeof(m_ringSizes)));
		DPRINTF(k_DBG_AI, ("m_bonusFoodCoeff: %d\n", sizeof(m_bonusFoodCoeff)));
		DPRINTF(k_DBG_AI, ("m_bonusFood: %d\n", sizeof(m_bonusFood)));
		DPRINTF(k_DBG_AI, ("m_bonusProdCoeff: %d\n", sizeof(m_bonusProdCoeff)));
		DPRINTF(k_DBG_AI, ("m_bonusProd: %d\n", sizeof(m_bonusProd)));
		DPRINTF(k_DBG_AI, ("m_bonusGoldCoeff: %d\n", sizeof(m_bonusGoldCoeff)));
		DPRINTF(k_DBG_AI, ("m_bonusGold: %d\n", sizeof(m_bonusGold)));
		DPRINTF(k_DBG_AI, ("m_bonusScieCoeff: %d\n", sizeof(m_bonusScieCoeff)));
		DPRINTF(k_DBG_AI, ("m_bonusScie: %d\n", sizeof(m_bonusScie)));
		DPRINTF(k_DBG_AI, ("m_cityRadiusOp: %d\n", sizeof(m_cityRadiusOp)));
		DPRINTF(k_DBG_AI, ("m_killList: %d\n", sizeof(m_killList)));
		DPRINTF(k_DBG_AI, ("m_radiusNewOwner: %d\n", sizeof(m_radiusNewOwner)));
		DPRINTF(k_DBG_AI, ("m_tilecount: %d\n", sizeof(m_tilecount)));
		DPRINTF(k_DBG_AI, ("m_whichtile: %d\n", sizeof(m_whichtile)));
		DPRINTF(k_DBG_AI, ("m_tempGoodAdder: %d\n", sizeof(m_tempGoodAdder)));
		DPRINTF(k_DBG_AI, ("m_tempGood: %d\n", sizeof(m_tempGood)));
		DPRINTF(k_DBG_AI, ("m_tempGoodCount: %d\n", sizeof(m_tempGoodCount)));
		DPRINTF(k_DBG_AI, ("m_sentInefficientMessageAlready: %d\n", sizeof(m_sentInefficientMessageAlready)));
		DPRINTF(k_DBG_AI, ("m_culture: %d\n", sizeof(m_culture)));
		DPRINTF(k_DBG_AI, ("m_secthappy: %d\n", sizeof(m_secthappy)));
		DPRINTF(k_DBG_AI, ("\n"));
	}

	void PrintData()
	{
		DPRINTF(k_DBG_AI, ("\n"));
		DPRINTF(k_DBG_AI, ("Data of CityData class:\n"));
		DPRINTF(k_DBG_AI, ("m_owner: %d\n", m_owner));
		DPRINTF(k_DBG_AI, ("m_slaveBits: %d\n", m_slaveBits));
		DPRINTF(k_DBG_AI, ("m_accumulated_food: %d\n", m_accumulated_food));
		DPRINTF(k_DBG_AI, ("m_shieldstore: %d\n", m_shieldstore));
		DPRINTF(k_DBG_AI, ("m_shieldstore_at_begin_turn: %d\n", m_shieldstore_at_begin_turn));
		DPRINTF(k_DBG_AI, ("m_build_category_at_begin_turn: %d\n", m_build_category_at_begin_turn));
		DPRINTF(k_DBG_AI, ("m_net_gold: %d\n", m_net_gold));
		DPRINTF(k_DBG_AI, ("m_gold_lost_to_crime: %d\n", m_gold_lost_to_crime));
		DPRINTF(k_DBG_AI, ("m_gross_gold: %d\n", m_gross_gold));
		DPRINTF(k_DBG_AI, ("m_goldFromTradeRoutes: %d\n", m_goldFromTradeRoutes));
		DPRINTF(k_DBG_AI, ("m_goldLostToPiracy: %d\n", m_goldLostToPiracy));
		DPRINTF(k_DBG_AI, ("m_goldFromTransitRoutes: %d\n", m_goldFromTransitRoutes));
		DPRINTF(k_DBG_AI, ("m_science: %d\n", m_science));
		DPRINTF(k_DBG_AI, ("m_luxury: %d\n", m_luxury));
		DPRINTF(k_DBG_AI, ("m_city_attitude: %d\n", m_city_attitude));
		DPRINTF(k_DBG_AI, ("m_collected_production_this_turn: %d\n", m_collected_production_this_turn));
		DPRINTF(k_DBG_AI, ("m_gross_production: %d\n", m_gross_production));
		DPRINTF(k_DBG_AI, ("m_net_production: %d\n", m_net_production));
		DPRINTF(k_DBG_AI, ("m_production_lost_to_crime: %d\n", m_production_lost_to_crime));
		DPRINTF(k_DBG_AI, ("m_built_improvements: %d\n", m_built_improvements));
		DPRINTF(k_DBG_AI, ("m_builtWonders: %d\n", m_builtWonders));
		DPRINTF(k_DBG_AI, ("m_food_delta: %f\n", m_food_delta));
		DPRINTF(k_DBG_AI, ("m_gross_food: %f\n", m_gross_food));
		DPRINTF(k_DBG_AI, ("m_net_food: %f\n", m_net_food));
		DPRINTF(k_DBG_AI, ("m_food_lost_to_crime: %f\n", m_food_lost_to_crime));
		DPRINTF(k_DBG_AI, ("m_food_consumed_this_turn: %f\n", m_food_consumed_this_turn));
		DPRINTF(k_DBG_AI, ("m_total_pollution: %d\n", m_total_pollution));
		DPRINTF(k_DBG_AI, ("m_cityPopulationPollution: %d\n", m_cityPopulationPollution));
		DPRINTF(k_DBG_AI, ("m_cityIndustrialPollution: %d\n", m_cityIndustrialPollution));
		DPRINTF(k_DBG_AI, ("m_cityPollutionCleaner: %d\n", m_cityPollutionCleaner));
		DPRINTF(k_DBG_AI, ("m_contribute_materials: %d\n", m_contribute_materials));
		DPRINTF(k_DBG_AI, ("m_contribute_military: %d\n", m_contribute_military));
		DPRINTF(k_DBG_AI, ("m_capturedThisTurn: %d\n", m_capturedThisTurn));
		DPRINTF(k_DBG_AI, ("m_spied_upon: %d\n", m_spied_upon));
		DPRINTF(k_DBG_AI, ("m_walls_nullified: %d\n", m_walls_nullified));
		DPRINTF(k_DBG_AI, ("m_franchise_owner: %d\n", m_franchise_owner));
		DPRINTF(k_DBG_AI, ("m_franchiseTurnsRemaining: %d\n", m_franchiseTurnsRemaining));
		DPRINTF(k_DBG_AI, ("m_watchfulTurns: %d\n", m_watchfulTurns));
		DPRINTF(k_DBG_AI, ("m_bioInfectionTurns: %d\n", m_bioInfectionTurns));
		DPRINTF(k_DBG_AI, ("m_bioInfectedBy: %d\n", m_bioInfectedBy));
		DPRINTF(k_DBG_AI, ("m_nanoInfectionTurns: %d\n", m_nanoInfectionTurns));
		DPRINTF(k_DBG_AI, ("m_nanoInfectedBy: %d\n", m_nanoInfectedBy));
		DPRINTF(k_DBG_AI, ("m_convertedTo: %d\n", m_convertedTo));
		DPRINTF(k_DBG_AI, ("m_convertedGold: %d\n", m_convertedGold));
		DPRINTF(k_DBG_AI, ("m_convertedBy: %d\n", m_convertedBy));
		DPRINTF(k_DBG_AI, ("m_terrainWasPolluted: %d\n", m_terrainWasPolluted));
		DPRINTF(k_DBG_AI, ("m_terrainImprovementWasBuilt: %d\n", m_terrainImprovementWasBuilt));
		DPRINTF(k_DBG_AI, ("m_improvementWasBuilt: %d\n", m_improvementWasBuilt));
		DPRINTF(k_DBG_AI, ("m_isInjoined: %d\n", m_isInjoined));
		DPRINTF(k_DBG_AI, ("m_injoinedBy: %d\n", m_injoinedBy));
		DPRINTF(k_DBG_AI, ("m_airportLastUsed: %d\n", m_airportLastUsed));
		DPRINTF(k_DBG_AI, ("m_founder: %d\n", m_founder));
		DPRINTF(k_DBG_AI, ("m_wages_paid: %d\n", m_wages_paid));
		DPRINTF(k_DBG_AI, ("m_pw_from_infrastructure: %d\n", m_pw_from_infrastructure));
		DPRINTF(k_DBG_AI, ("m_gold_from_capitalization: %d\n", m_gold_from_capitalization));
		DPRINTF(k_DBG_AI, ("m_buildInfrastructure: %d\n", m_buildInfrastructure));
		DPRINTF(k_DBG_AI, ("m_buildCapitalization: %d\n", m_buildCapitalization));
		DPRINTF(k_DBG_AI, ("m_paidForBuyFront: %d\n", m_paidForBuyFront));
		DPRINTF(k_DBG_AI, ("m_doUprising: %d\n", m_doUprising));
		DPRINTF(k_DBG_AI, ("m_turnFounded: %d\n", m_turnFounded));
		DPRINTF(k_DBG_AI, ("m_productionLostToFranchise: %d\n", m_productionLostToFranchise));
		DPRINTF(k_DBG_AI, ("m_probeRecoveredHere: %d\n", m_probeRecoveredHere));
		DPRINTF(k_DBG_AI, ("m_lastCelebrationMsg: %d\n", m_lastCelebrationMsg));
		DPRINTF(k_DBG_AI, ("m_alreadySoldABuilding: %d\n", m_alreadySoldABuilding));
		DPRINTF(k_DBG_AI, ("m_population: %d\n", m_population));
		DPRINTF(k_DBG_AI, ("m_partialPopulation: %d\n", m_partialPopulation));
		DPRINTF(k_DBG_AI, ("m_numSpecialists[0]: %d\n", m_numSpecialists[0]));
		DPRINTF(k_DBG_AI, ("m_numSpecialists[1]: %d\n", m_numSpecialists[1]));
		DPRINTF(k_DBG_AI, ("m_numSpecialists[2]: %d\n", m_numSpecialists[2]));
		DPRINTF(k_DBG_AI, ("m_numSpecialists[3]: %d\n", m_numSpecialists[3]));
		DPRINTF(k_DBG_AI, ("m_numSpecialists[4]: %d\n", m_numSpecialists[4]));
		DPRINTF(k_DBG_AI, ("m_numSpecialists[5]: %d\n", m_numSpecialists[5]));
		DPRINTF(k_DBG_AI, ("m_numSpecialists[6]: %d\n", m_numSpecialists[6]));
		DPRINTF(k_DBG_AI, ("m_specialistDBIndex[0]: %d\n", m_specialistDBIndex[0]));
		DPRINTF(k_DBG_AI, ("m_specialistDBIndex[1]: %d\n", m_specialistDBIndex[1]));
		DPRINTF(k_DBG_AI, ("m_specialistDBIndex[2]: %d\n", m_specialistDBIndex[2]));
		DPRINTF(k_DBG_AI, ("m_specialistDBIndex[3]: %d\n", m_specialistDBIndex[3]));
		DPRINTF(k_DBG_AI, ("m_specialistDBIndex[4]: %d\n", m_specialistDBIndex[4]));
		DPRINTF(k_DBG_AI, ("m_specialistDBIndex[5]: %d\n", m_specialistDBIndex[5]));
		DPRINTF(k_DBG_AI, ("m_specialistDBIndex[6]: %d\n", m_specialistDBIndex[6]));
		DPRINTF(k_DBG_AI, ("m_sizeIndex: %d\n", m_sizeIndex));
		DPRINTF(k_DBG_AI, ("m_workerFullUtilizationIndex: %d\n", m_workerFullUtilizationIndex));
		DPRINTF(k_DBG_AI, ("m_workerPartialUtilizationIndex: %d\n", m_workerPartialUtilizationIndex));
		DPRINTF(k_DBG_AI, ("m_useGovernor: %d\n", m_useGovernor));
		DPRINTF(k_DBG_AI, ("m_buildListSequenceIndex: %d\n", m_buildListSequenceIndex));
		DPRINTF(k_DBG_AI, ("m_garrisonOtherCities: %d\n", m_garrisonOtherCities));
		DPRINTF(k_DBG_AI, ("m_garrisonComplete: %d\n", m_garrisonComplete));
		DPRINTF(k_DBG_AI, ("m_currentGarrison: %d\n", m_currentGarrison));
		DPRINTF(k_DBG_AI, ("m_neededGarrison: %d\n", m_neededGarrison));
		DPRINTF(k_DBG_AI, ("m_currentGarrisonStrength: %f\n", m_currentGarrisonStrength));
		DPRINTF(k_DBG_AI, ("m_neededGarrisonStrength: %f\n", m_neededGarrisonStrength));
		DPRINTF(k_DBG_AI, ("m_sellBuilding: %d\n", m_sellBuilding));
		DPRINTF(k_DBG_AI, ("m_buyFront: %d\n", m_buyFront));
		DPRINTF(k_DBG_AI, ("m_max_food_from_terrain: %d\n", m_max_food_from_terrain));
		DPRINTF(k_DBG_AI, ("m_max_prod_from_terrain: %d\n", m_max_prod_from_terrain));
		DPRINTF(k_DBG_AI, ("m_max_gold_from_terrain: %d\n", m_max_gold_from_terrain));
		DPRINTF(k_DBG_AI, ("m_growth_rate: %d\n", m_growth_rate));
		DPRINTF(k_DBG_AI, ("m_overcrowdingCoeff: %d\n", m_overcrowdingCoeff));
		DPRINTF(k_DBG_AI, ("m_starvation_turns: %d\n", m_starvation_turns));
		DPRINTF(k_DBG_AI, ("m_cityStyle: %d\n", m_cityStyle));
		DPRINTF(k_DBG_AI, ("m_pos: %d\n", m_pos));
		DPRINTF(k_DBG_AI, ("m_is_rioting: %d\n", m_is_rioting));
		DPRINTF(k_DBG_AI, ("m_build_category_before_load_queue: %d\n", m_build_category_before_load_queue));
		DPRINTF(k_DBG_AI, ("m_scie_lost_to_crime: %d\n", m_scie_lost_to_crime));
		DPRINTF(k_DBG_AI, ("m_gross_food_before_bonuses: %d\n", m_gross_food_before_bonuses));
		DPRINTF(k_DBG_AI, ("m_gross_prod_before_bonuses: %d\n", m_gross_prod_before_bonuses));
		DPRINTF(k_DBG_AI, ("m_happinessAttackedBy: %d\n", m_happinessAttackedBy));
		DPRINTF(k_DBG_AI, ("m_home_city: %d\n", m_home_city));
		DPRINTF(k_DBG_AI, ("m_min_turns_revolt: %d\n", m_min_turns_revolt));
		DPRINTF(k_DBG_AI, ("m_build_queue: %x\n", m_build_queue));
		DPRINTF(k_DBG_AI, ("m_tradeSourceList: %x\n", m_tradeSourceList));
		DPRINTF(k_DBG_AI, ("m_tradeDestinationList: %x\n", m_tradeDestinationList));
		DPRINTF(k_DBG_AI, ("m_collectingResources: %x\n", m_collectingResources));
		DPRINTF(k_DBG_AI, ("m_sellingResources: %x\n", m_sellingResources));
		DPRINTF(k_DBG_AI, ("m_buyingResources: %x\n", m_buyingResources));
		DPRINTF(k_DBG_AI, ("m_happy: %x\n", m_happy));
		DPRINTF(k_DBG_AI, ("m_name: %x\n", m_name));
		DPRINTF(k_DBG_AI, ("m_distanceToGood: %x\n", m_distanceToGood));
		DPRINTF(k_DBG_AI, ("m_defensiveBonus: %f\n", m_defensiveBonus));
		DPRINTF(k_DBG_AI, ("m_ringFood: %d\n", m_ringFood));
		DPRINTF(k_DBG_AI, ("m_ringProd: %d\n", m_ringProd));
		DPRINTF(k_DBG_AI, ("m_ringGold: %d\n", m_ringGold));
		DPRINTF(k_DBG_AI, ("m_ringSizes: %d\n", m_ringSizes));
		DPRINTF(k_DBG_AI, ("m_bonusFoodCoeff: %f\n", m_bonusFoodCoeff));
		DPRINTF(k_DBG_AI, ("m_bonusFood: %f\n", m_bonusFood));
		DPRINTF(k_DBG_AI, ("m_bonusProdCoeff: %f\n", m_bonusProdCoeff));
		DPRINTF(k_DBG_AI, ("m_bonusProd: %f\n", m_bonusProd));
		DPRINTF(k_DBG_AI, ("m_bonusGoldCoeff: %f\n", m_bonusGoldCoeff));
		DPRINTF(k_DBG_AI, ("m_bonusGold: %f\n", m_bonusGold));
		DPRINTF(k_DBG_AI, ("m_bonusScieCoeff: %f\n", m_bonusScieCoeff));
		DPRINTF(k_DBG_AI, ("m_bonusScie: %f\n", m_bonusScie));
		DPRINTF(k_DBG_AI, ("m_cityRadiusOp: %d\n", m_cityRadiusOp));
		DPRINTF(k_DBG_AI, ("m_killList: %x\n", m_killList));
		DPRINTF(k_DBG_AI, ("m_radiusNewOwner: %d\n", m_radiusNewOwner));
		DPRINTF(k_DBG_AI, ("m_tilecount: %d\n", m_tilecount));
		DPRINTF(k_DBG_AI, ("m_whichtile: %d\n", m_whichtile));
		DPRINTF(k_DBG_AI, ("m_tempGoodAdder: %x\n", m_tempGoodAdder));
		DPRINTF(k_DBG_AI, ("m_tempGood: %d\n", m_tempGood));
		DPRINTF(k_DBG_AI, ("m_tempGoodCount: %d\n", m_tempGoodCount));
		DPRINTF(k_DBG_AI, ("m_sentInefficientMessageAlready: %d\n", m_sentInefficientMessageAlready));
		DPRINTF(k_DBG_AI, ("m_culture: %d\n", m_culture));
		DPRINTF(k_DBG_AI, ("m_secthappy: %d\n", m_secthappy));
		DPRINTF(k_DBG_AI, ("\n"));
	}
#endif
};

uint32 CityData_CityData_GetVersion(void);
#endif
