//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Unit data
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
//
//----------------------------------------------------------------------------

#pragma once
#ifndef __UNIT_DATA_H__ 
#define __UNIT_DATA_H__ 1

#include "GameObj.h"
#include "MapPoint.h"
#include "Unit.h"
#include "citydata.h"
#include "Army.h"


class UnitList;
class citydata; 
class SpriteState; 
class UnitActor; 
class TradeRoute;
class Installation;
class UnitDynamicArray;
class Resources;
template <class T> class DynamicArray;
class SlicObject;
class BitMask;

enum UNIT_ORDER_TYPE;
enum HAPPY_REASON;

#define k_UNITDATA_VERSION_MAJOR    0
#define k_UNITDATA_VERSION_MINOR    0

#define k_DEFAULT_VIS_DURATION_SIZE 2
#ifndef k_MAX_PLAYERS

#define k_MAX_PLAYERS 32
#endif

enum UNIT_COMMAND {
	UNIT_COMMAND_NEAREST_CITY,
	UNIT_COMMAND_NEAREST_FORT,
	UNIT_COMMAND_NEAREST_AIRFIELD,
	UNIT_COMMAND_ENTRENCH,
	UNIT_COMMAND_SLEEP
};

class VisibilityDurationArray
{
public:
	VisibilityDurationArray() {
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

enum CAUSE_REMOVE_CITY; 

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

enum DEFAULT_PLACE_POP;

class UnitData : public GAMEOBJ,
                 public CityRadiusCallback
{

private:




	PLAYER_INDEX m_owner;
	sint32 m_fuel;

	double m_hp;
	double m_movement_points;

	sint32 m_type;
	uint32 m_visibility;
	uint32 m_ever_visible;
	uint32 m_temp_visibility;
	
	uint32 m_radar_visibility;
	uint32 m_flags;









	Army m_army;
	MapPoint m_pos;
	UnitDynamicArray   *m_cargo_list;
	CityData *m_city_data;

	SpriteState *m_sprite_state;

	UnitActor *m_actor;

	VisibilityDurationArray m_temp_visibility_array;
	Unit m_transport; 

	Unit m_target_city; 

	BitMask *m_roundTheWorldMask;



	friend class NetCity;
	friend class NetPop;
	friend class NetUnit;
	friend class NetUnitMove;

#ifdef _PLAYTEST
	
	friend class CreateImprovementCommand;
#endif

public:

#if _DEBUG
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

	BOOL Flag(uint32 f) const { return (m_flags & f) != 0; }
	void SetFlag(uint32 f) { m_flags |= f; }
	void ClearFlag(uint32 f) { m_flags &= ~(f); }

	bool IsImmobile()const; //PFT
	sint32 ResetMovement();
	
	void GetPos(MapPoint &p) const { p = m_pos; };
	MapPoint GetPos(void) { return m_pos; }
	void SetPos(const MapPoint &p, BOOL &revealed_unexplored, BOOL &left_map);
	void SetPosAndNothingElse(const MapPoint &p);


	void GetInserted(const Unit &transport);
	sint32 InsertCargo(const Unit &addme);
	void DelFromCargo(const Unit delme);
	sint32 GetType() const { return m_type; };
	sint32 CanCarry(const sint32 src) const;
	sint32 GetCargoCapacity() const;
	sint32 GetNumCarried() const;
	BOOL CargoHasLandUnits() const;
	void GetCargoHP(sint32 &i, sint32 unit_type[100],
	                sint32 unit_hp[100]);

	BOOL CanAtLeastOneCargoUnloadAt(const MapPoint &old_pos, const MapPoint &dest_pos, const BOOL & use_vision) const;
	BOOL CanThisCargoUnloadAt(const Unit &the_cargo, const MapPoint & old_pos, const MapPoint & new_pos, const BOOL & use_vision) const;
	BOOL UnloadCargo(const MapPoint &new_pos, Army &debark,
	                 BOOL justOneUnit, Unit &theUnit);
	BOOL UnloadSelectedCargo(const MapPoint &new_pos, Army &debark);


	sint32 IsBeingTransported() const { return Flag(k_UDF_IS_IN_TRANSPORT); };
	void SetIsInTransport(const Unit &transport);
	void UnsetIsInTransport();
	BOOL IsMovePointsEnough(const MapPoint &pos) const;

	
	void SetIsProfessional(BOOL on);
	BOOL GetIsProfessional() const { return Flag(k_UDF_IS_PROFESSIONAL); }

	uint32 GetOwnerBit() const { return 0x1 << m_owner; } 
	PLAYER_INDEX GetOwner () const { return m_owner; }; 
	void SetOwner(PLAYER_INDEX newo);

	double GetHP() const;
	void DeductHP(const double fp);
	void SetHP(const double hp);

	double GetMovementPoints() const { return m_movement_points; };
	void   SetMovementPoints(double mp);

	sint32 GetFirstMoveThisTurn()const { return Flag(k_UDF_FIRST_MOVE); }; 
	void SetFirstMoveThisTurn(sint32 fm);

	sint32 GetFuel() const { return m_fuel; }
	void SetFuel(sint32 fuel);
	BOOL GetUsedFuel (sint32 &fuel_remaining, sint32 &max_fuel) const;

	sint32 IsVeteran() const { return Flag(k_UDF_IS_VET); }; 
	void SetVeteran();
	void UnVeteran();
	
	UnitDynamicArray* GetCargoList() const { return m_cargo_list; }
	CityData* GetCityData() const { return m_city_data; }
	void InitializeCityData(sint32 settlerType = -1);

	void Serialize(CivArchive &archive) ;
#ifdef _DEBUG
	char *GetText(); 
	void SetText(char *str);
	void GamestateDebug();
#endif
	BOOL BuildUnit(sint32 type);
	BOOL BuildImprovement(sint32 type);
	BOOL BuildWonder(sint32 wonder);
	BOOL BuildEndGame(sint32 type);

	void AddWonder(sint32 type);
	BOOL ChangeCurrentlyBuildingItem(sint32 category, sint32 item_type);

	BOOL HaveImprovement(const sint32 type) const;
	uint64 GetImprovements() const;

	void DestroyCapitol(); 
	void DestroyImprovement(sint32 type);
	void NewGovernment(sint32 type);
	double GetDefendersBonus() const; 
	sint32 ImprovementCanRefuel(const Unit &u) const;     
	sint32 DeductMoveCost(const Unit &me, const double cost, BOOL &out_of_fuel);
	sint32 CanRustle(CellUnitList &defender) const;
	sint32 CanConvertCity(Unit &city) const;
	BOOL CanBombardType(const Unit & defender) const;
	sint32 CanBombard(CellUnitList &defender) const;
	sint32 CanCounterBombard(CellUnitList &defender) const;
	sint32 CanActivelyDefend(CellUnitList &attacker) const;

	void BombardOneRound(const UnitRecord *rec, Unit &defender, double dbonus,
	                     double dmr);
	void Bombard(const UnitRecord *rec, Unit defender, BOOL isCounterBombardment);
	BOOL Bombard(CellUnitList &defender, BOOL isCounterBombardment);
	void FightOneRound(Unit did, double defenders_bonus, double amr, double dmr);

	BOOL CanSettle(const MapPoint &pos) const;
	BOOL Settle();
	void ResetCityOwner(const Unit &me, const PLAYER_INDEX newo, sint32 is_conquest,
	                    const CAUSE_REMOVE_CITY cause); 
	void ResetUnitOwner(const Unit &me, const PLAYER_INDEX new_owner,
	                    CAUSE_REMOVE_ARMY cause) ;
	void BeginTurnCity(const Unit &me, UnitDynamicArray &dead);



	void SetSpriteState(SpriteState *s) { m_sprite_state = s; };
	SpriteState * GetSpriteState() const { return m_sprite_state; };












	void SetActor(UnitActor *a) { m_actor = a; };
	UnitActor * GetActor() const { return m_actor; };

	sint32 IsCity() const { return m_city_data != NULL; }
	void GetConsumptionStats(sint32 &foodConsumed) const;
	void GetPop(sint32 &p)const; 
	void GetTurnsToNextPop(sint32 &p)const; //PFT 29 mar 05, show # turns until city grows

	void AddTradeRoute(TradeRoute &route, BOOL fromNetwork);
	void DelTradeRoute(TradeRoute route);

	
	BOOL CanInterceptTrade() const;
	ORDER_RESULT InterceptTrade();
	BOOL HasResource(const sint32 resource) const;
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
	void AddUnitVision(BOOL &revealed);
	void DoVision(UnitDynamicArray &revealedUnits);
	void UndoVision();
	void KillVision();

	uint32 GetVisibility() const; 
	uint32 GetRealVisibility() const { return m_visibility; }
	uint32 GetRadarVisibility() const { return m_radar_visibility; }
	uint32 GetEverVisible() const { return m_ever_visible; }

	void SetRadar(sint32 o) { m_radar_visibility |= (1 << o); }
	void ForceVisibleDuration(const PLAYER_INDEX to_me, sint32 duration);

	BOOL PacMan() const;
	void SetPacMan();
	sint32 CollectProduction(sint32 &mil, sint32 &mat);
	void PayFederalProduction(double percent_military,
	                          sint32 &mil_paid,
	                          double percent_terrain,
	                          sint32 &mat_paid);
	void PayFederalProductionAbs(sint32 mil_paid,
	                             double percent_mat,
	                             sint32 &mat_paid);

	void SetMaterialContribution(BOOL on);
	BOOL GetMaterialContribution() const;

	void SetMilitaryContribution(BOOL on);
	BOOL GetMilitaryContribution() const;

	static sint32 GetDistance(UnitData *unit1, UnitData *unit2, sint32 wrap);
	static sint32 GetDistance(Installation &inst, UnitData *unit2, sint32 wrap);
	static sint32 GetDistance(const UnitData *unit, const MapPoint &pos, sint32 wrap);
	static sint32 GetDistance(const MapPoint &pos1, const MapPoint &pos2, sint32 wrap);

	BOOL IsEntrenched() const;
	BOOL IsEntrenching() const;
	BOOL CanEntrench() const;
	void Entrench();
	void Detrench();
	BOOL IsAsleep() const;
	void Sleep();
	void WakeUp();
	void SetPatrolling(BOOL patrolling);
	BOOL IsPatrolling() const { return Flag(k_UDF_IS_PATROLLING); }

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
	sint32 GetWagesNeeded();

	void CalcHappiness(sint32 &virtualGoldSpent, BOOL firstPass) { Assert(m_city_data); m_city_data->CalcHappiness(virtualGoldSpent, firstPass); } 

	BOOL IsInsideCityRadius(const MapPoint &pos) const;
	void BeginTurn();
	void AddHappyTimer(sint32 turns, double adjust, HAPPY_REASON reason);
	void EndTurnCity();
	sint32 GetPollution() const;
	void CityToPark(sint32 agressor);

	void RebuildQuadTree();
	void GetNuked(UnitDynamicArray &killList);

	BOOL SafeFromNukes() const;

	
	double GetPositionDefense(const Unit &attacker) const;

	double GetOffense(const Unit &defender) const;
	double GetDefense(const Unit &attacker) const;

	double GetAttack(const UnitRecord *rec, const Unit defender) const;
	BOOL HasAirport() const;
	void UseAirport();
	sint32 AirportLastUsed() const;

	void CitySupportUnit(const Unit &u);

	BOOL HasCityWalls() const;
	BOOL HasForceField() const;

	BOOL StoppedBySpies(Unit &c);
	ORDER_RESULT InvestigateCity(Unit &c);
	ORDER_RESULT StealTechnology(Unit &c, sint32 whichAdvance);
	ORDER_RESULT InciteRevolution(Unit &c);
	ORDER_RESULT AssassinateRuler(Unit &c);
	ORDER_RESULT NullifyWalls(Unit &c);

	BOOL HasBeenSpiedUpon() const;
	void SetSpiedUpon();

	void CityNullifyWalls();

	ORDER_RESULT EstablishEmbassy(Unit &c);
	ORDER_RESULT ThrowParty(Unit &c, sint32 gold);
	void HearGossip(Unit &c);

	void CityRadiusFunc(const MapPoint &pos);
	BOOL IsCapitol() const;

	void MakeFranchise(sint32 player);
	sint32 GetFranchiseOwner() const;
	sint32 GetFranchiseTurnsRemaining() const;
	void SetFranchiseTurnsRemaining(sint32 turns);

	BOOL CanSee(Army &al) const;

#ifdef _DEBUG
	void SetIgnoreHappiness(BOOL v); 
#endif

	void MakeCitizen(PopDBIndex pi, const MapPoint &point, sint32 origOwner);

	sint32 PopCount() const;
	sint32 CountSlaves() const;
	BOOL IsWatchful() const;
	void SetWatchful();
	void ModifySpecialAttackChance(UNIT_ORDER_TYPE order, double &chance);

	void RemoveOneSlave(PLAYER_INDEX p);
	void DoUprising(UPRISING_CAUSE cause);

	BOOL IsTempSlaveUnit() const;
	void SetTempSlaveUnit(BOOL b);

	void BioInfect( sint32 player );
	void NanoInfect( sint32 player );
	
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

	sint32 GetCombatUnits() const;

//	BOOL CanHavePopType(sint32 type) const;
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
	
	
	double GetOverseasDistress() const;

	void SupportBuildings();
	void CheckRiot();
	BOOL AiGetCargoMovementPoints(double &min_move_points, 
	                              BOOL  &first) const;

#if 0
	void SetLastBattle(sint32 turn);
	sint32 GetLastBattle() const;
#endif

	void EndTurn();
	void RecalculateResources();

	
	BOOL FightOneLineDanceRangedAttack(Unit &defender);
	BOOL FightOneLineDanceAssault(Unit &defender);

	sint32 CountTradeWith(PLAYER_INDEX) const;

	BOOL IsProtectedFromSlavery() const;


	void NotifyAdvance(AdvanceType advance);
	sint32 CreateOwnArmy();
	void GiveCommand(UNIT_COMMAND command);

	void ExitWormhole(MapPoint &pos);
	BOOL HasLeftMap() const;

	void ContributeScience(double incomePercent,
	                       double &addscience,
	                       double &subgold);
	sint32 FreeSlaves();

	void SetArmy(const Army &army);
	void ChangeArmy(const Army &army, CAUSE_NEW_ARMY cause);
	Army GetArmy() const { return m_army; }

	void KillTransportedUnits();

	BOOL CanPerformSpaceTransitionNow() const;
	void IndicateSpaceTransition();

	void AddWonderHPBonus(sint32 amt);

	sint32 CanPlantNuke(const MapPoint &pos) const;
	sint32 CanMakePark(const MapPoint &pos) const;
	sint32 CanUndergroundRailway(const MapPoint &pos) const;
	sint32 CanConvert(const MapPoint &pos) const;
	sint32 CanEstablishEmbassy(const MapPoint &pos) const;
	sint32 CanCreateFranchise(const MapPoint &pos) const;
	sint32 CanAssasinateRuler(const MapPoint &pos) const;
	sint32 CanStealTechnology(const MapPoint &pos) const;
	sint32 CanInjoin(const MapPoint &pos) const;
	sint32 CanInciteRevolution(const MapPoint &pos) const;
	sint32 CanCauseUnhappiness(const MapPoint &pos) const;
	sint32 CanExpel(const MapPoint &pos) const;


	void AddEndGameObject(sint32 type);
	double GetVisionRange() const;

	BOOL SendSlaveTo(Unit &dest);

	void SetFullHappinessTurns(sint32 turns);
	sint32 GetIncomingTrade() const;
	sint32 GetOutgoingTrade() const;
	BOOL CheckForRefuel();
	void FinishBuilding();
	void DestroyRandomBuilding();
	void AddConversionUnhappiness(sint32 who);

	void BuildCapitalization();
	void BuildInfrastructure();

	void ActionSuccessful(SPECATTACK attack, Unit &c);
	void ActionUnsuccessful(void);;

	const Unit &GetTransport() const;
	void RemoveTransportedUnit(const Unit &u);

	void DisbandCity();
	BOOL CanPerformSpecialAction() const;

	void AddGoods(SlicObject *obj);
	void UpdateZOCForInsertion();

	void CheckVisionRadius();

	void SetTargetCity(Unit &city);
	const Unit &GetTargetCity();

	bool CanBeachAssaultRightNow();
};

uint32 UnitData_UnitData_GetVersion(void) ;
#endif


