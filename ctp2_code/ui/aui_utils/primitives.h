//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The primitive rectabgles for painting
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Standardized code (May 21st 2006 Martin Gühmann)
// - Added primitives_GetScreenAdjustedRectCopy function. (3-Mar-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#include "pixelutils.h"

#ifndef __AUI_USE_DIRECTX__
#include "aui_textbase.h"
#include "aui_stringtable.h"

class aui_StringTable;
class aui_Surface;
#endif
class aui_Image;
class Pattern;
//enum AUI_TABGROUP_ALIGNMENT;
#include "aui_tabgroup.h"

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

enum LINE_FLAGS {
	LF_NONE         = 0x00,
	LF_ANTI_ALIASED = 0x01,
	LF_SHADOW       = 0x02
};

enum TRIANGLE_ID {
	TI_LEFT_TOP,
	TI_LEFT_BOTTOM,
	TI_RIGHT_TOP,
	TI_RIGHT_BOTTOM
};

// All kind of patterns may be defined and used
// Restrictions:
//   pattern-length has to be a multitude of 2, i.e. 2, 4, 8, 16 or 32
//   lowest and highest element of a pattern has to be 1 (when there are no more ones then pattern is restarted)
static const uint32 LINE_PATTERN_DOT         = 0b00000000000000000000000000001001;
static const uint32 LINE_PATTERN_DOT_LENGTH  = 4;
static const uint32 LINE_PATTERN_DASH        = 0b00000000000000000000000011000011;
static const uint32 LINE_PATTERN_DASH_LENGTH = 8;

#ifndef __AUI_USE_DIRECTX__
#include "c3ui.h"
extern C3UI			*g_c3ui;

inline aui_BitmapFont * getBitmapFont()
    {
    aui_BitmapFont *font= NULL;

    font = g_c3ui->LoadBitmapFont("arialbd.ttf"); // using hard coded font name (from TiledMapFontStringTable) because this is needed in the debug version before DB initialization
    Assert(font);
    font->SetPointSize(9); // size set for TiledMapFontStringTable

    return font;
    }
#endif

PRIMITIVES_ERRCODE	primitives_FrameRect16(aui_Surface *pSurface,RECT *pRect,Pixel16 color);
PRIMITIVES_ERRCODE	primitives_PaintRect16(aui_Surface *pSurface,RECT *pRect,Pixel16 color);
PRIMITIVES_ERRCODE  primitives_Scale16(aui_Surface *pSrc, aui_Surface *pDst, const fRect &sRect, const fRect &dRect, bool bFilter);

PRIMITIVES_ERRCODE	primitives_DrawLine16(aui_Surface *pSurface,
			  sint32 x1,sint32 y1,sint32 x2,sint32 y2,Pixel16 color);

PRIMITIVES_ERRCODE	primitives_DrawText(aui_Surface *pDirectSurface,
				sint32 x, sint32 y, const MBCHAR *pString,COLORREF color, bool bg);
PRIMITIVES_ERRCODE	primitives_DrawBoundedText(aui_Surface *pDirectSurface,
				RECT *bound, const MBCHAR *pString,COLORREF color,BOOL bg);
PRIMITIVES_ERRCODE	primitives_DrawTextBatch(aui_Surface *pDirectSurface,sint32 x,sint32 y,
				const MBCHAR **pString,sint32 numStrings,COLORREF color,BOOL bg);
PRIMITIVES_ERRCODE	primitives_DropText(aui_Surface *pDirectSurface,
				sint32 x,sint32 y,const MBCHAR *pString,COLORREF color,BOOL bg);
PRIMITIVES_ERRCODE	primitives_ColoredDropText(aui_Surface *pDirectSurface,
				sint32 x,sint32 y,const MBCHAR *pString,COLORREF textColor,COLORREF dropColor,BOOL bg);
PRIMITIVES_ERRCODE primitives_DropTextCentered(
	aui_Surface *pDirectSurface,
	RECT *destRect,
	const MBCHAR *pString,
	COLORREF color,
	BOOL bg
	);
PRIMITIVES_ERRCODE primitives_ColoredDropTextCentered(
	aui_Surface *pDirectSurface,
	RECT *destRect,
	const MBCHAR *pString,
	COLORREF textColor,
	COLORREF dropColor,
	BOOL bg
	);
PRIMITIVES_ERRCODE	primitives_DropTextBatch(aui_Surface *pDirectSurface,sint32 x,sint32 y,
				const MBCHAR **pString,sint32 numStrings,COLORREF color,BOOL bg);

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

void	primitives_BlendSurfaces( aui_Surface *pOldSurface, aui_Surface *pNewSurface, aui_Surface *pDstSurface, RECT *pDstRect, sint32 blend );
void	primitives_LightenSurface( aui_Surface *pSurface, sint32 percentLighten );
RECT	primitives_GetScreenAdjustedRectCopy(aui_Surface *surf, RECT &clipRect);

// Clipped primitives
void primitives_ClippedPaintRect16(aui_Surface & surf, const RECT & rect, Pixel16 color,
		uint8 alpha = pixelutils_OPAQUE);
inline void primitives_ClippedPaint16(aui_Surface & surf, sint32 x, sint32 y, sint32 width, sint32 height,
		Pixel16 color, uint8 alpha = pixelutils_OPAQUE)
{
	RECT paintRect = { x, y, x + width - 1, y + height - 1 };
	primitives_ClippedPaintRect16(surf, paintRect, color, alpha);
}
void primitives_ClippedFrameRect16(aui_Surface & surf, const RECT & rect, Pixel16 color,
		uint8 alpha = pixelutils_OPAQUE);
inline void primitives_ClippedFrame16(aui_Surface & surf, sint32 x, sint32 y, sint32 width, sint32 height,
		Pixel16 color, uint8 alpha = pixelutils_OPAQUE)
{
	RECT frameRect = { x, y, x + width - 1, y + height - 1 };
	primitives_ClippedFrameRect16(surf, frameRect, color, alpha);
}
void primitives_ClippedShadowRect16(aui_Surface & surf, const RECT & rect);

void primitives_ClippedTriangle16(aui_Surface & surf, const RECT & rect, TRIANGLE_ID triangleId, Pixel16 color,
		uint8 alpha = pixelutils_OPAQUE, bool antiAliased = false);

// Clipped lines primitives
void primitives_SetPatternOffset(sint32 x, sint32 y);
void primitives_BaseClippedDrawLine16(aui_Surface & surf, sint32 x1, sint32 y1, sint32 x2, sint32 y2, Pixel16 color,
		LINE_FLAGS lineFlags , uint32 fullPattern, uint32 patternLength); // Do not use directly; use below signatures

inline void primitives_ClippedLine16(aui_Surface & surf, sint32 x1, sint32 y1, sint32 x2, sint32 y2, Pixel16 color,
		LINE_FLAGS lineFlags = LF_NONE)
{
	primitives_BaseClippedDrawLine16(surf, x1, y1, x2, y2, color, lineFlags, 0, 0);
}

inline void primitives_ClippedPatternLine16(aui_Surface & surf, sint32 x1, sint32 y1, sint32 x2, sint32 y2, Pixel16 color,
		uint32 fullPattern, uint32 patternLength, LINE_FLAGS lineFlags = LF_NONE)
{
	primitives_BaseClippedDrawLine16(surf, x1, y1, x2, y2, color, lineFlags, fullPattern, patternLength);
}

#endif
