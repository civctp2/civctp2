










#include "c3.h"

#include "aui_ldl.h"
#include "ctp2_window.h"
#include "ctp2_button.h"
#include "ctp2_Tab.h"
#include "ctp2_TabGroup.h"
#include "c3ui.h"

#include "infowindow.h"
#include "RankingTab.h"
#include "ScoreTab.h"
#include "WonderTab.h"

extern void cpw_NumberToCommas( uint64 number, MBCHAR *s );

static InfoWindow *s_InfoWindow = NULL;
extern C3UI *g_c3ui;

InfoWindow::InfoWindow(void) :
	m_window(static_cast<ctp2_Window*>(
		aui_Ldl::BuildHierarchyFromRoot("InfoDialog"))),	
	m_closeButton(static_cast<ctp2_Button*>(
		aui_Ldl::GetObject("InfoDialog.CloseButton")))
{
	Assert(m_window);

	m_score_tab = new ScoreTab();
	
	m_ranking_tab = new RankingTab(m_window);

	m_wonder_tab = new WonderTab(m_window);

	
	Assert(m_closeButton);
	
	m_closeButton->SetActionFuncAndCookie(CloseButtonActionCallback, this);
}

void InfoWindow::SelectRankingTab(void)
{
	Open();
	ctp2_TabGroup *tabGroup = (ctp2_TabGroup *)aui_Ldl::GetObject("InfoDialog.TabGroup");
	tabGroup->SelectTab((ctp2_Tab *)aui_Ldl::GetObject("InfoDialog.TabGroup.Tab3"));
}

void InfoWindow::SelectWonderTab(void)
{
	Open();
	ctp2_TabGroup *tabGroup = (ctp2_TabGroup *)aui_Ldl::GetObject("InfoDialog.TabGroup");
	tabGroup->SelectTab((ctp2_Tab *)aui_Ldl::GetObject("InfoDialog.TabGroup.Tab2"));
}

void InfoWindow::SelectScoreTab(void)
{
	Open();
	ctp2_TabGroup *tabGroup = (ctp2_TabGroup *)aui_Ldl::GetObject("InfoDialog.TabGroup");
	tabGroup->SelectTab((ctp2_Tab *)aui_Ldl::GetObject("InfoDialog.TabGroup.Tab1"));
}

void InfoWindow::Open(void)
{
	if (s_InfoWindow==NULL)
		s_InfoWindow = new InfoWindow();

	g_c3ui->AddWindow(s_InfoWindow->m_window);
	s_InfoWindow->Show();
}

void InfoWindow::Close(void)
{
	if (s_InfoWindow != NULL) {
		s_InfoWindow->m_window->Hide();
		g_c3ui->RemoveWindow(s_InfoWindow->m_window->Id());
	}
}


void InfoWindow::Show()
{
	
	m_score_tab->Update();
	
	m_ranking_tab->LoadData();
	m_wonder_tab->UpdateList();

	m_window->Show();
}


void InfoWindow::Hide()
{
	Close(); 
}


void InfoWindow::CloseButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	static_cast<InfoWindow*>(cookie)->Hide();
}

InfoWindow::~InfoWindow(void)
{
	if (m_window) 
	{
		aui_Ldl::DeleteHierarchyFromRoot("InfoDialog");
		m_window = NULL;
	}

	
	if (m_ranking_tab)
		delete m_ranking_tab;

	if (m_score_tab)
		delete m_score_tab;

	if (m_wonder_tab)
		delete m_wonder_tab;
}

