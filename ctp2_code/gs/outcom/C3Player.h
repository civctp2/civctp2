










#pragma once
#ifndef __C3PLAYER_H__
#define __C3PLAYER_H__

#include "ic3player.h"
class Player;
class CivArchive; 
class CityData; 
class Army;
enum AGREEMENT_TYPE;

#define k_REGARD_INSANE_HATRED 10
#define k_REGARD_HOTWAR	20
#define k_REGARD_COLDWAR 40
#define k_REGARD_FRIENDLY 60
#define k_REGARD_LOVE	80

class C3Player : public IC3Player
{
	ULONG m_refCount;
	
	
	Player *m_ptr;
    sint32 m_owner; 

public:
	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

    	
    C3Player(sint32 idx); 
    C3Player(CivArchive &archive);
    void Serialize(CivArchive &archive);

    
   
    STDMETHODIMP_(BOOL) GetArmyPos(uint32 army_id,  BOOL *is_unknown_id, MapPointData *start);
    STDMETHODIMP_(BOOL) GetArmyXYPos(PLAYER_INDEX player_index, uint32 army_id,  BOOL *is_unknown_id, MapPointData *start);
    STDMETHODIMP_(BOOL) Norm2XY(const MapPointData &norm_pos, MapPointData &xy_pos);
    STDMETHODIMP_(BOOL) XY2Norm(const MapPointData &xy_pos, MapPointData &norm_pos);
    STDMETHODIMP_(BOOL) GetArmyCurMinMovementPoints(uint32 army_id,  BOOL *is_unknown_id, double *cur);
    STDMETHODIMP_(BOOL) GetArmyMinMovementPoints(uint32 army_id,  BOOL *is_unknown_id, double *min_move);
    STDMETHODIMP_(BOOL) ArmySettle(uint32 army_id,  BOOL *is_unknown_id);
    
    STDMETHODIMP_(BOOL) ArmyCanEnter(uint32 army_id,  BOOL *is_unknown_id, MapPointData *pos, 
         BOOL *move_to_many_units_dest, BOOL *move_violated_zoc, BOOL *move_violated_movetype, 
        BOOL *move_out_of_fuel);

    STDMETHODIMP_ (BOOL) UnitTypeCanEnter(sint32 unit_type,
        MapPointData *pos); 

	STDMETHODIMP_ (BOOL) MoveTypeCanEnter(uint32 move_type, MapPointData *pos);

    STDMETHODIMP_(BOOL) ArmyMoveTo(uint32 army_id,  BOOL *is_unknown_id, 
        MapPointData *pos, 
        BOOL *did_move, BOOL *i_died, BOOL *move_violated_zoc, 
        BOOL *revealed_foreign_units, BOOL *revealed_unexplored,
        BOOL *is_transported, BOOL *out_of_fuel);

    STDMETHODIMP_ (BOOL) UnitTypeCanSettle (uint32 unit_type, 
        MapPointData *pos); 

    STDMETHODIMP_ (BOOL) ArmyCanSettle (uint32 army_id, BOOL *is_unknon_id, 
         MapPointData *pos); 

    STDMETHODIMP_ (BOOL) ArmyGroup(uint32 add_me, uint32 target, BOOL *is_unknown_id);

    STDMETHODIMP_ (BOOL) ArmyUngroup(uint32 split_me, BOOL *is_unknown_id);

    
    STDMETHODIMP_(BOOL) GetCityPos (uint32 city_id, BOOL *is_unknown_id, 
         MapPointData *pos); 
	STDMETHODIMP_(BOOL) GetCityXYPos (PLAYER_INDEX owner, uint32 city_id, BOOL *is_unknown_id, 
         MapPointData *pos); 
	STDMETHODIMP_(BOOL) CityEnqueueBuildItem (uint32 city_id, BOOL *is_unknown_id, 
         sint32 category, sint32 item_type); 

	STDMETHODIMP_(BOOL) CityChangeBuildItem (uint32 city_id, BOOL *is_unknown_id, 
         sint32 category, sint32 item_type); 

	STDMETHODIMP_ (BOOL) CityBuildQueueLen (uint32 city_id, BOOL *is_unknown_id);
     STDMETHODIMP_ (BOOL) CityChangeCurrentlyBuildingItem (uint32 city_id, BOOL *is_unknown_id, 
        sint32 category, sint32 item_type);
	STDMETHODIMP_ (BOOL) CityBuildQueueFront (uint32 city_id, BOOL *is_unknown_id, 
											   sint32 &category, sint32 &type);
   
     STDMETHODIMP_ (sint32)  CityGetStoredProduction (uint32 city_id, BOOL *is_unknown_id);  
     STDMETHODIMP_ (sint32)  CityGetGrossProduction (uint32 city_id, BOOL *is_unknown_id); 
     STDMETHODIMP_ (sint32)  CityGetNetProduction (uint32 city_id, BOOL *is_unknown_id); 

     STDMETHODIMP_(double) GetTotalProduction(PLAYER_INDEX test_me); 
     STDMETHODIMP_(double) GetTotalUnitCost(PLAYER_INDEX test_me); 

     STDMETHODIMP_ (sint32) CityGetStoredFood (uint32 city_id, BOOL *is_unknown_id); 
     STDMETHODIMP_ (sint32) CityGetGrossFood (uint32 city_id, BOOL *is_unknown_id); 
     STDMETHODIMP_ (sint32) CityGetNetFood (uint32 city_id, BOOL *is_unknown_id); 
     STDMETHODIMP_ (sint32) GetAccumulatedGrowthFood(uint32 city_id, BOOL *is_unknown_id);
     STDMETHODIMP_ (sint32) GetAccumulatedGrowthThreshold(uint32 city_id, BOOL *is_unknown_id);

     STDMETHODIMP_ (sint32)  CityGetGrossGold (uint32 city_id, BOOL *is_unknown_id); 
     STDMETHODIMP_ (sint32)  CityGetNetGold (uint32 city_id, BOOL *is_unknown_id); 

     STDMETHODIMP_ (sint32)  CityGetGrossScience (uint32 city_id, BOOL *is_unknown_id); 

     STDMETHODIMP_ (void) SetMaterialsTax(double mt);
     STDMETHODIMP_ (sint32) GetMaterialsStored ();

     STDMETHODIMP_ (sint32) GetUnusedFreight();
     STDMETHODIMP_ (sint32) GetTotalFreight();
     STDMETHODIMP_ (BOOL) CreateTradeRoute(BOOL *is_unknown_id, uint32 src_city,
        sint32 type_route, sint32 scr_good,   uint32 dest_city);

     STDMETHODIMP_ (void) CancelTradeRoute(sint32 id);
     STDMETHODIMP_ (sint32) CancelCityRoutesToPlayer(uint32 city_id, BOOL *is_unknown_id, 
        PLAYER_INDEX no_trade_for_you);


     STDMETHODIMP_ (void) GetAllRegard(sint32 player_idx, double i_like[k_MAX_PLAYERS]);

     STDMETHODIMP_ (BOOL) CityBuyFront(BOOL *is_unknown_id , uint32 u_city_id);
     STDMETHODIMP_ (sint32) CityGetOvertimeCost(BOOL *is_unknown_id , 
         uint32 u_city_id);

     STDMETHODIMP_ (void) GetGoodCount(BOOL *is_unknown_id, uint32 u_city_id, 
        sint32 type_good, sint32 *local_count, sint32 *total_count); 
    
     STDMETHODIMP_ (double) GetRouteCost(BOOL *is_unknown_id , 
        uint32 u_src_city, uint32 u_dest_city); 

     STDMETHODIMP_ (void) SetWorkdayLevel (sint32 w);
     STDMETHODIMP_ (void) SetWagesLevel (sint32 w);
     STDMETHODIMP_ (void) SetRationsLevel (sint32 w);

     STDMETHODIMP_ (void) GetExpectedWGF (sint32 *w, sint32 *g, sint32 *f); 
     STDMETHODIMP_ (void) SetWGF (sint32 w, sint32 g, sint32 f); 

     STDMETHODIMP_ (sint32) GetWagesPerPerson(); 
     STDMETHODIMP_ (sint32) GetTotalBuildingUpkeep(); 

     STDMETHODIMP_ (void) GetGoldLevels(sint32 *income, sint32 *lost_to_cleric,
         sint32 *lost_to_crime, sint32 *maintenance, sint32 *wages, sint32 *science, sint32 *old_savings, 
         sint32 *current_savings); 

     STDMETHODIMP_ (void) SetReadinessLevel(sint32 level);
     STDMETHODIMP_ (sint32) GetReadinessLevel();
     STDMETHODIMP_ (void) GetReadinessCost(sint32 *total_cost, 
         double *percent_cost);

     STDMETHODIMP_ (BOOL) ArmyGetNumCargo(BOOL *is_unknown_id, uint32 u_id, 
         sint32 *full_slots, sint32 *empty_slots);

	 STDMETHODIMP_ (sint32) AiNumUnitsCanMoveIntoThisTranportEver(BOOL *is_unknown_id,  
        uint32 test_me, uint32 transport);

     STDMETHODIMP_ (BOOL) AiCanMoveArmyIntoThisTranportEver(BOOL *is_unknown_id,  
        uint32 test_me, uint32 transport);

     STDMETHODIMP_ (BOOL) AiCanMoveArmyIntoThisTranportRightNow(
         BOOL *is_unknown_id,  
        uint32 move_me, uint32 transport, BOOL *enough_move, 
        BOOL *adjacent, BOOL *ever);

     STDMETHODIMP_ (BOOL) AiArmyMoveIntoTranport(BOOL *is_unknown_id,  uint32 move_me, 
        uint32 transport, BOOL *is_transported);


     STDMETHODIMP_ (BOOL) AiUnloadAllTransportsInArmy(BOOL *is_unknown_id, 
        uint32 unload_me, MapPointData *dest_pos, 
        BOOL *did_move, BOOL *i_died, 
		BOOL *revealed_foreign_units, BOOL *revealed_unexplored, 
        BOOL *zocViolation, BOOL *is_transported);

     STDMETHODIMP_ (BOOL) GetCargoMovementPoints (BOOL *is_unknown_id, 
         uint32 u_tran_id, double* min_move_point, BOOL* first_move);


     STDMETHODIMP_ (BOOL) Paradrop(BOOL *is_unknown_id, uint32 u_id,  
        const MapPointData *dest_pos, BOOL *all_dropped, BOOL *some_dropped, 
        BOOL *all_died, BOOL *revealed_foreign_units, BOOL *revealed_unexplored);
     
     STDMETHODIMP_ (sint32) GetParadropMaxDistance();

     STDMETHODIMP_ (BOOL) CanParadropNow(BOOL *is_unknown_id, uint32 u_id);


     STDMETHODIMP_ (BOOL) CanBeCargoPodded (BOOL *is_unknown_id, 
         uint32 u_id); 

#ifdef _DEBUG
     STDMETHODIMP_ (void) Dprint(char *str);
#endif


     STDMETHODIMP_ (BOOL) GetCurrentFuel(BOOL *is_unknown_id, uint32 u_id, sint32 *current); 
     STDMETHODIMP_ (sint32) GetAllTileValue(uint32 city_id, BOOL *is_unknown_id, 
        sint32 num_tile, TileUtility *open_tile[k_NUM_CITY_TILES]);


     CityData* GetCityData(uint32 city_id, BOOL *is_unknown);
     Army GetArmyList(uint32 army_id, BOOL *is_unknown_id);

     STDMETHODIMP_ (void) GetCityHappiness(uint32 city_id, BOOL *is_unknown_id, 
        sint32 *old_happines, double *old_crime);
     STDMETHODIMP_ (void) GetCityCrimePrevention(uint32 city_id, BOOL *is_unknown_id, 
        double *cop);

     STDMETHODIMP_ (BOOL) GetCityPopAffect(uint32 city_id, 
         BOOL *is_unknown_id, sint32 *pop_affect);

     STDMETHODIMP_ (long) GetCitySizePopEffect(uint32 city_id, 
         BOOL *is_unknown_id, sint32 *city_size_pop_effect);

     STDMETHODIMP_ (void) SetScienceTax(double s);

     STDMETHODIMP_ (void) CityFindWhatIsBuilt (uint32 city_id, BOOL *is_unknown_id, 
         BOOL is_blg_built[64], BOOL is_wndr_built[64]);

     STDMETHODIMP_ (BOOL) SetGovernmentType(sint32 idx_gov);
     
     STDMETHODIMP_ (sint32) GetGovernmentType ();
     STDMETHODIMP_ (double) GetCityDistFromCapitol (uint32 city_id, 
         BOOL *is_unknown_id);
     STDMETHODIMP_ (void) GetPeaceMovement (double *overseas_defeat, 
         double *home_defeat, double *overseas); 
     
     STDMETHODIMP_ (double) GetCityPollution (uint32 city_id, BOOL *is_unknown_id); 

     STDMETHODIMP_ (BOOL) AttemptSlaveRaid (uint32 u_id, BOOL *is_unknown_id, 
          MapPointData *target_pos);

  	 STDMETHODIMP_ (BOOL) IsSlaveRaidPossible(uint32 u_id, BOOL *is_unknown_id, 
          MapPointData *target_pos);

	 STDMETHODIMP_ (sint32) GetSlaveCount(uint32 city_id, BOOL *is_unknown_id);

     STDMETHODIMP_ (BOOL) FindNearestCity(MapPointData *start_pos, 
         uint32 *nearest_city);

     STDMETHODIMP_ (void) SetArmyDBGString (uint32 u_id, BOOL *is_unknow_id, 
        char *str); 

     STDMETHODIMP_ (void) SetCityDBGString (uint32 u_id, BOOL *is_unknow_id, 
        char *str); 

	STDMETHODIMP_(double) GetPollutionLevel();
	STDMETHODIMP_(sint32) NearTradeRoute(MapPointData *pos);
	STDMETHODIMP_(sint32) GetNumTradeRoutes();
	STDMETHODIMP_(sint32) GetTradeWith(PLAYER_INDEX second);

	STDMETHODIMP_(BOOL) ReformCity(uint32 u_id, BOOL *is_unknown_id, 
								   MapPointData *target_pos);

	STDMETHODIMP_(BOOL) CanReformCity(uint32 u_id, BOOL *is_unknown_id, 
									  MapPointData *target_pos);

	STDMETHODIMP_(BOOL) SueFranchise(uint32 u_id, BOOL *is_unknown_id, 
									 MapPointData *target_pos);

	STDMETHODIMP_(BOOL) CanSueFranchise(uint32 u_id, BOOL *is_unknown_id, 
										MapPointData *target_pos);

	STDMETHODIMP_(BOOL) Bombard(uint32 u_id, BOOL *is_unknown_id, 
								MapPointData *target_pos);

	STDMETHODIMP_(BOOL) CanBombard(uint32 u_id, BOOL *is_unknown_id, 
								   MapPointData *target_pos);

	STDMETHODIMP_ (BOOL) ConvertCity (uint32 u_id, BOOL *is_unknown_id, 
									  MapPointData *target_pos);

	STDMETHODIMP_ (BOOL) CanConvertCity(uint32 u_id, BOOL *is_unknown_id, 
										MapPointData *target_pos);

	STDMETHODIMP_ (BOOL) Rustle(uint32 u_id, BOOL *is_unknown_id, 
							 MapPointData *target_pos);

	STDMETHODIMP_ (BOOL) CanRustle(uint32 u_id, BOOL *is_unknown_id, 
								MapPointData *target_pos);

	STDMETHODIMP_ (BOOL) Pillage(uint32 u_id, BOOL *is_unknown_id, 
							  MapPointData *target_pos);

	STDMETHODIMP_ (BOOL) CanPillage(uint32 u_id, BOOL *is_unknown_id, 
								 MapPointData *target_pos);

	STDMETHODIMP_ (BOOL) Pirate(uint32 u_id, BOOL *is_unknown_id, 
							 MapPointData *target_pos);

	STDMETHODIMP_ (BOOL) CanPirate(uint32 u_id, BOOL *is_unknown_id, 
								MapPointData *target_pos);


	STDMETHODIMP_ (BOOL) IsEnemy(const PLAYER_INDEX & second);

	STDMETHODIMP_ (BOOL) HaveActiveAgreement(const PLAYER_INDEX & second, const AGREEMENT_TYPE &type);




	STDMETHODIMP_ (BOOL) ExecuteAIOrder(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos, uint32 order);

	STDMETHODIMP_ (BOOL)AbleToPlantNukeTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	STDMETHODIMP_ (BOOL)AbleToMakeParkTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	STDMETHODIMP_ (BOOL)AbleToUndergroundRailwayTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	STDMETHODIMP_ (BOOL)AbleToConvertTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	STDMETHODIMP_ (BOOL)AbleToEstablishEmbassyTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	STDMETHODIMP_ (BOOL)AbleToCreateFranchiseTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	STDMETHODIMP_ (BOOL)AbleToAssasinateRulerTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	STDMETHODIMP_ (BOOL)AbleToStealTechnologyTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	STDMETHODIMP_ (BOOL)AbleToInjoinTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	STDMETHODIMP_ (BOOL)AbleToInciteRevolutionTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	STDMETHODIMP_ (BOOL)AbleToCauseUnhappinessTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	STDMETHODIMP_ (BOOL)AbleToExpelTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	STDMETHODIMP_ (BOOL)AbleToSueTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos);
	
	STDMETHODIMP_ (BOOL)HasBuildingType(uint32 city_id, sint32 type);
	STDMETHODIMP_ (void)SellBuilding(uint32 city_id, sint32 type);

	STDMETHODIMP_ (BOOL) HasEmbassyWith(const PLAYER_INDEX &second);

	STDMETHODIMP_ (void) RejectTradeBid(uint32 bidId);
	STDMETHODIMP_ (void) AcceptTradeBid(uint32 bidId);

    STDMETHODIMP_ (sint32) CityTurnsToBuild(uint32 city_id, BOOL *is_unknown_id);
	STDMETHODIMP_ (void) CancelTradeOffer(IC3TradeOffer *offer);
	STDMETHODIMP_ (void) MakeTradeOffer(uint32 cityId, sint32 resource,
										sint32 gold);
	STDMETHODIMP_ (BOOL) PlayerCanSeeCity(THIS_ uint32 city, sint32 otherPlayer);
	STDMETHODIMP_(sint32) UnusedTradePoints();


    STDMETHODIMP_ (sint32) GetCityPopulation (sint32 player, uint32 city_id, BOOL *is_unknown_id);
    STDMETHODIMP_ (sint32) GetCityProduction (sint32 player, uint32 city_id, BOOL *is_unknown_id);
    STDMETHODIMP_ (sint32) GetCityFood (sint32 player, uint32 city_id, BOOL *is_unknown_id);
    STDMETHODIMP_ (sint32) GetCityGold (sint32 player, uint32 city_id, BOOL *is_unknown_id);

	STDMETHODIMP_(BOOL) CityCanBuildUnit(THIS_ uint32 city, sint32 unitType);
	STDMETHODIMP_(BOOL) CityCanBuildBuilding(THIS_ uint32 city, sint32 buildingType);
	STDMETHODIMP_(BOOL) CityCanBuildWonder(THIS_ uint32 city, sint32 wonderType);
	STDMETHODIMP_(BOOL) CityCanBuildEndGameObject(THIS_ uint32 city, sint32 endGameType);
	STDMETHODIMP_(BOOL) CityCanBuildCapitalization(THIS_ uint32 city);

	STDMETHODIMP_(void) AiStartMovingPops(THIS_ uint32 city);
	STDMETHODIMP_(void) AiDoneMovingPops(THIS_ uint32 city);
	STDMETHODIMP_(BOOL) IsOccupiedByForeigner(THIS_ MapPointData *target_pos);

    STDMETHODIMP_(BOOL) GetCapitolPos(sint32 him, MapPointData *cap_pos);

    STDMETHODIMP_(sint32) GetNumAtWarWith(PLAYER_INDEX test_me);

	
	STDMETHODIMP_(double) GetCeaseFire(PLAYER_INDEX test_me, sint32 player_idx);
	STDMETHODIMP_(double) GetAlliance(PLAYER_INDEX test_me, sint32 player_idx);
	STDMETHODIMP_(double) GetWar(PLAYER_INDEX this_ai, sint32 test_player_one, sint32 test_player_two);
	STDMETHODIMP_(double) GetNeutrality(PLAYER_INDEX test_me, sint32 player_idx);

	STDMETHODIMP_(double) GetPlayerIndex(PLAYER_INDEX test_me);

	STDMETHODIMP_(double) GetPlayerType(PLAYER_INDEX test_me);
	STDMETHODIMP_ (BOOL) IsRobotPlayer(PLAYER_INDEX test_me);
	
	STDMETHODIMP_(uint32) CreateUnit(sint32 type, MapPointData *pos, uint32 home_city);

    STDMETHODIMP_(void) GetBestProfessionals(
        uint32 cityId, BOOL *isUnknown, 
        sint32 *pop_best_scientist,     sint32 *pop_best_science, 
        sint32 *pop_best_grunt,     sint32 *pop_best_production, 
        sint32 *pop_best_musician,     sint32 *pop_best_luxury,
        sint32 *pop_best_banker,    sint32 *pop_best_gold
    );

    
    STDMETHODIMP_ (sint32) CountMyTradeToForeignCity(PLAYER_INDEX test_him, 
        uint32 city_id, 
        BOOL *is_unknown_id);
	STDMETHODIMP_(void) SetRegard(PLAYER_INDEX who, sint32 level);

	STDMETHODIMP_(BOOL) HeAgreed(PLAYER_INDEX who, AGREEMENT_TYPE agreement);
    
    STDMETHODIMP_ (void) GetArmyCurrentHP(uint32 id, BOOL *is_unknown_id,
        sint32 *n, sint32 unit_type[100], sint32 unit_hp[100]);

    STDMETHODIMP_ (BOOL) ValidateArmyID(uint32 u_id, sint32 unit_num);

    STDMETHODIMP_ (void) GetCityNumTradeRoutes(PLAYER_INDEX owner, uint32 u_id,  BOOL *is_unknown_id, 
        sint32 *in_num, sint32 *in_max, sint32 *out_num, sint32 *out_max);
	STDMETHODIMP_(BOOL) CanSeeHisCity(uint32 cityId,
									  sint32 cityOwner);
	STDMETHODIMP_ (sint32) GetSlavesPerMilitaryUnit();
	STDMETHODIMP_ (BOOL) BuildCapitalization(uint32 city_id, BOOL *is_unknown_id);

	STDMETHODIMP_(BOOL) ArmyHasLeftMap(uint32 army_id, BOOL *is_unknown_id);
	STDMETHODIMP_ (sint32) GetStage();
	STDMETHODIMP_ (sint32) GetNumberBuilt(sint32 type);
	STDMETHODIMP_ (BOOL) HasLab();

    STDMETHODIMP_ (void) DipLogStr(sint32 player_idx, char *str);
	STDMETHODIMP_ (BOOL) IsActuallyVisible(sint32 player, uint32 army_id);
	STDMETHODIMP_ (double) GetProductionPercentBonus(uint32 city_id, BOOL &is_unknown_id, const PRODUCTION_TYPE &type);
};

#endif
