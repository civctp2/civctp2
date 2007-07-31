









 
#ifndef __DIALOGBOXWINDOW_H__
#define __DIALOGBOXWINDOW_H__


#include "ns_window.h"
#include "aui_action.h"

class aui_Button;



class DialogBoxWindow : public ns_Window
{

public: 
	
	DialogBoxWindow(
		AUI_ERRCODE *retval,
		MBCHAR *ldlBlock,
		aui_Action **actions );

public:
	virtual ~DialogBoxWindow();

protected:
	DialogBoxWindow() : ns_Window() {}
	AUI_ERRCODE	InitCommon( void );
	AUI_ERRCODE CreateControls(
		MBCHAR *ldlBlock,
		aui_Action **actions );

public:
	
	static DialogBoxWindow *PopUp(
		MBCHAR *ldlBlock,
		aui_Action **actions );

	
	static void PopDown(
		DialogBoxWindow *dbw,
		aui_Button *button = NULL );

	

	enum CONTROL
	{
		CONTROL_FIRST = 0,
		CONTROL_TITLESTATICTEXT = CONTROL_FIRST,
		CONTROL_TITLEBOX,

		CONTROL_LEFTTOPCORNER,
		CONTROL_RIGHTTOPCORNER,
		CONTROL_LEFTBOTTOMCORNER,
		CONTROL_RIGHTBOTTOMCORNER,

		CONTROL_LEFTEDGE,
		CONTROL_TOPEDGE,
		CONTROL_RIGHTEDGE,
		CONTROL_BOTTOMEDGE,

		CONTROL_DESCRIPTIONSTATICTEXT,
		CONTROL_PROGRESSBAR,

		CONTROL_LAST,
		CONTROL_MAX = CONTROL_LAST - CONTROL_FIRST
	};

	aui_Button *GetButton( sint32 i ) const { return m_buttons[ i ]; }

protected:
	sint32		m_numButtons;
	aui_Button	**m_buttons; 

	

	class SafeDeleteAction : public aui_Action
	{
	public:
		SafeDeleteAction( DialogBoxWindow *dbw ) : m_dbw( dbw ) {}
		virtual ~SafeDeleteAction() {}
		virtual ActionCallback Execute;
	protected:
		DialogBoxWindow *m_dbw;
	};
};


#endif 
