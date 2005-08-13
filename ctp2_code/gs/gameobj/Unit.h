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
// _MSC_VER
// - Compiler version (for the Microsoft C++ compiler only)
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
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __UNIT_H__ 
#define __UNIT_H__ 1

#include <string>

#include "ID.h"
#include "dbtypes.h"
#include "gstypes.h"
#include "AICause.h"
#include "OrderRecord.h" // For order valid check

class MapPoint;
class UnitData;
class UnitRecord; 
class UnitDynamicArray;
class SpriteState;
class UnitActor; 
class TradeRoute;
class aui_DirectSurface;
class Resources;
class Army;
class CellUnitList;
class SlicObject;
class CityData;

#define k_UNIT_VERSION_MAJOR    0
#define k_UNIT_VERSION_MINOR    0

enum CONVERTED_BY;
enum CAUSE_REMOVE_ARMY;
enum CAUSE_REMOVE_CITY; 
enum CAUSE_NEW_ARMY;
enum UNIT_ORDER_TYPE;
enum HAPPY_REASON;
enum UPRISING_CAUSE;
typedef sint32 AdvanceType;

enum UNIT_COMMAND;

enum SPECATTACK;
enum DEFAULT_PLACE_POP;
enum ORDER_RESULT;
enum WORLD_DIRECTION;

class Unit : public ID {
	void RemoveAllReferences(const CAUSE_REMOVE_ARMY cause, sint32 killedBy);

public:
	Unit () : ID() { return; } ;
	Unit (sint32 val) : ID (val) { return; };
	Unit (uint32 val) : ID (val) { return; };
	Unit (ID i) : ID (i.m_id) { return; };

	Unit (const int val) : ID (val) { return; };
	Unit (const unsigned int val) : ID (val) { return; };

	
	UnitData *operator -> () const { return AccessData(); }
#ifdef _DEBUG
	char * GetText();
	void SetText(char *str);
#endif

	const UnitRecord * GetDBRec() const;
	const UnitRecord * GetDBRec(UnitData *u) const;
	const UnitData * GetData(void) const;
	UnitData * AccessData(void) const; 
	void KillUnit(const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy);
	void Kill(const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy)
	{ KillUnit(cause, killedBy); }
	void FastKill(void);

	BOOL IsValid() const;

	PLAYER_INDEX GetOwner() const;
	uint32 GetOwnerBit() const;
	uint32 GetFlag0();
	sint32 GetType() const;
	
	const MBCHAR * GetName() const;
	BOOL NearestFriendlyCity(MapPoint &c) const;
	BOOL NearestFriendlyCity(Unit &u) const;
	BOOL NearestFriendlyCityWithRoom(MapPoint &p, sint32 needRoomFor,
	                                 Army *army) const;


	sint32 ExertsMartialLaw() const;
	sint32 IsContentOverseas() const { return FALSE; }


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
	void CalcHappiness(sint32 &virtualGoldSpent, BOOL firstPass); 
	sint32 PayWages(sint32 w);
	sint32 GetWagesNeeded();
	
	sint32 GetFirstMoveThisTurn() const;

	sint32 GetShieldHunger() const;
	sint32 GetFoodHunger() const;

	sint32 IsIgnoresZOC() const;
	sint32 IsNoZoc() const;
	sint32 GetMovementTypeLand() const;
	sint32 GetMovementTypeSea() const;
	sint32 GetMovementTypeShallowWater() const;
	sint32 GetMovementTypeAir() const;
	sint32 GetMovementTypeSpace() const;
	sint32 GetMovementTypeMountain() const;
	sint32 IsSameMovementType(uint32 bit_field) const;
	sint32 IsSubmarine() const;
	uint32 GetMovementType() const;


	double GetMaxMovePoints() const;
	double GetMovementPoints() const;
	void SetMovementPoints(double m);
	sint32 DeductMoveCost(const double cost, BOOL &out_of_fuel);
	void GetPos(MapPoint &pos) const;
	MapPoint RetPos(void) const;
	sint32 MoveToPosition(const MapPoint &p, UnitDynamicArray &revealedUnits,
	BOOL &revealed_unexplored);
	sint32 SetPosition(const MapPoint &p, UnitDynamicArray &revealedUnits,
	BOOL &revealed_unexplored);
	void SetPosAndNothingElse(const MapPoint &p);

	void Launch(void);
	void MoveIt(const sint32 owner, const WORLD_DIRECTION d,
	            const sint32 is_manual);

	void SetIsInTransport(const Unit &transport);
	sint32 IsBeingTransported() const;
	
	BOOL CanAtLeastOneCargoUnloadAt(const MapPoint & old_pos, const MapPoint & dest_pos, const BOOL & used_vision) const;

	BOOL UnloadCargo(const MapPoint &new_pos, Army &debark,
					 BOOL justOneUnit, Unit &theUnit);
	BOOL UnloadSelectedCargo(const MapPoint &new_pos, Army &debark);

	BOOL IsMovePointsEnough(const MapPoint &pos) const;

	void GetCargoHP(sint32 &i, sint32 unit_type[100],
	                sint32 unit_hp[100]);
	
	void GetInserted (const Unit &transport);
	BOOL InsertCargo(const Unit &addme);

	BOOL CanBeachAssault() const;

	bool IsImmobile()const; //PFT
	sint32 ResetMovement();

	BOOL CanExpelPop()const;
	BOOL CanBeExpelled()const;
	double GetAttack() const;
	double GetDefense() const;
	double GetDefense(const Unit &attacker) const;
	sint32 GetFirepower() const;
	double GetActiveDefenseRange() const;

	double GetHPModifier();
	double GetHP() const;
	void SetHP(const double newval);
	void SetHPToMax();
	void DeductHP(const double val);

	sint32 GetFuel() const;
	void SetFuel(sint32 val);
	void SetFuelToMax();
	BOOL GetUsedFuel (sint32 &fuel_remaining, sint32 &max_fuel) const;

	sint32 IsVeteran() const;
	void SetVeteran();
	void UnVeteran();
	void GetPosInArmy(MapPoint &p) const;
	void SetPosInArmy(const MapPoint &p);
	void GetDesiredPosInArmy(MapPoint &p) const;
	void SetDesiredPosInArmy(const MapPoint &p);

	sint32 CanInterceptTrade() const;
	sint32 CanRustle(CellUnitList &defender) const; 
	sint32 CanConvertCity(Unit &city) const;
	sint32 CanBombard(CellUnitList &defender) const;
	sint32 CanCounterBombard(CellUnitList &defender) const;
	sint32 CanActivelyDefend(Army &attacker) const;
	sint32 CanActivelyDefend(CellUnitList &attacker) const;

	void Bombard(const UnitRecord *rec, Unit defender, BOOL isCounterBombardment);
	BOOL Bombard(CellUnitList &defender, BOOL isCounterBombardment);
	void BombardOneRound(Unit &defender, double dbonus, double dmr);

	void FightOneRound(Unit did, double defenders_bonuse, double amr, double dmr);
	BOOL IsFlanker() const;
	sint32 GetZBRange() const;


	sint32 GetCargoCapacity() const;
	sint32 GetNumCarried() const;
	sint32 CanCarry(Unit u) const ;
    BOOL DeathEffectsHappy() const;

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

	BOOL GetSpecialAttackInfo(SPECATTACK attack, sint32 *soundID, sint32 *spriteID);


	BOOL CanSettle(const MapPoint &pos) const;

	BOOL IsSettleLand() const;
	BOOL IsSettleMountain() const;
	BOOL IsSettleWater() const;
	BOOL IsSettleSpace() const;

	sint32 Settle(); 

	sint32 IsCantCaptureCity();
	void ResetCityOwner(const PLAYER_INDEX newo, sint32 is_conquest,
	                    const CAUSE_REMOVE_CITY cause);
	void ResetUnitOwner(const PLAYER_INDEX newo,
	                    CAUSE_REMOVE_ARMY cause);
	void InitializeCityData();

	void GamestateDebug();

	void BeginTurnCity(UnitDynamicArray &dead);

	BOOL BuildUnit(sint32 type);
	BOOL BuildImprovement(sint32 type);
	BOOL BuildWonder(sint32 wonder);
	BOOL BuildEndGame(sint32 type);

	void AddWonder(sint32 type);
	BOOL ChangeCurrentlyBuildingItem(sint32 category, sint32 item_type);

	BOOL HaveImprovement(sint32 type) const;
	uint64 GetImprovements() const;

	void DestroyCapitol();
	void DestroyImprovement(sint32 type);
	void NewGovernment(sint32 type);

	double GetDefendersBonus() const;
	sint32 ImprovementCanRefuel(const Unit &u) const;
	void UnsetIsInTransport();

	void GetPop(sint32 &p)const;
	void GetTurnsToNextPop(sint32 &p)const;//PFT 29 mar 05, show # turns until city next grows a pop
	sint32 IsCity() const;
	void DrawCityStats(aui_DirectSurface *surf, sint32 x, sint32 y);

	
	void AddTradeRoute(TradeRoute &route, BOOL fromNetwork = FALSE);
	void DelTradeRoute(TradeRoute route);

	ORDER_RESULT InterceptTrade();
	BOOL HasResource(const sint32 resource) const;
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
	void AddUnitVision(BOOL &revealed);
	void DoVision(UnitDynamicArray &revealedUnits);
	void UndoVision();
	uint32 GetVisibility() const;
	uint32 GetVisibilityClass() const;
	uint32 GetEverVisible() const;

	BOOL IsVisibilityClass(uint32 bit) const;
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


	void SetMaterialContribution(BOOL on);
	BOOL GetMaterialContribution() const;
	BOOL GetMilitaryContribution() const;
	void SetMilitaryContribution(BOOL on);

	void SetIsProfessional(BOOL on);
	BOOL GetIsProfessional() const;

	BOOL GetNeedsNoSupport() const;
	BOOL GetShieldCost() const; 

	
	void SetSpriteState(SpriteState *s);
	SpriteState * GetSpriteState();
	void SetActor(UnitActor *a);
	UnitActor * GetActor();

	SpriteState * GetPrimaryProjectileState();
	SpriteState * GetPrimaryProjectileEndState();
	sint32        GetPrimaryProjectilePath();

	SpriteState * GetSecondaryProjectileState();
	SpriteState * GetSecondaryProjectileEndState();
	sint32        GetSecondaryProjectilePath();

	BOOL IsEntrenched() const;
	BOOL IsEntrenching() const;
	BOOL CanEntrench() const;
	void Entrench();
	void Detrench();
	BOOL IsAsleep() const;
	void Sleep();
	void WakeUp();
	BOOL IsPatrolling() const;
	void SetPatrolling(BOOL);

	void BeginTurn();
	void EndTurn();
	void EndTurnCity();
	void AddHappyTimer(sint32 turns, double adjust, HAPPY_REASON reason);
	sint32 GetPollution() const;
	void CityToPark(sint32 agressor);

	void GetNuked(UnitDynamicArray &killList);

	BOOL SafeFromNukes() const;
	BOOL AttackFromSpaceship() const;
	BOOL HasAirport() const;
	void UseAirport();
	sint32 AirportLastUsed() const;

	BOOL HasCityWalls() const;
	BOOL HasForceField() const;

	ORDER_RESULT InvestigateCity(Unit &c);
	ORDER_RESULT StealTechnology(Unit &c, sint32 whichAdvance);
	ORDER_RESULT InciteRevolution(Unit &c);
	ORDER_RESULT AssassinateRuler(Unit &c);
	ORDER_RESULT InvestigateReadiness(Unit &c);
	ORDER_RESULT NullifyWalls(Unit &c);

	BOOL HasBeenSpiedUpon() const;
	void SetSpiedUpon();

	void CityNullifyWalls();
	
	ORDER_RESULT EstablishEmbassy(Unit &c);
	ORDER_RESULT ThrowParty(Unit &c, sint32 gold);

	BOOL IsCapitol() const;

	void MakeFranchise(sint32 player);
	sint32 GetFranchiseOwner() const;
	void SetFranchiseTurnsRemaining(sint32 turns);
	sint32 GetFranchiseTurnsRemaining() const;

	BOOL CanSee(Army &army) const;

#ifdef _DEBUG
	void SetIgnoreHappiness(BOOL v);
#endif


	void MakeCitizen(sint32 pi, const MapPoint &pos, sint32 origOwner = -1);
	
	sint32 PopCount() const;
	sint32 CountSlaves() const;
	BOOL IsWatchful() const;
	void SetWatchful();
	void ModifySpecialAttackChance(UNIT_ORDER_TYPE attack, double &chance);

	void RemoveOneSlave(PLAYER_INDEX p);
	void DoUprising(UPRISING_CAUSE cause);

	void Place(const MapPoint &point);

	BOOL IsTempSlaveUnit() const;
	void SetTempSlaveUnit(BOOL b);

	void BioInfect(sint32 player);
	void NanoInfect(sint32 player);
	BOOL IsBioImmune() const;
	BOOL IsNanoImmune() const;
	BOOL IsNanoInfected() const;
	BOOL IsBioInfected() const;
	BOOL IsFranchised() const;
	BOOL IsConverted() const;

	void ConvertTo(sint32 player, CONVERTED_BY by);
	double TheologicalModifier() const;
	void Unconvert();
	sint32 IsConvertedTo() const;

	BOOL IsCloaked() const;
	void Cloak();
	void Uncloak();

#if 0
	BOOL CanOpenRiftGate() const;
	BOOL HasOpenRiftGate(MapPoint &destination) const;
	BOOL OpenRiftGate(const MapPoint &destination);
	BOOL IsTravellingRift() const;
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

	BOOL BuyFront();
	void RemoveFront();

	sint32 GetOvertimeCost() const;
	sint32 HowMuchLonger() const;

	void SetCitySize(sint32 size);
	sint32 GetCombatUnits() const;

	BOOL CanBuildUnit(sint32 type) const;
	BOOL CanBuildBuilding(sint32 type) const;
	
	BOOL CanBuildCapitalization() const;
	BOOL CanBuildWonder(sint32 type) const;
	void RemoveWonderFromQueue(sint32 type);

	void Injoin(sint32 player);
	BOOL IsInjoined() const;
	sint32 InjoinedBy() const;

	double GetDistanceToCapitol() const;
	BOOL GetCurrentOrderString(StringId &id) const;
	
	
	double GetSupportCost() const;
	double GetOverseasDistress() const;

	void SupportBuildings();
	void CheckRiot();

	BOOL AiGetCargoMovementPoints(double &min_move_points, 
	                              BOOL  &first) const;

#if 0
	void SetLastBattle(sint32 turn);
	sint32 GetLastBattle() const;
#endif
	BOOL IsInjured() const;

	void RecalculateResources();
	
	
	BOOL FightOneLineDanceRangedAttack(Unit &defender);
	BOOL FightOneLineDanceAssault(Unit &defender);

	sint32 CountTradeWith(PLAYER_INDEX) const;

	BOOL IsProtectedFromSlavery() const;

	void NotifyAdvance(AdvanceType advance);
	void GiveCommand(UNIT_COMMAND command);

	void ExitWormhole(MapPoint &pos);
	BOOL HasLeftMap() const;

	void ContributeScience(double incomePercent,
	                       double &addscience,
	                       double &subgold);
	sint32 FreeSlaves();

	void SetArmy(const Army &army);
	void ChangeArmy(const Army &army, CAUSE_NEW_ARMY cause);
	Army GetArmy() const;

	BOOL CanPerformSpaceTransitionNow() const;
	void IndicateSpaceTransition();

	void AddWonderHPBonus(sint32 amt);
	sint32 CanPlantNuke(const MapPoint &pos);
	sint32 CanMakePark(const MapPoint &pos);
	sint32 CanUndergroundRailway(const MapPoint &pos);
	sint32 CanConvert(const MapPoint &pos);
	sint32 CanEstablishEmbassy(const MapPoint &pos);
	sint32 CanCreateFranchise(const MapPoint &pos);
	sint32 CanAssasinateRuler(const MapPoint &pos);
	sint32 CanStealTechnology(const MapPoint &pos);
	sint32 CanInjoin(const MapPoint &pos);
	sint32 CanInciteRevolution(const MapPoint &pos);
	sint32 CanCauseUnhappiness(const MapPoint &pos);
	sint32 CanExpel(const MapPoint &pos);




	void AddEndGameObject(sint32 type);

	BOOL SendSlaveTo(Unit &dest);

	void SetFullHappinessTurns(sint32 turns);

	void FinishBuilding();

	void DestroyRandomBuilding();
	void AddConversionUnhappiness(sint32 who);

	void BuildCapitalization();
	void BuildInfrastructure();

	BOOL Flag(uint32 flag) const;
	void SetFlag(uint32 flag);
	void ClearFlag(uint32 flag);

	const Unit &GetTransport() const;
	void RemoveTransportedUnit(const Unit &u);

	void DisbandCity();
	BOOL CanPerformSpecialAction() const;

	void AddGoods(SlicObject *obj);

	void UpdateZOCForRemoval();
	void UpdateZOCForInsertion();

	void CheckVisionRadius();

	CityData *GetCityData() const;
	CityData *CD() { return GetCityData(); }

	std::string	GetDisplayName(void) const;

	bool NeedsRefueling() const;
	bool UnitValidForOrder(const OrderRecord * order_rec) const;
};

uint32 Unit_Unit_GetVersion(void) ;
#else

class Unit;

#endif
