#include "c3.h"
#include "net_world.h"
#include "World.h"
#include "net_util.h"
#include "ResourceRecord.h"

void NetWorld::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_WORLD_ID);

	sint32 g;
	PUSHLONG(g_theResourceDB->NumRecords());
	for(g = 0; g < g_theResourceDB->NumRecords(); g++) {
		PUSHDOUBLE(g_theWorld->m_goodValue[g]);
	}
}

void NetWorld::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_WORLD_ID);

	sint32 gc;
	PULLLONG(gc);
	Assert(gc == g_theResourceDB->NumRecords());

	sint32 g;
	for(g = 0; g < gc && g < g_theResourceDB->NumRecords(); g++) {
		PULLDOUBLE(g_theWorld->m_goodValue[g]);
	}
}
