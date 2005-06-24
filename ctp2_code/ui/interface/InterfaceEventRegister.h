#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef ___BMH_INTERFACE_EVENT_REGISTER_HEADER
#define ___BMH_INTERFACE_EVENT_REGISTER_HEADER


#include <queue>


#include "GameEventTypes.h"
#include "GameEventHook.h"





template <class attach_type>
class InterfaceEventHookCallback : public GameEventHookCallback {
public:
	
	typedef GAME_EVENT_HOOK_DISPOSITION
		(*InterfaceEventCallback)(attach_type *attachment,
		GAME_EVENT gameEventType, GameEventArgList *args);

	
	InterfaceEventHookCallback(attach_type *attachment,
		InterfaceEventCallback callback) :
		m_attachment(attachment),
		m_callback(callback)
	{}

	
	GAME_EVENT_HOOK_DISPOSITION GEVHookCallback(
		GAME_EVENT gameEventType, GameEventArgList *args)
	{ return(m_callback(m_attachment, gameEventType, args)); }

private:
	
	attach_type *m_attachment;

	
	InterfaceEventCallback m_callback;
};


class InterfaceEventRegister {
public:
	
	
	
	
	
	static bool AddCallback(GAME_EVENT type, GAME_EVENT_PRIORITY priority,
		GameEventHookCallback *callback);

	
	static void Initialize();

	
	static void Cleanup();

private:
	
	class EventInfo {
	public:
		
		EventInfo(GAME_EVENT type, GAME_EVENT_PRIORITY priority,
			GameEventHookCallback *callback)
		{ m_type = type; m_priority = priority; m_callback = callback; }

		GAME_EVENT				m_type;			
		GAME_EVENT_PRIORITY		m_priority;		
		GameEventHookCallback	*m_callback;	
	};

	
	
	static bool &m_isInitialized();

	
	static std::queue<EventInfo> &m_interfaceCallbackList();
};

#endif 
