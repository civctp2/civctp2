//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer player select window
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

#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_static.h"
#include "aui_screen.h"
#include "aui_button.h"

#include "c3_button.h"
#include "c3_static.h"
#include "c3textfield.h"

#include "netshell.h"
#include "ns_customlistbox.h"


#include "playerselectwindow.h"
#include "lobbywindow.h"
#include "playereditwindow.h"
#include "passwordscreen.h"

#include "spnewgamewindow.h" 

PlayerSelectWindow::PlayerSelectWindow(
	AUI_ERRCODE *retval )
	:
	ns_Window(
		retval,
		aui_UniqueId(),
		"playerselectwindow",
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


AUI_ERRCODE PlayerSelectWindow::InitCommon( void )
{
	m_controls = new aui_Control *[ m_numControls = CONTROL_MAX ];
	Assert( m_controls != NULL );
	if ( !m_controls ) return AUI_ERRCODE_MEMALLOCFAILED;
	memset( m_controls, 0, m_numControls * sizeof( aui_Control *) );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE PlayerSelectWindow::CreateControls( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;


	
	aui_Control *control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"playerselectwindow.titlestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_TITLESTATICTEXT ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"playerselectwindow.currentplayerstatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CURRENTPLAYERSTATICTEXT ] = control;

	control = new C3TextField(
		&errcode,
		aui_UniqueId(),
		"playerselectwindow.playernametextfield" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYERNAMETEXTFIELD ] = control;

	control = new ns_PlayerSetupListBox(
		&errcode,
		aui_UniqueId(),
		"playerselectwindow.playernamelistbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYERNAMELISTBOX ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"playerselectwindow.playernamelistbox.playernamestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYERNAMESTATICTEXT ] = control;

	control = spNew_ctp2_Button(&errcode,
		"playerselectwindow",
		"newbutton", NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_NEWBUTTON ] = control;

	control = spNew_ctp2_Button(&errcode,
		"playerselectwindow",
		"editbutton", NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_EDITBUTTON ] = control;

	control = spNew_ctp2_Button(&errcode,
		"playerselectwindow",
		"deletebutton", NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_DELETEBUTTON ] = control; 

 
	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"playerselectwindow.okbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_OKBUTTON ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"playerselectwindow.cancelbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CANCELBUTTON ] = control;


	
	aui_Ldl::SetupHeirarchyFromRoot( "playerselectwindow" );


	
	aui_Action *action;

	action = new PlayerNameTextFieldAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_PLAYERNAMETEXTFIELD ]->SetAction( action );

	action = new NewButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_NEWBUTTON ]->SetAction( action );

	action = new EditButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_EDITBUTTON ]->SetAction( action );

	action = new DeleteButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_DELETEBUTTON ]->SetAction( action );

	action = new OKButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_OKBUTTON ]->SetAction( action );

	action = new CancelButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_CANCELBUTTON ]->SetAction( action );

	action = new PlayerListBoxAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_PLAYERNAMELISTBOX ]->SetAction( action );

	
		((aui_ListBox *)m_controls[ CONTROL_PLAYERNAMELISTBOX ])->
		SetForceSelect( TRUE );

	Update();

	return AUI_ERRCODE_OK;
}

nf_PlayerSetup *PlayerSelectWindow::GetPlayerSetup(NETFunc::Player *player) {
	ns_PlayerSetupListBox *l = (ns_PlayerSetupListBox *)(FindControl( PlayerSelectWindow ::CONTROL_PLAYERNAMELISTBOX ));
	ns_PlayerSetupListBox::iterator i;
	nf_PlayerSetup *s;

	for(i = l->begin(); i != l->end(); i++) {
		s = (*i);
		if(strcmp(player->GetName(), s->GetName()) == 0)
			return s;
	}

	s = new nf_PlayerSetup(player);

	l->InsertItem(s);

	return s;
}

void PlayerSelectWindow::Update(void)
{
	ns_PlayerSetupListBox *listbox = (ns_PlayerSetupListBox *)(FindControl( PlayerSelectWindow::CONTROL_PLAYERNAMELISTBOX ));
	ns_PlayerSetupItem *item = (ns_PlayerSetupItem *)listbox->GetSelectedItem();
//	c3_Button *b_ok = (c3_Button *)(FindControl( PlayerSelectWindow::CONTROL_OKBUTTON ));
	c3_Button *b_edit = (c3_Button *)(FindControl( PlayerSelectWindow::CONTROL_EDITBUTTON ));
	c3_Button *b_del = (c3_Button *)(FindControl( PlayerSelectWindow::CONTROL_DELETEBUTTON ));

	aui_TextField *tf = (aui_TextField *)FindControl
		( PlayerSelectWindow::CONTROL_PLAYERNAMETEXTFIELD );

	if (item) {
		b_edit->Enable(TRUE);
		b_del->Enable(TRUE);
		tf->SetFieldText( item->GetNetShellObject()->GetNETFuncObject()->GetName() );
	} else {
		b_edit->Enable(FALSE);
		b_del->Enable(FALSE);
		tf->SetFieldText( "" );
	}
}

AUI_ERRCODE PlayerSelectWindow::Idle( void )
{
	while (NETFunc::Message * m = g_netfunc->GetMessage()) 
    {
		g_netfunc->HandleMessage(m);

		if (dp_SESSIONLOST_PACKET_ID == m->GetCode())
		{
			passwordscreen_displayMyWindow(PASSWORDSCREEN_MODE_CONNECTIONLOST);
		}

		delete m;
	}

	return AUI_ERRCODE_OK;
}

void PlayerSelectWindow::PlayerListBoxAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	switch ( action )
	{
	case AUI_LISTBOX_ACTION_SELECT:
		((PlayerSelectWindow *)control->GetParent())->Update();
		break;

	case AUI_LISTBOX_ACTION_DOUBLECLICKSELECT:
	{
		aui_ListBox *listbox = (aui_ListBox *)control;
		sint32 index = listbox->ExtractDoubleClickedItem( data );
		aui_Item *item = listbox->GetItemByIndex( index );
		listbox->SelectItem( item );

		c3_Button *button = (c3_Button *)((PlayerSelectWindow *)control->
			GetParentWindow())->FindControl( CONTROL_OKBUTTON );
		button->GetAction()->Execute( button, AUI_BUTTON_ACTION_EXECUTE, 0 );
		break;
	}

	default:
		break;
	}
}

AUI_ERRCODE PlayerSelectWindow::SetParent( aui_Region *region )
{
	AUI_ERRCODE r = ns_Window::SetParent( region );

	if ( region ) {

		
		m_controls[ CONTROL_PLAYERNAMETEXTFIELD ]->SetKeyboardFocus();
	}

	return r;
}

void PlayerSelectWindow::NewButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	PlayerSelectWindow *w = (PlayerSelectWindow *)control->GetParentWindow();
	ns_PlayerSetupListBox *listbox = (ns_PlayerSetupListBox *)(w->FindControl( PlayerSelectWindow ::CONTROL_PLAYERNAMELISTBOX ));
	ns_PlayerSetupItem *item = (ns_PlayerSetupItem *)listbox->GetSelectedItem();
	PlayerEditWindow *p = (PlayerEditWindow *)g_netshell->FindWindow( NetShell::WINDOW_PLAYEREDIT );

	if(item)
		listbox->DeselectItem(item);

	p->SetPlayerSetup(NULL);
	p->SetMode(p->EDIT);
	g_netshell->GetCurrentScreen()->AddWindow(p, TRUE);
}


void PlayerSelectWindow::EditButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	PlayerSelectWindow *w = (PlayerSelectWindow *)control->GetParentWindow();
	ns_PlayerSetupListBox *listbox = (ns_PlayerSetupListBox *)(w->FindControl( PlayerSelectWindow ::CONTROL_PLAYERNAMELISTBOX ));
	ns_PlayerSetupItem *item = (ns_PlayerSetupItem *)listbox->GetSelectedItem();

	PlayerEditWindow *p = (PlayerEditWindow *)g_netshell->FindWindow( NetShell::WINDOW_PLAYEREDIT );

	if(item) {
		p->SetPlayerSetup(item->GetNetShellObject()->GetNETFuncObject());
		p->SetMode(p->EDIT);
		g_netshell->GetCurrentScreen()->AddWindow(p, TRUE);
	}
}


void PlayerSelectWindow::DeleteButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	PlayerSelectWindow *w = (PlayerSelectWindow *)control->GetParentWindow();
	ns_PlayerSetupListBox *listbox = (ns_PlayerSetupListBox *)(w->FindControl( PlayerSelectWindow ::CONTROL_PLAYERNAMELISTBOX ));
	ns_PlayerSetupItem *item = (ns_PlayerSetupItem *)listbox->GetSelectedItem();

	if(item) {
		nf_PlayerSetup *player = item->GetNetShellObject()->GetNETFuncObject();
		listbox->DeleteItem(player);
		w->Update();
	}
}


void PlayerSelectWindow::OKButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	PlayerSelectWindow *w = (PlayerSelectWindow *)control->GetParentWindow();
	ns_PlayerSetupListBox *listbox = (ns_PlayerSetupListBox *)w->
		FindControl( PlayerSelectWindow::CONTROL_PLAYERNAMELISTBOX );

	aui_TextField *tf = (aui_TextField *)w->FindControl(
		PlayerSelectWindow::CONTROL_PLAYERNAMETEXTFIELD );

	MBCHAR name[ dp_PNAMELEN + 1 ];
	memset( name, 0, sizeof( name ) );
	tf->GetFieldText( name, dp_PNAMELEN );

	
	if ( strlen( name ) )
	{
		
		ListPos pos = listbox->GetPane()->ChildList()->GetHeadPosition();
		sint32 i;
		for ( i = 0; i < listbox->NumItems(); i++ )
		{
			ns_PlayerSetupItem *item = (ns_PlayerSetupItem *)
				listbox->GetPane()->ChildList()->GetNext( pos );

			if ( !strnicmp(
				name,
				item->GetNetShellObject()->GetNETFuncObject()->GetName(),
				dp_PNAMELEN ) )
			{
				listbox->SelectItem( i );
				break;
			}
		}

		
		if ( i == listbox->NumItems() )
		{
			
			c3_Button *newbut = (c3_Button *)w->
				FindControl( w->CONTROL_NEWBUTTON );
			newbut->GetAction()->
				Execute( newbut, AUI_BUTTON_ACTION_EXECUTE, 0 );

			
			PlayerEditWindow *pew = (PlayerEditWindow *)g_netshell->
				FindWindow( NetShell::WINDOW_PLAYEREDIT );
			((aui_TextField *)pew->
			 FindControl( pew->CONTROL_PLAYERNAMETEXTFIELD ))
				->SetFieldText( name );

			
			c3_Button *okbut = (c3_Button *)pew->
				FindControl( pew->CONTROL_OKBUTTON );
			okbut->GetAction()->Execute( okbut, AUI_BUTTON_ACTION_EXECUTE, 0 );
		}
	}

	
	ns_PlayerSetupItem *item =
		(ns_PlayerSetupItem *)listbox->GetSelectedItem();

	if(item) {
		
		((aui_ListBox *)listbox)->RemoveItem( item->Id() );
		((aui_ListBox *)listbox)->InsertItem( item, 0 );
		listbox->SelectItem( (sint32)0 );

		g_playersetup = nf_PlayerSetup(*(item->GetNetShellObject()->
										 GetNETFuncObject()));
		g_netfunc->SetPlayerSetup(&g_playersetup);

		g_netfunc->Connect();

		LobbyWindow *lw = (LobbyWindow *)g_netshell->
			FindWindow( NetShell::WINDOW_LOBBY );
		g_netshell->GotoScreen( NetShell::SCREEN_LOBBY );
		lw->Update();
	}
}


void PlayerSelectWindow::PlayerNameTextFieldAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_TEXTFIELD_ACTION_EXECUTE ) return;

	PlayerSelectWindow *w = (PlayerSelectWindow *)control->GetParentWindow();

	
	aui_Control *ctrl = w->FindControl( w->CONTROL_OKBUTTON );
	ctrl->GetAction()->Execute( ctrl, AUI_BUTTON_ACTION_EXECUTE, 0 );
}


void PlayerSelectWindow::CancelButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	g_netfunc->Disconnect();
	if(g_netfunc->GetTransport() && g_netfunc->GetTransport()->GetType() == NETFunc::Transport::INTERNET)
		g_netshell->GotoScreen( NetShell::SCREEN_SERVERSELECT );
	else
		g_netshell->GotoScreen( NetShell::SCREEN_CONNECTIONSELECT );
}
