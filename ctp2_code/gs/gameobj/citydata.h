//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
// _MSC_VER		
// - When defined, allows Microsoft C++ extensions.
// - When not defined, generates standard C++.
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks 
//       between #else and #endif are modified Apolyton code. The blocks 
//       between #if and #else are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragma once commented out
// - Enable reading of files created with the Activision 1.1 patch.
// - Prevent crash when settling in the Alexander scenario.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CITY_DATA_H__
#define __CITY_DATA_H__ 1

#include "Unit.h"
#include "BldQue.h"


#include "UnitDynArr.h"
#include "TradeDynArr.h"
#include "Resources.h"
#include "GWRecord.h"

#include "CityRadius.h"

#include "MapPoint.h"

#define k_CITYDATA_VERSION_MAJOR	0								
#define k_CITYDATA_VERSION_MINOR	0								

#define CITY_TILE_SIZE				21								

#define k_CITY_RADIUS 2

class CivArchive ;
class Happy;
class SlicObject;
class Cell;


#define k_PEOPLE_PER_POPULATION 10000

enum OPTIMISE_STATE {
	OPTIMISE_STATE_NONE,
	OPTIMISE_STATE_FOOD,
	OPTIMISE_STATE_PRODUCTION,
	OPTIMISE_STATE_GOLD,
	OPTIMISE_STATE_HAPPINESS,
	OPTIMISE_STATE_SCIENCE,
	OPTIMISE_STATE_INVALID,
} ;

enum CITY_ATTITUDE { 
   CITY_ATTITUDE_CONTENT,
   CITY_ATTITUDE_WE_LOVE_THE_KING, 
   CITY_ATTITUDE_HAPPY,
   CITY_ATTITUDE_DISORDER,
};

enum RADIUS_OP {
    RADIUS_OP_UKNOWN = -1, 
	RADIUS_OP_REMOVE_IMPROVEMENTS = 0,
	RADIUS_OP_KILL_UNITS,
	RADIUS_OP_RESET_OWNER,
	RADIUS_OP_KILL_TILE,
	RADIUS_OP_ADD_GOODS,
	RADIUS_OP_COUNT_GOODS,
};

enum CONVERTED_BY {
	CONVERTED_BY_CLERIC,
	CONVERTED_BY_TELEVANGELIST,
	CONVERTED_BY_NOTHING
};

enum UPRISING_CAUSE {
	UPRISING_CAUSE_NONE,
	UPRISING_CAUSE_SLAVE_STARVED,
	UPRISING_CAUSE_UNGUARDED_SLAVES,
	UPRISING_CAUSE_INCITED,
	UPRISING_CAUSE_INTERNAL, 
};



enum POP_TYPE {
	POP_WORKER,
	POP_SCIENTIST,
	POP_ENTERTAINER,
	POP_FARMER,
	POP_LABORER,
	POP_MERCHANT,
	POP_SLAVE,
	POP_MAX
};

struct TileValue; 
struct TerrainValue;

class CityData : public CityRadiusCallback { 

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
    sint32 m_trade; 
    sint32 m_trade_lost_to_crime;
    sint32 m_gross_trade; 
	sint32 m_goldFromTradeRoutes;
	sint32 m_goldLostToPiracy;
    sint32 m_science; 
    sint32 m_luxury; 
    CITY_ATTITUDE m_city_attitude;
	
	sint32 m_collected_production_this_turn;						
    sint32 m_gross_production_this_turn;							
    sint32 m_shields_this_turn ;									
    sint32 m_shields_lost_to_crime;

    uint64 m_built_improvements;									
	uint64 m_builtWonders;											

    double  m_food_delta;
    double  m_gross_food_this_turn;									
    double	m_food_produced_this_turn;								
    double  m_food_lost_to_crime;									
	double	m_food_consumed_this_turn ;								

	
	sint32	m_total_pollution ;										
	sint32	m_cityPopulationPollution ;
	sint32	m_cityIndustrialPollution ;
	sint32	m_foodVatPollution ;
	sint32	m_cityPollutionCleaner ;
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
			m_improvementWasBuilt ;									

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

	uint8 m_probeRecoveredHere;

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
	sint32 m_max_production_from_terrain;
	sint32 m_max_trade_from_terrain;

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
#if !defined(ACTIVISION_ORIGINAL)                       
	uint8		m_unknown_from_patch;
#endif
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
	Happy	*m_happy ;
	
	MBCHAR	m_name[k_MAX_NAME_LEN] ;								

	sint32 *m_distanceToGood;

	
	double m_defensiveBonus;
 	
	

	
	
	RADIUS_OP m_cityRadiusOp;
	UnitDynamicArray *m_killList;
	sint32 m_radiusNewOwner;
	sint32 m_tilecount, m_whichtile;
	SlicObject *m_tempGoodAdder;
	sint32 m_tempGood, m_tempGoodCount;
	BOOL m_sentInefficientMessageAlready;
	
	

	
	
#ifdef _DEBUG
    BOOL m_ignore_happiness; 
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

	CityData(CivArchive &archive) ;									
    CityData(PLAYER_INDEX o, Unit hc, const MapPoint &center_pos);
	CityData(CityData *copy);
	~CityData();

	void Copy(CityData *copy);
    void Serialize(CivArchive &archive) ;

	void Initialize(sint32 settlerType);
	void NetworkInitialize();

    void PrepareToRemove(const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy);

    uint64 GetImprovements() const { return m_built_improvements; }; 
    Unit GetHomeCity() const { return m_home_city; };
    sint32 GetOwner() const { return m_owner; }
	void ResetCityOwner(sint32 owner);

    sint32 GetStoredCityFood() const { return m_accumulated_food; } 
    sint32 GetNetCityFood() const { return sint32(m_food_delta); } 
    sint32 GetGrossCityFood() const { return sint32(m_gross_food_this_turn); } 

	
	sint32 GetStarvationTurns() const {return m_starvation_turns;}

    double GetProducedFood() const { return m_food_produced_this_turn; }
    double GetConsumedFood() const { return m_food_consumed_this_turn; }

    
    void SetShieldstore (sint32 s);    
	void AddShields(sint32 s);

	
	sint32 ComputeGrossProduction( double workday_per_person, sint32 collected_production, sint32 & crime_loss, sint32 & franchise_loss ) const;

    sint32 ProcessProduction(bool projectedOnly);
	
    double ProjectMilitaryContribution();
    sint32 GetStoredCityProduction() const { return m_shieldstore; } 
    sint32 GetNetCityProduction() const { return m_shields_this_turn; } 
    sint32 GetGrossCityProduction() const { return m_gross_production_this_turn; } 
    sint32 GetCollectedProduction() const { return m_collected_production_this_turn; } 

	
	sint32 ComputeMaterialsPaid(double percent_terrain);

    void PayFederalProduction (double percent_military,
							   sint32 &mil_paid, 
							   double percent_terrain, 
							   sint32 &mat_paid);
    void PayFederalProductionAbs (sint32 mil_paid, 
								  double percent_mat, 
								  sint32 &mat_paid);

    void ShieldSupportTroops();
    void AddShieldsToBuilding();

    void SetMaterialContribution(BOOL on) { m_contribute_materials = on;}
	BOOL GetMaterialContribution() const { return m_contribute_materials;}

    void SetMilitaryContribution(BOOL on) { m_contribute_military = on; }
    BOOL GetMilitaryContribution() { return m_contribute_military; }

    
    void IncrementBuildQueue();
	sint32 LoadQueue(const MBCHAR *file) ;
	sint32 SaveQueue(const MBCHAR *file) ;

	
	BuildQueue *GetBuildQueue(void) { return &m_build_queue; }

    

    void CollectResources();
	sint32 ProcessFood();
    void EatFood();
    int FoodSupportTroops();
	sint32 GetBuildingOvercrowdingBonus();
	sint32 GetBuildingMaxPopIncrease();
	void CalculateGrowthRate();
    sint32 GrowOrStarve();
	double GetFoodRequired() const;
	double GetFoodRequiredPerCitizen() const;

	sint32 GetAccumulatedFood() { return m_accumulated_food; }
	sint32 SubtractAccumulatedFood(sint32 amount);

    

    void SetTerrainEmptyRow(const sint32 row_len, sint32 &n, DynamicArray<TerrainValue> &val);
    void GotoRowStart(const MapPoint &center, const WORLD_DIRECTION d1, 
        const WORLD_DIRECTION d2, const sint32 row_len, sint32 &n, DynamicArray<TerrainValue> &val);
    void GetOpenTerrainValuesRow(sint32 len, MapPoint &pos, sint32 &n, DynamicArray<TerrainValue> &val);
    void GetOpenTerrainValues(const MapPoint &center, sint32 &n, DynamicArray<TerrainValue> &val);
    BOOL CityCanHavePopAt(MapPoint &pos) const;

	void UpdateSprite(void);
	
	BOOL IsWatchful() const;
	void SetWatchful();
	void ModifySpecialAttackChance(UNIT_ORDER_TYPE attack, double &chance);

	void RemoveOneSlave(PLAYER_INDEX p);
	BOOL AdjustedBestTile(const double foodCoef, const double productionCoef, const double resourceCoef, MapPoint &bestPos) ;

    
    void CollectOtherTrade(const BOOL projectedOnly, BOOL changeResources = TRUE);
    void CollectPopScience(bool projectedOnly);
    void CheckTopTen();
	sint32 SupportBuildings(bool projectedOnly);
	sint32 GetSupportBuildingsCost();

	void AddTradeRoute(TradeRoute &route, BOOL fromNetwork);
	void DelTradeRoute(TradeRoute route);
    sint32 IsUsedInTradeRoute(const MapPoint &qpos);
	sint32 CalculateTradeRoutes(BOOL projectedOnly);
	sint32 CalculateGoldFromResources();
	void AddTradeResource(ROUTE_TYPE type, sint32 resource);
#ifdef CTP1_TRADE
	sint32 GetResourceCount(sint32 resource);
	sint32 GetLocalResourceCount(sint32 resource);
#endif

	sint32 CalcWages(sint32 wage);
    BOOL PayWages(sint32 wage, bool projectedOnly);
	sint32 GetWagesNeeded(const sint32 & wages_per_person) const;
	sint32 GetWagesNeeded();

	sint32 GetGoldFromTradeRoutes() const { return m_goldFromTradeRoutes; }
	sint32 GetGoldLostToPiracy() const { return m_goldLostToPiracy; }
	TradeDynamicArray* GetTradeSourceList() { return (&m_tradeSourceList); }
	TradeDynamicArray* GetTradeDestinationList() { return (&m_tradeDestinationList); }

	bool BreakOneSourceRoute(ROUTE_TYPE type, sint32 resource);

    sint32 GetNetCityGold() const { return m_trade; } 
    sint32 GetGrossCityGold() const { return m_gross_trade; } 



    void ResetConquestDistress(double new_distress);


    void ImprovementHealUnitsInCity() const;
    void ImprovementRefuelUnitsInCity() const;
    sint32 ImprovementCanRefuel(const Unit &u) const;
	void AddRawMaterials(sint32 amount);
	sint32 GetRawMaterials() const;
	void UseRawMaterials(sint32 amt);
	BOOL IsConnected(MapPoint &point, uint8* array = NULL, sint32 w = 0, sint32 h = 0);



    void CalcPollution(void) ;
    void DoLocalPollution();





    void CheatBuildFirstItem();

	void InitBeginTurnVariables();
	void DoTurnCounters();
	void TryToBuild();
    sint32 BeginTurn(); 
	void EndTurn();

    sint32 GetNumPop() const; 






	BOOL BuildUnit(sint32 type);
	BOOL BuildImprovement(sint32 type);
	BOOL BuildWonder(sint32 type);
	BOOL BuildEndGame(sint32 type);

	void AddWonder(sint32 type);
    BOOL ChangeCurrentlyBuildingItem(sint32 category, sint32 item_type);

	uint64 GetBuiltWonders() const { return m_builtWonders; }

    void DestroyCapitol(); 
	void SetCapitol(const BOOL delay_registration);
	void DestroyImprovement(sint32 type);
	void NewGovernment(sint32 government_type);

    double GetDefendersBonus() const;
	double GetDefendersBonusNoWalls() const;

	BOOL ShouldRevolt(const sint32 inciteBonus) ;					
	void Revolt(sint32 &playerToJoin, BOOL causeIsExternal = FALSE) ;
	void TeleportUnits(const MapPoint &pos,  BOOL &revealed_foreign_units, 
                             BOOL &revealed_unexplored, sint32 foreigner) ;						
	void StopTradingWith(const PLAYER_INDEX bannedRecipient) ;		
	Happy *GetHappy(void) { return (m_happy) ; }					
	
	MBCHAR *GetName(void) { return (m_name) ; }						
	void SetName(const MBCHAR *name);

    void GetPop(sint32 &p)const { p = m_population; }

    void GetTradeStats(sint32 &g, sint32 &l, sint32 &sci) const
    {
        g = m_trade; 
        l = m_luxury; 
        sci = m_science; 
    }

	void GetPollutionStats(sint32 &pollution) const {
		pollution = m_total_pollution ;
		}

    void GetProductionStats(sint32 &s, sint32 &t, sint32 &f) const {
        s = m_shieldstore; 
        t = m_trade; 
        f = 0;  
    }
	void GetProjectedProductionDeltas(sint32 &s, sint32 &t, sint32 &f,
									  sint32 &science);
	void GetProjectedGrossProduction(sint32 &prod, sint32 &prod_crime);
	void GetProjectedGrossFood(sint32 &food, sint32 &food_crime);
	void GetProjectedGrossGold(sint32 &gold, sint32 &gold_crime);

	void GetDetailedProjectedHappinessCrime(double &hap, double &crime);
	void GetProjectedHappinessCrime(double &hap, double &crime, sint32 &delta_martial_law);
	void GetProjectedFood(sint32 &food) ;
	void GetDetailedProjectedFood(sint32 &food, sint32 &foodBeforeCrime,
								  sint32 &foodBeforeCrimeAndEaten);

	void GetDetailedProjectedProduction(sint32 &production, sint32 &productionBeforeCrime) const;
	void GetProjectedProduction(sint32 &production) ;
	void GetDetailedProjectedTradeScience(sint32 &projected_gross_gold, 
										  sint32 &projected_net_gold,
										  sint32 &science,
										  sint32 &popScience,
										  sint32 &projected_gold_before_crime,
										  sint32 &projected_wages);
	void GetProjectedTrade(sint32 &projected_gross_gold, sint32 &projected_net_gold) ;
	sint32 GetDetailedProjectedTradeDeltaAdjustResource(sint32 resource, sint32 adjust);

	void CollectTradeFromCell(MapPoint &popPos, sint32 &trade) ;
	void GetProjectedTradeFromCell(MapPoint &pos, sint32 &trade) ;

	void GetConsumptionStats(sint32 &foodConsumed) const {
		foodConsumed = (sint32)(m_food_consumed_this_turn) ;
	}

	
	sint32 GetTradeCrime() const { return m_trade_lost_to_crime; }
	sint32 GetProdCrime() const { return m_shields_lost_to_crime; }
	void GetFoodCrime( sint32 &foodCrime ) const {
		foodCrime = (sint32)(m_food_lost_to_crime);
	}

	BOOL GetIsRioting(void) const { return m_is_rioting; }

    void CalcHappiness(sint32 &virtualGoldSpent, BOOL isFirstPass);
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
	 BOOL IsCelebratingHappiness(void) const;

	double GetImprovementCrimeMod() const;
	sint32 GetImprovementPeaceMod() const;
	void AddHappyTimer(sint32 turns, double adjust, HAPPY_REASON reason);
	sint32 GetPollution() const;
	sint32 GetPopulationPollution() const;
	sint32 GetProductionPollution() const;
	void RemoveImprovements(const MapPoint &pos, sint32 owner);
	void CityToPark(sint32 agressor);

	void CityRadiusFunc(const MapPoint &pos);
	void GetNuked(UnitDynamicArray &killList);

	BOOL SafeFromNukes() const;

	BOOL HasAirport() const;
	void UseAirport();
	sint32 AirportLastUsed() const;
	BOOL HasCityWalls() const;
	BOOL HasForceField() const;

	BOOL HasBeenSpiedUpon() const;
	void SetSpiedUpon();

	void CityNullifyWalls();
	BOOL IsCapitol() const;

	void MakeFranchise(sint32 player);
	sint32 GetFranchiseOwner() const { return m_franchise_owner;}
	sint32 GetFranchiseTurnsRemaining() const;
	void SetFranchiseTurnsRemaining(sint32 turns);

#ifdef _DEBUG
    void SetIgnoreHappiness(BOOL v) { m_ignore_happiness = v; }
#endif


	void DoUprising(UPRISING_CAUSE cause);
	void FinishUprising(Army &sa, UPRISING_CAUSE cause);
	void CleanupUprising(Army &sa);

	void Plague(sint32 player);
	void BioInfect( sint32 player );
	void NanoInfect( sint32 player );
	BOOL IsBioImmune() const;
	BOOL IsNanoImmune() const;
	void SpreadBioTerror();
	void SpreadNanoTerror();

	BOOL IsNanoInfected() const { return m_nanoInfectionTurns > 0; }
	BOOL IsBioInfected() const { return m_bioInfectionTurns > 0; }
	BOOL IsFranchised() const { return m_franchise_owner >= 0; }
	BOOL IsConverted() const { return m_convertedTo >= 0; }
	sint32 GetConvertedTo(void) const { return (m_convertedTo) ; }
	sint32 GetConvertedGold(void) const { return (m_convertedGold) ; }

	void ConvertTo(sint32 player, CONVERTED_BY by);
	double TheologicalModifier() const;
	void Unconvert(BOOL makeUnhappy = TRUE);
	sint32 IsConvertedTo() const { return m_convertedTo; }

	BOOL HasResource(sint32 resource) const;
#ifdef CTP1_TRADE
	const Resources *GetResources() const { return &m_resources; }
	const Resources *GetLocalResources() const { return &m_localResources; }
#else
	const Resources *GetCollectingResources() const { return &m_collectingResources; }
	const Resources *GetBuyingResources() const { return &m_buyingResources; }
#endif

	BOOL IsLocalResource( sint32 resource ) const;

	
	
	bool GetResourceTradeRoute(sint32 resource, TradeRoute & route) const;

	
	bool IsSellingResourceTo(sint32 resource, Unit & destination) const;

	sint32 GetOvertimeCost();
	BOOL BuyFront();
	BOOL AlreadyBoughtFront() { return m_buyFront; }
	void RemoveFront();
	void BuildWhat() const;
	sint32 HowMuchLonger() const;
	sint32 HowMuchLonger( sint32 productionRemaining ) const;
	void SellBuilding(sint32 which, BOOL byChoice = TRUE);
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
	BOOL HasPopChanged(void) const { Assert(FALSE); return FALSE; }
	BOOL HasPopGrown(void) const { Assert(FALSE); return FALSE; }
	BOOL HasPopStarved(void) const { Assert(FALSE); return FALSE; }
	void IndicateImprovementBuilt(void) { m_improvementWasBuilt = TRUE ; }
	BOOL WasImprovementBuilt(void) const { return (m_improvementWasBuilt) ; }
	void IndicateTerrainImprovementBuilt(void) { m_terrainImprovementWasBuilt=TRUE ; }
	BOOL WasTerrainImprovementBuilt(void) const { return (m_terrainImprovementWasBuilt) ; }
	void IndicateHappinessAttacked(void) { m_happinessAttacked = TRUE ; }
	BOOL WasHappinessAttacked(void) const;
	void IndicateTerrainPolluted(void) { m_terrainWasPolluted = TRUE ; }
	BOOL WasTerrainPolluted(void) const { return (m_terrainWasPolluted) ; }
	BOOL CanHavePopType(sint32 type) const;
	BOOL CanHaveUIPopType(sint32 uitype) const;
	sint32 GetScience(void) const { return (m_science) ; }
	sint32 GetScienceFromCommerce(void) const;
	BOOL CanBuildUnit(sint32 type) const;
	BOOL CanBuildBuilding(sint32 type) const;

	BOOL CanBuildWonder(sint32 type) const;
	void RemoveWonderFromQueue(sint32 type);
	void Injoin(sint32 player);
	BOOL IsInjoined() const;
	sint32 InjoinedBy() const { return m_injoinedBy; }

    BOOL HaveImprovement(const sint32 type) const;
    sint32 GetPreferedPopType() const;

	void RecalculateResources();
	sint32 CountTradeWith(PLAYER_INDEX player) const;

	double IsProtectedFromSlavery() const;
	void NotifyAdvance(AdvanceType advance);

	void ContributeScience(double incomePercent,
						   double &addscience, double &subgold);
	sint32 TurnsToNextPop();
	sint32 FreeSlaves();

	void AddEndGameObject(sint32 type);

	BOOL SendSlaveTo(Unit &dest);
	void SetFullHappinessTurns(sint32 turns);

	void AiStartMovingPops();
	void AiDoneMovingPops();

	sint32 GetHappinessFromPops();
	sint32 GetScienceFromPops();
	sint32 GetGoldFromPops();
	sint32 GetProductionFromPops();

	sint32 GetIncomingTrade() const;
	sint32 GetOutgoingTrade() const;
	void FinishBuilding();
	void DestroyRandomBuilding();
	void AddConversionUnhappiness(sint32 who);

	void InsertCapitalization();
	void InsertInfrastructure();
	void BuildCapitalization();
	BOOL CanBuildCapitalization() const;
	void BuildInfrastructure();
	BOOL CanBuildInfrastructure() const;
	BOOL IsBuildingCapitalization() { return m_buildCapitalization; }
	BOOL IsBuildingInfrastructure() { return m_buildInfrastructure; }
	void StopInfrastructureCapitalization();
	void EliminateNukes();

	void KillAllTradeRoutes();

	void RemoveEndGameObjects();

	BOOL NeedToDoUprising() const;
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

	void SetProbeRecoveredHere(BOOL recovered);
	BOOL GetProbeRecoveredHere() { return (BOOL)m_probeRecoveredHere; }

	BOOL HasSleepingUnits();

	bool IsProtectedFromConversion();
	sint32 GetGoldFromCapitalization() { return m_gold_from_capitalization; }

	sint32 CityGrowthCoefficient();
	void DestroyWonder(sint32 which);

	BOOL CapturedThisTurn() const { return m_capturedThisTurn; }
	void SetSentInefficientMessage() { m_sentInefficientMessageAlready = TRUE; }

	void BuildFront();

	
	sint32 GetValue() const;

	sint32 GetSizeIndex() const { return m_sizeIndex; }
	void MakeCitizen(PopDBIndex type, const MapPoint &center_pos,
					 sint32 origOwner);

	
	void GetFullAndPartialRadii(sint32 &fullRadius, sint32 &partRadius);
	sint32 PopCount() const;
	sint32 SpecialistCount(POP_TYPE type) const;
	sint32 SlaveCount() const;
	sint32 WorkerCount() const;
	sint32 ScientistCount() const;
	sint32 FarmerCount() const;
	sint32 EntertainerCount() const;
	sint32 MerchantCount() const;
	sint32 LaborerCount() const;
	
	
	void ComputeSizeIndexes(const sint32 & workers, sint32 & size_index, sint32 & full_index, sint32 & partial_index) const;

	
	sint32 GetBestSpecialist(const POP_TYPE & type) const;

	void AdjustSizeIndices();
	void ChangeSpecialists(POP_TYPE type, sint32 delta);
	void ChangePopulation(sint32 delta);
	void FindBestSpecialists();

	
	BOOL GetUseGovernor() const;
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
	sint32 GetMaxProductionFromTerrain() { return m_max_production_from_terrain; }
	sint32 GetMaxCommerceFromTerrain() { return m_max_trade_from_terrain; }

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

	void DoSupport(bool projectedOnly);
	void SplitScience(bool projectedOnly);
	sint32 GetProjectedScience();
	sint32 GetFounder() const;

#if !defined(ACTIVISION_ORIGINAL)
	sint32 GetCityStyle() const;
#endif
	void SetCityStyle(sint32 style); 
}; 

uint32 CityData_CityData_GetVersion(void) ;
#endif


