#include "ctp/c3.h"

#include "ui/aui_common/aui_ui.h"

#include "ui/netshell/netshell.h"

#include "ui/netshell/allinonewindow.h"
#include "ui/interface/loadsavewindow.h"
#include "ui/interface/scenariowindow.h"

#include "ui/interface/initialplaywindow.h"

#include "net/general/network.h"
#include "ctp/civapp.h"

#include "ui/netshell/netshell_game.h"
#include "gs/fileio/civscenarios.h"
extern CivScenarios *g_civScenarios;

extern CivApp *g_civApp;


void EnterMainMenu( void )
{
	initialplayscreen_Initialize();
	initialplayscreen_displayMyWindow();
}


void LeaveMainMenu( void )
{

	g_ui->AddAction( new DestroyInitialPlayScreenAction );
}


void LaunchGame( void )
{
	AllinoneWindow *w = g_allinoneWindow;


	switch ( w->GetMode() )
	{
	case w->CONTINUE_CREATE:
	case w->CONTINUE_JOIN:

		{
			g_network.SetLaunchFromNetFunc(TRUE);
			loadsavescreen_LoadMPGame();
		}






		break;

	case w->CREATE:

		g_network.SetLaunchFromNetFunc(FALSE);

		if(w->IsScenarioGame()) {
			if(w->GetScenarioInfo()->m_haveSavedGame) {
				ScenarioPack *pack;
				Scenario *scen;
				if(g_civScenarios->FindScenario(g_scenarioName,
												&pack, &scen)) {
					MBCHAR path[_MAX_PATH];
					sprintf(path, "%s\\%s",
							scen->m_path,
							k_SCENARIO_DEFAULT_SAVED_GAME_NAME);
					g_civApp->PostLoadSaveGameAction(path);
					break;
				}
			}
		}
		g_civApp->PostStartGameAction();
		break;

	case w->JOIN:
		g_network.SetLaunchFromNetFunc(FALSE);
		g_civApp->PostStartGameAction();
		break;

	default:

		Assert( FALSE );
		break;
	}
}




void DestroyInitialPlayScreenAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	initialplayscreen_Cleanup();
}
