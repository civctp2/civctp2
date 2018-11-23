#include "c3.h"
#include "net_achievement.h"
#include "AchievementTracker.h"
#include "net_util.h"

void NetAchievementTracker::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_ACHIEVEMENT_TRACKER_ID);
	PUSHLONG64(g_theAchievementTracker->GetData());
}

void NetAchievementTracker::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_ACHIEVEMENT_TRACKER_ID);

	uint64 built;
	PULLLONG64(built);
	g_theAchievementTracker->SetData(built);

	Assert(pos == size);
}
