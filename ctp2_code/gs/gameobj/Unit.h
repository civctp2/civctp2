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
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __UNIT_H__ 
#define __UNIT_H__ 1

#include <string>

class Unit;

enum OPTIMISE_STATE {
	OPTIMISE_STATE_NONE,
	OPTIMISE_STATE_FOOD,
	OPTIMISE_STATE_PRODUCTION,
	OPTIMISE_STATE_GOLD,
	OPTIMISE_STATE_HAPPINESS,
	OPTIMISE_STATE_SCIENCE,
	OPTIMISE_STATE_INVALID,
} ;

enum CITY_ATTITUDE { 
   CITY_ATTITUDE_CONTENT,
   CITY_ATTITUDE_WE_LOVE_THE_KING, 
   CITY_ATTITUDE_HAPPY,
   CITY_ATTITUDE_DISORDER,
};

enum RADIUS_OP {
    RADIUS_OP_UKNOWN = -1, 
	RADIUS_OP_REMOVE_IMPROVEMENTS = 0,
	RADIUS_OP_KILL_UNITS,
	RADIUS_OP_RESET_OWNER,
	RADIUS_OP_KILL_TILE,
	RADIUS_OP_ADD_GOODS,
	RADIUS_OP_COUNT_GOODS,
};

enum CONVERTED_BY {
	CONVERTED_BY_CLERIC,
	CONVERTED_BY_TELEVANGELIST,
	CONVERTED_BY_NOTHING
};

enum UPRISING_CAUSE {
	UPRISING_CAUSE_NONE,
	UPRISING_CAUSE_SLAVE_STARVED,
	UPRISING_CAUSE_UNGUARDED_SLAVES,
	UPRISING_CAUSE_INCITED,
	UPRISING_CAUSE_INTERNAL, 
};

enum POP_TYPE {
	POP_WORKER,
	POP_SCIENTIST,
	POP_ENTERTAINER,
	POP_FARMER,
	POP_LABORER,
	POP_MERCHANT,
	POP_SLAVE,
	POP_MAX
};

enum UNIT_COMMAND {
	UNIT_COMMAND_NEAREST_CITY,
	UNIT_COMMAND_NEAREST_FORT,
	UNIT_COMMAND_NEAREST_AIRFIELD,
	UNIT_COMMAND_ENTRENCH,
	UNIT_COMMAND_SLEEP
};

enum UNIT_ORDER_TYPE {
	UNIT_ORDER_NONE,
	UNIT_ORDER_MOVE,
	UNIT_ORDER_PATROL,
	UNIT_ORDER_CIRCULAR_PATROL,
	UNIT_ORDER_ENTRENCH,
	UNIT_ORDER_SLEEP,
	UNIT_ORDER_DETRENCH,
	UNIT_ORDER_UNLOAD,
	UNIT_ORDER_MOVE_TO,
	UNIT_ORDER_TELEPORT_TO,

	
	UNIT_ORDER_EXPEL_TO,
	UNIT_ORDER_GROUP,
	UNIT_ORDER_UNGROUP,
	UNIT_ORDER_INVESTIGATE_CITY,
	UNIT_ORDER_NULLIFY_WALLS,
	UNIT_ORDER_STEAL_TECHNOLOGY,
	UNIT_ORDER_INCITE_REVOLUTION,
	UNIT_ORDER_ASSASSINATE,
	UNIT_ORDER_INVESTIGATE_READINESS,
	UNIT_ORDER_BOMBARD,

	
	UNIT_ORDER_SUE,
	UNIT_ORDER_FRANCHISE,
	UNIT_ORDER_SUE_FRANCHISE,
	UNIT_ORDER_EXPEL,
	UNIT_ORDER_ESTABLISH_EMBASSY,
	UNIT_ORDER_THROW_PARTY,
	UNIT_ORDER_CAUSE_UNHAPPINESS,
	UNIT_ORDER_PLANT_NUKE,
	UNIT_ORDER_SLAVE_RAID,
	UNIT_ORDER_ENSLAVE_SETTLER,

	
	UNIT_ORDER_UNDERGROUND_RAILWAY,
	UNIT_ORDER_INCITE_UPRISING,
	UNIT_ORDER_BIO_INFECT,
	UNIT_ORDER_NANO_INFECT,
	UNIT_ORDER_CONVERT,
	UNIT_ORDER_REFORM,
	UNIT_ORDER_INDULGENCE,
	UNIT_ORDER_SOOTHSAY,
	UNIT_ORDER_CREATE_PARK,
	UNIT_ORDER_PILLAGE,

	
	UNIT_ORDER_INJOIN,
	UNIT_ORDER_INTERCEPT_TRADE,
	UNIT_ORDER_PARADROP_MOVE,
	UNIT_ORDER_SET_UNLOAD_MOVEMENT_POINTS,
	UNIT_ORDER_GROUP_UNIT,
	UNIT_ORDER_DISBAND,
	UNIT_ORDER_FINISH_ATTACK,
	UNIT_ORDER_UNLOAD_ONE_UNIT,
	UNIT_ORDER_BOARD_TRANSPORT,
	UNIT_ORDER_WAKE_UP,

	
	UNIT_ORDER_PILLAGE_UNCONDITIONALLY,
	UNIT_ORDER_MOVE_THEN_UNLOAD,
	UNIT_ORDER_ADVERTISE,
    UNIT_ORDER_INFORM_AI_CAPTURE_CITY, 
	UNIT_ORDER_UNLOAD_SELECTED_STACK,
	UNIT_ORDER_ADD_EVENT,
	UNIT_ORDER_SETTLE, 
	UNIT_ORDER_LAUNCH,
	UNIT_ORDER_TARGET,
	UNIT_ORDER_CLEAR_TARGET,

	
	UNIT_ORDER_PLAGUE,
	UNIT_ORDER_VICTORY_MOVE,

	UNIT_ORDER_MAX
};

enum SPECATTACK {
	SPECATTACK_NONE = -1,

	SPECATTACK_NUKE,
	SPECATTACK_HEARGOSSIP,
	SPECATTACK_THROWPARTY,
	SPECATTACK_ESTABLISHEMBASSY,
	SPECATTACK_INCITEREVOLUTION,
	SPECATTACK_BOMBCABINET,
	SPECATTACK_CREATEFRANCHISE,
	SPECATTACK_CAUSEUNHAPPINESS,
	SPECATTACK_CONDUCTHIT,
	SPECATTACK_BIOTERROR,
	SPECATTACK_NANOTERROR,
	SPECATTACK_SLAVERAID,
	SPECATTACK_ENSLAVESETTLER,
	SPECATTACK_SLAVEUPRISING,
	SPECATTACK_FREESLAVES,
	SPECATTACK_SELLINDULGENCE,
	SPECATTACK_CONVERTCITY,
	SPECATTACK_PLANTNUKE,
	SPECATTACK_SOOTHSAY,
	SPECATTACK_CREATEPARK,
	SPECATTACK_INJOIN,
	SPECATTACK_SPY,
	SPECATTACK_STEALTECH,
	SPECATTACK_REVOLUTION,
	SPECATTACK_REFORMCITY,

	SPECATTACK_MAX
};

#include "ID.h"
#include "dbtypes.h"
#include "directions.h"     // WORLD_DIRECTION
#include "Advances.h"       // AdvanceType
#include "AICause.h"        // CAUSE_...
#include "HappyTracker.h"   // HAPPY_REASON
#include "MapPoint.h"       // MapPoint
#include "Order.h"          // ORDER_RESULT
#include "OrderRecord.h"    // For order valid check
#include "Player.h"         // PLAYER_INDEX

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

	sint32 IsIgnoresZOC() const;
	sint32 IsNoZoc() const;
	sint32 GetMovementTypeLand() const;
	sint32 GetMovementTypeSea() const;
	sint32 GetMovementTypeShallowWater() const;
	sint32 GetMovementTypeAir() const;
	sint32 GetAllTerrainAsImprovement() const;  //EMOD
	sint32 GetMovementTypeSpace() const;
	sint32 GetMovementTypeMountain() const;
	sint32 IsSameMovementType(uint32 bit_field) const;
	sint32 IsSubmarine() const;
	uint32 GetMovementType() const;


	double GetMaxMovePoints() const;
	double GetMovementPoints() const;
	void SetMovementPoints(double m);
	sint32 DeductMoveCost(const double cost, bool &out_of_fuel);
	void GetPos(MapPoint &pos) const;
	MapPoint RetPos(void) const;
	bool MoveToPosition(const MapPoint &p, UnitDynamicArray &revealedUnits,
	                    bool &revealed_unexplored);
	bool SetPosition(const MapPoint &p, UnitDynamicArray &revealedUnits,
	                 bool &revealed_unexplored);
	void SetPosAndNothingElse(const MapPoint &p);

	void Launch(void);
	void MoveIt(const sint32 owner, const WORLD_DIRECTION d,
	            const sint32 is_manual);

	void SetIsInTransport(const Unit &transport);
	sint32 IsBeingTransported() const;
	
	bool CanAtLeastOneCargoUnloadAt(const MapPoint & old_pos, const MapPoint & dest_pos, const bool & used_vision) const;

	bool UnloadCargo(const MapPoint &new_pos, Army &debark,
			 bool justOneUnit, const Unit &theUnit);
	bool UnloadSelectedCargo(const MapPoint &new_pos, Army &debark);

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

	double GetHPModifier();
	double GetHP() const;
	void SetHP(const double newval);
	void SetHPToMax();
	void DeductHP(const double val);

	sint32 GetFuel() const;
	void SetFuel(sint32 val);
	void SetFuelToMax();
	bool GetUsedFuel (sint32 &fuel_remaining, sint32 &max_fuel) const;

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
	sint32 CanActivelyDefend(const Army &attacker) const;
	sint32 CanActivelyDefend(CellUnitList &attacker) const;

	void Bombard(const UnitRecord *rec, Unit defender, bool isCounterBombardment);
	bool Bombard(CellUnitList &defender, bool isCounterBombardment);
	void BombardOneRound(Unit &defender, double dbonus, double dmr);

	void FightOneRound(Unit did, double defenders_bonuse, double amr, double dmr);
	bool IsFlanker() const;
	sint32 GetZBRange() const;


	sint32 GetCargoCapacity() const;
	sint32 GetNumCarried() const;
	sint32 CanCarry(Unit u) const ;
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


	bool CanSettle(const MapPoint &pos) const;

	sint32 Settle(); 

	sint32 IsCantCaptureCity();
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
	sint32 IsCity() const;
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
	void AddUnitVision(bool &revealed);
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
	void SetActor(UnitActor *a);
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
	void SetFranchiseTurnsRemaining(sint32 turns);
	sint32 GetFranchiseTurnsRemaining() const;

	bool CanSee(const Army &army) const;

#ifdef _DEBUG
	void SetIgnoreHappiness(bool v);
#endif


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

	bool IsProtectedFromSlavery() const;

	void NotifyAdvance(AdvanceType advance);
	void GiveCommand(UNIT_COMMAND command);

	void ExitWormhole(MapPoint &pos);
	bool HasLeftMap() const;

	void ContributeScience(double incomePercent,
	                       double &addscience,
	                       double &subgold);
	sint32 FreeSlaves();

	void SetArmy(const Army &army);
	void ChangeArmy(const Army &army, CAUSE_NEW_ARMY cause);
	Army GetArmy() const;

	bool CanPerformSpaceTransitionNow() const;
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

	BOOL SendSlaveTo(Unit dest);

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
	CityData *CD() { return GetCityData(); }

	std::string	GetDisplayName(void) const;

	bool NeedsRefueling() const;
	bool UnitValidForOrder(const OrderRecord * order_rec) const;

	void SetType(const sint32 type);  //EMOD
};

uint32 Unit_Unit_GetVersion(void) ;

#endif
