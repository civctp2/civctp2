#ifndef __aui_sdl__aui_sdlkeyboard_h__
#define __aui_sdl__aui_sdlkeyboard_h__ 1

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#include "ctp2_config.h"

#if defined(__AUI_USE_SDL__)

#include "aui_keyboard.h"
#include "aui_sdlinput.h"

class aui_SDLKeyboard : public aui_Keyboard, public aui_SDLInput {
public:
	aui_SDLKeyboard(AUI_ERRCODE *retval);
	virtual ~aui_SDLKeyboard() {}

protected:
	aui_SDLKeyboard() {}
	AUI_ERRCODE createSDLKeyboard();
	void convertSDLKeyboardEvent(SDL_KeyboardEvent &sdlevent,
	                             aui_KeyboardEvent &auievent);

public:
	virtual AUI_ERRCODE Acquire();
	virtual AUI_ERRCODE Unacquire();
	virtual AUI_ERRCODE GetInput();
};

#endif // defined(__AUI_USE_SDL__)

#endif
