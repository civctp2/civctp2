//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Science Manager Dialog
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Display the cost for the player, not the base advancement cost.
// - Start the great library with the current research project of the player.
// - Reduced the length of the generated advance effect string.
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "ScienceManagementDialog.h"


#include "AdvanceRecord.h"
#include "aui_Blitter.h"
#include "aui_Ldl.h"
#include "aui_Switch.h"
#include "c3ui.h"
#include "ColorSet.h"
#include "ctp2_Button.h"
#include "ctp2_HyperTextBox.h"
#include "ctp2_ListBox.h"
#include "ctp2_ListItem.h"
#include "ctp2_Static.h"
#include "ctp2_Window.h"
#include "c3_listitem.h"
#include "GreatLibrary.h"
#include "IconRecord.h"
#include "Player.h"
#include "PrjFile.h"
#include "sci_AdvanceScreen.h"
#include "Sci.h"
#include "ScreenUtils.h"
#include "SelItem.h"


#include "UnitRecord.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "StrDB.h"
#include "TerrainImprovementRecord.h"
#include "GovernmentRecord.h"
#include "terrainutil.h"


static const sint32 k_SMD_CIVILIZATION_COLUMNS	= 8;


extern C3UI			*g_c3ui;
extern ColorSet		*g_colorSet;
extern ProjectFile	*g_GreatLibPF;


ScienceManagementDialog *g_scienceManagementDialog = NULL;


#define	k_SCI_COL_ADVANCE 0


void ScienceManagementDialog::Open()
{
	
	if(!g_scienceManagementDialog) {
		g_scienceManagementDialog = new ScienceManagementDialog;
	}

	
	g_scienceManagementDialog->Update();
	g_scienceManagementDialog->Show();
}


void ScienceManagementDialog::Close()
{
	
	if(g_scienceManagementDialog)
		g_scienceManagementDialog->Hide();
}


void ScienceManagementDialog::Cleanup()
{
	
	if(g_scienceManagementDialog &&
	   g_c3ui->GetWindow(g_scienceManagementDialog->m_window->Id()) &&
	   !g_scienceManagementDialog->m_window->IsHidden())
		g_scienceManagementDialog->Hide();

	
	delete g_scienceManagementDialog;
	g_scienceManagementDialog = NULL;
}


ScienceManagementDialog::ScienceManagementDialog() :
m_window(static_cast<ctp2_Window*>(
		 aui_Ldl::BuildHierarchyFromRoot("ScienceManagementDialog"))),
m_closeButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"ScienceManagementDialog.CloseButton"))),
m_changeButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"ScienceManagementDialog.ChangeResearch"))),
m_scienceLabel(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceManagementDialog.CurrentResearch.AdvanceProgress.Title"))),
m_scienceIconButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"ScienceManagementDialog.CurrentResearch.AdvanceProgress.IconBorder."
	"IconButton"))),
m_scienceTurnButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"ScienceManagementDialog.CurrentResearch.AdvanceProgress.IconBorder."
	"IconButton.RadialButton"))),
m_scienceTurnValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceManagementDialog.CurrentResearch.Turn.Value"))),
m_scienceCurrentValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceManagementDialog.CurrentResearch.Spent.Value"))),
m_scienceTotalValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"ScienceManagementDialog.CurrentResearch.TotalCost.Value"))),
m_scienceDescription(static_cast<ctp2_HyperTextBox*>(aui_Ldl::GetObject(
	"ScienceManagementDialog.CurrentResearch.Description"))),
m_advanceList(static_cast<ctp2_ListBox*>(aui_Ldl::GetObject(
	"ScienceManagementDialog.AdvanceList")))
{
	g_c3ui->AddWindow(m_window);

	
	Assert(m_closeButton);
	Assert(m_scienceLabel);
	Assert(m_scienceIconButton);
	Assert(m_scienceTurnButton);
	Assert(m_scienceTurnValue);
	Assert(m_scienceCurrentValue);
	Assert(m_scienceTotalValue);
	Assert(m_scienceDescription);
	Assert(m_advanceList);

	
	m_closeButton->SetActionFuncAndCookie(
		CloseButtonActionCallback, this);
	m_changeButton->SetActionFuncAndCookie(
		EditResearchButtonActionCallback, this);
	m_scienceIconButton->SetActionFuncAndCookie(
		EditResearchButtonActionCallback, this);
	m_scienceTurnButton->SetActionFuncAndCookie(
		EditResearchButtonActionCallback, this);
	m_scienceDescription->SetActionFuncAndCookie(
		HyperlinkActionCallback, this);

	
	m_advanceList->SetActionFuncAndCookie(AdvanceListCallback, this);
	m_advanceList->SetForceSelect( TRUE );




	
	ctp2_Button *adviceButton = (ctp2_Button *)aui_Ldl::GetObject("ScienceManagementDialog.AdvisorButton");
	if(adviceButton) {
		
		adviceButton->Move(-adviceButton->X(), -adviceButton->Y());
	}
}

ScienceManagementDialog::~ScienceManagementDialog()
{
	if(m_window) {
		aui_Ldl::DeleteHierarchyFromRoot("ScienceManagementDialog");
	}
}


void ScienceManagementDialog::Show()
{
	g_c3ui->AddWindow(m_window);
	m_window->Show();
}


void ScienceManagementDialog::Hide()
{
	m_window->Hide();
	g_c3ui->RemoveWindow(m_window->Id());
}


void ScienceManagementDialog::Update()
{
	
	UpdateScience();
	UpdateAdvanceList();
}


void ScienceManagementDialog::UpdateScience()
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

	
	if(!currentAdvanceRecord || alreadyHas) {
		ClearScience();
		return;
	}

	
	MBCHAR tempStr[257];
	strncpy(tempStr, currentAdvanceRecord->GetNameText(), 256);
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

	
	MBCHAR buffer[64];
	if(numberOfTurns < 0)
		sprintf(buffer, "-");
	else
		sprintf(buffer, "%d", numberOfTurns + 1);
	m_scienceTurnButton->SetText(buffer);
	m_scienceTurnValue->SetText(buffer);

	
	sprintf(buffer, "%d", player->m_science->GetLevel());
	m_scienceCurrentValue->SetText(buffer);


	sprintf(buffer, "%d", player->GetCurrentScienceCost());
	m_scienceTotalValue->SetText(buffer);

	
	long textLength = 0;
	MBCHAR *description = static_cast<MBCHAR *>(
		g_GreatLibPF->getData(
		const_cast<char*>(currentAdvanceRecord->GetIcon()->GetVari()),
		&textLength));

	
	
	
	
	MBCHAR *descriptionCopy = new char[textLength + 1];
	memcpy(descriptionCopy, description, textLength);
	descriptionCopy[textLength] = 0;

	
	m_scienceDescription->SetHyperText(descriptionCopy);

	
	delete [] descriptionCopy;
	g_GreatLibPF->freeData(description);
}


void ScienceManagementDialog::ClearScience()
{
	m_scienceLabel->SetText("");
	m_scienceIconButton->SetText("");
	m_scienceIconButton->ExchangeImage(4, 0, NULL);
	m_scienceTurnButton->SetText("");
	m_scienceTurnValue->SetText("");
	m_scienceCurrentValue->SetText("");
	m_scienceTotalValue->SetText("");
	m_scienceDescription->SetHyperText("");
}


void ScienceManagementDialog::UpdateAdvanceList()
{
	
	ctp2_ListItem *item = NULL;

	
	for(int index = 1; index <= k_SMD_CIVILIZATION_COLUMNS; index++) {
		
		aui_Switch *header = m_advanceList->GetHeaderSwitchByIndex(index+1);

		
		header->SetDrawCallbackAndCookie(ColorHeaderActionCallback,
			reinterpret_cast<void*>(index), false);

		
		Player *player = g_player[g_selected_item->GetVisiblePlayer()];
		if(player) {
			
			if(player->HasEmbassyWith(index)) {
				
				
				header->SetImage("upic01.tga", 0);
				header->SetImage("upic01.tga", 1);
				header->SetImage("upic01.tga", 2);
			} else {
				
				header->SetImage(NULL, 0);
				header->SetImage(NULL, 1);
				header->SetImage(NULL, 2);
			}
		}
	}

	
	m_advanceList->BuildListStart();

	
	m_advanceList->Clear();

	
	for(sint32 advanceIndex = 0; advanceIndex <
		g_theAdvanceDB->NumRecords(); advanceIndex++) {
		
		
		item = CreateAdvanceItem( g_theAdvanceDB->Get(advanceIndex) );
		if (item)
			m_advanceList->AddItem(item);

	}

	
	m_advanceList->BuildListEnd();

	
	m_advanceList->SortByColumn(0, true);
}


ctp2_ListItem *ScienceManagementDialog::CreateAdvanceItem(const AdvanceRecord *advance)
{
	
	ctp2_ListItem *item = static_cast<ctp2_ListItem*>(
		aui_Ldl::BuildHierarchyFromRoot("ScienceAdvanceListItem"));

	
	Assert(item);
	if(!item)
		return(NULL);

	
	item->SetUserData(reinterpret_cast<void*>(advance->GetIndex()));

	
	item->SetCompareCallback(CompareAdvance);

	
	if (UpdateAdvanceItem(item, advance))
		
		return(item);
	else
	{
		
		
		delete item;
		return NULL;
	}
}


BOOL ScienceManagementDialog::UpdateAdvanceItem(ctp2_ListItem *item,
												const AdvanceRecord *advance)
{
	BOOL discovered = FALSE;

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	if(ctp2_Static *column = GetListItemColumn(item, k_SCI_COL_ADVANCE)) {
		column->SetText(advance->GetNameText());
	}

	
	for(int index = 1; index <= k_SMD_CIVILIZATION_COLUMNS; index++) {
		
		if(ctp2_Static *column = GetListItemColumn(item, k_SCI_COL_ADVANCE + index)) {
			
			
			if(g_player[index] && g_player[index]->HasAdvance(advance->GetIndex()) &&
				((index == g_selected_item->GetVisiblePlayer()) ||
				player->HasEmbassyWith(index))) {
				
				
				column->SetDrawCallbackAndCookie(ColorBoxActionCallback,
					reinterpret_cast<void*>(index));
				
				discovered = TRUE;
			} else {
				
				column->SetDrawCallbackAndCookie(NULL, NULL);
			}
		}
	}
	
	return discovered;
}


ctp2_Static *ScienceManagementDialog::GetListItemColumn(ctp2_ListItem *item,
														sint32 column)
{
	return(static_cast<ctp2_Static*>(item->GetChildByIndex(column)));
}


void ScienceManagementDialog::CloseButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	static_cast<ScienceManagementDialog*>(cookie)->Hide();
}



void ScienceManagementDialog::HyperlinkActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(CTP2_HYPERLINK_ACTION_EXECUTE))
		return;
	
	
	ctp2_HyperLink *hyperlink =
		static_cast<ctp2_HyperTextBox*>(control)->GetSelectedHyperLink();

	
	if(hyperlink) {
		open_GreatLibrary();
		g_greatLibrary->SetLibrary(hyperlink->m_index,
			static_cast<DATABASE>(hyperlink->m_db));
	}
}


void ScienceManagementDialog::EditResearchButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	
	sci_advancescreen_displayMyWindow(NULL, k_SCI_INCLUDE_CANCEL);
}


AUI_ERRCODE ScienceManagementDialog::ColorBoxActionCallback(ctp2_Static *control,
	aui_Surface *surface, RECT &rect, void *cookie)
{
	
	RECT colorRect = rect;
	colorRect.left		+= 1;
	colorRect.top		+= 1;
	colorRect.right		-= 1;
	colorRect.bottom	-= 1;

	
	return(g_c3ui->TheBlitter()->ColorBlt16(surface, &colorRect,
		g_colorSet->GetPlayerColor(reinterpret_cast<sint32>(cookie)), 0));
}


AUI_ERRCODE ScienceManagementDialog::ColorHeaderActionCallback(aui_Switch *control,
	aui_Surface *surface, RECT &rect, void *cookie)
{
	
	RECT colorRect = rect;
	colorRect.left		+= 1;
	colorRect.top		+= 1;
	colorRect.right		-= 1;
	colorRect.bottom	-= 1;

	
	sint32 index = reinterpret_cast<sint32>(cookie);

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	
	
	if(!player || ((index != g_selected_item->GetVisiblePlayer()) &&
		!player->HasContactWith(index)))
		return(AUI_ERRCODE_OK);

	
	return(g_c3ui->TheBlitter()->ColorBlt16(surface, &colorRect,
		g_colorSet->GetPlayerColor(index), 0));
}


sint32 ScienceManagementDialog::CompareAdvance(ctp2_ListItem *item1,
											   ctp2_ListItem *item2,
											   sint32 column)
{
	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	const AdvanceRecord *advance1 = g_theAdvanceDB->Get(
		reinterpret_cast<sint32>(item1->GetUserData()));
	const AdvanceRecord *advance2 = g_theAdvanceDB->Get(
		reinterpret_cast<sint32>(item2->GetUserData()));

	
	
	if(!column || ((column != g_selected_item->GetVisiblePlayer()) &&
		!player->HasEmbassyWith(column))) {

		
		return(advance1->GetCost() - advance2->GetCost());
	}

	
	sint32 result = (g_player[column] && g_player[column]->HasAdvance(advance1->GetIndex())) ?
		((g_player[column] && g_player[column]->HasAdvance(advance2->GetIndex())) ? 0 : -1) :
		((g_player[column] && g_player[column]->HasAdvance(advance2->GetIndex())) ? 1 : 0);

	
	if(!result) {

		
		return(advance1->GetCost() - advance2->GetCost());
	}

	
	return(result);
}




void ScienceManagementDialog::AdvanceListCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	if (action != AUI_LISTBOX_ACTION_SELECT)
		return;
	
	ctp2_ListItem *item;

	
	ScienceManagementDialog *pMe = static_cast<ScienceManagementDialog*>(cookie);


	ctp2_ListBox *listbox =
		static_cast<ctp2_ListBox*>(control);


	
	item = (ctp2_ListItem *)listbox->GetSelectedItem();
	if (!item) return;

	sint32 index =  (sint32)item->GetUserData();


	

	if (pMe->m_scienceDescription == NULL) 
        return;

#define GIVES_TEXT_LEN 8192
	MBCHAR givesText[GIVES_TEXT_LEN];
	MBCHAR linkText[GIVES_TEXT_LEN];
	givesText[0] = 0;

	
	
	

	
	bool isAdvance = false;
	
	bool anyAdvance = false;


	sint32 i;
	for(i = 0; i < g_theUnitDB->NumRecords(); i++) 
	{
		const UnitRecord *rec = g_theUnitDB->Get(i);
		if(rec->GetEnableAdvanceIndex() == index) 
		{
			
			if( !isAdvance )
			{
				sprintf(givesText, "%s\n", g_theStringDB->GetNameStr("ADVANCE_GIVES_UNITS"));
				isAdvance = true;
				anyAdvance = true;
			}

			sprintf(linkText, "  <L:DATABASE_UNITS,%s><e>\n", rec->GetIDText());
			strncat(givesText, linkText, GIVES_TEXT_LEN - strlen(givesText));
		}
	}

	isAdvance = false;

	if(strlen(givesText) > GIVES_TEXT_LEN - 100) {
		return;
	}


	for(i = 0; i < g_theBuildingDB->NumRecords(); i++) {
		const BuildingRecord *rec = g_theBuildingDB->Get(i);
		if(rec->GetEnableAdvanceIndex() == index) 		
		{
			
			if( !isAdvance )
			{
				sprintf(givesText + strlen(givesText), "%s\n", 
					g_theStringDB->GetNameStr("ADVANCE_GIVES_BUILDINGS"));
				isAdvance = true;
				anyAdvance = true;
			}

			sprintf(linkText, "  <L:DATABASE_BUILDINGS,%s><e>\n", rec->GetIDText());
			strncat(givesText, linkText, GIVES_TEXT_LEN - strlen(givesText));
		}
	}

	if(strlen(givesText) > GIVES_TEXT_LEN - 100) {
		return;
	}

	isAdvance = false;


	for(i = 0; i < g_theWonderDB->NumRecords(); i++) {
		const WonderRecord *rec = g_theWonderDB->Get(i);

		if(rec->GetEnableAdvanceIndex() == index) 
		{
			
			if( !isAdvance )
			{
				sprintf(givesText + strlen(givesText), "%s\n", 
					g_theStringDB->GetNameStr("ADVANCE_GIVES_WONDERS"));
				isAdvance = true;
				anyAdvance = true;
			}

			sprintf(linkText, "  <L:DATABASE_WONDERS,%s><e>\n", rec->GetIDText());
			strncat(givesText, linkText, GIVES_TEXT_LEN - strlen(givesText));
		}
	}

	bool isTileImp = false;
	for(i = 0; i < g_theTerrainImprovementDB->NumRecords(); i++) {
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(i);

		if(terrainutil_AdvanceEnablesImprovement(index, i)) {
			if(!isTileImp) {
				sprintf(givesText + strlen(givesText), "%s\n",
						g_theStringDB->GetNameStr("ADVANCE_GIVES_TILE_IMPROVEMENTS"));
				isTileImp = true;
				anyAdvance = true;
			}

			sprintf(linkText, "  <L:DATABASE_TILE_IMPROVEMENTS,%s><e>\n", rec->GetIDText());
			strncat(givesText, linkText, GIVES_TEXT_LEN - strlen(givesText));
		}
	}

	bool isGov = false;
	for(i = 0; i < g_theGovernmentDB->NumRecords(); i++) {
		const GovernmentRecord *rec = g_theGovernmentDB->Get(i);
		if(rec->GetEnableAdvanceIndex() == index) {
			if(!isGov) {
				sprintf(givesText + strlen(givesText), "%s\n", g_theStringDB->GetNameStr("ADVANCE_GIVES_GOVS"));
				isGov = true;
				anyAdvance = true;
			}

			sprintf(linkText, "  <L:DATABASE_GOVERNMENTS,%s><e>\n", rec->GetIDText());
			strncat(givesText, linkText, GIVES_TEXT_LEN - strlen(givesText));
		}
	}

	
	if( !anyAdvance )
	{
		sprintf( givesText, "%s", g_theStringDB->GetNameStr( "ADVANCE_NO_SPECIFIC_ADVANCES" ) );
	}
	
	pMe->m_scienceDescription->SetHyperText(givesText);
	pMe->m_window->ShouldDraw(TRUE); 

}
