#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_EXCLUSIONS_H__
#define __NET_EXCLUSIONS_H__

#include "net_packet.h"

class NetExclusions : public Packetizer
{
public:
	NetExclusions() {}
	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
};

#endif
