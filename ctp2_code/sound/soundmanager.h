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
// USE_SDL
// - Compile with sdl support instead of mss (define: civsound.h)
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
// - includes fixed for case sensitive filesystems
// - added sdl sound and cdrom support
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __SOUNDMANAGER_H__
#define __SOUNDMANAGER_H__

#include "civsound.h"
#include "pointerlist.h"
#include "gamesounds.h"

#if defined(USE_SDL)
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#endif

template <class T> class PointerList;

enum SOUNDTYPE {
	SOUNDTYPE_NONE = 0,

	SOUNDTYPE_SFX,
	SOUNDTYPE_VOICE,
	SOUNDTYPE_MUSIC,

	SOUNDTYPE_MAX
};

enum MUSICSTYLE {
	MUSICSTYLE_NONE = 0,

	MUSICSTYLE_PLAYLIST,
	MUSICSTYLE_RANDOM,
	MUSICSTYLE_USER,

	MUSICSTYLE_MAX
};

#define k_SM_MAX_SFXSOUNDS		4
#define k_SM_MAX_VOICESOUNDS	4

class SoundManager {
public:
	static void Initialize();
	static void Cleanup();

	SoundManager();
	~SoundManager();

	void InitSoundDriver();
	void CleanupSoundDriver();

	void InitRedbook();
	void CleanupRedbook();

	void DumpAllSounds();

	void ProcessRedbook(void);
	void Process(const uint32 &target_milliseconds, uint32 &used_milliseconds);

	void AddGameSound(const GAMESOUNDS &sound);
	void AddSound(const SOUNDTYPE &type, const uint32 &associatedObject,
                  const sint32 &soundID, sint32 x=0, sint32 y=0);
	void AddLoopingSound(const SOUNDTYPE &type,
                         const uint32 &associatedObject,
                         const sint32 &soundID, sint32 x=0, sint32 y=0);
	void TerminateLoopingSound(const SOUNDTYPE &type,
                               const uint32 &associatedObject);
	void TerminateAllLoopingSounds(const SOUNDTYPE &type);
	void TerminateSounds(const SOUNDTYPE &type);
	void TerminateAllSounds(void);

	void SetVolume(const SOUNDTYPE &type, const uint32 &volume);
	void SetMasterVolume(const uint32 &volume);
	CivSound *FindSound(const SOUNDTYPE &type,
                        const uint32 &associatedObject);
	CivSound *FindLoopingSound(const SOUNDTYPE &type,
                               const uint32 &associatedObject);
	void SetPosition(const SOUNDTYPE &type,
                     const uint32 &associatedObject,
                     const sint32 &x, const sint32 &y);

	void		StartMusic();
	void		StartMusic(const sint32 &trackNum);
	void		TerminateMusic();
	void		PickNextTrack();

	void		SetMusicStyle(const MUSICSTYLE &style);
	void		SetUserTrack(const sint32 &trackNum);
	void		SetPlayListPosition(const sint32 &pos);
	void		SetAutoRepeat(const BOOL &autoRepeat);
	const MUSICSTYLE GetMusicStyle(void) const;
	const sint32     GetUserTrack(void) const;
	const sint32     GetPlayListPosition(void) const;

	void             EnableMusic();
	void             DisableMusic();

	const BOOL       IsAutoRepeat() const;
	const BOOL       IsMusicEnabled() const;

	const sint32     GetLastTrack() const;
	void             SetLastTrack(const sint32 &track);

	void StupidPlaySound(const sint32 &soundID);
	void PlayManagedSound(const MBCHAR *fullFilename, const bool &bNoWait);
	void ReleaseSoundDriver();
	void ReacquireSoundDriver();

private:
	PointerList<CivSound>	*m_sfxSounds;
	PointerList<CivSound>	*m_voiceSounds;

	PointerList<CivSound>::Walker *m_soundWalker;

	uint32					m_sfxVolume;
	uint32					m_musicVolume;
	uint32					m_voiceVolume;

	uint32					m_oldRedbookVolume;

	BOOL					m_noSound;
	BOOL					m_usePlaySound;

#if defined(USE_SDL)
	BOOL					m_useOggTracks;
	Mix_Music				*m_oggTrack;
#else // USE_SDL
	HREDBOOK				m_redbook;
#endif // USE_SDL
	uint32					m_timeToCheckMusic;
	sint32					m_numTracks;
	sint32					m_curTrack;
	sint32					m_lastTrack;
	BOOL					m_musicEnabled;

	MUSICSTYLE				m_style;
	sint32					m_playListPosition;
	sint32					m_userTrack;
	BOOL					m_autoRepeat;

	BOOL					m_stopRedbookTemporarily;
};

extern SoundManager *g_soundManager;

#endif
