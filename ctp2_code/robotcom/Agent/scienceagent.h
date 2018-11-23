#pragma once
#ifndef __SCIENCE_AGENT_H__
#define __SCIENCE_AGENT_H__ 1

#include "Agent.h"

class AiMain;
class BSetID;

enum BQ_CATEGORY;
enum GOAL_TYPE;
enum CAUSE_SCI;

enum COEFTYPE
	{
	COEFTYPE_UNIT_MILITARY,
	COEFTYPE_UNIT_ABILITY,
	COEFTYPE_UNIT_MOVE_AIR,
	COEFTYPE_UNIT_MOVE_WATER,
	COEFTYPE_UNIT_MOVE_SPACE,
	COEFTYPE_UNIT_SETTLE_WATER,
	COEFTYPE_UNIT_SETTLE_SPACE,
	COEFTYPE_UNIT_NUKE,
	COEFTYPE_UNIT_CARAVAN,

	COEFTYPE_INSTALLATION_MOVE,
	COEFTYPE_INSTALLATION_DEFENSE,
	COEFTYPE_INSTALLATION_FOOD,
	COEFTYPE_INSTALLATION_PRODUCTION,
	COEFTYPE_INSTALLATION_VISION,
	COEFTYPE_BUILDING_DEFENSE,
	COEFTYPE_BUILDING_FOOD,
	COEFTYPE_BUILDING_PRODUCTION,
	COEFTYPE_BUILDING_SCIENCE,
	COEFTYPE_BUILDING_GOLD,
	COEFTYPE_BUILDING_HAPPY,

	COEFTYPE_WONDER_IS_NEW,
	COEFTYPE_WONDER_OBSOLETE_HIS,
	COEFTYPE_WONDER_OBSOLETE_MINE,

	COEFTYPE_TECH_END_GAME,

	COEFTYPE_GOV_WORKDAY,
	COEFTYPE_GOV_WAGES,
	COEFTYPE_GOV_RATIONS,
	COEFTYPE_GOV_EMPIRE_SIZE,
	COEFTYPE_GOV_MARTIAL_LAW,
	COEFTYPE_GOV_COMBAT_MORAL,
    COEFTYPE_GOV_UNIT_DIST,
	COEFTYPE_GOV_CRIME,
    COEFTYPE_GOV_GOLD,
	COEFTYPE_GOV_KNOWLEDGE,
	COEFTYPE_GOV_POLLUTION,
	COEFTYPE_GOV_POLLUTION_UNHAPPY,
	COEFTYPE_GOV_READINESS,

	COEFTYPE_MAX
};

struct MapPointData;
class AiMain;
class ScienceAgent {

    BOOL m_has_water_settle;
    BOOL m_has_water_move;
    BOOL m_has_space_settle;
    BOOL m_has_space_move;
    sint32 m_best_freight;
    BOOL m_can_rail_launch;

	sint32 m_best_hp;
	sint32 m_best_fp;
	sint32 m_best_land_attack;
	sint32 m_best_water_attack;
	sint32 m_best_air_attack;
	sint32 m_best_space_attack;

	sint32 m_best_land_defense;
	sint32 m_best_water_defense;
	sint32 m_best_air_defense;
	sint32 m_best_space_defense;

	sint32 m_best_land_move;
	sint32 m_best_water_move;
	sint32 m_best_air_move;
	sint32 m_best_space_move;

	double m_best_convert_chance;

    sint32 m_best_road_type;
    sint32 m_best_farm_type;
    sint32 m_best_quarry_type;

    sint32 m_science_count;
    BOOL m_has_undersea_tunnel;

    BOOL m_can_missile_nuke;
    BOOL m_can_plant_nuke;
    BOOL m_can_make_park;
    BOOL m_can_underground_railway;
    BOOL m_can_convert_city;
    BOOL m_can_establish_embassy;
    BOOL m_can_franchise;
    BOOL m_can_assasinate_ruler;
    BOOL m_can_steal_technology;
    BOOL m_can_injoin;
    BOOL m_can_incite_revolution;
    BOOL m_can_cause_unhappiness;
    BOOL m_can_cause_bioterror;
    BOOL m_can_cause_nanoattack;
	BOOL m_can_sue;
    BOOL m_can_build_wormhole_probe;

	float m_max_move_cost;

protected:
	sint32	m_numAdvances ;
	sint32	m_numUnitTypes ;
    BOOL m_must_learn_more_science;
    sint32 m_current_science;

protected:
	double	*m_unitMilitaryWorth ;

public:

    ScienceAgent();
	ScienceAgent(AiMain *ai);
    ScienceAgent(AiMain *ai, CivArchive &archive);
    void Init(AiMain *ai);
    ~ScienceAgent();

    void Serialize(CivArchive &archive);
    void BeginTurn ();

    sint32 FindMaxDepth(AiMain *ai, const sint32 idx_sci, sint32 *depth,
       const sint32 stack_limit);

    void InitGraph(AiMain *ai, sint32 maxDepth, double *coef,
      double *valueList, sint32 *depthList, sint32 &max_sci);

    void FindBestPrereq(AiMain *ai, sint32 *depthList, double *valueList,
        const sint32 idx_sci, sint32 &max_pre, double &pre_val);

    sint32 ResearchNext(AiMain *ai, sint32 maxDepth, double *coef);








    void ResearchScience(AiMain *ai) ;

    void RegisterLearnedScience(AiMain *ai, sint32 idx_sci, CAUSE_SCI cause, BOOL &is_new_unit_discovered);
    BOOL MustLearnMoreScience() const { return m_must_learn_more_science; }


    double UnhappyGovWorkday(AiMain *ai, double *c, const sint32 gov);
    double UnhappyGovWages(AiMain *ai, double *c, const sint32 gov);
    double UnhappyGovRations(AiMain *ai, double *c, const sint32 gov);
    double UnhappyGovEmpSize(AiMain *ai, double *c, const sint32 gov);
    double HappyMartialLaw(AiMain *ai, double *c, const sint32 gov);
    double UnhappyUnitDist(AiMain *ai, double *c, const sint32 gov);
    double UnhappyCombatMoral(AiMain *ai, double *c, const sint32 gov);
    double UnhappyPolluion(AiMain *ai, double *c, const sint32 gov);

    double GovCrime(AiMain *ai, double *c,
        const sint32 gov, const double happy);

    double GovGold(AiMain *ai, double *c, const sint32 gov, const double happy, const double crime);
    double UtilityGovProduction(AiMain *ai, double *c, const sint32 gov, const double crime);
    double UtilityGovKnowlege(AiMain *ai, double *c, const sint32 gov, const double gold);
    double UtilityGovReadiness(AiMain *ai, double *c,  const sint32 gov, const double crime);

    double UtilityGoverment(AiMain *ai, double *c, sint32 sci_id);

    double UtilityInstProd(AiMain *ai, double *c, const sint32 inst);
    double UtilityInstFood(AiMain *ai, double *c, const sint32 inst);
    double UtilityInstDefense(AiMain *ai, double *c, const sint32 inst);
    double UtilityInstVision(AiMain *ai, double *c, const sint32 inst);
    double UtilityInstMovement(AiMain *ai, double *c, const sint32 inst);

    double UtilityInstallation(AiMain *ai, double *c, const sint32 sci_id);

    double GetUnitMilitaryWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_unit);
	double GetUnitAbilityWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_unit);
	double GetUnitMoveAirWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_unit);
	double GetUnitMoveWaterWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_unit);
	double GetUnitMoveSpaceWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_unit);
	double GetUnitSettleSpaceWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_unit);
	double GetUnitSettleWaterWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_unit) ;
	double GetUnitNukeWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_unit);
	double GetUnitCaravanWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_unit);

    double UtilityUnit(AiMain *ai, double *c, const sint32 sci_id);

	double GetBuildingDefenseWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_blg);
	double GetBuildingFoodWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_blg);
	double GetBuildingProductionWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_blg);
	double GetBuildingScienceWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_blg);
	double GetBuildingGoldWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_blg);
	double GetBuildingHappyWorth(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_blg);

    double UtilityBuilding(AiMain *ai, double *c, const sint32 sci_id);

    double IsNewWonder(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_wonder);
    double CanObsoleteHisWonder(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_wonder);
    double CanObsoleteMyWonder(AiMain *ai, double *c, const sint32 sci_id, const sint32 idx_wndr);

    double UtilityWonder(AiMain *ai, double *c, const sint32 sci_id);

	double IsEndGameTech(AiMain *ai, sint32 sci_id) ;

	double EvalAdvancement(AiMain *ai, sint32 sci_id, double coef[COEFTYPE_MAX]) ;

    BOOL HasWaterSettle() const;
    BOOL HasWaterMove() const;
    BOOL HasSpaceSettle() const;
    BOOL HasSpaceMove() const;
    BOOL GetBestFreight(sint32 &idx_freight)const;
    BOOL CanRailLaunch() const;

	BOOL GetBestLandAttack(sint32 &value) const;
	BOOL GetBestWaterAttack(sint32 &value) const;
	BOOL GetBestAirAttack(sint32 &value) const;
	BOOL GetBestSpaceAttack(sint32 &value) const;
	BOOL GetBestLandDefense(sint32 &value) const;
	BOOL GetBestWaterDefense(sint32 &value) const;
	BOOL GetBestAirDefense(sint32 &value) const;
	BOOL GetBestSpaceDefense(sint32 &value) const;
	BOOL GetBestLandMove(sint32 &value) const;
	BOOL GetBestWaterMove(sint32 &value) const;
	BOOL GetBestAirMove(sint32 &value) const;
	BOOL GetBestSpaceMove(sint32 &value) const;
	BOOL GetBestHP(sint32 &value) const;
	BOOL GetBestFP(sint32 &value) const;

    BOOL GetBestRoadType(sint32 &road) const;
    BOOL GetBestFarmType(sint32 &road) const;
    BOOL GetBestQuarryType(sint32 &road) const;

	BOOL GetBestConvertChance(double &value) const;

    BOOL IknowItAll() const { return m_numAdvances <= m_science_count; }
    void UpdateWhatIKnow(AiMain *ai, const sint32 sci_idx, BOOL &is_new_unit_discovered);
    BOOL HasUnderseaTunnel();

	sint32 HowManyIKnow() const { return m_science_count; }

    BOOL CanMissileNuke() const;
    BOOL CanPlantNuke() const;
    BOOL CanMakePark() const;
    BOOL CanUndergroundRailway() const;
    BOOL CanConvertCity() const;
    BOOL CanEstablishEmbassy() const;
    BOOL CanFranchise() const;
    BOOL CanAssasinateRuler() const;
    BOOL CanStealTechnology() const;
    BOOL CanInjoin() const;
    BOOL CanInciteRevolution() const;
    BOOL CanCauseUnhappiness() const;
    BOOL CanBioterrorCity() const;
    BOOL CanNanoattackCity() const;
    BOOL CanSue() const;
    BOOL CanBuildWormholeProbe() const;

	float ComputeMaxMoveCost(AiMain *ai);

};

#endif __SCIENCE_AGENT_H__
