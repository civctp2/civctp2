
#ifndef UNIT_MANAGER_H__
#define UNIT_MANAGER_H__

class ctp2_Window;
class ctp2_ListBox;
class ctp2_TabGroup;
class ctp2_Tab;
class aui_Control;
class ctp2_ListItem;
class ctp2_Static;
class aui_Surface;

#include "PointerList.h"

struct UnitManagerCategoryInfo
{
	sint32 stringId;
	sint32 numUnits;
};

class UnitManager {
  public:
	UnitManager(AUI_ERRCODE *err);
	~UnitManager();

	static AUI_ERRCODE Initialize();
	static AUI_ERRCODE Cleanup();
	static AUI_ERRCODE Display();
	static AUI_ERRCODE Hide();

	void Update();
	void UpdateStatsList();
	void UpdateTacticalList();
	void UpdateAdvice();
	void UpdateReadiness();
	void UpdateNumUnits();
	static void UpdateAdviceText();

	static void TabGroupCallback(ctp2_TabGroup *group, ctp2_Tab *tab, void *cookie);
	static void Close(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static sint32 CompareStatItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column);
	static sint32 CompareTacticalItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column);
	static sint32 CompareAdviceItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column);

	static AUI_ERRCODE DrawHealthBar(ctp2_Static *control, aui_Surface *surface,
									 RECT &rect, void *cookie);

	static void UpkeepButton(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Advice(aui_Control *control, uint32 action, uint32 data, void *cookie);
	

	static void TacticalList(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void DisbandButton(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void TabChanged(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void ReadinessActionCallback(aui_Control *control,
					uint32 action, uint32 data, void *cookie);

	static void DisbandQuery(bool response, void *data);
	void DisbandSelected();


	static void InitializeEvents();
	static void CleanupEvents();

	uint32 GetLastDisbandedUnit() { return m_lastDisbandedUnit; }
	void SetLastDisbandedUnit(uint32 u) { m_lastDisbandedUnit = u; }
  private:
    
	static bool sm_statsTabVisible;

	ctp2_Window *m_window, *m_adviceWindow;
	ctp2_ListBox *m_statsList, *m_tacticalList, *m_adviceList;
	ctp2_TabGroup *m_tabGroup;
	PointerList<UnitManagerCategoryInfo> m_unitCategories;
	uint32 m_lastDisbandedUnit;
};

#endif
