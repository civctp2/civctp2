//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : General text handling
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
// - Unload font when something fails.
// - Added left, center and right to textblttype. (Aug 16th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_textbase.h"

#include "aui_ui.h"
#include "aui_ldl.h"
#include "aui_surface.h"
#include "aui_bitmapfont.h"
#include "StrDB.h"            // g_theStringDB



aui_TextBase::aui_TextBase
(
	MBCHAR const *  ldlBlock,
	MBCHAR const *  text 
)
{
	InitCommonLdl(ldlBlock,	text);
}



aui_TextBase::aui_TextBase
(
	MBCHAR const *  text,
	uint32          maxLength 
)
{
	InitCommon
    (
        text,
		maxLength,
		k_AUI_TEXTBASE_DEFAULT_FONTNAME,
		k_AUI_TEXTBASE_DEFAULT_FONTSIZE,
		k_AUI_TEXTBASE_DEFAULT_COLOR,
		k_AUI_TEXTBASE_DEFAULT_SHADOWCOLOR,
		k_AUI_TEXTBASE_DEFAULT_BOLD,
		k_AUI_TEXTBASE_DEFAULT_ITALIC,
		k_AUI_TEXTBASE_DEFAULT_UNDERLINE,
		k_AUI_TEXTBASE_DEFAULT_SHADOW,
		k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER | k_AUI_BITMAPFONT_DRAWFLAG_VERTCENTER
    );
}



AUI_ERRCODE aui_TextBase::InitCommonLdl(MBCHAR const * ldlBlock, MBCHAR const * text)
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	sint32 fontsize = k_AUI_TEXTBASE_DEFAULT_FONTSIZE;
	if ( block->GetAttributeType( k_AUI_TEXTBASE_LDL_FONTSIZE )
			== ATTRIBUTE_TYPE_INT) 
		fontsize = block->GetInt( k_AUI_TEXTBASE_LDL_FONTSIZE );
	else if ( block->GetAttributeType( k_AUI_TEXTBASE_LDL_FONTSIZEDEPENDENT )
			== ATTRIBUTE_TYPE_STRING)
		fontsize = (uint8)aui_Ldl::GetIntDependent(
			block->GetString( k_AUI_TEXTBASE_LDL_FONTSIZEDEPENDENT ) );

	COLORREF color = k_AUI_TEXTBASE_DEFAULT_COLOR;
	if ( (block->GetAttributeType( k_AUI_TEXTBASE_LDL_RED )
			== ATTRIBUTE_TYPE_INT)
	||   (block->GetAttributeType( k_AUI_TEXTBASE_LDL_GREEN )
			== ATTRIBUTE_TYPE_INT)
	||   (block->GetAttributeType( k_AUI_TEXTBASE_LDL_BLUE )
			== ATTRIBUTE_TYPE_INT) )
	{
		uint8 red = block->GetInt( k_AUI_TEXTBASE_LDL_RED );
		uint8 green = block->GetInt( k_AUI_TEXTBASE_LDL_GREEN );
		uint8 blue = block->GetInt( k_AUI_TEXTBASE_LDL_BLUE );
		color = RGB( red, green, blue );
	}

	COLORREF shadowcolor = k_AUI_TEXTBASE_DEFAULT_SHADOWCOLOR;
	if ( (block->GetAttributeType( k_AUI_TEXTBASE_LDL_SHADOWRED )
			== ATTRIBUTE_TYPE_INT)
	||   (block->GetAttributeType( k_AUI_TEXTBASE_LDL_SHADOWGREEN )
			== ATTRIBUTE_TYPE_INT)
	||   (block->GetAttributeType( k_AUI_TEXTBASE_LDL_SHADOWBLUE )
			== ATTRIBUTE_TYPE_INT) )
	{
		uint8 red = block->GetInt( k_AUI_TEXTBASE_LDL_SHADOWRED );
		uint8 green = block->GetInt( k_AUI_TEXTBASE_LDL_SHADOWGREEN );
		uint8 blue = block->GetInt( k_AUI_TEXTBASE_LDL_SHADOWBLUE );
		shadowcolor = RGB( red, green, blue );
	}

	MBCHAR * fontname = block->GetString( k_AUI_TEXTBASE_LDL_FONTNAME );
	if ( !fontname )
		fontname = k_AUI_TEXTBASE_DEFAULT_FONTNAME;

	
	uint32 flags = k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER |
		k_AUI_BITMAPFONT_DRAWFLAG_VERTCENTER;
	MBCHAR *type = block->GetString( k_AUI_TEXTBASE_LDL_BLTTYPE );
	if ( type )
	{
		if ( !stricmp( type, k_AUI_TEXTBASE_LDL_FILL ) )
			flags = k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT |
				k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP;
		else if ( !stricmp( type, k_AUI_TEXTBASE_LDL_LEFT ) )
			flags = k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT |
				k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP;
		else if ( !stricmp( type, k_AUI_TEXTBASE_LDL_CENTER ) )
			flags = k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER |
				k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP;
		else if ( !stricmp( type, k_AUI_TEXTBASE_LDL_RIGHT ) )
			flags = k_AUI_BITMAPFONT_DRAWFLAG_JUSTRIGHT |
				k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP;
		else
			flags = k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT |
				k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP;
	}

	type = block->GetString( k_AUI_TEXTBASE_LDL_JUST );
	BOOL vertcenter = block->GetBool( k_AUI_TEXTBASE_LDL_VERTCENTER );
	BOOL wordwrap = block->GetBool( k_AUI_TEXTBASE_LDL_WORDWRAP );
	if ( type || vertcenter || wordwrap )
	{
		
		flags = 0;

		if ( type )
		{
			if ( !stricmp( type, k_AUI_TEXTBASE_LDL_JUSTRIGHT ) )
				flags |= k_AUI_BITMAPFONT_DRAWFLAG_JUSTRIGHT;
			else if ( !stricmp( type, k_AUI_TEXTBASE_LDL_JUSTCENTER ) )
				flags |= k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER;
			else if ( !stricmp( type, k_AUI_TEXTBASE_LDL_JUSTFULL ) )
				flags |= k_AUI_BITMAPFONT_DRAWFLAG_JUSTFULL;
			else if ( !stricmp( type, k_AUI_TEXTBASE_LDL_JUSTALL ) )
				flags |= k_AUI_BITMAPFONT_DRAWFLAG_JUSTALL;
			else
				flags |= k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT;
		}

		if ( vertcenter )
			flags |= k_AUI_BITMAPFONT_DRAWFLAG_VERTCENTER;

		if ( wordwrap )
			flags |= k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP;
	}

	if ( block->GetBool(k_AUI_TEXTBASE_LDL_NODATABASE) || !block->GetString(k_AUI_TEXTBASE_LDL_TEXT) ) {
		InitCommon(
			block->GetString( text ? text : k_AUI_TEXTBASE_LDL_TEXT ),
			block->GetInt( k_AUI_TEXTBASE_LDL_MAXLENGTH ), 
			fontname, 
			fontsize, 
			color,
			shadowcolor,
			block->GetInt( k_AUI_TEXTBASE_LDL_BOLD ),
			block->GetInt( k_AUI_TEXTBASE_LDL_ITALIC ),
			block->GetInt( k_AUI_TEXTBASE_LDL_UNDERLINE ),
			block->GetBool( k_AUI_TEXTBASE_LDL_SHADOW ),
			flags );
	}
	else {
		InitCommon(
			g_theStringDB->GetNameStr( block->GetString(k_AUI_TEXTBASE_LDL_TEXT) ),
			block->GetInt( k_AUI_TEXTBASE_LDL_MAXLENGTH ), 
			fontname, 
			fontsize, 
			color,
			shadowcolor,
			block->GetInt( k_AUI_TEXTBASE_LDL_BOLD ),
			block->GetInt( k_AUI_TEXTBASE_LDL_ITALIC ),
			block->GetInt( k_AUI_TEXTBASE_LDL_UNDERLINE ),
			block->GetBool( k_AUI_TEXTBASE_LDL_SHADOW ),
			flags );
	}


	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_TextBase::InitCommon(
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
	uint32 flags )
{
	m_text = NULL,
	m_maxLength = maxLength ? maxLength : k_AUI_TEXTBASE_DEFAULTMAXLENGTH,
	m_curLength = 0;

	m_textfont = NULL;
	m_textflags = flags;

	m_textshadow = shadow;
	m_textshadowcolor = shadowcolor;

	m_textcolor = color;
	m_textunderline = underline;

	m_textreload = TRUE;
	memset( m_textttffile, 0, sizeof( m_textttffile ) );
	strncpy( m_textttffile, fontname, MAX_PATH );
	m_textpointsize = fontsize;
	m_textbold = bold;
	m_textitalic = italic;

	if ( text )
	{
		m_curLength = strlen( text );

		if ( m_curLength > m_maxLength )
			m_curLength = m_maxLength;

		
		m_text = new MBCHAR[ m_maxLength + 1 ];
		Assert( m_text != NULL );
		if ( !m_text ) return AUI_ERRCODE_MEMALLOCFAILED;

		memset( m_text, '\0', m_maxLength + 1 );

		
		strncpy( m_text, text, m_maxLength );
	}
	else
	{
		if ( m_maxLength )
		{
			
			m_text = new MBCHAR[ m_maxLength + 1 ];
			Assert( m_text != NULL );
			if ( !m_text ) return AUI_ERRCODE_MEMALLOCFAILED;

			memset( m_text, '\0', m_maxLength + 1 );
		}
	}

	return AUI_ERRCODE_OK;
}



aui_TextBase::~aui_TextBase()
{
	delete [] m_text;

	if (m_textfont && g_ui)
	{
		g_ui->UnloadBitmapFont(m_textfont);
	}
}



AUI_ERRCODE aui_TextBase::SetText(
	const MBCHAR *text,
	uint32 maxlen )
{
	Assert( text != NULL );
	if ( !text ) return AUI_ERRCODE_INVALIDPARAM;

	
	memset( m_text, '\0', m_maxLength + 1 );

	
	if ( maxlen > m_maxLength ) maxlen = m_maxLength;
	strncpy( m_text, text, maxlen );

	m_curLength = strlen( m_text );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE	aui_TextBase::SetText2(MBCHAR *fmt,...)
{
	
	Assert(fmt != NULL );
	if ( !fmt ) 
		return AUI_ERRCODE_INVALIDPARAM;

	
   	va_list          v_args;

	MBCHAR			 buff[256];

	
	buff[255]='\0';

	
    va_start(v_args, fmt);    
    _vsnprintf(buff,255,fmt,v_args);
    va_end( v_args );         

	
	SetText(buff);

	return AUI_ERRCODE_OK;
}





AUI_ERRCODE aui_TextBase::AppendText(MBCHAR const * text)
{
	Assert( text != NULL );
	if ( !text ) return AUI_ERRCODE_INVALIDPARAM;

	
	Assert( m_curLength + strlen( text ) <= m_maxLength );
	strncat( m_text, text, m_maxLength - m_curLength );

	m_curLength = strlen( m_text );

	return AUI_ERRCODE_OK;
}



void aui_TextBase::SetTextFont(MBCHAR const * ttffile)
{
	if ( !ttffile ) return;
	strncpy( m_textttffile, ttffile, MAX_PATH );
	m_textreload = TRUE;
}

	

void aui_TextBase::SetTextFontSize( sint32 pointSize )
{
	m_textpointsize = pointSize;
	m_textreload = TRUE;
}



void aui_TextBase::SetTextBold( sint32 bold )
{
	m_textbold = bold;
	m_textreload = TRUE;
}



void aui_TextBase::SetTextItalic( sint32 italic )
{
	m_textitalic = italic;
	m_textreload = TRUE;
}



void aui_TextBase::TextReloadFont( void )
{
	if ( !m_textreload ) return;

	aui_BitmapFont *oldFont = m_textfont;

	static MBCHAR descriptor[ k_AUI_BITMAPFONT_MAXDESCLEN + 1 ];
	aui_BitmapFont::AttributesToDescriptor(
		descriptor,
		m_textttffile,
		m_textpointsize,
		m_textbold,
		m_textitalic );

	m_textfont = g_ui->LoadBitmapFont( descriptor );
	Assert( m_textfont != NULL );
	if (m_textfont)
	{
		if (oldFont)
		{
			g_ui->UnloadBitmapFont(oldFont);
		}
		m_textreload = FALSE;
	}
	else
	{
		g_ui->UnloadBitmapFont(descriptor);
		m_textfont = oldFont;
	}
}



AUI_ERRCODE aui_TextBase::DrawThisText(
	aui_Surface *destSurf,
	RECT *destRect )
{
	
	if ( !m_text ) return AUI_ERRCODE_OK;

	
	if ( m_textreload ) TextReloadFont();

	if ( m_textshadow )
	{
		RECT shadowRect = *destRect;
		OffsetRect( &shadowRect, 1, 1 );
		AUI_ERRCODE errcode = m_textfont->DrawString(
			destSurf,
			&shadowRect,
			NULL,
			m_text,
			m_textflags,
			m_textshadowcolor,
			m_textunderline );
		Assert( AUI_SUCCESS(errcode) );
		if ( !AUI_SUCCESS(errcode) ) return errcode;
	}

	return m_textfont->DrawString(
		destSurf,
		destRect,
		NULL,
		m_text,
		m_textflags,
		m_textcolor,
		m_textunderline );
}




uint32 aui_TextBase::FindNextWordBreak
(
	MBCHAR const *  text, 
    HDC             hdc, 
    sint32          width 
)
{
	Assert(text);
	if ( !text ) return 0;

	uint32 totalLength = 0;	
	sint32 totalSize = 0;	

	MBCHAR const *  word = text;
	while (MBCHAR const * token = FindNextToken(word, " \t\n", 1 ))
	{
		SIZE wordSize = { 0, 0 };				
		sint32 wordLength = token - word + 1;	

		
		BOOL success = GetTextExtentPoint32( hdc, word, wordLength, &wordSize );
		Assert( success );
		if ( !success ) return totalLength + wordLength;

		if ( (totalSize += wordSize.cx) < width )
			totalLength += wordLength;
		else
			return totalLength;

		if ( *token == '\n' )
			return totalLength;

		word = token + 1;
	}

	
	SIZE wordSize = { 0, 0 };
	sint32 wordLength = strlen( word );

	
	BOOL success = GetTextExtentPoint32( hdc, word, wordLength, &wordSize );
	Assert( success );
	if ( !success ) return totalLength + wordLength;

	if ( (totalSize += wordSize.cx) < width )
		totalLength += wordLength;

	return totalLength;
}



MBCHAR const * aui_TextBase::FindNextToken
(
	MBCHAR const *  text, 
    MBCHAR *        tokenList, 
    sint32          count 
)
{
	Assert(text && tokenList);
	if (!text || !tokenList) return NULL;

	MBCHAR const *  tokenPtr    = NULL;
	MBCHAR *        charPtr     = tokenList;

	for (size_t i = strlen(tokenList); i; i--)
	{
		MBCHAR const *  newTokenPtr = strchr(text, *charPtr++);
		if (newTokenPtr)
		{
			if ( tokenPtr )
			{
				if ( newTokenPtr < tokenPtr ) tokenPtr = newTokenPtr;
			}
			else
			{
				tokenPtr = newTokenPtr;
			}
		}
	}

	if ( count > 1 && tokenPtr )
	{
		MBCHAR const *  nextTokenPtr = FindNextToken(tokenPtr + 1, tokenList, --count);
		return nextTokenPtr ? nextTokenPtr : tokenPtr;
	}
	else
		return tokenPtr;
}
