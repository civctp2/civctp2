//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Network framework
// Id           : $Id network.h 763 2007-07-15 13:21:26Z richardh $
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
// _DEBUG
// - Generates debug information when set.
//
// _PLAYTEST
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - None
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _NETWORK_H_
#define _NETWORK_H_

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// None

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class Network;
typedef enum {
	NETSTATE_READY,
	NETSTATE_HOSTING,
	NETSTATE_JOINING,
	NETSTATE_SHOWSESSIONS,
} NETSTATE;

#define k_GAME_STYLE_UNIT_MOVES 0x01
#define k_GAME_STYLE_TOTAL_TIME 0x02
#define k_GAME_STYLE_SPEED  0x04
#define k_GAME_STYLE_SIMULTANEOUS 0x08
#define k_GAME_STYLE_CARRYOVER 0x10
#define k_GAME_STYLE_SPEED_CITIES 0x20

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "net_io.h"
#include "net_const.h"
#include "gstypes.h"
#include "GameEventDescription.h"

class UnitData;
class Packetizer;
class NetAction;
class NetInfo;
class CityData;
class TradeRouteData;
class TradeOfferData;
class TerrainImprovementData;
class InstallationData;
class Gold;
class MilitaryReadiness;
class PlayerHappiness;
class UnitMoveList;
class Path;
class AgreementData;
class Agreement;
class CivilisationData;
class DiplomaticRequestData;
class MessageData;
class NetCheat;
class Cell;
class PlayerData;
class Unit;
template <class T> class PointerList;
template <class T> class DynamicArray;
class NetGameObj;
class GAMEOBJ;
class MapPoint;
class NetHash;
class UnitDynamicArray;
class NetOrder;
class Army;
class ArmyData;
class DiplomaticRequest;
class Exclusions;
struct Response;
class ChatList;
class CellUnitList;

#include "player.h" // PLAYER_INDEX

#ifdef _DEBUG
class aui_Surface;
#endif

#define ENQUEUE() { if(g_network.IsActive() && g_network.IsHost()) g_network.Enqueue(this); }

#define k_GAME_STYLE_UNIT_MOVES 0x01
#define k_GAME_STYLE_TOTAL_TIME 0x02
#define k_GAME_STYLE_SPEED  0x04
#define k_GAME_STYLE_SIMULTANEOUS 0x08
#define k_GAME_STYLE_CARRYOVER 0x10
#define k_GAME_STYLE_SPEED_CITIES 0x20

void network_PlayerListCallback(sint32 player, sint32 val, sint32 action);

class SessionData
{
public:
	SessionData(sint32 index, const char* name) {
		m_index = index;
		m_name = new char[strlen(name) + 1];
		strcpy(m_name, name);
	}

	~SessionData() {
		delete [] m_name;
	}

	char* m_name;
	int m_index;
};

class NSPlayerInfo
{
public:
	uint16 m_id;
	char *m_name;
	int m_civ;
	int m_group;
	int m_civpoints;
	int m_settlers;
	NSPlayerInfo(uint16 id, const char *name, int civ, int group, int civpoints,
				 int settlers)
	{
		m_id = id;
		Assert(name);
		if(name) {
			m_name = new char[strlen(name) + 1];
			strcpy(m_name, name);
		} else {
			m_name = new char[1];
			m_name[0] = 0;
		}

		m_civ = civ;
		m_group = group;
		m_civpoints = civpoints;
		m_settlers = settlers;
	}

	~NSPlayerInfo() {
		if(m_name) {
			delete [] m_name;
		}
	}
};

class NSAIPlayerInfo
{
public:
	int m_civ;
	int m_group;
	int m_civpoints;
	int m_settlers;
	NSAIPlayerInfo(int civ, int group, int civpoints, int settlers) {
		m_civ = civ;
		m_group = group;
		m_civpoints = civpoints;
		m_settlers = settlers;
	}
};

class Network : public NetIOResponse
{
public:
	Network();
	virtual ~Network();

	void SetLaunchFromNetFunc(BOOL fromSave);
	void InitFromNetFunc();
	void SetNSPlayerInfo(uint16 id,
						 const char *name,
						 int civ,
						 int group,
						 int civpoints,
						 int settlers);
	void SetNSAIPlayerInfo(int civ, int group, int civpoints,
						   int settlers);
	NSPlayerInfo *GetNSPlayerInfo(sint32 index);
	NSPlayerInfo *GetNSPlayerInfoByID(uint16 id);
	NSAIPlayerInfo *GetNSAIPlayerInfo(sint32 index);

	void Cleanup();

	BOOL IsHost() { return m_initialized && m_iAmHost; };
	BOOL IsClient() { return m_initialized && m_iAmClient; };
	BOOL IsActive() { return m_initialized; };
	BOOL IsLaunchHost() { return m_launchHost; }

	void Process();
	void ProcessSends();
	void Host();
	void EnumSessions();
	void Join(sint32 index);

	void EnumTransport(NET_ERR result,
						sint32 index,
						const char* transname,
						void* transdata);
	void EnumSession(NET_ERR result,
					  sint32 index,
					  const char* sessionName,
					  void* sessionData);
	void SessionReady(NET_ERR result,
					   void* session_data);
	void PacketReady(sint32 from,
					  uint8* buf,
					  sint32 size);
	void AddPlayer(uint16 id,
				const char* name);
	void RemovePlayer(uint16 id);
	void SetToHost();
	void ChangeHost(uint16 id);
	void SessionLost();
	bool ReadyForPackets();

	void SetReady(uint16 id);

	void SyncRand();
	void SyncRand(sint32 index);

	void Enqueue(UnitData*);
	void Enqueue(UnitData *unit, Unit useActor);
	void AddNewUnit(sint32 owner, Unit u);
	void Enqueue(UnitData*, CityData*, BOOL isInitial = FALSE);
	void SendCityName(CityData *city);
	void MoveUnit(UnitData *data, const MapPoint &p);
	void Enqueue(Cell*, sint32 x, sint32 y);

	void Enqueue(TradeRouteData*);
	void Enqueue(TradeOfferData*);

	void Enqueue(NetInfo*);
	void Enqueue(TerrainImprovementData*);
	void Enqueue(InstallationData *);
	void Enqueue(Gold *);
	void Enqueue(MilitaryReadiness *);
	void Enqueue(uint8 owner, PlayerHappiness *hap);
	void Enqueue(AgreementData *data);
	void MakeAgreement(Agreement &a);
	void Enqueue(CivilisationData *data);
	void Enqueue(DiplomaticRequestData *data);
	void SendDiplomaticRequest(DiplomaticRequestData *data);

	void Enqueue(MessageData *data);
	void EnqueuePollution();
	void Enqueue(NetOrder *netorder);
	void Enqueue(ArmyData *armyData);
	void AddNewArmy(sint32 owner, const Army &army);

	void Enqueue(CityData *cd);
	void SendBuildQueue(CityData *cd);

	void SendMessage(MessageData *);
#ifdef _PLAYTEST
	void SendCheat(NetCheat*);
#endif
	void SendAction(NetAction*);
	void SendActionBookmark(NetAction*);
	void SendOrder(sint32 owner, const Army &army, UNIT_ORDER_TYPE order,
				   Path *a_path, const MapPoint &point,
				   sint32 argument, GAME_EVENT event);
	void SendToServer(Packetizer *packet);
	void QueuePacket(uint16 id, Packetizer *packet);
	void QueuePacketBookmark(uint16 id, Packetizer *packet);
	void QueueHeadPacket(uint16 id, Packetizer *packet);
	void QueuePacketToAll(Packetizer *packet);

	void Freeze(uint16 id = 0xffff);
	void Unfreeze(uint16 id = 0xffff);
	void Bookmark(uint16 id = 0xffff);
	void Block(sint32 index);
	void Unblock(sint32 index);

	void SetPlayerIndex(sint32 index, uint16 id);

	sint32 GetPlayerIndex() { return m_playerIndex; }

	sint32 IdToIndex(uint16 id);
	uint16 IndexToId(sint32 index);

	BOOL IsMyTurn() { return m_isMyTurn; }
	BOOL IsLocalPlayer(sint32 index);

	void SetMyTurn(BOOL turn);

	void SetLoop(BOOL loop) { m_inLoop = loop; }

	BOOL DeadUnit(sint32);
	void RemoveDeadUnit(sint32);
	void AddDeadUnit(sint32);
	void ClearDeadUnits();

	void AddCreatedObject(GAMEOBJ *obj);
	void HandleObjectACK(uint32 id);
	void HandleObjectNAK(uint32 myId, uint32 realId);
	void CheckReceivedObject(uint32 id);
	void AddChatText(const MBCHAR *str, sint32 len, uint8 from, BOOL priv);
	void SetChatMask(uint32 mask) { m_chatMask = mask; }
	void SendChatText(const MBCHAR *str, sint32 len);

	void AddCivilization(sint32 index, PLAYER_TYPE pt, sint32 civ);

	void KillPlayer(sint32 p, GAME_OVER reason, sint32 data);
	void GetSliceFor(sint32 player);

	uint8 GetGameStyle() const;
	BOOL IsClassicStyle() const;
	BOOL IsUnitMovesStyle() const;
	BOOL IsSpeedStyle() const;
	BOOL IsTimedStyle() const;
	BOOL IsSimultaneousStyle() const;
	sint32 GetUnitMovesPerSlice() const;
	sint32 GetUnitMovesUsed() const;
	sint32 GetTotalStartTime() const;
	sint32 GetTotalTimeUsed() const;
	sint32 GetTurnStartTime() const;
	sint32 GetTurnStartedAt() const;
	sint32 GetTurnEndsAt() const;
	sint32 GetBonusTime() const;

	void SetClassicStyle(BOOL fromServer = FALSE);
	void SetUnitMovesStyle(BOOL on, sint32 unitMovesPerSlice, BOOL fromServer = FALSE);
	void SetSpeedStyle(BOOL on, sint32 timePerTurn, BOOL fromServer = FALSE,
					   sint32 timePerCity = 0);
	void SetExtraTimePerCity(sint32 t) { m_extraTimePerCity = t; }

	void SetTimedStyle(BOOL on, sint32 timePerGame, BOOL fromServer = FALSE);
	void SetSimultaneousStyle(BOOL on, BOOL fromServer = FALSE);
	void SetCarryoverStyle(BOOL on, BOOL fromServer = FALSE);

	void SetStyleFromServer(uint8 gameStyle,
							sint32 movesPerSlice,
							sint32 totalTime,
							sint32 turnTime,
							sint32 cityTime);
	void UnitsMoved(sint32 count);
	void TurnSync();

	void EnterSetupMode();
	void ExitSetupMode();
	void SignalSetupDone(PLAYER_INDEX player);
	void SetSetupArea(PLAYER_INDEX player, const MapPoint &center, sint32 radius);
	BOOL SetupMode() const;
	BOOL IsInSetupArea(PLAYER_INDEX player, const MapPoint &pnt) const;
	void SetPowerPoints(PLAYER_INDEX player, sint32 points);
	BOOL CanStillSetup(PLAYER_INDEX index);

	BOOL CurrentPlayerAckedBeginTurn();
	void BeginTurn(PLAYER_INDEX player);
	void AckBeginTurn(PLAYER_INDEX player);

	void AddCreatedCity(PLAYER_INDEX player, Unit &city);
	UnitDynamicArray *GetCreatedCities(PLAYER_INDEX player);

	void AddResetCityOwnerHack(const Unit &unit);
	void DoResetCityOwnerHack();

	void SetupPlayerFromNSPlayerInfo(uint16 id, sint32 index);

	sint32 GetNumHumanPlayers();
	BOOL IsNetworkLaunch() { return m_launchFromNetFunc; }
	uint32 GetHumanMask();
	void SetStartingAge(sint32 age) { m_startingAge = age; }
	sint32 GetStartingAge() { return m_startingAge; }
	void SetEndingAge(sint32 age) { m_endingAge = age; }
	sint32 GetEndingAge() { return m_endingAge; }

	BOOL CondensePopMoves() { return m_condensePopMoves; }
	void SetCondensePopMoves(BOOL on);

	BOOL ShouldAckBeginTurn();
	void AddEnact(DiplomaticRequest &req);
	void RemoveEnact(DiplomaticRequest &req);

	const GUID *GetGuid() { return &m_guid; }
	void SetGuid(uint16 id, GUID *guid);

	void KickPlayer(sint32 player);
	sint32 GetProgress();
	void SetProgress(sint32 progress);

	void ResetGuid(sint32 player);
	sint32 CountOpenSlots();
	sint32 CountTakenSlots();
	void OpenPlayer(sint32 player);
	void ClosePlayer(sint32 player);

	void ResetTurnEndsAt();
	BOOL ReadyToStart() { return m_readyToStart; }
	void SetReadyToStart(BOOL ready);
	void SetAllPlayersReady();

	BOOL TeamsEnabled() { return m_teamsEnabled; }

	void SendJoinedMessage(const MBCHAR *name, sint32 player);
	void SendWrongPlayerJoinedMessage(const MBCHAR *name, sint32 player);
	void SendLeftMessage(const MBCHAR *name, sint32 player);
	void SendNewHostMessage(const MBCHAR *name, sint32 player);

	void ProcessNewPlayer(uint16 id);

	void Resync(sint32 playerIndex);
	void StartResync();
	void AckResync(sint32 index);
	void RequestResync(RESYNC_REASON reason);
	BOOL SentResync(sint32 playerIndex);

	uint32 PackedPos(const MapPoint &pnt);
	void UnpackedPos(uint32 p, MapPoint &pnt);

	void SetEndTurnWhenClear() { m_endTurnWhenClear = TRUE; }
	MBCHAR* GetStatusString(sint32 index);

	void SetDynamicJoin(BOOL on);
	BOOL GetDynamicJoin() { return m_dynamicJoin; }

	void ChunkList(uint16 id, PointerList<Packetizer> *a_Lst);
	void DechunkList(sint32 from, uint8 *buf, sint32 len);

	void SetCRCError();

	void SetMaxPlayers(sint32 players);
	void SetRobotName(sint32 player);

	void SendCity(CityData *cd);

	void NotifyDiplomacyResponse(Response &response, sint32 p1, sint32 p2);
	void NotifyDiplomacyThreatRejected(Response &response, const Response &sender_response, sint32 p1, sint32 p2);

	void SetSensitiveUIBlocked(bool block)  { m_sensitiveUIBlocked = block; }
	bool GetSensitiveUIBlocked() { return m_sensitiveUIBlocked; }

	ChatList *GetChatList() { return m_chatList; }

	void SendGroupRequest(const CellUnitList &units, const Army &army);
	void SendUngroupRequest(const Army &army, const CellUnitList &units);

#ifdef _DEBUG
	void DisplayChat(aui_Surface *surf);
	void LogPacket(uint8 c1, uint8 c2, uint16 size);
	void LogSentPacket(uint8 c1, uint8 c2, uint16 size);
	void InitPacketLog();
	void TogglePacketLog();

#define k_NUM_PACKET_TYPES 33
	char m_packetName[k_NUM_PACKET_TYPES][2];
	uint32 m_packetCounter[k_NUM_PACKET_TYPES];
	uint32 m_packetBytes[k_NUM_PACKET_TYPES];
	uint32 m_sentPacketCounter[k_NUM_PACKET_TYPES];
	uint32 m_sentPacketBytes[k_NUM_PACKET_TYPES];
	uint32 m_blockedPackets;

#endif // _DEBUG

private:
	void Init();
	Packetizer* GetHandler(uint8*, uint16);
	sint32 FindEmptySlot(PlayerData *player, uint16 id);
	sint32 FindOldSlot(PlayerData *player, uint16 id);

	BOOL m_launchFromNetFunc;

	NETSTATE m_state;
	NetIO* m_netIO;
	uint16 m_pid;
	uint16 m_hostId;
	BOOL   m_iAmHost;
	BOOL   m_iAmClient;
	sint32 m_initialized;
	BOOL   m_deleting;

	PlayerData *m_playerData[k_MAX_PLAYERS];
	PointerList<PlayerData> *m_newPlayerList;
	PointerList<SessionData> *m_sessionList;
	PointerList<NSPlayerInfo> *m_nsPlayerInfo;
	PointerList<NSAIPlayerInfo> *m_nsAIPlayerInfo;

	DynamicArray<Unit> *m_resetCityOwnerHackList;

	NetHash *m_deadUnitList;

	NetGameObj *m_gameObjects;

	uint32 m_transport;
	char   m_sessionName[256];
	sint32 m_sessionIndex;

	sint32 m_playerIndex;
	BOOL m_isMyTurn;

	BOOL m_inLoop;
	BOOL m_processingNewPlayers;

	BOOL m_sentReadySignal;

	uint8 m_gameStyle;
	sint32 m_unitMovesPerSlice;
	sint32 m_unitMovesUsed;
	sint32 m_totalStartTime;
	sint32 m_totalTimeUsed;
	sint32 m_turnStartTime;
	time_t m_turnStartedAt;
	sint32 m_bonusTime;
	sint32 m_turnEndsAt;
	sint32 m_extraTimePerCity;

	BOOL   m_setupMode;

	BOOL   m_noThread;
	BOOL   m_fromSave;
	BOOL   m_readyToStart;

	sint32 m_startingAge;
	sint32 m_endingAge;

	BOOL m_condensePopMoves;

	sint32 m_progress;

	Exclusions *m_rememberExclusions;

	DynamicArray<DiplomaticRequest> *m_enactedDiplomaticRequests;

	GUID m_guid;
	BOOL m_launchHost;

	BOOL m_teamsEnabled;

	BOOL m_waitingOnResync;
	BOOL m_wasAttached;
	BOOL m_endTurnWhenClear;

	BOOL m_dynamicJoin;
	BOOL m_crcError;

	bool m_sensitiveUIBlocked;

	MBCHAR m_chatStr[k_MAX_CHAT_LEN * 2];
	uint32 m_chatMask;

	ChatList *m_chatList;

	sint32 m_battleViewOpenedTime;
	sint32 m_battleViewOriginalEndTime;

	friend BOOL CALLBACK NetSessionNameDlgProc(HWND hwndDlg,
									UINT uMsg,
									WPARAM wParam,
									LPARAM lParam);
	friend BOOL CALLBACK NetSessionListDlgProc(HWND hwndDlg,
									UINT uMsg,
									WPARAM wParam,
									LPARAM lParam);

	friend class NetAction;

#ifdef _DEBUG
	BOOL m_displayPackets;
#endif
};

extern Network g_network;

#endif
