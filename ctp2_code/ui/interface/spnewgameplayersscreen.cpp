//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling single player game start options.
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
// - Redisign of the single player new game players screen by Martin Gühmann.
//   Instead of buttons to determine the number of players in a new game
//   this screen now allows the player to specify the player index he wants
//   to use determines the player color, he can also specify how many civs
//   should be in the game at the start and how many civs in the game should
//   be maximal in the game. The maximum number of players in one game is
//   currently 32 and is hard encoded somewhere else.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "spnewgameplayersscreen.h"

#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3ui.h"

//Added by Martin Gühmann
#include "ctp2_spinner.h"
#include "ctp2_Static.h"

#include "colorset.h"
extern ColorSet			*g_colorSet;

#include "aui_uniqueid.h"

#include "profileDB.h"

#include "spnewgamewindow.h"
#include "keypress.h"

extern C3UI			*g_c3ui;
extern ProfileDB	*g_theProfileDB;

namespace
{

c3_PopupWindow *    s_spNewGamePlayersScreen = NULL;

//----------------------------------------------------------------------------
//
// Name       : CleanupControl
//
// Description: Release data of a control
//
// Parameters : a_Control   : the control to release
//
// Globals    : s_spNewGamePlayersScreen
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
template <typename T>
void CleanupControl(T * & a_Control)
{
    if (a_Control)
    {
        s_spNewGamePlayersScreen->RemoveControl(a_Control->Id());
        delete a_Control;
        a_Control = NULL;
    }
}

} // namespace

//Added by Martin Gühmann
static ctp2_Spinner *s_num_player_spinner = NULL;
static ctp2_Spinner *s_max_player_spinner = NULL;
static ctp2_Spinner *s_player_spinner = NULL;

static c3_Static *s_num_player = NULL;
static c3_Static *s_max_player = NULL;
static c3_Static *s_player = NULL;

static sint32		s_maxPlayers=0;

//----------------------------------------------------------------------------
//
// Name       : spnewgameplayersscreen_displayMyWindow
//
// Description: Global Function
//
// Parameters : void
//
// Globals    : s_spNewGamePlayersScreen
//
// Returns    : sint32
//
// Remark(s)  : -Displays and initialzes (if necessary) the single
//               player new game player screen
//
//----------------------------------------------------------------------------
sint32	spnewgameplayersscreen_displayMyWindow()
{
	sint32 retval=0;
	if(!s_spNewGamePlayersScreen) { retval = spnewgameplayersscreen_Initialize(); }

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->AddWindow(s_spNewGamePlayersScreen);
	keypress_RegisterHandler(s_spNewGamePlayersScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	return retval;
}

//----------------------------------------------------------------------------
//
// Name       : spnewgameplayersscreen_removeMyWindow
//
// Description: Global Function
//
// Parameters : uint32 action
//
// Globals    : s_spNewGamePlayersScreen
//              s_num_player_spinner
//              s_max_player_spinner
//              s_player_spinner
//
// Returns    : sint32
//
// Remark(s)  : -Hides the single player new game player screen
//
//----------------------------------------------------------------------------

sint32 spnewgameplayersscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	if (!s_spNewGamePlayersScreen) return 1;

//Added by Martin Gühmann
	if(s_num_player_spinner){
		g_theProfileDB->SetNPlayers(s_num_player_spinner->GetValueX()+1);
	}
	if(s_max_player_spinner){
		g_theProfileDB->SetMaxPlayers(s_max_player_spinner->GetValueX()+1);
	}
	if(s_player_spinner){
		g_theProfileDB->SetPlayerIndex(s_player_spinner->GetValueX());
	}
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_spNewGamePlayersScreen->Id() );
	keypress_RemoveHandler(s_spNewGamePlayersScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	spnewgamescreen_update();

	return 1;
}

//----------------------------------------------------------------------------
//
// Name       : spnewgameplayersscreen_Initialize
//
// Description: Initializes the single player new game player screen
//
// Parameters : aui_Control::ControlActionCallback *callback
//
// Globals    : s_spNewGamePlayersScreen
//              s_num_player_spinner
//              s_max_player_spinner
//              s_player_spinner
//              s_maxPlayers
//              g_ui
//
// Returns    : AUI_ERRCODE
//
// Remark(s)  : Assumption: g_ui and g_ui->TheLdl() are non-NULL.
//
//----------------------------------------------------------------------------
AUI_ERRCODE spnewgameplayersscreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
//Added by Martin Gühmann
	s_maxPlayers = k_MAX_PLAYERS;

	if ( s_spNewGamePlayersScreen ) {






		return AUI_ERRCODE_OK;
	}

	strcpy(windowBlock, "SPNewGamePlayersScreen");

	{
		s_spNewGamePlayersScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(s_spNewGamePlayersScreen, errcode) );

		s_spNewGamePlayersScreen->Resize(s_spNewGamePlayersScreen->Width(),s_spNewGamePlayersScreen->Height());
		s_spNewGamePlayersScreen->GrabRegion()->Resize(s_spNewGamePlayersScreen->Width(),s_spNewGamePlayersScreen->Height());
		s_spNewGamePlayersScreen->SetStronglyModal(TRUE);
	}


	sprintf( controlBlock, "%s.%s", windowBlock, "Name" );
	s_spNewGamePlayersScreen->AddTitle( controlBlock );

	if (!callback) callback = spnewgameplayersscreen_backPress;

	s_spNewGamePlayersScreen->AddClose( callback );

    // Added by Martin Gühmann, may not exist for mods
	sprintf( controlBlock, "%s.%s", windowBlock, "NumPlayerSpinner");
    if (g_ui->GetLdl()->IsValid(controlBlock))
    {
	    s_num_player_spinner = new ctp2_Spinner(&errcode, aui_UniqueId(), controlBlock, spnewgameplayersscreen_NumPlayerSpinner, NULL);
		s_num_player_spinner->SetSpinnerCallback(spnewgameplayersscreen_NumPlayerSpinner, NULL);
		s_num_player_spinner->SetValue(g_theProfileDB->GetNPlayers() - 1, 0);
		if(s_num_player_spinner->GetMaximumX() >= k_MAX_PLAYERS){
			s_num_player_spinner->SetMaximum(k_MAX_PLAYERS-1, 0);
		}
	    s_spNewGamePlayersScreen->AddControl(s_num_player_spinner);

	    sprintf( controlBlock, "%s.%s", windowBlock, "NumPlayerText");
	    s_num_player = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	    s_spNewGamePlayersScreen->AddControl(s_num_player);
	}

	sprintf( controlBlock, "%s.%s", windowBlock, "MaxPlayerSpinner");
    if (g_ui->GetLdl()->IsValid(controlBlock))
    {
        s_max_player_spinner = new ctp2_Spinner(&errcode, aui_UniqueId(), controlBlock, spnewgameplayersscreen_NumPlayerSpinner, NULL);
		s_max_player_spinner->SetSpinnerCallback(spnewgameplayersscreen_MaxPlayerSpinner, NULL);
		s_max_player_spinner->SetValue(g_theProfileDB->GetMaxPlayers() - 1, 0);
		if(s_max_player_spinner->GetMaximumX() >= k_MAX_PLAYERS){
			s_max_player_spinner->SetMaximum(k_MAX_PLAYERS-1, 0);
		}
        s_spNewGamePlayersScreen->AddControl(s_max_player_spinner);

	    sprintf( controlBlock, "%s.%s", windowBlock, "MaxPlayerText");
	    s_max_player = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	    s_spNewGamePlayersScreen->AddControl(s_max_player);
	}

	sprintf( controlBlock, "%s.%s", windowBlock, "PlayerSpinner");
    if (g_ui->GetLdl()->IsValid(controlBlock))
    {
	    s_player_spinner = new ctp2_Spinner(&errcode, aui_UniqueId(), controlBlock);
		s_player_spinner->SetSpinnerCallback(spnewgameplayersscreen_PlayerSpinner, NULL);
		s_player_spinner->SetValue(g_theProfileDB->GetPlayerIndex(), 0);
		s_player_spinner->SetMaximum(g_theProfileDB->GetNPlayers() - 1, 0);
	    s_spNewGamePlayersScreen->AddControl(s_player_spinner);

	    sprintf( controlBlock, "%s.%s", windowBlock, "PlayerText");
        s_player = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
		s_player->SetTextColor(g_colorSet->GetColorRef(g_colorSet->ComputePlayerColor(g_theProfileDB->GetPlayerIndex())));
	    s_spNewGamePlayersScreen->AddControl(s_player);
	}

	return AUI_ERRCODE_OK;
}

//----------------------------------------------------------------------------
//
// Name       : spnewgameplayersscreen_Cleanup
//
// Description: Release the data of the single player new game player screen.
//
// Parameters : -
//
// Globals    : s_spNewGamePlayersScreen
//              s_num_player_spinner
//              s_max_player_spinner
//              s_player_spinner
//              s_num_player
//              s_max_player
//              s_player
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void spnewgameplayersscreen_Cleanup()
{
	if (s_spNewGamePlayersScreen)
    {
        CleanupControl(s_num_player_spinner);
        CleanupControl(s_max_player_spinner);
        CleanupControl(s_player_spinner);
        CleanupControl(s_num_player);
        CleanupControl(s_max_player);
        CleanupControl(s_player);

        if (g_c3ui)
        {
            g_c3ui->RemoveWindow(s_spNewGamePlayersScreen->Id());
        }
        keypress_RemoveHandler(s_spNewGamePlayersScreen);

        delete s_spNewGamePlayersScreen;
        s_spNewGamePlayersScreen = NULL;
    }
}

//----------------------------------------------------------------------------
//
// Name       : spnewgameplayersscreen_backPress
//
// Description: Global Function
//
// Parameters : aui_Control *control
//              uint32 action
//              uint32 data
//              void *cookie
//
// Globals    : -
//
// Returns    : void
//
// Remark(s)  : -Hides the single player new game player screen when the
//               back button is pressed
//
//----------------------------------------------------------------------------

void spnewgameplayersscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{


	spnewgameplayersscreen_removeMyWindow(action);

}

//----------------------------------------------------------------------------
//
// Name       : spnewgameplayersscreen_SetMaxPlayers
//
// Description: Global Function
//
// Parameters : sint32 maxPlayers
//
// Globals    : s_maxPlayers
//
// Returns    : void
//
// Remark(s)  : Sets the max number of players if it is over the
//              limit.
//
//----------------------------------------------------------------------------

void spnewgameplayersscreen_SetMaxPlayers(sint32 maxPlayers)
{
//Added by Martin Gühmann
	if (maxPlayers > k_MAX_PLAYERS) maxPlayers = k_MAX_PLAYERS;

	s_maxPlayers = maxPlayers;

}

//Added by Martin Gühmann

//----------------------------------------------------------------------------
//
// Name       : spnewgameplayersscreen_NumPlayerSpinner
//
// Description: Global Function
//
// Parameters : aui_Control *control
//              uint32 action
//              uint32 data
//              void *cookie
//
// Globals    : s_num_player_spinner
//
// Returns    : void
//
// Remark(s)  : -Sets the maximum of the s_player_spinner on the value of the
//               s_num_player_spinner
//
//----------------------------------------------------------------------------

void spnewgameplayersscreen_NumPlayerSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_RANGER_ACTION_VALUECHANGE) return;

	if(!s_num_player_spinner) return;
	sint32 value = s_num_player_spinner->GetValueX();

	if(s_player_spinner) s_player_spinner->SetMaximum(value, 0);
}

//----------------------------------------------------------------------------
//
// Name       : spnewgameplayersscreen_MaxPlayerSpinner
//
// Description: Global Function
//
// Parameters : aui_Control *control
//              uint32 action
//              uint32 data
//              void *cookie
//
// Globals    : s_max_player_spinner
//
// Returns    : void
//
// Remark(s)  : -Handels changes on the UI when the value of the
//               s_max_player_spinner changes
//
//----------------------------------------------------------------------------

void spnewgameplayersscreen_MaxPlayerSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_RANGER_ACTION_VALUECHANGE) return;

	if(!s_max_player_spinner) return;
}

//----------------------------------------------------------------------------
//
// Name       : spnewgameplayersscreen_PlayerSpinner
//
// Description: Global Function
//
// Parameters : aui_Control *control
//              uint32 action
//              uint32 data
//              void *cookie
//
// Globals    : s_player_spinner
//
// Returns    : void
//
// Remark(s)  : -Handels changes on the UI when the value of the
//               s_player_spinner changes
//
//----------------------------------------------------------------------------

void spnewgameplayersscreen_PlayerSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_RANGER_ACTION_VALUECHANGE) return;

	if(!s_player_spinner) return;
	if(!s_player) return;
	s_player->SetTextColor(g_colorSet->GetColorRef(g_colorSet->ComputePlayerColor(s_player_spinner->GetValueX())));
	s_player->ShouldDraw();
}
