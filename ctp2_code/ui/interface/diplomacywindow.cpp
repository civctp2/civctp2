//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Diplomacy window
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
// - Keep the embargo and war buttons enabled until confirmed by the player.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "diplomacywindow.h"

#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_blitter.h"
#include "c3ui.h"

#include "ctp2_Window.h"
#include "ctp2_button.h"
#include "ctp2_listbox.h"
#include "ctp2_Static.h"
#include "ctp2_listitem.h"
#include "ctp2_Menu.h"

#include "Diplomat.h"
#include "diplomacyutil.h"

#include "SelItem.h"
#include "player.h"
#include "StrDB.h"
#include "Civilisation.h"
#include "UnitDynArr.h"

#include "GameEventUser.h"
#include "Events.h"

#include "pixelutils.h"
#include "colorset.h"

#include "DiplomacyProposalRecord.h"
#include "DiplomacyThreatRecord.h"
#include "AdvanceRecord.h"

#include "SlicObject.h"
#include "stringutils.h"

#include "Gold.h"
#include "ctp2_spinner.h"

#include "intelligencewindow.h"
#include "AttractWindow.h"

#include "ctp2_TabGroup.h"
#include "ctp2_Tab.h"
#include "ctp2_button.h"

#include "AgreementMatrix.h"
#include "DiplomacyDetails.h"
#include "Diplomat.h"

static MBCHAR                 *s_dipWindowBlock = "DiplomacyWindow";
static DiplomacyWindow        *s_dipWindow;
extern C3UI                   *g_c3ui;
extern ColorSet               *g_colorSet;
static MBCHAR                 *k_DIP_WINDOW_ATTRACT_BUTTON = "ControlPanelWindow.ControlPanel.ShortcutPad.DiplomacyButton";

#define k_INTELLIGENCE_TAB    0
#define k_NEGOTIATION_TAB     1
#define k_CREATE_PROPOSAL_TAB 2

char *DiplomacyWindow::sm_toneIcons[DIPLOMATIC_TONE_MAX] = {
	"updi39.tga",
	"updi40.tga",
	"updi41.tga",
	"updi42.tga",
	"updi43.tga"
};

DiplomacyWindow::DiplomacyWindow(AUI_ERRCODE *err)
{
	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_dipWindowBlock);
	Assert(m_window);
	if(!m_window) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	
	
	

	m_goldRequestWindow = NULL;
	m_pollutionRequestWindow = NULL;
	m_percentRequestWindow = NULL;

	m_create_mode = DW_CREATE_MODE_NONE;
	m_sendToCiv = -1;
	m_sendTone = DIPLOMATIC_TONE(sint32(DIPLOMATIC_TONE_MAX) / 2);
	m_curMenu = NULL;
	m_threatMenu = NULL;
	m_getRequest = true;
	m_sendCounter = false;
	m_selectingProgramatically = false;

	aui_Ldl::SetActionFuncAndCookie(s_dipWindowBlock, "CloseButton", DiplomacyWindow::Close, NULL);










	

























	
	
	

	
	sm_messageButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Intelligence.TabPanel.MessageButton");
	sm_messageButton->SetActionFuncAndCookie(SendMessage, NULL);
	sm_messageButton->Enable(FALSE);

	sm_warButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Intelligence.TabPanel.WarButton");
	sm_warButton->SetActionFuncAndCookie(DeclareWar, NULL);
	sm_warButton->Enable(FALSE);

	sm_embargoButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Intelligence.TabPanel.EmbargoButton");
	sm_embargoButton->SetActionFuncAndCookie(DeclareEmbargo, NULL);
	sm_embargoButton->Enable(FALSE);

	sm_detailsButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Intelligence.TabPanel.DetailsButton");
	sm_detailsButton->SetActionFuncAndCookie(Details, NULL);
	sm_detailsButton->Enable(FALSE);

	




	SetSendProposal(-1);

	*err = AUI_ERRCODE_OK;
}

ctp2_Button * DiplomacyWindow::sm_detailsButton;
ctp2_Button * DiplomacyWindow::sm_warButton;
ctp2_Button * DiplomacyWindow::sm_embargoButton;
ctp2_Button * DiplomacyWindow::sm_messageButton;

DiplomacyWindow::~DiplomacyWindow()
{
	if(m_window) {
		aui_Ldl::DeleteHierarchyFromRoot("DiplomacyWindow");
		m_window = NULL;
	}

	if(m_goldRequestWindow) {
		aui_Ldl::DeleteHierarchyFromRoot("DipGoldRequest");
		m_goldRequestWindow = NULL;
	}

	if(m_pollutionRequestWindow) {
		aui_Ldl::DeleteHierarchyFromRoot("DipPollutionRequest");
		m_pollutionRequestWindow = NULL;
	}

	if(m_percentRequestWindow) {
		aui_Ldl::DeleteHierarchyFromRoot("DipPercentRequest");
		m_percentRequestWindow = NULL;
	}

	if(m_curMenu) {
		delete m_curMenu;
		m_curMenu = NULL;
	}

	if(m_threatMenu) {
		delete m_threatMenu;
		m_threatMenu = NULL;
	}
}

AUI_ERRCODE DiplomacyWindow::Initialize()
{
	
	if(s_dipWindow)
		return AUI_ERRCODE_OK;

	
	AUI_ERRCODE err = AUI_ERRCODE_OK;
	s_dipWindow = new DiplomacyWindow(&err);

	Assert(err == AUI_ERRCODE_OK);

	return err;
}

AUI_ERRCODE DiplomacyWindow::Cleanup()
{
	if(s_dipWindow) {
		Hide();

		delete s_dipWindow;
		s_dipWindow = NULL;
	}
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE DiplomacyWindow::Display()
{
	if(!s_dipWindow)
		Initialize();

	Assert(s_dipWindow);
	if(!s_dipWindow) {
		return AUI_ERRCODE_HACK;
	}
	
	if(IsShown()) {
		
		return AUI_ERRCODE_OK;
	}

	AUI_ERRCODE err = AUI_ERRCODE_INVALIDPARAM;

	Assert(s_dipWindow->m_window);
	if(s_dipWindow->m_window) {
		err = g_c3ui->AddWindow(s_dipWindow->m_window);
		Assert(err == AUI_ERRCODE_OK);
		if(err == AUI_ERRCODE_OK) {
			err = s_dipWindow->m_window->Show();
		}

		s_dipWindow->ShowInitialSections();

		
		s_dipWindow->Update();
		
		
		if(g_attractWindow) {
			g_attractWindow->RemoveControl(k_DIP_WINDOW_ATTRACT_BUTTON);
		}

		
		EnableButtons(FALSE);









	}

	return err;
}

AUI_ERRCODE DiplomacyWindow::Hide()
{
    
	
	
	
	if(!s_dipWindow)
		return AUI_ERRCODE_OK;

	Assert(s_dipWindow->m_window);
	if(!s_dipWindow->m_window)
		return AUI_ERRCODE_INVALIDPARAM;

	s_dipWindow->m_window->Hide();
	return g_c3ui->RemoveWindow(s_dipWindow->m_window->Id());
}


void DiplomacyWindow::GotoTab(sint32 tabIndex)
{
	Assert(FALSE);
















}

bool DiplomacyWindow::IsShown()
{
	if(!s_dipWindow) return false;

	if(!s_dipWindow->m_window) return false;

	if(!g_c3ui) return false;

	if(!g_c3ui->GetWindow(s_dipWindow->m_window->Id()))
		return false;

	return true;
}

void DiplomacyWindow::ShowInitialSections()
{





	



	

}

void DiplomacyWindow::ShowSection(MBCHAR *section, bool show)
{
	aui_Region *component = (aui_Region *)aui_Ldl::GetObject(s_dipWindowBlock, section);
	Assert(component);
	if(component) {
		if(show) {
			component->Show();
		} else {
			component->Hide();
		}
	}
}

void DiplomacyWindow::ShowSections(uint32 sections)
{




























































































	Update();
}

void DiplomacyWindow::Update()
{
	static bool updating = false;

	if(updating)
		return;
	updating = true;

	






	






	






	







	






	






	ctp2_ListBox *intList = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Intelligence.TabPanel.List");
	Assert(intList);
	if(intList) {
		
		
		IntelligenceWindow::Update(intList);
	}

	updating = false;
}

void DiplomacyWindow::UpdateProposalList(ctp2_ListBox *propList, bool toPlayer)
{
	sint32 player = g_selected_item->GetVisiblePlayer();
	sint32 i;
	bool selectedSomething = false;

	Assert(propList);
	if(propList) {
		ctp2_ListItem *oldItem = (ctp2_ListItem *)propList->GetSelectedItem();
		sint32 oldSelectedPlayer = -1;
		const Response *response = NULL;
		if(oldItem) {
			if(toPlayer) {
#if defined(__LP64__)
				oldSelectedPlayer = (sint64)oldItem->GetUserData();
			} else {
				oldSelectedPlayer = (sint64)oldItem->GetUserData();
#else
				oldSelectedPlayer = (sint32)oldItem->GetUserData();
			} else {
				oldSelectedPlayer = (sint32)oldItem->GetUserData();
#endif
			}
		}

		propList->Clear();

		
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i] && player != i) {
				const NewProposal *prop;
				ProposalData propData;
				sint32 sender, receiver;

				
				if(toPlayer) {				
					
					sender = i;
					receiver = player;
					response = &Diplomat::GetDiplomat(player).GetResponsePending(i);				

					
					if (*response == Diplomat::s_badResponse)
						
						prop = &Diplomat::GetDiplomat(i).GetNewProposalPending(player);
					else
						
						prop = &Diplomat::GetDiplomat(i).GetMyLastNewProposal(player);

					
















					if(*prop == Diplomat::s_badNewProposal)
						continue;

					propData = prop->detail;

					
					if(!response) {
						response = &Diplomat::GetDiplomat(i).GetResponsePending(g_selected_item->GetVisiblePlayer());
						if(*response == Diplomat::s_badResponse) {
							response = NULL;
						} else if(response->type != RESPONSE_THREATEN) {
							response = NULL;
						}
					}
				} else {
					
					sender = player;
					receiver = i;

					response = &Diplomat::GetDiplomat(receiver).GetResponsePending(player);				

					
					if (*response == Diplomat::s_badResponse)
						
						prop = &Diplomat::GetDiplomat(player).GetNewProposalPending(receiver);
					else
						
						prop = &Diplomat::GetDiplomat(player).GetMyLastNewProposal(receiver);

					if(*prop == Diplomat::s_badNewProposal)
						continue;

					









					propData = prop->detail;
				}

				ctp2_ListItem *item;
				if(toPlayer) {
					item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ProposalReceivedListItem");
				} else {
					item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ProposalMadeListItem");
				}
				Assert(item);
				if(item) {
					
					ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
					Assert(box);
					if(box) {
						ctp2_Static *textStatic = (ctp2_Static *)box->GetChildByIndex(1);
						Assert(textStatic);
						if(textStatic) {
							MBCHAR finalText[k_MAX_NAME_LEN];
							if(toPlayer) {
								strcpy(finalText, g_theStringDB->GetNameStr("str_ldl_From"));
								strcat(finalText, " ");
								g_player[sender]->m_civilisation->GetCountryName(finalText + strlen(finalText));
								strcat(finalText, ": ");
							} else {
								strcpy(finalText, g_theStringDB->GetNameStr("str_ldl_To"));
								strcat(finalText, " ");
								g_player[receiver]->m_civilisation->GetCountryName(finalText + strlen(finalText));
								strcat(finalText, ": ");
							}

							
							GetProposalSummary(propData, sender, receiver, finalText + strlen(finalText), false, response);
							textStatic->SetText(finalText);
						}

						ctp2_Static *attitude = (ctp2_Static *)box->GetChildByIndex(0);
						Assert(attitude);
						if(attitude) {
							if(toPlayer) {
								attitude->SetImage(sm_toneIcons[propData.tone]);
							} else {
								DIPLOMATIC_TONE tone = (m_sendTone > DIPLOMATIC_TONE_NOT_CHOSEN) && (m_sendTone < DIPLOMATIC_TONE_MAX) ? m_sendTone : DIPLOMATIC_TONE_EQUAL;

								attitude->SetImage(sm_toneIcons[tone]);
							}									
						}
					}
					
					
					item->SetUserData((void *)i);
					propList->AddItem(item);
					
					if(oldSelectedPlayer == i) {
						selectedSomething = true;
						propList->SelectItem(item);
					}
				}
			}
		}
	}
}

void DiplomacyWindow::GetProposalDetails(const ProposalData &prop, 
										 sint32 sender, 
										 sint32 receiver,
										 MBCHAR *finalText,
										 MBCHAR *exchangeText,
										 bool isCounter,
										 const Response *response)
{
	SlicObject so;
	so.AddPlayer(sender);
	so.AddPlayer(receiver);
	sint32 dbIndex = diplomacyutil_GetDBIndex(prop.first_type);
	
	AddProposalData(so, dbIndex, prop.first_arg);

	StringId origText = GetProposalText(dbIndex, prop.tone, false, isCounter);
	stringutils_Interpret(g_theStringDB->GetNameStr(origText), so, finalText);
	
	if(prop.second_type != PROPOSAL_NONE) {
		
		dbIndex = diplomacyutil_GetDBIndex(prop.second_type);
		SlicObject eso;
		eso.AddPlayer(sender);
		eso.AddPlayer(receiver);
		AddProposalData(eso, dbIndex, prop.second_arg);
		origText = GetProposalText(dbIndex, prop.tone, true, isCounter);
		
		MBCHAR *exch;
		if(exchangeText)
			exch = exchangeText;
		else {
			strcat(finalText, "  ");
			exch = finalText + strlen(finalText);
		}
		stringutils_Interpret(g_theStringDB->GetNameStr(origText), eso, exch);
	}

	if(response) {
		if(response->type == RESPONSE_THREATEN) {
			sint32 threatDBIndex = diplomacyutil_GetDBIndex(response->threat.type);
			Assert((threatDBIndex >= 0) && (threatDBIndex < g_theDiplomacyThreatDB->NumRecords()));
			if((threatDBIndex >= 0) && (threatDBIndex < g_theDiplomacyThreatDB->NumRecords())) {
				SlicObject tso;
				tso.AddPlayer(sender);
				tso.AddPlayer(receiver);
				AddThreatData(tso, threatDBIndex, response->threat.arg);
				origText = g_theDiplomacyThreatDB->Get(threatDBIndex)->GetDetails();
				strcat(finalText, "  ");
				stringutils_Interpret(g_theStringDB->GetNameStr(origText), tso, finalText + strlen(finalText));
			}
		}
	}
}

void DiplomacyWindow::GetProposalSummary(const ProposalData &prop, 
										 sint32 sender, 
										 sint32 receiver,
										 MBCHAR *finalText,
										 bool isCounter,
										 const Response *response)
{
	SlicObject so;
	so.AddPlayer(sender);
	so.AddPlayer(receiver);
	sint32 dbIndex = diplomacyutil_GetDBIndex(prop.first_type);
	
	AddProposalData(so, dbIndex, prop.first_arg);

	StringId origText = GetProposalSummaryText(dbIndex, prop.tone, false, isCounter);
	stringutils_Interpret(g_theStringDB->GetNameStr(origText), so, finalText);
	
	if(prop.second_type != PROPOSAL_NONE) {
		
		dbIndex = diplomacyutil_GetDBIndex(prop.second_type);
		SlicObject eso;
		eso.AddPlayer(sender);
		eso.AddPlayer(receiver);
		AddProposalData(eso, dbIndex, prop.second_arg);
		origText = GetProposalSummaryText(dbIndex, prop.tone, true, isCounter);
		
		strcat(finalText, "  ");
		stringutils_Interpret(g_theStringDB->GetNameStr(origText), eso, finalText + strlen(finalText));
	}

	if(response) {
		if(response->type == RESPONSE_THREATEN) {
			sint32 threatDBIndex = diplomacyutil_GetDBIndex(response->threat.type);
			Assert((threatDBIndex >= 0) && (threatDBIndex < g_theDiplomacyThreatDB->NumRecords()));
			if((threatDBIndex >= 0) && (threatDBIndex < g_theDiplomacyThreatDB->NumRecords())) {
				SlicObject tso;
				tso.AddPlayer(sender);
				tso.AddPlayer(receiver);
				AddThreatData(tso, threatDBIndex, response->threat.arg);
				origText = g_theDiplomacyThreatDB->Get(threatDBIndex)->GetDetails();
				strcat(finalText, "  ");
				stringutils_Interpret(g_theStringDB->GetNameStr(origText), tso, finalText + strlen(finalText));
			}
		}
	}
}

void DiplomacyWindow::UpdateProposalsReceived()
{
	


}

void DiplomacyWindow::UpdateProposalsMade()
{
	


}

void DiplomacyWindow::UpdateProposalDetails()
{
	













































































}

void DiplomacyWindow::UpdateCreateList()
{
































































































































































}

void DiplomacyWindow::UpdateSendProposalDetails()
{
	











































































































}

void DiplomacyWindow::SetViewingResponse(sint32 sender, sint32 receiver)
{
	m_viewResponseSender = sender;
	m_viewResponseReceiver = receiver;
}

void DiplomacyWindow::UpdateResponseDetails()
{



















































































































}

void DiplomacyWindow::UpdateThreatList()
{
















































































}

StringId DiplomacyWindow::GetProposalText(sint32 proposal, DIPLOMATIC_TONE tone, bool isExchange, bool useReciprocal)
{
	const DiplomacyProposalRecord *rec = g_theDiplomacyProposalDB->Get(proposal);
	Assert(rec);
	if(!rec)
		return 0;

	

	if(useReciprocal) {
		
		
		sint32 recipIndex;
		if(rec->GetReciprocalIndex(recipIndex)) {
			rec = g_theDiplomacyProposalDB->Get(recipIndex);
		}
	}

	const DiplomacyProposalRecord::StrengthStrings *strings;

	bool gotTone = false;
	switch(tone) {
		case DIPLOMATIC_TONE_KIND:
			if(isExchange) {
				gotTone = rec->GetDetailsEx0(strings);
			} else {
				gotTone = rec->GetDetails0(strings);
			}
			break;
		case DIPLOMATIC_TONE_EQUAL:
			if(isExchange) {
				gotTone = rec->GetDetailsEx1(strings);
			} else {
				gotTone = rec->GetDetails1(strings);
			}
			break;
		case DIPLOMATIC_TONE_MEEK:
			if(isExchange) {
				gotTone = rec->GetDetailsEx2(strings);
			} else {
				gotTone = rec->GetDetails2(strings);
			}
			break;
		case DIPLOMATIC_TONE_INDIGNANT:
			if(isExchange) {
				gotTone = rec->GetDetailsEx3(strings);
			} else {
				gotTone = rec->GetDetails3(strings);
			}
			break;
		case DIPLOMATIC_TONE_ANGRY:
			if(isExchange) {
				gotTone = rec->GetDetailsEx4(strings);
			} else {
				gotTone = rec->GetDetails4(strings);
			}
			break;
		default:
			Assert(FALSE);
			if(isExchange) {
				gotTone = rec->GetDetailsEx2(strings);
			} else {
				gotTone = rec->GetDetails2(strings);
			}
			break;
	}

	if(!gotTone) {
		if(isExchange) {
			gotTone = rec->GetDetailsEx2(strings);
		} else {
			gotTone = rec->GetDetails2(strings);
		}
	}
	
	
	Assert(gotTone);
	if(!gotTone) {
		return 0;
	}

	
	StringId strId;
	if(strings->GetDetailsEven(strId)) {
		return strId;
	}

	Assert(FALSE);
	return 0;
}

StringId DiplomacyWindow::GetProposalSummaryText(sint32 proposal, DIPLOMATIC_TONE tone, bool isExchange, bool useReciprocal)
{
	const DiplomacyProposalRecord *rec = g_theDiplomacyProposalDB->Get(proposal);
	Assert(rec);
	if(!rec)
		return 0;

	

	if(useReciprocal) {
		
		
		sint32 recipIndex;
		if(rec->GetReciprocalIndex(recipIndex)) {
			rec = g_theDiplomacyProposalDB->Get(recipIndex);
		}
	}

	if(rec) {
		return rec->GetTitle();
	} else {
		Assert(rec);
		return 0;
	}
}

bool DiplomacyWindow::AddProposalData(SlicObject &so, sint32 proposal, DiplomacyArg arg)
{
	const DiplomacyProposalRecord *rec = g_theDiplomacyProposalDB->Get(proposal);
	Assert(rec);
	if(!rec)
		return false;

	
	
	
	switch(rec->GetArg1()) {
		case k_DiplomacyProposal_Arg1_OwnCity_Bit:
		case k_DiplomacyProposal_Arg1_HisCity_Bit:
		{
			Unit city(arg.cityId);
			if(city.IsValid()) {
				so.AddCity(Unit(arg.cityId));
				return true;
			} else {
				return false;
			}
		}
		case k_DiplomacyProposal_Arg1_OwnArmy_Bit:
			return true;
		case k_DiplomacyProposal_Arg1_HisArmy_Bit:
			return true;
		case k_DiplomacyProposal_Arg1_HisAgreement_Bit:
			return true;
		case k_DiplomacyProposal_Arg1_OwnAdvance_Bit:
		case k_DiplomacyProposal_Arg1_HisAdvance_Bit:
		case k_DiplomacyProposal_Arg1_OwnStopResearch_Bit:
		case k_DiplomacyProposal_Arg1_HisStopResearch_Bit:
		{
			if((arg.advanceType >= 0) && (arg.advanceType < g_theAdvanceDB->NumRecords())) {
				so.AddAdvance(arg.advanceType);
				return true;
			} else {
				return false;
			}
		}
		case k_DiplomacyProposal_Arg1_OwnUnitType_Bit:
			return true;
		case k_DiplomacyProposal_Arg1_OwnGood_Bit:
		case k_DiplomacyProposal_Arg1_HisGood_Bit:
			return true;
		case k_DiplomacyProposal_Arg1_OwnGold_Bit:
		case k_DiplomacyProposal_Arg1_HisGold_Bit:
			so.AddGold(arg.gold);
			return true;
		case k_DiplomacyProposal_Arg1_ThirdParty_Bit:
			so.AddPlayer(arg.playerId);
			return true;
		case k_DiplomacyProposal_Arg1_OwnPollution_Bit:
		case k_DiplomacyProposal_Arg1_HisPollution_Bit:
			so.AddInt(arg.pollution);
			return true;
		case k_DiplomacyProposal_Arg1_Percent_Bit:
			so.AddInt(static_cast<sint32>(arg.percent * 100.0));
			return true;
		default:
			return true;
	}
}

bool DiplomacyWindow::AddThreatData(SlicObject &so, sint32 threat, const DiplomacyArg &arg)
{
	
	
	const DiplomacyThreatRecord *rec = g_theDiplomacyThreatDB->Get(threat);
	Assert(rec);
	if(!rec)
		return false;

	switch(rec->GetArg1()) {
		case k_DiplomacyThreat_Arg1_HisCity_Bit:
		case k_DiplomacyThreat_Arg1_SpecialAttack_Bit:
		{
			Unit city(arg.cityId);
			if(city.IsValid()) {
				so.AddCity(Unit(arg.cityId));
				return true;
			} else {
				return false;
			}
		}
		case k_DiplomacyThreat_Arg1_ThirdParty_Bit:
		{
			so.AddPlayer(arg.playerId);
			return true;
		}
		default:
			return true;
	}
	Assert(false); 
	return false;
}

void DiplomacyWindow::Close(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;















	DiplomacyWindow::Hide();
}

void DiplomacyWindow::ProposalsReceived(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT && action != AUI_LISTBOX_ACTION_SELECT) return;
	
	Assert(s_dipWindow);
	if(s_dipWindow) {
		s_dipWindow->ShowReceivedProposalDetails();
	}
}

void DiplomacyWindow::ShowReceivedProposalDetails()
{


























}
	
void DiplomacyWindow::Respond(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(s_dipWindow) {
		s_dipWindow->ShowReceivedProposalDetails();
	}
}

void DiplomacyWindow::Respond(RESPONSE_TYPE disposition)
{




























}

void DiplomacyWindow::Accept(aui_Control *control, uint32 action, uint32 data, void *cookie)
{






}

void DiplomacyWindow::Reject(aui_Control *control, uint32 action, uint32 data, void *cookie)
{






}

void DiplomacyWindow::Counter(aui_Control *control, uint32 action, uint32 data, void *cookie)
{














































}

void DiplomacyWindow::Create(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
















}

void DiplomacyWindow::SendProposal()
{












}

void DiplomacyWindow::SendCounter()
{













}
	
void DiplomacyWindow::Send(aui_Control *control, uint32 action, uint32 data, void *cookie)
{































}
		
STDEHANDLER(DipWinContinueDiplomacyEvent)
{
	if(!s_dipWindow) return GEV_HD_Continue;

	sint32 p1, p2;
	if(!args->GetPlayer(0, p1)) return GEV_HD_Continue;
	if(!args->GetPlayer(1, p2)) return GEV_HD_Continue;

	s_dipWindow->Update();
	return GEV_HD_Continue;
}

STDEHANDLER(DipWinResponseReady)
{
	if(!s_dipWindow) return GEV_HD_Continue;

	sint32 p1, p2;
	if(!args->GetPlayer(0, p1)) return GEV_HD_Continue;
	if(!args->GetPlayer(1, p2)) return GEV_HD_Continue;

	if(p1 == g_selected_item->GetVisiblePlayer()) {
		
		s_dipWindow->Display();
		s_dipWindow->SetViewingResponse(p1, p2);

		
		ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Negotiations.TabPanel.ProposalsMadeBox.List");
		Assert(lb);
		sint32 i;
		for(i = 0; i < lb->NumItems(); i++) {
			ctp2_ListItem *item = (ctp2_ListItem *)lb->GetItemByIndex(i);
#if defined(__LP64__)
			if(sint64(item->GetUserData()) == p2) {
#else
			if(sint32(item->GetUserData()) == p2) {
#endif
				lb->SelectItem(item);
				break;
			}
		}

		RESPONSE_TYPE rtype = Diplomat::GetDiplomat(p2).GetResponsePending(p1).type;
		
		if((rtype == RESPONSE_COUNTER) || (rtype == RESPONSE_REJECT)) {
			
			s_dipWindow->ShowSections(k_DIPWIN_RESPONSE | k_DIPWIN_RESPONSE_WITH_COUNTER | k_DIPWIN_PROPOSALS_MADE | k_DIPWIN_PROPOSALS_RECEIVED);
		} else {
			s_dipWindow->ShowSections(k_DIPWIN_RESPONSE | k_DIPWIN_PROPOSALS_MADE | k_DIPWIN_PROPOSALS_RECEIVED);
		}
	} else if(p2 == g_selected_item->GetVisiblePlayer()) {
		
		ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Negotiations.TabPanel.ProposalsReceivedBox.List");
		Assert(lb);
		sint32 i;
		for(i = 0; i < lb->NumItems(); i++) {
			ctp2_ListItem *item = (ctp2_ListItem *)lb->GetItemByIndex(i);
#if defined(__LP64__)
			if(sint64(item->GetUserData()) == p1) {
#else
			if(sint32(item->GetUserData()) == p1) {
#endif
				lb->SelectItem(item);
				break;
			}
		}

		if(!DiplomacyWindow::IsShown()) {
			g_attractWindow->HighlightControl(k_DIP_WINDOW_ATTRACT_BUTTON);
		} else {
			g_attractWindow->RemoveControl(k_DIP_WINDOW_ATTRACT_BUTTON);
			
			s_dipWindow->ShowReceivedProposalDetails();
		}
	}
	return GEV_HD_Continue;
}

STDEHANDLER(DipWinNewProposalEvent)
{
	sint32 p1, p2;
	if(!args->GetPlayer(0, p1)) return GEV_HD_Continue;
	if(!args->GetPlayer(1, p2)) return GEV_HD_Continue;

	if(p2 == g_selected_item->GetVisiblePlayer()) {

		if(!g_attractWindow) {
			AttractWindow::Initialize();
		}

		Assert(g_attractWindow);
		if(g_attractWindow) {
			if(!DiplomacyWindow::IsShown()) {
				g_attractWindow->HighlightControl(k_DIP_WINDOW_ATTRACT_BUTTON);
			} else {
				g_attractWindow->RemoveControl(k_DIP_WINDOW_ATTRACT_BUTTON);
			}
		}
			
		
		ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Negotiations.TabPanel.ProposalsReceivedBox.List");
		

		if(lb) {
			sint32 i;
			for(i = 0; i < lb->NumItems(); i++) {
				ctp2_ListItem *item = (ctp2_ListItem *)lb->GetItemByIndex(i);
				Assert(item);
#if defined(__LP64__)
				if(item && sint64(item->GetUserData()) == p1) {
#else
				if(item && sint32(item->GetUserData()) == p1) {
#endif
					lb->SelectItem(item);
					break;
				}
			}
		}
	}
	return GEV_HD_Continue;
}

STDEHANDLER(DipWinNewNegotiationEvent)
{
	sint32 p1, p2;
	if(!args->GetPlayer(0, p1)) return GEV_HD_Continue;
	if(!args->GetPlayer(1, p2)) return GEV_HD_Continue;

	if(p2 == g_selected_item->GetVisiblePlayer()) {
		
	}
	return GEV_HD_Continue;
}

void DiplomacyWindow::InitializeEvents()
{
	
	
	
	
}

void DiplomacyWindow::CleanupEvents()
{
}

void DiplomacyWindow::Civ(aui_Control *control, uint32 action, uint32 data, void *cookie)
{



}

AUI_ERRCODE DiplomacyWindow::DrawCivColor(ctp2_Static *control, aui_Surface *surface, RECT &rect, void *cookie )
{
#if defined(__LP64__)
	sint32 player = (sint64)cookie;
#else
	sint32 player = (sint32)cookie;
#endif
	Assert(g_colorSet);
	if(!g_colorSet)
		return AUI_ERRCODE_INVALIDPARAM;

	rect.left += 2;
	rect.top += 2;
	rect.right -= 2;
	rect.bottom -= 2;

	return g_ui->TheBlitter()->ColorBlt16(surface, &rect, g_colorSet->GetPlayerColor(player), 0);
}


void DiplomacyWindow::Tone(aui_Control *control, uint32 action, uint32 data, void *cookie)
{








}

void DiplomacyWindow::Request(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
















}

void DiplomacyWindow::Treaty(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
















}

void DiplomacyWindow::Gift(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
















}

void DiplomacyWindow::Exchange(aui_Control *control, uint32 action, uint32 data, void *cookie)
{








}

class DiplomacyWindowChangeModeAction : public aui_Action
{
  public:
	DiplomacyWindowChangeModeAction(DW_CREATE_MODE mode) {
		m_mode = mode;
	}
	virtual void Execute
    (
	aui_Control *   control,
		uint32		action,
		uint32		data
    );

  protected:
	DW_CREATE_MODE m_mode;
};

void DiplomacyWindowChangeModeAction::Execute(aui_Control *control,
											  uint32 action,
											  uint32 data)
{




}

void DiplomacyWindow::CreateList(aui_Control *control, uint32 action, uint32 data, void *cookie)
{








































































































}

void DiplomacyWindow::ClearForm(aui_Control *control, uint32 action, uint32 data, void *cookie)
{












}

void DiplomacyWindow::AddCityItems(ctp2_Menu *menu, sint32 player)
{
	Assert(player >= 0);
	Assert(player < k_MAX_PLAYERS);
	if((player < 0) || (player >= k_MAX_PLAYERS))
		return;
	
	Assert(g_player[player]);
	if(!g_player[player])
		return;

	sint32 i;
	for(i = 0; i < g_player[player]->m_all_cities->Num(); i++) {
		Unit city = g_player[player]->m_all_cities->Access(i);
		menu->AddItem(city.GetName(), NULL, (void *)city.m_id);
	}
}

void DiplomacyWindow::AddAdvanceItems(ctp2_Menu *menu, sint32 sender, sint32 receiver)
{
	Assert(sender >= 0);
	Assert(sender < k_MAX_PLAYERS);
	if((sender < 0) || (sender >= k_MAX_PLAYERS))
		return;

	Assert(receiver >= 0);
	Assert(receiver < k_MAX_PLAYERS);
	if((receiver < 0) || (receiver >= k_MAX_PLAYERS))
		return;

	Assert(g_player[sender]);
	if(!g_player[sender])
		return;

	Assert(g_player[receiver]);
	if(!g_player[receiver])
		return;

	sint32 a;
	for(a = 0; a < g_theAdvanceDB->NumRecords(); a++) {
		if(!g_player[sender]->HasAdvance(a)) {
			
			continue;
		}

		if(g_player[receiver]->HasAdvance(a)) {
			
			continue;
		}

		menu->AddItem(g_theAdvanceDB->Get(a)->GetNameText(), NULL, (void *)a);
	}
}

void DiplomacyWindow::AddThirdPartyItems(ctp2_Menu *menu, sint32 sender, sint32 receiver)
{
	sint32 p;
	for(p = 1; p < k_MAX_PLAYERS; p++) {
		if((p == sender) || (p == receiver))
			continue;

		if(!g_player[p])
			continue;

		MBCHAR civName[k_MAX_NAME_LEN];
		g_player[p]->GetCivilisation()->GetPluralCivName(civName);
		menu->AddItem(civName, NULL, (void *)p);
	}
}

void DiplomacyWindow::RequestGoldValue(sint32 player)
{
	if(!m_goldRequestWindow) {
		m_goldRequestWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot("DipGoldRequest");
		Assert(m_goldRequestWindow);
		if(!m_goldRequestWindow)
			return;
		ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipGoldRequest.Spinner");
		Assert(spinner);
		if(spinner) {
			spinner->SetSpinnerCallback(DiplomacyWindow::GoldSpinner, NULL);
		}
		aui_Ldl::SetActionFuncAndCookie("DipGoldRequest.OkButton", DiplomacyWindow::GoldOk, NULL);
		aui_Ldl::SetActionFuncAndCookie("DipGoldRequest.CancelButton", DiplomacyWindow::GoldCancel, NULL);

		m_goldRequestWindow->SetStronglyModal(TRUE);
	}

	ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipGoldRequest.Spinner");
	if(player == g_selected_item->GetVisiblePlayer()) {
		spinner->SetMaximum(g_player[player]->m_gold->GetLevel(), 0);
	} else {
		
		spinner->SetMaximum(~(1 << 31), 0);
	}

	g_c3ui->AddWindow(m_goldRequestWindow);
	
}

void DiplomacyWindow::RequestPollutionValue(sint32 player)
{
	if(!m_pollutionRequestWindow) {
		m_pollutionRequestWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot("DipPercentRequest");
		Assert(m_pollutionRequestWindow);
		if(!m_pollutionRequestWindow)
			return;
		ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipPollutionRequest.Spinner");
		Assert(spinner);
		if(spinner) {
			spinner->SetSpinnerCallback(DiplomacyWindow::GoldSpinner, NULL);
		}
		aui_Ldl::SetActionFuncAndCookie("DipPollutionRequest.OkButton", DiplomacyWindow::PollutionOk, NULL);
		aui_Ldl::SetActionFuncAndCookie("DipPollutionRequest.CancelButton", DiplomacyWindow::PollutionCancel, NULL);

		m_pollutionRequestWindow->SetStronglyModal(TRUE);
	}

	ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipPollutionRequest.Spinner");
	
	spinner->SetMaximum(static_cast<sint32>(g_player[player]->GetPollutionLevel() * 0.95), 0);
	
	spinner->SetMinimum(static_cast<sint32>(g_player[player]->GetPollutionLevel() * 0.25), 0);
	
	spinner->SetPage(static_cast<sint32>(g_player[player]->GetPollutionLevel() * 0.20), 0);
	
	spinner->SetIncrement(static_cast<sint32>(g_player[player]->GetPollutionLevel() * 0.10), 0);

	g_c3ui->AddWindow(m_pollutionRequestWindow);
}

void DiplomacyWindow::RequestPercentValue()
{
	if(!m_percentRequestWindow) {
		m_percentRequestWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot("DipPercentRequest");
		Assert(m_percentRequestWindow);
		if(!m_percentRequestWindow)
			return;
		ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipPercentRequest.Spinner");
		Assert(spinner);
		if(spinner) {
			spinner->SetSpinnerCallback(DiplomacyWindow::GoldSpinner, NULL);
		}
		aui_Ldl::SetActionFuncAndCookie("DipPercentRequest.OkButton", DiplomacyWindow::PercentOk, NULL);
		aui_Ldl::SetActionFuncAndCookie("DipPercentRequest.CancelButton", DiplomacyWindow::PercentCancel, NULL);

		m_percentRequestWindow->SetStronglyModal(TRUE);
	}

	ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipPercentRequest.Spinner");
	
	spinner->SetMaximum(75, 0);
	
	spinner->SetMinimum(5,0);
	
	spinner->SetPage(20, 0);
	
	spinner->SetIncrement(10, 0);
	
	g_c3ui->AddWindow(m_percentRequestWindow);
}


bool DiplomacyWindow::ProposalContextMenu(sint32 proposal)
{
	Assert(!m_curMenu);
	if(m_curMenu) {
		
		delete m_curMenu;
	}
	
	const DiplomacyProposalRecord *rec = g_theDiplomacyProposalDB->Get(proposal);
	Assert(rec);
	if(!rec)
		return false;

	bool needItems = true;
	m_curMenu = new ctp2_Menu(true, DiplomacyWindow::MenuCallback);
	switch(rec->GetArg1()) {
		case k_DiplomacyProposal_Arg1_OwnCity_Bit:
			AddCityItems(m_curMenu, g_selected_item->GetVisiblePlayer());
			break;
		case k_DiplomacyProposal_Arg1_HisCity_Bit:
			AddCityItems(m_curMenu, m_sendToCiv);
			break;
		case k_DiplomacyProposal_Arg1_OwnArmy_Bit:
			
			break;
		case k_DiplomacyProposal_Arg1_HisArmy_Bit:
			
			break;
		case k_DiplomacyProposal_Arg1_HisAgreement_Bit:
			
			break;
		case k_DiplomacyProposal_Arg1_OwnAdvance_Bit:
			AddAdvanceItems(m_curMenu, g_selected_item->GetVisiblePlayer(), m_sendToCiv);
			break;
		case k_DiplomacyProposal_Arg1_HisAdvance_Bit:
			AddAdvanceItems(m_curMenu, m_sendToCiv, g_selected_item->GetVisiblePlayer());
			break;
		
		
		
		
		
		
		case k_DiplomacyProposal_Arg1_OwnUnitType_Bit:
			
			break;
		case k_DiplomacyProposal_Arg1_OwnGood_Bit:
			
			break;
		case k_DiplomacyProposal_Arg1_HisGood_Bit:
			
			break;
		case k_DiplomacyProposal_Arg1_OwnGold_Bit:
			RequestGoldValue(g_selected_item->GetVisiblePlayer());
			if(m_getRequest)
				m_proposalArg.gold = 0;
			else
				m_exchangeArg.gold = 0;
			needItems = false;
			break;
		case k_DiplomacyProposal_Arg1_HisGold_Bit:
			RequestGoldValue(m_sendToCiv);
			if(m_getRequest)
				m_proposalArg.gold = 0;
			else
				m_exchangeArg.gold = 0;
			needItems = false;
			break;
		case k_DiplomacyProposal_Arg1_OwnPollution_Bit:
			RequestPollutionValue(g_selected_item->GetVisiblePlayer());
			if(m_getRequest)
				m_proposalArg.gold = 0;
			else
				m_exchangeArg.gold = 0;
			needItems = false;
			break;
		case k_DiplomacyProposal_Arg1_HisPollution_Bit:

			RequestPollutionValue(m_sendToCiv);
			if(m_getRequest)
				m_proposalArg.gold = 0;
			else
				m_exchangeArg.gold = 0;
			needItems = false;
			break;
		case k_DiplomacyProposal_Arg1_Percent_Bit:
			RequestPercentValue();
			if(m_getRequest)
				m_proposalArg.gold = 0;
			else
				m_exchangeArg.gold = 0;
			needItems = false;
			break;
		case k_DiplomacyProposal_Arg1_ThirdParty_Bit:
			AddThirdPartyItems(m_curMenu, g_selected_item->GetVisiblePlayer(), m_sendToCiv);
			break;
		default:
			
			Assert(rec->GetArg1() == 0);
			needItems = false;
			break;
	}

	if(m_curMenu->GetNumItems() < 1) {
		delete m_curMenu;
		m_curMenu = NULL;
		if(needItems) {
			
			return false;
		}
		UpdateSendProposalDetails();
	} else {
		m_curMenu->Open();
	}
	return true;
}						

void DiplomacyWindow::ProcessMenuSelection(sint32 itemIndex, void *cookie)
{
	sint32 prop = m_getRequest ? m_sendProposal : m_sendExchange;

	Assert((prop >= 0) && (prop < g_theDiplomacyProposalDB->NumRecords()));
	if((prop >= 0) && (prop < g_theDiplomacyProposalDB->NumRecords())) {
		const DiplomacyProposalRecord *rec = g_theDiplomacyProposalDB->Get(prop);
		DiplomacyArg & arg = m_getRequest ? m_proposalArg : m_exchangeArg;

		switch(rec->GetArg1()) {
			case k_DiplomacyProposal_Arg1_OwnCity_Bit:
			case k_DiplomacyProposal_Arg1_HisCity_Bit:
#if defined(__LP64__)
				arg.cityId = (sint64)cookie;
#else
				arg.cityId = (sint32)cookie;
#endif
				break;
			case k_DiplomacyProposal_Arg1_OwnArmy_Bit:
				break;
			case k_DiplomacyProposal_Arg1_HisArmy_Bit:
				break;
			case k_DiplomacyProposal_Arg1_HisAgreement_Bit:
				break;
			case k_DiplomacyProposal_Arg1_OwnAdvance_Bit:
			case k_DiplomacyProposal_Arg1_HisAdvance_Bit:
			case k_DiplomacyProposal_Arg1_OwnStopResearch_Bit:
			case k_DiplomacyProposal_Arg1_HisStopResearch_Bit:
#if defined(__LP64__)
				arg.advanceType = (sint64)cookie;
#else
				arg.advanceType = (sint32)cookie;
#endif
				break;
			case k_DiplomacyProposal_Arg1_OwnUnitType_Bit:
				break;
			case k_DiplomacyProposal_Arg1_OwnGood_Bit:
				break;
			case k_DiplomacyProposal_Arg1_HisGood_Bit:
				break;
			case k_DiplomacyProposal_Arg1_OwnGold_Bit:
			case k_DiplomacyProposal_Arg1_HisGold_Bit:
#if defined(__LP64__)
				arg.gold = (sint64)cookie;
				break;
			case k_DiplomacyProposal_Arg1_ThirdParty_Bit:
				arg.playerId = (sint64)cookie;
				break;
			case k_DiplomacyProposal_Arg1_Percent_Bit:
				arg.percent = (double) ((sint64) cookie) / 100.0;
#else
				arg.gold = (sint32)cookie;
				break;
			case k_DiplomacyProposal_Arg1_ThirdParty_Bit:
				arg.playerId = (sint32)cookie;
				break;
			case k_DiplomacyProposal_Arg1_Percent_Bit:
				arg.percent = (double) ((sint32) cookie) / 100.0;
#endif
				break;
			default:
				
				Assert(FALSE);
				break;
		}
	}
	UpdateSendProposalDetails();
}

void DiplomacyWindow::ProcessMenuCancel()
{






}

void DiplomacyWindow::MenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	Assert(s_dipWindow);
	if(!s_dipWindow)
		return;

	Assert(menu == s_dipWindow->m_curMenu);
	if(menu != s_dipWindow->m_curMenu)
		return;

	if(action == CTP2_MENU_ACTION_SELECT) {
		s_dipWindow->ProcessMenuSelection(itemIndex, cookie);
	} else if(action == CTP2_MENU_ACTION_CANCEL) {
		s_dipWindow->ProcessMenuCancel();
	}

	delete s_dipWindow->m_curMenu;
	s_dipWindow->m_curMenu = NULL;
}

void DiplomacyWindow::GoldSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	
}

void DiplomacyWindow::GoldOk(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(!s_dipWindow) return;

	ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipGoldRequest.Spinner");

	Assert(s_dipWindow->m_goldRequestWindow);
	if(s_dipWindow->m_goldRequestWindow) {
		g_c3ui->RemoveWindow(s_dipWindow->m_goldRequestWindow->Id());
	}

	if(s_dipWindow->m_getRequest) {
		s_dipWindow->m_proposalArg.gold = spinner->GetValueX();
	} else {
		s_dipWindow->m_exchangeArg.gold = spinner->GetValueX();
	}

	s_dipWindow->UpdateSendProposalDetails();
}

void DiplomacyWindow::GoldCancel(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(!s_dipWindow) return;

	Assert(s_dipWindow->m_goldRequestWindow);
	if(s_dipWindow->m_goldRequestWindow) {
		g_c3ui->RemoveWindow(s_dipWindow->m_goldRequestWindow->Id());
	}

	
	
	s_dipWindow->ProcessMenuCancel();
}

void DiplomacyWindow::PollutionOk(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(!s_dipWindow) return;

	ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipPollutionRequest.Spinner");

	Assert(s_dipWindow->m_pollutionRequestWindow);
	if(s_dipWindow->m_pollutionRequestWindow) {
		g_c3ui->RemoveWindow(s_dipWindow->m_pollutionRequestWindow->Id());
	}

	if(s_dipWindow->m_getRequest) {
		s_dipWindow->m_proposalArg.pollution = spinner->GetValueX();
	} else {
		s_dipWindow->m_exchangeArg.pollution = spinner->GetValueX();
	}

	s_dipWindow->UpdateSendProposalDetails();
}

void DiplomacyWindow::PollutionCancel(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(!s_dipWindow) return;

	Assert(s_dipWindow->m_pollutionRequestWindow);
	if(s_dipWindow->m_pollutionRequestWindow) {
		g_c3ui->RemoveWindow(s_dipWindow->m_pollutionRequestWindow->Id());
	}

	
	
	s_dipWindow->ProcessMenuCancel();
}


void DiplomacyWindow::PercentOk(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(!s_dipWindow) return;

	ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipPercentRequest.Spinner");

	Assert(s_dipWindow->m_percentRequestWindow);
	if(s_dipWindow->m_percentRequestWindow) {
		g_c3ui->RemoveWindow(s_dipWindow->m_percentRequestWindow->Id());
	}

	if(s_dipWindow->m_getRequest) {
		s_dipWindow->m_proposalArg.percent = (double) spinner->GetValueX() / 100.0;
	} else {
		s_dipWindow->m_exchangeArg.percent = (double) spinner->GetValueX() / 100.0;
	}

	s_dipWindow->UpdateSendProposalDetails();
}

void DiplomacyWindow::PercentCancel(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(!s_dipWindow) return;

	Assert(s_dipWindow->m_percentRequestWindow);
	if(s_dipWindow->m_percentRequestWindow) {
		g_c3ui->RemoveWindow(s_dipWindow->m_percentRequestWindow->Id());
	}

	
	
	s_dipWindow->ProcessMenuCancel();
}


void DiplomacyWindow::ProposalsMade(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if((action != AUI_LISTBOX_ACTION_SELECT) &&
	   (action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT))
		return;

	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Negotiations.TabPanel.ProposalsMadeBox.List");
	Assert(lb);
	if(!lb)
		return;
	
	ctp2_ListItem *item = (ctp2_ListItem *)lb->GetSelectedItem();

	if(!item) {
		s_dipWindow->ShowSections(k_DIPWIN_PROPOSALS_RECEIVED | k_DIPWIN_PROPOSALS_MADE | k_DIPWIN_PROPOSAL_DETAILS);
	} else {

#if defined(__LP64__)
		sint32 otherPlayer = (sint64)item->GetUserData();
#else
		sint32 otherPlayer = (sint32)item->GetUserData();
#endif
		s_dipWindow->SetViewingResponse(g_selected_item->GetVisiblePlayer(),
										otherPlayer);
		RESPONSE_TYPE rtype = Diplomat::GetDiplomat(otherPlayer).GetResponsePending(g_selected_item->GetVisiblePlayer()).type;
		if((rtype == RESPONSE_COUNTER) || (rtype == RESPONSE_REJECT)) {
			s_dipWindow->ShowSections(k_DIPWIN_RESPONSE | k_DIPWIN_RESPONSE_WITH_COUNTER | k_DIPWIN_PROPOSALS_RECEIVED | k_DIPWIN_PROPOSALS_MADE);
		} else {
			s_dipWindow->ShowSections(k_DIPWIN_RESPONSE | k_DIPWIN_PROPOSALS_MADE | k_DIPWIN_PROPOSALS_RECEIVED);
		}
	}
}

void DiplomacyWindow::ResponseOK(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(s_dipWindow) {
		ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Negotiations.TabPanel.ProposalsMadeBox.List");
		Assert(lb);
		if(lb && lb->GetSelectedItem()) {
			lb->DeselectItem(lb->GetSelectedItem());
		} else {
			s_dipWindow->ShowSections(k_DIPWIN_PROPOSALS_RECEIVED | k_DIPWIN_PROPOSALS_MADE | k_DIPWIN_PROPOSAL_DETAILS);
		}
	}
}

void DiplomacyWindow::ThreatenCounter(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(s_dipWindow) {
		Response response = Diplomat::GetDiplomat(s_dipWindow->m_viewResponseReceiver)
			                .GetResponsePending(s_dipWindow->m_viewResponseSender);

		
		Assert(response != Diplomat::s_badResponse);
		if(response == Diplomat::s_badResponse) {
			s_dipWindow->ShowSections(k_DIPWIN_PROPOSALS_RECEIVED | k_DIPWIN_PROPOSALS_MADE | k_DIPWIN_PROPOSAL_DETAILS);
			return;
		}

		s_dipWindow->ShowSections(k_DIPWIN_RESPONSE | 
								  k_DIPWIN_RESPONSE_WITH_COUNTER | 
								  k_DIPWIN_THREAT_LIST);
	}
}

void DiplomacyWindow::AcceptCounter(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(s_dipWindow) {
		Response response;
		response.senderId = s_dipWindow->m_viewResponseSender;
		response.receiverId = s_dipWindow->m_viewResponseReceiver;

		RESPONSE_TYPE typeRespondingTo = Diplomat::GetDiplomat(s_dipWindow->m_viewResponseReceiver).GetResponsePending(s_dipWindow->m_viewResponseSender).type;
		if(typeRespondingTo == RESPONSE_COUNTER) {
			response.type = RESPONSE_ACCEPT;
		} else {
			
			
			response.type = RESPONSE_REJECT;
		}

		Diplomat::GetDiplomat(s_dipWindow->m_viewResponseSender).ExecuteResponse(response);

		ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Negotiations.TabPanel.ProposalsMadeBox.List");
		Assert(lb);
		if(lb && lb->GetSelectedItem()) {
			lb->DeselectItem(lb->GetSelectedItem());
		}
		s_dipWindow->ShowSections(k_DIPWIN_PROPOSALS_RECEIVED | k_DIPWIN_PROPOSALS_MADE | k_DIPWIN_PROPOSAL_DETAILS);
	}
}

void DiplomacyWindow::RejectCounter(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(s_dipWindow) {
		Response response;
		response.senderId = s_dipWindow->m_viewResponseSender;
		response.receiverId = s_dipWindow->m_viewResponseReceiver;

		RESPONSE_TYPE typeRespondingTo = Diplomat::GetDiplomat(s_dipWindow->m_viewResponseReceiver).GetResponsePending(s_dipWindow->m_viewResponseSender).type;

		
		response.type = RESPONSE_REJECT;

		Diplomat::GetDiplomat(s_dipWindow->m_viewResponseSender).ExecuteResponse(response);

		ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Negotiations.TabPanel.ProposalsMadeBox.List");
		Assert(lb);
		if(lb && lb->GetSelectedItem()) {
			lb->DeselectItem(lb->GetSelectedItem());
		}
		s_dipWindow->ShowSections(k_DIPWIN_PROPOSALS_RECEIVED | k_DIPWIN_PROPOSALS_MADE | k_DIPWIN_PROPOSAL_DETAILS);
	}
}

bool DiplomacyWindow::ThreatContextMenu(sint32 threat)
{
	Assert(!m_threatMenu);
	if(m_threatMenu) {
		
		delete m_threatMenu;
	}

	const DiplomacyThreatRecord *rec = g_theDiplomacyThreatDB->Get(threat);
	Assert(rec);
	if(!rec)
		return false;

	bool needItems = true;
	m_threatMenu = new ctp2_Menu(true, DiplomacyWindow::ThreatMenuCallback);
	switch(rec->GetArg1()) {
		case k_DiplomacyThreat_Arg1_HisCity_Bit:
		case k_DiplomacyThreat_Arg1_SpecialAttack_Bit:
			AddCityItems(m_threatMenu, m_viewResponseReceiver);
			break;
		case k_DiplomacyThreat_Arg1_ThirdParty_Bit:
			AddThirdPartyItems(m_threatMenu, g_selected_item->GetVisiblePlayer(), m_viewResponseReceiver);
			break;
		default:
			needItems = false;
			break;
	}

	if(m_threatMenu->GetNumItems() < 1) {
		delete m_threatMenu;
		m_threatMenu = NULL;

		if(needItems) {
			return false;
		}
	} else {
		m_threatMenu->Open();
	}
	return true;
}

void DiplomacyWindow::ThreatList(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if((action != AUI_LISTBOX_ACTION_SELECT) &&
	   (action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT))
		return;

	Assert(s_dipWindow);
	if(s_dipWindow) {
		static bool inCallback = false;
		if(inCallback)
			return;
		inCallback = true;

		ctp2_ListBox *lb = (ctp2_ListBox *)control;
		ctp2_ListItem *selItem = (ctp2_ListItem *)lb->GetSelectedItem();
		
		if(selItem) {
#if defined(__LP64__)
			if(!s_dipWindow->ThreatContextMenu((sint64)selItem->GetUserData())) {
				s_dipWindow->m_sendThreat = THREAT_NONE;
				lb->DeselectItem(selItem);
			} else {
				s_dipWindow->m_sendThreat = (sint64)selItem->GetUserData();
#else
			if(!s_dipWindow->ThreatContextMenu((sint32)selItem->GetUserData())) {
				s_dipWindow->m_sendThreat = THREAT_NONE;
				lb->DeselectItem(selItem);
			} else {
				s_dipWindow->m_sendThreat = (sint32)selItem->GetUserData();
#endif
			}
		}
		s_dipWindow->Update();

		inCallback = false;
	}
}

void DiplomacyWindow::ThreatMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	if((action != CTP2_MENU_ACTION_CANCEL) &&
	   (action != CTP2_MENU_ACTION_SELECT))
		return;

	Assert(s_dipWindow);
	if(!s_dipWindow) return;

	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.Negotiations.TabPanel.Threaten.List");
	Assert(lb);
	if(!lb)
		return;

	Assert(s_dipWindow->m_threatMenu && (s_dipWindow->m_threatMenu == menu));
	if(s_dipWindow->m_threatMenu) {
		if(action == CTP2_MENU_ACTION_CANCEL) {
			s_dipWindow->m_sendThreat = sint32(THREAT_NONE);
			lb->DeselectItem(lb->GetSelectedItem());
		} else {
			ctp2_ListItem *item = (ctp2_ListItem *)lb->GetSelectedItem();
			Assert(item); 
			if(item) {
#if defined(__LP64__)
				s_dipWindow->m_sendThreat = (sint64)item->GetUserData();
#else
				s_dipWindow->m_sendThreat = (sint32)item->GetUserData();
#endif
				const DiplomacyThreatRecord *rec = g_theDiplomacyThreatDB->Get(s_dipWindow->m_sendThreat);
				Assert(rec);
				if(rec) {
					switch(rec->GetArg1()) {
						case k_DiplomacyThreat_Arg1_HisCity_Bit:
						case k_DiplomacyThreat_Arg1_SpecialAttack_Bit:
#if defined(__LP64__)
							s_dipWindow->m_threatArg.cityId = (sint64)cookie;
							break;
						case k_DiplomacyThreat_Arg1_ThirdParty_Bit:
							s_dipWindow->m_threatArg.playerId = (sint64)cookie;
#else
							s_dipWindow->m_threatArg.cityId = (sint32)cookie;
							break;
						case k_DiplomacyThreat_Arg1_ThirdParty_Bit:
							s_dipWindow->m_threatArg.playerId = (sint32)cookie;
#endif
							break;
						default:
							
							Assert(FALSE);
							break;
					}
				}
			} else {
				s_dipWindow->m_sendThreat = sint32(THREAT_NONE);
			}
		}


		delete s_dipWindow->m_threatMenu;
		s_dipWindow->m_threatMenu = NULL;
		s_dipWindow->Update();
	}
}

void DiplomacyWindow::MakeThreat(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(s_dipWindow) {
		if(s_dipWindow->m_sendThreat > 0 && s_dipWindow->m_sendThreat < g_theDiplomacyThreatDB->NumRecords()) {
			s_dipWindow->SendThreat();
			s_dipWindow->ShowSections(k_DIPWIN_PROPOSALS_MADE | k_DIPWIN_PROPOSALS_RECEIVED | k_DIPWIN_PROPOSAL_DETAILS);
		}
	}
}

void DiplomacyWindow::CancelThreat(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_dipWindow);
	if(s_dipWindow) {
		s_dipWindow->ShowSections(k_DIPWIN_PROPOSALS_MADE | k_DIPWIN_PROPOSALS_RECEIVED | k_DIPWIN_PROPOSAL_DETAILS);
	}
}
void DiplomacyWindow::SendThreat()
{
	Response response;
	response.senderId = m_viewResponseSender;
	response.receiverId = m_viewResponseReceiver;
	response.type = RESPONSE_THREATEN;
	response.threat.type = diplomacyutil_GetThreatType(m_sendThreat);
	response.threat.arg = m_threatArg;

	
	
	Diplomat::GetDiplomat(m_viewResponseSender).ExecuteResponse(response);
}

void DiplomacyWindow::Intelligence(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Hide();
	IntelligenceWindow::Display();
}


void DiplomacyWindow::TabPanelActionCallback(aui_Control *control, uint32 action,
											 uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(ctp2_Tab::ACTION_ACTIVATED))
		return;

	
#if defined(__LP64__)
	sint32 section = (sint64)cookie;
#else
	sint32 section = (sint32)cookie;
#endif

	if(section != k_DIPWIN_CREATE_PROPOSAL) {
		
		
		s_dipWindow->m_sendCounter = false;
	}

	if(section == k_DIPWIN_PROPOSALS_RECEIVED) {
		
		
		if(s_dipWindow) {
			s_dipWindow->ShowSections(k_DIPWIN_PROPOSALS_RECEIVED | k_DIPWIN_PROPOSALS_MADE);
		}
	} else if(section == k_DIPWIN_CREATE_PROPOSAL) {
		

		ctp2_Button *sendButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWindowBlock,
																	"DiplomacyTabs.CreateProposal.TabPanel.NewProposalBox.SendProposalButton");
		Assert(sendButton);

		ctp2_Static *panelTitle = (ctp2_Static *)aui_Ldl::GetObject(s_dipWindowBlock,
																	"DiplomacyTabs.CreateProposal.TabPanel.NewProposalBox.CreateProposalLabel");
		Assert(panelTitle);

		if(!s_dipWindow->m_sendCounter) {
			s_dipWindow->m_create_mode = DW_CREATE_MODE_CIV;
			s_dipWindow->EnableRequests(true);
			if(sendButton) {
				sendButton->SetText(g_theStringDB->GetNameStr("str_ldl_DipSendProposal"));
			}
			if(panelTitle) {
				panelTitle->SetText(g_theStringDB->GetNameStr("str_ldl_DipTitleCreateNewProposal"));
			}
		} else {
			s_dipWindow->m_create_mode = DW_CREATE_MODE_TONE;
			s_dipWindow->EnableRequests(false);
			if(sendButton) {
				sendButton->SetText(g_theStringDB->GetNameStr("str_ldl_DipSendCounterProposal"));
			}
			if(panelTitle) {
				panelTitle->SetText(g_theStringDB->GetNameStr("str_ldl_DipTitleCreateCounterProposal"));
			}
		}
		s_dipWindow->Update();
	}
}

void DiplomacyWindow::SetSendProposal(sint32 prop)
{
	m_sendProposal = prop;
	ctp2_Button *exchRequest = (ctp2_Button *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.CreateProposal.TabPanel.NewProposalBox.ExchangeRequestButton");
	ctp2_Button *exchTreaty = (ctp2_Button *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.CreateProposal.TabPanel.NewProposalBox.ExchangeTreatyButton");
	ctp2_Button *exchGift = (ctp2_Button *)aui_Ldl::GetObject(s_dipWindowBlock, "DiplomacyTabs.CreateProposal.TabPanel.NewProposalBox.ExchangeGiftButton");

	if(m_sendProposal < 0) {
		if(exchRequest) exchRequest->Enable(FALSE);
		if(exchTreaty) exchTreaty->Enable(FALSE);
		if(exchGift) exchGift->Enable(FALSE);
	} else {
		sint32 propIndex = prop;
		const DiplomacyProposalRecord *rec = g_theDiplomacyProposalDB->Get(propIndex);
		if(m_sendCounter) {
			if(rec->GetReciprocalIndex(propIndex)) {
				rec = g_theDiplomacyProposalDB->Get(propIndex);
				Assert(rec);
			}
		}

		sint32 excludes = 0;
		if(rec) {
			excludes = rec->GetExcludes();
		}
		if(exchRequest) {
			if(!(excludes & k_DiplomacyProposal_Excludes_Request_Bit)) exchRequest->Enable(TRUE);
			else exchRequest->Enable(FALSE);
		}

		if(exchTreaty) {
			if(!(excludes & k_DiplomacyProposal_Excludes_Treaty_Bit)) exchTreaty->Enable(TRUE);
			else exchTreaty->Enable(FALSE);
		}

		if(exchGift) {
			if(!(excludes & k_DiplomacyProposal_Excludes_Offer_Bit)) exchGift->Enable(TRUE);
			else exchGift->Enable(FALSE);
		}
	}
}

void DiplomacyWindow::EnableRequests(bool enable)
{































}

void DiplomacyWindow::DeclareWar(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	IntelligenceWindow::DeclareWarOnSelected();
}

void DiplomacyWindow::DeclareEmbargo(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	IntelligenceWindow::DeclareEmbargoOnSelected();
}


void DiplomacyWindow::SendMessage(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	IntelligenceWindow::SendMessageToSelected();
}

void DiplomacyWindow::Details(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	IntelligenceWindow::DisplayDetailsOfSelected();
}

void DiplomacyWindow::SetCreateMode(DW_CREATE_MODE mode)
{
	m_create_mode = mode;
	UpdateCreateList();
	UpdateSendProposalDetails();
}

void DiplomacyWindow::EnableButtons(BOOL enable, sint32 player)
{
	sm_detailsButton->Enable(enable);
	sm_messageButton->Enable(enable);
	
	if ( enable && AgreementMatrix::s_agreements.HasAgreement(g_selected_item->GetVisiblePlayer(),
			player, PROPOSAL_TREATY_DECLARE_WAR) )
	{
		sm_warButton->Enable(FALSE);
	} else {
		sm_warButton->Enable(enable);
	}

	if ( enable && Diplomat::GetDiplomat(g_selected_item->GetVisiblePlayer()).GetEmbargo(player) )
	{
		sm_embargoButton->Enable(FALSE);
	} else {
		sm_embargoButton->Enable(enable);
	}

}
