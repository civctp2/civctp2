#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef OPTIONWARNING_FLAG
#define		OPTIONWARNING_FLAG

#include "aui_action.h"

sint32 optionwarningscreen_displayMyWindow(uint32 warning);
sint32 optionwarningscreen_removeMyWindow(uint32 action);
AUI_ERRCODE optionwarningscreen_Initialize( void );
AUI_ERRCODE optionwarningscreen_Cleanup();



typedef enum
{
	OWS_OWSFIRST=0,
	OWS_LOAD=0,
	OWS_NEWGAME,
	OWS_RESTART,
	OWS_QUIT,
	OWS_QUITTOSHELL,
	OWS_OWSTOTAL
} OWS_ENUM;

void optionwarningscreen_but1Press(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionwarningscreen_but2Press(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionwarningscreen_nevermindPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

class CloseOptionWarningScreenAction : public aui_Action
{
	virtual ActionCallback Execute;
};

#endif
