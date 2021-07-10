//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The city espionage screen
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Improved cleanup and structure.
//
//----------------------------------------------------------------------------
//
/// \file   CityEspionage.cpp
/// \brief  City espionage screen display (definitions)

#include "c3.h"
#include "cityespionage.h"

#include "aui_ldl.h"
#include "BuildingRecord.h"     // g_theBuildingDB
#include "c3ui.h"               // C3UI
#include "Cell.h"
#include "cellunitlist.h"
#include "citydata.h"
#include "citywindow.h"         // InventoryItemInfo
#include "ctp2_button.h"
#include "ctp2_listitem.h"
#include "ctp2_listbox.h"
#include "ctp2_Static.h"
#include "ctp2_Window.h"
#include "gstypes.h"            // k_MAX_ARMY_SIZE
#include "IconRecord.h"
#include "SlicObject.h"
#include "StrDB.h"              // g_theStringDB
#include "stringutils.h"
#include "Unit.h"
#include "UnitRecord.h"
#include "WonderRecord.h"       // g_theWonderDB
#include "World.h"              // g_theWorld

extern C3UI *   g_c3ui;

namespace
{
	/// City espionage data and window (singleton)
	CityEspionage * s_CityEspionage     = NULL;

	/// Block name for lookup of elements and layout from the ldl-file
	MBCHAR const *  LDL_BLOCK           = "CityEspionage";

	/// Set label text - after substitution of variables
	/// \param      a_Label     Name of label text to set
	/// \param      a_Text      Name of text item to set (may contain variables)
	/// \param      a_Context   Context, containing variable substitution values
	void SetLabelText
	(
	    MBCHAR const *      a_Label,
	    MBCHAR const *      a_Text,
	    SlicObject &        a_Context
	)
	{
		ctp2_Static * st = static_cast<ctp2_Static *>
		                        (aui_Ldl::GetObject(LDL_BLOCK, a_Label));

		if (st)
		{
		    MBCHAR parsedText[k_MAX_NAME_LEN];
		    parsedText[0] = 0;
		    stringutils_Interpret(g_theStringDB->GetNameStr(a_Text),
		                          a_Context,
		                          parsedText,
		                          k_MAX_NAME_LEN
		                         );
		    st->SetText(parsedText);
		}
	}
}

CityEspionage::CityEspionage()
:
    m_window        (NULL),
    m_cancelButton  (NULL),
    m_inventoryList (NULL)
{
	m_window = static_cast<ctp2_Window *>
	        (aui_Ldl::BuildHierarchyFromRoot(LDL_BLOCK));

	if (m_window)
	{
		m_cancelButton  = static_cast<ctp2_Button *>
		    (aui_Ldl::GetObject(LDL_BLOCK, "DialogBackground.CancelButton"));
		if (m_cancelButton)
		{
			m_cancelButton->SetActionFuncAndCookie(CancelCallback, this);
		}

		m_inventoryList = static_cast<ctp2_ListBox *>
		    (aui_Ldl::GetObject(LDL_BLOCK, "DialogBackground.InventoryList"));
	}
}

CityEspionage::~CityEspionage()
{
	HideWindow();

	if (m_inventoryList)
	{
		m_inventoryList->ClearUserData CALL_TEMPLATE_FUNCTION_WITHOUT_ARGUMENT(InventoryItemInfo);
		m_inventoryList->Clear();
	}

	for (size_t unitIndex = 0; unitIndex < k_MAX_ARMY_SIZE; ++unitIndex)
	{
		MBCHAR  block [k_MAX_NAME_LEN];
		sprintf(block, "DialogBackground.FortifiedUnitsBox.Unit%i", unitIndex);

		ctp2_Static * unitPicture  = static_cast<ctp2_Static *>
		                                (aui_Ldl::GetObject(LDL_BLOCK, block));
		if (unitPicture)
		{
			unitPicture->ExchangeImage(0, 0, NULL);
		}
	}

	aui_Ldl::DeleteHierarchyFromRoot(LDL_BLOCK);
}

/// Release all data of the singleton.
/// \remarks Can be used as ::UiCleanupCallback
void CityEspionage::Cleanup()
{
	delete s_CityEspionage;
	s_CityEspionage = NULL;
}

/// Display espionage intelligence of a city
/// \param  a_City  The city that has been investigated
/// \remarks Creates or reuses the singleton s_CityEspionage.
void CityEspionage::Display(Unit a_City)
{
	if (!s_CityEspionage)
	{
		s_CityEspionage = new CityEspionage();
		g_c3ui->RegisterCleanup(&CityEspionage::Cleanup);
	}

	s_CityEspionage->DisplayWindow(a_City);
}

/// Display espionage intelligence of a city
/// \param  a_City  The city that has been investigated
void CityEspionage::DisplayWindow(Unit a_City)
{
	if (m_window)
	{
		AUI_ERRCODE err = g_c3ui->AddWindow(m_window);
		Assert(err == AUI_ERRCODE_OK);

		if (err == AUI_ERRCODE_OK)
		{
			err = m_window->Show();
		}

		if (!a_City.IsValid())
		{
			return;
		}

		SlicObject so;
		so.AddPlayer(a_City.GetOwner());
		so.AddCity(a_City);

		SetLabelText("DialogBackground.CityLabel",          "str_ldl_Espionage_City_Label",         so);
		SetLabelText("DialogBackground.PopulationLabel",    "str_ldl_Espionage_Population_Label",   so);
		SetLabelText("DialogBackground.HapinessLabel",      "str_ldl_Espionage_Happiness_Label",    so);
		SetLabelText("DialogBackground.Building",           "str_ldl_Espionage_Building_Label",     so);
		SetLabelText("DialogBackground.CompleteIn",         "str_ldl_Espionage_CompleteIn_Label",   so);
		SetLabelText("DialogBackground.CityIncome",         "str_ldl_Espionage_CityIncome_Label",   so);
		SetLabelText("DialogBackground.GoldFromTrade",      "str_ldl_Espionage_GoldInTrade_Label",  so);

		Assert(m_inventoryList);
		if (m_inventoryList)
		{
			m_inventoryList->ClearUserData CALL_TEMPLATE_FUNCTION_WITHOUT_ARGUMENT(InventoryItemInfo);
			m_inventoryList->Clear();
			m_inventoryList->BuildListStart();

			CityData const *    cityData    = a_City.GetCityData();

			for (sint32 buildIndex = 0; buildIndex < g_theBuildingDB->NumRecords(); ++buildIndex)
			{
				if (cityData->HasBuilding(buildIndex))
				{
					ctp2_ListItem * item = static_cast<ctp2_ListItem *>
							(aui_Ldl::BuildHierarchyFromRoot("ce_InventoryListItem"));
					if (item)
					{
						ctp2_Static * box = static_cast<ctp2_Static*>(item->GetChildByIndex(0));
						ctp2_Static * label = static_cast<ctp2_Static*>(box->GetChildByIndex(0));
						label->SetText(g_theBuildingDB->Get(buildIndex)->GetNameText());
						item->SetUserData(new InventoryItemInfo(true, buildIndex));
					}

					m_inventoryList->AddItem(item);
				}
			}

			for (sint32 wonderIndex = 0; wonderIndex < g_theWonderDB->NumRecords(); ++wonderIndex)
			{
				if (cityData->GetBuiltWonders() & ((uint64) 1 << (uint64) wonderIndex))
				{
					ctp2_ListItem * item = static_cast<ctp2_ListItem *>
					    (aui_Ldl::BuildHierarchyFromRoot("ce_InventoryListItem"));

					if (item)
					{
						ctp2_Static * box = static_cast<ctp2_Static*>(item->GetChildByIndex(0));
						ctp2_Static * label = static_cast<ctp2_Static*>(box->GetChildByIndex(0));
						label->SetText(g_theWonderDB->Get(wonderIndex)->GetNameText());
						item->SetUserData(new InventoryItemInfo(false, wonderIndex));
					}

					m_inventoryList->AddItem(item);
				}
			}

			m_inventoryList->BuildListEnd();

			MapPoint pos;
			a_City.GetPos(pos);
			Cell * cell = g_theWorld->GetCell(pos);

			CellUnitList cellUnitList;
			cell->GetArmy(cellUnitList);

			sint32 unitIndex = 0;

			for (sint32 multiIndex = 0; multiIndex < k_MAX_ARMY_SIZE; ++multiIndex)
			{
				MBCHAR interp[k_MAX_NAME_LEN];
				sprintf(interp, "DialogBackground.FortifiedUnitsBox.Unit%i", multiIndex);
				ctp2_Static * st = (ctp2_Static *) aui_Ldl::GetObject(LDL_BLOCK, interp);

				if (st)
				{
					if (unitIndex < cellUnitList.Num())
					{
						Unit &  unit = cellUnitList[unitIndex++];

						if (st->IsDisabled())
						{
							st->Enable(true);
						}

						st->ExchangeImage(0, 0,	unit.GetDBRec()->GetDefaultIcon()->GetIcon());
					}
					else
					{
						if (!st->IsDisabled())
						{
							st->Enable(false);
						}

						st->ExchangeImage(0, 0, NULL);
					}
				}
			}
		}
	}
}

/// Hide the espionage intelligence of a city
void CityEspionage::HideWindow()
{
	if (m_window)
	{
		m_window->Hide();

		if (g_c3ui)
		{
			g_c3ui->RemoveWindow(m_window->Id());
		}
	}
}

/// Handle the cancel button
/// \param  control Not used
/// \param  action  Action to perform
/// \param  data    Not used
/// \param  cookie  Not used, except for consistency testing in the debug version.
/// \remarks Can be used as aui_Control::ControlActionCallback
void CityEspionage::CancelCallback
(
    aui_Control *   control,
    uint32          action,
    uint32          data,
    void *          cookie
)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_CityEspionage == static_cast<CityEspionage const *>(cookie));
	if (s_CityEspionage)
	{
		s_CityEspionage->HideWindow();
	}
}
