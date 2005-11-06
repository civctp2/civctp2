










#ifndef __NETSHELL_H__
#define __NETSHELL_H__


#include "aui_shell.h"
#include "aui_action.h"
#include "aui_ui.h"
#include "netfunc.h"
#include "ns_gamesetup.h"
#include "ns_playersetup.h"
#include "ns_string.h"

class aui_Screen;
class aui_Window;
class NetShell;

class ns_Tribes;


#define k_NS_FLAGS_CREATE			0x00000001
#define k_NS_FLAGS_DESTROYNETSHELL	0x00000002
#define k_NS_FLAGS_DESTROYNETFUNC	0x00000004
#define k_NS_FLAGS_DESTROY			(k_NS_FLAGS_DESTROYNETSHELL|k_NS_FLAGS_DESTROYNETFUNC)
#define k_NS_FLAGS_LAUNCH			0x00000008
#define k_NS_FLAGS_CREATE3P			0x00000010
#define k_NS_FLAGS_RETURN			0x00000020
#define k_NS_FLAGS_MAINMENU			0x00000040


extern NetShell	*g_netshell;
extern NETFunc	*g_netfunc;

extern nf_GameSetup g_gamesetup;
extern nf_PlayerSetup g_playersetup;
extern nf_PlayerSetup g_rplayersetup;



#define k_PACKET_DELAY 2000


#define k_NETCHUNKSIZE (dpio_MAXLEN_UNRELIABLE - 8)


enum CustomCode
{
	
	
	CUSTOMCODE_REQUESTTRIBE = dppt_MAKE(ns_PACKET_INITIALBYTE, 0),

	
	
	CUSTOMCODE_REQUESTDENIED = dppt_MAKE(ns_PACKET_INITIALBYTE, 1),

	
	
	CUSTOMCODE_STARTDOWNLOADMAP = dppt_MAKE(ns_PACKET_INITIALBYTE, 2),

	
	CUSTOMCODE_CONTINUEDOWNLOADMAP = dppt_MAKE(ns_PACKET_INITIALBYTE, 3)
};



class NetShell : public aui_Shell
{
public:
	
	NetShell(
		AUI_ERRCODE *retval );
	virtual ~NetShell();

protected:
	NetShell() : aui_Shell() {}
	AUI_ERRCODE	InitCommon( void );
	AUI_ERRCODE	CreateScreens( void );
	void		DestroyScreens( void );
	static void	DestroyNETFunc( void );

public:
	enum SCREEN
	{
		SCREEN_FIRST = 0,
		SCREEN_CONNECTIONSELECT = SCREEN_FIRST,
		SCREEN_SERVERSELECT,
		SCREEN_PLAYERSELECT,
		SCREEN_PLAYEREDIT,
		SCREEN_LOBBY,
		SCREEN_LOBBYCHANGE,
		SCREEN_STARTSELECTING,
		SCREEN_GAMESELECT,
		SCREEN_ALLINONE,
		SCREEN_LAST,
		SCREEN_MAX = SCREEN_LAST - SCREEN_FIRST
	};

	enum WINDOW
	{
		WINDOW_FIRST = 0,
		WINDOW_CONNECTIONSELECT = WINDOW_FIRST,
		WINDOW_SERVERSELECT,
		WINDOW_PLAYERSELECT,
		WINDOW_PLAYEREDIT,


		WINDOW_LOBBY,
		WINDOW_LOBBYCHANGE,
		WINDOW_STARTSELECTING,
		WINDOW_GAMESELECT,
		WINDOW_ALLINONE,
		WINDOW_LAST,
		WINDOW_MAX = WINDOW_LAST - WINDOW_FIRST
	};

	virtual aui_Screen *FindScreen( uint32 id );
	aui_Window *FindWindow( uint32 id );

	
	static AUI_ERRCODE	Enter( uint32 flags );
	static void			Leave( uint32 flags, BOOL safe = FALSE );

	static void SavePlayerSetupList( void );
	static void SaveGameSetupList( void );
	static void SaveAiSetupList( void );

	BOOL &WasMinimizing( void ) { return m_wasMinimizing; }

	AUI_ACTION_BASIC(DestroyAction);

	MBCHAR *GetTrueBmp( void ) { return m_truebmp->GetString(); }

protected:
	aui_Screen *m_screens[ SCREEN_MAX ];
	aui_Window *m_windows[ WINDOW_MAX ];

	BOOL m_wasMinimizing; 

	ns_String *m_truebmp;


	ns_Tribes *m_tribes;
	ns_Wonders *m_wonders;

	aui_Control *m_bg; 

	
	
	void NetShell::MoveButton(aui_Window *window, const MBCHAR *parentBlock, const MBCHAR *regionBlock, BOOL left);

};



void EnterMainMenu( void );
void LeaveMainMenu( void );
void LaunchGame( void );


#endif 
