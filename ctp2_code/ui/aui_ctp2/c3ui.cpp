//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : User interface general resource handling
// Id           : $Id$
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
// __AUI_USE_SDL__
// Use SDL instead of DirectX.
// 
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added compiler option to use SDL as a replacement for DirectX.
// - Added option to register cleanup functions.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3ui.h"

#include "aui.h"
#include "aui_keyboard.h"
#include "aui_surface.h"

//----------------------------------------------------------------------------
//
// Name       : C3UI::C3UI
//
// Description: Constructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
C3UI::C3UI
(
	AUI_ERRCODE *retval,
	HINSTANCE hinst,
	HWND hwnd,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *ldlFilename,
	BOOL useExclusiveMode 
)
:
#if defined(__AUI_USE_SDL__)
	aui_SDLUI(retval, hinst, hwnd, width, height, bpp, ldlFilename, useExclusiveMode),
#else
	aui_DirectUI(retval, hinst, hwnd, width, height, bpp, ldlFilename, useExclusiveMode),
#endif
    m_cleanupActions    ()
{
	m_patternResource   = new aui_Resource<Pattern>;
	m_iconResource      = new aui_Resource<Icon>;
	m_pictureResource   = new aui_Resource<Picture>;
}

//----------------------------------------------------------------------------
//
// Name       : C3UI::~C3UI
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
C3UI::~C3UI()
{
    // Execute the registered cleanup functions.
    for 
    (
        std::list<UiCleanupCallback>::iterator p = m_cleanupActions.begin();
        p != m_cleanupActions.end();
        p = m_cleanupActions.erase(p)
    )
    {
        (*p)();
    }

    delete m_patternResource;
    delete m_iconResource;
    delete m_pictureResource;
}

//----------------------------------------------------------------------------
//
// Name       : C3UI::RegisterCleanup
//
// Description: Register a cleanup function for execution in the destructor.
//
// Parameters : callback    : the cleanup function
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void C3UI::RegisterCleanup(UiCleanupCallback callback)
{
    m_cleanupActions.push_front(callback);
}

//----------------------------------------------------------------------------
//
// Name       : C3UI::TopWindowIsNonBackground
//
// Description: Determine whether the top window is *not* in the background.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool    : the top window is *not* a background window
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool C3UI::TopWindowIsNonBackground(void) const
{
	return TopWindow()                                          &&
           (TopWindow()->Type() != AUI_WINDOW_TYPE_BACKGROUND)  &&
		   (TopWindow()->Type() != AUI_WINDOW_TYPE_SINKING); 
}