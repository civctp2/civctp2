#ifndef __aui_sdl__aui_sdlkeyboard_h__
#define __aui_sdl__aui_sdlkeyboard_h__ 1

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#include "ctp2_config.h"

#if defined(__AUI_USE_SDL__)

#include <queue>

#include "aui_keyboard.h"
#include "aui_sdlinput.h"

// See aui_sdlkeyboard.cpp to understand this queue/mutex
extern std::queue<SDL_Event> g_secondaryKeyboardEventQueue;
extern SDL_mutex* g_secondaryKeyboardEventQueueMutex;

class aui_SDLKeyboard : public aui_Keyboard, public aui_SDLInput {
public:
	aui_SDLKeyboard(AUI_ERRCODE *retval);
	virtual ~aui_SDLKeyboard() {}

protected:
	aui_SDLKeyboard() {}
	AUI_ERRCODE createSDLKeyboard();
	void convertSDLKeyboardEvent(SDL_KeyboardEvent &sdlevent,
	                             aui_KeyboardEvent &auievent);
	uint32 convertSDLKey(SDL_Keysym keysym);

public:
	virtual AUI_ERRCODE Acquire();
	virtual AUI_ERRCODE Unacquire();
	virtual AUI_ERRCODE GetInput();
};

typedef aui_SDLKeyboard aui_NativeKeyboard;

#endif // defined(__AUI_USE_SDL__)

#endif
