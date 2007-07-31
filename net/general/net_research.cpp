

#include "c3.h"
#include "net_research.h"
#include "net_util.h"
#include "Advances.h"
#include "Player.h"

extern Player **g_player;

NetResearch::NetResearch(Advances *adv)
{
	m_adv = adv;
}

void NetResearch::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_RESEARCH_ID);

	PUSHLONG(m_adv->m_owner);
	sint32 i;
	sint32 d;
	for(i = 0; i < m_adv->m_size; i+= 8) {
		buf[size] = 0;
		for(d = 0; d < 8 && i+d < m_adv->m_size; d++) {
			Assert((m_adv->m_canResearch[i+d] & 0xfe) == 0);
			buf[size] |= m_adv->m_canResearch[i+d] << (7 - d);
		}
		size++;
	}
}

void NetResearch::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_RESEARCH_ID);
	sint32 owner;
	PULLLONG(owner);
	Assert(g_player[owner]);
	if(!g_player[owner])
		return;

	m_adv = g_player[owner]->m_advances;
	sint32 i, d;
	for(i = 0; i < m_adv->m_size; i += 8) {
		for(d = 0; d < 8 && i+d < m_adv->m_size; d++) {
			m_adv->m_canResearch[i + d] = uint8((buf[pos] & (1 << (7-d))) >> (7-d));
		}
		pos++;
	}
	Assert(pos == size);
}
