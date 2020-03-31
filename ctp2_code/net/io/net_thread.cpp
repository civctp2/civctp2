//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Network receiver thread
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
// _DEBUG
// Generate extra debug information when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Repaired memory leaks.
// - Display the main thread function name in the debugger.
// - Initialized local variables. (Sep 9th 2005 Martin G?hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include <chrono>
#include <thread>

#include "net_thread.h"
#include "net_types.h"
#include "net_util.h"
#include "SimpleDynArr.h"
#include "zlib.h"

#if defined(_DEBUG)
#include "debug.h"  // Os::SetThreadName
#endif

TPacketData::TPacketData(uint16 id, sint32 flags, uint8 *buf, sint32 len,
						 BOOL sendPacket)
{
	m_id = id;
	m_flags = flags;

	m_actualBuf = NULL;
	m_totalLen = 0;

	if(!sendPacket &&
	   buf[0] == k_SPLIT_PACKET_HEAD &&
	   buf[1] == k_SPLIT_PACKET_START) {
		sint32 realLen = getlong(&buf[2]);
		m_totalLen = realLen;
		m_buf = new uint8[m_totalLen + 6];
		memcpy(m_buf, buf, len);
		len -= 6;
	} else if(buf) {
		m_buf = new uint8[len];
		memcpy(m_buf, buf, len);
	} else {
		m_buf = NULL;
	}
	m_len = len;
}

TPacketData::TPacketData(uint16 id, sint32 flags, uint8 *buf,sint32 len,
						 uint8 *header, uint16 headerLen)
{
	m_id = id;
	m_flags = flags;
	m_actualBuf = NULL;
	m_buf = new uint8[len + headerLen];
	memcpy(m_buf, header, headerLen);
	memcpy(m_buf + headerLen, buf, len);
	m_len = len + headerLen;
}

TPacketData::~TPacketData()
{
	delete [] m_actualBuf;
	delete [] m_buf;
}

void TPacketData::Append(uint8 *buf, sint32 len)
{
	Assert(m_buf[0] == k_SPLIT_PACKET_HEAD);
	if(m_buf[0] == k_SPLIT_PACKET_HEAD) {
		Assert(m_len + len <= m_totalLen);
		memcpy(&m_buf[m_len + 6], buf, len);
		m_len += len;
	}
}

void TPacketData::RemoveSplitInfo()
{
	Assert(m_buf[0] == k_SPLIT_PACKET_HEAD);
	Assert(m_len == m_totalLen);
	Assert(m_totalLen > 0);
	m_actualBuf = m_buf;
	m_buf = m_buf + 6;
}

NET_ERR NetThread::Reset()
{

	m_exit = TRUE;
	while(!m_exited)
		;

	m_exit = m_exited = FALSE;

	if(m_origDP) {
		dpSetActiveThread(m_origDP);
	}

	m_incoming->DeleteAll();
	sint32 i;
	for(i = 0; i < k_MAX_NETWORK_PLAYERS; i++) {
		if(m_outgoing[i]) {
			delete m_outgoing[i];
			m_outgoing[i] = NULL;
		}
		m_ids[i] = 0;
	}

	m_dp = NULL;
	m_origDP = NULL;
	m_setMaxPlayers = -1;

	Init(m_response);

	return NET_ERR_OK;
}

NetThread::~NetThread()
{
	m_exit = TRUE;
	while(!m_exited)
		;
	if(m_origDP) {
		dpSetActiveThread(m_origDP);
	}
	m_incoming->DeleteAll();
	delete m_incoming;
	sint32 i;
	for(i = 0; i < k_MAX_NETWORK_PLAYERS; i++) {
		if(m_outgoing[i]) {
			delete m_outgoing[i];
			m_outgoing[i] = NULL;
		}
	}
	if(m_kickPlayers) {
		delete m_kickPlayers;
	}
}

#ifdef USE_SDL
static int NetThread_StartThread(void *obj)
#elif defined(WIN32)
DWORD WINAPI NetThread_StartThread(LPVOID obj)
#else
#error "Threading not implemented."
#endif
{
	NetThread *threadObj = (NetThread*)obj;
	threadObj->Run();
	return 0;
}

NetThread::NetThread()
{
	m_dp = NULL;
	m_origDP = NULL;
	sint32 i;
	for(i = 0; i < k_MAX_NETWORK_PLAYERS; i++) {
		m_outgoing[i] = NULL;
		m_ids[i] = 0;
	}
	m_incoming = new PointerList<TPacketData>;
	m_anet = NULL;
	m_exit = m_exited = FALSE;

#ifdef USE_SDL
	m_mutex = SDL_CreateMutex();
#elif defined(WIN32)
	InitializeCriticalSection(&m_mutex);
#endif
	m_setMaxPlayers = -1;
	m_kickPlayers = new SimpleDynamicArray<uint16>;
}

NET_ERR NetThread::Init(NetIOResponse *response)
{
	NetIO::Init(response);

#ifdef USE_SDL
	if ((m_thread = SDL_CreateThread(NetThread_StartThread, "Net-start-thread", this)) == NULL) {
#elif defined(WIN32)
	if((m_threadHandle = CreateThread(NULL,
									  0,
									  NetThread_StartThread,
									  this,
									  0,
									  &m_threadId)) == NULL) {
#endif
		DPRINTF(k_DBG_NET, ("Failed to start thread\n"));
	}
	return NET_ERR_OK;
}

void NetThread::Run()
{
	TPacketData *packet;

#if defined(_DEBUG) && defined(WIN32)
	Os::SetThreadName("NetThread::Run");
#endif

	while(!m_exit) {
//		std::this_thread::sleep_for(std::chrono::microseconds(100));
#ifdef WIN32
		Sleep(100);
#else
		usleep(100 * 1000);
#endif
		if(m_anet) {
			if(m_dp) {

				dpSetActiveThread(m_dp);
				m_anet->SetDP(m_dp);
				m_origDP = m_dp;
				m_dp = NULL;

			}

			if(m_setMaxPlayers >= 0) {
				m_anet->SetMaxPlayers((uint16)m_setMaxPlayers, m_setLock);
				m_setMaxPlayers = -1;
			}

			Lock();
			while(m_kickPlayers->Num() > 0) {
				m_anet->KickPlayer(m_kickPlayers->Access(0));
				m_kickPlayers->DelIndex(0);
			}
			Unlock();

			sint32 p;
			for(p = 0; p < k_MAX_NETWORK_PLAYERS; p++) {
				Lock();
				PointerList<TPacketData> *outgoing = m_outgoing[p];
				if(!outgoing) {
					Unlock();
					continue;
				}

				if(outgoing->GetCount() > 0) {
					BOOL busy = FALSE;
					while(!busy && ((packet = outgoing->RemoveHead()) != NULL)) {
						static uint8 buf[dp_MAXLEN_UNRELIABLE * 2];
						memcpy(buf, packet->m_buf, packet->m_len);

						NET_ERR err = m_anet->Send(packet->m_id,
												   packet->m_flags,
												   buf,
												   packet->m_len);
						switch(err) {
							case NET_ERR_OK:
								delete packet;
								break;
							case NET_ERR_WOULDBLOCK:
								busy = TRUE;
								outgoing->AddHead(packet);
								break;
							default:

								break;
						}
					}
				}
				Unlock();
			}
			m_anet->Idle();
		}
	}
	if(m_anet) {
		uint16 myId;
		m_anet->GetMyId(myId);
		m_anet->KickPlayer(myId);
		m_anet->SetDP(NULL);
		delete m_anet;
		m_anet = NULL;
	}
	m_exited = TRUE;
}

void NetThread::Lock()
{
#ifdef USE_SDL
	SDL_mutexP(m_mutex);
#else
	EnterCriticalSection(&m_mutex);
#endif
}

void NetThread::Unlock()
{
#ifdef USE_SDL
	SDL_mutexV(m_mutex);
#else
	LeaveCriticalSection(&m_mutex);
#endif
}

void NetThread::SetDP(dp_t *dp)
{
	Lock();
	m_anet = new ActivNetIO;
	m_anet->Init(this);
	m_dp = dp;
	Unlock();
}

NET_ERR NetThread::EnumTransports()
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR NetThread::SetTransport(sint32 trans_id)
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR NetThread::Host(char* sessionName)
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR NetThread::EnumSessions()
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR NetThread::Join(sint32 index)
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR NetThread::GetMyId(uint16& id)
{
	Assert(m_anet);

	return m_anet->GetMyId(id);
}

NET_ERR NetThread::GetHostId(uint16& id)
{
	Assert(m_anet);
	return m_anet->GetHostId(id);
}

NET_ERR NetThread::EnumPlayers()
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR NetThread::Send(uint16 id, sint32 flags, uint8* buf, sint32 len)
{
	Lock();

	sint32 i, idx = 0;
	for(i = 0; i < k_MAX_NETWORK_PLAYERS; i++) {
		if(m_ids[i] == id) {
			idx = i;
			break;
		}
	}
	Assert(i < k_MAX_NETWORK_PLAYERS);
	if(i >= k_MAX_NETWORK_PLAYERS) {
		Unlock();
		return NET_ERR_INVALIDADDR;
	}

	PointerList<TPacketData> *outgoing = m_outgoing[idx];
	Assert(outgoing);
	if(!outgoing) {
		Unlock();
		return NET_ERR_UNKNOWN;
	}

	if(len >= k_SPLIT_LEN) {
		sint32 added = 0;
		uint8 header[6];
		header[0] = k_SPLIT_PACKET_HEAD;
		header[1] = k_SPLIT_PACKET_START;
		putlong(&header[2], len);
		outgoing->AddTail(new TPacketData(id, flags, buf, k_SPLIT_LEN - 1,
											header, 6));
		added = k_SPLIT_LEN - 1;
		while(added < len) {
			sint32 left = len - added;
			sint32 toSend;
			header[0] = k_SPLIT_PACKET_HEAD;
			if(left < k_SPLIT_LEN) {
				header[1] = k_SPLIT_PACKET_END;
				toSend = left;
			} else {
				header[1] = k_SPLIT_PACKET_BODY;
				toSend = k_SPLIT_LEN - 1;
			}

			outgoing->AddTail(new TPacketData(id, flags, buf + added, toSend,
												header, 2));
			added += toSend;
		}
		Assert(added == len);
	} else {
		outgoing->AddTail(new TPacketData(id, flags, buf, len, TRUE));
	}

	Unlock();
	return NET_ERR_OK;
}

NET_ERR NetThread::SendCompressed(uint16 id, sint32 flags, uint8 *buf, sint32 len)
{
	uLongf  cbufsize = (uLongf)(((double)len * 1.01) + 12.5);
	uint8 * cbuf    = new uint8[cbufsize + 5];

	cbuf[0] = k_COMPRESSED_PACKET;
	int err;
	err = compress2(&cbuf[5], &cbufsize, buf, len, Z_DEFAULT_COMPRESSION);
	if(err == Z_OK) {
		putlong(&cbuf[1], len);
		return Send(id, flags, cbuf, cbufsize + 5);
	} else {
		delete [] cbuf;
		return NET_ERR_UNKNOWN;
	}
}

NET_ERR NetThread::Idle()
{
	sint32 hackPackets;
	Lock();
	if(m_incoming->GetCount() <= 0) {
		Unlock();
		return NET_ERR_OK;
	}

	TPacketData *packet;
	BOOL stopProcessing = FALSE;
	hackPackets = 0;
	while(!stopProcessing && (m_response->ReadyForPackets()) &&
		  ((m_incoming->GetHead() != NULL))) {
		packet = m_incoming->RemoveHead();

		if(packet->m_buf && packet->m_buf[0] == k_SPLIT_PACKET_HEAD &&
			packet->m_id != k_RPC_ID) {

			m_incoming->AddHead(packet);
			stopProcessing = TRUE;
			continue;
		}

		Unlock();
		if(packet->m_id == k_RPC_ID) {
			switch(packet->m_flags) {
				case k_RPC_SESSION_READY:
					m_response->SessionReady(NET_ERR_OK, NULL);
					break;
				case k_RPC_ADD_PLAYER:
				{
					m_response->AddPlayer(getshort(&packet->m_buf[packet->m_len - 2]),
										  (char*)packet->m_buf);

					stopProcessing = TRUE;
					break;
				}
				case k_RPC_REMOVE_PLAYER:
					m_response->RemovePlayer(getshort(packet->m_buf));
					break;
				case k_RPC_SET_TO_HOST:
					m_response->SetToHost();
					break;
				case k_RPC_CHANGE_HOST:
					m_response->ChangeHost(getshort(packet->m_buf));
					break;
				case k_RPC_SESSION_LOST:
					m_response->SessionLost();
					break;
			}
		} else {
			if(packet->m_buf[0] == k_COMPRESSED_PACKET) {
				uLongf uSize = getlong(&packet->m_buf[1]);

				uint8 *uBuf = new uint8[uSize];
				int err;
				err = uncompress(uBuf, &uSize, &packet->m_buf[5], packet->m_len - 5);
				Assert(err == Z_OK);
				if(err == Z_OK) {
					m_response->PacketReady(packet->m_id, uBuf, uSize);
				}
				delete [] uBuf;
			} else {
				m_response->PacketReady(packet->m_id, packet->m_buf, packet->m_len);
			}
		}
		delete packet;
		Lock();
		hackPackets++;
		if(hackPackets >= 1000)
			break;
	}
	Unlock();
	return NET_ERR_OK;
}

NET_ERR NetThread::SetName(char* name)
{
	return NET_ERR_NOTIMPLEMENTED;
}

NET_ERR NetThread::SetLobby(char* serverName)
{
	return NET_ERR_NOTIMPLEMENTED;
}

BOOL NetThread::ReadyForData()
{
	Lock();
	BOOL const	ready = m_anet ? m_anet->ReadyForData() : false;
	Unlock();
	return ready;
}

void NetThread::EnumTransport(NET_ERR result,
				   sint32 index,
				   const char* transname,
				   void* transdata)
{

}

void NetThread::EnumSession(NET_ERR result,
				 sint32 index,
				 const char* sessionName,
				 void* sessionData)
{

}
void NetThread::SessionReady(NET_ERR result,
				  void* sessionData)
{
	Lock();
	m_incoming->AddTail(new TPacketData(k_RPC_ID,
										k_RPC_SESSION_READY,
										NULL,
										0,
										TRUE));
	Unlock();
}

TPacketData *NetThread::FindSplitStart(uint16 from)
{
	PointerList<TPacketData>::Walker walk(m_incoming);
	for(; walk.IsValid(); walk.Next()) {
		if(walk.GetObj()->m_id == from && walk.GetObj()->m_buf[0] == k_SPLIT_PACKET_HEAD) {
			Assert(walk.GetObj()->m_buf[1] == k_SPLIT_PACKET_START);
			return walk.GetObj();
		}
	}
	return NULL;
}

void NetThread::PacketReady(sint32 from, uint8* buf, sint32 size)
{
	Lock();
#if 0
	TPacketData *currentTail = m_incoming->GetTail();
#endif
	if(buf[0] == k_SPLIT_PACKET_HEAD && m_incoming->GetCount() > 0) {

		TPacketData *splitStart = FindSplitStart((uint16)from);
		if(!splitStart) {
			Assert(buf[1] == k_SPLIT_PACKET_START);
			m_incoming->AddTail(new TPacketData((uint16)from, 0, buf, size, FALSE));
		} else {
			Assert(buf[1] != k_SPLIT_PACKET_START);
			splitStart->Append(&buf[2], size - 2);
			switch(buf[1]) {
				case k_SPLIT_PACKET_BODY:
					break;
				case k_SPLIT_PACKET_END:
					splitStart->RemoveSplitInfo();
					break;
				default:
					Assert(FALSE);
					break;
			}
		}
#if 0
		if((currentTail->m_buf &&
			(currentTail->m_buf[0] != k_SPLIT_PACKET_HEAD))) {
			Assert(buf[1] == k_SPLIT_PACKET_START);
			m_incoming->AddTail(new TPacketData((uint16)from, 0, buf, size,
												FALSE));
		} else {
			currentTail->Append(&buf[2], size - 2);
			switch(buf[1]) {
				case k_SPLIT_PACKET_BODY:
					break;
				case k_SPLIT_PACKET_END:
					currentTail->RemoveSplitInfo();
					break;
				default:
					Assert(FALSE);
					break;
			}
		}
#endif

	} else {
#ifdef _DEBUG
		TPacketData *splitStart = FindSplitStart((uint16)from);
		Assert(!splitStart);
#endif







		m_incoming->AddTail(new TPacketData((uint16)from, 0, buf, size, FALSE));
	}
	Unlock();
}

void NetThread::AddPlayer(uint16 id, char* name)
{
	Lock();
	DPRINTF(k_DBG_NET, ("AddPlayer(%d, %s)\n", id, name));

	TPacketData *packet = new TPacketData(k_RPC_ID,
										  k_RPC_ADD_PLAYER,
										  (uint8*)name,
										  strlen(name) + 1 + sizeof(uint16),
										  TRUE);
	putshort(&packet->m_buf[strlen(name) + 1], id);
	m_incoming->AddTail(packet);

	sint32 i;
	for(i = 0; i < k_MAX_NETWORK_PLAYERS; i++) {
		if(m_ids[i] == 0) {
			m_ids[i] = id;
			m_outgoing[i] = new PointerList<TPacketData>;
			break;
		}
	}
	Assert(i < k_MAX_NETWORK_PLAYERS);
	Unlock();
}

void NetThread::RemovePlayer(uint16 id)
{
	Lock();
	TPacketData *packet = new TPacketData(k_RPC_ID,
										  k_RPC_REMOVE_PLAYER,
										  NULL,
										  sizeof(uint16),
										  TRUE);
	packet->m_buf = new uint8[sizeof(uint16)];
	putshort(packet->m_buf, id);
	m_incoming->AddTail(packet);

	sint32 i;
	for(i = 0; i < k_MAX_NETWORK_PLAYERS; i++) {
		if(m_ids[i] == id) {
			m_ids[i] = 0;
			if(m_outgoing[i]) {
				delete m_outgoing[i];
				m_outgoing[i] = NULL;
				break;
			}
		}
	}


	PointerList<TPacketData>::Walker walk(m_incoming);
	while(walk.IsValid()) {
		if(walk.GetObj()->m_id == id) {
			delete walk.Remove();
		} else {
			walk.Next();
		}
	}

	Assert(i < k_MAX_NETWORK_PLAYERS);
	Unlock();
}

void NetThread::SetToHost()
{
	Lock();
	m_incoming->AddTail(new TPacketData(k_RPC_ID,
										k_RPC_SET_TO_HOST,
										NULL,
										0,
										TRUE));
	Unlock();
}

void NetThread::ChangeHost(uint16 id)
{
	Lock();
	TPacketData *packet = new TPacketData(k_RPC_ID,
										  k_RPC_CHANGE_HOST,
										  (uint8 *)&id,
										  sizeof(uint16),
										  TRUE);
	m_incoming->AddTail(packet);
	Unlock();
}

NET_ERR NetThread::SetMaxPlayers(uint16 players, bool lock)
{
	m_setMaxPlayers = players;
	m_setLock = lock;
	return NET_ERR_OK;
}

NET_ERR NetThread::KickPlayer(uint16 player)
{
	Lock();
	m_kickPlayers->Insert(player);
	Unlock();
	return NET_ERR_OK;
}

void NetThread::SessionLost()
{
	Lock();
	TPacketData *packet = new TPacketData(k_RPC_ID,
										  k_RPC_SESSION_LOST,
										  NULL,
										  0,
										  TRUE);


	m_incoming->DeleteAll();

	m_incoming->AddTail(packet);
	Unlock();
}
