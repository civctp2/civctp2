//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Army interface
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
// _MSC_VER		
// - Compiler version (for the Microsoft C++ compiler only)
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines, and the blocks
//       that are inactive for _MSC_VER value 1200 are modified Apolyton code. 
//       The blocks that are inactiThe blocks that are active for _MSC_VER value 
//       1200 are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added option to reduce resync reporting.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __ARMY_H__
#define __ARMY_H__ 1

#include "ID.h"

class CivArchive;
class Path;
class ArmyData;
class Unit;
class CellUnitList;
class UnitActor;
enum UNIT_ORDER_TYPE;
typedef sint32 PLAYER_INDEX;
class MapPoint;
enum CAUSE_REMOVE_ARMY;
class Order;

class Army : public ID
{ 
public: 
	const ArmyData *GetData() const;
	ArmyData *AccessData() const;


	


	Army() : ID() {}
	Army(sint32 val) : ID(val) {}
	Army(uint32 val) : ID(val) {}
	Army(ID i) : ID(i.m_id) {}
	Army(const int val) : ID(val) {}
	Army(const unsigned int val) : ID(val) {}

	ArmyData *operator -> () const { return AccessData(); }

	bool IsValid() const;

	void KillArmy();
	void Kill() { KillArmy(); }
	void FastKill();

	void RemoveAllReferences();

	Unit & operator [] (const sint32 i);
	const Unit & operator [] (const sint32 i) const;
	Unit & Access(const sint32 i);
	sint32 Num() const;
	sint32 Del(const Unit &unit);
	sint32 DelIndex(sint32 i);
	const Unit &Get(sint32 i) const;
	BOOL IsPresent(Unit &u);
	


	BOOL Insert(const Unit &id); 

	PLAYER_INDEX GetOwner() const;
	void SetOwner(PLAYER_INDEX p);
	void GetPos(MapPoint &pos) const;

	void SortByAttack();
	void SortByDefense();

    uint32 GetMovementType() const; 
	BOOL   CanEnter(const MapPoint &point) const;

	BOOL IsAtLeastOneMoveLand() const;
	BOOL IsAtLeastOneMoveWater() const;
    BOOL IsAtLeastOneMoveShallowWater() const;
	BOOL IsAtLeastOneMoveAir() const;
	BOOL IsAtLeastOneMoveMountain() const;
    BOOL GetMovementTypeAir() const; 
	BOOL IsMovePointsEnough(double cost);

    BOOL IsIgnoresZOC() const;

    BOOL CanSettle(const MapPoint &pos) const;
    BOOL CanSettle() const;

	BOOL IsOccupiedByForeigner(const MapPoint &pos);
	sint32 IsEnemy(PLAYER_INDEX owner) const;
	sint32 IsEnemy(Unit defender) const;
	sint32 IsEnemy(Army &defender) const;


    double GetHPModifier(); 

	BOOL IsAsleep() const;
	BOOL IsEntrenched() const;
	BOOL IsEntrenching() const;
	BOOL CanEntrench();
	BOOL CanPatrol() const;
	BOOL IsPatrolling() const;
	void SetPatrolling(BOOL p);

    void GetActors(sint32 &n, UnitActor **moveActor, UnitActor *butnotthis);

#if defined(ACTIVISION_ORIGINAL)
    BOOL GetTopVisibleUnitOfMoveType(const sint32 looking_player, const uint32 move, sint32 &maxi) const;
    Unit GetTopVisibleUnit(const sint32 looking_player) const; 
#else
	bool GetTopVisibleUnitOfMoveType
	(
		PLAYER_INDEX const	looker,
		uint32 const		moveType,
		sint32 &			maxi,
		bool &				isResyncReported
	) const;
	Unit GetTopVisibleUnit
	(
		PLAYER_INDEX const	looker
	) const;
#endif
    void ForceVisibleThisTurn(const PLAYER_INDEX to_me);


    BOOL IsArmyPosFilled(const MapPoint &pos) const;
    void ResolveArmyPosConflicts();

	void BeginTurn();
	BOOL CanFight(CellUnitList &defender);

	BOOL IsVisible(sint32 player);

	sint32 GetCost();

	BOOL CanFranchise(double &chance, sint32 &uindex);

	BOOL CanSue(sint32 &uindex);
	BOOL CanSue();
	BOOL CanBeSued();
	BOOL CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amount,
							 sint32 &uindex);
	BOOL CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amount);
	BOOL CanPlantNuke(double &chance, double &escape_chance,
					  sint32 &uindex);
	BOOL CanPlantNuke(double &chance, double &escape_chance);
	BOOL CanSlaveRaid(double &success, double &death, 
					  sint32 &timer, sint32 &amount,
					  sint32 &uindex);
	BOOL CanSlaveRaid(double &success, double &death, 
					  sint32 &timer, sint32 &amount);
    BOOL IsSlaveRaidPossible(const MapPoint &point, 
							 double &success, double &death, sint32 &timer, sint32 &amount,
							 sint32 &uindex, BOOL &target_is_city, Unit &target_city, Unit &home_city);

	BOOL CanEnslaveSettler(sint32 &uindex);
	BOOL CanUndergroundRailway(double &success, double &death,
							   sint32 &uindex);
	BOOL CanUndergroundRailway(double &success, double &death);
	BOOL CanInciteUprising(sint32 &uindex);
	
	BOOL CanEstablishEmbassy(sint32 &uindex);
	BOOL CanEstablishEmbassy();

	BOOL CanBioInfect(double &chance, sint32 &uindex);
	BOOL CanBioInfect(double &chance);
	BOOL CanNanoInfect(double &chance, sint32 &uindex);
	BOOL CanNanoInfect(double &chance);

	BOOL CanConvertCity(double &chance, double &deathChance, sint32 &uindex);
	BOOL CanConvertCity(double &chance, double &deathChance);
	BOOL CanConvertCity(const MapPoint &point);

	
	BOOL CanThrowParty(sint32 &uindex);
	BOOL CanThrowParty();
	
	BOOL CanReformCity(sint32 &uindex, const MapPoint &point);
	BOOL CanReformCity();

	BOOL CanSellIndulgences(sint32 &uindex);
	BOOL CanSellIndulgences();
	BOOL CanSoothsay(sint32 &uindex);
	BOOL CanSoothsay();

	BOOL AbleToPlantNukeTarget(const MapPoint &point, sint32 &uindex);
	BOOL AbleToMakeParkTarget(const MapPoint &point, sint32 &uindex);
	BOOL AbleToUndergroundRailwayTarget(const MapPoint &point, sint32 &uindex);
	BOOL AbleToConvertTarget(const MapPoint &point, sint32 &uindex);
	BOOL AbleToEstablishEmbassyTarget(const MapPoint &point, sint32 &uindex);
	BOOL AbleToCreateFranchiseTarget(const MapPoint &point, sint32 &uindex);
	BOOL AbleToAssasinateRulerTarget(const MapPoint &point, sint32 &uindex);
	BOOL AbleToStealTechnologyTarget(const MapPoint &point, sint32 &uindex);
	BOOL AbleToInjoinTarget(const MapPoint &point, sint32 &uindex);
	BOOL AbleToInciteRevolutionTarget(const MapPoint &point, sint32 &uindex);
	BOOL AbleToCauseUnhappinessTarget(const MapPoint &point, sint32 &uindex);
	BOOL AbleToExpelTarget(const MapPoint &point, sint32 &uindex);
	BOOL CanExpel();

	BOOL CanCreatePark(sint32 &uindex);
	BOOL CanCreatePark();

	BOOL CanPillage(uint32 &uindex) const;
	BOOL CanPillage() const;

	BOOL CanInjoin(sint32 &uindex);
	BOOL CanInjoin();

	BOOL GetFirstMoveThisTurn() const;
	BOOL CanBeAirlifted(const MapPoint &dest,
						BOOL &isSourceCity, BOOL &isDestCity);

	BOOL HasLeftMap() const;
	
    void CurMinMovementPoints(double &cur) const; 
    void MinMovementPoints(double &cur) const; 
	

	BOOL CanBombard(const MapPoint &point);
	BOOL CanBombard();
	BOOL CanInterceptTrade(uint32 &uindex) const;

	void Fight(CellUnitList &defender);

	void SetRemoveCause(CAUSE_REMOVE_ARMY cause);
	CAUSE_REMOVE_ARMY GetRemoveCause() const;

	void ResetPos();

	


	sint32 NumOrders() const;
	const Order *GetOrder(sint32 index) const;
	void AddOrders(UNIT_ORDER_TYPE order, Path *path);
	void AddOrders(UNIT_ORDER_TYPE order, const MapPoint &point);
	void AddOrders(UNIT_ORDER_TYPE order);
	void AddOrders(UNIT_ORDER_TYPE order, Path *path, const MapPoint &point, sint32 arg);
	void AutoAddOrders(UNIT_ORDER_TYPE order, Path *path, const MapPoint &point, sint32 arg);
	void AutoAddOrdersWrongTurn(UNIT_ORDER_TYPE order, Path *path, const MapPoint &point, sint32 arg);
	void ClearOrders();
	void ExecuteOrders();
	void ResumePatrol();
	void ForgetPatrol();

    BOOL CanMoveIntoCell(const MapPoint &pos, BOOL &zocViolation);
	BOOL CanMoveIntoTransport(const MapPoint &pos, CellUnitList &transports);
	sint32 NumUnitsCanMoveIntoThisTransport(const Army &transports);
	BOOL CanMoveIntoThisTransport(CellUnitList &transports);
	BOOL CanMoveIntoThisTransport(const Army &transports);

	void IndicateAdded() const;
	PLAYER_INDEX GetKiller() const;
	void SetKiller(PLAYER_INDEX who);

	void AddDeath(const Unit &u, CAUSE_REMOVE_ARMY cause, PLAYER_INDEX who);

	sint32 GetMinFuel() const;

    void CalcRemainingFuel(sint32 &num_tiles_to_half, sint32 &num_tiles_to_empty) const ;

	BOOL CanMove() const;
	
	BOOL CanAdvertise() const;

	
	BOOL CanBeachAssault();
	BOOL CanHearGossip();
	BOOL CanSlaveUprising();
	BOOL CanInciteRevolution( double &chance, double &eliteChance );
	BOOL CanCloak();
	BOOL CanCreateFranchise( double &chance );
	BOOL CanAssasinateRuler( double &chance, double &eliteChance );
	BOOL CanStealTechnology( double &randChance, double &chance );
	BOOL CanInvestigateCity( double &chance, double &eliteChance );

	void SetTurnOver();
	BOOL TurnOver() const;

    BOOL CanAtLeastOneCargoUnloadAt(const MapPoint &old_pos, const MapPoint &dest_pos, const BOOL & use_vision);

    void GetCurrentHP(sint32 &n, sint32 unit_type[100], 
        sint32 unit_hp[100]); 

	BOOL ExecutedThisTurn() const;
    void InformAI(const UNIT_ORDER_TYPE order_type, const MapPoint &pos);

	static BOOL GetInciteRevolutionCost( const MapPoint &point, sint32 &attackCost );
	static BOOL GetInciteUprisingCost( const MapPoint &point, sint32 &attackCost );
}; 

#endif
