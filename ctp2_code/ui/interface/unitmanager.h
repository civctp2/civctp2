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
union Cookie;

#include "pointerlist.h"

struct UnitManagerCategoryInfo
{
	sint32 stringId;
	sint32 numUnits;
};

class UnitManager {
  public:
	UnitManager(AUI_ERRCODE *err);
	~UnitManager();

	void DisbandSelected();
	uint32 GetLastDisbandedUnit() const { return m_lastDisbandedUnit; }
	void SetLastDisbandedUnit(uint32 u) { m_lastDisbandedUnit = u; }
	void Update();
	void UpdateStatsList();
	void UpdateTacticalList();
	void UpdateAdvice();
	void UpdateReadiness();
	void UpdateNumUnits();

	static AUI_ERRCODE  Initialize();
	static void         Cleanup();
	static AUI_ERRCODE  Display();
	static AUI_ERRCODE  Hide();

	static void UpdateAdviceText();

	static void TabGroupCallback(ctp2_TabGroup *group, ctp2_Tab *tab, Cookie cookie);
	static void Close(aui_Control *control, uint32 action, uint32 data, Cookie cookie);

	static sint32 CompareStatItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column);
	static sint32 CompareTacticalItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column);
	static sint32 CompareAdviceItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column);

	static AUI_ERRCODE DrawHealthBar(ctp2_Static *control, aui_Surface *surface,
									 RECT &rect, Cookie cookie);

	static void UpkeepButton(aui_Control *control, uint32 action, uint32 data, Cookie cookie);
	static void Advice(aui_Control *control, uint32 action, uint32 data, Cookie cookie);

	static void TacticalList(aui_Control *control, uint32 action, uint32 data, Cookie cookie);
	static void DisbandButton(aui_Control *control, uint32 action, uint32 data, Cookie cookie);
	static void TabChanged(aui_Control *control, uint32 action, uint32 data, Cookie cookie);

	static void ReadinessActionCallback(aui_Control *control,
					uint32 action, uint32 data, Cookie cookie);

	static void DisbandQuery(bool response, Cookie data);

	static void InitializeEvents();
	static void CleanupEvents();

private:
	static bool sm_statsTabVisible;

	ctp2_Window *       m_window;
    ctp2_Window *       m_adviceWindow;
	ctp2_ListBox *      m_statsList;
    ctp2_ListBox *      m_tacticalList;
    ctp2_ListBox *      m_adviceList;
	ctp2_TabGroup *     m_tabGroup;
	PointerList<UnitManagerCategoryInfo>
                        m_unitCategories;
	uint32              m_lastDisbandedUnit;
};

#endif
