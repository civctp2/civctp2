#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _NET_TERRAIN_H_
#define _NET_TERRAIN_H_

#include "net_packet.h"

class TerrainImprovementData;

class NetTerrainImprovement : public Packetizer
{
public:
	NetTerrainImprovement(TerrainImprovementData *);
	NetTerrainImprovement() {}

	void Packetize(uint8* buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	TerrainImprovementData *m_data;
};

#endif
