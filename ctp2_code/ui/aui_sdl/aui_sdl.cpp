#include "ctp2_config.h"
#include "c3.h"

#ifdef __AUI_USE_SDL__

#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_sdl.h"

SDL_Surface *aui_SDL::m_lpdd = 0;
uint32 aui_SDL::m_SDLClassId = aui_UniqueId();
sint32 aui_SDL::m_SDLRefCount = 0;

// SKIP_SDL2_EVENT_ISSUES: TODO: handle SDL2 new keyboard-events especially for unicode characters:
//    SDL_TEXTEDITING,            /**< Keyboard text editing (composition) */
//    SDL_TEXTINPUT,              /**< Keyboard text input */
//    SDL_KEYMAPCHANGED           /**< Keymap changed due to a system event such as an
//                                     input language or keyboard layout change.
//                                */

int FilterEvents(void* userData, SDL_Event *event) {
	switch(event->type) {
		// Quit event
		case SDL_QUIT:
		// Keyboard events
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		// Mouse events
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEWHEEL:
			return 1;
		default:
			return 0;
	}
}

AUI_ERRCODE aui_SDL::InitCommon(BOOL useExclusiveMode)
{
	m_exclusiveMode = useExclusiveMode;

//	SDL Init is done in CivApp::InitializeApp
/*	int rc = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD |
	                  SDL_INIT_AUDIO | SDL_INIT_TIMER);

	if (0 != rc) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return AUI_ERRCODE_CREATEFAILED;
	}
*/

	SDL_SetEventFilter(FilterEvents, NULL);

	return AUI_ERRCODE_OK;
}

aui_SDL::~aui_SDL()
{
	if (! --m_SDLRefCount) {
		SDL_Quit();
		m_lpdd = 0;
	}
}

#endif
