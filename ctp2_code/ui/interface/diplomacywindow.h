#ifndef DIPLOMACY_WINDOW_H__
#define DIPLOMACY_WINDOW_H__

class ctp2_Window;
class aui_Control;
class ctp2_ListBox;
class ctp2_Static;
class aui_Surface;
class ctp2_Menu;
class SlicObject;
class ctp2_TabGroup;
class ctp2_Tab;

class ctp2_Button;

typedef sint32 StringId;

#include "diplomattypes.h"

#include "ctp2_Menu.h"

enum DW_CREATE_MODE {
	DW_CREATE_MODE_NONE,
	DW_CREATE_MODE_CIV,
	DW_CREATE_MODE_TONE,
	DW_CREATE_MODE_REQUEST,
	DW_CREATE_MODE_TREATY,
	DW_CREATE_MODE_GIFT,
};


#define k_DIPWIN_PROPOSALS_RECEIVED      0x001
#define k_DIPWIN_PROPOSALS_MADE          0x002
#define k_DIPWIN_CREATE_PROPOSAL         0x004
#define k_DIPWIN_RESPONSE                0x008
#define k_DIPWIN_PROPOSAL_DETAILS        0x010
#define k_DIPWIN_CREATE_COUNTER_PROPOSAL 0x020
#define k_DIPWIN_RESPONSE_WITH_COUNTER   0x040
#define k_DIPWIN_THREAT_LIST             0x080
#define k_DIPWIN_PROPOSAL_DETAILS_THREAT 0x100
#define k_DIPWIN_INTELLIGENCE            0x200

class DiplomacyWindow {
  private:
	ctp2_Window *m_window;
	ctp2_Window *m_goldRequestWindow;
	ctp2_Window *m_pollutionRequestWindow;
	ctp2_Window *m_percentRequestWindow;
	DW_CREATE_MODE m_create_mode;
	sint32 m_sendToCiv;
	DIPLOMATIC_TONE m_sendTone;
	ctp2_Menu *m_curMenu;
	ctp2_Menu *m_threatMenu;

	ctp2_TabGroup *m_tabGroup;

	static ctp2_Button	*sm_detailsButton;
	static ctp2_Button	*sm_warButton;
	static ctp2_Button	*sm_embargoButton;
	static ctp2_Button	*sm_messageButton;

	bool m_getRequest;
	sint32 m_sendProposal;
	DiplomacyArg m_proposalArg;
	sint32 m_sendExchange;
	DiplomacyArg m_exchangeArg;
	bool m_sendCounter;

	sint32 m_viewResponseSender, m_viewResponseReceiver;

	sint32 m_sendThreat;
	DiplomacyArg m_threatArg;

	static char *sm_toneIcons[DIPLOMATIC_TONE_MAX];
	bool m_selectingProgramatically;

  public:
	DiplomacyWindow(AUI_ERRCODE *err);
	~DiplomacyWindow();

	static AUI_ERRCODE Initialize();
	static AUI_ERRCODE Cleanup();
	static AUI_ERRCODE Display();
	static AUI_ERRCODE Hide();

	static void GotoTab(sint32 tab);

	static bool IsShown();

	void ShowInitialSections();
	void ShowSection(MBCHAR *section, bool show);

	void ShowSections(uint32 sections);

	void Update();
	void UpdateProposalList(ctp2_ListBox *propList, bool toPlayer);
	static void GetProposalDetails(const ProposalData &prop, sint32 sender, sint32 receiver, MBCHAR *finalText, MBCHAR *exchangeText, bool isCounter, const Response *);
	void GetProposalSummary(const ProposalData &prop, sint32 sender, sint32 receiver, MBCHAR *finalText, bool isCounter, const Response *);
	void UpdateProposalsReceived();
	void UpdateProposalsMade();
	void UpdateProposalDetails();
	void UpdateCreateList();
	void UpdateSendProposalDetails();
	void SetViewingResponse(sint32 sender, sint32 receiver);
	void UpdateResponseDetails();
	void UpdateThreatList();

	static StringId GetProposalText(sint32 proposal, DIPLOMATIC_TONE tone, bool isExchange, bool useReciprocal);
	StringId GetProposalSummaryText(sint32 proposal, DIPLOMATIC_TONE tone, bool isExchange, bool useReciprocal);
	static bool AddProposalData(SlicObject &so, sint32 proposal, DiplomacyArg arg);
	static bool AddThreatData(SlicObject &so, sint32 threat, const DiplomacyArg &arg);

	static void Close(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ProposalsReceived(aui_Control *control, uint32 action, uint32 data, void *cookie);
	void ShowReceivedProposalDetails();
	static void Respond(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Accept(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Reject(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Counter(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Create(aui_Control *control, uint32 action, uint32 data, void *cookie);
	void SendProposal();
	void SendCounter();
	static void Send(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void Civ(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static AUI_ERRCODE DrawCivColor(ctp2_Static *control, aui_Surface *surface, RECT &rect, void *cookie );
	static void Tone(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Request(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Treaty(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Gift(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Exchange(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CreateList(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ClearForm(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ProposalsMade(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ResponseOK(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ThreatenCounter(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void AcceptCounter(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void RejectCounter(aui_Control *control, uint32 action, uint32 data, void *cookie);

	bool ThreatContextMenu(sint32 threat);
	static void ThreatList(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ThreatMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie);
	static void MakeThreat(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CancelThreat(aui_Control *control, uint32 action, uint32 data, void *cookie);
	void SendThreat();

	void Respond(RESPONSE_TYPE disposition);

	static void InitializeEvents();
	static void CleanupEvents();
	void AddCityItems(ctp2_Menu *menu, sint32 player);
	void AddAdvanceItems(ctp2_Menu *menu, sint32 sender, sint32 receiver);
	void RequestGoldValue(sint32 player);
	void AddThirdPartyItems(ctp2_Menu *menu, sint32 sender, sint32 receiver);
	void RequestPollutionValue(sint32 player);
	void RequestPercentValue();

	bool ProposalContextMenu(sint32 proposal);
	void ProcessMenuSelection(sint32 itemIndex, void *cookie);
	void ProcessMenuCancel();
	static void MenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie);

	static void GoldSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void GoldOk(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void GoldCancel(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void PollutionOk(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void PollutionCancel(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void PercentOk(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void PercentCancel(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void Intelligence(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void DeclareWar(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void DeclareEmbargo(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void SendMessage(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Details(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void TabPanelActionCallback(aui_Control *control, uint32 action,
		uint32 data, void *cookie);

	static char **GetToneIcons() { return sm_toneIcons; }

	void SetSendProposal(sint32 prop);
	void EnableRequests(bool enable);

	void SetCreateMode(DW_CREATE_MODE mode);

	static void EnableButtons(BOOL enable, sint32 player = 0);
};

#endif
