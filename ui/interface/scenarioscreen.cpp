



#include "c3.h"


#include "aui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_stringtable.h"
#include "aui_textfield.h"
#include "c3ui.h"
#include "c3window.h"
#include "c3_static.h"
#include "c3_button.h"
#include "c3_dropdown.h"
#include "c3_listbox.h"
#include "c3_listitem.h"

#include "spwindow.h"
#include "spnewgamewindow.h"
#include "ScenarioWindow.h"

#include "ProfileDB.h"

#include "CivApp.h"
#include "CivPaths.h"
#include "CivScenarios.h"

extern C3UI					*g_c3ui;
extern ProfileDB			*g_theProfileDB;
extern CivScenarios			*g_civScenarios;
extern CivPaths				*g_civPaths;
extern CivApp				*g_civApp;

extern SPNewGameWindow		*g_spNewGameWindow;







sint32	scenarioscreen_displayMyWindow()
{
	ScenarioWindow::Display();

	sint32 retval=0;
	

	

	
	return retval;
}

sint32 scenarioscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	ScenarioWindow::Hide();

	

	
	

	return 1;
}


sint32 scenarioscreen_SetExitCallback(aui_Control::ControlActionCallback *callback)
{
	ScenarioWindow::SetExitCallback(callback);

	return 0;
}




AUI_ERRCODE scenarioscreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	ScenarioWindow::Initialize();

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE scenarioscreen_Cleanup()
{
	ScenarioWindow::Cleanup();
	return AUI_ERRCODE_OK;
}










void CloseScenarioScreenAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	
	
	
	
	


	
	scenarioscreen_removeMyWindow( AUI_BUTTON_ACTION_EXECUTE );

	if (g_spNewGameWindow) {
		g_spNewGameWindow->Update();
	}
}
