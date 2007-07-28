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
#include "netshell.h"

#include <algorithm>
#include "allinonewindow.h"
#include "aui_button.h"
#include "aui_screen.h"
#include "connectionselectwindow.h"
#include "ctp2_button.h"
#include "gameselectwindow.h"
#include "Globals.h"
#include "lobbywindow.h"
#include "lobbychangewindow.h"
#include "netshell_game.h"
#include "ns_customlistbox.h"
#include "ns_tribes.h"
#include "passwordscreen.h"
#include "playereditwindow.h"
#include "playerselectwindow.h"
#include "serverselectwindow.h"

extern void EnterMainMenu(void);
extern void LeaveMainMenu(void);
extern void LaunchGame(void);
extern MBCHAR g_serverName[ 100 + 1 ];


NetShell *          g_netshell          = NULL;
NETFunc *           g_netfunc           = NULL;
nf_GameSetup        g_gamesetup;
nf_PlayerSetup      g_playersetup;
nf_PlayerSetup      g_rplayersetup;



AUI_ERRCODE NetShell::Enter( uint32 flags )
{
	if ( (flags & k_NS_FLAGS_CREATE) || !g_netshell || !g_netfunc )
	{
		if ( !g_netfunc )
		{
			g_netfunc = new NETFunc();
		}

		if ( !g_netshell )
		{
			g_netshell = new NetShell();
		}
	}

	LeaveMainMenu();

	g_ui->SetBackgroundColor( RGB(0,0,0) );

	if (aui_Control * bg = g_netshell->m_bg)
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





	if (flags & k_NS_FLAGS_RETURN) 
	{
		g_netfunc->Leave();
		LobbyWindow *w = (LobbyWindow *)(g_netshell->FindWindow( NetShell::WINDOW_LOBBY ));
		g_netshell->GotoScreen( NetShell::SCREEN_LOBBY );
		w->Update();
	} 
	else if (flags & k_NS_FLAGS_CREATE3P) 
	{
		if(g_netfunc->Connect("freeze.dat") == NETFunc::OK) {
			
			if(g_netfunc->IsHost()) {
				GameSelectWindow *sw = (GameSelectWindow *)(g_netshell->FindWindow(NetShell::WINDOW_GAMESELECT));
				g_gamesetup = *sw->GetGameSetup(g_netfunc->GetSession());
			}

			AllinoneWindow *w = (AllinoneWindow *)(g_netshell->FindWindow(NetShell::WINDOW_ALLINONE));
			g_netshell->GotoScreen( SCREEN_ALLINONE );
			w->Update();
		} else {
#ifdef WIN32
			PostMessage( g_ui->TheHWND(), WM_CLOSE, 0, 0 );
#endif
		}
	} 
	else
	{
		g_netshell->GotoScreen( SCREEN_CONNECTIONSELECT );
	}

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
			g_ui->AddAction(new EnterMainMenuAction);
		} else {
			EnterMainMenu();
		}
	}

	
	if ( flags & k_NS_FLAGS_DESTROYNETFUNC )
		DestroyNETFunc();

	
	if ( flags & ( k_NS_FLAGS_DESTROYNETSHELL | k_NS_FLAGS_LAUNCH ) )
	{
		if ( safe )
		{
			g_ui->AddAction(new DestroyAction);
		}
		else
		{
			DestroyAction().Execute( NULL, 0, 0 );
		}
	}

	if ( flags & k_NS_FLAGS_LAUNCH ) 
	{
		LaunchGame();
	}
}



NetShell::NetShell()
:
    aui_Shell            (),
    m_wasMinimizing      (false),
    m_truebmp            (NULL),
    m_tribes             (NULL),
    m_wonders            (NULL),
    m_bg                 (NULL) 
{
	std::fill(m_screens, m_screens + SCREEN_MAX, (aui_Screen *) NULL);
	std::fill(m_windows, m_windows + WINDOW_MAX, (aui_Window *) NULL);

	m_truebmp = new ns_String( "strings.truebmp" );
	m_tribes  = new ns_Tribes;

	AUI_ERRCODE	errcode = AUI_ERRCODE_OK;
	m_bg      = new aui_Control(&errcode, aui_UniqueId(), "nsbackground");
	Assert( AUI_NEWOK(m_bg, errcode) );

	errcode = CreateScreens();
	Assert( AUI_SUCCESS(errcode) );

	if (!g_netshell) 
	{
		g_netshell = this;
		/// @todo Check next 4 lines
		g_nsUnits = new ns_Units;
		g_nsImprovements = new ns_Improvements;
		g_nsWonders = new ns_Wonders;
		strncpy( g_serverName, "", 100 );
	}

}



AUI_ERRCODE NetShell::CreateScreens( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	aui_Screen *screen;

	screen = new aui_Screen( &errcode, SCREEN_PLAYERSELECT );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_PLAYERSELECT ] = screen;

	{
		m_windows[ WINDOW_PLAYERSELECT ] = new PlayerSelectWindow( &errcode );
		m_screens[ SCREEN_PLAYERSELECT ]->
			AddWindow( m_windows[ WINDOW_PLAYERSELECT ] );
	}

	screen = new aui_Screen( &errcode, SCREEN_CONNECTIONSELECT );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_CONNECTIONSELECT ] = screen;

	{
		ConnectionSelectWindow * window = new ConnectionSelectWindow( &errcode );
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
		m_windows[ WINDOW_SERVERSELECT ] = new ServerSelectWindow( &errcode );
		m_screens[ SCREEN_SERVERSELECT ]->
			AddWindow( m_windows[ WINDOW_SERVERSELECT ] );
	}

	screen = new aui_Screen( &errcode, SCREEN_PLAYEREDIT );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_PLAYEREDIT ] = screen;
	{
		m_windows[ WINDOW_PLAYEREDIT ] = new PlayerEditWindow( &errcode );
		m_screens[ SCREEN_PLAYEREDIT ]->
			AddWindow( m_windows[ WINDOW_PLAYEREDIT ] );
	}

	screen = new aui_Screen( &errcode, SCREEN_LOBBY );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_LOBBY ] = screen;
	{
		LobbyWindow * window = new LobbyWindow( &errcode );
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
		m_windows[ WINDOW_LOBBYCHANGE ] = new LobbyChangeWindow( &errcode) ;
		m_screens[ SCREEN_LOBBYCHANGE ]->
			AddWindow( m_windows[ WINDOW_LOBBYCHANGE ] );
	}


	

	screen = new aui_Screen( &errcode, SCREEN_GAMESELECT );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_GAMESELECT ] = screen;
	{
		m_windows[ WINDOW_GAMESELECT ] = new GameSelectWindow( &errcode );
		m_screens[ SCREEN_GAMESELECT ]->
			AddWindow( m_windows[ WINDOW_GAMESELECT ] );
	}


	

	screen = new aui_Screen( &errcode, SCREEN_STARTSELECTING );
	Assert( AUI_NEWOK(screen,errcode) );
	if ( !AUI_NEWOK(screen,errcode) ) return errcode;
	m_screens[ SCREEN_STARTSELECTING ] = screen;
	{
		StartSelectingWindow * window = new StartSelectingWindow( &errcode );
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
		m_windows[ WINDOW_ALLINONE ] = new AllinoneWindow( &errcode );
		m_screens[ SCREEN_ALLINONE ]->
			AddWindow( m_windows[ WINDOW_ALLINONE ] );
	}

	passwordscreen_Initialize();

	return AUI_ERRCODE_OK;
}



NetShell::~NetShell()
{
	DestroyScreens();

	delete m_truebmp;
	delete m_tribes;

	if ( m_bg )
	{
		aui_Image *	mpBackgroundImage = g_ui->SetBackgroundImage(NULL);
		g_ui->UnloadImage(mpBackgroundImage);
		delete m_bg;
	}

    if (g_netshell == this)
    {
        allocated::clear(g_nsUnits);
        allocated::clear(g_nsImprovements);
        allocated::clear(g_nsWonders);

        g_netshell = NULL;
    }
}


void NetShell::DestroyScreens( void )
{
    SavePlayerSetupList();
    SaveGameSetupList();

    if (GetCurrentScreen())
    {
        GetCurrentScreen()->Hide();
    }

	int i;
	for ( i = 0; i < SCREEN_MAX; i++ )
	{
		delete m_screens[ i ];
		m_screens[i] = NULL;
	}
	
	for ( i = 0; i < WINDOW_MAX; i++ )
	{
		delete m_windows[ i ];
		m_windows[i] = NULL;
	}

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
	allocated::clear(g_netfunc);
}


aui_Screen *NetShell::FindScreen( uint32 id )
{
	Assert(id < (uint32)SCREEN_MAX);
	return (id < (uint32)SCREEN_MAX) ? m_screens[id] : NULL;
}


aui_Window *NetShell::FindWindow( uint32 id )
{
	Assert(id < (uint32)WINDOW_MAX);
	return (id < (uint32)WINDOW_MAX) ? m_windows[id] : NULL;
}


void NetShell::DestroyAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	allocated::clear(g_netshell);
}


void NetShell::MoveButton(aui_Window *window, const MBCHAR *parentBlock, const MBCHAR *regionBlock, BOOL left)
{
	ctp2_Button *button = (ctp2_Button *)aui_Ldl::GetObject(parentBlock, regionBlock);
	if (button) 
	{
		sint32 xPos = (left) ? 17 : window->Width() - button->Width() - 14;
		button->Move(xPos, window->Height() - button->Height() - 17);
	}
}
