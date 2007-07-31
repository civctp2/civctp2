










#include "c3.h"
#include "aui_directsurface.h"

#include "aui_directblitter.h"



AUI_ERRCODE aui_DirectBlitter::Blt16To16(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

	
	
	
	if ( !destSurf->Buffer()
	&&   !srcSurf->Buffer()
	&&   destSurf->IsThisA( aui_DirectSurface::m_directSurfaceClassId )
	&&   srcSurf->IsThisA( aui_DirectSurface::m_directSurfaceClassId )
	&&   (!flags || (flags &
			(k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_CHROMAKEY))) )
	{
		HRESULT hr;

		if ( flags & k_AUI_BLITTER_FLAG_CHROMAKEY )
		{

			do {

				Assert(((aui_DirectSurface *)srcSurf)->DDS()->IsLost() == DD_OK);
				Assert(((aui_DirectSurface *)destSurf)->DDS()->IsLost() == DD_OK);

				hr = ((aui_DirectSurface *)destSurf)->DDS()->Blt(destRect,
						((aui_DirectSurface *)srcSurf)->DDS(),
						srcRect,
						DDBLT_KEYSRC,
						NULL);











				if ( hr != DDERR_SURFACEBUSY && hr != DDERR_WASSTILLDRAWING )
					break;






			} while ( 1 );
		}
		else
		{

			do {









				Assert(((aui_DirectSurface *)srcSurf)->DDS()->IsLost() == DD_OK);
				Assert(((aui_DirectSurface *)destSurf)->DDS()->IsLost() == DD_OK);

				hr = ((aui_DirectSurface *)destSurf)->DDS()->Blt(destRect,
						((aui_DirectSurface *)srcSurf)->DDS(),
						srcRect,
						0,
						NULL);

				if ( hr != DDERR_SURFACEBUSY && hr != DDERR_WASSTILLDRAWING )
					break;






			} while ( 1 );
		}

		
		
		
		
		if(hr != DD_OK) {
			DPRINTF(k_DBG_UI, ("%s:%d: hr = %d\n", __FILE__, __LINE__, hr));
		}
		if ( hr != DD_OK ) retcode = AUI_ERRCODE_BLTFAILED;
	}
	else
		return aui_Blitter::Blt16To16(
			destSurf,
			destRect,
			srcSurf,
			srcRect,
			flags );

	return retcode;
}



AUI_ERRCODE aui_DirectBlitter::ColorBlt16(
	aui_Surface *destSurf,
	RECT *destRect,
	uint32 color,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

	
	
	if ( !destSurf->Buffer()
	&&   destSurf->IsThisA( aui_DirectSurface::m_directSurfaceClassId ) )
	{
		m_ddbfx.dwFillColor = color;
		HRESULT hr;


		do {
			hr = ((aui_DirectSurface *)destSurf)->DDS()->Blt(
				destRect,
				NULL,
				NULL,
				DDBLT_COLORFILL | 0,
				&m_ddbfx );

			if ( hr != DDERR_SURFACEBUSY && hr != DDERR_WASSTILLDRAWING )
				break;






		} while ( 1 );

		Assert( hr == DD_OK );
		if ( hr != DD_OK ) retcode = AUI_ERRCODE_BLTFAILED;
	}
	else
		return aui_Blitter::ColorBlt16(
			destSurf,
			destRect,
			color,
			flags );

	return retcode;
}

AUI_ERRCODE aui_DirectBlitter::ColorStencilBlt16(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *stencilSurf,
	RECT *stencilRect,
	uint32 color,
	uint32 flags )
{
	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_DirectBlitter::StretchBlt16To16(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

	
	
	
	if ( !destSurf->Buffer()
	&&   !srcSurf->Buffer()
	&&   destSurf->IsThisA( aui_DirectSurface::m_directSurfaceClassId )
	&&   srcSurf->IsThisA( aui_DirectSurface::m_directSurfaceClassId )
	&&   (!flags || (flags &
			(k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_CHROMAKEY))) )
	{
		HRESULT hr;

		if ( flags & k_AUI_BLITTER_FLAG_CHROMAKEY )
		{

			do {
				hr = ((aui_DirectSurface *)destSurf)->DDS()->Blt(
					destRect,
					((aui_DirectSurface *)srcSurf)->DDS(),
					srcRect,
					DDBLT_KEYSRC | 0,
					&m_ddbfx );

				if ( hr != DDERR_SURFACEBUSY && hr != DDERR_WASSTILLDRAWING )
					break;






			} while ( 1 );
		}
		else
		{

			do {
				hr = ((aui_DirectSurface *)destSurf)->DDS()->Blt(
					destRect,
					((aui_DirectSurface *)srcSurf)->DDS(),
					srcRect,
					0,
					&m_ddbfx );

				if ( hr != DDERR_SURFACEBUSY && hr != DDERR_WASSTILLDRAWING )
					break;






			} while ( 1 );
		}

		Assert( hr == DD_OK );
		if ( hr != DD_OK ) retcode = AUI_ERRCODE_BLTFAILED;
	}
	else
		return aui_Blitter::StretchBlt16To16(
			destSurf,
			destRect,
			srcSurf,
			srcRect,
			flags );

	return retcode;
}






































































