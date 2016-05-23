#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __VICTORYWIN_H__
#define __VICTORYWIN_H__

#include "ctp2_listitem.h"
#include "aui_action.h"

class Unit;
class c3_PopupWindow;
class ctp2_Button;
class c3_Static;
class ctp2_ListBox;
class HighScoreDB;

class ctp2_Button;

#define k_VICWIN_DEFEAT		0
#define k_VICWIN_VICTORY	1

#define k_VICWIN_WONDER_ROW_MAX	5
#define k_VICWIN_WONDER_COL_MAX	7

#define k_VICWIN_WONDERICON_MAX (k_VICWIN_WONDER_ROW_MAX * k_VICWIN_WONDER_COL_MAX)

enum {
	k_VICWIN_SCORE_LABEL,
	k_VICWIN_HISTORY_LABEL,
	k_VICWIN_POP_LABEL,
	k_VICWIN_POP_BOX,
	k_VICWIN_CITY_LABEL,
	k_VICWIN_CITY_BOX,
	k_VICWIN_RANKING_TITLE,
	k_VICWIN_MAIN_TITLE,
	k_VICWIN_STATIC_MAX
};




class WonderIconListItem : public ctp2_ListItem
{
public:

	WonderIconListItem(AUI_ERRCODE *retval, sint32 age, sint32 *array, MBCHAR *ldlBlock);


	virtual void Update(void);

	sint32	*GetArray( void ) { return m_array; }

protected:
	WonderIconListItem() : ctp2_ListItem() {}


	AUI_ERRCODE InitCommonLdl(sint32 age, sint32 *array, MBCHAR *ldlBlock);

public:

	virtual sint32 Compare(ctp2_ListItem *item2, uint32 column);

private:
	sint32	m_age;
	sint32	m_array[k_VICWIN_WONDER_COL_MAX];
};




class WonderListItem : public ctp2_ListItem
{
public:
	WonderListItem(AUI_ERRCODE *retval, MBCHAR *name, sint32 score, MBCHAR *ldlBlock);
protected:
	WonderListItem() : ctp2_ListItem() { }
};




class HighScoreListItem : public ctp2_ListItem
{
public:

	HighScoreListItem(AUI_ERRCODE *retval, MBCHAR *name, sint32 score, MBCHAR *ldlBlock);


	virtual void Update(void);

	MBCHAR	*GetName( void ) { return m_name; }
	sint32	GetScore( void ) { return m_score; }

protected:
	HighScoreListItem() : ctp2_ListItem() {}


	AUI_ERRCODE InitCommonLdl(MBCHAR *name, sint32 score, MBCHAR *ldlBlock);

public:

	virtual sint32 Compare(ctp2_ListItem *item2, uint32 column);

private:
	MBCHAR			m_name[256];
	sint32			m_score;
};






class HighScoreWindowPopup
{
public:
	HighScoreWindowPopup( sint32 type );
	~HighScoreWindowPopup( void );

	c3_PopupWindow	*m_window;

	c3_Static		*m_title;
	ctp2_ListBox	*m_list;
	ctp2_Button		*m_continueButton;
	ctp2_Button		*m_quitButton;
	ctp2_Button		*m_creditsButton;
	HighScoreDB		*m_highScoreDB;

public:
	sint32  Initialize ( MBCHAR *windowBlock );
	void    Cleanup(void);
	sint32  UpdateData ( void );

	void    RemoveWindow( void );
	void    DisplayWindow( void );
	c3_PopupWindow *GetWindow() { return m_window; }
};

AUI_ACTION_BASIC(CloseVictoryWindowAction);

sint32 victorywin_AddWonders( MBCHAR* windowBlock );
sint32 victorywin_Initialize( sint32 type = 0 );
sint32 victorywin_Init_Controls( MBCHAR* windowBlock );
sint32 victorywin_Init_TabGroups( MBCHAR* windowBlock );

sint32 victorywin_DisplayWindow( sint32 type = 0 );
void   victorywin_Cleanup( void );
sint32 victorywin_UpdateData( sint32 type );
sint32 victorywin_RemoveWindow( void );

sint32 victorywin_DisplayHighScore( void );

sint32 victorywin_SetLineGraph( BOOL lineGraph);

sint32 victorywin_GetWonderFilename( sint32 index, MBCHAR *name );
sint32 victorywin_LoadGraphData( void );
sint32 victorywin_LoadScoreData( void );
sint32 victorywin_LoadWonderData( void );

sint32 victorywin_GetRankName( sint32 player, MBCHAR *name, sint32 gameResult );
bool   victorywin_IsOnScreen();

#endif
