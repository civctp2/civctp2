//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// _DEBUG
// Generate extra debug output
//
// _JAPANESE
// Support SJIS characters
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added support for Japanese characters (by t.s. 2003-12)
//
//----------------------------------------------------------------------------
//
/// \file  aui_bitmapfont.h
/// \brief Font handling

#ifndef __AUI_BITMAPFONT_H__
#define __AUI_BITMAPFONT_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// #include <>

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

#include "aui_base.h"
#include "tech_wllist.h"
#include "aui_surface.h"

#define k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT		0x00000001
#define k_AUI_BITMAPFONT_DRAWFLAG_JUSTRIGHT		0x00000002
#define k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER	0x00000004
#define k_AUI_BITMAPFONT_DRAWFLAG_JUSTFULL		0x00000008
#define k_AUI_BITMAPFONT_DRAWFLAG_JUSTALL		0x00000010
#define k_AUI_BITMAPFONT_DRAWFLAG_VERTCENTER	0x00000020
#define k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP		0x00000040




#define k_AUI_BITMAPFONT_SURFACEWIDTH		512

#define k_AUI_BITMAPFONT_MAXSTRLEN			4096

#define k_AUI_BITMAPFONT_MAXDESCLEN			(2*MAX_PATH)


class aui_BitmapFont : public aui_Base
{
public:

	aui_BitmapFont(
		AUI_ERRCODE *retval,
		const MBCHAR *descriptor = NULL );
	virtual ~aui_BitmapFont();

	static void AttributesToDescriptor(
		MBCHAR out[ k_AUI_BITMAPFONT_MAXDESCLEN + 1 ],
		const MBCHAR *ttffile,
		sint32 pointSize,
		sint32 bold,
		sint32 italic );
	static void DescriptorToAttributes(
		const MBCHAR *in,
		MBCHAR ttffile[ MAX_PATH + 1 ],
		sint32 *pointSize,
		sint32 *bold,
		sint32 *italic );

protected:
	aui_BitmapFont() : aui_Base() {}
	AUI_ERRCODE InitCommon( const MBCHAR *descriptor );

public:
	AUI_ERRCODE Load( void );
	AUI_ERRCODE Unload( void );

	BOOL IsLoaded( void ) const { return m_ttFace.z != NULL; }

	BOOL HasCached( void ) const { return m_surfaceList->L() != 0; }

	MBCHAR		*GetFilename( void ) const { return (MBCHAR *)m_descriptor; }
	AUI_ERRCODE	SetFilename( const MBCHAR *descriptor );

	MBCHAR		*GetTTFFile( void ) const { return (MBCHAR *)m_ttffile; }
	AUI_ERRCODE	SetTTFFile( const MBCHAR *ttffile );
	sint32		GetPointSize( void ) const { return m_pointSize; }
	AUI_ERRCODE	SetPointSize( sint32 pointSize );
	sint32		GetBold( void ) const { return m_bold; }
	sint32		SetBold( sint32 bold );
	sint32		GetItalic( void ) const { return m_italic; }
	sint32		SetItalic( sint32 italic );

	sint32		GetLineSkip( void ) const { return m_lineSkip; }
	sint32		SetLineSkip( sint32 lineSkip );
	sint32		GetTabSkip( void ) const { return m_tabSkip; }
	sint32		SetTabSkip( sint32 tabSkip );




	sint32		GetMaxHeight( void ) const { return m_maxHeight; }
	sint32		SetMaxHeight( sint32 maxHeight );
	sint32		GetBaseLine( void ) const { return m_baseLine; }
	sint32		SetBaseLine( sint32 baseLine );

	AUI_ERRCODE DrawString(
		aui_Surface *surface,
		RECT *bound,
		RECT *clip,
		const MBCHAR *string,
		uint32 flags = 0,
		COLORREF color = RGB(255,255,255),
		sint32 underline = 0 );

	struct GlyphInfo
	{
		MBCHAR		c;
#if defined(_JAPANESE)
		uint16		c2;
#endif
		aui_Surface	*surface;
		RECT		bbox;
		sint16		bearingX;
		sint16		bearingY;
		sint16		advance;
	};

	GlyphInfo *GetGlyphInfo( MBCHAR c );
	GlyphInfo *GetGlyphInfo( const MBCHAR *c );

	AUI_ERRCODE RenderLine(
		aui_Surface *surface,
		RECT *bound,
		RECT *clip,
		POINT *penPos,
		const MBCHAR **start,
		const MBCHAR *stop,
		COLORREF color = RGB(255,255,255),
		sint32 underline = 0,
		sint32 *ascend = NULL,
		sint32 *descend = NULL,
		BOOL wrap = FALSE,
		BOOL midWordBreaks = FALSE,
		BOOL modWordBreaksOnly = FALSE );


	AUI_ERRCODE GetLineInfo(
		RECT *wrap,
		POINT *penPos,
		sint32 *ascend,
		sint32 *descend,
		const MBCHAR **start,
		const MBCHAR *stop,
		BOOL midWordBreaks = FALSE,
		BOOL midWordBreaksOnly = FALSE );




	sint32 GetStringWidth( const MBCHAR *string );


	AUI_ERRCODE RenderGlyph(
		aui_Surface *destSurf,
		RECT *clipRect,
		POINT *penPos,
		GlyphInfo *gi,
		COLORREF color = RGB(255,255,255),
		sint32 underline = 0 );

	BOOL TruncateString( MBCHAR *name, sint32 width );

#ifdef _DEBUG
	void DumpCachedSurfaces( aui_Surface *destSurf = NULL );
#endif




	AUI_ERRCODE RenderGlyph16(
		aui_Surface *destSurf,
		RECT *destRect,
		aui_Surface *srcSurf,
		RECT *srcRect,
		COLORREF color = RGB(255,255,255) );

protected:

#if !defined(_JAPANESE)
	BOOL IsCached( MBCHAR c )
	{ return m_glyphs[ (uint16)uint8(c) ].surface != NULL; }
#else
	BOOL IsCached( MBCHAR c )
	{ return IsCached( (uint16)uint8(c) ); }
	BOOL IsCached( uint16 c )
	{ return m_glyphs[ c ].surface != NULL; }
#endif


	MBCHAR m_descriptor[ k_AUI_BITMAPFONT_MAXDESCLEN + 1 ];

	MBCHAR		m_ttffile[ MAX_PATH + 1 ];
	sint32		m_pointSize;
	sint32		m_bold;
	sint32		m_italic;

	tech_WLList<aui_Surface *> *m_surfaceList;

#if !defined(_JAPANESE)
	GlyphInfo m_glyphs[ 256 ];
#else
	GlyphInfo m_glyphs[ 65536 ];
#endif

	sint32 m_curOffset;
	sint32 m_maxHeight;
	sint32 m_baseLine;
	sint32 m_lineSkip;
	sint32 m_tabSkip;

	static sint32 m_bitmapFontRefCount;

	static TT_Engine	m_ttEngine;
	TT_Face				m_ttFace;
	TT_Face_Properties	m_ttFaceProperties;
	TT_Instance			m_ttInstance;
	TT_Instance_Metrics	m_ttInstanceMetrics;
	TT_CharMap			m_ttCharMap;
};

#endif
