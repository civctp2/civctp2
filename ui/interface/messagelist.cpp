






#include "c3.h"


#include "SelItem.h"
#include "profiledb.h"


#include "aui.h"
#include "c3ui.h"
#include "aui_ldl.h"
#include "aui_UniqueId.h"

#include "aui_static.h"
#include "aui_surface.h"


#include "textbox.h"


#include "textbutton.h"
#include "aui_button.h"
#include "aui_window.h"


#include "message.h"


#include "c3windows.h"
#include "messageiconwindow.h"
#include "messagewindow.h"
#include "messageactions.h"
#include "messagewin.h"
#include "messagelist.h"


extern C3UI				*g_c3ui;
extern SelectedItem		*g_selected_item;
extern ProfileDB		*g_theProfileDB;
extern uint16			g_messageReadPositionY;
extern uint8			g_messageMaxVisible;
extern uint8			g_messageIconHeight;
extern uint8			g_messageIconSpacing;

MessageList::MessageList( PLAYER_INDEX player )
{
	m_player = player;

	m_iconList = new tech_WLList<MessageIconWindow *>;
	Assert( m_iconList != NULL );

	m_offset = 0;
}

MessageList::~MessageList( )
{
	MessageIconWindow	*iconWindow = NULL;
	MessageWindow		*window = NULL;
	ListPos position = m_iconList->GetHeadPosition();

	for ( uint32 count = m_iconList->L(); count; count-- ) {
		iconWindow = m_iconList->GetNext( position );
		if (iconWindow) {
			window = iconWindow->GetWindow();

			if (window) {
				if ( g_c3ui->GetWindow( iconWindow->Id( )))
					g_c3ui->RemoveWindow( iconWindow->Id( ));

				if ( g_c3ui->GetWindow( window->Id( )))
					window->ShowWindow( FALSE );

				delete window;
			}
			
			delete iconWindow;
		}
	}

	m_iconList->DeleteAll( );
	delete m_iconList;
	m_iconList = NULL;
}


AUI_ERRCODE MessageList::CreateMessage( Message data )
{
	AUI_ERRCODE			errcode = AUI_ERRCODE_OK;
	MessageIconWindow	*mIconWindow = NULL;
	MessageWindow		*mWindow = NULL;
	MBCHAR				windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	mIconWindow = new MessageIconWindow( &errcode, 
										 aui_UniqueId(), 
										 "MessageIconWindow", 
										 data,
										 16,
										 this );
	Assert( AUI_NEWOK( mIconWindow, errcode ));
	if ( !AUI_NEWOK( mIconWindow, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	
	m_iconList->AddTail( mIconWindow );

	
	strcpy( windowBlock, "StandardMessageWindow" );
	
	mWindow = new MessageWindow( &errcode, aui_UniqueId(), windowBlock,
								 16, data, mIconWindow );
	Assert( AUI_NEWOK( mWindow, errcode ));
	if ( !AUI_NEWOK( mWindow, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	
	mIconWindow->SetWindow( mWindow );

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	CheckVisibleMessages( );

	return AUI_ERRCODE_OK;

}


void MessageList::HideVisibleWindows( void )
{
	MessageIconWindow	*iconWindow = NULL;
	MessageWindow		*window = NULL;
	
	if ( !m_iconList ) return;

	ListPos position = m_iconList->GetHeadPosition();

	for ( uint32 count = m_iconList->L(); count; count-- ) {
		iconWindow = m_iconList->GetNext( position );
		if (iconWindow) {
			window = iconWindow->GetWindow();

			iconWindow->SetCurrentIconButton( NULL );

			if ( g_c3ui->GetWindow( iconWindow->Id( )))
				g_c3ui->RemoveWindow( iconWindow->Id( ));
			
			if (window)
				if ( g_c3ui->GetWindow( window->Id( )))
					window->ShowWindow( FALSE );
		}
	}

}


void MessageList::CheckVisibleMessages( void ) 
{




return;

	uint32 minCount = m_offset;
	uint32 maxCount = ( m_offset + g_messageMaxVisible );
	uint32 count = 0;
	MessageIconWindow *iconWindow;

	if ( !g_selected_item || m_player != g_selected_item->GetVisiblePlayer( )) {
		HideVisibleWindows();
		return;
	}

	ListPos position = m_iconList->GetHeadPosition();

	for ( uint32 i = m_iconList->L(); i; i-- ) {
		iconWindow = m_iconList->GetNext( position );


		if (( count < maxCount ) && ( count >= minCount )) {
			if ( !g_c3ui->GetWindow( iconWindow->Id( )))
				if ( iconWindow->CheckShowWindow() ) {
					g_c3ui->AddWindow( iconWindow );
					if ( !BOUNCE_IT )

						iconWindow->Move( iconWindow->X(), ( g_messageReadPositionY - 
								(( g_messageIconHeight + g_messageIconSpacing ) * 
								 ( count - minCount ))));
				
				}
		} else {
			if ( g_c3ui->GetWindow( iconWindow->Id( )))
				g_c3ui->RemoveWindow( iconWindow->Id( ));
		}
		count++;
	}

	
	if (( count <= minCount ) && ( m_offset > 0 )) {
		ChangeOffset( -1, SHOW_MESSAGE_OFFSET_RELATIVE );
		return;
	}

	
	if ( count > maxCount )
		messagewin_MoreMessagesIcon( TRUE );
	else
		messagewin_MoreMessagesIcon( FALSE );


	if ( m_offset > 0 )
		messagewin_LessMessagesIcon( TRUE );
	else  
		messagewin_LessMessagesIcon( FALSE );



}




void MessageList::CheckMaxMessages( void ) 
{
	
	uint32 count = m_iconList->L();
	uint32 maxCount = ( m_offset + g_messageMaxVisible );

	if ( count > maxCount )
		messagewin_MoreMessagesIcon( TRUE );
	else
		messagewin_MoreMessagesIcon( FALSE );

}



void MessageList::ChangeOffset( sint32 offset, int flag )
{
	if ( flag == SHOW_MESSAGE_OFFSET_ABSOLUTE )
		m_offset = offset;
	else  
		m_offset = m_offset + offset;

	Assert( m_offset >= 0 );
	if ( m_offset < 0 ) {
		ChangeOffset( 0, SHOW_MESSAGE_OFFSET_ABSOLUTE );
		return;
	}


	uint32 minCount = m_offset;
	uint32 maxCount = ( m_offset + g_messageMaxVisible );
	uint32 count = 0;
	ListPos position = m_iconList->GetHeadPosition();

	for ( uint32 i = m_iconList->L(); i; i-- ) {
		MessageIconWindow *iconWindow = m_iconList->GetNext( position );

		if (iconWindow) {
			if ( count >= minCount ) {
				if ( count <= maxCount ) {
					if ( !g_c3ui->GetWindow( iconWindow->Id( )))
						g_c3ui->AddWindow( iconWindow );

					if ( iconWindow->Animating( ))
						iconWindow->SetupAnimation( count - minCount );
					else 
						iconWindow->Move( iconWindow->X(), ( g_messageReadPositionY - 
								(( g_messageIconHeight + g_messageIconSpacing ) *
								 ( count - minCount ))));
				} else {
					iconWindow->SetupAnimation( count - minCount );
				}
			} else {
				if ( g_c3ui->GetWindow( iconWindow->Id( ))) {
					g_c3ui->RemoveWindow( iconWindow->Id( ));
					iconWindow->StopAnimation();
				}
			}
		}
		count++;
	}

	CheckVisibleMessages();
}


void MessageList::Remove( MessageIconWindow *iconWindow, 
						  MessageWindow *window )
{
	
	if ( g_c3ui->GetWindow( iconWindow->Id( )))
		g_c3ui->RemoveWindow( iconWindow->Id( ));
	if ( g_c3ui->GetWindow( window->Id( )))
		window->ShowWindow(FALSE);

	ListPos position = m_iconList->Find( iconWindow );
	Assert( position != NULL );
	m_iconList->DeleteAt( position );

	if ( iconWindow == iconWindow->GetCurrentMessageIconWindow())
		iconWindow->SetCurrentIconButton( NULL );

	position = m_iconList->GetHeadPosition();
	for ( uint32 count = 0; count < m_iconList->L(); count++ ) {
		MessageIconWindow *iw = m_iconList->GetNext( position );
		if (( count >= m_offset ) && 
			( count < ( m_offset + g_messageMaxVisible ))) {
			iw->SetupAnimation( count - m_offset );
		}
	}

	delete window;
	delete iconWindow;

	CheckVisibleMessages();
}


