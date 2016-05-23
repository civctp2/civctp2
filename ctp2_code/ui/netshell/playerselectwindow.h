#ifndef __PLAYERSELECTWINDOW_H__
#define __PLAYERSELECTWINDOW_H__

#include "ns_window.h"
#include "aui_action.h"
#include "ns_playersetup.h"

class PlayerSelectWindow : public ns_Window
{
public:

	PlayerSelectWindow( AUI_ERRCODE *retval );
	virtual ~PlayerSelectWindow() {}

protected:
	PlayerSelectWindow() : ns_Window() {}
	AUI_ERRCODE	InitCommon( void );
	AUI_ERRCODE CreateControls( void );

public:
	void	Update( void );
	virtual AUI_ERRCODE Idle( void );
	virtual AUI_ERRCODE SetParent( aui_Region *region );
	nf_PlayerSetup *GetPlayerSetup(NETFunc::Player *player);


	enum CONTROL
	{
		CONTROL_FIRST = 0,

		CONTROL_TITLESTATICTEXT = CONTROL_FIRST,
		CONTROL_CURRENTPLAYERSTATICTEXT,
		CONTROL_PLAYERNAMETEXTFIELD,
		CONTROL_PLAYERNAMESTATICTEXT,
		CONTROL_PLAYERNAMELISTBOX,
		CONTROL_NEWBUTTON,
		CONTROL_EDITBUTTON,
		CONTROL_DELETEBUTTON,
		CONTROL_OKBUTTON,
		CONTROL_CANCELBUTTON,
		CONTROL_LAST,
		CONTROL_MAX = CONTROL_LAST - CONTROL_FIRST
	};

protected:
    AUI_ACTION_BASIC(PlayerNameTextFieldAction);
	AUI_ACTION_BASIC(NewButtonAction);
    AUI_ACTION_BASIC(EditButtonAction);
    AUI_ACTION_BASIC(DeleteButtonAction);
    AUI_ACTION_BASIC(OKButtonAction);
    AUI_ACTION_BASIC(CancelButtonAction);
    AUI_ACTION_BASIC(PlayerListBoxAction);
};

#endif
