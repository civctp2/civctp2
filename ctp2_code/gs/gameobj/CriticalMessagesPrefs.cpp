//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - Memory leak repaired.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3files.h"

#include "CriticalMessagesPrefs.h"

//----------------------------------------------------------------------------
//
// Name       : CriticalMessagesData::CriticalMessagesData
//
// Description: Constructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
CriticalMessagesData::CriticalMessagesData()
:	m_messageName(NULL),
	m_messageEnabled(false)
{};

//----------------------------------------------------------------------------
//
// Name       : CriticalMessagesData::~CriticalMessagesData
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : - 
//
//----------------------------------------------------------------------------
CriticalMessagesData::~CriticalMessagesData()
{
	delete [] m_messageName;	// new [] in CriticalMessagesPrefs::SetEnabled 
};


CriticalMessagesPrefs::CriticalMessagesPrefs()
{
	 m_messagesList=new PointerList<CriticalMessagesData>;
	 Load();
}

CriticalMessagesPrefs::~CriticalMessagesPrefs()
{

	m_messagesList->DeleteAll();
	delete m_messagesList;
}

sint32 CriticalMessagesPrefs::IsEnabled(const char *name)
{
	PointerList<CriticalMessagesData>::PointerListNode *msg;
	msg=FindMessage(name);
	if(msg)
	{
		return (msg->GetObj()->m_messageEnabled);
	}
	else
	{
		return -1;
	}
}

void CriticalMessagesPrefs::SetEnabled(const char *name, bool enable)
{
	PointerList<CriticalMessagesData>::PointerListNode *curDataPtr;
	curDataPtr=FindMessage(name);
	if(curDataPtr)
	{
		if(curDataPtr->GetObj()->m_messageEnabled!=enable)
		{
			curDataPtr->GetObj()->m_messageEnabled=enable;
			Save();
		}
	}
	else
	{
		CriticalMessagesData *newdata=new CriticalMessagesData;
		newdata->m_messageName=new MBCHAR[strlen(name)+1];
		strcpy(newdata->m_messageName,name);
		newdata->m_messageEnabled=enable;
		m_messagesList->AddTail(newdata);
		Save();
	}
}

void CriticalMessagesPrefs::EnableAll()
{
	PointerList<CriticalMessagesData>::Walker walk(m_messagesList);
	for(; walk.IsValid(); walk.Next()) {
		walk.GetObj()->m_messageEnabled = true;
	}

	Save();
}

void CriticalMessagesPrefs::Load()
{
	FILE *filePtr;
	MBCHAR inputStr[100];
	bool value;
	filePtr=c3files_fopen(C3DIR_DIRECT,"usercritmsgs.txt","rt");
	if(!filePtr) {
		filePtr=c3files_fopen(C3DIR_UIDATA, "CritMsgs.txt", "rt");
		if(!filePtr)
			return;
	}

	while(!c3files_feof(filePtr))
	{
		c3files_fgets(inputStr,100,filePtr);
		if(*inputStr)
		{
			if(strchr(inputStr,':'))
			{
				value=(atoi(strchr(inputStr,':')+1)>0);
				*strchr(inputStr,':')=0;
				SetEnabled(inputStr,value);
			}
		}
	}
	c3files_fclose(filePtr);
}

void CriticalMessagesPrefs::Save()
{
	FILE *filePtr;
	filePtr=c3files_fopen(C3DIR_DIRECT,"usercritmsgs.txt","wt");
	if(!filePtr)
		return;

	PointerList<CriticalMessagesData>::PointerListNode *curDataPtr;
	if(m_messagesList && m_messagesList->GetCount())
	{
		curDataPtr=m_messagesList->GetHeadNode();
		while(curDataPtr)
		{
			fprintf(filePtr,"%s:%i\n",curDataPtr->GetObj()->m_messageName,
				curDataPtr->GetObj()->m_messageEnabled);
			curDataPtr = curDataPtr->GetNext();
		}

	}
	c3files_fclose(filePtr);
}

PointerList<CriticalMessagesData>::PointerListNode *CriticalMessagesPrefs::FindMessage(const char *name)
{
	PointerList<CriticalMessagesData>::PointerListNode *curDataPtr;
	if(m_messagesList && m_messagesList->GetCount())
	{
		curDataPtr=m_messagesList->GetHeadNode();
		while(curDataPtr)
		{
			if(stricmp(name, curDataPtr->GetObj()->m_messageName)==0)
			{
				return curDataPtr;
			}
			curDataPtr = curDataPtr->GetNext();
		}

	}
	return NULL;
}
