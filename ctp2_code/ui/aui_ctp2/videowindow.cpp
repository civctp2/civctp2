












#include "c3.h"
#include "aui.h"

#include "c3errors.h"
#include "aui_control.h"
#include "pattern.h"

#include "primitives.h"

#include "aui_Factory.h"
#include "CivPaths.h"
#include "c3ui.h"
#include "directvideo.h"

#include "videowindow.h"

#ifdef __AUI_USE_DIRECTX__
extern DirectVideo		*g_video;
#endif
extern CivPaths			*g_civPaths;
extern C3UI				*g_c3ui;



VideoWindow::VideoWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *pattern,
	MBCHAR *name,
	BOOL modal,
	AUI_WINDOW_TYPE type )
	:
	C3Window()
{
	
	*retval = aui_Region::InitCommon( id, x, y, width, height );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	
	*retval = aui_Window::InitCommon( bpp, type );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	
	*retval = C3Window::InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	
	*retval = PatternBase::InitCommon( pattern );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateVideoSurface( name, modal );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE VideoWindow::InitCommon( void )
{
#ifdef __AUI_USE_DIRECTX__
	m_video = NULL,
#endif
	m_modal = FALSE;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE VideoWindow::CreateVideoSurface(MBCHAR *name, BOOL modal)
{
	HRESULT			hr;
	AUI_ERRCODE		errcode;

#ifdef __AUI_USE_DIRECTX__
	m_video = new DirectVideo();
	if (m_video == NULL) return AUI_ERRCODE_MEMALLOCFAILED;

	m_modal = modal;

	MBCHAR	fname[_MAX_PATH];
	strcpy(m_filename, name);

	g_civPaths->FindFile(C3DIR_VIDEOS, name, fname);
	hr = m_video->OpenStream(fname);
	Assert(!hr);
	if (hr != 0) {
		
		c3errors_ErrorDialog("Video", "Error opening DirectMovie Indeo 5 video stream.  Be sure that DirectMovie and Indeo 5 are installed.");
		return AUI_ERRCODE_MEMALLOCFAILED;
	}

	RECT rect;

	m_video->GetVideoRect(&rect);




	Resize(rect.right-rect.left+20, rect.bottom-rect.top+40);

	m_surface = aui_Factory::new_Surface(errcode, m_width, m_height);
	Assert( m_surface != NULL );
	if ( !m_surface ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_video->Initialize((aui_DirectUI *)g_c3ui, (aui_Window *)this, m_modal);

	OffsetRect(&rect, 10, 30);
	if (m_modal) {
		OffsetRect(&rect, X(), Y());
	}

	m_video->SetDestRect(&rect);
#endif

	return AUI_ERRCODE_OK;
}



VideoWindow::~VideoWindow()
{
#ifdef __AUI_USE_DIRECTX__
	if (m_video != NULL) {
		
		m_video->CloseStream();
		delete m_video;
		m_video = NULL;
	}
#endif
}



AUI_ERRCODE VideoWindow::Idle(void)
{
#ifdef __AUI_USE_DIRECTX__
	if (m_video != NULL) {
		m_video->Process();
		if (m_video->Finished()) {
			
			m_video->CloseStream();
			delete m_video;
			m_video = NULL;

			
		}
	}
#endif
	return AUI_ERRCODE_OK;
}


AUI_ERRCODE VideoWindow::DrawThis(aui_Surface *surface, sint32 x, sint32 y)
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_width, m_height };

	m_pattern->Draw( m_surface, &rect );

	
	primitives_BevelRect16( m_surface, &rect, 1, 0, 16, 16 );

	primitives_DropText(m_surface, 5, 3, m_filename, 0xFFFF, TRUE);

	rect.top += 20;

	InflateRect(&rect, -5, -5);
	primitives_BevelRect16( m_surface, &rect, 5, 1, 16, 16 );

	
	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;
}
