
#include "c3.h"
#include "HappyTracker.h"
#include "StrDB.h"
#include "civarchive.h"
#include "gamefile.h"

extern StringDB *g_theStringDB;

StringID HappyTracker::sm_happinessNames[HAPPY_REASON_MAX];




static sint32 s_refCount = 0;

HappyTracker::HappyTracker()
{
	if(!s_refCount) {
		
		g_theStringDB->GetStringID("HAPPY_REASON_CITY_SIZE", sm_happinessNames[HAPPY_REASON_CITY_SIZE]);
		g_theStringDB->GetStringID("HAPPY_REASON_DISTANCE_FROM_CAPITOL", sm_happinessNames[HAPPY_REASON_DISTANCE_FROM_CAPITOL]);
		g_theStringDB->GetStringID("HAPPY_REASON_ENEMY_ACTION", sm_happinessNames[HAPPY_REASON_ENEMY_ACTION]);
		g_theStringDB->GetStringID("HAPPY_REASON_POLLUTION", sm_happinessNames[HAPPY_REASON_POLLUTION]);
		g_theStringDB->GetStringID("HAPPY_REASON_STARVATION", sm_happinessNames[HAPPY_REASON_STARVATION]);
		g_theStringDB->GetStringID("HAPPY_REASON_PEACE_MOVEMENT", sm_happinessNames[HAPPY_REASON_PEACE_MOVEMENT]);
		g_theStringDB->GetStringID("HAPPY_REASON_WORKDAY", sm_happinessNames[HAPPY_REASON_WORKDAY]);
		g_theStringDB->GetStringID("HAPPY_REASON_WAGES", sm_happinessNames[HAPPY_REASON_WAGES]);
		g_theStringDB->GetStringID("HAPPY_REASON_RATIONS", sm_happinessNames[HAPPY_REASON_RATIONS]);
		g_theStringDB->GetStringID("HAPPY_REASON_MARTIAL_LAW", sm_happinessNames[HAPPY_REASON_MARTIAL_LAW]);
		g_theStringDB->GetStringID("HAPPY_REASON_ENTERTAINERS", sm_happinessNames[HAPPY_REASON_ENTERTAINERS]);
		g_theStringDB->GetStringID("HAPPY_REASON_BUILDINGS", sm_happinessNames[HAPPY_REASON_BUILDINGS]);
		g_theStringDB->GetStringID("HAPPY_REASON_WONDERS", sm_happinessNames[HAPPY_REASON_WONDERS]);
		g_theStringDB->GetStringID("HAPPY_REASON_CRIME", sm_happinessNames[HAPPY_REASON_CRIME]);
		g_theStringDB->GetStringID("HAPPY_REASON_CONQUEST_DISTRESS", sm_happinessNames[HAPPY_REASON_CONQUEST_DISTRESS]);
		g_theStringDB->GetStringID("HAPPY_REASON_FEATS", sm_happinessNames[HAPPY_REASON_FEATS]);
		g_theStringDB->GetStringID("HAPPY_REASON_NUM_CITIES", sm_happinessNames[HAPPY_REASON_NUM_CITIES]);

		
		g_theStringDB->GetStringID("HAPPY_REASON_HAPPINESS_ATTACK", sm_happinessNames[HAPPY_REASON_HAPPINESS_ATTACK]);
		g_theStringDB->GetStringID("HAPPY_REASON_SLAVES_TAKEN", sm_happinessNames[HAPPY_REASON_SLAVES_TAKEN]);
		g_theStringDB->GetStringID("HAPPY_REASON_INDULGENCES", sm_happinessNames[HAPPY_REASON_INDULGENCES]);
		g_theStringDB->GetStringID("HAPPY_REASON_BIO_INFECTION", sm_happinessNames[HAPPY_REASON_BIO_INFECTION]);
		g_theStringDB->GetStringID("HAPPY_REASON_REFORMATION", sm_happinessNames[HAPPY_REASON_REFORMATION]);
		g_theStringDB->GetStringID("HAPPY_REASON_POP_TYPE_REMOVED", sm_happinessNames[HAPPY_REASON_POP_TYPE_REMOVED]);
		g_theStringDB->GetStringID("HAPPY_REASON_POP_TYPE_ADDED", sm_happinessNames[HAPPY_REASON_POP_TYPE_ADDED]);
		g_theStringDB->GetStringID("HAPPY_REASON_SOOTHSAY", sm_happinessNames[HAPPY_REASON_SOOTHSAY]);
		g_theStringDB->GetStringID("HAPPY_REASON_EMANCIPATION", sm_happinessNames[HAPPY_REASON_EMANCIPATION]);
		g_theStringDB->GetStringID("HAPPY_REASON_POST_REVOULTION_BLISS", sm_happinessNames[HAPPY_REASON_POST_REVOULTION_BLISS]);
		g_theStringDB->GetStringID("HAPPY_REASON_WONDER_TURNS_REMAINING", sm_happinessNames[HAPPY_REASON_WONDER_TURNS_REMAINING]);
		g_theStringDB->GetStringID("HAPPY_REASON_ATTACKED_CONVERTER", sm_happinessNames[HAPPY_REASON_ATTACKED_CONVERTER]);
		g_theStringDB->GetStringID("HAPPY_REASON_ASSASSINATION", sm_happinessNames[HAPPY_REASON_ASSASSINATION]);
	}

	ClearEntries(HAPPY_REASON((sint32)HAPPY_REASON_SMOKING_CRACK + 1), HAPPY_REASON_MAX);
	s_refCount++;
}

HappyTracker::HappyTracker(CivArchive &archive)
{
	Serialize(archive);
}

HappyTracker::~HappyTracker()
{
	s_refCount--;
}

void HappyTracker::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive.Store((uint8*)m_happinessAmounts, sizeof(double) * (HAPPY_REASON_MAX));
	} else {
		
		if(g_saveFileVersion < 65) {
			archive.Load((uint8*)m_happinessAmounts, sizeof(double) * (HAPPY_REASON_MAX - 1));
			sint32 i;
			for(i = (sint32)HAPPY_REASON_HAPPINESS_ATTACK; i < HAPPY_REASON_MAX; i++) {
				m_happinessAmounts[i] = m_happinessAmounts[i - 1];
			}
			m_happinessAmounts[HAPPY_REASON_FEATS] = 0;
		} else {
			archive.Load((uint8*)m_happinessAmounts, sizeof(double) * (HAPPY_REASON_MAX));
		}
	}
}

void HappyTracker::GetHappiness(HAPPY_REASON which,
								 double &amount,
								 StringID &name)
{
	amount = m_happinessAmounts[which];
	name = sm_happinessNames[which];
}


void HappyTracker::SetHappiness(HAPPY_REASON which, double amount)
{
	m_happinessAmounts[which] = amount;
}

void HappyTracker::SaveHappiness()
{
	memcpy(m_tempSaveHappiness, m_happinessAmounts, sizeof(m_happinessAmounts));
}

void HappyTracker::RestoreHappiness()
{
	memcpy(m_happinessAmounts, m_tempSaveHappiness, sizeof(m_happinessAmounts));
}

void HappyTracker::ClearEntries(HAPPY_REASON start, HAPPY_REASON end)
{
	
	
	

	if((sint32)start < 0) {
		start = (HAPPY_REASON)0;
	}

	for(; start < end; start = HAPPY_REASON((sint32)start + 1)) {
		m_happinessAmounts[start] = 0.0;
	}
}


void HappyTracker::AddTimedChange(HAPPY_REASON which, double delta)
{
	Assert(which > HAPPY_REASON_SMOKING_CRACK);
	Assert(which < HAPPY_REASON_MAX);
	if(which <= HAPPY_REASON_SMOKING_CRACK ||
	   which >= HAPPY_REASON_MAX)
		return;

	m_happinessAmounts[which] += delta;
}

#ifdef _DEBUG
void HappyTracker::Dump()
{
	HAPPY_REASON i;

	DPRINTF(k_DBG_GAMESTATE, ("Happy happy, joy joy:\n"));
	for(i = HAPPY_REASON((sint32)HAPPY_REASON_SMOKING_CRACK + 1);
		i < HAPPY_REASON_MAX; i = HAPPY_REASON((sint32)i + 1)) {
		DPRINTF(k_DBG_GAMESTATE, ("  %20s: %.2lf\n", 
							 g_theStringDB->GetNameStr(sm_happinessNames[i]),
							 m_happinessAmounts[i]));
	}
}
#endif
