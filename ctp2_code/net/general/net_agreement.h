









#pragma once
#ifndef _NET_AGREEMENT_H_
#define _NET_AGREEMENT_H_

#include "net_packet.h"

class AgreementData;

class NetAgreement : public Packetizer
{
public:
	NetAgreement(AgreementData *);
	NetAgreement() {}

	static void PacketizeData(AgreementData *data, uint8 *buf, uint16 &size);
	static void UnpacketizeData(AgreementData *data,
								uint8 *buf, sint32 &pos, uint16 size);

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	AgreementData *m_data;
};

class NetClientAgreement : public Packetizer
{
public:
	NetClientAgreement(AgreementData *);
	NetClientAgreement() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	AgreementData *m_data;
};

#endif
