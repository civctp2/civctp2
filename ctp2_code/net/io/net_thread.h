#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_THREAD_H__
#define __NET_THREAD_H__

#include "net_io.h"
#include "anet.h"
#include "net_array.h"
#include "pointerlist.h"
#include "net_anet.h"
#ifdef USE_SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#endif

#define k_MAX_NETWORK_PLAYERS 32

#define k_RPC_ID 0

#define k_RPC_SESSION_READY 1
#define k_RPC_ADD_PLAYER 2
#define k_RPC_REMOVE_PLAYER 3
#define k_RPC_SET_TO_HOST 4
#define k_RPC_CHANGE_HOST 5
#define k_RPC_SESSION_LOST 6

#define k_SPLIT_PACKET_HEAD  'Z'
#define k_SPLIT_PACKET_START 's'
#define k_SPLIT_PACKET_BODY  'b'
#define k_SPLIT_PACKET_END   'e'
#define k_COMPRESSED_PACKET  '^'

#define k_SPLIT_LEN 238

template <class T> class SimpleDynamicArray;

class TPacketData
{
private:
	sint32 m_totalLen;

public:
	TPacketData(uint16 id, sint32 flags, uint8 *buf, sint32 len, BOOL sendPacket);
	TPacketData(uint16 id, sint32 flags, uint8 *buf, sint32 len,
				uint8 *header, uint16 headerlen);

	~TPacketData();

	void Append(uint8 *buf, sint32 len);
	void RemoveSplitInfo();

	uint16 m_id;
	sint32 m_flags;
	uint8  *m_buf;
	uint8 *m_actualBuf;
	sint32 m_len;
};

class NetThread : public NetIO, public NetIOResponse
{
private:
	ActivNetIO *m_anet;
	PointerList<TPacketData> *m_outgoing[k_MAX_NETWORK_PLAYERS];
	uint16 m_ids[k_MAX_NETWORK_PLAYERS];
	PointerList<TPacketData> *m_incoming;
#ifndef USE_SDL
	DWORD m_threadId;
	HANDLE m_threadHandle;
	CRITICAL_SECTION m_mutex;
#else
	Uint32 m_threadId;
	SDL_Thread *m_thread;
	SDL_mutex *m_mutex;
#endif
	dp_t *m_dp;
	dp_t *m_origDP;
	volatile BOOL m_exit, m_exited;
	volatile sint32 m_setMaxPlayers;
	volatile bool m_setLock;
	SimpleDynamicArray<uint16> *m_kickPlayers;
	void Lock();
	void Unlock();

public:
	NetThread();
	virtual ~NetThread();

	void Run();

	void SetDP(dp_t *dp);




	NET_ERR Init(NetIOResponse *response);
	NET_ERR EnumTransports();
	NET_ERR SetTransport(sint32 trans_id);
	NET_ERR Host(char* sessionName);
	NET_ERR EnumSessions();
	NET_ERR Join(sint32 index);
	NET_ERR GetMyId(uint16& id);
	NET_ERR GetHostId(uint16& id);
	NET_ERR EnumPlayers();
	NET_ERR Send(uint16 id, sint32 flags, uint8* buf, sint32 len);
	NET_ERR SendCompressed(uint16 id, sint32 flags, uint8* buf, sint32 len);
	NET_ERR Idle();
	NET_ERR SetName(char* name);
	NET_ERR SetLobby(char* serverName);
	NET_ERR SetMaxPlayers(uint16 players, bool lock);
	NET_ERR KickPlayer(uint16 player);
	NET_ERR Reset();
	BOOL    ReadyForData();

	TPacketData *FindSplitStart(uint16 from);




	void EnumTransport(NET_ERR result,
					   sint32 index,
					   const char* transname,
					   void* transdata);
	void EnumSession(NET_ERR result,
					 sint32 index,
					 const char* sessionName,
					 void* sessionData);
	void SessionReady(NET_ERR result,
					  void* sessionData);
	void PacketReady(sint32 from,
					 uint8* buf,
					 sint32 size);
	void AddPlayer(uint16 id,
				   char* name);
	void RemovePlayer(uint16 id);
	void SetToHost();
	void ChangeHost(uint16 id);
	void SessionLost();


	bool ReadyForPackets() { return true; }
};

#endif
