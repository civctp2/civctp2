
// Looks like this is dead file, should be removed

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TimelineTab_H__
#define __TimelineTab_H__

class aui_Control;
class ctp2_Window;
class ctp2_Button;
class ctp2_ListItem;
class ctp2_ListBox;
class ctp2_Window;
class ctp2_DropDown;
class ctp2_ListBox;
class LineGraph;



class TimelineTab
{
public:
	TimelineTab(ctp2_Window *parent);
	~TimelineTab();

	static void Open(void);
	static void Close(void);

	
	void UpdateGraph();

private:
	ctp2_Button *m_rightButton, *m_leftButton;

	sint32 m_currentWonderDisplay;

	
	LineGraph * m_infoGraph;

	
	double **m_infoGraphData;

	
	sint32 m_infoYCount;

	
	ctp2_Window *m_info_window;

	
	void Show();

	
	void Hide();

	
	void LoadData();

	
	void CleanupGraph();

	void doButtonCallback(ctp2_Button *button);

	static void EventsInfoButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );

};


#endif 
