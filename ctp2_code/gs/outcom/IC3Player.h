










#pragma once
#ifndef __IC3PLAYER_H__
#define __IC3PLAYER_H__

#include <OBJBASE.H>  
#include "IMapPointData.h"
#include "gstypes.h"

typedef sint32 PLAYER_INDEX; 
enum AGREEMENT_TYPE;
enum PRODUCTION_TYPE;

DEFINE_GUID(CLSID_IC3Player, 
     0xb6133fa0,
    0xc94b,
    0x11d1,
    0x85, 0xa7, 0x00, 0x60, 0x08, 0x3a, 0x40, 0x82
);

#undef INTERFACE
#define INTERFACE IC3Player

struct MapPointData; 
struct TileUtility; 
interface IC3TradeOffer;

#define k_MAX_AI_DBG_STR_LEN 50

DECLARE_INTERFACE_(IC3Player, IUnknown)
{
 	

    








    STDMETHOD_(BOOL, GetArmyPos) (THIS_ 
        uint32 army_id,     
        BOOL *is_unknown_id, 
        MapPointData *pos 
        ) PURE;









    STDMETHOD_(BOOL, GetArmyXYPos) (THIS_ 
        PLAYER_INDEX player_index, 
        uint32 army_id, 
        BOOL *is_unknown_id, 
        MapPointData *start 
        ) PURE;








    STDMETHOD_(BOOL, Norm2XY) (THIS_ 
        const MapPointData &norm_pos, 
        MapPointData &xy_pos 
        ) PURE;








    STDMETHOD_(BOOL, XY2Norm) (THIS_ 
        const MapPointData &xy_pos, 
        MapPointData &norm_pos 
        ) PURE;














    STDMETHOD_(BOOL, GetArmyCurMinMovementPoints)(THIS_ 
        uint32 army_id, 
        BOOL *is_unknown_id,  
        double *cur 
        ) PURE;












    STDMETHOD_(BOOL, GetArmyMinMovementPoints)(THIS_ 
        uint32 army_id, 
        BOOL *is_unknown_id, 
        double *min_move 
        ) PURE;









    STDMETHOD_(BOOL, ArmySettle)(THIS_ 
        uint32 army_id, 
        BOOL *is_unknown_id 
        ) PURE;







 
    STDMETHOD_ (BOOL, ArmyCanEnter)(THIS_ 
        uint32 army_id,  
        BOOL *is_unknown_id, 
        MapPointData *pos,  
        BOOL *move_to_many_units_dest, 
                                        
        BOOL *move_violated_zoc, 
                                    
        BOOL *move_violated_movetype,  
                                        
                                        
        BOOL *move_out_of_fuel  
                                
        ) PURE; 


















    STDMETHOD_ (BOOL, UnitTypeCanEnter)(THIS_ 
        sint32 unit_type,  
        MapPointData *pos  
        ) PURE; 









	STDMETHOD_ (BOOL, MoveTypeCanEnter)(THIS_ 
		uint32 move_type,  
		MapPointData *pos  
		) PURE;









     STDMETHOD_ (BOOL, UnitTypeCanSettle) (THIS_ 
         uint32 unit_type, 
         MapPointData *pos 
         ) PURE; 










































































    STDMETHOD_(BOOL, ArmyMoveTo)(THIS_ 
        uint32 army_id,  
        BOOL *is_unknown_id, 
        MapPointData *pos, 
        BOOL *did_move, 
        BOOL *i_died, 
        BOOL *move_violated_zoc,  
        BOOL *revealed_foreign_units, 
                                        
        BOOL *revealed_unexplored, 
        BOOL *is_transported, 
        BOOL *out_of_fuel 
        ) PURE;

    







     STDMETHOD_ (BOOL, ArmyCanSettle) (THIS_ 
         uint32 army_id, 
         BOOL *is_unknon_id, 
         MapPointData *pos 
         ) PURE; 








     STDMETHOD_ (BOOL, ArmyGroup) (
         uint32 add_me, 
         uint32 target,  
         BOOL *is_unknown_id 
         ) PURE;









     STDMETHOD_ (BOOL, ArmyUngroup) (
         uint32 split_me, 
         BOOL *is_unknown_id 
         ) PURE;

     
     






     STDMETHOD_(BOOL, GetCityPos) (THIS_ 
         uint32 city_id, 
         BOOL *is_unknown_id, 
         MapPointData *pos 
         ) PURE; 







     STDMETHOD_(BOOL, GetCityXYPos) (THIS_ 
         PLAYER_INDEX owner,  
         uint32 city_id, 
         BOOL *is_unknown_id, 
         MapPointData *pos 
         ) PURE; 








     STDMETHOD_(BOOL, CityEnqueueBuildItem) (THIS_ 
         uint32 city_id, 
         BOOL *is_unknown_id,  
         sint32 category, 
         
         sint32 item_type
         ) PURE; 

	STDMETHOD_(BOOL, CityChangeBuildItem) (THIS_ uint32 city_id, BOOL *is_unknown_id, 
         sint32 category, sint32 item_type) PURE; 


	STDMETHOD_ (BOOL, CityBuildQueueLen) (THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;







     STDMETHOD_ (sint32,  CityGetStoredProduction) (THIS_ 
         uint32 city_id, 
         BOOL *is_unknown_id 
         ) PURE;  








     STDMETHOD_ (sint32,  CityGetGrossProduction) (THIS_ 
         uint32 city_id,  
         BOOL *is_unknown_id 
         ) PURE; 








     STDMETHOD_ (sint32,  CityGetNetProduction) (THIS_ 
         uint32 city_id, 
         BOOL *is_unknown_id 
         ) PURE; 


     STDMETHOD_ (double, GetTotalProduction) (THIS_ PLAYER_INDEX test_me) PURE; 
     STDMETHOD_ (double, GetTotalUnitCost) (THIS_ PLAYER_INDEX test_me) PURE; 







     STDMETHOD_ (sint32,  CityGetStoredFood) (THIS_ 
         uint32 city_id, 
         BOOL *is_unknown_id 
         ) PURE; 







     STDMETHOD_ (sint32,  CityGetGrossFood) (THIS_ 
         uint32 city_id, 
         BOOL *is_unknown_id 
         ) PURE; 







     STDMETHOD_ (sint32,  CityGetNetFood) (THIS_ 
         uint32 city_id, 
         BOOL *is_unknown_id 
         ) PURE; 







     STDMETHOD_ (sint32,  CityGetGrossGold) (THIS_ 
         uint32 city_id, 
         BOOL *is_unknown_id 
         ) PURE; 







     STDMETHOD_ (sint32,  CityGetNetGold) (THIS_ 
         uint32 city_id, 
         BOOL *is_unknown_id 
         ) PURE; 







     STDMETHOD_ (sint32,  CityGetGrossScience) (THIS_ 
         uint32 city_id, 
         BOOL *is_unknown_id 
         ) PURE; 








     STDMETHOD_ (BOOL, CityChangeCurrentlyBuildingItem) (THIS_ 
         uint32 city_id, 
         BOOL *is_unknown_id, 
        sint32 category,  





         sint32 item_type 
                            
         ) PURE;

	 STDMETHOD_ (BOOL, CityBuildQueueFront) (THIS_ uint32 city_id, BOOL *is_unknown_id, 
											 sint32 &category, sint32 &type) PURE;








     STDMETHOD_ (void, SetMaterialsTax) (THIS_ 
         double mt 
         ) PURE;







     STDMETHOD_ (sint32, GetMaterialsStored) (THIS_) PURE;







     STDMETHOD_ (sint32, GetUnusedFreight) (THIS_) PURE;







     STDMETHOD_ (sint32, GetTotalFreight) (THIS_) PURE;















     STDMETHOD_ (BOOL, CreateTradeRoute) (THIS_ 
         BOOL *is_unknown_id,  
         uint32 src_city,  
         sint32 type_route, 
         	
		    
            

         sint32 scr_good,   
         uint32 dest_city  
         
         )PURE;

      STDMETHOD_ (void, CancelTradeRoute) (THIS_ sint32 trade_id) PURE;
      STDMETHOD_ (sint32, CancelCityRoutesToPlayer) (THIS_ uint32 city_id, BOOL *is_unknown_id, 
        PLAYER_INDEX no_trade_for_you) PURE;


      STDMETHOD_ (void,  GetAllRegard) ( THIS_
              sint32  player_idx, 
              double i_like[k_MAX_PLAYERS]) PURE;










 
     STDMETHOD_ (BOOL, CityBuyFront)(
         THIS_
         BOOL *is_unknown_id, 
         uint32 u_city_id 
         ) PURE;








     STDMETHOD_ (sint32, CityGetOvertimeCost)(
         THIS_
         BOOL *is_unknown_id, 
         uint32 u_city_id 
         ) PURE;







     STDMETHOD_ (void,  GetGoodCount) (BOOL *is_unknown_id , uint32 u_city_id, 
        sint32 type_good, sint32 *local_count, sint32 *total_count) PURE; 









     STDMETHOD_ (double, GetRouteCost) (THIS_ 
        BOOL *is_unknown_id, 
        uint32 u_src_city, 
        uint32 u_dest_city 
        ) PURE; 










     STDMETHOD_ (void, SetWorkdayLevel) (THIS_ 
         sint32 w  
         ) PURE;









     STDMETHOD_ (void, SetWagesLevel) (THIS_ 
         sint32 w 
         ) PURE;









     STDMETHOD_ (void, SetRationsLevel) (THIS_ 
         sint32 w
         ) PURE;












     STDMETHOD_ (void, GetExpectedWGF) (sint32 *w, sint32 *g, sint32 *f) PURE; 







     STDMETHOD_ (void, SetWGF) ( THIS_
         sint32 w, 
         sint32 g, 
         sint32 f  
         ) PURE; 







     STDMETHOD_ (sint32, GetWagesPerPerson) (THIS_) PURE; 







     STDMETHOD_ (sint32, GetTotalBuildingUpkeep)(THIS_) PURE; 







     STDMETHOD_ (void, GetGoldLevels) (THIS_ 
         sint32 *income,  
         sint32 *lost_to_cleric,  
         sint32 *lost_to_crime, 
         sint32 *maintenance,  
         sint32 *wages,  
         sint32 *science,  
         sint32 *old_savings,  
         sint32 *current_savings 
         ) PURE; 












     STDMETHOD_ (void, SetReadinessLevel)(THIS_ 
         sint32 level 
                       
                       
                       
         ) PURE;







     STDMETHOD_ (sint32, GetReadinessLevel)(THIS_ ) PURE;







     STDMETHOD_ (void, GetReadinessCost)(THIS_ 
         sint32 *total_cost, 
         double *percent_cost 
         ) PURE;







     STDMETHOD_ (BOOL, ArmyGetNumCargo) (THIS_ 
         BOOL *is_unknown_id, 
         uint32 u_id, 
         sint32 *full_slots, 
         sint32 *empty_slots 
         ) PURE;








	 STDMETHOD_ (sint32, AiNumUnitsCanMoveIntoThisTranportEver)(THIS_ 
		BOOL *is_unknown_id,  
        uint32 test_me, 
		uint32 transport 
		) PURE;







     STDMETHOD_ (BOOL, AiCanMoveArmyIntoThisTranportEver)(THIS_ 
        BOOL* is_unknown_id,  
        uint32 test_me, 
        uint32 transport 
        ) PURE;







     STDMETHOD_ (BOOL, AiCanMoveArmyIntoThisTranportRightNow) (THIS_
         BOOL* is_unknown_id,  
         uint32 move_me,  
         uint32 transport, 
         BOOL* enough_move, 
         BOOL* adjacent, 
         BOOL* ever 
         ) PURE;











     STDMETHOD_ (BOOL, AiArmyMoveIntoTranport) (THIS_ 
         BOOL* is_unknown_id,   
         uint32 move_me,  
         uint32 transport, 
         BOOL* is_transported 
         ) PURE;
















     STDMETHOD_ (BOOL, AiUnloadAllTransportsInArmy) (THIS_ 
        BOOL *is_unknown_id, 
        uint32 unload_me, 
        MapPointData* dest_pos, 
        BOOL* did_move, 
        BOOL* i_died,   
		BOOL* revealed_foreign_units,  
        BOOL* revealed_unexplored,  
        BOOL* zocViolation,     
        BOOL* is_transported 
        ) PURE;








     STDMETHOD_ (BOOL, GetCargoMovementPoints) (THIS_ 
         BOOL *is_unknown_id,   
         uint32 u_tran_id,      
         double* min_move_point, 
         BOOL* first_move   
         ) PURE;














     STDMETHOD_ (BOOL, Paradrop) (THIS_ 
         BOOL *is_unknown_id, 
         uint32 u_id,  
         const MapPointData *dest_pos,  
         BOOL *all_dropped, 
         BOOL *some_dropped, 
         BOOL *all_died,    
         BOOL *revealed_foreign_units, 
         BOOL *revealed_unexplored  
         ) PURE;

STDMETHOD_ (sint32, GetParadropMaxDistance) (THIS_ ) PURE;

STDMETHOD_ (BOOL, CanParadropNow) (THIS_ BOOL *is_unknown_id, uint32 u_id) PURE;







     STDMETHOD_ (BOOL, CanBeCargoPodded) (THIS_ 
         BOOL *is_unknown_id,   
         uint32 u_id 
         ) PURE; 
     
#ifdef _DEBUG






     STDMETHOD_ (void, Dprint)(THIS_ char *str) PURE;
#endif








     STDMETHOD_ (BOOL, GetCurrentFuel) (THIS_ 
         BOOL *is_unknown_id, 
         uint32 u_id, 
         sint32 *current 
         ) PURE; 








     STDMETHOD_ (sint32, GetAllTileValue) (THIS_ 
         uint32 city_id,  
         BOOL *is_unknown_id, 
         sint32 num_tile, 
         TileUtility *open_tile[k_NUM_CITY_TILES] 
                            
                            
         ) PURE;








     STDMETHOD_ (void, GetCityHappiness) (THIS_ 
         uint32 city_id, 
         BOOL *is_unknown_id, 
         sint32 *old_happines, 
         double *old_crime
         ) PURE;

     STDMETHOD_ (void, GetCityCrimePrevention)(THIS_ uint32 city_id, BOOL *is_unknown_id, 
        double *cop) PURE;

     STDMETHOD_ (sint32, GetAccumulatedGrowthFood) (THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;
     STDMETHOD_ (sint32, GetAccumulatedGrowthThreshold)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;

    STDMETHOD_ (long, GetCitySizePopEffect) (THIS_ uint32 city_id, 
         BOOL *is_unknown_id, sint32 *city_size_pop_effect) PURE;

     STDMETHOD_ (void, SetScienceTax)(double s) PURE;

     STDMETHOD_ (void, CityFindWhatIsBuilt) (THIS_ uint32 city_id, BOOL *is_unknown_id, 
         BOOL is_blg_built[64], BOOL is_wndr_built[64]) PURE;
     







     STDMETHOD_ (BOOL, SetGovernmentType) (THIS_ 
sint32 idx_gov 
) PURE;

     







     STDMETHOD_ (sint32, GetGovernmentType) (THIS_) PURE;









STDMETHOD_ (double, GetCityDistFromCapitol) (THIS_ 
uint32 city_id, 
BOOL *is_unknown_id 
) PURE;










STDMETHOD_ (void, GetPeaceMovement) (THIS_ 
double *overseas_defeat, 
double *home_defeat, 
double *overseas 
) PURE;








STDMETHOD_ (double, GetCityPollution) (THIS_ 
uint32 city_id, 
BOOL *is_unknown_id 
) PURE; 












STDMETHOD_ (BOOL, AttemptSlaveRaid) (THIS_
    uint32 u_id, 
    BOOL *is_unknown_id, 
    MapPointData *target_pos 
) PURE;


STDMETHOD_ (BOOL, IsSlaveRaidPossible) (THIS_ uint32 u_id, BOOL *is_unknown_id, 
          MapPointData *target_pos) PURE;

STDMETHOD_ (sint32, GetSlaveCount)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;

STDMETHOD_ (BOOL, FindNearestCity)(THIS_ MapPointData *start_pos, 
         uint32 *nearest_city) PURE;

STDMETHOD_ (void, SetArmyDBGString) (THIS_ uint32 u_id, BOOL *is_unknow_id, 
        char *str) PURE; 

STDMETHOD_ (void, SetCityDBGString) (THIS_ uint32 u_id, BOOL *is_unknow_id, 
        char *str) PURE; 

STDMETHOD_(double, GetPollutionLevel) (THIS) PURE;
STDMETHOD_(sint32, NearTradeRoute) (THIS_ MapPointData *pos) PURE;
STDMETHOD_(sint32, GetNumTradeRoutes) (THIS) PURE;
STDMETHOD_(sint32, GetTradeWith) (THIS_ PLAYER_INDEX second) PURE;

STDMETHOD_(BOOL, ReformCity)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
							 MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, CanReformCity)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
								 MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, SueFranchise)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
								MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, CanSueFranchise)(THIS_ uint32 u_id, 
								  BOOL *is_unknown_id, 
								  MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, Bombard)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
						  MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, CanBombard)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
							 MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, ConvertCity)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
							  MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, CanConvertCity)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
								 MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, Rustle)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
							  MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, CanRustle)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
								 MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, Pillage)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
							  MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, CanPillage)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
								 MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, Pirate)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
							  MapPointData *target_pos) PURE;

STDMETHOD_(BOOL, CanPirate)(THIS_ uint32 u_id, BOOL *is_unknown_id, 
								 MapPointData *target_pos) PURE;


STDMETHOD_(BOOL, IsEnemy)(THIS_ const PLAYER_INDEX & second) PURE;

STDMETHOD_ (BOOL, HaveActiveAgreement)(THIS_ const PLAYER_INDEX & second, const AGREEMENT_TYPE & type) PURE;





STDMETHOD_ (BOOL, ExecuteAIOrder)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos, uint32 order) PURE;

STDMETHOD_ (BOOL, AbleToPlantNukeTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, AbleToMakeParkTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, AbleToUndergroundRailwayTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, AbleToConvertTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, AbleToEstablishEmbassyTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, AbleToCreateFranchiseTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, AbleToAssasinateRulerTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, AbleToStealTechnologyTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, AbleToInjoinTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, AbleToInciteRevolutionTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, AbleToCauseUnhappinessTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, AbleToExpelTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, AbleToSueTarget)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;
STDMETHOD_ (BOOL, HasBuildingType)(THIS_ uint32 city_id, sint32 type) PURE;
STDMETHOD_ (void, SellBuilding)(THIS_ uint32 city_id, sint32 type) PURE;


STDMETHOD_ (BOOL, HasEmbassyWith)(THIS_ const PLAYER_INDEX &second) PURE;

STDMETHOD_ (void, RejectTradeBid) (THIS_ uint32 bidId) PURE;
STDMETHOD_ (void, AcceptTradeBid) (THIS_ uint32 bidId) PURE;

STDMETHOD_ (sint32, CityTurnsToBuild) (THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;
STDMETHOD_ (void, CancelTradeOffer) (THIS_ IC3TradeOffer *offer) PURE;
STDMETHOD_ (void, MakeTradeOffer) (THIS_ uint32 city, sint32 resource,
								   sint32 gold) PURE;
STDMETHOD_ (BOOL, PlayerCanSeeCity) (THIS_ uint32 city, sint32 otherPlayer) PURE;

STDMETHOD_ (sint32, UnusedTradePoints) (THIS_) PURE;

STDMETHOD_ (sint32, GetCityPopulation) (THIS_ sint32 player, uint32 city_id, BOOL *is_unknown_id) PURE;
STDMETHOD_ (sint32, GetCityProduction) (THIS_ sint32 player, uint32 city_id, BOOL *is_unknown_id) PURE;
STDMETHOD_ (sint32, GetCityFood) (THIS_ sint32 player, uint32 city_id, BOOL *is_unknown_id) PURE;
STDMETHOD_ (sint32, GetCityGold) (THIS_ sint32 player, uint32 city_id, BOOL *is_unknown_id) PURE;


STDMETHOD_ (BOOL, CityCanBuildUnit) (THIS_ uint32 city, sint32 unitType) PURE;
STDMETHOD_ (BOOL, CityCanBuildBuilding) (THIS_ uint32 city, sint32 buildingType) PURE;
STDMETHOD_ (BOOL, CityCanBuildWonder) (THIS_ uint32 city, sint32 wonderType) PURE;
STDMETHOD_ (BOOL, CityCanBuildEndGameObject) (THIS_ uint32 city, sint32 endGameType) PURE;
STDMETHOD_(BOOL, CityCanBuildCapitalization) (THIS_ uint32 city) PURE;

STDMETHOD_(void, AiStartMovingPops) (THIS_ uint32 city) PURE;
STDMETHOD_(void, AiDoneMovingPops) (THIS_ uint32 city) PURE;
STDMETHOD_(BOOL, IsOccupiedByForeigner) (THIS_ MapPointData *target_pos) PURE;








STDMETHOD_(BOOL, GetCapitolPos) (THIS_ sint32 him, MapPointData *cap_pos) PURE;

STDMETHOD_(sint32, GetNumAtWarWith)(THIS_ PLAYER_INDEX test_me) PURE;


STDMETHOD_(double, GetCeaseFire)(THIS_ PLAYER_INDEX test_me, sint32 player_idx) PURE;
STDMETHOD_(double, GetAlliance)(THIS_ PLAYER_INDEX test_me, sint32 player_idx) PURE;
STDMETHOD_(double, GetWar)(THIS_ PLAYER_INDEX this_ai, sint32 test_player_one, sint32 test_player_two) PURE;
STDMETHOD_(double, GetNeutrality)(THIS_ PLAYER_INDEX test_me, sint32 player_idx) PURE;

STDMETHOD_(double, GetPlayerType)(THIS_ PLAYER_INDEX test_me) PURE;
STDMETHOD_ (BOOL, IsRobotPlayer)(THIS_ PLAYER_INDEX test_me) PURE;

STDMETHOD_(uint32, CreateUnit)(THIS_ sint32 type, MapPointData *pos, uint32 home_city) PURE;

    
    STDMETHOD_(void, GetBestProfessionals) (THIS_
        uint32 cityId, BOOL *isUnknown, 
        sint32 *pop_best_scientist,     sint32 *pop_best_science, 
        sint32 *pop_best_grunt,     sint32 *pop_best_production, 
        sint32 *pop_best_musician,     sint32 *pop_best_luxury,
        sint32 *pop_best_banker,    sint32 *pop_best_gold
    ) PURE;








    STDMETHOD_ (sint32, CountMyTradeToForeignCity) (THIS_ 
        PLAYER_INDEX test_him, 
        uint32 city_id,          
        BOOL *is_unknown_id     
        ) PURE;

	STDMETHOD_(void, SetRegard) (THIS_ PLAYER_INDEX who, sint32 level) PURE;

	STDMETHOD_(BOOL, HeAgreed) (THIS_ PLAYER_INDEX who, AGREEMENT_TYPE agreement) PURE;

    STDMETHOD_ (void, GetArmyCurrentHP) (THIS_ uint32 id, BOOL *is_unknown_id, 
        sint32 *n, sint32 unit_type[100], 
        sint32 unit_hp[100]) PURE;

    STDMETHOD_ (BOOL, ValidateArmyID) (THIS_ uint32 u_id, sint32 unit_num) PURE; 

    STDMETHOD_ (void, GetCityNumTradeRoutes) (THIS_ PLAYER_INDEX owner, uint32 u_id, BOOL *is_unknown_id, 
        sint32 *in_num, sint32 *in_max, sint32 *out_num, sint32 *out_max) PURE;

	STDMETHOD_(BOOL, CanSeeHisCity) (THIS_ uint32 cityId, sint32 cityOwner) PURE;
	STDMETHOD_ (sint32, GetSlavesPerMilitaryUnit) (THIS_) PURE;

	STDMETHOD_ (BOOL, BuildCapitalization)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;
	STDMETHOD_ (BOOL, ArmyHasLeftMap)(THIS_ uint32 army_id, BOOL *is_unknown_id) PURE;

	STDMETHOD_ (sint32, GetStage)(THIS_ ) PURE;
	STDMETHOD_ (sint32, GetNumberBuilt)(THIS_ sint32 type) PURE;
	STDMETHOD_ (BOOL, HasLab)(THIS_ ) PURE;

    STDMETHOD_ (void, DipLogStr) (THIS_ sint32 player_idx, char *str) PURE;
	STDMETHOD_(BOOL, IsActuallyVisible) (THIS_ sint32 player, uint32 army_id) PURE;
	STDMETHOD_(double, GetProductionPercentBonus)(THIS_ uint32 city_id, BOOL &is_unknown_id, const PRODUCTION_TYPE &type) PURE;

}; 



struct C3PlayerInfo {
	sint32 m_gold;
};

#endif
