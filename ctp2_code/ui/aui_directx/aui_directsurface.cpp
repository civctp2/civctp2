//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface DirectX surface
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
// __AUI_USE_DIRECTX__
// Use DirectX
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added back buffering capability. (1-Jan-2010 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"


#ifdef __AUI_USE_DIRECTX__


#include "aui_ui.h"
#include "aui_uniqueid.h"

#include "aui_directsurface.h"



uint32 aui_DirectSurface::m_directSurfaceClassId = aui_UniqueId();



aui_DirectSurface::aui_DirectSurface(
	AUI_ERRCODE *retval,
	sint32 width,
	sint32 height,
	sint32 bpp,
	LPDIRECTDRAW lpdd,
	LPDIRECTDRAWSURFACE lpdds,
	BOOL isPrimary,
	BOOL useVideoMemory,
	LPDIRECTDRAWSURFACE back)
	:
	aui_Surface(),
	m_back      (back)
{
	*retval = aui_Surface::InitCommon( width, height, bpp, isPrimary );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	HRESULT hr;

	m_lpdds = lpdds;
	if (!m_lpdds)
	{
		if ( !lpdd )
		{
			*retval = AUI_ERRCODE_INVALIDDIMENSION;
			return;
		}

		DDSURFACEDESC ddsd;
		memset( &ddsd, 0, sizeof( ddsd ) );
		ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		if ( useVideoMemory )
			ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		else
			ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
		ddsd.dwHeight = m_height;
		ddsd.dwWidth = m_width;

		hr = lpdd->CreateSurface( &ddsd, &m_lpdds, NULL );
		Assert( hr == DD_OK );
		if ( hr == DD_OK ) m_allocated = TRUE;
		else
		{
			*retval = AUI_ERRCODE_MEMALLOCFAILED;
			return;
		}

		DDBLTFX ddbfx;
		memset( &ddbfx, 0, sizeof( ddbfx ) );
		ddbfx.dwSize = sizeof( ddbfx );
		ddbfx.dwFillColor = 0x00000000;

		hr = m_lpdds->Blt(
			NULL,
			NULL,
			NULL,
			DDBLT_COLORFILL | DDBLT_WAIT,
			&ddbfx );
		Assert( hr == DD_OK );


		if (hr != DD_OK) {
			MBCHAR *s;
			
			switch (hr) {
			case DDERR_GENERIC  :	
				s = "Generic";
				break;
			case DDERR_INVALIDCLIPLIST:  
				s = "Invalid Clip List";
				break;
			case DDERR_INVALIDOBJECT  :
				s = "Invalid Object";
				break;
			case DDERR_INVALIDPARAMS  :
				s = "Invalid Params";
				break;
			case DDERR_INVALIDRECT  :
				s = "Invalid Rect";
				break;
			case DDERR_NOALPHAHW  :
				s = "No Alpha HW";
				break;
			case DDERR_NOBLTHW  :
				s = "No BLIT HW";
				break;
			case DDERR_NOCLIPLIST  :
				s = "No Clip List";
				break;
			case DDERR_NODDROPSHW  :
				s = "No Drops HW";
				break;
			case DDERR_NOMIRRORHW  :
				s = "No Mirror HW";
				break;
			case DDERR_NORASTEROPHW  :
				s = "No Rasterop HW";
				break;
			case DDERR_NOROTATIONHW  :
				s = "No Rotation HW";
				break;
			case DDERR_NOSTRETCHHW  :
				s = "No Stretch HW";
				break;
			case DDERR_NOZBUFFERHW  :
				s = "No Z Buffer HW";
				break;
			case DDERR_SURFACEBUSY  :
				s = "Surface Busy";
				break;
			case DDERR_SURFACELOST  :
				s = "Surface Lost";
				break;
			case DDERR_UNSUPPORTED  :
				s = "Unsupported";
				break;
			case DDERR_WASSTILLDRAWING :
				s = "Was Still Drawing";
				break;
			case DDERR_EXCEPTION :
				s = "Unsupported";
				break;
			default:
				s = "Unknown";
				break;
			}
		}
	}
	
	
	DDPIXELFORMAT pixelFormat;
	memset( &pixelFormat, 0, sizeof( pixelFormat ) );
	pixelFormat.dwSize = sizeof( pixelFormat );
	hr = m_lpdds->GetPixelFormat( &pixelFormat );
	Assert( hr == DD_OK );
	if ( hr != DD_OK )
	{
		*retval = AUI_ERRCODE_BADPIXELFORMAT;
		return;
	}

	
	Assert( pixelFormat.dwFlags & DDPF_RGB );
	if ( !( pixelFormat.dwFlags & DDPF_RGB ) )
	{
		*retval = AUI_ERRCODE_BADPIXELFORMAT;
		return;
	}

	
	if ( bpp == 16 )
	{
		if ( pixelFormat.dwRBitMask == 0xF800 )
			m_pixelFormat = AUI_SURFACE_PIXELFORMAT_565;
		else
			m_pixelFormat = AUI_SURFACE_PIXELFORMAT_555;
	}

	
	SetChromaKey( m_chromaKey = 0x00000000 );

	DDSURFACEDESC ddsd;
	memset( &ddsd, 0, sizeof( ddsd ) );
	ddsd.dwSize = sizeof( ddsd );
	hr = m_lpdds->Lock(
		NULL,
		&ddsd,
		DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
		NULL );
	Assert( hr == DD_OK );
	if ( hr == DD_OK )
	{
		m_pitch = ddsd.lPitch;
		m_size = m_pitch * m_height;
	//	m_saveBuffer = (uint8 *)ddsd.lpSurface; // This might become invalid
		m_lpdds->Unlock( ddsd.lpSurface );
	}
	else
	{
		*retval = AUI_ERRCODE_SURFACELOCKFAILED;
		return;
	}
}



AUI_ERRCODE aui_DirectSurface::InitCommon( void )
{
	m_lpdds = NULL;

	return AUI_ERRCODE_OK;
}



aui_DirectSurface::~aui_DirectSurface()
{
	if ( m_allocated && m_lpdds )
	{
		m_lpdds->Release();
		m_lpdds = NULL;
		m_allocated = FALSE;
	}
}



uint32 aui_DirectSurface::SetChromaKey( uint32 color )
{
	DDCOLORKEY ddck;
	memset( &ddck, 0, sizeof( ddck ) );
	ddck.dwColorSpaceLowValue = ddck.dwColorSpaceHighValue = color;

	HRESULT hr = m_lpdds->SetColorKey( DDCKEY_SRCBLT, &ddck );
	Assert( hr == DD_OK );
	if ( hr == DD_OK )
		return aui_Surface::SetChromaKey( color );
	else
		return (uint32)-1;
}



BOOL aui_DirectSurface::IsOK( void ) const
{
	return m_lpdds->IsLost() == DD_OK && m_saveBuffer != NULL;
}

void aui_DirectSurface::Flip()
{
	if(m_exclusiveMode)
	{
		m_lpdds->Flip(m_back, DDFLIP_WAIT);
	}
	else if(m_back != NULL)
	{
		m_lpdds->Blt(NULL, m_back, NULL, 0, NULL);
	}
}

AUI_ERRCODE aui_DirectSurface::Lock( RECT *rect, LPVOID *buffer, DWORD flags )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	DDSURFACEDESC ddsd;
	memset( &ddsd, 0, sizeof( ddsd ) );
	ddsd.dwSize = sizeof( ddsd );

	if ( g_ui->DXVer() >= 0x500 && !m_isPrimary ) flags |= DDLOCK_NOSYSLOCK;

	HRESULT hr;

	if(m_back == NULL)
	{
		hr = m_lpdds->Lock(
				rect,
				&ddsd,
				DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | flags,
				NULL );
	}
	else
	{
		hr = m_back->Lock(
				rect,
				&ddsd,
				DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | flags,
				NULL );
	}

	switch ( hr )
	{
	case DD_OK:
		*buffer = ddsd.lpSurface;
		m_saveBuffer = static_cast<uint8*>(ddsd.lpSurface);
		errcode = ManipulateLockList( rect, buffer, AUI_SURFACE_LOCKOP_ADD );
		break;

	case DDERR_INVALIDPARAMS:
		errcode = AUI_ERRCODE_INVALIDDIMENSION;
		break;

	case DDERR_SURFACEBUSY:
	case DDERR_WASSTILLDRAWING:
		errcode = AUI_ERRCODE_SURFACEBUSY;
		break;

	default:
		errcode = AUI_ERRCODE_SURFACELOCKFAILED;
		break;
	}
	
	return errcode;
}




AUI_ERRCODE aui_DirectSurface::Unlock( LPVOID buffer )
{
	AUI_ERRCODE errcode =
		ManipulateLockList( NULL, &buffer, AUI_SURFACE_LOCKOP_REMOVE );

	if ( errcode == AUI_ERRCODE_OK )
	{
		HRESULT hr;
		if(m_back == NULL)
		{
			hr = m_lpdds->Unlock( buffer );
		}
		else
		{
			hr = m_back->Unlock( buffer );
		}

		m_saveBuffer = NULL;

		if ( hr != DD_OK ) errcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return errcode;
}



AUI_ERRCODE aui_DirectSurface::GetDC( HDC *hdc )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	Assert( !m_dcIsGot );
	if ( m_dcIsGot )
		errcode = AUI_ERRCODE_SURFACELOCKFAILED;
	else
	{
		HRESULT hr = m_lpdds->GetDC( hdc );
		if ( hr != DD_OK )
			errcode = AUI_ERRCODE_SURFACELOCKFAILED;
		else
			m_dcIsGot = true;
	}

	return errcode;
}



AUI_ERRCODE aui_DirectSurface::ReleaseDC( HDC hdc )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	Assert( m_dcIsGot );
	if ( !m_dcIsGot )
		errcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
	else
	{
		HRESULT hr = m_lpdds->ReleaseDC( hdc );
		if ( hr != DD_OK )
			errcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		else
			m_dcIsGot = false;
	}

	return errcode;
}

AUI_ERRCODE aui_DirectSurface::Blank(const uint32 &color)
{
	DDBLTFX ddbltfx;
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = color;

        AUI_ERRCODE errcode = static_cast<AUI_ERRCODE>
        (m_lpdds->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&ddbltfx));

        Assert(errcode == AUI_ERRCODE_OK);
        return (AUI_ERRCODE_OK == errcode) ? AUI_ERRCODE_OK : AUI_ERRCODE_BLTFAILED;
}


#endif 
