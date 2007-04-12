//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The civilization 3 fancy window
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
#include "aui_control.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "pattern.h"
#include "primitives.h"
#include "c3window.h"
#include "c3fancywindow.h"
#include "c3ui.h"
#include "c3_button.h"

extern C3UI						*g_c3ui;





C3FancyWindow::C3FancyWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	MBCHAR ldlBorder[k_NUM_BORDERS][ k_AUI_LDL_MAXBLOCK + 1 ],
	AUI_WINDOW_TYPE type,
	bool bevel,
	void (*exitCallBack)( aui_Control *, uint32, uint32, void *))
	: C3Window(retval, id, ldlBlock, bpp, type,bevel), m_exit(NULL)
{
	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;








	
	
	m_originalDimensions.x = 546;
	m_originalDimensions.y = 173;
	
	NullBorders();
	
		for(int i=0; i< k_NUM_BORDERS; i++)
			if(ldlBorder[i]) {
				m_border[i] = new C3Window(retval, aui_UniqueId(), ldlBorder[i], 16, AUI_WINDOW_TYPE_FLOATING, false );
				Assert( AUI_NEWOK(m_border[i], *retval) );
				if ( !AUI_NEWOK(m_border[i], *retval) ) return;
				
				
				m_border[i]->SetTransparent(true);
				m_offset[i].x = m_border[i]->X();
				m_offset[i].y = m_border[i]->Y();
				m_border[i]->Offset(m_x,m_y);
			}
	
	if(exitCallBack && m_border[TR]) {
		MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
		sprintf( buttonBlock, "%s.%s", ldlBorder[TR], "ExitButton" );
		m_exit = new c3_Button( retval, aui_UniqueId(), buttonBlock, exitCallBack );
		Assert( AUI_NEWOK(m_exit, *retval) );
		*retval = m_border[TR]->AddControl(m_exit);
		Assert( *retval == AUI_ERRCODE_OK );
	}

	g_c3ui->BringWindowToTop(this);
	BringBorderToTop();
}




C3FancyWindow::~C3FancyWindow()
{
	
	

	for(int i=0; i<k_NUM_BORDERS; i++) {
		if(m_border[i]) {
			g_c3ui->RemoveWindow(m_border[i]->Id());
			delete m_border[i]; m_border[i]= NULL;
		}
	}
	if(m_exit) { delete m_exit; m_exit = NULL;}
}


void C3FancyWindow::MakeDraggable( BOOL draggable )
{
	SetDraggable( draggable );
	m_grabRegion->Move( 0, 0 );
	m_grabRegion->Resize( m_width, m_height );

	
	for(int i=0; i<k_NUM_BORDERS; i++) {
		if(m_border[i]) {
			m_border[i]->SetBlindness( TRUE );
		}
	}
}



void C3FancyWindow::MouseLGrabInside (aui_MouseEvent *mouseData)
{
	if (IsDisabled()) return;

	C3Window::MouseLGrabInside(mouseData);
	
	BringBorderToTop();

}

void C3FancyWindow::MouseLDragAway (aui_MouseEvent *mouseData)
{
	if (IsDisabled()) return;

	C3Window::MouseLDragAway(mouseData);
	
	for(int i=0; i< k_NUM_BORDERS; i++) {
		if(m_border[i])
			m_border[i]->Move(m_offset[i].x+m_x,m_offset[i].y+m_y);
	}
}


void C3FancyWindow::BringBorderToTop()
{
	for(int i=0; i< k_NUM_BORDERS; i++)
		if(m_border[i])
			g_c3ui->BringWindowToTop( m_border[i] ); 
}

AUI_ERRCODE C3FancyWindow::AddBordersToUI()
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	for(int i=0; i< k_NUM_BORDERS; i++)
	{
		if(m_border[i])
			errcode = g_c3ui->AddWindow(m_border[i]);
		if(	errcode != AUI_ERRCODE_OK ) return errcode;
	}
	return errcode;
}

AUI_ERRCODE C3FancyWindow::RemoveBordersFromUI()
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	for(int i=0; i< k_NUM_BORDERS; i++)
	{
		if(m_border[i])
			errcode = g_c3ui->RemoveWindow(m_border[i]->Id());
		if(	errcode != AUI_ERRCODE_OK ) return errcode;
	}
	return errcode;
}






AUI_ERRCODE	C3FancyWindow::Resize( sint32 width, sint32 height )
{
	C3Window::Resize(width,height);
	
	sint32 dw = width - m_originalDimensions.x,
			dh = height - m_originalDimensions.y;

	
	
	
	
	sint32 lx =0, cx = dw/2, rx = dw,
			ty = 0, cy = dh/2, by = dh;
	
	
#if 0
	m_border[TL]->Offset(lx,ty);
	m_border[TR]->Offset(rx,ty);
	m_border[BL]->Offset(lx,by);
	m_border[BR]->Offset(rx,by);
	m_border[TOPL]->Offset(lx,ty);
	m_border[TOPR]->Offset(cx,ty);
	m_border[BOTTOM]->Offset(cx,by);
	m_border[FB_LEFT]->Offset(lx,ty);
	m_border[FB_RIGHT]->Offset(rx,ty);
	m_border[HEADER]->Offset(cx,ty);
#else
	m_offset[TL].x+=lx; m_offset[TL].y+=ty;
	m_offset[TR].x+=rx; m_offset[TR].y+=ty;
	m_offset[BL].x+=lx; m_offset[BL].y+=by;
	m_offset[BR].x+=rx; m_offset[BR].y+=by;
	m_offset[TOPL].x+=lx; m_offset[TOPL].y+=ty;
	m_offset[TOPR].x+=cx; m_offset[TOPR].y+=ty;
	m_offset[BOTTOM].x+=lx; m_offset[BOTTOM].y+=by;
	m_offset[FB_LEFT].x+=lx; m_offset[FB_LEFT].y+=ty;
	m_offset[FB_RIGHT].x+=rx; m_offset[FB_RIGHT].y+=ty;
	m_offset[HEADER].x+=cx; m_offset[HEADER].y+=ty;
	for(int i=0; i<k_NUM_BORDERS; i++) {
		if(m_border[i])
			m_border[i]->Move(m_offset[i].x+m_x,m_offset[i].y+m_y);
	}
#endif
	
	m_originalDimensions.x = width; m_originalDimensions.y = height;

	
	m_border[TOPL]->Resize(m_border[TOPL]->Width()+cx,m_border[TOPL]->Height());
	m_border[TOPR]->Resize(m_border[TOPR]->Width()+cx,m_border[TOPR]->Height());
	m_border[FB_LEFT]->Resize(m_border[FB_LEFT]->Width(),m_border[FB_LEFT]->Height()+dh);
	m_border[FB_RIGHT]->Resize(m_border[FB_RIGHT]->Width(),m_border[FB_RIGHT]->Height()+dh);
	m_border[BOTTOM]->Resize(m_border[BOTTOM]->Width()+dw,m_border[BOTTOM]->Height());
	return AUI_ERRCODE_OK;
}
