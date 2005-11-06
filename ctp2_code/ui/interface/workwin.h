#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __WORKWIN_H__
#define __WORKWIN_H__

#include "aui_action.h"

AUI_ACTION_BASIC(WorkWinCleanupAction);
AUI_ACTION_BASIC(WorkWinUpdateAction);

sint32 workwin_Initialize( void );
sint32 workwin_Cleanup( void );

sint32 workwin_Update( void );

#endif
