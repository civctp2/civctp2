











#pragma once
#ifndef __C3ROBOT_H__
#define __C3ROBOT_H__ 1

typedef HRESULT (STDAPICALLTYPE *CreateUnknownObj)(IUnknown  *&u); 


typedef HRESULT (STDAPICALLTYPE *DebugInitProc)(void *, void *, void *, char *, int *, int *);




#include "MapPoint.h"

enum CAUSE_REMOVE_ARMY; 
enum CAUSE_NEW_ARMY; 
enum CAUSE_REMOVE_CITY; 
enum CAUSE_NEW_CITY; 
class Unit;
class TradeOffer;

typedef sint32 PLAYER_INDEX; 

enum NEW_PLAYER_IS { 
    NEW_PLAYER_IS_ROBOT, 
    NEW_PLAYER_IS_HUMAN
}; 

struct New_Age_Is_Built { 
    BOOL m_underwater; 
    BOOL m_space; 
    BOOL m_wormhole; 
};

class aui_Surface;
class BSetID;


class RobotInterface { 

    sint32 m_the_stop_player;
    BOOL m_my_turn_is_over; 
    sint32 m_nAIPlayers;
    char m_name[k_MAX_PLAYERS][_MAX_PATH]; 
    HINSTANCE  m_robot_dll[k_MAX_PLAYERS];
	CreateUnknownObj m_robot_create[k_MAX_PLAYERS]; 
    
    
    uint32 m_accumulated_frame_time[k_MAX_PLAYERS]; 
    New_Age_Is_Built m_new_age_unit[k_MAX_PLAYERS];

    MapValueStruct ***m_map_value; 

public:
    
    RobotInterface();
    void Clear(); 
    RobotInterface(CivArchive &archive); 
    ~RobotInterface();

    BOOL Init(sint32 n_ai, PLAYER_INDEX player_idx[k_MAX_PLAYERS]);
    BOOL LoadCom(sint32 p) ;
    BOOL AddRobotPlayer(const sint32 player_index);
	
	void ResizeMap(sint32 x, sint32 y, sint32 z);

	BOOL AttachRobotTo(sint32 playerIndex);
	BOOL DetachRobotFrom(sint32 playerIndex);
        
    BOOL AiIsMovingThisTurn() const;

    void DoWholeTurn();  
    BOOL BeginTurn();
    sint32 ProcessRobot(const uint32 target_milliseconds);
    sint32 GetStopPlayer() const { return m_the_stop_player; }
    void SetStopPlayer(sint32 player_index) { m_the_stop_player = player_index; } 
    void CleanupARobot(const sint32 p);

    BOOL IsCurrentRobotTurnOver() const { return m_my_turn_is_over; } 

	void RegisterDeathDifferential (double adjustment, PLAYER_INDEX player, MapPoint &position);
	void Load_AIP(char *aip_file_name, PLAYER_INDEX player);
	void Set_AILog_Verbosity(sint32 log_level, PLAYER_INDEX player);
    BOOL RegisterDeadArmy(PLAYER_INDEX owner, BSetID &id, 
		const CAUSE_REMOVE_ARMY cause);
    BOOL RegisterCreatedArmy(PLAYER_INDEX owner, BSetID &id, 
        const CAUSE_NEW_ARMY cause, const uint32 hc_id, 
         sint32 nUnits, sint32 unit_type[k_MAX_ARMY_SIZE], 
         sint32 unit_hp[k_MAX_ARMY_SIZE]);
    BOOL RegisterDeadCity(PLAYER_INDEX owner, Unit &killme, BSetID &id, 
        const CAUSE_REMOVE_CITY cause);
    void RegisterCityDiedAt(MapPoint &ipos);

    BOOL RegisterDeadUnit(PLAYER_INDEX owner, uint32 army_id, const sint32 unit_type);
    BOOL RegisterCreatedCity(Unit u, PLAYER_INDEX owner, BSetID &id, 
        const CAUSE_NEW_CITY cause, MapPoint &pos);

	void RegisterClearCargo(sint32 owner, uint32 army_id);
    void RegisterInsertCargo(sint32 owner, uint32 army_id, const sint32 unit_type, sint32 hp);
    void RegisterUnloadCargo(sint32 owner, uint32 army_id, const sint32 unit_type, sint32 hp, CAUSE_REMOVE_ARMY cause);

    BOOL RegisterArmyWasJoined(PLAYER_INDEX owner, uint32 target, 
        sint32 nUnits, sint32 unit_type[k_MAX_ARMY_SIZE], sint32 unit_hp[k_MAX_ARMY_SIZE]);

    BOOL RegisterYourArmyWasMoved(const PLAYER_INDEX owner, 
        const uint32 army_id, const MapPoint &pos);

    void RegisterHisArmyTeleported (PLAYER_INDEX his_owner, uint32 i_moved, 
        MapPointData *new_pos);

    BOOL RegisterCreateBuilding(PLAYER_INDEX owner, uint32 city_id, sint32 blg_type); 
    BOOL RegisterLostBuilding(PLAYER_INDEX owner, uint32 city_id, sint32 blg_type); 
	BOOL RegisterNewGovernment(PLAYER_INDEX owner, uint32 city_id, sint32 government_type);
    void RegisterNewWGF(PLAYER_INDEX owner, sint32 w, sint32 g, sint32 f);

    BOOL RegisterNewCapitolBuilding(PLAYER_INDEX owner, uint32 city_id); 

    BOOL RegisterCreateWonder(PLAYER_INDEX owner, uint32 city_id, sint32 wonder_type);

    BOOL RegisterLearnedScience(PLAYER_INDEX owner, sint32 idx_sci, CAUSE_SCI cause);
    void RegisterNewPlayer(PLAYER_INDEX player_id, CAUSE_NEW_PLAYER cause, 
        NEW_PLAYER_IS type, PLAYER_INDEX old_owner);
    void RegisterDeadPlayer(PLAYER_INDEX player_id);

    void RegisterAddInstallation(PLAYER_INDEX owner, 
        MapPointData &pos, sint32 inst_type);

    void RegisterRemoveInstallation(PLAYER_INDEX owner, 
        MapPointData &pos, sint32 inst_type);

	
	BOOL RegisterCityAttack(PLAYER_INDEX owner, 
							uint32 my_city_id, 
							PLAYER_INDEX his_owner, 
							uint32 his_armyid, 
							UNIT_ORDER_TYPE attack_type);

   	
	BOOL RegisterDiplomaticRequest(PLAYER_INDEX owner,
								   IC3DiplomaticRequest *request);
	BOOL RegisterDiplomaticResponse(PLAYER_INDEX owner,
									IC3DiplomaticRequest *request);

	
	void RegisterAttack(PLAYER_INDEX agressor, PLAYER_INDEX defender);
    void RegisterFightWinner(sint32 attack_owner, sint32 attack_start_num,
        sint32 defense_owner, sint32 defense_start_num, sint32 winner);

	void RegisterContactMade(PLAYER_INDEX contacter, PLAYER_INDEX contactee);
    void RegisterLeaveOurLands(PLAYER_INDEX i_leave, PLAYER_INDEX get_off_me); 
    void RegisterLeaveOurLandsExpired(PLAYER_INDEX i_leave, PLAYER_INDEX get_off_me); 

	void RegisterPlayerInfo(PLAYER_INDEX index, C3PlayerInfo *pi);

    void Serialize(CivArchive &archive); 
    BOOL IsComLoaded(const sint32 p) const; 

    
    sint32 GetNumFuzzySections() const; 
    sint32 GetNumFuzzyVariables(const sint32 idx_section) const; 
    void GetFuzzyGraph(const sint32 idx_section, const sint32 idx_variable, 
        char *&label, double &minx, double &maxx, double &miny, double &maxy, 
        sint32 &num_graphs, sint32 &num_x, double ***height, double &defuzz_val); 

    void ReloadFuzzyLogic(const PLAYER_INDEX p, const BOOL run_rules); 
    void ResetFuzzyInput(const sint32 idx_section, const sint32 idx_variable, 
        double new_defuzz_val); 

    void DumpStats();

	void SetInputLogComment(MBCHAR *comment);

	
	BOOL GetFliLoggingEnabled(sint32 owner);
	void SetFliLoggingEnabled(sint32 owner, BOOL enabled);

	
	void SendTradeBid(PLAYER_INDEX bidder,
					  PLAYER_INDEX biddee,
					  uint32 fromCity_id,
					  sint32 resource,
					  uint32 toCity_id,
					  sint32 price,
					  uint32 bidId);
	void RegisterTradeOffer(TradeOffer &offer);
	void RemoveTradeOffer(TradeOffer &offer);
	void AcceptTradeOffer(TradeOffer &offer, Unit &sourceCity, Unit &destCity,
						  PLAYER_INDEX player);

    
    void RegisterNewContinents();

	void RegisterBrokenCeaseFire(PLAYER_INDEX breaker, PLAYER_INDEX breakee);
	void RegisterBrokenAlliance(PLAYER_INDEX breaker, PLAYER_INDEX breakee);

    void RawStaticFP(WorkingMapValueStruct ***raw);
    void StaticSettlePoint(WorkingMapValueStruct ***raw, MapValueStruct ***result, MapPoint &center);
    void StaticSettle(WorkingMapValueStruct ***raw, MapValueStruct ***result);
    void InitStaticSettle();

    MapValueStruct *** GetMapValue() { return m_map_value; } 
    sint16 GetSettleValue(sint16 x, sint16 y, sint16 z); 

	void ToggleSuperFastDebugMode(PLAYER_INDEX player, BOOL on);

    void RegisterHeBuiltUnit(PLAYER_INDEX he_did_it, sint32 unit_type); 

    void RegisterHostileAction(UNIT_ORDER_TYPE attack_type, 
        PLAYER_INDEX target_player, uint32 my_army_id, PLAYER_INDEX his_owner, 
        uint32 his_unit_id, MapPointData *target_pos);

    void RegisterPollution(MapPoint &pos);

    void RegisterHeNuked(PLAYER_INDEX he_did_it, PLAYER_INDEX his_target);
    void RegisterPiracy(PLAYER_INDEX he_did_it, PLAYER_INDEX source_owner, uint32 src_id, 
                PLAYER_INDEX dest_owner, uint32 dest_id, sint32 resource);
    
    void RegisterNewRoute(const PLAYER_INDEX source_owner, const uint32 src_id, 
        const PLAYER_INDEX dest_owner, const uint32 dest_id, const sint32 resource);

    void RegisterKillRoute(const PLAYER_INDEX source_owner, const uint32 src_id, 
        const PLAYER_INDEX dest_owner, const uint32 dest_id, const sint32 resource);

	void RegisterRegardIncrease(PLAYER_INDEX who, sint32 amount);
	void RegisterAgreementViolation(AGREEMENT_TYPE agreement,
									PLAYER_INDEX violator,
									PLAYER_INDEX violatee,
									sint32 rounds);

    void RegisterAgreementAgainstMe(PLAYER_INDEX first_player, 
        PLAYER_INDEX second_player, PLAYER_INDEX target_player, double agreement_str);

    void GetFZRegard(sint32 me, sint32 him, sint32 &diplomatic, sint32 &unit, sint32 &bonus);
    void SetFZRegard(sint32 me, sint32 him, sint32 diplomatic, sint32 unit, sint32 bonus); 

    void GetAllRegard(sint32 player_idx, double i_like[k_MAX_PLAYERS]);


	char *GetAIPName(sint32 player);
	void RegisterRemoveBuildNode(PLAYER_INDEX owner, 
			uint32 cityid, 
			sint32 build_category, 
			sint32 unit_type);

	void RegisterSetBuildFront(PLAYER_INDEX owner, 
			uint32 cityid, 
			sint32 build_category, 
			sint32 unit_type);

    BOOL IWantToAttackRevealedArmy (uint32 revealed_army_id, 
        MapPoint &pos, PLAYER_INDEX reveled_player, 
        uint32 attacking_army_id, PLAYER_INDEX attacking_player);

    BOOL ValidateArmyID(PLAYER_INDEX owner, uint32 u_id, sint32 unit_num);
    BOOL IsLivingRobot(PLAYER_INDEX owner) const;

	void ForceRegard(sint32 ofPlayer, sint32 forPlayer, double toRegard);
	void ForceHate(sint32 player, sint32 forPlayer);
};

extern BOOL IsThisPlayerARobot(sint32 p);

extern RobotInterface *g_robot_interface;

#endif
