//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Old CTP1 info window, seems that most parts aren't used.
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
// - Do not trigger disaster warnings when there is no pollution at all.
// - Replaced old difficulty database by new one. (April 29th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "infowin.h"

#include "aui.h"
#include "aui_stringtable.h"
#include "aui_textfield.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_ranger.h"
#include "c3_button.h"
#include "c3_dropdown.h"
#include "c3_listbox.h"
#include "c3_static.h"
#include "c3ui.h"
#include "c3windows.h"
#include "colorset.h"               // g_colorSet
#include "controlpanelwindow.h"     // g_controlPanel
#include "controlsheet.h"
#include "Globals.h"                // allocated::clear
#include "infowindow.h"
#include "pixelutils.h"
#include "player.h"                 // g_player
#include "radarmap.h"
#include "screenutils.h"
#include "staticpicture.h"
#include "textbox.h"
#include "textbutton.h"
#include "thermometer.h"
#include "Unit.h"
#include "UnitData.h"
#include "UnitDynArr.h"
#include "World.h"                  // g_theWorld
#include "workmap.h"

#include "StrDB.h"                  // g_theStringDB
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "Advances.h"
#include "TopTen.h"
#include "AgeRecord.h"
#include "Score.h"
#include "DifficultyRecord.h"
#include "Diffcly.h"
#include "profileDB.h"              // g_theProfileDB
#include "pollution.h"
#include "EndGame.h"
#include "WonderTracker.h"
#include "Civilisation.h"
#include "CivilisationPool.h"       // g_theCivilisationPool;
#include "CivPaths.h"               // g_civPaths
#include "SelItem.h"                // g_selected_item
#include "BldQue.h"
#include "ObjPool.h"
#include "Cell.h"
#include "c3files.h"
#include "pointerlist.h"
#include "linegraph.h"
#include "TurnCnt.h"                // g_turn
#include "Strengths.h"
#include "UnitPool.h"               // g_theUnitPool
#include "keypress.h"
#include "wonderutil.h"
#include "EventTracker.h"
#include "GameSettings.h"

extern sint32                   g_ScreenWidth;
extern sint32                   g_ScreenHeight;
extern C3UI                     *g_c3ui;
extern TopTen                   *g_theTopTen;
extern PointerList<Player>      *g_deadPlayer;
extern sint32                   g_modalWindow;
extern WorkMap                  *g_workMap;
extern Pollution                *g_thePollution;


#define k_INFORADAR_WIDTH       202
#define k_INFORADAR_HEIGHT      151

ctp2_Window                     *g_infoWindow = NULL;

static c3_Button                *s_exitButton;

static sint32                   s_infoSetting;
static sint32                   s_infoDataSetting;

static aui_StringTable          *s_stringTable;

static sint32                   s_infoXCount;
static sint32                   s_infoYCount;
static double                   **s_infoGraphData;

static sint32                   s_pollutionXCount;
static sint32                   s_pollutionYCount;
static double                   **s_pollutionGraphData;

static c3_Static                *s_civNameLabel;
static c3_Static                *s_turnsLabel;
static c3_Static                *s_foundedLabel;
static c3_Static                *s_pollutionLabel;

static c3_Static                *s_civNameBox;
static c3_Static                *s_turnsBox;
static c3_Static                *s_foundedBox;
static c3_Static                *s_pollutionBox;

static c3_Static                *s_titleBox;

static c3_ListBox               *s_infoBigList;
static c3_ListBox               *s_infoWonderList;
static c3_ListBox               *s_infoPlayerList;
static c3_ListBox               *s_infoScoreList;
static c3_ListBox               *s_pollutionList;

static LineGraph                *s_infoGraph;
static LineGraph                *s_pollutionGraph;

static Thermometer              *s_pollutionTherm;

static c3_Button                *s_returnButton;
static c3_Button                *s_bigButton;
static c3_Button                *s_wonderButton;
static c3_Button                *s_strengthButton;
static c3_Button                *s_scoreButton;
static c3_Button                *s_pollutionButton;

static c3_Button                *s_eventsInfoButton[17];
static c3_Button                *s_eventsInfoButtonLeft,*s_eventsInfoButtonRight;
static sint32                   s_currentWonderDisplay;

static c3_Button                *s_labButton;
static c3_Button                *s_throneButton;

static RadarMap                 *s_infoRadar;

static c3_Static                *s_bottomRightBox;
static c3_Static                *s_bottomRightImage;

static sint32                   s_minRound = 0;

namespace
{

/// Evaluate a combined "strength" value at a given turn
/// @param  a_Strengths Information per category and turn
/// @param  a_Turn      The turn
/// @return The combined "strength" value
sint32 GetCombinedStrength(Strengths const & a_Strengths, sint32 a_Turn)
{
	return a_Strengths.GetTurnStrength(STRENGTH_CAT_UNITS,      a_Turn)
	     + a_Strengths.GetTurnStrength(STRENGTH_CAT_GOLD,       a_Turn)
	     + a_Strengths.GetTurnStrength(STRENGTH_CAT_BUILDINGS,  a_Turn)
	     + a_Strengths.GetTurnStrength(STRENGTH_CAT_WONDERS,    a_Turn)
	     + a_Strengths.GetTurnStrength(STRENGTH_CAT_PRODUCTION, a_Turn);
}

} // namespace

void InfoCleanupAction::Execute(aui_Control *control,
									uint32 action,
									uint32 data )
{
	infowin_Cleanup();
}

void InfoButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if ((c3_Button*)control == s_bigButton)
	{
		infowin_ChangeSetting(k_INFOWIN_BIG_SETTING);
	}
	else if ((c3_Button*)control == s_wonderButton)
	{
		infowin_ChangeSetting(k_INFOWIN_WONDER_SETTING);
	}
	else if ((c3_Button*)control == s_strengthButton)
	{
		infowin_ChangeSetting(k_INFOWIN_STRENGTH_SETTING);
	}
	else if ((c3_Button*)control == s_scoreButton)
	{
		infowin_ChangeSetting(k_INFOWIN_SCORE_SETTING);
	}
#if 0   // Inactive CTP1 functionality
	else if ((c3_Button*)control == s_labButton)
	{
		infowin_DisplayLab();
	}
	else if ((c3_Button*)control == s_throneButton)
	{

	}
#endif  // Inactive CTP1 functionality
	else if ((c3_Button*)control == s_pollutionButton)
	{
		infowin_ChangeSetting(k_INFOWIN_POLLUTION_SETTING);
	}
}

void EventsInfoButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if ((c3_Button*)control == s_eventsInfoButtonLeft)
	{
		s_currentWonderDisplay--;
		if(s_currentWonderDisplay<0)
			s_currentWonderDisplay=0;
		s_infoGraph->RenderGraph(s_currentWonderDisplay);
		s_infoGraph->ShouldDraw(TRUE);
	}
	else if ((c3_Button*)control == s_eventsInfoButtonRight)
	{
		s_currentWonderDisplay++;
		if(s_currentWonderDisplay>=g_eventTracker->GetEventCount())
			s_currentWonderDisplay=g_eventTracker->GetEventCount()-1;
		s_infoGraph->RenderGraph(s_currentWonderDisplay);
		s_infoGraph->ShouldDraw(TRUE);
	}
}

void InfoExitButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_c3ui->AddAction(new InfoCleanupAction());
}

void InfoBigListCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) return;
	sint32 curPlayer =  g_selected_item->GetVisiblePlayer();

	InfoBigListItem *item = (InfoBigListItem *) s_infoBigList->GetSelectedItem();
	if (!item)
	{
		infowin_ChangeDataSetting( k_INFOWIN_DATA_OFF );
	}
	else
    {
		infowin_UpdateCivData();

		infowin_ChangeDataSetting( k_INFOWIN_DATA_ON );

		Unit *unit = item->GetCity();

		if (curPlayer == unit->GetOwner())
		{
		    MapPoint cityPos;
			unit->GetPos(cityPos);
			s_infoRadar->CenterMap(cityPos);
		}

        s_infoRadar->Update();
		s_infoRadar->Idle();
	}
}

sint32 infowin_Initialize( void )
{
	return 0;
}

void infowin_Cleanup(void)
{
}

void infowin_Cleanup_Controls(void)
{
    allocated::clear(s_titleBox);
    allocated::clear(s_bottomRightBox);
	allocated::clear(s_bottomRightImage);
	allocated::clear(s_civNameBox);
	allocated::clear(s_turnsBox);
	allocated::clear(s_foundedBox);
	allocated::clear(s_pollutionBox);
	allocated::clear(s_civNameLabel);
	allocated::clear(s_turnsLabel);
	allocated::clear(s_foundedLabel);
	allocated::clear(s_pollutionLabel);
	allocated::clear(s_pollutionTherm);
	allocated::clear(s_infoPlayerList);
	allocated::clear(s_infoBigList);
	allocated::clear(s_infoScoreList);
	allocated::clear(s_infoWonderList);
	allocated::clear(s_pollutionList);
	allocated::clear(s_infoGraph);
	allocated::clear(s_pollutionGraph);

	if (s_infoGraphData)
	{
		for( sint32 i = 0 ; i < s_infoYCount ; i++ )
		{
			delete s_infoGraphData[i];
		}
		delete [] s_infoGraphData;
		s_infoGraphData = NULL;
	}

	if (s_pollutionGraphData)
	{
		for( sint32 i = 0 ; i < s_pollutionYCount ; i++ )
		{
			delete s_pollutionGraphData[i];
		}
		delete [] s_pollutionGraphData;
		s_pollutionGraphData = NULL;
	}

	allocated::clear(s_bigButton);
	allocated::clear(s_wonderButton);
	allocated::clear(s_strengthButton);
	allocated::clear(s_scoreButton);
	allocated::clear(s_labButton);
	allocated::clear(s_throneButton);
	allocated::clear(s_pollutionButton);
    allocated::clear(s_stringTable);
}

sint32 infowin_Init_Controls( MBCHAR *windowBlock )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	MBCHAR			controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			controlSubBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( controlBlock, "%s.%s", windowBlock, "TitleBox" );
	s_titleBox = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(s_titleBox, errcode) );
	if ( !AUI_NEWOK(s_titleBox, errcode) ) return -1;
	s_titleBox->SetBlindness( TRUE );

	sprintf( controlBlock, "%s.%s", windowBlock, "BottomRightBox" );
	s_bottomRightBox = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(s_bottomRightBox, errcode) );
	if ( !AUI_NEWOK(s_bottomRightBox, errcode) ) return -1;

	strcpy(controlSubBlock, controlBlock);
	sprintf( controlBlock, "%s.%s", controlSubBlock, "BottomRightImage" );
	s_bottomRightImage = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(s_bottomRightImage, errcode) );
	if ( !AUI_NEWOK(s_bottomRightImage, errcode) ) return -1;






	sprintf( controlBlock, "%s.%s", controlSubBlock, "TurnsBox" );
	s_turnsBox = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(s_turnsBox, errcode) );
	if ( !AUI_NEWOK(s_turnsBox, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", controlSubBlock, "FoundedBox" );
	s_foundedBox = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(s_foundedBox, errcode) );
	if ( !AUI_NEWOK(s_foundedBox, errcode) ) return -1;






	sprintf( controlBlock, "%s.%s", controlSubBlock, "TurnsLabel" );
	s_turnsLabel = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(s_turnsLabel, errcode) );
	if ( !AUI_NEWOK(s_turnsLabel, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", controlSubBlock, "FoundedLabel" );
	s_foundedLabel = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(s_foundedLabel, errcode) );
	if ( !AUI_NEWOK(s_foundedLabel, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "PollutionBox" );
	s_pollutionBox = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(s_pollutionBox, errcode) );
	if ( !AUI_NEWOK(s_pollutionBox, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "PollutionLabel" );
	s_pollutionLabel = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(s_pollutionLabel, errcode) );
	if ( !AUI_NEWOK(s_pollutionLabel, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "PollutionTherm" );
	s_pollutionTherm = new Thermometer(&errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(s_pollutionTherm, errcode) );
	if ( !AUI_NEWOK(s_pollutionTherm, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "PollutionList" );
	s_pollutionList = new c3_ListBox(&errcode, aui_UniqueId(), controlBlock, NULL, NULL);
	Assert( AUI_NEWOK(s_pollutionList, errcode) );
	if ( !AUI_NEWOK(s_pollutionList, errcode) ) return -1;

	s_pollutionList->GetHeader()->Enable( FALSE );

	sprintf( controlBlock, "%s.%s", windowBlock, "InfoPlayerList" );
	s_infoPlayerList = new c3_ListBox(&errcode, aui_UniqueId(), controlBlock, NULL, NULL);
	Assert( AUI_NEWOK(s_infoPlayerList, errcode) );
	if ( !AUI_NEWOK(s_infoPlayerList, errcode) ) return -1;

	s_infoPlayerList->GetHeader()->Enable( FALSE );

	sprintf( controlBlock, "%s.%s", windowBlock, "InfoBigList" );
	s_infoBigList = new c3_ListBox(&errcode, aui_UniqueId(), controlBlock, InfoBigListCallback, NULL);
	Assert( AUI_NEWOK(s_infoBigList, errcode) );
	if ( !AUI_NEWOK(s_infoBigList, errcode) ) return -1;

	s_infoBigList->GetHeader()->Enable( FALSE );

	sprintf( controlBlock, "%s.%s", windowBlock, "InfoScoreList" );
	s_infoScoreList = new c3_ListBox(&errcode, aui_UniqueId(), controlBlock, NULL, NULL);
	Assert( AUI_NEWOK(s_infoScoreList, errcode) );
	if ( !AUI_NEWOK(s_infoScoreList, errcode) ) return -1;

	s_infoScoreList->Enable(FALSE);

	aui_Ranger	*s_infoScoreListRanger = s_infoScoreList->GetVerticalRanger();
	s_infoScoreListRanger->Enable(TRUE);

	sprintf( controlBlock, "%s.%s", windowBlock, "InfoWonderList" );
	s_infoWonderList = new c3_ListBox(&errcode, aui_UniqueId(), controlBlock, NULL, NULL);
	Assert( AUI_NEWOK(s_infoWonderList, errcode) );
	if ( !AUI_NEWOK(s_infoWonderList, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "InfoGraph" );
	s_infoGraph = new LineGraph(&errcode, aui_UniqueId(), controlBlock, NULL, NULL, g_eventTracker);
	Assert( AUI_NEWOK(s_infoGraph, errcode) );
	if ( !AUI_NEWOK(s_infoGraph, errcode) ) return -1;

	s_currentWonderDisplay=0;
	for(int i=0; i<17; i++)
	{
		sprintf( buttonBlock, "%s.%s%i", controlBlock, "EventsInfoButton",i+1 );
		s_eventsInfoButton[i] = new c3_Button(&errcode, aui_UniqueId(), buttonBlock, EventsInfoButtonActionCallback);
		Assert( AUI_NEWOK(s_eventsInfoButton[i], errcode) );
		if ( !AUI_NEWOK(s_eventsInfoButton[i], errcode) ) return -1;
	}

	sprintf( buttonBlock, "%s.%s", controlBlock, "EventsInfoButtonLeft");
	s_eventsInfoButtonLeft = new c3_Button(&errcode, aui_UniqueId(), buttonBlock, EventsInfoButtonActionCallback);
	Assert( AUI_NEWOK(s_eventsInfoButtonLeft, errcode) );
	if ( !AUI_NEWOK(s_eventsInfoButtonLeft, errcode) ) return -1;

	sprintf( buttonBlock, "%s.%s", controlBlock, "EventsInfoButtonRight");
	s_eventsInfoButtonRight = new c3_Button(&errcode, aui_UniqueId(), buttonBlock, EventsInfoButtonActionCallback);
	Assert( AUI_NEWOK(s_eventsInfoButtonRight, errcode) );
	if ( !AUI_NEWOK(s_eventsInfoButtonRight, errcode) ) return -1;

	s_infoGraph->EnableYNumber(FALSE);
	s_infoGraph->EnablePrecision(FALSE);

	sprintf( controlBlock, "%s.%s", windowBlock, "PollutionGraph" );
	s_pollutionGraph = new LineGraph(&errcode, aui_UniqueId(), controlBlock, NULL, NULL);
	Assert( AUI_NEWOK(s_pollutionGraph, errcode) );
	if ( !AUI_NEWOK(s_pollutionGraph, errcode) ) return -1;

	s_pollutionGraph->EnableYNumber(FALSE);
	s_pollutionGraph->EnablePrecision(FALSE);

	sprintf( buttonBlock, "%s.%s", windowBlock, "BigButton" );
	s_bigButton = new c3_Button(&errcode, aui_UniqueId(), buttonBlock, InfoButtonActionCallback);
	Assert( AUI_NEWOK(s_bigButton, errcode) );
	if ( !AUI_NEWOK(s_bigButton, errcode) ) return -1;

	sprintf( buttonBlock, "%s.%s", windowBlock, "WonderButton" );
	s_wonderButton = new c3_Button(&errcode, aui_UniqueId(), buttonBlock, InfoButtonActionCallback);
	Assert( AUI_NEWOK(s_wonderButton, errcode) );
	if ( !AUI_NEWOK(s_wonderButton, errcode) ) return -1;

	sprintf( buttonBlock, "%s.%s", windowBlock, "StrengthButton" );
	s_strengthButton = new c3_Button(&errcode, aui_UniqueId(), buttonBlock, InfoButtonActionCallback);
	Assert( AUI_NEWOK(s_strengthButton, errcode) );
	if ( !AUI_NEWOK(s_strengthButton, errcode) ) return -1;

	sprintf( buttonBlock, "%s.%s", windowBlock, "ScoreButton" );
	s_scoreButton = new c3_Button(&errcode, aui_UniqueId(), buttonBlock, InfoButtonActionCallback);
	Assert( AUI_NEWOK(s_scoreButton, errcode) );
	if ( !AUI_NEWOK(s_scoreButton, errcode) ) return -1;

	sprintf( buttonBlock, "%s.%s", windowBlock, "LabButton" );
	s_labButton = new c3_Button(&errcode, aui_UniqueId(), buttonBlock, InfoButtonActionCallback);
	Assert( AUI_NEWOK(s_labButton, errcode) );
	if ( !AUI_NEWOK(s_labButton, errcode) ) return -1;

	sprintf( buttonBlock, "%s.%s", windowBlock, "ThroneButton" );
	s_throneButton = new c3_Button(&errcode, aui_UniqueId(), buttonBlock, InfoButtonActionCallback);
	Assert( AUI_NEWOK(s_throneButton, errcode) );
	if ( !AUI_NEWOK(s_throneButton, errcode) ) return -1;

	sprintf( buttonBlock, "%s.%s", windowBlock, "PollutionButton" );
	s_pollutionButton = new c3_Button(&errcode, aui_UniqueId(), buttonBlock, InfoButtonActionCallback);
	Assert( AUI_NEWOK(s_pollutionButton, errcode) );
	if ( !AUI_NEWOK(s_pollutionButton, errcode) ) return -1;

	return 0;
}

void infowin_SetMinRoundForGraphs(sint32 minRound)
{
	s_minRound = minRound;
}

sint32 infowin_LoadData( void )
{
	infowin_UpdateBigList();
	infowin_UpdateScoreList();
	infowin_UpdateWonderList();
	infowin_UpdateGraph(s_infoGraph, s_infoXCount, s_infoYCount, &s_infoGraphData);
	infowin_UpdatePlayerList();
	infowin_UpdateCivData();
	infowin_UpdatePollutionGraph(s_pollutionGraph, s_pollutionXCount, s_pollutionYCount, &s_pollutionGraphData);
	infowin_UpdatePollutionData();

	if (!infowin_LabReady()) s_labButton->Hide();

	return 0;
}

sint32 infowin_UpdateCivData( void )
{
	MBCHAR strbuf[256];

	sint32 curPlayer =  g_selected_item->GetVisiblePlayer();

	s_foundedBox->SetText("");
	s_turnsBox->SetText("");

	Player *p = g_player[curPlayer];

	if (!p) return 0;

	if (!s_infoBigList) return 0;

	InfoBigListItem *item = (InfoBigListItem *) s_infoBigList->GetSelectedItem();
	if (!item) return 0;
	else {

		Unit *unit = item->GetCity();

		sint32 turnFounded = unit->GetData()->GetCityData()->GetTurnFounded();

		const char *yearStr = diffutil_GetYearStringFromTurn(g_theGameSettings->GetDifficulty(), turnFounded);

#if 0
		sint32 yearFounded = diffutil_GetYearFromTurn(g_theProfileDB->GetDifficulty(), turnFounded);

		if (yearFounded > 0)
		{
			sprintf(strbuf,"%d AD",yearFounded);
		}
		else
		{

			yearFounded *= -1;
			sprintf(strbuf,"%d BC",yearFounded);
		}
#endif
		s_foundedBox->SetText(yearStr);

		sint32 turnsOld = g_turn->GetRound() - turnFounded;
		sprintf(strbuf,"%d",turnsOld);
		s_turnsBox->SetText(strbuf);

	}








	return 0;
}

sint32 infowin_UpdateBigList( void )
{
	if (!g_theTopTen) return 0;
	g_theTopTen->CalculateBiggestCities();

	AUI_ERRCODE	retval;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	s_infoBigList->Clear();
	strcpy(ldlBlock,"InfoBigListItem");

	for (sint32 i = 0 ; i < 5 ; i++ )
	{
		Unit unit = g_theTopTen->GetBiggestCity(i);
		if ( g_theUnitPool->IsValid(unit) )
		{
			c3_ListItem* bItem = new InfoBigListItem(&retval, &unit, i, ldlBlock);
			s_infoBigList->AddItem(bItem);
		}
	}

	return 0;
}

sint32 infowin_UpdateScoreList( void )
{
	AUI_ERRCODE	retval;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR strbuf[256];

	sint32 curPlayer =  g_selected_item->GetVisiblePlayer();
	Player *pl = g_player[curPlayer];
	if(!pl) {
		PointerList<Player>::Walker walk(g_deadPlayer);
		while(walk.IsValid()) {
			if(walk.GetObj()->m_owner == g_selected_item->GetVisiblePlayer()) {
				pl = walk.GetObj();
				break;
			}
			walk.Next();
		}
	}
	if(!pl)
		return 0;


	s_infoScoreList->Clear();
	strcpy(ldlBlock,"InfoScoreListItem");
	InfoScoreListItem *item = NULL;
	InfoScoreLabelListItem *label = NULL;

	sint32 posValue = 0;
	sint32 negValue = 0;

	label = new InfoScoreLabelListItem(&retval, s_stringTable->GetString(0), NULL, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)label);

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_CELEBRATIONS, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_ADVANCES, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_WONDERS, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_POPULATION, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_CITIES, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_YEARS_AT_PEACE, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_YEAR_OF_VICTORY, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_DIFFICULTY_BONUS, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_MAP_SIZE, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_NUMBER_OF_OPPONENTS, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_TYPE_OF_VICTORY, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	posValue += item->GetValue();

	sprintf(strbuf,"%d",posValue);
	label = new InfoScoreLabelListItem(&retval, s_stringTable->GetString(1), strbuf, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)label);

	item = new InfoScoreListItem(&retval, -1, 0, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);

	label = new InfoScoreLabelListItem(&retval, s_stringTable->GetString(2), NULL, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)label);

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_UNITS_LOST, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	negValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_RIOTS, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	negValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_REVOLUTIONS, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	negValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_POLLUTION, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);
	negValue += item->GetValue();

	sprintf(strbuf,"%d",negValue);
	label = new InfoScoreLabelListItem(&retval, s_stringTable->GetString(3), strbuf, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)label);

	item = new InfoScoreListItem(&retval, -1, 0, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)item);

	Score *score = NULL;
	if(g_player[curPlayer]) {
		score = g_player[curPlayer]->m_score;
	} else {
		Player *deadPlayer = Player::GetDeadPlayer(curPlayer);
		Assert(deadPlayer);
		if(deadPlayer) {
			score = deadPlayer->m_score;
		}
	}
	if(score) {
		sint32 totalValue = score->GetTotalScore();
		sprintf(strbuf,"%d",totalValue);
	} else {
		strcpy(strbuf, "0");
	}

	label = new InfoScoreLabelListItem(&retval, s_stringTable->GetString(4), strbuf, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)label);

	sint32 civScore = infowin_GetCivScore(curPlayer);
	sprintf(strbuf,"%d%%",civScore);
	label = new InfoScoreLabelListItem(&retval, s_stringTable->GetString(5), strbuf, ldlBlock);
	s_infoScoreList->AddItem((c3_ListItem *)label);

	return 0;
}

sint32 infowin_UpdateWonderList( void )
{
	AUI_ERRCODE	retval;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	s_infoWonderList->Clear();
	strcpy(ldlBlock,"InfoWonderListItem");
	InfoWonderListItem *wItem = NULL;
	Unit city;

	sint32 thePlayer = 0;

	for ( sint32 i = 0; i < g_theWonderDB->NumRecords() ; i++ )
	{

		if (wonderutil_IsBuilt(i) && g_theWonderTracker->GetCityWithWonder(i, city))
		{
			thePlayer = wonderutil_GetOwner(i);
			if (thePlayer != PLAYER_INDEX_INVALID)
			{

				wItem = new InfoWonderListItem(&retval, thePlayer, i, ldlBlock);
				s_infoWonderList->AddItem((c3_ListItem *)wItem);
			}
		}
	}

	return 0;
}




sint32 infowin_UpdateGraph( LineGraph *infoGraph,
							sint32 &infoXCount,
							sint32 &infoYCount,
							double ***infoGraphData)
{
	if (!infoGraph) return 0;

	sint32 i = 0;

	sint32 maxPlayers = k_MAX_PLAYERS + g_deadPlayer->GetCount();
	sint32 *color = new sint32[maxPlayers];

	infoYCount = 0;
	infoXCount = 0;







	BOOL dumpStrings = FALSE;
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	if (!s_stringTable) {
		s_stringTable = new aui_StringTable( &errcode, "InfoStrings" );
		Assert( AUI_NEWOK(s_stringTable, errcode) );
		if ( !AUI_NEWOK(s_stringTable, errcode) ) return -2;

		dumpStrings = TRUE;
	}

	infoGraph->SetXAxisName(s_stringTable->GetString(6));
	infoGraph->SetYAxisName("Power");

	double minRound = s_minRound;
	double curRound = g_turn->GetRound();
	double minPower = 0.0;
	double maxPower = 10.0;

    infoGraph->SetGraphBounds(minRound, curRound, minPower, maxPower);
	infoGraph->HasIndicator(FALSE);

	for ( i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{
		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{
			color[infoYCount] = g_colorSet->ComputePlayerColor(i);
			infoYCount++;
		}
	}

	for
    (
	    PointerList<Player>::Walker walk(g_deadPlayer);
	    walk.IsValid();
        walk.Next()
    )
    {
		color[infoYCount] = g_colorSet->ComputePlayerColor(walk.GetObj()->GetOwner());
		infoYCount++;
	}

	infoXCount = (sint32)curRound - (sint32)minRound;

	if (!infoXCount)
	{
		delete [] color;

		infoGraph->RenderGraph();
		return 0;
	}

    infoXCount = std::max<sint32>(infoXCount, 1);
    infoYCount = std::max<sint32>(infoYCount, 1);

	Assert(!*infoGraphData);
	*infoGraphData = new double *[infoYCount];
	for (i = 0 ; i < infoYCount ; ++i)
    {
		(*infoGraphData)[i] = new double[infoXCount];
        std::fill((*infoGraphData)[i], (*infoGraphData)[i] + infoXCount, 0.0);
    }

	sint32 playerCount = 0;
	for ( i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{
		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{
			for (sint32 round = 0 ; round < infoXCount ; ++round)
			{
                sint32 strValue = GetCombinedStrength(*g_player[i]->m_strengths, round);
				(*infoGraphData)[playerCount][round] = strValue;

				while (strValue > maxPower)
					maxPower += 10.0;
			}

			playerCount++;
		}
	}

	for
    (
        PointerList<Player>::Walker walk2(g_deadPlayer);
	    walk2.IsValid();
        walk2.Next()
    )
	{
		for (sint32 round = 0 ; round < infoXCount ; ++round)
		{
			sint32 strValue = GetCombinedStrength(*walk2.GetObj()->m_strengths, round);
			(*infoGraphData)[playerCount][round] = strValue;

			while (strValue > maxPower)
				maxPower += 10.0;
		}

		playerCount++;
	}

	Assert(playerCount == infoYCount);

	infoGraph->SetLineData(infoYCount, infoXCount, (*infoGraphData), color);
	infoGraph->SetGraphBounds(minRound, curRound, minPower, maxPower);
	infoGraph->RenderGraph();

	delete [] color;

	if (dumpStrings)
    {
        allocated::clear(s_stringTable);
	}

	return 0;
}




sint32 infowin_UpdatePollutionGraph( LineGraph *infoGraph,
							sint32 &infoXCount,
							sint32 &infoYCount,
							double ***infoGraphData)
{
	if (!infoGraph) return 0;

	infoYCount = 0;
	infoXCount = 0;

	infoGraph->SetXAxisName(s_stringTable->GetString(6));
	infoGraph->SetYAxisName("Pollution");

	double curRound = g_turn->GetRound();
    double minRound = std::max<double>(0.0, curRound - 20.0);
	double minPower = 0.0;
	double maxPower = 10.0;

	infoGraph->SetGraphBounds(minRound, curRound, minPower, maxPower);
	infoGraph->HasIndicator(FALSE);

	sint32 color[k_MAX_PLAYERS];

	sint32 i;
	for ( i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{
		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{
			color[infoYCount] = g_colorSet->ComputePlayerColor(i);
			infoYCount++;
		}
	}

    infoXCount = std::min<sint32>(20, (sint32) curRound);

	if (!infoXCount)
	{
		infoGraph->RenderGraph();
		return 0;
	}

	Assert(!*infoGraphData);
	*infoGraphData = new double *[infoYCount];
	for (i = 0 ; i < infoYCount ; ++i)
    {
		(*infoGraphData)[i] = new double[infoXCount];
        std::fill((*infoGraphData)[i], (*infoGraphData)[i] + infoXCount, 0.0);
	}

	sint32 playerCount = 0;
	for ( i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{
		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{
			for (sint32 j = 0 ; j < infoXCount ; j++ )
			{
				sint32 pollutionValue =
                    g_player[i]->m_pollution_history[(infoXCount - 1) - j];
				(*infoGraphData)[playerCount][j] = pollutionValue;

                while (pollutionValue > maxPower)
					maxPower += 10.0;
			}

			playerCount++;
		}
	}

	Assert(playerCount == infoYCount);

	infoGraph->SetLineData(infoYCount, infoXCount, (*infoGraphData), color);
	infoGraph->SetGraphBounds(minRound, curRound, minPower, maxPower);
	infoGraph->RenderGraph();

	return 0;
}

sint32 infowin_UpdatePlayerList( void )
{
	AUI_ERRCODE	retval;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR strbuf[256];

	s_infoPlayerList->Clear();
	strcpy(ldlBlock,"InfoPlayerListItem");

    sint32 color = 0;

	LineGraphData *myData = s_infoGraph->GetData();

	sint32 lineIndex = 0;

	Civilisation *civ = NULL;

	for ( sint32 i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{

		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{

			if (!myData)
				color = (sint32)g_colorSet->ComputePlayerColor(i);
			else color = myData[lineIndex++].color;

			civ = g_player[i]->GetCivilisation();
			if (civ != NULL && g_theCivilisationPool->IsValid(*civ)) {
				civ->GetSingularCivName(strbuf);

				s_infoPlayerList->AddItem
                    ((c3_ListItem *) new InfoPlayerListItem(&retval, strbuf, color, ldlBlock));
			}
		}
	}

	for
    (
        PointerList<Player>::Walker walk(g_deadPlayer);
        walk.IsValid();
        walk.Next()
    )
    {
		Player * p = walk.GetObj();

		if (p)
        {
		    if (myData)
            {
                color = myData[lineIndex++].color;
            }
			else
            {
                color = (sint32)g_colorSet->ComputePlayerColor(p->GetOwner());
            }

			civ = p->GetCivilisation();
			if (civ && g_theCivilisationPool->IsValid(*civ))
            {
				civ->GetSingularCivName(strbuf);

				s_infoPlayerList->AddItem
                    ((c3_ListItem *) new InfoPlayerListItem(&retval, strbuf, color, ldlBlock));
			}
		}
	}

	return 0;
}

sint32 infowin_UpdatePollutionData( void )
{
	AUI_ERRCODE	retval;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR strbuf[256];

	s_pollutionList->Clear();
	strcpy(ldlBlock, "InfoPlayerListItem");


	sint32 color = 0;

	LineGraphData *myData = s_pollutionGraph->GetData();

	sint32 lineIndex = 0;

	for ( sint32 i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{

		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{

			if (!myData)
				color = (sint32)g_colorSet->ComputePlayerColor(i);
			else color = myData[lineIndex++].color;

			Civilisation * civ = g_player[i]->GetCivilisation();
			civ->GetSingularCivName(strbuf);

			s_pollutionList->AddItem
                ((c3_ListItem *) new InfoPlayerListItem(&retval, strbuf, color, ldlBlock));
		}
	}


	sint32 turnsLeft = g_thePollution->GetRoundsToNextDisaster();
	sint32 percent;

	if ((turnsLeft < 0) || (turnsLeft >= Pollution::ROUNDS_COUNT_IMMEASURABLE))
	{
		strcpy(strbuf, "-");
		percent	= 0;
	}
	else
	{
		sprintf(strbuf, "%d", turnsLeft);
		double const	val0 = g_thePollution->GetGlobalPollutionLevel();
		double const	val1 = g_thePollution->GetNextTrigger();
		percent = static_cast<sint32>(100.0 * (val0 / val1));
	}

	s_pollutionBox->SetText(strbuf);
	s_pollutionTherm->SetPercentFilled(percent);

	return 0;
}


sint32 infowin_ChangeSetting( sint32 type )
{
	if (s_infoSetting == type ) return type;

	sint32 oldType = s_infoSetting;

	s_infoSetting = type;

	InfoBigListItem *bItem = NULL;
	InfoWonderListItem *wItem = NULL;

	switch(type)
	{
	case k_INFOWIN_SCORE_SETTING:

		s_infoWonderList->Hide();
		s_infoPlayerList->Hide();
		s_infoGraph->Hide();
		s_infoBigList->Hide();
		s_infoRadar->Hide();
		s_infoScoreList->Show();
		s_bottomRightBox->Hide();

		s_pollutionGraph->Hide();
		s_pollutionList->Hide();
		s_pollutionLabel->Hide();
		s_pollutionBox->Hide();
		s_pollutionTherm->Hide();

		infowin_ChangeDataSetting( k_INFOWIN_DATA_OFF );

		bItem = (InfoBigListItem *) s_infoBigList->GetSelectedItem();
		if (bItem) s_infoBigList->DeselectItem(bItem);

		wItem = (InfoWonderListItem *) s_infoWonderList->GetSelectedItem();
		if (wItem) s_infoWonderList->DeselectItem(wItem);

		break;
	case k_INFOWIN_BIG_SETTING:

		s_infoWonderList->Hide();
		s_infoPlayerList->Hide();
		s_infoGraph->Hide();
		s_infoBigList->Show();
		s_infoRadar->Show();
		s_infoScoreList->Hide();
		s_bottomRightBox->Show();

		s_pollutionGraph->Hide();
		s_pollutionList->Hide();
		s_pollutionLabel->Hide();
		s_pollutionBox->Hide();
		s_pollutionTherm->Hide();

		infowin_ChangeDataSetting( k_INFOWIN_DATA_OFF );

		wItem = (InfoWonderListItem *) s_infoWonderList->GetSelectedItem();
		if (wItem) s_infoWonderList->DeselectItem(wItem);

		break;
	case k_INFOWIN_WONDER_SETTING:

		s_infoWonderList->Show();
		s_infoPlayerList->Hide();
		s_infoGraph->Hide();
		s_infoBigList->Hide();
		s_infoRadar->Hide();
		s_infoScoreList->Hide();
		s_bottomRightBox->Hide();

		s_pollutionGraph->Hide();
		s_pollutionList->Hide();
		s_pollutionLabel->Hide();
		s_pollutionBox->Hide();
		s_pollutionTherm->Hide();

		infowin_ChangeDataSetting( k_INFOWIN_DATA_OFF );

		bItem = (InfoBigListItem *) s_infoBigList->GetSelectedItem();
		if (bItem) s_infoBigList->DeselectItem(bItem);

		break;
	case k_INFOWIN_STRENGTH_SETTING:

		s_infoWonderList->Hide();
		s_infoPlayerList->Show();
		s_infoGraph->Show();
		s_infoBigList->Hide();
		s_infoRadar->Hide();
		s_infoScoreList->Hide();
		s_bottomRightBox->Hide();

		s_pollutionGraph->Hide();
		s_pollutionList->Hide();
		s_pollutionLabel->Hide();
		s_pollutionBox->Hide();
		s_pollutionTherm->Hide();

		infowin_ChangeDataSetting( k_INFOWIN_DATA_OFF );

		bItem = (InfoBigListItem *) s_infoBigList->GetSelectedItem();
		if (bItem) s_infoBigList->DeselectItem(bItem);

		wItem = (InfoWonderListItem *) s_infoWonderList->GetSelectedItem();
		if (wItem) s_infoWonderList->DeselectItem(wItem);

		break;
	case k_INFOWIN_POLLUTION_SETTING:

		s_infoWonderList->Hide();
		s_infoPlayerList->Hide();
		s_infoGraph->Hide();
		s_infoBigList->Hide();
		s_infoRadar->Hide();
		s_infoScoreList->Hide();
		s_bottomRightBox->Hide();

		s_pollutionBox->Show();
		s_pollutionLabel->Show();
		s_pollutionTherm->Show();
		s_pollutionGraph->Show();
		s_pollutionList->Show();

		infowin_ChangeDataSetting( k_INFOWIN_DATA_OFF );

		bItem = (InfoBigListItem *) s_infoBigList->GetSelectedItem();
		if (bItem) s_infoBigList->DeselectItem(bItem);

		wItem = (InfoWonderListItem *) s_infoWonderList->GetSelectedItem();
		if (wItem) s_infoWonderList->DeselectItem(wItem);

		break;
	}

	return oldType;
}

sint32 infowin_ChangeDataSetting( sint32 type )
{
	sint32 oldType = s_infoDataSetting;





	s_infoDataSetting = type;

	switch(type)
	{
	case k_INFOWIN_DATA_OFF:




		s_foundedBox->Hide();
		s_turnsBox->Hide();

		s_foundedLabel->Hide();
		s_turnsLabel->Hide();

		s_infoRadar->Update();

		s_infoRadar->Idle();

		break;
	case k_INFOWIN_DATA_ON:




		s_foundedBox->Show();
		s_turnsBox->Show();

		s_foundedLabel->Show();
		s_turnsLabel->Show();
		break;
	}

	return oldType;
}

sint32 infowin_GetCivScore( sint32 player )
{
	Player *pl = g_player[player];
	if(!pl) {
		PointerList<Player>::Walker walk(g_deadPlayer);
		while(walk.IsValid()) {
			if(walk.GetObj()->m_owner == player) {
				pl = walk.GetObj();
				break;
			}
			walk.Next();
		}
	}
	if (!pl) return 0;

	Score *         score   = pl->m_score;
	Difficulty *    diff    = pl->m_difficulty;

	return score->GetTotalScore() + diff->GetBaseScore();
}

/// Open alien life window (removed CTP1 functionality)
sint32 infowin_DisplayLab()
{
	return open_EndGame();
}

sint32 infowin_LabReady()
{
#if 0   // Old CTP1 functionality, does nothing worthwhile

	sint32 curPlayer =  g_selected_item->GetVisiblePlayer();
	Player *pl = g_player[curPlayer];
	if(!pl) {
		PointerList<Player>::Walker walk(g_deadPlayer);
		while(walk.IsValid()) {
			if(walk.GetObj()->m_owner == g_selected_item->GetVisiblePlayer()) {
				pl = walk.GetObj();
				break;
			}
			walk.Next();
		}
	}
	if(!pl)
		return 0;

	Player *p = g_player[curPlayer];
	if(!p) {
		p = Player::GetDeadPlayer(curPlayer);
		Assert(p);
		if(!p)
			return FALSE;
	}

//	for (sint32 i = 0; i < g_theEndGameDB->m_nRec; i++)
//	{


//	}

#endif // 0

	return FALSE;
}

sint32 infowin_GetWonderCityName( sint32 index, MBCHAR *name)
{
	Unit city;

	if (g_theWonderTracker->GetCityWithWonder( index, city ))
	{
		strcpy(name, city.GetData()->GetCityData()->GetName());
	}
	else
	{
		strcpy(name, "NULL");
	}

	return 0;
}







InfoBigListItem::InfoBigListItem(AUI_ERRCODE *retval, Unit *city, sint32 index, const MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (const MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(city, index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
}

InfoBigListItem::~InfoBigListItem()
{
	ListPos position = m_childList->GetHeadPosition();

	for ( sint32 i = m_childList->L(); i; i-- ) {
		aui_Region		*subControl;

		subControl = m_childList->GetNext( position );
		if (subControl) {
			ListPos	subPos = subControl->ChildList()->GetHeadPosition();

			for (sint32 j = subControl->ChildList()->L(); j; j--) {
				delete subControl->ChildList()->GetNext(subPos);
			}
		}
		delete subControl;
	}

	m_childList->DeleteAll();
}

AUI_ERRCODE InfoBigListItem::InitCommonLdl(Unit *city, sint32 index, const MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			subBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;
	MBCHAR			strbuf[256];

	if (!g_theUnitPool->IsValid(*city)) return AUI_ERRCODE_OK;

	m_city = *city;

	m_index = index;

	CityData *cd = m_city.GetData()->GetCityData();
	strcpy(m_name,cd->GetName());

	m_size = cd->PopCount();

	Player *p = g_player[cd->GetOwner()];
	Civilisation *civ = p->GetCivilisation();

	civ->GetSingularCivName(strbuf);
	sprintf(m_civ_name,"%s", strbuf);

	c3_Static		*subItem;
	c3_Static		*iconItem;

	sprintf(block, "%s.%s", ldlBlock, "CityName");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "CivName");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "PopSize");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "WonderBlock");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	strcpy(subBlock,block);
	sprintf(block, "%s.%s", subBlock, "CountBlock1");
	iconItem = new c3_Static(&retval, aui_UniqueId(), block);
	subItem->AddChild(iconItem);

	sprintf(block, "%s.%s", subBlock, "IconBlock1");
	iconItem = new c3_Static(&retval, aui_UniqueId(), block);
	subItem->AddChild(iconItem);

	sprintf(block, "%s.%s", subBlock, "CountBlock2");
	iconItem = new c3_Static(&retval, aui_UniqueId(), block);
	subItem->AddChild(iconItem);

	sprintf(block, "%s.%s", subBlock, "IconBlock2");
	iconItem = new c3_Static(&retval, aui_UniqueId(), block);
	subItem->AddChild(iconItem);

	sprintf(block, "%s.%s", subBlock, "CountBlock3");
	iconItem = new c3_Static(&retval, aui_UniqueId(), block);
	subItem->AddChild(iconItem);

	sprintf(block, "%s.%s", subBlock, "IconBlock3");
	iconItem = new c3_Static(&retval, aui_UniqueId(), block);
	subItem->AddChild(iconItem);

	sprintf(block, "%s.%s", subBlock, "CountBlock4");
	iconItem = new c3_Static(&retval, aui_UniqueId(), block);
	subItem->AddChild(iconItem);

	sprintf(block, "%s.%s", subBlock, "IconBlock4");
	iconItem = new c3_Static(&retval, aui_UniqueId(), block);
	subItem->AddChild(iconItem);

	sprintf(block, "%s.%s", subBlock, "CountBlock5");
	iconItem = new c3_Static(&retval, aui_UniqueId(), block);
	subItem->AddChild(iconItem);

	sprintf(block, "%s.%s", subBlock, "IconBlock5");
	iconItem = new c3_Static(&retval, aui_UniqueId(), block);
	subItem->AddChild(iconItem);

	Update();

	return AUI_ERRCODE_OK;
}

void InfoBigListItem::Update(void)
{
	sint32 i,j;

	c3_Static *subItem;

	MBCHAR strbuf[256];
	sint32 cityPop = 0;

	CityData *cd = m_city.GetData()->GetCityData();
	cd->GetPop(cityPop);

	subItem = (c3_Static *)GetChildByIndex(0);

	MBCHAR name[ 80 + 1 ];
	strncpy( name, m_name, 80 );

	if ( !subItem->GetTextFont() )
		subItem->TextReloadFont();

	subItem->GetTextFont()->TruncateString(
		name,
		subItem->Width() );

	subItem->SetText(name);

	subItem = (c3_Static *)GetChildByIndex(1);
	subItem->SetText(m_civ_name);

	subItem = (c3_Static *)GetChildByIndex(2);
	sprintf(strbuf,"%d",cd->PopCount());
	subItem->SetText(strbuf);

	uint64 wonders = cd->GetBuiltWonders();
	sint32 ageCount = g_theAgeDB->NumRecords();

	Assert(ageCount == 5);
	sint32 wonderCount[5];

	for ( j = 0; j < ageCount ; j++ )
	{

		wonderCount[j] = 0;

		for ( i = 0; i < g_theWonderDB->NumRecords() ; i++ )
		{

			if (wonders & ((uint64)1 << (uint64)i))
			{

				if (0  == j)
					wonderCount[j]++;
			}
		}
	}


	subItem = (c3_Static *)GetChildByIndex(3)->GetChildByIndex(0);
	sprintf(strbuf,"%d",wonderCount[0]);
	subItem->SetText(strbuf);

	subItem = (c3_Static *)GetChildByIndex(3)->GetChildByIndex(2);
	sprintf(strbuf,"%d",wonderCount[1]);
	subItem->SetText(strbuf);

	subItem = (c3_Static *)GetChildByIndex(3)->GetChildByIndex(4);
	sprintf(strbuf,"%d",wonderCount[2]);
	subItem->SetText(strbuf);

	subItem = (c3_Static *)GetChildByIndex(3)->GetChildByIndex(6);
	sprintf(strbuf,"%d",wonderCount[3]);
	subItem->SetText(strbuf);

	subItem = (c3_Static *)GetChildByIndex(3)->GetChildByIndex(8);
	sprintf(strbuf,"%d",wonderCount[4]);
	subItem->SetText(strbuf);

}

sint32 InfoBigListItem::Compare(c3_ListItem *item2, uint32 column)
{
	return 0;
}




InfoWonderListItem::InfoWonderListItem(AUI_ERRCODE *retval, sint32 player, sint32 index, const MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (const MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(player, index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
}

AUI_ERRCODE InfoWonderListItem::InitCommonLdl(sint32 player, sint32 index, const MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;


	m_city.m_id = (0);
	m_index = index;
	m_player = player;

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "WonderName");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "CivName");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "CityName");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void InfoWonderListItem::Update(void)
{

	c3_Static *subItem;

	MBCHAR strbuf[256];
	MBCHAR civName[256];

	subItem = (c3_Static *)GetChildByIndex(0);
	strcpy(strbuf,g_theStringDB->GetNameStr(g_theWonderDB->Get(m_index)->m_name));
	subItem->SetText(strbuf);

	Player *p = g_player[m_player];
	Civilisation *civ = p->GetCivilisation();

	civ->GetSingularCivName(strbuf);
	sprintf(civName,"%s", strbuf);

	subItem = (c3_Static *)GetChildByIndex(1);

	subItem->SetText(civName);




	subItem = (c3_Static *)GetChildByIndex(2);
	infowin_GetWonderCityName(m_index, strbuf);
	subItem->SetText(strbuf);

}

sint32 InfoWonderListItem::Compare(c3_ListItem *item2, uint32 column)
{
	c3_Static		*i1, *i2;

	if (column < 0) return 0;

	switch (column) {
	case 0:
	case 1:
	case 2:

		i1 = (c3_Static *)this->GetChildByIndex(column);
		i2 = (c3_Static *)item2->GetChildByIndex(column);

		return strcmp(i1->GetText(), i2->GetText());
		break;
	}

	return 0;
}




InfoScoreListItem::InfoScoreListItem(AUI_ERRCODE *retval, sint32 player, sint32 index, const MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (const MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(player, index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
}

AUI_ERRCODE InfoScoreListItem::InitCommonLdl(sint32 player, sint32 index, const MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	m_index = index;
	m_player = player;
	m_value = 0;

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "ScoreCategory");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "ScoreValue");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void InfoScoreListItem::Update(void)
{

	c3_Static *subItem;

	if (m_player < 0) return;

	MBCHAR strbuf[256];
	Player *pl = g_player[m_player];
	if(!pl) {
		PointerList<Player>::Walker walk(g_deadPlayer);
		while(walk.IsValid()) {
			if(walk.GetObj()->m_owner == m_player) {
				pl = walk.GetObj();
				break;
			}
			walk.Next();
		}
	}
	if (!pl)
		return;

	Score *score = pl->m_score;

	subItem = (c3_Static *)GetChildByIndex(0);
	strcpy(strbuf,score->GetScoreString((SCORE_CATEGORY)m_index));
	subItem->SetText(strbuf);

	m_value = score->GetPartialScore((SCORE_CATEGORY)m_index);

	subItem = (c3_Static *)GetChildByIndex(1);
	sprintf(strbuf,"%d",m_value);
	subItem->SetText(strbuf);
}

sint32 InfoScoreListItem::Compare(c3_ListItem *item2, uint32 column)
{
	return 0;
}




InfoScoreLabelListItem::InfoScoreLabelListItem(AUI_ERRCODE *retval, const MBCHAR *label, const MBCHAR *text, const MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(label, text, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
}

AUI_ERRCODE InfoScoreLabelListItem::InitCommonLdl(const MBCHAR *label, const MBCHAR *text, const MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	strcpy(m_label,label);
	if (!text) strcpy(m_text,"");
	else strcpy(m_text,text);

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "ScoreCategory");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "ScoreValue");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void InfoScoreLabelListItem::Update(void)
{

	c3_Static *subItem;




	subItem = (c3_Static *)GetChildByIndex(0);
	subItem->SetText(m_label);

	subItem = (c3_Static *)GetChildByIndex(1);
	subItem->SetText(m_text);
}

sint32 InfoScoreLabelListItem::Compare(c3_ListItem *item2, uint32 column)
{
	return 0;
}

InfoPlayerListItem::InfoPlayerListItem(AUI_ERRCODE *retval, const MBCHAR *name, sint32 index, const MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (const MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(name, index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
}

AUI_ERRCODE InfoPlayerListItem::InitCommonLdl(const MBCHAR *name, sint32 index, const MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	m_index = index;
	strcpy(m_name, name);

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "Civilization");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void InfoPlayerListItem::Update(void)
{

	c3_Static *subItem;

	subItem = (c3_Static *)GetChildByIndex(0);

	subItem->SetText(m_name);
	subItem->SetTextColor(g_colorSet->GetColorRef((COLOR)m_index));
}

sint32 InfoPlayerListItem::Compare(c3_ListItem *item2, uint32 column)
{
	c3_Static		*i1, *i2;

	if (column < 0) return 0;

	switch (column) {
	case 0:
	case 1:
	case 2:

		i1 = (c3_Static *)this->GetChildByIndex(column);
		i2 = (c3_Static *)item2->GetChildByIndex(column);

		return strcmp(i1->GetText(), i2->GetText());
		break;
	}
	return 0;
}
