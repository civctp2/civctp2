#include "c3.h"
#include "aui_sdlmoviemanager.h"

#if defined(__AUI_USE_SDL__)

#include "aui_sdlmovie.h"

#if defined(USE_SDL_FFMPEG)
#include <SDL_mutex.h>
extern "C" {
	#include <libavformat/avformat.h>
}

static int lockmgr(void **mtx, enum AVLockOp op)
{
	switch(op) {
		case AV_LOCK_CREATE:
			*mtx = SDL_CreateMutex();
			if(!*mtx) {
				av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
				return 1;
			}
			return 0;
		case AV_LOCK_OBTAIN:
			return !!SDL_LockMutex((SDL_mutex*) *mtx);
		case AV_LOCK_RELEASE:
			return !!SDL_UnlockMutex((SDL_mutex*) *mtx);
		case AV_LOCK_DESTROY:
			SDL_DestroyMutex((SDL_mutex*) *mtx);
			return 0;
	}
	return 1;
}
#endif // USE_SDL_FFMPEG

aui_SDLMovieManager::aui_SDLMovieManager(SDL_Renderer *renderer, SDL_Texture *background, const int windowWidth,
		const int windowHeight) :
	aui_MovieManager(false),
	m_renderer(renderer),
	m_background(background),
	m_windowWidth(windowWidth),
	m_windowHeight(windowHeight)
{
	Assert(m_renderer);
	Assert(m_background);
	m_movieResource = new aui_Resource<aui_SDLMovie>();

#if defined(USE_SDL_FFMPEG)
	av_register_all();
	if (av_lockmgr_register(lockmgr)) {
		av_log(NULL, AV_LOG_FATAL, "Could not initialize lock manager!\n");
	}
#endif // USE_SDL_FFMPEG
}

aui_SDLMovieManager::~aui_SDLMovieManager() {
	delete m_movieResource;
#if defined(USE_SDL_FFMPEG)
	av_lockmgr_register(NULL);
#endif // USE_SDL_FFMPEG
}

aui_Movie* aui_SDLMovieManager::Load(const MBCHAR *filename, C3DIR dir) {
	aui_SDLMovie *movie = m_movieResource->Load(filename, dir);
	movie->SetContext(m_renderer, m_background, m_windowWidth, m_windowHeight);
	return movie;
}

#endif // defined(__AUI_USE_SDL__)
