







#pragma once
#ifndef __MESSAGEMODAL_H__
#define __MESSAGEMODAL_H__

#include "messagewindow.h"
#include "c3_PopupWindow.h"

#define k_MODAL_BUTTON_SPACING			4
#define k_MODAL_BUTTON_TEXT_PADDING			15
#define k_MODAL_BUTTON_DEFAULT_WIDTH		70


class aui_Static;
class aui_Button;
class aui_HyperTextBox;
class Message;

class ctp2_Button;

class MessageEyePointStandard;
class MessageEyePointDropdown;
class MessageEyePointListbox;


class MessageModalResponseAction;


class MessageModal : public c3_PopupWindow 
{
public:
	MessageModal(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		Message data,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_FLOATING );

	virtual ~MessageModal();

	virtual AUI_ERRCODE InitCommon( MBCHAR *ldlBlock, Message data );

	void BringBorderToTop();
	AUI_ERRCODE AddBordersToUI();
	AUI_ERRCODE RemoveBordersFromUI();

	Message				*GetMessage( void ) { return &m_message; }

protected:
	virtual MouseEventCallback MouseLGrabInside;
	virtual MouseEventCallback MouseLDragAway;

	AUI_ERRCODE CreateWindowEdges( MBCHAR *ldlBlock );
	AUI_ERRCODE CreateResponses( MBCHAR *ldlBlock );
	AUI_ERRCODE CreateStandardTextBox( MBCHAR *ldlBlock );

	
	AUI_ERRCODE CreateEyePointBox( MBCHAR *ldlBlock ); 
	AUI_ERRCODE CreateStandardEyePointBox( MBCHAR *ldlBlock ); 
	AUI_ERRCODE CreateDropdownEyePointBox( MBCHAR *ldlBlock ); 
	AUI_ERRCODE CreateListboxEyePointBox( MBCHAR *ldlBlock );

private:
	aui_HyperTextBox						*m_messageText;

	Message							m_message;

	MessageEyePoint					m_messageEyePoint;

	aui_Static						*m_leftBar;
	aui_Static						*m_rightBar;

	C3Window						*m_topBar;
	C3Window						*m_bottomBar;

	tech_WLList<ctp2_Button *>			*m_messageModalResponseButton;
	tech_WLList<MessageModalResponseAction *>	*m_messageModalResponseAction;

	POINT	m_offsetTop;
	POINT	m_offsetBottom;

};


int messagemodal_CreateModalMessage( Message data );	
int messagemodal_DestroyModalMessage( void );
int messagemodal_PrepareDestroyWindow();

#endif 
