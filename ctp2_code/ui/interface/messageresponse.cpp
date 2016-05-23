#include "c3.h"

#include "aui.h"
#include "c3ui.h"
#include "aui_control.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_bitmapfont.h"
#include "tech_wllist.h"

#include "SlicButton.h"

#include "c3_button.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3listbox.h"
#include "ctp2_button.h"

#include "message.h"
#include "MessageData.h"
#include "messageactions.h"
#include "messagewindow.h"
#include "messageresponse.h"

#include "SlicSegment.h"
#include "CriticalMessagesPrefs.h"

extern C3UI			*g_c3ui;
extern uint8 g_messageRespButtonSpacing;
extern uint8 g_messageRespTextPadding;
extern uint8 g_messageRespButtonWidth;
extern uint8 g_messageRespDropPadding;


MessageResponseListItem::MessageResponseListItem(AUI_ERRCODE *retval,
    const MBCHAR *name, sint32 index, const MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(name, index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

AUI_ERRCODE MessageResponseListItem::InitCommonLdl(
    const MBCHAR *name, sint32 index, const MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	strcpy(m_name, name);
	m_index = index;

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "name");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	subItem->TextFlags() = k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER;
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void MessageResponseListItem::Update(void)
{
	c3_Static *subItem;

	subItem = (c3_Static *)GetChildByIndex(0);
	subItem->SetText(m_name);
}

sint32 MessageResponseListItem::Compare(c3_ListItem *item2, uint32 column)
{
	return 0;
}


















MessageResponseStandard::MessageResponseStandard(
	AUI_ERRCODE *retval,
	const MBCHAR *ldlBlock,
	MessageWindow *window )
{
	*retval = InitCommon( ldlBlock, window );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE MessageResponseStandard::InitCommon(const MBCHAR *ldlBlock, MessageWindow *window )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	ctp2_Button		*button, *lastbutton = NULL;
	MessageResponseAction	*action = NULL;
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	const MBCHAR	*text = NULL;
	sint32			responseCount = 0;
	SlicButton		*sButton = NULL;

	m_messageResponseButton = NULL;
	m_messageResponseAction = NULL;

	m_messageResponseButton = new tech_WLList<ctp2_Button *>;
	Assert( m_messageResponseButton != NULL );
	if ( m_messageResponseButton == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_messageResponseAction = new tech_WLList<MessageResponseAction *>;
	Assert( m_messageResponseAction != NULL );
	if ( m_messageResponseAction == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	uint32 textlength = g_messageRespButtonWidth;









	while ((sButton = window->GetMessage()->AccessData()->GetButton( responseCount ))) {
		text = sButton->GetName();

		sprintf(buttonBlock, "%s.%s", ldlBlock, "StandardResponseButton");
		button = new ctp2_Button( &errcode, aui_UniqueId(), buttonBlock );
		Assert( AUI_NEWOK( button, errcode ));
		if ( !AUI_NEWOK( button, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

		button->TextReloadFont();

		aui_BitmapFont *font = button->GetTextFont();
		Assert(font);
		textlength = font->GetStringWidth(text);
		if ( textlength < g_messageRespButtonWidth ) textlength = g_messageRespButtonWidth;

		button->Resize( ( textlength + ( g_messageRespTextPadding << 1 )), button->Height() );

		button->SetText( text );

		m_messageResponseButton->AddTail( button );


		if ( lastbutton ) {
			button->Move( lastbutton->X() -
						  button->Width() -
						  g_messageRespButtonSpacing, button->Y() );
		} else {

			button->Move( button->X() -
				( textlength - g_messageRespButtonWidth + g_messageRespTextPadding ),
				button->Y() );
		}

		action = new MessageResponseAction( window, responseCount );
		Assert( action != NULL );
		if ( action == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

		m_messageResponseAction->AddTail( action );

		button->SetAction( action );

		window->AddControl( button );

		button->Enable(TRUE);

		lastbutton = button;

		responseCount++;
	}

	m_dontShowButton=NULL;
	m_identifier=NULL;
	if(((MessageData*)window->GetMessage()->GetData())->GetSlicSegment())
	{
		if(g_theCriticalMessagesPrefs->IsEnabled(((MessageData*)window->GetMessage()->GetData())->GetSlicSegment()->GetName())>0)
		{
			sprintf(buttonBlock, "%s.%s", ldlBlock, "StandardDontShowButton");
			m_dontShowButton = new ctp2_Button( &errcode, aui_UniqueId(), buttonBlock );
			Assert( AUI_NEWOK( m_dontShowButton, errcode ));
			m_dontShowButton->SetActionFuncAndCookie(	DontShowButtonActionCallback, this);
			m_identifier=new MBCHAR[strlen(((MessageData*)window->GetMessage()->GetData())->GetSlicSegment()->GetName())+1];
			strcpy(m_identifier,((MessageData*)window->GetMessage()->GetData())->GetSlicSegment()->GetName());

			window->AddControl(m_dontShowButton);
		}
	}
	return AUI_ERRCODE_OK;
}


MessageResponseStandard::~MessageResponseStandard()
{

	if ( m_messageResponseAction ) {
		MessageResponseAction *action = NULL;
		ListPos position = m_messageResponseAction->GetHeadPosition();

		for ( sint32 i = m_messageResponseAction->L(); i; i-- ) {
			action = m_messageResponseAction->GetNext( position );

			if ( action ) {
				delete action;
				action = NULL;
			}
		}

		m_messageResponseAction->DeleteAll();
		delete m_messageResponseAction;
		m_messageResponseAction = NULL;
	}

	if ( m_messageResponseButton ) {
		ctp2_Button *button = NULL;
		ListPos position = m_messageResponseButton->GetHeadPosition();

		for ( sint32 i = m_messageResponseButton->L(); i; i-- ) {
			button = m_messageResponseButton->GetNext( position );

			if ( button ) {
				delete button;
				button = NULL;
			}
		}

		m_messageResponseButton->DeleteAll();
		delete m_messageResponseButton;
		m_messageResponseButton = NULL;
	}
	if(m_identifier)
	{
		delete m_identifier;
		m_identifier=NULL;
	}

	if(m_dontShowButton) delete m_dontShowButton;
}




















MessageResponseDropdown::MessageResponseDropdown(
	AUI_ERRCODE *retval,
	const MBCHAR *ldlBlock,
	MessageWindow *window )
{
	*retval = InitCommon( ldlBlock, window );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE MessageResponseDropdown::InitCommon(const MBCHAR *ldlBlock, MessageWindow *window )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	SlicButton		*sButton = NULL;
	uint32			i = 0;
	const MBCHAR	*text;

	m_action = NULL;
	m_submitButton = NULL;
	m_dropdown = NULL;







	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardResponseButton");
	m_submitButton = new ctp2_Button( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_submitButton, errcode ));
	if ( !AUI_NEWOK( m_submitButton, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_action = new MessageResponseSubmitAction( window );
	Assert( m_action != NULL );
	if ( m_action == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_submitButton->TextReloadFont();

	if (const MBCHAR *submitText = window->GetMessage()->AccessData()->GetSubmitString())
		m_submitButton->SetText( submitText );

	m_submitButton->SetAction( m_action );

	window->AddControl( m_submitButton );

	sint32 textlength = g_messageRespButtonWidth;
	i = 0;
	while ((sButton = window->GetMessage()->AccessData()->GetButton( i++ ))) {
		text = sButton->GetName();
		aui_BitmapFont *font = m_submitButton->GetTextFont();
		Assert(font);
		sint32 length = font->GetStringWidth(text);

		if ( length > textlength ) textlength = length;
	}

	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardResponseDropdown" );
	m_dropdown = new c3_DropDown( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_dropdown, errcode ));
	if ( !AUI_NEWOK( m_dropdown, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_action->SetDropdown( m_dropdown );

	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardResponseDropdownItem" );
	i = 0;
	while ((sButton = window->GetMessage()->AccessData()->GetButton( i++ ))) {

		text = sButton->GetName();

		MessageResponseListItem		*item;

		item = new MessageResponseListItem(&errcode, (char *)text, i, buttonBlock);

		if ( item )
			m_dropdown->AddItem((aui_Item *)item );

















	}

	m_dropdown->Offset( -m_dropdown->Width() - g_messageRespDropPadding, 0 );

	window->AddControl( m_dropdown );

	return AUI_ERRCODE_OK;
}


MessageResponseDropdown::~MessageResponseDropdown()
{

	if ( m_submitButton ) {
		delete m_submitButton;
		m_submitButton = NULL;
	}

	if ( m_action ) {
		delete m_action;
		m_action = NULL;
	}















	if ( m_dropdown ) {
		delete m_dropdown;
		m_dropdown = NULL;
	}

}

void MessageResponseStandard::DontShowButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{

	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	MessageResponseStandard *dialog =
		static_cast<MessageResponseStandard*>(cookie);

	dialog->m_dontShowButton->SetToggleState(!dialog->m_dontShowButton->GetToggleState());
	g_theCriticalMessagesPrefs->SetEnabled(dialog->m_identifier,!dialog->m_dontShowButton->GetToggleState());
}
