










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
	aui_DirectUI() : aui_UI() {}
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

protected:
	LPDIRECTDRAWSURFACE m_lpdds; 
};


#endif 

#endif 
