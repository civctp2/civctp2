#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef MUSICSCREEN_H_
#define MUSICSCREEN_H_

enum MS_STRING {
	MS_STRING_AUTO_ON,
	MS_STRING_AUTO_OFF,
	MS_STRING_RANDOM_ON,
	MS_STRING_RANDOM_OFF
};

#include "auitypes.h"       // AUI_ERRCODE
#include "ctp2_inttypes.h"  // sint32, uint32
class aui_Control;

sint32 musicscreen_displayMyWindow();
sint32 musicscreen_removeMyWindow(uint32 action);
AUI_ERRCODE musicscreen_Initialize( void );
void musicscreen_Cleanup();

void musicscreen_checkPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void musicscreen_selectTrackPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void musicscreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

#endif
