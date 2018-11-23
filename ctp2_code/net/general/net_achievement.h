#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_ACHIEVEMENT_H__
#define __NET_ACHIEVEMENT_H__

#include "net_packet.h"

class NetAchievementTracker : public Packetizer
{
public:
	NetAchievementTracker() {}
	void Packetize(uint8* buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
};

#endif
