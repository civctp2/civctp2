
#ifndef __GAME_EVENT_H__
#define __GAME_EVENT_H__

#include "GameEventTypes.h"
#include "GameEventDescription.h"

class GameEventArgList;

class GameEvent {
public:
	GameEvent(GAME_EVENT type, GameEventArgList *args, sint32 serial,
			  GAME_EVENT addedDuring);
	~GameEvent();

	void SetType(GAME_EVENT type) { m_type = type; }
	GAME_EVENT GetType() { return m_type; }

	GAME_EVENT_ERR Process(BOOL &complete);
	
	sint32 GetSerial() { return m_serial; }
private:
	GAME_EVENT m_type;
	GameEventArgList *m_argList;
	sint32 m_resumeIndex;
	GAME_EVENT m_addedDuring;

	sint32 m_serial;
};
	
#endif
