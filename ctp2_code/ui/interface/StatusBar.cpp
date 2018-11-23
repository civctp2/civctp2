//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The Status Bar
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - None so far.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "StatusBar.h"

#include "aui_ldl.h"
#include "ctp2_Static.h"

MBCHAR *StatusBar::m_text = NULL;
std::list<StatusBar*> StatusBar::m_list;
sint32 StatusBar::m_allocatedLen = 0;
const aui_Control *StatusBar::m_owner = NULL;

void StatusBar::SetText(const MBCHAR *text, const aui_Control *owner)
{
	if(m_text && (unsigned) m_allocatedLen < strlen(text) + 1) {

		delete m_text;
		m_text = NULL;
	}

	if(text && !m_text) {
		m_text = new char[strlen(text) + 1];
		m_allocatedLen = strlen(text) + 1;
		m_text[0] = 0;
	}

	if(text) {
		if(strcmp(m_text, text) == 0)
			return;

		strcpy(m_text, text);
	}

	m_owner = owner;

	for(std::list<StatusBar*>::iterator i = m_list.begin(); i != m_list.end(); i++)
		(*i)->Update();
}

StatusBar::StatusBar(const MBCHAR *ldlBlock) :
m_statusBar(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock, "StatusBar")))
{

	Assert(m_statusBar);

	m_list.push_back(this);

}

StatusBar::~StatusBar()
{

	m_list.remove(this);

}

void StatusBar::Update()
{

	if(m_text)
		m_statusBar->SetText(m_text);
	else
		m_statusBar->SetText("");

}
