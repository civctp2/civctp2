

#pragma once

#ifndef __SPNEWGAMEPLAYERSSCREEN_H__
#define __SPNEWGAMEPLAYERSSCREEN_H__

#include "aui_control.h"

sint32 spnewgameplayersscreen_displayMyWindow();
sint32 spnewgameplayersscreen_removeMyWindow(uint32 action);
AUI_ERRCODE spnewgameplayersscreen_Initialize( aui_Control::ControlActionCallback *callback = NULL );
AUI_ERRCODE spnewgameplayersscreen_Cleanup();

void spnewgameplayersscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgameplayersscreen_SetMaxPlayers(sint32 maxPlayers);

#endif