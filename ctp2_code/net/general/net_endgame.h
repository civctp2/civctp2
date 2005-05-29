#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_ENDGAME_H_
#define __NET_ENDGAME_H_

#include "net_packet.h"

class NetEndGame : public Packetizer
{
public:
	NetEndGame(sint32 owner);
	NetEndGame() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	uint8 m_owner;
};

class NetWormhole : public Packetizer
{
public:
	NetWormhole() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
};
	
#endif
