#include "ctp2_config.h"
#include "c3.h"

#if defined(__AUI_USE_SDL__)

#include <SDL.h>
#include "aui_sdlkeyboard.h"
#include "aui_ui.h"
#include "c3ui.h"

extern C3UI		*g_c3ui;

#include "civapp.h"
extern CivApp	*g_civApp;


aui_SDLKeyboard::aui_SDLKeyboard(
	AUI_ERRCODE *retval )
	:
	aui_Keyboard( retval ),
	aui_Input( retval ),
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
	sint32 numElements = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_KEYDOWN | SDL_KEYUP);

	if (-1 == numElements)
		return AUI_ERRCODE_NODIRECTINPUTDEVICE;

	if (0 == numElements)
		return AUI_ERRCODE_NOINPUT;

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
						// TODO: scancode to keycode abstraction
						g_civApp->BeginKeyboardScrolling(event.key.keysym.scancode);
					} else {
						// TODO: scancode to keycode abstraction
						g_civApp->StopKeyboardScrolling(event.key.keysym.scancode);
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
	// TODO: scancode to keycode abstraction
	auievent.key = sdlevent.keysym.scancode;
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
