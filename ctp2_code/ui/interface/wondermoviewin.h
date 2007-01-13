

#pragma once
#ifndef __WONDERMOVIEWIN__
#define __WONDERMOVIEWIN__

#include "aui_action.h"

class Sequence;

void wondermoviewin_Initialize(Sequence *seq);
void wondermoviewin_DisplayWonderMovie(sint32 id);

void wondermoviewin_Cleanup();

void wondermoviewin_MovieButtonCallback(aui_Control *control, uint32 action, uint32 data, void * cookie);

class CloseMovieAction : public aui_Action
{
	virtual ActionCallback Execute;
};

#endif
