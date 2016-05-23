//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer server select window
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
#include "aui_textfield.h"

#include "c3_button.h"
#include "c3_static.h"

#include "netshell.h"
#include "ns_customlistbox.h"

#include "serverselectwindow.h"
#include "playerselectwindow.h"
#include "passwordscreen.h"
#include "dialogboxwindow.h"

static DialogBoxWindow *s_dbw = NULL;

MBCHAR g_serverName[ 100 + 1 ] = "";

ServerSelectWindow::ServerSelectWindow(
	AUI_ERRCODE *retval )
	:
	ns_Window(
		retval,
		aui_UniqueId(),
		"serverselectwindow",
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
}

AUI_ERRCODE ServerSelectWindow::InitCommon( void )
{
	m_controls = new aui_Control *[ m_numControls = CONTROL_MAX ];
	Assert( m_controls != NULL );
	if ( !m_controls ) return AUI_ERRCODE_MEMALLOCFAILED;
	memset( m_controls, 0, m_numControls * sizeof( aui_Control *) );

	m_dbActionArray[ 0 ] = new DialogBoxPopDownAction;
	Assert( m_dbActionArray[ 0 ] != NULL );
	if ( !m_dbActionArray[ 0 ] ) return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE ServerSelectWindow::CreateControls( void )
{
	AUI_ERRCODE     errcode = AUI_ERRCODE_OK;
	aui_Control *   control = new c3_Static
        (&errcode, aui_UniqueId(), "serverselectwindow.titlestatictext");
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_TITLESTATICTEXT ] = control;

	control = new ns_ServerListBox(
		&errcode,
		aui_UniqueId(),
		"serverselectwindow.selectserverlistbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_SELECTSERVERLISTBOX ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"serverselectwindow.okbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_OKBUTTON ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"serverselectwindow.cancelbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CANCELBUTTON ] = control;


	aui_Ldl::SetupHeirarchyFromRoot( "serverselectwindow" );


	aui_Action *action;

	action = new OKButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_OKBUTTON ]->SetAction( action );

	action = new CancelButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_CANCELBUTTON ]->SetAction( action );

	action = new ServerListBoxAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_SELECTSERVERLISTBOX ]->SetAction( action );




	((aui_ListBox *)m_controls[ CONTROL_SELECTSERVERLISTBOX ])->
		SetForceSelect( TRUE );

	Update();

	return AUI_ERRCODE_OK;
}

ServerSelectWindow::~ServerSelectWindow()
{
	delete m_dbActionArray[ 0 ];
}

void ServerSelectWindow::Update( bool wait )
{
	ns_ServerListBox *listbox = (ns_ServerListBox *)(FindControl( ServerSelectWindow::CONTROL_SELECTSERVERLISTBOX ));
	ns_ServerItem *item = (ns_ServerItem *)listbox->GetSelectedItem();
	c3_Button *b = (c3_Button *)(FindControl( ServerSelectWindow::CONTROL_OKBUTTON ));
	if(wait)
		if ( !s_dbw )
		s_dbw = DialogBoxWindow::PopUp(
			"waitserverdialogboxwindow",
			m_dbActionArray );
	if(item)
	{
		b->Enable(TRUE);

		static bool didThisAlready = false;
		if ( !didThisAlready )
		{
			NETFunc::Server *server = item->GetNetShellObject()->GetNETFuncObject();
			g_netfunc->SetServer( server );
			didThisAlready = true;

			((aui_ListBox *)m_controls[ CONTROL_SELECTSERVERLISTBOX ])->
				SetForceSelect( FALSE );

		}
	}
	else
	{
		b->Enable(FALSE);
	}
}

AUI_ERRCODE ServerSelectWindow::Idle( void )
{
	if (g_netfunc)
    {
        while (NETFunc::Message * m = g_netfunc->GetMessage())
        {
			g_netfunc->HandleMessage(m);

			switch ( m->GetCode() )
			{
			case dp_SESSIONLOST_PACKET_ID:
				passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_CONNECTIONLOST );
				break;

			case dp_OBJECTDELTA_PACKET_ID:
                {
            	    dp_objectDelta_packet_t *   p =
                        (dp_objectDelta_packet_t *) m->GetBody();

				    if (p->key[0] == dp_KEY_SERVERPINGS
					    && (p->status == dp_RES_CREATED || p->status == dp_RES_CHANGED)
					    && p->data.serv.loss_percent != 100 && s_dbw
                       )
                    {
					    DialogBoxWindow::PopDown(s_dbw);
					    s_dbw = NULL;
				    }
                }
				break;
			default:
				break;
			}

			delete m;
		}

		if(g_netfunc->GetStatus() == NETFunc::START)
			g_netshell->GotoScreen( NetShell::SCREEN_CONNECTIONSELECT );
	}

	return AUI_ERRCODE_OK;
}

void ServerSelectWindow::ServerListBoxAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	ns_ServerListBox *listbox = (ns_ServerListBox *)control;
	switch ( action )
	{
	case AUI_LISTBOX_ACTION_SELECT:
		if(listbox->size())
			((ServerSelectWindow *)control->GetParent())->Update();
		break;

	case AUI_LISTBOX_ACTION_DOUBLECLICKSELECT:
	{
		sint32 index = listbox->ExtractDoubleClickedItem( data );
		aui_Item *item = listbox->GetItemByIndex( index );
		listbox->SelectItem( item );

		c3_Button *button = (c3_Button *)((ServerSelectWindow *)control->
			GetParentWindow())->FindControl( CONTROL_OKBUTTON );
		button->GetAction()->Execute( button, AUI_BUTTON_ACTION_EXECUTE, 0 );
		break;
	}

	default:
		break;
	}
}

void ServerSelectWindow::OKButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	ServerSelectWindow *w = (ServerSelectWindow *)control->GetParentWindow();
	ns_ServerListBox *listbox = (ns_ServerListBox *)(w->FindControl( ServerSelectWindow ::CONTROL_SELECTSERVERLISTBOX ));
	ns_ServerItem *item = (ns_ServerItem *)listbox->GetSelectedItem();

	if(item) {

		strncpy( g_serverName, item->GetText(), 100 );

		NETFunc::Server *server = item->GetNetShellObject()->GetNETFuncObject();
		g_netfunc->SetServer(server);
		g_netfunc->Login( "", "" );

		g_netshell->GotoScreen( NetShell::SCREEN_PLAYERSELECT );
	}
	else {

		strncpy( g_serverName, "", 100 );
	}
}

void ServerSelectWindow::CancelButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	strncpy( g_serverName, "", 100 );

	g_netfunc->Disconnect();
}

void ServerSelectWindow::DialogBoxPopDownAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_netfunc->Disconnect();


	if ( s_dbw )
	{
		DialogBoxWindow::PopDown( s_dbw );
		s_dbw = NULL;
	}

	g_netshell->GotoScreen( NetShell::SCREEN_CONNECTIONSELECT );
}
