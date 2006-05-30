



#include "c3.h"
#include "net_io.h"
#include "net_anet.h"
#include "net_util.h"
#include "net_types.h"

#include "anet.h"

#include "netfunc.h"
#include "netshell.h"

extern NETFunc *g_netfunc;

#define HOST_ID dppt_MAKE('H', 'I')
#define ADDME_ID dppt_MAKE('A', 'M')




ActivNetIO::ActivNetIO()
{
	m_dp = NULL;
	m_sessionState = SESSION_STATE_READY;
	m_isHost = FALSE;
	m_response = NULL;
	m_name = NULL;
	m_hostId = 0;
	m_pid = 0;
	m_broadcastAddMessage = FALSE;
	dpini_SetFile("dp.ini");
}

ActivNetIO::~ActivNetIO()
{
	uint16 i;

	if(m_name) {
		delete [] m_name;
		m_name = NULL;
	}

	if(m_dp) {
		dpDestroyPlayer(m_dp, m_pid);
		Idle();
		dpClose(m_dp);
		
		
		

		int start = time(0);
		do {
			Idle();
		} while(time(0) < start + 3);


		dpDestroy(m_dp, 0);
		m_dp = NULL;
	}

	while(!m_playerList.IsEmpty()) {
		AnetPlayerData *pd = m_playerList.RemoveHead();
		delete pd;
	}

	for(i = 0; i < m_transports.GetSize(); i++) {
		dp_transport_t *trans = (dp_transport_t *)m_transports.Get(i);
		delete trans;
	}

	for(i = 0; i < m_sessions.GetSize(); i++) {
		dp_session_t *sess = (dp_session_t *)m_sessions.Get(i);
		delete sess;
	}
}

void dp_PASCAL anet_EnumPlayers(dpid_t id,
								dp_char_t *name,
								long flags,
								void *context)
{
	((ActivNetIO *)context)->PlayerCallback(id, name, flags);
}

void ActivNetIO::PlayerCallback(dpid_t id,
								dp_char_t *name,
								long flags)
{
	if(id != dp_ID_NONE) {
		Assert(!m_got_end_players);
		if(flags & dp_EPC_FLAGS_LOCAL) {
			m_pid = id;
		}
		m_playerList.AddTail(new AnetPlayerData(id, name));
		m_response->AddPlayer(id, name);
	} else {
		m_got_end_players = TRUE;
		if(m_isHost) {
			
#if 0
			PointerList<AnetPlayerData>::Walker walk(&m_playerList);
			while(walk.IsValid()) {
				uint8 buf[512];
				buf[0] = 'H';
				buf[1] = 'I';
				putshort(&buf[2], m_pid);
				dp_result_t res;
				res = dpSend(m_dp,
							 m_pid,
							 walk.GetObj()->m_id,
							 dp_SEND_RELIABLE,
							 buf,
							 4);
				Assert(res == dp_RES_OK);
				walk.Next();
			}
#endif
		} else {
			m_broadcastAddMessage = TRUE;
			m_broadcastAddMessageTime = time(0) - 10;
		}
  	}
}

void ActivNetIO::SetDP(dp_t *dp)
{
	dp_result_t res;
	m_dp = dp;

	
	m_isHost = g_netfunc->IsHost();
	

	if(m_isHost) {
		m_hostId = m_pid;
	}

	if(m_dp) {
		res = dpSetPingIntervals(m_dp, 0, 0);
		Assert(res == dp_RES_OK);

		m_got_end_players = FALSE;
		dpEnumPlayers(m_dp, NULL, anet_EnumPlayers,
					  this, 2000);
		Assert(m_got_end_players);
		if(m_isHost) {
			PointerList<AnetPlayerData>::Walker walk(&m_playerList);
			while(walk.IsValid()) {
				uint8 buf[512];
				buf[0] = 'H';
				buf[1] = 'I';
				putshort(&buf[2], m_pid);
				res = dpSend(m_dp,
							 m_pid,
							 walk.GetObj()->m_id,
							 dp_SEND_RELIABLE,
							 buf,
							 4);
				Assert(res == dp_RES_OK);
				walk.Next();
			}
		}
		m_response->SessionReady(NET_ERR_OK, NULL);
	}

}


void dp_PASCAL anet_EnumTransportsCallback(const dp_transport_t *fname,
							  const comm_driverInfo_t *description,
							  void* context)
{
	
	((ActivNetIO*)context)->TransportCallback(fname, description);
}



void
ActivNetIO::TransportCallback(const dp_transport_t *fname,
							   const comm_driverInfo_t *description)
{
	dp_transport_t* add = new dp_transport_t;
	*add = *fname;

	
	sint32 id = m_transports.Add(add);

	
	m_response->EnumTransport(NET_ERR_OK, id, fname->fname, (void*)description);
}


NET_ERR
ActivNetIO::EnumTransports()
{
	dp_transport_t dlldir;
	memset(&dlldir, 0, sizeof(dlldir));

	
	strcpy(dlldir.fname, "dll\\net");

	
	dp_result_t dp_res = dpEnumTransports(&dlldir,
										anet_EnumTransportsCallback,
										this);
	return (dp_res == dp_RES_OK) ? NET_ERR_OK : NET_ERR_NO_TRANSPORTS;
}


NET_ERR
ActivNetIO::SetTransport(sint32 trans_id) 

{
	if(!m_dp) {

		
		Assert(trans_id < m_transports.GetSize() && trans_id >= 0);
		if(trans_id >= m_transports.GetSize() || trans_id < 0) {
			return NET_ERR_BADTRANSPORT;
		}

		dp_transport_t* trans;
		commInitReq_t commInitReq;
		uint8 modeminit[80];
		uint8 phonenum[80];

		
		
		memset(&commInitReq, 0, sizeof(commInitReq));
		commInitReq.sessionId = rand() ^ (rand() << 16) ^ time(0);
		commInitReq.reqLen = sizeof(commInitReq_t);
		modeminit[0] = 0;
		phonenum[0] = 0;
		commInitReq.modeministr = (char*)modeminit;
		commInitReq.phonenum = (char*)phonenum;
		commInitReq.baud = 19200;
		commInitReq.hwirq = 0;
		commInitReq.portnum = 0;
		commInitReq.dialing_method = comm_INIT_DIALING_METHOD_TONE;
		
		
		trans = (dp_transport_t*)m_transports.Get(trans_id);

		
		dp_result_t res = dpCreate(&m_dp, 
								   trans,
								   &commInitReq,
								   NULL);
		if(res != dp_RES_OK)
			return NET_ERR_TRANSPORTERROR;
		return NET_ERR_OK;
	}
	
	
	return NET_ERR_ALREADYOPEN;
}

int dp_PASCAL anet_CreateSessionCallback(dp_session_t *ps,
									  long *pTimeout,
									  long flags,
									  void* context)
{
	
	return ((ActivNetIO*)context)->SessionReadyCallback(ps, pTimeout, flags);
}


void dp_PASCAL anet_PlayerReadyCallback(dpid_t id, dp_char_t *name,
									long flags, void *context)
{
	
	((ActivNetIO*)context)->PlayerReady(id, name, flags);
}


void
ActivNetIO::PlayerReady(dpid_t id, char_t *name, sint32 flags)
{
	
	m_pid = id;

	
	m_response->SessionReady(NET_ERR_OK, &m_session);
}


sint32
ActivNetIO::SessionReadyCallback(dp_session_t *ps,
								 long * pTimeout,
								 long flags)
{
	if(ps) {
		m_session = *ps;

		
		
		
		
		m_sessionState = SESSION_STATE_CREATE_PLAYER;
	} else {
		m_response->SessionReady(NET_ERR_TRANSPORTERROR, NULL);
	}

	return ps != NULL;
}


NET_ERR
ActivNetIO::Host(char* sessionName)
{
	dp_session_t sess;
	dp_result_t res;

	Assert(m_dp != NULL);
	if(m_dp == NULL) {
		return NET_ERR_NOTSTARTED;
	}

	
	if(m_state == ANET_STATE_CONTACTING_LOBBY) {
		sint32 t = time(0) + 3;
		while(time(0) < t) {
			Idle();
		}
		m_state = ANET_STATE_READY;
	}

	m_isHost = TRUE;

	
	memset(&sess, 0, sizeof(sess));
	sess.sessionType = CIV3_SPECIES; 
	sess.maxPlayers = (uint16)16 - 1;
	strncpy(sess.sessionName, sessionName, dp_SNAMELEN);
	sess.sessionName[dp_SNAMELEN - 1] = 0;
	sess.flags = dp_SESSION_FLAGS_CREATESESSION;  
	sess.dwUser1 = 0;

	
	res = dpOpen(m_dp, &sess, anet_CreateSessionCallback, this);
	if(res == dp_RES_OK) {
		return NET_ERR_OK;
	} else {
		return NET_ERR_TRANSPORTERROR;
	}
}


int dp_PASCAL anet_EnumSessionsCallback(dp_session_t *sDesc,
										long *pTimeout,
										long flags,
										void* context)
{
	
	return ((ActivNetIO*)context)->SessionCallback(sDesc, pTimeout, flags);
}


sint32
ActivNetIO::SessionCallback(dp_session_t *sDesc, 
							 long *pTimeout, 
							 long flags)
{
	if(sDesc) {
		
		dp_session_t *add = new dp_session_t;
		*add = *sDesc;
		sint32 idx = m_sessions.Add(add);

		
		m_response->EnumSession(NET_ERR_OK,
								   idx,
								   sDesc->sessionName,
								   add);
		return TRUE;
	} else {
		
		
		m_response->EnumSession(NET_ERR_NOMORESESSIONS,
								   -1,
								   "ERROR",
								   NULL);
		return FALSE;
	}
}


NET_ERR
ActivNetIO::EnumSessions()
{
	Assert(m_dp != NULL);

	dp_session_t sess;
	dp_result_t res;

	
	if(m_state == ANET_STATE_CONTACTING_LOBBY) {
		sint32 t = time(0) + 3;
		while(time(0) < t) {
			Idle();
		}
		m_state = ANET_STATE_READY;
	}

	
	memset(&sess, 0, sizeof(sess));
	sess.sessionType = CIV3_SPECIES;
	res = dpEnumSessions(m_dp, &sess, NULL, 1750L, anet_EnumSessionsCallback, this);
	return NET_ERR_OK;
}


NET_ERR
ActivNetIO::Join(sint32 sesindex)
{
	dp_session_t* sess;
	dp_result_t res;
	
	Assert(m_dp != NULL);

	
	Assert(sesindex >= 0 && sesindex < m_sessions.GetSize());

	if(sesindex < 0 || sesindex >= m_sessions.GetSize()) {
		return NET_ERR_INVALIDSESSION;
	}
	sess = (dp_session_t*)m_sessions.Get(sesindex);

	
	res = dpOpen(m_dp, sess, anet_CreateSessionCallback, this);
	if(res == dp_RES_OK) {
		return NET_ERR_OK;
	} else {
		return NET_ERR_TRANSPORTERROR;
	}
}


NET_ERR
ActivNetIO::GetMyId(uint16 & id) 
{
	id = m_pid;
	return NET_ERR_OK;
}


NET_ERR
ActivNetIO::GetHostId(uint16 & id)
{
	id = m_hostId;
	return NET_ERR_OK;
}


NET_ERR
ActivNetIO::EnumPlayers()
{
	
	
	return NET_ERR_NOTIMPLEMENTED;
}


NET_ERR
ActivNetIO::Send(uint16 id,    
				 sint32 flags, 
				 uint8* buf,   
				 sint32 len)   
{
	Assert(len >= 0 && len <0x10000);
	if(len < 0 || len >= 0x10000)
		return NET_ERR_TOO_LARGE;

	dp_result_t res;

	res = dpSend(m_dp,
				 m_pid,
				 id,
				 flags ? dp_SEND_RELIABLE : 0,
				 buf,
				 len);
	
	if(res == dp_RES_OK) {
#ifdef LOG_NETWORK_OUTPUT
		FILE *ntfile = fopen("netnetthread.log", "a");
		if(ntfile) {
			fprintf(ntfile, "%c%c ", buf[0], buf[1]);
			sint32 i;
			for(i = 0; i < len; i++) {
				fprintf(ntfile, "%02lx ", buf[i]);
			}
			fprintf(ntfile, "\n");
			fclose(ntfile);
		}
#endif
		return NET_ERR_OK;
	} else if(res == dp_RES_FULL) {
		

		
		
		
		
		
		
		

		
		
		
		
		return NET_ERR_WOULDBLOCK;
	} else {
		return NET_ERR_WRITEERR;
	}
}


NET_ERR
ActivNetIO::Idle()
{
	uint8 buf[dp_MAXLEN_UNRELIABLE + 512];
    size_t size = dp_MAXLEN_UNRELIABLE + 512;
	uint16 idFrom;
	uint16 idTo;
	dp_result_t res;
	uint32 empties = 0;

	if(!m_dp) {
		return NET_ERR_NOTSTARTED;
	}
	
	if(m_broadcastAddMessage) {
		if(time(0) - m_broadcastAddMessageTime >= 2) {
			char buf[10];
			buf[0] = 'A'; 
			buf[1] = 'M'; 
				
			dpSend(m_dp,
				   m_pid,
				   dp_ID_BROADCAST,
				   0,
				   buf,
				   2);
			m_broadcastAddMessageTime = time(0);
		}
	}
						 
	switch(m_sessionState) {
	case SESSION_STATE_CREATE_PLAYER:
		
		
		res = dpCreatePlayer(m_dp,
							 anet_PlayerReadyCallback,
							 this,
							 m_name);
		if(res != dp_RES_OK) {
			return NET_ERR_TRANSPORTERROR;
		}

		m_sessionState = SESSION_STATE_READY;
		break;
	default:
		break;
	}

	do {
		size = dp_MAXLEN_UNRELIABLE + 512;

		
		
		res = dpReceive(m_dp, &idFrom, &idTo, 0, buf, &size);

		if(res == dp_RES_HOST_NOT_RESPONDING) {
			m_response->SessionLost();
		} else {
			Assert(res == dp_RES_EMPTY || res == dp_RES_OK);
		}
		
		if(res == dp_RES_EMPTY) {
			
			
			
			empties++;
			dpFlush(m_dp);
		} else if(res == dp_RES_OK) {
			

#ifdef LOG_NETWORK_INPUT
			if(buf[0] >= 'A' && buf[0] <= 'Z') {
				FILE *ntifile = fopen("netthreadinput.log", "a");
				if(ntifile) {
					fprintf(ntifile, "%c%c ", buf[0], buf[1]);
					sint32 i;
					for(i = 0; i < size; i++) {
						fprintf(ntifile, "%02lx ", buf[i]);
					}
					fprintf(ntifile, "\n");
					fclose(ntifile);
				}
			}
#endif
			if(buf[0] == nf_PACKET_INITIALBYTE || buf[0] == dp_PACKET_INITIALBYTE) {
				dp_playerId_t pd;
				pd.id = m_pid;
				NETFunc::player.Set(&pd);
				NETFunc::Message msg(buf, size, idFrom, false);
				if(m_isHost) {
					g_gamesetup.Handle(m_dp, &msg);
				}
				if(buf[0] == nf_PACKET_INITIALBYTE)
					continue;
			}


			empties = 0;
			sint16 pkttype = dppt_MAKE(buf[0], buf[1]);
			switch(pkttype) {
				case dp_USER_ADDPLAYER_PACKET_ID:
				{
					
					dp_user_addPlayer_packet_t* addPlayer = 
						(dp_user_addPlayer_packet_t*)&buf[2];
					if(m_isHost) {
						
						uint8 buf[512];
						buf[0] = 'H';
						buf[1] = 'I';
						putshort(&buf[2], m_pid);
						res = dpSend(m_dp,
									 m_pid,
									 addPlayer->id,
									 dp_SEND_RELIABLE,
									 buf,
									 4);
						Assert(res == dp_RES_OK);
						if(res != dp_RES_OK)
							return NET_ERR_WRITEERR;
					}
					AnetPlayerData* playerData = new AnetPlayerData(addPlayer->id,
															addPlayer->name);
					m_playerList.AddTail(playerData);

					
					m_response->AddPlayer(addPlayer->id,
										   addPlayer->name);
					break;
				}
			case dp_USER_DELPLAYER_PACKET_ID:
				{
					
					dp_user_delPlayer_packet_t* delPlayer =
						(dp_user_delPlayer_packet_t*)&buf[2];

					PointerList<AnetPlayerData>::Walker walk(&m_playerList);
					while(walk.IsValid()) {
						
						AnetPlayerData* playerData = walk.GetObj();
						if(delPlayer->id == playerData->m_id) {
							walk.Remove();
							delete playerData;
						} else {
							walk.Next();
						}
					}

					
					m_response->RemovePlayer(delPlayer->id);
					break;
				}
			case dp_OBJECTDELTA_PACKET_ID:
			{
				dp_objectDelta_packet_t *pkt = (dp_objectDelta_packet_t *)&buf[2];
				if(pkt->key[0] == dp_KEY_PLAYERS) {
					if(pkt->status == dp_RES_DELETED) {
						dpid_t delId = pkt->data.p.id;
						
						PointerList<AnetPlayerData>::Walker walk(&m_playerList);
						while(walk.IsValid()) {
							
							AnetPlayerData* playerData = walk.GetObj();
							if(delId == playerData->m_id) {
								walk.Remove();
								delete playerData;
							} else {
								walk.Next();
							}
						}
						
						
						m_response->RemovePlayer(delId);
					} else if(pkt->status == dp_RES_CREATED) {
						dpid_t addId = pkt->data.p.id;
						char name[512];
						dpGetPlayerName(m_dp,
										addId,
										name,
										512);
						AnetPlayerData* playerData = new AnetPlayerData(idFrom,
																		name);
						m_playerList.AddTail(playerData);
						m_response->AddPlayer(addId, name);
					}
				}
				break;
			}
			case dp_USER_HOST_PACKET_ID:
				{
					DPRINTF(k_DBG_NET, ("ActivNet: This computer is now host\n"));
					
					uint8 buf[512];
					buf[0] = 'H';
					buf[1] = 'I';
					putshort(&buf[2], m_pid);

					m_isHost = TRUE;

					PointerList<AnetPlayerData>::Walker walk(&m_playerList);
					while(walk.IsValid()) {
						
						AnetPlayerData* playerData = walk.GetObj();
						res = dpSend(m_dp,
									 m_pid,
									 playerData->m_id,
									 0,
									 buf,
									 4);
						Assert(res == dp_RES_OK);
						if(res != dp_RES_OK) {
							return NET_ERR_WRITEERR;
						}
						walk.Next();
					}
					m_hostId = m_pid;
					m_response->SetToHost();
					break;
				}
			case dp_SESSIONLOST_PACKET_ID:
				m_response->SessionLost();
				break;
			case HOST_ID:
				m_hostId = getshort(&buf[2]);
				Assert(m_hostId != 0);
				if(m_hostId != 0) {
					m_broadcastAddMessage = FALSE;
				}
				m_response->ChangeHost(m_hostId);
				break;
			case ADDME_ID:
			{
				if(m_isHost) {
					PointerList<AnetPlayerData>::Walker walk(&m_playerList);
					BOOL add = TRUE;
					while(walk.IsValid()) {
						if(walk.GetObj()->m_id == idFrom) {
							
							
							
							add = FALSE;
						}
						walk.Next();
					}
					
					uint8 buf[512];
					buf[0] = 'H';
					buf[1] = 'I';
					putshort(&buf[2], m_pid);
					res = dpSend(m_dp,
								 m_pid,
								 idFrom,
								 dp_SEND_RELIABLE,
								 buf,
								 4);
					Assert(res == dp_RES_OK);
					if(res != dp_RES_OK)
						return NET_ERR_WRITEERR;
					if(add) {
						char name[512];
						dpGetPlayerName(m_dp,
										idFrom,
										name,
										512);
						
						
						
					
						
						

					}
				}
				break;
			}
			default:
				if((buf[0] >= 'A' && buf[0] <= 'Z') || buf[0] == '^') {
					
					
					
					m_response->PacketReady(idFrom,
											buf,
											size);
				}
				break;
			}
		} else {
			break;
		}
	} while(empties < 1);

	return NET_ERR_OK; 
}


NET_ERR
ActivNetIO::SetName(char* name)
{
	m_name = new char[strlen(name) + 1];
	strcpy(m_name, name);
	return NET_ERR_OK;
}


NET_ERR
ActivNetIO::SetLobby(char* serverName)
{
	Assert(m_dp);
	dp_result_t res;

	m_state = ANET_STATE_CONTACTING_LOBBY;
	res = dpSetGameServer(m_dp, serverName);
	Assert(res == dp_RES_OK);
	return NET_ERR_OK;
}

BOOL ActivNetIO::ReadyForData()
{
	return (m_hostId != 0) && (m_pid != 0);
}

NET_ERR ActivNetIO::SetMaxPlayers(uint16 players, bool lock)
{
	size_t size = sizeof(m_session);
	dpGetSessionDesc(m_dp, &m_session, &size);
	m_session.maxPlayers = players;
	dpSetSessionDesc(m_dp, &m_session, 0);
	dpEnableNewPlayers(m_dp, (int)lock);

	return NET_ERR_OK;
}

NET_ERR ActivNetIO::KickPlayer(uint16 player)
{
	return dpDestroyPlayer(m_dp, player) == dp_RES_OK ? NET_ERR_OK : NET_ERR_UNKNOWN;
}

NET_ERR ActivNetIO::Reset()
{
	return NET_ERR_NOTIMPLEMENTED;
}
