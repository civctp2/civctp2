










#include "c3.h"
#include "aui_ui.h"

#include "aui_moviemanager.h"



aui_MovieManager::aui_MovieManager( BOOL init )
	:
	aui_Base(),
	m_movieResource( NULL )
{
	if ( init )
	{
		m_movieResource = new aui_Resource<aui_Movie>;
		Assert( m_movieResource != NULL );
	}
}



aui_MovieManager::~aui_MovieManager()
{
	if ( m_movieResource )
	{
		delete m_movieResource;
		m_movieResource = NULL;
	}
}
