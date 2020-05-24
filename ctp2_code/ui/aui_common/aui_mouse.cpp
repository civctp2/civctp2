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

#include "aui_Factory.h"
#include "aui_ui.h"

#include "aui_blitter.h"
#include "aui_cursor.h"
#include "aui_window.h"
#include "aui_rectangle.h"
#include "aui_ldl.h"

#include "profileDB.h"

#include "c3ui.h"

#include "aui_hypertextbox.h"

#include "ldl_data.hpp"
#include "ldl_file.hpp"

#ifdef __AUI_USE_DIRECTX__
sint32 aui_Mouse::m_mouseRefCount = 0;
LPCRITICAL_SECTION aui_Mouse::m_lpcs = NULL;
#define k_AUI_MOUSE_THREAD_SLEEP_TIME	10
#endif // __AUI_USE_DIRECTX__

#include "civapp.h"
extern CivApp		*g_civApp;

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

#ifdef __AUI_USE_DIRECTX__
	m_privateMix = NULL;
	m_pickup = NULL;
	m_prevPickup = NULL;

	m_thread = NULL;
	m_threadId = 0;
	m_threadEvent = NULL;
	m_terminateEvent = NULL;
	m_suspendEvent = NULL;
	m_resumeEvent = NULL;
	m_replyEvent = NULL;
#endif // __AUI_USE_DIRECTX__

	memset( m_cursors, 0, sizeof( m_cursors ) );
	memset( &m_data, 0, sizeof( m_data ) );
	memset( m_inputs, 0, sizeof( m_inputs ) );

	SetClip( NULL );

#ifdef __AUI_USE_DIRECTX__
	GetCursorPos( &m_data.position );

	if ( !m_mouseRefCount++ )
	{
		m_lpcs = new CRITICAL_SECTION;
		Assert( m_lpcs != NULL );
		if ( m_lpcs )
			InitializeCriticalSection( m_lpcs );
		else
			return AUI_ERRCODE_MEMALLOCFAILED;
	}
#endif // __AUI_USE_DIRECTX__

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

#ifdef __AUI_USE_DIRECTX__
	DestroyPrivateBuffers();

	if ( !--m_mouseRefCount )
	{
		if ( m_lpcs )
		{
			DeleteCriticalSection( m_lpcs );
			delete m_lpcs;
			m_lpcs = NULL;
		}
	}
#endif // __AUI_USE_DIRECTX__
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

#ifdef __AUI_USE_DIRECTX__
	CreatePrivateBuffers();

	m_thread = CreateThread( NULL, 0, MouseThreadProc, (LPVOID)this, 0, &m_threadId );
	if ( m_thread )
	{
		SetThreadPriority( m_thread, THREAD_PRIORITY_NORMAL );

		m_threadEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		m_terminateEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		m_suspendEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		m_resumeEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		m_replyEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

		Acquire();
	}
#endif // __AUI_USE_DIRECTX__

	return AUI_ERRCODE_OK;
}

#ifdef __AUI_USE_DIRECTX__

AUI_ERRCODE aui_Mouse::CreatePrivateBuffers( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	DestroyPrivateBuffers();

	m_privateMix = aui_Factory::new_Surface(errcode, k_MOUSE_MAXSIZE, k_MOUSE_MAXSIZE);
	Assert( AUI_NEWOK(m_privateMix, errcode) );
	if ( !AUI_NEWOK(m_privateMix, errcode) ) return errcode;

	m_pickup =     aui_Factory::new_Surface(errcode, k_MOUSE_MAXSIZE, k_MOUSE_MAXSIZE);
	Assert( AUI_NEWOK(m_pickup, errcode) );
	if ( !AUI_NEWOK(m_pickup, errcode) ) return errcode;

	m_prevPickup = aui_Factory::new_Surface(errcode, k_MOUSE_MAXSIZE, k_MOUSE_MAXSIZE);
	Assert( AUI_NEWOK(m_prevPickup, errcode) );
	if ( !AUI_NEWOK(m_prevPickup, errcode) ) return errcode;

	return errcode;
}

void aui_Mouse::DestroyPrivateBuffers( void )
{
	delete m_privateMix;
	m_privateMix = NULL;

	delete m_pickup;
	m_pickup = NULL;

	delete m_prevPickup;
	m_prevPickup = NULL;
}

#endif // __AUI_USE_DIRECTX__

AUI_ERRCODE aui_Mouse::End( void )
{
#ifdef __AUI_USE_DIRECTX__
	Unacquire();

	if ( m_thread )
	{
		if ( m_threadEvent && m_terminateEvent )
		{
			SetEvent( m_terminateEvent );
			SetEvent( m_threadEvent );

			if ( WaitForSingleObject( m_thread, 2000 ) != WAIT_OBJECT_0 )
				TerminateThread( m_thread, 1 );

			CloseHandle( m_threadEvent );
			m_threadEvent = NULL;

			CloseHandle( m_terminateEvent );
			m_terminateEvent = NULL;
		}
		else
			TerminateThread( m_thread, 1 );

		Erase();

		if ( m_suspendEvent )
		{
			CloseHandle( m_suspendEvent );
			m_suspendEvent = NULL;
		}

		if ( m_resumeEvent )
		{
			CloseHandle( m_resumeEvent );
			m_resumeEvent = NULL;
		}

		if ( m_replyEvent )
		{
			CloseHandle( m_replyEvent );
			m_replyEvent = NULL;
		}

		CloseHandle( m_thread );

		m_threadId = 0;
	}

	SetCursorPos( m_data.position.x, m_data.position.y );
#endif // __AUI_USE_DIRECTX__

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Mouse::Suspend( BOOL eraseCursor )
{
#ifdef __AUI_USE_DIRECTX__
	if ( !m_thread ) return AUI_ERRCODE_NOTHREAD;
#endif // __AUI_USE_DIRECTX__

	if ( m_suspendCount )
	{
		m_suspendCount++;
		return AUI_ERRCODE_OK;
	}
#ifdef __AUI_USE_SDL__
	return AUI_ERRCODE_OK;
#elif defined(__AUI_USE_DIRECTX__)
	SetEvent( m_suspendEvent );
	SetEvent( m_threadEvent );

	if ( WaitForSingleObject( m_replyEvent, INFINITE ) == WAIT_OBJECT_0 )
	{
		ResetEvent( m_replyEvent );

		if ( SuspendThread( m_thread ) != 0xffffffff )
		{

			if ( eraseCursor )
			{
				Erase();
				Unacquire();
			}

			m_suspendCount++;

			return AUI_ERRCODE_OK;
		}
	}

	return AUI_ERRCODE_SUSPENDFAILED;
#endif
}

AUI_ERRCODE aui_Mouse::Resume( void )
{
#ifdef __AUI_USE_DIRECTX__
	if ( !m_thread ) return AUI_ERRCODE_NOTHREAD;
#endif // __AUI_USE_DIRECTX__

	if ( m_suspendCount == 0 ) return AUI_ERRCODE_OK;

	if ( m_suspendCount > 1 )
	{
		m_suspendCount--;
		return AUI_ERRCODE_OK;
	}

#ifdef __AUI_USE_SDL__
	return AUI_ERRCODE_OK;
#elif defined(__AUI_USE_DIRECTX__)
	if ( ResumeThread( m_thread ) != 0xffffffff )
	{

		if (!IsAcquired())
		{
			Acquire();
		}

		SetEvent( m_resumeEvent );

		m_suspendCount = 0;

		return AUI_ERRCODE_OK;
	}

	return AUI_ERRCODE_RESUMEFAILED;
#endif
}

#ifdef __AUI_USE_DIRECTX__

inline BOOL aui_Mouse::ShouldTerminateThread( void )
{

	if ( WaitForSingleObject( m_threadEvent, 0 ) == WAIT_OBJECT_0 )
	{
		ResetEvent( m_threadEvent );

		if ( WaitForSingleObject( m_terminateEvent, 0 ) == WAIT_OBJECT_0 )
		{
			ResetEvent( m_terminateEvent );
			return TRUE;
		}

		if ( WaitForSingleObject( m_suspendEvent, 0 ) == WAIT_OBJECT_0 )
		{
			ResetEvent( m_suspendEvent );

			SetEvent( m_replyEvent );
			WaitForSingleObject( m_resumeEvent, INFINITE );

			ResetEvent( m_resumeEvent );
		}
	}
	return FALSE;
}

DWORD WINAPI MouseThreadProc( LPVOID param )
{
	aui_Mouse *mouse = (aui_Mouse *)param;

	while ( !mouse->ShouldTerminateThread()  )
	{
		Sleep(k_AUI_MOUSE_THREAD_SLEEP_TIME);

		// For that something is wrong with the mouse delay
//		std::this_thread::sleep_for(std::chrono::milliseconds(k_AUI_MOUSE_THREAD_SLEEP_TIME));

		mouse->HandleAnim();
		mouse->GetInput();
		mouse->ReactToInput();
		mouse->ManipulateInputs( mouse->GetLatestMouseEvent(), TRUE );
	}

	return 0;
}

#endif // __AUI_USE_DIRECTX__

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

AUI_ERRCODE aui_Mouse::ReactToInput( void )
{
#ifdef __AUI_USE_DIRECTX__
	if ( IsHidden() || ((*m_curCursor) == NULL)) return AUI_ERRCODE_OK;

	POINT hotspot;
	(*m_curCursor)->GetHotspot(hotspot);

	POINT image =
	{
		m_data.position.x - hotspot.x,
		m_data.position.y - hotspot.y
	};

	static POINT prevImage = image;
	if ( m_reset )
		prevImage = image;

	sint32 dx = image.x - prevImage.x;
	sint32 dy = image.y - prevImage.y;

	prevImage.x = image.x;
	prevImage.y = image.y;

	RECT mixRect =
	{
		image.x,
		image.y,
		image.x + k_MOUSE_MAXSIZE,
		image.y + k_MOUSE_MAXSIZE
	};

	RECT unclippedMixRect = mixRect;

	Rectangle_Clip( &mixRect, &m_clip );

	RECT rect =
	{
		mixRect.left - image.x,
		mixRect.top - image.y,
		mixRect.right - image.x,
		mixRect.bottom - image.y
	};

	static RECT prevRect = { 0, 0, 0, 0 };
	static RECT prevUnclippedMixRect = { 0, 0, 0, 0 };
	if ( m_reset )
	{
		memset( &prevRect, 0, sizeof( prevRect ) );
		memset( &prevUnclippedMixRect, 0, sizeof( prevUnclippedMixRect ) );
	}

	AUI_ERRCODE errcode = g_ui->TheBlitter()->Blt(
		m_pickup,
		rect.left,
		rect.top,
		g_ui->Secondary(),
		&mixRect,
		k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST);
	Assert( errcode == AUI_ERRCODE_OK );

	if ( m_reset ) m_reset = FALSE;
	else if ( (-k_MOUSE_MAXSIZE < dx && dx < k_MOUSE_MAXSIZE)
	&&        (-k_MOUSE_MAXSIZE < dy && dy < k_MOUSE_MAXSIZE) )
	{
		sint32 fixX = 0;
		sint32 fixY = 0;
		RECT fixRect = { 0, 0, k_MOUSE_MAXSIZE, k_MOUSE_MAXSIZE };

		if ( dx > 0 )
		{
			fixRect.left += dx;
		}
		else
		{
			fixX = -dx;
			fixRect.right += dx;
		}

		if ( dy > 0 )
		{
			fixRect.top += dy;
		}
		else
		{
			fixY = -dy;
			fixRect.bottom += dy;
		}

		errcode = g_ui->TheBlitter()->Blt(
			m_pickup,
			fixX,
			fixY,
			m_prevPickup,
			&fixRect,
			k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST);
		Assert( errcode == AUI_ERRCODE_OK );
	}

	errcode = g_ui->TheBlitter()->Blt(
		m_privateMix,
		rect.left,
		rect.top,
		m_pickup,
		&rect,
		k_AUI_BLITTER_FLAG_COPY );
	Assert( errcode == AUI_ERRCODE_OK );

	errcode = g_ui->TheBlitter()->Blt(
		m_privateMix,
		rect.left,
		rect.top,
		(*m_curCursor)->TheSurface(),
		&rect,
		k_AUI_BLITTER_FLAG_CHROMAKEY );
	Assert( errcode == AUI_ERRCODE_OK );

	errcode = g_ui->BltToSecondary(
		mixRect.left,
		mixRect.top,
		m_privateMix,
		&rect,
		k_AUI_BLITTER_FLAG_COPY );
	Assert( errcode == AUI_ERRCODE_OK );

	static RECT eraser1, eraser2;
	Rectangle_GetErasers(
		&eraser1, &eraser2,
		unclippedMixRect.left, unclippedMixRect.top,
		dx, dy,
		unclippedMixRect.right - unclippedMixRect.left,
		unclippedMixRect.bottom - unclippedMixRect.top );

	Rectangle_Clip( &eraser1, &m_clip );
	Rectangle_Clip( &eraser2, &m_clip );

	SetRect(
		&prevRect,
		eraser1.left - prevUnclippedMixRect.left,
		eraser1.top - prevUnclippedMixRect.top,
		eraser1.right - prevUnclippedMixRect.left,
		eraser1.bottom - prevUnclippedMixRect.top );

	errcode = g_ui->BltToSecondary(
		eraser1.left,
		eraser1.top,
		m_prevPickup,
		&prevRect,
		k_AUI_BLITTER_FLAG_COPY );
	Assert( errcode == AUI_ERRCODE_OK );

	SetRect(
		&prevRect,
		eraser2.left - prevUnclippedMixRect.left,
		eraser2.top - prevUnclippedMixRect.top,
		eraser2.right - prevUnclippedMixRect.left,
		eraser2.bottom - prevUnclippedMixRect.top );

	errcode = g_ui->BltToSecondary(
		eraser2.left,
		eraser2.top,
		m_prevPickup,
		&prevRect,
		k_AUI_BLITTER_FLAG_COPY );
	Assert( errcode == AUI_ERRCODE_OK );

	errcode = g_ui->BltSecondaryToPrimary(k_AUI_BLITTER_FLAG_COPY);
	Assert( errcode == AUI_ERRCODE_OK );

	aui_Surface *tempSurf = m_prevPickup;
	m_prevPickup = m_pickup;
	m_pickup = tempSurf;

	CopyRect( &prevRect, &rect );
	CopyRect( &prevUnclippedMixRect, &unclippedMixRect );
#endif // __AUI_USE_DIRECTX__

	return AUI_ERRCODE_OK;
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

AUI_ERRCODE	aui_Mouse::BltWindowToPrimary( aui_Window *window )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

#ifdef __AUI_USE_DIRECTX__
	AUI_ERRCODE errcode;

	sint32 windowX = window->X();
	sint32 windowY = window->Y();
	aui_Surface *windowSurface = window->TheSurface();
	aui_DirtyList *windowDirtyList = window->GetDirtyList();

	ListPos position = windowDirtyList->GetHeadPosition();
	for ( sint32 j = windowDirtyList->L(); j; j-- )
	{

		RECT *windowDirtyRect = windowDirtyList->GetNext( position );

		RECT screenDirtyRect = *windowDirtyRect;
		OffsetRect( &screenDirtyRect, windowX, windowY );

		POINT hotspot;
		(*m_curCursor)->GetHotspot(hotspot);

		POINT image = m_data.position;
		image.x -= hotspot.x;
		image.y -= hotspot.y;

		RECT mixRect =
		{
			image.x,
			image.y,
			image.x + k_MOUSE_MAXSIZE,
			image.y + k_MOUSE_MAXSIZE
		};

		Rectangle_Clip( &mixRect, &m_clip );
		Rectangle_Clip( &mixRect, &screenDirtyRect );

		RECT rect =
		{
			mixRect.left - image.x,
			mixRect.top - image.y,
			mixRect.right - image.x,
			mixRect.bottom - image.y
		};

		RECT windowMixRect = mixRect;
		OffsetRect( &windowMixRect, -windowX, -windowY );

		errcode = g_ui->TheBlitter()->Blt(
			m_prevPickup,
			rect.left,
			rect.top,
			windowSurface,
			&windowMixRect,
			k_AUI_BLITTER_FLAG_COPY );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->Blt(
			windowSurface,
			windowMixRect.left,
			windowMixRect.top,
			(*m_curCursor)->TheSurface(),
			&rect,
			k_AUI_BLITTER_FLAG_CHROMAKEY );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

#ifdef _DEBUG

		static FILE *f = fopen( "__seisureblit__", "rb" );
		if ( f )
		{
			g_ui->ColorBltToSecondary(
				&screenDirtyRect,
				RGB(255,255,255),
				0 );
			Sleep( 20 );
			fclose(f);
		}
#endif

		errcode = g_ui->BltToSecondary(
			screenDirtyRect.left,
			screenDirtyRect.top,
			windowSurface,
			windowDirtyRect,
			k_AUI_BLITTER_FLAG_COPY );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->Blt(
			windowSurface,
			windowMixRect.left,
			windowMixRect.top,
			m_prevPickup,
			&rect,
			k_AUI_BLITTER_FLAG_COPY );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}
	}
#endif // __AUI_USE_DIRECTX__

	return retcode;
}

AUI_ERRCODE	aui_Mouse::BltDirtyRectInfoToPrimary( void )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

#ifdef __AUI_USE_DIRECTX__
	AUI_ERRCODE errcode;

	if (g_civApp->IsInBackground()) return retcode;

	tech_WLList<aui_UI::DirtyRectInfo *> *driList =
		g_ui->GetDirtyRectInfoList();

	uint32 blitFlags;
	LPVOID primaryBuf = NULL;

	if (g_theProfileDB && g_theProfileDB->IsUseDirectXBlitter())
	{
		blitFlags = k_AUI_BLITTER_FLAG_COPY;
	}
	else
	{
		blitFlags = k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST;
		errcode = g_ui->Secondary()->Lock( NULL, &primaryBuf, 0 );
		Assert( errcode == AUI_ERRCODE_OK );
	}

	ListPos position = driList->GetHeadPosition();
	for ( sint32 j = driList->L(); j; j-- )
	{
		aui_UI::DirtyRectInfo *dri = driList->GetNext( position );

		aui_Window *window = dri->window;

		sint32 windowX = window->X();
		sint32 windowY = window->Y();
		aui_Surface *windowSurface = window->TheSurface();

		if ( !windowSurface ) continue;

		if (g_civApp->IsInBackground()) continue;

		RECT *windowDirtyRect = &dri->rect;

		RECT screenDirtyRect = *windowDirtyRect;
		OffsetRect( &screenDirtyRect, windowX, windowY );

		POINT hotspot;
		(*m_curCursor)->GetHotspot(hotspot);

		POINT image = m_data.position;
		image.x -= hotspot.x;
		image.y -= hotspot.y;

		RECT mixRect =
		{
			image.x,
			image.y,
			image.x + k_MOUSE_MAXSIZE,
			image.y + k_MOUSE_MAXSIZE
		};

		Rectangle_Clip( &mixRect, &m_clip );
		Rectangle_Clip( &mixRect, &screenDirtyRect );

		RECT rect =
		{
			mixRect.left - image.x,
			mixRect.top - image.y,
			mixRect.right - image.x,
			mixRect.bottom - image.y
		};

		RECT windowMixRect = mixRect;
		OffsetRect( &windowMixRect, -windowX, -windowY );

		errcode = g_ui->TheBlitter()->Blt(
			m_prevPickup,
			rect.left,
			rect.top,
			windowSurface,
			&windowMixRect,
			k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->Blt(
			windowSurface,
			windowMixRect.left,
			windowMixRect.top,
			(*m_curCursor)->TheSurface(),
			&rect,
			k_AUI_BLITTER_FLAG_CHROMAKEY | k_AUI_BLITTER_FLAG_FAST);
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

#ifdef SEIZUREBLIT
		{
			g_ui->TheBlitter()->ColorBlt(
				g_ui->Secondary(),
				&screenDirtyRect,
				RGB(255,255,255),
				0 );
			Sleep( 20 );
		}
#endif

		if (!g_civApp->IsInBackground()) // Actual Drawing
		{
			errcode = g_ui->BltToSecondary(
				screenDirtyRect.left,
				screenDirtyRect.top,
				windowSurface,
				windowDirtyRect,
				blitFlags );
			Assert( errcode == AUI_ERRCODE_OK );
			if ( errcode != AUI_ERRCODE_OK )
			{
				retcode = AUI_ERRCODE_BLTFAILED;
				break;
			}
		}

		errcode = g_ui->TheBlitter()->Blt(
			windowSurface,
			windowMixRect.left,
			windowMixRect.top,
			m_prevPickup,
			&rect,
			k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST);

		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}
	}

	errcode = g_ui->BltSecondaryToPrimary(blitFlags);
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK )
	{
		retcode = AUI_ERRCODE_BLTFAILED;
	}

	if (!g_theProfileDB || !g_theProfileDB->IsUseDirectXBlitter())
	{
		errcode = g_ui->Secondary()->Unlock( primaryBuf );
		Assert( errcode == AUI_ERRCODE_OK );
	}
#endif // __AUI_USE_DIRECTX__

	return retcode;
}

AUI_ERRCODE	aui_Mouse::BltBackgroundColorToPrimary(
	COLORREF color,
	aui_DirtyList *colorAreas )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

#ifdef __AUI_USE_DIRECTX__
	AUI_ERRCODE errcode;

	if (g_civApp->IsInBackground()) return retcode;

	POINT hotspot;
	(*m_curCursor)->GetHotspot(hotspot);

	POINT cursorLocation = m_data.position;
	cursorLocation.x -= hotspot.x;
	cursorLocation.y -= hotspot.y;

	RECT screenCursorRect =
	{
		cursorLocation.x,
		cursorLocation.y,
		cursorLocation.x + k_MOUSE_MAXSIZE,
		cursorLocation.y + k_MOUSE_MAXSIZE
	};

	ListPos position = colorAreas->GetHeadPosition();
	for ( sint32 j = colorAreas->L(); j; j-- )
	{
		RECT *screenDirtyRect = colorAreas->GetNext( position );

		RECT clippedScreenCursorRect = screenCursorRect;
		Rectangle_Clip( &clippedScreenCursorRect, screenDirtyRect );

		RECT clippedCursorRect = clippedScreenCursorRect;
		OffsetRect( &clippedCursorRect, -cursorLocation.x, -cursorLocation.y );

		errcode = g_ui->TheBlitter()->ColorBlt(
			m_prevPickup,
			&clippedCursorRect,
			color,
			0 );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->ColorBlt(
			m_privateMix,
			&clippedCursorRect,
			color,
			0 );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->Blt(
			m_privateMix,
			clippedCursorRect.left,
			clippedCursorRect.top,
			(*m_curCursor)->TheSurface(),
			&clippedCursorRect,
			k_AUI_BLITTER_FLAG_CHROMAKEY );

		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->BltToSecondary(
			clippedScreenCursorRect.left,
			clippedScreenCursorRect.top,
			m_privateMix,
			&clippedCursorRect,
			k_AUI_BLITTER_FLAG_COPY );

		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		static RECT rects[ 4 ];
		sint32 numRects = Rectangle_Subtract(
			screenDirtyRect,
			&clippedScreenCursorRect,
			rects );

		Assert( numRects != -1 );
		if ( numRects == -1 )
		{
			retcode = AUI_ERRCODE_INVALIDPARAM;
			break;
		}

		for ( RECT *rectPtr = rects; numRects; numRects-- )
		{
			errcode = g_ui->ColorBltToSecondary(
				rectPtr++,
				color,
				0 );
			Assert( errcode == AUI_ERRCODE_OK );
			if ( errcode != AUI_ERRCODE_OK )
			{
				retcode = AUI_ERRCODE_BLTFAILED;
				break;
			}
		}
	}
#endif // __AUI_USE_DIRECTX__

	return retcode;
}

AUI_ERRCODE	aui_Mouse::BltBackgroundImageToPrimary(
	aui_Image *image,
	RECT *imageRect,
	aui_DirtyList *imageAreas )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

#ifdef __AUI_USE_DIRECTX__
	AUI_ERRCODE errcode;

	if (g_civApp->IsInBackground()) return retcode;

	POINT hotspot;
	(*m_curCursor)->GetHotspot(hotspot);

	POINT cursorLocation = m_data.position;
	cursorLocation.x -= hotspot.x;
	cursorLocation.y -= hotspot.y;

	RECT screenCursorRect =
	{
		cursorLocation.x,
		cursorLocation.y,
		cursorLocation.x + k_MOUSE_MAXSIZE,
		cursorLocation.y + k_MOUSE_MAXSIZE
	};

	ListPos position = imageAreas->GetHeadPosition();
	for ( sint32 j = imageAreas->L(); j; j-- )
	{

		RECT *screenDirtyRect = imageAreas->GetNext( position );

		RECT clippedScreenCursorRect = screenCursorRect;
		Rectangle_Clip( &clippedScreenCursorRect, screenDirtyRect );

		RECT clippedCursorRect = clippedScreenCursorRect;
		OffsetRect( &clippedCursorRect, -cursorLocation.x, -cursorLocation.y );

		RECT clippedImageCursorRect = clippedScreenCursorRect;
		OffsetRect(
			&clippedImageCursorRect, -imageRect->left, -imageRect->top );

		errcode = g_ui->TheBlitter()->Blt(
			m_prevPickup,
			clippedCursorRect.left,
			clippedCursorRect.top,
			image->TheSurface(),
			&clippedImageCursorRect,
			k_AUI_BLITTER_FLAG_COPY );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->Blt(
			m_privateMix,
			clippedCursorRect.left,
			clippedCursorRect.top,
			image->TheSurface(),
			&clippedImageCursorRect,
			k_AUI_BLITTER_FLAG_COPY );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->Blt(
			m_privateMix,
			clippedCursorRect.left,
			clippedCursorRect.top,
			(*m_curCursor)->TheSurface(),
			&clippedCursorRect,
			k_AUI_BLITTER_FLAG_CHROMAKEY );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->BltToSecondary(
			clippedScreenCursorRect.left,
			clippedScreenCursorRect.top,
			m_privateMix,
			&clippedCursorRect,
			k_AUI_BLITTER_FLAG_COPY );

		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		static RECT rects[ 4 ];
		sint32 numRects = Rectangle_Subtract(
			screenDirtyRect,
			&clippedScreenCursorRect,
			rects );
		Assert( numRects != -1 );
		if ( numRects == -1 )
		{
			retcode = AUI_ERRCODE_INVALIDPARAM;
			break;
		}

		for ( RECT *rectPtr = rects; numRects; numRects--, rectPtr++ )
		{

			RECT imageDirtyRect = *rectPtr;
			OffsetRect( &imageDirtyRect, -imageRect->left, -imageRect->top );

			errcode = g_ui->BltToSecondary(
				rectPtr->left,
				rectPtr->top,
				image->TheSurface(),
				&imageDirtyRect,
				k_AUI_BLITTER_FLAG_COPY );

			Assert( errcode == AUI_ERRCODE_OK );
			if ( errcode != AUI_ERRCODE_OK )
			{
				retcode = AUI_ERRCODE_BLTFAILED;
				break;
			}
		}
	}
#endif // __AUI_USE_DIRECTX__

	return retcode;
}

#ifdef __AUI_USE_DIRECTX__

AUI_ERRCODE aui_Mouse::Erase( void )
{
	AUI_ERRCODE errcode;

	POINT hotspot;
	(*m_curCursor)->GetHotspot(hotspot);

	POINT image = m_data.position;
	image.x -= hotspot.x;
	image.y -= hotspot.y;

	RECT mixRect =
	{
		image.x,
		image.y,
		image.x + k_MOUSE_MAXSIZE,
		image.y + k_MOUSE_MAXSIZE
	};

	Rectangle_Clip( &mixRect, &m_clip );

	RECT rect = mixRect;
	OffsetRect( &rect, -image.x, -image.y );

	errcode = g_ui->BltToSecondary(
		mixRect.left,
		mixRect.top,
		m_prevPickup,
		&rect,
		k_AUI_BLITTER_FLAG_COPY );

	m_reset = TRUE;

	return AUI_ERRCODE_OK;
}


#endif // __AUI_USE_DIRECTX__

sint32 aui_Mouse::ManipulateInputs( aui_MouseEvent *data, BOOL add )
{
	sint32 numManipulated = 0;

#ifdef __AUI_USE_DIRECTX__

	static sint32 index = 0;

	EnterCriticalSection( m_lpcs );

	if ( add )
	{
		if ( index <= k_MOUSE_MAXINPUT )
		{
			if ( index > 1 )
			{
				aui_MouseEvent *event = m_inputs + index - 2;
				if ( event->position.x == data->position.x
				&&   event->position.y == data->position.y
				&&   event->lbutton == data->lbutton
				&&   event->rbutton == data->rbutton )
				{
					event++;
					if ( event->position.x == data->position.x
					&&   event->position.y == data->position.y
					&&   event->lbutton == data->lbutton
					&&   event->rbutton == data->rbutton )
						index--;
				}
			}

			if ( index < k_MOUSE_MAXINPUT )
			{
				memcpy( m_inputs + index, data, sizeof( aui_MouseEvent ) );
				numManipulated = 1;
				index++;
			}
		}
	}

	else
	{
		if ( index )
			memcpy( data, m_inputs, index * sizeof( aui_MouseEvent ) );
		numManipulated = index;
		index = 0;
	}

	LeaveCriticalSection( m_lpcs );
#endif // __AUI_USE_DIRECTX__

	return numManipulated;
}
