






#include "c3.h"


#include "ctp2_Tab.h"


#include "aui_Action.h"
#include "aui_Ldl.h"
#include "ctp2_Static.h"
#include "ctp2_TabButton.h"
#include "ctp2_TabGroup.h"


ctp2_Tab::ctp2_Tab(AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock,
				   ControlActionCallback *ActionFunc, void *cookie) :
aui_Control(retval, id, ldlBlock, ActionFunc, cookie),
aui_ImageBase(ldlBlock),
aui_TextBase(ldlBlock, NULL),
m_tabGroup(NULL),
m_tabButton(NULL),
m_tabPanel(NULL)
{
}


ctp2_Tab::~ctp2_Tab()
{
}


AUI_ERRCODE ctp2_Tab::DoneInstantiatingThis(const MBCHAR *ldlBlock)
{
	
	m_tabButton = static_cast<ctp2_TabButton*>(aui_Ldl::GetObject(ldlBlock, "TabButton"));
	Assert(m_tabButton);	

	
	m_tabButton->SetActionFuncAndCookie(TabButtonActionCallback, this);

	
	m_tabPanel = static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock, "TabPanel"));
	Assert(m_tabPanel);	

	
	m_tabPanel->Hide();

	
	return AUI_ERRCODE_OK;
}


void ctp2_Tab::Activate()
{
	
	Assert(m_tabPanel);	
	m_tabPanel->Show();

	
	if(m_ActionFunc)
		m_ActionFunc(this, ACTION_ACTIVATED, 0, m_cookie);
	else if(m_action)
		m_action->Execute(this, ACTION_ACTIVATED, 0);
}


void ctp2_Tab::Deactivate()
{
	
	Assert(m_tabPanel);	
	m_tabPanel->Hide();

	
	if(m_ActionFunc)
		m_ActionFunc(this, ACTION_DEACTIVATED, 0, m_cookie);
	else if(m_action)
		m_action->Execute(this, ACTION_DEACTIVATED, 0);
}

void ctp2_Tab::HidePanel()
{
	Assert(m_tabPanel);
	m_tabPanel->Hide();
}


bool ctp2_Tab::IsPanelActive()
{
	
	Assert(m_tabPanel);	
	return(!m_tabPanel->IsHidden());
}


void ctp2_Tab::TabButtonActionCallback(aui_Control *control, uint32 action,
									   uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	ctp2_Tab *tab = static_cast<ctp2_Tab*>(cookie);

	
	if(tab->m_tabGroup)
		tab->m_tabGroup->SelectTab(tab);
}
