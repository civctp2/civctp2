//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The civilization 3 slider
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

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

#include "c3slider.h"

#include "SlicEngine.h"

extern C3UI			*g_c3ui;
extern SlicEngine	*g_slicEngine;

#include "gamesounds.h"
#include "soundmanager.h"

extern SoundManager		*g_soundManager;


C3Slider::C3Slider(
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


C3Slider::C3Slider(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	BOOL isVertical,
	MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_Ranger()
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


AUI_ERRCODE C3Slider::InitCommon( MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	m_ticks = block->GetInt( k_C3SLIDER_LDL_TICKS );

	return InitCommon();
}


AUI_ERRCODE C3Slider::InitCommon( void )
{
	return AUI_ERRCODE_OK;
}


AUI_ERRCODE C3Slider::CreateThumb( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

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
				C3SliderThumbActionCallback,
				this );
	}

	if ( !m_thumb )
		m_thumb = new C3Thumb(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0,
			m_pattern->GetFilename(),
			C3SliderThumbActionCallback,
			this );

	Assert( AUI_NEWOK(m_thumb,errcode) );
	if ( !AUI_NEWOK(m_thumb,errcode) )
		return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_thumb );

	RepositionThumb( FALSE );

	return AUI_ERRCODE_OK;
}

void C3Slider::MouseRGrabInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !m_thumb ) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{

		return;
	}
	else
		MouseRGrabOutside( mouseData );
}




AUI_ERRCODE C3Slider::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
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

	if ( m_pattern ) {
		if ( m_srcWidthPix || m_srcHeightPix ) {
			RECT srcRect = { m_srcX, m_srcY, m_srcX + m_srcWidthPix, m_srcY + m_srcHeightPix };
			m_pattern->Draw( surface, &dirtyRect , &srcRect );
		}
		else {
			m_pattern->Draw( surface, &dirtyRect  );
		}
	}


	if ( m_ticks && m_quantized )
	{
		if ( m_orientation == AUI_RANGER_ORIENTATION_VERTICAL )
		{
			sint32 spacing = m_height / ( m_maxY - m_minY ) - 1;
			RECT tickRect;
			for ( sint32 i = m_maxY - m_minY; i >= 0; i -= m_ticks )
			{
				tickRect.top =
					i * spacing + dirtyRect.top + m_minThumbSize / 2;
				tickRect.left = dirtyRect.left + 6;
				tickRect.bottom = tickRect.top + 3;
				tickRect.right = tickRect.left + 6;
				primitives_BevelRect16( surface, &tickRect, 1, 1, 16, 16 );
				tickRect.left = dirtyRect.right - 12;
				tickRect.right = dirtyRect.right - 6;
				primitives_BevelRect16( surface, &tickRect, 1, 1, 16, 16 );
			}
		}
		else
		{
			sint32 spacing = m_width / ( m_maxX - m_minX ) - 1;
			RECT tickRect;
			for ( sint32 i = m_maxX - m_minX; i >= 0; i -= m_ticks )
			{
				tickRect.left =
					i * spacing + dirtyRect.left + m_minThumbSize / 2;
				tickRect.top = dirtyRect.top + 6;
				tickRect.right = tickRect.left + 3;
				tickRect.bottom = tickRect.top + 6;
				primitives_BevelRect16( surface, &tickRect, 1, 1, 16, 16 );
				tickRect.top = dirtyRect.bottom - 12;
				tickRect.bottom = dirtyRect.bottom - 6;
				primitives_BevelRect16( surface, &tickRect, 1, 1, 16, 16 );
			}
		}
	}

	RECT origRect = rect;
	primitives_BevelRect16( surface, &origRect, 2, 0, 16, 16 );
	InflateRect( &rect, -4, -4 );
	primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	if ( IsActive() )
	{

		primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	}

	InflateRect( &rect, -1, -1 );
	primitives_PaintRect16( surface, &rect, 0x0000 );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &dirtyRect );

	return AUI_ERRCODE_OK;
}

void C3SliderThumbActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	aui_Ranger *ranger = (aui_Ranger *)cookie;

	sint32		sound = -1;
	static aui_Ranger *oldRanger = 0;
	static sint32 val = 0;

	if (oldRanger != ranger) {
		oldRanger = ranger;
		val = 0;
	}

	switch ( action )
	{
	case AUI_THUMB_ACTION_DRAG:
		ranger->ThumbMoved();

		if (val != ranger->GetValueX()) {
			sound = GAMESOUNDS_SLIDERUP;
			val = ranger->GetValueX();
		}

		break;

	case AUI_THUMB_ACTION_DROP:
		ranger->ThumbMoved( TRUE );
		sound = GAMESOUNDS_SLIDERDOWN;
		if(ranger->GetActionFunc()) {
			ranger->GetActionFunc()(ranger, AUI_RANGER_ACTION_RELEASE, 0, ranger->GetCookie());
		}
		break;
	}

	if (sound != -1)
		if (g_soundManager) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, 0,
					gamesounds_GetGameSoundID(sound), 0, 0);
		}
}


void C3SliderButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_PRESS ) return;

	aui_Ranger *ranger = (aui_Ranger *)cookie;
	aui_Button *button = (aui_Button *)control;

	sint32		sound = -1;

	if ( button == ranger->GetIncrementXButton() ) {
		ranger->IncrementUpX();
		sound = GAMESOUNDS_SLIDERUP;
	} else if ( button == ranger->GetIncrementYButton() ) {
 		ranger->IncrementUpY();
		sound = GAMESOUNDS_SLIDERUP;
	} else if ( button == ranger->GetDecrementXButton() ) {
		ranger->IncrementDownX();
		sound = GAMESOUNDS_SLIDERDOWN;
	} else {
		ranger->IncrementDownY();
		sound = GAMESOUNDS_SLIDERDOWN;
	}

	if (sound != -1)
	if (g_soundManager) {
		g_soundManager->AddSound(SOUNDTYPE_SFX, 0,
				gamesounds_GetGameSoundID(sound), 0, 0);
	}
	return;

}
