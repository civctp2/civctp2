//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface advice pop-up
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
// - #pragma once commented out.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

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
    virtual void	MouseLGrabInside(aui_MouseEvent * data);
    virtual void	MouseLDragAway(aui_MouseEvent * data);

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
