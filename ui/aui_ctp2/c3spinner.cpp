












#include "c3.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_surface.h"
#include "aui_window.h"
#include "aui_ranger.h"
#include "aui_ldl.h"

#include "c3ui.h"
#include "c3thumb.h"
#include "textbutton.h"
#include "pattern.h"

#include "primitives.h"

#include "c3spinner.h"


extern C3UI *g_c3ui;



C3Spinner::C3Spinner(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_Ranger(),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_ImageBase( ldlBlock )
{
	*retval = aui_Region::InitCommonLdl( id, ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommonLdl( ldlBlock, ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Ranger::InitCommon( AUI_RANGER_TYPE_SPINNER, AUI_RANGER_ORIENTATION_VERTICAL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommonLdl( ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommonLdl( ldlBlock, (MBCHAR *)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateButtons();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE C3Spinner::InitCommon( MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_c3ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	
	InitCommon( block->GetBool( "vertical" ) );

	return AUI_ERRCODE_OK;
}



C3Spinner::C3Spinner(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	BOOL isVertical,
	MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_Ranger(),
	aui_TextBase(NULL),
	aui_ImageBase( (sint32)0 )
{
	aui_Region::InitCommon( id, x, y, width, height );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	aui_Control::InitCommon( ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	aui_Ranger::InitCommon( AUI_RANGER_TYPE_SPINNER, AUI_RANGER_ORIENTATION_VERTICAL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommon((MBCHAR **)NULL);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommon( pattern );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( isVertical );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateButtons();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE C3Spinner::InitCommon( BOOL isVertical )
{
	if ( m_isVertical = isVertical )
		m_valX = m_minX = m_maxX = m_incX = m_pageX = 0;
	else
		m_valY = m_minY = m_maxY = m_incY = m_pageY = 0;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE C3Spinner::CreateButtons( void )
{
	aui_Button *button1;
	aui_Button *button2;

	AUI_ERRCODE errcode;

	if ( m_isVertical )
	{
		button1 = m_incYButton = new TextButton( &errcode,
			aui_UniqueId(), 0, 0, 0, 0, m_pattern->GetFilename(), "v", RangerButtonActionCallback, this );
		button2 = m_decYButton = new TextButton( &errcode,
			aui_UniqueId(), 0, 0, 0, 0, m_pattern->GetFilename(), "^", RangerButtonActionCallback, this );
	}
	else
	{
		button1 = m_incXButton = new TextButton( &errcode,
			aui_UniqueId(), 0, 0, 0, 0, m_pattern->GetFilename(), ">", RangerButtonActionCallback, this );
		button2 = m_decXButton = new TextButton( &errcode,
			aui_UniqueId(), 0, 0, 0, 0, m_pattern->GetFilename(), "<", RangerButtonActionCallback, this );
	}

	
	AddChild( button1 );
	AddChild( button2 );

	
	RepositionButtons();

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE C3Spinner::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	

	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect;
	if ( m_isVertical )
		SetRect(
			&rect,
			-m_width,
			-m_buttonSize,
			0,
			m_height + m_buttonSize );
	else
		SetRect(
			&rect,
			-m_buttonSize,
			-m_height,
			m_width + m_buttonSize,
			0 );
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	if ( m_pattern ) m_pattern->Draw( surface, &rect );

	primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	if ( IsActive() )
	{
		
		primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	}

	

	static char s[ 20 ];
	sprintf( s, "%d", 100 - ( m_isVertical ? GetValueY() : GetValueX() ) );

	primitives_DropText(
		(aui_DirectSurface *)surface,
		rect.left + 5,
		rect.top + 5,
		s,
		0x0000,
		1 );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
