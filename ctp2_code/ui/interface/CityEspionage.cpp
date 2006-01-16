//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The city espionage scree
// Id           : $Id:$
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "cityespionage.h"

#include "aui_ldl.h"
#include "aui_uniqueid.h"

#include "ctp2_Window.h"
#include "ctp2_Menu.h"
#include "ctp2_button.h"
#include "ctp2_Static.h"
#include "ctp2_dropdown.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "ctp2_Tab.h"
#include "ctp2_TabButton.h"
#include "ctp2_TabGroup.h"
#include "ctp2_spinner.h"

#include "c3ui.h"

#include "diplomacywindow.h"
#include "diplomacyutil.h"
#include "Diplomat.h"

#include "SelItem.h"

#include "StrDB.h"
#include "DiplomacyProposalRecord.h"
#include "DiplomacyThreatRecord.h"

#include "AdvanceRecord.h"
#include "Unit.h"
#include "SlicObject.h"
#include "stringutils.h"

#include "player.h"
#include "Civilisation.h"
#include "UnitDynArr.h"

#include "GameEventUser.h"
#include "Events.h"
#include "BuildingRecord.h"
#include "UnitData.h"
#include "citywindow.h"
#include "WonderRecord.h"
#include "cellunitlist.h"
#include "World.h"
#include "Cell.h"
#include "UnitRecord.h"
#include "IconRecord.h"

static CityEspionage *s_CityEspionage;
static MBCHAR *s_CityEspionageBlock = "CityEspionage";

extern C3UI *g_c3ui;
ctp2_Button	*CityEspionage::m_cancelButton = NULL;

CityEspionage::CityEspionage(AUI_ERRCODE *err)
{
	*err = AUI_ERRCODE_OK;
	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_CityEspionageBlock);
	Assert(m_window);
	if(!m_window) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	m_cancelButton = (ctp2_Button *)aui_Ldl::GetObject(s_CityEspionageBlock, "DialogBackground.CancelButton");
	if(m_cancelButton) m_cancelButton->SetActionFuncAndCookie(CancelCallback, NULL);

}

CityEspionage::~CityEspionage()
{
	if(m_window) {
		aui_Ldl::DeleteHierarchyFromRoot(s_CityEspionageBlock);
	}
}

AUI_ERRCODE CityEspionage::Initialize()
{
	
	if(s_CityEspionage)
		return AUI_ERRCODE_OK;

	
	AUI_ERRCODE err = AUI_ERRCODE_OK;
	s_CityEspionage = new CityEspionage(&err);

	Assert(err == AUI_ERRCODE_OK);

	return err;
}

AUI_ERRCODE CityEspionage::Cleanup()
{
	if(s_CityEspionage) {
		Hide();

		delete s_CityEspionage;
		s_CityEspionage = NULL;
	}
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE CityEspionage::Display(Unit *c)
{
	if(!s_CityEspionage)
		Initialize();

	Assert(s_CityEspionage);
	if(!s_CityEspionage) {
		return AUI_ERRCODE_HACK;
	}
	

	AUI_ERRCODE err = AUI_ERRCODE_INVALIDPARAM;

	Assert(s_CityEspionage->m_window);
	if(s_CityEspionage->m_window) {
		err = g_c3ui->AddWindow(s_CityEspionage->m_window);
		Assert(err == AUI_ERRCODE_OK);
		if(err == AUI_ERRCODE_OK) {
			err = s_CityEspionage->m_window->Show();
		}

	
		ctp2_Static *st;
		SlicObject so;
		Unit city;
		if(c==NULL)
		{
			if(!g_selected_item->GetSelectedCity(city)) 
			{
				return err;
			}
		}
		else
		{
			city=*c;
		}
		so.AddPlayer(city.GetOwner());
		so.AddCity(city);

		MBCHAR interp[k_MAX_NAME_LEN];

		
		interp[0] = 0;
		stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_Espionage_City_Label"), so, interp);
		st = (ctp2_Static *)aui_Ldl::GetObject(s_CityEspionageBlock, "DialogBackground.CityLabel");
		st->SetText(interp);

		
		interp[0] = 0;
		stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_Espionage_Population_Label"), so, interp);
		st = (ctp2_Static *)aui_Ldl::GetObject(s_CityEspionageBlock, "DialogBackground.PopulationLabel");
		st->SetText(interp);

		
		interp[0] = 0;
		stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_Espionage_Happiness_Label"), so, interp);
		st = (ctp2_Static *)aui_Ldl::GetObject(s_CityEspionageBlock, "DialogBackground.HapinessLabel");
		st->SetText(interp);

		
		interp[0] = 0;
		stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_Espionage_Building_Label"), so, interp);
		st = (ctp2_Static *)aui_Ldl::GetObject(s_CityEspionageBlock, "DialogBackground.Building");
		st->SetText(interp);

		
		interp[0] = 0;
		stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_Espionage_CompleteIn_Label"), so, interp);
		st = (ctp2_Static *)aui_Ldl::GetObject(s_CityEspionageBlock, "DialogBackground.CompleteIn");
		st->SetText(interp);

		
		interp[0] = 0;
		stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_Espionage_CityIncome_Label"), so, interp);
		st = (ctp2_Static *)aui_Ldl::GetObject(s_CityEspionageBlock, "DialogBackground.CityIncome");
		st->SetText(interp);

		
		interp[0] = 0;
		stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_Espionage_GoldInTrade_Label"), so, interp);
		st = (ctp2_Static *)aui_Ldl::GetObject(s_CityEspionageBlock, "DialogBackground.GoldFromTrade");
		st->SetText(interp);

		
		ctp2_ListBox *m_inventoryList = (ctp2_ListBox *)aui_Ldl::GetObject(s_CityEspionageBlock, "DialogBackground.InventoryList");
		Assert(m_inventoryList);
		if(m_inventoryList) {
			
			m_inventoryList->Clear();

			
			sint32 i;
			for(i = 0; i < g_theBuildingDB->NumRecords(); i++) {
				if(city->GetImprovements() & ((uint64)1 << (uint64)i)) {
					ctp2_ListItem *item;
					item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ce_InventoryListItem");
					Assert(item);
					if(item) {

						
						item->SetText(g_theBuildingDB->Get(i)->GetNameText());
						InventoryItemInfo *info = new InventoryItemInfo(true, i);
						item->SetUserData(info);
						m_inventoryList->AddItem(item);
					}
				}
			}

			
			for(i = 0; i < g_theWonderDB->NumRecords(); i++) {
				if(city->GetCityData()->GetBuiltWonders() & ((uint64)1 << (uint64)i)) {
					ctp2_ListItem *item;
					item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ce_InventoryListItem");
					Assert(item);
					if(item) {

						
						item->SetText(g_theWonderDB->Get(i)->GetNameText());
						InventoryItemInfo *info = new InventoryItemInfo(false, i);
						item->SetUserData(info);
						m_inventoryList->AddItem(item);
					}
				}
			}

			MapPoint pos;
			CellUnitList cellUnitList;
			city.GetPos(pos);
			Cell *cell = g_theWorld->GetCell(pos);
			cell->GetArmy(cellUnitList);

			
			sint32 multiIndex = 0;
			sint32 unitIndex = 0;

			
			while(multiIndex < 12) {
				sprintf(interp,"DialogBackground.FortifiedUnitsBox.Unit%i",multiIndex);
				st = (ctp2_Static *)aui_Ldl::GetObject(s_CityEspionageBlock, interp);
				
				if(unitIndex < cellUnitList.Num()) 
				{
					
					Unit &unit = cellUnitList[unitIndex++];
					
					if(st->IsDisabled())
						st->Enable(true);

					st->ExchangeImage(0, 0,	unit.GetDBRec()->GetDefaultIcon()->GetIcon());
				}
				else
				{	
					if(!st->IsDisabled())
						st->Enable(false);
					st->ExchangeImage(0, 0, NULL);
				}
				multiIndex++;
			}
		}
	}

	return err;
}

AUI_ERRCODE CityEspionage::Hide()
{
	if(!s_CityEspionage)
		return AUI_ERRCODE_OK;

	Assert(s_CityEspionage->m_window);
	if(!s_CityEspionage->m_window)
		return AUI_ERRCODE_INVALIDPARAM;

	s_CityEspionage->m_window->Hide();
	return g_c3ui->RemoveWindow(s_CityEspionage->m_window->Id());
}

void CityEspionage::CancelCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Hide();
}

