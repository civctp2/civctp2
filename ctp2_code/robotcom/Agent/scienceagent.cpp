#include "c3.h"
#include "MapPoint.h"
#include "civarchive.h"
#include "FlatPtr.h"
#include "ic3GameState.h"
#include "ic3UnitDB.h"
#include "ic3Government.h"
#include "ic3Science.h"
#include "ic3InstDB.h"
#include "ic3BlgDB.h"
#include "ic3Wonder.h"
#include "ic3TerrDB.h"

#include "aimain.h"
#include "aicell.h"
#include "AiMap.h"
#include "airndcnt.h"
#include "aicause.h"

#include "scienceagent.h"

#include "FzOut.h"

extern double fz_sci_unit_mil;
extern double fz_sci_unit_specialability ;
extern double fz_sci_unit_moveair;
extern double fz_sci_unit_movewater;
extern double fz_sci_unit_movespace;
extern double fz_sci_unit_settlewater;
extern double fz_sci_unit_settlespace;
extern double fz_sci_unit_nuke;
extern double fz_sci_unit_caravan;
extern double fz_sci_inst_move;
extern double fz_sci_inst_def;
extern double fz_sci_inst_food;
extern double fz_sci_inst_prod;
extern double fz_sci_inst_vis;
extern double fz_sci_blg_def;
extern double fz_sci_blg_food;
extern double fz_sci_blg_prod;
extern double fz_sci_blg_sci;
extern double fz_sci_blg_gold;
extern double fz_sci_blg_happy;
extern double fz_sci_wndr_isnew;
extern double fz_sci_wndr_obs_his;
extern double fz_sci_wndr_obs_mine;
extern double fz_sci_tech_end_game;

extern double fz_sci_gov_workday;
extern double fz_sci_gov_wages;
extern double fz_sci_gov_rations;
extern double fz_sci_gov_sci;
extern double fz_sci_gov_poll;
extern double fz_sci_gov_poll_unhap;
extern double fz_sci_gov_emp_size;
extern double fz_sci_gov_crime;
extern double fz_sci_gov_martial_law;
extern double fz_sci_gov_combat_moral;
extern double fz_sci_gov_readiness;
extern double fz_sci_gov_unit_dist;
extern double fz_sci_gov_gold;

extern double fz_sci_plantime;




#include "InstBidList.h"
#include "GoalInst.h"
#include "GoodAgent.h"
#include "BldAgent.h"

#include "gold.h"
#include "AllocWGF.h"
#include "WonderBlg.h"
#include "WonderTbl.h"








ScienceAgent::ScienceAgent()
{
}









ScienceAgent::ScienceAgent(AiMain *ai)
{
    Init(ai);
}

ScienceAgent::ScienceAgent(AiMain *ai, CivArchive &archive)
{

	m_unitMilitaryWorth = NULL;

    Serialize(archive);
}









void ScienceAgent::Init(AiMain *ai)
	{
	double	unitValue,
			discoveryValue ;

	sint32	science,
			unitNum ;

    m_best_freight = -1;
    m_current_science = -1;
    m_must_learn_more_science = TRUE;
	m_numUnitTypes = ai->m_unitDB->GetNumUnitTypes();
	m_numAdvances = ai->m_science->GetNumAdvances() ;

	m_best_hp = -1;
	m_best_fp = -1;
	m_best_land_attack = -1;
	m_best_water_attack = -1;
	m_best_air_attack = -1;
	m_best_space_attack = -1;

	m_best_land_defense = -1;
	m_best_water_defense = -1;
	m_best_air_defense = -1;
	m_best_space_defense = -1;

	m_best_land_move = -1;
	m_best_water_move = -1;
	m_best_air_move = -1;
	m_best_space_move = -1;

    m_best_road_type = -1;
    m_best_farm_type = -1;
    m_best_quarry_type = -1;
    m_has_undersea_tunnel = FALSE;

	double m_best_convert_chance = -1.0;

	m_unitMilitaryWorth = new double[m_numAdvances] ;
	for (science=0; science<m_numAdvances; science++)
		{
		discoveryValue = 0.0 ;
		for (unitNum=0; unitNum<m_numUnitTypes; unitNum++) {
			if (ai->m_unitDB->EnablingDiscovery(unitNum) == science) {
				unitValue = (ai->m_unitDB->GetAttack(unitNum) + ai->m_unitDB->GetDefense(unitNum)) * ai->m_unitDB->GetFirepower(unitNum) * ai->m_unitDB->GetHP(unitNum) ;
				discoveryValue += unitValue ;
				}

			}

		m_unitMilitaryWorth[science] = discoveryValue ;
		}

    m_has_water_settle = FALSE;
    m_has_water_move = FALSE;
    m_has_space_settle = FALSE;
    m_has_space_move = FALSE;
    m_can_rail_launch = FALSE;

    m_can_missile_nuke = FALSE;
    m_can_plant_nuke = FALSE;
    m_can_make_park = FALSE;
    m_can_underground_railway = FALSE;
    m_can_convert_city = FALSE;
    m_can_establish_embassy = FALSE;
    m_can_franchise = FALSE;
    m_can_assasinate_ruler = FALSE;
    m_can_steal_technology = FALSE;
    m_can_injoin = FALSE;
    m_can_incite_revolution = FALSE;
    m_can_cause_unhappiness = FALSE;
    m_can_cause_bioterror = FALSE;
    m_can_cause_nanoattack = FALSE;
    m_can_sue = FALSE;
    m_can_build_wormhole_probe = FALSE;

    sint32 i;
    m_science_count=0;
    BOOL is_new_unit_discovered;
    for (i=0; i< m_numAdvances; i++) {
	   if (ai->m_science->HasAdvance(i)) {
           m_science_count++;
           UpdateWhatIKnow(ai, i, is_new_unit_discovered);
       }
    }
}









ScienceAgent::~ScienceAgent()
	{
	delete m_unitMilitaryWorth ;
     m_unitMilitaryWorth = NULL;
	}











void ScienceAgent::Serialize(CivArchive &archive)
	{
    CHECKSERIALIZE

	if (archive.IsStoring()) {
        archive.PutSINT32(m_has_water_settle);
        archive.PutSINT32(m_has_water_move);
        archive.PutSINT32(m_has_space_settle);
        archive.PutSINT32(m_has_space_move);
        archive.PutSINT32(m_best_freight);
        archive.PutSINT32(m_can_rail_launch);

        archive.PutSINT32(m_science_count);

		archive.PutSINT32(m_best_land_attack);
		archive.PutSINT32(m_best_water_attack);
		archive.PutSINT32(m_best_air_attack);
		archive.PutSINT32(m_best_space_attack);

		archive.PutSINT32(m_best_land_defense);
		archive.PutSINT32(m_best_water_defense);
		archive.PutSINT32(m_best_air_defense);
		archive.PutSINT32(m_best_space_defense);

		archive.PutSINT32(m_best_land_move);
		archive.PutSINT32(m_best_water_move);
		archive.PutSINT32(m_best_air_move);
		archive.PutSINT32(m_best_space_move);

        archive.PutSINT32(m_best_road_type);
        archive.PutSINT32(m_best_farm_type);
        archive.PutSINT32(m_best_quarry_type);

        archive.PutSINT32(m_best_hp);
        archive.PutSINT32(m_best_fp);

		archive.PutSINT32((sint32) m_best_convert_chance);
        archive.PutSINT32(m_has_undersea_tunnel);

		archive<<m_numAdvances ;
		archive<<m_numUnitTypes ;
		archive.PutSINT32(m_must_learn_more_science) ;
		archive<<m_current_science ;
		archive.Store((uint8*)m_unitMilitaryWorth, m_numAdvances * sizeof(double)) ;

        archive.PutSINT32(m_can_missile_nuke);
        archive.PutSINT32(m_can_plant_nuke);
        archive.PutSINT32(m_can_make_park);
        archive.PutSINT32(m_can_underground_railway);
        archive.PutSINT32(m_can_convert_city);
        archive.PutSINT32(m_can_establish_embassy);
        archive.PutSINT32(m_can_franchise);
        archive.PutSINT32(m_can_assasinate_ruler);
        archive.PutSINT32(m_can_steal_technology);
        archive.PutSINT32(m_can_injoin);
        archive.PutSINT32(m_can_incite_revolution);
        archive.PutSINT32(m_can_cause_unhappiness);
        archive.PutSINT32(m_can_cause_bioterror);
        archive.PutSINT32(m_can_cause_nanoattack);
        archive.PutSINT32(m_can_sue);
        archive.PutSINT32(m_can_build_wormhole_probe);

    } else {
        m_has_water_settle = archive.GetSINT32();
        m_has_water_move = archive.GetSINT32();
        m_has_space_settle = archive.GetSINT32();
        m_has_space_move = archive.GetSINT32();
        m_best_freight = archive.GetSINT32();

        m_can_rail_launch = archive.GetSINT32();

        m_science_count = archive.GetSINT32();

		m_best_land_attack = archive.GetSINT32();
		m_best_water_attack = archive.GetSINT32();
		m_best_air_attack = archive.GetSINT32();
		m_best_space_attack = archive.GetSINT32();

		m_best_land_defense = archive.GetSINT32();
		m_best_water_defense = archive.GetSINT32();
		m_best_air_defense = archive.GetSINT32();
		m_best_space_defense = archive.GetSINT32();

		m_best_land_move = archive.GetSINT32();
		m_best_water_move = archive.GetSINT32();
		m_best_air_move = archive.GetSINT32();
		m_best_space_move = archive.GetSINT32();

        m_best_road_type = archive.GetSINT32();
        m_best_farm_type = archive.GetSINT32();
        m_best_quarry_type = archive.GetSINT32();

        m_best_hp = archive.GetSINT32();
        m_best_fp = archive.GetSINT32();

		m_best_convert_chance = (double) archive.GetSINT32();

        m_has_undersea_tunnel = archive.GetSINT32();

		archive>>m_numAdvances ;
		archive>>m_numUnitTypes ;
		m_must_learn_more_science = archive.GetSINT32() ;
		archive>>m_current_science;

        if (m_unitMilitaryWorth) {
			delete m_unitMilitaryWorth ;
            m_unitMilitaryWorth = NULL;
        }

		m_unitMilitaryWorth = new double[m_numAdvances] ;
		archive.Load((uint8*)m_unitMilitaryWorth, m_numAdvances * sizeof(double)) ;

        m_can_missile_nuke = archive.GetSINT32();
        m_can_plant_nuke = archive.GetSINT32();
        m_can_make_park = archive.GetSINT32();
        m_can_underground_railway = archive.GetSINT32();
        m_can_convert_city = archive.GetSINT32();
        m_can_establish_embassy = archive.GetSINT32();
        m_can_franchise = archive.GetSINT32();
        m_can_assasinate_ruler = archive.GetSINT32();
        m_can_steal_technology = archive.GetSINT32();
        m_can_injoin = archive.GetSINT32();
        m_can_incite_revolution = archive.GetSINT32();
        m_can_cause_unhappiness = archive.GetSINT32();
        m_can_cause_bioterror = archive.GetSINT32();
        m_can_cause_nanoattack = archive.GetSINT32();
        m_can_sue = archive.GetSINT32();
        m_can_build_wormhole_probe = archive.GetSINT32();

	}
}









void ScienceAgent::BeginTurn()
	{
	}









double ScienceAgent::GetUnitMilitaryWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 unit_idx)
{
	Assert(sci_id>=0) ;
	Assert(sci_id<m_numAdvances) ;

	return c[COEFTYPE_UNIT_MILITARY] * m_unitMilitaryWorth[sci_id];






















}












#define ABILITY_VALUE	0.05

double ScienceAgent::GetUnitAbilityWorth(AiMain *ai, double *c,
  const sint32 sci_id, const sint32 unit_idx)
{
	double	worth = 0.0 ;

	if (ai->m_unitDB->CanInvestigateCity(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanStealTechnology(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanInciteRevolution(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanAssasinateRuler(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanInvestigateReadiness(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanNullifyCityWalls(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanCreateFranchise(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanCauseUnhappiness(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanConductHits(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanConductBioTerror(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanConductNanoTerror(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanConductSlaveRaids(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanConductSettlerSlaveRaids(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanConductSlaveUprising(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanConductUndergroundRailway(unit_idx))
		worth += ABILITY_VALUE ;

	if (ai->m_unitDB->CanVictoryEnslave(unit_idx))
		worth += ABILITY_VALUE ;

	return c[COEFTYPE_UNIT_ABILITY] * worth;
}








double ScienceAgent::GetUnitMoveAirWorth(AiMain *ai, double *c,
  const sint32 sci_id, const sint32 unit_idx)
{
    if (ai->m_unitDB->CanMoveAir(unit_idx)) {
        return c[COEFTYPE_UNIT_MOVE_AIR];
    } else {
    	return 0.0;
    }
}









double ScienceAgent::GetUnitMoveWaterWorth(AiMain *ai, double *c,
  const sint32 sci_id, const sint32 unit_idx)
{
    if (ai->m_unitDB->CanMoveWater(unit_idx)) {
		return c[COEFTYPE_UNIT_MOVE_WATER];
    } else {
        return 0.0;
    }
}








double ScienceAgent::GetUnitMoveSpaceWorth(AiMain *ai, double *c,
  const sint32 sci_id, const sint32 unit_idx)
{
    if (ai->m_unitDB->CanMoveSpace(unit_idx)) {
		return c[COEFTYPE_UNIT_MOVE_SPACE];
    } else {
    	return 0.0;
	}
}









double ScienceAgent::GetUnitSettleWaterWorth(AiMain *ai, double *c,
  const sint32 sci_id, const sint32 unit_idx)
{
    if (ai->m_unitDB->CanSettleWater(unit_idx)) {
		return c[COEFTYPE_UNIT_SETTLE_WATER] ;
    } else {
    	return 0.0;
	}
}








double ScienceAgent::GetUnitSettleSpaceWorth(AiMain *ai, double *c,
  const sint32 sci_id, const sint32 unit_idx)
{
    if (ai->m_unitDB->CanSettleSpace(unit_idx)) {
        return c[COEFTYPE_UNIT_SETTLE_SPACE];
    } else {
    	return 0.0 ;
    }
}









double ScienceAgent::GetUnitNukeWorth(AiMain *ai, double *c,
  const sint32 sci_id, const sint32 unit_idx)
{
    if (ai->m_unitDB->CanPlantNuke(unit_idx) || ai->m_unitDB->CanLaunchNuke(unit_idx)) {
		return c[COEFTYPE_UNIT_NUKE];
    } else {
	    return 0.0;
	}
}








double ScienceAgent::GetUnitCaravanWorth(AiMain *ai, double *c,
  const sint32 sci_id, const sint32 unit_idx)
{
    if (ai->m_unitDB->GetIsTrader(unit_idx)) {
        return c[COEFTYPE_UNIT_CARAVAN] * ai->m_unitDB->UnitTypeMovementPoints(unit_idx);
    } else {
        return 0.0;
    }
}

double ScienceAgent::UtilityUnit(AiMain *ai, double *c, const sint32 sci_id)
{
    sint32 unit_idx;

    double utility=0.0;
    for (unit_idx=0; unit_idx < m_numUnitTypes; unit_idx++) {

       if (ai->m_unitDB->EnablingDiscovery(unit_idx) == sci_id) {
          utility +=
            GetUnitMilitaryWorth(ai, c, sci_id, unit_idx) +
		    GetUnitAbilityWorth(ai, c, sci_id, unit_idx) +
		    GetUnitMoveAirWorth(ai, c, sci_id, unit_idx) +
		    GetUnitMoveWaterWorth(ai, c, sci_id, unit_idx) +
		    GetUnitMoveSpaceWorth(ai, c, sci_id, unit_idx) +
		    GetUnitSettleWaterWorth(ai, c, sci_id, unit_idx) +
		    GetUnitSettleSpaceWorth(ai, c, sci_id, unit_idx) +
		    GetUnitNukeWorth(ai, c, sci_id, unit_idx) +
		    GetUnitCaravanWorth(ai, c, sci_id, unit_idx);
       }
   }
   return utility;
}








#define k_AVE_CITY_SIZE 10.0
double ScienceAgent::UnhappyGovWorkday(AiMain *ai, double *c, const sint32 gov)
{
    double work_coef = ai->m_governmentDB->GetWorkdayCoef(gov);
    double work_expt = ai->m_governmentDB->GetWorkdayExpectation(gov);

    return c[COEFTYPE_GOV_RATIONS] * k_AVE_CITY_SIZE * work_coef * (work_expt - 2.0);
}

double ScienceAgent::UnhappyGovWages(AiMain *ai, double *c, const sint32 gov)
{
    double wage_coef = ai->m_governmentDB->GetWagesCoef(gov);
    double wage_expt = ai->m_governmentDB->GetWagesExpectation(gov);

    return c[COEFTYPE_GOV_RATIONS] * k_AVE_CITY_SIZE * wage_coef * (wage_expt - 2.0);
}

double ScienceAgent::UnhappyGovRations(AiMain *ai, double *c, const sint32 gov)
{
    double ration_coef = ai->m_governmentDB->GetRationsCoef(gov);
    double ration_expt = ai->m_governmentDB->GetRationsExpectation(gov);

    return c[COEFTYPE_GOV_RATIONS] * k_AVE_CITY_SIZE * ration_coef * (ration_expt - 2.0);
}

#define k_AVE_CITY_DIST 1000

double ScienceAgent::UnhappyGovEmpSize(AiMain *ai, double *c, const sint32 gov)
{
   double scale = ai->m_governmentDB->GetEmpireDistanceScale(gov);

   return c[COEFTYPE_GOV_EMPIRE_SIZE] * scale * k_AVE_CITY_DIST * k_AVE_CITY_DIST;
}

double ScienceAgent::HappyMartialLaw(AiMain *ai,  double *c, const sint32 gov)
{

    double e = ai->m_governmentDB->GetMartialLawEffect(gov);
    double u = ai->m_governmentDB->GetMaxMartialUnits(gov);

    return c[COEFTYPE_GOV_MARTIAL_LAW] * e * u;
}

#define k_AVE_UNITS_OVERSEAS 1.0

double ScienceAgent::UnhappyUnitDist(AiMain *ai, double *c, const sint32 gov)
{
    double d = ai->m_governmentDB->GetAtHomeRadius(gov);
    double o = ai->m_governmentDB->GetOverseasCoef(gov);

    return c[COEFTYPE_GOV_UNIT_DIST] * o * k_AVE_UNITS_OVERSEAS;
}

#define k_LOSSES_OVERSEAS 3.0
#define k_LOSSES_HOME 1.0

double ScienceAgent::UnhappyCombatMoral(AiMain *ai, double *c, const sint32 gov)
{
    double o = ai->m_governmentDB->GetOverseasDefeatCoef(gov);
    double h = ai->m_governmentDB->GetHomeDefeatCoef(gov);

    return c[COEFTYPE_GOV_COMBAT_MORAL] * (k_LOSSES_OVERSEAS * o - k_LOSSES_HOME * h);
}

#define k_AVE_POLLUTION_PER_CITY 5.0

double ScienceAgent::UnhappyPolluion(AiMain *ai, double *c, const sint32 gov)
{
    double p = ai->m_governmentDB->GetPollutionCoef(gov);

    return p * k_AVE_POLLUTION_PER_CITY;
}

double ScienceAgent::GovCrime(AiMain *ai, double *c,
        const sint32 gov, const double happy)
{
    double o = ai->m_governmentDB->GetCrimeOffset(gov);
    double m = ai->m_governmentDB->GetCrimeCoef(gov);

    return c[COEFTYPE_GOV_CRIME] * (o - happy) * m;
}

double ScienceAgent::GovGold(AiMain *ai, double *c,
    const sint32 gov, const double happy, const double crime)
{

    double i = ai->m_gold->GetGrossIncome();
    i -= i * crime;

    return c[COEFTYPE_GOV_GOLD] * i;
}

#define k_AVE_CITY_PRODUCTION 100.0
double ScienceAgent::UtilityGovProduction(AiMain *ai, double *c,
   const sint32 gov, const double crime)
{
    double p = k_AVE_CITY_PRODUCTION;
    p -= p * crime;

    return p;
}

#define k_DESIRED_SCIENCE 100
#define k_TOTAL_SCI_FROM_CITY 10

double ScienceAgent::UtilityGovKnowlege(AiMain *ai, double *c,
   const sint32 gov, const double gold)
{
    double s = ai->m_governmentDB->GetKnowledgeCoef(gov);
    if (s <= 0.0) {
        s = 0.0001;
    }





    double spend_at_least_this = ai->m_alloc_WGF->Gold2Prod(k_DESIRED_SCIENCE);
    double gold_cost = (k_DESIRED_SCIENCE - k_TOTAL_SCI_FROM_CITY * s) / s;
    double prod_cost = ai->m_alloc_WGF->Gold2Prod(gold_cost);

    return spend_at_least_this - prod_cost;
}

#define k_NUM_UNITS 10.0

double ScienceAgent::UtilityGovReadiness(AiMain *ai, double *c,
   const sint32 gov, const double crime)
{









    double np = ai->m_governmentDB->GetProfessionalUnits(gov);
    double r = ai->m_governmentDB->GetReadyAlertCoef(gov);

    return -r * (np - k_NUM_UNITS);
}








double ScienceAgent::UtilityGoverment(AiMain *ai, double *c, sint32 sci_id)
{
	sint32	numGovs,
			gov ;

    double happy;
    double crime;
    double gold;
	double utility;

	numGovs = ai->m_governmentDB->GetNumGovs();

    utility = 0.0;

	for (gov=0; gov<numGovs; gov++) {

        if (ai->m_governmentDB->EnablingDiscovery(gov) == sci_id) {
            happy = 0.0;
            crime = 0.0;
            gold = 0.0;

            happy = 100.0 - UnhappyGovWorkday(ai,  c, gov)
                -UnhappyGovWages(ai, c, gov)
                -UnhappyGovRations(ai, c, gov)
                -UnhappyGovEmpSize(ai, c, gov)
                + HappyMartialLaw(ai, c, gov)
                - UnhappyUnitDist(ai, c, gov)
                - UnhappyCombatMoral(ai, c, gov)
                - UnhappyPolluion(ai, c, gov);

            crime = GovCrime(ai, c, gov, happy);
            gold =  GovGold(ai, c, gov, happy, crime);

			utility +=
                UtilityGovProduction(ai, c, gov, crime) +
	            UtilityGovKnowlege(ai, c, gov, gold) +
	            UtilityGovReadiness(ai, c, gov, crime);

        }
    }
    return utility;
}








double ScienceAgent::UtilityInstProd(AiMain *ai, double *c, const sint32 inst)
{
    sint32 idx_terrain, num_terrain;
    num_terrain = ai->m_terrainDB->GetNumTerrainTypes();
    Assert(0 < num_terrain);
    double sum=0.0;

    for (idx_terrain=0; idx_terrain<num_terrain; idx_terrain++) {
        sum += ai->m_installationDB->GetProductionBonus(inst, idx_terrain);
    }

    double p_ave = sum/double(num_terrain);

    return c[COEFTYPE_INSTALLATION_PRODUCTION] * p_ave * 0.2
        * ai->m_alloc_WGF->GetTotalAffectivePop();
}








double ScienceAgent::UtilityInstFood(AiMain *ai, double *c, const sint32 inst)
{
    sint32 idx_terrain, num_terrain;
    num_terrain = ai->m_terrainDB->GetNumTerrainTypes();
    Assert(0 < num_terrain);
    double sum=0.0;

    for (idx_terrain=0; idx_terrain<num_terrain; idx_terrain++) {
        sum += ai->m_installationDB->GetFoodBonus(inst, idx_terrain);
    }

    double f_ave = sum/double(num_terrain);

    double hunger = ai->m_alloc_WGF->GetCitizenHunger();
    double pop  = ai->m_alloc_WGF->GetTotalAffectivePop();
    double new_pop =  pop * f_ave / hunger;
    return c[COEFTYPE_INSTALLATION_FOOD] * new_pop * ai->m_alloc_WGF->GetAveTileProd();
}








double ScienceAgent::UtilityInstDefense(AiMain *ai, double *c, const sint32 inst)
{
    return c[COEFTYPE_INSTALLATION_DEFENSE] * ai->m_installationDB->GetInstDefenseDelta(inst);
}







double ScienceAgent::UtilityInstVision(AiMain *ai, double *c, const sint32 inst)
{
    return c[COEFTYPE_INSTALLATION_VISION] * ai->m_installationDB->GetInstVisionRange(inst);
}








double ScienceAgent::UtilityInstMovement(AiMain *ai, double *c, const sint32 inst)
{
    double m = ai->m_installationDB->GetInstMovement(inst);

    return c[COEFTYPE_INSTALLATION_VISION] * 1000.0 * (100.0 - m);
}









double ScienceAgent::UtilityInstallation(AiMain *ai, double *c, const sint32 sci_id)
{
    sint32	inst,
	numInstallations ;

	double	utility = 0.0 ;

	numInstallations = ai->m_installationDB->GetNumInstallations() ;
	for (inst=0; inst<numInstallations; inst++) {
        if (ai->m_installationDB->EnablingDiscovery(inst) == sci_id) {
            utility +=
                UtilityInstProd(ai, c, inst) +
                UtilityInstFood(ai, c, inst) +
                UtilityInstDefense(ai, c, inst) +
                UtilityInstVision(ai, c, inst) +
                UtilityInstMovement(ai, c, inst);

        }
	}

	return utility;
}








double ScienceAgent::GetBuildingDefenseWorth(AiMain *ai, double *c,
    const sint32 sci_id, const sint32 idx_blg)
{
	return c[COEFTYPE_BUILDING_DEFENSE] * ai->m_buildingDB->GetDefenseBonus(idx_blg);
}









double ScienceAgent::GetBuildingFoodWorth(AiMain *ai, double *c,
     const sint32 sci_id, const sint32 idx_blg)
{
	return c[COEFTYPE_BUILDING_FOOD] * ai->m_buildingDB->GetFoodBonus(idx_blg);
}









double ScienceAgent::GetBuildingProductionWorth(AiMain *ai, double *c,
   const sint32 sci_id, const sint32 idx_blg)
{
	return c[COEFTYPE_BUILDING_PRODUCTION] * ai->m_buildingDB->GetProductionBonus(idx_blg) ;
}









double ScienceAgent::GetBuildingScienceWorth(AiMain *ai, double *c,
    const sint32 sci_id, const sint32 idx_blg)
{
	return c[COEFTYPE_BUILDING_SCIENCE] * ai->m_buildingDB->GetScienceBonus(idx_blg) ;
}









double ScienceAgent::GetBuildingGoldWorth(AiMain *ai, double *c,
   const sint32 sci_id, const sint32 idx_blg)
{
	return c[COEFTYPE_BUILDING_GOLD] * ai->m_buildingDB->GetGoldBonus(idx_blg);
}









double ScienceAgent::GetBuildingHappyWorth(AiMain *ai, double *c,
   const sint32 sci_id, const sint32 idx_blg)
{
	return c[COEFTYPE_BUILDING_HAPPY] * ai->m_buildingDB->GetHappyBonus(idx_blg);
}









double ScienceAgent::UtilityBuilding(AiMain *ai, double *c, const sint32 sci_id)
{
    sint32 idx_blg, num_blg;
    num_blg = ai->m_buildingDB->GetNumBuildings();

    double utility = 0;
    for (idx_blg = 0; idx_blg < num_blg; idx_blg++) {
        if (ai->m_buildingDB->EnablingDiscovery(idx_blg) == sci_id) {
            utility +=
        	    GetBuildingDefenseWorth(ai, c, sci_id, idx_blg) +
	            GetBuildingFoodWorth(ai, c, sci_id, idx_blg) +
	            GetBuildingProductionWorth(ai, c, sci_id, idx_blg) +
	            GetBuildingScienceWorth(ai, c, sci_id, idx_blg) +
	            GetBuildingGoldWorth(ai, c, sci_id, idx_blg) +
	            GetBuildingHappyWorth(ai, c, sci_id, idx_blg);
        }
    }

    return utility;
}








double ScienceAgent::IsNewWonder(AiMain *ai, double *c,
   const sint32 sci_id, const sint32 idx_wonder)
{
    if (ai->m_wonderDB->EnablingDiscovery(idx_wonder) == sci_id) {
        if ((ai->m_wonder_table->GetGlobalState(idx_wonder) != WONDER_STATE_DONE) &&
            (ai->m_wonder_table->GetGlobalState(idx_wonder) != WONDER_STATE_OBSOLETE)) {
            return c[COEFTYPE_WONDER_IS_NEW];
        }
	}

	return 0.0;
}








double ScienceAgent::CanObsoleteHisWonder(AiMain *ai, double *c,
   const sint32 sci_id, const sint32 idx_wonder)
{
    if (ai->m_wonderDB->ObsoleteDiscovery(idx_wonder) == sci_id) {
        if (!ai->m_wonder_table->IOwnIt(idx_wonder)) {
            return c[COEFTYPE_WONDER_OBSOLETE_HIS];
        }
	}

	return 0.0;
}









double ScienceAgent::CanObsoleteMyWonder(AiMain *ai, double *c,
   const sint32 sci_id, const sint32 idx_wonder)
{
    if (ai->m_wonderDB->ObsoleteDiscovery(idx_wonder) == sci_id) {
        if ((ai->m_wonder_table->GetMyState(idx_wonder) == WONDER_STATE_DONE) &&
            (ai->m_wonder_table->IOwnIt(idx_wonder))) {
            return -c[COEFTYPE_WONDER_OBSOLETE_MINE];
        }
	}

	return 0.0;
}








double ScienceAgent::UtilityWonder(AiMain *ai, double *c, const sint32 sci_id)
{
    sint32 idx_wndr, num_wndr;
    num_wndr = ai->m_wonderDB->GetNumWonders();

    double utility = 0.0;

    for (idx_wndr=0; idx_wndr<num_wndr; idx_wndr++) {
        utility =
            IsNewWonder(ai, c, sci_id, idx_wndr) +
            CanObsoleteHisWonder(ai, c, sci_id, idx_wndr) +
            CanObsoleteMyWonder(ai,  c, sci_id, idx_wndr);
    }

    return utility;
}









double ScienceAgent::IsEndGameTech(AiMain *ai, sint32 sci_id)
	{
	return (0.0) ;
	}

















double ScienceAgent::EvalAdvancement(AiMain *ai, sint32 sci_id, double *coef)
	{
	double	scienceValue ;

	scienceValue =
        UtilityGoverment(ai, coef, sci_id) +
        UtilityInstallation(ai, coef, sci_id) +
        UtilityUnit(ai, coef, sci_id) +
        UtilityBuilding(ai, coef, sci_id) +
        UtilityWonder(ai, coef, sci_id) +
		coef[COEFTYPE_TECH_END_GAME] * IsEndGameTech(ai, sci_id);

	return (scienceValue) ;
}











































































































sint32 ScienceAgent::FindMaxDepth(AiMain *ai, const sint32 idx_sci, sint32 *depth,
      const sint32 stack_limit)
{
    Assert(0 <= idx_sci);
    Assert(idx_sci < m_numAdvances);
    Assert(stack_limit < m_numAdvances);
    if (m_numAdvances <= stack_limit) {
        depth[idx_sci] = 0;
        return 0;
    }

    if (depth[idx_sci] == m_numAdvances) {
        sint32 idx_pre;
        sint32 num_pre = ai->m_science->GetNumPrerequisites(idx_sci);
        if (num_pre == 0)  {
				depth[idx_sci] = 1;
        } else {

            sint32 d_max = -1;
            sint32 d;
            sint32 i;
			for (i=0; i<num_pre; i++) {
				idx_pre = ai->m_science->GetPrereq(idx_sci, i);
                d = FindMaxDepth(ai, idx_pre, depth, stack_limit + 1);
                if (d_max < d) {
                    d_max = d;
                }
            }
            Assert(0 <= d_max);
            depth[idx_sci] = d_max+1;
        }
   }

   return depth[idx_sci];

}

void ScienceAgent::InitGraph(AiMain *ai, sint32 maxDepth, double *coef,
      double *valueList, sint32 *depthList, sint32 &max_sci)
{
    Assert(valueList);
    Assert(depthList);

    sint32 idx_sci;
	for (idx_sci=0; idx_sci<m_numAdvances; idx_sci++) {
        if (ai->m_science->HasAdvance(idx_sci))	{
            depthList[idx_sci] = 0;
        } else {
            depthList[idx_sci] = m_numAdvances;
        }
		valueList[idx_sci] = -1.0 ;
	}

    for (idx_sci=(m_numAdvances-1); 0 <= idx_sci; idx_sci--) {
        FindMaxDepth(ai, idx_sci, depthList, 0);
    }

    max_sci = -1;
    double val_max = -1000000.0;
	for (idx_sci=0; idx_sci<m_numAdvances; idx_sci++) {
        if  ((0 < depthList[idx_sci]) && (depthList[idx_sci] <= maxDepth)) {

            valueList[idx_sci] = EvalAdvancement(ai, idx_sci, coef);

            if (val_max < valueList[idx_sci]) {
                max_sci = idx_sci;
                val_max = valueList[idx_sci];
            }

        } else {
            valueList[idx_sci] = -1.0;
        }
    }
}
































































































































void ScienceAgent::FindBestPrereq(AiMain *ai, sint32 *depth, double *value,
   const sint32 idx_sci, sint32 &max_pre, double &pre_val)
{
    Assert(0 <= idx_sci);
    Assert(idx_sci < m_numAdvances);

    Assert(0 <= depth[idx_sci]);

    if (depth[idx_sci] == 0) {
        max_pre = idx_sci;
        pre_val = -1000000.0;
    } else if (depth[idx_sci] == 1) {
        max_pre = idx_sci;
        pre_val = value[idx_sci];
    } else {

        sint32 idx_pre;
        sint32 num_pre = ai->m_science->GetNumPrerequisites(idx_sci);
        Assert(num_pre != 0);

        max_pre = -1;
        pre_val = -10000000.0;
        double v;
        sint32 i;
        sint32 m;
		for (i=0; i<num_pre; i++) {
			idx_pre = ai->m_science->GetPrereq(idx_sci, i);
            FindBestPrereq(ai, depth, value, idx_pre, m, v);
            if (pre_val < v) {
                max_pre = m;
                pre_val = v;
            }
        }
        Assert(max_pre != -1);
    }
}

sint32 ScienceAgent::ResearchNext(AiMain *ai, sint32 maxDepth, double *coef)
{
	sint32	*depthList;
	double	*valueList;

	valueList = new double[m_numAdvances] ;
	depthList = new sint32[m_numAdvances] ;

    sint32 max_sci;
    InitGraph(ai, maxDepth, coef, valueList, depthList, max_sci);

    if (max_sci == -1) {
    	delete depthList ;
        depthList = NULL;
	    delete valueList ;
        valueList = NULL;
        return -1;
    }
    if (depthList[max_sci] == 1) {
    	delete depthList ;
        depthList = NULL;
	    delete valueList ;
        valueList = NULL;
        return max_sci;
    }

    sint32 max_pre;
    double pre_val;
    FindBestPrereq(ai, depthList, valueList, max_sci, max_pre, pre_val);

    Assert(!ai->m_science->HasAdvance(max_pre));

	delete depthList ;
    depthList = NULL;
	delete valueList ;
    valueList = NULL;
	return max_pre ;
}












void ScienceAgent::ResearchScience(AiMain *ai)
{
	sint32	sci_id ;


























































	sci_id = ai->m_planner->the_AIP.Choose_Next_Science();

    if (sci_id < 0) {
        m_current_science = -1;
    } else {
        Assert(sci_id < m_numAdvances);
        ai->m_science->SetResearching(sci_id) ;
        Assert(m_current_science != sci_id);
        m_current_science = sci_id;
    }
    m_must_learn_more_science = FALSE;

}

void ScienceAgent::RegisterLearnedScience(AiMain *ai, sint32 idx_sci,
    CAUSE_SCI cause, BOOL &is_new_unit_discovered)
{
    Assert(0 <= idx_sci);
    Assert(idx_sci < m_numAdvances);

   if ((0 < m_current_science) &&
       (m_current_science < m_numAdvances)) {

	  if (ai->m_science->HasAdvance(m_current_science)) {
		  m_must_learn_more_science = TRUE;
      }
   }else {
      m_must_learn_more_science = TRUE;
   }

   UpdateWhatIKnow(ai, idx_sci, is_new_unit_discovered);

   m_science_count++;
   if (IknowItAll()) {
       m_must_learn_more_science = FALSE;
   }
}

BOOL ScienceAgent::HasWaterSettle() const
{
    return m_has_water_settle;
}

BOOL ScienceAgent::HasWaterMove() const
{
    return m_has_water_move;
}

BOOL ScienceAgent::HasSpaceSettle() const
{
    return m_has_space_settle;
}








BOOL ScienceAgent::HasSpaceMove() const
{
    return m_has_space_move;
}

BOOL ScienceAgent::CanRailLaunch() const
{
    return m_can_rail_launch;
}

BOOL ScienceAgent::GetBestFreight(sint32 &idx_freight) const
{
    idx_freight = m_best_freight;
    if (-1 == m_best_freight) {
        return FALSE;
    } else {
        return TRUE;
    }
}

BOOL ScienceAgent::GetBestLandAttack(sint32 &value) const
{
		return ((value = m_best_land_attack) > 0);
}

BOOL ScienceAgent::GetBestWaterAttack(sint32 &value) const
{
		return ((value = m_best_water_attack) > 0);
}

BOOL ScienceAgent::GetBestAirAttack(sint32 &value) const
{
		return ((value = m_best_air_attack) > 0);
}

BOOL ScienceAgent::GetBestSpaceAttack(sint32 &value) const
{
		return ((value = m_best_space_attack) > 0);
}

BOOL ScienceAgent::GetBestLandDefense(sint32 &value) const
{
		return ((value = m_best_land_defense) > 0);
}

BOOL ScienceAgent::GetBestWaterDefense(sint32 &value) const
{
		return ((value = m_best_water_defense) > 0);
}

BOOL ScienceAgent::GetBestAirDefense(sint32 &value) const
{
		return ((value = m_best_air_defense) > 0);
}

BOOL ScienceAgent::GetBestSpaceDefense(sint32 &value) const
{
		return ((value = m_best_space_defense) > 0);
}

BOOL ScienceAgent::GetBestLandMove(sint32 &value) const
{
		return ((value = m_best_land_move) > 0);
}

BOOL ScienceAgent::GetBestWaterMove(sint32 &value) const
{
		return ((value = m_best_water_move) > 0);
}

BOOL ScienceAgent::GetBestAirMove(sint32 &value) const
{
		return ((value = m_best_air_move) > 0);
}

BOOL ScienceAgent::GetBestSpaceMove(sint32 &value) const
{
		return ((value = m_best_space_move) > 0);
}

BOOL ScienceAgent::GetBestConvertChance(double &value) const
{
		return ((value = m_best_convert_chance) > 0);
}

BOOL ScienceAgent::GetBestHP(sint32 &value) const
{
		return ((value = m_best_hp) > 0);
}

BOOL ScienceAgent::GetBestFP(sint32 &value) const
{
		return ((value = m_best_fp) > 0);
}

BOOL ScienceAgent::GetBestRoadType(sint32 &value) const
{
    return (value = m_best_road_type) != -1;
}

BOOL ScienceAgent::GetBestFarmType(sint32 &value) const
{
    return (value = m_best_farm_type) != -1;
}

BOOL ScienceAgent::GetBestQuarryType(sint32 &value) const
{
    return (value = m_best_quarry_type) != -1;
}

void ScienceAgent::UpdateWhatIKnow(AiMain *ai, const sint32 sci_idx, BOOL &is_new_unit_discovered)
{

    sint32 unit_idx;
    sint32 enabling;

    is_new_unit_discovered = FALSE;

    for (unit_idx=0; unit_idx < m_numUnitTypes; unit_idx++) {

		if (ai->m_unitDB->IsUnitObsolete(unit_idx))
		{
			continue;
		}

        enabling = ai->m_unitDB->EnablingDiscovery(unit_idx);
        if (enabling) {
            is_new_unit_discovered = TRUE;
        }

        if ((sci_idx == enabling) || (enabling == -1)) {

            if (ai->m_unitDB->IsWormholeProbe(unit_idx)) {
                m_can_build_wormhole_probe = TRUE;
            }

            if (ai->m_unitDB->CanSettleWater(unit_idx)) {
                m_has_water_settle = TRUE;
            }

            if (ai->m_unitDB->CanMoveWater(unit_idx) ||
                ai->m_unitDB->CanMoveShallowWater(unit_idx)) {
                m_has_water_move = TRUE;
            }

            if (ai->m_unitDB->CanSettleSpace(unit_idx)) {
                m_has_space_settle = TRUE;
            }

            if (ai->m_unitDB->CanMoveSpace(unit_idx)) {
                m_has_space_move = TRUE;
            }

            if (ai->m_unitDB->GetIsTrader(unit_idx)) {
                if (-1 == m_best_freight) {
                    m_best_freight = unit_idx;
                } else {
                    double old_mp = ai->m_unitDB->UnitTypeMovementPoints(m_best_freight);
                    double mp = ai->m_unitDB->UnitTypeMovementPoints(unit_idx);
                    if (old_mp < mp) {
                        m_best_freight = unit_idx;
                    }
                }
            }

			if (!ai->m_unitDB->GetIsTrader(unit_idx) &&
				!ai->m_unitDB->IsSingleUse(unit_idx))
			{

				double move;
				move = ai->m_unitDB->UnitTypeMovementPoints(unit_idx);
				if ( move > m_best_land_move && ai->m_unitDB->GetMovementTypeLand(unit_idx))
					m_best_land_move = (sint32) move;

				if ( move > m_best_water_move && ai->m_unitDB->GetMovementTypeSea(unit_idx))
					m_best_water_move = (sint32) move;

				if ( move > m_best_air_move && ai->m_unitDB->GetMovementTypeAir(unit_idx))
					m_best_air_move = (sint32) move;

				if ( move > m_best_space_move && ai->m_unitDB->GetMovementTypeSpace(unit_idx))
					m_best_space_move = (sint32) move;

				sint32 attack;
				attack = ai->m_unitDB->GetAttack(unit_idx);
				if (ai->m_unitDB->CanMoveLand(unit_idx) &&
					attack > m_best_land_attack)
					m_best_land_attack = attack;

				if (ai->m_unitDB->CanMoveWater(unit_idx) &&
					attack > m_best_water_attack)
					m_best_water_attack = attack;

				if (ai->m_unitDB->CanMoveAir(unit_idx) &&
					attack > m_best_air_attack)
					m_best_air_attack = attack;

				if (ai->m_unitDB->CanMoveSpace(unit_idx) &&
					attack > m_best_space_attack)
					m_best_space_attack = attack;

				sint32 defense;
				defense = ai->m_unitDB->GetDefense(unit_idx);
				if (ai->m_unitDB->CanMoveLand(unit_idx) &&
					defense > m_best_land_defense)
					m_best_land_defense = defense;

				if (ai->m_unitDB->CanMoveWater(unit_idx) &&
					defense > m_best_water_defense)
					m_best_water_defense = defense;

				if (ai->m_unitDB->CanMoveAir(unit_idx) &&
					defense > m_best_air_defense)
					m_best_air_defense = defense;

				if (ai->m_unitDB->CanMoveSpace(unit_idx) &&
					defense > m_best_space_defense)
					m_best_space_defense = defense;

				double convert;
				convert = ai->m_unitDB->GetConvertCityChance(unit_idx);
				if ( convert > m_best_convert_chance )
					m_best_convert_chance = convert;

				sint32 hp;
				hp = ai->m_unitDB->GetHP(unit_idx);
				if ( hp > m_best_hp) {
					m_best_hp = hp;
				}

				sint32 fp;
				fp = ai->m_unitDB->GetFirepower(unit_idx);
				if (fp > m_best_fp )
					m_best_fp = fp;
			}

            if (ai->m_unitDB->CanLaunchNuke(unit_idx)) m_can_missile_nuke = TRUE;
            if (ai->m_unitDB->CanPlantNuke(unit_idx)) m_can_plant_nuke = TRUE;
            if (ai->m_unitDB->CanCreateParks(unit_idx)) m_can_make_park = TRUE;
            if (ai->m_unitDB->CanConductUndergroundRailway(unit_idx)) m_can_underground_railway = TRUE;
            if (ai->m_unitDB->CanConvertCity(unit_idx)) m_can_convert_city = TRUE;
            if (ai->m_unitDB->CanEstablishEmbassy(unit_idx)) m_can_establish_embassy = TRUE;
            if (ai->m_unitDB->CanCreateFranchise(unit_idx)) m_can_franchise = TRUE;
            if (ai->m_unitDB->CanAssasinateRuler(unit_idx)) m_can_assasinate_ruler = TRUE;
            if (ai->m_unitDB->CanStealTechnology(unit_idx)) m_can_steal_technology = TRUE;
            if (ai->m_unitDB->CanInjoin(unit_idx)) m_can_injoin = TRUE;
            if (ai->m_unitDB->CanInciteRevolution(unit_idx)) m_can_incite_revolution = TRUE;
            if (ai->m_unitDB->CanCauseUnhappiness(unit_idx)) m_can_cause_unhappiness = TRUE;

            if (ai->m_unitDB->CanBioterrorCity(unit_idx)) m_can_cause_bioterror = TRUE;
            if (ai->m_unitDB->CanNanoattackCity(unit_idx)) m_can_cause_nanoattack = TRUE;
			if (ai->m_unitDB->CanSue(unit_idx)) m_can_sue = TRUE;

        }
    }

    sint32 building_idx;
    sint32 building_num = ai->m_nBuildings;
    for (building_idx=0; building_idx<building_num; building_idx++) {
        enabling = ai->m_buildingDB->EnablingDiscovery(building_idx);
        if ((enabling == sci_idx) || (enabling == -1)) {
            if (ai->m_buildingDB->IsSpaceLauncher(building_idx)) {
                m_can_rail_launch = TRUE;
                m_has_space_move = FALSE;
            }
        }
    }

    sint32 inst_idx;
    sint32 inst_num;

    inst_num = ai->m_installationDB->GetNumInstallations ();
    double road_value ;
    double min_road_value = 100.0;
    sint32 min_road_idx = -1;
    double food_value;
    double max_food_value = 0;
    sint32 max_food_idx = -1;
    double prod_value;
    double max_prod_value = 0;
    sint32 max_prod_idx = -1;

    min_road_idx = m_best_road_type;
    max_food_idx = m_best_farm_type;
    max_prod_idx = m_best_quarry_type;

    for ( inst_idx=0; inst_idx < inst_num; inst_idx++) {

        enabling = ai->m_installationDB->EnablingDiscovery(inst_idx);

        if ((sci_idx == enabling) || (-1 == enabling)) {

            if (ai->m_installationDB->IsUnderseaTunnel(inst_idx)) {
                m_has_undersea_tunnel = TRUE;
            }

            road_value = ai->m_installationDB->GetInstMovement(inst_idx);
            if (road_value < min_road_value) {
                min_road_value = road_value;
                min_road_idx = inst_idx;
            }

            food_value = ai->m_installationDB->GetFoodBonus(inst_idx, TERRAIN_GRASSLAND);
            if (max_food_value < food_value) {
                max_food_value = food_value;
                max_food_idx = inst_idx;
            }

            prod_value = ai->m_installationDB->GetProductionBonus(inst_idx, TERRAIN_HILL);
            if (max_prod_value < prod_value) {
                max_prod_value = prod_value;
                max_prod_idx = inst_idx;
            }
        }
    }

    m_best_road_type = min_road_idx;
    m_best_farm_type = max_food_idx;
    m_best_quarry_type = max_prod_idx;
}

BOOL ScienceAgent::HasUnderseaTunnel()
{
    return m_has_undersea_tunnel;
}

BOOL ScienceAgent::CanMissileNuke() const
{
    return m_can_missile_nuke;
}

BOOL ScienceAgent::CanPlantNuke() const
{
    return m_can_plant_nuke;
}

BOOL ScienceAgent::CanMakePark() const
{
    return m_can_make_park;
}

BOOL ScienceAgent::CanUndergroundRailway() const
{
    return m_can_underground_railway;
}

BOOL ScienceAgent::CanConvertCity() const
{
    return m_can_convert_city;
}

BOOL ScienceAgent::CanEstablishEmbassy() const
{
    return m_can_establish_embassy;
}

BOOL ScienceAgent::CanFranchise() const
{
    return m_can_franchise;
}

BOOL ScienceAgent::CanAssasinateRuler() const
{
    return m_can_assasinate_ruler;
}

BOOL ScienceAgent::CanStealTechnology() const
{
    return m_can_steal_technology;
}

BOOL ScienceAgent::CanInjoin() const
{
    return m_can_injoin;
}

BOOL ScienceAgent::CanInciteRevolution() const
{
    return m_can_incite_revolution;
}

BOOL ScienceAgent::CanCauseUnhappiness() const
{
    return m_can_cause_unhappiness;
}

BOOL ScienceAgent::CanBioterrorCity() const
{
    return m_can_cause_bioterror;
}

BOOL ScienceAgent::CanNanoattackCity() const
{
    return m_can_cause_nanoattack;
}

BOOL ScienceAgent::CanSue() const
{
    return m_can_sue;
}

BOOL ScienceAgent::CanBuildWormholeProbe() const
{
    return m_can_build_wormhole_probe;
}

float ScienceAgent::ComputeMaxMoveCost(AiMain *ai) {

	m_max_move_cost = 0.0;
	sint32 num_terrain_types = ai->m_terrainDB->GetNumTerrainTypes();
	for (sint8 terrain_type=0; terrain_type < num_terrain_types; terrain_type++) {
		if (ai->m_terrainDB->GetMoveCost(terrain_type) > m_max_move_cost) {
			m_max_move_cost = (float)ai->m_terrainDB->GetMoveCost(terrain_type);
		}
	}

	return m_max_move_cost;
}
