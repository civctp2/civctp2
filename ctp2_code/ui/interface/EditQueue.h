//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
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
// - Added National Manager button and functions callback. - July 24th 2005 Martin Gühmann
// - Added a suggest build item button to the build manager for AI testing. (30-Jun-2008 Martin Gühmann)
// - Added stuff for reimplementing switch production penalty. (22-Jul-2009 Maq)
//
//----------------------------------------------------------------------------

#ifndef EDIT_QUEUE_H__
#define EDIT_QUEUE_H__

class ctp2_ListBox;
class CityData;
class ctp2_Window;
class aui_Control;
class ctp2_Static;
class ctp2_HyperTextBox;
class ctp2_DropDown;
class UnitDynamicArray;
class ctp2_TextField;
class ctp2_Button;
class ctp2_ListItem;

#include "pointerlist.h"
#include "Unit.h"

enum EDIT_QUEUE_MODE {
	EDIT_QUEUE_MODE_SINGLE,
	EDIT_QUEUE_MODE_MULTI,
	EDIT_QUEUE_MODE_CUSTOM
};

enum EDIT_QUEUE_MULTI_ACTION {
	EDIT_QUEUE_MULTI_ACTION_INSERT,
	EDIT_QUEUE_MULTI_ACTION_APPEND,
	EDIT_QUEUE_MULTI_ACTION_OVERWRITE,
	EDIT_QUEUE_MULTI_ACTION_OVERWRITE_CONFIRMED
};

class EditItemInfo {
public:
	EditItemInfo(uint32 cat, sint32 type) {
		m_category = cat;
		m_type = type;
	}

	uint32 m_category;
	sint32 m_type;
};

class EditQueueCityInfo {
public:
	EditQueueCityInfo(CityData *cd) {
		m_cityData = cd;
	}
	CityData *m_cityData;
};

class EditQueue {
public:
	EditQueue(AUI_ERRCODE * error);
	~EditQueue();

	static AUI_ERRCODE   Display(CityData * city);
	static AUI_ERRCODE   Display(const UnitDynamicArray & cities);
	static AUI_ERRCODE   Hide();
	static EditQueue   * GetEditQueueWindow();
	static AUI_ERRCODE   Cleanup();

	static void SaveQueryCallback(bool response, void * data);

	CityData * GetCityData() const { return m_cityData; }

	void InsertInQueue(EditItemInfo * info, bool insert, bool confirm = false, bool confirmSwitch = false);
	void Down(bool confirmSwitch = false);
	void Up(bool confirmSwitch = false);
	void Remove(bool confirmSwitch = false);

	static void NotifyCityCaptured(const Unit & unit);
	static bool EditingCity(CityData * city);

private:
	static AUI_ERRCODE Initialize();
	static AUI_ERRCODE Display();
	static bool        IsShown();

	static const MBCHAR * GetSelectedQueueName();

	static void ShowSelectedInfo();
	static void ClearChoiceList(ctp2_ListBox * choiceList);

	static void Save(const MBCHAR * saveFileName);
	static void LoadCustom(const MBCHAR * loadFileName);

	static void SetMode(EDIT_QUEUE_MODE mode);
	static void SetCity(CityData * city);
	static void SetMultiCities(const UnitDynamicArray & cities);

	static sint32 CompareUnitItems(ctp2_ListItem * item1, ctp2_ListItem * item2, sint32 column);
	static sint32 CompareBuildingWonderItems(ctp2_ListItem * item1, ctp2_ListItem * item2, sint32 column);

	static void ToggleUnits        (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void ToggleBuildings    (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void ToggleWonders      (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void AddItem            (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void InsertItem         (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void SuggestItem        (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void RemoveItem         (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void ItemUp             (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void ItemDown           (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void ListCallback       (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void ClearButton        (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void Close              (aui_Control * control, uint32 action, uint32 data, void * cookie);

	static void CityDropDown       (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void PreviousCity       (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void NextCity           (aui_Control * control, uint32 action, uint32 data, void * cookie);

	static void RushBuyCallback    (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void Library            (aui_Control * control, uint32 action, uint32 data, void * cookie);

	static void GotoCity           (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void OpenNationalManager(aui_Control * control, uint32 action, uint32 data, void * cookie);

	static void LoadModeCallback   (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void LoadCallback       (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void DeleteCallback     (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void CustomButton       (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void SaveButton         (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void MultiActionButton  (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void QueueFileList      (aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void QueueListCallback  (aui_Control * control, uint32 action, uint32 data, void * cookie);

	static void LoadQueryCallback   (bool response, void * data);
	static void DeleteQueryCallback (bool response, void * data);
	static void ClearMessageCallback(bool response, void * data);
	static void ConfirmOverwrite    (bool response, void * data);

	static void SaveNameResponse(bool response, const char * text, void * data);

	ctp2_ListBox * GetVisibleItemList();

	void DisplayQueueContents(const MBCHAR * queueName);

	void AddChoiceItem(const MBCHAR * text, EditItemInfo * info, sint32 time, ctp2_ListBox * list);
	void Update();
	void UpdateChoiceLists();
	void UpdateQueueList();
	void UpdateCityLists();
	void UpdateFileLists();
	void UpdateButtons();
	void ClearChoiceLists();

	void Add(bool insert);
	void Suggest(bool insert);

	void EnterLoadMode();
	void ExitLoadMode();

	void SelectChoiceList(ctp2_ListBox * list);

	void RushBuy();

	bool IsItemInQueueList(uint32 category, sint32 type);

	ctp2_Window       * m_window;
	ctp2_ListBox      * m_unitList, * m_buildingList, * m_wonderList;
	ctp2_ListBox      * m_queueList;
	CityData          * m_cityData;
	ctp2_Window       * m_attachedToWindow;
	ctp2_Button       * m_itemImageButton;
	ctp2_HyperTextBox * m_itemDescription;

	ctp2_Static   * m_itemsBox, * m_loadBox, * m_queueBox;
	ctp2_Static   * m_singleCityChooser;
	ctp2_ListBox  * m_multiCityList;
	ctp2_DropDown * m_cityDropDown;
	ctp2_Static   * m_multiButtonGroup;
	ctp2_Static   * m_queueName;
	ctp2_ListBox  * m_queueFileList;
	ctp2_ListBox  * m_queueContents;

	ctp2_Button * m_unitsButton, * m_buildingsButton, * m_wondersButton;

	ctp2_Button * m_rushBuyButton;
	ctp2_Static * m_rushBuyCost;
	ctp2_Button * m_libraryButton;

	ctp2_Button * m_addButton;
	ctp2_Button * m_insertButton;
	ctp2_Button * m_suggestButton;
	ctp2_Button * m_upButton;
	ctp2_Button * m_downButton;
	ctp2_Button * m_removeButton;
	ctp2_Button * m_clearButton;

	ctp2_Static  * m_modeLabel;
	ctp2_Static  * m_customModeButtons, * m_normalModeButtons;
	ctp2_ListBox * m_listBeforeLoadSaveMode;
	ctp2_Static  * m_queueLabel;
	ctp2_Button  * m_createCustomQueueButton;

	ctp2_Button * m_loadModeLoadButton;

	ctp2_Button * m_gotoCityButton;
	ctp2_Button * m_nationalManagerButton;

	PointerList<EditItemInfo>      m_customBuildList;
	PointerList<EditQueueCityInfo> m_multiCities;
	PointerList<MBCHAR>            m_queueFileNames;

	bool m_inCallback;
	bool m_updating;
	Unit m_oldCity;

	sint32 m_itemCategory;
	sint32 m_itemType;

	EDIT_QUEUE_MODE m_mode, m_oldMode;
};

#endif
