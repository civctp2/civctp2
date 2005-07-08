










#include "c3.h"


#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_static.h"
#include "aui_button.h"


#include "c3_button.h"
#include "c3_static.h"
#include "c3textfield.h"


#include "netshell.h"
#include "ns_customlistbox.h"


#include "passwordscreen.h"


#include "lobbychangewindow.h"
#include "lobbywindow.h"


LobbyChangeWindow::LobbyChangeWindow(
	AUI_ERRCODE *retval )
	:
	ns_Window(
		retval,
		aui_UniqueId(),
		"lobbychangewindow",
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



AUI_ERRCODE LobbyChangeWindow::InitCommon( void )
{
	m_controls = new aui_Control *[ m_numControls = CONTROL_MAX ];
	Assert( m_controls != NULL );
	if ( !m_controls ) return AUI_ERRCODE_MEMALLOCFAILED;
	memset( m_controls, 0, m_numControls * sizeof( aui_Control *) );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE LobbyChangeWindow::CreateControls( void )
{
	AUI_ERRCODE errcode;


	

	aui_Control *control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"lobbychangewindow.titlestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_TITLESTATICTEXT ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"lobbychangewindow.currentlobbystatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CURRENTLOBBYSTATICTEXT ] = control;

	control = new C3TextField(
		&errcode,
		aui_UniqueId(),
		"lobbychangewindow.currentlobbytextfield" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CURRENTLOBBYTEXTFIELD ] = control;









	control = new ns_LobbyListBox(
		&errcode,
		aui_UniqueId(),
		"lobbychangewindow.lobbieslistbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_LOBBIESLISTBOX ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"lobbychangewindow.okbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_OKBUTTON ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"lobbychangewindow.cancelbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CANCELBUTTON ] = control;


	

	aui_Ldl::SetupHeirarchyFromRoot( "lobbychangewindow" );


	

	aui_Action *action;

	action = new OKButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_OKBUTTON ]->SetAction( action );

	action = new CancelButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_CANCELBUTTON ]->SetAction( action );

	action = new LobbyListBoxAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_LOBBIESLISTBOX ]->SetAction( action );

	

	
	aui_Header *hdr = ((aui_ListBox *)m_controls[ CONTROL_LOBBIESLISTBOX ])
		->GetHeader();
	uint32 switchid = hdr->WhichIsSelected();
	aui_Switch *sw = (aui_Switch *)hdr->GetChild( switchid );
	if ( sw ) sw->SetState( sw->GetState() );


	Update();

	m_controls[ CONTROL_CURRENTLOBBYTEXTFIELD ]->Enable( FALSE );


	return AUI_ERRCODE_OK;
}


void LobbyChangeWindow::Update(void)
{
	ns_LobbyListBox *listbox = (ns_LobbyListBox *)(FindControl( LobbyChangeWindow::CONTROL_LOBBIESLISTBOX ));
	ns_LobbyItem *item = (ns_LobbyItem *)listbox->GetSelectedItem();
	c3_Button *b = (c3_Button *)(FindControl( LobbyChangeWindow::CONTROL_OKBUTTON ));
	if(item)
		b->Enable(TRUE);
	else
		b->Enable(FALSE);
}


AUI_ERRCODE LobbyChangeWindow::Idle( void )
{
	NETFunc::Message *m;
	
	while((m = g_netfunc->GetMessage())) {
		
		
		
		g_netfunc->HandleMessage(m);

		switch ( m->GetCode() )
		{
		case dp_SESSIONLOST_PACKET_ID:
			passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_CONNECTIONLOST );
			break;

		default:
			break;
		}

		delete m;
	}
	return AUI_ERRCODE_OK;
}


void LobbyChangeWindow::LobbyListBoxAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	switch ( action )
	{
	case AUI_LISTBOX_ACTION_SELECT:
		((LobbyChangeWindow *)control->GetParent())->Update();
		break;

	case AUI_LISTBOX_ACTION_DOUBLECLICKSELECT:
	{
		aui_ListBox *listbox = (aui_ListBox *)control;
		sint32 index = listbox->ExtractDoubleClickedItem( data );
		aui_Item *item = listbox->GetItemByIndex( index );
		listbox->SelectItem( item );

		c3_Button *button = (c3_Button *)((LobbyChangeWindow *)control->
			GetParentWindow())->FindControl( CONTROL_OKBUTTON );
		button->GetAction()->Execute( button, AUI_BUTTON_ACTION_EXECUTE, 0 );
		break;
	}

	default:
		break;
	}
}


AUI_ERRCODE LobbyChangeWindow::SetParent( aui_Region *region )
{
	if(region)
	
	((aui_TextField *)m_controls[CONTROL_CURRENTLOBBYTEXTFIELD])->
		SetFieldText(g_netfunc->GetSession()->GetName());

	return ns_Window::SetParent( region );
}


void LobbyChangeWindow::OKButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	LobbyChangeWindow *w = (LobbyChangeWindow *)control->GetParentWindow();
	ns_LobbyListBox *listbox = (ns_LobbyListBox *)(w->FindControl( LobbyChangeWindow ::CONTROL_LOBBIESLISTBOX ));
	ns_LobbyItem *item = (ns_LobbyItem *)listbox->GetSelectedItem();

	if(item) {
		ns_Lobby *lobby = item->GetNetShellObject();
		if(!lobby->IsMine()) {
			g_netfunc->Join(lobby->GetNETFuncObject());
			LobbyWindow *w = (LobbyWindow *)(g_netshell->FindWindow(NetShell::WINDOW_LOBBY));
			w->Update();
		}
		
		g_netshell->GotoScreen( NetShell::SCREEN_LOBBY );
	}
}



void LobbyChangeWindow::CancelButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	
	g_netshell->GotoScreen( NetShell::SCREEN_LOBBY );
}
