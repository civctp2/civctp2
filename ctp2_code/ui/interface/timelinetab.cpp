










#include "c3.h"

#include "aui_ldl.h"
#include "ctp2_Window.h"
#include "ctp2_button.h"
#include "c3ui.h"
#include "ctp2_Static.h"


#include "StrDB.h"


#include "timelinetab.h"


#include "linegraph.h"


#include "aui_uniqueid.h"


#include "aui_stringtable.h"


#include "colorset.h"


#include "rankingtab.h"

#include "EventTracker.h"



#include "c3_button.h"

extern C3UI *g_c3ui;
extern StringDB *g_theStringDB;

static sint32			s_currentWonderDisplay;
static c3_Button		*s_eventsInfoButton[17];

TimelineTab::TimelineTab(ctp2_Window *parent) :
	m_infoGraph(static_cast<LineGraph *>(
		aui_Ldl::GetObject("InfoDialog", "TabGroup.Tab2.TabPanel.InfoGraph"))),
	m_leftButton(static_cast<ctp2_Button*>(
		aui_Ldl::GetObject("InfoDialog.TabGroup.Tab2.TabPanel.LeftButton"))),
	m_rightButton(static_cast<ctp2_Button*>(
		aui_Ldl::GetObject("InfoDialog.TabGroup.Tab2.TabPanel.RightButton")))
{

	
	m_info_window = parent;

	m_infoGraphData = NULL;

	
	m_infoYCount = 0;

	m_infoGraph->SetEventTracker(g_eventTracker);

	
	m_infoGraph->EnableYNumber(FALSE);
	m_infoGraph->EnablePrecision(FALSE);


	
	m_leftButton->SetActionFuncAndCookie(EventsInfoButtonActionCallback, this);
	m_rightButton->SetActionFuncAndCookie(EventsInfoButtonActionCallback, this);

	
	
	
	Assert( m_infoGraph );

	
	m_infoGraph->Show();

	
	
	
	LoadData();

	m_currentWonderDisplay=0;
}

void TimelineTab::EventsInfoButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	TimelineTab *tab = (TimelineTab *)cookie;
	tab->doButtonCallback((ctp2_Button *)control);
}

void TimelineTab::doButtonCallback(ctp2_Button *button)
{
	if (button == m_leftButton)
	{
		m_currentWonderDisplay--;
		if(m_currentWonderDisplay<0)
			m_currentWonderDisplay=0;
		m_infoGraph->RenderGraph(m_currentWonderDisplay);
		m_infoGraph->ShouldDraw(TRUE);
	}
	else if (button == m_rightButton)
	{
		m_currentWonderDisplay++;
		if(m_currentWonderDisplay>=g_eventTracker->GetEventCount())
			m_currentWonderDisplay=g_eventTracker->GetEventCount()-1;
		m_infoGraph->RenderGraph(m_currentWonderDisplay);
		m_infoGraph->ShouldDraw(TRUE);
	}
}



void TimelineTab::Show()
{
}


void TimelineTab::Hide()
{
}


void TimelineTab::LoadData()
{
	
	UpdateGraph();
}


void TimelineTab::CleanupGraph()
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


void TimelineTab::UpdateGraph()
{
	CleanupGraph();
	m_infoYCount = SetupRankingGraph(m_infoGraph, &m_infoGraphData, kRankingOverall);
	m_info_window->Draw();
}

TimelineTab::~TimelineTab(void)
{
	CleanupGraph();
}
