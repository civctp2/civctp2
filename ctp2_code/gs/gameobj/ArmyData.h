//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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
// - When defined, allows Microsoft C++ extensions.
// - When not defined, generates standard C++.
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks 
//       between #else and #endif are modified Apolyton code. The blocks 
//       between #if and #else are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - TestOrderAny added.
// - CanTransport method added. - Calvitix
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __ARMY_DATA_H__
#define __ARMY_DATA_H__ 1

#include "GameObj.h"
#include "Unit.h"
#include "TradeRoute.h"
#include "CellUnitList.h"
#include "CityRadius.h"
#include "MapPoint.h"

#include "GameEventDescription.h"

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

class ArmyData : public GAMEOBJ,
				 public CellUnitList,
				 public CityRadiusCallback
{ 

private:   
	friend class NetArmy;

	UnitDynamicArray *m_tempKillList;  
	                                   
	                                   
	UnitDynamicArray *m_attackedByDefenders; 
	PointerList<Order> *m_orders;
	PLAYER_INDEX m_owner;
	MapPoint m_pos;
	CAUSE_REMOVE_ARMY m_removeCause;
	PLAYER_INDEX m_killer;
	BOOL m_hasBeenAdded;
	bool m_isPirating;
	MBCHAR *m_name;
	sint32 m_reentryTurn;
	MapPoint m_reentryPos;

	
	uint8 m_debugStringColor;
	
	
	PointerList<KillRecord> *m_killMeSoon;
	Army m_debarked;

	BOOL m_isTransported, m_revealedForeignUnits, m_revealedUnexplored,
	     m_zocViolation, m_didBattle, m_didMove;

	uint8 m_dontKillCount;
	BOOL m_needToKill;

	
	MBCHAR *m_debugString;	

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
	


	BOOL Insert(const Unit &id); 
	void GetPos(MapPoint &pos) const { pos = m_pos; }
	MapPoint RetPos() const { return m_pos; }

    uint32 GetMovementType() const; 
	uint32 GetCargoMovementType() const;
	BOOL HasCargo() const;
	BOOL GetCargo(sint32 &transports, sint32 &max, sint32 &empty) const;
	BOOL CargoCanEnter(const MapPoint &pos) const;

	sint16 CountMovementTypeSea() const;

    BOOL CanSettle(const MapPoint &pos) const;
    BOOL CanSettle() const;

#if !defined (ACTIVISION_ORIGINAL)
    BOOL CanTransport() const;
#endif

	BOOL IsAsleep() const;
	void Sleep();
	void WakeUp();
	BOOL IsEntrenched() const;
	BOOL IsEntrenching() const;
	BOOL CanEntrench() const;
	void Entrench();
	void Detrench();
	BOOL CanPatrol() const;
	BOOL IsPatrolling() const;
	void SetPatrolling(BOOL p);

    void GetActors(Unit &excludeMe, UnitActor **restOfStack);

    void GroupArmy(Army &addme); 
	void GroupAllUnits();
	void GroupUnit(Unit &unit);
	void UngroupUnits();

    void FastKill();

	void CityRadiusFunc(const MapPoint &pos);
	void GetActiveDefenders(UnitDynamicArray &input, 
							UnitDynamicArray &output,
							BOOL isCargoPodCheck);
	BOOL CheckActiveDefenders(MapPoint &pos, 
							  BOOL isCargoPodCheck);
	
	void BeginTurn();
	BOOL CanFight(CellUnitList &defender);
	sint32 Fight(CellUnitList &defender);

	ORDER_RESULT InvestigateCity(const MapPoint &point);
	ORDER_RESULT NullifyWalls(const MapPoint &point);
	ORDER_RESULT StealTechnology(const MapPoint &point);
	ORDER_RESULT InciteRevolution(const MapPoint &point);
	ORDER_RESULT AssassinateRuler(const MapPoint &point);
	Unit GetAdjacentCity(const MapPoint &point) const;
	sint32 GetCost();

	BOOL CanFranchise(double &chance, sint32 &uindex) const;
	ORDER_RESULT Franchise(const MapPoint &point);

	BOOL CanSue(sint32 &uindex) const;
	BOOL CanSue() const;
	BOOL CanBeSued() const;
	ORDER_RESULT Sue(const MapPoint &point);
	ORDER_RESULT SueFranchise(const MapPoint &point);
	ORDER_RESULT Expel(const MapPoint &point);

	BOOL CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amount,
							 sint32 &uindex) const;
	BOOL CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amount) const;
	ORDER_RESULT CauseUnhappiness(const MapPoint &point, sint32 uindex);
	
	BOOL CanPlantNuke(double &chance, double &escape_chance,
					  sint32 &uindex) const;
	BOOL CanPlantNuke(double &chance, double &escape_chance) const;
	ORDER_RESULT PlantNuke(const MapPoint &point);
	void SetPositionAndFixActors(const MapPoint &p);
	void FixActors(MapPoint &opos, const MapPoint &npos, UnitDynamicArray &revealedUnits);

	BOOL CanSlaveRaid(double &success, double &death, 
					  sint32 &timer, sint32 &amount,
					  sint32 &uindex) const;
	BOOL CanSlaveRaid(double &success, double &death, 
					  sint32 &timer, sint32 &amount) const;
	ORDER_RESULT SlaveRaid(const MapPoint &point);

    BOOL IsSlaveRaidPossible(const MapPoint &point, 
							 double &success, double &death, sint32 &timer, sint32 &amount,
							 sint32 &uindex, BOOL &target_is_city, Unit &target_city, Unit &home_city);

	BOOL CanEnslaveSettler(sint32 &uindex) const;
	ORDER_RESULT EnslaveSettler(const MapPoint &point, const sint32 uindex, Unit home_city);
	BOOL CanUndergroundRailway(double &success, double &death,
							   sint32 &uindex) const;
	BOOL CanUndergroundRailway(double &success, double &death) const;
	ORDER_RESULT UndergroundRailway(const MapPoint &point);
	BOOL CanInciteUprising(sint32 &uindex) const;
	ORDER_RESULT InciteUprising(const MapPoint &point);

	
	BOOL CanEstablishEmbassy(sint32 &uindex) const;
	BOOL CanEstablishEmbassy() const;
	ORDER_RESULT EstablishEmbassy(const MapPoint &point);

	BOOL CanThrowParty(sint32 &uindex) const;
	BOOL CanThrowParty() const;
	ORDER_RESULT ThrowParty(const MapPoint &point);

	BOOL CanBioInfect(double &chance, sint32 &uindex) const;
	BOOL CanBioInfect(double &chance) const;
	ORDER_RESULT BioInfect(const MapPoint &point);
	BOOL CanPlague(double &chance, sint32 &uindex) const;
	BOOL CanPlague(double &chance) const;
	ORDER_RESULT Plague(const MapPoint &point);
	BOOL CanNanoInfect(double &chance, sint32 &uindex) const;
	BOOL CanNanoInfect(double &chance) const;
	ORDER_RESULT NanoInfect(const MapPoint &point);

	BOOL CanConvertCity(double &chance, double &deathChance, sint32 &uindex) const;
	BOOL CanConvertCity(double &chance, double &deathChance) const;
	BOOL CanConvertCity(const MapPoint &point) const;
	ORDER_RESULT ConvertCity(const MapPoint &point);

	BOOL CanInterceptTrade(uint32 &uindex) const;
	ORDER_RESULT InterceptTrade();

	BOOL CanReformCity(sint32 &uindex, const MapPoint &point) const;
	BOOL CanReformCity() const;
	ORDER_RESULT ReformCity(const MapPoint &point);

	BOOL CanSellIndulgences(sint32 &uindex) const;
	BOOL CanSellIndulgences() const;
	ORDER_RESULT IndulgenceSale(const MapPoint &point);
	BOOL CanSoothsay(sint32 &uindex) const;
	BOOL CanSoothsay() const;
	ORDER_RESULT Soothsay(const MapPoint &point);

	BOOL CanAdvertise(sint32 &uindex) const;
	BOOL CanAdvertise() const;
	ORDER_RESULT Advertise(const MapPoint &point);

    void GetCurrentHP(sint32 &n, sint32 unit_type[100], 
        sint32 unit_hp[100]);

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
	BOOL CanExpel() const;

	BOOL CanCreatePark(sint32 &uindex) const;
	BOOL CanCreatePark() const;
	ORDER_RESULT CreatePark(const MapPoint &point);

	BOOL CanPillage(uint32 &uindex) const;
	BOOL CanPillage() const;
	ORDER_RESULT Pillage(BOOL test_ownership);

	BOOL CanInjoin(sint32 &uindex) const;
	BOOL CanInjoin() const;
	ORDER_RESULT Injoin(const MapPoint &point);

	BOOL GetFirstMoveThisTurn() const;

	BOOL HasLeftMap() const;
	BOOL CanNukeCity() const;

    void CurMinMovementPoints(double &cur) const; 
    void MinMovementPoints(double &cur) const; 

    void ThisMeansWAR(PLAYER_INDEX denfender);

	
	
	BOOL CanBombardTargetType(const CellUnitList & units) const;

	BOOL CanBombard(const MapPoint &point) const;
	BOOL CanBombard() const;
	BOOL BombardCity(const MapPoint &point, BOOL doAnimations);
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
	BOOL ExecuteOrders(bool propagate = true);
    void InformAI(const UNIT_ORDER_TYPE order_type, const MapPoint &pos);

	void ResumePatrol();
	void ForgetPatrol();

	BOOL IsOccupiedByForeigner(const MapPoint &pos);
    
	sint32 NumUnitsCanMoveIntoThisTransport(const CellUnitList &transports) const;
	BOOL CanMoveIntoTransport(const MapPoint &pos, CellUnitList &transports) const;
	BOOL CanMoveIntoThisTransport(const CellUnitList &transports) const;

	BOOL MoveIntoTransport(const MapPoint &pos, CellUnitList &transports);
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
    BOOL HasBeenAdded() const { return m_hasBeenAdded; }

	sint32 GetMinFuel();
    void CalcRemainingFuel(sint32 &num_tiles_to_half, sint32 &num_tiles_to_empty) const; 

	BOOL CanMove();

	
	BOOL CanBeachAssault() const;
	BOOL CanHearGossip() const;
	BOOL CanSlaveUprising() const;
	BOOL CanInciteRevolution( double &chance, double &eliteChance ) const;
	BOOL CanCloak() const;
	BOOL CanCreateFranchise( double &chance) const;
	BOOL CanAssasinateRuler( double &chance, double &eliteChance) const;
	BOOL CanStealTechnology( double &randChance, double &chance) const;
	BOOL CanInvestigateCity( double &chance, double &eliteChance) const;

	BOOL CanLaunch(sint32 &uindex) const;
	BOOL CanLaunch() const;
	ORDER_RESULT Launch(Order *order);
	void SetReentry(sint32 turns, MapPoint &pos);
	void Reenter();

	BOOL CanTarget(sint32 &uindex) const;
	BOOL CanTarget() const;
	ORDER_RESULT Target(Order *order);
	ORDER_RESULT ClearTarget();

	void SetTurnOver();
	BOOL TurnOver();

    BOOL CanAtLeastOneCargoUnloadAt(const MapPoint &old_pos, const MapPoint &dest_pos, const BOOL & used_vision) const;

	static BOOL GetInciteRevolutionCost( const MapPoint &point, sint32 &attackCost );
	static BOOL GetInciteUprisingCost( const MapPoint &point, sint32 &attackCost );

	
	

	


	BOOL ExecuteMoveOrder(Order *order);

	void CheckLoadSleepingCargoFromCity(Order *order);
	BOOL Move(WORLD_DIRECTION, Order *order);
	BOOL FinishMove(WORLD_DIRECTION d, MapPoint &newPos, Order *order);
	BOOL FinishAttack(Order *order);
	BOOL CheckSpecialUnitMove(const MapPoint &pos);
	BOOL MoveIntoForeigner(const MapPoint &pos);
	BOOL VerifyAttack(UNIT_ORDER_TYPE order, const MapPoint &pos,
					  sint32 defense_owner);

	BOOL ExertsZOC() const;
	void UpdateZOCForMove(const MapPoint &pos, WORLD_DIRECTION d);
	void RevealZOCUnits(const MapPoint &pos);
	BOOL MoveIntoCell(const MapPoint &pos,
					  UNIT_ORDER_TYPE order, WORLD_DIRECTION d);

	void MoveActors(const MapPoint &pos,
					UnitDynamicArray &revealedUnits,
					BOOL teleport = FALSE);
	void MoveUnits(const MapPoint &pos);
	void DeductMoveCost(const MapPoint &pos);

	void CheckTerrainEvents();

	void Disband();
	void Settle();
	


    
	BOOL ExecuteUnloadOrder(Order *order);

	void UpdateZOCForRemoval();
	BOOL ExecuteTeleportOrder(Order *order);
	void SetUnloadMovementPoints();

	void ActionSuccessful(SPECATTACK attack, Unit &unit, Unit &c);
	void ActionUnsuccessful(const MapPoint &point);

	BOOL ExecuteSpecialOrder(Order *order, BOOL &keepGoing);
	void AddSpecialActionUsed(Unit &who);

	BOOL CheckWasEnemyVisible(const MapPoint &pos, bool justCheck = false);
	BOOL DoLeaveOurLandsCheck(const MapPoint &newPos, UNIT_ORDER_TYPE order_type);

	void FinishUnloadOrder(Army &unloadingArmy, MapPoint &to_pt);

	


	Path *RemovePathedOrder();

	
	void CharacterizeArmy( bool & isspecial, 
		 			       sint32 & maxattack, 
						   sint32 & maxdefense, 
						   bool & cancapture,
						   bool & haszoc,
						   bool & canbombard) const;

	
	BOOL IsCivilian() const;

	
	void GetArmyStrength( sint32 & hitpoints, 
						  sint32 & defense_count, 
						  sint32 & ranged_count, 
						  sint32 & attack_strength,
						  sint32 & defense_strength,
						  sint32 & ranged_strength,
					      sint32 & total_value ) const;

	bool CanPerformSpecialAction() const;
	void CheckAddEventOrder();
	void IncrementOrderPath();


	
	
	

	
	bool CheckValidDestination(const MapPoint &dest) const;

	
	bool AtEndOfPath() const;

	
	bool GetNextPathPoint(MapPoint & next_pos) const;

	
	
	MBCHAR *GetDebugString(void) const { return m_debugString; }
	void SetDebugString(MBCHAR *string) { m_debugString = string; }
	uint8 GetDebugStringColor(void) const { return m_debugStringColor; }
	void SetDebugStringColor(uint8 color) { m_debugStringColor = color; }

	
	
	bool TestOrderAll(const OrderRecord *order_rec) const;
#if !defined(ACTIVISION_ORIGINAL)
	bool TestOrderAny(const OrderRecord * order_rec) const;
#endif
	
	
	bool TestOrderUnit(const OrderRecord *order_rec, uint32 unit_index) const;

	
	ORDER_TEST TestOrder(const OrderRecord * order_rec) const;

	
	ORDER_TEST TestOrderHere(const OrderRecord * order_rec, const MapPoint & pos) const;

	
	ORDER_TEST CargoTestOrderHere(const OrderRecord * order_rec, const MapPoint & pos) const;

	
	void PerformOrder(const OrderRecord * order_rec);

	
	void PerformOrderHere(const OrderRecord * order_rec, const Path *path);

	
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

	
	static bool UnitValidForOrder(const OrderRecord * order_rec, const UnitRecord *unit_rec);

	
	bool HasVeterans() const;

	
	
	
	

private:

	
	static sint32 *s_orderDBToEventMap;

	
	static void AssociateEventsWithOrdersDB();
};

#endif

