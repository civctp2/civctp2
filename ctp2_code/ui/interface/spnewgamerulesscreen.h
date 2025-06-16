// edit rulesscreen code to be more like gameplay options
// now it works
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SPNEWGAMERULESSCREEN_H__
#define __SPNEWGAMERULESSCREEN_H__

sint32 spnewgamerulesscreen_displayMyWindow();
sint32 spnewgamerulesscreen_removeMyWindow(uint32 action);
AUI_ERRCODE spnewgamerulesscreen_Initialize( aui_Control::ControlActionCallback *callback = NULL );
AUI_ERRCODE spnewgamerulesscreen_Cleanup();

void spnewgamerulesscreen_agesPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamerulesscreen_checkPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamerulesscreen_exitPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

void spnewgamerulesscreen_mouseSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
#endif
