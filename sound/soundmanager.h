

#pragma once
#ifndef __SOUNDMANAGER_H__
#define __SOUNDMANAGER_H__

#include "CivSound.h"
#include "PointerList.h"

template <class T> class PointerList;

enum GAMESOUNDS;

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
	static void Initialize(void);
	static void Cleanup(void);

	SoundManager();
	~SoundManager();

	void InitSoundDriver(void);
	void CleanupSoundDriver(void);

	void InitRedbook(void);
	void CleanupRedbook(void);

	void DumpAllSounds(void);

	void ConvertCoordinates(sint32 x, sint32 y, sint32 *soundX, sint32 *soundY, sint32 *soundZ);
	void ProcessRedbook(void);
	void Process(const uint32 target_milliseconds, uint32 &used_milliseconds);
	
	void AddGameSound(GAMESOUNDS sound);
	void AddSound(SOUNDTYPE type, uint32 associatedObject, sint32 soundID, sint32 x=0, sint32 y=0);
	void AddLoopingSound(SOUNDTYPE type, uint32 associatedObject, sint32 soundID, sint32 x=0, sint32 y=0);
	void TerminateLoopingSound(SOUNDTYPE type, uint32 associatedObject);
	void TerminateAllLoopingSounds(SOUNDTYPE type);
	void TerminateSounds(SOUNDTYPE type);
	void TerminateAllSounds(void);

	void SetVolume(SOUNDTYPE type, uint32 volume);
	void SetMasterVolume(uint32 volume);
	CivSound *FindSound(SOUNDTYPE type, uint32 associatedObject);
	CivSound *FindLoopingSound(SOUNDTYPE type, uint32 associatedObject);
	void SetPosition(SOUNDTYPE type, uint32 associatedObject, sint32 x, sint32 y);

	
	void		StartMusic(void) { StartMusic(m_curTrack); }
	void		StartMusic(sint32 trackNum);
	void		TerminateMusic(void);
	void		PickNextTrack(void);

	void		SetMusicStyle(MUSICSTYLE style) { m_style = style; PickNextTrack();}
	void		SetUserTrack(sint32 trackNum) { m_userTrack = trackNum;  PickNextTrack();}
	void		SetPlayListPosition(sint32 pos) { m_playListPosition = pos;  PickNextTrack();}
	void		SetAutoRepeat(BOOL autoRepeat) { m_autoRepeat = autoRepeat;  PickNextTrack();}
	MUSICSTYLE	GetMusicStyle(void) { return m_style; }
	sint32		GetUserTrack(void) { return m_userTrack; }
	sint32		GetPlayListPosition(void) { return m_playListPosition; }

	void		EnableMusic(void) { m_musicEnabled = TRUE; }
	void		DisableMusic(void) { m_musicEnabled = FALSE; TerminateMusic();}

	BOOL		IsAutoRepeat( void ) const { return m_autoRepeat; }
	BOOL		IsMusicEnabled( void ) const { return m_musicEnabled; }

	sint32		GetLastTrack(void) const { return m_lastTrack; }
	void		SetLastTrack(sint32 track) { m_lastTrack = track; }

	
	void StupidPlaySound(sint32 soundID);

	void ReleaseSoundDriver(void);
	void ReacquireSoundDriver(void);

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

	
	HREDBOOK				m_redbook;		
	uint32					m_timeToCheckCD;
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
