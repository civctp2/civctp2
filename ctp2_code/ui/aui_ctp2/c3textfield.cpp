#include "c3.h"
#include "c3textfield.h"

#include "aui.h"
#include "aui_window.h"
#include "aui_surface.h"
#include "aui_ldl.h"

#include "pattern.h"

#include "primitives.h"
#include "c3ui.h"

#include "ldl_data.hpp"

extern C3UI			*g_c3ui;

C3TextField::C3TextField(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_TextField( retval, id, ldlBlock, ActionFunc, cookie ),
	PatternBase(ldlBlock, NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
}

C3TextField::C3TextField(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	MBCHAR *text,
	ControlActionCallback *ActionFunc,
	void *cookie )
:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase(NULL),
	aui_TextField( retval, id, x + 2, y + 2, width - 4, height - 4, text, ActionFunc, cookie ),
	PatternBase( pattern )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon(k_C3_TEXTFIELD_DEFAULT_BEVELWIDTH);
	Assert( AUI_SUCCESS(*retval) );
}

AUI_ERRCODE C3TextField::InitCommonLdl( MBCHAR *ldlBlock )
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	sint32	bevelWidth = k_C3_TEXTFIELD_DEFAULT_BEVELWIDTH;

	if (block->GetAttributeType( k_C3_TEXTFIELD_LDL_BEVELWIDTH) == ATTRIBUTE_TYPE_INT) {
		bevelWidth = block->GetInt( k_C3_TEXTFIELD_LDL_BEVELWIDTH );
	}

	return InitCommon(bevelWidth);
}


AUI_ERRCODE C3TextField::InitCommon( sint32 bevelWidth )
{
	m_bevelWidth = bevelWidth;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE C3TextField::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width + 4, m_height + 4 };
	OffsetRect( &rect, m_x + x - 2, m_y + y - 2 );
	ToWindow( &rect );

	if ( m_pattern ) m_pattern->Draw( surface, &rect );

	if (m_bevelWidth > 0)
		primitives_BevelRect16( surface, &rect, m_bevelWidth, 1, 16, 16 );








	aui_TextField::DrawThis( surface, x, y );


	if ( IsDisabled() && rect.left < rect.right && rect.top < rect.bottom )
	{

		uint16 *pixel;

		AUI_ERRCODE errcode = surface->Lock( &rect, (void **)&pixel, 0 );
		Assert( AUI_SUCCESS(errcode) );
		if ( AUI_SUCCESS(errcode) )
		{

			uint16 *origPixel = pixel;

			const sint32 pitch = surface->Pitch() / 2;
			const sint32 width = rect.right - rect.left;
			const sint32 diff = pitch - width;

			uint16 *stopHorizontal = pixel + width;
			const uint16 *stopVertical = pixel +
				pitch * ( rect.bottom - rect.top );

			do
			{
				do
				{
					*pixel = pixelutils_BlendFast(
						*pixel,
						0x0000,
						24 );

				} while ( ++pixel != stopHorizontal );

				stopHorizontal += pitch;
			} while ( (pixel += diff) != stopVertical );

			errcode = surface->Unlock( origPixel );
			Assert( AUI_SUCCESS(errcode) );
		}
	}

	return AUI_ERRCODE_OK;
}
