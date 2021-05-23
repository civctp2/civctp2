#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef ___BMH_UNIT_CONTROL_PANEL_HEADER
#define ___BMH_UNIT_CONTROL_PANEL_HEADER

#include <utility>
#include <vector>

class UnitControlPanel;

#include "Army.h"
#include "Unit.h"

class aui_Control;
class aui_Region;
class aui_Surface;
class ctp2_Button;
class ctp2_Static;
class ctp2_Switch;
class OrderRecord;
struct aui_MouseEvent;

#define k_MAX_CP_CARGO 5
#define k_CP_CARGO_HEIGHT 4

class UnitControlPanel {
public:
	enum UnitSelectionMode {
		SINGLE_SELECTION,
		MULTIPLE_SELECTION,
		ARMY_SELECTION,
		TRANSPORT_SELECTION,
	};

	UnitControlPanel(MBCHAR *ldlBlock);

	void Update();
	void SelectedUnit();
	void Activated();

	bool GetSelectedCargo(CellUnitList & cargoList);

	void SetSelectionMode(UnitSelectionMode mode);

private:
	void DoSetSelectionMode(UnitSelectionMode mode);

	void UpdateSingleSelectionDisplay();
	void UpdateMultipleSelectionDisplay();
	void UpdateArmySelectionDisplay();
	void UpdateTransportSelectionDisplay();
	void UpdateOrderButtons();

	Army GetSelectedArmy();
	void GiveOrder(OrderRecord *order);
	void UnsetCargoButtons();
	void UpdateSingleSelectionSymbols();
	void UpdateMultiSelectionArmySymbols();

	static void UnitDisplayGroupCallback(aui_Region * region, void * userData);
	static void NextUnitButtonActionCallback(aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void MultiButtonActionCallback(aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void ArmyButtonActionCallback(aui_Control * control, uint32 action, uint32 data, void * cookie);
	static void OrderButtonActionCallback(aui_Control * control, uint32 action, uint32 data, void * cookie);

	static AUI_ERRCODE HealthBarActionCallback(ctp2_Static * control, aui_Surface * surface, RECT & rect,
			void * cookie);
	static AUI_ERRCODE StackSymbolDrawCallback(ctp2_Static * control, aui_Surface * surface, RECT & rect,
			void * cookie);
	static AUI_ERRCODE FuelBarDrawCallback(ctp2_Static * control, aui_Surface * surface, RECT & rect, void * cookie);
	static AUI_ERRCODE DrawCargoCallback(ctp2_Static * control, aui_Surface * surface, RECT & rect, void * cookie);

	static void SingleSelectionArmySymbolImageCallback(ctp2_Static * control, aui_MouseEvent * event, void * cookie);
	static void StackSymbolImageCallback(ctp2_Static * control, aui_MouseEvent * event, void * cookie);
	static void TransportImageCallback(ctp2_Static * control, aui_MouseEvent * event, void * cookie);
	static void TransportSelectionImageCallback(ctp2_Static * control, aui_MouseEvent * event, void * cookie);

	static bool SelectionContainsMultipleArmies();
	static void LoadImage00(ctp2_Static * control);
	static void SetVisibilityStackSymbol(ctp2_Static * stackSymbol);

	UnitSelectionMode m_currentMode;

	ctp2_Static * m_unitDisplayGroup;

	ctp2_Static * m_unitListLabel;
	ctp2_Button * m_unitListNextButton;

	ctp2_Static * m_singleSelectionDisplay;
	ctp2_Static * m_singleSelectionIcon;
	ctp2_Static * m_singleSelectionArmySymbol;
	ctp2_Static * m_singleSelectionCargoSymbol;
	ctp2_Static * m_singleSelectionStackSymbol;
	ctp2_Static * m_singleSelectionAttack;
	ctp2_Static * m_singleSelectionDefend;
	ctp2_Static * m_singleSelectionMove;
	ctp2_Static * m_singleSelectionRange;
	ctp2_Static * m_singleSelectionArmor;
	ctp2_Static * m_singleSelectionFirepower;
	ctp2_Static * m_singleSelectionHealth;
	ctp2_Static * m_singleSelectionFuel;

	static const sint32 NUMBER_OF_MULTIPLE_SELECTION_BUTTONS = 12;
	ctp2_Static * m_multipleSelectionDisplay;
	ctp2_Button * m_multipleSelectionButton[NUMBER_OF_MULTIPLE_SELECTION_BUTTONS];
	ctp2_Static * m_multipleSelectionHealth[NUMBER_OF_MULTIPLE_SELECTION_BUTTONS];
	ctp2_Static * m_multipleSelectionArmySymbol[NUMBER_OF_MULTIPLE_SELECTION_BUTTONS];

	static const sint32 NUMBER_OF_ARMY_SELECTION_BUTTONS = 12;
	ctp2_Static * m_armySelectionDisplay;
	ctp2_Static * m_armySelectionIcon;
	ctp2_Static * m_armySelectionStackSymbol;
	ctp2_Button * m_armySelectionButton[NUMBER_OF_ARMY_SELECTION_BUTTONS];
	ctp2_Static * m_armySelectionHealth[NUMBER_OF_ARMY_SELECTION_BUTTONS];
	sint32        m_armySelectionUnit;

	ctp2_Static * m_transportSelectionDisplay;
	ctp2_Static * m_transportSelectionIcon;
	ctp2_Static * m_transportSelectionIconArmySymbol;
	ctp2_Switch * m_transportSelectionButton[k_MAX_CP_CARGO];
	uint32        m_transportSelectionCargo[k_MAX_CP_CARGO];
	ctp2_Static * m_transportSelectionHealth[k_MAX_CP_CARGO];

	static const sint32 NUMBER_OF_ORDER_BUTTONS = 12;
	sint32        m_displayedOrderIndex[NUMBER_OF_ORDER_BUTTONS];
	ctp2_Button * m_orderButton[NUMBER_OF_ORDER_BUTTONS];
	std::pair<UnitControlPanel*, OrderRecord*> m_orderPair[NUMBER_OF_ORDER_BUTTONS];

	Army   m_curSingleArmy;
	Unit   m_curSingleUnit;
	double m_curMovement;
	double m_curHealth;
	sint32 m_curCargo;
	sint32 m_curFuel;

	Army   m_lastSelectedArmy;
	sint32 m_lastSelectionUnit;
	sint32 m_lastSelectedArmyCount;

	std::vector<Unit> m_cellUnitList;
	std::vector<Army> m_cellArmyList;

	class SetSelectionAction;
	class SelectUnitAction;
};

#endif
