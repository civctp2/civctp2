//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Empire control panel tab handling
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Blank function added to hide the data of the previous player for hotseat
//   games. 
// - Use the same science percentage everywhere.
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "DomesticControlPanel.h"


#include <sstream>


#include "AdvanceRecord.h"
#include "aui_ldl.h"
#include "ctp2_button.h"
#include "ctp2_Static.h"
#include "IconRecord.h"
#include "player.h"
#include "sci_advancescreen.h"
#include "SelItem.h"

#include "pixelutils.h"
#include "primitives.h"
#include "colorset.h"
#include "pollution.h"
#include "GovernmentRecord.h"

#include "aui_bitmapfont.h"
#include "c3math.h"		// AsPercentage

extern ColorSet				*g_colorSet;
extern Pollution *g_thePollution;

AUI_ERRCODE domesticcontrolpanel_HappinessDrawCallback(ctp2_Static *control, 
													   aui_Surface *surface,
													   RECT &rect, 
													   void *cookie)
{
	if (g_selected_item==NULL)
		return AUI_ERRCODE_OK;

	sint32 hapvals[3];
	float total;

	if(!g_player[g_selected_item->GetVisiblePlayer()]) {
		
		return AUI_ERRCODE_OK;
	}
	
	
	g_player[g_selected_item->GetVisiblePlayer()]->CountCityHappiness(hapvals[0],hapvals[1],hapvals[2]);

	
	total = (float)(hapvals[0]+hapvals[1]+hapvals[2]);

	RECT tmp=rect;
	
	
	tmp.left += 2;
	tmp.right -= 4;
	
	
	tmp.top += 5;
	tmp.left += 2;
	tmp.right -= 2;
	tmp.bottom -= 5;
	
	if(total<=0.0f)
		primitives_PaintRect16(surface, &tmp, g_colorSet->GetColor(COLOR_YELLOW));
	else
	{
		float width = (float)(tmp.right-tmp.left);

		Pixel16 colors[3]={
							g_colorSet->GetColor(COLOR_RED),
							g_colorSet->GetColor(COLOR_YELLOW),
							g_colorSet->GetColor(COLOR_GREEN)
						  };

		for (uint32 i=0;i<3;i++)
		{
			tmp.right = tmp.left + (sint32)(((float)hapvals[i]/total)*width);
			primitives_PaintRect16(surface, &tmp, colors[i]);

			tmp.left = tmp.right;
		}
	}
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE domesticcontrolpanel_PollutionDrawCallback(ctp2_Static *control, 
													   aui_Surface *surface,
													   RECT &rect, 
													   void *cookie)
{
	
	double total = double(g_thePollution->GetGlobalPollutionLevel());
	double nextEvent = double(g_thePollution->GetNextTrigger());

	RECT tmp=rect;
	
	
	tmp.left += 2;
	tmp.right -= 4;
	
	
	tmp.top += 5;	
	tmp.left += 2;
	tmp.right -= 2;
	tmp.bottom -= 5;
	
	sint32 width = (total * (tmp.right - tmp.left)) / nextEvent;
	primitives_PaintRect16(surface, &tmp, g_colorSet->GetColor(COLOR_BLACK));

	if(width > 0) {
		tmp.right = tmp.left + width;
		if(tmp.right > rect.right)
			tmp.right = rect.right;

		primitives_PaintRect16(surface, &tmp, g_colorSet->GetColor(COLOR_RED));
	}

	return AUI_ERRCODE_OK;
}



DomesticControlPanel::DomesticControlPanel(MBCHAR *ldlBlock) :
m_scienceLabel(static_cast<ctp2_Static*>(
			   aui_Ldl::GetObject(ldlBlock,
			   "DomesticTab.TabPanel.AdvanceProgress.Title"))),
m_scienceIconButton(static_cast<ctp2_Button*>(
					aui_Ldl::GetObject(ldlBlock,
					"DomesticTab.TabPanel.AdvanceProgress."
					"IconBorder.IconButton"))),
m_scienceTurnButton(static_cast<ctp2_Button*>(
					aui_Ldl::GetObject(ldlBlock,
					"DomesticTab.TabPanel.AdvanceProgress."
					"IconBorder.IconButton.RadialButton"))),
m_citiesValue(static_cast<ctp2_Static*>(
			  aui_Ldl::GetObject(ldlBlock,
			  "DomesticTab.TabPanel.CitiesValue"))),
m_populationValue(static_cast<ctp2_Static*>(
			  aui_Ldl::GetObject(ldlBlock,
			  "DomesticTab.TabPanel.PopulationValue"))),
m_publicWorksValue(static_cast<ctp2_Static*>(
				   aui_Ldl::GetObject(ldlBlock,
				   "DomesticTab.TabPanel.PublicWorksValue"))),
m_governmentValue(static_cast<ctp2_Static*>(
				   aui_Ldl::GetObject(ldlBlock,
				   "DomesticTab.TabPanel.GovernmentValue"))),
m_scienceValue(static_cast<ctp2_Static*>(
				   aui_Ldl::GetObject(ldlBlock,
				   "DomesticTab.TabPanel.ScienceValue"))),
m_pollutionValue(static_cast<ctp2_Static*>(
				   aui_Ldl::GetObject(ldlBlock,
				   "DomesticTab.TabPanel.PollutionValue"))),
m_menuGoldValue(static_cast<ctp2_Static*>(
	            aui_Ldl::GetObject("MainMenu.GoldStatic"))),
m_menuPublicWorksValue(static_cast<ctp2_Static*>(
	                   aui_Ldl::GetObject("MainMenu.PWStatic"))),
m_menuHappinessValue(static_cast<ctp2_Static*>(
	aui_Ldl::GetObject("MainMenu.HappinessBar"))),
m_menuPollutionValue(static_cast<ctp2_Static*>(
	aui_Ldl::GetObject("MainMenu.PollutionBar")))
{
	
	Assert(m_scienceLabel);
	Assert(m_scienceIconButton);
	Assert(m_scienceTurnButton);
	Assert(m_citiesValue);
	Assert(m_populationValue);
	Assert(m_publicWorksValue);
	Assert(m_governmentValue);
	Assert(m_scienceValue);
	Assert(m_pollutionValue);

	Assert(m_menuGoldValue);
	Assert(m_menuPollutionValue);

	strcpy(m_ldlBlock,ldlBlock);

	
	m_scienceIconButton->SetActionFuncAndCookie(
		EditResearchButtonActionCallback, this);
	m_scienceTurnButton->SetActionFuncAndCookie(
		EditResearchButtonActionCallback, this);

	m_menuHappinessValue->SetDrawCallbackAndCookie(domesticcontrolpanel_HappinessDrawCallback, NULL, false);
	m_menuHappinessValue->ShouldDraw(TRUE);

	m_menuPollutionValue->SetDrawCallbackAndCookie(domesticcontrolpanel_PollutionDrawCallback, NULL, false);
	m_menuPollutionValue->ShouldDraw(TRUE);

	m_currentResearch = -1;
	m_currentTurns = -1;
	m_currentGold = -1;
	m_currentCities = -1;
	m_currentPopulation = -1;
	m_currentPW = -1;
	m_currentGovernment = -1;
	m_currentScience = -1;
	m_currentPollution = -1;

}

//----------------------------------------------------------------------------
//
// Name       : DomesticControlPanel::Blank
//
// Description: Blank out the data of the previous player in between turns for
//              hotseat play.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Only the top bar data is blanked out now. The other panels are 
//              hidden later anyway.
//
//----------------------------------------------------------------------------
void DomesticControlPanel::Blank()
{
	m_menuGoldValue->SetText("");
	m_currentGold	= -1;

 // m_menuHappinessValue: TODO (or let it be: not too much info from it)

 // m_menuPollutionValue not blanked out: this is global data for all players.

	m_menuPublicWorksValue->SetText("");
	m_currentPW		= -1;
}

void DomesticControlPanel::Update()
{
	
	UpdateGoldPW();

	
	if(m_scienceIconButton->IsHidden())
		return;

	
	UpdateScience();
	UpdateStats();
}


void DomesticControlPanel::EditResearchButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	
	sci_advancescreen_displayMyWindow(NULL, k_SCI_INCLUDE_CANCEL);
}


void DomesticControlPanel::UpdateScience()
{
	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];
	if(!player) {
		ClearScience();
		return;
	}

	
	Advances *advances = player->m_advances;
	AdvanceType currentAdvance = advances->GetResearching();
	sint32 numberOfTurns = advances->TurnsToNextAdvance();
	BOOL alreadyHas = player->HasAdvance(advances->GetResearching());
	const AdvanceRecord *currentAdvanceRecord =
		g_theAdvanceDB->Get(currentAdvance);

	if(currentAdvance == m_currentResearch && numberOfTurns == m_currentTurns)
		return;

	m_currentResearch = currentAdvance;
	m_currentTurns = numberOfTurns;

	
	if(!currentAdvanceRecord || alreadyHas) {
		ClearScience();
		return;
	}

	
	MBCHAR tempStr[100];
	strncpy(tempStr,currentAdvanceRecord->GetNameText(),99);
	tempStr[99]=0;
	m_scienceLabel->GetTextFont()->TruncateString(tempStr, m_scienceLabel->Width());
	m_scienceLabel->SetText(tempStr);

	
	const MBCHAR *scienceIconName =
		currentAdvanceRecord->GetIcon()->GetIcon();

	
	
	if(scienceIconName && strcmp(scienceIconName, "NULL")) {
		
		m_scienceIconButton->SetText("");
		m_scienceIconButton->ExchangeImage(4, 0, scienceIconName);
	} else {
		
		m_scienceIconButton->SetText("---");
		m_scienceIconButton->ExchangeImage(4, 0, NULL);
	}

	
	MBCHAR numTurns[50];
	
	if(numberOfTurns < 0)
		
		sprintf(numTurns, "---");
	else
		
		sprintf(numTurns, "%d", numberOfTurns + 1);
	m_scienceTurnButton->SetText(numTurns); 
}


void DomesticControlPanel::ClearScience()
{
	m_scienceLabel->SetText("");
	m_scienceIconButton->SetText("");
	m_scienceIconButton->ExchangeImage(4, 0, NULL);
	m_scienceTurnButton->SetText("");
}


void DomesticControlPanel::UpdateStats()
{
	if(!g_player[g_selected_item->GetVisiblePlayer()]) {
		
		return;
	}

	sint32 cities, population, pw, government, science, pollution;
	double scienceTax = 0.0;

	cities = g_player[g_selected_item->GetVisiblePlayer()]->GetNumCities();
	population = g_player[g_selected_item->GetVisiblePlayer()]->GetTotalPopulation();
	pw = (g_player[g_selected_item->GetVisiblePlayer()]->m_materialsTax * 100.0) + 0.5;
	government = g_player[g_selected_item->GetVisiblePlayer()]->GetGovernmentType();

	g_player[g_selected_item->GetVisiblePlayer()]->GetScienceTaxRate(scienceTax);
	science = AsPercentage(scienceTax);
	pollution = g_player[g_selected_item->GetVisiblePlayer()]->GetPollutionLevel();

	if(cities == m_currentCities && population == m_currentPopulation &&
		pw == m_currentPW && government == m_currentGovernment &&
		science == m_currentScience && pollution == m_currentPollution)
	{
		
		return;
	}

	m_currentCities		=cities;     
	m_currentPopulation	=population;
	m_currentPW			=pw;
	m_currentGovernment	=government;
	m_currentScience	=science;
	m_currentPollution	=pollution;

	
	MBCHAR stringBuffer[50];

	sprintf(stringBuffer, "%d", cities);
	m_citiesValue->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", population);
	m_populationValue->SetText(stringBuffer);

	sprintf(stringBuffer, "%d%%", pw);
	m_publicWorksValue->SetText(stringBuffer);
	
	sprintf(stringBuffer, "%s", g_theGovernmentDB->Get(government)->GetNameText());
	m_governmentValue->SetText(stringBuffer);

	sprintf(stringBuffer, "%d%%", science);
	m_scienceValue->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", pollution);
	m_pollutionValue->SetText(stringBuffer);

	static_cast<ctp2_Button*>(aui_Ldl::GetObject(m_ldlBlock,
					"DomesticTab.TabPanel"))->ShouldDraw(TRUE);
}

void DomesticControlPanel::UpdateGoldPW()
{
	
	if (g_player[g_selected_item->GetVisiblePlayer()] == NULL)
		return;

	sint32 gold = g_player[g_selected_item->GetVisiblePlayer()]->GetGold();
	sint32 pw = g_player[g_selected_item->GetVisiblePlayer()]->GetMaterialsStored();

	if(gold == m_currentGold && pw == m_currentPW)
		return;

	m_currentGold = gold;
	m_currentPW = pw;

	
	
	MBCHAR goldStr[50];
	sprintf(goldStr, "%d", gold);

	m_menuGoldValue->SetText(goldStr);

	MBCHAR pwStr[50];
	sprintf(pwStr, "%d", pw);
	m_publicWorksValue->SetText(pwStr); 
	m_menuPublicWorksValue->SetText(pwStr);

	m_menuHappinessValue->ShouldDraw(TRUE);
	m_menuPollutionValue->ShouldDraw(TRUE);
}
