//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : List box handling
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
// - Prevented crash (access to deleted memory).
// - #01 Optimized sorting of list entries.
//   (L. Hirth 7/2004)
// - Corrected ambiguous function reference.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui.h"

#include "aui_ui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_ranger.h"
#include "aui_window.h"
#include "aui_item.h"
#include "aui_switch.h"
#include "aui_blitter.h"

#include "c3_header.h"
#include "c3_ranger.h"
#include "c3_listitem.h"

#include "ctp2_listbox.h"
#include "ctp2_dropdown.h"

#include "pattern.h"
#include "primitives.h"
#include "colorset.h"

#include "ctp2_MenuButton.h"
#include "aui_dragdropwindow.h"


extern aui_UI		*g_ui;
extern ColorSet		*g_colorSet;




uint32 ctp2_ListBox::m_ctp2_listboxClassId = aui_UniqueId();
ctp2_ListBox *ctp2_ListBox::ms_mouseFocusListBox = NULL;


ctp2_ListBox::ctp2_ListBox(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock),
	aui_TextBase(ldlBlock, (const MBCHAR *)NULL),
	aui_ListBox(),
	PatternBase(ldlBlock, (const MBCHAR *)NULL)
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


ctp2_ListBox::ctp2_ListBox(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	const MBCHAR *pattern,
	sint32 bevelWidth,
	sint32 bevelType,
	ControlActionCallback *ActionFunc,
	void *cookie)
	:
	aui_ImageBase((sint32) 0),
	aui_TextBase((const MBCHAR *) NULL, (uint32) 0),
	aui_ListBox(),
	PatternBase(pattern)
{
	*retval = aui_Region::InitCommon( id, x, y, width, height );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommon((const MBCHAR **)NULL);
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




ctp2_ListBox::~ctp2_ListBox()
{
	if (this == ms_mouseFocusListBox)
	{
		ms_mouseFocusListBox = NULL;
	}

	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = m_pane->ChildList()->L(); i; i-- )
	{
		aui_Item *item = (aui_Item *)m_pane->ChildList()->GetNext( position );
		delete item;
	}
	m_pane->ChildList()->DeleteAll();
}


AUI_ERRCODE ctp2_ListBox::InitCommonLdl(const MBCHAR *ldlBlock)
{
	sint32		bevelWidth=0, bevelType=0;
	aui_Ldl		*theLdl = g_ui->GetLdl();

	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );

	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	bevelWidth = block->GetInt( k_CTP2_LISTBOX_LDL_BEVELWIDTH );
	bevelType = block->GetInt( k_CTP2_LISTBOX_LDL_BEVELTYPE );

	return InitCommon(bevelWidth, bevelType);
}


AUI_ERRCODE ctp2_ListBox::InitCommon(sint32 bevelWidth, sint32 bevelType)
{

	m_drawMask = k_AUI_REGION_DRAWFLAG_UPDATE
		| k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE
		| k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE
		| k_AUI_REGION_DRAWFLAG_MOUSELDOUBLECLICKINSIDE
		| k_AUI_REGION_DRAWFLAG_KEYBOARDFOCUSCHANGE;

	m_bevelWidth = bevelWidth;
	m_bevelType = bevelType;
	m_menuButton = NULL;

	m_borderOffset.x = 0;
	m_borderOffset.y = 0;

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE ctp2_ListBox::CreateRangersAndHeader(const MBCHAR *ldlBlock)
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
			m_header = new c3_Header(
				&errcode,
				aui_UniqueId(),
				block );
	}

	if ( !m_header )
		m_header = new c3_Header(
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


void ctp2_ListBox::Clear(void)
{
	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = m_pane->ChildList()->L(); i; i-- )
	{
		aui_Item *item = (aui_Item *)m_pane->ChildList()->GetNext( position );

		RemoveItem(item->Id());
		delete item;

	}
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	m_pane->ChildList()->DeleteAll();
}




AUI_ERRCODE ctp2_ListBox::SortByColumn(
	sint32 column,
	BOOL ascending )
{

	if ( column == -1 ) return AUI_ERRCODE_OK;

	if ( column != -2 )
	{
		Assert( 0 <= column && column < m_numColumns );
		if ( 0 > column || column >= m_numColumns ) return AUI_ERRCODE_INVALIDPARAM;
	}

	m_sortColumn = column;
	m_sortAscending = ascending;

	if (m_numRows <= 1) return AUI_ERRCODE_OK;

	sint32 cycles = 0;
	BOOL changed;
	do
	{
		changed = FALSE;

		for ( sint32 i = 0; i < m_numRows-1-cycles; i++ )
		{
			ListPos positionA = m_pane->ChildList()->FindIndex( i );
			ListPos positionB = positionA;

			c3_ListItem *itemA = (c3_ListItem *)m_pane->ChildList()->GetNext( positionB );
			c3_ListItem *itemB = (c3_ListItem *)m_pane->ChildList()->GetAt( positionB );

			if ( m_sortAscending ?
				itemA->Compare(itemB, m_sortColumn) > 0 :
				itemA->Compare(itemB, m_sortColumn) < 0 )
			{
				SwitchItems( i, i + 1 );
				changed = TRUE;
			}
		}
	++cycles;
	} while ( changed );

	RepositionItems();

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE ctp2_ListBox::Draw(aui_Surface *surf, sint32 x, sint32 y)
{

	aui_Region::Draw(surf, x, y);

	if (IsHidden()) return AUI_ERRCODE_OK;

	if ( !surf ) surf = m_window->TheSurface();

	sint32 headerHeight = m_header->IsHidden() ? 0 : m_header->Height();

	RECT rect = { m_borderOffset.x, -headerHeight + m_borderOffset.y,
				  m_width - m_borderOffset.x, m_height - m_borderOffset.y };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	sint32 minVertical = m_verticalRanger->GetValueY();
	sint32 maxVertical = minVertical + m_itemsPerHeight;

	if ( maxVertical > m_numRows ) maxVertical = m_numRows;

	RECT selectRect = rect;
	InflateRect( &selectRect, -1, 0 );

	ListPos position = m_visualSelectedList->GetHeadPosition();

		for ( sint32 i = m_visualSelectedList->L(); i; i-- )
		{

			sint32 itemIndex = m_visualSelectedList->GetNext( position );
			if ( minVertical <= itemIndex && itemIndex < maxVertical )
			{
				selectRect.top = rect.top + headerHeight
					+ ( itemIndex - minVertical ) * m_maxItemHeight;
				selectRect.bottom = selectRect.top + m_maxItemHeight;

				g_ui->TheBlitter()->ColorBlt(surf, &selectRect,
											 RGB(255,179,69), 0);
				GetItemByIndex(itemIndex)->Draw(surf);




			}
		}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE ctp2_ListBox::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
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

		if(GetNumberOfLayers() > 0)	{

			RECT layerRectangle = { 0, 0, m_width, m_height };
			OffsetRect(&layerRectangle, m_x + x, m_y + y);
			ToWindow(&layerRectangle);
			DrawLayers(surface, &layerRectangle);
		} else
			DrawThisStateImage(0, surface, &bevelRect);

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

		if(GetNumberOfLayers() > 0)	{

			RECT layerRectangle = { 0, 0, m_width, m_height };
			OffsetRect(&layerRectangle, m_x + x, m_y + y);
			ToWindow(&layerRectangle);
			DrawLayers(surface, &layerRectangle);
		} else
			DrawThisStateImage(0, surface, &rect);

		if(m_window->GetFocusControl() == this) {
			primitives_FrameThickRect16(surface, &rect, g_colorSet->GetColor(COLOR_UI_BOX), 3);
		}
		m_window->AddDirtyRect(&rect);
	}

	return AUI_ERRCODE_OK;
}





void ctp2_ListBox::ForceScroll(sint32 deltaX, sint32 deltaY)
{
	m_scrollDx = 0;
	m_scrollDy = (deltaY == 0) ? 0 : ((deltaY < 0) ? -1 : 1);

	ScrollList();
}

















void ctp2_ListBox::MouseMoveInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	aui_ListBox::MouseMoveInside(mouseData);

	ctp2_ListBox::SetMouseFocusListBox(this);
}

void ctp2_ListBox::MouseMoveOver( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	aui_ListBox::MouseMoveOver(mouseData);

	ctp2_ListBox::SetMouseFocusListBox(this);
}

void ctp2_ListBox::MouseMoveOutside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	aui_ListBox::MouseMoveOutside(mouseData);

}

void ctp2_ListBox::MouseMoveAway( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	aui_ListBox::MouseMoveAway(mouseData);

	ctp2_ListBox::SetMouseFocusListBox(NULL);
}

void ctp2_ListBox::MouseLDragOver( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	aui_ListBox::MouseLDragOver(mouseData);

	ctp2_ListBox::SetMouseFocusListBox(this);
}

void ctp2_ListBox::MouseLDragAway( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( GetMouseOwnership() == this )
	{
		if ( m_dragDropWindow )
			RemoveItem( m_dragDropWindow->GetDragDropItem()->Id() );

		CalculateScroll( mouseData->position.x, mouseData->position.y );


		ScrollList();
		DragSelect( mouseData->position.y - m_y );
		m_scrolling = TRUE;
		m_startWaitTime = mouseData->time;

		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		if(m_menuButton) {
			if(mouseData->position.y < Y()) {
				SetWhichSeesMouse(NULL);
				m_menuButton->MouseLGrabInside(mouseData);
			}
		}
	}

	ctp2_ListBox::SetMouseFocusListBox(NULL);
}

void ctp2_ListBox::MouseRDragOver( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	aui_ListBox::MouseRDragOver(mouseData);

	ctp2_ListBox::SetMouseFocusListBox(this);
}

void ctp2_ListBox::MouseRDragAway( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	aui_ListBox::MouseRDragAway(mouseData);

	ctp2_ListBox::SetMouseFocusListBox(NULL);
}

AUI_ERRCODE ctp2_ListBox::DoneInstantiatingThis(const MBCHAR *ldlBlock)
{
	aui_Ldl		*theLdl = g_ui->GetLdl();
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock(ldlBlock);
	Assert( block != NULL );

	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	m_borderOffset.x = block->GetInt(k_CTP2_LISTBOX_LDL_BORDER_WIDTH);
	m_borderOffset.y = block->GetInt(k_CTP2_LISTBOX_LDL_BORDER_HEIGHT);

	Resize(Width() + m_borderOffset.x * 2,
		   Height() + m_borderOffset.y * 2);
	Move(m_x - m_borderOffset.x,
		 m_y - m_borderOffset.y);
	m_headerOffset.x = m_borderOffset.x;
	m_pane->Move(m_pane->X() + m_borderOffset.x,
				 m_pane->Y() + m_borderOffset.y);
	return AUI_ERRCODE_OK;
}

void ctp2_ListBox::GetDisplayRange(sint32 &top, sint32 &bottom)
{
	top = m_verticalRanger->GetValueY();
	bottom = top + m_itemsPerHeight - 1;
	if(bottom > m_numRows)
		bottom = m_numRows;
}

void ctp2_ListBox::EnsureItemVisible(sint32 index)
{
	if(index < 0 || index > m_numRows)

		return;

	sint32 top, bottom;
	GetDisplayRange(top, bottom);
	if(top <= index && index <= bottom)

		return;

	if(index < top) {
		m_verticalRanger->SetValue(0, index);
	} else if(index > bottom) {
		sint32 newTop = index - m_itemsPerHeight + 1;
		m_verticalRanger->SetValue(0, newTop);
	}
}
