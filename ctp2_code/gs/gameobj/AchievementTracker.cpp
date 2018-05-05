#include "ctp/c3.h"
#include "gs/gameobj/AchievementTracker.h"
#include "robot/aibackdoor/civarchive.h"
#include "gs/gameobj/Player.h"
#include "net/general/network.h"
#include "net/general/net_info.h"
#include "gs/fileio/gamefile.h"

extern Player **g_player;

AchievementTracker *g_theAchievementTracker = NULL;

void AchievementTracker::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {

	} else {
		if(g_saveFileVersion < 55) {
			archive >> m_achievements;
		}
	}
}

BOOL AchievementTracker::HasAchieved(sint32 which)
{
	return (m_achievements & ((uint64)1 << (uint64)which)) != 0;
}

void AchievementTracker::AddAchievement(sint32 which)
{
	m_achievements |= (uint64)1 << (uint64)which;
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_ACHIEVEMENTS,
									  (uint32)(m_achievements & 0xffffffff),
									  (uint32)(m_achievements >> 32)));
	}
}
