#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __MESSAGE_WINDOW_H__
#define __MESSAGE_WINDOW_H__


#include "c3_popupwindow.h"
#include "aui_textbase.h"
#include "message.h"
#include "aui_hypertextbox.h"


class aui_Static;
class aui_Button;
class Message;
class MessageIconWindow;

class MessageEyePointStandard;
class MessageEyePointDropdown;
class MessageEyePointListbox;

class MessageResponseStandard;
class MessageResponseDropdown;


class MessageMinimizeAction;
class MessageDismissAction;
class MessageOpenAction;
class MessageLibraryAction;

union MessageEyePoint {
	MessageEyePointStandard		*m_messageEyePointStandard;
	MessageEyePointDropdown		*m_messageEyePointDropdown;
	MessageEyePointListbox		*m_messageEyePointListbox;
};

union MessageResponse {
	MessageResponseStandard		*m_messageResponseStandard;
	MessageResponseDropdown		*m_messageResponseDropdown;
};


class MessageWindow : public c3_PopupWindow 
{
public:
	MessageWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		Message data,
		MessageIconWindow *icon,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );

	virtual ~MessageWindow();

	virtual AUI_ERRCODE InitCommon( Message data, MessageIconWindow *iconWindow );

	MessageIconWindow	*GetIconWindow( void ) { return m_iconWindow; }

	Message				*GetMessage( void ) { return &m_message; }

	MessageOpenAction	*GetMessageOpenAction( void );
	
	AUI_ERRCODE	ShowWindow( BOOL show );

	void SetMessageText( MBCHAR *text ) { m_messageText->SetHyperText( text ); }

	PLAYER_INDEX		GetPlayer( void ) { return m_player; }

	aui_Button	*GetGreatLibraryButton( void ) { return m_libraryIcon; }
	MessageMinimizeAction *GetMinimizeAction() { return m_minimizeAction; }
protected:
	
	void BringBorderToTop();
	AUI_ERRCODE AddBordersToUI();
	AUI_ERRCODE RemoveBordersFromUI();

	AUI_ERRCODE CreateWindowEdges( MBCHAR *ldlBlock );
	AUI_ERRCODE CreateStandardTextBox( MBCHAR *ldlBlock );
	AUI_ERRCODE CreateTurnText( MBCHAR *ldlBlock );
	AUI_ERRCODE CreateStandardMinimizeButton( MBCHAR *ldlBlock ); 
	AUI_ERRCODE CreateStandardDismissButton( MBCHAR *ldlBlock ); 
	AUI_ERRCODE CreateGreatLibraryButton( MBCHAR *ldlBlock );

	AUI_ERRCODE CreateEyePointBox( MBCHAR *ldlBlock ); 
	AUI_ERRCODE CreateStandardEyePointBox( MBCHAR *ldlBlock ); 
	AUI_ERRCODE CreateDropdownEyePointBox( MBCHAR *ldlBlock ); 
	AUI_ERRCODE CreateListboxEyePointBox( MBCHAR *ldlBlock );

	AUI_ERRCODE CreateResponses( MBCHAR *ldlBlock );
	AUI_ERRCODE CreateSelectResponses( MBCHAR *ldlBlock );
	AUI_ERRCODE CreateSubmitResponses( MBCHAR *ldlBlock );


private:
	aui_HyperTextBox						*m_messageText;
	aui_Static						*m_turnText;

	MessageIconWindow				*m_iconWindow;

	Message							m_message;

	PLAYER_INDEX					m_player;

	MessageEyePoint					m_messageEyePoint;

	MessageResponse					m_messageResponse;

	aui_Button						*m_minimizeIcon;
	aui_Button						*m_dismissIcon;
	aui_Button						*m_libraryIcon;

	aui_Static						*m_leftBar;
	aui_Static						*m_topBar;
	aui_Static						*m_rightBar;
	aui_Static						*m_bottomBar;









	MessageMinimizeAction			*m_minimizeAction;
	MessageDismissAction			*m_dismissAction;
	MessageLibraryAction			*m_libraryAction;
};

#endif 
