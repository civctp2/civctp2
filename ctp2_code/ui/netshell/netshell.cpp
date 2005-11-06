//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Network (multiplayer) user interface
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
// - Prevented memory leaks and debug exit popups.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "aui_screen.h"
#include "aui_button.h"


#include "connectionselectwindow.h"
#include "serverselectwindow.h"
#include "playerselectwindow.h"
#include "playereditwindow.h"


#include "lobbywindow.h"
#include "lobbychangewindow.h"
#include "gameselectwindow.h"
#include "allinonewindow.h"
#include "ns_customlistbox.h"




#include "netshell.h"
#include "netshell_game.h"

#include "ns_tribes.h"
#include "passwordscreen.h"


#include "ctp2_button.h"

NetShell *g_netshell = NULL;
NETFunc *g_netfunc = NULL;
nf_GameSetup g_gamesetup;
nf_PlayerSetup g_playersetup;
nf_PlayerSetup g_rplayersetup;

extern MBCHAR g_serverName[ 100 + 1 ];


AUI_ERRCODE NetShell::Enter( uint32 flags )
{
	if ( (flags & k_NS_FLAGS_CREATE) || !g_netshell || !g_netfunc )
	{
		if ( !g_netfunc )
		{
			
			g_netfunc = new NETFunc();
			Assert( g_netfunc != NULL );
			if ( !g_netfunc ) return AUI_ERRCODE_MEMALLOCFAILED;
		}

		if ( !g_netshell )
		{
			
			AUI_ERRCODE errcode = AUI_ERRCODE_OK;

			g_netshell = new NetShell( &errcode );
			Assert( AUI_NEWOK(g_netshell,errcode) );
			if ( !AUI_NEWOK(g_netshell,errcode) ) return errcode;
		}
	}

	LeaveMainMenu();


	g_ui->SetBackgroundColor( RGB(0,0,0) );
	aui_Control *bg = g_netshell->m_bg;
	if ( bg )
	{
		
		aui_Image * image    = g_ui->LoadImage(bg->GetImage()->GetFilename());
		aui_Image *	oldImage = g_ui->SetBackgroundImage
			(image,
			 (g_ui->Width() - image->TheSurface()->Width()) / 2,
		     (g_ui->Height() - image->TheSurface()->Height()) / 2
			);
		if (oldImage)
		{
			g_ui->UnloadImage(oldImage);
		}
	}

	
	g_ui->Invalidate();





	if(flags & k_NS_FLAGS_RETURN) {

		g_netfunc->Leave();
		LobbyWindow *w = (LobbyWindow *)(g_netshell->FindWindow( NetShell::WINDOW_LOBBY ));
		g_netshell->GotoScreen( NetShell::SCREEN_LOBBY );
		w->Update();
	} else

	if(flags & k_NS_FLAGS_CREATE3P) {
		if(g_netfunc->Connect("freeze.dat") == NETFunc::OK) {
			
			if(g_netfunc->IsHost()) {
				GameSelectWindow *sw = (GameSelectWindow *)(g_netshell->FindWindow(NetShell::WINDOW_GAMESELECT));
				g_gamesetup = *sw->GetGameSetup(g_netfunc->GetSession());


				AllinoneWindow *w = (AllinoneWindow *)(g_netshell->FindWindow(NetShell::WINDOW_ALLINONE));
				g_netshell->GotoScreen( SCREEN_ALLINONE );
				w->Update();
			} else {
				AllinoneWindow *w = (AllinoneWindow *)(g_netshell->FindWindow(NetShell::WINDOW_ALLINONE));
				g_netshell->GotoScreen( SCREEN_ALLINONE );
				w->Update();
			}
		} else {
#ifdef WIN32
			PostMessage( g_ui->TheHWND(), WM_CLOSE, 0, 0 );
#endif
		}
	} else
	
		g_netshell->GotoScreen( SCREEN_CONNECTIONSELECT );

	return AUI_ERRCODE_OK;
}

class EnterMainMenuAction : public aui_Action
{
public: 
	virtual void	Execute
	(                             
		aui_Control	*	control,
		uint32			action, 
		uint32			data   
	)
    {
        EnterMainMenu();
    };
};


void NetShell::Leave( uint32 flags, BOOL safe )
{
	if ( g_netshell )
	{
		
		g_ui->Draw();
		g_ui->SetBackgroundColor( k_AUI_UI_NOCOLOR );
		aui_Image *prev = g_ui->SetBackgroundImage( NULL );
		g_ui->UnloadImage(prev);



		g_netshell->LeaveCurrentScreen();
	}

	if ( flags & k_NS_FLAGS_MAINMENU ) {
		if(safe) {
			aui_Action *action = new EnterMainMenuAction;
			g_ui->AddAction(action);
		} else {
			EnterMainMenu();
		}
	}

	
	if ( flags & k_NS_FLAGS_DESTROYNETFUNC )
		DestroyNETFunc();

	
	if ( flags & ( k_NS_FLAGS_DESTROYNETSHELL | k_NS_FLAGS_LAUNCH ) )
	{
		aui_Action *action = new DestroyAction;
		if ( safe )
		{
			g_ui->AddAction( action );
			
		}
		else
		{
			action->Execute( NULL, 0, 0 );
			delete action;
		}
	}

	
	if ( flags & k_NS_FLAGS_LAUNCH ) {
		
		LaunchGame();
	}
}



NetShell::NetShell(
	AUI_ERRCODE *retval )
	:
	aui_Shell( retval )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateScreens();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE NetShell::InitCommon( void )
{
	if ( !g_netshell ) g_netshell = this;

	memset( m_screens, 0, sizeof( m_screens ) );
	memset( m_windows, 0, sizeof( m_windows ) );

	m_wasMinimizing = FALSE;

	
	m_truebmp = new ns_String( "strings.truebmp" );
	Assert( m_truebmp != NULL );
	if ( !m_truebmp ) return AUI_ERRCODE_MEMALLOCFAILED;


	m_tribes = new ns_Tribes;
	g_nsUnits = new ns_Units;
	g_nsImprovements = new ns_Improvements;
	g_nsWonders = new ns_Wonders;

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	m_bg = new aui_Control(
		&errcode,
		aui_UniqueId(),
		"nsbackground" );
	Assert( AUI_NEWOK(m_bg,errcode) );
	if ( !AUI_NEWOK(m_bg,errcode) ) return errcode;

	
	strncpy( g_serverName, "", 100 );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE NetShell::CreateScreens( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	aui_Screen *screen;
	aui_Window *window;


	

	screen = new aui_Screen( &errcode, SCREEN_PLAYERSELECT );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_PLAYERSELECT ] = screen;

	{
		

		window = new PlayerSelectWindow( &errcode );
		Assert( AUI_NEWOK(window,errcode) );
		if ( !AUI_NEWOK(window,errcode) ) return errcode;
		m_windows[ WINDOW_PLAYERSELECT ] = window;

		

		m_screens[ SCREEN_PLAYERSELECT ]->
			AddWindow( m_windows[ WINDOW_PLAYERSELECT ] );
	}


	

	screen = new aui_Screen( &errcode, SCREEN_CONNECTIONSELECT );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_CONNECTIONSELECT ] = screen;

	{
		

		window = new ConnectionSelectWindow( &errcode );
		Assert( AUI_NEWOK(window,errcode) );
		if ( !AUI_NEWOK(window,errcode) ) return errcode;
		m_windows[ WINDOW_CONNECTIONSELECT ] = window;

		
		
		
		MoveButton(window, "connectionselectwindow", "cancelbutton", true);
		MoveButton(window, "connectionselectwindow", "okbutton", false);


		
		m_screens[ SCREEN_CONNECTIONSELECT ]->
			AddWindow( m_windows[ WINDOW_CONNECTIONSELECT ] );
	}


	

	screen = new aui_Screen( &errcode, SCREEN_SERVERSELECT );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_SERVERSELECT ] = screen;

	{
		

		window = new ServerSelectWindow( &errcode );
		Assert( AUI_NEWOK(window,errcode) );
		if ( !AUI_NEWOK(window,errcode) ) return errcode;
		m_windows[ WINDOW_SERVERSELECT ] = window;

		
		



		

		m_screens[ SCREEN_SERVERSELECT ]->
			AddWindow( m_windows[ WINDOW_SERVERSELECT ] );
	}


	

	screen = new aui_Screen( &errcode, SCREEN_PLAYEREDIT );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_PLAYEREDIT ] = screen;

	{
		

		window = new PlayerEditWindow( &errcode );
		Assert( AUI_NEWOK(window,errcode) );
		if ( !AUI_NEWOK(window,errcode) ) return errcode;
		m_windows[ WINDOW_PLAYEREDIT ] = window;

		
		



		

		m_screens[ SCREEN_PLAYEREDIT ]->
			AddWindow( m_windows[ WINDOW_PLAYEREDIT ] );
	}


	

	screen = new aui_Screen( &errcode, SCREEN_LOBBY );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_LOBBY ] = screen;

	{
		

		window = new LobbyWindow( &errcode );
		Assert( AUI_NEWOK(window,errcode) );
		if ( !AUI_NEWOK(window,errcode) ) return errcode;
		m_windows[ WINDOW_LOBBY ] = window;

		
		
		MoveButton(window, "lobbywindow", "backbutton", true);
		MoveButton(window, "lobbywindow", "closebutton", false);

		

		m_screens[ SCREEN_LOBBY ]->
			AddWindow( m_windows[ WINDOW_LOBBY ] );
	}


	

	screen = new aui_Screen( &errcode, SCREEN_LOBBYCHANGE );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_LOBBYCHANGE ] = screen;

	{
		

		window = new LobbyChangeWindow( &errcode );
		Assert( AUI_NEWOK(window,errcode) );
		if ( !AUI_NEWOK(window,errcode) ) return errcode;
		m_windows[ WINDOW_LOBBYCHANGE ] = window;

		
		



		

		m_screens[ SCREEN_LOBBYCHANGE ]->
			AddWindow( m_windows[ WINDOW_LOBBYCHANGE ] );
	}


	

	screen = new aui_Screen( &errcode, SCREEN_GAMESELECT );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_GAMESELECT ] = screen;

	{
		

		window = new GameSelectWindow( &errcode );
		Assert( AUI_NEWOK(window,errcode) );
		if ( !AUI_NEWOK(window,errcode) ) return errcode;
		m_windows[ WINDOW_GAMESELECT ] = window;

		
		



		

		m_screens[ SCREEN_GAMESELECT ]->
			AddWindow( m_windows[ WINDOW_GAMESELECT ] );
	}


	

	screen = new aui_Screen( &errcode, SCREEN_STARTSELECTING );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_STARTSELECTING ] = screen;

	{
		

		window = new StartSelectingWindow( &errcode );
		Assert( AUI_NEWOK(window,errcode) );
		if ( !AUI_NEWOK(window,errcode) ) return errcode;
		m_windows[ WINDOW_STARTSELECTING ] = window;

		
		
		MoveButton(window, "startselectingwindow", "cancelbutton", true);


		

		m_screens[ SCREEN_STARTSELECTING ]->
			AddWindow( m_windows[ WINDOW_STARTSELECTING ] );
	}


	

	screen = new aui_Screen( &errcode, SCREEN_ALLINONE );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_ALLINONE ] = screen;

	{
		

		window = new AllinoneWindow( &errcode );
		Assert( AUI_NEWOK(window,errcode) );
		if ( !AUI_NEWOK(window,errcode) ) return errcode;
		m_windows[ WINDOW_ALLINONE ] = window;

		
		



		

		m_screens[ SCREEN_ALLINONE ]->
			AddWindow( m_windows[ WINDOW_ALLINONE ] );
	}

	












	passwordscreen_Initialize();


	return AUI_ERRCODE_OK;
}



NetShell::~NetShell()
{
	DestroyScreens();


	if ( m_truebmp )
	{
		delete m_truebmp;
		m_truebmp = NULL;
	}







	if ( m_tribes )
	{
		delete m_tribes;
		m_tribes = NULL;
	}

	if ( g_nsUnits )
	{
		delete g_nsUnits;
		g_nsUnits = NULL;
	}

	if ( g_nsImprovements )
	{
		delete g_nsImprovements;
		g_nsImprovements = NULL;
	}

	if ( g_nsWonders )
	{
		delete g_nsWonders;
		g_nsWonders = NULL;
	}

	if ( m_bg )
	{
		aui_Image *	mpBackgroundImage = g_ui->SetBackgroundImage(NULL);
		g_ui->UnloadImage(mpBackgroundImage);
		delete m_bg;
	}
}



void NetShell::DestroyScreens( void )
{
	
	SavePlayerSetupList();
	SaveGameSetupList();


	sint32 i;
	for ( i = 0; i < (sint32)SCREEN_MAX; i++ )
		if ( m_screens[ i ] ) delete m_screens[ i ];
	memset( m_screens, 0, sizeof( m_screens ) );

	
	for ( i = 0; i < (sint32)WINDOW_MAX; i++ )
		if ( m_windows[ i ] ) delete m_windows[ i ];
	memset( m_windows, 0, sizeof( m_windows ) );

	passwordscreen_Cleanup();
}



void NetShell::SavePlayerSetupList( void )
{
	
	PlayerSelectWindow *pw = (PlayerSelectWindow *)g_netshell->FindWindow( NetShell::WINDOW_PLAYERSELECT );
	ns_PlayerSetupListBox *pl = (ns_PlayerSetupListBox *)(pw->FindControl( PlayerSelectWindow::CONTROL_PLAYERNAMELISTBOX ));
	pl->Save();
}



void NetShell::SaveGameSetupList( void )
{
	
	GameSelectWindow *gw = (GameSelectWindow *)g_netshell->FindWindow( NetShell::WINDOW_GAMESELECT );
	ns_GameSetupListBox *gl = (ns_GameSetupListBox *)(gw->FindControl( GameSelectWindow::CONTROL_GAMENAMELISTBOX ));
	gl->Save();
}



void NetShell::SaveAiSetupList( void )
{






}



void NetShell::DestroyNETFunc( void )
{
	

	if(g_netfunc) {












		delete g_netfunc;
		g_netfunc = NULL;
	}
}















aui_Screen *NetShell::FindScreen( uint32 id )
{
	Assert( id < (uint32)SCREEN_MAX );
	if ( id >= (uint32)SCREEN_MAX ) return NULL;
	return m_screens[ id ];
}



aui_Window *NetShell::FindWindow( uint32 id )
{
	Assert( id < (uint32)WINDOW_MAX );
	if ( id >= (uint32)WINDOW_MAX ) return NULL;
	return m_windows[ id ];
}

















void NetShell::DestroyAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( g_netshell )
	{
		aui_Screen *curScreen = g_netshell->GetCurrentScreen();
		if ( curScreen )
			curScreen->Hide();

		delete g_netshell;
		g_netshell = NULL;
	}
}


void NetShell::MoveButton(aui_Window *window, const MBCHAR *parentBlock, const MBCHAR *regionBlock, BOOL left)
{
	ctp2_Button *button = (ctp2_Button *)aui_Ldl::GetObject(parentBlock, regionBlock);
	if (button) {
		if (left)
			button->Move( 17, window->Height() - button->Height() - 17);
		else
			button->Move( window->Width() - button->Width() - 14, window->Height() - button->Height() - 17);
	}
}
