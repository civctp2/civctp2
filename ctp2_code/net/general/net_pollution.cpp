

#include "c3.h"
#include "net_pollution.h"
#include "network.h"
#include "net_util.h"

#include "pollution.h"
#include "PollutionConst.h"

extern Pollution *g_thePollution;

void NetPollution::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;

	PUSHID(k_PACKET_POLLUTION_ID);

	PUSHLONG(g_thePollution->m_trend);
	for(sint32 i = 0; i < k_MAX_GLOBAL_POLLUTION_RECORD_TURNS; i++) {
		PUSHLONG(g_thePollution->m_history[i]);
	}
	PUSHLONG(g_thePollution->m_phase);

}

void NetPollution::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_POLLUTION_ID);
	if(packid != k_PACKET_POLLUTION_ID)
		return;

	PULLLONG(g_thePollution->m_trend);
	for(sint32 i = 0; i < k_MAX_GLOBAL_POLLUTION_RECORD_TURNS; i++) {
		PULLLONG(g_thePollution->m_history[i]);
	}
	PULLLONG(g_thePollution->m_phase);


	Assert(pos == size);
}

