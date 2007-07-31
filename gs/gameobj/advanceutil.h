
#ifndef __ADVANCE_UTIL_H__
#define __ADVANCE_UTIL_H__

void advanceutil_Initialize();
sint32 advanceutil_GetNukeAdvance();
sint32 advanceutil_GetAlienLifeAdvance();
bool advanceutil_AdvanceHasPrereq(sint32 advance, sint32 prereq);

#endif
