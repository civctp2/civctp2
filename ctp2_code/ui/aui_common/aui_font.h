#ifndef __AUI_FONT_H__
#define __AUI_FONT_H__

#include "aui_base.h"

#define k_AUI_FONT_MAXFONTNAME	32


class aui_Font : public aui_Base
{
public:

	aui_Font(
		AUI_ERRCODE *retval,
		MBCHAR *descriptor = NULL );
	virtual ~aui_Font();

protected:
	aui_Font() : aui_Base() {}
	AUI_ERRCODE InitCommon( MBCHAR *descriptor );

public:
	AUI_ERRCODE Load( void );
	AUI_ERRCODE Unload( void );

	HFONT		GetHFONT( void ) const { return m_hfont; }
	MBCHAR		*GetDescriptor( void ) const { return (MBCHAR *)m_descriptor; }

	AUI_ERRCODE	SetDescriptor( MBCHAR *descriptor );

	MBCHAR		*GetFilename( void ) const { return GetDescriptor(); }
	AUI_ERRCODE	SetFilename( MBCHAR *filename )
	{ return SetDescriptor( filename ); }

protected:
	AUI_ERRCODE	SetVariablesFromDescriptor( void );

	MBCHAR	m_descriptor[ MAX_PATH + 1 ];

	HFONT	m_hfont;

	sint32	m_height;
	sint32	m_averageWidth;
	sint32	m_escapement;
	sint32	m_orientation;
	sint32	m_weight;
	uint32	m_italic;
	uint32	m_underline;
	uint32	m_strikeout;
	uint32	m_charset;
	uint32	m_outputPrecision;
	uint32	m_clipPrecision;
	uint32	m_quality;
	uint32	m_pitchAndFamily;
	MBCHAR	m_fontname[ k_AUI_FONT_MAXFONTNAME + 1 ];
};

#endif
