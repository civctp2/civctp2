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
// - USE SDL for sound, cdrom, ... TODO
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

#include "c3.h"

#include "civsound.h"
#include "SoundRecord.h"
#include "prjfile.h"

#if defined(USE_SDL)
#include <SDL2/SDL_rwops.h>
#endif

extern ProjectFile  *g_SoundPF;

CivSound::CivSound(const uint32 &associatedObject, const sint32 &soundID)
#if !defined(USE_SDL)
  : m_hAudio(0),
#else
  : m_Audio(0), m_Channel(-1),
#endif
    m_associatedObject(associatedObject)

{
    const char *fname;
	if(soundID < 0)
		fname = 0;
	else
		fname = g_theSoundDB->Get(soundID)->GetValue();

	m_soundID = soundID;
    m_isPlaying = FALSE;
    m_isLooping = FALSE;

    if (0 == fname) {
        m_soundFilename[0] = 0;
        m_dataptr = NULL;
        m_datasize = 0;
        return;
    }

    strcpy(m_soundFilename, fname);

    size_t      l_dataSize = 0;
    m_dataptr   = g_SoundPF->getData(m_soundFilename, l_dataSize);
    m_datasize  = static_cast<sint32>(l_dataSize);

#if !defined(USE_SDL)
	m_hAudio = AIL_quick_load_mem(m_dataptr, m_datasize);
#else
# if 0
    // Argh, audio format mismatch!!!
	m_Audio = Mix_QuickLoad_RAW((Uint8 *) m_dataptr, (Uint32) m_datasize);
# else
    m_Audio = Mix_LoadWAV_RW(SDL_RWFromMem(m_dataptr, m_datasize), 1);
# endif
#endif
}

CivSound::~CivSound()
{
#if !defined(USE_SDL)
    if (m_hAudio) {
        AIL_quick_unload(m_hAudio);
	}
#else
	if (m_Audio) {
		Mix_FreeChunk(m_Audio);
	}
#endif

    if (m_dataptr) {
        g_SoundPF->freeData(m_dataptr);
	}
}

const uint32
CivSound::GetAssociatedObject() const
{
	return m_associatedObject;
}

#if defined(USE_SDL)
Mix_Chunk *
CivSound::GetAudio() const
{
	return m_Audio;
}

const int
CivSound::GetChannel() const
{
    return m_Channel;
}

#else
HAUDIO
CivSound::GetHAudio() const
{
	return m_hAudio;
}
#endif

MBCHAR
*CivSound::GetSoundFilename()
{
	return m_soundFilename;
}

const sint32
CivSound::GetSoundID() const
{
	return m_soundID;
}

const sint32
CivSound::GetVolume()
{
	return m_volume;
}

const BOOL
CivSound::IsLooping()
{
	return m_isLooping;
}

const BOOL
CivSound::IsPlaying() const
{
	return m_isPlaying;
}

#if defined(USE_SDL)
void
CivSound::SetChannel(const int &channel)
{
    m_Channel = channel;
}
#endif

void
CivSound::SetIsLooping(const BOOL &looping)
{
	m_isLooping = looping;
}

void
CivSound::SetIsPlaying(const BOOL &is)
{
	m_isPlaying = is;
}

void
CivSound::SetVolume(const sint32 &volume)
{
#if !defined(USE_SDL)
    if (0 == m_hAudio) {
#else
    if (0 == m_Audio) {
#endif
        return;
    }

	// Assume max volume is 10...
	sint32 scaledVolume = (sint32)((double)volume * 12.7);

#if defined(USE_SDL)
	if (scaledVolume > MIX_MAX_VOLUME) {
		Mix_VolumeChunk(m_Audio, MIX_MAX_VOLUME);
	} else {
		Mix_VolumeChunk(m_Audio, (Uint8) scaledVolume);
	}
#else
	AIL_quick_set_volume(m_hAudio, scaledVolume, 64);
#endif
	m_volume = volume;
}
