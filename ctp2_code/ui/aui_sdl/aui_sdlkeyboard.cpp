#include "ctp2_config.h"
#include "c3.h"

#if defined(__AUI_USE_SDL__)

#include <SDL2/SDL.h>
#include "aui_sdlkeyboard.h"
#include "aui_ui.h"
#include "c3ui.h"

extern C3UI		*g_c3ui;

#include "civapp.h"
extern CivApp	*g_civApp;

// We have to have a secondary keyboard event queue from which
// we extract events here.  This is because keyboard events are
// handled in two places.  This queue is filled by the main
// game loop in CivMain in civ3_main.cpp
std::queue<SDL_Event> g_secondaryKeyboardEventQueue;
// Then we need a mutex to allow us to thread-safely access the queue
// (Actually I don't think we do, now I understand the main game loop
// better, but I'll leave it in here to be on the safe side).
// This is created in civ3_main.cpp just before the main loop, and
// destroyed in AtExitProc() (also in civ3_main.cpp).
SDL_mutex* g_secondaryKeyboardEventQueueMutex = NULL;

aui_SDLKeyboard::aui_SDLKeyboard(
	AUI_ERRCODE *retval )
	:
	aui_Input(),
	aui_Keyboard(),
	aui_SDLInput( retval, FALSE )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = createSDLKeyboard();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

AUI_ERRCODE aui_SDLKeyboard::createSDLKeyboard( void )
{
	// TODO: SDL_Init()
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_SDLKeyboard::GetInput( void )
{
	SDL_Event event;
	BOOL gotEvent = FALSE;

	if (g_secondaryKeyboardEventQueueMutex != NULL) {
		if (-1==SDL_LockMutex(g_secondaryKeyboardEventQueueMutex)) {
			fprintf(stderr, "[aui_SDLKeyboard::GetInput] SDL_LockMutex failed: %s\n", SDL_GetError());
			return AUI_ERRCODE_NODIRECTINPUTDEVICE;
		}
		if (!g_secondaryKeyboardEventQueue.empty()) {
			gotEvent = TRUE;
			event = g_secondaryKeyboardEventQueue.front();
			g_secondaryKeyboardEventQueue.pop();
		}

		if (-1==SDL_UnlockMutex(g_secondaryKeyboardEventQueueMutex)) {
			fprintf(stderr, "[aui_SDLKeyboard::GetInput] SDL_UnlockMutex failed: %s\n", SDL_GetError());
			return AUI_ERRCODE_NODIRECTINPUTDEVICE;
		}
	}

	if (!gotEvent) {
		return AUI_ERRCODE_NOINPUT;
	}

	m_data.time = SDL_GetTicks();
	switch (event.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
				case SDLK_LSHIFT:
					if (g_c3ui->TheMouse()) {
						if (event.key.state & SDL_PRESSED) {
							g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() | k_MOUSE_EVENT_FLAG_LSHIFT);
						} else {
							g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() & ~k_MOUSE_EVENT_FLAG_LSHIFT);
						}
					}
					return AUI_ERRCODE_OK;
				case SDLK_RSHIFT:
					if (g_c3ui->TheMouse()) {
						if (event.key.state & SDL_PRESSED) {
							g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() | k_MOUSE_EVENT_FLAG_RSHIFT);
						} else {
							g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() & ~k_MOUSE_EVENT_FLAG_RSHIFT);
						}
					}
					return AUI_ERRCODE_OK;
				case SDLK_LCTRL:
					if (g_c3ui->TheMouse()) {
						if (event.key.state & SDL_PRESSED) {
							g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() | k_MOUSE_EVENT_FLAG_LCONTROL);
						} else {
							g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() & ~k_MOUSE_EVENT_FLAG_LCONTROL);
						}
					}
					return AUI_ERRCODE_OK;
				case SDLK_RCTRL:
					if (g_c3ui->TheMouse()) {
						if (event.key.state & SDL_PRESSED) {
							g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() | k_MOUSE_EVENT_FLAG_RCONTROL);
						} else {
							g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() & ~k_MOUSE_EVENT_FLAG_RCONTROL);
						}
					}
					return AUI_ERRCODE_OK;
				case SDLK_RETURN:
#ifdef _DEBUG
					extern BOOL commandMode;
					if (commandMode)
						return AUI_ERRCODE_OK;
#endif
					break;
				case SDLK_UP:
				case SDLK_DOWN:
				case SDLK_LEFT:
				case SDLK_RIGHT:
					if (event.key.state & SDL_PRESSED) {
						g_civApp->BeginKeyboardScrolling(convertSDLKey(event.key.keysym));
					} else {
						g_civApp->StopKeyboardScrolling(convertSDLKey(event.key.keysym));
					}
					break;
			}
			convertSDLKeyboardEvent(event.key, m_data);
			break;
		default:
			Assert(true);
	}

	return AUI_ERRCODE_OK;
}

void aui_SDLKeyboard::convertSDLKeyboardEvent(SDL_KeyboardEvent &sdlevent,
                                      aui_KeyboardEvent &auievent)
{
	auievent.down = (sdlevent.state & SDL_PRESSED) ? TRUE : FALSE;
	auievent.key = convertSDLKey(sdlevent.keysym);
	/*printf("convertSDLKeyboardEvent(): %08x %d %08x %c\n", auievent.key,
		(sdlevent.state & SDL_PRESSED) ? TRUE : FALSE, sdlevent.keysym.sym, (sdlevent.keysym.sym>' ' && sdlevent.keysym.sym<127)?sdlevent.keysym.sym:' ');*/
}

uint32 aui_SDLKeyboard::convertSDLKey(SDL_Keysym keysym)
{
	switch (keysym.sym) {
		case SDLK_ESCAPE:
			return AUI_KEYBOARD_KEY_ESCAPE;
		case SDLK_RETURN:
			return AUI_KEYBOARD_KEY_RETURN;
		case SDLK_SPACE:
			return AUI_KEYBOARD_KEY_SPACE;
		case SDLK_TAB:
			return AUI_KEYBOARD_KEY_TAB;
		case SDLK_UP:
			return AUI_KEYBOARD_KEY_UPARROW;
		case SDLK_DOWN:
			return AUI_KEYBOARD_KEY_DOWNARROW;
		case SDLK_LEFT:
			return AUI_KEYBOARD_KEY_LEFTARROW;
		case SDLK_RIGHT:
			return AUI_KEYBOARD_KEY_RIGHTARROW;
		default:
			return AUI_KEYBOARD_KEY_INVALID;
	}
}

AUI_ERRCODE aui_SDLKeyboard::Acquire()
{
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_SDLKeyboard::Unacquire()
{
	return AUI_ERRCODE_OK;
}

#endif // defined(__AUI_USE_SDL__)
