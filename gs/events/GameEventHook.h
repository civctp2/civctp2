
#pragma once
#ifndef __GAME_EVENT_HOOKS_H__
#define __GAME_EVENT_HOOKS_H__

#include "PointerList.h"
#include "GameEventTypes.h"

class GameEventArgList;


class GameEventHookCallback {
public:
	virtual GAME_EVENT_HOOK_DISPOSITION GEVHookCallback(GAME_EVENT type, GameEventArgList *args) = 0;
	virtual void GetDescription(char *str, sint32 maxsize) { 
		strncpy(str, "An undescribed callback", maxsize);
	}
};

class GameEventHookNode {
public:
	GameEventHookNode(GameEventHookCallback *cb, GAME_EVENT_PRIORITY pri) {
		m_cb = cb;
		m_priority = pri;
	}

	GameEventHookCallback *m_cb;
	GAME_EVENT_PRIORITY m_priority;
};
	
class GameEventHook {
public:
	GameEventHook(GAME_EVENT type);
	~GameEventHook();

	void AddCallback(GameEventHookCallback *cb, GAME_EVENT_PRIORITY pri);
	void RemoveCallback(GameEventHookCallback *cb);

	GAME_EVENT_ERR Activate(GameEventArgList *args, sint32 &resumeIndex);
	GAME_EVENT_ERR Resume(GameEventArgList *args, sint32 startHookIndex, sint32 &resumeIndex);
	GAME_EVENT_ERR Run(PointerList<GameEventHookNode>::Walker *walk,
					   GameEventArgList *args,
					   sint32 &resumeIndex);


#ifdef _DEBUG
	void Dump(FILE *f);
#endif

private:
	GAME_EVENT m_type;
	PointerList<GameEventHookNode> *m_callbacks;
};


#endif
