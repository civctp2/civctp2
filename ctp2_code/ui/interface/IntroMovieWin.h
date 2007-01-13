

#pragma once
#ifndef __INTROMOVIEWIN__
#define __INTROMOVIEWIN__

#include "aui_action.h"

void intromoviewin_Initialize(void);
void intromoviewin_DisplayIntroMovie(void);

void intromoviewin_Cleanup();

void intromoviewin_MovieButtonCallback(aui_Control *control, uint32 action, uint32 data, void * cookie);

class CloseIntroMovieAction : public aui_Action
{
	virtual ActionCallback Execute;
};

#endif
