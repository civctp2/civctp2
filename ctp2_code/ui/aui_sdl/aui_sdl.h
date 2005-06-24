#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __aui_sdl__aui_sdl__h__
#define __aui_sdl__aui_sdl__h__ 1

#include "ctp2_config.h"

#if defined(__AUI_USE_SDL__)

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

protected:
	BOOL					m_exclusiveMode;

private:
	static sint32			m_SDLRefCount;
	static uint32           m_SDLClassId;
};

#endif // defined(__AUI_USE_SDL__)

#endif
