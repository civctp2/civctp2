










#ifndef __AUI_TAB_H__
#define __AUI_TAB_H__


#include "aui_radio.h"



class aui_Tab : public aui_Radio
{
public:
	
	aui_Tab(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_Tab(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 paneWidth,
		sint32 paneHeight,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		sint32 state = 0,
		sint32 numStates = k_AUI_SWITCH_DEFAULTNUMSTATES );
	virtual ~aui_Tab();

protected:
	aui_Tab() : aui_Radio() {}
	AUI_ERRCODE InitCommon( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( sint32 paneWidth, sint32 paneHeight );

public:
	virtual sint32 SetState( sint32 state );

	aui_Control	*GetPane( void ) const { return m_pane; }

	AUI_ERRCODE	AddPaneControl( aui_Control *control );
	AUI_ERRCODE RemovePaneControl( uint32 controlId );
	aui_Control	*GetPaneControl( uint32 controlId ) const
		{ return (aui_Control *)m_pane->GetChild( controlId ); }

	virtual AUI_ERRCODE	Show( void );

protected:
	aui_Control		*m_pane;

	
	
	

	
	virtual MouseEventCallback MouseLDragOver;
	virtual MouseEventCallback MouseLDragAway;

	
	virtual MouseEventCallback MouseLGrabInside;
	virtual MouseEventCallback MouseLDropInside;
	virtual MouseEventCallback MouseLDropOutside {}

	virtual MouseEventCallback MouseRGrabInside;
};


#endif 
