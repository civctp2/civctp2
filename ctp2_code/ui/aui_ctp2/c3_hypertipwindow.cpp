










#include "c3.h"
#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_static.h"
#include "aui_blitter.h"
#include "c3_hypertipwindow.h"
#include "aui_hypertextbox.h"

#include "pattern.h"
#include "pixelutils.h"
#include "primitives.h"
#include "colorset.h"               // g_colorSet


c3_HyperTipWindow::c3_HyperTipWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock )
	:
	aui_TipWindow( retval, id, ldlBlock ),
	PatternBase( ldlBlock, NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



c3_HyperTipWindow::c3_HyperTipWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR	*pattern )
	:
	aui_TipWindow( retval, id, x, y, width, height ),
	PatternBase(pattern)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE c3_HyperTipWindow::InitCommonLdl( MBCHAR *ldlBlock )
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	AUI_ERRCODE errcode = InitCommon();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	
	MBCHAR tipBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( tipBlock, "%s.%s", ldlBlock, k_C3_HYPERTIPWINDOW_LDL_TIP );

    if (aui_Ldl::GetLdl()->FindDataBlock( tipBlock ) )
	{
		m_hyperTip = new aui_HyperTextBox(
			&errcode,
			aui_UniqueId(),
			tipBlock );
		Assert( AUI_NEWOK(m_hyperTip,errcode) );
		if ( !AUI_NEWOK(m_hyperTip,errcode) ) return errcode;

		m_hyperTip->SetBlindness( TRUE );

		m_hyperTip->TextReloadFont();

		
		aui_Ldl::Remove( m_hyperTip );
		m_allocatedHyperTip = TRUE;
		AddChild( m_hyperTip );
		m_hyperTip->Move( 0, 0 );
		m_hyperTip->Resize( m_width-5, m_height-10 );
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE c3_HyperTipWindow::SetHyperTipText(MBCHAR *text)
{
	m_hyperTip->SetHyperText(text);

	aui_BitmapFont	*font = m_hyperTip->GetTextFont();

	sint32			width   = 10 + font->GetStringWidth(text);
	sint32			height  = 2 + font->GetMaxHeight();

	Resize(width, height);

	m_hyperTip->Resize(m_width - 2, m_height);
	m_hyperTip->Move( 2, m_hyperTip->Y() );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE c3_HyperTipWindow::InitCommon( void )
{
	m_allocatedHyperTip = FALSE;
	m_hyperTip = NULL;

	SetDynamic( TRUE );

	return AUI_ERRCODE_OK;
}



c3_HyperTipWindow::~c3_HyperTipWindow()
{
	if ( m_allocatedHyperTip)
	{
		delete m_hyperTip;
	}
}

AUI_ERRCODE c3_HyperTipWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_surface;

	RECT rect = { 0, 0, m_width, m_height };

	g_ui->TheBlitter()->ColorBlt( surface, &rect, RGB(0,0,0), 0 );

	primitives_FrameRect16(surface, &rect, g_colorSet->GetColor(COLOR_GREEN));

	if ( surface == m_surface )
		AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
