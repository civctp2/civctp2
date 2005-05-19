//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Critical messages preferences
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
// - Repaired memory leak.
//
//----------------------------------------------------------------------------

#ifndef CRITICALMESSAGESPREFS_H
#define CRITICALMESSAGESPREFS_H

#include "pointerlist.h"

struct CriticalMessagesData
{
	CriticalMessagesData();
	virtual ~CriticalMessagesData();

	MBCHAR *m_messageName;
	bool m_messageEnabled;
};

class CriticalMessagesPrefs 
{
public:
	CriticalMessagesPrefs();
	~CriticalMessagesPrefs();

	
	sint32 IsEnabled(const char *name);

	
	void SetEnabled(const char *name, bool enable);

	
	void EnableAll();

	
	void Load();

	
	void Save();

private:
	PointerList<CriticalMessagesData>::PointerListNode *FindMessage(const char *name);
	PointerList<CriticalMessagesData> *m_messagesList;
};

extern CriticalMessagesPrefs *g_theCriticalMessagesPrefs;

#endif 
