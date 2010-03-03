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
// - Added setting up of single-player start and end age values. (11-Apr-2009 Maq)
// - Ensure agesscreen::s_numAges is set when selecting a scenario directly.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "spnewgamewindow.h"

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
#include "civscenarios.h"
#include "globals.h"
#include "profileDB.h"                  // g_theProfileDB
#include "CivilisationRecord.h"
#include "CivilisationPool.h"
#include "c3textfield.h"
#include "loadsavemapwindow.h"
#include "spnewgametribescreen.h"
#include "spnewgamemapsizescreen.h"
#include "StrDB.h"                      // g_theStringDB
#include "agerecord.h"					// g_theAgeDB
#include "agesscreen.h"

extern LoadSaveMapWindow			*g_loadSaveMapWindow;


extern MBCHAR g_scenarioName[k_SCENARIO_NAME_MAX];







































SPNewGameWindow::SPNewGameWindow(AUI_ERRCODE *retval, uint32 id,
		MBCHAR *ldlBlock, sint32 bpp, AUI_WINDOW_TYPE type, bool bevel)
: 
    C3Window                (retval, id, ldlBlock, bpp, type, bevel),
	m_spStart               (NULL),
    m_spOk                  (NULL),
    m_spReturn              (NULL),
	m_spTribe               (NULL), 
    m_spDifficulty          (NULL),
	m_spMapSize             (NULL), 
    m_spPlayers             (NULL),
	m_spMap                 (NULL),
    m_spRules               (NULL),
	m_spEditor              (NULL),
	m_spScenario            (NULL),
	m_spGeneral             (NULL), 
    m_spWorld               (NULL), 
    m_spCustom              (NULL),
    m_spName                (NULL), 
	m_mapTypeButton         (NULL),
	m_mapTypeLabel          (NULL),
	m_worldShapeButton      (NULL),
	m_worldShapeLabel       (NULL),
    m_useCustomMap          (false),
    m_civilizationLabel     (NULL),
	m_leaderNameLabel       (NULL),
	m_difficultyLabel       (NULL),
	m_worldSizeLabel        (NULL),
	m_rulesLabel            (NULL),
	m_playersLabel          (NULL),
	m_worldTypeLabel        (NULL),
	m_quitButton            (NULL),
	m_spTitle               (NULL),
	m_spBackground          (NULL),
	m_string                (NULL),
	m_scenarioName          (NULL),
	m_scenarioStaticText    (NULL)
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

	// Reset failsafe start/end ages here, so they're correct before agesscreen is initialized.
	g_theProfileDB->SetSPStartingAge(0);
	g_theProfileDB->SetSPEndingAge(g_theAgeDB->NumRecords() - 1);
	g_theProfileDB->Save();

    Update();
}


SPNewGameWindow::~SPNewGameWindow()
{
    delete m_spStart;
    delete m_spOk;
	delete m_spReturn;
	delete m_spTribe;
	delete m_spDifficulty;
	delete m_spMapSize;
	delete m_spPlayers;
	delete m_spMap;
	delete m_spRules;
	delete m_spEditor;
	delete m_spScenario;
	delete m_spGeneral;
    delete m_spWorld;
    delete m_spCustom;
    delete m_spName;
	delete m_mapTypeButton;
	delete m_mapTypeLabel;
	delete m_worldShapeButton;
	delete m_worldShapeLabel;
	delete m_civilizationLabel;
	delete m_leaderNameLabel;
	delete m_difficultyLabel;
	delete m_worldSizeLabel;
	delete m_rulesLabel;
	delete m_playersLabel;
	delete m_worldTypeLabel;
	delete m_quitButton;
	delete m_spTitle;
	delete m_spBackground;
	delete m_string;
	delete m_scenarioName;
	delete m_scenarioStaticText;
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
//Added by Martin Gühmann
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













//Added by Martin Gühmann
//Makes sure that the game doesn't crash if the according map size string is invalid.
	sprintf( s, "%s", m_string->GetString(SP_NEWGAME_STR_SMALL + index) );
	m_spMapSize->SetText( s );

	
	sint32 shape = g_theProfileDB->GetWorldShape();
//Added by Martin Gühmann
//Makes sure that the game doesn't crash if the according world shape string is invalid.
	sprintf( s, "%s", m_string->GetString(SP_NEWGAME_STR_EARTH + shape) );
	m_worldShapeButton->SetText( s );


	
	sint32 numPlayers = g_theProfileDB->GetNPlayers() - 1; 

	// Removed the alteration to the value when it was below 3 - JJB
	sprintf( s, "%d", numPlayers);
	m_spPlayers->SetText( s );
	

	// Make sure start and end ages are still within range.
	// A scenario was loaded.
	if (g_civPaths->GetCurScenarioPath() != NULL) {

		if (strlen(g_scenarioName) > 0) {
			m_scenarioName->SetText(g_scenarioName);
			m_scenarioName->ShouldDraw(TRUE);
		}
		m_spScenario->SetText(g_theStringDB->GetNameStr("str_ldl_SP_STANDARD_GAME"));
		m_spScenario->ShouldDraw(TRUE);
		m_scenarioName->Show();
		m_scenarioStaticText->Show();

		sint32 ages		= g_theAgeDB->NumRecords();

		// Reset failsafe start/end age.
		g_theProfileDB->SetSPStartingAge(0);
		g_theProfileDB->SetSPEndingAge(ages - 1);
		agesscreen_Initialize();
		agesscreen_setStartAge(0);
		agesscreen_setEndAge(ages - 1);

	// No scenario loaded.
	} else {
		
		m_scenarioName->SetText(g_theStringDB->GetNameStr("str_ldl_SP_STANDARD_GAME"));
		m_scenarioName->Hide();
		m_scenarioStaticText->Hide();
		m_scenarioName->ShouldDraw(TRUE);

		m_spScenario->SetText(g_theStringDB->GetNameStr("str_ldl_SP_SCENARIO_PICKER"));
		m_spScenario->ShouldDraw(TRUE);

		sint32 ages		= g_theAgeDB->NumRecords();
		sint32 startAge	= g_theProfileDB->GetSPStartingAge();
		sint32 endAge	= g_theProfileDB->GetSPEndingAge();

		// Check ages are still within range.
		// Do not reset failsafe ages here, or they can never be set.
		if (startAge != 0) {
			if (startAge > endAge
			 || startAge < 0) {
				g_theProfileDB->SetSPStartingAge(0);
				agesscreen_setStartAge(0);
			}
		}

		if (endAge != (ages - 1)) {
			if (endAge < startAge
			 || endAge > (ages - 1)) {
				g_theProfileDB->SetSPEndingAge(ages - 1);
				agesscreen_setEndAge(ages - 1);
			}
		}
	}
	// Make sure changes are saved for a new game.
	g_theProfileDB->Save();
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
		m_spGender		= spNew_c3_DropDown(retval,ldlBlock,"Gender",spnewgamescreen_genderSelect);
		aui_StringTable * gender = spNewStringTable(retval,"SPGenderChoicesStringTable");
		spFillDropDown(retval,m_spGender,gender,"SPDropDownListItem","Gender");
		delete gender;
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
	delete m_spClan;
	delete m_spGender;
	delete m_spName;
	delete m_spPreferences;
	delete m_spCustom;
	
    delete m_PTOP;
	delete m_PHEADER;
	delete m_PBOT;
	delete m_PLEFT;
	delete m_PRIGHT;
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
	m_mapSize		= spNew_c3_DropDown(retval,ldlBlock,"MapSize",spnewgamescreen_mapSizeSelect);
	{
		aui_StringTable * mysizes = spNewStringTable(retval,"SPMapSizeStringTable");
		spFillDropDown(retval,m_mapSize,mysizes,"SPDropDownListItem","MapSize");
		delete mysizes;
	}
	m_worldType		= spNew_c3_DropDown(retval,ldlBlock,"WorldType",spnewgamescreen_worldTypeSelect);
	{
		aui_StringTable * mytypes = spNewStringTable(retval,"SPWorldTypeStringTable");
		spFillDropDown(retval,m_worldType,mytypes,"SPDropDownListItem","WorldType");
		delete mytypes;
	}
	m_worldShape	= spNew_c3_DropDown(retval,ldlBlock,"WorldShape",spnewgamescreen_worldShapeSelect);
	{
		aui_StringTable * myshapes	= spNewStringTable(retval,"SPWorldShapeStringTable");
		spFillDropDown(retval,m_worldShape,myshapes,"SPDropDownListItem","WorldShape");
		delete myshapes;
	}
	m_difficulty	= spNew_c3_DropDown(retval,ldlBlock,"Difficulty",spnewgamescreen_difficultySelect);
	{
		aui_StringTable * mydiffs = spNewStringTable(retval,"SPDifficultyStringTable");
		spFillDropDown(retval,m_difficulty,mydiffs,"SPDropDownListItem","Difficulty");
		delete mydiffs;
		
		m_difficulty->SetSelectedItem(g_theProfileDB->GetDifficulty());
	}
	m_riskLevel		= spNew_c3_DropDown(retval,ldlBlock,"RiskLevel",spnewgamescreen_riskLevelSelect);
	{
		aui_StringTable * myrisks = spNewStringTable(retval,"SPRiskLevelStringTable");
		spFillDropDown(retval,m_riskLevel,myrisks,"SPDropDownListItem","RiskLevel");
		delete myrisks;
		
		m_riskLevel->SetSelectedItem(g_theProfileDB->GetRiskLevel());
	}
	m_opponent		= spNew_c3_DropDown(retval,ldlBlock,"Opponent",spnewgamescreen_opponentSelect);
	{
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
	delete m_mapSize;
	delete m_worldType;
	delete m_worldShape;
	delete m_difficulty;
	delete m_riskLevel;
	delete m_opponent;
	delete m_spCustom;
	
	delete m_WTOP;
	delete m_WHEADER;
	delete m_WBOT;
	delete m_WLEFT;
	delete m_WRIGHT;
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
	delete m_spGenocide;
	delete m_spTrade;
	delete m_spCombat;
	delete m_spPollution;
	
	delete m_RTOP;
	delete m_RHEADER;
	delete m_RBOT;
	delete m_RLEFT;
	delete m_RRIGHT;
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












