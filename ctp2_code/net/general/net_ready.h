









#pragma once
#ifndef _NET_READY_H_
#define _NET_READY_H_

#include "net_packet.h"

class MilitaryReadiness;

class NetReadiness : public Packetizer {
public:
	NetReadiness(MilitaryReadiness *);
	NetReadiness() {}

	void Packetize(uint8* buf, uint16& size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);

private:
	MilitaryReadiness * m_data;
};

#endif
