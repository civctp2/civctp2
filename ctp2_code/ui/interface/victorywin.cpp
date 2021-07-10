//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The victory window plays the win/lose video
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
// - Removed references to the old civilisation database. (Aug 20th 2005 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Cleaned up static data.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "victorywin.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "c3ui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"

#include "c3_static.h"
#include "ctp2_Static.h"

#include "textbutton.h"
#include "c3_button.h"

#include "ctp2_button.h"
#include "ctp2_Window.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"

#include "c3listbox.h"
#include "c3_listbox.h"
#include "aui_listbox.h"

#include "c3_dropdown.h"

#include "texttab.h"
#include "aui_tabgroup.h"

#include "ctp2_TabGroup.h"
#include "ctp2_Tab.h"

#include "c3window.h"
#include "c3windows.h"
#include "victorywindow.h"
#include "infowin.h"
#include "c3fancywindow.h"
#include "aui_stringtable.h"
#include "c3_popupwindow.h"
#include "screenutils.h"

#include "CivPaths.h"           // g_civPaths
#include "SelItem.h"
#include "linegraph.h"
#include "TurnCnt.h"
#include "Score.h"

#include "StrDB.h"
#include "WonderRecord.h"
#include "highscoredb.h"
#include "GameSettings.h"

#include "c3_listitem.h"
#include "civapp.h"
#include "colorset.h"           // g_colorSet
#include "IconRecord.h"
#include "network.h"
#include "TurnCnt.h"
#include "wonderutil.h"
#include "CivilisationPool.h"   // g_theCivilisationPool

extern sint32                   g_ScreenWidth;
extern sint32                   g_ScreenHeight;
extern C3UI                     *g_c3ui;
extern CivApp                   *g_civApp;
extern PointerList<Player>      *g_deadPlayer;
extern sint32                   g_modalWindow;

VictoryWindow                   *g_victoryWindow = NULL;

static ctp2_Button              *s_okButton;
static ctp2_Static              **s_staticControls;
static aui_StringTable          *s_stringTable;

static HighScoreWindowPopup     *s_highScoreWin;

static LineGraph                *s_graph;
static ctp2_ListBox             *s_graphList;
static ctp2_Button              *s_lineOrZeroSumButton;
static BOOL                     s_lineGraph;
static ctp2_ListBox             *s_scoreList;

static ctp2_ListBox             *s_wonderList;

static ctp2_Static              *s_wonderBlock;

static ctp2_Static              **s_wonderIcons;


void VictoryWindowButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AUI_ERRCODE auiErr;

	if ((ctp2_Button*)control == s_okButton)
	{
		auiErr = g_c3ui->RemoveWindow( g_victoryWindow->m_window->Id() );
		Assert( auiErr == AUI_ERRCODE_OK );
		if ( auiErr != AUI_ERRCODE_OK ) return;

		g_modalWindow--;

		s_highScoreWin->DisplayWindow();
	}
}

void LineOrZeroSumButtonActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	victorywin_SetLineGraph( s_lineGraph ? false : true);

	s_graph->RenderGraph();
	g_victoryWindow->m_window->Draw();
}

void HighScoreWinButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	HighScoreWindowPopup *popup = (HighScoreWindowPopup *)cookie;
	if (!popup) return;

	if ((ctp2_Button*)control == popup->m_continueButton)
	{
		popup->RemoveWindow();

		if(g_turn->IsHotSeat() || g_turn->IsEmail())
		{
			Player* player = g_player[g_selected_item->GetVisiblePlayer()];
			if(!player
			||  player->m_isDead
			){
				if(player->IsRobot())
				{
					g_turn->EndThisTurnBeginNewTurn(FALSE);
				}
				else
				{
					g_selected_item->SetPlayerOnScreen(g_selected_item->GetCurPlayer());
				}
			}
		}
	}
	else if ((ctp2_Button*)control == popup->m_creditsButton)
	{
		open_CreditsScreen();
	}
	else if ((ctp2_Button*)control == popup->m_quitButton)
	{
		popup->RemoveWindow();
		if(g_network.IsActive()
		|| g_network.IsNetworkLaunch()
		){
			g_civApp->PostQuitToLobbyAction();
		}
		else
		{
			g_civApp->PostEndGameAction();
		}
	}
}

sint32 victorywin_SetLineGraph( BOOL lineGraph)
{
	s_lineGraph = lineGraph;

	if (s_lineGraph)
	{
		s_graph->SetGraphType(GRAPH_TYPE_LINE);
		s_lineOrZeroSumButton->SetText(g_theStringDB->GetNameStr("str_ldl_ZEROSUM_BUTTON"));
	}
	else
	{
		s_graph->SetGraphType(GRAPH_TYPE_ZEROSUM);
		s_lineOrZeroSumButton->SetText(g_theStringDB->GetNameStr("str_ldl_LINE_BUTTON"));
	}

	return 0;
}

sint32 victorywin_Initialize( sint32 type )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( g_victoryWindow )
	{

		victorywin_UpdateData(type);

		return 0;
	}

	strcpy(windowBlock, "VictoryWindow");

	g_victoryWindow = new VictoryWindow(&errcode);
	Assert( AUI_NEWOK(g_victoryWindow, errcode) );
	if ( !AUI_NEWOK(g_victoryWindow, errcode) ) return -1;

	g_victoryWindow->m_window->SetStronglyModal(TRUE);

	victorywin_Init_Controls(windowBlock);

	s_highScoreWin = new HighScoreWindowPopup(type);

	s_stringTable = new aui_StringTable( &errcode, "VictoryStrings" );
	Assert( AUI_NEWOK(s_stringTable, errcode) );
	if ( !AUI_NEWOK(s_stringTable, errcode) ) return -2;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return -1;

	return 0;
}

sint32 victorywin_DisplayWindow(sint32 type)
{
	AUI_ERRCODE		errcode;

	errcode = g_c3ui->AddWindow( g_victoryWindow->m_window );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return -1;

	g_modalWindow++;

	victorywin_UpdateData(type);

	return 0;
}

sint32 victorywin_RemoveWindow( void )
{
	if ( g_c3ui->GetWindow(g_victoryWindow->m_window->Id()) ) {
		g_c3ui->RemoveWindow( g_victoryWindow->m_window->Id() );
		g_modalWindow--;
	}

	return 1;
}

void victorywin_Cleanup( void )
{
    // The individual "s_wonderIcons[i]" items will be deleted through
    // DeleteHierarchyFromRoot(s_VictoryWindowBlock) in the destructor
    // of g_victoryWindow.
    delete [] s_wonderIcons;
    s_wonderIcons = NULL;

    if (s_graphList)
    {
        s_graphList->Clear();
    }
    if (s_wonderList)
    {
        s_wonderList->Clear();
    }
    if (s_scoreList)
    {
        s_scoreList->Clear();
    }

#define mycleanup(mypointer) { delete mypointer; mypointer = NULL; }
    {
	    mycleanup(s_highScoreWin);
	    mycleanup(s_stringTable);
        mycleanup(g_victoryWindow);
    }
#undef mycleanup
}

sint32 victorywin_AddWonders(const MBCHAR *windowBlock )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	MBCHAR			controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	int i = 0;

	s_wonderIcons = new ctp2_Static *[k_VICWIN_WONDERICON_MAX];

	sprintf( controlBlock, "%s", "TabGroup.Tab1.TabPanel.WonderList" );
	s_wonderList = (ctp2_ListBox *)aui_Ldl::GetObject(windowBlock, controlBlock);
	s_wonderList->SetAbsorbancy( FALSE );
	sint32 height = s_wonderList->Height();

	tech_WLList<ctp2_Static *> wonderList;
	ctp2_Static *item = NULL;

	for ( i = 0; i < k_VICWIN_WONDERICON_MAX; i++ )
	{
		item = new ctp2_Static(&errcode, aui_UniqueId(), "VictoryWindow_WonderIcon");
		Assert( AUI_NEWOK(item,errcode) );
		if ( !AUI_NEWOK(item,errcode) ) return - 1;

		s_wonderIcons[i] = item;

		if ( (height -= item->Height()) < 0 )
			break;

		wonderList.AddTail( item );
	}
	ListPos pos = wonderList.GetHeadPosition();

	for ( i++; i < k_VICWIN_WONDERICON_MAX; i++ )
	{
		wonderList.GetNext( pos )->AddChild( item );

		item = new ctp2_Static(&errcode, aui_UniqueId(), "VictoryWindow_WonderIcon");
		Assert( AUI_NEWOK(item,errcode) );
		if ( !AUI_NEWOK(item,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

		s_wonderIcons[i] = item;

		if ( !pos )
			pos = wonderList.GetHeadPosition();
	}

	wonderList.GetAt( pos )->AddChild( item );
	pos = wonderList.GetHeadPosition();
	for ( i = wonderList.L(); i; i-- )
		s_wonderList->AddItem( (aui_Item *)wonderList.GetNext( pos ) );

	return 0;
}

sint32 victorywin_Init_Controls(const MBCHAR *windowBlock )
{
	MBCHAR			controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			tabBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sint32 i = 0;

	sint32 staticNum = k_VICWIN_STATIC_MAX;
	s_staticControls = new ctp2_Static *[staticNum];

	s_okButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, "CloseButton");
	s_okButton->SetActionFuncAndCookie(VictoryWindowButtonActionCallback, NULL);

	s_staticControls[k_VICWIN_MAIN_TITLE] = (ctp2_Static *)aui_Ldl::GetObject(windowBlock, "Title");

	sprintf(tabBlock, "%s", "TabGroup.Tab2.TabPanel");

	for ( i = 0 ; i < k_VICWIN_STATIC_MAX - 1; i++ )
	{
		sprintf( controlBlock, "%s.StaticText%d", tabBlock, i );
		s_staticControls[i] = (ctp2_Static *)aui_Ldl::GetObject(windowBlock, controlBlock);
	}

	sprintf(controlBlock, "%s.%s", tabBlock, "ScoreList");
	s_scoreList = (ctp2_ListBox *)aui_Ldl::GetObject(windowBlock, controlBlock);

	sprintf(tabBlock, "%s", "TabGroup.Tab3.TabPanel");
	sprintf(controlBlock, "%s.%s", tabBlock, "Graph");
	s_graph = (LineGraph *)aui_Ldl::GetObject(windowBlock, controlBlock);

	sprintf(controlBlock, "%s.%s", tabBlock, "LineOrZeroSum");
	s_lineOrZeroSumButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, controlBlock);
	s_lineOrZeroSumButton->SetActionFuncAndCookie(LineOrZeroSumButtonActionCallback, NULL);

	victorywin_SetLineGraph( false );
	s_graph->EnableYNumber(FALSE);
	s_graph->EnablePrecision(FALSE);

	sprintf(controlBlock, "%s.%s", tabBlock, "GraphList");
	s_graphList = (ctp2_ListBox *)aui_Ldl::GetObject(windowBlock, controlBlock);

	sprintf(controlBlock, "%s.%s", windowBlock, "TabGroup");
	ctp2_TabGroup *tabGroup = (ctp2_TabGroup *)aui_Ldl::GetObject(controlBlock);
	sprintf(controlBlock, "%s.%s", controlBlock, "Tab2");
	tabGroup->SelectTab((ctp2_Tab *)aui_Ldl::GetObject(controlBlock));

	victorywin_AddWonders(windowBlock);

	return 0;
}

sint32 victorywin_UpdateData( sint32 type )
{
	MBCHAR strbuf[256];

	sint32 curPlayer = g_selected_item->GetVisiblePlayer();
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

	bool disableContinue = (type == k_VICWIN_DEFEAT);
	if(disableContinue
	&&(g_turn->IsHotSeat()
	|| g_turn->IsEmail())
	){
		for(sint32 i = 1; i < k_MAX_PLAYERS; i++)
		{
			if(i != curPlayer
			&& g_player[i]
			&& g_player[i]->IsHuman()
			){
				disableContinue = false;
			}
		}
	}
	else if(disableContinue
	     && g_player[curPlayer]
	     &&!g_player[curPlayer]->m_isDead
	     ){
		disableContinue = false;
	}

	if(type == k_VICWIN_DEFEAT) {
		s_staticControls[k_VICWIN_MAIN_TITLE]->SetText(s_stringTable->GetString(1));
	} else {
		s_staticControls[k_VICWIN_MAIN_TITLE]->SetText(s_stringTable->GetString(0));
	}

	if (disableContinue)
	{
		s_highScoreWin->m_continueButton->Enable(FALSE);
	}
	else
	{
		if((g_player[g_selected_item->GetVisiblePlayer()]
		&& !g_player[g_selected_item->GetVisiblePlayer()]->m_isDead)
		||  g_turn->IsEmail()
		||  g_turn->IsHotSeat()
		){
			s_highScoreWin->m_continueButton->Enable(TRUE);
		}
		else
		{
			s_highScoreWin->m_continueButton->Enable(FALSE);
		}
	}

	sprintf(strbuf,"%d",pl->GetTotalPopulation());
	s_staticControls[k_VICWIN_POP_BOX]->SetText(strbuf);

	sprintf(strbuf,"%d",pl->GetNumCities());
	s_staticControls[k_VICWIN_CITY_BOX]->SetText(strbuf);

	sint32 curScore = infowin_GetCivScore(curPlayer);
	sprintf(strbuf,"%s %d", s_stringTable->GetString(8),curScore);
	s_staticControls[k_VICWIN_SCORE_LABEL]->SetText(strbuf);
	strcpy(strbuf, pl->GetDescriptionString());

	if (s_highScoreWin) {
		if(g_theGameSettings && g_theGameSettings->GetKeeppScore()) {
			s_highScoreWin->m_highScoreDB->AddHighScore(strbuf,curScore);
		}
	}

	victorywin_LoadWonderData();
	victorywin_LoadGraphData();
	victorywin_LoadScoreData();

	return 0;
}

sint32 victorywin_DisplayHighScore( void )
{
	s_highScoreWin->DisplayWindow();

	return 0;
}

HighScoreListItem::HighScoreListItem(AUI_ERRCODE *retval, const MBCHAR *name, sint32 score, const MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	ctp2_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(name, score, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

AUI_ERRCODE HighScoreListItem::InitCommonLdl(const MBCHAR *name, sint32 score, const MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	m_score = score;

	strcpy(m_name,name);

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "Rank");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "Score");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void HighScoreListItem::Update(void)
{

	c3_Static *subItem;
	MBCHAR	strbuf[256];

	subItem = (c3_Static *)GetChildByIndex(0);
	subItem->SetText(m_name);

	subItem = (c3_Static *)GetChildByIndex(1);
	sprintf(strbuf,"%d",m_score);
	subItem->SetText(strbuf);
}

sint32 HighScoreListItem::Compare(ctp2_ListItem *item2, uint32 column)
{
	c3_Static		*i1, *i2;

	if (column < 0) return 0;

	switch (column) {
	case 0:
	case 1:

		i1 = (c3_Static *)this->GetChildByIndex(column);
		i2 = (c3_Static *)item2->GetChildByIndex(column);

		return strcmp(i1->GetText(), i2->GetText());
		break;
	}
	return 0;
}

HighScoreWindowPopup::HighScoreWindowPopup( sint32 type )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	strcpy(windowBlock, "HighScoreWindowPopup");

	{
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);
	}

	m_continueButton = NULL;
	m_quitButton = NULL;
	m_creditsButton = NULL;

	m_list = NULL;
	m_title = NULL;

	m_highScoreDB = new HighScoreDB();

	Initialize( windowBlock );

	if (g_player[g_selected_item->GetVisiblePlayer()] == NULL) {
		m_continueButton->Enable(FALSE);
	} else {
		m_continueButton->Enable(TRUE);
	}
}

sint32 HighScoreWindowPopup::Initialize(const MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( controlBlock, "%s.%s", windowBlock, "ContinueButton" );
	m_continueButton = new ctp2_Button(&errcode, aui_UniqueId(), controlBlock, HighScoreWinButtonActionCallback, this);

	Assert( AUI_NEWOK(m_continueButton, errcode) );
	if ( !AUI_NEWOK(m_continueButton, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "QuitButton" );
	m_quitButton = new ctp2_Button(&errcode, aui_UniqueId(), controlBlock, HighScoreWinButtonActionCallback, this);

	Assert( AUI_NEWOK(m_quitButton, errcode) );
	if ( !AUI_NEWOK(m_quitButton, errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", windowBlock, "HighScoreList" );
	m_list = new ctp2_ListBox(&errcode, aui_UniqueId(), controlBlock, NULL, NULL);
	Assert( AUI_NEWOK(m_list, errcode) );
	if ( !AUI_NEWOK(m_list, errcode) ) return -1;
	m_list->DoneInstantiating();

	sprintf(controlBlock, "%s.%s", windowBlock, "Title");
	m_window->AddTitle(controlBlock);

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}

HighScoreWindowPopup::~HighScoreWindowPopup( void )
{
	Cleanup();
}

void HighScoreWindowPopup::Cleanup( void )
{
    if (m_window && g_c3ui)
    {
	    g_c3ui->RemoveWindow(m_window->Id());
    }

#define mycleanup(mypointer) { delete mypointer; mypointer = NULL; };
    {
	    mycleanup( m_continueButton );
	    mycleanup( m_creditsButton );
	    mycleanup( m_quitButton );
	    mycleanup( m_list );
	    mycleanup( m_title );
	    mycleanup( m_highScoreDB );
	    mycleanup( m_window );
    }
#undef mycleanup
}

void HighScoreWindowPopup::DisplayWindow( void )
{
	AUI_ERRCODE auiErr;

	UpdateData();

	g_modalWindow++;
	auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );
}

void HighScoreWindowPopup::RemoveWindow( void )
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	g_modalWindow--;
	g_c3ui->AddAction(new CloseVictoryWindowAction);
}

sint32 HighScoreWindowPopup::UpdateData( void )
{
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	AUI_ERRCODE		retval;

	m_list->Clear();
	strcpy(ldlBlock,"HighScoreListItem");
	HighScoreListItem *item = NULL;
	HighScoreInfo *info = NULL;

	for ( sint32 i = 0; i < m_highScoreDB->m_nHighScores ; i++ )
	{
		info = m_highScoreDB->GetHighScoreInfo(i);

		item = new HighScoreListItem(&retval, info->m_name, info->m_score, ldlBlock);
		m_list->AddItem((ctp2_ListItem *)item);
	}

	return 0;
}

sint32 victorywin_GetWonderFilename( sint32 index, MBCHAR *name )
{
	MBCHAR filename[80];
	MBCHAR strbuf[_MAX_PATH];

	if ( index < 0 ) return FALSE;

	strcpy(filename,g_theWonderDB->Get(index)->GetDefaultIcon()->GetIcon());

	if (g_civPaths->FindFile(C3DIR_PICTURES, filename, strbuf))
	{
		strcpy(name, filename);
		return TRUE;
	}

	return FALSE;
}

sint32 victorywin_LoadGraphData( void )
{
	AUI_ERRCODE	retval;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR strbuf[256];

	double		**graphData;
	sint32		xCount, yCount;

	if (!s_graph) return -1;

	graphData = NULL;
	infowin_UpdateGraph(s_graph, xCount, yCount, &graphData);

	s_graphList->Clear();
	strcpy(ldlBlock,"VictoryPlayerListItem");
	InfoPlayerListItem *pItem = NULL;

	sint32 color = 0;

	LineGraphData *myData = s_graph->GetData();

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
			if(g_theCivilisationPool->IsValid(civ->m_id)) {
				civ->GetSingularCivName(strbuf);
			} else {
				strbuf[0] = 0;
			}

			pItem = new InfoPlayerListItem(&retval, strbuf, color, ldlBlock);
			s_graphList->AddItem((ctp2_ListItem *)pItem);
		}
	}

	PointerList<Player>::Walker walk(g_deadPlayer);

	while(walk.IsValid()) {

		if (!myData)
			color = (sint32)g_colorSet->ComputePlayerColor(walk.GetObj()->GetOwner());
		else color = myData[lineIndex++].color;

		p = walk.GetObj();
		civ = p->GetCivilisation();
		if(g_theCivilisationPool->IsValid(civ->m_id)) {
			civ->GetSingularCivName(strbuf);
		} else {
			strbuf[0] = 0;
		}

		pItem = new InfoPlayerListItem(&retval, strbuf, color, ldlBlock);
		s_graphList->AddItem((ctp2_ListItem *)pItem);
		walk.Next();
	}

	return 0;
}

void CloseVictoryWindowAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	victorywin_Cleanup();
}

sint32 victorywin_LoadScoreData( void )
{
	AUI_ERRCODE	retval;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR strbuf[256];

	sint32 curPlayer =  g_selected_item->GetVisiblePlayer();

	s_scoreList->Clear();
	strcpy(ldlBlock,"VictoryScoreListItem");
	InfoScoreListItem *item = NULL;
	InfoScoreLabelListItem *label = NULL;

	sint32 posValue = 0;
	sint32 negValue = 0;

	label = new InfoScoreLabelListItem(&retval, s_stringTable->GetString(2), NULL, ldlBlock);
	s_scoreList->AddItem((ctp2_ListItem *)label);

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_ADVANCES, ldlBlock);
	s_scoreList->AddItem((ctp2_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_WONDERS, ldlBlock);
	s_scoreList->AddItem((ctp2_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_POPULATION, ldlBlock);
	s_scoreList->AddItem((ctp2_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_FEATS, ldlBlock);
	s_scoreList->AddItem((ctp2_ListItem *)item);
	posValue += item->GetValue();

	item = new InfoScoreListItem(&retval, curPlayer, SCORE_CAT_TYPE_OF_VICTORY, ldlBlock);
	s_scoreList->AddItem((c3_ListItem *)item);
	posValue += item->GetValue();

	sprintf(strbuf,"%d",posValue);
	label = new InfoScoreLabelListItem(&retval, s_stringTable->GetString(3), strbuf, ldlBlock);
	s_scoreList->AddItem((ctp2_ListItem *)label);

	item = new InfoScoreListItem(&retval, -1, 0, ldlBlock);
	s_scoreList->AddItem((ctp2_ListItem *)item);

	label = new InfoScoreLabelListItem(&retval, s_stringTable->GetString(4), NULL, ldlBlock);
	s_scoreList->AddItem((ctp2_ListItem *)label);

	sprintf(strbuf,"%d",negValue);
	label = new InfoScoreLabelListItem(&retval, s_stringTable->GetString(5), strbuf, ldlBlock);
	s_scoreList->AddItem((ctp2_ListItem *)label);

	item = new InfoScoreListItem(&retval, -1, 0, ldlBlock);
	s_scoreList->AddItem((ctp2_ListItem *)item);

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

	Score *score = pl->m_score;
	sint32 totalValue = score->GetTotalScore();
	sprintf(strbuf,"%d",totalValue);
	label = new InfoScoreLabelListItem(&retval, s_stringTable->GetString(6), strbuf, ldlBlock);
	s_scoreList->AddItem((ctp2_ListItem *)label);

	sint32 civScore = infowin_GetCivScore(curPlayer);
	sprintf(strbuf,"%d",civScore);
	label = new InfoScoreLabelListItem(&retval, s_stringTable->GetString(7), strbuf, ldlBlock);
	s_scoreList->AddItem((ctp2_ListItem *)label);

	return 0;
}

sint32 victorywin_LoadWonderData( void )
{
	MBCHAR strbuf[256];

	Assert(g_theWonderDB->NumRecords() <= (k_VICWIN_WONDER_COL_MAX * k_VICWIN_WONDER_ROW_MAX));

	sint32 curPlayer = g_selected_item->GetVisiblePlayer();

	for ( sint32 i = 0; i < g_theWonderDB->NumRecords() ; i++ )
	{
		if (curPlayer == wonderutil_GetOwner(i))
		{
			victorywin_GetWonderFilename(i,strbuf);

			s_wonderIcons[i]->SetImage(strbuf);
		}
	}

	return 0;
}

sint32 victorywin_GetRankName( sint32 player, MBCHAR *name, sint32 gameResult )
{
	MBCHAR strbuf[256];

	strcpy(strbuf,"NULL");

	sint32 curScore = infowin_GetCivScore(player);

	if (gameResult == k_VICWIN_DEFEAT)
	{
		if (curScore <= 5) strcpy(strbuf,s_stringTable->GetString(9));
		else if ((curScore > 5) && (curScore <= 10)) strcpy(strbuf,s_stringTable->GetString(10));
		else if ((curScore > 10) && (curScore <= 15)) strcpy(strbuf,s_stringTable->GetString(11));
		else if ((curScore > 15) && (curScore <= 20)) strcpy(strbuf,s_stringTable->GetString(12));
		else if ((curScore > 20) && (curScore <= 25)) strcpy(strbuf,s_stringTable->GetString(13));
		else if (curScore >= 26) strcpy(strbuf,s_stringTable->GetString(14));
	}
	else
	{
		if (curScore <= 20) strcpy(strbuf,s_stringTable->GetString(15));
		else if ((curScore > 20) && (curScore <= 40)) strcpy(strbuf,s_stringTable->GetString(16));
		else if ((curScore > 40) && (curScore <= 60)) strcpy(strbuf,s_stringTable->GetString(17));
		else if ((curScore > 60) && (curScore <= 80)) strcpy(strbuf,s_stringTable->GetString(18));
		else if ((curScore > 80) && (curScore <= 100)) strcpy(strbuf,s_stringTable->GetString(19));
		else if (curScore > 100) strcpy(strbuf,s_stringTable->GetString(20));
	}

	strcpy(name, strbuf);
	return 0;
}

bool victorywin_IsOnScreen()
{
    return  (g_victoryWindow && g_c3ui->GetWindow(g_victoryWindow->m_window->Id())) ||
            (s_highScoreWin  && g_c3ui->GetWindow(s_highScoreWin->GetWindow()->Id()));
}
