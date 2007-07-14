#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_SHINY_HAPPY_CODE_H__
#define __NET_SHINY_HAPPY_CODE_H__

#include "net_packet.h"
#include "Unit.h"

class Happy;
class PlayerHappiness;

class NetHappy : public Packetizer
{
public:
	NetHappy(Unit city, Happy *, BOOL isInitial = FALSE);
	NetHappy() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
	
private:
	Unit m_city;
	Happy *m_data;
	uint8 m_isInitialPacket;
};

class NetPlayerHappy : public Packetizer
{
public:
	NetPlayerHappy(uint8 owner, PlayerHappiness *hap,
				   uint8 isInitialPacket);
	NetPlayerHappy() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	uint8 m_owner;
	PlayerHappiness *m_playerHappiness;
	uint8 m_isInitialPacket;
};

#endif
