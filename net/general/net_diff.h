








#pragma once
#ifndef _NET_DIFF_H_
#define _NET_DIFF_H_

#include "net_packet.h"

class Difficulty;

class NetDifficulty : public Packetizer
{
public:
	NetDifficulty(Difficulty* diff);
	NetDifficulty() {}

	void Packetize(uint8* buf, uint16& size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);

private:
	Difficulty* m_difficulty;
};

#else
class NetDifficulty;
#endif
