









#pragma once
#ifndef __MESSAGEICONWINDOW_H__
#define __MESSAGEICONWINDOW_H__

#include "aui_window.h"
#include "messagewindow.h"


#define k_MESSAGE_ICON_PICTURE_WARNING		"ups001.tga"
#define k_MESSAGE_ICON_PICTURE_DIPLOMATIC	"ups002.tga"
#define k_MESSAGE_ICON_PICTURE_TRADE		"ups003.tga"
#define k_MESSAGE_ICON_PICTURE_MILITARY		"ups004.tga"
#define k_MESSAGE_ICON_PICTURE_KNOWLEDGE	"ups005.tga"
#define k_MESSAGE_ICON_PICTURE_ZOOMEYE		"ups006.tga"
#define k_MESSAGE_ICON_PICTURE_LIBRARY		"ups007.tga"
#define k_MESSAGE_ICON_PICTURE_FORWARD		"ups008.tga"
#define k_MESSAGE_ICON_PICTURE_BACKWARD		"ups009.tga"
#define k_MESSAGE_ICON_PICTURE_WARNING_SELECTED		"TEMPUps666.tga"
#define k_MESSAGE_ICON_PICTURE_DIPLOMATIC_SELECTED	"TEMPUps666.tga"
#define k_MESSAGE_ICON_PICTURE_TRADE_SELECTED		"TEMPUps666.tga"
#define k_MESSAGE_ICON_PICTURE_MILITARY_SELECTED	"TEMPUps666.tga"
#define k_MESSAGE_ICON_PICTURE_KNOWLEDGE_SELECTED	"TEMPUps666.tga"
#define k_MESSAGE_ICON_PICTURE_WONDER			"ups002.tga"
#define k_MESSAGE_ICON_PICTURE_WONDER_SELECTED	"TEMPUps666.tga"

class MessageIconButton;
class Message;
class MessageList;
class MessageWindow;

class MessageOpenAction;


class MessageIconWindow : public aui_Window
{
public:
	
	MessageIconWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		Message data,
		sint32 bpp,
		MessageList *messagelist,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );

	virtual AUI_ERRCODE	InitCommon( Message *data, MBCHAR *ldlBlock, MessageList *messagelist );

	virtual ~MessageIconWindow( void );

	MessageWindow	*GetWindow()	{ return m_messageWindow; }
	void SetWindow( MessageWindow *window ) { m_messageWindow = window; }

	MessageIconButton		*GetIconButton( void ) { return m_icon; }

	Message					*GetMessage( void ) 
				{ return ( m_messageWindow ? m_messageWindow->GetMessage() : NULL ); }


	BOOL			IsMoving( void ) { return m_isMoving; }

	uint32			GetTicks( void ) { return m_lastticks; }

	void			SetMoving( BOOL moving ) { m_isMoving = moving; }

	
	AUI_ERRCODE SetupAnimation( uint32 position ); 
	void	StopAnimation( void );

	void SetTipWindowText( MBCHAR *text );
	void ChangeIcon( const MBCHAR *image, const MBCHAR *image2 = NULL );

	void SetCurrentIconButton( MessageIconButton *iconButton );
	MessageIconWindow *GetCurrentMessageIconWindow( void ) { return m_currentIconWindow; }

	MessageOpenAction		*GetMessageOpenAction( void ) { return m_messageOpenAction; }

	void SetNext( MessageIconWindow *iconWindow ) { m_next = iconWindow; }
	void SetPrev( MessageIconWindow *iconWindow ) { m_prev = iconWindow; }
	BOOL CheckShowWindow( void );

	void	SetAcceleration( sint32 dy ) { m_acceleration = dy; }
	sint32	Acceleration( void ) { return m_acceleration; }

	BOOL	Animating( void ) { return m_isMoving; }

	
	virtual AUI_ERRCODE Idle( void );
	
	
	


protected:
	MessageOpenAction		*m_messageOpenAction;
	MessageIconButton		*m_icon;
	MessageWindow			*m_messageWindow;
	static MessageIconWindow	*m_currentIconWindow;
	MessageList				*m_messageList;

	
	BOOL					m_isMoving; 
	sint32					m_currentY;
	sint32					m_targetY;
	sint32					m_acceleration;
	uint32					m_lastticks;	
	MessageIconWindow		*m_prev;
	MessageIconWindow		*m_next;

};
#else

class MessageIconWindow;

#endif 
