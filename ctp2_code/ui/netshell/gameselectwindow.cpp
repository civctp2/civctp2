//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Game type selection UI
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
// - Default values for new multiplayer game are now taken from profile, like:
//   - World size
//   - World shape
//   - World type wet/dry
//   - World type warm/cold
//   - World type ocean/land
//   - World type island/continent
//   - World type homo/deverse
//   - World type goodcount
// - Default end age is set to last age in database.
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
#include "loadsavewindow.h"



#include "netshell.h"
#include "ns_customlistbox.h"
#include "ns_civlistbox.h"
#include "passwordscreen.h"




#include "gameselectwindow.h"
#include "allinonewindow.h"
#include "playerselectwindow.h"
#include "passwordscreen.h"

#include "netshell_game.h"

#include "scenariowindow.h"


#include "spnewgamewindow.h" 

extern LoadSaveWindow *g_loadsaveWindow;

#include "profileDB.h"
#include "AgeRecord.h"


GameSelectWindow *g_gameSelectWindow = NULL;
StartSelectingWindow *g_startSelectingWindow = NULL;


GameSelectWindow::GameSelectWindow(
	AUI_ERRCODE *retval )
	:
	ns_Window(
		retval,
		aui_UniqueId(),
		"gameselectwindow",
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



AUI_ERRCODE GameSelectWindow::InitCommon( void )
{
	g_gameSelectWindow = this;

	m_controls = new aui_Control *[ m_numControls = CONTROL_MAX ];
	Assert( m_controls != NULL );
	if ( !m_controls ) return AUI_ERRCODE_MEMALLOCFAILED;
	memset( m_controls, 0, m_numControls * sizeof( aui_Control *) );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE GameSelectWindow::CreateControls( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;


	

	aui_Control *control;









	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"gameselectwindow.titlestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_TITLESTATICTEXT ] = control;

	control = new ns_GameSetupListBox(
		&errcode,
		aui_UniqueId(),
		"gameselectwindow.gamenamelistbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_GAMENAMELISTBOX ] = control;









	control = spNew_ctp2_Button(
		&errcode,
		"gameselectwindow",
		"deletebutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_DELETEBUTTON ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"gameselectwindow.okbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_OKBUTTON ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"gameselectwindow.cancelbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CANCELBUTTON ] = control;


	

	aui_Ldl::SetupHeirarchyFromRoot( "gameselectwindow" );


	

	aui_Action *action;






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

	action = new GameListBoxAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_GAMENAMELISTBOX ]->SetAction( action );


	

	
	((aui_ListBox *)m_controls[ CONTROL_GAMENAMELISTBOX ])->
		SetForceSelect( TRUE );

	Update();


	return AUI_ERRCODE_OK;
}


GameSelectWindow::~GameSelectWindow()
{
	if (this == g_gameSelectWindow)
	{
		g_gameSelectWindow = NULL;
	}
}



nf_GameSetup *GameSelectWindow::GetGameSetup(NETFunc::Session *session) {
	ns_GameSetupListBox *l = (ns_GameSetupListBox *)(FindControl( GameSelectWindow ::CONTROL_GAMENAMELISTBOX ));
	ns_GameSetupListBox::iterator i;
	nf_GameSetup *s;

	for(i = l->begin(); i != l->end(); i++) {
		s = (*i);
		if(strcmp(session->GetName(), s->GetName()) == 0)
			return s;
	}

	NETFunc::Game game = NETFunc::Game(session);
	s = new nf_GameSetup(&game);
	l->InsertItem(s);

	return s;
}


void GameSelectWindow::Update(void)
{
	ns_GameSetupListBox *listbox = (ns_GameSetupListBox *)
		(FindControl( GameSelectWindow::CONTROL_GAMENAMELISTBOX ));
	ns_GameSetupItem *item = (ns_GameSetupItem *)listbox->GetSelectedItem();
	c3_Button *b_ok =
		(c3_Button *)(FindControl( GameSelectWindow::CONTROL_OKBUTTON ));
	c3_Button *b_del =
		(c3_Button *)(FindControl( GameSelectWindow::CONTROL_DELETEBUTTON ));

	if(item) {
		b_ok->Enable(TRUE);
		b_del->Enable(TRUE);
	} else {
		b_ok->Enable(FALSE);
		b_del->Enable(FALSE);
	}
}


AUI_ERRCODE GameSelectWindow::Idle( void )
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


void GameSelectWindow::GameListBoxAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	switch ( action )
	{
	case AUI_LISTBOX_ACTION_SELECT:
		((GameSelectWindow *)control->GetParent())->Update();
		break;

	case AUI_LISTBOX_ACTION_DOUBLECLICKSELECT:
	{
		aui_ListBox *listbox = (aui_ListBox *)control;
		sint32 index = listbox->ExtractDoubleClickedItem( data );
		aui_Item *item = listbox->GetItemByIndex( index );
		listbox->SelectItem( item );

		c3_Button *button = (c3_Button *)((GameSelectWindow *)control->
			GetParentWindow())->FindControl( CONTROL_OKBUTTON );
		button->GetAction()->Execute( button, AUI_BUTTON_ACTION_EXECUTE, 0 );
		break;
	}

	default:
		break;
	}
}


AUI_ERRCODE GameSelectWindow::SetParent( aui_Region *region )
{


















	return ns_Window::SetParent( region );
}



void GameSelectWindow::DeleteButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	GameSelectWindow *w = (GameSelectWindow *)control->GetParentWindow();
	ns_GameSetupListBox *listbox = (ns_GameSetupListBox *)w->
		FindControl( GameSelectWindow ::CONTROL_GAMENAMELISTBOX );
	ns_GameSetupItem *item = (ns_GameSetupItem *)listbox->GetSelectedItem();

	if(item) {
		nf_GameSetup *game = item->GetNetShellObject()->GetNETFuncObject();
		listbox->DeleteItem(game);
		w->Update();
	}
}



void GameSelectWindow::OKButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_ASK );
}



uint32 TellEricAboutThisBug( aui_Window *w )
{
	uint32 id = 0;
	if ( w ) id = w->Id();
	return id;
}


void GameSelectWindow::PasswordScreenDone( MBCHAR *password )
{
	
	AllinoneWindow *w = g_allinoneWindow;
	AllinoneWindow::Mode mode = w->JOIN; 

	
	TellEricAboutThisBug( g_loadsaveWindow );

	
	if ( g_loadsaveWindow && g_ui->GetChild( g_loadsaveWindow->Id() ) )
	{
		
		
		SaveInfo *saveInfo = g_loadsaveWindow->GetSaveInfo();
		Assert( saveInfo != NULL );
		if ( saveInfo )
		{
			loadsavescreen_removeMyWindow( AUI_BUTTON_ACTION_EXECUTE );

			
			g_gamesetup = saveInfo->gameSetup;

			NETFunc::Session *s = (NETFunc::Session *)&g_gamesetup;

			dp_session_t *sess =
				(dp_session_t *)((uint8*)s + sizeof(NETFunc::Key));

			if(sess->sessionType == 0) {
				sess->sessionType = GAMEID;
				char name[ dp_SNAMELEN + 1 ];
				ns_String format( "strings.newgame" );

				char truncname[ dp_PNAMELEN + 1 ];
				strcpy( truncname, g_playersetup.GetName() );

				sint32 trunclen = dp_SNAMELEN -
					( strlen( truncname ) + ( strlen( format.GetString() ) - 2  ) );

				if ( trunclen < 0 )
				{
					memset( truncname, 0, sizeof( truncname ) );
					strncpy(
						truncname,
						g_playersetup.GetName(),
						strlen( truncname ) + trunclen );
				}

				sprintf( name, format.GetString(), truncname );

				strcpy( sess->sessionName, name );
			}

			mode = w->CONTINUE_CREATE;

			switch ( g_loadsaveWindow->GetType() )
			{
			case LSS_LOAD_GAME:
			case LSS_LOAD_MP:
				w->SetScenarioGame( FALSE );

				g_gamesetup.SetSavedId( 1 );
				break;








			default:
				
				Assert( FALSE );
				break;
			}
		}
	}
	
	else
	{
		ns_GameSetupListBox *listbox = (ns_GameSetupListBox *)
			FindControl( GameSelectWindow::CONTROL_GAMENAMELISTBOX );
		ns_GameSetupItem *item = (ns_GameSetupItem *)listbox->GetSelectedItem();

		if(item) {
			
			((aui_ListBox *)listbox)->RemoveItem( item->Id() );
			((aui_ListBox *)listbox)->InsertItem( item, 0 );
			listbox->SelectItem( (sint32)0 );

			g_gamesetup = *item->GetNetShellObject()->GetNETFuncObject();

			mode = w->CREATE;
			g_gamesetup.SetSavedId( 0 );
		}
	}

	
	if ( mode != w->JOIN )
	{
		MBCHAR temp[ dp_PASSWORDLEN + 1 ] = "";
		if ( password )
		{
			strncpy( temp, password, dp_PASSWORDLEN );
			for ( size_t i = 0; i < strlen( temp ); i++ )
			{
				temp[ i ] = tolower( temp[ i ] );
			}
		}

		g_gamesetup.SetPassword( temp );
		g_gamesetup.SetSize( k_NS_MAX_HUMANS );
		g_gamesetup.SetClosed( false );
		g_gamesetup.SetSyncLaunch( true );

		PlayerSelectWindow *psw = (PlayerSelectWindow *)g_netshell->
			FindWindow(NetShell::WINDOW_PLAYERSELECT);
		psw->GetPlayerSetup(g_netfunc->GetPlayer())->Reset();

		g_playersetup.SetReadyToLaunch(false);
		if(g_netfunc->Create(&g_gamesetup) == NETFunc::OK) {
			w->SetMode(mode);
			g_netshell->GotoScreen( NetShell::SCREEN_ALLINONE );
			w->Update();
		}
	}
}



void GameSelectWindow::CancelButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_netshell->GotoScreen( NetShell::SCREEN_STARTSELECTING );
}




StartSelectingWindow::StartSelectingWindow(
	AUI_ERRCODE *retval )
	:
	ns_Window(
		retval,
		aui_UniqueId(),
		"startselectingwindow",
		0,
		AUI_WINDOW_TYPE_STANDARD )
{
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateControls();
}



AUI_ERRCODE StartSelectingWindow::InitCommon( void )
{
	g_startSelectingWindow = this;

	m_controls = new aui_Control *[ m_numControls = CONTROL_MAX ];
	Assert( m_controls != NULL );
	if ( !m_controls ) return AUI_ERRCODE_MEMALLOCFAILED;
	memset( m_controls, 0, m_numControls * sizeof( aui_Control *) );

	loadsavescreen_Initialize( StartSelectingLoadSaveCallback );


	return AUI_ERRCODE_OK;
}



AUI_ERRCODE StartSelectingWindow::CreateControls( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;


	

	aui_Control *control;









	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"startselectingwindow.titlestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_TITLESTATICTEXT ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"startselectingwindow",
		"newbutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_NEWBUTTON ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"startselectingwindow",
		"gamesetupbutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_GAMESETUPBUTTON ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"startselectingwindow",
		"savedbutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_SAVEDBUTTON ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"startselectingwindow",
		"scenariobutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_SCENARIOBUTTON ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"startselectingwindow.cancelbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CANCELBUTTON ] = control;


	

	aui_Ldl::SetupHeirarchyFromRoot( "startselectingwindow" );


	

	aui_Action *action;






	action = new NewButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_NEWBUTTON ]->SetAction( action );

	action = new GameSetupButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_GAMESETUPBUTTON ]->SetAction( action );

	action = new SavedButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_SAVEDBUTTON ]->SetAction( action );

	action = new ScenarioButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_SCENARIOBUTTON ]->SetAction( action );

	action = new CancelButtonAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;
	m_controls[ CONTROL_CANCELBUTTON ]->SetAction( action );


	


	return AUI_ERRCODE_OK;
}



StartSelectingWindow::~StartSelectingWindow()
{
	loadsavescreen_Cleanup();

	if (this == g_startSelectingWindow)
	{
		g_startSelectingWindow = NULL;
	}
}



AUI_ERRCODE StartSelectingWindow::Idle( void )
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



AUI_ERRCODE StartSelectingWindow::SetParent( aui_Region *region )
{
	return ns_Window::SetParent( region );
}



void StartSelectingWindow::NewButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_allinoneWindow->SetScenarioGame(FALSE);

	GameSelectWindow *w = g_gameSelectWindow;
	ns_GameSetupListBox *listbox = (ns_GameSetupListBox *)w->
		FindControl( GameSelectWindow::CONTROL_GAMENAMELISTBOX );

	nf_GameSetup *s = new nf_GameSetup;
	if ( s )
	{
		char name[ dp_SNAMELEN + 1 ];
		ns_String format( "strings.newgame" );

		char truncname[ dp_PNAMELEN + 1 ];
		strcpy( truncname, g_playersetup.GetName() );

		sint32 trunclen = dp_SNAMELEN -
			( strlen( truncname ) + ( strlen( format.GetString() ) - 2  ) );

		if ( trunclen < 0 )
		{
			memset( truncname, 0, sizeof( truncname ) );
			strncpy(
				truncname,
				g_playersetup.GetName(),
				strlen( truncname ) + trunclen );
		}

		sprintf( name, format.GetString(), truncname );

		
		char test[ dp_SNAMELEN + 1 ];
		strncpy( test, name, dp_SNAMELEN );
		sint32 num = 2;
		while ( 1 )
		{
			sint32 i;
			for ( i = 0; i < listbox->NumItems(); i++ )
			{
				NETFunc::GameSetup *game = (NETFunc::GameSetup *)
					((ns_GameSetupItem *)listbox->GetItemByIndex( i ))->
					GetNetShellObject()->GetNETFuncObject();

				char *existing = game->GetName();
				if ( strnicmp( test, existing, dp_SNAMELEN ) == 0 )
					break;
			}

			if ( i == listbox->NumItems() )
			{
				strncpy( name, test, dp_SNAMELEN );
				break;
			}

			sprintf( test, "%s %d", name, num++ );
		}

		s->SetName( name );
		//Special rules
		s->SetBloodlust(!g_theProfileDB->IsAlienEndGameOn());
		s->SetPollution(g_theProfileDB->IsPollutionRule()); 

		//Ages
		s->SetStartAge(0);
		s->SetEndAge(g_theAgeDB->NumRecords()-1);
		//World size and shape
		s->SetMapSize(g_theProfileDB->GetMapSize());
		s->SetWorldShape(g_theProfileDB->GetWorldShape());
		//World types
		s->SetWorldType1(g_theProfileDB->GetWetDry());
		s->SetWorldType2(g_theProfileDB->GetWarmCold());
		s->SetWorldType3(g_theProfileDB->GetOceanLand());
		s->SetWorldType4(g_theProfileDB->GetIslandContinent());
		s->SetWorldType5(g_theProfileDB->GetHomoDiverse());
		s->SetWorldType6(g_theProfileDB->GetGoodCount());

		//Level of difficuilties
		s->SetDifficulty1(g_theProfileDB->GetDifficulty());
		s->SetDifficulty2(g_theProfileDB->GetRiskLevel());
		listbox->InsertItem( s );
		listbox->SelectItem(listbox->FindItem(s));

		
		c3_Button *button = (c3_Button *)w->FindControl( w->CONTROL_OKBUTTON );
		button->GetAction()->Execute( button, AUI_BUTTON_ACTION_EXECUTE, 0 );
	}
}



void StartSelectingWindow::GameSetupButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_netshell->GotoScreen( NetShell::SCREEN_GAMESELECT );
}



void StartSelectingWindow::SavedButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	uint32 type = LSS_LOAD_MP;

	NETFunc::TransportSetup *t = g_netfunc->GetTransport();
	if ( t->GetType() == NETFunc::Transport::UNKNOWN )
	{
		if ( ((FakeTransport *)t)->GetSubType() == FakeTransport::EMAIL )
		{
			
			

		}
	}

	
	loadsavescreen_Initialize( StartSelectingLoadSaveCallback );
	loadsavescreen_displayMyWindow( type );
}


void gameselectwindow_scenarioExitCallback(aui_Control *control, 
										   uint32 action, 
										   uint32 data, 
										   void *cookie )
{
	
	
	GameSelectWindow *w = g_gameSelectWindow;
	ns_GameSetupListBox *listbox = (ns_GameSetupListBox *)w->
		FindControl( GameSelectWindow::CONTROL_GAMENAMELISTBOX );

	nf_GameSetup *s = new nf_GameSetup;
	if ( s )
	{
		char name[ dp_SNAMELEN + 1 ];
		ns_String format( "strings.newgame" );

		char truncname[ dp_PNAMELEN + 1 ];
		strcpy( truncname, g_playersetup.GetName() );

		sint32 trunclen = dp_SNAMELEN -
			( strlen( truncname ) + ( strlen( format.GetString() ) - 2  ) );

		if ( trunclen < 0 )
		{
			memset( truncname, 0, sizeof( truncname ) );
			strncpy(
				truncname,
				g_playersetup.GetName(),
				strlen( truncname ) + trunclen );
		}

		sprintf( name, format.GetString(), truncname );

		
		char test[ dp_SNAMELEN + 1 ];
		strncpy( test, name, dp_SNAMELEN );
		sint32 num = 2;
		while ( 1 )
		{
			sint32 i;
			for ( i = 0; i < listbox->NumItems(); i++ )
			{
				NETFunc::GameSetup *game = (NETFunc::GameSetup *)
					((ns_GameSetupItem *)listbox->GetItemByIndex( i ))->
					GetNetShellObject()->GetNETFuncObject();

				char *existing = game->GetName();
				if ( strnicmp( test, existing, dp_SNAMELEN ) == 0 )
					break;
			}

			if ( i == listbox->NumItems() )
			{
				strncpy( name, test, dp_SNAMELEN );
				break;
			}

			sprintf( test, "%s %d", name, num++ );
		}

		s->SetName( name );
		listbox->InsertItem( s );
		listbox->SelectItem(listbox->FindItem(s));

		
		c3_Button *button = (c3_Button *)w->FindControl( w->CONTROL_OKBUTTON );
		button->GetAction()->Execute( button, AUI_BUTTON_ACTION_EXECUTE, 0 );
	}
	g_allinoneWindow->SetScenarioGame(TRUE);
	passwordscreen_displayMyWindow(PASSWORDSCREEN_MODE_ASK);
}


void StartSelectingWindow::ScenarioButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	

	
	

	
	
	scenarioscreen_displayMyWindow();
	scenarioscreen_SetExitCallback(gameselectwindow_scenarioExitCallback);
}



void StartSelectingWindow::CancelButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_netshell->GotoScreen( NetShell::SCREEN_LOBBY );
}




void StartSelectingLoadSaveCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	
	if ( g_loadsaveWindow->GetSaveInfo() )
		passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_ASK );
}

















