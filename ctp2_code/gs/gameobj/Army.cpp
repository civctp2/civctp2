//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - Added IsWounded method
// - Added CanTransport and IsCivilian methods.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Globals.h"
#include "Army.h"
#include "ArmyPool.h"
#include "ArmyData.h"
#include "player.h"
#include "SelItem.h"

extern SelectedItem *g_selected_item;
extern Player **g_player;

bool Army::IsValid() const
{
	return g_theArmyPool->IsValid(m_id) == TRUE;
}

void Army::KillArmy()
{
	if(GetData()->DontKillCount()) {
		AccessData()->SetNeedToKill();
		return;
	}

	CAUSE_REMOVE_ARMY cause = GetRemoveCause();

	AccessData()->StopPirating();

	Army tmp(*this);
	tmp.SetRemoveCause(cause); 
	tmp.RemoveAllReferences();
}

void Army::RemoveAllReferences()
{
	Assert(Num() < 1);

	if(GetOwner() >= 0 && GetData()->HasBeenAdded()) {
		g_player[GetOwner()]->RemoveArmy(*this, GetRemoveCause(),
										 GetKiller());
	}
	g_selected_item->RegisterRemovedArmy(GetOwner(), *this);

	g_theArmyPool->Del(*this);
}

void Army::FastKill()
{
	if(g_theArmyPool->IsValid(m_id)) {
		sint32 r = g_theArmyPool->Del(*this);
		Assert(r);
	}
}

const ArmyData *Army::GetData() const
{
	return g_theArmyPool->GetArmy(m_id);
}

ArmyData *Army::AccessData() const
{
	return g_theArmyPool->AccessArmy(m_id);
}

Unit & Army::operator [] (const sint32 i)
{
	return AccessData()->Access(i);
}

const Unit &Army::operator [] (const sint32 i) const
{
	return GetData()->Get(i);
}

Unit & Army::Access(const sint32 i)
{
	return AccessData()->Access(i);
}

sint32 Army::Num() const
{
	return GetData()->Num();
}

sint32 Army::Del(const Unit &unit)
{
	sint32 r = AccessData()->Del(unit);
	if(Num() < 1) {
		Kill();
	}
	return r;
}

sint32 Army::DelIndex(sint32 i)
{
	sint32 r = AccessData()->DelIndex(i);
	if(Num() < 1) {
		Kill();
	}
	return r;
}

const Unit &Army::Get(sint32 i) const
{
	return GetData()->Get(i);
}

bool Army::Insert(const Unit &id)
{
	return AccessData()->Insert(id);
}

bool Army::IsPresent(Unit &u)
{
	return AccessData()->IsPresent(u);
}

PLAYER_INDEX Army::GetOwner() const
{
	return GetData()->GetOwner();
}

void Army::SetOwner(PLAYER_INDEX p)
{
	AccessData()->SetOwner(p);
}

void Army::GetPos(MapPoint &pos) const
{
	AccessData()->GetPos(pos);
}


uint32 Army::GetMovementType() const
{
	return GetData()->GetMovementType();
}

bool Army::CanEnter(const MapPoint &point) const
{
	return AccessData()->CanEnter(point);
}


bool Army::IsAtLeastOneMoveLand() const
{
	return AccessData()->IsAtLeastOneMoveLand();
}

bool Army::IsAtLeastOneMoveWater() const
{
	return AccessData()->IsAtLeastOneMoveWater();
}

bool Army::IsAtLeastOneMoveShallowWater() const
{
	return AccessData()->IsAtLeastOneMoveShallowWater();
}

bool Army::IsAtLeastOneMoveAir() const
{
	return AccessData()->IsAtLeastOneMoveAir();
}


bool Army::IsAtLeastOneMoveMountain() const
{
	return AccessData()->IsAtLeastOneMoveMountain();
}

bool Army::GetMovementTypeAir() const
{
	return GetData()->GetMovementTypeAir();
}

bool Army::IsMovePointsEnough(double cost)
{
	return GetData()->IsMovePointsEnough(cost);
}

bool Army::IsIgnoresZOC() const
{
	return GetData()->IsIgnoresZOC();
}


bool Army::CanSettle(const MapPoint &pos) const
{
	return GetData()->CanSettle(pos);
}

bool Army::CanSettle() const
{
	return GetData()->CanSettle();
}


bool Army::IsOccupiedByForeigner(const MapPoint &pos)
{
	return AccessData()->IsOccupiedByForeigner(pos);
}

sint32 Army::IsEnemy(PLAYER_INDEX owner) const
{
	return GetData()->IsEnemy(owner);
}

sint32 Army::IsEnemy(Unit defender) const
{
	return GetData()->IsEnemy(defender);
}

sint32 Army::IsEnemy(Army &defender) const
{
	return GetData()->IsEnemy(defender.GetOwner());
}


double Army::GetHPModifier()
{
	return AccessData()->GetHPModifier();
}

bool Army::IsAsleep() const
{
	return GetData()->IsAsleep();
}

bool Army::IsEntrenched() const
{
	return GetData()->IsEntrenched();
}

bool Army::IsEntrenching() const
{
	return GetData()->IsEntrenching();
}

bool Army::CanEntrench()
{
	return AccessData()->CanEntrench();
}

bool Army::CanPatrol() const
{
	return GetData()->CanPatrol();
}

bool Army::IsPatrolling() const
{
	return GetData()->IsPatrolling();
}

void Army::SetPatrolling(bool p)
{
	Assert(false);
	
}


void Army::GetActors(sint32 &n, UnitActor **moveActor, UnitActor *butnotthis)
{
	Assert(false);

}

bool Army::GetTopVisibleUnitOfMoveType
(
	PLAYER_INDEX const	looker,
	uint32 const		moveType,
	sint32 &			maxi,
	bool &				isResyncReported
) const
{
	return GetData()->GetTopVisibleUnitOfMoveType
						(looker, moveType, maxi, isResyncReported);
}

Unit Army::GetTopVisibleUnit(PLAYER_INDEX const looking_player) const
{
	return GetData()->GetTopVisibleUnit(looking_player);
}

void Army::ForceVisibleThisTurn(const PLAYER_INDEX to_me)
{
	AccessData()->ForceVisibleThisTurn(to_me);
}

void Army::BeginTurn()
{
	AccessData()->BeginTurn();
}

bool Army::CanFight(CellUnitList &defender)
{
	return AccessData()->CanFight(defender);
}

bool Army::IsVisible(sint32 player)
{
	return AccessData()->IsVisible(player);
}


sint32 Army::GetCost()
{
	return AccessData()->GetCost();
}


bool Army::CanFranchise(double &chance, sint32 &uindex)
{
	return AccessData()->CanFranchise(chance, uindex);
}

bool Army::CanSue(sint32 &uindex)
{
	return AccessData()->CanSue(uindex);
}

bool Army::CanSue()
{
	return AccessData()->CanSue();
}

bool Army::CanBeSued()
{
	return AccessData()->CanBeSued();
}

bool Army::CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amount,
							   sint32 &uindex)
{
	return AccessData()->CanCauseUnhappiness(chance, timer, amount, uindex);
}

bool Army::CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amount)
{
	return AccessData()->CanCauseUnhappiness(chance, timer, amount);
}

bool Army::CanPlantNuke(double &chance, double &escape_chance,
				  sint32 &uindex)
{
	return AccessData()->CanPlantNuke(chance, escape_chance, uindex);
}

bool Army::CanPlantNuke(double &chance, double &escape_chance)
{
	return AccessData()->CanPlantNuke(chance, escape_chance);
}


bool Army::CanSlaveRaid(double &success, double &death, 
						sint32 &timer, sint32 &amount)
{
	return AccessData()->CanSlaveRaid(success, death,
									  timer, amount);
}
bool Army::IsSlaveRaidPossible(const MapPoint &point, 
							   double &success, 
							   double &death, 
							   sint32 &timer, 
							   sint32 &amount,
							   sint32 &uindex, 
							   BOOL &target_is_city, 
							   Unit &target_city, 
							   Unit &home_city)
{
	return AccessData()->IsSlaveRaidPossible(point,
											 success,
											 death,
											 timer,
											 amount,
											 uindex,
											 target_is_city,
											 target_city,
											 home_city);
}


bool Army::CanEnslaveSettler(sint32 &uindex)
{
	return AccessData()->CanEnslaveSettler(uindex);
}

bool Army::CanUndergroundRailway(double &success, double &death,
								 sint32 &uindex)
{
	return AccessData()->CanUndergroundRailway(success, death, uindex);
}

bool Army::CanUndergroundRailway(double &success, double &death)
{
	return AccessData()->CanUndergroundRailway(success, death);
}

bool Army::CanInciteUprising(sint32 &uindex)
{
	return AccessData()->CanInciteUprising(uindex);
}


bool Army::CanEstablishEmbassy(sint32 &uindex)
{
	return AccessData()->CanEstablishEmbassy(uindex);
}

bool Army::CanEstablishEmbassy()
{
	return AccessData()->CanEstablishEmbassy();
}

bool Army::CanBioInfect(double &chance)
{
	return AccessData()->CanBioInfect(chance);
}

bool Army::CanNanoInfect(double &chance, sint32 &uindex)
{
	return AccessData()->CanNanoInfect(chance, uindex);
}

bool Army::CanNanoInfect(double &chance)
{
	return AccessData()->CanNanoInfect(chance);
}

bool Army::CanConvertCity(double &chance, double &deathChance, sint32 &uindex)
{
	return AccessData()->CanConvertCity(chance, deathChance, uindex);
}

bool Army::CanConvertCity(double &chance, double &deathChance)
{
	return AccessData()->CanConvertCity(chance, deathChance);
}

bool Army::CanConvertCity(const MapPoint &point)
{
	return AccessData()->CanConvertCity(point);
}


bool Army::CanReformCity(sint32 &uindex, const MapPoint &point)
{
	return AccessData()->CanReformCity(uindex, point);
}

bool Army::CanReformCity()
{
	return AccessData()->CanReformCity();
}

bool Army::CanSellIndulgences(sint32 &uindex)
{
	return AccessData()->CanSellIndulgences(uindex);
}

bool Army::CanSellIndulgences()
{
	return AccessData()->CanSellIndulgences();
}

bool Army::CanSoothsay(sint32 &uindex)
{
	return AccessData()->CanSoothsay(uindex);
}

bool Army::CanSoothsay()
{
	return AccessData()->CanSoothsay();
}
















bool Army::AbleToPlantNukeTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToPlantNukeTarget(point, uindex);
}


bool Army::AbleToMakeParkTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToMakeParkTarget(point, uindex);
}

bool Army::AbleToUndergroundRailwayTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToUndergroundRailwayTarget(point, uindex);
}

bool Army::AbleToConvertTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToConvertTarget(point, uindex);
}

bool Army::AbleToEstablishEmbassyTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToEstablishEmbassyTarget(point, uindex);
}

bool Army::AbleToCreateFranchiseTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToCreateFranchiseTarget(point, uindex);
}

bool Army::AbleToAssasinateRulerTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToAssasinateRulerTarget(point, uindex);
}

bool Army::AbleToStealTechnologyTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToStealTechnologyTarget(point, uindex);
}

bool Army::AbleToInjoinTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToInjoinTarget(point, uindex);
}

bool Army::AbleToInciteRevolutionTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToInciteRevolutionTarget(point, uindex);
}

bool Army::AbleToCauseUnhappinessTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToCauseUnhappinessTarget(point, uindex);
}

bool Army::AbleToExpelTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToExpelTarget(point, uindex);
}

bool Army::CanExpel()
{
	return AccessData()->CanExpel();
}

bool Army::CanCreatePark(sint32 &uindex)
{
	return AccessData()->CanCreatePark(uindex);
}

bool Army::CanCreatePark()
{
	return AccessData()->CanCreatePark();
}

#if 0
bool Army::CanTravelRift(const MapPoint &newpos, MapPoint &dest)
{
	return AccessData()->CanTravelRift(newpos, dest);
}

bool Army::CanCreateRift(sint32 &uindex)
{
	return AccessData()->CanCreateRift(uindex);
}

bool Army::CanCreateRift()
{
	return AccessData()->CanCreateRift();
}
#endif

bool Army::CanPillage(uint32 &uindex) const
{
	return AccessData()->CanPillage(uindex);
}

bool Army::CanPillage() const
{
	return AccessData()->CanPillage();
}

bool Army::CanInjoin(sint32 &uindex)
{
	return AccessData()->CanInjoin(uindex);
}

bool Army::CanInjoin()
{
	return AccessData()->CanInjoin();
}

bool Army::GetFirstMoveThisTurn() const
{
	return AccessData()->GetFirstMoveThisTurn();
}


bool Army::HasLeftMap() const
{
	return AccessData()->HasLeftMap();
}


void Army::CurMinMovementPoints(double &cur) const
{
	AccessData()->CurMinMovementPoints(cur);
}

void Army::MinMovementPoints(double &cur) const
{
	AccessData()->MinMovementPoints(cur);
}


bool Army::CanBombard(const MapPoint &point)
{
	return AccessData()->CanBombard(point);
}

bool Army::CanBombard()
{
	return AccessData()->CanBombard();
}

bool Army::CanInterceptTrade(uint32 &uindex) const
{
	return AccessData()->CanInterceptTrade(uindex);
}

void Army::Fight(CellUnitList &defender)
{
	AccessData()->Fight(defender);
}

void Army::SetRemoveCause(CAUSE_REMOVE_ARMY cause)
{
	AccessData()->SetRemoveCause(cause);
}

CAUSE_REMOVE_ARMY Army::GetRemoveCause() const
{
	return GetData()->GetRemoveCause();
}




sint32 Army::NumOrders() const
{
	return AccessData()->NumOrders();
}

const Order *Army::GetOrder(sint32 index) const
{
	return GetData()->GetOrder(index);
}

void Army::AddOrders(UNIT_ORDER_TYPE order, Path *path)
{
	AccessData()->AddOrders(order, path);
}

void Army::AddOrders(UNIT_ORDER_TYPE order, const MapPoint &point)
{
	AccessData()->AddOrders(order, point);
}

void Army::AddOrders(UNIT_ORDER_TYPE order)
{
	AccessData()->AddOrders(order);
}

void Army::AddOrders(UNIT_ORDER_TYPE order, Path *path, const MapPoint &point, sint32 arg)
{
	AccessData()->AddOrders(order, path, point, arg);
}

void Army::AutoAddOrders(UNIT_ORDER_TYPE order, Path *path, const MapPoint &point, sint32 arg)
{
	AccessData()->AutoAddOrders(order, path, point, arg);
}

void Army::AutoAddOrdersWrongTurn(UNIT_ORDER_TYPE order, Path *path, const MapPoint &point, sint32 arg)
{
	AccessData()->AutoAddOrdersWrongTurn(order, path, point, arg);
}

void Army::ClearOrders()
{
	AccessData()->ClearOrders();
}

void Army::ExecuteOrders()
{
	AccessData()->ExecuteOrders();
}

void Army::ResumePatrol()
{
	AccessData()->ResumePatrol();
}

void Army::ForgetPatrol()
{
	AccessData()->ForgetPatrol();
}

bool Army::CanMoveIntoCell(const MapPoint &pos, bool &zocViolation)
{
	bool alliedCity;
	return AccessData()->CanMoveIntoCell(pos, zocViolation, false, alliedCity);
}

bool Army::CanMoveIntoTransport(const MapPoint &pos, CellUnitList &transports)
{
	return AccessData()->CanMoveIntoTransport(pos, transports);
}

sint32 Army::NumUnitsCanMoveIntoThisTransport(const Army &transports)
{
	return AccessData()->NumUnitsCanMoveIntoThisTransport(*transports.AccessData());
}

bool Army::CanMoveIntoThisTransport(CellUnitList &transports)
{
	return AccessData()->CanMoveIntoThisTransport(transports);
}

bool Army::CanMoveIntoThisTransport(const Army &transports)
{
	return AccessData()->CanMoveIntoThisTransport(*transports.AccessData());
}


void Army::ResetPos()
{
	AccessData()->ResetPos();
}

void Army::IndicateAdded() const
{
	AccessData()->IndicateAdded();
}

PLAYER_INDEX Army::GetKiller() const
{
	return GetData()->GetKiller();
}

void Army::SetKiller(PLAYER_INDEX who)
{
	AccessData()->SetKiller(who);
}

void Army::AddDeath(const Unit &u, CAUSE_REMOVE_ARMY cause, PLAYER_INDEX who)
{
	AccessData()->AddDeath(u, cause, who);
}


sint32 Army::GetMinFuel() const
{
	return AccessData()->GetMinFuel();
}

void Army::CalcRemainingFuel(sint32 &num_tiles_to_half, sint32 &num_tiles_to_empty) const 
{
    GetData()->CalcRemainingFuel(num_tiles_to_half, num_tiles_to_empty); 
}

bool Army::CanMove() const
{
	return AccessData()->CanMove();
}


bool Army::CanBeachAssault()
{
	return AccessData()->CanBeachAssault();
}

bool Army::CanHearGossip()
{
	return AccessData()->CanHearGossip();
}

bool Army::CanSlaveUprising()
{
	return AccessData()->CanSlaveUprising();
}

bool Army::CanInciteRevolution( double &chance, double &eliteChance )
{
	return AccessData()->CanInciteRevolution( chance, eliteChance );
}

bool Army::CanCloak()
{
	return AccessData()->CanCloak();
}


bool Army::CanCreateFranchise( double &chance )
{
	return AccessData()->CanCreateFranchise( chance );
}

bool Army::CanAssasinateRuler( double &chance, double &eliteChance )
{
	return AccessData()->CanAssasinateRuler( chance, eliteChance );
}

bool Army::CanStealTechnology( double &randChance, double &chance )
{
	return AccessData()->CanStealTechnology( randChance, chance );
}

bool Army::CanInvestigateCity( double &chance, double &eliteChance )
{
	return AccessData()->CanInvestigateCity( chance, eliteChance );
}

void Army::SetTurnOver()
{
	AccessData()->SetTurnOver();
}

bool Army::TurnOver() const
{
	return AccessData()->TurnOver();
}

bool Army::CanAdvertise() const
{
	return GetData()->CanAdvertise();
}

void Army::GetCurrentHP
(
	sint32 &	count,
	sint32		unit_type[MAX_UNIT_COUNT],
	sint32		unit_hp[MAX_UNIT_COUNT] 
) const
{
	GetData()->GetCurrentHP(count, unit_type, unit_hp);
}

bool Army::CanTransport(void) const
{
	return GetData()->CanTransport();
}

bool Army::IsCivilian(void) const
{
	return GetData()->IsCivilian();
}

bool Army::IsWounded(void) const
{
	return GetData()->IsWounded();
}

bool Army::CanAtLeastOneCargoUnloadAt(const MapPoint &old_pos, const MapPoint &dest_pos, const bool & use_vision)
{
	return AccessData()->CanAtLeastOneCargoUnloadAt(old_pos, dest_pos, use_vision);
}

bool Army::ExecutedThisTurn() const
{
	return (AccessData()->m_flags & k_CULF_EXECUTED_THIS_TURN) != 0;
}

void Army::InformAI(const UNIT_ORDER_TYPE order_type, const MapPoint &pos)
{
	AccessData()->InformAI(order_type, pos);
}

bool Army::GetInciteRevolutionCost( const MapPoint &point, sint32 &attackCost )
{
	return ArmyData::GetInciteRevolutionCost( point, attackCost );
}

bool Army::GetInciteUprisingCost( const MapPoint &point, sint32 &attackCost )
{
	return ArmyData::GetInciteUprisingCost( point, attackCost );
}

