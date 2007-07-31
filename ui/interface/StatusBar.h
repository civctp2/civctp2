






#pragma once
#ifndef ___BMH_STATUS_BAR_HEADER
#define ___BMH_STATUS_BAR_HEADER


#include <list>


class aui_Control;
class ctp2_Static;


class StatusBar {
public:
	
	static void SetText(const MBCHAR *text, const aui_Control *owner = NULL);

	static const aui_Control *GetOwner() { return m_owner; }

	
	StatusBar(MBCHAR *ldlBlock);

	
	~StatusBar();

private:
	
	void Update();

	
	static MBCHAR *m_text;
	static sint32 m_allocatedLen;

	
	static std::list<StatusBar*> m_list;

	
	ctp2_Static	*m_statusBar;
	
	static const aui_Control *m_owner;
};

#endif 
