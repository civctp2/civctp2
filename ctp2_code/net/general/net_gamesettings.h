#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_GAMESETTINGS_H__
#define __NET_GAMESETTINGS_H__

#include "net_packet.h"

class NetGameSettings : public Packetizer
{
public:
	NetGameSettings(sint32 x, sint32 y,
					sint32 numPlayers,
					uint8 gameStyle,
					sint32 movesPerSlice,
					time_t totalTime,
					time_t turnTime,
					time_t cityTime);
	NetGameSettings() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);
private:
	sint32 m_x, m_y;
	sint32 m_numPlayers;
	uint8  m_gameStyle;
	sint32 m_movesPerSlice;
	time_t m_totalTime;
	time_t m_turnTime;
	time_t m_cityTime;
};

#endif
