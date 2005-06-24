#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SPNEWGAMERULESSCREEN_H__
#define __SPNEWGAMERULESSCREEN_H__

enum RULE {
	RULE_GENOCIDE,
	RULE_POLLUTION,
	RULE_TRADE,
	RULE_INVULNERABLE_TRADE,
	RULE_LINE_OF_SIGHT
};

sint32 spnewgamerulesscreen_displayMyWindow();
sint32 spnewgamerulesscreen_removeMyWindow(uint32 action);
AUI_ERRCODE spnewgamerulesscreen_Initialize( void );
AUI_ERRCODE spnewgamerulesscreen_Cleanup();

void spnewgamerulesscreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamerulesscreen_cancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamerulesscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamerulesscreen_switchPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

#endif
