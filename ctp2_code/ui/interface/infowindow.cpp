//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Information window
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
// - Added Update function so that the info window doesn't need to be closed
//   for update during the turns. - Aug 7th 2005 Martin
// - Added cleanup method. (Sep 13th 2005 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui_ldl.h"
#include "ctp2_Window.h"
#include "ctp2_button.h"
#include "ctp2_Tab.h"
#include "ctp2_TabGroup.h"
#include "c3ui.h"

#include "infowindow.h"
#include "rankingtab.h"
#include "scoretab.h"
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

//----------------------------------------------------------------------------
//
// Name       : InfoWindow::Update
//
// Description: Updates the information window without closing it.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void InfoWindow::Update(void)
{
	if(s_InfoWindow
	&&!s_InfoWindow->m_window->IsHidden()
	){
		s_InfoWindow->Show();
	}
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

//----------------------------------------------------------------------------
//
// Name       : InfoWindow::Cleanup
//
// Description: Deletes the static InfoWindow.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void InfoWindow::Cleanup(void)
{
	if(s_InfoWindow) {
		s_InfoWindow->m_window->Hide();
		g_c3ui->RemoveWindow(s_InfoWindow->m_window->Id());
		delete s_InfoWindow;
		s_InfoWindow = NULL;
	}
}
