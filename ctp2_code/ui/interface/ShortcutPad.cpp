#include "c3.h"

#include "ShortcutPad.h"

#include "aui_ldl.h"
#include "citywindow.h"
#include "ctp2_button.h"
#include "SelItem.h"
#include "screenutils.h"
#include "trademanager.h"
#include "unitmanager.h"
#include "greatlibrary.h"
#include "World.h"
#include "ArmyData.h"

namespace Shortcuts {

	void CityManagementCallbackFunction()
	{

		CityWindow::Initialize();

		Unit city;
		Army a;
		if(g_selected_item->GetSelectedCity(city))
			CityWindow::Display(city);
		else if(g_selected_item->GetSelectedArmy(a)) {
			if(g_theWorld->HasCity(a->RetPos())) {
				CityWindow::Display(g_theWorld->GetCity(a->RetPos()));
			} else {
				CityWindow::Display();
			}
		} else {
			CityWindow::Display();
		}
	}

	ShortcutPad::ShortcutCallback<void> CityManagementCallback(CityManagementCallbackFunction);
	ShortcutPad::ShortcutCallback<sint32> CivilizationManagementCallback(open_CivStatus);
	ShortcutPad::ShortcutCallback<sint32> NationalManagementCallback(open_CityStatus);
	ShortcutPad::ShortcutCallback<AUI_ERRCODE> UnitManagementCallback(UnitManager::Display);
	ShortcutPad::ShortcutCallback<sint32> DiplomacyCallback(open_Diplomacy);
	ShortcutPad::ShortcutCallback<sint32> ScienceCallback(open_ScienceStatus);
	ShortcutPad::ShortcutCallback<bool> GreatLibraryCallback(open_GreatLibrary);
	ShortcutPad::ShortcutCallback<AUI_ERRCODE> TradeCallback(TradeManager::Display);
};

ShortcutPad::ShortcutPad(MBCHAR *ldlBlock) :
m_cityManagementButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock,
	"ShortcutPad.CityManagementButton"))),
m_civilizationManagementButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock,
	"ShortcutPad.CivilizationManagementButton"))),
m_nationalManagementButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock,
	"ShortcutPad.NationalManagementButton"))),
m_unitManagementButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock,
	"ShortcutPad.UnitManagementButton"))),
m_diplomacyButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock,
	"ShortcutPad.DiplomacyButton"))),
m_scienceButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock,
	"ShortcutPad.ScienceButton"))),
m_tradeButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock,
	"ShortcutPad.TradeButton"))),
m_greatLibraryButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock,
	"ShortcutPad.GreatLibraryButton")))
{

	Assert(m_cityManagementButton);
	Assert(m_civilizationManagementButton);
	Assert(m_nationalManagementButton);
	Assert(m_unitManagementButton);
	Assert(m_diplomacyButton);
	Assert(m_scienceButton);
	Assert(m_greatLibraryButton);
	Assert(m_tradeButton);

	m_cityManagementButton->SetActionFuncAndCookie(
		ShortcutButtonActionCallback, &Shortcuts::CityManagementCallback);
	m_civilizationManagementButton->SetActionFuncAndCookie(
		ShortcutButtonActionCallback, &Shortcuts::CivilizationManagementCallback);
	m_nationalManagementButton->SetActionFuncAndCookie(
		ShortcutButtonActionCallback, &Shortcuts::NationalManagementCallback);
	m_unitManagementButton->SetActionFuncAndCookie(
		ShortcutButtonActionCallback, &Shortcuts::UnitManagementCallback);
	m_diplomacyButton->SetActionFuncAndCookie(
		ShortcutButtonActionCallback, &Shortcuts::DiplomacyCallback);
	m_scienceButton->SetActionFuncAndCookie(
		ShortcutButtonActionCallback, &Shortcuts::ScienceCallback);
	m_greatLibraryButton->SetActionFuncAndCookie(
		ShortcutButtonActionCallback, &Shortcuts::GreatLibraryCallback);
	m_tradeButton->SetActionFuncAndCookie(
		ShortcutButtonActionCallback, &Shortcuts::TradeCallback);
}

void ShortcutPad::ShortcutButtonActionCallback(aui_Control *control, uint32 action,
											   uint32 data, void *cookie)
{

	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;




	close_AllScreens();

	Assert(cookie)
	(*(static_cast<ShortcutObject*>(cookie)))();
}
