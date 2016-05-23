//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The Status Bar
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added CleanUp function for m_text, by Martin Gühmann.
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
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

	static void CleanUp(){
		if(m_text){
			delete m_text;
			m_text = NULL;
		}
	}

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
