#include "c3.h"
#include "globals.h"

#include "ic3GameState.h"
#include "ic3player.h"
#include "ic3world.h"
#include "RobotCOM.h"
#include "ic3RobotAstar.h"
#include "ic3UnitDB.h"
#include "ic3BlgDB.h"
#include "ic3InstDB.h"
#include "ic3TerrDB.h"
#include "C3Player.h"

#include "IMapPointData.h"
#include "aicause.h"
#include "order.h"

#include "ic3GameState.h"
#include "Fuzzy_Variables.h"
#include "aimain.h"
#include "FrameClock.h"
#include "Cont.h"
#include "WonderTbl.h"

#include "AiMap.h"
#include "airndcnt.h"
#include "Foreigner.h"
#include "AiStats.h"

#include "FrameState.h"
#include "FSBeginTurn.h"
#include "fscompete.h"
#include "FSWar.h"
#include "FSGold.h"
#include "fsalloc.h"
#include "fsfuzzy.h"
#include "MST.h"

#include "ArmyAgent.h"
#include "CityAgent.h"
#include "scienceagent.h"
#include "ForeignAgent.h"
#include "ForeignCity.h"

#include "bset.h"
#include "dynarr.h"
#include "DMapPoint.h"

#include "FzOut.h"
extern double fz_pop_food_max;
extern double fz_pop_food_min;

extern double fz_pop_production_max;
extern double fz_pop_production_min;

extern double fz_pop_gold;
extern double fz_pop_science;
extern double fz_pop_happiness;

extern double fz_settler_packing;

#include "InstBidList.h"
#include "RouteUtilityList.h"
#include "OTUtilityList.h"
#include "dynarr.h"
#include "AllocWGF.h"

#include "common.h"
#include "linked_list.h"
#include "semi_dynamic_array.h"
#include "list_array.h"
#include "sorted_array.h"
#include "Plan.h"
#include "pool.h"
#include "pointer_pool.h"
#include "aip.h"
#include "AIP_Manager.h"
#include "planner.h"
#include "Scheduler.h"
#include "ailog.h"

#include "CityGrowth.h"
#include "CityProd.h"
#include "CityFP.h"
#include "FSBeginTurn.h"
#include "wotp.h"

#include "FliEngine.h"
#include "gold.h"

#include "FsDiplomat.h"
#include "ic3DipReq.h"
#include "AIDipReq.h"

#include "debugmemory.h"

#include "Oa.h"
#include "FSOppAction.h"

#include "OA_Paradrop.h"
#include "OA_Refuel.h"
#include "OA_Pillage.h"
#include "OA_Entrench.h"
#include "oa_loot.h"
#include "OA_Repair_Installation.h"
#include "OA_NoTrade.h"

#include "AiBldQue.h"

#include "GoalMapTarget.h"

#include "Wallclock.h"
Wall_Clock *g_wall_clock;

AIP_Manager * g_AIP_manager = NULL;
int g_AIP_manager_reference_count = 0;

#include <errno.h>


sint32 g_nBuildings;

#include "bit_table.h"
Bit_Table *g_can_settle_ground = NULL;
Bit_Table *g_can_settle_space = NULL;










Pool<ArmyAgent> * g_army_agent_pool;
Pool<CityAgent> * g_city_agent_pool;
Pool<ForeignAgent> * g_foreign_agent_pool;
Pool<ForeignCity> * g_foreign_city_pool;

Pool<Squad> * g_squad_pool = NULL;
Pointer_Pool<Plan> * g_plan_pool = NULL;
int g_AI_pools_reference_count = 0;

AiMain::AiMain()
{

    if (NULL == g_squad_pool) {
        g_squad_pool = new Pool <Squad> (SQUAD_ARRAY_BASE_SIZE);
    }

    if (NULL == g_plan_pool)
	{

    	g_plan_pool = new Pointer_Pool <Plan> (INITAL_PLAN_POOL_SIZE);
    }

	g_AI_pools_reference_count++;


    memset(this, 0, sizeof(*this));

	AI_VERBOSITY = LOGGING_OFF;

    m_unbuilt_army_count = 0xfffffff0;

	g_army_agent_pool = NULL;
	g_city_agent_pool = NULL;
	g_foreign_agent_pool = NULL;
	g_foreign_city_pool = NULL;

    m_my_raw_strength = 0.0;
    m_total_production = 0.0;
    m_total_unit_cost = 0.0;














	m_caravans_reserved = FALSE;
}

AiMain::~AiMain()
{
    Cleanup();

	g_AI_pools_reference_count--;

	if (g_AI_pools_reference_count == 0)
	{




		if (g_squad_pool != NULL)
		{

			delete g_squad_pool;
			g_squad_pool = NULL;

		}

		if (g_plan_pool != NULL)
		{

			delete g_plan_pool;
			g_plan_pool = NULL;

		}

	}

}








BOOL AiMain::InitFrameMachine()
{
    m_frame_machine = new FrameState*[FS_END];
    memset (m_frame_machine, 0, FS_END * sizeof (FrameState*));

    m_frame_machine[FS_BEGIN_TURN] = new FSBeginTurn();
    m_frame_machine[FS_ALLOC_POP] = new FSAllocatePopulation();

    m_frame_machine[FS_FUZZY_BEGIN_TURN_PRE] = new FSFuzzyBeginTurnPre();
    m_frame_machine[FS_FUZZY_BEGIN_TURN_DIPLOMACY] = new FSFuzzyBeginTurnDiplomacy();
    m_frame_machine[FS_FUZZY_BEGIN_TURN_RULES] = new FSFuzzyBeginTurnRules();
    m_frame_machine[FS_FUZZY_BEGIN_TURN_POST] = new FSFuzzyBeginTurnPost();

    m_frame_machine[FS_FUZZY_END_TURN] = new FSFuzzyEndTurn();
    m_frame_machine[FS_FUZZY_DIPLOMACY] = new FSDiplomat();

    m_frame_machine[FS_SET_TAXES] = new FSSetTaxes();
    m_frame_machine[FS_CONNECT_TRADE_ROUTES] = new FSConnectTradeRoutes();
    m_frame_machine[FS_COMPETE_FOR_ALL_MOVEMENT] = new FSCompeteForAllMovement();
    m_frame_machine[FS_COMPETE_FOR_INSTALLATIONS] = new FSCompeteForInstallations();

    m_frame_machine[FS_ALLOC_SCIENCE] = new FSAllocScience();
    m_frame_machine[FS_COMPETE_FOR_GOLD] = new FSCompeteForGold();

    m_frame_machine[FS_BEGIN_WAR] = new FSBeginWar();
    m_frame_machine[FS_UPDATE_THREAT] = new FSUpdateThreat();


    m_frame_machine[FS_CLEAN_UP] = new FSCleanup();
    m_frame_machine[FS_UPDATE_CONTINENT] = new FSUpdateContinent();
    m_frame_machine[FS_MST] = new FS_MinSpanTree();

    m_frame_machine[FS_WILL_OF_THE_PEOPLE] = new FSWillOfThePeople();
    m_frame_machine[FS_OPPORTUNITY_ACTIONS] = new FS_Opportunity_Actions();

	m_frame_machine[FS_DIPLOMACY] = new FSDiplomat;

    m_opportunity_action[OPPORTUNITY_ACTION_PARADROP] = new Opportunity_Action_Paradrop();

    m_opportunity_action[OPPORTUNITY_ACTION_REFUEL] = new Opportunity_Action_Refuel();
    m_opportunity_action[OPPORTUNITY_ACTION_PILLAGE] = new Opportunity_Action_Pillage();
	m_opportunity_action[OPPORTUNITY_ACTION_ENTRENCH] = new Opportunity_Action_Entrench();
	m_opportunity_action[OPPORTUNITY_ACTION_LOOT] = new Opportunity_Action_Loot();
    m_opportunity_action[OPPORTUNITY_ACTION_REPAIR_INSTALLATION] = new Opportunity_Action_Repair_Installation();


    return TRUE;
}







BOOL AiMain::CleanupFrameMachine()
{
    if (m_frame_machine) {
            Assert(m_frame_machine[FS_BEGIN_TURN]);
            delete (FSBeginTurn *) m_frame_machine[FS_BEGIN_TURN];
            m_frame_machine[FS_BEGIN_TURN] = NULL;

            Assert(m_frame_machine[FS_ALLOC_POP]);
            delete (FSAllocatePopulation *) m_frame_machine[FS_ALLOC_POP];
            m_frame_machine[FS_ALLOC_POP] = NULL;

            Assert(m_frame_machine[FS_SET_TAXES]);
            delete m_frame_machine[FS_SET_TAXES];
            m_frame_machine[FS_SET_TAXES]=NULL;

            Assert(m_frame_machine[FS_CONNECT_TRADE_ROUTES]);
            delete m_frame_machine[FS_CONNECT_TRADE_ROUTES];
            m_frame_machine[FS_CONNECT_TRADE_ROUTES]=NULL;

            Assert(m_frame_machine[FS_FUZZY_BEGIN_TURN_PRE]);
            delete m_frame_machine[FS_FUZZY_BEGIN_TURN_PRE];
            m_frame_machine[FS_FUZZY_BEGIN_TURN_PRE] = NULL;

            Assert(m_frame_machine[FS_FUZZY_BEGIN_TURN_DIPLOMACY]);
            delete m_frame_machine[FS_FUZZY_BEGIN_TURN_DIPLOMACY];
            m_frame_machine[FS_FUZZY_BEGIN_TURN_DIPLOMACY] = NULL;

            Assert(m_frame_machine[FS_FUZZY_BEGIN_TURN_RULES]);
            delete m_frame_machine[FS_FUZZY_BEGIN_TURN_RULES];
            m_frame_machine[FS_FUZZY_BEGIN_TURN_RULES] = NULL;

            Assert(m_frame_machine[FS_FUZZY_BEGIN_TURN_POST]);
            delete m_frame_machine[FS_FUZZY_BEGIN_TURN_POST];
            m_frame_machine[FS_FUZZY_BEGIN_TURN_POST] = NULL;

            Assert(m_frame_machine[FS_FUZZY_END_TURN]);
            delete m_frame_machine[FS_FUZZY_END_TURN];
            m_frame_machine[FS_FUZZY_END_TURN] = NULL;

            Assert(m_frame_machine[FS_FUZZY_DIPLOMACY]);
            delete (FSDiplomat *)m_frame_machine[FS_FUZZY_DIPLOMACY];
            m_frame_machine[FS_FUZZY_DIPLOMACY] = NULL;

            Assert(m_frame_machine[FS_COMPETE_FOR_ALL_MOVEMENT]);
            delete (FSCompeteForAllMovement *) m_frame_machine[FS_COMPETE_FOR_ALL_MOVEMENT];
            m_frame_machine[FS_COMPETE_FOR_ALL_MOVEMENT] = NULL;

            Assert(m_frame_machine[FS_COMPETE_FOR_INSTALLATIONS]);
            delete (FSCompeteForInstallations *) m_frame_machine[FS_COMPETE_FOR_INSTALLATIONS];
            m_frame_machine[FS_COMPETE_FOR_INSTALLATIONS] = NULL;

            Assert(m_frame_machine[FS_ALLOC_SCIENCE]);
            delete (FSAllocScience *) m_frame_machine[FS_ALLOC_SCIENCE];
            m_frame_machine[FS_ALLOC_SCIENCE] = NULL;

            Assert(m_frame_machine[FS_COMPETE_FOR_GOLD]);
            delete (FSCompeteForGold *) m_frame_machine[FS_COMPETE_FOR_GOLD];
            m_frame_machine[FS_COMPETE_FOR_GOLD] = NULL;


        Assert(m_frame_machine[FS_BEGIN_WAR]);
        delete (FSBeginWar *) m_frame_machine[FS_BEGIN_WAR];
        m_frame_machine[FS_BEGIN_WAR] = NULL;

        Assert(m_frame_machine[FS_UPDATE_THREAT]);
        delete (FSUpdateThreat *) m_frame_machine[FS_UPDATE_THREAT];
        m_frame_machine[FS_UPDATE_THREAT] = NULL;




        m_frame_machine[FS_USE_MARKED] = NULL;

        Assert(m_frame_machine[FS_CLEAN_UP]);
        delete (FSCleanup *) m_frame_machine[FS_CLEAN_UP];
        m_frame_machine[FS_CLEAN_UP] = NULL;

        Assert(m_frame_machine[FS_UPDATE_CONTINENT]);
        delete (FSUpdateContinent *) m_frame_machine[FS_UPDATE_CONTINENT];
        m_frame_machine[FS_UPDATE_CONTINENT] = NULL;

        Assert(m_frame_machine[FS_MST]);
        delete (FS_MinSpanTree *) m_frame_machine[FS_MST];
        m_frame_machine[FS_MST] = NULL;

        Assert(m_frame_machine[FS_WILL_OF_THE_PEOPLE]);
        delete (FSWillOfThePeople *) m_frame_machine[FS_WILL_OF_THE_PEOPLE];
        m_frame_machine[FS_WILL_OF_THE_PEOPLE] = NULL;

		Assert(m_frame_machine[FS_DIPLOMACY]);
		delete (FSDiplomat *)m_frame_machine[FS_DIPLOMACY];
		m_frame_machine[FS_DIPLOMACY];

        Assert(m_frame_machine[FS_OPPORTUNITY_ACTIONS]);
        delete(FS_Opportunity_Actions*)m_frame_machine[FS_OPPORTUNITY_ACTIONS];
        m_frame_machine[FS_OPPORTUNITY_ACTIONS];

        delete[] m_frame_machine;
        m_frame_machine = NULL;
    }

    if (m_opportunity_action) {

        Assert(m_opportunity_action[OPPORTUNITY_ACTION_PARADROP]);
        delete (Opportunity_Action_Paradrop *) m_opportunity_action[OPPORTUNITY_ACTION_PARADROP];
        m_opportunity_action[OPPORTUNITY_ACTION_PARADROP]=NULL;

        Assert(m_opportunity_action[OPPORTUNITY_ACTION_REFUEL]);
        delete (Opportunity_Action_Refuel *)m_opportunity_action[OPPORTUNITY_ACTION_REFUEL];
        m_opportunity_action[OPPORTUNITY_ACTION_REFUEL]=NULL;

        Assert(m_opportunity_action[OPPORTUNITY_ACTION_PILLAGE]);
	    delete (Opportunity_Action_Pillage*)m_opportunity_action[OPPORTUNITY_ACTION_PILLAGE];
	    m_opportunity_action[OPPORTUNITY_ACTION_PILLAGE]=NULL;

	    Assert(m_opportunity_action[OPPORTUNITY_ACTION_ENTRENCH]);
	    delete (Opportunity_Action_Entrench*)m_opportunity_action[OPPORTUNITY_ACTION_ENTRENCH];
	    m_opportunity_action[OPPORTUNITY_ACTION_ENTRENCH]=NULL;

	    Assert(m_opportunity_action[OPPORTUNITY_ACTION_LOOT]);
	    delete (Opportunity_Action_Loot*)m_opportunity_action[OPPORTUNITY_ACTION_LOOT];
	    m_opportunity_action[OPPORTUNITY_ACTION_LOOT]=NULL;

        Assert(m_opportunity_action[OPPORTUNITY_ACTION_REPAIR_INSTALLATION]);
        delete ((Opportunity_Action_Repair_Installation*)m_opportunity_action[OPPORTUNITY_ACTION_REPAIR_INSTALLATION]);
        m_opportunity_action[OPPORTUNITY_ACTION_REPAIR_INSTALLATION] = NULL;





    }
    return TRUE;
}










BOOL AiMain::Init(
    IC3GameState *gs,
    uint32 goal_time_slice,

    uint32 max_time_slice,

    uint32 max_total_time

) {

    m_my_turn_is_over = TRUE;

	AI_VERBOSITY = LOGGING_OFF;

	Assert(gs);

	fuzzy_variables = new Fuzzy_Variables();

    CompileJumpTable();


    m_has_built_a_city = FALSE;

	m_gs = gs;
    m_player = gs->GetPlayer();
    m_world = gs->GetWorld();
    m_robot_astar = gs->GetAstar();
    m_unitDB = gs->GetUnitDB();
    m_buildingDB = gs->GetBuildingDB();
	m_population = gs->GetPopulation() ;
	m_science = gs->GetScience() ;
	m_governmentDB = gs->GetGovernmentDB() ;
	m_wonderDB = gs->GetWonderDB() ;
    m_installationDB = gs->GetInstallationDB();
    m_terrainDB = gs->GetTerrainDB();
	m_rand = gs->GetRand();
	m_endGameDB = gs->GetEndGameDB();

    my_player_index = gs->GetPlayerIndex();
    m_i_am_dead = FALSE;

	Init_Logging();

    g_nBuildings = m_nBuildings = m_buildingDB->GetNumBuildings();

    sint16 x = m_world->GetXWidth();
    sint16 y = m_world->GetYHeight();
    sint16 z = m_world->GetZHeight();


    m_map = new AiMap(x, y, z, m_gs);
    Assert(m_map);
    if (!m_map) {
       return FALSE;
    }

    m_frame_clock = new FrameClock(goal_time_slice, max_time_slice, max_total_time);
    Assert(m_frame_clock);
    if (!m_frame_clock) {
       return FALSE;
    }

    InitFrameMachine();


    m_city_set = new BSet<CityAgent>;
    m_army_set = new BSet<ArmyAgent>;

    m_round_count = new AIRoundCount();

    m_nStartPlayers = gs->GetNumPlayersAtStart();
    m_num_players = gs->GetNumPlayers();
    m_my_player_id = gs->GetMyPlayerId();






    m_foreigner = new Foreigner*[k_MAX_PLAYERS];
    sint32 i;
    for(i=0; i<k_MAX_PLAYERS; i++) {
		if(i == m_my_player_id || !gs->IsPlayerAlive(i)) {
            m_foreigner[i] = NULL;
        } else {
            m_foreigner[i] = new Foreigner(i);
        }
    }

    m_continents = new Continents(this);
    m_route_utility_list = new RouteUtilityList();
    m_overtime_utility_list = new OvertimeUtilityList();
    m_inst_bid_list = new InstallationBidList(m_installationDB->GetNumInstallations());
    m_alloc_WGF = new AllocWGF();
    m_gold = new AiGold();
    m_stats = new AiStats();
    m_city_growth = new CityGrowth();
    m_city_prod = new CityProduction();
    m_city_fp = new CityFP();
    m_science_agent = new ScienceAgent(this);
    m_wonder_table = new WonderTable(this);
    m_will_of_the_people = new WillOfThePeople(this);

    m_airfield_list = new DynamicArray<MapPointData>;
	m_wanderGoalTargets = new DynamicArray<GoalMapTarget *>;

    BOOL r = InitFuzzyOut(gs, this);

	if (g_AIP_manager == NULL)
	{

		g_AIP_manager = new AIP_Manager();

		g_AIP_manager->Init(this);

	}

	g_AIP_manager_reference_count++;


	m_planner = new Planner(this);

	g_AIP_manager->Update_AIP(this, &(m_planner->the_AIP), "default.aip");

    m_capitol_state = CAPITOL_STATE_NONE;

    sint32 max_x = m_world->GetXWidth();
    sint32 max_y = m_world->GetYHeight();

    if (NULL == g_can_settle_ground) {
        g_can_settle_ground = new Bit_Table(max_x, max_y, 1);
    } else {
        g_can_settle_ground->AddRef();
    }

    if (NULL == g_can_settle_space) {
        g_can_settle_space = new Bit_Table(max_x, max_y, 1);
    } else {
        g_can_settle_space->AddRef();
    }

    m_desired_farm_pw = 0.0;
    m_desired_road_pw = 0.0;
    m_desired_mine_pw = 0.0;
    m_desired_transform_pw = 0.0;
    m_desired_inst_pw = 0.0;

#ifdef DO_WALLCLOCK
    if (NULL == g_wall_clock) {
        g_wall_clock = new Wall_Clock();
    }
#endif
    return r;
}









BOOL AiMain::Cleanup()
{

   if (m_map) {
        delete m_map;
        m_map = NULL;
    }

    if (m_frame_clock) {
        delete m_frame_clock;
        m_frame_clock =  NULL;
    }

    CleanupFrameMachine();

    if (m_city_set) {
        delete m_city_set;
        m_city_set = NULL;
    }

     if (m_army_set) {
        delete m_army_set;
        m_army_set = NULL;
    }

    if (m_round_count) {
        delete m_round_count;
        m_round_count = NULL;
    }

    Assert(m_foreigner);
    sint32 i;
    for(i=0; i<k_MAX_PLAYERS; i++) {
        if (m_foreigner[i] != NULL) {
            delete m_foreigner[i];
            m_foreigner[i] = NULL;
        }
    }
    delete[] m_foreigner;
    m_foreigner=NULL;

    if (m_continents) {
        delete m_continents;
        m_continents = NULL;
    }

    if (m_inst_bid_list) {
        delete m_inst_bid_list;
        m_inst_bid_list = NULL;
    }

    Assert(m_route_utility_list);
    delete m_route_utility_list;
    m_route_utility_list = NULL;

    Assert(m_overtime_utility_list);
    delete m_overtime_utility_list;
    m_overtime_utility_list = NULL;

    Assert(m_alloc_WGF);
    delete m_alloc_WGF;
    m_alloc_WGF = NULL;

    Assert(m_gold);
    delete m_gold;
    m_gold = NULL;

    delete m_fliEngine;
    m_fliEngine = NULL;

    Assert(m_stats);
    delete m_stats;
    m_stats = NULL;

    Assert(m_city_growth);
    delete m_city_growth;
    m_city_growth = NULL;

    Assert(m_city_prod);
    delete m_city_prod;
    m_city_prod = NULL;

    Assert(m_city_fp);
    delete m_city_fp;
    m_city_fp = NULL;

    Assert(m_wonder_table);
    delete m_wonder_table;
    m_wonder_table = NULL;

    Assert(m_jump_table);
    delete m_jump_table;
    m_jump_table = NULL;

    if (m_science_agent) {
        Assert(m_science_agent);

        delete m_science_agent;
        m_science_agent = NULL;
    }

    if (m_capitol_id) {
        delete m_capitol_id;
        m_capitol_id = NULL;
    }

    Assert(m_will_of_the_people);
    delete m_will_of_the_people;
    m_will_of_the_people = NULL;

    Assert(m_airfield_list);
    delete m_airfield_list;
    m_airfield_list = NULL;




	if (m_planner)
	{

		delete m_planner;
        m_planner = NULL;
	}

	Assert(m_wanderGoalTargets);
	delete m_wanderGoalTargets;
	m_wanderGoalTargets = NULL;

	if (fuzzy_variables != NULL)
	{
		delete fuzzy_variables;
		fuzzy_variables = NULL;
	}

	if (m_personality_file) {
		delete[] m_personality_file;
		m_personality_file = NULL;
	}

    if (g_can_settle_ground) {
        if (g_can_settle_ground->Release()) {
            g_can_settle_ground = NULL;
        }
    }
    if (g_can_settle_space) {
        if (g_can_settle_space->Release()) {
            g_can_settle_space = NULL;
        }
    }

	g_AIP_manager_reference_count--;

	if ((g_AIP_manager_reference_count == 0) && (g_AIP_manager != NULL))
	{

		delete g_AIP_manager;

		g_AIP_manager = NULL;

	}

	return TRUE;
}








void AiMain::Save(CivArchive &archive)
{

    sint32 i;


    archive.PutSINT32(AI_VERBOSITY);
    archive.PutSINT32(m_has_built_a_city);
    archive.PutSINT32(m_my_turn_is_over);
    archive.PutSINT32(m_unbuilt_army_count);
    archive.PutSINT32(my_player_index);
    archive.PutSINT32(m_i_am_dead);

    m_map->Serialize(archive);

    for (i=FS_NULL+1; i<FS_END; i++) {
        if (m_frame_machine[i]) {
            m_frame_machine[i]->Serialize(archive);
        }
    }

    for (i=0; i<OPPORTUNITY_ACTION_MAX; i++) {
        m_opportunity_action[i]->Serialize(archive);
    }

    m_frame_clock->Serialize(archive);
    m_round_count->Serialize(archive);

    m_city_set->Serialize(&archive);
    m_army_set->Serialize(&archive);

    archive << m_nStartPlayers;
    archive << m_num_players;
    archive << m_my_player_id;
    for (i=0; i<k_MAX_PLAYERS; i++) {
        if (m_foreigner[i] == NULL) {
           archive.PutSINT32(0);
        } else {
           archive.PutSINT32(1);
           m_foreigner[i]->Serialize(archive);

        }
    }

    m_continents->Serialize(archive);
    m_inst_bid_list->Serialize(m_city_set, archive);
    m_route_utility_list->Serialize(m_city_set, archive);
    m_overtime_utility_list->Serialize(m_city_set, archive);

    m_gold->Serialize(archive);
    m_alloc_WGF->Serialize(archive);

    m_stats->Serialize(archive);
    m_city_growth->Serialize(m_city_set, archive);
    m_city_prod->Serialize(m_city_set, archive);
    m_city_fp->Serialize(m_city_set, archive);

    archive << m_desired_farm_pw;
    archive << m_desired_road_pw;
    archive << m_desired_mine_pw;
    archive << m_desired_transform_pw;
    archive << m_desired_inst_pw;

    m_science_agent->Serialize(archive);
    m_wonder_table->Serialize(archive);
    archive << m_wonders_available;
    uint32 c;
    if (m_capitol_id) {
        c = m_capitol_id->GetVal();
    } else {
        c = 0;
    }

    archive << c;

    archive.PutSINT32(m_capitol_state);

    m_will_of_the_people->Serialize(archive);

    sint32 air_idx, air_num;
    archive << m_airfield_list->ArraySize();
    air_num = m_airfield_list->Num();
    archive << air_num;
    for (air_idx=0; air_idx<air_num; air_idx++) {
        archive << m_airfield_list->Access(air_idx).x;
        archive << m_airfield_list->Access(air_idx).y;
        archive << m_airfield_list->Access(air_idx).z;
    }

	archive << m_instruction;

    sint32 n = strlen (m_personality_file)+1;
    archive << n;
    archive.Store((uint8*)m_personality_file, n);

	archive << m_wanderGoalTargets->ArraySize();

    m_planner->Serialize(this, archive);

    archive << m_my_raw_strength;
    archive << m_total_production;
    archive << m_total_unit_cost;


	n = sizeof(Fuzzy_Variables);

	archive.Store((uint8*) fuzzy_variables, n);

}









BOOL AiMain::Load(IC3GameState *gs, CivArchive &archive)
{

    m_gs = gs;
    m_player = gs->GetPlayer();
    m_world = gs->GetWorld();
    m_robot_astar = gs->GetAstar();
    m_unitDB = gs->GetUnitDB();
    m_buildingDB = gs->GetBuildingDB();
    m_installationDB = gs->GetInstallationDB();
	m_population = gs->GetPopulation() ;
	m_science = gs->GetScience() ;
	m_governmentDB = gs->GetGovernmentDB() ;
	m_wonderDB = gs->GetWonderDB() ;
    m_terrainDB = gs->GetTerrainDB();
	m_rand = gs->GetRand();
	m_endGameDB = gs->GetEndGameDB();

    g_nBuildings = m_nBuildings =  m_buildingDB->GetNumBuildings();

    AI_VERBOSITY = BOOL(archive.GetSINT32());
    m_has_built_a_city = BOOL(archive.GetSINT32());
    m_my_turn_is_over = BOOL(archive.GetSINT32());
    m_unbuilt_army_count = archive.GetSINT32();
    my_player_index = PLAYER_INDEX(archive.GetSINT32());
    m_i_am_dead = BOOL(archive.GetSINT32());

    Assert(my_player_index == gs->GetPlayerIndex());

    m_map = new AiMap(archive);

    InitFrameMachine() ;
    sint32 i;
    for (i=FS_NULL+1; i<FS_END; i++) {
        if (m_frame_machine[i]) {
            m_frame_machine[i]->Serialize(archive);
        }
    }

    for (i=0; i<OPPORTUNITY_ACTION_MAX; i++) {
        m_opportunity_action[i]->Serialize(archive);
    }

    m_frame_clock = new FrameClock(archive);
    m_round_count = new AIRoundCount(archive);

    m_city_set = new BSet<CityAgent>;
    m_city_set->Serialize(&archive);

    m_army_set = new BSet<ArmyAgent>;
    m_army_set->Serialize(&archive);

    BSetID id;
    CityAgent *the_city;
    ArmyAgent *agent;











    for (agent = m_army_set->First(id); m_army_set->Last(); agent = m_army_set->Next(id)) {
        agent->HookBldQueue(this);
    }

    for (the_city = m_city_set->First(id); m_city_set->Last(); the_city = m_city_set->Next(id)) {
        the_city->HookPotentialArmies();
    }

    for (the_city = m_city_set->First(id); m_city_set->Last(); the_city = m_city_set->Next(id)) {
        the_city->HookRoads(this);
    }

    archive >> m_nStartPlayers;
    archive >> m_num_players;
    archive >> m_my_player_id;
    sint32 isptr;
    m_foreigner = new Foreigner*[k_MAX_PLAYERS];
    for (i=0; i<k_MAX_PLAYERS; i++) {
        isptr = archive.GetSINT32();
        if (isptr) {
           m_foreigner[i] = new Foreigner(archive, this);
        } else {
            m_foreigner[i] = NULL;
        }
    }

    m_continents = new Continents(archive);
    m_inst_bid_list = new InstallationBidList(m_city_set, archive);
    m_route_utility_list = new RouteUtilityList(m_city_set, archive);
    m_overtime_utility_list = new OvertimeUtilityList(m_city_set, archive);

    m_gold = new AiGold(archive);
    m_alloc_WGF  = new AllocWGF(archive);
    m_stats = new AiStats(archive);
    m_city_growth = new CityGrowth(m_city_set, archive);
    m_city_prod = new CityProduction(m_city_set, archive);
    m_city_fp = new CityFP(m_city_set, archive);

    archive >> m_desired_farm_pw;
    archive >> m_desired_road_pw;
    archive >> m_desired_mine_pw;
    archive >> m_desired_transform_pw;
    archive >> m_desired_inst_pw;

    m_science_agent = new ScienceAgent(this, archive);
    m_wonder_table = new WonderTable(archive);
    archive >> m_wonders_available;
    uint32 c;
    archive >> c;
    if (c) {
        m_capitol_id = new BSetID(c);
    } else {
        m_capitol_id = NULL;
    }
    m_capitol_state = CAPITOL_STATE(archive.GetSINT32());

    m_will_of_the_people = new WillOfThePeople(archive);

    sint32 air_idx, air_num, air_size;
    archive >> air_size;
    m_airfield_list = new DynamicArray<MapPointData>(air_size);
    archive >> air_num;
    MapPointData tmp_pos;
    for (air_idx=0; air_idx<air_num; air_idx++) {
        archive >> tmp_pos.x;
        archive >> tmp_pos.y;
        archive >> tmp_pos.z;
        m_airfield_list->InsertFlat(tmp_pos);
    }

    archive >> m_instruction;

    sint32 n;
    archive >> n;
    m_personality_file = new char [n+1];
    archive.Load((uint8*)m_personality_file, n);

    sint32 target_size;
	archive >> target_size;
	m_wanderGoalTargets = new DynamicArray<GoalMapTarget *>(target_size);

	if (g_AIP_manager == NULL)
	{

		g_AIP_manager = new AIP_Manager();

		g_AIP_manager->Init(this);

	}

	g_AIP_manager_reference_count++;

    m_planner = new Planner(this, archive);

    m_planner->HookUp(this);

    ((Opportunity_Action_Paradrop*)m_opportunity_action[OPPORTUNITY_ACTION_PARADROP])->HookParatroops(this);

#ifdef VALIDATE_ON
    m_planner->Validate(this);
#endif VALIDATE_ON

    archive >> m_my_raw_strength;
    archive >> m_total_production;
    archive >> m_total_unit_cost;


	fuzzy_variables = new Fuzzy_Variables();

    CompileJumpTable();
    BOOL r = InitFuzzyOut(gs, this);




	n = sizeof(Fuzzy_Variables);

    archive.Load((uint8*) fuzzy_variables, n);


    sint32 max_x = m_world->GetXWidth();
    sint32 max_y = m_world->GetYHeight();
    if (NULL == g_can_settle_ground) {
        g_can_settle_ground = new Bit_Table(max_x, max_y, 1);
    } else {
        g_can_settle_ground->AddRef();
    }

    if (NULL == g_can_settle_space) {
        g_can_settle_space = new Bit_Table(max_x, max_y, 1);
    } else {
        g_can_settle_space->AddRef();
    }

    return r;
}












void AiMain::ChkSerialize(CivArchive &archive)
{

    CHECKSERIALIZE

}








enum FRAME_STATE_OP {
    OP_END,
    OP_EXEC,
    OP_LABEL,
    OP_JUMP,
    OP_BRANCH2,

    OP_BRANCH4
};

enum FS_LABEL {
    LABEL_START,
    LABEL_START_NEW_WAR,
    LABEL_BEGIN_TURN_WAR,
    LABEL_MOVE_SOME_UNITS,
    LABEL_CLEAN_UP,
    LABEL_REMOVE_MARKED,
    LABEL_WE_SAW_BADDIE,
    LABEL_END,
    LABEL_MAX
};























sint32 g_main_loop[][2] = {
{ OP_LABEL,     LABEL_START},
    { OP_EXEC,      FS_UPDATE_THREAT},
	 { OP_EXEC,      FS_FUZZY_DIPLOMACY},

    { OP_EXEC,      FS_BEGIN_TURN},

    { OP_EXEC,      FS_FUZZY_BEGIN_TURN_PRE},
    { OP_EXEC,      FS_FUZZY_BEGIN_TURN_DIPLOMACY},

    { OP_EXEC,      FS_FUZZY_BEGIN_TURN_RULES},
    { OP_EXEC,      FS_FUZZY_BEGIN_TURN_POST},

    { OP_EXEC,      FS_WILL_OF_THE_PEOPLE},
    { OP_EXEC,      FS_COMPETE_FOR_INSTALLATIONS},
    { OP_EXEC,      FS_CONNECT_TRADE_ROUTES},
    { OP_EXEC,      FS_COMPETE_FOR_GOLD},






{ OP_LABEL, LABEL_MOVE_SOME_UNITS},
    { OP_EXEC,  FS_COMPETE_FOR_ALL_MOVEMENT},
    { OP_EXEC,  FS_OPPORTUNITY_ACTIONS},

    { OP_EXEC,      FS_SET_TAXES},
    { OP_EXEC,      FS_ALLOC_SCIENCE},
    { OP_EXEC,      FS_ALLOC_POP},

{ OP_LABEL, LABEL_CLEAN_UP},
    { OP_EXEC,      FS_CLEAN_UP},
    { OP_EXEC,      FS_UPDATE_CONTINENT},
    { OP_EXEC,      FS_MST},
    { OP_EXEC,      FS_FUZZY_END_TURN},

{ OP_LABEL, LABEL_END},
    { OP_END, 0}
};

#define MAX_FS_INSTRUCTION (sizeof(g_main_loop)/8)









void AiMain::CompileJumpTable()

{
    m_jump_table = new sint32[LABEL_MAX];

    sint32 i;
    for (i=0; i<LABEL_MAX; i++) {
        m_jump_table[i] = -1;
    }


    sint32 val;
    for (i=0; i<MAX_FS_INSTRUCTION; i++) {
        if (g_main_loop[i][0] == OP_LABEL) {
            val = g_main_loop[i][1];

            Assert(0 <= val);
            Assert(val < LABEL_MAX);
            Assert(m_jump_table[val] == -1);

            m_jump_table[val] = i;
        }
    }
}









void AiMain::ExecuteInstruction(FILE *fout)
{
    Assert (0 <= m_instruction);
    Assert (m_instruction < MAX_FS_INSTRUCTION);
    if ((m_instruction < 0) ||
        (MAX_FS_INSTRUCTION <= m_instruction )) {
        m_my_turn_is_over = TRUE;
        return;
    }

    sint32 branch = -1;
    FRAME_STATE_OP op = FRAME_STATE_OP(g_main_loop[m_instruction][0]);
    FRAME_STATE_INDEX val = FRAME_STATE_INDEX(g_main_loop[m_instruction][1]);
    switch (op) {
    case OP_END:
        m_my_turn_is_over = TRUE;
        break;
    case OP_EXEC:
        Assert(0 <= val);
        Assert(val < FS_END);
        Assert(m_frame_machine[val]);
        m_frame_machine[val]->Execute(this, branch, fout);
        m_instruction++;
        break;
    case OP_LABEL:
        m_instruction++;
        break;
    case OP_JUMP:
        Assert(0 <= val);
        Assert(val < LABEL_MAX);
        m_instruction = m_jump_table[val];
        break;
    case OP_BRANCH2:
        Assert(0 <= val);
        Assert(val < FS_END);
        Assert(m_frame_machine[val]);
        m_frame_machine[val]->Execute(this, branch, fout);
        Assert(0 < branch);
        Assert(branch <= 2);
        m_instruction += branch;
        break;
    case OP_BRANCH4:
        Assert(0 <= val);
        Assert(val < FS_END);
        Assert(m_frame_machine[val]);
        m_frame_machine[val]->Execute(this, branch, fout);
        Assert(0 < branch);
        Assert(branch <= 4);
        m_instruction += branch;
        break;
    default:
        INSANE(UNKNOWN_OP_CODE);
        break;
    }
}



















BOOL AiMain::BeginTurn(uint32 end_time, BOOL &my_turn_is_over)
{
    FILE *fout = NULL;
    AiMain *ai = this;

    START_WALLCLOCK(m_my_player_id==0);

    WPRINT((wstr, "--AI------<%d>--------", ai->m_round_count->GetRound()));

    if (m_round_count->GetRound() == 0) {
        m_continents->Update(this);
    }

    m_round_count->BeginTurn(ai);
    m_my_turn_is_over = FALSE;

    m_instruction = 0;

	m_leave_our_lands = FALSE;

    return TRUE;
}


















BOOL AiMain::ExecuteTimeSlice(uint32 end_frame_by,  BOOL &my_turn_is_over, char *debug_str)
{
    if (m_i_am_dead) {
        Assert(FALSE);
        return FALSE;
    }

    sint32 count = 0;
    FILE *fout=NULL;
    uint32 cur_time = GetTickCount();

    count = 0;

#ifdef _DEBUG
    if (m_instruction == 0) {
        ValidateAllArmyID();
    }
#endif

    for (count = 0;
        (count<500) && !m_my_turn_is_over && !m_i_am_dead && (cur_time < end_frame_by);
        count++)  {

       ExecuteInstruction(fout);

       cur_time = GetTickCount();

    }


    SetDebugStr(debug_str);
    Assert(count < 500 );
    my_turn_is_over = m_my_turn_is_over;

#ifdef _DEBUG
    if (my_turn_is_over) {
        ValidateAllArmyID();
    }
#endif





    BOOL create_new_wallclock_log;
    create_new_wallclock_log = m_round_count->GetRound()==1;
    END_WALLCLOCK(m_my_player_id, create_new_wallclock_log);

    return TRUE;
}

void AiMain::SetDebugStr(char debug_str[80])
{
    sint32 prev_instruction = m_instruction-1;

    if (prev_instruction<0)
        prev_instruction=0;

    if (g_main_loop[prev_instruction][0] == OP_LABEL) {
        switch (g_main_loop[prev_instruction][1]) {
        case LABEL_START:
            sprintf (debug_str, "LABEL_START");
            break;
        case LABEL_START_NEW_WAR:
            sprintf (debug_str, "LABEL_START_NEW_WAR");
            break;
        case LABEL_BEGIN_TURN_WAR:
            sprintf (debug_str, "LABEL_BEGIN_TURN_WAR");
            break;
        case LABEL_MOVE_SOME_UNITS:
            sprintf (debug_str, "LABEL_MOVE_SOME_UNITS");
            break;
        case LABEL_CLEAN_UP:
            sprintf (debug_str, "LABEL_CLEAN_UP");
            break;
        case LABEL_REMOVE_MARKED:
            sprintf (debug_str, "LABEL_REMOVE_MARKED");
            break;
        case LABEL_WE_SAW_BADDIE:
            sprintf (debug_str, "LABEL_WE_SAW_BADDIE");
            break;
        case LABEL_END:
            sprintf (debug_str, "LABEL_END");
            break;
        default:
            INSANE(UNKNOWN_LABEL);
        }
    } else if (g_main_loop[prev_instruction][0] == OP_EXEC) {
        switch (g_main_loop[prev_instruction][1]) {
        case FS_BEGIN_TURN:
            sprintf (debug_str, "FS_BEGIN_TURN");
            break;
        case FS_FUZZY_BEGIN_TURN_PRE:
            sprintf (debug_str, "FS_FUZZY_BEGIN_TURN_PRE");
            break;
        case FS_FUZZY_BEGIN_TURN_DIPLOMACY:
            sprintf (debug_str, "FS_FUZZY_BEGIN_TURN_DIPLOMACY");
            break;
        case FS_FUZZY_BEGIN_TURN_RULES:
            sprintf (debug_str, "FS_FUZZY_BEGIN_TURN_RULES");
            break;
        case FS_FUZZY_BEGIN_TURN_POST:
            sprintf (debug_str, "FS_FUZZY_BEGIN_TURN_POST");
            break;
        case FS_FUZZY_END_TURN:
            sprintf (debug_str, "FS_FUZZY_END_TURN");
            break;
        case FS_FUZZY_DIPLOMACY:
            sprintf (debug_str, "FS_FUZZY_DIPLOMACY");
            break;
        case FS_SET_TAXES:
            sprintf (debug_str, "FS_SET_TAXES");
            break;
        case FS_CONNECT_TRADE_ROUTES:
            sprintf (debug_str, "FS_CONNECT_TRADE_ROUTES");
            break;
        case FS_COMPETE_FOR_INSTALLATIONS:
            sprintf (debug_str, "FS_COMPETE_FOR_INSTALLATIONS");
            break;
        case FS_COMPETE_FOR_ALL_MOVEMENT:
            sprintf (debug_str, "FS_COMPETE_FOR_ALL_MOVEMENT");
            break;
        case FS_ALLOC_SCIENCE:
            sprintf (debug_str, "FS_ALLOC_SCIENCE");
            break;
        case FS_COMPETE_FOR_GOLD:
            sprintf (debug_str, "FS_COMPETE_FOR_GOLD");
            break;
        case FS_BUILD_RECRUITMENT:
            sprintf (debug_str, "FS_BUILD_RECRUITMENT");
            break;
        case FS_BEGIN_WAR:
            sprintf (debug_str, "FS_BEGIN_WAR");
            break;
        case FS_UPDATE_THREAT:
            sprintf (debug_str, "FS_UPDATE_THREAT");
            break;
        case FS_ALLOC_POP:
            sprintf (debug_str, "FS_ALLOC_POP");
            break;
        case FS_USE_MARKED:
            sprintf (debug_str, "FS_USE_MARKED");
            break;
        case FS_CLEAN_UP:
            sprintf (debug_str, "FS_CLEAN_UP");
            break;
        case FS_UPDATE_CONTINENT:
            sprintf (debug_str, "FS_UPDATE_CONTINENT");
            break;
        case FS_MST:
            sprintf (debug_str, "FS_MST");
            break;
        case FS_DIPLOMACY:
	        sprintf (debug_str, "FS_DIPLOMACY");
            break;
        case FS_WILL_OF_THE_PEOPLE:
            sprintf (debug_str, "FS_WILL_OF_THE_PEOPLE");
            break;
        case FS_OPPORTUNITY_ACTIONS:
            sprintf (debug_str, "FS_OPPORTUNITY_ACTIONS");
            break;
        default:
            INSANE(UNKNOWN_FS_LABAL);
        }
    } else {
        INSANE(NO_MATCHING_STRING);
    }

}









BOOL AiMain::IsAtWar() const
{
    sint32 i;

   for (i=0; i<k_MAX_PLAYERS; i++) {
       if (m_foreigner[i]) {
            if (m_foreigner[i]->IsAtHotWar()) {
                return TRUE;
            }
       }
   }
   return FALSE;
}










void AiMain::RegisterAddArmy (
   uint32 uid,
   CAUSE_NEW_ARMY cause,
   uint32 uhc_id,
   sint32 nUnits,
   sint32 unit_type[k_MAX_ARMY_SIZE],
   sint32 unit_hp[k_MAX_ARMY_SIZE]
) {
    BSetID id = uid;
    ArmyAgent *aa = NULL;
    BSetNode<ArmyAgent> *an;
    BSetID hc;
    CityAgent *the_city=NULL;
    sint32 i;






    Assert(0 < nUnits);

    if (cause != CAUSE_NEW_ARMY_TRADER) {
        aa =  new ArmyAgent(this, id, NULL, AGENT_STATE_LOOKING_FOR_WORK);
        an = new BSetNode<ArmyAgent>(id, aa);
        for (i=0; i<nUnits; i++) {
            aa->InsertUnitType(this, unit_type[i], unit_hp[i]);
        }
		aa->ResetArmyStrength(this);

        aa->UpdateParadropStatus(this, cause);
        m_army_set->InsertNode(an);

   		m_planner->Insert_Agent(aa);

        ((Opportunity_Action_Paradrop*)m_opportunity_action[OPPORTUNITY_ACTION_PARADROP])->RegisterAddArmy(aa);

		#ifdef _DEBUG
		BOOL ret;
		sint32 units = aa->GetNUnits();
		ret = m_player->ValidateArmyID(aa->GetID().GetVal(), units);
		#endif
    }

    switch (cause) {
    case CAUSE_NEW_ARMY_TRADER:
        hc = uhc_id;
        the_city = m_city_set->Find(hc);
        Assert(m_unitDB->GetIsTrader(unit_type[0]));
		if(the_city)
			the_city->RegisterBuiltFreight(this);
        break;
    case CAUSE_NEW_ARMY_BUILT:
        hc = uhc_id;
        the_city = m_city_set->Find(hc);
        Assert(the_city);
        the_city->RegisterBuiltArmy(this, id);
        break;
    case CAUSE_NEW_ARMY_UNGROUPING:

        break;
    default:
        break;
    }
}










void AiMain::RegisterMyAddCity
(
	uint32 id,
	CAUSE_NEW_CITY cause,
	MapPointData *pos
)
{

    CityAgent *the_city = new CityAgent(this, id, pos);
    BSetNode<CityAgent> *cn = new BSetNode<CityAgent>(id, the_city);
    m_city_set->InsertNode(cn);

    sint32 i;

    if (!m_has_built_a_city) {
        m_has_built_a_city = TRUE;
        for (i=0; i<k_MAX_PLAYERS; i++) {
            if (m_foreigner[i]) {
               m_foreigner[i]->ResetCenterOfMass(*pos);
            }
        }
    }




    the_city->PlaceAllPop(this, fz_pop_food_max, fz_pop_production_min,
                    fz_pop_gold, fz_pop_science, fz_pop_happiness, 0 , 1) ;

    the_city->BeginTurn(this);


    the_city->ComputePotentialUnits(this);


	m_planner->Insert_Agent(the_city);

   	the_city->Setup_Defense_Goal(this);

    the_city->InsertBuildingGoals(this);

   	the_city->Setup_Retreat_Goal(this);


    m_map->FlattenStaticSettle(*pos, sint16(max(1, fz_settler_packing)));

	if (cause == CAUSE_NEW_CITY_CONQUEST)
		the_city->SetCaptured(m_round_count->GetRound());

}

void AiMain::RegisterHisAddCity
(
	uint32 id,
	CAUSE_NEW_CITY cause,
	MapPointData *pos
)
{

    m_map->FlattenStaticSettle(*pos, 3);
}






void AiMain::RegisterAdjustDeathDifferential (double adjustment, MapPointData &pos)
{
	if (m_planner) {
		m_planner->Adjust_Death_Differential (adjustment, pos);
	}
}







void AiMain::Load_AIP(char *aip_file_name)
{
	if (m_planner) {

		g_AIP_manager->Update_AIP(this, &(m_planner->the_AIP), aip_file_name);
	}
}







void AiMain::Set_AILog_Verbosity(sint32 new_verbosity)
{
	AI_VERBOSITY = new_verbosity;
}


















void AiMain::RegisterMyRemoveArmy (
   uint32 uid,
   CAUSE_REMOVE_ARMY cause)
{
    BSetID id(uid);

    ArmyAgent *aa = m_army_set->Find(id);
    Assert(aa);

	if (aa == NULL)
		return;

    ((Opportunity_Action_Paradrop*)m_opportunity_action[OPPORTUNITY_ACTION_PARADROP])->RegisterMyRemoveArmy(aa);

    switch (cause) {
    case CAUSE_REMOVE_ARMY_TRANSPORTED:
        aa->EventMarkTransported();
        break;
    case CAUSE_REMOVE_ARMY_NUKE:
		aa->RegisterNukeDeath(this);
		aa->EventMarkDead();
		break;
    case CAUSE_REMOVE_ARMY_CHEAT:
    case CAUSE_REMOVE_ARMY_GROUPING:
    case CAUSE_REMOVE_ARMY_UNGROUPING:
    case CAUSE_REMOVE_ARMY_RAIL_LAUNCH:
    case CAUSE_REMOVE_ARMY_PARADROP:
    case CAUSE_REMOVE_ARMY_SETTLE:
    case CAUSE_REMOVE_ARMY_STARVATION:
    case CAUSE_REMOVE_ARMY_PARKRANGER:
    case CAUSE_REMOVE_ARMY_BOMBARD:
    case CAUSE_REMOVE_ARMY_SUE:
    case CAUSE_REMOVE_ARMY_POLLUTION:
    case CAUSE_REMOVE_ARMY_SLAVE_UPRISING:
    case CAUSE_REMOVE_ARMY_ENSLAVED:
    case CAUSE_REMOVE_ARMY_NO_MAT_SUPPORT:
    case CAUSE_REMOVE_ARMY_DIED_IN_ATTACK:
    case CAUSE_REMOVE_ARMY_DIED_IN_SPYING:
    case CAUSE_REMOVE_ARMY_DIED_IN_SLAVERAID:
    case CAUSE_REMOVE_ARMY_DIED_IN_UNDERGROUND_RR_RAID:
    case CAUSE_REMOVE_ARMY_DIED_IN_CONVERT_RAID:
    case CAUSE_REMOVE_ARMY_DIED_IN_REFORMATION:
    case CAUSE_REMOVE_ARMY_COUNTERBOMBARD:
    case CAUSE_REMOVE_ARMY_OUTOFFUEL:
	case CAUSE_REMOVE_ARMY_ATTACKED:
    case CAUSE_REMOVE_ARMY_RUSTLE:
    case CAUSE_REMOVE_ARMY_DIPLOMACY:
	default:
		aa->EventMarkDead();
	}




















    if (m_my_turn_is_over) {
        m_planner->the_scheduler->Clean_Up();
    }

}

void AiMain::RegisterHisRemoveArmy(PLAYER_INDEX owner, uint32 u_id)
{

	if(m_foreigner[owner]) {
		m_foreigner[owner]->RemoveArmy(this, u_id);
	}

    if (m_my_turn_is_over) {
        m_planner->the_scheduler->Clean_Up();
    }
}










void AiMain::RegisterMyRemoveCity (uint32 uid, CAUSE_REMOVE_CITY cause)
{
    BSetID cityid(uid);
    BSetID id;

    ArmyAgent *agent;
    CityAgent *the_city = m_city_set->Find(cityid);
    Assert(the_city);

    m_inst_bid_list->RegisterMyRemoveCity(uid);
    m_overtime_utility_list->RegisterMyRemoveCity(uid);

	the_city->Remove_Defense_Goal(this);

	the_city->Remove_Retreat_Goal(this);





    if (m_capitol_id) {
		if (*m_capitol_id == uid) {
		    WeLostTheCapitol();
		}
    }

    m_continents->RemoveCityReferance(the_city);

    CityAgent *ca=NULL;
    for (ca = m_city_set->First(id); m_city_set->Last(); ca = m_city_set->Next(id)) {
        if (ca != the_city) {
            ca->RemoveCityReferance(the_city);
        }
    }


    AGENT_STATE state;
    for (agent = m_army_set->First(id); m_army_set->Last(); agent = m_army_set->Next(id)) {
        if (agent->GetHomeCityAgent() == the_city) {
            state = agent->GetState();

            agent->EventMarkDead();
        }
    }

    m_city_set->Del(cityid);

    if (m_my_turn_is_over) {
        m_planner->the_scheduler->Clean_Up();
    }
}










void AiMain::RegisterHisRemoveCity(
   PLAYER_INDEX owner,
   uint32 id
) {
	Assert(m_foreigner[owner]);
	if(m_foreigner[owner]) {
		m_foreigner[owner]->RemoveCity(this, id);
	}

    if (m_my_turn_is_over) {
        m_planner->the_scheduler->Clean_Up();
    }

}

void AiMain::RegisterCityDiedAt(MapPointData *pos)
{

    m_map->UnflattenSettle(*pos, 2);
}

void AiMain::RegisterClearCargo(uint32 army_id)
{
    ArmyAgent *the_army=NULL;
    the_army = m_army_set->Find(army_id);
    Assert(the_army);
    the_army->RegisterClearCargo();
	the_army->ResetArmyStrength(this);

	#ifdef _DEBUG
	BOOL ret;
	sint32 units = the_army->GetNUnits();
	ret = m_player->ValidateArmyID(the_army->GetID().GetVal(), units);
	#endif
}

void AiMain::RegisterInsertCargo (uint32 army_id, sint32 unit_type, sint32 hp)
{
    ArmyAgent *the_army=NULL;
    the_army = m_army_set->Find(army_id);
    Assert(the_army);
    the_army->RegisterInsertCargo(this, unit_type, hp);
	the_army->ResetArmyStrength(this);

	#ifdef _DEBUG
	BOOL ret;
	sint32 units = the_army->GetNUnits();
	ret = m_player->ValidateArmyID(the_army->GetID().GetVal(), units);
	#endif
}

void AiMain::RegisterUnloadCargo (uint32 army_id, sint32 unit_type, sint32 hp,
     CAUSE_REMOVE_ARMY cause)
{
    ArmyAgent *the_army=NULL;

    the_army = m_army_set->Find(army_id);
    Assert(the_army);
    the_army->RegisterUnloadCargo(this, unit_type, hp, cause);
	the_army->ResetArmyStrength(this);

#ifdef _DEBUG

	Assert(m_player);
    if (CAUSE_REMOVE_ARMY_TRANSPORT_DIED != cause) {
	    BOOL ret;
	    sint32 units = the_army->GetNUnits();
	    ret = m_player->ValidateArmyID(the_army->GetID().GetVal(), units);
    }
#endif

}

BOOL AiMain::ValidateArmyID(uint32 army_id, sint32 unit_num)
{
    ArmyAgent *the_army=NULL;

    the_army = m_army_set->Find(army_id);
    Assert(the_army);
    Assert(the_army->GetNUnits() == unit_num);

    return the_army != NULL;
}

void AiMain::ValidateAllArmyID()
{
    BSetID id;

    BOOL ret;
    ArmyAgent *aa=NULL;
    sint32 unit_num;
    for (aa = m_army_set->First(id); m_army_set->Last(); aa = m_army_set->Next(id)) {
        switch (aa->GetState()) {
        case AGENT_STATE_MARKED_DEAD:
        case AGENT_STATE_UNBUILT:
        case AGENT_STATE_BUILDING:
            continue;
        default:
            unit_num = aa->GetNUnits();
            ret = m_player->ValidateArmyID(id.GetVal(), unit_num);
            Assert(ret);
        }
    }
}







void AiMain::RegisterDeadUnit (uint32 army_id, sint32 unit_type)
{
    ArmyAgent *the_army=NULL;

    the_army = m_army_set->Find(army_id);
    Assert(the_army);

	#ifdef _DEBUG

	BOOL ret;
	sint32 units = the_army->GetNUnits();
	ret = m_player->ValidateArmyID(the_army->GetID().GetVal(), units);
	#endif

	if (the_army)
		the_army->RegisterDeadUnit(this, unit_type);
}







void AiMain::RegisterArmyWasGrouped(
   uint32 target,
   sint32 nUnits,
   sint32 unit_type[k_MAX_ARMY_SIZE],
   sint32 unit_hp[k_MAX_ARMY_SIZE]
) {
    Assert (1 < nUnits);
    Assert(nUnits <= k_MAX_ARMY_SIZE);
    BSetID target_id = target;
    ArmyAgent *aa = m_army_set->Find(target_id);
    Assert(aa);

    sint32 i;
    aa->ClearArmy(this);
    for (i=0; i<nUnits; i++) {
        aa->InsertUnitType(this, unit_type[i], unit_hp[i]);
    }
	aa->ResetArmyStrength(this);

	#ifdef _DEBUG
	BOOL ret;
	sint32 units = aa->GetNUnits();
	ret = m_player->ValidateArmyID(aa->GetID().GetVal(), units);
	#endif
}











void AiMain::RegisterLearnedScience(
   sint32 idx_sci,
   const CAUSE_SCI cause
) {

    BOOL is_new_unit_discovered=FALSE;
    m_science_agent->RegisterLearnedScience(this, idx_sci, cause, is_new_unit_discovered);

    m_wonder_table->RegisterNewScience(idx_sci);
    m_will_of_the_people->RegisterLearnedScience(this, idx_sci);


    if (is_new_unit_discovered) {
        BSetID id;
        CityAgent *ca=NULL;
        for (ca = m_city_set->First(id); m_city_set->Last(); ca = m_city_set->Next(id)) {
            ca->ComputePotentialUnits(this);
        }
    }
}









void AiMain::RegisterNewPlayer(
    PLAYER_INDEX player_id,
    CAUSE_NEW_PLAYER cause,
    PLAYER_INDEX old_owner)
{
     Assert(0 < player_id);
     Assert(player_id < k_MAX_PLAYERS);
     Assert(NULL == m_foreigner[player_id]);
     m_foreigner[player_id] = new Foreigner(player_id);
     m_num_players++;
     Assert(m_num_players <= k_MAX_PLAYERS);
     if (m_my_player_id == old_owner) {
         m_foreigner[player_id]->SetFZRegard(k_REGARD_INSANE_HATRED, k_REGARD_INSANE_HATRED, 0);
     } else {
         m_foreigner[player_id]->SetFZRegard(k_REGARD_FRIENDLY+1, k_REGARD_FRIENDLY+1, 0);
     }
}









void AiMain::RegisterDeadPlayer(
    PLAYER_INDEX player_id
    )
{
     Assert(0 < player_id);
     Assert(player_id < k_MAX_PLAYERS);

    if (player_id == my_player_index) {
        m_i_am_dead = TRUE;
    } else {
         Assert(m_foreigner[player_id]);
		 if (m_foreigner[player_id])
			 {
				 delete m_foreigner[player_id];
				 m_foreigner[player_id]=NULL;
				 m_num_players--;
				 Assert(0 <= m_num_players);
			 }
    }
}

void AiMain::RegisterAddInstallation(MapPointData &pos, sint32 inst_type)
{

    MapPointData w;

    w.x=0;
    w.y=0;
    w.z=0;
    if (m_installationDB->GetIsAirfield(inst_type, &w)) {
        m_airfield_list->InsertFlat(pos);
    }
}

void AiMain::RegisterRemoveInstallation(MapPointData &pos, sint32 inst_type)
{
    sint32 air_num, air_idx, dead_idx;
    BOOL searching=TRUE;
    MapPointData *nth_field=NULL;
    MapPointData w;
    w.x=0;
    w.y=0;
    w.z=0;

    if (m_installationDB->GetIsAirfield(inst_type, &w)) {

        air_num = m_airfield_list->Num();

        for (air_idx=0; searching && air_idx<air_num; air_idx++) {
            nth_field = &(m_airfield_list->Access(air_idx));

            if ((nth_field->x == pos.x) &&
                (nth_field->y == pos.y) &&
                (nth_field->z == pos.z)) {
                searching = FALSE;
                dead_idx = air_idx;
            }
        }

        if (!searching) {
            m_airfield_list->DelIndexFlat(dead_idx);
        } else {
            INSANE(REGISTER_REMOVE_INST_NO_AIRFIELD);
        }
    }
}











void AiMain::RegisterCityAttack(uint32 my_city_id, PLAYER_INDEX his_owner, uint32 his_army_id, UNIT_ORDER_TYPE attack_type)
{
    BSetID his_id(his_army_id);
    BSetID my_id(my_city_id);
    BSet<ForeignAgent> *foreign_agents=NULL;
	ForeignAgent *attacker;

    BSetID id(my_city_id);
	CityAgent *defender;

	Assert(m_foreigner[his_owner]);
	foreign_agents = m_foreigner[his_owner]->GetVisibleUnits();

	if (foreign_agents->GetNum() > 0)
	{
		attacker = foreign_agents->Find(his_id);
		if (attacker != NULL)
			return;
	}

	defender = m_city_set->Find(id);
	Assert(defender);

	switch (attack_type)
		{

            case UNIT_ORDER_PLANT_NUKE:
			case UNIT_ORDER_CREATE_PARK:
			case UNIT_ORDER_INVESTIGATE_CITY:
			case UNIT_ORDER_NULLIFY_WALLS:
			case UNIT_ORDER_STEAL_TECHNOLOGY:
			case UNIT_ORDER_INCITE_REVOLUTION:
			case UNIT_ORDER_ASSASSINATE:
			case UNIT_ORDER_INVESTIGATE_READINESS:
			case UNIT_ORDER_BOMBARD:
			case UNIT_ORDER_SUE:
			case UNIT_ORDER_FRANCHISE:
			case UNIT_ORDER_SUE_FRANCHISE:
			case UNIT_ORDER_CAUSE_UNHAPPINESS:

			case UNIT_ORDER_SLAVE_RAID:
			case UNIT_ORDER_UNDERGROUND_RAILWAY:
			case UNIT_ORDER_INCITE_UPRISING:
			case UNIT_ORDER_BIO_INFECT:
			case UNIT_ORDER_NANO_INFECT:
			case UNIT_ORDER_CONVERT:
			case UNIT_ORDER_REFORM:
			case UNIT_ORDER_INJOIN:
			default:

				MapPointData pos;
				defender->GetPos(pos);
				m_map->RegisterStealthAttack(pos);
		}
}












void AiMain::RegisterHostileAction(UNIT_ORDER_TYPE attack_type, uint32 my_army_id,
        PLAYER_INDEX his_owner, uint32 his_army_id, MapPointData *target_pos)
{
    double delta = 1.0;

    BOOL is_coldwar;
    switch(attack_type) {
	case UNIT_ORDER_INVESTIGATE_CITY:
	case UNIT_ORDER_NULLIFY_WALLS:
	case UNIT_ORDER_STEAL_TECHNOLOGY:
	case UNIT_ORDER_ASSASSINATE:
	case UNIT_ORDER_INVESTIGATE_READINESS:
	case UNIT_ORDER_SUE:
	case UNIT_ORDER_FRANCHISE:
	case UNIT_ORDER_SUE_FRANCHISE:

	case UNIT_ORDER_CAUSE_UNHAPPINESS:
	case UNIT_ORDER_SLAVE_RAID:
	case UNIT_ORDER_UNDERGROUND_RAILWAY:
	case UNIT_ORDER_INCITE_UPRISING:
	case UNIT_ORDER_BIO_INFECT:
	case UNIT_ORDER_NANO_INFECT:
	case UNIT_ORDER_CONVERT:
	case UNIT_ORDER_INDULGENCE:
	case UNIT_ORDER_SOOTHSAY:
	case UNIT_ORDER_INJOIN:
	case UNIT_ORDER_INCITE_REVOLUTION:
	case UNIT_ORDER_EXPEL:
	case UNIT_ORDER_INTERCEPT_TRADE:
        is_coldwar = TRUE;
     break;

	case UNIT_ORDER_PLANT_NUKE:
	case UNIT_ORDER_BOMBARD:
	case UNIT_ORDER_ENSLAVE_SETTLER:
	case UNIT_ORDER_FINISH_ATTACK:
	case UNIT_ORDER_CREATE_PARK:
    case UNIT_ORDER_INFORM_AI_CAPTURE_CITY:
        is_coldwar =FALSE;
        break;
    case UNIT_ORDER_PILLAGE:
        ((Opportunity_Action_Repair_Installation*)m_opportunity_action[OPPORTUNITY_ACTION_REPAIR_INSTALLATION])->
            RegisterHostileAction(this, *target_pos);
        is_coldwar=FALSE;
        break;

    default:
        INSANE(UNKNOWN_ORDER_TYPE);
        return;
	}

	Assert(m_foreigner[his_owner]);

    if (is_coldwar) {
        m_foreigner[his_owner]->ColdwarAttackedMe(this, delta);
    } else {
        m_foreigner[his_owner]->HotwarAttackedMe(this, delta);
    }

}

void AiMain::RegisterPollution(MapPointData *target_pos)
{

    ((Opportunity_Action_Repair_Installation*)m_opportunity_action[OPPORTUNITY_ACTION_REPAIR_INSTALLATION])->
            RegisterPollution(this, *target_pos);
}





















































void AiMain::RegisterPiracy(const PLAYER_INDEX he_did_it, const PLAYER_INDEX source_owner,
        const uint32 src_id, const PLAYER_INDEX dest_owner, const uint32 dest_id, const sint32 resource)
{
    if (m_foreigner[he_did_it]) {
        m_foreigner[he_did_it]->RegisterPiracy();
    }

    CityAgent *the_city=NULL;
    if (m_my_player_id == source_owner) {
        the_city = m_city_set->Find(src_id);
        Assert(the_city);
        the_city->RegisterPiracy(dest_owner, dest_id, m_round_count->GetRound());
    }

    if (m_my_player_id == dest_owner) {
        the_city = m_city_set->Find(dest_id);
        Assert(the_city);
        the_city->RegisterPiracy(source_owner, src_id, m_round_count->GetRound());
    }

}































































BSetID AiMain::GenUnbuiltID()

{
    BSetID tmpid = BitsFlip(m_unbuilt_army_count);
    m_unbuilt_army_count--;

    return tmpid;
}









void AiMain::RegisterCreateBuilding (
    uint32 city_id,
    const sint32 building_type)
{
    BSetID id(city_id);
    CityAgent *the_city;

    the_city = m_city_set->Find(id);
    the_city->RegisterCreateBuilding(this, building_type);
}









void AiMain::RegisterLostBuilding (
    uint32 city_id,
    const sint32 building_type)
{
    BSetID id(city_id);
    CityAgent *the_city;

    the_city = m_city_set->Find(id);

	if (!the_city) return;

    the_city->RegisterLostBuilding(this, building_type);

	if (m_capitol_id)
	{
		if (*m_capitol_id == city_id) {
			if (m_buildingDB->IsCapitolBuilding(building_type)) {
	            WeLostTheCapitol();
		    }
		}
    }
}









void AiMain::RegisterNewGovernment (
    uint32 city_id,
    const sint32 government_type)
{
    BSetID id(city_id);
    CityAgent *the_city;

    the_city = m_city_set->Find(id);
    the_city->RegisterNewGovernment(this, government_type);
}

void AiMain::RegisterNewWGF (sint32 w, sint32 g, sint32 f)
{
    if (m_alloc_WGF) {
        m_alloc_WGF->RegisterNewWGF (w, g, f);
    }
}








void AiMain::WeLostTheCapitol()
{
    if (m_capitol_id) {
        delete m_capitol_id;
        m_capitol_id = NULL;
    }

    m_capitol_state = CAPITOL_STATE_NONE;


    sint32 capitol_idx, building_idx;
    sint32 building_num = m_buildingDB->GetNumBuildings();
    for (building_idx =0; building_idx < building_num; building_idx++) {
        if (m_buildingDB->IsCapitolBuilding(building_idx)) {
            capitol_idx = building_idx;
            break;
        }
    }

    BSetID id;
    CityAgent *ca=NULL;
    for (ca = m_city_set->First(id); m_city_set->Last(); ca = m_city_set->Next(id)) {
        ca->ResetBuildingGoal(capitol_idx);
    }
}









void AiMain::RegisterNewCapitolBuilding (uint32 city_id)
{
    if (m_capitol_id) {
        delete m_capitol_id;
    }

    m_capitol_id = new BSetID(city_id);
    m_capitol_state = CAPITOL_STATE_HASIT;
}









void AiMain::RegisterCreateWonder (uint32 city_id, const sint32 wonder_type,
                                   PLAYER_INDEX owner)
{

	m_wonder_table->RegisterCreateWonder(city_id, wonder_type, m_my_player_id, owner);

	if (owner == my_player_index)
	{

		BSetID id(city_id);
	    CityAgent *the_city = m_city_set->Find(id);
	    Assert(the_city);

		if (the_city)
		{


			the_city->RegisterObsoleteBuild(this, k_GAME_OBJ_TYPE_WONDER, wonder_type);
		}
	}
}












sint32 AiMain::GetNumFuzzySections()
{
    return 	m_fliEngine->GetNumFuzzySections() + 1;

}








sint32 AiMain::GetNumFuzzyVariables(
   sint32 idx_section
   )
{
    if (idx_section == 0) {
        return m_stats->GetNumFuzzyVariables();
    } else {
        return m_fliEngine->GetNumFuzzyVariables(idx_section-1);
    }
}












void AiMain::GetFuzzyGraph(
    sint32 idx_section,
    sint32 idx_variable,
    char **label,

    double *minx,
    double *maxx,
    double *miny,
    double *maxy,
    sint32 *num_graphs,
    sint32 *num_x,
    double ***height,

    double *defuzz_val
    )
{

    if (idx_section == 0) {
       m_stats->GetFuzzyGraph(idx_section-1, idx_variable,
            label, minx, maxx, miny, maxy,
            num_graphs, num_x, height, defuzz_val);
    } else {
        m_fliEngine->GetFuzzyGraph(idx_section-1, idx_variable,
            label, minx, maxx, miny, maxy,
            num_graphs, num_x, height, defuzz_val);
   }
}








BOOL AiMain::ReloadFuzzyLogic(BOOL run_rules)
{
    Assert(m_fliEngine);
    delete m_fliEngine;
    m_fliEngine = NULL;

	FSDiplomat *diplomat = (FSDiplomat *)m_frame_machine[FS_FUZZY_DIPLOMACY];
    sint32 i;

    for (i=0; i<FLI_SECT_MAX; i++) {
    	diplomat->m_registered_foreigner_vars[i] = FALSE;
    }

	diplomat->m_registered_incoming_vars = FALSE;
    diplomat->m_registered_pre_outgoing = FALSE;
	diplomat->m_registered_received_vars = FALSE;

    ((FSFuzzyBeginTurnRules*)m_frame_machine[FS_FUZZY_BEGIN_TURN_RULES])->m_registered_variables = FALSE;

    BOOL r = InitFuzzyOut(m_gs, this);

    if (run_rules) {

        sint32 branch;
        m_frame_machine[FS_FUZZY_BEGIN_TURN_PRE]->Execute(this,  branch, 0);
        m_frame_machine[FS_FUZZY_BEGIN_TURN_DIPLOMACY]->Execute(this,  branch, 0);
        m_frame_machine[FS_FUZZY_BEGIN_TURN_RULES]->Execute(this,  branch, 0);
        m_frame_machine[FS_FUZZY_BEGIN_TURN_POST]->Execute(this,  branch, 0);

        return r;
    } else {
        return TRUE;
    }
}








void AiMain::ResetFuzzyInput(sint32 idx_section, sint32 idx_variable,
        double new_defuzz_val)
{
    m_fliEngine->ResetFuzzyInput(idx_section, idx_variable,
        new_defuzz_val);
}









void AiMain::DumpStats()

{
   m_stats->Dump(this);
}









void AiMain::RegisterDiplomaticRequest(
    IC3DiplomaticRequest *request
){
	FSDiplomat *diplomat = (FSDiplomat *)m_frame_machine[FS_FUZZY_DIPLOMACY];
	diplomat->HandleIncomingMessage(this, request);
}









void AiMain::RegisterDiplomaticResponse(IC3DiplomaticRequest *request)
{
	FSDiplomat *diplomat = (FSDiplomat *)m_frame_machine[FS_FUZZY_DIPLOMACY];
	diplomat->HandleIncomingResponse(this, request);
}










void AiMain::RegisterYouAttacked(PLAYER_INDEX defender)
{

	if (m_foreigner[defender])
	{

        if ((defender != 0) && (m_my_player_id != 0)) {
            double r = m_foreigner[defender]->GetUnitRegard() + m_foreigner[defender]->GetBonusRegard();
            if (k_REGARD_HOTWAR < r) {
                sint32 ai_attacked_by_accident=0;
                Assert(ai_attacked_by_accident);
            }
        }

		m_foreigner[defender]->AttackedByMe();
	}
}

void AiMain::RegisterFightWinner(sint32 attack_owner, sint32 attack_start_num,
        sint32 defense_owner, sint32 defense_start_num, sint32 winner)
{
    sint32 lost_units;
    if (attack_owner == winner) {
        lost_units = defense_start_num;
    } else {
        lost_units = attack_start_num;
    }

    if (attack_owner == m_my_player_id) {
        if (m_foreigner[defense_owner]) {
            m_foreigner[defense_owner]->RegisterFightWinner(lost_units, winner);
        }
    } else {
        if (m_foreigner[attack_owner]) {
            m_foreigner[attack_owner]->RegisterFightWinner(lost_units, winner);
        }
    }
}









void AiMain::RegisterAttackedBy(PLAYER_INDEX agressor)
{





}








void AiMain::SendDiplomaticRequest(AIDiplomaticRequest *request)
{
	if(m_foreigner[request->m_recipient]) {
		m_foreigner[request->m_recipient]->RegisterSentRequestTo(this, request->m_request);
	}

	m_gs->SendDiplomaticRequest(request);

}









void AiMain::RegisterContactMade(PLAYER_INDEX contacted)
{
	Assert(m_foreigner[contacted]);
	if(m_foreigner[contacted]) {
		m_foreigner[contacted]->ContactMade();
	}
}

void AiMain::RegisterLeaveOurLands(PLAYER_INDEX get_off_me)
{
    Assert(m_foreigner[get_off_me]);
    m_foreigner[get_off_me]->RegisterLeaveOurLands();
}

void AiMain::RegisterLeaveOurLandsExpired (PLAYER_INDEX get_off_me)
{
    Assert(m_foreigner[get_off_me]);
    m_foreigner[get_off_me]->RegisterLeaveOurLandsExpired();
}








void AiMain::RegisterPlayerInfo(PLAYER_INDEX index, C3PlayerInfo *pi)
{
	Assert(m_foreigner[index]);
	if(m_foreigner[index]) {
		m_foreigner[index]->RegisterPlayerInfo(pi);
	}
}








sint32 AiMain::GetTradeWith(PLAYER_INDEX second, PLAYER_INDEX third)
{
	return m_gs->GetTradeWith(second, third);
}








double AiMain::GetStrength(PLAYER_INDEX player)
{
	double strength = m_gs->GetStrength(player);
	if(strength < 1)
		strength = 1;
	return strength;
}







double AiMain::GetMyDefense()
{

	return 5;
}







void AiMain::Init_Logging()
{
#ifndef _DEBUG
return;
#endif

	char log_file_name[200];
	FILE * fp;

	sprintf(log_file_name, "logs\\LOGAI%d.txt", my_player_index);

	fp = fopen(log_file_name, "w");








	fclose(fp);
}









void AiMain::Log(char * message)
{
#ifndef _DEBUG
return;
#endif

	char log_file_name[200];
	FILE * fp;
	bool done = false;
	int attempts = 0;
	const int MAX_ATTEMPTS = 3;
	const int PAUSE_INTERVAL = 2;

	sprintf(log_file_name, "logs\\LOGAI%d.txt", my_player_index);

	int a1 = errno;
	int b1 = _doserrno;
	DWORD c1 = GetLastError();

	while ((!done) && (attempts < MAX_ATTEMPTS))
	{

		fp = fopen(log_file_name, "a");

		if (fp)
		{

			fprintf(fp, message);









			fclose(fp);
			done = true;
		}
		else
		{
			int a2 = errno;
			int b2 = _doserrno;
			DWORD c2 = GetLastError();

			attempts++;

			DWORD time = timeGetTime();
			while ((time + PAUSE_INTERVAL) > timeGetTime())
			{
			}
		}
	}

	if (!done)
	{

		_asm int 0x03;

	}
}










void AiMain::SetPersonalityFilename(
    char *name
)
{
	m_personality_file = new char[strlen(name) + 1];
	strcpy(m_personality_file, name);
}









const char *AiMain::GetPersonalityFilename()
{
	return m_personality_file;
}

void AiMain::ReceiveTradeBid(uint32 fromCity,
							 sint32 resource,
							 sint32 toCityOwner,
							 uint32 toCity,
							 sint32 price,
							 uint32 bidId)
{
	FSDiplomat *diplomat = (FSDiplomat *)m_frame_machine[FS_FUZZY_DIPLOMACY];
	diplomat->ReceiveTradeBid(this, fromCity,
									 resource,
									 toCityOwner,
									 toCity,
									 price,
									 bidId);
}

void AiMain::RejectTradeBid(uint32 bidId)
{
	m_player->RejectTradeBid(bidId);
}

void AiMain::AcceptTradeBid(uint32 bidId)
{
	m_player->AcceptTradeBid(bidId);
}

void AiMain::RegisterTradeOffer(IC3TradeOffer *offer)
{
	FSDiplomat *diplomat = (FSDiplomat *)m_frame_machine[FS_FUZZY_DIPLOMACY];
	diplomat->RegisterTradeOffer(this, offer);
}

void AiMain::RemoveTradeOffer(uint32 id)
{
	FSDiplomat *diplomat = (FSDiplomat *)m_frame_machine[FS_FUZZY_DIPLOMACY];
	diplomat->RemoveTradeOffer(this, id);
}

CityAgent *AiMain::GetBestCityForResource(sint32 resource)
{
    BSetID id;
    CityAgent *the_city;
	sint32 highestCount = 0;
	CityAgent *highestCity = NULL;
    BOOL is_unknown_id;
    sint32 in_num;
    sint32 in_max;
    sint32 out_num;
    sint32 out_max;
    sint32 local_good_count;
    sint32 total_good_count;

	for(the_city = m_city_set->First(id); m_city_set->Last(); the_city = m_city_set->Next(id)) {

		m_player->GetGoodCount(&is_unknown_id, the_city->m_id.GetVal(),
            resource, &local_good_count, &total_good_count);

		if(total_good_count > highestCount) {
            m_player->GetCityNumTradeRoutes (m_my_player_id, the_city->GetID().GetVal(), &is_unknown_id,
                    &in_num, &in_max, &out_num, &out_max);

            if (in_num >= in_max) continue;

			highestCount = total_good_count;
			highestCity = the_city;
		}
	}

	return highestCity;
}

BOOL AiMain::ConfirmAcceptTradeOffer(uint32 offer,
									 uint32 destCityId,
									 PLAYER_INDEX player)
{
	FSDiplomat *diplomat = (FSDiplomat *)m_frame_machine[FS_FUZZY_DIPLOMACY];
	return diplomat->ConfirmAcceptTradeOffer(this, offer, destCityId, player);
}

void AiMain::RegisterNewContinents()
{
    m_continents->RegisterNewContinents(this);
}

void AiMain::RegisterYouBrokeCeaseFire(PLAYER_INDEX with)
{

}

void AiMain::RegisterCeaseFireBroken(PLAYER_INDEX by)
{
	Assert(m_foreigner[by]);
	if(m_foreigner[by]) {
		m_foreigner[by]->HeBrokeCeaseFire();
	}
}

void AiMain::RegisterYouBrokeAlliance(PLAYER_INDEX with)
{

}

void AiMain::RegisterAllianceBroken(PLAYER_INDEX by)
{
	Assert(m_foreigner[by]);
	if(m_foreigner[by]) {
		m_foreigner[by]->HeBrokeAlliance();
	}
}

void AiMain::RegisterYourArmyWasMoved(uint32 army_id, MapPointData *new_pos)
{

    ArmyAgent *aa = m_army_set->Find(army_id);

    Assert(aa);

    aa->UpdatePosition(this, new_pos);
}

void AiMain::RegisterHisArmyTeleported (PLAYER_INDEX his_owner, uint32 i_moved,
        MapPointData *new_pos)
{
    Assert(m_foreigner[his_owner]);
    m_foreigner[his_owner]->RegisterHisArmyTeleported (this, i_moved, new_pos);
}

void AiMain::RegisterHeNuked(PLAYER_INDEX he_did_it, PLAYER_INDEX his_target)
{
    Assert(m_foreigner[he_did_it]);
	if(m_foreigner[he_did_it]) {
		m_foreigner[he_did_it]->RegisterHeNuked(m_my_player_id, his_target);
	}
}

void AiMain::RegisterHeBuiltNewAgeUnit(PLAYER_INDEX he_did_it, NEW_AGE unit_type)
{
    Assert(m_foreigner[he_did_it]);
	if(m_foreigner[he_did_it]) {
		m_foreigner[he_did_it]->RegisterHeBuiltNewAgeUnit(unit_type);
	}
}

void AiMain::RegisterRegardIncrease(PLAYER_INDEX who, sint32 amount)
{
	Assert(m_foreigner[who]);
	if(m_foreigner[who]) {
		m_foreigner[who]->RegisterRegardIncrease(amount);
	}
}

void AiMain::GetFZRegard (sint32 him, sint32 &diplomatic, sint32 &unit,
        sint32 &bonus)
{
    Assert(m_foreigner[him]);
	if (m_foreigner[him]) {
	    m_foreigner[him]->GetFZRegard(diplomatic, unit, bonus);
	}
}

void AiMain::SetFZRegard (sint32 him, sint32 diplomatic, sint32 unit,
        sint32 bonus)
{
    Assert(m_foreigner[him]);
    m_foreigner[him]->SetFZRegard(diplomatic, unit, bonus);

}

void AiMain::GetAllRegard(double i_like[k_MAX_PLAYERS])
{
    sint32 player_idx;
    sint32 diplomatic;
    sint32 unit;
    sint32 bonus;

    for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) {
        if (m_foreigner[player_idx]) {
            m_foreigner[player_idx]->GetFZRegard(diplomatic, unit, bonus);
            i_like[player_idx] = diplomatic;
        } else {
            i_like[player_idx] = 100.0;
        }
    }
}

void AiMain::RegisterYouAreViolatingAgreement(AGREEMENT_TYPE agreement,
											  PLAYER_INDEX against,
											  sint32 rounds)
{
	FSDiplomat *diplomat = (FSDiplomat *)m_frame_machine[FS_FUZZY_DIPLOMACY];
	diplomat->RegisterYouAreViolatingAgreement(this, agreement,
											   against, rounds);
}

void AiMain::RegisterOtherCivIsViolatingAgreement(AGREEMENT_TYPE agreement,
												  PLAYER_INDEX violator,
												  sint32 rounds)
{
	FSDiplomat *diplomat = (FSDiplomat *)m_frame_machine[FS_FUZZY_DIPLOMACY];
	diplomat->RegisterOtherCivIsViolatingAgreement(this, agreement,
												   violator, rounds);
}

void AiMain::RegisterAttack(PLAYER_INDEX attacker,
							PLAYER_INDEX defender)
{
	FSDiplomat *diplomat = (FSDiplomat *)m_frame_machine[FS_FUZZY_DIPLOMACY];
	diplomat->RegisterAttack(this, attacker, defender);
}

void AiMain::RegisterBrokeTreaty(PLAYER_INDEX breaker,
								 PLAYER_INDEX breakee)
{
	FSDiplomat *diplomat = (FSDiplomat *)m_frame_machine[FS_FUZZY_DIPLOMACY];
	diplomat->RegisterBrokeTreaty(this, breaker, breakee);
}







void AiMain::UnloadRefueledCargo()
{
    BSetID id;
    ArmyAgent *agent;

    for (agent = m_army_set->First(id);
		 m_army_set->Last();
		 agent = m_army_set->Next(id))
		{
			agent->UnloadRefueledCargo(this);
		}


}




void AiMain::BombardAdjacentEnemies()
{
    BSetID id;
    ArmyAgent *agent;
	MapPointData my_pos;
	sint32 player_id;
	uint32 a_id;
	sint32 top_unit_type;
	sint32 num_units;
	uint32 c_id;
	BOOL unitVis;
	BOOL cityVis;
	BOOL mineVis;
	BOOL is_unknown_id;
	MapPointData best_target_pos;
	MapPointData wrap_pos;
	double best_threat = 0.0;
	double attack;
	double defense;
	double hp;
	double firepower;
	double threat;
	sint16 dx,dy;

    for (agent = m_army_set->First(id);
		 m_army_set->Last();
		 agent = m_army_set->Next(id))
		{

			 if (agent->GetMovePoints() > 0 &&
				 agent->CanBombard(this, 0xffffffff) > 0)
			 {
				 agent->GetPos(this, my_pos);
				 best_target_pos = my_pos;


				 for (dx = -1; dx < 2;  dx++)
					 for (dy = -1; dy < 2;  dy++)
					 {

						 if (dx == 0 && dy == 0)
							 continue;

						 m_map->Wrap(my_pos.x + dx,
							 my_pos.y + dy,
							 my_pos.z,
							 wrap_pos);

						 m_world->GetCityAndArmyId( &wrap_pos,
							 &player_id,
							 &a_id,
							 &top_unit_type,
							 &num_units,
							 &c_id,
							 &unitVis,
							 &cityVis,
							 &mineVis );


						 if ((unitVis || cityVis) &&
							 player_id != my_player_index &&
							 m_foreigner[player_id] &&
							 m_foreigner[player_id]->IsAtHotWar() &&
							 agent->CanBombardTarget(this,wrap_pos))
						 {
							 attack = m_unitDB->GetAttack(top_unit_type);
							 defense = m_unitDB->GetDefense(top_unit_type);
							 hp = m_unitDB->GetHP(top_unit_type);
							 firepower = m_unitDB->GetFirepower(top_unit_type);
							 threat = (attack + defense) * hp * firepower * num_units;

							 if (m_unitDB->IsLandBombard(top_unit_type) ||
								 m_unitDB->IsMountainBombard(top_unit_type) ||
								 m_unitDB->IsWaterBombard(top_unit_type) ||
 								 m_unitDB->IsAirBombard(top_unit_type) ||
								 m_unitDB->IsSpaceBombard(top_unit_type))
								 threat *= 2;

							 if (threat > best_threat)
							 {
								 best_threat = threat;
								 best_target_pos = wrap_pos;
							 }
						 }
					 }

					 if (best_target_pos != my_pos)
					 {
						 m_player->Bombard( agent->GetID().GetVal(), &is_unknown_id,
											  &best_target_pos);
					 }
			 }
		 }
}

void AiMain::DisbandObsoleteArmies()
{
    BSetID id;
    ArmyAgent *agent;

    for (agent = m_army_set->First(id);
	m_army_set->Last();
	agent = m_army_set->Next(id))
	{

		if (agent->DisbandObsolete(this) > 0)
			return;
	}
}

void  AiMain::GetCurrentAIPname(char str[100])
{
    m_planner->the_AIP.GetCurrentAIPname(str);
}

void AiMain::RegisterRemoveBuildNode(PLAYER_INDEX owner,
			uint32 cityid,
			sint32 category,
			sint32 type)
{
    BSetID id(cityid);
    CityAgent *the_city = m_city_set->Find(id);
    Assert(the_city);




























	if (the_city)
		the_city->RegisterObsoleteBuild(this,
										category,
										type);
}

void AiMain::RegisterSetBuildFront(uint32 cityid,
								   sint32 category,
								   sint32 type)
{
    BSetID id(cityid);
    CityAgent *the_city = m_city_set->Find(id);
    Assert(the_city);

	if (the_city)
		the_city->RegisterSetBuildFront(category,
										type);
}

BOOL AiMain::IWantToAttackRevealedArmy (uint32 revealed_army_id, MapPointData *pos,
     PLAYER_INDEX reveled_player, uint32 attacking_army_id)
{
    return TRUE;
}

void AiMain::ForceRegard(sint32 forPlayer, double toRegard)
{
	Assert(m_foreigner[forPlayer]);
	if(m_foreigner[forPlayer]) {
		m_foreigner[forPlayer]->SetDiplomaticRegard(this, toRegard);
	}
}

void AiMain::RegisterAgreementAgainstMe(sint32 first_player,
    sint32 second_player, double str)
{
    Assert(m_foreigner[first_player]);
    if (m_foreigner[first_player])
        m_foreigner[first_player]->RegisterAgreementAgainstMe(str);

    Assert(m_foreigner[second_player]);
    if (m_foreigner[second_player])
        m_foreigner[second_player]->RegisterAgreementAgainstMe(str);

}

void AiMain::ForceHate(sint32 forPlayer)
{
	Assert(m_foreigner[forPlayer]);
	if(m_foreigner[forPlayer]) {
		m_foreigner[forPlayer]->SetInsaneHatred(TRUE);
	}
}
