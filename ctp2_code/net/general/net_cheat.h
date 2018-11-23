#ifndef __NET_CHEAT_H__
#define __NET_CHEAT_H__

#include "net_packet.h"

enum NET_CHEAT {
	NET_CHEAT_CREATE_UNIT,
	NET_CHEAT_ADD_MATERIALS,
	NET_CHEAT_GLOBAL_WARMING,
	NET_CHEAT_OZONE_DEPLETION,
	NET_CHEAT_ADD_GOLD,
	NET_CHEAT_GRANT_ADVANCE,
	NET_CHEAT_GRANT_ALL,
	NET_CHEAT_CREATE_IMPROVEMENT,
	NET_CHEAT_MAX
};

class NetCheat : public Packetizer
{
public:
	NetCheat(NET_CHEAT cheat, ...);
	NetCheat() : m_data(NULL) {}
	~NetCheat();

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	NET_CHEAT m_cheat;
	uint32 *m_data;
	static const uint32 m_args[NET_CHEAT_MAX];
};

#endif
