#include "c3.h"

#include "aui.h"
#include "aui_control.h"
#include "aui_uniqueid.h"
#include "c3window.h"
#include "c3_static.h"
#include "c3_button.h"
#include "c3ui.h"

#include "c3_fancywindow.h"

extern C3UI						*g_c3ui;

#define	k_ORIGINAL_UNIT_STATUS_WIDTH	538
#define k_ORIGINAL_UNIT_STATUS_HEIGHT	479

#define k_C3_FANCYWINDOW_CANCEL_XOFFSET	5
#define k_C3_FANCYWINDOW_CANCEL_YOFFSET	4

#define k_C3_FANCYWINDOW_OK_XOFFSET		3
#define k_C3_FANCYWINDOW_OK_YOFFSET		4


c3_FancyWindow::c3_FancyWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	sint32 bpp,
	MBCHAR ldlBorder[k_NUM_C3_FANCYBORDERS][ k_AUI_LDL_MAXBLOCK + 1 ],
	const MBCHAR *ldlTitle,
	AUI_WINDOW_TYPE type,
	bool bevel,
	void (*exitCallBack)( aui_Control *, uint32, uint32, void *))
	: C3Window(retval, id, ldlBlock, bpp, type, bevel)
{
	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;










	m_originalDimensions.x = k_ORIGINAL_UNIT_STATUS_WIDTH;
	m_originalDimensions.y = k_ORIGINAL_UNIT_STATUS_HEIGHT;

	NullBorders();

	for(int i=0; i< k_NUM_C3_FANCYBORDERS; i++) {
		if(ldlBorder && ldlBorder[i]) {
			m_border[i] = new C3Window(retval, aui_UniqueId(), ldlBorder[i], 16, AUI_WINDOW_TYPE_FLOATING, false );
			Assert( AUI_NEWOK(m_border[i], *retval) );
			if ( !AUI_NEWOK(m_border[i], *retval) ) return;
			aui_Ldl::Remove( m_border[i] );

			m_border[i]->SetTransparent(true);
			m_offset[i].x = m_border[i]->X();
			m_offset[i].y = m_border[i]->Y();
			m_border[i]->Offset(m_x,m_y);
		}
	}

	m_title = NULL;
	m_cancel = NULL;
	m_ok = NULL;

	if ( ldlTitle ) {
		m_title = new c3_Static( retval, aui_UniqueId(), ldlTitle );
		Assert( AUI_NEWOK(m_title, *retval) );
		if ( !AUI_NEWOK(m_title, *retval) ) return;
	}

	if ( exitCallBack ) {
		MBCHAR buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
		sprintf( buttonBlock, "c3_FancyCancel" );
		m_cancel = new c3_Button( retval, aui_UniqueId(), buttonBlock, exitCallBack, this );
		Assert( AUI_NEWOK(m_cancel, *retval) );
		if ( !AUI_NEWOK(m_cancel, *retval) ) return;

		m_cancel->Move( 0, m_height - m_cancel->Height() );
		AddControl( m_cancel );

		sprintf( buttonBlock, "c3_FancyOk" );
		m_ok = new c3_Button( retval, aui_UniqueId(), buttonBlock, exitCallBack, this );
		Assert( AUI_NEWOK(m_ok, *retval) );
		if ( !AUI_NEWOK(m_ok, *retval) ) return;

		m_ok->Move( m_width - m_ok->Width(), m_height - m_ok->Height() );
		AddControl( m_ok );

	}


}




c3_FancyWindow::~c3_FancyWindow()
{









	if ( m_title ) {
		delete m_title;
		m_title = NULL;
	}

	if ( m_cancel ) {
		delete m_cancel;
		m_cancel = NULL;
	}

	if ( m_ok ) {
		delete m_ok;
		m_ok = NULL;
	}
}

void c3_FancyWindow::MakeDraggable( BOOL draggable )
{
	SetDraggable( draggable );
	m_grabRegion->Move( 0, 0 );
	m_grabRegion->Resize( m_width, m_height );

	for(int i=0; i<k_NUM_C3_FANCYBORDERS; i++) {
		if(m_border[i]) {
			m_border[i]->SetBlindness( TRUE );
		}
	}
}


void c3_FancyWindow::MouseLGrabInside (aui_MouseEvent *mouseData)
{
	if (IsDisabled()) return;

	C3Window::MouseLGrabInside(mouseData);

	BringBorderToTop();

}

void c3_FancyWindow::MouseLDragAway (aui_MouseEvent *mouseData)
{
	if (IsDisabled()) return;

	C3Window::MouseLDragAway(mouseData);

	for(int i=0; i< k_NUM_C3_FANCYBORDERS; i++) {
		if(m_border[i])
			m_border[i]->Move(m_offset[i].x+m_x,m_offset[i].y+m_y);
	}
}

void c3_FancyWindow::BringBorderToTop()
{





}

AUI_ERRCODE c3_FancyWindow::AddBordersToUI()
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;






	return errcode;
}

AUI_ERRCODE c3_FancyWindow::RemoveBordersFromUI()
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;






	return errcode;
}






AUI_ERRCODE	c3_FancyWindow::Resize( sint32 width, sint32 height )
{
	C3Window::Resize(width,height);

	sint32 dw = width - m_originalDimensions.x;

	sint32 lx =0, rx = dw, ty = 0;

#if 0
	m_border[C3_FANCYBORDER_TL]->Offset(lx,ty);
	m_border[C3_FANCYBORDER_TR]->Offset(rx,ty);
	m_border[C3_FANCYBORDER_TOP]->Offset(lx,ty);
#else
	m_offset[C3_FANCYBORDER_TL].x+=lx; m_offset[C3_FANCYBORDER_TL].y+=ty;
	m_offset[C3_FANCYBORDER_TR].x+=rx; m_offset[C3_FANCYBORDER_TR].y+=ty;

	m_offset[C3_FANCYBORDER_TOP].y+=ty;
	for(int i=0; i<k_NUM_C3_FANCYBORDERS; i++) {
		if(m_border[i])
			m_border[i]->Move(m_offset[i].x+m_x,m_offset[i].y+m_y);
	}
#endif

	m_originalDimensions.x = width; m_originalDimensions.y = height;

	return AUI_ERRCODE_OK;
}
