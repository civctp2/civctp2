#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __STATSWINDOW_H__
#define __STATSWINDOW_H__

#include "c3window.h"

enum SW {
	SW_NO_CITIES,
	SW_NO_UNITS,
	SW_NO_DIPLOMACY
};

enum BUTTON {
	BUTTON_CIV,
	BUTTON_CITY,
	BUTTON_UNIT,
	BUTTON_SCI,
	BUTTON_DIP,
	BUTTON_TRADE,
	BUTTON_LIB,
	BUTTON_INFO,
	BUTTON_OPTIONS
};

class StatsWindow : public C3Window
{
public:
	StatsWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_BACKGROUND );
	StatsWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		const MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_BACKGROUND );

	virtual ~StatsWindow() {}

	virtual AUI_ERRCODE InitCommon(void);
	virtual AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	virtual AUI_ERRCODE			Idle( void );

	BOOL	CheckCity( void );
	BOOL	CheckUnit( void );
	BOOL	CheckDiplomacy( void );

private:
	sint32 m_dest;
	sint32 m_current;
	BOOL m_scroll;

};

#define k_STATS_WINDOW_HEIGHT		40
#define k_STATS_NUM_BUTTONS			8

int StatsWindow_Initialize( void );
int StatsWindow_Cleanup( void );

sint32 statswindow_EnableDiplomacy();
sint32 statswindow_EnableTrade();
sint32 statswindow_EnableCityButtons();

BOOL statswindow_IsTradeDisabled();

sint32 statswindow_DisableScreensButton( void );
sint32 statswindow_EnableScreensButton( void );

#endif
