










#include "c3.h"

#ifdef __AUI_USE_DIRECTX__
#include "aui_directui.h"
#include "aui_directsurface.h"
#else
#include "aui_ui.h"
#include "aui_surface.h"
#endif 

#include "aui_blitter.h"
#include "aui_memmap.h"
#include "aui_pixel.h"

#include "aui_image.h"



aui_Image::aui_Image(
	AUI_ERRCODE *retval,
	MBCHAR *filename )
	:
	aui_Base()
{

	
	if (filename==NULL)
	{
		*retval = AUI_ERRCODE_OK;
		return;
	}

	*retval = InitCommon( filename );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_Image::InitCommon( MBCHAR *filename )
{
	m_surface = NULL,
	m_format = NULL;

	AUI_ERRCODE errcode = SetFilename( filename );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	return AUI_ERRCODE_OK;
}



aui_Image::~aui_Image()
{
	Unload();
}



AUI_ERRCODE aui_Image::SetFilename( MBCHAR *filename )
{
	
	Unload();

	memset( m_filename, '\0', sizeof( m_filename ) );

	if ( !filename ) return AUI_ERRCODE_INVALIDPARAM;

	strncpy( m_filename, filename, MAX_PATH );

	
	m_format = (aui_ImageFormat *)
		g_ui->TheMemMap()->GetFileFormat( m_filename );
	Assert( m_format != NULL );
	if ( !m_format ) return AUI_ERRCODE_MEMALLOCFAILED;
	
	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_Image::Load( void )
{
	Assert( m_format != NULL );
	if ( !m_format ) return AUI_ERRCODE_INVALIDPARAM;

	
	if ( m_surface ) return AUI_ERRCODE_OK;

	AUI_ERRCODE retval=m_format->Load( m_filename, this );

	return retval;
}




AUI_ERRCODE aui_Image::Unload( void )
{
	if ( m_format )
	{
		g_ui->TheMemMap()->ReleaseFileFormat( m_format );
		m_format = NULL;
	}

	if ( m_surface )
	{
		delete m_surface;
		m_surface = NULL;
	}

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_Image::LoadEmpty( sint32 width, sint32 height, sint32 bpp )
{
	AUI_ERRCODE retcode;

#ifdef __AUI_USE_DIRECTX__
	m_surface = new aui_DirectSurface(
		&retcode,
		width,
		height,
		bpp,
		((aui_DirectUI *)g_ui)->DD() );
#else
	m_surface = new aui_Surface(
		&retcode,
		width,
		height,
		bpp );
#endif 

	Assert( AUI_NEWOK(m_surface,retcode) );

	return retcode;
}



AUI_ERRCODE aui_Image::LoadFileMapped( sint32 width, sint32 height,
                                       sint32 bpp, sint32 pitch, 
                                       uint8 *buffer )
{
	AUI_ERRCODE retcode;

	m_surface = new aui_Surface(
		&retcode,
		width,
		height,
		bpp,
        pitch,
        buffer );

	Assert( AUI_NEWOK(m_surface,retcode) );

	return retcode;
}







AUI_ERRCODE aui_BmpImageFormat::Load( MBCHAR *filename, aui_Image *image )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

	
	uint8 *filebits = g_ui->TheMemMap()->GetFileBits( filename );
	Assert( filebits != NULL );
	if ( !filebits ) return AUI_ERRCODE_HACK;

	
	BITMAPFILEHEADER bfh;
	memcpy( &bfh, filebits, sizeof( bfh ) );
	uint32 foffset = sizeof( bfh );

	
	if ( LOBYTE(bfh.bfType) != 'B' || HIBYTE(bfh.bfType) != 'M' ) {
		g_ui->TheMemMap()->ReleaseFileBits( filebits );
		return AUI_ERRCODE_LOADFAILED;
	}

	
	BITMAPINFOHEADER bih;
	memcpy( &bih, filebits + foffset, sizeof( bih ) );
	foffset += sizeof( bih );

	
	if ( bih.biCompression != BI_RGB ) {
		g_ui->TheMemMap()->ReleaseFileBits( filebits );
		return AUI_ERRCODE_LOADFAILED;
	}

	
	RGBQUAD *rgbq = NULL;
	if ( bih.biBitCount == 8 &&
		(bfh.bfOffBits - (sizeof(bih) + sizeof(bfh)) == (256 * sizeof( RGBQUAD ))) ) {

		rgbq = new RGBQUAD[256];
		Assert( rgbq != NULL );
		if ( !rgbq ) {
			g_ui->TheMemMap()->ReleaseFileBits( filebits );
			return AUI_ERRCODE_LOADFAILED;
		}

		memcpy( rgbq, filebits + foffset, ( 256 * sizeof( RGBQUAD ) ) );
		foffset += 256 * sizeof( RGBQUAD );
	}

	
	uint32 width = bih.biWidth;
	uint32 height = bih.biHeight >= 0 ? bih.biHeight : -bih.biHeight;

	
	sint32 temp = width * bih.biBitCount / 8;
	uint32 bmpPitch = temp + Mod(-temp,sizeof( LONG ));

	
	uint32 bpp = g_ui->Primary()->BitsPerPixel();

	
	AUI_ERRCODE errcode = image->LoadEmpty( width, height, bpp );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) )
	{
		g_ui->TheMemMap()->ReleaseFileBits( filebits );
		if ( rgbq ) delete rgbq;
		return AUI_ERRCODE_LOADFAILED;
	}

	aui_Surface *surface = image->TheSurface();

	
	switch ( bpp )
	{
	case 8:
		
		Assert( FALSE );
		retcode = AUI_ERRCODE_LOADFAILED;
		break;

	case 16:
		switch ( bih.biBitCount )
		{
		case 8:
			errcode = aui_Pixel::Convert8To16(
				surface,
				filebits + foffset,
				width,
				height,
				bmpPitch,
				rgbq );
			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_LOADFAILED;
			break;

		case 16:
			
			Assert( FALSE );
			retcode = AUI_ERRCODE_LOADFAILED;
			break;

		case 24:
			errcode = aui_Pixel::Convert24To16(
				surface,
				filebits + foffset,
				width,
				height,
				bmpPitch );
			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_LOADFAILED;
			break;

		default:
			
			Assert( FALSE );
			retcode = AUI_ERRCODE_LOADFAILED;
		}
		break;

	case 24:
		
		Assert( FALSE );
		retcode = AUI_ERRCODE_LOADFAILED;
		break;

	default:
		
		Assert( FALSE );
		retcode = AUI_ERRCODE_LOADFAILED;
		break;
	}

	
	if ( rgbq ) delete rgbq;
	g_ui->TheMemMap()->ReleaseFileBits( filebits );

	
	if ( bih.biHeight > 0 )
	{
		LPVOID bits = NULL;
		errcode = surface->Lock( NULL, &bits, 0 );
		Assert( AUI_SUCCESS(errcode) );
		if ( !AUI_SUCCESS(errcode) )
			retcode = AUI_ERRCODE_SURFACELOCKFAILED;
		else
		{
			const sint32 pitch = surface->Pitch();

			
			uint8 *temp = new uint8[ pitch ];
			Assert( temp != NULL );
			if ( !temp ) return AUI_ERRCODE_MEMALLOCFAILED;

			uint8 *top = (uint8 *)bits;
			uint8 *bot = top + pitch * ( surface->Height() - 1 );
			for ( sint32 i = surface->Height() / 2; i; i-- )
			{
				memcpy( temp, top, pitch );
				memcpy( top, bot, pitch );
				memcpy( bot, temp, pitch );

				top += pitch;
				bot -= pitch;
			}

			delete[ pitch ] temp;

			errcode = surface->Unlock( bits );
			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}


void aui_Image::SetChromakey(sint32 r, sint32 g, sint32 b)
{
	aui_Surface		*surf = TheSurface();

	Assert(surf != NULL);
	if (surf == NULL)
		return;

	
	
	
	
	Assert(r >= 0);
	Assert(r <= 255);
	Assert(g >= 0);
	Assert(g <= 255);
	Assert(b >= 0);
	Assert(b <= 255);


	surf->SetChromaKey((uint8) r, (uint8) g,(uint8) b);
}
