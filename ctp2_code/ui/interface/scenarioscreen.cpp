#include "ctp/c3.h"

#include "ui/aui_common/aui.h"
#include "ui/aui_common/aui_ldl.h"
#include "ui/aui_common/aui_uniqueid.h"
#include "ui/aui_common/aui_stringtable.h"
#include "ui/aui_common/aui_textfield.h"
#include "ui/aui_ctp2/c3ui.h"
#include "ui/aui_ctp2/c3window.h"
#include "ui/aui_ctp2/c3_static.h"
#include "ui/aui_ctp2/c3_button.h"
#include "ui/aui_ctp2/c3_dropdown.h"
#include "ui/aui_ctp2/c3_listbox.h"
#include "ui/aui_ctp2/c3_listitem.h"

#include "ui/interface/spnewgamewindow.h"
#include "ui/interface/scenariowindow.h"

#include "gs/database/profileDB.h"

#include "ctp/civapp.h"
#include "gs/fileio/CivPaths.h"
#include "gs/fileio/civscenarios.h"

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
