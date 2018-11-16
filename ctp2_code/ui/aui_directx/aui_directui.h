//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface DirectX component
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
// - Initialised pointer in default constructor to prevent destructor crash.
//
//----------------------------------------------------------------------------

#ifndef __AUI_DIRECTUI_H__
#define __AUI_DIRECTUI_H__

#ifdef __AUI_USE_DIRECTX__

#include "aui_ui.h"
#include "aui_directx.h"




class aui_DirectUI : public aui_UI, public aui_DirectX
{
public:

	aui_DirectUI(
		AUI_ERRCODE *retval,
		HINSTANCE hinst,
		HWND hwnd,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *ldlFilename = NULL,
		BOOL useExclusiveMode = FALSE );
	virtual ~aui_DirectUI();

protected:
	aui_DirectUI()
    :   aui_UI              (),
        aui_DirectX         (),
        m_lpdds             (NULL),
        m_isCoinitialized   (false)
    { ; };

	AUI_ERRCODE InitCommon( void );

public:

	virtual AUI_ERRCODE AltTabOut( void );
	virtual AUI_ERRCODE AltTabIn( void );
	AUI_ERRCODE CreateNativeScreen( BOOL useExclusiveMode );
	AUI_ERRCODE DestroyNativeScreen(void);
	AUI_ERRCODE TearDownMouse(void);
	AUI_ERRCODE RestoreMouse(void);

	void SetWidth(sint32 width) { m_width = width; }
	void SetHeight(sint32 height) { m_height = height; }

protected:
	LPDIRECTDRAWSURFACE m_lpdds;
    bool                m_isCoinitialized;
};

typedef aui_DirectUI aui_NativeUI;

#endif

#endif
