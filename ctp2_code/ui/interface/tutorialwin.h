

#pragma once
#ifndef __TUTORIALWIN_H__
#define __TUTORIALWIN_H__

#include "c3_ListItem.h"
#include "c3_listbox.h"

class c3_PopupWindow;
class c3_Button;
class c3_Switch;
class aui_StringTable;

#define k_MINIMIZED_HEIGHT	40		
#define k_MAXIMIZED_HEIGHT	310

class TutorialWin {
public:
	c3_PopupWindow	*m_window;

	TutorialWin( void );
	~TutorialWin( void );

	sint32 Initialize( MBCHAR *windowBlock );
protected:

	c3_ListBox		*m_list;

	c3_Switch		*m_titleButton;
	c3_Button		*m_endButton;
	c3_Button		*m_exitButton;

	BOOL		m_minimized;

	aui_StringTable *m_string;

public:
	void Display( void );
	void Remove( void );

	sint32 UpdateData( void );
	sint32 HandleButton( c3_Button *button );
	sint32 HandleSwitch( c3_Switch *button );

	sint32 AddToList( MBCHAR *text, sint32 index );

	void ClearList( void ) { m_list->Clear(); }
};


sint32 tutorialwin_Initialize( void );
sint32 tutorialwin_Cleanup( void );

#endif
