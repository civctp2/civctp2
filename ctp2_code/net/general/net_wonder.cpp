

#include "c3.h"
#include "net_wonder.h"
#include "WonderTracker.h"
#include "net_util.h"

void NetWonderTracker::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_WONDER_TRACKER_ID);
	PUSHLONG64(g_theWonderTracker->GetBuiltWonders());
	PUSHLONG(g_theWonderTracker->GlobeSatFlags());
}

void NetWonderTracker::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_WONDER_TRACKER_ID);
	
	uint64 built;
	PULLLONG64(built);
	g_theWonderTracker->SetBuiltWonders(built);

	uint32 globeSatFlags;
	PULLLONG(globeSatFlags);
	g_theWonderTracker->SetGlobeSatFlags(globeSatFlags);

	Assert(pos == size);
}


