//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Knowledge window. Unused CTP1 leftover?
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Use the same science percentage everywhere.
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "aui.h"
#include "aui_uniqueid.h"
#include "c3ui.h"
#include "aui_ldl.h"
#include "aui_static.h"
#include "c3_static.h"
#include "tech_wllist.h"

#include "c3dropdown.h"
#include "aui_listbox.h"
#include "c3_ListBox.h"
#include "C3DropDown.h"

#include "ControlSheet.h"
#include "StaticTextBox.h"
#include "c3_HyperTextBox.h"

#include "pixelutils.h"
#include "c3_Switch.h"
#include "colorset.h"
#include "tileset.h"
#include "c3_icon.h"

#include "C3Slider.h"
#include "Thermometer.h"


#include "textbutton.h"
#include "textswitch.h"
#include "aui_switchgroup.h"
#include "picturebutton.h"
#include "aui_Button.h"
#include "c3_Button.h"

#include "aui_listbox.h"
#include "c3_listbox.h"

#include "StrDB.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "Unit.h"
#include "UnitData.h"
#include "CityData.h"
#include "Advances.h"
#include "Globals.h"
#include "AdvanceRecord.h"

#include "Player.h"
#include "Readiness.h"
#include "SelItem.h"
#include "ObjPool.h"

#include "debugwindow.h"

#include "unitdata.h"
#include "Player.h"
#include "PlayHap.h"
#include "SelItem.h"
#include "sci.h"

#include "chart.h"


#include "pixelutils.h"


#include "kss_advancescreen.h"

#include "greatlibrary.h"
#include "greatlibrarywindow.h"
#include "c3windows.h"
#include "knowledgewindow.h"
#include "knowledgewin.h"


extern C3UI					*g_c3ui;
extern Player				**g_player;
extern SelectedItem			*g_selected_item; 
extern DebugWindow			*g_debugWindow;
extern StringDB				*g_theStringDB;
extern GreatLibraryWindow	*g_greatLibrary;
extern ColorSet				*g_colorSet;

extern aui_Surface			*g_sharedSurface;


KnowledgeWindow		*g_knowledgeWindow = NULL;







static c3_Button		*s_returnButton;


static c3_Button		*s_libraryButton;

static c3_Static		*s_researchBox;
static c3_Static		*s_researchText;


static c3_Static		*s_turnsText;
static c3_Static		*s_turnsNumText;
static Thermometer		*s_barThermometer;

static c3_Button		*s_changeButton;
static c3_Static		*s_costLabel;
static c3_Static		*s_costBox;
static c3_Static		*s_sciLabel;
static c3_Static		*s_sciBox;
static c3_Button		*s_plusButton;
static c3_Button		*s_minusButton;
static c3_Static		*s_sciPercentLabel;
static c3_Static		*s_sciPercentBox;

static sint32			s_scienceTax;
























static Chart			*s_scrollingChart;

















static c3_Static	*s_lt;
static c3_Static	*s_ct;
static c3_Static	*s_rt;
static c3_Static	*s_left;
static c3_Static	*s_right;
static c3_Static	*s_bottom;

static c3_Static	*s_listtop;
static c3_Static	*s_listbl;
static c3_Static	*s_listbc;
static c3_Static	*s_listbr;

static c3_Static	*s_titleText;



#define	k_PLAYERS		7

static sint32		s_playerSwitch[k_PLAYERS];

static MBCHAR			s_ldlBlocks[k_PLAYERS][50] = {
	"PlayerOne",
	"PlayerTwo",
	"PlayerThree",
	"PlayerFour",
	"PlayerFive",
	"PlayerSix",
	"PlayerSeven"
};

static MBCHAR			s_flagBlocks[k_PLAYERS][50] = {
	"FlagOne",
	"FlagTwo",
	"FlagThree",
	"FlagFour",
	"FlagFive",
	"FlagSix",
	"FlagSeven"
};

static c3_HyperTextBox	*s_givesBox;

static c3_Static		*s_civBox;
static c3_Static		*s_civText;
static c3_Switch		**s_playerText;
static c3_Icon			**s_playerFlag;
static c3_ListBox		*s_advanceList;


extern sint32			g_modalWindow;

void knowledgewin_ReturnButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( g_knowledgeWindow->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;

	
	g_modalWindow--;
}

void knowledgewin_LibraryButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AUI_ERRCODE auiErr;
	
	sint32 curPlayer = g_selected_item->GetVisiblePlayer();
	sint32 curIndex = g_player[curPlayer]->m_advances->GetResearching();

	if (!g_greatLibrary) GreatLibraryWindow_Initialize( curIndex );
		

	auiErr = g_c3ui->AddWindow( g_greatLibrary );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;
}

void knowledgewin_ChangeButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	kss_advancescreen_displayMyWindow();
}

void knowledgewin_PlayerActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	switch ( action ) {
	case AUI_SWITCH_ACTION_ON:
		knowledgewin_UpdateFromSwitch();
		break;

	case AUI_SWITCH_ACTION_OFF:
		
		if ( s_advanceList ) {
			BOOL otherSwitchIsOn = FALSE;
			
			for ( sint32 i = 0;i < k_PLAYERS;i++ ) {
				if ( s_playerText[i]->IsOn() )
					otherSwitchIsOn = TRUE;
			}

			if ( otherSwitchIsOn ) {
				knowledgewin_UpdateFromSwitch();
			}
			else {
				knowledgewin_UpdateList();
			}
			break;
		}
	}
}

void knowledgewin_SciButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Player *p = g_player[g_selected_item->GetVisiblePlayer()];

	if ( (c3_Button *)control == s_plusButton ) {
		
		s_scienceTax += 10;
		
		
		if ( s_scienceTax > 100 )
			s_scienceTax = 100;
		
		
		double taxRate = (double)s_scienceTax / 100;
		p->SetTaxes( taxRate );

		
		knowledgewin_UpdateData(1);
	}
	else if ( (c3_Button *)control == s_minusButton ) {
		
		s_scienceTax -= 10;
		
		
		if ( s_scienceTax < 0 )
			s_scienceTax = 0;

		
		double taxRate = (double)s_scienceTax / 100;
		p->SetTaxes( taxRate );

		
		knowledgewin_UpdateData(1);
	}
}




















void knowledgewin_PrereqActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Chart *chart = (Chart *)cookie;
	sint32 numPreReq = chart->GetNumPreReq();

	for ( sint32 i = 0;i < numPreReq;i++ ) {
		if ( control->Id() == chart->GetPreReqButton(i)->Id() ) {
			g_knowledgeWindow->SetTechMode( chart->GetPreReqIndex(i), DATABASE_ADVANCES );
			s_scrollingChart->Update( chart->GetPreReqIndex(i) );
			g_knowledgeWindow->LoadGivesText();
		}
	}

}

void knowledgewin_LeadsToActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Chart *chart = (Chart *)cookie;
	sint32 numLeadsTo = chart->GetNumLeadsTo();

	for ( sint32 i = 0;i < numLeadsTo;i++ ) {
		if ( control->Id() == chart->GetLeadsToButton(i)->Id() ) {
			g_knowledgeWindow->SetTechMode( chart->GetLeadsToIndex(i), DATABASE_ADVANCES );
			s_scrollingChart->Update( chart->GetLeadsToIndex(i) );
			g_knowledgeWindow->LoadGivesText();
		}
	}

}

void knowledgewin_AdvanceListActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) return;
	if ( !s_advanceList ) return;

	AdvanceListItem *item;
	item = (AdvanceListItem *)s_advanceList->GetSelectedItem();

	if ( item ) {
		g_knowledgeWindow->SetTechMode( item->GetIndex(), DATABASE_ADVANCES );
		s_scrollingChart->Update( item->GetIndex() );
		g_knowledgeWindow->LoadGivesText();
	}
}

sint32 knowledgewin_UpdateFromSwitch( void )
{
	AUI_ERRCODE errcode;
	MBCHAR		ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sint32		curPlayer = g_selected_item->GetVisiblePlayer();
	sint32		num = g_theAdvanceDB->NumRecords();

	
	s_advanceList->Clear();

	sprintf( ldlBlock, "AdvanceListItem" );

	for ( sint32 i = 0;i < num;i++ ) {
		for ( sint32 k = 0;k < k_PLAYERS;k++ ) {
			
			if ( s_playerText[k]->IsOn() ) {
				if ( g_player[s_playerSwitch[k]]->HasAdvance(i) ) {
					AdvanceListItem *item = new AdvanceListItem( &errcode, i, ldlBlock );
					s_advanceList->AddItem( (c3_ListItem *)item );	
					break;
				}
			}
		}
	}

	return 0;
}

sint32 knowledgewin_UpdateList( void )
{
	AUI_ERRCODE errcode;
	MBCHAR		ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sint32		curPlayer = g_selected_item->GetVisiblePlayer();

	sint32		num = g_theAdvanceDB->NumRecords();

	
	s_advanceList->Clear();

	sprintf( ldlBlock, "AdvanceListItem" );

	for ( sint32 i = 0;i < num;i++ ) {

		if ( g_player[curPlayer]->HasAdvance(i) ) {
			AdvanceListItem *item = new AdvanceListItem( &errcode, i, ldlBlock );
			s_advanceList->AddItem( (c3_ListItem *)item );	
		}
		else {
			for ( sint32 j = 0;j < k_MAX_PLAYERS;j++ ) {
				if ( j != curPlayer && g_player[j]) {
					if ( g_player[curPlayer]->HasEmbassyWith(j) && g_player[j]->HasAdvance(i) ) {
						AdvanceListItem *item = new AdvanceListItem( &errcode, i, ldlBlock );
						s_advanceList->AddItem( (c3_ListItem *)item );	
						break;
					}
				}
			}
		}
	}

	return 0;
}




























































sint32 knowledgewin_UpdateData( sint32 flag )
{
	MBCHAR str[_MAX_PATH];

	sint32 curPlayer = g_selected_item->GetVisiblePlayer();
	Player *p = g_player[g_selected_item->GetVisiblePlayer()];

	
	sint32 researching = g_player[curPlayer]->m_advances->GetResearching();

	sprintf( str, "%s", g_theAdvanceDB->GetNameStr(researching) );
	s_researchBox->SetText( str );

	
	sint32 sciLevel = p->GetCurrentScienceLevel();
	sint32 advanceCost = p->GetCurrentScienceCost();

	
	s_barThermometer->SetPercentFilled( (sciLevel * 100) / advanceCost );

	
	sprintf( str, "%d", advanceCost );
	s_costBox->SetText( str );

	
	sprintf( str, "%d", sciLevel );
	s_sciBox->SetText( str );
		
	double scienceTax;
	
	p->GetScienceTaxRate( scienceTax );
	s_scienceTax = AsPercentage(scienceTax);

	sprintf(str,"%d",s_scienceTax);
	s_sciPercentBox->SetText(str);

	sint32 advanceTurns = p->m_advances->TurnsToNextAdvance();

	
	if ( advanceTurns == -1 ) {
		sprintf( str, "-" );
	}
	else {
		sprintf( str, "%d", advanceTurns + 1 );
	}

	s_turnsNumText->SetText( str );

	if ( flag == 1) return 0;	

	s_scrollingChart->Update(researching );

	g_knowledgeWindow->SetTechMode( researching, DATABASE_ADVANCES );

	
	g_knowledgeWindow->LoadGivesText();

	if ( flag == 2 ) return 0;	
	
	knowledgewin_UpdateList();

	sint32 x = 0;
	for ( sint32 i = 0;i < k_MAX_PLAYERS;i++ ) {
		if ( i != curPlayer && g_player[i]) {
			if ( g_player[curPlayer]->HasEmbassyWith(i) ) {
				g_player[i]->GetPluralCivName( str );
				s_playerText[x]->SetText( str );
				s_playerText[x]->Show();
				s_playerFlag[x]->SetMapIcon( MAPICON_FLAG );
				s_playerFlag[x]->SetColor( g_colorSet->ComputePlayerColor(i) );
				s_playerSwitch[x] = i;
				x++;
			}
		}
	}

	while ( x < k_PLAYERS ) {
		s_playerText[x]->Hide();
		s_playerText[x]->SetText( "" );
		s_playerFlag[x]->SetMapIcon( MAPICON_MAX );
		s_playerSwitch[x] = -1;
		x++;
	}

	return 0;
}

sint32 knowledgewin_Initialize( void )
{

	AUI_ERRCODE		errcode;
	MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	MBCHAR			controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( g_knowledgeWindow ) {
		g_modalWindow++;

		knowledgewin_UpdateData(0);
		return 0; 
	}

	
	strcpy(windowBlock, "KnowledgeScreen");

	g_knowledgeWindow = new KnowledgeWindow(&errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING );
	Assert( AUI_NEWOK(g_knowledgeWindow, errcode) );
	if ( !AUI_NEWOK(g_knowledgeWindow, errcode) ) return -1;

	g_knowledgeWindow->SetSurface( g_sharedSurface );























	
	sprintf( buttonBlock, "%s.%s", windowBlock, "ReturnButton" );
	s_returnButton = new c3_Button( &errcode, aui_UniqueId(), buttonBlock, knowledgewin_ReturnButtonActionCallback );
	Assert( AUI_NEWOK(s_returnButton, errcode) );
	if ( !AUI_NEWOK(s_returnButton, errcode) ) return -2;







	
	sprintf( buttonBlock, "%s.%s", windowBlock, "LibraryButton" );
	s_libraryButton = new c3_Button( &errcode, aui_UniqueId(), buttonBlock, knowledgewin_LibraryButtonActionCallback );
	Assert( AUI_NEWOK(s_libraryButton, errcode) );
	if ( !AUI_NEWOK(s_libraryButton, errcode) ) return -2;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "ResearchBox" );
	s_researchBox = new c3_Static( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_researchBox, errcode) );
	if ( !AUI_NEWOK(s_researchBox, errcode) ) return -4;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "ResearchText" );
	s_researchText = new c3_Static( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_researchText, errcode) );
	if ( !AUI_NEWOK(s_researchText, errcode) ) return -3;













	
	sprintf( buttonBlock, "%s.%s", windowBlock, "TurnsText" );
	s_turnsText = new c3_Static( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_turnsText, errcode) );
	if ( !AUI_NEWOK(s_turnsText, errcode) ) return -3;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "TurnsNumText" );
	s_turnsNumText = new c3_Static( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_turnsNumText, errcode) );
	if ( !AUI_NEWOK(s_turnsNumText, errcode) ) return -4;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "BarThermometer" );
	s_barThermometer = new Thermometer( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_barThermometer, errcode) );
	if ( !AUI_NEWOK(s_barThermometer, errcode) ) return -3;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "ChangeButton" );
	s_changeButton = new c3_Button( &errcode, aui_UniqueId(), buttonBlock, knowledgewin_ChangeButtonActionCallback );
	Assert( AUI_NEWOK(s_changeButton, errcode) );
	if ( !AUI_NEWOK(s_changeButton, errcode) ) return -4;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "CostLabel" );
	s_costLabel = new c3_Static( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_costLabel, errcode) );
	if ( !AUI_NEWOK(s_costLabel, errcode) ) return -5;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "CostBox" );
	s_costBox = new c3_Static( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_costBox, errcode) );
	if ( !AUI_NEWOK(s_costBox, errcode) ) return -5;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "SciLabel" );
	s_sciLabel = new c3_Static( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_sciLabel, errcode) );
	if ( !AUI_NEWOK(s_sciLabel, errcode) ) return -5;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "SciBox" );
	s_sciBox = new c3_Static( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_sciBox, errcode) );
	if ( !AUI_NEWOK(s_sciBox, errcode) ) return -5;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "PlusButton" );
	s_plusButton = new c3_Button( &errcode, aui_UniqueId(), buttonBlock, knowledgewin_SciButtonActionCallback, s_plusButton );
	Assert( AUI_NEWOK(s_plusButton, errcode) );
	if ( !AUI_NEWOK(s_plusButton, errcode) ) return -4;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "MinusButton" );
	s_minusButton = new c3_Button( &errcode, aui_UniqueId(), buttonBlock, knowledgewin_SciButtonActionCallback, s_minusButton );
	Assert( AUI_NEWOK(s_minusButton, errcode) );
	if ( !AUI_NEWOK(s_minusButton, errcode) ) return -4;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "SciPercentLabel" );
	s_sciPercentLabel = new c3_Static( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_sciPercentLabel, errcode) );
	if ( !AUI_NEWOK(s_sciPercentLabel, errcode) ) return -5;

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "SciPercentBox" );
	s_sciPercentBox = new c3_Static( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_sciPercentBox, errcode) );
	if ( !AUI_NEWOK(s_sciPercentBox, errcode) ) return -5;



































































































































	sprintf( controlBlock, "%s.%s", windowBlock, "ScrollingChart" );
	s_scrollingChart = new Chart( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_scrollingChart, errcode) );
	if ( !AUI_NEWOK(s_scrollingChart, errcode) ) return -7;
	s_scrollingChart->Update(0);

	
	sint32 i;
	for ( i = 0;i < k_MAX_ADVANCE_PREREQ;i++ ) {
		s_scrollingChart->GetPreReqButton(i)->SetActionFuncAndCookie( knowledgewin_PrereqActionCallback, s_scrollingChart );
	}

	for ( i = 0;i < k_MAX_ADVANCE_LEADSTO;i++ ) {
		s_scrollingChart->GetLeadsToButton(i)->SetActionFuncAndCookie( knowledgewin_LeadsToActionCallback, s_scrollingChart );
	}

	sprintf( buttonBlock, "%s.%s", windowBlock, "GivesBox" );
	s_givesBox = new c3_HyperTextBox( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_givesBox, errcode) );
	if ( !AUI_NEWOK(s_givesBox, errcode) ) return -4;

	sprintf( buttonBlock, "%s.%s", windowBlock, "CivBox" );
	s_civBox = new c3_Static( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_civBox, errcode) );
	if ( !AUI_NEWOK(s_civBox, errcode) ) return -5;

	sprintf( controlBlock, "%s.%s", buttonBlock, "CivText" );
	s_civText = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_civText, errcode) );
	if ( !AUI_NEWOK(s_civText, errcode) ) return -6;

	s_playerText = new c3_Switch*[k_PLAYERS];
	s_playerFlag = new c3_Icon*[k_PLAYERS];

	for ( i = 0;i < k_PLAYERS;i++ ) {
		sprintf( controlBlock, "%s.%s", buttonBlock, s_ldlBlocks[i] );
		s_playerText[i] = new c3_Switch( &errcode, aui_UniqueId(), controlBlock, knowledgewin_PlayerActionCallback, s_playerText[i] );
		Assert( AUI_NEWOK(s_playerText[i], errcode) );
		if ( !AUI_NEWOK(s_playerText[i], errcode ) ) return -10;

		sprintf( controlBlock, "%s.%s", buttonBlock, s_flagBlocks[i] );
		s_playerFlag[i] = new c3_Icon( &errcode, aui_UniqueId(), controlBlock );
		Assert( AUI_NEWOK(s_playerFlag[i], errcode) );
		if ( !AUI_NEWOK(s_playerFlag[i], errcode ) ) return -10;
		s_playerFlag[i]->SetColor((COLOR)i);
		s_playerFlag[i]->SetMapIcon(MAPICON_FLAG);
	}





	sprintf( buttonBlock, "%s.%s", windowBlock, "AdvanceList" );
	s_advanceList = new c3_ListBox( &errcode, aui_UniqueId(), buttonBlock, knowledgewin_AdvanceListActionCallback);
	Assert( AUI_NEWOK(s_advanceList, errcode) );
	if ( !AUI_NEWOK(s_advanceList, errcode) ) return -11;

	sprintf( controlBlock, "%s.%s", windowBlock, "TitleText" );
	s_titleText = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_titleText, errcode ) );
	if ( !AUI_NEWOK(s_titleText, errcode) ) return -23;

	
	knowledgewin_InitGraphicTrim( windowBlock );

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return -1;

	g_knowledgeWindow->SetGivesText ( s_givesBox );

	
	g_modalWindow++;


	knowledgewin_UpdateData(0);

	return 0;
}

sint32 knowledgewin_InitGraphicTrim( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode;
	MBCHAR		imageBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( imageBlock, "%s.%s", windowBlock, "Lt" );
	s_lt = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_lt, errcode) );
	if ( !AUI_NEWOK(s_lt, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "Ct" );
	s_ct = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_ct, errcode) );
	if ( !AUI_NEWOK(s_ct, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "Rt" );
	s_rt = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_rt, errcode) );
	if ( !AUI_NEWOK(s_rt, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "LeftImage" );
	s_left = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_left, errcode) );
	if ( !AUI_NEWOK(s_left, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "RightImage" );
	s_right = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_right, errcode) );
	if ( !AUI_NEWOK(s_right, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "BottomImage" );
	s_bottom = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_bottom, errcode) );
	if ( !AUI_NEWOK(s_bottom, errcode) ) return -10;

	sprintf( imageBlock, "%s.%s", windowBlock, "ListTop" );
	s_listtop = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_listtop, errcode) );
	if ( !AUI_NEWOK(s_listtop, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "ListBl" );
	s_listbl = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_listbl, errcode) );
	if ( !AUI_NEWOK(s_listbl, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "ListBc" );
	s_listbc = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_listbc, errcode) );
	if ( !AUI_NEWOK(s_listbc, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "ListBr" );
	s_listbr = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_listbr, errcode) );
	if ( !AUI_NEWOK(s_listbr, errcode) ) return -10;

















































	return 0;
}

sint32 knowledgewin_Cleanup( void )
{
	if ( !g_knowledgeWindow ) return 0; 

	g_c3ui->RemoveWindow( g_knowledgeWindow->Id() );

	delete s_returnButton;
	s_returnButton = NULL;




	delete s_libraryButton;
	s_libraryButton = NULL;

	delete s_researchBox;
	s_researchBox = NULL;

	delete s_researchText;
	s_researchText = NULL;




	delete s_turnsText;
	s_turnsText = NULL;

	delete s_turnsNumText;
	s_turnsNumText = NULL;

	delete s_barThermometer;
	s_barThermometer = NULL;

	delete s_changeButton;
	s_changeButton = NULL;
	delete s_costLabel;
	s_costLabel = NULL;
	delete s_costBox;
	s_costBox = NULL;
	delete s_sciLabel;
	s_sciLabel = NULL;
	delete s_sciBox;
	s_sciBox = NULL;
	delete s_plusButton;
	s_plusButton = NULL;
	delete s_minusButton;
	s_minusButton = NULL;
	delete s_sciPercentLabel;
	s_sciPercentLabel = NULL;
	delete s_sciPercentBox;
	s_sciPercentBox = NULL;











































	delete s_scrollingChart;
	s_scrollingChart = NULL;






























	
	
	delete s_givesBox;
	s_givesBox = NULL;
	delete s_civBox;
	s_civBox = NULL;

	delete s_civText;
	s_civText = NULL;

	for ( sint32 i = 0;i < k_PLAYERS;i++ ) {
		delete s_playerText[i];
		s_playerText[i] = NULL;
	}

	for ( i = 0;i < k_PLAYERS;i++ ) {
		delete s_playerFlag[i];
		s_playerFlag[i] = NULL;
	}

	delete s_playerText;
	s_playerText = NULL;
	delete s_playerFlag;
	s_playerFlag = NULL;
	delete s_advanceList;
	s_advanceList = NULL;

	delete s_titleText;
	s_titleText = NULL;

	
	delete s_lt;
	s_lt = NULL;
	delete s_ct;
	s_ct = NULL;
	delete s_rt;
	s_rt = NULL;
	delete s_left;
	s_left = NULL;
	delete s_right;
	s_right = NULL;
	delete s_bottom;
	s_bottom = NULL;

	delete s_listtop;
	s_listtop = NULL;
	delete s_listbl;
	s_listbl = NULL;
	delete s_listbc;
	s_listbc = NULL;
	delete s_listbr;
	s_listbr = NULL;

	g_knowledgeWindow->SetSurface( NULL );

	delete g_knowledgeWindow;
	g_knowledgeWindow = NULL;

	return 0;
}


KnowledgeListItem::KnowledgeListItem(AUI_ERRCODE *retval, sint32 index, MBCHAR *ldlBlock)
	:
	c3_ListItem( retval, ldlBlock),
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;	
}


AUI_ERRCODE KnowledgeListItem::InitCommonLdl(sint32 index, MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	m_index = index;

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "Name");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void KnowledgeListItem::Update(void)
{


	c3_Static *subItem;

	
	subItem = (c3_Static *)GetChildByIndex(0);
	subItem->SetText( g_theAdvanceDB->GetNameStr(m_index) );
}

sint32 KnowledgeListItem::Compare(c3_ListItem *item2, uint32 column)
{
	sint32	val1, val2;

	if (column < 0) return 0;

	switch (column) {
	case 0:
		val1 = m_index; 
		val2 = ((KnowledgeListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	}

	return 0;
}


EmbassyListItem::EmbassyListItem(AUI_ERRCODE *retval, sint32 index, MBCHAR *ldlBlock)
	:
	c3_ListItem( retval, ldlBlock),
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;	
}


AUI_ERRCODE EmbassyListItem::InitCommonLdl(sint32 index, MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	m_index = index;

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "Name");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void EmbassyListItem::Update(void)
{
	MBCHAR			name[_MAX_PATH];

	c3_Static *subItem;

	g_player[m_index]->GetPluralCivName( name );

	
	subItem = (c3_Static *)GetChildByIndex(0);
	subItem->SetText( name );
}

sint32 EmbassyListItem::Compare(c3_ListItem *item2, uint32 column)
{
	sint32	val1, val2;

	if (column < 0) return 0;

	switch (column) {
	case 0:
		val1 = m_index; 
		val2 = ((EmbassyListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	}

	return 0;
}


AdvanceListItem::AdvanceListItem(AUI_ERRCODE *retval, sint32 index, MBCHAR *ldlBlock)
	:
	c3_ListItem( retval, ldlBlock),
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;	
}


AUI_ERRCODE AdvanceListItem::InitCommonLdl(sint32 index, MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	m_index = index;

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "PlayerFlag");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "SymTwo");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "Name");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "SymThree");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagOne");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagTwo");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagThree");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagFour");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagFive");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagSix");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagSeven");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void AdvanceListItem::Update(void)
{
	c3_Static *subItem;
	c3_Icon	*subIcon;

	sint32 curPlayer = g_selected_item->GetVisiblePlayer();

	
	subIcon = (c3_Icon*)GetChildByIndex(0);
	if ( g_player[curPlayer]->HasAdvance(m_index) ) {
		subIcon->SetColor( g_colorSet->ComputePlayerColor(curPlayer) );
		subIcon->SetMapIcon( MAPICON_FLAG );
	}
	else {
		subIcon->SetMapIcon( MAPICON_MAX );
	}

	
	subItem = (c3_Static *)GetChildByIndex(1);

	
	subItem = (c3_Static *)GetChildByIndex(2);
	subItem->SetText( g_theAdvanceDB->GetNameStr(m_index) );

	
	subItem = (c3_Static *)GetChildByIndex(3);

	sint32 i,x = 0;

	
	for ( i = 0;i < k_MAX_PLAYERS;i++ ) {
		if ( i != curPlayer && g_player[i]) {
			
			if (g_player[i] != 0) {
				if ( g_player[curPlayer]->HasEmbassyWith(i) ) {
					subIcon = (c3_Icon *)GetChildByIndex(x+4);
					x++;
					
					if ( g_player[i]->HasAdvance(m_index) ) {
						subIcon->SetColor( g_colorSet->ComputePlayerColor(i) );
						subIcon->SetMapIcon( MAPICON_FLAG );
					}
					
					else {
						subIcon->SetMapIcon( MAPICON_MAX );
					}
				}
			}
		}
	}

	
	while ( x < k_PLAYERS ) {
		subIcon = (c3_Icon *)GetChildByIndex(x+4);
		subIcon->SetMapIcon( MAPICON_MAX );
		x++;
	}
}

sint32 AdvanceListItem::Compare(c3_ListItem *item2, uint32 column)
{
	sint32	val1, val2;

	if (column < 0) return 0;

	switch (column) {
	case 0:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 1:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 2:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 3:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 4:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 5:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 6:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 7:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 8:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 9:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 10:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	}

	return 0;
}

