












#include "c3.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_surface.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "c3_Ranger.h"
#include "pattern.h"

#include "pixelutils.h"
#include "ColorSet.h"
#include "primitives.h"

#include "c3listbox.h"


extern ColorSet		*g_colorSet;


C3ListBox::C3ListBox(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ListBox(),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_ImageBase( ldlBlock )
{
	*retval = aui_Region::InitCommonLdl( id, ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommonLdl( ldlBlock, ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommonLdl( ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommonLdl( ldlBlock, (MBCHAR *)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_ListBox::InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRangers();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	
	m_drawMask = k_AUI_REGION_DRAWFLAG_UPDATE
		| k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE
		| k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE
		| k_AUI_REGION_DRAWFLAG_MOUSELDOUBLECLICKINSIDE;
}



C3ListBox::C3ListBox(
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
	aui_ListBox(),
	aui_TextBase( NULL ),
	aui_ImageBase( (sint32)0 )
{
	*retval = aui_Region::InitCommon( id, x, y, width, height );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommon( ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommon((MBCHAR **)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommon( pattern );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_ListBox::InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRangers();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	
	m_drawMask = k_AUI_REGION_DRAWFLAG_UPDATE
		| k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE
		| k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE
		| k_AUI_REGION_DRAWFLAG_MOUSELDOUBLECLICKINSIDE;
}



AUI_ERRCODE C3ListBox::CreateRangers( void )
{
	AUI_ERRCODE errcode;

	m_verticalRanger = new c3_Ranger(
		&errcode,
		aui_UniqueId(),
		0, 0, 0, 0,
		AUI_RANGER_TYPE_SCROLLER,
		AUI_RANGER_ORIENTATION_VERTICAL,
		m_pattern->GetFilename(),
		ListBoxRangerActionCallback,
		this );
	if ( !m_verticalRanger ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_horizontalRanger = new c3_Ranger(
		&errcode,
		aui_UniqueId(),
		0, 0, 0, 0,
		AUI_RANGER_TYPE_SCROLLER,
		AUI_RANGER_ORIENTATION_HORIZONTAL,
		m_pattern->GetFilename(),
		ListBoxRangerActionCallback,
		this );
	if ( !m_horizontalRanger ) return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_verticalRanger );
	AddChild( m_horizontalRanger );

	return RepositionRangers();
}



AUI_ERRCODE C3ListBox::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	if ( !surface ) surface = m_window->TheSurface();

	sint32 headerHeight = m_header->IsHidden() ? 0 : m_header->Height();

	RECT rect = { 0, -headerHeight, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	if ( m_pattern ) m_pattern->Draw( surface, &rect );

	primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	if ( IsActive() )
	{
		
		primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	}


	

	sint32 minVertical = m_verticalRanger->GetValueY();
	sint32 maxVertical = minVertical + m_itemsPerHeight;

	if ( maxVertical > m_numRows ) maxVertical = m_numRows;

	RECT selectRect = rect;
	InflateRect( &selectRect, -1, 0 );

	ListPos position = m_visualSelectedList->GetHeadPosition();

	if ( m_absorbEvents ) {
		for ( sint32 i = m_visualSelectedList->L(); i; i-- )
		{
			sint32 itemIndex = m_visualSelectedList->GetNext( position );
			if ( minVertical <= itemIndex && itemIndex < maxVertical )
			{
				selectRect.top = rect.top + headerHeight + 2 + ( itemIndex - minVertical ) * m_maxItemHeight;
				selectRect.bottom = selectRect.top + m_maxItemHeight - 4;
				primitives_FrameRect16( surface, &selectRect, g_colorSet->GetColor(COLOR_SELECT_0));
				primitives_BevelRect16( surface, &selectRect, 1, 1, 10, 10 );
	
			}
		}
	}


	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
