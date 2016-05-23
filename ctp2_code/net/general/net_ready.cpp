#include "c3.h"
#include "Readiness.h"
#include "net_ready.h"
#include "player.h"
#include "net_util.h"

extern Player **g_player;

NetReadiness::NetReadiness(MilitaryReadiness *data)
{
	m_data = data;
}

void NetReadiness::Packetize(uint8 *buf, uint16 &size)
{
	buf[0] = k_PACKET_READINESS_ID >> 8;
	buf[1] = k_PACKET_READINESS_ID & 0xff;

	size = 2;
	PUSHBYTE((uint8)m_data->m_owner);
	PUSHBYTE(m_data->m_readinessLevel);
	PUSHDOUBLE(m_data->m_delta);
	PUSHDOUBLE(m_data->m_hp_modifier);
	PUSHDOUBLE(m_data->m_cost);
	PUSHBYTE(m_data->m_ignore_unsupport);
	PUSHDOUBLE(m_data->m_percent_last_turn);

}

void NetReadiness::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	Assert(MAKE_CIV3_ID(buf[0], buf[1]) == k_PACKET_READINESS_ID);

	sint32 pos = 2;

	uint8 owner;
	PULLBYTE(owner);

	if(!g_player[owner])
		return;

	MilitaryReadiness *readiness = g_player[owner]->m_readiness;

	uint8 level;
	PULLBYTE(level);
	readiness->m_readinessLevel = (READINESS_LEVEL)level;
	PULLDOUBLE(readiness->m_delta);
	PULLDOUBLE(readiness->m_hp_modifier);
	PULLDOUBLE(readiness->m_cost);
	PULLBYTE(readiness->m_ignore_unsupport);
	PULLDOUBLE(readiness->m_percent_last_turn);
}
