//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Unit manager
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
// - Enable selection of transported units from the tactical info tab.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Handled crash when disbanding armies at the main screen while the unit
//   manager was shown.
// - Changed occurances of UnitRecord::GetMaxHP to
//   UnitData::CalculateTotalHP. (Aug 3rd 2009 Maq)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "unitmanager.h"

#include "Army.h"
#include "ArmyData.h"
#include "aui.h"
#include "aui_blitter.h"
#include "aui_ldl.h"
#include "aui_ranger.h"
#include "c3ui.h"
#include "Cell.h"
#include "colorset.h"
#include "ctp2_button.h"
#include "ctp2_hypertextbox.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "ctp2_spinner.h"
#include "ctp2_Static.h"
#include "ctp2_TabGroup.h"
#include "ctp2_Window.h"
#include "director.h"
#include "Events.h"
#include "GameEventManager.h"
#include "GameEventUser.h"
#include "Globals.h"                // allocated::clear
#include "governor.h"
#include "IconRecord.h"
#include "MessageBoxDialog.h"
#include "net_action.h"
#include "network.h"
#include "player.h"
#include "Readiness.h"
#include "SelItem.h"
#include "SlicContext.h"
#include "StrDB.h"
#include "stringutils.h"
#include "Unit.h"
#include "UnitData.h"
#include "UnitDynArr.h"
#include "UnitRecord.h"
#include "World.h"

extern C3UI *g_c3ui;

static UnitManager *s_unitManager = NULL;
static MBCHAR *s_unitManagerBlock = "UnitManager";
static MBCHAR *s_unitManagerAdviceBlock = "UnitManagerAdviceWindow";
bool UnitManager::sm_statsTabVisible = true;

#define k_UNITMAN_STATS 0
#define k_UNITMAN_TACTICAL 1

#define k_STATS_ICON_COL 0
#define k_STATS_NAME_COL 1
#define k_STATS_COUNT_COL 2
#define k_STATS_TYPE_COL 3
#define k_STATS_SUPPORT_COL 4
#define k_STATS_A_COL 5
#define k_STATS_R_COL 6
#define k_STATS_D_COL 7
#define k_STATS_M_COL 8

#define k_TACTICAL_ICON_COL 0
#define k_TACTICAL_NAME_COL 1
#define k_TACTICAL_ARMY_COL 2
#define k_TACTICAL_ORDER_COL 3
#define k_TACTICAL_LOCATION_COL 4
#define k_TACTICAL_HEALTH_COL 5

#define k_ADVICE_CATEGORY_COL 0
#define k_ADVICE_PERCENT_COL 1
#define k_ADVICE_TOTAL_COL 2

UnitManager::UnitManager(AUI_ERRCODE *err)
{
	*err = AUI_ERRCODE_OK;

	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_unitManagerBlock);
	Assert(m_window);
	if(!m_window) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	m_statsList = (ctp2_ListBox *)aui_Ldl::GetObject(s_unitManagerBlock, "Tabs.Stats.TabPanel.List");
	Assert(m_statsList);
	if(m_statsList) {
		m_statsList->SetMultiSelect(TRUE);
	}

	m_tacticalList = (ctp2_ListBox *)aui_Ldl::GetObject(s_unitManagerBlock, "Tabs.Tactical.TabPanel.List");
	Assert(m_tacticalList);
	if(m_tacticalList) {
		m_tacticalList->SetActionFuncAndCookie(TacticalList, NULL);
		m_tacticalList->SetMultiSelect(TRUE);
	}











	aui_Ldl::SetActionFuncAndCookie(s_unitManagerBlock, "CloseButton", Close, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_unitManagerBlock, "AdviceButton", Advice, NULL);




	aui_Ldl::SetActionFuncAndCookie(s_unitManagerBlock, "Slider", ReadinessActionCallback, NULL);

	aui_Ldl::SetActionFuncAndCookie(s_unitManagerBlock, "DisbandButton", DisbandButton, NULL);

	aui_Ldl::SetActionFuncAndCookie(s_unitManagerBlock, "Tabs.Stats", TabChanged, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_unitManagerBlock, "Tabs.Tactical", TabChanged, NULL);

	m_adviceWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_unitManagerAdviceBlock);
	Assert(m_adviceWindow);
	if(!m_adviceWindow) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	m_adviceList = (ctp2_ListBox *)aui_Ldl::GetObject(s_unitManagerAdviceBlock, "List");
	Assert(m_adviceList);

	m_adviceWindow->Move(m_window->X() - m_adviceWindow->Width(), m_window->Y());

	m_window->AddDockedWindow(m_adviceWindow);
	m_adviceWindow->SetDock(m_window);

	aui_Ldl::SetActionFuncAndCookie(s_unitManagerAdviceBlock, "UpkeepButton", UpkeepButton, NULL);

	*err = AUI_ERRCODE_OK;

	m_lastDisbandedUnit = 0;
}

UnitManager::~UnitManager()
{
	if (m_statsList)
    {
		m_statsList->Clear();
    }

    if (m_tacticalList)
    {
        m_tacticalList->Clear();
    }

	if (m_window)
    {
		aui_Ldl::DeleteHierarchyFromRoot(s_unitManagerBlock);
	}

	if (m_adviceWindow)
    {
		aui_Ldl::DeleteHierarchyFromRoot(s_unitManagerAdviceBlock);
	}

	m_unitCategories.DeleteAll();
}

AUI_ERRCODE UnitManager::Initialize()
{
	if(s_unitManager) {
		return AUI_ERRCODE_OK;
	}

	AUI_ERRCODE err = AUI_ERRCODE_OK;
	s_unitManager = new UnitManager(&err);
	Assert(err == AUI_ERRCODE_OK);

	return err;
}

void UnitManager::Cleanup()
{
	Hide();
	allocated::clear(s_unitManager);
}

AUI_ERRCODE UnitManager::Display()
{
	if(g_network.IsClient() && g_network.GetSensitiveUIBlocked())
		return AUI_ERRCODE_OK;

	if(!s_unitManager) {
		Initialize();
	}

	Assert(s_unitManager);
	if(!s_unitManager) {
		return AUI_ERRCODE_OK;
	}

	AUI_ERRCODE err = AUI_ERRCODE_INVALIDPARAM;
	Assert(s_unitManager->m_window);
	if(s_unitManager->m_window) {
		s_unitManager->Update();
		err = g_c3ui->AddWindow(s_unitManager->m_window);
		Assert(err == AUI_ERRCODE_OK);

		err = s_unitManager->m_window->Show();












		Assert(s_unitManager->m_adviceWindow);
		if(s_unitManager->m_adviceWindow) {
			g_c3ui->AddWindow(s_unitManager->m_adviceWindow);
		}
	}
	return err;
}

AUI_ERRCODE UnitManager::Hide()
{
	if(!s_unitManager) {
		return AUI_ERRCODE_OK;
	}

	Assert(s_unitManager->m_window);
	if(!s_unitManager->m_window) {
		return AUI_ERRCODE_INVALIDPARAM;
	}

	if(s_unitManager->m_adviceWindow) {
		g_c3ui->RemoveWindow(s_unitManager->m_adviceWindow->Id());
	}

	return g_c3ui->RemoveWindow(s_unitManager->m_window->Id());
}

void UnitManager::TabGroupCallback(ctp2_TabGroup *group, ctp2_Tab *tab, void *cookie)
{
	UnitManager::UpdateAdviceText();
}

void UnitManager::Close(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;
	UnitManager::Hide();
}

void UnitManager::Update()
{
	UpdateStatsList();
	UpdateTacticalList();
	UpdateAdvice();
	UpdateReadiness();
	UpdateNumUnits();
	UnitManager::UpdateAdviceText();
}

void UnitManager::UpdateStatsList()
{
	Assert(m_statsList);
	if(!m_statsList) return;

	sint32 i;
	sint32 visP = g_selected_item->GetVisiblePlayer();
	if(!g_player[visP]) return;
	Player *pl = g_player[visP];

	sint32 *unitcount = new sint32[g_theUnitDB->NumRecords()];
	memset(unitcount, 0, g_theUnitDB->NumRecords() * sizeof(sint32));

	if(m_statsList) {
		m_statsList->Clear();

		for(i = 0; i < pl->m_all_units->Num(); i++) {
			unitcount[pl->m_all_units->Access(i).GetType()]++;
		}

		for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
			if(unitcount[i] > 0) {

				const UnitRecord *rec = g_theUnitDB->Get(i);

				ctp2_ListItem * item = (ctp2_ListItem *) aui_Ldl::BuildHierarchyFromRoot("UnitStatsItem");
				Assert(item);
				if (!item) {
					break;
				}
				ctp2_Static   * box  = (ctp2_Static *) item->GetChildByIndex(0);
				Assert(box);
				if (!box) {
					break;
				}

				ctp2_Static * child;
				child = (ctp2_Static *) box->GetChildByIndex(k_STATS_ICON_COL);
				if (child)
				{
					if (rec->GetDefaultIcon())
					{
						if (stricmp(rec->GetDefaultIcon()->GetSmallIcon(), "NULL") == 0)
						{
							child->SetImageBltType(AUI_IMAGEBASE_BLTTYPE_STRETCH);
							child->SetImage((MBCHAR *)rec->GetDefaultIcon()->GetIcon());
						}
						else
						{
							child->SetImageBltType(AUI_IMAGEBASE_BLTTYPE_COPY);
							child->SetImage((MBCHAR *)rec->GetDefaultIcon()->GetSmallIcon());
						}
					}
				}

				child = (ctp2_Static *) box->GetChildByIndex(k_STATS_NAME_COL);
				if (child) {
					child->SetText((MBCHAR *)rec->GetNameText());
				}

				child = (ctp2_Static *) box->GetChildByIndex(k_STATS_COUNT_COL);
				if (child)
				{
					MBCHAR buf[20];
					sprintf(buf, "%d", unitcount[i]);
					child->SetText(buf);
				}

				child = (ctp2_Static *) box->GetChildByIndex(k_STATS_TYPE_COL);
				if (child) {
					child->SetText((MBCHAR *)g_theStringDB->GetNameStr(rec->GetCategory()));
				}

				child = (ctp2_Static *) box->GetChildByIndex(k_STATS_SUPPORT_COL);
				if (child)
				{
					MBCHAR buf[20];
					sprintf(buf, "%d", g_theUnitDB->Get(i)->GetShieldHunger());
					child->SetText(buf);
				}

				child = (ctp2_Static *)box->GetChildByIndex(k_STATS_A_COL);
				if (child)
				{
					MBCHAR buf[20];
					sprintf(buf, "%d", (sint32) rec->GetAttack());
					child->SetText(buf);
				}

				child = (ctp2_Static *)box->GetChildByIndex(k_STATS_R_COL);
				if (child)
				{
					MBCHAR buf[20];
					sprintf(buf, "%d", (sint32) rec->GetZBRangeAttack());
					child->SetText(buf);
				}

				child = (ctp2_Static *) box->GetChildByIndex(k_STATS_D_COL);
				if (child)
				{
					MBCHAR buf[20];
					sprintf(buf, "%d", (sint32) rec->GetDefense());
					child->SetText(buf);
				}

				child = (ctp2_Static *) box->GetChildByIndex(k_STATS_M_COL);
				if (child)
				{
					MBCHAR buf[20];
					sprintf(buf, "%d", (sint32) rec->GetMaxMovePoints());
					child->SetText(buf);
				}

				item->SetUserData((void *)i);
				item->SetCompareCallback(CompareStatItems);

				m_statsList->AddItem(item);
			}
		}
	}
	delete [] unitcount;
}

void UnitManager::UpdateTacticalList()
{
	Player *pl = g_player[g_selected_item->GetVisiblePlayer()];
	Assert(pl);
	if(!pl) return;

	Assert(m_tacticalList);
	if(!m_tacticalList) return;

	m_tacticalList->Clear();

	UnitDynamicArray *units = pl->m_all_units;
	Assert(units);
	if(!units) return;

	m_tacticalList->BuildListStart();
	for (sint32 i = 0; i < units->Num(); i++)
	{
		Unit u = units->Access(i);
		ctp2_ListItem * item = (ctp2_ListItem *) aui_Ldl::BuildHierarchyFromRoot("UnitTacticalItem");
		Assert(item);
		if (!item) {
			break;
		}

		ctp2_Static * box  = (ctp2_Static *) item->GetChildByIndex(0);
		Assert(box);
		if (!box) {
			break;
		}

		ctp2_Static * child;
		child = (ctp2_Static *) box->GetChildByIndex(k_STATS_ICON_COL);
		if (child)
		{
			const UnitRecord *rec = u.GetDBRec();
			if (rec->GetDefaultIcon())
			{
				if (stricmp(rec->GetDefaultIcon()->GetSmallIcon(), "NULL") == 0)
				{
					child->SetImageBltType(AUI_IMAGEBASE_BLTTYPE_STRETCH);
					child->SetImage(rec->GetDefaultIcon()->GetIcon());
				}
				else
				{
					child->SetImageBltType(AUI_IMAGEBASE_BLTTYPE_COPY);
					child->SetImage(rec->GetDefaultIcon()->GetSmallIcon());
				}
			}
		}

		child = (ctp2_Static *) box->GetChildByIndex(k_TACTICAL_NAME_COL);
		if (child) {
			child->SetText(u.GetDBRec()->GetNameText());
		}

		child = (ctp2_Static *) box->GetChildByIndex(k_TACTICAL_ARMY_COL);
		if (child)
		{
			if (u.GetArmy().IsValid()) {
				child->SetText(u.GetArmy()->GetName());
			} else {
				child->SetText("-");
			}
		}

		child = (ctp2_Static *) box->GetChildByIndex(k_TACTICAL_ORDER_COL);
		if (child) {
			StringId str;
			if (u->GetCurrentOrderString(str)) {
				child->SetText(g_theStringDB->GetNameStr(str));
			} else {
				child->SetText(g_theStringDB->GetNameStr("UNIT_ORDER_NONE"));
			}
		}

		child = (ctp2_Static *) box->GetChildByIndex(k_TACTICAL_LOCATION_COL);
		if (child) {
			if (u.Flag(k_UDF_IN_SPACE)) {
				child->SetText(g_theStringDB->GetNameStr("UNIT_LOCATION_IN_SPACE"));
			} else if(g_theWorld->GetCell(u.RetPos())->GetCity().IsValid()) {
				child->SetText(g_theWorld->GetCell(u.RetPos())->GetCity().GetName());
			} else {
				child->SetText(g_theStringDB->GetNameStr("UNIT_LOCATION_IN_FIELD"));
			}
		}

		child = (ctp2_Static *) box->GetChildByIndex(k_TACTICAL_HEALTH_COL);
		if (child) {
			child->SetDrawCallbackAndCookie(DrawHealthBar, (void *)u.m_id);
		}
		item->SetUserData((void *)u.m_id);
		item->SetCompareCallback(CompareTacticalItems);
		m_tacticalList->AddItem(item);
	}

	m_tacticalList->BuildListEnd();
}

static void UpdateUpkeepButton(ctp2_Button *butt, Player *pl, bool useTotalFormat)
{
	Assert(butt);
	Assert(pl);

	MBCHAR buf[k_MAX_NAME_LEN];

	if(useTotalFormat) { // total format
		pl->m_readiness->RecalcCost();
		sprintf(buf, g_theStringDB->GetNameStr("str_ldl_UpkeepTotalFormat"), (sint32)pl->m_readiness->GetCost());
	} else { // percent format
		double totalProd = pl->m_total_production;
		double readinessCost = pl->m_readiness->GetCost();

		sint32 p;
		if(readinessCost == 0)
			p = 0;
		else if(totalProd == 0)
			p = 100;
		else
			p = (sint32(100.0 * (readinessCost / totalProd)));

		sprintf(buf, g_theStringDB->GetNameStr("str_ldl_UpkeepPercentFormat"), p);
	}

	butt->SetText(buf);
}

void UnitManager::UpdateAdvice()
{
	Player *pl = g_player[g_selected_item->GetVisiblePlayer()];
	Assert(pl);
	if(!pl)
		return;

	Assert(m_adviceList);
	if(!m_adviceList) return;
	m_adviceList->Clear();

	PointerList<UnitManagerCategoryInfo>::Walker walk(&m_unitCategories);
	while(walk.IsValid()) {
		walk.GetObj()->numUnits = 0;
		walk.Next();
	}

	sint32 i;
	for(i = 0; i < pl->m_all_units->Num(); i++) {
		sint32 cat = pl->m_all_units->Access(i).GetDBRec()->GetCategory();
		PointerList<UnitManagerCategoryInfo>::Walker walk(&m_unitCategories);
		while(walk.IsValid()) {
			if(walk.GetObj()->stringId == cat) {
				walk.GetObj()->numUnits++;
				break;
			}
			walk.Next();
		}

		if(!walk.IsValid()) {

			UnitManagerCategoryInfo *info = new UnitManagerCategoryInfo;
			info->stringId = cat;
			info->numUnits = 1;
			m_unitCategories.AddTail(info);
		}
	}

	walk.SetList(&m_unitCategories);
	while(walk.IsValid()) {
		ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("UnitManagerAdviceItem");
		Assert(item);
		if(!item) break;

		ctp2_Static *child = (ctp2_Static *)item->GetChildByIndex(k_ADVICE_CATEGORY_COL);
		if(child) {
			child->SetText(g_theStringDB->GetNameStr(walk.GetObj()->stringId));
		}

		MBCHAR buf[20];
		child = (ctp2_Static *)item->GetChildByIndex(k_ADVICE_PERCENT_COL);
		if(child) {
			sprintf(buf, "%d%%", sint32((100.0 * walk.GetObj()->numUnits) / double(pl->m_all_units->Num())));
			child->SetText(buf);
		}

		child = (ctp2_Static *)item->GetChildByIndex(k_ADVICE_TOTAL_COL);
		if(child) {
			sprintf(buf, "%d", walk.GetObj()->numUnits);
			child->SetText(buf);
		}

		item->SetCompareCallback(CompareAdviceItems);
		item->SetUserData(walk.GetObj());
		m_adviceList->AddItem(item);
		walk.Next();
	}

	ctp2_Button *upkeepButt = (ctp2_Button *)aui_Ldl::GetObject(s_unitManagerAdviceBlock, "UpkeepButton");
	Assert(upkeepButt);
	bool useTotalFormat = false; // force initial percent format

	UpdateUpkeepButton(upkeepButt, pl, useTotalFormat);
}

void UnitManager::UpdateReadiness()
{
	Player *pl = g_player[g_selected_item->GetVisiblePlayer()];
	if(!pl) return;

	ctp2_Static *state = (ctp2_Static *)aui_Ldl::GetObject(s_unitManagerBlock, "State");
	Assert(state);
	if(!state) return;

	switch(pl->GetReadinessLevel()) {
		case READINESS_LEVEL_PEACE:
			state->SetText(g_theStringDB->GetNameStr("str_ldl_READINESS_LEVEL_PEACE"));
			(static_cast<ctp2_Spinner*>(aui_Ldl::GetObject("UnitManager","Slider")))->SetValue(0,0);
			break;
		case READINESS_LEVEL_ALERT:
			state->SetText(g_theStringDB->GetNameStr("str_ldl_READINESS_LEVEL_ALERT"));
			(static_cast<ctp2_Spinner*>(aui_Ldl::GetObject("UnitManager","Slider")))->SetValue(1,0);
			break;
		case READINESS_LEVEL_WAR:
			state->SetText(g_theStringDB->GetNameStr("str_ldl_READINESS_LEVEL_WAR"));
			(static_cast<ctp2_Spinner*>(aui_Ldl::GetObject("UnitManager","Slider")))->SetValue(2,0);
			break;
		default:
			Assert(false);
			break;
	}

	ctp2_Button *butt = (ctp2_Button *)aui_Ldl::GetObject(s_unitManagerAdviceBlock, "UpkeepButton");
	Assert(butt);

	// keep existing format: if there is NO '%' sign then useTotalFormat==true,
	// if percent sign exist then useTotalFormat==false
	bool useTotalFormat = !strstr(butt->GetText(), "%");

	UpdateUpkeepButton(butt, pl, useTotalFormat);
}

void  UnitManager::UpdateNumUnits()
{
	ctp2_Static *counter = (ctp2_Static *)aui_Ldl::GetObject(s_unitManagerBlock, "TotalUnits");
	Assert(counter);
	if(!counter) return;

	Player *pl = g_player[g_selected_item->GetVisiblePlayer()];
	Assert(pl);
	if(!pl) return;

	MBCHAR buf[20];
	sprintf(buf, "%d", pl->m_all_units->Num());
	counter->SetText(buf);
}

void UnitManager::UpdateAdviceText()
{
	ctp2_HyperTextBox *advice =
		(ctp2_HyperTextBox *)aui_Ldl::GetObject(s_unitManagerAdviceBlock, "Advice");

	Assert(advice);
	if(advice == NULL)
		return;

	PLAYER_INDEX playerId = g_selected_item->GetVisiblePlayer();
	const Governor & governor = Governor::GetGovernor(playerId);

	SlicContext sc;
	StringId    adviceId = (sm_statsTabVisible)
                           ? governor.GetUnitsAdvice(sc)
                           : governor.GetTacticalAdvice(sc);

    if (adviceId < 0)
	{
		advice->SetHyperText("");
		return;
	}

	MBCHAR	strbuf[k_MAX_NAME_LEN];
	stringutils_Interpret
        (g_theStringDB->GetNameStr(adviceId), sc, strbuf, k_MAX_NAME_LEN);
	advice->SetHyperText(strbuf);
}

sint32 UnitManager::CompareStatItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column)
{
	sint32 idx1 = (sint32)item1->GetUserData();
	sint32 idx2 = (sint32)item2->GetUserData();
	const UnitRecord *rec1 = g_theUnitDB->Get(idx1);
	const UnitRecord *rec2 = g_theUnitDB->Get(idx2);

	switch(column) {
		case k_STATS_NAME_COL:
		case k_STATS_ICON_COL:
			return stricmp(rec1->GetNameText(), rec2->GetNameText());
		case k_STATS_COUNT_COL:
		{
			Player *pl = g_player[g_selected_item->GetVisiblePlayer()];
			if(!pl)
				return 0;

			sint32 count1 = 0, count2 = 0;
			for(sint32 i = 0; i < pl->m_all_units->Num(); i++) {
				if(pl->m_all_units->Access(i).GetType() == idx1) {
					count1++;
				}

				if(pl->m_all_units->Access(i).GetType() == idx2) {
					count2++;
				}
			}
			return count1 - count2;
		}

		case k_STATS_TYPE_COL:
			return stricmp(g_theStringDB->GetNameStr(rec1->GetCategory()),
						   g_theStringDB->GetNameStr(rec2->GetCategory()));

		case k_STATS_SUPPORT_COL:
			return rec1->GetShieldHunger() - rec2->GetShieldHunger();
		case k_STATS_A_COL:

			return sint32(rec1->GetAttack() - rec2->GetAttack());
		case k_STATS_R_COL:

			return sint32(rec1->GetZBRangeAttack() - rec2->GetZBRangeAttack());
		case k_STATS_D_COL:

			return sint32(rec1->GetDefense() - rec2->GetDefense());
		case k_STATS_M_COL:

			return sint32(rec1->GetMaxMovePoints() - rec2->GetMaxMovePoints());
		default:
			return 0;
	}
}

sint32 UnitManager::CompareTacticalItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column)
{
	Unit u1; u1.m_id = (uint32)item1->GetUserData();
	Unit u2; u2.m_id = (uint32)item2->GetUserData();
	if(!u1.IsValid() || !u2.IsValid())
		return 0;

	switch(column) {
		case k_TACTICAL_NAME_COL:
		case k_TACTICAL_ICON_COL:
			return stricmp(u1.GetName(), u2.GetName());

		case k_TACTICAL_ARMY_COL:
		{
			const MBCHAR *a1name = u1.GetArmy().IsValid() ? u1.GetArmy()->GetName() : "-";
			const MBCHAR *a2name = u2.GetArmy().IsValid() ? u2.GetArmy()->GetName() : "-";
			return stricmp(a1name, a2name);
		}

		case k_TACTICAL_ORDER_COL:
		{
			StringId o1;
			if(!u1->GetCurrentOrderString(o1)) {
				if(!g_theStringDB->GetStringID("UNIT_ORDER_NONE", o1)) {
					return 0;
				}
			}

            StringId o2;
			if(!u2->GetCurrentOrderString(o2)) {
				if(!g_theStringDB->GetStringID("UNIT_ORDER_NONE", o2)) {
					return 0;
				}
			}

			return stricmp(g_theStringDB->GetNameStr(o1),
						   g_theStringDB->GetNameStr(o2));
		}

		case k_TACTICAL_LOCATION_COL:
		{
			Unit c1 = g_theWorld->GetCell(u1.RetPos())->GetCity();
			Unit c2 = g_theWorld->GetCell(u2.RetPos())->GetCity();

			const MBCHAR *l1name, *l2name;
			if(u1.Flag(k_UDF_IN_SPACE)) {
				l1name = g_theStringDB->GetNameStr("UNIT_LOCATION_IN_SPACE");
			} else if(c1.IsValid()) {
				l1name = c1.GetName();
			} else {
				l1name = g_theStringDB->GetNameStr("UNIT_LOCATION_IN_FIELD");
			}

			if(u2.Flag(k_UDF_IN_SPACE)) {
				l2name = g_theStringDB->GetNameStr("UNIT_LOCATION_IN_SPACE");
			} else if(c2.IsValid()) {
				l2name = c2.GetName();
			} else {
				l2name = g_theStringDB->GetNameStr("UNIT_LOCATION_IN_FIELD");
			}

			return stricmp(l1name, l2name);
		}

		case k_TACTICAL_HEALTH_COL:
		{

			double p1 = u1.GetHP() / u1->CalculateTotalHP();//.GetDBRec()->GetMaxHP();
			double p2 = u2.GetHP() / u2->CalculateTotalHP();//.GetDBRec()->GetMaxHP();

			return sint32((100.0 * p1) - (100.0 * p2));
		}
		default:
			Assert(false);
			return 0;
	}
}

sint32 UnitManager::CompareAdviceItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column)
{
	UnitManagerCategoryInfo *info1 = (UnitManagerCategoryInfo *)item1->GetUserData();
	UnitManagerCategoryInfo *info2 = (UnitManagerCategoryInfo *)item2->GetUserData();

	switch(column) {
		case k_ADVICE_CATEGORY_COL:
			return stricmp(g_theStringDB->GetNameStr(info1->stringId),
						   g_theStringDB->GetNameStr(info2->stringId));
		case k_ADVICE_PERCENT_COL:
		case k_ADVICE_TOTAL_COL:

			return info1->numUnits - info2->numUnits;
		default:
			Assert(FALSE);
			return 0;
	}
}

AUI_ERRCODE UnitManager::DrawHealthBar(ctp2_Static *control, aui_Surface *surface,
									   RECT &rect, void *cookie)
{
	RECT destRect = {
		rect.left + 2,
		rect.top + 2,
		rect.right - 2,
		rect.bottom - 2
	};

	AUI_ERRCODE err = g_ui->TheBlitter()->ColorBlt(surface, &destRect, RGB(0,0,0), 0);

	if(err != AUI_ERRCODE_OK)
		return err;

	Unit u ((uint32) cookie);
	Assert(u.IsValid());
	if(!u.IsValid())
		return AUI_ERRCODE_INVALIDPARAM;

	RECT barRect = {
		destRect.left + 1,
		destRect.top + 1,
		destRect.right - 1,
		destRect.bottom - 1
	};

	sint32 width = barRect.right - barRect.left;

	double percent;
	if(u->CalculateTotalHP() > 0)//.GetDBRec()->GetMaxHP() > 0)
	{
		percent = double(u.GetHP()) / double(u->CalculateTotalHP());
	}
	else
	{
		percent = 1;
	}

	barRect.right = barRect.left + sint32(double(width) * percent);

	Pixel16 color;
	if(percent > 0.666)
	{
		color = RGB(0,255,0);
	}
	else if(percent > 0.333)
	{
		color = RGB(255,255,0);
	}
	else
	{
		color = RGB(255,0,0);
	}

	err = g_ui->TheBlitter()->ColorBlt(surface, &barRect, color, 0);

	return err;
}

void UnitManager::UpkeepButton(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Player *pl = g_player[g_selected_item->GetVisiblePlayer()];
	Assert(pl);
	if(!pl) return;

	ctp2_Button *butt = (ctp2_Button *)control;
	Assert(butt);

	// toggle existing format: if there IS '%' sign then change to total format
	// (i.e. useTotalFormat==true) and vice versa
	bool useTotalFormat = strstr(butt->GetText(), "%");

	UpdateUpkeepButton(butt, pl, useTotalFormat);
}

void UnitManager::Advice(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_unitManager && s_unitManager->m_adviceWindow);
	if (s_unitManager && s_unitManager->m_adviceWindow)
    {
	    if (g_c3ui->GetWindow(s_unitManager->m_adviceWindow->Id()))
        {
		    g_c3ui->RemoveWindow(s_unitManager->m_adviceWindow->Id());
	    }
        else
        {
		    UnitManager::UpdateAdviceText();
		    g_c3ui->AddWindow(s_unitManager->m_adviceWindow);
        }
	}
}

















void UnitManager::TacticalList(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) return;

	ctp2_ListBox *lb = (ctp2_ListBox *)control;

    ctp2_ListItem *item = lb ? (ctp2_ListItem *)lb->GetSelectedItem() : NULL;
	if(!item) return;

	Unit u(reinterpret_cast<uint32>(item->GetUserData()));
	Assert(u.IsValid());

	if (u.IsValid())
	{
		if (u.GetArmy().IsValid())
		{
			// Select the unit directly.
		}
		else if (u.IsBeingTransported())
		{
			// Select the transport.
			u = u.GetTransport();
		}
		else
		{
			// No suitable selection.
			return;
		}
	}
	g_selected_item->SetSelectUnit(u);
	g_director->AddCenterMap(u.RetPos());
}

void UnitManager::DisbandButton(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_unitManager);
	if(!s_unitManager) return;

	MessageBoxDialog::Query("str_ldl_DisbandSelectedUnits",
							"QueryUnitDisband",
							DisbandQuery);


}

void UnitManager::TabChanged(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	UnitManager::sm_statsTabVisible =
        (control == aui_Ldl::GetObject(s_unitManagerBlock, "Tabs.Stats"));
	UnitManager::UpdateAdviceText();
}

void UnitManager::DisbandQuery(bool response, void *data)
{
	if(response) {
		Assert(s_unitManager);
		s_unitManager->DisbandSelected();
	}
}

void UnitManager::DisbandSelected()
{
	Assert(m_tacticalList);
	if(!m_tacticalList) return;

	Assert(m_statsList);
	if(!m_statsList) return;

	ctp2_ListBox *theList = NULL;

	if(!m_tacticalList->IsHidden()) {
		theList = m_tacticalList;
	} else if(!m_statsList->IsHidden()) {
		theList = m_statsList;
	}

	Assert(theList);
	if(!theList) return;

	tech_WLList<sint32> * selList = theList->GetSelectedList();
	Assert(selList);
	if(!selList) return;

	if(selList->L() <= 0) return;

	g_gevManager->Pause();
	ListPos position = selList->GetHeadPosition();
	sint32 i;
	for(i = selList->L(); i > 0; i--) {
		sint32 itemIndex = selList->GetNext(position);

		ctp2_ListItem *item = (ctp2_ListItem *)theList->GetItemByIndex(itemIndex);

		if(theList == m_tacticalList) {

			Unit u; u.m_id = (uint32)item->GetUserData();

			m_lastDisbandedUnit = u.m_id;
			if(g_network.IsClient()) {
				g_network.SendAction(new NetAction(NET_ACTION_DISBAND_UNIT, u.m_id));
			}
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_DisbandUnit,
								   GEA_Unit, u.m_id,
								   GEA_End);
		} else if(theList == m_statsList) {

			sint32 unitType = (sint32)item->GetUserData();
			sint32 i;
			Player *pl = g_player[g_selected_item->GetVisiblePlayer()];
			Assert(pl);
			if(!pl) break;

			for(i = 0; i < pl->m_all_units->Num(); i++) {
				Unit u = pl->m_all_units->Access(i);
				if(u.GetType() == unitType) {

					m_lastDisbandedUnit = u.m_id;
					if(g_network.IsClient()) {
						g_network.SendAction(new NetAction(NET_ACTION_DISBAND_UNIT, u.m_id));
					}
					g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_DisbandUnit,
										   GEA_Unit, u.m_id,
										   GEA_End);
				}
			}
		}
	}
	g_gevManager->Resume();
}

/// Handle disbanding of armies
STDEHANDLER(UnitManagerArmyDisbanded)
{
    if (s_unitManager)
    {
        // Refresh all lists to remove units that have become invalid
        s_unitManager->Update();
 		s_unitManager->SetLastDisbandedUnit(0);
    }

    return GEV_HD_Continue;
}

/// Handle disbanding of individual units
/// @todo Verify whether using the "last disbanded unit" is sound.
///       This may prevent removal of invalid units from the lists when not
///       removed through the unit manager.
STDEHANDLER(UnitManagerUnitDisbanded)
{
	if(!s_unitManager) return GEV_HD_Continue;
	if(s_unitManager->GetLastDisbandedUnit() == 0) return GEV_HD_Continue;

	Unit u;


	GameEventArgument *arg = args->GetArg(GEA_Unit, 0);
	if(!arg->GetUnit(u)) {
		return GEV_HD_Continue;
	}

	if(u.m_id == s_unitManager->GetLastDisbandedUnit()) {
		s_unitManager->Update();
		s_unitManager->SetLastDisbandedUnit(0);
	}
	return GEV_HD_Continue;
}

void UnitManager::InitializeEvents()
{
	g_gevManager->AddCallback
        (GEV_DisbandUnit, GEV_PRI_Post, &s_UnitManagerUnitDisbanded);
	g_gevManager->AddCallback
        (GEV_DisbandArmyOrder, GEV_PRI_Post, &s_UnitManagerArmyDisbanded);
}

void UnitManager::CleanupEvents()
{
}

void UnitManager::ReadinessActionCallback(aui_Control *control,
													 uint32 action, uint32 data,
													 void *cookie)
{
	if(action != static_cast<uint32>(AUI_RANGER_ACTION_VALUECHANGE))
		return;

	Player *pl = g_player[g_selected_item->GetVisiblePlayer()];
	Assert(pl);
	if(!pl) return;

	ctp2_Spinner *spinner = static_cast<ctp2_Spinner*>(control);

	pl->SetReadinessLevel((READINESS_LEVEL) spinner->GetValueX());
	s_unitManager->UpdateReadiness();
}
