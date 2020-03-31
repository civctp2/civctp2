//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Unit data
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
// - Added GetTurnsToNextPop(sint32 &p)const; PFT 29 mar 05, to help show # turns until city grows
// - Added IsImmobile( )const; PFT 10 apr 05, to identify immobile units
// - Removed some unsused method to removed some unused in methods in
//   CityData.. - Aug 6th 2005 Martin Gühmann
// - Removed another unused and unecessary function. (Aug 12th 2005 Martin Gühmann)
// - Total fuel, total move points and total hp calculation moved into their own
//   methods. (Dec 24th 2006 Martin Gühmann)
// - Added IsReligion bools
// - Added check move points option to CanAtLeastOneCargoUnloadAt (8-Feb-2008 Martin Gühmann).
// - Separated the Settle event drom the Settle in City event. (19-Feb-2008 Martin Gühmann)
// - Added GetRanged, and GetDefCounterAttack for new combat option. (07-Mar-2009 Maq)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __UNIT_DATA_H__
#define __UNIT_DATA_H__ 1

class UnitData;
class VisibilityDurationArray;

#include "Unit.h"
#include "GameObj.h"
#include "MapPoint.h"
#include "Unit.h"
#include "citydata.h"
#include "Army.h"
#include "Order.h"          // ORDER_RESULT
#include "player.h"         // k_MAX_PLAYERS
#include "UnitActor.h"      // UnitActor
#include "UnitRecord.h"

class UnitList;
class citydata;
class SpriteState;
class TradeRoute;
class Installation;
class UnitDynamicArray;
class Resources;
template <class T> class DynamicArray;
class SlicObject;
class BitMask;

#define k_UNITDATA_VERSION_MAJOR    0
#define k_UNITDATA_VERSION_MINOR    0

#define k_DEFAULT_VIS_DURATION_SIZE 2

class VisibilityDurationArray
{
public:
	VisibilityDurationArray()
	{
		sint32 i;

		m_array_index = 0;
		for(i = 0; i < k_DEFAULT_VIS_DURATION_SIZE; i++) {
			m_array[i] = 0;
		}
	}
	~VisibilityDurationArray() {

	}

	uint32 GetCurrentVisibility(sint32 player) {
		return m_array[GetArrayIndex(player) & (1 << player)];
	}
	void NextTurn(sint32 player) {
		m_array[GetArrayIndex(player)] &= ~(1 << player);
		IncArrayIndex(player);
	}

	void SetCurrentVisibility(const sint32 player) {
		m_array[GetArrayIndex(player)] |= (1 << player);
	}

	void SetVisibleDuration(const sint32 player, sint32 duration) {
		Assert(m_array != NULL);
		Assert(duration >= 0);

		Assert(duration <= k_DEFAULT_VIS_DURATION_SIZE);

		sint32 index = GetArrayIndex(player);
		for(sint32 i = 0; i < duration; i++) {
			m_array[index] |= (1 << player);
			index++;
			if(index >= k_DEFAULT_VIS_DURATION_SIZE)
				index = 0;
		}
	}

	uint8 GetArrayIndex(sint32 player) {
		return uint8((m_array_index & (1 << player)) >> player);
	}

	void IncArrayIndex(sint32 player) {
		m_array_index ^= (1 << player);
	}

	void Serialize(CivArchive &archive);
private:
	uint32 m_array[k_DEFAULT_VIS_DURATION_SIZE];
	uint32 m_array_index;


	friend class NetUnit;
};

#define k_UDF_FIRST_MOVE                             0x00000001
#define k_UDF_IS_VET                                 0x00000002
#define k_UDF_IS_IN_TRANSPORT                        0x00000004
#define k_UDF_PACMAN                                 0x00000008
#define k_UDF_IS_ENTRENCHED                          0x00000010
#define k_UDF_IS_ENTRENCHING                         0x00000020
#define k_UDF_IS_ASLEEP                              0x00000040
#define k_UDF_IS_PATROLLING                          0x00000080
#define k_UDF_IS_PROFESSIONAL                        0x00000100
#define k_UDF_IS_CLOAKED                             0x00000200
#define k_UDF_FOUGHT_THIS_TURN                       0x00000400
#define k_UDF_IS_ELITE                               0x00000800 // Added by E - maybe use professional for elite? Be carefull what you want to add you have only one bit left.

#define k_UDF_HAS_LEFT_MAP                           0x00001000
#define k_UDF_ALREADY_PERFORMED_SPACE_TRANSITION     0x00002000
#define k_UDF_TEMP_SLAVE_UNIT                        0x00004000
#define k_UDF_VISION_ADDED                           0x00008000
#define k_UDF_USED_ACTIVE_DEFENSE                    0x00010000
#define k_UDF_USED_SPECIAL_ACTION_THIS_TURN          0x00020000
#define k_UDF_USED_SPECIAL_ACTION_JUST_NOW           0x00040000
#define k_UDF_TURN_OVER                              0x00080000
#define k_UDF_RETURNED_FROM_WORMHOLE                 0x00100000
#define k_UDF_IN_WORMHOLE                            0x00200000
#define k_UDF_WAS_TOP_UNIT_BEFORE_BATTLE             0x00400000
#define k_UDF_TEMP_TRANSPORT_SELECT                  0x00800000
#define k_UDF_CANT_BE_ATTACKED                       0x01000000
#define k_UDF_CANT_RIOT_OR_REVOLT                    0x02000000
#define k_UDF_IN_SPACE                               0x04000000
#define k_UDF_HAS_TARGET                             0x08000000
#define k_UDF_MAD_LAUNCHED                           0x10000000
#define k_UDF_TELEPORT_DEATH                         0x20000000
#define k_UDF_BEACH_ASSAULT_LEGAL                    0x40000000

class UnitData : public GameObj,
                 public CityRadiusCallback
{

private:

	//
	// Sort the members by size to save memory
	// Size of UnitData before sorting: 120 byte
	// Size after sorting: 120 byte
	//

	// 20 byte from GameObj
	// 4 byte from CityRadiusCallback

	// 8 byte
	double              m_hp;
	double              m_movement_points;

	// 4 byte in a 32 bit program, 8 byte in a 64 bit program
	UnitDynamicArray   *m_cargo_list;
	CityData           *m_city_data;
	SpriteState        *m_sprite_state;
	UnitActor          *m_actor;
	BitMask            *m_roundTheWorldMask;

	// 4 byte
	Unit                m_transport;
	Unit                m_target_city;
	Army                m_army;

	PLAYER_INDEX        m_owner;
	sint32              m_fuel;
	sint32              m_type;
	uint32              m_visibility;
	uint32              m_ever_visible;
	uint32              m_temp_visibility;
	uint32              m_radar_visibility;
	uint32              m_flags;

	MapPoint            m_pos;

	// 12 byte
	// Putting this before it screws up the alignment and we need more
	VisibilityDurationArray m_temp_visibility_array;

	friend class NetCity;
	friend class NetUnit;
	friend class NetUnitMove;

#ifdef _PLAYTEST

	friend class CreateImprovementCommand;
#endif

public:

#ifdef _DEBUG
	char m_text[80];
#endif

	UnitData(const sint32 t, const sint32 trans_t, const Unit &i,
	         const PLAYER_INDEX o, const MapPoint &center_point, const Unit hc,
	         UnitActor *actor = NULL);
	UnitData(const sint32 t, const sint32 trans_t, const Unit &i,
	         const PLAYER_INDEX o, const MapPoint &actor_pos);
	void Create(const sint32 t, const sint32 trans_t, const Unit &i,
	            const PLAYER_INDEX o);
	void Place(const MapPoint &center);

	UnitData(CivArchive &archive);

	virtual ~UnitData();

	bool Flag(uint32 f) const { return (m_flags & f) != 0; }
	void SetFlag(uint32 f) { m_flags |= f; }
	void ClearFlag(uint32 f) { m_flags &= ~(f); }

	bool IsImmobile()const; //PFT
	bool CantGroup()const; //by E
	sint32 ResetMovement();

	void GetPos(MapPoint &p) const { p = m_pos; };
	MapPoint GetPos(void) { return m_pos; }
	void SetPos(const MapPoint &p, bool &left_map);
	void SetPosAndNothingElse(const MapPoint &p);

	void GetInserted(const Unit &transport);
	bool InsertCargo(const Unit &addme);
	void DelFromCargo(const Unit & delme);
	sint32 GetType() const { return m_type; };
	bool CanCarry(const sint32 src) const;
	sint32 GetCargoCapacity() const;
	sint32 GetMaxCargoCapacity() const;
	sint32 GetNumCarried() const;
	bool CargoHasLandUnits() const;
	void GetCargoHP(sint32 &i, sint32 unit_type[100],
	                sint32 unit_hp[100]) const;

	bool CanAtLeastOneCargoUnloadAt
	(
	    MapPoint const &    unload_pos,
	    bool                use_vision,
	    bool                check_move_points = true
	) const;

	bool CanThisCargoUnloadAt
	(
	    Unit                the_cargo,
	    MapPoint const &    unload_pos,
	    bool                use_vision,
	    bool                check_move_points = true
	) const;

	bool UnloadCargo(const MapPoint &unload_pos, Army &debark, sint32 &count, bool unloadSelected = false);

	bool IsBeingTransported() const { return Flag(k_UDF_IS_IN_TRANSPORT); };
	void SetIsInTransport(const Unit &transport);
	void UnsetIsInTransport();
	bool IsMovePointsEnough(const MapPoint &pos) const;

	void SetIsProfessional(bool on);
	bool GetIsProfessional() const { return Flag(k_UDF_IS_PROFESSIONAL); }

	uint32 GetOwnerBit() const { return 0x1 << m_owner; }
	PLAYER_INDEX GetOwner () const { return m_owner; };
	void SetOwner(PLAYER_INDEX newo);

	double GetHP() const;
	void DeductHP(const double fp);
	void SetHP(const double hp);

	double GetMovementPoints() const { return m_movement_points; };
	void   SetMovementPoints(double mp);

	bool GetFirstMoveThisTurn()const { return Flag(k_UDF_FIRST_MOVE); };
	void SetFirstMoveThisTurn(sint32 fm);

	sint32 GetFuel() const { return m_fuel; }
	void SetFuel(sint32 fuel);
	bool GetUsedFuel (sint32 &fuel_remaining, sint32 &max_fuel) const;

	bool IsVeteran() const { return Flag(k_UDF_IS_VET); };
	void SetVeteran();
	void UnVeteran();

	UnitDynamicArray* GetCargoList() const { return m_cargo_list; }
	CityData* GetCityData() const { return m_city_data; }
	void InitializeCityData(sint32 settlerType = -1);

	void Serialize(CivArchive &archive);
#ifdef _DEBUG
	char const * GetText() const;
	void SetText(char const * str);
	void GamestateDebug();
#endif
	bool BuildUnit(sint32 type);
	bool BuildImprovement(sint32 type);
	bool BuildWonder(sint32 wonder);
	bool BuildEndGame(sint32 type);

	void AddWonder(sint32 type);
	bool ChangeCurrentlyBuildingItem(sint32 category, sint32 item_type);

	bool HaveImprovement(const sint32 type) const;
	uint64 GetImprovements() const;

	void DestroyCapitol();
	void DestroyImprovement(sint32 type);
	void NewGovernment(sint32 type);
	double GetDefendersBonus() const;
	bool ImprovementCanRefuel(const Unit &u) const;
	bool DeductMoveCost(const Unit &me, const double cost, bool &out_of_fuel);
	bool CanRustle(CellUnitList const & defender) const;
	bool CanConvertCity(Unit city) const;
	bool CanBombardType(const Unit & defender) const;
	bool CanBombard(CellUnitList const & defender) const;
	bool CanCounterBombard(CellUnitList const & defender) const;
	bool CanActivelyDefend(CellUnitList const & attacker) const;

	void BombardOneRound(const UnitRecord *rec, Unit &defender, double dbonus,
	                     double dmr);
	void Bombard(const UnitRecord *rec, Unit defender, bool isCounterBombardment);
	bool Bombard(CellUnitList &defender, bool isCounterBombardment);
	void FightOneRound(Unit did, double defenders_bonus, double amr, double dmr);

	bool CanSettle(const MapPoint &pos, const bool settleOnCity = false) const;
	bool Settle();
	void ResetCityOwner(const Unit &me, const PLAYER_INDEX newo, sint32 is_conquest,
	                    const CAUSE_REMOVE_CITY cause);
	void ResetUnitOwner(const Unit &me, const PLAYER_INDEX new_owner,
	                    CAUSE_REMOVE_ARMY cause) ;
	void BeginTurnCity(const Unit &me, UnitDynamicArray &dead);
	void SetType(sint32 type);
	bool CanUpgrade(sint32 & upgradeType, sint32 & upgradeCosts) const;
	void Upgrade(const sint32 type, const sint32 costs);
	sint32 GetBestUpgradeUnitType() const;
	sint32 GetUpgradeCosts(sint32 upgradeType) const;


	void SetSpriteState(SpriteState *s) { m_sprite_state = s; };
	SpriteState * GetSpriteState() const { return m_sprite_state; };

	void SetActor(UnitActor *a) { m_actor = a; };
	UnitActor * GetActor() const { return m_actor; };

	bool IsCity() const { return m_city_data != NULL; }
	void GetPop(sint32 &p)const;
	void GetTurnsToNextPop(sint32 &p)const; //PFT 29 mar 05, show # turns until city grows

	void AddTradeRoute(TradeRoute &route, bool fromNetwork);
	void DelTradeRoute(TradeRoute route);

	bool CanInterceptTrade() const;
	ORDER_RESULT InterceptTrade();
	bool HasResource(const sint32 resource) const;
#ifdef CTP1_TRADE
	sint32 GetResourceCount(const sint32 resource) const;
	sint32 GetLocalResourceCount(const sint32 resource) const;
	const Resources *GetResources() const;
#endif
	sint32 GetNumTradeRoutes() const;

	void SetVisible(PLAYER_INDEX player);
	void UnsetVisible(PLAYER_INDEX player);
	void BeginTurnVision(PLAYER_INDEX player);

	void RemoveUnitVision();
	void RemoveOldUnitVision(double radius);
	void AddUnitVision();
	void DoVision(UnitDynamicArray &revealedUnits);
	void UndoVision();
	void KillVision();

	uint32 GetVisibility() const;
	uint32 GetRealVisibility() const { return m_visibility; }
	uint32 GetRadarVisibility() const { return m_radar_visibility; }
	uint32 GetEverVisible() const { return m_ever_visible; }

	void SetRadar(sint32 o) { m_radar_visibility |= (1 << o); }
	void ForceVisibleDuration(const PLAYER_INDEX to_me, sint32 duration);

	bool PacMan() const;
	void SetPacMan();
	sint32 CollectProduction(sint32 &mil, sint32 &mat);
	void PayFederalProduction(double percent_military,
	                          sint32 &mil_paid,
	                          double percent_terrain,
	                          sint32 &mat_paid);
	void PayFederalProductionAbs(sint32 mil_paid,
	                             double percent_mat,
	                             sint32 &mat_paid);

	void SetMaterialContribution(bool on);
	bool GetMaterialContribution() const;

	void SetMilitaryContribution(bool on);
	bool GetMilitaryContribution() const;

	static sint32 GetDistance(UnitData *unit1, UnitData *unit2, sint32 wrap);
	static sint32 GetDistance(const Installation &inst, UnitData *unit2, sint32 wrap);
	static sint32 GetDistance(const UnitData *unit, const MapPoint &pos, sint32 wrap);
	static sint32 GetDistance(const MapPoint &pos1, const MapPoint &pos2, sint32 wrap);

	bool IsEntrenched() const;
	bool IsEntrenching() const;
	bool CanEntrench() const;
	void Entrench();
	void Detrench();
	bool IsAsleep() const;
	void Sleep();
	void WakeUp();
	void SetPatrolling(bool patrolling);
	bool IsPatrolling() const { return Flag(k_UDF_IS_PATROLLING); }

	double GetHappiness() const { Assert(m_city_data); return m_city_data->GetHappiness(); }
	double GetHappySize() const { Assert(m_city_data); return m_city_data->GetHappySize(); }
	double GetHappyPollution() const { Assert(m_city_data); return m_city_data->GetHappyPollution(); }
	double GetHappyConquestDistress() const { Assert(m_city_data); return m_city_data->GetHappyConquestDistress(); }
	double GetHappyEmpireDist() const { Assert(m_city_data); return m_city_data->GetHappyEmpireDist(); }
	double GetHappyEnemyAction() const { Assert(m_city_data); return m_city_data->GetHappyEnemyAction(); }
	double GetHappyPeace() const  { Assert(m_city_data); return m_city_data->GetHappyPeace(); }
	double GetHappyWorkday() const { Assert(m_city_data); return m_city_data->GetHappyWorkday(); }
	double GetHappyWages() const { Assert(m_city_data); return m_city_data->GetHappyWages(); }
	double GetHappyRations() const { Assert(m_city_data); return m_city_data->GetHappyRations(); }
	double GetHappyMartialLaw() const { Assert(m_city_data); return m_city_data->GetHappyMartialLaw(); }
	double GetHappyPopEntertainment() const { Assert(m_city_data); return m_city_data->GetHappyPopEntertainment(); }
	double GetHappyImprovement() const { Assert(m_city_data); return m_city_data->GetHappyImprovement(); }
	double GetHappyWonders() const { Assert(m_city_data); return m_city_data->GetHappyWonders(); }
	double GetHappyCrime() const { Assert(m_city_data); return m_city_data->GetHappyCrime(); }

	sint32 PayWages(sint32 w);
	sint32 GetWagesNeeded() const;

	void CalcHappiness(sint32 &virtualGoldSpent, bool firstPass) { Assert(m_city_data); m_city_data->CalcHappiness(virtualGoldSpent, firstPass); }

	void BeginTurn();
	void AddHappyTimer(sint32 turns, double adjust, HAPPY_REASON reason);
	void EndTurnCity();
	sint32 GetPollution() const;
	void CityToPark(sint32 agressor);

	void RebuildQuadTree();
	void GetNuked(UnitDynamicArray &killList);

	bool SafeFromNukes() const;

	double GetPositionDefense(const Unit &attacker) const;

	double GetOffense(const Unit &defender) const;
	double GetDefense(const Unit &attacker) const;
	double GetRanged(const Unit &defender) const;
	double GetDefCounterAttack(const Unit &attacker) const;

	double GetAttack(const UnitRecord *rec, const Unit defender) const;
	bool HasAirport() const;
	void UseAirport();
	sint32 AirportLastUsed() const;

	void CitySupportUnit(const Unit &u);

	bool HasCityWalls() const;
	bool HasForceField() const;

	bool StoppedBySpies(const Unit &c);
	ORDER_RESULT InvestigateCity(Unit c);
	ORDER_RESULT StealTechnology(Unit c, sint32 whichAdvance);
	ORDER_RESULT InciteRevolution(Unit c);
	ORDER_RESULT AssassinateRuler(Unit c);
	ORDER_RESULT NullifyWalls(Unit c);

	bool HasBeenSpiedUpon() const;
	void SetSpiedUpon();

	void CityNullifyWalls();

	ORDER_RESULT EstablishEmbassy(Unit c);
	ORDER_RESULT ThrowParty(Unit c, sint32 gold);
	void HearGossip(Unit c);

	void CityRadiusFunc(const MapPoint &pos);
	bool IsCapitol() const;

	void MakeFranchise(sint32 player);
	sint32 GetFranchiseOwner() const;
	sint32 GetProductionLostToFranchise() const;
	sint32 GetFranchiseTurnsRemaining() const;
	void SetFranchiseTurnsRemaining(sint32 turns);

	bool CanSee(const Army &al) const;

	void MakeCitizen(PopDBIndex pi, const MapPoint &point, sint32 origOwner);

	sint32 PopCount() const;
	sint32 CountSlaves() const;
	bool IsWatchful() const;
	void SetWatchful();
	void ModifySpecialAttackChance(UNIT_ORDER_TYPE order, double &chance);

	void RemoveOneSlave(PLAYER_INDEX p);
	void DoUprising(UPRISING_CAUSE cause);

	bool IsTempSlaveUnit() const;
	void SetTempSlaveUnit(bool b);

	void BioInfect( sint32 player );
	void NanoInfect( sint32 player );

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

#if 0
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

	sint32 GetCombatUnits() const;

//	bool CanHavePopType(sint32 type) const;
	bool CanBuildUnit(sint32 type) const;
	bool CanBuildBuilding(sint32 type) const;

	bool CanBuildCapitalization() const;
	bool CanBuildWonder(sint32 type) const;

	void Injoin(sint32 player);
	bool IsInjoined() const;
	sint32 InjoinedBy() const;

	double GetDistanceToCapitol() const;
	bool GetCurrentOrderString(StringId &id) const;

	double GetOverseasDistress() const;

	void SupportBuildings();
	void CheckRiot();
	bool AiGetCargoMovementPoints(double &min_move_points,
	                              bool  &first) const;

#if 0
	void SetLastBattle(sint32 turn);
	sint32 GetLastBattle() const;
#endif

	void EndTurn();
	void RecalculateResources();

	bool FightOneLineDanceRangedAttack(Unit &defender);
	bool FightOneLineDanceAssault(Unit &defender);

	sint32 CountTradeWith(PLAYER_INDEX) const;

	double IsProtectedFromSlavery() const;

	void NotifyAdvance(AdvanceType advance);
	sint32 CreateOwnArmy();
	void GiveCommand(UNIT_COMMAND command);

	void ExitWormhole(MapPoint &pos);
	bool HasLeftMap() const;

	sint32 FreeSlaves();

	void SetArmy(const Army &army);
	void ChangeArmy(const Army &army, CAUSE_NEW_ARMY cause);
	Army GetArmy() const { return m_army; }

	void KillTransportedUnits();

	bool CanPerformSpaceTransitionNow() const;
	void IndicateSpaceTransition();

	void AddWonderHPBonus(sint32 amt);

	bool CanPlantNuke(const MapPoint &pos) const;
	bool CanMakePark(const MapPoint &pos) const;
	bool CanUndergroundRailway(const MapPoint &pos) const;
	bool CanConvert(const MapPoint &pos) const;
	bool CanEstablishEmbassy(const MapPoint &pos) const;
	bool CanCreateFranchise(const MapPoint &pos) const;
	bool CanAssasinateRuler(const MapPoint &pos) const;
	bool CanStealTechnology(const MapPoint &pos) const;
	bool CanInjoin(const MapPoint &pos) const;
	bool CanInciteRevolution(const MapPoint &pos) const;
	bool CanCauseUnhappiness(const MapPoint &pos) const;
	bool CanExpel(const MapPoint &pos) const;

	void AddEndGameObject(sint32 type);
	double GetVisionRange() const;

	bool SendSlaveTo(Unit &dest);

	void SetFullHappinessTurns(sint32 turns);
	sint32 GetIncomingTrade() const;
	sint32 GetOutgoingTrade() const;
	bool CheckForRefuel();
	void FinishBuilding();
	void DestroyRandomBuilding();
	void AddConversionUnhappiness(sint32 who);

	void BuildCapitalization();
	void BuildInfrastructure();

	void ActionSuccessful(SPECATTACK attack, const Unit &c);
	void ActionUnsuccessful(void);;

	const Unit &GetTransport() const;
	void RemoveTransportedUnit(const Unit &u);

	void DisbandCity();
	bool CanPerformSpecialAction() const;

	void AddGoods(SlicObject *obj);
	void UpdateZOCForInsertion();

	void CheckVisionRadius();

	void SetTargetCity(const Unit &city);
	const Unit &GetTargetCity();

	bool CanBeachAssaultRightNow();
	const UnitRecord * GetDBRec(void) const;
	sint32 CalculateTotalHP() const;
	sint32 CalculateTotalFuel() const;
	double CalculateTotalMovePoints() const;

//emod
	bool IsElite() const { return Flag(k_UDF_IS_ELITE); };
	void SetElite();
	void UnElite();

private:
    bool CanExecuteNextTo
    (
        MapPoint const &            a_Position,
        UnitRecord::BoolAccessor    a_Function
    ) const;

#if 0
	void PrintSizeOfClass()
	{
		DPRINTF(k_DBG_AI, ("\n"));
		DPRINTF(k_DBG_AI, ("Size of UnitData class:\n"));
		DPRINTF(k_DBG_AI, ("GameObj: %d\n",                 sizeof(GameObj)));
		DPRINTF(k_DBG_AI, ("CityRadiusCallback: %d\n",      sizeof(CityRadiusCallback)));

		DPRINTF(k_DBG_AI, ("UnitData: %d\n",                sizeof(UnitData)));
		DPRINTF(k_DBG_AI, ("m_hp: %d\n",                    sizeof(m_hp)));
		DPRINTF(k_DBG_AI, ("m_movement_points: %d\n",       sizeof(m_movement_points)));
		DPRINTF(k_DBG_AI, ("m_cargo_list: %d\n",            sizeof(m_cargo_list)));
		DPRINTF(k_DBG_AI, ("m_city_data: %d\n",             sizeof(m_city_data)));
		DPRINTF(k_DBG_AI, ("m_sprite_state: %d\n",          sizeof(m_sprite_state)));
		DPRINTF(k_DBG_AI, ("m_actor: %d\n",                 sizeof(m_actor)));
		DPRINTF(k_DBG_AI, ("m_roundTheWorldMask: %d\n",     sizeof(m_roundTheWorldMask)));
		DPRINTF(k_DBG_AI, ("m_army: %d\n",                  sizeof(m_army)));
		DPRINTF(k_DBG_AI, ("m_transport: %d\n",             sizeof(m_transport)));
		DPRINTF(k_DBG_AI, ("m_target_city: %d\n",           sizeof(m_target_city)));
		DPRINTF(k_DBG_AI, ("m_owner: %d\n",                 sizeof(m_owner)));
		DPRINTF(k_DBG_AI, ("m_fuel: %d\n",                  sizeof(m_fuel)));
		DPRINTF(k_DBG_AI, ("m_type: %d\n",                  sizeof(m_type)));
		DPRINTF(k_DBG_AI, ("m_visibility: %d\n",            sizeof(m_visibility)));
		DPRINTF(k_DBG_AI, ("m_ever_visible: %d\n",          sizeof(m_ever_visible)));
		DPRINTF(k_DBG_AI, ("m_temp_visibility: %d\n",       sizeof(m_temp_visibility)));
		DPRINTF(k_DBG_AI, ("m_radar_visibility: %d\n",      sizeof(m_radar_visibility)));
		DPRINTF(k_DBG_AI, ("m_flags: %d\n",                 sizeof(m_flags)));
		DPRINTF(k_DBG_AI, ("m_pos: %d\n",                   sizeof(m_pos)));
		DPRINTF(k_DBG_AI, ("m_temp_visibility_array: %d\n", sizeof(m_temp_visibility_array)));
		DPRINTF(k_DBG_AI, ("\n"));
	}
#endif
};

uint32 UnitData_UnitData_GetVersion(void);
#endif
