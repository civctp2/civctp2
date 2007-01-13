










#include "c3.h"


#ifdef __AUI_USE_DIRECTX__


#include "aui_ui.h"
#include "aui_directaudiomanager.h"

#include "aui_directsound.h"



aui_DirectSound::aui_DirectSound(
	AUI_ERRCODE *retval,
	MBCHAR *filename )
	:
	aui_Sound( retval, filename )
{
}



aui_DirectSound::~aui_DirectSound()
{
	Unload();
}



AUI_ERRCODE aui_DirectSound::Load( void ) 
{
	LPVOID pMem1, pMem2;
	DWORD dwSize1, dwSize2;
	HRESULT hr;

	Assert ( m_format != NULL );
	if ( !m_format ) return AUI_ERRCODE_INVALIDPARAM;

	if ( m_data ) return AUI_ERRCODE_OK;

	if ( m_format->LoadSoundData( m_filename, &m_data, &m_size ) == AUI_ERRCODE_OK )
	{
		if( !CreateDSBuffer() )
		{
		 if ((hr=dsb->Lock( 0, m_size, &pMem1, &dwSize1, &pMem2, &dwSize2, 0 )))
			return AUI_ERRCODE_MEMALLOCFAILED;

		 CopyMemory(pMem1,m_data, m_size);

		 if((hr=dsb->Unlock( pMem1, dwSize1, pMem2, dwSize2 )))
			return AUI_ERRCODE_MEMALLOCFAILED;
		}		
		m_format->ReleaseSoundData();
	}
	return AUI_ERRCODE_OK;

}



AUI_ERRCODE aui_DirectSound::Unload( void ) 
{
	if ( dsb )
	{
		dsb->Release();
		dsb = NULL;
	}

	return AUI_ERRCODE_OK;
}



void aui_DirectSound::GetStatus( DWORD *dwStatus )
{
	dsb->GetStatus( dwStatus );
}



void aui_DirectSound::SetPlayFlag( int flag )
{
	m_playFlags = flag;
}



void aui_DirectSound::Play() 
{
	dsb->Play( 0, 0, m_playFlags );
}



void aui_DirectSound::Stop() 
{
	dsb->Stop();
}



AUI_ERRCODE aui_DirectSound::CreateDSBuffer() 
{ 
	PCMWAVEFORMAT pcmwf; 
	DSBUFFERDESC dsbdesc; 
	LPDIRECTSOUND	dsHandle; 

	aui_DirectAudioManager *audioManager =
		(aui_DirectAudioManager *)(void *)g_ui->TheAudioManager();

  
  memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT)); 
  pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM; 
  pcmwf.wf.nChannels = 1; 
  pcmwf.wf.nSamplesPerSec = 22050; 
  pcmwf.wf.nBlockAlign = 1; 
  pcmwf.wf.nAvgBytesPerSec = pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign; 
  pcmwf.wBitsPerSample = 8; 

  memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 
  dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
  dsbdesc.dwFlags =   DSBCAPS_STATIC;
  dsbdesc.dwBufferBytes = m_size; 
  dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf; 

	dsHandle = audioManager->GetdDSHandle();
	
	if (dsHandle->CreateSoundBuffer( &dsbdesc, &dsb, NULL) < DS_OK)
		return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}


#endif 
