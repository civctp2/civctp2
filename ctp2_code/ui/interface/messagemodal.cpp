#include "c3.h"

#include "aui.h"
#include "c3ui.h"
#include "aui_button.h"
#include "aui_static.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_hypertextbox.h"

#include "c3_popupwindow.h"

#include "SlicButton.h"
#include "ctp2_button.h"
#include "aui_dimension.h"

#include "message.h"
#include "MessageData.h"
#include "messageactions.h"
#include "messagewindow.h"
#include "messageeyepoint.h"
#include "messagemodal.h"
#include "player.h"
#include "SelItem.h"

extern Player **g_player;
extern SelectedItem *g_selected_item;

extern C3UI			*g_c3ui;

extern sint32 g_ScreenHeight;

MessageModal		*g_modalMessage = NULL;

int messagemodal_CreateModalMessage( Message data )
{
	AUI_ERRCODE	errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( g_modalMessage ) return 1;

	uint32 count = data.AccessData()->GetNumButtons();
	Assert( count > 0 );
	if ( count == 0 ) return -1;

	strcpy( windowBlock, "ModalWindow" );

	g_modalMessage = new MessageModal( &errcode, aui_UniqueId(),
										windowBlock, 16, data );
	Assert( AUI_NEWOK( g_modalMessage, errcode ));
	if ( !AUI_NEWOK( g_modalMessage, errcode )) return -1;
	g_modalMessage->SetDraggable( TRUE );

	if(g_ScreenHeight >= 768) {
		g_modalMessage->Move(0, 228);
	}

	g_c3ui->AddWindow( g_modalMessage );

	g_modalMessage->AddBordersToUI();

	return 1;
}

int messagemodal_PrepareDestroyWindow()
{
	g_c3ui->AddDestructiveAction( new MessageModalDestroyAction());
	return 1;
}

int messagemodal_DestroyModalMessage( void )
{
	if ( g_modalMessage )
	{
		if ( g_c3ui->GetWindow( g_modalMessage->Id() ))
			g_c3ui->RemoveWindow( g_modalMessage->Id() );

		g_modalMessage->RemoveBordersFromUI();

		delete g_modalMessage;
		g_modalMessage = NULL;


		if(g_player && g_player[g_selected_item->GetVisiblePlayer()]) {
			g_player[g_selected_item->GetVisiblePlayer()]->NotifyModalMessageDestroyed();
		}
	}

	return 1;
}


MessageModal::MessageModal(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	sint32 bpp,
	Message data,
	AUI_WINDOW_TYPE type )
	:
	c3_PopupWindow(retval, id, ldlBlock, bpp, type)
{
	*retval = InitCommon( ldlBlock, data );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE MessageModal::InitCommon(const MBCHAR *ldlBlock, Message data)
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	m_messageText = NULL;
	m_message = data;

	m_leftBar = NULL;
	m_topBar = NULL;
	m_rightBar = NULL;
	m_bottomBar = NULL;

	SetStronglyModal( TRUE );
	SetDraggable( TRUE );

	errcode = CreateResponses( ldlBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;





	errcode = CreateEyePointBox( ldlBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;


	MakeSureSurfaceIsValid( );

	errcode = CreateStandardTextBox( ldlBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	return AUI_ERRCODE_OK;
}













































AUI_ERRCODE MessageModal::CreateStandardTextBox(const MBCHAR *ldlBlock)
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	sprintf( textBlock, "%s.%s", ldlBlock, "MessageTextBox" );
	m_messageText = new aui_HyperTextBox( &errcode, aui_UniqueId(), textBlock );
	Assert( AUI_NEWOK( m_messageText, errcode ));
	if ( !AUI_NEWOK( m_messageText, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;





	m_messageText->SetDrawMask( k_AUI_REGION_DRAWFLAG_UPDATE );

	AddControl( m_messageText );

	errcode = m_messageText->SetHyperText( m_message.GetText( ));
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE MessageModal::CreateEyePointBox(const MBCHAR *ldlBlock)
{
	switch( m_message.AccessData()->GetEyePointStyle())
	{
	case MESSAGE_EYEPOINT_STYLE_NONE:
		return AUI_ERRCODE_OK;
	case MESSAGE_EYEPOINT_STYLE_STANDARD:
		return CreateStandardEyePointBox( ldlBlock );
	case MESSAGE_EYEPOINT_STYLE_DROPDOWN:
		return CreateDropdownEyePointBox( ldlBlock );
	case MESSAGE_EYEPOINT_STYLE_LIST:
		return CreateListboxEyePointBox( ldlBlock );
	default:
		return AUI_ERRCODE_OK;
	}
}


AUI_ERRCODE MessageModal::CreateStandardEyePointBox(const MBCHAR *ldlBlock)
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	m_messageEyePoint.m_messageEyePointStandard = new MessageEyePointStandard( &errcode,
						ldlBlock, this );
	Assert( m_messageEyePoint.m_messageEyePointStandard != NULL );
	if ( m_messageEyePoint.m_messageEyePointStandard == NULL )
		return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageModal::CreateDropdownEyePointBox(const MBCHAR *ldlBlock)
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	m_messageEyePoint.m_messageEyePointDropdown = new MessageEyePointDropdown( &errcode,
						ldlBlock, this );
	Assert( m_messageEyePoint.m_messageEyePointDropdown != NULL );
	if ( m_messageEyePoint.m_messageEyePointDropdown == NULL )
		return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageModal::CreateListboxEyePointBox(const MBCHAR *ldlBlock)
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	m_messageEyePoint.m_messageEyePointListbox = new MessageEyePointListbox( &errcode,
						ldlBlock, this );
	Assert( m_messageEyePoint.m_messageEyePointListbox != NULL );
	if ( m_messageEyePoint.m_messageEyePointListbox == NULL )
		return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageModal::CreateResponses(const MBCHAR *ldlBlock)
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	ctp2_Button		*button, *lastbutton = NULL;
	MessageModalResponseAction	*action = NULL;
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	const MBCHAR	*text = NULL;
	sint32			responseCount = 0;
	SlicButton		*sButton = NULL;

	m_messageModalResponseButton = NULL;
	m_messageModalResponseAction = NULL;

	m_messageModalResponseButton = new tech_WLList<ctp2_Button *>;
	Assert( m_messageModalResponseButton != NULL );
	if ( m_messageModalResponseButton == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_messageModalResponseAction = new tech_WLList<MessageModalResponseAction *>;
	Assert( m_messageModalResponseAction != NULL );
	if ( m_messageModalResponseAction == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	sint32 textlength = k_MODAL_BUTTON_DEFAULT_WIDTH;














	while ((sButton = m_message.AccessData()->GetButton( responseCount ))) {
		text = sButton->GetName();

		sprintf(buttonBlock, "%s.%s", ldlBlock, "ModalResponseButton");
		button = new ctp2_Button( &errcode, aui_UniqueId(), buttonBlock );
		Assert( AUI_NEWOK( button, errcode ));
		if ( !AUI_NEWOK( button, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

		button->TextReloadFont();
		button->Enable(TRUE);

		aui_BitmapFont *font = button->GetTextFont();
		Assert(font);
		textlength = font->GetStringWidth(text);

		button->Resize( ( textlength + k_MODAL_BUTTON_TEXT_PADDING ), button->Height() );

		button->SetText( text );

		m_messageModalResponseButton->AddTail( button );


		if ( lastbutton ) {
			button->Move( lastbutton->X() -
						  button->Width() -
						  k_MODAL_BUTTON_SPACING, button->Y() );
		} else {




			button->Move(Width() - button->Width() - k_MODAL_BUTTON_SPACING - button->GetDim()->HorizontalPositionData(), button->Y());
		}

		action = new MessageModalResponseAction( &m_message, responseCount );
		Assert( action != NULL );
		if ( action == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

		m_messageModalResponseAction->AddTail( action );

		button->SetAction( action );

		AddControl( button );

		button->ResetThis();

		lastbutton = button;

		responseCount++;
	}

	return AUI_ERRCODE_OK;
}


MessageModal::~MessageModal ()
{
	if ( m_messageText ) {
		delete m_messageText;
		m_messageText = NULL;
	}






















	if ( m_messageModalResponseAction ) {
		MessageModalResponseAction *action = NULL;
		ListPos position = m_messageModalResponseAction->GetHeadPosition();

		for ( sint32 i = m_messageModalResponseAction->L(); i; i-- ) {
			action = m_messageModalResponseAction->GetNext( position );

			if ( action ) {
				delete action;
				action = NULL;
			}
		}

		m_messageModalResponseAction->DeleteAll();
		delete m_messageModalResponseAction;
		m_messageModalResponseAction = NULL;
	}

	if ( m_messageModalResponseButton ) {
		ctp2_Button *button = NULL;
		ListPos position = m_messageModalResponseButton->GetHeadPosition();

		for ( sint32 i = m_messageModalResponseButton->L(); i; i-- ) {
			button = m_messageModalResponseButton->GetNext( position );

			if ( button ) {
				delete button;
				button = NULL;
			}
		}

		m_messageModalResponseButton->DeleteAll();
		delete m_messageModalResponseButton;
		m_messageModalResponseButton = NULL;
	}

}


void MessageModal::MouseLGrabInside (aui_MouseEvent *mouseData)
{
	if ( IsDisabled() ) return;
	C3Window::MouseLGrabInside(mouseData);

	BringBorderToTop();

}


void MessageModal::MouseLDragAway (aui_MouseEvent *mouseData)
{
	if ( IsDisabled() ) return;
	C3Window::MouseLDragAway(mouseData);

	if ( m_topBar )
		m_topBar->Move( m_offsetTop.x + m_x, m_offsetTop.y + m_y );

	if ( m_bottomBar )
		m_bottomBar->Move( m_offsetBottom.x + m_x, m_offsetBottom.y + m_y );

}


void MessageModal::BringBorderToTop()
{
	if ( m_topBar )
		g_c3ui->BringWindowToTop( m_topBar );

	if ( m_bottomBar )
		g_c3ui->BringWindowToTop( m_bottomBar );
}


AUI_ERRCODE MessageModal::AddBordersToUI()
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


AUI_ERRCODE MessageModal::RemoveBordersFromUI()
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
