#ifndef __AUI_AUDIOMANAGER_H__
#define __AUI_AUDIOMANAGER_H__

#include "aui_base.h"

class aui_Sound;






class aui_AudioManager : aui_Base
{
public:
	aui_AudioManager();
	virtual ~aui_AudioManager();

protected:
	AUI_ERRCODE InitCommon( void );

public:
	virtual aui_Sound	*Load( const MBCHAR *filename )
		{ return NULL; }

	virtual AUI_ERRCODE Unload( aui_Sound *sound )
		{ return AUI_ERRCODE_OK; }
	virtual AUI_ERRCODE Unload( const MBCHAR *sound )
		{ return AUI_ERRCODE_OK; }

	virtual AUI_ERRCODE	AddSearchPath( const MBCHAR *path )
		{ return AUI_ERRCODE_OK; }
	virtual AUI_ERRCODE	RemoveSearchPath( const MBCHAR *path )
		{ return AUI_ERRCODE_OK; }

	virtual AUI_ERRCODE PlaySound( const MBCHAR *name )
		{ return AUI_ERRCODE_OK; }

	BOOL UsingAudio( void ) const { return m_useAudio; }

protected:
	BOOL m_useAudio;
};

#endif
