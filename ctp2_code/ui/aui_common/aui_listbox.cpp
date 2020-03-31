//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface element: list with rangers.
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
// - Always focus on the latest message.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Standardized code (May 21st 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_listbox.h"

#include "aui_ui.h"
#include "aui_blitter.h"
#include "aui_window.h"
#include "aui_dragdropwindow.h"
#include "aui_surface.h"
#include "aui_uniqueid.h"
#include "aui_item.h"
#include "aui_ranger.h"
#include "aui_header.h"
#include "aui_switch.h"
#include "aui_action.h"
#include "aui_ldl.h"
#include "aui_static.h"

#include "ctp2_listitem.h"
#include "ldl_data.hpp"
#include "ldl_file.hpp"

#ifdef USE_SDL
#include <SDL2/SDL.h>
#endif

aui_DragDropWindow *aui_ListBox::m_dragDropWindow = NULL;
aui_ListBox *aui_ListBox::ms_mouseFocusListBox = NULL;

aui_ListBox::aui_ListBox(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_Control( retval, id, ldlBlock, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRangersAndHeader( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

aui_ListBox::aui_ListBox(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_Control( retval, id, x, y, width, height, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRangersAndHeader( NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

AUI_ERRCODE aui_ListBox::InitCommonLdl( const MBCHAR *ldlBlock )
{
	ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	AUI_ERRCODE errcode = InitCommon();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	m_alwaysRanger = block->GetBool( k_AUI_LISTBOX_LDL_ALWAYSRANGER );

	m_headerOffset.x = block->GetInt(  k_AUI_LISTBOX_LDL_HEADEROFFSETX );
	m_headerOffset.y = block->GetInt(  k_AUI_LISTBOX_LDL_HEADEROFFSETY );
	m_verticalRangerOffset.x =
		block->GetInt(  k_AUI_LISTBOX_LDL_RANGERYOFFSETX );
	m_verticalRangerOffset.y =
		block->GetInt(  k_AUI_LISTBOX_LDL_RANGERYOFFSETY );
	m_horizontalRangerOffset.x =
		block->GetInt(  k_AUI_LISTBOX_LDL_RANGERXOFFSETX );
	m_horizontalRangerOffset.y =
		block->GetInt(  k_AUI_LISTBOX_LDL_RANGERXOFFSETY );
	m_itemWidth = block->GetInt( k_AUI_LISTBOX_LDL_ITEMWIDTH );
	m_itemHeight = block->GetInt( k_AUI_LISTBOX_LDL_ITEMHEIGHT );

	m_sendRightClicks = block->GetBool(k_AUI_LISTBOX_LDL_SENDRIGHTCLICKS) != FALSE;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_ListBox::InitCommon( void )
{
	m_header = NULL,
	m_verticalRanger = NULL,
	m_horizontalRanger = NULL,
	m_headerOffset.x = m_headerOffset.y = 0,
	m_verticalRangerOffset.x = m_verticalRangerOffset.y = 0,
	m_horizontalRangerOffset.x = m_horizontalRangerOffset.y = 0,
	m_itemWidth = 0,
	m_itemHeight = 0,
	m_rangerSize = k_CONTROL_DEFAULT_SIZE,
	m_alwaysRanger = FALSE,
	m_multiSelect = FALSE,
	m_forceSelect = FALSE,
	m_absorbEvents = TRUE,
	m_absorbed = FALSE,
	m_selectStatus = AUI_LISTBOX_SELECTSTATUS_NONE,
	m_dragIndex = -1,
	m_scrolling = FALSE,
	m_scrollDx = 0,
	m_scrollDy = 0,
	m_numRows = 0,
	m_numColumns = 0,
	m_maxItemWidth = 0,
	m_maxItemHeight = 0,
	m_itemsPerWidth = 0,
	m_itemsPerHeight = 0;
	m_sortColumn = -1;
	m_sortAscending = TRUE;
	m_ignoreOutsideDrops = false;
	m_keyboardActionControl = NULL;

	m_buildingTheList = FALSE;

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	m_pane =
		new aui_Static( &errcode, aui_UniqueId(), 0, 0, m_width, m_height );
	Assert( AUI_NEWOK(m_pane,errcode) );
	if ( !AUI_NEWOK(m_pane,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_pane->SetBlindness( TRUE );

	AddChild( m_pane );

	m_header = new aui_Header( &errcode, aui_UniqueId(), 0, 0, m_width, 0 );
	Assert( AUI_NEWOK(m_header,errcode) );
	if ( !AUI_NEWOK(m_header,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_header );

	m_widthList = new tech_WLList<sint32>;
	Assert( m_widthList != NULL );
	if ( !m_widthList ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_selectedList = new tech_WLList<sint32>;
	Assert( m_selectedList != NULL );
	if ( !m_selectedList ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_selectedListLastTime = new tech_WLList<sint32>;
	Assert( m_selectedListLastTime != NULL );
	if ( !m_selectedListLastTime ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_visualSelectedList = new tech_WLList<sint32>;
	Assert( m_visualSelectedList != NULL );
	if ( !m_visualSelectedList ) return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_ListBox::CreateRangersAndHeader( const MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

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

		if (aui_Ldl::GetLdl()->FindDataBlock(block))
			m_header = new aui_Header(
				&errcode,
				aui_UniqueId(),
				block );
	}

	if ( !m_header )
		m_header = new aui_Header(
			&errcode,
			aui_UniqueId(),
			0, 0, m_width, 0 );

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

		if (aui_Ldl::GetLdl()->FindDataBlock(block))
			m_verticalRanger = new aui_Ranger(
				&errcode,
				aui_UniqueId(),
				block,
				ListBoxRangerActionCallback,
				this );
	}

	if ( !m_verticalRanger )
		m_verticalRanger = new aui_Ranger(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0,
			AUI_RANGER_TYPE_SCROLLER,
			AUI_RANGER_ORIENTATION_VERTICAL,
			ListBoxRangerActionCallback,
			this );

	Assert( AUI_NEWOK(m_verticalRanger,errcode) );
	if ( !AUI_NEWOK(m_verticalRanger,errcode) )
		return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_verticalRanger );

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_LISTBOX_LDL_RANGERX );

		if (aui_Ldl::GetLdl()->FindDataBlock(block))
			m_horizontalRanger = new aui_Ranger(
				&errcode,
				aui_UniqueId(),
				block,
				ListBoxRangerActionCallback,
				this );
	}

	if ( !m_horizontalRanger )
		m_horizontalRanger = new aui_Ranger(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0,
			AUI_RANGER_TYPE_SCROLLER,
			AUI_RANGER_ORIENTATION_HORIZONTAL,
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

aui_ListBox::~aui_ListBox()
{
	delete m_pane;
	delete m_header;
	delete m_verticalRanger;
	delete m_horizontalRanger;
	delete m_widthList;
	delete m_selectedList;
	delete m_selectedListLastTime;
	delete m_visualSelectedList;

	if (m_dragDropWindow)
	{
		DestroyDragDropWindow(m_dragDropWindow);
	}
}

AUI_ERRCODE aui_ListBox::Draw(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if (!surface)
	{
		surface = m_window->TheSurface();
		if (!surface) return AUI_ERRCODE_OK;
	}

	AUI_ERRCODE errcode = DrawThis( surface, x, y );

	if ( errcode == AUI_ERRCODE_OK )
		DrawChildren( surface, x, y );

	m_draw = 0;

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	sint32 minVertical = m_verticalRanger->GetValueY();
	sint32 maxVertical = minVertical + m_itemsPerHeight;

	if ( maxVertical > m_numRows ) maxVertical = m_numRows;

	RECT selectRect = rect;

	ListPos position = m_visualSelectedList->GetHeadPosition();

		for ( sint32 i = m_visualSelectedList->L(); i; i-- )
		{
			sint32 itemIndex = m_visualSelectedList->GetNext( position );
			if ( minVertical <= itemIndex && itemIndex < maxVertical )
			{
				selectRect.top = rect.top +
					( itemIndex - minVertical ) * m_maxItemHeight;
				selectRect.bottom = selectRect.top + m_maxItemHeight;
				g_ui->TheBlitter()->BevelBlt(
					surface,
					&selectRect,
					&selectRect,
					2,
					0,
					0,
					k_AUI_BLITTER_FLAG_IN );
			}
		}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

BOOL aui_ListBox::IsItem( aui_Region *region )
{
	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = m_pane->ChildList()->L(); i; i-- )
	{
		aui_Region *item = m_pane->ChildList()->GetNext( position );
		if ( item == region ) return TRUE;

		ListPos childPos = item->ChildList()->GetHeadPosition();
		for ( sint32 j = item->ChildList()->L(); j; j-- )
		{
			aui_Region *childItem = item->ChildList()->GetNext( childPos );
			if ( childItem == region ) return TRUE;
		}
	}

	return FALSE;
}

aui_Item *aui_ListBox::GetSelectedItem( void ) const
{
	if ( m_selectedList->L() )
		return (aui_Item *)m_pane->ChildList()->GetAt(
			m_pane->ChildList()->FindIndex( m_selectedList->GetHead() ) );
	else
		return NULL;
}

sint32 aui_ListBox::GetSelectedItemIndex(void) const
{
	return m_selectedList->L() ? m_selectedList->GetHead() : -1;
}

BOOL aui_ListBox::SetMultiSelect( BOOL multiSelect )
{
	BOOL wasMultiSelect = m_multiSelect;
	m_multiSelect = multiSelect;
	return wasMultiSelect;
}

BOOL aui_ListBox::SetForceSelect( BOOL forceSelect )
{
	BOOL wasForceSelect = m_forceSelect;
	m_forceSelect = forceSelect;
	return wasForceSelect;
}

BOOL aui_ListBox::SetAbsorbancy( BOOL absorbEvents )
{
	BOOL wasAbsorbant = m_absorbEvents;
	m_absorbEvents = absorbEvents;
	return wasAbsorbant;
}

AUI_ERRCODE aui_ListBox::Resize( sint32 width, sint32 height )
{
	aui_Control::Resize( width, height );

	if (!m_buildingTheList) {
		CalculateDimensions();
		RepositionHeaderSwitches();
		RepositionRangers();
		RepositionItems();
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_ListBox::Show( void )
{
	aui_Control::Show();

	if (!m_buildingTheList) {
		RepositionHeaderSwitches();
		RepositionRangers();
		RepositionItems();
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_ListBox::AddItem( aui_Item *item )
{
	AUI_ERRCODE errcode = m_pane->AddChild( item );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	if ( m_itemWidth > 0 )
	{
		item->Resize( m_itemWidth, item->Height() );

		ListPos position = item->ChildList()->GetHeadPosition();
		for ( sint32 i = item->ChildList()->L(); i; i-- )
		{
			aui_Region *child = item->ChildList()->GetNext( position );
			child->Resize( m_itemWidth, child->Height() );
		}
	}

	if ( m_itemHeight > 0 )
	{
		item->Resize( item->Width(), m_itemHeight );

		ListPos position = item->ChildList()->GetHeadPosition();
		for ( sint32 i = item->ChildList()->L(); i; i-- )
		{
			aui_Region *child = item->ChildList()->GetNext( position );
			child->Resize( child->Width(), m_itemHeight );
		}
	}

	if ( m_forceSelect && !m_selectedList->L() )
		SelectItem( (sint32)0, 1 );

	if (!m_buildingTheList) {
		CalculateDimensions();
		RepositionHeaderSwitches();
		RepositionRangers();
		RepositionItems();

		SortByColumn( m_sortColumn, m_sortAscending );
	}

	return errcode;
}

AUI_ERRCODE aui_ListBox::InsertItem( aui_Item *item, sint32 index )
{
	AUI_ERRCODE errcode = m_pane->InsertChild( item, index );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	if ( m_itemWidth > 0 )
	{
		item->Resize( m_itemWidth, item->Height() );

		ListPos position = item->ChildList()->GetHeadPosition();
		for ( sint32 i = item->ChildList()->L(); i; i-- )
		{
			aui_Region *child = item->ChildList()->GetNext( position );
			child->Resize( m_itemWidth, child->Height() );
		}
	}

	if ( m_itemHeight > 0 )
	{
		item->Resize( item->Width(), m_itemHeight );

		ListPos position = item->ChildList()->GetHeadPosition();
		for ( sint32 i = item->ChildList()->L(); i; i-- )
		{
			aui_Region *child = item->ChildList()->GetNext( position );
			child->Resize( child->Width(), m_itemHeight );
		}
	}

	if ( m_forceSelect && !m_selectedList->L() )
		SelectItem( (sint32)0, 1 );

	ListPos position = m_selectedList->GetHeadPosition();
	sint32	i;
	for ( i = m_selectedList->L(); i; i-- )
	{
		ListPos prevPosition = position;
		sint32 curIndex = m_selectedList->GetNext( position );
		if ( curIndex >= index )
			m_selectedList->SetAt( prevPosition, curIndex + 1 );
	}

	position = m_selectedListLastTime->GetHeadPosition();
	for ( i = m_selectedListLastTime->L(); i; i-- )
	{
		ListPos prevPosition = position;
		sint32 curIndex = m_selectedListLastTime->GetNext( position );
		if ( curIndex >= index )
			m_selectedListLastTime->SetAt( prevPosition, curIndex + 1 );
	}

	position = m_visualSelectedList->GetHeadPosition();
	for ( i = m_visualSelectedList->L(); i; i-- )
	{
		ListPos prevPosition = position;
		sint32 curIndex = m_visualSelectedList->GetNext( position );
		if ( curIndex >= index )
			m_visualSelectedList->SetAt( prevPosition, curIndex + 1 );
	}

	if (!m_buildingTheList) {
		CalculateDimensions();
		RepositionHeaderSwitches();
		RepositionRangers();
		RepositionItems();

		SortByColumn( m_sortColumn, m_sortAscending );
	}

	return errcode;
}

AUI_ERRCODE aui_ListBox::RemoveItem( uint32 itemId )
{
	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = 0; i < (sint32)m_pane->ChildList()->L(); i++ )
	{
		ListPos prevPosition = position;
		aui_Item *item = (aui_Item *)m_pane->ChildList()->GetNext( position );
		if ( item->Id() == itemId )
		{

			m_pane->ChildList()->DeleteAt( prevPosition );

			ListPos selectedPosition = m_selectedList->Find( i );
			if ( selectedPosition )
				m_selectedList->DeleteAt( selectedPosition );

			selectedPosition = m_selectedList->GetHeadPosition();
			for ( sint32 j = m_selectedList->L(); j; j-- )
			{
				ListPos prevSelectedPosition = selectedPosition;
				sint32 index = m_selectedList->GetNext( selectedPosition );
				if ( index > i )
					m_selectedList->SetAt( prevSelectedPosition, index - 1 );
			}

			if ( m_forceSelect
			&&   !m_selectedList->L()
			&&   m_pane->ChildList()->L() )
			{
				if ( i < (sint32)m_pane->ChildList()->L() )
					m_selectedList->AddHead( i );
				else
					m_selectedList->AddHead( i ? i - 1 : 0 );
			}

			m_selectedListLastTime->DeleteAll();
			m_selectedListLastTime->AddTail( *m_selectedList );

			m_visualSelectedList->DeleteAll();
			m_visualSelectedList->AddTail( *m_selectedList );

			break;
		}
	}

	if (!m_buildingTheList) {
		CalculateDimensions();
		RepositionHeaderSwitches();
		RepositionRangers();
		RepositionItems();

		SortByColumn( m_sortColumn, m_sortAscending );
	}

	return AUI_ERRCODE_OK;
}

void aui_ListBox::RemoveItems( BOOL destroy, BOOL destroyAction )
{
	for ( sint32 i = m_pane->ChildList()->L(); i; i-- )
	{
		aui_Item *item = (aui_Item *)m_pane->ChildList()->GetHead();
		RemoveItem( item->Id() );
		if ( destroyAction && item->GetAction() ) delete item->GetAction();
		if ( destroy ) delete item;
	}
}

AUI_ERRCODE aui_ListBox::RemoveItemByIndex( sint32 index )
{
	aui_Item *item = GetItemByIndex( index );
	Assert( item != NULL );
	return item ? RemoveItem( item->Id() ) : AUI_ERRCODE_INVALIDPARAM;
}

aui_Item *aui_ListBox::GetItemByIndex( sint32 index )
{
	Assert( index >= 0 && index < (sint32)m_pane->ChildList()->L() );
	if ( index < 0 || index >= (sint32)m_pane->ChildList()->L() )
		return NULL;

	ListPos position = m_pane->ChildList()->FindIndex( index );
	if ( !position ) return NULL;

	return (aui_Item *)m_pane->ChildList()->GetAt( position );
}

AUI_ERRCODE aui_ListBox::AddHeaderSwitch( aui_Switch *theSwitch )
{
	Assert( theSwitch != NULL );
	if ( !theSwitch ) return AUI_ERRCODE_INVALIDPARAM;

	m_header->AddChild( theSwitch );
	m_widthList->AddTail( theSwitch->Width() );

	if (!m_buildingTheList) {
		CalculateDimensions();
		RepositionHeaderSwitches();
		RepositionRangers();
		RepositionItems();
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_ListBox::RemoveHeaderSwitch( uint32 switchId )
{
	aui_Switch *theSwitch = (aui_Switch *)m_header->GetChild( switchId );
	if ( !theSwitch ) return AUI_ERRCODE_OK;

	ListPos widthPosition = m_widthList->GetHeadPosition();
	ListPos position = m_header->ChildList()->GetHeadPosition();
	for ( sint32 i = m_header->ChildList()->L(); i; i-- )
	{
		ListPos prevWidthPosition = widthPosition;
		m_widthList->GetNext( widthPosition );

		if ( m_header->ChildList()->GetNext( position ) == theSwitch )
		{
			m_widthList->DeleteAt( prevWidthPosition );
			break;
		}
	}

	m_header->RemoveChild( switchId );

	if (!m_buildingTheList) {
		CalculateDimensions();
		RepositionHeaderSwitches();
		RepositionRangers();
		RepositionItems();
	}

	return AUI_ERRCODE_OK;
}

aui_Switch *aui_ListBox::GetHeaderSwitchByIndex( sint32 index )
{
	Assert( index >= 0 && index < (sint32)m_header->ChildList()->L() );
	if ( index < 0 || index >= (sint32)m_header->ChildList()->L() )
		return NULL;

	ListPos position = m_header->ChildList()->FindIndex( index );
	if ( !position ) return NULL;

	return (aui_Switch *)m_header->ChildList()->GetAt( position );
}

AUI_ERRCODE aui_ListBox::RangerMoved( void )
{

	if (!m_buildingTheList) {
		RepositionHeaderSwitches();
		RepositionItems();
	}

	return AUI_ERRCODE_OK;
}

sint32 aui_ListBox::ItemsPerWidth( sint32 column )
{
	sint32 itemsPerWidth = 0;
	sint32 width = 0;

	ListPos position = m_widthList->FindIndex( column );
	for ( sint32 i = m_numColumns - column; i > 0; i-- )
	{
		if ( (width += position ?
				m_widthList->GetNext( position ) :
				m_maxItemWidth) <= m_width )
			itemsPerWidth++;
		else
			break;
	}

	return itemsPerWidth ? itemsPerWidth : m_itemsPerWidth;
}

sint32 aui_ListBox::ColumnWidth( sint32 column )
{
	ListPos position = m_widthList->FindIndex( column );
	if ( position )
		return m_widthList->GetAt( position );
	else
		return m_maxItemWidth;
}

sint32 aui_ListBox::HorizontalRangerPositionCount( void )
{
	sint32 width = 0;
	sint32 count = 0;

	sint32 i;
	for(
		i = m_numColumns - m_widthList->L();
		i > 0 && (width += m_maxItemWidth) <= m_width;
		i-- )
			count++;

	ListPos position = m_widthList->GetTailPosition();
	for(
		sint32 j = m_widthList->L();
		j && (width += m_widthList->GetPrev( position )) <= m_width;
		j-- )
			count++;

	sint32 columns = m_numColumns ? m_numColumns : m_widthList->L();
	if ( !columns ) columns = count;

	return columns - count;
}

AUI_ERRCODE aui_ListBox::CalculateDimensions( void )
{

	m_maxItemWidth = 0;
	m_maxItemHeight = 0;

	m_numColumns = m_header->ChildList()->L();

	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = m_numRows = m_pane->ChildList()->L(); i; i-- )
	{
		aui_Item *item = (aui_Item *)m_pane->ChildList()->GetNext( position );

		if ( item->Width() > m_maxItemWidth )
			m_maxItemWidth = item->Width();
		if ( item->Height() > m_maxItemHeight )
			m_maxItemHeight = item->Height();

		sint32 j = item->ChildList()->L();
		if ( j + 1 > m_numColumns ) m_numColumns = j + 1;

		ListPos subPosition = item->ChildList()->GetHeadPosition();
		for ( j; j; j-- )
		{
			aui_Item *subItem =
				(aui_Item *)item->ChildList()->GetNext( subPosition );

			if ( subItem->Width() > m_maxItemWidth )
				m_maxItemWidth = subItem->Width();
			if ( subItem->Height() > m_maxItemHeight )
				m_maxItemHeight = subItem->Height();
		}
	}

	m_itemsPerWidth = m_maxItemWidth ? m_width / m_maxItemWidth : 0;
	m_itemsPerHeight = m_maxItemHeight ? m_height / m_maxItemHeight : 0;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_ListBox::RepositionItems( void )
{
	sint32 minVertical = m_verticalRanger->GetValueY();
	sint32 maxVertical = minVertical + m_itemsPerHeight;

	if ( maxVertical > m_numRows ) maxVertical = m_numRows;

	Assert( m_pane );
	if ( !m_pane ) return AUI_ERRCODE_INVALIDPARAM;

	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = 0; i < m_numRows; i++ )
	{
		aui_Item *item = (aui_Item *)m_pane->ChildList()->GetNext( position );
		if ( minVertical <= i && i < maxVertical )
		{
			if ( !IsHidden() )
				item->Show();
			item->Move( 0, ( i - minVertical ) * m_maxItemHeight );

			if ( m_widthList->L() )
				item->Resize( m_widthList->GetHead(), item->Height() );

			sint32 minHorizontal =
				m_horizontalRanger->GetValueX();
			sint32 maxHorizontal =
				minHorizontal + ItemsPerWidth( minHorizontal );

			sint32 x = 0;

			if ( minHorizontal > 0 )
				item->HideThis();
			else
				x = ColumnWidth( 0 );

			ListPos subPosition = item->ChildList()->GetHeadPosition();
			for ( sint32 j = 1; j < m_numColumns; j++ )
			{

				if ( !subPosition ) break;

				aui_Item *subItem =
					(aui_Item *)item->ChildList()->GetNext( subPosition );
				if ( minHorizontal <= j && j < maxHorizontal )
				{
					subItem->Move( x, 0 );

					ListPos widthPosition = m_widthList->FindIndex( j );
					if ( widthPosition )
						subItem->Resize(
							m_widthList->GetAt( widthPosition ),
							subItem->Height() );

					x += ColumnWidth( j );
				}
				else
					subItem->Hide();
			}
		}
		else
		{
			item->Hide();
		}
	}

	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_ListBox::RepositionHeaderSwitches( void )
{
	m_header->Move( m_headerOffset.x, m_headerOffset.y - m_header->Height() );

	if ( !IsHidden() ) m_header->Show();

	sint32 minHorizontal = m_horizontalRanger->GetValueX();
	sint32 maxHorizontal = minHorizontal + ItemsPerWidth( minHorizontal );

	sint32 x = 0;
	ListPos widthPosition = m_widthList->GetHeadPosition();
	ListPos position = m_header->ChildList()->GetHeadPosition();
	for ( sint32 i = 0; i < m_numColumns; i++ )
	{

		if ( !position ) break;

		sint32 width = m_widthList->GetNext( widthPosition );

		aui_Switch *theSwitch =
			(aui_Switch *)m_header->ChildList()->GetNext( position );

		if ( minHorizontal <= i && i < maxHorizontal )
		{
			theSwitch->Move(
				x,
				m_header->Height() - theSwitch->Height() );

			theSwitch->Resize(
				width,
				theSwitch->Height() );

			x += width;
		}
		else
			theSwitch->Hide();
	}


	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_ListBox::RepositionRangers( void )
{
	sint32 rangerDimension;

	rangerDimension =
		m_width - 2 * m_horizontalRanger->GetButtonSize();
	if ( rangerDimension < 0 ) rangerDimension = 0;

	m_horizontalRanger->Resize(
		rangerDimension,
		m_rangerSize );

	m_horizontalRanger->Move(
		m_horizontalRanger->GetButtonSize(),
		m_height );
	m_horizontalRanger->Offset(
		m_horizontalRangerOffset.x,
		m_horizontalRangerOffset.y );

	sint32 itemsPerWidth = ItemsPerWidth( m_horizontalRanger->GetValueX() );
	if ( !itemsPerWidth ) itemsPerWidth = m_itemsPerWidth;
	sint32 newMax = HorizontalRangerPositionCount();

	m_horizontalRanger->SetMaximum( newMax, 0 );
	m_horizontalRanger->SetIncrement( 1, 0 );
	m_horizontalRanger->SetPage(
		itemsPerWidth ? itemsPerWidth : 1,
		0 );

	if ( newMax && !IsHidden() )
	{
		if ( m_horizontalRanger->IsHidden() )
			m_horizontalRanger->Show();
	}
	else
	{
		if ( !m_horizontalRanger->IsHidden() )
		{
			m_horizontalRanger->Hide();
			if ( m_parent ) m_parent->ShouldDraw();
		}
	}

	sint32 headerHeight = 0;

	rangerDimension =
		m_height - 2 * m_verticalRanger->GetButtonSize() + headerHeight;
	if ( rangerDimension < 0 ) rangerDimension = 0;

	m_verticalRanger->Resize(
		m_rangerSize,
		rangerDimension );

	m_verticalRanger->Move(
		m_width,
		m_verticalRanger->GetButtonSize() - headerHeight );
	m_verticalRanger->Offset(
		m_verticalRangerOffset.x,
		m_verticalRangerOffset.y );

	newMax = m_itemsPerHeight ?
		m_numRows - m_itemsPerHeight :
		0;
	if ( newMax < 0 ) newMax = 0;

	m_verticalRanger->SetMaximum( 0, newMax );
	m_verticalRanger->SetIncrement( 0, 1 );
	m_verticalRanger->SetPage(
		0,
		m_itemsPerHeight ? m_itemsPerHeight : 1 );

	if ( !IsHidden() && ( m_alwaysRanger || newMax ) )
	{
		if ( m_verticalRanger->IsHidden() )
			m_verticalRanger->Show();
	}
	else
	{
		if ( !m_verticalRanger->IsHidden() )
		{
			m_verticalRanger->Hide();
			if ( m_parent ) m_parent->ShouldDraw();
		}
	}

	return AUI_ERRCODE_OK;
}

aui_DragDropWindow *aui_ListBox::CreateDragDropWindow( aui_Control *dragDropItem )
{
	if ( !IsDragDrop() ) return NULL;

	POINT itemPoint =
	{
		m_window->X() + dragDropItem->X(),
		m_window->Y() + dragDropItem->Y()
	};
	dragDropItem->ToWindow( &itemPoint );

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	aui_DragDropWindow *ddw = new aui_DragDropWindow(
		&errcode,
		dragDropItem,
		this,
		itemPoint.x,
		itemPoint.y,
		dragDropItem->Width(),
		dragDropItem->Height() );
	Assert( AUI_NEWOK(ddw,errcode) );
	if ( !AUI_NEWOK(ddw,errcode) ) return NULL;

	g_ui->AddChild( ddw );

	return ddw;
}

AUI_ERRCODE aui_ListBox::CalculateScroll( sint32 x, sint32 y )
{

	if ( x < m_x )
		m_scrollDx = -1;
	else if ( x >= m_x + m_width )
		m_scrollDx = 1;
	else
		m_scrollDx = 0;

	if ( y < m_y )
		m_scrollDy = -1;
	else if ( y >= m_y + m_height )
		m_scrollDy = 1;
	else
		m_scrollDy = 0;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_ListBox::ScrollList( void )
{
	if ( m_dragDropWindow ) return AUI_ERRCODE_OK;

	if ( m_scrollDx == 1 )
		m_horizontalRanger->IncrementUpX();
	else if ( m_scrollDx == -1 )
		m_horizontalRanger->IncrementDownX();

	if ( m_scrollDy == 1 )
		m_verticalRanger->IncrementUpY();
	else if ( m_scrollDy == -1 )
		m_verticalRanger->IncrementDownY();

	return AUI_ERRCODE_OK;
}

void aui_ListBox::SendSelectCallback(
	AUI_LISTBOX_ACTION action,
	uint32 data )
{
	if ( !data && (action == AUI_LISTBOX_ACTION_SELECT
		|| action == AUI_LISTBOX_ACTION_RMOUSESELECT))
		data = (uint32)m_selectedList; // Something you should not do
		                               // It seems that we just want to have something that is not zero, but that is not clear

	m_selectedList->DeleteAll();

	m_selectedList->AddTail( *m_visualSelectedList );

	if ( m_ActionFunc ) {
		m_ActionFunc(
			this,
			action,
			data,
			m_cookie );
	} else {
		if ( m_action ) {
			m_action->Execute(
				this,
				action,
				data );
		}
	}

	m_selectedListLastTime->DeleteAll();
	m_selectedListLastTime->AddTail( *m_selectedList );
}

void aui_ListBox::WhatsChanged(
	tech_WLList<sint32> &selectedList,
	tech_WLList<sint32> &deselectedList )
{
	selectedList.DeleteAll();
	deselectedList.DeleteAll();

	ListPos position = m_selectedList->GetHeadPosition();
	sint32 i;
	for ( i = m_selectedList->L(); i; i-- )
	{
		sint32 itemIndex = m_selectedList->GetNext( position );
		if ( !m_selectedListLastTime->Find( itemIndex ) )
			selectedList.AddTail( itemIndex );
	}

	position = m_selectedListLastTime->GetHeadPosition();
	for ( sint32 j = m_selectedListLastTime->L(); j; j-- )
	{
		sint32 itemIndex = m_selectedListLastTime->GetNext( position );
		if ( !m_selectedList->Find( itemIndex ) )
			deselectedList.AddTail( itemIndex );
	}
}

AUI_ERRCODE aui_ListBox::DragSelect( sint32 y )
{
	if ( m_dragDropWindow ) return AUI_ERRCODE_OK;

	sint32 maxY = m_itemsPerHeight;
	if ( maxY > m_numRows ) maxY = m_numRows;
	maxY *= m_maxItemHeight;

	sint32 itemIndex = 0;
	if ( y < 0 )
		itemIndex = m_verticalRanger->GetValueY();
	else if ( y < maxY )
		itemIndex = y / m_maxItemHeight + m_verticalRanger->GetValueY();
	else
	{
		itemIndex = m_itemsPerHeight + m_verticalRanger->GetValueY() - 1;
		if ( itemIndex >= m_numRows ) itemIndex = m_numRows - 1;
	}

	ListPos position = m_visualSelectedList->Find( itemIndex );
	if ( position )
	{
		if ( m_selectStatus == AUI_LISTBOX_SELECTSTATUS_DESELECTING )
		{
			if ( m_multiSelect )
			{
				ListPos pos;
				if ( m_dragIndex < itemIndex )
				{
					for ( sint32 i = m_dragIndex; i <= itemIndex; i++ )
						if ((pos = m_visualSelectedList->Find( i )))
							m_visualSelectedList->DeleteAt( pos );
				}
				else
				{
					for ( sint32 i = itemIndex; i <= m_dragIndex; i++ )
						if ((pos = m_visualSelectedList->Find( i )))
							m_visualSelectedList->DeleteAt( pos );
				}
			}
			else
				m_visualSelectedList->DeleteAt( position );
		}
	}
	else
	{
		if (!m_forceSelect && !m_multiSelect && m_visualSelectedList->L() )
			m_visualSelectedList->RemoveHead();

		if ( m_selectStatus == AUI_LISTBOX_SELECTSTATUS_SELECTING )
		{
			if ( m_multiSelect )
			{
				if ( m_dragIndex < itemIndex )
				{
					for ( sint32 i = m_dragIndex; i <= itemIndex; i++ )
						if ( !m_visualSelectedList->Find( i ) )
							m_visualSelectedList->AddTail( i );
				}
				else
				{
					for ( sint32 i = itemIndex; i <= m_dragIndex; i++ )
						if ( !m_visualSelectedList->Find( i ) )
							m_visualSelectedList->AddTail( i );
				}
			}
			else
				m_visualSelectedList->AddTail( itemIndex );
		}
	}

	m_dragIndex = itemIndex;

	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
		m_mouseCode = AUI_ERRCODE_HANDLED;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_ListBox::SelectItem( sint32 index, uint32 data )
{
	if ( index < 0 || index >= (sint32)m_pane->ChildList()->L() )
		return AUI_ERRCODE_INVALIDPARAM;

	if ( m_multiSelect )
	{
		m_selectedList->AddTail( index );
		m_selectedList->RemoveDuplicates();
	}
	else
	{
		if ( m_selectedList->L() )
			m_selectedList->SetAt( m_selectedList->GetHeadPosition(), index );
		else
			m_selectedList->AddHead( index );
	}

	m_visualSelectedList->DeleteAll();
	m_visualSelectedList->AddTail( *m_selectedList );

	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	SendSelectCallback( AUI_LISTBOX_ACTION_SELECT, data );

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_ListBox::SelectItem( aui_Item *item, uint32 data )
{
	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = 0; i < m_numRows; i++ )
		if ( m_pane->ChildList()->GetNext( position ) == item )
			return SelectItem( i, data );

	return AUI_ERRCODE_INVALIDPARAM;
}

AUI_ERRCODE aui_ListBox::DeselectItem( sint32 index, uint32 data )
{
	ListPos position = m_selectedList->Find( index );
	if ( position )
	{
		m_selectedList->DeleteAt( position );

		if ( m_forceSelect && !m_selectedList->L() && m_pane->ChildList()->L() )
		{
			if ( index < (sint32)m_pane->ChildList()->L() )
				m_selectedList->AddHead( index );
			else
				m_selectedList->AddHead( index ? index - 1 : 0 );
		}

		m_visualSelectedList->DeleteAll();
		m_visualSelectedList->AddTail( *m_selectedList );

		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

		SendSelectCallback( AUI_LISTBOX_ACTION_SELECT, data );
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_ListBox::DeselectItem( aui_Item *item, uint32 data )
{
	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = 0; i < m_numRows; i++ )
		if ( m_pane->ChildList()->GetNext( position ) == item )
			return DeselectItem( i, data );

	return AUI_ERRCODE_INVALIDPARAM;
}

AUI_ERRCODE aui_ListBox::StartSelecting( void )
{
	SetMouseOwnership();
	SetKeyboardFocus();
	m_selectStatus = AUI_LISTBOX_SELECTSTATUS_SELECTING;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_ListBox::SwitchItems( sint32 indexA, sint32 indexB )
{
	Assert( 0 <= indexA && indexA < m_numRows );
	Assert( 0 <= indexB && indexB < m_numRows );

	if ( 0 > indexA || indexA >= m_numRows ) return AUI_ERRCODE_INVALIDPARAM;
	if ( 0 > indexB || indexB >= m_numRows ) return AUI_ERRCODE_INVALIDPARAM;

	if ( indexA == indexB ) return AUI_ERRCODE_OK;

	ListPos positionA = m_pane->ChildList()->FindIndex( indexA );
	ListPos positionB = m_pane->ChildList()->FindIndex( indexB );

	aui_Item *itemA = (aui_Item *)m_pane->ChildList()->GetAt( positionA );
	aui_Item *itemB = (aui_Item *)m_pane->ChildList()->GetAt( positionB );

	m_pane->ChildList()->SetAt( positionA, itemB );
	m_pane->ChildList()->SetAt( positionB, itemA );

	ListPos selectedPositionA = m_selectedList->Find( indexA );
	ListPos selectedPositionB = m_selectedList->Find( indexB );
	if ( selectedPositionA )
		m_selectedList->SetAt( selectedPositionA, indexB );
	if ( selectedPositionB )
		m_selectedList->SetAt( selectedPositionB, indexA );

	selectedPositionA = m_visualSelectedList->Find( indexA );
	selectedPositionB = m_visualSelectedList->Find( indexB );
	if ( selectedPositionA )
		m_visualSelectedList->SetAt( selectedPositionA, indexB );
	if ( selectedPositionB )
		m_visualSelectedList->SetAt( selectedPositionB, indexA );

	selectedPositionA = m_selectedListLastTime->Find( indexA );
	selectedPositionB = m_selectedListLastTime->Find( indexB );
	if ( selectedPositionA )
		m_selectedListLastTime->SetAt( selectedPositionA, indexB );
	if ( selectedPositionB )
		m_selectedListLastTime->SetAt( selectedPositionB, indexA );

	return AUI_ERRCODE_OK;
}

void aui_ListBox::PreChildrenCallback( aui_MouseEvent *mouseData )
{
	if ( m_absorbEvents )
	{
		m_absorbed = TRUE;

		ListPos position = m_pane->ChildList()->GetHeadPosition();
		for ( sint32 i = m_pane->ChildList()->L(); i; i-- )
		{
			aui_Item *item =
				(aui_Item *)m_pane->ChildList()->GetNext( position );
			item->IgnoreEvents( TRUE );
		}
	}
}

void aui_ListBox::PostChildrenCallback( aui_MouseEvent *mouseData )
{
	if ( !m_dragDropWindow )
	if ( m_scrolling && mouseData->time - m_startWaitTime > m_timeOut )
	if ( mouseData->time - m_lastRepeatTime > m_repeatTime )
	{
		m_lastRepeatTime = mouseData->time;

		ScrollList();
		DragSelect( mouseData->position.y - m_y );

		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;
	}

	if ( m_absorbed )
	{
		m_absorbed = FALSE;

		ListPos position = m_pane->ChildList()->GetHeadPosition();
		for ( sint32 i = m_pane->ChildList()->L(); i; i-- )
		{
			aui_Item *item =
				(aui_Item *)m_pane->ChildList()->GetNext( position );
			item->IgnoreEvents( FALSE );
		}
	}
}

void aui_ListBox::ForceScroll(sint32 deltaX, sint32 deltaY)
{
	m_scrollDx = 0;
	m_scrollDy = (deltaY == 0) ? 0 : ((deltaY < 0) ? -1 : 1);

	ScrollList();
}

void aui_ListBox::MouseMoveInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( GetWhichSeesMouse() && GetWhichSeesMouse() != this && !IsItem( GetWhichSeesMouse() ) )
		MouseMoveAway( mouseData );
	else if ( !IsActive() )
		MouseMoveOver( mouseData );

	aui_ListBox::SetMouseFocusListBox(this);
}

void aui_ListBox::MouseMoveAway( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	aui_Control::MouseMoveAway(mouseData);

	aui_ListBox::SetMouseFocusListBox(NULL);
}

void aui_ListBox::MouseMoveOver( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this || IsItem( GetWhichSeesMouse() ) )
	{
		if ( !GetWhichSeesMouse() )
			SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ACTIVATE );

		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSEMOVEOVER;

		if ( m_ActionFunc )
			m_ActionFunc( this, 0, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, 0, 0 );
	}
	else
	{
		MouseMoveOutside( mouseData );
	}

	aui_ListBox::SetMouseFocusListBox(this);
}

void aui_ListBox::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

		HideTipWindow();

		SetMouseOwnership();
		SetKeyboardFocus();

		sint32 y = mouseData->position.y - m_y;

		sint32 maxY = m_itemsPerHeight;
		if ( maxY > m_numRows ) maxY = m_numRows;
		maxY *= m_maxItemHeight;

		if ( y < maxY )
		{
			sint32 itemIndex =
				y / (m_maxItemHeight != 0 ? m_maxItemHeight : 1) + m_verticalRanger->GetValueY();

			ListPos position = m_selectedList->Find( itemIndex );
			if ( position )
			{
				if ( m_forceSelect && m_selectedList->L() == 1 )
				{
					m_selectStatus = AUI_LISTBOX_SELECTSTATUS_SELECTING;
				}
				else
				{
					if (m_multiSelect) {
						if (!(mouseData->flags & k_MOUSE_EVENT_FLAG_LCONTROL
								|| mouseData->flags & k_MOUSE_EVENT_FLAG_RCONTROL)) {

							while (m_visualSelectedList->L() > 0)
								m_visualSelectedList->RemoveHead();

							m_selectStatus = AUI_LISTBOX_SELECTSTATUS_SELECTING;

						} else {

							m_selectStatus = AUI_LISTBOX_SELECTSTATUS_DESELECTING;

							m_visualSelectedList->DeleteAt(
								m_visualSelectedList->Find( itemIndex ) );
						}
					}  else {

					}
				}
			}
			else
			{
				m_selectStatus = AUI_LISTBOX_SELECTSTATUS_SELECTING;

				if ( !m_multiSelect
					|| (m_multiSelect
						&& !(mouseData->flags & k_MOUSE_EVENT_FLAG_LCONTROL
							|| mouseData->flags & k_MOUSE_EVENT_FLAG_RCONTROL
							|| mouseData->flags & k_MOUSE_EVENT_FLAG_LSHIFT
							|| mouseData->flags & k_MOUSE_EVENT_FLAG_RSHIFT))
						&& m_visualSelectedList->L() ) {

					while (m_visualSelectedList->L() > 0)
						m_visualSelectedList->RemoveHead();
				}
				if (m_multiSelect
					&& (mouseData->flags & k_MOUSE_EVENT_FLAG_LSHIFT
						|| mouseData->flags & k_MOUSE_EVENT_FLAG_RSHIFT)) {

					sint32		firstIndex = 999999,
								index;

					sint32 i;
					for (i=0; i<(sint32)(m_visualSelectedList->L()); i++) {
						index = m_visualSelectedList->GetAtIndex(i);
						if (index <= firstIndex) firstIndex = index;
					}
					if (firstIndex != 999999 && firstIndex != itemIndex) {
						if (firstIndex < itemIndex) {
							for (sint32 i=firstIndex; i<itemIndex; i++) {
								m_visualSelectedList->AddTail(i);
							}
						} else {
							for (sint32 i=itemIndex+1; i<firstIndex; i++) {
								m_visualSelectedList->AddTail(i);
							}
						}
					}
				}

				m_visualSelectedList->AddTail( itemIndex );
				m_visualSelectedList->RemoveDuplicates();
			}

			if ( IsDragDrop() )
			{
				if ( m_dragDropWindow ) DestroyDragDropWindow( m_dragDropWindow );

				aui_Item *item = GetItemByIndex( itemIndex );
				if ( item && (m_dragDropWindow = CreateDragDropWindow( item )) )
				{
					m_dragDropWindow->StartDragging(
						mouseData->position.x - m_x - item->X(),
						y - item->Y() );
				}

				SendSelectCallback();
			}

			m_dragIndex = itemIndex;

			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE;
		} else {

			if(!m_forceSelect) {
				for(sint32 i = m_selectedList->L() - 1; i >= 0; i--) {
					DeselectItem(m_selectedList->GetAtIndex(0));
				}
			}
		}

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;
	}
	else
		MouseLGrabOutside( mouseData );
}

void aui_ListBox::MouseRGrabInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

		HideTipWindow();

		SetMouseOwnership();
		SetKeyboardFocus();

		sint32 y = mouseData->position.y - m_y;

		sint32 maxY = m_itemsPerHeight;
		if ( maxY > m_numRows ) maxY = m_numRows;
		maxY *= m_maxItemHeight;

		if ( y < maxY )
		{
			sint32 itemIndex =
				y / m_maxItemHeight + m_verticalRanger->GetValueY();

			ListPos position = m_selectedList->Find( itemIndex );
			if ( position )
			{
				while (m_visualSelectedList->L() > 0)
					m_visualSelectedList->RemoveHead();
			}

			m_selectStatus = AUI_LISTBOX_SELECTSTATUS_SELECTING;

			if(!m_multiSelect) {
				while (m_visualSelectedList->L() > 0)
					m_visualSelectedList->RemoveHead();
			}

			m_visualSelectedList->AddTail( itemIndex );

			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSERGRABINSIDE;
		}

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;
	}
	else
		MouseRGrabOutside( mouseData );
}

void aui_ListBox::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		if ( m_dragDropWindow )
		{
			AddItem( (aui_Item *)m_dragDropWindow->GetDragDropItem() );
			DestroyDragDropWindow( m_dragDropWindow );
			m_dragDropWindow = NULL;
		}

		if ( GetMouseOwnership() == this )
		{
			ReleaseMouseOwnership();

			PlaySound( AUI_SOUNDBASE_SOUND_EXECUTE );

			SendSelectCallback();

			m_selectStatus = AUI_LISTBOX_SELECTSTATUS_NONE;

			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;
		}
		else
		{
			PlaySound( AUI_SOUNDBASE_SOUND_ACTIVATE );

			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;

			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE;
		}
	}
	else if ( !IsItem( GetWhichSeesMouse() ) )
		MouseLDropOutside( mouseData );
}

void aui_ListBox::MouseRDropInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		if ( GetMouseOwnership() == this )
		{
			ReleaseMouseOwnership();

			PlaySound( AUI_SOUNDBASE_SOUND_EXECUTE );

			if(!m_sendRightClicks) {


				SendSelectCallback();
			} else {
				SendSelectCallback(AUI_LISTBOX_ACTION_RMOUSESELECT);
			}

			m_selectStatus = AUI_LISTBOX_SELECTSTATUS_NONE;

			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;
		}
		else
		{
			PlaySound( AUI_SOUNDBASE_SOUND_ACTIVATE );

			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;

			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSERDROPINSIDE;
		}

	}
	else if ( !IsItem( GetWhichSeesMouse() ) )
		MouseRDropOutside( mouseData );
}

void aui_ListBox::MouseLDropOutside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	m_scrolling = FALSE;

	if ( GetMouseOwnership() == this )
	{
		ReleaseMouseOwnership();

		m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPOUTSIDE;

		if(!m_ignoreOutsideDrops) {
			SendSelectCallback();
		}
	}
	else if ( !GetMouseOwnership() )
	{
		m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPOUTSIDE;
	}

	m_selectStatus = AUI_LISTBOX_SELECTSTATUS_NONE;

	if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
		m_mouseCode = AUI_ERRCODE_HANDLED;
}

void aui_ListBox::MouseLDragAway( aui_MouseEvent *mouseData )
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
	}

	aui_ListBox::SetMouseFocusListBox(NULL);
}

void aui_ListBox::MouseRDragOver( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	aui_Control::MouseRDragOver(mouseData);

	aui_ListBox::SetMouseFocusListBox(this);
}

void aui_ListBox::MouseLDragOver( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	m_scrolling = FALSE;

	if ( GetMouseOwnership() == this )
		DragSelect( mouseData->position.y - m_y );

	aui_ListBox::SetMouseFocusListBox(this);
}

void aui_ListBox::MouseRDragAway( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	aui_Control::MouseRDragAway(mouseData);

	aui_ListBox::SetMouseFocusListBox(NULL);
}

void aui_ListBox::MouseLDragInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( GetMouseOwnership() == this )
		DragSelect( mouseData->position.y - m_y );
}

void aui_ListBox::MouseLDragOutside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( GetMouseOwnership() == this )
	{
		CalculateScroll( mouseData->position.x, mouseData->position.y );

		DragSelect( mouseData->position.y - m_y );
	}
}

void aui_ListBox::MouseLDoubleClickInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		if ( !GetWhichSeesMouse() )
			SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_EXECUTE );

		sint32 y = mouseData->position.y - m_y;

		sint32 maxY = m_itemsPerHeight;
		if ( maxY > m_numRows ) maxY = m_numRows;
		maxY *= m_maxItemHeight;

		if ( y < maxY )
		{
			sint32 itemIndex =
				y / m_maxItemHeight + m_verticalRanger->GetValueY();

			SendSelectCallback(
				AUI_LISTBOX_ACTION_DOUBLECLICKSELECT,
				(uint32)itemIndex );
		}

		m_selectStatus = AUI_LISTBOX_SELECTSTATUS_NONE;

		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;
	}
	else
		MouseLDoubleClickOutside( mouseData );
}

//----------------------------------------------------------------------------
//
// Name       : aui_ListBox::BuildListStart
//
// Description: Start building a list.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : This suppresses some (e.g. ranger) automatic update actions
//              while building a list. When the list is complete, call
//              BuildListEnd to resume automatic updating.
//
//----------------------------------------------------------------------------
void aui_ListBox::BuildListStart(void)
{
	m_savedForceSelect = IsForceSelect();

	SetForceSelect(FALSE);

	m_buildingTheList = TRUE;
}

//----------------------------------------------------------------------------
//
// Name       : aui_ListBox::BuildListEnd
//
// Description: Finish building a list.
//
// Parameters : isAddBottom	: the newest element is at the bottom
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : To be called after BuildListStart.
//
//----------------------------------------------------------------------------
void aui_ListBox::BuildListEnd(bool isAddBottom)
{
	m_buildingTheList = FALSE;

	SetForceSelect(m_savedForceSelect);

	CalculateDimensions();
	RepositionHeaderSwitches();
	RepositionRangers();
	sint32 const	verticalStart = isAddBottom
									? m_verticalRanger->GetMaximumY()
									: m_verticalRanger->GetMinimumY();
	m_verticalRanger->SetValue(m_verticalRanger->GetValueX(), verticalStart);
	RepositionItems();

	if ( m_forceSelect && !m_selectedList->L() )
		SelectItem( (sint32)0, 1 );

	SortByColumn( m_sortColumn, m_sortAscending );
}

void ListBoxRangerActionCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void *cookie )
{
	if ( action == (uint32)AUI_RANGER_ACTION_VALUECHANGE )
	{
		aui_ListBox *listbox = (aui_ListBox *)cookie;

		listbox->RangerMoved();
	}
}

aui_Item *aui_ListBox::ConstructAndAddTextItem(const MBCHAR *ldlblock, const MBCHAR *text, void *userData)
{
	/// @ToDo: An aui_ListBox should not know anything about a ctp2_ListItem
	ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot((MBCHAR *)ldlblock);
	Assert(item);
	if(!item) return NULL;

	aui_Static *box = (aui_Static *)item->GetChildByIndex(0);
	Assert(box);
	if(!box) {
		delete item;
		return NULL;
	}

	box->SetText(text);
	item->SetUserData(userData);
	AddItem(item);
	return item;
}

void aui_ListBox::SendKeyboardAction()
{
	if(m_keyboardActionControl) {
		m_keyboardActionControl->SendKeyboardAction();
	}
}

bool aui_ListBox::HandleKey(uint32 wParam)
{
	if(GetParentWindow() && GetParentWindow()->GetFocusControl() == this) {
		if(NumItems() < 1)
			return false;

		if(wParam == VK_UP + 256) {
			if(!GetSelectedItem() || GetSelectedItemIndex() == 0) {
				SelectItem(NumItems() - 1);
			} else {
				SelectItem(GetSelectedItemIndex() - 1);
			}
			return true;
		} else if(wParam == VK_DOWN + 256) {
			if(!GetSelectedItem() || GetSelectedItemIndex() >= NumItems() - 1) {
				SelectItem((sint32)0);
			} else {
				SelectItem(GetSelectedItemIndex() + 1);
			}
			return true;
		} else if(wParam == ' ') {
			if(m_keyboardActionControl) {
				m_keyboardActionControl->SendKeyboardAction();
				return true;
			}
		}
	}
	return false;
}

void aui_ListBox::SetKeyboardActionControl(aui_Control *control)
{
	m_keyboardActionControl = control;
}
