//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C/C++ header
// Description  : Player game object header
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
// BATTLE_FLAGS
//
// _DEBUG_INCOMPATIBLE
//
// _DEBUG
// - Generate debug version when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Return type of SetResearchGoal function set from void to
//   sint32, by Martin G�hmann.
// - Made GetCivilisation method const - May 7th 2005 Martin G�hmann
// - Import structure improved, merged with linux version.
// - Made player.h to compile again.
// - Removed unused void BeginTurnAllCities all cities method. - Aug. 7th 2005 Martin G�hmann
// - Added civilisation specific happiness bonus method. (Oct 7th 2005 Martin G�hmann)
// - Added EnergySupply method 2-28-2007
// - Slaves are distributed to more than just the closest city, the number
//   of closest cities to them slaves are sent can be set in const.txt. (25-Jan-2008 Martin G�hmann)
// - The player's cargo capacity is now calculated before the AI uses its
//   units and not afterwards. (3-Feb-2008 Martin G�hmann)
// - Separated the Settle event drom the Settle in City event. (19-Feb-2008 Martin G�hmann)
// - Added stuff for unit and city gold support. (22-Jul-2009 Maq)
// -Added HasPeaceTreatyWith() and HasAnyPactWith() functions (7-Jan-10 EPW)
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __PLAYER_H__
#define __PLAYER_H__ 1

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// None

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class Player;

enum ATTITUDE_TYPE
{
	ATTITUDE_TYPE_MIN,
	ATTITUDE_TYPE_NEUTRAL           = ATTITUDE_TYPE_MIN,
	ATTITUDE_TYPE_STRONG_HOSTILE,
	ATTITUDE_TYPE_STRONG_FRIENDLY,
	ATTITUDE_TYPE_WEAK_HOSTILE,
	ATTITUDE_TYPE_WEAK_FRIENDLY,
	ATTITUDE_TYPE_MAX,
};

enum DEATH_EFFECT_MORALE
{
    DEATH_EFFECT_OVERSEAS,
    DEATH_EFFECT_HOME,
    DEATH_EFFECT_CALC
};

enum DIPLOMATIC_STATE
{
	DIPLOMATIC_STATE_WAR,
	DIPLOMATIC_STATE_CEASEFIRE,
	DIPLOMATIC_STATE_NEUTRAL,
	DIPLOMATIC_STATE_ALLIED
};

enum DIPLOMATIC_STRENGTH
{
	DIPLOMATIC_STRENGTH_VERY_WEAK,
	DIPLOMATIC_STRENGTH_WEAK,
	DIPLOMATIC_STRENGTH_AVERAGE,
	DIPLOMATIC_STRENGTH_STRONG,
	DIPLOMATIC_STRENGTH_VERY_STRONG
};

enum PLAYER_TYPE
{
	PLAYER_TYPE_HUMAN,
	PLAYER_TYPE_ROBOT,
	PLAYER_TYPE_NETWORK
};

typedef sint32      PLAYER_INDEX;
typedef sint32      TERRAIN_IMPROVEMENT;

PLAYER_INDEX const  PLAYER_UNASSIGNED   = -1;
PLAYER_INDEX const  PLAYER_BARBARIAN    =  0;

#define k_ANNOUNCE_TRADE_MESSAGES       1
#define k_SQUELCH_TRADE_MESSAGES        0

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "Advances.h"           // Advances, AdvanceType
#include "AgreementTypes.h"     // AGREEMENT_TYPE
#include "AICause.h"            // CAUSE_..., ERR_BUILD_INST
#include "GameObj_types.h"      // CIV_INDEX, GENDER
#include "GameOver.h"           // GAME_OVER
#include "gstypes.h"            // k_MAX_PLAYERS
#include "MapPoint.h"           // MapPoint
#include "PollutionConst.h"     // k_MAX_EVENT_POLLUTION_TURNS, etc.
#include "TradeRoute.h"         // TradeRoute, ROUTE_TYPE
#include "Readiness.h"          // READINESS_LEVEL
#include "Strengths.h"          // STRENGTH_CAT
#include "Unit.h"               // UNIT_COMMAND
#include "GreatLibraryTypes.h"  // DATABASE

template <class T> class DynamicArray;
template <class T> class Database;

class UnseenCellCarton;
class Agreement;
class Regard;
class Civilisation;
class Message;
class UnitOrderQueue;
class UnitDynamicArray;
class Unit;
class Gold;
class Science;
class TaxRate;
class Difficulty;
class TradeDynamicArray;
class TradeOffer;
class PlayerHappiness;
class GovernmentRecord;
class Pop;
class Strengths;
class Army;
class CellUnitList;
class Score;
class ID;
class Vision;
class DiplomaticRequest;
class TerrainImprovement;
class MilitaryReadiness;
class MaterialPool;
class Installation;
class CreateUnitRequest;
class CivArchive;
class UnitActor;
class ID;
class Throne;
class EndGame;
class GaiaController;
class CityData;

struct PSlicComplexRegion;

//----------------------------------------------------------------------------
// General declarations
//----------------------------------------------------------------------------

bool    player_isAlly(PLAYER_INDEX me, PLAYER_INDEX him);
bool    player_isEnemy(PLAYER_INDEX me, PLAYER_INDEX him);
uint32  Player_Player_GetVersion(void);

extern Player **    g_player;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class Player
{
private:

	MBCHAR *GenerateDescriptionString(bool is_winner);

public:

//----------------------------------------------------------------------------
// Do not change anything in the types or order of the following variable
// declarations. Doing so will break reading in of save files.
// See the Serialize implementation for more details.
//----------------------------------------------------------------------------

	PLAYER_INDEX     m_owner;
	PLAYER_TYPE      m_playerType;

	uint32           m_diplomatic_mute;
	uint32           mask_alliance;
	uint32           m_mask_hostile;
	DIPLOMATIC_STATE m_diplomatic_state[k_MAX_PLAYERS];

	sint32           m_government_type;
	sint32           m_tradeTransportPoints;
	sint32           m_usedTradeTransportPoints;
	uint32           m_pollution_history[k_MAX_POLLUTION_HISTORY];
	sint32           m_event_pollution[k_MAX_EVENT_POLLUTION_TURNS];
	BOOL             m_terrainPollution ;
	BOOL             m_deepOceanVisible;
	sint32           m_patience[k_MAX_PLAYERS] ;
	sint32           m_sent_requests_this_turn[k_MAX_PLAYERS];

	double           m_materialsTax;

	sint32           m_home_lost_unit_count;
	sint32           m_oversea_lost_unit_count;
	uint64           m_builtWonders;
	uint64           m_wonderBuildings;

	double           m_income_Percent;

	uint32           m_embassies;
	uint32           m_productionFromFranchises;

	sint32           m_assasinationModifier;
	sint32           m_assasinationTimer;
	BOOL             m_isDead;
	BOOL             m_first_city;

	sint32           m_totalArmiesCreated;
	BOOL             m_hasUsedCityView;
	BOOL             m_hasUsedWorkView;
	BOOL             m_hasUsedProductionControls;

	sint32           m_total_production;
	BOOL             m_is_turn_over;                // Used
	BOOL             m_end_turn_soon;               // Maybe unused
	sint32           m_powerPoints;
	sint32           m_lastActionCost;
	MapPoint         m_setupCenter;
	sint32           m_setupRadius;
	BOOL             m_doneSettingUp;

	uint32           m_contactedPlayers;
	BOOL             m_endingTurn;                  // Unused
	sint32           m_set_government_type;
	sint32           m_change_government_turn;
	BOOL             m_changed_government_this_turn;
	sint32           m_pop_science;
	sint32           m_num_revolted;

	BOOL             m_can_build_capitalization;
	BOOL             m_can_build_infrastructure;

	sint32           m_last_attacked[k_MAX_PLAYERS];

	BOOL             m_can_use_terra_tab;
	BOOL             m_can_use_space_tab;
	BOOL             m_can_use_sea_tab;
	BOOL             m_can_use_space_button;

	GUID             m_networkGuid;
	uint16           m_networkId;
	sint32           m_networkGroup;

	sint32           m_civRevoltingCitiesShouldJoin;

	BOOL             m_hasWonTheGame;
	BOOL             m_hasLostTheGame;

	BOOL             m_disableChooseResearch;

	BOOL             m_openForNetwork;

	sint32           m_virtualGoldSpent;

	sint32           m_current_round;

	sint32           m_maxCityCount;

	sint32           m_age;

	sint32           m_researchGoal;

	sint32           m_broken_alliances_and_cease_fires;

//----------------------------------------------------------------------------
// Changing the order below this line should not break anything.
//----------------------------------------------------------------------------

	DynamicArray<Army>               *m_all_armies;               // Serialized
	UnitDynamicArray                 *m_all_cities;               // Serialized

	UnitDynamicArray                 *m_all_units;                // Serialized
	UnitDynamicArray                 *m_traderUnits;              // Serialized
	Unit                             *m_capitol;                  // Serialized

	Regard                           *m_regard;                   // Serialized
	Gold                             *m_gold;                     // Serialized
	Science                          *m_science;                  // Serialized
	TaxRate                          *m_tax_rate;                 // Serialized
	Difficulty                       *m_difficulty;               // Serialized
	Advances                         *m_advances;                 // Serialized
	MaterialPool                     *m_materialPool;

	DynamicArray<TradeOffer>         *m_tradeOffers;              // Serialized
#ifdef BATTLE_FLAGS
	DynamicArray<MapPoint>           *m_battleFlags;              // Serialized
#endif

	DynamicArray<DiplomaticRequest>  *m_requests;                 // Serialized
	DynamicArray<Agreement>          *m_agreed;                   // Serialized
	DynamicArray<Message>            *m_messages;                 // Serialized

	Vision                           *m_vision;                   // Serialized

	DynamicArray<TerrainImprovement> *m_terrainImprovements;      // Serialized
	DynamicArray<Installation>       *m_allRadarInstallations;    // Serialized
	DynamicArray<Installation>       *m_allInstallations;         // Serialized

	MilitaryReadiness                *m_readiness;
	PlayerHappiness                  *m_global_happiness;         // Serialized
	Civilisation                     *m_civilisation;             // Serialized

	Throne                           *m_throne;                   // Not serialized

	Strengths                        *m_strengths;                // Serialized
	Score                            *m_score;                    // Serialized
	EndGame                          *m_endGame;                  // Serialized
	sint32                           *m_goodSalePrices;
	MBCHAR                           *m_email;                    // Serialized
	GaiaController                   *m_gaiaController;           // Serialized

	CreateUnitRequest                *m_unitRequestList;

	Unit                             *m_slic_special_city;        // Not serialized
	BOOL                              m_hasGlobalRadar;           // Not serialized

#ifdef _DEBUG_INCOMPATIBLE

	ATTITUDE_TYPE                     m_attitude[k_MAX_PLAYERS];  // Not serialized
#endif

	MBCHAR                            m_descrip_string[256];      // Not serialized, maybe used
	sint32                            m_starting_index;           // Not serialzed and unused
	sint32                            m_governorPwReserve;        // Not serialied, maybe used
	sint16                            m_cargoCapacity;            // Not serialized possibly used.
	double                            m_energysupply;
	double                            m_breadbasket;

	friend class NetInfo;
	friend class NetDifficulty;
	friend class NetPlayer;

public:
	friend class SelectedItem;

	Player(const PLAYER_INDEX o, sint32 d, PLAYER_TYPE pt);
	Player(const PLAYER_INDEX o, sint32 d, PLAYER_TYPE pt, const sint32 civ, GENDER gender) ;
	Player(CivArchive &archive) ;
	~Player();

	void InitPlayer(const PLAYER_INDEX o, sint32 d, PLAYER_TYPE pt) ;

	Unit CreateUnitNoPosition(const sint32 type, CellUnitList &army,
							  const MapPoint &actor_pos, sint32 oldOwner);
	Unit CreateUnit(sint32 type, const MapPoint &pos, const Unit hc,
					bool tempUnit,  CAUSE_NEW_ARMY cause);
	void CreateUnitSoon(sint32 type, const MapPoint &pos, const Unit hc,
						bool tempUnit, CAUSE_NEW_ARMY cause);
	void DoCreateUnits();

	sint32 FindArmyIndex(const Unit &unit) const;
	sint32 FindArmyIndex(const Army &army) const;
	sint32 FindCityIndex(const Unit &find) const;
	sint32 GetWeakestEnemy() const;


	Unit InsertUnitReference(const Unit &u,  const CAUSE_NEW_ARMY cause,
							 const Unit &whereBuilt);
	Army GetNewArmy(CAUSE_NEW_ARMY cause);
	bool RemoveUnitReference(const Unit &id, const CAUSE_REMOVE_ARMY cause,
							   PLAYER_INDEX killedBy);
	void AddArmy(const Army &army,
				 const CAUSE_NEW_ARMY cause,
				 bool fromNetwork,
				 const Unit &whereBuilt);
	void RemoveArmy(const Army &army, bool fromNetwork = false);

	bool RemoveCityReferenceFromPlayer(const Unit &id,  CAUSE_REMOVE_CITY cause,
									   sint32 &killedBy);

	bool RemoveUnitReferenceFromPlayer(const Unit &killme,  CAUSE_REMOVE_ARMY cause,
									   sint32 &killedBy);

	bool AddCityReferenceToPlayer(Unit id,  CAUSE_NEW_CITY cause);
	Unit CreateCity(const sint32 t, const MapPoint &pos,  CAUSE_NEW_CITY cause,
					UnitActor *actor, sint32 settlerType);

	bool RegisterCityAttack(const Unit &c, const PLAYER_INDEX &his_owner,
			const Unit &his_unit, UNIT_ORDER_TYPE attack_type);

	void ResetAllMovement();

	void BeginTurnUnits();

	void BeginTurn();
	void BeginTurnScience();

	void EndTurn();
	void EndTurnSoon();
	void ProcessUnitOrders(bool currentOnly = false);
	Civilisation *GetCivilisation(void) const { return (m_civilisation) ; }
	void GetPluralCivName(MBCHAR *s) ;
	void GetSingularCivName(MBCHAR *s) ;
	const MBCHAR *GetLeaderName(void) ;

	void RegisterLostUnits(sint32 nUnits, const MapPoint &pos,
	                       const DEATH_EFFECT_MORALE mtype);

	void GetArmyPos(sint32 index, MapPoint &army_pos);

	bool Settle      (Army &settle_army);
	bool SettleInCity(Army &settle_army);
	Unit CityIndexToUnit(sint32 index);

	void Entrench(sint32 idx);
	void Detrench(sint32 idx);
	void AddFeatHPBonus(sint32 hpBonus);
	void Sleep(sint32 idx);
	void WakeUp(sint32 idx);

	sint32 GetCheapestMilitaryUnit(const MapPoint& pos);

	void BeginTurnPollution(void) ;
	void EndTurnPollution(void) ;
	void AdjustPollution(const sint32 amount) ;
	void AdjustEventPollution(const sint32 amount) ;
	uint32 GetCurrentPollution(void) const { return (m_pollution_history[0]) ; }

	uint32 *GetPollutionHistory(void) { return (&m_pollution_history[0]) ; }
	uint32 GetPollutionLevel(void) { return (m_pollution_history[0]) ; }
	uint32 GetAverageEventPollution(void) ;

	Army GetArmy(sint32 s_index);
	UnitDynamicArray *GetAllUnitList() { return m_all_units; }
	UnitDynamicArray *GetAllCitiesList() { return m_all_cities; }
	UnitDynamicArray *GetTradersList() { return m_traderUnits; }
	DynamicArray<TradeOffer>* GetTradeOffersList() { return m_tradeOffers; }
	DynamicArray<Army> *GetAllArmiesList() { return m_all_armies; }
	DynamicArray<TerrainImprovement> *GetAllTileimpsList() { return m_terrainImprovements; } //emod
	DynamicArray<Installation> *GetAllInstallationsList() { return 	m_allInstallations; } //emod

	Unit GetTopSelectedArmy(const sint32 selected_army);

	sint32  GetNumUnits() const;   //EMOD
	sint32  GetNumCities() const;
	sint32  GetMaxCityCount() const;
	sint32  GetNumTileimps() const; //emod
	bool    GetNearestCity(const MapPoint &pos, Unit &nearest, double &distance,
	                       bool butNotThisOne = false, const sint32 continent = -1, bool mustHaveRoom = false);
	bool    GetSlaveCity(const MapPoint &pos, Unit &city);

	bool    GetNearestFort(const MapPoint &src, MapPoint &dest);
	bool    GetNearestAirfield(const MapPoint &src, MapPoint &dest, const sint32 continent = -1);

	bool    GetCityIndex(const Unit &c, sint32 &idx);
	Unit    GetCityFromIndex(sint32 unit_idx);
	bool    GetArmyIndex(const Unit &c, sint32 &idx);

	sint32	ReturnPopToCity(const MapPoint &pos, const sint32 announce);
	sint32	LoadBuildQueue(const sint32 city, const MBCHAR *file) ;
	sint32	SaveBuildQueue(const sint32 city, const MBCHAR *file) ;

	bool IsFriendly(sint32 p) { Assert((p>=0) && (p<k_MAX_PLAYERS) );
	                            return ((mask_alliance & 1L<<p) != 0); } ;
	sint32 GetMaskAlliance() { return mask_alliance;} ;

	DIPLOMATIC_STATE GetDiplomaticState(const PLAYER_INDEX p) const {
		Assert(0 <= p);
		Assert(p < k_MAX_PLAYERS);
		return m_diplomatic_state[p];
	}

	void SetDiplomaticState(const PLAYER_INDEX p, const DIPLOMATIC_STATE s);

	void GiveCity(const PLAYER_INDEX player, const sint32 c) ;
	void GiveCity(const PLAYER_INDEX player, Unit c) ;
	void ExchangeCity(const PLAYER_INDEX player, const sint32 c1, const sint32 c2) ;
	void ExchangeCity(const PLAYER_INDEX player, const Unit &c1, const Unit &c2) ;
	bool GiveGold(const PLAYER_INDEX other_player, const Gold &amount) ;
	void BequeathGold(const Gold &amount) ;
	void GiveAdvance(const PLAYER_INDEX recipient, const AdvanceType adv, CAUSE_SCI cause) ;
	void GiveUnit(const PLAYER_INDEX other_player, const sint32 unit_idx) ;

	void StopTradingWith(const PLAYER_INDEX bannedRecipient) ;
	void SetAlliance(PLAYER_INDEX ally) ;
	void ClearAlliance(PLAYER_INDEX ally) ;
	void FormAlliance(const PLAYER_INDEX ally) ;
	void BreakAlliance(const PLAYER_INDEX ally) ;
	void ExchangeMap(const PLAYER_INDEX recipient) ;
	void GiveMap(const PLAYER_INDEX recipient) ;
	bool IsPollutionReduced(void) ;
	bool IsViolatingPeace(const PLAYER_INDEX violator) ;
	bool IsViolatingBorders(const PLAYER_INDEX violator) ;
	bool WillViolateCeaseFire(const PLAYER_INDEX violator) ;
	void MakeNoPiracyPact(PLAYER_INDEX other_player) ;
	void MakeCeaseFire(PLAYER_INDEX treatyAlly) ;
	void MakeShortCeaseFire(PLAYER_INDEX treatyAlly, AGREEMENT_TYPE agreement, PLAYER_INDEX thirdParty = PLAYER_UNASSIGNED);
	void BreakCeaseFire(PLAYER_INDEX other_player, bool sendMessage) ;
	void BreakAllTreaties(PLAYER_INDEX with);

	void BeginTurnAgreements();

	void DumpAgreements(void) ;
	void DumpRequests(void) ;
	void RemoveAgreementReferences(Agreement a) ;
	void RemoveDiplomaticReferences(DiplomaticRequest a) ;
	DynamicArray<Agreement> *GetAgreements(void) const { return (m_agreed) ; }
	Agreement FindAgreement(const AGREEMENT_TYPE agreement, const PLAYER_INDEX otherParty) const ;
	Agreement FindAgreement(const AGREEMENT_TYPE agreement) const ;
	Agreement FindAgreement(const PLAYER_INDEX otherParty) const ;
	Agreement MakeCaptureCityPact(PLAYER_INDEX player, Unit &city) ;


	Agreement MakeEndPollutionPact(PLAYER_INDEX player) ;
	Agreement MakeLeaveOurLands(PLAYER_INDEX player) ;
	Agreement MakeReducePollution(PLAYER_INDEX player) ;


	DiplomaticRequest RequestIndexToRequest(sint32 requestIndex) ;

	bool WillViolatePact(PLAYER_INDEX otherParty) ;
	bool HaveNoPiracyAgreement(PLAYER_INDEX otherParty);

	bool FulfillCaptureCityAgreement(Unit city) ;
	DynamicArray<DiplomaticRequest>	*GetRequests(void) const { return (m_requests) ; }
	void RequestGreeting(const PLAYER_INDEX recipient) ;
	void RequestDemandAdvance(const PLAYER_INDEX recipient, AdvanceType advance) ;
	void RequestDemandCity(const PLAYER_INDEX recipient, Unit &city) ;
	void RequestDemandMap(const PLAYER_INDEX recipient) ;
	void RequestDemandGold(const PLAYER_INDEX recipient, Gold &amount) ;
	void RequestDemandStopTrade(const PLAYER_INDEX recipient, const PLAYER_INDEX third_party) ;
	void RequestDemandNoPiracy(const PLAYER_INDEX recipient) ;
	void RequestDemandAttackEnemy(const PLAYER_INDEX recipient, const PLAYER_INDEX third_party) ;
	void RequestDemandLeaveOurLands(const PLAYER_INDEX recipient) ;
	void RequestDemandReducePollution(const PLAYER_INDEX recipient) ;
	void RequestOfferAdvance(const PLAYER_INDEX recipient, AdvanceType &advance) ;
	void RequestOfferCity(const PLAYER_INDEX recipient, Unit &city) ;
	void RequestOfferMap(const PLAYER_INDEX recipient) ;
	void RequestOfferGold(const PLAYER_INDEX recipient, const Gold &amount) ;
	void RequestOfferCeaseFire(const PLAYER_INDEX recipient) ;
	void RequestOfferPermanentAlliance(const PLAYER_INDEX recipient) ;
	void RequestOfferPactCaptureCity(const PLAYER_INDEX recipient, Unit &city) ;
	void RequestOfferPactEndPollution(const PLAYER_INDEX recipient) ;

	void RequestExchangeAdvance(const PLAYER_INDEX recipient, AdvanceType &advance, AdvanceType &desired_advance) ;
	void RequestExchangeCity(const PLAYER_INDEX recipient, Unit &offerCity, Unit &wantCity) ;
	void RequestExchangeMap(const PLAYER_INDEX recipient) ;
	void AddAgreement(Agreement a) ;
	void AddDiplomaticRequest(DiplomaticRequest &request) ;
	void DiplomaticMute(PLAYER_INDEX player, bool enable) ;

	bool IsMuted(PLAYER_INDEX player) ;
	void DumpAllies(void) ;


	void AddMessage(Message &msg) ;
	void RegisterDiplomaticMessage(const Message &msg);
	void RegisterDiplomaticResponse(const DiplomaticRequest &req);
	void RemoveMessageReferences(Message msg) ;
	void DumpMessages(void) ;
	void SendTestMessage(void) ;
	DynamicArray<Message> *GetMessages(void) const { return (m_messages) ; }
	void NotifyModalMessageDestroyed();

	bool CanUseNukes() const;
	bool HasAdvance(AdvanceType adv) const;
	sint32 NumAdvances();
	double GetPollutionSizeModifier(void) const;
	double GetPollutionProductionModifier(void) const;
	void SetCityRoads();
#ifdef _DEBUG
	void DisplayAdvances() ;
#endif

	TradeRoute CreateTradeRoute(Unit sourceCity, ROUTE_TYPE sourceType,
								sint32 sourceResource,
								Unit destCity,
								PLAYER_INDEX paying_for,
								sint32 gold_in_return);
	TradeRoute PayForTrade(TradeRoute &route);
	void AddTransportPoints(sint32 delta);
	void RemoveTransportPoints(sint32 delta);
	void AddUsedTransportPoints(sint32 delta);
	void RemoveUsedTransportPoints(sint32 delta);


	void RemoveTradeRoute(TradeRoute route, CAUSE_KILL_TRADE_ROUTE cause);




	void CancelTradeRoute(TradeRoute route);

	void RenumberTradeRoute(TradeRoute oldRoute, TradeRoute newRoute);
	void InterceptTrade(sint32 army_index);
	void KillATrader();

	TradeOffer CreateTradeOffer(Unit fromCity,
								ROUTE_TYPE offerType, sint32 offerResource,
								ROUTE_TYPE askingType, sint32 askingResource,
								Unit toCity);
	void AddTradeOffer(TradeOffer offer);
	void RemoveTradeOffer(TradeOffer offer);
	void WithdrawTradeOffer(TradeOffer offer);
	void AcceptTradeOffer(TradeOffer offer, Unit &sourceCity, Unit &destCity);
	void CreateTradeBid(Unit &fromCity, sint32 resource, Unit &toCity);
	void SendTradeBid(const Unit &fromCity, sint32 resource, const Unit &toCity, sint32 price);
	void AcceptTradeBid(const Unit &fromCity, sint32 resource, const Unit &toCity, sint32 price);
	void RejectTradeBid(const Unit &fromCity, sint32 resource, const Unit &toCity, sint32 price);

	void AddTrader(Unit uid);
	sint32 GetUnusedFreight()const;
	sint32 GetTotalFreight()const;

	void AddUnitVision(const MapPoint &pnt, double range);
	void RemoveUnitVision(const MapPoint &pnt, double range);
	void OwnExploredArea();
	bool IsExplored(const MapPoint &pos) const;
	bool IsVisible(const MapPoint &pos) const;
	bool IsExplored(sint32 x, sint32 y) const;
	bool IsVisible(sint32 x, sint32 y) const;
	bool GetLastSeen(const MapPoint &pnt, UnseenCellCarton &ucell);
	void BeginTurnEnemyUnits();

	bool CanCreateImprovement(sint32 type,
	                          const MapPoint &point,
	                          const bool check_materials);
	TerrainImprovement CreateImprovement(sint32 dbIndex, MapPoint const & point,
										 sint32 extraData);
	TerrainImprovement CreateSpecialImprovement(sint32 dbIndex, MapPoint const & point,
										 sint32 extraData);  //EMOD

	void AddImprovement(TerrainImprovement imp);
	void RemoveImprovementReferences(TerrainImprovement imp);
	void BeginTurnImprovements();

	sint32 GetMaterialsStored()  const;
	Installation CreateInstallation(sint32 type, MapPoint &point);
	void AddInstallation(const Installation &inst);
	void RemoveInstallationReferences(const Installation &inst);

#ifdef BATTLE_FLAGS
	void AddBattleFlag(const MapPoint &point);
	void BeginTurnBattleFlags();
#endif

	sint32 GetReadinessLevel() const;
	void SetReadinessLevel(READINESS_LEVEL level, bool immediate = false);
	sint32 GetReadinessCost() const;

	sint32 GetTotalProduction() const { return m_total_production; }
	sint32 GetTotalUnitCost();
	sint32 GetTotalGoldHunger();//EMOD

	double GetHPModifier() const;
	double GetSupportModifier() const;

	void GamestateDebug();

	void GiveOrders(sint32 army, UNIT_ORDER_TYPE orders);

	void SetPlayerType(PLAYER_TYPE pt);

	PLAYER_TYPE GetPlayerType() { return m_playerType; };

	bool IsRobot  () const { return m_playerType == PLAYER_TYPE_ROBOT;   }
	bool IsHuman  () const { return m_playerType == PLAYER_TYPE_HUMAN;   }
	bool IsNetwork() const { return m_playerType == PLAYER_TYPE_NETWORK; }

	static bool IsThisPlayerARobot(const sint32 &p)
	{
		Assert(0 <= p);
		Assert(p < k_MAX_PLAYERS);
		Assert(g_player);
		Assert(g_player[p]);
		return (g_player[p]->IsRobot());
	}

	void BeginTurnWonders();
	void BeginTurnUnitSupportGold();
	void BeginTurnCommodityMarket();
	sint32 CalcWonderGold();
	sint32 CalcUnitSupportGold();
	sint32 CalcTotalBuildingUpkeep();
	void BuildUnit(sint32 type, Unit city);
	void BuildImprovement(sint32 type, Unit city);
	void BuildWonder(sint32 wonder, Unit city);
	void BuildEndGame(sint32 type, Unit city);

	bool ChangeCurrentlyBuildingItem(Unit city, sint32 category, sint32 item_type);

	void Serialize(CivArchive &archive) ;

	void BeginTurnProduction();

	void AddGold (const sint32 d);
	void SubGold (const sint32 d);
	void AddGold (const Gold &amount);
	void SubGold (const Gold &amount);
	sint32 GetGold();

	void SetGold(const sint32 d);
	void AddScience (const sint32 d);
	void AddPopScience(const sint32 d);

	void SpecialDiscoveryNotices(AdvanceType adv);
	void GovernmentDiscoveryNotices(AdvanceType advance) ;
	void ObsoleteNotices(AdvanceType advance);
	void BuildResearchDialog(AdvanceType adv);
	void SetResearching(AdvanceType advance);
	sint32 GetCurrentScienceCost();
	sint32 GetCurrentScienceLevel();
	void StartResearching(sint32 idx) ;
	void GetScienceTaxRate(double &s);
	void SetTaxes(double s);
	void DelTailPathOrder(sint32 index);

	void SetMaterialsTax(double m);
	void ContributeMaterials(sint32 &cityProduction);

	PLAYER_INDEX GetOwner() const { return m_owner; }
	sint32 GetGovernmentType() const { return m_government_type; }
	bool   SetGovernmentType(sint32 type);
	bool   ActuallySetGovernment(sint32 type);
	void   AttemptRevolt(void) ;
	void   Revolt(const sint32 idx) ;

	sint32 GetNumRevolted(void) const { return m_num_revolted; }

	sint32 GetUnitsLostHome() const { return m_home_lost_unit_count; }
	sint32 GetUnitsLostOverseas() const { return m_oversea_lost_unit_count; }

	void   GetPeaceMovement(double &overseas_defeat,
	                        double &home_defeat, double &overseas);

	void   SetWorkdayLevel (sint32 w);
	double GetWorkdayPerPerson ();
	double GetUnitlessWorkday ();

	void   SetWagesLevel (sint32 w);
	double GetWagesPerPerson();
	double GetUnitlessWages();

	void   SetRationsLevel (sint32 w);
	double GetRationsPerPerson() const;
	double GetUnitlessRations ();
	void   DisplayWWR();

	void   CalcAllHappiness();
	sint32 GetAverageHappiness();
	void   RegisterProfessionalChange(bool on, Unit &u);


	Difficulty* GetDifficulty() { return m_difficulty; }

	sint32 GetBaseContentment() const;

	double GetBigCityScale() const;
	double GetBigCityOffset() const;
	double GetPollutionDifficultyMultiplier() const;
	double GetRiotChance() const;
	double GetStarvationEffect() const;

	double GetPositiveWorkdayCoef() const;
	double GetNegativeWorkdayCoef() const;
	double GetWorkdayExpectation() const;
	double GetWorkdayLevel() const;

	double GetPositiveWagesCoef() const;
	double GetNegativeWagesCoef() const;
	double GetWagesExpectation() const;
	double GetWagesLevel() const;

	double GetPositiveRationsCoef() const;
	double GetNegativeRationsCoef() const;
	double GetRationsExpectation() const;
	double GetRationsLevel() const;

	double GetFoodCoef()      const;
	double GetProdCoef()      const;
	double GetGoldCoef()      const;
	double GetKnowledgeCoef() const;

	double GetPollutionCoef() const;
	double GetPollutionUnhappyCoef() const;

	double GetConquestDistress() const;
	double GetConquestDistressDecay() const;
	double GetEmpireDistanceScale() const;
	double GetMaxEmpireDistance() const;

	sint32 GetMaxMartialLawUnits() const;
	double GetMartialLawEffect() const;
	sint32 GetMartialLawThreshold() const;

	double GetAtHomeRadius() const;
	double GetOverseasCoef() const;

	double GetOverseasDefeatDecay() const;
	double GetOverseasDefeatCoef() const;
	double GetHomeDefeatDecay() const;
	double GetHomeDefeatCoef() const;

	double GetCrimeCoef() const;
	double GetCrimeOffset () const;

	sint32 GetProfessionalUnits() const;

	double GetReadyPeaceCoef() const;
	double GetReadyPeaceHP() const;

	double GetReadyAlertCoef() const;
	double GetReadyAlertHP() const;

	double GetReadyWarCoef() const;
	double GetReadyWarHP() const;

	double GetIncomePercent() {return m_income_Percent; }

	sint32 GetHappyDueToWonders() { return 0; };
	sint32 GetScienceHandicap();

	void   SetCapitol(const Unit &c);
	bool   GetCapitolPos(MapPoint &pos) const;

	void   BuildWonder(Unit city, sint32 wonder);
	void   AddWonder(sint32 wonder, Unit &city);
	void   RemoveWonder(sint32 wonder, bool destroyed);
	bool   HasWonder(sint32 wonder) const;
	uint64 GetBuiltWonders();
	uint64 GetWonderBuildings();

	sint32 GetTimedHappiness();
	void   AssasinateRuler();

	void   EstablishEmbassy(sint32 player);
	void   CloseEmbassy(sint32 player);
	bool   HasEmbassyWith(sint32 player) const;
	sint32 GetProductionFromFranchises();
	void   AddProductionFromFranchise(sint32 amt);

	void IndicateTerrainPolluted(void) { m_terrainPollution = TRUE; }
	bool WasTerrainPolluted(void) const { return m_terrainPollution != FALSE; }

	bool CheckPlayerDead();
	bool IsDead(void) const { return m_isDead != FALSE; }
	void StartDeath(GAME_OVER reason, sint32 data);
	static void RemoveDeadPlayers();
	static Player *GetDeadPlayer(sint32 index);

	sint32 GetTotalResources();
	bool ContinentShared() const;
	void BeginTurnMonopoly() ;
	void CheckResourcesForTutorial() const;
	sint32 GetNumTradeRoutes() const;
	bool HasSameGoodAsTraded() const;
	sint32 GetTradeWith(PLAYER_INDEX third);

	sint32 GetTradeTransportPoints() const { return m_tradeTransportPoints; }
	sint32 GetUsedTradeTransportPoints() const { return m_usedTradeTransportPoints; }

	double GetPercentProductionToMilitary() const;

	Regard *GetRegard() const { return (m_regard) ; }

	ATTITUDE_TYPE GetAttitude(PLAYER_INDEX player) const ;
	void SetAttitude(PLAYER_INDEX player, ATTITUDE_TYPE attitude) ;
	DIPLOMATIC_STRENGTH GetRelativeStrength(PLAYER_INDEX him) const;

	sint32 GetKnowledgeStrength() const;
	sint32 GetMilitaryStrength() const;
	sint32 GetTradeStrength() const;
	sint32 GetTotalPopulation() const;
	sint32 GetPartialPopulation() const;
	sint32 GetLandArea() const;
	sint32 GetSpaceStrength() const;
	sint32 GetUnderseaStrength() const;

	sint32 GetRank(STRENGTH_CAT category) const;

	void MakeCaptureCityPriority(const Unit &city);
	void MakeCaptureRegionPriority(const struct PSlicComplexRegion *region);
	void MakeLeaveRegionPriority(const struct PSlicComplexRegion *region);
	void Surrender();

	sint32 GetCurRound() const;
	bool IsTurnOver() const;

	void BuildDiplomaticSlicMessage(DiplomaticRequest &req);

	void GameOverCheck(sint32 killedBy);
	void GameOver(GAME_OVER reason, sint32 data);

	sint32 GetPoints() const;
	sint32 DeductPoints(sint32 p);
	sint32 AddPoints(sint32 p);
	sint32 SetPoints(sint32 p);
	MBCHAR *GetDescriptionString();

	void TradeUnitsForPoints(const MapPoint &pnt);
	void TradeUnitForPoints(Unit &unit);
	void TradeCityForPoints(Unit &city);
	void TradeImprovementsForPoints(const MapPoint &pnt);

	void RegisterAttack(PLAYER_INDEX against);

	bool GetDeepOceanVisible(void) const { return m_deepOceanVisible != FALSE; }
	void SetDeepOceanVisible(BOOL visible) { m_deepOceanVisible = visible; }

	void ContactMade(PLAYER_INDEX with);
	bool HasSeen(PLAYER_INDEX player);
	bool HasContactWith(PLAYER_INDEX player) const;
	void SetHasAdvance(AdvanceType advance, const bool init = false);
	void GiveArmyCommand(Army &army, UNIT_COMMAND command);
	void CheckWonderObsoletions(AdvanceType advance);

	void Emancipate();

	void AddEndGameObject(Unit &city, sint32 type);

	void AiMakeTradeOffer(uint32 city, sint32 resource, sint32 gold);

	void MakeConvertedCitiesUnhappy(sint32 convertedTo);

	bool CanBuildInfrastructure() const;
	bool CanBuildCapitalization() const;
	bool CanBuildUnit(const sint32 type) const;

	void RemoveEmptyCities(CAUSE_REMOVE_ARMY);
	sint32 GetLastSalePrice(sint32 good);
	void SetLastSalePrice(sint32 good, sint32 price);
	sint32 GetLastAttacked(PLAYER_INDEX player);

	bool CanUseTerraTab( void ) const { return m_can_use_terra_tab != FALSE; }
	bool CanUseSpaceTab( void ) const { return m_can_use_space_tab != FALSE; }
	bool CanUseSeaTab( void ) const { return m_can_use_sea_tab != FALSE; }
	bool CanUseSpaceButton( void ) const { return m_can_use_space_button != FALSE; }

	bool CanStillSendRequestsTo(PLAYER_INDEX otherPlayer);
	void IncrementSentRequests(PLAYER_INDEX otherPlayer);

	void ReconsiderCostOfTrade();
	void RecoveredProbe(const Unit &city);

	void RecreateMessageIcons();
	void ThisMeansWAR(PLAYER_INDEX defense_owner);
	bool HasWarWith(PLAYER_INDEX otherPlayer) const;
	bool HasAllianceWith(PLAYER_INDEX otherPlayer) const;
	bool HasPeaceTreatyWith(PLAYER_INDEX otherPlayer) const;
	bool HasAnyPactWith(PLAYER_INDEX otherPlayer) const;

	void ResetVision();

	void SetGovernorPwReserve(const sint32 &reserve){ m_governorPwReserve = reserve; }

	sint32 GetGovernorPwReserve() const { return m_governorPwReserve; };

	sint32 CountCityHappiness(sint32 &rioting, sint32 &content, sint32 &happy);
	sint32 CityHappinessIncrease() const;
	sint32 CivHappinessIncrease() const;
	sint32 CivProductionBonus() const;
	sint32 CivFoodBonus() const;
	sint32 CivCommerceBonus() const;
	sint32 CivScienceBonus() const;
	sint32 CivBoatBonus() const;
	sint32 CivHpBonus() const;

	sint16 GetCargoCapacity() const;
	void CalcCargoCapacity();
	void AddCargoCapacity(const sint16 delta_cargo_slots);

	GaiaController *GetGaiaController();

	void EnterNewAge(sint32 age);

	sint32 SetResearchGoal(enum DATABASE db, sint32 item);

	void StartResearchingAdvanceForGoal(sint32 goal);
	bool RecursivelyStartResearching(sint32 advance);
	bool HasFreeUnitUpgrades() const;
	sint32 CommodityMarket(); //EMOD
	void CreateLeader(); //EMOD
	bool CanBuildLeader(const sint32 type) const;
	void MergeCivs(sint32 Merger, sint32 Mergee);

	bool HasCostalCities() const;
	bool HasTransporters() const;
	sint32 GetTransporterNum() const;

	bool IsConnected(MapPoint const & center, sint32 maxSquaredDistance, sint32 & distance, bool isLandOnly = true) const;

	MapPoint CalcEmpireCenter() const;

	double EnergySupply(); // Probably make return type void
	double GetEnergySupply() const { return m_energysupply; };

	double BreadBasket();
	double GetBreadBasket() const  { return m_breadbasket;  };

	void   PreResourceCalculation(CityData* city = NULL);

private:
	/// @ToDo: This is a copy from governor, merge both.
	struct CityDist
	{
		CityDist(sint32 city, sint32 dist) : m_city(city), m_dist(dist) {}
		bool   operator<(const CityDist & rval) const { return m_dist < rval.m_dist; }
		sint32 m_city; // This has to be a sint32 otherwise MSVC6 refuses to compile the game.
		sint32 m_dist; // This has to be a sint32 otherwise MSVC6 refuses to compile the game.
	};

	typedef std::vector<CityDist> CityDistQueue;
};

#endif
