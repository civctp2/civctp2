#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __aui_sdl__aui_sdlmouse_h__
#define __aui_sdl__aui_sdlmouse_h__ 1

#include "os/include/ctp2_config.h"

#if defined(__AUI_USE_SDL__)

#include "ui/aui_common/aui_mouse.h"
#include "ui/aui_sdl/aui_sdlinput.h"

class aui_SDLMouse : public aui_Mouse, public aui_SDLInput {
public:
	aui_SDLMouse(AUI_ERRCODE *retval, MBCHAR *ldlBlock,
	             BOOL useExclusiveMode = FALSE);
	virtual ~aui_SDLMouse();

protected:
	aui_SDLMouse();
	virtual AUI_ERRCODE GetInput();
};

typedef aui_SDLMouse aui_NativeMouse;

#endif // defined(__AUI_USE_SDL__)

#endif
