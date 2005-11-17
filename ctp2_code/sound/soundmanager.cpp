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
#include <cctype>

extern HWND			gHwnd;
extern ProfileDB	*g_theProfileDB;
extern CivPaths		*g_civPaths;
extern PlayListDB	*g_thePlayListDB;

SoundManager		*g_soundManager     = NULL;

namespace
{
    uint32 const    k_CHECK_CD_PERIOD	= 4000;     // [ms]
    uint32 const    SLIDER_FULL         = 10;

#if !defined(USE_SDL)
    S32             s_masterVolume;
#endif
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
#if !defined(USE_SDL)
    m_redbook                   (0),
#else
    m_cdrom                     (0),
    m_SDLInitFlags              (SDL_INIT_NOPARACHUTE),
#endif
    m_timeToCheckCD             (0),
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

#if defined(USE_SDL) && defined(_DEBUG)
    // Do not remove this or debugging won't work... :(
    m_SDLInitFlags |= SDL_INIT_NOPARACHUTE;
#endif

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
	int     errcode         = SDL_Init(SDL_INIT_AUDIO | m_SDLInitFlags);

    if (errcode < 0) 
    {
        // char *err = SDL_GetError(); cerr << "SDL Init failed:" << err << endl;
    }
    else
    {
        errcode = Mix_OpenAudio(output_rate, output_format, output_channels, 8192);
        if (errcode < 0) 
        {
            // char *err = SDL_GetError(); cerr << "Opening mixer failed:" << err << endl;
        }
    } 

    m_noSound = (errcode < 0);
#else // !USE_SDL
	S32		use_digital     = 1;
	S32		use_MIDI        = 0;
	S32		output_rate     = 22050;	// 22khz @ 16 Bit stereo
	S32		output_bits     = 16;
	S32		output_channels = 2;

	m_noSound = !AIL_quick_startup
            (use_digital, use_MIDI, output_rate, output_bits, output_channels);
#endif // defined(USE_SDL)

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

        // SDL_Quit() -> civ_main.cpp:AtExitProc()
#else // !USE_SDL
		AIL_quick_shutdown();
#endif // USE_SDL
	}
}

void SoundManager::InitRedbook()
{
#if defined(USE_SDL)
    if (!m_cdrom) {
        int errcode = SDL_Init(SDL_INIT_CDROM | m_SDLInitFlags);

        Assert(0 == errcode);
        if (errcode < 0) {
            return;
        }

        int numDrives = SDL_CDNumDrives();
        Assert(numDrives >= 0);

        int drive = -1;
        int i = 0;
        // Hack: We don't have the num of the SDL drive stored,
        //       so we search for the drive with the drive letter stored
        MBCHAR driveLetter = toupper(c3files_GetCtpCdId());
        while ((i < numDrives) && (-1 == drive)) {
            const char *cd_name = SDL_CDName(i);
            if (cd_name) {
                if (toupper(cd_name[0]) == driveLetter) {
                    drive = i;
                }
            }
            i++;
        }

        // No drive match?!
        if (drive < 0) {
            return;
        }
        m_cdrom = SDL_CDOpen(drive);
        Assert(m_cdrom != 0);
        // No control structur?
        if (m_cdrom) 
        {
            CDstatus status = SDL_CDStatus(m_cdrom);
        }
    }
#else // !USE_SDL
	if (!m_redbook) 
	{
		m_redbook = AIL_redbook_open_drive(c3files_GetCtpCdId());
	}
#endif // USE_SDL
}

void SoundManager::CleanupRedbook()
{
#if defined(USE_SDL)
    if (m_cdrom) {
        SDL_CDClose(m_cdrom);
        m_cdrom = 0;
    }
#else
    if (m_redbook) {
		AIL_redbook_stop(m_redbook);
		AIL_redbook_close(m_redbook);
		m_redbook = NULL;
	}
#endif
}


void SoundManager::ProcessRedbook()
{
	if (!g_theProfileDB->IsUseRedbookAudio()) return;

	if (!m_musicEnabled) return;

	if (GetTickCount() > m_timeToCheckCD) {
#if defined(USE_SDL)
        CDstatus status;
        if (m_cdrom) {
            status = SDL_CDStatus(m_cdrom);
#else
        U32 status;
		if (m_redbook) {
			status = AIL_redbook_status(m_redbook);
#endif
			switch (status) {
#if !defined(USE_SDL)
			case REDBOOK_ERROR:
#else
            case CD_TRAYEMPTY:
                break;
            case CD_ERROR:
#endif
				break;
#if !defined(USE_SDL)
			case REDBOOK_PLAYING:
#else
            case CD_PLAYING:
#endif
				break;
#if !defined(USE_SDL)
			case REDBOOK_PAUSED:
#else
            case CD_PAUSED:
#endif
				break;
#if !defined(USE_SDL)
			case REDBOOK_STOPPED:
#else
            case CD_STOPPED:
#endif			
				if (m_curTrack != -1) 
					PickNextTrack();
				
				if (m_curTrack != -1 && !m_stopRedbookTemporarily) 
					StartMusic(m_curTrack);
				break;
			}
		}

		m_timeToCheckCD = GetTickCount() + k_CHECK_CD_PERIOD;
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
#if !defined(USE_SDL)
				if (AIL_quick_status(sound->GetHAudio()) == QSTAT_DONE) {
#else
                if (!Mix_Playing(sound->GetChannel())) {
#endif
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
#if !defined(USE_SDL)
				if (AIL_quick_status(sound->GetHAudio()) == QSTAT_DONE) {
#else
                if ((-1 == sound->GetChannel()) ||
                    (!Mix_Playing(sound->GetChannel()))) {
#endif
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


BOOL FindSoundinList(PointerList<CivSound> *sndList, CivSound *sound)
{
	
	PointerList<CivSound>::Walker walk(sndList);
	while(walk.IsValid()) {
		if (walk.GetObj()->GetSoundID() == sound->GetSoundID())
		{
			return TRUE;
		}
		walk.Next();
	}
	return FALSE;
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
	CivSound	*sound;
	BOOL		found = FALSE;

	if (m_noSound) return;

	if (m_usePlaySound) {
		StupidPlaySound(soundID);

		return;
	}
	
	sound = new CivSound(associatedObject, soundID);
	
	switch (type) {
	case SOUNDTYPE_SFX:
		sound->SetVolume(m_sfxVolume);
		found = FindSoundinList(m_sfxSounds, sound);
		if (!found)
		{
			m_sfxSounds->AddTail(sound);
		}
		break;
	case SOUNDTYPE_VOICE:
		sound->SetVolume(m_voiceVolume);
		found = FindSoundinList(m_voiceSounds, sound);
		if (!found)
		{
			m_voiceSounds->AddTail(sound);
		}
		break;
	}

	if (!found)
	{
#if !defined(USE_SDL)
		AIL_quick_play(sound->GetHAudio(), 1);	
#else
        int channel = Mix_PlayChannel(-1, sound->GetAudio(), 0);
        sound->SetChannel(channel);
#endif
		sound->SetIsPlaying(TRUE);
	} else {
		delete sound;
	}
}

void
SoundManager::AddLoopingSound(const SOUNDTYPE &type,
                              const uint32 &associatedObject,
                              const sint32 &soundID, sint32 x, sint32 y)
{
	CivSound	*sound;
	CivSound	*existingSound = FindLoopingSound(type, associatedObject);

	if (m_noSound) return;

	if (m_usePlaySound) {
		StupidPlaySound(soundID);

		return;
	}

	
	if (existingSound) {
		if (existingSound->GetSoundID() == soundID)
			return;
	}

	
	sound = new CivSound(associatedObject, soundID);

	
	switch (type) {
	case SOUNDTYPE_SFX:
		sound->SetVolume(m_sfxVolume);
		m_sfxSounds->AddTail(sound);
		break;
	case SOUNDTYPE_VOICE:
		sound->SetVolume(m_voiceVolume);
		m_voiceSounds->AddTail(sound);
		break;
	}

#if !defined(USE_SDL)
	AIL_quick_play(sound->GetHAudio(), 0);	
#else
    int channel = Mix_PlayChannel(-1, sound->GetAudio(), -1);
    sound->SetChannel(channel);
#endif

	sound->SetIsLooping(TRUE);
	sound->SetIsPlaying(TRUE);
}

void
SoundManager::TerminateLoopingSound(const SOUNDTYPE &type,
                                    const uint32 &associatedObject)
{
	CivSound *existingSound = FindLoopingSound(type, associatedObject);

	if (!existingSound) return;

#if !defined(USE_SDL)
	AIL_quick_halt(existingSound->GetHAudio());
#else
    int channel = existingSound->GetChannel();
    if (channel >= 0) {
        Mix_HaltChannel(channel);
    }
    existingSound->SetChannel(-1);
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
#if !defined(USE_SDL)
			AIL_quick_halt(sound->GetHAudio());
#else
            int channel = sound->GetChannel();
            if (channel >= 0) {
                Mix_HaltChannel(channel);
            }
            sound->SetChannel(-1);
#endif
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
#if !defined(USE_SDL)
			AIL_quick_halt(sound->GetHAudio());
#else
            int channel = sound->GetChannel();
            if (channel >= 0) {
                Mix_HaltChannel(channel);
            }
#endif
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
#if !defined(USE_SDL)
		if (m_redbook)
			AIL_redbook_set_volume(m_redbook, (sint32)((double)volume * 12.7));
#else
        if (m_cdrom) {
            // TODO: found nothing in reference
        }   
#endif
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
#else
            sint32 panValue = 64;
			HAUDIO hAudio   = sound->GetHAudio();
            if (hAudio) 
            {
                AIL_quick_set_volume(hAudio, volume, panValue);
            }
#endif
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

	if (!g_theProfileDB->IsUseRedbookAudio() || !c3files_HasCD()) return;

	if (m_noSound) return;

	if (m_usePlaySound) return;

	if (m_curTrack == -1) return;

#if defined(USE_SDL)
    if (!m_cdrom) {
        return;
    }

    CDstatus status = SDL_CDStatus(m_cdrom);

    if ((CD_ERROR == status) || (!CD_INDRIVE(status))) {
        return;
    }

	sint32 const numTracks = m_cdrom->numtracks;
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
#endif

	if (numTracks <= s_startTrack) return;

	m_numTracks = numTracks;
	
	sint32 trackNum = InTrackNum;
	if (trackNum < 0) trackNum = 0;
	if (trackNum > m_numTracks) trackNum = m_numTracks;

	m_curTrack = trackNum;

#if defined(USE_SDL)
    SDL_CDPlayTracks(m_cdrom, trackNum, 0, 1, 0);
#else
	U32 start;
    U32 end;
	AIL_redbook_track_info(m_redbook, trackNum, &start, &end);

    // Why?
	TerminateAllSounds();

	AIL_redbook_play(m_redbook, start, end);
#endif
}

void SoundManager::TerminateMusic(void)
{
	if (!g_theProfileDB->IsUseRedbookAudio() || !c3files_HasCD()) return;

	if (m_noSound) return;

	if (m_usePlaySound) return;

#if !defined(USE_SDL)
	if (!m_redbook) return;
#else
    if (!m_cdrom) return;
#endif

	
	m_stopRedbookTemporarily = TRUE;

#if !defined(USE_SDL)
	if (AIL_redbook_track(m_redbook)) {		
		AIL_redbook_stop(m_redbook);
	}
#else
    CDstatus status = SDL_CDStatus(m_cdrom);

    if (CD_PLAYING == status) {
        SDL_CDStop(m_cdrom);
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

void SoundManager::StupidPlaySound(const sint32 &soundID)
{
    SoundRecord const * soundRecord = g_theSoundDB->Get(soundID);
    char const *        soundValue  = soundRecord ? soundRecord->GetValue() : NULL;

	if (soundValue && (strlen(soundValue) > 0))
    {
		MBCHAR		fullPath[_MAX_PATH];
		fullPath[0] = 0;

		g_civPaths->FindFile(C3DIR_SOUNDS, soundValue, fullPath);
		PlaySound(fullPath, NULL, (SND_ASYNC | SND_FILENAME | SND_NOWAIT)); 
	}
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
#endif
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
#endif
}
