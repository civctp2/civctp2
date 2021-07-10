//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Trade manager window
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
// - Corrected non-standard syntax.
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "trademanager.h"

#include "aui.h"
#include "aui_ldl.h"
#include "c3ui.h"
extern C3UI *g_c3ui;

#include "ctp2_Window.h"
#include "ctp2_listbox.h"
#include "ctp2_button.h"
#include "ctp2_listitem.h"
#include "ctp2_Static.h"
#include "ctp2_hypertextbox.h"
#include "ctp2_Tab.h"
#include "ctp2_TabGroup.h"

#include "ResourceRecord.h"
#include "Unit.h"
#include "citydata.h"
#include "player.h"
#include "tradeutil.h"
#include "TradeRouteData.h"

#include "SelItem.h"

#include "pixelutils.h"
#include "colorset.h"           // g_colorSet
#include "aui_blitter.h"

#include "GameEventManager.h"
#include "GameEventUser.h"
#include "Events.h"
#include "StrDB.h"
#include "stringutils.h"
#include "SlicContext.h"
#include "TradeRoute.h"
#include "IconRecord.h"
#include "c3slider.h"

#include "AgreementMatrix.h"
#include "Diplomat.h"

#include "network.h"
#include "net_action.h"
#include "tiledmap.h"

static TradeManager *   s_tradeManager      = NULL;
static MBCHAR *         s_tradeManagerBlock = "TradeManager";
static MBCHAR *         s_tradeAdviceBlock  = "TradeAdvice";

#define k_MAX_CITIES_PER_GOOD 5 //make this a constDB? - it should be 6.12.2007

#define k_CITY_COL_INDEX 0
#define k_GOODICON_COL_INDEX 1
#define k_GOOD_COL_INDEX 2
#define k_TOCITY_COL_INDEX 3
#define k_NATION_COL_INDEX 4
#define k_PRICE_COL_INDEX 5
#define k_CARAVANS_COL_INDEX 6

#define k_CITY_COL_SUM_INDEX 0
#define k_PIRACY_COL_SUM_INDEX 1
#define k_GOODICON_COL_SUM_INDEX 2
#define k_GOOD_COL_SUM_INDEX 3
#define k_TOCITY_COL_SUM_INDEX 4
#define k_NATION_COL_SUM_INDEX 5
#define k_PRICE_COL_SUM_INDEX 6
#define k_CARAVANS_COL_SUM_INDEX 7

TradeManager::TradeManager(AUI_ERRCODE *err)
{
	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_tradeManagerBlock);
	Assert(m_window);
	if(!m_window) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	m_createList = (ctp2_ListBox *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Market.TabPanel.List");
	m_summaryList = (ctp2_ListBox *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Summary.TabPanel.List");
	m_importList = (ctp2_ListBox *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Import.TabPanel.List");

	Assert(m_createList);

	aui_Ldl::SetActionFuncAndCookie(s_tradeManagerBlock, "TradeTabs.Market", TabCallback, (void *)0);
	aui_Ldl::SetActionFuncAndCookie(s_tradeManagerBlock, "TradeTabs.Summary", TabCallback, (void *)1);
	aui_Ldl::SetActionFuncAndCookie(s_tradeManagerBlock, "TradeTabs.Import", TabCallback, (void *)2);

	aui_Ldl::SetActionFuncAndCookie(s_tradeManagerBlock, "CloseButton", Close, NULL);
	
	m_createButton = (ctp2_Button *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Market.TabPanel.CreateRouteButton");
	aui_Ldl::SetActionFuncAndCookie(s_tradeManagerBlock, "TradeTabs.Market.TabPanel.CreateRouteButton", CreateRoute, (void *)0);

	m_breakButton = (ctp2_Button *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Summary.TabPanel.BreakRouteButton");
	aui_Ldl::SetActionFuncAndCookie(s_tradeManagerBlock, "TradeTabs.Summary.TabPanel.BreakRouteButton", CreateRoute, (void *)1);

	m_breakImpBut = (ctp2_Button *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Import.TabPanel.BreakRouteButton");
	aui_Ldl::SetActionFuncAndCookie(s_tradeManagerBlock, "TradeTabs.Import.TabPanel.BreakRouteButton", CreateRoute, (void *)2);

	aui_Ldl::SetActionFuncAndCookie(s_tradeManagerBlock, "ShowAdviceButton", ShowAdvice, NULL);

	if(m_createList) {
		m_createList->SetActionFuncAndCookie(ListSelect, NULL);
	}

	if(m_summaryList) {
		m_summaryList->SetActionFuncAndCookie(SummaryListSelect, NULL);
	}

	if(m_importList) {
		m_importList->SetActionFuncAndCookie(SummaryListSelect, NULL);
	}

	m_adviceWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_tradeAdviceBlock);
	Assert(m_adviceWindow);

	if(m_adviceWindow) {
		m_adviceWindow->Move(m_window->X() - m_adviceWindow->Width(), m_window->Y());
		m_window->AddDockedWindow(m_adviceWindow);
		m_adviceWindow->SetDock(m_window);
	}

	m_ownCitiesButton = (ctp2_Button *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Market.TabPanel.Filters.OwnButton");
	m_friendlyCitiesButton = (ctp2_Button *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Market.TabPanel.Filters.FriendlyButton");
	m_allCitiesButton = (ctp2_Button *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Market.TabPanel.Filters.AllButton");

	m_ownCitiesButton->SetActionFuncAndCookie(CityFilterButton, (void *)TRADE_CITIES_OWN);
	m_friendlyCitiesButton->SetActionFuncAndCookie(CityFilterButton, (void *)TRADE_CITIES_FRIENDLY);
	m_allCitiesButton->SetActionFuncAndCookie(CityFilterButton, (void *)TRADE_CITIES_ALL);

	m_showCities = TRADE_CITIES_OWN;
	m_ownCitiesButton->SetToggleState(true);

	m_citiesSlider = (C3Slider *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Market.TabPanel.Filters.CitiesSlider");
	m_citiesSlider->SetActionFuncAndCookie(NumCitiesSlider, NULL);

	m_numCitiesLabel = (ctp2_Static *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Market.TabPanel.Filters.NumCities");

	SetNumCities(1);

	*err = AUI_ERRCODE_OK;
}

void TradeManager::TabCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != ctp2_Tab::ACTION_ACTIVATED) return;
	
	if(sint32(cookie) > 0){ // avoid updating market tab, which can take very long and only needs to be updated in case of cancelling a route
	    s_tradeManager->Update();
	    }
}

TradeManager::~TradeManager()
{
	aui_Ldl::DeleteHierarchyFromRoot(s_tradeManagerBlock);
	m_window = NULL;

	aui_Ldl::DeleteHierarchyFromRoot(s_tradeAdviceBlock);
	m_adviceWindow = NULL;

	m_createData.DeleteAll();
}

AUI_ERRCODE TradeManager::Initialize()
{
	if(s_tradeManager)
		return AUI_ERRCODE_OK;

	AUI_ERRCODE err = AUI_ERRCODE_OK;
	s_tradeManager = new TradeManager(&err);
	Assert(err == AUI_ERRCODE_OK);

	return err;
}

AUI_ERRCODE TradeManager::Cleanup()
{
	if(s_tradeManager) {
		Hide();

		delete s_tradeManager;
		s_tradeManager = NULL;
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE TradeManager::Display()
{
	if(!s_tradeManager)
		Initialize();

	Assert(s_tradeManager);
	if(!s_tradeManager) {
		return AUI_ERRCODE_HACK;
	}

	AUI_ERRCODE err = AUI_ERRCODE_INVALIDPARAM;
	Assert(s_tradeManager->m_window);
	if(s_tradeManager->m_window && !g_c3ui->GetWindow(s_tradeManager->m_window->Id())) {
		err = g_c3ui->AddWindow(s_tradeManager->m_window);
		Assert(err == AUI_ERRCODE_OK);
		if(err == AUI_ERRCODE_OK) {
			err = s_tradeManager->m_window->Show();
		}

		if(s_tradeManager->m_adviceWindow) {
			err = g_c3ui->AddWindow(s_tradeManager->m_adviceWindow);
			if(err == AUI_ERRCODE_OK) {
				err = s_tradeManager->m_adviceWindow->Show();
			}
		}

//		ctp2_Tab *tab = (ctp2_Tab *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Market");
//		ctp2_TabGroup *tabGroup = (ctp2_TabGroup *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs");






		s_tradeManager->Update();
	}
	return err;
}

AUI_ERRCODE TradeManager::Hide()
{
	if(!s_tradeManager)
		return AUI_ERRCODE_OK;

	Assert(s_tradeManager->m_window);
	if(!s_tradeManager->m_window)
		return AUI_ERRCODE_INVALIDPARAM;

	if(s_tradeManager->m_adviceWindow) {
		g_c3ui->RemoveWindow(s_tradeManager->m_adviceWindow->Id());
	}

	return g_c3ui->RemoveWindow(s_tradeManager->m_window->Id());
}

void TradeManager::SetMode(TRADE_MANAGER_MODE mode)
{
//	MBCHAR *tradeBlock="TradeManager:TradeTabs";
	ctp2_Tab *market = (ctp2_Tab *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Market");
	ctp2_Tab *summary = (ctp2_Tab *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Summary");
	ctp2_Tab *import = (ctp2_Tab *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Import");
	ctp2_TabGroup *group = (ctp2_TabGroup *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs");

	switch(mode) {
		case TRADE_MANAGER_MARKET:
			market->Activate();
			summary->Deactivate();
			import->Deactivate();
			group->SelectTab(market);
			break;
		case TRADE_MANAGER_SUMMARY:
			market->Deactivate();
			summary->Activate();
			import->Deactivate();
			group->SelectTab(summary);
			break;
		case TRADE_MANAGER_IMPORT:
			market->Deactivate();
			summary->Deactivate();
			import->Activate();
			group->SelectTab(import);
			break;
		default:
			Assert(FALSE);
			break;
	}
}

void TradeManager::Notify()
{
	if(s_tradeManager && g_c3ui->GetWindow(s_tradeManager->m_window->Id())) {
		s_tradeManager->Update();
	}
}

void TradeManager::Update()
{
	if(s_tradeManager->m_window->IsHidden()) // only update if window is visible to avoid spending time on UpdateCreateList e.g. in case AI declares war and all trade routes get deleted which in turn leads to freeing of caravans which is bound to TradeManager::Update() due to the infos in the advice window
	    return;
	
	ctp2_Tab *market = (ctp2_Tab *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Market");
	ctp2_Tab *summary = (ctp2_Tab *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Summary");
	ctp2_Tab *import = (ctp2_Tab *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs.Import");
	ctp2_TabGroup *group = (ctp2_TabGroup *)aui_Ldl::GetObject(s_tradeManagerBlock, "TradeTabs");
	
	if(market && group->GetCurrentTab() == market)
	    UpdateCreateList(g_selected_item->GetVisiblePlayer());
	if(summary && group->GetCurrentTab() == summary)
	    UpdateSummaryList(m_summaryList, true);
	if(import && group->GetCurrentTab() == import)
	    UpdateSummaryList(m_importList, false);
	
	UpdateAdviceWindow();
}

void TradeManager::UpdateCreateList(const PLAYER_INDEX & player_id)
{
	Assert(player_id >= 0 && player_id < k_MAX_PLAYERS);
	if(player_id < 0 || player_id >= k_MAX_PLAYERS) return;

	Player *    p = g_player[player_id];
	Assert(p);
	if (!p) return;

	sint32 d;
	Unit maxCity[k_MAX_CITIES_PER_GOOD];

	m_createList->Clear();
	m_createList->BuildListStart();
	m_createData.DeleteAll();

	for (sint32 c = 0; c < p->m_all_cities->Num(); c++)
    {
		Unit city = p->m_all_cities->Access(c);

		for (sint32 g = 0; g < g_theResourceDB->NumRecords(); g++)
        {
			if(city.CD()->IsLocalResource(g)) { // only consider collected goods (not bought goods)

				sint32 op;
				sint32 maxPrice[k_MAX_CITIES_PER_GOOD];
				sint32 sellingPrice = -1;
				Unit curDestCity;

				sint32 i,j;
				for(i = 0; i < k_MAX_CITIES_PER_GOOD; i++) {
					maxCity[i].m_id = 0;
					maxPrice[i] = 0;
				}


				if(city.CD()->IsSellingResourceTo(g, curDestCity)) { // have already a route for g
					sellingPrice = tradeutil_GetTradeValue(player_id, curDestCity, g);

				//need to add something here where cities that have an improvement that needs a good will demand the good. May be increase the value of selling that good?
				}
				else {
					curDestCity.m_id = 0;
					sellingPrice = -1;
				}

				for(op = 1; op < k_MAX_PLAYERS; op++) { // determine player offering highest price for good g
					if(!g_player[op]) continue; // player gone/dead
					if(player_id != op && !p->HasContactWith(op)) continue; // player not yet known
					if(m_showCities == TRADE_CITIES_OWN && op != g_selected_item->GetVisiblePlayer()) continue;
					if ((m_showCities == TRADE_CITIES_ALL)			&&
						(op != g_selected_item->GetVisiblePlayer()) &&
						(AgreementMatrix::s_agreements.TurnsAtWar(player_id, op) >= 0) // no trade if at ware
					   )
						continue;

					if ((m_showCities == TRADE_CITIES_FRIENDLY)		&&
						(op != g_selected_item->GetVisiblePlayer()) &&
						(!AgreementMatrix::s_agreements.HasAgreement
							(player_id, op, PROPOSAL_TREATY_PEACE)
						)
					   )
						continue;

					if(Diplomat::GetDiplomat(op).GetEmbargo(player_id)) // no trade if embargo enacted
						continue;

					for(d = 0; d < g_player[op]->m_all_cities->Num(); d++) { // check all cities if suitabelfor trading
						Unit destCity = g_player[op]->m_all_cities->Access(d);
						if(!(destCity.IsValid())) continue;
						if(!(destCity.GetVisibility() & (1 << player_id))) continue; // city not visible/known
						if(destCity.m_id == city.m_id) continue; // exclude current city


						if(curDestCity.m_id == destCity.m_id) continue; // exclude standing trade route destination

						sint32 price = tradeutil_GetTradeValue(player_id, destCity, g); // # of gold
						for(i = 0; i < m_numCities; i++) {
							if(price > maxPrice[i]) { // determine best offer
								for(j = m_numCities - 1; j>= i; j--) {
									maxPrice[j] = maxPrice[j - 1];
									maxCity[j].m_id = maxCity[j - 1].m_id;
								}
								maxPrice[i] = price;
								maxCity[i] = destCity;
								break;
							}
						}
					}
				}

				for(i = 0; i < k_MAX_CITIES_PER_GOOD; i++) {
					if(maxPrice[i] > 0) {

						if(maxCity[i].m_id == curDestCity.m_id)
							continue;


						ctp2_ListItem * item =
                            (ctp2_ListItem *) aui_Ldl::BuildHierarchyFromRoot("CreateRouteItem");
						Assert(item);
						if(!item)
							break;

						CreateListData *data = new CreateListData;
						data->m_source = city;
						data->m_resource = g;
						data->m_destination = maxCity[i];
						data->m_price = maxPrice[i];
						data->m_caravans = tradeutil_GetTradeDistance(city, maxCity[i]);
						data->m_curDestination.m_id = curDestCity.m_id;

						m_createData.AddTail(data);
						item->SetUserData(data);

						if (ctp2_Static * origin = (ctp2_Static *)item->GetChildByIndex(k_CITY_COL_INDEX))
                        {
							MBCHAR name[k_MAX_NAME_LEN + 1];
							strncpy(name, city.GetName(), k_MAX_NAME_LEN);
							name[k_MAX_NAME_LEN] = 0;
							origin->TextReloadFont();
							origin->GetTextFont()->TruncateString(name, origin->Width());
							origin->SetText(name);
						}

						if (ctp2_Static * icon = (ctp2_Static *)item->GetChildByIndex(k_GOODICON_COL_INDEX))
                        {
							const char *iconname = g_theResourceDB->Get(g)->GetIcon()->GetIcon();
							if (stricmp(iconname, "NULL") == 0)
                            {
								iconname = NULL;
							}
							icon->SetImage(iconname);
						}

						if (ctp2_Static * good = (ctp2_Static *)item->GetChildByIndex(k_GOOD_COL_INDEX))
                        {
							good->SetText(g_theResourceDB->Get(g)->GetNameText());
							if (curDestCity.m_id != 0)
                            {
								good->SetTextColor(g_colorSet->GetColorRef(COLOR_RED));
							}
						}

						if (ctp2_Static * dest = (ctp2_Static *)item->GetChildByIndex(k_TOCITY_COL_INDEX))
                        {
							MBCHAR name[k_MAX_NAME_LEN + 1];
							strncpy(name, maxCity[i].GetName(), k_MAX_NAME_LEN);
							name[k_MAX_NAME_LEN] = 0;
							dest->TextReloadFont();
							dest->GetTextFont()->TruncateString(name, dest->Width());
							dest->SetText(name);
						}

						MBCHAR buf[20];
						sprintf(buf, "%d", maxPrice[i]);
						if (ctp2_Static * price = (ctp2_Static *)item->GetChildByIndex(k_PRICE_COL_INDEX))
                        {
							price->SetText(buf);
						}

						if (ctp2_Static * count = (ctp2_Static *)item->GetChildByIndex(k_CARAVANS_COL_INDEX))
                        {
							sprintf(buf, "%d", data->m_caravans);
							count->SetText(buf);
						}

						if (ctp2_Static * nation = (ctp2_Static *)item->GetChildByIndex(k_NATION_COL_INDEX))
                        {
							nation->SetDrawCallbackAndCookie
                                (DrawNationColumn, (void *)data->m_destination.GetOwner());
						}

						item->SetCompareCallback(CompareCreateItems);

						m_createList->AddItem(item);
					}
				}
			}
		}
	}
	m_createList->BuildListEnd();

	m_createButton->Enable(FALSE);
}

void TradeManager::UpdateAdviceWindow()
{
	Assert(m_adviceWindow);
	if(!m_adviceWindow) return;

	ctp2_Button *showButton = (ctp2_Button *)aui_Ldl::GetObject(s_tradeManagerBlock, "ShowAdviceButton");
	if(showButton) {
		if(g_c3ui->GetWindow(m_adviceWindow->Id())) {
			showButton->SetText(g_theStringDB->GetNameStr("str_ldl_TradeHideAdvisor"));
		} else {
			showButton->SetText(g_theStringDB->GetNameStr("str_ldl_TradeShowAdvisor"));
		}
	}

	MBCHAR buf[20];
	sint32 pl = g_selected_item->GetVisiblePlayer();
	if(!g_player[pl]) return;

	ctp2_Static *child = (ctp2_Static *)aui_Ldl::GetObject(s_tradeAdviceBlock, "Available");
	if(child) {
		sprintf(buf, "%d", g_player[pl]->m_tradeTransportPoints - g_player[pl]->m_usedTradeTransportPoints);
		child->SetText(buf);
	}

	child = (ctp2_Static *)aui_Ldl::GetObject(s_tradeAdviceBlock, "InUse");
	if(child) {
		sprintf(buf, "%d", g_player[pl]->m_usedTradeTransportPoints);
		child->SetText(buf);
	}

	sint32 i, totalProfit = 0, totalPiracy = 0, totalTransi = 0, totalRoutes = 0;
	for(i = 0; i < g_player[pl]->m_all_cities->Num(); i++) {
		Unit city = g_player[pl]->m_all_cities->Access(i);
		totalRoutes += city.CD()->GetTradeSourceList()->Num();
		totalProfit += city.CD()->CalculateGoldFromResources(); // takes piracy and wonder bonus into account
		totalPiracy += city.CD()->GetGoldLostToPiracy(); // takes wonder bonus into account
		totalTransi += city.CD()->GetGoldFromTransitRoutes(); // takes piracy into account
	}

	child = (ctp2_Static *)aui_Ldl::GetObject(s_tradeAdviceBlock, "Profit");
	if(child) {
		sprintf(buf, "%d", totalProfit);
		child->SetText(buf);
	}

	child = (ctp2_Static *)aui_Ldl::GetObject(s_tradeAdviceBlock, "Piracy");
	if(child) {
		sprintf(buf, "%d", totalPiracy);
		child->SetText(buf);
	}

	child = (ctp2_Static *)aui_Ldl::GetObject(s_tradeAdviceBlock, "Transit");
	if(child) {
		sprintf(buf, "%d", totalTransi);
		child->SetText(buf);
	}

	child = (ctp2_Static *)aui_Ldl::GetObject(s_tradeAdviceBlock, "Routes");
	if(child) {
		sprintf(buf, "%d", totalRoutes);
		child->SetText(buf);
	}

	UpdateAdviceText();
}

void TradeManager::UpdateAdviceText()
{
	ctp2_HyperTextBox *advice = (ctp2_HyperTextBox *)aui_Ldl::GetObject(s_tradeAdviceBlock, "Advice");
	Assert(advice);
	if(advice) {
		Assert(m_createList);

		SlicContext sc;

		Player *p = g_player[g_selected_item->GetVisiblePlayer()];

		if(m_createList) {
			ctp2_ListItem *selItem = (ctp2_ListItem *)m_createList->GetSelectedItem();
			if(selItem && !m_createList->IsHidden()) {
				CreateListData *data = (CreateListData *)selItem->GetUserData();
				sc.AddCity(data->m_source);
				sc.AddCity(data->m_destination);
				sc.AddGood(data->m_resource);
				sc.AddGold(data->m_price);
				sc.AddInt(data->m_caravans);

				MBCHAR interp[k_MAX_NAME_LEN];
				interp[0] = 0;

				stringutils_Interpret(g_theStringDB->GetNameStr("SELECTED_TRADE_ADVICE"),
									  sc, interp);

				if(p) {
					if(data->m_caravans > (p->m_tradeTransportPoints - p->m_usedTradeTransportPoints)) {
						SlicContext sc2;
						sc2.AddInt(data->m_caravans - (p->m_tradeTransportPoints - p->m_usedTradeTransportPoints));

						strcat(interp + strlen(interp), "  ");

						stringutils_Interpret(g_theStringDB->GetNameStr("NEED_MORE_CARAVANS"),
											  sc2, interp + strlen(interp));
					}
				}

				advice->SetHyperText(interp);
			} else {

				if(p) {
					PointerList<CreateListData>::Walker walk(&m_createData);
					CreateListData *maxData = NULL;
					while(walk.IsValid()) {
						CreateListData *data = walk.GetObj();
						if(data->m_curDestination.m_id != 0) {

							walk.Next();
							continue;
						}
						if(data->m_caravans <= (p->m_tradeTransportPoints - p->m_usedTradeTransportPoints)) {
							if(!maxData || data->m_price > maxData->m_price) {
								maxData = data;
							}
						}
						walk.Next();
					}

					MBCHAR interp[k_MAX_NAME_LEN];
					interp[0] = 0;

					if(maxData) {
						SlicContext sc;
						sc.AddCity(maxData->m_source);
						sc.AddCity(maxData->m_destination);
						sc.AddGood(maxData->m_resource);

						stringutils_Interpret(g_theStringDB->GetNameStr("CREATE_ROUTE_ADVICE"),
											  sc, interp);
					} else if(m_createData.GetCount() > 0) {
						strcpy(interp, g_theStringDB->GetNameStr("BUILD_MORE_CARAVANS"));
					} else {
						strcpy(interp, g_theStringDB->GetNameStr("MAXIMUM_TRADE_EFFICIENCY"));
					}

					advice->SetHyperText(interp);
				}
			}
		}
	}
}

void TradeManager::UpdateSummaryList(ctp2_ListBox *summaryList, bool source)
{
	sint32 pl = g_selected_item->GetVisiblePlayer();
	Assert(pl >= 0 && pl < k_MAX_PLAYERS);
	if(pl < 0 || pl >= k_MAX_PLAYERS) return;

	Assert(g_player[pl]);
	if(!g_player[pl]) return;

	Player *p = g_player[pl];
	Unit maxCity;

	summaryList->Clear();
	summaryList->BuildListStart();

	for (sint32 c = 0; c < p->m_all_cities->Num(); c++)
	    {
	    Unit city = p->m_all_cities->Access(c);
	    
	    sint32 numTradeRoutes= source ? city.CD()->GetTradeSourceList()->Num() : city.CD()->GetTradeDestinationList()->Num();
	    for (sint32 r = 0; r < numTradeRoutes; r++)
		{
		ctp2_ListItem * item = (ctp2_ListItem *)
		    aui_Ldl::BuildHierarchyFromRoot("TradeSummaryItem");
		Assert(item);
		if(!item)
		    break;
		
		TradeRoute route = source ? city.CD()->GetTradeSourceList()->Access(r) : city.CD()->GetTradeDestinationList()->Access(r) ; // routes from TradeSourceList and TradeDestinationList are expected to be active, so skipping inactive routes should not be necessary here
		
		if (ctp2_Static * origin = (ctp2_Static *)item->GetChildByIndex(k_CITY_COL_SUM_INDEX))
		    {
		    MBCHAR name[k_MAX_NAME_LEN + 1];
		    strncpy(name, route.GetSource().GetName(), k_MAX_NAME_LEN);
		    name[k_MAX_NAME_LEN] = 0;
		    origin->TextReloadFont();
		    origin->GetTextFont()->TruncateString(name, origin->Width());
		    origin->SetText(name);
		    }
		
		ROUTE_TYPE rtype;
		sint32 resource;
		route.GetSourceResource(rtype, resource);
		
		if (ctp2_Static * icon = (ctp2_Static *)item->GetChildByIndex(k_GOODICON_COL_SUM_INDEX))
		    {
		    if (rtype == ROUTE_TYPE_RESOURCE)
			{
			const MBCHAR *imageName = g_theResourceDB->Get(resource)->GetIcon()->GetIcon();
			if (stricmp(imageName, "NULL") == 0)
			    {
			    icon->SetImage(NULL);
			    }
			else
			    {
			    icon->SetImage(g_theResourceDB->Get(resource)->GetIcon()->GetIcon());
			    }
			}
		    }
		
		if (ctp2_Static * good = (ctp2_Static *)item->GetChildByIndex(k_GOOD_COL_SUM_INDEX))
		    {
		    if (rtype == ROUTE_TYPE_RESOURCE)
			{
			good->SetText(g_theResourceDB->Get(resource)->GetNameText());
			}
		    else
			{
			good->SetText(g_theStringDB->GetNameStr("ROUTE_TYPE_FOOD"));
			}
		    }
		
		if (ctp2_Static * dest = (ctp2_Static *)item->GetChildByIndex(k_TOCITY_COL_SUM_INDEX))
		    {
		    MBCHAR name[k_MAX_NAME_LEN + 1];
		    Unit dCity = route.GetDestination();
		    strncpy(name, dCity.GetName(), k_MAX_NAME_LEN);
		    name[k_MAX_NAME_LEN] = 0;
		    dest->TextReloadFont();
		    dest->GetTextFont()->TruncateString(name, dest->Width());
		    dest->SetText(name);
		    }
		
		if (ctp2_Static * piracy = (ctp2_Static *)item->GetChildByIndex(k_PIRACY_COL_SUM_INDEX))
		    {
		    piracy->SetDrawCallbackAndCookie(DrawPiracyColumn, (void *)route.m_id);
		    }
		
		MBCHAR buf[20];
		if(rtype == ROUTE_TYPE_RESOURCE) {
		    sprintf(buf, "%d", route->GetValue());
		    } else {
		    strcpy(buf, "---");
		    }
		
		if (ctp2_Static * price = (ctp2_Static *)item->GetChildByIndex(k_PRICE_COL_SUM_INDEX))
		    {
		    price->SetText(buf);
		    }
		
		if (ctp2_Static * count = (ctp2_Static *)item->GetChildByIndex(k_CARAVANS_COL_SUM_INDEX))
		    {
		    if(source){
			sprintf(buf, "%d", route.GetCost());
			}
		    else{
			sprintf(buf, "%d,%d,%d",
			    g_theResourceDB->Get(resource)->GetFood(),
			    g_theResourceDB->Get(resource)->GetProduction(),
			    g_theResourceDB->Get(resource)->GetGold()
			    );
			}
		    count->SetText(buf);
		    }
		
		if (ctp2_Static * nation = (ctp2_Static *)item->GetChildByIndex(k_NATION_COL_SUM_INDEX))
		    {
		    nation->SetDrawCallbackAndCookie(DrawNationColumn,
			source ? (void *)route.GetDestination().GetOwner() : (void *)route.GetSource().GetOwner());
		    }
		
		item->SetUserData((void *)route.m_id);
		item->SetCompareCallback(CompareSummaryItems);
		
		summaryList->AddItem(item);
		}
	    }
	summaryList->BuildListEnd();
	
	m_breakButton->Enable(FALSE);
	m_breakImpBut->Enable(FALSE);
}

void TradeManager::Close(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	TradeManager::Hide();
}

void TradeManager::CreateRoute(aui_Control *control, uint32 action, uint32 uidata, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_tradeManager);
	if(!s_tradeManager) return;

//	ctp2_Static *market = (ctp2_Static *)aui_Ldl::GetObject(s_tradeManagerBlock, "Market");
	switch(sint32(cookie)) {
	case 0: {
		Assert(s_tradeManager->m_createList);
		if(!s_tradeManager->m_createList) return;

		ctp2_ListItem *item = (ctp2_ListItem *)s_tradeManager->m_createList->GetSelectedItem();
		if(!item) return;

		CreateListData *data = (CreateListData *)item->GetUserData();

		Assert(data);
		if(!data) return;

		Assert(data->m_source.IsValid());
		if(!data->m_source.IsValid()) return;

		Assert(data->m_destination.IsValid());
		if(!data->m_destination.IsValid()) return;

		Assert(g_player[data->m_source.GetOwner()]);
		if(!g_player[data->m_source.GetOwner()]) return;

		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SendGood,
							   GEA_Int, data->m_resource,
							   GEA_City, data->m_source,
							   GEA_City, data->m_destination,
							   GEA_End);
		break;
	    }
	case 1:
	case 2: {
		bool source = sint32(cookie) == 1;
		ctp2_ListBox *summaryList;
		summaryList= source ? s_tradeManager->m_summaryList : s_tradeManager->m_importList;

		Assert(summaryList);
		if(!summaryList) return;

		ctp2_ListItem *item = (ctp2_ListItem *)summaryList->GetSelectedItem();
		if(!item) return;

		TradeRoute route((uint32)item->GetUserData());
		Assert(route.IsValid());
		if(!route.IsValid()) return;

		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillTradeRoute,
							   GEA_TradeRoute, route.m_id,
							   GEA_Int,
							   source ? CAUSE_KILL_TRADE_ROUTE_SENDER_KILLED : CAUSE_KILL_TRADE_ROUTE_RECIPIENT_KILLED ,
							   GEA_End);

		if(g_network.IsClient()) {
			g_network.SendAction(new NetAction(NET_ACTION_CANCEL_TRADE_ROUTE, (uint32)route));
		break;
		}
	    }
	} //switch
}

void TradeManager::ShowAdvice(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	if(!s_tradeManager->m_adviceWindow) return;

	if(g_c3ui->GetWindow(s_tradeManager->m_adviceWindow->Id())) {
		g_c3ui->RemoveWindow(s_tradeManager->m_adviceWindow->Id());
	} else {
		g_c3ui->AddWindow(s_tradeManager->m_adviceWindow);
	}
	s_tradeManager->UpdateAdviceWindow();
}

void TradeManager::Summary(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_tradeManager);
	if(!s_tradeManager)
		return;

	ctp2_Static *market = (ctp2_Static *)aui_Ldl::GetObject(s_tradeManagerBlock, "Market");
	Assert(market);
	if(market && market->IsHidden()) {
		s_tradeManager->SetMode(TRADE_MANAGER_MARKET);
	} else {
		s_tradeManager->SetMode(TRADE_MANAGER_SUMMARY);
	}
}

sint32 TradeManager::CompareCreateItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column)
{
	CreateListData *data1 = (CreateListData *)item1->GetUserData();
	CreateListData *data2 = (CreateListData *)item2->GetUserData();

	Assert(data1 && data2);
	if(!data1 || !data2) return 0;

	switch(column) {
		case k_CITY_COL_INDEX:
			Assert(data1->m_source.IsValid());
			Assert(data2->m_source.IsValid());
			if(data1->m_source.IsValid() && data2->m_source.IsValid()) {
				return stricmp(data1->m_source.GetName(), data2->m_source.GetName());
			}
			return 0;
		case k_GOOD_COL_INDEX:
		case k_GOODICON_COL_INDEX:
			return stricmp(g_theResourceDB->Get(data1->m_resource)->GetNameText(),
						   g_theResourceDB->Get(data2->m_resource)->GetNameText());
		case k_TOCITY_COL_INDEX:
			Assert(data1->m_destination.IsValid());
			Assert(data2->m_destination.IsValid());
			if(data1->m_destination.IsValid() && data2->m_destination.IsValid()) {
				return stricmp(data1->m_destination.GetName(), data2->m_destination.GetName());
			}
			return 0;
		case k_PRICE_COL_INDEX:
			return data1->m_price - data2->m_price;
		case k_CARAVANS_COL_INDEX:
			return data1->m_caravans - data2->m_caravans;
		case k_NATION_COL_INDEX:
			Assert(data1->m_destination.IsValid());
			Assert(data2->m_destination.IsValid());
			if(data1->m_destination.IsValid() && data2->m_destination.IsValid()) {
				if(data1->m_destination.GetOwner() == data2->m_destination.GetOwner()) {
					return stricmp(data1->m_destination.GetName(), data2->m_destination.GetName());
				} else {
					return data1->m_destination.GetOwner() - data2->m_destination.GetOwner();
				}
			}
			return 0;
		default:
			Assert(FALSE);
			return 0;
	}
}

sint32 TradeManager::CompareSummaryItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column)
{
	TradeRoute route1 = TradeRoute(uint32(item1->GetUserData()));
	TradeRoute route2 = TradeRoute(uint32(item2->GetUserData()));

	Assert(route1.IsValid());
	Assert(route2.IsValid());
	if(!route1.IsValid() || !route2.IsValid()) {
		return 0;
	}

	ROUTE_TYPE rtype1, rtype2;
	sint32 resource1, resource2;

	route1.GetSourceResource(rtype1, resource1);
	route2.GetSourceResource(rtype2, resource2);

	switch(column) {
		case k_CITY_COL_SUM_INDEX:
			Assert(route1.GetSource().IsValid());
			Assert(route2.GetSource().IsValid());
			if(route1.GetSource().IsValid() && route2.GetSource().IsValid()) {
				return stricmp(route1.GetSource().GetName(), route2.GetSource().GetName());
			}
			return 0;
		case k_PIRACY_COL_SUM_INDEX:
		{
			sint32 pl1 = -1;
			sint32 pl2 = -1;
			if(route1->IsBeingPirated()) {
				pl1 = route1->GetPiratingArmy().GetOwner();
			}

			if(route2->IsBeingPirated()) {
				pl2 = route2->GetPiratingArmy().GetOwner();
			}

			return pl1 - pl2;
		}

		case k_GOOD_COL_SUM_INDEX:
		case k_GOODICON_COL_SUM_INDEX:
		{
			const MBCHAR *str1 = rtype1 == ROUTE_TYPE_RESOURCE ? g_theResourceDB->Get(resource1)->GetNameText() :
				g_theStringDB->GetNameStr("ROUTE_TYPE_FOOD");
			const MBCHAR *str2 = rtype2 == ROUTE_TYPE_RESOURCE ? g_theResourceDB->Get(resource2)->GetNameText() :
				g_theStringDB->GetNameStr("ROUTE_TYPE_FOOD");
			return stricmp(str1, str2);
		}
		case k_TOCITY_COL_SUM_INDEX:
			Assert(route1.GetDestination().IsValid());
			Assert(route2.GetDestination().IsValid());
			if(route1.GetDestination().IsValid() && route2.GetDestination().IsValid()) {
				return stricmp(route1.GetDestination().GetName(), route2.GetDestination().GetName());
			}
			return 0;
		case k_PRICE_COL_SUM_INDEX:
			return route1->GetValue() - route2->GetValue();
		case k_CARAVANS_COL_SUM_INDEX:
			return route1.GetCost() - route2.GetCost();
		case k_NATION_COL_SUM_INDEX:
			Assert(route1.GetDestination().IsValid());
			Assert(route2.GetDestination().IsValid());
			if(route1.GetDestination().IsValid() && route2.GetDestination().IsValid()) {
				if(route1.GetDestination().GetOwner() == route2.GetDestination().GetOwner()) {
					return stricmp(route1.GetDestination().GetName(), route2.GetDestination().GetName());
				} else {
					return route1.GetDestination().GetOwner() - route2.GetDestination().GetOwner();
				}
			}
			return 0;
		default:
			Assert(FALSE);
			return 0;
	}
}

AUI_ERRCODE TradeManager::DrawNationColumn(ctp2_Static *control,
										   aui_Surface *surface,
										   RECT &rect,
										   void *cookie)
{
	sint32 player = (sint32)cookie;
	Assert(g_colorSet);
	if(!g_colorSet)
		return AUI_ERRCODE_INVALIDPARAM;
	rect.left += 2;
	rect.top += 2;
	rect.right -= 2;
	rect.bottom -= 2;
	g_c3ui->TheBlitter()->ColorBlt16(surface, &rect, 0, 0);

	rect.left += 8;
	rect.top += 2;
	rect.right -= 8;
	rect.bottom -= 2;

	return g_c3ui->TheBlitter()->ColorBlt16(surface, &rect, g_colorSet->GetPlayerColor(player), 0);
}

AUI_ERRCODE TradeManager::DrawPiracyColumn(ctp2_Static *control,
										   aui_Surface *surface,
										   RECT &rect,
										   void *cookie)
{
	TradeRoute route((uint32)cookie);
	if(!route.IsValid()) return AUI_ERRCODE_OK;

	Pixel16 color = 0xffff;

	rect.left += 2;
	rect.top += 2;
	rect.right -= 2;
	rect.bottom -= 2;

	if(route->IsBeingPirated()) {
		color = g_colorSet->GetPlayerColor(route->GetPiratingArmy().GetOwner());
	}
	return g_c3ui->TheBlitter()->ColorBlt16(surface, &rect, color, 0);
}

void TradeManager::ListSelect(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_LISTBOX_ACTION_SELECT && action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {
		return;
	}

	Assert(s_tradeManager);
	if(!s_tradeManager) return;

	ctp2_ListBox *lb = (ctp2_ListBox *)control;
	Assert(lb == s_tradeManager->m_createList);
	if(lb != s_tradeManager->m_createList) return;

	s_tradeManager->UpdateAdviceText();

	ctp2_ListItem *item = (ctp2_ListItem *)lb->GetSelectedItem();
	bool canCreate = false;
	if(item) {
		CreateListData *data = (CreateListData *)item->GetUserData();
		Assert(data);
		if(data) {
			Player *pl = g_player[g_selected_item->GetVisiblePlayer()];
			Assert(pl);
			if(pl && (data->m_caravans <= pl->m_tradeTransportPoints - pl->m_usedTradeTransportPoints)) {
				canCreate = true;
			}
		}
	}
	s_tradeManager->m_createButton->Enable(canCreate);

	if(action == AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {

		CreateRoute(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);
	}
}

void TradeManager::SummaryListSelect(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_LISTBOX_ACTION_SELECT && action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {
		return;
	}

	Assert(s_tradeManager);
	if(!s_tradeManager) return;

	ctp2_ListBox *lb = (ctp2_ListBox *)control;
	bool source = lb == s_tradeManager->m_summaryList;
	if(source){
	    Assert(lb == s_tradeManager->m_summaryList);
	    if(lb != s_tradeManager->m_summaryList) return;
	    }
	else {
	    Assert(lb == s_tradeManager->m_importList);
	    if(lb != s_tradeManager->m_importList) return;
	    }

	ctp2_ListItem *item = (ctp2_ListItem *)lb->GetSelectedItem();
	if(item) {
	        source ? 
		    s_tradeManager->m_breakButton->Enable(TRUE) :
		    s_tradeManager->m_breakImpBut->Enable(TRUE);
	} else {
	        source ? 
		    s_tradeManager->m_breakButton->Enable(FALSE) :
		    s_tradeManager->m_breakImpBut->Enable(FALSE);
	}
	if(action == AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {

		CreateRoute(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);
	}
}

STDEHANDLER(TradeManagerSendGoodEvent)
{
	if(!s_tradeManager) return GEV_HD_Continue;

	Unit source, destination;
	sint32 good;
	if(!args->GetInt(0, good)) return GEV_HD_Continue;
	if(!args->GetCity(0, source)) return GEV_HD_Continue;
	if(!args->GetCity(1, destination)) return GEV_HD_Continue;

	if(source.GetOwner() == g_selected_item->GetVisiblePlayer() ||
	   destination.GetOwner() == g_selected_item->GetVisiblePlayer()) {
		s_tradeManager->Update();
	}
	return GEV_HD_Continue;
}

AUI_ACTION_BASIC(UpdateTradeAction);

void UpdateTradeAction::Execute(aui_Control *control, uint32 action, uint32 data)
{

	if(s_tradeManager) {
		s_tradeManager->Update();
	}
}

STDEHANDLER(TradeManagerKillRouteEvent)
{
	if(!s_tradeManager) return GEV_HD_Continue;

	TradeRoute route;
	if(!args->GetTradeRoute(0, route)) return GEV_HD_Continue;

	if((route.GetSource().IsValid() && route.GetSource().GetOwner() == g_selected_item->GetVisiblePlayer()) ||
	   (route.GetDestination().IsValid() && route.GetDestination().GetOwner() == g_selected_item->GetVisiblePlayer())) { // check appears to be robust concerning inactive routes


		g_c3ui->AddAction(new UpdateTradeAction);
	}
	return GEV_HD_Continue;
}

void TradeManager::InitializeEvents()
{
	g_gevManager->AddCallback(GEV_SendGood, GEV_PRI_Post, &s_TradeManagerSendGoodEvent);
	g_gevManager->AddCallback(GEV_KillTradeRoute, GEV_PRI_Pre, &s_TradeManagerKillRouteEvent);
}

void TradeManager::CleanupEvents()
{
}

void TradeManager::FilterButtonActivated(aui_Control *control)
{
	if(control == m_ownCitiesButton) {
		m_showCities = TRADE_CITIES_OWN;
		m_ownCitiesButton->SetToggleState(true);
		m_friendlyCitiesButton->SetToggleState(false);
		m_allCitiesButton->SetToggleState(false);
	} else if(control == m_friendlyCitiesButton) {
		m_showCities = TRADE_CITIES_FRIENDLY;
		m_ownCitiesButton->SetToggleState(false);
		m_friendlyCitiesButton->SetToggleState(true);
		m_allCitiesButton->SetToggleState(false);
	} else if(control == m_allCitiesButton) {
		m_showCities = TRADE_CITIES_ALL;
		m_ownCitiesButton->SetToggleState(false);
		m_friendlyCitiesButton->SetToggleState(false);
		m_allCitiesButton->SetToggleState(true);
	} else {

		Assert(FALSE);
	}

	UpdateCreateList(g_selected_item->GetVisiblePlayer());
}

void TradeManager::CityFilterButton(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	if(!s_tradeManager) return;
	s_tradeManager->FilterButtonActivated(control);
}

void TradeManager::SetNumCities(sint32 num)
{
	Assert(num <= k_MAX_CITIES_PER_GOOD);
    m_numCities = std::min<sint32>(num, k_MAX_CITIES_PER_GOOD);

	char buf[10];
	sprintf(buf, "%d", m_numCities);
	m_numCitiesLabel->SetText(buf);
}

void TradeManager::NumCitiesSlider(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(!s_tradeManager) return;
	switch(action) {
		case AUI_RANGER_ACTION_VALUECHANGE:
			s_tradeManager->SetNumCities(s_tradeManager->m_citiesSlider->GetValueX() + 1);
			break;
		case AUI_RANGER_ACTION_RELEASE:
			s_tradeManager->UpdateCreateList(g_selected_item->GetVisiblePlayer());
			break;
	}
}
