//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The wonder tab of the info window.
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
// - Replaced old difficulty database by new one. (April 29th 2006 Martin G"uhmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui_ldl.h"
#include "ctp2_Window.h"
#include "ctp2_button.h"
#include "c3ui.h"
#include "ctp2_Static.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"

#include "StrDB.h"

#include "WonderTab.h"

#include "linegraph.h"

#include "aui_uniqueid.h"

#include "aui_stringtable.h"

#include "colorset.h"

#include "rankingtab.h"

#include "EventTracker.h"

#include "WonderRecord.h"
#include "IconRecord.h"
#include "DiffDB.h"
#include "profileDB.h"


#include "c3_button.h"

#include "Unit.h"
#include "citydata.h"
#include "player.h"
#include "Civilisation.h"

#include "WonderTracker.h"

#include "colorset.h"

#include "GameSettings.h"

extern C3UI *g_c3ui;
extern StringDB *g_theStringDB;

extern DifficultyDB *g_theDifficultyDB;
extern PointerList<Player>      *g_deadPlayer;
extern ColorSet *g_colorSet;

WonderTab::WonderTab(ctp2_Window *parent) :
	m_list(static_cast<ctp2_ListBox *>(aui_Ldl::GetObject("InfoDialog", "TabGroup.Tab2.TabPanel.List")))
{

	m_info_window = parent;






	Assert( m_list);




	LoadData();
}

WonderTab::~WonderTab(void)
{
}

void WonderTab::Show()
{
}

void WonderTab::Hide()
{
}

void WonderTab::LoadData()
{

	UpdateList();
}

void WonderTab::AddWonderItem(sint32 wonder, sint32 player, sint32 turn)
{
	Unit curCity;
	if(!g_theWonderTracker->GetCityWithWonder(wonder, curCity))
		curCity.m_id = 0;

	const WonderRecord *rec = g_theWonderDB->Get(wonder);
	ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("WonderTabListItem");
	Assert(item);
	if(item) {
		ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);

		Assert(box);
		if(box) {
			ctp2_Static *st = (ctp2_Static *)box->GetChildByIndex(0);
			Assert(st);
			if(st) {
				st->SetImage((char *)rec->GetDefaultIcon()->GetFirstFrame());
			}

			st = (ctp2_Static *)box->GetChildByIndex(1);
			Assert(st);
			if(st) {
				st->SetText(rec->GetNameText());
			}

			st = (ctp2_Static *)box->GetChildByIndex(2);
			Assert(st);
			if(st) {
				const char *year = g_theDifficultyDB->GetYearStringFromTurn(g_theGameSettings->GetDifficulty(), turn);
				if(year) {
					st->SetText(year);
				} else {
					st->SetText("error");
				}

				item->SetUserData((void*)(intptr_t)turn);
			}

			st = (ctp2_Static *)box->GetChildByIndex(3);
			Assert(st);
			if(st) {
				if(curCity.IsValid()) {
					st->SetText(curCity.CD()->GetName());
					st->SetTextColor(g_colorSet->GetColorRef(g_colorSet->ComputePlayerColor(curCity.GetOwner())));
				} else {
					st->SetText("---");
				}
			}

			st = (ctp2_Static *)box->GetChildByIndex(4);
			Assert(st);
			if(st) {
				Player *p = g_player[player];
				if(!p) {
					PointerList<Player>::Walker walk(g_deadPlayer);
					for(; walk.IsValid(); walk.Next()) {
						if(walk.GetObj()->m_owner == player) {
							p = walk.GetObj();
							break;
						}
					}
				}
				if(!p) {
					st->SetText("---");
				} else {
					MBCHAR buf[k_MAX_NAME_LEN];
					p->m_civilisation->GetCountryName(buf);
					st->SetText(buf);
				}
			}
		}

		item->SetCompareCallback(CompareWonderItems);

		m_list->AddItem(item);
	}
}

sint32 WonderTab::CompareWonderItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column)
{
	ctp2_Static *box1 = (ctp2_Static *)item1->GetChildByIndex(0);
	ctp2_Static *box2 = (ctp2_Static *)item2->GetChildByIndex(0);

	column++;

	ctp2_Static *st1 = (ctp2_Static *)box1->GetChildByIndex(column);
	ctp2_Static *st2 = (ctp2_Static *)box2->GetChildByIndex(column);

	if(column==2)
	{
#if defined(__LP64__)
		return (sint64)item1->GetUserData() - (sint64)item2->GetUserData();
#else
		return (sint32)item1->GetUserData() - (sint32)item2->GetUserData();
#endif
	}
	else
	{
		return stricmp(st1->GetText(), st2->GetText());
	}
}

void WonderTab::UpdateList()
{
	m_list->Clear();

	PointerList<EventData>::Walker walk(g_eventTracker->m_dataList);
	for(; walk.IsValid(); walk.Next()) {
		switch(walk.GetObj()->m_type) {
			case EVENT_TYPE_WONDER:
				AddWonderItem(walk.GetObj()->m_dbIndex,
							  walk.GetObj()->m_playerNum, walk.GetObj()->m_turn);
				break;
		}
	}

	m_list->Show();

}
