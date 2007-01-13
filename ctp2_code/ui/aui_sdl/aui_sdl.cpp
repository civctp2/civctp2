#include "ctp2_config.h"
#include "c3.h"

#ifdef __AUI_USE_SDL__

#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_sdl.h"


uint32 aui_SDL::m_SDLClassId = aui_UniqueId();
sint32 aui_SDL::m_SDLRefCount = 0;

AUI_ERRCODE aui_SDL::InitCommon(BOOL useExclusiveMode)
{
	m_exclusiveMode = useExclusiveMode;
	return AUI_ERRCODE_OK;
}

aui_SDL::~aui_SDL()
{
//	if (! --m_SDLRefCount) {
//	}
}

#endif
