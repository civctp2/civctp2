#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TEXTUTILS_H
#define __TEXTUTILS_H

#include "pixelutils.h"
#include "colorset.h"

class aui_Surface;

#define k_FONT_ATTR_BOLD		1
#define k_FONT_ATTR_ITALIC		2
#define k_FONT_ATTR_UNDERLINE	4
#define k_FONT_ATTR_STRIKEOUT	8

extern HFONT g_hFont;

void	textutils_Initialize(void);
void	textutils_Cleanup(void);

sint32	textutils_GetWidth(aui_Surface *pSurface, const MBCHAR *pString);
sint32	textutils_GetHeight(aui_Surface *pSurface, const MBCHAR *pString);
sint32	textutils_GetFontHeight(aui_Surface *pSurface, uint32 size);
HFONT	textutils_GetFont(aui_Surface *pSurface, uint32 size);

RECT	textutils_GetBounds(aui_Surface *pSurface, const MBCHAR *pString);
RECT	textutils_CenterText(aui_Surface *pSurface, const MBCHAR *pString,
					  RECT *pDestRect);

void	textutils_AddFont(const MBCHAR *szFileName);
void	textutils_RemoveFont(const MBCHAR *szFileName);
HFONT	textutils_CreateFont(aui_Surface *pSurface, const MBCHAR *szFaceName,
						   sint32 iDeciPtHeight, sint32 iDeciPtWidth,
						   sint32 iAttributes, BOOL fLogRes);

void	textutils_SelectFont(HFONT hFont);
void	textutils_DeleteFont(HFONT hFont);

void	textutils_DropString(aui_Surface *surface, const MBCHAR *text, sint32 x, sint32 y, sint32 size, COLOR color, sint32 font);
void	textutils_ColoredDropString(aui_Surface *surface, const MBCHAR *text, sint32 x, sint32 y, sint32 size, COLOR textColor, COLOR dropColor, sint32 font);
void	textutils_CenteredDropString(aui_Surface *surface, const MBCHAR *text, RECT *destRect, sint32 size, COLOR color, sint32 font);
void	textutils_CenteredColoredDropString(aui_Surface *surface, const MBCHAR *text, RECT *destRect, sint32 size, COLOR textColor, COLOR dropColor,sint32 font);
void	textutils_SizedBoundedString(aui_Surface *surface, const MBCHAR *text, RECT *destRect, sint32 size, COLOR color, sint32 font);

void	textutils_TestFonts(aui_Surface *pSurface);

#endif
