//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer connection type select window
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
#include "aui_button.h"
#include "aui_stringtable.h"


#include "c3_static.h"
#include "c3_button.h"


#include "civapp.h"
extern CivApp *g_civApp;


#include "netshell.h"
#include "ns_customlistbox.h"




#include "connectionselectwindow.h"
#include "playerselectwindow.h"
#include "passwordscreen.h"
#include "serverselectwindow.h"


ConnectionSelectWindow::ConnectionSelectWindow(
	AUI_ERRCODE *retval )
	:
	ns_Window(
		retval,
		aui_UniqueId(),
		"connectionselectwindow",
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



AUI_ERRCODE ConnectionSelectWindow::InitCommon( void )
{
	m_controls = new aui_Control *[ m_numControls = CONTROL_MAX ];
	Assert( m_controls != NULL );
	if ( !m_controls ) return AUI_ERRCODE_MEMALLOCFAILED;
	memset( m_controls, 0, m_numControls * sizeof( aui_Control *) );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE ConnectionSelectWindow::CreateControls( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;


	

	aui_Control *control;









	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"connectionselectwindow.titlestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_TITLESTATICTEXT ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"connectionselectwindow.connectiontypestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CONNECTIONTYPESTATICTEXT ] = control;

	control = new ns_TransportListBox(
		&errcode,
		aui_UniqueId(),
		"connectionselectwindow.connectiontypelistbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CONNECTIONTYPELISTBOX ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"connectionselectwindow.connectiondescriptionstatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CONNECTIONDESCRIPTIONSTATICTEXT ] = control;

	
	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"connectionselectwindow.disclaimer" );
	Assert(AUI_NEWOK(control, errcode));
	if(!AUI_NEWOK(control,errcode)) return errcode;
	m_controls[CONTROL_DISCLAIMERTEXT] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"connectionselectwindow.connectiondescriptiontextfield" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CONNECTIONDESCRIPTIONTEXTFIELD ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"connectionselectwindow.okbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_OKBUTTON ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"connectionselectwindow.cancelbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CANCELBUTTON ] = control;

	

	aui_Ldl::SetupHeirarchyFromRoot( "connectionselectwindow" );


	

	aui_Action *action;






	action = new OKButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_OKBUTTON ]->SetAction( action );

	action = new CancelButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_CANCELBUTTON ]->SetAction( action );

	action = new ConnectionListBoxAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_CONNECTIONTYPELISTBOX ]->SetAction( action );

	

	m_controls[ CONTROL_CONNECTIONDESCRIPTIONTEXTFIELD ]->Enable( FALSE );

	Update();

	return AUI_ERRCODE_OK;
}


ConnectionSelectWindow::~ConnectionSelectWindow()
{
}


void ConnectionSelectWindow::Update(void)
{
	c3_Static *text = (c3_Static *)(FindControl( ConnectionSelectWindow::CONTROL_CONNECTIONDESCRIPTIONTEXTFIELD ));
	ns_TransportListBox *listbox = (ns_TransportListBox *)(FindControl( ConnectionSelectWindow::CONTROL_CONNECTIONTYPELISTBOX ));
	ns_TransportItem *item = (ns_TransportItem *)listbox->GetSelectedItem();
	aui_Button *b = (aui_Button *)(FindControl( ConnectionSelectWindow::CONTROL_OKBUTTON ));


	if(item) {
		b->Enable(TRUE);

		NETFunc::Transport *t = item->GetNetShellObject()->GetNETFuncObject();













		char *filename = t->GetFileName();
		char *slash = strrchr(filename, '/');
		if(slash)
			filename = slash + 1;
		char *dot = strchr(filename, '.');
		if(dot)
			*dot = 0;
		static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];
		sprintf( block, "connectionselectwindow.connectiondescriptionstrings.%s", filename );
		if(dot)
			*dot = '.';

		ns_String description = ns_String(block);
		if(description.GetString())
			text->SetText(description.GetString());
		else
			text->SetText("");
	} else {
		b->Enable(FALSE);

		ns_String description = ns_String("connectionselectwindow.connectiondescriptionstrings");
		if(description.GetString())
			text->SetText(description.GetString());
		else
			text->SetText("");
	}
}



AUI_ERRCODE ConnectionSelectWindow::Idle( void )
{	
    while (NETFunc::Message * m = g_netfunc->GetMessage()) 
    {
		g_netfunc->HandleMessage(m);

        if (NETFunc::Message::NETWORKERR == m->GetCode())
		{
			passwordscreen_displayMyWindow(PASSWORDSCREEN_MODE_CONNECTIONERR);
		}

		delete m;
	}

	if (g_netfunc->GetStatus() == NETFunc::READY) 
    {
		g_netshell->GotoScreen( NetShell::SCREEN_PLAYERSELECT );
	}

	return AUI_ERRCODE_OK;
}



void ConnectionSelectWindow::ConnectionListBoxAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	switch ( action )
	{
	case AUI_LISTBOX_ACTION_SELECT:
		((ConnectionSelectWindow *)control->GetParent())->Update();
		break;

	case AUI_LISTBOX_ACTION_DOUBLECLICKSELECT:
	{
		aui_ListBox *listbox = (aui_ListBox *)control;
		sint32 index = listbox->ExtractDoubleClickedItem( data );
		aui_Item *item = listbox->GetItemByIndex( index );
		listbox->SelectItem( item );

		aui_Button *button = (aui_Button *)((ConnectionSelectWindow *)control->
			GetParentWindow())->FindControl( CONTROL_OKBUTTON );
		button->GetAction()->Execute( button, AUI_BUTTON_ACTION_EXECUTE, 0 );
		break;
	}

	default:
		break;
	}
}



void ConnectionSelectWindow::OKButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	ConnectionSelectWindow *w = (ConnectionSelectWindow *)control->
		GetParentWindow();
	ns_TransportItem *item = (ns_TransportItem *)
		((ns_TransportListBox *)w->
		 FindControl( CONTROL_CONNECTIONTYPELISTBOX ))->GetSelectedItem();
	if(item) {
		NETFunc::Transport *t = item->GetNetShellObject()->GetNETFuncObject();
		if ( t->GetType() == NETFunc::Transport::UNKNOWN )
		{
			if ( ((FakeTransport *)t)->GetSubType() ==
					FakeTransport::EMAIL )
			{
				
				NetShell::Leave( k_NS_FLAGS_DESTROY, TRUE );
				g_civApp->PostStartGameAction();
				
			}
			else if ( ((FakeTransport *)t)->GetSubType() ==
					FakeTransport::HOTSEAT )
			{
				
				NetShell::Leave( k_NS_FLAGS_DESTROY, TRUE );
				g_civApp->PostStartGameAction();
				
			}
		}
		else
		{
			NETFunc::STATUS status = g_netfunc->SetTransport(t);
			if ( status != NETFunc::OK )
			{
				passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_NODIALUP );
			} else if(t->GetType() == NETFunc::Transport::INTERNET) {
				
				g_netshell->GotoScreen( NetShell::SCREEN_SERVERSELECT );
				((ServerSelectWindow *)g_netshell->FindWindow( NetShell::WINDOW_SERVERSELECT ))->Update( true );
			}
		}
	}
}



void ConnectionSelectWindow::CancelButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	control->Enable(FALSE); 
	
	NetShell::Leave( k_NS_FLAGS_DESTROY | k_NS_FLAGS_MAINMENU, TRUE );
}
