//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Network message
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
// - None
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__

#include "net_packet.h"

class NetMessage : public Packetizer
{
public:
	NetMessage(MessageData *data) { m_data = data; }
	NetMessage() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);

private:
	MessageData *m_data;
};

enum NET_MSG_TYPE {
	NET_MSG_PLAYER_JOINED,
	NET_MSG_PLAYER_LEFT,
	NET_MSG_NEW_HOST,
};

class NetInfoMessage : public Packetizer
{
public:
	NetInfoMessage(NET_MSG_TYPE msg, const MBCHAR *playerName, sint32 index);
	NetInfoMessage() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	NET_MSG_TYPE m_msg;
	const MBCHAR *m_name;
	sint32 m_player;
};

#endif
