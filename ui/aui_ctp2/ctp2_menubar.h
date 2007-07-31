










 

#pragma once
#ifndef __CTP2_MENUBAR_H__
#define __CTP2_MENUBAR_H__


#include "aui_window.h"
#include "ctp2_menu.h"
#include "patternbase.h"




class ctp2_MenuBar : public aui_Window, public PatternBase
{
public:
	ctp2_MenuBar(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD, bool bevel = true );
	ctp2_MenuBar(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD, bool bevel = true );
	virtual ~ctp2_MenuBar() {}

	
	ctp2_Menu		*GetMenu(MBCHAR *ldlParent,MBCHAR *menuname);
	void			SetMenuCallback	(ctp2_Menu *menu,CTP2MenuCallback *callback);
	void			AddMenuItem		(ctp2_Menu *menu,MBCHAR *itemID,MBCHAR *shortcut,void *cookie);

	void            BuildNeighbors();

protected:
	ctp2_MenuBar() : aui_Window() {}
	AUI_ERRCODE InitCommon( void );

	
	AUI_ERRCODE	AddChild( aui_Region *child );

	
	sint32	m_runningWidth;
	sint32  m_rightRunningWidth;
	sint32	m_largestItemHeight;

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

private:
	bool m_bevel;
};


#endif 
