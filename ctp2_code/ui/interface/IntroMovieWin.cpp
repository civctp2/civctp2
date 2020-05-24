//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The intro movie window
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "c3ui.h"
#include "aui_uniqueid.h"
#include "aui_button.h"

#include "c3window.h"

#include "IntroMovieWindow.h"
#include "IntroMovieWin.h"

#include "moviedb.h"
#include "StrDB.h"
#include "AgeRecord.h"
#include "WonderRecord.h"

#include "initialplaywindow.h"
#include "soundmanager.h"

#include "wonderutil.h"

extern MovieDB			*g_theVictoryMovieDB;
extern C3UI				*g_c3ui;
extern SoundManager		*g_soundManager;

IntroMovieWindow		*g_introMovieWindow = NULL;


void intromoviewin_Initialize(void)
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	if (g_introMovieWindow == NULL) {
		g_introMovieWindow = new IntroMovieWindow(&errcode, aui_UniqueId(), "IntroMovieWindow", 16);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
			g_introMovieWindow = NULL;
		Assert(g_introMovieWindow != NULL);
	}
}


void intromoviewin_DisplayIntroMovie(void)
{
	MBCHAR		*whichMovie;

	Assert(g_introMovieWindow != NULL);
	if (g_introMovieWindow == NULL) return;

	whichMovie = "VICTORY_INTRO";

	sint32 index = g_theVictoryMovieDB->FindTypeIndex(whichMovie);
	Assert(index >= 0);

	MBCHAR *filename = g_theVictoryMovieDB->GetMovieFilename(index);
	g_introMovieWindow->SetMovie(filename);

	AUI_ERRCODE		errcode;

	errcode = g_c3ui->AddWindow(g_introMovieWindow);
	Assert(errcode == AUI_ERRCODE_OK);

}


void intromoviewin_Cleanup()
{
	if (g_introMovieWindow) {

		g_c3ui->RemoveWindow(g_introMovieWindow->Id());

		delete g_introMovieWindow;
		g_introMovieWindow = NULL;
	}
}


void intromoviewin_MovieButtonCallback(aui_Control *control, uint32 action, uint32 data, void * cookie)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_c3ui->AddAction(new CloseIntroMovieAction);
}


void CloseIntroMovieAction::Execute(aui_Control *control, uint32 action, uint32 data)
{

	intromoviewin_Cleanup();

	AUI_ERRCODE errcode;

	if (g_soundManager) {
		g_soundManager->EnableMusic();
		g_soundManager->PickNextTrack();
		g_soundManager->StartMusic();
	}

	errcode = initialplayscreen_Initialize();
	Assert(errcode == AUI_ERRCODE_OK);

	initialplayscreen_displayMyWindow();

}
