//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Call to Power 2 user interface tabgroup
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
// - Standardized code (May 21th 2006 Martin G"uhmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "ctp2_TabGroup.h"

#include "ctp2_Tab.h"
#include "ctp2_TabButton.h"

ctp2_TabGroup::ctp2_TabGroup(AUI_ERRCODE *retval, uint32 id, const MBCHAR *ldlBlock,
							 ControlActionCallback *ActionFunc, void *cookie) :
aui_ImageBase(ldlBlock),
aui_TextBase(ldlBlock, (const MBCHAR *)NULL),
aui_Control(retval, id, ldlBlock, ActionFunc, cookie),
m_currentTab(NULL)
{
}

ctp2_TabGroup::~ctp2_TabGroup()
{
}

AUI_ERRCODE ctp2_TabGroup::DoneInstantiatingThis(const MBCHAR *ldlBlock)
{

	ListPos position = ChildList()->GetHeadPosition();
	for(int countdown = ChildList()->L(); countdown; countdown--) {

		ctp2_Tab *tab = static_cast<ctp2_Tab*>(ChildList()->GetNext(position));

		tab->m_tabGroup = this;
	}

	Assert(ChildList()->L());
	SelectTab(static_cast<ctp2_Tab*>(ChildList()->GetHead()));

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE ctp2_TabGroup::Show()
{

	aui_Control::Show();

	bool foundCurrentActiveTab = false;
	ListPos position = ChildList()->GetHeadPosition();
	for(int countdown = ChildList()->L(); countdown; countdown--) {

		ctp2_Tab *tab = static_cast<ctp2_Tab*>(ChildList()->GetNext(position));

		if(m_currentTab == tab) {

			tab->m_tabButton->SetTabMode(ctp2_TabButton::TAB_ACTIVE);
			foundCurrentActiveTab = true;
			continue;
		} else if(!foundCurrentActiveTab) {

			tab->m_tabButton->SetTabMode(ctp2_TabButton::TAB_LEFT);
		} else {

			tab->m_tabButton->SetTabMode(ctp2_TabButton::TAB_RIGHT);
		}

		tab->HidePanel();
	}

	ShouldDraw();

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE ctp2_TabGroup::Draw(aui_Surface *surface, sint32 x, sint32 y)
{

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	if(!IsHidden())
		errcode = DrawThis( surface, x, y );




	if(errcode == AUI_ERRCODE_OK) {

		ListPos position = ChildList()->GetHeadPosition();
		int countdown;
		for(countdown = ChildList()->L(); countdown; countdown--) {

			ctp2_Tab *tab = static_cast<ctp2_Tab*>(ChildList()->GetNext(position));

			if(m_currentTab == tab)
				break;

			tab->ShouldDraw(m_draw | 1);

			tab->Draw(surface, x, y);
		}


		position = ChildList()->GetTailPosition();
		for(countdown = ChildList()->L(); countdown; countdown--) {

			ctp2_Tab *tab = static_cast<ctp2_Tab*>(ChildList()->GetPrev(position));

			tab->ShouldDraw(m_draw | 1);

			tab->Draw(surface, x, y);

			if(m_currentTab == tab)
				break;
		}

		m_currentTab->ShouldDraw(m_draw | 1);
		m_currentTab->Draw(surface, x, y);
	}

	m_draw = 0;

	return(errcode);
}

void ctp2_TabGroup::SelectTab(ctp2_Tab *tab)
{

	if(tab != m_currentTab) {

		if(m_currentTab)
			m_currentTab->Deactivate();

		m_currentTab = tab;
		if(m_currentTab) {

			m_currentTab->Activate();

			m_currentTab->m_tabButton->SetTabMode(ctp2_TabButton::TAB_ACTIVE);


			ListPos position = ChildList()->GetHeadPosition();
			int countdown;
			for(countdown = ChildList()->L(); countdown; countdown--) {

				ctp2_Tab *tab = static_cast<ctp2_Tab*>(ChildList()->GetNext(position));

				if(m_currentTab == tab)
					break;

				tab->m_tabButton->SetTabMode(ctp2_TabButton::TAB_LEFT);
			}


			position = ChildList()->GetTailPosition();
			for(countdown = ChildList()->L(); countdown; countdown--) {

				ctp2_Tab *tab = static_cast<ctp2_Tab*>(ChildList()->GetPrev(position));

				if(m_currentTab == tab)
					break;

				tab->m_tabButton->SetTabMode(ctp2_TabButton::TAB_RIGHT);
			}
		}

		ShouldDraw();
	}
}
