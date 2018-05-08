#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __WONDERMOVIEWIN__
#define __WONDERMOVIEWIN__

#include "ui/aui_common/aui_action.h"     // aui_Action
#include "os/include/ctp2_inttypes.h"  // sint32, uint32

class Sequence;

void wondermoviewin_Initialize(std::weak_ptr<Sequence> seq);
void wondermoviewin_DisplayWonderMovie(sint32 id);

void wondermoviewin_Cleanup();

void wondermoviewin_MovieButtonCallback(aui_Control *control, uint32 action, uint32 data, void * cookie);

AUI_ACTION_BASIC(CloseMovieAction);

#endif
