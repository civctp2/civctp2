







#pragma once
#ifndef __MESSAGEADVICE_H__
#define __MESSAGEADVICE_H__


class C3ListBox;

class MessageAdviceDismissAction;


class MessageAdvice : public C3Window 
{
public:
	MessageAdvice(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_FLOATING );

	virtual ~MessageAdvice();

	virtual AUI_ERRCODE InitCommon( MBCHAR *ldlBlock );

	void BringBorderToTop();
	AUI_ERRCODE AddBordersToUI();
	AUI_ERRCODE RemoveBordersFromUI();

	AUI_ERRCODE AppendText( MBCHAR *text );

protected:
	virtual MouseEventCallback MouseLGrabInside;
	virtual MouseEventCallback MouseLDragAway;

	AUI_ERRCODE CreateWindowEdges( MBCHAR *ldlBlock );
	AUI_ERRCODE CreateTextBox( MBCHAR *ldlBlock );
	AUI_ERRCODE CreateDismissButton( MBCHAR *ldlBlock );

private:
	aui_Static						*m_leftBar;
	aui_Static						*m_rightBar;
	C3Window						*m_topBar;
	C3Window						*m_bottomBar;
	POINT	m_offsetTop;
	POINT	m_offsetBottom;

	aui_Button						*m_dismissButton;
	MessageAdviceDismissAction		*m_dismissAction;
	
	C3ListBox						*m_listBox;

};


int messageadvice_AddText( MBCHAR *text );	
int messageadvice_DestroyWindow( void );

#endif 
