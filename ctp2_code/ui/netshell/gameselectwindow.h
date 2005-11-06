










#ifndef __GAMESELECTWINDOW_H__
#define __GAMESELECTWINDOW_H__


#include "ns_window.h"
#include "aui_action.h"
#include "ns_customlistbox.h"

class GameSelectWindow;
extern GameSelectWindow *g_gameSelectWindow;


class GameSelectWindow : public ns_Window
{
public:
	
	GameSelectWindow( AUI_ERRCODE *retval );
	virtual ~GameSelectWindow();

protected:
	GameSelectWindow() : ns_Window() {}
	AUI_ERRCODE	InitCommon( void );
	AUI_ERRCODE CreateControls( void );

public:
	void	Update( void );
	virtual AUI_ERRCODE Idle( void );
	virtual AUI_ERRCODE SetParent( aui_Region *region );
	nf_GameSetup *GetGameSetup(NETFunc::Session *session);

	
	void PasswordScreenDone( MBCHAR *password );

	

	enum CONTROL
	{
		CONTROL_FIRST = 0,

		CONTROL_TITLESTATICTEXT = CONTROL_FIRST,

		CONTROL_GAMENAMELISTBOX,
		CONTROL_DELETEBUTTON,
		CONTROL_OKBUTTON,
		CONTROL_CANCELBUTTON,
		CONTROL_LAST,
		CONTROL_MAX = CONTROL_LAST - CONTROL_FIRST
	};

protected:
    AUI_ACTION_BASIC(DeleteButtonAction);
	AUI_ACTION_BASIC(OKButtonAction);
	AUI_ACTION_BASIC(CancelButtonAction);
	AUI_ACTION_BASIC(GameListBoxAction);
};


class StartSelectingWindow;
extern StartSelectingWindow *g_startSelectingWindow;


class StartSelectingWindow : public ns_Window
{
public:
	
	StartSelectingWindow( AUI_ERRCODE *retval );
	virtual ~StartSelectingWindow();

protected:
	StartSelectingWindow() : ns_Window() {}
	AUI_ERRCODE	InitCommon( void );
	AUI_ERRCODE CreateControls( void );

public:
	virtual AUI_ERRCODE Idle( void );
	virtual AUI_ERRCODE SetParent( aui_Region *region );

	

	enum CONTROL
	{
		CONTROL_FIRST = 0,

		CONTROL_TITLESTATICTEXT = CONTROL_FIRST,
		CONTROL_NEWBUTTON,
		CONTROL_GAMESETUPBUTTON,
		CONTROL_SAVEDBUTTON,
		CONTROL_SCENARIOBUTTON,
		CONTROL_DELETEBUTTON,
		CONTROL_CANCELBUTTON,
		CONTROL_LAST,
		CONTROL_MAX = CONTROL_LAST - CONTROL_FIRST
	};

protected:
    AUI_ACTION_BASIC(NewButtonAction);
	AUI_ACTION_BASIC(GameSetupButtonAction);
    AUI_ACTION_BASIC(SavedButtonAction);
    AUI_ACTION_BASIC(ScenarioButtonAction);
	AUI_ACTION_BASIC(CancelButtonAction);
};


void StartSelectingLoadSaveCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie );

void StartSelectingScenarioCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie );


#endif 
