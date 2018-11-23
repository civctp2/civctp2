#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _NET_RAND_H_
#define _NET_RAND_H_

#include "net_packet.h"

class NetRand : public Packetizer
{
public:
	NetRand();

	void Packetize(uint8* buf, uint16& size);
	void Unpacketize(uint16 id, uint8* buf,	uint16 size);
};

#endif
