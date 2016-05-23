#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef HAPPY_TRACKER_H__
#define HAPPY_TRACKER_H__

class HappyTracker;

enum HAPPY_REASON
{
	HAPPY_REASON_SMOKING_CRACK = -1,

	HAPPY_REASON_CITY_SIZE,
	HAPPY_REASON_DISTANCE_FROM_CAPITOL,
	HAPPY_REASON_ENEMY_ACTION,
	HAPPY_REASON_POLLUTION,
	HAPPY_REASON_STARVATION,
	HAPPY_REASON_PEACE_MOVEMENT,
	HAPPY_REASON_WORKDAY,
	HAPPY_REASON_WAGES,
	HAPPY_REASON_RATIONS,
	HAPPY_REASON_MARTIAL_LAW,
	HAPPY_REASON_ENTERTAINERS,
	HAPPY_REASON_BUILDINGS,
	HAPPY_REASON_WONDERS,
	HAPPY_REASON_CRIME,
	HAPPY_REASON_CONQUEST_DISTRESS,
	HAPPY_REASON_NUM_CITIES,
	HAPPY_REASON_FEATS,

	HAPPY_REASON_HAPPINESS_ATTACK,
	HAPPY_REASON_SLAVES_TAKEN,
	HAPPY_REASON_INDULGENCES,
	HAPPY_REASON_BIO_INFECTION,
	HAPPY_REASON_REFORMATION,
	HAPPY_REASON_POP_TYPE_REMOVED,
	HAPPY_REASON_POP_TYPE_ADDED,
	HAPPY_REASON_SOOTHSAY,
	HAPPY_REASON_EMANCIPATION,
	HAPPY_REASON_ATTACKED_CONVERTER,

	HAPPY_REASON_POST_REVOULTION_BLISS,
	HAPPY_REASON_WONDER_TURNS_REMAINING,
	HAPPY_REASON_ASSASSINATION,
	HAPPY_REASON_SECTHAPPY, //emod

	HAPPY_REASON_MAX
};

#include "dbtypes.h"    // StringId
class CivArchive;

class HappyTracker
{
public:
	HappyTracker();
	HappyTracker(CivArchive &archive);
	~HappyTracker();

	void Serialize(CivArchive &archive);

    void GetHappiness
    (
        HAPPY_REASON    which,
        double &        amount,
        StringId &      name
    );
	void SetHappiness(HAPPY_REASON which, double amount);

	void SaveHappiness();
	void RestoreHappiness();

	void ClearEntries(HAPPY_REASON start, HAPPY_REASON end);
	void AddTimedChange(HAPPY_REASON which, double delta);

#if defined(_DEBUG)
	void Dump();
#endif

private:
    static void RefreshStringIds(void);

	double m_happinessAmounts[HAPPY_REASON_MAX];
	static StringId sm_happinessNames[HAPPY_REASON_MAX];

	double m_tempSaveHappiness[HAPPY_REASON_MAX];
};

#endif
