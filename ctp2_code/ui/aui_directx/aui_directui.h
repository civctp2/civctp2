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

//Definitions from obsolete reftime.h
const LONGLONG MILLISECONDS = (1000);            // 10 ^ 3
const LONGLONG NANOSECONDS = (1000000000);       // 10 ^ 9
const LONGLONG UNITS = (NANOSECONDS / 100);      // 10 ^ 7

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
		const MBCHAR *ldlFilename = NULL,
		BOOL useExclusiveMode = FALSE );
	virtual ~aui_DirectUI();

protected:
	aui_DirectUI()
	:   aui_UI              (),
	    aui_DirectX         (),
	    m_lpdds             (NULL),
	    m_back              (NULL),
	    m_isCoinitialized   (false)
	{ ; };

	AUI_ERRCODE InitCommon( void );

public:

	virtual AUI_ERRCODE AltTabOut( void );
	virtual AUI_ERRCODE AltTabIn( void );
	AUI_ERRCODE CreateDirectScreen( BOOL useExclusiveMode );
	AUI_ERRCODE DestroyDirectScreen(void);
	AUI_ERRCODE TearDownMouse(void);
	AUI_ERRCODE RestoreMouse(void);

	void SetWidth(sint32 width) { m_width = width; }
	void SetHeight(sint32 height) { m_height = height; }

	aui_MovieManager* CreateMovieManager( void );
protected:
	LPDIRECTDRAWSURFACE m_lpdds;
	LPDIRECTDRAWSURFACE m_back;
	bool                m_isCoinitialized;
};

typedef aui_DirectUI aui_NativeUI;

#endif

#endif
