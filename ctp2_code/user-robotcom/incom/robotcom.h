#pragma once

#ifndef __ROBOT_COM_H__
#define __ROBOT_COM_H__ 1

#include <OBJBASE.H>

#include "IRobot.h"
class AiMain;

enum CAUSE_REMOVE_ARMY;
enum CAUSE_NEW_ARMY;
enum CAUSE_REMOVE_CITY;
enum CAUSE_NEW_CITY;

class IC3RobotCom : public IRobot
{
	ULONG m_refCount;


	AiMain *m_ai_main;

public:

	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

    IC3RobotCom();
    ~IC3RobotCom();

    STDMETHODIMP Init(IC3GameState *gs, uint32 goal_time_slice,
    uint32 max_time_slice, uint32 max_total_time);
    STDMETHODIMP Cleanup();

    STDMETHODIMP_ (void) Save (IC3CivArchive *archive);
    STDMETHODIMP_ (BOOL) Load (IC3GameState *gs, IC3CivArchive *archive);

    STDMETHODIMP BeginTurn(uint32 end_frame_by,  BOOL &my_turn_is_over);

    STDMETHODIMP ExecuteTimeSlice(uint32 end_frame_by,  BOOL &my_turn_is_over,
        char debug_str[80] );

    STDMETHODIMP RegisterAddArmy (uint32 id, CAUSE_NEW_ARMY cause,
        uint32 hc_id, sint32 nUnits, sint32 unit_type[k_MAX_ARMY_SIZE],
        sint32 unit_hp[k_MAX_ARMY_SIZE]);

    STDMETHODIMP RegisterHisArmyTeleported (PLAYER_INDEX his_owner, uint32 i_moved,
        MapPointData *new_pos);

    STDMETHODIMP RegisterMyAddCity (uint32 id, CAUSE_NEW_CITY cause,
        MapPointData *pos);
    STDMETHODIMP RegisterHisAddCity (uint32 id, CAUSE_NEW_CITY cause,
        MapPointData *pos);

    STDMETHODIMP RegisterAdjustDeathDifferential (double adjustment, MapPointData &pos);
    STDMETHODIMP Load_AIP (char *aip_file_name);
    STDMETHODIMP Set_AILog_Verbosity (sint32 log_level);
    STDMETHODIMP RegisterMyRemoveArmy (uint32 id, CAUSE_REMOVE_ARMY cause);
    STDMETHODIMP RegisterHisRemoveArmy (PLAYER_INDEX owner, uint32 id);
    STDMETHODIMP RegisterCityDiedAt(MapPointData *pos);

    STDMETHODIMP RegisterMyRemoveCity (uint32 id, CAUSE_REMOVE_CITY cause);
	STDMETHODIMP RegisterHisRemoveCity(PLAYER_INDEX owner, uint32 id);

	STDMETHODIMP RegisterClearCargo(uint32 army_id);

    STDMETHODIMP RegisterInsertCargo (uint32 army_id, sint32 unit_type, sint32 hp);
    STDMETHODIMP RegisterUnloadCargo (uint32 army_id, sint32 unit_type, sint32 hp, CAUSE_REMOVE_ARMY cause);
	STDMETHODIMP RegisterDeadUnit (uint32 army_id, sint32 unit_type);

    STDMETHODIMP RegisterArmyWasGrouped (uint32 target, sint32 nUnits,
        sint32 unit_type[k_MAX_ARMY_SIZE], sint32 unit_hp[k_MAX_ARMY_SIZE]);

    STDMETHODIMP RegisterYourArmyWasMoved(uint32 army_id, MapPointData *new_pos);

    STDMETHODIMP RegisterCreateBuilding (uint32 city_id,
        sint32 building_type);
    STDMETHODIMP RegisterLostBuilding (uint32 city_id,
        sint32 building_type);
	STDMETHODIMP RegisterNewGovernment (uint32 city_id,
        sint32 government_type);
    STDMETHODIMP RegisterNewWGF (sint32 w, sint32 g, sint32 f);

    STDMETHODIMP RegisterNewCapitolBuilding (uint32 city_id);

    STDMETHODIMP RegisterCreateWonder (uint32 city_id,
        sint32 wonder_type, PLAYER_INDEX owner);

    STDMETHODIMP RegisterLearnedScience(sint32 idx_sci, CAUSE_SCI cause);

    STDMETHODIMP RegisterNewPlayer (PLAYER_INDEX player_id, CAUSE_NEW_PLAYER cause, PLAYER_INDEX old_owner);
    STDMETHODIMP RegisterDeadPlayer (PLAYER_INDEX player_id);

    STDMETHODIMP RegisterAddInstallation(MapPointData &pos, sint32 inst_type);
    STDMETHODIMP RegisterRemoveInstallation(MapPointData &pos, sint32 inst_type);

	STDMETHODIMP_ (void) RegisterCityAttack(uint32 my_city_id, PLAYER_INDEX his_owner, uint32 his_army_id, UNIT_ORDER_TYPE attack_type);

	STDMETHODIMP_ (void) RegisterHostileAction(UNIT_ORDER_TYPE attack_type, uint32 my_army_id,
        PLAYER_INDEX his_owner, uint32 his_army_id, MapPointData *target_pos);

    STDMETHODIMP_ (void) RegisterPollution(MapPointData *target_pos);

    STDMETHODIMP_ (void) RegisterPiracy(PLAYER_INDEX he_did_it, PLAYER_INDEX source_owner,
        uint32 src_id, PLAYER_INDEX dest_owner, uint32 dest_id, sint32 resource);










	STDMETHODIMP_(void) RegisterDiplomaticRequest(IC3DiplomaticRequest *request);
	STDMETHODIMP_(void) RegisterDiplomaticResponse(IC3DiplomaticRequest *request);
	STDMETHODIMP_(void) RegisterYouAttacked(PLAYER_INDEX defender);
	STDMETHODIMP_(void) RegisterAttackedBy(PLAYER_INDEX agressor);
    STDMETHODIMP_(void) RegisterFightWinner(sint32 attack_owner, sint32 attack_start_num,
        sint32 defense_owner, sint32 defense_start_num, sint32 winner);

	STDMETHODIMP_(void) RegisterContactMade(PLAYER_INDEX contacted);
    STDMETHODIMP_(void) RegisterLeaveOurLands (PLAYER_INDEX get_off_me);
    STDMETHODIMP_(void) RegisterLeaveOurLandsExpired (PLAYER_INDEX get_off_me);

	STDMETHODIMP_(void) RegisterPlayerInfo(PLAYER_INDEX index, C3PlayerInfo *pi);


    STDMETHODIMP_ (sint32) GetNumFuzzySections();
    STDMETHODIMP_ (sint32) GetNumFuzzyVariables(sint32 idx_section);
    STDMETHODIMP_ (void) GetFuzzyGraph(sint32 idx_section, sint32 idx_variable,
        char **label, double *minx, double *maxx, double *miny, double *maxy,
        sint32 *num_graphs, sint32 *num_x, double ***height, double *defuzz_val);

    STDMETHODIMP_ (BOOL) ReloadFuzzyLogic(BOOL run_rules);
    STDMETHODIMP_ (void) ResetFuzzyInput(sint32 idx_section, sint32 idx_variable,
        double new_defuzz_val);

    STDMETHODIMP_ (void) DumpStats();

	STDMETHODIMP_ (void) SetPersonality(const char* filename);

	STDMETHODIMP_ (void) SetInputLogComment(const char *comment);

	STDMETHODIMP_ (BOOL) GetFliLoggingEnabled(void);
	STDMETHODIMP_ (void) SetFliLoggingEnabled(BOOL enabled);

	STDMETHODIMP_(void) SendTradeBid(uint32 fromCity_id,
									 sint32 resource,
									 sint32 toCityOwner,
									 uint32 toCity_id,
									 sint32 price,
									 uint32 bidId);
	STDMETHODIMP_(void) RegisterTradeOffer(IC3TradeOffer *offer);
	STDMETHODIMP_(void) RemoveTradeOffer(uint32 id);
	STDMETHODIMP_(BOOL) ConfirmAcceptTradeOffer(uint32 offer,
												uint32 destCityID,
												PLAYER_INDEX player);

    STDMETHODIMP_(void) RegisterNewContinents();

	STDMETHODIMP_(void) RegisterYouBrokeCeaseFire(PLAYER_INDEX with);
	STDMETHODIMP_(void) RegisterCeaseFireBroken(PLAYER_INDEX by);
	STDMETHODIMP_(void) RegisterYouBrokeAlliance(PLAYER_INDEX with);
	STDMETHODIMP_(void) RegisterAllianceBroken(PLAYER_INDEX by);

	STDMETHODIMP_(void) ToggleSuperFastDebugMode(BOOL on);

    STDMETHODIMP_(void) RegisterHeNuked(PLAYER_INDEX he_did_it, PLAYER_INDEX his_target);

    STDMETHODIMP_(void) RegisterHeBuiltNewAgeUnit(PLAYER_INDEX he_did_it,
        NEW_AGE unit_type);
	STDMETHODIMP_(void) RegisterRegardIncrease(PLAYER_INDEX who, sint32 amount);

    STDMETHODIMP_(void) GetFZRegard (sint32 him, sint32 &diplomatic, sint32 &unit,
        sint32 &bonus);

    STDMETHODIMP_(void) SetFZRegard (sint32 him, sint32 diplomatic, sint32 unit,
        sint32 bonus);

    STDMETHODIMP_(void)  GetAllRegard(double i_like[k_MAX_PLAYERS]);

	STDMETHODIMP_(void) RegisterYouAreViolatingAgreement(AGREEMENT_TYPE agreement,
														 PLAYER_INDEX against,
														 sint32 rounds);
	STDMETHODIMP_(void) RegisterOtherCivIsViolatingAgreement (AGREEMENT_TYPE agreement,
															  PLAYER_INDEX violator,
															  sint32 rounds);

    STDMETHODIMP_ (void) RegisterAgreementAgainstMe(sint32 first_player,
             sint32 second_player, double agreement_str);

	STDMETHODIMP_(char *) GetAIPName(void);
	STDMETHODIMP_(void) RegisterAttack(PLAYER_INDEX attacker,
									   PLAYER_INDEX defender);
	STDMETHODIMP_(void) RegisterBrokeTreaty(PLAYER_INDEX breaker,
            PLAYER_INDEX breakee);

	STDMETHODIMP_(void) RegisterRemoveBuildNode(PLAYER_INDEX owner,
		uint32 city,
		sint32 category,
		sint32 type);

	STDMETHODIMP_(void) RegisterSetBuildFront(uint32 city,
											  sint32 category,
											  sint32 type);

   STDMETHODIMP_ (BOOL) IWantToAttackRevealedArmy (uint32 revealed_army_id, MapPointData *pos, PLAYER_INDEX reveled_player,
       uint32 attacking_army_id);

    STDMETHODIMP_ (BOOL) ValidateArmyID(uint32 army_id, sint32 unit_num);

	STDMETHODIMP_ (void) ForceRegard(sint32 forPlayer, double toRegard);
	STDMETHODIMP_ (void) ForceHate(sint32 forPlayer);
	STDMETHODIMP_ (void) SetBeingAttached(BOOL on);
};

#endif __ROBOT_COM_H__
