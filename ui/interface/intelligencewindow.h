
#ifndef INTELLIGENCE_WINDOW_H__
#define INTELLIGENCE_WINDOW_H__

class ctp2_Window;
class ctp2_ListBox;
class ctp2_ListItem;
class ctp2_Static;
class aui_Surface;
class aui_Control;
class aui_StringTable;
struct aui_MouseEvent;

class IntelligenceWindow {
  private:
	ctp2_Window *m_window;
	ctp2_Window *m_adviceWindow;
	
	static ctp2_Window *sm_showTreatyDetail;

	static ctp2_ListBox *sm_list;

	static aui_StringTable *sm_strengthImages;
	static aui_StringTable *sm_embassyImages;

  public:
	IntelligenceWindow(AUI_ERRCODE *err);
	~IntelligenceWindow();

	static AUI_ERRCODE Initialize();
	static AUI_ERRCODE Cleanup();
	static AUI_ERRCODE Display();
	static AUI_ERRCODE Hide();

	static void SetRegardTip(MBCHAR *buf, const sint32 player, const sint32 foreigner);
	static void Update(ctp2_ListBox *list);

	static void Close(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Negotiations(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Advice(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void SelectItem(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void DeclareWarOnSelected();
	static void DeclareEmbargoOnSelected();
	static void SendMessageToSelected();
	static void DisplayDetailsOfSelected();

	
	static void UpdateAdviceText();

	static AUI_ERRCODE DrawPlayerColor(ctp2_Static *control, aui_Surface *surface,
									   RECT &rect, void *cookie);
	
	static AUI_ERRCODE DrawPlayerFlag(ctp2_Static *control, aui_Surface *surface,
									   RECT &rect, void *cookie);
	static sint32 GetRegardThreshold(sint32 ofPlayer, sint32 forPlayer);
	static AUI_ERRCODE DrawPlayerRegard(ctp2_Static *control,
										aui_Surface *surface,
										RECT &rect,
										void *cookie);
	static AUI_ERRCODE DrawPlayerStrength(ctp2_Static *control,
										  aui_Surface *surface,
										  RECT &rect,
										  void *cookie);
	static AUI_ERRCODE DrawEmbassy(ctp2_Static *control,
								   aui_Surface *surface,
								   RECT &rect,
								   void *cookie);
	static AUI_ERRCODE DrawTreaties(ctp2_Static *control,
									aui_Surface *surface,
									RECT &rect,
									void *cookie);
	
	static void ClickTreaties(ctp2_Static *control, aui_MouseEvent *mouseData, void *cookie);

	static void InitImageTables();
};

#endif

