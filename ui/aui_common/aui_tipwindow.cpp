










#include "c3.h"
#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_static.h"
#include "aui_blitter.h"
#include "aui_tipwindow.h"

#include "pixelutils.h"
#include "primitives.h"
#include "ColorSet.h"
extern ColorSet *g_colorSet;


aui_TipWindow::aui_TipWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock )
	:
	aui_Window( retval, id, ldlBlock, 0, AUI_WINDOW_TYPE_TIP )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_TipWindow::aui_TipWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height )
	:
	aui_Window( retval, id, x, y, width, height, 0, AUI_WINDOW_TYPE_TIP )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_TipWindow::InitCommonLdl( MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	AUI_ERRCODE errcode = InitCommon();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	
	MBCHAR tipBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( tipBlock, "%s.%s", ldlBlock, k_AUI_TIPWINDOW_LDL_TIP );

	

	if ( theLdl->GetLdl()->FindDataBlock( tipBlock ) )
	{
		m_staticTip = new aui_Static(
			&errcode,
			aui_UniqueId(),
			tipBlock );
		Assert( AUI_NEWOK(m_staticTip,errcode) );
		if ( !AUI_NEWOK(m_staticTip,errcode) ) return errcode;

		m_staticTip->SetBlindness( TRUE );

		m_staticTip->TextReloadFont();

		
		aui_Ldl::Remove( m_staticTip );
		m_allocatedTip = TRUE;
		AddChild( m_staticTip );
		m_staticTip->Move( 0, 0 );
		m_staticTip->Resize( m_width, m_height );
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_TipWindow::SetTipText(MBCHAR *text)
{
	m_staticTip->SetText(text);
	aui_BitmapFont	*font = m_staticTip->GetTextFont();

	sint32			width = font->GetStringWidth(text);
	sint32			height = font->GetMaxHeight();
	
	width += 10;
	height += 2;
	
	Resize(width, height);

	m_staticTip->Resize(m_width, m_height);

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_TipWindow::InitCommon( void )
{
	m_allocatedTip = FALSE;
	m_staticTip = NULL;

	SetDynamic( TRUE );

	return AUI_ERRCODE_OK;
}



aui_TipWindow::~aui_TipWindow()
{
	if ( m_allocatedTip && m_staticTip )
	{
		delete m_staticTip;
		m_staticTip = NULL;
		m_allocatedTip = FALSE;
	}
}

AUI_ERRCODE aui_TipWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_surface;

	RECT rect = { 0, 0, m_width, m_height };

	g_ui->TheBlitter()->ColorBlt( surface, &rect, RGB(0,0,0), 0 );

	primitives_FrameRect16(surface, &rect, g_colorSet->GetColor(COLOR_GRAY));

	if ( surface == m_surface )
		AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
