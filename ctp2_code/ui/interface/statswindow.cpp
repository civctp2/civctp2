//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Statistics window
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
// - Start the great library with the current research project of the player.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "statswindow.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "c3ui.h"
#include "aui_stringtable.h"

#include "pixelutils.h"
#include "primitives.h"


#include "c3_button.h"
#include "ctp2_button.h"


#include "c3_static.h"
#include "c3_coloredstatic.h"
#include "c3_switch.h"
#include "c3_utilitydialogbox.h"


#include "c3window.h"
#include "c3windows.h"


#include "tiledmap.h"                   // g_tiledMap
#include "colorset.h"                   // g_colorSet
#include "SelItem.h"                    // g_selected_item
#include "player.h"                     // g_player
#include "UnitDynArr.h"

#include "screenutils.h"

#include "UIUtils.h"
#include "bevellesswindow.h"

#include "network.h"
#include "victorywin.h"


#include "spnewgamewindow.h" 

#include "MessageBoxDialog.h"


extern sint32       g_ScreenWidth;
extern sint32       g_ScreenHeight;
extern C3UI			*g_c3ui;
extern C3Window		*g_toolbar;
extern C3Window		*g_turnWindow;


StatsWindow *g_statsWindow = NULL;



#define k_STATS_SCROLL		197
#define k_STATS_SCROLL_OFFSET	5
#define k_TURN_WINDOW_HEIGHT	181

#define k_STATS_SCROLL640		122
#define k_TURN_WINDOW_HEIGHT640	135	


static ctp2_Button **s_statsButton = NULL;
static c3_Switch *s_screensSwitch;

static MBCHAR	s_buttonName[k_STATS_NUM_BUTTONS][256] ={
	"CivButton",
	"TradeButton",
	"SciButton",
	"UnitButton",
	"LibButton",
	"DipButton",
	"OptionsButton",
	"CityButton",
 
 
};

static aui_StringTable *s_statsString;




























































































































































void statswindow_HideButtons( void )
{
	sint32 i;
	
	for ( i = 0;i < k_STATS_NUM_BUTTONS;i++ ) {
		s_statsButton[i]->Hide();
	}
}

void statswindow_ShowButtons( void )
{
	sint32 i;
	
	for ( i = 0;i < k_STATS_NUM_BUTTONS;i++ ) {
		s_statsButton[i]->Show();
	}
}

void StatsSwitchActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	switch ( action ) {
	case AUI_SWITCH_ACTION_ON:
		statswindow_ShowButtons();
		if ( g_ScreenWidth >= 1024 || g_ScreenHeight >= 768 ) g_statsWindow->Offset( 0, -k_STATS_SCROLL );
		else g_statsWindow->Offset( 0, -k_STATS_SCROLL640 );

		break;
	case AUI_SWITCH_ACTION_OFF:
		if ( s_statsButton ) {
			if ( g_ScreenWidth >= 1024 || g_ScreenHeight >= 768 ) g_statsWindow->Offset( 0, k_STATS_SCROLL );
			else g_statsWindow->Offset( 0, k_STATS_SCROLL640 );
			statswindow_HideButtons();
		}
		break;
	}
}

void StatsButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	for ( sint32 i = 0;i<k_STATS_NUM_BUTTONS;i++) 
	{
		if ( control->Id() == s_statsButton[i]->Id() ) 
		{
			switch ( i ) 
			{
			case 0:

					open_CivStatus();

				break;
			case 1:

				if ( g_statsWindow->CheckCity() ) {
					open_TradeStatus();
				}
				break;
		   
			case 2:
			  	if ( g_statsWindow->CheckCity() ) {
			  		open_ScienceStatus();
			  	}
				break;
			
			case 3:
				if ( g_statsWindow->CheckUnit() ) {
					open_UnitStatus();
				}
				break;
			case 4:
				open_GreatLibrary();
				break;
			case 5:
				if ( g_statsWindow->CheckDiplomacy() ) {
					open_Diplomacy();
				}
				break;
			case 6:
				open_OptionsScreen( 1 );
				break;
			case 7:
				if ( g_statsWindow->CheckCity() ) {
					open_CityStatus();
				}
				break;



			case 8:
				open_OptionsScreen( 1 );
				break;
			}
		}
	}
}


int StatsWindow_Initialize( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32		i;

	if ( g_statsWindow ) return 0; 

	
	strcpy(windowBlock, "StatsWindow");
	g_statsWindow = new StatsWindow(&errcode, aui_UniqueId(), windowBlock, 16 );
	Assert( AUI_NEWOK(g_statsWindow, errcode) );
	if ( !AUI_NEWOK(g_statsWindow, errcode) ) return -1;

	g_statsWindow->SetTransparent( TRUE );

	


	
	






























































	sprintf( controlBlock, "%s.%s", windowBlock, "ScreensSwitch" );
	
	

	
	
	s_statsButton = new ctp2_Button*[k_STATS_NUM_BUTTONS];

	for ( i = 0;i < k_STATS_NUM_BUTTONS;i++ ) 
	{
		sprintf( controlBlock, "%s.%s", windowBlock, s_buttonName[i] );
		
		
	   
 		s_statsButton[i] = spNew_ctp2_Button(&errcode,
		   									windowBlock, 
		   									s_buttonName[i],
		   									s_buttonName[i],
		   									StatsButtonActionCallback,
											"CTP2_BUTTON_TITLE_BAR");

		
		Assert( AUI_NEWOK(s_statsButton[i], errcode) );
		if ( !AUI_NEWOK(s_statsButton[i], errcode) ) return -10;

	}


	s_statsString = new aui_StringTable( &errcode, "StatsStrings" );
	TestControl( s_statsString );

	








































	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return -1;

	

	
	if ( g_ScreenWidth >= 1024 || g_ScreenHeight >= 768 ) 
	{
		
	   
	   
	} 
	else
	{
		
	   
	   
	}

	statswindow_ShowButtons();







	return 0;
}



int StatsWindow_Cleanup( void )
{
	
	if ( !g_statsWindow ) return 0; 

	g_c3ui->RemoveWindow( g_statsWindow->Id() );

	

	
	for ( sint32 i = 0;i < k_STATS_NUM_BUTTONS;i++ ) {
		delete s_statsButton[i];
		s_statsButton[i] = NULL;
	}

	delete s_statsButton;
	s_statsButton = NULL;

	RemoveControl( s_statsString );
















	delete g_statsWindow;
	g_statsWindow = NULL;

	return 0;
}



StatsWindow::StatsWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type )
	:
	C3Window(retval, id, ldlBlock, bpp, type)
{
	*retval = InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



StatsWindow::StatsWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *pattern,
	AUI_WINDOW_TYPE type)
	:
	C3Window( retval, id, x, y, width, height, bpp, pattern, type )
{
	*retval = InitCommon();
	Assert(AUI_SUCCESS(*retval));
	if (!AUI_SUCCESS(*retval)) return;
}



AUI_ERRCODE StatsWindow::InitCommon(void)
{
	m_scroll = FALSE;
	m_dest = 0;
	m_current = 0;

	return C3Window::InitCommon();
}



AUI_ERRCODE StatsWindow::InitCommonLdl(MBCHAR *ldlBlock)
{		
	InitCommon();
	return C3Window::InitCommon();
}



AUI_ERRCODE StatsWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_width, m_height };

	if (m_pattern!=NULL)
		m_pattern->Draw( m_surface, &rect );

	


	
	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;
}

BOOL StatsWindow::CheckCity( void ) 
{
	if(!g_player[g_selected_item->GetVisiblePlayer()])
		return FALSE;

	sint32 cities = g_player[g_selected_item->GetVisiblePlayer()]->GetNumCities();

	if ( cities ) {
		return TRUE;
	}
	else {
		
		
		MessageBoxDialog::Information(s_statsString->GetString( SW_NO_CITIES ),"SW_NO_CITIES",NULL, NULL, "str_ldl_MB_OK", false);
		return FALSE;
	}
}

BOOL StatsWindow::CheckUnit( void ) 
{
	sint32 units = g_player[g_selected_item->GetVisiblePlayer()]->m_all_units->Num(); 

	if ( units ) {
		return TRUE;
	}
	else {
		
		
		MessageBoxDialog::Information(s_statsString->GetString( SW_NO_UNITS ),"SW_NO_UNITS",NULL, NULL, "str_ldl_MB_OK", false);
		return FALSE;
	}
}

BOOL StatsWindow::CheckDiplomacy( void ) 
{
	sint32 player = g_selected_item->GetVisiblePlayer();

	if(!g_player[player])
		return FALSE;

	for ( sint32 i = 1;i < k_MAX_PLAYERS;i++ ) {
		if ( g_player[i] && i != player && !g_player[i]->IsDead() ) {
			if ( g_player[player]->HasContactWith(i) ) {
				return TRUE;
			}
		}
	}
	
	
	
	MessageBoxDialog::Information(s_statsString->GetString( SW_NO_DIPLOMACY ),"SW_NO_DIPLOMACY",NULL, NULL, "str_ldl_MB_OK", false);
	return FALSE;
}









































































































AUI_ERRCODE StatsWindow::Idle( void )
{

	return AUI_ERRCODE_OK;
}

sint32 statswindow_EnableDiplomacy( void ) 
{
	s_statsButton[BUTTON_DIP]->Enable( TRUE );
	return 1;
}

sint32 statswindow_EnableTrade( void )
{
	s_statsButton[BUTTON_TRADE]->Enable( TRUE );
	return 1;
}

sint32 statswindow_EnableCityButtons( void )
{
	s_statsButton[BUTTON_CITY]->Enable( TRUE );
	s_statsButton[BUTTON_CIV]->Enable( TRUE );
	s_statsButton[BUTTON_SCI]->Enable( TRUE );
	return 1;
}

BOOL statswindow_IsTradeDisabled( void )
{
	return s_statsButton[BUTTON_TRADE]->IsDisabled();
}

sint32 statswindow_DisableScreensButton( void )
{
	
	return 1;
}

sint32 statswindow_EnableScreensButton( void )
{
	
	return 1;
}

