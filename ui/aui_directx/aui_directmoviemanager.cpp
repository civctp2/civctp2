










#include "c3.h"


#ifdef __AUI_USE_DIRECTMEDIA__


#include "aui_ui.h"

#include "aui_directmoviemanager.h"



aui_DirectMovieManager::aui_DirectMovieManager( BOOL init, BOOL initDirect )
	:
	aui_MovieManager( init ),
	m_directMovieResource( NULL )
{
	if ( initDirect )
	{
		m_directMovieResource = new aui_Resource<aui_DirectMovie>;
		Assert( m_directMovieResource != NULL );
	}
}



aui_DirectMovieManager::~aui_DirectMovieManager()
{
	if ( m_directMovieResource )
	{
		delete m_directMovieResource;
		m_directMovieResource = NULL;
	}
}


#endif 
