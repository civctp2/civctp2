//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Army manager window
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
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
// - Standardized code (May 21st 2006 Martin G�hmann)
// - Added army debug text to the army manager window. (Dec 24th 2006 Martin G�hmann)
// - Changed occurances of UnitRecord::GetMaxHP to
//   UnitData::CalculateTotalHP. (Aug 3rd 2009 Maq)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "armymanagerwindow.h"

#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_blitter.h"
#include "c3ui.h"

#include "Army.h"
#include "ArmyData.h"
#include "ArmyPool.h"

#include "ctp2_Window.h"
#include "ctp2_textfield.h"

#include "World.h"
#include "Cell.h"
#include "IconRecord.h"
#include "UnitRecord.h"
#include "ctp2_Switch.h"
#include "SelItem.h"
#include "ctp2_listitem.h"
#include "ctp2_listbox.h"
#include "ctp2_Static.h"
#include "ctp2_button.h"
#include "player.h"
#include "AICause.h"

#include "pointerlist.h"

#include "StrDB.h"

#include "GameEventUser.h"
#include "Events.h"
#include "primitives.h"
#include "colorset.h"           // g_colorSet
#include "gfx_options.h"        // g_graphicsOptions
#include "tiledmap.h"           // g_tiledMap->ColorMagnitudeToRGB()

#include "network.h"

#include "UnitPool.h"
#include "profileDB.h"          // g_theProfileDB
#include "UnitData.h"

extern C3UI *g_c3ui;

static ArmyManagerWindow *s_armyWindow = NULL;
static MBCHAR *s_armyWindowBlock = "ArmyManager";

ArmyManagerWindow::ArmyManagerWindow(AUI_ERRCODE *err)
{
	m_armies = new PointerList<ArmyListNode>;

	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_armyWindowBlock);
	Assert(m_window);
	if(!m_window) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	aui_Ldl::SetActionFuncAndCookie(s_armyWindowBlock, "NewArmyButton", ArmyManagerWindow::NewArmy, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_armyWindowBlock, "CloseButton", ArmyManagerWindow::Close, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_armyWindowBlock, "ArmiesList", ArmyManagerWindow::List, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_armyWindowBlock, "AddButton", ArmyManagerWindow::Add, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_armyWindowBlock, "AddAllButton", ArmyManagerWindow::AddAll, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_armyWindowBlock, "RemoveButton", ArmyManagerWindow::Remove, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_armyWindowBlock, "RemoveAllButton", ArmyManagerWindow::RemoveAll, NULL);

	sint32 i;
	for(i = 0; i < k_MAX_ARMY_SIZE; i++) {
		MBCHAR name[k_MAX_NAME_LEN];
		sprintf(name, "%s.InArmyBox.Unit%d", s_armyWindowBlock, i);
		aui_Ldl::SetActionFuncAndCookie(name, ArmyManagerWindow::InArmy, NULL);
		(static_cast<ctp2_Static*>(aui_Ldl::GetObject(name,"UnitHealth")))->SetDrawCallbackAndCookie(ArmyManagerWindow::DrawHealthCallbackInArmy,(void *)i);

		sprintf(name, "%s.OutOfArmyBox.Unit%d", s_armyWindowBlock, i);
		aui_Ldl::SetActionFuncAndCookie(name, ArmyManagerWindow::OutOfArmy, NULL);
		(static_cast<ctp2_Static*>(aui_Ldl::GetObject(name,"UnitHealth")))->SetDrawCallbackAndCookie(ArmyManagerWindow::DrawHealthCallbackOutOfArmy,(void *)i);
	}

	*err = AUI_ERRCODE_OK;
}

ArmyManagerWindow::~ArmyManagerWindow()
{
	if(m_armies) {
		m_armies->DeleteAll();
		delete m_armies;
	}

	if(m_window) {
		aui_Ldl::DeleteHierarchyFromRoot(s_armyWindowBlock);
	}
}

AUI_ERRCODE ArmyManagerWindow::Initialize()
{
	if(s_armyWindow)
		return AUI_ERRCODE_OK;

	AUI_ERRCODE err = AUI_ERRCODE_OK;
	s_armyWindow = new ArmyManagerWindow(&err);
	Assert(err == AUI_ERRCODE_OK);

	return err;
}

void ArmyManagerWindow::Cleanup()
{
	if (s_armyWindow)
    {
		Hide();
        allocated::clear(s_armyWindow);
	}
}

AUI_ERRCODE ArmyManagerWindow::Display()
{
	if(!s_armyWindow)
		Initialize();

	Assert(s_armyWindow);
	if(!s_armyWindow) {
		return AUI_ERRCODE_HACK;
	}

	if(g_selected_item->GetSelectedArmy(s_armyWindow->m_army)) {
		s_armyWindow->m_army.GetPos(s_armyWindow->m_pos);
	} else {

		return AUI_ERRCODE_OK;
	}


	s_armyWindow->m_armies->DeleteAll();

	s_armyWindow->FillArmies();

	AUI_ERRCODE err = AUI_ERRCODE_INVALIDPARAM;
	Assert(s_armyWindow->m_window);
	if(s_armyWindow->m_window) {
		err = g_c3ui->AddWindow(s_armyWindow->m_window);
		Assert(err == AUI_ERRCODE_OK);
		if(err == AUI_ERRCODE_OK) {
			err = s_armyWindow->m_window->Show();
		}

		s_armyWindow->Update();
		s_armyWindow->NotifySelection();
	}
	return err;
}

AUI_ERRCODE ArmyManagerWindow::Hide()
{
	if(!s_armyWindow)
		return AUI_ERRCODE_OK;

	Assert(s_armyWindow->m_window);
	if(!s_armyWindow->m_window)
		return AUI_ERRCODE_INVALIDPARAM;

	s_armyWindow->RenameArmy();

	return g_c3ui->RemoveWindow(s_armyWindow->m_window->Id());
}

bool ArmyManagerWindow::IsShown()
{
	if(!s_armyWindow)
		return false;

	if(!s_armyWindow->m_window)
		return false;

	return g_c3ui->GetWindow(s_armyWindow->m_window->Id()) != NULL;
}

void ArmyManagerWindow::Toggle()
{
	if(IsShown())
		Hide();
	else
		Display();
}

void ArmyManagerWindow::RenameArmy()
{
	if(m_army.IsValid()) {
		MBCHAR buf[32];
		ctp2_TextField *tf = (ctp2_TextField *)aui_Ldl::GetObject(s_armyWindowBlock, "ArmyName");
		if(tf) {
			tf->GetFieldText(buf, 31);
			if(buf[0] != 0 && strcmp(m_army->GetName(), buf) != 0) {
				m_army->SetName(buf);
			}
		}
	}
}

void ArmyManagerWindow::NotifySelection()
{
	if(!s_armyWindow)
		return;

	Army a;
	if (g_selected_item->GetSelectedArmy(a))
    {
		s_armyWindow->m_army        = a;
		s_armyWindow->m_pos         = a->RetPos();
	}
    else
    {
		s_armyWindow->m_army.m_id   = 0;
		s_armyWindow->m_pos         = g_selected_item->GetCurSelectPos();
	}

	s_armyWindow->UpdateArmyName();
	s_armyWindow->FillArmies();
	s_armyWindow->Update();
}

void ArmyManagerWindow::NotifyRemoteGroupComplete(const Army &army)
{
	if (army.IsValid() && army[0].IsValid())
    {
		g_selected_item->SetSelectUnit(army[0]);
	}
}

void ArmyManagerWindow::ClearAll()
{
	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_armyWindowBlock, "ArmiesList");
	Assert(lb);
	if(lb) {
		lb->Clear();
	}

	m_armies->DeleteAll();
	m_army.m_id = 0;
}

void ArmyManagerWindow::Update()
{
	sint32 i;

	Cell *cell = g_theWorld->GetCell(m_pos);
	if(!cell)
		return;

	static bool updating = false;
	if(updating) return;
	updating = true;

	MBCHAR switchName[k_MAX_NAME_LEN];

	sint32 sw = 0;

	for(i = 0; i < cell->GetNumUnits(); i++) {
		Unit u = cell->AccessUnit(i);
		if(u.IsValid()) {
			if(u.GetArmy().m_id == m_army.m_id)
				continue;

			if(!u.GetArmy().IsValid())
				continue;

			if(u.GetArmy().Num() > 1)
				continue;

			m_outOfArmy[sw] = u;

			const IconRecord *irec = u.GetDBRec()->GetDefaultIcon();

			sprintf(switchName, "OutOfArmyBox.Unit%d", sw++);
			ctp2_Switch *inCellSwitch = (ctp2_Switch *)aui_Ldl::GetObject(s_armyWindowBlock, switchName);
			Assert(inCellSwitch);
			if(inCellSwitch) {

				inCellSwitch->Enable(TRUE);
				inCellSwitch->SetState(0);
				inCellSwitch->SetImage((char *)irec->GetLargeIcon(), 0);
				inCellSwitch->SetImage((char *)irec->GetLargeIcon(), 1);
				inCellSwitch->ShouldDraw(TRUE);
			}
		}
	}

	for(i = sw; i < k_MAX_ARMY_SIZE; i++) {
		sprintf(switchName, "OutOfArmyBox.Unit%d", i);
		ctp2_Switch *inCellSwitch = (ctp2_Switch *)aui_Ldl::GetObject(s_armyWindowBlock, switchName);
		Assert(inCellSwitch);
		m_outOfArmy[i].m_id = 0;

		if(inCellSwitch) {
			inCellSwitch->SetState(0);
			inCellSwitch->SetImage(NULL, 0);
			inCellSwitch->SetImage(NULL, 1);
			inCellSwitch->Enable(FALSE);
			inCellSwitch->ShouldDraw(TRUE);
		}
	}

	if(g_theArmyPool->IsValid(m_army)) {
		for(i = 0; i < m_army->Num(); i++) {
			const IconRecord *irec = m_army[i].GetDBRec()->GetDefaultIcon();

			m_inArmy[i] = m_army[i];

			sprintf(switchName, "InArmyBox.Unit%d", i);
			ctp2_Switch *inArmySwitch = (ctp2_Switch *)aui_Ldl::GetObject(s_armyWindowBlock, switchName);
			Assert(inArmySwitch);
			if(inArmySwitch) {
				inArmySwitch->SetState(0);
				inArmySwitch->SetImage((char *)irec->GetLargeIcon(), 0);
				inArmySwitch->SetImage((char *)irec->GetLargeIcon(), 1);
				inArmySwitch->Enable(TRUE);
				inArmySwitch->ShouldDraw(TRUE);
			}
		}
	} else {
		i = 0;
	}

	for(; i < k_MAX_ARMY_SIZE; i++) {
		sprintf(switchName, "InArmyBox.Unit%d", i);
		ctp2_Switch *inArmySwitch = (ctp2_Switch *)aui_Ldl::GetObject(s_armyWindowBlock, switchName);
		Assert(inArmySwitch);

		m_inArmy[i].m_id = 0;

		if(inArmySwitch) {
			inArmySwitch->SetState(0);
			inArmySwitch->SetImage(NULL, 0);
			inArmySwitch->SetImage(NULL, 1);
			inArmySwitch->Enable(FALSE);
			inArmySwitch->ShouldDraw(TRUE);
		}
	}

	UpdateArmyName();

	ctp2_Static *armyTextlabel = (ctp2_Static *)aui_Ldl::GetObject(s_armyWindowBlock, "ArmyTextLabel");
	if(armyTextlabel){
		if((g_graphicsOptions
		&&  g_graphicsOptions->IsArmyTextOn()
		||  g_theProfileDB->GetDebugAI())
		&& m_army.IsValid()
		&& m_army->GetDebugString()
		){
			armyTextlabel->SetText(m_army->GetDebugString());

			sint32		r,g,b;
			uint8		col = m_army.GetData()->GetDebugStringColor();

			g_tiledMap->ColorMagnitudeToRGB(col, &r, &g, &b);

			COLORREF	fgColor = RGB(r, g, b);
			// not used COLORREF	bgColor = RGB(0,0,0);

			armyTextlabel->SetTextColor(fgColor);
			armyTextlabel->SetTextShadow(true);
			armyTextlabel->SetTextShadowColor(fgColor); /// @todo bgColor?
		}
		else{
			armyTextlabel->SetText("");
		}
		armyTextlabel->ShouldDraw(true);
	}

	updating = false;
}

void ArmyManagerWindow::UpdateArmyName()
{
	ctp2_TextField *tf = (ctp2_TextField *)aui_Ldl::GetObject(s_armyWindowBlock, "ArmyName");
	if(tf) {
		if(m_army.IsValid()) {
			tf->Enable(TRUE);
			tf->SetFieldText(m_army->GetName());
		} else {
			tf->Enable(FALSE);
			tf->SetFieldText("");
		}
	}
}

void ArmyManagerWindow::UpdateArmyItem(ctp2_ListItem *item)
{
	ArmyListNode *node = (ArmyListNode *)item->GetUserData();
	Assert(node);
	if(!node)
		return;

	ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
	Assert(box);
	if(box) {
		ctp2_Static *icon = (ctp2_Static *)box->GetChildByIndex(0);
		Assert(icon);
		if(icon) {

			icon->SetImage("upic21.tga");
		}

		ctp2_Static *count = (ctp2_Static *)box->GetChildByIndex(1);
		Assert(count);
		if(count) {
			MBCHAR text[20];
			if(g_theArmyPool->IsValid(node->m_army)) {
				sprintf(text, "%d", node->m_army.Num());
			} else {
				strcpy(text, "0");
			}
			count->SetText(text);
		}
	}
}

void ArmyManagerWindow::UpdateAllArmyItems()
{
	ctp2_ListBox *list = (ctp2_ListBox *)aui_Ldl::GetObject(s_armyWindowBlock, "ArmiesList");

	for(sint32 i = 0; i < list->NumItems(); i++)
	{
		ctp2_ListItem *item = (ctp2_ListItem *)list->GetItemByIndex(i);
		Assert(item);
		if(item)
		{
			UpdateArmyItem(item);
		}
	}
}

ctp2_ListItem *ArmyManagerWindow::AddArmyItem(ctp2_ListBox *listBox, ArmyListNode *node)
{
	ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ArmyManagerItem");
	Assert(item);
	if(item)
	{
		item->SetUserData(node);
		UpdateArmyItem(item);
		listBox->AddItem(item);
	}

	return item;
}

void ArmyManagerWindow::RemoveDeadArmies()
{
	ctp2_ListBox *armyList = (ctp2_ListBox *)aui_Ldl::GetObject(s_armyWindowBlock, "ArmiesList");
	Assert(armyList);
	if(armyList) {

		PointerList<ArmyListNode>::Walker walk(m_armies);
		sint32 i = 0;

		while(walk.IsValid()) {
			ctp2_ListItem *item = (ctp2_ListItem *)armyList->GetItemByIndex(i);
			Assert((ArmyListNode *)item->GetUserData() == walk.GetObj());
			if((walk.GetObj()->m_army.m_id != 0) &&
			   !g_theArmyPool->IsValid(walk.GetObj()->m_army)) {
				walk.Remove();
				armyList->RemoveItemByIndex(i);
				delete item;
			} else {
				walk.Next();
				i++;
			}
		}
	}
}

void ArmyManagerWindow::UpdateList()
{
	ctp2_ListBox *armyList = (ctp2_ListBox *)aui_Ldl::GetObject(s_armyWindowBlock, "ArmiesList");
	Assert(armyList);

	Army a;
	g_selected_item->GetSelectedArmy(a);

	if(armyList) {
		armyList->Clear();

		PointerList<ArmyListNode>::Walker walk(m_armies);
		while(walk.IsValid()) {
			ctp2_ListItem *item = AddArmyItem(armyList, walk.GetObj());
			if((walk.GetObj()->m_army == a) && (a.m_id != 0))
				armyList->SelectItem(item);

			walk.Next();
		}
	}
}

void ArmyManagerWindow::FillArmies()
{
	sint32 i, j;
	Cell *cell = g_theWorld->GetCell(m_pos);
	Assert(cell);
	if(!cell)
		return;

	PointerList<ArmyListNode>::Walker walk(m_armies);
	while(walk.IsValid()) {

		if(walk.GetObj()->m_army.m_id != 0) {
			delete walk.Remove();
		} else {
			walk.Next();
		}
	}

	for(i = 0; i < cell->GetNumUnits(); i++) {
		bool alreadyIn = false;

		for(j = 0; j < i; j++) {
			if(cell->AccessUnit(j).GetArmy().m_id == cell->AccessUnit(i).GetArmy().m_id) {
				alreadyIn = true;
				break;
			}
		}

		if(alreadyIn)
			continue;

		Unit u = cell->AccessUnit(i);
		Army a= u.GetArmy();
		if(!a.IsValid() || (a.Num() < 2 && a.m_id != m_army.m_id)) continue;

		m_armies->AddTail(new ArmyListNode(cell->AccessUnit(i).GetArmy()));
	}

	UpdateList();
}

void ArmyManagerWindow::NewArmy(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_armyWindow);
	if(!s_armyWindow) return;

	s_armyWindow->m_armies->AddTail(new ArmyListNode());
	ctp2_ListBox *armyList = (ctp2_ListBox *)aui_Ldl::GetObject(s_armyWindowBlock, "ArmiesList");
	Assert(armyList);
	if(armyList) {
		armyList->SelectItem(s_armyWindow->AddArmyItem(armyList, s_armyWindow->m_armies->GetTail()));
	}

	s_armyWindow->Update();
}

void ArmyManagerWindow::Close(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	if(s_armyWindow) {
		s_armyWindow->RenameArmy();
		if(g_theArmyPool->IsValid(s_armyWindow->m_army) &&
			(s_armyWindow->m_army.Num() > 0)) {
			g_selected_item->SetSelectUnit(s_armyWindow->m_army[0]);
		}
	}

	Hide();
}

void ArmyManagerWindow::List(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_LISTBOX_ACTION_SELECT) return;
	Assert(s_armyWindow);
	if(!s_armyWindow) return;

	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_armyWindowBlock, "ArmiesList");
	Assert(lb);
	if(!lb) return;

	s_armyWindow->RenameArmy();

	ctp2_ListItem *selItem = (ctp2_ListItem *)lb->GetSelectedItem();
	if(selItem) {
		ArmyListNode *node = (ArmyListNode *)selItem->GetUserData();
		s_armyWindow->m_army = node->m_army;

		if(g_theArmyPool->IsValid(node->m_army) &&
		   node->m_army.Num() > 0) {
			Army a;
			if(g_selected_item->GetSelectedArmy(a)) {
				if(a.m_id != node->m_army.m_id) {

					g_selected_item->SetSelectUnit(node->m_army[0]);
				} else {
					s_armyWindow->Update();
				}
			} else {

				g_selected_item->SetSelectUnit(node->m_army[0]);
			}
		} else {
			s_armyWindow->m_army.m_id = 0;
			s_armyWindow->Update();

		}
	} else {
		s_armyWindow->m_army.m_id = 0;
		g_selected_item->Deselect(g_selected_item->GetVisiblePlayer());
		s_armyWindow->Update();
	}
}

void ArmyManagerWindow::Add(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_armyWindow);
	if(!s_armyWindow) return;

	s_armyWindow->AddSelectedUnits();
}

void ArmyManagerWindow::AddAll(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_armyWindow);
	if(!s_armyWindow) return;

	sint32 i;
	for(i = 0; i < k_MAX_ARMY_SIZE; i++) {
		MBCHAR switchName[k_MAX_NAME_LEN];
		sprintf(switchName, "OutOfArmyBox.Unit%d", i);
		ctp2_Switch *sw = (ctp2_Switch *)aui_Ldl::GetObject(s_armyWindowBlock, switchName);
		Assert(sw);
		if(s_armyWindow->m_outOfArmy[i].m_id)
		{
			sw->SetState(1);
		}
	}
	s_armyWindow->AddSelectedUnits();
}

void ArmyManagerWindow::RemoveAll(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_armyWindow);
	if(!s_armyWindow) return;

	sint32 i;
	for(i = 0; i < k_MAX_ARMY_SIZE; i++) {
		MBCHAR switchName[k_MAX_NAME_LEN];
		sprintf(switchName, "InArmyBox.Unit%d", i);
		ctp2_Switch *sw = (ctp2_Switch *)aui_Ldl::GetObject(s_armyWindowBlock, switchName);
		Assert(sw);
		if(s_armyWindow->m_inArmy[i].m_id)
		{
			sw->SetState(1);
		}
	}
	s_armyWindow->RemoveSelectedUnits();
}

void ArmyManagerWindow::AddSelectedUnits()
{
	ctp2_ListBox *armyList = (ctp2_ListBox *)aui_Ldl::GetObject(s_armyWindowBlock, "ArmiesList");
	Assert(armyList);
	if(!armyList)
		return;

	ctp2_ListItem *item = (ctp2_ListItem *)armyList->GetSelectedItem();
	ArmyListNode *node;
	if(!item)
	{
		node = new ArmyListNode();
	}
	else
	{
		node = (ArmyListNode *)item->GetUserData();
	}

	sint32 i;
	CellUnitList units;

	for(i = 0; i < k_MAX_ARMY_SIZE; i++) {
		MBCHAR switchName[k_MAX_NAME_LEN];
		sprintf(switchName, "OutOfArmyBox.Unit%d", i);
		ctp2_Switch *sw = (ctp2_Switch *)aui_Ldl::GetObject(s_armyWindowBlock, switchName);
		Assert(sw);
		if(sw && sw->IsSelected()) {
			Assert(m_outOfArmy[i].IsValid());
			if(m_outOfArmy[i].IsValid()) {
				units.Insert(m_outOfArmy[i]);
			}
		}
	}

	if(g_network.IsClient()) {
		if(node->m_army.m_id != 0) {
			Assert(node->m_army.IsValid());
			if(!node->m_army.IsValid()) {
				g_network.RequestResync(RESYNC_INVALID_ARMY_OTHER);
			} else {
				g_network.SendGroupRequest(units, node->m_army);
			}
		} else {
			g_network.SendGroupRequest(units, Army());
		}
		return;
	}

	Army theArmy;
	bool newArmy = false;
	if(node->m_army.m_id == 0) {
		theArmy = g_player[g_selected_item->GetVisiblePlayer()]->GetNewArmy(CAUSE_NEW_ARMY_GROUPING);
		node->m_army = theArmy;
		m_army = theArmy;
	} else {
		Assert(g_theArmyPool->IsValid(node->m_army));
		if(g_theArmyPool->IsValid(node->m_army)) {
			theArmy = node->m_army;
		} else {

			theArmy = g_player[g_selected_item->GetVisiblePlayer()]->GetNewArmy(CAUSE_NEW_ARMY_GROUPING);
			node->m_army = theArmy;
		}
	}

	for(i = units.Num() - 1; i >= 0; i--) {
		theArmy->GroupUnit(units[i]);
	}

	if(newArmy) {
		m_armies->AddTail(new ArmyListNode(theArmy));
		AddArmyItem(armyList, m_armies->GetTail());
	}

	Army selArmy;
	if(g_selected_item->GetSelectedArmy(selArmy)) {
		if(selArmy.m_id != theArmy.m_id) {
			g_selected_item->SetSelectUnit(theArmy[0]);
		}
	} else {
		if(theArmy.Num() && theArmy[0].IsValid()) {
			g_selected_item->SetSelectUnit(theArmy[0]);
		}
	}











	UpdateAllArmyItems();

	RemoveDeadArmies();
	Update();
}

void ArmyManagerWindow::Remove(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_armyWindow);
	if(!s_armyWindow) return;

	s_armyWindow->RemoveSelectedUnits();
}

void ArmyManagerWindow::RemoveSelectedUnits()
{
	ctp2_ListBox *armyList = (ctp2_ListBox *)aui_Ldl::GetObject(s_armyWindowBlock, "ArmiesList");
	Assert(armyList);
	if(!armyList)
		return;

	ctp2_ListItem *item = (ctp2_ListItem *)armyList->GetSelectedItem();
	if(!item) {

		return;
	}

	ArmyListNode *node = (ArmyListNode *)item->GetUserData();

	Army theArmy;
	if((node->m_army.m_id == 0) || (node->m_army.Num() < 1)) {

		return;
	}

	theArmy = node->m_army;
	sint32 i;

	CellUnitList units;

	for(i = 0; i < k_MAX_ARMY_SIZE; i++) {
		MBCHAR switchName[k_MAX_NAME_LEN];
		sprintf(switchName, "InArmyBox.Unit%d", i);
		ctp2_Switch *sw = (ctp2_Switch *)aui_Ldl::GetObject(s_armyWindowBlock, switchName);
		Assert(sw);
		if(sw && sw->IsSelected()) {
			Assert(m_inArmy[i].IsValid());
			Assert(m_inArmy[i].GetArmy() == theArmy);
			if(m_inArmy[i].IsValid()) {
				if(g_network.IsClient()) {


					units.Insert(m_inArmy[i]);
				} else {
					Army newArmy = g_player[g_selected_item->GetVisiblePlayer()]->GetNewArmy(CAUSE_NEW_ARMY_UNGROUPING);
					m_inArmy[i].ChangeArmy(newArmy, CAUSE_NEW_ARMY_UNGROUPING);

					m_armies->AddTail(new ArmyListNode(newArmy));
					AddArmyItem(armyList, m_armies->GetTail());
				}
			}
		}
	}

	if(g_network.IsClient()) {
		g_network.SendUngroupRequest(theArmy, units);
		return;
	}

	if(!g_theArmyPool->IsValid(theArmy) ||
		theArmy.Num() <= 0)
		g_selected_item->Deselect(g_selected_item->GetVisiblePlayer());

	NotifySelection();




}

void ArmyManagerWindow::InArmy(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action == AUI_SWITCH_ACTION_DOUBLECLICK)
	{
		Assert(s_armyWindow);
		if(s_armyWindow) {
			s_armyWindow->RemoveSelectedUnits();
		}
	}

	bool enableRemoveButton		= false;
	bool enableRemoveAllButton	= false;
	for (int i = 0; (i < k_MAX_ARMY_SIZE) && !enableRemoveButton; ++i)
	{
		MBCHAR switchName[k_MAX_NAME_LEN];
		sprintf(switchName, "InArmyBox.Unit%d", i);
		ctp2_Switch *sw = (ctp2_Switch *)aui_Ldl::GetObject(s_armyWindowBlock, switchName);
		Assert(sw);
		enableRemoveButton = sw && sw->IsSelected();

		if (s_armyWindow->m_inArmy[i].m_id)
		{
			enableRemoveAllButton=true;
		}
	}

	(static_cast<ctp2_Button*>(aui_Ldl::GetObject("ArmyManager.RemoveButton")))->Enable(enableRemoveButton);
	(static_cast<ctp2_Button*>(aui_Ldl::GetObject("ArmyManager.RemoveAllButton")))->Enable(enableRemoveAllButton);
}

void ArmyManagerWindow::OutOfArmy(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action == AUI_SWITCH_ACTION_DOUBLECLICK)
	{
		Assert(s_armyWindow);
		if(s_armyWindow) {
			s_armyWindow->AddSelectedUnits();
		}
	}

	bool enableAddButton	= false;
	bool enableAddAllButton	= false;
//	ctp2_ListBox *armyList = (ctp2_ListBox *)aui_Ldl::GetObject(s_armyWindowBlock, "ArmiesList");
	for (int i = 0; (i < k_MAX_ARMY_SIZE) && ! enableAddButton; ++i)
	{
		MBCHAR switchName[k_MAX_NAME_LEN];
		sprintf(switchName, "OutOfArmyBox.Unit%d", i);
		ctp2_Switch * sw = (ctp2_Switch *)aui_Ldl::GetObject(s_armyWindowBlock, switchName);
		Assert(sw);
		if(s_armyWindow->m_outOfArmy[i].m_id)
		{
			enableAddAllButton=true;
		}
		if (sw && sw->IsSelected())
		{
			enableAddButton = true;
		}
	}

	(static_cast<ctp2_Button*>(aui_Ldl::GetObject("ArmyManager.AddButton")))->Enable(enableAddButton);
	(static_cast<ctp2_Button*>(aui_Ldl::GetObject("ArmyManager.AddAllButton")))->Enable(enableAddAllButton);
}

AUI_ERRCODE ArmyManagerWindow::DrawHealthCallbackInArmy(ctp2_Static *control, aui_Surface *surface, RECT &rect, void *cookie)
{
	if (s_armyWindow->m_inArmy[(int)cookie].IsValid())
	{
		sint32 const maxhp		= s_armyWindow->m_inArmy[(int)cookie]->CalculateTotalHP();//.GetDBRec()->GetMaxHP();
		sint32 const curhp		= static_cast<sint32>(s_armyWindow->m_inArmy[(int)cookie].GetHP());
		sint32 const width		= rect.right - rect.left;
		sint32 const hpwidth	= width * curhp / maxhp;
		Pixel16 drawColor=(	hpwidth > (width/2)?g_colorSet->GetColor(COLOR_GREEN):
							hpwidth > (width/4)?g_colorSet->GetColor(COLOR_YELLOW):
							g_colorSet->GetColor(COLOR_RED));

		primitives_DrawLine16(surface, rect.left, rect.top, rect.left+hpwidth, rect.top, drawColor);
		primitives_DrawLine16(surface, rect.left, rect.bottom, rect.left+hpwidth, rect.bottom, drawColor);
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE ArmyManagerWindow::DrawHealthCallbackOutOfArmy(ctp2_Static *control, aui_Surface *surface, RECT &rect, void *cookie)
{
	if (s_armyWindow->m_outOfArmy[(int)cookie].IsValid())
	{
		sint32 const maxhp		= s_armyWindow->m_outOfArmy[(int)cookie]->CalculateTotalHP();//.GetDBRec()->GetMaxHP();
		sint32 const curhp		= static_cast<sint32>(s_armyWindow->m_outOfArmy[(int)cookie].GetHP());
		sint32 const width		= rect.right - rect.left;
		sint32 const hpwidth	= width * curhp / maxhp;
		Pixel16 drawColor=(	hpwidth > (width/2)?g_colorSet->GetColor(COLOR_GREEN):
							hpwidth > (width/4)?g_colorSet->GetColor(COLOR_YELLOW):
							g_colorSet->GetColor(COLOR_RED));

		primitives_DrawLine16(surface, rect.left, rect.top, rect.left+hpwidth, rect.top, drawColor);
		primitives_DrawLine16(surface, rect.left, rect.bottom, rect.left+hpwidth, rect.bottom, drawColor);
	}

	return AUI_ERRCODE_OK;
}

STDEHANDLER(ArmyManagerArmyMoved)
{
	Army a;
	if(!args->GetArmy(0, a))
		return GEV_HD_Continue;

    if (a.GetOwner() == g_selected_item->GetVisiblePlayer())
    {
        ArmyManagerWindow::NotifySelection();
    }

    return GEV_HD_Continue;
}

void ArmyManagerWindow::InitializeEvents()
{
	g_gevManager->AddCallback(GEV_MoveUnits, GEV_PRI_Post, &s_ArmyManagerArmyMoved);
}

void ArmyManagerWindow::CleanupEvents()
{
}
