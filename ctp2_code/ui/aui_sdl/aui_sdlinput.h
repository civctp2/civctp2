#ifndef __aui_sdl__aui_sdlinput__h__
#define __aui_sdl__aui_sdlinput__h__ 1

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#include "ctp2_config.h"

#if defined(__AUI_USE_SDL__)

#include "aui_input.h"
#include "aui_sdl.h"

class aui_SDLInput : public virtual aui_Input, public aui_SDL
{
public:
	
	aui_SDLInput(
		AUI_ERRCODE *retval,
		BOOL useExclusiveMode = FALSE );
	virtual ~aui_SDLInput();

protected:
	aui_SDLInput() {}
	AUI_ERRCODE InitCommon( BOOL useExclusiveMode );

public:
	
	virtual AUI_ERRCODE Acquire( void );
	virtual AUI_ERRCODE Unacquire( void );

};

#endif // defined(__AUI_USE_SDL__)

#endif
