#include "ctp2_config.h"
#include "c3.h"

#ifdef __AUI_USE_SDL__

#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_sdl.h"

SDL_Surface *aui_SDL::m_lpdd = 0;
uint32 aui_SDL::m_SDLClassId = aui_UniqueId();
sint32 aui_SDL::m_SDLRefCount = 0;

AUI_ERRCODE aui_SDL::InitCommon(BOOL useExclusiveMode)
{
	m_exclusiveMode = useExclusiveMode;

#if 0
	int rc = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD |
	                  SDL_INIT_AUDIO | SDL_INIT_TIMER);
#elif 0
	int rc = SDL_Init(SDL_INIT_EVENTTHREAD |
	                  SDL_INIT_AUDIO | SDL_INIT_TIMER);

	if (0 != rc) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return AUI_ERRCODE_CREATEFAILED;
	}
#endif
	SDL_ShowCursor(SDL_DISABLE);

	// enable only a handfull of events
	SDL_EventState(SDL_IGNORE, SDL_ALLEVENTS);
	SDL_EventState(SDL_ENABLE, SDL_MOUSEEVENTMASK);
	SDL_EventState(SDL_ENABLE, SDL_KEYDOWNMASK);
	SDL_EventState(SDL_ENABLE, SDL_QUITMASK);

	SDL_EnableUNICODE(1);

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
