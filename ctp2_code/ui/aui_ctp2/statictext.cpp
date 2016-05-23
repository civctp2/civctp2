#include "c3.h"

#include "aui.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "c3ui.h"

#include "primitives.h"

#include "statictext.h"
#include "textutils.h"

#include "colorset.h"

extern ColorSet	*g_colorSet;
extern C3UI		*g_c3ui;

StaticText::StaticText(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock)
:
	aui_Static( retval, id, ldlBlock ),
	TextBase( ldlBlock, (MBCHAR *)NULL )
{
	m_border = 0;

	*retval = InitCommon( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

StaticText::StaticText(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *text,
	uint32 maxLength,
	uint32 size)
:
	aui_Static( retval, id, x, y, width, height ),
	TextBase( text, maxLength )
{
	m_border = 0;

	if (size) m_size = size;
}

AUI_ERRCODE StaticText::InitCommon( MBCHAR *ldlBlock )
{
	uint32 size = 0;

	aui_Ldl *theLdl = g_c3ui->GetLdl();

	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	size = block->GetInt( k_STATICTEXT_LDL_TEXTSIZE );

	if (size) m_size = size;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE StaticText::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{




	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if (m_border)
		primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );

	rect.left += 2;

	textutils_SizedBoundedString((aui_DirectSurface *)surface, m_text, &rect, m_size, COLOR_BUTTON_TEXT_PLAIN, 0);











	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
