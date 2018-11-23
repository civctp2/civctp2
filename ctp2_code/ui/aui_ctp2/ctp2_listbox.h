//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface list box
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
// - Event handlers declared in a notation that is more standard C++.
// - #pragma once commented out.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CTP2_LISTBOX__
#define __CTP2_LISTBOX__

#include "patternbase.h"
#include "aui_listbox.h"

#define k_CTP2_LISTBOX_LDL_BEVELWIDTH		"bevelwidth"
#define k_CTP2_LISTBOX_LDL_BEVELTYPE		"beveltype"
#define k_CTP2_LISTBOX_LDL_BORDER_WIDTH     "borderwidth"
#define k_CTP2_LISTBOX_LDL_BORDER_HEIGHT    "borderheight"

class aui_Surface;
class ctp2_ListItem;
class ctp2_MenuButton;

class ctp2_ListBox : public aui_ListBox, public PatternBase
{
public:
	ctp2_ListBox() : aui_ListBox() {}
	ctp2_ListBox(AUI_ERRCODE *retval,	uint32 id, const MBCHAR *ldlBlock,
							ControlActionCallback *ActionFunc=NULL, void *cookie=NULL );
	ctp2_ListBox(AUI_ERRCODE *retval, uint32 id, sint32 x, sint32 y, sint32 width, sint32 height,
							const MBCHAR *pattern, sint32 bevelwidth = 0, sint32 beveltype = 0,
							ControlActionCallback *ActionFunc = NULL, void *cookie = NULL);

	virtual ~ctp2_ListBox();


	virtual BOOL IsThisA( uint32 classId )
	{
		return classId == m_ctp2_listboxClassId;
	}

	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	AUI_ERRCODE InitCommon(sint32 bevelWidth, sint32 bevelType );
	AUI_ERRCODE CreateRangersAndHeader(const MBCHAR *ldlBlock = NULL);

	void Clear(void);

	virtual AUI_ERRCODE SortByColumn( sint32 column, BOOL ascending );
	virtual AUI_ERRCODE Draw(aui_Surface *surface = NULL, sint32 x = 0, sint32 y = 0);
	virtual AUI_ERRCODE DrawThis(aui_Surface *surface = NULL, sint32 x = 0, sint32 y = 0 );

	sint32 GetBevelWidth(void) { return m_bevelWidth; }
	sint32 GetBevelType(void) { return m_bevelType; }


	void ForceScroll(sint32 deltaX, sint32 deltaY);

	static ctp2_ListBox *ms_mouseFocusListBox;

	static ctp2_ListBox *GetMouseFocusListBox(void) { return ms_mouseFocusListBox; }
	static void SetMouseFocusListBox(ctp2_ListBox *box) { ms_mouseFocusListBox = box; }

	sint32 GetMaxItemHeight() { return m_maxItemHeight; }
	sint32 GetMaxItemWidth() { return m_maxItemWidth; }
	void SetMenuButton(ctp2_MenuButton *butt) { m_menuButton = butt; }

	void GetDisplayRange(sint32 &top, sint32 &bottom);
	void EnsureItemVisible(sint32 index);

protected:
	virtual AUI_ERRCODE DoneInstantiatingThis(const MBCHAR *ldlBlock);

	AUI_ERRCODE ReformatItemFromHeader(aui_Item *item);

	virtual void	MouseMoveOver(aui_MouseEvent * mouseData);
	virtual void	MouseMoveInside(aui_MouseEvent * mouseData);
	virtual void	MouseMoveOutside(aui_MouseEvent * mouseData);
	virtual void	MouseMoveAway(aui_MouseEvent * mouseData);

	virtual void	MouseLDragOver(aui_MouseEvent * mouseData);
	virtual void	MouseLDragAway(aui_MouseEvent * mouseData);
	virtual void	MouseRDragOver(aui_MouseEvent * mouseData);
	virtual void	MouseRDragAway(aui_MouseEvent * mouseData);

public:

	static uint32		m_ctp2_listboxClassId;

	friend class ctp2_Menu;
private:
	sint32		m_bevelWidth;
	sint32		m_bevelType;

	ctp2_MenuButton *m_menuButton;

	POINT m_borderOffset;
};

#endif
