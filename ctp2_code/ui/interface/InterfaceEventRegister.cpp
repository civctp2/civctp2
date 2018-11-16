#include "c3.h"

#include "InterfaceEventRegister.h"

#include "GameEventManager.h"






bool InterfaceEventRegister::AddCallback(GAME_EVENT type,
										 GAME_EVENT_PRIORITY priority,
										 GameEventHookCallback *callback)
{

	if(m_isInitialized()) {

		g_gevManager->AddCallback(type, priority, callback);
	} else {

		m_interfaceCallbackList().push(EventInfo(type, priority, callback));
	}

	return(m_isInitialized());
}

void InterfaceEventRegister::Initialize()
{

	while(!m_interfaceCallbackList().empty()) {

		const EventInfo &eventInfo = m_interfaceCallbackList().front();
		g_gevManager->AddCallback(eventInfo.m_type, eventInfo.m_priority,
			eventInfo.m_callback);

		m_interfaceCallbackList().pop();
	}

	m_isInitialized() = true;
}

void InterfaceEventRegister::Cleanup()
{

	m_isInitialized() = false;
}


bool &InterfaceEventRegister::m_isInitialized()
{

	static bool isInitialized = false;

	return(isInitialized);
}

std::queue<InterfaceEventRegister::EventInfo>
&InterfaceEventRegister::m_interfaceCallbackList()
{

	static std::queue<EventInfo> interfaceCallbackList;

	return(interfaceCallbackList);
}
