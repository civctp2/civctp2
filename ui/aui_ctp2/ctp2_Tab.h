






#pragma once
#ifndef CTP2_TAB_H__
#define CTP2_TAB_H__


#include "aui_Control.h"


class ctp2_TabButton;
class ctp2_Static;
class ctp2_TabGroup;


class ctp2_Tab : public aui_Control {
public:
	
	enum ACTION
	{
		ACTION_FIRST = 0,
		ACTION_NULL = 0,
		ACTION_ACTIVATED,
		ACTION_DEACTIVATED,
		ACTION_LAST
	};

	
	ctp2_Tab(AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL, void *cookie = NULL);

	
	virtual ~ctp2_Tab();

	
	virtual AUI_ERRCODE DoneInstantiatingThis(const MBCHAR *ldlBlock);

	
	void Activate();

	
	void Deactivate();

	
	bool IsPanelActive();

	
	void HidePanel();

	ctp2_TabButton *GetButton() { return m_tabButton; }
	ctp2_Static *GetPanel() { return m_tabPanel; }

private:
	
	static void TabButtonActionCallback(aui_Control *control, uint32 action,
		uint32 data, void *cookie);

	
	ctp2_TabGroup *m_tabGroup;

	
	ctp2_TabButton *m_tabButton;

	
	ctp2_Static *m_tabPanel;

	
	
	friend ctp2_TabGroup;
};

#endif 
