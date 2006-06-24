//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Army interface
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added option to reduce resync reporting.
// - Added IsWounded method.
// - Added CanTransport and IsCivilian methods.
// - Made GetCurrentHP const.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __ARMY_H__
#define __ARMY_H__ 1

class Army;

#include "AICause.h"
#include "ID.h"
#include "Player.h"
#include "Unit.h"

class CivArchive;
class Path;
class ArmyData;
class Unit;
class CellUnitList;
class UnitActor;
class MapPoint;
class Order;

size_t const	MAX_UNIT_COUNT	= 100;   // TODO: check k_MAX_ARMY_SIZE


class Army : public ID
{ 
public: 
	const ArmyData *GetData() const;
	ArmyData *AccessData() const;

	Army() : ID() {}
	Army(sint32 val) : ID(val) {}
	Army(uint32 val) : ID(val) {}
	Army(ID i) : ID(i.m_id) {}

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
	bool IsPresent(Unit &u);
	


	bool Insert(const Unit &id); 

	PLAYER_INDEX GetOwner() const;
	void SetOwner(PLAYER_INDEX p);
	void GetPos(MapPoint &pos) const;

	void SortByAttack();
	void SortByDefense();

	uint32 GetMovementType() const; 
	bool   CanEnter(const MapPoint &point) const;

	bool IsAtLeastOneMoveLand() const;
	bool IsAtLeastOneMoveWater() const;
	bool IsAtLeastOneMoveShallowWater() const;
	bool IsAtLeastOneMoveAir() const;
	bool IsAtLeastOneMoveMountain() const;
	bool GetMovementTypeAir() const; 
	bool IsMovePointsEnough(double cost);

	bool IsIgnoresZOC() const;

	bool CanSettle(const MapPoint &pos) const;
	bool CanSettle() const;

	bool IsOccupiedByForeigner(const MapPoint &pos);
	sint32 IsEnemy(PLAYER_INDEX owner) const;
	sint32 IsEnemy(Unit defender) const;
	sint32 IsEnemy(Army &defender) const;


	double GetHPModifier(); 

	bool IsAsleep() const;
	bool IsEntrenched() const;
	bool IsEntrenching() const;
	bool CanEntrench();
	bool CanPatrol() const;
	bool IsPatrolling() const;
	void SetPatrolling(bool p);

	void GetActors(sint32 &n, UnitActor **moveActor, UnitActor *butnotthis);

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
	void ForceVisibleThisTurn(const PLAYER_INDEX to_me);


	bool IsArmyPosFilled(const MapPoint &pos) const;
	void ResolveArmyPosConflicts();

	void BeginTurn();
	bool CanFight(CellUnitList &defender);

	bool IsVisible(sint32 player);

	sint32 GetCost();

	bool CanFranchise(double &chance, sint32 &uindex);

	bool CanSue(sint32 &uindex);
	bool CanSue();
	bool CanBeSued();
	bool CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amount,
							 sint32 &uindex);
	bool CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amount);
	bool CanPlantNuke(double &chance, double &escape_chance,
					  sint32 &uindex);
	bool CanPlantNuke(double &chance, double &escape_chance);
	bool CanSlaveRaid(double &success, double &death, 
					  sint32 &timer, sint32 &amount,
					  sint32 &uindex);
	bool CanSlaveRaid(double &success, double &death, 
					  sint32 &timer, sint32 &amount);
    bool IsSlaveRaidPossible(const MapPoint &point, 
							 double &success, double &death, sint32 &timer, sint32 &amount,
							 sint32 &uindex, bool &target_is_city, Unit &target_city, Unit &home_city);

	bool CanEnslaveSettler(sint32 &uindex);
	bool CanUndergroundRailway(double &success, double &death,
							   sint32 &uindex);
	bool CanUndergroundRailway(double &success, double &death);
	bool CanInciteUprising(sint32 &uindex);
	
	bool CanEstablishEmbassy(sint32 &uindex);
	bool CanEstablishEmbassy();

	bool CanBioInfect(double &chance, sint32 &uindex);
	bool CanBioInfect(double &chance);
	bool CanNanoInfect(double &chance, sint32 &uindex);
	bool CanNanoInfect(double &chance);

	bool CanConvertCity(double &chance, double &deathChance, sint32 &uindex);
	bool CanConvertCity(double &chance, double &deathChance);
	bool CanConvertCity(const MapPoint &point);

	
	bool CanThrowParty(sint32 &uindex);
	bool CanThrowParty();
	
	bool CanReformCity(sint32 &uindex, const MapPoint &point);
	bool CanReformCity();

	bool CanSellIndulgences(sint32 &uindex);
	bool CanSellIndulgences();
	bool CanSoothsay(sint32 &uindex);
	bool CanSoothsay();

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
	bool CanExpel();

	bool CanCreatePark(sint32 &uindex);
	bool CanCreatePark();

	bool CanPillage(uint32 &uindex) const;
	bool CanPillage() const;

	bool CanInjoin(sint32 &uindex);
	bool CanInjoin();

	bool GetFirstMoveThisTurn() const;
	bool CanBeAirlifted(const MapPoint &dest,
						bool &isSourceCity, bool &isDestCity);

	bool HasLeftMap() const;
	
	void CurMinMovementPoints(double &cur) const; 
	void MinMovementPoints(double &cur) const; 
	

	bool CanBombard(const MapPoint &point);
	bool CanBombard();
	bool CanInterceptTrade(uint32 &uindex) const;

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

	bool CanMoveIntoCell(const MapPoint &pos, bool &zocViolation);
	bool CanMoveIntoTransport(const MapPoint &pos, CellUnitList &transports);
	sint32 NumUnitsCanMoveIntoThisTransport(const Army &transports);
	bool CanMoveIntoThisTransport(CellUnitList &transports);
	bool CanMoveIntoThisTransport(const Army &transports);

	void IndicateAdded() const;
	PLAYER_INDEX GetKiller() const;
	void SetKiller(PLAYER_INDEX who);

	void AddDeath(const Unit &u, CAUSE_REMOVE_ARMY cause, PLAYER_INDEX who);

	sint32 GetMinFuel() const;

	void CalcRemainingFuel(sint32 &num_tiles_to_half, sint32 &num_tiles_to_empty) const ;

	bool CanMove() const;
	
	bool CanAdvertise() const;

	
	bool CanBeachAssault();
	bool CanHearGossip();
	bool CanSlaveUprising();
	bool CanInciteRevolution( double &chance, double &eliteChance );
	bool CanCloak();
	bool CanCreateFranchise( double &chance );
	bool CanAssasinateRuler( double &chance, double &eliteChance );
	bool CanStealTechnology( double &randChance, double &chance );
	bool CanInvestigateCity( double &chance, double &eliteChance );

	void SetTurnOver();
	bool TurnOver() const;

	bool CanAtLeastOneCargoUnloadAt(const MapPoint &old_pos, const MapPoint &dest_pos, const bool & use_vision);

	void GetCurrentHP
	(
		sint32 &	n,
		sint32		unit_type[MAX_UNIT_COUNT],
		sint32		unit_hp[MAX_UNIT_COUNT]
	) const;

	bool CanTransport(void) const;
	bool IsCivilian(void) const;
	bool IsWounded(void) const;

	bool ExecutedThisTurn() const;
	void InformAI(const UNIT_ORDER_TYPE order_type, const MapPoint &pos);

	static bool GetInciteRevolutionCost( const MapPoint &point, sint32 &attackCost );
	static bool GetInciteUprisingCost( const MapPoint &point, sint32 &attackCost );
};

#endif
