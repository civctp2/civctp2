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
#include "screenutils.h"

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

extern DebugWindow          *g_debugWindow;

extern TutorialWin          *g_tutorialWin;
extern Player               **g_player;
extern SelectedItem         *g_selected_item;
extern ProfileDB            *g_theProfileDB;

extern Network              g_network;
extern BattleViewWindow     *g_battleViewWindow;
extern sint32               g_modalWindow;

double	g_screenTime = 0.0;

sint32 open_WorkView( void )
{
	sint32      err     = workwin_Initialize();
	Assert( !err );
	if ( err ) return -1;

	AUI_ERRCODE auiErr  = g_c3ui->AddWindow(g_workWindow);
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return -1;

	return 0;
}

void close_WorkView(void)
{
	if (g_workWindow)
    {
		g_c3ui->RemoveWindow(g_workWindow->Id());
	}
}

sint32 open_CityView( void )
{
	AUI_ERRCODE auiErr  = CityWindow::Display();

	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return -1;

	return 0;
}

void close_CityView(void)
{
	CityWindow::Hide();
}

sint32 open_CityStatus( void )
{
#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG

		NationalManagementDialog::Open();

#ifdef _DEBUG
	GET_ELAPSED_TIME( "City Status" );
#endif // _DEBUG

	return 0;
}

void close_CityStatus(void)
{
	NationalManagementDialog::Close();
}

sint32 open_CivStatus()
{
#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG

	DomesticManagementDialog::Open();

#ifdef _DEBUG
	GET_ELAPSED_TIME( "Civ Status" );
#endif // _DEBUG

	return 0;
}

void close_CivStatus(void)
{
	DomesticManagementDialog::Close();
}

sint32 open_ScienceStatus( void )
{
	ScienceManagementDialog::Open();
	return 0;
}

void close_ScienceStatus(void)
{
	ScienceManagementDialog::Close();
}

sint32 open_ScienceVictory( void )
{
	ScienceVictoryDialog::Open();
	return 0;
}

void close_ScienceVictory( void )
{
	ScienceVictoryDialog::Close();
}

sint32 open_UnitStatus( void )
{
	UnitManager::Display();
	return 0;
}

void close_UnitStatus( void )
{
	UnitManager::Hide();
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

void close_TradeStatus( void )
{
	TradeManager::Hide();
}

sint32 open_VictoryWindow( void )
{
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

void close_VictoryWindow( void )
{
	victorywin_RemoveWindow();
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

void close_Diplomacy(void)
{
	DiplomacyWindow::Hide();
}

sint32 open_InfoScreen( void )
{
#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG

	InfoWindow::Open();

#ifdef _DEBUG
	GET_ELAPSED_TIME( "Info" );
#endif // _DEBUG

	return 0;
}

void close_InfoScreen( void )
{
	InfoWindow::Close();
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
// Returns    : sint32              : opening succeeded
//
// Remark(s)  : When the library is not open already, or the sci parameter is
//              set, it will open with the indicated advance.
//
//----------------------------------------------------------------------------
bool open_GreatLibrary(sint32 index, bool sci)
{
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
bool open_GreatLibrary( void )
{
	sint32 const		player	= g_selected_item->GetVisiblePlayer();
	AdvanceType const	advance	= g_player[player]->m_advances->GetResearching();

	return open_GreatLibrary(advance);
}

void close_GreatLibrary(void)
{
	if (g_greatLibrary)
    {
		g_greatLibrary->Remove();
	}
}

sint32 open_OptionsScreen( sint32 fromWhichScreen )
{
#ifdef _DEBUG
	SET_TIME
#endif // _DEBUG

	sint32 err = optionsscreen_displayMyWindow( fromWhichScreen );

#ifdef _DEBUG
	GET_ELAPSED_TIME( "Options Screen" );
#endif // _DEBUG

	return err;
}

void close_OptionsScreen( void )
{
	optionsscreen_removeMyWindow(AUI_BUTTON_ACTION_EXECUTE);
}

sint32 open_KeyMappingScreen( void )
{
	return km_screen_displayMyWindow();
}

sint32 open_ScenarioEditor(void)
{
	return ScenarioEditor::Display();
}

void close_ScenarioEditor(void)
{
	ScenarioEditor::Hide();
}

void close_KeyMappingScreen( void )
{
	km_screen_removeMyWindow(AUI_BUTTON_ACTION_EXECUTE);
}






























/// Open alien life window (removed CTP1 functionality)
sint32 open_EndGame()
{
	return 0;
}

/// Close alien life window (removed CTP1 functionality)
void close_EndGame(void)
{
}

sint32 open_TutorialWin( void )
{
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

void close_TutorialWin(void)
{
	if (g_tutorialWin)
    {
    	g_tutorialWin->Remove();
    }
}


sint32 open_CreditsScreen()
{
	sint32 err = creditsscreen_Initialize();
	Assert(!err);
	if(err) return(-1);

	AUI_ERRCODE auiErr = g_c3ui->AddWindow(g_creditsWindow);
	Assert(auiErr == AUI_ERRCODE_OK);
    return (auiErr == AUI_ERRCODE_OK) ? 0 : -1;
}

void close_CreditsScreen(void)
{
	creditsscreen_Cleanup();
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
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void close_AllScreens(void)
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

	if (g_battleViewWindow)
    {
		g_modalWindow = 1;

		battleview_ExitButtonActionCallback
            (NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);
	}
    else
    {
	    g_modalWindow = 0;
    }
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
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void close_AllScreensAndUpdateInfoScreen(void)
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

	if (g_battleViewWindow)
    {
		g_modalWindow = 1;
		battleview_ExitButtonActionCallback
            (NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);
	}
    else
    {
        g_modalWindow = 0;
    }
}
