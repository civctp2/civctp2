

#include "c3.h"
#include "soundmanager.h"
#include "PointerList.h"
#include "ProfileDB.h"
#include "SoundRecord.h"
#include "CivPaths.h"
#include "c3files.h"
#include "PlayListDB.h"
#include "Gamesounds.h"

extern HWND			gHwnd;
extern ProfileDB	*g_theProfileDB;
extern CivPaths		*g_civPaths;
extern PlayListDB	*g_thePlayListDB;

SoundManager		*g_soundManager = NULL;

static BOOL			s_initSuccessful = FALSE;

#define k_CHECK_CD_PERIOD		4000	
										

void SoundManager::Initialize(void)
{
	uint32 volume = 100;



	s_initSuccessful = FALSE;

	g_soundManager = new SoundManager();
	if (!s_initSuccessful) {
		if (g_soundManager) {
			delete g_soundManager;
			g_soundManager = NULL;
		}
		c3errors_ErrorDialog("Sound Manager", "Could not initialize the sound system.  Sound is disabled.");
	}
}

void SoundManager::Cleanup(void)
{
	if (g_soundManager != NULL)
		delete g_soundManager;

	g_soundManager = NULL;
}


SoundManager::SoundManager()
{
	if (g_theProfileDB) {
		m_sfxVolume = g_theProfileDB->GetSFXVolume();
		m_voiceVolume = g_theProfileDB->GetVoiceVolume();
		m_musicVolume = g_theProfileDB->GetMusicVolume();
	} else {
		Assert(FALSE);
		m_sfxVolume = 10;
		m_voiceVolume = 10;
		m_musicVolume = 10;
	}
	
	m_oldRedbookVolume = 0;






	
	m_numTracks = 0;
	m_curTrack = 0;
	m_lastTrack = 0;

	m_redbook = 0;
	m_timeToCheckCD = 0;

	m_sfxSounds = new PointerList<CivSound>;
	m_voiceSounds = new PointerList<CivSound>;
	m_soundWalker = new PointerList<CivSound>::Walker;

	m_noSound = FALSE;
	m_musicEnabled = FALSE;

	m_style = MUSICSTYLE_PLAYLIST;
	m_playListPosition = 0;


	m_usePlaySound = FALSE;


	
	
	m_autoRepeat = TRUE;

	if (m_usePlaySound) {
		s_initSuccessful = TRUE;

		return;
	}

	InitSoundDriver();

	s_initSuccessful = TRUE;
	
	m_stopRedbookTemporarily = FALSE;
}

SoundManager::~SoundManager()
{
	if (m_sfxSounds) {
		m_sfxSounds->DeleteAll();
		delete m_sfxSounds;
	}
	if (m_voiceSounds) {
		m_voiceSounds->DeleteAll();
		delete m_voiceSounds;
	}

	if (m_soundWalker) {
		delete m_soundWalker;
	}

	CleanupSoundDriver();
}

void SoundManager::DumpAllSounds(void)
{
	if (m_sfxSounds) {
		m_sfxSounds->DeleteAll();
	}
	if (m_voiceSounds) {
		m_voiceSounds->DeleteAll();
	}
}

void SoundManager::InitSoundDriver(void)
{
	
	S32		errcode;
	S32		use_digital;
	S32		use_MIDI;
	S32		output_rate;
	S32		output_bits;
	S32		output_channels;
#ifdef _DEBUG
	use_digital = AIL_QUICK_USE_WAVEOUT;	
#else
	use_digital = 1;						
#endif


use_digital = 1;						


	use_MIDI = 0;			
	output_channels = 2;	


	
	output_rate = 22050;	
	output_bits = 16;		






	errcode = AIL_quick_startup(use_digital, use_MIDI, output_rate, output_bits,
								output_channels);

	MBCHAR *smname = "Sound Manager";

	if (!errcode) {
		

		m_noSound = TRUE;
	}

	
	InitRedbook();

	
	SetVolume(SOUNDTYPE_SFX, m_sfxVolume);
	SetVolume(SOUNDTYPE_VOICE, m_voiceVolume);
	SetVolume(SOUNDTYPE_MUSIC, m_musicVolume);

}

void SoundManager::CleanupSoundDriver(void)
{
	if (s_initSuccessful && !m_usePlaySound) {
		CleanupRedbook();

		
		AIL_quick_shutdown();
	}
}

void SoundManager::InitRedbook(void)
{
	
	if (!m_redbook) {
		MBCHAR drive = c3files_GetCTPCDDriveLetter();
		m_redbook = AIL_redbook_open_drive(drive);
		if (!m_redbook) {
			
			g_theProfileDB->SetUseRedbookAudio(FALSE);
		}
	}
}

void SoundManager::CleanupRedbook(void)
{
	
	if (m_redbook) {
		AIL_redbook_stop(m_redbook);
		AIL_redbook_close(m_redbook);
		m_redbook = NULL;
	}
}

void SoundManager::ConvertCoordinates(sint32 x, sint32 y, sint32 *soundX, sint32 *soundY, sint32 *soundZ)
{
	*soundX = x;
	*soundY = y;
	*soundZ = 0;
}

void SoundManager::ProcessRedbook(void)
{
	
	if (!g_theProfileDB->IsUseRedbookAudio()) return;

	if (!m_musicEnabled) return;

	if (GetTickCount() > m_timeToCheckCD) {
		U32 status;
		if (m_redbook) {
			status = AIL_redbook_status(m_redbook);
			switch (status) {
			case REDBOOK_ERROR:
				break;
			case REDBOOK_PLAYING:
				break;
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
		m_timeToCheckCD = GetTickCount() + k_CHECK_CD_PERIOD;
	}
}

void SoundManager::Process(const uint32 target_milliseconds, uint32 &used_milliseconds)
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
				if (AIL_quick_status(sound->GetHAudio()) == QSTAT_DONE) {
					
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
				if (AIL_quick_status(sound->GetHAudio()) == QSTAT_DONE) {
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

void SoundManager::AddGameSound(GAMESOUNDS sound)
{
	sint32 id = gamesounds_GetGameSoundID(sound);
	AddSound(SOUNDTYPE_SFX, 0, id, 0, 0);
}

void SoundManager::AddSound(SOUNDTYPE type, uint32 associatedObject, sint32 soundID, sint32 x, sint32 y)
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
		
		AIL_quick_play(sound->GetHAudio(), 1);	

		sound->SetIsPlaying(TRUE);
	} else {
		
		delete sound;
	}
}

void SoundManager::AddLoopingSound(SOUNDTYPE type, uint32 associatedObject, sint32 soundID, sint32 x, sint32 y)
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

	
	AIL_quick_play(sound->GetHAudio(), 0);	

	sound->SetIsLooping(TRUE);
	sound->SetIsPlaying(TRUE);

}

void SoundManager::TerminateLoopingSound(SOUNDTYPE type, uint32 associatedObject)
{
	CivSound *existingSound = FindLoopingSound(type, associatedObject);

	if (!existingSound) return;

	AIL_quick_halt(existingSound->GetHAudio());
}

void SoundManager::TerminateAllLoopingSounds(SOUNDTYPE type)
{
	PointerList<CivSound>::PointerListNode *node;

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
			AIL_quick_halt(sound->GetHAudio());
		}
		node = node->GetNext();
	}
}

void SoundManager::TerminateSounds(SOUNDTYPE type)
{
	PointerList<CivSound>::PointerListNode *node;

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
			AIL_quick_halt(sound->GetHAudio());
		}
		node = node->GetNext();
	}
}

void SoundManager::TerminateAllSounds(void)
{
	TerminateSounds(SOUNDTYPE_SFX);
	TerminateSounds(SOUNDTYPE_VOICE);
}

void SoundManager::SetVolume(SOUNDTYPE type, uint32 volume)
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
		if (m_redbook)
			AIL_redbook_set_volume(m_redbook, (sint32)((double)volume * 12.7));
		break;
	}
}

void SoundManager::SetMasterVolume(uint32 volume)
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

CivSound *SoundManager::FindLoopingSound(SOUNDTYPE type, uint32 associatedObject)
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


CivSound *SoundManager::FindSound(SOUNDTYPE type, uint32 associatedObject)
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

void SoundManager::SetPosition(SOUNDTYPE type, uint32 associatedObject, sint32 x, sint32 y)
{
	PointerList<CivSound>::PointerListNode *node;

	sint32 volume;

	switch (type) {
	case SOUNDTYPE_SFX:
			node = m_sfxSounds->GetHeadNode();
			volume = m_sfxVolume;
		break;
	case SOUNDTYPE_VOICE:
			node = m_voiceSounds->GetHeadNode();
			volume = m_voiceVolume;
		break;
	}
	
	CivSound	*sound;
	sint32		objectX, objectY, objectZ;
	HAUDIO		hAudio;

	ConvertCoordinates(x, y, &objectX, &objectY, &objectZ);

	while (node) {
		sound = node->GetObj();
		if (sound) {
			if (sound->GetAssociatedObject() == associatedObject) {
				hAudio = sound->GetHAudio();

				sint32 panValue = 64;	

				if (hAudio) {
					AIL_quick_set_volume(hAudio, volume, panValue);
				}
			}
		}
		node = node->GetNext();
	}
}

void SoundManager::StartMusic(sint32 trackNum)
{
	
	m_stopRedbookTemporarily = FALSE;

	if (!g_theProfileDB->IsUseRedbookAudio() || !c3files_HasCD()) return;

	if (m_noSound) return;

	if (m_usePlaySound) return;

	if (m_curTrack == -1) return;

	if (!m_redbook) return;

	U32 status = AIL_redbook_status(m_redbook);

	if (status == REDBOOK_ERROR) return;

	
	if (AIL_redbook_track(m_redbook)) {
		
		AIL_redbook_stop(m_redbook);
	}

	sint32 numTracks = 0;
	numTracks = AIL_redbook_tracks(m_redbook);
	
	if (numTracks <= 1) return;

	m_numTracks = numTracks;
	
	if (trackNum < 0) trackNum = 0;
	if (trackNum > m_numTracks) trackNum = m_numTracks;

	m_curTrack = trackNum;

	U32 start, end;
	AIL_redbook_track_info(m_redbook, trackNum, &start, &end);

	
	
	
	TerminateAllSounds();

	AIL_redbook_play(m_redbook, start, end);
}

void SoundManager::TerminateMusic(void)
{
	if (!g_theProfileDB->IsUseRedbookAudio() || !c3files_HasCD()) return;

	if (m_noSound) return;

	if (m_usePlaySound) return;

	if (!m_redbook) return;

	
	m_stopRedbookTemporarily = TRUE;

	
	if (AIL_redbook_track(m_redbook)) {
		
		AIL_redbook_stop(m_redbook);
	}
}

void SoundManager::PickNextTrack(void)
{
	switch (m_style) {
	case MUSICSTYLE_PLAYLIST:
		m_playListPosition++;
		if (m_playListPosition >= g_thePlayListDB->GetNumSongs()) {
			if (m_autoRepeat) {
				m_playListPosition = 0;
			} else {
				m_playListPosition = 0;
				m_curTrack = -1;
				return;
			}
		}

		m_curTrack = 1 + g_thePlayListDB->GetSong(m_playListPosition);

		break;
	case MUSICSTYLE_RANDOM:
		
		m_curTrack = 2 + rand() % (m_numTracks-2);
		break;
	case MUSICSTYLE_USER:
		
		m_curTrack = m_userTrack + 2;
		if (!m_autoRepeat && m_curTrack == m_lastTrack) {
			
			
			m_curTrack = -1;
		}
		break;
	}
	m_lastTrack = m_curTrack;
}

void SoundManager::StupidPlaySound(sint32 soundID)
{
	if (g_theSoundDB->Get(soundID)->GetValue()) {
		MBCHAR		fullPath[_MAX_PATH];
		MBCHAR      nonConst[_MAX_PATH];
		fullPath[0] = 0;

		if (strlen(g_theSoundDB->Get(soundID)->GetValue()) > 0) {
			strcpy(nonConst, g_theSoundDB->Get(soundID)->GetValue());
			g_civPaths->FindFile(C3DIR_SOUNDS, nonConst, fullPath);
			
			PlaySound(fullPath, NULL,  (SND_ASYNC | SND_FILENAME | SND_NOWAIT)); 
		}
	}
}

S32 masterVolume;

void SoundManager::ReleaseSoundDriver(void)
{
	if (m_usePlaySound) return;
	if (m_noSound) return;

	HDIGDRIVER		dig;
	HMDIDRIVER		mdi;
	HDLSDEVICE		dls;

	TerminateAllSounds();

	AIL_quick_handles(&dig, &mdi, &dls);

	S32 err;

	
	masterVolume = AIL_digital_master_volume(dig);
	
	AIL_set_digital_master_volume(dig, 0);

	err = AIL_digital_handle_release(dig);
	Assert(err);
}


void SoundManager::ReacquireSoundDriver(void)
{
	if (m_usePlaySound) return;
	if (m_noSound) return;

	HDIGDRIVER		dig;
	HMDIDRIVER		mdi;
	HDLSDEVICE		dls;

	AIL_quick_handles(&dig, &mdi, &dls);

	S32 err;

	err = AIL_digital_handle_reacquire(dig);
	Assert(err);

	
	AIL_set_digital_master_volume(dig, masterVolume);
}
