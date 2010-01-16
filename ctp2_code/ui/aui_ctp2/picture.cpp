












#include "c3.h"

#include "aui.h"
#include "aui_factory.h"
#include "c3blitter.h"
#include "c3ui.h"

#include "pixelutils.h"

#include "picture.h"

extern C3UI		*g_c3ui;

Picture::Picture(
	AUI_ERRCODE *retval,
	MBCHAR const * szFileName )
: aui_Image( retval, szFileName ), m_mipmap( NULL )
{
	Load();
	MakeMipmap();
}

AUI_ERRCODE Picture::MakeMipmap( void )
{
	Assert(m_surface);
	if (m_surface == NULL) return AUI_ERRCODE_INVALIDPARAM;
	if (m_surface->BitsPerPixel() != 16) return AUI_ERRCODE_INVALIDPARAM;


	
	aui_Surface *pMipmap = NULL;


	aui_Surface *   pSrcSurf    = m_surface;
	uint16 *        pSrcBuffer  = NULL;
	sint32 errcode = pSrcSurf->Lock(NULL, (LPVOID *)&pSrcBuffer, 0);

	if ( errcode == AUI_ERRCODE_OK )
	{
		sint32 srcWidth = pSrcSurf->Width();
		sint32 srcHeight = pSrcSurf->Height();
		sint32 mipWidth = srcWidth >> 1;
		sint32 mipHeight = srcHeight >> 1;

		AUI_ERRCODE retcode;
		pMipmap = aui_Factory::new_Surface(retcode, mipWidth, mipHeight);
		Assert(pMipmap);
		if (pMipmap)
		{
			uint16 *  pDestBuffer = NULL;
			errcode = pMipmap->Lock(NULL, (LPVOID *)&pDestBuffer, 0);

			if (errcode == AUI_ERRCODE_OK)
			{

				uint16 * pSrcPixel  = pSrcBuffer;
				uint16 * pDestPixel = pDestBuffer;

				for (sint32 i=0; i < mipHeight; i++)
				{
					pSrcPixel = pSrcBuffer + (i<<1) * srcWidth;
					for (sint32 j=0;j < mipWidth;j++)
					{
						*pDestPixel++ = AveragePixels(pSrcPixel, srcWidth);
						pSrcPixel += 2;
					}
				}

				
				errcode = pMipmap->Unlock((LPVOID)pDestBuffer);
			}
		}

		errcode = pSrcSurf->Unlock((LPVOID)pSrcBuffer);
	}

	m_mipmap = pMipmap;

	return AUI_ERRCODE_OK;
}



Pixel16 Picture::AveragePixels( uint16 *pBuffer, sint32 width )
{
	Pixel16 upperLeft,upperRight,lowerLeft,lowerRight;

	Assert(pBuffer);
	if (pBuffer==NULL) return 0;

	upperLeft = *pBuffer++;
	upperRight = *pBuffer;
	pBuffer += width - 2;
	lowerLeft = *pBuffer++;
	lowerRight = *pBuffer;

	sint16 b = ((upperLeft & 0x001f)+(upperRight & 0x001f)+
			(lowerLeft & 0x001f)+(lowerRight & 0x001f)) >> 2;

	if ( m_surface->PixelFormat() == AUI_SURFACE_PIXELFORMAT_555 )
	{
		sint16 g = (((upperLeft & 0x07e0) >> 5)+((upperRight & 0x07e0) >> 5)+
			((lowerLeft & 0x07e0) >> 5)+((lowerRight & 0x07e0) >> 5)) >> 2;

		sint16 r = (((upperLeft & 0xf800) >> 11)+((upperRight & 0xf800) >> 11)+
			((lowerLeft & 0xf800) >> 11)+((lowerRight & 0xf800) >> 11)) >> 2;

		return (r<<11) | (g<<5) | b;
	}
	else
	{
		sint16 g = (((upperLeft & 0x02e0) >> 5)+((upperRight & 0x02e0) >> 5)+
			((lowerLeft & 0x02e0) >> 5)+((lowerRight & 0x02e0) >> 5)) >> 2;

		sint16 r = (((upperLeft & 0x7c00) >> 10)+((upperRight & 0x7c00) >> 10)+
			((lowerLeft & 0x7c00) >> 10)+((lowerRight & 0x7c00) >> 10)) >> 2;

		return (r<<10) | (g<<5) | b;
	}
}








Picture::~Picture()
{
	delete m_mipmap;
}








AUI_ERRCODE Picture::Draw( aui_Surface *pDestSurf, RECT *pDestRect )
{
	Assert(m_surface->BitsPerPixel() == 16 );

	
	Assert(pDestRect);
	if (pDestRect==NULL) return AUI_ERRCODE_INVALIDPARAM;
	Assert(pDestRect->left<=pDestRect->right);
	if (pDestRect->left>pDestRect->right) return AUI_ERRCODE_INVALIDPARAM;
	Assert(pDestRect->top<=pDestRect->bottom);
	if (pDestRect->top>pDestRect->bottom) return AUI_ERRCODE_INVALIDPARAM;

	
	sint32 x = pDestRect->left;
	sint32 y = pDestRect->top;
	sint32 width = pDestRect->right - pDestRect->left;
	sint32 height = pDestRect->bottom - pDestRect->top;

	
	if (width == m_surface->Width() && height == m_surface->Height()) {
		RECT srcRect = {0, 0, m_surface->Width(), m_surface->Height()};
		g_c3ui->TheBlitter()->Blt(pDestSurf, pDestRect->left, pDestRect->top, m_surface, &srcRect, k_AUI_BLITTER_FLAG_COPY);
		return AUI_ERRCODE_OK;
	}

	
	if (width == m_mipmap->Width() && height == m_mipmap->Height()) {
		RECT srcRect = {0, 0, m_mipmap->Width(), m_mipmap->Height()};
		g_c3ui->TheBlitter()->Blt(pDestSurf, pDestRect->left, pDestRect->top, m_mipmap, &srcRect, k_AUI_BLITTER_FLAG_COPY);
		return AUI_ERRCODE_OK;
	}

	BYTE *pSrcBuffer;
	BYTE *pMipBuffer;
	BYTE *pDestBuffer;

	aui_Surface *pSrcSurf = m_surface;
	aui_Surface *pMipSurf = m_mipmap;

	Assert(pSrcSurf);
	if (pSrcSurf==NULL) return AUI_ERRCODE_INVALIDPARAM;
	Assert(pMipSurf);
	if (pMipSurf==NULL) return AUI_ERRCODE_INVALIDPARAM;
	Assert(pDestSurf);
	if (pDestSurf==NULL) return AUI_ERRCODE_INVALIDPARAM;

	sint32 errcode;

	
	errcode = pSrcSurf->Lock(NULL, (LPVOID *)&pSrcBuffer, 0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode == AUI_ERRCODE_OK)
	{
		errcode = pMipSurf->Lock(NULL, (LPVOID *)&pMipBuffer, 0);
		if (errcode == AUI_ERRCODE_OK)
		{
			errcode = pDestSurf->Lock(NULL, (LPVOID *)&pDestBuffer, 0);
			if (errcode == AUI_ERRCODE_OK)
			{
				sint32 srcWidth = pSrcSurf->Width();
				sint32 srcHeight = pSrcSurf->Height();
				sint32 mipWidth = pMipSurf->Width();
				sint32 mipHeight = pMipSurf->Height();
				sint32 destPitch = pDestSurf->Pitch();
			
				uint16 *pDestPixel;
				sint32 inc = (destPitch >> 1) - width;

				double left,top,right,bottom;
				double centerX,centerY;
				double distCenter;
				double xSquared,ySquared;
				Pixel16 srcPixel;
				double mipX,mipY;
				Pixel16 mipPixel;
				Pixel16 weightedPixel;
				sint32 cX,cY;

				pDestPixel = (uint16 *)(pDestBuffer + y * destPitch + (x << 1));

				sint32 ySrcIndex;
				sint32 yMipIndex;

				for (sint32 i=0;i < height;i++)
				{
					top = (double)i*srcHeight / height;
					bottom = ((double)i+1.0)*srcHeight / height;
					centerY = (top+bottom)/2;
					cY = (sint32)centerY;
					ySquared = centerY-((double)cY+.5);
					ySquared *= ySquared;
					mipY = (double)i*mipHeight / height;
					ySrcIndex = srcWidth*(sint32)centerY;
					yMipIndex = mipWidth*(sint32)mipY;

					for (sint32 j=0;j < width;j++)
					{
						
						left = (double)j * srcWidth / width;
						right = ((double)j+1.0)* srcWidth / width;

						
						centerX = (left+right)/2;
						cX = (sint32)centerX;
						srcPixel = ((uint16 *)pSrcBuffer)[ySrcIndex + (sint32)centerX];

						xSquared = centerX-((double)cX+.5);
						xSquared *= xSquared;

						distCenter = sqrt((xSquared+ySquared));

						
						mipX = (double)j*mipWidth / width;
						mipPixel = ((uint16 *)pMipBuffer)[yMipIndex + (sint32)mipX];

						
						sint16 srcR,srcG,srcB;
						double tempsrcR,tempsrcG,tempsrcB;
						sint16 mipR,mipG,mipB;
						double tempmipR,tempmipG,tempmipB;
						double a = distCenter,b = 1-distCenter;

						if ( pDestSurf->PixelFormat() == AUI_SURFACE_PIXELFORMAT_565 )
						{
							srcR = (srcPixel & 0xf800) >> 11;
							srcG = (srcPixel & 0x07e0) >> 5;
							srcB = (srcPixel & 0x001f);
							tempsrcR = a*(double)srcR;
							tempsrcG = a*(double)srcG;
							tempsrcB = a*(double)srcB;
							mipR = (mipPixel & 0xf800) >> 11;
							mipG = (mipPixel & 0x07e0) >> 5;
							mipB = (mipPixel & 0x001f);
							tempmipR = b*(double)mipR;
							tempmipG = b*(double)mipG;
							tempmipB = b*(double)mipB;
							sint16 R = (sint16)((tempsrcR+tempmipR));
							sint16 G = (sint16)((tempsrcG+tempmipG));
							sint16 B = (sint16)((tempsrcB+tempmipB));

							weightedPixel = (R<<11) | (G<<5) | B;
						}
						else
						{
							srcR = (srcPixel & 0x7c00) >> 10;
							srcG = (srcPixel & 0x02e0) >> 5;
							srcB = (srcPixel & 0x001f);
							tempsrcR = a*(double)srcR;
							tempsrcG = a*(double)srcG;
							tempsrcB = a*(double)srcB;
							mipR = (mipPixel & 0x7c00) >> 10;
							mipG = (mipPixel & 0x02e0) >> 5;
							mipB = (mipPixel & 0x001f);
							tempmipR = b*(double)mipR;
							tempmipG = b*(double)mipG;
							tempmipB = b*(double)mipB;
							sint16 R = (sint16)((tempsrcR+tempmipR));
							sint16 G = (sint16)((tempsrcG+tempmipG));
							sint16 B = (sint16)((tempsrcB+tempmipB));

							weightedPixel = (R<<10) | (G<<5) | B;
						}

						*pDestPixel++ = weightedPixel;
					}
					pDestPixel += inc;
				}

				
				errcode = pDestSurf->Unlock((LPVOID)pDestBuffer);
			}

			errcode = pMipSurf->Unlock((LPVOID)pMipBuffer);
		}

		errcode = pSrcSurf->Unlock((LPVOID)pSrcBuffer);
	}

	return AUI_ERRCODE_OK;
}
