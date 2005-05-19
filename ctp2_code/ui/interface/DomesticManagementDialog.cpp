






#include "c3.h"


#include "DomesticManagementDialog.h"


#include "aui_ldl.h"
#include "c3ui.h"
#include "CauseAndEffectTab.h"
#include "ctp2_button.h"
#include "ctp2_Window.h"
#include "GovernmentTab.h"
#include "network.h"
#include "ctp2_TabGroup.h"


extern C3UI *g_c3ui;


DomesticManagementDialog *g_domesticManagementDialog = NULL;


void DomesticManagementDialog::Open()
{
	if(g_network.IsClient() && g_network.GetSensitiveUIBlocked())
		return;

	
	if(!g_domesticManagementDialog) {
		g_domesticManagementDialog = new DomesticManagementDialog;
	}

	
	g_domesticManagementDialog->Show();

	
	g_domesticManagementDialog->m_causeAndEffectTab->Update();
	g_domesticManagementDialog->m_governmentTab->Update();

	ctp2_TabGroup *tabGroup = (ctp2_TabGroup *)aui_Ldl::GetObject("DomesticDialog.TabGroup");
	tabGroup->SelectTab((ctp2_Tab *)aui_Ldl::GetObject("DomesticDialog.TabGroup.Tab1"));
}


void DomesticManagementDialog::Close()
{
	
	if(g_domesticManagementDialog) {
		g_domesticManagementDialog->Hide();
	}
}


DomesticManagementDialog::DomesticManagementDialog() :
m_window(static_cast<ctp2_Window*>(
		 aui_Ldl::BuildHierarchyFromRoot("DomesticDialog"))),	
m_closeButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
			  "DomesticDialog.CloseButton"))),
m_causeAndEffectTab(new CauseAndEffectTab(
					"DomesticDialog.TabGroup.Tab1.TabPanel")),
m_governmentTab(new GovernmentTab(
				"DomesticDialog.TabGroup.Tab2.TabPanel"))
{
	
	

	
	Assert(m_closeButton);

	
	m_closeButton->SetActionFuncAndCookie(CloseButtonActionCallback, this);
}




void DomesticManagementDialog::Cleanup()
{
	
	if(g_domesticManagementDialog) {
		if(g_c3ui->GetWindow(g_domesticManagementDialog->m_window->Id()) &&
		   !g_domesticManagementDialog->m_window->IsHidden()) {
			g_domesticManagementDialog->Hide();
		}

		if(g_domesticManagementDialog->m_window) {
			aui_Ldl::DeleteHierarchyFromRoot("DomesticDialog");
		}
	}

	
	delete g_domesticManagementDialog;
	g_domesticManagementDialog = NULL;
}


void DomesticManagementDialog::Show()
{
	g_c3ui->AddWindow(m_window);
	m_window->Show();
}


void DomesticManagementDialog::Hide()
{
	m_window->Hide();
	g_c3ui->RemoveWindow(m_window->Id());
}


void DomesticManagementDialog::CloseButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	static_cast<DomesticManagementDialog*>(cookie)->Hide();
}

void DomesticManagementDialog::Update()
{
	m_causeAndEffectTab->Update();
	m_governmentTab->Update();
}
