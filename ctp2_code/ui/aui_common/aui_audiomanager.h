










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
	virtual aui_Sound	*Load( MBCHAR *filename )
		{ return NULL; }

	virtual AUI_ERRCODE Unload( aui_Sound *sound )
		{ return AUI_ERRCODE_OK; }
	virtual AUI_ERRCODE Unload( MBCHAR *sound )
		{ return AUI_ERRCODE_OK; }

	virtual AUI_ERRCODE	AddSearchPath( MBCHAR *path )
		{ return AUI_ERRCODE_OK; }
	virtual AUI_ERRCODE	RemoveSearchPath( MBCHAR *path )
		{ return AUI_ERRCODE_OK; }

	virtual AUI_ERRCODE PlaySound( MBCHAR *name )
		{ return AUI_ERRCODE_OK; }

	BOOL UsingAudio( void ) const { return m_useAudio; }

protected:
	BOOL m_useAudio;
};


#endif 
