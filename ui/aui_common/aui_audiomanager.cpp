










#include "c3.h"

#include "aui_audiomanager.h"



aui_AudioManager::aui_AudioManager()
{
	InitCommon();
}



AUI_ERRCODE aui_AudioManager::InitCommon( void )
{
	m_useAudio = TRUE;

	return AUI_ERRCODE_OK;
}



aui_AudioManager::~aui_AudioManager()
{
}
