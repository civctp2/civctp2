
#pragma once
#ifndef __CITY_AGENT_H__
#define __CITY_AGENT_H__ 1

#include "Agent.h"
#include "MapPoint.h"


#include "common.h"
#include "linked_list.h"


#include "gstypes.h"

class AiMain; 
class BSetID; 
class AiBuildQueue; 
class ArmyAgent;
class ForeignAgent; 
class GoalDefense;
template <class T> class PointerList;
interface IC3TradeOffer;

#define k_SAFETY_ON_THE_FRONT 0x00000001

#define COST_TOO_BIG 1000000.0
#define CITY_TILE_SIZE				21								



enum GOAL_TYPE; 
enum INST_TILE_STATE;
enum PRODUCTION_TYPE;


enum POPTYPE
	{
	POPTYPE_FIRST,
	POPTYPE_WORKER=POPTYPE_FIRST,
	POPTYPE_MUSICIAN,
	POPTYPE_SCIENTIST,
	POPTYPE_GRUNT,
	POPTYPE_SLAVE,
	POPTYPE_BANKER,
	POPTYPE_LAST=POPTYPE_BANKER,
	POPTYPE_MAX,
	} ;




















class CityAgent; 
class RoadDest; 
class RoadDest { 
public:
    uint32 m_id; 
    CityAgent *m_dest; 
    sint32 m_road_type; 
    RoadDest *m_next;
   
    RoadDest(CityAgent *da, const sint32 rt); 
    RoadDest(CivArchive &archive); 
    void Serialize(CivArchive &archive); 
    void HookUp(AiMain *ai);

};

class CityAgentFlat { 

protected:
    sint32 m_stored_production; 
    sint32 m_gross_production; 
    sint32 m_net_production;
    sint32 m_projected_gross_production; 
    sint32 m_projected_net_production; 

    sint32 m_stored_food;
    sint32 m_gross_food; 
    sint32 m_net_food; 
    sint32 m_projected_food; 
    sint32 m_accumulated_growth;
    sint32 m_accumulated_threshold;

    sint32 m_gross_gold; 
    sint32 m_net_gold; 
    sint32 m_projected_gross_gold; 
    sint32 m_projected_net_gold; 

    sint32 m_total_pollution; 

    sint32 m_gross_science; 

    uint32 m_status_bits; 

    BOOL m_is_materials_tax_exempt;

    double m_delta_threat_strength;
    double m_threat_strength; 
    double temp_goal_relief_def_str;

    sint32 m_defenders_nUnits; 
    double m_defenders_ranged; 
    double m_defenders_attack_strength; 
    double m_defenders_defense_strength; 


    sint32 m_total_citizens;
    sint32 m_num_slaves; 
    sint32 m_num_field_workers; 
    sint32 m_num_musicians; 
    sint32 m_old_happiness; 
    double m_old_crime; 
    double m_cop;
    sint32 m_total_field_prod; 
    sint32 m_total_field_food; 
    double m_ave_prod_tile; 
    double m_ave_food_tile; 
	sint32 m_min_slave_garrison;

	

    sint32 m_pop_needs_changing;

	bool m_defend_me_goal_set;		  
    sint32 m_defend_me_goal_id; 
    BOOL m_has_space_launcher; 
    BOOL m_has_airport; 
    BOOL m_has_granary; 
	sint32 m_retreatGoalID;

    double m_pop_prod; 
    double m_pop_food; 
    double m_pop_gold;

    BOOL m_i_need_forced_tile_improvements;
	
	ArmyAgent *m_bestUnitForGoal[GOAL_TYPE_MAX];

	sint32 m_turnCaptured;
	
	
	sint32 m_queueItemType;
	sint32 m_queueItemCat;
	sint32 m_buildCapitalization;

public:
	double best_build_bid;
	GOAL_TYPE current_goal_type;

    CityAgentFlat(); 
    void Serialize (CivArchive &archive); 

} ;

struct MapPointData; 
class BuildingAgent; 
class ArmyGoal;
class GoalBuilding;

class GoalFreight; 
class Pirate_Marker; 
template <class DATA_TYPE> class list_array;

class CityAgentPtr { 
protected:
    MapPointData *m_pos; 
	
    
    BuildingAgent *m_building;
	sint16 m_garrisonCount; 

    
    double *m_production_bonus;

    


  
    

    





	ArmyAgent *m_troops[k_MAX_ARMY_SIZE]; 

	
	
	
	
	

	list_array<ArmyAgent *> * m_list_of_potential_units;

    RoadDest *m_road_list; 

	PointerList<IC3TradeOffer> *m_tradeOffers;

    Pirate_Marker *m_pirate_head; 

    friend class GoalDefense; 

public:

    CityAgentPtr(); 
    ~CityAgentPtr(); 
    void Serialize(CivArchive &archive); 
    void Store(CivArchive &archive); 
    void Load(CivArchive &archive);

};

enum REGISTER_TYPE { 
    REGISTER_TYPE_SCIENCE,
    REGISTER_TYPE_ADD_CITY, 
    REGISTER_TYPE_BLG
}; 

class CityVertex; 
class CityAgent : public Agent, public CityAgentFlat, public CityAgentPtr { 

     friend CityVertex;

public:
    
    CityAgent(); 
	CityAgent(AiMain *ai, BSetID id, MapPointData *pos);
    CityAgent(IC3CivArchive *archive);

    void ZeroInit(); 
    void MemInit(AiMain *ai, MapPointData *pos);
    ~CityAgent();
    
    void Serialize(IC3CivArchive *archive);
    BOOL Validate(AiMain *ai); 

    void BeginTurn (AiMain *ai);

    void SetPos(AiMain *ai); 
    void GetPos(MapPointData &pos) const; 
    void GetXYPos(AiMain *ai, MapPointData &pos); 


    sint32 GetTotalNumCitizens() { return m_total_citizens; } 
    sint32 GetNetFood() const { return m_net_food; } 
    sint32 GetProjectedFood() const { return m_projected_food; }

    sint32 GetGrossProduction() const { return m_gross_production; } 
    sint32 GetGrossGrowth() const { return m_accumulated_growth; } 
    sint32 GetGrossGold() const { return m_gross_gold; } 
    sint32 GetGrossScience() const { return m_gross_science; } 
    sint32 GetHappiness() const { return m_old_happiness; } 

    sint32 GetNetProduction() const { return m_net_production; }
    sint32 GetProjectedNetProduction() const { return m_projected_net_production; } 

    double GetTotalPollution() const { return m_total_pollution; } 
    
    sint32 GetQueueLen() const;
    void CalcEstimatedCityProduction();
    sint32 EstimateRoundsToFinished(AiMain *ai, BSetID m_id); 
    sint32 EstimateRoundsToEmptyQueue(AiMain *ai); 
    sint32 EstimateRoundsToBuildUnit(AiMain *ai,  sint32 unit_type);
	sint32 EstimateRoundsToBuildBuilding(AiMain *ai,  sint32 building_type);
	sint32 EstimateRoundsToBuildWonder(AiMain *ai,  sint32 wonder_type);
	sint32 EstimateRoundsToBuildEndGameObject(AiMain *ai, sint32 object_type);
    BOOL HasLotOfStoredProduction();

	
    double CityAgent::EstimatedPercentFinished(AiMain *ai);
	BOOL CityAgent::FrontIsImprovement();
	sint32 CityAgent::TurnsToFinishFront();

    void HookRoads(AiMain *ai);


    void EnqueueArmy(AiMain *ai, const sint32 unit_idx, const double utility); 
    void RegisterBuiltArmy(AiMain *ai, BSetID &id); 
    void RegisterBuiltFreight(AiMain *ai);
    void RegisterPiracy(const sint32 other_owner, const uint32 other_city_id, const sint32 when_priated);
    Pirate_Marker* FindPiracyMarker(const sint32 other_owner, const uint32 other_city_id);
    BOOL PiracyRecently(AiMain *ai);

    double EstimateCostToGiveUpCurrentBuild(AiMain *ai, GOAL_TYPE goal);  

	
	
	
	
	sint32 What_Army_Am_I_Building(AiMain *ai);

    

    void UpdateThreat(AiMain *ai);
    void ClearDeltaThreat();
    void AddToDeltaThreat(AiMain *ai, ForeignAgent *him);
    void AddToThreat(); 
    void GetThreatGradient(MapPointData &pos, double &value) const;
	double GetDefenseRequired(AiMain *ai);

    double GetThreatStrength() const { return m_threat_strength; }
	double GetDefendersDefense() const { return m_defenders_defense_strength;}
	double GetDefendersAttack() const { return m_defenders_attack_strength;}
    sint32 GetNumDefenders() const { return m_defenders_nUnits; } 
    sint32 GetNumUnitsInCity(AiMain *ai) const; 
	sint32 GetMinSlaveGarrison() const { return m_min_slave_garrison;}
	sint16 GetGarrisonCount() const { return m_garrisonCount; }
	void RemoveFromGarrison(sint16 count);
   
	void ResetDefenseStrength(const sint32 nu,
							  const double ranged,
							  const double as, const double ds);
    
    
    double EstimateUtility(AiMain *ai, ArmyGoal *goal_obj);



    BOOL IsGoalSatisfied(GOAL_TYPE goal);
	void Setup_Defense_Goal(AiMain *ai);
	void Remove_Defense_Goal(AiMain *ai);
	void Setup_Retreat_Goal(AiMain *ai);
	void Remove_Retreat_Goal(AiMain *ai);

    void RemoveCityReferance(CityAgent *remove_me);


    
    void InsertBuildingGoals(AiMain *ai);
    void RegisterCreateBuilding(AiMain *ai,  sint32 building_type);     
	void RegisterNewGovernment(AiMain *ai, sint32 government_type);
    void RegisterLostBuilding(AiMain *ai,  sint32 building_type); 
    void RegisterObsoleteBuild(AiMain *ai, sint32 cat, sint32 type);
    void RegisterSetBuildFront(sint32 cat, sint32 type);

    void RegisterKillRoute(const PLAYER_INDEX dest_owner, const uint32 dest_id, const sint32 resource); 

    void RegisterNewRoute(const PLAYER_INDEX dest_owner, const uint32 dest_id, const sint32 resource);

	void EventWonBuildCompition(AiMain *ai,  ArmyGoal *goal_obj, 
        const double utility);

    void EnqueueBuildBuilding(AiMain * ai, const sint32 blg_type, 
               const double utility);
    
    Goal_Result EnqueueBuildWonder(AiMain * ai, const sint32 idx_wonder, 
               const double utility);

    Goal_Result EnqueueBuildEndGameObject(AiMain * ai, const sint32 idx_object, 
              const double utility);

    Goal_Result BuildCapitalization(AiMain * ai);

    void EnqueueBuildFreight(AiMain * ai, 
               const double utility);

#include "CityAgentBlg.h"

    
    sint32 PretestContructBuilding(AiMain *ai, sint32 building_type);
	BOOL PretestBuildingBuildTime(AiMain *ai, sint32 building_type);
	sint32 PretestContructUnit(AiMain *ai, sint32 building_type);
	sint32 PretestContructWonder(AiMain *ai, sint32 building_type);
	BOOL PretestConstructEndGameObject(AiMain *ai, sint32 object_type);
	BOOL PretestConstructCapitalization(AiMain *ai);
     
	
    BOOL CanBuildUnit(AiMain *ai, sint32 unit_idx);
	void InsertPotentialArmy(AiMain *ai, ArmyAgent * aa);
    BOOL IsAPotentialUnit(const sint32 unit_idx);
    void InsertNewPotentialUnits(AiMain *ai, const REGISTER_TYPE r, 
        const sint32 item_type);
    void HookPotentialArmies();
	double CityAgent::FindBestBuildUtility(	AiMain *ai, 
											ArmyGoal *goal_obj,
											ArmyAgent *&best_agent);

	
	
	
	
	
	
	
	
	
	
	double Find_Best_Utility_Agent_We_Can_Build
	(
        AiMain *ai, 
		ArmyGoal *goal_obj,				
        ArmyAgent *&best_agent
	);


	void Set_Defend_Me_Status(bool new_defense_status);

	bool Get_Defend_Me_Status();

	 

    BOOL CanBuildInstallation(const sint32 inst_idx, MapPointData &w);

    double EstimateInstallationFarm(AiMain *ai, const sint32 inst_idx, 
        MapPointData &w, const sint32 idx_city, const sint32 median, 
        INST_TILE_STATE &tile_state);
    double EstimateInstallationQuarry(AiMain *ai, const sint32 inst_idx, 
        MapPointData &w, const sint32 idx_city, const sint32 median, 
        INST_TILE_STATE &tile_state);
    double EstimateTerainTransform(
        AiMain *ai,
        const sint32 inst_idx, 
        MapPointData &w, 
        const sint32 idx_city, 
        const sint32 median,  
        INST_TILE_STATE &tile_state, 
        sint32 &new_terrain, 
        BOOL can_be_grass, 
        BOOL can_be_hills
        ); 

    double EstimateInstallationGuardtower(AiMain *ai, const sint32 inst_idx, 
        MapPointData &w, const sint32 idx_city, const sint32 median, 
        INST_TILE_STATE &tile_state);
    double EstimateInstallationAirfield(AiMain *ai, const sint32 inst_idx, 
        MapPointData &w, const sint32 idx_city, const sint32 median, 
        INST_TILE_STATE &tile_state);
    double EstimateInstallationFort(AiMain *ai, const sint32 inst_idx, 
        MapPointData &w, const sint32 idx_city, const sint32 median, 
        INST_TILE_STATE &tile_state);
    double EstimateInstallationLandMine(AiMain *ai, const sint32 inst_idx, 
        MapPointData &w, const sint32 idx_city, const sint32 median, 
        INST_TILE_STATE &tile_state);

    double EstimateInstallationBenefit(AiMain *ai, const sint32 inst_idx, 
        MapPointData &w,
        const sint32 idx_city, const sint32 median, 
        INST_TILE_STATE &tile_state, 
        const double old_utility, const BOOL can_make_farm);

    void FindInstallationBenefitsCell(AiMain *ai, const sint32 stored, 
        sint16 x, sint16 y, sint16 z, 
        const sint32 idx_city, const sint32 median, 
        const double pos_bonus, 
        double &best_utility_max,
        double &best_cost, 
        sint32 &best_inst_max,
        sint32 &best_extra_data,
        MapPointData &best_w, 
        const BOOL can_make_farm 
    );
        

    void FindInstallationBenefits(AiMain *ai,  const sint32 idx_city, 
       const sint32 median);	

    BOOL IsConnectedByRoad(const CityAgent *da) const;
    BOOL IsConnectedByRoadAtLeast(const CityAgent *da, const sint32 best_road) const;
    void InsertRoad(CityAgent *da, const sint32 road_type); 

    
    sint32 GetProjectedGrossIncome() const { return m_projected_gross_gold; }
    BOOL OwnsGoodAt(MapPointData &w); 


  
    
    BOOL IsConstructingFreight(AiMain *ai, sint32 &freight_cost); 
    double ActionBidBuild(AiMain *ai,  GoalFreight* goal_obj);
    


    

  

	PointerList<IC3TradeOffer> *GetTradeOffers();
	void AddTradeOffer(IC3TradeOffer *offer);
	void RemoveTradeOffer(uint32 id);

	BOOL SeenByPlayer(AiMain *ai, PLAYER_INDEX player);

    void GetGross(sint32 &p, sint32 &g, sint32 &f) 
    {
        p = m_gross_production; 
        f = m_gross_food; 
        g = m_gross_gold; 
    }
    void GetFoodGrowth(double &nf, double &at, double &ag); 

    sint32 GetNumMusicians() const { return m_num_musicians; } 
    
#include "CityAgentPop.h"

    
    void ResetSquad(Squad *new_squad); 

    void DumpStr(AiMain *ai, char *str);
 

	
	
	
	
	
	
	double Estimate_City_Strength(AiMain *ai);

	sint32 Estimate_City_Value(AiMain *ai);

	
	
	
	
	
	
	sint32 Already_Has_One(sint32 building_type);

	
	
	
	
	
	
	sint32 Can_Build_Building(AiMain *ai,sint32 building_type);

	void AddPotentialArmyStrength(AiMain *ai, sint32 unit_type);
	void RecomputePotentialArmyStrength(AiMain *ai);
	void ComputePotentialUnits(AiMain *ai);
	void ComputeBestAgent(AiMain *ai);
	void SetCaptured(const sint32 &turn);
	BOOL WasRecentlyCaptured(AiMain *ai) const;

	double GetProductionBonus(const PRODUCTION_TYPE &type) const;
	void UpdateProductionBonuses(AiMain *ai);

private:
	BOOL m_initialized; 
}; 


extern double WeightTimeToBuild(AiMain *ai, const double val, const double turns_to_complete);

#endif __CITT_AGENT_H__ 
