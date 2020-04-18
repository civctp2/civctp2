//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Single player new game screen
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
// - Clean up any created subscreens when cleaning up the main screen.
// - Always return to main menu, never SP menu (JJB)
// - Repaired memory leaks.
// - Added tribe index check.
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "spnewgamewindow.h"    // spnewgamescreen.h does not exist

#include "aui.h"
#include "aui_ldl.h"
#include "aui_stringtable.h"
#include "aui_switchgroup.h"
#include "aui_textfield.h"
#include "aui_uniqueid.h"
#include "c3_button.h"
#include "c3_checkbox.h"
#include "c3_dropdown.h"
#include "c3_listbox.h"
#include "c3_listitem.h"
#include "c3_popupwindow.h"
#include "c3_static.h"
#include "c3_switch.h"
#include "c3slider.h"
#include "c3textfield.h"
#include "c3ui.h"
#include "c3window.h"
#include "civ3_main.h"
#include "civapp.h"
#include "civscenarios.h"
#include "ctp2_button.h"
#include "custommapscreen.h"
#include "gamefile.h"
#include "gameinit.h"
#include "Globals.h"
#include "hotseatlist.h"
#include "initialplaywindow.h"
#include "loadsavewindow.h"
#include "MessageBoxDialog.h"
#include "profileDB.h"                      // g_theProfileDB
#include "scenariowindow.h"
#include "scorewarn.h"
#include "SlicEngine.h"
#include "spnewgamediffscreen.h"
#include "spnewgamemapshapescreen.h"
#include "spnewgamemapsizescreen.h"
#include "spnewgameplayersscreen.h"
#include "spnewgamerandomcustomscreen.h"
#include "spnewgamerulesscreen.h"
#include "spnewgametribescreen.h"
#include "TurnYearStatus.h"

extern C3UI                 *g_c3ui;
extern CivApp               *g_civApp;
extern c3_PopupWindow       *g_spNewGameTribeScreen;
extern MBCHAR               g_slic_filename[_MAX_PATH];
extern MBCHAR               g_civilisation_filename[_MAX_PATH];

SPNewGameWindow             *g_spNewGameWindow      = NULL;

BOOL                        g_launchIntoCheatMode = FALSE;

void spnewgamescreen_SetupHotseatOrEmail();

sint32 spnewgamescreen_displayMyWindow()
{

	g_launchIntoCheatMode = FALSE;

	sint32 retval = g_spNewGameWindow ? 0 : spnewgamescreen_Initialize();

	if ( !g_spNewGameTribeScreen ) spnewgametribescreen_Initialize();

	sint32 const    tribeIndex = spnewgametribescreen_getTribeIndex();
	if ((tribeIndex < 0) || (tribeIndex >= INDEX_TRIBE_INVALID))
	{
		spnewgamescreen_setPlayerName(g_theProfileDB->GetLeaderName());
	}
	else
	{
		MBCHAR lname[k_MAX_NAME_LEN + 1];
		spnewgametribescreen_getLeaderName( lname );
		spnewgamescreen_setPlayerName( lname );
	}

	if (g_spNewGameWindow)
	{
		g_theProfileDB->DefaultSettings();
		g_spNewGameWindow->m_useCustomMap=false;
		g_civPaths->ClearCurScenarioPath();

		if (g_slicEngine)
		{
			SlicEngine::Reload(g_slic_filename);
		}

		g_spNewGameWindow->Update();
		g_c3ui->AddWindow(g_spNewGameWindow);
	}

	return retval;
}

sint32 spnewgamescreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow( g_spNewGameWindow->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	return 1;
}




AUI_ERRCODE spnewgamescreen_Initialize( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	if (!g_spNewGameWindow)
	{
		MBCHAR windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
		strcpy(windowBlock, "SPNewGameWindow");

		g_spNewGameWindow= new SPNewGameWindow(&errcode, aui_UniqueId(), windowBlock, 16 );
		Assert(AUI_NEWOK(g_spNewGameWindow, errcode));
		if (AUI_NEWOK(g_spNewGameWindow, errcode))
		{
			errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
			Assert(AUI_SUCCESS(errcode));
		}
	}

	return errcode;
}

//----------------------------------------------------------------------------
//
// Name       : spnewgamescreen_Cleanup
//
// Description: Release the memory of the single player new game screen(s).
//
// Parameters : -
//
// Globals    : g_spNewGameWindow
//              g_c3ui
//
// Returns    : -
//
// Remark(s)  : Clean up the subscreens as well.
//
//----------------------------------------------------------------------------
void spnewgamescreen_Cleanup(void)
{
	// Clean up subscreens.
	spnewgamediffscreen_Cleanup();
	spnewgamemapshapescreen_Cleanup();
	spnewgamemapsizescreen_Cleanup();
	spnewgameplayersscreen_Cleanup();
	spnewgamerandomcustomscreen_Cleanup();
	spnewgamerulesscreen_Cleanup();
	spnewgametribescreen_Cleanup();
	custommapscreen_Cleanup();
	scenarioscreen_Cleanup();

	allocated::clear(g_hotseatList);

	// Clean up main screen
	if (g_spNewGameWindow)
	{
		g_c3ui->RemoveWindow(g_spNewGameWindow->Id());
		allocated::clear(g_spNewGameWindow);
	}
}

sint32 spnewgamescreen_setPlayerName( const MBCHAR *name )
{

	if ( g_spNewGameWindow )
		g_spNewGameWindow->m_spName->SetFieldText( name );

	return 1;
}

sint32 spnewgamescreen_update( void )
{

	if(g_spNewGameWindow)
		g_spNewGameWindow->Update();

	return 1;
}





sint32 callbackSetSelected(aui_Control *control, void *cookie)
{
	if(control && cookie) {
		c3_DropDown	*mydrop		= (c3_DropDown*)cookie;
		c3_ListBox	*mylist		= (c3_ListBox*) control;
		uint32		index		= mylist->GetSelectedItemIndex();

		mydrop->SetSelectedItem(index);
		return index;
	}
	return -1;
}





void
spnewgamescreen_instaPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{

}

void
spnewgamescreen_startPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action == uint32(AUI_BUTTON_ACTION_EXECUTE) )
	{
#if defined(USE_SDL)
		bool legalGame = true;
#else // USE_SDL
		bool legalGame = c3files_HasLegalCD();
#endif // USE_SDL
		if (legalGame)
		{
			MBCHAR fieldText[k_MAX_NAME_LEN];
			g_spNewGameWindow->m_spName->GetFieldText(fieldText, k_MAX_NAME_LEN);
			g_theProfileDB->SetLeaderName(fieldText);

			if (g_startEmailGame || g_startHotseatGame)
			{
				spnewgamescreen_SetupHotseatOrEmail();
			}
			else
			{
				spnewgamescreen_removeMyWindow(action);

				g_theProfileDB->SetSaveNote("");
				g_theProfileDB->SetTutorialAdvice(FALSE);
				g_civApp->PostStartGameAction();
			}
		}
	}
}

void
spnewgamescreen_returnPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

#if 0 // not used
	bool wasPBEMOrHotseat = g_startHotseatGame || g_startEmailGame;
#endif

	g_startHotseatGame  = FALSE;
	g_startEmailGame    = FALSE;
	g_isScenario        = FALSE;
	memset(g_scenarioName, '\0', k_SCENARIO_NAME_MAX);
	g_civPaths->ClearCurScenarioPath();
	g_civPaths->ClearCurScenarioPackPath();

	if (spnewgamescreen_removeMyWindow(action))
	{
		// In the new interface there is no SP window
		initialplayscreen_displayMyWindow();
	}

	ScenarioWindow::Hide();
}

void
spnewgamescreen_quitPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	spnewgamescreen_removeMyWindow(action);

	ExitGame();
}




void spnewgamescreen_tribePress( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;


	spnewgametribescreen_displayMyWindow( NULL, TRUE );

	sint32 index = (sint32)g_theProfileDB->GetCivIndex();

	const sint32 size = k_MAX_NAME_LEN;
	MBCHAR lname[ size + 1 ];
	g_spNewGameWindow->m_spName->GetFieldText( lname, size );

	spnewgametribescreen_setTribeIndex( index, lname );
}
void spnewgamescreen_malePress( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
}

void spnewgamescreen_femalePress( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
}

void spnewgamescreen_difficultyPress( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	spnewgamediffscreen_displayMyWindow(FALSE,TRUE);
}

void spnewgamescreen_mapSizePress( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	spnewgamemapsizescreen_displayMyWindow( FALSE, 0 );
}

void spnewgamescreen_playersPress( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	spnewgameplayersscreen_Cleanup();
	spnewgameplayersscreen_displayMyWindow();
}

void spnewgamescreen_mapPress( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	custommapscreen_displayMyWindow();
}

void spnewgamescreen_rulesPress( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	spnewgamerulesscreen_displayMyWindow();
}

void spnewgamescreen_editorPress( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_launchIntoCheatMode = TRUE;

	disclaimer_Initialize(spnewgamescreen_startPress);

}





void spnewgamescreen_scenarioExitCallback(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	MBCHAR	tempPath[_MAX_PATH];
	sprintf(tempPath, "%s%s%s",
	        g_civPaths->GetCurScenarioPath(),
	        FILE_SEP,
	        k_SCENARIO_DEFAULT_SAVED_GAME_NAME
	       );

	if (c3files_PathIsValid(tempPath)) {

#if !defined(USE_SDL)
		if(!c3files_HasLegalCD())
			exit(0);
#endif // USE_SDL

		SaveInfo *saveInfo = new SaveInfo;

		strcpy(saveInfo->fileName, k_SCENARIO_DEFAULT_SAVED_GAME_NAME);

		strcpy(saveInfo->pathName, tempPath);

		if (GameFile::FetchExtendedSaveInfo(tempPath, saveInfo)) {
			MBCHAR scenPath[_MAX_PATH];
			strcpy(scenPath, g_civPaths->GetCurScenarioPath());
			g_startInfoType = saveInfo->startInfoType;
			loadsavescreen_BeginLoadProcess(saveInfo, scenPath);
		}

		delete saveInfo;

		return;
	}

	g_spNewGameWindow->Update();
}





void spnewgamescreen_scenarioPress(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if (g_civPaths->GetCurScenarioPath())
	{
		g_civPaths->ClearCurScenarioPath();
		g_spNewGameWindow->Update();
	}
	else
	{
		scenarioscreen_displayMyWindow();
		scenarioscreen_SetExitCallback(spnewgamescreen_scenarioExitCallback);
	}
}

void spnewgamescreen_mapTypePress( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	spnewgamerandomcustomscreen_displayMyWindow();
}

void spnewgamescreen_worldShapePress( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	spnewgamemapshapescreen_displayMyWindow();
}


void spnewgamescreen_clanSelect(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
}

void spnewgamescreen_genderSelect(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT  ) return;

	if(g_spNewGameWindow) callbackSetSelected(control,cookie);
}
void
spnewgamescreen_preferencePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{

}
void
spnewgamescreen_pCustomPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{

}

void spnewgamescreen_mapSizeSelect(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT  ) return;

	if(g_spNewGameWindow) {
		callbackSetSelected(control,cookie);

		switch(((c3_ListBox*)control)->GetSelectedItemIndex()) {
		case 0:
			g_theProfileDB->SetMapSize( MAPSIZE_SMALL);

			break;
		case 1:
			g_theProfileDB->SetMapSize( MAPSIZE_MEDIUM);

			break;
		case 2:
			g_theProfileDB->SetMapSize( MAPSIZE_LARGE);

			break;
		case 3:
			g_theProfileDB->SetMapSize( MAPSIZE_GIGANTIC);

			break;
		default: Assert(0);
		}
	}
}
void spnewgamescreen_worldTypeSelect(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT  ) return;

	if(g_spNewGameWindow) callbackSetSelected(control,cookie);
}
void spnewgamescreen_worldShapeSelect(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT  ) return;

	if(g_spNewGameWindow) callbackSetSelected(control,cookie);
}
void spnewgamescreen_difficultySelect(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT  ) return;

	if(g_spNewGameWindow) {
		callbackSetSelected(control,cookie);

		g_theProfileDB->SetDifficulty( ((c3_ListBox*)control)->GetSelectedItemIndex());
	}
}
void spnewgamescreen_riskLevelSelect(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT  ) return;

	if(g_spNewGameWindow) {
		callbackSetSelected(control,cookie);

		g_theProfileDB->SetRiskLevel( ((c3_ListBox*)control)->GetSelectedItemIndex() );
	}
}
void
spnewgamescreen_opponentSelect(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT  ) return;

	if(g_spNewGameWindow) {
		callbackSetSelected(control,cookie);

		g_theProfileDB->SetNPlayers( ((c3_ListBox*)control)->GetSelectedItemIndex()+3 );
	}
}
void
spnewgamescreen_wCustomPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

		custommapscreen_displayMyWindow();
}

void
spnewgamescreen_genocidePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action == uint32(AUI_SWITCH_ACTION_PRESS) ) {
		uint32 state = data;
//		c3_CheckBox *mycheckbox = (c3_CheckBox*)control;

		g_theProfileDB->SetGenocideRule( state ? FALSE : TRUE );
	}
}
void
spnewgamescreen_tradePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action == uint32(AUI_SWITCH_ACTION_PRESS) ) {
		uint32 state = data;
//		c3_CheckBox *mycheckbox = (c3_CheckBox*)control;

		g_theProfileDB->SetTradeRule( state ? FALSE : TRUE );
	}
}
void
spnewgamescreen_combatPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action == uint32(AUI_SWITCH_ACTION_PRESS) ) {
		uint32 state = data;
//		c3_CheckBox *mycheckbox = (c3_CheckBox*)control;

		g_theProfileDB->SetSimpleCombatRule( state ? FALSE : TRUE );
	}
}
void
spnewgamescreen_pollutionPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action == uint32(AUI_SWITCH_ACTION_PRESS) ) {
		uint32 state = data;
//		c3_CheckBox *mycheckbox = (c3_CheckBox*)control;

		g_theProfileDB->SetPollutionRule( state ? FALSE : TRUE );
	}
}

c3_Button* spNew_c3_Button(AUI_ERRCODE *errcode, const MBCHAR *ldlParent, const MBCHAR *ldlMe,
					void (*callback)(aui_Control*,uint32,uint32,void*))
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( textBlock, "%s.%s", ldlParent, ldlMe );

	return new c3_Button(errcode, aui_UniqueId(), textBlock, callback);
}

ctp2_Button* spNew_ctp2_Button(AUI_ERRCODE *errcode, const MBCHAR *ldlParent, const MBCHAR *ldlMe,
							   void (*callback)(aui_Control*,uint32,uint32,void*))
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( textBlock, "%s.%s", ldlParent, ldlMe );

	return new ctp2_Button(errcode, aui_UniqueId(), textBlock, callback);
}




ctp2_Button*
spNew_ctp2_Button(AUI_ERRCODE *errcode,
				  const MBCHAR *ldlParent,
				  const MBCHAR *ldlMe,
				  const MBCHAR *default_text,
				  void (*callback)(aui_Control*,uint32,uint32,void*),
				  const MBCHAR *buttonFlavor)
{
	MBCHAR		textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if (ldlParent==NULL)
		sprintf( textBlock, "%s",ldlMe );
	else
		sprintf( textBlock, "%s.%s", ldlParent, ldlMe );

	return new ctp2_Button
	    (errcode, aui_UniqueId(), textBlock,
	     buttonFlavor,
	     500, 10,
	     100, 20,
	     NULL,
	     (void*)callback
	    );
}


c3_Switch* spNew_c3_Switch(AUI_ERRCODE *errcode, const MBCHAR *ldlParent, const MBCHAR *ldlMe,
					void (*callback)(aui_Control*,uint32,uint32,void*), void *cookie)
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( textBlock, "%s.%s", ldlParent, ldlMe );

	return new c3_Switch( errcode, aui_UniqueId(), textBlock, callback, cookie );
}

aui_Switch* spNew_aui_Switch(
	AUI_ERRCODE *errcode,
	const MBCHAR *ldlParent, const MBCHAR *ldlMe,
	void (*callback)(aui_Control*,uint32,uint32,void*),
	void *cookie)
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( textBlock, "%s.%s", ldlParent, ldlMe );

	return new aui_Switch( errcode, aui_UniqueId(), textBlock, callback, cookie );
}

c3_ListBox* spNew_c3_ListBox(AUI_ERRCODE *errcode, const MBCHAR *ldlParent, const MBCHAR *ldlMe,
					void (*callback)(aui_Control*,uint32,uint32,void*),
					void *cookie)
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( textBlock, "%s.%s", ldlParent, ldlMe );

	return new c3_ListBox(errcode,aui_UniqueId(), textBlock, callback,cookie );
}

c3_DropDown* spNew_c3_DropDown(AUI_ERRCODE *errcode, const MBCHAR *ldlParent, const MBCHAR *ldlMe,
					void (*callback)(aui_Control*,uint32,uint32,void*))
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf(textBlock, "%s.%s", ldlParent, ldlMe);

	c3_DropDown * myDropDown = new c3_DropDown( errcode, aui_UniqueId(), textBlock);
	myDropDown->GetListBox()->SetActionFuncAndCookie(callback,myDropDown);

	return myDropDown;
}

aui_StringTable* spNewStringTable(AUI_ERRCODE *errcode, const MBCHAR *ldlme)
{
	return new aui_StringTable(errcode, ldlme);
}

void spFillDropDown(AUI_ERRCODE *retval, c3_DropDown *mydrop, aui_StringTable *mytable, const MBCHAR *listitemparent, const MBCHAR *listitemme)
{
	for (sint32 i = 0; i < mytable->GetNumStrings(); ++i)
	{
		mydrop->AddItem
		    (new SPDropDownListItem
		        (retval, listitemparent, listitemme, mytable->GetString(i))
		    );
	}
}
void spFillListBox(AUI_ERRCODE *retval, c3_ListBox *mylist, aui_StringTable *mytable, const MBCHAR *listitemparent, const MBCHAR *listitemme)
{
	for (sint32 i = 0; i < mytable->GetNumStrings(); i++)
		{
		mylist->AddItem
		    (new SPDropDownListItem
		        (retval, listitemparent, listitemme, mytable->GetString(i))
		    );
	}
}

c3_Static* spNew_c3_Static(AUI_ERRCODE *errcode, const MBCHAR *ldlParent, const MBCHAR *ldlMe)
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( textBlock, "%s.%s", ldlParent, ldlMe );

	return new c3_Static(errcode, aui_UniqueId(), textBlock);
}

C3TextField* spNewTextEntry(AUI_ERRCODE *errcode, const MBCHAR *ldlParent, const MBCHAR *ldlMe,
					void (*callback)(aui_Control*,uint32,uint32,void*),void *cookie )

{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( textBlock, "%s.%s", ldlParent, ldlMe );

	return new C3TextField( errcode, aui_UniqueId(), textBlock, callback, cookie);
}

TwoChoiceButton* spNewTwoChoiceButton(AUI_ERRCODE *errcode, const MBCHAR* ldlParent, const MBCHAR *ldlMe,
									  const MBCHAR *ldlstringtable, uint32 state,
					void (*callback)(aui_Control*,uint32,uint32,void*))
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	const MBCHAR *c0= NULL,*c1= NULL;
	aui_StringTable * choices = spNewStringTable(errcode,ldlstringtable);
	if(choices && choices->GetNumStrings()==2)
	{ c0 = choices->GetString(0); c1 = choices->GetString(1); }

	sprintf( textBlock, "%s.%s", ldlParent, ldlMe );
	TwoChoiceButton * mybutton = new TwoChoiceButton(errcode,aui_UniqueId(),textBlock,c0,c1,state,callback);

	delete choices;
	return mybutton;
}

C3Slider* spNew_C3Slider(AUI_ERRCODE *errcode, const MBCHAR *ldlParent, const MBCHAR *ldlMe,
	 					void (*callback)(aui_Control*,uint32,uint32,void*))
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( textBlock, "%s.%s", ldlParent, ldlMe );

	return new C3Slider(errcode, aui_UniqueId(), textBlock, callback);
}
c3_CheckBox* spNew_c3_CheckBox(AUI_ERRCODE *errcode, const MBCHAR* ldlParent, const MBCHAR *ldlMe,
					uint32 state, void (*callback)(aui_Control*,uint32,uint32,void*), void*cookie)
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( textBlock, "%s.%s", ldlParent, ldlMe );

	c3_CheckBox * mycheck = new c3_CheckBox(errcode,aui_UniqueId(),textBlock,callback,cookie);
	mycheck->SetState(state);

	return mycheck;
}
aui_SwitchGroup* spNew_aui_SwitchGroup( AUI_ERRCODE *errcode, const MBCHAR *ldlParent, const MBCHAR *ldlMe )
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( textBlock, "%s.%s", ldlParent, ldlMe );

	return new aui_SwitchGroup(errcode, aui_UniqueId(), textBlock);
}

void spnewgamescreen_HotseatCallback(sint32 launch, sint32 player,
									 sint32 civ, BOOL human,
									 const MBCHAR *name, const MBCHAR *email)
{
	if(launch) {
		DPRINTF(k_DBG_GAMESTATE, ("Hotseat callback: %d, %d, %d, %s\n", player, civ, human, email));

		MBCHAR fieldText[k_MAX_NAME_LEN];

		g_spNewGameWindow->m_spName->GetFieldText(fieldText, k_MAX_NAME_LEN);
		g_theProfileDB->SetLeaderName(fieldText);





		g_theProfileDB->SetSaveNote("");


		g_theProfileDB->SetTutorialAdvice(FALSE);

		g_civApp->PostStartGameAction();
	} else {

		g_hsPlayerSetup[player].civ = civ;
		g_hsPlayerSetup[player].isHuman = human;
		delete [] g_hsPlayerSetup[player].name;
		delete [] g_hsPlayerSetup[player].email;
		g_hsPlayerSetup[player].name = new MBCHAR[strlen(name) + 1];
		strcpy(g_hsPlayerSetup[player].name, name);
		g_hsPlayerSetup[player].email = new MBCHAR[strlen(email) + 1];
		strcpy(g_hsPlayerSetup[player].email, email);
	}
}

void spnewgamescreen_SetupHotseatOrEmail()
{
	memset(g_hsPlayerSetup, 0, sizeof(g_hsPlayerSetup));

	hotseatlist_ClearOptions();
	hotseatlist_EnableAllCivs();

	if(!g_hotseatList) {
		g_hotseatList = new HotseatList((HotseatListCallback *)spnewgamescreen_HotseatCallback);
	}
	g_hotseatList->DisplayWindow();
}
