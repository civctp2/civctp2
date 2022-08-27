//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The civilization 3 utility dialog box
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Fixed memory leaks.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3_utilitydialogbox.h"

#include "aui.h"
#include "aui_ldl.h"
#include "aui_listbox.h"
#include "aui_static.h"
#include "aui_uniqueid.h"
#include "BuildingRecord.h"
#include "c3_button.h"
#include "c3_listbox.h"
#include "c3_popupwindow.h"
#include "c3_static.h"
#include "c3textfield.h"
#include "c3ui.h"
#include "c3window.h"
#include "c3windows.h"
#include "citydata.h"
#include "ctp2_button.h"
#include "ctp2_listitem.h"
#include "ctp2_listbox.h"
#include "Globals.h"                // allocated::...
#include "keypress.h"
#include "MainControlPanel.h"
#include "network.h"
#include "player.h"                 // g_player
#include "SelItem.h"                // g_selected_item
#include "TerrainRecord.h"
#include "textbutton.h"
#include "thermometer.h"
#include "UIUtils.h"
#include "Unit.h"
#include "UnitData.h"
#include "UnitDynArr.h"
#include "WonderRecord.h"
#include "World.h"

extern C3UI	*       g_c3ui;
extern sint32   	g_modalWindow;
extern sint32		g_ScreenWidth;
extern sint32		g_ScreenHeight;

c3_ExpelPopup *                 g_expelPopup            = NULL;
c3_UtilityAbortPopup *          g_utilityAbort          = NULL;
c3_UtilityTextMessagePopup *    g_utilityTextMessage    = NULL;


void C3UtilityCityListButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	c3_UtilityCityListPopup *popup = (c3_UtilityCityListPopup *)cookie;
	if (!popup) return;

	if ((ctp2_Button*)control == popup->m_window->Ok())
	{

		SingleListItem *item = (SingleListItem *) popup->m_list->GetSelectedItem();
		if (!item) return;

//      sint32 cityIndex = item->GetValue();





		Assert(FALSE);
		if (popup->m_callback)
		{

			popup->RemoveWindow();
		}
	}
	if ((c3_Button*)control == popup->m_window->Cancel())
	{
		if (popup->m_callback)
			popup->m_callback(Unit(), FALSE);

		popup->RemoveWindow();
	}
}

void C3PiracyButtonCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	c3_PiracyPopup *popup = (c3_PiracyPopup *)cookie;
	if (!popup) return;

	if ((c3_Button*)control == popup->m_pirate)
	{







		if (popup->m_callback)
		{

			popup->RemoveWindow();
		}
	}
	if ((c3_Button*)control == popup->m_cancel)
	{




		popup->RemoveWindow();
	}
}

void C3ExpelButtonCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	c3_ExpelPopup *popup = (c3_ExpelPopup *)cookie;
	if (!popup) return;

	if ((c3_Button*)control == popup->m_attack)
	{
		if (popup->m_callback)
			popup->m_callback( EXPEL_ACTION_ATTACK );

		popup->RemoveWindow();

	}

	if ((c3_Button*)control == popup->m_expel)
	{
		if (popup->m_callback)
			popup->m_callback( EXPEL_ACTION_EXPEL );

		popup->RemoveWindow();
	}

	if ((c3_Button*)control == popup->m_cancel)
	{
		if (popup->m_callback)
			popup->m_callback( EXPEL_ACTION_CANCEL );

		popup->RemoveWindow();
	}
}


void C3UtilityTextFieldButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE &&
		action != (uint32)AUI_TEXTFIELD_ACTION_EXECUTE) return;

	c3_UtilityTextFieldPopup *popup = (c3_UtilityTextFieldPopup *)cookie;
	if (!popup) return;

	if ((ctp2_Button*)control == popup->m_window->Ok() ||
			(((aui_TextField *)control == popup->m_text) &&
				(action == AUI_TEXTFIELD_ACTION_EXECUTE)))
	{

		MBCHAR *resultText = new MBCHAR[256];

		popup->m_text->GetFieldText(resultText, 256);

		if (!strlen(resultText))
		{

			delete[] resultText;
			resultText = NULL;
			if(popup->m_wantEmpties) {
				popup->m_callback(resultText, TRUE, popup->GetData());
				popup->RemoveWindow();
			}
			return;
		}
		else if (popup->m_callback)
		{
			popup->m_callback(resultText, TRUE, popup->GetData());

			popup->RemoveWindow();
		}
	}
	if ((c3_Button*)control == popup->m_window->Cancel())
	{
		if (popup->m_callback)
			popup->m_callback(NULL, FALSE, popup->GetData());

		popup->RemoveWindow();
	}
}

void C3UtilityTextMessageButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	c3_UtilityTextMessagePopup *popup = (c3_UtilityTextMessagePopup *)cookie;
	if (!popup) return;

	if ((ctp2_Button*)control == popup->m_window->Ok())
	{
		if (popup->m_callback)
			popup->m_callback( TRUE );

		popup->RemoveWindow();

	}

	if ( popup->m_type )
	{
		if ((c3_Button*)control == popup->m_window->Cancel())
		{
			if (popup->m_callback)
				popup->m_callback( FALSE );

			popup->RemoveWindow();
		}
	}
}

void C3AbortButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	c3_UtilityAbortPopup *popup = (c3_UtilityAbortPopup *)cookie;
	if (!popup) return;

	if ((ctp2_Button*)control == popup->m_abort)
	{
		if (popup->m_callback)
			popup->m_callback( FALSE );

		popup->RemoveWindow();

	}
}

void C3UtilityPlayerListButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	c3_UtilityPlayerListPopup *popup = (c3_UtilityPlayerListPopup *)cookie;
	if (!popup) return;

	if ((ctp2_Button*)control == popup->m_kick)
	{

		DoubleListItem *item = (DoubleListItem *) popup->m_list->GetSelectedItem();
		if (!item) return;

		sint32 playerIndex = item->GetValue();

		if (popup->m_callback)
		{
			popup->m_callback( playerIndex, TRUE, PLAYER_ACTION_KICK );
		}
	}
	if ((ctp2_Button*)control == popup->m_open)
	{

		DoubleListItem *item = (DoubleListItem *) popup->m_list->GetSelectedItem();
		if (!item) return;

		sint32 playerIndex = item->GetValue();

		if (popup->m_callback)
		{
			popup->m_callback( playerIndex, TRUE, PLAYER_ACTION_OPEN );
		}
	}
	if ((ctp2_Button*)control == popup->m_close)
	{

		DoubleListItem *item = (DoubleListItem *) popup->m_list->GetSelectedItem();
		if (!item) return;

		sint32 playerIndex = item->GetValue();

		if (popup->m_callback)
		{
			popup->m_callback( playerIndex, TRUE, PLAYER_ACTION_CLOSE );
		}
	}
	if ((ctp2_Button*)control == popup->m_abort)
	{
		if (popup->m_callback)
			popup->m_callback( -1, FALSE, PLAYER_ACTION_MAX );

		popup->RemoveWindow();
	}
}












c3_UtilityCityListPopup::c3_UtilityCityListPopup( c3_UtilityCityListCallback *callback, MBCHAR *ldlBlock )
:
	m_window        (NULL),
	m_title_label   (NULL),
	m_list          (NULL),
	m_ok            (NULL),
	m_cancel        (NULL),
	m_callback      (callback)
{
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if (ldlBlock) strcpy(windowBlock,ldlBlock);
	else strcpy(windowBlock,"DefaultUtilityCityListPopup");

	{
	    AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

	Initialize(windowBlock);
}

sint32 c3_UtilityCityListPopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( controlBlock, "%s.%s", windowBlock, "TitleLabel");
	m_window->AddTitle( controlBlock );
	m_window->AddCancel(C3UtilityCityListButtonActionCallback, this);
	m_window->AddOk(C3UtilityCityListButtonActionCallback, this);


















	sprintf( controlBlock, "%s.%s", windowBlock, "CityList" );
	m_list = new ctp2_ListBox(&errcode, aui_UniqueId(), controlBlock, NULL, NULL);
	Assert( AUI_NEWOK(m_list, errcode) );
	if ( !AUI_NEWOK(m_list, errcode) ) return -1;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}

c3_UtilityCityListPopup::~c3_UtilityCityListPopup( void )
{
    if (m_list)
    {
        m_list->Clear();
        delete m_list;
    }
    delete m_title_label;
    delete m_ok;
    delete m_cancel;
    delete m_window;
}

void c3_UtilityCityListPopup::Cleanup(void)
{
    if (m_list)
    {
        m_list->Clear();
        allocated::clear(m_list);
    }
    allocated::clear(m_title_label);
    allocated::clear(m_ok);
    allocated::clear(m_cancel);
    allocated::clear(m_window);
	m_callback = NULL;
}

void c3_UtilityCityListPopup::DisplayWindow( void )
{
	UpdateData();

	AUI_ERRCODE auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );
}

void c3_UtilityCityListPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
}

sint32 c3_UtilityCityListPopup::UpdateData( void )
{
	MBCHAR strbuf[256];
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32 curPlayer = g_selected_item->GetVisiblePlayer();

	AUI_ERRCODE		retval;

	UnitDynamicArray *cityList = g_player[curPlayer]->GetAllCitiesList();

	strcpy(ldlBlock,"SingleListItem");
	m_list->Clear();

	for ( sint32 i = 0 ; i < cityList->Num() ; i++ )
	{
		strcpy(strbuf, (*cityList)[i].GetData()->GetCityData()->GetName());
		m_list->AddItem(new SingleListItem(&retval, strbuf, i, ldlBlock));
	}

	return 0;
}




c3_PiracyPopup::c3_PiracyPopup( c3_PiracyCallback *callback, MBCHAR *ldlBlock )
:
    m_window            (NULL),
    m_title_label       (NULL),
    m_list              (NULL),
    m_pirate            (NULL),
    m_cancel            (NULL),
	m_callback          (callback)
{
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if (ldlBlock) strcpy(windowBlock,ldlBlock);
	else strcpy(windowBlock,"DefaultPiracyPopup");

	{
	    AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

    Initialize( windowBlock );
}

sint32 c3_PiracyPopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( controlBlock, "%s.%s", windowBlock, "PirateButton" );
	m_pirate = new c3_Button(&errcode, aui_UniqueId(), controlBlock, C3PiracyButtonCallback, this);
	Assert( AUI_NEWOK(m_pirate, errcode) );
	if ( !AUI_NEWOK(m_pirate, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "CancelButton" );
	m_cancel = new c3_Button(&errcode, aui_UniqueId(), controlBlock, C3PiracyButtonCallback, this);
	Assert( AUI_NEWOK(m_cancel, errcode) );
	if ( !AUI_NEWOK(m_cancel, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "TitleLabel");
	m_title_label = new c3_Static( &errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(m_title_label, errcode) );
	if ( !AUI_NEWOK(m_title_label, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "PlayerList" );
	m_list = new ctp2_ListBox(&errcode, aui_UniqueId(), controlBlock, NULL, NULL);
	Assert( AUI_NEWOK(m_list, errcode) );
	if ( !AUI_NEWOK(m_list, errcode) ) return -1;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}

c3_PiracyPopup::~c3_PiracyPopup( void )
{
    if (m_list)
    {
        m_list->Clear();
        delete m_list;
    }
    delete m_title_label;
    delete m_pirate;
    delete m_cancel;
    delete m_window;
}

void c3_PiracyPopup::Cleanup( void )
{
    if (m_list)
    {
        m_list->Clear();
        allocated::clear(m_list);
    }
    allocated::clear(m_title_label);
    allocated::clear(m_pirate);
    allocated::clear(m_cancel);
    allocated::clear(m_window);
	m_callback = NULL;
}

void c3_PiracyPopup::DisplayWindow( void )
{
	UpdateData();

	AUI_ERRCODE auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );
}

void c3_PiracyPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
}

sint32 c3_PiracyPopup::UpdateData( void )
{
	MBCHAR strbuf[256];
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32 curPlayer = g_selected_item->GetVisiblePlayer();

	AUI_ERRCODE		retval;

	UnitDynamicArray *cityList = g_player[curPlayer]->GetAllCitiesList();

	strcpy(ldlBlock,"PiracyListItem");

    m_list->Clear();

	for ( sint32 i = 0 ; i < cityList->Num() ; i++ )
	{
		strcpy(strbuf, (*cityList)[i].GetData()->GetCityData()->GetName());
		m_list->AddItem(new SingleListItem(&retval, strbuf, i, ldlBlock));
	}

	return 0;
}




c3_ExpelPopup::c3_ExpelPopup( c3_ExpelCallback *callback, MBCHAR *ldlBlock )
:
	m_window            (NULL),
	m_title_label       (NULL),
	m_attack            (NULL),
	m_expel             (NULL),
	m_cancel            (NULL),
	m_callback          (callback)
{
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

    if (ldlBlock) strcpy(windowBlock,ldlBlock);
	else strcpy(windowBlock,"DefaultExpelPopup");

	{
	    AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

	Initialize( windowBlock );
}

sint32 c3_ExpelPopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( controlBlock, "%s.%s", windowBlock, "AttackButton" );
	m_attack = new c3_Button(&errcode, aui_UniqueId(), controlBlock, C3ExpelButtonCallback, this);
	Assert( AUI_NEWOK(m_attack, errcode) );
	if ( !AUI_NEWOK(m_attack, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "ExpelButton" );
	m_expel = new c3_Button(&errcode, aui_UniqueId(), controlBlock, C3ExpelButtonCallback, this);
	Assert( AUI_NEWOK(m_expel, errcode) );
	if ( !AUI_NEWOK(m_expel, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "CancelButton" );
	m_cancel = new c3_Button(&errcode, aui_UniqueId(), controlBlock, C3ExpelButtonCallback, this);
	Assert( AUI_NEWOK(m_cancel, errcode) );
	if ( !AUI_NEWOK(m_cancel, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "TitleLabel");
	m_title_label = new c3_Static( &errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(m_title_label, errcode) );
	if ( !AUI_NEWOK(m_title_label, errcode) ) return -1;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}

c3_ExpelPopup::~c3_ExpelPopup( void )
{
    delete m_title_label;
    delete m_attack;
    delete m_expel;
    delete m_cancel;
    delete m_window;
}

void c3_ExpelPopup::Cleanup(void)
{
    allocated::clear(m_title_label);
    allocated::clear(m_attack);
    allocated::clear(m_expel);
    allocated::clear(m_cancel);
    allocated::clear(m_window);
	m_callback = NULL;
}

void c3_ExpelPopup::DisplayWindow( void )
{
	AUI_ERRCODE auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );
}

void c3_ExpelPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
}






c3_UtilityTextFieldPopup::c3_UtilityTextFieldPopup
(
    c3_UtilityTextFieldCallback *   callback,
    MBCHAR const *                  titleText,
    MBCHAR const *                  defaultText,
    MBCHAR const *                  messageText,
    MBCHAR *                        ldlBlock,
    void *                          data,
	bool                            wantEmpties
)
:
	m_window                (NULL),
	m_title_label           (NULL),
	m_message_label         (NULL),
    m_text                  (NULL),
    m_default_text          (NULL),
    m_title_text            (NULL),
	m_message_text          (NULL),
	m_ok                    (NULL),
    m_cancel                (NULL),
    m_callback              (callback),
	m_data                  (data),
	m_wantEmpties           (wantEmpties)
{
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

    if (ldlBlock) strcpy(windowBlock,ldlBlock);
	else strcpy(windowBlock,"DefaultUtilityTextFieldPopup");

	{
	    AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

	if (defaultText)
	{
		m_default_text = new MBCHAR[256];
		strcpy(m_default_text, defaultText);
	}

	if (titleText) {
		m_title_text = new MBCHAR[strlen(titleText)+1];
		strcpy(m_title_text, titleText);
	}

	if (messageText) {
		m_message_text = new MBCHAR[strlen(messageText)+1];
		strcpy(m_message_text, messageText);
	}

	Initialize( windowBlock );

	if (m_text)
    {
        m_text->SetKeyboardFocus();
    }
}

sint32 c3_UtilityTextFieldPopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( controlBlock, "%s.%s", windowBlock, "TitleLabel");
	m_window->AddTitle( controlBlock );
	m_window->AddCancel(C3UtilityTextFieldButtonActionCallback, this);
	m_window->AddOk(C3UtilityTextFieldButtonActionCallback, this);












	sprintf( controlBlock, "%s.%s", windowBlock, "TextMessage");
	m_title_label = new c3_Static( &errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(m_title_label, errcode) );
	if ( !AUI_NEWOK(m_title_label, errcode) ) return -1;
	if (m_title_text)
		m_title_label->SetText(m_title_text);

	sprintf( controlBlock, "%s.%s", windowBlock, "TextField");
	m_text = new C3TextField( &errcode, aui_UniqueId(), controlBlock,
		C3UtilityTextFieldButtonActionCallback, this);
	Assert( AUI_NEWOK(m_text, errcode) );
	if ( !AUI_NEWOK(m_text, errcode) ) return -1;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}

c3_UtilityTextFieldPopup::~c3_UtilityTextFieldPopup( void )
{
    delete m_ok;
    delete m_cancel;
    delete m_text;
    delete m_title_label;
    delete [] m_default_text;
    delete [] m_title_text;
    delete [] m_message_text;
    delete m_window;
}

void c3_UtilityTextFieldPopup::Cleanup(void)
{
    allocated::clear(m_ok);
    allocated::clear(m_cancel);
    allocated::clear(m_text);
    allocated::clear(m_title_label);

    delete [] m_default_text;
    m_default_text  = NULL;

    delete [] m_title_text;
    m_title_text    = NULL;

    delete [] m_message_text;
    m_message_text  = NULL;

    allocated::clear(m_window);
}

void c3_UtilityTextFieldPopup::DisplayWindow( void )
{
	UpdateData();

	AUI_ERRCODE auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RegisterHandler(m_window);
}

void c3_UtilityTextFieldPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RemoveHandler(m_window);
}

sint32 c3_UtilityTextFieldPopup::UpdateData( void )
{

	if (m_default_text)
		m_text->SetFieldText(m_default_text);

	return 0;
}






c3_UtilityTextMessagePopup::c3_UtilityTextMessagePopup
(
    MBCHAR const *                  text,
    sint32                          type,
    c3_UtilityTextMessageCallback * callback,
    MBCHAR const *                  ldlBlock
)
:
    m_window        (NULL),
    m_callback      (callback),
    m_type          (type),
    m_title_label   (NULL),
    m_text          (NULL),
	m_ok            (NULL),
	m_cancel        (NULL)
{
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if (ldlBlock)
    {
        strcpy(windowBlock,ldlBlock);
    }
	else if (type)
    {
        strcpy(windowBlock,"DefaultUtilityTextMessagePopup");
    }
    else
    {
        strcpy(windowBlock,"DefaultUtilityTextMessageOkPopup");
	}

	{
	    AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

    Initialize( windowBlock );
}

sint32 c3_UtilityTextMessagePopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( controlBlock, "%s.%s", windowBlock, "TitleLabel");
	m_window->AddTitle( controlBlock );

	switch ( m_type ) {
	case k_UTILITY_TEXTMESSAGE_OK:
		m_window->AddClose(C3UtilityTextMessageButtonActionCallback, this);
		break;

	case k_UTILITY_TEXTMESSAGE_OKCANCEL:
		m_window->AddCancel(C3UtilityTextMessageButtonActionCallback, this);
		m_window->AddOk(C3UtilityTextMessageButtonActionCallback, this);
		break;

	case k_UTILITY_TEXTMESSAGE_YESNO:
		m_window->AddYes(C3UtilityTextMessageButtonActionCallback, this);
		m_window->AddNo(C3UtilityTextMessageButtonActionCallback, this);
		break;
	}





















	sprintf( controlBlock, "%s.%s", windowBlock, "TextMessage");
	m_text = new c3_Static( &errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(m_text, errcode) );
	if ( !AUI_NEWOK(m_text, errcode) ) return -1;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}

c3_UtilityTextMessagePopup::~c3_UtilityTextMessagePopup( void )
{
    delete m_ok;
    delete m_cancel;
    delete m_text;
    delete m_title_label;
    delete m_window;
}

void c3_UtilityTextMessagePopup::Cleanup( void )
{
    allocated::clear(m_ok);
    allocated::clear(m_cancel);
    allocated::clear(m_text);
    allocated::clear(m_title_label);
    allocated::clear(m_window);
	m_type = 0;
}

void c3_UtilityTextMessagePopup::DisplayWindow( MBCHAR const *text )
{
	UpdateData(text);

	AUI_ERRCODE auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );

	keypress_RegisterHandler(m_window);
}

void c3_UtilityTextMessagePopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	keypress_RemoveHandler(m_window);

    allocated::clear(g_utilityTextMessage);
}

sint32 c3_UtilityTextMessagePopup::UpdateData( MBCHAR const *text )
{

	if (text)
		m_text->SetText(text);

	return 0;
}

void c3_UtilityTextMessageCleanupAction::Execute(aui_Control *control,
									uint32 action,
									uint32 data )
{
	if (g_utilityTextMessage)
    {
		g_utilityTextMessage->Cleanup();
        allocated::clear(g_utilityTextMessage);
    }
}

c3_UtilityTextMessageCreateAction::c3_UtilityTextMessageCreateAction
(
    MBCHAR const *                  text,
    sint32                          type,
    c3_UtilityTextMessageCallback * callback,
    MBCHAR const *                  ldlBlock
)
:
    m_text      (text),
    m_type      (type),
    m_callback  (callback),
    m_ldlBlock  (ldlBlock)
{ ; }

void c3_UtilityTextMessageCreateAction::Execute( aui_Control *control, uint32 action, uint32 data )
{

	c3_TextMessage( m_text, m_type, m_callback, m_ldlBlock );
}

void c3_UtilityAbortCleanupAction::Execute(aui_Control *control,
									uint32 action,
									uint32 data )
{
	// That's a design: Deleting from a member function a global object from the same type.
	if (g_utilityAbort)
    {
		g_utilityAbort->Cleanup();
        allocated::clear(g_utilityAbort);
    }
}





void c3_TextMessage(MBCHAR const *text, sint32 type, c3_UtilityTextMessageCallback *callback, MBCHAR const *ldlBlock )
{

	if (g_utilityTextMessage) return;

	g_utilityTextMessage = new c3_UtilityTextMessagePopup( text, type, callback, ldlBlock );
	g_utilityTextMessage->DisplayWindow(text);
}

void c3_KillTextMessage( void )
{
	if (g_utilityTextMessage)
    {
		g_utilityTextMessage->Cleanup();
        allocated::clear(g_utilityTextMessage);
    }
}





void c3_AbortMessage(MBCHAR *text, sint32 type, c3_AbortMessageCallback *callback, MBCHAR *ldlBlock )
{

	if (g_utilityAbort) return;

	g_utilityAbort = new c3_UtilityAbortPopup( text, type, callback, ldlBlock );
	g_utilityAbort->DisplayWindow(text);
}

void c3_AbortUpdateData( MBCHAR *text, sint32 percentFilled )
{
	if ( !g_utilityAbort ) return;

	if(text) {
		g_utilityAbort->UpdateData( text );
	}

	if ( g_utilityAbort->m_type == k_UTILITY_PROGRESS_ABORT ) {
		g_utilityAbort->UpdateMeter( percentFilled );
	}
}

void c3_RemoveAbortMessage( void )
{
	if ( g_utilityAbort ) {
		g_utilityAbort->RemoveWindow();
	}
}





c3_UtilityAbortPopup::c3_UtilityAbortPopup( MBCHAR const *text, sint32 type, c3_UtilityTextMessageCallback* callback,  MBCHAR const *ldlBlock )
:
    m_window        (NULL),
    m_text          (NULL),
    m_meter         (NULL),
    m_abort         (NULL),
    m_type          (type),
    m_callback      (callback)
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if (ldlBlock) strcpy(windowBlock,ldlBlock);
	else
	{
		if (type == k_UTILITY_ABORT) strcpy(windowBlock,"DefaultUtilityAbortPopup");
		else strcpy(windowBlock,"DefaultUtilityAbortProgressPopup");
	}

	{
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

	Initialize( windowBlock );
}

sint32 c3_UtilityAbortPopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( controlBlock, "%s.%s", windowBlock, "AbortButton" );
	m_abort = new ctp2_Button(&errcode, aui_UniqueId(), controlBlock, C3AbortButtonActionCallback, this);
	TestControl( m_abort );

	if (m_type == k_UTILITY_PROGRESS_ABORT)
	{
		sprintf( controlBlock, "%s.%s", windowBlock, "Meter" );
		m_meter = new Thermometer(&errcode, aui_UniqueId(), controlBlock );
		TestControl( m_meter );
	}

	sprintf( controlBlock, "%s.%s", windowBlock, "TextMessage");
	m_text = new c3_Static( &errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(m_text, errcode) );
	if ( !AUI_NEWOK(m_text, errcode) ) return -1;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}

c3_UtilityAbortPopup::~c3_UtilityAbortPopup( void )
{
    delete m_abort;
    delete m_meter;
    delete m_text;
    delete m_window;
}

void c3_UtilityAbortPopup::Cleanup(void)
{
    allocated::clear(m_abort);
    allocated::clear(m_meter);
    allocated::clear(m_text);
    allocated::clear(m_window);
	m_type = 0;
}

void c3_UtilityAbortPopup::DisplayWindow( MBCHAR const *text, sint32 percentFilled )
{
	if ( text ) {
		UpdateData(text);
	}

	if ( m_type == k_UTILITY_PROGRESS_ABORT ) {
		UpdateMeter( percentFilled );
	}
	keypress_RegisterHandler(this);

	AUI_ERRCODE auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );
}

void c3_UtilityAbortPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	keypress_RemoveHandler(this);

    allocated::clear(g_utilityAbort);
}

sint32 c3_UtilityAbortPopup::UpdateData( MBCHAR const *text )
{

	if (text)
		m_text->SetText(text);

	m_window->ShouldDraw();

	return 0;
}

sint32 c3_UtilityAbortPopup::UpdateMeter( sint32 percentFilled )
{
	m_meter->SetPercentFilled( percentFilled );

	m_window->ShouldDraw();

	return 0;
}

void c3_UtilityAbortPopup::kh_Close()
{
	if(m_callback)
		m_callback(FALSE);

	RemoveWindow();
}




c3_UtilityPlayerListPopup::c3_UtilityPlayerListPopup( c3_UtilityPlayerListCallback *callback, MBCHAR *ldlBlock )
:
    m_window        (NULL),
	m_list          (NULL),
    m_abort         (NULL),
    m_kick          (NULL),
    m_open          (NULL),
    m_close         (NULL),
    m_callback      (callback)
{
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if (ldlBlock) strcpy(windowBlock,ldlBlock);
	else strcpy(windowBlock,"DefaultUtilityPlayerListPopup");

	{
	    AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

    Initialize( windowBlock );
}

sint32 c3_UtilityPlayerListPopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( controlBlock, "%s.%s", windowBlock, "AbortButton" );
	m_abort = new ctp2_Button(&errcode, aui_UniqueId(), controlBlock, C3UtilityPlayerListButtonActionCallback, this);
	TestControl( m_abort );

	sprintf( controlBlock, "%s.%s", windowBlock, "KickButton" );
	m_kick = new ctp2_Button(&errcode, aui_UniqueId(), controlBlock, C3UtilityPlayerListButtonActionCallback, this);
	TestControl( m_kick );

	sprintf( controlBlock, "%s.%s", windowBlock, "OpenButton" );
	m_open = new ctp2_Button(&errcode, aui_UniqueId(), controlBlock, C3UtilityPlayerListButtonActionCallback, this);
	TestControl( m_open );

	sprintf( controlBlock, "%s.%s", windowBlock, "CloseButton" );
	m_close = new ctp2_Button(&errcode, aui_UniqueId(), controlBlock, C3UtilityPlayerListButtonActionCallback, this);
	TestControl( m_close );

	sprintf( controlBlock, "%s.%s", windowBlock, "PlayerList" );
	m_list = new ctp2_ListBox(&errcode, aui_UniqueId(), controlBlock, NULL, NULL);
	Assert( AUI_NEWOK(m_list, errcode) );
	if ( !AUI_NEWOK(m_list, errcode) ) return -1;
	m_list->DoneInstantiating();

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}

c3_UtilityPlayerListPopup::~c3_UtilityPlayerListPopup( void )
{
    delete m_abort;
    delete m_kick;
    delete m_open;
    delete m_close;
    if (m_list)
    {
        m_list->Clear();
        delete m_list;
    }
    delete m_window;
}

void c3_UtilityPlayerListPopup::Cleanup( void )
{
    allocated::clear(m_abort);
    allocated::clear(m_kick);
    allocated::clear(m_open);
    allocated::clear(m_close);
    if (m_list)
    {
        m_list->Clear();
        allocated::clear(m_list);
    }
    allocated::clear(m_window);
	m_callback = NULL;
}

void c3_UtilityPlayerListPopup::DisplayWindow( void )
{
	UpdateData();

	AUI_ERRCODE auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );

	keypress_RegisterHandler(this);
}

void c3_UtilityPlayerListPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow(m_window->Id());
	Assert(auiErr == AUI_ERRCODE_OK);

	keypress_RemoveHandler(this);
}

void c3_UtilityPlayerListPopup::kh_Close()
{
	RemoveWindow();
}

sint32 c3_UtilityPlayerListPopup::UpdateData( void )
{
	MBCHAR ldlBlock[k_AUI_LDL_MAXBLOCK + 1];
	strcpy(ldlBlock, "DoubleListItem");

	MBCHAR strbuf[256];
	AUI_ERRCODE		retval;

	m_list->Clear();

	for (sint32 i = 1 ; i < k_MAX_PLAYERS ; ++i)
	{
		if (g_player[i])
        {
			strcpy(strbuf, g_player[i]->GetLeaderName());
            m_list->AddItem(new DoubleListItem
                                (&retval,
                                 strbuf,
                                 i,
                                 g_network.GetStatusString(i),
                                 ldlBlock
                                )
                           );
		}
	}

	return 0;
}

sint32 c3_UtilityPlayerListPopup::EnableButtons( void )
{
	m_kick->Enable( TRUE );
	m_open->Enable( TRUE );
	m_close->Enable( TRUE );

	return 1;
}

sint32 c3_UtilityPlayerListPopup::DisableButtons( void )
{
	m_kick->Enable( FALSE );
	m_open->Enable( FALSE );
	m_close->Enable( FALSE );

	return 1;
}

void c3_UtilityPlayerListPopup::SetText(MBCHAR * s, sint32 index)
{
	DoubleListItem * item = (DoubleListItem *) m_list->GetItemByIndex(index);

	if (item)
    {
		item->SetSecondColumn(s);
	}
}













DoubleListItem::DoubleListItem(AUI_ERRCODE *retval, MBCHAR *name, sint32 value, MBCHAR *text, MBCHAR *ldlBlock)
:
	aui_ImageBase   (ldlBlock),
	aui_TextBase    (ldlBlock, (MBCHAR const *) NULL),
	c3_ListItem     (retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(name, value, text, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
}

AUI_ERRCODE DoubleListItem::InitCommonLdl(MBCHAR *name, sint32 value, MBCHAR *text, MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	strcpy(m_name, name);
	m_value = value;

	if ( text ) {
		strcpy( m_text, text );
	}
	else {
		strcpy( m_text, "" );
	}

	sprintf(block, "%s.%s", ldlBlock, "Name");
	AddChild(new c3_Static(&retval, aui_UniqueId(), block));

	sprintf(block, "%s.%s", ldlBlock, "Text");
	AddChild(new c3_Static(&retval, aui_UniqueId(), block));

	Update();

	return AUI_ERRCODE_OK;
}

void DoubleListItem::Update(void)
{

	c3_Static *subItem;

	subItem = (c3_Static *)GetChildByIndex(0);
	subItem->SetText(m_name);

	subItem = (c3_Static *)GetChildByIndex(1);
	subItem->SetText(m_text);

}

sint32 DoubleListItem::SetSecondColumn( MBCHAR *s)
{
	c3_Static * subItem = (c3_Static *) GetChildByIndex(1);
	subItem->SetText(s);

	return 0;
}

sint32 DoubleListItem::Compare(c3_ListItem *item2, uint32 column)
{
	switch (column)
    {
    default:
        return 0;

	case 0:
	case 1:
        {
		    c3_Static * i1 = (c3_Static *)this->GetChildByIndex(column);
		    c3_Static * i2 = (c3_Static *)item2->GetChildByIndex(column);

	        MBCHAR			strbuf1[256];
		    strcpy(strbuf1,i1->GetText());
	        MBCHAR			strbuf2[256];
		    strcpy(strbuf2,i2->GetText());

		    return (strbuf1[0] - strbuf2[0]);
        }
	}
}

void c3Expel_Initialize( c3_ExpelCallback callback )
{
	if ( g_expelPopup ) return;

	g_expelPopup = new c3_ExpelPopup( callback );
}

void c3Expel_Cleanup( void )
{
    allocated::clear(g_expelPopup);
}


static c3_UtilityTextFieldPopup	*s_nameTheCityPopup;
static Unit						s_unit;

void NameTheCityDialogBoxCallback(MBCHAR const * text, sint32 val2, void *data)
{
	if (!val2) return;

	if (s_unit.IsValid())
    {
		if (s_unit.GetOwner() == g_selected_item->GetVisiblePlayer())
        {
			s_unit.GetData()->GetCityData()->SetName(text);
			g_mainControlPanel->UpdateCityList();
		}
	}
}

void c3_utilitydialogbox_NameCity(Unit city)
{
	MBCHAR		nameText[k_MAX_NAME_LEN];

	if (!city.IsValid()) return;

	s_unit = city;

	strcpy(nameText, city.GetData()->GetCityData()->GetName());

	if ( !s_nameTheCityPopup ) {

		s_nameTheCityPopup = new c3_UtilityTextFieldPopup(NameTheCityDialogBoxCallback,
									NULL,
									nameText,
									NULL,
									"NewNameTheCityPopup"
									);
	} else {

		if (s_nameTheCityPopup->m_default_text) {
			delete [] s_nameTheCityPopup->m_default_text;
			s_nameTheCityPopup->m_default_text = new MBCHAR[strlen(nameText) + 1];
			strcpy(s_nameTheCityPopup->m_default_text, nameText);

			s_nameTheCityPopup->UpdateData();

			s_nameTheCityPopup->m_text->SetKeyboardFocus();
			s_nameTheCityPopup->m_text->SelectAll();
		}
	}

	s_nameTheCityPopup->DisplayWindow();
}

void c3_utilitydialogbox_NameCityCleanup(void)
{
    allocated::clear(s_nameTheCityPopup);
}




c3_UtilityTextFieldPopup		*s_genericTextEntryPopup = NULL;

void c3_utilitydialogbox_TextFieldDialog(MBCHAR *titleText,
								   MBCHAR *defaultText,
								   MBCHAR *messageText,
								   c3_UtilityTextFieldCallback *callback,
								   MBCHAR *ldlBlock)
{

	if ( !s_genericTextEntryPopup ) {

		s_genericTextEntryPopup = new c3_UtilityTextFieldPopup(callback,
																titleText,
																defaultText,
																messageText,
																ldlBlock);
		Assert(s_genericTextEntryPopup);
	}

	if (s_genericTextEntryPopup->m_default_text) {
		delete [] s_genericTextEntryPopup->m_default_text;
		s_genericTextEntryPopup->m_default_text = new MBCHAR[strlen(defaultText) + 1];
		strcpy(s_genericTextEntryPopup->m_default_text, defaultText);
	}


	s_genericTextEntryPopup->UpdateData();

	s_genericTextEntryPopup->m_text->SetKeyboardFocus();
	s_genericTextEntryPopup->m_text->SelectAll();

	s_genericTextEntryPopup->DisplayWindow();
}

void c3_utilitydialogbox_CleanupTextFieldDialog(void)
{
    allocated::clear(s_genericTextEntryPopup);
}
