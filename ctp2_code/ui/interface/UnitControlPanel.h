






#pragma once
#ifndef ___BMH_UNIT_CONTROL_PANEL_HEADER
#define ___BMH_UNIT_CONTROL_PANEL_HEADER


#include <utility>


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
class CellUnitList;

#define k_MAX_CP_CARGO 5
#define k_CP_CARGO_HEIGHT 4


class UnitControlPanel {
public:
	
	UnitControlPanel(MBCHAR *ldlBlock);

	
	void Update();

	
	void SelectedUnit();

	
	void Activated();

	bool GetSelectedCargo(CellUnitList &cargo);

	
	enum UnitSelectionMode {
		SINGLE_SELECTION,
		MULTIPLE_SELECTION,
		ARMY_SELECTION,
		TRANSPORT_SELECTION,
	};

	
	
	void SetSelectionMode(UnitSelectionMode mode);

private:
	
	void UpdateSingleSelectionDisplay();

	
	void UpdateMultipleSelectionDisplay();

	
	void UpdateArmySelectionDisplay();

	
	void UpdateTransportSelectionDisplay();

	
	void UpdateOrderButtons();

	
	
	Army GetSelectedArmy();

	
	void GiveOrder(OrderRecord *order);

	
	
	
	static void UnitDisplayGroupCallback(aui_Region *region,
		void *userData);

	
	static void PrevUnitButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	static void NextUnitButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	static AUI_ERRCODE HealthBarActionCallback(ctp2_Static *control,
		aui_Surface *surface, RECT &rect, void *cookie);

	
	static AUI_ERRCODE FuelBarDrawCallback(ctp2_Static *control,
		aui_Surface *surface, RECT &rect, void *cookie);

	
	static AUI_ERRCODE DrawCargoCallback(ctp2_Static *control, aui_Surface *surface,
										 RECT &rect, void *cookie);

	
	
	static void MultiButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	
	static void ArmyButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	static void OrderButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	static void TransportImageCallback(ctp2_Static *control,
									   aui_MouseEvent *event,
									   void *cookie);

	void UnsetCargoButtons();

	
	UnitSelectionMode m_currentMode;

	
	ctp2_Static *m_unitDisplayGroup;

	
	ctp2_Button *m_unitListPreviousButton;	
	ctp2_Static *m_unitListLabel;			
	ctp2_Button *m_unitListNextButton;		

	
	ctp2_Static *m_singleSelectionDisplay;		
	ctp2_Static *m_singleSelectionIcon;			
	ctp2_Static *m_singleSelectionAttack;		
	ctp2_Static *m_singleSelectionDefend;		
	ctp2_Static *m_singleSelectionMove;			
	ctp2_Static *m_singleSelectionRange;		
	ctp2_Static *m_singleSelectionArmor;        
	ctp2_Static *m_singleSelectionFirepower;    
	ctp2_Static *m_singleSelectionHealth;		
	ctp2_Static *m_singleSelectionFuel;         

	
	enum { NUMBER_OF_MULTIPLE_SELECTION_BUTTONS = 12 };
	ctp2_Static *m_multipleSelectionDisplay;	
	ctp2_Button *m_multipleSelectionButton[		
		NUMBER_OF_MULTIPLE_SELECTION_BUTTONS];
	ctp2_Static *m_multipleSelectionHealth[NUMBER_OF_MULTIPLE_SELECTION_BUTTONS];

	
	std::pair<UnitControlPanel*, uint32>
		m_multiPair[NUMBER_OF_MULTIPLE_SELECTION_BUTTONS];

	
	enum { NUMBER_OF_ARMY_SELECTION_BUTTONS = 12 };
	ctp2_Static *m_armySelectionDisplay;		
	ctp2_Button *m_armySelectionIcon;			
	ctp2_Button *m_armySelectionButton[			
		NUMBER_OF_ARMY_SELECTION_BUTTONS];
	ctp2_Static *m_armySelectionHealth[NUMBER_OF_ARMY_SELECTION_BUTTONS];

	
	ctp2_Static *m_transportSelectionDisplay;   
	ctp2_Button *m_transportSelectionIcon;      
	ctp2_Switch *m_transportSelectionButton[k_MAX_CP_CARGO]; 
	uint32       m_transportSelectionCargo[k_MAX_CP_CARGO]; 
	ctp2_Static *m_transportSelectionHealth[k_MAX_CP_CARGO];

	
	sint32 m_armySelectionUnit;

	
	enum { NUMBER_OF_ORDER_BUTTONS = 12 };
	sint32 m_displayedOrderIndex[NUMBER_OF_ORDER_BUTTONS];
	ctp2_Button *m_orderButton[NUMBER_OF_ORDER_BUTTONS];

	
	std::pair<UnitControlPanel*, OrderRecord*>
		m_orderPair[NUMBER_OF_ORDER_BUTTONS];

	
	Army m_curSingleArmy;
	Unit m_curSingleUnit;
	double m_curMovement;
	double m_curHealth;
	sint32 m_curCargo;
	sint32 m_curFuel;

	Army m_lastSelectedArmy;
	sint32 m_lastSelectionUnit;
	sint32 m_lastSelectedArmyCount;
};

#endif 
