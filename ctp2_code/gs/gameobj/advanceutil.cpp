#include "c3.h"
#include "AdvanceRecord.h"
#include "advanceutil.h"

sint32 s_nukeAdvance = -1;
sint32 s_alienLife = -1;

void advanceutil_Initialize()
{
	sint32 i;
	for(i = 0 ; i < g_theAdvanceDB->NumRecords(); i++) {
		const AdvanceRecord *rec = g_theAdvanceDB->Get(i);
		if(rec->GetPlantNuke())
			s_nukeAdvance = i;

		if(rec->GetAlienLife())
			s_alienLife = i;
	}
}

sint32 advanceutil_GetNukeAdvance()
{
	return s_nukeAdvance;
}

sint32 advanceutil_GetAlienLifeAdvance()
{
	return s_alienLife;
}

bool advanceutil_AdvanceHasPrereq(sint32 advance, sint32 prereq)
{
	const AdvanceRecord *adv = g_theAdvanceDB->Get(advance);
	sint32 i;

	for(i = 0; i < adv->GetNumPrerequisites(); i++)
	{
		if(adv->GetPrerequisitesIndex(i) == prereq)
			return true;
	}

	return false;
}

bool advanceutil_AdvanceHasEitherPrereq(sint32 advance, sint32 prereq)
{
	const AdvanceRecord *adv = g_theAdvanceDB->Get(advance);
	sint32 i;

	for(i = 0; i < adv->GetNumEitherPrerequisites(); i++)
	{
		if(adv->GetEitherPrerequisitesIndex(i) == prereq)
			return true;
	}

	return false;
}
