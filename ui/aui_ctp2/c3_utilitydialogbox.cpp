











#include "c3.h"


#include "aui.h"
#include "aui_uniqueid.h"
#include "c3ui.h"
#include "aui_ldl.h"
#include "aui_UniqueId.h"
#include "aui_static.h"
#include "aui_hypertextbox.h"

#include "c3_Static.h"
#include "Thermometer.h"


#include "textbutton.h"
#include "c3_Button.h"
#include "ctp2_Button.h"


#include "c3textfield.h"


#include "ctp2_listbox.h"
#include "c3_listbox.h"
#include "aui_listbox.h"


#include "c3window.h"
#include "c3windows.h"
#include "c3_popupwindow.h"
#include "c3_utilitydialogbox.h"
#include "SelItem.h"




#include "Player.h"
#include "UnitRec.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Unit.h"
#include "UnitData.h"
#include "UnitDynArr.h"
#include "CityData.h"
#include "StrDB.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "TerrainRecord.h"

#include "uiutils.h"

#include "network.h"

#include "keypress.h"

#include "MainControlPanel.h"


#include "UnitPool.h"
extern UnitPool		*g_theUnitPool;


extern sint32		g_ScreenWidth;
extern sint32		g_ScreenHeight;
extern C3UI			*g_c3ui;
extern SelectedItem				*g_selected_item; 
extern Player					**g_player;


extern StringDB					*g_theStringDB;

extern sint32					g_modalWindow;


c3_UtilityTextMessagePopup		*g_utilityTextMessage = NULL;
c3_UtilityAbortPopup			*g_utilityAbort = NULL;

c3_ExpelPopup					*g_expelPopup = NULL;


void C3UtilityCityListButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	
	c3_UtilityCityListPopup *popup = (c3_UtilityCityListPopup *)cookie;
	if (!popup) return;

	if ((ctp2_Button*)control == popup->m_window->Ok())
	{
		
		SingleListItem *item = (SingleListItem *) popup->m_list->GetSelectedItem();
		if (!item) return;

		sint32 cityIndex = item->GetValue();

		
		
		
		
		Assert(FALSE);
		if (popup->m_callback)
		{
		
			
			popup->RemoveWindow();
		}
	}
	if ((c3_Button*)control == popup->m_window->Cancel())
	{
		if (popup->m_callback)
			popup->m_callback( Unit(0), FALSE );

		
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
			
			delete resultText;
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

	
	if ( popup->m_type )
	{
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
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if (ldlBlock) strcpy(windowBlock,ldlBlock);	
	else strcpy(windowBlock,"DefaultUtilityCityListPopup");

	
	{ 
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		
		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

	
	m_ok = NULL;
	m_cancel = NULL;
	m_list = NULL;
	m_title_label = NULL;

	
	m_callback = callback;

	
	Initialize( windowBlock );

}

sint32 c3_UtilityCityListPopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode;
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
	Cleanup();
}

sint32 c3_UtilityCityListPopup::Cleanup( void )
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	g_c3ui->RemoveWindow( m_window->Id() );

	mycleanup( m_title_label );
	mycleanup( m_ok );
	mycleanup( m_cancel );
	
	m_callback = NULL;

	delete m_window;
	m_window = NULL;

	return 0 ;

#undef mycleanup
}

void c3_UtilityCityListPopup::DisplayWindow( void )
{
	AUI_ERRCODE auiErr;

	
	UpdateData();

	auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );

}

void c3_UtilityCityListPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( m_window->Id() );
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
	SingleListItem *item = NULL;

	m_list->Clear();

	for ( sint32 i = 0 ; i < cityList->Num() ; i++ )
	{
		strcpy(strbuf, (*cityList)[i].GetData()->GetCityData()->GetName());
		item = new SingleListItem(&retval, strbuf, i, ldlBlock);
		m_list->AddItem((c3_ListItem *)item);
	}

	return 0;
}




c3_PiracyPopup::c3_PiracyPopup( c3_PiracyCallback *callback, MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if (ldlBlock) strcpy(windowBlock,ldlBlock);	
	else strcpy(windowBlock,"DefaultPiracyPopup");

	
	{ 
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		
		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

	
	m_pirate = NULL;
	m_cancel = NULL;
	m_list = NULL;
	m_title_label = NULL;

	
	m_callback = callback;

	
	Initialize( windowBlock );
}

sint32 c3_PiracyPopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode;
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
	Cleanup();
}

sint32 c3_PiracyPopup::Cleanup( void )
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	g_c3ui->RemoveWindow( m_window->Id() );

	mycleanup( m_title_label );
	mycleanup( m_pirate );
	mycleanup( m_cancel );
	
	m_callback = NULL;

	delete m_window;
	m_window = NULL;

	return 0 ;

#undef mycleanup
}

void c3_PiracyPopup::DisplayWindow( void )
{
	AUI_ERRCODE auiErr;

	
	UpdateData();

	auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );

}

void c3_PiracyPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( m_window->Id() );
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
	SingleListItem *item = NULL;

	m_list->Clear();

	for ( sint32 i = 0 ; i < cityList->Num() ; i++ )
	{
		strcpy(strbuf, (*cityList)[i].GetData()->GetCityData()->GetName());
		item = new SingleListItem(&retval, strbuf, i, ldlBlock);
		m_list->AddItem((c3_ListItem *)item);
	}

	return 0;
}




c3_ExpelPopup::c3_ExpelPopup( c3_ExpelCallback *callback, MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if (ldlBlock) strcpy(windowBlock,ldlBlock);	
	else strcpy(windowBlock,"DefaultExpelPopup");

	
	{ 
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		
		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

	
	m_attack = NULL;
	m_expel = NULL;
	m_cancel = NULL;
	m_title_label = NULL;

	
	m_callback = callback;

	
	Initialize( windowBlock );
}

sint32 c3_ExpelPopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode;
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
	Cleanup();
}

sint32 c3_ExpelPopup::Cleanup( void )
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	g_c3ui->RemoveWindow( m_window->Id() );

	mycleanup( m_title_label );
	mycleanup( m_attack );
	mycleanup( m_expel );
	mycleanup( m_cancel );
	
	m_callback = NULL;

	delete m_window;
	m_window = NULL;

	return 0 ;

#undef mycleanup
}

void c3_ExpelPopup::DisplayWindow( void )
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );

}

void c3_ExpelPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
}






c3_UtilityTextFieldPopup::c3_UtilityTextFieldPopup( c3_UtilityTextFieldCallback* callback, 
												   MBCHAR *titleText, MBCHAR *defaultText, 
												   MBCHAR *messageText, MBCHAR *ldlBlock, void *data,
													bool wantEmpties)
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if (ldlBlock) strcpy(windowBlock,ldlBlock);	
	else strcpy(windowBlock,"DefaultUtilityTextFieldPopup");

	{ 
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		
		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

	
	m_ok = NULL;
	m_cancel = NULL;

	m_title_label = NULL;
	m_message_label = NULL;
	m_text = NULL;

	m_default_text = NULL;
	m_title_text = NULL;
	m_message_text = NULL;

	m_data = data;

	m_wantEmpties = wantEmpties;

	
	if (defaultText) 
	{
		m_default_text = new MBCHAR[256];
		strcpy(m_default_text, defaultText);
	}
	else m_default_text = NULL;

	if (titleText) {
		m_title_text = new MBCHAR[strlen(titleText)+1];
		strcpy(m_title_text, titleText);
	}

	if (messageText) {
		m_message_text = new MBCHAR[strlen(messageText)+1];
		strcpy(m_message_text, messageText);
	}

	m_callback = callback;

	
	Initialize( windowBlock );

	m_text->SetKeyboardFocus();

}

sint32 c3_UtilityTextFieldPopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode;
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
	Cleanup();
}

sint32 c3_UtilityTextFieldPopup::Cleanup( void )
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	g_c3ui->RemoveWindow( m_window->Id() );

	mycleanup( m_ok );
	mycleanup( m_cancel );
	mycleanup( m_text );
	mycleanup( m_title_label );
	mycleanup( m_default_text );
	
	delete m_window;
	m_window = NULL;

#undef mycleanup
	return 0;
}

void c3_UtilityTextFieldPopup::DisplayWindow( void )
{
	AUI_ERRCODE auiErr;

	
	UpdateData();
	
	auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RegisterHandler(m_window);

}

void c3_UtilityTextFieldPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RemoveHandler(m_window);
}

sint32 c3_UtilityTextFieldPopup::UpdateData( void )
{
	
	if (m_default_text) 
		m_text->SetFieldText(m_default_text);

	return 0;
}






c3_UtilityTextMessagePopup::c3_UtilityTextMessagePopup( MBCHAR *text, sint32 type, c3_UtilityTextMessageCallback* callback,  MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if (ldlBlock) strcpy(windowBlock,ldlBlock);	
	else 
	{
		if (type) strcpy(windowBlock,"DefaultUtilityTextMessagePopup");
		else strcpy(windowBlock,"DefaultUtilityTextMessageOkPopup");
	}

	{ 
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		
		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

	
	m_ok = NULL;
	m_cancel = NULL;
	m_title_label = NULL;
	m_text = NULL;
	m_type = type;
	
	m_callback = callback;

	
	Initialize( windowBlock );
}

sint32 c3_UtilityTextMessagePopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode;
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
	Cleanup();
}

sint32 c3_UtilityTextMessagePopup::Cleanup( void )
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	g_c3ui->RemoveWindow( m_window->Id() );

	mycleanup( m_ok );
	mycleanup( m_cancel );
	mycleanup( m_text );
	mycleanup( m_title_label );

	m_type = 0;
	
	delete m_window;
	m_window = NULL;

#undef mycleanup
	return 0;
}

void c3_UtilityTextMessagePopup::DisplayWindow( MBCHAR *text )
{
	AUI_ERRCODE auiErr;

	
	UpdateData(text);
	
	auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );

	keypress_RegisterHandler(m_window);
}

void c3_UtilityTextMessagePopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	c3_UtilityTextMessageCleanupAction *tempAction = new c3_UtilityTextMessageCleanupAction;

	g_c3ui->AddAction(tempAction);

	keypress_RemoveHandler(m_window);
}

sint32 c3_UtilityTextMessagePopup::UpdateData( MBCHAR *text )
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
		g_utilityTextMessage->Cleanup();

	g_utilityTextMessage = NULL;
}

c3_UtilityTextMessageCreateAction::c3_UtilityTextMessageCreateAction( MBCHAR *text, sint32 type, c3_UtilityTextMessageCallback *callback, MBCHAR *ldlBlock )
{
	m_text = text;
	m_type = type;
	m_callback = callback;
	m_ldlBlock = ldlBlock;
}


void c3_UtilityTextMessageCreateAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	
	c3_TextMessage( m_text, m_type, m_callback, m_ldlBlock );
}


void c3_UtilityAbortCleanupAction::Execute(aui_Control *control,
									uint32 action,
									uint32 data )
{
	
	if (g_utilityAbort)
		g_utilityAbort->Cleanup();

	g_utilityAbort = NULL;
}





void c3_TextMessage(MBCHAR *text, sint32 type, c3_UtilityTextMessageCallback *callback, MBCHAR *ldlBlock )
{
	
	if (g_utilityTextMessage) return;

	g_utilityTextMessage = new c3_UtilityTextMessagePopup( text, type, callback, ldlBlock );
	g_utilityTextMessage->DisplayWindow(text);
}

void c3_KillTextMessage( void )
{
	
	if (g_utilityTextMessage)
		g_utilityTextMessage->Cleanup();

	g_utilityTextMessage = NULL;
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









c3_UtilityAbortPopup::c3_UtilityAbortPopup( MBCHAR *text, sint32 type, c3_UtilityTextMessageCallback* callback,  MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode;
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

	m_text = NULL;
	m_meter = NULL;
	m_type = type;
	
	m_callback = callback;

	
	Initialize( windowBlock );
}

sint32 c3_UtilityAbortPopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode;
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
	Cleanup();
}

sint32 c3_UtilityAbortPopup::Cleanup( void )
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	g_c3ui->RemoveWindow( m_window->Id() );

	mycleanup( m_abort );
	mycleanup( m_meter );

	m_type = 0;
	
	delete m_window;
	m_window = NULL;

#undef mycleanup
	return 0;
}

void c3_UtilityAbortPopup::DisplayWindow( MBCHAR *text, sint32 percentFilled )
{
	AUI_ERRCODE auiErr;

	
	if ( text ) {
		UpdateData(text);
	}

	if ( m_type == k_UTILITY_PROGRESS_ABORT ) {
		UpdateMeter( percentFilled );
	}
	keypress_RegisterHandler(this);

	auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );
}

void c3_UtilityAbortPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	keypress_RemoveHandler(this);

	g_c3ui->AddAction( new c3_UtilityAbortCleanupAction );

}

sint32 c3_UtilityAbortPopup::UpdateData( MBCHAR *text )
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
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if (ldlBlock) strcpy(windowBlock,ldlBlock);	
	else strcpy(windowBlock,"DefaultUtilityPlayerListPopup");

	
	{ 
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		
		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

	
	m_abort = NULL;
	m_kick = NULL;
	m_open = NULL;
	m_close = NULL;

	
	m_callback = callback;

	
	Initialize( windowBlock );
}

sint32 c3_UtilityPlayerListPopup::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode;
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

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}

c3_UtilityPlayerListPopup::~c3_UtilityPlayerListPopup( void )
{
	Cleanup();
}

sint32 c3_UtilityPlayerListPopup::Cleanup( void )
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	g_c3ui->RemoveWindow( m_window->Id() );

	mycleanup( m_abort );
	mycleanup( m_kick );
	mycleanup( m_open );
	mycleanup( m_close );
	mycleanup( m_list );
	
	m_callback = NULL;

	delete m_window;
	m_window = NULL;

	return 0 ;

#undef mycleanup
}

void c3_UtilityPlayerListPopup::DisplayWindow( void )
{
	AUI_ERRCODE auiErr;

	
	UpdateData();

	auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );

	keypress_RegisterHandler(this);
}

void c3_UtilityPlayerListPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	keypress_RemoveHandler(this);
}

void c3_UtilityPlayerListPopup::kh_Close()
{
	RemoveWindow();
}

sint32 c3_UtilityPlayerListPopup::UpdateData( void )
{
	MBCHAR strbuf[256];
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	AUI_ERRCODE		retval;

	
	strcpy(ldlBlock,"DoubleListItem");
	DoubleListItem *item = NULL;

	m_list->Clear();

	
	for ( sint32 i = 1 ; i < k_MAX_PLAYERS ; i++ )
	{
		if ( g_player[i] ) {
			strcpy(strbuf, g_player[i]->GetLeaderName());
			item = new DoubleListItem(&retval, strbuf, i, g_network.GetStatusString(i), ldlBlock);
			m_list->AddItem((c3_ListItem *)item);
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

void c3_UtilityPlayerListPopup::SetText( MBCHAR *s, sint32 index )
{
	DoubleListItem *item;

	if ( item = (DoubleListItem *)m_list->GetItemByIndex(index) ) {
		(DoubleListItem *)item->SetSecondColumn( s );
	}
}













DoubleListItem::DoubleListItem(AUI_ERRCODE *retval, MBCHAR *name, sint32 value, MBCHAR *text, MBCHAR *ldlBlock)
	:
	c3_ListItem( retval, ldlBlock),
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(name, value, text, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;	
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

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "Name");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "Text");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

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
	
	
	c3_Static *subItem;
	
	
	subItem = (c3_Static *)GetChildByIndex(1);
	subItem->SetText(s);

	return 0;

}

sint32 DoubleListItem::Compare(c3_ListItem *item2, uint32 column)
{
	c3_Static		*i1, *i2;
	MBCHAR			strbuf1[256];
	MBCHAR			strbuf2[256];

	if (column < 0) return 0;

	switch (column) {
	case 0:
	case 1:
		
		i1 = (c3_Static *)this->GetChildByIndex(column);
		i2 = (c3_Static *)item2->GetChildByIndex(column);
		strcpy(strbuf1,i1->GetText());
		strcpy(strbuf2,i2->GetText());

		return (strbuf1[0] - strbuf2[0]);
		break;
	}
	return 0;
}


void c3Expel_Initialize( c3_ExpelCallback callback )
{
	if ( g_expelPopup ) return;

	g_expelPopup = new c3_ExpelPopup( callback );
}

void c3Expel_Cleanup( void )
{
	if ( g_expelPopup ) {
		delete g_expelPopup;
		g_expelPopup = NULL;
	}
}



static c3_UtilityTextFieldPopup	*s_nameTheCityPopup;
static Unit						s_unit;

void NameTheCityDialogBoxCallback(MBCHAR *text, sint32 val2, void *data)
{
	if (!val2) return; 

	
	

	if(g_theUnitPool->IsValid(s_unit)) {
		if (s_unit.GetOwner() == g_selected_item->GetVisiblePlayer()) {
			s_unit.GetData()->GetCityData()->SetName(text);
			g_mainControlPanel->UpdateCityList();
		}
	}

	
	delete[] text;

}

void c3_utilitydialogbox_NameCity(Unit city)
{
	MBCHAR		nameText[k_MAX_NAME_LEN];

	if (!g_theUnitPool->IsValid(city)) return;

	s_unit = city;

	strcpy(nameText, city.GetData()->GetCityData()->GetName());

	if ( !s_nameTheCityPopup ) {

		s_nameTheCityPopup = new c3_UtilityTextFieldPopup(NameTheCityDialogBoxCallback, 
									NULL, 
									nameText, 
									NULL, 
									"NewNameTheCityPopup"
									);
		Assert(s_nameTheCityPopup);
	} else {
		
		if (s_nameTheCityPopup->m_default_text) {
			delete s_nameTheCityPopup->m_default_text;
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
	if (s_nameTheCityPopup) {
		delete s_nameTheCityPopup;
		s_nameTheCityPopup = NULL;
	}
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
		delete s_genericTextEntryPopup->m_default_text;
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
	if (s_genericTextEntryPopup) {
		delete s_genericTextEntryPopup;
		s_genericTextEntryPopup = NULL;
	}
}
