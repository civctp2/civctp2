//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Army event handlers
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Do not generate an Assert popup when slaves revolt and take over a city.
// - Do not generate an Assert popup when an army is destroyed in an attack.
// - Added Elite and Leader Chance 6-4-2007
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - The FinishMoveEvent event now fills a transporter up to the transport
//   capacy limit even if the army to be transported has more units than the
//   transporter space, the units that do not fit on board stay at land. (25-Jan-2008 Martin Gühmann)
// - Separated the Settle event drom the Settle in City event. (19-Feb-2008 Martin Gühmann)
// - Merged finish move. (13-Aug-2008 Martin Gühmann)
// - Added an upgrade order event. (13-Sep-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "ArmyEvent.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Army.h"
#include "Order.h"
#include "ArmyData.h"
#include "MapPoint.h"
#include "World.h"
#include "Cell.h"
#include "UnitData.h"
#include "ArmyData.h"
#include "WonderRecord.h"
#include "player.h"
#include "AICause.h"
#include "ConstRecord.h"
#include "RandGen.h"
#include "UnitPool.h"
#include "director.h"
#include "MoveFlags.h"
#include "network.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "SelItem.h"
#include "UnitRecord.h"
#include "wonderutil.h"
#include "ArmyPool.h"
#include "CTP2Combat.h"
#include "net_action.h"
#include "MainControlPanel.h"
#include "gamesounds.h"
#include "soundmanager.h"
#include "Diplomat.h"
#include "CriticalMessagesPrefs.h"
#include "Barbarians.h"			// EMOD
#include "RiskRecord.h"			// Add for barb code
#include "GameSettings.h"		// EMOD
#include "GovernmentRecord.h"   // EMOD to access government data
#include "DifficultyRecord.h"   // EMOD
#include "CivilisationPool.h"
#include "Happy.h"
#include "profileDB.h"

extern ArmyPool		*g_theArmyPool;




STDEHANDLER(ArmyMoveOrderEvent)
{
	Army a;
	Path *p;
	MapPoint curPos;
	sint32 extra;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPath(0, p)) return GEV_HD_Continue;
	if(!args->GetPos(0, curPos)) return GEV_HD_Continue;
	if(!args->GetInt(0, extra)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_MOVE, p, curPos, extra);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyMoveToOrderEvent)
{
	Army a;
	WORLD_DIRECTION d;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetDirection(0, d)) return GEV_HD_Continue;

	MapPoint oldPos, newPos;
	a.GetPos(oldPos);
	if(oldPos.GetNeighborPosition(d, newPos)) {
		a->AddOrders(UNIT_ORDER_MOVE_TO, newPos);
	}
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyVictoryMoveOrderEvent)
{
	Army a;

//	g_director->DecrementPendingGameActions();

	MapPoint newPos;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, newPos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_VICTORY_MOVE, newPos);

	return GEV_HD_Continue;
}

STDEHANDLER(ArmyMovePathOrderEvent)
{
	Army army; // Changed from a to army for consistency. // Why consitency? Half of them are a and the other half of them are army in this file.
	MapPoint p;

	if(!args->GetArmy(0, army)) return GEV_HD_Continue;
	if(!args->GetPos(0, p)) return GEV_HD_Continue;
	/// @todo Finish code or remove it.
/*	// EMOD add rebase here? just set pos as p?

	bool rebase = false;
	for(i = 0; i < army.Num(); i++) {
		if(army.AccessData()->m_array[i].GetDBRec()->GetCanRebase()) {
			rebase = true;
		}
	}

	if (rebase) {  ///fix
			if(army.AccessData()->IsOccupiedByForeigner(p)) {
			if (g_theWorld->HasCity(p) || terrainutil_HasAirfield(p)) {  //add unit later?

					g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
							   GEV_FinishMove,
							   GEA_Army, army,
							   GEA_Direction, dir,
							   GEA_MapPoint, newPos,
							   GEA_Int, order,
							   GEA_End);


		return GEV_HD_Continue;

	end EMOD	*/

	g_selected_item->EnterMovePath(army.GetOwner(), army, army->RetPos(), p);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyUnloadOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	UNIT_ORDER_TYPE ord = UNIT_ORDER_UNLOAD;
	if (a.GetOwner() == g_selected_item->GetVisiblePlayer())
	{
		CellUnitList cargoToUnload;
		if (MainControlPanel::GetSelectedCargo(cargoToUnload) && !g_player[a.GetOwner()]->IsRobot())
		{
			ord = UNIT_ORDER_UNLOAD_SELECTED_STACK;
		}
	}

	a->ClearOrders();
	a->AddOrders(ord, pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmySleepOrderEvent)
{
	Army a;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_SLEEP);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyMoveUnloadOrderEvent)
{
	Army a;
	MapPoint pos;
	Path *p;
	sint32 extra;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPath(0, p)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;
	if(!args->GetInt(0, extra)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_MOVE_THEN_UNLOAD, p, pos, extra);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyEntrenchOrderEvent)
{
	Army a;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_ENTRENCH );
	return GEV_HD_Continue;

}
STDEHANDLER(ArmyDetrenchOrderEvent)
{
	Army a;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_DETRENCH );
	return GEV_HD_Continue;

}
STDEHANDLER(ArmyDisbandArmyOrderEvent)
{
	Army a;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_DISBAND);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyGroupOrderEvent)
{
	Army a;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_GROUP );
	return GEV_HD_Continue;

}

STDEHANDLER(ArmyGroupUnitOrderEvent)
{
	Army a;
	Unit u;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;

	MapPoint pos;

	a->AddOrders(UNIT_ORDER_GROUP_UNIT, NULL, pos, u.m_id);

	return GEV_HD_Continue;

}
STDEHANDLER(ArmyUngroupOrderEvent)
{
	Army a;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_UNGROUP );
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyInvestigateCityOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_INVESTIGATE_CITY , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyNullifyWallsOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_NULLIFY_WALLS , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyStealTechnologyOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_STEAL_TECHNOLOGY , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyInciteRevolutionOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_INCITE_REVOLUTION , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyAssassinateRulerOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_ASSASSINATE , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyInvestigateReadinessOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_INVESTIGATE_READINESS , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyBombardOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_BOMBARD, pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyFranchiseOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_FRANCHISE , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmySueOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_SUE , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmySueFranchiseOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_SUE_FRANCHISE);

	return GEV_HD_Continue;
}

STDEHANDLER(ArmyExpelOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_EXPEL , pos);
	
	if(g_selected_item->IsAutoCenterOn() 
	    && !g_director->TileWillBeCompletelyVisible(pos.x, pos.y)
	    && g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(pos)
	    ){ // center on expell pos if generally visible but not in current view
	    g_director->AddCenterMap(pos);
	    }
	
	if(g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(pos)){ // game sound only if expel position is visible (not in FOW)
	    g_soundManager->AddGameSound(GAMESOUNDS_ALERT);
	    }

	return GEV_HD_Continue;
}

STDEHANDLER(ArmyEstablishEmbassyOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_ESTABLISH_EMBASSY , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyThrowPartyOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_THROW_PARTY , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyAdvertiseOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_ADVERTISE , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyPlantNukeOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_PLANT_NUKE , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmySlaveRaidOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_SLAVE_RAID , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyEnslaveSettlerOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_ENSLAVE_SETTLER , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyUndergroundRailwayOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_UNDERGROUND_RAILWAY , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyInciteUprisingOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_INCITE_UPRISING , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyBioInfectOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_BIO_INFECT, pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyPlagueOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_PLAGUE, pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyNanoInfectOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_NANO_INFECT , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyConvertCityOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_CONVERT , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyReformCityOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_REFORM);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmySellIndulgencesOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_INDULGENCE , pos);

	Unit c;
	c.m_id = g_theWorld->GetCity(pos).m_id;

	return GEV_HD_Continue;
}

STDEHANDLER(ArmySoothsayOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_SOOTHSAY , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyCreateParkOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_CREATE_PARK , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyPillageOrderEvent)
{
	Army a;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_PILLAGE);
	return GEV_HD_Continue;

}
STDEHANDLER(ArmyInjoinOrderEvent)
{
	Army a;
	MapPoint pos;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_INJOIN , pos);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyPirateOrderEvent)
{
	Army a;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_INTERCEPT_TRADE);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyGetExpelledOrderEvent)
{
	Army a;
	MapPoint pos;
	sint32 player;
	sint32 victim;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;
	if(!args->GetPlayer(0, player)) return GEV_HD_Continue;

	victim = a->GetOwner();

	a->AutoAddOrdersWrongTurn(UNIT_ORDER_EXPEL_TO, NULL, pos, 0);

	SlicObject *so = new SlicObject("42UnitExpelled");
	so->AddCivilisation(player);
	so->AddRecipient(victim);
	so->AddLocation(pos);
	g_slicEngine->Execute(so);

	return GEV_HD_Continue;
}

STDEHANDLER(ArmySettleOrderEvent)
{
	Army a;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_SETTLE);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmySettleInCityOrderEvent)
{
	Army a;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_SETTLE_IN_CITY);
	return GEV_HD_Continue;
}

STDEHANDLER(BoardTransportOrderEvent)
{
	Army a;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_BOARD_TRANSPORT);
	return GEV_HD_Continue;
}

STDEHANDLER(LaunchOrderEvent)
{
	Army a;
	MapPoint reentryPos;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, reentryPos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_LAUNCH, reentryPos);
	return GEV_HD_Continue;
}

STDEHANDLER(TargetOrderEvent)
{
	Army a;
	MapPoint targetPos;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, targetPos)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_TARGET, targetPos);
	return GEV_HD_Continue;
}

STDEHANDLER(UpgradeOrderEvent)
{
	Army a;
	MapPoint targetPos;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;

	a->AddOrders(UNIT_ORDER_UPGRADE);
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyMoveEvent)
{
	Army army;
	if(!args->GetArmy(0, army)) return GEV_HD_Continue;

	WORLD_DIRECTION dir;
	if(!args->GetDirection(0, dir))	return GEV_HD_Continue;

	UNIT_ORDER_TYPE order;
	if(!args->GetInt(0, (sint32 &)order)) return GEV_HD_Continue;

	sint32 extra;
	if(!args->GetInt(1, extra)) return GEV_HD_Continue;

	MapPoint extraOrderPos;
	if(!args->GetPos(0, extraOrderPos))	return GEV_HD_Continue;

	MapPoint    oldPos;
	army.GetPos(oldPos);

	MapPoint    newPos;
	if (!oldPos.GetNeighborPosition(dir, newPos))
		return GEV_HD_Continue;

	ArmyData *  armyData    = army.AccessData();
	if (armyData->CheckSpecialUnitMove(newPos)) {
		return GEV_HD_Continue;
	}

	armyData->CheckLoadSleepingCargoFromCity();

	if (armyData->IsMovePointsEnough(newPos))
	{
		/// @todo finish this code or remove it
/*EMOD for rebasing? this just moves it to newpos maybe do pathing order? or move to? and then set newpos as endpos?
		if (order == UNIT_ORDER_MOVE_TO

// EMOD - Rebasing of units, especially aircraft - code removed trying to create a code that automatically moves a unit from a
//city to another city anywhere in the world and costing that unit 1 move.

		for (sint32 i = m_nElements - 1; i>= 0; i--) {   //for(i = 0; i < m_nElements; i++) {
			if(!m_array[i].GetDBRec()->GetCanRebase()){
				if (!IsOccupiedByForeigner(order->m_point)){
					if (g_theWorld->HasCity(order->m_point) || terrainutil_HasAirfield(order->m_point)) {  //add unit later?
						m_array[i].SetPosition(order->m_point);
						return true;
					}
				}
				return false;
			}
		}

*/		//end EMOD
		if (armyData->IsOccupiedByForeigner(newPos))
		{
			CellUnitList * defender = g_theWorld->GetCell(newPos)->UnitArmy();

			for (sint32 d = 0; d < defender->Num(); ++d)
			{
				if (defender->Access(d).Flag(k_UDF_CANT_BE_ATTACKED))
					return GEV_HD_Continue;
			}

			PLAYER_INDEX owner = army.GetOwner();
			if ((owner == PLAYER_INDEX_VANDALS) &&
				wonderutil_GetProtectFromBarbarians(g_player[defender->GetOwner()]->m_builtWonders))
			{
				return GEV_HD_Continue;
			}

			if(!armyData->CheckWasEnemyVisible(newPos))
			{
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
									   GEV_ContactMade,
									   GEA_Player, owner,
									   GEA_Player, defender->GetOwner(),
									   GEA_End);

				if(order == UNIT_ORDER_MOVE ||
				   order == UNIT_ORDER_MOVE_TO)
				{
					DPRINTF(k_DBG_GAMESTATE, ("Army 0x%lx clear orders, was not visible\n", army.m_id));
					g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
										   GEV_ClearOrders,
										   GEA_Army, army,
										   GEA_End);

					if(!extra)
						return GEV_HD_Continue;

					if (newPos != extraOrderPos)
						return GEV_HD_Continue;
				}
			}

			if (order == UNIT_ORDER_MOVE_THEN_UNLOAD)
				return GEV_HD_Continue;

			for (sint32 i = 0; i < army.Num(); i++)
			{
				if ((*armyData)[i].Flag(k_UDF_FOUGHT_THIS_TURN) ||
				    (*armyData)[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN)
				   )
				{
					return GEV_HD_Continue;
				}
			}

			Assert(!g_player[owner]->IsRobot() || Diplomat::GetDiplomat(owner).HasWarOrDesiresPreemptivelyWith(defender->GetOwner()));

			if(g_player[owner]->IsRobot() && army->CanFight(*defender) || g_player[owner]->IsHuman())
			{
				DPRINTF(k_DBG_GAMESTATE, ("Army 0x%lx gets event to attack foreigner\n", army.m_id));
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
									   GEV_FinishAttack,
									   GEA_Army, army,
									   GEA_MapPoint, newPos,
									   GEA_End);

				DPRINTF(k_DBG_GAMESTATE, ("Army 0x%lx clears current oders via event before attacking foreigner\n", army.m_id));
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
									   GEV_ClearOrders,
									   GEA_Army, army,
									   GEA_End);
			}
			else
			{
				DPRINTF(k_DBG_GAMESTATE, ("Army 0x%lx unloads for attacking foreigner\n", army.m_id));
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
									   GEV_UnloadOrder,
									   GEA_Army, army,
									   GEA_MapPoint, newPos,
									   GEA_End);
			}
		}
		else
		{
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
							       GEV_FinishMove,
							       GEA_Army, army,
							       GEA_Direction, dir,
							       GEA_MapPoint, newPos,
							       GEA_Int, order,
							       GEA_End);
		}
	}
	else
	{
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
							   GEV_CantMoveYet,
							   GEA_Army, army,
							   GEA_Direction, dir,
							   GEA_MapPoint, newPos,
							   GEA_End);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(ClearOrdersEvent)
{
	Army army;
	if (args->GetArmy(0, army))
	{
		army.ClearOrders();
	}

	return GEV_HD_Continue;
}

STDEHANDLER(FinishAttackEvent)
{
	Army        a;
	MapPoint    pos;
	if (    args->GetArmy(0, a)     // May return false when the whole army
	                                // has been destroyed during the attack.
	     && args->GetPos(0, pos)
	   )
	{
		a.AddOrders(UNIT_ORDER_FINISH_ATTACK, NULL, pos, 0);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(FinishMoveEvent)
{
	Army army;
	WORLD_DIRECTION dir;
	MapPoint pos;
	sint32 order;

	if(!args->GetArmy(0, army))
		return GEV_HD_Continue;

	if(!args->GetDirection(0, dir))
		return GEV_HD_Continue;

	if(!args->GetPos(0, pos))
		return GEV_HD_Continue;

	if(!args->GetInt(0, order))
		return GEV_HD_Continue;

	army->FinishMove(dir, pos, (UNIT_ORDER_TYPE)order);

	return GEV_HD_Continue;
}

STDEHANDLER(MoveIntoTransportEvent)
{
	Army a;
	MapPoint pos;

	if(!args->GetArmy(0, a))
	{
		Assert( g_theArmyPool->IsValid(a) );
		return GEV_HD_Continue;
	}
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	CellUnitList transports;
	sint32 canMoveIntoTransport = a.AccessData()->NumUnitsCanMoveIntoTransport(pos, transports);

	if(a.AccessData()->Num() <= canMoveIntoTransport)
	{
		a.AccessData()->MoveIntoTransport(pos, transports);
	}
	// @ToDo merge this with the above when also the settler escord can find the goal without a settler
	else if(canMoveIntoTransport > 0)
	{
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
							   GEV_MoveIntoTransport,
							   GEA_Army, a,
							   GEA_MapPoint, pos,
							   GEA_End);

		a->RemainNumUnits(canMoveIntoTransport);
	}
	else if(g_selected_item->GetVisiblePlayer() == a.GetOwner()) // center and sound only if army belongs to visible player
	{
	if(g_selected_item->IsAutoCenterOn() 
	    && !g_director->TileWillBeCompletelyVisible(pos.x, pos.y)
	    && g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(pos)
	    ){ // center on expell pos if generally visible but not in current view
	    g_director->AddCenterMap(pos);
	    }
	
	if(g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(pos)){ // game sound only if position is visible (not in FOW)
	    g_soundManager->AddGameSound(GAMESOUNDS_ILLEGAL_MOVE);
	    }
	}

	return GEV_HD_Continue;
}

STDEHANDLER(BattleEvent)
{
	Army army;
	MapPoint pos;

	if(!args->GetArmy(0, army))
		return GEV_HD_Continue;

	if(!args->GetPos(0, pos))
		return GEV_HD_Continue;

	CellUnitList defender;
	g_theWorld->GetArmy(pos, defender);

	bool const  i_died = !army.AccessData()->Fight(defender);
	if (!i_died)
	{
		for (sint32 k = 0; k < army.Num(); k++) {
			bool out_of_fuel;
			army[k].DeductMoveCost(g_theConstDB->Get(0)->GetSpecialActionMoveCost(),
								   out_of_fuel);
		}
	}

	return GEV_HD_Continue;
}

STDEHANDLER(AftermathEvent)
{
	Army army;
	MapPoint pos;
	Unit ta;
	Unit td;
	sint32 attack_owner, defense_owner;

	sint32 fromARealBattle;
	if(!args->GetInt(0, fromARealBattle)) return GEV_HD_Continue;

	Assert(!fromARealBattle || g_theCurrentBattle);
	if (g_theCurrentBattle)
	{
		delete g_theCurrentBattle;
		g_theCurrentBattle = NULL;
	}

	args->GetArmy(0, army);

	if(!args->GetPos(0, pos))
		return GEV_HD_Continue;

	args->GetUnit(0, ta);
	args->GetUnit(1, td);

	if(!args->GetPlayer(0, attack_owner))
		return GEV_HD_Continue;

	if(!args->GetPlayer(1, defense_owner))
		return GEV_HD_Continue;

	Unit            c = g_theWorld->GetCell(pos)->GetCity();
	CellUnitList    defender;
	g_theWorld->GetArmy(pos, defender);

	if(c.IsValid())
	{
		//add civil war here?  no because its in MoveEvent? Why?  EMOD

		//end EMOD
		if(g_rand->Next(100) < g_theConstDB->Get(0)->GetAssaultDestroyBuildingChance() * 100)
		{
			//shouldn't constDB allow players to set how many buildings be destroyed?
			c.DestroyRandomBuilding();
		}
		//this code actually isn't used...
		if(defender.Num() > 0)
		{
			if(c.PopCount() > 1 && (g_rand->Next(100) < g_theConstDB->Get(0)->GetAssaultKillPopChance() * 100))
			{
				//emod allows for differnt city casualty rates instead of just one  6.22.2007
				sint32 casualties = 0;
				if (g_theConstDB->Get(0)->GetCapturedCityKillPop() < 0)
				{
					casualties = g_rand->Next(c.PopCount()) * -1 ;
				}
				else
				{
					casualties = g_theConstDB->Get(0)->GetCapturedCityKillPop() * -1;
				}

				c.CD()->ChangePopulation(casualties);
			}
		}
	}

	if(ta.IsValid())
	{
		g_director->AddTerminateFaceoff(ta);
	}

	if(td.IsValid())
	{
		g_director->AddTerminateFaceoff(td);
	}

	//// similar to ArmyData::IsWounded()
	double attacker_relTotHP= 0;
	if(army.IsValid() && army.Num() > 0)
	{
		for(sint32 i = 0; i < army.Num(); i++)
		{
			attacker_relTotHP+= army[i].GetHP();
		}
		attacker_relTotHP/= army.Num();
	}
	
	//// similar to ArmyData::IsWounded()
	double defender_relTotHP= 0;
	if(defender.Num() > 0)
	{
		for(sint32 i = 0; i < defender.Num(); i++)
		{
			defender_relTotHP+= defender[i].GetHP();
		}
		defender_relTotHP/= defender.Num();
	}

	bool attackerWon = attacker_relTotHP > defender_relTotHP;

	if(attackerWon)
	{
		if(g_selected_item->IsPlayerVisible(defense_owner) && !c.m_id)
		{
			g_soundManager->AddGameSound(GAMESOUNDS_VICTORY_FANFARE);
		}

		if(g_selected_item->IsPlayerVisible(attack_owner) && !c.m_id)
		{
			g_soundManager->AddGameSound(GAMESOUNDS_VICTORY_FANFARE);
		}

		if(army.IsValid())
		{
			army.AccessData()->DoVictoryEnslavement(defense_owner, td);
		}
	}
	else
	{
		if(g_selected_item->IsPlayerVisible(attack_owner) && !c.m_id)
		{
			g_soundManager->AddGameSound(GAMESOUNDS_LOSE_PLAYER_BATTLE);
		}

		if(g_selected_item->IsPlayerVisible(defense_owner) && !c.m_id)
		{
			g_soundManager->AddGameSound(GAMESOUNDS_LOSE_PLAYER_BATTLE);
		}

		defender.DoVictoryEnslavement(attack_owner, ta);
	}

	if(attackerWon && army.IsValid() && army.Num() > 0)
	{
		for(sint32 i = 0; i < army.Num(); i++)
		{
			if(army[i].GetHP() < 0.5) {
				continue;
			}
			//TODO find out why its not running the leader & elite code
			// apparently a '#' outcomented the value in constDB.txt
			if( (army[i].GetAttack() > 0)
			&&  (g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetCombatLeaderChance() * 100.0))
			&&  (army[i].IsElite()) //IsElite
			){
				g_player[attack_owner]->CreateLeader(); //Great Leader Code - Emod 6-5-2007
			}

			if( (army[i].GetAttack() > 0)
			&&  (g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetCombatEliteChance() * 100.0))
			&&  (army[i].IsVeteran())
			){
				army[i].SetElite(); //elite code - Emod 6-5-2007
			}

			if( (army[i].GetAttack() > 0)
			&&  (g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetCombatVeteranChance() * 100.0))
			&&  (!army[i].IsVeteran())
			){
				army[i].SetVeteran();
			}

			//end emod
			army[i].WakeUp();
			army[i].ClearFlag(k_UDF_WAS_TOP_UNIT_BEFORE_BATTLE);
			army[i].SetFlag(k_UDF_FIRST_MOVE);
		}
//		g_director->IncrementPendingGameActions();
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_VictoryMoveOrder,
							   GEA_Army, army,
							   GEA_MapPoint, pos,
							   GEA_End);
	}

	for(sint32 i = 0; i < defender.Num() ; i++)
	{
		if(defender[i].GetAttack() > 0 &&
		   g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetCombatVeteranChance() * 100.0))
		{
			defender[i].SetVeteran();
		}
		if( (defender[i].GetAttack() > 0)
		&&  (g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetCombatEliteChance() * 100.0))
		&&  (defender[i].IsVeteran())
		){
			defender[i].SetElite();
		}
		//Great Leader Code - Emod 6-5-2007
		if( (defender[i].GetAttack() > 0)
		&&  (g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetCombatLeaderChance() * 100.0))
		&&  (defender[i].IsElite())
		){
			g_player[defense_owner]->CreateLeader();
		}

		//copy and make for elite units
		//add great leader chance but only if veteran and/or elite
		defender[i].WakeUp();
		defender[i].ClearFlag(k_UDF_WAS_TOP_UNIT_BEFORE_BATTLE);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(CheckOrdersEvent)
{
	Army a;
	if(!args->GetArmy(0, a))
		return GEV_HD_Continue;

	if(!g_network.IsActive() || g_network.IsLocalPlayer(a.GetOwner()))
	{
		a->CheckAddEventOrder();

		a.AccessData()->ExecuteOrders();
	}
	return GEV_HD_Continue;
}

STDEHANDLER(BeginTurnArmyEvent)
{
	Army a;
	if(!args->GetArmy(0, a))
		return GEV_HD_Continue;

	a.BeginTurn();
	return GEV_HD_Continue;
}

STDEHANDLER(MoveUnitsEvent)
{
	Army a;
	MapPoint from, to;

	if(!args->GetArmy(0, a)) return GEV_HD_Stop;
	if(!args->GetPos(0, from)) return GEV_HD_Continue;
	if(!args->GetPos(1, to)) return GEV_HD_Continue;

	for(sint32 i = 0; i < a.Num(); i++)
	{
		if(a[i].GetDBRec()->GetCantMove())
		{
			return GEV_HD_Stop;
		}
	}

	g_gevManager->AddEvent
	                      (
	                       GEV_INSERT_AfterCurrent,
	                       GEV_CheckOrders,
	                       GEA_Army,        a,
	                       GEA_MapPoint,    from,
	                       GEA_MapPoint,    to,
	                       GEA_End
	                      );

	a->MoveUnits(to);

//	sint32 old_cell_owner = g_theWorld->GetCell(from)->GetOwner();
	sint32 new_cell_owner = g_theWorld->GetCell(to)->GetOwner();
	sint32 army_owner = a->GetOwner();

	Player *player_ptr = g_player[new_cell_owner];
	if ( new_cell_owner != -1 &&
		 new_cell_owner != army_owner &&
		 player_ptr &&
		 player_ptr->IsVisible(to)) {

		const Diplomat & new_cell_diplomat = Diplomat::GetDiplomat(new_cell_owner);
		uint32 incursion_permission = new_cell_diplomat.GetIncursionPermission();
		if (!(incursion_permission & (0x1 << army_owner)) &&
			!new_cell_diplomat.GetBorderIncursionBy(army_owner))
		{
			bool is_threat = (a->HasCargo() || !a->IsCivilian()) &&
			                 a->PlayerCanSee(new_cell_owner);

			if (is_threat)
			{
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BorderIncursion,
					GEA_Player, new_cell_owner,
					GEA_Player, army_owner,
					GEA_End);
			}
		}
	}

	if(g_theWorld->HasCity(to))
	{
		Unit c = g_theWorld->GetCity(to);
		PLAYER_INDEX city_owner = c.GetOwner();
		if(city_owner != a->GetOwner())
		{
			if(!a->IsEnemy(city_owner))
			{
				SlicObject *so;
				if(g_network.IsActive()
				&& g_network.TeamsEnabled()
				&& g_player[a->GetOwner()]->m_networkGroup == g_player[city_owner]->m_networkGroup
				){
					so = new SlicObject("110aCantAttackTeammates");
				}
				else
				{
					so = new SlicObject("110CantAttackAllies");
				}
				so->AddRecipient(a->GetOwner());
				so->AddCivilisation(city_owner);
				so->AddUnit(a[0]);
				so->AddLocation(to);
				g_slicEngine->Execute(so);
			}
			else
			{
				for(sint32 i = 0; i < a->Num(); i++)
				{
					if(!a[i].IsCantCaptureCity())
					{
						PLAYER_INDEX originalOwner = c.GetOwner();

						if(g_rand->Next(100) < g_theConstDB->Get(0)->GetCaptureKillPopChance() * 100)
						{
							g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillPop,
												   GEA_City, c.m_id,
												   GEA_End);
#if 0
							if(g_network.IsHost())
							{
								g_network.Block(a.GetOwner());
							}
							c.CD()->ChangePopulation(-1);
							if(g_network.IsHost())
							{
								g_network.Unblock(a.GetOwner());
							}
#endif
						}

						if (c.IsCapitol())
						{
							//civil war code didnt work here either  EMOD

							SlicObject *so = new SlicObject("127CapitalCityCapturedVictim");
							so->AddRecipient(originalOwner);
							so->AddCity(c);
							g_slicEngine->Execute(so);

							so = new SlicObject("126CapitalCityCapturedAttacker");
							so->AddRecipient(a->GetOwner());
							so->AddCivilisation(originalOwner);
							so->AddCity(c);
							g_slicEngine->Execute(so);
						}
						else if (c.GetData()->GetCityData()->PopCount() >= 1)
						{
						//	Maybe we find another use for this message
						//	SlicObject *so = new SlicObject("123CitiesCapturedVictim");
						//	so->AddRecipient(originalOwner);
						//	so->AddCity(c);
						//	g_slicEngine->Execute(so);
						}

						if(c.GetOwner() == g_selected_item->GetVisiblePlayer())
							g_director->AddCenterMap(to);

						for(sint32 k = 0; k < a->Num(); k++)
						{
							a[k].SetMovementPoints(0.0);
						}

						g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
											   GEV_CaptureCity,
											   GEA_City, c,
											   GEA_Player, a->GetOwner(),
											   GEA_Int, (sint32)CAUSE_REMOVE_CITY_ATTACK,
											   GEA_End);
						break;
					}
				}
			}
		}
	}

	a->CheckActiveDefenders(to, false);

	if(!a.IsValid())
	{
		return GEV_HD_Stop;
	}

	a->CheckTerrainEvents();

	a->IncrementOrderPath();

	return GEV_HD_Continue;
}

STDEHANDLER(DirectorMoveUnitsEvent)
{
	Army a;
	MapPoint from, to;

	if (!args->GetArmy(0, a)) return GEV_HD_Continue;
	if (!args->GetPos(0, from)) return GEV_HD_Continue;
	if (!args->GetPos(1, to)) return GEV_HD_Continue;

	if (a.Num() <= 0) return GEV_HD_Continue;

	bool hiddenMove = false;
	if (a->IsStealth() && !a->IsVisible(g_selected_item->GetPlayerOnScreen())) {
		hiddenMove = true;
	}

	if (g_selected_item->GetPlayerOnScreen() != -1 &&
		g_selected_item->GetPlayerOnScreen() != g_selected_item->GetVisiblePlayer() &&
		!g_network.IsActive())
	{
		return GEV_HD_Continue;
	}

	Unit top_src = a->GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());
	if (top_src.m_id == 0) {
		top_src = a[0];
	}

	UnitActor **restOfStack = NULL;
	sint32 numRest = a->Num() - 1;
	if (numRest > 0) {
		restOfStack = new (UnitActor* [numRest]);
		a->GetActors(top_src, restOfStack);
	}

	MapPoint newPos = to;

	if(!hiddenMove && g_selected_item->IsAutoCenterOn()
	   && !g_director->TileWillBeCompletelyVisible(newPos.x, newPos.y)
	   && (top_src.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()))
	   && (g_theProfileDB->IsEnemyMoves() || top_src.GetOwner() == g_selected_item->GetVisiblePlayer()))
	{
		g_director->AddCenterMap(newPos);
	}

	if (hiddenMove || !to.IsNextTo(from)) {
		g_director->AddTeleport(top_src, from, newPos, numRest, restOfStack);
	} else {
		g_director->AddMove(top_src, from, newPos, numRest, restOfStack, top_src.GetMoveSoundID());
	}

	if (top_src.GetData()->HasLeftMap()) {
		g_director->AddHide(top_src);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(ArmyFinishUnloadEvent)
{
	Army transportArmy;
	Army unloadingArmy;
	MapPoint to_pt;

	if(!args->GetArmy(0, transportArmy)) return GEV_HD_Continue;
	if(!args->GetArmy(1, unloadingArmy)) return GEV_HD_Continue;
	if(!args->GetPos(0, to_pt)) return GEV_HD_Continue;

	transportArmy->FinishUnloadOrder(unloadingArmy, to_pt);

	return GEV_HD_Continue;
}

STDEHANDLER(LawsuitEvent)
{
	Army a;
	MapPoint point;
	Unit lawyer;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, point)) return GEV_HD_Continue;
	if(!args->GetUnit(0, lawyer)) return GEV_HD_Continue;

	Cell *cell = g_theWorld->GetCell(point);

	a->InformAI(UNIT_ORDER_SUE, point);

	sint32 victim = cell->AccessUnit(0)->GetOwner();
	sint32 i, n = cell->GetNumUnits();
	sint32 utype = -1;
	for(i = n-1; i >= 0; i--) {
		Unit *u = &(cell->AccessUnit(i));
		if(u->GetDBRec()->GetCanBeSued()) {
			if(utype < 0)
				utype = u->GetType();

			victim = u->GetOwner();
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
								   GEA_Unit, u->m_id,
								   GEA_Int, CAUSE_REMOVE_ARMY_SUE,
								   GEA_Player, a->GetOwner(),
								   GEA_End);

		}
	}

	a->AddSpecialActionUsed(lawyer);

	if(utype >= 0) {
		SlicObject *so = new SlicObject("161SueCompleteVictim");
		so->AddRecipient(victim);
		so->AddLocation(a->RetPos());
		so->AddUnitRecord(utype);
		g_slicEngine->Execute(so);
		
		/* disabled because disappearing of the sued unit already tells the success and therefore message spamming can be avoided
		so = new SlicObject("911SueCompleteAttacker");
		so->AddRecipient(lawyer->GetOwner());
		so->AddUnitRecord(utype);
		g_slicEngine->Execute(so);
		*/
	}

	return GEV_HD_Continue;
}

STDEHANDLER(RemoveFranchiseEvent)
{
	Army a;
	Unit lawyer;
	Unit city;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetUnit(0, lawyer)) return GEV_HD_Continue;
	if(!args->GetCity(0, city)) return GEV_HD_Continue;

        // report message before owner is possibly reset by SetFranchiseTurnsRemaining
	sint32 TurnsToSueFranchise= g_theConstDB->Get(0)->GetTurnsToSueFranchise();
	SlicObject *so = new SlicObject("911SueFranchiseCompleteVictim");
	so->AddRecipient(city.GetFranchiseOwner());
	so->AddCity(city);
	so->AddGold(TurnsToSueFranchise >= 0 ? TurnsToSueFranchise + 1 : 0);// misuseing AddGold for passing the remaining franchise turns
	so->AddGold(city.GetProductionLostToFranchise());
	g_slicEngine->Execute(so);

	so = new SlicObject("911SueFranchiseCompleteAttacker");
	so->AddRecipient(lawyer.GetOwner());
	so->AddCity(city);
	so->AddGold(TurnsToSueFranchise >= 0 ? TurnsToSueFranchise + 1 : 0);// misuseing AddGold for passing the remaining franchise turns
	g_slicEngine->Execute(so);

	city.SetFranchiseTurnsRemaining(TurnsToSueFranchise);

	return GEV_HD_Continue;
}

STDEHANDLER(ExpelUnitsEvent)
{
	return GEV_HD_Continue;
}

STDEHANDLER(EnslaveSettlerEvent)
{
	Army    a;
	Unit    slaver;
	Unit    settler;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetUnit(0, slaver)) return GEV_HD_Continue;
	if(!args->GetUnit(1, settler)) return GEV_HD_Continue;

	PLAYER_INDEX    settlerOwner    = settler.GetOwner();
	PLAYER_INDEX    slaverOwner     = slaver.GetOwner();

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
						   GEA_Unit,    settler,
						   GEA_Int,     CAUSE_REMOVE_ARMY_ENSLAVED,
						   GEA_Player,  slaverOwner,
						   GEA_End);

	Unit home_city;
	if (g_player[slaverOwner]->GetSlaveCity(slaver.RetPos(), home_city))
	{
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MakePop,
		                       GEA_City,    home_city.m_id,
		                       GEA_Player,  settlerOwner,
		                       GEA_End);

		SlicObject *so = new SlicObject("139SettlerSlavedVictim");
		so->AddRecipient(settlerOwner);
		so->AddUnitRecord(settler.GetType());
		so->AddLocation(settler.RetPos());
		g_slicEngine->Execute(so);

		so = new SlicObject("137SlaveryCompleteAttacker");
		so->AddRecipient(slaverOwner);
		so->AddCivilisation(slaverOwner);
		so->AddCity(home_city);
		g_slicEngine->Execute(so);
	}
	// else No action: the slaver does not have any cities

	return GEV_HD_Continue;
}

STDEHANDLER(TeleportEvent)
{
	Army a;
	MapPoint pos;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	a->SetPositionAndFixActors(pos);

	return GEV_HD_Continue;
}

STDEHANDLER(ReentryEvent)
{
	Army a;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	a->Reenter();
	return GEV_HD_Continue;
}

STDEHANDLER(SetUnloadMovementEvent)
{
	Army a;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	a->SetUnloadMovementPoints();
	return GEV_HD_Continue;
}

STDEHANDLER(ArmyBeginTurnExecuteEvent)
{
	Army a;
	if (args->GetArmy(0, a))
	{
		a->ExecuteOrders();
	}

	return GEV_HD_Continue;
}

void armyevent_Initialize()
{
	g_gevManager->AddCallback(GEV_MoveOrder, GEV_PRI_Primary, &s_ArmyMoveOrderEvent);
	g_gevManager->AddCallback(GEV_MoveToOrder, GEV_PRI_Primary, &s_ArmyMoveToOrderEvent);
	g_gevManager->AddCallback(GEV_MovePathOrder, GEV_PRI_Primary, &s_ArmyMovePathOrderEvent);
	g_gevManager->AddCallback(GEV_VictoryMoveOrder, GEV_PRI_Primary, &s_ArmyVictoryMoveOrderEvent);

	g_gevManager->AddCallback(GEV_UnloadOrder, GEV_PRI_Primary, &s_ArmyUnloadOrderEvent);
	g_gevManager->AddCallback(GEV_SleepOrder, GEV_PRI_Primary, &s_ArmySleepOrderEvent);
	g_gevManager->AddCallback(GEV_MoveUnloadOrder, GEV_PRI_Primary, &s_ArmyMoveUnloadOrderEvent);
	g_gevManager->AddCallback(GEV_EntrenchOrder, GEV_PRI_Primary, &s_ArmyEntrenchOrderEvent);
	g_gevManager->AddCallback(GEV_DetrenchOrder, GEV_PRI_Primary, &s_ArmyDetrenchOrderEvent);
	g_gevManager->AddCallback(GEV_DisbandArmyOrder, GEV_PRI_Primary, &s_ArmyDisbandArmyOrderEvent);
	g_gevManager->AddCallback(GEV_GroupOrder, GEV_PRI_Primary, &s_ArmyGroupOrderEvent);
	g_gevManager->AddCallback(GEV_GroupUnitOrder, GEV_PRI_Primary, &s_ArmyGroupUnitOrderEvent);
	g_gevManager->AddCallback(GEV_UngroupOrder, GEV_PRI_Primary, &s_ArmyUngroupOrderEvent);
	g_gevManager->AddCallback(GEV_InvestigateCityOrder, GEV_PRI_Primary, &s_ArmyInvestigateCityOrderEvent);
	g_gevManager->AddCallback(GEV_NullifyWallsOrder, GEV_PRI_Primary, &s_ArmyNullifyWallsOrderEvent);
	g_gevManager->AddCallback(GEV_StealTechnologyOrder, GEV_PRI_Primary, &s_ArmyStealTechnologyOrderEvent);
	g_gevManager->AddCallback(GEV_InciteRevolutionOrder, GEV_PRI_Primary, &s_ArmyInciteRevolutionOrderEvent);
	g_gevManager->AddCallback(GEV_AssassinateRulerOrder, GEV_PRI_Primary, &s_ArmyAssassinateRulerOrderEvent);
	g_gevManager->AddCallback(GEV_InvestigateReadinessOrder, GEV_PRI_Primary, &s_ArmyInvestigateReadinessOrderEvent);
	g_gevManager->AddCallback(GEV_BombardOrder, GEV_PRI_Primary, &s_ArmyBombardOrderEvent);
	g_gevManager->AddCallback(GEV_FranchiseOrder, GEV_PRI_Primary, &s_ArmyFranchiseOrderEvent);
	g_gevManager->AddCallback(GEV_SueOrder, GEV_PRI_Primary, &s_ArmySueOrderEvent);
	g_gevManager->AddCallback(GEV_SueFranchiseOrder, GEV_PRI_Primary, &s_ArmySueFranchiseOrderEvent);
	g_gevManager->AddCallback(GEV_ExpelOrder, GEV_PRI_Primary, &s_ArmyExpelOrderEvent);
	g_gevManager->AddCallback(GEV_EstablishEmbassyOrder, GEV_PRI_Primary, &s_ArmyEstablishEmbassyOrderEvent);
	g_gevManager->AddCallback(GEV_ThrowPartyOrder, GEV_PRI_Primary, &s_ArmyThrowPartyOrderEvent);
	g_gevManager->AddCallback(GEV_AdvertiseOrder, GEV_PRI_Primary, &s_ArmyAdvertiseOrderEvent);
	g_gevManager->AddCallback(GEV_PlantNukeOrder, GEV_PRI_Primary, &s_ArmyPlantNukeOrderEvent);
	g_gevManager->AddCallback(GEV_SlaveRaidOrder, GEV_PRI_Primary, &s_ArmySlaveRaidOrderEvent);
	g_gevManager->AddCallback(GEV_EnslaveSettlerOrder, GEV_PRI_Primary, &s_ArmyEnslaveSettlerOrderEvent);
	g_gevManager->AddCallback(GEV_UndergroundRailwayOrder, GEV_PRI_Primary, &s_ArmyUndergroundRailwayOrderEvent);
	g_gevManager->AddCallback(GEV_InciteUprisingOrder, GEV_PRI_Primary, &s_ArmyInciteUprisingOrderEvent);
	g_gevManager->AddCallback(GEV_BioInfectOrder, GEV_PRI_Primary, &s_ArmyBioInfectOrderEvent);
	g_gevManager->AddCallback(GEV_PlagueOrder, GEV_PRI_Primary, &s_ArmyPlagueOrderEvent);
	g_gevManager->AddCallback(GEV_NanoInfectOrder, GEV_PRI_Primary, &s_ArmyNanoInfectOrderEvent);
	g_gevManager->AddCallback(GEV_ConvertCityOrder, GEV_PRI_Primary, &s_ArmyConvertCityOrderEvent);
	g_gevManager->AddCallback(GEV_ReformCityOrder, GEV_PRI_Primary, &s_ArmyReformCityOrderEvent);
	g_gevManager->AddCallback(GEV_SellIndulgencesOrder, GEV_PRI_Primary, &s_ArmySellIndulgencesOrderEvent);
	g_gevManager->AddCallback(GEV_SoothsayOrder, GEV_PRI_Primary, &s_ArmySoothsayOrderEvent);
	g_gevManager->AddCallback(GEV_CreateParkOrder, GEV_PRI_Primary, &s_ArmyCreateParkOrderEvent);
	g_gevManager->AddCallback(GEV_PillageOrder, GEV_PRI_Primary, &s_ArmyPillageOrderEvent);
	g_gevManager->AddCallback(GEV_InjoinOrder, GEV_PRI_Primary, &s_ArmyInjoinOrderEvent);
	g_gevManager->AddCallback(GEV_PirateOrder, GEV_PRI_Primary, &s_ArmyPirateOrderEvent);
	g_gevManager->AddCallback(GEV_GetExpelledOrder, GEV_PRI_Primary, &s_ArmyGetExpelledOrderEvent);
	g_gevManager->AddCallback(GEV_SettleOrder, GEV_PRI_Primary, &s_ArmySettleOrderEvent);
	g_gevManager->AddCallback(GEV_SettleInCityOrder, GEV_PRI_Primary, &s_ArmySettleInCityOrderEvent);
	g_gevManager->AddCallback(GEV_BoardTransportOrder, GEV_PRI_Primary, &s_BoardTransportOrderEvent);
	g_gevManager->AddCallback(GEV_LaunchOrder, GEV_PRI_Primary, &s_LaunchOrderEvent);
	g_gevManager->AddCallback(GEV_TargetOrder, GEV_PRI_Primary, &s_TargetOrderEvent);
	g_gevManager->AddCallback(GEV_UpgradeOrder, GEV_PRI_Primary, &s_UpgradeOrderEvent);

	g_gevManager->AddCallback(GEV_MoveArmy, GEV_PRI_Primary, &s_ArmyMoveEvent);
	g_gevManager->AddCallback(GEV_ClearOrders, GEV_PRI_Primary, &s_ClearOrdersEvent);
	g_gevManager->AddCallback(GEV_FinishAttack, GEV_PRI_Primary, &s_FinishAttackEvent);
	g_gevManager->AddCallback(GEV_FinishMove, GEV_PRI_Primary, &s_FinishMoveEvent);

	g_gevManager->AddCallback(GEV_MoveUnits, GEV_PRI_Primary, &s_MoveUnitsEvent);
	g_gevManager->AddCallback(GEV_MoveUnits, GEV_PRI_Post, &s_DirectorMoveUnitsEvent);

	g_gevManager->AddCallback(GEV_CheckOrders, GEV_PRI_Primary, &s_CheckOrdersEvent);

	g_gevManager->AddCallback(GEV_MoveIntoTransport, GEV_PRI_Primary, &s_MoveIntoTransportEvent);
	g_gevManager->AddCallback(GEV_Battle, GEV_PRI_Primary, &s_BattleEvent);
	g_gevManager->AddCallback(GEV_BattleAftermath, GEV_PRI_Primary, &s_AftermathEvent);

	g_gevManager->AddCallback(GEV_BeginTurnArmy, GEV_PRI_Primary, &s_BeginTurnArmyEvent);
	g_gevManager->AddCallback(GEV_FinishUnload, GEV_PRI_Primary, &s_ArmyFinishUnloadEvent);

	g_gevManager->AddCallback(GEV_Lawsuit, GEV_PRI_Primary, &s_LawsuitEvent);
	g_gevManager->AddCallback(GEV_RemoveFranchise, GEV_PRI_Primary, &s_RemoveFranchiseEvent);
	g_gevManager->AddCallback(GEV_ExpelUnits, GEV_PRI_Primary, &s_ExpelUnitsEvent);
	g_gevManager->AddCallback(GEV_EnslaveSettler, GEV_PRI_Primary, &s_EnslaveSettlerEvent);

	g_gevManager->AddCallback(GEV_Teleport, GEV_PRI_Primary, &s_TeleportEvent);
	g_gevManager->AddCallback(GEV_Reentry, GEV_PRI_Primary, &s_ReentryEvent);

	g_gevManager->AddCallback(GEV_SetUnloadMovement, GEV_PRI_Primary, &s_SetUnloadMovementEvent);

	g_gevManager->AddCallback(GEV_BeginTurnExecute, GEV_PRI_Primary, &s_ArmyBeginTurnExecuteEvent);
}

void armyevent_Cleanup()
{
}
