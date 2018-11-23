#include "c3.h"
#include "globals.h"
#include "c3types.h"
#include "c3errors.h"
#include "globals.h"
#include "MoveFlags.h"
#include "ic3player.h"
#include "ic3BlgDB.h"
#include "ic3Population.h"
#include "ic3UnitDB.h"
#include "ic3world.h"
#include "ic3InstDB.h"
#include "ic3Rand.h"
#include "ic3Wonder.h"
#include "ic3endgamedb.h"

#include "MapPoint.h"
#include "FlatPtr.h"
#include "civarchive.h"

#include "ic3GameState.h"
#include "aicause.h"
#include "aicell.h"

#include "aimain.h"
#include "AiMap.h"
#include "airndcnt.h"

#include "common.h"
#include "dr_debug.h"
#include "linked_list.h"
#include "semi_dynamic_array.h"
#include "sorted_array.h"
#include "list_array.h"

#include "ArmyAgent.h"
#include "CityAgent.h"
#include "AiBldQue.h"
#include "bset.h"
#include "civarchive.h"
#include "ForeignAgent.h"
#include "ForeignCity.h"
#include "Foreigner.h"
#include "WonderTbl.h"
#include "Cont.h"

#include "FzOut.h"

#include "InstBidList.h"
#include "GoalInst.h"

#include "BldAgent.h"

#include "ArmyGoal.h"
#include "glblg.h"
#include "glfreight.h"
#include "GoalMapTarget.h"
#include "GoalRetreat.h"




#include "unitflow.h"
#include "aip.h"
#include "squad.h"
#include "goal.h"

#include "GoalDefense.h"

#include "Scheduler.h"
#include "planner.h"
#include "grassfire.h"
#include "strategic_map.h"

#include "AllocWGF.h"
#include "CityGrowth.h"
#include "CityProd.h"
#include "CityFP.h"
#include "scienceagent.h"
#include "squad_strength.h"

#include "IC3Trade.h"
#include "Pirate_Marker.h"
#include "pointerlist.h"

#define k_SIZE_COEF 5

extern double fz_freight_coef;
extern double fz_production_tax;
extern double fz_city_threat_decay;
extern double fz_build_time_decay;

extern double fz_inst_prod;
extern double fz_inst_food;
extern double fz_inst_move;
extern double fz_inst_def;
extern double fz_inst_vision_range;
extern double fz_inst_airfield;
extern double fz_inst_fort;
extern double fz_inst_attack;
extern double fz_inst_firepower;
extern double fz_inst_threat_threshold;






extern sint32 g_nBuildings;

extern double fz_wonder_uscale;
extern double fz_wonder_effect_embassy;
extern double fz_wonder_effect_close_embassy;
extern double fz_wonder_effect_reform_cities;
extern double fz_wonder_effect_free_slaves;
extern double fz_wonder_effect_global_radar;
extern double fz_wonder_effect_spies_everywhere;
extern double fz_wonder_effect_free_space_trans;
extern double fz_wonder_effect_stop_biowar;
extern double fz_wonder_effect_pop_monitor;
extern double fz_wonder_effect_enable_parkranger;
extern double fz_wonder_effect_all_pop_content;
extern double fz_wonder_effect_disable_nukes;

extern double fz_wonder_effect_reduce_readiness_cost;
extern double fz_wonder_effect_increase_gold;
extern double fz_wonder_effect_decrease_crime;
extern double fz_wonder_effect_increase_knowledge;
extern double fz_wonder_effect_decrease_empire_size;
extern double fz_wonder_effect_increase_happy_empire;
extern double fz_wonder_effect_increase_converted_city_fee;
extern double fz_wonder_effect_tax_water_routes;
extern double fz_wonder_effect_tax_television;
extern double fz_wonder_effect_increase_happy;
extern double fz_wonder_effect_make_genetics_cheap;
extern double fz_wonder_effect_tax_international_route;
extern double fz_wonder_effect_polluters_to_parks;
extern double fz_wonder_effect_reduce_world_pollution;
extern double fz_wonder_effect_pollution_bonus;
extern double fz_wonder_effect_pollution_cleaner;


CityAgent::CityAgent()
{
    ZeroInit();
}

CityAgent::CityAgent(AiMain *ai, BSetID id, MapPointData *pos)
{
	m_id = id;
    ZeroInit();
    MemInit(ai, pos);
}

void CityAgent::ZeroInit()
{
   m_agent_type = AGENT_TYPE_CITY;

    m_stored_production = 0;
    m_gross_production = 0;
    m_net_production = 0;
    m_projected_gross_production = 5;
    m_projected_net_production = 5;

    m_stored_food = 0;
    m_gross_food = 0;
    m_net_food = 0;
    m_projected_food = 10;

    m_gross_gold = 0;
    m_net_gold = 0;
    m_projected_gross_gold = 0;
    m_projected_net_gold = 0;

    m_gross_science = 0;

    m_status_bits = 0;

    m_delta_threat_strength = 0.0;
    m_threat_strength = 0.0;
    temp_goal_relief_def_str = 0.0;

    m_defenders_nUnits=0;
    m_defenders_ranged=0.0;
    m_defenders_attack_strength=0.0;
    m_defenders_defense_strength=0.0;
	m_min_slave_garrison = 0;

    m_is_materials_tax_exempt = FALSE;






    m_num_slaves = 0;
    m_old_happiness = 0;
    m_old_crime = 0.0;
    m_cop = 0.0;
    m_total_field_prod = 0;
    m_total_field_food = 0;
    m_pop_needs_changing = TRUE;
    m_num_field_workers = 1;
    m_total_field_food = 10;
    m_total_field_prod = 5;
    m_ave_prod_tile = 10.0;
    m_ave_food_tile = 5.0;
    m_old_happiness = 65;

    m_road_list = NULL;

    m_defend_me_goal_id = UNASSIGNED_ID;
	m_retreatGoalID = UNASSIGNED_ID;
	m_defend_me_goal_set = false;


    m_pos = NULL;
    m_building = NULL;
    m_production_bonus = NULL;

    m_list_of_potential_units = NULL;
    m_has_space_launcher = FALSE;
    m_has_airport = FALSE;
    m_has_granary =FALSE;

	m_tradeOffers = NULL;
	for (uint32 i = 0; i< GOAL_TYPE_MAX; i++)
		m_bestUnitForGoal[i] = NULL;
	m_initialized = FALSE;
	m_turnCaptured = -1;
	m_queueItemType = -1;
	m_queueItemCat = -1;
	m_buildCapitalization = FALSE;
    m_pirate_head = NULL;
	m_garrisonCount = 0;
}

void CityAgent::MemInit(AiMain *ai, MapPointData *pos)
{







    m_pos = new MapPointData();
    *m_pos = *pos;
    m_building = new BuildingAgent[g_nBuildings];

    sint16 i;
    for (i=0; i<g_nBuildings; i++) {
        m_building[i].SetType(i);

    }

    m_production_bonus = new double [4];

    BOOL is_blg_built[64];
    BOOL is_wndr_built[64];

    BOOL is_unknown_id;
    ai->m_player->CityFindWhatIsBuilt(m_id.GetVal(), &is_unknown_id, is_blg_built, is_wndr_built);
    Assert(is_unknown_id == FALSE);

    for (i=0; i<ai->m_nBuildings; i++) {
        if (is_blg_built[i]) {
            m_building[i].SetState(BUILDING_STATE_DONE);
            if (ai->m_buildingDB->IsSpaceLauncher(i)) {
                SetHasSpaceLauncher(TRUE);
            }

            if (ai->m_buildingDB->IsGranary(i)) {
                m_has_granary = TRUE;
            }
         }
    }

    sint32 n;
    n = ai->m_wonder_table->GetNumWonders();
    for (i=0; i<n; i++) {
        if (is_wndr_built[i]) {
            ai->m_wonder_table->RegisterCapturedWonder(i);
        }
    }

	int num_unit_types = ai->m_unitDB->GetNumUnitTypes();

    m_list_of_potential_units = new list_array<ArmyAgent *> (num_unit_types);

	m_agent_strength->AddArmies(1);

	m_agent_strength->AddUnits(1);
}

CityAgentPtr::~CityAgentPtr()
{
    NDELETE (m_pos);


    delete[] m_building;
    m_building = NULL;

    delete[] m_production_bonus;
    m_production_bonus = NULL;





















	if(m_tradeOffers) {
		while(m_tradeOffers->GetHead()) {
			m_tradeOffers->RemoveHead()->Release();
		}
		delete m_tradeOffers;
        m_tradeOffers = NULL;
	}

#ifdef _DEBUG
    sint32 finite_loop=0;
#endif

    RoadDest *p=NULL, *del_me=NULL;
    p = m_road_list;
    while (p) {
Assert(++finite_loop < 100000);
        del_me = p;
        p = p->m_next;
        delete del_me;
    }
    m_road_list = NULL;

    ArmyAgent *aa=NULL;
    sint32 i;
    for (i=0; i < m_list_of_potential_units->count; i++) {
        aa = (ArmyAgent *) m_list_of_potential_units->Return_Data_By_Number(i);
        delete aa;
    }
    delete m_list_of_potential_units;
    m_list_of_potential_units = NULL;

    Pirate_Marker *pirate_ptr=NULL, *pirate_del_me=NULL;
    pirate_ptr = m_pirate_head;
    while (pirate_ptr) {
        pirate_del_me = pirate_ptr;
        pirate_ptr = pirate_ptr->m_next;
        delete pirate_del_me;
    }
    m_pirate_head= NULL;

}

CityAgent::~CityAgent()

{

	Set_Defend_Me_Status(FALSE);
}

ZEROMEM(CityAgentFlat);
FLATSERIALIZE(CityAgentFlat);
ZEROMEM(CityAgentPtr);

void CityAgentPtr::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) {
        Store(archive);
    } else {
        memset(this, 0, sizeof(*this));
        Load(archive);
    }
}

void CityAgentPtr::Store(CivArchive &archive)
{

    Assert(m_pos);
    archive << m_pos->x;
    archive << m_pos->y;
    archive << m_pos->z;






    sint32 i;
    for (i=0; i<g_nBuildings; i++) {
        m_building[i].Serialize(archive);
    }






































    Assert(m_list_of_potential_units);
	archive << (sint32) m_list_of_potential_units->count;

	archive << (sint32) m_list_of_potential_units->Get_Max_Size();

	ArmyAgent	*aa;
	for (i = m_list_of_potential_units->count - 1; i >= 0; i--)
	{
		aa = (ArmyAgent *) m_list_of_potential_units->Return_Data_By_Number( i );
	    aa->Serialize((IC3CivArchive *)&archive);
	}

#ifdef _DEBUG
    sint32 finite_loop=0;
#endif

    if (m_road_list) {
        archive.PutSINT32(1);

        RoadDest *p = m_road_list;
        while (p) {
            Assert(++finite_loop < 100000);

            p->Serialize(archive);
            if (p->m_next) {
                archive.PutSINT32(1);
                p = p->m_next;
            } else {
                archive.PutSINT32(0);
                break;
            }
        }
    } else {
        archive.PutSINT32(0);
    }

    Pirate_Marker *pirate_ptr=NULL;
    sint32 pirate_count=0;
    pirate_ptr = m_pirate_head;
    while (pirate_ptr) {
        pirate_count++;
        pirate_ptr = pirate_ptr->m_next;
    }

    archive << pirate_count;
    pirate_ptr = m_pirate_head;
    while (pirate_ptr) {
        pirate_ptr->Serialize(archive);
        pirate_ptr = pirate_ptr->m_next;
    }

}

void CityAgentPtr::Load(CivArchive &archive)
{

    m_pos = new MapPointData;
    archive >> m_pos->x;
    archive >> m_pos->y;
    archive >> m_pos->z;





    sint32 i;
    m_building = new BuildingAgent[g_nBuildings];
    for (i=0; i<g_nBuildings; i++) {
        m_building[i].Serialize(archive);
    }
    Assert(m_building);

    m_production_bonus = new double [4];










































    sint32 pcount;
	archive >> pcount;

	sint32 num_unit_types;

	archive >> num_unit_types;

    m_list_of_potential_units = new list_array<ArmyAgent *> (num_unit_types);

	ArmyAgent	*aa;
	for (i = pcount - 1; i >= 0; i--)
	{
	    aa = new ArmyAgent((IC3CivArchive *)&archive);
        m_list_of_potential_units->Append_Data(aa);
	}

    sint32 has_road_list = archive.GetSINT32();
    if (has_road_list) {

        m_road_list = new RoadDest(archive);

        RoadDest *p;
        sint32 has_another;

        p = m_road_list;
#ifdef _DEBUG
    sint32 finite_loop=0;
#endif
        for (has_another = archive.GetSINT32(); has_another; has_another = archive.GetSINT32()) {
            Assert(++finite_loop < 100000);

            p->m_next = new RoadDest(archive);
            p = p->m_next;
        }
        p->m_next=NULL;
    }

    Pirate_Marker *pirate_ptr=NULL;
    sint32 pirate_count, pirate_idx;
    archive >> pirate_count;

    m_pirate_head = NULL;
    for (pirate_idx=0; pirate_idx<pirate_count; pirate_idx++) {
        pirate_ptr = new Pirate_Marker(archive);
        pirate_ptr->m_next = m_pirate_head;
        m_pirate_head = pirate_ptr;
    }

}

CityAgent::CityAgent(IC3CivArchive *a)
{
    ZeroInit();
    Serialize(a);
}

void CityAgent::Serialize(IC3CivArchive *archive)
{
    CivArchive *a = (CivArchive *) archive;

    Agent::Serialize(*a);
    CityAgentFlat::Serialize(*a);
	for (uint32 i = 0; i< GOAL_TYPE_MAX; i++)
		m_bestUnitForGoal[i] = NULL;
    CityAgentPtr::Serialize(*a);
}

BOOL CityAgent::Validate(AiMain *ai)
{
    BOOL r = Agent::Validate(ai);

    Assert(r);
    return r;
}

void CityAgent::BeginTurn (AiMain *ai)

{
    BOOL is_unknown_id;

	m_buildCapitalization = FALSE;

    m_total_citizens = ai->m_player->GetCityPopulation(ai->m_my_player_id, m_id.GetVal(), &is_unknown_id);
    Assert(!is_unknown_id);

	if(!ai->IsBeingAttached()) {

		Assert(GetQueueLen() == ai->m_player->CityBuildQueueLen(m_id.GetVal(), &is_unknown_id));
	}

	if(GetQueueLen() != ai->m_player->CityBuildQueueLen(m_id.GetVal(), &is_unknown_id))
		{
			if (ai->m_player->CityBuildQueueLen(m_id.GetVal(), &is_unknown_id) > 0)

				ai->m_player->CityBuildQueueFront(m_id.GetVal(), &is_unknown_id,
												m_queueItemType, m_queueItemCat);
			else
				m_queueItemType = -1;
		}

	if (m_defend_me_goal_set) {
		Assert(m_defend_me_goal_id != UNASSIGNED_ID);
	} else {
		Assert(m_defend_me_goal_id == UNASSIGNED_ID);
	}

	if (m_initialized)
		{
			ComputePotentialUnits(ai);
			m_initialized = FALSE;
		}




    m_stored_production = ai->m_player->CityGetStoredProduction(m_id.GetVal(), &is_unknown_id);
    Assert(is_unknown_id == FALSE);
    m_gross_production = ai->m_player->CityGetGrossProduction(m_id.GetVal(), &is_unknown_id);
    Assert(is_unknown_id == FALSE);
    m_net_production = ai->m_player->CityGetNetProduction(m_id.GetVal(), &is_unknown_id);
    Assert(is_unknown_id == FALSE);
	ai->m_population->GetCityProjectedProduction(m_id.GetVal(), &m_projected_gross_production, &is_unknown_id);

    m_stored_food = ai->m_player->CityGetStoredFood(m_id.GetVal(), &is_unknown_id);
    Assert(is_unknown_id == FALSE);
    m_gross_food = ai->m_player->CityGetGrossFood(m_id.GetVal(), &is_unknown_id);
    Assert(is_unknown_id == FALSE);
    m_net_food = ai->m_player->CityGetNetFood(m_id.GetVal(), &is_unknown_id);
    Assert(is_unknown_id == FALSE);
    ai->m_population->GetCityProjectedFood(m_id.GetVal(), &m_projected_food, &is_unknown_id);

    m_accumulated_growth = ai->m_player->GetAccumulatedGrowthFood (m_id.GetVal(), &is_unknown_id);
    Assert(is_unknown_id == FALSE);
    m_accumulated_threshold = ai->m_player->GetAccumulatedGrowthThreshold(m_id.GetVal(), &is_unknown_id);
    Assert(is_unknown_id == FALSE);

    m_gross_gold = ai->m_player->CityGetGrossGold(m_id.GetVal(), &is_unknown_id);
    Assert(is_unknown_id == FALSE);
    m_net_gold = ai->m_player->CityGetNetGold(m_id.GetVal(), &is_unknown_id);
    Assert(is_unknown_id == FALSE);

    ai->m_population->GetCityProjectedTrade(m_id.GetVal(),
        &is_unknown_id,  &m_projected_gross_gold, &m_projected_net_gold);

    m_gross_science = ai->m_player->CityGetGrossScience(m_id.GetVal(), &is_unknown_id);

    temp_goal_relief_def_str = 0;

    CalcEstimatedCityProduction();

	m_num_slaves = ai->m_population->GetCitySlaveCount(m_id.GetVal(), &is_unknown_id);

	if (m_num_slaves > 0)
		m_min_slave_garrison = (sint32) ceil(m_num_slaves / ai->m_player->GetSlavesPerMilitaryUnit());
	else
		m_min_slave_garrison = 0;


	ResetDefenseStrength(0, 0.0, 0.0, 0.0);

	PLAYER_INDEX owner;
	sint32 unit_count;
	sint32 unit_type[k_MAX_ARMY_SIZE];
	sint32 unit_hp[k_MAX_ARMY_SIZE];
	BOOL is_entrenched;
	m_garrisonCount = 0;

	if (ai->m_world->GetUnitData(m_pos, &owner, &unit_count,
							 unit_type, unit_hp, &is_entrenched))
	{
		for (int i=0; i < unit_count; i++)
		{
			if (ai->m_unitDB->GetAttack(unit_type[i]) > 0 &&
				ai->m_unitDB->GetDefense(unit_type[i]) > 0 &&
				!ai->m_unitDB->CanMoveWater(unit_type[i]))
				m_garrisonCount++;
		}
	}

    sint32 goal_idx;
    for (goal_idx=0; goal_idx < GOAL_TYPE_MAX; goal_idx++) {
        if (m_bestUnitForGoal[goal_idx]) {
            m_bestUnitForGoal[goal_idx]->UpdateContinent(ai);
        }
    }
}

void CityAgent::GetFoodGrowth(double &nf, double &at, double &ag)
{
    nf = m_net_food;
    at = m_accumulated_growth;
    ag = m_accumulated_threshold;
}

sint32 CityAgent::GetQueueLen() const
{





	if (m_queueItemType >= 0 || m_buildCapitalization)
		return 1;
	else
		return 0;
}

void CityAgent::SetPos(AiMain *ai)
{
    BOOL is_unknown_id;

    ai->m_player->GetCityPos(m_id.GetVal(), &is_unknown_id, m_pos);

    Assert(is_unknown_id == FALSE);
}

void CityAgent::GetPos(MapPointData &pos) const
{
    pos = *m_pos;
}

void CityAgent::GetXYPos(AiMain *ai, MapPointData &pos)
{
    BOOL is_unknown_id;

    ai->m_player->GetCityXYPos(ai->my_player_index, m_id.GetVal(), &is_unknown_id, &pos);

    Assert(is_unknown_id == FALSE);
}

void CityAgent::EnqueueArmy(AiMain *ai, const sint32 unit_type, const double utility)
{





















	Assert(m_queueItemType < 0);

    BOOL is_unknown_id;
    sint32 r = ai->m_player->CityEnqueueBuildItem(m_id.GetVal(),
												  &is_unknown_id,
												  k_GAME_OBJ_TYPE_UNIT,
												  unit_type);
    Assert(r);
    Assert(is_unknown_id == FALSE);
	if (!r)
		return;

	m_queueItemType = unit_type;
	m_queueItemCat = k_GAME_OBJ_TYPE_UNIT;
}
























































































































































BOOL CityAgent::IsConstructingFreight( AiMain *ai, sint32 &freight_cost)
{
	sint32 idx_freight;

    freight_cost = 0;

	if (ai->m_science_agent->GetBestFreight(idx_freight) &&
		m_queueItemType == idx_freight)
		{
			freight_cost =
				sint32 (ai->m_unitDB->
						UnitTypeMovementPoints(idx_freight));
			return TRUE;
		}

	return FALSE;





















}

sint32 CityAgent::GetNumUnitsInCity(AiMain *ai) const
{
    MapPointData pos;

    GetPos(pos);

    sint32 player_id;
    uint32 a_id;
    sint32 top_unit_type;
    sint32 unit_num;

    if (ai->m_world->GetArmyId(&pos, &player_id, &a_id,
        &top_unit_type, &unit_num)){
        return unit_num;
    } else {
        return 0;
    }
}

double CityAgent::ActionBidBuild(AiMain *ai,  GoalFreight* goal_obj)
{
   double u = goal_obj->GetUtility();
   double t = ai->m_round_count->EstimateRoundsToEnd();
   Assert(0 < t);
   double q = u/t;





















   sint32 idx_freight = 0;
   if (!ai->m_science_agent->GetBestFreight(idx_freight))
	   {
		   return BAD_UTILITY;
	   }
   else if (m_projected_net_production < 1)
	   {
		   return BAD_UTILITY;
	   }
   else
	   {
		   sint32 c = ai->m_unitDB->GetProductionCost(idx_freight);
		   double u =
			   100000 *
			   fz_freight_coef *
			   q *
			   (t - (c/m_projected_net_production));

		   if (u < 0.01)
			   return BAD_UTILITY;
		   else
			   return u;
	   }
   return BAD_UTILITY;
}

























void CityAgent::RegisterBuiltArmy(AiMain *ai,  BSetID &id)
{

	Assert(m_queueItemType != -1);
	Assert(m_queueItemCat == k_GAME_OBJ_TYPE_UNIT);

	m_queueItemType = -1;


















	if (the_squad != NULL)
	{
		char text[100];
		sprintf(text, "null S:%d", the_squad->squad_ID);
		DumpStr(ai, text);
	}
	else
	    DumpStr(ai, "null");

    Assert(ai);
    Assert(ai->m_planner);
	#ifdef DBGAILOG
    if (ai->AI_VERBOSITY >= LOG_DETAILS)
	{
		AILOG(( wstr, "RegisterBuiltArmy\n"));
	}
	#endif DBGAILOG




	if (the_squad)
	{
		Squad *old_squad = the_squad;

		the_squad->Remove_Agent( this );

		old_squad->Recompute_Squad_Class( ai );
	}




}

void CityAgent::RegisterBuiltFreight(AiMain *ai)
{

	Assert(m_queueItemType != -1);
	Assert(m_queueItemCat == k_GAME_OBJ_TYPE_UNIT);

	m_queueItemType = -1;

	if (the_squad != NULL)
	{
		char text[100];
		sprintf(text, "null S:%d", the_squad->squad_ID);
		DumpStr(ai, text);
	}
    else {
	    DumpStr(ai, "null");
    }

    if (the_squad) {
		if (the_squad->my_strategic_goal != NULL)
            the_squad->my_strategic_goal->Set_Totally_Complete(TRUE);
    }












}

void CityAgent::RegisterCreateBuilding(AiMain *ai, sint32 building_type)
{







	m_queueItemType = -1;

    Assert(0 <= building_type);
    Assert(building_type < ai->m_nBuildings);
    m_building[building_type].RegisterCreateBuilding();

    if (FALSE == m_has_space_launcher) {
        if (ai->m_buildingDB->IsSpaceLauncher(building_type)) {
           SetHasSpaceLauncher(TRUE);
        }
    }

    if (FALSE == m_has_granary) {
         if (ai->m_buildingDB->IsGranary(building_type)) {
            m_has_granary = TRUE;
        }
    }

    if (FALSE == m_has_airport) {
        if (ai->m_buildingDB->GetIsAirport(building_type)) {
            m_has_airport = TRUE;
        }
    }









    if (0 < GetQueueLen()) {
		if (the_squad != NULL)
		{
			char text[100];
			sprintf(text, "null S:%d", the_squad->squad_ID);
			DumpStr(ai, text);
		}
        else {
	        DumpStr(ai, "null");
        }

        if (the_squad) {
			if (the_squad->my_strategic_goal != NULL)
				the_squad->my_strategic_goal->Set_Totally_Complete(TRUE);
        }
    }

    ComputePotentialUnits(ai);
}

void CityAgent::RegisterNewGovernment(AiMain *ai, sint32 government_type)
{

    ComputePotentialUnits(ai);
}

void CityAgent::RegisterLostBuilding(AiMain *ai,  sint32 building_idx)
{
    Assert(0 <= building_idx);
    Assert(building_idx < ai->m_nBuildings);

    if (m_has_space_launcher) {
        if (ai->m_buildingDB->IsSpaceLauncher(building_idx)) {
           SetHasSpaceLauncher(FALSE);
        }
    }

    if (m_has_airport) {
        if (ai->m_buildingDB->GetIsAirport(building_idx)) {
            m_has_airport = FALSE;
        }
    }

    if (m_has_granary) {
         if (ai->m_buildingDB->IsGranary(building_idx)) {
            m_has_granary = FALSE;
        }
    }

    m_building[building_idx].RegisterLostBuilding();

	ComputePotentialUnits(ai);
}

void CityAgent::RegisterObsoleteBuild(AiMain *ai,
									  sint32 cat,
									  sint32 type)
{
	Assert(type == m_queueItemType);
	Assert(cat == m_queueItemCat);

	m_queueItemType = -1;



































}

void CityAgent::RegisterPiracy(const sint32 other_owner, const uint32 other_city_id, const sint32 when_pirated)
{
    Pirate_Marker *marker = FindPiracyMarker(other_owner, other_city_id);

    if (marker) {
        marker->RegisterPiracy(when_pirated);
    } else {
        marker = new Pirate_Marker(other_owner, other_city_id, when_pirated);
        marker->m_next = m_pirate_head;
        m_pirate_head = marker;
    }
}

Pirate_Marker *CityAgent::FindPiracyMarker(const sint32 other_owner, const uint32 other_city_id)
{
    Pirate_Marker *ptr;

    ptr = m_pirate_head;
    while (ptr) {
        if ((other_owner == ptr->m_other_owner) && (other_city_id == ptr->m_other_city.GetVal())) {
            return ptr;
        }
        ptr = ptr->m_next;
    }

    return NULL;
}

BOOL CityAgent::PiracyRecently(AiMain *ai)
{
    Pirate_Marker *ptr;

    sint32 current_round = ai->m_round_count->GetRound();
    ptr = m_pirate_head;
    while (ptr) {
        if (!ptr->IsSafe(current_round)) {
            return TRUE;
        }
		ptr = ptr->m_next;
    }

    return FALSE;
}


void CityAgent::RegisterSetBuildFront(sint32 cat,
									  sint32 type)
{

	m_queueItemType = type;
	m_queueItemCat = cat;
}














































void CityAgent::CalcEstimatedCityProduction()
{
   if (m_is_materials_tax_exempt) {
       m_projected_net_production = m_projected_gross_production;
   } else {
       m_projected_net_production = sint32(m_projected_gross_production * (1.0 - fz_production_tax));
   }
}

sint32 CityAgent::EstimateRoundsToFinished(AiMain *ai, BSetID m_id)
{







	return EstimateRoundsToEmptyQueue(ai);
}

BOOL CityAgent::HasLotOfStoredProduction()
{
    if (m_stored_production < 500.0) {
        return FALSE;
    } else if (m_projected_net_production < 30.0) {
        return FALSE;
    } else {
        return 5.0 <= (m_stored_production /m_projected_net_production) ;
    }
}

double CityAgent::EstimatedPercentFinished(AiMain *ai)
{
    double cost = 0;
	double percent;

    if (m_projected_net_production < 1)
        return 100000;

	Assert(m_queueItemType != -1);
	if (m_queueItemType < 0)
		return 1.0;

    cost = ai->m_unitDB->GetProductionCost(m_queueItemType);

	if(cost == 0) {cost = 0.000001; }

	percent = m_stored_production / cost;

	if(percent < 0 || percent > 1)
	{
		percent = 1.0;
	}
	return percent;


}











BOOL CityAgent::FrontIsImprovement()
{
	return (m_queueItemType != -1 &&
			m_queueItemCat == k_GAME_OBJ_TYPE_IMPROVEMENT);

}

sint32 CityAgent::EstimateRoundsToEmptyQueue(AiMain *ai)
{
    sint32 rounds = 0;

    if (m_projected_net_production < 1)
        return 100000;

	if (m_queueItemType < 0)
		return 0;

    double cost = ai->m_unitDB->GetProductionCost(m_queueItemType);

	if(cost == 0) {cost = 0.000001; }

	rounds = (sint32)
		ceil((cost - m_stored_production) / m_projected_net_production);

	return rounds;




}

sint32 CityAgent::EstimateRoundsToBuildUnit(AiMain *ai,  sint32 unit_type)
{

    if (m_projected_net_production < 1) {
       return int (0.5 + ai->m_unitDB->GetProductionCost(unit_type));
    } else {
       return int (0.5 + ai->m_unitDB->GetProductionCost(unit_type) / m_projected_net_production);
    }
}

sint32 CityAgent::EstimateRoundsToBuildBuilding(AiMain *ai,  sint32 building_type)
{

    if (m_projected_net_production < 1) {
       return int (0.5 + ai->m_buildingDB->GetProductionCost(building_type));
    } else {
       return int ((0.5 + ai->m_buildingDB->GetProductionCost(building_type)- m_stored_production) / m_projected_net_production);
    }
}

sint32 CityAgent::EstimateRoundsToBuildWonder(AiMain *ai,  sint32 wonder_type)
{

    if (m_projected_net_production < 1) {
       return int (0.5 + ai->m_wonder_table->GetProductionCost(wonder_type));
    } else {
       return int ((0.5 + ai->m_wonder_table->GetProductionCost(wonder_type) - m_stored_production) / m_projected_net_production);
    }
}

sint32 CityAgent::EstimateRoundsToBuildEndGameObject(AiMain *ai,
													 sint32 object_type)
{
    if (m_projected_net_production < 1) {
       return int (0.5 + ai->m_endGameDB->GetCost(object_type));
    } else {
       return int ((0.5 + ai->m_endGameDB->GetCost(object_type) - m_stored_production) /
				   m_projected_net_production);
    }
}

double CityAgent::EstimateCostToGiveUpCurrentBuild(AiMain *ai, GOAL_TYPE goal)

{
    if (GetQueueLen() < 1) {
        return -1.0;
    } else {

        return COST_TOO_BIG;
    }





































}





sint32 CityAgent::What_Army_Am_I_Building(AiMain *ai)
{

	if (m_queueItemType < 0)
		return -1;

	if (m_queueItemCat == k_GAME_OBJ_TYPE_UNIT)
		return m_queueItemType;

	return -1;


















}

void CityAgent::UpdateThreat(AiMain *ai)

{
    sint32 i;
    ForeignAgent *him;
    BSetID h_id;
    BSet<ForeignAgent> *vu;

    ClearDeltaThreat();
    for (i=0; i<k_MAX_PLAYERS; i++) {
        if (ai->m_foreigner[i]) {
            if (ai->m_foreigner[i]->IsAtHotWar() ||
                ai->m_foreigner[i]->IsAtColdWar()) {
                vu = ai->m_foreigner[i]->GetVisibleUnits();
                for (him = vu->First(h_id); vu->Last(); him =  vu->Next(h_id)) {
                    AddToDeltaThreat(ai, him);
                }
            }
        }
    }
    AddToThreat();
}

void CityAgent::ClearDeltaThreat()

{
    m_delta_threat_strength = 0.0;
}

void CityAgent::AddToDeltaThreat(AiMain *ai,  ForeignAgent *him)
{

   double threat_distance_scale = 5.0;
   MapPointData pos;

   him->GetPos(pos);

   double d = ai->m_map->WrappedDistance(pos, *m_pos);

   double turns = int ( d/ (him->GetMaxMovePoints() * 0.01));

   double s = threat_distance_scale / (threat_distance_scale + turns);

   m_delta_threat_strength += him->GetAttackStrength() * s;
}

void CityAgent::AddToThreat()

{
    if (m_delta_threat_strength < m_threat_strength) {
        m_threat_strength  = fz_city_threat_decay * m_delta_threat_strength
            + (1.0 - fz_city_threat_decay) * m_threat_strength;
    } else {
        m_threat_strength = m_delta_threat_strength;
    }
}

void CityAgent::GetThreatGradient(MapPointData &pos, double &value) const

{
    pos = *m_pos;

    value = m_threat_strength - m_defenders_defense_strength;
}

double CityAgent::GetDefenseRequired(AiMain *ai)
{
	MapPointData XYpos;
	ai->m_player->Norm2XY(*m_pos, XYpos);
	return ai->m_planner->the_strategic_map->Get_Threat(XYpos);
}


















BOOL CityAgent::IsGoalSatisfied(GOAL_TYPE goal)
{
    switch (goal) {
    case GOAL_TYPE_DEFENSE:
        return m_threat_strength < (m_defenders_defense_strength + temp_goal_relief_def_str);
    default:
        INSANE(IS_GOAL_SATISFIED_UNKNOWN_GOAL);
        return FALSE;
    }
}

void CityAgent::ResetDefenseStrength(const sint32 nu,
									 const double ranged,
									 const double as, const double ds)
{
    m_defenders_nUnits = nu;
    m_defenders_ranged = ranged;
    m_defenders_attack_strength = as;
    m_defenders_defense_strength = ds;
}

void CityAgent::InsertBuildingGoals(AiMain *ai)

{
    GoalBuilding *goal_obj;
    sint32 i;
    double v;






	i = 1;






			if (m_building[i].CanConstructBuilding(ai)) {
				v = m_building[i].CalcPayoff(ai,this);
				goal_obj = new GoalBuilding(v, i, m_id.GetVal());
				Assert(m_building[i].GetState() == BUILDING_STATE_UNBUILT);

				ai->m_planner->Add_Goal(goal_obj);


			}




}








































































































void CityAgent::EnqueueBuildBuilding(AiMain * ai, const sint32 blg_type,
               const double utility)
{

    Assert(m_building[blg_type].GetState() == BUILDING_STATE_UNBUILT);







	Assert(m_queueItemType < 0);

    BOOL is_unknown_id;
    sint32 r;
    r = ai->m_player->CityEnqueueBuildItem(m_id.GetVal(), &is_unknown_id,
           k_GAME_OBJ_TYPE_IMPROVEMENT, blg_type);
    Assert(r);

	if (!r)
		return;
	m_queueItemType = blg_type;
	m_queueItemCat = k_GAME_OBJ_TYPE_IMPROVEMENT;

    Assert(is_unknown_id == FALSE);
}

Goal_Result CityAgent::EnqueueBuildWonder(AiMain * ai, const sint32 idx_wonder,
               const double utility)
{
    if (!ai->m_wonder_table->CanBuild(ai, idx_wonder))
        return GOAL_FAILED;

    ai->m_wonder_table->SetIamBuilding(idx_wonder);









	Assert(m_queueItemType < 0);

    BOOL is_unknown_id;
    sint32 r;
    r = ai->m_player->CityEnqueueBuildItem(m_id.GetVal(), &is_unknown_id,
        k_GAME_OBJ_TYPE_WONDER, idx_wonder);

    Assert(r);
	Assert(is_unknown_id == FALSE);
	if (!r)
		return GOAL_FAILED;

	m_queueItemType = idx_wonder;
	m_queueItemCat = k_GAME_OBJ_TYPE_WONDER;

    return GOAL_IN_PROGRESS;

}

Goal_Result CityAgent::EnqueueBuildEndGameObject(AiMain * ai, const sint32 idx_object,
	const double utility)
{

	Assert(m_queueItemType < 0);

    BOOL is_unknown_id;
    sint32 r;
    r = ai->m_player->CityEnqueueBuildItem(m_id.GetVal(), &is_unknown_id,
           k_GAME_OBJ_TYPE_ENDGAME_OBJECT, idx_object);
    Assert(r);
    Assert(is_unknown_id == FALSE);
	if (!r)
		return GOAL_FAILED;

	m_queueItemType = idx_object;
	m_queueItemCat = k_GAME_OBJ_TYPE_ENDGAME_OBJECT;

	return GOAL_IN_PROGRESS;
}

Goal_Result CityAgent::BuildCapitalization(AiMain * ai)
{

	Assert(m_queueItemType < 0);
    BOOL is_unknown_id;
    sint32 r;
    r = ai->m_player->BuildCapitalization(m_id.GetVal(), &is_unknown_id);
    Assert(r);
    Assert(is_unknown_id == FALSE);
	m_buildCapitalization = TRUE;
	return GOAL_IN_PROGRESS;
}

void CityAgent::EnqueueBuildFreight(AiMain * ai,
               const double utility)
{

	Assert(m_queueItemType < 0);

    sint32 idx_freight;
    if (ai->m_science_agent->GetBestFreight(idx_freight)) {








       BOOL is_unknown_id;
       sint32 r;
       r = ai->m_player->CityEnqueueBuildItem(m_id.GetVal(), &is_unknown_id,
            k_GAME_OBJ_TYPE_UNIT, idx_freight);

       Assert(r);
       Assert(is_unknown_id == FALSE);
		if (!r)
			return;

	   m_queueItemType = idx_freight;
	   m_queueItemCat = k_GAME_OBJ_TYPE_UNIT;

    } else {
        INSANE(DONT_HAVE_TECH_FOR_CARAVAN);
        return;
    }
}














double CityAgent::Find_Best_Utility_Agent_We_Can_Build
(
    AiMain *ai,
	ArmyGoal *goal_obj,
    ArmyAgent *&best_agent
)
{




	double best_utility;


    best_agent = NULL;
	best_utility = BAD_UTILITY;

	if (GetQueueLen() > 0)
		return BAD_UTILITY;

    if (GetProjectedNetProduction() < 1) {
       return BAD_UTILITY;
    }

	Assert(goal_obj->GetType() < GOAL_TYPE_MAX);
	Assert(goal_obj->GetType() >= 0);

	best_agent = m_bestUnitForGoal[goal_obj->GetType()];

	if (best_agent != NULL)
		best_utility = best_agent->EstimateUtility(ai,goal_obj);
	else
		best_utility = BAD_UTILITY;

	return best_utility;





























}















double CityAgent::FindBestBuildUtility
(
    AiMain *ai,
	ArmyGoal *goal_obj,
    ArmyAgent *&best_agent
)
{




	double best_utility;


    best_agent = NULL;
	best_utility = BAD_UTILITY;

	if (GetQueueLen() > 0)
		return BAD_UTILITY;

    if (GetProjectedNetProduction() < 1) {
       return BAD_UTILITY;
    }

	Assert(goal_obj->GetType() < GOAL_TYPE_MAX);
	Assert(goal_obj->GetType() >= 0);
	best_agent = m_bestUnitForGoal[goal_obj->GetType()];

	if (best_agent != NULL)
		best_utility = best_agent->EstimateUtility(ai,goal_obj);
	else
		best_utility = BAD_UTILITY;

	return best_utility;



























}





double CityAgent::EstimateUtility(AiMain *ai,  ArmyGoal *goal_obj)
{
    Assert(goal_obj);
	ArmyAgent* aa;


    ArmyAgent *best_agent=NULL;
    switch (goal_obj->GetType()) {
    case GOAL_TYPE_CONSTRUCT_BUILDING:
        return ActionBidBuild(ai, (GoalBuilding *) goal_obj);
    case GOAL_TYPE_CONSTRUCT_FREIGHT:
        return ActionBidBuild(ai, (GoalFreight *) goal_obj);
    case GOAL_TYPE_CONSTRUCT_WONDER:


	case GOAL_TYPE_HARASS_CITY:
	case GOAL_TYPE_BOMBARD:
	case GOAL_TYPE_CONVERT_CITY:
    case GOAL_TYPE_BIOTERROR_CITY:
    case GOAL_TYPE_NANOATTACK_CITY:
	case GOAL_TYPE_RUSTLE:
	case GOAL_TYPE_EXPEL:
	case GOAL_TYPE_HURT_ARMY:

	case GOAL_TYPE_PILLAGE:
	case GOAL_TYPE_PIRATE:
	case GOAL_TYPE_COUNTER_STEALTH:
	case GOAL_TYPE_BONUS_FOOD:
	case GOAL_TYPE_GOODY_HUT:
	case GOAL_TYPE_PLANT_NUKE:
	case GOAL_TYPE_MAKE_PARK:
	case GOAL_TYPE_CAUSE_UNHAPPINESS:
	case GOAL_TYPE_ESTABLISH_EMBASSY:
	case GOAL_TYPE_FRANCHISING:
	case GOAL_TYPE_STEAL_TECHNOLOGY:
	case GOAL_TYPE_INCITE_REVOLUTION:
	case GOAL_TYPE_INJOIN:
	case GOAL_TYPE_ASSASINATE_RULER:
	case GOAL_TYPE_UNDERGROUND_RAILWAY:
	case GOAL_TYPE_RETREAT:
	case GOAL_TYPE_NUKE_CITY:
	case GOAL_TYPE_WANDER:

		return FindBestBuildUtility(ai,goal_obj,aa);
    default:

		return Find_Best_Utility_Agent_We_Can_Build(ai, goal_obj,aa);
	}
}








void CityAgent::Set_Defend_Me_Status(bool new_defense_status)
{

	m_defend_me_goal_set = new_defense_status;
	if (new_defense_status == FALSE)
		m_defend_me_goal_id = UNASSIGNED_ID;
}














bool CityAgent::Get_Defend_Me_Status()
{
	return m_defend_me_goal_set;
}













void CityAgent::Setup_Defense_Goal(AiMain *ai)
{

	if (Get_Defend_Me_Status() == false)
	{

		GoalDefense *tmp_defend_goal = new GoalDefense(ai, this, *m_pos);

		Set_Defend_Me_Status(true);

		Assert(tmp_defend_goal);
		ai->m_planner->Add_Goal(tmp_defend_goal);

        m_defend_me_goal_id = tmp_defend_goal->GetID();

	}
}













void CityAgent::Setup_Retreat_Goal(AiMain *ai)
{

	if (m_retreatGoalID == UNASSIGNED_ID)
	{

		GoalRetreat *tmp_retreat_goal = new GoalRetreat(ai, this, *m_pos);

		Assert(tmp_retreat_goal);
		ai->m_planner->Add_Goal(tmp_retreat_goal);

        m_retreatGoalID = tmp_retreat_goal->GetID();

	}
}













void CityAgent::Remove_Retreat_Goal(AiMain *ai)
{

    Assert(ai);
    Assert(ai->m_planner);

	if (m_retreatGoalID != UNASSIGNED_ID)
	{

		Goal *tmp_retreat_goal = ai->m_planner->Return_Goal_By_ID(m_retreatGoalID);

#ifdef DBGAILOG
    if (ai->AI_VERBOSITY >= LOG_MEDIUM)
	{
		AILOG(( wstr, "Remove_Retreat_Goal #%x from city %x \n", tmp_retreat_goal, this));
	}
#endif DBGAILOG

		if (tmp_retreat_goal)
		{

			if (tmp_retreat_goal->attacking_squad != NULL)
			{
				tmp_retreat_goal->Set_Totally_Complete(TRUE);

				tmp_retreat_goal->attacking_squad->Detach_Goal();

			}

			ai->m_planner->the_scheduler->
				Remove_Goal_And_Add_To_Delete_List(tmp_retreat_goal);
		}

		m_retreatGoalID = UNASSIGNED_ID;

	}
}













void CityAgent::Remove_Defense_Goal(AiMain *ai)
{

    Assert(ai);
    Assert(ai->m_planner);

	if (Get_Defend_Me_Status() == true)
	{

		Goal *tmp_defend_goal = ai->m_planner->Return_Goal_By_ID(m_defend_me_goal_id);

#ifdef DBGAILOG
    if (ai->AI_VERBOSITY >= LOG_MEDIUM)
	{
		AILOG(( wstr, "Remove_Defense_Goal #%x from city %x \n", tmp_defend_goal, this));
	}
#endif DBGAILOG

		if (tmp_defend_goal)
		{

			if (tmp_defend_goal->attacking_squad != NULL)
			{
				tmp_defend_goal->Set_Totally_Complete(TRUE);

				tmp_defend_goal->attacking_squad->Detach_Goal();

			}

			ai->m_planner->the_scheduler->
				Remove_Goal_And_Add_To_Delete_List(tmp_defend_goal);
		}

		Set_Defend_Me_Status(false);

	}
}







double WeightTimeToBuild(AiMain *ai, const double val, const double turns_to_complete)
{
    double prod = 1.0, sum = 0.0;
    double i;
    double n = ai->m_round_count->EstimateRoundsToEnd();

    for (i=1.0; (i<turns_to_complete) && (i<n); i += 1.0) {
        prod *= fz_build_time_decay;
    }

    for (; (i<n) ; i += 1.0) {
        prod *= fz_build_time_decay;
        sum += val * prod;
    }

    return sum;
}












































































sint32 CityAgent::PretestContructBuilding(AiMain *ai, sint32 building_type)
{
   if (0 < GetQueueLen()) return FALSE;

    if ((building_type != 1) &&
        (ai->m_buildingDB->GetProductionCost(building_type) < m_stored_production)) return FALSE;
	return TRUE;
}

BOOL CityAgent::PretestBuildingBuildTime(AiMain *ai, sint32 building_type)
{
	sint32 rounds_to_build = EstimateRoundsToBuildBuilding(ai,  building_type);

	return (rounds_to_build <
			ai->m_planner->the_AIP.max_build_building_rounds);
}

sint32 CityAgent::PretestContructUnit(AiMain *ai, sint32 unit_type)

{
    if (0 < GetQueueLen()) return FALSE;

    if (ai->m_unitDB->GetProductionCost(unit_type) < m_stored_production)
		return FALSE;


	uint32 movement_type = ai->m_unitDB->GetMovementType(unit_type);
	BOOL space_only = !(movement_type & ~k_BIT_MOVEMENT_TYPE_SPACE);
	BOOL can_launch = ai->m_unitDB->CanSpaceLaunch(unit_type);
	if (space_only &&
		!can_launch &&
		m_pos->z != SPACE_Z && !m_has_space_launcher )
		return FALSE;

	sint32 rounds_to_build = EstimateRoundsToBuildUnit(ai,  unit_type);
	return (rounds_to_build < ai->m_planner->the_AIP.max_build_unit_rounds &&
		CanBuildUnit(ai,unit_type));

}

sint32 CityAgent::PretestContructWonder(AiMain *ai, sint32 wonder_type)

{
	sint32 rounds_to_build = EstimateRoundsToBuildWonder(ai,  wonder_type);
	Assert(ai->m_wonders_available);
    return (GetQueueLen() < 1 &&
		rounds_to_build <= ai->m_planner->the_AIP.max_build_wonder_rounds);
}

BOOL CityAgent::PretestConstructEndGameObject(AiMain *ai, sint32 object_type)
{
    if (0 < GetQueueLen()) return FALSE;


	sint32 rounds_to_build = EstimateRoundsToBuildEndGameObject(ai,
																object_type);

    return (rounds_to_build <= ai->m_planner->the_AIP.max_build_wonder_rounds &&
		ai->m_player->CityCanBuildEndGameObject(m_id.GetVal(), object_type));
}

BOOL CityAgent::PretestConstructCapitalization(AiMain *ai)
{
    if (0 < m_stored_production)
		return FALSE;

    return (GetQueueLen() < 1 &&
		ai->m_player->CityCanBuildCapitalization(m_id.GetVal()));
}

void CityAgent::FindInstallationBenefits(AiMain *ai,  const sint32 idx_city,
   const sint32 median)

{
    sint16 dx, dy;
    sint32 stored;

    double pos_bonus;
    double best_utility_max;
    double best_cost;
    sint32 best_inst_max;
    sint32 best_extra_data;
    MapPointData best_w;

    long city_size_pop_effect;
    BOOL is_unknown_id;
	ai->m_player->GetCitySizePopEffect(m_id.GetVal(), &is_unknown_id, &city_size_pop_effect);

	BOOL can_make_farm = TRUE;




    stored = ai->m_player->GetMaterialsStored();
    best_utility_max=1.0;

    FindInstallationBenefitsCell(ai, stored,
        m_pos->x-1, m_pos->y-2, m_pos->z, idx_city, median,
        0.0, best_utility_max, best_cost, best_inst_max,
        best_extra_data, best_w, can_make_farm);
    FindInstallationBenefitsCell(ai, stored,
        m_pos->x, m_pos->y-2, m_pos->z, idx_city, median,
        0.0, best_utility_max, best_cost, best_inst_max,
        best_extra_data, best_w, can_make_farm );
    FindInstallationBenefitsCell(ai, stored,
        m_pos->x+1, m_pos->y-2, m_pos->z, idx_city, median,
        0.0, best_utility_max, best_cost, best_inst_max,
        best_extra_data, best_w, can_make_farm);

    for (dx = -2; dx <= 2; dx++) {
        for (dy = -1; dy <= 1; dy++) {
            if ((dx != 0) || (dy != 0)) {

                if ((0 <= dx) && (0 <= dy)) {
                    pos_bonus = 1.0;
                } else {
                    pos_bonus = 0.0;
                }

                FindInstallationBenefitsCell(ai, stored,
                    m_pos->x+dx, m_pos->y+dy, m_pos->z,
                    idx_city, median,
                    pos_bonus, best_utility_max, best_cost, best_inst_max,
                    best_extra_data, best_w, can_make_farm);
            }
        }
    }
    FindInstallationBenefitsCell(ai, stored,
        m_pos->x-1, m_pos->y+2, m_pos->z, idx_city, median,
        0.0, best_utility_max, best_cost, best_inst_max,
        best_extra_data, best_w, can_make_farm);

    FindInstallationBenefitsCell(ai, stored, m_pos->x, m_pos->y+2,
        m_pos->z, idx_city, median,
        0.0, best_utility_max, best_cost, best_inst_max,
        best_extra_data, best_w, can_make_farm);

    FindInstallationBenefitsCell(ai, stored,
        m_pos->x+1, m_pos->y+2, m_pos->z,
        idx_city, median,
        0.0, best_utility_max, best_cost, best_inst_max,
        best_extra_data, best_w, can_make_farm);


    GoalInstallation *goal_obj;
    if (1.0 < best_utility_max) {
       goal_obj = new GoalInstallation(best_inst_max, best_extra_data,
          best_w, best_utility_max, this, sint32(best_cost));
       ai->m_inst_bid_list->InsertGoal(goal_obj);
    }

}

enum INST_TILE_STATE {
    INST_TILE_STATE_OK,
    INST_TILE_STATE_OVERWRITE,
    INST_TILE_STATE_UPGRADE,
    INST_TILE_STATE_BLOCKED,
    INST_TILE_STATE_ROAD
};









void CityAgent::FindInstallationBenefitsCell(
    AiMain *ai,
    sint32 stored,
    sint16 x,
    sint16 y,
    sint16 z,
    const sint32 idx_city,
    const sint32 median,
    const double pos_bonus,
    double &best_utility_max,
    double &best_cost,
    sint32 &best_inst_max,
    sint32 &best_extra_data,
    MapPointData &best_w,
    BOOL can_make_farm

) {
    MapPointData w;
    double u;
    ERR_BUILD_INST err;

    ai->m_map->Wrap(x,y,z, w);

    MapPoint XYpos;
    ai->m_player->Norm2XY(w, XYpos);
    double threat = ai->m_planner->the_strategic_map->Get_Threat(XYpos);
    if (fz_inst_threat_threshold < threat) {
        return;
    }

    INST_TILE_STATE tile_state;
    sint32 old_inst;
    double old_utility = 0.0;

    if (ai->m_installationDB->GetOverwritableInstallation(&w, &old_inst)) {
        if (w.z == SPACE_Z) return;

        old_utility = EstimateInstallationBenefit(ai, old_inst, w,
            idx_city, median, tile_state, 0.0, can_make_farm);
    } else {
        old_utility = 0;
    }

    sint32 inst_max = -1;
    double utility_max = 1.0;

    sint32 inst_idx;
    sint32 extra_data = 0;

    sint32 tt;
    BOOL i_to_improve;
    sint32 type_good;
    ai->m_world->GetTerrainType(&w, &tt);

    switch (tt) {
    case TERRAIN_FOREST:
    case TERRAIN_DEAD:
    case TERRAIN_DEAD_HILL:
        i_to_improve = FALSE;
        break;
    case TERRAIN_PLAINS:
    case TERRAIN_TUNDRA:
    case TERRAIN_GLACIER:
    case TERRAIN_DESERT:
    case TERRAIN_SWAMP:
    case TERRAIN_JUNGLE:
        if (ai->m_world->GetGood (&w, &type_good)) {
            i_to_improve = TRUE;
        } else {
            i_to_improve = FALSE;
        }
        break;
    default:
        i_to_improve = TRUE;
        break;
    }

    if (i_to_improve) {

		for (inst_idx=0; inst_idx<0 ; inst_idx++) {

           if (ai->m_installationDB->IsEnabled(inst_idx)) {

               if (ai->m_installationDB->CanBuildHere(inst_idx, 0, &w, &err)) {
                    Assert(ERR_BUILD_INST_BLOCKED != err);

                    switch (err) {
                    case ERR_BUILD_INST_OVERWRITE: tile_state = INST_TILE_STATE_OVERWRITE; break;
                    case ERR_BUILD_INST_OK: tile_state = INST_TILE_STATE_OK; break;
                    case ERR_BUILD_INST_UPGRADE: tile_state = INST_TILE_STATE_UPGRADE; break;
                    default:
                        INSANE(INSTALLATION_UNKNOWN_ERROR);
                        tile_state = INST_TILE_STATE_BLOCKED;
                        break;
                    }

                    u = EstimateInstallationBenefit(ai, inst_idx, w, idx_city,
                        median, tile_state, old_utility, can_make_farm);

                    if (utility_max <= u) {
                        inst_max = inst_idx;
                        utility_max = u;
                    }
               }
           }
        }
    } else {

        sint32 new_terrain;
        BOOL can_be_grass=FALSE, can_be_hill=FALSE;

        if (ai->m_installationDB->IsEnabled(0 )) {

           can_be_grass = can_make_farm && ai->m_installationDB->CanBuildHere(0 , TERRAIN_GRASSLAND, &w, &err);

           can_be_hill = ai->m_installationDB->CanBuildHere(0 , TERRAIN_HILL, &w, &err);

           if (can_be_grass || can_be_hill) {
               u = EstimateTerainTransform (ai, 0 ,  w, idx_city,
                        median, tile_state, new_terrain, can_be_grass, can_be_hill);

               if (utility_max <= u) {
                   inst_max = 0 ;
                   extra_data = new_terrain;
                   utility_max = u;
               }
           }
        }
    }

    if (best_utility_max < utility_max) {
       best_utility_max = utility_max + pos_bonus;
       best_inst_max = inst_max;
       best_extra_data = extra_data;
       best_w = w;
       best_utility_max = best_utility_max;
       best_cost = ai->m_installationDB->GetCost(inst_max, &w, extra_data);
    }
}

#if 0
enum BEST_INST {
    BEST_INST_NOTHING,
    BEST_INST_FARM,
    BEST_INST_QUARRY,
    BEST_INST_TERRAFORM,
    BEST_INST_CITY_BALANCE
};

int g_best_inst [] = {
BEST_INST_TERRAFORM,
BEST_INST_FARM,
BEST_INST_TERRAFORM,
BEST_INST_TERRAFORM,
BEST_INST_FARM,
BEST_INST_TERRAFORM,
BEST_INST_TERRAFORM,
BEST_INST_TERRAFORM,
BEST_INST_QUARRY,
BEST_INST_QUARRY,
BEST_INST_SEA_FARM,
BEST_INST_BALANCE,
BEST_INST_SEA_QUARRY,
BEST_INST_BALANCE,
BEST_INST_SEA_FARM,
BEST_INST_SEA_FARM,
BEST_INST_SEA_QUARRY,
BEST_INST_SEA_QUARRY,
BEST_INST_TERRAFORM,
BEST_INST_QUARRY,
BEST_INST_QUARRY,
BEST_INST_QUARRY,
BEST_INST_QUARRY,
BEST_INST_TERRAFORM
}

void CityAgent::LookupBestInstallation(
    AiMain *ai,
    sint32 stored,
    sint32 x,
    sint32 y,
    sint32 z,
    const sint32 idx_city,
    const sint32 median
)
{
}
#endif

RoadDest::RoadDest(CityAgent *da, const sint32 rt)
{
    m_dest = da;
    m_id = m_dest->GetID().GetVal();
    m_road_type = rt;
    m_next=NULL;
}

RoadDest::RoadDest(CivArchive &archive)
{
    m_next = NULL;
    m_dest = NULL;
    Serialize(archive);
}

void RoadDest::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) {
        archive << m_id;
        archive << m_road_type;
    } else {
        archive >> m_id;
        archive >> m_road_type;
    }
}

void RoadDest::HookUp(AiMain *ai)
{
    m_dest = ai->m_city_set->Find(m_id);
}

BOOL CityAgent::IsConnectedByRoad(const CityAgent *da) const
{
    RoadDest *r;

    for (r = m_road_list; r; r = r->m_next) {
        if (r->m_dest == da) {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL CityAgent::IsConnectedByRoadAtLeast(const CityAgent *da, const sint32 best_road) const
{
    RoadDest *r;

    for (r = m_road_list; r; r = r->m_next) {
        if (r->m_dest == da) {
            return r->m_road_type == best_road;
        }
    }

    return FALSE;
}
void CityAgent::InsertRoad(CityAgent *da, const sint32 rt)
{
   RoadDest *r;


    for (r = m_road_list; r; r = r->m_next) {
        if (r->m_dest == da) {
            r->m_road_type = rt;
            return;
        }
    }


    r = new RoadDest(da, rt);
    r->m_next = m_road_list;
    m_road_list = r;
}








double CityAgent::EstimateInstallationFarm(
    AiMain *ai,
    const sint32 inst_idx,
    MapPointData &w,
    const sint32 idx_city,
    const sint32 median,
    INST_TILE_STATE &tile_state)
{


    double f = ai->m_installationDB->GetFoodBonus(inst_idx, &w);
    if (0 < f) {
        if ( !ai->m_map->HasPopFromThisCity(ai, w, m_id.GetVal())) {
            return 0.0;
        }
    } else {
        return 0.0;
    }

    double r;
    if (INST_TILE_STATE_ROAD == tile_state) {
        r = 0.85;
    } else {
        r = 1.0;
    }

    sint32 cell_food;
    sint32 cell_prod;
    ai->m_world->GetFood(&w, &cell_food);
    ai->m_world->GetProd(&w, &cell_prod);

    double is_good = 0.0;
    sint32 type_good;
    if (cell_prod < cell_food) {
        if (ai->m_world->GetGood (&w, &type_good)) {
            is_good = 5.0;
        }
    }

    double want_space_food = 0.0;

   double space_bonus;
    if (w.z == SPACE_Z) {
        if (m_total_citizens < 3) {
            want_space_food  = 5000.0;
        } else if (( m_projected_food) < (1.5 * m_total_citizens * ai->m_alloc_WGF->GetCurrentRation())) {
            want_space_food  = 3000.0;
        } else {
            want_space_food = -5000.0;
        }
        space_bonus = 5000.0;
    } else {
        space_bonus = 0.0;
    }

	double size_bonus = k_SIZE_COEF * ((ai->m_city_set->GetNum() - idx_city) / ai->m_city_set->GetNum());

	if((median - idx_city) > 0)
	{
		return 5 * fz_inst_food * r * (
            f  +
          is_good +
           0.1 * cell_food +
           0.02 * cell_prod
          )
		  + size_bonus
          + space_bonus +
          want_space_food;
	}else
	{
		return fz_inst_food * r * (
			f  +
			is_good +
			0.1 * cell_food +
			0.02 * cell_prod
			)
		  + size_bonus
          + space_bonus +
          want_space_food;
	}
}









double CityAgent::EstimateInstallationQuarry(
    AiMain *ai,
    const sint32 inst_idx,
    MapPointData &w,
    const sint32 idx_city,
    const sint32 median,
    INST_TILE_STATE &tile_state)
{

    double p = ai->m_installationDB->GetProductionBonus(inst_idx, &w);
    if (5 < p) {
        if (!ai->m_map->HasPopFromThisCity(ai, w, m_id.GetVal())) {
            return 0.0;
        }
    }  else {
        return 0.0;
    }

    double r;
    if (INST_TILE_STATE_ROAD == tile_state) {
        r = 0.95;
    } else {
        r = 1.0;
    }

    sint32 cell_food;
    sint32 cell_prod;
    ai->m_world->GetFood(&w, &cell_food);
    ai->m_world->GetProd(&w, &cell_prod);

    double is_good = 0.0;
    sint32 type_good;
    if (w.z == GROUND_Z) {
        if (cell_food < cell_prod) {
            if (ai->m_world->GetGood (&w, &type_good)) {
                is_good = 5.0;
            }
        } else {
            return 0.0;
        }
    }

    double space_bonus;
    if (w.z == SPACE_Z) {
        if (m_total_citizens == 1) {
            space_bonus = 0.0;
        } else if (( m_projected_food) < (1.5 * m_total_citizens * ai->m_alloc_WGF->GetCurrentRation())) {
            space_bonus = 0.0;
        } else {
            space_bonus = 5000.0;
        }
    } else {
        space_bonus = 0.0;
    }

	double size_bonus = k_SIZE_COEF * ((ai->m_city_set->GetNum() - idx_city) / ai->m_city_set->GetNum());

    if(idx_city - median > 0)
	{
		return 5 * fz_inst_prod * r * (
			p  +
			is_good +
			0.02 * cell_food +
			0.1 * cell_prod
			) + size_bonus
            + space_bonus;
	}else
	{
		return fz_inst_prod * r * (
			p  +
			is_good +
			0.02 * cell_food +
			0.1 * cell_prod
			) + size_bonus
            + space_bonus;
	}

}

double CityAgent::EstimateTerainTransform(
    AiMain *ai,
    const sint32 inst_idx,
    MapPointData &w,
    const sint32 idx_city,
    const sint32 median,
    INST_TILE_STATE &tile_state,
    sint32 &new_terrain,
    BOOL can_be_grass,
    BOOL can_be_hill
    )
{

    sint32 tt;
    ai->m_world->GetTerrainType(&w, &tt);

    sint32 type_good;
    double is_dead;
    double is_forest = 1.0;
    switch (tt) {
    case TERRAIN_FOREST:
        is_forest = 0.2;
    case TERRAIN_PLAINS:
    case TERRAIN_TUNDRA:
    case TERRAIN_GLACIER:
    case TERRAIN_DESERT:
    case TERRAIN_SWAMP:
    case TERRAIN_JUNGLE:
        if (!ai->m_map->HasPopFromThisCity(ai, w, m_id.GetVal())) {
            return -10.0;
        }

        if (ai->m_world->GetGood (&w, &type_good)) {
            return -10.0;
        }
        is_dead = 1.0;
        break;
    case TERRAIN_DEAD:
    case TERRAIN_DEAD_HILL:
        is_dead = 25.0;
        break;
    default:
        return -10.0;
    }

    double is_river;
    if (ai->m_world->IsRiver(&w)) {
        is_river = 1.5;
    } else {
        is_river = 1.0;
    }

    double  is_road;
    sint32 road_type;
    if (ai->m_world->GetRoad (&w, &road_type)) {
        is_road = 0.9;
    } else {
        is_road = 1.0;
    }

    BOOL want_food;
    double is_irrigated_grass;
    double is_irrigated_hill;
    if (ai->m_world->CanBeIrrigated(&w)) {
        is_irrigated_grass = 1.0;
        is_irrigated_hill = 0.75;
    } else {
        is_irrigated_grass = 0.25;
        is_irrigated_hill = 1.0;
    }

    double food_target;
    if ((can_be_grass && !can_be_hill) || ((median - idx_city) > 0)) {
        food_target = 1.15;
    } else {
        food_target = 1.05;
    }

    if (m_projected_food < (food_target * m_total_citizens * ai->m_alloc_WGF->GetCurrentRation())) {
        want_food  = TRUE;
    } else {
        want_food = FALSE;
    }

	double size_bonus = k_SIZE_COEF * ((ai->m_city_set->GetNum() - idx_city) / ai->m_city_set->GetNum());

    if((median - idx_city) > 0) {
        if (can_be_grass && want_food) {
            new_terrain = TERRAIN_GRASSLAND;
            return 180.0 * is_dead * is_road * is_river * is_forest * is_irrigated_grass;
        } else if (can_be_hill) {
            new_terrain = TERRAIN_HILL;
            return 75.0 * is_dead * is_road * is_river * is_forest * is_irrigated_hill + idx_city * 2;
        }
    } else {
        if (can_be_grass && want_food) {

            new_terrain = TERRAIN_GRASSLAND;
            return 25.0 * is_dead * is_road * is_river * is_forest * is_irrigated_grass;
        } else if (can_be_hill) {
            new_terrain = TERRAIN_HILL;
            return 200.0 * is_dead * is_road * is_river * is_forest * is_irrigated_hill + idx_city * 2;
        }
	}

    return -10.0;
}








double CityAgent::EstimateInstallationGuardtower(
    AiMain *ai,
    const sint32 inst_idx,
    MapPointData &w,
    const sint32 idx_city,
    const sint32 median,
    INST_TILE_STATE &tile_state)
{
    return fz_inst_vision_range * ai->m_installationDB->GetVisionRange(inst_idx, &w);
}








double CityAgent::EstimateInstallationAirfield(
    AiMain *ai,
    const sint32 inst_idx,
    MapPointData &w,
    const sint32 idx_city,
    const sint32 median,
    INST_TILE_STATE &tile_state)
{
    return fz_inst_airfield * ai->m_installationDB->GetIsAirfield(inst_idx, &w);
}








double CityAgent::EstimateInstallationFort(
    AiMain *ai,
    const sint32 inst_idx,
    MapPointData &w,
    const sint32 idx_city,
    const sint32 median,
    INST_TILE_STATE &tile_state)
{
    return 0.0;
	return fz_inst_def * ai->m_installationDB->GetInstDefenseDelta(inst_idx) +
        fz_inst_fort * ai->m_installationDB->GetIsFort(inst_idx, &w);
}









double CityAgent::EstimateInstallationLandMine(
    AiMain *ai,
    const sint32 inst_idx,
    MapPointData &w,
    const sint32 idx_city,
    const sint32 median,
    INST_TILE_STATE &tile_state)
{
    double choke;
    if (ai->m_world->GetIsChokePoint(w)) {
        choke = 5.0;
    } else {
        choke = 1.0;
    }

    switch (tile_state) {
    case INST_TILE_STATE_ROAD:
        return choke *
            (fz_inst_attack * ai->m_installationDB->GetAttackValue(inst_idx, &w) * 0.001 +
            fz_inst_firepower * ai->m_installationDB->GetFirepowerValue(inst_idx, &w))
            * ai->m_rand->Next(60);
    default:
         return choke *
            (fz_inst_attack * ai->m_installationDB->GetAttackValue(inst_idx, &w) * 0.001 +
            fz_inst_firepower * ai->m_installationDB->GetFirepowerValue(inst_idx, &w))
            * ai->m_rand->Next(50);
    }
}








double CityAgent::EstimateInstallationBenefit(
    AiMain *ai,
    const sint32 inst_idx,
    MapPointData &w,
    const sint32 idx_city,
    const sint32 median,
    INST_TILE_STATE &tile_state,
    const double old_utility,
    const BOOL can_make_farm
) {

    sint32 road_type = -1;
    if (INST_TILE_STATE_OK == tile_state) {
        if (ai->m_world->GetRoad(&w, &road_type)) {
            tile_state = INST_TILE_STATE_ROAD;
        }
    }


    double u;
    if (can_make_farm) {

      u = EstimateInstallationFarm(ai, inst_idx, w, idx_city, median, tile_state) +
        EstimateInstallationQuarry(ai, inst_idx, w, idx_city, median, tile_state) ;




    } else {
        u = EstimateInstallationQuarry(ai, inst_idx, w, idx_city, median, tile_state) ;

    }

    if ((INST_TILE_STATE_OVERWRITE == tile_state) && (0.0 < u)){
        u -= old_utility;
    } else if (INST_TILE_STATE_UPGRADE == tile_state) {
        u *= 0.75;
    }

    return u;
}
































void CityAgent::InsertNewPotentialUnits(AiMain *ai, const REGISTER_TYPE r,
                                        const sint32 item_type)
{
    sint32 unit_idx;
    sint32 unit_num = ai->m_unitDB->GetNumUnitTypes();
    BSetID tmpid;
    ArmyAgent *aa = NULL;
    sint32 max_hp;

    for (unit_idx=0; unit_idx<unit_num; unit_idx++) {
        if (m_has_space_launcher &&
            ai->m_unitDB->CanMoveSpace(unit_idx)) {

        } else {
            if (ai->m_player->UnitTypeCanEnter(unit_idx, m_pos) == FALSE) {
              continue;
            }
        }

        if (CanBuildUnit(ai, unit_idx)) {
            if (!IsAPotentialUnit(unit_idx)) {
                tmpid = ai->GenUnbuiltID();

                aa = new ArmyAgent(ai, tmpid, this, AGENT_STATE_UNBUILT);

                max_hp = ai->m_unitDB->GetHP(unit_idx);
                aa->SetSingleUnitType(ai, unit_idx, max_hp);

				m_list_of_potential_units->Append_Data( aa );

				AddPotentialArmyStrength(ai, aa->GetUnbuiltType());
            }
        }
    }

	ComputeBestAgent(ai);
}


void CityAgent::ComputePotentialUnits(AiMain *ai)
{
    sint32 unit_idx;
    sint32 unit_num = ai->m_unitDB->GetNumUnitTypes();
    BSetID tmpid;
    ArmyAgent *aa = NULL;
	BOOL can_build;
	BOOL potential_unit;
	BOOL recompute_strength;
    sint32 i;
    sint32 max_hp;


    for (unit_idx=0; unit_idx<unit_num; unit_idx++)
		{
			can_build = CanBuildUnit(ai, unit_idx);
			potential_unit = IsAPotentialUnit(unit_idx);
			recompute_strength = FALSE;

			if (can_build && !potential_unit)
				{

					tmpid = ai->GenUnbuiltID();

					aa = new ArmyAgent(ai, tmpid, this, AGENT_STATE_UNBUILT);

					max_hp = ai->m_unitDB->GetHP(unit_idx);
					aa->SetSingleUnitType(ai, unit_idx, max_hp);

					m_list_of_potential_units->Append_Data( aa );

					AddPotentialArmyStrength(ai, aa->GetUnbuiltType());

				}
			else if (!can_build && potential_unit)
				{

					for (i=0; i < m_list_of_potential_units->count; i++)
						{

							aa = (ArmyAgent *) m_list_of_potential_units->
								Return_Data_By_Number(i);

							Assert(aa->the_squad == NULL);

							Assert(aa->my_planner == NULL);

							m_list_of_potential_units->
								Delete_Element(i);

							delete aa;
                            aa = NULL;
						}

					recompute_strength = TRUE;
				}

    }

	if (recompute_strength)
		RecomputePotentialArmyStrength(ai);

	ComputeBestAgent(ai);
}


BOOL CityAgent::CanBuildUnit(AiMain *ai, sint32 unit_idx)
{

    if (!(ai->m_player->CityCanBuildUnit(m_id.GetVal(),unit_idx)) ||
        ai->m_unitDB->GetIsTrader(unit_idx) ||
        (ai->m_unitDB->GetProductionCost(unit_idx) <= 0) ) {
        return(FALSE);
    }

    if ((ai->m_unitDB->CanMoveWater(unit_idx) ||
         ai->m_unitDB->CanMoveShallowWater(unit_idx)) &&
        !(ai->m_unitDB->CanMoveLand(unit_idx)) &&
        !(ai->m_unitDB->CanMoveAir(unit_idx))) {

		MapPointData wrap_pos;




		ai->m_map->Wrap(m_pos->x+1, m_pos->y+1, m_pos->z, wrap_pos);
		if (ai->m_continents->IsOcean(ai, wrap_pos.x, wrap_pos.y, wrap_pos.z))
			return TRUE;

		ai->m_map->Wrap(m_pos->x+1, m_pos->y-1, m_pos->z, wrap_pos);
		if (ai->m_continents->IsOcean(ai, wrap_pos.x, wrap_pos.y, wrap_pos.z))
			return TRUE;

		ai->m_map->Wrap(m_pos->x, m_pos->y+1, m_pos->z, wrap_pos);
		if (ai->m_continents->IsOcean(ai, wrap_pos.x, wrap_pos.y, wrap_pos.z))
			return TRUE;

		ai->m_map->Wrap(m_pos->x, m_pos->y-1, m_pos->z, wrap_pos);
		if (ai->m_continents->IsOcean(ai, wrap_pos.x, wrap_pos.y, wrap_pos.z))
			return TRUE;

		ai->m_map->Wrap(m_pos->x-1, m_pos->y+1, m_pos->z, wrap_pos);
		if (ai->m_continents->IsOcean(ai, wrap_pos.x, wrap_pos.y, wrap_pos.z))
			return TRUE;

		ai->m_map->Wrap(m_pos->x-1, m_pos->y, m_pos->z, wrap_pos);
		if (ai->m_continents->IsOcean(ai, wrap_pos.x, wrap_pos.y, wrap_pos.z))
			return TRUE;

		ai->m_map->Wrap(m_pos->x-1, m_pos->y-1, m_pos->z, wrap_pos);
		if (ai->m_continents->IsOcean(ai, wrap_pos.x, wrap_pos.y, wrap_pos.z))
			return TRUE;

		return FALSE;
	}

    return(TRUE);
}

void CityAgent::AddPotentialArmyStrength(AiMain *ai, sint32 unit_type)
{









    double as = ai->m_unitDB->GetAttack(unit_type);
    double ds = ai->m_unitDB->GetDefense(unit_type);
    double fp = ai->m_unitDB->GetFirepower(unit_type);
    double hp = ai->m_unitDB->GetHP(unit_type);

    float old_attack = m_agent_strength->GetAttack();
    float new_attack = float(as * fp * hp);

    if (old_attack < new_attack) {
        if (the_squad) {

			the_squad->m_squad_strength.AddNetAttack(new_attack - old_attack);
        }
        m_agent_strength->SetAttack(new_attack);
    }

    float old_defense = m_agent_strength->GetDefense();
    float new_defense = float(ds * fp * hp);
    if (old_defense < new_defense) {
        if (the_squad) {

             the_squad->m_squad_strength.AddNetDefense(new_defense - old_defense);
        }
        m_agent_strength->SetDefense(new_defense);
    }















}


void CityAgent::RecomputePotentialArmyStrength(AiMain *ai)
{
	ArmyAgent *aa;

	if (the_squad)

		the_squad->m_squad_strength.Subtract(m_agent_strength);

	m_agent_strength->Init();

	for (sint32 i=0; i < m_list_of_potential_units->count; i++)
	{

		aa = (ArmyAgent *) m_list_of_potential_units->Return_Data_By_Number(i);

		AddPotentialArmyStrength(ai, aa->GetUnbuiltType());
    }
}

BOOL CityAgent::IsAPotentialUnit(const sint32 unit_idx)
{
    sint32 i;
    ArmyAgent *aa=NULL;

    for (i=0; i < m_list_of_potential_units->count; i++) {

        aa = (ArmyAgent *) m_list_of_potential_units->Return_Data_By_Number(i);

        if (aa->GetUnbuiltType() == unit_idx) {
            return TRUE;
        }
    }

    return FALSE;
}

void CityAgent::HookPotentialArmies()
{
    sint32 i;
    ArmyAgent *aa=NULL;

    for (i=0; i < m_list_of_potential_units->count; i++) {

        aa = (ArmyAgent *) m_list_of_potential_units->Return_Data_By_Number(i);

        aa->SetHomeCityAgent(this);
    }
}
















void CityAgent::HookRoads(AiMain *ai)

{
    RoadDest *p=NULL;

#ifdef _DEBUG
sint32 finite_loop=0;
#endif

    for (p = m_road_list; p; p = p->m_next) {
        Assert(++finite_loop < 100000);
        p->HookUp(ai);
    }
}

void CityAgent::RemoveCityReferance(CityAgent *remove_me)
{
    RoadDest *ptr;
    RoadDest *old;
    RoadDest *del_me;

#ifdef _DEBUG
    sint32 finite_loop=0;
#endif

    BOOL searching;
    do {
        searching = FALSE;
        if (m_road_list == NULL) {
            return;
        }

        if (m_road_list->m_dest == remove_me) {
            del_me = m_road_list;
            m_road_list = m_road_list->m_next;
            searching = TRUE;
            delete del_me;
        }

Assert(++finite_loop < 1000000);
    } while (searching);


    old = m_road_list;
    ptr = m_road_list->m_next;

    while (ptr) {
Assert(++finite_loop < 100000);
        if (ptr->m_dest == remove_me) {
            old->m_next = ptr->m_next;
            del_me = ptr;
            ptr = ptr->m_next;
            delete del_me;
        } else {
            old = ptr;
            ptr = ptr->m_next;
        }
    }

}

void CityAgent::ResetSquad(Squad *new_squad)
{
    the_squad = new_squad;
}

void CityAgent::DumpStr(AiMain *ai, char *str)
{
    uint32 u_id = m_id.GetVal();
    BOOL is_unknown_id;

    ai->m_player->SetCityDBGString (u_id, &is_unknown_id, str);
}











double CityAgent::Estimate_City_Strength(AiMain *ai)
{
	sint32 attack;
	sint32 hp;
	sint32 fp;

	if (ai->m_science_agent->GetBestLandAttack(attack) &&
		ai->m_science_agent->GetBestHP(hp) &&
		ai->m_science_agent->GetBestFP(fp))
		return attack * hp * fp;
	else
		return 1.0;
}













sint32 CityAgent::Estimate_City_Value(AiMain *ai)
{
	sint32 value = 1;
	sint32 type;
	sint32 max_building = ai->m_buildingDB->GetNumBuildings();
	sint32 max_wonder = ai->m_wonderDB->GetNumWonders();

	for (type = 0; type < max_wonder; type++)
		{

			BSetID *home_city = ai->m_wonder_table->GetHomeCity(type);
			if (!home_city)
				continue;

			if ( home_city->GetVal() == m_id.GetVal())
				{
					value += (sint32) ai->m_wonder_table->GetProductionCost(type);
				}
		}

	for (type = 0; type < max_building; type++)
		{
			if (!m_building[type].GetState() == BUILDING_STATE_UNBUILT)
				{
					value += ai->m_buildingDB->GetProductionCost(type);
				}
		}
	return value;
}













sint32 CityAgent::Already_Has_One(sint32 building_type)
{

	sint32 built_yet;




	built_yet = !(m_building[building_type].GetState()
					== BUILDING_STATE_UNBUILT);

	return built_yet;
}













sint32 CityAgent::Can_Build_Building(AiMain *ai,sint32 building_type)
{
    uint32 u_id = m_id.GetVal();




	if (ai->m_player->CityCanBuildBuilding(u_id, building_type) &&
		PretestContructBuilding(ai, building_type))


		return TRUE;
	else return FALSE;
}






PointerList<IC3TradeOffer> *CityAgent::GetTradeOffers()
{
	return m_tradeOffers;
}

void CityAgent::AddTradeOffer(IC3TradeOffer *offer)
{
	if(!m_tradeOffers) {
		m_tradeOffers = new PointerList<IC3TradeOffer>;
	}

	offer->AddRef();
	m_tradeOffers->AddTail(offer);
}

void CityAgent::RemoveTradeOffer(uint32 id)
{
	if(!m_tradeOffers)
		return;
	PointerList<IC3TradeOffer>::Walker walk;
	for(walk.SetList(m_tradeOffers); walk.IsValid(); walk.Next()) {
		if(walk.GetObj()->GetID() == id) {
			walk.GetObj()->Release();
			walk.Remove();
			if(m_tradeOffers->GetCount() < 1) {
				delete m_tradeOffers;
				m_tradeOffers = NULL;
			}
			return;
		}
	}
}

BOOL CityAgent::SeenByPlayer(AiMain *ai, PLAYER_INDEX player)
{
	return ai->m_player->PlayerCanSeeCity(m_id.GetVal(), player);
}

void CityAgent::ComputeBestAgent(AiMain *ai)
{
	sint32 unit_type;
	sint32 best_unit_type;
	sint32 potential_unit_idx;
	uint32 goal_idx;

    sint32 unit_type_max = ai->m_unitDB->GetNumUnitTypes();
	double attack;
	double defense;
	double firepower;
	double hitpoints;

	double new_attack_value;
	double new_defense_value;
	sint32 new_cargo;

	double best_attack_value;
	double best_defense_value;
	sint32 best_cargo;
	ArmyAgent *aa;
	ArmyAgent *best_aa;

	for (goal_idx = 0; goal_idx < GOAL_TYPE_MAX; goal_idx++)
		{

			m_bestUnitForGoal[goal_idx] = NULL;

			for (potential_unit_idx=0;
				 potential_unit_idx < m_list_of_potential_units->count;
				 potential_unit_idx++)
				{
					aa = (ArmyAgent *) m_list_of_potential_units->
						Return_Data_By_Number(potential_unit_idx);

					if (ai->m_player->UnitTypeCanEnter(aa->GetUnbuiltType(),
													   m_pos) == FALSE)
						continue;








					if (aa->CanMoveSpace(ai) &&

						(!m_has_space_launcher || m_pos->z == SPACE_Z))
						continue;

					unit_type = aa->GetUnbuiltType();
					Assert(unit_type >= 0);
					Assert(unit_type < unit_type_max);

					attack = ai->m_unitDB->GetAttack(unit_type);
					defense = ai->m_unitDB->GetDefense(unit_type);
					firepower = ai->m_unitDB->GetFirepower(unit_type);
					hitpoints = ai->m_unitDB->GetHP(unit_type);

					new_attack_value = attack * firepower * hitpoints;
					new_defense_value = defense * firepower * hitpoints;
					new_cargo = ai->m_unitDB->GetMaxCargo(unit_type);

					if (ArmyGoal::PretestUnitTypes(ai, aa, (GOAL_TYPE) goal_idx) == FALSE)
						continue;

					best_aa = m_bestUnitForGoal[goal_idx];

					if (best_aa == NULL)
						{

							m_bestUnitForGoal[goal_idx] = aa;
							continue;
						}

					best_unit_type = best_aa->GetUnbuiltType();

					attack = ai->m_unitDB->GetAttack(best_unit_type);
					defense = ai->m_unitDB->GetDefense(best_unit_type);
					firepower = ai->m_unitDB->GetFirepower(best_unit_type);
					hitpoints = ai->m_unitDB->GetHP(best_unit_type);

					best_attack_value = attack * firepower * hitpoints;
					best_defense_value = defense * firepower * hitpoints;
					best_cargo = ai->m_unitDB->GetMaxCargo(best_unit_type);

					if (new_attack_value >= best_attack_value &&
						new_defense_value >= best_defense_value &&
						new_cargo >= best_cargo)
						m_bestUnitForGoal[goal_idx] = aa;
				}
		}
}

void CityAgent::SetCaptured(const sint32 &turn)
{
	m_turnCaptured = turn;
}

BOOL CityAgent::WasRecentlyCaptured(AiMain *ai) const
{
	return (m_turnCaptured > 0 &&
			(ai->m_round_count->GetRound() - m_turnCaptured) >
			ai->m_planner->the_AIP.captured_city_defend_turns);
}

void CityAgent::RemoveFromGarrison(sint16 count)
{
	Assert(m_garrisonCount >= 0);
	m_garrisonCount-= count;
	Assert(m_garrisonCount >= 0);
}

double CityAgent::GetProductionBonus(const PRODUCTION_TYPE &type) const
{
	Assert(m_production_bonus);
	Assert(type < 4);
	Assert(type >= 0);

	if (m_production_bonus)
		return m_production_bonus[type];

	return 0.0;
}

void CityAgent::UpdateProductionBonuses(AiMain *ai)
{
	BOOL is_unknown_id;
    for (sint32 type = 0; type < (sint32) 4; type++)
	{
		Assert(m_production_bonus);
		m_production_bonus[type] = ai->m_player->
			GetProductionPercentBonus(m_id.GetVal(), is_unknown_id,
			(PRODUCTION_TYPE) type);
		Assert(!is_unknown_id);
		Assert(m_production_bonus[type] >= 0.0);
	}
}
