 











#pragma once 

#ifndef __IROBOT_H__     
#define __IROBOT_H__ 1

#include <OBJBASE.H>  


DEFINE_GUID(CLSID_IRobot, 
    0xf07b1920,
    0xc7f1,
    0x11d1,
    0x85, 0xa7, 0x00, 0x60, 0x08, 0x3a, 0x40, 0x82
);

#undef INTERFACE
#define INTERFACE IRobot

#include "gstypes.h"



interface IC3DiplomaticRequest;
interface IC3TradeOffer;

enum CAUSE_REMOVE_ARMY; 
enum CAUSE_NEW_ARMY; 
enum CAUSE_REMOVE_CITY; 
enum CAUSE_NEW_CITY; 
enum CAUSE_SCI; 
enum CAUSE_NEW_PLAYER; 
enum UNIT_ORDER_TYPE;
enum AGREEMENT_TYPE;

interface IC3GameState; 
interface IC3CivArchive; 
interface C3PlayerInfo; 
struct MapPointData; 

enum NEW_AGE { 
    NEW_AGE_UNDERWATER, 
    NEW_AGE_SPACE, 
    NEW_AGE_WORMHOLE
}; 

DECLARE_INTERFACE_ (IRobot, IUnknown)
{

public:

    
   	  STDMETHOD_ (STDMETHODIMP, Init) (THIS_ 
       IC3GameState *gs, 
       uint32 goal_time_slice, 
                                
                                
                                
       uint32 max_time_slice, 
                                
                                
       uint32 max_total_time 
                            
                            
       ) 
       PURE;

    STDMETHOD_ (void, Save) (THIS_ IC3CivArchive *archive) PURE; 
    STDMETHOD_ (BOOL, Load) (IC3GameState *gs, THIS_ IC3CivArchive *archive) PURE; 

    
    STDMETHOD_ (STDMETHODIMP, Cleanup) (THIS) PURE; 
    
    

    
    
    
     STDMETHOD_ (STDMETHODIMP, BeginTurn) (THIS_
        uint32 end_frame_by,  BOOL &my_turn_is_over
        ) PURE;

    
    
    
     
    STDMETHOD_ (STDMETHODIMP, ExecuteTimeSlice) (THIS_
      uint32 end_frame_by,  
      BOOL &my_turn_is_over, 
      char debug_str[80] 
      ) PURE;









      STDMETHOD_ (STDMETHODIMP, RegisterAddArmy) (THIS_ uint32 id,  
        CAUSE_NEW_ARMY cause, uint32 hc_id,  sint32 nUnits, 
        sint32 unit_type[k_MAX_ARMY_SIZE], sint32 unit_hp[k_MAX_ARMY_SIZE]) PURE; 










       STDMETHOD_ (STDMETHODIMP, RegisterMyAddCity) (THIS_ 
           uint32 id, 
           CAUSE_NEW_CITY cause, 
           MapPointData *pos 
           ) PURE;
       
       STDMETHOD_ (STDMETHODIMP, RegisterHisAddCity) (THIS_ uint32 id, CAUSE_NEW_CITY cause, 
            MapPointData *pos) PURE;









    STDMETHOD_ (STDMETHODIMP, RegisterAdjustDeathDifferential) (THIS_ 
	    double adjustment,  
		MapPointData &pos	
		) PURE; 








    STDMETHOD_ (STDMETHODIMP, Load_AIP) (THIS_ 
	    char *	aip_file_name 
		) PURE; 








    STDMETHOD_ (STDMETHODIMP, Set_AILog_Verbosity) (THIS_ 
	    sint32 log_level 
		) PURE; 









    STDMETHOD_ (STDMETHODIMP, RegisterMyRemoveArmy) (THIS_ 
        uint32 id,  
        CAUSE_REMOVE_ARMY cause 
        ) PURE; 













    STDMETHOD_ (STDMETHODIMP, RegisterHisRemoveArmy) (THIS_
        PLAYER_INDEX owner, 
        uint32 id 
    ) PURE; 








    
    STDMETHOD_ (STDMETHODIMP, RegisterMyRemoveCity) (THIS_ 
        uint32 id,  
        CAUSE_REMOVE_CITY cause 
    ) PURE; 











	STDMETHOD_ (STDMETHODIMP, RegisterHisRemoveCity) (THIS_ 
        PLAYER_INDEX owner, 
        uint32 id 
    ) PURE;

    STDMETHOD_ (STDMETHODIMP, RegisterCityDiedAt) (THIS_ MapPointData *pos) PURE;









   
    STDMETHOD_ (STDMETHODIMP, RegisterArmyWasGrouped) (THIS_ 
        uint32 u_di,  
        sint32 nUnits, 
        sint32 unit_type[k_MAX_ARMY_SIZE], 
        sint32 unit_hp[k_MAX_ARMY_SIZE]
        ) PURE;


    STDMETHOD_ (STDMETHODIMP, RegisterYourArmyWasMoved) (THIS_
        uint32 army_id, 
        MapPointData *new_pos
        ) PURE;

    STDMETHOD_ (STDMETHODIMP, RegisterHisArmyTeleported) (THIS_
        PLAYER_INDEX his_owner, uint32 i_moved, MapPointData *new_pos) PURE; 

	STDMETHOD_ (STDMETHODIMP, RegisterClearCargo)(THIS_
	   uint32 army_id) PURE;







   
    STDMETHOD_ (STDMETHODIMP, RegisterInsertCargo) (THIS_ 
        uint32 army_id, 
        sint32 unit_type, 
        sint32 hp
        )PURE;








   
    STDMETHOD_ (STDMETHODIMP, RegisterUnloadCargo) (THIS_ 
        uint32 army_id, 
        sint32 unit_type, 
        sint32 hp, 
        CAUSE_REMOVE_ARMY cause 
        )PURE;







   
    STDMETHOD_ (STDMETHODIMP, RegisterDeadUnit) (THIS_ 
        uint32 army_id, 
        sint32 unit_type 
        )PURE;







   

    STDMETHOD_ (STDMETHODIMP, RegisterCreateBuilding) (THIS_ uint32 city_id, 
        sint32 building_type) PURE; 






   
    STDMETHOD_ (STDMETHODIMP, RegisterLostBuilding) (THIS_ 
        uint32 city_id, 
        sint32 building_type 
        ) PURE; 

	STDMETHOD_ (STDMETHODIMP, RegisterNewGovernment) (THIS_ 
        uint32 city_id, 
        sint32 government_type
        ) PURE; 

    STDMETHOD_ (STDMETHODIMP, RegisterNewWGF) (THIS_  sint32 w, sint32 g, sint32 f)PURE;








   
    STDMETHOD_ (STDMETHODIMP, RegisterNewCapitolBuilding) (THIS_ uint32 city_id) PURE; 







   
    STDMETHOD_ (STDMETHODIMP, RegisterCreateWonder) (THIS_ 
        uint32 city_id, 
        sint32 wonder_type, 
        PLAYER_INDEX owner 
        ) PURE; 










       
    STDMETHOD_ (STDMETHODIMP, RegisterLearnedScience) (THIS_ 
        sint32 idx_sci, 
        CAUSE_SCI cause 
        ) PURE;
    












    STDMETHOD_ (STDMETHODIMP, RegisterNewPlayer) (THIS_ 
        PLAYER_INDEX player_id,  
        CAUSE_NEW_PLAYER cause, 
        PLAYER_INDEX old_owner
        ) PURE; 
    







    STDMETHOD_ (STDMETHODIMP, RegisterDeadPlayer) (THIS_ 
        PLAYER_INDEX player_id 
        ) PURE; 

    STDMETHOD_ (STDMETHODIMP,  RegisterAddInstallation) (THIS_ 
         MapPointData &pos, sint32 inst_type)PURE;

    STDMETHOD_ (STDMETHODIMP, RegisterRemoveInstallation) (THIS_ 
        MapPointData &pos, sint32 inst_type)PURE;











	STDMETHOD_ (void, RegisterCityAttack) (THIS_ uint32 my_city_id, PLAYER_INDEX his_owner, uint32 his_unit_id, UNIT_ORDER_TYPE attack_type) PURE;













	STDMETHOD_ (void, RegisterHostileAction) (THIS_ UNIT_ORDER_TYPE attack_type, uint32 my_army_id, 
        PLAYER_INDEX his_owner, uint32 his_unit_id, MapPointData *target_pos) PURE;

    STDMETHOD_ (void, RegisterPollution) (THIS_ MapPointData *target_pos) PURE;

    STDMETHOD_ (void, RegisterPiracy) (THIS_ PLAYER_INDEX he_did_it, PLAYER_INDEX source_owner, 
        uint32 src_id, PLAYER_INDEX dest_owner, uint32 dest_id, sint32 resource) PURE;











            









   
	STDMETHOD_ (void, RegisterDiplomaticRequest) (THIS_ 
        IC3DiplomaticRequest *request 
        ) PURE;








   
	STDMETHOD_ (void, RegisterDiplomaticResponse) (THIS_ 
        IC3DiplomaticRequest *request
        ) PURE;







   
	STDMETHOD_ (void, RegisterAttackedBy) (THIS_ PLAYER_INDEX agressor) PURE;







   

    STDMETHOD_ (void, RegisterYouAttacked) (THIS_ PLAYER_INDEX defender) PURE;

    STDMETHOD_ (void, RegisterFightWinner) (THIS_ sint32 attack_owner, sint32 attack_start_num,
        sint32 defense_owner, sint32 defense_start_num, sint32 winner) PURE;

    STDMETHOD_ (void,  GetAllRegard) (THIS_ double i_like[k_MAX_PLAYERS]) PURE;








   
    STDMETHOD_(void, RegisterContactMade) (THIS_ PLAYER_INDEX contacted) PURE;

    STDMETHOD_(void, RegisterLeaveOurLands) (THIS_ PLAYER_INDEX get_off_me) PURE; 
    STDMETHOD_(void, RegisterLeaveOurLandsExpired) (THIS_ PLAYER_INDEX get_off_me) PURE; 










   
	STDMETHOD_(void, RegisterPlayerInfo) (THIS_ 
        PLAYER_INDEX index, 
        C3PlayerInfo *pi) PURE;



        


    
    
    

    STDMETHOD_ (sint32, GetNumFuzzySections)(THIS) PURE; 
    STDMETHOD_ (sint32, GetNumFuzzyVariables)(THIS_ sint32 idx_section) PURE; 
    STDMETHOD_ (void, GetFuzzyGraph)(THIS_ sint32 idx_section, sint32 idx_variable, 
        char **label, double *minx, double *maxx, double *miny, double *maxy, 
        sint32 *num_graphs, sint32 *num_x, double ***height, double *defuzz_val) PURE; 

    STDMETHOD_ (BOOL, ReloadFuzzyLogic)(THIS_ BOOL run_rules) PURE; 
    STDMETHOD_ (void, ResetFuzzyInput)(THIS_ sint32 idx_section, sint32 idx_variable, 
        double new_defuzz_val) PURE; 

    STDMETHOD_ (void, DumpStats) (THIS) PURE;

	STDMETHOD_ (void, SetPersonality) (THIS_ const char* filename) PURE;

	STDMETHOD_ (void, SetInputLogComment) (THIS_ const char *comment) PURE;

	
	STDMETHOD_ (BOOL, GetFliLoggingEnabled) (THIS) PURE;
	STDMETHOD_ (void, SetFliLoggingEnabled) (THIS_ BOOL enabled) PURE;
	STDMETHOD_(void, SendTradeBid) (THIS_
									uint32 fromCity_id,
									sint32 resource,
									sint32 toCityOwner,
									uint32 toCity_id,
									sint32 price,
									uint32 bidId) PURE;
	STDMETHOD_(void, RegisterTradeOffer) (THIS_ IC3TradeOffer *offer) PURE;
	STDMETHOD_(void, RemoveTradeOffer) (THIS_ uint32 id) PURE;
	STDMETHOD_(BOOL, ConfirmAcceptTradeOffer) (THIS_ 
											   uint32 offer,
											   uint32 destCityID,
											   PLAYER_INDEX player) PURE;

    STDMETHOD_(void, RegisterNewContinents) (THIS) PURE;
	STDMETHOD_(void, RegisterYouBrokeCeaseFire) (THIS_ PLAYER_INDEX with) PURE;
	STDMETHOD_(void, RegisterCeaseFireBroken) (THIS_ PLAYER_INDEX by) PURE;
	STDMETHOD_(void, RegisterYouBrokeAlliance) (THIS_ PLAYER_INDEX with) PURE;
	STDMETHOD_(void, RegisterAllianceBroken) (THIS_ PLAYER_INDEX by) PURE;
	
	STDMETHOD_(void, ToggleSuperFastDebugMode) (THIS_ BOOL on) PURE;

    STDMETHOD_(void, RegisterHeNuked)(THIS_ PLAYER_INDEX he_did_it, 
        PLAYER_INDEX his_target) PURE;

    STDMETHOD_(void, RegisterHeBuiltNewAgeUnit) (THIS_ 
        PLAYER_INDEX he_did_it, NEW_AGE unit_type) PURE; 

	STDMETHOD_(void, RegisterRegardIncrease) (THIS_
											  PLAYER_INDEX who,
											  sint32 amount) PURE;

    STDMETHOD_(void, GetFZRegard) (THIS_ sint32 him, sint32 &diplomatic, sint32 &unit, 
        sint32 &bonus) PURE;

    STDMETHOD_(void, SetFZRegard) (THIS_ sint32 him, sint32 diplomatic, sint32 unit, 
        sint32 bonus) PURE;

	STDMETHOD_(void, RegisterYouAreViolatingAgreement) (THIS_
														AGREEMENT_TYPE agreement,
														PLAYER_INDEX against,
														sint32 rounds) PURE;
	STDMETHOD_(void, RegisterOtherCivIsViolatingAgreement) (THIS_
															AGREEMENT_TYPE agreement,
															PLAYER_INDEX violator,
															sint32 rounds) PURE;

    STDMETHOD_ (void, RegisterAgreementAgainstMe) (THIS_ 
        sint32 first_player, sint32 second_player, double agreement_str) PURE;

	STDMETHOD_(char *, GetAIPName) (THIS) PURE;
	STDMETHOD_(void, RegisterAttack) (THIS_
									  PLAYER_INDEX attacker,
									  PLAYER_INDEX defender) PURE;
	STDMETHOD_(void, RegisterBrokeTreaty) (THIS_
										   PLAYER_INDEX breaker,
										   PLAYER_INDEX breakee) PURE;
	STDMETHOD_(void, RegisterRemoveBuildNode)(THIS_
											  PLAYER_INDEX owner, 
											  uint32 city,
											  sint32 category, 
											  sint32 type) PURE;

	STDMETHOD_(void, RegisterSetBuildFront)(THIS_
											uint32 city,
											sint32 category, 
											sint32 type) PURE;

   STDMETHOD_ (BOOL, IWantToAttackRevealedArmy) (THIS_ uint32 revealed_army_id, MapPointData *pos, PLAYER_INDEX reveled_player, 
       uint32 attacking_army_id) PURE;

   STDMETHOD_ (BOOL, ValidateArmyID) (THIS_ uint32 army_id, sint32 unit_num) PURE;

   STDMETHOD_ (void, ForceRegard) (THIS_ sint32 forPlayer, double toRegard) PURE;
   STDMETHOD_ (void, ForceHate) (THIS_ sint32 forPlayer) PURE;

   STDMETHOD_ (void, SetBeingAttached) (THIS_ BOOL on) PURE;
};

#endif __IROBOT_H__
