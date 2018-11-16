#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3_LISTBOX__
#define __C3_LISTBOX__

#include "patternbase.h"
#include "aui_listbox.h"

#define k_C3_LISTBOX_LDL_BEVELWIDTH		"bevelwidth"
#define k_C3_LISTBOX_LDL_BEVELTYPE		"beveltype"

class aui_Surface;
class c3_ListItem;

class c3_ListBox : public aui_ListBox, public PatternBase
{
public:
	c3_ListBox() : aui_ListBox() {}
	c3_ListBox(AUI_ERRCODE *retval,	uint32 id, MBCHAR *ldlBlock,
							ControlActionCallback *ActionFunc=NULL, void *cookie=NULL );
	c3_ListBox(AUI_ERRCODE *retval, uint32 id, sint32 x, sint32 y, sint32 width, sint32 height,
							MBCHAR *pattern, sint32 bevelwidth = 0, sint32 beveltype = 0,
							ControlActionCallback *ActionFunc = NULL, void *cookie = NULL);

	~c3_ListBox();

	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	AUI_ERRCODE InitCommon(sint32 bevelWidth, sint32 bevelType );
	AUI_ERRCODE CreateRangersAndHeader( MBCHAR *ldlBlock = NULL );

	void Clear(void);

	virtual AUI_ERRCODE SortByColumn( sint32 column, BOOL ascending );
	virtual AUI_ERRCODE Draw(aui_Surface *surface = NULL, sint32 x = 0, sint32 y = 0);
	virtual AUI_ERRCODE DrawThis(aui_Surface *surface = NULL, sint32 x = 0, sint32 y = 0 );

	sint32 GetBevelWidth(void) { return m_bevelWidth; }
	sint32 GetBevelType(void) { return m_bevelType; }

	void ForceScroll(sint32 deltaX, sint32 deltaY);

	static c3_ListBox *ms_mouseFocusListBox;

	static c3_ListBox *GetMouseFocusListBox(void) { return ms_mouseFocusListBox; }
	static void SetMouseFocusListBox(c3_ListBox *box) { ms_mouseFocusListBox = box; }

protected:
	AUI_ERRCODE ReformatItemFromHeader(aui_Item *item);

	virtual void	MouseMoveOver(aui_MouseEvent * mouseData);
	virtual void	MouseMoveInside(aui_MouseEvent * mouseData);
	virtual void	MouseMoveOutside(aui_MouseEvent * mouseData);
	virtual void	MouseMoveAway(aui_MouseEvent * mouseData);

	virtual void	MouseLDragOver(aui_MouseEvent * mouseData);
//	virtual void	MouseLDragAway(aui_MouseEvent * mouseData);
	virtual void	MouseRDragOver(aui_MouseEvent * mouseData);
	virtual void	MouseRDragAway(aui_MouseEvent * mouseData);

private:
	sint32		m_bevelWidth;
	sint32		m_bevelType;
};

#endif
