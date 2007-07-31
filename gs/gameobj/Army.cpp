

#include "c3.h"
#include "globals.h"
#include "Army.h"
#include "ArmyPool.h"
#include "ArmyData.h"
#include "Player.h"
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

BOOL Army::Insert(const Unit &id)
{
	return AccessData()->Insert(id);
}

BOOL Army::IsPresent(Unit &u)
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

BOOL Army::CanEnter(const MapPoint &point) const
{
	return AccessData()->CanEnter(point);
}


BOOL Army::IsAtLeastOneMoveLand() const
{
	return AccessData()->IsAtLeastOneMoveLand();
}

BOOL Army::IsAtLeastOneMoveWater() const
{
	return AccessData()->IsAtLeastOneMoveWater();
}

BOOL Army::IsAtLeastOneMoveShallowWater() const
{
	return AccessData()->IsAtLeastOneMoveShallowWater();
}

BOOL Army::IsAtLeastOneMoveAir() const
{
	return AccessData()->IsAtLeastOneMoveAir();
}


BOOL Army::IsAtLeastOneMoveMountain() const
{
	return AccessData()->IsAtLeastOneMoveMountain();
}

BOOL Army::GetMovementTypeAir() const
{
	return GetData()->GetMovementTypeAir();
}

BOOL Army::IsMovePointsEnough(double cost)
{
	return GetData()->IsMovePointsEnough(cost);
}

BOOL Army::IsIgnoresZOC() const
{
	return GetData()->IsIgnoresZOC();
}


BOOL Army::CanSettle(const MapPoint &pos) const
{
	return GetData()->CanSettle(pos);
}

BOOL Army::CanSettle() const
{
	return GetData()->CanSettle();
}


BOOL Army::IsOccupiedByForeigner(const MapPoint &pos)
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
	return GetData()->IsEnemy(defender);
}


double Army::GetHPModifier()
{
	return AccessData()->GetHPModifier();
}

BOOL Army::IsAsleep() const
{
	return GetData()->IsAsleep();
}

BOOL Army::IsEntrenched() const
{
	return GetData()->IsEntrenched();
}

BOOL Army::IsEntrenching() const
{
	return GetData()->IsEntrenching();
}

BOOL Army::CanEntrench()
{
	return AccessData()->CanEntrench();
}

BOOL Army::CanPatrol() const
{
	return GetData()->CanPatrol();
}

BOOL Army::IsPatrolling() const
{
	return GetData()->IsPatrolling();
}

void Army::SetPatrolling(BOOL p)
{
	Assert(FALSE);
	
}


void Army::GetActors(sint32 &n, UnitActor **moveActor, UnitActor *butnotthis)
{
	Assert(FALSE);

}


BOOL Army::GetTopVisibleUnitOfMoveType(const sint32 looking_player, const uint32 move, sint32 &maxi) const
{
	return GetData()->GetTopVisibleUnitOfMoveType(looking_player, move, maxi);
}

Unit Army::GetTopVisibleUnit(const sint32 looking_player) const
{
	return AccessData()->GetTopVisibleUnit(looking_player);
}

void Army::ForceVisibleThisTurn(const PLAYER_INDEX to_me)
{
	AccessData()->ForceVisibleThisTurn(to_me);
}

void Army::BeginTurn()
{
	AccessData()->BeginTurn();
}

BOOL Army::CanFight(CellUnitList &defender)
{
	return AccessData()->CanFight(defender);
}

BOOL Army::IsVisible(sint32 player)
{
	return AccessData()->IsVisible(player);
}


sint32 Army::GetCost()
{
	return AccessData()->GetCost();
}


BOOL Army::CanFranchise(double &chance, sint32 &uindex)
{
	return AccessData()->CanFranchise(chance, uindex);
}

BOOL Army::CanSue(sint32 &uindex)
{
	return AccessData()->CanSue(uindex);
}

BOOL Army::CanSue()
{
	return AccessData()->CanSue();
}

BOOL Army::CanBeSued()
{
	return AccessData()->CanBeSued();
}

BOOL Army::CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amount,
							   sint32 &uindex)
{
	return AccessData()->CanCauseUnhappiness(chance, timer, amount, uindex);
}

BOOL Army::CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amount)
{
	return AccessData()->CanCauseUnhappiness(chance, timer, amount);
}

BOOL Army::CanPlantNuke(double &chance, double &escape_chance,
				  sint32 &uindex)
{
	return AccessData()->CanPlantNuke(chance, escape_chance, uindex);
}

BOOL Army::CanPlantNuke(double &chance, double &escape_chance)
{
	return AccessData()->CanPlantNuke(chance, escape_chance);
}


BOOL Army::CanSlaveRaid(double &success, double &death, 
						sint32 &timer, sint32 &amount)
{
	return AccessData()->CanSlaveRaid(success, death,
									  timer, amount);
}
BOOL Army::IsSlaveRaidPossible(const MapPoint &point, 
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


BOOL Army::CanEnslaveSettler(sint32 &uindex)
{
	return AccessData()->CanEnslaveSettler(uindex);
}

BOOL Army::CanUndergroundRailway(double &success, double &death,
								 sint32 &uindex)
{
	return AccessData()->CanUndergroundRailway(success, death, uindex);
}

BOOL Army::CanUndergroundRailway(double &success, double &death)
{
	return AccessData()->CanUndergroundRailway(success, death);
}

BOOL Army::CanInciteUprising(sint32 &uindex)
{
	return AccessData()->CanInciteUprising(uindex);
}


BOOL Army::CanEstablishEmbassy(sint32 &uindex)
{
	return AccessData()->CanEstablishEmbassy(uindex);
}

BOOL Army::CanEstablishEmbassy()
{
	return AccessData()->CanEstablishEmbassy();
}

BOOL Army::CanBioInfect(double &chance)
{
	return AccessData()->CanBioInfect(chance);
}

BOOL Army::CanNanoInfect(double &chance, sint32 &uindex)
{
	return AccessData()->CanNanoInfect(chance, uindex);
}

BOOL Army::CanNanoInfect(double &chance)
{
	return AccessData()->CanNanoInfect(chance);
}

BOOL Army::CanConvertCity(double &chance, double &deathChance, sint32 &uindex)
{
	return AccessData()->CanConvertCity(chance, deathChance, uindex);
}

BOOL Army::CanConvertCity(double &chance, double &deathChance)
{
	return AccessData()->CanConvertCity(chance, deathChance);
}

BOOL Army::CanConvertCity(const MapPoint &point)
{
	return AccessData()->CanConvertCity(point);
}


BOOL Army::CanReformCity(sint32 &uindex, const MapPoint &point)
{
	return AccessData()->CanReformCity(uindex, point);
}

BOOL Army::CanReformCity()
{
	return AccessData()->CanReformCity();
}

BOOL Army::CanSellIndulgences(sint32 &uindex)
{
	return AccessData()->CanSellIndulgences(uindex);
}

BOOL Army::CanSellIndulgences()
{
	return AccessData()->CanSellIndulgences();
}

BOOL Army::CanSoothsay(sint32 &uindex)
{
	return AccessData()->CanSoothsay(uindex);
}

BOOL Army::CanSoothsay()
{
	return AccessData()->CanSoothsay();
}
















BOOL Army::AbleToPlantNukeTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToPlantNukeTarget(point, uindex);
}


BOOL Army::AbleToMakeParkTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToMakeParkTarget(point, uindex);
}

BOOL Army::AbleToUndergroundRailwayTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToUndergroundRailwayTarget(point, uindex);
}

BOOL Army::AbleToConvertTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToConvertTarget(point, uindex);
}

BOOL Army::AbleToEstablishEmbassyTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToEstablishEmbassyTarget(point, uindex);
}

BOOL Army::AbleToCreateFranchiseTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToCreateFranchiseTarget(point, uindex);
}

BOOL Army::AbleToAssasinateRulerTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToAssasinateRulerTarget(point, uindex);
}

BOOL Army::AbleToStealTechnologyTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToStealTechnologyTarget(point, uindex);
}

BOOL Army::AbleToInjoinTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToInjoinTarget(point, uindex);
}

BOOL Army::AbleToInciteRevolutionTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToInciteRevolutionTarget(point, uindex);
}

BOOL Army::AbleToCauseUnhappinessTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToCauseUnhappinessTarget(point, uindex);
}

BOOL Army::AbleToExpelTarget(const MapPoint &point, sint32 &uindex)
{
	return AccessData()->AbleToExpelTarget(point, uindex);
}

BOOL Army::CanExpel()
{
	return AccessData()->CanExpel();
}

BOOL Army::CanCreatePark(sint32 &uindex)
{
	return AccessData()->CanCreatePark(uindex);
}

BOOL Army::CanCreatePark()
{
	return AccessData()->CanCreatePark();
}

#if 0
BOOL Army::CanTravelRift(const MapPoint &newpos, MapPoint &dest)
{
	return AccessData()->CanTravelRift(newpos, dest);
}

BOOL Army::CanCreateRift(sint32 &uindex)
{
	return AccessData()->CanCreateRift(uindex);
}

BOOL Army::CanCreateRift()
{
	return AccessData()->CanCreateRift();
}
#endif

BOOL Army::CanPillage(uint32 &uindex) const
{
	return AccessData()->CanPillage(uindex);
}

BOOL Army::CanPillage() const
{
	return AccessData()->CanPillage();
}

BOOL Army::CanInjoin(sint32 &uindex)
{
	return AccessData()->CanInjoin(uindex);
}

BOOL Army::CanInjoin()
{
	return AccessData()->CanInjoin();
}

BOOL Army::GetFirstMoveThisTurn() const
{
	return AccessData()->GetFirstMoveThisTurn();
}


BOOL Army::HasLeftMap() const
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


BOOL Army::CanBombard(const MapPoint &point)
{
	return AccessData()->CanBombard(point);
}

BOOL Army::CanBombard()
{
	return AccessData()->CanBombard();
}

BOOL Army::CanInterceptTrade(uint32 &uindex) const
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

BOOL Army::CanMoveIntoCell(const MapPoint &pos, BOOL &zocViolation)
{
	BOOL alliedCity;
    return AccessData()->CanMoveIntoCell(pos, zocViolation, FALSE, alliedCity);
}

BOOL Army::CanMoveIntoTransport(const MapPoint &pos, CellUnitList &transports)
{
	return AccessData()->CanMoveIntoTransport(pos, transports);
}

sint32 Army::NumUnitsCanMoveIntoThisTransport(const Army &transports)
{
	return AccessData()->NumUnitsCanMoveIntoThisTransport(*transports.AccessData());
}

BOOL Army::CanMoveIntoThisTransport(CellUnitList &transports)
{
	return AccessData()->CanMoveIntoThisTransport(transports);
}

BOOL Army::CanMoveIntoThisTransport(const Army &transports)
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

BOOL Army::CanMove() const
{
	return AccessData()->CanMove();
}


BOOL Army::CanBeachAssault()
{
	return AccessData()->CanBeachAssault();
}

BOOL Army::CanHearGossip()
{
	return AccessData()->CanHearGossip();
}

BOOL Army::CanSlaveUprising()
{
	return AccessData()->CanSlaveUprising();
}

BOOL Army::CanInciteRevolution( double &chance, double &eliteChance )
{
	return AccessData()->CanInciteRevolution( chance, eliteChance );
}

BOOL Army::CanCloak()
{
	return AccessData()->CanCloak();
}


BOOL Army::CanCreateFranchise( double &chance )
{
	return AccessData()->CanCreateFranchise( chance );
}

BOOL Army::CanAssasinateRuler( double &chance, double &eliteChance )
{
	return AccessData()->CanAssasinateRuler( chance, eliteChance );
}

BOOL Army::CanStealTechnology( double &randChance, double &chance )
{
	return AccessData()->CanStealTechnology( randChance, chance );
}

BOOL Army::CanInvestigateCity( double &chance, double &eliteChance )
{
	return AccessData()->CanInvestigateCity( chance, eliteChance );
}

void Army::SetTurnOver()
{
	AccessData()->SetTurnOver();
}

BOOL Army::TurnOver() const
{
	return AccessData()->TurnOver();
}

BOOL Army::CanAdvertise() const
{
	return GetData()->CanAdvertise();
}

void Army::GetCurrentHP(sint32 &n, sint32 unit_type[100], 
        sint32 unit_hp[100])
{
    AccessData()->GetCurrentHP(n, unit_type, unit_hp); 
}


BOOL Army::CanAtLeastOneCargoUnloadAt(const MapPoint &old_pos, const MapPoint &dest_pos, const BOOL & use_vision)
{
    return AccessData()->CanAtLeastOneCargoUnloadAt(old_pos, dest_pos, use_vision);
}

BOOL Army::ExecutedThisTurn() const
{
	return (AccessData()->m_flags & k_CULF_EXECUTED_THIS_TURN) != 0;
}

void Army::InformAI(const UNIT_ORDER_TYPE order_type, const MapPoint &pos)
{
	AccessData()->InformAI(order_type, pos);
}

BOOL Army::GetInciteRevolutionCost( const MapPoint &point, sint32 &attackCost )
{
	return ArmyData::GetInciteRevolutionCost( point, attackCost );
}

BOOL Army::GetInciteUprisingCost( const MapPoint &point, sint32 &attackCost )
{
	return ArmyData::GetInciteUprisingCost( point, attackCost );
}

