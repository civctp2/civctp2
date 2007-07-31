










#include "c3.h"

#include "aui_ldl.h"
#include "ctp2_window.h"
#include "ctp2_button.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "c3ui.h"
#include "ctp2_DropDown.h"
#include "ctp2_Static.h"
#include "ctp2_ListBox.h"


#include "StrDB.h"


#include "RankingTab.h"


#include "LineGraph.h"


#include "aui_uniqueid.h"


#include "Player.h"


#include "aui_StringTable.h"


#include "ColorSet.h"


#include "infowin.h"
#include "TurnCnt.h"
#include "gstypes.h"
#include "Strengths.h"
#include "CivilisationPool.h"


extern C3UI *g_c3ui;
extern StringDB *g_theStringDB;
extern PointerList<Player>      *g_deadPlayer;
extern TurnCount				*g_turn; 
extern ColorSet		*g_colorSet;

static aui_StringTable	*s_stringTable;


static sint32			s_minRound = 0;



RankingTab * RankingTab::s_current_ranking_tab = NULL;



RankingTab::RankingTab(ctp2_Window *parent) :
	m_rankingDropDown(static_cast<ctp2_DropDown*>(aui_Ldl::GetObject(
		"InfoDialog.TabGroup.Tab3.TabPanel.RankSelect.Pulldown"))),
	m_lineOrZeroSumButton(static_cast<ctp2_Button*>(
		aui_Ldl::GetObject("InfoDialog.TabGroup.Tab3.TabPanel.LineOrZeroSum"))),
	m_infoPlayerList(static_cast<ctp2_ListBox *>(
		aui_Ldl::GetObject("InfoDialog", "TabGroup.Tab3.TabPanel.InfoPlayerList"))),
	m_infoGraph(static_cast<LineGraph *>(
		aui_Ldl::GetObject("InfoDialog", "TabGroup.Tab3.TabPanel.InfoGraph")))
{

	
	m_info_window = parent;

	m_infoGraphData = NULL;

	
	
	

	
	Assert(m_rankingDropDown);

	
	m_rankingDropDown->SetActionFuncAndCookie(SelectRankingActionCallback, m_info_window);

	
	m_rankingDropDown->Clear();


	
	uint32 counter = 0;
	Add_Dropdown_Category("str_ldl_RANKING_MILITARY");
	m_rankingMilitary = counter++;
	Add_Dropdown_Category("str_ldl_RANKING_ECONOMIC");
	m_rankingEconomic = counter++;
	Add_Dropdown_Category("str_ldl_RANKING_SCIENTIFIC");
	m_rankingScientific = counter++;
	Add_Dropdown_Category("str_ldl_RANKING_OVERALL");
	m_rankingOverall = counter++;

	
	m_rankingDropDown->SetSelectedItem(m_rankingOverall);


	
	
	

	
	Assert(m_lineOrZeroSumButton);

	
	SetLineGraph( true );

	
	m_lineOrZeroSumButton->SetActionFuncAndCookie(LineOrZeroSumButtonActionCallback, parent);

	
	m_lineOrZeroSumButton->Show();


	
	
	


	



	Assert( m_infoGraph );

	
	m_infoGraph->Show();

	
	m_infoGraph->EnableYNumber(FALSE);
	m_infoGraph->EnablePrecision(FALSE);


	
	
	
	Assert( m_infoPlayerList );

	
	m_infoPlayerList->GetHeader()->Enable( FALSE );


	
	
	
	
	s_current_ranking_tab = this;


	
	
	
	LoadData();

}


void RankingTab::Add_Dropdown_Category(char * category)
{
	
	ctp2_ListItem *listItem = NULL;

	
	listItem = static_cast<ctp2_ListItem*>(
		aui_Ldl::BuildHierarchyFromRoot("RankingListItem"));

	
	ctp2_Static *label = static_cast<ctp2_Static*>(
		listItem->GetChildByIndex(0));

	
	label->SetText(g_theStringDB->GetNameStr(category));

	
	m_rankingDropDown->AddItem(listItem);

	
	m_rankingDropDown->SetSelectedItem(0);
}



void RankingTab::SetLineGraph( bool line_graph )
{
	
	m_line_graph = line_graph;

	
	if (m_line_graph)
	{
		
		m_infoGraph->SetGraphType(GRAPH_TYPE_LINE);

		
		m_lineOrZeroSumButton->SetText(g_theStringDB->GetNameStr("str_ldl_ZEROSUM_BUTTON"));

	} 
	else
	{
		
		m_infoGraph->SetGraphType(GRAPH_TYPE_ZEROSUM);

		
		m_lineOrZeroSumButton->SetText(g_theStringDB->GetNameStr("str_ldl_LINE_BUTTON"));

	} 
}


void RankingTab::Open(void)
{




}

void RankingTab::Close(void)
{


}


void RankingTab::Show()
{
	
	m_lineOrZeroSumButton->Show();
}


void RankingTab::Hide()
{
	
	m_lineOrZeroSumButton->Hide();
}


void RankingTab::LoadData()
{
	
	UpdateGraph();

	
	UpdatePlayerList();

}


void RankingTab::CleanupGraph()
{
	
	if (m_infoGraphData)
	{
		for( sint32 i = 0 ; i < m_infoYCount ; i++ )
		{
			delete m_infoGraphData[i];
			m_infoGraphData[i] = NULL;
		}
		delete m_infoGraphData;
		m_infoGraphData = NULL;
	}
}


void RankingTab::UpdateGraph()
{
	CleanupGraph();
	sint32 category = m_rankingDropDown->GetSelectedItem();
	if (category == m_rankingOverall)
		category = kRankingOverall;
	else if (category == m_rankingMilitary)
		category = kRankingMilitary;
	else if (category == m_rankingEconomic)
		category = kRankingEconomic;
	else if (category == m_rankingScientific)
		category = kRankingScientific;

	m_infoYCount = SetupRankingGraph(m_infoGraph, &m_infoGraphData, category);
	m_info_window->Draw();
}


sint32 SetupRankingGraph(
	LineGraph * pInfoGraph, 
	double ***pInfoGraphData,
	sint32 category)
{

	sint32 i = 0;
	sint32 j = 0;
	sint32 infoXCount = 0;
	sint32 infoYCount = 0;
	double **infoGraphData;


	
	if (!pInfoGraph) return infoYCount;

	sint32 maxPlayers = k_MAX_PLAYERS + g_deadPlayer->GetCount();
	sint32 *color = new sint32[maxPlayers];

	
	
	
	
	
	
	
	BOOL dumpStrings = FALSE;
	AUI_ERRCODE		errcode;

	if (!s_stringTable) {
		s_stringTable = new aui_StringTable( &errcode, "InfoStrings" );
		Assert( AUI_NEWOK(s_stringTable, errcode) );
		if ( !AUI_NEWOK(s_stringTable, errcode) ) return infoYCount;

		dumpStrings = TRUE;
	}

	
	pInfoGraph->SetXAxisName(s_stringTable->GetString(6));	
	pInfoGraph->SetYAxisName("Power");

	
	double minRound = s_minRound;
	double curRound = g_turn->GetRound();
	double minPower = 0;
	double maxPower = 10;
	pInfoGraph->SetGraphBounds(minRound, curRound, minPower, maxPower);

	
	pInfoGraph->HasIndicator(FALSE);

	

	
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
		
		pInfoGraph->RenderGraph();
		return infoYCount;
	}

	
	if (infoYCount <= 0)
		infoYCount = 1;

	infoGraphData = new double *[infoYCount];
	*pInfoGraphData = infoGraphData;

	if (infoXCount <= 0)
		infoXCount = 1;

	for ( i = 0 ; i < infoYCount ; i++ )
		infoGraphData[i] = new double[infoXCount];

	
	for ( i = 0 ; i < infoYCount ; i++ )
	{
		for ( j = 0 ; j < infoXCount ; j++ )
			infoGraphData[i][j] = 0;
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
#ifdef OLD_GRAPHS
				strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_MILITARY, round);
				strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_GOLD, round);
				strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_BUILDINGS, round);
				strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_WONDERS, round);
				strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_PRODUCTION, round);

#else
				
				if (category == kRankingMilitary)
				{
					strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_MILITARY, round);
				}
				else if (category == kRankingEconomic)
				{
					strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_GOLD, round);
					strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_BUILDINGS, round);
					strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_WONDERS, round);
				}
				else if (category == kRankingScientific)
				{
					strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_KNOWLEDGE, round);
				}
				else 
				{
					strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_MILITARY, round);
					strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_GOLD, round);
					strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_BUILDINGS, round);
					strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_WONDERS, round);
					strValue += g_player[i]->m_strengths->GetTurnStrength(STRENGTH_CAT_PRODUCTION, round);
				}
#endif

				infoGraphData[playerCount][j] = (double)strValue;

				
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
#ifdef OLD_GRAPHS
			strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_MILITARY, round);
			strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_GOLD, round);
			strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_BUILDINGS, round);
			strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_WONDERS, round);
			strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_PRODUCTION, round);

#else
			
			if (category == kRankingMilitary)
			{
				strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_MILITARY, round);
			}
			else if (category == kRankingEconomic)
			{
				strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_GOLD, round);
				strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_BUILDINGS, round);
				strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_WONDERS, round);
			}
			else if (category == kRankingScientific)
			{
				strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_KNOWLEDGE, round);
			}
			else 
			{
				strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_MILITARY, round);
				strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_GOLD, round);
				strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_BUILDINGS, round);
				strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_WONDERS, round);
				strValue += walk2.GetObj()->m_strengths->GetTurnStrength(STRENGTH_CAT_PRODUCTION, round);
			}
#endif
			infoGraphData[playerCount][j] = (double)strValue;

			
			while (strValue > maxPower)
				maxPower += 10;
		}
		
		playerCount++;
		walk2.Next();
	}

	
	Assert(playerCount == infoYCount);

	
	pInfoGraph->SetLineData(infoYCount, infoXCount, infoGraphData, color);
	
	
	pInfoGraph->SetGraphBounds(minRound, curRound, minPower, maxPower);

	
	pInfoGraph->RenderGraph();
	
	delete color;

	
	
	
	
	if (dumpStrings) {
		delete s_stringTable;
		s_stringTable = NULL;
	}
	return infoYCount;
}


void RankingTab::UpdatePlayerList( void )
{

	
	AUI_ERRCODE	retval;
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR strbuf[256];

	
	m_infoPlayerList->Clear();
	strcpy(ldlBlock,"InfoPlayerListItem");
	InfoPlayerListItem *pItem = NULL;

	
	
	sint32 color = 0;

	
	LineGraphData *myData = m_infoGraph->GetData();

	
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
				m_infoPlayerList->AddItem((c3_ListItem *)pItem);
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
				m_infoPlayerList->AddItem((c3_ListItem *)pItem);
			}
		}
		walk.Next();
	}

}



void RankingTab::SelectRankingActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_DROPDOWN_ACTION_SELECT))
		return;

	if (s_current_ranking_tab)
	{
		s_current_ranking_tab->UpdateGraph();
	}


































}



void RankingTab::LineOrZeroSumButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	s_current_ranking_tab->SetLineGraph( s_current_ranking_tab->m_line_graph ? false : true);

	s_current_ranking_tab->UpdateGraph();
}


RankingTab::~RankingTab(void)
{
	







	CleanupGraph();





}
