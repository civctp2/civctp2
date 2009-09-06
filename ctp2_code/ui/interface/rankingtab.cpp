//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The ranking tab of the info window
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
// OLD_GRAPHS
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added pollution power graph (Nov 2nd 2003 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Redesigned constructor, fixed possible crash. (June 5th 2006 Martin Gühmann)
// - Repaired crashes and memory leaks
// - Moved graph functionality to LineGraph (30-Sep-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "rankingtab.h"

#include <algorithm>            // std::fill
#include "aui_ldl.h"
#include "aui_stringtable.h"
#include "aui_uniqueid.h"
#include "c3ui.h"
#include "CivilisationPool.h"
#include "colorset.h"           // g_colorSet
#include "ctp2_button.h"
#include "ctp2_dropdown.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "ctp2_TabGroup.h"
#include "ctp2_Static.h"
#include "ctp2_Window.h"
#include "gstypes.h"
#include "infowin.h"            // InfoPlayerListItem should be moved to somewhere else so that infowin.h can be removed
#include "linegraph.h"
#include <memory>               // std::auto_ptr
#include "player.h"
#include "StrDB.h"              // g_theStringDB
#include "Strengths.h"
#include "TurnCnt.h"            // g_turn


extern C3UI *g_c3ui;
extern PointerList<Player>      *g_deadPlayer;

static sint32			s_minRound = 0;



RankingTab * RankingTab::s_current_ranking_tab = NULL;



RankingTab::RankingTab(ctp2_Window *parent)
:   
    m_line_graph         (true), // Has to be set again
    m_infoGraph          (static_cast<LineGraph *>(aui_Ldl::GetObject(
                          "InfoDialog", "TabGroup.Tab3.TabPanel.InfoGraph"))),
    m_infoGraphData      (NULL),
    m_infoYCount         (0),
    m_info_window        (parent),
    m_rankingDropDown    (static_cast<ctp2_DropDown*>(aui_Ldl::GetObject(
                          "InfoDialog.TabGroup.Tab3.TabPanel.RankSelect.Pulldown"))),
    m_lineOrZeroSumButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
                          "InfoDialog.TabGroup.Tab3.TabPanel.LineOrZeroSum"))),
    m_infoPlayerList     (static_cast<ctp2_ListBox *>(aui_Ldl::GetObject(
                          "InfoDialog", "TabGroup.Tab3.TabPanel.InfoPlayerList")))
//  m_rankingMilitary    (0), // Initialized seperately
//  m_rankingEconomic    (0), // Initialized seperately
//  m_rankingScientific  (0), // Initialized seperately
//  m_rankingPollution   (0), // Initialized seperately
//  m_rankingWonders     (0), // Initialized seperately
//  m_rankingOverall     (0), // Initialized seperately
{
	Assert(m_rankingDropDown);
	m_rankingDropDown->SetActionFuncAndCookie(SelectRankingActionCallback, parent);
	m_rankingDropDown->Clear();

	uint32 counter = 0;
	Add_Dropdown_Category("str_ldl_RANKING_MILITARY");
	m_rankingMilitary = counter++;
	Add_Dropdown_Category("str_ldl_RANKING_ECONOMIC");
	m_rankingEconomic = counter++;
	Add_Dropdown_Category("str_ldl_RANKING_SCIENTIFIC");
	m_rankingScientific = counter++;
	Add_Dropdown_Category("str_ldl_RANKING_POLLUTION");
	m_rankingPollution = counter++;
	Add_Dropdown_Category("str_ldl_RANKING_WONDERS");
	m_rankingWonders = counter++;
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

	LoadData();

	if (!s_current_ranking_tab)
	{
		s_current_ranking_tab = this;
	}
}


void RankingTab::Add_Dropdown_Category(char * category)
{
	
	ctp2_ListItem * listItem = 
	    static_cast<ctp2_ListItem*>(aui_Ldl::BuildHierarchyFromRoot("RankingListItem"));

	
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
		for (sint32 i = 0 ; i < m_infoYCount; ++i)
		{
			delete m_infoGraphData[i];
		}
		delete [] m_infoGraphData;
		m_infoGraphData = NULL;
		m_infoYCount    = 0;
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
	else if (category == m_rankingPollution)
		category = kRankingPollution;
	else if (category == m_rankingWonders)
		category = kRankingWonders;

	sint32 infoXCount = 0;
	m_infoGraph->GenrateGraph(infoXCount, m_infoYCount, &m_infoGraphData, category);

	ctp2_TabGroup *tabGroup = (ctp2_TabGroup *)aui_Ldl::GetObject("InfoDialog.TabGroup");
	if(tabGroup->GetCurrentTab() == (ctp2_Tab *)aui_Ldl::GetObject("InfoDialog.TabGroup.Tab3"))
	{
		m_info_window->Draw();
	}
}

void RankingTab::UpdatePlayerList( void )
{
	m_infoPlayerList->Clear();

	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	strcpy(ldlBlock, "InfoPlayerListItem");
	
	AUI_ERRCODE	retval;
	MBCHAR strbuf[256];

	sint32 color = 0;

	
	LineGraphData * myData = m_infoGraph->GetData();

	
	sint32 lineIndex = 0;
	for (sint32 i = 1 ; i < k_MAX_PLAYERS ; i++)
	{
		Player * p = g_player[i];

		if (p)
		{
			if (myData)
			{
				color = myData[lineIndex++].color;
			}
			else
			{
				color = (sint32)g_colorSet->ComputePlayerColor(i);
			}

			Civilisation * civ = p->GetCivilisation();
			if (civ && g_theCivilisationPool->IsValid(*civ)) 
			{
				civ->GetSingularCivName(strbuf);
				m_infoPlayerList->AddItem
				    (new InfoPlayerListItem(&retval, strbuf, color, ldlBlock));
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
				color = (sint32) g_colorSet->ComputePlayerColor(p->GetOwner());
			}

			Civilisation * civ = p->GetCivilisation();
			if (civ  && g_theCivilisationPool->IsValid(*civ)) 
			{
				civ->GetSingularCivName(strbuf);
				m_infoPlayerList->AddItem
				    (new InfoPlayerListItem(&retval, strbuf, color, ldlBlock));
			}
		}
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
	if (action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	if (s_current_ranking_tab)
	{
		s_current_ranking_tab->SetLineGraph(!s_current_ranking_tab->m_line_graph);
		s_current_ranking_tab->UpdateGraph();
	}
}


RankingTab::~RankingTab(void)
{
	if (this == s_current_ranking_tab)
	{
		s_current_ranking_tab = NULL;
	}

	CleanupGraph();
}
