

#pragma once

#ifndef __SPNEWGAMEDIFFSCREEN_H__
#define __SPNEWGAMEDIFFSCREEN_H__

sint32 spnewgamediffscreen_displayMyWindow(BOOL viewMode = FALSE,BOOL reinit=FALSE);
sint32 spnewgamediffscreen_removeMyWindow(uint32 action);
AUI_ERRCODE spnewgamediffscreen_Initialize( aui_Control::ControlActionCallback *callback = NULL );
AUI_ERRCODE spnewgamediffscreen_Cleanup();

void spnewgamediffscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

sint32 spnewgamediffscreen_getDifficulty1( void );
sint32 spnewgamediffscreen_getDifficulty2( void );
void spnewgamediffscreen_setDifficulty1( sint32 );
void spnewgamediffscreen_setDifficulty2( sint32 );

#endif
