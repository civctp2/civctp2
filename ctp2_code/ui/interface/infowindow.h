#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif 

#ifndef __INFOWINDOW_H__
#define __INFOWINDOW_H__

class aui_Control;
class ctp2_Window;
class ctp2_Button;
class ScoreTab;
class RankingTab;
class WonderTab;

class InfoWindow
{
public:
	InfoWindow();
	~InfoWindow();

	static void Open(void);
	static void Close(void);
	static void SelectRankingTab(void);
	static void SelectScoreTab(void);
	static void SelectWonderTab(void);

	ctp2_Window *GetWindow() {return m_window;}

private:
	
	void Show();

	
	void Hide();

	
	static void CloseButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	ctp2_Window *m_window;

	
	ctp2_Button *m_closeButton;

	
	RankingTab *m_ranking_tab;

	ScoreTab *m_score_tab;

	WonderTab *m_wonder_tab;
};

#endif 
