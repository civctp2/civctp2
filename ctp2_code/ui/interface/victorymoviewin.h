#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __VICTORYMOVIEWIN__
#define __VICTORYMOVIEWIN__

#include "aui_action.h"
#include "GameOver.h"

class Sequence;

void victorymoviewin_Initialize(Sequence *seq);
void victorymoviewin_DisplayVictoryMovie(GAME_OVER reason);

void victorymoviewin_Cleanup();

void victorymoviewin_MovieButtonCallback(aui_Control *control, uint32 action, uint32 data, void * cookie);

AUI_ACTION_BASIC(CloseVictoryMovieAction);

#endif
