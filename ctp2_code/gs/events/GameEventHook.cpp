
#include "c3.h"

#include "GameEventHook.h"
#include "PointerList.h"
#include "GameEventManager.h"

GameEventHook::GameEventHook(GAME_EVENT type)
{
	m_type = type;
	m_callbacks = new PointerList<GameEventHookNode>;
}

GameEventHook::~GameEventHook()
{
	if(m_callbacks) {
		m_callbacks->DeleteAll();
		delete m_callbacks;
	}
}

void GameEventHook::AddCallback(GameEventHookCallback *cb,
								GAME_EVENT_PRIORITY pri)
{
	if(!m_callbacks->GetHead()) {
		m_callbacks->AddTail(new GameEventHookNode(cb, pri));
		return;
	}

	PointerList<GameEventHookNode>::Walker walk(m_callbacks);

	
	
	while(walk.IsValid() && walk.GetObj()->m_priority < pri) {
		walk.Next();
	}
	if(walk.IsValid()) {
		walk.Insert(new GameEventHookNode(cb, pri));
	} else {
		m_callbacks->AddTail(new GameEventHookNode(cb, pri));
	}
}

void GameEventHook::RemoveCallback(GameEventHookCallback *cb)
{
	PointerList<GameEventHookNode>::Walker walk(m_callbacks);
	while(walk.IsValid()) {
		if(walk.GetObj()->m_cb == cb)
			walk.Remove();
		else
			walk.Next();
	}
}

								   
GAME_EVENT_ERR GameEventHook::Activate(GameEventArgList *args, sint32 &resumeIndex)
{
	resumeIndex = 0;
	PointerList<GameEventHookNode>::Walker walk(m_callbacks);

	return Run(&walk, args, resumeIndex);
}

GAME_EVENT_ERR GameEventHook::Resume(GameEventArgList *args, sint32 startHookIndex, sint32 &resumeIndex)
{
	PointerList<GameEventHookNode>::Walker walk(m_callbacks);
	sint32 i;
	for(i = 0; i < startHookIndex; i++) {
		walk.Next();
	}
	resumeIndex = startHookIndex;
	return Run(&walk, args, resumeIndex);
}

GAME_EVENT_ERR GameEventHook::Run(PointerList<GameEventHookNode>::Walker *walk,
								  GameEventArgList *args,
								  sint32 &resumeIndex)
{
	
	while(walk->IsValid()) {
		GAME_EVENT_HOOK_DISPOSITION disp;
#ifdef _DEBUG
		{
			char desc[1024];
			walk->GetObj()->m_cb->GetDescription(desc, 1024);
			EVENTLOG(("  %s\n", desc));
		}
#endif
		disp = walk->GetObj()->m_cb->GEVHookCallback(m_type, args);

		resumeIndex++;
		walk->Next();

		switch(disp) {
			case GEV_HD_Continue:				
				break;
			case GEV_HD_Stop:
				return GEV_ERR_StopProcessing;
			case GEV_HD_NeedUserInput:
				return GEV_ERR_NeedUserInput;
			default:
				Assert(FALSE);
		}
	}
	return GEV_ERR_OK;
}

#ifdef _DEBUG
void GameEventHook::Dump(FILE *f)
{
	if(!m_callbacks->GetHead()) {
		fprintf(f, "  <empty>\n");
		return;
	}

	PointerList<GameEventHookNode>::Walker walk(m_callbacks);
	while(walk.IsValid()) {
		char str[1024];		
		walk.GetObj()->m_cb->GetDescription(str, 1024);
		fprintf(f, "  %s\n", str);
		walk.Next();
	}
}
#endif
