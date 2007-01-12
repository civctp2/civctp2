












#include "c3.h"
#include "c3scroller.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_surface.h"
#include "aui_window.h"
#include "c3thumb.h"
#include "c3ui.h"
#include "textbutton.h"
#include "c3_button.h"
#include "aui_ranger.h"
#include "pattern.h"
#include "primitives.h"

extern C3UI *   g_c3ui;



C3Scroller::C3Scroller
(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie 
)
:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
    aui_Ranger      (),
    PatternBase     (),
    m_isVertical    (false)
{
	*retval = aui_Region::InitCommonLdl( id, ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommonLdl( ldlBlock, ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Ranger::InitCommon( AUI_RANGER_TYPE_SCROLLER, AUI_RANGER_ORIENTATION_VERTICAL);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommonLdl( ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommonLdl( ldlBlock, (MBCHAR *)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if (!block)
    {
        *retval = AUI_ERRCODE_LDLFINDDATABLOCKFAILED;
        return;
    }

	m_isVertical = block->GetBool("vertical");
    
	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateButtonsAndThumb();
	Assert( AUI_SUCCESS(*retval) );
}



C3Scroller::C3Scroller
(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	bool isVertical,
	MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie 
)
:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
    aui_Ranger      (),
    PatternBase     (),
    m_isVertical    (isVertical)
{
	*retval = aui_Region::InitCommon( id, x, y, width, height );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommon( ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Ranger::InitCommon( AUI_RANGER_TYPE_SCROLLER, AUI_RANGER_ORIENTATION_VERTICAL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommon((MBCHAR **) NULL);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommon( pattern );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateButtonsAndThumb();
	Assert( AUI_SUCCESS(*retval) );
}






AUI_ERRCODE C3Scroller::InitCommon()
{
	if (m_isVertical)
    {
		m_valX = m_minX = m_maxX = m_incX = m_pageX = 0;
    }
	else
    {
		m_valY = m_minY = m_maxY = m_incY = m_pageY = 0;
    }

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE C3Scroller::CreateButtonsAndThumb( void )
{
	aui_Button *button1;
	aui_Button *button2;

	AUI_ERRCODE errcode;

	m_thumb = new C3Thumb( &errcode, aui_UniqueId(), 0, 0, 0, 0, m_pattern->GetFilename(), RangerThumbActionCallback, this );
	if ( m_isVertical )
	{
		button1 = m_incYButton = new c3_Button( &errcode,
			aui_UniqueId(), 0, 0, 0, 0, m_pattern->GetFilename(), RangerButtonActionCallback, this );
		button2 = m_decYButton = new c3_Button( &errcode,
			aui_UniqueId(), 0, 0, 0, 0, m_pattern->GetFilename(), RangerButtonActionCallback, this );
	}
	else
	{
		button1 = m_incXButton = new TextButton( &errcode,
			aui_UniqueId(), 0, 0, 0, 0, m_pattern->GetFilename(), ">", RangerButtonActionCallback, this );
		button2 = m_decXButton = new TextButton( &errcode,
			aui_UniqueId(), 0, 0, 0, 0, m_pattern->GetFilename(), "<", RangerButtonActionCallback, this );
	}

	
	AddChild( m_thumb );
	AddChild( button1 );
	AddChild( button2 );

	
	RepositionButtons();

	
	RepositionThumb( FALSE );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE C3Scroller::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	

	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	if ( m_pattern ) m_pattern->Draw( surface, &rect );












	primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	if ( IsActive() )
	{
		
		primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
