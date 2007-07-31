










#ifndef __SERVERSELECTWINDOW_H__
#define __SERVERSELECTWINDOW_H__


#include "ns_window.h"
#include "aui_action.h"



class ServerSelectWindow : public ns_Window
{
public:
	
	ServerSelectWindow( AUI_ERRCODE *retval );
	virtual ~ServerSelectWindow();

protected:
	ServerSelectWindow() : ns_Window() {}
	AUI_ERRCODE	InitCommon( void );
	AUI_ERRCODE CreateControls( void );

public:
	void	Update( bool wait = false );
	virtual AUI_ERRCODE Idle( void );

	

	enum CONTROL
	{
		CONTROL_FIRST = 0,

		CONTROL_TITLESTATICTEXT = CONTROL_FIRST,

		CONTROL_SELECTSERVERLISTBOX,
		CONTROL_OKBUTTON,
		CONTROL_CANCELBUTTON,
		CONTROL_LAST,
		CONTROL_MAX = CONTROL_LAST - CONTROL_FIRST
	};

protected:
	

	aui_Action *m_dbActionArray[ 1 ]; 

	class OKButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class CancelButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class ServerListBoxAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class DialogBoxPopDownAction : public aui_Action
	{ public: virtual ActionCallback Execute; };
};


#endif 
