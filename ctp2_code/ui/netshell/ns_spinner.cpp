


#include "c3.h"
#include "aui_window.h"
#include "aui_surface.h"

#include "pattern.h"
#include "primitives.h"

#include "ns_spinner.h"



ns_Spinner::ns_Spinner(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_Ranger( retval, id, ldlBlock, ActionFunc, cookie ),
	PatternBase( ldlBlock, NULL ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_ImageBase( ldlBlock )
{
	
}



ns_Spinner::ns_Spinner(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	AUI_RANGER_ORIENTATION orientation,
	MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_Ranger( retval, id, x, y, width, height,
			   AUI_RANGER_TYPE_SPINNER, orientation,
				ActionFunc, cookie),
	PatternBase( pattern ),
	aui_TextBase( NULL ),
	aui_ImageBase( (sint32)0 )
{
}



AUI_ERRCODE ns_Spinner::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( m_pattern ) m_pattern->Draw( surface, &rect );

	primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );

	if ( m_orientation == AUI_RANGER_ORIENTATION_HORIZONTAL )
		sprintf( m_text, "%d", m_valX );
	else
		sprintf( m_text, "%d", m_valY );

	DrawThisText(
		surface,
		&rect );

	
	
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

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
