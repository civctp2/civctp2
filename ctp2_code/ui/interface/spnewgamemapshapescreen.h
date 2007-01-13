//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Import structure improved, linux synchronisation
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __SPNEWGAMEMAPSHAPESCREEN_H__
#define __SPNEWGAMEMAPSHAPESCREEN_H__

#include "ctp2_inttypes.h"  // sint32
#include "aui_Control.h"    // aui_Control

sint32 spnewgamemapshapescreen_displayMyWindow(BOOL viewMode = FALSE, sint32 useMode = 0);
sint32 spnewgamemapshapescreen_removeMyWindow(uint32 action);
AUI_ERRCODE spnewgamemapshapescreen_Initialize( aui_Control::ControlActionCallback *callback = NULL );
void    spnewgamemapshapescreen_Cleanup();

void spnewgamemapshapescreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamemapshapescreen_cancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamemapshapescreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

size_t  spnewgamemapshapescreen_getMapShapeIndex(void);
void    spnewgamemapshapescreen_setMapShapeIndex(size_t);

#endif
