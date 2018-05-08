#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __VICTORYMOVIEWIN__
#define __VICTORYMOVIEWIN__

#include <memory>

#include "ui/aui_common/aui_action.h"

class Sequence;
enum GAME_OVER;

void victorymoviewin_Initialize(std::weak_ptr<Sequence> seq);
void victorymoviewin_DisplayVictoryMovie(GAME_OVER reason);

void victorymoviewin_Cleanup();

void victorymoviewin_MovieButtonCallback(aui_Control *control, uint32 action, uint32 data, void * cookie);

AUI_ACTION_BASIC(CloseVictoryMovieAction);

#endif
