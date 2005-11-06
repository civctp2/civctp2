
#ifndef ARMY_MANAGER_WINDOW_H__
#define ARMY_MANAGER_WINDOW_H__

class ArmyListNode;
class ArmyManagerWindow;

#include "MapPoint.h"
#include "Army.h"
#include "Unit.h"
#include "aui_Control.h"    // aui_Control
#include "ctp2_inttypes.h"  // uint32

class ctp2_Window;
class ctp2_ListItem;
class ctp2_ListBox;
class ctp2_Static;

template <class T> class PointerList;

class ArmyListNode {
  public:
	Army m_army;

	ArmyListNode(Army &a) { m_army = a; }
	ArmyListNode() { m_army.m_id = 0; }
};

class ArmyManagerWindow {
  private:
	ctp2_Window *m_window;
	MapPoint m_pos;
	Army m_army;
	PointerList<ArmyListNode> *m_armies;
	Unit m_inArmy[k_MAX_ARMY_SIZE];
	Unit m_outOfArmy[k_MAX_ARMY_SIZE];

  public:
	ArmyManagerWindow(AUI_ERRCODE *err);
	~ArmyManagerWindow();

	static AUI_ERRCODE Initialize();
	static AUI_ERRCODE Cleanup();
	static AUI_ERRCODE Display();
	static AUI_ERRCODE Hide();

	static void Toggle();
	static bool IsShown();

	void RenameArmy();
	static void NotifySelection();
	static void NotifyRemoteGroupComplete(const Army &army);

	void ClearAll();

	void Update();
	void UpdateArmyName();
	void UpdateArmyItem(ctp2_ListItem *item);
	void UpdateAllArmyItems();
	ctp2_ListItem *AddArmyItem(ctp2_ListBox *list, ArmyListNode *node);
	void RemoveDeadArmies();
	void UpdateList();

	void FillArmies();

	static void NewArmy(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Close(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void List(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Add(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void AddAll(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Remove(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void RemoveAll(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void InArmy(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void OutOfArmy(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static AUI_ERRCODE DrawHealthCallbackInArmy(ctp2_Static *control, aui_Surface *surface, RECT &rect, void *cookie);
	static AUI_ERRCODE DrawHealthCallbackOutOfArmy(ctp2_Static *control, aui_Surface *surface, RECT &rect, void *cookie);

	void AddSelectedUnits();
	void RemoveSelectedUnits();

	static void ArmyManagerWindow::InitializeEvents();
	static void ArmyManagerWindow::CleanupEvents();
};

	
#endif
