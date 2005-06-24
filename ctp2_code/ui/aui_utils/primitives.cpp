#include "c3.h"
#include "c3math.h"

#include "Globals.h"

#include "aui.h"
#include "aui_surface.h"
#include "aui_directsurface.h"
#include "aui_image.h"
#include "pattern.h"
#include "aui_blitter.h"
#include "aui_tabgroup.h"

#include "c3ui.h"

#include "pixelutils.h"
#include "primitives.h"
#include "textutils.h"
#include "tileset.h"
#include "colorset.h"

extern sint32		g_is565Format;
extern ColorSet		*g_colorSet;
extern C3UI			*g_c3ui;







PRIMITIVES_ERRCODE primitives_SetRect(
	RECT *rect,		
	sint32 left,	
	sint32 top,		
	sint32 right,	
	sint32 bottom	
	)
{
	Assert(rect);
	if (rect==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	rect->left = left;
	rect->top = top;
	rect->right = right;
	rect->bottom = bottom;

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_FrameRect16(
	aui_Surface *pSurface,	
	RECT *pRect,			
	Pixel16 color			
	)
{
	BOOL			wasUnlocked = FALSE;
	AUI_ERRCODE		errcode;

	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect);
	if (pRect == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 left = pRect->left, top = pRect->top, right = pRect->right, bottom = pRect->bottom;

	Assert(left <= right);
	Assert(top <= bottom);
	if ((left >= right) || (top >= bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	uint8 *pSurfBase;

	if (pSurface->Buffer() == NULL) {
		errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;
		wasUnlocked = TRUE;
	} else {
		pSurfBase = (uint8 *)pSurface->Buffer();
	}

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	uint16 *pDest;
	sint32 width, height;
	sint32 inc1,inc2;

	
	width = right - left;
	height = bottom - top;

	pDest = (uint16 *)(pSurfBase + top * surfPitch + (left << 1));
	inc1 = (surfPitch >> 1) - width;
	inc2 = width - 1;

	sint32 i;
	for (i=width;i;i--)
		*pDest++ = color;

	sint32 tempHeight = height - 2;
	for (sint32 j=tempHeight;j;j--)
	{
		pDest += inc1;
		*pDest = color;
		pDest += inc2;
		*pDest++ = color;
	}

	pDest += inc1;

	for (i=width;i;i--)
		*pDest++ = color;

	
	if (wasUnlocked) {
		errcode = pSurface->Unlock((LPVOID)pSurfBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return PRIMITIVES_ERRCODE_OK;
}





#define RED555(c) ((c)&0x1f)
#define GREEN555(c) (((c)>>5)&0x1f)
#define BLUE555(c) (((c)>>10)&0x1f)
#define RED565(c) ((c)&0x1f)
#define GREEN565(c) (((c)>>5)&0x3f)
#define BLUE565(c) (((c)>>11)&0x1f)

PRIMITIVES_ERRCODE primitives_Scale16(
	aui_Surface *pSrc,
	aui_Surface *pDst,
	const fRect &sRect,
	const fRect &dRect,
	bool bFilter
	)
{
	BOOL			srcUnlocked = FALSE, dstUnlocked = FALSE;
	AUI_ERRCODE		errcode;

	if ((pSrc == NULL) || (pDst == NULL))
	{
		Assert(0);
		return PRIMITIVES_ERRCODE_INVALIDPARAM;
	}

	Assert(dRect.left <= dRect.right);
	Assert(dRect.top <= dRect.bottom);
	if ((dRect.left >= dRect.right) || (dRect.top >= dRect.bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 width = (sint32) (ceil(dRect.right) - ceil(dRect.left));
	sint32 height = (sint32) (ceil(dRect.bottom) - ceil(dRect.top));
	if ((width == 0) || (height == 0))
		return PRIMITIVES_ERRCODE_OK;

	uint16 *pSrcBase;

	if (pSrc->Buffer() == NULL) { 
		errcode = pSrc->Lock(NULL,(LPVOID *)&pSrcBase,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;
		srcUnlocked = TRUE;
	} else {
		pSrcBase = (uint16 *)pSrc->Buffer();
	}

	uint16 *pDstBase;

	if (pDst->Buffer() == NULL) { 
		errcode = pDst->Lock(NULL,(LPVOID *)&pDstBase,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;
		dstUnlocked = TRUE;
	} else {
		pDstBase = (uint16 *)pDst->Buffer();
	}

	
	sint32 srow = pSrc->Pitch()>>1;
	sint32 drow = pDst->Pitch()>>1;

	

	sint32 dst_y0 = (sint32) ceil(dRect.top);
	sint32 dst_x0 = (sint32) ceil(dRect.left);
	uint16 *pDstPixel = pDstBase + dst_y0 * drow + dst_x0;

	double src_dy = (sRect.bottom - sRect.top)/(dRect.bottom - dRect.top);
	double src_dx = (sRect.right - sRect.left)/(dRect.right - dRect.left);
	double src_y = sRect.top + (dst_y0 - dRect.top) * src_dy;
	if (bFilter) src_y -= 0.5;
	for (sint32 j=0;j<height;j++)
	{
		double src_x = sRect.left + (dst_x0 - dRect.left) * src_dx;
		if (bFilter) src_x -= 0.5;
		uint16 *pSrcPixel = &pSrcBase[srow * ((uint32 )floor(src_y))];
		double fy = src_y - floor(src_y);
		for (sint32 i=0;i<width;i++)
		{
			uint32 x = floor(src_x);
			if (!bFilter)
			{
				pDstPixel[i] = pSrcPixel[x];
			} 
			else 
			{
				uint16 c0 = pSrcPixel[x], c1 = pSrcPixel[x+1];
				uint16 c2 = pSrcPixel[x + srow], c3 = pSrcPixel[x + srow + 1];
				double f0, f1, f2, f3;
				double fx = src_x - floor(src_x);
				f1 = 1.0 - fy;
				f2 = 1.0 - fx;
				f3 = fx * fy;
				f0 = f1 * f2;
				f1 *= fx;
				f2 *= fy;
				if (g_is565Format)
				{
					double red = f0 * RED565(c0) + f1 * RED565(c1) + f2 * RED565(c2) + f3 * RED565(c3);
					double green = f0 * GREEN565(c0) + f1 * GREEN565(c1) + f2 * GREEN565(c2) + f3 * GREEN565(c3);
					double blue = f0 * BLUE565(c0) + f1 * BLUE565(c1) + f2 * BLUE565(c2) + f3 * BLUE565(c3);
					pDstPixel[i] = ((sint32 )red) + (((sint32 )green)<<5) + (((sint32 )blue)<<11);
				} 
				else 
				{
					double red = f0 * RED555(c0) + f1 * RED555(c1) + f2 * RED555(c2) + f3 * RED555(c3);
					double green = f0 * GREEN555(c0) + f1 * GREEN555(c1) + f2 * GREEN555(c2) + f3 * GREEN555(c3);
					double blue = f0 * BLUE555(c0) + f1 * BLUE555(c1) + f2 * BLUE555(c2) + f3 * BLUE555(c3);
					pDstPixel[i] = ((sint32 )red) + (((sint32 )green)<<5) + (((sint32 )blue)<<10);
				}
			}
			src_x += src_dx;
		}
		src_y += src_dy;
		pDstPixel += drow;
	}

	if (srcUnlocked) {
		
		errcode = pSrc->Unlock((LPVOID)pSrcBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;
	}

	if (dstUnlocked) {
		
		errcode = pDst->Unlock((LPVOID)pDstBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return PRIMITIVES_ERRCODE_OK;
}








PRIMITIVES_ERRCODE primitives_PaintRect16(
	aui_Surface *pSurface,	
	RECT *pRect,			
	Pixel16 color			
	)
{
	BOOL			wasUnlocked = FALSE;
	AUI_ERRCODE		errcode;

	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect);
	if (pRect == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 left = pRect->left, top = pRect->top, right = pRect->right, bottom = pRect->bottom;

	Assert(left <= right);
	Assert(top <= bottom);
	if ((left >= right) || (top >= bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	uint8 *pSurfBase;

	if (pSurface->Buffer() == NULL) { 
		errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;
		wasUnlocked = TRUE;
	} else {
		pSurfBase = pSurface->Buffer();
	}

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	uint16 *pDestPixel;

	
	sint32 width = right - left;
	sint32 height = bottom - top;

	pDestPixel = (uint16 *)(pSurfBase + top * surfPitch + (left << 1));
	sint32 inc = (surfPitch >> 1) - width;

	for (sint32 j=height;j;j--)
	{
		for (sint32 i=width;i;i--)
			*pDestPixel++ = color;
		pDestPixel += inc;
	}

	if (wasUnlocked) {
		
		errcode = pSurface->Unlock((LPVOID)pSurfBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_OldBevelRect16(
	aui_Surface *pSurface,	
	RECT *pRect,			
	sint32 level,			
	BOOL flag				
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect);
	if (pRect == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 left = pRect->left, top = pRect->top, right = pRect->right, bottom = pRect->bottom;

	Assert(left <= right);
	Assert(top <= bottom);
	if ((left >= right) || (top >= bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	uint8 *pSurfBase;

	bool wasUnlocked = false;

	sint32 errcode;
	if(pSurface->Buffer() == NULL) {
		errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;
		wasUnlocked = true;
	} else {
		pSurfBase = (uint8 *)pSurface->Buffer();
	}

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	
	sint32 width = right - left;
	sint32 height = bottom - top;

	
	if (width > height) 
	{
		if (level > (height >> 1))
			level = height >> 1;
	}
	else 
	{
		if (level > (width >> 1))
			level = width >> 1;
	}

	pDestPixel = (uint16 *)(pSurfBase + top * surfPitch + (left << 1));
	sint32 inc = (surfPitch >> 1) - width;

	sint32 tempWidth = width-1;
	sint32 tempHeight = height-1;
	sint32 tempInc = inc;

	if (flag)
	{
		sint32 j;
		for (j = level;j;j--)
		{
			for (sint32 i=tempWidth;i;i--)
			{
				
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = inc;
		tempWidth = width-1;
		pDestPixel = (uint16 *)(pSurfBase + (bottom-1) * surfPitch + ((right-1) << 1));

		
		for (j = level;j;j--)
		{
			for (sint32 i=tempWidth;i;i--)
			{
				
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel-- = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel -= tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		
		sint32 temp = top+1;
		sint32 i;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (temp+i) * surfPitch + ((left+i) << 1));

			for (sint32 y=tempHeight;y;y--)
			{
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}

		tempHeight = height-1;

		
		temp = right - 1;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i) * surfPitch + ((temp-i) << 1));

			for (sint32 y=tempHeight;y;y--)
			{
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}	
	}
	else
	{
		sint32 j;
		for (j = level;j;j--)
		{
			for (sint32 i=tempWidth;i;i--)
			{
				
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = inc;
		tempWidth = width-1;
		pDestPixel = (uint16 *)(pSurfBase + (bottom-1) * surfPitch + ((right-1) << 1));

		
		for (j = level;j;j--)
		{
			for (sint32 i=tempWidth;i;i--)
			{
				
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel-- = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel -= tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		
		sint32 temp = top+1;
		sint32 i;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (temp+i) * surfPitch + ((left+i) << 1));

			for (sint32 y=tempHeight;y;y--)
			{
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}

		tempHeight = height-1;

		
		temp = right - 1;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i) * surfPitch + ((temp-i) << 1));

			for (sint32 y=tempHeight;y;y--)
			{
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}	
	}

	
	if(wasUnlocked) {
		errcode = pSurface->Unlock((LPVOID)pSurfBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_BevelRect16(
	aui_Surface *pSurface,	
	RECT *pRect,			
	sint32 level,			
	BOOL flag,				
	sint32 blendLight,		
	sint32 blendDark		
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect);
	if (pRect == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 left = pRect->left, top = pRect->top, right = pRect->right, bottom = pRect->bottom;

	Assert(left <= right);
	Assert(top <= bottom);
	if ((left >= right) || (top >= bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;


	if ( left < 0 || top < 0 || right > pSurface->Width() || bottom > pSurface->Height() ) 
		return PRIMITIVES_ERRCODE_INVALIDPARAM;

	uint8 *pSurfBase;

	bool wasUnlocked = false;

	sint32 errcode;

	if(pSurface->Buffer() == NULL) {
		errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;
		wasUnlocked = true;
	} else {
		pSurfBase = (uint8 *)pSurface->Buffer();
	}
		

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	
	sint32 width = right - left;
	sint32 height = bottom - top;

	
	if (width > height) 
	{
		if (level > (height >> 1))
			level = height >> 1;
	}
	else 
	{
		if (level > (width >> 1))
			level = width >> 1;
	}

	pDestPixel = (uint16 *)(pSurfBase + top * surfPitch + (left << 1));
	sint32 inc = (surfPitch >> 1) - width;

	sint32 tempWidth = width-1;
	sint32 tempHeight = height-1;
	sint32 tempInc = inc;

	if (flag)
	{
		sint32 j;
		for (j = level;j;j--)
		{
			for (sint32 i=tempWidth;i;i--)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = inc;
		tempWidth = width-1;
		pDestPixel = (uint16 *)(pSurfBase + (bottom-1) * surfPitch + ((right-1) << 1));

		
		for (j = level;j;j--)
		{
			for (sint32 i=tempWidth;i;i--)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel-- = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel -= tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		
		sint32 temp = top+1;
		sint32 i;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (temp+i) * surfPitch + ((left+i) << 1));

			for (sint32 y=tempHeight;y;y--)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}

		tempHeight = height-1;

		
		temp = right - 1;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i) * surfPitch + ((temp-i) << 1));

			for (sint32 y=tempHeight;y;y--)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}	
	}
	else
	{
		sint32 j;
		for (j = level;j;j--)
		{
			for (sint32 i=tempWidth;i;i--)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = inc;
		tempWidth = width-1;
		pDestPixel = (uint16 *)(pSurfBase + (bottom-1) * surfPitch + ((right-1) << 1));

		
		for (j = level;j;j--)
		{
			for (sint32 i=tempWidth;i;i--)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel-- = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel -= tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		
		sint32 temp = top+1;
		sint32 i;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (temp+i) * surfPitch + ((left+i) << 1));

			for (sint32 y=tempHeight;y;y--)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}

		tempHeight = height-1;

		
		temp = right - 1;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i) * surfPitch + ((temp-i) << 1));

			for (sint32 y=tempHeight;y;y--)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}	
	}

	if(wasUnlocked) {
		
		errcode = pSurface->Unlock((LPVOID)pSurfBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_FrameThickRect16(
	aui_Surface *pSurface,	
	RECT *pRect,			
	Pixel16 color,
	sint32 level			
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect);
	if (pRect == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 left = pRect->left, top = pRect->top, right = pRect->right, bottom = pRect->bottom;

	Assert(left <= right);
	Assert(top <= bottom);
	if ((left >= right) || (top >= bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	uint8 *pSurfBase;

	sint32 errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	

	uint16 *pDestPixel;

	
	sint32 width = right - left;
	sint32 height = bottom - top;

	
	if (width > height) 
	{
		if (level > (height >> 1))
			level = height >> 1;
	}
	else 
	{
		if (level > (width >> 1))
			level = width >> 1;
	}

	pDestPixel = (uint16 *)(pSurfBase + top * surfPitch + (left << 1));
	sint32 inc = (surfPitch >> 1) - width;

	sint32 tempWidth = width-1;
	sint32 tempHeight = height-1;
	sint32 tempInc = inc;

	{
		sint32 j;
		for (j = level;j;j--)
		{
			for (sint32 i=tempWidth;i;i--)
			{
				


				*pDestPixel++ = color;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = inc;
		tempWidth = width-1;
		pDestPixel = (uint16 *)(pSurfBase + (bottom-1) * surfPitch + ((right-1) << 1));

		
		for (j = level;j;j--)
		{
			for (sint32 i=tempWidth;i;i--)
			{
				


				*pDestPixel-- = color;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel -= tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		
		sint32 temp = top+1;
		sint32 i;
		for (sint32 i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (temp+i) * surfPitch + ((left+i) << 1));

			for (sint32 y=tempHeight;y;y--)
			{


				*pDestPixel++ = color;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}

		tempHeight = height-1;

		
		temp = right - 1;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i) * surfPitch + ((temp-i) << 1));

			for (sint32 y=tempHeight;y;y--)
			{


				*pDestPixel++ = color;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}	
	}

	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_DrawLine16(
	aui_Surface *pSurface,	
	sint32 x1,				
	sint32 y1,				
	sint32 x2,				
	sint32 y2,				
	Pixel16 color			
	)
{
	Assert(pSurface);
	if(pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 dx,dy,sdx,sdy,absdx,absdy,num,den;

	dx = x2-x1;
	dy = y2-y1;
	sdx = SGN(dx);
	sdy = SGN(dy);
	absdx = ABS(dx);
	absdy = ABS(dy);
	num = absdy >> 1;
	den = absdx >> 1;

	uint8 *pSurfBase;

	sint32 errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	uint16 *pDest;

	pDest = (uint16 *)(pSurfBase + y1 * surfPitch + (x1 << 1));
	*pDest = color;

	if (absdx >= absdy)	
	{
		for (sint32 i=absdx;i;i--)
		{
			num += absdy;
			if (num > absdx)
			{
				num -= absdx;
				y1 += sdy;
			}
			x1 += sdx;

			pDest = (uint16 *)(pSurfBase + y1 * surfPitch + (x1 << 1));
			*pDest = color;
		}
	}
	else			
	{
		for (sint32 i=absdy;i;i--)
		{
			den += absdx;
			if (den > absdy)
			{
				den -= absdy;
				x1 += sdx;
			}
			y1 += sdy;

			pDest = (uint16 *)(pSurfBase + y1 * surfPitch + (x1 << 1));
			*pDest = color;
		}
	}

	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;

	return PRIMITIVES_ERRCODE_OK;
}

#define k_MAX_BLEND_VALUES	8
#define k_BLEND_COLOR		0x0000
#define k_BLEND_VALUE		10





























































































































































































PRIMITIVES_ERRCODE primitives_DrawText(
	aui_Surface *pSurface,	
	sint32 x,							
	sint32 y,							
	MBCHAR *pString,					
	COLORREF color,						
	BOOL bg								
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pString);
	if (pString == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

#ifdef __AUI_USE_DIRECTX__
	HDC hdc;
	HRESULT hr;
	COLORREF oldColor;
	sint32 oldMode;

	hr = pSurface->GetDC(&hdc);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_DSGETDCFAILED;

	
	if (bg)
		oldMode = SetBkMode(hdc,TRANSPARENT);
	
	
	oldColor = SetTextColor(hdc, color);

	
	HFONT hOldFont;
	if (g_hFont)
		hOldFont = (HFONT)SelectObject(hdc,g_hFont);

	TextOut(hdc,x,y,pString,strlen(pString));

	
	if (g_hFont)
		SelectObject(hdc,hOldFont);

	
	SetTextColor(hdc, oldColor);

	
	if (bg)
		SetBkMode(hdc,oldMode);

	hr = pSurface->ReleaseDC(hdc);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_DSRELEASEDCFAILED;
#endif

	return PRIMITIVES_ERRCODE_OK;
}




PRIMITIVES_ERRCODE primitives_DrawBoundedText(
		aui_Surface *pSurface,	
		RECT *bound,						
		MBCHAR *pString,				
		COLORREF color,					
		BOOL bg							
		)
{
#ifdef __AUI_USE_DIRECTX__
	HDC hdc;
	HRESULT hr;
	COLORREF oldColor;
	sint32 oldMode;
#endif

	Assert(pSurface);
	if (pSurface==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;
	Assert(pString);
	if (pString==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

#ifdef __AUI_USE_DIRECTX__
	hr = pSurface->GetDC(&hdc);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	
	if (bg)
		oldMode = SetBkMode(hdc,TRANSPARENT);
	
	
	oldColor = SetTextColor(hdc, g_colorSet->GetColorRef(COLOR_BUTTON_TEXT_DROP));

	
	HFONT hOldFont;
	if (g_hFont)
		hOldFont = (HFONT)SelectObject(hdc,g_hFont);

	DrawText(hdc,pString,-1,bound,DT_WORDBREAK);

	oldColor = SetTextColor(hdc, color);

	RECT bound2 = *bound;
	OffsetRect(&bound2, -1, -1);
	DrawText(hdc,pString,-1,&bound2,DT_WORDBREAK);

	
	if (g_hFont)
		SelectObject(hdc,hOldFont);

	
	SetTextColor(hdc, oldColor);

	
	if (bg)
		SetBkMode(hdc,oldMode);

	hr = pSurface->ReleaseDC(hdc);
	Assert(hr == AUI_ERRCODE_OK);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_DSRELEASEDCFAILED;
#endif
	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_DrawTextBatch(
	aui_Surface *pSurface,	
	sint32 x,							
	sint32 y,							
	MBCHAR **pString,					
	sint32 numStrings,					
	COLORREF color,						
	BOOL bg								
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pString);
	if (pString == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

#ifdef __AUI_USE_DIRECTX__
	HDC hdc;
	HRESULT hr;
	COLORREF oldColor;
	TEXTMETRIC tm;
	sint32 oldMode;

	hr = pSurface->GetDC(&hdc);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_DSGETDCFAILED;

	
	if (bg)
		oldMode = SetBkMode(hdc,TRANSPARENT);
	
	GetTextMetrics(hdc,&tm);

	
	oldColor = SetTextColor(hdc, color);

	
	HFONT hOldFont;
	if (g_hFont)
		hOldFont = (HFONT)SelectObject(hdc,g_hFont);

	for (sint32 i=0;i < numStrings;i++)
	{
		TextOut(hdc,x,y,pString[i],strlen(pString[i]));
		y += tm.tmHeight + tm.tmExternalLeading;
	}

	
	if (g_hFont)
		SelectObject(hdc,hOldFont);

	
	SetTextColor(hdc, oldColor);

	
	if (bg)
		SetBkMode(hdc,oldMode);

	hr = pSurface->ReleaseDC(hdc);
	Assert(hr == AUI_ERRCODE_OK);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_DSRELEASEDCFAILED;
#endif
	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_DropText(
	aui_Surface *pSurface,	
	sint32 x,							
	sint32 y,							
	MBCHAR *pString,					
	COLORREF color,						
	BOOL bg								
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pString);
	if (pString==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

#ifdef __AUI_USE_DIRECTX__
	HDC hdc;
	HRESULT hr;
	COLORREF oldColor;
	sint32 oldMode;

	hr = pSurface->GetDC(&hdc);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_DSGETDCFAILED;

	
	if (bg)
		oldMode = SetBkMode(hdc,TRANSPARENT);
	
	
	oldColor = SetTextColor(hdc, 0);

	
	HFONT hOldFont;
	if (g_hFont)
		hOldFont = (HFONT)SelectObject(hdc,g_hFont);

	
	COLORREF	dropTextColor = g_colorSet->GetColorRef(COLOR_BUTTON_TEXT_DROP);

	SetTextColor(hdc, dropTextColor);
	TextOut(hdc,x+1,y+1,pString,strlen(pString));

	SetTextColor(hdc, color);
	TextOut(hdc,x,y,pString,strlen(pString));

	
	if (g_hFont)
		SelectObject(hdc,hOldFont);

	
	SetTextColor(hdc, oldColor);

	
	if (bg)
		SetBkMode(hdc,oldMode);

	hr = pSurface->ReleaseDC(hdc);
	Assert(hr == AUI_ERRCODE_OK);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;
#endif
	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_ColoredDropText(
	aui_Surface *pSurface,	
	sint32 x,							
	sint32 y,							
	MBCHAR *pString,					
	COLORREF textColor,						
	COLORREF dropColor,						
	BOOL bg								
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pString);
	if (pString==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

#ifdef __AUI_USE_DIRECTX__
	HDC hdc;
	HRESULT hr;
	COLORREF oldColor;
	sint32 oldMode;

	hr = pSurface->GetDC(&hdc);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_DSGETDCFAILED;

	
	if (bg)
		oldMode = SetBkMode(hdc,TRANSPARENT);
	
	
	oldColor = SetTextColor(hdc, 0);

	
	HFONT hOldFont;
	if (g_hFont)
		hOldFont = (HFONT)SelectObject(hdc,g_hFont);

	



	SetTextColor(hdc, dropColor);
	TextOut(hdc,x+1,y+1,pString,strlen(pString));

	SetTextColor(hdc, textColor);
	TextOut(hdc,x,y,pString,strlen(pString));

	
	if (g_hFont)
		SelectObject(hdc,hOldFont);

	
	SetTextColor(hdc, oldColor);

	
	if (bg)
		SetBkMode(hdc,oldMode);

	hr = pSurface->ReleaseDC(hdc);
	Assert(hr == AUI_ERRCODE_OK);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;
#endif
	return PRIMITIVES_ERRCODE_OK;
}

PRIMITIVES_ERRCODE primitives_DropTextCentered(
	aui_Surface *pSurface,	
	RECT *destRect,
	MBCHAR *pString,					
	COLORREF color,						
	BOOL bg								
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pString);
	if (pString==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

#ifdef __AUI_USE_DIRECTX__
	HDC			hdc;
	HRESULT		hr;
	COLORREF	oldColor;
	sint32		oldMode;
	SIZE		size;
	sint32		x,y;

	hr = pSurface->GetDC(&hdc);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_DSGETDCFAILED;

	
	if (bg)
		oldMode = SetBkMode(hdc,TRANSPARENT);
	
	
	oldColor = SetTextColor(hdc, 0);

	
	HFONT hOldFont;
	if (g_hFont)
		hOldFont = (HFONT)SelectObject(hdc,g_hFont);

	
	COLORREF	dropTextColor = g_colorSet->GetColorRef(COLOR_BUTTON_TEXT_DROP);

	GetTextExtentPoint32(hdc, pString, strlen(pString),  &size);

	x = destRect->left + (destRect->right-destRect->left)/2 - size.cx/2;
	y = destRect->top + (destRect->bottom-destRect->top)/2 - size.cy/2;

	SetTextColor(hdc, dropTextColor);
	TextOut(hdc,x+1,y+1,pString,strlen(pString));

	SetTextColor(hdc, color);
	TextOut(hdc,x,y,pString,strlen(pString));

	
	if (g_hFont)
		SelectObject(hdc,hOldFont);

	
	SetTextColor(hdc, oldColor);

	
	if (bg)
		SetBkMode(hdc,oldMode);

	hr = pSurface->ReleaseDC(hdc);
	Assert(hr == AUI_ERRCODE_OK);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;
#endif
	return PRIMITIVES_ERRCODE_OK;
}

PRIMITIVES_ERRCODE primitives_ColoredDropTextCentered(
	aui_Surface *pSurface,	
	RECT *destRect,
	MBCHAR *pString,					
	COLORREF textColor,						
	COLORREF dropColor,						
	BOOL bg								
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pString);
	if (pString==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

#ifdef __AUI_USE_DIRECTX__
	HDC			hdc;
	HRESULT		hr;
	COLORREF	oldColor;
	sint32		oldMode;
	SIZE		size;
	sint32		x,y;

	hr = pSurface->GetDC(&hdc);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_DSGETDCFAILED;

	
	if (bg)
		oldMode = SetBkMode(hdc,TRANSPARENT);
	
	
	oldColor = SetTextColor(hdc, 0);

	
	HFONT hOldFont;
	if (g_hFont)
		hOldFont = (HFONT)SelectObject(hdc,g_hFont);

	



	GetTextExtentPoint32(hdc, pString, strlen(pString),  &size);

	x = destRect->left + (destRect->right-destRect->left)/2 - size.cx/2;
	y = destRect->top + (destRect->bottom-destRect->top)/2 - size.cy/2;

	SetTextColor(hdc, dropColor);
	TextOut(hdc,x+1,y+1,pString,strlen(pString));

	SetTextColor(hdc, textColor);
	TextOut(hdc,x,y,pString,strlen(pString));

	
	if (g_hFont)
		SelectObject(hdc,hOldFont);

	
	SetTextColor(hdc, oldColor);

	
	if (bg)
		SetBkMode(hdc,oldMode);

	hr = pSurface->ReleaseDC(hdc);
	Assert(hr == AUI_ERRCODE_OK);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;
#endif

	return PRIMITIVES_ERRCODE_OK;
}








PRIMITIVES_ERRCODE primitives_DropTextBatch(
	aui_Surface *pSurface,	
	sint32 x,							
	sint32 y,							
	MBCHAR **pString,					
	sint32 numStrings,					
	COLORREF color,						
	BOOL bg								
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pString);
	if (pString == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

#ifdef __AUI_USE_DIRECTX__
	HDC hdc;
	HRESULT hr;
	COLORREF oldColor;
	TEXTMETRIC tm;
	sint32 oldMode;
	sint32 saveY = y;

	hr = pSurface->GetDC(&hdc);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_DSGETDCFAILED;

	
	if (bg)
		oldMode = SetBkMode(hdc,TRANSPARENT);
	
	GetTextMetrics(hdc,&tm);

	
	oldColor = SetTextColor(hdc, 0);

	
	HFONT hOldFont;
	if (g_hFont)
		hOldFont = (HFONT)SelectObject(hdc,g_hFont);

	
	COLORREF	dropTextColor = g_colorSet->GetColorRef(COLOR_BUTTON_TEXT_DROP);

	SetTextColor(hdc, dropTextColor);

	sint32 i;	
	for (i=0;i < numStrings;i++)
	{

		TextOut(hdc,x+1,y+1,pString[i],strlen(pString[i]));
		y += tm.tmHeight + tm.tmExternalLeading;
	}

	SetTextColor(hdc, color);
	y = saveY;

	
	for (i=0;i < numStrings;i++)
	{
		TextOut(hdc,x,y,pString[i],strlen(pString[i]));
		y += tm.tmHeight + tm.tmExternalLeading;
	}

	
	if (g_hFont)
		SelectObject(hdc,hOldFont);

	
	SetTextColor(hdc, oldColor);

	
	if (bg)
		SetBkMode(hdc,oldMode);

	hr = pSurface->ReleaseDC(hdc);
	Assert(hr == AUI_ERRCODE_OK);
	if (hr != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_DSRELEASEDCFAILED;
#endif
	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_OldBevelPane16(
	aui_Surface *pSurface,	
	RECT *pRect,			
	sint32 level,			
	BOOL flag				
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect);
	if (pRect == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 left = pRect->left, top = pRect->top, right = pRect->right, bottom = pRect->bottom;

	Assert(left <= right);
	Assert(top <= bottom);
	if ((left >= right) || (top >= bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	uint8 *pSurfBase;

	sint32 errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	
	sint32 width = right - left;
	sint32 height = bottom - top;

	
	if (width > height)
	{
		if (level > (height >> 1))
			level = height >> 1;
	}
	else
	{
		if (level > (width >> 1))
			level = width >> 1;
	}

	pDestPixel = (uint16 *)(pSurfBase + top * surfPitch + (left << 1));
	sint32 inc = (surfPitch >> 1) - width;

	sint32 tempWidth = width-1;
	sint32 tempHeight = height-1;
	sint32 tempInc = inc;

	tempInc = inc;
	tempWidth = width-1;
	pDestPixel = (uint16 *)(pSurfBase + (bottom-1) * surfPitch + ((right-1) << 1));

	if (flag)
	{
		
		for (sint32 j = level;j;j--)
		{
			for (sint32 i = tempWidth;i;i--)
			{
				
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel-- = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel -= tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		sint32 i;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((left+i) << 1));

			for (sint32 y = tempHeight;y;y--)
			{
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}

		tempHeight = height-1;

		
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i) * surfPitch + ((right-1-i) << 1));

			for (sint32 y = tempHeight;y;y--)
			{
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}	
	}
	else	
	{
		
		for (sint32 j = level;j;j--)
		{
			for (sint32 i = tempWidth;i;i--)
			{
				
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel-- = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel -= tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		sint32 i;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((left+i) << 1));

			for (sint32 y = tempHeight;y;y--)
			{
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}

		tempHeight = height-1;

		
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i) * surfPitch + ((right-1-i) << 1));

			for (sint32 y = tempHeight;y;y--)
			{
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}	
	}

	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_OldBevelTabSelected16(
	aui_Surface *pSurface,	
	RECT *pRect,			
	sint32 level,			
	BOOL flag				
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect);
	if (pRect == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 left = pRect->left, top = pRect->top, right = pRect->right, bottom = pRect->bottom;

	Assert(left <= right);
	Assert(top <= bottom);
	if ((left >= right) || (top >= bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	uint8 *pSurfBase;

	sint32 errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	
	sint32 width = right - left;
	sint32 height = bottom - top;

	
	if (width > height)
	{
		if (level > (height >> 1))
			level = height >> 1;
	}
	else
	{
		if (level > (width >> 1))
			level = width >> 1;
	}

	pDestPixel = (uint16 *)(pSurfBase + top * surfPitch + (left << 1));
	sint32 inc = (surfPitch >> 1) - width;

	sint32 tempWidth = width-1;

	sint32 tempHeight = height;
	sint32 tempInc = inc;

	if (flag)
	{
		
		for (sint32 j = level;j;j--)
		{
			for (sint32 i = tempWidth;i;i--)
			{
				
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		sint32 i;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((left+i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
		}

		
		for (i=0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((right-1-i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
		}	
	}
	else 
	{
		
		for (sint32 j = level;j;j--)
		{
			for (sint32 i = tempWidth;i;i--)
			{
				
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		sint32 i;
		for (sint32 i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((left+i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
		}

		
		for (i=0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((right-1-i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
		}	
	}

	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_OldBevelTabDeselected16(
	aui_Surface *pSurface,	
	RECT *pRect,			
	sint32 level,			
	BOOL flag				
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect);
	if (pRect == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 left = pRect->left, top = pRect->top, right = pRect->right, bottom = pRect->bottom;

	Assert(left <= right);
	Assert(top <= bottom);
	if ((left >= right) || (top >= bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	uint8 *pSurfBase;

	sint32 errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	
	sint32 width = right - left;
	sint32 height = bottom - top;

	
	if (width > height)
	{
		if (level > (height >> 1))
			level = height >> 1;
	}
	else
	{
		if (level > (width >> 1))
			level = width >> 1;
	}

	pDestPixel = (uint16 *)(pSurfBase + top * surfPitch + (left << 1));
	sint32 inc = (surfPitch >> 1) - width;

	sint32 tempWidth = width-1;
	sint32 tempHeight = height-1;
	sint32 tempInc = inc;

	if (flag)
	{
		
		for (sint32 j = 0;j < level;j++)
		{
			for (sint32 i=0;i < tempWidth;i++)
			{
				
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		sint32 i;
		for (i=0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((left+i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
			tempHeight -=1;
		}

		tempHeight = height-1;

		
		for (i=0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((right-1-i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
			tempHeight -= 1;
		}	
	}
	else 
	{
		
		for (sint32 j = 0;j < level;j++)
		{
			for (sint32 i=0;i < tempWidth;i++)
			{
				
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		sint32 i;
		for (i=0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((left+i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
			tempHeight -=1;
		}

		tempHeight = height-1;

		
		for (i=0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((right-1-i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
			tempHeight -= 1;
		}	
	}

	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_OldBevelLeftPiece16(
	aui_Surface *pSurface,	
	sint32 xStart,			
	sint32 xEnd,			
	sint32 y,				
	sint32 level,			
	BOOL flag				
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	
	
	Assert(xStart <= xEnd);
	if (xStart >= xEnd) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 width;
	sint32 inc;

	uint8 *pSurfBase;

	sint32 errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	width = xEnd-xStart;

	pDestPixel = (uint16 *)(pSurfBase + y * surfPitch + (xStart << 1));
	inc = (surfPitch >> 1) - (width-1);

	sint32 tempWidth = width;
	sint32 tempInc = inc;

	if (flag)
	{
		
		for (sint32 j = 0;j < level;j++)
		{
			for (sint32 i=0;i < tempWidth;i++)
			{
				
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
			}
			pDestPixel += tempInc;
		}
	}
	else 
	{
		
		for (sint32 j = 0;j < level;j++)
		{
			for (sint32 i=0;i < tempWidth;i++)
			{
				
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
			}
			pDestPixel += tempInc;
		}
	}

	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_OldBevelRightPiece16(
	aui_Surface *pSurface,	
	sint32 xStart,			
	sint32 xEnd,			
	sint32 y,				
	sint32 level,			
	BOOL flag				
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	
	
	Assert(xStart <= xEnd);
	if (xStart >= xEnd) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 width;
	sint32 inc;

	uint8 *pSurfBase;

	sint32 errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	width = xEnd-xStart;

	pDestPixel = (uint16 *)(pSurfBase + y * surfPitch + (xStart << 1));
	inc = (surfPitch >> 1) - (width);

	sint32 tempWidth = width-1;
	sint32 tempInc = inc;

	if (flag)
	{
		
		for (sint32 j = 0;j < level;j++)
		{
			for (sint32 i=0;i < tempWidth;i++)
			{
				
				srcPixel = pixelutils_Shadow(*pDestPixel);
				*pDestPixel++ = srcPixel;
			}
			pDestPixel += tempInc;
		}
	}
	else 
	{
		
		for (sint32 j = 0;j < level;j++)
		{
			for (sint32 i=0;i < tempWidth;i++)
			{
				
				srcPixel = pixelutils_Lightening(*pDestPixel);
				*pDestPixel++ = srcPixel;
			}
			pDestPixel += tempInc;
		}
	}

	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_BevelPane16(
	aui_Surface *pSurface,	
	RECT *pRect,			
	sint32 level,			
	BOOL flag,				
	sint32 blendLight,
	sint32 blendDark,
	AUI_TABGROUP_ALIGNMENT a
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect);
	if (pRect == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 left = pRect->left, top = pRect->top, right = pRect->right, bottom = pRect->bottom;

	Assert(left <= right);
	Assert(top <= bottom);
	if ((left >= right) || (top >= bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	uint8 *pSurfBase;

	sint32 errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	
	sint32 width = right - left;
	sint32 height = bottom - top;

	
	if (width > height)
	{
		if (level > (height >> 1))
			level = height >> 1;
	}
	else
	{
		if (level > (width >> 1))
			level = width >> 1;
	}

	pDestPixel = (uint16 *)(pSurfBase + top * surfPitch + (left << 1));
	sint32 inc = (surfPitch >> 1) - width;

	sint32 tempWidth = width-1;
	sint32 tempHeight = height-1;
	sint32 tempInc = inc;

	tempInc = inc;
	tempWidth = width-1;
	pDestPixel = (uint16 *)(pSurfBase + (bottom-1) * surfPitch + ((right-1) << 1));

	if ( a == AUI_TABGROUP_ALIGNMENT_TOP )
	{
	if (flag)
	{
		
		for (sint32 j = level;j;j--)
		{
			for (sint32 i = tempWidth;i;i--)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel-- = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel -= tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		sint32 i;
		for (sint32 i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((left+i) << 1));

			for (sint32 y = tempHeight;y;y--)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}

		tempHeight = height-1;

		
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i) * surfPitch + ((right-1-i) << 1));

			for (sint32 y = tempHeight;y;y--)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}	
	}
	else	
	{
		sint32 j;
		for (j = level;j;j--)
		{
			for (sint32 i = tempWidth;i;i--)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel-- = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel -= tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		sint32 i;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((left+i) << 1));

			for (sint32 y = tempHeight;y;y--)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}

		tempHeight = height-1;

		
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i) * surfPitch + ((right-1-i) << 1));

			for (sint32 y = tempHeight;y;y--)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}

			tempHeight -= 2;
		}	
	}
	}
	else if ( a == AUI_TABGROUP_ALIGNMENT_LEFT )
	{
	}
	else
	{
		
		Assert( FALSE );
	}

	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_BevelTabSelected16(
	aui_Surface *pSurface,	
	RECT *pRect,			
	sint32 level,			
	BOOL flag,				
	sint32 blendLight,
	sint32 blendDark,
	AUI_TABGROUP_ALIGNMENT a
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect);
	if (pRect == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 left = pRect->left, top = pRect->top, right = pRect->right, bottom = pRect->bottom;

	Assert(left <= right);
	Assert(top <= bottom);
	if ((left >= right) || (top >= bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	uint8 *pSurfBase;

	sint32 errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	
	sint32 width = right - left;
	sint32 height = bottom - top;

	
	if (width > height)
	{
		if (level > (height >> 1))
			level = height >> 1;
	}
	else
	{
		if (level > (width >> 1))
			level = width >> 1;
	}

	pDestPixel = (uint16 *)(pSurfBase + top * surfPitch + (left << 1));
	sint32 inc = (surfPitch >> 1) - width;

	sint32 tempWidth = width-1;

	sint32 tempHeight = height;
	sint32 tempInc = inc;

	if ( a == AUI_TABGROUP_ALIGNMENT_TOP )
	{
	if (flag)
	{
		
		for (sint32 j = level;j;j--)
		{
			for (sint32 i = tempWidth;i;i--)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		sint32 i;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((left+i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
		}

		
		for (i=0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((right-1-i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
		}	
	}
	else 
	{
		
		for (sint32 j = level;j;j--)
		{
			for (sint32 i = tempWidth;i;i--)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		sint32 i;
		for (i = 0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((left+i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
		}

		
		for (i=0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((right-1-i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
		}	
	}
	}
	else if ( a == AUI_TABGROUP_ALIGNMENT_LEFT )
	{
	}
	else
	{
		
		Assert( FALSE );
	}

	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_BevelTabDeselected16(
	aui_Surface *pSurface,	
	RECT *pRect,			
	sint32 level,			
	BOOL flag,				
	sint32 blendLight,
	sint32 blendDark,
	AUI_TABGROUP_ALIGNMENT a
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect);
	if (pRect == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 left = pRect->left, top = pRect->top, right = pRect->right, bottom = pRect->bottom;

	Assert(left <= right);
	Assert(top <= bottom);
	if ((left >= right) || (top >= bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	uint8 *pSurfBase;

	sint32 errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	
	sint32 width = right - left;
	sint32 height = bottom - top;

	
	if (width > height)
	{
		if (level > (height >> 1))
			level = height >> 1;
	}
	else
	{
		if (level > (width >> 1))
			level = width >> 1;
	}

	pDestPixel = (uint16 *)(pSurfBase + top * surfPitch + (left << 1));
	sint32 inc = (surfPitch >> 1) - width;

	sint32 tempWidth = width-1;
	sint32 tempHeight = height-1;
	sint32 tempInc = inc;

	if ( a == AUI_TABGROUP_ALIGNMENT_TOP )
	{
	if (flag)
	{
		
		for (sint32 j = 0;j < level;j++)
		{
			for (sint32 i=0;i < tempWidth;i++)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		sint32 i;
		for (sint32 i=0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((left+i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
			tempHeight -=1;
		}

		tempHeight = height-1;

		
		for (i=0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((right-1-i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
			tempHeight -= 1;
		}	
	}
	else 
	{
		
		for (sint32 j = 0;j < level;j++)
		{
			for (sint32 i=0;i < tempWidth;i++)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
			}
			tempWidth-=2;
			tempInc += 2;
			pDestPixel += tempInc;
		}

		tempInc = (surfPitch >> 1) - 1;

		sint32 i;
		for (i=0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((left+i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
			tempHeight -=1;
		}

		tempHeight = height-1;

		
		for (i=0;i < level;i++)
		{
			pDestPixel = (uint16 *)(pSurfBase + (top+i+1) * surfPitch + ((right-1-i) << 1));

			for (sint32 y=0;y < tempHeight;y++)
			{

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
				pDestPixel += tempInc;
			}
			tempHeight -= 1;
		}	
	}
	}
	else if ( a == AUI_TABGROUP_ALIGNMENT_LEFT )
	{
	}
	else
	{
		
		Assert( FALSE );
	}

	
	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_BevelLeftPiece16(
	aui_Surface *pSurface,	
	RECT *tab,
	RECT *pane,
	sint32 level,			
	BOOL flag,				
	sint32 blendLight,
	sint32 blendDark,
	AUI_TABGROUP_ALIGNMENT a
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 xStart;
	sint32 xEnd;
	sint32 y;

	switch ( a )
	{
	case AUI_TABGROUP_ALIGNMENT_TOP:
		xStart = pane->left;
		xEnd = tab->left;
		y = pane->top;
		break;

	case AUI_TABGROUP_ALIGNMENT_LEFT:
		xStart = tab->bottom;
		xEnd = pane->bottom;
		y = pane->left;
		break;

	default:
		
		Assert( FALSE );
	}
	
	
	
	Assert(xStart <= xEnd);
	if (xStart >= xEnd) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 width;
	sint32 inc;

	uint8 *pSurfBase;

	sint32 errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	width = xEnd-xStart;

	pDestPixel = (uint16 *)(pSurfBase + y * surfPitch + (xStart << 1));
	inc = (surfPitch >> 1) - (width-1);

	sint32 tempWidth = width;
	sint32 tempInc = inc;

	if ( a == AUI_TABGROUP_ALIGNMENT_TOP )
	{
	if (flag)
	{
		
		for (sint32 j = 0;j < level;j++)
		{
			for (sint32 i=0;i < tempWidth;i++)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
			}
			pDestPixel += tempInc;
		}
	}
	else 
	{
		
		for (sint32 j = 0;j < level;j++)
		{
			for (sint32 i=0;i < tempWidth;i++)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
			}
			pDestPixel += tempInc;
		}
	}
	}
	else if ( a == AUI_TABGROUP_ALIGNMENT_LEFT )
	{
	}
	else
	{
		
		Assert( FALSE );
	}

	
	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_BevelRightPiece16(
	aui_Surface *pSurface,	
	RECT *tab,
	RECT *pane,
	sint32 level,			
	BOOL flag,				
	sint32 blendLight,
	sint32 blendDark,
	AUI_TABGROUP_ALIGNMENT a
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 xStart;
	sint32 xEnd;
	sint32 y;

	switch ( a )
	{
	case AUI_TABGROUP_ALIGNMENT_TOP:
		xStart = tab->right;
		xEnd = pane->right;
		y = pane->top;
		break;

	case AUI_TABGROUP_ALIGNMENT_LEFT:
		xStart = pane->top;
		xEnd = tab->top;
		y = pane->left;
		break;

	default:
		
		Assert( FALSE );
	}
	
	
	
	Assert(xStart <= xEnd);
	if (xStart >= xEnd) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(level >= 0);
	if (level <= 0) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	sint32 width;
	sint32 inc;

	uint8 *pSurfBase;

	sint32 errcode = pSurface->Lock(NULL,(LPVOID *)&pSurfBase,0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACELOCKFAILED;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	width = xEnd-xStart;

	pDestPixel = (uint16 *)(pSurfBase + y * surfPitch + (xStart << 1));
	inc = (surfPitch >> 1) - (width);

	sint32 tempWidth = width-1;
	sint32 tempInc = inc;

	if ( a == AUI_TABGROUP_ALIGNMENT_TOP )
	{
	if (flag)
	{
		
		for (sint32 j = 0;j < level;j++)
		{
			for (sint32 i=0;i < tempWidth;i++)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0x0000,blendDark);
				*pDestPixel++ = srcPixel;
			}
			pDestPixel += tempInc;
		}
	}
	else 
	{
		
		for (sint32 j = 0;j < level;j++)
		{
			for (sint32 i=0;i < tempWidth;i++)
			{
				

				srcPixel = pixelutils_BlendFast(*pDestPixel,0xffff,blendLight);
				*pDestPixel++ = srcPixel;
			}
			pDestPixel += tempInc;
		}
	}
	}
	else if ( a == AUI_TABGROUP_ALIGNMENT_LEFT )
	{
	}
	else
	{
		
		Assert( FALSE );
	}

	

	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED;

	return PRIMITIVES_ERRCODE_OK;
}







PRIMITIVES_ERRCODE primitives_DrawFrame16(
	aui_Surface *pSurface,			
	aui_Image *pImageUpperLeft,		
	aui_Image *pImageUpperRight,	
	aui_Image *pImageLowerLeft,		
	aui_Image *pImageLowerRight,	
	Pattern *pPatternLeft,			
	Pattern *pPatternTop,			
	Pattern *pPatternRight,			
	Pattern *pPatternBottom,		
	RECT *pRect						
	)
{
	Assert(pSurface);
	if (pSurface == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pImageUpperLeft);
	if (pImageUpperLeft==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;
	Assert(pImageUpperRight);
	if (pImageUpperRight==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;
	Assert(pImageLowerLeft);
	if (pImageLowerLeft==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;
	Assert(pImageLowerRight);
	if (pImageLowerRight==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;
	
	Assert(pPatternLeft);
	if (pPatternLeft==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;
	Assert(pPatternTop);
	if (pPatternTop==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;
	Assert(pPatternRight);
	if (pPatternRight==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;
	Assert(pPatternBottom);
	if (pPatternBottom==NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect);
	if (pRect == NULL) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	Assert(pRect->left < pRect->right);
	Assert(pRect->top < pRect->bottom);
	if ((pRect->left >= pRect->right) || (pRect->top >= pRect->bottom)) return PRIMITIVES_ERRCODE_INVALIDPARAM;

	
	sint32	imageWidth = pImageUpperLeft->TheSurface()->Width();
	sint32	imageHeight = pImageUpperLeft->TheSurface()->Height();
	RECT	srcRect = { 0, 0, imageWidth, imageHeight };
	
	sint32	x = pRect->left, y = pRect->top;

	g_c3ui->TheBlitter()->Blt(pSurface, x, y, pImageUpperLeft->TheSurface(), &srcRect, k_AUI_BLITTER_FLAG_COPY);

	x = pRect->right - imageWidth;

	g_c3ui->TheBlitter()->Blt(pSurface, x, y, pImageUpperRight->TheSurface(), &srcRect, k_AUI_BLITTER_FLAG_COPY);

	x = pRect->left;
	y = pRect->bottom - imageHeight;

	g_c3ui->TheBlitter()->Blt(pSurface, x, y, pImageLowerLeft->TheSurface(), &srcRect, k_AUI_BLITTER_FLAG_COPY);

	x = pRect->right - imageWidth;

	g_c3ui->TheBlitter()->Blt(pSurface, x, y, pImageLowerRight->TheSurface(), &srcRect, k_AUI_BLITTER_FLAG_COPY);

	
	RECT rect;
	rect.left = pRect->left + imageWidth;
	rect.top = pRect->top;
	rect.right = pRect->right - imageWidth;
	rect.bottom = pRect->top + imageHeight;

	pPatternTop->Draw(pSurface, &rect);

	rect.top = pRect->bottom - imageHeight;
	rect.bottom = pRect->bottom;

	pPatternBottom->Draw(pSurface, &rect);

	rect.left = pRect->left;
	rect.top = pRect->top + imageHeight;
	rect.right = pRect->left + imageWidth;
	rect.bottom = pRect->bottom - imageHeight;

	pPatternLeft->Draw(pSurface, &rect);

	rect.left = pRect->right - imageWidth;
	rect.right = pRect->right;

	pPatternRight->Draw(pSurface, &rect);

	return PRIMITIVES_ERRCODE_OK;
}

#include "tileutils.h"

void primitives_HackTileDraw(aui_Surface *pSurface)
{
	FILE		*file;
	uint16		len;
	Pixel16		*data, *dataPtr;;
	sint32		x, y;
	sint32		startX, endX;
	uint32		accumTable[k_TILE_PIXEL_HEIGHT][3];

	file = fopen("gtfb000.bin", "rb");
	fread((void *)&accumTable, 1, sizeof(uint32)*3*k_TILE_PIXEL_HEIGHT, file);
	fread((void *)&len, 1, sizeof(uint16), file);
	data = new Pixel16[len/2];
	fread((void *)data, 1, len, file);
	fclose(file);

	dataPtr = data;

	uint8 *pSurfBase;

	sint32 errcode;

	errcode = pSurface->Lock(NULL, (LPVOID *)&pSurfBase, 0);
	if ( errcode != AUI_ERRCODE_OK ) return;

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	sint32 yoffset = 300;

	{


		for (y=0; y<k_TILE_PIXEL_HEIGHT; y++) {
			if (y<=23) {
				startX = (23-y)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (y-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}

			for (x = startX; x<endX; x++) {
				srcPixel = *dataPtr++;
				switch (srcPixel) {
				case 0x0000 : srcPixel = 0xF800; break;
				case 0x0001 : srcPixel = 0x07E0; break;
				case 0x0002 : srcPixel = 0x001F; break;
				case 0x0003 : srcPixel = 0xF81F; break;
				}
				pDestPixel = (Pixel16 *)(pSurfBase + ((y+yoffset) * surfPitch) + (x << 1));
				*pDestPixel = srcPixel;
			}
		}
	}

	
	errcode = pSurface->Unlock((LPVOID)pSurfBase);


}



#include "tiledmap.h"
#include "tileset.h"

extern TiledMap *g_tiledMap;


void primitives_HackStencilDraw(aui_Surface *pSurface)
{
	extern uint32 g_bitsTable[];
	extern Pixel16 g_offsets[];





	if (g_tiledMap == NULL) return;


	static uint16 from=4, to=14;

	tileutils_LoadStencil();



	Pixel16 *image;


	TileSet *tileSet = g_tiledMap->GetTileSet();
	
	if (tileSet) 
		image = tileSet->GetTransitionData(from, to, 0);

	if (image != NULL) {

		Assert(pSurface);
		if (pSurface==NULL) return;

		uint8 *pSurfBase;

		sint32 errcode;
		errcode = pSurface->Lock(NULL, (LPVOID *)&pSurfBase, 0);
		if ( errcode != AUI_ERRCODE_OK ) return;

		
		sint32 surfWidth = pSurface->Width();
		sint32 surfHeight = pSurface->Height();
		sint32 surfPitch = pSurface->Pitch();

		
		Pixel16 srcPixel;
		uint16 *pDestPixel;

		{
			uint32		accum;
			sint32		i,j;
			sint32		nudge;
			sint32		x, y;
			Pixel16		*srcPixelPtr;

			x = 10; y = 400;

			srcPixelPtr = image;
			for (i=0; i<48; i++) {
				accum = g_bitsTable[i];

				if (i<=23) nudge = (23-i)*2;
				else nudge = (i-24)*2;

				x=nudge;

				for (j=0; j<32; j++) {
					if (accum & 1) {
						
						srcPixel = *srcPixelPtr;
						srcPixelPtr++;
						pDestPixel = (Pixel16 *)(pSurfBase + (y * surfPitch) + (x << 1));
						*pDestPixel = srcPixel;
					} else {
						
					}
					accum>>=1;
					x++;
				}
				y++;
			}
		}

		
		errcode = pSurface->Unlock((LPVOID)pSurfBase);

	}
	uint16 tmp;

	tmp = from;
	from = to;
	to = tmp;











}



#define DIR_STEEP		1
#define DIR_NEGY		2

#define PIXINC(a, b)		{a,b}
#define SWAPVARS(a, b)		{ sint32 temp; temp = a; a = b; b = temp; }
#define PIXADDR(x, y)		(pSurfBase + (y * surfPitch) + x * 2)
#define COVERAGE(dist)		(4)
#define SQRTFUNC(val)		(sqrt(1/(1+val*val)))
#define BLEND(p1, p2)		pixelutils_BlendFast(p1, p2, 8)

static int adj_pixinc_x[4] =	{	1, 0, 1, 0		};
static int adj_pixinc_y[4] =	{	0, 1, 0, -1		};
static int diag_pixinc_x[4] =	{	1, 1, 1, 1,		};
static int diag_pixinc_y[4] =	{	1, 1, -1, -1	};
static int orth_pixinc_x[4] =	{	0, 1, 0, 1		};
static int orth_pixinc_y[4] =	{	1, 0, -1, 0		};

double Pmax = 1.0;

void primitives_DrawAALine16(aui_Surface *pSurface, sint32 x1, sint32 y1, sint32 x2, sint32 y2, Pixel16 color)
{
	sint32		Bvar,
				Bainc,
				Bdinc;
	double		Pmid,
				Pnow,
				Painc,
				Pdinc,
				Poinc;
	uint8		*mid_addr,
				*now_addr;
	sint32		addr_ainc,
				addr_dinc,
				addr_oinc;
	sint32		dx,
				dy,
				dir;
	double		slope;

	Assert(pSurface);
	if (pSurface==NULL) return;

	
	if (x1 == x2 && y1 == y2) return;

	uint8		*pSurfBase;

	sint32 errcode;
	errcode = pSurface->Lock(NULL, (LPVOID *)&pSurfBase, 0);
	if ( errcode != AUI_ERRCODE_OK ) return;

	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	if (x1 > x2) {
		SWAPVARS(x1, x2);
		SWAPVARS(y1, y2);
	}

	dx = x2 - x1;
	dy = y2 - y1;

	dir = 0;
	if (dy < 0) {
		dir |= DIR_NEGY;
		dy = -dy;
	}
	if (dy > dx) {
		dir |= DIR_STEEP;
		SWAPVARS(dx, dy)
	}

	mid_addr = PIXADDR(x1, y1);

	addr_ainc = adj_pixinc_y[dir] * surfPitch + adj_pixinc_x[dir] * 2;
	addr_dinc = diag_pixinc_y[dir] * surfPitch + diag_pixinc_x[dir] * 2;
	addr_oinc = orth_pixinc_y[dir] * surfPitch + orth_pixinc_x[dir] * 2;

	slope = (double)dy / (double)dx;

	Poinc = SQRTFUNC(slope);
	Painc = slope * Poinc;
	Pdinc = Painc - Poinc;
	
	Pmid = 0;

	Bainc = dy * 2;
	Bdinc = (dy-dx) * 2;
	Bvar = Bainc - dx;

	do {

		*(Pixel16 *)mid_addr = pixelutils_Blend(color, *mid_addr, 13);

		
		for (
				Pnow = Poinc - Pmid, now_addr = mid_addr + addr_oinc;
				Pnow < Pmax;
				Pnow += Poinc, now_addr += addr_oinc
			)
			*(Pixel16 *)now_addr =  pixelutils_Blend(color, *now_addr, COVERAGE(Pnow));

		
		for (
				Pnow = Poinc + Pmid, now_addr = mid_addr - addr_oinc;
				Pnow < Pmax;
				Pnow += Poinc, now_addr -= addr_oinc
			)
			*(Pixel16 *)now_addr = pixelutils_Blend(color, *now_addr, COVERAGE(Pnow));

				if (Bvar < 0) {
					Bvar += Bainc;
					mid_addr = (uint8 *)((size_t)mid_addr + addr_ainc);
					Pmid += Painc;
				} else {
					Bvar += Bdinc;
					mid_addr = (uint8 *)((size_t)mid_addr + addr_dinc);
					Pmid += Pdinc;
				}

				--dx;
	} while (dx >= 0);

	errcode = pSurface->Unlock((LPVOID)pSurfBase);
}

void primitives_DrawDashedAALine16(aui_Surface *pSurface, sint32 x1, sint32 y1, sint32 x2, sint32 y2, Pixel16 color, sint32 length)
{
	sint32		Bvar,
				Bainc,
				Bdinc;
	double		Pmid,
				Pnow,
				Painc,
				Pdinc,
				Poinc;
	uint8		*mid_addr,
				*now_addr;
	sint32		addr_ainc,
				addr_dinc,
				addr_oinc;
	sint32		dx,
				dy,
				dir;
	double		slope;

	Assert(pSurface);
	if (pSurface==NULL) return;

	
	if (x1 == x2 && y1 == y2) return;

	uint8		*pSurfBase;

	sint32 errcode;
	errcode = pSurface->Lock(NULL, (LPVOID *)&pSurfBase, 0);
	if ( errcode != AUI_ERRCODE_OK ) return;

	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	if (x1 > x2) {
		SWAPVARS(x1, x2);
		SWAPVARS(y1, y2);
	}

	dx = x2 - x1;
	dy = y2 - y1;

	dir = 0;
	if (dy < 0) {
		dir |= DIR_NEGY;
		dy = -dy;
	}
	if (dy > dx) {
		dir |= DIR_STEEP;
		SWAPVARS(dx, dy)
	}

	mid_addr = PIXADDR(x1, y1);

	addr_ainc = adj_pixinc_y[dir] * surfPitch + adj_pixinc_x[dir] * 2;
	addr_dinc = diag_pixinc_y[dir] * surfPitch + diag_pixinc_x[dir] * 2;
	addr_oinc = orth_pixinc_y[dir] * surfPitch + orth_pixinc_x[dir] * 2;

	slope = (double)dy / (double)dx;

	Poinc = SQRTFUNC(slope);
	Painc = slope * Poinc;
	Pdinc = Painc - Poinc;
	
	Pmid = 0;

	Bainc = dy * 2;
	Bdinc = (dy-dx) * 2;
	Bvar = Bainc - dx;

	sint32 draw = length;
	sint32 skip = 0;

	do {

		if ( draw ) {
			*(Pixel16 *)mid_addr = pixelutils_Blend(color, *mid_addr, 13);
			draw--;
			if ( !draw ) {
				skip = length;
			}
		}
		else {
			skip--;
			if ( !skip ) {
				draw = length;
			}
		}

		
		for (
				Pnow = Poinc - Pmid, now_addr = mid_addr + addr_oinc;
				Pnow < Pmax;
				Pnow += Poinc, now_addr += addr_oinc
			)
			if ( draw  ) {
				*(Pixel16 *)now_addr =  pixelutils_Blend(color, *now_addr, COVERAGE(Pnow));
				draw--;
				if ( !draw ) {
					skip = length;
				}
			}
			else {
				skip--;
				if ( !skip ) {
					draw = length;
				}
			}

		
		for (
				Pnow = Poinc + Pmid, now_addr = mid_addr - addr_oinc;
				Pnow < Pmax;
				Pnow += Poinc, now_addr -= addr_oinc
			)
			if ( draw ) {
				*(Pixel16 *)now_addr = pixelutils_Blend(color, *now_addr, COVERAGE(Pnow));
				draw--;
				if ( !draw ) {
					skip = length;
				}
			}
			else {
				skip--;
				if ( !skip ) {
					draw = length;
				}
			}

				if (Bvar < 0) {
					Bvar += Bainc;
					mid_addr = (uint8 *)((size_t)mid_addr + addr_ainc);
					Pmid += Painc;
				} else {
					Bvar += Bdinc;
					mid_addr = (uint8 *)((size_t)mid_addr + addr_dinc);
					Pmid += Pdinc;
				}

				--dx;
	} while (dx >= 0);

	errcode = pSurface->Unlock((LPVOID)pSurfBase);
}

void primitives_BlendSurfaces( aui_Surface *pOldSurface, aui_Surface *pNewSurface, aui_Surface *pDstSurface, RECT *pDstRect, sint32 blend )
{
	BOOL			oldWasUnlocked = FALSE;
	BOOL			newWasUnlocked = FALSE;
	BOOL			dstWasUnlocked = FALSE;
	AUI_ERRCODE		errcode;

	uint8 *pOldBase;
	uint8 *pNewBase;
	uint8 *pDstBase;

	if (pOldSurface->Buffer() == NULL) { 
		errcode = pOldSurface->Lock(NULL,(LPVOID *)&pOldBase,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;
		oldWasUnlocked = TRUE;
	} else {
		pOldBase = pOldSurface->Buffer();
	}

	if (pNewSurface->Buffer() == NULL) { 
		errcode = pNewSurface->Lock(NULL,(LPVOID *)&pNewBase,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;
		newWasUnlocked = TRUE;
	} else {
		pNewBase = pNewSurface->Buffer();
	}

	if (pDstSurface->Buffer() == NULL) { 
		errcode = pDstSurface->Lock(NULL,(LPVOID *)&pDstBase,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;
		dstWasUnlocked = TRUE;
	} else {
		pDstBase = pDstSurface->Buffer();
	}

	
	sint32 oldSurfWidth = pOldSurface->Width();
	sint32 oldSurfHeight = pOldSurface->Height();
	sint32 oldSurfPitch = pOldSurface->Pitch();

	sint32 newSurfWidth = pNewSurface->Width();
	sint32 newSurfHeight = pNewSurface->Height();
	sint32 newSurfPitch = pNewSurface->Pitch();

	sint32 dstSurfWidth = pDstSurface->Width();
	sint32 dstSurfHeight = pDstSurface->Height();
	sint32 dstSurfPitch = pDstSurface->Pitch();

	
	uint16 *pDestPixel;
	uint16 *pOldSrc = (uint16 *)pOldBase;
	uint16 *pNewSrc = (uint16 *)pNewBase;

	
	sint32 width = pDstRect->right - pDstRect->left;
	sint32 height = pDstRect->bottom - pDstRect->top;

	pDestPixel = (uint16 *)(pDstBase + pDstRect->top * dstSurfPitch + (pDstRect->left << 1));
	sint32 dstInc = (dstSurfPitch >> 1) - width;

	
	sint32 oldInc = (oldSurfPitch >> 1) - width;
	sint32 newInc = (newSurfPitch >> 1) - width;

	Pixel16 color;

	for (sint32 j=height;j;j--) {
		for (sint32 i=width;i;i--) {
			color = pixelutils_BlendFast( *pOldSrc++, *pNewSrc++, blend );
			*pDestPixel++ = color;
		}
		pDestPixel += dstInc;
		pOldSrc += oldInc;
		pNewSrc += newInc;
	}

	if ( oldWasUnlocked ) {
		
		errcode = pOldSurface->Unlock((LPVOID)pOldBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;
	}

	if ( newWasUnlocked ) {
		
		errcode = pNewSurface->Unlock((LPVOID)pNewBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;
	}

	if ( dstWasUnlocked ) {
		
		errcode = pDstSurface->Unlock((LPVOID)pDstBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;
	}

	return;
}

void primitives_LightenSurface( aui_Surface *pSurface, sint32 percentLighten )
{
	BOOL			wasUnlocked = FALSE;
	AUI_ERRCODE		errcode;

	uint8 *pBase;

	if (pSurface->Buffer() == NULL) { 
		errcode = pSurface->Lock(NULL,(LPVOID *)&pBase,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;
		wasUnlocked = TRUE;
	} else {
		pBase = pSurface->Buffer();
	}

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	uint16 *pDestPixel;

	
	sint32 width = surfWidth;
	sint32 height = surfHeight;

	pDestPixel = (uint16 *)(pBase);
	sint32 inc = (surfPitch >> 1) - width;

	Pixel16 color;

	for (sint32 j=height;j;j--) {
		for (sint32 i=width;i;i--) {
			if ( *pDestPixel ) {
				color = pixelutils_PercentLighten( *pDestPixel, percentLighten );
				*pDestPixel++ = color;
			}
			else {
				pDestPixel++;
			}
		}
		pDestPixel += inc;
	}

	if ( wasUnlocked ) {
		
		errcode = pSurface->Unlock((LPVOID)pBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;
	}

	return;
}


void primitives_LightenRect(aui_Surface *pSurface, RECT &rect, sint32 percentLighten)
{
	BOOL			wasUnlocked = FALSE;
	AUI_ERRCODE		errcode;

	uint8 *pBase;

	if (pSurface->Buffer() == NULL) { 
		errcode = pSurface->Lock(NULL,(LPVOID *)&pBase,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;
		wasUnlocked = TRUE;
	} else {
		pBase = pSurface->Buffer();
	}

	
	sint32 surfWidth = pSurface->Width();
	sint32 surfHeight = pSurface->Height();
	sint32 surfPitch = pSurface->Pitch();

	
	uint16 *pDestPixel;

	
	sint32 width = rect.right - rect.left;
	sint32 height = rect.bottom - rect.top;

	Pixel16 color;

	for (sint32 j=0; j<height; j++) {

		pDestPixel = (uint16 *)(pBase + (rect.top + j) * surfPitch + rect.left * 2);
		for (sint32 i=width;i;i--) {
			color = pixelutils_Desaturate( *pDestPixel );
			*pDestPixel++ = color;
		}
	}

	if ( wasUnlocked ) {
		
		errcode = pSurface->Unlock((LPVOID)pBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;
	}

	return;
}
