









#pragma once
#ifndef __NET_REPORT_H__
#define __NET_REPORT_H__

#include "net_packet.h"

enum NET_REPORT {
	NET_REPORT_READY_FOR_DATA,
	NET_REPORT_ACK_RESYNC,
	NET_REPORT_MAX
};

class NetReport : public Packetizer
{
public:
	NetReport(NET_REPORT type);
	NetReport() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	NET_REPORT m_type;
};

#endif

	
