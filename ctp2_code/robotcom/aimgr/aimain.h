










#pragma once 

#ifndef __AI_MAIN_H__     
#define __AI_MAIN_H__ 1

#include "IC3GameState.h"
#include "gstypes.h"	

#define k_UNIT_STRENGTH_COEF 1.0
#define k_PRODUCTION_STRENGTH_COEF 0.2




class Planner;
struct MapPointData; 
class FrameState; 
class FrameClock; 
class AiMap; 
class ArmyAgent;
class CityAgent;
class AIRoundCount;
class BSetID; 
class DMapPoint; 
class Foreigner; 
class MoveUtility; 
class CityGrowth; 
class CityProduction; 
class CityFP; 
class AIDiplomaticRequest;
class GoalMapTarget;

template <class T> class BSet;
template <class T> class DynamicArray;
template <class DATA_TYPE> class Pool;

enum CAUSE_REMOVE_ARMY; 
enum CAUSE_NEW_ARMY; 
enum CAUSE_REMOVE_CITY; 
enum CAUSE_NEW_CITY; 
enum CAUSE_SCI; 
enum CAUSE_NEW_PLAYER; 

enum FRAME_STATE_INDEX;
enum UNIT_ORDER_TYPE;
enum AGREEMENT_TYPE;

class CivArchive; 
class Continents; 
class AiDiscovery;
class InstallationBidList;
class RouteUtilityList; 
class OvertimeUtilityList;
class AllocWGF; 
class FliEngine; 
class AiGold; 
class AiStats; 
class ScienceAgent; 
class WonderTable; 
interface IC3DiplomaticRequest;
interface IC3TradeOffer;
class WillOfThePeople; 
class Fuzzy_Variables;
class ArmyAgent;
class CityAgent;
class ForeignAgent;
class ForeignCity;


#define LOG_PAINFUL_DETAILS 4
#define LOG_DETAILS 3
#define LOG_MEDIUM 2
#define LOG_HEADERS 1
#define LOGGING_OFF 0

#define GROUND_Z 0
#define SPACE_Z 1



#define INITIAL_ARMY_AGENT_POOL_SIZE 200
#define INITIAL_CITY_AGENT_POOL_SIZE 100
#define INITIAL_FOREIGN_AGENT_POOL_SIZE 400
#define INITIAL_FOREIGN_CITY_POOL_SIZE 200

enum OPPORTUNITY_ACTION { 
OPPORTUNITY_ACTION_PARADROP, 
OPPORTUNITY_ACTION_REFUEL,
OPPORTUNITY_ACTION_PILLAGE, 
OPPORTUNITY_ACTION_ENTRENCH, 
OPPORTUNITY_ACTION_LOOT, 
OPPORTUNITY_ACTION_REPAIR_INSTALLATION,

OPPORTUNITY_ACTION_MAX 
};

class Opportunity_Action; 
interface C3PlayerInfo; 

enum CAPITOL_STATE { 
    CAPITOL_STATE_NONE, 
    CAPITOL_STATE_BUILDING, 
    CAPITOL_STATE_HASIT 
};

enum NEW_AGE;

class AiMain
{	

    sint32 m_instruction; 
    BOOL m_my_turn_is_over; 
    sint32 *m_jump_table; 

    BOOL m_has_built_a_city; 

    uint32 m_unbuilt_army_count; 

    char *m_personality_file;
	BOOL m_caravans_reserved;

public:

	
    
    sint32 AI_VERBOSITY;				

    PLAYER_INDEX my_player_index; 

	Fuzzy_Variables *fuzzy_variables;

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

    FrameState **m_frame_machine; 
    Opportunity_Action *m_opportunity_action[OPPORTUNITY_ACTION_MAX]; 
    DynamicArray<MapPointData> *m_airfield_list;

    AiMap *m_map;    
    AIRoundCount *m_round_count; 
    FrameClock *m_frame_clock; 
    BSet<CityAgent> *m_city_set; 
    BSet<ArmyAgent> *m_army_set; 

	Planner *m_planner;					
										
    sint32 m_nStartPlayers;
    sint32 m_num_players; 

    sint32 m_my_player_id; 
    BOOL m_i_am_dead; 
    Foreigner **m_foreigner; 
    Continents *m_continents; 
    sint32 m_nBuildings; 
    InstallationBidList *m_inst_bid_list;
    RouteUtilityList *m_route_utility_list; 
    OvertimeUtilityList *m_overtime_utility_list; 
    AllocWGF *m_alloc_WGF; 
    FliEngine *m_fliEngine;
    AiGold *m_gold; 
    AiStats *m_stats; 
    CityGrowth * m_city_growth; 
    CityProduction *m_city_prod; 
    CityFP *m_city_fp; 
    WillOfThePeople *m_will_of_the_people; 

    double m_my_raw_strength;
    double m_total_production; 
    double m_total_unit_cost; 

    double m_desired_farm_pw; 
    double m_desired_road_pw; 
    double m_desired_mine_pw; 
    double m_desired_transform_pw; 
    double m_desired_inst_pw; 

    ScienceAgent *m_science_agent; 
    WonderTable *m_wonder_table; 
    sint32 m_wonders_available;
    BSetID  *m_capitol_id; 
    CAPITOL_STATE m_capitol_state; 
	DynamicArray<GoalMapTarget *> *m_wanderGoalTargets;

	BOOL m_being_attached;
	BOOL m_leave_our_lands;

    AiMain();
    ~AiMain();

    BOOL InitFrameMachine();
    BOOL Init(IC3GameState *gs, uint32 goal_time_slice,
    uint32 max_time_slice, uint32 max_total_time);
    BOOL CleanupFrameMachine();
    BOOL Cleanup();
    
    void CompileJumpTable();
    void ExecuteInstruction(FILE *fout);

    void Save(CivArchive &archive);
    BOOL Load(IC3GameState *gs, CivArchive &archive); 
    void ChkSerialize(CivArchive &archive); 

    BOOL BeginTurn(uint32 end_time, BOOL &my_turn_is_over);	
    BOOL ExecuteTimeSlice(uint32 end_time, BOOL &my_turn_is_over, char debug_str[80]);	

    void SetDebugStr(char debug_str[80]);

    BSetID GenUnbuiltID(); 

    BOOL IsAtWar() const; 
    BOOL IsAtCold() const; 

    
    void RegisterAddArmy (uint32 id, CAUSE_NEW_ARMY cause, uint32 hc_id, 
        sint32 nUnits, sint32 unit_type[k_MAX_ARMY_SIZE], sint32 unit_hp[k_MAX_ARMY_SIZE]);

    void RegisterMyAddCity (uint32 id, CAUSE_NEW_CITY cause, MapPointData *pos);
    void RegisterHisAddCity (uint32 id, CAUSE_NEW_CITY cause, MapPointData *pos);
    
	void RegisterAdjustDeathDifferential (double adjustment, MapPointData &pos);
	void Load_AIP(char *aip_file_name);
	void Set_AILog_Verbosity(sint32 new_verbosity);
    void RegisterMyRemoveArmy (uint32 id, CAUSE_REMOVE_ARMY cause);
    void RegisterHisRemoveArmy (PLAYER_INDEX owner, uint32 id); 
    
    void RegisterMyRemoveCity (uint32 id, CAUSE_REMOVE_CITY cause);
	void RegisterHisRemoveCity(PLAYER_INDEX owner, uint32 id);
    void RegisterCityDiedAt(MapPointData *pos);

	void RegisterClearCargo(uint32 army_id);
    void RegisterInsertCargo (uint32 army_id, sint32 unit_type, sint32 hp);
    void RegisterUnloadCargo (uint32 army_id, sint32 unit_type, sint32 hp, CAUSE_REMOVE_ARMY cause);
    void RegisterDeadUnit (uint32 army_id, sint32 unit_type);

    void RegisterArmyWasGrouped(uint32 target, sint32 nUnits, 
        sint32 unit_type[k_MAX_ARMY_SIZE], sint32 unit_hp[k_MAX_ARMY_SIZE]);  

    void RegisterYourArmyWasMoved(uint32 army_id, MapPointData *new_pos);
    void RegisterHisArmyTeleported (PLAYER_INDEX his_owner, uint32 i_moved, 
        MapPointData *new_pos);

    void RegisterCreateBuilding (uint32 city_id, const sint32 building_type); 
    void RegisterLostBuilding (uint32 city_id, const sint32 building_type); 
	void RegisterNewGovernment (uint32 city_id, const sint32 government_type);
    void RegisterNewWGF (sint32 w, sint32 g, sint32 f);

    void RegisterNewCapitolBuilding (uint32 city_id); 
    void WeLostTheCapitol(); 

    void RegisterCreateWonder (uint32 city_id, const sint32 wonder_type, 
         PLAYER_INDEX owner); 
   
    void RegisterLearnedScience(sint32 idx_sci, const CAUSE_SCI cause); 
    void RegisterNewPlayer(PLAYER_INDEX player_id, CAUSE_NEW_PLAYER cause, PLAYER_INDEX old_owner);
    void RegisterDeadPlayer(PLAYER_INDEX player_id);

    void RegisterAddInstallation(MapPointData &pos, sint32 inst_type); 
    void RegisterRemoveInstallation(MapPointData &pos, sint32 inst_type); 

	void RegisterCityAttack(uint32 my_city_id, PLAYER_INDEX his_owner, uint32 his_army_id, UNIT_ORDER_TYPE attack_type);
	void RegisterHostileAction(UNIT_ORDER_TYPE attack_type, uint32 my_army_id, 
        PLAYER_INDEX his_owner, uint32 his_army_id, MapPointData *target_pos);

    void RegisterPollution(MapPointData *target_pos);

    void RegisterPiracy(const PLAYER_INDEX he_did_it, const PLAYER_INDEX source_owner, 
        const uint32 src_id, const PLAYER_INDEX dest_owner, 
        const uint32 dest_id, const sint32 resource);


  


  

	
	void RegisterDiplomaticRequest(IC3DiplomaticRequest *request);
	void RegisterDiplomaticResponse(IC3DiplomaticRequest *request);
	void RegisterYouAttacked(PLAYER_INDEX defender);
	void RegisterAttackedBy(PLAYER_INDEX agressor);
    void RegisterFightWinner(sint32 attack_owner, sint32 attack_start_num,
        sint32 defense_owner, sint32 defense_start_num, sint32 winner);

	void SendDiplomaticRequest(AIDiplomaticRequest *request);
	void RegisterContactMade(PLAYER_INDEX contacted);
    void RegisterLeaveOurLands (PLAYER_INDEX get_off_me); 
    void RegisterLeaveOurLandsExpired (PLAYER_INDEX get_off_me); 

	void RegisterPlayerInfo(PLAYER_INDEX index, C3PlayerInfo *info);
	sint32 GetTradeWith(PLAYER_INDEX second, PLAYER_INDEX third);
	double GetStrength(PLAYER_INDEX);
	double GetMyDefense();

	void SetPersonalityFilename(char *filename);
	const char *GetPersonalityFilename();

    
    sint32 GetNumFuzzySections(); 
    sint32 GetNumFuzzyVariables(sint32 idx_section); 
    void GetFuzzyGraph(sint32 idx_section, sint32 idx_variable, 
        char **label, double *minx, double *maxx, double *miny, double *maxy, 
        sint32 *num_graphs, sint32 *num_x, double ***height, double *defuzz_val); 

    BOOL ReloadFuzzyLogic(BOOL run_rules); 
    void ResetFuzzyInput(sint32 idx_section, sint32 idx_variable, 
        double new_defuzz_val); 


    void DumpStats(); 

	void Init_Logging();
	void Log(char * message);

	void ReceiveTradeBid(uint32 fromCity_id,
						 sint32 resource,
						 sint32 toOwner,
						 uint32 toCity_id,
						 sint32 price,
						 uint32 bidId);
	void RejectTradeBid(uint32 bidId);
	void AcceptTradeBid(uint32 bidId);

	void RegisterTradeOffer(IC3TradeOffer *offer);
	void RemoveTradeOffer(uint32 id);
	CityAgent *GetBestCityForResource(sint32 resource);
	BOOL ConfirmAcceptTradeOffer(uint32 offer,
								 uint32 destCityId,
								 PLAYER_INDEX player);

    void RegisterNewContinents(); 

	void RegisterYouBrokeCeaseFire(PLAYER_INDEX with);
	void RegisterCeaseFireBroken(PLAYER_INDEX by);
	void RegisterYouBrokeAlliance(PLAYER_INDEX with);
	void RegisterAllianceBroken(PLAYER_INDEX by);

    void RegisterHeNuked(PLAYER_INDEX he_did_it, PLAYER_INDEX his_target); 

    void RegisterHeBuiltNewAgeUnit(PLAYER_INDEX he_did_it, NEW_AGE unit_type); 

	void RegisterRegardIncrease(PLAYER_INDEX player, sint32 amount);

    void GetFZRegard (sint32 him, sint32 &diplomatic, sint32 &unit, 
        sint32 &bonus);

    void SetFZRegard (sint32 him, sint32 diplomatic, sint32 unit, 
        sint32 bonus);
    
    void GetAllRegard(double i_like[k_MAX_PLAYERS]);

	void RegisterYouAreViolatingAgreement(AGREEMENT_TYPE agreement,
										  PLAYER_INDEX against,
										  sint32 rounds);
	void RegisterOtherCivIsViolatingAgreement(AGREEMENT_TYPE agreement,
											  PLAYER_INDEX violator,
											  sint32 rounds);
	void RegisterAttack(PLAYER_INDEX attacker, PLAYER_INDEX defender);
	void RegisterBrokeTreaty(PLAYER_INDEX breaker, PLAYER_INDEX breakee);
    void RegisterAgreementAgainstMe(sint32 first_player, sint32 second_player, double str);

	void UnloadRefueledCargo();
	void BombardAdjacentEnemies();
	void RegisterRemoveBuildNode(PLAYER_INDEX owner, 
			uint32 cityid,
			sint32 category, 
			sint32 type);

	void RegisterSetBuildFront(	uint32 cityid,
								sint32 category, 
								sint32 type);

    BOOL IWantToAttackRevealedArmy (uint32 revealed_army_id, MapPointData *pos, PLAYER_INDEX reveled_player, 
       uint32 attacking_army_id);

	void DisbandObsoleteArmies();

    void  GetCurrentAIPname(char str[100]);

    BOOL ValidateArmyID(uint32 army_id, sint32 unit_num);
    void ValidateAllArmyID();

	
	void ForceRegard(sint32 forPlayer, double toRegard);

	
	void ForceHate(sint32 forPlayer);

	void SetBeingAttached(BOOL on) { m_being_attached = on; }
	BOOL IsBeingAttached() { return m_being_attached; }
	BOOL GetLeaveOurLands() const { return m_leave_our_lands; }
	void SetLeaveOurLands(const BOOL val) { m_leave_our_lands = val; }
	BOOL GetCaravansReserved() const {return m_caravans_reserved;}
	void SetCaravansReserved(const BOOL & val) {m_caravans_reserved = val;}
};

#ifdef _DEBUG
#define INSANE(insane_var) { BOOL insane_var=0; Assert(insane_var); } 
#else
#define INSANE(insane_var)
#endif

#ifdef _DEBUG
#define WPRINT(x) { char wstr[200]; sprintf x; ai->m_player->Dprint(wstr); }
#else 
#define WPRINT(x)
#endif


#define BAD_UTILITY (-1000000000.0)
#define MIN_UTILITY (BAD_UTILITY - 10.0)

#define NDELETE(del_this_ptr) delete (del_this_ptr); \
    (del_this_ptr) = NULL; \

#endif
