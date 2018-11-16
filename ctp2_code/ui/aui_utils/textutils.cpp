//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Text handling utilities
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Ambiguous fabs calls resolved.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_directsurface.h"

#include "textutils.h"
#include "primitives.h"
#include "colorset.h"
#include "CivPaths.h"

#ifndef WIN32
struct hfont_t { int iDummy; };
#endif
HFONT				g_hFont;
extern CivPaths		*g_civPaths;
extern ColorSet		*g_colorSet;

#define k_FONT_FILE_NAME	"ArialBd.ttf"
#define k_FONT_FACE_NAME	"Arial Bold"

#ifdef __AUI_USE_DIRECTX__
namespace
{
	LONG RoundToNearest(double const a_Rational)
	{
		return static_cast<LONG>(0.5 + a_Rational);
	}
};	// namespace
#endif

void textutils_Initialize(void)
{
	MBCHAR		path[_MAX_PATH];

	g_civPaths->FindFile(C3DIR_FONTS, k_FONT_FILE_NAME, path);
	textutils_AddFont(path);
}

void textutils_Cleanup(void)
{
}







sint32 textutils_GetWidth(
		aui_Surface *pSurface,
		const MBCHAR *pString
		)
{
#ifdef __AUI_USE_DIRECTX__
	aui_DirectSurface *pDirectSurface = (aui_DirectSurface *)pSurface;
	HDC hdc;
	HRESULT hr;
	TEXTMETRIC tm;
	SIZE size;

	Assert(pDirectSurface);
	if (pDirectSurface==NULL) return AUI_ERRCODE_INVALIDPARAM;
	Assert(pString);
	if (pString==NULL) return AUI_ERRCODE_INVALIDPARAM;

	hr = pDirectSurface->GetDC(&hdc);
	if (hr != AUI_ERRCODE_OK) return AUI_ERRCODE_SURFACELOCKFAILED;

	GetTextMetrics(hdc, &tm);
	GetTextExtentPoint32(hdc,pString,strlen(pString),&size);

	hr = pDirectSurface->ReleaseDC(hdc);
	if (hr != AUI_ERRCODE_OK) return AUI_ERRCODE_SURFACEUNLOCKFAILED;

	return size.cx;
#else // __AUI_USE_DIRECTX__
	return 0;
#endif // __AUI_USE_DIRECTX__
}







sint32 textutils_GetHeight(
		aui_Surface *pSurface,
		const MBCHAR *pString
		)
{
#ifdef __AUI_USE_DIRECTX__
	aui_DirectSurface *pDirectSurface = (aui_DirectSurface *)pSurface;
	HDC hdc;
	HRESULT hr;
	TEXTMETRIC tm;

	Assert(pDirectSurface);
	if (pDirectSurface==NULL) return AUI_ERRCODE_INVALIDPARAM;
	Assert(pString);
	if (pString==NULL) return AUI_ERRCODE_INVALIDPARAM;

	hr = pDirectSurface->GetDC(&hdc);
	if (hr != AUI_ERRCODE_OK) return AUI_ERRCODE_SURFACELOCKFAILED;

	GetTextMetrics(hdc,&tm);

	hr = pDirectSurface->ReleaseDC(hdc);
	if (hr != AUI_ERRCODE_OK) return AUI_ERRCODE_SURFACEUNLOCKFAILED;

	return tm.tmHeight+tm.tmExternalLeading;
#else // __AUI_USE_DIRECTX__
	return 0;
#endif // __AUI_USE_DIRECTX__
}

sint32 textutils_GetFontHeight(
		aui_Surface *pSurface,
		uint32 size
		)
{
#ifdef __AUI_USE_DIRECTX__
	aui_DirectSurface *pDirectSurface = (aui_DirectSurface *)pSurface;
	HDC hdc;
	HRESULT hr;
	TEXTMETRIC tm;
	HFONT hfont = textutils_CreateFont(surface, k_FONT_FACE_NAME, size * 10, 0, 0, TRUE);
	HFONT holdfont;

	Assert(surface);
	if (surface==NULL) return AUI_ERRCODE_INVALIDPARAM;

	hr = surface->GetDC(&hdc);
	if (hr != AUI_ERRCODE_OK) return AUI_ERRCODE_SURFACELOCKFAILED;

	holdfont = (HFONT)SelectObject(hdc,hfont);
	GetTextMetrics(hdc,&tm);
	DeleteObject(SelectObject(hdc,holdfont));

	hr = surface->ReleaseDC(hdc);
	if (hr != AUI_ERRCODE_OK) return AUI_ERRCODE_SURFACEUNLOCKFAILED;

	return tm.tmHeight+tm.tmExternalLeading;
#else
	return 0;
#endif
}

HFONT textutils_GetFont(
		aui_Surface *surface,
		uint32 size
		)
{
	return textutils_CreateFont(surface, k_FONT_FACE_NAME, size * 10, 0, 0, TRUE);
}








RECT textutils_GetBounds(
		aui_Surface *pSurface,
		const MBCHAR *pString
		)
{
#ifdef __AUI_USE_DIRECTX__
	aui_DirectSurface *pDirectSurface = (aui_DirectSurface *)pSurface;
	HDC hdc;
	HRESULT hr;
#endif // __AUI_USE_DIRECTX__
	RECT rect = {0,0,0,0};

#ifdef __AUI_USE_DIRECTX__
	SIZE size;

	Assert(pDirectSurface);
	if (pDirectSurface==NULL) return rect;
	Assert(pString);
	if (pString==NULL) return rect;

	hr = pDirectSurface->GetDC(&hdc);
	if (hr != AUI_ERRCODE_OK) return rect;

	GetTextExtentPoint32(hdc, pString, strlen(pString),  &size);

	rect.right = size.cx;
	rect.bottom = size.cy;

	hr = pDirectSurface->ReleaseDC(hdc);
	if (hr != AUI_ERRCODE_OK) return rect;
#endif // __AUI_USE_DIRECTX__
	return rect;
}







RECT textutils_CenterText(
		aui_Surface *pSurface,
		const MBCHAR *pString,
		RECT *pDestRect
		)
{
	RECT center = {0,0,0,0};

	Assert(pSurface);
	if (pSurface==NULL) return center;
	Assert(pString);
	if (pString==NULL) return center;
	Assert(pDestRect);
	if (pDestRect==NULL) return center;

	RECT bounds = textutils_GetBounds(pSurface,pString);

	OffsetRect(&bounds,
				pDestRect->left + ((pDestRect->right-pDestRect->left)/2 - (bounds.right-bounds.left)/2),
				pDestRect->top + ((pDestRect->bottom-pDestRect->top)/2 - (bounds.bottom-bounds.top)/2));

	return bounds;

	sint32 boundsWidth = bounds.right - bounds.left;
	sint32 boundsHeight = bounds.bottom - bounds.top;
	sint32 destWidth = pDestRect->right - pDestRect->left;
	sint32 destHeight = pDestRect->bottom - pDestRect->top;

	Assert(boundsWidth>=0);
	Assert(boundsHeight>=0);




    Assert(destWidth>=0);
	Assert(destHeight>=0);

	sint32 xOffset = (destWidth - boundsWidth)>>1;
	sint32 yOffset = (destHeight - boundsHeight)>>1;

	center.left = pDestRect->left + xOffset;
	center.top = pDestRect->top + yOffset;
	center.right = pDestRect->right - xOffset;
	center.bottom = pDestRect->bottom - yOffset;

	return center;
}







void textutils_AddFont(
		const MBCHAR *szFileName
		)
{
#ifdef __AUI_USE_DIRECTX__
	sint32 i = AddFontResource(szFileName);
	Assert(i);
	if (i==NULL) return;
	SendMessage(HWND_BROADCAST, WM_FONTCHANGE, NULL, NULL);
#endif
}







void textutils_RemoveFont(
		const MBCHAR *szFileName
		)
{
#ifdef __AUI_USE_DIRECTX__
	sint32 i = RemoveFontResource(szFileName);
	Assert(i);
	if (i==NULL) return;
	SendMessage(HWND_BROADCAST, WM_FONTCHANGE, NULL, NULL);
#endif
}







HFONT textutils_CreateFont(
		aui_Surface *pSurface,
		const MBCHAR *szFaceName,
		sint32 iDeciPtHeight,
		sint32 iDeciPtWidth,
		sint32 iAttributes,

		BOOL fLogRes
		)
{
#ifdef __AUI_USE_DIRECTX__
	aui_DirectSurface *pDirectSurface = (aui_DirectSurface *)pSurface;
	FLOAT		cxDpi, cyDpi;
	HFONT		hFont;
	LOGFONT		lf;
	POINT		pt;
	TEXTMETRIC	tm;
	HDC			hdc;
	HRESULT		hr;
	BOOL		wasLocked = FALSE;

	Assert(pDirectSurface);
	if (pDirectSurface==NULL) return NULL;

	if (pDirectSurface->Buffer()) {
		pDirectSurface->Unlock(pDirectSurface->Buffer());
		wasLocked = TRUE;
	}

	hr = pDirectSurface->GetDC(&hdc);
	Assert(hr==AUI_ERRCODE_OK);
	if (hr != AUI_ERRCODE_OK) return NULL;

	SaveDC(hdc);

	SetGraphicsMode(hdc, GM_ADVANCED);
	ModifyWorldTransform(hdc, NULL, MWT_IDENTITY);
	SetViewportOrgEx(hdc, 0, 0, NULL);
	SetWindowOrgEx(hdc, 0, 0, NULL);

	if (fLogRes)
	{
		cxDpi = (FLOAT) GetDeviceCaps(hdc, LOGPIXELSX);
		cyDpi = (FLOAT) GetDeviceCaps(hdc, LOGPIXELSY);
	}
	else
	{
		cxDpi = (FLOAT) (25.4*GetDeviceCaps(hdc, HORZRES) /
							GetDeviceCaps(hdc, HORZSIZE));

		cyDpi = (FLOAT) (25.4*GetDeviceCaps(hdc, VERTRES) /
							GetDeviceCaps(hdc, VERTSIZE));
	}

	pt.x = (sint32) (iDeciPtWidth * cxDpi / 72);
	pt.y = (sint32) (iDeciPtHeight * cyDpi / 72);

	DPtoLP(hdc, &pt, 1);

	lf.lfHeight = - RoundToNearest(fabs(static_cast<double>(pt.y)) / 10.0);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = iAttributes & k_FONT_ATTR_BOLD			?  700 : 0;
	lf.lfItalic = iAttributes & k_FONT_ATTR_ITALIC			?	1 : 0;
	lf.lfUnderline = iAttributes & k_FONT_ATTR_UNDERLINE	?	1 : 0;
	lf.lfStrikeOut = iAttributes & k_FONT_ATTR_STRIKEOUT	?	1 : 0;
	lf.lfCharSet = 0;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = 0;
	lf.lfPitchAndFamily = 0;

	strcpy(lf.lfFaceName, szFaceName);

	hFont = CreateFontIndirect(&lf);

	if (iDeciPtWidth != 0)
	{
		hFont = (HFONT) SelectObject(hdc, hFont);
		GetTextMetrics(hdc, &tm);
		DeleteObject(SelectObject(hdc, hFont));
		lf.lfWidth = RoundToNearest(tm.tmAveCharWidth *
			                        fabs(static_cast<double>(pt.x)) /
			                        fabs(static_cast<double>(pt.y))
						           );
		hFont = CreateFontIndirect(&lf);
	}

	RestoreDC(hdc, -1);

	hr = pDirectSurface->ReleaseDC(hdc);
	Assert(hr==AUI_ERRCODE_OK);
	if (hr != AUI_ERRCODE_OK) return NULL;

	if (wasLocked) {
		uint8 *buffer;
		pDirectSurface->Lock(NULL, (VOID **)&buffer, 0);
	}
	return hFont;
#else // __AUI_USE_DIRECTX__
	return 0;
#endif // __AUI_USE_DIRECTX__
}







void textutils_SelectFont(
		HFONT hFont
		)
{
	Assert(hFont);
	if (hFont==NULL) return;

	if (g_hFont)
		textutils_DeleteFont(g_hFont);

	g_hFont = hFont;
}

void textutils_DeleteFont(
		HFONT hFont
		)
{

	Assert(hFont);
	if (hFont==NULL) return;
#ifdef __AUI_USE_DIRECTX__
	DeleteObject(hFont);
#else
	delete hFont;
#endif // __AUI_USE_DIRECTX__
}




void textutils_DropString(aui_Surface *surface, const MBCHAR *text, sint32 x, sint32 y, sint32 size, COLOR color, sint32 font)
{
	HFONT		tempFont;
	COLORREF	colorRef = g_colorSet->GetColorRef(color);

	tempFont = textutils_CreateFont(surface, k_FONT_FACE_NAME, size * 10, 0, 0, TRUE);
	Assert(tempFont);
	if (!tempFont) return;

	textutils_SelectFont(tempFont);
	primitives_DropText(surface, x, y, text, colorRef, 1);
	textutils_DeleteFont(tempFont);
}




void textutils_ColoredDropString(aui_Surface *surface, const MBCHAR *text, sint32 x, sint32 y, sint32 size, COLOR textColor, COLOR dropColor, sint32 font)
{
	HFONT		tempFont;
	COLORREF	colorRefText = g_colorSet->GetColorRef(textColor);
	COLORREF	colorRefDrop = g_colorSet->GetColorRef(dropColor);

	tempFont = textutils_CreateFont(surface, k_FONT_FACE_NAME, size * 10, 0, 0, TRUE);
	Assert(tempFont);
	if (!tempFont) return;

	textutils_SelectFont(tempFont);
	primitives_ColoredDropText(surface, x, y, text, colorRefText, colorRefDrop, 1);
	textutils_DeleteFont(tempFont);
}




void textutils_CenteredDropString(aui_Surface *surface, const MBCHAR *text, RECT *destRect, sint32 size, COLOR color, sint32 font)
{
	HFONT		tempFont;
	COLORREF	colorRef = g_colorSet->GetColorRef(color);

	tempFont = textutils_CreateFont(surface, k_FONT_FACE_NAME, size * 10, 0, 0, TRUE);
	Assert(tempFont);
	if (!tempFont) return;


	textutils_SelectFont(tempFont);

	textutils_CenterText(surface, text, destRect);

	primitives_DropTextCentered(surface, destRect, text, colorRef, 1);

	textutils_DeleteFont(tempFont);
}




void textutils_CenteredColoredDropString(aui_Surface *surface, const MBCHAR *text, RECT *destRect, sint32 size, COLOR textColor, COLOR dropColor,sint32 font)
{
	HFONT		tempFont;
	COLORREF	colorRefText = g_colorSet->GetColorRef(textColor);
	COLORREF	colorRefDrop = g_colorSet->GetColorRef(dropColor);

	tempFont = textutils_CreateFont(surface, k_FONT_FACE_NAME, size * 10, 0, 0, TRUE);
	Assert(tempFont);
	if (!tempFont) return;


	textutils_SelectFont(tempFont);

	textutils_CenterText(surface, text, destRect);

	primitives_ColoredDropTextCentered(surface, destRect, text, colorRefText, colorRefDrop, 1);

	textutils_DeleteFont(tempFont);
}

void textutils_SizedBoundedString(aui_Surface *surface, const MBCHAR *text, RECT *destRect, sint32 size, COLOR color, sint32 font)
{
	HFONT		tempFont;
	COLORREF	colorRef = g_colorSet->GetColorRef(color);

	tempFont = textutils_CreateFont(surface, k_FONT_FACE_NAME, size * 10, 0, 0, TRUE);
	Assert(tempFont);
	if (!tempFont) return;

	textutils_SelectFont(tempFont);

	textutils_CenterText(surface, text, destRect);

	primitives_DrawBoundedText(surface, destRect, text, colorRef, 1);

	textutils_DeleteFont(tempFont);
}







void textutils_TestFonts(
		aui_Surface *pSurface
		)
{
#ifdef __AUI_USE_DIRECTX__
	aui_DirectSurface *pDirectSurface = (aui_DirectSurface *) pSurface;
	MBCHAR l[80];

	textutils_AddFont("../Fusion/Fonts/BickleyScript.ttf");
	textutils_AddFont("../Fusion/Fonts/FineHand.ttf");
	textutils_AddFont("../Fusion/Fonts/Flight.ttf");
	textutils_AddFont("../Fusion/Fonts/JohnHandy.ttf");
	textutils_AddFont("../Fusion/Fonts/RageItalic.ttf");
	textutils_AddFont("../Fusion/Fonts/Symbol.ttf");

	HFONT bickleyScript;
	bickleyScript = textutils_CreateFont(pDirectSurface, "Bickley Script LET", 80, 0, 0, TRUE);
	textutils_SelectFont(bickleyScript);
	sprintf(l,"Bickley Script test");
	primitives_DropText(pDirectSurface,15,100,l,0xcccc,1);
	textutils_DeleteFont(bickleyScript);

	HFONT fineHand;
	fineHand = textutils_CreateFont(pDirectSurface, "Fine Hand LET", 180, 0, 0, TRUE);
	textutils_SelectFont(fineHand);
	sprintf(l,"Fine Hand test");
	primitives_DropText(pDirectSurface,15,140,l,0xcccc,1);
	textutils_DeleteFont(fineHand);

	HFONT flight;
	flight = textutils_CreateFont(pDirectSurface, "Flight LET", 150, 0, 0, TRUE);
	textutils_SelectFont(flight);
	sprintf(l,"Flight test");
	primitives_DropText(pDirectSurface,15,180,l,0xcccc,1);
	textutils_DeleteFont(flight);

	HFONT johnHandy;
	johnHandy = textutils_CreateFont(pDirectSurface, "John Handy LET", 200, 0, 0, TRUE);
	textutils_SelectFont(johnHandy);
	sprintf(l,"John Handy test");
	primitives_DropText(pDirectSurface,15,220,l,0xcccc,1);
	textutils_DeleteFont(johnHandy);

	HFONT rageItalic;
	rageItalic = textutils_CreateFont(pDirectSurface, "Rage Italic LET", 120, 0, 0, TRUE);
	textutils_SelectFont(rageItalic);
	sprintf(l,"Rage Italic test");
	primitives_DropText(pDirectSurface,15,260,l,0xcccc,1);
	textutils_DeleteFont(rageItalic);

	HFONT symbol;
	symbol = textutils_CreateFont(pDirectSurface, "Symbol", 70, 0, 0, TRUE);
	textutils_SelectFont(symbol);
	sprintf(l,"Symbol test");
	primitives_DropText(pDirectSurface,15,300,l,0xcccc,1);
	textutils_DeleteFont(symbol);

	textutils_RemoveFont("../Fusion/Fonts/BickleyScript.ttf");
	textutils_RemoveFont("../Fusion/Fonts/FineHand.ttf");
	textutils_RemoveFont("../Fusion/Fonts/Flight.ttf");
	textutils_RemoveFont("../Fusion/Fonts/JohnHandy.ttf");
	textutils_RemoveFont("../Fusion/Fonts/RageItalic.ttf");
	textutils_RemoveFont("../Fusion/Fonts/Symbol.ttf");
#endif // __AUI_USE_DIRECTX__
}
