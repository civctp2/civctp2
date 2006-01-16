//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : 
// Id           : $Id:$
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
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_surface.h"
#include "aui_window.h"
#include "textbutton.h"
#include "picturebutton.h"
#include "c3listbox.h"
#include "c3ui.h"
#include "pattern.h"

#include "primitives.h"

#include "c3dropdown.h"


extern C3UI *g_c3ui;


C3DropDown::C3DropDown(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	sint32 buttonSize,
	sint32 windowSize,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_DropDown(retval, id,x, y, width, height, buttonSize, windowSize, ActionFunc, cookie),
	PatternBase(pattern)
{

	*retval = aui_SoundBase::InitCommon( (MBCHAR **)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateComponents();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

C3DropDown::C3DropDown(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie)
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_DropDown(retval, id, ldlBlock, ActionFunc, cookie),
	PatternBase(ldlBlock, NULL)
{
	*retval = aui_SoundBase::InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateComponents();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

AUI_ERRCODE C3DropDown::CreateComponents( void )
{
	AUI_ERRCODE errcode;

	m_button = new PictureButton(
		&errcode,
		aui_UniqueId(),
		0, 0, 15, 8,
		"upba0103.tga", // Isn't in database
		"upba0103.tga",
		DropDownButtonActionCallback, this );
	Assert( AUI_NEWOK(m_button,errcode) );
	if ( !AUI_NEWOK(m_button,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_listBox = new C3ListBox(
		&errcode,
		aui_UniqueId(),
		0, 0, 0, 0,
		m_pattern->GetFilename(),
		DropDownListBoxActionCallback, this );
	Assert( AUI_NEWOK(m_listBox,errcode) );
	if ( !AUI_NEWOK(m_listBox,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	
	m_listBox->SetForceSelect( TRUE );

	errcode;
	m_listBoxWindow = new aui_Window(
		&errcode,
		aui_UniqueId(),
		0, 0, m_width + m_buttonSize, m_windowSize,
		16,
		AUI_WINDOW_TYPE_POPUP );
	Assert( AUI_NEWOK(m_listBoxWindow,errcode) );
	if ( !AUI_NEWOK(m_listBoxWindow,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_button );

	m_listBoxWindow->AddChild( m_listBox );

	RepositionButton();
	RepositionListBoxWindow();

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE C3DropDown::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
		if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width + m_buttonSize, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( m_pattern ) m_pattern->Draw( surface, &rect );

	primitives_BevelRect16( surface, &rect, 2, 0, 16, 16 );


	DrawSelectedItem( surface, 2, 2 );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE C3DropDown::RepositionButton( void )
{
	m_button->Move( m_width, 5 );

	return AUI_ERRCODE_OK;
}
