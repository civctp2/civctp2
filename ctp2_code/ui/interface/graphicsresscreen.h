#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __GRAPHICSRESSCREEN_H__
#define __GRAPHICSRESSCREEN_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// #include <>

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class ScreenResListItem;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "auitypes.h"           // AUI_ERRCODE
#include "c3_listitem.h"        // c3_ListItem
#include "ctp2_inttypes.h"      // sint32, uint32

class   aui_Control;
struct  CTPDisplayMode;

//----------------------------------------------------------------------------
// Declarations
//----------------------------------------------------------------------------

sint32 graphicsresscreen_displayMyWindow();
sint32 graphicsresscreen_removeMyWindow(uint32 action);
AUI_ERRCODE graphicsresscreen_Initialize( void );
AUI_ERRCODE graphicsresscreen_Cleanup();

void graphicsscreen_unitSpeedSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
void graphicsresscreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

class ScreenResListItem : public c3_ListItem
{
public:

	ScreenResListItem(AUI_ERRCODE *retval, CTPDisplayMode *mode, const MBCHAR *ldlBlock);


	virtual void Update(void) {}

	CTPDisplayMode *GetDisplayMode(void) { return m_mode; }
protected:
	ScreenResListItem() : c3_ListItem() {}
	AUI_ERRCODE InitCommonLdl(CTPDisplayMode *mode, const MBCHAR *ldlBlock);

private:
	CTPDisplayMode *m_mode;
};

#endif
