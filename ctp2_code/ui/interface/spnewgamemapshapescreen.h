

#pragma once

#ifndef __SPNEWGAMEMAPSHAPESCREEN_H__
#define __SPNEWGAMEMAPSHAPESCREEN_H__

sint32 spnewgamemapshapescreen_displayMyWindow(BOOL viewMode = FALSE, sint32 useMode = 0);
sint32 spnewgamemapshapescreen_removeMyWindow(uint32 action);
AUI_ERRCODE spnewgamemapshapescreen_Initialize( aui_Control::ControlActionCallback *callback = NULL );
AUI_ERRCODE spnewgamemapshapescreen_Cleanup();

void spnewgamemapshapescreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamemapshapescreen_cancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamemapshapescreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

sint32 spnewgamemapshapescreen_getMapShapeIndex( void );
void spnewgamemapshapescreen_setMapShapeIndex( sint32 );

#endif
