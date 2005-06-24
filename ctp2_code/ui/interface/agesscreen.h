#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __AGESSCREEN_H__
#define __AGESSCREEN_H__

sint32 agesscreen_displayMyWindow(BOOL viewMode = FALSE);
sint32 agesscreen_removeMyWindow(uint32 action);
AUI_ERRCODE agesscreen_Initialize( aui_Control::ControlActionCallback *callback = NULL );
AUI_ERRCODE agesscreen_Cleanup();

void agesscreen_startDropDownCallback(aui_Control *,uint32,uint32,void *);
void agesscreen_endDropDownCallback(aui_Control *,uint32,uint32,void *);
void agesscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

sint32 agesscreen_getStartAge( void );
sint32 agesscreen_getEndAge( void );
void agesscreen_setStartAge( sint32 );
void agesscreen_setEndAge( sint32 );

#endif
