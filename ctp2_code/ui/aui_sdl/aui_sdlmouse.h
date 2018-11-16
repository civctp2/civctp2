#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __aui_sdl__aui_sdlmouse_h__
#define __aui_sdl__aui_sdlmouse_h__ 1

#include "ctp2_config.h"

#if defined(__AUI_USE_SDL__)

#include "aui_mouse.h"
#include "aui_sdlinput.h"

class aui_SDLMouse : public aui_Mouse, public aui_SDLInput {
public:
	aui_SDLMouse(AUI_ERRCODE *retval, MBCHAR *ldlBlock,
	             BOOL useExclusiveMode = FALSE);
	virtual ~aui_SDLMouse();

protected:
	aui_SDLMouse();
	virtual AUI_ERRCODE GetInput();
};

#endif // defined(__AUI_USE_SDL__)

#endif
