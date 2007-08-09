//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer password screen
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
#include "c3window.h"
#include "dialogboxwindow.h"
#include "c3_button.h"
#include "c3_static.h"
#include "c3ui.h"
#include "aui_switchgroup.h"
#include "aui_uniqueid.h"
#include "textradio.h"
#include "c3textfield.h"

#include "netshell.h"
#include "gameselectwindow.h"
#include "lobbywindow.h"

#include "passwordscreen.h"

#include "spnewgamewindow.h"

extern C3UI			*g_c3ui;


static ns_Window *s_passwordScreen	= NULL;

static c3_Static	*s_askStatic			= NULL;
static c3_Static	*s_joinStatic			= NULL;
static aui_SwitchGroup *s_yesnoSwitchGroup	= NULL;
static aui_Radio	*s_yesRadio				= NULL;
static aui_Radio	*s_noRadio				= NULL;
static c3_Static	*s_inputStatic			= NULL;
static C3TextField	*s_inputTextField		= NULL;

static aui_Button	*s_okButton				= NULL;

static c3_Static	*s_denyStatic			= NULL;

static c3_Static	*s_fullStatic			= NULL;

static c3_Static	*s_nolobbyStatic		= NULL;

static c3_Static	*s_connectionlostStatic	= NULL;

static c3_Static	*s_nodialupStatic		= NULL;

static c3_Static	*s_connectionerrStatic	= NULL;

static c3_Static    *s_scenarionotfoundStatic = NULL;

sint32 passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE m )
{
	sint32 retval=0;
	if(!s_passwordScreen) { retval = passwordscreen_Initialize(); }

	AUI_ERRCODE auiErr;

	switch ( m )
	{
	case PASSWORDSCREEN_MODE_ASK:
		s_passwordScreen->AddChild( s_askStatic );
		s_passwordScreen->AddChild( s_yesnoSwitchGroup );

		
		s_yesRadio->SetState( 0 );

		
		s_noRadio->SetState( 1 );

		s_passwordScreen->RemoveChild( s_joinStatic->Id() );

		s_passwordScreen->AddChild( s_okButton );

		s_passwordScreen->RemoveChild( s_denyStatic->Id() );
		s_passwordScreen->RemoveChild( s_fullStatic->Id() );
		s_passwordScreen->RemoveChild( s_nolobbyStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionlostStatic->Id() );
		s_passwordScreen->RemoveChild( s_nodialupStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionerrStatic->Id() );
		s_passwordScreen->RemoveChild( s_scenarionotfoundStatic->Id() );
		break;

	case PASSWORDSCREEN_MODE_JOIN:
		s_passwordScreen->RemoveChild( s_askStatic->Id() );
		s_passwordScreen->RemoveChild( s_yesnoSwitchGroup->Id() );

		
		s_noRadio->SetState( 0 );

		
		s_yesRadio->SetState( 1 );

		
		s_inputTextField->SetFieldText( "" );

		s_passwordScreen->AddChild( s_joinStatic );

		s_passwordScreen->AddChild( s_okButton );

		s_passwordScreen->RemoveChild( s_denyStatic->Id() );
		s_passwordScreen->RemoveChild( s_fullStatic->Id() );
		s_passwordScreen->RemoveChild( s_nolobbyStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionlostStatic->Id() );
		s_passwordScreen->RemoveChild( s_nodialupStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionerrStatic->Id() );
		s_passwordScreen->RemoveChild( s_scenarionotfoundStatic->Id() );
		break;

	case PASSWORDSCREEN_MODE_DENY:
		s_passwordScreen->RemoveChild( s_askStatic->Id() );
		s_passwordScreen->RemoveChild( s_yesnoSwitchGroup->Id() );

		
		s_noRadio->SetState( 1 );

		s_passwordScreen->RemoveChild( s_joinStatic->Id() );

		s_passwordScreen->AddChild( s_okButton );

		s_passwordScreen->AddChild( s_denyStatic );
		s_passwordScreen->RemoveChild( s_fullStatic->Id() );
		s_passwordScreen->RemoveChild( s_nolobbyStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionlostStatic->Id() );
		s_passwordScreen->RemoveChild( s_nodialupStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionerrStatic->Id() );
		s_passwordScreen->RemoveChild( s_scenarionotfoundStatic->Id() );
		break;

	case PASSWORDSCREEN_MODE_FULL:
		s_passwordScreen->RemoveChild( s_askStatic->Id() );
		s_passwordScreen->RemoveChild( s_yesnoSwitchGroup->Id() );

		
		s_noRadio->SetState( 1 );

		s_passwordScreen->RemoveChild( s_joinStatic->Id() );

		s_passwordScreen->AddChild( s_okButton );

		s_passwordScreen->RemoveChild( s_denyStatic->Id() );
		s_passwordScreen->AddChild( s_fullStatic );
		s_passwordScreen->RemoveChild( s_nolobbyStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionlostStatic->Id() );
		s_passwordScreen->RemoveChild( s_nodialupStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionerrStatic->Id() );
		s_passwordScreen->RemoveChild( s_scenarionotfoundStatic->Id() );
		break;

	case PASSWORDSCREEN_MODE_NOLOBBY:
		s_passwordScreen->RemoveChild( s_askStatic->Id() );
		s_passwordScreen->RemoveChild( s_yesnoSwitchGroup->Id() );

		
		s_noRadio->SetState( 1 );

		s_passwordScreen->RemoveChild( s_joinStatic->Id() );

		s_passwordScreen->AddChild( s_okButton );

		s_passwordScreen->RemoveChild( s_denyStatic->Id() );
		s_passwordScreen->RemoveChild( s_fullStatic->Id() );
		s_passwordScreen->AddChild( s_nolobbyStatic );
		s_passwordScreen->RemoveChild( s_connectionlostStatic->Id() );
		s_passwordScreen->RemoveChild( s_nodialupStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionerrStatic->Id() );
		s_passwordScreen->RemoveChild( s_scenarionotfoundStatic->Id() );
		break;

	case PASSWORDSCREEN_MODE_CONNECTIONLOST:
		s_passwordScreen->RemoveChild( s_askStatic->Id() );
		s_passwordScreen->RemoveChild( s_yesnoSwitchGroup->Id() );

		
		s_noRadio->SetState( 1 );

		s_passwordScreen->RemoveChild( s_joinStatic->Id() );

		s_passwordScreen->AddChild( s_okButton );

		s_passwordScreen->RemoveChild( s_denyStatic->Id() );
		s_passwordScreen->RemoveChild( s_fullStatic->Id() );
		s_passwordScreen->RemoveChild( s_nolobbyStatic->Id() );
		s_passwordScreen->AddChild( s_connectionlostStatic );
		s_passwordScreen->RemoveChild( s_nodialupStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionerrStatic->Id() );
		s_passwordScreen->RemoveChild( s_scenarionotfoundStatic->Id() );
		break;

	case PASSWORDSCREEN_MODE_NODIALUP:
		s_passwordScreen->RemoveChild( s_askStatic->Id() );
		s_passwordScreen->RemoveChild( s_yesnoSwitchGroup->Id() );

		
		s_noRadio->SetState( 1 );

		s_passwordScreen->RemoveChild( s_joinStatic->Id() );

		s_passwordScreen->AddChild( s_okButton );

		s_passwordScreen->RemoveChild( s_denyStatic->Id() );
		s_passwordScreen->RemoveChild( s_fullStatic->Id() );
		s_passwordScreen->RemoveChild( s_nolobbyStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionlostStatic->Id() );
		s_passwordScreen->AddChild( s_nodialupStatic );
		s_passwordScreen->RemoveChild( s_connectionerrStatic->Id() );
		s_passwordScreen->RemoveChild( s_scenarionotfoundStatic->Id() );
		break;

	case PASSWORDSCREEN_MODE_CONNECTIONERR:
		s_passwordScreen->RemoveChild( s_askStatic->Id() );
		s_passwordScreen->RemoveChild( s_yesnoSwitchGroup->Id() );

		
		s_noRadio->SetState( 1 );

		s_passwordScreen->RemoveChild( s_joinStatic->Id() );

		s_passwordScreen->AddChild( s_okButton );

		s_passwordScreen->RemoveChild( s_denyStatic->Id() );
		s_passwordScreen->RemoveChild( s_fullStatic->Id() );
		s_passwordScreen->RemoveChild( s_nolobbyStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionlostStatic->Id() );
		s_passwordScreen->RemoveChild( s_nodialupStatic->Id() );
		s_passwordScreen->AddChild( s_connectionerrStatic );
		s_passwordScreen->RemoveChild( s_scenarionotfoundStatic->Id() );
		break;

	case PASSWORDSCREEN_MODE_SCENARIO_NOT_FOUND:
		s_passwordScreen->RemoveChild( s_askStatic->Id() );
		s_passwordScreen->RemoveChild( s_yesnoSwitchGroup->Id() );

		
		s_noRadio->SetState( 1 );

		s_passwordScreen->RemoveChild( s_joinStatic->Id() );

		s_passwordScreen->AddChild( s_okButton );

		s_passwordScreen->RemoveChild( s_denyStatic->Id() );
		s_passwordScreen->RemoveChild( s_fullStatic->Id() );
		s_passwordScreen->RemoveChild( s_nolobbyStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionlostStatic->Id() );
		s_passwordScreen->RemoveChild( s_nodialupStatic->Id() );
		s_passwordScreen->RemoveChild( s_connectionerrStatic->Id() );
		s_passwordScreen->AddChild( s_scenarionotfoundStatic );
		break;
	default:
		
		Assert( FALSE );
		break;
	}

	auiErr = g_c3ui->AddWindow( s_passwordScreen );
	Assert( auiErr == AUI_ERRCODE_OK );

	return retval;
}
sint32 passwordscreen_removeMyWindow( void )
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_passwordScreen->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	return 1;
}

AUI_ERRCODE passwordscreen_Initialize( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( s_passwordScreen ) return AUI_ERRCODE_OK;
	strcpy(windowBlock, "passwordscreen");

	s_passwordScreen = new ns_Window(
		&errcode,
		aui_UniqueId(),
		windowBlock,
		0,
		AUI_WINDOW_TYPE_FLOATING);
	Assert( AUI_NEWOK(s_passwordScreen, errcode) );
	if ( !AUI_NEWOK(s_passwordScreen, errcode) ) errcode;

	s_okButton = spNew_ctp2_Button(
				&errcode,
				windowBlock,
				"button0", 
				PasswordScreenCallback);
	Assert( AUI_NEWOK(s_okButton, errcode) );
	if ( !AUI_NEWOK(s_okButton, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "askstatic" );
	s_askStatic = new c3_Static(
		&errcode,
		aui_UniqueId(),
		controlBlock );
	Assert( AUI_NEWOK(s_askStatic, errcode) );
	if ( !AUI_NEWOK(s_askStatic, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "joinstatic" );
	s_joinStatic = new c3_Static(
		&errcode,
		aui_UniqueId(),
		controlBlock );
	Assert( AUI_NEWOK(s_joinStatic, errcode) );
	if ( !AUI_NEWOK(s_joinStatic, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "yesnoswitchgroup" );
	s_yesnoSwitchGroup = new aui_SwitchGroup(
		&errcode,
		aui_UniqueId(),
		controlBlock );
	Assert( AUI_NEWOK(s_yesnoSwitchGroup, errcode) );
	if ( !AUI_NEWOK(s_yesnoSwitchGroup, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "yesnoswitchgroup.yesradio" );
	s_yesRadio = new aui_Radio(
		&errcode,
		aui_UniqueId(),
		controlBlock,
		PasswordScreenCallback ); 

	Assert( AUI_NEWOK(s_yesRadio, errcode) );
	if ( !AUI_NEWOK(s_yesRadio, errcode) ) return errcode;

	s_yesnoSwitchGroup->AddSwitch( s_yesRadio );

	sprintf( controlBlock, "%s.%s", windowBlock, "yesnoswitchgroup.noradio" );
	s_noRadio = new aui_Radio(
		&errcode,
		aui_UniqueId(),
		controlBlock,
		PasswordScreenCallback );
	Assert( AUI_NEWOK(s_noRadio, errcode) );
	if ( !AUI_NEWOK(s_noRadio, errcode) ) return errcode;

	s_yesnoSwitchGroup->AddSwitch( s_noRadio );

	sprintf( controlBlock, "%s.%s", windowBlock, "inputstatic" );
	s_inputStatic = new c3_Static(
		&errcode,
		aui_UniqueId(),
		controlBlock );
	Assert( AUI_NEWOK(s_inputStatic, errcode) );
	if ( !AUI_NEWOK(s_inputStatic, errcode) ) return errcode;
	Assert( AUI_NEWOK(s_yesnoSwitchGroup, errcode) );
	if ( !AUI_NEWOK(s_yesnoSwitchGroup, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "inputtextfield" );
	s_inputTextField = new C3TextField(
		&errcode,
		aui_UniqueId(),
		controlBlock,
		PasswordScreenCallback );
	Assert( AUI_NEWOK(s_inputTextField, errcode) );
	if ( !AUI_NEWOK(s_inputTextField, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "denystatic" );
	s_denyStatic = new c3_Static(
		&errcode,
		aui_UniqueId(),
		controlBlock );
	Assert( AUI_NEWOK(s_denyStatic, errcode) );
	if ( !AUI_NEWOK(s_denyStatic, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "fullstatic" );
	s_fullStatic = new c3_Static(
		&errcode,
		aui_UniqueId(),
		controlBlock );
	Assert( AUI_NEWOK(s_fullStatic, errcode) );
	if ( !AUI_NEWOK(s_fullStatic, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "nolobbystatic" );
	s_nolobbyStatic = new c3_Static(
		&errcode,
		aui_UniqueId(),
		controlBlock );
	Assert( AUI_NEWOK(s_nolobbyStatic, errcode) );
	if ( !AUI_NEWOK(s_nolobbyStatic, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "connectionloststatic" );
	s_connectionlostStatic = new c3_Static(
		&errcode,
		aui_UniqueId(),
		controlBlock );
	Assert( AUI_NEWOK(s_connectionlostStatic, errcode) );
	if ( !AUI_NEWOK(s_connectionlostStatic, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "nodialupstatic" );
	s_nodialupStatic = new c3_Static(
		&errcode,
		aui_UniqueId(),
		controlBlock );
	Assert( AUI_NEWOK(s_nodialupStatic, errcode) );
	if ( !AUI_NEWOK(s_nodialupStatic, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "connectionerrstatic" );
	s_connectionerrStatic = new c3_Static(
		&errcode,
		aui_UniqueId(),
		controlBlock );
	Assert( AUI_NEWOK(s_connectionerrStatic, errcode) );
	if ( !AUI_NEWOK(s_connectionerrStatic, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "noscenariostatic" );
	s_scenarionotfoundStatic = new c3_Static(
		&errcode,
		aui_UniqueId(),
		controlBlock );
	Assert( AUI_NEWOK(s_scenarionotfoundStatic, errcode) );
	if ( !AUI_NEWOK(s_scenarionotfoundStatic, errcode) ) return errcode;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE passwordscreen_Cleanup()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !s_passwordScreen  ) return AUI_ERRCODE_OK;
	g_c3ui->RemoveWindow( s_passwordScreen->Id() );

	mycleanup( s_askStatic );
	mycleanup( s_joinStatic );
	mycleanup( s_yesnoSwitchGroup );
	mycleanup( s_yesRadio );
	mycleanup( s_noRadio );
	mycleanup( s_inputStatic );
	mycleanup( s_inputTextField );
	mycleanup( s_okButton );
	mycleanup( s_denyStatic );
	mycleanup( s_fullStatic );
	mycleanup( s_nolobbyStatic );
	mycleanup( s_connectionlostStatic );
	mycleanup( s_nodialupStatic );
	mycleanup( s_connectionerrStatic );
	mycleanup( s_scenarionotfoundStatic );

	delete s_passwordScreen;
	s_passwordScreen = NULL;

	return AUI_ERRCODE_OK;

#undef mycleanup
}


void PasswordScreenCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void *cookie )
{
	if ( control == s_yesRadio )
	{
		if ( action != (uint32)AUI_SWITCH_ACTION_ON ) return;

		s_passwordScreen->AddChild( s_inputStatic );
		s_passwordScreen->AddChild( s_inputTextField );

		
		s_inputTextField->SetKeyboardFocus();
	}
	else if ( control == s_noRadio )
	{
		if ( action != (uint32)AUI_SWITCH_ACTION_ON ) return;

		s_passwordScreen->RemoveChild( s_inputStatic->Id() );
		s_passwordScreen->RemoveChild( s_inputTextField->Id() );

		
		s_inputTextField->SetFieldText( "" );
	}
	else if ( control == s_inputTextField )
	{
		if ( action != (uint32)AUI_TEXTFIELD_ACTION_EXECUTE ) return;

		
		PasswordScreenCallback(
			s_okButton,
			AUI_BUTTON_ACTION_EXECUTE,
			0,
			NULL );
	}
	else if ( control == s_okButton )
	{
		if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

		passwordscreen_removeMyWindow();

		
		if ( !s_passwordScreen->GetChild( s_denyStatic->Id() ) &&
			 !s_passwordScreen->GetChild( s_fullStatic->Id() ) &&
			 !s_passwordScreen->GetChild( s_nolobbyStatic->Id() ) &&
			 !s_passwordScreen->GetChild( s_scenarionotfoundStatic->Id() ) &&
			 !s_passwordScreen->GetChild( s_connectionlostStatic->Id() ) )
		{
			
			MBCHAR password[ dp_PASSWORDLEN + 1 ];
			memset( password, 0, sizeof( password ) );

			
			
			if ( s_passwordScreen->GetChild( s_inputTextField->Id() ) )
				s_inputTextField->GetFieldText( password, dp_PASSWORDLEN );

			
			if ( s_passwordScreen->GetChild( s_yesnoSwitchGroup->Id() ) )
			{
				g_gameSelectWindow->PasswordScreenDone( password );
			}
			else 
			{
				g_lobbyWindow->PasswordScreenDone( password );
			}
		}
		else if(s_passwordScreen->GetChild(s_nolobbyStatic->Id()))
		{
			g_netshell->GotoScreen(NetShell::SCREEN_PLAYERSELECT);
		}
		else
		{
			
		}
	}
}
