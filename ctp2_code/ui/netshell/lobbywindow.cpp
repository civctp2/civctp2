//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Lobby window for multiplayer games.
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
// - Memory leak repaired.
// - Memory leak report prevented.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_screen.h"


#include "netshell.h"
#include "ns_chatbox.h"
#include "ns_customlistbox.h"
#include "dialogboxwindow.h"




#include "c3_static.h"
#include "c3_button.h"
#include "textradio.h"
#include "c3textfield.h"


#include "lobbywindow.h"
#include "playereditwindow.h"
#include "playerselectwindow.h"
#include "allinonewindow.h"
#include "passwordscreen.h"


#include "ctp2_Switch.h"
#include "spnewgamewindow.h" 

static DialogBoxWindow *s_dbw = NULL;
static sint32 s_startedLeavingAt = 0;

LobbyWindow *g_lobbyWindow = NULL;

extern MBCHAR g_serverName[ 100 + 1 ];



#define k_LEAVE_LOBBY_TIMEOUT 70


LobbyWindow::LobbyWindow(
	AUI_ERRCODE *retval )
	:
	ns_Window(
		retval,
		aui_UniqueId(),
		"lobbywindow",
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



AUI_ERRCODE LobbyWindow::InitCommon( void )
{
	g_lobbyWindow = this;

	m_messageLobbyEnter = new ns_String("strings.system.lobbyenter");
	s_startedLeavingAt = 0;

	wait = false;

	
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	m_PPStrings = new aui_StringTable(
		&errcode,
		"strings.ppt" );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return AUI_ERRCODE_HACK;

	m_controls = new aui_Control *[ m_numControls = CONTROL_MAX ];
	Assert( m_controls != NULL );
	if ( !m_controls ) return AUI_ERRCODE_MEMALLOCFAILED;
	memset( m_controls, 0, m_numControls * sizeof( aui_Control *) );

	m_dbActionArray[ 0 ] = new DialogBoxPopDownAction;
	Assert( m_dbActionArray[ 0 ] != NULL );
	if ( !m_dbActionArray[ 0 ] ) return AUI_ERRCODE_MEMALLOCFAILED;


	return AUI_ERRCODE_OK;
}



AUI_ERRCODE LobbyWindow::CreateControls( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;


	

	aui_Control *control;









	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.titlestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_TITLESTATICTEXT ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.gamesstatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_GAMESSTATICTEXT ] = control;

	control = new ns_GameListBox(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.gameslistbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_GAMESLISTBOX ] = control;

	control = new ns_ChatBox(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.chatbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CHATBOX ] = control;

	
	
	control = spNew_ctp2_Button(&errcode,
		"lobbywindow",
		"changebutton", NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CHANGEBUTTON ] = control;

	control = spNew_ctp2_Button(&errcode,
		"lobbywindow",
		"joinbutton", NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_JOINBUTTON ] = control;

	control = spNew_ctp2_Button(&errcode,
		"lobbywindow",
		"createbutton", NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CREATEBUTTON ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.currentlobbystatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CURRENTLOBBYSTATICTEXT ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.currentlobbytextfield" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CURRENTLOBBYTEXTFIELD ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.currentserverstatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CURRENTSERVERSTATICTEXT ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.currentservertextfield" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CURRENTSERVERTEXTFIELD ] = control;

	control = new ns_PlayerListBox(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.playerslistbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYERSLISTBOX ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.playerslistbox.playersstatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYERSSTATICTEXT ] = control;

	control = spNew_ctp2_Button(&errcode,
		"lobbywindow",
		"infobutton", NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_INFOBUTTON ] = control;





	control = new ctp2_Switch(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.muteswitch");
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_MUTESWITCH ] = control;





	control = new ctp2_Switch(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.whisperswitch");
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_WHISPERSWITCH ] = control;

	control = spNew_ctp2_Button(&errcode,
		"lobbywindow",
		"reviewbutton", NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_REVIEWBUTTON ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"lobbywindow.backbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_BACKBUTTON ] = control;


	

	aui_Ldl::SetupHeirarchyFromRoot( "lobbywindow" );


	

	aui_Action *action;






	action = new ChangeButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_CHANGEBUTTON ]->SetAction( action );

	action = new JoinButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_JOINBUTTON ]->SetAction( action );

	action = new CreateButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_CREATEBUTTON ]->SetAction( action );

	action = new InfoButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_INFOBUTTON ]->SetAction( action );

	action = new MuteSwitchAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_MUTESWITCH ]->SetAction( action );

	action = new WhisperSwitchAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_WHISPERSWITCH ]->SetAction( action );

	action = new ReviewButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_REVIEWBUTTON ]->SetAction( action );

	action = new BackButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_BACKBUTTON ]->SetAction( action );

	action = new PlayersListBoxAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_PLAYERSLISTBOX ]->SetAction( action );

	action = new GamesListBoxAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_GAMESLISTBOX ]->SetAction( action );


	

	
	((aui_Static *)m_controls[CONTROL_CURRENTSERVERTEXTFIELD])->SetText( "" );
	m_controls[ CONTROL_CURRENTSERVERSTATICTEXT ]->Hide();
	m_controls[ CONTROL_CURRENTSERVERTEXTFIELD ]->Hide();

	
	((aui_ListBox *)m_controls[ CONTROL_PLAYERSLISTBOX ])->
		SetForceSelect( TRUE );
	((aui_ListBox *)m_controls[ CONTROL_GAMESLISTBOX ])->
		SetForceSelect( TRUE );

	((aui_ListBox *)m_controls[CONTROL_GAMESLISTBOX])->ShouldDraw(TRUE);

	
	aui_Header *hdr = ((aui_ListBox *)m_controls[ CONTROL_PLAYERSLISTBOX ])
		->GetHeader();
	uint32 switchid = hdr->WhichIsSelected();
	aui_Switch *sw = (aui_Switch *)hdr->GetChild( switchid );
	if ( sw ) sw->SetState( sw->GetState() );

	hdr = ((aui_ListBox *)m_controls[ CONTROL_GAMESLISTBOX ])
		->GetHeader();
	switchid = hdr->WhichIsSelected();
	sw = (aui_Switch *)hdr->GetChild( switchid );
	if ( sw ) sw->SetState( sw->GetState() );


	
	sw = (aui_Switch *)m_controls[ CONTROL_WHISPERSWITCH ];
	sw->SetState( sw->GetState() );


	
	Update( FALSE );


	return AUI_ERRCODE_OK;
}


LobbyWindow::~LobbyWindow( void )
{
	sint32 numActions = sizeof( m_dbActionArray ) / sizeof( aui_Action *);
	for ( sint32 i = 0; i < numActions; i++ )
	{
		if ( m_dbActionArray[ i ] )
		{
			delete m_dbActionArray[ i ];
			m_dbActionArray[ i ] = NULL;
		}
	}

	delete m_messageLobbyEnter;

	if ( m_PPStrings )
	{
		delete m_PPStrings;
		m_PPStrings = NULL;
	}

	g_lobbyWindow = NULL;
}


void LobbyWindow::Update(BOOL init)
{
	if ( init )
	{
		ns_PlayerListBox *list = (ns_PlayerListBox *)
			FindControl( LobbyWindow ::CONTROL_PLAYERSLISTBOX );
		list->Destroy();

		aui_Static *text = (aui_Static *)
			FindControl( LobbyWindow ::CONTROL_CURRENTLOBBYTEXTFIELD );
		text->SetText("");

		text = (aui_Static *)
			FindControl( LobbyWindow ::CONTROL_CURRENTSERVERTEXTFIELD );
		text->SetText("");

		wait = true;
	}
}


AUI_ERRCODE LobbyWindow::Idle( void )
{
	NETFunc::Message *m;
	bool n = true;
	NETFunc::KeyStruct lobbyKey;
	bool joinedLobby = false;
	if(wait) {
		

		if ( !s_dbw )
			s_dbw = DialogBoxWindow::PopUp(
				"joinlobbydialogboxwindow",
				m_dbActionArray );

			
			
			((aui_Control *)((ns_ChatBox *)FindControl( CONTROL_CHATBOX ))->
				GetInputField())->ReleaseKeyboardFocus();

		wait = false;
	}
	
	while(n) {
		
		if((m = g_netfunc->GetMessage())) {
			
			
			g_netfunc->HandleMessage(m);
			


			switch ( m->GetCode() )
			{
			case dp_SESSIONLOST_PACKET_ID:
				passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_CONNECTIONLOST );
				break;

			default:
				break;
			}

			
			if ( m->GetCode() == NETFunc::Message::NETWORKERR )
			{
				
				passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_NOLOBBY );
				s_startedLeavingAt = 0;
			}
			else if(m->GetCode() == NETFunc::Message::GAMESESSION)
			{


			}
			else if(m->GetCode() == NETFunc::Message::ENTERLOBBY) {
				
				
				memcpy(&lobbyKey, (NETFunc::KeyStruct *)(m->GetBody()), sizeof(NETFunc::KeyStruct));
				joinedLobby = true;
			} else if(m->GetCode() == NETFunc::Message::PLAYERPACKET) {
				ns_PlayerListBox *l = (ns_PlayerListBox *)(FindControl( LobbyWindow::CONTROL_PLAYERSLISTBOX ));
				ns_PlayerItem *item = (ns_PlayerItem *)(l->GetSelectedItem());
				if(item) {
					NETFunc::Player *player = item->GetNetShellObject()->GetNETFuncObject();
					PlayerEditWindow *p = (PlayerEditWindow *)g_netshell->FindWindow( NetShell::WINDOW_PLAYEREDIT );
					*(NETFunc::PlayerSetup *)&g_rplayersetup = NETFunc::PlayerSetup(player);
					g_rplayersetup.Packet::Set(m->GetBodySize(), m->GetBody());
					p->SetPlayerSetup(&g_rplayersetup);
					p->SetMode(p->VIEW);
					g_netshell->GetCurrentScreen()->AddWindow(p, TRUE);
				}
			}
			delete m;
		} else
			n = false;
	}
	if(joinedLobby && g_netfunc->GetStatus() == NETFunc::OK) {
		g_netfunc->PushChatMessage(m_messageLobbyEnter->GetString());
		
		((aui_Static *)m_controls[CONTROL_CURRENTLOBBYTEXTFIELD])->
			SetText(g_netfunc->GetSession()->GetName());
		if ( strlen( NETFunc::servername ) )
		{
			
			aui_Control *item = m_controls[ CONTROL_CURRENTSERVERTEXTFIELD ];

			MBCHAR servern[ 100 + 1 ];
			strncpy( servern, NETFunc::servername, 100 );

			if ( !item->GetTextFont() ) item->TextReloadFont();

			item->GetTextFont()->TruncateString( servern, item->Width() );

			item->SetText( servern );

			m_controls[ CONTROL_CURRENTSERVERSTATICTEXT ]->Show();
			m_controls[ CONTROL_CURRENTSERVERTEXTFIELD ]->Show();

			
			ShouldDraw(TRUE);
		}
		else
		{
			((aui_Static *)m_controls[CONTROL_CURRENTSERVERTEXTFIELD])->
				SetText( "" );

			m_controls[ CONTROL_CURRENTSERVERSTATICTEXT ]->Hide();
			m_controls[ CONTROL_CURRENTSERVERTEXTFIELD ]->Hide();
		}
		
		NETFunc::EnumPlayers(false, &lobbyKey);
		
		((ns_PlayerListBox *)m_controls[ CONTROL_PLAYERSLISTBOX ])->SetKey(&lobbyKey);
		
		((ns_ChatBox *)m_controls[ CONTROL_CHATBOX ])->SetKey(&lobbyKey);
		
		NETFunc::EnumPlayers(true, &lobbyKey);

		UpdatePlayerButtons();

		

		if ( s_dbw )
		{
			DialogBoxWindow::PopDown( s_dbw );
			s_dbw = NULL;
			s_startedLeavingAt = 0;

			
			
			((aui_Control *)((ns_ChatBox *)FindControl( CONTROL_CHATBOX ))->
				GetInputField())->SetKeyboardFocus();
		}
	}
	if(g_netfunc->GetStatus() == NETFunc::READY || (s_startedLeavingAt > 0 && time(0) > s_startedLeavingAt + k_LEAVE_LOBBY_TIMEOUT)) {
		

		if ( s_dbw )
		{
			DialogBoxWindow::PopDown( s_dbw );
			s_dbw = NULL;
			s_startedLeavingAt = 0;
		}
		g_netshell->GotoScreen( NetShell::SCREEN_PLAYERSELECT );
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE LobbyWindow::SetParent( aui_Region *region )
{
	
	if ( region ) {
		ns_ChatBox *chatbox = (ns_ChatBox *)FindControl( CONTROL_CHATBOX );
		chatbox->SetText( "" );
		chatbox->GetInputField()->SetKeyboardFocus();
	}
	return ns_Window::SetParent( region );
}



void LobbyWindow::ChangeButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_netshell->GotoScreen( NetShell::SCREEN_LOBBYCHANGE );
}



void LobbyWindow::GamesListBoxAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( g_lobbyWindow )
		g_lobbyWindow->UpdatePlayerButtons();

	switch ( action )
	{
	case AUI_LISTBOX_ACTION_SELECT:
	{
		ns_GameListBox *listbox = (ns_GameListBox *)control;
		LobbyWindow *w = (LobbyWindow *)listbox->GetParent();












		w->Update( FALSE );
		break;
	}

	case AUI_LISTBOX_ACTION_DOUBLECLICKSELECT:
	{
		aui_ListBox *listbox = (aui_ListBox *)control;
		sint32 index = listbox->ExtractDoubleClickedItem( data );
		aui_Item *item = listbox->GetItemByIndex( index );
		listbox->SelectItem( item );

		aui_Button *button = (aui_Button *)((LobbyWindow *)control->
			GetParentWindow())->FindControl( CONTROL_JOINBUTTON );
		button->GetAction()->Execute( button, AUI_BUTTON_ACTION_EXECUTE, 0 );
		break;
	}

	default:
		break;
	}
}


void LobbyWindow::JoinButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	ns_GameListBox *listbox = (ns_GameListBox *)g_lobbyWindow->
		FindControl( LobbyWindow::CONTROL_GAMESLISTBOX );
	ns_GameItem *item = (ns_GameItem *)listbox->GetSelectedItem();

	if(item) {
		ns_Game *game = item->GetNetShellObject();
		if(!game->IsMine()) {
			NETFunc::Game *g = game->GetNETFuncObject();

			if ( !g->GetPassword() || *g->GetPassword() == '\0' )
			{
				
				g_lobbyWindow->PasswordScreenDone( "" );
			}
			else
			{
				
				passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_JOIN );
			}
		}
	}
}


void LobbyWindow::PasswordScreenDone( MBCHAR *password )
{
	ns_GameListBox *listbox = (ns_GameListBox *)
		FindControl( LobbyWindow::CONTROL_GAMESLISTBOX );
	ns_GameItem *item = (ns_GameItem *)listbox->GetSelectedItem();

	if(item) {
		ns_Game *game = item->GetNetShellObject();
		if(!game->IsMine()) {
			NETFunc::Game *g = game->GetNETFuncObject();

			
			PlayerSelectWindow *psw = (PlayerSelectWindow *)g_netshell->
				FindWindow(NetShell::WINDOW_PLAYERSELECT);
			psw->GetPlayerSetup(g_netfunc->GetPlayer())->Reset();

			MBCHAR temp[ dp_PASSWORDLEN + 1 ] = "";
			if ( password )
			{
				strncpy( temp, password, dp_PASSWORDLEN );
				for ( sint32 i = 0; (unsigned) i < strlen( temp ); i++ )
				{
					
					temp[ i ] = tolower( temp[ i ] );
				}
			}

			g_playersetup.SetReadyToLaunch(false);
			if(g_netfunc->Join(g, temp) == NETFunc::OK) {
				g_gamesetup = nf_GameSetup(g);


				g_gamesetup.SetSavedId( ((uint32 *)g->GetUserField())[ 1 ] );

				AllinoneWindow *w = (AllinoneWindow *)g_netshell->
					FindWindow(NetShell::WINDOW_ALLINONE);



				if ( g_gamesetup.GetSavedId() )
					w->SetMode( w->CONTINUE_JOIN );
				else
					w->SetMode( w->JOIN );

				g_netshell->GotoScreen( NetShell::SCREEN_ALLINONE );
				w->Update();
			}
			else
			{
				passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_DENY );
			}
		}
	}
}



void LobbyWindow::CreateButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_netshell->GotoScreen( NetShell::SCREEN_STARTSELECTING );
}



void LobbyWindow::PlayersListBoxAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != AUI_LISTBOX_ACTION_SELECT ) return;

	ns_PlayerListBox *listbox = (ns_PlayerListBox *)control;

	tech_WLList<sint32>	justSelectedList;
	tech_WLList<sint32> justDeselectedList;
	sint32 index;

	listbox->WhatsChanged(justSelectedList,justDeselectedList);
	LobbyWindow *w = (LobbyWindow *)listbox->GetParent();

	w->UpdatePlayerButtons();

	ns_ChatBox *chatbox = (ns_ChatBox *)(w->FindControl(LobbyWindow::CONTROL_CHATBOX));

	aui_Switch *ms = (aui_Switch *)(w->FindControl(LobbyWindow::CONTROL_MUTESWITCH));

	ListPos position = justDeselectedList.GetHeadPosition();
	sint32 i;
	for ( i = justDeselectedList.L(); i; i-- ) {
		index = justDeselectedList.GetNext( position );
		ns_PlayerItem *item = (ns_PlayerItem *)listbox->GetItemByIndex(index);
		NETFunc::Player *player = item->GetNetShellObject()->GetNETFuncObject();
		if(chatbox->GetPlayer() && player->Equals(chatbox->GetPlayer())) {
			chatbox->SetPlayer(0);
		}
	}

	position = justSelectedList.GetHeadPosition();
	for ( i = justSelectedList.L(); i; i-- ) {
		index = justSelectedList.GetNext( position );
		ns_PlayerItem *item = (ns_PlayerItem *)listbox->GetItemByIndex(index);
		NETFunc::Player *player = item->GetNetShellObject()->GetNETFuncObject();
		chatbox->SetPlayer(player);

		if(player->IsMuted()) {

			ms->SetState(1);
		} else {

			ms->SetState(0);
		}
	}

	justSelectedList.DeleteAll();
	justDeselectedList.DeleteAll();

	aui_Switch *s;
	if(!chatbox->GetPlayer() || chatbox->GetPlayer()->IsMe()) {


		s = (aui_Switch *)(w->FindControl(LobbyWindow::CONTROL_WHISPERSWITCH));	
		s->SetState( k_PP_PUBLIC );
		ms->SetState(0);
	}

}


void LobbyWindow::UpdatePlayerButtons( void )
{
	
	ns_PlayerListBox *listbox = (ns_PlayerListBox *)
		m_controls[ CONTROL_PLAYERSLISTBOX ];
	ns_PlayerItem *item = (ns_PlayerItem *)listbox->GetSelectedItem();
	if ( item )
	{
		m_controls[ CONTROL_INFOBUTTON ]->Enable( true );

		if ( item->GetNetShellObject()->GetNETFuncObject()->IsMe() )
		{
			m_controls[ CONTROL_WHISPERSWITCH ]->Enable( false );
			m_controls[ CONTROL_MUTESWITCH ]->Enable( false );
		}
		else
		{
			m_controls[ CONTROL_WHISPERSWITCH ]->Enable( true );
			m_controls[ CONTROL_MUTESWITCH ]->Enable( true );
		}
	}
	else
	{
		m_controls[ CONTROL_INFOBUTTON ]->Enable( false );
		m_controls[ CONTROL_WHISPERSWITCH ]->Enable( false );
		m_controls[ CONTROL_MUTESWITCH ]->Enable( false );
	}

	
	ns_GameListBox *glistbox = (ns_GameListBox *)
		FindControl( LobbyWindow::CONTROL_GAMESLISTBOX );
	ns_GameItem *gitem = (ns_GameItem *)glistbox->GetSelectedItem();
	if(gitem)
	{
		m_controls[ CONTROL_JOINBUTTON ]->Enable(TRUE);
		m_controls[ CONTROL_REVIEWBUTTON ]->Enable(TRUE);
	}
	else
	{
		m_controls[ CONTROL_JOINBUTTON ]->Enable(FALSE);
		m_controls[ CONTROL_REVIEWBUTTON ]->Enable(FALSE);
	}
}



void LobbyWindow::WhisperSwitchAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON
	&&   action != (uint32)AUI_SWITCH_ACTION_OFF ) return;

	LobbyWindow *w = g_lobbyWindow;
	ns_ChatBox *chatbox = (ns_ChatBox *)(w->FindControl(LobbyWindow::CONTROL_CHATBOX));

	aui_Switch *s = (aui_Switch *)control;
	sint32 state = s->GetState();
	switch ( state )
	{
	case k_PP_PUBLIC:
		chatbox->SetWhisper(false);
		break;

	case k_PP_PRIVATE:
		if(!chatbox->GetPlayer() || chatbox->GetPlayer()->IsMe())
		{
			s->SetState(k_PP_PUBLIC);
		}
		else
		{
			chatbox->SetWhisper(true);
		}
		break;

	default:
		
		Assert( FALSE );
		break;
	}

	
	s->SetText( w->m_PPStrings->GetString( s->GetState() ) );
}



void LobbyWindow::MuteSwitchAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON
	&&   action != (uint32)AUI_SWITCH_ACTION_OFF ) return;


	LobbyWindow *w = (LobbyWindow *)g_netshell->FindWindow( NetShell::WINDOW_LOBBY );
	ns_ChatBox *chatbox = (ns_ChatBox *)(w->FindControl(LobbyWindow::CONTROL_CHATBOX));

	NETFunc::Player *p = chatbox->GetPlayer();

	switch ( action )
	{
	case AUI_SWITCH_ACTION_ON:
		if(!p || p->IsMe()) {
			aui_Switch *s = (aui_Switch *)(w->FindControl(LobbyWindow::CONTROL_MUTESWITCH));	
			s->SetState(0);
		} else
			g_netfunc->Mute(p, true);
		break;
	case AUI_SWITCH_ACTION_OFF:
		if(p)
			g_netfunc->Mute(p, false);
		break;
	default:
		break;
	}
}



void LobbyWindow::InfoButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	LobbyWindow *w = (LobbyWindow *)g_netshell->FindWindow( NetShell::WINDOW_LOBBY );
	ns_PlayerListBox *l = (ns_PlayerListBox *)(w->FindControl( LobbyWindow::CONTROL_PLAYERSLISTBOX ));

	ns_PlayerItem *item = (ns_PlayerItem *)(l->GetSelectedItem());
	if(item) {
		if(item->GetNetShellObject()->IsMine()) {
			PlayerEditWindow *p = (PlayerEditWindow *)g_netshell->FindWindow( NetShell::WINDOW_PLAYEREDIT );
			p->SetPlayerSetup(&g_playersetup);
			p->SetMode(p->EDIT);
			g_netshell->GetCurrentScreen()->AddWindow(p, TRUE);
		} else {
			NETFunc::Player *player = item->GetNetShellObject()->GetNETFuncObject();
			g_netfunc->GetPlayerSetupPacket(player);
		}
	}
}



void LobbyWindow::ReviewButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_lobbyWindow->SpitOutDetails();
}




void LobbyWindow::SpitOutDetails( void )
{
	ns_GameItem *item = (ns_GameItem *)
		((ns_GameListBox *)m_controls[ CONTROL_GAMESLISTBOX ])->
			GetSelectedItem();
	if ( !item ) return;

	NETFunc::Game *game = item->GetNetShellObject()->GetNETFuncObject();
	nf_GameSetup gamesetup(game);

	gamesetup.SetSavedId( ((uint32 *)game->GetUserField())[ 1 ] );

	bool displayedSomething = false;

	const sint32 biglen = 1024;
	static MBCHAR info[ biglen + 1 ];

	
	memset( info, 0, sizeof( info ) );

	static ns_String gameDetails( "strings.gamedetails" );
	strncat( info, gameDetails.GetString(), biglen );
	strncat( info, "\n", biglen );

	




















	

	if ( gamesetup.GetSavedId() )
	{
		static ns_String savedGame( "strings.savedgame" );
		strncat( info, savedGame.GetString(), biglen );
		strncat( info, "\n", biglen );
		displayedSomething = true;

		
		
	}
	else
	{
		static ns_String notSavedGame( "strings.notsavedgame" );
		strncat( info, notSavedGame.GetString(), biglen );
		strncat( info, "\n", biglen );
		displayedSomething = true;
	}


	if ( !displayedSomething )
	{



	}

	g_netfunc->PushChatMessage( info );
}



void LobbyWindow::BackButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_netfunc->Disconnect();


	if ( !s_dbw )
	{
		s_dbw = DialogBoxWindow::PopUp(
			"leavelobbydialogboxwindow",
			NULL );
		s_startedLeavingAt = time(0);

		
		
		
		
		
		
		

		ns_Window *window = (ns_Window *)g_netshell->FindWindow( NetShell::WINDOW_LOBBY );
		((aui_Control *)((ns_ChatBox *)window->FindControl( CONTROL_CHATBOX ))->
			GetInputField())->SetKeyboardFocus();
	}
}


void LobbyWindow::DialogBoxPopDownAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	
	g_netfunc->Disconnect();

	

	if ( s_dbw )
	{
		DialogBoxWindow::PopDown( s_dbw );
	}

	s_dbw = DialogBoxWindow::PopUp(
		"leavelobbydialogboxwindow",
		NULL );
	s_startedLeavingAt = time(0);
}
