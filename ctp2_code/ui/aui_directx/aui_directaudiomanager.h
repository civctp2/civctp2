#ifndef __AUI_DIRECTAUDIOMANAGER_H__
#define __AUI_DIRECTAUDIOMANAGER_H__

#ifdef __AUI_USE_DIRECTX__

#include "aui_audiomanager.h"
#include "aui_directsound.h"
#include "aui_resource.h"


#define DS3D_LISTENERDOPPLERFACTOR DS3D_DEFAULTDOPPLERFACTOR
#define DS3D_LISTENERROLLOFFFACTOR 10.0f
#define DS3D_LISTENERDISTANCEFACTOR (D3DVALUE)1.0
#define DS3D_LISTENERMINDISTANCE (D3DVALUE)2500.0
#define DS3D_LISTENERMAXDISTANCE (D3DVALUE)30000.0

#define RATE44KHZ				44100
#define RATE22KHZ				22050
#define RATE11KHZ				11025
#define DEFAULT_RATE		RATE11

#define DEFAULT_CHANNELS		2
#define DEFAULT_BITS				8

#define			DS_SOUND_MAX_BUF		500000

#define DEFAULT_NUMBER_OF_ACTIVE_SOUND 16
#define MAX_NUMBER_OF_INACTIVE_SOUND 255






class aui_DirectAudioManager : public aui_AudioManager
{
public:
	aui_DirectAudioManager(
		sint32 maxNumSounds = DEFAULT_NUMBER_OF_ACTIVE_SOUND );
	virtual ~aui_DirectAudioManager();

protected:
	AUI_ERRCODE InitCommon( sint32 maxNumSounds );

public:
	virtual aui_Sound	*Load( MBCHAR *filename );

	virtual AUI_ERRCODE Unload( aui_Sound *sound )
	{
		return m_useAudio ?
			m_activeSoundResource->Unload( (aui_DirectSound *)sound ) :
			AUI_ERRCODE_OK;
	}
	virtual AUI_ERRCODE Unload( MBCHAR *sound )
	{
		return m_useAudio ?
			m_activeSoundResource->Unload( sound ) :
			AUI_ERRCODE_OK;
	}

	virtual AUI_ERRCODE AddSearchPath( MBCHAR *path )
	{
		return m_useAudio ?
			m_activeSoundResource->AddSearchPath( path ) :
			AUI_ERRCODE_OK;
	}
	virtual AUI_ERRCODE RemoveSearchPath( MBCHAR *path )
	{
		return m_useAudio ?
			m_activeSoundResource->RemoveSearchPath( path ) :
			AUI_ERRCODE_OK;
	}

	virtual AUI_ERRCODE PlaySound( MBCHAR *name );

	void Update( void );

	void StartMusic() {};
	void StartMusic( sint32 trackNumber ) {};

	LPDIRECTSOUND GetdDSHandle( void );
	aui_Sound *AttachSound( MBCHAR *filename, sint32 flag );
	AUI_ERRCODE DetachSound( MBCHAR *filename );

protected:
	aui_Resource<aui_DirectSound>	*m_activeSoundResource;
	aui_Resource<aui_DirectSound>	*m_inactiveSoundResource;

	LPDIRECTSOUND	m_dsHandle;
	LPDIRECTSOUNDBUFFER m_primaryBuffer;
	LPDIRECTSOUND3DLISTENER m_listener;
	sint32	m_timeOut;
	sint32	m_numObjects;
	sint32	m_maxObjects;
	sint32	m_masterVolume1;
	sint32	m_masterVolume2;

	sint32 m_numberOfActiveSounds;;
	BOOL m_use3D;
	BOOL m_bA3DAvailable;
	sint32	m_DSHwFreeMemory;
	sint32	m_DSHwMaxContigFreeMem;

	BOOL CreateDSInterface( HWND hwnd );
	BOOL CreatePrimaryBuffer( void );
	BOOL SetDSoundRate(DWORD newRate);
	BOOL InitDS( HWND hwnd );
};

#endif // __AUI_USE_DIRECTX__

#endif
