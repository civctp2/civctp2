

#include "c3.h"
#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3ui.h"
#include "aui_Radio.h"
#include "aui_SwitchGroup.h"
#include "aui_uniqueid.h"

#include "profileDB.h"

#include "spnewgamewindow.h"
#include "spnewgameplayersscreen.h"
#include "keypress.h"

extern C3UI			*g_c3ui;
extern ProfileDB	*g_theProfileDB;

#define k_NUM_PLAYERSBOXES	6

static c3_PopupWindow	*s_spNewGamePlayersScreen	= NULL;
static c3_Button	*s_back				= NULL;

static c3_Static	*s_name				= NULL;

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

static sint32		s_maxPlayers=0;




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
sint32 spnewgameplayersscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	if (!s_spNewGamePlayersScreen) return 1;
	if (!s_group) return 1;

	uint32 id = s_group->WhichIsSelected();

	if ( id ) {
		for ( sint32 i = 0;i < k_NUM_PLAYERSBOXES;i ++ ) {
			if ( id == s_checkBox[i]->Id() ) {

				g_theProfileDB->SetNPlayers( (i + 3) + 1 );	
			}
		}
	}
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_spNewGamePlayersScreen->Id() );
	keypress_RemoveHandler(s_spNewGamePlayersScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	spnewgamescreen_update();

	return 1;
}



AUI_ERRCODE spnewgameplayersscreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		switchBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32 i;

	s_maxPlayers = 8;

	if ( s_spNewGamePlayersScreen ) {








		for (sint32 i = 0;i < k_NUM_PLAYERSBOXES;i++ ) {
			s_checkBox[i]->Enable(TRUE);
			s_checkBox[i]->SetState(0);
		}

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

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE spnewgameplayersscreen_Cleanup()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !s_spNewGamePlayersScreen  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( s_spNewGamePlayersScreen->Id() );
	keypress_RemoveHandler(s_spNewGamePlayersScreen);

	for (sint32 i = 0;i < k_NUM_PLAYERSBOXES;i++ ) {
		mycleanup( s_checkBox[i] );
	}

	mycleanup( s_group );




	delete s_spNewGamePlayersScreen;
	s_spNewGamePlayersScreen = NULL;

	return AUI_ERRCODE_OK;

#undef mycleanup
}




void spnewgameplayersscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	

	spnewgameplayersscreen_removeMyWindow(action);

}



void spnewgameplayersscreen_SetMaxPlayers(sint32 maxPlayers)
{
	if (maxPlayers > 8) maxPlayers = 8;

	s_maxPlayers = maxPlayers;

	for (sint32 i = 0;i < k_NUM_PLAYERSBOXES;i++ ) {
		s_checkBox[i]->Enable(FALSE);
		s_checkBox[i]->SetState(0);
	}

	for (i = 3;i <= s_maxPlayers;i++ ) {
		s_checkBox[i-3]->Enable(TRUE);
	}

	s_checkBox[s_maxPlayers-3]->SetState(1);
}
