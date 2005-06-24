//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Menu bar user interface element
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
// - Made AddMenuItem itemID parameter const.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CTP2_MENUBAR_H__
#define __CTP2_MENUBAR_H__


#include "aui_window.h"
#include "ctp2_Menu.h"
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
	void			AddMenuItem		
    (
        ctp2_Menu *     menu,
        MBCHAR const *  itemID,
        MBCHAR const *  shortcut,
        void *          cookie
    );

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
