











#include "c3.h"

#include "aui.h"
#include "aui_window.h"
#include "primitives.h"
#include "aui_ldl.h"
#include "c3ui.h"

#include "thermometer.h"

#include "colorset.h"       // g_colorSet

extern C3UI			*g_c3ui;


Thermometer::Thermometer(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	sint32 percentFilled,
	ControlActionCallback *ActionFunc,
	void *cookie )
:
	aui_ImageBase((sint32)0),
	aui_TextBase((MBCHAR *)NULL),
	aui_Control( retval, id, x, y, width, height, ActionFunc, cookie ),
	PatternBase(pattern),
	m_percentFilled(percentFilled)
{
}


Thermometer::Thermometer(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL),
	aui_Control( retval, id, ldlBlock, ActionFunc, cookie ),
	PatternBase(ldlBlock, NULL)
{
	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

AUI_ERRCODE Thermometer::InitCommonLdl( MBCHAR *ldlBlock )
{
	
	sint32 percentFilled = 0;

	aui_Ldl *theLdl = g_c3ui->GetLdl();
	
	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	percentFilled = block->GetInt( k_THERMOMETER_PERCENT_FILLED );

	SetPercentFilled(percentFilled);

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE Thermometer::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	

	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );
	RECT mainRect = rect;

	
	if ( m_pattern ) {
		if ( m_srcWidthPix || m_srcHeightPix ) {
			RECT srcRect = { m_srcX, m_srcY, m_srcX + m_srcWidthPix, m_srcY + m_srcHeightPix };
			m_pattern->Draw( surface, &rect, &srcRect );
		}
		else {
			m_pattern->Draw( surface, &rect );
		}
	}

	primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	InflateRect( &rect, -1, -1 );

	sint32 width = rect.right - rect.left;
	sint32 fill = rect.left + m_percentFilled * width / 100;
	rect.right = fill;

	COLOR		color;

    Assert(m_percentFilled >= 0);

    if (m_percentFilled < 0)
        m_percentFilled=0; 
    Assert(m_percentFilled <= 100);

    if (100 < m_percentFilled)
        m_percentFilled=100; 

	if (m_percentFilled < 25) {
		color = COLOR_RED;
	} else
		if (m_percentFilled < 50) {
			color = COLOR_ORANGE;
		} else
			if (m_percentFilled < 75) {
				color = COLOR_YELLOW;
			} else {
				color = COLOR_GREEN;
			}

	if (rect.right > rect.left)
		primitives_PaintRect16( surface, &rect, g_colorSet->GetColor(color) );

	
	DrawThisText(
		surface,
		&mainRect );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &mainRect );

	return AUI_ERRCODE_OK;
}

void Thermometer::SetPercentFilled( sint32 percentFilled ) 
{
	percentFilled > 100 ? m_percentFilled = 100 : m_percentFilled = percentFilled;
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
}
