//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Army data handling
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// HAVE_PRAGMA_ONCE
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - TestOrderAny added.
// - CanTransport method added. - Calvitix
// - Added IsWounded method - Calvitix
// - Added a isstealth paramater in characterizeArmy method - Calvitix
// - Moved UnitValidForOrder to Unit.h to be able to access the Unit
//   properties. - April 24th 2005 Martin Gühmann
// - Added Cleanup. (Sep. 25th 2005 Martin Gühmann)
// - Moved the upgrade stuff into its own methods, however more work is needed.
//   (Dec 24th 2006 Martin Gühmann)
// - Improved Ungroup and transport capacity methods. (5-Aug-2007 Martin Gühmann)
// - PerformOrderHere move to target order can now be inserted at tail into the
//   event queue. (30-Jan-2008 Martin Gühmann)
// - Added check move points option to CanAtLeastOneCargoUnloadAt (8-Feb-2008 Martin Gühmann).
// - Separated the Settle event drom the Settle in City event. (19-Feb-2008 Martin Gühmann)
// - Merged finish move. (13-Aug-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef ARMY_DATA_H__
#define ARMY_DATA_H__

class ArmyData;

#include "GameObj.h"
#include "Unit.h"
#include "TradeRoute.h"
#include "cellunitlist.h"
#include "CityRadius.h"
#include "MapPoint.h"

#include "GameEventTypes.h"
#include "GameEventDescription.h"
#include "UnitRecord.h"

class CivArchive;
class Path;
class ArmyData;
class Unit;
template <class T> class PointerList;
class Order;
enum CAUSE_REMOVE_ARMY;
enum ORDER_RESULT;





class OrderRecord;
enum ORDER_TEST {
    ORDER_TEST_OK,
    ORDER_TEST_ILLEGAL,
    ORDER_TEST_LACKS_GOLD,
    ORDER_TEST_NEEDS_TARGET,
    ORDER_TEST_INVALID_TARGET,
    ORDER_TEST_NO_MOVEMENT
};

#include "Army.h"

class KillRecord {
public:
    Unit m_unit;
    CAUSE_REMOVE_ARMY m_cause;
    PLAYER_INDEX m_who;

    KillRecord(const Unit &unit, CAUSE_REMOVE_ARMY cause, PLAYER_INDEX who) {
        m_unit = unit;
        m_cause = cause;
        m_who = who;
    }
};

class ArmyData : public GameObj,
                 public CellUnitList,
                 public CityRadiusCallback
{

private:
    friend class NetArmy;

    UnitDynamicArray          *m_tempKillList;         // Not really used


    UnitDynamicArray          *m_attackedByDefenders;  // Unused but serialized
    PointerList<Order>        *m_orders;               // Used and serialized
    PLAYER_INDEX               m_owner;                // Used and serialized
    MapPoint                   m_pos;                  // Used and serialized
    CAUSE_REMOVE_ARMY          m_removeCause;          // Used and serialized (sint32)
    PLAYER_INDEX               m_killer;               // In unused methods used and is serialized
    bool                       m_hasBeenAdded;         // Used and serialized (uint8)
    bool                       m_isPirating;           // Used and serialized (uint8)
    MBCHAR                    *m_name;                 // Used and serialized
    sint32                     m_reentryTurn;          // Used
    MapPoint                   m_reentryPos;           // Used


    uint8                      m_debugStringColor;     // Unused


    PointerList<KillRecord>   *m_killMeSoon;           // Used

    uint8                      m_dontKillCount;        // Used and serialized
    bool                       m_needToKill;           // Used and serialized (uint8)


    MBCHAR                    *m_debugString;          // Unused

public:




    ArmyData(const Army &army, const UnitDynamicArray &units);
    ArmyData(const Army &army, const CellUnitList &units);
    ArmyData(const Army &army, Unit &unit);
    ArmyData(const Army &army);

    ArmyData(CivArchive &archive);
    ~ArmyData();

    void Serialize(CivArchive &archive);

    PLAYER_INDEX GetOwner() const { return m_owner; }
    void SetOwner(PLAYER_INDEX p);



    bool Insert(const Unit &id);
    void GetPos(MapPoint &pos) const { pos = m_pos; }
    MapPoint RetPos() const { return m_pos; }

    uint32 GetMovementType() const;
    uint32 GetCargoMovementType() const;
    bool HasCargo() const;
    bool GetCargo(sint32 &transports, sint32 &max, sint32 &empty) const;
    bool CargoCanEnter(const MapPoint &pos) const;

    size_t  CountMovementTypeSea() const;

    bool CanSettle(const MapPoint &pos) const;
    bool CanSettle() const;

    bool CanTransport() const;
    bool IsWounded() const;

    bool IsAsleep() const;
    void Sleep();
    void WakeUp();
    bool IsEntrenched() const;
    bool IsEntrenching() const;
    bool CanEntrench() const;
    void Entrench();
    void Detrench();
    bool CanPatrol() const;
    bool IsPatrolling() const;
    void SetPatrolling(bool p);

    void GetActors(Unit &excludeMe, UnitActor **restOfStack);

    void GroupArmy(Army &addme);
    void GroupAllUnits();
    void GroupUnit(Unit unit);
    void UngroupUnits();
    void RemainNumUnits(sint32 remain);

    void FastKill();

    void CityRadiusFunc(const MapPoint &pos);
    void GetActiveDefenders(UnitDynamicArray &input,
                            UnitDynamicArray &output,
                            bool isCargoPodCheck);
    bool CheckActiveDefenders(MapPoint &pos,
                              bool isCargoPodCheck);

    void BeginTurn();
    bool CanFight(CellUnitList &defender);
    sint32 Fight(CellUnitList &defender);

    ORDER_RESULT InvestigateCity(const MapPoint &point);
    ORDER_RESULT NullifyWalls(const MapPoint &point);
    ORDER_RESULT StealTechnology(const MapPoint &point);
    ORDER_RESULT InciteRevolution(const MapPoint &point);
    ORDER_RESULT AssassinateRuler(const MapPoint &point);
    Unit GetAdjacentCity(const MapPoint &point) const;
    sint32 GetCost();

    bool CanFranchise(double &chance, sint32 &uindex) const;
    ORDER_RESULT Franchise(const MapPoint &point);

    bool CanSue(sint32 &uindex) const;
    bool CanSue() const;
    bool CanBeSued() const;
    ORDER_RESULT Sue(const MapPoint &point);
    ORDER_RESULT SueFranchise(const MapPoint &point);
    ORDER_RESULT Expel(const MapPoint &point);

    bool CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amount,
                             sint32 &uindex) const;
    bool CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amount) const;
    ORDER_RESULT CauseUnhappiness(const MapPoint &point, sint32 uindex);

    bool CanPlantNuke(double &chance, double &escape_chance,
                      sint32 &uindex) const;
    bool CanPlantNuke(double &chance, double &escape_chance) const;
    ORDER_RESULT PlantNuke(const MapPoint &point);
    void SetPositionAndFixActors(const MapPoint &p);
    void FixActors(MapPoint &opos, const MapPoint &npos, UnitDynamicArray &revealedUnits);

    bool CanSlaveRaid(double &success, double &death,
                      sint32 &timer, sint32 &amount,
                      sint32 &uindex) const;
    bool CanSlaveRaid(double &success, double &death,
                      sint32 &timer, sint32 &amount) const;
    ORDER_RESULT SlaveRaid(const MapPoint &point);

    bool IsSlaveRaidPossible(const MapPoint &point,
                             double &success, double &death, sint32 &timer, sint32 &amount,
                             sint32 &uindex, bool &target_is_city, Unit &target_city, Unit &home_city);

    bool CanEnslaveSettler(sint32 &uindex) const;
    ORDER_RESULT EnslaveSettler(const MapPoint &point, const sint32 uindex, Unit home_city);
    bool CanUndergroundRailway(double &success, double &death,
                               sint32 &uindex) const;
    bool CanUndergroundRailway(double &success, double &death) const;
    ORDER_RESULT UndergroundRailway(const MapPoint &point);
    bool CanInciteUprising(sint32 &uindex) const;
    ORDER_RESULT InciteUprising(const MapPoint &point);


    bool CanEstablishEmbassy(sint32 &uindex) const;
    bool CanEstablishEmbassy() const;
    ORDER_RESULT EstablishEmbassy(const MapPoint &point);

    bool CanThrowParty(sint32 &uindex) const;
    bool CanThrowParty() const;
    ORDER_RESULT ThrowParty(const MapPoint &point);

    bool CanBioInfect(double &chance, sint32 &uindex) const;
    bool CanBioInfect(double &chance) const;
    ORDER_RESULT BioInfect(const MapPoint &point);
    bool CanPlague(double &chance, sint32 &uindex) const;
    bool CanPlague(double &chance) const;
    ORDER_RESULT Plague(const MapPoint &point);
    bool CanNanoInfect(double &chance, sint32 &uindex) const;
    bool CanNanoInfect(double &chance) const;
    ORDER_RESULT NanoInfect(const MapPoint &point);

    bool CanConvertCity(double &chance, double &deathChance, sint32 &uindex) const;
    bool CanConvertCity(double &chance, double &deathChance) const;
    bool CanConvertCity(const MapPoint &point) const;
    ORDER_RESULT ConvertCity(const MapPoint &point);

    bool CanInterceptTrade(uint32 &uindex) const;
    ORDER_RESULT InterceptTrade();

    bool CanReformCity(sint32 &uindex, const MapPoint &point) const;
    bool CanReformCity() const;
    ORDER_RESULT ReformCity(const MapPoint &point);

    bool CanSellIndulgences(sint32 &uindex) const;
    bool CanSellIndulgences() const;
    ORDER_RESULT IndulgenceSale(const MapPoint &point);
    bool CanSoothsay(sint32 &uindex) const;
    bool CanSoothsay() const;
    ORDER_RESULT Soothsay(const MapPoint &point);

    bool CanAdvertise(sint32 &uindex) const;
    bool CanAdvertise() const;
    ORDER_RESULT Advertise(const MapPoint &point);

    void GetCurrentHP
    (
        sint32 &	count,
        sint32		unit_type[MAX_UNIT_COUNT],
        sint32		unit_hp[MAX_UNIT_COUNT]
    ) const;

    bool AbleToPlantNukeTarget(const MapPoint &point, sint32 &uindex);
    bool AbleToMakeParkTarget(const MapPoint &point, sint32 &uindex);
    bool AbleToUndergroundRailwayTarget(const MapPoint &point, sint32 &uindex);
    bool AbleToConvertTarget(const MapPoint &point, sint32 &uindex);
    bool AbleToEstablishEmbassyTarget(const MapPoint &point, sint32 &uindex);
    bool AbleToCreateFranchiseTarget(const MapPoint &point, sint32 &uindex);
    bool AbleToAssasinateRulerTarget(const MapPoint &point, sint32 &uindex);
    bool AbleToStealTechnologyTarget(const MapPoint &point, sint32 &uindex);
    bool AbleToInjoinTarget(const MapPoint &point, sint32 &uindex);
    bool AbleToInciteRevolutionTarget(const MapPoint &point, sint32 &uindex);
    bool AbleToCauseUnhappinessTarget(const MapPoint &point, sint32 &uindex);
    bool AbleToExpelTarget(const MapPoint &point, sint32 &uindex);
    bool CanExpel() const;

    bool CanCreatePark(sint32 &uindex) const;
    bool CanCreatePark() const;
    ORDER_RESULT CreatePark(const MapPoint &point);

    bool CanPillage(uint32 &uindex) const;
    bool CanPillage() const;
    ORDER_RESULT Pillage(bool test_ownership);

    bool CanInjoin(sint32 &uindex) const;
    bool CanInjoin() const;
    ORDER_RESULT Injoin(const MapPoint &point);

    bool GetFirstMoveThisTurn() const;

    bool HasLeftMap() const;
    bool CanNukeCity() const;

    void CurMinMovementPoints(double &cur) const;
    void MinMovementPoints(double &cur) const;

    void ThisMeansWAR(PLAYER_INDEX denfender);



    bool CanBombardTargetType(const CellUnitList & units) const;
    bool GetBombardRange(sint32 & min_rge, sint32 & max_rge);
    bool CanBombard(const MapPoint &point) const;
    bool CanBombard() const;
    bool BombardCity(const MapPoint &point, bool doAnimations);
    ORDER_RESULT Bombard(const MapPoint &point);




    sint32 NumOrders() const;
    const Order *GetOrder(sint32 index) const;
    void AddOrders(UNIT_ORDER_TYPE order, Path *path);
    void AddOrders(UNIT_ORDER_TYPE order, const MapPoint &point);
    void AddOrders(UNIT_ORDER_TYPE order);
    void AutoAddOrders(UNIT_ORDER_TYPE order, Path *path,
                       const MapPoint &point, sint32 argument);
    void AutoAddOrdersWrongTurn(UNIT_ORDER_TYPE order, Path *path,
                                const MapPoint &point, sint32 argument);

    void AddOrders(UNIT_ORDER_TYPE order, Path *path, const MapPoint &point,
                   sint32 argument, GAME_EVENT passedEvent = GEV_MAX);

    void ClearOrders();
    void ClearOrdersExceptGroupUnits();
    bool ExecuteOrders(bool propagate = true);
    void InformAI(const UNIT_ORDER_TYPE order_type, const MapPoint &pos);

    void ResumePatrol();
    void ForgetPatrol();

    bool IsOccupiedByForeigner(const MapPoint &pos);

    sint32 NumUnitsCanMoveIntoTransport(const MapPoint &pos, CellUnitList &transports) const;
    sint32 NumUnitsCanMoveIntoThisTransport(const CellUnitList &transports) const;
    bool CanMoveIntoTransport(const MapPoint &pos, CellUnitList &transports) const;
    bool CanMoveIntoThisTransport(const CellUnitList &transports) const;

    bool MoveIntoTransport(const MapPoint &pos, CellUnitList &transports);
    void DoBoardTransport(Order *order);

    void SetRemoveCause(CAUSE_REMOVE_ARMY cause);
    CAUSE_REMOVE_ARMY GetRemoveCause() const;

    uint8 DontKillCount() const { return m_dontKillCount; }
    void SetNeedToKill() { m_needToKill = TRUE; }
    void IncrementDontKillCount() { m_dontKillCount++; }
    void DecrementDontKillCount	();

    void ResetPos();

    void GetAdvanceFromCityAssault(const Unit &c, PLAYER_INDEX otherPlayer);

    void IndicateAdded();
    PLAYER_INDEX GetKiller() const;
    void SetKiller(PLAYER_INDEX who);

    void AddDeath(const Unit &unit, CAUSE_REMOVE_ARMY cause,
                  PLAYER_INDEX who);
    bool HasBeenAdded() const { return m_hasBeenAdded; }

    sint32 GetMinFuel();
    void CalcRemainingFuel(sint32 &num_tiles_to_half, sint32 &num_tiles_to_empty) const;

    bool CanMove();


    bool CanBeachAssault() const;
    bool CanHearGossip() const;
    bool CanSlaveUprising() const;
    bool CanInciteRevolution( double &chance, double &eliteChance ) const;
    bool CanCloak() const;
    bool CanCreateFranchise( double &chance) const;
    bool CanAssasinateRuler( double &chance, double &eliteChance) const;
    bool CanStealTechnology( double &randChance, double &chance) const;
    bool CanInvestigateCity( double &chance, double &eliteChance) const;

    bool CanLaunch(sint32 &uindex) const;
    bool CanLaunch() const;
    ORDER_RESULT Launch(Order *order);
    void SetReentry(sint32 turns, MapPoint &pos);
    void Reenter();

    bool CanTarget(sint32 &uindex) const;
    bool CanTarget() const;
    ORDER_RESULT Target(Order *order);
    ORDER_RESULT ClearTarget();

    void SetTurnOver();
    bool TurnOver();

    bool CanAtLeastOneCargoUnloadAt(const MapPoint &old_pos, const MapPoint &dest_pos, const bool & used_vision, const bool check_move_points = true) const;

    static bool GetInciteRevolutionCost( const MapPoint &point, sint32 &attackCost );
    static bool GetInciteUprisingCost( const MapPoint &point, sint32 &attackCost );







    bool ExecuteMoveOrder(Order *order);

    void CheckLoadSleepingCargoFromCity(Order *order);
    bool Move(WORLD_DIRECTION, Order *order);
    bool FinishMove(WORLD_DIRECTION d, MapPoint &newPos, UNIT_ORDER_TYPE order);
    bool FinishAttack(Order *order);
    bool CheckSpecialUnitMove(const MapPoint &pos);
    bool MoveIntoForeigner(const MapPoint &pos);
    bool VerifyAttack(UNIT_ORDER_TYPE order, const MapPoint &pos,
                      sint32 defense_owner);

    bool ExertsZOC() const;
    void UpdateZOCForMove(const MapPoint &pos, WORLD_DIRECTION d);
    void RevealZOCUnits(const MapPoint &pos);
    bool MoveIntoCell(const MapPoint &pos,
                      UNIT_ORDER_TYPE order, WORLD_DIRECTION d);

    void MoveActors(const MapPoint &pos,
                    UnitDynamicArray &revealedUnits,
                    bool teleport = false);
    void MoveUnits(const MapPoint &pos);
    void DeductMoveCost(const MapPoint &pos);

    void CheckTerrainEvents();

    void Disband();
    void Settle();
    void SettleInCity();




    bool ExecuteUnloadOrder(Order *order);

    void UpdateZOCForRemoval();
    bool ExecuteTeleportOrder(Order *order);
    void SetUnloadMovementPoints();

    void ActionSuccessful(SPECATTACK attack, Unit &unit, const Unit &c);
    void ActionUnsuccessful(const MapPoint &point);

    bool ExecuteSpecialOrder(Order *order, bool &keepGoing);
    void AddSpecialActionUsed(Unit &who);

    bool CheckWasEnemyVisible(const MapPoint &pos, bool justCheck = false);
    bool DoLeaveOurLandsCheck(const MapPoint &newPos, UNIT_ORDER_TYPE order_type);

    void FinishUnloadOrder(Army &unloadingArmy, MapPoint &to_pt);




    Path *RemovePathedOrder();


    void CharacterizeArmy(
                           bool & isspecial,
                           bool & isstealth,
                           sint32 & maxattack,
                           sint32 & maxdefense,
                           bool & cancapture,
                           bool & haszoc,
                           bool & canbombard
                          ) const;

    void CharacterizeCargo(
                           bool & isspecial,
                           bool & isstealth,
                           sint32 & maxattack,
                           sint32 & maxdefense,
                           bool & cancapture,
                           bool & haszoc,
                           bool & canbombard
                          ) const;


    bool IsCivilian() const;

    bool CanPerformSpecialAction() const;
    void CheckAddEventOrder();
    void IncrementOrderPath();







    bool CheckValidDestination(const MapPoint &dest) const;


    bool AtEndOfPath() const;


    bool GetNextPathPoint(MapPoint & next_pos) const;



    MBCHAR const * GetDebugString(void) const;
    void SetDebugString(MBCHAR const * string);
    uint8 GetDebugStringColor(void) const { return m_debugStringColor; }
    void SetDebugStringColor(uint8 color) { m_debugStringColor = color; }



    bool TestOrderAll(const OrderRecord *order_rec) const;
    bool TestOrderAny(const OrderRecord * order_rec) const;


    bool TestOrderUnit(const OrderRecord *order_rec, uint32 unit_index) const;


    ORDER_TEST TestOrder(const OrderRecord * order_rec) const;


    ORDER_TEST TestOrderHere(const OrderRecord * order_rec, const MapPoint & pos) const;


    ORDER_TEST CargoTestOrderHere(const OrderRecord * order_rec, const MapPoint & pos) const;


    void PerformOrder(const OrderRecord * order_rec);


    void PerformOrderHere(const OrderRecord * order_rec, const Path * path, GAME_EVENT_INSERT priority = GEV_INSERT_AfterCurrent);


    bool IsObsolete() const;

    void StopPirating();


    const MBCHAR *GetName() const;
    void SetName(const MBCHAR *name);


    bool PlayerCanSee(const PLAYER_INDEX playerId) const;


    sint16 CountNuclearUnits() const;


    sint16 CountBioUnits() const;


    sint16 CountNanoUnits() const;


    sint16 DisbandNuclearUnits(const sint16 count);


    sint16 DisbandBioUnits(const sint16 count);


    sint16 DisbandNanoUnits(const sint16 count);


    static bool TargetValidForOrder(const OrderRecord * order_rec, const MapPoint &pos);


    bool HasVeterans() const;
    bool NearestUnexplored(MapPoint &pos) const;

    void Upgrade();

    static void DisassociateEventsFromOrdersDB(){ delete[] s_orderDBToEventMap; s_orderDBToEventMap = NULL; };
    static void AssociateEventsWithOrdersDB();
    static sint32 OrderToEvent(sint32 order) { return s_orderDBToEventMap[order]; };
    
    void CheckHostileTerrain();
    void CheckMineField();
    bool CheckSink();
    void BarbarianSpawning();
    bool HasElite() const;
    bool IsInVisionRangeAndCanEnter(MapPoint &pos) const;

    void Battle(const MapPoint &pos, CellUnitList & defender);


private:
    size_t CargoCountIf(UnitRecord::BoolAccessor a_Property) const;

    static sint32 *s_orderDBToEventMap;
};

#endif

