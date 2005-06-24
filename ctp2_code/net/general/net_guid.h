#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_GUID_H__
#define __NET_GUID_H__

#include "net_packet.h"

class NetGuid : public Packetizer
{
public:
	NetGuid(const GUID *guid);
	NetGuid();

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	const GUID *m_guid;
};

class NetSetPlayerGuid : public Packetizer
{
public:
	NetSetPlayerGuid(sint32 player) {
		m_player = (uint8)player;
	}
	NetSetPlayerGuid() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	uint8 m_player;
};

#endif
