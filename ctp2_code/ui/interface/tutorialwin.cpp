//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The tutorial window
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
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_directsurface.h"
#include "aui_stringtable.h"

#include "c3ui.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_switch.h"
#include "c3_listbox.h"

#include "SlicEngine.h"
#include "player.h"
#include "SelItem.h"

#include "pointerlist.h"
#include "SlicRecord.h"

#include "profileDB.h"

#include "c3_utilitydialogbox.h"

#include "UIUtils.h"
#include "screenutils.h"

#include "tutorialwin.h"


extern C3UI			*g_c3ui;
extern Player		**g_player;
extern SelectedItem	*g_selected_item; 

extern SlicEngine	*g_slicEngine;

extern ProfileDB	*g_theProfileDB;


TutorialWin		*g_tutorialWin = NULL;



void tutorialwin_DialogCallback( sint32 val )
{
	if ( val ) {
		g_theProfileDB->SetTutorialAdvice( FALSE );

		close_TutorialWin();

	}
}

void tutorialwin_ButtonCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_tutorialWin->HandleButton( (c3_Button *)control );
}

void tutorialwin_SwitchCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	switch ( action ) {
	case AUI_SWITCH_ACTION_ON:
	case AUI_SWITCH_ACTION_OFF:
		if ( g_tutorialWin ) {
			g_tutorialWin->HandleSwitch( (c3_Switch *)control );
		}
		break;
	}

}

void tutorialwin_ListCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) return;

	SingleListItem *item;

	item = (SingleListItem *)(((aui_ListBox *)control)->GetSelectedItem());
	if ( !item ) return;

	sint32 i = 0;

	
	sint32 player = g_selected_item->GetVisiblePlayer();
	PointerList<SlicRecord> *recordList = g_slicEngine->GetRecords(player);
	if ( !recordList ) return;

	PointerList<SlicRecord>::Walker walk(recordList);

	while(walk.IsValid()) {
		if ( i++ == item->GetValue() ) {	
			walk.GetObj()->Reconstitute();
			return;
		}
		walk.Next();
	}
}

sint32 tutorialwin_Initialize( void )
{
	if ( g_tutorialWin ) {
		g_tutorialWin->UpdateData();
		return 0;	
	}

	g_tutorialWin = new TutorialWin();

	return 0;
}

sint32 tutorialwin_Cleanup( void )
{
	RemoveControl( g_tutorialWin );

	return 0;
}

TutorialWin::TutorialWin( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	strcpy(windowBlock,"TutorialWin");

	m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING );
	Assert( AUI_NEWOK(m_window, errcode) );
	if ( !AUI_NEWOK(m_window, errcode) ) return;

	m_window->Resize( m_window->Width(), m_window->Height() );
	m_window->GrabRegion()->Resize( m_window->Width(), m_window->Height() );
	m_window->SetDraggable( TRUE );

	m_minimized = TRUE;

	
	Initialize( windowBlock );

	UpdateData();
}

sint32 TutorialWin::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	sprintf( controlBlock, "%s.%s", windowBlock, "List" );
	m_list = new c3_ListBox( &errcode, aui_UniqueId(), controlBlock, tutorialwin_ListCallback, NULL );
	TestControl( m_list );
	m_list->Hide();

	
	sprintf( controlBlock, "%s.%s", windowBlock, "TitleButton" );
	m_titleButton = new c3_Switch( &errcode, aui_UniqueId(), controlBlock, tutorialwin_SwitchCallback );
	TestControl( m_titleButton );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "EndButton" );
	m_endButton = new c3_Button( &errcode, aui_UniqueId(), controlBlock, tutorialwin_ButtonCallback );
	TestControl( m_endButton );
	m_endButton->Hide();

	
	sprintf( controlBlock, "%s.%s", windowBlock, "ExitButton" );
	m_exitButton = new c3_Button( &errcode, aui_UniqueId(), controlBlock, tutorialwin_ButtonCallback );
	TestControl( m_exitButton );

	
	sprintf( controlBlock, "TutorialWinStrings" );
	m_string = new aui_StringTable( &errcode, controlBlock );
	TestControl( m_string );

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}

TutorialWin::~TutorialWin( void )
{
	Remove();

	RemoveControl( m_list );
	RemoveControl( m_titleButton );
	RemoveControl( m_endButton );
	RemoveControl( m_exitButton );
	RemoveControl( m_string );

	RemoveControl( m_window );
}

void TutorialWin::Display( void )
{
	AUI_ERRCODE errcode = g_c3ui->AddWindow( m_window );
	Assert( errcode == AUI_ERRCODE_OK );



}

void TutorialWin::Remove( void )
{
	AUI_ERRCODE errcode = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( errcode == AUI_ERRCODE_OK );



}

sint32 TutorialWin::UpdateData( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR title[_MAX_PATH];
	sint32 i = 0;

	
	sint32 player = g_selected_item->GetVisiblePlayer();
	PointerList<SlicRecord> *recordList = g_slicEngine->GetRecords(player);
	if ( !recordList ) return -1;

	m_list->Clear();

	sprintf( ldlBlock, "TutorialListItem" );
	SingleListItem *item;


	PointerList<SlicRecord>::Walker walk(recordList);

	while(walk.IsValid()) {
		strcpy( title, walk.GetObj()->GetTitle() );
		item = new SingleListItem( &errcode, title, i++, ldlBlock );
		TestControl( item );
		
		m_list->AddItem( (c3_ListItem *)item );

		walk.Next();
	}

	return 0;
}

sint32 TutorialWin::AddToList( MBCHAR *text, sint32 index )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	sint32 player = g_selected_item->GetVisiblePlayer();
	PointerList<SlicRecord> *recordList = g_slicEngine->GetRecords(player);
	if ( !recordList ) return -1;

	sprintf( ldlBlock, "TutorialListItem" );
	SingleListItem *item;

	item = new SingleListItem( &errcode, text, index, ldlBlock );
	TestControl( item );
		
	m_list->AddItem( (c3_ListItem *)item );


	return 0;
}

sint32 TutorialWin::HandleSwitch( c3_Switch *button )
{
	if ( button == m_titleButton ) {	
		m_minimized = !m_minimized;
		if ( !button->IsOn() ) {
			m_window->Resize( m_window->Width(), k_MINIMIZED_HEIGHT );
			m_list->Hide();
			m_endButton->Hide();
		}
		else {
			m_window->Resize( m_window->Width(), k_MAXIMIZED_HEIGHT );
			m_list->Show();
			m_endButton->Show();
		}
	}

	return 0;
}

sint32 TutorialWin::HandleButton( c3_Button *button )
{
	if ( button == m_endButton ) {
		c3_TextMessage( m_string->GetString(0), k_UTILITY_TEXTMESSAGE_YESNO, tutorialwin_DialogCallback );
	}
	else if ( button == m_exitButton ) {
		if ( m_titleButton->IsOn() ) {
			m_titleButton->SetState( 0 );
		}
	}

	return 0;
}

