//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : General declarations
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// _DEBUG
// - Generate debug version
//
// _MSC_VER
// - Use Microsoft C++ extensions when set.
//
// USE_SDL
// - Use SDL for sound, cdrom, ... (TODO)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
// - includes fixed for case sensitive filesystems
// - sdl sound and cdrom
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CIVSOUND_H__
#define __CIVSOUND_H__

#ifdef STREAM_TYPE
#error
#undef STREAM_TYPE
#endif

#if defined(USE_SDL)
#include <SDL2/SDL_mixer.h>
#else
#include "mss.h"
#endif

class CivSound {
public:
	CivSound(const uint32 &associatedObject, const sint32 &soundID);
	~CivSound();

	const uint32 GetAssociatedObject() const;
#if !defined(USE_SDL)
	HAUDIO       GetHAudio() const;
#else
	Mix_Chunk    *GetAudio() const;
    void         SetChannel(const int &channel);
    const int    GetChannel() const;
#endif
	MBCHAR       *GetSoundFilename();
	const BOOL   IsPlaying() const;
	void         SetIsPlaying(const BOOL &is);
	const sint32 GetSoundID() const;
	void         SetIsLooping(const BOOL &looping);
	const BOOL   IsLooping();
	const sint32 GetVolume();
	void         SetVolume(const sint32 &volume);

private:
#if !defined(USE_SDL)
	HAUDIO			m_hAudio;
#else
	Mix_Chunk      *m_Audio;
    int             m_Channel;
#endif
	uint32 			m_associatedObject;
	MBCHAR			m_soundFilename[_MAX_PATH];
	BOOL			m_isPlaying;
	BOOL			m_isLooping;
	sint32			m_soundID;
	sint32			m_volume;
	void			*m_dataptr;
    size_t          m_datasize;
};

#endif
