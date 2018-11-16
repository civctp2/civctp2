#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef ___BMH_END_TURN_BUTTON_HEADER
#define ___BMH_END_TURN_BUTTON_HEADER

#include "Globals.h"
#include "player.h" // PLAYER_INDEX

class aui_Control;
class ctp2_Button;

class EndTurnButton {
public:

	EndTurnButton(const MBCHAR *ldlBlock);


	void UpdatePlayer(PLAYER_INDEX player);

private:

	static void EndTurnButtonActionCallback(aui_Control *control, uint32 action,
		uint32 data, void *cookie);

	ctp2_Button *m_endTurn;
};

#endif
