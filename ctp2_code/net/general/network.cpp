//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Network object bookkeeping gets cleared when exiting a game (exiting and 
//   rejoining occasionally left something lying around, resulting in a resync
//   not long after joining) (bug #30)
// - Updated the above to prevent an invalid second delete.
// - Feat tracking added.
// - Memory leaks repaired.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Cell.h"


#include "net_types.h"
#include "network.h"
#include "net_io.h"
#include "net_anet.h"
#include "net_thread.h"
#include "net_packet.h"
#include "net_cell.h"
#include "net_util.h"
#include "net_unit.h"
#include "net_action.h"
#include "net_info.h"
#include "net_city.h"
#include "net_diff.h"
#include "net_player.h"
#include "net_traderoute.h"
#include "net_tradeoffer.h"
#include "net_rand.h"
#include "net_terrain.h"
#include "net_installation.h"
#include "net_ready.h"
#include "net_happy.h"
#include "net_report.h"
#include "net_order.h"
#include "net_agreement.h"
#include "net_civ.h"
#include "net_diplomacy.h"
#include "net_message.h"
#include "net_pollution.h"
#include "net_keys.h"
#include "net_gamesettings.h"
#include "net_army.h"
#include "net_playerdata.h"
#include "net_gameobj.h"
#include "net_chat.h"
#include "net_crc.h"
#include "net_wonder.h"
#include "net_achievement.h"
#include "net_vision.h"
#include "net_exclusions.h"
#include "net_research.h"
#include "net_guid.h"
#include "net_strengths.h"
#include "net_endgame.h"
#include "net_world.h"
#include "net_feat.h"

#ifdef _PLAYTEST
#include "net_cheat.h"
#endif


#include "UnitData.h"
#include "Player.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "UnitPool.h"
#include "CityData.h"
#include "TradeRouteData.h"
#include "Gold.h"
#include "Path.h"
#include "Agreement.h"
#include "CivilisationPool.h"
#include "Civilisation.h"
#include "TradeOffer.h"
#include "TerrImprove.h"
#include "Installation.h"
#include "DiplomaticRequest.h"
#include "DiplomaticRequestPool.h"
#include "Message.h"
#include "TradePool.h"
#include "TurnCnt.h"
#include "TiledMap.h"
#include "RadarMap.h"
#include "ChatBox.h"
#include "ArmyData.h"
#include "ArmyPool.h"
#include "Order.h"
#include "UnseenCell.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "Exclusions.h"



#include "profileDB.h"
#include "PointerList.h"

#include "c3_utilitydialogbox.h"


#include "netfunc.h"
#include "netshell.h"

#include "civapp.h"

#include "StrDB.h"
extern StringDB					*g_theStringDB;

#include "GameSettings.h"
#include "AgeRecord.h"
#include "CivilisationDB.h"

#include "GameEventManager.h"

#include "CtpAI.h"
#include "ChatList.h"

#include "SoundManager.h"
#include "gamesounds.h"

#include "progresswindow.h"
extern ProgressWindow *g_theProgressWindow;

extern TurnCount *g_turn;
extern TiledMap *g_tiledMap;
extern RadarMap     *g_radarMap;
extern ProfileDB *g_theProfileDB;
extern NETFunc *g_netfunc;
extern DiplomaticRequestPool *g_theDiplomaticRequestPool;
extern CivApp *g_civApp;
extern CivilisationDatabase *g_theCivilisationDB;


#include "SelItem.h"


#include "resource.h"

#include "Director.h"

#include "GameOver.h"
#include "civ3_main.h"

#include "sci_advancescreen.h"

#include "c3_utilitydialogbox.h"

#include "aui_button.h"
#ifdef _DEBUG
#include "aui.h"
#include "aui_surface.h"
#include "primitives.h"
#endif

#include "controlpanelwindow.h"
#include "MainControlPanel.h"
extern ControlPanelWindow		*g_controlPanel;

#include "RandGen.h"

#include "stringutils.h"

#include "screenutils.h"

#include "battleviewwindow.h"
#include "c3ui.h"

#include "sci_advancescreen.h"

#include "DipWizard.h"
#include "Diplomat.h"

#include "CTP2Combat.h"
#include "Strengths.h"

extern C3UI *g_c3ui;

#define k_CHUNK_HEAD '>'
#define k_CHUNK_BODY 'C'

extern ChatBox *g_chatBox;


extern c3_UtilityPlayerListPopup *g_networkPlayersScreen;

void battleview_ExitButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );


void network_AbortCallback( sint32 type )
{
	


	g_civApp->PostQuitToLobbyAction();
	
}

namespace
{

//----------------------------------------------------------------------------
//
// Name       : PacketManager
//
// Description: Simple automatic owner class to prevent memory leaks.
//
// Parameters : a_Packet	: packet to own
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Assumption: a_Packet != NULL
//
//----------------------------------------------------------------------------
	class PacketManager
	{
	public:
		PacketManager(Packetizer * a_Packet)
		:	m_Packet	(a_Packet)
		{
			Assert(m_Packet);
			m_Packet->AddRef();
		};

		virtual ~PacketManager()
		{
			m_Packet->Release();
		};
	
	private:
		Packetizer *	m_Packet;
	};

} // namespace

Network::Network() :
	m_initialized(FALSE),
	m_pid(0),
	m_hostId(0),
	m_iAmHost(FALSE),
	m_iAmClient(FALSE),
	m_state(NETSTATE_READY),
	m_processingNewPlayers(FALSE)
{
	m_noThread = FALSE;

	if(m_noThread) {
		m_netIO = new ActivNetIO;
		m_netIO->Init(this);
	} else {
		m_netIO = new NetThread;
		m_netIO->Init(this);
	}

	for(uint16 i = 0; i < k_MAX_PLAYERS; i++) {
		m_playerData[i] = NULL;
	}
	m_transport = 5;
	m_sessionIndex = -1;
	m_chatMask = 0xffffffff;
#ifdef _DEBUG
	InitPacketLog();
	m_displayPackets = FALSE;
#endif

	m_sentReadySignal = FALSE;
	m_deleting = FALSE;

	m_gameStyle = 0;
	m_setupMode = FALSE;

	m_newPlayerList = new PointerList<PlayerData>;
	m_sessionList = new PointerList<SessionData>;
	m_gameObjects = new NetGameObj();
	m_deadUnitList = new NetHash;

	m_launchFromNetFunc = FALSE;

	m_resetCityOwnerHackList = new DynamicArray<Unit>;
	m_nsPlayerInfo = new PointerList<NSPlayerInfo>;
	m_nsAIPlayerInfo = new PointerList<NSAIPlayerInfo>;

	m_startingAge = 0;

	m_condensePopMoves = FALSE;
	m_enactedDiplomaticRequests = new DynamicArray<DiplomaticRequest>;

	
	

	
	

	char exepath[_MAX_PATH];
	if(GetModuleFileName(NULL, exepath, _MAX_PATH) != 0) {
		char *lastbackslash = strrchr(exepath, '\\');
		if(lastbackslash) {
			*lastbackslash = 0;
			SetCurrentDirectory(exepath);
		}
	}

	FILE *guidFile = fopen("nguid.ctp", "rb");
	if(!guidFile) {
		CoCreateGuid(&m_guid);
		guidFile = fopen("nguid.ctp", "wb");
		Assert(guidFile);
		if(guidFile) {
			fwrite(&m_guid, sizeof(m_guid), 1, guidFile);
			fclose(guidFile);
		}
	} else {
		sint32 r = fread(&m_guid, 1, sizeof(m_guid), guidFile);
		Assert(r == sizeof(m_guid));
		fclose(guidFile);
	}

	m_progress = -1;
	m_extraTimePerCity = 0;
	m_launchHost = FALSE;
	m_rememberExclusions = NULL;
	m_teamsEnabled = FALSE;
	m_waitingOnResync = FALSE;
	m_wasAttached = FALSE;
	m_endTurnWhenClear = FALSE;
	m_dynamicJoin = FALSE;
	m_crcError = FALSE;
	m_sensitiveUIBlocked = false;

	m_chatList = new ChatList;
}

Network::~Network()
{
	m_deleting = TRUE;

	if(m_netIO) {
		delete m_netIO;
		m_netIO = NULL;
	}

	for(uint16 i = 0; i < k_MAX_PLAYERS; i++) {
		if(m_playerData[i]) {
			delete m_playerData[i];
			m_playerData[i] = NULL;
		}
	}
	SessionData* ses;
	while(!m_sessionList->IsEmpty()) {
		ses = m_sessionList->RemoveHead();
		delete ses;
	}

	if(m_newPlayerList)
		delete m_newPlayerList;

	if(m_sessionList)
		delete m_sessionList;

	if(m_gameObjects)
		delete m_gameObjects;

	if(m_deadUnitList)
		delete m_deadUnitList;

	if(m_resetCityOwnerHackList)
		delete m_resetCityOwnerHackList;

	if(m_nsPlayerInfo) {
		m_nsPlayerInfo->DeleteAll();
		delete m_nsPlayerInfo;
	}

	if(m_nsAIPlayerInfo) {
		m_nsAIPlayerInfo->DeleteAll();
		delete m_nsAIPlayerInfo;
	}

	if(m_enactedDiplomaticRequests) {
		delete m_enactedDiplomaticRequests;
		m_enactedDiplomaticRequests = NULL;
	}

	if(m_rememberExclusions) {
		delete m_rememberExclusions;
		m_rememberExclusions = NULL;
	}

	delete m_chatList;
}



void
Network::Cleanup()
{
	uint16 i;

	
	c3_RemoveAbortMessage();

	if(m_netIO) {
		m_netIO->Reset();
		
		
	}

	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(m_playerData[i]) {
			delete m_playerData[i];
			m_playerData[i] = NULL;
		}
	}

	SessionData* ses;
	while(!m_sessionList->IsEmpty()) {
		ses = m_sessionList->RemoveHead();
		delete ses;
	}

	m_initialized = FALSE;
	m_pid = 0;
	m_hostId = 0;
	m_iAmHost = FALSE;
	m_iAmClient = FALSE;
	m_state = NETSTATE_READY;
	m_processingNewPlayers = FALSE;
	m_readyToStart = FALSE;
	m_crcError = FALSE;

	
	
	
	
	
	
	
	m_transport = 5;
	m_sessionIndex = -1;
	m_setupMode = FALSE;
	
	m_launchFromNetFunc = FALSE;

	if(m_nsPlayerInfo) {
		m_nsPlayerInfo->DeleteAll();
	}

	if(m_nsAIPlayerInfo) {
		m_nsAIPlayerInfo->DeleteAll();
	}

	m_startingAge = 0;

	m_enactedDiplomaticRequests->Clear();
	m_sentReadySignal = FALSE;
	m_launchHost = FALSE;
	m_teamsEnabled = FALSE;
	m_waitingOnResync = FALSE;

	m_wasAttached = FALSE;
	m_endTurnWhenClear = FALSE;

	m_dynamicJoin = FALSE;

	if(!g_exclusions) {
		g_exclusions = new Exclusions();
	}

	if(m_rememberExclusions) {
		delete m_rememberExclusions;
		m_rememberExclusions = NULL;
	}

	if(g_networkPlayersScreen) {
		g_networkPlayersScreen->RemoveWindow();
	}

	if(m_newPlayerList) {
		m_newPlayerList->DeleteAll();
	}

	m_sensitiveUIBlocked = false;
}

void Network::SetLaunchFromNetFunc(BOOL fromSave)
{
	m_launchFromNetFunc = TRUE;
	m_fromSave = fromSave;
	m_readyToStart = FALSE;
	m_launchHost = g_netfunc->IsHost();

	
	
	m_rememberExclusions = g_exclusions;
	g_exclusions = NULL;

	if(!m_noThread) {
		((NetThread *)m_netIO)->SetDP(g_netfunc->GetDP());
	}

	m_newPlayerList->DeleteAll();
}

void Network::InitFromNetFunc()
{
	m_initialized = TRUE;
	m_iAmHost = g_netfunc->IsHost();
	m_iAmClient = !m_iAmHost;

	Assert(g_player && g_player[0]);
	g_player[0]->SetPlayerType(PLAYER_TYPE_ROBOT);

	m_battleViewOpenedTime = -1;
	m_battleViewOriginalEndTime = 0;

	if(m_fromSave) {
		
		
		
		sint32 i, numLegalSlots = 1;
		GUID zeroGuid;
		memset(&zeroGuid, 0, sizeof(GUID));
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i] && memcmp(&zeroGuid, &g_player[i]->m_networkGuid, sizeof(GUID))) {
				numLegalSlots++;
				OpenPlayer(i);
			} else if(g_player[i]) {
				ClosePlayer(i);
			}
		}
		g_gamesetup.SetSize(numLegalSlots);
		SetMaxPlayers(numLegalSlots);
		if(!g_exclusions) {
			
			g_exclusions = m_rememberExclusions;
			m_rememberExclusions = NULL;
		}
	} else {
		if(m_rememberExclusions) {
			if(g_exclusions) {
				delete g_exclusions;
			}
			g_exclusions = m_rememberExclusions;
			m_rememberExclusions = NULL;
		}
	}

	if(m_noThread) {
		((ActivNetIO *)m_netIO)->SetDP(g_netfunc->GetDP());
	} else {
		
	}

	if(!m_readyToStart) {
		if(m_iAmHost) {
			if(!m_launchHost) {
				
				
				g_civApp->PostQuitToLobbyAction();
				m_readyToStart = TRUE;
				return;
			}
			
			
			
			const char *str = g_theStringDB->GetNameStr("NETWORK_WAITING_ON_PLAYERS");
			
			char nonConstStr[1024];
			if(str)
				strcpy(nonConstStr, str);
			c3_AbortMessage( str ? nonConstStr : "Waiting on players", k_UTILITY_ABORT, network_AbortCallback );
		} else if(!m_crcError) {
			const char *str = g_theStringDB->GetNameStr("NETWORK_WAITING_FOR_DATA");
			char nonConstStr[1024];
			if(str)
				strcpy(nonConstStr, str);
			
			c3_AbortMessage( str ? nonConstStr : "Waiting on data", k_UTILITY_PROGRESS_ABORT, network_AbortCallback );
		}
	}
}

void Network::SetNSPlayerInfo(uint16 id,
							  char *name,
							  int civ,
							  int group,
							  int civpoints,
							  int settlers)
{
	if(group > 0) {
		m_teamsEnabled = TRUE;
	}

	m_nsPlayerInfo->AddTail(new NSPlayerInfo(id, name, civ, group, civpoints,
											 settlers));
}

void Network::SetNSAIPlayerInfo(int civ,
								int group,
								int civpoints,
								int settlers)
{
	m_nsAIPlayerInfo->AddTail(new NSAIPlayerInfo(civ, group, civpoints, settlers));
}


NSPlayerInfo *Network::GetNSPlayerInfo(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_nsPlayerInfo->GetCount());
	if(index < 0 || index >= m_nsPlayerInfo->GetCount()) {
		return NULL;
	}
	sint32 c = 0;
	PointerList<NSPlayerInfo>::Walker walk(m_nsPlayerInfo);
	while(walk.IsValid()) {
		if(c == index) {
			return walk.GetObj();
		}
		walk.Next();
		c++;
	}
	Assert(FALSE);
	return NULL;
}

NSPlayerInfo *Network::GetNSPlayerInfoByID(uint16 id)
{
	PointerList<NSPlayerInfo>::Walker walk(m_nsPlayerInfo);
	while(walk.IsValid()) {
		if(id == walk.GetObj()->m_id)
			return walk.GetObj();
		walk.Next();
	}
	return NULL;
}

NSAIPlayerInfo *Network::GetNSAIPlayerInfo(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_nsAIPlayerInfo->GetCount());
	if(index < 0 || index >= m_nsAIPlayerInfo->GetCount())
		return NULL;

	sint32 i = 0;
	PointerList<NSAIPlayerInfo>::Walker walk(m_nsAIPlayerInfo);
	while(walk.IsValid()) {
		if(i == index) {
			return walk.GetObj();
		}
		walk.Next();
		i++;
	}
	Assert(FALSE);
	return NULL;
}


void
Network::Process()
{
	NET_ERR err;

	if(m_launchFromNetFunc) {
		InitFromNetFunc();
		m_launchFromNetFunc = FALSE;
	}

	m_chatList->RemoveExpired();

	if(!m_initialized)
		return;

	DoResetCityOwnerHack();

	
	
	
	
	

	ProcessSends();

	switch(m_state) {
	case NETSTATE_JOINING:
		Join(m_sessionIndex);
		m_state = NETSTATE_READY;
		break;
	case NETSTATE_SHOWSESSIONS:
		break;
	default:
		break;
	}

	if(!m_sentReadySignal && m_netIO->ReadyForData()) {
		if(m_hostId == 0) {
			m_netIO->GetHostId(m_hostId);
		}

		if(m_hostId == 0) {
			Assert(m_iAmClient);
		} else {
			uint8 buf[512];
			uint16 size;
			NetGuid *guid = new NetGuid(GetGuid());
			guid->AddRef();
			guid->Packetize(buf, size);
			NET_ERR err = m_netIO->Send(m_hostId, TRUE, buf, size);
			guid->Release();
			Assert(err == NET_ERR_OK);

			NetReport *report = new NetReport(NET_REPORT_READY_FOR_DATA);
			report->AddRef();
			report->Packetize(buf, size);
			err = m_netIO->Send(m_hostId, TRUE, buf, size);
			report->Release();
			
			Assert(err == NET_ERR_OK);
			
			m_sentReadySignal = TRUE;
		}
	}

	


	if(m_netIO) {
		err = m_netIO->Idle();
		Assert(err == NET_ERR_OK);
		if(err != NET_ERR_OK)
			return;
	}

	static sint32 battleEndedTime = -1;

	
	if(g_battleViewWindow && g_c3ui->GetWindow(g_battleViewWindow->Id()) && (!g_theCurrentBattle || g_theCurrentBattle->IsDone())) {
		if(battleEndedTime < 0) {
			battleEndedTime = time(0);
		} else if(battleEndedTime + 30 < time(0)) {
			battleview_ExitButtonActionCallback(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);
			battleEndedTime = -1;
		}
	} else {
		battleEndedTime = -1;
	}

		
	if(m_gameStyle & (k_GAME_STYLE_SPEED | k_GAME_STYLE_SPEED_CITIES)) {

		sint32 timeNow = time(0); 

		bool diplomacyShouldPause = false;
		if(!DipWizard::CanInitiateRightNow() && IsMyTurn()) {
			
			sint32 p;
			for(p = 0; p < k_MAX_PLAYERS; p++) {
				if(!g_player[p] || p == g_selected_item->GetVisiblePlayer())
					continue;
				if(Diplomat::GetDiplomat(GetPlayerIndex()).GetReceiverHasInitiative(p)) {
					
					diplomacyShouldPause = true;
					break;
				}
			}
		}

		
		if(g_c3ui && 
		   (g_battleViewWindow && g_c3ui->GetWindow(g_battleViewWindow->Id())) ||
		   (diplomacyShouldPause)) {
			if(m_battleViewOpenedTime < 0) {
				m_battleViewOpenedTime = timeNow;
			m_battleViewOriginalEndTime = m_turnEndsAt;
			}
			m_turnEndsAt = m_battleViewOriginalEndTime + (timeNow - m_battleViewOpenedTime);
		} else {
			m_battleViewOpenedTime = -1;
		}

		if(g_selected_item) {
			if(g_selected_item->GetCurPlayer() ==
			   g_selected_item->GetVisiblePlayer() &&
			   IsMyTurn() && (!m_iAmHost || m_readyToStart)) {
				if(timeNow >= m_turnEndsAt) {
					if(sci_advancescreen_isOnScreen()) {
						sci_advancescreen_removeMyWindow(AUI_BUTTON_ACTION_EXECUTE);
					}
					g_director->AddEndTurn();
					
				}
			}
		}
	}

	if(m_gameStyle & k_GAME_STYLE_TOTAL_TIME) {
		if(IsMyTurn() && 
		   (m_totalTimeUsed + (time(0) - m_turnStartedAt) > m_totalStartTime)) {
			
			g_player[g_selected_item->GetCurPlayer()]->
				GameOver(GAME_OVER_LOST_OUT_OF_TIME, -1);
		}
	}
}

void Network::ProcessSends()
{
	NET_ERR err;
	BOOL isBusy;
	for(sint32 pl = 0; pl < k_MAX_PLAYERS; pl++) {
		if(!g_player[pl]) continue;
		if(m_playerData[pl] && !m_playerData[pl]->m_frozen && m_playerData[pl]->m_ready) {
			PointerList<Packetizer>* packetList = m_playerData[pl]->m_packetList;

			isBusy = FALSE;

			
			while(m_playerData[pl] && !packetList->IsEmpty() && !isBusy) {
				uint8 buf[8192];
				uint8 *sbuf;
				sint32 size;
				Packetizer* packet = packetList->RemoveHead();
				if(!packet->m_packetbuf) {
					Assert(FALSE);
					uint16 psize;
					packet->Packetize(buf, psize);
					size = (sint32)psize;
					sbuf = buf;
				} else {
					sbuf = packet->m_packetbuf;
					size = packet->m_packetsize;
				}

				if(packet->ShouldSendCompressed()) {
					err = m_netIO->SendCompressed(m_playerData[pl]->m_id,
											packet->m_reliability,
											sbuf, size);
				} else {
					err = m_netIO->Send(m_playerData[pl]->m_id,
										packet->m_reliability,
										sbuf, size);
				}
				switch(err) {
					case NET_ERR_OK:
						if(packet->m_unitId != 0) {
							m_playerData[pl]->m_unitHash.Remove(
								packet->m_unitId);
						}

#ifdef _DEBUG
						LogSentPacket(sbuf[0], sbuf[1], size);
#endif
						break;
					case NET_ERR_WOULDBLOCK:
						
						isBusy = TRUE;
						if (m_playerData[pl])
						{
							// Reinsert for retry
							packet->AddRef();
							packetList->AddHead(packet);
						}
						break;
					case NET_ERR_INVALIDADDR:
						RemovePlayer(m_playerData[pl]->m_id);
						break;
					default:
						Assert(FALSE);
						break;
				}
				packet->Release();
			}

			if(!m_playerData[pl])
				continue;

#ifdef _DEBUG
			if(isBusy) {
				m_blockedPackets++;
			}
#endif
		}
	}
}



void Network::Init()
{
	NET_ERR err;
	if(!m_initialized) {
		m_transport = 5;
		

		
		err = m_netIO->EnumTransports();
		if(err == NET_ERR_OK) {
			err = m_netIO->SetTransport(m_transport);
		}

		if(err == NET_ERR_OK) {
			if(!g_theProfileDB->UseIPX()) {
				
				
				err = m_netIO->SetLobby("california12.activision.com");
			}
		}
		if(err == NET_ERR_OK) {
			m_initialized = TRUE;
		}
	}
}


void
Network::Host()
{
}


void
Network::EnumSessions()
{
}


void Network::Join(sint32 index )
{
}


void
Network::EnumTransport(NET_ERR result, 
					   sint32 index,      
					   const char* transname, 
					   void* transdata) 
{
	DPRINTF(k_DBG_NET, ("Transport %d: %s\n", index, transname));

	if(g_theProfileDB->UseIPX()) {
		if(strstr(transname, "wipx2d.dll")) {
			m_transport = index;
		}
	} else {
		if(strstr(transname, "winets2d.dll")) {
			
			m_transport = index;
		}
	}
}


void
Network::EnumSession(NET_ERR result, 
					 sint32 index,      
					 const char* sessionName, 
					 void* sessionData) 
{
	if(result == NET_ERR_OK) {
		DPRINTF(k_DBG_NET, ("Session %d: %s\n", index, sessionName));
		SessionData* sessionData = new SessionData(index, sessionName);
		m_sessionList->AddTail(sessionData);
		if(strcmp(sessionName, m_sessionName) == 0) {
			m_sessionIndex = index;
		}
	} else if(result == NET_ERR_NOMORESESSIONS) {
		m_state = NETSTATE_SHOWSESSIONS;
	}
}


void Network::SessionReady(NET_ERR result,     
						   void* session_data) 
{
	DPRINTF(k_DBG_NET, ("Session Ready\n"));
	m_netIO->GetMyId(m_pid);
	if(m_iAmHost) {
		ProcessNewPlayer(m_pid);
	}
}


Packetizer*
Network::GetHandler(uint8* buf, 
					uint16 size) 
{
	Packetizer *handler = NULL;
	switch(MAKE_CIV3_ID(buf[0], buf[1])) {
		case k_PACKET_CELL_ID:			handler = new NetCellData; break;
		case k_PACKET_CELL_LIST_ID:		handler = new NetCellList; break;
		case k_PACKET_UNIT_ID:			handler = new NetUnit; break;
		case k_PACKET_ACTION_ID:		handler = new NetAction; break;
		case k_PACKET_INFO_ID:			handler = new NetInfo; break;
		case k_PACKET_CITY_ID:			handler = new NetCity; break;
		case k_PACKET_DIFFICULTY_ID:    handler = new NetDifficulty; break;
		case k_PACKET_PLAYER_ID:        handler = new NetPlayer; break;
		case k_PACKET_TRADE_ROUTE_ID:   handler = new NetTradeRoute; break;
		case k_PACKET_TRADE_OFFER_ID:   handler = new NetTradeOffer; break;
		case k_PACKET_RAND_ID:          handler = new NetRand; break;
		case k_PACKET_TERRAIN_ID:       handler = new NetTerrainImprovement; break;
		case k_PACKET_INSTALLATION_ID:  handler = new NetInstallation; break;
		case k_PACKET_CHAT_ID:          handler = new NetChat; break;
		case k_PACKET_READINESS_ID:     handler = new NetReadiness; break;
		case k_PACKET_HAPPY_ID:         handler = new NetHappy; break;
		case k_PACKET_PLAYER_HAPPY_ID:  handler = new NetPlayerHappy; break;
		case k_PACKET_REPORT_ID:        handler = new NetReport; break;
		case k_PACKET_UNIT_MOVE_ID:     handler = new NetUnitMove; break;
		case k_PACKET_UNIT_ORDER_ID:    handler = new NetOrder; break;
		case k_PACKET_AGREEMENT_ID:     handler = new NetAgreement; break;
		case k_PACKET_CIVILIZATION_ID:  handler = new NetCivilization; break;
		case k_PACKET_CITY_NAME_ID:     handler = new NetCityName; break;
		case k_PACKET_DIP_PROPOSAL_ID:  handler = new NetDipProposal; break;
		case k_PACKET_DIP_RESPONSE_ID:  handler = new NetDipResponse; break;
		case k_PACKET_MESSAGE_ID:       handler = new NetMessage; break;
		case k_PACKET_CITY2_ID:         handler = new NetCity2; break;
		case k_PACKET_POLLUTION_ID:     handler = new NetPollution; break;
		case k_PACKET_CITY_BQ_ID:       handler = new NetCityBuildQueue; break;
		case k_PACKET_KEYS_ID:          handler = new NetKeys; break;
		case k_PACKET_GAME_SETTINGS_ID: handler = new NetGameSettings; break;
		case k_PACKET_NEW_ARMY_ID:      handler = new NetNewArmy; break;
		case k_PACKET_REMOVE_ARMY_ID:   handler = new NetRemoveArmy; break;
		case k_PACKET_CRC_ID:           handler = new NetCRC; break;
		case k_PACKET_ARMY_ID:          handler = new NetArmy; break;
		case k_PACKET_WONDER_TRACKER_ID: handler= new NetWonderTracker; break;
		case k_PACKET_ACHIEVEMENT_TRACKER_ID: handler= new NetAchievementTracker; break;
		case k_PACKET_VISION_ID:        handler = new NetVision; break;
		case k_PACKET_UNSEEN_CELL_ID:   handler = new NetUnseenCell; break;
		case k_PACKET_EXCLUSIONS_ID:    handler = new NetExclusions; break;
		case k_PACKET_RESOURCES_ID:     handler = new NetCityResources; break;
		case k_PACKET_UNIT_HP_ID:       handler = new NetUnitHP; break;
		case k_PACKET_CELL_UNIT_ORDER_ID: handler = new NetCellUnitOrder; break;
		case k_PACKET_ADD_PLAYER_ID:    handler = new NetAddPlayer; break;
		case k_PACKET_RESEARCH_ID:      handler = new NetResearch; break;
		case k_PACKET_GUID_ID:          handler = new NetGuid; break;
		case k_PACKET_STRENGTH_ID:      handler = new NetStrengths; break;
		case k_PACKET_FULL_STRENGTHS_ID: handler = new NetFullStrengths; break;
		case k_PACKET_NET_INFO_MESSAGE_ID: handler = new NetInfoMessage; break;
		case k_PACKET_ENDGAME_ID:       handler = new NetEndGame; break;
		case k_PACKET_WORMHOLE_ID:      handler = new NetWormhole; break;
		case k_PACKET_SET_PLAYER_GUID_ID: handler = new NetSetPlayerGuid; break;
		case k_PACKET_SET_LEADER_NAME_ID: handler = new NetSetLeaderName; break;
		case k_PACKET_WORLD_ID:           handler = new NetWorld; break;
		case k_PACKET_DIP_AGREEMENT_MATRIX_ID: handler = new NetAgreementMatrix; break;
		case k_PACKET_GROUP_REQUEST_ID: handler = new NetGroupRequest; break;
		case k_PACKET_UNGROUP_REQUEST_ID: handler = new NetUngroupRequest; break;
		case k_PACKET_SCORES_ID:          handler = new NetScores; break;

		case k_PACKET_FEAT_TRACKER_ID:	handler = new NetFeatTracker(); break;

#ifdef _DEBUG
		case k_PACKET_CHEAT_ID:         handler = new NetCheat; break;
#endif
	}
	if(handler) {
		handler->AddRef();
	}
#ifdef _DEBUG
	LogPacket(buf[0], buf[1], size);
#endif
	return handler;
}


void Network::PacketReady(sint32 from, 
						  uint8* buf, 
						  sint32 size) 
{
	if(m_deleting)
		return;
	if(buf[0] == k_CHUNK_HEAD &&
	   buf[1] == k_CHUNK_BODY) {
		
		DechunkList(from, &buf[2], size - 2);
	} else {
		Packetizer* handler = GetHandler(buf, size);
		Assert(handler != NULL);
		if(handler) {
			handler->Unpacketize((uint16)from, buf, size);
			handler->Release();
		}
		
		
	}
}


void Network::AddPlayer(uint16 id, 
						char* name) 
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetAddPlayer(id, name));
	}
	
	for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
		if(m_playerData[i] && m_playerData[i]->m_id == id) {
			DPRINTF(k_DBG_NET, ("AddPlayer(%d) but already have that player.\n",
								id));
			return;
		}
	}

	PointerList<PlayerData>::Walker walk(m_newPlayerList);
	while(walk.IsValid()) {
		if(walk.GetObj()->m_id == id) {
			DPRINTF(k_DBG_NET, ("AddPlayer(%d), but player %d (%s) is already in the new player list\n",
								id, name));
			return;
		}
		walk.Next();
	}
	DPRINTF(k_DBG_NET, ("Adding player %s (id=%d)\n", name, id));
	m_newPlayerList->AddTail(new PlayerData(name, (uint16)id));
}


void Network::RemovePlayer(uint16 id) 
{
	DPRINTF(k_DBG_NET, ("Removing player %d\n", id));

	if(id == m_pid) {
		SessionLost();
		// removing object bookkeeping
		delete m_gameObjects;
		m_gameObjects = new NetGameObj();
	}

	if(m_deleting)
		return;

	sint32 index = IdToIndex(id);
	
	if(index < 0) {
		
		PointerList<PlayerData>::Walker walk(m_newPlayerList);
		while(walk.IsValid()) {
			if(walk.GetObj()->m_id == id) {
				walk.Remove();
				if(m_iAmHost && m_newPlayerList->IsEmpty() && !m_readyToStart) {
					ResetTurnEndsAt();
					c3_RemoveAbortMessage();
					SetReadyToStart(TRUE);
				}
				return;
			}
			walk.Next();
		}

		
		Assert(FALSE);
		return;
	}

	
	if(m_playerData[index]) {
		char *name = new char[strlen(m_playerData[index]->m_name) + 1];
		strcpy(name, m_playerData[index]->m_name);

		delete m_playerData[index];
		m_playerData[index] = NULL;

		if(m_iAmHost && g_player[index] && !g_player[index]->m_isDead) {
			SendLeftMessage(name, index);
		}
		delete [] name;

	}

	if(index == g_selected_item->GetCurPlayer()) {
		m_enactedDiplomaticRequests->Clear();
	}

	if(m_iAmHost) {

		if(g_player[index]) {
			g_player[index]->SetPlayerType(PLAYER_TYPE_ROBOT);
			if(index == g_selected_item->GetCurPlayer()) {
				g_director->AddEndTurn();
			}

			SetRobotName(index);
		}
		if(g_player[index] && !g_player[index]->m_isDead)
			OpenPlayer(index);
		else
			ClosePlayer(index);

		SetMaxPlayers(CountOpenSlots() + CountTakenSlots());
	} else {
		
		if(g_player[index]) {
			g_player[index]->m_openForNetwork = TRUE;
		}
	}
	if(g_networkPlayersScreen) {
		g_networkPlayersScreen->UpdateData();
	}

}


void Network::SetToHost()
{
	if(!m_readyToStart) {
		g_civApp->PostQuitToLobbyAction();
		m_readyToStart = TRUE;
		return;
	}

	m_iAmHost = TRUE;
	m_iAmClient = FALSE;
	if(!m_deleting) {
		if(g_player[m_playerIndex]) {
			g_player[m_playerIndex]->SetPlayerType(PLAYER_TYPE_HUMAN);
			if(g_player[g_selected_item->GetCurPlayer()]->GetPlayerType() == PLAYER_TYPE_HUMAN) {
				SetMyTurn(TRUE);
			}
		}

		g_turn->NotifyBecameHost();

		sint32 p;
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			if(p == m_playerIndex)
				continue;
			if(!g_player[p])
				continue;

			if(!m_playerData[p]) {
				
				g_player[p]->SetPlayerType(PLAYER_TYPE_ROBOT);
				SetRobotName(p);
			} else {
				
				
				
				g_player[p]->SetPlayerType(PLAYER_TYPE_NETWORK);
				Resync(p);
			}
		}

		SendNewHostMessage(m_playerData[m_playerIndex]->m_name,
						   m_playerIndex);
		SetMaxPlayers(CountOpenSlots() + CountTakenSlots());

		if(!g_player[g_selected_item->GetCurPlayer()] ||
		   g_player[g_selected_item->GetCurPlayer()]->m_playerType == PLAYER_TYPE_ROBOT) {
			DPRINTF(k_DBG_GAMESTATE, ("Set to host, cur player (%d) is robot, adding EndTurn\n", g_selected_item->GetCurPlayer()));
			g_director->AddEndTurn();
		}
	}
}

void Network::ChangeHost(uint16 id)
{
	if(m_hostId != 0) {
		
		m_readyToStart = FALSE;
	}
	m_hostId = id;
	const char *str = g_theStringDB->GetNameStr("NETWORK_WAITING_FOR_DATA");
	char nonConstStr[1024];
	if(str)
		strcpy(nonConstStr, str);
	c3_AbortMessage( str ? nonConstStr : "Waiting on data", k_UTILITY_PROGRESS_ABORT, network_AbortCallback );
}

void Network::SessionLost()
{
	if(g_slicEngine && m_readyToStart) {
		SlicObject *so = new SlicObject("355SessionLost");
		so->AddRecipient(m_playerIndex);
		g_slicEngine->Execute(so);
	} else {
		g_civApp->PostQuitToLobbyAction();
	}
}

bool Network::ReadyForPackets()
{
	if(g_gevManager->EventsPending())
		return false;
	else
		return true;
}

void Network::SetReady(uint16 id)
{
	sint32 i,n;

	sint32 index = IdToIndex(id);
	if(index < 0)
		return;

	Assert(m_playerData[index]);
	if(!m_playerData[index])
		return;

	PlayerData *player = m_playerData[index];
	m_playerData[index]->m_ready = TRUE;

	MapPoint* size = g_theWorld->GetSize();

	QueuePacket(player->m_id, new NetCRC(0, 10));
	QueuePacket(player->m_id, new NetCRC(11, 20));

	QueuePacket(player->m_id, new NetGameSettings(size->x, size->y,
												  g_theProfileDB->GetNPlayers(),
												  m_gameStyle,
												  m_unitMovesPerSlice,
												  m_totalStartTime,
												  m_turnStartTime,
												  m_extraTimePerCity));

	
	NetInfo* netInfo = new NetInfo(NET_INFO_CODE_PLAYER_INDEX, 
								   index, player->m_id);
	QueuePacket(player->m_id, netInfo);
		
	SetupPlayerFromNSPlayerInfo(player->m_id, index);

	
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(!g_player[i]) continue;
		if(m_playerData[i] && i != index) {
			NetInfo* netInfo2 = new NetInfo(NET_INFO_CODE_PLAYER_INDEX,
											i, m_playerData[i]->m_id);
			QueuePacket(player->m_id, netInfo2);
		}
	}

	if(player->m_id == m_pid) {
		m_playerIndex = index;

		if(g_director) {
			g_director->NextPlayer();
		}
		if(g_tiledMap) {
			g_tiledMap->NextPlayer();
            g_tiledMap->CopyVision();
            g_tiledMap->InvalidateMix();
			g_tiledMap->InvalidateMap();
			g_tiledMap->Refresh();
		}
		if(g_radarMap) {
			g_radarMap->Update();
		}

		MainControlPanel::UpdateCityList();
		
		return;
	}

#define PROGRESS(x) { QueuePacket(player->m_id, new NetInfo(NET_INFO_CODE_PROGRESS, x)); }
#define CPROGRESS(x) { chunkPackets.AddTail(new NetInfo(NET_INFO_CODE_PROGRESS, x)); }


#define k_CELL_LIST_CELL_SIZE 6

	PROGRESS(0);
	
	uint16 cells = 0;
	NetCellList* cellList = NULL;

	double percentMap = 0;

	PointerList<Packetizer> chunkPackets;

	sint32 x, y;
	for(x = 0; x < size->x; x++) {
		for(y = 0; y < size->y; y++) {
			if(!cellList) {
				cellList = new NetCellList(x,y);
			}
			cellList->m_cells++;
			


			if(cellList->m_cells * k_CELL_LIST_CELL_SIZE >= 220) {
				chunkPackets.AddTail(cellList);
				
				cellList = NULL;
			}
		}
		if(x < size->x - 1) {
			percentMap = double(x) / double(size->x);
			CPROGRESS(sint32(double(50) * percentMap));
		}
	}
	if(cellList && cellList->m_cells > 0) {
		chunkPackets.AddTail(cellList);
		
	}

	ChunkList(player->m_id, &chunkPackets);
	Assert(!chunkPackets.GetHead());

	QueuePacket(player->m_id, new NetInfo(NET_INFO_CODE_MAP_DONE, 0));
	PROGRESS(50);

	
	for(sint32 p = 0; p < k_MAX_PLAYERS; p++) {
		if(!g_player[p]) continue;
		chunkPackets.AddTail(new NetPlayer(g_player[p]));
		chunkPackets.AddTail(new NetResearch(g_player[p]->m_advances));
		chunkPackets.AddTail(
					new NetDifficulty(g_player[p]->GetDifficulty()));
		Assert(g_theCivilisationPool->IsValid(*g_player[p]->m_civilisation));
		if(g_theCivilisationPool->IsValid(*g_player[p]->m_civilisation)) {
			chunkPackets.AddTail(
						new NetCivilization(g_player[p]->m_civilisation->AccessData()));
		}
		sint32 r;
		sint32 n = g_player[p]->m_strengths->m_strengthRecords[0].Num();
		for(r = 0; r < n; r += 100) {			
			chunkPackets.AddTail(new NetFullStrengths(p, r, ((r + 99) < n) ? (r+99) : (n - 1)));
		}
	}

	ChunkList(player->m_id, &chunkPackets);
	Assert(!chunkPackets.GetHead());

	PROGRESS(55);
	QueuePacket(player->m_id, new NetInfo(NET_INFO_CODE_START_UNITS, 0));
	

	sint32 numPlayers = 0;
	for(p = 0; p < k_MAX_PLAYERS; p++) {
		if(g_player[p]) {
			numPlayers++;
		}
	}
	double percentPerPlayer = 1.0 / double(numPlayers);
	double playerPercent = 0;

	for(p = 0; p < k_MAX_PLAYERS; p++) {
		if(!g_player[p]) continue;
		
		chunkPackets.AddTail( new NetSetPlayerGuid(p));

		
		UnitDynamicArray *unitList = g_player[p]->GetAllCitiesList();
		for(n = 0; n < unitList->Num(); n++) {
			UnitData* unitData;
			unitData = g_theUnitPool->GetUnit(unitList->Get(n).m_id);

			
			chunkPackets.AddTail( new NetUnit(unitData));

			
			chunkPackets.AddTail( new NetCity(unitData, TRUE));
			chunkPackets.AddTail( new NetCityName(unitData->GetCityData()));
			chunkPackets.AddTail( new NetCity2(unitData->GetCityData(), TRUE));
			chunkPackets.AddTail( new NetCityBuildQueue(unitData->GetCityData()));
			chunkPackets.AddTail( 
						new NetHappy(unitList->Get(n), 
									 unitData->GetCityData()->GetHappy(),
									 TRUE));

			
			
			

		}

		
		unitList = g_player[p]->GetAllUnitList();
		for(n = 0; n < unitList->Num(); n++) {
			chunkPackets.AddTail( new NetUnit(g_theUnitPool->GetUnit(unitList->Get(n).m_id)));
		}

		
		for(n = 0; n < g_player[p]->m_all_armies->Num(); n++) {
			Army army = g_player[p]->m_all_armies->Access(n);
			chunkPackets.AddTail( 
						new NetArmy(g_theArmyPool->AccessArmy(army)));

			chunkPackets.AddTail( new NetInfo(NET_INFO_CODE_ADD_ARMY,
												  p,
												  CAUSE_NEW_ARMY_INITIAL,
												  g_player[p]->m_all_armies->Access(n)));
			
			sint32 m;
			for(m = 0; m < army.NumOrders(); m++) {
				const Order *order = army.GetOrder(m);
				Assert(order);
				if(order) {
					chunkPackets.AddTail( new NetOrder(p,
														   army,
														   order->m_order,
														   order->m_path,
														   order->m_point,
														   order->m_argument,
														   order->m_eventType));
				}
			}
		}


		
		UnitDynamicArray* traderList = g_player[p]->GetTradersList();
		for(n = 0; n < traderList->Num(); n++) {
			UnitData* unitData;
			unitData = g_theUnitPool->GetUnit(traderList->Get(n).m_id);
			chunkPackets.AddTail( new NetUnit(unitData));
		}

		
		n = g_player[p]->m_terrainImprovements->Num();
		for(i = 0; i < n; i++) {
			chunkPackets.AddTail( new NetTerrainImprovement(g_player[p]->m_terrainImprovements->Access(i).AccessData()));
		}

		
		n = g_player[p]->m_allInstallations->Num();
		for(i = 0; i < n; i++) {
			chunkPackets.AddTail( new NetInstallation(g_player[p]->m_allInstallations->Access(i).AccessData()));
		}

		
		chunkPackets.AddTail( new NetInfo(NET_INFO_CODE_GOLD,
											  p, g_player[p]->m_gold->GetLevel()));
		
		chunkPackets.AddTail( new NetReadiness(g_player[p]->m_readiness));

		
		chunkPackets.AddTail( new NetPlayerHappy((uint8)p, g_player[p]->m_global_happiness, TRUE));
		
		
		chunkPackets.AddTail( new NetCivilization(g_player[p]->m_civilisation->AccessData()));

		
		

		
		sint32 y;
		for(y = 0; y < g_theWorld->GetYHeight(); y += k_VISION_STEP) {
			chunkPackets.AddTail( new NetVision(p, y, k_VISION_STEP));
		}
		static DynamicArray<UnseenCellCarton> array;
		g_player[p]->m_vision->GetUnseenCellList(array);
		n = array.Num();
		for(i = 0; i < n; i++) {
			chunkPackets.AddTail( new NetUnseenCell(array[i].m_unseenCell,
														p));
		}

		chunkPackets.AddTail( new NetEndGame(p));

		playerPercent += percentPerPlayer;
		CPROGRESS(55 + (playerPercent * 30));
		ChunkList(player->m_id, &chunkPackets);
		Assert(!chunkPackets.GetHead());
	}

	PROGRESS(85);

	chunkPackets.AddTail( new NetWormhole());

	chunkPackets.AddTail( new NetPollution());

	chunkPackets.AddTail( new NetWonderTracker());
	chunkPackets.AddTail( new NetAchievementTracker());
	chunkPackets.AddTail( new NetFeatTracker());
	chunkPackets.AddTail( new NetExclusions());

	chunkPackets.AddTail( new NetWorld());
	n = g_theTradePool->m_all_routes->Num();
	for(i = 0; i < n; i++) {
		chunkPackets.AddTail( new NetTradeRoute(g_theTradePool->m_all_routes->Access(i).AccessData(), false));
	}

	PROGRESS(90);

	
	for(x = 0; x < g_theWorld->GetXWidth(); x++) {
		for(y = 0; y < g_theWorld->GetYHeight(); y++) {
			if(g_theWorld->GetCell(x, y)->GetNumUnits() >= 2) {
				chunkPackets.AddTail( new NetCellUnitOrder(x, y));
			}
		}
	}

	
	chunkPackets.AddTail( new NetInfo(NET_INFO_CODE_END_UNITS, 
										  g_theUnitPool->HackGetKey(),
										  g_theArmyPool->HackGetKey()));
	
	PROGRESS(95);

	chunkPackets.AddTail( new NetAgreementMatrix);

	chunkPackets.AddTail( new NetRand());

	chunkPackets.AddTail( new NetKeys());
	chunkPackets.AddTail( new NetInfo(NET_INFO_CODE_YEAR,
										  g_turn->GetRound(),
										  g_turn->GetYear()));

	ChunkList(player->m_id, &chunkPackets);
	Assert(!chunkPackets.GetHead());

	if(m_setupMode) {
		sint32 index = IdToIndex(player->m_id);
		MapPoint center = g_player[index]->m_setupCenter;
		QueuePacket(player->m_id, new NetInfo(NET_INFO_CODE_SET_SETUP_MODE,
											  m_setupMode));
		QueuePacket(player->m_id, new NetInfo(NET_INFO_CODE_SET_SETUP_AREA,
											  index,
											  center.x, center.y,
											  g_player[index]->m_setupRadius));
		QueuePacket(player->m_id, new NetInfo(NET_INFO_CODE_POWER_POINTS,
											  index,
											  g_player[index]->m_powerPoints));
	}

	PROGRESS(100);

	SendJoinedMessage(player->m_name, index);
	QueuePacket(player->m_id, new NetInfoMessage(NET_MSG_PLAYER_JOINED,
												 m_playerData[m_playerIndex]->m_name,
												 m_playerIndex));

	
	QueuePacket(player->m_id, new NetInfo(NET_INFO_CODE_SET_TURN,
										  g_selected_item->GetCurPlayer()));

	if(index == g_selected_item->GetCurPlayer()) {
		player->m_ackBeginTurn = TRUE;
	}

	if(m_readyToStart) {
		QueuePacket(player->m_id, new NetInfo(NET_INFO_CODE_ALL_PLAYERS_READY));
	}
}

void Network::SyncRand()
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetRand());
	}
}

void Network::SyncRand(sint32 index)
{
	if(m_iAmHost) {
		QueuePacket(IndexToId(index), new NetRand());
	}
}


void 
Network::Enqueue(UnitData* unit) 
{
	if(m_iAmHost) {
		NetUnit* netUnit = new NetUnit(unit);
		QueuePacketToAll(netUnit);
	}
}

void Network::Enqueue(UnitData *unit, Unit useActor)
{
	if(m_iAmHost) {
		NetUnit *netUnit = new NetUnit(unit, useActor);
		QueuePacketToAll(netUnit);
	}
}

void
Network::MoveUnit(UnitData *data, const MapPoint &pnt)
{
	if(m_iAmHost) {
		Block(data->GetOwner());
		QueuePacketToAll(new NetUnitMove(Unit(data->m_id), pnt));
		Unblock(data->GetOwner());
	}
}


void
Network::Enqueue(UnitData* unit, CityData* city, BOOL isInitial)
{
	if(m_iAmHost) {
		NetCity* netCity = new NetCity(unit, isInitial);
		QueuePacketToAll(netCity);

		NetCity2* netCity2 = new NetCity2(city, isInitial);
		QueuePacketToAll(netCity2);

		Unit u(unit->m_id);
		QueuePacketToAll(new NetHappy(u, city->GetHappy(), isInitial));

		Block(city->GetOwner());
		QueuePacketToAll(new NetCityBuildQueue(city));
		Unblock(city->GetOwner());
	}
}

void Network::SendCityName(CityData *city)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetCityName(city));
	} else {
		QueuePacket(m_hostId, new NetCityName(city));
	}
}


void
Network::AddNewUnit(sint32 owner, Unit u)
{
	if(m_playerData[owner] && g_player[owner]->GetPlayerType() == PLAYER_TYPE_NETWORK) {
		m_playerData[owner]->m_createdUnits.Insert(u);
	}
}


void 
Network::Enqueue(Cell* cell, 
				 sint32 x, sint32 y) 
{
	if(m_iAmHost) {
		
		
		NetCellData* cellData = new NetCellData(cell, x, y);
		QueuePacketToAll(cellData);
	}
}

void
Network::Enqueue(TradeRouteData* tradeRoute)
{
	if(m_iAmHost) {
		NetTradeRoute* netTradeRoute = new NetTradeRoute(tradeRoute, true);
		QueuePacketToAll(netTradeRoute);
	}
}

void
Network::Enqueue(TradeOfferData* offer)
{
	if(m_iAmHost) {
		NetTradeOffer* netTradeOffer = new NetTradeOffer(offer);
		QueuePacketToAll(netTradeOffer);
	}
}


void
Network::Enqueue(NetInfo* netInfo) 
{
	QueuePacketToAll(netInfo);
}

void
Network::Enqueue(TerrainImprovementData *data)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetTerrainImprovement(data));
	}
}

void
Network::Enqueue(InstallationData *data)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetInstallation(data));
	}
}

void
Network::Enqueue(Gold *gold)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetInfo(NET_INFO_CODE_GOLD,
									 gold->GetOwner(), gold->GetLevel()));
	}
}

void 
Network::Enqueue(MilitaryReadiness *readiness)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetReadiness(readiness));
	}
}

void
Network::Enqueue(uint8 owner, PlayerHappiness *hap)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetPlayerHappy(owner, hap, FALSE));
	}
}

void
Network::Enqueue(AgreementData *data)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetAgreement(data));
	}
}

void
Network::MakeAgreement(Agreement &a)
{
	if(m_iAmClient) {
		QueuePacket(m_hostId, new NetClientAgreement(a.AccessData()));
	}
}

void
Network::Enqueue(CivilisationData *data)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetCivilization(data));
	}
}

void
Network::Enqueue(DiplomaticRequestData *data)
{
}

void Network::SendDiplomaticRequest(DiplomaticRequestData *data)
{
}

void
Network::Enqueue(MessageData *data)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetMessage(data));
	}
}

void
Network::EnqueuePollution()
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetPollution());
	}
}

void
Network::Enqueue(NetOrder *order)
{
	PacketManager	l_AutoRelease(order);

	if(m_iAmHost) {
		QueuePacketToAll(order);
	}
}

void
Network::Enqueue(ArmyData *armyData)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetArmy(armyData));
	}
}

void
Network::AddNewArmy(sint32 owner, const Army &army)
{
	if(m_playerData[owner] && g_player[owner]->GetPlayerType() == PLAYER_TYPE_NETWORK) {
		m_playerData[owner]->m_createdArmies.Insert(army);
	}
}

void
Network::Enqueue(CityData *cd)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetCityBuildQueue(cd));
	}
}

void
Network::SendBuildQueue(CityData *cd)
{
	if(m_hostId == 0) {
		m_netIO->GetHostId(m_hostId);
	}

	QueuePacket(m_hostId, new NetCityBuildQueue(cd));
}

void
Network::SendMessage(MessageData *data)
{
	if(m_hostId == 0) {
		m_netIO->GetHostId(m_hostId);
	}
	QueuePacket(m_hostId, new NetMessage(data));
}

#ifdef _PLAYTEST
void
Network::SendCheat(NetCheat *netCheat)
{
	if(m_hostId == 0) {
		m_netIO->GetHostId(m_hostId);
	}
	QueuePacket(m_hostId, netCheat);
}
#endif


void
Network::SendAction(NetAction* netAction) 
{
	if(m_hostId == 0) {
		m_netIO->GetHostId(m_hostId);
	}

	QueuePacket(m_hostId, netAction);
}

void
Network::SendActionBookmark(NetAction* netAction) 
{
	if(m_hostId == 0) {
		m_netIO->GetHostId(m_hostId);
	}

	QueuePacketBookmark(m_hostId, netAction);
}

void
Network::SendOrder(sint32 owner, const Army &army, UNIT_ORDER_TYPE o,
				   Path *a_path, const MapPoint &point, sint32 arg,
				   GAME_EVENT event)
{
	if(m_hostId == 0) {
		m_netIO->GetHostId(m_hostId);
	}

	QueuePacket(m_hostId, new NetOrder(owner, army, 
									   o, a_path, point,
									   arg, event));
}

void Network::SendToServer(Packetizer *packet)
{
	if(m_hostId == 0) {
		m_netIO->GetHostId(m_hostId);
	}
	QueuePacket(m_hostId, packet);
}


void
Network::QueuePacket(uint16 id, 
					 Packetizer* packet) 
{
	PacketManager	l_AutoRelease(packet);

	if(m_iAmClient && m_waitingOnResync)
		return;

	sint32 index = IdToIndex(id);
	Assert(m_playerData[index]);
	if(!m_playerData[index])
		return;

	
	
	packet->m_destination = (sint32)id;

	if(m_playerData[index]->m_blocked > 0)
		return;

	
	
	if(packet->m_unitId == 0 || 
	   !m_playerData[index]->m_unitHash.IsPresent(packet->m_unitId)) {
		
		m_playerData[index]->m_packetList->AddTail(packet);
		packet->PacketizeAndSave();

		
		if(packet->m_unitId != 0) {
			m_playerData[index]->m_unitHash.Add(packet->m_unitId);
		}
		packet->AddRef();
	}
	ProcessSends(); 
}


void
Network::QueuePacketBookmark(uint16 id, 
							 Packetizer* packet) 
{
	PacketManager	l_AutoRelease(packet);
	sint32 index = IdToIndex(id);
	Assert(m_playerData[index]);

	
	
	
	
	if(m_playerData[index]->m_blocked > 0)
		return;

	if(m_playerData[index]->m_frozen <= 0) {
		BOOL NetworkProgrammerSmokingCrackPleaseIgnore = FALSE;
		Assert(NetworkProgrammerSmokingCrackPleaseIgnore);
		QueuePacket(id, packet);
		return;
	}

	
	
	packet->m_destination = (sint32)id;

	
	
	if(packet->m_unitId == 0 || 
	   !m_playerData[index]->m_unitHash.IsPresent(packet->m_unitId)) {
		
		m_playerData[index]->m_packetList->InsertAt(m_playerData[index]->m_bookmarks->GetTail(), packet);
		packet->PacketizeAndSave();

		
		if(packet->m_unitId != 0) {
			m_playerData[index]->m_unitHash.Add(packet->m_unitId);
		}
		packet->AddRef();
	}
}

void
Network::QueuePacketToAll(Packetizer* packet)
{
	PacketManager	l_AutoRelease(packet);

	if(!g_player) {
		
		return;
	}
	
	for(sint32 pl = 0; pl < k_MAX_PLAYERS; pl++) {
		if(!g_player[pl]) continue;
		
		if(m_playerData[pl] && 
		   g_player[pl]->GetPlayerType() == PLAYER_TYPE_NETWORK) {

			
			
			

			if(m_playerData[pl]->m_blocked > 0)
				continue;

			
			
			if(packet->m_unitId == 0 || 
			   !m_playerData[pl]->m_unitHash.IsPresent(packet->m_unitId)) {

				
				m_playerData[pl]->m_packetList->AddTail(packet);
				packet->PacketizeAndSave();
				if(packet->m_unitId != 0) {
					
					m_playerData[pl]->m_unitHash.Add(packet->m_unitId);
				}
				packet->AddRef();
			}
		}
	}
	ProcessSends(); 

}

void Network::Freeze(uint16 id)
{ 
	if(id == 0xffff) {
		Assert(IsClient());
		id = m_hostId;
	}
	m_playerData[IdToIndex(id)]->m_frozen++;
}

void Network::Unfreeze(uint16 id)
{ 
	if(id == 0xffff) {
		Assert(IsClient());
		id = m_hostId;
	}
	DPRINTF(k_DBG_NET, ("Unbookmarking %d\n", IdToIndex(id)));
	m_playerData[IdToIndex(id)]->m_frozen--;
	m_playerData[IdToIndex(id)]->m_bookmarks->RemoveTail();
	Assert(m_playerData[IdToIndex(id)]->m_frozen >= 0);
}

void Network::Bookmark(uint16 id)
{
	if(id == 0xffff) {
		Assert(IsClient());
		id = m_hostId;
	}
	DPRINTF(k_DBG_NET, ("Bookmarking %d\n", IdToIndex(id)));

	m_playerData[IdToIndex(id)]->m_frozen++;
	m_playerData[IdToIndex(id)]->m_bookmarks->AddTail(
		m_playerData[IdToIndex(id)]->m_packetList->GetTailNode());
}

void Network::Block(sint32 index)
{
	
	if(m_playerData[index])
		m_playerData[index]->m_blocked++;
}

void Network::Unblock(sint32 index)
{
	
	if(m_playerData[index]) {
		m_playerData[index]->m_blocked--;
		Assert(m_playerData[index]->m_blocked >= 0);
	}
}



sint32
Network::IdToIndex(uint16 id)
{
	for(sint32 i = 0 ; i < k_MAX_PLAYERS; i++) {
		if(m_playerData[i] && m_playerData[i]->m_id == id) {
			return m_playerData[i]->m_index;
		}
	}

#ifdef _DEBUG
	PointerList<PlayerData>::Walker walk(m_newPlayerList);
	while(walk.IsValid()) {
		if(walk.GetObj()->m_id == id) {
			return -1;
		}
		walk.Next();
	}
	Assert(FALSE);
#endif
	return -1;
}



uint16
Network::IndexToId(sint32 index)
{
	for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
		if(m_playerData[i] && m_playerData[i]->m_index == index) {
			return m_playerData[i]->m_id;
		}
	}
	Assert(FALSE);
	return 0xffff;
}

void Network::SetPlayerIndex(sint32 index, uint16 id)
{ 
	BOOL sendMessage = FALSE;
	if(id == m_pid) {
		m_playerIndex = index; 
	} else {
		sendMessage = TRUE;
	}
	m_totalTimeUsed = 0;
	
	PointerList<PlayerData>::Walker walk(m_newPlayerList);
	while(walk.IsValid()) {
		PlayerData* pd = (PlayerData*)walk.GetObj();
		if(pd->m_id == id) {
			pd->m_index = index;
			m_playerData[index] = pd;
			m_playerData[index]->m_ready = TRUE;
			walk.Remove();
			break;
		}
		walk.Next();
	}

	if(!m_playerData[index]) {
		AddPlayer(id, "anotherclient");
		m_playerData[index] = m_newPlayerList->RemoveTail();
		m_playerData[index]->m_ready = TRUE;
	}
	m_playerData[index]->m_id = id;
	
	
	
	
}







BOOL
Network::DeadUnit(sint32 unitId)
{
	return m_deadUnitList->IsPresent(unitId);
}

void
Network::AddDeadUnit(sint32 unitId)
{
	Assert(!m_deadUnitList->IsPresent(unitId));
	if(!m_deadUnitList->IsPresent(unitId)) {
		m_deadUnitList->Add(unitId);
	}
}

void
Network::RemoveDeadUnit(sint32 unitId)
{
	m_deadUnitList->Remove(unitId);
}

void
Network::ClearDeadUnits()
{
	m_deadUnitList->Clear();
}

void Network::AddCreatedObject(GAMEOBJ *obj)
{
	m_gameObjects->AddCreated(obj);
}

void Network::HandleObjectACK(uint32 id)
{
	m_gameObjects->ACKObject(id);
}

void Network::HandleObjectNAK(uint32 myId, uint32 realId)
{
	m_gameObjects->NAKObject(myId, realId);
}

void Network::CheckReceivedObject(uint32 id)
{
	m_gameObjects->CheckReceived(id);
}

sint32 Network::FindEmptySlot(PlayerData *player, uint16 id)
{
	sint32 newslot = -1;
	
	NSPlayerInfo *nspi = GetNSPlayerInfoByID(id);
	
	sint32 p;
	for(p = 1; p < k_MAX_PLAYERS; p++) {
		if(g_player[p] && g_player[p]->m_networkId == id &&
		   !m_playerData[p]) {
			return p;
		}

		if(nspi && g_player[p] && 
		   g_player[p]->m_civilisation->GetCivilisation() == nspi->m_civ &&
		   !m_playerData[p]) {
			return p;
		}
	}
			
	GUID zeroGuid;
	memset((uint8 *)&zeroGuid, 0, sizeof(zeroGuid));

	for(p = 1; p < k_MAX_PLAYERS; p++) {
		if(!g_player[p]) continue;
		
		if(!m_playerData[p]) {
			if(newslot < 0 || g_player[p]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
					newslot = p;
			}
			
			
			
			
			if(g_theProfileDB->NoHumanPlayersOnHost() && player->m_id == m_pid &&
			   newslot >= 0 && g_player[newslot]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
				break;
			}
			if(g_player[newslot] &&
				(g_player[newslot]->GetPlayerType() == PLAYER_TYPE_HUMAN ||
				(g_player[newslot]->m_openForNetwork &&
				 memcmp(&g_player[newslot]->m_networkGuid, &zeroGuid, sizeof(GUID)) == 0))) {
				
				
				
				
				
				break;
			}
		}
	}
	if(!g_player[newslot])
		return -1;

	return newslot;
}

sint32 Network::FindOldSlot(PlayerData *player, uint16 id)
{
	sint32 p;
	
	GUID zeroGuid;
	memset((uint8 *)&zeroGuid, 0, sizeof(zeroGuid));

	for(p = 1; p < k_MAX_PLAYERS; p++) {
		if(!g_player[p])
			continue;
		if(!g_player[p]->m_openForNetwork)
			continue;
		if(memcmp(&g_player[p]->m_networkGuid, &zeroGuid, sizeof(GUID)) == 0)
			continue;

		if(memcmp(&g_player[p]->m_networkGuid, &player->m_guid, sizeof(GUID)) == 0) {
			if(m_playerData[p]) {
				
				return -1;
			}
			return p;
		}
	}
	return -1;
}

void
Network::ProcessNewPlayer(uint16 id)
{
	m_processingNewPlayers = TRUE;
	sint32 newslot = -1;
	BOOL found = FALSE;

	PointerList<PlayerData>::Walker walk(m_newPlayerList);

	
	while(walk.IsValid() && !found) {
		PlayerData* player = walk.GetObj();
		if(player->m_id != id) {
			walk.Next();
			continue;
		}
		NSPlayerInfo *nspi = GetNSPlayerInfoByID(player->m_id);
		if(nspi)
			player->m_group = nspi->m_group;

		GUID zeroGuid;
		memset((uint8 *)&zeroGuid, 0, sizeof(zeroGuid));

		if(id == m_pid) {
			player->m_guid = m_guid;
		}

		newslot = FindOldSlot(player, id);
		if(newslot < 0) {
			newslot = FindEmptySlot(player, id);
		}

		Assert(newslot >= 0);
		if(newslot < 0) {
			m_netIO->KickPlayer(id);
			return;
		}

		
		if(memcmp(&g_player[newslot]->m_networkGuid, &zeroGuid, sizeof(GUID)) &&
		   memcmp(&g_player[newslot]->m_networkGuid, &player->m_guid, sizeof(GUID))) {
			
			
			SendWrongPlayerJoinedMessage(player->m_name, newslot);			
		} else {
			g_player[newslot]->m_networkGuid = player->m_guid;
			g_player[newslot]->m_networkGroup = player->m_group;
			ClosePlayer(newslot);
		}
		found = TRUE;
		
		
		
		m_playerData[newslot] = player;
		
		if(m_iAmHost) {
			SetMaxPlayers(CountOpenSlots() + CountTakenSlots());
		}

		
		
		Assert(g_player[newslot]->GetPlayerType() != PLAYER_TYPE_NETWORK);
		
		
		
		sint32 oldVisPlayer = g_selected_item->GetVisiblePlayer();
		if(player->m_id != m_pid) {
			g_player[newslot]->SetPlayerType(PLAYER_TYPE_NETWORK);
		} else {
			m_playerIndex = newslot;
			g_player[m_playerIndex]->m_networkId = m_pid;

			if(newslot == g_selected_item->GetCurPlayer()) {
				SetMyTurn(TRUE);
			}
			if(g_director) {
				g_director->NextPlayer();
			}
			if(g_tiledMap) {
                g_tiledMap->NextPlayer();
                g_tiledMap->CopyVision();
				g_tiledMap->InvalidateMix();
				g_tiledMap->InvalidateMap();
				g_tiledMap->Refresh();
			}
			if(g_radarMap) {
				g_radarMap->Update();
			}
		}
		
		player->m_index = newslot;
		
		
		NetInfo* netInfo = new NetInfo(NET_INFO_CODE_PLAYER_INDEX, 
									   newslot, player->m_id);
		QueuePacketToAll(netInfo);
		QueuePacketToAll(new NetSetPlayerGuid(newslot));
		if(player->m_name) {
			g_player[newslot]->m_civilisation->AccessData()->SetLeaderName(player->m_name);
			QueuePacketToAll(new NetSetLeaderName(newslot));
			if(g_networkPlayersScreen) {
				g_networkPlayersScreen->UpdateData();
			}
		}
		if(player->m_id == m_pid) {
			SetupPlayerFromNSPlayerInfo(m_pid, m_playerIndex);
		}
		walk.Remove();
		
		SetReady(id);
		if(!m_readyToStart) {
			if(m_newPlayerList->IsEmpty()) {
				ResetTurnEndsAt();
				
				c3_RemoveAbortMessage();
				SetReadyToStart(TRUE);
			}
		}
	}
	m_processingNewPlayers = FALSE;
	Assert(found && newslot >= 0);

	if(g_networkPlayersScreen) {
		g_networkPlayersScreen->UpdateData();
	}
}

extern sint32 g_debugOwner;
void Network::AddChatText(MBCHAR *str, sint32 len, uint8 from, BOOL priv)
{
	
	

	if(!priv)
		sprintf(m_chatStr, "[%s] ", 
				((from == 0) ? g_theStringDB->GetNameStr("NETWORK_SENDER_SYSTEM") : 
				 (g_player[from] ? (g_player[from]->m_civilisation->GetLeaderName()) : ".")));
	else
		sprintf(m_chatStr, "[P] (%s) ", ((from == 0) ? 
										 (g_theStringDB->GetNameStr("NETWORK_SENDER_SYSTEM")) : 
										 (g_player[from] ? 
										  (g_player[from]->m_civilisation->GetLeaderName()) :
										  ("."))));

	strcat(m_chatStr, str);
	
	if (g_chatBox) {
		MBCHAR *c;
		
		for(c = m_chatStr; *c; c++) {
			if(*c == '<')
				*c = '(';
			else if(*c == '>')
				*c = ')';
		}

		g_chatBox->AddLine(from, m_chatStr);
	}

	m_chatList->AddLine((sint32)from, m_chatStr);
}

void Network::SendChatText(MBCHAR *str, sint32 len)
{
	if(str[0] == '/') {
		if(stricmp(str, "/rules") == 0) {
			char buf[1024];
			sprintf(buf, "Difficulty: %d", g_theGameSettings->GetDifficulty());
			g_chatBox->AddLine(m_playerIndex, buf);

			sprintf(buf, "Risk: %d", g_theGameSettings->GetRisk());
			g_chatBox->AddLine(m_playerIndex, buf);

			sprintf(buf, "Pollution: %s", g_theGameSettings->GetPollution() ? "On" : "Off");			
			g_chatBox->AddLine(m_playerIndex, buf);
			
			sprintf(buf, "Bloodlust: %s", g_theGameSettings->GetAlienEndGame() ? "Off" : "On");
			g_chatBox->AddLine(m_playerIndex, buf);

			if(g_theGameSettings->GetStartingAge() > 0 || g_theGameSettings->GetEndingAge() < g_theAgeDB->NumRecords()) {
				sprintf(buf, "Starting Age: %d", g_theGameSettings->GetStartingAge());
				g_chatBox->AddLine(m_playerIndex, buf);

				sprintf(buf, "Ending Age: %d", g_theGameSettings->GetEndingAge());
				g_chatBox->AddLine(m_playerIndex, buf);
			}

			if(TeamsEnabled()) {
				sprintf(buf, "Teammates: ");
				sint32 i;
				for(i = 1; i < k_MAX_PLAYERS; i++) {
					if(i == m_playerIndex)
						continue;
					if(!g_player[i])
						continue;
					if(g_player[i]->m_networkGroup == g_player[m_playerIndex]->m_networkGroup) {
						char civname[1024];
						g_player[i]->m_civilisation->GetSingularCivName(civname);
						sprintf(buf + strlen(buf), "%s(%s)  ", 
								g_player[i]->m_civilisation->GetLeaderName(),
								civname);
					}
				}
				g_chatBox->AddLine(m_playerIndex, buf);
			}
		} else if(strnicmp(str, "/msg", 4) == 0) {
			char destination[256];
			sint32 dest = -1;
			sint32 d = 0;
			char *c = &str[4];
			while(*c && isspace(*c))
				c++;

			if(*c) {
				while(!isspace(*c) && *c) {
					destination[d] = *c;
					c++; d++;
				}
				destination[d] = 0;
				if(*c) {
					if(!isdigit(destination[0])) {
						sint32 p;
						for(p = 0; p < k_MAX_PLAYERS; p++) {
							if(!g_player[p])
								continue;
							
							char name[256];
							char *n, *ln;
							
							for(n = &name[0], ln = g_player[p]->m_civilisation->GetLeaderName(); 
							*ln && !isspace(*ln); ln++, n++)
								*n = *ln;					   						
							*n = 0;
							
							if(g_player[p] && 
								stricmp(destination, name) == 0) {
								dest = p;
								break;
							}
						}
					} else {						
						dest = atoi(destination);
					}
					if(dest > 0 && dest < k_MAX_PLAYERS && g_player[dest]) {
						NetChat *chatPacket = new NetChat(1 << dest, c, (sint16)len - (c - str));
						if(g_network.IsHost()) {
							QueuePacket(IndexToId(dest), chatPacket);
						} else {
							QueuePacket(m_hostId, chatPacket);
						}
					}
				}
			}
		}

		
		return;
	}

	
	
	AddChatText(str, len, g_selected_item->GetVisiblePlayer(), FALSE);

	NetChat *chatPacket = new NetChat(m_chatMask, str, (sint16)len);
	chatPacket->AddRef();
    if (IsActive()) {
#if 0
		MBCHAR tempStr[_MAX_PATH];
		memcpy(tempStr, str, len);
		tempStr[len] = 0;
		if (g_chatBox) {
			g_chatBox->AddLine(GetPlayerIndex(), tempStr);
		}
#endif

		if(g_network.IsHost()) {
			for(sint32 p = 0; p < k_MAX_PLAYERS; p++) {
				if(!g_player[p]) continue;
				if(m_chatMask & (1 << p) && g_player[p]->m_playerType != PLAYER_TYPE_ROBOT &&
				   m_playerData[p]) {
					QueuePacket(IndexToId(p), chatPacket);
				}
			}
		} else {
			QueuePacket(m_hostId, chatPacket);
		}
    }
	chatPacket->Release();
}

void Network::AddCivilization(sint32 index, PLAYER_TYPE pt, sint32 civ)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetInfo(NET_INFO_CODE_NEW_CIVILIZATION,
									 index, pt, civ));
	} else {
		QueuePacket(m_hostId, new NetAction(NET_ACTION_CREATED_CIV,
											index, pt, civ));
	}
}

void Network::KillPlayer(sint32 p, GAME_OVER reason, sint32 data)
{
	if(m_iAmHost) {
		QueuePacketToAll(new NetInfo(NET_INFO_CODE_KILL_PLAYER, p,
									 reason, data));
		ClosePlayer(p);
	} else {
		QueuePacket(m_hostId, new NetAction(NET_ACTION_KILLED_PLAYER, p));
	}
}

void Network::GetSliceFor(sint32 player)
{
	if(g_turn->SimultaneousMode() && IsHost()) {
		if(g_selected_item->GetCurPlayer() != player) {
			if(g_player[g_selected_item->GetCurPlayer()]->GetPlayerType() != PLAYER_TYPE_NETWORK) {
				g_turn->SetSliceTo(player);
			} else {
				g_turn->QueueSliceFor(player);
			}
		}
	}
}

#ifdef _DEBUG
#define k_LEFT_EDGE 200
#define k_TOP_EDGE 80
#define k_TEXT_SPACING 16

void Network::DisplayChat(aui_Surface *surf)
{
	if(m_displayPackets) {
		MBCHAR buf[256];
		uint32 totalCount=0, totalBytes=0;
		uint32 totalSent = 0, totalSentBytes = 0;

		for(sint32 i = 0; i < k_NUM_PACKET_TYPES; i++) {
			sprintf(buf, "%c%c : Rx: %d/%d   Tx: %d/%d",
					m_packetName[i][0], m_packetName[i][1],
					m_packetCounter[i], m_packetBytes[i],
					m_sentPacketCounter[i], m_sentPacketBytes[i]);
			primitives_DrawText((aui_DirectSurface *)surf, k_LEFT_EDGE,
								k_TOP_EDGE + ((i+1) * k_TEXT_SPACING),
								buf, 0, 0);
			totalCount += m_packetCounter[i];
			totalBytes += m_packetBytes[i];
			totalSent += m_sentPacketCounter[i];
			totalSentBytes += m_sentPacketBytes[i];
		}
		sprintf(buf, "Total: Rx: %d/%d, Tx: %d/%d [%d blocked]",
				totalCount, totalBytes, 
				totalSent, totalSentBytes,
				m_blockedPackets);
		primitives_DrawText((aui_DirectSurface *)surf, k_LEFT_EDGE,
							k_TOP_EDGE + ((i+1) * k_TEXT_SPACING),
							buf, 0, 0);
	}

	primitives_DrawText((aui_DirectSurface *)surf, k_LEFT_EDGE, k_TOP_EDGE,
						m_chatStr, 0, 0);
}
#endif

#ifdef _DEBUG
static sint32 GetPacketLogIndex(uint16 id)
{
	sint32 idx;
	switch(id) {
		case k_PACKET_CELL_ID: idx=0; break;
		case k_PACKET_CELL_LIST_ID: idx=1; break;
		case k_PACKET_UNIT_ID: idx=2; break;
		case k_PACKET_ACTION_ID: idx=3; break;
		case k_PACKET_INFO_ID: idx=4; break;
		case k_PACKET_CITY_ID: idx=5; break;
		case k_PACKET_POP_ID: idx=6; break;
		case k_PACKET_DIFFICULTY_ID: idx=7; break;
		case k_PACKET_PLAYER_ID: idx=8; break;
		case k_PACKET_TRADE_ROUTE_ID: idx=9; break;
		case k_PACKET_TRADE_OFFER_ID: idx=10; break;
		case k_PACKET_RAND_ID: idx=11; break;
		case k_PACKET_TERRAIN_ID: idx=12; break;
		case k_PACKET_INSTALLATION_ID: idx=13; break;
		case k_PACKET_CHAT_ID: idx=14; break;
		case k_PACKET_READINESS_ID: idx=15; break;
		case k_PACKET_HAPPY_ID: idx=16; break;
		case k_PACKET_PLAYER_HAPPY_ID: idx=17; break;
		case k_PACKET_REPORT_ID: idx = 18; break;
		case k_PACKET_UNIT_MOVE_ID: idx = 19; break;
		case k_PACKET_UNIT_ORDER_ID: idx = 20; break;
		case k_PACKET_AGREEMENT_ID: idx = 21; break;
		case k_PACKET_CIVILIZATION_ID: idx = 22; break;
		case k_PACKET_CITY_NAME_ID: idx = 23; break;
		case k_PACKET_DIP_PROPOSAL_ID: idx = 24; break;
		case k_PACKET_MESSAGE_ID: idx = 25; break;
		case k_PACKET_CITY2_ID: idx = 26; break;
		case k_PACKET_POLLUTION_ID: idx = 27; break;
		case k_PACKET_CITY_BQ_ID: idx = 28; break;
		case k_PACKET_KEYS_ID: idx = 29; break;
		case k_PACKET_GAME_SETTINGS_ID: idx = 30; break;
		case k_PACKET_NEW_ARMY_ID: idx = 31; break;
		case k_PACKET_REMOVE_ARMY_ID: idx = 32; break;
		case k_PACKET_DIP_RESPONSE_ID: idx = 33; break;
		default: idx = k_NUM_PACKET_TYPES - 1;
	}
	return idx;
}

void Network::LogPacket(uint8 c1, uint8 c2, uint16 size)
{
	sint32 idx;


	idx = GetPacketLogIndex(MAKE_CIV3_ID(c1, c2));
	m_packetCounter[idx]++;
	m_packetBytes[idx] += size;
	

}

void Network::LogSentPacket(uint8 c1, uint8 c2, uint16 size)
{
	sint32 idx;

	idx = GetPacketLogIndex(MAKE_CIV3_ID(c1, c2));
	m_sentPacketCounter[idx]++;
	m_sentPacketBytes[idx]+=size;
}

void Network::InitPacketLog()
{
	m_blockedPackets = 0;
	for(sint32 i = 0; i < k_NUM_PACKET_TYPES; i++) {
		m_packetCounter[i] = 0;
		m_packetBytes[i] = 0;
		m_sentPacketCounter[i] = 0;
		m_sentPacketBytes[i] = 0;

		switch(i) {
			case 0:
				m_packetName[i][0] = k_PACKET_CELL_ID >> 8;
				m_packetName[i][1] = k_PACKET_CELL_ID & 0xff; break;
			case 1:
				m_packetName[i][0] = k_PACKET_CELL_LIST_ID >> 8;
				m_packetName[i][1] = k_PACKET_CELL_LIST_ID & 0xff; break;
			case 2:
				m_packetName[i][0] = k_PACKET_UNIT_ID >> 8;
				m_packetName[i][1] = k_PACKET_UNIT_ID & 0xff; break;
			case 3:
				m_packetName[i][0] = k_PACKET_ACTION_ID >> 8;
				m_packetName[i][1] = k_PACKET_ACTION_ID & 0xff; break;
			case 4:
				m_packetName[i][0] = k_PACKET_INFO_ID >> 8;
				m_packetName[i][1] = k_PACKET_INFO_ID & 0xff; break;
			case 5:
				m_packetName[i][0] = k_PACKET_CITY_ID >> 8;
				m_packetName[i][1] = k_PACKET_CITY_ID & 0xff; break;
			case 6:
				m_packetName[i][0] = k_PACKET_POP_ID >> 8;
				m_packetName[i][1] = k_PACKET_POP_ID & 0xff; break;
			case 7:
				m_packetName[i][0] = k_PACKET_DIFFICULTY_ID >> 8;
				m_packetName[i][1] = k_PACKET_DIFFICULTY_ID & 0xff; break;
			case 8:
				m_packetName[i][0] = k_PACKET_PLAYER_ID >> 8;
				m_packetName[i][1] = k_PACKET_PLAYER_ID & 0xff; break;
			case 9:
				m_packetName[i][0] = k_PACKET_TRADE_ROUTE_ID >> 8;
				m_packetName[i][1] = k_PACKET_TRADE_ROUTE_ID & 0xff; break;
			case 10:
				m_packetName[i][0] = k_PACKET_TRADE_OFFER_ID >> 8;
				m_packetName[i][1] = k_PACKET_TRADE_OFFER_ID & 0xff; break;
			case 11:
				m_packetName[i][0] = k_PACKET_RAND_ID >> 8;
				m_packetName[i][1] = k_PACKET_RAND_ID & 0xff; break;
			case 12:
				m_packetName[i][0] = k_PACKET_TERRAIN_ID >> 8;
				m_packetName[i][1] = k_PACKET_TERRAIN_ID & 0xff; break;
			case 13:
				m_packetName[i][0] = k_PACKET_INSTALLATION_ID >> 8;
				m_packetName[i][1] = k_PACKET_INSTALLATION_ID & 0xff; break;
			case 14:
				m_packetName[i][0] = k_PACKET_CHAT_ID >> 8;
				m_packetName[i][1] = k_PACKET_CHAT_ID & 0xff; break;
			case 15:
				m_packetName[i][0] = k_PACKET_READINESS_ID >> 8;
				m_packetName[i][1] = k_PACKET_READINESS_ID & 0xff; break;
			case 16:
				m_packetName[i][0] = k_PACKET_HAPPY_ID >> 8;
				m_packetName[i][1] = k_PACKET_HAPPY_ID & 0xff; break;
			case 17:
				m_packetName[i][0] = k_PACKET_PLAYER_HAPPY_ID >> 8;
				m_packetName[i][1] = k_PACKET_PLAYER_HAPPY_ID & 0xff; break;
			case 18:
				m_packetName[i][0] = k_PACKET_REPORT_ID >> 8;
				m_packetName[i][1] = k_PACKET_REPORT_ID & 0xff; break;
			case 19:
				m_packetName[i][0] = k_PACKET_UNIT_MOVE_ID >> 8;
				m_packetName[i][1] = k_PACKET_UNIT_MOVE_ID & 0xff; break;
			case 20:
				m_packetName[i][0] = k_PACKET_UNIT_ORDER_ID >> 8;
				m_packetName[i][1] = k_PACKET_UNIT_ORDER_ID & 0xff; break;
			case 21:
				m_packetName[i][0] = k_PACKET_AGREEMENT_ID >> 8;
				m_packetName[i][1] = k_PACKET_AGREEMENT_ID & 0xff; break;
			case 22:
				m_packetName[i][0] = k_PACKET_CIVILIZATION_ID >> 8;
				m_packetName[i][1] = k_PACKET_CIVILIZATION_ID & 0xff; break;
			case 23:
				m_packetName[i][0] = k_PACKET_CITY_NAME_ID >> 8;
				m_packetName[i][1] = k_PACKET_CITY_NAME_ID & 0xff; break;
			case 24:
				m_packetName[i][0] = k_PACKET_DIP_PROPOSAL_ID >> 8;
				m_packetName[i][1] = k_PACKET_DIP_PROPOSAL_ID & 0xff; break;
			case 25:
				m_packetName[i][0] = k_PACKET_MESSAGE_ID >> 8;
				m_packetName[i][1] = k_PACKET_MESSAGE_ID & 0xff; break;
			case 26:
				m_packetName[i][0] = k_PACKET_CITY2_ID >> 8;
				m_packetName[i][1] = k_PACKET_CITY2_ID & 0xff; break;
			case 27:
				m_packetName[i][0] = k_PACKET_POLLUTION_ID >> 8;
				m_packetName[i][1] = k_PACKET_POLLUTION_ID & 0xff; break; 
			case 28:
				m_packetName[i][0] = k_PACKET_CITY_BQ_ID >> 8;
				m_packetName[i][1] = k_PACKET_CITY_BQ_ID & 0xff; break;
			case 29:
				m_packetName[i][0] = k_PACKET_KEYS_ID >> 8;
				m_packetName[i][1] = k_PACKET_KEYS_ID & 0xff; break;
			case 30:
				m_packetName[i][0] = k_PACKET_GAME_SETTINGS_ID >> 8;
				m_packetName[i][1] = k_PACKET_GAME_SETTINGS_ID & 0xff; break;
			case 31:
				m_packetName[i][0] = k_PACKET_NEW_ARMY_ID >> 8;
				m_packetName[i][1] = k_PACKET_NEW_ARMY_ID & 0xff; break;
			case 32:
				m_packetName[i][0] = k_PACKET_REMOVE_ARMY_ID >> 8;
				m_packetName[i][1] = k_PACKET_REMOVE_ARMY_ID & 0xff; break;
			case 33:
				m_packetName[i][0] = k_PACKET_DIP_RESPONSE_ID >> 8;
				m_packetName[i][1] = k_PACKET_DIP_RESPONSE_ID & 0xff; break;
			default:
				m_packetName[i][0] = '?';
				m_packetName[i][0] = '?';
				break;
		}
	}
}

void Network::TogglePacketLog()
{
	m_displayPackets = !m_displayPackets;
	if(m_displayPackets) {
		g_debugOwner = k_DEBUG_OWNER_NETWORK_CHAT;
	}
}

#endif

PlayerData::PlayerData(char* name, uint16 id) :
	m_id(id),
	m_index(-1),
	m_frozen(FALSE),
	m_blocked(0),
	m_ready(FALSE),
	m_ackBeginTurn(FALSE)
{
	if(name) {
		m_name = new char[strlen(name) + 1];
		strcpy(m_name, name);
	} else {
		m_name = new char[1];
		m_name[0] = 0;
	}
	m_bookmarks = new PointerList<PointerList<Packetizer>::PointerListNode>;
	m_packetList = new PointerList<Packetizer>;
	m_createdCities = new UnitDynamicArray;
	memset(&m_guid, 0, sizeof(GUID));

	m_sentResync = FALSE;
};

PlayerData::~PlayerData()
{
	Packetizer* packet;
	while(!m_packetList->IsEmpty()) {
		packet = m_packetList->RemoveHead();
		packet->Release();
	}

	if(m_name) {
		delete [] m_name;
	}
	delete m_bookmarks;
	delete m_packetList;
	delete m_createdCities;
}

uint8 Network::GetGameStyle() const
{
	return m_gameStyle;
}

BOOL Network::IsClassicStyle() const
{
	return m_gameStyle == 0;
}

BOOL Network::IsUnitMovesStyle() const
{
	return (m_gameStyle & k_GAME_STYLE_UNIT_MOVES) != 0;
}

BOOL Network::IsSpeedStyle() const
{
	return (m_gameStyle & k_GAME_STYLE_SPEED) != 0;
}

BOOL Network::IsTimedStyle() const
{
	return (m_gameStyle & k_GAME_STYLE_TOTAL_TIME) != 0;
}

sint32 Network::GetUnitMovesPerSlice() const
{
	return m_unitMovesPerSlice;
}

sint32 Network::GetUnitMovesUsed() const
{
	return m_unitMovesUsed;
}

sint32 Network::GetTotalStartTime() const
{
    return m_totalStartTime;
}

sint32 Network::GetTotalTimeUsed() const
{
    return m_totalTimeUsed;
}

sint32 Network::GetTurnStartTime() const
{
    return m_turnStartTime;
}

sint32 Network::GetTurnStartedAt() const
{
    return m_turnStartedAt;
}

sint32 Network::GetTurnEndsAt() const
{
	return m_turnEndsAt;
}

sint32 Network::GetBonusTime() const
{
	return m_bonusTime;
}

void Network::SetClassicStyle(BOOL fromServer)
{
	if(!IsActive() || (m_iAmHost || fromServer)) {
		m_gameStyle = 0;
		if(m_iAmHost) {
			Enqueue(new NetInfo(NET_INFO_CODE_CLASSIC_STYLE));
		}
	}
}

void Network::SetUnitMovesStyle(BOOL on, sint32 unitMovesPerSlice,
								BOOL fromServer)
{
	if(!IsActive() || (m_iAmHost || fromServer)) {
		m_unitMovesPerSlice = unitMovesPerSlice;
		if(on) {
			m_gameStyle |= k_GAME_STYLE_UNIT_MOVES;
		} else {
			m_gameStyle &= ~(k_GAME_STYLE_UNIT_MOVES);
		}
		if(m_iAmHost) {
			Enqueue(new NetInfo(NET_INFO_CODE_UNIT_MOVES_STYLE, on, unitMovesPerSlice));
		}
	}
}

void Network::SetSpeedStyle(BOOL on, sint32 timePerTurn,
							BOOL fromServer, sint32 timePerCity)
{
	if(!IsActive() || (m_iAmHost || fromServer)) {
		m_turnStartTime = timePerTurn;
		if(on) {
			uint32 oldStyle = m_gameStyle;
			m_gameStyle |= k_GAME_STYLE_SPEED;
			if(timePerCity > 0) {
				m_gameStyle |= k_GAME_STYLE_SPEED_CITIES;
				m_extraTimePerCity = timePerCity;
			} else {
				m_extraTimePerCity = 0;
				m_gameStyle &= ~(k_GAME_STYLE_SPEED_CITIES);
			}
			if(oldStyle != m_gameStyle) {
				m_turnStartedAt = time(0);
				ResetTurnEndsAt();
			}
		} else {
			m_gameStyle &= ~(k_GAME_STYLE_SPEED | k_GAME_STYLE_SPEED_CITIES);
		}
		
		if(m_iAmHost) {
			Enqueue(new NetInfo(NET_INFO_CODE_SPEED_STYLE, on, timePerTurn, timePerCity));
		}
	}
}

void Network::SetTimedStyle(BOOL on, sint32 timePerGame,
							BOOL fromServer)
{
	if(!IsActive() || (m_iAmHost || fromServer)) {
		m_totalStartTime = timePerGame;
		if(on) {
			if(!(m_gameStyle & k_GAME_STYLE_TOTAL_TIME))
				m_totalTimeUsed = 0;
			
			m_gameStyle |= k_GAME_STYLE_TOTAL_TIME;
		} else {
			m_gameStyle &= ~(k_GAME_STYLE_TOTAL_TIME);
		}
		if(m_iAmHost) {
			Enqueue(new NetInfo(NET_INFO_CODE_TIMED_STYLE, on, timePerGame));
		}
	}
}

void Network::SetSimultaneousStyle(BOOL on, BOOL fromServer)
{
	if(!IsActive() || (m_iAmHost || fromServer)) {
		if(on) {
			m_gameStyle = k_GAME_STYLE_SIMULTANEOUS;
		} else {
			m_gameStyle &= ~(k_GAME_STYLE_SIMULTANEOUS);
		}
		
		if(m_iAmHost) {
			Enqueue(new NetInfo(NET_INFO_CODE_SIMULTANEOUS_STYLE, on));
		}
	}
}
	
void Network::SetCarryoverStyle(BOOL on, BOOL fromServer)
{
	if(!IsActive() || (m_iAmHost || fromServer)) {
		if(on) {
			if(!(m_gameStyle & k_GAME_STYLE_CARRYOVER)) {
				m_gameStyle |= k_GAME_STYLE_CARRYOVER;
			}
		} else {
			m_gameStyle &= ~(k_GAME_STYLE_CARRYOVER);
		}

		m_bonusTime = 0;
		if(m_iAmHost) {
			Enqueue(new NetInfo(NET_INFO_CODE_CARRYOVER_STYLE, on));
		}
	}
}	

void Network::SetStyleFromServer(uint8 gameStyle,
								 sint32 movesPerSlice,
								 sint32 totalTime,
								 sint32 turnTime,
								 sint32 cityTime)
{
	m_gameStyle = gameStyle;
	m_unitMovesPerSlice = movesPerSlice;
	m_totalStartTime = totalTime;
	m_turnStartTime = turnTime;
	m_extraTimePerCity = cityTime;

	m_totalTimeUsed = 0;
}
 
void Network::UnitsMoved(sint32 count)
{
	if(!(m_gameStyle & k_GAME_STYLE_UNIT_MOVES))
		return;

	m_unitMovesUsed += count;
	if(m_unitMovesUsed >= m_unitMovesPerSlice && IsMyTurn()) {
		g_turn->EndThisSliceBeginNewSlice();
	}
}

void Network::SetMyTurn(BOOL turn)
{
	if(turn) {
		if(m_gameStyle & k_GAME_STYLE_UNIT_MOVES) {
			m_unitMovesUsed = 0;
		}
		
		m_turnStartedAt = time(0);
		if(m_gameStyle & (k_GAME_STYLE_SPEED | k_GAME_STYLE_SPEED_CITIES)) {
			ResetTurnEndsAt();
		}
		if(g_theProgressWindow) {
			ProgressWindow::EndProgress(g_theProgressWindow);
		}
		if(!m_isMyTurn) {
		}

	} else {
		if(m_isMyTurn) {
			
			sint32 timeUsed = time(0) - m_turnStartedAt;
			m_totalTimeUsed += timeUsed;
			if((m_gameStyle & k_GAME_STYLE_SPEED) &&
			   (m_gameStyle & k_GAME_STYLE_CARRYOVER)) {
				m_bonusTime = m_turnStartTime - timeUsed;
				if(m_bonusTime < 0)
					m_bonusTime = 0;
			}
		}
	}
	m_isMyTurn = turn;

	


}

BOOL Network::IsLocalPlayer(sint32 index)
{
	if(index == m_playerIndex)
		return TRUE;

	if(m_iAmClient)
		return FALSE;

	if(!g_player[index])
		return FALSE;

	if(g_player[index]->GetPlayerType() == PLAYER_TYPE_NETWORK)
		return FALSE;

	return TRUE;
}

void Network::TurnSync()
{
	if(IsLocalPlayer(g_selected_item->GetCurPlayer())) {
		SetMyTurn(TRUE);

		




















		m_turnStartedAt = time(0);
		if(m_gameStyle & (k_GAME_STYLE_SPEED | k_GAME_STYLE_SPEED_CITIES)) {
			ResetTurnEndsAt();
		}

		DPRINTF(k_DBG_NET, ("Adding finish begin turn for player %d.  Rand call count: %d\n",
							g_selected_item->GetCurPlayer(), g_rand->CallCount()));
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_FinishBeginTurn,
							   GEA_Player, g_selected_item->GetCurPlayer(),
							   GEA_End);
		if(g_player[g_selected_item->GetCurPlayer()]->m_playerType == PLAYER_TYPE_ROBOT) {
			CtpAi::BeginTurn(g_selected_item->GetCurPlayer());
		}
	}
}




void Network::EnterSetupMode()
{
	Assert(g_turn->GetRound() == 0);
	if(g_turn->GetRound() != 0)
		return;

	for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i]) {
			if(g_player[i]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
				g_player[i]->m_doneSettingUp = TRUE;
			} else {
				g_player[i]->m_doneSettingUp = FALSE;
				if(m_iAmHost) {
					Assert(g_player[i]->m_all_armies->Num() > 0);
					
					if(g_player[i]->m_all_armies->Num() > 0) {
						MapPoint pos;
						g_player[i]->m_all_armies->Access(0).GetPos(pos);
						SetSetupArea(i, pos, g_theProfileDB->SetupRadius());
					}
					SetPowerPoints(i, g_theProfileDB->PowerPoints());
				}
			}
		}
	}

	if(m_iAmHost) {
		Enqueue(new NetInfo(NET_INFO_CODE_SET_SETUP_MODE, TRUE));
	}
	m_setupMode = TRUE;
}

void Network::ExitSetupMode()
{
	if(m_iAmHost) {
		Enqueue(new NetInfo(NET_INFO_CODE_SET_SETUP_MODE, FALSE));
	}
	m_setupMode = FALSE;
}

void Network::SignalSetupDone(PLAYER_INDEX player)
{
	g_player[player]->m_doneSettingUp = TRUE;

	if(m_iAmClient) {
		SendAction(new NetAction(NET_ACTION_DONE_SETTING_UP));
		return;
	}		

	sint32 count = 0;
	sint32 active = 0;

	for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i]) {
			active++;
			if(g_player[i]->GetPlayerType() != PLAYER_TYPE_NETWORK) {
				g_player[i]->m_doneSettingUp = TRUE;
			}
			if(g_player[i]->m_doneSettingUp)
				count++;
		}
	}
	if(count >= active)
		ExitSetupMode();
}

BOOL Network::SetupMode() const
{
	return m_setupMode;
}

void Network::SetSetupArea(PLAYER_INDEX player, const MapPoint &center,
						   sint32 radius)
{
	BOOL revealed;

	g_player[player]->m_setupCenter = center;
	g_player[player]->m_setupRadius = radius;

	
	g_player[player]->AddUnitVision(center, radius, revealed);

	
	g_player[player]->OwnExploredArea();

	
	g_player[player]->RemoveUnitVision(center, radius);

	if(player == g_selected_item->GetVisiblePlayer()) {
		g_tiledMap->CopyVision();
	}

	if(m_iAmHost) {
		Enqueue(new NetInfo(NET_INFO_CODE_SET_SETUP_AREA,
							player, center.x, center.y,
							radius));
	}
}

BOOL Network::IsInSetupArea(PLAYER_INDEX player, const MapPoint &pnt) const
{
	sint32 dist = UnitData::GetDistance(pnt,
										g_player[player]->m_setupCenter,
										g_player[player]->m_setupRadius);
	double rplus = double(g_player[player]->m_setupRadius) + 0.5;
	if(dist > (rplus * rplus)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

void Network::SetPowerPoints(PLAYER_INDEX player, sint32 points)
{
	g_player[player]->m_powerPoints = points;
	if(m_iAmHost) {
		Enqueue(new NetInfo(NET_INFO_CODE_POWER_POINTS,
							player, points));
	}
}

BOOL Network::CanStillSetup(PLAYER_INDEX index)
{
	return !g_player[index]->m_doneSettingUp;
}

BOOL Network::CurrentPlayerAckedBeginTurn()
{
	Assert(m_playerData[g_selected_item->GetCurPlayer()]);
	if(!m_playerData[g_selected_item->GetCurPlayer()])
		return FALSE;

	return m_playerData[g_selected_item->GetCurPlayer()]->m_ackBeginTurn;
}

void Network::BeginTurn(PLAYER_INDEX index)
{
	if(m_playerData[index]) {
		m_playerData[index]->m_ackBeginTurn = FALSE;
	}
}

void Network::AckBeginTurn(PLAYER_INDEX index)
{
	Assert(m_playerData[index]);
	if(m_playerData[index]) {
		m_playerData[index]->m_ackBeginTurn = TRUE;
	}
	

	
	
	g_gevManager->AddEvent(GEV_INSERT_Tail,
		GEV_BeginTurn,
		GEA_Player, index,
		GEA_Int, g_player[index]->m_current_round + 1,
		GEA_End);

}

void Network::AddCreatedCity(PLAYER_INDEX owner, Unit &city)
{
	if(m_playerData[owner]) {
		m_playerData[owner]->m_createdCities->Insert(city);
	}
}

UnitDynamicArray *Network::GetCreatedCities(PLAYER_INDEX owner)
{
	Assert(m_playerData[owner]);
	if(!m_playerData[owner])
		return NULL;
	
	return m_playerData[owner]->m_createdCities;
}

void Network::AddResetCityOwnerHack(const Unit &unit)
{
	m_resetCityOwnerHackList->Insert(unit);
}

void Network::DoResetCityOwnerHack()
{
	sint32 i, n = m_resetCityOwnerHackList->Num();
	
	for(i = 0; i < n; i++) {
		Unit u = m_resetCityOwnerHackList->Access(i);
		if(g_theUnitPool->IsValid(u)) {
			Enqueue(u.AccessData());
			Enqueue(u.AccessData(),
					u.AccessData()->GetCityData(),
					TRUE);
		}
	}

	for(i = 0; i < n; i++) {
		m_resetCityOwnerHackList->DelIndex(0);
	}
}

void Network::SetupPlayerFromNSPlayerInfo(uint16 id, sint32 index)
{
	Player *p = g_player[index];
	Assert(p);
	if(p) {
		NSPlayerInfo *nspi = NULL;
		PointerList<NSPlayerInfo>::Walker walk(m_nsPlayerInfo);
		while(walk.IsValid()) {
			if(walk.GetObj()->m_id == id) {
				nspi = walk.GetObj();
				break;
			}
			walk.Next();
		}
		
		if(!nspi)
			return;
		if(!m_fromSave) {
		}
	}
}

sint32 Network::GetNumHumanPlayers()
{
	return m_nsPlayerInfo->GetCount();
}

uint32 Network::GetHumanMask()
{
	uint32 mask = 0;
	sint32 i;
	sint32 n = m_nsPlayerInfo->GetCount();
	if(g_theProfileDB->NoHumanPlayersOnHost())
		n--;

	for(i = 1; i < k_MAX_PLAYERS; i++) {
		if(g_player[i] && g_player[i]->m_networkId != 0) {
			mask |= (1 << i);
			n--;
		}
	}
	if(n > 0) {
		for(i = 1; i < k_MAX_PLAYERS; i++) {
			if(g_player[i] && !(mask & (1 << i))) {
				mask |= (1 << i);
				n--;
				if(n <= 0)
					break;
			}
		}
	}
	
	return mask;
}

void Network::SetCondensePopMoves(BOOL on)
{
	m_condensePopMoves = on;
}

BOOL Network::ShouldAckBeginTurn()
{
	
	if(m_enactedDiplomaticRequests->Num() > 0)
		return FALSE;
	if(m_iAmClient) {
		if(m_waitingOnResync)
			return FALSE;
	}

	return TRUE;
}

void Network::AddEnact(DiplomaticRequest &req)
{
	m_enactedDiplomaticRequests->Insert(req);
}

void Network::RemoveEnact(DiplomaticRequest &req)
{
	if(m_iAmClient) {
		
		
		
		
		
		
		m_enactedDiplomaticRequests->Del(req);
	} else {
		Assert(m_enactedDiplomaticRequests->Access(0).m_id == req.m_id);
		if(m_enactedDiplomaticRequests->Access(0).m_id != req.m_id) {
			while(m_enactedDiplomaticRequests->Num() > 0) {
				m_enactedDiplomaticRequests->Access(0).Enact(TRUE);
				m_enactedDiplomaticRequests->DelIndex(0);
			}
			Resync(g_selected_item->GetCurPlayer());
			return;
		}
		if(g_theDiplomaticRequestPool->IsValid(req)) {
			
			
			m_enactedDiplomaticRequests->Access(0).Enact(TRUE);
		}
		m_enactedDiplomaticRequests->DelIndex(0);

		if(m_enactedDiplomaticRequests->Num() < 1 &&
		   m_endTurnWhenClear) {
			g_turn->EndThisTurnBeginNewTurn(TRUE);
		}
	}
}

void Network::SetGuid(uint16 id, GUID *guid)
{
	BOOL found = FALSE;
	PointerList<PlayerData>::Walker walk(m_newPlayerList);
	while(walk.IsValid() && !found) {
		if(walk.GetObj()->m_id == id) {
			walk.GetObj()->m_guid = *guid;
			found = TRUE;
		}
		walk.Next();
	}
	Assert(found);
}

void Network::KickPlayer(sint32 player)
{
	if(m_iAmHost && m_playerData[player]) {
		m_netIO->KickPlayer(m_playerData[player]->m_id);
	}
}

sint32 Network::GetProgress()
{
	return m_progress;
}

void Network::ResetGuid(sint32 player)
{
	if(m_playerData[player] && g_player[player]) {
		g_player[player]->m_networkGuid = m_playerData[player]->m_guid;
		QueuePacketToAll(new NetSetPlayerGuid(player));
	}
}


sint32 Network::CountOpenSlots()
{
	sint32 i;
	sint32 count = 0;
	for(i = 1; i < k_MAX_PLAYERS; i++) {
		if(g_player[i] && g_player[i]->m_openForNetwork)
			count++;
	}
	return count;
}

sint32 Network::CountTakenSlots()
{
	sint32 i;
	sint32 count = 0;
	for(i = 1; i < k_MAX_PLAYERS; i++) {
		if(m_playerData[i]) {
			count++;
		}
	}
	return count;
}

void Network::OpenPlayer(sint32 player)
{
	if(!m_iAmHost)
		return;

	if(!g_player[player] || g_player[player]->m_isDead) {
		return;
	}
	g_player[player]->m_openForNetwork = TRUE;
	SetMaxPlayers(CountOpenSlots() + CountTakenSlots());
}

void Network::ClosePlayer(sint32 player)
{
	if(!m_iAmHost)
		return;

	if(!g_player[player]) {
		return;
	}
	g_player[player]->m_openForNetwork = FALSE;
	SetMaxPlayers(CountOpenSlots() + CountTakenSlots());
}

void Network::ResetTurnEndsAt()
{
	m_turnEndsAt = time(0) + m_turnStartTime + m_bonusTime;
	if((m_gameStyle & k_GAME_STYLE_SPEED_CITIES) && g_player && g_player[m_playerIndex]) {
		m_turnEndsAt += g_player[m_playerIndex]->m_all_cities->Num() *
			m_extraTimePerCity;
	}
}

void Network::SetAllPlayersReady()
{
	if(!m_readyToStart && !m_crcError) {
		if(g_tiledMap) {
			g_tiledMap->CopyVision();
		}
		NewTurnCount::ClientStartNewYear();

		
		c3_RemoveAbortMessage();
		SetReadyToStart(TRUE);
	}
}


void Network::SetProgress(sint32 progress)
{
	if(m_crcError)
		return;

	m_progress = progress;
	c3_AbortUpdateData( NULL, (progress > 100 ? 100 : progress) );
	if(m_progress >= 100) {
		const char *str = g_theStringDB->GetNameStr("NETWORK_WAITING_ON_PLAYERS");
		
		char nonConstStr[1024];
		if(str)
			strcpy(nonConstStr, str);
		c3_AbortUpdateData( str ? nonConstStr : "Waiting on players", 100);
	} else {
		c3_AbortUpdateData(NULL, progress);

		
		g_civApp->ProcessGraphicsCallback();
	}
}

void Network::SetReadyToStart(BOOL ready)
{
	m_readyToStart = ready;
	if(ready) {
		g_director->AddCopyVision();
		g_tiledMap->InvalidateMix();
		g_tiledMap->InvalidateMap();
		g_tiledMap->Refresh();
		g_radarMap->Update();

		if(g_player[m_playerIndex]->m_first_city) {
			MapPoint pos;
			if(g_player[m_playerIndex]->m_all_armies->Num() > 0) {
				g_player[m_playerIndex]->m_all_armies->Access(0).GetPos(pos);
				g_director->AddCenterMap(pos);
			}
		}

		if(m_wasAttached) {
			
			g_player[m_playerIndex]->SetPlayerType(PLAYER_TYPE_ROBOT);
		}

		if(m_iAmHost) {
			
			
			sint32 i;
			for(i = 0; i < k_MAX_PLAYERS; i++) {
				if(!g_player[i]) continue;
				if(i == m_playerIndex) continue;
				if(!m_playerData[i] && g_player[i]->GetPlayerType() != PLAYER_TYPE_ROBOT) {
					SendLeftMessage(g_player[i]->m_civilisation->GetLeaderName(), i);
					
					
					
					
					g_player[i]->SetPlayerType(PLAYER_TYPE_ROBOT);
					SetRobotName(i);
					OpenPlayer(i);
				} else if(g_player[i]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
					if(m_dynamicJoin) {
						OpenPlayer(i);
					} else {
						ClosePlayer(i);
					}
				}
				if(i == g_selected_item->GetCurPlayer() && g_player[i]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
					
					g_director->AddEndTurn();
				}
				QueuePacketToAll(new NetSetPlayerGuid(i));
			}

			
			
			
			QueuePacketToAll(new NetInfo(NET_INFO_CODE_ALL_PLAYERS_READY));
		}

		MainControlPanel::UpdatePlayer(g_selected_item->GetCurPlayer());
	}
}

void Network::SendJoinedMessage(MBCHAR *name, sint32 player)
{
	if(g_slicEngine && player != m_playerIndex && name) {
		SlicObject *so = new SlicObject("351NetworkPlayerJoined");
		so->AddAction(name);
		so->AddCivilisation(player);
		so->AddRecipient(m_playerIndex);

		MBCHAR interp[k_MAX_NAME_LEN];
		stringutils_Interpret(g_theStringDB->GetNameStr("NETWORK_PLAYER_JOINED"), *so, interp);
		AddChatText(interp, strlen(interp), 0, FALSE);

		g_slicEngine->Execute(so);

		
	}

	if(m_iAmHost) {
		QueuePacketToAll(new NetInfoMessage(NET_MSG_PLAYER_JOINED,
										name, player));
	}
}

void Network::SendWrongPlayerJoinedMessage(MBCHAR *name, sint32 player)
{
	if(g_slicEngine) {
		SlicObject *so = new SlicObject("352DifferentPlayerJoined");
		so->AddAction(name);
		so->AddCivilisation(player);
		so->AddRecipient(m_playerIndex);
		g_slicEngine->Execute(so);
	}
}

void Network::SendLeftMessage(MBCHAR *name, sint32 player)
{
	if(g_slicEngine) {
		SlicObject *so;
		so = new SlicObject("350NetworkPlayerLeft");
		so->AddAction(name);
		
		so->AddRecipient(m_playerIndex);

		MBCHAR interp[k_MAX_NAME_LEN];
		stringutils_Interpret(g_theStringDB->GetNameStr("NETWORK_PLAYER_LEFT"), *so, interp);
		AddChatText(interp, strlen(interp), 0, FALSE);

		g_slicEngine->Execute(so);
	}

	if(m_iAmHost) {
		QueuePacketToAll(new NetInfoMessage(NET_MSG_PLAYER_LEFT,
										name, player));
	}
}

void Network::SendNewHostMessage(MBCHAR *name, sint32 player)
{
	if(g_slicEngine) {
		SlicObject *so;
		if(m_iAmHost) {
			so = new SlicObject("353YouAreNowHost");
		} else {
			so = new SlicObject("354NewHost");
		}
		so->AddAction(name);
		so->AddRecipient(m_playerIndex);

		MBCHAR interp[k_MAX_NAME_LEN];
		stringutils_Interpret(g_theStringDB->GetNameStr("NETWORK_YOU_ARE_NOW_HOST"), *so, interp);
		AddChatText(interp, strlen(interp), 0, FALSE);

		g_slicEngine->Execute(so);
	}

	if(m_iAmHost) {
		QueuePacketToAll(new NetInfoMessage(NET_MSG_NEW_HOST,
											name, player));
	}
}

void Network::Resync(sint32 playerIndex) 
{	
	if(!m_playerData[playerIndex]) {
		return;
	}

	if(m_playerData[playerIndex]->m_sentResync)
		return;

	uint16 id = IndexToId(playerIndex);
	if(id != 0xffff) {
		m_playerData[playerIndex]->m_sentResync = TRUE;

		
		
		
		m_playerData[playerIndex]->m_createdUnits.Clear();
		m_playerData[playerIndex]->m_createdArmies.Clear();
		m_playerData[playerIndex]->m_createdCities->Clear();

		
		
		if(g_selected_item->GetCurPlayer() == playerIndex &&
		   !m_playerData[playerIndex]->m_ackBeginTurn) {

			
			
			g_gevManager->AddEvent(GEV_INSERT_Tail,
				GEV_BeginTurn,
				GEA_Player, playerIndex,
				GEA_Int, g_player[playerIndex]->m_current_round,
				GEA_End);
		}

		QueuePacket(id, new NetInfo(NET_INFO_CODE_RESYNC));
		SetReady(id);
	}
	DPRINTF(k_DBG_NET, ("Resync stack trace: %s\n", c3debug_StackTrace()));
}

void Network::StartResync()
{
	Assert(m_iAmClient);
	if(!m_iAmClient)
		return;

	close_AllScreens();

	if(g_director) {
		g_director->CatchUp();
	}

	if(g_tiledMap) {
		g_tiledMap->QuickBlackBackGround(NULL);
	}

	if(sci_advancescreen_isOnScreen()) {
		sci_advancescreen_removeMyWindow(AUI_BUTTON_ACTION_EXECUTE);
	}

	
	
	if(g_player[m_playerIndex]->GetPlayerType() == PLAYER_TYPE_ROBOT ) {
		
		m_wasAttached = TRUE;
	}

	m_readyToStart = FALSE;
	m_waitingOnResync = FALSE;

	if(m_gameObjects) {
		
		
		
		
		
		delete m_gameObjects;
		m_gameObjects = new NetGameObj;
	}

	ClearDeadUnits();

	DPRINTF(k_DBG_NET, ("Acknowledging resync\n"));
	QueuePacket(m_hostId, new NetReport(NET_REPORT_ACK_RESYNC));
	

	const char *str = g_theStringDB->GetNameStr("NETWORK_RESYNCING");
	char nonConstStr[1024];
	if(str)
		strcpy(nonConstStr, str);
	c3_AbortMessage( str ? nonConstStr : "Resyncing", k_UTILITY_PROGRESS_ABORT, network_AbortCallback );

	if(g_gevManager)
		g_gevManager->NotifyResync();

	if(g_director)
		g_director->NotifyResync();

	CtpAi::Initialize();
}

void Network::AckResync(sint32 index)
{
	if(!m_playerData[index])
		return;

	m_playerData[index]->m_sentResync = FALSE;
}

void Network::RequestResync(RESYNC_REASON reason)
{
	Assert(m_iAmClient);
	if(!m_iAmClient)
		return;

	if(m_waitingOnResync)
		return;

	DPRINTF(k_DBG_NET, ("RequestResync(%d) stack trace: %s\n", reason, c3debug_StackTrace()));

	m_readyToStart = FALSE;
	const char *str = g_theStringDB->GetNameStr("NETWORK_RESYNCING");
	char nonConstStr[1024];
	if(str)
		strcpy(nonConstStr, str);
	c3_AbortMessage( str ? nonConstStr : "Resyncing", k_UTILITY_PROGRESS_ABORT, network_AbortCallback);

	SendAction(new NetAction(NET_ACTION_REQUEST_RESYNC, reason));

	m_waitingOnResync = TRUE;
}

BOOL Network::SentResync(sint32 playerindex)
{
	Assert(m_iAmHost);
	if(!m_iAmHost)
		return FALSE;

	if(!m_playerData[playerindex])
		return FALSE;

	return m_playerData[playerindex]->m_sentResync;
}

uint32 Network::PackedPos(const MapPoint &pnt)
{
	return ((pnt.x & 0x7fff) << 16) |
		(pnt.y & 0xffff);
}

void Network::UnpackedPos(uint32 p, MapPoint &pnt)
{
	pnt.x = (p & 0x7fff0000) >> 16;
	pnt.y = (p & 0x0000ffff);
}

void network_PlayerListCallback(sint32 player, sint32 val, sint32 action)
{
	if(val) {
		switch(action) {
			case PLAYER_ACTION_KICK:
				if(player != g_network.GetPlayerIndex())
					g_network.KickPlayer(player);
				break;
			case PLAYER_ACTION_OPEN:
				
					g_network.OpenPlayer(player);
				
				break;
			case PLAYER_ACTION_CLOSE:
				
					g_network.ClosePlayer(player);
				
				break;
		}
	}
	if(g_networkPlayersScreen) {
		g_networkPlayersScreen->UpdateData();
	}
}


MBCHAR *Network::GetStatusString(sint32 player)
{
	static MBCHAR strbuf[1024];

	if(!g_player || !g_player[player])
		return NULL;
	if(g_player[player]->m_playerType == PLAYER_TYPE_HUMAN) {
		strcpy(strbuf, g_theStringDB->GetNameStr("NETWORK_PLAYER_STATUS_HUMAN"));
	} else if(g_player[player]->m_playerType == PLAYER_TYPE_NETWORK) {
		strcpy(strbuf, g_theStringDB->GetNameStr("NETWORK_PLAYER_STATUS_CONNECTED"));
	} else {
		if(g_player[player]->m_openForNetwork) {
			strcpy(strbuf, g_theStringDB->GetNameStr("NETWORK_PLAYER_STATUS_AI_OPEN"));
		} else {
			strcpy(strbuf, g_theStringDB->GetNameStr("NETWORK_PLAYER_STATUS_AI_CLOSED"));
		}
	}
	return strbuf;
}

void Network::SetDynamicJoin(BOOL on)
{
	m_dynamicJoin = on;
	if(m_readyToStart) {
		if(m_netIO) {
			if(m_dynamicJoin) {
				SetMaxPlayers(CountOpenSlots() + CountTakenSlots());
			} else {
				SetMaxPlayers(CountTakenSlots());
			}
		}
	}
}

void Network::ChunkList(uint16 id, PointerList<Packetizer> *list)
{
	Assert(list);
	if(!list)
		return;

	sint32 mapBufSize = list->GetCount() * 258 + 16384;
	uint8 *mapBuf = new uint8[mapBufSize];
	
	mapBuf[0] = k_CHUNK_HEAD;
	mapBuf[1] = k_CHUNK_BODY;
	sint32 size = 2;

	
	while(list->GetHead()) {
		uint16 len = 0;
		Packetizer *packet = list->RemoveHead();
		packet->Packetize(&mapBuf[size + 2], len);
		Assert(len < 16384); 
		                   
		putshort(&mapBuf[size], len);
		size += len + 2;

		if(len >= 256) {
			
			
			uint8* newMapBuf = new uint8[mapBufSize + len - 256];
			memcpy(newMapBuf, mapBuf, size);
			delete [] mapBuf;
			mapBuf = newMapBuf;
			mapBufSize += len - 256;
		}

		delete packet;
	}
	
	Packetizer *chunk = new Packetizer(mapBuf, size);
	QueuePacket(id, chunk);
	
}

void Network::DechunkList(sint32 from, uint8 *buf, sint32 len)
{
	
	sint32 pos;
	pos = 0;
	while(pos < len) {
		sint32 packLen;
		packLen = getshort(&buf[pos]); pos += 2;
		PacketReady(from, &buf[pos], packLen);
		if(!m_initialized)
			return;
		pos += packLen;
	}
	Assert(pos == len);
}

void Network::SetCRCError()
{
	m_crcError = TRUE;
}

void Network::SetMaxPlayers(sint32 maxPlayers)
{
	m_netIO->SetMaxPlayers(maxPlayers, CountOpenSlots() > 0);
}

void Network::SetRobotName(sint32 player)
{
	if(!g_player[player])
		return;

	Civilisation *civ = g_player[player]->m_civilisation;
	StringId strId;
	if(civ->GetGender() == GENDER_MALE) {
		strId = g_theCivilisationDB->GetLeaderName(civ->GetCivilisation());
	} else {
		strId = g_theCivilisationDB->GetLeaderNameFemale(civ->GetCivilisation());
	}
	civ->AccessData()->SetLeaderName(g_theStringDB->GetNameStr(strId));

	QueuePacketToAll(new NetSetLeaderName(player));
	if(g_networkPlayersScreen) {
		g_networkPlayersScreen->UpdateData();
	}
}

					
void Network::SendCity(CityData *cd)
{
	SendToServer(new NetCity(cd->GetHomeCity().AccessData(), TRUE));
	SendToServer(new NetCity2(cd, TRUE));
	SendToServer(new NetCityBuildQueue(cd));
	SendToServer(new NetHappy(cd->GetHomeCity(),
							  cd->GetHappy(), TRUE));
}

void Network::NotifyDiplomacyResponse(Response &response, sint32 p1, sint32 p2)
{	
	if(!g_network.IsActive()) return;

	if(IsHost()) {
		QueuePacketToAll(new NetDipResponse(response, p1, p2));
	} else if(IsLocalPlayer(p1)) {
		SendToServer(new NetDipResponse(response, p1, p2));
	}
}

void Network::NotifyDiplomacyThreatRejected(Response &response, const Response &sender_response, sint32 p1, sint32 p2)
{
}

void Network::SendGroupRequest(const CellUnitList &units, const Army &army)
{
	SendToServer(new NetGroupRequest(units, army));
}

void Network::SendUngroupRequest(const Army &army, const CellUnitList &units)
{
	SendToServer(new NetUngroupRequest(army, units));
}


void network_VerifyGameData()
{
	
	sint32 p;
	sint32 u;
	for(p = 0; p < k_MAX_PLAYERS; p++) {
		if(!g_player[p]) continue;

		for(u = 0; u < g_player[p]->m_all_units->Num(); u++) {
			if(!g_player[p]->m_all_units->Access(u).IsValid()) {
				g_network.RequestResync(RESYNC_INVALID_UNIT);
				return;
			}
		}

		sint32 a;
		for(a = 0; a < g_player[p]->m_all_armies->Num(); a++) {
			if(!g_player[p]->m_all_armies->Access(a).IsValid()) {
				g_network.RequestResync(RESYNC_INVALID_ARMY_OTHER);
				return;
			}
		}

		sint32 c;
		for(c = 0; c < g_player[p]->m_all_cities->Num(); c++) {
			if(!g_player[p]->m_all_cities->Access(c).IsValid()) {
				g_network.RequestResync(RESYNC_INVALID_UNIT);
				return;
			}
		}
	}

	sint32 x, y;
	for(x = 0; x < g_theWorld->GetXWidth(); x++) {
		for(y = 0; y < g_theWorld->GetYHeight(); y++) {
			Cell *cell = g_theWorld->GetCell(x, y);
			for(u = 0; u < cell->GetNumUnits(); u++) {
				if(!cell->AccessUnit(u).IsValid()) {
					g_network.RequestResync(RESYNC_INVALID_UNIT);
					return;
				}
				if(cell->GetCity().m_id != 0 && !cell->GetCity().IsValid()) {
					g_network.RequestResync(RESYNC_INVALID_UNIT);
					return;
				}
			}
		}
	}
}
