










#include "c3.h"

#include "aui_ui.h"

#include "netshell.h"

#include "allinonewindow.h"
#include "loadsavewindow.h"
#include "scenariowindow.h"

#include "initialplaywindow.h"

#include "network.h"
#include "civapp.h"

#include "netshell_game.h"
#include "CivScenarios.h"
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
