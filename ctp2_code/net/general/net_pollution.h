
#ifndef _NET_POLLUTION_H_
#define _NET_POLLUTION_H_

#include "net_packet.h"

class NetPollution : public Packetizer
{
public:
	NetPollution() {}

	void Packetize(uint8* buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
};

#endif
