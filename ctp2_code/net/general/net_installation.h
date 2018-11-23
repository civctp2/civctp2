#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _NET_INSTALLATION_H_
#define _NET_INSTALLATION_H_

#include "net_packet.h"

class NetInstallation : public Packetizer
{
public:
	NetInstallation(InstallationData *);
	NetInstallation() {}

	void Packetize(uint8* buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	InstallationData *m_data;
};

#endif
