#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __KM_SCREEN_H__
#define __KM_SCREEN_H__

class KeyListItem;

#include "auitypes.h"       // AUI_ERRCODE
#include "c3_listitem.h"    // c3_ListItem
#include "ctp2_inttypes.h"  // sint32, uint32

class KeyListItem : public c3_ListItem
{
public:
	KeyListItem(AUI_ERRCODE *retval, sint32 index, uint32 keycode, MBCHAR *ldlBlock);

	virtual void    Update(void);
	void            UpdateKey(uint32 keycode);

	sint32	        GetKey(void) const      { return m_index; };
	sint32	        GetKeycode(void) const  { return m_keycode; };
    MBCHAR *        GetName(void) const     { return m_name; };

	virtual sint32  Compare(c3_ListItem *item2, uint32 column);

	static MBCHAR const * GetKeyFromKMScreen(uint32 keycode);

private:
	AUI_ERRCODE InitCommonLdl(sint32 index, uint32 keycode, MBCHAR *ldlBlock);

	MBCHAR *    m_name;
	sint32      m_index;
	uint32	    m_keycode;
};

sint32      km_screen_displayMyWindow();
sint32      km_screen_removeMyWindow(uint32 action);
void        km_screen_Cleanup(void);
sint32      km_screen_remapKey( WPARAM wParam, LPARAM lParam );

void        km_screen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void        km_screen_resetPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void        km_screen_switchPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

#endif
