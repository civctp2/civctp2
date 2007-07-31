










#include "c3.h"
#include "aui_ui.h"
#include "aui_window.h"

#include "aui_screen.h"



aui_Screen::aui_Screen(
	AUI_ERRCODE *retval,
	uint32 id )
{
	*retval = InitCommon( id );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_Screen::InitCommon( uint32 id )
{
	m_id = id;
	m_showing = FALSE;

	m_windowList = new tech_WLList<aui_Window *>;
	Assert( m_windowList != NULL );
	if ( !m_windowList ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_tempWindowList = new tech_WLList<aui_Window *>;
	Assert( m_tempWindowList != NULL );
	if ( !m_tempWindowList ) return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}



aui_Screen::~aui_Screen()
{
	Hide();

	if ( m_windowList )
	{
		delete m_windowList;
		m_windowList = NULL;
	}

	if ( m_tempWindowList )
	{
		delete m_tempWindowList;
		m_tempWindowList = NULL;
	}
}



AUI_ERRCODE aui_Screen::Show( void )
{
	ListPos position = m_windowList->GetHeadPosition();
	for ( sint32 i = m_windowList->L(); i; i-- )
	{
		aui_Window *window = m_windowList->GetNext( position );
		if ( !g_ui->GetWindow( window->Id() ) )
			g_ui->AddWindow( window );
	}

	m_showing = TRUE;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Screen::Hide( void )
{
	ListPos position = m_windowList->GetHeadPosition();
	for ( sint32 i = m_windowList->L(); i; i-- )
	{
		aui_Window *window = m_windowList->GetNext( position );
		g_ui->RemoveWindow( window->Id() );
	}

	
	for ( i = m_tempWindowList->L(); i; i-- )
	{
		aui_Window *window = m_tempWindowList->RemoveTail();
		g_ui->RemoveWindow( window->Id() );
	}

	m_showing = FALSE;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Screen::AddWindow( aui_Window *window, BOOL temp )
{
	Assert( window != NULL );
	if ( !window ) return AUI_ERRCODE_INVALIDPARAM;

	
	if ( !GetWindow( window->Id() ) )
	{
		if ( temp )
			m_tempWindowList->AddTail( window );
		else
			m_windowList->AddTail( window );
	}

	
	if ( m_showing && !g_ui->GetWindow( window->Id() ) )
		g_ui->AddWindow( window );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Screen::RemoveWindow( uint32 windowId )
{
	ListPos position = m_windowList->GetHeadPosition();
	for ( sint32 i = m_windowList->L(); i; i-- )
	{
		ListPos prevPos = position;
		aui_Window *window = m_windowList->GetNext( position );
		if ( window->Id() == windowId )
		{
			
			m_windowList->DeleteAt( prevPos );
			g_ui->RemoveWindow( windowId );
			break;
		}
	}

	position = m_tempWindowList->GetHeadPosition();
	for ( i = m_tempWindowList->L(); i; i-- )
	{
		ListPos prevPos = position;
		aui_Window *window = m_tempWindowList->GetNext( position );
		if ( window->Id() == windowId )
		{
			
			m_tempWindowList->DeleteAt( prevPos );
			g_ui->RemoveWindow( windowId );
			break;
		}
	}

	return AUI_ERRCODE_OK;
}



aui_Window *aui_Screen::GetWindow( uint32 windowId )
{
	ListPos position = m_windowList->GetHeadPosition();
	for ( sint32 i = m_windowList->L(); i; i-- )
	{
		aui_Window *window = m_windowList->GetNext( position );
		if ( window->Id() == windowId ) return window;
	}

	position = m_tempWindowList->GetHeadPosition();
	for ( i = m_tempWindowList->L(); i; i-- )
	{
		aui_Window *window = m_tempWindowList->GetNext( position );
		if ( window->Id() == windowId ) return window;
	}

	return NULL;
}
