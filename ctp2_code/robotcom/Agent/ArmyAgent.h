#pragma once

#ifndef __ARMY_AGENT_H__
#define __ARMY_AGENT_H__

#include "common.h"
#include "linked_list.h"





#include "Agent.h"

enum CAUSE_REMOVE_ARMY;

#include "squad_strength.h"

#include "gstypes.h"

#define MINIMUM_MOVEMENT_POINTS_TO_MOVE (16.0)

enum AGENT_STATE {
    AGENT_STATE_LOOKING_FOR_WORK,
    AGENT_STATE_UNBUILT,
    AGENT_STATE_BUILDING,
    AGENT_STATE_GOTO_SETTLE_DEST,
    AGENT_STATE_GOTO_EXPLORE_DEST,
    AGENT_STATE_GOTO_PERIMETER_DEST,
    AGENT_STATE_GOTO_PATROL_DEST,
    AGENT_STATE_GOTO_CHOKEPOINT_DEST,
    AGENT_STATE_GOTO_RECRUITMENT_DEST,
    AGENT_STATE_GOTO_RECRUITMENT_ATTACK_DEST,
    AGENT_STATE_GOTO_ATTACK_DEST,
    AGENT_STATE_GOTO_ENSLAVE_DEST,
	AGENT_STATE_GOTO_RALLY_POINT,
    AGENT_STATE_CARGO_GOTO_BOARD,
    AGENT_STATE_TRANSPORT_GOTO_BOARD,

    AGENT_STATE_WAITING_FOR_TRANSPORT,
    AGENT_STATE_WAITING_FOR_CARGO,

    AGENT_STATE_MARKED_DEAD,
    AGENT_STATE_MARKED_SETTLE,
    AGENT_STATE_MARKED_GARRISON,
    AGENT_STATE_MARKED_WAITING_TOBE_CARGO,
    AGENT_STATE_MARKED_GROUP_ARMY,
    AGENT_STATE_MARKED_WAITING_TO_RECIEVE,
    AGENT_STATE_MARKED_ENSLAVE_IT,

	AGENT_STATE_AT_GOAL
};

class CityAgent;
class AiMain;
class ArmyGoal;
class GoalRecruitAttackers;
class GoalSettle;
class GoalExplore;
class GoalAttack;
class GoalDefense;
class GoalRecruitAttackers;
class GoalTransport;
class GoalEnslave;

enum FRAME_STATE_INDEX;
enum GOAL_TYPE;
enum CAUSE_NEW_ARMY;

class DynamicArray<CityAgent *>;

class ArmyAgent : public Agent {

    sint32 m_path_total_nPoints;
    sint32 m_path_cur_nPoints;
    sint16 m_path_cur;
    MapPointData *m_path;
	double m_pathThreat;

    AGENT_STATE m_state;
    CityAgent *m_home_city_agent;

    sint16 m_nUnits;
    sint16 m_unit_type[k_MAX_ARMY_SIZE];
    BSetID m_home_city_id;

    DynamicArray<sint32> *m_cargo;

    MapPointData *m_my_pos;
    MapPointData *m_XYpos;
    sint16 m_my_continent;
    double m_current_movepoints;
    double m_max_movepoints;
    sint16 m_max_fuel;
    sint16 m_current_fuel;
	GOAL_TYPE m_lastGoal;


    uint8 m_my_is_land  : 1;
	uint8 m_pathValid : 1;
    uint8 m_army_contains_settler : 1;
    uint8 m_uses_fuel : 1;
    uint8 m_is_paratroop : 1;
    uint8 m_has_been_paradropped : 1;
    uint8 m_is_wounded : 1;
	uint8 m_probed_wormhole : 1;
public:

    ArmyAgent(AiMain *ai);
    ArmyAgent(AiMain *ai, BSetID id, CityAgent *hc, const AGENT_STATE the_state);

    ArmyAgent(IC3CivArchive *a);
    ~ArmyAgent();

    void Serialize(IC3CivArchive *archive);

    void Init(AiMain *ai, const AGENT_STATE the_state);
    void Init(AiMain *ai);

    void HookBldQueue(AiMain *ai);

    BOOL Validate(AiMain *ai);

    void BeginTurn(AiMain *ai);

    sint32 GetNUnits() const { return m_nUnits; }
    AGENT_STATE GetState() const { return m_state; }
    void SetState(AGENT_STATE s) { m_state = s; }

    void SetHomeCityAgent(CityAgent *ca);
    CityAgent * GetHomeCityAgent() { return m_home_city_agent; }
    BSetID GetHomeCityId();

	void AddDelUnitTypeStrength(AiMain *ai, sint32 unit_type,
        BOOL add, float hp);

    void FlattenSquadStrength();
    void ResetArmyStrength(AiMain *ai);
    BOOL IsWounded() const { return m_is_wounded; }

    void InsertUnitType(AiMain *ai,  sint32 unit_type, sint32 hp);
	void SetSingleUnitType(AiMain *ai,  sint32 unit_type, sint32 hp);
    sint32 GetUnbuiltType() const;

    void ChangeBuildingType (AiMain *ai,  sint32 unit_type);

    BOOL GetPos(AiMain *ai, MapPointData &pos) const;
    BOOL GetXYPos(AiMain *ai, MapPointData &pos) const;

    void UpdateXYPos(AiMain *ai);
    void UpdatePosition(AiMain *ai);
    void UpdatePosition(AiMain *ai, MapPointData *new_pos);

    void UpdateContinent(AiMain *ai);
    sint32 GetContinent() const;
    void SetContinent(sint32 continent);

    BOOL GetMyIsLand() const;
    void SetMyIsLand(const BOOL l);

    BOOL IsWaitState();

    double EstimateUtility (AiMain *ai, ArmyGoal *goal_obj);

    double PretestBid(AiMain *ai,  GOAL_TYPE goal, MapPointData &pos);

    BOOL PretestContinentReachable(AiMain *ai, MapPointData &dest_pos, BOOL &is_same_cont, BOOL &is_space_dest);

    double BidDistance(AiMain *ai, BOOL will_be_transported, MapPointData &pos, double &move_point_cost,
        sint32 &rounds, double &build_time);

    double ActionBidToSettle(AiMain *ai, GoalSettle *goal_obj,
        const BOOL test_cont);
    double PretestSettle(AiMain *ai,  MapPointData &pos);
    BOOL IsCellWaterCont(AiMain *ai, sint16 x, sint16 y, sint16 z,
        sint32 dest_cont);

    BOOL TouchesThisOcean(AiMain *ai, MapPointData &my_pos, sint32 dest_cont);

    double ActionBidToExplore(AiMain *ai, GoalExplore *goal_obj);
    double PretestExploreBid() const;

    double ActionBidToPatrol(AiMain *ai, ArmyGoal *goal_obj);
    double ActionBidToAttack(AiMain *ai, GoalAttack *goal_obj);
    double ComputeTargetCombatBonus(AiMain *ai, MapPointData &dest_pos, ArmyGoal *goal_obj,
        const sint32 rounds);

    double PretestAttack(AiMain *ai, const MapPointData &pos, const double value);

    double ActionBidToDefend(AiMain *ai, GoalDefense *goal_obj);





    double ActionBidToEnslave(AiMain *ai, GoalEnslave* goal_obj);
    BOOL CanEnslave(AiMain *ai) const;
	BOOL CanConvertCity(AiMain *ai) const;
    BOOL CanBioterrorCity(AiMain *ai) const;
    BOOL CanNanoattackCity(AiMain *ai) const;

	BOOL CanBonusFood(AiMain *ai) const;
	BOOL CanMakePark(AiMain *ai) const;
	BOOL CanCauseUnhappiness(AiMain *ai) const;
	BOOL CanEstablishEmbassy(AiMain *ai) const;
	BOOL CanCreateFranchise(AiMain *ai) const;
	BOOL CanStealTechnology(AiMain *ai) const;
	BOOL CanInciteRevolution(AiMain *ai) const;
	BOOL CanInjoin(AiMain *ai) const;
	BOOL CanSueFranchise(AiMain *ai) const;
	BOOL CanAssasinateRuler(AiMain *ai) const;
	BOOL CanUndergroundRailway(AiMain *ai) const;
	BOOL CanAdvertise(AiMain *ai) const;
	BOOL CanConductHits(AiMain *ai) const;
	BOOL CanSoothsay(AiMain *ai) const;
	BOOL CanPlantNuke(AiMain *ai) const;
	BOOL CanBeachAssault(AiMain *ai, const MapPointData &pos) const;
    BOOL CanAttackFromSpaceShip(AiMain *ai);
    BOOL CanBeachAssaultAtAll(AiMain *ai);
	BOOL CanProbeWormhole(AiMain *ai) const;
	BOOL CanBombard(AiMain *ai, const uint32 move_type) const;
	BOOL CanBombardTarget(AiMain *ai, const MapPointData &pos ) const;
	BOOL CanSue(AiMain *ai) const;

	BOOL ActionTryToEntrench(AiMain *ai);

    BOOL ActionTryToEnslave(AiMain *ai, MapPointData &pos,
							GoalEnslave *goal_obj);
	BOOL ActionTryToRustle(AiMain *ai, MapPointData &pos,
							ArmyGoal *goal_obj);
	BOOL ActionTryToPillage(AiMain *ai, const MapPointData &pos);
	BOOL ActionTryToPirate(AiMain *ai, const MapPointData &pos);
	BOOL ActionTryToSueFranchise(AiMain *ai, const MapPointData &target_pos);
	BOOL ActionTryToReformCity(AiMain *ai, const MapPointData &target_pos);
	BOOL ActionTryToBombard(AiMain *ai, MapPointData &pos,
							ArmyGoal *goal_obj);
	BOOL ActionTryToConvert(AiMain *ai, const MapPointData &target_pos);

	BOOL ActionTryToPlantNuke(AiMain *ai, const MapPointData &target_pos);
	BOOL ActionTryToMakePark(AiMain *ai, const MapPointData &target_pos);
	BOOL ActionTryToUndergroundRailway(AiMain *ai, const MapPointData &target_pos);
	BOOL ActionTryToEstablishEmbassy(AiMain *ai, const MapPointData &target_pos);
	BOOL ActionTryToCreateFranchise(AiMain *ai, const MapPointData &target_pos);
	BOOL ActionTryToAssasinateRuler(AiMain *ai, const MapPointData &target_pos);
	BOOL ActionTryToStealTechnology(AiMain *ai, const MapPointData &target_pos);
	BOOL ActionTryToInjoin(AiMain *ai, const MapPointData &target_pos);
	BOOL ActionTryToInciteRevolution(AiMain *ai, const MapPointData &target_pos);
	BOOL ActionTryToCauseUnhappiness(AiMain *ai, const MapPointData &target_pos);
	BOOL ActionTryToExpel(AiMain *ai, const MapPointData &target_pos);
    BOOL ActionTryToBioterrorCity(AiMain *ai, const MapPointData &target_pos);
    BOOL ActionTryToNanoattackCity(AiMain *ai, const MapPointData &target_pos);
    BOOL ActionTryToBackAway(AiMain *ai);

    void RemoveLastPathPoint();

	sint32 GetBestAttack(AiMain *ai);
	sint32 GetBestDefense(AiMain *ai);
	double GetBestMove(AiMain *ai);


    double PretestIsSameCont(AiMain *ai,  MapPointData &dest_pos,
        sint32 &dest_cont, BOOL &dest_is_land);
    sint32 MovePoints2Rounds(AiMain *ai,  double total_cost);

    BOOL AtEndOfPath(AiMain *ai);
	sint32 GetRemainingPathPoints(AiMain *ai);

    BOOL CanEnter(AiMain *ai, MapPointData &pos);
    BOOL CanEnterNextPathPoint(AiMain *ai);
	void GetNextPathPoint(MapPointData &pos);

    void GotoNextPathPoint(AiMain *ai, BOOL &did_move);


    BOOL FindPath(AiMain *ai, MapPointData &pos, sint32 &pathtime,
        BOOL check_rail_launcher, sint32 cutoff,
    sint32 &nodes_opened, BOOL check_dest, const GOAL_TYPE what_goal);
    sint32 GetPathLen() const { return  m_path_cur_nPoints; }

    BOOL IsInCity(AiMain *ai,  CityAgent *&the_city);

    void UpdateMovePoints(AiMain *ai);

    double GetMovePoints() const { return m_current_movepoints; }
    double GetMaxMovePoints() const { return m_max_movepoints; }
    void UpdateMaxMovePoints(AiMain *ai);
    uint32 GetMoveIntersection(AiMain* ai);

    BOOL ActionSettle(AiMain *ai);
    BOOL GetArmyContainsSettler() const;
    void UpdateArmyContainsSettler(AiMain *ai);
	sint32 GetSettlersCost(AiMain *ai);

	uint32 GetActiveDefenseRangeCount(AiMain *ai);
    BOOL ArmyContainsTransport(AiMain *ai);
    BOOL GetNumCargo(AiMain *ai, sint32 &full, sint32 &empty);
	sint32 GetTransports(AiMain *ai);


    void ActionUngroupArmy(AiMain *ai);


    BOOL BoatCanReachShore(AiMain *ai, MapPointData &dest_pos);
    BOOL PretestTransportSomewhere(AiMain *ai,  ArmyGoal *goal_obj);




	double EstimateTransportUtility(AiMain *ai, ArmyAgent *transport);

    void RegisterClearCargo();
    void RegisterInsertCargo(AiMain *ai, sint32 unit_type, sint32 hp);
    void RegisterUnloadCargo(AiMain *ai, sint32 unit_type, sint32 hp,
		const CAUSE_REMOVE_ARMY & cause);
    void RegisterDeadUnit(AiMain *ai, sint32 unit_type);

    BOOL CanTransport(AiMain *ai, ArmyAgent *cargo);

    BOOL FindPathRoundTrip(AiMain *ai, MapPointData &pos,
        sint32 &pathtime, const sint32 & max_rounds,
        BOOL check_rail_launcher, sint32 cutoff, sint32 &nodes_opened);

	BOOL UseOldPath(AiMain *ai, MapPointData &pos, const GOAL_TYPE the_goal);
	void ValidatePath(AiMain *ai, GOAL_TYPE what_goal);

	void SetPathValid(const BOOL &valid) {m_pathValid = valid;}
	BOOL GetPathValid(void) {return m_pathValid;}

    BOOL FindPathToBoard(AiMain *ai, uint32 move_intersection,
        MapPointData &pos, sint32 &pathtime, sint32 cutoff,
        sint32 &nodes_opened, BOOL check_dest, const GOAL_TYPE what_goal);

    BOOL GotCargo() const;

    BOOL TryToBoardTransport(AiMain *ai,  ArmyAgent *transport);







    void FlipCopyPath(ArmyAgent *cargo);
	void MakePathRoundTrip();

    BOOL MoveToTransportBoard(AiMain *ai, GoalTransport* goal_obj,
        ArmyAgent* cargo,
        BOOL &did_move, BOOL &revealed_unexplored, BOOL &use_marked,
        BOOL &revealed_forign_units, double utility);

   BOOL CargoCanEnter(AiMain *ai, MapPointData &dest_pos);
   void InsertCargo(AiMain *ai, ArmyAgent *cargo_agent);
   BOOL DebarkCargoOntoPath(AiMain *ai);


    void ClearArmy(AiMain *ai);

    void UpdateThreat(AiMain *ai, MapPointData &pos);

    void EventMarkDead();
    void EventMarkTransported();

    double GetDefenseStrength();
    double GetAttackStrength();

    void GetCombatStrength (sint32 &nUnits, sint32 &nFlankers,
        sint32 &range0, sint32 &nrange1,  sint32 &range2,
        double &attack_str, double &defense_str);

    void EventUngarrison();

    BOOL GetUsesFuel()const { return m_uses_fuel; }
    void UpdateUsesFuel(AiMain *ai);

    sint32 GetCurrentFuel()const { return m_current_fuel; }
    void UpdateCurrentFuel(AiMain *ai);

    sint32 GetMaxFuel()const { return m_max_fuel; }
    void UpdateMaxFuel(AiMain *ai);

    BOOL OverFuelSafety(AiMain *ai);
	BOOL IsInFuelRange(AiMain * ai, MapPointData &dest_pos);
    BOOL PretestFuelToTarget(AiMain *ai, MapPointData &dest_pos);

    BOOL IsParatroop() const;
    BOOL IsMigratingParatroop() const;
    void UpdateParadropStatus(AiMain *ai, const CAUSE_NEW_ARMY cause);
    BOOL CanParadropNow(AiMain *ai);

    BOOL IsSingleUse(AiMain *ai);
	BOOL IsNuclearAttack(AiMain *ai);
	void RegisterNukeDeath(AiMain *ai);


    BOOL CanSpaceLaunch(AiMain *ai);
    BOOL CanReachLauncher(AiMain *ai);
    BOOL IsAtSpaceLauncher(AiMain *ai);

    BOOL CanSpaceLand(AiMain *ai);

    BOOL CanMoveSpace(AiMain *ai);
    BOOL CanMoveAir(AiMain *ai);
    BOOL CanMoveWater(AiMain *ai);
	BOOL CanMoveDeepWater(AiMain *ai);

	BOOL HasNoZOC(AiMain *ai);
	BOOL CantCaptureCity(AiMain *ai);
	BOOL CanEntrench(AiMain *ai) const;
	BOOL IsStealthy(AiMain *ai) const;
	BOOL CanBeRustled(AiMain *ai) const;

    sint32 GetActualCost(AiMain *ai);
    void DumpStr(AiMain *ai, char *str);

	void CountUnitTypes(semi_dynamic_array <int> * actual_units);
	sint32 GetUnitCount() const;
	sint32 GetUnitTypeByNumber(const int &i) const;
	sint32 GetCargoUnitTypeByNumber(AiMain *ai, const int &i) const;







	double Estimate_Army_Strength(AiMain *ai);








	double Get_Army_Attack_Strength(AiMain *ai);








	void Get_Army_Attack_And_Defense_Strengths
	(
		AiMain *ai,
		double & attack,
		double & defend
	);








	void Display_Army(AiMain *ai);

	GOAL_TYPE GetLastGoal();
	void SetLastGoal(const GOAL_TYPE &type);




	void UnloadRefueledCargo(AiMain *ai);
	sint32 DisbandObsolete(AiMain *ai);

	void SetProbedWormhole(const BOOL &val);
	BOOL HasProbedWormhole();

	sint16 LandWaterTransitionCount(AiMain *ai);
};





#endif __ARMY_AGENT_H__
