

#pragma once

#ifndef __SPNEWGAMETRIBESCREEN_H__
#define __SPNEWGAMETRIBESCREEN_H__

#include "Civilisation.h"

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
