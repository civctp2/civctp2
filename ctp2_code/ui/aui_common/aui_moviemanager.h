










#ifndef __AUI_MOVIEMANAGER_H__
#define __AUI_MOVIEMANAGER_H__


#include "aui_base.h"
#include "aui_movie.h"
#include "aui_resource.h"

#include "c3files.h"


class aui_MovieManager : aui_Base
{
public:
	aui_MovieManager( BOOL init = TRUE );
	virtual ~aui_MovieManager();

	virtual aui_Movie	*Load( MBCHAR *filename, C3DIR dir = C3DIR_DIRECT )
	{ return m_movieResource->Load( filename, dir ); }

	virtual AUI_ERRCODE Unload( aui_Movie *movie )
	{ return m_movieResource->Unload( movie ); }
	virtual AUI_ERRCODE Unload( MBCHAR *movie )
	{ return m_movieResource->Unload( movie ); }

	virtual AUI_ERRCODE AddSearchPath( MBCHAR *path )
	{ return m_movieResource->AddSearchPath( path ); }
	virtual AUI_ERRCODE RemoveSearchPath( MBCHAR *path )
	{ return m_movieResource->RemoveSearchPath( path ); }

	aui_Resource<aui_Movie> *MovieResource( void ) const
	{ return m_movieResource; }

protected:
	aui_Resource<aui_Movie>	*m_movieResource;
};


#endif 
