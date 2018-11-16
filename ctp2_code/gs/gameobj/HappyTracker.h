#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __HAPPY_TRACKER_H__
#define __HAPPY_TRACKER_H__

#include "ctp2_enums.h"

typedef sint32 StringID;

class CivArchive;

#define POLLUTION_LIST_MAX  70

class HappyTracker {
private:
	double m_happinessAmounts[HAPPY_REASON_MAX];
	static StringID sm_happinessNames[HAPPY_REASON_MAX];

	double m_tempSaveHappiness[HAPPY_REASON_MAX];
public:
	HappyTracker();
	HappyTracker(CivArchive &archive);
	~HappyTracker();
	void Serialize(CivArchive &archive);

	void GetHappiness(HAPPY_REASON which,
					  double &amount,
					  StringID &name);
	void SetHappiness(HAPPY_REASON which, double amount);

	void SaveHappiness();
	void RestoreHappiness();

	void ClearEntries(HAPPY_REASON start, HAPPY_REASON end);
	void AddTimedChange(HAPPY_REASON which, double delta);

#ifdef _DEBUG
	void Dump();
#endif
};

#endif
