//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface dirty list (whatever this is)
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
// - Standardized code (May 21th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui_rectangle.h"

#include "aui_dirtylist.h"


aui_DirtyList::aui_DirtyList(
	BOOL useSpans,
	sint32 width,
	sint32 height )
	:
	m_width( width ),
	m_height( height ),
	m_isEmpty( TRUE )
{
	m_rectMemory = new tech_Memory<RECT>;
	Assert( m_rectMemory != NULL );
	if ( !m_rectMemory ) return;

	if ( useSpans )
	{
		Assert( m_width > 0 && m_height > 0 );
		if ( m_width <= 0 || m_height <= 0 ) return;

		m_spanListArray = new aui_SpanList[ m_height ];
		Assert( m_spanListArray != NULL );
		if ( !m_spanListArray ) return;

		memset( m_spanListArray, 0, m_height * sizeof( aui_SpanList ) );
	}
	else
		m_spanListArray = NULL;
}


aui_DirtyList::~aui_DirtyList()
{
	Flush();

	if ( m_rectMemory )
	{
		delete m_rectMemory;
		m_rectMemory = NULL;
	}

	if ( m_spanListArray )
	{
		delete[] m_spanListArray;
		m_spanListArray = NULL;
	}
}


AUI_ERRCODE aui_DirtyList::AddRect(
	sint32 left,
	sint32 top,
	sint32 right,
	sint32 bottom )
{

	if ( left < right && top < bottom )
	{
		RECT *rect = m_rectMemory->New();
		Assert( rect != NULL );
		if ( !rect ) return AUI_ERRCODE_MEMALLOCFAILED;

		rect->left = left;
		rect->top = top;
		rect->right = right;
		rect->bottom = bottom;

		AddTail( rect );

		if ( m_spanListArray )
			ComputeSpans( rect );
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_DirtyList::AddRect(
	RECT *rect )
{
	Assert( rect != NULL );
	if ( !rect ) return AUI_ERRCODE_INVALIDPARAM;

	return AddRect(
		rect->left,
		rect->top,
		rect->right,
		rect->bottom );
}


AUI_ERRCODE aui_DirtyList::SubtractRect(
	sint32 left,
	sint32 top,
	sint32 right,
	sint32 bottom )
{
	RECT rect = { left, top, right, bottom };
	return SubtractRect( &rect );
}


AUI_ERRCODE aui_DirtyList::SubtractRect( RECT *sub )
{
	AUI_ERRCODE alteredList = AUI_ERRCODE_UNHANDLED;

	Assert( sub != NULL );
	if ( !sub ) return alteredList;

	if ( sub->left < sub->right && sub->top < sub->bottom )
	{

		ListPos position = GetHeadPosition();
		for ( sint32 i = L(); i; i-- )
		{
			ListPos prevPosition = position;
			RECT *rect = GetNext( position );

			static RECT moreRects[4];
			static sint32 num;
			if ( (num = Rectangle_Subtract( rect, sub, moreRects )) > 0 )
			{

				if ( memcmp( rect, moreRects, sizeof( RECT ) ) )
				{
					alteredList = AUI_ERRCODE_HANDLED;

					CopyRect( rect, moreRects );

					ListPos insertPosition = prevPosition;
					for ( sint32 j = 1; j < num; j++ )
					{
						RECT *r = m_rectMemory->New();
						Assert( r != NULL );
						if ( !r ) return alteredList;

						CopyRect( r, moreRects + j );

						InsertAfter( insertPosition, r );
						GetNext( insertPosition );
					}
				}
			}
			else if ( !num )
			{
				m_rectMemory->Delete( rect );
				DeleteAt( prevPosition );
			}
			else
			{

			}
		}
	}

	return alteredList;
}


AUI_ERRCODE aui_DirtyList::Minimize( void )
{

	BOOL shouldContinue = L() > 1;

	while ( shouldContinue )
	{

		shouldContinue = FALSE;

		ListPos curPos = GetHeadPosition();

		for ( sint32 i = L() - 1; i; i-- )
		{

			RECT *curRect = GetNext( curPos );

			ListPos nextPos = curPos;

			for ( sint32 j = i; j; j-- )
			{

				ListPos prevPos = nextPos;
				RECT *nextRect = GetNext( nextPos );

				RECT conRect;
				if ( Rectangle_SmartConsolidate( &conRect, curRect, nextRect ) )
				{

					m_rectMemory->Delete( nextRect );
					DeleteAt( prevPos );

					CopyRect( curRect, &conRect );

					shouldContinue = L() > 1;
					break;
				}
			}

			if ( shouldContinue ) break;
		}
	}

	return AUI_ERRCODE_OK;
}


void aui_DirtyList::Flush( void )
{

	ListPos position = GetHeadPosition();
	for ( sint32 i = L(); i; i-- )
		m_rectMemory->Delete( GetNext( position ) );


	DeleteAll();

	if ( m_spanListArray )
		memset( m_spanListArray, 0, m_height * sizeof( aui_SpanList ) );

	m_isEmpty = TRUE;
}


AUI_ERRCODE aui_DirtyList::SetSpans( aui_DirtyList *newDirtyList )
{
	Assert( newDirtyList != NULL );
	if ( !newDirtyList )
		return AUI_ERRCODE_INVALIDPARAM;

	aui_SpanList *newSpanListArray = newDirtyList->GetSpans();

	Assert( newSpanListArray != NULL );
	if ( !newSpanListArray )
		return AUI_ERRCODE_INVALIDPARAM;

	Assert( newDirtyList->GetWidth() == m_width );
	if ( newDirtyList->GetWidth() != m_width )
		return AUI_ERRCODE_INVALIDPARAM;

	Assert( newDirtyList->GetHeight() == m_height );
	if ( newDirtyList->GetHeight() != m_height )
		return AUI_ERRCODE_INVALIDPARAM;

	Flush();

	if ( !newDirtyList->IsEmpty() )
	{
		memcpy(
			m_spanListArray,
			newSpanListArray,
			m_height * sizeof( aui_SpanList ) );

		m_isEmpty = FALSE;
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_DirtyList::ComputeSpans( RECT *newRect )
{
	Assert( newRect != NULL );
	if ( !newRect ) return AUI_ERRCODE_HACK;

	if (newRect->top < 0) newRect->top = 0;
	if (newRect->left < 0) newRect->left = 0;
	if (newRect->bottom >= m_height) newRect->bottom = m_height-1;
	if (newRect->right >= m_width) newRect->right = m_width-1;




	sint32 newStart = newRect->left;
	sint32 newStop = newRect->right;

	Assert( newStart <= newStop );
	if ( newStart > newStop ) return AUI_ERRCODE_INVALIDPARAM;

	if ( newStart == newStop ) return AUI_ERRCODE_OK;

	sint32 h = newRect->bottom - newRect->top;

	Assert( h >= 0 );
	if ( h < 0 ) return AUI_ERRCODE_INVALIDPARAM;

	if ( !h ) return AUI_ERRCODE_OK;

	aui_SpanList *curSpanList = m_spanListArray + newRect->top;
	for ( ; h; h--, curSpanList++ )
	{

		aui_Span *curSpan = curSpanList->spans;
		BOOL consolidated = FALSE;

		sint32 curStart = 0;
		sint32 curStop = 0;

		sint32 prevStop = 0;

		sint32 s;
		for ( s = curSpanList->num; s; s--, curSpan++ )
		{
			curStop = ( curStart = curStop + curSpan->run ) + curSpan->length;

			if ( newStart <= curStop )
			{

				if ( newStop >= curStart )
				{
					newStart = newStart < curStart ? newStart : curStart;
					newStop = newStop > curStop ? newStop : curStop;

					if ( consolidated )
					{
						memmove( curSpan - 1, curSpan, s * sizeof( aui_Span ) );
						curSpanList->num--;
						curSpan--;
					}
					else
						consolidated = TRUE;

					curSpan->run = sint16(newStart - prevStop);
					curSpan->length = sint16(newStop - newStart);

					Assert( curSpan->run >= 0 );
					Assert( curSpan->length > 0 );
				}
				else
				{

					if ( consolidated )
						break;


					if ( curSpanList->num < k_DIRTYLIST_MAXSPANS )
						memmove( curSpan + 1, curSpan, s * sizeof( aui_Span ) );

					s = 0;
					break;
				}
			}

			if ( !consolidated )
				prevStop = curStop;
		}

		if ( !s && !consolidated )
		{

			if ( curSpanList->num == k_DIRTYLIST_MAXSPANS )
			{

				curSpanList->num = 1;

				curSpan = curSpanList->spans;
				if ( newStart < curSpan->run )
					curSpan->run = (sint16)newStart;
				curSpan->length = 0;

				const aui_Span *pStop = curSpan + k_DIRTYLIST_MAXSPANS;
				for ( aui_Span *p = curSpan; p != pStop; p++ )
					curSpan->length += p->run + p->length;

				if ( newStop > curSpan->length )
					curSpan->length = (sint16)newStop;

				curSpan->length -= curSpan->run;
			}
			else
			{
				curSpanList->num++;
				curSpan->run = sint16(newStart - prevStop);
				curSpan->length = sint16(newStop - newStart);

				Assert( curSpan->run >= 0 );
				Assert( curSpan->length > 0 );
			}
		}
	}

	m_isEmpty = FALSE;

	return AUI_ERRCODE_OK;
}
