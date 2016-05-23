#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _NET_PLAYER_H_
#define _NET_PLAYER_H_

class NetPlayer;
class NetAddPlayer;

#include "net_packet.h"     // Packetizer
class Player;

class NetPlayer : public Packetizer
{
public:
	NetPlayer(Player* player);
	NetPlayer() {}

	void Packetize(uint8* buf, uint16& size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);
private:
	Player* m_player;
};

class NetAddPlayer : public Packetizer
{
public:
	NetAddPlayer(uint16 id, char *name) {
		m_id = id;
		m_name = name;
	}

	NetAddPlayer() {}
	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	uint16 m_id;
	char *m_name;
};

#endif
