









#ifndef __MESSAGE_ACTIONS_H__
#define __MESSAGE_ACTIONS_H__

#include "aui_action.h"
#include "message.h"
#include "messagewindow.h"

class c3_DropDown;

class Message;
class MessageList;
class MessageIconWindow;
class MessageWindow;
class MessageModal;



class MessageOpenAction : public aui_Action
{
public:
	MessageOpenAction( MessageIconWindow *iconWindow )
					{ m_iconWindow = iconWindow; }
	virtual ~MessageOpenAction() {}

	virtual ActionCallback Execute;

protected:
	MessageIconWindow		*m_iconWindow;
};



class MessageMinimizeAction : public aui_Action
{
public:
	MessageMinimizeAction( MessageWindow *window ) { m_window = window; }
	virtual ~MessageMinimizeAction() {}

	virtual ActionCallback Execute;

protected:
	MessageWindow			*m_window;
};



class MessageDismissUIAction : public aui_Action
{
public:
	MessageDismissUIAction( MessageWindow *window ) 
							{ m_window = window; }
	virtual ~MessageDismissUIAction() {}

	virtual ActionCallback Execute;

protected:
	MessageWindow			*m_window;
};


class MessageDismissAction : public aui_Action
{
public:
	MessageDismissAction( MessageWindow *window ) 
							{ m_window = window; }
	virtual ~MessageDismissAction() {}

	virtual ActionCallback Execute;

protected:
	MessageWindow			*m_window;
};



class MessageLibraryAction : public aui_Action
{
public:
	MessageLibraryAction( MessageWindow *window ) 
							{ m_window = window; }
	virtual ~MessageLibraryAction() {}

	virtual ActionCallback Execute;

protected:
	MessageWindow			*m_window;
};



class MessageStandardEyePointAction : public aui_Action
{
public:
	MessageStandardEyePointAction( MessageWindow *window, MessageModal *modal = NULL ) { m_window = window; 
		if ( modal ) m_modal = modal; }
	virtual ~MessageStandardEyePointAction() {}

	virtual ActionCallback Execute;

protected:
	MessageWindow					*m_window;
	MessageModal					*m_modal;
};



class MessageDropdownEyePointAction : public aui_Action
{
public:
	MessageDropdownEyePointAction( MessageWindow *window, MessageModal *modal = NULL )
				{ m_window = window; if ( modal ) m_modal = modal; }
	virtual ~MessageDropdownEyePointAction() {}
	void SetDropdown( c3_DropDown *dropdown ) { m_dropdown = dropdown; }

	virtual ActionCallback Execute;

protected:
	MessageWindow					*m_window;
	MessageModal					*m_modal;
	c3_DropDown						*m_dropdown;
};



class MessageDropdownAction : public aui_Action
{
public:
	MessageDropdownAction( MessageWindow *window, c3_DropDown *dropdown, MessageModal *modal = NULL ) 
			{ m_window = window; m_dropdown = dropdown; if ( modal ) m_modal = modal; }
	virtual ~MessageDropdownAction() {}

	virtual ActionCallback Execute;

protected:
	MessageWindow					*m_window;
	MessageModal					*m_modal;
	c3_DropDown						*m_dropdown;
};




class MessageListboxEyePointAction : public aui_Action
{
public:
	MessageListboxEyePointAction( MessageWindow *window, sint32 index,
								  sint32 change, sint32 min, sint32 max, MessageModal *modal = NULL )
				{ m_window = window;
				  m_index = index;
				  m_change = change;
				  m_min = min;
				  m_max = max; if ( modal ) m_modal = modal; }
	virtual ~MessageListboxEyePointAction() {}
	void SetIndex( sint32 index ) { m_index = index; }
	void SetAction( MessageListboxEyePointAction *action ) { m_action = action; }

	virtual ActionCallback Execute;

protected:
	MessageWindow					*m_window;
	sint32							m_index;
	sint32							m_change;
	sint32							m_min;
	sint32							m_max;
	MessageListboxEyePointAction	*m_action;
	MessageModal					*m_modal;
};



class MessageResponseUIAction : public aui_Action
{
public:

	MessageResponseUIAction( MessageWindow *window, sint32 response ) 
						{ m_window = window; m_response = response; }
	virtual ~MessageResponseUIAction() {}

	virtual ActionCallback Execute;

protected:
	MessageWindow			*m_window;
	sint32					m_response;

};



class MessageResponseAction : public aui_Action
{
public:

	MessageResponseAction( MessageWindow *window, sint32 response ) 
						{ m_window = window; m_response = response; }
	virtual ~MessageResponseAction() {}

	virtual ActionCallback Execute;

protected:
	MessageWindow			*m_window;
	sint32					m_response;

};



class MessageModalResponseAction : public aui_Action
{
public:

	MessageModalResponseAction( Message *message, sint32 response ) 
						{ m_message = message; m_response = response; }
	virtual ~MessageModalResponseAction() {}

	virtual ActionCallback Execute;

protected:
	Message					*m_message;
	sint32					m_response;

};



class MessageResponseSubmitAction : public aui_Action
{
public:

	MessageResponseSubmitAction( MessageWindow *window ) 
						{ m_window = window; }
	virtual ~MessageResponseSubmitAction() {}

	void SetDropdown( c3_DropDown *dropdown ) { m_dropdown = dropdown; }
	virtual ActionCallback Execute;

protected:
	MessageWindow			*m_window;
	c3_DropDown				*m_dropdown;
};



class MessageCleanupAction : public aui_Action
{
public:
	MessageCleanupAction( MessageIconWindow *iconWindow, PLAYER_INDEX index ) 
				{ m_iconWindow = iconWindow; m_index = index; }
	MessageCleanupAction( MessageWindow *window, PLAYER_INDEX index ) 
				{ m_iconWindow = window->GetIconWindow(); m_index = index; }
	virtual ~MessageCleanupAction() {}

	virtual ActionCallback Execute;

protected:
	PLAYER_INDEX				m_index;
	MessageIconWindow			*m_iconWindow;
};



class ChangeOffsetMessageIconButtonAction : public aui_Action
{
public:
	ChangeOffsetMessageIconButtonAction( sint32 offset, MessageList *list, 
										BOOL flag = TRUE ) 
				{ m_offset = offset; m_messagelist = list; m_offsetflag = flag; }
	virtual ~ChangeOffsetMessageIconButtonAction() {}

	virtual ActionCallback Execute;
	void SetList(MessageList *newlist) { m_messagelist = newlist; };

protected:
	sint32			m_offset;			
	BOOL			m_offsetflag;		
	MessageList		*m_messagelist;
};



class ChangeOffsetDestroyButtonAction : public aui_Action
{
public:
	ChangeOffsetDestroyButtonAction( BOOL more ) 
				{ m_more = more; }
	virtual ~ChangeOffsetDestroyButtonAction() {}

	virtual ActionCallback Execute;

protected:
	BOOL			m_more;
};


class MessageAdviceDismissAction : public aui_Action
{
public:
	MessageAdviceDismissAction() { ; }
	virtual ~MessageAdviceDismissAction() {}

	virtual ActionCallback Execute;

protected:
};



class MessageAdviceDestroyAction : public aui_Action
{
public:
	MessageAdviceDestroyAction( ) { ; }
	virtual ~MessageAdviceDestroyAction() {}

	virtual ActionCallback Execute;

protected:
};



class MessageModalDestroyAction : public aui_Action
{
public:
	MessageModalDestroyAction( ) { ; }
	virtual ~MessageModalDestroyAction() {}

	virtual ActionCallback Execute;

protected:
};




#endif 
