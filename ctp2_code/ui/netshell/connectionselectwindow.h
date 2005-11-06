










#ifndef __CONNECTIONSELECTWINDOW_H__
#define __CONNECTIONSELECTWINDOW_H__


#include "ns_window.h"
#include "aui_action.h"


class aui_StringTable;



class ConnectionSelectWindow : public ns_Window
{
public:
	
	ConnectionSelectWindow( AUI_ERRCODE *retval );
	virtual ~ConnectionSelectWindow();

protected:
	ConnectionSelectWindow() : ns_Window() {}
	AUI_ERRCODE	InitCommon( void );
	AUI_ERRCODE CreateControls( void );

public:
	void	Update( void );
	virtual	AUI_ERRCODE Idle( void );

	

	enum CONTROL
	{
		CONTROL_FIRST = 0,

		CONTROL_TITLESTATICTEXT = CONTROL_FIRST,
		CONTROL_CONNECTIONTYPESTATICTEXT,
		CONTROL_CONNECTIONTYPELISTBOX,
		CONTROL_CONNECTIONDESCRIPTIONSTATICTEXT,
		CONTROL_CONNECTIONDESCRIPTIONTEXTFIELD,
		CONTROL_OKBUTTON,
		CONTROL_CANCELBUTTON,
		CONTROL_DISCLAIMERTEXT,
		CONTROL_LAST,
		CONTROL_MAX = CONTROL_LAST - CONTROL_FIRST
	};

protected:
	AUI_ACTION_BASIC(OKButtonAction);
    AUI_ACTION_BASIC(CancelButtonAction);
    AUI_ACTION_BASIC(ConnectionListBoxAction);
};


#endif 
