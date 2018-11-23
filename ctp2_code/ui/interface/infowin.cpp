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
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "c3ui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_ranger.h"
#include "screenutils.h"

#include "textbutton.h"
#include "c3_button.h"

#include "c3_static.h"
#include "staticpicture.h"
#include "textbox.h"
#include "c3_listbox.h"
#include "controlsheet.h"
#include "aui_textfield.h"
#include "c3_dropdown.h"
#include "thermometer.h"
#include "aui_stringtable.h"

#include "workmap.h"

#include "pixelutils.h"
#include "radarmap.h"

#include "colorset.h"
#include "c3windows.h"
#include "infowin.h"
#include "infowindow.h"
#include "controlpanelwindow.h"




#include "player.h"
#include "UnitRec.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Unit.h"
#include "UnitData.h"
#include "UnitDynArr.h"
#include "citydata.h"
#include "StrDB.h"
#include "ConstDB.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "Advances.h"
#include "TopTen.h"
#include "AgeRecord.h"
#include "Score.h"
#include "DiffDB.h"
#include "Diffcly.h"
#include "profileDB.h"
#include "pollution.h"
#include "EndGame.h"
#include "WonderTracker.h"

#include "Civilisation.h"
#include "CivilisationData.h"
#include "CivilisationPool.h"


#include "CivPaths.h"
#include "SelItem.h"
#include "BldQue.h"
#include "ObjPool.h"
#include "Cell.h"
#include "c3files.h"
#include "pointerlist.h"
#include "linegraph.h"
#include "TurnCnt.h"
#include "Strengths.h"
#include "UnitPool.h"

#include "keypress.h"

#include "wonderutil.h"

#include "EventTracker.h"

#include "GameSettings.h"

extern sint32		g_ScreenWidth;
extern sint32		g_ScreenHeight;
extern C3UI			*g_c3ui;
extern CivPaths		*g_civPaths;
extern ColorSet		*g_colorSet;
extern World		*g_theWorld;
extern TopTen		*g_theTopTen;
extern DifficultyDB	*g_theDifficultyDB;

extern Player					**g_player;
extern PointerList<Player>      *g_deadPlayer;
extern SelectedItem				*g_selected_item;
extern StringDB					*g_theStringDB;
extern ControlPanelWindow		*g_controlPanel;
extern UnitPool					*g_theUnitPool;

extern sint32					g_modalWindow;
extern WorkMap					*g_workMap;
extern ConstDB					*g_theConstDB;
extern TurnCount				*g_turn;

extern ProfileDB                *g_theProfileDB;
extern Pollution				*g_thePollution;

#include "CivilisationPool.h"
extern CivilisationPool			*g_theCivilisationPool;

#define k_INFORADAR_WIDTH		202
#define k_INFORADAR_HEIGHT		151

ctp2_Window			*g_infoWindow = NULL;

static sint32		s_infoSetting;
static sint32		s_infoDataSetting;

static aui_StringTable	*s_stringTable;

static sint32		s_infoXCount;
static sint32		s_infoYCount;
static double		**s_infoGraphData;

static sint32		s_pollutionXCount;
static sint32		s_pollutionYCount;
static double		**s_pollutionGraphData;

static c3_Static		*s_civNameLabel;
static c3_Static		*s_turnsLabel;
static c3_Static		*s_foundedLabel;
static c3_Static		*s_pollutionLabel;

static c3_Static		*s_civNameBox;
static c3_Static		*s_turnsBox;
static c3_Static		*s_foundedBox;
static c3_Static		*s_pollutionBox;

static c3_Static		*s_titleBox;

static c3_ListBox		*s_infoBigList;
static c3_ListBox		*s_infoWonderList;
static c3_ListBox		*s_infoPlayerList;
static c3_ListBox		*s_infoScoreList;
static c3_ListBox		*s_pollutionList;

static LineGraph		*s_infoGraph;
static LineGraph		*s_pollutionGraph;

static Thermometer		*s_pollutionTherm;

static c3_Button		*s_bigButton;
static c3_Button		*s_wonderButton;
static c3_Button		*s_strengthButton;
static c3_Button		*s_scoreButton;
static c3_Button		*s_pollutionButton;

#if 0
static c3_Button		*s_eventsInfoButton[17];
#endif
static c3_Button		*s_eventsInfoButtonLeft,*s_eventsInfoButtonRight;
static sint32			s_currentWonderDisplay;

static c3_Button		*s_labButton;
static c3_Button		*s_throneButton;

static RadarMap			*s_infoRadar;

static c3_Static		*s_bottomRightBox;
static c3_Static		*s_bottomRightImage;

static sint32			s_minRound = 0;

static void infowin_UpdateCivData(void);
static sint32 infowin_ChangeDataSetting(sint32 type);
static void infowin_UpdateBigList(void);
static void infowin_UpdateScoreList(void);
static void infowin_UpdateWonderList(void);
static void infowin_UpdatePlayerList(void);
static void infowin_UpdateCivData(void);
static void infowin_UpdatePollutionGraph(LineGraph *infoGraph,
    sint32 &infoXCount,
    sint32 &infoYCount,
    double ***infoGraphData);
static void infowin_UpdatePollutionData(void);
static sint32 infowin_LabReady(void);
static sint32 infowin_ChangeSetting(sint32);
static void infowin_DisplayLab(void);

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
	else if ((c3_Button*)control == s_labButton)
	{
		infowin_DisplayLab();
	}
	else if ((c3_Button*)control == s_throneButton)
	{

	}
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












	InfoCleanupAction *tempAction = new InfoCleanupAction;

	g_c3ui->AddAction(tempAction);

}

void InfoBigListCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) return;
	sint32 curPlayer =  g_selected_item->GetVisiblePlayer();

	InfoBigListItem *item = (InfoBigListItem *) s_infoBigList->GetSelectedItem();
	if (!item)
	{
		infowin_ChangeDataSetting( k_INFOWIN_DATA_OFF );

		return;
	}
	else {

		infowin_UpdateCivData();

		infowin_ChangeDataSetting( k_INFOWIN_DATA_ON );

		Unit *unit = item->GetCity();
		Unit nullUnit;
		nullUnit.m_id = 0;

		MapPoint cityPos;





		if (curPlayer == unit->GetOwner())
		{

			s_infoRadar->SetSelectedCity(*unit);
			unit->GetPos(cityPos);
			s_infoRadar->CenterMap(cityPos);
			s_infoRadar->Update();

			s_infoRadar->Idle();
		}
		else
		{

			s_infoRadar->SetSelectedCity(nullUnit);

			s_infoRadar->Update();

			s_infoRadar->Idle();
		}
	}

	return;
}

sint32 infowin_Initialize( void )
{




	return 0;









































































































}

void infowin_Cleanup( void )
{
}

void infowin_Cleanup_Controls( void )
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	mycleanup( s_titleBox );
	mycleanup( s_bottomRightBox );
	mycleanup( s_bottomRightImage );

	mycleanup( s_civNameBox );
	mycleanup( s_turnsBox );
	mycleanup( s_foundedBox );
	mycleanup( s_pollutionBox );
	mycleanup( s_civNameLabel );
	mycleanup( s_turnsLabel );
	mycleanup( s_foundedLabel );
	mycleanup( s_pollutionLabel );
	mycleanup( s_pollutionTherm );

	mycleanup( s_infoPlayerList );
	mycleanup( s_infoBigList );
	mycleanup( s_infoScoreList );
	mycleanup( s_infoWonderList );
	mycleanup( s_pollutionList );

	mycleanup( s_infoGraph );
	mycleanup( s_pollutionGraph );

	if (s_infoGraphData)
	{
		for( sint32 i = 0 ; i < s_infoYCount ; i++ )
		{
			delete s_infoGraphData[i];
			s_infoGraphData[i] = NULL;
		}
		delete s_infoGraphData;
		s_infoGraphData = NULL;
	}

	if (s_pollutionGraphData)
	{
		for( sint32 i = 0 ; i < s_pollutionYCount ; i++ )
		{
			delete s_pollutionGraphData[i];
			s_pollutionGraphData[i] = NULL;
		}
		delete s_pollutionGraphData;
		s_pollutionGraphData = NULL;
	}

	mycleanup( s_bigButton );
	mycleanup( s_wonderButton );
	mycleanup( s_strengthButton );
	mycleanup( s_scoreButton );
	mycleanup( s_labButton );
	mycleanup( s_throneButton );
	mycleanup( s_pollutionButton );

#undef mycleanup
}

#if 0
static sint32
infowin_Init_Controls(const MBCHAR *windowBlock)
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
#endif

void infowin_SetMinRoundForGraphs(sint32 minRound)
{
	s_minRound = minRound;
}

void
infowin_LoadData(void)
{

	infowin_UpdateBigList();

	infowin_UpdateScoreList();

	infowin_UpdateWonderList();

	infowin_UpdateGraph(s_infoGraph, s_infoXCount, s_infoYCount, &s_infoGraphData);

	infowin_UpdatePlayerList();

	infowin_UpdateCivData();

	infowin_UpdatePollutionGraph(s_pollutionGraph, s_pollutionXCount, s_pollutionYCount, &s_pollutionGraphData);

	infowin_UpdatePollutionData();

	if (!infowin_LabReady())
		s_labButton->Hide();
}

void
infowin_UpdateCivData(void)
{
	MBCHAR strbuf[256];

	sint32 curPlayer =  g_selected_item->GetVisiblePlayer();

	s_foundedBox->SetText("");
	s_turnsBox->SetText("");

	Player *p = g_player[curPlayer];

	if (!p)
		return;

	if (!s_infoBigList)
		return;

	InfoBigListItem *item = (InfoBigListItem *) s_infoBigList->GetSelectedItem();
	if (!item)
		return;

	Unit *unit = item->GetCity();

	sint32 turnFounded = unit->GetData()->GetCityData()->GetTurnFounded();

	const char *yearStr = g_theDifficultyDB->GetYearStringFromTurn(g_theGameSettings->GetDifficulty(), turnFounded);

#if 0
	sint32 yearFounded = g_theDifficultyDB->GetYearFromTurn(g_theProfileDB->GetDifficulty(), turnFounded);

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

void
infowin_UpdateBigList(void)
{
	AUI_ERRCODE	retval;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sint32 i;

	Unit unit;

	if (!g_theTopTen)
		return;

	g_theTopTen->CalculateBiggestCities();

	s_infoBigList->Clear();
	strcpy(ldlBlock,"InfoBigListItem");
	InfoBigListItem *bItem = NULL;

	for ( i = 0 ; i < 5 ; i++ )
	{

		unit = g_theTopTen->GetBiggestCity(i);
		if ( g_theUnitPool->IsValid(unit) )
		{

			bItem = new InfoBigListItem(&retval, &unit, i, ldlBlock);
			s_infoBigList->AddItem((c3_ListItem *)bItem);
		}
	}
}

void
infowin_UpdateScoreList(void)
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
		return;

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
}

void
infowin_UpdateWonderList(void)
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
}




sint32 infowin_UpdateGraph( LineGraph *infoGraph,
							sint32 &infoXCount,
							sint32 &infoYCount,
							double ***infoGraphData)
{
	sint32 i = 0;
	sint32 j = 0;

	if (!infoGraph) return 0;

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
	double minPower = 0;
	double maxPower = 10;
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

	PointerList<Player>::Walker walk(g_deadPlayer);
	while(walk.IsValid()) {
		color[infoYCount] = g_colorSet->ComputePlayerColor(walk.GetObj()->GetOwner());
		infoYCount++;
		walk.Next();
	}

	infoXCount = (sint32)curRound - (sint32)minRound;

	if (!infoXCount)
	{
		delete color;

		infoGraph->RenderGraph();
		return 0;
	}

	Assert(!*infoGraphData);
	if (infoYCount == 0)
		infoYCount = 1;

	*infoGraphData = new double *[infoYCount];

	if (infoXCount <= 0)
		infoXCount = 1;

	for ( i = 0 ; i < infoYCount ; i++ )
		(*infoGraphData)[i] = new double[infoXCount];

	for ( i = 0 ; i < infoYCount ; i++ )
	{
		for ( j = 0 ; j < infoXCount ; j++ )
			(*infoGraphData)[i][j] = 0;
	}

	sint32 playerCount = 0;
	sint32 strValue = 0;
	for ( i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{
		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{
			for ( j = 0 ; j < infoXCount ; j++ )
			{
				strValue = 0;


				sint32 round =  j;

				strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_UNITS, round);
				strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_GOLD, round);
				strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_BUILDINGS, round);
				strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_WONDERS, round);
				strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_PRODUCTION, round);
				(*infoGraphData)[playerCount][j] = (double)strValue;

				while (strValue > maxPower)
					maxPower += 10;
			}

			playerCount++;
		}
	}

	PointerList<Player>::Walker walk2(g_deadPlayer);

	while (walk2.IsValid())
	{
		for ( j = 0 ; j < infoXCount ; j++ )
		{
			strValue = 0;


			sint32 round =  j;

			strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_UNITS, round);
			strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_GOLD, round);
			strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_BUILDINGS, round);
			strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_WONDERS, round);
			strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_PRODUCTION, round);
			(*infoGraphData)[playerCount][j] = (double)strValue;

			while (strValue > maxPower)
				maxPower += 10;
		}

		playerCount++;
		walk2.Next();
	}

	Assert(playerCount == infoYCount);

	infoGraph->SetLineData(infoYCount, infoXCount, (*infoGraphData), color);

	infoGraph->SetGraphBounds(minRound, curRound, minPower, maxPower);

	infoGraph->RenderGraph();

	delete color;





	if (dumpStrings) {
		delete s_stringTable;
		s_stringTable = NULL;
	}

	return 0;
}

void
infowin_UpdatePollutionGraph( LineGraph *infoGraph,
    sint32 &infoXCount,
    sint32 &infoYCount,
    double ***infoGraphData)
{
	sint32 i = 0;
	sint32 j = 0;
	sint32 color[k_MAX_PLAYERS];

	if (!infoGraph)
		return;

	infoYCount = 0;
	infoXCount = 0;

	infoGraph->SetXAxisName(s_stringTable->GetString(6));
	infoGraph->SetYAxisName("Pollution");

	double curRound = g_turn->GetRound();
	double minRound = curRound - 20;

	if (minRound < 0) minRound = 0;

	double minPower = 0;
	double maxPower = 10;
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

	if (curRound > 20) infoXCount = 20;
	else infoXCount = (sint32)curRound;

	if (!infoXCount)
	{
		infoGraph->RenderGraph();
		return;
	}

	Assert(!*infoGraphData);
	*infoGraphData = new double *[infoYCount];

	for ( i = 0 ; i < infoYCount ; i++ )
		(*infoGraphData)[i] = new double[infoXCount];

	for ( i = 0 ; i < infoYCount ; i++ )
	{
		for ( j = 0 ; j < infoXCount ; j++ )
			(*infoGraphData)[i][j] = 0;
	}

	sint32 playerCount = 0;
	sint32 pollutionValue = 0;
	for ( i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{
		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{
			for ( j = 0 ; j < infoXCount ; j++ )
			{
				pollutionValue = 0;

				pollutionValue += g_player[i]->m_pollution_history[(infoXCount - 1) - j];
				(*infoGraphData)[playerCount][j] = (double)pollutionValue;

				while (pollutionValue > maxPower)
					maxPower += 10;
			}

			playerCount++;
		}
	}

	Assert(playerCount == infoYCount);

	infoGraph->SetLineData(infoYCount, infoXCount, (*infoGraphData), color);

	infoGraph->SetGraphBounds(minRound, curRound, minPower, maxPower);

	infoGraph->RenderGraph();
}

void
infowin_UpdatePlayerList(void)
{
	AUI_ERRCODE	retval;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR strbuf[256];

	s_infoPlayerList->Clear();
	strcpy(ldlBlock,"InfoPlayerListItem");
	InfoPlayerListItem *pItem = NULL;

	sint32 color = 0;

	LineGraphData *myData = s_infoGraph->GetData();

	sint32 lineIndex = 0;

	Player *p = NULL;
	Civilisation *civ = NULL;

	for ( sint32 i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{

		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{

			if (!myData)
				color = (sint32)g_colorSet->ComputePlayerColor(i);
			else color = myData[lineIndex++].color;

			p = g_player[i];
			if (!p) continue;

			civ = p->GetCivilisation();
			if (civ != NULL && g_theCivilisationPool->IsValid(*civ)) {
				civ->GetSingularCivName(strbuf);

				pItem = new InfoPlayerListItem(&retval, strbuf, color, ldlBlock);
				s_infoPlayerList->AddItem((c3_ListItem *)pItem);
			}
		}
	}

	PointerList<Player>::Walker walk(g_deadPlayer);

	while(walk.IsValid()) {

		if (!myData)
			color = (sint32)g_colorSet->ComputePlayerColor(walk.GetObj()->GetOwner());
		else color = myData[lineIndex++].color;

		p = walk.GetObj();
		if (p) {
			civ = p->GetCivilisation();
			if (civ != NULL && g_theCivilisationPool->IsValid(*civ)) {
				civ->GetSingularCivName(strbuf);

				pItem = new InfoPlayerListItem(&retval, strbuf, color, ldlBlock);
				s_infoPlayerList->AddItem((c3_ListItem *)pItem);
			}
		}
		walk.Next();
	}
}

void
infowin_UpdatePollutionData(void)
{
	AUI_ERRCODE	retval;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR strbuf[256];

	s_pollutionList->Clear();
	strcpy(ldlBlock,"InfoPlayerListItem");
	InfoPlayerListItem *pItem = NULL;


	sint32 color = 0;

	LineGraphData *myData = s_pollutionGraph->GetData();

	sint32 lineIndex = 0;

	Player *p = NULL;
	Civilisation *civ = NULL;

	for ( sint32 i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{

		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{

			if (!myData)
				color = (sint32)g_colorSet->ComputePlayerColor(i);
			else color = myData[lineIndex++].color;

			p = g_player[i];
			civ = p->GetCivilisation();
			civ->GetSingularCivName(strbuf);

			pItem = new InfoPlayerListItem(&retval, strbuf, color, ldlBlock);
			s_pollutionList->AddItem((c3_ListItem *)pItem);
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

}

sint32
infowin_ChangeSetting( sint32 type )
{
	sint32 oldType = s_infoSetting;

	if (s_infoSetting == type ) return type;

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

sint32
infowin_ChangeDataSetting( sint32 type )
{
	sint32 oldType = s_infoDataSetting;

	s_infoDataSetting = type;

	Unit nullUnit;
	nullUnit.m_id = 0;

	switch(type)
	{
	case k_INFOWIN_DATA_OFF:




		s_foundedBox->Hide();
		s_turnsBox->Hide();

		s_foundedLabel->Hide();
		s_turnsLabel->Hide();

		s_infoRadar->SetSelectedCity(nullUnit);
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
	sint32 result = 0;

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

	Score *score = pl->m_score;
	sint32 totalValue = score->GetTotalScore();

	Difficulty *diff = pl->m_difficulty;
	sint32 baseValue = diff->GetBaseScore();

	double percent = (double(totalValue) / double(baseValue)) * 100.0;
	if (percent < 0) percent = 0;

	return result = (totalValue+baseValue);
}

void
infowin_DisplayLab()
{

	open_EndGame();
}


sint32 infowin_LabReady()
{
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
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(city, index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
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
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(player, index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
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
	strcpy(strbuf,g_theStringDB->GetNameStr(wonderutil_Get(m_index)->m_name));
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
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(player, index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
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




InfoScoreLabelListItem::InfoScoreLabelListItem(AUI_ERRCODE *retval,
    const MBCHAR *label, const MBCHAR *text, const MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(label, text, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

AUI_ERRCODE InfoScoreLabelListItem::InitCommonLdl(
    const MBCHAR *label, const MBCHAR *text, const MBCHAR *ldlBlock)
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
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(name, index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
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
