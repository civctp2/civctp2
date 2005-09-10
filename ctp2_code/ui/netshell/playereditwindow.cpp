//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer player edit window
// Id           : $Id:$
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

#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_static.h"
#include "aui_screen.h"
#include "aui_switchgroup.h"
#include "aui_radio.h"
#include "aui_stringtable.h"

#include "c3_button.h"
#include "textradio.h"
#include "c3textfield.h"

#include "netshell.h"


#include "playereditwindow.h"
#include "playerselectwindow.h"
#include "ns_customlistbox.h"


PlayerEditWindow::PlayerEditWindow(
	AUI_ERRCODE *retval )
	:
	ns_Window(
		retval,
		aui_UniqueId(),
		"playereditwindow",
		0,
		AUI_WINDOW_TYPE_STANDARD )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateControls();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE PlayerEditWindow::InitCommon( void )
{
	m_controls = new aui_Control *[ m_numControls = CONTROL_MAX ];
	Assert( m_controls != NULL );
	if ( !m_controls ) return AUI_ERRCODE_MEMALLOCFAILED;
	memset( m_controls, 0, m_numControls * sizeof( aui_Control *) );

	SetPlayerSetup( NULL );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE PlayerEditWindow::CreateControls( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;


	
	aui_Control *control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.titlestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_TITLESTATICTEXT ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.playernamestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYERNAMESTATICTEXT ] = control;

	control = new C3TextField(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.playernametextfield" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYERNAMETEXTFIELD ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.icqstatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_ICQSTATICTEXT ] = control;

	control = new C3TextField(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.icqtextfield" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_ICQTEXTFIELD ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.emailstatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_EMAILSTATICTEXT ] = control;

	control = new C3TextField(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.emailtextfield" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_EMAILTEXTFIELD ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.locationstatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_LOCATIONSTATICTEXT ] = control;

	control = new C3TextField(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.locationtextfield" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_LOCATIONTEXTFIELD ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.experiencestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_EXPERIENCESTATICTEXT ] = control;

	control = new aui_SwitchGroup(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.experienceswitchgroup" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_EXPERIENCESWITCHGROUP ] = control;

	{
		control = new aui_Radio(
			&errcode,
			aui_UniqueId(),
			"playereditwindow.experienceswitchgroup.experience0checkbox" );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
		m_controls[ CONTROL_EXPERIENCE0CHECKBOX ] = control;

		control = new aui_Radio(
			&errcode,
			aui_UniqueId(),
			"playereditwindow.experienceswitchgroup.experience1checkbox" );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
		m_controls[ CONTROL_EXPERIENCE1CHECKBOX ] = control;

		control = new aui_Radio(
			&errcode,
			aui_UniqueId(),
			"playereditwindow.experienceswitchgroup.experience2checkbox" );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
		m_controls[ CONTROL_EXPERIENCE2CHECKBOX ] = control;

		control = new aui_Radio(
			&errcode,
			aui_UniqueId(),
			"playereditwindow.experienceswitchgroup.experience3checkbox" );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
		m_controls[ CONTROL_EXPERIENCE3CHECKBOX ] = control;

		control = new aui_Radio(
			&errcode,
			aui_UniqueId(),
			"playereditwindow.experienceswitchgroup.experience4checkbox" );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
		m_controls[ CONTROL_EXPERIENCE4CHECKBOX ] = control;

		control = new aui_Radio(
			&errcode,
			aui_UniqueId(),
			"playereditwindow.experienceswitchgroup.experience5checkbox" );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
		m_controls[ CONTROL_EXPERIENCE5CHECKBOX ] = control;
	}

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.playerinfostatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYERINFOSTATICTEXT ] = control;

	control = new C3TextField(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.playerinfotextfield" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYERINFOTEXTFIELD ] = control;


	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.okbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_OKBUTTON ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"playereditwindow.cancelbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CANCELBUTTON ] = control;


	
	aui_Ldl::SetupHeirarchyFromRoot( "playereditwindow" );


	
	aui_Action *action;

	action = new OKButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_OKBUTTON ]->SetAction( action );

	action = new CancelButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_CANCELBUTTON ]->SetAction( action );


	
	SetStronglyModal( TRUE );


	return AUI_ERRCODE_OK;
}

PlayerEditWindow::~PlayerEditWindow()
{
}


void PlayerEditWindow::SetPlayerSetup(nf_PlayerSetup *p)
{
	m_playersetup = p;
}

void PlayerEditWindow::SetMode(Mode m)
{
	mode = m;
	switch(mode) {
	case EDIT_GAMESETUP:

		m_controls[ CONTROL_PLAYERNAMETEXTFIELD ]->Enable( true );
		m_controls[ CONTROL_ICQTEXTFIELD ]->Enable( true );
		m_controls[ CONTROL_EMAILTEXTFIELD ]->Enable( true );
		m_controls[ CONTROL_LOCATIONTEXTFIELD ]->Enable( true );
		m_controls[ CONTROL_EXPERIENCESWITCHGROUP ]->Enable( true );
		m_controls[ CONTROL_PLAYERINFOTEXTFIELD ]->Enable( true );
		m_controls[ CONTROL_CANCELBUTTON ]->Show();
		break;

	case EDIT:

		m_controls[ CONTROL_PLAYERNAMETEXTFIELD ]->Enable( true );
		m_controls[ CONTROL_ICQTEXTFIELD ]->Enable( true );
		m_controls[ CONTROL_EMAILTEXTFIELD ]->Enable( true );
		m_controls[ CONTROL_LOCATIONTEXTFIELD ]->Enable( true );
		m_controls[ CONTROL_EXPERIENCESWITCHGROUP ]->Enable( true );
		m_controls[ CONTROL_PLAYERINFOTEXTFIELD ]->Enable( true );
		m_controls[ CONTROL_CANCELBUTTON ]->Show();
		break;

	case VIEW:

		m_controls[ CONTROL_PLAYERNAMETEXTFIELD ]->Enable( false );
		m_controls[ CONTROL_ICQTEXTFIELD ]->Enable( false );
		m_controls[ CONTROL_EMAILTEXTFIELD ]->Enable( false );
		m_controls[ CONTROL_LOCATIONTEXTFIELD ]->Enable( false );
		m_controls[ CONTROL_EXPERIENCESWITCHGROUP ]->Enable( false );
		m_controls[ CONTROL_PLAYERINFOTEXTFIELD ]->Enable( false );
		m_controls[ CONTROL_CANCELBUTTON ]->Hide();
		break;
	}
}

PlayerEditWindow::Mode PlayerEditWindow::GetMode()
{
	return mode;
}

AUI_ERRCODE PlayerEditWindow::Idle( void )
{
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE PlayerEditWindow::SetParent( aui_Region *region )
{
		if ( region ) {
		((aui_Control *)FindControl( CONTROL_PLAYERNAMETEXTFIELD ))->
			SetKeyboardFocus();

		if(m_playersetup)
		{
			((aui_TextField *)(FindControl(
				PlayerEditWindow::CONTROL_PLAYERNAMETEXTFIELD)))->
					SetFieldText(m_playersetup->GetName());

			((aui_TextField *)(FindControl(
				PlayerEditWindow::CONTROL_ICQTEXTFIELD)))->
					SetFieldText(m_playersetup->GetIcq());
			((aui_TextField *)(FindControl(
				PlayerEditWindow::CONTROL_EMAILTEXTFIELD)))->
					SetFieldText(m_playersetup->GetEmail());
			((aui_TextField *)(FindControl(
				PlayerEditWindow::CONTROL_LOCATIONTEXTFIELD)))->
					SetFieldText(m_playersetup->GetLocation());

			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE0CHECKBOX ])->
				SetState( FALSE );
			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE1CHECKBOX ])->
				SetState( FALSE );
			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE2CHECKBOX ])->
				SetState( FALSE );
			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE3CHECKBOX ])->
				SetState( FALSE );
			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE4CHECKBOX ])->
				SetState( FALSE );
			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE5CHECKBOX ])->
				SetState( FALSE );
			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE0CHECKBOX +
									m_playersetup->GetExperience() ])->
				SetState( TRUE );

			((aui_TextField *)(FindControl(
				PlayerEditWindow::CONTROL_PLAYERINFOTEXTFIELD)))->
					SetFieldText(m_playersetup->GetDescription());


		}
		else
		{
			((aui_TextField *)(FindControl(
				PlayerEditWindow::CONTROL_PLAYERNAMETEXTFIELD)))->
					SetFieldText("");

			((aui_TextField *)(FindControl(
				PlayerEditWindow::CONTROL_ICQTEXTFIELD)))->
					SetFieldText("");
			((aui_TextField *)(FindControl(
				PlayerEditWindow::CONTROL_EMAILTEXTFIELD)))->
					SetFieldText("");
			((aui_TextField *)(FindControl(
				PlayerEditWindow::CONTROL_LOCATIONTEXTFIELD)))->
					SetFieldText("");

			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE0CHECKBOX ])->
				SetState( TRUE );
			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE1CHECKBOX ])->
				SetState( FALSE );
			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE2CHECKBOX ])->
				SetState( FALSE );
			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE3CHECKBOX ])->
				SetState( FALSE );
			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE4CHECKBOX ])->
				SetState( FALSE );
			((aui_Radio *)m_controls[ CONTROL_EXPERIENCE5CHECKBOX ])->
				SetState( FALSE );

			((aui_TextField *)(FindControl(
				PlayerEditWindow::CONTROL_PLAYERINFOTEXTFIELD)))->
					SetFieldText("");


		}
	}
	return ns_Window::SetParent( region );
}


void PlayerEditWindow::OKButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	PlayerSelectWindow *w = (PlayerSelectWindow *)g_netshell->
		FindWindow( NetShell::WINDOW_PLAYERSELECT );
	PlayerEditWindow *p = (PlayerEditWindow *)(control->GetParentWindow());
	ns_PlayerSetupListBox *l = (ns_PlayerSetupListBox *)w->
		FindControl( PlayerSelectWindow::CONTROL_PLAYERNAMELISTBOX );

	char name[dp_PNAMELEN + 1];
	char icq[ k_PS_MAXLEN + 1 ];
	char email[ k_PS_MAXLEN + 1 ];
	char location[ k_PS_MAXLEN + 1 ];
	int experience = 0;
	char info[nf_PLAYERDESCLEN + 1];


	((aui_TextField *)
	 (p->FindControl( PlayerEditWindow::CONTROL_PLAYERNAMETEXTFIELD)))->
		GetFieldText(name, dp_PNAMELEN);

	if(strlen(name) == 0) return;

	char scannedName[ 256 ] = "";
	sscanf( name, "%s", scannedName );
	if (strlen(scannedName) == 0) return;

	((aui_TextField *)
	 (p->FindControl( PlayerEditWindow::CONTROL_ICQTEXTFIELD)))->
		GetFieldText(icq, k_PS_MAXLEN);
	((aui_TextField *)
	 (p->FindControl( PlayerEditWindow::CONTROL_EMAILTEXTFIELD)))->
		GetFieldText(email, k_PS_MAXLEN);
	((aui_TextField *)
	 (p->FindControl( PlayerEditWindow::CONTROL_LOCATIONTEXTFIELD)))->
		GetFieldText(location, k_PS_MAXLEN);

	aui_SwitchGroup *sg = (aui_SwitchGroup *)p->
		FindControl( PlayerEditWindow::CONTROL_EXPERIENCESWITCHGROUP );
	aui_Radio *radio = (aui_Radio *)sg->GetChild( sg->WhichIsSelected() );
	if ( radio )
	{
		for ( sint32 i = 0; i < sg->ChildList()->L(); i++ )
		if ( p->FindControl(
			PlayerEditWindow::CONTROL_EXPERIENCE0CHECKBOX + i ) == radio )
		{
			experience = i;
			break;
		}
	}

	((aui_TextField *)
	 (p->FindControl( PlayerEditWindow::CONTROL_PLAYERINFOTEXTFIELD)))->
		GetFieldText(info, nf_PLAYERDESCLEN);


	if(p->GetPlayerSetup()) {
		if(p->GetMode() == p->EDIT || p->GetMode() == p->EDIT_GAMESETUP) {

			p->GetPlayerSetup()->SetName(name);
			p->GetPlayerSetup()->SetIcq(icq);
			p->GetPlayerSetup()->SetEmail(email);
			p->GetPlayerSetup()->SetLocation(location);
			p->GetPlayerSetup()->SetExperience(experience);


			p->GetPlayerSetup()->SetDescription(info);

			p->GetPlayerSetup()->Update();

			if(l->FindItem(p->GetPlayerSetup()))
			{
				l->ChangeItem(p->GetPlayerSetup());
			}
		}
	} else {

		p->SetPlayerSetup( new nf_PlayerSetup() );

		p->GetPlayerSetup()->SetName(name);
		p->GetPlayerSetup()->SetIcq(icq);
		p->GetPlayerSetup()->SetEmail(email);
		p->GetPlayerSetup()->SetLocation(location);
		p->GetPlayerSetup()->SetExperience(experience);


		p->GetPlayerSetup()->SetDescription(info);

		p->GetPlayerSetup()->Update();

		l->InsertItem(p->GetPlayerSetup());

		l->SelectItem(l->FindItem(p->GetPlayerSetup()));
	}

		g_netshell->GetCurrentScreen()->RemoveWindow(p->Id());
		w->Update();
}


void PlayerEditWindow::CancelButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	PlayerEditWindow *p = (PlayerEditWindow *)(control->GetParentWindow());
	g_netshell->GetCurrentScreen()->RemoveWindow(p->Id());
}
