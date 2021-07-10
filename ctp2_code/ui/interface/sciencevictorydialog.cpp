//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface screen
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
// - Standardized code (May 29th 2006 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "sciencevictorydialog.h"

#include "aui_blitter.h"
#include "aui_ldl.h"
#include "c3ui.h"
#include "citydata.h"
#include "civarchive.h"
#include "colorset.h"           // g_colorSet
#include "ctp2_button.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "ctp2_Static.h"
#include "ctp2_Window.h"
#include "EditQueue.h"
#include "gaiacontroller.h"
#include "Globals.h"            // k_GAME_OBJ_TYPE_IMPROVEMENT
#include "installation.h"
#include "player.h"
#include "radarmap.h"
#include "SelItem.h"
#include "StrDB.h"
#include "UnitDynArr.h"
#include "World.h"
#include "citywindow.h"

extern C3UI		*g_c3ui;

ScienceVictoryDialog *g_scienceVictoryDialog = NULL;

const sint32 k_SWITCH_CONSTRUCTION		= 0;
const sint32 k_SWITCH_STATUS			= 1;

void ScienceVictoryDialog::Open()
{

	if(!g_scienceVictoryDialog) {
		g_scienceVictoryDialog = new ScienceVictoryDialog;
	}

	g_scienceVictoryDialog->Show();

	g_scienceVictoryDialog->Update();
}

void ScienceVictoryDialog::Close()
{

	if(g_scienceVictoryDialog) {
		g_scienceVictoryDialog->Hide();
	}
}

void ScienceVictoryDialog::Cleanup()
{

	if(g_scienceVictoryDialog &&
		!g_scienceVictoryDialog->m_window->IsHidden())
		g_scienceVictoryDialog->Hide();

	delete g_scienceVictoryDialog;
	g_scienceVictoryDialog = NULL;
}

ScienceVictoryDialog::ScienceVictoryDialog() :
m_window(static_cast<ctp2_Window*>(
		 aui_Ldl::BuildHierarchyFromRoot("ScienceVictoryDialog"))),
m_switch(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch"))),
m_switchState(-1),
m_startButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Construction.Start"))),
m_map(static_cast<RadarMap*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Construction.Map"))),
m_mainframeList(static_cast<ctp2_ListBox*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Construction.MainframeList"))),
m_towerRadius(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Construction.TowerRadius.Value"))),
m_coverage(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Construction.Coverage.Value"))),
m_mainframeReq(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Construction.ReqComGroup.Mainframe.Required.Value"))),
m_towerReq(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Construction.ReqComGroup.Tower.Required.Value"))),
m_satReq(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Construction.ReqComGroup.Satellite.Required.Value"))),
m_mainframeCom(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Construction.ReqComGroup.Mainframe.Complete.Value"))),
m_towerCom(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Construction.ReqComGroup.Tower.Complete.Value"))),
m_satCom(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Construction.ReqComGroup.Satellite.Complete.Value"))),
m_statusText(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Status.Text"))),
m_statusBar(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.Switch.Status.Bar"))),
m_buildQueueButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.BuildQueueButton"))),
m_closeButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"ScienceVictoryDialog.CloseButton")))
{
	g_c3ui->AddWindow(m_window);

	Assert(m_closeButton);

	m_map->SetInteractive(false);

	m_map->DisplayUnits(false);
	m_map->DisplayCities(false);
	m_map->DisplayBorders(false);

	m_startButton->Enable(false);





	SetSwitchState(k_SWITCH_CONSTRUCTION);


	m_switch->SetShowCallback(SwitchShowCallback, this);

	m_startButton->SetActionFuncAndCookie(StartButtonActionCallback, this);
	m_buildQueueButton->SetActionFuncAndCookie(BuildButtonActionCallback, this);
	m_closeButton->SetActionFuncAndCookie(CloseButtonActionCallback, this);
}

ScienceVictoryDialog::~ScienceVictoryDialog()
{
	if(m_window) {
		aui_Ldl::DeleteHierarchyFromRoot("ScienceVictoryDialog");
		m_window = NULL;
	}
}

void ScienceVictoryDialog::Update()
{

	Assert(g_selected_item);
	Assert(g_player[g_selected_item->GetVisiblePlayer()]);
	Assert(g_player[g_selected_item->GetVisiblePlayer()]->GetGaiaController());

	if(!g_player[g_selected_item->GetVisiblePlayer()])
		return;

	GaiaController *gaiaController =
		g_player[g_selected_item->GetVisiblePlayer()]->GetGaiaController();

	if(gaiaController->TurnsToComplete() >= 0)
		SetSwitchState(k_SWITCH_STATUS);
	else
		SetSwitchState(k_SWITCH_CONSTRUCTION);

	switch(m_switchState) {
	case k_SWITCH_CONSTRUCTION:
		UpdateStartButton(gaiaController);
		UpdateConstructionInformation(gaiaController);
		UpdateConstructionList(gaiaController);
		UpdateConstructionMap(gaiaController);
		m_buildQueueButton->Enable(true);
		break;
	case k_SWITCH_STATUS:
		UpdateStatus(gaiaController);
		m_buildQueueButton->Enable(false);
		break;
	default:
		Assert(false);
		break;
	}
}

void ScienceVictoryDialog::Show()
{
	if(m_window) {
		g_c3ui->AddWindow(m_window);
		m_window->Show();
	}
}

void ScienceVictoryDialog::Hide()
{
	if(m_window) {
		m_window->Hide();
		g_c3ui->RemoveWindow(m_window->Id());
	}
}


void ScienceVictoryDialog::SetSwitchState(sint32 switchState)
{

	if(m_switch->IsHidden()) {
		m_switchState = switchState;
		return;
	}




	for(sint32 childIndex = 0; childIndex < m_switch->NumChildren(); childIndex++) {

		aui_Region *child = m_switch->GetChildByIndex(childIndex);

		if(childIndex == switchState) {

			if(child->IsHidden())
				child->Show();
		} else {

			if(!child->IsHidden())
				child->Hide();
		}
	}

	if(m_switchState != switchState)
		m_window->ShouldDraw();

	m_switchState = switchState;
}

void ScienceVictoryDialog::UpdateStartButton(
	const GaiaController *gaiaController)
{

	if(gaiaController->CanStartCountdown())
		m_startButton->Enable(true);
	else
		m_startButton->Enable(false);
}

void ScienceVictoryDialog::UpdateConstructionInformation(
	const GaiaController *gaiaController)
{

	char buffer[256];

	sprintf(buffer, "%d%s",
		static_cast<sint32>(gaiaController->GetTowerCoverage() * 100.0f),
		g_theStringDB->GetNameStr("str_ldl_percent"));
	m_coverage->SetText(buffer);

	sprintf(buffer, "%d", gaiaController->GetTowerRadius());
	m_towerRadius->SetText(buffer);

	sprintf(buffer, "%d", gaiaController->NumMainframesRequired());
	m_mainframeReq->SetText(buffer);
	sprintf(buffer, "%d", gaiaController->NumTowersRequired());
	m_towerReq->SetText(buffer);
	sprintf(buffer, "%d", gaiaController->NumSatellitesRequired());
	m_satReq->SetText(buffer);

	sprintf(buffer, "%d", gaiaController->NumMainframesBuilt());
	m_mainframeCom->SetText(buffer);
	sprintf(buffer, "%d", gaiaController->NumTowersBuilt());
	m_towerCom->SetText(buffer);
	sprintf(buffer, "%d", gaiaController->NumSatellitesLaunched());
	m_satCom->SetText(buffer);
}

ctp2_Static *ScienceVictoryDialog::GetListItemColumn(ctp2_ListItem * item, sint32 column)
{
	ctp2_Static * box = static_cast<ctp2_Static *>(item->GetChildByIndex(0));
	return static_cast<ctp2_Static *>(box->GetChildByIndex(column));
}

ctp2_ListItem *ScienceVictoryDialog::CreateMainframeItem(const Unit &city,
														 sint32 timeRemaining)
{

	ctp2_ListItem *item = static_cast<ctp2_ListItem*>(
		aui_Ldl::BuildHierarchyFromRoot("MainframeListItem"));

	Assert(item);
	if(!item)
		return(NULL);

	item->SetUserData(reinterpret_cast<void*>(city.m_id));

	item->SetCompareCallback(CompareMainframeCities);

	CityData *cityData = city.GetCityData();

	if(ctp2_Static *column = GetListItemColumn(item, 0)) {
		column->SetText(cityData->GetName());
	}

	if(ctp2_Static *column = GetListItemColumn(item, 1)) {
		if(timeRemaining < 0)
			column->SetText(g_theStringDB->GetNameStr("str_ldl_SV_COMPLETE"));
		else {

			char buffer[32];
			sprintf(buffer, "%d", timeRemaining);
			column->SetText(buffer);
		}
	}

	return(item);
}

void ScienceVictoryDialog::UpdateConstructionList(
	const GaiaController *gaiaController)
{

	m_mainframeList->BuildListStart();

	m_mainframeList->Clear();

	UnitDynamicArray *cityList =
		g_player[g_selected_item->GetVisiblePlayer()]->GetAllCitiesList();


	for(sint32 cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {

		Unit city = cityList->Get(cityIndex);

		if(city.HaveImprovement(GaiaController::GetMainframeBuildingIndex())) {
			m_mainframeList->AddItem(CreateMainframeItem(city, -1));
			continue;
		}

		BuildQueue *buildQueue = city.GetCityData()->GetBuildQueue();

		if(buildQueue->GetLen() &&
			(buildQueue->GetHead()->m_category == k_GAME_OBJ_TYPE_IMPROVEMENT) &&
			(buildQueue->GetHead()->m_type ==
			GaiaController::GetMainframeBuildingIndex())) {

			m_mainframeList->AddItem(
				CreateMainframeItem(city, city.HowMuchLonger()));
		}
	}

	m_mainframeList->BuildListEnd();

	m_mainframeList->SortByColumn(1, true);
}

void ScienceVictoryDialog::UpdateConstructionMap(
	const GaiaController *gaiaController)
{

	m_map->ClearMapOverlay();




	for(sint32 y = 0; y < g_theWorld->GetHeight(); y++) {
		for(sint32 x = 0; x < g_theWorld->GetWidth(); x++) {
			if(gaiaController->GetCoverage().Get(x, y))
				m_map->SetMapOverlayCell(MapPoint(x, y), COLOR_YELLOW);
		}
	}

	Player *player = g_player[g_selected_item->GetVisiblePlayer()];
	for(sint32 installation = 0; installation <
		player->m_allInstallations->Num(); installation++) {

		if(gaiaController->GaiaControllerTileImp(
			player->m_allInstallations->Access(installation).GetType())) {

			MapPoint position;
			player->m_allInstallations->Access(installation).GetPos(position);

			m_map->SetMapOverlayCell(position, COLOR_RED);
		}
	}

	m_map->Update();
}


void ScienceVictoryDialog::UpdateStatus(const GaiaController *gaiaController)
{

	char buffer[256];
	sprintf(buffer, g_theStringDB->GetNameStr("str_ldl_SV_COUNTDOWN_SEQUENCE"),
		gaiaController->TurnsToComplete());
	m_statusText->SetText(buffer);




	m_statusBar->SetDrawCallbackAndCookie(StatusBarActionCallback,
		const_cast<GaiaController*>(gaiaController), false);
}

AUI_ERRCODE ScienceVictoryDialog::StatusBarActionCallback(ctp2_Static *control,
	aui_Surface *surface, RECT &rect, void *cookie)
{

	const GaiaController *gaiaController =
		static_cast<const GaiaController *>(cookie);

	double percentComplete =
		static_cast<double>(gaiaController->TotalCountdownTurns() -
		gaiaController->TurnsToComplete()) /
		static_cast<double>(gaiaController->TotalCountdownTurns());


	if(percentComplete < 0.001)
		return(AUI_ERRCODE_OK);

	RECT colorRect = rect;
	colorRect.left		+= 2;
	colorRect.top		+= 2;
	colorRect.right		-= 2;
	colorRect.bottom	-= 2;

	colorRect.right = colorRect.left + static_cast<long>(
		static_cast<double>(colorRect.right - colorRect.left) *
		percentComplete);

	return(g_c3ui->TheBlitter()->ColorBlt16(surface, &colorRect,
		g_colorSet->GetColor(COLOR_GREEN), 0));
}


void ScienceVictoryDialog::SwitchShowCallback(aui_Region *region,
											  void *userData)
{

	ScienceVictoryDialog *dialog = static_cast<ScienceVictoryDialog*>(userData);

	dialog->SetSwitchState(dialog->m_switchState);
}

sint32 ScienceVictoryDialog::CompareMainframeCities(ctp2_ListItem *item1,
													ctp2_ListItem *item2,
													sint32 column)
{

	Unit city1, city2;
	city1.m_id = reinterpret_cast<uint32>(item1->GetUserData());
	city2.m_id = reinterpret_cast<uint32>(item2->GetUserData());

	Assert(city1.IsValid());
	Assert(city2.IsValid());

	bool mainframe1 = (city1.HaveImprovement(
		GaiaController::GetMainframeBuildingIndex()) == TRUE);
	bool mainframe2 = (city2.HaveImprovement(
		GaiaController::GetMainframeBuildingIndex()) == TRUE);


	if(mainframe1 != mainframe2)
		return(mainframe1 ? -1 : 1);




	if(mainframe1)
		column = 0;

	CityData *cityData1 = city1.GetCityData();
	CityData *cityData2 = city2.GetCityData();

	switch(column) {
		case 0:

			return(stricmp(cityData1->GetName(), cityData2->GetName()));
		case 1:
			{

				BuildQueue *queue1 = cityData1->GetBuildQueue();
				BuildQueue *queue2 = cityData2->GetBuildQueue();

				return((queue1->GetLen() ? cityData1->HowMuchLonger() : -1) -
					(queue2->GetLen() ? cityData2->HowMuchLonger() : -1));
			}
		default:
			Assert(false);
			return(0);
	}
}

void ScienceVictoryDialog::StartButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{

	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	Assert(g_selected_item);
	Assert(g_player[g_selected_item->GetVisiblePlayer()]);
	Assert(g_player[g_selected_item->GetVisiblePlayer()]->GetGaiaController());

	if(!g_player[g_selected_item->GetVisiblePlayer()])
		return;

	GaiaController *gaiaController =
		g_player[g_selected_item->GetVisiblePlayer()]->GetGaiaController();

	gaiaController->StartCountdown();

	static_cast<ScienceVictoryDialog*>(cookie)->Update();
}

void ScienceVictoryDialog::BuildButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{

	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	UnitDynamicArray *cityList =
		g_player[g_selected_item->GetVisiblePlayer()]->GetAllCitiesList();

	sint32 cityIndex;
	for(cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {

		Unit city = cityList->Get(cityIndex);

		if(!city.HaveImprovement(GaiaController::GetMainframeBuildingIndex())) {
			EditQueue::Display(CityWindow::GetCityData(city));
			return;
		}
	}


	for(cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {

		Unit city = cityList->Get(cityIndex);

		if(!city.HaveImprovement(GaiaController::GetSatelliteBuildingIndex())) {
			EditQueue::Display(CityWindow::GetCityData(city));
			return;
		}
	}


	if(cityList->Num())
		EditQueue::Display(CityWindow::GetCityData(cityList->Get(0)));
}

void ScienceVictoryDialog::CloseButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{

	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	static_cast<ScienceVictoryDialog*>(cookie)->Hide();
}
