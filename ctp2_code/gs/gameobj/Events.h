
#pragma once
#ifndef __EVENTS_H__
#define __EVENTS_H__

#define EHANDLER(name) class name : public GameEventHookCallback
#define STDEHANDLER(name) static class name : public GameEventHookCallback {\
	GAME_EVENT_HOOK_DISPOSITION GEVHookCallback(GAME_EVENT gameEventType, \
												GameEventArgList *args);\
    void GetDescription(char *str, sint32 maxsize) { \
        sprintf(str, "%s: %s@%d", #name, __FILE__, __LINE__); \
	} \
} s_##name;\
GAME_EVENT_HOOK_DISPOSITION name::GEVHookCallback(GAME_EVENT gameEventType,\
												  GameEventArgList *args)
void events_Initialize();
void events_Cleanup();

#endif
