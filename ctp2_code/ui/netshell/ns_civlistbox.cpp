//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : 
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
// - Prevented a compiler error (ambiguity) for .NET.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui_ui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_window.h"
#include "aui_item.h"

#include "c3_ranger.h"

#include "pattern.h"
#include "primitives.h"
#include "colorset.h"

#include "ns_header.h"

#include "ns_civlistbox.h"


extern ColorSet		*g_colorSet;



ns_CivListBox::ns_CivListBox(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_ListBox(),
	PatternBase(ldlBlock, (MBCHAR *)NULL)
{
	*retval = aui_Region::InitCommonLdl( id, ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommonLdl( ldlBlock, ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_ListBox::InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRangersAndHeader( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



ns_CivListBox::ns_CivListBox(
	AUI_ERRCODE *retval, 
	uint32 id, 
	sint32 x, 
	sint32 y, 
	sint32 width, 
	sint32 height, 
	MBCHAR *pattern,
	sint32 bevelWidth,
	sint32 bevelType,
	ControlActionCallback *ActionFunc,
	void *cookie)
	:
	aui_ImageBase((sint32) 0),
	aui_TextBase((MBCHAR const *) NULL, (uint32) 0),
	aui_ListBox(),
	PatternBase(pattern)
{
	*retval = aui_Region::InitCommon( id, x, y, width, height );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommon( (MBCHAR **)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommon(ActionFunc, cookie);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_ListBox::InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon(bevelWidth, bevelType);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRangersAndHeader( NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE ns_CivListBox::InitCommonLdl( MBCHAR *ldlBlock )
{
	sint32		bevelWidth=0, bevelType=0;
	aui_Ldl		*theLdl = g_ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );

	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	bevelWidth = block->GetInt( k_NS_CIVLISTBOX_LDL_BEVELWIDTH );
	bevelType = block->GetInt( k_NS_CIVLISTBOX_LDL_BEVELTYPE );

	return InitCommon(bevelWidth, bevelType);
}



AUI_ERRCODE ns_CivListBox::InitCommon( sint32 bevelWidth, sint32 bevelType )
{
	
	m_drawMask = k_AUI_REGION_DRAWFLAG_UPDATE
		| k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE
		| k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE
		| k_AUI_REGION_DRAWFLAG_MOUSELDOUBLECLICKINSIDE;

	m_bevelWidth = bevelWidth;
	m_bevelType = bevelType;

	m_selectableList = TRUE;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE ns_CivListBox::CreateRangersAndHeader( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		*patternFilename = NULL;

	if (m_pattern)
		patternFilename = m_pattern->GetFilename();

	
	aui_Ldl *theLdl = g_ui->GetLdl();
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		
		if ( m_header )
		{
			RemoveChild( m_header->Id() );
			delete m_header;
			m_header = NULL;
		}

		sprintf( block, "%s.%s", ldlBlock, k_AUI_LISTBOX_LDL_HEADER );

		
		if ( theLdl->GetLdl()->FindDataBlock( block ) )
			m_header = new ns_Header(
				&errcode,
				aui_UniqueId(),
				block );
	}

	if ( !m_header )
		m_header = new ns_Header(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0 );


	Assert( AUI_NEWOK(m_header,errcode) );
	if ( !AUI_NEWOK(m_header,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_header );

	
	ListPos position = m_header->ChildList()->GetHeadPosition();
	for ( sint32 i = m_header->ChildList()->L(); i; i-- )
		m_widthList->AddTail(
			m_header->ChildList()->GetNext( position )->Width() );

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_LISTBOX_LDL_RANGERY );

		
		if ( theLdl->GetLdl()->FindDataBlock( block ) )
			m_verticalRanger = new c3_Ranger(
				&errcode,
				aui_UniqueId(),
				block,
				ListBoxRangerActionCallback,
				this );
	}

	if ( !m_verticalRanger )
		m_verticalRanger = new c3_Ranger(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0,
			AUI_RANGER_TYPE_SCROLLER,
			AUI_RANGER_ORIENTATION_VERTICAL,
			patternFilename,
			ListBoxRangerActionCallback,
			this );

	Assert( AUI_NEWOK(m_verticalRanger,errcode) );
	if ( !AUI_NEWOK(m_verticalRanger,errcode) )
		return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_verticalRanger );

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_LISTBOX_LDL_RANGERX );

		
		if ( theLdl->GetLdl()->FindDataBlock( block ) )
			m_horizontalRanger = new c3_Ranger(
				&errcode,
				aui_UniqueId(),
				block,
				ListBoxRangerActionCallback,
				this );
	}

	if ( !m_horizontalRanger )
		m_horizontalRanger = new c3_Ranger(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0,
			AUI_RANGER_TYPE_SCROLLER,
			AUI_RANGER_ORIENTATION_HORIZONTAL,
			patternFilename,
			ListBoxRangerActionCallback,
			this );

	Assert( AUI_NEWOK(m_horizontalRanger,errcode) );
	if ( !AUI_NEWOK(m_horizontalRanger,errcode) )
		return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_horizontalRanger );

	sint32 maxRangerSize = m_verticalRanger->Width();
	if ( m_horizontalRanger->Height() > maxRangerSize )
		maxRangerSize = m_horizontalRanger->Height();

	if ( maxRangerSize )
		SetRangerSize( maxRangerSize ); 
	else
		RepositionRangers();

	CalculateDimensions();
	RepositionHeaderSwitches();

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE ns_CivListBox::RemoveItem( uint32 id )
{
	return aui_ListBox::RemoveItem( id );
}



AUI_ERRCODE ns_CivListBox::Draw(
	aui_Surface *surf,
	sint32 x,
	sint32 y)
{
	
	aui_Region::Draw(surf, x, y);

	if (IsHidden()) return AUI_ERRCODE_OK;

	if ( !surf ) surf = m_window->TheSurface();

	sint32 headerHeight = m_header->IsHidden() ? 0 : m_header->Height();

	RECT rect = { 0, -headerHeight, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	sint32 minVertical = m_verticalRanger->GetValueY();
	sint32 maxVertical = minVertical + m_itemsPerHeight;

	if ( maxVertical > m_numRows ) maxVertical = m_numRows;

	RECT selectRect = rect;
	InflateRect( &selectRect, -1, 0 );

	if ( m_selectableList )
	{
		
		ListPos position = m_visualSelectedList->GetHeadPosition();

		
		
		

		for ( sint32 i = m_visualSelectedList->L(); i; i-- )
		{
			
			sint32 itemIndex = m_visualSelectedList->GetNext( position );
			if ( minVertical <= itemIndex && itemIndex < maxVertical )
			{
				selectRect.top = rect.top + headerHeight + 2 +
					( itemIndex - minVertical ) * m_maxItemHeight;
				selectRect.bottom = selectRect.top + m_maxItemHeight - 4;

				
				primitives_FrameRect16(
					surf,
					&selectRect,
					g_colorSet->GetColor(COLOR_SELECT_0) );
				primitives_BevelRect16( surf, &selectRect, 1, 1, 10, 10 );
			}
		}

	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE ns_CivListBox::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{
	if (IsHidden()) return AUI_ERRCODE_OK;
	
	if ( !surface ) surface = m_window->TheSurface();

	sint32 headerHeight = m_header->IsHidden() ? 0 : m_header->Height();

	RECT rect = { 0, -headerHeight, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	if (m_bevelWidth > 0) {
		RECT bevelRect = rect;
		if (!m_verticalRanger->IsHidden())
			bevelRect.right += m_rangerSize;
		if (!m_horizontalRanger->IsHidden())
			bevelRect.bottom += m_rangerSize;
		InflateRect(&bevelRect, m_bevelWidth, m_bevelWidth);

		
		if ( m_pattern ) {
			if ( m_srcWidthPix || m_srcHeightPix ) {
				RECT srcRect = { m_srcX, m_srcY, m_srcX + m_srcWidthPix, m_srcY + m_srcHeightPix };
				m_pattern->Draw( surface, &bevelRect, &srcRect );
			}
			else {
				m_pattern->Draw( surface, &bevelRect );
			}
		}
		
		DrawThisStateImage(
			0,
			surface,
			&bevelRect );

		if ( m_bevelType == 2 ) {
			primitives_FrameThickRect16( surface, &bevelRect, g_colorSet->GetColor( COLOR_UI_BOX ), m_bevelWidth );
		}
		else {
			primitives_BevelRect16( surface, &bevelRect, m_bevelWidth, m_bevelType, 16, 16 );
		}

		if ( surface == m_window->TheSurface() )
			m_window->AddDirtyRect( &bevelRect );
	} else {
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
		
		m_window->AddDirtyRect(&rect);
	}

	return AUI_ERRCODE_OK;
}



ns_HPlayerListBox::ns_HPlayerListBox(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	ns_CivListBox( retval, id, ldlBlock, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



ns_HPlayerListBox::ns_HPlayerListBox(
	AUI_ERRCODE *retval, 
	uint32 id, 
	sint32 x, 
	sint32 y, 
	sint32 width, 
	sint32 height, 
	MBCHAR *pattern,
	sint32 bevelWidth,
	sint32 bevelType,
	ControlActionCallback *ActionFunc,
	void *cookie)
	:
	aui_ImageBase((sint32) 0),
	aui_TextBase((MBCHAR const *) NULL, (uint32) 0),
	ns_CivListBox(
		retval, id, x, y, width, height, pattern, bevelWidth, bevelType,
		ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

ns_HPlayerListBox::~ns_HPlayerListBox()
{
	
	
	
	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = m_pane->ChildList()->L(); i; i-- )
	{
		ListPos prevPosition = position;
		aui_Item *item = (aui_Item *)m_pane->ChildList()->GetNext( position );
		delete item;
	}
	m_pane->ChildList()->DeleteAll();
}




AUI_ERRCODE ns_HPlayerListBox::InitCommonLdl( MBCHAR *ldlBlock )
{
	return InitCommon();
}



AUI_ERRCODE ns_HPlayerListBox::InitCommon( void )
{
	
	SetAbsorbancy( FALSE );

	return AUI_ERRCODE_OK;
}
