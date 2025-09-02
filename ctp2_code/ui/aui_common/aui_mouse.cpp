//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface mouse handling
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
// _DEBUG
// - Generate debug version when set.
//
// SEIZUREBLIT
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin G?hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_mouse.h"

#include <chrono>
#include <thread>

#include "aui_ui.h"

#include "aui_blitter.h"
#include "aui_cursor.h"
#include "aui_window.h"
#include "aui_ldl.h"

#include "profileDB.h"

#include "c3ui.h"

#include "aui_hypertextbox.h"

#include "ldl_data.hpp"
#include "ldl_file.hpp"

aui_Mouse::aui_Mouse
(
	AUI_ERRCODE  *   retval,
	const MBCHAR *   ldlBlock
)
:
	aui_Base    (),
	aui_Input   ()
{
	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
}

AUI_ERRCODE aui_Mouse::InitCommon( void )
{
	m_curCursor = m_cursors;
	m_firstIndex = 0;
	m_lastIndex = 0;
	m_animDelay = k_MOUSE_DEFAULTANIMDELAY;
	m_time = 0;

	if(g_theProfileDB) {

		m_sensitivity = 0.25 * (1 + g_theProfileDB->GetMouseSpeed());
	} else {
		m_sensitivity = 1.0;
	}
	m_suspendCount = 0;
	m_showCount = 0;
	m_reset = TRUE;
	m_flags = 0;

	memset( m_cursors, 0, sizeof( m_cursors ) );
	memset( &m_data, 0, sizeof( m_data ) );
	memset( m_inputs, 0, sizeof( m_inputs ) );

	SetClip( NULL );

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Mouse::InitCommonLdl( const MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = InitCommon();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	if ( ldlBlock )
	{
		ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
		Assert( block != NULL );
		if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

		sint32 numCursors = FindNumCursorsFromLdl( block );
		if ( numCursors )
		{
			Assert( numCursors <= k_MOUSE_MAXNUMCURSORS );
			if ( numCursors > k_MOUSE_MAXNUMCURSORS )
				numCursors = k_MOUSE_MAXNUMCURSORS;

			MBCHAR temp[ k_AUI_LDL_MAXBLOCK + 1 ] = "";
			for ( sint32 i = 0; i < numCursors; i++ )
			{
				aui_Cursor *cursor = NULL;

				sprintf( temp, k_MOUSE_LDL_CURSOR "%d", i );

				const MBCHAR *filename = block->GetString(temp);
				if ( filename )
				{
					cursor = g_ui->LoadCursor( filename );
					Assert( cursor != NULL );
					if ( !cursor ) return AUI_ERRCODE_LOADFAILED;

					Assert(cursor->TheSurface() != NULL);
					Assert( cursor->TheSurface()->Width() <= k_MOUSE_MAXSIZE );
					Assert( cursor->TheSurface()->Height() <= k_MOUSE_MAXSIZE );
					if ( cursor->TheSurface()->Width() > k_MOUSE_MAXSIZE
					||   cursor->TheSurface()->Height() > k_MOUSE_MAXSIZE )
					{
						g_ui->UnloadCursor( cursor );
						return AUI_ERRCODE_INVALIDDIMENSION;
					}

					MBCHAR temp2[ k_AUI_LDL_MAXBLOCK + 1 ] = "";
					sprintf( temp, k_MOUSE_LDL_HOTSPOTX "%d", i );
					sprintf( temp2, k_MOUSE_LDL_HOTSPOTY "%d", i );

					cursor->SetHotspot(
						block->GetInt( temp ),
						block->GetInt( temp2 ) );

					cursor->TheSurface()->SetChromaKey(0);
				}

				m_cursors[ i ] = cursor;
			}
		}

		uint32 animDelay = block->GetInt( k_MOUSE_LDL_ANIMDELAY );
		SetAnimDelay( animDelay ? animDelay : k_MOUSE_DEFAULTANIMDELAY );
		SetAnimIndexes(
			block->GetInt( k_MOUSE_LDL_FIRSTINDEX ),
			block->GetInt( k_MOUSE_LDL_LASTINDEX ) );

		sint32 i = 0;
		MBCHAR temp[ k_AUI_LDL_MAXBLOCK + 1 ] = "";
		while ( 1 )
		{
			sprintf( temp, "%s.%s%d", ldlBlock, k_MOUSE_LDL_ANIM, i++ );

			ldl_datablock *blk = aui_Ldl::GetLdl()->FindDataBlock( temp );
			if ( !blk ) break;

			POINT indexes;
			indexes.x = blk->GetInt( k_MOUSE_LDL_FIRSTINDEX );
			indexes.y = blk->GetInt( k_MOUSE_LDL_LASTINDEX );
			m_animIndexList.AddTail( indexes );
			m_animDelayList.AddTail( blk->GetInt( k_MOUSE_LDL_ANIMDELAY ) );
		}
	}
	ActivateCursor(*m_curCursor);

	return AUI_ERRCODE_OK;
}

sint32 aui_Mouse::FindNumCursorsFromLdl( ldl_datablock *block )
{
	sint32 i = block->GetInt( k_MOUSE_LDL_NUMCURSORS );
	if ( i ) return i;

	i = -1;
	BOOL found = TRUE;
	MBCHAR temp[ k_AUI_LDL_MAXBLOCK + 1 ] = "";
	while ( found )
	{
		found = FALSE;
		sprintf( temp, k_MOUSE_LDL_CURSOR "%d", ++i );
		if ( block->GetString( temp ) ) found = TRUE;
	}

	return i;
}

aui_Mouse::~aui_Mouse()
{
	End();

	if ( m_cursors )
	{
		for ( sint32 i = 0; i < k_MOUSE_MAXNUMCURSORS; i++ )
			if ( m_cursors[ i ] )
				g_ui->UnloadCursor( m_cursors[ i ] );

		memset( m_cursors, 0, sizeof( m_cursors ) );
	}

	m_animIndexList.DeleteAll();
	m_animDelayList.DeleteAll();
}

void aui_Mouse::SetClip( sint32 left, sint32 top, sint32 right, sint32 bottom )
{
	if ( left < 0 ) left = 0;
	if ( top < 0 ) top = 0;
	if ( right > g_ui->Width() ) right = g_ui->Width();
	if ( bottom > g_ui->Height() ) bottom = g_ui->Height();

	SetRect( &m_clip, left, top, right, bottom );
}

void aui_Mouse::SetClip( RECT *clip )
{
	if ( clip )
		SetClip( clip->left, clip->top, clip->right, clip->bottom );
	else
		SetClip( 0, 0, g_ui->Width(), g_ui->Height() );
}

void aui_Mouse::SetCursor(sint32 index, MBCHAR * cursor)
{
	Assert( index >= 0 && index < k_MOUSE_MAXNUMCURSORS );
	if ( index < 0 || index >= k_MOUSE_MAXNUMCURSORS ) return;

	aui_Cursor *    oldCursor   = m_cursors[index];
	aui_Cursor *    c           = cursor ? g_ui->LoadCursor(cursor) : NULL;

	if (cursor)
	{
		Assert(c);
		if (c)
		{
			Assert(c->TheSurface()->Width() <= k_MOUSE_MAXSIZE);
			Assert(c->TheSurface()->Height() <= k_MOUSE_MAXSIZE);

			if (    c->TheSurface()->Width() > k_MOUSE_MAXSIZE
			     || c->TheSurface()->Height() > k_MOUSE_MAXSIZE
			   )
			{
				g_ui->UnloadCursor(c);
				c = NULL;
			}
		}
	}

	m_cursors[index] = c;

	if (oldCursor) g_ui->UnloadCursor(oldCursor);
}

void aui_Mouse::SetCurrentCursor( sint32 index )
{
	Assert( index >= 0 && index < k_MOUSE_MAXNUMCURSORS );
	if ( index < 0 || index >= k_MOUSE_MAXNUMCURSORS ) return;

	Assert( index >= m_firstIndex && index <= m_lastIndex );
	if ( index < m_firstIndex || index > m_lastIndex ) return;

	m_curCursor = m_cursors + index;
	Assert(*m_curCursor);
	ActivateCursor(*m_curCursor);
}

sint32 aui_Mouse::GetCurrentCursorIndex(void)
{
	for (sint32 i = m_firstIndex; i <= m_lastIndex; ++i)
	{
		if (m_curCursor == (m_cursors + i))
			return i;
	}

	return 0;
}

void aui_Mouse::SetAnimDelay( uint32 animDelay )
{
	m_animDelay = animDelay;
}

void aui_Mouse::SetAnimIndexes( sint32 firstIndex, sint32 lastIndex )
{
	if (firstIndex == m_firstIndex && lastIndex == m_lastIndex)
		return;

	Assert( firstIndex >= 0 );
	if ( firstIndex < 0 ) return;
	Assert( lastIndex >= 0 );
	if ( lastIndex < 0 ) return;
	Assert( firstIndex <= lastIndex );
	if ( firstIndex > lastIndex ) return;

	m_firstIndex = firstIndex;
	m_lastIndex = lastIndex;
	SetCurrentCursor(m_firstIndex);
}

void aui_Mouse::SetAnim( sint32 anim )
{
	Assert( anim >= 0 );
	if ( anim < 0 ) return;

	SetAnimIndexes(anim, anim);
}

AUI_ERRCODE aui_Mouse::Start( void )
{
	m_curCursor = m_cursors + m_firstIndex;
	ActivateCursor(*m_curCursor);

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Mouse::End( void )
{
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Mouse::Suspend( BOOL eraseCursor )
{
	// That is never true
	if ( m_suspendCount )
	{
		m_suspendCount++;
		return AUI_ERRCODE_OK;
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Mouse::Resume( void )
{
	if ( m_suspendCount == 0 ) return AUI_ERRCODE_OK;

	if ( m_suspendCount > 1 )
	{
		m_suspendCount--;
		return AUI_ERRCODE_OK;
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Mouse::SetPosition( sint32 x, sint32 y )
{
	if ( x < m_clip.left ) x = m_clip.left;
	if ( y < m_clip.top ) y = m_clip.top;
	if ( x >= m_clip.right ) x = m_clip.right - 1;
	if ( y >= m_clip.bottom ) y = m_clip.bottom - 1;

	AUI_ERRCODE errcode = Suspend( FALSE );
	Assert( AUI_SUCCESS(errcode) );

	m_data.position.x = x;
	m_data.position.y = y;

	errcode = Resume();
	Assert( AUI_SUCCESS(errcode) );

	return errcode;
}

AUI_ERRCODE aui_Mouse::SetPosition( POINT *point )
{
	Assert( point != NULL );
	if ( !point ) return AUI_ERRCODE_INVALIDPARAM;

	return SetPosition( point->x, point->y );
}

AUI_ERRCODE aui_Mouse::GetHotspot( sint32 *x, sint32 *y, sint32 index )
{
	Assert( index >= 0 && index < k_MOUSE_MAXNUMCURSORS );
	if ( index < 0 || index >= k_MOUSE_MAXNUMCURSORS )
		return AUI_ERRCODE_INVALIDPARAM;

	aui_Cursor *cursor = m_cursors[ index ];
	if ( cursor )
		return cursor->GetHotspot( x, y );

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Mouse::SetHotspot( sint32 x, sint32 y, sint32 index )
{
	Assert( index >= 0 && index < k_MOUSE_MAXNUMCURSORS );
	if ( index < 0 || index >= k_MOUSE_MAXNUMCURSORS )
		return AUI_ERRCODE_INVALIDPARAM;

	aui_Cursor * cursor = m_cursors[index];
	return (cursor) ? cursor->SetHotspot(x, y) : AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Mouse::HandleAnim( void )
{
	if ( m_lastIndex >= m_firstIndex )
	{

		if(m_curCursor < m_cursors + m_firstIndex) {
			m_curCursor = m_cursors + m_firstIndex;
			ActivateCursor(*m_curCursor);
		}

		uint32 now = GetTickCount();
		if ( now - m_time > m_animDelay )
		{

			if ( m_curCursor++ >= m_cursors + m_lastIndex )
				m_curCursor = m_cursors + m_firstIndex;
			ActivateCursor(*m_curCursor);
			m_time = now;

			return AUI_ERRCODE_HANDLED;
		}
	}

	return AUI_ERRCODE_UNHANDLED;
}
