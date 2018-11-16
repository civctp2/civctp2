//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The civilization 3 button
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
#include "aui_ui.h"
#include "aui_action.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_gamespecific.h"

#include "c3ui.h"
#include "c3_button.h"
#include "c3textfield.h"
#include "patternbase.h"
#include "pattern.h"
#include "primitives.h"
#include "colorset.h"

#include "SlicEngine.h"

extern C3UI			*g_c3ui;
extern SlicEngine	*g_slicEngine;
extern ColorSet		*g_colorSet;

c3_Button::c3_Button(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_Button( retval, id, ldlBlock, ActionFunc, cookie ),
	PatternBase(ldlBlock, NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


c3_Button::c3_Button(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_Button( retval, id, x, y, width, height, ActionFunc, cookie ),
	PatternBase(pattern)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon(k_C3_BUTTON_DEFAULT_BEVELWIDTH);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE c3_Button::InitCommonLdl( MBCHAR *ldlBlock )
{
	sint32		bevelWidth=k_C3_BUTTON_DEFAULT_BEVELWIDTH,
				bevelType=0;
	aui_Ldl		*theLdl = g_c3ui->GetLdl();

	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );

	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	if (block->GetAttributeType( k_C3_BUTTON_LDL_BEVELWIDTH) == ATTRIBUTE_TYPE_INT) {
		bevelWidth = block->GetInt( k_C3_BUTTON_LDL_BEVELWIDTH );
	}

	if (block->GetAttributeType( k_C3_BUTTON_LDL_BEVELTYPE ) == ATTRIBUTE_TYPE_INT) {
		m_bevelType = block->GetInt( k_C3_BUTTON_LDL_BEVELTYPE );
	} else {
		m_bevelType = 0;
	}

	return InitCommon(bevelWidth);
}


AUI_ERRCODE c3_Button::InitCommon( sint32 bevelWidth )
{
	m_bevelWidth = bevelWidth;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE c3_Button::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( m_pattern ) {
		if ( m_srcWidthPix || m_srcHeightPix ) {
			RECT srcRect = { m_srcX, m_srcY, m_srcX + m_srcWidthPix, m_srcY + m_srcHeightPix };
			m_pattern->Draw( surface, &rect, &srcRect );
		}
		else {
			m_pattern->Draw( surface, &rect );
		}
	}

	DrawThisStateImage(
		IsDown() ? 1 : 0,
		surface,
		&rect );

	if (m_bevelWidth > 0) {
		if ( m_bevelType == 2 ) {
			if ( IsDown() ) {
				primitives_FrameThickRect16( surface, &rect, g_colorSet->GetColor( COLOR_UI_BOX ), m_bevelWidth );
			}
		}
		else {
			primitives_BevelRect16( surface, &rect, m_bevelWidth, (IsDown() ? 1 : 0), 16, 16 );
		}
	}

	RECT down = rect;
	down.left += 2;
	down.top += 2;

	DrawThisText(
		surface,
		IsDown() ? &down : &rect );














































	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}


c3_EditButton::c3_EditButton(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	c3_Button( retval, id, ldlBlock, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateFieldAndActions( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


c3_EditButton::c3_EditButton(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	c3_Button( retval, id, x, y, width, height, pattern, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon(
		k_C3_EDITBUTTON_DEFAULTVAL,
		k_C3_EDITBUTTON_DEFAULTMIN,
		k_C3_EDITBUTTON_DEFAULTMAX );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateFieldAndActions();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE c3_EditButton::InitCommonLdl( MBCHAR *ldlBlock )
{
	aui_Ldl		*theLdl = g_c3ui->GetLdl();

	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );

	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	return InitCommon(
		block->GetAttributeType( k_C3_EDITBUTTON_LDL_VAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_C3_EDITBUTTON_LDL_VAL ) :
			k_C3_EDITBUTTON_DEFAULTVAL,
		block->GetAttributeType( k_C3_EDITBUTTON_LDL_MINVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_C3_EDITBUTTON_LDL_MINVAL ) :
			k_C3_EDITBUTTON_DEFAULTMIN,
		block->GetAttributeType( k_C3_EDITBUTTON_LDL_MAXVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_C3_EDITBUTTON_LDL_MAXVAL ) :
			k_C3_EDITBUTTON_DEFAULTMAX );
}


AUI_ERRCODE c3_EditButton::InitCommon( sint32 val, sint32 min, sint32 max )
{
	m_val = val;
	m_min = min;
	m_max = max;

	m_field = NULL;

	m_origCookie = NULL;
	m_origAction = NULL;
	m_origCallback = NULL;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE c3_EditButton::CreateFieldAndActions( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	aui_Ldl *theLdl = g_ui->GetLdl();
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_C3_EDITBUTTON_LDL_FIELD );

		if ( theLdl->GetLdl()->FindDataBlock( block ) )
			m_field = new C3TextField(
				&errcode,
				aui_UniqueId(),
				block );
	}

	if ( !m_field )
		m_field = new C3TextField(
			&errcode,
			aui_UniqueId(),
			m_x, m_y, m_width, m_height,
			m_pattern ? m_pattern->GetFilename() : NULL );

	Assert( AUI_NEWOK(m_field,errcode) );
	if ( !AUI_NEWOK(m_field,errcode) )
		return AUI_ERRCODE_MEMALLOCFAILED;


	aui_Ldl::Remove( m_field );

	m_field->SetActionFuncAndCookie( c3_EditButtonFieldCallback, this );

	if ( (m_origCallback = GetActionFunc()) )
		m_origCookie = GetCookie();
	else
		m_origAction = GetAction();

	SetActionFuncAndCookie( c3_EditButtonCallback, m_field );

	return AUI_ERRCODE_OK;
}


c3_EditButton::~c3_EditButton( void )
{
	if ( m_field )
	{
		delete m_field;
		m_field = NULL;
	}






	if ( !m_origCallback && m_origAction )
		delete m_origAction;

	m_origCallback = NULL;
	m_cookie = NULL;
	m_origAction = NULL;
}


AUI_ERRCODE c3_EditButton::SetValue( sint32 val )
{
	if ( val < m_min ) val = m_min;
	if ( val > m_max ) val = m_max;

	BOOL changed = m_val != val;

	m_val = val;

	MBCHAR text[ 50 ];
	sprintf(text, "%d", m_val);
	SetText( text );

	if ( changed ) DoCallback();

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE c3_EditButton::SetMinimum( sint32 min )
{
	Assert( min <= m_max );
	if ( min > m_max )
		return AUI_ERRCODE_INVALIDDIMENSION;

	m_min = min;

	if ( m_val < m_min ) m_val = m_min;

	return SetValue( m_val );
}


AUI_ERRCODE c3_EditButton::SetMaximum( sint32 max )
{
	Assert( max >= m_min );
	if ( max < m_min )
		return AUI_ERRCODE_INVALIDDIMENSION;

	m_max = max;

	if ( m_val > m_max ) m_val = m_max;

	return SetValue( m_val );
}


void c3_EditButton::DoCallback( void )
{

	if ( !GetActionFunc() && !GetAction() ) return;

	if ( !HandleGameSpecificLeftClick( this ) )
	if ( m_origCallback )
		m_origCallback( this, AUI_BUTTON_ACTION_EXECUTE, 0, m_origCookie );
	else if ( m_origAction )
		m_origAction->Execute( this, AUI_BUTTON_ACTION_EXECUTE, 0 );
}


void c3_EditButtonCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	c3_EditButton *button = (c3_EditButton *)control;
	C3TextField *field = (C3TextField *)cookie;

	button->GetParent()->AddChild( field );
	button->GetParent()->RemoveChild( button->Id() );

	MBCHAR text[ 50 ];
	sprintf(text, "%d", button->GetValue());
	field->SetFieldText( text );

	field->SetKeyboardFocus();


	field->GetParentWindow()->ShouldDraw();
}


void c3_EditButtonFieldCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_TEXTFIELD_ACTION_EXECUTE
		&& action != (uint32)AUI_TEXTFIELD_ACTION_DISMISS ) return;

	C3TextField *field = (C3TextField *)control;
	c3_EditButton *button = (c3_EditButton *)cookie;

	field->GetParent()->AddChild( button );
	field->GetParent()->RemoveChild( field->Id() );

	MBCHAR text[ 50 ];
	field->GetFieldText( text, 50 );
	button->SetValue( atoi( text ) );

	button->SetKeyboardFocus();


	button->GetParentWindow()->ShouldDraw();
}
