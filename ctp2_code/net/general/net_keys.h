#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_KEYS_H__
#define __NET_KEYS_H__

#include "net_packet.h"
class NetKeys : public Packetizer
{
public:
	NetKeys() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
};

#endif
