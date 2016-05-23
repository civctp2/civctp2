#include "c3.h"
#include "aui.h"
#include "aui_ldl.h"
#include "aui_ui.h"
#include "aui_window.h"
#include "aui_gamespecific.h"

#include "c3_static.h"

#include "patternbase.h"
#include "pattern.h"
#include "primitives.h"
#include "colorset.h"

extern aui_UI		*g_ui;
extern ColorSet		*g_colorSet;

c3_Static::c3_Static(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_Static( retval, id, ldlBlock ),
	PatternBase(ldlBlock, NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


c3_Static::c3_Static(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	MBCHAR *text,
	uint32 maxLength,
	uint32 bevelWidth,
	uint32 bevelType)
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( text, maxLength ),
	aui_Static( retval, id, x, y, width, height, text, maxLength),
	PatternBase(pattern)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon(bevelWidth, bevelType);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE c3_Static::InitCommonLdl( MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	if (block->GetAttributeType( k_C3_STATIC_LDL_BEVELWIDTH ) == ATTRIBUTE_TYPE_INT) {
		m_bevelWidth = block->GetInt( k_C3_STATIC_LDL_BEVELWIDTH );
	} else {
		m_bevelWidth = 0;
	}

	if (block->GetAttributeType( k_C3_STATIC_LDL_BEVELTYPE ) == ATTRIBUTE_TYPE_INT) {
		m_bevelType = block->GetInt( k_C3_STATIC_LDL_BEVELTYPE );
	} else {
		m_bevelType = 0;
	}

	return AUI_ERRCODE_OK;

	return InitCommon(m_bevelWidth, m_bevelType);
}


AUI_ERRCODE c3_Static::InitCommon(uint32 bevelWidth, uint32 bevelType )
{
	m_bevelWidth = bevelWidth;
	m_bevelType = bevelType;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE c3_Static::DrawThis(
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
		0,
		surface,
		&rect );









	if (m_bevelWidth > 0) {
		if ( m_bevelType == 2 ) {
			primitives_FrameThickRect16( surface, &rect, g_colorSet->GetColor( COLOR_UI_BOX ), m_bevelWidth );
		}
		else {
			primitives_BevelRect16( surface, &rect, m_bevelWidth, m_bevelType, 16, 16 );
		}
	}

	DrawThisText(
		surface,
		&rect );

	if (m_window)
		if ( surface == m_window->TheSurface() )
			m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

void c3_Static::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );

	m_mouseCode = AUI_ERRCODE_HANDLED;
}

void c3_Static::MouseRGrabInside( aui_MouseEvent *mouseData)
{
	if (!GetWhichSeesMouse())
		SetWhichSeesMouse(this);

	m_mouseCode = AUI_ERRCODE_HANDLED;
}


void c3_Static::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );

	if(!HandleGameSpecificLeftClick((void *)this)) {
		if(m_ActionFunc) {
			m_ActionFunc(this, k_C3_STATIC_ACTION_LMOUSE, 0, m_cookie);
		}
	}

	m_mouseCode = AUI_ERRCODE_HANDLED;
}

void c3_Static::MouseRDropInside( aui_MouseEvent *mouseData )
{

#if 0
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this ) {
		SetWhichSeesMouse( this );
	}

	HandleGameSpecificRightClick((void *)this);
	m_mouseCode = AUI_ERRCODE_HANDLED;








#else
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this ) {
		SetWhichSeesMouse( this );

		if(m_ActionFunc) {
			m_ActionFunc(this, k_C3_STATIC_ACTION_RMOUSE, 0, m_cookie);
		} else {
			HandleGameSpecificRightClick((void *)this);
		}

		m_mouseCode = AUI_ERRCODE_HANDLED;
	}
	else {
		MouseRDropOutside( mouseData );
	}
#endif
}
