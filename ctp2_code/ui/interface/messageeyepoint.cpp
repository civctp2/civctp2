#include "c3.h"

#include "aui.h"
#include "c3ui.h"
#include "aui_control.h"
#include "aui_imagebase.h"
#include "aui_textbase.h"
#include "aui_button.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_bitmapfont.h"

#include "c3_dropdown.h"
#include "c3_listbox.h"
#include "c3_static.h"

#include "message.h"
#include "MessageData.h"
#include "messageactions.h"
#include "messagewindow.h"
#include "messagemodal.h"
#include "messageeyepoint.h"

extern C3UI			*g_c3ui;
extern uint8		g_messageEyeDropWidth;
extern uint8		g_messageEyeGreatPadding;


MessageEyePointListItem::MessageEyePointListItem(AUI_ERRCODE *retval, MBCHAR *name, sint32 index, MBCHAR *ldlBlock)
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

AUI_ERRCODE MessageEyePointListItem::InitCommonLdl(MBCHAR *name, sint32 index, MBCHAR *ldlBlock)
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

void MessageEyePointListItem::Update(void)
{
	c3_Static *subItem;

	subItem = (c3_Static *)GetChildByIndex(0);
	subItem->SetText(m_name);
}

sint32 MessageEyePointListItem::Compare(c3_ListItem *item2, uint32 column)
{
	return 0;
}



















MessageEyePointStandard::MessageEyePointStandard(
	AUI_ERRCODE *retval,
	MBCHAR *ldlBlock,
	MessageWindow *window )
{
	*retval = InitCommon( ldlBlock, window );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


MessageEyePointStandard::MessageEyePointStandard(
	AUI_ERRCODE *retval,
	MBCHAR *ldlBlock,
	MessageModal *window )
{
	*retval = InitCommon( ldlBlock, window );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE MessageEyePointStandard::InitCommon( MBCHAR *ldlBlock, MessageWindow *window )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	m_button = NULL;
	m_action = NULL;

	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardEyeButton" );
	m_button = new aui_Button( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_button, errcode ));
	if ( !AUI_NEWOK( m_button, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_action = new MessageStandardEyePointAction( window );
	Assert( m_action != NULL );
	if ( m_action == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_button->SetAction( m_action );

	if ( window->GetGreatLibraryButton() ) {
		m_button->Offset( window->GetGreatLibraryButton()->Width() + 3, 0 );
	}

	errcode = window->AddControl( m_button );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageEyePointStandard::InitCommon( MBCHAR *ldlBlock, MessageModal *window )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	m_button = NULL;
	m_action = NULL;

	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardEyeButton" );
	m_button = new aui_Button( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_button, errcode ));
	if ( !AUI_NEWOK( m_button, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_action = new MessageStandardEyePointAction( NULL, window );
	Assert( m_action != NULL );
	if ( m_action == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_button->SetAction( m_action );







	errcode = window->AddControl( m_button );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	return AUI_ERRCODE_OK;
}


MessageEyePointStandard::~MessageEyePointStandard ()
{
	if ( m_button )
	{
		delete m_button;
		m_button = NULL;
	}

	if ( m_action )
	{
		delete m_action;
		m_action = NULL;
	}
}





















MessageEyePointDropdown::MessageEyePointDropdown(
	AUI_ERRCODE *retval,
	MBCHAR *ldlBlock,
	MessageWindow *window )
{
	*retval = InitCommon( ldlBlock, window );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


MessageEyePointDropdown::MessageEyePointDropdown(
	AUI_ERRCODE *retval,
	MBCHAR *ldlBlock,
	MessageModal *window )
{
	*retval = InitCommon( ldlBlock, window );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE MessageEyePointDropdown::InitCommon( MBCHAR *ldlBlock, MessageWindow *window )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	c3_Static		*item = NULL;
	const MBCHAR	*text = NULL;
	uint32			i = 0;

	m_button = NULL;
	m_action = NULL;
	m_dropdown = NULL;
	m_dropaction = NULL;

	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardEyeButton" );
	m_button = new aui_Button( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_button, errcode ));
	if ( !AUI_NEWOK( m_button, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_button->TextReloadFont();

	m_action = new MessageDropdownEyePointAction( window );
	Assert( m_action != NULL );
	if ( m_action == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_button->SetAction( m_action );

	if ( window->GetGreatLibraryButton() ) {
		m_button->Offset( window->GetGreatLibraryButton()->Width() + 3, 0 );
	}

	errcode = window->AddControl( m_button );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	i = 0;
	sint32 textlength = g_messageEyeDropWidth;
	while ((text = window->GetMessage()->AccessData()->GetEyePointName( i++ ))) {

		aui_BitmapFont *font = m_button->GetTextFont();
		Assert(font);
		sint32 length = font->GetStringWidth(text);

		if ( length > textlength ) textlength = length;
	}

	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardEyePointDropdown" );
	m_dropdown = new c3_DropDown( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_dropdown, errcode ));
	if ( !AUI_NEWOK( m_dropdown, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_action->SetDropdown( m_dropdown );

	m_dropaction = new MessageDropdownAction( window, m_dropdown );
	Assert( m_dropaction != NULL );
	if ( m_dropaction == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_dropdown->SetAction( m_dropaction );

	if (m_dropdown->GetListBox())
		m_dropdown->GetListBox()->SetForceSelect(FALSE);

	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardEyePointDropdownItem" );
	i = 0;
	while ((text = window->GetMessage()->AccessData()->GetEyePointName( i++ ))) {

		MessageEyePointListItem		*item;

		item = new MessageEyePointListItem(&errcode, (char *)text, i, buttonBlock);

		if ( item )
			m_dropdown->AddItem((aui_Item *)item );

	}

	if ( window->GetGreatLibraryButton() ) {
		m_dropdown->Offset( window->GetGreatLibraryButton()->Width() +
							g_messageEyeGreatPadding, 0 );
	}

	window->AddControl( m_dropdown );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageEyePointDropdown::InitCommon( MBCHAR *ldlBlock, MessageModal *window )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	c3_Static		*item = NULL;
	const MBCHAR	*text = NULL;
	uint32			i = 0;

	m_button = NULL;
	m_action = NULL;
	m_dropdown = NULL;
	m_dropaction = NULL;

	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardEyeButton" );
	m_button = new aui_Button( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_button, errcode ));
	if ( !AUI_NEWOK( m_button, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_button->TextReloadFont();

	m_action = new MessageDropdownEyePointAction( NULL, window );
	Assert( m_action != NULL );
	if ( m_action == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_button->SetAction( m_action );







	errcode = window->AddControl( m_button );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	i = 0;
	sint32 textlength = g_messageEyeDropWidth;
	while ((text = window->GetMessage()->AccessData()->GetEyePointName( i++ ))) {

		aui_BitmapFont *font = m_button->GetTextFont();
		Assert(font);
		sint32 length = font->GetStringWidth(text);

		if ( length > textlength ) textlength = length;
	}

	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardEyePointDropdown" );
	m_dropdown = new c3_DropDown( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_dropdown, errcode ));
	if ( !AUI_NEWOK( m_dropdown, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_action->SetDropdown( m_dropdown );

	m_dropaction = new MessageDropdownAction( NULL, m_dropdown, window );
	Assert( m_dropaction != NULL );
	if ( m_dropaction == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_dropdown->SetAction( m_dropaction );

	if (m_dropdown->GetListBox())
		m_dropdown->GetListBox()->SetForceSelect(FALSE);

	sprintf( buttonBlock, "%s.%s", ldlBlock, "StandardEyePointDropdownItem" );
	i = 0;
	while ((text = window->GetMessage()->AccessData()->GetEyePointName( i++ ))) {

		MessageEyePointListItem		*item;

		item = new MessageEyePointListItem(&errcode, (char *)text, i, buttonBlock);

		if ( item )
			m_dropdown->AddItem((aui_Item *)item );

	}







	window->AddControl( m_dropdown );

	return AUI_ERRCODE_OK;
}


MessageEyePointDropdown::~MessageEyePointDropdown()
{
	if ( m_button )
	{
		delete m_button;
		m_button = NULL;
	}

	if ( m_action )
	{
		delete m_action;
		m_action = NULL;
	}

	if ( m_dropaction )
	{
		delete m_dropaction;
		m_dropaction = NULL;
	}

	if ( m_dropdown ) {
		delete m_dropdown;
		m_dropdown = NULL;
	}

}





















MessageEyePointListbox::MessageEyePointListbox(
	AUI_ERRCODE *retval,
	MBCHAR *ldlBlock,
	MessageWindow *window )
{
	*retval = InitCommon( ldlBlock, window );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


MessageEyePointListbox::MessageEyePointListbox(
	AUI_ERRCODE *retval,
	MBCHAR *ldlBlock,
	MessageModal *window )
{
	*retval = InitCommon( ldlBlock, window );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE MessageEyePointListbox::InitCommon( MBCHAR *ldlBlock, MessageWindow *window )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32			count = 0;

	m_buttonLeft = NULL;
	m_buttonRight = NULL;
	m_action1 = NULL;
	m_action2 = NULL;


	count = window->GetMessage()->AccessData()->GetNumEyePoints( );

	sprintf( buttonBlock, "%s.%s", ldlBlock, "EyeLeftButton" );
	m_buttonLeft = new aui_Button( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_buttonLeft, errcode ));
	if ( !AUI_NEWOK( m_buttonLeft, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_action1 = new MessageListboxEyePointAction( window, 0, -1, 0, count );
	Assert( m_action1 != NULL );
	if ( m_action1 == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_buttonLeft->SetAction( m_action1 );

	if ( window->GetGreatLibraryButton() ) {
		m_buttonLeft->Offset( window->GetGreatLibraryButton()->Width() + 3, 0 );
	}

	errcode = window->AddControl( m_buttonLeft );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	sprintf( buttonBlock, "%s.%s", ldlBlock, "EyeRightButton" );
	m_buttonRight = new aui_Button( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_buttonRight, errcode ));
	if ( !AUI_NEWOK( m_buttonRight, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_action2 = new MessageListboxEyePointAction( window, 0, 1, 0, count );
	Assert( m_action2 != NULL );
	if ( m_action2 == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_buttonRight->SetAction( m_action2 );

	if ( window->GetGreatLibraryButton() ) {
		m_buttonRight->Offset( window->GetGreatLibraryButton()->Width() + 3, 0 );
	}

	errcode = window->AddControl( m_buttonRight );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;


	m_action1->SetAction( m_action2 );
	m_action2->SetAction( m_action1 );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE MessageEyePointListbox::InitCommon( MBCHAR *ldlBlock, MessageModal *window )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32			count = 0;

	m_buttonLeft = NULL;
	m_buttonRight = NULL;
	m_action1 = NULL;
	m_action2 = NULL;


	count = window->GetMessage()->AccessData()->GetNumEyePoints( );

	sprintf( buttonBlock, "%s.%s", ldlBlock, "EyeLeftButton" );
	m_buttonLeft = new aui_Button( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_buttonLeft, errcode ));
	if ( !AUI_NEWOK( m_buttonLeft, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_action1 = new MessageListboxEyePointAction( NULL, 0, -1, 0, count, window );
	Assert( m_action1 != NULL );
	if ( m_action1 == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_buttonLeft->SetAction( m_action1 );







	errcode = window->AddControl( m_buttonLeft );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;

	sprintf( buttonBlock, "%s.%s", ldlBlock, "EyeRightButton" );
	m_buttonRight = new aui_Button( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK( m_buttonRight, errcode ));
	if ( !AUI_NEWOK( m_buttonRight, errcode )) return AUI_ERRCODE_MEMALLOCFAILED;

	m_action2 = new MessageListboxEyePointAction( NULL, 0, 1, 0, count, window );
	Assert( m_action2 != NULL );
	if ( m_action2 == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_buttonRight->SetAction( m_action2 );







	errcode = window->AddControl( m_buttonRight );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return errcode;


	m_action1->SetAction( m_action2 );
	m_action2->SetAction( m_action1 );

	return AUI_ERRCODE_OK;
}


MessageEyePointListbox::~MessageEyePointListbox()
{
	if ( m_buttonLeft )
	{
		delete m_buttonLeft;
		m_buttonLeft = NULL;
	}

	if ( m_buttonRight )
	{
		delete m_buttonRight;
		m_buttonRight = NULL;
	}

	if ( m_action1 )
	{
		delete m_action1;
		m_action1 = NULL;
	}

	if ( m_action2 )
	{
		delete m_action2;
		m_action2 = NULL;
	}

}
