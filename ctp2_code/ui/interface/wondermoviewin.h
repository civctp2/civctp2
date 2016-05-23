#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __WONDERMOVIEWIN__
#define __WONDERMOVIEWIN__

#include "aui_action.h"     // aui_Action
#include "ctp2_inttypes.h"  // sint32, uint32

class Sequence;

void wondermoviewin_Initialize(Sequence *seq);
void wondermoviewin_DisplayWonderMovie(sint32 id);

void wondermoviewin_Cleanup();

void wondermoviewin_MovieButtonCallback(aui_Control *control, uint32 action, uint32 data, void * cookie);

AUI_ACTION_BASIC(CloseMovieAction);

#endif
