

#include "c3.h"

#include "c3ui.h"
#include "aui_uniqueid.h"
#include "aui_button.h"

#include "c3window.h"

#include "VictoryMovieWindow.h"
#include "victorymoviewin.h"
#include "GameOver.h"
#include "victorywin.h"
#include "victorywindow.h"
#include "infowin.h"
#include "EndGameWindow.h"

#include "MovieDB.h"
#include "StrDB.h"
#include "AgeRecord.h"
#include "WonderRecord.h"

#include "screenutils.h"
#include "EndGame.h"

#include "Director.h"
extern MovieDB			*g_theVictoryMovieDB;
extern C3UI				*g_c3ui;
extern VictoryWindow	*g_victoryWindow;

#include "SoundManager.h"
extern SoundManager		*g_soundManager;

#include "selitem.h"
extern SelectedItem		*g_selected_item;

#include "player.h"
extern Player			**g_player;

VictoryMovieWindow		*g_victoryMovieWindow = NULL;

static GAME_OVER		s_result;



void victorymoviewin_Initialize(Sequence *seq)
{
	AUI_ERRCODE		errcode;

	if (g_victoryMovieWindow == NULL) {
		g_victoryMovieWindow = new VictoryMovieWindow(&errcode, aui_UniqueId(), "VictoryMovieWindow", 16);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
			g_victoryMovieWindow = NULL;
		Assert(g_victoryMovieWindow != NULL);

	}

	g_victoryMovieWindow->SetSequence(seq);














}



void victorymoviewin_DisplayVictoryMovie(GAME_OVER reason)
{
	MBCHAR		*whichMovie;

	Assert(g_victoryMovieWindow != NULL);
	if (g_victoryMovieWindow == NULL) return;

	s_result = reason;

	switch (reason) {
	case GAME_OVER_LOST_OUT_OF_TIME :		whichMovie = "VICTORY_LOST_OUT_OF_TIME";
		break;
	case GAME_OVER_LOST_CONQUERED :			whichMovie = "VICTORY_LOST_CONQUERED";
		break;
	case GAME_OVER_LOST_DIPLOMACY :			whichMovie = "VICTORY_LOST_DIPLOMACY";
		break;
	case GAME_OVER_LOST_SCIENCE   :			whichMovie = "VICTORY_LOST_SCIENCE";
		break;
	case GAME_OVER_LOST_INEPT:				whichMovie = "VICTORY_LOST_INEPT";
		break;
	case GAME_OVER_LOST_SCENARIO:			whichMovie = "VICTORY_LOST_SCENARIO";
		break;
	case GAME_OVER_WON_SCENARIO:			whichMovie = "VICTORY_WON_SCENARIO";
		break;
	case GAME_OVER_WON_CONQUERED_WORLD :	whichMovie = "VICTORY_WON_CONQUERED_WORLD";
		break;
	case GAME_OVER_WON_DIPLOMACY :			whichMovie = "VICTORY_WON_DIPLOMACY";
		break;
	case GAME_OVER_WON_SCIENCE :			whichMovie = "VICTORY_WON_SCIENCE";
		break;

	
	case GAME_OVER_WON_OUT_OF_TIME:			whichMovie = "VICTORY_WON_CONQUERED_WORLD";
		break;
		
    default:
        {
            BOOL I_DONT_KNOW_WHAT_MOVIE_YOU_WANT_TO_SEE=0; 
            Assert(I_DONT_KNOW_WHAT_MOVIE_YOU_WANT_TO_SEE);
            whichMovie = "VICTORY_LOST_INEPT";
        }
	}

	if (g_soundManager) {
		g_soundManager->TerminateAllSounds();
		g_soundManager->TerminateMusic();
	}

	sint32 index = g_theVictoryMovieDB->FindTypeIndex(whichMovie);
	Assert(index >= 0);

	MBCHAR *filename = g_theVictoryMovieDB->GetMovieFilename(index);
	g_victoryMovieWindow->SetMovie(filename);

	AUI_ERRCODE		errcode;

	errcode = g_c3ui->AddWindow(g_victoryMovieWindow);
	Assert(errcode == AUI_ERRCODE_OK);

}



void victorymoviewin_Cleanup()
{
	Sequence	*seq = NULL;

	if (g_victoryMovieWindow) {
		seq = g_victoryMovieWindow->GetSequence();

		g_c3ui->RemoveWindow(g_victoryMovieWindow->Id());

		delete g_victoryMovieWindow;
		g_victoryMovieWindow = NULL;
	}

	
	g_director->ActionFinished(seq);
}



void victorymoviewin_MovieButtonCallback(aui_Control *control, uint32 action, uint32 data, void * cookie)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_c3ui->AddAction(new CloseVictoryMovieAction);
}



void CloseVictoryMovieAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	
	victorymoviewin_Cleanup();

	

	sint32 type;
	switch (s_result) {
	case GAME_OVER_LOST_OUT_OF_TIME :
	case GAME_OVER_LOST_CONQUERED :
	case GAME_OVER_LOST_SCIENCE :
	case GAME_OVER_LOST_DIPLOMACY :
	case GAME_OVER_LOST_INEPT:
	case GAME_OVER_LOST_SCENARIO:
		type = k_VICWIN_DEFEAT;
		break;
	case GAME_OVER_WON_OUT_OF_TIME:
	case GAME_OVER_WON_SCENARIO:
	case GAME_OVER_WON_CONQUERED_WORLD :		
	case GAME_OVER_WON_SCIENCE:
	case GAME_OVER_WON_DIPLOMACY:
		type = k_VICWIN_VICTORY;
		break;
	default:
		Assert(0);
	}

	
	
	infowin_Cleanup();	
						

	infowin_Initialize();

	victorywin_Initialize(type);

	victorywin_DisplayWindow(type);

	if (s_result == GAME_OVER_WON_SCIENCE) {
		EndGame *endGame = NULL;

		sint32 p = g_selected_item->GetVisiblePlayer();
		if (g_player[p] != NULL)
			endGame = g_player[p]->m_endGame;

		
		
	}




}

