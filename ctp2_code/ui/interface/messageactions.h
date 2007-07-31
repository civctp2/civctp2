









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
	MessageOpenAction(MessageIconWindow * iconWindow)
    :   aui_Action      (),
        m_iconWindow    (iconWindow)
    { ; };
	virtual ~MessageOpenAction(void) { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

protected:
	MessageIconWindow * m_iconWindow;
};

//----------------------------------------------------------------------------
//
// Name       : AUI_ACTION_MESSAGE_WINDOW
//
// Description: Class generation macro to define an aui_Action derived class
//              that manages a message window.
//
// Parameters : ClassName   : name of the derived class
//
// Remark(s)  : MACRO
//
//----------------------------------------------------------------------------
#define AUI_ACTION_MESSAGE_WINDOW(ClassName)    \
class ClassName : public aui_Action             \
{                                               \
public:                                         \
    ClassName(MessageWindow * window)           \
    :   aui_Action  (),                         \
        m_window    (window)                    \
    { ; };                                      \
    virtual ~ClassName(void) { ; };             \
                                                \
    virtual void    Execute                     \
    (                                           \
        aui_Control *   control,                \
        uint32          action,                 \
        uint32          data                    \
    );                                          \
protected:                                      \
    MessageWindow * m_window;                   \
}

AUI_ACTION_MESSAGE_WINDOW(MessageMinimizeAction);
AUI_ACTION_MESSAGE_WINDOW(MessageDismissAction);
AUI_ACTION_MESSAGE_WINDOW(MessageDismissUIAction);
AUI_ACTION_MESSAGE_WINDOW(MessageLibraryAction);

#undef  AUI_ACTION_MESSAGE_WINDOW

class MessageStandardEyePointAction : public aui_Action
{
public:
	MessageStandardEyePointAction
    ( 
        MessageWindow * window, 
        MessageModal *  modal = NULL 
    )
    :   aui_Action  (),
        m_window    (window),
        m_modal     (modal)
    { ; };
	virtual ~MessageStandardEyePointAction(void) { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

protected:
	MessageWindow * m_window;
	MessageModal *  m_modal;
};

class MessageDropdownEyePointAction : public MessageStandardEyePointAction
{
public:
	MessageDropdownEyePointAction
    ( 
        MessageWindow * window, 
        MessageModal *  modal       = NULL,
        c3_DropDown *   dropdown    = NULL
    )
    :   MessageStandardEyePointAction   (window, modal),
        m_dropdown                      (dropdown)
    { ; };
    virtual ~MessageDropdownEyePointAction(void) { ; };

	void SetDropdown(c3_DropDown * dropdown) { m_dropdown = dropdown; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

protected:
	c3_DropDown	*   m_dropdown;
};

class MessageDropdownAction : public aui_Action
{
public:
	MessageDropdownAction
    ( 
        MessageWindow * window, 
        c3_DropDown *   dropdown, 
        MessageModal *  modal = NULL 
    ) 
	:   aui_Action  (),
        m_window    (window),
        m_dropdown  (dropdown),
        m_modal     (modal)
    { ; };
	virtual ~MessageDropdownAction(void) { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

protected:
	MessageWindow * m_window;
	MessageModal *  m_modal;
	c3_DropDown	*   m_dropdown;
};

class MessageListboxEyePointAction : public MessageStandardEyePointAction
{
public:
	MessageListboxEyePointAction
    ( 
        MessageWindow * window, 
        sint32          index,
		sint32          change, 
        sint32          min, 
        sint32          max, 
        MessageModal *  modal = NULL 
    )
    :   MessageStandardEyePointAction   (window, modal),
        m_index                         (index),
        m_change                        (change),
        m_min                           (min),
        m_max                           (max),
        m_action                        (NULL)
    { ; }
	virtual ~MessageListboxEyePointAction(void) { ; };

	void SetIndex(sint32 index) { m_index = index; };
	void SetAction(MessageListboxEyePointAction * action ) { m_action = action; }

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

protected:
	sint32							m_index;
	sint32							m_change;
	sint32							m_min;
	sint32							m_max;
	MessageListboxEyePointAction *  m_action;
};

class MessageResponseAction : public aui_Action
{
public:
	MessageResponseAction(MessageWindow * window, sint32 response) 
    :   aui_Action  (),
        m_window    (window),
        m_response  (response)
    { ; };
	virtual ~MessageResponseAction(void) { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

protected:
	MessageWindow * m_window;
	sint32			m_response;
};

class MessageResponseUIAction : public MessageResponseAction
{
public:
	MessageResponseUIAction(MessageWindow * window, sint32 response) 
    : MessageResponseAction (window, response)
    { ; };
	virtual ~MessageResponseUIAction(void) { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);
};

class MessageModalResponseAction : public aui_Action
{
public:
	MessageModalResponseAction(Message * message, sint32 response) 
    :   aui_Action  (),
        m_message   (message),
        m_response  (response)
    { ; };
	virtual ~MessageModalResponseAction(void) { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

protected:
	Message	*   m_message;
	sint32		m_response;

};

class MessageResponseSubmitAction : public aui_Action
{
public:
	MessageResponseSubmitAction(MessageWindow * window) 
    :   aui_Action  (),
        m_window    (window)
    { ; };
	virtual ~MessageResponseSubmitAction(void) { ; };

	void SetDropdown(c3_DropDown * dropdown) { m_dropdown = dropdown; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

protected:
	MessageWindow * m_window;
	c3_DropDown	*   m_dropdown;
};

class MessageCleanupAction : public aui_Action
{
public:
	MessageCleanupAction
    ( 
        MessageIconWindow * iconWindow, 
        PLAYER_INDEX        index 
    ) 
    :   aui_Action      (),
        m_iconWindow    (iconWindow),
        m_index         (index)
    { ; };
	MessageCleanupAction
    ( 
        MessageWindow *     window, 
        PLAYER_INDEX        index 
    ) 
    :   aui_Action      (),
        m_iconWindow    (window ? window->GetIconWindow() : NULL),
        m_index         (index)
    { ; };
	virtual ~MessageCleanupAction(void) { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

protected:
	PLAYER_INDEX        m_index;
	MessageIconWindow * m_iconWindow;
};

class ChangeOffsetMessageIconButtonAction : public aui_Action
{
public:
	ChangeOffsetMessageIconButtonAction
    ( 
        sint32          offset, 
        MessageList *   list, 
		bool            flag = true
    )
    :   aui_Action      (),
        m_offset        (offset),
        m_messagelist   (list), 
        m_offsetflag    (flag)
    { ; };
	virtual ~ChangeOffsetMessageIconButtonAction(void) { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

	void SetList(MessageList * newlist) { m_messagelist = newlist; };

protected:
	sint32			m_offset;			
	bool			m_offsetflag;		
	MessageList	*   m_messagelist;
};


class ChangeOffsetDestroyButtonAction : public aui_Action
{
public:
	ChangeOffsetDestroyButtonAction(bool more) 
	:   aui_Action  (),
        m_more      (more)
    { ; };
	virtual ~ChangeOffsetDestroyButtonAction(void) { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

protected:
	bool    m_more;
};

AUI_ACTION_BASIC(MessageAdviceDismissAction);
AUI_ACTION_BASIC(MessageAdviceDestroyAction);
AUI_ACTION_BASIC(MessageModalDestroyAction);

#endif 
