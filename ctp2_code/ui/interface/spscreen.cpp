
#include "c3.h"

#include "civapp.h"


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
#include "c3_listitem.h"

#include "initialplaywindow.h"
#include "spnewgamewindow.h"
#include "scenariowindow.h"
#include "optionswindow.h"
#include "loadsavewindow.h"
#include "spnewgamemapsizescreen.h"

#include "civ3_main.h"

#include "ProfileDB.h"

#include "spWindow.h"

extern C3UI					*g_c3ui;
extern CivApp				*g_civApp;
extern ProfileDB			*g_theProfileDB;


SPWindow					*g_spWindow		= NULL;




sint32	spscreen_displayMyWindow()
{
	sint32 retval=0;
	if(!g_spWindow) { retval = spscreen_Initialize(); }

	extern bool g_e3Demo;
	if(g_e3Demo) return 0;

	g_c3ui->AddWindow(g_spWindow);

	
	return retval;
}
sint32 spscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( g_spWindow->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	return 1;
}



AUI_ERRCODE spscreen_Initialize( void )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( g_spWindow ) return AUI_ERRCODE_OK; 


	strcpy(windowBlock, "SPWindow");

	g_spWindow= new SPWindow(&errcode, aui_UniqueId(), windowBlock, 16 );
	Assert( AUI_NEWOK(g_spWindow, errcode) );
	if ( !AUI_NEWOK(g_spWindow, errcode) ) return errcode;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE spscreen_Cleanup()
{
	if ( !g_spWindow  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( g_spWindow->Id() );

	delete g_spWindow;
	g_spWindow = NULL;

	return AUI_ERRCODE_OK;
}





void
spscreen_tutorialPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	
	
	if (c3files_HasLegalCD()) {
		if(spscreen_removeMyWindow(action)) {
			
			g_theProfileDB->SetTutorialAdvice(TRUE);

			
			g_civApp->PostStartGameAction();
		}
	}
}
void
spscreen_newgamePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	if(spscreen_removeMyWindow(action)) 
		spnewgamescreen_displayMyWindow();
}




void spscreen_loadScenarioGameCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	loadsavescreen_displayMyWindow(LSS_LOAD_GAME);
}




















void
spscreen_loadgamePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
		loadsavescreen_displayMyWindow(LSS_LOAD_GAME);

}
void
spscreen_instaPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	if(spscreen_removeMyWindow(action))
		g_civApp->PostStartGameAction();
}
void
spscreen_returnPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	if(spscreen_removeMyWindow(action))
		initialplayscreen_displayMyWindow();
}
void
spscreen_quitPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	spscreen_removeMyWindow(action);

	
		ExitGame();
}


void
spscreen_mapPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	spnewgamemapsizescreen_displayMyWindow( FALSE, 1 );
}
void
spscreen_optionsPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	
	
	

		optionsscreen_displayMyWindow(0);
}
