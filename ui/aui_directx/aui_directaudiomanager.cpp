










#include "c3.h"


#ifdef __AUI_USE_DIRECTX__


#include "aui_ui.h"

#include "aui_directaudiomanager.h"


DS3DLISTENER	listenerTemplate;
DS3DBUFFER		ds3dBufferTemplate;



aui_DirectAudioManager::aui_DirectAudioManager( sint32 maxNumSounds )
{
	InitCommon( maxNumSounds );
}



AUI_ERRCODE aui_DirectAudioManager::InitCommon( sint32 maxNumOfActiveSounds )
{
	m_listener = NULL;
	m_timeOut = 0;
	m_numObjects = 0;
	m_maxObjects = 0;
	m_masterVolume1 = 0;
	m_masterVolume2 = 0;

	m_use3D = FALSE;
	m_DSHwFreeMemory = 0;
	m_DSHwMaxContigFreeMem = 0;

	m_primaryBuffer = NULL;

	
	m_dsHandle = NULL;
	m_bA3DAvailable = FALSE; 

	m_numberOfActiveSounds = maxNumOfActiveSounds ?
		maxNumOfActiveSounds :
		DEFAULT_NUMBER_OF_ACTIVE_SOUND;

	m_activeSoundResource = new aui_Resource<aui_DirectSound>;
	Assert( m_activeSoundResource != NULL );
	if ( !m_activeSoundResource ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_inactiveSoundResource = new aui_Resource<aui_DirectSound>;
	Assert( m_inactiveSoundResource != NULL );
	if ( !m_inactiveSoundResource ) return AUI_ERRCODE_MEMALLOCFAILED;

	
	if ( InitDS( g_ui->TheHWND() ) )
	{
		m_useAudio = FALSE;	
		return AUI_ERRCODE_NODSAUDIO;
	}
	
	return AUI_ERRCODE_OK;
}



aui_DirectAudioManager::~aui_DirectAudioManager()
{
	if( m_dsHandle )
	{
		m_dsHandle->Release();
		m_dsHandle = NULL;
	}

	if ( m_activeSoundResource )
	{
		delete m_activeSoundResource;
		m_activeSoundResource = NULL;
	}

	if ( m_inactiveSoundResource )
	{
		delete m_inactiveSoundResource;
		m_inactiveSoundResource = NULL;
	}
}



BOOL aui_DirectAudioManager::CreateDSInterface( HWND hwnd )
{
	HRESULT hr;
	DSCAPS	SoundDeviceCaps;

	if( DirectSoundCreate(NULL, &m_dsHandle, NULL) < DS_OK) 
		return ( 1 );


	
	if( (hr = m_dsHandle->SetCooperativeLevel( hwnd, DSSCL_NORMAL ) ) < DS_OK)
	{
		return ( 1 );
	}

	
	memset(&SoundDeviceCaps,0,sizeof(DSCAPS));
	SoundDeviceCaps.dwSize = sizeof(DSCAPS);
	m_dsHandle->GetCaps( &SoundDeviceCaps );
	m_DSHwFreeMemory = SoundDeviceCaps.dwFreeHwMemBytes;
	if( m_DSHwFreeMemory == 0 )
		m_DSHwFreeMemory = DS_SOUND_MAX_BUF;
	m_DSHwMaxContigFreeMem = SoundDeviceCaps.dwMaxContigFreeHwMemBytes;

	return( 0 );
}


BOOL aui_DirectAudioManager::CreatePrimaryBuffer()
{
	DSBUFFERDESC	dsbd;

	memset(&dsbd, 0, sizeof(dsbd));
	dsbd.dwSize	= sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;

	
	if (m_dsHandle->CreateSoundBuffer(&dsbd, &m_primaryBuffer, NULL) < DS_OK)
		return( 1 );

	
	m_dsHandle->SetSpeakerConfig( DSSPEAKER_QUAD );


	return( 0 );
}


BOOL aui_DirectAudioManager::SetDSoundRate( uint32 newRate )
{
	PCMWAVEFORMAT	pcmwf;

	if( m_primaryBuffer )
	{
		memset(&pcmwf, 0, sizeof(pcmwf));
		pcmwf.wf.wFormatTag			= WAVE_FORMAT_PCM;
		pcmwf.wf.nChannels			= DEFAULT_CHANNELS;
		pcmwf.wf.nSamplesPerSec		= newRate;
		pcmwf.wf.nAvgBytesPerSec 	= newRate * (DEFAULT_BITS / 8) * DEFAULT_CHANNELS;
		pcmwf.wf.nBlockAlign	 		=	(DEFAULT_BITS / 8) * DEFAULT_CHANNELS;
		pcmwf.wBitsPerSample	 		= DEFAULT_BITS;

		if (m_primaryBuffer->SetFormat((WAVEFORMATEX *)&pcmwf) != DS_OK)
		{
			
			return 1;
		}
	}

	return ( 0 );
}


BOOL aui_DirectAudioManager::InitDS( HWND hwnd )
{
	
	if( CreateDSInterface( hwnd ) )
		return ( 1 );

	
	if( CreatePrimaryBuffer() )
		return ( 1 );

	return ( 0 );
}







aui_Sound *aui_DirectAudioManager::AttachSound( MBCHAR *fileName, sint32 flag )
{
	if( !m_useAudio ) return NULL;

	aui_DirectSound *newSoundObject = m_activeSoundResource->Load( fileName );
	if ( newSoundObject )
		newSoundObject->SetPlayFlag( flag );

	return newSoundObject;
}


aui_Sound *aui_DirectAudioManager::Load( MBCHAR *filename )
{
	return AttachSound( filename, 0 );
}



AUI_ERRCODE aui_DirectAudioManager::DetachSound( MBCHAR *fileName )
{
	return Unload( fileName );
}


LPDIRECTSOUND aui_DirectAudioManager::GetdDSHandle()
{
	return m_dsHandle;
}



AUI_ERRCODE aui_DirectAudioManager::PlaySound( MBCHAR *name )
{
	DWORD dwStatus;
	aui_DirectSound *soundObject = NULL;

	if( !m_useAudio ) return AUI_ERRCODE_OK;

	soundObject = (aui_DirectSound *)Load( name );
	if ( soundObject )
	{
		soundObject->GetStatus( &dwStatus );
		if( !(dwStatus & DSBSTATUS_PLAYING) )
		{
			soundObject->Play();
		}

		Unload( soundObject );
	}

	return AUI_ERRCODE_OK;
}


#endif 
