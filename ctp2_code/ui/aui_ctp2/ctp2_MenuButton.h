
#ifndef CTP2_MENUBUTTON_H__
#define CTP2_MENUBUTTON_H__

#include "c3_button.h"

class ctp2_Menu;

class ctp2_MenuButton : public c3_Button
{
  public:
	ctp2_MenuButton(AUI_ERRCODE *retval,
					uint32 id,
					MBCHAR *ldlBlock,
					ControlActionCallback *ActionFunc = NULL,
					void *cookie = NULL );

	
	virtual ~ctp2_MenuButton();
	virtual BOOL IsThisA(uint32 classId)
	{
		return classId == m_menuButtonClassId ||
			aui_Control::IsThisA(classId);
	}
	static uint32 m_menuButtonClassId;

	void SetMenu(ctp2_Menu *menu) { m_menu = menu; }
	void SetLeftNeighbor(ctp2_MenuButton *butt) { m_leftNeighbor = butt; }
	void SetRightNeighbor(ctp2_MenuButton *butt) { m_rightNeighbor = butt; }

	ctp2_Menu *GetMenu() { return m_menu; }


  protected:
	ctp2_MenuButton() : c3_Button() {}
	
	virtual MouseEventCallback MouseLGrabInside;
	virtual MouseEventCallback MouseLDragAway;

	friend class ctp2_ListBox;

	ctp2_Menu *m_menu;
	ctp2_MenuButton *m_rightNeighbor, *m_leftNeighbor;
};

#endif
