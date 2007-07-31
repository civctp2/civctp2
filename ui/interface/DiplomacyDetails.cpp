
#include "c3.h"
#include "DiplomacyDetails.h"

#include "aui_ldl.h"
#include "aui_uniqueid.h"

#include "ctp2_Window.h"
#include "ctp2_Menu.h"
#include "ctp2_Button.h"
#include "ctp2_Static.h"
#include "ctp2_DropDown.h"
#include "ctp2_ListBox.h"
#include "ctp2_ListItem.h"
#include "ctp2_Tab.h"
#include "ctp2_TabButton.h"
#include "ctp2_TabGroup.h"
#include "ctp2_Spinner.h"

#include "c3ui.h"

#include "DiplomacyWindow.h"
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

#include "Player.h"
#include "Civilisation.h"
#include "UnitDynArr.h"

#include "GameEventUser.h"
#include "Events.h"
#include "BuildingRecord.h"
#include "UnitData.h"
#include "CityWindow.h"
#include "wonderrecord.h"
#include "cellunitlist.h"
#include "world.h"
#include "cell.h"
#include "unitrecord.h"
#include "iconrecord.h"

#include "Mapanalysis.h"
#include "IntelligenceWindow.h"
#include "ColorSet.h"
#include "aui_blitter.h"
#include "aui_stringtable.h"
#include "AgreementMatrix.h"

#include "ctp2_HyperTextBox.h"

static DiplomacyDetails *s_DiplomacyDetails;
static MBCHAR *s_DiplomacyDetailsBlock = "DiplomacyDetails";

#define k_INT_FLAG_COL		0
#define k_INT_NATION_COL	1		
#define k_INT_REGARD_COL	2		
#define k_INT_STRENGTH_COL	3		
#define k_INT_EMBASSY_COL	4		
#define k_INT_TREATIES_COL	5		

#define k_WEAK_STRENGTH -50
#define k_EQUAL_STRENGTH 0
#define k_STRONG_STRENGTH 50

extern C3UI *g_c3ui;
ctp2_Button	*DiplomacyDetails::m_cancelButton = NULL;

ctp2_ListBox *DiplomacyDetails::sm_list = NULL;
extern ColorSet *g_colorSet;
aui_StringTable *DiplomacyDetails::sm_strengthImages = NULL;
aui_StringTable *DiplomacyDetails::sm_embassyImages = NULL;

sint32 DiplomacyDetails::detailPlayer;

DiplomacyDetails::DiplomacyDetails(AUI_ERRCODE *err)
{
	*err = AUI_ERRCODE_OK;
	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_DiplomacyDetailsBlock);
	Assert(m_window);
	if(!m_window) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	m_cancelButton = (ctp2_Button *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "CloseButton");
	if(m_cancelButton) m_cancelButton->SetActionFuncAndCookie(CancelCallback, NULL);

}

DiplomacyDetails::~DiplomacyDetails()
{
	if(m_window) {
		aui_Ldl::DeleteHierarchyFromRoot(s_DiplomacyDetailsBlock);
	}
	if(sm_strengthImages) {
		delete sm_strengthImages;
		sm_strengthImages = NULL;
	}

	if(sm_embassyImages) {
		delete sm_embassyImages;
		sm_embassyImages = NULL;
	}
}

AUI_ERRCODE DiplomacyDetails::Initialize()
{
	
	if(s_DiplomacyDetails)
		return AUI_ERRCODE_OK;

	
	AUI_ERRCODE err;
	s_DiplomacyDetails = new DiplomacyDetails(&err);

	Assert(err == AUI_ERRCODE_OK);

	return err;
}

AUI_ERRCODE DiplomacyDetails::Cleanup()
{
	if(s_DiplomacyDetails) {
		Hide();

		delete s_DiplomacyDetails;
		s_DiplomacyDetails = NULL;
	}
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE DiplomacyDetails::Display(Unit *cfdshk)
{
	if(!s_DiplomacyDetails)
		Initialize();

	Assert(s_DiplomacyDetails);
	if(!s_DiplomacyDetails) {
		return AUI_ERRCODE_HACK;
	}
	

	AUI_ERRCODE err = AUI_ERRCODE_INVALIDPARAM;

	Assert(s_DiplomacyDetails->m_window);
	if(s_DiplomacyDetails->m_window) {
		err = g_c3ui->AddWindow(s_DiplomacyDetails->m_window);
		Assert(err == AUI_ERRCODE_OK);
		if(err == AUI_ERRCODE_OK) {



			ctp2_Static *st;
			ctp2_HyperTextBox *htb;
			SlicObject so;
			
			so.AddPlayer(detailPlayer);
			so.AddGovernment(g_player[detailPlayer]->GetGovernmentType());

			



			
			MBCHAR interp[20000];

			const char *input = NULL;
			
			interp[0] = 0;
			input = g_theStringDB->GetNameStr("str_ldl_DipDetails_IntelReport");
			if(input)
				stringutils_Interpret(input, so, interp);
			st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "IntelReportLabel");
			st->SetText(interp);

			
			interp[0] = 0;
			input = g_theStringDB->GetNameStr("str_ldl_DipDetails_LeaderName");
			if(input)
				stringutils_Interpret(input, so, interp);
			st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "LeaderNameLabel");
			st->SetText(interp);

			
			interp[0] = 0;
			input = g_theStringDB->GetNameStr("str_ldl_DipDetails_LeaderPersonality");
			if(input)
				stringutils_Interpret(input, so, interp);
			st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "LeaderPersonalityLabel");
			st->SetText(interp);

			
			interp[0] = 0;

			
			StringId dip_adviceId = Diplomat::GetDiplomat(detailPlayer).ExplainRegard(g_selected_item->GetVisiblePlayer());
			if (dip_adviceId != -1)
			{
				stringutils_Interpret(g_theStringDB->GetNameStr(dip_adviceId), so, interp);
			}
			else
			{
				
				dip_adviceId = Diplomat::GetDiplomat(g_selected_item->GetVisiblePlayer()).GetDiplomacyAdvice(so, detailPlayer);
				if (dip_adviceId != -1)
				{
					stringutils_Interpret(g_theStringDB->GetNameStr(dip_adviceId), so, interp);
				}
			}

			
			if (dip_adviceId == -1)
			{
				stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipDetails_EmbassyMsg"), so, interp);
			}
			
			htb = (ctp2_HyperTextBox *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "EmbassyMsgLabel");
			htb->SetText(interp);

			
			interp[0] = 0;
			stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipDetails_KnownToGov"), so, interp);
			st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab3.TabPanel.KnownToGovLabel");
			st->SetText(interp);

			ctp2_ListItem *item;

			sint32 p;
			Player *visPl = g_player[detailPlayer];
			Assert(visPl);

			sm_list = (ctp2_ListBox *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab1.TabPanel.List");
			if(sm_list)
			{
				sm_list->Clear();
				sm_list->SetAbsorbancy(FALSE);
					
				for(p = 1; p < k_MAX_PLAYERS; p++) {
					if(p == detailPlayer) continue;
					if(!visPl->HasContactWith(p)) continue;

					
					item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("DiplomacyDetailsListItem");
					
					Assert(item);
					if(!item) break;

					ctp2_Static *child = NULL;
					MBCHAR buf[k_MAX_NAME_LEN];
					Civilisation civ = *g_player[p]->m_civilisation;

					
					if(child = (ctp2_Static *)item->GetChildByIndex(k_INT_FLAG_COL)) {
						child->SetDrawCallbackAndCookie(DrawPlayerFlag, (void *)p, false);
						child->SetActionFuncAndCookie(SelectItem, (void *)item);
					}
					if(child = (ctp2_Static *)item->GetChildByIndex(k_INT_NATION_COL)) {
						civ->GetCountryName(buf);
						child->SetText(buf);
						
						
						child->SetActionFuncAndCookie(SelectItem, (void *)item);
					}

					if(child = (ctp2_Static *)item->GetChildByIndex(k_INT_REGARD_COL)) {
						child->SetDrawCallbackAndCookie(DrawPlayerRegard, (void *)p, true);
						MBCHAR buf[k_MAX_NAME_LEN];
						sprintf(buf, "%s: %d", 
								g_theStringDB->GetNameStr("str_ldl_Regard"),
								Diplomat::GetDiplomat(p).GetPublicRegard(detailPlayer));
			
						child->SetActionFuncAndCookie(SelectItem, (void *)item);
					}

					if(child = (ctp2_Static *)item->GetChildByIndex(k_INT_STRENGTH_COL)) {
						child->SetDrawCallbackAndCookie(DrawPlayerStrength, (void *)p, true);

						MBCHAR buf[k_MAX_NAME_LEN];
						sprintf(buf, "%s: %d",
								g_theStringDB->GetNameStr("str_ldl_Strength"),
								MapAnalysis::GetMapAnalysis().TotalValue(p));
			
						child->SetActionFuncAndCookie(SelectItem, (void *)item);
					}

					if(child = (ctp2_Static *)item->GetChildByIndex(k_INT_EMBASSY_COL)) {
						child->SetDrawCallbackAndCookie(DrawEmbassy, (void *)p, true);
						child->SetActionFuncAndCookie(SelectItem, (void *)item);
					}

					if(child = (ctp2_Static *)item->GetChildByIndex(k_INT_TREATIES_COL)) {
						child->SetDrawCallbackAndCookie(DrawTreaties, (void *)p, true);
						
						
						child->SetActionFuncAndCookie(SelectItem, (void *)item);
					}

					item->SetUserData((void*)p);
					sm_list->AddItem(item);
				}
			}		



			
			MBCHAR needEmbassy[k_MAX_NAME_LEN];
			needEmbassy[0]=0;
			strcpy(needEmbassy,g_theStringDB->GetNameStr("str_ldl_DipDetails_NoEmbassy"));

			
			interp[0] = 0;
			

			
			
			sprintf(interp, g_theStringDB->GetNameStr("str_ldl_DipDetails_Pollution"), g_player[detailPlayer]->GetPollutionLevel());


			st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.PollutionLabel");
			st->SetText(interp);

			if(g_player[g_selected_item->GetVisiblePlayer()]->HasEmbassyWith(detailPlayer))
			{
				
				interp[0] = 0;
				stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipDetails_EmpireSize"), so, interp);
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.EmpireSizeLabel");
				st->SetText(interp);

				
				interp[0] = 0;
				
				sprintf(interp, g_theStringDB->GetNameStr("str_ldl_DipDetails_Population"), g_player[detailPlayer]->GetTotalPopulation()*k_PEOPLE_PER_POPULATION+g_player[detailPlayer]->GetPartialPopulation());
				
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.PopulationLabel");
				st->SetText(interp);

				
				interp[0] = 0;
				if(g_player[detailPlayer]->m_capitol && g_player[detailPlayer]->m_capitol->m_id)
				{
					stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipDetails_Capital"), so, interp);
				}
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.CapitalLabel");
				st->SetText(interp);

				
				interp[0] = 0;
				if(g_player[detailPlayer]->m_all_cities->Num())
				{
					stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipDetails_LargestCity"), so, interp);
				}
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.LargestCityLabel");
				st->SetText(interp);

				
				interp[0] = 0;
				stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipDetails_MilitaryState"), so, interp);
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.MilStateLabel");
				st->SetText(interp);

				
				interp[0] = 0;
				stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipDetails_GovType"), so, interp);
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.GovTypeLabel");
				st->SetText(interp);

				
				interp[0] = 0;
				stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipDetails_NowResearching"), so, interp);
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab3.TabPanel.NowResearchLabel");
				st->SetText(interp);

				
				sm_list = (ctp2_ListBox *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.WonderList");
				if(sm_list)
				{
					sm_list->Clear();
					sm_list->SetAbsorbancy(FALSE);
					for(int a = 0; a < g_player[detailPlayer]->m_all_cities->Num(); a++) {
						if(!g_player[detailPlayer]->m_all_cities->Get(a)->GetCityData()->GetBuiltWonders())
						{
							
							continue;
						}

						item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("DipDetWondersItem");
						ctp2_Static *child = NULL;
						if(child = (ctp2_Static *)item->GetChildByIndex(0)) {
							child->SetText(g_player[detailPlayer]->m_all_cities->Get(a)->GetCityData()->GetName());
						}

						sm_list->AddItem(item);
					}
				}


				
				sm_list = (ctp2_ListBox *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab3.TabPanel.ForAdvancesList");
				if(sm_list)
				{
					sm_list->Clear();
					sm_list->SetAbsorbancy(FALSE);
					for(int a = 0; a < g_theAdvanceDB->NumRecords(); a++) {
						if(g_player[g_selected_item->GetVisiblePlayer()]->HasAdvance(a))
						{
							
							continue;
						}

						if(!g_player[detailPlayer]->HasAdvance(a))
						{
							
							continue;
						}
						item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ForAdvancesItem");
						ctp2_Static *child = NULL;
						if(child = (ctp2_Static *)item->GetChildByIndex(0)) {
							child->SetText(g_theAdvanceDB->Get(a)->GetNameText());
						}

						sm_list->AddItem(item);
					}
				}

				
				sm_list = (ctp2_ListBox *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab3.TabPanel.YourAdvancesList");
				if(sm_list)
				{
					sm_list->Clear();
					sm_list->SetAbsorbancy(FALSE);
					for(int a = 0; a < g_theAdvanceDB->NumRecords(); a++) {
						if(!g_player[g_selected_item->GetVisiblePlayer()]->HasAdvance(a))
						{
							
							continue;
						}

						if(g_player[detailPlayer]->HasAdvance(a))
						{
							
							continue;
						}

						item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("YourAdvancesItem");
						ctp2_Static *child = NULL;
						if(child = (ctp2_Static *)item->GetChildByIndex(0)) {
							child->SetText(g_theAdvanceDB->Get(a)->GetNameText());
						}

						sm_list->AddItem(item);
					}
				}
			}
			else
			{
				
				interp[0] = 0;
				strcpy(interp,g_theStringDB->GetNameStr("str_ldl_DipDetails_EmpireSize"));
				
				if(strchr(interp, '{'))
					*strchr(interp,'{')=0;	
				strcat(interp,needEmbassy);
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.EmpireSizeLabel");
				st->SetText(interp);

				
				interp[0] = 0;
				strcpy(interp,g_theStringDB->GetNameStr("str_ldl_DipDetails_Population"));
				if(strchr(interp, '%'))
					*strchr(interp,'%')=0;	
				strcat(interp,needEmbassy);
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.PopulationLabel");
				st->SetText(interp);

				
				interp[0] = 0;
				strcpy(interp,g_theStringDB->GetNameStr("str_ldl_DipDetails_Capital"));
				if(strchr(interp, '{'))
					*strchr(interp,'{')=0;	
				strcat(interp,needEmbassy);
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.CapitalLabel");
				st->SetText(interp);

				
				interp[0] = 0;
				strcpy(interp,g_theStringDB->GetNameStr("str_ldl_DipDetails_LargestCity"));
				if(strchr(interp, '{'))
					*strchr(interp,'{')=0;	
				strcat(interp,needEmbassy);
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.LargestCityLabel");
				st->SetText(interp);

				
				interp[0] = 0;
				strcpy(interp,g_theStringDB->GetNameStr("str_ldl_DipDetails_MilitaryState"));
				if(strchr(interp, '{'))
					*strchr(interp,'{')=0;	
				strcat(interp,needEmbassy);
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.MilStateLabel");
				st->SetText(interp);

				
				interp[0] = 0;
				strcpy(interp,g_theStringDB->GetNameStr("str_ldl_DipDetails_GovType"));
				if(strchr(interp, '{'))
					*strchr(interp,'{')=0; 
				strcat(interp,needEmbassy);
				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.GovTypeLabel");
				st->SetText(interp);

				
				interp[0] = 0;
				strcpy(interp,g_theStringDB->GetNameStr("str_ldl_DipDetails_NowResearching"));
				if(strchr(interp, '{'))
					*strchr(interp,'{')=0;	

				st = (ctp2_Static *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab3.TabPanel.NowResearchLabel");
				st->SetText(interp);

				
				sm_list = (ctp2_ListBox *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab2.TabPanel.WonderList");
				if(sm_list)
				{
					sm_list->Clear();
				}


				
				needEmbassy[0]=0;
				strcpy(needEmbassy,g_theStringDB->GetNameStr("str_ldl_DipDetails_EmbassyReq"));

				
				sm_list = (ctp2_ListBox *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab3.TabPanel.ForAdvancesList");
				if(sm_list)
				{
					sm_list->Clear();
					
					
					ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ForAdvancesItem");
					Assert(item);
					if(item)
					{
						ctp2_Static *child = NULL;
						if(child = (ctp2_Static *)item->GetChildByIndex(0)) {
							child->SetText(needEmbassy);
						}
						sm_list->AddItem( item );
					}
				}

				
				sm_list = (ctp2_ListBox *)aui_Ldl::GetObject(s_DiplomacyDetailsBlock, "TabGroup.Tab3.TabPanel.YourAdvancesList");
				if(sm_list)
				{
					sm_list->Clear();
					
					
					ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("YourAdvancesItem");
					Assert(item);
					if(item) 
					{
						ctp2_Static *child = NULL;
						if(child = (ctp2_Static *)item->GetChildByIndex(0)) {
							child->SetText(needEmbassy);
						}
						sm_list->AddItem( item );
					}
				}
			}

			err = s_DiplomacyDetails->m_window->Show();
			s_DiplomacyDetails->m_window->ShouldDraw(TRUE);
		}
	}

	return err;
}

AUI_ERRCODE DiplomacyDetails::Hide()
{
	if(!s_DiplomacyDetails)
		return AUI_ERRCODE_OK;

	Assert(s_DiplomacyDetails->m_window);
	if(!s_DiplomacyDetails->m_window)
		return AUI_ERRCODE_INVALIDPARAM;

	s_DiplomacyDetails->m_window->Hide();
	return g_c3ui->RemoveWindow(s_DiplomacyDetails->m_window->Id());
}

void DiplomacyDetails::CancelCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Hide();
}

void DiplomacyDetails::SetNation(sint32 player) 
{
	detailPlayer=player;
}

AUI_ERRCODE DiplomacyDetails::DrawPlayerColor(ctp2_Static *control,
												 aui_Surface *surface,
												 RECT &rect,
												 void *cookie)
{
	
	sint32 player = (sint32)cookie;
	Assert(g_colorSet);
	if(!g_colorSet)
		return AUI_ERRCODE_INVALIDPARAM;
	
	RECT drawRect = rect;
	drawRect.top += 2;
	drawRect.bottom -= 2;
	drawRect.left += 2;

	return g_c3ui->TheBlitter()->ColorBlt16(surface, &drawRect, g_colorSet->GetPlayerColor(player), 0);
}

AUI_ERRCODE DiplomacyDetails::DrawPlayerFlag(ctp2_Static *control,
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

	rect.left += 2;
	rect.top += 8;
	rect.right -= 2;
	rect.bottom -= 8;

	return g_c3ui->TheBlitter()->ColorBlt16(surface, &rect, g_colorSet->GetPlayerColor(player), 0);	
}

void DiplomacyDetails::SelectItem(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != k_CTP2_STATIC_ACTION_LMOUSE) return;
	ctp2_ListItem *item = (ctp2_ListItem *)cookie;

	if(!sm_list) return;

	if(sm_list->GetSelectedItem() == item) {
		sm_list->DeselectItem(item);
	} else {
		sm_list->SelectItem(item);
	}
}

AUI_ERRCODE DiplomacyDetails::DrawPlayerRegard(ctp2_Static *control,
												 aui_Surface *surface,
												 RECT &rect,
												 void *cookie)
{
	aui_Image *image = NULL;
	MBCHAR *imageName = NULL;
	char **toneIcons = DiplomacyWindow::GetToneIcons();
	sint32 p = (sint32)cookie;
	







	
	switch(GetRegardThreshold(p, detailPlayer)) {
		case HOTWAR_REGARD: imageName = toneIcons[DIPLOMATIC_TONE_ANGRY]; break;
		case COLDWAR_REGARD: imageName = toneIcons[DIPLOMATIC_TONE_INDIGNANT]; break;
		case NEUTRAL_REGARD: imageName = toneIcons[DIPLOMATIC_TONE_EQUAL]; break;
		case FRIEND_REGARD: imageName = toneIcons[DIPLOMATIC_TONE_MEEK]; break;
		case ALLIED_REGARD: imageName = toneIcons[DIPLOMATIC_TONE_KIND]; break;
		default:
			Assert(FALSE);
			break;
	}

	
	if(imageName) {
		image = g_c3ui->LoadImage(imageName);
		if(image) {
			
			rect.left += ((rect.right - rect.left) / 2) - (image->TheSurface()->Width() / 2);
			rect.top += ((rect.bottom - rect.top) / 2) - (image->TheSurface()->Height() / 2);

			RECT srcRect = {
				0, 0,
				image->TheSurface()->Width(),
				image->TheSurface()->Height()
			};

			
			image->SetChromakey(255,0,255);

			g_c3ui->TheBlitter()->Blt(
				surface,
				rect.left,
				rect.top,
				image->TheSurface(),
				&srcRect,
				k_AUI_BLITTER_FLAG_CHROMAKEY
				);
			g_c3ui->UnloadImage(image);
		}
	}
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE DiplomacyDetails::DrawPlayerStrength(ctp2_Static *control,
												   aui_Surface *surface,
												   RECT &rect,
												   void *cookie)
{
	aui_Image *image = NULL;
	MBCHAR *imageName = NULL;
	sint32 p = (sint32)cookie;

	if(!g_player[p]) return AUI_ERRCODE_OK;
	if(!g_player[detailPlayer]) return AUI_ERRCODE_OK;

	






	sint32 myTotalStrength = 0;
	sint32 hisTotalStrength = 0;

	myTotalStrength = MapAnalysis::GetMapAnalysis().TotalValue(detailPlayer);
	hisTotalStrength = MapAnalysis::GetMapAnalysis().TotalValue(p);

	sint32 relativeStrength = hisTotalStrength - myTotalStrength;
	
	if(!sm_strengthImages) {
		InitImageTables();
	}

	if(relativeStrength < k_WEAK_STRENGTH) imageName = sm_strengthImages->GetString(0);
	else if(relativeStrength < k_EQUAL_STRENGTH) imageName = sm_strengthImages->GetString(1);
	else if(relativeStrength < k_STRONG_STRENGTH) imageName = sm_strengthImages->GetString(2);
	else imageName = sm_strengthImages->GetString(3);
											   
	
	if(imageName) {
		image = g_c3ui->LoadImage(imageName);
		if(image) {
			
			rect.left += ((rect.right - rect.left) / 2) - (image->TheSurface()->Width() / 2);
			rect.top += ((rect.bottom - rect.top) / 2) - (image->TheSurface()->Height() / 2);

			RECT srcRect = {
				0, 0,
				image->TheSurface()->Width(),
				image->TheSurface()->Height()
			};

			
			image->SetChromakey(255,0,255);

			g_c3ui->TheBlitter()->Blt(
				surface,
				rect.left,
				rect.top,
				image->TheSurface(),
				&srcRect,
				k_AUI_BLITTER_FLAG_CHROMAKEY
				);
			g_c3ui->UnloadImage(image);
		}
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE DiplomacyDetails::DrawEmbassy(ctp2_Static *control,
											aui_Surface *surface,
											RECT &rect,
											void *cookie)
{
	aui_Image *image = NULL;
	MBCHAR *imageName = NULL;
	sint32 p = (sint32)cookie;

	






	if(!sm_embassyImages) {
		InitImageTables();
	}

	if(g_player[detailPlayer]->HasEmbassyWith(p)) {
		imageName = sm_embassyImages->GetString(0);
		if(imageName) {
			aui_Image *image = g_c3ui->LoadImage(imageName);
			if(image) {
				
				rect.left += ((rect.right - rect.left) / 2) - (image->TheSurface()->Width() / 2);
				rect.top += ((rect.bottom - rect.top) / 2) - (image->TheSurface()->Height() / 2);
				
				RECT srcRect = {
					0, 0,
					image->TheSurface()->Width(),
					image->TheSurface()->Height()
				};
				
				
				image->SetChromakey(255,0,255);
				
				g_c3ui->TheBlitter()->Blt(
					surface,
					rect.left,
					rect.top,
					image->TheSurface(),
					&srcRect,
					k_AUI_BLITTER_FLAG_CHROMAKEY
					);
				g_c3ui->UnloadImage(image);
			}
		}
	} else {
		
	}
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE DiplomacyDetails::DrawTreaties(ctp2_Static *control,
											 aui_Surface *surface,
											 RECT &rect,
											 void *cookie)
{
	sint32 p = (sint32)cookie;
	sint32 visP = g_selected_item->GetVisiblePlayer();

	







	sint32 x = 0;

	
	
	

	sint32 ag;
	sint32 slot;
	for(ag = 1; ag < PROPOSAL_MAX; ag++) {
		
		
		
		
		

		const DiplomacyProposalRecord *rec = 
			g_theDiplomacyProposalDB->Get(diplomacyutil_GetDBIndex((PROPOSAL_TYPE)ag));

		if (!rec->GetImageSlot())
			continue;

		rec->GetImageSlot(slot);

		
		
		if (p == visP)
		{
			
			if (slot > 4)
				continue;
		}
		else
		{
			
			if (slot < 4)
				slot = 0;
			else
				slot -= 3;
		}

		if(AgreementMatrix::s_agreements.HasAgreement(detailPlayer, p, (PROPOSAL_TYPE)ag)) {
			aui_Image *image = g_c3ui->LoadImage((char *)rec->GetImage());
			Assert(image);
			if(!image)
				continue;
			RECT srcRect = {
				0, 0,
				image->TheSurface()->Width(),
				image->TheSurface()->Height()
			};

			image->SetChromakey(255,0,255);

			
			x = image->TheSurface()->Width() * slot;

			g_c3ui->TheBlitter()->Blt(surface, rect.left + x, 
									  rect.top + (((rect.bottom - rect.top) - image->TheSurface()->Height()) / 2),
									  image->TheSurface(),
									  &srcRect,
									  k_AUI_BLITTER_FLAG_CHROMAKEY);

			
			

			g_c3ui->UnloadImage(image);
		}

		
		
		
		
		
		
		
	}
	return AUI_ERRCODE_OK;
}

sint32 DiplomacyDetails::GetRegardThreshold(sint32 ofPlayer, sint32 forPlayer)
{
	
	ai::Regard airegard = Diplomat::GetDiplomat(ofPlayer).GetPublicRegard(forPlayer);

	if(airegard <= HOTWAR_REGARD) return HOTWAR_REGARD;
	else if(airegard <= COLDWAR_REGARD) return COLDWAR_REGARD;
	else if(airegard >= ALLIED_REGARD) return ALLIED_REGARD;
	else if(airegard >= FRIEND_REGARD) return FRIEND_REGARD;
	return NEUTRAL_REGARD;
}

void DiplomacyDetails::InitImageTables()
{
	AUI_ERRCODE err;
	if(!sm_strengthImages) {
		sm_strengthImages = new aui_StringTable(&err, "StrengthImages");
	}

	if(!sm_embassyImages) {
		sm_embassyImages = new aui_StringTable(&err, "EmbassyImages");
	}
}

