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

class ctp2_Listbox;

#define k_CTP2_LISTBOX_LDL_BEVELWIDTH    "bevelwidth"
#define k_CTP2_LISTBOX_LDL_BEVELTYPE     "beveltype"
#define k_CTP2_LISTBOX_LDL_BORDER_LEFT   "borderleft"
#define k_CTP2_LISTBOX_LDL_BORDER_RIGHT  "borderright"
#define k_CTP2_LISTBOX_LDL_BORDER_TOP    "bordertop"
#define k_CTP2_LISTBOX_LDL_BORDER_BOTTOM "borderbottom"
#define k_CTP2_LISTBOX_LDL_BORDER_WIDTH  "borderwidth"
#define k_CTP2_LISTBOX_LDL_BORDER_HEIGHT "borderheight"

#include "aui_control.h"    // ControlActionCallback
#include "aui_listbox.h"    // aui_ListBox
#include "auitypes.h"       // AUI_ERRCODE
#include "c3types.h"        // MBCHAR
#include "ctp2_inttypes.h"  // sint32, uint32
#include "patternbase.h"    // PatternBase
#include "windows.h"        // BOOL, POINT
#include "ctp2_listitem.h"	// ctp2_ListItem

struct  aui_MouseEvent;
class   aui_Surface;
//class   ctp2_ListItem;
class   ctp2_MenuButton;

class ctp2_ListBox : public aui_ListBox, public PatternBase
{
public:
    ctp2_ListBox()
    :
        aui_ListBox     (),
        m_bevelWidth    (0),
        m_bevelType     (0),
        m_menuButton    (NULL)
        // m_borderOffset
    {};

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

	AUI_ERRCODE InitCommonLdl( const MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon(sint32 bevelWidth, sint32 bevelType );
	AUI_ERRCODE CreateRangersAndHeader( const MBCHAR *ldlBlock = NULL );

/// Clear the user data.
/// \remarks Does not clear the list itself.
    template <typename T>
    void ClearUserData(VOID_PARAMETER_FOR_TEMPLATE(T))
    {
        for (int i = 0; i < this->NumItems(); ++i)
        {
            ctp2_ListItem * item = static_cast<ctp2_ListItem *>(this->GetItemByIndex(i));
            if (item)
            {
                 T * info = static_cast<T *>(item->GetUserData());
                 delete info;
                 item->SetUserData(NULL);
            }
        } // for
    }

	void Clear(void);

	virtual AUI_ERRCODE SortByColumn( sint32 column, BOOL ascending );
	virtual AUI_ERRCODE Draw(aui_Surface *surface = NULL, sint32 x = 0, sint32 y = 0);
	virtual AUI_ERRCODE DrawThis(aui_Surface *surface = NULL, sint32 x = 0, sint32 y = 0 );

	sint32 GetBevelWidth(void) { return m_bevelWidth; }
	sint32 GetBevelType(void) { return m_bevelType; }

	sint32 GetMaxItemHeight() { return m_maxItemHeight; }
	sint32 GetMaxItemWidth() { return m_maxItemWidth; }
	void SetMenuButton(ctp2_MenuButton *butt) { m_menuButton = butt; }

	void GetDisplayRange(sint32 &top, sint32 &bottom);
	void EnsureItemVisible(sint32 index);

	static uint32		m_ctp2_listboxClassId;

	friend class ctp2_Menu;

protected:
	virtual sint32 WidthForItems() const {
		return aui_ListBox::WidthForItems() - m_borderOffset.left - m_borderOffset.right;
	}
	virtual sint32 HeightForItems() const {
		return aui_ListBox::HeightForItems() - m_borderOffset.top - m_borderOffset.bottom;
	}
	virtual AUI_ERRCODE DoneInstantiatingThis(const MBCHAR *ldlBlock);

	AUI_ERRCODE ReformatItemFromHeader(aui_Item *item);

	virtual void	MouseLDragAway(aui_MouseEvent * mouseData);

private:
	sint32		m_bevelWidth;
	sint32		m_bevelType;

	ctp2_MenuButton *m_menuButton;

	RECT m_borderOffset;
};

#endif
