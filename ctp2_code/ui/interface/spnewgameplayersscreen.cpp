//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling single player game start options. 
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
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
//
//----------------------------------------------------------------------------



#include "c3.h"
#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3ui.h"

#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühmann
#include "aui_Radio.h"
#include "aui_SwitchGroup.h"

#else
//Added by Martin Gühmann
#include "ctp2_spinner.h"
#include "ctp2_static.h"

#include "ColorSet.h"
extern ColorSet			*g_colorSet;

#endif

#include "aui_uniqueid.h"

#include "profileDB.h"

#include "spnewgamewindow.h"
#include "spnewgameplayersscreen.h"
#include "keypress.h"

extern C3UI			*g_c3ui;
extern ProfileDB	*g_theProfileDB;

#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühmann
#define k_NUM_PLAYERSBOXES	6
#endif

static c3_PopupWindow	*s_spNewGamePlayersScreen	= NULL;
static c3_Button	*s_back				= NULL;

static c3_Static	*s_name				= NULL;

#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühmann
static aui_SwitchGroup	*s_group		= NULL;
static aui_Radio	**s_checkBox;

static MBCHAR	checknames[k_NUM_PLAYERSBOXES][50] = {
	"PlayerOne",
	"PlayerTwo",
	"PlayerThree",
	"PlayerFour",
	"PlayerFive",
	"PlayerSix"
};

#else
//Added by Martin Gühmann
static ctp2_Spinner *s_num_player_spinner = NULL;
static ctp2_Spinner *s_max_player_spinner = NULL;
static ctp2_Spinner *s_player_spinner = NULL;

static c3_Static *s_num_player = NULL;
static c3_Static *s_max_player = NULL;
static c3_Static *s_player = NULL;

#endif

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

#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühmann
	if (!s_group) return 1;

	uint32 id = s_group->WhichIsSelected();

	if ( id ) {
		for ( sint32 i = 0;i < k_NUM_PLAYERSBOXES;i ++ ) {
			if ( id == s_checkBox[i]->Id() ) {

				g_theProfileDB->SetNPlayers( (i + 3) + 1 );	
			}
		}
	}
#else
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
#endif
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
// Description: Global Function
//
// Parameters : aui_Control::ControlActionCallback *callback
//
// Globals    : s_spNewGamePlayersScreen
//              s_num_player_spinner
//              s_max_player_spinner
//              s_player_spinner
//              s_maxPlayers
//
// Returns    : AUI_ERRCODE
//
// Remark(s)  : -Initializes the single player new game player screen
//
//----------------------------------------------------------------------------

AUI_ERRCODE spnewgameplayersscreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühmann
	MBCHAR		switchBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32 i;

	s_maxPlayers = 8;
#else
//Added by Martin Gühmann
	s_maxPlayers = k_MAX_PLAYERS;
#endif

	if ( s_spNewGamePlayersScreen ) {






#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühmann
		for (sint32 i = 0;i < k_NUM_PLAYERSBOXES;i++ ) {
			s_checkBox[i]->Enable(TRUE);
			s_checkBox[i]->SetState(0);
		}
#endif
		return AUI_ERRCODE_OK;		
	}

	strcpy(windowBlock, "SPNewGamePlayersScreen");

	{ 
		s_spNewGamePlayersScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(s_spNewGamePlayersScreen, errcode) );
		if ( !AUI_NEWOK(s_spNewGamePlayersScreen, errcode) ) errcode;

		
		s_spNewGamePlayersScreen->Resize(s_spNewGamePlayersScreen->Width(),s_spNewGamePlayersScreen->Height());
		s_spNewGamePlayersScreen->GrabRegion()->Resize(s_spNewGamePlayersScreen->Width(),s_spNewGamePlayersScreen->Height());
		s_spNewGamePlayersScreen->SetStronglyModal(TRUE);
	}
	

	
	sprintf( controlBlock, "%s.%s", windowBlock, "Name" );
	s_spNewGamePlayersScreen->AddTitle( controlBlock );

	
	if (!callback) callback = spnewgameplayersscreen_backPress;

	s_spNewGamePlayersScreen->AddClose( callback );

#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühmann
	sprintf( controlBlock, "%s.%s", windowBlock, "Group" );
	s_group = new aui_SwitchGroup( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_group, errcode) );

	if ( !AUI_NEWOK(s_group, errcode) ) return errcode;

	s_checkBox = new aui_Radio*[k_NUM_PLAYERSBOXES];

	for ( i = 0;i < k_NUM_PLAYERSBOXES;i++ ) {
		sprintf( switchBlock, "%s.%s", controlBlock, checknames[i] );
		s_checkBox[i] = new aui_Radio( &errcode, aui_UniqueId(), switchBlock );
		Assert( AUI_NEWOK(s_checkBox[i], errcode) );
		if ( !AUI_NEWOK(s_checkBox[i], errcode) ) return errcode;
		s_group->AddSwitch( (aui_Radio *)s_checkBox[i] );
	
	}

	
	
	if (g_theProfileDB->GetNPlayers() <= 3)
		g_theProfileDB->SetNPlayers(4);

	s_checkBox[(g_theProfileDB->GetNPlayers()-1)-3]->SetState( 1 );	

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

#else
//Added by Martin Gühmann
	sprintf( controlBlock, "%s.%s", windowBlock, "NumPlayerSpinner");
	s_num_player_spinner = new ctp2_Spinner(&errcode, aui_UniqueId(), controlBlock, spnewgameplayersscreen_NumPlayerSpinner, NULL);
	if(s_num_player_spinner){ 
		s_num_player_spinner->SetSpinnerCallback(spnewgameplayersscreen_NumPlayerSpinner, NULL);
		s_num_player_spinner->SetValue(g_theProfileDB->GetNPlayers() - 1, 0);
		if(s_num_player_spinner->GetMaximumX() >= k_MAX_PLAYERS){
			s_num_player_spinner->SetMaximum(k_MAX_PLAYERS-1, 0);
		}
	}

	sprintf( controlBlock, "%s.%s", windowBlock, "MaxPlayerSpinner");
	s_max_player_spinner = new ctp2_Spinner(&errcode, aui_UniqueId(), controlBlock, spnewgameplayersscreen_NumPlayerSpinner, NULL);
	if(s_max_player_spinner){
		s_max_player_spinner->SetSpinnerCallback(spnewgameplayersscreen_MaxPlayerSpinner, NULL);
		s_max_player_spinner->SetValue(g_theProfileDB->GetMaxPlayers() - 1, 0);
		if(s_max_player_spinner->GetMaximumX() >= k_MAX_PLAYERS){
			s_max_player_spinner->SetMaximum(k_MAX_PLAYERS-1, 0);
		}
	}

	sprintf( controlBlock, "%s.%s", windowBlock, "PlayerSpinner");
	s_player_spinner = new ctp2_Spinner(&errcode, aui_UniqueId(), controlBlock);
	if(s_player_spinner){
		s_player_spinner->SetSpinnerCallback(spnewgameplayersscreen_PlayerSpinner, NULL);
		s_player_spinner->SetValue(g_theProfileDB->GetPlayerIndex(), 0);
		s_player_spinner->SetMaximum(g_theProfileDB->GetNPlayers() - 1, 0);
	}
	sprintf( controlBlock, "%s.%s", windowBlock, "NumPlayerText");
	s_num_player = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	sprintf( controlBlock, "%s.%s", windowBlock, "MaxPlayerText");
	s_max_player = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	sprintf( controlBlock, "%s.%s", windowBlock, "PlayerText");
	s_player = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	if(s_player){
		s_player->SetTextColor(g_colorSet->GetColorRef(g_colorSet->ComputePlayerColor(g_theProfileDB->GetPlayerIndex())));
	}

	//Add the new spinners to the player window.
	s_spNewGamePlayersScreen->AddControl(s_num_player_spinner);
	s_spNewGamePlayersScreen->AddControl(s_max_player_spinner);
	s_spNewGamePlayersScreen->AddControl(s_player_spinner);
	s_spNewGamePlayersScreen->AddControl(s_num_player);
	s_spNewGamePlayersScreen->AddControl(s_max_player);
	s_spNewGamePlayersScreen->AddControl(s_player);
#endif

	return AUI_ERRCODE_OK;
}

//----------------------------------------------------------------------------
//
// Name       : spnewgameplayersscreen_Cleanuo
//
// Description: Global Function
//
// Parameters : void
//
// Globals    : s_spNewGamePlayersScreen
//              s_num_player_spinner
//              s_max_player_spinner
//              s_player_spinner
//
// Returns    : void
//
// Remark(s)  : -Removes the single player new game player screen
//
//----------------------------------------------------------------------------

AUI_ERRCODE spnewgameplayersscreen_Cleanup()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !s_spNewGamePlayersScreen  ) return AUI_ERRCODE_OK; 

#if !defined(ACTIVISION_ORIGINAL)
//Added by Martin Gühmann
	s_spNewGamePlayersScreen->RemoveControl(s_num_player_spinner->Id());
	s_spNewGamePlayersScreen->RemoveControl(s_max_player_spinner->Id());
	s_spNewGamePlayersScreen->RemoveControl(s_player_spinner->Id());
	s_spNewGamePlayersScreen->RemoveControl(s_num_player->Id());
	s_spNewGamePlayersScreen->RemoveControl(s_max_player->Id());
	s_spNewGamePlayersScreen->RemoveControl(s_player->Id());
#endif
	g_c3ui->RemoveWindow( s_spNewGamePlayersScreen->Id() );
	keypress_RemoveHandler(s_spNewGamePlayersScreen);

#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühmann
	for (sint32 i = 0;i < k_NUM_PLAYERSBOXES;i++ ) {
		mycleanup( s_checkBox[i] );
	}

	mycleanup( s_group );
#else
//Added by Martin Gühmann
	mycleanup(s_num_player_spinner);
	mycleanup(s_max_player_spinner);
	mycleanup(s_player_spinner);
	mycleanup(s_num_player);
	mycleanup(s_max_player);
	mycleanup(s_player);

#endif

	delete s_spNewGamePlayersScreen;
	s_spNewGamePlayersScreen = NULL;

	return AUI_ERRCODE_OK;

#undef mycleanup
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
#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühmann
	if (maxPlayers > 8) maxPlayers = 8;
#else
//Added by Martin Gühmann
	if (maxPlayers > k_MAX_PLAYERS) maxPlayers = k_MAX_PLAYERS;
#endif

	s_maxPlayers = maxPlayers;

#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühmann
	for (sint32 i = 0;i < k_NUM_PLAYERSBOXES;i++ ) {
		s_checkBox[i]->Enable(FALSE);
		s_checkBox[i]->SetState(0);
	}

	for (i = 3;i <= s_maxPlayers;i++ ) {
		s_checkBox[i-3]->Enable(TRUE);
	}

	s_checkBox[s_maxPlayers-3]->SetState(1);
#endif
}

#if !defined(ACTIVISION_ORIGINAL)
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
#endif