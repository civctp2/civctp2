//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Tribe selection screen functions
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
// _MSC_VER		
// - Compiler version (for the Microsoft C++ compiler only)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Import structure cleaned up somewhat.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __SPNEWGAMETRIBESCREEN_H__
#define __SPNEWGAMETRIBESCREEN_H__

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "aui_control.h"    // aui_Control
#include "c3types.h"        // MBCHAR, sint32, uint32
#include "Civilisation.h"   // GENDER

//----------------------------------------------------------------------------
// Declarations
//----------------------------------------------------------------------------

sint32 spnewgametribescreen_displayMyWindow(
	void *cookie = NULL,
	BOOL edit = FALSE );
sint32 spnewgametribescreen_removeMyWindow(uint32 action,MBCHAR *lname = NULL);

AUI_ERRCODE spnewgametribescreen_Initialize( aui_Control::ControlActionCallback *callback = NULL );
AUI_ERRCODE spnewgametribescreen_Cleanup();


void spnewgamescreen_scenarioExitCallback(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_scenarioPress(aui_Control *control, uint32 action, uint32 data, void *cookie) ;

void spnewgametribescreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgametribescreen_cancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgametribescreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgametribescreen_switchPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgametribescreen_malePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgametribescreen_femalePress(aui_Control *control, uint32 action, uint32 data, void *cookie );

sint32 spnewgametribescreen_getTribeIndex( void );
GENDER spnewgametribescreen_getGender(void);
void spnewgametribescreen_setTribeIndex( sint32, MBCHAR *lname = NULL );
void spnewgametribescreen_getLeaderName( MBCHAR *lname );

void spnewgametribescreen_enableTribes( void );
void spnewgametribescreen_disableTribes(void);		
void spnewgametribescreen_disableTribe( sint32 tribe );
void spnewgametribescreen_enableTribe( sint32 tribe );

#endif
