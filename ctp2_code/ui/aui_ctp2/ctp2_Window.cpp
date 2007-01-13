












#include "c3.h"

#include "aui.h"
#include "aui_control.h"
#include "aui_ldl.h"
#include "aui_ui.h"
#include "pattern.h"

#include "primitives.h"

#include "ctp2_Window.h"
#include "aui_action.h"

#include "c3ui.h"
#include "ctp2_Static.h"
#include "StrDB.h"

extern C3UI *g_c3ui;

#define k_DOCK_SNAP_MARGIN 10

#define k_STANDARD_MAIN_WINDOW_X 224
#define k_STANDARD_MAIN_WINDOW_Y 28


ctp2_Window::ctp2_Window(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type,
	bool bevel)
	:
	aui_Window( retval, id, ldlBlock, bpp, type ),
	PatternBase( ldlBlock, NULL )
{
	m_bevel = bevel;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	m_weaklyModalCancelCallback = NULL;
	m_weaklyModalCancelCookie = NULL;
	m_dockedTo = NULL;
	m_dock = NULL;
}



ctp2_Window::ctp2_Window(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *pattern,
	AUI_WINDOW_TYPE type,
	bool bevel)
	:
	aui_Window( retval, id, x, y, width, height, bpp, type ),
	PatternBase( pattern )
{
	m_bevel = bevel;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	m_weaklyModalCancelCallback = NULL;
	m_weaklyModalCancelCookie = NULL;
	m_dockedTo = NULL;
	m_dock = NULL;
}



AUI_ERRCODE ctp2_Window::InitCommon( void )
{
	GrabRegion()->Move( 0, 0 );
	GrabRegion()->Resize( m_width, 20 );

	
	SetDynamic(TRUE);

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE ctp2_Window::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_width, m_height };

	if (m_pattern)
		m_pattern->Draw( m_surface, &rect );

	
	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;
}

class WeaklyModalCloseAction : public aui_Action
{
  public:
	WeaklyModalCloseAction(ctp2_Window *win) : m_window(win) {}
	virtual ActionCallback Execute;

  protected:
	ctp2_Window *m_window;
};

void WeaklyModalCloseAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	g_ui->RemoveWindow(m_window->Id());
}

void ctp2_Window::MouseLGrabOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( IsWeaklyModal() )
	{
		
		
		
		g_ui->AddAction(new WeaklyModalCloseAction(this));

		bool passEventOn = false;
		if(m_weaklyModalCancelCallback) {
			m_weaklyModalCancelCallback(mouseData, this, m_weaklyModalCancelCookie, passEventOn);
		}
		if ( !passEventOn && m_mouseCode == AUI_ERRCODE_UNHANDLED )
			
			m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

	}
}

void    ctp2_Window::ResetCurrentMouseState()
{
	m_mouseState = *g_ui->TheMouse()->GetLatestMouseEvent();
}



AUI_ERRCODE ctp2_Window::DoneInstantiatingThis(const MBCHAR *ldlBlock)
{
	aui_Ldl *theLdl = g_c3ui->GetLdl();

	ctp2_Static *background = (ctp2_Static *)aui_Ldl::GetObject((MBCHAR *)ldlBlock, "Background");
	if(background) {
		background->Enable(FALSE);
	}

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock((MBCHAR *)ldlBlock);
	if(block) {
		MBCHAR *title = block->GetString("title");
		if(title) {
			ctp2_Static *titleBar = (ctp2_Static *)aui_Ldl::GetObject((MBCHAR *)ldlBlock, "TitleBar");
			if(!titleBar) {
				titleBar = (ctp2_Static *)aui_Ldl::GetObject((MBCHAR *)ldlBlock, "Background.TitleBar");
			}
			if(titleBar) {
				if(g_theStringDB->GetNameStr(title)) {
					titleBar->SetText((MBCHAR *)g_theStringDB->GetNameStr(title));
				}
				SetDraggable(TRUE);
				titleBar->Move(0, 0);
				titleBar->Resize(m_width, titleBar->Height());
				titleBar->SetBlindness(TRUE);
				titleBar->Enable(FALSE);
			}
		}
	}

	if(block->GetBool("centeredwindow")) {
		sint32 x, y;
		x = g_c3ui->Width() / 2 - Width() / 2;
		y = g_c3ui->Height() / 2 - Height() / 2;
		if(x < k_STANDARD_MAIN_WINDOW_X) x = k_STANDARD_MAIN_WINDOW_X;
		if(y < k_STANDARD_MAIN_WINDOW_Y) y = k_STANDARD_MAIN_WINDOW_Y;

		if(x + Width() > g_c3ui->Width()) {
			x = g_c3ui->Width() - Width();
		}

		if(y + Height() > g_c3ui->Height()) {
			y = g_c3ui->Height() - Height();
		}
		Move(x, y);
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE
ctp2_Window::Move(sint32 x, sint32 y)
{	
	sint32 oldx = X(), oldy = Y();

	AUI_ERRCODE err = aui_Window::Move(x, y);
	PointerList<ctp2_Window>::Walker walk(&m_dockedWindows);
	while(walk.IsValid()) {
		walk.GetObj()->aui_Window::Offset(x - oldx, y - oldy);
		walk.Next();
	}
	return err;
	
}


AUI_ERRCODE
ctp2_Window::Offset(sint32 dx, sint32 dy)
{
	AUI_ERRCODE err = aui_Window::Offset(dx, dy);

	PointerList<ctp2_Window>::Walker walk(&m_dockedWindows);
	while(walk.IsValid()) {
		walk.GetObj()->aui_Window::Offset(dx, dy);
		walk.Next();
	}

	return err;
}

void ctp2_Window::AddDockedWindow(ctp2_Window *window)
{
	if(window->m_dockedTo) {
		
		window->m_dockedTo->RemoveDockedWindow(window);
	}

	if(!m_dockedWindows.Find(window)) {
		m_dockedWindows.AddTail(window);
	}

	window->m_dockedTo = this;
}

void ctp2_Window::RemoveDockedWindow(ctp2_Window *window)
{
	if(window->m_dockedTo != this)
		return;

	PointerList<ctp2_Window>::Walker walk(&m_dockedWindows);
	while(walk.IsValid()) {
		if(walk.GetObj() == window) {
			walk.Remove();
		} else {
			walk.Next();
		}
	}

	window->m_dockedTo = NULL;
}

void ctp2_Window::MouseLDropOutside(aui_MouseEvent *mouseData)
{
	if(IsDisabled()) return;

	if(m_isDragging) {
		if(m_dock) {

			bool docked = false;
			if(abs(m_dock->X() - (X() + Width())) < k_DOCK_SNAP_MARGIN) {
				if((Y() + Height() >= m_dock->Y() - k_DOCK_SNAP_MARGIN) &&
				   (Y() <= m_dock->Y() + m_dock->Height() + k_DOCK_SNAP_MARGIN)) {
					
					sint32 newy;
					if(abs(Y() - m_dock->Y()) < k_DOCK_SNAP_MARGIN) {
						newy = m_dock->Y();
					} else {
						newy = Y();
					}

					Move(m_dock->X() - Width(), newy);
					m_dock->AddDockedWindow(this);
					docked = true;
				}
			} else if(abs((m_dock->X() + m_dock->Width()) - X()) < k_DOCK_SNAP_MARGIN) {
				if((Y() + Height() >= m_dock->Y() - k_DOCK_SNAP_MARGIN) &&
				   (Y() <= m_dock->Y() + m_dock->Height() + k_DOCK_SNAP_MARGIN)) {
					
					sint32 newy;
					if(abs(Y() - m_dock->Y()) < k_DOCK_SNAP_MARGIN) {
						newy = m_dock->Y();
					} else {
						newy = Y();
					}

					Move(m_dock->X() + m_dock->Width(), newy);
					m_dock->AddDockedWindow(this);
					docked = true;
				}				
			} else if(abs((Y() + Height()) - m_dock->Y()) < k_DOCK_SNAP_MARGIN) {
				if((X() + Width() >= m_dock->X() - k_DOCK_SNAP_MARGIN) &&
				   (X() <= m_dock->X() + m_dock->Width())) {
					
					Move(X(), m_dock->Y() - Height());
					m_dock->AddDockedWindow(this);
					docked = true;
				}
			} else if(abs(Y() - (m_dock->Y() + m_dock->Height())) < k_DOCK_SNAP_MARGIN) {
				if((X() + Width() >= m_dock->X() - k_DOCK_SNAP_MARGIN) &&
				   (X() <= m_dock->X() + m_dock->Width())) {
					
					Move(X(), m_dock->Y() + m_dock->Height());
					m_dock->AddDockedWindow(this);
					docked = true;
				}
			}
					

			if(!docked) {
				m_dock->RemoveDockedWindow(this);
			}


		}
	}

	aui_Window::MouseLDropOutside(mouseData);
}

void ctp2_Window::SetDock(ctp2_Window *window)
{
	m_dock = window;
	if(m_dockedTo && m_dockedTo != m_dock) {
		m_dockedTo->RemoveDockedWindow(this);
	}
}

bool ctp2_Window::HandleKey(uint32 wParam)
{
	if(aui_Window::HandleKey(wParam)) {
		return true;
	}

	if(m_dockedTo) {
		return m_dockedTo->HandleKey(wParam);
	}
	
	return false;
}

