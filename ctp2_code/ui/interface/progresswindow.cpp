

#include "c3.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "c3_static.h"
#include "c3ui.h"

#include "progresswindow.h"

extern C3UI *g_c3ui;
extern sint32 g_ScreenWidth;

void ProgressWindow::BeginProgress(
	ProgressWindow *&progwin,
	MBCHAR *ldlBlock,
	sint32 maxval,
	sint32 x, sint32 y)
{
	Assert( maxval >= 0 );
	if ( maxval < 0 ) return;

	if ( !progwin )
	{
		AUI_ERRCODE errcode;
		progwin = new ProgressWindow( &errcode, ldlBlock, x, y );

		progwin->m_pbar->SetMaxValue( maxval );

		progwin->m_factorList.AddTail( 1.0 );

		
		
		
		g_c3ui->SetBackgroundColor( RGB(0,0,0) );

		




		g_c3ui->AddWindow( progwin );
	}
	else
	{
		progwin->m_factorList.AddTail(
			double(progwin->m_nextValList.GetTail() -
				   progwin->m_prevValList.GetTail()) / (double)maxval );
	}

	progwin->m_prevValList.AddTail( 0 );
	progwin->m_nextValList.AddTail( 0 );

	progwin->m_baseList.AddTail( progwin->m_pbar->GetCurValue() );

	
	progwin->StartCountingTo( 0 );
}

void ProgressWindow::StartCountingTo( sint32 val, MBCHAR *message )
{
	
	if ( message )
	{
		m_message->SetText( message );
	}
	else
	{
		
	}

	m_pbar->SetCurValue(
		m_baseList.GetTail() +
		sint32((double)m_nextValList.GetTail()*m_factorList.GetTail()) );

	m_prevValList.GetTail() = m_nextValList.GetTail();
	m_nextValList.GetTail() = val;

	
	Draw();
	g_c3ui->Draw();
}

void ProgressWindow::EndProgress( ProgressWindow *&progwin )
{
	if ( progwin )
	{
		
		progwin->StartCountingTo( progwin->m_pbar->GetMaxValue() );

		progwin->m_prevValList.RemoveTail();
		progwin->m_nextValList.RemoveTail();
		progwin->m_baseList.RemoveTail();
		progwin->m_factorList.RemoveTail();

		if ( progwin->m_baseList.IsEmpty() )
		{
			g_c3ui->RemoveWindow( progwin->Id() );

			delete progwin;
			progwin = NULL;

			
			g_c3ui->Draw();
			g_c3ui->SetBackgroundColor( k_AUI_UI_NOCOLOR );
		}
	}
}

ProgressWindow::ProgressWindow(
	AUI_ERRCODE *retval,
	MBCHAR *ldlBlock,
	sint32 x, sint32 y)
	:
	c3_PopupWindow(
		retval,
		aui_UniqueId(),
		ldlBlock, 16,
		AUI_WINDOW_TYPE_FLOATING,
		FALSE )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	if(x >= 0 || y >= 0) {
		Move(x, y);
	}

	MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( block, "%s.progressbar", ldlBlock );
	m_pbar = new StandardProgressBar(
		retval,
		aui_UniqueId(),
		block );
	Assert( AUI_NEWOK(m_pbar, *retval) );
	if ( !AUI_NEWOK(m_pbar, *retval) ) return;

	*retval = AddChild( m_pbar );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	sprintf( block, "%s.message", ldlBlock );
	m_message = new c3_Static(
		retval,
		aui_UniqueId(),
		block );
	Assert( AUI_NEWOK(m_message, *retval) );
	if ( !AUI_NEWOK(m_message, *retval) ) return;

	*retval = AddChild( m_message );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	SetStronglyModal( TRUE );
	SetDraggable(TRUE);
}

ProgressWindow::~ProgressWindow()
{
	if ( m_message )
	{
		RemoveChild( m_message->Id() );
		delete m_message;
		m_message = NULL;
	}

	if ( m_pbar )
	{
		RemoveChild( m_pbar->Id() );
		delete m_pbar;
		m_pbar = NULL;
	}
}



COLORREF ProgressWindow::SetBarColor(COLORREF color)
{
	return m_pbar->SetBarColor(color);
}

AUI_ERRCODE StandardProgressBar::CalculateIntervals(
	double *start,
	double *stop )
{
	double x = (double)m_curValue / (double)m_maxValue;

	
	
	
	*start = 0.0;
	*stop = x;

	return AUI_ERRCODE_OK;
}
