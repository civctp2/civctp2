#include "c3.h"

#include "Events.h"
#include "CityEvent.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "Player.h"
#include "Director.h"
#include "SlicEngine.h"
#include "SlicSegment.h"
#include "SlicObject.h"
#include "QuickSlic.h"
#include "AICause.h"
#include "SelItem.h"
#include "SlicObject.h"
#include "RandGen.h"
#include "ConstDB.h"
#include "gamesounds.h"
#include "UnitData.h"
#include "CityData.h"
#include "UnitDynArr.h"
#include "CityData.h"
#include "BldQue.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "ProfileDB.h"
#include "network.h"
#include "net_info.h"

#include "buildingutil.h"
#include "wonderutil.h"
#include "UnitRecord.h"
#include "UnitPool.h"
#include "World.h"
#include "Cell.h"
#include "TiledMap.h"
#include "unitutil.h"
#include "ArmyData.h"
#include "GaiaController.h"


extern void player_ActivateSpaceButton(sint32 pl);

STDEHANDLER(CaptureCityEvent)
{
	Unit city;
	sint32 newOwner;
	sint32 cause;
	MapPoint pos;
	sint32 originalOwner;

	if(!args->GetCity(0, city)) 
		return GEV_HD_Continue;

	if(!args->GetPlayer(0, newOwner)) 
		return GEV_HD_Continue;

	if(!args->GetInt(0, cause)) 
		return GEV_HD_Continue;

	originalOwner = city.GetOwner();
	city.GetPos(pos);

	city.ResetCityOwner(newOwner, TRUE, (CAUSE_REMOVE_CITY)cause);
	
	if(city.GetData()->GetCityData()->PopCount() < 1) {
		
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillCity,
			GEA_City, city,
			GEA_Int, CAUSE_REMOVE_ARMY_ATTACKED, 
			GEA_Player, newOwner, 
			GEA_End);
	} else {
		if(city.GetOwner() == g_selected_item->GetVisiblePlayer()) {
			g_director->AddCenterMap(pos);
		}
		
		if(newOwner == g_selected_item->GetVisiblePlayer())
			g_selected_item->SetSelectCity(city);

		SlicObject *so = new SlicObject("20IAFreeSlaves");
		
		
		if (city.AccessData()->CountSlaves() > 0) {
			so->AddRecipient(newOwner);
			so->AddCity(city);
			g_slicEngine->Execute(so);
		}
		
		so = new SlicObject("911CityNewOwner");
		if(newOwner > 0 && originalOwner > 0 && city.IsValid()) {
			so->AddRecipient(originalOwner);
			so->AddPlayer(originalOwner);
			so->AddPlayer(newOwner);
			so->AddCity(city);
			g_slicEngine->Execute(so);
		}		
		
		if(g_rand->Next(100) < 
		   g_theConstDB->CaptureCityAdvanceChance() * 100) {
			
			
		}
		Assert(g_player[newOwner]); 
		g_player[newOwner]->FulfillCaptureCityAgreement(city) ;	
		g_slicEngine->RunCityCapturedTriggers(newOwner, originalOwner,
											  city);
		
		if(city.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()))
   		{
			sint32 soundID = gamesounds_GetGameSoundID(GAMESOUNDS_CITYCONQUERED);
			if (soundID != 0)
				g_director->AddPlaySound(soundID, city.RetPos());
		}
	}
	return GEV_HD_Continue;
}

STDEHANDLER(CityTurnPreProductionEvent)
{
	Unit city;
	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

	city.CalcHappiness(g_player[city.GetOwner()]->m_virtualGoldSpent, TRUE);
	city.CheckRiot();
	return GEV_HD_Continue;
}

STDEHANDLER(CityBeginTurnEvent)
{
	Unit city;
	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

    static UnitDynamicArray dead;
	dead.Clear();

	city.BeginTurnCity(dead);
	dead.KillList(CAUSE_REMOVE_ARMY_UNKNOWN, -1);
	return GEV_HD_Continue;
}

STDEHANDLER(CityBeginTurnVisionEvent)
{
	Unit city;
	sint32 player;

	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	city.BeginTurnVision(player);
	return GEV_HD_Continue;
}

STDEHANDLER(CityBuildFrontEvent)
{
	Unit city;
	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

	city.CD()->BuildFront();

    
    
    if (city.CD()->GetBuildQueue()->m_settler_pending) {
        if (city.CD()->PopCount() == 1) {
            SlicObject *so = new SlicObject("111BuildingSettlerCityOfOne");
            so->AddCity(city);
			so->AddUnitRecord(city.CD()->GetBuildQueue()->GetHead()->m_type);
            so->AddRecipient(city.GetOwner());
            g_slicEngine->Execute(so);
        }
    }

	return GEV_HD_Continue;
}

STDEHANDLER(CityCreateUnitEvent)
{
	Unit homeCity;
	Unit unit;
	if(!args->GetCity(0, homeCity)) {
		
		return GEV_HD_Continue;
	}

	if(!args->GetUnit(0, unit))
		return GEV_HD_Continue;

	homeCity.CD()->GetBuildQueue()->FinishCreatingUnit(unit);

	return GEV_HD_Continue;
}

STDEHANDLER(CityBuildUnitEvent)
{
	Unit city;
	sint32 type;

	if(!args->GetCity(0, city)) return GEV_HD_Continue;
	if(!args->GetInt(0, type)) return GEV_HD_Continue;

	city.BuildUnit(type);
	return GEV_HD_Continue;
}

STDEHANDLER(CityBuildBuildingEvent)
{
	Unit city;
	sint32 type;
	if(!args->GetCity(0, city)) return GEV_HD_Continue;
	if(!args->GetInt(0, type)) return GEV_HD_Continue;

	city.BuildImprovement(type);
	return GEV_HD_Continue;
}

STDEHANDLER(CityBuildWonderEvent)
{
	Unit city;
	sint32 type;
	if(!args->GetCity(0, city)) return GEV_HD_Continue;
	if(!args->GetInt(0, type)) return GEV_HD_Continue;

	city.BuildWonder(type);
	return GEV_HD_Continue;
}

STDEHANDLER(ZeroProductionEvent)
{
	Unit city;
	if(!args->GetCity(0, city)) return GEV_HD_Continue;

	city.CD()->SetShieldstore(0);
	return GEV_HD_Continue;
}

STDEHANDLER(MakePopEvent)
{
	Unit city;
	sint32 origPlayer;

	if(!args->GetCity(0, city)) return GEV_HD_Continue;
	if(!args->GetPlayer(0, origPlayer))
		origPlayer = -1;

	city.CD()->ChangePopulation(1);
	if (origPlayer >= 0) {
		city.CD()->AddSlaveBit(origPlayer);
		city.CD()->ChangeSpecialists(POP_SLAVE, 1);
	}
	return GEV_HD_Continue;
}

STDEHANDLER(KillPopEvent)
{
	Unit city;

	if(!args->GetCity(0, city)) return GEV_HD_Continue;

	city.CD()->ChangePopulation(-1);

	return GEV_HD_Continue;
}
	
STDEHANDLER(FinishUprisingEvent)
{
	Army sa;
	Unit city;
	sint32 cause;

	if(!args->GetCity(0, city)) return GEV_HD_Continue;
	if(!args->GetArmy(0, sa)) return GEV_HD_Continue;
	if(!args->GetInt(0, cause)) return GEV_HD_Continue;

	city.CD()->FinishUprising(sa, UPRISING_CAUSE(cause));
	return GEV_HD_Continue;
}

STDEHANDLER(CleanupUprisingEvent)
{
	Army sa;
	Unit city;

	args->GetArmy(0, sa); 
	if(!args->GetCity(0, city)) {
		if(sa.IsValid()) {
			sa->DecrementDontKillCount();
		}
		return GEV_HD_Continue;
	}

	city.CD()->CleanupUprising(sa);
	if(sa.IsValid()) {
		sa->DecrementDontKillCount();
	}
	return GEV_HD_Continue;
}

STDEHANDLER(NukeCityEvent)
{
	Unit c;
	sint32 nuker;

	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetPlayer(0, nuker)) return GEV_HD_Continue;

	
	if(g_player[c.GetOwner()]) {
		sint32 i;

		for(i = 0; i < g_player[c.GetOwner()]->m_all_units->Num(); i++) {
			
			Unit u = g_player[c.GetOwner()]->m_all_units->Access(i);

			if(!u.GetDBRec()->GetNuclearAttack())
				continue;

			if(!g_theUnitPool->IsValid(u->GetTargetCity()))
				continue;

			if(u->GetTargetCity().GetOwner() != nuker)
				continue;
			
			unitutil_ExecuteMadLaunch(u);
		}
	}

	static UnitDynamicArray killList;
	killList.Clear();

	if(g_network.IsHost() && nuker == g_selected_item->GetCurPlayer()) {
		
		g_network.Block(nuker);
	}
	c.GetNuked(killList);
	if(g_network.IsHost() && nuker == g_selected_item->GetCurPlayer()) {
		g_network.Unblock(nuker);
	}

	sint32 j;
	for(j = 0; j < killList.Num(); j++) {
		if(killList[j].DeathEffectsHappy()) {
			g_player[killList[j].GetOwner()]->RegisterLostUnits(1, c.RetPos(), DEATH_EFFECT_CALC);
		}
	}

	for(j = 0; j < killList.Num(); j++) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
							   GEA_Unit, killList[j],
							   GEA_Int, CAUSE_REMOVE_ARMY_NUKE,
							   GEA_Player, nuker,
							   GEA_End);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(MakeFranchiseEvent)
{
	Unit c;
	sint32 p;

	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetPlayer(0, p)) return GEV_HD_Continue;

	c->MakeFranchise(p);
	return GEV_HD_Continue;
}

STDEHANDLER(SlaveRaidCityEvent)
{
	
	
	Unit city;
	if(!args->GetCity(0, city)) return GEV_HD_Continue;

	city.CD()->ChangePopulation(-1);

	return GEV_HD_Continue;
}

STDEHANDLER(BioInfectCityEvent)
{
	Unit c;
	sint32 pl;

	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	c.BioInfect(pl);
	return GEV_HD_Continue;
}

STDEHANDLER(PlagueCityEvent)
{
	Unit c;
	sint32 pl;

	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	c.CD()->Plague(pl);
	return GEV_HD_Continue;
}

STDEHANDLER(NanoInfectCityEvent)
{
	Unit c;
	sint32 pl;

	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	c.NanoInfect(pl);
	return GEV_HD_Continue;
}

STDEHANDLER(ConvertCityEvent)
{
	Unit c;
	sint32 pl;
	sint32 by;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;
	if(!args->GetInt(0, by)) return GEV_HD_Continue;

	c.ConvertTo(pl, (CONVERTED_BY)by);
	return GEV_HD_Continue;
}

STDEHANDLER(UnconvertCityEvent)
{
	Unit c;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	
	c.Unconvert();
	return GEV_HD_Continue;
}

STDEHANDLER(AddHappyTimerEvent)
{
	Unit c;
	sint32 turns;
	sint32 amount;
	sint32 reason;

	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetInt(0, turns)) return GEV_HD_Continue;
	if(!args->GetInt(1, amount)) return GEV_HD_Continue;
	if(!args->GetInt(2, reason)) return GEV_HD_Continue;

	c.AddHappyTimer(turns, amount, (HAPPY_REASON)reason);
	return GEV_HD_Continue;
}

STDEHANDLER(CreateParkEvent)
{
	Unit c;
	sint32 pl;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	c.CityToPark(pl);

	return GEV_HD_Continue;
}

STDEHANDLER(InjoinCityEvent)
{
	Unit c;
	sint32 pl;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	c.Injoin(pl);
	
	return GEV_HD_Continue;
}

STDEHANDLER(CreateBuildingEvent)
{
	Unit c;
	sint32 building, player, i;
	SlicObject *so;
	SlicSegment *seg;

	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetInt(0, building)) return GEV_HD_Continue;

	c.CD()->AddImprovement(building);

	Unit u;
	c.CD()->GetBuildQueue()->FinishBuildFront(u);

	player = c.GetOwner();
	if(!g_player[player]) {
		return GEV_HD_Continue;
	}
	if(g_player[player]->GetGaiaController()->HasMaxSatsBuilt()) {
		so = new SlicObject("GCMaxSatsReached");
		seg = g_slicEngine->GetSegment("GCMaxSatsReached");
		if(seg && !seg->TestLastShown(player, 10000)) {
			so->AddRecipient(player);
			so->AddPlayer(player);
			g_slicEngine->Execute(so);
		}		
	}

	if(g_player[player]->GetGaiaController()->HasMinSatsBuilt()) {
		so = new SlicObject("GCMinSatsReachedUs");
		seg = g_slicEngine->GetSegment("GCMinSatsReachedUs");
		if(seg && !seg->TestLastShown(player, 10000)) {
			so->AddRecipient(player);
			so->AddPlayer(player);
			g_slicEngine->Execute(so);
		
			so = new SlicObject("GCMinSatsReachedThem");
			seg = g_slicEngine->GetSegment("GCMinSatsReachedThem");
			for(i = 1; i < g_theProfileDB->GetMaxPlayers(); i++) {
				if(!seg->TestLastShown(i, 10000) && i != player) {
					so->AddRecipient(i);
					so->AddPlayer(player);
				}
			}
			g_slicEngine->Execute(so);
		}
	}

	if(g_player[player]->GetGaiaController()->HasMinCoresBuilt()) {
		so = new SlicObject("GCMinCoresReachedUs");
		seg = g_slicEngine->GetSegment("GCMinCoresReachedUs");
		if(seg && !seg->TestLastShown(player, 10000)) {
			so->AddRecipient(player);
			so->AddPlayer(player);
			g_slicEngine->Execute(so);
	
			so = new SlicObject("GCMinCoresReachedThem");
			seg = g_slicEngine->GetSegment("GCMinCoresReachedThem");
			for(i = 1; i < g_theProfileDB->GetMaxPlayers(); i++) {
				if(seg && !seg->TestLastShown(i, 10000) && i != player) {
					so->AddRecipient(i);
					so->AddPlayer(player);
				}
			}
		}
		g_slicEngine->Execute(so);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(CreateWonderEvent)
{
	Unit c;
	sint32 wonder;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetInt(0, wonder)) return GEV_HD_Continue;

	c.CD()->SetWonders(c.CD()->GetBuiltWonders() | ((uint64)1 << (uint64)wonder));
	wonderutil_AddBuilt(wonder);
	g_player[c->GetOwner()]->AddWonder(wonder, c);
	g_player[c->GetOwner()]->RegisterCreateWonder(c, wonder);

	
	if (c->GetOwner() == g_selected_item->GetVisiblePlayer() &&
		!Player::IsThisPlayerARobot(c->GetOwner())) {
		
		if ( g_theProfileDB->IsWonderMovies() ) {
			if (g_director) {
				g_director->AddPlayWonderMovie(c.CD()->GetBuildQueue()->GetHead()->m_type);
			}
		}
		
	}
	if(g_network.IsHost()) {
		g_network.Block(c.GetOwner());
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_WONDER_BUILT,
									  c.CD()->GetBuildQueue()->GetHead()->m_type, (uint32)c.m_id));
		g_network.Unblock(c.GetOwner());
	}

	Unit u;
	c.CD()->GetBuildQueue()->FinishBuildFront(u);
	SlicObject *so;
	if(wonder == wonderutil_GetFobCityIndex()) {
		so = new SlicObject("911ForbiddenCityPeace");
		so->AddRecipient(c.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);
	}

	if(wonder == wonderutil_GetGaiaIndex()) {
		so = new SlicObject("GCMustDiscoverGaiaController");
		for(sint32 i = 1; i < g_theProfileDB->GetMaxPlayers(); i++) {
			if(g_player[i] && i != c->GetOwner()) {
				so->AddRecipient(i);
				so->AddPlayer(i);
				so->AddPlayer(c.GetOwner());
				g_slicEngine->Execute(so);
			}
		}
	}

	return GEV_HD_Continue;
}

STDEHANDLER(RushBuyEvent)
{
	Unit c;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	c.CD()->BuyFront();
	return GEV_HD_Continue;
}

STDEHANDLER(DisbandCityEvent)
{
	Unit c;
	if(!args->GetCity(0, c)) return GEV_HD_Continue;

	c.DisbandCity();
	return GEV_HD_Continue;
}

STDEHANDLER(SellBuildingEvent)
{
	Unit c;
	sint32 b;

	if(!args->GetCity(0, c)) return GEV_HD_Continue;
	if(!args->GetInt(0, b)) return GEV_HD_Continue;

	c.CD()->SellBuilding(b, TRUE);
	return GEV_HD_Continue;
}

STDEHANDLER(KillTileEvent)
{
	MapPoint pos;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	Cell *cell = g_theWorld->GetCell(pos);
	if(cell->GetCanDie()) {
		cell->Kill();
		
		g_theWorld->CutImprovements(pos);
		
		cell->CalcTerrainMoveCost();
		g_tiledMap->PostProcessTile(pos, g_theWorld->GetTileInfo(pos));
		g_tiledMap->TileChanged(pos);
		MapPoint npos;
		for(WORLD_DIRECTION d = NORTH; d < NOWHERE; 
			d = (WORLD_DIRECTION)((sint32)d + 1)) {
			if(pos.GetNeighborPosition(d, npos)) {
				g_tiledMap->PostProcessTile(
					npos, 
					g_theWorld->GetTileInfo(npos));
				g_tiledMap->TileChanged(npos);
			}
		}
		g_tiledMap->RedrawTile(&pos);
	}
	return GEV_HD_Continue;
}
	
void cityevent_Initialize()
{
	g_gevManager->AddCallback(GEV_CaptureCity, GEV_PRI_Primary, &s_CaptureCityEvent);	

	g_gevManager->AddCallback(GEV_CityTurnPreProduction, GEV_PRI_Primary, &s_CityTurnPreProductionEvent);
	g_gevManager->AddCallback(GEV_CityBeginTurn, GEV_PRI_Primary, &s_CityBeginTurnEvent);
	g_gevManager->AddCallback(GEV_CityBeginTurnVision, GEV_PRI_Primary, &s_CityBeginTurnVisionEvent);
	g_gevManager->AddCallback(GEV_BuildFront, GEV_PRI_Primary, &s_CityBuildFrontEvent);
	g_gevManager->AddCallback(GEV_CreateUnit, GEV_PRI_Post, &s_CityCreateUnitEvent);

	g_gevManager->AddCallback(GEV_BuildUnit, GEV_PRI_Primary, &s_CityBuildUnitEvent);
	g_gevManager->AddCallback(GEV_BuildBuilding, GEV_PRI_Primary, &s_CityBuildBuildingEvent);
	g_gevManager->AddCallback(GEV_BuildWonder, GEV_PRI_Primary, &s_CityBuildWonderEvent);
	g_gevManager->AddCallback(GEV_ZeroProduction, GEV_PRI_Primary, &s_ZeroProductionEvent);

	g_gevManager->AddCallback(GEV_MakePop, GEV_PRI_Primary, &s_MakePopEvent);
	g_gevManager->AddCallback(GEV_KillPop, GEV_PRI_Primary, &s_KillPopEvent);

	g_gevManager->AddCallback(GEV_FinishUprising, GEV_PRI_Primary, &s_FinishUprisingEvent);
	g_gevManager->AddCallback(GEV_CleanupUprising, GEV_PRI_Primary, &s_CleanupUprisingEvent);

	g_gevManager->AddCallback(GEV_NukeCity, GEV_PRI_Primary, &s_NukeCityEvent);
	g_gevManager->AddCallback(GEV_MakeFranchise, GEV_PRI_Primary, &s_MakeFranchiseEvent);
	g_gevManager->AddCallback(GEV_SlaveRaidCity, GEV_PRI_Primary, &s_SlaveRaidCityEvent);
	g_gevManager->AddCallback(GEV_BioInfectCity, GEV_PRI_Primary, &s_BioInfectCityEvent);
	g_gevManager->AddCallback(GEV_PlagueCity, GEV_PRI_Primary, &s_PlagueCityEvent);
	g_gevManager->AddCallback(GEV_NanoInfectCity, GEV_PRI_Primary, &s_NanoInfectCityEvent);
	g_gevManager->AddCallback(GEV_ConvertCity, GEV_PRI_Primary, &s_ConvertCityEvent);
	g_gevManager->AddCallback(GEV_UnconvertCity, GEV_PRI_Primary, &s_UnconvertCityEvent);
	g_gevManager->AddCallback(GEV_AddHappyTimer, GEV_PRI_Primary, &s_AddHappyTimerEvent);
	g_gevManager->AddCallback(GEV_CreatePark, GEV_PRI_Primary, &s_CreateParkEvent);
	g_gevManager->AddCallback(GEV_InjoinCity, GEV_PRI_Primary, &s_InjoinCityEvent);

	g_gevManager->AddCallback(GEV_CreateBuilding, GEV_PRI_Primary, &s_CreateBuildingEvent);
	g_gevManager->AddCallback(GEV_CreateWonder, GEV_PRI_Primary, &s_CreateWonderEvent);

	g_gevManager->AddCallback(GEV_BuyFront, GEV_PRI_Primary, &s_RushBuyEvent);
	g_gevManager->AddCallback(GEV_DisbandCity, GEV_PRI_Primary, &s_DisbandCityEvent);
	g_gevManager->AddCallback(GEV_SellBuilding, GEV_PRI_Primary, &s_SellBuildingEvent);

	g_gevManager->AddCallback(GEV_KillTile, GEV_PRI_Primary, &s_KillTileEvent);

}

void cityevent_Cleanup()
{
}
