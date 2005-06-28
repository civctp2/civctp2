









 
#ifndef __LOBBYWINDOW_H__
#define __LOBBYWINDOW_H__


#include "ns_window.h"
#include "aui_action.h"
#include "ns_string.h"


class aui_StringTable;
class LobbyWindow;
extern LobbyWindow *g_lobbyWindow;



#define k_PP_PUBLIC		0
#define k_PP_PRIVATE	1



class LobbyWindow : public ns_Window
{
public:
	
	LobbyWindow( AUI_ERRCODE *retval );
	virtual ~LobbyWindow();

protected:
	LobbyWindow() : ns_Window() {}
	AUI_ERRCODE	InitCommon( void );
	AUI_ERRCODE CreateControls( void );

public:
	void	Update( BOOL init = TRUE );
	virtual AUI_ERRCODE Idle( void );
	virtual AUI_ERRCODE SetParent( aui_Region *region );

	void PasswordScreenDone( MBCHAR *password );


	

	enum CONTROL
	{
		CONTROL_FIRST = 0,

		CONTROL_TITLESTATICTEXT = CONTROL_FIRST,
		CONTROL_GAMESSTATICTEXT,
		CONTROL_GAMESLISTBOX,
		CONTROL_CHATBOX,
		CONTROL_CHANGEBUTTON,
		CONTROL_JOINBUTTON,
		CONTROL_CREATEBUTTON,
		CONTROL_CURRENTLOBBYSTATICTEXT,
		CONTROL_CURRENTLOBBYTEXTFIELD,
		CONTROL_CURRENTSERVERSTATICTEXT,
		CONTROL_CURRENTSERVERTEXTFIELD,
		CONTROL_PLAYERSSTATICTEXT,
		CONTROL_PLAYERSLISTBOX,
		CONTROL_INFOBUTTON,
		CONTROL_MUTESWITCH,
		CONTROL_WHISPERSWITCH,
		CONTROL_REVIEWBUTTON,
		CONTROL_BACKBUTTON,
		CONTROL_LAST,
		CONTROL_MAX = CONTROL_LAST - CONTROL_FIRST
	};

	void SpitOutDetails( void );
	void UpdatePlayerButtons( void );

protected:
	ns_String	*m_messageLobbyEnter;

	aui_StringTable *m_PPStrings;

	bool wait;
	

	aui_Action *m_dbActionArray[ 1 ]; 

	class ChangeButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };
public: 
	class JoinButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class CreateButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class InfoButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class MuteSwitchAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class WhisperSwitchAction : public aui_Action
	{ public: virtual ActionCallback Execute; };
	friend class WhisperSwitchAction;

	class ReviewButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class BackButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class PlayersListBoxAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class DialogBoxPopDownAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class GamesListBoxAction : public aui_Action
	{ public: virtual ActionCallback Execute; };
};


#endif 
