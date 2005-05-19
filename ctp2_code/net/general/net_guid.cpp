

#include "c3.h"
#include "net_guid.h"
#include "net_util.h"
#include "network.h"
#include "player.h"

extern Player **g_player;

NetGuid::NetGuid(const GUID *guid)
{
	m_guid = guid;
}

NetGuid::NetGuid()
{
}

void NetGuid::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_GUID_ID);
	memcpy(&buf[size], (uint8*)m_guid, sizeof(*m_guid));
	size += sizeof(*m_guid);
}

void NetGuid::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_GUID_ID);

	GUID guid;
	memcpy((uint8*)&guid, &buf[pos], sizeof(GUID));
	pos += sizeof(GUID);
	g_network.SetGuid(id, &guid);
	Assert(pos == size);
}

void NetSetPlayerGuid::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_SET_PLAYER_GUID_ID);
	PUSHBYTE(m_player);
	memcpy(&buf[size], (uint8*)&g_player[m_player]->m_networkGuid, sizeof(GUID));
	size += sizeof(GUID);
}

void NetSetPlayerGuid::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_SET_PLAYER_GUID_ID);

	PULLBYTE(m_player);

	GUID guid;
	memcpy((uint8*)&guid, &buf[pos], sizeof(GUID));
	pos += sizeof(GUID);
	if(g_player[m_player]) {
		g_player[m_player]->m_networkGuid = guid;
	}
	Assert(pos == size);
}
