#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SCI_ADVANCESCREEN_H__
#define __SCI_ADVANCESCREEN_H__

#define k_SCI_INCLUDE_CANCEL		1

class aui_Control;

sint32 sci_advancescreen_displayMyWindow( MBCHAR *messageText = NULL, sint32 from = NULL);
sint32 sci_advancescreen_removeMyWindow(uint32 action);
AUI_ERRCODE sci_advancescreen_Initialize( MBCHAR *messageText = NULL );
void sci_advancescreen_Cleanup(void);

void sci_advancescreen_cancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void sci_advancescreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

sint32 sci_advancescreen_loadList( void );
sint32 sci_advancescreen_updateData( MBCHAR *messageText = NULL, BOOL defaultMessage = TRUE );
sint32 sci_advancescreen_setStatsInfo(const sint32 index, const sint32 owner);
sint32 sci_advancescreen_isOnScreen();
bool   sci_advancescreen_hasGoal();
bool   sci_advancescreen_isGoal(sint32 goal);
void   sci_advancescreen_initAndFillGoalArray(sint32 goal);
void   sci_advancescreen_clearGoalArray();
void   sci_advancescreen_getGoalAdvances(sint32 & have, sint32 & all);

#endif
