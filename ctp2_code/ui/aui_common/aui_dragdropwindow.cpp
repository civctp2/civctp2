









#include "c3.h"
#include "aui_ui.h"
#include "aui_blitter.h"
#include "aui_uniqueid.h"
#include "aui_control.h"

#include "aui_dragdropwindow.h"



aui_DragDropWindow::aui_DragDropWindow(
	AUI_ERRCODE *retval,
	aui_Control *dragDropItem,
	aui_Region *launchPad,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height )
	:
	aui_Window(
		retval,
		aui_UniqueId(),
		x, y, width, height,
		0, AUI_WINDOW_TYPE_FLOATING )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( dragDropItem, launchPad );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_DragDropWindow::InitCommon(
	aui_Control *dragDropItem,
	aui_Region *launchPad )
{
	m_dragDropItem = dragDropItem;
	m_launchPad = launchPad;

	SetDynamic( TRUE );
	SetTransparent( TRUE );

	
	SetDraggable( TRUE );
	m_grabRegion->Move( 0, 0 );
	m_grabRegion->Resize( m_width, m_height );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_DragDropWindow::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_surface;

	RECT rect = { 0, 0, m_width, m_height };

	
	g_ui->TheBlitter()->ColorBlt( surface, &rect, RGB(0,0,0), 0 );

	if ( m_dragDropItem )
	{
		POINT itemPoint = { m_dragDropItem->X(), m_dragDropItem->Y() };
		m_dragDropItem->ToWindow( &itemPoint );

		
		BOOL wasHidden = m_dragDropItem->IsHidden();
		m_dragDropItem->Show();

		(void) m_dragDropItem->Draw(surface, -itemPoint.x, -itemPoint.y);

		if ( wasHidden ) m_dragDropItem->Hide();
	}

	if ( surface == m_surface )
		AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}



void aui_DragDropWindow::StartDragging( sint32 localX, sint32 localY )
{
	if (IsDisabled()) return;

	
	SetWhichSeesMouse( this );

	m_isDragging = TRUE;
	m_grabPoint.x = localX;
	m_grabPoint.y = localY;

	aui_Control *focus = aui_Control::GetKeyboardFocus();
	if ( focus )
		focus->ReleaseKeyboardFocus();
}








void aui_DragDropWindow::MouseLDropOutside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	aui_Window::MouseLDropOutside( mouseData );

	
	SetWhichSeesMouse( NULL );
}
