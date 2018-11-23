#ifndef _NET_CIV_H_
#define _NET_CIV_H_

#include "net_packet.h"
class CivilisationData;

class NetCivilization : public Packetizer
{
public:
	NetCivilization(CivilisationData *data);
	NetCivilization() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	CivilisationData *m_data;
};

class NetSetLeaderName : public Packetizer
{
public:
	NetSetLeaderName(sint32 player) {
		m_player = (uint8)player;
	}
	NetSetLeaderName() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	uint8 m_player;
};

#endif
