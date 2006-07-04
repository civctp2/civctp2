












#include "c3.h"

#include "aui.h"
#include "aui_surface.h"
#include "aui_ldl.h"

#include "c3ui.h"
#include "CivPaths.h"

#include "pixelutils.h"
#include "icon.h"

extern C3UI		*g_c3ui;

Icon::Icon(	AUI_ERRCODE *retval,
			MBCHAR const * szFileName)
:
aui_Image(retval, szFileName)
{
}

Icon::~Icon()
{
}







AUI_ERRCODE Icon::Draw( aui_Surface *pDestSurf,	RECT *pDestRect, uint32 color )
{

	Assert(pDestRect);
	if (pDestRect==NULL) return AUI_ERRCODE_INVALIDPARAM;
	Assert(pDestRect->left<pDestRect->right);
	if (pDestRect->left>=pDestRect->right) return AUI_ERRCODE_INVALIDPARAM;
	Assert(pDestRect->top<pDestRect->bottom);
	if (pDestRect->top>=pDestRect->bottom) return AUI_ERRCODE_INVALIDPARAM;

	sint32 x = pDestRect->left;
	sint32 y = pDestRect->top;
	sint32 width = pDestRect->right - pDestRect->left;
	sint32 height = pDestRect->bottom - pDestRect->top;

	BYTE *pDestBuffer;
	BYTE *pSrcBuffer;

	aui_Surface *pSrcSurf = m_surface;

	Assert(pDestSurf);
	if (pDestSurf==NULL) return AUI_ERRCODE_INVALIDPARAM;
	Assert(pSrcSurf);
	if (pSrcSurf==NULL) return AUI_ERRCODE_INVALIDPARAM;

	sint32 errcode;

	
	errcode = pDestSurf->Lock(NULL, (LPVOID *)&pDestBuffer, 0);
	if (errcode == AUI_ERRCODE_OK)
	{
		errcode = pSrcSurf->Lock(NULL, (LPVOID *)&pSrcBuffer, 0);
		if (errcode == AUI_ERRCODE_OK)
		{
			sint32 surfPitch = pDestSurf->Pitch();
			sint32 srcWidth = pSrcSurf->Width();
			sint32 srcHeight = pSrcSurf->Height();
			sint32 srcPitch = pSrcSurf->Pitch();

			if (width > srcWidth) width = srcWidth;
			if (height > srcHeight) height = srcHeight;


			
			uint16 *pSrcPixel;
			uint16 *pDestPixel;



			Pixel16 blendPixel;
			sint16 blendValue;

			for (sint32 i=0;i < height;i++)
			{
				pSrcPixel = (uint16 *)(pSrcBuffer + i * srcPitch);
				pDestPixel = (uint16 *)(pDestBuffer + (y+i) * surfPitch + (x << 1));
				for (sint32 j=0;j < width;j++)
				{
					if (*pSrcPixel)
					{

						blendValue = *pSrcPixel & 0x001f;
						blendPixel = pixelutils_BlendFast((Pixel16)color,*pDestPixel,blendValue);
						*pDestPixel = blendPixel;
					}

					pDestPixel++;
					pSrcPixel++;
				}

			}

			
			errcode = pSrcSurf->Unlock((LPVOID)pSrcBuffer);
		}

		errcode = pDestSurf->Unlock((LPVOID)pDestBuffer);
	}

	return AUI_ERRCODE_OK;
}
