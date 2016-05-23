//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Single Player New game Start Screen
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
// - Fixed crash when the game tries to display invalid text strings,
//   by Martin Gühmann.
// - Tribe index check updated.
// - Allowed for a number of players less than 3 to be displayed
//   - JJB 2005/06/28
// - Replaced old civilisation database by new one. (Aug 21st 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_ldl.h"
#include "aui_surface.h"
#include "aui_uniqueid.h"
#include "aui_imagebase.h"
#include "aui_textbase.h"
#include "aui_textfield.h"
#include "aui_stringtable.h"
#include "c3ui.h"
#include "c3window.h"
#include "ctp2_button.h"
#include "c3_switch.h"
#include "c3_static.h"
#include "c3_checkbox.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "StrDB.h"
#include "profileDB.h"
#include "CivilisationRecord.h"
#include "CivilisationPool.h"
#include "c3textfield.h"

#include "spnewgametribescreen.h"
#include "spnewgamemapsizescreen.h"
#include "loadsavemapwindow.h"
#include "spnewgamewindow.h"

#include "civscenarios.h"

#include "Globals.h"

extern ProfileDB					*g_theProfileDB;
extern StringDB						*g_theStringDB;
extern LoadSaveMapWindow			*g_loadSaveMapWindow;

extern MBCHAR g_scenarioName[k_SCENARIO_NAME_MAX];







































SPNewGameWindow::SPNewGameWindow(AUI_ERRCODE *retval, uint32 id,
		MBCHAR *ldlBlock, sint32 bpp, AUI_WINDOW_TYPE type, bool bevel)
		: C3Window(retval,id,ldlBlock,bpp,type,bevel),
		m_spStart(NULL), m_spReturn(NULL),
		m_spTribe( NULL ), m_spDifficulty( NULL ),
		m_spMapSize( NULL ), m_spPlayers( NULL ),
		m_spMap( NULL ), m_spRules( NULL ),
		m_spGeneral( NULL ), m_spWorld( NULL ), m_spCustom( NULL ),
		m_spName( NULL ), m_useCustomMap( FALSE )

{
	Assert(AUI_SUCCESS(*retval));

	m_spStart = spNew_ctp2_Button(retval,ldlBlock,"StartButton",spnewgamescreen_startPress);













	m_spReturn = spNew_ctp2_Button(retval,ldlBlock,"ReturnButton",spnewgamescreen_returnPress);

	m_scenarioName = spNew_c3_Static(retval, ldlBlock, "ScenarioName");

	m_scenarioStaticText = spNew_c3_Static(retval, ldlBlock, "ScenarioStaticText");


	m_spTribe		= spNew_ctp2_Button( retval, ldlBlock, "TribeButton", spnewgamescreen_tribePress );
	m_spDifficulty	= spNew_ctp2_Button( retval, ldlBlock, "DifficultyButton", spnewgamescreen_difficultyPress );
	m_spMapSize		= spNew_ctp2_Button( retval, ldlBlock, "MapSizeButton", spnewgamescreen_mapSizePress );
	m_spPlayers		= spNew_ctp2_Button( retval, ldlBlock, "PlayersButton", spnewgamescreen_playersPress );
	m_spMap			= spNew_ctp2_Button( retval, ldlBlock, "MapButton", spnewgamescreen_mapPress );
	m_spRules		= spNew_ctp2_Button( retval, ldlBlock, "RulesButton", spnewgamescreen_rulesPress );

	m_spEditor = spNew_ctp2_Button(retval, ldlBlock, "EditorButton", spnewgamescreen_editorPress);

	m_spScenario = spNew_ctp2_Button(retval, ldlBlock, "ScenarioButton", spnewgamescreen_scenarioPress);

	m_spName = spNewTextEntry(retval,ldlBlock,"Name");






	m_worldShapeLabel = spNew_c3_Static( retval, ldlBlock, "WorldShapeLabel" );
	m_worldShapeButton = spNew_ctp2_Button( retval, ldlBlock, "WorldShapeButton", spnewgamescreen_worldShapePress );

	m_civilizationLabel = spNew_c3_Static( retval, ldlBlock, "CivilizationLabel" );
	m_leaderNameLabel = spNew_c3_Static( retval, ldlBlock, "LeaderNameLabel" );
	m_difficultyLabel = spNew_c3_Static( retval, ldlBlock, "DifficultyLabel" );
	m_worldSizeLabel = spNew_c3_Static( retval, ldlBlock, "MapSizeLabel" );
	m_rulesLabel = spNew_c3_Static( retval, ldlBlock, "RulesLabel" );
	m_playersLabel = spNew_c3_Static( retval, ldlBlock, "PlayersLabel" );
	m_worldTypeLabel = spNew_c3_Static( retval, ldlBlock, "MapButtonLabel" );

	m_quitButton = spNew_ctp2_Button( retval, ldlBlock, "QuitButton", spnewgamescreen_quitPress );


	m_spTitle			= spNew_c3_Static(retval,ldlBlock,"Title");
	m_spBackground		= spNew_c3_Static(retval,ldlBlock,"Background");

	m_string			= spNewStringTable(retval,"SPNewGameStrings");

	Update();

}

SPNewGameWindow::~SPNewGameWindow()
{
#define mycleanup(mypointer) { delete mypointer; mypointer = NULL; };
	mycleanup(m_spStart);

	mycleanup(m_spReturn);




	mycleanup( m_spTribe );
	mycleanup( m_spDifficulty );
	mycleanup( m_spMapSize );
	mycleanup( m_spPlayers );
	mycleanup( m_spMap );
	mycleanup( m_spRules );

	mycleanup( m_spEditor );

	mycleanup(m_spName);













	mycleanup(m_worldShapeLabel);
	mycleanup(m_worldShapeButton);

	mycleanup(m_civilizationLabel);
	mycleanup(m_leaderNameLabel);
	mycleanup(m_difficultyLabel);
	mycleanup(m_worldSizeLabel);
	mycleanup(m_rulesLabel);
	mycleanup(m_playersLabel);
	mycleanup(m_worldTypeLabel);

	mycleanup(m_quitButton);

	mycleanup(m_spTitle);
	mycleanup(m_spBackground);

	mycleanup( m_string );

	mycleanup(m_spScenario);
	mycleanup(m_scenarioName);
	mycleanup(m_scenarioStaticText);

#undef mycleanup
}

//----------------------------------------------------------------------------
//
// Name       : Update
//
// Description: Updates the settings information in the SPNewGameWindow.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : All sprintf function calls have now three arguments to
//              prevent a crash if the last argument is invalid.
//
//----------------------------------------------------------------------------
void SPNewGameWindow::Update( void )
{
	MBCHAR s[_MAX_PATH];
	sint32 index;

	m_spTribe->SetText( g_theProfileDB->GetCivName() );

	if ( m_useCustomMap && g_loadSaveMapWindow && g_loadSaveMapWindow->GetSaveMapInfo() )
	{
		MBCHAR mname[ 100 ];
		g_loadSaveMapWindow->GetSaveMapName( mname );

	}
	else
	{

		m_useCustomMap = false;

	}

	index = g_theProfileDB->GetDifficulty();
//Added by MartinGühmann
//Makes sure that the game doesn't crash if the according map size string is invalid.
	sprintf( s, "%s", m_string->GetString(SP_NEWGAME_STR_CHIEFTAIN + index) );
	m_spDifficulty->SetText( s );


	MAPSIZE size;
	size = g_theProfileDB->GetMapSize();

	switch (size) {
	case MAPSIZE_SMALL:
		index = 0;
		break;
	case MAPSIZE_MEDIUM:
		index = 1;
		break;
	case MAPSIZE_LARGE:
		index = 2;
		break;
	case MAPSIZE_GIGANTIC:
		index = 3;
		break;
	}













//Added by MartinGühmann
//Makes sure that the game doesn't crash if the according map size string is invalid.
	sprintf( s, "%s", m_string->GetString(SP_NEWGAME_STR_SMALL + index) );
	m_spMapSize->SetText( s );

	sint32 shape = g_theProfileDB->GetWorldShape();
//Added by MartinGühmann
//Makes sure that the game doesn't crash if the according world shape string is invalid.
	sprintf( s, "%s", m_string->GetString(SP_NEWGAME_STR_EARTH + shape) );
	m_worldShapeButton->SetText( s );


	sint32 numPlayers = g_theProfileDB->GetNPlayers() - 1;

	// Removed the alteration to the value when it was below 3 - JJB
	sprintf( s, "%d", numPlayers);
	m_spPlayers->SetText( s );










	if (g_civPaths->GetCurScenarioPath() != NULL) {

		if (strlen(g_scenarioName) > 0) {
			m_scenarioName->SetText(g_scenarioName);
			m_scenarioName->ShouldDraw(TRUE);
		}
		m_spScenario->SetText(g_theStringDB->GetNameStr("str_ldl_SP_STANDARD_GAME"));
		m_spScenario->ShouldDraw(TRUE);
		m_scenarioName->Show();
		m_scenarioStaticText->Show();
	} else {

		m_scenarioName->SetText(g_theStringDB->GetNameStr("str_ldl_SP_STANDARD_GAME"));
		m_scenarioName->Hide();
		m_scenarioStaticText->Hide();
		m_scenarioName->ShouldDraw(TRUE);

		m_spScenario->SetText(g_theStringDB->GetNameStr("str_ldl_SP_SCENARIO_PICKER"));
		m_spScenario->ShouldDraw(TRUE);
	}
}










SPProfileBox::SPProfileBox ( AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock ) :
	m_spClan(NULL),m_spGender(NULL),m_spName(NULL),
	m_spPreferences(NULL),m_spCustom(NULL),
	m_PTOP(NULL),
	m_PHEADER(NULL),
	m_PBOT(NULL),
	m_PLEFT(NULL),
	m_PRIGHT(NULL)
{
	{
		int i=0,numClans;

		numClans = g_theCivilisationDB->NumRecords();

		m_spClan		= spNew_c3_DropDown(retval,ldlBlock,"Clan",spnewgamescreen_clanSelect);

		while(i<numClans) {
			aui_Item	*item = NULL;
			const MBCHAR *cName = g_theStringDB->GetNameStr(g_theCivilisationDB->Get(i)->GetPluralCivName());
			item = (aui_Item*)new SPDropDownListItem(retval, "SPDropDownListItem", "Clan", cName);
			if (item)
				m_spClan->AddItem(item );
			i++;
		}
	}
	{
		aui_StringTable *gender = NULL;

		m_spGender		= spNew_c3_DropDown(retval,ldlBlock,"Gender",spnewgamescreen_genderSelect);
		gender			= spNewStringTable(retval,"SPGenderChoicesStringTable");
		spFillDropDown(retval,m_spGender,gender,"SPDropDownListItem","Gender");
		if(gender) delete gender;
	}
	m_spName		= spNewTextEntry(retval,ldlBlock,"Name");
	m_spPreferences = spNew_ctp2_Button(retval,ldlBlock,"Preferences",spnewgamescreen_preferencePress);
	m_spCustom		= spNew_ctp2_Button(retval,ldlBlock,"PCustom", spnewgamescreen_pCustomPress);

	m_PTOP			= spNew_c3_Static(retval,ldlBlock, "PTOP");
	m_PHEADER		= spNew_c3_Static(retval,ldlBlock, "PHEADER");
	m_PBOT			= spNew_c3_Static(retval,ldlBlock, "PBOT");
	m_PLEFT			= spNew_c3_Static(retval,ldlBlock, "PLEFT");
	m_PRIGHT		= spNew_c3_Static(retval,ldlBlock, "PRIGHT");
}

SPProfileBox::~SPProfileBox()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };
	mycleanup(m_spClan);
	mycleanup(m_spGender);
	mycleanup(m_spName);
	mycleanup(m_spPreferences);
	mycleanup(m_spCustom);

	mycleanup(m_PTOP);
	mycleanup(m_PHEADER);
	mycleanup(m_PBOT);
	mycleanup(m_PLEFT);
	mycleanup(m_PRIGHT);

#undef mycleanup
}

void SPProfileBox::SetLeader(uint32 index)
{
	sint32 const    tribeIndex = spnewgametribescreen_getTribeIndex();
	if ((tribeIndex < 0) || (tribeIndex >= INDEX_TRIBE_INVALID))
	{
		const MBCHAR *name =
			g_theStringDB->GetNameStr(
				g_theCivilisationDB->Get(index)->GetLeaderNameMale());

		m_spName->SetFieldText(name);
	}
	else
	{
		const sint32 size = 100;
		MBCHAR lname[ size + 1 ];
		spnewgametribescreen_getLeaderName( lname );
		m_spName->SetFieldText( lname );
	}
}










SPWorldBox::SPWorldBox ( AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock ) :
	m_mapSize(NULL), m_worldType(NULL), m_worldShape(NULL),
	m_difficulty(NULL), m_riskLevel(NULL), m_opponent(NULL), m_spCustom(NULL),
	m_WTOP(NULL),
	m_WHEADER(NULL),
	m_WBOT(NULL),
	m_WLEFT(NULL),
	m_WRIGHT(NULL)
{
	{
		aui_StringTable *mysizes = NULL;
		m_mapSize		= spNew_c3_DropDown(retval,ldlBlock,"MapSize",spnewgamescreen_mapSizeSelect);
		mysizes			= spNewStringTable(retval,"SPMapSizeStringTable");
		spFillDropDown(retval,m_mapSize,mysizes,"SPDropDownListItem","MapSize");
		if(mysizes) delete mysizes;

	}
	{
		aui_StringTable *mytypes = NULL;
		m_worldType		= spNew_c3_DropDown(retval,ldlBlock,"WorldType",spnewgamescreen_worldTypeSelect);
		mytypes			= spNewStringTable(retval,"SPWorldTypeStringTable");
		spFillDropDown(retval,m_worldType,mytypes,"SPDropDownListItem","WorldType");
		if(mytypes) delete mytypes;
	}
	{
		aui_StringTable *myshapes = NULL;
		m_worldShape		= spNew_c3_DropDown(retval,ldlBlock,"WorldShape",spnewgamescreen_worldShapeSelect);
		myshapes			= spNewStringTable(retval,"SPWorldShapeStringTable");
		spFillDropDown(retval,m_worldShape,myshapes,"SPDropDownListItem","WorldShape");
		if(myshapes) delete myshapes;
	}
	{
		aui_StringTable *mydiffs = NULL;
		m_difficulty	= spNew_c3_DropDown(retval,ldlBlock,"Difficulty",spnewgamescreen_difficultySelect);
		mydiffs			= spNewStringTable(retval,"SPDifficultyStringTable");
		spFillDropDown(retval,m_difficulty,mydiffs,"SPDropDownListItem","Difficulty");
		if(mydiffs) delete mydiffs;

		m_difficulty->SetSelectedItem(g_theProfileDB->GetDifficulty());
	}
	{
		aui_StringTable *myrisks = NULL;
		m_riskLevel		= spNew_c3_DropDown(retval,ldlBlock,"RiskLevel",spnewgamescreen_riskLevelSelect);
		myrisks			= spNewStringTable(retval,"SPRiskLevelStringTable");
		spFillDropDown(retval,m_riskLevel,myrisks,"SPDropDownListItem","RiskLevel");
		if(myrisks) delete myrisks;

		m_riskLevel->SetSelectedItem(g_theProfileDB->GetRiskLevel());
	}
	{
		m_opponent		= spNew_c3_DropDown(retval,ldlBlock,"Opponent",spnewgamescreen_opponentSelect);
		for(uint32 i=3; i<=16; i++) {
			MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
			sprintf(textBlock, "%d",i);
			c3_ListItem *myitem = new SPDropDownListItem(retval,"SPDropDownListItem","Opponent", textBlock);
			if(myitem) m_opponent->AddItem(myitem);
		}

		uint32 numPlayers = g_theProfileDB->GetNPlayers();
		Assert((numPlayers>2) && (numPlayers<17));
		m_opponent->SetSelectedItem(numPlayers-3);
	}
	m_spCustom		= spNew_ctp2_Button(retval,ldlBlock,"WCustom", spnewgamescreen_wCustomPress);


	m_WTOP			= spNew_c3_Static(retval,ldlBlock, "WTOP");
	m_WHEADER		= spNew_c3_Static(retval,ldlBlock, "WHEADER");
	m_WBOT			= spNew_c3_Static(retval,ldlBlock, "WBOT");
	m_WLEFT			= spNew_c3_Static(retval,ldlBlock, "WLEFT");
	m_WRIGHT		= spNew_c3_Static(retval,ldlBlock, "WRIGHT");

}

SPWorldBox::~SPWorldBox()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };
	mycleanup(m_mapSize);
	mycleanup(m_worldType);
	mycleanup(m_worldShape);
	mycleanup(m_difficulty);
	mycleanup(m_riskLevel);
	mycleanup(m_opponent);
	mycleanup(m_spCustom);

	mycleanup(m_WTOP);
	mycleanup(m_WHEADER);
	mycleanup(m_WBOT);
	mycleanup(m_WLEFT);
	mycleanup(m_WRIGHT);

#undef mycleanup
}









SPRulesBox::SPRulesBox ( AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock ) :
	m_spGenocide(NULL), m_spTrade(NULL), m_spCombat(NULL),
	m_spPollution(NULL),
	m_RTOP(NULL),
	m_RHEADER(NULL),
	m_RBOT(NULL),
	m_RLEFT(NULL),
	m_RRIGHT(NULL)
{

	m_spGenocide	= spNew_c3_CheckBox(retval,ldlBlock,"GenocideButton",0,spnewgamescreen_genocidePress);
	m_spTrade		= spNew_c3_CheckBox(retval,ldlBlock,"TradeButton",0,spnewgamescreen_tradePress);
	m_spCombat		= spNew_c3_CheckBox(retval,ldlBlock,"CombatButton",0,spnewgamescreen_combatPress);
	m_spPollution	= spNew_c3_CheckBox(retval,ldlBlock,"PollutionButton",0,spnewgamescreen_pollutionPress);

	m_spGenocide->SetState(g_theProfileDB->IsGenocideRule());
	m_spTrade->SetState(g_theProfileDB->IsTradeRule());
	m_spCombat->SetState(g_theProfileDB->IsSimpleCombatRule());
	m_spPollution->SetState(g_theProfileDB->IsPollutionRule());

	m_RTOP			= spNew_c3_Static(retval,ldlBlock, "RTOP");
	m_RHEADER		= spNew_c3_Static(retval,ldlBlock, "RHEADER");
	m_RBOT			= spNew_c3_Static(retval,ldlBlock, "RBOT");
	m_RLEFT			= spNew_c3_Static(retval,ldlBlock, "RLEFT");
	m_RRIGHT		= spNew_c3_Static(retval,ldlBlock, "RRIGHT");

}

SPRulesBox::~SPRulesBox()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };
	mycleanup(m_spGenocide);
	mycleanup(m_spTrade);
	mycleanup(m_spCombat);
	mycleanup(m_spPollution);

	mycleanup(m_RTOP);
	mycleanup(m_RHEADER);
	mycleanup(m_RBOT);
	mycleanup(m_RLEFT);
	mycleanup(m_RRIGHT);
#undef mycleanup
}

uint32 SPRulesBox::GetGenocideRules() { return m_spGenocide->IsOn(); }
uint32 SPRulesBox::GetTradeRules() { return m_spGenocide->IsOn(); }
uint32 SPRulesBox::GetCombatRules() { return m_spGenocide->IsOn(); }
uint32 SPRulesBox::GetPollutionRules() { return m_spGenocide->IsOn(); }











SPDropDownListItem::SPDropDownListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock,MBCHAR *type,const MBCHAR *name)
:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock),
	m_myItem(NULL)
{

	m_myItem = spNew_c3_Static(retval, ldlBlock, type);
	if(m_myItem) {
		m_myItem->SetText(name);
		AddChild(m_myItem);
	}
}

SPDropDownListItem::~SPDropDownListItem()
{

}
#if 0
sint32 SPDropDownListItem::Compare(c3_ListItem *item2, uint32 column)
{

}
#endif











TwoChoiceButton::TwoChoiceButton(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		MBCHAR *choiceOff, MBCHAR *choiceOn, uint32 onoff,
		ControlActionCallback *ActionFunc,
		void *cookie) :
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	ctp2_Button(retval,id,ldlBlock,ActionFunc,cookie),
	m_choice(0)
{
		Assert(onoff == 1 || onoff == 0 );
	m_choice = onoff;
	strncpy(m_choices[0],choiceOff,k_AUI_LDL_MAXBLOCK);
	strncpy(m_choices[1],choiceOn,k_AUI_LDL_MAXBLOCK);
	SetText(m_choices[m_choice]);
}

uint32 TwoChoiceButton::Switch()
{
	m_choice = (m_choice ? 0 : 1);
	SetText(m_choices[m_choice]);
	return m_choice;
}
