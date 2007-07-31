#ifndef EVENT_TRACKER_H
#define EVENT_TRACKER_H

#include "pointerlist.h"

void trackerevent_Initialize();
void trackerevent_Cleanup();

class CivArchive;

enum EVENT_TYPE
{
	EVENT_TYPE_NONE = -1,
	EVENT_TYPE_WONDER,
	EVENT_TYPE_FEAT,
	EVENT_TYPE_AGES,
	EVENT_TYPE_MAX,
};

struct EventData
{
	EVENT_TYPE m_type;
	sint32 m_playerNum;
	sint32 m_turn;
	sint32 m_dbIndex;
};

class EventTracker
{
public:
	EventTracker();
	EventTracker(CivArchive &archive);
	~EventTracker();
	void AddEvent(EVENT_TYPE type, sint32 playerNum, sint32 turn, sint32 dbIndex);
	void ResetList();
	EventData *GetEvents(BOOL Reset);
	void Serialize(CivArchive &archive);
	int GetEventCount();
public:
	PointerList<EventData> *m_dataList;
};

extern EventTracker *g_eventTracker;

#endif	