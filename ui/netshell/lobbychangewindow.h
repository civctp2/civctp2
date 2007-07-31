









 
#ifndef __LOBBYCHANGEWINDOW_H__
#define __LOBBYCHANGEWINDOW_H__


#include "ns_window.h"
#include "aui_action.h"



class LobbyChangeWindow : public ns_Window
{
public:
	
	LobbyChangeWindow( AUI_ERRCODE *retval );
	virtual ~LobbyChangeWindow() {}

protected:
	LobbyChangeWindow() : ns_Window() {}
	AUI_ERRCODE	InitCommon( void );
	AUI_ERRCODE CreateControls( void );

public:
	void	Update( void );
	virtual AUI_ERRCODE Idle( void );
	virtual AUI_ERRCODE SetParent( aui_Region *region );

	

	enum CONTROL
	{
		CONTROL_FIRST = 0,
		CONTROL_TITLESTATICTEXT = CONTROL_FIRST,
		CONTROL_CURRENTLOBBYSTATICTEXT,
		CONTROL_CURRENTLOBBYTEXTFIELD,

		CONTROL_LOBBIESLISTBOX,
		CONTROL_OKBUTTON,
		CONTROL_CANCELBUTTON,
		CONTROL_LAST,
		CONTROL_MAX = CONTROL_LAST - CONTROL_FIRST
	};

protected:
	

	class OKButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class CancelButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class LobbyListBoxAction : public aui_Action
	{ public: virtual ActionCallback Execute; };
};


#endif 
