//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : General declarations
// Id           : $Id$
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
// - Compile with sdl support instead of mss (define: civsound.h)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
// - Includes fixed for case sensitive filesystems
// - Added sdl sound and cdrom support
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "soundmanager.h"
#include "pointerlist.h"
#include "profileDB.h"
#include "SoundRecord.h"
#include "CivPaths.h"
#include "c3files.h"
#include "PlayListDB.h"
#include "gamesounds.h"
#include <iostream>
#include <cctype>

extern HWND			gHwnd;
extern ProfileDB	*g_theProfileDB;
extern CivPaths		*g_civPaths;
extern PlayListDB	*g_thePlayListDB;

SoundManager		*g_soundManager     = NULL;

namespace
{
    uint32 const    k_CHECK_MUSIC_PERIOD = 4000;     // [ms]
    uint32 const    SLIDER_FULL         = 10;

#if !defined(USE_SDL)
    S32             s_masterVolume;
#endif // USE_SDL
    sint32          s_startTrack        = 1;        // skip CD data track
}

void SoundManager::Initialize()
{
    delete g_soundManager;
    g_soundManager = new SoundManager();
}

void SoundManager::Cleanup()
{
    delete g_soundManager;
    g_soundManager = NULL;
}

SoundManager::SoundManager()
:
		m_sfxSounds                 (NULL),
		m_voiceSounds               (NULL),
		m_soundWalker               (NULL),
		m_sfxVolume                 (SLIDER_FULL),
		m_musicVolume               (SLIDER_FULL),
		m_voiceVolume               (SLIDER_FULL),
		m_oldRedbookVolume          (0),
		m_noSound                   (false),
		m_usePlaySound              (false),
#if defined(USE_SDL)
		m_useOggTracks				(false),
		m_oggTrack					(0),
#else // USE_SDL
    	m_redbook                   (0),
#endif // USE_SDL
		m_timeToCheckMusic          (0),
		m_numTracks                 (0),
		m_curTrack                  (0),
		m_lastTrack                 (0),
		m_musicEnabled              (false),
		m_style                     (MUSICSTYLE_PLAYLIST),
		m_playListPosition          (0),
		m_userTrack                 (0),
		m_autoRepeat                (true),
		m_stopRedbookTemporarily    (false)
{
    if (g_theProfileDB)
    {
		m_sfxVolume     = static_cast<uint32>(g_theProfileDB->GetSFXVolume());
		m_voiceVolume   = static_cast<uint32>(g_theProfileDB->GetVoiceVolume());
		m_musicVolume   = static_cast<uint32>(g_theProfileDB->GetMusicVolume());
    }

	m_sfxSounds     = new PointerList<CivSound>;
	m_voiceSounds   = new PointerList<CivSound>;
	m_soundWalker   = new PointerList<CivSound>::Walker;

	InitSoundDriver();
}

SoundManager::~SoundManager()
{
    DumpAllSounds();
    CleanupSoundDriver();

    delete m_sfxSounds;
    delete m_voiceSounds;
    delete m_soundWalker;
}

void SoundManager::DumpAllSounds()
{
	if (m_sfxSounds) {
		m_sfxSounds->DeleteAll();
	}
	if (m_voiceSounds) {
		m_voiceSounds->DeleteAll();
	}
}

void SoundManager::InitSoundDriver()
{
#if defined(USE_SDL)
	int     use_digital     = 1;
	int     use_MIDI        = 0;
	int     output_rate     = 22050;	// 22khz @ 16 Bit stereo
	Uint16  output_format   = AUDIO_S16SYS;
	int     output_channels = 2;
	int     errcode         = SDL_InitSubSystem(SDL_INIT_AUDIO);

    if (errcode < 0)
    {
        // char *err = SDL_GetError(); cerr << "SDL Init failed:" << err << endl;
    }
    else
    {
        errcode = Mix_OpenAudio(output_rate, output_format, output_channels, 256);
        if (errcode < 0)
        {
            // char *err = SDL_GetError(); cerr << "Opening mixer failed:" << err << endl;
        }
    }

    m_noSound = (errcode < 0);
#else // USE_SDL
	S32		use_digital     = 1;
	S32		use_MIDI        = 0;
	S32		output_rate     = 22050;	// 22khz @ 16 Bit stereo
	S32		output_bits     = 16;
	S32		output_channels = 2;

	m_noSound = !AIL_quick_startup(use_digital, use_MIDI, output_rate, output_bits, output_channels);
#endif // USE_SDL

	InitRedbook();

	SetVolume(SOUNDTYPE_SFX,   m_sfxVolume);
	SetVolume(SOUNDTYPE_VOICE, m_voiceVolume);
	SetVolume(SOUNDTYPE_MUSIC, m_musicVolume);
}

void SoundManager::CleanupSoundDriver()
{
	if (!m_usePlaySound)
    {
		CleanupRedbook();

#if defined(USE_SDL)
        if (!m_noSound) {
            Mix_CloseAudio();
        }

        SDL_QuitSubSystem(SDL_INIT_AUDIO);
#else // USE_SDL
		AIL_quick_shutdown();
#endif // USE_SDL
	}
}

void SoundManager::InitRedbook()
{
#if defined(USE_SDL)
	// check if Ogg Tracks are present first
	if( !m_useOggTracks) {
		if(CI_FileExists("music/Track02.ogg")) {
			m_useOggTracks = true;
			printf("Detected Ogg Music track\n");
		}
	}
#else // USE_SDL
	if (!m_redbook)
	{
		m_redbook = AIL_redbook_open_drive(c3files_GetCtpCdId());
	}
#endif // USE_SDL
}

void SoundManager::CleanupRedbook()
{
#if defined(USE_SDL)
	if (m_oggTrack) {
		Mix_FreeMusic(m_oggTrack);
		m_oggTrack = NULL;
	}
#else // USE_SDL
    if (m_redbook) {
		AIL_redbook_stop(m_redbook);
		AIL_redbook_close(m_redbook);
		m_redbook = NULL;
	}
#endif // USE_SDL
}

void SoundManager::ProcessRedbook()
{
	if (!g_theProfileDB->IsUseRedbookAudio()) return;

	if (!m_musicEnabled) return;

	if (GetTickCount() > m_timeToCheckMusic) {

#if defined(USE_SDL)
		if(m_useOggTracks) {
			if(!Mix_PlayingMusic()) {
				if (m_curTrack != -1)
					PickNextTrack();

				if (m_curTrack != -1 && !m_stopRedbookTemporarily)
					StartMusic(m_curTrack);
			}
		}
#else // USE_SDL
        U32 status;
		if (m_redbook) {
			status = AIL_redbook_status(m_redbook);
			switch (status) {
			case REDBOOK_ERROR:
			case REDBOOK_PLAYING:
			case REDBOOK_PAUSED:
				break;
			case REDBOOK_STOPPED:
				if (m_curTrack != -1)
					PickNextTrack();

				if (m_curTrack != -1 && !m_stopRedbookTemporarily)
					StartMusic(m_curTrack);
				break;
			}
		}
#endif // USE_SDL
		m_timeToCheckMusic = GetTickCount() + k_CHECK_MUSIC_PERIOD;
	}
}

void SoundManager::Process(const uint32 &target_milliseconds,
                           uint32 &used_milliseconds)
{
	CivSound						*sound;

    sint32 start_time_ms = GetTickCount();

    if ((m_noSound) ||(m_usePlaySound)) {
        used_milliseconds = GetTickCount() - start_time_ms;
        return;
    }

	if (m_sfxSounds->GetCount() > 0) {
		m_soundWalker->SetList(m_sfxSounds);
		while (m_soundWalker->IsValid()) {
			sound = m_soundWalker->GetObj();
			Assert(sound);
			if (!sound) continue;

			if (sound->IsPlaying()) {
#if defined(USE_SDL)
				if (!Mix_Playing(sound->GetChannel())) {
#else // USE_SDL
				if (AIL_quick_status(sound->GetHAudio()) == QSTAT_DONE) {
#endif // USE_SDL
					m_soundWalker->Remove();

					delete sound;
				} else {
					m_soundWalker->Next();
				}
			}
		}
	}

	if (m_voiceSounds->GetCount() > 0) {

		m_soundWalker->SetList(m_voiceSounds);
		while (m_soundWalker->IsValid()) {
			sound = m_soundWalker->GetObj();
			Assert(sound);
			if (!sound) continue;

			if (sound->IsPlaying()) {
#if defined(USE_SDL)
				if ((-1 == sound->GetChannel()) ||
					(!Mix_Playing(sound->GetChannel()))) {
#else // USE_SDL
				if (AIL_quick_status(sound->GetHAudio()) == QSTAT_DONE) {
#endif // USE_SDL
					m_soundWalker->Remove();
					delete sound;
				} else {
					m_soundWalker->Next();
				}
			}
		}
	}

	ProcessRedbook();

    used_milliseconds = GetTickCount() - start_time_ms;
}

bool FindSoundinList(PointerList<CivSound> * sndList, sint32 soundID)
{
    for
    (
	    PointerList<CivSound>::Walker walk(sndList);
	    walk.IsValid();
        walk.Next()
    )
    {
		if (walk.GetObj()->GetSoundID() == soundID)
		{
			return true;
		}
	}

	return false;
}

void
SoundManager::AddGameSound(const GAMESOUNDS &sound)
{
	sint32 id = gamesounds_GetGameSoundID(sound);
	AddSound(SOUNDTYPE_SFX, 0, id, 0, 0);
}

void
SoundManager::AddSound(const SOUNDTYPE &type,
                       const uint32 &associatedObject,
                       const sint32 &soundID, sint32 x, sint32 y)
{
	if (m_noSound) return;

	if (m_usePlaySound)
    {
		StupidPlaySound(soundID);
		return;
	}

    bool        found   = false;
    CivSound *  sound   = new CivSound(associatedObject, soundID);

	switch (type)
    {
    default:
//  case SOUNDTYPE_MUSIC:
        break;

	case SOUNDTYPE_SFX:
		sound->SetVolume(m_sfxVolume);
		found = FindSoundinList(m_sfxSounds, soundID);
		if (!found)
		{
			m_sfxSounds->AddTail(sound);
		}
		break;

	case SOUNDTYPE_VOICE:
		sound->SetVolume(m_voiceVolume);
		found = FindSoundinList(m_voiceSounds, soundID);
		if (!found)
		{
			m_voiceSounds->AddTail(sound);
		}
		break;
	}

	if (found)
    {
        // This sound was already being played
        delete sound;
    }
    else
	{
#if defined(USE_SDL)
		int channel = Mix_PlayChannel(-1, sound->GetAudio(), 0);
		sound->SetChannel(channel);
#else // USE_SDL
		AIL_quick_play(sound->GetHAudio(), 1);
#endif // USE_SDL
		sound->SetIsPlaying(TRUE);
	}
}

void
SoundManager::AddLoopingSound(const SOUNDTYPE &type,
                              const uint32 &associatedObject,
                              const sint32 &soundID, sint32 x, sint32 y)
{
	if (m_noSound) return;

	if (m_usePlaySound)
    {
		StupidPlaySound(soundID);
        return;
    }

	CivSound *  existingSound   = FindLoopingSound(type, associatedObject);

	if (existingSound && (existingSound->GetSoundID() == soundID)) return;

	CivSound *  sound           = new CivSound(associatedObject, soundID);

	switch (type)
    {
    default:
        delete sound;
        return;

	case SOUNDTYPE_SFX:
		sound->SetVolume(m_sfxVolume);
		m_sfxSounds->AddTail(sound);
		break;

	case SOUNDTYPE_VOICE:
		sound->SetVolume(m_voiceVolume);
		m_voiceSounds->AddTail(sound);
		break;
	}

#if defined(USE_SDL)
	int channel = Mix_PlayChannel(-1, sound->GetAudio(), -1);
	sound->SetChannel(channel);
#else // USE_SDL
	AIL_quick_play(sound->GetHAudio(), 0);
#endif // USE_SDL

	sound->SetIsLooping(TRUE);
	sound->SetIsPlaying(TRUE);
}

void
SoundManager::TerminateLoopingSound(const SOUNDTYPE &type,
                                    const uint32 &associatedObject)
{
	CivSound *existingSound = FindLoopingSound(type, associatedObject);

	if (!existingSound) return;

#if defined(USE_SDL)
	int channel = existingSound->GetChannel();
	if (channel >= 0) {
		Mix_HaltChannel(channel);
	}
	existingSound->SetChannel(-1);
#else
	AIL_quick_halt(existingSound->GetHAudio());
#endif
}

void
SoundManager::TerminateAllLoopingSounds(const SOUNDTYPE &type)
{
	PointerList<CivSound>::PointerListNode *node = NULL;

	switch (type) {
	case SOUNDTYPE_SFX:
			node = m_sfxSounds->GetHeadNode();
		break;
	case SOUNDTYPE_VOICE:
			node = m_voiceSounds->GetHeadNode();
		break;
	}

	CivSound	*sound;

	while (node) {
		sound = node->GetObj();
		if (sound && sound->IsLooping()) {
#if defined(USE_SDL)
			int channel = sound->GetChannel();
			if (channel >= 0) {
				Mix_HaltChannel(channel);
			}
			sound->SetChannel(-1);
#else // USE_SDL
			AIL_quick_halt(sound->GetHAudio());
#endif // USE_SDL
		}
		node = node->GetNext();
	}
}

void
SoundManager::TerminateSounds(const SOUNDTYPE &type)
{
	PointerList<CivSound>::PointerListNode *node = NULL;

	switch (type) {
	case SOUNDTYPE_SFX:
			node = m_sfxSounds->GetHeadNode();
		break;
	case SOUNDTYPE_VOICE:
			node = m_voiceSounds->GetHeadNode();
		break;
	}

	CivSound	*sound;

	while (node) {
		sound = node->GetObj();
		if (sound) {
#if defined(USE_SDL)
			int channel = sound->GetChannel();
            if (channel >= 0) {
                Mix_HaltChannel(channel);
            }
#else // USE_SDL
			AIL_quick_halt(sound->GetHAudio());
#endif // USE_SDL
		}
		node = node->GetNext();
	}
}

void
SoundManager::TerminateAllSounds()
{
	TerminateSounds(SOUNDTYPE_SFX);
	TerminateSounds(SOUNDTYPE_VOICE);
}

void
SoundManager::SetVolume(const SOUNDTYPE &type, const uint32 &volume)
{
	CivSound *sound;

	Assert(volume >= 0);
	Assert(volume <= 10);

	if (m_noSound) return;

	switch (type) {
	case SOUNDTYPE_SFX:
		m_sfxVolume = volume;

		m_soundWalker->SetList(m_sfxSounds);
		while (m_soundWalker->IsValid()) {
			sound = m_soundWalker->GetObj();
			sound->SetVolume(volume);
			m_soundWalker->Next();
		}
		break;
	case SOUNDTYPE_VOICE:
		m_voiceVolume = volume;

		m_soundWalker->SetList(m_voiceSounds);
		while (m_soundWalker->IsValid()) {
			sound = m_soundWalker->GetObj();
			sound->SetVolume(volume);
			m_soundWalker->Next();
		}
		break;
	case SOUNDTYPE_MUSIC:
		m_musicVolume = volume;
#if defined(USE_SDL)
			if (m_useOggTracks) {
			// Assume max volume is 10...
			sint32 scaledVolume = (sint32)((double)volume * 12.7);
			if (scaledVolume > MIX_MAX_VOLUME) {
				Mix_VolumeMusic(MIX_MAX_VOLUME);
			} else {
				Mix_VolumeMusic(scaledVolume);
			}
		}
#else // USE_SDL
		if (m_redbook)
			AIL_redbook_set_volume(m_redbook, (sint32)((double)volume * 12.7));
#endif // USE_SDL
		break;
	}
}

void
SoundManager::SetMasterVolume(const uint32 &volume)
{
	if (m_noSound) return;

	if (m_usePlaySound) return;

	CivSound *sound;

	m_soundWalker->SetList(m_sfxSounds);
	while (m_soundWalker->IsValid()) {
		sound = m_soundWalker->GetObj();
		sound->SetVolume(volume);
		m_soundWalker->Next();
	}
	m_sfxVolume = volume;

	m_soundWalker->SetList(m_voiceSounds);
	while (m_soundWalker->IsValid()) {
		sound = m_soundWalker->GetObj();
		sound->SetVolume(volume);
		m_soundWalker->Next();
	}
	m_voiceVolume = volume;
}

void
SoundManager::DisableMusic()
{
    m_musicEnabled = FALSE;
    TerminateMusic();
}

void
SoundManager::EnableMusic()
{
    m_musicEnabled = TRUE;
}

CivSound
*SoundManager::FindLoopingSound(const SOUNDTYPE &type,
                                const uint32 &associatedObject)
{
	switch (type) {
	case SOUNDTYPE_SFX:
		m_soundWalker->SetList(m_sfxSounds);
		break;
	case SOUNDTYPE_VOICE:
		m_soundWalker->SetList(m_voiceSounds);
		break;
	}

	while (m_soundWalker->IsValid()) {
		if (m_soundWalker->GetObj()->IsLooping() && m_soundWalker->GetObj()->GetAssociatedObject() == associatedObject) {
			CivSound *sound = m_soundWalker->GetObj();
			return sound;
		}
		m_soundWalker->Next();
	}

	return NULL;
}

CivSound
*SoundManager::FindSound(const SOUNDTYPE &type,
                         const uint32 &associatedObject)
{
	switch (type) {
	case SOUNDTYPE_SFX:
		m_soundWalker->SetList(m_sfxSounds);
		break;
	case SOUNDTYPE_VOICE:
		m_soundWalker->SetList(m_voiceSounds);
		break;
	}

	while (m_soundWalker->IsValid()) {
		if (m_soundWalker->GetObj()->GetAssociatedObject() == associatedObject) {
			CivSound *sound = m_soundWalker->GetObj();
			return sound;
		}
		m_soundWalker->Next();
	}

	return NULL;
}

const MUSICSTYLE
SoundManager::GetMusicStyle() const
{
    return m_style;
}

const sint32
SoundManager::GetPlayListPosition() const
{
    return m_playListPosition;
}

const sint32
SoundManager::GetLastTrack() const
{
    return m_lastTrack;
}

const sint32
SoundManager::GetUserTrack() const
{
    return m_userTrack;
}

const BOOL
SoundManager::IsAutoRepeat() const
{
    return m_autoRepeat;
}

const BOOL
SoundManager::IsMusicEnabled() const
{
    return m_musicEnabled;
}

void
SoundManager::SetAutoRepeat(const BOOL &autoRepeat)
{
    m_autoRepeat = autoRepeat;
    PickNextTrack();
}

void
SoundManager::SetLastTrack(const sint32 &track)
{
    m_lastTrack = track;
}

void
SoundManager::SetMusicStyle(const MUSICSTYLE &style)
{
    m_style = style;
    PickNextTrack();
}

void
SoundManager::SetPlayListPosition(const sint32 &pos)
{
    m_playListPosition = pos;
    PickNextTrack();
}

void
SoundManager::SetPosition(const SOUNDTYPE &type,
                          const uint32 &associatedObject,
                          const sint32 &x, const sint32 &y)
{
	PointerList<CivSound>::PointerListNode *node = NULL;

	sint32 volume = 0;

	switch (type)
    {
	case SOUNDTYPE_SFX:
		node    = m_sfxSounds->GetHeadNode();
		volume  = m_sfxVolume;
		break;
	case SOUNDTYPE_VOICE:
		node    = m_voiceSounds->GetHeadNode();
		volume  = m_voiceVolume;
		break;
	}

	while (node)
    {
	    CivSound *  sound = node->GetObj();

		if (sound && (sound->GetAssociatedObject() == associatedObject))
        {
#if defined(USE_SDL)
            Mix_Chunk * myChunk = sound->GetAudio();
            // Why set the volume again?
#else // USE_SDL
            sint32 panValue = 64;
			HAUDIO hAudio   = sound->GetHAudio();
            if (hAudio)
            {
                AIL_quick_set_volume(hAudio, volume, panValue);
            }
#endif // USE_SDL
		}

		node = node->GetNext();
	}
}

void SoundManager::SetUserTrack(const sint32 &trackNum)
{
    m_userTrack = trackNum;
    PickNextTrack();
}

void SoundManager::StartMusic()
{
    StartMusic(m_curTrack);
}

void SoundManager::StartMusic(const sint32 &InTrackNum)
{
	m_stopRedbookTemporarily = FALSE;

	if (!g_theProfileDB->IsUseRedbookAudio()) return;
#if defined(USE_SDL)
	if (!m_useOggTracks) return;
#else
	if (!c3files_HasCD()) return;
#endif

	if (m_noSound) return;

	if (m_usePlaySound) return;

	if (m_curTrack == -1) return;

#if defined(USE_SDL)
	if(m_useOggTracks) {
		char buf[60];
		if(!m_numTracks) {
			// first search number of tracks
			sint32 numTracks = 1;
			do {
				numTracks++;
				sprintf(buf, "music/Track%02d.ogg", numTracks);
			} while(CI_FileExists(buf));
			m_numTracks = numTracks-1; // start at 2
		}
		// setting up
		if (m_numTracks <= s_startTrack) return;
		
		sint32 trackNum = InTrackNum;
		if (trackNum < 0) trackNum = 0;
		if (trackNum > m_numTracks) trackNum = m_numTracks;
	
		m_curTrack = trackNum;

		sprintf(buf, "music/Track%02d.ogg", m_curTrack+1);
		// clean previous if there
		if(m_oggTrack) {
			Mix_FreeMusic(m_oggTrack);
			m_oggTrack = NULL;
		}
		m_oggTrack = Mix_LoadMUS(CI_FixName(buf));
		if(m_oggTrack)
			Mix_PlayMusic(m_oggTrack, 1);
		else
			printf("Error, music track %s not found\n", buf);
		
	}
#else
    if (!m_redbook) {
        return;
    }

	U32 status = AIL_redbook_status(m_redbook);

    if (status == REDBOOK_ERROR) {
        return;
    }

	if (AIL_redbook_track(m_redbook)) {
		AIL_redbook_stop(m_redbook);
	}

	sint32 const numTracks = AIL_redbook_tracks(m_redbook);
	if (numTracks <= s_startTrack) return;

	m_numTracks = numTracks;

	sint32 trackNum = InTrackNum;
	if (trackNum < 0) trackNum = 0;
	if (trackNum > m_numTracks) trackNum = m_numTracks;

	m_curTrack = trackNum;

	U32 start;
    U32 end;
	AIL_redbook_track_info(m_redbook, trackNum, &start, &end);

    // Why?
	TerminateAllSounds();

	AIL_redbook_play(m_redbook, start, end);
#endif // USE_SDL
}

void SoundManager::TerminateMusic(void)
{
	if (!g_theProfileDB->IsUseRedbookAudio()) return;
#if !defined(USE_SDL)
	if (!c3files_HasCD()) return;
#endif

	if (m_noSound) return;

	if (m_usePlaySound) return;

#if defined(USE_SDL)
	if (m_useOggTracks) {
		if(m_oggTrack) {
			Mix_FreeMusic(m_oggTrack);
			m_oggTrack = NULL;
		}
	}
#else
	if (!m_redbook) return;
#endif

	m_stopRedbookTemporarily = TRUE;

#if !defined(USE_SDL)
	if (AIL_redbook_track(m_redbook)) {
		AIL_redbook_stop(m_redbook);
	}
#endif
}

void SoundManager::PickNextTrack(void)
{
	switch (m_style)
    {
    default:
//	case MUSICSTYLE_PLAYLIST:
		++m_playListPosition;
		if (m_playListPosition >= g_thePlayListDB->GetNumSongs())
        {
			m_playListPosition = 0;

			if (!m_autoRepeat)
            {
				m_curTrack = -1;
				return;
			}
		}
		m_curTrack = s_startTrack + g_thePlayListDB->GetSong(m_playListPosition);
		break;

	case MUSICSTYLE_RANDOM:
		m_curTrack = (1 + s_startTrack) + rand() % (m_numTracks - (1 + s_startTrack));
		break;

	case MUSICSTYLE_USER:
		m_curTrack = (1 + s_startTrack) + m_userTrack;
		if (!m_autoRepeat && m_curTrack >= m_lastTrack)
        {
			m_curTrack = -1;
            return;
		}
		break;
	}

	m_lastTrack = m_curTrack;
}

// This is StupidPlaySound, because you cannot name it PlaySound,
// otherwise you get intro trouble the define from windows.
void SoundManager::StupidPlaySound(const sint32 &soundID)
{
    SoundRecord const * soundRecord = g_theSoundDB->Get(soundID);
    char const *        soundValue  = soundRecord ? soundRecord->GetValue() : NULL;

	if (soundValue && (strlen(soundValue) > 0))
	{
		MBCHAR		fullPath[_MAX_PATH];
		fullPath[0] = 0;

		g_civPaths->FindFile(C3DIR_SOUNDS, soundValue, fullPath);

		PlayManagedSound(fullPath, true);
	}
}

void SoundManager::PlayManagedSound(const MBCHAR *fullFilename, const bool &bNoWait)
{
#if !defined(USE_SDL)
	PlaySound(fullFilename, NULL,
		(SND_ASYNC | SND_FILENAME | (bNoWait ? SND_NOWAIT : 0)));
#else // USE_SDL
	std::cerr << "SoundManager::PlayManagedSound("
		<< fullFilename
		<< ") called." << std::endl;
#endif // USE_SDL
}

void SoundManager::ReleaseSoundDriver()
{
	if (m_usePlaySound) return;
	if (m_noSound) return;

	TerminateAllSounds();

#if !defined(USE_SDL)
	HDIGDRIVER		dig;
	HMDIDRIVER		mdi;
	HDLSDEVICE		dls;
	AIL_quick_handles(&dig, &mdi, &dls);

	s_masterVolume = AIL_digital_master_volume(dig);
	AIL_set_digital_master_volume(dig, 0);

	S32 err = AIL_digital_handle_release(dig);
	Assert(err);
#endif // USE_SDL
}

void SoundManager::ReacquireSoundDriver()
{
	if (m_usePlaySound) return;
	if (m_noSound) return;

#if !defined(USE_SDL)
	HDIGDRIVER		dig;
	HMDIDRIVER		mdi;
	HDLSDEVICE		dls;
	AIL_quick_handles(&dig, &mdi, &dls);

	S32 err = AIL_digital_handle_reacquire(dig);
	Assert(err);

	AIL_set_digital_master_volume(dig, s_masterVolume);
#endif // USE_SDL
}
