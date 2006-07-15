










#ifndef __AUI_DIRECTSOUND_H__
#define __AUI_DIRECTSOUND_H__

#ifdef __AUI_USE_DIRECTX__


#include "aui_sound.h"






class aui_DirectSound : public aui_Sound
{
public:
	aui_DirectSound(
		AUI_ERRCODE *retval,
		MBCHAR const * filename = NULL );
	virtual ~aui_DirectSound();

	virtual AUI_ERRCODE Load(
		char *fileName,
		float x,
		float y,
		float z ) { return AUI_ERRCODE_OK; }; 
	virtual AUI_ERRCODE Load( void );	
	virtual AUI_ERRCODE Unload ( void );
	virtual void Play( void );
	virtual void Stop( void );
	void GetStatus( DWORD * dwStatus );
	void SetPlayFlag( int flag );

protected:
	AUI_ERRCODE CreateDSBuffer( void );

	LPDIRECTSOUNDBUFFER	dsb;
	uint32	m_userPriority;
	sint32	m_timeStamp;
	sint32	m_maxCopies;
	sint32	m_volume;
	sint32	m_random;
	sint32	m_sampleRate;	
	uint32	m_copiesPlaying;
	BOOL	m_duplicated;
	BOOL	m_alreadyDeleted;
	sint32	m_count;			
	uint32	m_flags;
	sint32	m_playFlags;
}; 


#endif 

#endif 
