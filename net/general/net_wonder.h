#pragma once
#ifndef __NET_WONDER_H__
#define __NET_WONDER_H__

#include "net_packet.h"

class NetWonderTracker : public Packetizer
{
public:
	NetWonderTracker() {}
	void Packetize(uint8* buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
};

#endif
