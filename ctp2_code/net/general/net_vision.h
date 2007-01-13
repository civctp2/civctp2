

#pragma once
#ifndef __NET_VISION_H__
#define __NET_VISION_H__

#include "net_packet.h"


#define k_VISION_STEP 7

class UnseenCell;

class NetVision : public Packetizer
{
public:
	NetVision(sint32 owner, uint16 row, uint8 numRows);
	NetVision() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	uint8 m_owner;
	uint16 m_row;
	uint8 m_numRows;
};


class NetUnseenCell : public Packetizer
{
public:
	NetUnseenCell(UnseenCell *ucell, uint8 owner);
	NetUnseenCell() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	UnseenCell *m_ucell;
	uint8 m_owner;
};

#endif
