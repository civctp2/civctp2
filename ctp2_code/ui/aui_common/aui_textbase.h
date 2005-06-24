//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Text base class
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
// _MSC_VER		
// - Compiler version (for the Microsoft C++ compiler only)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialised all members in the default constructor, to prevent a crash 
//   in the destructor.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __AUI_TEXTBASE_H__
#define __AUI_TEXTBASE_H__


class aui_Surface;
class aui_BitmapFont;



#define k_AUI_TEXTBASE_DEFAULTMAXLENGTH			256
#define k_AUI_TEXTBASE_DEFAULT_COLOR			RGB(255,255,255)
#define k_AUI_TEXTBASE_DEFAULT_SHADOWCOLOR		RGB(0,0,0)
#define k_AUI_TEXTBASE_DEFAULT_FONTSIZE			12
#define k_AUI_TEXTBASE_DEFAULT_BOLD				0
#define k_AUI_TEXTBASE_DEFAULT_ITALIC			0
#define k_AUI_TEXTBASE_DEFAULT_UNDERLINE		0
#define k_AUI_TEXTBASE_DEFAULT_SHADOW			FALSE
#define k_AUI_TEXTBASE_DEFAULT_FONTNAME			"arial.ttf"
#define k_AUI_TEXTBASE_DEFAULT_USEDATABASE		FALSE


#define k_AUI_TEXTBASE_LDL_TEXT					"text"
#define k_AUI_TEXTBASE_LDL_MAXLENGTH			"maxlength"
#define k_AUI_TEXTBASE_LDL_RED					"fontcolorred"
#define k_AUI_TEXTBASE_LDL_GREEN				"fontcolorgreen"
#define k_AUI_TEXTBASE_LDL_BLUE					"fontcolorblue"
#define k_AUI_TEXTBASE_LDL_SHADOWRED			"fontshadowcolorred"
#define k_AUI_TEXTBASE_LDL_SHADOWGREEN			"fontshadowcolorgreen"
#define k_AUI_TEXTBASE_LDL_SHADOWBLUE			"fontshadowcolorblue"
#define k_AUI_TEXTBASE_LDL_FONTSIZE				"fontsize"
#define k_AUI_TEXTBASE_LDL_BOLD					"boldfont"
#define k_AUI_TEXTBASE_LDL_ITALIC				"italicfont"
#define k_AUI_TEXTBASE_LDL_UNDERLINE			"underlinefont"
#define k_AUI_TEXTBASE_LDL_SHADOW				"shadowfont"
#define k_AUI_TEXTBASE_LDL_FONTSIZEDEPENDENT	"fontsizedependent"
#define k_AUI_TEXTBASE_LDL_FONTNAME				"fontname"


#define k_AUI_TEXTBASE_LDL_NODATABASE			"nodatabase"


#define k_AUI_TEXTBASE_LDL_BLTTYPE				"textblttype"
#define k_AUI_TEXTBASE_LDL_CENTER				"center"
#define k_AUI_TEXTBASE_LDL_FILL					"fill"


#define k_AUI_TEXTBASE_LDL_JUST					"just"
#define k_AUI_TEXTBASE_LDL_JUSTLEFT				"left"
#define k_AUI_TEXTBASE_LDL_JUSTRIGHT			"right"
#define k_AUI_TEXTBASE_LDL_JUSTCENTER			"center"
#define k_AUI_TEXTBASE_LDL_JUSTFULL				"full"
#define k_AUI_TEXTBASE_LDL_JUSTALL				"all"

#define k_AUI_TEXTBASE_LDL_VERTCENTER			"vertcenter"
#define k_AUI_TEXTBASE_LDL_WORDWRAP				"wordwrap"



class aui_TextBase
{
public:
	
	aui_TextBase(
		MBCHAR *ldlBlock,
		const MBCHAR *text );
	aui_TextBase(
		const MBCHAR *text = NULL,
		uint32 maxLength = 0 );
	virtual ~aui_TextBase();

protected:
	AUI_ERRCODE InitCommonLdl(
		MBCHAR *ldlBlock,
		const MBCHAR *text );
	AUI_ERRCODE InitCommon(
		const MBCHAR *text,
		uint32 maxLength,
		MBCHAR *fontname,
		sint32 fontsize,
		COLORREF color,
		COLORREF shadowcolor,
		sint32 bold,
		sint32 italic,
		sint32 underline,
		BOOL shadow,
		uint32 flags );

public:
	MBCHAR *GetText( void ) const { return (MBCHAR *)m_text; }
	virtual AUI_ERRCODE	SetText(
		const MBCHAR *text,
		uint32 len = 0xffffffff );

	
	AUI_ERRCODE	SetText2(MBCHAR *fmt,...);

	virtual AUI_ERRCODE	AppendText( MBCHAR *text );

	COLORREF GetTextColor( void ) const { return m_textcolor; }
	void SetTextColor( COLORREF color ) { m_textcolor = color; }

	sint32 GetTextUnderline( void ) const { return m_textunderline; }
	void SetTextUnderline( sint32 underline ) { m_textunderline = underline; }

	BOOL GetTextShadow(void) const { return m_textshadow; }
	void SetTextShadow( BOOL flag ) { m_textshadow = flag; }

	COLORREF GetTextShadowColor(void) const { return m_textshadowcolor; }
	void SetTextShadowColor( COLORREF color ) { m_textshadowcolor = color; }

	
	aui_BitmapFont *GetTextFont( void ) const { return m_textfont; }

	uint32 &TextFlags( void ) { return m_textflags; }

	
	
	virtual void SetTextFont( MBCHAR *ttffile );
	virtual void SetTextFontSize( sint32 pointSize );
	void SetTextBold( sint32 bold );
	void SetTextItalic( sint32 italic );

	void TextReloadFont( void );

protected:
	
	static uint32 FindNextWordBreak(
		MBCHAR *text, HDC hdc, sint32 width );
	static MBCHAR *FindNextToken(
		MBCHAR *text, MBCHAR *tokenList, sint32 count );

	AUI_ERRCODE DrawThisText(
		aui_Surface *destSurf,
		RECT *destRect );

	MBCHAR		*m_text;
	uint32		m_maxLength;
	uint32		m_curLength;

	aui_BitmapFont	*m_textfont;
	uint32			m_textflags;

	COLORREF	m_textshadowcolor;
	BOOL		m_textshadow;

	COLORREF	m_textcolor;
	sint32		m_textunderline;

	
	BOOL		m_textreload;
	MBCHAR		m_textttffile[ MAX_PATH + 1 ];
	sint32		m_textpointsize;
	sint32		m_textbold;
	sint32		m_textitalic;
};


#endif 
