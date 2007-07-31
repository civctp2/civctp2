
#include "c3.h"
#include "combatevent.h"
#include "Events.h"
#include "CTP2Combat.h"
#include "GameEventUser.h"
#include "Army.h"
#include "MapPoint.h"
#include "World.h"
#include "Cell.h"
#include "CellUnitList.h"
#include "ArmyData.h"
#include "director.h"
#include "battleviewwindow.h"
#include "c3ui.h"
#include "aui_button.h"
extern C3UI *g_c3ui;



STDEHANDLER(RunCombatEvent)
{
	
	Army a;
	MapPoint p;
	sint32 attacker, defender;

	
	if(!args->GetArmy(0, a))
		return(GEV_HD_Continue);
	if(!args->GetPos(0, p))
		return(GEV_HD_Continue);
	if(!args->GetPlayer(0, attacker))
		return(GEV_HD_Continue);
	if(!args->GetPlayer(1, defender))
		return(GEV_HD_Continue);

	
	Assert(g_theCurrentBattle);
	if(!g_theCurrentBattle)
		return(GEV_HD_Continue);

	bool playAnimations = false;

	
	Assert(!g_theCurrentBattle->IsDone());
	if(!g_theCurrentBattle->IsDone()) {

		
		playAnimations = g_theCurrentBattle->ResolveOneRound();
	}

	
	if(g_theCurrentBattle->IsDone()) {
		
		
		
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_BattleAftermath,
							   GEA_Army, a, GEA_MapPoint, p, GEA_Unit, a[0],
							   GEA_Unit, g_theWorld->GetCell(p)->AccessUnit(0),
							   GEA_Player, attacker, GEA_Player, defender,
							   GEA_Int, 1, 
							   GEA_End);

		g_theCurrentBattle->KillUnits();

		g_director->DecrementPendingGameActions();
	} else {	
		
		
		
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_RunCombat,
							   GEA_Army, a, GEA_MapPoint, p,
							   GEA_Player, attacker, GEA_Player, defender,
							   GEA_End);
	}

	
	
	
	if(g_theCurrentBattle->GetBattle() && playAnimations)
		return(GEV_HD_NeedUserInput);

	
	return(GEV_HD_Continue);
}

STDEHANDLER(StartCombatEvent)
{
	Army a;
	MapPoint p;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, p)) return GEV_HD_Continue;

	Assert(!g_theCurrentBattle);

	if(g_theCurrentBattle) {
		if(g_battleViewWindow && g_c3ui->GetWindow(g_battleViewWindow->Id())) {
			battleview_ExitButtonActionCallback(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);
		}
		g_theCurrentBattle->ClearBattle();
		delete g_theCurrentBattle;
		g_theCurrentBattle = NULL;
	}

	if(!g_theCurrentBattle) {
		static CellUnitList defender;
		defender.Clear();
		g_theWorld->GetCell(p)->GetArmy(defender);
		
		Assert(a.Num() > 0);
		Assert(defender.Num() > 0);
		if(a.Num() > 0 && defender.Num() > 0 &&
		   a.GetOwner() != defender.GetOwner()) {
			g_theCurrentBattle = new CTP2Combat(k_COMBAT_WIDTH, k_COMBAT_HEIGHT, *a.AccessData(), defender);
		}
	}
	return GEV_HD_Continue;
}

void combatevent_Initialize()
{
	g_gevManager->AddCallback(GEV_RunCombat, GEV_PRI_Primary, &s_RunCombatEvent);
	g_gevManager->AddCallback(GEV_StartCombat, GEV_PRI_Primary, &s_StartCombatEvent);
}

void combatevent_Cleanup()
{
}
