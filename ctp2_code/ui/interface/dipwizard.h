
#ifndef DIPWIZARD_H__
#define DIPWIZARD_H__

class ctp2_Window;
class ctp2_ListBox;
class ctp2_Static;
class ctp2_DropDown;
class ctp2_Button;
class aui_Control;
class DiplomacyProposalRecord;
class SlicObject;

#include "ctp2_Menu.h"

enum DIP_WIZ_STAGE {
	DIP_WIZ_STAGE_NONE = -1,
	DIP_WIZ_STAGE_RECIPIENT,
	DIP_WIZ_STAGE_PROPOSAL,
	DIP_WIZ_STAGE_EXCHANGE,
	DIP_WIZ_STAGE_VIEW_PROPOSAL,
	DIP_WIZ_STAGE_MAKE_THREAT,
	DIP_WIZ_STAGE_MAX
};

enum DIP_WIZ_PROP_TAB {
	DIP_WIZ_PROP_TAB_NONE = -1,
	DIP_WIZ_PROP_TAB_REQUEST,
	DIP_WIZ_PROP_TAB_OFFER,
	DIP_WIZ_PROP_TAB_TREATY,
	DIP_WIZ_PROP_TAB_MAX
};

enum DIP_WIZ_VIEW_TYPE {
	DIP_WIZ_VIEW_TYPE_PROPOSAL, 
	DIP_WIZ_VIEW_TYPE_RESPONSE, 
	DIP_WIZ_VIEW_TYPE_FINAL_RESPONSE, 
	DIP_WIZ_VIEW_TYPE_THREAT,   
	DIP_WIZ_VIEW_TYPE_COUNTER,  
	DIP_WIZ_VIEW_TYPE_MAX
};


#include "diplomattypes.h"

class DipWizard
{
  private:
	
	ctp2_Window *m_window;
	static ctp2_Static *m_stages[DIP_WIZ_STAGE_MAX];
	static ctp2_DropDown *m_nations;
	static ctp2_Button *m_toneButtons[DIPLOMATIC_TONE_MAX];

	static ctp2_Button *m_backButton, *m_nextButton, *m_sendButton, *m_cancelButton;
	static ctp2_Static *m_createButtons, *m_viewButtons;

	static ctp2_Static *m_parchment, *m_responseDiplomat;

	static ctp2_ListBox *m_propList[DIP_WIZ_PROP_TAB_MAX];
	static ctp2_ListBox *m_exchList[DIP_WIZ_PROP_TAB_MAX];
	static ctp2_ListBox *m_threatList;
	static ctp2_Button *m_counterOrThreatenButton;
	static ctp2_Button *m_rejectButton, *m_acceptButton;
	static ctp2_Button *m_intelButton;

	
	static ctp2_Window *m_goldRequestWindow;
	static ctp2_Window *m_pollutionRequestWindow;
	static ctp2_Window *m_percentRequestWindow;
	static ctp2_Menu *m_curMenu;
	static ctp2_Menu *m_threatMenu;
	static bool m_proposalDataPending;
	static bool m_threatDataPending;

	
	static sint32 m_recipient;
	static sint32 m_tone;
	static sint32 m_proposal;
	static sint32 m_exchange;
	static sint32 m_menuProposal, m_menuExchange;
	static sint32 m_threat, m_menuThreat;

	static DiplomacyArg m_proposalArg, m_exchangeArg, m_threatArg;

	
	static sint32 m_viewTone,
		m_viewRecipient, m_viewSender,
		m_viewProposal, m_viewExchange,
		m_viewThreat, m_viewResponseType;
	static DiplomacyArg m_viewProposalArg, m_viewExchangeArg, m_viewThreatArg;
	static DIP_WIZ_VIEW_TYPE m_viewType;

	static bool m_sendCounter;

  public:
	DipWizard(AUI_ERRCODE *err);
	~DipWizard();

	static AUI_ERRCODE Initialize();
	static AUI_ERRCODE Cleanup();
	static AUI_ERRCODE Display();
	static AUI_ERRCODE Hide();
	static bool CanInitiateRightNow();

	static void SetNation(sint32 pl);
	static void SetTone(sint32 tone);
	static void SetProposal(sint32 prop);
	static void SetExchange(sint32 exch);
	static void SetThreat(sint32 threat);

	static sint32 GetStage();
	static void SetStage(sint32 stage);
	static void SetViewResponse(sint32 sender, sint32 recipient, bool negotiationsComplete, const Response *resp = NULL, const Response *threatResp = NULL);
	static void SetViewProposal(sint32 sender, sint32 receiver);
	static void UpdateProposalStage();
	static void UpdateExchangeStage();
	static void UpdateViewProposalStage();
	static void UpdateDetails();
	static void FillInProposalData(ProposalData &prop, bool counter = false);

	static void FillProposalLists();
	static void FillRecipientLists();
	static void AddProposalItem(ctp2_ListBox *propList, const DiplomacyProposalRecord *rec);
	static const MBCHAR *GetCategoryName(DIP_WIZ_PROP_TAB tab);
	static void UpdateButtons();

	
	static void ToneButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void PropListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ExchListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void NationCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	
	static void BackCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void NextCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CancelCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void SendCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void ProposalTabCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ExchangeTabCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void AcceptCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void RejectCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CounterOrThreatenCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CheckIntelligence(aui_Control *control, uint32 action, uint32 data, void *cookie);

	
	static void ProcessMenuSelection(sint32 itemIndex, void *cookie);
	static void ProcessMenuCancel();
	static void MenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie);

	static bool ProposalContextMenu(sint32 proposal);
	static void AddCityItems(ctp2_Menu *menu, sint32 player);
	static void AddAgreementItems(ctp2_Menu *menu, sint32 player);
	static void AddAdvanceItems(ctp2_Menu *menu, sint32 sender, sint32 receiver);
	static void AddStopResearchItems(ctp2_Menu *menu, sint32 playerId);
	static bool AddThreatData(SlicObject &so, sint32 threat, const DiplomacyArg &arg);
	static void GoldOk(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void PollutionOk(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void PercentOk(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void GoldCancel(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void PollutionCancel(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void PercentCancel(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void RequestGoldValue(sint32 player);
	static void AddThirdPartyItems(ctp2_Menu *menu, sint32 sender, sint32 receiver);
	static void RequestPollutionValue(sint32 player);
	static void RequestPercentValue();

	
	static void ThreatMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie);
	static bool ThreatContextMenu(sint32 threat);
	static void ThreatListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);


	
	static void InitializeEvents();

	
	static void NotifyResponse(const Response &resp, sint32 responder, sint32 other_player);
	static void NotifyThreatRejected(const Response &resp, const Response &sender_response, sint32 responder, sint32 other_player);

	
	static void DisplayDiplomat(sint32 player);
	static void DisplayResponseDiplomat(sint32 player);
	static void DisplayParchment(sint32 player);

	static aui_Window *GetWindow();
	
};

#endif
