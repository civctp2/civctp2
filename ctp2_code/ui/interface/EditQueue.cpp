//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : City build queue handling.
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
// - Switched INF/CAP typo corrected by Peter Triggs
// - Start the great library with the current research project of the player.
// - Made rush buy button behaviour consistent with other windows.
// - Disabled rush-buy button if infrastructure or capitalization are
//   at the front of the build queue, by Martin Gühmann.
// - If infrastructure or capitalization are at the front of the
//   build queue turns are shown anymore, by Martin Gühmann.
// - Disabled rush buy button when it is not your turn by Martin Gühmann.
// - Repaired CtD when double-clicking on an empty build queue - caused by
//   the changes above.
// - #01 Standardization of city selection and focus handling
//   (L. Hirth 6/2004)
// - Added National Manager button and functions callback. - July 24th 2005 Martin Gühmann
// - Made Build Manager window non-modal. - July 24th 2005 Martin Gühmann
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Added a suggest build item button to the build manager for AI testing. (30-Jun-2008 Martin Gühmann)
// - Added stuff for reimplementing switch production penalty. (22-Jul-2009 Maq)
// - Fixed AI city rank calculation. (9-Nov-2009 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "EditQueue.h"

#include "aui_ldl.h"
#include "ctp2_Window.h"
#include "ctp2_listitem.h"
#include "ctp2_listbox.h"
#include "citydata.h"
#include "ctp2_button.h"
#include "aui_ranger.h"
#include "ctp2_dropdown.h"
#include "MessageBoxDialog.h"
#include "StrDB.h"
#include "ctp2_textfield.h"

#include "citywindow.h"
#include "ConstRecord.h"

#include "UnitRecord.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"

#include "c3ui.h"
#include "screenutils.h"

#include "greatlibrary.h"
#include "SlicContext.h"

#include "ctp2_Static.h"
#include "ctp2_hypertextbox.h"
#include "player.h"
#include "SelItem.h"

#include "UnitDynArr.h"

#include "Gold.h"

#include "network.h"
#include "IconRecord.h"
#include "NationalManagementDialog.h"
#include "Globals.h"
#include "governor.h"
#include "buildingutil.h"
#include "wonderutil.h"
#include "mapanalysis.h"
#include "stringutils.h"

#include "prjfile.h"

extern ProjectFile * g_GreatLibPF;

static EditQueue * s_editQueue = NULL;

static MBCHAR * s_editQueueBlock = "BuildEditorWindow";

extern C3UI * g_c3ui;

EditItemInfo * EditItemInfo::Construct(const char * string)
{
	sint32 category = -1;
	sint32 type = -1;
	switch (string[0])
	{
		case 'B':
			category = k_GAME_OBJ_TYPE_IMPROVEMENT;
			if (!g_theBuildingDB->GetNamedItem(&string[2], type)) {
				return NULL;
			}
			break;
		case 'W':
			category = k_GAME_OBJ_TYPE_WONDER;
			if (!g_theWonderDB->GetNamedItem(&string[2], type)) {
				return NULL;
			}
			break;
		case '#':
			return NULL;
		case 'C':
			if (string[1] == 'A' && string[2] == 'P')
			{
				category = k_GAME_OBJ_TYPE_CAPITALIZATION;
				type = 0;
			}
			else {
				return NULL;
			}
			break;
		case 'I':
			if (string[1] == 'N' && string[2] == 'F')
			{
				category = k_GAME_OBJ_TYPE_INFRASTRUCTURE;
				type = 0;
			}
			else {
				return NULL;
			}
			break;
		default:
			category = k_GAME_OBJ_TYPE_UNIT;
			if (!g_theUnitDB->GetNamedItem(&string[2], type)) {
				return NULL;
			}
			break;
	}
	return new EditItemInfo(category, type);
}

const char * EditItemInfo::GetCategory() const
{
	switch (m_category)
	{
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
			return "B ";
		case k_GAME_OBJ_TYPE_WONDER:
			return "W ";
		case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
			return "INF";
		case k_GAME_OBJ_TYPE_CAPITALIZATION:
			return "CAP";
		default:
			return "U ";
	}
}

const char * EditItemInfo::GetID() const
{
	switch (m_category)
	{
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
			return g_theBuildingDB->Get(m_type)->GetIDText();
		case k_GAME_OBJ_TYPE_WONDER:
			return g_theWonderDB->Get(m_type)->GetIDText();
		case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
		case k_GAME_OBJ_TYPE_CAPITALIZATION:
			return "";
		default:
			return g_theUnitDB->Get(m_type)->GetIDText();
	}
}

const char * EditItemInfo::GetName() const
{
	switch (m_category) {
		case k_GAME_OBJ_TYPE_WONDER:
			return g_theWonderDB->Get(m_type)->GetNameText();
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
			return g_theBuildingDB->Get(m_type)->GetNameText();
		case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
			return g_theStringDB->GetNameStr("INFRASTRUCTURE");
		case k_GAME_OBJ_TYPE_CAPITALIZATION:
			return g_theStringDB->GetNameStr("CAPITALIZATION");
		default:
			return g_theUnitDB->Get(m_type)->GetNameText();
	}
}

sint32 EditItemInfo::GetCost() const
{
	switch (m_category) {
		case k_GAME_OBJ_TYPE_WONDER:
			return wonderutil_Get(m_type, g_selected_item->GetVisiblePlayer())->GetProductionCost();
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
			return buildingutil_Get(m_type, g_selected_item->GetVisiblePlayer())->GetProductionCost();
		case k_GAME_OBJ_TYPE_CAPITALIZATION:
		case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
			return 0;
		default:
			return g_theUnitDB->Get(m_type)->GetShieldCost();
	}
}

EditQueue::EditQueue(AUI_ERRCODE * error)
{
	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_editQueueBlock);
	Assert(m_window);
	if (!m_window) {
		*error = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	m_itemsBox = (ctp2_Static *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox");
	m_queueBox = (ctp2_Static *)aui_Ldl::GetObject(s_editQueueBlock, "QueueGroup");

	m_unitList = (ctp2_ListBox *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox.UnitsList");
	Assert(m_unitList);
	m_unitList->SetActionFuncAndCookie(EditQueue::ListCallback, NULL);

	m_buildingList = (ctp2_ListBox *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox.BuildingList");
	Assert(m_buildingList);
	m_buildingList->SetActionFuncAndCookie(EditQueue::ListCallback, NULL);

	m_wonderList = (ctp2_ListBox *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox.WonderList");
	Assert(m_wonderList);
	m_wonderList->SetActionFuncAndCookie(EditQueue::ListCallback, NULL);

	m_queueList = (ctp2_ListBox *)aui_Ldl::GetObject(s_editQueueBlock, "QueueGroup.QueueList");
	Assert(m_queueList);
	m_queueList->SetActionFuncAndCookie(QueueListCallback, NULL);

	m_unitsButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox.UnitsButton");
	Assert(m_unitsButton);
	m_unitsButton->SetActionFuncAndCookie(EditQueue::ToggleUnits, NULL);

	m_buildingsButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox.BuildingsButton");
	Assert(m_buildingsButton);
	m_buildingsButton->SetActionFuncAndCookie(EditQueue::ToggleBuildings, NULL);

	m_wondersButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox.WondersButton");
	Assert(m_wondersButton);
	m_wondersButton->SetActionFuncAndCookie(EditQueue::ToggleWonders, NULL);
	//EMOD to have button that has all units buildings, and wonders in the build box instead of sort
	//m_AllButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox.AllButton");

	m_libraryButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "LibraryButton");
	Assert(m_libraryButton);
	m_libraryButton->SetActionFuncAndCookie(EditQueue::Library, NULL);

	m_addButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox.AddButton");
	m_insertButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox.InsertButton");
	m_suggestButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox.SuggestButton");
	m_removeButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "QueueGroup.RemoveButton");
	m_upButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "QueueGroup.UpButton");
	m_downButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "QueueGroup.DownButton");
	m_clearButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "QueueGroup.ClearButton");

	m_unitList->SetKeyboardActionControl(m_addButton);
	m_buildingList->SetKeyboardActionControl(m_addButton);
	m_wonderList->SetKeyboardActionControl(m_addButton);

	m_addButton->SetActionFuncAndCookie(EditQueue::AddItem, NULL);
	m_insertButton->SetActionFuncAndCookie(EditQueue::InsertItem, NULL);
	m_suggestButton->SetActionFuncAndCookie(EditQueue::SuggestItem, NULL);
	m_removeButton->SetActionFuncAndCookie(EditQueue::RemoveItem, NULL);
	m_upButton->SetActionFuncAndCookie(EditQueue::ItemUp, NULL);
	m_downButton->SetActionFuncAndCookie(EditQueue::ItemDown, NULL);
	m_clearButton->SetActionFuncAndCookie(EditQueue::ClearButton, NULL);

	m_itemImageButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "ItemImage.IconBorder.IconButton");
	Assert(m_itemImageButton);
	m_itemImageButton->Enable(false);
	m_itemDescription = (ctp2_HyperTextBox *)aui_Ldl::GetObject(s_editQueueBlock, "ItemDescription");

	m_itemsBox = (ctp2_Static *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox");
	m_loadBox = (ctp2_Static *)aui_Ldl::GetObject(s_editQueueBlock, "LoadBox");

	Assert(m_itemsBox);
	Assert(m_loadBox);

	m_singleCityChooser = (ctp2_Static *)aui_Ldl::GetObject(s_editQueueBlock, "SingleCityChooser");
	m_multiCityList = (ctp2_ListBox *)aui_Ldl::GetObject(s_editQueueBlock, "MultiCityList");
	m_multiCityList->SetAbsorbancy(false);

	Assert(m_singleCityChooser && m_multiCityList);

	m_cityDropDown = (ctp2_DropDown *)aui_Ldl::GetObject(s_editQueueBlock, "SingleCityChooser.Pulldown");
	Assert(m_cityDropDown);
	if(m_cityDropDown) m_cityDropDown->SetActionFuncAndCookie(CityDropDown, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_editQueueBlock, "SingleCityChooser.Previous", PreviousCity, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_editQueueBlock, "SingleCityChooser.Next", NextCity, NULL);

	m_rushBuyButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "RushBuyButton");
	Assert(m_rushBuyButton);
	m_rushBuyButton->SetActionFuncAndCookie(RushBuyCallback, NULL);
	m_rushBuyCost = (ctp2_Static *)aui_Ldl::GetObject(s_editQueueBlock, "RushBuyCost");

	aui_Ldl::SetActionFuncAndCookie(s_editQueueBlock, "NormalModeButtons.CloseButton", EditQueue::Close, NULL);

	aui_Ldl::SetActionFuncAndCookie(s_editQueueBlock, "ItemsBox.LoadSaveButton", EditQueue::LoadModeCallback, NULL);

	m_createCustomQueueButton = (ctp2_Button *) aui_Ldl::GetObject(s_editQueueBlock, "NormalModeButtons.CustomButton");
	Assert(m_createCustomQueueButton);
	m_createCustomQueueButton->SetActionFuncAndCookie(EditQueue::CustomButton, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_editQueueBlock, "CustomModeButtons.CancelButton", EditQueue::CustomButton, NULL);

	m_saveQueueButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "CustomModeButtons.SaveButton");
	Assert(m_saveQueueButton);
	m_saveQueueButton->SetActionFuncAndCookie(EditQueue::SaveButton, NULL);

	m_multiButtonGroup = (ctp2_Static *)aui_Ldl::GetObject(s_editQueueBlock, "MultiGroup");
	Assert(m_multiButtonGroup);

	aui_Ldl::SetActionFuncAndCookie(s_editQueueBlock, "MultiGroup.InsertButton", EditQueue::MultiActionButton,
			(void *)EDIT_QUEUE_MULTI_ACTION_INSERT);
	aui_Ldl::SetActionFuncAndCookie(s_editQueueBlock, "MultiGroup.AppendButton", EditQueue::MultiActionButton,
			(void *)EDIT_QUEUE_MULTI_ACTION_APPEND);
	aui_Ldl::SetActionFuncAndCookie(s_editQueueBlock, "MultiGroup.OverwriteButton", EditQueue::MultiActionButton,
			(void *)EDIT_QUEUE_MULTI_ACTION_OVERWRITE);

	m_queueName = (ctp2_Static *)aui_Ldl::GetObject(s_editQueueBlock, "LoadBox.QueueName");

	m_queueFileList = (ctp2_ListBox *)aui_Ldl::GetObject(s_editQueueBlock, "LoadBox.QueuesList");
	m_queueFileList->SetActionFuncAndCookie(QueueFileList, NULL);
	m_queueContents = (ctp2_ListBox *)aui_Ldl::GetObject(s_editQueueBlock, "LoadBox.Contents");
	Assert(m_queueContents);
	m_queueContents->SetActionFuncAndCookie(EditQueue::ListCallback, NULL);

	m_loadModeLoadButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "LoadBox.LoadButton");
	Assert(m_loadModeLoadButton);
	m_loadModeLoadButton->SetActionFuncAndCookie(EditQueue::LoadCallback, NULL);

	aui_Ldl::SetActionFuncAndCookie(s_editQueueBlock, "LoadBox.CancelButton", EditQueue::LoadModeCallback, NULL);
	m_deleteButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "LoadBox.DeleteButton");
	Assert(m_deleteButton);
	m_deleteButton->SetActionFuncAndCookie(EditQueue::DeleteCallback, NULL);

	m_modeLabel = (ctp2_Static *)aui_Ldl::GetObject(s_editQueueBlock, "ItemsBox.ModeLabel");

	m_customModeButtons = (ctp2_Static *)aui_Ldl::GetObject(s_editQueueBlock, "CustomModeButtons");
	m_normalModeButtons = (ctp2_Static *)aui_Ldl::GetObject(s_editQueueBlock, "NormalModeButtons");

	m_queueLabel = (ctp2_Static *)aui_Ldl::GetObject(s_editQueueBlock, "QueueGroup.QueueLabel");

	m_gotoCityButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "GotoCityButton");
	m_gotoCityButton->SetActionFuncAndCookie(GotoCity, NULL);

	m_nationalManagerButton = (ctp2_Button *)aui_Ldl::GetObject(s_editQueueBlock, "NationalManagerButton");
	if(m_nationalManagerButton) m_nationalManagerButton->SetActionFuncAndCookie(OpenNationalManager, NULL);

	m_attachedToWindow = NULL;

	m_city = 0;

	m_updating = false;

	m_mode = EDIT_QUEUE_MODE_SINGLE;
	m_oldMode = m_mode;

	m_listBeforeLoadSaveMode = NULL;

	m_itemCategory = -1;
	m_itemType = -1;
}

EditQueue::~EditQueue()
{
	ClearChoiceLists();

	if (m_window)
	{
		aui_Ldl::DeleteHierarchyFromRoot(s_editQueueBlock);
		m_window = NULL;
	}

	m_customBuildList.DeleteAll();
	m_multiCities.DeleteAll();
	m_queueFileNames.DeleteAll();
}

AUI_ERRCODE EditQueue::Initialize()
{
	if (s_editQueue) {
		return AUI_ERRCODE_OK;
	}

	AUI_ERRCODE err = AUI_ERRCODE_OK;
	s_editQueue = new EditQueue(&err);

	Assert(err == AUI_ERRCODE_OK);
	return err;
}

AUI_ERRCODE EditQueue::Display(const Unit & city)
{
	if (g_network.IsClient() && g_network.GetSensitiveUIBlocked()) {
		return AUI_ERRCODE_OK;
	}

	AUI_ERRCODE err = Display();

	SetCity(city);
	DoFocusList(s_editQueue->GetVisibleItemList(), true);

	return err;
}

AUI_ERRCODE EditQueue::Display(const UnitDynamicArray & cities)
{
	AUI_ERRCODE err = Display();

	SetMultiCities(cities);

	ctp2_ListBox *visibleList = s_editQueue->GetVisibleItemList();

	if(visibleList && visibleList->NumItems() > 0 && !visibleList->GetSelectedItem()) {
		visibleList->SelectItem((sint32)0);
	}

	return err;
}

AUI_ERRCODE EditQueue::Display()
{
	if (!s_editQueue) {
		Initialize();
	}

	Assert(s_editQueue);
	if (!s_editQueue) {
		return AUI_ERRCODE_NOCONTROL;
	}

	g_c3ui->AddWindow(s_editQueue->m_window);

	AUI_ERRCODE err = s_editQueue->m_window->Show();

	if (err == AUI_ERRCODE_OK) {
		g_c3ui->BringWindowToTop(s_editQueue->m_window);
	}

	if (s_editQueue->m_loadBox) {
		s_editQueue->m_loadBox->Hide();
	}

	s_editQueue->ClearChoiceLists();
	s_editQueue->m_queueList->Clear();
	s_editQueue->SelectChoiceList(s_editQueue->m_unitList);

	return err;
}

AUI_ERRCODE EditQueue::Hide()
{
	AUI_ERRCODE err = AUI_ERRCODE_OK;
	if (s_editQueue)
	{
		if (s_editQueue->m_multiCities.GetCount() > 0)
		{
			PointerList<EditQueueCityInfo>::Walker walk(&s_editQueue->m_multiCities);

			while (walk.IsValid()) {
				delete walk.Remove();
			}
			s_editQueue->m_multiCities.DeleteAll();
		}

		if (s_editQueue->m_attachedToWindow) {
			s_editQueue->m_attachedToWindow->RemoveDockedWindow(s_editQueue->m_window);
		}
		err = g_c3ui->RemoveWindow(s_editQueue->m_window->Id());
	}
	return err;
}

AUI_ERRCODE EditQueue::Cleanup()
{
	if (s_editQueue) {
		s_editQueue->Hide();
	}

	delete s_editQueue;
	s_editQueue = NULL;
	return AUI_ERRCODE_OK;
}

void EditQueue::SelectCity(const Unit & city)
{
	if (!s_editQueue || !s_editQueue->IsShown || EditingCity(city)) {
		return;
	}
	SetCity(city);
}

void EditQueue::UpdateCity(const Unit & city)
{
	if (!s_editQueue || !s_editQueue->IsShown() || !EditingCity(city)) {
		return;
	}
	s_editQueue->RushBuy(false);
}

bool EditQueue::IsShown()
{
	if (!s_editQueue || !s_editQueue->m_window) {
		return false;
	}

	return g_c3ui->GetWindow(s_editQueue->m_window->Id()) != NULL;
}

static void setIntColumn(ctp2_Static * box, sint32 column, sint32 value)
{
	ctp2_Static * field = (ctp2_Static *)box->GetChildByIndex(column);
	if (value < 0) {
		field->SetText("-");
	} else {
		char buf[20];
		sprintf(buf, "%d", value);
		field->SetText(buf);
	}
}

sint32 EditQueue::CompareUnitItems(ctp2_ListItem * item1, ctp2_ListItem * item2, sint32 column)
{
	EditItemInfo * info1 = (EditItemInfo *)item1->GetUserData();
	EditItemInfo * info2 = (EditItemInfo *)item2->GetUserData();

	if (!info1 || !info2) {
		return 0;
	}

	const UnitRecord * record1 = g_theUnitDB->Get(info1->m_type);
	const UnitRecord * record2 = g_theUnitDB->Get(info2->m_type);

	Assert(record1 && record2);
	if (!record1 || !record2) {
		return 0;
	}

	switch (column)
	{
		case 0:
			return stricmp(record1->GetNameText(), record2->GetNameText());
		case 1:
			return static_cast<sint32>(record1->GetAttack() - record2->GetAttack());
		case 2:
			return static_cast<sint32>(record1->GetDefense() - record2->GetDefense());
		case 3:
			return static_cast<sint32>(record1->GetArmor() - record2->GetArmor());
		case 4:
			return record1->GetZBRangeAttack() - record2->GetZBRangeAttack();
		case 5:
			return record1->GetFirepower() - record2->GetFirepower();
		case 6:
			return static_cast<sint32>(record1->GetMaxMovePoints() - record2->GetMaxMovePoints());
		case 7:
			return record1->GetShieldCost() - record2->GetShieldCost();
	}
	return 0;
}

sint32 EditQueue::CompareBuildingWonderItems(ctp2_ListItem * item1, ctp2_ListItem * item2, sint32 column)
{
	EditItemInfo * info1 = (EditItemInfo *)item1->GetUserData();
	EditItemInfo * info2 = (EditItemInfo *)item2->GetUserData();

	if (!info1 || !info2) {
		return 0;
	}

	switch (column)
	{
		case 0:
			return stricmp(info1->GetName(), info2->GetName());
		case 1:
			return info1->GetCost() - info2->GetCost();
	}
	return 0;
}

void EditQueue::AddChoiceItem(const MBCHAR * text, EditItemInfo * userData, sint32 time, ctp2_ListBox * choiceList)
{
	bool isUnitList = (choiceList == m_unitList);
	ctp2_ListItem * item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot(
			isUnitList ? "eq_UnitItem" : "eq_BuildingWonderItem");

	Assert(item);
	if (item)
	{
		ctp2_Static * box = (ctp2_Static *)item->GetChildByIndex(0);
		if (box)
		{
			ctp2_Static * label = (ctp2_Static *)box->GetChildByIndex(0);
			if (label) {
				label->SetText(text);
			}

			ctp2_Static * timeLabel = (ctp2_Static *)box->GetChildByIndex(isUnitList ? 7 : 1);
			if (timeLabel) {
				if (time >= 0 && time < 0x7fffffff)
				{
					MBCHAR buf[20];
					sprintf(buf, "%d", time);
					timeLabel->SetText(buf);
				} else {
					timeLabel->SetText("");
				}
			}

			if (userData->m_category == k_GAME_OBJ_TYPE_UNIT && isUnitList)
			{
				item->SetCompareCallback(EditQueue::CompareUnitItems);
				const UnitRecord * record = g_theUnitDB->Get(userData->m_type);
				Assert(record);
				if(record)
				{
					bool isTrader = record->GetIsTrader();
					setIntColumn(box, 1, isTrader ? -1 : (sint32)record->GetAttack());
					setIntColumn(box, 2, isTrader ? -1 : (sint32)record->GetDefense());
					setIntColumn(box, 3, isTrader ? -1 : (sint32)record->GetArmor());
					setIntColumn(box, 4, isTrader ? -1 : (sint32)record->GetZBRangeAttack());
					setIntColumn(box, 5, isTrader ? -1 : (sint32)record->GetFirepower());
					setIntColumn(box, 6, (sint32)ceil(record->GetMaxMovePoints() / 100));
				}
			} else {
				item->SetCompareCallback(EditQueue::CompareBuildingWonderItems);
			}

			item->SetUserData(userData);
			choiceList->AddItem(item);
		}
	}
    else
    {
        delete userData;
    }
}

void EditQueue::ClearChoiceList(ctp2_ListBox * choiceList)
{
	for(sint32 i = 0; i < choiceList->NumItems(); i++) {
		ctp2_ListItem * item = (ctp2_ListItem *)choiceList->GetItemByIndex(i);
		if (item) {
			EditItemInfo * info = (EditItemInfo *) item->GetUserData();
			delete info;
			item->SetUserData(NULL);
		}
	}

	choiceList->Clear();
}

void EditQueue::ClearChoiceLists()
{
	ClearChoiceList(m_unitList);
	ClearChoiceList(m_buildingList);
	ClearChoiceList(m_wonderList);
	ClearChoiceList(m_queueContents);
}

ctp2_ListBox *EditQueue::GetVisibleItemList()
{
	if (m_unitList && !m_unitList->IsHidden()) {
		return m_unitList;
	} else if (m_buildingList && !m_buildingList->IsHidden()) {
		return m_buildingList;
	} else if (m_wonderList && !m_wonderList->IsHidden()) {
		return m_wonderList;
	} else if (m_queueContents && !m_queueContents->IsHidden()) {
		return m_queueContents;
	}

	return NULL;
}

void EditQueue::Update()
{
	m_updating = true;

	UpdateChoiceLists();
	UpdateQueueList();
	UpdateCityLists();
	UpdateFileLists();
	UpdateButtons();

	m_updating = false;
}

void EditQueue::UpdateChoiceLists()
{
	Assert(m_unitList && m_buildingList && m_wonderList);
	if (m_unitList && m_buildingList && m_wonderList)
	{
		m_unitList->BuildListStart();
		m_buildingList->BuildListStart();
		m_wonderList->BuildListStart();

		ClearChoiceLists();

		CityData * cityData = m_city.IsValid() ? m_city.GetCityData() : NULL;
		for (sint32 unitIndex = 0; unitIndex < g_theUnitDB->NumRecords(); unitIndex++)
		{
			if (cityData && !cityData->CanBuildUnit(unitIndex)) {
				continue;
			}

			if (m_mode == EDIT_QUEUE_MODE_MULTI)
			{
				PointerList<EditQueueCityInfo>::Walker walk(&m_multiCities);
				bool include = false;
				while(walk.IsValid())
				{
					if(walk.GetObj()->m_city.GetCityData()->CanBuildUnit(unitIndex))
					{
						include = true;
						break;
					}
					walk.Next();
				}
				if (!include) {
					continue;
				}
			}

			const UnitRecord * record = g_theUnitDB->Get(unitIndex);
			MBCHAR buf[k_MAX_NAME_LEN];
			sprintf(buf, "%s", record->GetNameText());

			if (m_mode == EDIT_QUEUE_MODE_MULTI) {
				AddChoiceItem(record->GetNameText(),new EditItemInfo(k_GAME_OBJ_TYPE_UNIT, unitIndex),-1, m_unitList);
			}
			else if (!cityData)
			{
				if (record->GetHasPopAndCanBuild() || record->GetCantBuild()) {
					continue;
				}
				AddChoiceItem(record->GetNameText(),new EditItemInfo(k_GAME_OBJ_TYPE_UNIT, unitIndex),-1, m_unitList);
			}
			else if (cityData->CanBuildUnit(unitIndex))
			{
				AddChoiceItem(record->GetNameText(),
						new EditItemInfo(k_GAME_OBJ_TYPE_UNIT, unitIndex),
						cityData->HowMuchLonger(record->GetShieldCost()),
						m_unitList);
			}
		}

		for (sint32 buildingIndex = 0; buildingIndex < g_theBuildingDB->NumRecords(); buildingIndex++)
		{
			if (m_mode == EDIT_QUEUE_MODE_MULTI)
			{
				bool already = false;
				PointerList<EditItemInfo>::Walker customWalk(&m_customBuildList);
				while (customWalk.IsValid())
				{
					if (customWalk.GetObj()->m_category == k_GAME_OBJ_TYPE_IMPROVEMENT &&
					    customWalk.GetObj()->m_type == buildingIndex)
					{
						already = true;
						break;
					}
					customWalk.Next();
				}
				if (already) {
					continue;
				}

				bool include = false;
				PointerList<EditQueueCityInfo>::Walker cityWalk(&m_multiCities);
				while (cityWalk.IsValid())
				{
					if (cityWalk.GetObj()->m_city.GetCityData()->CanBuildBuilding(buildingIndex))
					{
						include = true;
						break;
					}
					cityWalk.Next();
				}
				if (!include) {
					continue;
				}
				AddChoiceItem(g_theBuildingDB->Get(buildingIndex)->GetNameText(),
						new EditItemInfo(k_GAME_OBJ_TYPE_IMPROVEMENT, buildingIndex),
						-1,
						m_buildingList);
			}
			else if (!cityData)
			{
				if (IsItemInQueueList(k_GAME_OBJ_TYPE_IMPROVEMENT, buildingIndex)) {
					continue;
				}
				AddChoiceItem(g_theBuildingDB->Get(buildingIndex)->GetNameText(),
						new EditItemInfo(k_GAME_OBJ_TYPE_IMPROVEMENT, buildingIndex),
						-1,
						m_buildingList);
			}
			else if (cityData->CanBuildBuilding(buildingIndex) &&
					!cityData->GetBuildQueue()->IsItemInQueue(k_GAME_OBJ_TYPE_IMPROVEMENT, buildingIndex))
			{
				sint32 production = buildingutil_Get(buildingIndex, g_selected_item->GetVisiblePlayer())
						->GetProductionCost();
				AddChoiceItem(g_theBuildingDB->Get(buildingIndex)->GetNameText(),
						new EditItemInfo(k_GAME_OBJ_TYPE_IMPROVEMENT, buildingIndex),
						cityData->HowMuchLonger(production),
						m_buildingList);
			}
		}

		if (g_player[g_selected_item->GetVisiblePlayer()]->CanBuildCapitalization()
				|| (!cityData && m_mode == EDIT_QUEUE_MODE_CUSTOM))
		{
			if (cityData || m_mode == EDIT_QUEUE_MODE_MULTI || !IsItemInQueueList(k_GAME_OBJ_TYPE_CAPITALIZATION, 0))
			{
				AddChoiceItem(g_theStringDB->GetNameStr("CAPITALIZATION"),
						new EditItemInfo(k_GAME_OBJ_TYPE_CAPITALIZATION, 0),
						-1,
						m_buildingList);
			}
		}

		if (g_player[g_selected_item->GetVisiblePlayer()]->CanBuildInfrastructure()
				|| (!cityData && m_mode == EDIT_QUEUE_MODE_CUSTOM))
		{
			if (cityData || m_mode == EDIT_QUEUE_MODE_MULTI || !IsItemInQueueList(k_GAME_OBJ_TYPE_INFRASTRUCTURE, 0))
			{
				AddChoiceItem(g_theStringDB->GetNameStr("INFRASTRUCTURE"),
						new EditItemInfo(k_GAME_OBJ_TYPE_INFRASTRUCTURE, 0),
						-1,
						m_buildingList);
			}
		}

		for (sint32 wonderIndex = 0; wonderIndex < g_theWonderDB->NumRecords(); wonderIndex++) {
			if (m_mode == EDIT_QUEUE_MODE_MULTI)
			{
				bool already = false;
				PointerList<EditItemInfo>::Walker customWalk(&m_customBuildList);
				while(customWalk.IsValid())
				{
					if (customWalk.GetObj()->m_category == k_GAME_OBJ_TYPE_WONDER &&
							customWalk.GetObj()->m_type == wonderIndex)
					{
						already = true;
						break;
					}
					customWalk.Next();
				}
				if (already) {
					continue;
				}

				bool include = false;
				PointerList<EditQueueCityInfo>::Walker cityWalk(&m_multiCities);
				while (cityWalk.IsValid())
				{
					if (cityWalk.GetObj()->m_city.GetCityData()->CanBuildWonder(wonderIndex))
					{
						include = true;
						break;
					}
					cityWalk.Next();
				}
				if (!include) {
					continue;
				}
				AddChoiceItem(g_theWonderDB->Get(wonderIndex)->GetNameText(),
						new EditItemInfo(k_GAME_OBJ_TYPE_WONDER, wonderIndex),
						-1,
						m_wonderList);
			}
			else if (!cityData)
			{
				if (!IsItemInQueueList(k_GAME_OBJ_TYPE_WONDER, wonderIndex))
				{
					AddChoiceItem(g_theWonderDB->Get(wonderIndex)->GetNameText(),
							new EditItemInfo(k_GAME_OBJ_TYPE_WONDER, wonderIndex),
							-1,
							m_wonderList);
				}
			}
			else if (cityData->CanBuildWonder(wonderIndex) &&
					!cityData->GetBuildQueue()->IsItemInQueue(k_GAME_OBJ_TYPE_WONDER, wonderIndex))
			{
				sint32 production = wonderutil_Get(wonderIndex, g_selected_item->GetVisiblePlayer())
						->GetProductionCost();
				AddChoiceItem(g_theWonderDB->Get(wonderIndex)->GetNameText(),
						new EditItemInfo(k_GAME_OBJ_TYPE_WONDER, wonderIndex),
						cityData->HowMuchLonger(production),
						m_wonderList);
			}
		}

		m_unitList->BuildListEnd();
		m_buildingList->BuildListEnd();
		m_wonderList->BuildListEnd();
	}
}

void EditQueue::UpdateFileLists()
{
	m_queueFileNames.DeleteAll();

	if (!c3files_getfilelist(C3SAVEDIR_QUEUES, NULL, &m_queueFileNames)) {
		return;
	}

	m_queueFileList->BuildListStart();
	m_queueFileList->Clear();
	PointerList<MBCHAR>::Walker walk(&m_queueFileNames);
	while (walk.IsValid())
	{
		if (walk.GetObj()[0] == '.')
		{
			walk.Next();
			continue;
		}

		ctp2_ListItem * item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("eq_LoadListItem");
		Assert(item);
		if (!item) {
			break;
		}

		ctp2_Static * label = (ctp2_Static *)item->GetChildByIndex(0);
		Assert(label);
		if (!label) {
			break;
		}
		label->SetText(walk.GetObj());
		item->SetUserData((void *)walk.GetObj());
		m_queueFileList->AddItem(item);

		walk.Next();
	}
	m_queueFileList->BuildListEnd();

	ClearChoiceList(m_queueContents);
	m_queueName->SetText("");
	ShowSelectedInfo();
}

void EditQueue::UpdateQueueList()
{
	if (m_city.IsValid())
	{
		CityWindow::NotifyBuildChange(m_city);

		if (m_queueList) {
			CityWindow::PopulateQueueList(m_city, m_queueList, "eq_QueueListItem");
		}
	}
	else
	{
		Assert(m_mode == EDIT_QUEUE_MODE_CUSTOM || m_mode == EDIT_QUEUE_MODE_MULTI);
		if (m_mode == EDIT_QUEUE_MODE_MULTI)
		{
			PointerList<EditQueueCityInfo>::Walker walk(&m_multiCities);
			while (walk.IsValid())
			{
				CityWindow::NotifyBuildChange(walk.GetObj()->m_city);
				walk.Next();
			}
		}

		Assert(m_queueList);
		if( !m_queueList) {
			return;
		}

		m_queueList->BuildListStart();
		m_queueList->Clear();

		PointerList<EditItemInfo>::Walker walk(&m_customBuildList);
		while (walk.IsValid())
		{
			ctp2_ListItem * item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("eq_QueueListItem");
			Assert(item);
			if (item)
			{
				ctp2_Static * box = (ctp2_Static *)item->GetChildByIndex(0);
				if (box)
				{
					ctp2_Static * label = (ctp2_Static *)box->GetChildByIndex(0);
					EditItemInfo * itemInfo = walk.GetObj();
					label->SetText(itemInfo->GetName());

					ctp2_Static *time = (ctp2_Static *)box->GetChildByIndex(1);
					time->SetText("");

					item->SetUserData((void *)itemInfo);
				}
				m_queueList->AddItem(item);
			}
			walk.Next();
		}
		m_queueList->BuildListEnd();
	}
}

void EditQueue::UpdateCityLists()
{
	Player * player = g_player[g_selected_item->GetVisiblePlayer()];
	Assert(player);
	if (!player) {
		return;
	}

	if (m_cityDropDown)
	{
		m_cityDropDown->BuildListStart();
		m_cityDropDown->Clear();

		sint32 viewingIndex = -1;
		for(sint32 cityIndex = 0; cityIndex < player->m_all_cities->Num(); cityIndex++)
		{
			ctp2_ListItem * item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("cw_CityItem");
			Assert(item);
			if (!item) {
				break;
			}

			ctp2_Static * label = (ctp2_Static *)item->GetChildByIndex(0);
			label->SetText(player->m_all_cities->Access(cityIndex).GetName());
			item->SetUserData((void *)player->m_all_cities->Access(cityIndex).m_id);
			m_cityDropDown->AddItem(item);

			if (m_city.IsValid() && player->m_all_cities->Access(cityIndex) == m_city) {
				viewingIndex = cityIndex;
			}
		}
		m_cityDropDown->BuildListEnd();

		if (viewingIndex >= 0) {
			m_cityDropDown->SetSelectedItem(viewingIndex);
		}
	}

	if (m_multiCities.GetCount() > 0 && m_multiCityList)
	{
		m_multiCityList->Clear();
		m_multiCityList->BuildListStart();

		PointerList<EditQueueCityInfo>::Walker cityWalk(&m_multiCities);
		while (cityWalk.IsValid())
		{
			ctp2_ListItem * item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("eq_MultiCityListItem");
			Assert(item);
			if (!item) {
				break;
			}

			ctp2_Static * label = (ctp2_Static *)item->GetChildByIndex(0);
			label->SetText(cityWalk.GetObj()->m_city.GetCityData()->GetName());
			item->SetUserData((void *)cityWalk.GetObj()->m_city.m_id);

			m_multiCityList->AddItem(item);
			cityWalk.Next();
		}
		m_multiCityList->BuildListEnd();
	}
}

//----------------------------------------------------------------------------
//
// Name       : EditQueue::UpdateButtons
//
// Description: Updates all the buttons of the build manager including
//              the rush buy button and the cost display.
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
void EditQueue::UpdateButtons()
{
	CityData * cityData = m_city.IsValid() ? m_city.GetCityData() : NULL;
	ctp2_ListBox * visibleList = GetVisibleItemList();
	bool alreadyBoughtFront = cityData && cityData->AlreadyBoughtFront();

	if (!m_queueList->GetSelectedItem())
	{
		m_upButton->Enable(false);
		m_downButton->Enable(false);
		m_removeButton->Enable(false);
		m_rushBuyButton->Enable(false);
		m_rushBuyCost->SetText("---");
	}
	else
	{
		m_removeButton->Enable(m_queueList->GetSelectedItemIndex() != 0 || !alreadyBoughtFront);
		if (m_queueList->GetSelectedItemIndex() == 0 ||
			(m_queueList->GetSelectedItemIndex() == 1 && alreadyBoughtFront))
		{
			m_upButton->Enable(false);
		} else {
			m_upButton->Enable(true);
		}

		// Added by Martin Gühmann to disable the rush-buy button and rush
		// buy costs if the first item is capitalization or infrastructure
		if (cityData && (m_queueList->GetSelectedItemIndex() == 0))
		{
			sint32 const cost = cityData->GetOvertimeCost();
			if ((cost <= 0)                                  // switched with overproduction
					|| cityData->AlreadyBoughtFront()        // already rush-buying
					|| cityData->IsBuildingCapitalization()  // building capitalisation
					|| cityData->IsBuildingInfrastructure()) // building infrastructure
			{
				m_rushBuyCost->SetText("---");
				m_rushBuyButton->Enable(false);
			}
			else
			{
				char buf[20];
				sprintf(buf, "%d", cost);
				m_rushBuyCost->SetText(buf);

				sint32 const visiblePlayer = g_selected_item->GetVisiblePlayer();
				m_rushBuyButton->Enable((visiblePlayer == g_selected_item->GetCurPlayer()) // my turn
						&& (g_player[visiblePlayer]->GetGold() >= cost));                         // enough money
			}
		}
		else
		{
			m_rushBuyButton->Enable(false);
			m_rushBuyCost->SetText("---");
		}

		if (m_queueList->GetSelectedItemIndex() == m_queueList->NumItems() - 1
				|| (m_queueList->GetSelectedItemIndex() == 0 && alreadyBoughtFront))
		{
			m_downButton->Enable(false);
		} else {
			m_downButton->Enable(true);
		}
	}

	m_addButton->Enable(visibleList && visibleList->GetSelectedItem());
	m_insertButton->Enable(visibleList && visibleList->GetSelectedItem());
	m_clearButton->Enable(m_queueList->NumItems() > 0 && !(m_queueList->NumItems() == 1 && alreadyBoughtFront));
	m_saveQueueButton->Enable(m_queueList->NumItems() > 0);
}

bool EditQueue::EditingCity(const Unit & city)
{
	if (!s_editQueue || s_editQueue->m_mode == EDIT_QUEUE_MODE_CUSTOM) {
		return false;
	}

	if (s_editQueue->m_city == city) {
		return true;
	}
	else
	{
		PointerList<EditQueueCityInfo>::Walker cityWalk(&s_editQueue->m_multiCities);
		while (cityWalk.IsValid())
		{
			if (cityWalk.GetObj()->m_city == city) {
				return true;
			}
			cityWalk.Next();
		}
	}
	return false;
}

void EditQueue::SetMode(EDIT_QUEUE_MODE mode)
{
	if (!s_editQueue){
		return;
	}

	s_editQueue->m_oldMode = s_editQueue->m_mode;
	Unit oldCity(s_editQueue->m_oldCity);
	s_editQueue->m_oldCity = s_editQueue->m_city;

	s_editQueue->m_mode = mode;

	switch(s_editQueue->m_mode)
	{
		case EDIT_QUEUE_MODE_SINGLE:
			if (s_editQueue->m_itemsBox->IsHidden()) {
				s_editQueue->ExitLoadMode();
			}
			// Can use suggest editing single queue.
			s_editQueue->m_suggestButton->Show();
			s_editQueue->m_customModeButtons->Hide();
			s_editQueue->m_normalModeButtons->Show();
			s_editQueue->m_modeLabel->SetText(g_theStringDB->GetNameStr("str_ldl_EditQueueBuildableItems"));
			s_editQueue->m_queueLabel->SetText(g_theStringDB->GetNameStr("str_ldl_CurrentQueue"));
			s_editQueue->m_multiCityList->Hide();
			s_editQueue->m_singleCityChooser->Show();
			s_editQueue->m_multiButtonGroup->Hide();
			s_editQueue->m_gotoCityButton->Enable(true);
			if (s_editQueue->m_oldMode == EDIT_QUEUE_MODE_CUSTOM && oldCity.IsValid()) {
				s_editQueue->m_city = oldCity;
			}
			break;

		case EDIT_QUEUE_MODE_MULTI:
			if (s_editQueue->m_itemsBox->IsHidden()) {
				s_editQueue->ExitLoadMode();
			}
			// Can't use suggest editing multiple queues.
			s_editQueue->m_suggestButton->Hide();
			s_editQueue->m_customModeButtons->Hide();
			s_editQueue->m_normalModeButtons->Show();
			s_editQueue->m_modeLabel->SetText(g_theStringDB->GetNameStr("str_ldl_EditQueueBuildableItems"));
			s_editQueue->m_queueLabel->SetText(g_theStringDB->GetNameStr("str_ldl_CurrentQueue"));
			s_editQueue->m_multiCityList->Show();
			s_editQueue->m_singleCityChooser->Hide();
			s_editQueue->m_multiButtonGroup->Show();
			s_editQueue->m_gotoCityButton->Enable(false);
			s_editQueue->m_city = 0;
			s_editQueue->m_customBuildList.DeleteAll();
			break;

		case EDIT_QUEUE_MODE_CUSTOM:
			// Can't use suggest creating custom queue for multiple cities.
			s_editQueue->m_suggestButton->Hide();
			s_editQueue->m_customModeButtons->Show();
			s_editQueue->m_normalModeButtons->Hide();
			s_editQueue->m_modeLabel->SetText(g_theStringDB->GetNameStr("str_ldl_EditQueueAllItems"));
			s_editQueue->m_queueLabel->SetText(g_theStringDB->GetNameStr("str_ldl_EditQueueCustomQueue"));
			s_editQueue->m_multiCityList->Hide();
			s_editQueue->m_singleCityChooser->Hide();
			s_editQueue->m_gotoCityButton->Enable(false);
			s_editQueue->m_city = 0;
			s_editQueue->m_customBuildList.DeleteAll();
			s_editQueue->m_multiButtonGroup->Hide();
			s_editQueue->m_saveQueueButton->Enable(false);
			break;
	}
	s_editQueue->m_itemsBox->ShouldDraw();
	s_editQueue->m_queueBox->ShouldDraw();

	s_editQueue->Update();
}

void EditQueue::SetCity(const Unit & city)
{
	if (!s_editQueue) {
		return;
	}

	s_editQueue->m_city = city;
	SetMode(EDIT_QUEUE_MODE_SINGLE);
	s_editQueue->Update();
	if (s_editQueue->m_queueList->NumItems() > 0) {
		s_editQueue->m_queueList->SelectItem(0);
	} else {
		ShowSelectedInfo();
	}

	Unit currentCity;
	if (city.IsValid() && (!g_selected_item->GetSelectedCity(currentCity) || currentCity != city)) {
		g_selected_item->SetSelectCity(city);
	}
}

void EditQueue::SetMultiCities(const UnitDynamicArray & cities)
{
	SetMode(EDIT_QUEUE_MODE_MULTI);
	s_editQueue->m_multiCities.DeleteAll();
	for(sint32 cityIndex = 0; cityIndex < cities.Num(); cityIndex++) {
		s_editQueue->m_multiCities.AddTail(new EditQueueCityInfo(cities[cityIndex]));
	}
	s_editQueue->Update();
}

struct CapitolConfirmData {
	EditItemInfo * info;
	bool insert;
};

static void ConfirmRebuildCapitol(bool confirm, void * data)
{
	if (confirm)
	{
		CapitolConfirmData * confirmData = (CapitolConfirmData *)data;
		Assert(confirmData && confirmData->info);
		Assert(s_editQueue);
		if (!s_editQueue || !confirmData || !confirmData->info) {
			return;
		}

		s_editQueue->InsertInQueue(confirmData->info, confirmData->insert, true, false);
	}
}

struct SwitchProductionConfirmData {
	EditItemInfo * info;
	bool insert;
};

static void ConfirmSwitchProduction(bool confirm, void * data)
{
	if (confirm)
	{
		SwitchProductionConfirmData *confirmData = (SwitchProductionConfirmData *)data;
		Assert(confirmData && confirmData->info);
		Assert(s_editQueue);
		if (!s_editQueue || !confirmData || !confirmData->info) {
			return;
		}
		s_editQueue->InsertInQueue(confirmData->info, confirmData->insert, true, true);
	}
}

//----------------------------------------------------------------------------
//
// Name       : EditQueue::InsertInQueue
//
// Description: Final stage for adding and inserting items into a queue.
//
// Parameters : EditItemInfo *info - The info of the item being added.
//									 Contains the category and type of item.
//				bool insert - Whether to insert the item or just add
//							  to the end.
//				bool confirmed - Message query to the player whether they want
//								 to rebuild their capital or not.
//				bool confirmedSwitch - Message query to the player whether
//									   they want to incur a production switch
//								       penalty or not.
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : "Suggest" button uses "Add", so also goes through this.
//
//----------------------------------------------------------------------------
void EditQueue::InsertInQueue(EditItemInfo * info, bool insert, bool confirmed, bool confirmedSwitch)
{
	Assert(info);
	if (!info || !m_queueList) {
		return;
	}

	if (!confirmed
			&& info->m_category == k_GAME_OBJ_TYPE_IMPROVEMENT
			&& buildingutil_Get(info->m_type, g_selected_item->GetVisiblePlayer())->GetCapitol()
			&& g_player[g_selected_item->GetVisiblePlayer()]->m_capitol
			&& g_player[g_selected_item->GetVisiblePlayer()]->m_capitol->IsValid()
			&& m_city.IsValid())
	{
		static CapitolConfirmData data;
		data.info = info;
		data.insert = insert;
		MessageBoxDialog::Query(g_theStringDB->GetNameStr("str_code_QueryRebuildCapitol"),"QueryRebuildCapitol",
				ConfirmRebuildCapitol, &data);
		return;
	}

	const sint32 TOP = 0;
	const sint32 BOTTOM = -1;
	sint32 insertIndex = (insert && m_queueList->NumItems() > 0) ? TOP : BOTTOM;

	CityData * cityData = m_city.IsValid() ? m_city.GetCityData() : NULL;
	if (cityData) // Editing a single queue?
	{
		if (insertIndex == TOP && cityData->AlreadyBoughtFront())
		{
			insertIndex++;
			if (insertIndex > m_queueList->NumItems()) {
				insertIndex = BOTTOM;
			}
		}

		if (insertIndex == TOP) // Queues must have at least one item selected to "insert", so head is available.
		{
			if (info->m_category != cityData->GetBuildQueue()->GetHead()->m_category
					&& cityData->GetStoredCityProduction() > 0)
			{
				if (!confirmedSwitch)
				{
					static SwitchProductionConfirmData data;
					data.info = info;
					data.insert = insert;

					MBCHAR buf[k_MAX_NAME_LEN];
					sint32 p = g_theConstDB->Get(0)->GetChangeCurrentlyBuildingItemPenalty();
					sprintf(buf, g_theStringDB->GetNameStr("str_code_QuerySwitchProduction"), p);

					MessageBoxDialog::Query(buf, "QuerySwitchProduction", ConfirmSwitchProduction, &data);
					return;
				}
				cityData->CheckSwitchProductionPenalty(info->m_category);
			}
		}
		else if (insertIndex == BOTTOM && m_queueList->NumItems() == 0) // Adding, no items in queue, so adding to top.
		{ // This is also for when the "suggest" button is used and the queue is empty.
			if (info->m_category != cityData->GetBuildCategoryAtBeginTurn()
					&& cityData->GetStoredCityProduction() > 0
					&& cityData->GetBuildCategoryAtBeginTurn() != -5) // -5 is the no penalty type.
			{
				if (!confirmedSwitch)
				{
					static SwitchProductionConfirmData data;
					data.info = info;
					data.insert = insert;

					MBCHAR buf[k_MAX_NAME_LEN];
					sint32 p = g_theConstDB->Get(0)->GetChangeCurrentlyBuildingItemPenalty();
					sprintf(buf, g_theStringDB->GetNameStr("str_code_QuerySwitchProduction"), p);

					MessageBoxDialog::Query(buf, "QuerySwitchProduction", ConfirmSwitchProduction, &data);
					return;
				}
				cityData->CheckSwitchProductionPenalty(info->m_category);
			}
		}

		// Item is actually replaced here:
		cityData->InsertBuildItem(insertIndex, info->m_category, info->m_type);
	}
	else // Editing a custom queue or multiple city queues?
	{
		Assert(m_mode == EDIT_QUEUE_MODE_CUSTOM || m_mode == EDIT_QUEUE_MODE_MULTI);
		EditItemInfo *copiedInfo = new EditItemInfo(info->m_category, info->m_type);
		if (insertIndex == BOTTOM) { // Adding to the end of the queue or to empty queue.
			m_customBuildList.AddTail(copiedInfo);
		}
		else
		{
			sint32 index = 0;
			PointerList<EditItemInfo>::Walker customWalk(&m_customBuildList);
			while (customWalk.IsValid())
			{
				if (index == insertIndex)
				{
					customWalk.Insert(copiedInfo);
					break;
				}
				customWalk.Next();
				index++;
			}
			if (!customWalk.IsValid()) {
				m_customBuildList.AddTail(copiedInfo);
			}
		}
	}

	ctp2_ListBox * checkRemoveList = NULL;
	if(info->m_category == k_GAME_OBJ_TYPE_IMPROVEMENT
			|| info->m_category == k_GAME_OBJ_TYPE_CAPITALIZATION
			|| info->m_category == k_GAME_OBJ_TYPE_INFRASTRUCTURE)
	{
		checkRemoveList = m_buildingList;
	} else if (info->m_category == k_GAME_OBJ_TYPE_WONDER) {
		checkRemoveList = m_wonderList;
	}

	bool removed = false;
	if (checkRemoveList)
	{
		sint32 type = info->m_type;
		uint32 category = info->m_category;
		for (sint32 i = checkRemoveList->NumItems() - 1; i >= 0; i--)
		{
			ctp2_ListItem * item     = (ctp2_ListItem *)checkRemoveList->GetItemByIndex(i);
			EditItemInfo  * itemInfo = (EditItemInfo *)item->GetUserData();

			if (category == itemInfo->m_category && type == itemInfo->m_type)
			{
				checkRemoveList->RemoveItem(item->Id());
				delete item;
				delete itemInfo;
				removed = true;
			}
		}
	}
	UpdateQueueList();
	if (removed) {
		m_queueList->SelectItem(insert ? insertIndex : m_queueList->NumItems() - 1);
	}
	UpdateButtons();
}

void EditQueue::Add(bool insert)
{
	ctp2_ListBox * visibleList = GetVisibleItemList();
	Assert(visibleList);
	if (!visibleList) {
		return;
	}

	ctp2_ListItem * item = reinterpret_cast<ctp2_ListItem *>(visibleList->GetSelectedItem());
	if (!item) {
		return;
	}

	EditItemInfo * info = reinterpret_cast<EditItemInfo *>(item->GetUserData());
	Assert(info);
	if (!info) {
		return;
	}

	InsertInQueue(info, insert);
}

void EditQueue::Suggest(bool insert)
{
	CityData * cityData = m_city.IsValid() ? m_city.GetCityData() : NULL;
	if (cityData)
	{
		MapAnalysis::GetMapAnalysis().RecalcCityRanks(cityData->GetOwner());

		sint32  category    = 0;
		sint32  type        = CTPRecord::INDEX_INVALID;
		Governor::GetGovernor(g_selected_item->GetVisiblePlayer()).ComputeDesiredUnits();
		Governor::GetGovernor(g_selected_item->GetVisiblePlayer()).ComputeNextBuildItem(cityData, category, type);

		EditItemInfo * info = new EditItemInfo(category, type);
		Assert(info);
		if (info) {
			InsertInQueue(info, insert);
		}
	}
}

static void ConfirmRemoveProduction(bool confirm, void * data)
{
	if (confirm)
	{
		Assert(s_editQueue);
		if(!s_editQueue) {
			return;
		}

		s_editQueue->Remove(true);
	}
}

void EditQueue::Remove(bool confirmedSwitch)
{
	if (!m_queueList) {
		return;
	}

	sint32 buildIndex = m_queueList->GetSelectedItemIndex();
	if (buildIndex >= 0)
	{
		CityData * cityData = m_city.IsValid() ? m_city.GetCityData() : NULL;
		if (cityData)
		{
			if (buildIndex == 0 // Removing the top item so 2nd item may move up if there is one.
				&& m_queueList->NumItems() >= 2)
			{
				BuildNode * node = cityData->GetBuildQueue()->GetNodeByIndex(1);// Get 2nd item.
				Assert(node);
				if (node->m_category != cityData->GetBuildQueue()->GetHead()->m_category
					&& cityData->GetStoredCityProduction() > 0)
				{
					if (!confirmedSwitch)
					{
						MBCHAR buf[k_MAX_NAME_LEN];
						sint32 p = g_theConstDB->Get(0)->GetChangeCurrentlyBuildingItemPenalty();
						sprintf(buf, g_theStringDB->GetNameStr("str_code_QuerySwitchProduction"), p);

						MessageBoxDialog::Query(buf, "QuerySwitchProduction", ConfirmRemoveProduction);
						return;
					}

					if (node) {
						cityData->CheckSwitchProductionPenalty(node->m_category);
					}
				}
			}

			cityData->GetBuildQueue()->RemoveNodeByIndex(buildIndex, CAUSE_REMOVE_BUILD_ITEM_MANUAL);
		}
		else
		{
			Assert(m_mode == EDIT_QUEUE_MODE_CUSTOM || m_mode == EDIT_QUEUE_MODE_MULTI);
			sint32 index = 0;
			PointerList<EditItemInfo>::Walker customWalk(&m_customBuildList);
			while (customWalk.IsValid())
			{
				if (index == buildIndex)
				{
					delete customWalk.Remove();
					break;
				}
				customWalk.Next();
				index++;
			}
		}
		Update();

		if (m_queueList->NumItems() > 0)
		{
			if (buildIndex >= m_queueList->NumItems()) {
				buildIndex = m_queueList->NumItems() - 1;
			}
			m_queueList->SelectItem(buildIndex);
		}
	}
}

static void ConfirmMoveUpProduction(bool confirm, void * data)
{
	if (confirm)
	{
		Assert(s_editQueue);
		if (!s_editQueue) {
			return;
		}

		s_editQueue->Up(true);
	}
}

void EditQueue::Up(bool confirmedSwitch)
{
	if (!m_queueList) {
		return;
	}

	sint32 selectedIndex = m_queueList->GetSelectedItemIndex();
	if (selectedIndex > 0)
	{
		CityData * cityData = m_city.IsValid() ? m_city.GetCityData() : NULL;
		if (cityData)
		{
			if (selectedIndex == 1)// If the 2nd item will be moved up to replace the top item.
			{
				BuildNode * node = cityData->GetBuildQueue()->GetNodeByIndex(1);// Get 2nd item.
				Assert(node);
				if (node->m_category != cityData->GetBuildQueue()->GetHead()->m_category
					&& cityData->GetStoredCityProduction() > 0)
				{
					if (!confirmedSwitch)
					{
						MBCHAR buf[k_MAX_NAME_LEN];
						sint32 p = g_theConstDB->Get(0)->GetChangeCurrentlyBuildingItemPenalty();
						sprintf(buf, g_theStringDB->GetNameStr("str_code_QuerySwitchProduction"), p);

						MessageBoxDialog::Query(buf, "QuerySwitchProduction", ConfirmMoveUpProduction);
						return;
					}

					if (node) {
						cityData->CheckSwitchProductionPenalty(node->m_category);
					}
				}
			}

			cityData->GetBuildQueue()->MoveNodeUp(selectedIndex);
		}
		else
		{
			Assert(m_mode == EDIT_QUEUE_MODE_CUSTOM || m_mode == EDIT_QUEUE_MODE_MULTI);
			sint32 index = 0;
			PointerList<EditItemInfo>::Walker customWalk(&m_customBuildList);
			while (customWalk.IsValid())
			{
				if (index == selectedIndex)
				{
					customWalk.MoveUp();
					break;
				}
				customWalk.Next();
				index++;
			}
		}

		sint32 topIndex, bottomIndex;
		m_queueList->GetDisplayRange(topIndex, bottomIndex);

		UpdateQueueList();
		UpdateButtons();
		m_queueList->SelectItem(selectedIndex - 1);

		m_queueList->GetVerticalRanger()->SetValue(0, topIndex);
		m_queueList->EnsureItemVisible(selectedIndex - 1);
	}
}

static void ConfirmMoveDownProduction(bool confirm, void * data)
{
	if (confirm)
	{
		Assert(s_editQueue);
		if (!s_editQueue) {
			return;
		}

		s_editQueue->Down(true);
	}
}

void EditQueue::Down(bool confirmedSwitch)
{
	if (!m_queueList) {
		return;
	}

	sint32 selectedIndex = m_queueList->GetSelectedItemIndex();
	if (selectedIndex >= 0 && selectedIndex < m_queueList->NumItems() - 1)
	{
		CityData * cityData = m_city.IsValid() ? m_city.GetCityData() : NULL;
		if (cityData)
		{
			if (selectedIndex == 0)// The top item will be moved down to get replaced by 2nd item.
			{
				BuildNode * node = cityData->GetBuildQueue()->GetNodeByIndex(1);// Get 2nd item.
				Assert(node);
				if (node->m_category != cityData->GetBuildQueue()->GetHead()->m_category
					&& cityData->GetStoredCityProduction() > 0)
				{
					if (!confirmedSwitch)
					{
						MBCHAR buf[k_MAX_NAME_LEN];
						sint32 p = g_theConstDB->Get(0)->GetChangeCurrentlyBuildingItemPenalty();
						sprintf(buf, g_theStringDB->GetNameStr("str_code_QuerySwitchProduction"), p);

						MessageBoxDialog::Query(buf, "QuerySwitchProduction", ConfirmMoveDownProduction);
						return;
					}

					if (node) {
						cityData->CheckSwitchProductionPenalty(node->m_category);
					}
				}
			}

			cityData->GetBuildQueue()->MoveNodeDown(selectedIndex);
		}
		else
		{
			Assert(m_mode == EDIT_QUEUE_MODE_CUSTOM || m_mode == EDIT_QUEUE_MODE_MULTI);
			sint32 index = 0;
			PointerList<EditItemInfo>::Walker customWalk(&m_customBuildList);
			while (customWalk.IsValid()) {
				if (index == selectedIndex) {
					customWalk.MoveDown();
					break;
				}
				customWalk.Next();
				index++;
			}
		}

		sint32 topIndex, bottomIndex;
		m_queueList->GetDisplayRange(topIndex, bottomIndex);

		UpdateQueueList();
		UpdateButtons();
		m_queueList->SelectItem(selectedIndex + 1);

		m_queueList->GetVerticalRanger()->SetValue(0, topIndex);
		m_queueList->EnsureItemVisible(selectedIndex + 1);
	}
}

void EditQueue::ToggleUnits(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	s_editQueue->SelectChoiceList(s_editQueue->m_unitList);
}

void EditQueue::ToggleBuildings(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	s_editQueue->SelectChoiceList(s_editQueue->m_buildingList);
}

void EditQueue::ToggleWonders(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	s_editQueue->SelectChoiceList(s_editQueue->m_wonderList);
}

void EditQueue::Library(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	if (s_editQueue->m_itemCategory < 0 || s_editQueue->m_itemType < 0) {
		return;
	}

	open_GreatLibrary();
	switch (s_editQueue->m_itemCategory)
	{
		case k_GAME_OBJ_TYPE_UNIT:
			g_greatLibrary->SetLibrary(s_editQueue->m_itemType, DATABASE_UNITS);
			break;
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
			g_greatLibrary->SetLibrary(s_editQueue->m_itemType, DATABASE_BUILDINGS);
			break;
		case k_GAME_OBJ_TYPE_WONDER:
			g_greatLibrary->SetLibrary(s_editQueue->m_itemType, DATABASE_WONDERS);
			break;
	}
}

void EditQueue::AddItem(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	s_editQueue->Add(false);
}

void EditQueue::InsertItem(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	s_editQueue->Add(true);
}

void EditQueue::SuggestItem(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	s_editQueue->Suggest(false);
}

void EditQueue::RemoveItem(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	s_editQueue->Remove();
}

void EditQueue::ItemUp(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	s_editQueue->Up();
}

void EditQueue::ItemDown(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	s_editQueue->Down();
}

void EditQueue::ListCallback(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT && action != AUI_LISTBOX_ACTION_SELECT) {
		return;
	}

	if (!s_editQueue) {
		return;
	}

	if (action == AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {
		s_editQueue->Add(false);
	}

	s_editQueue->FocusList(reinterpret_cast<ctp2_ListBox *>(control));
	ShowSelectedInfo();
	s_editQueue->UpdateButtons();
}

void EditQueue::ShowSelectedInfo()
{
	Assert(s_editQueue);
	if (!s_editQueue) {
		return;
	}

	SlicContext context;

	uint32 category = 0xffffffffu;
	sint32 type = -1;

	ctp2_ListItem * item = (ctp2_ListItem *) s_editQueue->m_queueList->GetSelectedItem();
	if (item)
	{
		CityData * cityData = s_editQueue->m_city.IsValid() ? s_editQueue->m_city.GetCityData() : NULL;
		if (cityData)
		{
			BuildNode *buildNode = cityData->GetBuildQueue()->GetNodeByIndex(
					s_editQueue->m_queueList->GetSelectedItemIndex());
			if (buildNode)
			{
				category = buildNode->m_category;
				type = buildNode->m_type;
			}
		}
		else
		{
			EditItemInfo * info = (EditItemInfo *) item->GetUserData();
			Assert(info);
			category = info->m_category;
			type = info->m_type;
		}
	}
	else
	{
		ctp2_ListBox * visibleList = s_editQueue->GetVisibleItemList();
		if (visibleList)
		{
			item = (ctp2_ListItem *) visibleList->GetSelectedItem();
			if (item)
			{
				EditItemInfo * info = (EditItemInfo *) item->GetUserData();
				Assert(info);
				category = info->m_category;
				type = info->m_type;
			}
		}
	}

	const IconRecord * icon = NULL;
	s_editQueue->m_itemCategory = -1;
	s_editQueue->m_itemType = -1;
	if (category >= 0 && type >= 0)
	{
		s_editQueue->m_itemCategory = category;
		s_editQueue->m_itemType = type;

		switch (category)
		{
			case k_GAME_OBJ_TYPE_UNIT:
				icon = g_theUnitDB->Get(type)->GetDefaultIcon();
				context.AddUnitRecord(type);
				break;
			case k_GAME_OBJ_TYPE_IMPROVEMENT:
				icon = g_theBuildingDB->Get(type)->GetDefaultIcon();
				context.AddBuilding(type);
				break;
			case k_GAME_OBJ_TYPE_WONDER:
				icon = g_theWonderDB->Get(type)->GetDefaultIcon();
				context.AddWonder(type);
				break;
			case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
			{
				sint32 index;
				g_theIconDB->GetNamedItem("ICON_IMPROVE_INFRASTRUCTURE", index);
				icon = g_theIconDB->Get(index);
				break;
			}
			case k_GAME_OBJ_TYPE_CAPITALIZATION:
			{
				sint32 index;
				g_theIconDB->GetNamedItem("ICON_IMPROVE_CAPITALIZATION", index);
				icon = g_theIconDB->Get(index);
				break;
			}
			default:
				Assert(false);
				break;
		}
	}
	SetItemDescription(icon, context, NULL, s_editQueue->m_itemDescription, s_editQueue->m_window,
			s_editQueue->m_itemImageButton);
	s_editQueue->m_libraryButton->Enable(s_editQueue->m_itemCategory != -1 && s_editQueue->m_itemType != -1);
}

void EditQueue::Close(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	Hide();
}

void EditQueue::EnterLoadMode()
{
	s_editQueue->m_listBeforeLoadSaveMode = s_editQueue->GetVisibleItemList();
	s_editQueue->m_listBeforeLoadSaveMode->Hide();
	s_editQueue->m_loadBox->Show();
	s_editQueue->m_itemsBox->Hide();
	DoFocusList(s_editQueue->m_listBeforeLoadSaveMode, false);
	DoFocusList(s_editQueue->m_queueList, false);
	ShowSelectedInfo();
	s_editQueue->m_deleteButton->Enable(s_editQueue->m_queueFileList->GetSelectedItemIndex() >= 0);
	s_editQueue->m_loadModeLoadButton->Enable(s_editQueue->m_queueContents->NumItems() > 0);
	s_editQueue->m_createCustomQueueButton->Enable(false);
}

void EditQueue::ExitLoadMode()
{
	DoFocusList(s_editQueue->m_queueList, false);
	s_editQueue->m_createCustomQueueButton->Enable(true);
	s_editQueue->m_loadBox->Hide();
	s_editQueue->m_itemsBox->Show();
	if (s_editQueue->m_mode == EDIT_QUEUE_MODE_CUSTOM) {
		s_editQueue->m_suggestButton->Hide();
	}
	s_editQueue->SelectChoiceList(s_editQueue->m_listBeforeLoadSaveMode);
}

void EditQueue::LoadModeCallback(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	Assert(s_editQueue);
	if (!s_editQueue) {
		return;
	}

	Assert(s_editQueue->m_itemsBox);
	Assert(s_editQueue->m_loadBox);
	if (s_editQueue->m_itemsBox && s_editQueue->m_loadBox)
	{
		if (s_editQueue->m_itemsBox->IsHidden()) {
			s_editQueue->ExitLoadMode();
		} else {
			s_editQueue->EnterLoadMode();
		}
	}
}

void EditQueue::CityDropDown(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_DROPDOWN_ACTION_SELECT) {
		return;
	}

	if (!s_editQueue || !s_editQueue->m_cityDropDown || s_editQueue->m_updating) {
		return;
	}

	if (s_editQueue->m_cityDropDown->GetSelectedItem() >= 0)
	{
		ctp2_ListItem * item = (ctp2_ListItem *)s_editQueue->m_cityDropDown->GetListBox()->GetItemByIndex(
				s_editQueue->m_cityDropDown->GetSelectedItem());
		Assert(item);
		if (item)
		{
			Unit city((uint32)item->GetUserData());
			if (!s_editQueue->EditingCity(city)) {
				SetCity(city);
			}
		}
	}
}

void EditQueue::PreviousCity(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	if(!s_editQueue || !s_editQueue->m_cityDropDown || s_editQueue->m_cityDropDown->GetListBox()->NumItems() < 2) {
		return;
	}

	sint32 index = s_editQueue->m_cityDropDown->GetSelectedItem();
	index--;
	if (index < 0) {
		index = s_editQueue->m_cityDropDown->GetListBox()->NumItems() - 1;
	}

	ctp2_ListItem * item = (ctp2_ListItem *)s_editQueue->m_cityDropDown->GetListBox()->GetItemByIndex(index);
	Assert(item);
	if (!item) {
		return;
	}

	SetCity((uint32)item->GetUserData());
}

void EditQueue::NextCity(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	if (!s_editQueue || !s_editQueue->m_cityDropDown || s_editQueue->m_cityDropDown->GetListBox()->NumItems() < 2) {
		return;
	}

	sint32 index = s_editQueue->m_cityDropDown->GetSelectedItem();
	index++;
	if (index >= s_editQueue->m_cityDropDown->GetListBox()->NumItems()) {
		index = 0;
	}

	ctp2_ListItem * item = (ctp2_ListItem *)s_editQueue->m_cityDropDown->GetListBox()->GetItemByIndex(index);
	Assert(item);
	if (!item) {
		return;
	}

	SetCity((uint32)item->GetUserData());
}

void EditQueue::CustomButton(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	if (!s_editQueue) {
		return;
	}

	switch (s_editQueue->m_mode)
	{
		case EDIT_QUEUE_MODE_SINGLE:
		case EDIT_QUEUE_MODE_MULTI:
			s_editQueue->m_oldMode = s_editQueue->m_mode;
			SetMode(EDIT_QUEUE_MODE_CUSTOM);
			break;
		case EDIT_QUEUE_MODE_CUSTOM:
			SetMode(s_editQueue->m_oldMode);
			break;
	}
}

void EditQueue::ClearMessageCallback(bool response, void * data)
{
	Assert(s_editQueue);
	if (!s_editQueue || !s_editQueue->m_city.IsValid()) {
		return;
	}

	if (response)
	{
		CityData * cityData = s_editQueue->m_city.GetCityData();
		if (cityData->AlreadyBoughtFront()) {
			cityData->GetBuildQueue()->ClearAllButHead();
		} else {
			cityData->GetBuildQueue()->Clear();
		}
		s_editQueue->Update();
		// In case already bought front, select bought front item
		DoFocusList(s_editQueue->m_queueList, true);
	}
}

void EditQueue::ClearButton(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	if (!s_editQueue) {
		return;
	}

	if (s_editQueue->m_city.IsValid())
	{
		MessageBoxDialog::Query(g_theStringDB->GetNameStr("str_ldl_ClearQueueQuery"),"QueryClearQueue",
				EditQueue::ClearMessageCallback);
	}
	else
	{
		s_editQueue->m_customBuildList.DeleteAll();
		s_editQueue->Update();
	}
}

void EditQueue::ConfirmOverwrite(bool response, void * data)
{
	if (response) {
		EditQueue::MultiActionButton(NULL, AUI_BUTTON_ACTION_EXECUTE, 0,
				(void *)EDIT_QUEUE_MULTI_ACTION_OVERWRITE_CONFIRMED);
	}
}

void EditQueue::MultiActionButton(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	Assert(s_editQueue);
	if (!s_editQueue) {
		return;
	}

	EDIT_QUEUE_MULTI_ACTION eqAction = (EDIT_QUEUE_MULTI_ACTION)(sint32)cookie;
	Assert(s_editQueue->m_mode == EDIT_QUEUE_MODE_MULTI);
	if (eqAction == EDIT_QUEUE_MULTI_ACTION_OVERWRITE)
	{
		MessageBoxDialog::Query(g_theStringDB->GetNameStr("str_ldl_EditQueueConfirmOverwrite"),"QueryConfirmOverwrite",
				EditQueue::ConfirmOverwrite);
		return;
	}

	PointerList<EditQueueCityInfo>::Walker cityWalk(&s_editQueue->m_multiCities);
	while (cityWalk.IsValid())
	{
		CityData * walkCityData = cityWalk.GetObj()->m_city.GetCityData();
		BuildQueue *buildQueue = walkCityData->GetBuildQueue();
		if (eqAction == EDIT_QUEUE_MULTI_ACTION_OVERWRITE_CONFIRMED) {
			buildQueue->Clear();
		}

		sint32 insertIndex = 0;
		switch (eqAction)
		{
			case EDIT_QUEUE_MULTI_ACTION_INSERT:
				insertIndex = 0;
				break;
			case EDIT_QUEUE_MULTI_ACTION_APPEND:
				insertIndex = buildQueue->GetLen();
				break;
			case EDIT_QUEUE_MULTI_ACTION_OVERWRITE_CONFIRMED:
				insertIndex = 0;
				break;
		}

		PointerList<EditItemInfo>::Walker customWalk(&s_editQueue->m_customBuildList);
		while(customWalk.IsValid())
		{
			switch(customWalk.GetObj()->m_category)
			{
				case k_GAME_OBJ_TYPE_UNIT:
					if (!walkCityData->CanBuildUnit(customWalk.GetObj()->m_type)) {
						continue;
					}
					break;
				case k_GAME_OBJ_TYPE_IMPROVEMENT:
					if (!walkCityData->CanBuildBuilding(customWalk.GetObj()->m_type)) {
						continue;
					}
					break;
				case k_GAME_OBJ_TYPE_WONDER:
					if (!walkCityData->CanBuildWonder(customWalk.GetObj()->m_type)) {
						continue;
					}
					break;
				case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
					if (!g_player[g_selected_item->GetVisiblePlayer()]->CanBuildInfrastructure()) {
						continue;
					}
					break;
				case k_GAME_OBJ_TYPE_CAPITALIZATION:
					if (!g_player[g_selected_item->GetVisiblePlayer()]->CanBuildCapitalization()) {
						continue;
					}
					break;
			}

			// Check first item only.
			if (insertIndex == 0)
			{
				if (customWalk.GetObj()->m_category != walkCityData->GetBuildCategoryAtBeginTurn()
						&& walkCityData->GetStoredCityProduction() > 0
						&& walkCityData->GetBuildCategoryAtBeginTurn() != -5) // -5 is the no penalty type.
				{
					walkCityData->CheckSwitchProductionPenalty(customWalk.GetObj()->m_category);
				}
			}

			walkCityData->InsertBuildItem(insertIndex++, customWalk.GetObj()->m_category, customWalk.GetObj()->m_type);
			customWalk.Next();
		}
		cityWalk.Next();
	}
}

void EditQueue::Save(const MBCHAR * saveFileName)
{
	if (s_editQueue->m_city.IsValid()) {
		s_editQueue->m_city.GetCityData()->SaveQueue(saveFileName);
	}
	else
	{
		FILE * saveFile = c3files_fopen(C3DIR_DIRECT, saveFileName, "w");
		Assert(saveFile);
		if (!saveFile) {
			return;
		}

		PointerList<EditItemInfo>::Walker customWalk(&s_editQueue->m_customBuildList);
		while (customWalk.IsValid())
		{
			EditItemInfo * itemInfo = customWalk.GetObj();
			c3files_fprintf(saveFile, "%s%s\n", itemInfo->GetCategory(), itemInfo->GetID());
			customWalk.Next();
		}
		c3files_fclose(saveFile);
	}
}

void EditQueue::LoadCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	if (!s_editQueue) {
		return;
	}

	const MBCHAR * loadName = s_editQueue->GetSelectedQueueName();
	if (!loadName) {
		return;
	}

	if (s_editQueue->m_city.IsValid())
	{
		MBCHAR buffer[k_MAX_NAME_LEN];
		const MBCHAR * format = g_theStringDB->GetNameStr("str_ldl_EditQueueReallyLoad");
		if (!format) {
			format = "Load queue %s, for city %s?";
		}
		sprintf(buffer, format, loadName, s_editQueue->m_city.GetCityData()->GetName());

		MessageBoxDialog::Query(buffer, "QueryLoadQueue", LoadQueryCallback, (void *)loadName);
	}
	else if (s_editQueue->m_mode == EDIT_QUEUE_MODE_MULTI)
	{
		MBCHAR buffer[k_MAX_NAME_LEN];
		const MBCHAR * format = g_theStringDB->GetNameStr("str_ldl_EditQueueReallyLoadMulti");
		if (!format) {
			format = "Load Queue %s into all selected cities?";
		}
		sprintf(buffer, format, loadName);

		MessageBoxDialog::Query(buffer, "QueryLoadQueueMulti", LoadQueryCallback, (void *)loadName);
	}
	else
	{
		LoadCustom(loadName);
		s_editQueue->ExitLoadMode();
		s_editQueue->Update();
		DoFocusList(s_editQueue->m_queueList, true);
	}
}

void EditQueue::LoadQueryCallback(bool response, void * data)
{
	if (!response) {
		return;
	}

	const MBCHAR * loadName = (const MBCHAR *)data;

	char loadFileName[_MAX_PATH];
	g_civPaths->GetSavePath(C3SAVEDIR_QUEUES, loadFileName);
	strcat(loadFileName, FILE_SEP);
	strcat(loadFileName, loadName);

	if (s_editQueue->m_city.IsValid()) {
		s_editQueue->m_city.GetCityData()->LoadQueue(loadFileName);
	}
	else
	{
		Assert(s_editQueue->m_mode == EDIT_QUEUE_MODE_MULTI);
		PointerList<EditQueueCityInfo>::Walker cityWalk(&s_editQueue->m_multiCities);
		while (cityWalk.IsValid())
		{
			cityWalk.GetObj()->m_city.GetCityData()->LoadQueue(loadFileName);
			cityWalk.Next();
		}
	}
	s_editQueue->ExitLoadMode();
	s_editQueue->Update();
	DoFocusList(s_editQueue->m_queueList, true);
}

void  EditQueue::LoadCustom(const MBCHAR * fileName)
{
	char loadFileName[_MAX_PATH];
	g_civPaths->GetSavePath(C3SAVEDIR_QUEUES, loadFileName);
	strcat(loadFileName, FILE_SEP);
	strcat(loadFileName, fileName);

	s_editQueue->m_customBuildList.DeleteAll();

	FILE * queueFile = c3files_fopen(C3DIR_DIRECT, loadFileName, "r");
	if (!queueFile) {
		return;
	}

	char buffer[k_MAX_NAME_LEN];
	while (!c3files_feof(queueFile))
	{
		if (!c3files_fgets(buffer, k_MAX_NAME_LEN, queueFile)) {
			continue;
		}
		buffer[strlen(buffer) - 1] = 0;

		EditItemInfo * editItemInfo = EditItemInfo::Construct(buffer);
		if (!editItemInfo) {
			continue;
		}
		s_editQueue->m_customBuildList.AddTail(editItemInfo);
	}

	c3files_fclose(queueFile);

	s_editQueue->Update();
}

const MBCHAR *EditQueue::GetSelectedQueueName()
{
	if (!s_editQueue || !s_editQueue->m_queueFileList) {
		return NULL;
	}

	ctp2_ListItem * item = (ctp2_ListItem *)s_editQueue->m_queueFileList->GetSelectedItem();
	if (!item) {
		return NULL;
	}

	return (const MBCHAR *)item->GetUserData();
}

void EditQueue::QueueFileList(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_LISTBOX_ACTION_SELECT) {
		return;
	}

	if (!s_editQueue || !s_editQueue->m_queueFileList) {
		return;
	}

	ClearChoiceList(s_editQueue->m_queueContents);
	ShowSelectedInfo();

	ctp2_ListItem * item = (ctp2_ListItem *)s_editQueue->m_queueFileList->GetSelectedItem();
	if (!item) {
		s_editQueue->m_queueName->SetText("");
	} else {
		s_editQueue->DisplayQueueContents((const MBCHAR *) item->GetUserData());
	}
	s_editQueue->m_deleteButton->Enable(item != NULL);
	s_editQueue->m_loadModeLoadButton->Enable(s_editQueue->m_queueContents->NumItems() > 0);
}

void EditQueue::DisplayQueueContents(const MBCHAR * queueName)
{
	char loadFileName[_MAX_PATH];
	g_civPaths->GetSavePath(C3SAVEDIR_QUEUES, loadFileName);
	strcat(loadFileName, FILE_SEP);
	strcat(loadFileName, queueName);

	FILE * queueFile = c3files_fopen(C3DIR_DIRECT, loadFileName, "r");
	if (!queueFile) {
		return;
	}

	ClearChoiceList(m_queueContents);

	char buffer[k_MAX_NAME_LEN];
	const MBCHAR *name;
	while (!c3files_feof(queueFile))
	{
		if (!c3files_fgets(buffer, k_MAX_NAME_LEN, queueFile)) {
			continue;
		}

		buffer[strlen(buffer) - 1] = 0;
		EditItemInfo * editItemInfo = EditItemInfo::Construct(buffer);
		if (!editItemInfo) {
			continue;
		}

		ctp2_ListItem * item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("eq_ContentsItem");
		Assert(item);
		if (!item) {
			break;
		}

		ctp2_Static * label = (ctp2_Static *)item->GetChildByIndex(0);
		Assert(label);
		if (!label) {
			break;
		}

		label->SetText(editItemInfo->GetName());
		item->SetUserData(editItemInfo);
		m_queueContents->AddItem(item);
	}

	c3files_fclose(queueFile);

	m_queueName->SetText(queueName);
}

void EditQueue::DeleteCallback(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	const MBCHAR * queueName = GetSelectedQueueName();
	if (!queueName) {
		return;
	}

	char buffer[k_MAX_NAME_LEN];
	const MBCHAR * format = g_theStringDB->GetNameStr("str_ldl_EditQueueReallyDelete");
	if (!format) {
		format = "Permanently delete Queue %s?";
	}
	sprintf(buffer, format, queueName);

	MessageBoxDialog::Query(buffer, "QueryDeleteQueue", DeleteQueryCallback, (void *)queueName);
}

void EditQueue::DeleteQueryCallback(bool response, void * data)
{
	if (!response) {
		return;
	}

	const MBCHAR * queueName = (const MBCHAR *)data;

	char deleteFileName[_MAX_PATH];
	g_civPaths->GetSavePath(C3SAVEDIR_QUEUES, deleteFileName);
	strcat(deleteFileName, FILE_SEP);
	strcat(deleteFileName, queueName);

	remove(deleteFileName);
	s_editQueue->UpdateFileLists();
}

void EditQueue::DoFocusList(ctp2_ListBox * list, bool focus)
{
	if (!list) {
		return;
	}

	if (!focus && list->GetSelectedItemIndex() >= 0) {
		list->DeselectItem(list->GetSelectedItemIndex());
	} else if (focus && list->GetSelectedItemIndex() < 0 && list->NumItems() > 0) {
		list->SelectItem(0);
	}
}

void EditQueue::FocusList(ctp2_ListBox * list)
{
	if (list->GetSelectedItemIndex() >= 0)
	{
		DoFocusList(m_unitList, list == m_unitList);
		DoFocusList(m_buildingList, list == m_buildingList);
		DoFocusList(m_wonderList, list == m_wonderList);
		DoFocusList(m_queueList, list == m_queueList);
		DoFocusList(m_queueContents, list == m_queueContents);
	}
}

void EditQueue::ToggleTab(ctp2_ListBox * list, ctp2_Button * button, bool show)
{
	if (show) {
		button->SetToggleState(true);
	}
	else
	{
		list->Hide();
		list->DeselectItem(list->GetSelectedItem());
		button->SetToggleState(false);
	}
}

void EditQueue::SelectChoiceList(ctp2_ListBox * list)
{
	list->Show();

	ToggleTab(m_unitList, m_unitsButton, list == m_unitList);
	ToggleTab(m_buildingList, m_buildingsButton, list == m_buildingList);
	ToggleTab(m_wonderList, m_wondersButton, list == m_wonderList);

	ShowSelectedInfo();
	UpdateButtons();
}

void EditQueue::RushBuy(bool pay)
{
	if (!m_queueList || !m_queueList->GetSelectedItem() || m_queueList->GetSelectedItemIndex() != 0) {
		return;
	}

	if (!m_city.IsValid()) {
		return;
	}

	if (pay) {
		m_city.GetCityData()->AddBuyFront();
	}
	m_rushBuyButton->Enable(false);
	m_rushBuyCost->SetText("---");
	UpdateQueueList();
	m_queueList->SelectItem(0);
}

void EditQueue::RushBuyCallback(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	if (!s_editQueue || !s_editQueue->m_city.IsValid()) {
		return;
	}

	s_editQueue->RushBuy();
	NationalManagementDialog::UpdateCity(s_editQueue->m_city);
}

void EditQueue::QueueListCallback(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT && action != AUI_LISTBOX_ACTION_SELECT) {
		return;
	}

	if (!s_editQueue) {
		return;
	}

	if (action == AUI_LISTBOX_ACTION_DOUBLECLICKSELECT)
	{
		if (data == 0 && s_editQueue->m_city.IsValid() && s_editQueue->m_city.GetCityData()->AlreadyBoughtFront()) {
			// no action
		} else {
			s_editQueue->Remove();
		}
	}
	else
	{
		s_editQueue->FocusList(reinterpret_cast<ctp2_ListBox *>(control));
		s_editQueue->ShowSelectedInfo();
		s_editQueue->UpdateButtons();
	}
}

void EditQueue::GotoCity(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	if(!s_editQueue || !s_editQueue->m_city.IsValid()) {
		return;
	}

	CityWindow::Display(s_editQueue->m_city);
}

//----------------------------------------------------------------------------
//
// Name       : EditQueue::OpenNationalManager
//
// Description: Opens the National Manager when the National Manager button is clicked.
//
// Parameters : aui_Control *control
//              uint32 action
//              uint32 data
//              void *cookie
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void EditQueue::OpenNationalManager(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	NationalManagementDialog::Open();
}

class ConfirmOverwriteQueueAction:public aui_Action
{
public:
	ConfirmOverwriteQueueAction(const MBCHAR * text)
	{
		strncpy(m_text, text, _MAX_PATH-1);
		m_text[_MAX_PATH-1] = 0;
	}

	virtual void Execute(aui_Control * control, uint32 action, uint32 data);

private:
	MBCHAR m_text[_MAX_PATH];
};

void ConfirmOverwriteQueueAction::Execute(aui_Control * control, uint32 action, uint32 data)
{
	MBCHAR buffer[k_MAX_NAME_LEN];
	const MBCHAR * format = g_theStringDB->GetNameStr("str_ldl_EditQueueReallyOverwrite");
	if (!format) {
		format = "Overwrite queue %s?";
	}
	sprintf(buffer, format, m_text);

	MessageBoxDialog::Query(buffer, "QueryOverwriteQueue", EditQueue::SaveQueryCallback);
};

AUI_ACTION_BASIC(MustEnterNameAction);

void MustEnterNameAction::Execute(aui_Control * control, uint32 action, uint32 data)
{
	MessageBoxDialog::Information("str_ldl_EditQueueMustEnterName", "InfoMustName",
		EditQueue::SaveInformationCallBack);
}

class QuerySaveNameAction:public aui_Action {
public:
	QuerySaveNameAction(const MBCHAR * defaultSaveName)
	{
		strncpy(m_defaultSaveName, defaultSaveName, _MAX_PATH-1);
		m_defaultSaveName[_MAX_PATH-1] = 0;
	}

	virtual void Execute(aui_Control * control, uint32 action, uint32 data);

private:
	MBCHAR m_defaultSaveName[_MAX_PATH];
};

void QuerySaveNameAction::Execute(aui_Control * control, uint32 action, uint32 data)
{
	MessageBoxDialog::TextQuery(g_theStringDB->GetNameStr("str_code_SaveQueueAsColon"), EditQueue::SaveNameResponse,
		NULL, "str_ldl_MB_OK", "str_ldl_MB_CANCEL", m_defaultSaveName);
}

void EditQueue::SaveQueryCallback(bool response, void * data)
{
	if (!s_editQueue) {
		return;
	}

	if (response)
	{
		MBCHAR saveFileName[_MAX_PATH];
		g_civPaths->GetSavePath(C3SAVEDIR_QUEUES, saveFileName);
		strncat(saveFileName, FILE_SEP, _MAX_PATH - 1 - strlen(saveFileName));
		strncat(saveFileName, s_editQueue->m_queueFileName, _MAX_PATH - 1 - strlen(saveFileName));
		saveFileName[_MAX_PATH - 1] = 0;

		Save(saveFileName);
		if (s_editQueue->m_mode == EDIT_QUEUE_MODE_CUSTOM) {
			SetMode(s_editQueue->m_oldMode);
		}
	}
	else {
		g_c3ui->AddAction(new QuerySaveNameAction(s_editQueue->m_queueFileName));
	}
}

void EditQueue::SaveNameResponse(bool response, const char * text, void * data)
{
	if (!s_editQueue) {
		return;
	}

	if (response)
	{
		if (strlen(text) < 1)
		{
			g_c3ui->AddAction(new MustEnterNameAction());
			return;
		}

		strncpy(s_editQueue->m_queueFileName, text, _MAX_PATH-1);
		s_editQueue->m_queueFileName[_MAX_PATH-1] = 0;

		MBCHAR saveFileName [_MAX_PATH];
		g_civPaths->GetSavePath(C3SAVEDIR_QUEUES, saveFileName);
		strncat(saveFileName, FILE_SEP, _MAX_PATH - 1 - strlen(saveFileName));
		strncat(saveFileName, s_editQueue->m_queueFileName, _MAX_PATH - 1 - strlen(saveFileName));
		saveFileName[_MAX_PATH - 1] = 0;

		FILE *test = c3files_fopen(C3DIR_DIRECT, saveFileName, "r");
		if (!test)
		{
			Save(saveFileName);
			if (s_editQueue->m_mode == EDIT_QUEUE_MODE_CUSTOM) {
				SetMode(s_editQueue->m_oldMode);
			}
		}
		else
		{
			c3files_fclose(test);
			g_c3ui->AddAction(new ConfirmOverwriteQueueAction(s_editQueue->m_queueFileName));
		}
	}
}

void EditQueue::SaveButton(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	g_c3ui->AddAction(new QuerySaveNameAction(""));
}

void EditQueue::SaveInformationCallBack(bool response, void * data)
{
	g_c3ui->AddAction(new QuerySaveNameAction(""));
}

bool EditQueue::IsItemInQueueList(uint32 category, sint32 type)
{
	for(sint32 i = 0; i < m_queueList->NumItems(); i++)
	{
		ctp2_ListItem * item = (ctp2_ListItem *)m_queueList->GetItemByIndex(i);
		Assert(item);
		if (item)
		{
			EditItemInfo * itemInfo = (EditItemInfo *)item->GetUserData();
			if (itemInfo && itemInfo->m_category == category && itemInfo->m_type == type) {
				return true;
			}
		}
	}
	return false;
}

void EditQueue::NotifyCityCaptured(const Unit & city)
{
	if (!s_editQueue) {
		return;
	}

	if (!IsShown()) {
		return;
	}

	if (s_editQueue->m_city.IsValid())
	{
		if (city == s_editQueue->m_city)
		{
			s_editQueue->m_city = 0;
			Hide();
		}
	}
	else if (s_editQueue->m_multiCities.GetCount() > 0)
	{
		bool wasEditing = false;
		PointerList<EditQueueCityInfo>::Walker cityWalk(&s_editQueue->m_multiCities);
		while (cityWalk.IsValid())
		{
			if (cityWalk.GetObj()->m_city == city)
			{
				cityWalk.Remove();
				wasEditing = true;
			}
			else {
				cityWalk.Next();
			}
		}
		if (wasEditing) {
			Hide();
		}
	}
}

void EditQueue::SetItemDescription(
		const IconRecord  * icon,
		SlicContext       & context,
		ctp2_Static       * imageBox,
		ctp2_HyperTextBox * hyperTextBox,
		aui_Region        * parent,
		ctp2_Button       * imageButton)
{
	Assert(imageBox || imageButton);
	if (!imageBox && !imageButton) {
		return;
	}

	Assert(hyperTextBox);
	if(!hyperTextBox) {
		return;
	}

	if (icon)
	{
		MBCHAR buffer[1024];
		if (!icon->GetLargeIcon() || stricmp(icon->GetLargeIcon(), "null") == 0) {
			strcpy(buffer, icon->GetIcon());
		} else {
			strcpy(buffer, icon->GetLargeIcon());
		}

		if (imageBox)
		{
			imageBox->SetImage(buffer);
			imageBox->ShouldDraw(true);
		}
		else if(imageButton)
		{
			imageButton->ExchangeImage(4, 0, buffer);
			imageButton->ShouldDraw(true);
		}

		char * allocatedText = NULL;
		const char * statText = icon->GetStatText();
		const char * descString = NULL;
		const char * greatLibraryText = NULL;
		if (strrchr(statText, '.') && (!(stricmp(strrchr(statText, '.'), ".txt"))))
		{
			size_t   size = 0;
			MBCHAR * fileText = reinterpret_cast<MBCHAR *>(g_GreatLibPF->getData(statText, size, C3DIR_GL));
			if (fileText)
			{
				allocatedText = new MBCHAR[size + 1];
				memcpy(allocatedText, fileText, size * sizeof(MBCHAR));
				allocatedText[size] = 0;
			}
			g_GreatLibPF->freeData(fileText);
		}
		else {
			greatLibraryText = glutil_LoadText(statText, context);
		}

		if( !allocatedText && !greatLibraryText) {
			descString = g_theStringDB->GetNameStr(icon->GetStatText());
		}

		Assert(descString || allocatedText || greatLibraryText);
		MBCHAR interpretedText[2048];
		if (descString) {
			stringutils_Interpret(descString, context, interpretedText);
		} else if (allocatedText)
		{
			stringutils_Interpret(allocatedText, context, interpretedText);
			delete [] allocatedText;
		} else if(greatLibraryText) {
			strcpy(interpretedText, greatLibraryText);
		} else {
			strcpy(interpretedText, icon->GetStatText());
		}

		hyperTextBox->SetHyperText(interpretedText);
	} else {
		if (imageBox)
		{
			imageBox->SetImage(NULL);
			imageBox->ShouldDraw(true);
		}
		else if (imageButton)
		{
			imageButton->ExchangeImage(4, 0, NULL);
			imageButton->ShouldDraw(true);
		}

		hyperTextBox->SetHyperText("");
		if (parent) {
			parent->ShouldDraw(true);
		}
	}
}
