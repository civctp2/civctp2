
#ifndef DIPLOMACYDETAILS_H__
#define DIPLOMACYDETAILS_H__

class ctp2_Window;
class ctp2_ListBox;
class ctp2_Static;
class ctp2_DropDown;
class ctp2_Button;
class aui_Control;
class DiplomacyProposalRecord;
class SlicObject;
class Unit;

#include "ctp2_Menu.h"


#include "diplomattypes.h"

class DiplomacyDetails
{
  private:
	
	ctp2_Window *m_window;
	static sint32 detailPlayer;

	static ctp2_Button *m_cancelButton;
	static ctp2_ListBox *sm_list;
	static aui_StringTable *sm_strengthImages;
	static aui_StringTable *sm_embassyImages;

  public:
	DiplomacyDetails(AUI_ERRCODE *err);
	~DiplomacyDetails();

	static AUI_ERRCODE Initialize();
	static AUI_ERRCODE Cleanup();
	static AUI_ERRCODE Display(Unit *c=NULL);
	static AUI_ERRCODE Hide();

	static void SetNation(sint32 player);

	static void CancelCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void SelectItem(aui_Control *control, uint32 action, uint32 data, void *cookie);
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
