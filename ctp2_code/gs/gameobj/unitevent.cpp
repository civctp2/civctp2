


#include "c3.h"
#include "UnitEvent.h"
#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "Army.h"
#include "UnitData.h"
#include "ArmyData.h"
#include "player.h"
#include "UnitActor.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "order.h"
#include "UnitRecord.h"
#include "aicause.h"
#include "tiledmap.h"
#include "cityespionage.h"
#include "Cell.h"
#include "network.h"
#include "net_info.h"
#include "CityInfluenceIterator.h"

STDEHANDLER(KillUnitEvent)
{
	Unit u;
	CAUSE_REMOVE_ARMY cause;
	sint32 killer;

	if(gameEventType == GEV_KillUnit) {
		if(!args->GetUnit(0, u))
			return GEV_HD_Continue;
	} else if(gameEventType == GEV_KillCity) {
		if(!args->GetCity(0, u))
			return GEV_HD_Continue;
	}

	if(!args->GetInt(0, (sint32&)cause))
		return GEV_HD_Continue;

	if(!args->GetPlayer(0, killer))
		killer = -1;

	if(u->GetCargoList() && u->GetCargoList()->Num() > 0) {
		
		
		
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
							   GEA_Unit, u,
							   GEA_Int, cause,
							   GEA_Player, killer,
							   GEA_End);
		sint32 c;
		for(c = 0; c < u->GetCargoList()->Num(); c++) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
								   GEA_Unit, u->GetCargoList()->Access(c).m_id,
								   GEA_Int, CAUSE_REMOVE_ARMY_TRANSPORT_DIED,
								   GEA_Player, killer,
								   GEA_End);
		}

		return GEV_HD_Stop; 
	}

	u.Kill(cause, killer);
	return GEV_HD_Continue;
}

STDEHANDLER(UnitBeginTurnVisionEvent)
{
	Unit u;
	sint32 player;

	if(!args->GetUnit(0, u))
		return GEV_HD_Continue;

	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	u.BeginTurnVision(player);
	return GEV_HD_Continue;
}

STDEHANDLER(BeginTurnUnitEvent)
{
	Unit u;
	if(!args->GetUnit(0, u))
		return GEV_HD_Continue;

	u.BeginTurn();
	return GEV_HD_Continue;
}

STDEHANDLER(AddUnitToArmyEvent)
{
	Unit u;
	Army a;
	CAUSE_NEW_ARMY cause;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetInt(0, (sint32 &)cause)) return GEV_HD_Continue;

	u->ChangeArmy(a, cause);
	
	Assert(u->GetArmy()->m_id == a->m_id);

	return GEV_HD_Continue;
}

STDEHANDLER(SleepUnitEvent)
{
    Unit u;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;

	u.Sleep();
	return GEV_HD_Continue;
}

STDEHANDLER(WakeUnitEvent)
{
    Unit u;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;

	u.WakeUp();
	return GEV_HD_Continue;
}

STDEHANDLER(EntrenchUnitEvent)
{
    Unit u;
	if(!args->GetUnit(0, u)) 
		
		return GEV_HD_Stop;

	u->Entrench();
	return GEV_HD_Continue;
}

STDEHANDLER(DetrenchUnitEvent)
{
	Unit u;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;

	u->Detrench();
	return GEV_HD_Continue;
}

STDEHANDLER(InvestigationEvent)
{
	Unit c;
	
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	CityEspionage::Display(&c);
	return GEV_HD_Continue;
}
	
STDEHANDLER(InciteRevolutionUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	u.InciteRevolution(c);
	return GEV_HD_Continue;
}

STDEHANDLER(AssassinateRulerUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	u.AssassinateRuler(c);
	return GEV_HD_Continue;
}

STDEHANDLER(PlantNukeUnitEvent)
{
	
	return GEV_HD_Continue;
}
	

STDEHANDLER(UndergroundRailwayUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	c.RemoveOneSlave(u.GetOwner());
	
	Unit hc;
	double distance;
	sint32 r = g_player[u.GetOwner()]->GetNearestCity(u.RetPos(),
													  hc, 
													  distance);
	Assert(r);

	if(!r)
		return GEV_HD_Continue;

	if(hc.m_id == 0) {
		Assert(hc.m_id != 0);
		return GEV_HD_Continue;
	}
	u->GetArmy()->AddSpecialActionUsed(u);

	MapPoint cpos;
	hc.GetPos(cpos);
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MakePop,
						   GEA_City, hc.m_id,
						   GEA_End);
	
	u->GetArmy()->ActionSuccessful(SPECATTACK_FREESLAVES, u, c);

	SlicObject *so;
	so = new SlicObject("163FreeslaveCompleteVictim") ;
	so->AddRecipient(c.GetOwner()) ;
	so->AddCity(c) ;
	g_slicEngine->Execute(so) ;

	so = new SlicObject("165FreeslaveCompleteAgressor") ;
	so->AddRecipient(u.GetOwner()) ;
	so->AddCity(c);
	so->AddCity(hc) ;
	g_slicEngine->Execute(so) ;

	return GEV_HD_Continue;
}

STDEHANDLER(InciteUprisingUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	c.DoUprising(UPRISING_CAUSE_INCITED);
	return GEV_HD_Continue;
}

STDEHANDLER(EstablishEmbassyUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	ORDER_RESULT res = u.EstablishEmbassy(c);

	if(res == ORDER_RESULT_SUCCEEDED)
    {
        SlicObject *so = new SlicObject("143EmbassyVictim") ;
        so->AddRecipient(c.GetOwner()) ;
        so->AddCivilisation(u.GetOwner());
		so->AddUnitRecord(u.GetType());
		so->AddCity(c) ;
        g_slicEngine->Execute(so) ;

        so = new SlicObject("144EmbassyAttacker");
        so->AddRecipient(u.GetOwner()) ;
        so->AddCivilisation(c.GetOwner());
		so->AddUnitRecord(u.GetType());
		so->AddCity(c) ;
        g_slicEngine->Execute(so) ;
    }
	return GEV_HD_Continue;
}

STDEHANDLER(ThrowPartyUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	ORDER_RESULT res = u->ThrowParty(c,0);

	if(res == ORDER_RESULT_SUCCEEDED)
    {
        SlicObject *so = new SlicObject("149PartyCompleteVictim") ;
        so->AddRecipient(c.GetOwner());
        so->AddPlayer(u.GetOwner());
		so->AddUnitRecord(u.GetType());
		so->AddCity(c) ;
        g_slicEngine->Execute(so) ;

        so = new SlicObject("150PartyCompleteAttacker");
        so->AddRecipient(u.GetOwner()) ;
        so->AddPlayer(c.GetOwner());
		so->AddUnitRecord(u.GetType());
		so->AddCity(c) ;
        g_slicEngine->Execute(so) ;
    }
	return GEV_HD_Continue;
}

STDEHANDLER(BioInfectCityUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	
	SlicObject *so;

	so = new SlicObject("33CrisisCityInfected") ;
	so->AddRecipient(c.GetOwner()) ;
	so->AddCity(c) ;
	g_slicEngine->Execute(so) ;

	DPRINTF(k_DBG_GAMESTATE, ("Bio infection succeeded\n"));
	
	so = new SlicObject("10iBioInfectComplete") ;
	so->AddRecipient(c.GetOwner()) ;
	so->AddCivilisation(u.GetOwner()) ;
	so->AddCity(c) ;
	g_slicEngine->Execute(so) ;

	so = new SlicObject("11iBioInfectComplete") ;
	so->AddRecipient(u.GetOwner()) ;
	so->AddCivilisation(c.GetOwner()) ;
	so->AddCity(c) ;
	g_slicEngine->Execute(so) ;

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_BioInfectCity,
						   GEA_City, c.m_id,
						   GEA_Player, u.GetOwner(),
						   GEA_End);
	

	u.GetArmy()->ActionSuccessful(SPECATTACK_BIOTERROR, u, c);
	return GEV_HD_Continue;
}

STDEHANDLER(PlagueCityUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	
	SlicObject *so;

	so = new SlicObject("33CrisisCityInfected") ;
	so->AddRecipient(c.GetOwner()) ;
	so->AddCity(c) ;
	g_slicEngine->Execute(so) ;

	DPRINTF(k_DBG_GAMESTATE, ("Bio infection succeeded\n"));
	
	so = new SlicObject("10jPlagueComplete") ;
	so->AddRecipient(c.GetOwner()) ;
	so->AddCivilisation(c.GetOwner()) ;
	so->AddCity(c) ;
	g_slicEngine->Execute(so) ;

	so = new SlicObject("11jPlagueComplete") ;
	so->AddRecipient(u.GetOwner()) ;
	so->AddCivilisation(u.GetOwner()) ;
	so->AddCity(c) ;
	g_slicEngine->Execute(so) ;

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_PlagueCity,
						   GEA_City, c.m_id,
						   GEA_Player, u.GetOwner(),
						   GEA_End);
	

	u.GetArmy()->ActionSuccessful(SPECATTACK_BIOTERROR, u, c);
	return GEV_HD_Continue;
}

STDEHANDLER(NanoInfectCityUnitEvent)
{
	Unit u, c;

	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	DPRINTF(k_DBG_GAMESTATE, ("Nano infection succeeded"));
	
	SlicObject *so;
	so = new SlicObject("911CrisisCityIsNanoInfected") ;
	so->AddRecipient(c.GetOwner()) ;
	so->AddCivilisation(u.GetOwner()) ;
	so->AddCity(c) ;
	g_slicEngine->Execute(so) ;
	
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_NanoInfectCity,
						   GEA_City, c.m_id,
						   GEA_Player, u.GetOwner(),
						   GEA_End);
	
	
	u.GetArmy()->ActionSuccessful(SPECATTACK_NANOTERROR, u, c);
	return GEV_HD_Continue;
}

STDEHANDLER(ConvertCityUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	
	DPRINTF(k_DBG_GAMESTATE, ("Conversion succeeded\n"));
	if(u.GetDBRec()->GetIsTelevangelist()) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ConvertCity,
							   GEA_City, c.m_id,
							   GEA_Player, u.GetOwner(),
							   GEA_Int, CONVERTED_BY_TELEVANGELIST,
							   GEA_End);
							   
		
	} else {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ConvertCity,
							   GEA_City, c.m_id,
							   GEA_Player, u.GetOwner(),
							   GEA_Int, CONVERTED_BY_CLERIC,
							   GEA_End);
		
	}
	
	{
		SlicObject *so = new SlicObject("151ConvertCompleteVictim") ;
		so->AddRecipient(c.GetOwner()) ;
		so->AddCity(c) ;
		so->AddUnitRecord(u.GetType());
		g_slicEngine->Execute(so) ;
	}
	
	u.GetArmy()->ActionSuccessful(SPECATTACK_CONVERTCITY, u, c);
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ContactMade,
						   GEA_Player, c.GetOwner(),
						   GEA_Player, u.GetOwner(),
						   GEA_End);
	
	return GEV_HD_Continue;
}

STDEHANDLER(ReformCityUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	DPRINTF(k_DBG_GAMESTATE, ("Reformation succeeded\n"));
	
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_UnconvertCity,
						   GEA_City, c.m_id,
						   GEA_End);
	
	u.GetArmy()->ActionSuccessful(SPECATTACK_REFORMCITY, u, c);
	
	SlicObject *so = new SlicObject("135ReformCity") ;
	so->AddCity(c);
	so->AddRecipient(c.GetOwner()) ;
	g_slicEngine->Execute(so) ;
	return GEV_HD_Continue;
}

STDEHANDLER(CreateParkUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_CreatePark,
						   GEA_City, c.m_id,
						   GEA_Player, u.GetOwner(),
						   GEA_End);

	return GEV_HD_Continue;
}

STDEHANDLER(InjoinUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_InjoinCity,
						   GEA_City, c.m_id,
						   GEA_Player, u.GetOwner(),
						   GEA_End);
	
	return GEV_HD_Continue;
}

STDEHANDLER(NukeCityUnitEvent)
{
	Unit u, c;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_NukeCity,
						   GEA_City, c,
						   GEA_Player, u.GetOwner(),
						   GEA_End);

	
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
		GEA_Unit, u,
		GEA_Int, CAUSE_REMOVE_ARMY_NUKE,
		GEA_Player, -1,
		GEA_End);

	return GEV_HD_Continue;
}

STDEHANDLER(NukeLocationUnitEvent)
{
	Unit u;
	MapPoint pos;

	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	sint32 nukeOwner = u.GetOwner();

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
						   GEA_Unit, u.m_id,
						   GEA_Int, CAUSE_REMOVE_ARMY_NUKE,
						   GEA_Player, -1,
						   GEA_End);

	static CellUnitList tempKillList;
	tempKillList.Clear();
	
	
	SquareIterator it(pos, 1);
	for(it.Start(); !it.End(); it.Next()) {
		Cell *cell = g_theWorld->GetCell(it.Pos());
		sint32 i;
		for(i = 0; i < cell->GetNumUnits(); i++) {
			if(cell->AccessUnit(i).m_id != u.m_id) {  
				tempKillList.Insert(cell->AccessUnit(i));
			}
		}
	}
		
	
	
	for(sint32 j = 0; j < tempKillList.Num(); j++) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
							   GEA_Unit, tempKillList[j].m_id,
							   GEA_Int, CAUSE_REMOVE_ARMY_NUKE,
							   GEA_Player, nukeOwner,
							   GEA_End);
	}

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
						   GEV_KillTile,
						   GEA_MapPoint, pos,
						   GEA_End);

	g_tiledMap->InvalidateMix();
	g_tiledMap->InvalidateMap();
	g_tiledMap->Refresh();
	return GEV_HD_Continue;
}

STDEHANDLER(MADLaunchEvent)
{
	Unit u;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MovePathOrder,
						   GEA_Army, u->GetArmy(),
						   GEA_MapPoint, u->GetTargetCity().RetPos(),
						   GEA_End);
	return GEV_HD_Continue;
}

STDEHANDLER(DisbandUnitEvent)
{
	Unit u;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;

	u.Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
	return GEV_HD_Continue;
}

STDEHANDLER(LaunchUnitEvent)
{
	Unit u;
	MapPoint pos;
	
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	if(!g_player[u.GetOwner()]) return GEV_HD_Continue;

	sint32 spaceTurns;
	if(!u.GetDBRec()->GetSpaceLaunch(spaceTurns))
		return GEV_HD_Continue;

	if(spaceTurns > 0) {
		g_theWorld->RemoveUnitReference(u.RetPos(), u);
		u->RemoveUnitVision();
		u.SetFlag(k_UDF_HAS_LEFT_MAP);
		u.SetFlag(k_UDF_IN_SPACE);

		u.GetArmy()->SetReentry(spaceTurns, pos);
		g_director->AddHide(u);
	} else {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_Reentry,
							   GEA_Army, u.GetArmy().m_id,
							   GEA_End);
	}


	return GEV_HD_Continue;
}

STDEHANDLER(SetTargetEvent)
{
	Unit u;
	Unit city;

	if(!args->GetUnit(0, u)) return GEV_HD_Continue;
	if(!args->GetCity(0, city)) return GEV_HD_Continue;

	u->SetTargetCity(city);
	return GEV_HD_Continue;
}

STDEHANDLER(ClearTargetEvent)
{
	Unit u;

	if(!args->GetUnit(0, u)) return GEV_HD_Continue;

	u->SetTargetCity(Unit(0));
	return GEV_HD_Continue;
}

STDEHANDLER(ActivateAllUnitsEvent)
{
	MapPoint pos;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	sint32 i;
	Cell *cell = g_theWorld->GetCell(pos);
	for(i = 0; i < cell->GetNumUnits(); i++) {
		if(cell->AccessUnit(i).IsEntrenched()) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
								   GEV_DetrenchUnit,
								   GEA_Unit, cell->AccessUnit(i),
								   GEA_End);
		} else if(cell->AccessUnit(i).IsAsleep()) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
								   GEV_WakeUnit,
								   GEA_Unit, cell->AccessUnit(i),
								   GEA_End);
		}
	}
	return GEV_HD_Continue;
}

STDEHANDLER(SetUnloadMovementUnitEvent)
{
	Unit u;
	if(!args->GetUnit(0, u)) return GEV_HD_Continue;

	u.SetMovementPoints(0.0);
	if(g_network.IsHost()) {
		g_network.Block(u.GetOwner());
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_MOVEMENT_TO_ZERO, u.m_id));
		g_network.Unblock(u.GetOwner());
	}
	return GEV_HD_Continue;
}

void unitevent_Initialize() 
{
	g_gevManager->AddCallback(GEV_KillUnit, GEV_PRI_Primary, &s_KillUnitEvent);
	g_gevManager->AddCallback(GEV_KillCity, GEV_PRI_Primary, &s_KillUnitEvent);

	g_gevManager->AddCallback(GEV_UnitBeginTurnVision, GEV_PRI_Primary, &s_UnitBeginTurnVisionEvent);
	g_gevManager->AddCallback(GEV_BeginTurnUnit, GEV_PRI_Primary, &s_BeginTurnUnitEvent);

	g_gevManager->AddCallback(GEV_AddUnitToArmy, GEV_PRI_Primary, &s_AddUnitToArmyEvent);

	g_gevManager->AddCallback(GEV_SleepUnit, GEV_PRI_Primary, &s_SleepUnitEvent);
	g_gevManager->AddCallback(GEV_WakeUnit, GEV_PRI_Primary, &s_WakeUnitEvent);
	g_gevManager->AddCallback(GEV_EntrenchUnit, GEV_PRI_Primary, &s_EntrenchUnitEvent);
	g_gevManager->AddCallback(GEV_DetrenchUnit, GEV_PRI_Primary, &s_DetrenchUnitEvent);
	g_gevManager->AddCallback(GEV_DisplayInvestigationWindow, GEV_PRI_Primary, &s_InvestigationEvent);
	g_gevManager->AddCallback(GEV_InciteRevolutionUnit, GEV_PRI_Primary, &s_InciteRevolutionUnitEvent);
	g_gevManager->AddCallback(GEV_AssassinateRulerUnit, GEV_PRI_Primary, &s_AssassinateRulerUnitEvent);

	g_gevManager->AddCallback(GEV_PlantNukeUnit, GEV_PRI_Primary, &s_PlantNukeUnitEvent);
	g_gevManager->AddCallback(GEV_UndergroundRailwayUnit, GEV_PRI_Primary, &s_UndergroundRailwayUnitEvent);
	g_gevManager->AddCallback(GEV_InciteUprisingUnit, GEV_PRI_Primary, &s_InciteUprisingUnitEvent);
	g_gevManager->AddCallback(GEV_EstablishEmbassyUnit, GEV_PRI_Primary, &s_EstablishEmbassyUnitEvent);
	g_gevManager->AddCallback(GEV_ThrowPartyUnit, GEV_PRI_Primary, &s_ThrowPartyUnitEvent);
	g_gevManager->AddCallback(GEV_BioInfectCityUnit, GEV_PRI_Primary, &s_BioInfectCityUnitEvent);
	g_gevManager->AddCallback(GEV_PlagueCityUnit, GEV_PRI_Primary, &s_PlagueCityUnitEvent);
	g_gevManager->AddCallback(GEV_NanoInfectCityUnit, GEV_PRI_Primary, &s_NanoInfectCityUnitEvent);
	g_gevManager->AddCallback(GEV_ConvertCityUnit, GEV_PRI_Primary, &s_ConvertCityUnitEvent);
	g_gevManager->AddCallback(GEV_ReformCityUnit, GEV_PRI_Primary, &s_ReformCityUnitEvent);
	g_gevManager->AddCallback(GEV_CreateParkUnit, GEV_PRI_Primary, &s_CreateParkUnitEvent);
	g_gevManager->AddCallback(GEV_InjoinUnit, GEV_PRI_Primary, &s_InjoinUnitEvent);

	g_gevManager->AddCallback(GEV_NukeCityUnit, GEV_PRI_Primary, &s_NukeCityUnitEvent);
	g_gevManager->AddCallback(GEV_NukeLocationUnit, GEV_PRI_Primary, &s_NukeLocationUnitEvent);
	g_gevManager->AddCallback(GEV_DisbandUnit, GEV_PRI_Primary, &s_DisbandUnitEvent);
	g_gevManager->AddCallback(GEV_LaunchUnit, GEV_PRI_Primary, &s_LaunchUnitEvent);
	g_gevManager->AddCallback(GEV_SetTarget, GEV_PRI_Primary, &s_SetTargetEvent);
	g_gevManager->AddCallback(GEV_ClearTarget, GEV_PRI_Primary, &s_ClearTargetEvent);
	g_gevManager->AddCallback(GEV_MADLaunch, GEV_PRI_Primary, &s_MADLaunchEvent);

	g_gevManager->AddCallback(GEV_ActivateAllUnits, GEV_PRI_Primary, &s_ActivateAllUnitsEvent);

	g_gevManager->AddCallback(GEV_SetUnloadMovementUnit, GEV_PRI_Primary, &s_SetUnloadMovementUnitEvent);
}

void unitevent_Cleanup()
{
}
