
#include "c3.h"


#include "aui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_stringtable.h"
#include "aui_textfield.h"
#include "c3ui.h"
#include "c3_popupwindow.h"
#include "c3_static.h"

#include "ctp2_button.h"
#include "c3_dropdown.h"
#include "c3_listitem.h"
#include "network.h"
#include "netshell.h"

#include "spwindow.h"
#include "graphicsscreen.h"
#include "soundscreen.h"
#include "gameplayoptions.h"
#include "optionwarningscreen.h"
#include "loadsavewindow.h"
#include "km_screen.h"
#include "musicscreen.h"

#include "screenutils.h"
#include "optionswindow.h"

#include "gamesounds.h"
#include "gamesettings.h"

#include "keypress.h"
#include "TurnCnt.h"

#include "ScenarioEditor.h"
#include "scorewarn.h"

#include "MessageBoxDialog.h"
#include "StrDB.h"
#include "ProfileDB.h"

extern C3UI					*g_c3ui;
extern c3_PopupWindow		*g_scorewarn;
extern GameSettings			*g_theGameSettings;

extern aui_Surface			*g_sharedSurface;

extern Network				g_network;
extern SPWindow				*g_spWindow;
extern sint32				g_isCheatModeOn;
extern sint32				g_modalWindow;
extern TurnCount           *g_turn;
extern BOOL g_launchIntoCheatMode;

OptionsWindow				*g_optionsWindow		= NULL;
static sint32			s_return = 0;




sint32	optionsscreen_displayMyWindow( sint32 from )
{
	sint32 retval=0;
	if(!g_optionsWindow) { retval = optionsscreen_Initialize(); }

	s_return = from;

	extern bool g_e3Demo;
	if(g_e3Demo) return 0;

	g_c3ui->AddWindow(g_optionsWindow);

	
	if ( !from )
		g_optionsWindow->DisableButtons();
	else
		g_optionsWindow->EnableButtons();

	
	
	g_optionsWindow->SaveGameButton()->Enable(
		!(g_netfunc && !g_network.IsHost()) &&
		!g_spWindow &&
		!g_isCheatModeOn );

	
	
	g_optionsWindow->LoadGameButton()->Enable(
		!g_netfunc &&
		!g_spWindow );

	
	if ( !g_network.IsActive() && !g_spWindow && !g_turn->IsHotSeat() && !g_turn->IsEmail())
	{
		if(!g_theProfileDB->IsScenario() && !g_isScenario) {
			g_optionsWindow->RestartButton()->Enable( TRUE );
		} else {
			g_optionsWindow->RestartButton()->Enable(FALSE);
		}

		g_optionsWindow->MapEditorButton()->Enable( TRUE );
	}
	else
	{
		g_optionsWindow->RestartButton()->Enable( FALSE );

		g_optionsWindow->MapEditorButton()->Enable( FALSE );
	}

	
	g_optionsWindow->NewGameButton()->Enable( !g_spWindow );

	
	
	if ( g_spWindow )
		g_optionsWindow->RemoveQuitToWindowsButton();
	else
		g_optionsWindow->AddQuitToWindowsButton();

	gamesounds_WindowOpened();

	g_modalWindow++;
	
	return retval;
}
sint32 optionsscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	if(!g_optionsWindow || !g_c3ui->GetWindow(g_optionsWindow->Id())) return 0;
	AUI_ERRCODE auiErr;

	if ( g_optionsWindow ) {
		auiErr = g_c3ui->RemoveWindow( g_optionsWindow->Id() );
		Assert( auiErr == AUI_ERRCODE_OK );
	}

	gamesounds_WindowClosed();

	if ( !s_return ) {
		spscreen_displayMyWindow();
	}

	g_modalWindow--;

	return 1;
}



AUI_ERRCODE optionsscreen_Initialize( void )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( !g_scorewarn )
	{
		scorewarn_Initialize();
	}

	if ( g_optionsWindow ) {
		g_optionsWindow->MoveOG();
		return AUI_ERRCODE_OK; 
	}


	strcpy(windowBlock, "OptionsWindow");

	g_optionsWindow= new OptionsWindow(&errcode, aui_UniqueId(), windowBlock, 16,AUI_WINDOW_TYPE_FLOATING,false );
	Assert( AUI_NEWOK(g_optionsWindow, errcode) );
	if ( !AUI_NEWOK(g_optionsWindow, errcode) ) return errcode;
	
	g_optionsWindow->SetStronglyModal(TRUE);



	g_optionsWindow->GrabRegion()->Resize( g_optionsWindow->Width(), 20 );
	g_optionsWindow->SetDraggable( TRUE );

	sprintf( controlBlock, "%s.%s", windowBlock, "Name" );
	g_optionsWindow->AddTitle( controlBlock );
	g_optionsWindow->Title()->SetBlindness( TRUE );

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE optionsscreen_Cleanup()
{
	if ( g_scorewarn )
	{
		scorewarn_Cleanup();
	}

	if ( !g_optionsWindow  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( g_optionsWindow->Id() );

	g_optionsWindow->SetSurface( NULL );

	delete g_optionsWindow;
	g_optionsWindow = NULL;

	return AUI_ERRCODE_OK;
}





void optionsscreen_graphicsPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	graphicsscreen_displayMyWindow();		

}
void optionsscreen_soundPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	soundscreen_displayMyWindow();		
}
void optionsscreen_musicPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	musicscreen_displayMyWindow();		
}
void optionsscreen_savegamePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	optionsscreen_removeMyWindow(action);

	uint32 type = LSS_SAVE_GAME;

	
	if ( g_network.IsActive() )
		type = LSS_SAVE_MP;

	
	
	g_isScenario = FALSE;

	loadsavescreen_displayMyWindow( type );
}
void optionsscreen_savescenarioPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	uint32 type = LSS_SAVE_SCEN;

	
	
	g_isScenario = TRUE;

	loadsavescreen_displayMyWindow( type );
}

void optionsscreen_loadgamePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	
	optionwarningscreen_displayMyWindow(OWS_LOAD);
}
void optionsscreen_newgamePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	optionwarningscreen_displayMyWindow(OWS_NEWGAME);
}
void optionsscreen_restartPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	optionwarningscreen_displayMyWindow(OWS_RESTART);
}
void optionsscreen_quitPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	optionwarningscreen_displayMyWindow(OWS_QUIT);
}
void optionsscreen_quitToShellPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	optionwarningscreen_displayMyWindow(OWS_QUITTOSHELL);
}
void optionsscreen_returnPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	optionsscreen_removeMyWindow(action);
}
void optionsscreen_gameplayPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	gameplayoptions_displayMyWindow();
}

void optionsscreen_AcceptWarningCallback(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;
	g_launchIntoCheatMode = TRUE;
	ScenarioEditor::Display();
}

void optionsscreen_ConfirmScoreWarning(bool confirm, void *data)
{
	if(confirm) {
		g_theGameSettings->SetKeepScore(FALSE);
		disclaimer_Initialize(optionsscreen_AcceptWarningCallback);
	}
}

void optionsscreen_mapeditorPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if(g_network.IsActive())
		
		return;




	
	
	

	

	

	
	



	if (!g_launchIntoCheatMode) {
		optionsscreen_removeMyWindow(action);
		
		MessageBoxDialog::Query(g_theStringDB->GetNameStr("str_ldl_Scorewarn"),
								"ScoreWarning", optionsscreen_ConfirmScoreWarning);

	} else if (g_launchIntoCheatMode){
		optionsscreen_removeMyWindow(action);
		
		ScenarioEditor::Display();
	}
}
void optionsscreen_keyboardPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	open_KeyMappingScreen();
}
