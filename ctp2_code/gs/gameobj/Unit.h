//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Unit
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
// CTP1_TRADE
// - Creates an executable with trade like in CTP1. Currently broken.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragma marked as Microsoft extension.
// - IsValid marked as const.
// - GetDisplayName added.
// - Made GetFuel method const - April 24th 2005 Martin Gühmann
// - Added NeedsRefueling method to remove code duplications.
//   - April 24th 2005 Martin Gühmann
// - Moved UnitValidForOrder from ArmyData to be able to access the Unit
//   properties as well. - April 24th 2005 Martin Gühmann
// - Removed some unsused method to removed some unused in methods in
//   CityData.. - Aug 6th 2005 Martin Gühmann
// - Removed another unused and unecessary function. (Aug 12th 2005 Martin Gühmann)
// - Moved sinking and upgrade functionality from ArmyData. (Dec 24th 2006 Martin Gühmann)
// - Modified sink to take a unit so the Slic identifies what sank - E 5-24-2007
// - Added an IsInVisionRange test. (25-Jan-2008 Martin Gühmann)
// - Added check move points option to CanAtLeastOneCargoUnloadAt (8-Feb-2008 Martin Gühmann).
// - Separated the Settle event drom the Settle in City event. (19-Feb-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __UNIT_H__
#define __UNIT_H__ 1

#include <string>

class Unit;

#include "UnitTypes.h"

#include "ID.h"
#include "dbtypes.h"
#include "Advances.h"       // AdvanceType
#include "AICause.h"        // CAUSE_...
#include "HappyTracker.h"   // HAPPY_REASON
#include "MapPoint.h"       // MapPoint
#include "Order.h"          // ORDER_RESULT
#include "OrderRecord.h"    // For order valid check
#include "player.h"         // PLAYER_INDEX

class CityData;
class UnitData;
class UnitRecord;
class UnitDynamicArray;
class SpriteState;
class UnitActor;
class TradeRoute;
class Resources;
class Army;
class CellUnitList;
class SlicObject;
class aui_Surface;

#define k_UNIT_VERSION_MAJOR    0
#define k_UNIT_VERSION_MINOR    0

class Unit : public ID
{
private:
	void RemoveAllReferences(const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy);

public:
	Unit () : ID() { return; } ;
	Unit (sint32 val) : ID (val) { return; };
	Unit (uint32 val) : ID (val) { return; };
	Unit (ID i) : ID (i.m_id) { return; };

	UnitData *operator -> () const { return AccessData(); }
#ifdef _DEBUG
	char const * GetText() const;
	void SetText(char * str);
#endif

	const UnitRecord * GetDBRec() const;
	const UnitData * GetData(void) const;
	UnitData * AccessData(void) const;
	void KillUnit(const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy);
	void Kill(const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy)
	{ KillUnit(cause, killedBy); }
	void FastKill(void);

	bool IsValid() const;

	PLAYER_INDEX GetOwner() const;
	uint32 GetOwnerBit() const;
	uint32 GetFlag0();
	sint32 GetType() const;

	const MBCHAR * GetName() const;
	bool IsInVisionRange(MapPoint &pos) const;
	bool NearestUnexplored(sint32 searchRadius, MapPoint &pos) const;
	bool NearestFriendlyCity(MapPoint &c) const;
	bool NearestFriendlyCity(Unit &u) const;
	bool NearestFriendlyCityWithRoom(MapPoint &p, sint32 needRoomFor,
	                                 Army army) const;

	sint32 ExertsMartialLaw() const;
	bool IsContentOverseas() const { return false; } // Should be implemented

	double GetHappiness() const;
	double GetHappySize() const;
	double GetHappyPollution() const;
	double GetHappyConquestDistress() const;
	double GetHappyEmpireDist() const;
	double GetHappyEnemyAction() const;
	double GetHappyPeace() const;
	double GetHappyWorkday() const;
	double GetHappyWages() const;
	double GetHappyRations() const;
	double GetHappyMartialLaw() const;
	double GetHappyPopEntertainment() const;
	double GetHappyImprovement() const;
	double GetHappyWonders() const;
	double GetHappyCrime() const;
	void CalcHappiness(sint32 &virtualGoldSpent, bool firstPass);
	sint32 PayWages(sint32 w);
	sint32 GetWagesNeeded() const;

	sint32 GetFirstMoveThisTurn() const;

	sint32 GetShieldHunger() const;
	sint32 GetGoldHunger() const;
	sint32 GetFoodHunger() const;

	bool    IsIgnoresZOC() const;
	bool    IsNoZoc() const;
	bool    GetMovementTypeLand() const;
	bool    GetMovementTypeSea() const;
	bool    GetMovementTypeShallowWater() const;
	bool    GetMovementTypeAir() const;
//  bool    GetAllTerrainAsImprovement() const;  //EMOD
	bool    GetMovementTypeSpace() const;
	bool    GetMovementTypeMountain() const;
	bool    IsSameMovementType(uint32 bit_field) const;
	bool    IsSubmarine() const;
	uint32  GetMovementType() const;
	double  GetMaxMovePoints() const;
	double  GetMovementPoints() const;
	void    SetMovementPoints(double m);
	bool    DeductMoveCost(const double cost, bool &out_of_fuel);
	void    GetPos(MapPoint &pos) const;
	MapPoint RetPos(void) const;
	bool MoveToPosition(const MapPoint &p, UnitDynamicArray &revealedUnits);
	bool SetPosition(const MapPoint &p, UnitDynamicArray &revealedUnits);
	void SetPosAndNothingElse(const MapPoint &p);

	void Launch(void);
	void MoveIt(const sint32 owner, const WORLD_DIRECTION d,
	            const sint32 is_manual);

	void SetIsInTransport(const Unit &transport);
	bool IsBeingTransported() const;

	bool CanAtLeastOneCargoUnloadAt(const MapPoint & unload_pos, const bool & used_vision, bool check_move_points = true) const;

	bool UnloadCargo(const MapPoint &unload_pos, Army &debark, sint32 &count, bool unloadSelected = false);

	bool IsMovePointsEnough(const MapPoint &pos) const;

	void GetCargoHP(sint32 &i, sint32 unit_type[100],
	                sint32 unit_hp[100]) const;

	void GetInserted (const Unit &transport);
	bool InsertCargo(const Unit &addme);

	bool CanBeachAssault() const;

	bool IsImmobile()const; //PFT
	bool CantGroup()const; //by E
	sint32 ResetMovement();

	bool CanExpelPop()const;
	bool CanBeExpelled()const;
	double GetAttack() const;
	double GetDefense() const;
	double GetDefense(const Unit &attacker) const;
	sint32 GetFirepower() const;
	double GetActiveDefenseRange() const;

	double GetHPModifier() const;
	double GetHP() const;
	void SetHP(const double newval);
	void SetHPToMax();
	void DeductHP(const double val);

	sint32 GetFuel() const;
	void SetFuel(sint32 val);
	void SetFuelToMax();
	bool GetUsedFuel (sint32 &fuel_remaining, sint32 &max_fuel) const;

	bool IsVeteran() const;
	void SetVeteran();  //copy and make for elite units
	void UnVeteran();  //copy and make for elite units
	void GetPosInArmy(MapPoint &p) const;
	void SetPosInArmy(const MapPoint &p);
	void GetDesiredPosInArmy(MapPoint &p) const;
	void SetDesiredPosInArmy(const MapPoint &p);

	bool CanInterceptTrade() const;
	bool CanRustle(CellUnitList &defender) const;
	bool CanConvertCity(Unit &city) const;
	bool CanBombard(CellUnitList &defender) const;
	bool CanCounterBombard(CellUnitList &defender) const;
	bool CanActivelyDefend(const Army &attacker) const;
	bool CanActivelyDefend(CellUnitList &attacker) const;

	void Bombard(const UnitRecord *rec, Unit defender, bool isCounterBombardment);
	bool Bombard(CellUnitList &defender, bool isCounterBombardment);
	void BombardOneRound(Unit &defender, double dbonus, double dmr);

	void FightOneRound(Unit did, double defenders_bonuse, double amr, double dmr);
	bool IsFlanker() const;
	sint32 GetZBRange() const;

	sint32 GetCargoCapacity() const;
	sint32 GetNumCarried() const;
	bool CanCarry(Unit u) const ;
	bool DeathEffectsHappy() const;

	sint32 GetSelect1SoundID(void);
	sint32 GetSelect2SoundID(void);
	sint32 GetMoveSoundID(void);
	sint32 GetAcknowledgeSoundID(void);
	sint32 GetCantMoveSoundID(void);
	sint32 GetAttackSoundID(void);

	sint32 GetWorkSoundID(void);
	sint32 GetVictorySoundID(void);
	sint32 GetDeathSoundID(void);
	sint32 GetLoadSoundID(void);
	sint32 GetUnloadSoundID(void);

	bool GetSpecialAttackInfo(SPECATTACK attack, sint32 *soundID, sint32 *spriteID);

	bool CanSettle(const MapPoint &pos, const bool settleOnCity = false) const;

	bool Settle();

	bool IsCantCaptureCity();
	void ResetCityOwner(const PLAYER_INDEX newo, sint32 is_conquest,
	                    const CAUSE_REMOVE_CITY cause);
	void ResetUnitOwner(const PLAYER_INDEX newo,
	                    CAUSE_REMOVE_ARMY cause);
	void InitializeCityData();

	void GamestateDebug();

	void BeginTurnCity(UnitDynamicArray &dead);

	bool BuildUnit(sint32 type);
	bool BuildImprovement(sint32 type);
	bool BuildWonder(sint32 wonder);
	bool BuildEndGame(sint32 type);

	void AddWonder(sint32 type);
	bool ChangeCurrentlyBuildingItem(sint32 category, sint32 item_type);

	bool HaveImprovement(sint32 type) const;
	uint64 GetImprovements() const;

	void DestroyCapitol();
	void DestroyImprovement(sint32 type);
	void NewGovernment(sint32 type);

	double GetDefendersBonus() const;
	sint32 ImprovementCanRefuel(const Unit &u) const;
	void UnsetIsInTransport();

	void GetPop(sint32 &p)const;
	void GetTurnsToNextPop(sint32 &p)const;//PFT 29 mar 05, show # turns until city next grows a pop
	bool IsCity() const;
	void DrawCityStats(aui_Surface *surf, sint32 x, sint32 y);

	void AddTradeRoute(TradeRoute &route, bool fromNetwork = false);
	void DelTradeRoute(TradeRoute route);

	ORDER_RESULT InterceptTrade();
	bool HasResource(const sint32 resource) const;
#ifdef CTP1_TRADE
	sint32 GetResourceCount(const sint32 resource) const;
	sint32 GetLocalResourceCount(const sint32 resource) const;
	const Resources *GetResources() const;
#endif
	sint32 GetNumTradeRoutes() const;
	void AddTradedResources(Resources &resources);
#ifdef CTP1_TRADE
	void LocalResources(Resources &resources);
#endif
	sint32 GetIncomingTrade() const;
	sint32 GetOutgoingTrade() const;

	void RemoveUnitVision();
	void AddUnitVision();
	void DoVision(UnitDynamicArray &revealedUnits);
	void UndoVision();
	uint32 GetVisibility() const;
	uint32 GetVisibilityClass() const;
	uint32 GetEverVisible() const;

	uint32 IsVisibilityClass(uint32 bit) const;
	uint32 GetRealVisibility() const;
	uint32 GetRadarVisibility() const;
	void SetRadar(sint32 owner);
	void ForceVisibleThisTurn(const PLAYER_INDEX to_me);
	void ForceVisibleDuration(const PLAYER_INDEX to_me, sint32 duration);
	void SetVisible(const PLAYER_INDEX to_me);

	double GetVisionRange() const;

	void BeginTurnVision(PLAYER_INDEX player);

	sint32 CollectProduction(sint32 &mil, sint32 &mat);
	void PayFederalProduction (double percent_military,
	                           sint32 &mil_paid,
	                           double percent_terrain,
	                           sint32 &mat_paid);
	void PayFederalProductionAbs (sint32 mil_paid,
	                              double percent_mat,
	                              sint32 &mat_paid);

	void SetMaterialContribution(bool on);
	bool GetMaterialContribution() const;
	bool GetMilitaryContribution() const;
	void SetMilitaryContribution(bool on);

	void SetIsProfessional(bool on);
	bool GetIsProfessional() const;

	bool GetNeedsNoSupport() const;
	sint32 GetShieldCost() const;

	void SetSpriteState(SpriteState *s);
	SpriteState * GetSpriteState() const;
	UnitActor * GetActor() const;

	SpriteState * GetPrimaryProjectileState();
	SpriteState * GetPrimaryProjectileEndState();
	sint32        GetPrimaryProjectilePath();

	SpriteState * GetSecondaryProjectileState();
	SpriteState * GetSecondaryProjectileEndState();
	sint32        GetSecondaryProjectilePath();

	bool IsEntrenched() const;
	bool IsEntrenching() const;
	bool CanEntrench() const;
	void Entrench();
	void Detrench();
	bool IsAsleep() const;
	void Sleep();
	void WakeUp();
	bool IsPatrolling() const;
	void SetPatrolling(bool);

	void BeginTurn();
	void EndTurn();
	void EndTurnCity();
	void AddHappyTimer(sint32 turns, double adjust, HAPPY_REASON reason);
	sint32 GetPollution() const;
	void CityToPark(sint32 agressor);

	void GetNuked(UnitDynamicArray &killList);

	bool SafeFromNukes() const;
	bool AttackFromSpaceship() const;
	bool HasAirport() const;
	void UseAirport();
	sint32 AirportLastUsed() const;

	bool HasCityWalls() const;
	bool HasForceField() const;

	ORDER_RESULT InvestigateCity(Unit c);
	ORDER_RESULT StealTechnology(Unit c, sint32 whichAdvance);
	ORDER_RESULT InciteRevolution(Unit c);
	ORDER_RESULT AssassinateRuler(Unit c);
	ORDER_RESULT InvestigateReadiness(Unit c);
	ORDER_RESULT NullifyWalls(Unit c);

	bool HasBeenSpiedUpon() const;
	void SetSpiedUpon();

	void CityNullifyWalls();

	ORDER_RESULT EstablishEmbassy(Unit &c);
	ORDER_RESULT ThrowParty(Unit c, sint32 gold);

	bool IsCapitol() const;

	void MakeFranchise(sint32 player);
	sint32 GetFranchiseOwner() const;
	sint32 GetProductionLostToFranchise() const;
	void SetFranchiseTurnsRemaining(sint32 turns);
	sint32 GetFranchiseTurnsRemaining() const;

	bool CanSee(const Army &army) const;

	void MakeCitizen(PopDBIndex pi, const MapPoint &pos, sint32 origOwner = -1);

	sint32 PopCount() const;
	sint32 CountSlaves() const;
	bool IsWatchful() const;
	void SetWatchful();
	void ModifySpecialAttackChance(UNIT_ORDER_TYPE attack, double &chance);

	void RemoveOneSlave(PLAYER_INDEX p);
	void DoUprising(UPRISING_CAUSE cause);

	void Place(const MapPoint &point);

	bool IsTempSlaveUnit() const;
	void SetTempSlaveUnit(bool b);

	void BioInfect(sint32 player);
	void NanoInfect(sint32 player);
	bool IsBioImmune() const;
	bool IsNanoImmune() const;
	bool IsNanoInfected() const;
	bool IsBioInfected() const;
	bool IsFranchised() const;
	bool IsConverted() const;

	void ConvertTo(sint32 player, CONVERTED_BY by);
	double TheologicalModifier() const;
	void Unconvert();
	sint32 IsConvertedTo() const;
	sint32 GetConvertedGold() const;

	bool IsCloaked() const;
	void Cloak();
	void Uncloak();

#if 0 // CTP1 space layer
	bool CanOpenRiftGate() const;
	bool HasOpenRiftGate(MapPoint &destination) const;
	bool OpenRiftGate(const MapPoint &destination);
	bool IsTravellingRift() const;
	void SetTravellingRift();
#endif

	sint32 GetStoredCityProduction() const;
	sint32 GetNetCityProduction() const;
	sint32 GetGrossCityProduction() const;
	sint32 GetStoredCityFood() const;
	sint32 GetNetCityFood() const;

	sint32 GetGrossCityFood() const;
	sint32 GetNetCityGold() const;
	sint32 GetGrossCityGold() const;

	bool BuyFront();
	void RemoveFront();

	sint32 GetOvertimeCost() const;
	sint32 HowMuchLonger() const;

	void SetCitySize(sint32 size);
	sint32 GetCombatUnits() const;

	bool CanBuildUnit(sint32 type) const;
	bool CanBuildBuilding(sint32 type) const;

	bool CanBuildCapitalization() const;
	bool CanBuildWonder(sint32 type) const;

	void Injoin(sint32 player);
	bool IsInjoined() const;
	sint32 InjoinedBy() const;

	double GetDistanceToCapitol() const;
	bool GetCurrentOrderString(StringId &id) const;

	double GetSupportCost() const;
	double GetOverseasDistress() const;

	void SupportBuildings();
	void CheckRiot();

	bool AiGetCargoMovementPoints(double &min_move_points,
	                              bool  &first) const;

#if 0
	void SetLastBattle(sint32 turn);
	sint32 GetLastBattle() const;
#endif
	bool IsInjured() const;

	void RecalculateResources();

	bool FightOneLineDanceRangedAttack(Unit &defender);
	bool FightOneLineDanceAssault(Unit &defender);

	sint32 CountTradeWith(PLAYER_INDEX) const;

	double IsProtectedFromSlavery() const;

	void NotifyAdvance(AdvanceType advance);
	void GiveCommand(UNIT_COMMAND command);

	void ExitWormhole(MapPoint &pos);
	bool HasLeftMap() const;

	sint32 FreeSlaves();

	void SetArmy(const Army &army);
	void ChangeArmy(const Army &army, CAUSE_NEW_ARMY cause);
	Army GetArmy() const;

	bool CanPerformSpaceTransitionNow() const;
	void IndicateSpaceTransition();

	void AddWonderHPBonus(sint32 amt);
	bool CanPlantNuke(const MapPoint &pos);
	bool CanMakePark(const MapPoint &pos);
	bool CanUndergroundRailway(const MapPoint &pos);
	bool CanConvert(const MapPoint &pos);
	bool CanEstablishEmbassy(const MapPoint &pos);
	bool CanCreateFranchise(const MapPoint &pos);
	bool CanAssasinateRuler(const MapPoint &pos);
	bool CanStealTechnology(const MapPoint &pos);
	bool CanInjoin(const MapPoint &pos);
	bool CanInciteRevolution(const MapPoint &pos);
	bool CanCauseUnhappiness(const MapPoint &pos);
	bool CanExpel(const MapPoint &pos);

	void AddEndGameObject(sint32 type);

	bool SendSlaveTo(Unit dest);

	void SetFullHappinessTurns(sint32 turns);

	void FinishBuilding();

	void DestroyRandomBuilding();
	void AddConversionUnhappiness(sint32 who);

	void BuildCapitalization();
	void BuildInfrastructure();

	bool Flag(uint32 flag) const;
	void SetFlag(uint32 flag);
	void ClearFlag(uint32 flag);

	const Unit &GetTransport() const;
	void RemoveTransportedUnit(const Unit &u);

	void DisbandCity();
	bool CanPerformSpecialAction() const;

	void AddGoods(SlicObject *obj);

	void UpdateZOCForRemoval();
	void UpdateZOCForInsertion();

	void CheckVisionRadius();

	CityData *GetCityData() const;
	CityData *CD() const { return GetCityData(); }

	std::string GetDisplayName(void) const;

	bool NeedsRefueling() const;
	bool UnitValidForOrder(const OrderRecord * order_rec) const;

	void SetType(const sint32 type);
	bool Sink(sint32 chance);

	bool IsHiddenNationality() const;  //emod for barb color 2-7-2009
	sint32 IsElite() const;
	void SetElite();
	void UnElite();
};

uint32 Unit_Unit_GetVersion(void);

#endif
