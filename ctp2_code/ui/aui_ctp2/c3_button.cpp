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
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3_button.h"

#include "aui.h"
#include "aui_action.h"
#include "aui_gamespecific.h"
#include "aui_ldl.h"
#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_window.h"
#include "c3textfield.h"
#include "c3ui.h"
#include "colorset.h"               // g_colorSet
#include "pattern.h"
#include "patternbase.h"
#include "primitives.h"

#include "ldl_data.hpp"

c3_Button::c3_Button
(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	Cookie cookie
)
:
    // virtual (see aui_Control)
    aui_ImageBase   (ldlBlock ),
    aui_TextBase    (ldlBlock, (MBCHAR *) NULL),
    // normal
    aui_Button      (retval, id, ldlBlock, ActionFunc, cookie),
    PatternBase     (ldlBlock, NULL),
    m_bevelWidth    (k_C3_BUTTON_DEFAULT_BEVELWIDTH),
    m_bevelType     (0)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
}


c3_Button::c3_Button(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	const MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	Cookie cookie )
:
    // virtual (see aui_Control)
    aui_ImageBase   ((sint32) 0),
    aui_TextBase    (NULL),
    // normal
    aui_Button      (retval, id, x, y, width, height, ActionFunc, cookie),
    PatternBase     (pattern),
    m_bevelWidth    (k_C3_BUTTON_DEFAULT_BEVELWIDTH),
    m_bevelType     (0)
{
	Assert(AUI_SUCCESS(*retval));
}


AUI_ERRCODE c3_Button::InitCommonLdl( const MBCHAR *ldlBlock )
{
	ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	if (block->GetAttributeType( k_C3_BUTTON_LDL_BEVELWIDTH) == ATTRIBUTE_TYPE_INT)
	{
		m_bevelWidth = block->GetInt( k_C3_BUTTON_LDL_BEVELWIDTH );
	}
	else
	{
		m_bevelWidth = k_C3_BUTTON_DEFAULT_BEVELWIDTH;
	}

	if (block->GetAttributeType( k_C3_BUTTON_LDL_BEVELTYPE ) == ATTRIBUTE_TYPE_INT)
	{
		m_bevelType = block->GetInt( k_C3_BUTTON_LDL_BEVELTYPE );
	}
	else
	{
		m_bevelType = 0;
	}

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


c3_EditButton::c3_EditButton
(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	Cookie cookie
)
:
    // virtual (defined in aui_Control)
    aui_ImageBase   (ldlBlock),
    aui_TextBase    (ldlBlock, (MBCHAR const *) NULL),
    // normal
    c3_Button       (retval, id, ldlBlock, ActionFunc, cookie),
    m_val           (k_C3_EDITBUTTON_DEFAULTVAL),
    m_min           (k_C3_EDITBUTTON_DEFAULTMIN),
    m_max           (k_C3_EDITBUTTON_DEFAULTMAX),
    m_field         (NULL),
    m_origAction    (NULL),
    m_origCallback  (NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateFieldAndActions( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
}

c3_EditButton::c3_EditButton
(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	const MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	Cookie cookie
)
:
    // virtual (see aui_Control)
    aui_ImageBase   ((sint32) 0),
    aui_TextBase    (NULL),
    // normal
    c3_Button       (retval, id, x, y, width, height, pattern, ActionFunc, cookie),
    m_val           (k_C3_EDITBUTTON_DEFAULTVAL),
    m_min           (k_C3_EDITBUTTON_DEFAULTMIN),
    m_max           (k_C3_EDITBUTTON_DEFAULTMAX),
    m_field         (NULL),
    m_origAction    (NULL),
    m_origCallback  (NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateFieldAndActions();
	Assert( AUI_SUCCESS(*retval) );
}

AUI_ERRCODE c3_EditButton::InitCommonLdl( const MBCHAR *ldlBlock )
{
	ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	m_val =
		block->GetAttributeType( k_C3_EDITBUTTON_LDL_VAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_C3_EDITBUTTON_LDL_VAL ) :
			k_C3_EDITBUTTON_DEFAULTVAL;
	m_min =
		block->GetAttributeType( k_C3_EDITBUTTON_LDL_MINVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_C3_EDITBUTTON_LDL_MINVAL ) :
			k_C3_EDITBUTTON_DEFAULTMIN;
	m_max =
		block->GetAttributeType( k_C3_EDITBUTTON_LDL_MAXVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_C3_EDITBUTTON_LDL_MAXVAL ) :
			k_C3_EDITBUTTON_DEFAULTMAX;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE c3_EditButton::CreateFieldAndActions( const MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_C3_EDITBUTTON_LDL_FIELD );

		if (aui_Ldl::FindDataBlock( block ) )
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

	m_origCallback  = GetActionFunc();

	/// @todo Check the working of the union: &m_origCookie == &m_origAction!
	if (m_origCallback)
	{
		m_origCookie    = GetCookie();
	}
	else
	{
		m_origAction    = GetAction();
	}

	SetActionFuncAndCookie( c3_EditButtonCallback, m_field );

	return AUI_ERRCODE_OK;
}


c3_EditButton::~c3_EditButton( void )
{
	delete m_field;

	if (!m_origCallback)
    {
        delete m_origAction;    /// @todo Check this is OK: m_origAction is not allocated
    }
}


AUI_ERRCODE c3_EditButton::SetValue( sint32 val )
{
	if ( val < m_min ) val = m_min;
	if ( val > m_max ) val = m_max;

	bool changed = m_val != val;

	m_val = val;

	MBCHAR text[ 50 ];
	sprintf( text, "%d", m_val ); // itoa( m_val, text, 10 );
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
	// Recheck impact on current value
	return SetValue(m_val);
}


AUI_ERRCODE c3_EditButton::SetMaximum( sint32 max )
{
	Assert( max >= m_min );
	if ( max < m_min )
		return AUI_ERRCODE_INVALIDDIMENSION;

	m_max = max;
	// Recheck impact on current value
	return SetValue(m_val);
}


void c3_EditButton::DoCallback( void )
{

	if ( !GetActionFunc() && !GetAction() ) return;

	if ( !HandleGameSpecificLeftClick( this ) ) {
		if ( m_origCallback ) {
			m_origCallback( this, AUI_BUTTON_ACTION_EXECUTE, 0, m_origCookie );
		}
		else if ( m_origAction ) {
			m_origAction->Execute( this, AUI_BUTTON_ACTION_EXECUTE, 0 );
		}
	}
}


void c3_EditButtonCallback( aui_Control *control, uint32 action, uint32 data, Cookie cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	c3_EditButton *button = (c3_EditButton *)control;
	C3TextField *field = (C3TextField *)cookie.m_voidPtr;

	button->GetParent()->AddChild( field );
	button->GetParent()->RemoveChild( button->Id() );

	MBCHAR text[ 50 ];
	sprintf(text, "%d", button->GetValue()); // itoa( button->GetValue(), text, 10 );

	field->SetFieldText( text );
	field->SetKeyboardFocus();
	field->GetParentWindow()->ShouldDraw();
}


void c3_EditButtonFieldCallback( aui_Control *control, uint32 action, uint32 data, Cookie cookie )
{
	if ( action != (uint32)AUI_TEXTFIELD_ACTION_EXECUTE
		&& action != (uint32)AUI_TEXTFIELD_ACTION_DISMISS ) return;

	C3TextField *field = (C3TextField *)control;
	c3_EditButton *button = (c3_EditButton *)cookie.m_voidPtr;

	field->GetParent()->AddChild( button );
	field->GetParent()->RemoveChild( field->Id() );

	MBCHAR text[ 50 ];
	field->GetFieldText( text, 50 );

	button->SetValue( atoi( text ) );
	button->SetKeyboardFocus();
	button->GetParentWindow()->ShouldDraw();
}
