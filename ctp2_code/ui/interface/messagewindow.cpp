//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Message text box window.
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
// - Made message window non-modal. - Oct 16th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "c3ui.h"
#include "aui_button.h"
#include "aui_static.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"

#include "aui_hypertextbox.h"

#include "c3_popupwindow.h"

#include "StrDB.h"

#include "message.h"
#include "MessageData.h"
#include "messageactions.h"
#include "messageeyepoint.h"
#include "messageresponse.h"
#include "messageiconwindow.h"
#include "messagewindow.h"
#include "MessagePool.h"

#include "TurnYearStatus.h"

extern C3UI			*g_c3ui;
extern StringDB		*g_theStringDB;

MessageWindow		*g_currentMessageWindow = NULL;
extern MessagePool *g_theMessagePool;

extern sint32 g_ScreenWidth;
extern sint32 g_ScreenHeight;

MessageWindow::MessageWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	Message data,
	MessageIconWindow *iconWindow,
	AUI_WINDOW_TYPE type )
	:
	c3_PopupWindow(retval, id, ldlBlock, bpp, type)
{
	*retval = InitCommon( data, iconWindow );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE MessageWindow::InitCommon( Message data,
									   MessageIconWindow *iconWindow )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	m_messageText = NULL;
	m_iconWindow = iconWindow;
	m_message = data;
	m_minimizeIcon = NULL;
	m_dismissIcon = NULL;
	m_libraryIcon = NULL;

	m_leftBar = NULL;
	m_topBar = NULL;
	m_rightBar = NULL;
	m_bottomBar = NULL;





	m_minimizeAction = NULL;
	m_dismissAction = NULL;
	m_libraryAction = NULL;
	m_messageEyePoint.m_messageEyePointStandard = NULL;
	m_messageResponse.m_messageResponseStandard = NULL;

	m_player = data.GetOwner();

	SetDraggable( TRUE );

	strcpy( windowBlock, "StandardMessageWindow" );

	errcode = CreateStandardMinimizeButton( windowBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;







	errcode = CreateGreatLibraryButton( windowBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	errcode = CreateEyePointBox( windowBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	errcode = CreateResponses( windowBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;







	MakeSureSurfaceIsValid( );

	errcode = CreateStandardTextBox( windowBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	errcode = CreateTurnText( windowBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	data.AccessData()->SetMessageWindow( this );

	SetStronglyModal(TRUE);
	SetDynamic(TRUE);

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE MessageWindow::CreateWindowEdges( MBCHAR *ldlBlock )
{
	MBCHAR			imageBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	sprintf( imageBlock, "%s.%s", ldlBlock, "MessageLeftBar" );
	m_leftBar = new aui_Static( &errcode, aui_UniqueId(), imageBlock );
	Assert( AUI_NEWOK( m_leftBar, errcode ));
	if ( !AUI_NEWOK( m_leftBar, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	sprintf( imageBlock, "%s.%s", ldlBlock, "MessageTopBar" );
	m_topBar = new aui_Static( &errcode, aui_UniqueId(), imageBlock );
	Assert( AUI_NEWOK( m_topBar, errcode ));
	if ( !AUI_NEWOK( m_topBar, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;
	m_topBar->SetImageBltType( AUI_IMAGEBASE_BLTTYPE_TILE );

	sprintf( imageBlock, "%s.%s", ldlBlock, "MessageRightBar" );
	m_rightBar = new aui_Static( &errcode, aui_UniqueId(), imageBlock );
	Assert( AUI_NEWOK( m_rightBar, errcode ));
	if ( !AUI_NEWOK( m_rightBar, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	sprintf( imageBlock, "%s.%s", ldlBlock, "MessageBottomBar" );
	m_bottomBar = new aui_Static( &errcode, aui_UniqueId(), imageBlock );
	Assert( AUI_NEWOK( m_bottomBar, errcode ));
	if ( !AUI_NEWOK( m_bottomBar, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;
	m_bottomBar->SetImageBltType( AUI_IMAGEBASE_BLTTYPE_TILE );













































	AddControl( m_leftBar );
	AddControl( m_topBar );
	AddControl( m_rightBar );
	AddControl( m_bottomBar );


	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageWindow::CreateTurnText( MBCHAR *ldlBlock )
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	sprintf( textBlock, "%s.%s", ldlBlock, "TurnTextBox" );
	m_turnText = new aui_Static( &errcode, aui_UniqueId(), textBlock );
	Assert( AUI_NEWOK( m_turnText, errcode ));
	if ( !AUI_NEWOK( m_turnText, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_turnText->TextFlags() = k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT | k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP;

	AddControl( m_turnText );

	MBCHAR	copyBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if(m_message.AccessData()->GetTitle()) {
		m_turnText->SetTextFontSize(16);
		strcpy(copyBlock, m_message.AccessData()->GetTitle());
	} else {

		if(g_useCustomYear && g_pTurnLengthOverride)
		{
			copyBlock[0]=0;
		}
		else
		{
			sint32 year = m_message.AccessData()->GetTimeStamp();

			if ( year < 0 ) {
				year *= -1;
				sprintf( copyBlock, "%d %s", year, g_theStringDB->GetNameStr("str_tbl_ldl_BC") );
			}
			else {
				sprintf( copyBlock, "%d %s", year, g_theStringDB->GetNameStr("str_tbl_ldl_AD") );
			}
		}

	}

	errcode = ((aui_TextBase *)m_turnText)->SetText( copyBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageWindow::CreateStandardTextBox( MBCHAR *ldlBlock )
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	sprintf( textBlock, "%s.%s", ldlBlock, "MessageTextBox" );

	m_messageText = new aui_HyperTextBox( &errcode, aui_UniqueId(), textBlock );
	Assert( AUI_NEWOK( m_messageText, errcode ));
	if ( !AUI_NEWOK( m_messageText, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_messageText->TextFlags() = k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT | k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP;

	AddControl( m_messageText );


	errcode = m_messageText->SetHyperText( m_message.GetText( ));
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;


	return AUI_ERRCODE_OK;
}

AUI_ERRCODE MessageWindow::CreateStandardDismissButton( MBCHAR *ldlBlock )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardDismissButton" );
	m_dismissIcon = new aui_Button( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_dismissIcon, errcode ));
	if ( !AUI_NEWOK( m_dismissIcon, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	errcode = AddControl( m_dismissIcon );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	m_dismissAction = new MessageDismissAction( this );
	Assert( m_dismissAction != NULL );
	if ( m_dismissAction == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_dismissIcon->SetAction( m_dismissAction );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageWindow::CreateStandardMinimizeButton( MBCHAR *ldlBlock )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardMinimizeButton" );
	m_minimizeIcon = new aui_Button( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_minimizeIcon, errcode ));
	if ( !AUI_NEWOK( m_minimizeIcon, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	errcode = AddControl( m_minimizeIcon );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	m_minimizeAction = new MessageMinimizeAction( this );
	Assert( m_minimizeAction != NULL );
	if ( m_minimizeAction == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_minimizeIcon->SetAction( m_minimizeAction );

	m_minimizeIcon->Move(Width() - m_minimizeIcon->Width() - 22, 20);

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageWindow::CreateGreatLibraryButton( MBCHAR *ldlBlock )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( g_theMessagePool->IsValid(m_message) &&
		 m_message.AccessData()->GetGreatLibraryButton() ) {

		sprintf( buttonBlock, "%s.%s", ldlBlock, "GreatLibraryButton" );
		m_libraryIcon = new aui_Button( &errcode, aui_UniqueId(), buttonBlock );
		Assert( AUI_NEWOK( m_libraryIcon, errcode ));
		if ( !AUI_NEWOK( m_libraryIcon, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

		errcode = AddControl( m_libraryIcon );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return errcode;

		m_libraryAction = new MessageLibraryAction( this );
		Assert( m_libraryAction != NULL );
		if ( m_libraryAction == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

		m_libraryIcon->SetAction( m_libraryAction );
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageWindow::CreateEyePointBox( MBCHAR *ldlBlock )
{
	if(!g_theMessagePool->IsValid(m_message))
		return AUI_ERRCODE_OK;

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


AUI_ERRCODE MessageWindow::CreateStandardEyePointBox( MBCHAR *ldlBlock )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	m_messageEyePoint.m_messageEyePointStandard = new MessageEyePointStandard( &errcode,
						ldlBlock, this );
	Assert( m_messageEyePoint.m_messageEyePointStandard != NULL );
	if ( m_messageEyePoint.m_messageEyePointStandard == NULL )
		return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageWindow::CreateDropdownEyePointBox( MBCHAR *ldlBlock )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	m_messageEyePoint.m_messageEyePointDropdown = new MessageEyePointDropdown( &errcode,
						ldlBlock, this );
	Assert( m_messageEyePoint.m_messageEyePointDropdown != NULL );
	if ( m_messageEyePoint.m_messageEyePointDropdown == NULL )
		return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageWindow::CreateListboxEyePointBox( MBCHAR *ldlBlock )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	m_messageEyePoint.m_messageEyePointListbox = new MessageEyePointListbox( &errcode,
						ldlBlock, this );
	Assert( m_messageEyePoint.m_messageEyePointListbox != NULL );
	if ( m_messageEyePoint.m_messageEyePointListbox == NULL )
		return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageWindow::CreateResponses( MBCHAR *ldlBlock )
{
	if(!g_theMessagePool->IsValid(m_message))
		return AUI_ERRCODE_OK;

	switch( m_message.AccessData()->GetResponseStyle() )
	{
	case MESSAGE_RESPONSE_STYLE_NONE:
		return AUI_ERRCODE_OK;
	case MESSAGE_RESPONSE_STYLE_SUBMIT:
		return CreateSubmitResponses( ldlBlock );
	case MESSAGE_RESPONSE_STYLE_SELECT:
		return CreateSelectResponses( ldlBlock );
	default:
		return AUI_ERRCODE_OK;
	}
}


AUI_ERRCODE MessageWindow::CreateSelectResponses( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	m_messageResponse.m_messageResponseStandard = new MessageResponseStandard( &errcode,
						ldlBlock, this );
	Assert( m_messageResponse.m_messageResponseStandard != NULL );
	if ( m_messageResponse.m_messageResponseStandard == NULL )
		return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE MessageWindow::CreateSubmitResponses( MBCHAR *ldlBlock )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	m_messageResponse.m_messageResponseDropdown = new MessageResponseDropdown( &errcode,
						ldlBlock, this );
	Assert( m_messageResponse.m_messageResponseDropdown != NULL );
	if ( m_messageResponse.m_messageResponseDropdown == NULL )
		return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageWindow::ShowWindow( BOOL show )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	if ( show ) {











		sint32 x,y;

		if(g_ScreenWidth < 1024) {
			x = 0,
			y = 50;
		} else {
			x = 0,
			y = 32;
		}

		Move(x, y);

		errcode = g_c3ui->AddWindow( this );
		Assert(errcode == AUI_ERRCODE_OK);

		g_currentMessageWindow = this;

		AddBordersToUI();
		Show();
	} else {
		errcode = g_c3ui->RemoveWindow( this->Id() );
		if ( g_currentMessageWindow == this ) {
			g_currentMessageWindow = NULL;
		}
		RemoveBordersFromUI();
	}

	Assert(errcode == AUI_ERRCODE_OK);
	return errcode;
}


MessageOpenAction *MessageWindow::GetMessageOpenAction( void )
{
	return m_iconWindow->GetMessageOpenAction();
}
























MessageWindow::~MessageWindow ()
{
	if ( g_c3ui->GetWindow( this->Id() ))
		g_c3ui->RemoveWindow( this->Id());

	RemoveBordersFromUI();

	if ( m_messageText ) {
		delete m_messageText;
		m_messageText = NULL;
	}

	if ( m_turnText ) {
		delete m_turnText;
		m_turnText = NULL;
	}

	if (g_theMessagePool->IsValid(m_message)) {

		m_message.AccessData()->SetMessageWindow(NULL);
	}









































	if ( m_minimizeIcon )
	{
		delete m_minimizeIcon;
		m_minimizeIcon = NULL;
	}

	if ( m_minimizeAction )
	{
		delete m_minimizeAction;
		m_minimizeAction = NULL;
	}

	if ( m_dismissIcon )
	{
		delete m_dismissIcon;
		m_dismissIcon = NULL;
	}

	if ( m_dismissAction )
	{
		delete m_dismissAction;
		m_dismissAction = NULL;
	}

	if ( m_libraryIcon )
	{
		delete m_libraryIcon;
		m_libraryIcon = NULL;
	}

	if ( m_libraryAction )
	{
		delete m_libraryAction;
		m_libraryAction = NULL;
	}

	if ( m_messageEyePoint.m_messageEyePointStandard )
	{
		delete m_messageEyePoint.m_messageEyePointStandard;
		m_messageEyePoint.m_messageEyePointStandard = NULL;
	}

	if ( m_messageResponse.m_messageResponseStandard )
	{
		delete m_messageResponse.m_messageResponseStandard;
		m_messageResponse.m_messageResponseStandard = NULL;
	}


	if(g_currentMessageWindow == this) {
		g_currentMessageWindow = NULL;
	}
}

void MessageWindow::BringBorderToTop()
{











}


AUI_ERRCODE MessageWindow::AddBordersToUI()
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;





























	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageWindow::RemoveBordersFromUI()
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;





























	return AUI_ERRCODE_OK;
}
