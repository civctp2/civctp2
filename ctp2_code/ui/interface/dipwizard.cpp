//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling diplomacy proposals between Human and other players.
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
// - Added emissary photo to the diplomatic manager by Martin Gühmann
// - Diplomatic proposals/responses sent from UI get the highest priority possible
//   so that the AI won't override them: DipWizard::SendCallback
// - Repaired crashes when the emissary photo is missing.
// - Added female leader images. (Aug 20th 2005 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - removed new diplo attempt - E 12.27.2006
// - Added HotSeat and PBEM human-human diplomacy support. (17-Oct-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "dipwizard.h"

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

#include "DiplomacyDetails.h"
#include "AgreementMatrix.h"
#include "Gold.h"

#include "network.h"
#include "UnitData.h"

#include "primitives.h"
#include "colorset.h"                   // g_colorSet

extern C3UI       *g_c3ui;

static DipWizard  *s_dipWizard;
static MBCHAR     *s_dipWizardBlock = "DipWizard";

ctp2_Static       *DipWizard::m_stages[DIP_WIZ_STAGE_MAX];
ctp2_Button       *DipWizard::m_toneButtons[DIPLOMATIC_TONE_MAX];
ctp2_Button       *DipWizard::m_backButton = NULL,
                  *DipWizard::m_nextButton = NULL,
                  *DipWizard::m_sendButton = NULL,
                  *DipWizard::m_cancelButton = NULL;
ctp2_Button       *DipWizard::m_counterOrThreatenButton = NULL;
ctp2_Button       *DipWizard::m_rejectButton = NULL,
                  *DipWizard::m_acceptButton = NULL;
ctp2_Button       *DipWizard::m_intelButton = NULL;

ctp2_Static       *DipWizard::m_createButtons = NULL,
                  *DipWizard::m_viewButtons = NULL,

                  *DipWizard::m_parchment = NULL,
                  *DipWizard::m_responseDiplomat;

//Added by Martin Gühmann to display the emissary photo of recipient
ctp2_Static       *DipWizard::m_emissary_photo = NULL;

ctp2_DropDown     *DipWizard::m_nations = NULL;

ctp2_ListBox      *DipWizard::m_propList[DIP_WIZ_PROP_TAB_MAX];
ctp2_ListBox      *DipWizard::m_exchList[DIP_WIZ_PROP_TAB_MAX];
ctp2_ListBox      *DipWizard::m_threatList;
ctp2_Menu         *DipWizard::m_curMenu = NULL,
                  *DipWizard::m_threatMenu = NULL;
ctp2_Window       *DipWizard::m_goldRequestWindow = NULL;
ctp2_Window       *DipWizard::m_pollutionRequestWindow = NULL;
ctp2_Window       *DipWizard::m_percentRequestWindow = NULL;
bool              DipWizard::m_proposalDataPending = false;
bool              DipWizard::m_threatDataPending = false;

sint32            DipWizard::m_recipient = -1;
sint32            DipWizard::m_tone = -1;
sint32            DipWizard::m_proposal = -1;
sint32            DipWizard::m_exchange = -1;
sint32            DipWizard::m_menuProposal = -1;
sint32            DipWizard::m_menuExchange = -1;
sint32            DipWizard::m_threat = -1;
sint32            DipWizard::m_menuThreat = -1;
DiplomacyArg      DipWizard::m_proposalArg,
                  DipWizard::m_exchangeArg,
                  DipWizard::m_threatArg;

sint32            DipWizard::m_viewTone = -1,
                  DipWizard::m_viewRecipient = -1,
                  DipWizard::m_viewSender = -1,
                  DipWizard::m_viewProposal = - 1,
                  DipWizard::m_viewExchange = -1,
                  DipWizard::m_viewThreat = -1,
                  DipWizard::m_viewResponseType = -1;
DiplomacyArg      DipWizard::m_viewProposalArg,
                  DipWizard::m_viewExchangeArg;
DiplomacyArg      DipWizard::m_viewThreatArg;
DIP_WIZ_VIEW_TYPE DipWizard::m_viewType;
bool              DipWizard::m_sendCounter;

DipWizard::DipWizard(AUI_ERRCODE *err)
{
	*err = AUI_ERRCODE_OK;
	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_dipWizardBlock);
	Assert(m_window);
	if(!m_window) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	sint32 i;
	for(i = 0; i < DIP_WIZ_STAGE_MAX; i++) {
		MBCHAR stageName[k_MAX_NAME_LEN];
		sprintf(stageName, "%s.Stage%d", s_dipWizardBlock, i);
		m_stages[i] = (ctp2_Static *)aui_Ldl::GetObject(stageName);
		Assert(m_stages[i]);
	}

	m_nations = (ctp2_DropDown *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage0.Nations");
	if(m_nations) m_nations->SetActionFuncAndCookie(NationCallback, NULL);

	for(i = 0; i < DIPLOMATIC_TONE_MAX; i++) {
		MBCHAR buttName[k_MAX_NAME_LEN];
		sprintf(buttName, "%s.Stage0.Tones.Tone%d", s_dipWizardBlock, i);
		m_toneButtons[i] = (ctp2_Button *)aui_Ldl::GetObject(buttName);
		Assert(m_toneButtons[i]);

		if(m_toneButtons[i]) {
			m_toneButtons[i]->SetActionFuncAndCookie(ToneButtonCallback, (void *)i);
		}

		MBCHAR labelName[k_MAX_NAME_LEN];
		sprintf(labelName, "%s.Stage0.Tones.ToneLabel%d", s_dipWizardBlock, i);
		ctp2_Static *label = (ctp2_Static *)aui_Ldl::GetObject(labelName);
		Assert(label);

		MBCHAR toneId[k_MAX_NAME_LEN];
		sprintf(toneId, "str_ldl_DipTone%d", i);
		label->SetText(g_theStringDB->GetNameStr(toneId));

	}

	m_backButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWizardBlock, "CreateButtons.BackButton");
	if(m_backButton) m_backButton->SetActionFuncAndCookie(BackCallback, NULL);

	m_nextButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWizardBlock, "CreateButtons.NextButton");
	if(m_nextButton) m_nextButton->SetActionFuncAndCookie(NextCallback, NULL);

	m_cancelButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWizardBlock, "CreateButtons.CancelButton");
	if(m_cancelButton) m_cancelButton->SetActionFuncAndCookie(CancelCallback, NULL);

	m_sendButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWizardBlock, "CreateButtons.SendButton");
	if(m_sendButton) m_sendButton->SetActionFuncAndCookie(SendCallback, NULL);

	m_acceptButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWizardBlock, "ViewButtons.AcceptButton");
	aui_Ldl::SetActionFuncAndCookie(s_dipWizardBlock, "ViewButtons.AcceptButton", AcceptCallback, NULL);

	m_rejectButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWizardBlock, "ViewButtons.RejectButton");
	aui_Ldl::SetActionFuncAndCookie(s_dipWizardBlock, "ViewButtons.RejectButton", RejectCallback, NULL);

	m_counterOrThreatenButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWizardBlock, "ViewButtons.CounterOrThreatenButton");
	if(m_counterOrThreatenButton) m_counterOrThreatenButton->SetActionFuncAndCookie(CounterOrThreatenCallback, NULL);

	m_createButtons = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "CreateButtons");
	m_viewButtons = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "ViewButtons");

	m_intelButton = (ctp2_Button *)aui_Ldl::GetObject(s_dipWizardBlock, "CheckIntelligenceButton");
	aui_Ldl::SetActionFuncAndCookie(s_dipWizardBlock, "CheckIntelligenceButton", CheckIntelligence, NULL);

	m_propList[DIP_WIZ_PROP_TAB_REQUEST] = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage1.Tabs.Request.TabPanel.List");
	m_propList[DIP_WIZ_PROP_TAB_OFFER] = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage1.Tabs.Offer.TabPanel.List");
	m_propList[DIP_WIZ_PROP_TAB_TREATY] = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage1.Tabs.Treaty.TabPanel.List");

	m_exchList[DIP_WIZ_PROP_TAB_REQUEST] = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage2.Tabs.Request.TabPanel.List");
	m_exchList[DIP_WIZ_PROP_TAB_OFFER] = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage2.Tabs.Offer.TabPanel.List");
	m_exchList[DIP_WIZ_PROP_TAB_TREATY] = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage2.Tabs.Treaty.TabPanel.List");

	aui_Ldl::SetActionFuncAndCookie(s_dipWizardBlock, "Stage1.Tabs.Request", ProposalTabCallback, (void *)DIP_WIZ_PROP_TAB_REQUEST);
	aui_Ldl::SetActionFuncAndCookie(s_dipWizardBlock, "Stage1.Tabs.Offer", ProposalTabCallback, (void *)DIP_WIZ_PROP_TAB_OFFER);
	aui_Ldl::SetActionFuncAndCookie(s_dipWizardBlock, "Stage1.Tabs.Treaty", ProposalTabCallback, (void *)DIP_WIZ_PROP_TAB_TREATY);

	aui_Ldl::SetActionFuncAndCookie(s_dipWizardBlock, "Stage2.Tabs.Request", ExchangeTabCallback, (void *)DIP_WIZ_PROP_TAB_REQUEST);
	aui_Ldl::SetActionFuncAndCookie(s_dipWizardBlock, "Stage2.Tabs.Offer", ExchangeTabCallback, (void *)DIP_WIZ_PROP_TAB_OFFER);
	aui_Ldl::SetActionFuncAndCookie(s_dipWizardBlock, "Stage2.Tabs.Treaty", ExchangeTabCallback, (void *)DIP_WIZ_PROP_TAB_TREATY);

	m_parchment = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Details.Parchment");
	m_responseDiplomat = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage3.Diplomat");
	//Added by Martin Gühmann to display the emissary photo of recipient
	m_emissary_photo = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Details.Picture");

	m_threatList = (ctp2_ListBox *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage4.List");
	m_threatList->SetActionFuncAndCookie(ThreatListCallback, NULL);

	m_window->SetStronglyModal(TRUE);

	m_recipient = -1;
	m_tone = -1;
	m_proposal = -1;
	m_exchange = -1;
	m_menuProposal = -1;
	m_menuExchange = -1;
	m_proposalDataPending = false;

	m_intelButton->Enable(FALSE);

	m_viewTone = -1;
	m_viewProposal = -1;
	m_viewExchange = -1;
	m_viewThreat = -1;

	m_sendCounter = false;
	//Added by Martin Gühmann to display the emissary photo of recipient
	//Makes shure that the default image is never shown not even for one or two seconds
	if (m_emissary_photo)
	{
		m_emissary_photo->ExchangeImage(0,0, NULL);
	}
}

DipWizard::~DipWizard()
{
	if(m_window) {
		aui_Ldl::DeleteHierarchyFromRoot(s_dipWizardBlock);
	}

	sint32 i;
	for(i = 0; i < DIP_WIZ_STAGE_MAX; i++) {
		m_stages[i] = NULL;
	}

	for(i = 0; i < DIPLOMATIC_TONE_MAX; i++) {
		m_toneButtons[i] = NULL;
	}

	m_nextButton = m_backButton = m_sendButton = m_cancelButton = NULL;
	m_counterOrThreatenButton = NULL;
	m_createButtons = m_viewButtons = NULL;
	m_rejectButton = NULL;
	m_acceptButton = NULL;

	m_nations = NULL;

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

	delete m_curMenu;
}

AUI_ERRCODE DipWizard::Initialize()
{

	if(s_dipWizard)
		return AUI_ERRCODE_OK;

	AUI_ERRCODE err = AUI_ERRCODE_OK;
	s_dipWizard = new DipWizard(&err);

	Assert(err == AUI_ERRCODE_OK);

	return err;
}

AUI_ERRCODE DipWizard::Cleanup()
{
	if(s_dipWizard) {
		Hide();

		delete s_dipWizard;
		s_dipWizard = NULL;
	}
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE DipWizard::Display()
{
	if(!s_dipWizard)
		Initialize();

	Assert(s_dipWizard);
	if(!s_dipWizard) {
		return AUI_ERRCODE_HACK;
	}

	if(g_network.IsActive() && g_player[g_selected_item->GetVisiblePlayer()] &&
	   g_player[g_selected_item->GetVisiblePlayer()]->IsRobot()) {

		return AUI_ERRCODE_OK;
	}

	AUI_ERRCODE err = AUI_ERRCODE_INVALIDPARAM;

	Assert(s_dipWizard->m_window);
	if(s_dipWizard->m_window) {
		err = g_c3ui->AddWindow(s_dipWizard->m_window);
		Assert(err == AUI_ERRCODE_OK);
		if(err == AUI_ERRCODE_OK) {
			err = s_dipWizard->m_window->Show();
		}

		m_recipient = -1;
		SetStage(0);

		FillRecipientLists();

		m_sendCounter = false;

		SetNation(-1);
		SetTone(-1);
		SetProposal(-1);
		SetExchange(-1);


	}

	return err;
}

AUI_ERRCODE DipWizard::Hide()
{
	if(!s_dipWizard)
		return AUI_ERRCODE_OK;

	Assert(s_dipWizard->m_window);
	if(!s_dipWizard->m_window)
		return AUI_ERRCODE_INVALIDPARAM;

	s_dipWizard->m_window->Hide();
	return g_c3ui->RemoveWindow(s_dipWizard->m_window->Id());
}

bool DipWizard::CanInitiateRightNow()
{
	if(!g_network.IsActive()) {

		return true;
	}

	if(!g_network.IsMyTurn()) {

		return false;
	}

	const Diplomat & diplomat =
		Diplomat::GetDiplomat(g_selected_item->GetVisiblePlayer());

	sint32 p;
	for(p = 0; p < k_MAX_PLAYERS; p++) {
		if(p == g_selected_item->GetVisiblePlayer())
			continue;
		if(!g_player[p])
			continue;

		if(diplomat.GetMyLastNewProposal(p) != Diplomat::s_badNewProposal)

			return false;
	}

	return true;
}

void DipWizard::FillProposalLists()
{

	sint32 pr;
	ctp2_ListBox *propList = NULL, *exchList = NULL;

	sint32 i;
	for(i = 0; i < DIP_WIZ_PROP_TAB_MAX; i++) {
		m_propList[i]->Clear();
		m_exchList[i]->Clear();
		m_propList[i]->SetActionFuncAndCookie(PropListCallback, (void *)i);
		m_exchList[i]->SetActionFuncAndCookie(ExchListCallback, (void *)i);
	}

	const Diplomat & diplomat =
		Diplomat::GetDiplomat(g_selected_item->GetVisiblePlayer());

	for(pr = 0; pr < g_theDiplomacyProposalDB->NumRecords(); pr++) {
		const DiplomacyProposalRecord *rec = g_theDiplomacyProposalDB->Get(pr);
		if(!rec) continue;

		if (diplomat.InvalidNewProposal(m_recipient, rec))
			continue;


		if(rec->GetArg1HisCity() && m_recipient >= 0 && m_recipient < k_MAX_PLAYERS && g_player[m_recipient]) {
			sint32 c;
			bool seenOne = false;
			for(c = 0; c < g_player[m_recipient]->m_all_cities->Num(); c++) {
				if(g_player[m_recipient]->m_all_cities->Access(c)->GetEverVisible() & (1 << g_selected_item->GetVisiblePlayer())) {
					seenOne = true;
					break;
				}
			}
			if(!seenOne)
				continue;
		}

		if(rec->GetCategoryRequest()) {
			propList = m_propList[DIP_WIZ_PROP_TAB_REQUEST];
			exchList = m_exchList[DIP_WIZ_PROP_TAB_REQUEST];
		}

		if(rec->GetCategoryGift()) {
			propList = m_propList[DIP_WIZ_PROP_TAB_OFFER];
			exchList = m_exchList[DIP_WIZ_PROP_TAB_OFFER];
		}

		if(rec->GetCategoryTreaty()) {
			propList = m_propList[DIP_WIZ_PROP_TAB_TREATY];
			exchList = m_exchList[DIP_WIZ_PROP_TAB_TREATY];
		}

		AddProposalItem(propList, rec);
		AddProposalItem(exchList, rec);
	}

	if(m_threatList) {
		m_threatList->Clear();
		if(GetStage() == DIP_WIZ_STAGE_MAKE_THREAT) {
			m_sendButton->Enable(FALSE);
		}

		sint32 threatenee = m_recipient;

		if(threatenee > 0 && threatenee < k_MAX_PLAYERS) {
			for(i = 1; i < g_theDiplomacyThreatDB->NumRecords(); i++) {
				const DiplomacyThreatRecord *rec = g_theDiplomacyThreatDB->Get(i);

				switch(rec->GetArg1()) {
				case k_DiplomacyThreat_Arg1_HisCity_Bit:
				case k_DiplomacyThreat_Arg1_SpecialAttack_Bit:
					{
						if(m_recipient < 0 || m_recipient >= k_MAX_PLAYERS || !g_player[threatenee])
							continue;

						bool foundCity = false;

						sint32 c;
						for(c = 0; c < g_player[threatenee]->m_all_cities->Num(); c++) {
							if(g_player[threatenee]->m_all_cities->Access(c).GetEverVisible() & (1 << g_selected_item->GetVisiblePlayer())) {
								foundCity = true;
								break;
							}
						}
						if(!foundCity)
							continue;
						break;
					}
				case k_DiplomacyThreat_Arg1_ThirdParty_Bit:
					{
						sint32 p;
						bool foundThirdParty = false;

						for(p = 1; p < k_MAX_PLAYERS; p++) {
							if(!g_player[p]) continue;

							if(p == g_selected_item->GetVisiblePlayer())
								continue;

							if(p == threatenee)
								continue;

							if(!g_player[g_selected_item->GetVisiblePlayer()]->HasContactWith(p))
								continue;

							foundThirdParty = true;
							break;
						}

						if(!foundThirdParty)
							continue;
						break;
					}

				case k_DiplomacyThreat_Arg1_AgreementId_Bit:
					{
						if(m_recipient < 0 || m_recipient >= k_MAX_PLAYERS || !g_player[threatenee])
							continue;

						const Diplomat & diplomat = Diplomat::GetDiplomat(g_selected_item->GetVisiblePlayer());

						ai::Agreement pact;

						bool has_pacts = diplomat.GetAgreementToBreak(threatenee, pact);
						if (!has_pacts)
							continue;
						break;
					}

				default:
					break;
				}

				ctp2_ListItem * item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("DipWizThreatItem");
				Assert(item);
				if (!item) {
					break;
				}

				item->SetUserData((void *)i);

				ctp2_Static * box = (ctp2_Static *)item->GetChildByIndex(0);
				Assert(box);
				if (!box) {
					break;
				}

				ctp2_Static * text = (ctp2_Static *)box->GetChildByIndex(0);
				Assert(text);
				if (!text) {
					break;
				}

				text->SetText(g_theStringDB->GetNameStr(rec->GetTitle()));
				m_threatList->AddItem(item);
			}
		}
	}
}

void DipWizard::FillRecipientLists()
{
	ctp2_ListItem *item = NULL;
	Player *visPlayer = g_player[g_selected_item->GetVisiblePlayer()];
	if(!visPlayer) return;


	if(m_nations) {
		m_nations->Clear();

		item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("DipWizNationItem");
		Assert(item);
		if(item) {
			ctp2_Static *label = (ctp2_Static *)item->GetChildByIndex(0);
			label->SetText(g_theStringDB->GetNameStr("str_ldl_DipWizNoNation"));
			item->SetUserData((void *)-1);
			m_nations->AddItem(item);
		}

		sint32 pl;
		for(pl = 1; pl < k_MAX_PLAYERS; pl++) {
			if(g_player[pl] == visPlayer) continue;
			if(!g_player[pl]) continue;
			if(!visPlayer->HasContactWith(pl)) continue;

			item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("DipWizNationItem");
			Assert(item);
			if(item) {

				ctp2_Static *label = (ctp2_Static *)item->GetChildByIndex(0);
				MBCHAR buf[k_MAX_NAME_LEN];
				g_player[pl]->m_civilisation->GetCountryName(buf);
				label->SetText(buf);

				item->SetUserData((void *)pl);

				m_nations->AddItem(item);
			}
		}
	}
}

void DipWizard::AddProposalItem(ctp2_ListBox *propList, const DiplomacyProposalRecord *rec)
{
	if (propList)
	{
		ctp2_ListItem * propItem = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("DipWizProposalItem");
		if (propItem)
		{
			ctp2_Static * box = (ctp2_Static *) propItem->GetChildByIndex(0);
			if (box)
			{
				ctp2_Static * label = (ctp2_Static *) box->GetChildByIndex(0);
				if (label) {
					label->SetText(g_theStringDB->GetNameStr(rec->GetTitle()));
				}
			}
			propItem->SetUserData((void *)rec->GetIndex());
			propList->AddItem(propItem);
		}
	}
}

void DipWizard::SetNation(sint32 pl)
{
	if(m_recipient == pl) return;




	if (pl >= 0)
		m_recipient = pl;

	m_intelButton->Enable(TRUE);

	ctp2_ListBox *nationList = (ctp2_ListBox *)m_nations->GetListBox();
	ctp2_ListItem *item = (ctp2_ListItem *)nationList->GetSelectedItem();
	if(!item || (pl != (sint32)item->GetUserData())) {
		sint32 i;
		for(i = 0; i < nationList->NumItems(); i++) {

			item = (ctp2_ListItem *)nationList->GetItemByIndex(i);
			Assert(item);
			if(item && (pl == (sint32)item->GetUserData())) {
				m_nations->SetSelectedItem(i);
				break;
			}
		}
	}
	UpdateButtons();
	UpdateDetails();

	FillProposalLists();





	if (pl < 0)
		m_recipient = pl;
}

void DipWizard::SetTone(sint32 tone)
{
	if(!s_dipWizard) return;

	m_tone = tone;
	sint32 i;
	for(i = 0; i < DIPLOMATIC_TONE_MAX; i++) {
		if(m_toneButtons[i]) {
			if(i == tone) {
				m_toneButtons[i]->SetText("X");
			} else {
				m_toneButtons[i]->SetText("");
			}
		}
	}

	UpdateButtons();
	UpdateDetails();
}

void DipWizard::SetProposal(sint32 prop)
{
	m_proposal = prop;
	UpdateButtons();
	UpdateDetails();

}

void DipWizard::SetExchange(sint32 exch)
{
	m_exchange = exch;
	UpdateButtons();
	UpdateDetails();
}

void DipWizard::SetThreat(sint32 threat)
{
	m_threat = threat;
	UpdateButtons();
	UpdateDetails();
}

sint32 DipWizard::GetStage()
{
	if(!s_dipWizard)
		return -1;

	sint32 i;
	for(i = 0; i < DIP_WIZ_STAGE_MAX; i++) {
		if(!m_stages[i]) continue;
		if(!m_stages[i]->IsHidden())
			return i;
	}
	return -1;
}

void DipWizard::SetStage(sint32 stage)
{
	if(!s_dipWizard)
		return;

	Assert(stage > DIP_WIZ_STAGE_NONE);
	Assert(stage < DIP_WIZ_STAGE_MAX);
	if(stage <= DIP_WIZ_STAGE_NONE || stage >= DIP_WIZ_STAGE_MAX)
		return;

	sint32 i;
	for(i = 0; i < DIP_WIZ_STAGE_MAX; i++) {
		if(!m_stages[i]) continue;
		if(i == stage) m_stages[i]->Show();
		else m_stages[i]->Hide();
	}

	UpdateButtons();
	UpdateProposalStage();
	UpdateExchangeStage();
	UpdateViewProposalStage();
}

void DipWizard::SetViewResponse(sint32 sender, sint32 recipient, bool negotiationsComplete, const Response *useResp, const Response *useThreatResp)
{
	const Response & sender_response =
		Diplomat::GetDiplomat(sender).GetResponsePending(recipient);

	const Response & last_receiver_response =
		Diplomat::GetDiplomat(recipient).GetMyLastResponse(sender);

	bool receiver_has_initiative = Diplomat::GetDiplomat(sender).GetReceiverHasInitiative(recipient);

	Response resp;
	if(useResp) {
		resp = *useResp;
	} else if(negotiationsComplete ||
		( receiver_has_initiative && sender_response.type != RESPONSE_THREATEN)
	   ) {

		resp = last_receiver_response;
	} else if (sender_response.type == RESPONSE_THREATEN) {

		resp = sender_response;
	} else {

		resp = Diplomat::GetDiplomat(recipient).GetResponsePending(sender);
	}

	if(resp.type == RESPONSE_INVALID)
		return;




	Initialize();

	m_viewSender = sender;
	m_viewRecipient = recipient;

	m_viewResponseType = resp.type;

	m_viewTone = -1;
	m_viewProposal = -1;
	m_viewExchange = -1;
	m_viewThreat = -1;


	if(resp.type == RESPONSE_COUNTER) {
		m_viewTone = resp.counter.tone;





			m_viewProposal = diplomacyutil_GetDBIndex(resp.counter.first_type);
			m_viewExchange = diplomacyutil_GetDBIndex(resp.counter.second_type);

		m_viewProposalArg = resp.counter.first_arg;
		m_viewExchangeArg = resp.counter.second_arg;
		m_viewType = DIP_WIZ_VIEW_TYPE_COUNTER;
	} else {
		if(resp.type == RESPONSE_THREATEN) {
			if(useThreatResp) {
				m_viewThreat = useThreatResp->threat.type;
				m_viewThreatArg = useThreatResp->threat.arg;
			} else {
				m_viewThreat = resp.threat.type;
				m_viewThreatArg = resp.threat.arg;
			}
			m_viewType = DIP_WIZ_VIEW_TYPE_THREAT;
		} else {
			if(  resp.type == RESPONSE_ACCEPT
			||   negotiationsComplete
			|| ( resp.type == RESPONSE_REJECT
			&&  (resp.counter.first_type != PROPOSAL_NONE
			||   resp.threat.type != THREAT_NONE))
			){
				m_viewType = DIP_WIZ_VIEW_TYPE_FINAL_RESPONSE;
			} else {
				m_viewType = DIP_WIZ_VIEW_TYPE_RESPONSE;
			}
		}

		Response senderResponse = useThreatResp ? *useThreatResp : sender_response;
		if(senderResponse != Diplomat::s_badResponse) {
			if(senderResponse.type == RESPONSE_THREATEN
			|| senderResponse.threat.type != THREAT_NONE
			){
				m_viewThreat = senderResponse.threat.type;
				m_viewThreatArg = senderResponse.threat.arg;
			}
		}

		const NewProposal & orig = Diplomat::GetDiplomat(sender).GetMyLastNewProposal(recipient);
		if(orig == Diplomat::s_badNewProposal)
		{
			bool isBadProposal = false;
			Assert(isBadProposal);
			return;
		}

		if(  resp.receiverId == g_selected_item->GetVisiblePlayer()
		&& ( last_receiver_response.type == RESPONSE_COUNTER
		||  (last_receiver_response.counter.first_type != PROPOSAL_NONE
		&&   sender_response.type != RESPONSE_THREATEN))
		){

			m_viewProposal = diplomacyutil_GetDBIndex(last_receiver_response.counter.first_type);
			m_viewProposalArg = last_receiver_response.counter.first_arg;

			m_viewExchange = diplomacyutil_GetDBIndex(last_receiver_response.counter.second_type);
			m_viewExchangeArg = last_receiver_response.counter.second_arg;
			m_viewTone = last_receiver_response.counter.tone;
		} else {

			m_viewProposal = diplomacyutil_GetDBIndex(orig.detail.first_type);
			m_viewProposalArg = orig.detail.first_arg;
			m_viewExchange = diplomacyutil_GetDBIndex(orig.detail.second_type);
			m_viewExchangeArg = orig.detail.second_arg;
			m_viewTone = orig.detail.tone;
		}

	}

	Display();
	SetStage(DIP_WIZ_STAGE_VIEW_PROPOSAL);
	UpdateDetails();
}

void DipWizard::SetViewProposal(sint32 sender, sint32 receiver)
{
	m_viewSender = sender;
	m_viewRecipient = receiver;

	m_viewTone = -1;
	m_viewProposal = -1;
	m_viewExchange = -1;
	m_viewThreat = -1;

	NewProposal prop = Diplomat::GetDiplomat(sender).GetMyLastNewProposal(receiver);
	if(prop == Diplomat::s_badNewProposal) return;




	Initialize();

	m_viewProposal = diplomacyutil_GetDBIndex(prop.detail.first_type);
	m_viewProposalArg = prop.detail.first_arg;
	m_viewExchange = diplomacyutil_GetDBIndex(prop.detail.second_type);
	m_viewExchangeArg = prop.detail.second_arg;
	m_viewTone = prop.detail.tone;

	m_viewType = DIP_WIZ_VIEW_TYPE_PROPOSAL;

	m_viewResponseType = -1;

	Display();
	SetStage(DIP_WIZ_STAGE_VIEW_PROPOSAL);
	UpdateDetails();
}

void DipWizard::UpdateProposalStage()
{
	if(GetStage() != DIP_WIZ_STAGE_PROPOSAL && GetStage() != DIP_WIZ_STAGE_EXCHANGE) {

		sint32 i;
		for(i = 0; i < DIP_WIZ_PROP_TAB_MAX; i++) {
			m_propList[i]->DeselectItem(m_propList[i]->GetSelectedItem());
		}

		if(!m_sendCounter)
			SetProposal(-1);

		return;
	}

}

void DipWizard::UpdateViewProposalStage()
{
	if(GetStage() != DIP_WIZ_STAGE_VIEW_PROPOSAL)
		return;

	SlicObject so;
	const MBCHAR *theString = NULL;
	MBCHAR text[k_MAX_NAME_LEN];
	const MBCHAR *counterButtonText = NULL;
	const MBCHAR *rejectButtonText = "str_ldl_DipWizReject";
	const MBCHAR *acceptButtonText = "str_ldl_DipWizAccept";

	ctp2_Static *st = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage3.Text");
	Assert(st);
	if(!st) return;

	switch(m_viewType) {
		case DIP_WIZ_VIEW_TYPE_PROPOSAL:
			so.AddPlayer(m_viewSender);
			theString = "str_ldl_DipWizReceivedProposal";
			counterButtonText = "str_ldl_DipWizCounter";
			DisplayResponseDiplomat(m_viewSender);
			break;
		case DIP_WIZ_VIEW_TYPE_RESPONSE:
		{
			sint32 pl = m_viewRecipient == g_selected_item->GetVisiblePlayer() ? m_viewSender : m_viewRecipient;
			so.AddPlayer(pl);
			DisplayResponseDiplomat(pl);

			if(m_viewResponseType == RESPONSE_ACCEPT) {
				theString = "str_ldl_DipWizReceivedAccept";
				counterButtonText = NULL;
				rejectButtonText = NULL;
			} else {
				theString = "str_ldl_DipWizReceivedReject";

				if(m_viewThreat < 0) {
					counterButtonText = "str_ldl_DipWizThreaten";
					rejectButtonText = NULL;
					acceptButtonText = "str_ldl_DipWizardClose";
				} else {
					counterButtonText = NULL;
					rejectButtonText = NULL;
				}
			}
			break;
		}
		case DIP_WIZ_VIEW_TYPE_FINAL_RESPONSE:
		{
			sint32 pl = m_viewRecipient == g_selected_item->GetVisiblePlayer() ? m_viewSender : m_viewRecipient;
			so.AddPlayer(pl);
			DisplayResponseDiplomat(pl);

			if(m_viewResponseType == RESPONSE_ACCEPT) {
				theString = "str_ldl_DipWizReceivedAccept";
			} else {
				theString = "str_ldl_DipWizReceivedReject";
			}
			counterButtonText = NULL;
			rejectButtonText = NULL;
			acceptButtonText = "str_ldl_Close";
			break;
		}
		case DIP_WIZ_VIEW_TYPE_COUNTER:
			so.AddPlayer(m_viewRecipient);
			theString = "str_ldl_DipWizReceivedCounter";

			counterButtonText = "str_ldl_DipWizThreaten";
			rejectButtonText = "str_ldl_DipWizReject";
			acceptButtonText = "str_ldl_DipWizAccept";
			DisplayResponseDiplomat(m_viewRecipient);
			break;
		case DIP_WIZ_VIEW_TYPE_THREAT:
			so.AddPlayer(m_viewSender);
			DisplayResponseDiplomat(m_viewSender);
			theString = "str_ldl_DipWizReceivedThreat";
			counterButtonText = NULL;
			break;
		default:
			Assert(FALSE);
			so.AddPlayer(m_viewSender);
			theString = "str_ldl_DipWizReceivedProposal";
			break;
	}

	stringutils_Interpret(g_theStringDB->GetNameStr(theString), so, text);
	st->SetText(text);

	if(!counterButtonText) {
		m_counterOrThreatenButton->Hide();
	} else {
		m_counterOrThreatenButton->Show();
		m_counterOrThreatenButton->SetText(g_theStringDB->GetNameStr(counterButtonText));
	}

	if(!rejectButtonText) {
		m_rejectButton->Hide();
	} else {
		m_rejectButton->Show();
		m_rejectButton->SetText(g_theStringDB->GetNameStr(rejectButtonText));
	}

	if(!acceptButtonText) {
		m_acceptButton->Hide();
	} else {
		m_acceptButton->Show();
		m_acceptButton->SetText(g_theStringDB->GetNameStr(acceptButtonText));
	}

	st = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage3");
	st->ShouldDraw(TRUE);
}
void DipWizard::UpdateExchangeStage()
{
	if(GetStage() != DIP_WIZ_STAGE_EXCHANGE) {

		sint32 i;
		for(i = 0; i < DIP_WIZ_PROP_TAB_MAX; i++) {
			m_exchList[i]->DeselectItem(m_exchList[i]->GetSelectedItem());
		}

		SetExchange(-1);
		return;
	}

	const DiplomacyProposalRecord *rec = NULL;
	if(m_proposal >= 0)
		rec = g_theDiplomacyProposalDB->Get(m_proposal);

	ctp2_TabButton *tabButton = NULL;
	ctp2_Tab *tab[3];
	bool changeTab = false;
	sint32 shown = -1;
	char labelStrName[k_MAX_NAME_LEN];
	strcpy(labelStrName, "str_ldl_DipWizSelect");

	tab[0] = (ctp2_Tab *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage2.Tabs.Request");
	if(tab[0])
		tabButton = tab[0]->GetButton();

	if(tabButton) {
		if(rec && rec->GetExcludesRequest()) {

			tabButton->Hide();
			if(tab[0] && tab[0]->GetPanel() && !tab[0]->GetPanel()->IsHidden()) {
				changeTab = true;
				shown = 0;
			}
		} else {
			tabButton->Show();
			strcat(labelStrName, "Request");
		}
	}
	tabButton = NULL;

	tab[1] = (ctp2_Tab *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage2.Tabs.Offer");
	if(tab[1])
		tabButton = tab[1]->GetButton();

	if(tabButton) {
		if(rec && rec->GetExcludesOffer()) {

			tabButton->Hide();
			if(tab[1] && tab[1]->GetPanel() && !tab[1]->GetPanel()->IsHidden()) {
				changeTab = true;
				shown = 1;
			}
		} else {
			tabButton->Show();
			strcat(labelStrName, "Offer");
		}
	}
	tabButton = NULL;

	tab[2] = (ctp2_Tab *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage2.Tabs.Treaty");
	if(tab[2])
		tabButton = tab[2]->GetButton();

	if(tabButton) {
		if(rec && rec->GetExcludesTreaty()) {
			tabButton->Hide();

			if(tab[2] && tab[2]->GetPanel() && !tab[2]->GetPanel()->IsHidden()) {
				changeTab = true;
				shown = 2;
			}
		} else {
			tabButton->Show();
			strcat(labelStrName, "Treaty");
		}
	}

	if(changeTab) {

		ctp2_TabGroup *tabGroup = (ctp2_TabGroup *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage2.Tabs");
		sint32 i;
		for(i = 0; i < DIP_WIZ_PROP_TAB_MAX; i++) {
			if(i == shown) continue;
			if(tab[i])
				tabGroup->SelectTab(tab[i]);
		}
	}

	ctp2_Static *label = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage2.Label");
	if(label) {
		if(m_sendCounter) {
			strcat(labelStrName, "ForCounter");
		}

		const MBCHAR *labelText = g_theStringDB->GetNameStr(labelStrName);
		if(labelText)
			label->SetText(labelText);
	}
}

void DipWizard::UpdateButtons()
{
	sint32 stage = GetStage();

	Assert(m_createButtons);
	Assert(m_viewButtons);
	Assert(m_backButton);
	Assert(m_nextButton);
	Assert(m_cancelButton);
	Assert(m_sendButton);
	ctp2_Static *st = NULL;

	switch(stage) {
		case DIP_WIZ_STAGE_RECIPIENT:
			m_createButtons->Show();
			m_viewButtons->Hide();

			st = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Stage0.NationsLabel");
			if(m_sendCounter) {
				m_nations->Hide();
				if(st)
					st->Hide();
			} else {
				m_nations->Show();
				if(st)
					st->Show();
			}

			m_sendButton->Enable(FALSE);
			m_backButton->Enable(FALSE);
			m_cancelButton->Enable(TRUE);
			if(m_tone >= 0 && m_recipient >= 0) {
				m_nextButton->Enable(TRUE);
			} else {
				m_nextButton->Enable(FALSE);
			}
			if(m_recipient >= 0) {
				m_intelButton->Enable(TRUE);
			} else {
				m_intelButton->Enable(FALSE);
			}
			break;
		case DIP_WIZ_STAGE_PROPOSAL:
			m_createButtons->Show();
			m_viewButtons->Hide();

			m_backButton->Enable(TRUE);
			m_cancelButton->Enable(TRUE);

			m_intelButton->Enable(TRUE);
			if(m_proposal >= 0) {
				m_nextButton->Enable(TRUE);
				m_sendButton->Enable(TRUE);
			} else {
				m_nextButton->Enable(FALSE);
				m_sendButton->Enable(FALSE);
			}
			break;
		case DIP_WIZ_STAGE_EXCHANGE:
		{
			m_createButtons->Show();
			m_viewButtons->Hide();

			m_intelButton->Enable(TRUE);
			if(m_exchange >= 0) {
				m_sendButton->Enable(TRUE);
			} else {
				m_sendButton->Enable(FALSE);
			}
			m_backButton->Enable(TRUE);
			m_nextButton->Enable(FALSE);
			m_cancelButton->Enable(TRUE);

			Assert(m_proposal >= 0);

			break;
		}
		case DIP_WIZ_STAGE_VIEW_PROPOSAL:
		{
			m_createButtons->Hide();
			m_viewButtons->Show();

			m_intelButton->Enable(TRUE);

			break;
		}
		case DIP_WIZ_STAGE_MAKE_THREAT:
		{
			m_createButtons->Show();
			m_viewButtons->Hide();

			m_backButton->Enable(FALSE);
			m_nextButton->Enable(FALSE);
			m_sendButton->Enable(TRUE);
			m_cancelButton->Enable(TRUE);

			m_intelButton->Enable(TRUE);
			m_sendButton->Enable(m_threat >= 0);
			break;
		}
		default:
			Assert(FALSE);
			break;
	}
}

void DipWizard::UpdateDetails()
{
	ctp2_Static *st;
	MBCHAR text[k_MAX_NAME_LEN];
	MBCHAR text2[k_MAX_NAME_LEN];
	bool viewingProposal = (GetStage() == DIP_WIZ_STAGE_VIEW_PROPOSAL) || (GetStage() == DIP_WIZ_STAGE_MAKE_THREAT);

	st = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Details.Year");
	if(st) {
		SlicObject so;
		stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipWizYear"), so, text);
		st->SetText(text);
	}

	st = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Details.Recipient");
	if(st) {
		if(!viewingProposal) {
			//Added by Martin Gühmann to display the emissary photo of recipient
			DisplayDiplomat(m_recipient);
			DisplayParchment(g_selected_item->GetVisiblePlayer());
			if(m_recipient >= 0) {
				SlicObject so;
				so.AddPlayer(m_recipient);
				stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipWizRecipient"), so, text);
				st->SetText(text);
			} else {
				st->SetText("");
			}
		} else {

			if(m_viewRecipient == g_selected_item->GetVisiblePlayer()) {
				SlicObject so;
				so.AddPlayer(m_viewSender);
				stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipWizSender"), so, text);
				st->SetText(text);

				//Modified by Martin Gühmann to display the emissary photo of recipient
				DisplayDiplomat(m_viewRecipient);
				DisplayParchment(m_viewSender);

			} else if(m_viewResponseType == RESPONSE_COUNTER) {
				//Modified by Martin Gühmann to display the emissary photo of recipient
				DisplayDiplomat(m_viewSender);
				DisplayParchment(m_viewRecipient);
				SlicObject so;
				so.AddPlayer(m_viewRecipient);
				stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipWizSender"), so, text);
				st->SetText(text);
			} else if(m_viewSender == g_selected_item->GetVisiblePlayer()) {
				//Added by Martin Gühmann to display the emissary photo of recipient
				DisplayDiplomat(m_viewRecipient);
				DisplayParchment(g_selected_item->GetVisiblePlayer());
				SlicObject so;
				so.AddPlayer(m_viewRecipient);
				stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipWizRecipient"), so, text);
				st->SetText(text);
			} else {
				//Added by Martin Gühmann to display the emissary photo of recipient
				DisplayDiplomat(-1);
				DisplayParchment(g_selected_item->GetVisiblePlayer());
				st->SetText("");
			}
		}
	}

	st = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Details.ToneIcon");
	if(st) {
		sint32 tone = viewingProposal ? m_viewTone : m_tone;
		if(tone >= 0 && tone < DIPLOMATIC_TONE_MAX) {
			st->SetImage(DiplomacyWindow::GetToneIcons()[tone]);
		} else {
			st->SetImage(NULL);
		}
	}

	st = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Details.Proposal");
	if(st) {
		if((viewingProposal && m_viewProposal >= 0) ||
		   (!viewingProposal && m_proposal >= 0)) {
			ProposalData propData;
			FillInProposalData(propData);

			sint32 sender, receiver;
			if(!viewingProposal) {
				sender = g_selected_item->GetVisiblePlayer();
				receiver = m_recipient;
			} else {
				sender = m_viewSender;
				receiver = m_viewRecipient;
			}

			text2[0] = 0;

			DiplomacyWindow::GetProposalDetails(propData, sender, receiver, text, text2,
												(viewingProposal && m_viewResponseType == RESPONSE_COUNTER), NULL);
			st->SetText(text);

			st = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Details.Exchange");
			if(st) {
				if(((viewingProposal && m_viewExchange >= 0) ||
				    (!viewingProposal && m_exchange >= 0)) &&
				   text2[0] != 0) {
					st->SetText(text2);
				} else {
					st->SetText("");
				}
			}

		} else {
			st->SetText("");
			st = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Details.Exchange");
			st->SetText("");
		}
	}

	st = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Details.Response");
	if(st) {

		if(!viewingProposal) {

			st->SetText("");
		} else {
			if(GetStage() == DIP_WIZ_STAGE_MAKE_THREAT) {
				if(m_threat < 0) {
					text[0] = 0;
				} else {
					sint32 threatDBIndex = diplomacyutil_GetDBIndex((THREAT_TYPE)m_threat);
					const DiplomacyThreatRecord *rec = g_theDiplomacyThreatDB->Get(threatDBIndex);
					Assert(rec);
					if(rec) {
						StringId threatDetail = rec->GetDetails();
						SlicObject so;
						so.AddPlayer(m_viewSender);
						so.AddPlayer(m_viewRecipient);
						if(AddThreatData(so, threatDBIndex, m_threatArg)) {
							stringutils_Interpret(g_theStringDB->GetNameStr(threatDetail), so, text);
						} else {

							strcpy(text, "");
						}
					}
				}
			} else {
				if(m_viewThreat >= 0) {
					sint32 threatDBIndex = diplomacyutil_GetDBIndex((THREAT_TYPE)m_viewThreat);
					const DiplomacyThreatRecord *rec = g_theDiplomacyThreatDB->Get(threatDBIndex);
					Assert(rec);
					if(rec) {
						StringId threatDetail = rec->GetDetails();
						SlicObject so;
						so.AddPlayer(m_viewSender);
						so.AddPlayer(m_viewRecipient);
						if(AddThreatData(so, threatDBIndex, m_viewThreatArg)) {
							stringutils_Interpret(g_theStringDB->GetNameStr(threatDetail), so, text);
						} else {

							strcpy(text, "");
						}
					}
				} else {
					text[0] = 0;
				}
#if 0
				switch(m_viewResponseType) {
					case RESPONSE_COUNTER:
					{
						SlicObject so;
						so.AddPlayer(m_viewRecipient);
						stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_DipWizCounterProposal"), so, text);
						break;
					}
					case RESPONSE_THREATEN:
					{
						sint32 threatDBIndex = diplomacyutil_GetDBIndex((THREAT_TYPE)m_viewThreat);
						const DiplomacyThreatRecord *rec = g_theDiplomacyThreatDB->Get(threatDBIndex);
						Assert(rec);
						if(rec) {
							StringId threatDetail = rec->GetDetails();
							SlicObject so;
							so.AddPlayer(m_viewSender);
							so.AddPlayer(m_viewRecipient);
							if(AddThreatData(so, threatDBIndex, m_viewThreatArg)) {
								stringutils_Interpret(g_theStringDB->GetNameStr(threatDetail), so, text);
							} else {
								strcpy(text, g_theStringDB->GetNameStr(threatDetail));
							}
						}
						break;
					}
					case RESPONSE_ACCEPT:
						strcpy(text, g_theStringDB->GetNameStr("str_dip_ResponseAccept"));
						break;
					case RESPONSE_REJECT:
						strcpy(text, g_theStringDB->GetNameStr("str_dip_ResponseReject"));
						break;
					default:

						text[0] = 0;
						break;
				}
#endif
			}
			st->SetText(text);
		}
	}




	st = (ctp2_Static *)aui_Ldl::GetObject(s_dipWizardBlock, "Details");
	st->ShouldDraw(TRUE);
}

void DipWizard::FillInProposalData(ProposalData &prop, bool counter)
{
	if(GetStage() != DIP_WIZ_STAGE_VIEW_PROPOSAL &&
	   GetStage() != DIP_WIZ_STAGE_MAKE_THREAT) {
		if(counter) {
			sint32 reciprocal;
			if (!g_theDiplomacyProposalDB->Get(m_proposal)->GetReciprocalIndex(reciprocal))

				prop.first_type = diplomacyutil_GetProposalType(m_proposal);
			else

				prop.first_type = diplomacyutil_GetProposalType(reciprocal);

			if (!g_theDiplomacyProposalDB->Get(m_exchange)->GetReciprocalIndex(reciprocal))

				prop.second_type = diplomacyutil_GetProposalType(m_exchange);
			else

				prop.second_type = diplomacyutil_GetProposalType(reciprocal);
		} else {
			prop.first_type = diplomacyutil_GetProposalType(m_proposal);
			prop.second_type = diplomacyutil_GetProposalType(m_exchange);
		}

		prop.first_arg = m_proposalArg;
		prop.second_arg = m_exchangeArg;
		prop.tone = (DIPLOMATIC_TONE)m_tone;
	} else {
		prop.first_type = diplomacyutil_GetProposalType(m_viewProposal);
		prop.first_arg = m_viewProposalArg;
		prop.second_type = diplomacyutil_GetProposalType(m_viewExchange);
		prop.second_arg = m_viewExchangeArg;
		prop.tone = (DIPLOMATIC_TONE)m_viewTone;
	}
}





void DipWizard::ToneButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	sint32 tone = (sint32)cookie;
	Assert(tone >= 0);
	Assert(tone < DIPLOMATIC_TONE_MAX);
	if(tone < 0 || tone >= DIPLOMATIC_TONE_MAX) return;

	SetTone(tone);
}

void DipWizard::PropListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_LISTBOX_ACTION_SELECT && action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) return;
	ctp2_ListBox *lb = (ctp2_ListBox *)control;

	Assert(lb);
	if(!lb) return;

	if(!lb->IsHidden()) {
		ctp2_ListItem *item = (ctp2_ListItem *)lb->GetSelectedItem();
		if(!item || !ProposalContextMenu((sint32)item->GetUserData())) {
			SetProposal(-1);
		} else {
			if(!m_proposalDataPending)
				SetProposal((sint32)item->GetUserData());
			else
				m_menuProposal = (sint32)item->GetUserData();
		}
	}
}

void DipWizard::ExchListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_LISTBOX_ACTION_SELECT && action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) return;
	ctp2_ListBox *lb = (ctp2_ListBox *)control;

	Assert(lb);
	if(!lb) return;

	if(!lb->IsHidden()) {
		ctp2_ListItem *item = (ctp2_ListItem *)lb->GetSelectedItem();
		if(!item || !ProposalContextMenu((sint32)item->GetUserData())) {
			SetExchange(-1);
		} else {
			if(!m_proposalDataPending)
				SetExchange((sint32)item->GetUserData());
			else
				m_menuExchange = (sint32)item->GetUserData();
		}
	}
}

void DipWizard::NationCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_DROPDOWN_ACTION_SELECT)
		return;

	Assert(control == m_nations);
	if(control != m_nations) return;

	ctp2_ListItem *item = (ctp2_ListItem *)m_nations->GetListBox()->GetSelectedItem();
	if(!item) SetNation(-1);
	else SetNation((sint32)item->GetUserData());
	UpdateButtons();
}

void DipWizard::BackCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	if(GetStage() > 0) {
		if(m_sendCounter && GetStage() == DIP_WIZ_STAGE_EXCHANGE)
			SetStage(DIP_WIZ_STAGE_RECIPIENT);
		else
			SetStage(GetStage() - 1);
	}
}

void DipWizard::NextCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	if(GetStage() < DIP_WIZ_STAGE_MAX - 1) {
		if(m_sendCounter && GetStage() == DIP_WIZ_STAGE_RECIPIENT) {
			SetStage(DIP_WIZ_STAGE_EXCHANGE);
		} else {
			SetStage(GetStage() + 1);
		}
	}

}

void DipWizard::CancelCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	if(GetStage() == DIP_WIZ_STAGE_MAKE_THREAT) {
		SetViewResponse(m_viewSender, m_viewRecipient, false);
	} else if(m_sendCounter) {
		SetViewProposal(m_viewSender, m_viewRecipient);
	} else {
		Hide();
	}
}

//----------------------------------------------------------------------------
//
// Name       : DipWizard::SendCallback
//
// Description: Send a NewProposal/Counter/Threat by pressing the 'Send' button
//
// Parameters : action                  : an AUI_BUTTON_ACTION (aui_button.h)
//            : control                   ?
//            : data                      ?
//            : cookie                    ?
//
// Globals    : g_network               : multiplayer manager
//              g_selected_item         : selected player
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void DipWizard::SendCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	if(m_sendCounter) {
		Response response;
		response.senderId = m_recipient;
		response.receiverId = g_selected_item->GetVisiblePlayer();
		response.priority = 9999;
		FillInProposalData(response.counter, true);
		response.type = RESPONSE_COUNTER;
		response.counter.tone = (DIPLOMATIC_TONE)m_tone;

		m_sendCounter = false;


		if(GetStage() != DIP_WIZ_STAGE_VIEW_PROPOSAL) {
			Hide();

		}

		Diplomat::GetDiplomat(response.receiverId).ExecuteResponse(response);
	} else if(GetStage() != DIP_WIZ_STAGE_MAKE_THREAT) {
		NewProposal prop;

		prop.senderId = g_selected_item->GetVisiblePlayer();
		prop.receiverId = m_recipient;
		prop.priority=9999;
		FillInProposalData(prop.detail);


		if(GetStage() != DIP_WIZ_STAGE_VIEW_PROPOSAL) {
			Hide();

		}

		Diplomat::GetDiplomat(g_selected_item->GetVisiblePlayer()).ExecuteNewProposal(prop);
	} else {

		Response resp;
		resp.type = RESPONSE_THREATEN;
		resp.senderId = g_selected_item->GetVisiblePlayer();
		resp.receiverId = m_viewRecipient;
		resp.threat.type = diplomacyutil_GetThreatType(m_threat);
		resp.threat.arg = m_threatArg;
		resp.priority=9999;
		if(g_network.IsActive()) {
			SetStage(DIP_WIZ_STAGE_RECIPIENT);
		}

		if(g_player[resp.senderId]->IsHuman()
		&& g_selected_item->GetVisiblePlayer() != resp.senderId
		){
			Hide();
		}
		else if(g_player[resp.receiverId]->IsHuman()
		&&      g_selected_item->GetVisiblePlayer() != resp.receiverId
		){
			Hide();
		}

		Diplomat::GetDiplomat(g_selected_item->GetVisiblePlayer()).ExecuteResponse(resp);
	}

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ResumeEmailAndHotSeatDiplomacy,
						   GEA_Player,		g_selected_item->GetCurPlayer(),
						   GEA_End
						  );
}

const MBCHAR *DipWizard::GetCategoryName(DIP_WIZ_PROP_TAB tab)
{
	switch(tab) {
		case DIP_WIZ_PROP_TAB_REQUEST: return "Request";
		case DIP_WIZ_PROP_TAB_OFFER: return "Offer";
		case DIP_WIZ_PROP_TAB_TREATY: return "Treaty";
		default: return "";
	}
}

void DipWizard::ProposalTabCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != ctp2_Tab::ACTION_ACTIVATED) return;

	MBCHAR buf[k_MAX_NAME_LEN];
	sprintf(buf, "%s.Stage%d.Tabs.%s.TabPanel.List", s_dipWizardBlock, DIP_WIZ_STAGE_PROPOSAL,
			GetCategoryName(DIP_WIZ_PROP_TAB(sint32(cookie))));

	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(buf);
	if(lb) {
		if(lb->GetSelectedItem()) {
			lb->DeselectItem(lb->GetSelectedItem());
		} else {
			SetProposal(-1);
		}
	}
}

void DipWizard::ExchangeTabCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != ctp2_Tab::ACTION_ACTIVATED) return;

	MBCHAR buf[k_MAX_NAME_LEN];
	sprintf(buf, "%s.Stage%d.Tabs.%s.TabPanel.List", s_dipWizardBlock, DIP_WIZ_STAGE_EXCHANGE,
			GetCategoryName(DIP_WIZ_PROP_TAB(sint32(cookie))));

	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(buf);
	if(lb) {
		if(lb->GetSelectedItem()) {
			lb->DeselectItem(lb->GetSelectedItem());
		} else {
			SetExchange(-1);
		}
	}
}

void DipWizard::AcceptCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(GetStage() == DIP_WIZ_STAGE_VIEW_PROPOSAL);
	if(GetStage() != DIP_WIZ_STAGE_VIEW_PROPOSAL)
		return;

	if( m_viewType == DIP_WIZ_VIEW_TYPE_FINAL_RESPONSE
	|| (m_viewSender == g_selected_item->GetVisiblePlayer()
	&&  m_viewResponseType != RESPONSE_COUNTER)
	){

		Hide();

		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ResumeEmailAndHotSeatDiplomacy,
							   GEA_Player,		g_selected_item->GetCurPlayer(),
							   GEA_End
							  );

		return;
	}

	Response response;
	response.senderId = m_viewSender;
	response.receiverId = m_viewRecipient;

	if(m_viewSender == g_selected_item->GetVisiblePlayer()) {
		if(m_viewResponseType == RESPONSE_COUNTER) {
			response.type = RESPONSE_ACCEPT;
		} else {

			response.type = RESPONSE_REJECT;
		}

		Hide();
		Diplomat::GetDiplomat(m_viewSender).ExecuteResponse(response);
	} else {
		response.type = RESPONSE_ACCEPT;
		Hide();
		Diplomat::GetDiplomat(m_viewRecipient).ExecuteResponse(response);
	}

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ResumeEmailAndHotSeatDiplomacy,
						   GEA_Player,		g_selected_item->GetCurPlayer(),
						   GEA_End
						  );
}

void DipWizard::RejectCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(GetStage() == DIP_WIZ_STAGE_VIEW_PROPOSAL);
	if(GetStage() != DIP_WIZ_STAGE_VIEW_PROPOSAL)
		return;

	Response response;
	response.senderId = m_viewSender;
	response.receiverId = m_viewRecipient;

	response.type = RESPONSE_REJECT;
	Hide();
	Diplomat::GetDiplomat(g_selected_item->GetVisiblePlayer()).ExecuteResponse(response);

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ResumeEmailAndHotSeatDiplomacy,
						   GEA_Player,		g_selected_item->GetCurPlayer(),
						   GEA_End
						  );
}

void DipWizard::CounterOrThreatenCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(GetStage() == DIP_WIZ_STAGE_VIEW_PROPOSAL);
	if(GetStage() != DIP_WIZ_STAGE_VIEW_PROPOSAL)
		return;

	if(m_viewSender == g_selected_item->GetVisiblePlayer()) {
		NewProposal prop = Diplomat::GetDiplomat(m_viewSender).GetMyLastNewProposal(m_viewRecipient);

		m_viewProposal = diplomacyutil_GetDBIndex(prop.detail.first_type);
		m_viewProposalArg = prop.detail.first_arg;
		m_viewExchange = diplomacyutil_GetDBIndex(prop.detail.second_type);
		m_viewExchangeArg = prop.detail.second_arg;
		m_viewTone = prop.detail.tone;
		m_viewThreat = -1;
		m_viewResponseType = RESPONSE_THREATEN;

		m_threatArg = -1;
		SetThreat(-1);

		SetStage(DIP_WIZ_STAGE_MAKE_THREAT);
	} else {
		m_sendCounter = true;
		SetStage(DIP_WIZ_STAGE_RECIPIENT);
		SetNation(m_viewSender);
		SetTone((sint32)DIPLOMATIC_TONE_EQUAL);
		m_proposalArg = m_viewProposalArg;
		sint32 prop;
		if(g_theDiplomacyProposalDB->Get(m_viewProposal)->GetReciprocalIndex(prop)) {
			SetProposal(prop);
		} else {
			SetProposal(m_viewProposal);
		}
		SetExchange(-1);
	}
}




void DipWizard::ProcessMenuSelection(sint32 itemIndex, void *cookie)
{
	bool isExchange = GetStage() == DIP_WIZ_STAGE_EXCHANGE;
	sint32 prop = isExchange ? m_menuExchange : m_menuProposal;

	Assert((prop >= 0) && (prop < g_theDiplomacyProposalDB->NumRecords()));
	if((prop >= 0) && (prop < g_theDiplomacyProposalDB->NumRecords())) {
		const DiplomacyProposalRecord *rec = g_theDiplomacyProposalDB->Get(prop);
		DiplomacyArg & arg = isExchange ? m_exchangeArg : m_proposalArg;

		switch(rec->GetArg1()) {
			case k_DiplomacyProposal_Arg1_OwnCity_Bit:
			case k_DiplomacyProposal_Arg1_HisCity_Bit:
				arg.cityId = (sint32)cookie;
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
				arg.advanceType = (sint32)cookie;
				break;
			case k_DiplomacyProposal_Arg1_OwnUnitType_Bit:
				break;
			case k_DiplomacyProposal_Arg1_OwnGood_Bit:
				break;
			case k_DiplomacyProposal_Arg1_HisGood_Bit:
				break;
			case k_DiplomacyProposal_Arg1_OwnGold_Bit:
			case k_DiplomacyProposal_Arg1_HisGold_Bit:
				arg.gold = (sint32)cookie;
				break;
			case k_DiplomacyProposal_Arg1_ThirdParty_Bit:
				arg.playerId = (sint32)cookie;
				break;
			case k_DiplomacyProposal_Arg1_Percent_Bit:
				arg.percent = (double) ((sint32) cookie) / 100.0;
				break;
			default:

				Assert(FALSE);
				break;
		}
	}
	if(isExchange) {
		SetExchange(m_menuExchange);
	} else {
		SetProposal(m_menuProposal);
	}
}

void DipWizard::ProcessMenuCancel()
{
	if(GetStage() == DIP_WIZ_STAGE_EXCHANGE) {
		SetExchange(-1);
	} else {
		SetProposal(-1);
	}
}

void DipWizard::MenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	Assert(menu == m_curMenu);
	if(menu != m_curMenu)
		return;

	if(action == CTP2_MENU_ACTION_SELECT) {
		ProcessMenuSelection(itemIndex, cookie);
	} else if(action == CTP2_MENU_ACTION_CANCEL) {
		ProcessMenuCancel();
	}

	delete m_curMenu;
	m_curMenu = NULL;

	m_menuProposal = -1;
	m_menuExchange = -1;
	m_proposalDataPending = false;
}

bool DipWizard::ProposalContextMenu(sint32 proposal)
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
	m_curMenu = new ctp2_Menu(true, DipWizard::MenuCallback);
	switch(rec->GetArg1()) {
		case k_DiplomacyProposal_Arg1_OwnCity_Bit:
			AddCityItems(m_curMenu, g_selected_item->GetVisiblePlayer());
			break;
		case k_DiplomacyProposal_Arg1_HisCity_Bit:
			AddCityItems(m_curMenu, m_recipient);
			break;
		case k_DiplomacyProposal_Arg1_OwnArmy_Bit:

			break;
		case k_DiplomacyProposal_Arg1_HisArmy_Bit:

			break;
		case k_DiplomacyProposal_Arg1_HisAgreement_Bit:

			break;
		case k_DiplomacyProposal_Arg1_OwnAdvance_Bit:
			AddAdvanceItems(m_curMenu, g_selected_item->GetVisiblePlayer(), m_recipient);
			break;
		case k_DiplomacyProposal_Arg1_HisAdvance_Bit:
			AddAdvanceItems(m_curMenu, m_recipient, g_selected_item->GetVisiblePlayer());
			break;
		case k_DiplomacyProposal_Arg1_OwnStopResearch_Bit:
			AddStopResearchItems(m_curMenu, g_selected_item->GetVisiblePlayer());
			break;
		case k_DiplomacyProposal_Arg1_HisStopResearch_Bit:
			AddStopResearchItems(m_curMenu, m_recipient);
			break;
		case k_DiplomacyProposal_Arg1_OwnUnitType_Bit:

			break;
		case k_DiplomacyProposal_Arg1_OwnGood_Bit:

			break;
		case k_DiplomacyProposal_Arg1_HisGood_Bit:

			break;
		case k_DiplomacyProposal_Arg1_OwnGold_Bit:
			RequestGoldValue(g_selected_item->GetVisiblePlayer());
			needItems = false;
			break;
		case k_DiplomacyProposal_Arg1_HisGold_Bit:
			RequestGoldValue(m_recipient);
			needItems = false;
			break;
		case k_DiplomacyProposal_Arg1_ThirdParty_Bit:
			AddThirdPartyItems(m_curMenu, g_selected_item->GetVisiblePlayer(), m_recipient);
			break;
		case k_DiplomacyProposal_Arg1_OwnPollution_Bit:
			RequestPollutionValue(g_selected_item->GetVisiblePlayer());
			needItems = false;
			break;
		case k_DiplomacyProposal_Arg1_HisPollution_Bit:
			RequestPollutionValue(m_recipient);
			needItems = false;
			break;
		case k_DiplomacyProposal_Arg1_Percent_Bit:
			RequestPercentValue();
			needItems = false;
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
	} else {
		m_curMenu->Open();
		m_proposalDataPending = true;
	}
	return true;
}

void DipWizard::AddCityItems(ctp2_Menu *menu, sint32 player)
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
		if(player != g_selected_item->GetVisiblePlayer()) {

			if(!(city.GetEverVisible() & (1 << g_selected_item->GetVisiblePlayer())))
				continue;
		}
		menu->AddItem(city.GetName(), NULL, (void *)city.m_id);
	}
}

void DipWizard::AddAgreementItems(ctp2_Menu *menu, sint32 player)
{
	Assert(player >= 0);
	Assert(player < k_MAX_PLAYERS);
	if((player < 0) || (player >= k_MAX_PLAYERS))
		return;

	Assert(g_player[player]);
	if(!g_player[player])
		return;

	sint32 visplayer = g_selected_item->GetVisiblePlayer();

	const AgreementMatrix & agreement_matrix = AgreementMatrix::s_agreements;
	ai::Agreement tmp_agreement;
	if (agreement_matrix.HasAgreement(player, visplayer, PROPOSAL_TREATY_PEACE))
	{
		tmp_agreement = agreement_matrix.GetAgreement(player, visplayer, PROPOSAL_TREATY_PEACE);
		menu->AddItem(g_theStringDB->GetNameStr("DIP_TREATY_PEACE"), NULL, (void *)PROPOSAL_TREATY_PEACE);
	}

	if (agreement_matrix.HasAgreement(player, visplayer, PROPOSAL_TREATY_TRADE_PACT))
	{
		tmp_agreement = agreement_matrix.GetAgreement(player, visplayer, PROPOSAL_TREATY_TRADE_PACT);
		menu->AddItem(g_theStringDB->GetNameStr("DIP_TREATY_TRADE_PACT"), NULL, (void *)PROPOSAL_TREATY_TRADE_PACT);
	}

	if (agreement_matrix.HasAgreement(player, visplayer, PROPOSAL_TREATY_RESEARCH_PACT))
	{
		tmp_agreement = agreement_matrix.GetAgreement(player, visplayer, PROPOSAL_TREATY_RESEARCH_PACT);
		menu->AddItem(g_theStringDB->GetNameStr("DIP_TREATY_RESEARCH_PACT"), NULL, (void *)PROPOSAL_TREATY_RESEARCH_PACT);
	}

	if (agreement_matrix.HasAgreement(player, visplayer, PROPOSAL_TREATY_MILITARY_PACT))
	{
		tmp_agreement = agreement_matrix.GetAgreement(player, visplayer, PROPOSAL_TREATY_MILITARY_PACT);
		menu->AddItem(g_theStringDB->GetNameStr("DIP_TREATY_MILITARY_PACT"), NULL, (void *)PROPOSAL_TREATY_MILITARY_PACT);
	}

	if (agreement_matrix.HasAgreement(player, visplayer, PROPOSAL_TREATY_POLLUTION_PACT))
	{
		tmp_agreement = agreement_matrix.GetAgreement(player, visplayer, PROPOSAL_TREATY_POLLUTION_PACT);
		menu->AddItem(g_theStringDB->GetNameStr("DIP_TREATY_POLLUTION_PACT"), NULL, (void *)PROPOSAL_TREATY_POLLUTION_PACT);
	}

	if (agreement_matrix.HasAgreement(player, visplayer, PROPOSAL_TREATY_ALLIANCE))
	{
		tmp_agreement = agreement_matrix.GetAgreement(player, visplayer, PROPOSAL_TREATY_ALLIANCE);
		menu->AddItem(g_theStringDB->GetNameStr("DIP_TREATY_ALLIANCE"), NULL, (void *)PROPOSAL_TREATY_ALLIANCE);
	}
}

void DipWizard::AddAdvanceItems(ctp2_Menu *menu, sint32 sender, sint32 receiver)
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

void DipWizard::AddStopResearchItems(ctp2_Menu *menu, sint32 playerId)
{
	Assert(playerId >= 0);
	Assert(playerId < k_MAX_PLAYERS);
	if((playerId < 0) || (playerId >= k_MAX_PLAYERS))
		return;

	Assert(g_player[playerId] && g_player[playerId]->m_advances);
	if(!g_player[playerId] || !g_player[playerId]->m_advances)
		return;

	sint32 a;
	for(a = 0; a < g_theAdvanceDB->NumRecords(); a++) {
		if(!g_player[playerId]->m_advances->CanResearch(a)) {

			continue;
		}

		menu->AddItem(g_theAdvanceDB->Get(a)->GetNameText(), NULL, (void *)a);
	}
}

void DipWizard::AddThirdPartyItems(ctp2_Menu *menu, sint32 sender, sint32 receiver)
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

bool DipWizard::AddThreatData(SlicObject &so, sint32 threat, const DiplomacyArg &arg)
{
	const DiplomacyThreatRecord *rec = g_theDiplomacyThreatDB->Get(threat);
	Assert(rec);
	if(!rec)
		return false;

	switch (rec->GetArg1())
    {
	case k_DiplomacyThreat_Arg1_HisCity_Bit:
	case k_DiplomacyThreat_Arg1_SpecialAttack_Bit:
	    {
		    Unit city(arg.cityId);
		    if (city.IsValid())
            {
			    so.AddCity(Unit(arg.cityId));
			    return true;
		    }
            return false;
	    }
	case k_DiplomacyThreat_Arg1_ThirdParty_Bit:
	    {
		    so.AddPlayer(arg.playerId);
		    return true;
	    }
	case k_DiplomacyThreat_Arg1_AgreementId_Bit:
	    {
	        ai::Agreement agreement =
                AgreementMatrix::s_agreements.GetAgreement
                    (m_viewRecipient,
                     g_selected_item->GetVisiblePlayer(),
                     static_cast<PROPOSAL_TYPE>(arg.agreementId)
                    );
			so.AddAgreement(agreement);
			return true;
		}
	default:
		return true;
	}
}

void DipWizard::PollutionOk(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipPollutionRequest.Spinner");

	Assert(m_pollutionRequestWindow);
	if(m_pollutionRequestWindow) {
		g_c3ui->RemoveWindow(m_pollutionRequestWindow->Id());
	}

	if(GetStage() == DIP_WIZ_STAGE_PROPOSAL) {
		m_proposalArg.pollution = spinner->GetValueX();
		SetProposal(m_menuProposal);
		m_menuProposal = -1;
	} else {
		m_exchangeArg.pollution = spinner->GetValueX();
		SetExchange(m_menuExchange);
		m_menuExchange = -1;
	}

	m_proposalDataPending = false;
}

void DipWizard::PercentOk(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipPercentRequest.Spinner");

	Assert(m_percentRequestWindow);
	if(m_percentRequestWindow) {
		g_c3ui->RemoveWindow(m_percentRequestWindow->Id());
	}

	if(GetStage() == DIP_WIZ_STAGE_PROPOSAL) {
		m_proposalArg.percent = (double)spinner->GetValueX() / 100.0;
		SetProposal(m_menuProposal);
		m_menuProposal = -1;
	} else {
		m_exchangeArg.percent = (double)spinner->GetValueX() / 100.0;
		SetExchange(m_menuExchange);
		m_menuExchange = -1;
	}

	m_proposalDataPending = false;
}

void DipWizard::GoldOk(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipGoldRequest.Spinner");

	Assert(m_goldRequestWindow);
	if(m_goldRequestWindow) {
		g_c3ui->RemoveWindow(m_goldRequestWindow->Id());
	}

	if(GetStage() == DIP_WIZ_STAGE_PROPOSAL) {
		m_proposalArg.gold = spinner->GetValueX();
		SetProposal(m_menuProposal);
		m_menuProposal = -1;
	} else {
		m_exchangeArg.gold = spinner->GetValueX();
		SetExchange(m_menuExchange);
		m_menuExchange = -1;
	}

	m_proposalDataPending = false;
}

void DipWizard::GoldCancel(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(m_goldRequestWindow);
	if(m_goldRequestWindow) {
		g_c3ui->RemoveWindow(m_goldRequestWindow->Id());
	}


	ProcessMenuCancel();
	m_proposalDataPending = false;
}

void DipWizard::PollutionCancel(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(m_pollutionRequestWindow);
	if(m_pollutionRequestWindow) {
		g_c3ui->RemoveWindow(m_pollutionRequestWindow->Id());
	}


	ProcessMenuCancel();
	m_proposalDataPending = false;
}

void DipWizard::PercentCancel(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(m_percentRequestWindow);
	if(m_percentRequestWindow) {
		g_c3ui->RemoveWindow(m_percentRequestWindow->Id());
	}


	ProcessMenuCancel();
	m_proposalDataPending = false;
}

void DipWizard::RequestGoldValue(sint32 player)
{
	if(!m_goldRequestWindow) {
		m_goldRequestWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot("DipGoldRequest");
		Assert(m_goldRequestWindow);
		if(!m_goldRequestWindow)
			return;




		aui_Ldl::SetActionFuncAndCookie("DipGoldRequest.OkButton", DipWizard::GoldOk, NULL);
		aui_Ldl::SetActionFuncAndCookie("DipGoldRequest.CancelButton", DipWizard::GoldCancel, NULL);
		m_goldRequestWindow->SetStronglyModal(TRUE);
	}
	ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipGoldRequest.Spinner");
	if(player == g_selected_item->GetVisiblePlayer()) {
		spinner->SetMaximum(g_player[player]->m_gold->GetLevel(), 0);
	} else {

		spinner->SetMaximum(~(1 << 31), 0);
	}

	g_c3ui->AddWindow(m_goldRequestWindow);
	m_proposalDataPending = true;
}

void DipWizard::RequestPollutionValue(sint32 player)
{
	if(!m_pollutionRequestWindow) {
		m_pollutionRequestWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot("DipPollutionRequest");
		Assert(m_pollutionRequestWindow);
		if(!m_pollutionRequestWindow)
			return;




		aui_Ldl::SetActionFuncAndCookie("DipPollutionRequest.OkButton", DipWizard::PollutionOk, NULL);
		aui_Ldl::SetActionFuncAndCookie("DipPollutionRequest.CancelButton", DipWizard::PollutionCancel, NULL);
		m_pollutionRequestWindow->SetStronglyModal(TRUE);
	}
	ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipPollutionRequest.Spinner");

	spinner->SetMaximum(static_cast<sint32>(g_player[player]->GetPollutionLevel() * 0.95), 0);

	spinner->SetMinimum(static_cast<sint32>(g_player[player]->GetPollutionLevel() * 0.25), 0);

	spinner->SetPage(static_cast<sint32>(g_player[player]->GetPollutionLevel() * 0.20), 0);

	spinner->SetIncrement(static_cast<sint32>(g_player[player]->GetPollutionLevel() * 0.10), 0);

	g_c3ui->AddWindow(m_pollutionRequestWindow);
	m_proposalDataPending = true;
}

void DipWizard::RequestPercentValue()
{
	if(!m_percentRequestWindow) {
		m_percentRequestWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot("DipPercentRequest");
		Assert(m_percentRequestWindow);
		if(!m_percentRequestWindow)
			return;




		aui_Ldl::SetActionFuncAndCookie("DipPercentRequest.OkButton", DipWizard::PercentOk, NULL);
		aui_Ldl::SetActionFuncAndCookie("DipPercentRequest.CancelButton", DipWizard::PercentCancel, NULL);
		m_percentRequestWindow->SetStronglyModal(TRUE);
	}
	ctp2_Spinner *spinner = (ctp2_Spinner *)aui_Ldl::GetObject("DipPercentRequest.Spinner");

	spinner->SetMaximum(75, 0);

	spinner->SetMinimum(5,0);

	spinner->SetPage(20, 0);

	spinner->SetIncrement(10, 0);

	g_c3ui->AddWindow(m_percentRequestWindow);
	m_proposalDataPending = true;
}

void DipWizard::ThreatMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	if((action != CTP2_MENU_ACTION_CANCEL) &&
	   (action != CTP2_MENU_ACTION_SELECT))
		return;

	Assert(m_threatMenu && (m_threatMenu == menu));
	if(m_threatMenu) {
		if(action == CTP2_MENU_ACTION_CANCEL) {
			SetThreat(-1);
			m_sendButton->Enable(FALSE);
			m_threatList->DeselectItem(m_threatList->GetSelectedItem());
		} else {
			ctp2_ListItem *item = (ctp2_ListItem *)m_threatList->GetSelectedItem();
			Assert(item);
			if(item) {
				m_menuThreat = (sint32)item->GetUserData();
				const DiplomacyThreatRecord *rec = g_theDiplomacyThreatDB->Get(m_threat);
				Assert(rec);
				if(rec) {
					switch(rec->GetArg1()) {
						case k_DiplomacyThreat_Arg1_HisCity_Bit:
						case k_DiplomacyThreat_Arg1_SpecialAttack_Bit:
							m_threatArg.cityId = (sint32)cookie;
							break;
						case k_DiplomacyThreat_Arg1_ThirdParty_Bit:
							m_threatArg.playerId = (sint32)cookie;
							break;
						case k_DiplomacyThreat_Arg1_AgreementId_Bit:
							m_threatArg.agreementId = (sint32)cookie;
							break;
						default:

							Assert(FALSE);
							break;
					}

					SetThreat(m_menuThreat);
					m_sendButton->Enable(TRUE);
				}
			} else {
				SetThreat(-1);
				m_sendButton->Enable(FALSE);
			}
		}

		delete m_threatMenu;
		m_threatMenu = NULL;
	}
}

bool DipWizard::ThreatContextMenu(sint32 threat)
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
	m_threatMenu = new ctp2_Menu(true, DipWizard::ThreatMenuCallback);
	switch(rec->GetArg1()) {
		case k_DiplomacyThreat_Arg1_HisCity_Bit:
		case k_DiplomacyThreat_Arg1_SpecialAttack_Bit:
			AddCityItems(m_threatMenu, m_viewRecipient);
			break;
		case k_DiplomacyThreat_Arg1_ThirdParty_Bit:
			AddThirdPartyItems(m_threatMenu, g_selected_item->GetVisiblePlayer(), m_viewRecipient);
			break;
		case k_DiplomacyThreat_Arg1_AgreementId_Bit:
			AddAgreementItems(m_threatMenu, m_viewRecipient);
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
		m_threatDataPending = true;
	}
	return true;
}

void DipWizard::ThreatListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_LISTBOX_ACTION_SELECT && action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) return;
	ctp2_ListBox *lb = (ctp2_ListBox *)control;

	Assert(lb);
	if(!lb) return;

	if(!lb->IsHidden()) {
		ctp2_ListItem *item = (ctp2_ListItem *)lb->GetSelectedItem();
		if(!item || !ThreatContextMenu((sint32)item->GetUserData())) {
			SetThreat(-1);
			m_sendButton->Enable(FALSE);
		} else {
			m_sendButton->Enable(TRUE);
			if(!m_proposalDataPending)
				SetThreat((sint32)item->GetUserData());
			else
				m_menuThreat = (sint32)item->GetUserData();
		}
	}
}

STDEHANDLER(DipWizResponseReady)
{
	sint32 p1, p2;
	if(!args->GetPlayer(0, p1)) return GEV_HD_Continue;
	if(!args->GetPlayer(1, p2)) return GEV_HD_Continue;

	if(p1 == g_selected_item->GetVisiblePlayer() ||
	   p2 == g_selected_item->GetVisiblePlayer()) {

		DipWizard::SetViewResponse(p1, p2, false);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(DipWizNewProposalEvent)
{
	sint32 p1, p2;
	if(!args->GetPlayer(0, p1)) return GEV_HD_Continue;
	if(!args->GetPlayer(1, p2)) return GEV_HD_Continue;

	if(p2 == g_selected_item->GetVisiblePlayer()) {
		DipWizard::SetViewProposal(p1, p2);
	}
	return GEV_HD_Continue;
}

// Not called
STDEHANDLER(DipWizContinueDiplomacyEvent)
{
	sint32 p1, p2;
	if(!args->GetPlayer(0, p1)) return GEV_HD_Continue;
	if(!args->GetPlayer(1, p2)) return GEV_HD_Continue;

	if(p1 == g_selected_item->GetVisiblePlayer()) {

		DipWizard::SetViewResponse(p1, p2, true);

		RESPONSE_TYPE rtype = Diplomat::GetDiplomat(p2).GetResponsePending(p1).type;
		/// @todo Probably need to do something here - this doesn't make sense
	} else if(p2 == g_selected_item->GetVisiblePlayer()) {

		DipWizard::SetViewResponse(p1, p2, true);
	}
	return GEV_HD_Continue;
}

void DipWizard::InitializeEvents()
{
	g_gevManager->AddCallback(GEV_ResponseReady, GEV_PRI_Post, &s_DipWizResponseReady);
	g_gevManager->AddCallback(GEV_NewProposal, GEV_PRI_Post, &s_DipWizNewProposalEvent);
}

void DipWizard::NotifyResponse(const Response &resp, sint32 responder, sint32 other_player)
{
	if(other_player == g_selected_item->GetVisiblePlayer()) {
		Assert(resp.senderId > 0);
		Assert(resp.receiverId > 0);
		SetViewResponse(resp.senderId, resp.receiverId, true, &resp);
	}
}

void DipWizard::NotifyThreatRejected(const Response &resp, const Response &sender_response, sint32 responder, sint32 other_player)
{
	if(other_player == g_selected_item->GetVisiblePlayer()) {
		SetViewResponse(resp.senderId, resp.receiverId, true, &resp, &sender_response);
	}
}

void DipWizard::DisplayDiplomat(sint32 player)
{
	//Added by Martin Gühmann to display the emissary photo of recipient
	if (m_emissary_photo)
	{
		const MBCHAR *	fileName	= NULL;
		if ((player >= 0) && (player < k_MAX_PLAYERS) && g_player[player])
		{
			StringId strID;
			if(g_player[player]->GetCivilisation()->GetGender() == GENDER_MALE){
				strID = g_player[player]->GetCivilisation()->GetDBRec()->GetEmissaryPhotoMale();
			}
			else{
				strID = g_player[player]->GetCivilisation()->GetDBRec()->GetEmissaryPhotoFemale();
			}
			fileName = g_theStringDB->GetNameStr(strID);
		}

		m_emissary_photo->ExchangeImage(0,0, fileName);
	}
}

AUI_ERRCODE DrawDiplomatColor(ctp2_Static *control,
							  aui_Surface *surface,
							  RECT &rect,
							  void *cookie)
{
	sint32 pl = (sint32)cookie;
	primitives_PaintRect16(surface, &rect, g_colorSet->GetPlayerColor(pl));
	return AUI_ERRCODE_OK;
}

void DipWizard::DisplayResponseDiplomat(sint32 player)
{
	m_responseDiplomat->Show();




	m_responseDiplomat->SetDrawCallbackAndCookie(DrawDiplomatColor, (void *)player);
}

void DipWizard::DisplayParchment(sint32 player)
{
	Assert(m_parchment);
	if(m_parchment) {
		char buf[50];
		sprintf(buf, "UPDG%02d.tga", g_player[player]->m_civilisation->GetDBRec()->GetParchment());
		m_parchment->ExchangeImage(0,0, buf);
	}
}

void DipWizard::CheckIntelligence(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	sint32 pl = -1;

	switch(GetStage()) {
		case DIP_WIZ_STAGE_RECIPIENT:
		case DIP_WIZ_STAGE_EXCHANGE:
		case DIP_WIZ_STAGE_PROPOSAL:
			pl = m_recipient;
			break;
		case DIP_WIZ_STAGE_VIEW_PROPOSAL:
		case DIP_WIZ_STAGE_MAKE_THREAT:
			pl = m_viewRecipient == g_selected_item->GetVisiblePlayer() ? m_viewSender : m_viewRecipient;
			break;
	}

	if(pl < 0 || pl >k_MAX_PLAYERS || !g_player[pl])
		return;

	DiplomacyDetails::SetNation(pl);
	DiplomacyDetails::Display();
}

aui_Window *DipWizard::GetWindow()
{
	if(!s_dipWizard)
		return NULL;

	return s_dipWizard->m_window;
}
