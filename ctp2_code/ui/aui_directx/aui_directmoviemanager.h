










#ifndef __AUI_DIRECTMOVIEMANAGER_H__
#define __AUI_DIRECTMOVIEMANAGER_H__

#ifdef __AUI_USE_DIRECTMEDIA__


#include "aui_moviemanager.h"
#include "aui_directmovie.h"
#include "aui_resource.h"

#include "c3files.h"


class aui_DirectMovieManager : public aui_MovieManager
{
public:
	aui_DirectMovieManager( BOOL init = FALSE, BOOL initDirect = TRUE );
	virtual ~aui_DirectMovieManager();

	virtual aui_Movie	*Load( MBCHAR *filename, C3DIR dir = C3DIR_DIRECT )
	{ return m_directMovieResource->Load( filename, dir ); }

	virtual AUI_ERRCODE Unload( aui_Movie *movie )
	{ return m_directMovieResource->Unload( (aui_DirectMovie *)movie ); }
	virtual AUI_ERRCODE Unload( MBCHAR *movie )
	{ return m_directMovieResource->Unload( movie ); }

	virtual AUI_ERRCODE AddSearchPath( MBCHAR *path )
	{ return m_directMovieResource->AddSearchPath( path ); }
	virtual AUI_ERRCODE RemoveSearchPath( MBCHAR *path )
	{ return m_directMovieResource->RemoveSearchPath( path ); }

	aui_Resource<aui_DirectMovie> *DirectMovieResource( void ) const
	{ return m_directMovieResource; }

protected:
	aui_Resource<aui_DirectMovie>	*m_directMovieResource;
};


#endif 

#endif 
