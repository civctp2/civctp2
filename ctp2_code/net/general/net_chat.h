








#pragma once
#ifndef _NET_CHAT_H_
#define _NET_CHAT_H_

#include "net_packet.h"
#include "net_const.h"

class NetChat : public Packetizer
{
public:
	NetChat(uint32 dest, MBCHAR *str, sint16 len);
	NetChat() { m_str = NULL; m_len = 0; m_destmask = 0; }
	~NetChat();

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	uint32 m_destmask;
	MBCHAR *m_str;
	sint16 m_len;
	uint8 m_from;
};
	
#endif
