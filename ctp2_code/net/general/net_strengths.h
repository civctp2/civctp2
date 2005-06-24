#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_STRENGTHS_H__
#define __NET_STRENGTHS_H__

#include "net_packet.h"

class NetStrengths : public Packetizer
{
public:
	NetStrengths() {}
	NetStrengths(sint32 player);
	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	sint32 m_player;
};

class NetFullStrengths : public Packetizer
{
  public:
	NetFullStrengths() {}
	NetFullStrengths(sint32 player, sint32 startRound, sint32 endRound) { 
		m_player = player; 
		m_startRound = startRound;
		m_endRound = endRound;
	}
	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	sint32 m_startRound, m_endRound;
	sint32 m_player;
};

class NetScores : public Packetizer
{
  public:
	NetScores() {}
	NetScores(sint32 player) { m_player = player; }
	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
  private:
	sint32 m_player;
};

#endif
