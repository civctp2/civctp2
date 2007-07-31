
#ifndef NET_WORLD_H__
#define NET_WORLD_H__

#include "net_packet.h"

class NetWorld : public Packetizer
{
  public:
	NetWorld() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
};

#endif
