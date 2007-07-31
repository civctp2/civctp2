

#pragma once
#ifndef __CIVSOUND_H__
#define __CIVSOUND_H__

#ifdef STREAM_TYPE
#error
#undef STREAM_TYPE
#endif

#include "mss.h"

class CivSound {
public:
	CivSound(uint32 associatedObject, sint32 soundID);
	~CivSound();

	uint32		GetAssociatedObject(void) const { return m_associatedObject; }
	HAUDIO		GetHAudio(void) const { return m_hAudio; }
	MBCHAR		*GetSoundFilename(void)  { return m_soundFilename; }
	BOOL		IsPlaying(void) const { return m_isPlaying; }
	void		SetIsPlaying(BOOL is) { m_isPlaying = is; }
	sint32		GetSoundID(void) const { return m_soundID; }
	void		SetIsLooping(BOOL looping) { m_isLooping = looping; }
	BOOL		IsLooping(void) { return m_isLooping; }
	sint32		GetVolume(void) { return m_volume; }
	void		SetVolume(sint32 volume);
private:
	HAUDIO			m_hAudio;
	uint32 			m_associatedObject;
	MBCHAR			m_soundFilename[_MAX_PATH];
	BOOL			m_isPlaying;
	BOOL			m_isLooping;
	sint32			m_soundID;
	sint32			m_volume;
	void			*m_dataptr;
    sint32          m_datasize;
};

#endif
