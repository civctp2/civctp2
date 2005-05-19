






#pragma once
#ifndef CTP2_TABGROUP_H__
#define CTP2_TABGROUP_H__


#include "aui_control.h"


class ctp2_Tab;


class ctp2_TabGroup : public aui_Control {
public:
	
	ctp2_TabGroup(AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL, void *cookie = NULL);

	
	virtual ~ctp2_TabGroup();

	
	virtual AUI_ERRCODE DoneInstantiatingThis(const MBCHAR *ldlBlock);

	
	virtual AUI_ERRCODE	Show();

	
	
	virtual AUI_ERRCODE Draw(aui_Surface *surface, sint32 x, sint32 y);

	
	void SelectTab(ctp2_Tab *tab);

	ctp2_Tab *GetCurrentTab() {return m_currentTab;}

private:
	
	ctp2_Tab *m_currentTab;
};

#endif 
