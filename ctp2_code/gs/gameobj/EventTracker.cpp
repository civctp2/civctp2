#include "c3.h"
#include "GameEventHook.h"
#include "Events.h"
#include "EventTracker.h"
#include "Unit.h"
#include "GameEventArgList.h"
#include "GameEventManager.h"
#include "newturncount.h"
#include "UnitData.h"

EventTracker::EventTracker()
{
	m_dataList=new PointerList<EventData>;
}

EventTracker::EventTracker(CivArchive &archive)
{
	m_dataList=new PointerList<EventData>;
	Serialize(archive);
}

EventTracker::~EventTracker()
{
	m_dataList->DeleteAll();
	delete m_dataList;
}

void EventTracker::AddEvent(EVENT_TYPE type, sint32 playerNum, sint32 turn, sint32 dbIndex)
{
	EventData *newdata=new EventData;
	newdata->m_type=type;
	newdata->m_playerNum=playerNum;
	newdata->m_turn=turn;
	newdata->m_dbIndex=dbIndex;
	m_dataList->AddTail(newdata);
}

void EventTracker::ResetList()
{
}

EventData *EventTracker::GetEvents(BOOL Reset)
{
	static PointerList<EventData>::PointerListNode *curDataPtr;
	if(Reset)
	{
		curDataPtr=m_dataList->GetHeadNode();
	}
	else if(curDataPtr)
	{
		curDataPtr = curDataPtr->GetNext();
	}

	if(curDataPtr)
	{
		return curDataPtr->GetObj();
	}
	else
	{
		return NULL;
	}
}
		

void EventTracker::Serialize(CivArchive &archive)
{
	sint32 i;
	if(archive.IsStoring()) 
	{
		archive << m_dataList->GetCount();
		PointerList<EventData>::PointerListNode *curDataPtr;
		curDataPtr=m_dataList->GetHeadNode();
		while(curDataPtr)
		{
			archive << (sint32)curDataPtr->GetObj()->m_type;
			archive << curDataPtr->GetObj()->m_playerNum;
			archive << curDataPtr->GetObj()->m_turn;
			archive << curDataPtr->GetObj()->m_dbIndex;
			curDataPtr = curDataPtr->GetNext();
		}
	}
	else
	{
		sint32 Items;
		sint32 type,playerNum,turn,dbIndex;
		archive >> Items;
		for(i=0; i<Items; i++)
		{
			archive >> type;
			archive >> playerNum;
			archive >> turn;
			archive >> dbIndex;
			AddEvent((EVENT_TYPE)type,playerNum,turn,dbIndex);
		}
	}
}

int EventTracker::GetEventCount()
{
	return m_dataList->GetCount();
}


STDEHANDLER(TrackCreateWonderEvent)
{
	Unit c;
	sint32 wonder;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetInt(0, wonder)) return GEV_HD_Continue;

	g_eventTracker->AddEvent(EVENT_TYPE_WONDER,c->GetOwner(),NewTurnCount::GetCurrentRound(),wonder);

	return GEV_HD_Continue;
}

void trackerevent_Initialize()
{
	g_gevManager->AddCallback(GEV_CreateWonder, GEV_PRI_Post, &s_TrackCreateWonderEvent);
}

void trackerevent_Cleanup()
{
}
