
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __INTROMOVIEWIN__
#define __INTROMOVIEWIN__

#include "aui_action.h"

void intromoviewin_Initialize(void);
void intromoviewin_DisplayIntroMovie(void);

void intromoviewin_Cleanup();

void intromoviewin_MovieButtonCallback(aui_Control *control, uint32 action, uint32 data, void * cookie);

AUI_ACTION_BASIC(CloseIntroMovieAction);

#endif
