

#pragma once

#ifndef __SPNEWGAMERULESSCREEN_H__
#define __SPNEWGAMERULESSCREEN_H__

enum RULE {
	RULE_GENOCIDE,
	RULE_POLLUTION,
	RULE_TRADE,
	RULE_INVULNERABLE_TRADE,
	RULE_LINE_OF_SIGHT,
//emod new rules
	RULE_CaptureCity,
	RULE_NoAIProductionDeficit,		
	RULE_NoAIGoldDeficit,					
	RULE_AICityDefenderBonus,					
	RULE_BarbarianCities,					
	RULE_SectarianHappiness,
	RULE_RevoltCasualties,					
	RULE_RevoltInsurgents,					
	RULE_BarbarianCamps,			
	RULE_BarbarianSpawnsBarbarian,			
	RULE_AINoSinking,							
	RULE_AINoCityLimit,						
	RULE_GoldPerUnitSupport,
	RULE_GoldPerCity,							
	RULE_AINoShieldHunger,					
	RULE_AINoGoldHunger,						
	RULE_AIFreeUpgrade,						
	RULE_AIMilitiaUnit,
	RULE_DebugAI
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