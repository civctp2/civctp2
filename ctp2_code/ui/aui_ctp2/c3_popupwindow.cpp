//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The civilization 3 popup window
// Id           : $Id$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_control.h"

#include "pattern.h"
#include "primitives.h"

#include "c3window.h"
#include "c3_static.h"
#include "c3_button.h"
#include "ctp2_button.h"

#include "UIUtils.h"

#include "c3_popupwindow.h"
#include "keypress.h"



c3_PopupWindow::c3_PopupWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type,
	bool bevel )
	:
	C3Window(retval, id, ldlBlock, bpp, type, bevel)
{
	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



c3_PopupWindow::c3_PopupWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *pattern,
	AUI_WINDOW_TYPE type,
	bool bevel)
	:
	C3Window( retval, id, x, y, width, height, bpp, pattern, type, bevel )
{
	*retval = InitCommon();
	Assert(AUI_SUCCESS(*retval));
	if (!AUI_SUCCESS(*retval)) return;
}



AUI_ERRCODE c3_PopupWindow::InitCommon(void)
{		
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32 i;

	for ( i = 0;i < POPUP_BORDER_MAX;i++ ) {
		m_border[i] = NULL;
	}

	m_title = NULL;
	m_titleText = NULL;
	m_cancel = NULL;
	m_ok = NULL;

	
	sprintf( ldlBlock, "c3_PopupUL" );
	m_border[POPUP_BORDER_UL] = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_UL], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_UL], errcode) ) return errcode;

	m_border[POPUP_BORDER_UL]->Move( 0, 0 );

	
	sprintf( ldlBlock, "c3_PopupUR" );
	m_border[POPUP_BORDER_UR] = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_UR], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_UR], errcode) ) return errcode;

	m_border[POPUP_BORDER_UR]->Move( m_width - m_border[POPUP_BORDER_UR]->Width(), 0 );

	
	sprintf( ldlBlock, "c3_PopupLL" );
	m_border[POPUP_BORDER_LL] = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_LL], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_LL], errcode) ) return errcode;

	m_border[POPUP_BORDER_LL]->Move( 0, m_height - m_border[POPUP_BORDER_LL]->Height() );

	
	sprintf( ldlBlock, "c3_PopupLR" );
	m_border[POPUP_BORDER_LR] = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_LR], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_LR], errcode) ) return errcode;

	m_border[POPUP_BORDER_LR]->Move( m_width - m_border[POPUP_BORDER_LR]->Width(), 
		m_height - m_border[POPUP_BORDER_LR]->Height() );

	sint32 cornerHeight = m_border[POPUP_BORDER_UL]->Height();
	sint32 cornerWidth = m_border[POPUP_BORDER_UL]->Width();

	
	sprintf( ldlBlock, "c3_PopupLeft" );
	m_border[POPUP_BORDER_LEFT] = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_LEFT], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_LEFT], errcode) ) return errcode;

	m_border[POPUP_BORDER_LEFT]->Move( 0, cornerHeight );
	m_border[POPUP_BORDER_LEFT]->Resize( m_border[POPUP_BORDER_LEFT]->Width(), m_height - cornerHeight * 2 );

	
	sprintf( ldlBlock, "c3_PopupTop" );
	m_border[POPUP_BORDER_TOP] = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_TOP], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_TOP], errcode) ) return errcode;

	m_border[POPUP_BORDER_TOP]->Move( cornerWidth, 0 );
	m_border[POPUP_BORDER_TOP]->Resize( m_width - cornerWidth * 2, m_border[POPUP_BORDER_TOP]->Height() );

	
	sprintf( ldlBlock, "c3_PopupRight" );
	m_border[POPUP_BORDER_RIGHT] = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_RIGHT], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_RIGHT], errcode) ) return errcode;

	m_border[POPUP_BORDER_RIGHT]->Move( m_width - m_border[POPUP_BORDER_RIGHT]->Width(), cornerHeight );
	m_border[POPUP_BORDER_RIGHT]->Resize( m_border[POPUP_BORDER_RIGHT]->Width(), m_height - cornerHeight * 2 );

	
	sprintf( ldlBlock, "c3_PopupBottom" );
	m_border[POPUP_BORDER_BOTTOM] = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_BOTTOM], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_BOTTOM], errcode) ) return errcode;

	m_border[POPUP_BORDER_BOTTOM]->Move( cornerWidth, m_height - m_border[POPUP_BORDER_BOTTOM]->Height() );
	m_border[POPUP_BORDER_BOTTOM]->Resize( m_width - cornerWidth * 2, m_border[POPUP_BORDER_BOTTOM]->Height() );

	for ( i = POPUP_BORDER_UL;i < POPUP_BORDER_MAX;i++ ) {
		m_border[i]->IgnoreEvents( TRUE );
		AddControl( m_border[i] );
	}

	return C3Window::InitCommon();
}


AUI_ERRCODE c3_PopupWindow::Resize( sint32 width, sint32 height )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	errcode = C3Window::Resize( width, height );

	
	m_border[POPUP_BORDER_UL]->Move( 0, 0 );

	
	m_border[POPUP_BORDER_UR]->Move( m_width - m_border[POPUP_BORDER_UR]->Width(), 0 );

	
	m_border[POPUP_BORDER_LL]->Move( 0, m_height - m_border[POPUP_BORDER_LL]->Height() );

	
	m_border[POPUP_BORDER_LR]->Move( m_width - m_border[POPUP_BORDER_LR]->Width(), 
		m_height - m_border[POPUP_BORDER_LR]->Height() );

	sint32 cornerHeight = m_border[POPUP_BORDER_UL]->Height();
	sint32 cornerWidth = m_border[POPUP_BORDER_UL]->Width();

	
	m_border[POPUP_BORDER_LEFT]->Move( 0, cornerHeight );
	m_border[POPUP_BORDER_LEFT]->Resize( m_border[POPUP_BORDER_LEFT]->Width(), m_height - cornerHeight * 2 );

	
	m_border[POPUP_BORDER_TOP]->Move( cornerWidth, 0 );
	m_border[POPUP_BORDER_TOP]->Resize( m_width - cornerWidth * 2, m_border[POPUP_BORDER_TOP]->Height() );

	
	m_border[POPUP_BORDER_RIGHT]->Move( m_width - m_border[POPUP_BORDER_RIGHT]->Width(), cornerHeight );
	m_border[POPUP_BORDER_RIGHT]->Resize( m_border[POPUP_BORDER_RIGHT]->Width(), m_height - cornerHeight * 2 );

	
	m_border[POPUP_BORDER_BOTTOM]->Move( cornerWidth, m_height - m_border[POPUP_BORDER_BOTTOM]->Height() );
	m_border[POPUP_BORDER_BOTTOM]->Resize( m_width - cornerWidth * 2, m_border[POPUP_BORDER_BOTTOM]->Height() );

	return errcode;
}



c3_PopupWindow::~c3_PopupWindow( void )
{
	sint32 i;

	for ( i = 0;i < POPUP_BORDER_MAX;i++ ) {
		RemoveControl( m_border[i] );
	}

	RemoveControl( m_title );
	RemoveControl( m_titleText );
	RemoveControl( m_cancel );
	RemoveControl( m_ok );
}



AUI_ERRCODE c3_PopupWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_width, m_height };

	if (m_pattern)
		m_pattern->Draw( m_surface, &rect );

	


	
	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;
}



sint32 c3_PopupWindow::AddTitle( MBCHAR *titleBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if ( m_title ) return 1;

	sprintf( ldlBlock, "c3_PopupTitle" );
	m_title = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
	TestControl( m_title );

	if ( !titleBlock ) {
		sprintf( ldlBlock, "c3_PopupTitle.c3_PopupTitleText" );
		m_titleText = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
		TestControl( m_titleText );
	}
	else {
		m_titleText = new c3_Static( &errcode, aui_UniqueId(), titleBlock );
		TestControl( m_titleText );
	}

	m_title->AddSubControl( m_titleText );

	m_title->Move( (m_width - m_title->Width()) / 2, 0 );

	
	InsertChild( (aui_Region *)m_title, 0 );
	m_title->SetParentWindow( this );

	m_title->SetBlindness( TRUE );
	return 1;
}



sint32 c3_PopupWindow::AddCancel(
	void (*actionFunc)( aui_Control *, uint32, uint32, void *),
	void *cookie,
	MBCHAR *buttonBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if ( m_cancel ) return 1;

	sprintf( ldlBlock, buttonBlock );
	m_cancel = new c3_Button( &errcode, aui_UniqueId(), ldlBlock, actionFunc, cookie );
	TestControl( m_cancel );

	m_cancel->Move( 17, m_height - m_cancel->Height() - 17);

	
	InsertChild( (aui_Region *)m_cancel, 0 );
	m_cancel->SetParentWindow( this );

	return 1;
}



sint32 c3_PopupWindow::AddOk(
	void (*actionFunc)( aui_Control *, uint32, uint32, void *),
	void *cookie,
	MBCHAR *buttonBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if ( m_ok ) return 1;

	sprintf( ldlBlock, buttonBlock );
	m_ok = new ctp2_Button( &errcode, aui_UniqueId(), ldlBlock, actionFunc, cookie );
	TestControl( m_ok );

	m_ok->Move( m_width - m_ok->Width() - 17, m_height - m_ok->Height() -17);

	
	InsertChild( (aui_Region *)m_ok, 0 );
	m_ok->SetParentWindow( this );

	return 1;
}



sint32 c3_PopupWindow::AddClose(
	void (*actionFunc)( aui_Control *, uint32, uint32, void *),
	void *cookie,
	MBCHAR *buttonBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if ( m_ok ) return 1;

	sprintf( ldlBlock, buttonBlock );
	m_ok = new ctp2_Button( &errcode, aui_UniqueId(), ldlBlock, actionFunc, cookie );
	TestControl( m_ok );

	m_ok->Move( m_width - m_ok->Width()- 17, m_height - m_ok->Height() - 17);

	
	InsertChild( (aui_Region *)m_ok, 0 );
	m_ok->SetParentWindow( this );

	return 1;
}



sint32 c3_PopupWindow::AddYes(
	void (*actionFunc)( aui_Control *, uint32, uint32, void *),
	void *cookie,
	MBCHAR *buttonBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if ( m_ok ) return 1;

	sprintf( ldlBlock, buttonBlock );
	m_ok = new ctp2_Button( &errcode, aui_UniqueId(), ldlBlock, actionFunc, cookie );
	TestControl( m_ok );

	m_ok->Move( m_width - m_ok->Width(), m_height - m_ok->Height() );

	
	InsertChild( (aui_Region *)m_ok, 0 );
	m_ok->SetParentWindow( this );

	return 1;
}



sint32 c3_PopupWindow::AddNo(
	void (*actionFunc)( aui_Control *, uint32, uint32, void *),
	void *cookie,
	MBCHAR *buttonBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if ( m_cancel ) return 1;

	sprintf( ldlBlock, buttonBlock );
	m_cancel = new c3_Button( &errcode, aui_UniqueId(), ldlBlock, actionFunc, cookie );
	TestControl( m_cancel );

	m_cancel->Move( 0, m_height - m_cancel->Height() );

	
	InsertChild( (aui_Region *)m_cancel, 0 );
	m_cancel->SetParentWindow( this );

	return 1;
}

void c3_PopupWindow::kh_Close()
{
	if(m_cancel) {
		m_cancel->GetActionFunc()(m_cancel, AUI_BUTTON_ACTION_EXECUTE, 0, m_cancel->GetCookie());
	} else if(m_ok) {
		m_ok->GetActionFunc()(m_ok, AUI_BUTTON_ACTION_EXECUTE, 0, m_ok->GetCookie());
	}
}
