
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __KM_SCREEN_H__
#define __KM_SCREEN_H__

#include "c3_listitem.h"

enum KM {
	KM_BASIC,
	KM_UNIT,
	KM_SCREEN,
	KM_MAP,
	KM_GAME,

	KM_MAX
};

enum KM_KEY {
	KM_KEY_NOOP,
	KM_KEY_MOVE_NORTH,

	KM_KEY_MAX
};

class KeyListItem : public c3_ListItem
{
public:
	
	
	KeyListItem(AUI_ERRCODE *retval, sint32 index, uint32 keycode, MBCHAR *ldlBlock);

	
	
	virtual void Update(void);

	void UpdateKey( uint32 keycode );

	
	MBCHAR	*GetName(void) { return m_name; }
	sint32	GetKey(void) { return m_index; }
	sint32	GetKeycode(void) { return m_keycode; }

	static MBCHAR *GetKeyFromKMScreen(uint32 keycode);

protected:
	KeyListItem() : c3_ListItem() {}

	
	
	AUI_ERRCODE InitCommonLdl(sint32 index, uint32 keycode, MBCHAR *ldlBlock);
	
public:
	
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	MBCHAR	*m_name;
	sint32	m_index;
	uint32	m_keycode;
};


sint32 km_screen_displayMyWindow();
sint32 km_screen_removeMyWindow(uint32 action);
AUI_ERRCODE km_screen_Initialize( void );
AUI_ERRCODE km_screen_Cleanup();
sint32 km_screen_loadKeyList( void );
sint32 km_screen_remapKey( WPARAM wParam );

void km_screen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void km_screen_resetPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void km_screen_switchPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

#endif
