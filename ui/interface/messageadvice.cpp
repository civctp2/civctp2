








#include "c3.h"


#include "aui.h"
#include "c3ui.h"
#include "aui_button.h"
#include "aui_static.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"

#include "c3listbox.h"


#include "messageactions.h"
#include "messagewindow.h"
#include "messageadvice.h"


extern C3UI			*g_c3ui;

MessageAdvice		*g_adviceMessageWindow = NULL;

int messageadvice_AddText( MBCHAR *text )
{
	AUI_ERRCODE	errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( !g_adviceMessageWindow ) {
		strcpy( windowBlock, "AdviceWindow" );

		g_adviceMessageWindow = new MessageAdvice( &errcode, aui_UniqueId(), 
											windowBlock, 16 );
		Assert( AUI_NEWOK( g_adviceMessageWindow, errcode ));
		if ( !AUI_NEWOK( g_adviceMessageWindow, errcode )) return -1;
		g_adviceMessageWindow->SetDraggable( TRUE );

		g_c3ui->AddWindow( g_adviceMessageWindow );
		
		g_adviceMessageWindow->AddBordersToUI();
	}
		
	g_adviceMessageWindow->AppendText( text );

	return 1;
}


int messageadvice_DestroyWindow( void )
{
	if ( g_adviceMessageWindow ) 
	{
		if ( g_c3ui->GetWindow( g_adviceMessageWindow->Id() ))
			g_c3ui->RemoveWindow( g_adviceMessageWindow->Id() );

		g_adviceMessageWindow->RemoveBordersFromUI();
		
		delete g_adviceMessageWindow;
		g_adviceMessageWindow = NULL;
	}

	return 1;
}



MessageAdvice::MessageAdvice(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type )
	:
	C3Window(retval, id, ldlBlock, bpp, type)
{
	*retval = InitCommon( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE MessageAdvice::InitCommon( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	m_leftBar = NULL;
	m_topBar = NULL;
	m_rightBar = NULL;
	m_bottomBar = NULL;
	m_listBox = NULL;

	errcode = CreateWindowEdges( ldlBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	errcode = CreateDismissButton( ldlBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	MakeSureSurfaceIsValid( );

	errcode = CreateTextBox( ldlBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	SetDraggable( TRUE );
	
	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageAdvice::CreateWindowEdges( MBCHAR *ldlBlock )
{
	MBCHAR		imageBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	sprintf( imageBlock, "%s.%s", ldlBlock, "MessageLeftBar" );
	m_leftBar = new aui_Static( &errcode, aui_UniqueId(), imageBlock );
	Assert( AUI_NEWOK( m_leftBar, errcode ));
	if ( !AUI_NEWOK( m_leftBar, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;
	m_leftBar->SetImageBltType( AUI_IMAGEBASE_BLTTYPE_TILE );
	AddControl( m_leftBar );

	sprintf( imageBlock, "%s.%s", ldlBlock, "MessageRightBar" );
	m_rightBar = new aui_Static( &errcode, aui_UniqueId(), imageBlock );
	Assert( AUI_NEWOK( m_rightBar, errcode ));
	if ( !AUI_NEWOK( m_rightBar, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;
	m_rightBar->SetImageBltType( AUI_IMAGEBASE_BLTTYPE_TILE );
	AddControl( m_rightBar );
	
	strcpy( imageBlock, "FancyAdviceTopBar" );
	m_topBar = new C3Window( &errcode, aui_UniqueId(), imageBlock, 16, AUI_WINDOW_TYPE_FLOATING, false );
	Assert( AUI_NEWOK( m_topBar, errcode ));
	if ( !AUI_NEWOK( m_topBar, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;
	m_topBar->SetTransparent( TRUE );
	m_topBar->SetBlindness( TRUE );

	m_offsetTop.x = m_topBar->X();
	m_offsetTop.y = m_topBar->Y();
	m_topBar->Offset( m_x, m_y );

	sprintf( imageBlock, "FancyAdviceBottomBar" );
	m_bottomBar = new C3Window( &errcode, aui_UniqueId(), imageBlock, 16, AUI_WINDOW_TYPE_FLOATING, false );
	Assert( AUI_NEWOK( m_bottomBar, errcode ));
	if ( !AUI_NEWOK( m_bottomBar, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;
	m_bottomBar->SetTransparent( TRUE );

	m_offsetBottom.x = m_bottomBar->X();
	m_offsetBottom.y = m_bottomBar->Y();
	m_bottomBar->Offset( m_x, m_y );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageAdvice::AppendText( MBCHAR *text )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	aui_Static *item;

	item = new aui_Static( &errcode, aui_UniqueId(), "AdviceWindowItem" );
	Assert( AUI_NEWOK( item, errcode ));
	if ( !AUI_NEWOK( item, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	item->SetText( text );

	m_listBox->AddItem((aui_Item *)item );

	return AUI_ERRCODE_OK;

}



AUI_ERRCODE MessageAdvice::CreateDismissButton( MBCHAR *ldlBlock ) 
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( buttonBlock, "%s.%s", ldlBlock, "DismissButton" );
	m_dismissButton = new aui_Button( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_dismissButton, errcode ));
	if ( !AUI_NEWOK( m_dismissButton, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	errcode = AddControl( m_dismissButton );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	m_dismissAction = new MessageAdviceDismissAction( );
	Assert( m_dismissAction != NULL );
	if ( m_dismissAction == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_dismissButton->SetAction( m_dismissAction );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE MessageAdvice::CreateTextBox( MBCHAR *ldlBlock )
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	sprintf( textBlock, "%s.%s", ldlBlock, "MessageTextBox" );
	m_listBox = new C3ListBox( &errcode, aui_UniqueId(), textBlock );
	Assert( AUI_NEWOK( m_listBox, errcode ));
	if ( !AUI_NEWOK( m_listBox, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_listBox->SetForceSelect( FALSE );
	m_listBox->SetMultiSelect( FALSE );

	AddControl( m_listBox );
	
	return AUI_ERRCODE_OK;
}



MessageAdvice::~MessageAdvice() 
{
	if ( m_leftBar ) {
		delete m_leftBar;
		m_leftBar = NULL;
	}

	if ( m_topBar ) {
		delete m_topBar;
		m_topBar = NULL;
	}

	if ( m_rightBar ) {
		delete m_rightBar;
		m_rightBar = NULL;
	}
	
	if ( m_bottomBar ) {
		delete m_bottomBar;
		m_bottomBar = NULL;
	}
	
	if ( m_listBox ) {
		aui_Static *item = NULL;
		sint32 count = m_listBox->NumItems();

		for ( sint32 i = 0; i < count; i++ ) {
			item = (aui_Static *)m_listBox->GetItemByIndex( i );
			
			if ( item ) {
				delete item;
				item = NULL;
			}
		}

		delete m_listBox;
		m_listBox = NULL;
	}

}



void MessageAdvice::MouseLGrabInside (aui_MouseEvent *mouseData)
{
	if ( IsDisabled() ) return;
	C3Window::MouseLGrabInside(mouseData);
	
	BringBorderToTop();

}



void MessageAdvice::MouseLDragAway (aui_MouseEvent *mouseData)
{
	if ( IsDisabled() ) return;
	C3Window::MouseLDragAway(mouseData);

	
	if ( m_topBar )
		m_topBar->Move( m_offsetTop.x + m_x, m_offsetTop.y + m_y );

	if ( m_bottomBar )
		m_bottomBar->Move( m_offsetBottom.x + m_x, m_offsetBottom.y + m_y );

}



void MessageAdvice::BringBorderToTop()
{
	if ( m_topBar )
		g_c3ui->BringWindowToTop( m_topBar );

	if ( m_bottomBar )
		g_c3ui->BringWindowToTop( m_bottomBar );
}



AUI_ERRCODE MessageAdvice::AddBordersToUI()
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	
	if ( m_topBar )
	{
		errcode = g_c3ui->AddWindow( m_topBar );
		Assert( errcode == AUI_ERRCODE_OK );
		if(	errcode != AUI_ERRCODE_OK ) return errcode;
	}

	if ( m_bottomBar )
	{
		errcode = g_c3ui->AddWindow( m_bottomBar );
		Assert( errcode == AUI_ERRCODE_OK );
		if(	errcode != AUI_ERRCODE_OK ) return errcode;
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE MessageAdvice::RemoveBordersFromUI()
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	if ( m_topBar )
	{
		errcode = g_c3ui->RemoveWindow( m_topBar->Id( ));
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return errcode;
	}

	if ( m_bottomBar )
	{
		errcode = g_c3ui->RemoveWindow( m_bottomBar->Id( ));
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return errcode;
	}

	return AUI_ERRCODE_OK;
}
