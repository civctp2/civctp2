
//ranking.h

//Pollution power graph added by Martin Gühmann on November the 2nd
 
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

enum eRankingCategories
{
	kRankingMilitary, 
	kRankingEconomic,
	kRankingScientific,
	kRankingPollution, //Added by Martin Gühmann
	kRankingOverall,
};

extern sint32 SetupRankingGraph(LineGraph *pLineGraph, double ***pGraphData, sint32 category);



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

	
	bool m_line_graph;

	
	LineGraph * m_infoGraph;

	
	double **m_infoGraphData;

	sint32 m_infoYCount;

	
	ctp2_Window *m_info_window;

	
	ctp2_DropDown	*m_rankingDropDown;	

	
	ctp2_Button *m_lineOrZeroSumButton;

	
	ctp2_ListBox *m_infoPlayerList;

	//m_rankingPollution added by Martin Gühmann
	sint32 m_rankingMilitary, m_rankingEconomic, m_rankingScientific, m_rankingPollution, m_rankingOverall;

	
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
