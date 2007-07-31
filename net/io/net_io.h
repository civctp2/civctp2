





#pragma once
#ifndef _NETIO_H_
#define _NETIO_H_

enum NET_ERR;

class NetIOResponse
{
public:
	virtual void EnumTransport(NET_ERR result, 
								sint32 index,
								const char* transname, 
								void* transdata) = 0;
	virtual void EnumSession(NET_ERR result,
							  sint32 index,
							  const char* sessionName,
							  void* sessionData) = 0;
	virtual void SessionReady(NET_ERR result,
							   void* sessionData) = 0;
	virtual void PacketReady(sint32 from,
							  uint8* buf,
							  sint32 size) = 0;
	virtual void AddPlayer(uint16 id,
							char* name) = 0;
	virtual void RemovePlayer(uint16 id) = 0;
	virtual void SetToHost() = 0;
	virtual void ChangeHost(uint16 id) = 0;
	virtual void SessionLost() = 0;
	virtual bool ReadyForPackets() = 0;
};

class NetIO 
{
public:
	virtual NET_ERR Init(NetIOResponse* response);
	virtual NET_ERR EnumTransports();
	virtual NET_ERR EnumSessions();
	virtual NET_ERR SetTransport(sint32 idx);
	virtual NET_ERR Host(char* sessionName);
	virtual NET_ERR Join(sint32 index);
	virtual NET_ERR GetMyId(uint16& id);
	virtual NET_ERR GetHostId(uint16& id);
	virtual NET_ERR EnumPlayers();
	virtual NET_ERR Send(uint16 id, sint32 flags, uint8* buf, sint32 len);
	virtual NET_ERR SendCompressed(uint16 id, sint32 flags, uint8* buf, sint32 len);
	virtual NET_ERR Idle() = 0;
	virtual NET_ERR SetName(char*);
	virtual NET_ERR SetLobby(char*);
	virtual BOOL ReadyForData() = 0;
	virtual NET_ERR SetMaxPlayers(uint16 players, bool lock) = 0;
	virtual NET_ERR KickPlayer(uint16 player) = 0;
	virtual NET_ERR Reset() = 0;
	virtual ~NetIO();
protected:
	NetIOResponse* m_response;
};

#endif
