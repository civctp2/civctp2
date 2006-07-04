





#include "c3.h"

#include "aui.h"
#include "aui_ui.h"
#include "aui_directui.h"
#include "aui_blitter.h"
#include "aui_surface.h"
#include "aui_window.h"
#include "aui_action.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"

#include "c3_static.h"
#include "c3_headerswitch.h"

#include "pattern.h"
#include "primitives.h"

extern aui_UI *g_ui;



c3_HeaderSwitch::c3_HeaderSwitch(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_Radio(
		retval,
		id,
		ldlBlock,
		ActionFunc,
		cookie ),
	m_image( NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommonLdl( ldlBlock, NULL);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



c3_HeaderSwitch::c3_HeaderSwitch(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *text,
	MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie,
	sint32 state,
	sint32 numStates )
	:
	aui_ImageBase( numStates ),
	aui_TextBase( text ),
	aui_Radio(
		retval,
		id,
		x,
		y,
		width,
		height,
		ActionFunc,
		cookie,
		state,
		numStates ),
	m_image( NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommon( pattern );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE c3_HeaderSwitch::InitCommonLdl( MBCHAR *ldlBlock )
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	
	if ( ldlBlock )
	{
		AUI_ERRCODE errcode;
		MBCHAR imageBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
		sprintf(imageBlock, "%s.%s", ldlBlock, k_C3_HEADERSWITCH_IMAGE );

        if (aui_Ldl::FindDataBlock(imageBlock))
		{
			m_image = new c3_Static(
				&errcode,
				aui_UniqueId(),
				imageBlock );
			
			m_image->SetBlindness( TRUE );
		}
	}

	AUI_ERRCODE errcode = InitCommon();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE c3_HeaderSwitch::InitCommon(void)
{
	
	SetNumStates( k_C3_HEADERSWITCH_DEFAULTNUMSTATES );

	return AUI_ERRCODE_OK;
}



c3_HeaderSwitch::~c3_HeaderSwitch()
{
	if ( m_image ) {
		delete m_image;
		m_image = NULL;
	}
}


AUI_ERRCODE c3_HeaderSwitch::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{

	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	if ( m_pattern ) 
		m_pattern->Draw( surface, &rect );

	
	
	aui_Radio::DrawThis( surface, x, y );

	if ( IsOn() )
		primitives_BevelRect16( surface, &rect, 2, 1, 16, 16 );
	else
		primitives_BevelRect16( surface, &rect, 2, 0, 16, 16 );

	
	

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
