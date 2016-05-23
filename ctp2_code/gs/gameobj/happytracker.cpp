#include "c3.h"
#include "HappyTracker.h"

#include <algorithm>
#include "civarchive.h"
#include "gamefile.h"
#include "StrDB.h"          // g_theStringDB

StringId HappyTracker::sm_happinessNames[HAPPY_REASON_MAX];


namespace
{

/// Reference count of cached string ids in ::sm_happinessNames
int             s_refCount                      = 0;
/// Number of stored happiness reasons in save game versions 64 or earlier
size_t const    HAPPY_REASON_MAX_VERSION_64     = 29;
/// Number of stored happiness reasons in save game versions 65 and 66
size_t const    HAPPY_REASON_MAX_VERSION_66     = 30;

} // namespace

HappyTracker::HappyTracker()
{
    std::fill(m_happinessAmounts, m_happinessAmounts + HAPPY_REASON_MAX, 0.0);
    std::fill(m_tempSaveHappiness, m_tempSaveHappiness + HAPPY_REASON_MAX, 0.0);

    if (!s_refCount++)
    {
        RefreshStringIds();
    }
}

HappyTracker::HappyTracker(CivArchive &archive)
{
    std::fill(m_happinessAmounts, m_happinessAmounts + HAPPY_REASON_MAX, 0.0);
    std::fill(m_tempSaveHappiness, m_tempSaveHappiness + HAPPY_REASON_MAX, 0.0);

    Serialize(archive);

    if (!s_refCount++)
    {
        RefreshStringIds();
    }
}

HappyTracker::~HappyTracker()
{
     s_refCount--;
}

void HappyTracker::RefreshStringIds(void)
{
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
	g_theStringDB->GetStringID("HAPPY_REASON_SECTHAPPY", sm_happinessNames[HAPPY_REASON_SECTHAPPY]);
}

/// Store/load the happiness information to/from an archive
/// \param archive  The archive to use
/// \remarks    The HAPPY_REASON_SECTHAPPY item can not be stored without
///             causing a save game incompatibility.
void HappyTracker::Serialize(CivArchive &archive)
{
    if (archive.IsStoring())
    {
        archive.Store((uint8*) m_happinessAmounts,
                      sizeof(double) * HAPPY_REASON_MAX_VERSION_66
                     );
    }
    else
    {
        if (g_saveFileVersion <= 64)
        {
            // Apparently HAPPY_REASON_FEATS was middle-inserted by Activision
            // in version 65, requiring a shift of all items with higher index.
            archive.Load((uint8*) m_happinessAmounts,
                         sizeof(double) * HAPPY_REASON_MAX_VERSION_64
                        );
            for (int i = HAPPY_REASON_FEATS + 1; i < HAPPY_REASON_MAX; ++i)
            {
                 m_happinessAmounts[i] = m_happinessAmounts[i - 1];
	        }
		    m_happinessAmounts[HAPPY_REASON_FEATS] = 0.0;
        }
        else if (g_saveFileVersion <= 66)
        {
            archive.Load((uint8*) m_happinessAmounts,
                         sizeof(double) * HAPPY_REASON_MAX_VERSION_66
                        );
	    }
    }
}

void HappyTracker::GetHappiness
(
    HAPPY_REASON    which,
    double &        amount,
    StringId &      name
)
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
    int validatedStart  = std::max<int>(0, static_cast<int>(start));
    int validatedEnd    = std::min<int>(static_cast<int>(HAPPY_REASON_MAX), static_cast<int>(end));

    std::fill(m_happinessAmounts + validatedStart, m_happinessAmounts + validatedEnd, 0.0);
}

void HappyTracker::AddTimedChange(HAPPY_REASON which, double delta)
{
    Assert((which >= 0) && (which < HAPPY_REASON_MAX));
    if ((which >= 0) && (which < HAPPY_REASON_MAX))
    {
        m_happinessAmounts[which] += delta;
    }
}

#ifdef _DEBUG
void HappyTracker::Dump()
{
    DPRINTF(k_DBG_GAMESTATE, ("Happy happy, joy joy:\n"));
    for (int i = 0; i < HAPPY_REASON_MAX; ++i)
    {
        DPRINTF(k_DBG_GAMESTATE, ("  %20s: %.2lf\n",
						 g_theStringDB->GetNameStr(sm_happinessNames[i]),
						 m_happinessAmounts[i]
                                 )
               );
    }
}
#endif
