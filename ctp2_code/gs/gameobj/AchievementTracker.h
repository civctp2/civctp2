
#pragma once










#ifndef __ACHIEVEMENT_TRACKER_H__
#define __ACHIEVEMENT_TRACKER_H__

#define ACHIEVE_UNDERSEA_CITY 0
#define ACHIEVE_SPACE_CITY    1

class CivArchive;

class AchievementTracker {
private:
	uint64 m_achievements;
public:
	AchievementTracker() { m_achievements = 0; }
	AchievementTracker(CivArchive &archive) { Serialize(archive); }
	void Serialize(CivArchive &archive);
    void SetData(uint64 data) { m_achievements = data; }
	uint64 GetData() const { return m_achievements; }
    
	BOOL HasAchieved(sint32 which);
	void AddAchievement(sint32 which);
};

extern AchievementTracker *g_theAchievementTracker;

#endif
