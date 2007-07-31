#ifndef CRITICALMESSAGESPREFS_H
#define CRITICALMESSAGESPREFS_H

#include "PointerList.h"

struct CriticalMessagesData
{
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
