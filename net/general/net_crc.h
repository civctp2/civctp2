
#pragma once
#ifndef __NET_CRC_H__
#define __NET_CRC_H__
#include "net_packet.h"

#define k_MAX_DBS 15

class NetCRC : public Packetizer
{
public:
	NetCRC(sint32 startat, sint32 stopat);
	NetCRC();

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	sint32 SerializeDBs();
	void Error(char *buf);

	sint32 m_startAt, m_stopAt;
	uint32 m_db_crc[k_MAX_DBS][4];
};

#endif
