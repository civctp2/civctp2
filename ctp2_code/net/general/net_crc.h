//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Checks weather the databases match in MP.
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Increased k_MAX_DBS to reflect the actual number of databases to check
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_CRC_H__
#define __NET_CRC_H__
#include "net_packet.h"

#define k_MAX_DBS 40

class NetCRC : public Packetizer
{
public:
	NetCRC(sint32 startat, sint32 stopat);
	NetCRC();

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	sint32 SerializeDBs();
	void Error(const char *buf);

	sint32 m_startAt, m_stopAt;
	uint32 m_db_crc[k_MAX_DBS][4];
};

#endif
