
#ifndef TRADE_MANAGER_H__
#define TRADE_MANAGER_H__

class ctp2_Window;
class ctp2_ListBox;
class ctp2_ListItem;
class ctp2_Static;
class aui_Surface;
class aui_Control;
class ctp2_Button;
class C3Slider;

#include "Unit.h"
#include "pointerlist.h"


struct CreateListData {
	Unit m_source;
	sint32 m_resource;
	Unit m_destination;
	sint32 m_price;
	sint32 m_caravans;
	Unit m_curDestination;
};

enum TRADE_MANAGER_MODE {
	TRADE_MANAGER_MARKET,
	TRADE_MANAGER_SUMMARY,
	TRADE_MANAGER_MAX
};

enum TRADE_CITIES {
	TRADE_CITIES_OWN,
	TRADE_CITIES_FRIENDLY,
	TRADE_CITIES_ALL
};

class TradeManager
{
  private:
	ctp2_Window *m_window;
	ctp2_Window *m_adviceWindow;

	ctp2_ListBox *m_createList;
	ctp2_ListBox *m_summaryList;

	ctp2_Button  *m_createButton, *m_breakButton;
	
	ctp2_Button *m_ownCitiesButton, *m_friendlyCitiesButton, *m_allCitiesButton;

	ctp2_Static *m_numCitiesLabel;

	PointerList<CreateListData> m_createData;

	TRADE_CITIES m_showCities;
	sint32 m_numCities;

	C3Slider *m_citiesSlider;

  public:
	TradeManager(AUI_ERRCODE *err);
	~TradeManager();

	static AUI_ERRCODE Initialize();
	static AUI_ERRCODE Cleanup();
	static AUI_ERRCODE Display();
	static AUI_ERRCODE Hide();

	static void SetMode(TRADE_MANAGER_MODE mode);
	static void Notify();
	void Update();
	void UpdateCreateList(const PLAYER_INDEX & player_id);
	void UpdateAdviceWindow();
	void UpdateAdviceText();
	void UpdateSummaryList();

	static void Close(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CreateRoute(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ShowAdvice(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Summary(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ListSelect(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void SummaryListSelect(aui_Control *control, uint32 action, uint32 data, void *cookie);
	void FilterButtonActivated(aui_Control *control);
	static void CityFilterButton(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static sint32 CompareCreateItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column);
	static sint32 CompareSummaryItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column);
	static AUI_ERRCODE DrawNationColumn(ctp2_Static *control,
													  aui_Surface *surface,
													  RECT &rect,
													  void *cookie);
	static AUI_ERRCODE DrawPiracyColumn(ctp2_Static *control,
													  aui_Surface *surface,
													  RECT &rect,
													  void *cookie);

	static void InitializeEvents();
	static void CleanupEvents();

	void SetNumCities(sint32 num);
	static void NumCitiesSlider(aui_Control *control, uint32 action, uint32 data, void *cookie);
};

#endif
