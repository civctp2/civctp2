









#include "c3.h"

#include "aui.h"
#include "aui_control.h"
#include "aui_uniqueid.h"
#include "aui_blitter.h"

#include "pattern.h"
#include "primitives.h"


#include "messageiconwindow.h"
#include "messagewindow.h"

#include "test.h"
#include "testwindow.h"

#include "c3ui.h"

#include "iconbutton.h"


extern C3UI				*g_c3ui;



UITestWindow::UITestWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		AUI_WINDOW_TYPE type )
		:
		C3Window( retval, id, x, y, width, height, bpp, pattern, type )
{
	Assert( AUI_SUCCESS( *retval ) );
	if ( !AUI_SUCCESS( *retval ) ) return;

	*retval = InitCommon( );
	Assert( AUI_SUCCESS( *retval ) );
	if ( !AUI_SUCCESS( *retval ) ) return;

}

AUI_ERRCODE UITestWindow::InitCommon( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	SetDraggable(TRUE);

	m_button1 = new IconButton( &errcode, aui_UniqueId(),
								20,
								37,
								32,
								32,
								"upba0104.tif", 
								k_MESSAGE_ICON_EVENT,
								0 );
	Assert( AUI_NEWOK( m_button1, errcode ));
	if ( !AUI_NEWOK( m_button1, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	errcode = AddControl( m_button1 );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	m_button1Action = new Button1Action( );
	Assert( m_button1Action != NULL );
	if ( m_button1Action == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_button1->SetAction( m_button1Action );
	
	m_button2 = new IconButton( &errcode, aui_UniqueId(),
								54,
								37,
								32,
								32,
								"upba0104.tif", 
								k_MESSAGE_ICON_DIPLOMATIC,
								0 );
	Assert( AUI_NEWOK( m_button2, errcode ));
	if ( !AUI_NEWOK( m_button2, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	errcode = AddControl( m_button2 );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	m_button2Action = new Button2Action( );
	Assert( m_button2Action != NULL );
	if ( m_button2Action == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_button2->SetAction( m_button2Action );
	
	return AUI_ERRCODE_OK;
}


AUI_ERRCODE UITestWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_surface;

	RECT rectInside = { 3, 26, (m_width - 4), (m_height - 4)};
	RECT rectTitle = { 4, 4, (m_width - 4), 24 };
	RECT rect = { 0, 0, m_width, m_height };

	
	g_c3ui->TheBlitter()->ColorBlt(
		surface,
		&rect,
		RGB(191,191,191),
		0 );











	
	g_c3ui->TheBlitter()->ColorBlt(
		surface,
		&rectTitle,
		RGB(0,0,142),
		0 );

	
	g_c3ui->TheBlitter()->ColorBlt(
		surface,
		&rectInside,
		RGB(255,255,255),
		0 );
	










	AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;

}

UITestWindow::~UITestWindow( )
{
	if ( m_button1 ) {
		delete m_button1;
		m_button1 = NULL;
	}

	if ( m_button1Action ) {
		delete m_button1Action;
		m_button1Action = NULL;
	}

	if ( m_button2 ) {
		delete m_button2;
		m_button2 = NULL;
	}

	if ( m_button2Action ) {
		delete m_button2Action;
		m_button2Action = NULL;
	}

}
