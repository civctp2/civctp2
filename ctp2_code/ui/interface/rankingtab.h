//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// - Moved graph functionality to LineGraph (30-Sep-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __RankingTab_H__
#define __RankingTab_H__

class aui_Control;
class ctp2_Window;
class ctp2_Button;
class ctp2_ListItem;
class ctp2_ListBox;
class ctp2_Window;
class ctp2_DropDown;
class ctp2_ListBox;
class LineGraph;

class RankingTab
{
public:
	RankingTab(ctp2_Window *parent);
	~RankingTab();

	static void Open(void);
	static void Close(void);

	void LoadData();

private:

	static RankingTab * s_current_ranking_tab;

	bool            m_line_graph;
	LineGraph     * m_infoGraph;
	double        **m_infoGraphData;
	sint32          m_infoYCount;
	ctp2_Window    *m_info_window;
	ctp2_DropDown  *m_rankingDropDown;
	ctp2_Button    *m_lineOrZeroSumButton;
	ctp2_ListBox   *m_infoPlayerList;

	sint32          m_rankingMilitary;
	sint32          m_rankingEconomic;
	sint32          m_rankingScientific;
	sint32          m_rankingPollution;
	sint32          m_rankingWonders;
	sint32          m_rankingOverall;

	
	void Show();

	
	void Hide();

	
	void Add_Dropdown_Category(char * category);

	
	void SetLineGraph( bool line_graph );

	
	void UpdatePlayerList();

	
	
	void UpdateGraph();

	
	void CleanupGraph();

	
	static void SelectRankingActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	static void LineOrZeroSumButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

};


#endif 
