










#include "c3.h"
#include "aui_directmoviemanager.h"

#ifdef __AUI_USE_DIRECTMEDIA__


#include "aui_ui.h"




aui_DirectMovieManager::aui_DirectMovieManager(bool init, bool initDirect)
:
	aui_MovieManager        (init),
	m_directMovieResource   (NULL)
{
	if (initDirect)
	{
		m_directMovieResource = new aui_Resource<aui_DirectMovie>;
		Assert(m_directMovieResource);
	}
}



aui_DirectMovieManager::~aui_DirectMovieManager()
{
	delete m_directMovieResource;
}


#endif 
