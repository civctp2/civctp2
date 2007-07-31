

#pragma once

#ifndef __SPNEWGAMERANDOMCUSTOMSCREEN_H__
#define __SPNEWGAMERANDOMCUSTOMSCREEN_H__

sint32 spnewgamerandomcustomscreen_displayMyWindow();
sint32 spnewgamerandomcustomscreen_removeMyWindow( uint32 action );
AUI_ERRCODE spnewgamerandomcustomscreen_Initialize(
	aui_Control::ControlActionCallback *callback = NULL );
AUI_ERRCODE spnewgamerandomcustomscreen_Cleanup();

void spnewgamerandomcustomscreen_acceptPress( aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamerandomcustomscreen_cancelPress( aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamerandomcustomscreen_backPress( aui_Control *control, uint32 action, uint32 data, void *cookie );

#endif 
