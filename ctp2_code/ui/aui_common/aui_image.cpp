//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface - image handling
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Crash prevented.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#ifdef __AUI_USE_DIRECTX__
#include "aui_directui.h"
#include "aui_directsurface.h"
#else
#include "aui_ui.h"
#include "aui_surface.h"
#endif 
#include "aui_Factory.h"

#include "aui_blitter.h"
#include "aui_memmap.h"
#include "aui_pixel.h"

#include "aui_image.h"



aui_Image::aui_Image(
	AUI_ERRCODE *retval,
	const MBCHAR *filename )
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



AUI_ERRCODE aui_Image::InitCommon( const MBCHAR *filename )
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



AUI_ERRCODE aui_Image::SetFilename( const MBCHAR *filename )
{
	
	Unload();	// deletes and NULLs m_format and m_surface

	memset( m_filename, '\0', sizeof( m_filename ) );

	if ( !filename ) return AUI_ERRCODE_INVALIDPARAM;

	strncpy( m_filename, filename, MAX_PATH );

	if (g_ui && g_ui->TheMemMap())
	{
		m_format = static_cast<aui_ImageFormat *>
						(g_ui->TheMemMap()->GetFileFormat(m_filename));
	}
	assert( m_format != NULL );
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
#ifdef _DEBUG
	fprintf(stderr, "%d==aui_Image::Load(%s)", retval, m_filename);
#endif

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

void
aui_Image::AttachSurface(aui_Surface *s)
{
	if (m_surface) {
		delete m_surface;
	}
	
	m_surface = s;
}


AUI_ERRCODE aui_Image::LoadEmpty( sint32 width, sint32 height, sint32 bpp )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

	m_surface = aui_Factory::new_Surface(
		retcode,
		width,
		height,
		bpp);

	Assert( AUI_NEWOK(m_surface,retcode) );

	return retcode;
}



AUI_ERRCODE aui_Image::LoadFileMapped( sint32 width, sint32 height,
                                       sint32 bpp, sint32 pitch, 
                                       uint8 *buffer )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

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







AUI_ERRCODE aui_BmpImageFormat::Load( const MBCHAR *filename, aui_Image *image )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	Assert( filename != NULL );
	Assert( image != NULL );
        

#ifdef WIN32	
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

			delete[] temp;

			errcode = surface->Unlock( bits );
			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
#elif defined(__AUI_USE_SDL__)
        printf("%s L%d: image %s!\n", __FILE__, __LINE__, filename); //is this ever called?
	assert(0);
	SDL_Surface *bmp = SDL_LoadBMP(filename);
	SDL_Surface *surf = NULL;
	SDL_PixelFormat fmt = { 0 };
//#if 0
//	if (aui_image_SDLPixelFormat(image, &fmt)) {
//		surf = SDL_ConvertSurface(bmp, &fmt, 0);
//	}
//#endif
        
        printf("%s L%d: image %s!\n", __FILE__, __LINE__, filename);
        if (g_ui->Primary()->BitsPerPixel() != 16)
            printf("%s L%d: bpp %d", __FILE__, __LINE__,  g_ui->Primary()->BitsPerPixel());
        if (bmp->format->Gmask >> bmp->format->Gshift == 0x3F)
            printf("%s L%d: 565 image!\n", __FILE__, __LINE__);
        if (bmp->format->Gmask >> bmp->format->Gshift == 0x1F)
            printf("%s L%d: 555 image!\n", __FILE__, __LINE__);
	if (NULL == surf) {
		surf = SDL_DisplayFormat(bmp);
	}
	SDL_FreeSurface(bmp);
	if (NULL == surf)
		return AUI_ERRCODE_LOADFAILED;
	//surface = image->TheSurface();
	//image->AttachSurface(surf);
        //The new surface should be assinged to the image-surfec but how???
        //conversion needed from SDL_Surface to aui_Surface
        //also using SDL_BlitSurface might be good in case surf 565 and img 555
        /* //this part doesn't create a visible effect:
        AUI_ERRCODE *retval;
        
        if (m_surface) {
            delete m_surface;
            }
        
        m_surface = new aui_Surface(retval, s->w, s->h, s->format->BitsPerPixel, s->pitch, (uint8 *) s->pixels); 
        Assert( AUI_SUCCESS(*retval) );
        if ( !AUI_SUCCESS(*retval) ) return;
        */
 
        if (image->TheSurface()) {
	} else {		
	}
	return retcode;
#else
	return AUI_ERRCODE_LOADFAILED;
#endif
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

