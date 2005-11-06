//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Screen utilities
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
// _DEBUG
// - Generate debug version when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Start the great library with the current research project of the player.
// - Prevent production errors when pressing F3 after end of turn.
// - Added close_AllScreensAndUpdateInfoScreen so that on a new turn the 
//   information window can stay open. (Aug. 7th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "c3ui.h"

#include "aui_button.h"

#include "c3window.h"
#include "workwindow.h"
#include "workwin.h"
#include "sciencewin.h"
#include "infowindow.h"
#include "infowin.h"
#include "victorywindow.h"
#include "victorywin.h"
#include "greatlibrary.h"
#include "optionswindow.h"
#include "citywindow.h"

#include "km_screen.h"
#include "EndgameWindow.h"
#include "creditsscreen.h"

#include "tutorialwin.h"

#include "debugwindow.h"

#include "debugmemory.h"

#include "player.h"
#include "SelItem.h"
#include "profileDB.h"
#include "network.h"

#include "screenutils.h"

#include "director.h"

#include "diplomacywindow.h"
#include "DomesticManagementDialog.h"
#include "NationalManagementDialog.h"

#include "trademanager.h"
#include "ScienceManagementDialog.h"
#include "unitmanager.h"
#include "sciencevictorydialog.h"

#include "battleviewwindow.h"
#include "scenarioeditor.h"
#include "EditQueue.h"

#include "dipwizard.h"

#include "sciencevictorydialog.h"


extern C3UI                 *g_c3ui;
extern WorkWindow           *g_workWindow;

extern GreatLibrary         *g_greatLibrary;

extern ScienceWin           *g_scienceWin;

extern InfoWindow           *g_infoWindow;

extern DebugWindow          *g_debugWindow;

extern TutorialWin          *g_tutorialWin;
extern Player               **g_player;
extern SelectedItem         *g_selected_item;
extern ProfileDB            *g_theProfileDB;

extern Network              g_network;
extern BattleViewWindow     *g_battleViewWindow;


double	g_screenTime = 0.0;

extern bool g_e3Demo;

sint32 open_WorkView( void )
{
	if(g_e3Demo) return 0;

	AUI_ERRCODE auiErr;
	sint32 err;

	err = workwin_Initialize();
	Assert( !err );
	if ( err ) return -1;

	auiErr = g_c3ui->AddWindow( g_workWindow );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return -1;

	return 0;
}

sint32 close_WorkView( void )
{
	if ( g_workWindow ) {
		g_c3ui->RemoveWindow( g_workWindow->Id() );
	}

	return 0;
}

sint32 open_CityView( void )
{
	AUI_ERRCODE auiErr;

	auiErr = CityWindow::Display(NULL);

	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return -1;

	return 0;
}

sint32 close_CityView( void )
{
	CityWindow::Close(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);

	return 0;
}

sint32 open_CityStatus( void )
{
	if(g_e3Demo) return 0;
#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG

		NationalManagementDialog::Open();

#ifdef _DEBUG
	GET_ELAPSED_TIME( "City Status" );
#endif // _DEBUG

	return 0;
}

sint32 close_CityStatus( void )
{
	NationalManagementDialog::Close();

	return 0;
}

sint32 open_CivStatus()
{
	if(g_e3Demo) return 0;

#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG

	DomesticManagementDialog::Open();

#ifdef _DEBUG
	GET_ELAPSED_TIME( "Civ Status" );
#endif // _DEBUG

	return 0;
}

sint32 close_CivStatus( void )
{
	DomesticManagementDialog::Close();
	return 0;
}

sint32 open_ScienceStatus( void )
{
	ScienceManagementDialog::Open();
	return 0;
}

sint32 close_ScienceStatus( void )
{
	ScienceManagementDialog::Close();
	return 0;
}

sint32 open_ScienceVictory( void )
{
	ScienceVictoryDialog::Open();
	return 0;
}

sint32 close_ScienceVictory( void )
{
	ScienceVictoryDialog::Close();
	return 0;
}

sint32 open_UnitStatus( void )
{
	UnitManager::Display();
	return 0;
}

sint32 close_UnitStatus( void )
{
	UnitManager::Hide();
	return 0;
}

sint32 open_TradeStatus( void )
{
#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG




	TradeManager::Display();

#ifdef _DEBUG
	GET_ELAPSED_TIME( "Trade" );
#endif // _DEBUG

	return 0;
}

sint32 close_TradeStatus( void )
{
	TradeManager::Hide();
	return 0;
}

sint32 open_VictoryWindow( void )
{
	if(g_e3Demo) return 0;

#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG




	victorywin_Initialize(0);
	victorywin_DisplayWindow(0);

#ifdef _DEBUG
	GET_ELAPSED_TIME( "Victory" );
#endif // _DEBUG

	return 0;
}

sint32 close_VictoryWindow( void )
{
	victorywin_RemoveWindow();
	return 0;
}

sint32 open_Diplomacy( void )
{
#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG

	DiplomacyWindow::Display();

#ifdef _DEBUG
	GET_ELAPSED_TIME( "Diplomacy" );
#endif // _DEBUG

	return 0;
}

sint32 close_Diplomacy( void )
{
	DiplomacyWindow::Hide();
	return 0;
}

sint32 open_InfoScreen( void )
{
	if(g_e3Demo) return 0;

#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG

	InfoWindow::Open();

#ifdef _DEBUG
	GET_ELAPSED_TIME( "Info" );
#endif // _DEBUG

	return 0;
}

sint32 close_InfoScreen( void )
{
	InfoWindow::Close();
	return 0;
}


//----------------------------------------------------------------------------
//
// Name       : open_GreatLibrary
//
// Description: (Re)open the great library window.
//
// Parameters : index               : advance to display on opening
//              sci                 : force display of index, even when the
//                                    library was open
//
// Globals    : g_e3Demo            : flag to indicate special E3 demo without
//                                    great library functionality
//
// Returns    : sint32              : opening succeeded
//
// Remark(s)  : When the library is not open already, or the sci parameter is
//              set, it will open with the indicated advance.
//
//----------------------------------------------------------------------------
sint32 open_GreatLibrary(sint32 index, BOOL sci)
{
	if (g_e3Demo) return false;

#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG

	sint32 const	err	= greatlibrary_Initialize(index, sci);
	Assert(!err);
	if (err) return false;

	g_greatLibrary->Display();

#ifdef _DEBUG
	GET_ELAPSED_TIME("Great Library");
#endif // _DEBUG

	return true;
}

//----------------------------------------------------------------------------
//
// Name       : open_GreatLibrary
//
// Description: (Re)open the great library window.
//
// Parameters : -
//
// Globals    : g_selected_item     : object selected at screen
//              g_player            : list of players
//
// Returns    : sint32              : opening succeeded
//
// Remark(s)  : When the library is not open already, it will open with the 
//              advance that is being researched by the current player.
//
//----------------------------------------------------------------------------
sint32 open_GreatLibrary( void )
{
	sint32 const		player	= g_selected_item->GetVisiblePlayer();
	AdvanceType const	advance	= g_player[player]->m_advances->GetResearching();

	return open_GreatLibrary(advance);
}


sint32 close_GreatLibrary( void )
{
	if ( g_greatLibrary ) {
		g_greatLibrary->Remove();
	}

	return 0;
}


sint32 open_OptionsScreen( sint32 fromWhichScreen )
{
#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG

	sint32 err = optionsscreen_displayMyWindow( fromWhichScreen );
	
	return err;

#ifdef _DEBUG
	GET_ELAPSED_TIME( "Options Screen" );
#endif // _DEBUG
}

sint32 close_OptionsScreen( void )
{
	return optionsscreen_removeMyWindow( AUI_BUTTON_ACTION_EXECUTE );
}


sint32 open_KeyMappingScreen( void )
{
	if(g_e3Demo) return 0;

	sint32 err = km_screen_displayMyWindow();

	return err;
}

sint32 open_ScenarioEditor(void)
{
	sint32 err = ScenarioEditor::Display();
	return err;
}

sint32 close_ScenarioEditor(void)
{
	sint32 err = ScenarioEditor::Hide();
	return err;
}


sint32 close_KeyMappingScreen( void )
{
	return km_screen_removeMyWindow( AUI_BUTTON_ACTION_EXECUTE );
}































sint32 open_EndGame() 
{
	if(g_e3Demo) return 0;

	
	
	
	sint32 curPlayer = g_selected_item->GetVisiblePlayer();

	
	
	
	
	
	

	
	

	
	
	

	
	return(0);
}


sint32 close_EndGame( void )
{
	
	
	
	

	
	
	

	
	return(0);
}

sint32 open_TutorialWin( void )
{
	if(g_e3Demo) return 0;
#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG

	sint32 err;

	err = tutorialwin_Initialize();
	Assert( !err );
	if ( err ) return -1;

	g_tutorialWin->Display();

#ifdef _DEBUG
	GET_ELAPSED_TIME( "Tutorial Win" );
#endif // _DEBUG

	return 0;
}

sint32 close_TutorialWin( void )
{
	if(!g_tutorialWin) return 0;

	g_tutorialWin->Remove();

	return 0;
}



sint32 open_CreditsScreen() 
{
	if(g_e3Demo) return 0;
	
	
	sint32 err = creditsscreen_Initialize();

	
	Assert(!err);
	if(err) return(-1);

	
	AUI_ERRCODE auiErr = g_c3ui->AddWindow(g_creditsWindow);

	
	Assert(auiErr == AUI_ERRCODE_OK);
	if(auiErr != AUI_ERRCODE_OK) return(-1);

	
	return(0);
}


sint32 close_CreditsScreen( void )
{
	
	
	
	sint32 err = creditsscreen_Cleanup();

	
	Assert(!err);
	if(err) return(-1);

	
	return(0);
}


void battleview_ExitButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );

//----------------------------------------------------------------------------
//
// Name       : close_AllScreens
//
// Description: Closes all open windows.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Returns always 1.
//
//----------------------------------------------------------------------------
sint32 close_AllScreens( void )
{
	close_CreditsScreen();
	close_WorkView();
	close_CityView();
	close_CityStatus();
	close_CivStatus();
	close_ScienceStatus();
	close_UnitStatus();
	close_TradeStatus();
	close_Diplomacy();
	close_InfoScreen();
	close_GreatLibrary();
	close_OptionsScreen();
	close_KeyMappingScreen();
	close_TutorialWin();
	close_ScenarioEditor();
	EditQueue::Hide();
	DipWizard::Hide();
	ScienceVictoryDialog::Close();

	extern sint32 g_modalWindow;

	g_modalWindow = 0;

	if (g_battleViewWindow) {
		
		g_modalWindow = 1;

		battleview_ExitButtonActionCallback(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);

		
		
	}

	
	

	return 1;
}

//----------------------------------------------------------------------------
//
// Name       : close_AllScreensAndUpdateInfoScreen
//
// Description: Closes all open windows except the information window.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Returns always 1.
//
//----------------------------------------------------------------------------
sint32 close_AllScreensAndUpdateInfoScreen( void )
{
	close_CreditsScreen();
	close_WorkView();
	close_CityView();
	close_CityStatus();
	close_CivStatus();
	close_ScienceStatus();
	close_UnitStatus();
	close_TradeStatus();
	close_Diplomacy();
	InfoWindow::Update();
	close_GreatLibrary();
	close_OptionsScreen();
	close_KeyMappingScreen();
	close_TutorialWin();
	close_ScenarioEditor();
	EditQueue::Hide();
	DipWizard::Hide();
	ScienceVictoryDialog::Close();

	extern sint32 g_modalWindow;

	g_modalWindow = 0;

	if (g_battleViewWindow) {
		
		g_modalWindow = 1;

		battleview_ExitButtonActionCallback(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);

		
		
	}

	
	

	return 1;
}
