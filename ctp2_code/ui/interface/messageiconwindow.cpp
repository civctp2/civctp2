








#include "c3.h"

#include "profiledb.h"

#include "aui.h"
#include "aui_ldl.h"
#include "aui_tipwindow.h"
#include "aui_uniqueid.h"
#include "aui_static.h"

#include "messageiconbutton.h"
#include "messageactions.h"
#include "messagewin.h"
#include "messagewindow.h"
#include "messagelist.h"
#include "messageiconwindow.h"
#include "filenamedb.h"

#include "c3ui.h"
#include "SelItem.h"

extern C3UI				*g_c3ui;
extern ProfileDB		*g_theProfileDB;

extern uint16 g_messageReadPositionX;
extern uint16 g_messageReadPositionY;
extern uint8 g_messageMaxVisible;
extern uint8 g_messageIconHeight;
extern uint8 g_messageIconSpacing;
extern FilenameDB *g_theMessageIconFileDB;
extern SelectedItem *g_selected_item;


MessageIconWindow *MessageIconWindow::m_currentIconWindow = NULL;


MessageIconWindow::MessageIconWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	Message data,
	sint32 bpp,
	MessageList *messagelist,
	AUI_WINDOW_TYPE type  )
	:
	aui_Window( retval, id, ldlBlock, bpp, type )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	errcode = InitCommon( &data, ldlBlock, messagelist );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return;
	
}



AUI_ERRCODE MessageIconWindow::InitCommon( Message *data, 
										  MBCHAR *ldlBlock, 
										  MessageList *messagelist )
{
	MBCHAR			iconDataBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	m_messageWindow = NULL;
	m_isMoving = FALSE;
	m_currentY = 0;
	m_targetY = 0;
	m_acceleration = 0;
	tech_WLList<MessageIconWindow *> *list = messagelist->GetList();
	if ( list->L() == 0 )
		m_prev = NULL;
	else
		m_prev = list->GetTail();

	m_next = NULL;
	m_messageList = messagelist;

	if ( m_prev )
		m_prev->SetNext( this );

	sprintf( iconDataBlock, "%s.%s", ldlBlock, "icon" );
	m_icon = new MessageIconButton( &errcode, aui_UniqueId(), iconDataBlock );
	Assert( AUI_NEWOK( m_icon, errcode ));
	if ( !AUI_NEWOK( m_icon, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	const MBCHAR *iconName = g_theMessageIconFileDB->GetFilename(data->GetMsgType());
	if(!iconName)
		iconName = k_MESSAGE_ICON_PICTURE_WARNING;
	const MBCHAR *iconSelectedName = g_theMessageIconFileDB->GetFilename(data->AccessData()->GetSelectedMsgType());
	if(!iconSelectedName)
		iconSelectedName = k_MESSAGE_ICON_PICTURE_WARNING_SELECTED;

	ChangeIcon(iconName, iconSelectedName);

#if 0
	switch( data->GetMsgType() )
	{
	case MESSAGE_TYPE_EVENT:
		ChangeIcon( k_MESSAGE_ICON_PICTURE_WARNING,
					k_MESSAGE_ICON_PICTURE_WARNING_SELECTED );
		break;
	case MESSAGE_TYPE_DIPLOMATIC:
		ChangeIcon( k_MESSAGE_ICON_PICTURE_DIPLOMATIC,
					k_MESSAGE_ICON_PICTURE_DIPLOMATIC_SELECTED );
		break;
	case MESSAGE_TYPE_TRADE:
		ChangeIcon( k_MESSAGE_ICON_PICTURE_TRADE,
					k_MESSAGE_ICON_PICTURE_TRADE_SELECTED );
		break;
	case MESSAGE_TYPE_MILITARY:
		ChangeIcon( k_MESSAGE_ICON_PICTURE_MILITARY,
					k_MESSAGE_ICON_PICTURE_MILITARY_SELECTED );
		break;
	case MESSAGE_TYPE_KNOWLEDGE:
		ChangeIcon( k_MESSAGE_ICON_PICTURE_KNOWLEDGE,
					k_MESSAGE_ICON_PICTURE_KNOWLEDGE_SELECTED );
		break;
	case MESSAGE_TYPE_WONDER:
		ChangeIcon( k_MESSAGE_ICON_PICTURE_WONDER,
					k_MESSAGE_ICON_PICTURE_WONDER_SELECTED );
		break;
	default:
		
		ChangeIcon( k_MESSAGE_ICON_PICTURE_WARNING,
					k_MESSAGE_ICON_PICTURE_WARNING_SELECTED );
		break;
	}
#endif

	AddControl( m_icon );





	uint32 pos = messagelist->GetList()->L();

	if ( pos > ( g_messageMaxVisible + messagelist->GetOffset() )) {
		if(data->GetOwner() == g_selected_item->GetVisiblePlayer())
			messagewin_MoreMessagesIcon( TRUE );
	} else {
		SetupAnimation( pos - messagelist->GetOffset( ));
	}

	
	m_messageOpenAction = new MessageOpenAction( this );
	Assert( m_messageOpenAction != NULL );
	if ( m_messageOpenAction == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_icon->SetAction( m_messageOpenAction );

	
	if ( MBCHAR *text = ( MBCHAR * ) data->AccessData()->GetMsgCaption() )
		SetTipWindowText( text );

	return AUI_ERRCODE_OK;
}



void MessageIconWindow::ChangeIcon( const MBCHAR *image, const MBCHAR *image2 )
{
	char nonConstImage[_MAX_PATH];
	strcpy(nonConstImage, image);
	((aui_ImageBase *)m_icon)->SetImage( nonConstImage,
										 0,
										 AUI_IMAGEBASE_SUBSTATE_STATE );
	if ( image2 ) {
		strcpy(nonConstImage, image2);
		((aui_ImageBase *)m_icon)->SetImage( nonConstImage,
											 1,
											 AUI_IMAGEBASE_SUBSTATE_STATE );
	}
}



void MessageIconWindow::SetCurrentIconButton( MessageIconButton *iconButton )
{
	
	if ( m_currentIconWindow && m_currentIconWindow->GetWindow()) {
		if (g_c3ui->GetWindow( m_currentIconWindow->GetWindow()->Id( ))) {
			m_currentIconWindow->GetWindow()->ShowWindow( FALSE );
		}
	}
	
	if ( iconButton ) {
		m_currentIconWindow = this;
	} else
		m_currentIconWindow = NULL;

	if (m_icon)
		m_icon->SetCurrentIconButton( iconButton );
}


void MessageIconWindow::SetTipWindowText( MBCHAR *text )
{
	if ( text == NULL ) return;

	if ( !strlen( text )) {
		
		delete ((aui_TipWindow *)m_icon->GetTipWindow());
		m_icon->SetTipWindow( NULL );
	} else {
		((aui_TipWindow *)m_icon->GetTipWindow())->GetStatic()->SetText( text );
	}
}


AUI_ERRCODE MessageIconWindow::SetupAnimation( uint32 position ) 
{
	
	


	m_targetY = g_messageReadPositionY - 
				(( g_messageIconHeight + g_messageIconSpacing ) * position );

	if ( BOUNCE_IT ) {

 		m_isMoving = TRUE;
		m_currentY = Y();
		if ( !m_acceleration )
			m_acceleration = 1;
	} else {
		Move( X(), m_targetY );
	}
	return AUI_ERRCODE_OK;
}



void MessageIconWindow::StopAnimation( void )
{
	m_targetY = 0;
	m_isMoving = FALSE;
	m_currentY = 0;
	m_acceleration = 0;
}



BOOL MessageIconWindow::CheckShowWindow( void )
{
	if ( m_prev ) {
		if ( m_prev->Y() > ( Y() + Height( )))
			return TRUE;
		else
			return FALSE;
	}

	return TRUE;
}


AUI_ERRCODE MessageIconWindow::Idle( void )
{
	if ( !m_isMoving ) return AUI_ERRCODE_OK;


	if ( BOUNCE_IT ) {
		if (( m_currentY + m_acceleration ) > m_targetY ) {
			m_currentY = m_targetY;
			if ( m_acceleration < 14 ) {
				m_isMoving = FALSE;
				m_acceleration = 0;
			} else
				m_acceleration = -abs( m_acceleration >> 1 );
		} else {
			m_currentY += m_acceleration;
			m_acceleration += 2;
		}

		Move( X(), m_currentY );

		if (( m_next ) && ( m_acceleration > 12 ))
			m_messageList->CheckVisibleMessages();
	}
	return AUI_ERRCODE_OK;
}



MessageIconWindow::~MessageIconWindow()
{
	if ( m_icon ) {
		delete m_icon;
		m_icon = NULL;
	}
	
	if ( m_messageOpenAction ) {
		delete m_messageOpenAction;
		m_messageOpenAction = NULL;
	}

	if ( m_next ) {
		m_next->SetPrev( m_prev );
	}

	if ( m_prev ) {
		m_prev->SetNext( m_next );
	}

}


