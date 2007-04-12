//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The message turning on/off screen
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "civ3_main.h"
#include "civapp.h"

#include "c3window.h"
#include "c3_popupwindow.h"

#include "ctp2_button.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3ui.h"
#include "aui_uniqueid.h"
#include "aui_stringtable.h"
#include "netshell.h"

#include "spnewgamewindow.h"
#include "optionswindow.h"
#include "loadsavewindow.h"
#include "optionwarningscreen.h"

#include "keypress.h"
#include "keyboardhandler.h"
class WarningKeyboardHandler : public KeyboardHandler
{
public:
	void kh_Close()
	{
		optionwarningscreen_removeMyWindow(AUI_BUTTON_ACTION_EXECUTE);
	}
};
WarningKeyboardHandler s_warningKeyboardHandler;

extern CivApp				*g_civApp;
extern C3UI					*g_c3ui;

#include "network.h"
extern Network				g_network;

static uint32	s_currentWarning = OWS_OWSTOTAL;
static c3_PopupWindow *s_optionwarningscreenWindow	= NULL;
static ctp2_Button	*s_but1				= NULL,
					*s_but2				= NULL,
					*s_nevermind		= NULL;
static c3_Static			*s_message				= NULL;

static aui_StringTable		*s_messageString	= NULL,
							*s_but1String		= NULL,
							*s_but2String		= NULL;

static void optionwarningscreen_setMyWarning(uint32);


sint32	optionwarningscreen_displayMyWindow(uint32 warning)
{
	sint32 retval=0;
	if(!s_optionwarningscreenWindow) { retval = optionwarningscreen_Initialize(); }

	optionwarningscreen_setMyWarning(warning);

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->AddWindow(s_optionwarningscreenWindow);
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RegisterHandler(&s_warningKeyboardHandler);

	return retval;
}
sint32 optionwarningscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_optionwarningscreenWindow->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RemoveHandler(&s_warningKeyboardHandler);

	return 1;
}

AUI_ERRCODE optionwarningscreen_Initialize( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( s_optionwarningscreenWindow ) return AUI_ERRCODE_OK;

	strcpy(windowBlock, "OptionWarningWindow");
	s_optionwarningscreenWindow = new c3_PopupWindow(
		&errcode,
		aui_UniqueId(),
		windowBlock,
		16,
		AUI_WINDOW_TYPE_FLOATING,
		false );
	Assert( AUI_NEWOK(s_optionwarningscreenWindow, errcode) );
	if ( !AUI_NEWOK(s_optionwarningscreenWindow, errcode) ) errcode;

	s_optionwarningscreenWindow->SetStronglyModal(TRUE);

	
	s_but1 = spNew_ctp2_Button(&errcode,windowBlock,"But1Button",optionwarningscreen_but1Press);
	s_but2 = spNew_ctp2_Button(&errcode,windowBlock,"But2Button",optionwarningscreen_but2Press);
	s_nevermind = spNew_ctp2_Button(&errcode,windowBlock,"NeverMindButton",optionwarningscreen_nevermindPress);

	s_message			= spNew_c3_Static(&errcode,windowBlock,"Message");

	s_messageString		= spNewStringTable(&errcode,"OWSMessageStringTable");
	s_but1String		= spNewStringTable(&errcode,"OWSBut1StringTable");
	s_but2String		= spNewStringTable(&errcode,"OWSBut2StringTable");

	MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( block, "%s.%s", windowBlock, "Name" );
	s_optionwarningscreenWindow->AddTitle( block );

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE optionwarningscreen_Cleanup()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !s_optionwarningscreenWindow  ) return AUI_ERRCODE_OK;

	g_c3ui->RemoveWindow( s_optionwarningscreenWindow->Id() );
	keypress_RemoveHandler(&s_warningKeyboardHandler);

	mycleanup(s_but1);
	mycleanup(s_but2);
	mycleanup(s_nevermind);

	mycleanup(s_message);

	mycleanup(s_messageString);
	mycleanup(s_but1String);
	mycleanup(s_but2String);

	delete s_optionwarningscreenWindow;
	s_optionwarningscreenWindow = NULL;

	s_currentWarning = OWS_OWSTOTAL;

	return AUI_ERRCODE_OK;

#undef mycleanup
}

void optionwarningscreen_but1Press(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	
	Assert((s_currentWarning >=OWS_OWSFIRST) && (s_currentWarning < OWS_OWSTOTAL));
	switch(s_currentWarning) {
	case OWS_LOAD: 
		
		optionsscreen_removeMyWindow(action);
		if(optionwarningscreen_removeMyWindow(action))
			loadsavescreen_displayMyWindow(LSS_LOAD_GAME);
		break;

	case OWS_NEWGAME:
		
		
		optionsscreen_removeMyWindow(action);
		if(optionwarningscreen_removeMyWindow(action))
			g_civApp->PostRestartGameAction();
		break;

	case OWS_RESTART:
		
		optionsscreen_removeMyWindow(action);
		if(optionwarningscreen_removeMyWindow(action)) {
			g_civApp->PostRestartGameSameMapAction();
		}
		break;

	case OWS_QUIT: 
		ExitGame();
		break;

	case OWS_QUITTOSHELL:
		
		
		optionsscreen_removeMyWindow(action);
		if(optionwarningscreen_removeMyWindow(action))
		{
			if ( g_netfunc || g_network.IsActive() || g_network.IsNetworkLaunch() )
				g_civApp->PostQuitToLobbyAction();
			else
				g_civApp->PostQuitToSPShellAction();
		}
		break;

	default:
		break;
	};
}

void optionwarningscreen_but2Press(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if(g_network.IsClient())
		return;

	if(optionwarningscreen_removeMyWindow(action)) {
		

		
		
		g_isScenario = FALSE;
		
		loadsavescreen_displayMyWindow(LSS_SAVE_GAME);
	}

}
void optionwarningscreen_nevermindPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	optionwarningscreen_removeMyWindow(action);
}

static
void optionwarningscreen_setMyWarning(uint32 warning)
{

	s_currentWarning = warning;

	Assert((s_currentWarning >=OWS_OWSFIRST) && (s_currentWarning < OWS_OWSTOTAL));

	s_message->SetText(s_messageString->GetString(warning));
	s_but1->SetText(s_but1String->GetString(warning));
	s_but2->SetText(s_but2String->GetString(warning));

}

void CloseOptionWarningScreenAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	optionwarningscreen_Cleanup();
}

