#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __aui_sdl__aui_sdl__h__
#define __aui_sdl__aui_sdl__h__ 1

#include "ctp2_config.h"

#if defined(__AUI_USE_SDL__)

#include <SDL2/SDL.h>

class aui_SDL
{
public:

	aui_SDL() { m_SDLRefCount++; m_exclusiveMode = FALSE; }
	virtual ~aui_SDL();

protected:
	AUI_ERRCODE InitCommon( BOOL useExclusiveMode );

public:
	virtual BOOL IsThisA( uint32 classId )
	{
		return classId == m_SDLClassId;
	}

	BOOL GetExclusiveMode(void) { return m_exclusiveMode; }
	static SDL_Surface *DD() { return m_lpdd; }

protected:
	BOOL			m_exclusiveMode;
	static SDL_Surface *	m_lpdd;

private:
	static sint32		m_SDLRefCount;
protected:
	static uint32           m_SDLClassId;
};

typedef aui_SDL aui_Native;

#endif // defined(__AUI_USE_SDL__)

#endif
