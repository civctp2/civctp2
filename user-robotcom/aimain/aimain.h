












#pragma once 

#ifndef __AI_MAIN_H__     
#define __AI_MAIN_H__ 1


#include "ctp.h"
#include "AICause.h"
#include "Order.h"


#include "IC3GameState.h"
#include "IC3Player.h"
#include "IC3World.h"
#include "IC3RobotAstar.h"
#include "IC3UnitDB.h"
#include "IC3BlgDB.h"
#include "IC3InstDB.h"
#include "IC3TerrDB.h"
#include "IC3DipReq.h"
#include "IC3GameState.h"
#include "IC3CivArchive.h"
#include "IC3Rand.h"

#include "MapPointData.h"

class AiMain
{	
public:	
	AiMain();

	AiMain::~AiMain();
	
	BOOL Init(IC3GameState *gs, uint32 goal_time_slice,
    uint32 max_time_slice, uint32 max_total_time);
	
	BOOL Cleanup();
	
	void Save(IC3CivArchive *archive);
	
	BOOL Load(IC3GameState *gs, IC3CivArchive *archive);
	
	BOOL BeginTurn( uint32 end_frame_by, BOOL &my_turn_is_over );
	
	BOOL ExecuteTimeSlice(uint32 end_frame_by, 
		BOOL &my_turn_is_over, 
		char debug_str[80]);
	
	void RegisterAddArmy (uint32 army_id,
							 CAUSE_NEW_ARMY cause,
							 uint32 city_id,
							 sint32 num_units,
							 sint32 unit_type[k_MAX_ARMY_SIZE],
							 sint32 unit_hp[k_MAX_ARMY_SIZE]);
	
	void RegisterMyRemoveArmy (uint32 army_id, 
		CAUSE_REMOVE_ARMY cause);
	
	void RegisterHisRemoveArmy (PLAYER_INDEX owner, 
		uint32 army_id);
	
	void RegisterMyAddCity (uint32 city_id, 
		CAUSE_NEW_CITY cause, 
		MapPointData *pos);
	
	void RegisterHisAddCity (uint32 id, 
		CAUSE_NEW_CITY cause, 
		MapPointData *pos);
	
	void RegisterMyRemoveCity (uint32 city_id, 
		CAUSE_REMOVE_CITY cause);
	
	void RegisterHisRemoveCity(PLAYER_INDEX owner, 
		uint32 city_id);
	
	void RegisterCityDiedAt(MapPointData *pos);
	
	void RegisterAdjustDeathDifferential (double adjustment, 
		MapPointData &pos);
	
	void RegisterClearCargo(uint32 army_id);
	
	void RegisterInsertCargo (uint32 army_id, 
							 sint32 unit_type, 
							 sint32 hp);
	
	void RegisterUnloadCargo (uint32 army_id, 
							 sint32 unit_type, 
							 sint32 hp, 
							 CAUSE_REMOVE_ARMY cause);
	
	void RegisterDeadUnit (uint32 army_id, 
						  sint32 unit_type);
	
	void RegisterArmyWasGrouped (uint32 army_id, 
								sint32 num_units, 
								sint32 unit_type[k_MAX_ARMY_SIZE], 
								sint32 unit_hp[k_MAX_ARMY_SIZE]);
	
	void RegisterYourArmyWasMoved(uint32 army_id, 
		MapPointData *new_pos);
	
	void RegisterHisArmyTeleported (PLAYER_INDEX his_owner, 
		uint32 army_id, 
		MapPointData *new_pos);
	
	void RegisterCreateBuilding (uint32 city_id, 
								sint32 building_type);
	
	void RegisterLostBuilding (uint32 city_id, 
		sint32 building_type);
	
	void RegisterNewGovernment (uint32 city_id, 
		sint32 government_type);
	
	void RegisterNewWGF (sint32 wages, 
		sint32 gold, 
		sint32 food);
	
	void RegisterNewCapitolBuilding (uint32 city_id);
	
	void RegisterCreateWonder (uint32 city_id, 
		sint32 wonder_type, 
		PLAYER_INDEX owner);
	
	void RegisterLearnedScience(sint32 idx_sci, CAUSE_SCI cause);
	
	void RegisterNewPlayer (PLAYER_INDEX player_id, 
		CAUSE_NEW_PLAYER cause, 
		PLAYER_INDEX old_owner);
	
	void RegisterDeadPlayer (PLAYER_INDEX player_id);
	
	void RegisterAddInstallation(MapPointData &pos, 
								sint32 inst_type);
	
	void RegisterRemoveInstallation(MapPointData &pos, 
		sint32 inst_type);
	
	void RegisterCityAttack(uint32 my_city_id, 
		PLAYER_INDEX his_owner, 
		uint32 his_unit_id, 
		UNIT_ORDER_TYPE attack_type);
	
	void RegisterHostileAction(UNIT_ORDER_TYPE attack_type, 
		uint32 my_army_id, 
		PLAYER_INDEX his_owner, 
		uint32 his_unit_id, 
		MapPointData *target_pos);
	
	void RegisterPollution(MapPointData *target_pos);
	
	void RegisterPiracy(PLAYER_INDEX he_did_it,
					   PLAYER_INDEX source_owner, 
					   uint32 src_city_id, 
					   PLAYER_INDEX dest_owner, 
					   uint32 dest_city_id, 
					   sint32 resource);
	
	void RegisterDiplomaticRequest(IC3DiplomaticRequest *request);
	
	void RegisterDiplomaticResponse(IC3DiplomaticRequest *request);
	
	void RegisterFightWinner(PLAYER_INDEX attack_owner, 
		sint32 attack_start_num,
		PLAYER_INDEX defense_owner, 
		sint32 defense_start_num, 
		PLAYER_INDEX winner);
	
	void RegisterContactMade(PLAYER_INDEX contacted);
	
	void RegisterLeaveOurLands(PLAYER_INDEX foreign_owner);
	
	void RegisterLeaveOurLandsExpired (PLAYER_INDEX foreign_owner);
	
	void RegisterPlayerInfo(PLAYER_INDEX index, 
								C3PlayerInfo *pi);
	
	void SetPersonalityFilename(char *filename);
	
	void ReceiveTradeBid(uint32 from_city_id,
							 sint32 resource,
							 PLAYER_INDEX to_city_owner,
							 uint32 to_city_id,
							 sint32 price,
							 uint32 bid_id);
	
	void RegisterTradeOffer(IC3TradeOffer *offer);
	
	void RemoveTradeOffer(uint32 id);
	
	BOOL ConfirmAcceptTradeOffer(uint32 offer,
		uint32 destCityID,
		PLAYER_INDEX player);
	
	void RegisterNewContinents();
	
	void RegisterYouBrokeCeaseFire(PLAYER_INDEX with);
	
	void RegisterCeaseFireBroken(PLAYER_INDEX by);
	
	void RegisterYouBrokeAlliance(PLAYER_INDEX with);
	
	void RegisterAllianceBroken(PLAYER_INDEX by);
	
	void RegisterHeNuked(PLAYER_INDEX he_did_it, 
							 PLAYER_INDEX his_target); 
	
	void RegisterRegardIncrease(PLAYER_INDEX who, 
		sint32 amount);
	
	void GetFZRegard (PLAYER_INDEX him, 
						  sint32 &diplomatic, 
						  sint32 &unit, 
						  sint32 &bonus);
	
	void SetFZRegard (PLAYER_INDEX him, 
						  sint32 diplomatic, 
						  sint32 unit, 
						  sint32 bonus);
	
	void RegisterAgreementAgainstMe(PLAYER_INDEX first_player, 
		PLAYER_INDEX second_player, 
		double agreement_str);
	
	void RegisterAttack(PLAYER_INDEX attacker,
		PLAYER_INDEX defender);
	
	void RegisterBrokeTreaty(PLAYER_INDEX breaker,
		PLAYER_INDEX breakee);
	
	void RegisterRemoveBuildNode(PLAYER_INDEX owner, 
		uint32 city_id,
		sint32 category, 
		sint32 type);
	
	void RegisterSetBuildFront(	uint32 city_id,
		sint32 category, 
		sint32 type);
	
	void ForceRegard(PLAYER_INDEX foreign_player, 
		double to_regard);
	
	void ForceHate(PLAYER_INDEX foreign_player);
	
	void SetBeingAttached(BOOL is_attaching);

private:
    IC3GameState *m_gs;
    IC3Player *m_player; 
    IC3World *m_world; 
    IC3RobotAstar *m_robot_astar; 
    IC3UnitDB *m_unitDB; 
    IC3BuildingDB *m_buildingDB; 
	IC3Population *m_population ;
	IC3Science *m_science ;
	IC3Government *m_governmentDB;
	IC3Wonder *m_wonderDB ;
    IC3InstDB *m_installationDB; 
    IC3TerrainDB *m_terrainDB; 
	IC3Rand *m_rand;
	IC3EndGameDB *m_endGameDB;

	
	PLAYER_INDEX m_playerIndex;
	BOOL m_isDead; 
	sint32 m_width;
	sint32 m_height;
	sint32 m_levels;
	sint32 m_nStartPlayers;
	sint32 m_nPlayers;
};

#endif __AI_MAIN_H__
