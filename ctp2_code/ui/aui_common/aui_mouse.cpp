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
// __AUI_USE_DIRECTX__
// SEIZUREBLIT
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#ifdef __AUI_USE_DIRECTX__
#include "aui_directui.h"
#include "aui_directsurface.h"
#elif defined(__AUI_USE_SDL__)
#include "aui_sdlui.h"
#include "aui_sdlsurface.h"
#else
#include "aui_ui.h"
#include "aui_surface.h"
#endif

#include "aui_blitter.h"
#include "aui_cursor.h"
#include "aui_window.h"
#include "aui_rectangle.h"
#include "aui_ldl.h"

#include "aui_mouse.h"

#include "profileDB.h"

#include "c3ui.h"

#ifdef USE_SDL
#include <SDL.h>
#include <SDL_thread.h>
#endif

#include <time.h>

sint32 aui_Mouse::m_mouseRefCount = 0;
#ifdef USE_SDL
SDL_mutex *aui_Mouse::m_lpcs = NULL;
#else
LPCRITICAL_SECTION aui_Mouse::m_lpcs = NULL;
#endif

#ifdef USE_SDL
BOOL g_mouseShouldTerminateThread = FALSE;
#endif

#define k_AUI_MOUSE_THREAD_SLEEP_TIME	10

#include "civapp.h"
extern CivApp		*g_civApp;

aui_Mouse::aui_Mouse(
	AUI_ERRCODE *retval,
	const MBCHAR *ldlBlock )
	:
	aui_Input( retval )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
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
	m_privateMix = NULL;
	m_pickup = NULL;
	m_prevPickup = NULL;
	m_suspendCount = 0;
	m_showCount = 0;
	m_reset = TRUE;
	m_thread = NULL;
#ifndef __AUI_USE_SDL__
	m_threadId = 0;
	m_threadEvent = NULL;
	m_terminateEvent = NULL;
	m_suspendEvent = NULL;
	m_resumeEvent = NULL;
	m_replyEvent = NULL;
#endif
	m_flags = 0;

	memset( m_cursors, 0, sizeof( m_cursors ) );
	memset( &m_data, 0, sizeof( m_data ) );
	memset( m_inputs, 0, sizeof( m_inputs ) );

	SetClip( NULL );
#ifndef __AUI_USE_SDL__
	GetCursorPos( &m_data.position );
#else
	int x = 0, y = 0;
	SDL_GetMouseState(&x, &y);
	m_data.position.x = x;
	m_data.position.y = y;
#endif

	if ( !m_mouseRefCount++ )
	{
#ifdef USE_SDL
		m_lpcs = SDL_CreateMutex();
#else
		m_lpcs = new CRITICAL_SECTION;
#endif
		Assert( m_lpcs != NULL );
#ifndef USE_SDL
		if ( m_lpcs )
			InitializeCriticalSection( m_lpcs );
		else
#else
		if (!m_lpcs)
#endif
			return AUI_ERRCODE_MEMALLOCFAILED;
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_Mouse::InitCommonLdl(const MBCHAR *ldlBlock)
{
	AUI_ERRCODE errcode = InitCommon();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	if ( ldlBlock )
	{
		aui_Ldl *theLdl = g_ui->GetLdl();

		BOOL valid = theLdl->IsValid( ldlBlock );
		Assert( valid );
		if ( !valid ) return AUI_ERRCODE_HACK;

		ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
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

					Assert( cursor->TheSurface() != NULL );
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

			ldl_datablock *blk = theLdl->GetLdl()->FindDataBlock( temp );
			if ( !blk ) break;

			POINT indexes;
			indexes.x = blk->GetInt( k_MOUSE_LDL_FIRSTINDEX );
			indexes.y = blk->GetInt( k_MOUSE_LDL_LASTINDEX );
			m_animIndexList.AddTail( indexes );
			m_animDelayList.AddTail( blk->GetInt( k_MOUSE_LDL_ANIMDELAY ) );
		}
	}

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

	DestroyPrivateBuffers();

	m_animIndexList.DeleteAll();
	m_animDelayList.DeleteAll();

	if ( !--m_mouseRefCount )
	{
		if ( m_lpcs )
		{
#ifdef USE_SDL
			SDL_DestroyMutex(m_lpcs);
#else
			DeleteCriticalSection( m_lpcs );
			delete m_lpcs;
#endif
			m_lpcs = NULL;
		}
	}
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

aui_MouseEvent *aui_Mouse::GetLatestMouseEvent( void )
{
	return &m_data;
}

void aui_Mouse::SetCursor( sint32 index, const MBCHAR *cursor )
{
	Assert( index >= 0 && index < k_MOUSE_MAXNUMCURSORS );
	if ( index < 0 || index >= k_MOUSE_MAXNUMCURSORS ) return;

	aui_Cursor *oldCursor = m_cursors[ index ];
	if ( cursor )
	{
		aui_Cursor *c = g_ui->LoadCursor( cursor );
		Assert( cursor != NULL );
		if ( !cursor ) return;

		Assert( c->TheSurface()->Width() <= k_MOUSE_MAXSIZE );
		Assert( c->TheSurface()->Height() <= k_MOUSE_MAXSIZE );
		if ( c->TheSurface()->Width() > k_MOUSE_MAXSIZE
		||   c->TheSurface()->Height() > k_MOUSE_MAXSIZE )
		{
			g_ui->UnloadCursor( c );
			return;
		}

		m_cursors[ index ] = c;
	}
	else
		m_cursors[ index ] = NULL;

	if ( oldCursor ) g_ui->UnloadCursor( oldCursor );
}


void aui_Mouse::SetCurrentCursor( sint32 index )
{
	Assert( index >= 0 && index < k_MOUSE_MAXNUMCURSORS );
	if ( index < 0 || index >= k_MOUSE_MAXNUMCURSORS ) return;

	Assert( index >= m_firstIndex && index <= m_lastIndex );
	if ( index < m_firstIndex || index > m_lastIndex ) return;

	m_curCursor = m_cursors + index;
}

aui_Cursor *aui_Mouse::GetCurrentCursor( void ) const
{
	return *m_curCursor;
}

sint32 aui_Mouse::GetCurrentCursorIndex(void)
{
	sint32 i=0;

	for (i=m_firstIndex; i<=m_lastIndex; i++) {
		if (m_curCursor == (m_cursors + i))
			return i;
	}

	return 0;
}

uint32 aui_Mouse::GetAnimDelay( void ) const
{
	return m_animDelay;
}

void aui_Mouse::SetAnimDelay( uint32 animDelay )
{
	m_animDelay = animDelay;
}

void aui_Mouse::GetAnimIndexes( sint32 *firstIndex, sint32 *lastIndex )
{
	if ( firstIndex ) *firstIndex = m_firstIndex;
	if ( lastIndex ) *lastIndex = m_lastIndex;
}

void aui_Mouse::SetAnimIndexes( sint32 firstIndex, sint32 lastIndex )
{
	Assert( firstIndex >= 0 );
	if ( firstIndex < 0 ) return;
	Assert( lastIndex >= 0 );
	if ( lastIndex < 0 ) return;
	Assert( firstIndex <= lastIndex );
	if ( firstIndex > lastIndex ) return;

	m_firstIndex = firstIndex;
	m_lastIndex = lastIndex;
}


void aui_Mouse::SetAnim( sint32 anim )
{
	Assert( anim >= 0 );
	if ( anim < 0 ) return;

	SetAnimIndexes(anim, anim);
}




AUI_ERRCODE aui_Mouse::Start( void )
{
	CreatePrivateBuffers();

#ifdef USE_SDL
	m_thread = SDL_CreateThread(MouseThreadProc, this);
#else
	m_thread =
		CreateThread( NULL, 0, MouseThreadProc, (LPVOID)this, 0, &m_threadId );
#endif

	m_curCursor = m_cursors + m_firstIndex;

	if ( m_thread )
	{
#ifndef USE_SDL
		SetThreadPriority( m_thread, THREAD_PRIORITY_NORMAL );

		m_threadEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		m_terminateEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		m_suspendEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		m_resumeEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		m_replyEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
#endif

		Acquire();
	}

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_Mouse::CreatePrivateBuffers( void )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

	DestroyPrivateBuffers();

#if defined(__AUI_USE_SDL) || defined(__AUI_USE_DIRECTX__)
	m_privateMix = aui_Factory::new_Surface(
			&retcode,
			k_MOUSE_MAXSIZE,
			k_MOUSE_MAXSIZE,
			g_ui->Primary()->BitsPerPixel(),
			NULL,
			FALSE,
			FALSE );
	Assert( AUI_NEWOK(m_privateMix,retcode) );
	if ( !AUI_NEWOK(m_privateMix,retcode) ) return retcode;

	m_pickup = aui_Factory::new_Surface(
			&retcode,
			k_MOUSE_MAXSIZE,
			k_MOUSE_MAXSIZE,
			g_ui->Primary()->BitsPerPixel(),
			NULL,
			FALSE,
			FALSE );
	Assert( AUI_NEWOK(m_pickup,retcode) );
	if ( !AUI_NEWOK(m_pickup,retcode) ) return retcode;

	m_prevPickup = aui_Factory::new_Surface(
			&retcode,
			k_MOUSE_MAXSIZE,
			k_MOUSE_MAXSIZE,
			g_ui->Primary()->BitsPerPixel(),
			NULL,
			FALSE,
			FALSE );
	Assert( AUI_NEWOK(m_prevPickup,retcode) );
	if ( !AUI_NEWOK(m_prevPickup,retcode) ) return retcode;
#else
	m_privateMix = new aui_Surface(
			&retcode,
			k_MOUSE_MAXSIZE,
			k_MOUSE_MAXSIZE,
			g_ui->Primary()->BitsPerPixel() );
	Assert( AUI_NEWOK(m_privateMix,retcode) );
	if ( !AUI_NEWOK(m_privateMix,retcode) ) return retcode;

	m_pickup = new aui_Surface(
			&retcode,
			k_MOUSE_MAXSIZE,
			k_MOUSE_MAXSIZE,
			g_ui->Primary()->BitsPerPixel() );
	Assert( AUI_NEWOK(m_pickup,retcode) );
	if ( !AUI_NEWOK(m_pickup,retcode) ) return retcode;

	m_prevPickup = new aui_Surface(
			&retcode,
			k_MOUSE_MAXSIZE,
			k_MOUSE_MAXSIZE,
			g_ui->Primary()->BitsPerPixel() );
	Assert( AUI_NEWOK(m_prevPickup,retcode) );
	if ( !AUI_NEWOK(m_prevPickup,retcode) ) return retcode;
#endif

	return retcode;
}


void aui_Mouse::DestroyPrivateBuffers( void )
{
	if ( m_privateMix )
	{
		delete m_privateMix;
		m_privateMix = NULL;
	}

	if ( m_pickup )
	{
		delete m_pickup;
		m_pickup = NULL;
	}

	if ( m_prevPickup )
	{
		delete m_prevPickup;
		m_prevPickup = NULL;
	}
}

uint32 aui_Mouse::GetFlags(void)
{
	return m_flags;
}

void aui_Mouse::SetFlags(uint32 flags)
{
	m_flags = flags;
}

AUI_ERRCODE aui_Mouse::End( void )
{

	Unacquire();

	if ( m_thread )
	{
#ifndef USE_SDL
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
#else
		SDL_KillThread(m_thread);
#endif

		Erase();

#ifndef USE_SDL
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
#endif
		m_thread = NULL;
#ifndef USE_SDL
		m_threadId = 0;
#endif
	}

#ifndef USE_SDL
	SetCursorPos( m_data.position.x, m_data.position.y );
#endif

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_Mouse::Suspend( BOOL eraseCursor )
{

	if ( !m_thread ) return AUI_ERRCODE_NOTHREAD;

	if ( m_suspendCount )
	{
		m_suspendCount++;
		return AUI_ERRCODE_OK;
	}

#ifdef USE_SDL
	return AUI_ERRCODE_OK;
#else
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
	if ( !m_thread ) return AUI_ERRCODE_NOTHREAD;

	if ( m_suspendCount == 0 ) return AUI_ERRCODE_OK;


	if ( m_suspendCount > 1 )
	{
		m_suspendCount--;
		return AUI_ERRCODE_OK;
	}

#ifdef USE_SDL
	return AUI_ERRCODE_OK;
#else
	if ( ResumeThread( m_thread ) != 0xffffffff )
	{

		if ( !m_acquired ) Acquire();

		SetEvent( m_resumeEvent );

		m_suspendCount = 0;

		return AUI_ERRCODE_OK;
	}

	return AUI_ERRCODE_RESUMEFAILED;
#endif
}

AUI_ERRCODE aui_Mouse::Show( void )
{
	m_showCount++;
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Mouse::Hide( void )
{
	if ( !m_showCount )
	{

		Suspend( TRUE );
		m_showCount--;
		Resume();
	}
	return AUI_ERRCODE_OK;
}

BOOL aui_Mouse::IsSuspended( void ) const
{
	return m_suspendCount;
}

BOOL aui_Mouse::IsHidden( void ) const
{
	return m_showCount < 0;
}

sint32	aui_Mouse::X( void )
{
	return m_data.position.x;
}

sint32	aui_Mouse::Y( void )
{
	return m_data.position.y;
}

inline BOOL aui_Mouse::ShouldTerminateThread( void )
{
#ifndef USE_SDL
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
#else
	return g_mouseShouldTerminateThread;
#endif
}


#ifdef USE_SDL
int MouseThreadProc(void *param)
#else
DWORD WINAPI MouseThreadProc( LPVOID param )
#endif
{
	aui_Mouse *mouse = (aui_Mouse *)param;

	while ( !mouse->ShouldTerminateThread()  )
	{
#ifdef WIN32
		Sleep( k_AUI_MOUSE_THREAD_SLEEP_TIME );
#else
		usleep( k_AUI_MOUSE_THREAD_SLEEP_TIME );
#endif
		mouse->HandleAnim();

		mouse->GetInput();

		mouse->ReactToInput();

		mouse->ManipulateInputs( mouse->GetLatestMouseEvent(), TRUE );
	}

	return 0;
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

	aui_Cursor *cursor = m_cursors[ index ];
	if ( cursor )
		return cursor->SetHotspot( x, y );

	return AUI_ERRCODE_OK;
}

double &aui_Mouse::Sensitivity( void )
{
	return m_sensitivity;
}

aui_Cursor *aui_Mouse::GetCursor( sint32 index ) const
{
	return m_cursors[ index ];
}

AUI_ERRCODE aui_Mouse::ReactToInput( void ){
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	POINT hotspot;
	(*m_curCursor)->GetHotspot( &hotspot.x, &hotspot.y );

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
		g_ui->Primary(),
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

	errcode = g_ui->TheBlitter()->Blt(
		g_ui->Primary(),
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

	errcode = g_ui->TheBlitter()->Blt(
		g_ui->Primary(),
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

	errcode = g_ui->TheBlitter()->Blt(
		g_ui->Primary(),
		eraser2.left,
		eraser2.top,
		m_prevPickup,
		&prevRect,
		k_AUI_BLITTER_FLAG_COPY );
	Assert( errcode == AUI_ERRCODE_OK );

	aui_Surface *tempSurf = m_prevPickup;
	m_prevPickup = m_pickup;
	m_pickup = tempSurf;

	CopyRect( &prevRect, &rect );
	CopyRect( &prevUnclippedMixRect, &unclippedMixRect );

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_Mouse::HandleAnim( void )
{

	if ( m_lastIndex >= m_firstIndex )
	{

		if(m_curCursor < m_cursors + m_firstIndex)
			m_curCursor = m_cursors + m_firstIndex;

		uint32 now = GetTickCount();
		if ( now - m_time > m_animDelay )
		{

			if ( m_curCursor++ >= m_cursors + m_lastIndex )
				m_curCursor = m_cursors + m_firstIndex;

			m_time = now;

			return AUI_ERRCODE_HANDLED;
		}
	}

	return AUI_ERRCODE_UNHANDLED;
}


AUI_ERRCODE	aui_Mouse::BltWindowToPrimary( aui_Window *window )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
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
		(*m_curCursor)->GetHotspot( &hotspot.x, &hotspot.y );

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
			g_ui->TheBlitter()->ColorBlt(
				g_ui->Primary(),
				&screenDirtyRect,
				RGB(255,255,255),
				0 );
#ifdef WIN32
			Sleep( 20 );
#else
			struct timespec t = { 0, 20000000L };
			nanosleep(&t, NULL);
#endif
			fclose(f);
		}
#endif

		errcode = g_ui->TheBlitter()->Blt(
			g_ui->Primary(),
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

	return retcode;
}

AUI_ERRCODE	aui_Mouse::BltDirtyRectInfoToPrimary( void )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
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
		errcode = g_ui->Primary()->Lock( NULL, &primaryBuf, 0 );
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
		(*m_curCursor)->GetHotspot( &hotspot.x, &hotspot.y );

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
				g_ui->Primary(),
				&screenDirtyRect,
				RGB(255,255,255),
				0 );
			Sleep( 20 );
		}
#endif


		if (!g_civApp->IsInBackground()) {

			errcode = g_ui->TheBlitter()->Blt(
				g_ui->Primary(),
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

	if (!g_theProfileDB || !g_theProfileDB->IsUseDirectXBlitter())
	{
		errcode = g_ui->Primary()->Unlock( primaryBuf );
		Assert( errcode == AUI_ERRCODE_OK );
	}

	return retcode;
}




AUI_ERRCODE	aui_Mouse::BltBackgroundColorToPrimary(
	COLORREF color,
	aui_DirtyList *colorAreas )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	if (g_civApp->IsInBackground()) return retcode;

	POINT hotspot;
	(*m_curCursor)->GetHotspot( &hotspot.x, &hotspot.y );

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

		errcode = g_ui->TheBlitter()->Blt(
			g_ui->Primary(),
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
			errcode = g_ui->TheBlitter()->ColorBlt(
				g_ui->Primary(),
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

	return retcode;
}




AUI_ERRCODE	aui_Mouse::BltBackgroundImageToPrimary(
	aui_Image *image,
	RECT *imageRect,
	aui_DirtyList *imageAreas )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	if (g_civApp->IsInBackground()) return retcode;

	POINT hotspot;
	(*m_curCursor)->GetHotspot( &hotspot.x, &hotspot.y );

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

		errcode = g_ui->TheBlitter()->Blt(
			g_ui->Primary(),
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

			errcode = g_ui->TheBlitter()->Blt(
				g_ui->Primary(),
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

	return retcode;
}

#ifdef USE_SDL
SDL_mutex *aui_Mouse::LPCS(void) const
{
	return m_lpcs;
}
#else // USE_SDL
LPCRITICAL_SECTION aui_Mouse::LPCS( void ) const
{
	return m_lpcs;
}
#endif // USE_SDL

AUI_ERRCODE aui_Mouse::Erase( void )
{
	AUI_ERRCODE errcode;

	POINT hotspot;
	(*m_curCursor)->GetHotspot( &hotspot.x, &hotspot.y );

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

	errcode = g_ui->TheBlitter()->Blt(
		g_ui->Primary(),
		mixRect.left,
		mixRect.top,
		m_prevPickup,
		&rect,
		k_AUI_BLITTER_FLAG_COPY );

	m_reset = TRUE;

	return AUI_ERRCODE_OK;
}


sint32 aui_Mouse::ManipulateInputs( aui_MouseEvent *data, BOOL add )
{
	sint32 numManipulated = 0;
	static sint32 index = 0;

#ifndef USE_SDL
	EnterCriticalSection( m_lpcs );
#else
	SDL_mutexP(m_lpcs);
#endif

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

#ifndef USE_SDL
	LeaveCriticalSection( m_lpcs );
#else
	SDL_mutexV(m_lpcs);
#endif

	return numManipulated;
}
