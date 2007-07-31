#pragma once
#ifndef __NET_RESEARCH_H__
#define __NET_RESEARCH_H__

#include "net_packet.h"
class Advances;

class NetResearch : public Packetizer
{
public:
	NetResearch(Advances *adv);
	NetResearch() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
	
private:
	Advances *m_adv;
};
	
#endif
