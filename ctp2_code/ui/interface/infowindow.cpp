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
// - Added cleanup method. (Sep 13th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "infowindow.h"

#include "aui_ldl.h"
#include "c3ui.h"
#include "ctp2_button.h"
#include "ctp2_Tab.h"
#include "ctp2_TabGroup.h"
#include "ctp2_Window.h"
#include "Globals.h"            // allocated::clear
#include "rankingtab.h"
#include "scoretab.h"
#include "WonderTab.h"

extern C3UI *       g_c3ui;

static InfoWindow * s_InfoWindow = NULL;

InfoWindow::InfoWindow(void)
:
	m_window        (static_cast<ctp2_Window*>
                        (aui_Ldl::BuildHierarchyFromRoot("InfoDialog"))
                    ),
	m_closeButton   (NULL),
	m_ranking_tab   (NULL),
    m_score_tab     (new ScoreTab()),
    m_wonder_tab    (NULL)
{
	Assert(m_window);

    m_ranking_tab   = new RankingTab(m_window);
	m_wonder_tab    = new WonderTab(m_window);

    m_closeButton   = static_cast<ctp2_Button*>
                        (aui_Ldl::GetObject("InfoDialog.CloseButton"));
    Assert(m_closeButton);
    if (m_closeButton)
    {
	    m_closeButton->SetActionFuncAndCookie(&CloseButtonActionCallback, this);
    }
}

InfoWindow::~InfoWindow(void)
{
    delete m_ranking_tab;
    delete m_score_tab;
    delete m_wonder_tab;

    if (m_window)
    {
        aui_Ldl::DeleteHierarchyFromRoot("InfoDialog");
    }
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

/// Update the data, without modifying the current window status
void InfoWindow::Update(void)
{
    if (s_InfoWindow)
    {
        s_InfoWindow->UpdateData();
    }
}

void InfoWindow::Close(void)
{
    if (s_InfoWindow)
    {
        s_InfoWindow->Hide();
    }
}

/// Update the data at all tabs
void InfoWindow::UpdateData(void)
{
    m_score_tab->Update();
    m_ranking_tab->LoadData();
    m_wonder_tab->UpdateList();
}

/// Show the window, after updating the data
void InfoWindow::Show()
{
    UpdateData();
    m_window->Show();
}

void InfoWindow::Hide()
{
    m_window->Hide();
	g_c3ui->RemoveWindow(m_window->Id());
}

void InfoWindow::CloseButtonActionCallback
(
    aui_Control *   control,
	uint32          action,
    uint32          data,
    void *          cookie
)
{
	if (action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	static_cast<InfoWindow*>(cookie)->Hide();
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
	Close();
    allocated::clear(s_InfoWindow);
}
