

#include "c3.h"

#include "aui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_surface.h"
#include "aui_window.h"
#include "aui_ranger.h"
#include "aui_ldl.h"
#include "aui_action.h"

#include "c3thumb.h"
#include "c3ui.h"
#include "textbutton.h"
#include "pattern.h"
#include "c3ui.h"

#include "primitives.h"
#include "colorset.h"

#include "c3_slidometer.h"

#include "SlicEngine.h"

extern C3UI			*g_c3ui;
extern SlicEngine	*g_slicEngine;
extern ColorSet		*g_colorSet;



c3_Slidometer::c3_Slidometer(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_Ranger()
{
	*retval = aui_Region::InitCommonLdl( id, ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommonLdl( ldlBlock, ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Ranger::InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommonLdl( ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommonLdl( ldlBlock, (MBCHAR *)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;


	*retval = InitCommon( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateThumb( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



c3_Slidometer::c3_Slidometer(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	BOOL isVertical,
	MBCHAR *pattern,
	sint32 percentFilled,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_Ranger(),
	m_percentFilled(percentFilled)
{
	*retval = aui_Region::InitCommon( id, x, y, width, height );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommon( ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Ranger::InitCommon(
		AUI_RANGER_TYPE_SLIDER,
		isVertical ?
			AUI_RANGER_ORIENTATION_VERTICAL :
			AUI_RANGER_ORIENTATION_HORIZONTAL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommon((MBCHAR **)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommon( pattern );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateThumb( NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE c3_Slidometer::InitCommon( MBCHAR *ldlBlock )
{
	
	sint32 percentFilled = 0;

	aui_Ldl *theLdl = g_c3ui->GetLdl();
	
	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	percentFilled = block->GetInt( k_C3_SLIDOMETER_PERCENT_FILLED );

	SetPercentFilled(percentFilled);

	return InitCommon();
}



AUI_ERRCODE c3_Slidometer::InitCommon( void )
{
	return AUI_ERRCODE_OK;
}



AUI_ERRCODE c3_Slidometer::CreateThumb( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode;

	
	aui_Ldl *theLdl = g_c3ui->GetLdl();
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_RANGER_LDL_THUMB );

		
		if ( theLdl->GetLdl()->FindDataBlock( block ) )
			m_thumb = new C3Thumb(
				&errcode,
				aui_UniqueId(),
				block,
				RangerThumbActionCallback,
				this );
	}

	if ( !m_thumb )
		m_thumb = new C3Thumb(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0,
			m_pattern->GetFilename(),
			RangerThumbActionCallback,
			this );

	Assert( AUI_NEWOK(m_thumb,errcode) );
	if ( !AUI_NEWOK(m_thumb,errcode) )
		return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_thumb );

	RepositionThumb( FALSE );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE c3_Slidometer::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	

	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	RECT dirtyRect = rect;

	if ( m_orientation == AUI_RANGER_ORIENTATION_VERTICAL )
		InflateRect( &rect, -m_width / 2 + 8, 0 );
	else
		InflateRect( &rect, 0, -m_height / 2 + 8 );

	
	if ( m_pattern ) m_pattern->Draw( surface, &dirtyRect );





































	RECT origRect = rect;
	primitives_BevelRect16( surface, &origRect, 2, 0, 16, 16 );
	InflateRect( &rect, -4, -4 );
	primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );

	InflateRect( &rect, -1, -1 );
	primitives_PaintRect16( surface, &rect, 0x0000 );

	sint32 width = rect.right - rect.left;
	sint32 fill = rect.left + m_percentFilled * width / 100;
	rect.right = fill;

	COLOR		color;

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

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &dirtyRect );

	return AUI_ERRCODE_OK;
}

void c3_Slidometer::SetPercentFilled( sint32 percentFilled ) 
{
	percentFilled > 100 ? m_percentFilled = 100 : m_percentFilled = percentFilled;
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
}
