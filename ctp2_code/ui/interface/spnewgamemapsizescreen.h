#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __SPNEWGAMEMAPSIZESCREEN_H__
#define __SPNEWGAMEMAPSIZESCREEN_H__

#define k_NUM_MAPSIZEBOXES	4

#define k_SMALL_X		24
#define k_SMALL_Y		48
#define k_SMALL_Z		2
#define k_MEDIUM_X		48
#define k_MEDIUM_Y		96
#define k_MEDIUM_Z		2
#define k_LARGE_X		64
#define k_LARGE_Y		128
#define k_LARGE_Z		2
#define k_GIGANTIC_X	70
#define k_GIGANTIC_Y	140
#define k_GIGANTIC_Z	2


sint32 spnewgamemapsizescreen_displayMyWindow(BOOL viewMode = FALSE, sint32 useMode = 0);
sint32 spnewgamemapsizescreen_removeMyWindow(uint32 action);
AUI_ERRCODE spnewgamemapsizescreen_Initialize( aui_Control::ControlActionCallback *callback = NULL );
AUI_ERRCODE spnewgamemapsizescreen_Cleanup();

void spnewgamemapsizescreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamemapsizescreen_cancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamemapsizescreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

sint32 spnewgamemapsizescreen_getMapSizeIndex( void );
void spnewgamemapsizescreen_setMapSizeIndex( sint32 );

#endif
