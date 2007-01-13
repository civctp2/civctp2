//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Surface (part of the screen)
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
// - Corrected a reported memory leak.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_ui.h"
#include "aui_surface.h"
#include "aui_uniqueid.h"

#include "aui_rectangle.h"



sint32 aui_Surface::m_surfaceRefCount = 0;
CRITICAL_SECTION	aui_Surface::m_cs;
uint32 aui_Surface::m_surfaceClassId = aui_UniqueId();

extern sint32 g_is565Format;


aui_Surface::aui_Surface(
	AUI_ERRCODE *retval,
	sint32 width,
	sint32 height,
	sint32 bpp,
	sint32 pitch,
	uint8 *buffer,
	BOOL isPrimary )
	:
	aui_Base()
{
	*retval = InitCommon( width, height, bpp, isPrimary );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	
	m_pitch = pitch ?
		pitch :
		m_bytewidth + ( uint32(-m_bytewidth) & 0x3 );

	m_size = m_pitch * m_height;

	
	if ( !(m_saveBuffer = buffer) )
	{
		HDC hdc = ::GetDC( g_ui->TheHWND() );

		
		m_hdc = CreateCompatibleDC( hdc );
		Assert( m_hdc != NULL );
		if ( !m_hdc ) return;

		
		m_hbitmap = CreateCompatibleBitmap( hdc, m_width, m_height );
		Assert( m_hbitmap != NULL );
		if ( !m_hbitmap ) return;

		::ReleaseDC( g_ui->TheHWND(), hdc );

		
		m_holdbitmap = (HBITMAP)SelectObject( m_hdc, m_hbitmap );

		
		RECT rect = { 0, 0, m_width, m_height };
		FillRect( m_hdc, &rect, (HBRUSH)GetStockObject( BLACK_BRUSH ) );

		
		m_saveBuffer = (uint8 *)(new uint32[ m_size >> 2 ]);
		Assert( m_saveBuffer != NULL );

		
		if ( m_allocated = (BOOL)m_saveBuffer )
			memset( m_saveBuffer, 0x00, m_size );
		else
		{
			*retval = AUI_ERRCODE_MEMALLOCFAILED;
			return;
		}
	}
}



AUI_ERRCODE aui_Surface::InitCommon( sint32 width, sint32 height, sint32 bpp, BOOL isPrimary )
{
	m_pixelFormat = AUI_SURFACE_PIXELFORMAT_UNKNOWN,
	m_chromaKey = 0x00000000, 
	m_isPrimary = isPrimary,
	m_buffer = NULL,
	m_hdc = NULL,
	m_dcIsGot = FALSE,
	m_hbitmap = NULL,
	m_holdbitmap = NULL,
	m_saveBuffer = NULL,
	m_allocated = FALSE,
	m_locksRemain = k_SURFACE_MAXLOCK;

	memset( m_locklist, 0, sizeof( m_locklist ) );

	m_width = width;
	m_height = height;
	m_bpp = bpp;
	m_Bpp = m_bpp >> 3;
	m_bytewidth = m_width * m_Bpp;

	
	if ( !m_surfaceRefCount++ )
	{
		InitializeCriticalSection(&m_cs);
	}

	

	if ( bpp == 16 ) 
		if (g_is565Format) {
			m_pixelFormat = AUI_SURFACE_PIXELFORMAT_565;
		} else {
			m_pixelFormat = AUI_SURFACE_PIXELFORMAT_555;
		}


	return AUI_ERRCODE_OK;
}



aui_Surface::~aui_Surface()
{
	if ( m_allocated )
	{
		SelectObject( m_hdc, m_holdbitmap );
		DeleteObject( m_hbitmap );
		DeleteObject( m_hdc );
		m_hdc = NULL;
		m_hbitmap = NULL;
		m_holdbitmap = NULL;

		delete[ m_size >> 2 ] m_saveBuffer;
		m_saveBuffer = m_buffer = NULL;
		m_allocated = FALSE;
	}

	if ( !--m_surfaceRefCount )
	{
		DeleteCriticalSection(&m_cs);
	}
}



uint32 aui_Surface::SetChromaKey( uint32 color )
{
	uint32 prevColor = m_chromaKey;
	m_chromaKey = color;
	return prevColor;
}



uint32 aui_Surface::SetChromaKey( uint8 red, uint8 green, uint8 blue )
{
	sint32 r = red, g = green, b = blue;

	switch ( m_Bpp )
	{
	case 1:
		
		return SetChromaKey(
			m_pixelFormat == AUI_SURFACE_PIXELFORMAT_332
			?
			( blue >> 5 ) |
			( ( green >> 5 ) << 3 ) |
			( ( red >> 6 ) << 6 )		
			:
			(uint32)red );				

	case 2:
		switch ( m_pixelFormat )
		{
		case AUI_SURFACE_PIXELFORMAT_555:
			
			return SetChromaKey(	((r & 0xF8) << 7) |
									((g & 0xF8) << 2) |
									((b & 0xF8) >> 3));




		case AUI_SURFACE_PIXELFORMAT_565:
			
			return SetChromaKey(	((r & 0xF8) << 8) |
									((g & 0xF8) << 3) |
									((b & 0xF8) >> 3));




		default:
			
			Assert( FALSE );
			break;
		}
		break;

	case 3:
		Assert( m_pixelFormat == AUI_SURFACE_PIXELFORMAT_888 );

		
		return SetChromaKey( RGB(red,green,blue) );

	default:
		
		Assert( FALSE );
		break;
	}

	return (uint32)-1;
}





AUI_ERRCODE aui_Surface::Lock( RECT *rect, LPVOID *buffer, DWORD flags )
{
	*buffer = NULL;

	
	while ( ManipulateLockList( rect, buffer, AUI_SURFACE_LOCKOP_ADD ) != AUI_ERRCODE_OK )
		;

	return AUI_ERRCODE_OK;
}





AUI_ERRCODE aui_Surface::Unlock( LPVOID buffer )
{
	return ManipulateLockList( NULL, &buffer, AUI_SURFACE_LOCKOP_REMOVE );
}



AUI_ERRCODE aui_Surface::GetDC( HDC *hdc )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	
	Assert( hdc != NULL );
	if ( !hdc ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( m_allocated != FALSE );
    if ( !m_allocated ) return AUI_ERRCODE_SURFACELOCKFAILED;

	Assert( !m_dcIsGot );
	if ( m_dcIsGot )
		errcode = AUI_ERRCODE_SURFACELOCKFAILED;
	else
	{
		if ( *hdc = m_hdc )
		{
			m_dcIsGot = TRUE;

			
			
			BITMAPINFOHEADER bih;
			memset( &bih, 0, sizeof( bih ) );
			bih.biSize = sizeof( bih );
			bih.biWidth = m_width;
			bih.biHeight = -m_height;
			bih.biPlanes = 1;
			bih.biBitCount = GetDeviceCaps( m_hdc, BITSPIXEL );
			bih.biCompression = BI_RGB;

			SetDIBits(
				m_hdc,
				m_hbitmap,
				0,
				m_height,
				m_saveBuffer,
				(BITMAPINFO *)&bih, 
				DIB_RGB_COLORS );
		}
		else
			errcode = AUI_ERRCODE_HACK;
	}

	return errcode;
}



AUI_ERRCODE aui_Surface::ReleaseDC( HDC hdc )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	Assert( m_allocated != FALSE );
    if ( !m_allocated ) return AUI_ERRCODE_SURFACEUNLOCKFAILED;

	Assert( m_dcIsGot );
	if ( !m_dcIsGot )
		errcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
	else
	{
		if ( hdc == m_hdc )
		{
			m_dcIsGot = FALSE;

			
			
			BITMAPINFOHEADER bih;
			memset( &bih, 0, sizeof( bih ) );
			bih.biSize = sizeof( bih );
			bih.biWidth = m_width;
			bih.biHeight = -m_height;
			bih.biPlanes = 1;
			bih.biBitCount = GetDeviceCaps( m_hdc, BITSPIXEL );
			bih.biCompression = BI_RGB;

			GetDIBits(
				m_hdc,
				m_hbitmap,
				0,
				m_height,
				m_saveBuffer,
				(BITMAPINFO *)&bih, 
				DIB_RGB_COLORS );
		}
		else
			errcode = AUI_ERRCODE_INVALIDPARAM;
	}

	return errcode;
}

AUI_ERRCODE aui_Surface::Blank(const uint32 &color)
{
	return AUI_ERRCODE_BLTFAILED;
}

AUI_ERRCODE aui_Surface::BlankRGB(const uint8 &red, const uint8 &green, const uint8 &blue);
{
	uint32 color = 0;
	return Blank(color);
}


inline BOOL aui_Surface::IsLocked( RECT *rect )
{
	aui_SurfaceSubset *subset = m_locklist;
	for ( sint32 i = k_SURFACE_MAXLOCK; i; i--, subset++ )
		if ( subset->buffer && Rectangle_Intersect( &subset->rect, rect ) )
			return TRUE;

	return FALSE;
}




inline BOOL aui_Surface::IsLocked( LPVOID buffer )
{
	aui_SurfaceSubset *subset = m_locklist;
	for ( sint32 i = k_SURFACE_MAXLOCK; i; i--, subset++ )
		if ( subset->buffer == buffer )
			return TRUE;

	return FALSE;
}



AUI_ERRCODE aui_Surface::ManipulateLockList( RECT *rect, LPVOID *buffer, AUI_SURFACE_LOCKOP op )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	EnterCriticalSection(&m_cs);

	switch ( op )
	{
	case AUI_SURFACE_LOCKOP_ADD:
		Assert( m_locksRemain > 0 );
		if ( !m_locksRemain )
		{
			errcode = AUI_ERRCODE_SURFACEMAXLOCKED;
		}
		else
		{
			
			RECT entireSurfaceRect = { 0, 0, m_width, m_height };
			if ( !rect ) rect = &entireSurfaceRect;

			
			if ( IsLocked( rect ) )
			{
				errcode = AUI_ERRCODE_SURFACEBUSY;

				
				Assert( errcode != AUI_ERRCODE_SURFACEBUSY );
			}
			else
			{
				
				if ( !*buffer )
					*buffer = m_saveBuffer +
						rect->top * m_pitch +
						rect->left * m_Bpp;

				aui_SurfaceSubset *subset = m_locklist;
				for ( sint32 i = k_SURFACE_MAXLOCK; i; i--, subset++ )
				if ( !subset->buffer )
				{
					
					m_locksRemain--;

					
					m_buffer = m_saveBuffer;

					
					subset->buffer = *buffer;
					CopyRect( &subset->rect, rect );

					break;
				}
			}
		}

		break;

	case AUI_SURFACE_LOCKOP_REMOVE:
		if ( *buffer )
		{
			aui_SurfaceSubset *subset = m_locklist;
			for ( sint32 i = k_SURFACE_MAXLOCK; i; i--, subset++ )
			if ( subset->buffer == *buffer )
			{
				
				if ( ++m_locksRemain == k_SURFACE_MAXLOCK ) m_buffer = NULL;

				
				memset( subset, 0, sizeof( aui_SurfaceSubset ) );

				break;
			}
		}
		else
			errcode = AUI_ERRCODE_INVALIDPARAM;

		break;

	default:
	case AUI_SURFACE_LOCKOP_CHECK:
		if ( rect )
			errcode = IsLocked( rect ) ? AUI_ERRCODE_LOCKED : AUI_ERRCODE_UNLOCKED;
		else if ( *buffer )
			errcode = IsLocked( *buffer ) ? AUI_ERRCODE_LOCKED : AUI_ERRCODE_UNLOCKED;
		else
			errcode = AUI_ERRCODE_INVALIDPARAM;

		break;
	}

	LeaveCriticalSection(&m_cs);

	return errcode;
}
