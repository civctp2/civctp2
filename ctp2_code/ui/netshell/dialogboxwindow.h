









 
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

	virtual ~DialogBoxWindow();

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
	DialogBoxWindow() 
	:	ns_Window		(),
	   	m_numButtons	(0),
		m_buttons		(NULL)
	{ ; };

	AUI_ERRCODE	InitCommon( void );
	AUI_ERRCODE CreateControls(
		MBCHAR *ldlBlock,
		aui_Action **actions );

	sint32		m_numButtons;
	aui_Button	**m_buttons; 

	class SafeDeleteAction : public aui_Action
	{
	public:
		SafeDeleteAction(DialogBoxWindow * dbw ) 
        :   aui_Action  (),
            m_dbw       (dbw) 
        { ; };
		virtual ~SafeDeleteAction(void) { ; };

	    virtual void	Execute
	    (
		    aui_Control	*	control,
		    uint32			action,
		    uint32			data
	    );

	protected:
		DialogBoxWindow *   m_dbw;
	};
};


#endif 
