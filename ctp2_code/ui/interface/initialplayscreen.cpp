//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Main menu screen
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Shifted buttons from the "Single Player" subscreen into this one to
//   simplify the interface.
//   (JJB)
// - Retrieve the modification date from the executable.
// - Added mod compatibility crash fix.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "initialplaywindow.h"  // initialplayscreen.h does not exist

#include "civ3_main.h"
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
#include "CivPaths.h"           // g_civPaths
#include "ctp2_listitem.h"
#include "ctp2_dropdown.h"
#include "ctp2_listbox.h"
#include "ctp2_Static.h"
#include "gameinit.h"
#include "loadsavewindow.h"
#include "netshell.h"
#include "optionswindow.h"
#include "profileDB.h"          // g_theProfileDB
#include "scenariowindow.h"
#include "screenutils.h"
#include <string>               // std::basic_string
#include "UIUtils.h"

extern  C3UI				*g_c3ui;
extern  CivApp				*g_civApp;
extern  BOOL                             g_runSpriteEditor;

namespace Os
{
    extern std::basic_string<TCHAR> GetExeVersion(void);
}

static C3Window				*s_initplayWindow		= NULL;
static MBCHAR				*s_initplayWindowLDLBlock = "InitPlayWindow";

sint32	initialplayscreen_displayMyWindow()
{
    sint32 retval = (s_initplayWindow) ? 0 : initialplayscreen_Initialize();

	g_c3ui->AddWindow(s_initplayWindow);

	return retval;
}
sint32 initialplayscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

    if (g_c3ui && s_initplayWindow)
    {
	    (void) g_c3ui->RemoveWindow(s_initplayWindow->Id());
    }

	return 1;
}


AUI_ERRCODE initialplayscreen_Initialize( void )
{
	if ( s_initplayWindow )
		return AUI_ERRCODE_OK;

	s_initplayWindow = (C3Window *)aui_Ldl::BuildHierarchyFromRoot(s_initplayWindowLDLBlock);
	Assert (s_initplayWindow != NULL);
	if (s_initplayWindow == NULL)
		return AUI_ERRCODE_INVALIDPARAM;

	AUI_ERRCODE errcode =
        aui_Ldl::SetActionFuncAndCookie(s_initplayWindowLDLBlock, "SpriteTestButton",
										spritetest_spPress, NULL);
	Assert(errcode == AUI_ERRCODE_OK);

	if (!g_runSpriteEditor){
	  ctp2_Button *spriteTest = (ctp2_Button *)aui_Ldl::GetObject(s_initplayWindowLDLBlock, "SpriteTestButton");
	  spriteTest->Hide();
	}
	
	errcode = aui_Ldl::SetActionFuncAndCookie(s_initplayWindowLDLBlock, "EmailButton",
											initialplayscreen_emailPress, NULL);
	Assert(errcode == AUI_ERRCODE_OK);

	errcode = aui_Ldl::SetActionFuncAndCookie(s_initplayWindowLDLBlock, "HotseatButton",
											initialplayscreen_hotseatPress, NULL);
	Assert(errcode == AUI_ERRCODE_OK);

	errcode = aui_Ldl::SetActionFuncAndCookie(s_initplayWindowLDLBlock, "MPButton",
											initialplayscreen_mpPress, NULL);
	Assert(errcode == AUI_ERRCODE_OK);

	errcode = aui_Ldl::SetActionFuncAndCookie(s_initplayWindowLDLBlock, "QuitButton",
											initialplayscreen_quitPress, NULL);
	Assert(errcode == AUI_ERRCODE_OK);

	errcode = aui_Ldl::SetActionFuncAndCookie(s_initplayWindowLDLBlock, "CreditsButton",
											initialplayscreen_creditsPress, NULL);
	Assert(errcode == AUI_ERRCODE_OK);

	if (aui_Ldl::GetObject(s_initplayWindowLDLBlock, "NewGameButton"))
    {
	    // Assume Apolyton initial screen layout
	    (void) aui_Ldl::SetActionFuncAndCookie
            (s_initplayWindowLDLBlock, "NewGameButton", initialplayscreen_newgamePress, NULL);
	    (void) aui_Ldl::SetActionFuncAndCookie
            (s_initplayWindowLDLBlock, "LoadGameButton", initialplayscreen_loadgamePress, NULL);
	    (void) aui_Ldl::SetActionFuncAndCookie
            (s_initplayWindowLDLBlock, "TutorialButton", initialplayscreen_tutorialPress, NULL);
	    (void) aui_Ldl::SetActionFuncAndCookie
            (s_initplayWindowLDLBlock, "OptionsButton", initialplayscreen_optionsPress, NULL);
    }
    else
    {
        // Original game layout compatibility handler (sort of)
        (void) aui_Ldl::SetActionFuncAndCookie
            (s_initplayWindowLDLBlock, "SPButton", initialplayscreen_newgamePress, NULL);
    }

	// Display executable version
	ctp2_Static * versionText = reinterpret_cast<ctp2_Static *>
		(aui_Ldl::GetObject(s_initplayWindowLDLBlock, "VersionString"));
	if (versionText)
	{
        versionText->SetText(Os::GetExeVersion().c_str());
	}

	return AUI_ERRCODE_OK;
}


void initialplayscreen_Cleanup(void)
{
	if (s_initplayWindow)
    {
        if (g_c3ui)
        {
	        g_c3ui->RemoveWindow(s_initplayWindow->Id());
        }

	    AUI_ERRCODE errcode =
            aui_Ldl::DeleteHierarchyFromRoot(s_initplayWindowLDLBlock);
	    Assert(errcode == AUI_ERRCODE_OK);
    	s_initplayWindow = NULL;
    }
}






void
spritetest_spPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if(initialplayscreen_removeMyWindow(action))
	{

			MBCHAR fieldText[k_MAX_NAME_LEN];

			sprintf(fieldText,"Jerry");


  			g_theProfileDB->SetLeaderName(fieldText);

			g_theProfileDB->SetSaveNote("");


			g_theProfileDB->SetTutorialAdvice(FALSE);

			g_theProfileDB->SetFogOfWar(false);

			g_civApp->PostSpriteTestAction();
	}
}

void
initialplayscreen_mpPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	control->Enable(FALSE);

	NetShell::Enter( k_NS_FLAGS_CREATE );
}

void initialplayscreen_hotseatPress(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	if(initialplayscreen_removeMyWindow(action)) {
		g_startHotseatGame = TRUE;
		spnewgamescreen_displayMyWindow();
	}
}

void initialplayscreen_emailPress(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	if(initialplayscreen_removeMyWindow(action)) {
		g_startEmailGame = TRUE;
		spnewgamescreen_displayMyWindow();
	}
}

void
initialplayscreen_loadPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

		loadsavescreen_displayMyWindow(LSS_LOAD_GAME);
}
void
initialplayscreen_continuePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
}
void
initialplayscreen_instantPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	initialplayscreen_removeMyWindow(action);
	g_civApp->PostStartGameAction();
}
void
initialplayscreen_mapeditorPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
}

void
initialplayscreen_quitPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	initialplayscreen_removeMyWindow(action);

	ExitGame();
}
void
initialplayscreen_creditsPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	open_CreditsScreen();
}

// Code for new buttons taked from spscreen.cpp (and altered)
void initialplayscreen_newgamePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	if(initialplayscreen_removeMyWindow(action))
		spnewgamescreen_displayMyWindow();
}

void initialplayscreen_loadgamePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	loadsavescreen_displayMyWindow(LSS_LOAD_GAME);
}

void initialplayscreen_tutorialPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

#if defined(USE_SDL)
	bool legalGame = true;
#else // USE_SDL
	bool legalGame = c3files_HasLegalCD();
#endif // USE_SDL
	if (legalGame) {
		if(initialplayscreen_removeMyWindow(action)) {
			g_theProfileDB->SetTutorialAdvice(TRUE);
			g_civApp->PostStartGameAction();
		}
	}
}

void initialplayscreen_optionsPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	optionsscreen_displayMyWindow(0);
}
