








#include "c3.h"
#include "net_chat.h"
#include "net_util.h"
#include "network.h"
#include "Player.h"

extern Player **g_player;

NetChat::NetChat(uint32 destmask, MBCHAR *str, sint16 len)
{
	m_destmask = destmask;
	m_str = new MBCHAR[len+1];
	memcpy(m_str, str, len * sizeof(MBCHAR));
	m_str[len] = 0;
	m_len = len;
	m_from = (uint8)g_network.GetPlayerIndex();
}

NetChat::~NetChat()
{
	if(m_str)
		delete [] m_str;
}

void 
NetChat::Packetize(uint8 *buf, uint16 &size)
{
	buf[0] = k_PACKET_CHAT_ID >> 8;
	buf[1] = k_PACKET_CHAT_ID & 0xff;

	size = 2;
	PUSHLONG(m_destmask);
	PUSHSHORT(m_len);
	PUSHBYTE(m_from);
	memcpy(&buf[size], m_str, m_len * sizeof(MBCHAR));
	size += m_len * sizeof(MBCHAR);
}

void 
NetChat::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	Assert(MAKE_CIV3_ID(buf[0], buf[1]) == k_PACKET_CHAT_ID);
	
	sint32 pos = 2;
	PULLLONG(m_destmask);
	PULLSHORT(m_len);
	PULLBYTE(m_from);
	if(g_network.IsHost()) {
		Assert(m_from == g_network.IdToIndex(id));
		m_from = (uint8)g_network.IdToIndex(id);
	}
	if(m_str)
		delete [] m_str;

	m_str = new MBCHAR[m_len + 1];
	memcpy(m_str, &buf[pos], m_len);
	m_str[m_len] = 0;
	pos += m_len * sizeof(MBCHAR);

	if(g_network.IsHost()) {
		for(sint32 p = 0; p < k_MAX_PLAYERS; p++) {
			if(!g_player[p]) continue;

			if(m_destmask & (1 << p) && p != g_network.GetPlayerIndex() &&
			   p != g_network.IdToIndex(id) && 
			   g_player[p]->m_playerType == PLAYER_TYPE_NETWORK) {
				g_network.QueuePacket(g_network.IndexToId(p), this);
			}
		}
	}
	if(m_destmask & ((uint32)1 << (uint32)g_network.GetPlayerIndex())) {
		if(m_destmask == ((uint32)1 << (uint32)g_network.GetPlayerIndex())) {
			g_network.AddChatText(m_str, (sint32)m_len, m_from, TRUE);
		} else {
			g_network.AddChatText(m_str, (sint32)m_len, m_from, FALSE);
		}
	}
}
