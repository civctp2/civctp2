#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#include "ctp2_enums.h"
#include "pixelutils.h"

class aui_Surface;
class aui_DirectSurface;
class aui_Image;
class Pattern;

enum PRIMITIVES_ERRCODE
{
	PRIMITIVES_ERRCODE_OK,
	PRIMITIVES_ERRCODE_INVALIDPARAM,
	PRIMITIVES_ERRCODE_SURFACELOCKFAILED,
	PRIMITIVES_ERRCODE_SURFACEUNLOCKFAILED,
	PRIMITIVES_ERRCODE_DSGETDCFAILED,
	PRIMITIVES_ERRCODE_DSRELEASEDCFAILED
};

struct fRect
{
	float left, top, right, bottom;
};

PRIMITIVES_ERRCODE	primitives_SetRect(RECT *rect,sint32 left,sint32 top,sint32 right,sint32 bottom);
PRIMITIVES_ERRCODE	primitives_FrameRect16(aui_Surface *pSurface,RECT *pRect,Pixel16 color);
PRIMITIVES_ERRCODE	primitives_PaintRect16(aui_Surface *pSurface,RECT *pRect,Pixel16 color);
PRIMITIVES_ERRCODE	primitives_OldBevelRect16(aui_Surface *pSurface, RECT *pRect,sint32 level,BOOL flag);
PRIMITIVES_ERRCODE  primitives_Scale16(aui_Surface *pSrc, aui_Surface *pDst, const fRect &sRect, const fRect &dRect, bool bFilter);

PRIMITIVES_ERRCODE	primitives_DrawLine16(aui_Surface *pSurface, 
			  sint32 x1,sint32 y1,sint32 x2,sint32 y2,Pixel16 color);



PRIMITIVES_ERRCODE	primitives_DrawText(aui_DirectSurface *pDirectSurface,
				sint32 x,sint32 y,MBCHAR *pString,COLORREF color,BOOL bg);
PRIMITIVES_ERRCODE	primitives_DrawBoundedText(aui_DirectSurface *pDirectSurface,
				RECT *bound,MBCHAR *pString,COLORREF color,BOOL bg);
PRIMITIVES_ERRCODE	primitives_DrawTextBatch(aui_DirectSurface *pDirectSurface,sint32 x,sint32 y,
				MBCHAR **pString,sint32 numStrings,COLORREF color,BOOL bg);
PRIMITIVES_ERRCODE	primitives_DropText(aui_DirectSurface *pDirectSurface,
				sint32 x,sint32 y,MBCHAR *pString,COLORREF color,BOOL bg);
PRIMITIVES_ERRCODE	primitives_ColoredDropText(aui_DirectSurface *pDirectSurface,
				sint32 x,sint32 y,MBCHAR *pString,COLORREF textColor,COLORREF dropColor,BOOL bg);
PRIMITIVES_ERRCODE primitives_DropTextCentered(
	aui_DirectSurface *pDirectSurface,	
	RECT *destRect,
	MBCHAR *pString,					
	COLORREF color,						
	BOOL bg								
	);
PRIMITIVES_ERRCODE primitives_ColoredDropTextCentered(
	aui_DirectSurface *pDirectSurface,	
	RECT *destRect,
	MBCHAR *pString,					
	COLORREF textColor,						
	COLORREF dropColor,						
	BOOL bg								
	);
PRIMITIVES_ERRCODE	primitives_DropTextBatch(aui_DirectSurface *pDirectSurface,sint32 x,sint32 y,
				MBCHAR **pString,sint32 numStrings,COLORREF color,BOOL bg);

PRIMITIVES_ERRCODE	primitives_OldBevelPane16(aui_Surface *pSurface, RECT *pRect,sint32 level,BOOL flag);
PRIMITIVES_ERRCODE	primitives_OldBevelTabSelected16(aui_Surface *pSurface, RECT *pRect,sint32 level,BOOL flag);
PRIMITIVES_ERRCODE	primitives_OldBevelTabDeselected16(aui_Surface *pSurface, RECT *pRect,sint32 level,BOOL flag);
PRIMITIVES_ERRCODE	primitives_OldBevelLeftPiece16(aui_Surface *pSurface, sint32 xStart, sint32 xEnd, sint32 y, sint32 level,BOOL flag);
PRIMITIVES_ERRCODE	primitives_OldBevelRightPiece16(aui_Surface *pSurface, sint32 xStart, sint32 xEnd, sint32 y, sint32 level,BOOL flag);

PRIMITIVES_ERRCODE	primitives_BevelPane16(
	aui_Surface *pSurface, RECT *pRect, sint32 level, BOOL flag,
	sint32 blendLight, sint32 blendDark, AUI_TABGROUP_ALIGNMENT a);
PRIMITIVES_ERRCODE	primitives_BevelTabSelected16(
	aui_Surface *pSurface, RECT *pRect, sint32 level, BOOL flag,
	sint32 blendLight, sint32 blendDark, AUI_TABGROUP_ALIGNMENT a);
PRIMITIVES_ERRCODE	primitives_BevelTabDeselected16(
	aui_Surface *pSurface, RECT *pRect, sint32 level, BOOL flag,
	sint32 blendLight, sint32 blendDark, AUI_TABGROUP_ALIGNMENT a);
PRIMITIVES_ERRCODE	primitives_BevelLeftPiece16(
	aui_Surface *pSurface, RECT *tab, RECT *pane, sint32 level,
	BOOL flag, sint32 blendLight, sint32 blendDark, AUI_TABGROUP_ALIGNMENT a);
PRIMITIVES_ERRCODE	primitives_BevelRightPiece16(
	aui_Surface *pSurface, RECT *tab, RECT *pane, sint32 level,
	BOOL flag, sint32 blendLight, sint32 blendDark, AUI_TABGROUP_ALIGNMENT a);

PRIMITIVES_ERRCODE	primitives_BevelRect16(aui_Surface *pSurface, RECT *pRect, sint32 level, BOOL flag, sint32 blendLight, sint32 blendDark);
PRIMITIVES_ERRCODE	primitives_FrameThickRect16(aui_Surface *pSurface, RECT *pRect, Pixel16 color, sint32 level );


PRIMITIVES_ERRCODE	primitives_DrawFrame16(aui_Surface *pSurface, 
				aui_Image *pImageUpperLeft, aui_Image *pImageUpperRight, aui_Image *pImageLowerLeft, aui_Image *pImageLowerRight,
				Pattern *pPatternLeft, Pattern *pPatternTop, Pattern *pPatternRight, Pattern *pPatternBottom,
				RECT *pDestRect);

void	primitives_DrawAALine16(aui_Surface *pSurface, sint32 x1, sint32 y1, sint32 x2, sint32 y2, Pixel16 color);
void	primitives_DrawDashedAALine16(aui_Surface *pSurface, sint32 x1, sint32 y1, sint32 x2, sint32 y2, Pixel16 color, sint32 length = 0);

void	primitives_HackStencilDraw(aui_Surface *pSurface);

void	primitives_BlendSurfaces( aui_Surface *pOldSurface, aui_Surface *pNewSurface, aui_Surface *pDstSurface, RECT *pDstRect, sint32 blend );
void	primitives_LightenSurface( aui_Surface *pSurface, sint32 percentLighten );
void	primitives_LightenRect(aui_Surface *pSurface, RECT &rect, sint32 percentLighten);

#endif
