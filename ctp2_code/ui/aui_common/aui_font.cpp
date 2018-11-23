#include "c3.h"

#include "aui_font.h"


aui_Font::aui_Font(
	AUI_ERRCODE *retval,
	const MBCHAR *descriptor)
	:
	aui_Base()
{
	assert(0);
	*retval = InitCommon( descriptor );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE aui_Font::InitCommon(const MBCHAR *descriptor)
{
	m_hfont = NULL;

	m_height = 0;
	m_averageWidth = 0;
	m_escapement = 0;
	m_orientation = 0;
	m_weight = 0;
	m_italic = 0;
	m_underline = 0;
	m_strikeout = 0;
	m_charset = 0;
	m_outputPrecision = 0;
	m_clipPrecision = 0;
	m_quality = 0;
	m_pitchAndFamily = 0;

	AUI_ERRCODE errcode = SetDescriptor( descriptor );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	return AUI_ERRCODE_OK;
}


aui_Font::~aui_Font()
{
	Unload();
}


AUI_ERRCODE aui_Font::SetDescriptor(const MBCHAR *descriptor)
{
	memset( m_descriptor, '\0', sizeof( m_descriptor ) );

	if ( !descriptor ) return AUI_ERRCODE_INVALIDPARAM;

	strncpy( m_descriptor, descriptor, MAX_PATH );

	return SetVariablesFromDescriptor();
}


AUI_ERRCODE aui_Font::SetVariablesFromDescriptor( void )
{
	if ( sscanf( m_descriptor,
		"%d.%d.%d.%d.%d.%u.%u.%u.%u.%u.%u.%u.%u.%[^\t\n]",
		&m_height,
		&m_averageWidth,
		&m_escapement,
		&m_orientation,
		&m_weight,
		&m_italic,
		&m_underline,
		&m_strikeout,
		&m_charset,
		&m_outputPrecision,
		&m_clipPrecision,
		&m_quality,
		&m_pitchAndFamily,
		m_fontname ) != 14 ) return AUI_ERRCODE_INVALIDPARAM;

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_Font::Load( void )
{

	Unload();
#ifdef WIN32
	m_hfont = CreateFont(
		m_height,
		m_averageWidth,
		m_escapement,
		m_orientation,
		m_weight,
		m_italic,
		m_underline,
		m_strikeout,
		m_charset,
		m_outputPrecision,
		m_clipPrecision,
		m_quality,
		m_pitchAndFamily,
		m_fontname );
#endif
	Assert( m_hfont != NULL );
	if ( !m_hfont ) return AUI_ERRCODE_LOADFAILED;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_Font::Unload( void )
{
	if ( m_hfont )
	{
#ifdef WIN32
		DeleteObject( m_hfont );
		m_hfont = NULL;
#endif
	}

	return AUI_ERRCODE_OK;
}
