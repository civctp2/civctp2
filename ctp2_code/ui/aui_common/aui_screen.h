#ifndef __AUI_SCREEN_H__
#define __AUI_SCREEN_H__

#include "aui_base.h"
#include "tech_wllist.h"

class aui_Window;


class aui_Screen : public aui_Base
{
public:

	aui_Screen(
		AUI_ERRCODE *retval,
		uint32 id );
	virtual ~aui_Screen();

protected:
	aui_Screen() : aui_Base() {}
	AUI_ERRCODE InitCommon( uint32 id );

public:
	virtual AUI_ERRCODE Show( void );
	virtual AUI_ERRCODE Hide( void );

	AUI_ERRCODE	AddWindow( aui_Window *window, BOOL temp = FALSE );
	AUI_ERRCODE	RemoveWindow( uint32 windowId );
	aui_Window	*GetWindow( uint32 windowId );

protected:
	uint32 m_id;
	BOOL m_showing;
	tech_WLList<aui_Window *> *m_windowList;
	tech_WLList<aui_Window *> *m_tempWindowList;
};

#endif
