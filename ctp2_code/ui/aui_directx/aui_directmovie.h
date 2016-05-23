#ifndef __AUI_DIRECTMOVIE_H__
#define __AUI_DIRECTMOVIE_H__

#ifdef __AUI_USE_DIRECTMEDIA__

#include "aui_movie.h"

class aui_DirectSurface;

class aui_DirectMovie : public aui_Movie
{
public:

	aui_DirectMovie(
		AUI_ERRCODE *retval,
		MBCHAR const * filename = NULL );
	virtual ~aui_DirectMovie();

protected:
	AUI_ERRCODE InitCommon( void );

public:

	virtual AUI_ERRCODE Open(
		uint32 flags = 0,
		aui_Surface *surface = NULL,
		RECT *rect = NULL );

	AUI_ERRCODE TargetSurface(aui_DirectSurface *surface);

	virtual AUI_ERRCODE Close( void );

	virtual AUI_ERRCODE Play( void );
	virtual AUI_ERRCODE Stop( void );

	virtual AUI_ERRCODE Pause( void );
	virtual AUI_ERRCODE Resume( void );

	virtual AUI_ERRCODE Process( void );

protected:
	IMultiMediaStream			*m_mmStream;
	IMediaStream				*m_primaryVidStream;
	IDirectDrawMediaStream		*m_ddStream;
	IDirectDrawStreamSample		*m_streamSample;
	LPDIRECTDRAWSURFACE			m_streamSurface;
	RECT						m_streamRect;
	uint32						m_endOfMovieTickCount;
};

#endif

#endif
