//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Unit
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
// - IsValid marked as const.
// - AddDisplayName added.
// - PFT 29 mar 05, show # turns until city next grows a pop
// - Made GetFuel method const - April 24th 2005 Martin Gühmann
// - Added NeedsRefueling method to remove code duplications.
//   - April 24th 2005 Martin Gühmann
// - Moved UnitValidForOrder from ArmyData to be able to access the Unit
//   properties as well. - April 24th 2005 Martin Gühmann
// - Implemented GovernmentModified for the UnitDB.  - April 24th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#include "c3.h"
 


#include "Globals.h"
#include "GWRecord.h"

#include "ConstDB.h"

#include "StrDB.h"

#include "DB.h"
#include "UnitRec.h"

#include "DynArr.h"
#include "Unit.h"
#include "UnitDynArr.h"

#include "CityData.h"

#include "CellUnitList.h"

#include "Player.h"
#include "Pollution.h"

#include "XY_Coordinates.h"
#include "World.h"

#include "UnitData.h"

#include "network.h"
#include "net_info.h"

#include "UnitPool.h"
#include "RandGen.h"

#include "aui.h"
#include "primitives.h"

#include "Director.h"
#include "tech_WLList.h"
#include "Readiness.h"

#include "AiCause.h"

#include "SlicEngine.h"
#include "HappyTracker.h"
#include "Cell.h"

#include "TradeOfferPool.h"
#include "ArmyPool.h"
#include "ArmyData.h"
#include "TradeBids.h"
#include "Cell.h"

#include "SelItem.h"

#include "WonderRecord.h"

extern UnitPool *g_theUnitPool; 
extern World *g_theWorld; 
extern Player **g_player;
extern ConstDB *g_theConstDB; 
extern StringDB *g_theStringDB; 
extern RandomGenerator *g_rand ;
extern Pollution *g_thePollution ;
extern Director *g_director; 

extern SelectedItem *g_selected_item;




#include "gamesounds.h"
#include "SpecialAttackInfoRecord.h"
#include "SpecialEffectRecord.h"

#include "GameEventManager.h"
#include "unitutil.h"
#include "CtpAi.h"


#include "gamefile.h"

void Unit::KillUnit (const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy)

{
	
	UnitData *u = g_theUnitPool->GetUnit(m_id) ;
	g_player[u->GetOwner()]->AdjustEventPollution(GetDBRec()->GetLaunchPollution()) ;
	
    Unit tmp(m_id); 
    tmp.RemoveAllReferences(cause, killedBy); 
                                
}

void Unit::RemoveAllReferences(const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy)

{
    BOOL is_renumber_cont = FALSE; 

	
	

    if (IsCity()) {
        AccessData()->GetCityData()->PrepareToRemove(cause, killedBy);
    }

	if(GetArmy().IsValid()) {
		GetArmy().SetRemoveCause(cause);
	}

	sint32 r=TRUE;
	
	switch(cause) {
		case CAUSE_REMOVE_ARMY_ATTACKED:
		case CAUSE_REMOVE_ARMY_DIED_IN_ATTACK:
		case CAUSE_REMOVE_ARMY_DIED_IN_ATTACK_ON_TOP:
		case CAUSE_REMOVE_ARMY_SLAVE_UPRISING:
		case CAUSE_REMOVE_ARMY_UPRISING:
		case CAUSE_REMOVE_ARMY_BOMBARD:
			Assert(killedBy >= 0);
			if(killedBy >= 0) {
				g_slicEngine->RunUnitDeadTriggers(*this, killedBy);
			}
			break;
		case CAUSE_REMOVE_ARMY_OUTOFFUEL:
			g_slicEngine->RunOutOfFuelTriggers(*this);
			break;
		case CAUSE_REMOVE_ARMY_NO_MAT_SUPPORT:
			g_slicEngine->RunUnitCantBeSupportedTriggers(*this);
			break;
		default:
			g_slicEngine->RunMiscUnitDeathTriggers(*this);
			break;
	}
	
	DPRINTF(k_DBG_GAMESTATE, ("Unit::RemoveAllReferences: id: %d",
							  m_id));
	if(g_network.IsActive()) {
		if(g_network.IsHost()) {
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_KILL_UNIT, m_id));
		} else {
			g_network.AddDeadUnit(m_id);
		}
	}
	
	if(GetActor() != NULL) {
		if (cause == CAUSE_REMOVE_ARMY_PARADROP_COMPLETE 
			|| IsBeingTransported()
			|| cause == CAUSE_REMOVE_ARMY_DIED_IN_ATTACK
			|| cause == CAUSE_REMOVE_ARMY_ATTACKED
			|| cause == CAUSE_REMOVE_ARMY_SETTLE
			|| cause == CAUSE_REMOVE_ARMY_DISBANDED 
			|| cause == CAUSE_REMOVE_ARMY_GOVERNMENT_CHANGE
			|| cause == CAUSE_REMOVE_ARMY_NUKE
			|| cause == CAUSE_REMOVE_ARMY_PARKRANGER) {
			g_director->AddFastKill(*this);
		} else {
			
			
			
			
			
				g_director->AddDeath(*this);
				
		}
	} else {
		sint32 xx=1;
	}
	
	if(!g_theUnitDB->Get(GetType(), g_player[GetOwner()]->GetGovernmentType())->GetIsTrader()) {
		AccessData()->KillVision();
	}
	
	MapPoint pos;
	GetPos(pos);
	PLAYER_INDEX owner = GetOwner(); 
	
	if(!g_theUnitDB->Get(GetType(), g_player[GetOwner()]->GetGovernmentType())->GetIsTrader() && !IsTempSlaveUnit() &&
		!IsBeingTransported() && !HasLeftMap()) {
		
		r = g_theWorld->RemoveUnitReference(pos, *this);
		Assert(r);

		
		
		











	}
	
	if(!IsTempSlaveUnit()) {
		
		r = g_player[owner]->RemoveUnitReference(*this, cause, killedBy);
		Assert(r); 
	}
	
	Unit transport = GetTransport();
	if(transport.IsValid()) {
		transport.RemoveTransportedUnit(*this);
	}

	if(GetDBRec()->GetHasPopAndCanBuild()) {
		if (cause != CAUSE_REMOVE_ARMY_POLLUTION 
			&& cause != CAUSE_REMOVE_ARMY_FLOOD) { 
			g_theWorld->SetCanalTunnel(pos, 0);
            is_renumber_cont = TRUE; 
		}
		
		Cell *cell = g_theWorld->GetCell(pos);

		
		
		g_theWorld->CutImprovements(pos);
		







		if(g_network.IsHost()) {
			g_network.Enqueue(cell,
							  pos.x, pos.y);
		}
		g_theTradeOfferPool->RemoveTradeOffersFromCity(*this);
		g_theTradeBids->CancelBidsWithCity(*this);

		if(cell->UnitArmy()) {
			sint32 i;
			if(cause != CAUSE_REMOVE_ARMY_NUKE) {
				
				

				for(i = cell->UnitArmy()->Num() - 1; i >= 0; i--) {
					if(!g_theWorld->CanEnter(pos, cell->UnitArmy()->Access(i).GetMovementType())) {
						cell->UnitArmy()->Access(i).Kill(CAUSE_REMOVE_ARMY_ILLEGAL_CELL, -1);
					}
				}
			}
		}

		CD()->RemoveBorders();

		g_selected_item->RegisterRemovedCity(GetOwner(), *this);

	}

	AccessData()->KillTransportedUnits();

	if(GetArmy().IsValid()) {
		GetArmy().SetKiller(killedBy);
		GetArmy().Del(*this);
	}

	

	if(!g_theUnitDB->Get(GetType())->GetIsTrader(), g_player[GetOwner()]->GetGovernmentType()) {
		UpdateZOCForRemoval();
	}
			   
	
	r = g_theUnitPool->Del(m_id); 

    if (is_renumber_cont)  { 
       g_theWorld->NumberContinents(); 
    }
	
	Assert(r); 
}



void Unit::FastKill()
{
	sint32 r;
	if(GetActor()) {
		if(g_director)
			g_director->FastKill(GetActor());
		AccessData()->SetActor(NULL);
	} else {
		Assert(FALSE);
	}

	r = g_theUnitPool->Del(m_id);
	Assert(r);
}

BOOL Unit::IsValid() const
{
	return g_theUnitPool->IsValid(*this);
}

sint32 Unit::GetShieldHunger() const

{
    return GetDBRec()->GetShieldHunger(); 
}

sint32 Unit::GetFoodHunger() const

{
    return GetDBRec()->GetFoodHunger(); 
}

const UnitRecord * Unit::GetDBRec(void) const

{
   UnitData *ptr = g_theUnitPool->GetUnit(m_id);
   return g_theUnitDB->Get(ptr->GetType(), g_player[ptr->GetOwner()]->GetGovernmentType()); 
}

const UnitRecord * Unit::GetDBRec(UnitData *u) const
{
   return g_theUnitDB->Get(u->GetType(), g_player[u->GetOwner()]->GetGovernmentType()); 
}

const UnitData * Unit::GetData() const 

{
   return g_theUnitPool->GetUnit(m_id);
}

UnitData * Unit::AccessData() const
{
   return g_theUnitPool->AccessUnit(m_id);
}

const MBCHAR * Unit::GetName() const
{
	if(GetDBRec()->GetHasPopAndCanBuild()) {
		return GetData()->GetCityData()->GetName();
	}
	StringId id = GetDBRec()->m_name;
	return g_theStringDB->GetNameStr(id);
}

//----------------------------------------------------------------------------
//
// Name       : Unit::GetDisplayName
//
// Description: Get display name (string) of a unit.
//
// Parameters : -
//
// Globals    : g_theStringDB   : string database
//              g_player        : player information
//
// Returns    : std::string     : text to display as name
//
// Remark(s)  : Differs from GetName only for units that have been marked as
//              leader in unit.txt. In this case, the player name is appended 
//              to the unit name.
//
//----------------------------------------------------------------------------
std::string Unit::GetDisplayName(void) const
{
	UnitRecord const *	info	= GetDBRec();
	Assert(info);

	if (info->GetHasPopAndCanBuild())
	{
		return GetData()->GetCityData()->GetName();
	}
	else
	{
		std::string	unitName(g_theStringDB->GetNameStr(info->m_name));
		return info->GetLeader() 
			   ? unitName + " " + g_player[GetOwner()]->GetLeaderName() 
			   : unitName;
	}
}

PLAYER_INDEX Unit::GetOwner() const

{ 
   return g_theUnitPool->GetUnit(m_id)->GetOwner(); 
} 

sint32 Unit::GetType() const
{ 
   return g_theUnitPool->GetUnit(m_id)->GetType(); 
} 


void Unit::GetPos(MapPoint &pos) const 

{
   UnitData *ptr = g_theUnitPool->GetUnit(m_id); 
   ptr->GetPos(pos); 
}

MapPoint Unit::RetPos() const 

{
	MapPoint pos;
	UnitData *ptr = g_theUnitPool->GetUnit(m_id); 
	ptr->GetPos(pos);
	return pos;
}


double Unit::GetHPModifier()

{ 
    return g_player[GetOwner()]->GetHPModifier(); 
} 

double Unit::GetHP() const 
{
   return g_theUnitPool->GetUnit(m_id)->GetHP(); 
}

uint32 Unit::GetOwnerBit() const

{
     return g_theUnitPool->GetUnit(m_id)->GetOwnerBit(); 
}


sint32 Unit::IsIgnoresZOC() const
{
   return GetDBRec()->GetIgnoreZOC(); 
}












BOOL Unit::NearestFriendlyCity(MapPoint &p) const
	{
	MapPoint	unit_pos,
				city_pos ;

	sint32	i,
			n ;

	uint32	closest_distance = 0xFFFFFFFF ;							

	GetPos(unit_pos) ;												
	p = unit_pos ;
	n = g_player[GetOwner()]->m_all_cities->m_nElements ;
	for (i=0; i<n; i++)
		{
		g_player[GetOwner()]->m_all_cities->Get(i).GetPos(city_pos) ;	

		
		uint32	d = max(abs(city_pos.x - unit_pos.x), abs(city_pos.y - unit_pos.y)) ;	

		
		if (d<closest_distance)
			{
			
			closest_distance = d ;
			p = city_pos ;
			}

		}

	return closest_distance < 0xffffffff;
	}

BOOL Unit::NearestFriendlyCityWithRoom(MapPoint &p, sint32 needRoom,
									   Army *army) const
{
	MapPoint	unit_pos,
				city_pos ;

	sint32	i,
			n ;

	
	uint32	closest_distance = 0xFFFFFFFF ;

	
	GetPos(unit_pos) ;
	p = unit_pos ;
	n = g_player[GetOwner()]->m_all_cities->m_nElements ;
	for (i=0; i<n; i++)	{
		
		g_player[GetOwner()]->m_all_cities->Get(i).GetPos(city_pos) ;
		
		
		uint32	d = max(abs(city_pos.x - unit_pos.x), abs(city_pos.y - unit_pos.y)) ;
		sint32 numUnits = g_theWorld->GetCell(city_pos)->GetNumUnits();
		if(numUnits + needRoom > k_MAX_ARMY_SIZE)
			continue;

		if(army && 
		   (army->IsAtLeastOneMoveWater() || army->IsAtLeastOneMoveShallowWater())) {
			if(!g_theWorld->IsNextToWater(city_pos.x, city_pos.y) && 
			   !g_theWorld->IsWater(city_pos))
				continue;
		}

		
		if (d<closest_distance)	{
			
			closest_distance = d ;
			p = city_pos ;
		}
	}

	return closest_distance < 0xffffffff;
}











BOOL Unit::NearestFriendlyCity(Unit &c) const
	{
	MapPoint	unit_pos,
				city_pos ;

	sint32	i,
			n ;

	uint32	closest_distance = 0xFFFFFFFF ;							

	c.m_id = (0) ;
	GetPos(unit_pos) ;												
	n = g_player[GetOwner()]->m_all_cities->m_nElements ;
	for (i=0; i<n; i++)
		{
		g_player[GetOwner()]->m_all_cities->Get(i).GetPos(city_pos) ;	

		
		uint32	d = max(abs(city_pos.x - unit_pos.x), abs(city_pos.y - unit_pos.y)) ;	

		
		if (d<closest_distance)
			{
			
			closest_distance = d ;
			c = g_player[GetOwner()]->m_all_cities->Get(i).m_id ;
			}

		}

	return closest_distance < 0xffffffff;
	}











void Unit::Launch()
	{
	UnitData *u = g_theUnitPool->GetUnit(m_id) ;
	g_player[u->GetOwner()]->AdjustEventPollution(GetDBRec()->GetLaunchPollution()) ;
	}














sint32 Unit::MoveToPosition(const MapPoint &p, UnitDynamicArray &revealed, 
                            BOOL &revealed_unexplored)
{


















	
    return (SetPosition(p, revealed, revealed_unexplored)) ;
}


sint32 Unit::SetPosition(const MapPoint &p, UnitDynamicArray &revealed, 
                         BOOL &revealed_unexplored)

{
	BOOL left_map;
	AccessData()->SetPos(p, revealed_unexplored, left_map); 
	if(!left_map) {
		return g_theWorld->InsertUnit(p, *this, revealed); 
	} else {
		return TRUE;
	}
}

void Unit::SetPosAndNothingElse(const MapPoint &p)
{
    AccessData()->SetPosAndNothingElse(p);
}

double Unit::GetMaxMovePoints() const  
{ 
   return GetDBRec()->GetMaxMovePoints(); 
}
   
double Unit::GetMovementPoints() const

{
   const UnitData *ud=GetData();
   return ud->GetMovementPoints(); 
}

void Unit::SetMovementPoints(double m)

{
   AccessData()->SetMovementPoints(m); 
}


BOOL Unit::CanBeExpelled() const
{ 
	return GetDBRec()->GetCanBeExpelled() && (g_theWorld->GetOwner(RetPos()) != GetOwner());
}


double Unit::GetAttack() const

{ 
   return GetDBRec()->GetAttack(); 
}

double Unit::GetDefense() const
{
	return GetDBRec()->GetDefense();
}

double Unit::GetDefense(const Unit &attacker) const

{ 
   return GetData()->GetDefense(attacker); 
}

double Unit::GetActiveDefenseRange() const
{
	return GetDBRec()->GetActiveDefenseRange();
}

sint32 Unit::PayWages(sint32 w)

{
    return AccessData()->PayWages(w); 
} 

sint32 Unit::GetWagesNeeded()
{
    return AccessData()->GetWagesNeeded();
}

sint32 Unit::GetFirepower() const

{ 
   return GetDBRec()->GetFirepower(); 
}

sint32 Unit::ExertsMartialLaw() const
{ 
   return GetDBRec()->GetExertsMartialLaw(); 
}


sint32 Unit::GetFirstMoveThisTurn() const

{
  return GetData()->GetFirstMoveThisTurn(); 
}


uint32 Unit::GetMovementType() const

{
  return GetDBRec()->GetMovementType(); 

}

sint32 Unit::GetMovementTypeLand() const

{
  return GetDBRec()->GetMovementTypeLand(); 
}


sint32 Unit::GetMovementTypeSea() const

{
  return GetDBRec()->GetMovementTypeSea(); 
}

sint32 Unit::GetMovementTypeShallowWater() const

{
  return GetDBRec()->GetMovementTypeShallowWater();
}

sint32 Unit::GetMovementTypeAir() const

{
  return GetDBRec()->GetMovementTypeAir(); 
}

sint32 Unit::GetMovementTypeSpace() const

{
  return GetDBRec()->GetMovementTypeSpace(); 
}

sint32 Unit::GetMovementTypeMountain() const

{
  return GetDBRec()->GetMovementTypeMountain(); 
}

sint32 Unit::IsSubmarine() const
{
	return GetDBRec()->GetIsSubmarine();
}

sint32 Unit::IsNoZoc() const 

{ 
   return GetDBRec()->GetNoZoc(); 
}

sint32 Unit::DeductMoveCost(const double cost, BOOL &out_of_fuel)
{
   return AccessData()->DeductMoveCost(*this, cost, out_of_fuel); 

}

 
void Unit::SetIsInTransport(const Unit &transport)

{
   AccessData()->SetIsInTransport(transport); 
}

void Unit::UnsetIsInTransport()
{
    AccessData()->UnsetIsInTransport();
}

sint32 Unit::IsBeingTransported() const
{
   return GetData()->IsBeingTransported(); 
}

#if 0
void Unit::Transform()

{
   AccessData()->Transform(); 
}
#endif



BOOL Unit::CanAtLeastOneCargoUnloadAt(const MapPoint & old_pos, const MapPoint & dest_pos, const BOOL & use_vision) const
{
    return AccessData()->CanAtLeastOneCargoUnloadAt(old_pos, dest_pos, use_vision);
}

BOOL Unit::UnloadCargo(const MapPoint &new_pos, Army &debark,
					   BOOL justOneUnit, Unit &theUnit)

{
    return AccessData()->UnloadCargo(new_pos, debark, justOneUnit, theUnit); 
}

BOOL Unit::UnloadSelectedCargo(const MapPoint &new_pos, Army &debark)
{
	return AccessData()->UnloadSelectedCargo(new_pos, debark);
}

BOOL Unit::IsMovePointsEnough(const MapPoint &pos) const
{
    return GetData()->IsMovePointsEnough(pos);
}



void Unit::GetInserted(const Unit &transport)
{
	AccessData()->GetInserted(transport);
}

BOOL Unit::InsertCargo(const Unit &addme)

{
   return AccessData()->InsertCargo(addme); 
}

BOOL Unit::CanBeachAssault() const
{
    return GetDBRec()->GetCanBeachAssault(); 
}

//PFT
bool Unit::IsImmobile()const 
{
	return AccessData()->IsImmobile();
}

sint32 Unit::ResetMovement()

{
   return AccessData()->ResetMovement(); 
}

void Unit::SetHP(const double newval)

{
  AccessData()->SetHP(newval); 
}

void Unit::SetHPToMax()

{
   SetHP(GetDBRec()->GetMaxHP());
}

sint32 Unit::GetFuel() const
{ 
    return GetData()->GetFuel(); 
} 


void Unit::SetFuel (sint32 val)

{ 
    AccessData()->SetFuel(val); 
}

void Unit::SetFuelToMax()

{
    SetFuel(GetDBRec()->GetMaxFuel()); 
}

BOOL Unit::GetUsedFuel (sint32 &fuel_remaining, sint32 &max_fuel) const
{
    return GetData()->GetUsedFuel(fuel_remaining, max_fuel); 
}

sint32 Unit::IsVeteran() const 

{
   return GetData()->IsVeteran(); 
}

void Unit::SetVeteran()

{
   AccessData()->SetVeteran(); 
}

void Unit::UnVeteran()

{
   AccessData()->UnVeteran(); 
}

sint32 Unit::CanInterceptTrade() const
{
    return GetData()->CanInterceptTrade(); 
}

sint32 Unit::CanRustle(CellUnitList &defender) const
{
	return GetData()->CanRustle(defender); 
}

sint32 Unit::CanConvertCity(Unit &city) const
{
    return GetData()->CanConvertCity(city); 
}

sint32 Unit::CanBombard(CellUnitList &defender) const 

{
    return GetData()->CanBombard(defender); 
} 


sint32 Unit::CanCounterBombard(CellUnitList &defender) const 

{
    return GetData()->CanCounterBombard(defender);
}

sint32 Unit::CanActivelyDefend(Army &attacker) const
{
	CellUnitList *list = g_theArmyPool->AccessArmy(attacker);
	return GetData()->CanActivelyDefend(*list);
}

sint32 Unit::CanActivelyDefend(CellUnitList &attacker) const
{
	return GetData()->CanActivelyDefend(attacker);
}

void Unit::Bombard(const UnitRecord *rec, Unit defender, BOOL isCounterBombardment)
{
    AccessData()->Bombard(rec, defender, isCounterBombardment); 
}

BOOL Unit::Bombard(CellUnitList &defender, BOOL isCounterBombardment)

{
    return AccessData()->Bombard(defender, isCounterBombardment); 
}

void Unit::BombardOneRound(Unit &defender, double dbonus, double dmr)
{
    AccessData()->BombardOneRound(GetDBRec(), defender, dbonus, dmr); 
}

void Unit::DeductHP(const double val)

{
   AccessData()->DeductHP(val); 
}

//not used
void Unit::FightOneRound(Unit did, double defenders_bonus, double amr, double dmr) 
{
    AccessData()->FightOneRound(did, defenders_bonus, amr, dmr);
}
 
BOOL Unit::IsFlanker() const

{
    return GetDBRec()->GetIsFlanker(); 
}

sint32 Unit::GetZBRange() const

{
    return GetDBRec()->GetZBRangeAttack();
}
  
sint32 Unit::GetCargoCapacity() const

{
   return g_theUnitPool->GetUnit(m_id)->GetCargoCapacity(); 
}

void Unit::GetCargoHP(sint32 &i, sint32 unit_type[100], 
        sint32 unit_hp[100])
{
    g_theUnitPool->GetUnit(m_id)->GetCargoHP(i, unit_type, unit_hp);

}


sint32 Unit::GetNumCarried() const
{
	return GetData()->GetNumCarried();
}

BOOL Unit::DeathEffectsHappy() const
{
    return GetDBRec()->GetDeathEffectsHappy(); 
}

sint32 Unit::GetSelect1SoundID(void)
{
	return GetDBRec()->GetSoundSelect1Index();;
}

sint32 Unit::GetSelect2SoundID(void)
{
	return GetDBRec()->GetSoundSelect2Index();
}

sint32 Unit::GetMoveSoundID(void)
{
	
	if(!g_theUnitPool->IsValid(m_id))
		return 0;
	return GetDBRec()->GetSoundMoveIndex();
}

sint32 Unit::GetAcknowledgeSoundID(void)
{
	return GetDBRec()->GetSoundAcknowledgeIndex();
}

sint32 Unit::GetCantMoveSoundID(void)
{
	return GetDBRec()->GetSoundCantMoveIndex();
}

sint32 Unit::GetAttackSoundID(void)
{
	return GetDBRec()->GetSoundAttackIndex();
}








sint32 Unit::GetWorkSoundID(void)
{
	return GetDBRec()->GetSoundWorkIndex();
}

sint32 Unit::GetVictorySoundID(void)
{
	return GetDBRec()->GetSoundVictoryIndex();
}

sint32 Unit::GetDeathSoundID(void)
{
	return GetDBRec()->GetSoundDeathIndex();
}

sint32 Unit::GetLoadSoundID(void)
{
	return GetDBRec()->GetSoundLoadIndex();
}

sint32 Unit::GetUnloadSoundID(void)
{
	return GetDBRec()->GetSoundUnloadIndex();
}

BOOL Unit::GetSpecialAttackInfo(SPECATTACK attack, sint32 *soundID, sint32 *spriteID)
{
	const SpecialAttackInfoRecord *rec = unitutil_GetSpecialAttack(attack);
	if(!rec)
		return FALSE;
	*soundID = rec->GetSoundIDIndex();
	*spriteID = rec->GetSpriteID()->GetValue();
	return TRUE;
}

sint32 Unit::CanCarry(Unit u) const 

{
   return g_theUnitPool->GetUnit(m_id)->CanCarry(u.GetType()); 

}

BOOL Unit::CanSettle(const MapPoint &pos) const
{
    return GetData()->CanSettle(pos); 
} 

BOOL Unit::IsSettleLand() const
{
    return GetDBRec()->GetSettleLand(); 
} 

BOOL Unit::IsSettleMountain() const
{
    return GetDBRec()->GetSettleMountain(); 
} 

BOOL Unit::IsSettleWater() const
{
    return GetDBRec()->GetSettleWater(); 
} 

BOOL Unit::IsSettleSpace() const
{
    return GetDBRec()->GetSettleSpace(); 
} 

sint32 Unit::Settle()

{ 
   sint32 r = AccessData()->Settle(); 

   return r;
}

#ifdef _DEBUG

char * Unit::GetText() 
{ 

	
	
	if (g_isScenario)
	{
		
		return (char *) GetName(); 

	} 
	else
		return g_theUnitPool->GetUnit(m_id)->GetText(); 
}

void Unit::SetText(char *str)
{
    AccessData()->SetText(str); 
} 

#endif


sint32 Unit::IsCantCaptureCity()

{
   return GetDBRec()->GetCantCaptureCity(); 
}

void Unit::ResetCityOwner(const PLAYER_INDEX newo, sint32 is_conquest, 
                          const CAUSE_REMOVE_CITY cause)

{
   AccessData()->ResetCityOwner(*this, newo, is_conquest, cause);

   if(g_network.IsHost()) {
	   g_network.Enqueue(new NetInfo(NET_INFO_CODE_RESET_CITY_OWNER,
									 (uint32)m_id,
									 (uint32)newo,
									 (uint32)is_conquest,
									 (uint32)cause));
	   g_network.Enqueue(AccessData(), AccessData()->GetCityData(), TRUE);

	   g_network.AddResetCityOwnerHack(Unit(m_id));
   }
}










void Unit::ResetUnitOwner(const PLAYER_INDEX newo, 
                          CAUSE_REMOVE_ARMY rem_cause)
{
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_RESET_UNIT_OWNER,
									  (uint32)m_id,
									  (uint32)newo,
									  (uint32)rem_cause));
	}
	AccessData()->ResetUnitOwner(*this, newo, rem_cause) ;

	if(g_network.IsHost()) {
		g_network.Enqueue(AccessData());
		
	}

}

void Unit::InitializeCityData()
{
	AccessData()->InitializeCityData();
}

void Unit::GamestateDebug()

{

#ifdef _DEBUG
   AccessData()->GamestateDebug(); 
#endif
}

void Unit::BeginTurnCity(UnitDynamicArray &dead)

{
    AccessData()->BeginTurnCity(*this, dead);
}

BOOL Unit::BuildUnit(sint32 type)
{
	return AccessData()->BuildUnit(type);
}

BOOL Unit::BuildImprovement(sint32 type)
{
	return AccessData()->BuildImprovement(type);
}

BOOL Unit::BuildWonder(sint32 wonder)
{
	return AccessData()->BuildWonder(wonder);
}

void Unit::AddWonder(sint32 type)
{
	AccessData()->AddWonder(type);
}

BOOL Unit::ChangeCurrentlyBuildingItem(sint32 category, sint32 item_type)
{
    return AccessData()->ChangeCurrentlyBuildingItem(category, item_type);
}

BOOL Unit::HaveImprovement(sint32 type) const
{
	return GetData()->HaveImprovement(type);
}

uint64 Unit::GetImprovements() const
{
	return GetData()->GetImprovements();
}

void Unit::SetSpriteState(SpriteState *s)

{
    AccessData()->SetSpriteState(s);
}

SpriteState * Unit::GetSpriteState()

{
   return GetData()->GetSpriteState(); 
}





































void Unit::SetActor(UnitActor *a) 

{
    AccessData()->SetActor(a);
}

UnitActor * Unit::GetActor()

{
    return GetData()->GetActor(); 
}

sint32 Unit::IsSameMovementType(uint32 bit_field) const
{
    return GetDBRec()->GetMovementType() & bit_field;
}


void Unit::DestroyCapitol()

{
    AccessData()->DestroyCapitol(); 
}

void Unit::DestroyImprovement(sint32 type)
{
	AccessData()->DestroyImprovement(type);
}

void Unit::NewGovernment(sint32 type)
{
	AccessData()->NewGovernment(type);
}

double Unit::GetDefendersBonus() const
{
    return GetData()->GetDefendersBonus();
}

sint32 Unit::ImprovementCanRefuel(const Unit &u) const

{
    return GetData()->ImprovementCanRefuel(u);
}














uint32 Unit_Unit_GetVersion(void)
	{
	return (k_UNIT_VERSION_MAJOR<<16 | k_UNIT_VERSION_MINOR) ;
	}


sint32 Unit::IsCity() const

{
    return GetData()->IsCity();  
}

void Unit::GetProductionStats(sint32 &s, sint32 &t, sint32 &f) const

{ 
    GetData()->GetProductionStats(s, t, f); 
} 

void Unit::GetPop(sint32 &p)const

{
    GetData()->GetPop(p); 
} 
//PFT 29 mar 05, show # turns until city next grows a pop
void Unit::GetTurnsToNextPop(sint32 &p)const 
{
	GetData()->GetTurnsToNextPop(p); 
}

void Unit::GetTradeStats(sint32 &g, sint32 &l, sint32 &sci) const

{ 
    GetData()->GetTradeStats(g, l, sci); 
}

    
void Unit::DrawCityStats(aui_DirectSurface *surf, sint32 x, sint32 y)

{

	
	
	

}

void Unit::AddTradeRoute(TradeRoute &route, BOOL fromNetwork)
{
	AccessData()->AddTradeRoute(route, fromNetwork);
}

void Unit::DelTradeRoute(TradeRoute route)
{
	AccessData()->DelTradeRoute(route);
}

ORDER_RESULT Unit::InterceptTrade()
{
	return AccessData()->InterceptTrade();
}

void Unit::RemoveUnitVision()
{
	AccessData()->RemoveUnitVision();
}

void Unit::AddUnitVision(BOOL &revealed)
{
	AccessData()->AddUnitVision(revealed);
}

void Unit::DoVision(UnitDynamicArray &revealedUnits)
{
	AccessData()->DoVision(revealedUnits);
}

void Unit::UndoVision()
{
	AccessData()->UndoVision();
}

double Unit::GetVisionRange() const
{
	return GetData()->GetVisionRange();
}

uint32 Unit::GetVisibility() const
{
	return GetData()->GetVisibility();
}

uint32 Unit::GetVisibilityClass() const
{ 
    return GetDBRec()->GetVisionClass(); 
}

uint32 Unit::GetEverVisible() const
{
	return GetData()->GetEverVisible();
}

BOOL Unit::IsVisibilityClass(const uint32 bit) const
{
    return GetDBRec()->GetVisionClass() & (1 << bit);
}
uint32 Unit::GetRealVisibility() const
{
	return GetData()->GetRealVisibility();
}

uint32 Unit::GetRadarVisibility() const
{
	return GetData()->GetRadarVisibility();
}

void Unit::SetRadar(sint32 o)
{
	AccessData()->SetRadar(o);
}

void Unit::ForceVisibleThisTurn(const PLAYER_INDEX to_me)
{
    AccessData()->ForceVisibleDuration(to_me, 0);
}

void Unit::ForceVisibleDuration(const PLAYER_INDEX to_me, sint32 duration)
{
	AccessData()->ForceVisibleDuration(to_me, duration);
}

void Unit::SetVisible(const PLAYER_INDEX to_me)
{
	AccessData()->SetVisible(to_me);
}

void Unit::BeginTurnVision(PLAYER_INDEX player)
{
	AccessData()->BeginTurnVision(player);
}

sint32 Unit::CollectProduction(sint32 &mil, sint32 &mat)

{
    return AccessData()->CollectProduction(mil, mat); 
}

void Unit::PayFederalProduction (double percent_military,
								 sint32 &mil_paid, 
								 double percent_terrain, 
								 sint32 &mat_paid)

{
    AccessData()->PayFederalProduction(percent_military, mil_paid, 
									   percent_terrain, mat_paid); 
}

void Unit::PayFederalProductionAbs (sint32 mil_paid, 
									double percent_mat, 
									sint32 &mat_paid)

{
    AccessData()->PayFederalProductionAbs (mil_paid, percent_mat, mat_paid);
}


void Unit::SetMaterialContribution(BOOL on)
{
	AccessData()->SetMaterialContribution(on);
}

BOOL Unit::GetMaterialContribution() const
{
	return GetData()->GetMaterialContribution();
}

BOOL Unit::GetMilitaryContribution() const
{
    return GetData()->GetMilitaryContribution();
}

void Unit::SetMilitaryContribution(BOOL on)

{
    AccessData()->SetMilitaryContribution(on);
}

void Unit::SetIsProfessional(BOOL on)
{
    if (GetIsProfessional() != on) { 
        g_player[GetOwner()]->RegisterProfessionalChange(on, *this);
        AccessData()->SetIsProfessional(on); 
    }
}

BOOL Unit::GetIsProfessional() const
{
    return GetData()->GetIsProfessional();
}

BOOL Unit::GetNeedsNoSupport() const

{
    return GetDBRec()->GetNeedsNoSupport(); 
}

BOOL Unit::GetShieldCost() const

{
    return GetDBRec()->GetShieldCost(); 
}

BOOL Unit::IsEntrenched() const
{
	return GetData()->IsEntrenched();
}

BOOL Unit::IsEntrenching() const
{
	return GetData()->IsEntrenching();
}

BOOL Unit::CanEntrench() const
{
	return GetData()->CanEntrench();
}

void Unit::Entrench()
{
	AccessData()->Entrench();
}

void Unit::Detrench()
{
	AccessData()->Detrench();
}

BOOL Unit::IsAsleep() const
{
	return GetData()->IsAsleep();
}

void Unit::Sleep()
{
	AccessData()->Sleep();
}

void Unit::WakeUp()
{
	AccessData()->WakeUp();
}

void Unit::BeginTurn()
{
	AccessData()->BeginTurn();
}

void Unit::EndTurn()
{
	AccessData()->EndTurn();
}

double Unit::GetHappiness() const
{
 
	return GetData()->GetHappiness();
}

double Unit::GetHappySize() const
{
	return GetData()->GetHappySize();
}
double Unit::GetHappyPollution() const
{
	return GetData()->GetHappyPollution();
}

double Unit::GetHappyConquestDistress() const
{
	return GetData()->GetHappyConquestDistress();
}

double Unit::GetHappyEmpireDist() const
{
	return GetData()->GetHappyEmpireDist();
}

double Unit::GetHappyEnemyAction() const
{
	return GetData()->GetHappyEnemyAction();
}

double Unit::GetHappyPeace() const
{
	return GetData()->GetHappyPeace();
}

double Unit::GetHappyWorkday() const
{
	return GetData()->GetHappyWorkday();
}

double Unit::GetHappyWages() const
{
	return GetData()->GetHappyWages();
}

double Unit::GetHappyRations() const
{
	return GetData()->GetHappyRations();
}
double Unit::GetHappyMartialLaw() const
{
	return GetData()->GetHappyMartialLaw();
}

double Unit::GetHappyPopEntertainment() const
{
	return GetData()->GetHappyPopEntertainment();
}

double Unit::GetHappyImprovement() const
{
	return GetData()->GetHappyImprovement();
}

double Unit::GetHappyWonders() const
{
	return GetData()->GetHappyWonders();
}

double Unit::GetHappyCrime() const
{
	return GetData()->GetHappyCrime();
}

void Unit::CalcHappiness(sint32 &virtualGoldSpent, BOOL firstPass) 

{ 
    AccessData()->CalcHappiness(virtualGoldSpent, firstPass); 
} 

BOOL Unit::IsPatrolling() const
{
	return GetData()->IsPatrolling();
}

void Unit::SetPatrolling(BOOL patrolling)
{
	AccessData()->SetPatrolling(patrolling);
}

BOOL Unit::IsInsideCityRadius(const MapPoint &pos) const
{
	return GetData()->IsInsideCityRadius(pos);
}

void Unit::AddHappyTimer(sint32 turns, double adjust,
						 HAPPY_REASON reason)
{
	AccessData()->AddHappyTimer(turns, adjust, reason);
}

void Unit::EndTurnCity()
{
	AccessData()->EndTurnCity();
}

sint32 Unit::GetPollution() const
{
	return GetData()->GetPollution();
}

void Unit::CityToPark(sint32 agressor)
{
	AccessData()->CityToPark(agressor);

}

void Unit::GetNuked(UnitDynamicArray &killList)
{
	AccessData()->GetNuked(killList);
}

BOOL Unit::SafeFromNukes() const
{
	return GetData()->SafeFromNukes();
}

BOOL Unit::AttackFromSpaceship() const
{
	return GetDBRec()->GetAttackFromSpaceship();
}

BOOL Unit::HasAirport() const
{
	return GetData()->HasAirport();
}

void Unit::UseAirport()
{
	AccessData()->UseAirport();
}

sint32 Unit::AirportLastUsed() const
{
	return GetData()->AirportLastUsed();
}
	

BOOL Unit::HasCityWalls() const
{
	return GetData()->HasCityWalls();
}

BOOL Unit::HasForceField() const
{
	return GetData()->HasForceField();
}

ORDER_RESULT Unit::InvestigateCity(Unit &c)
{
	return AccessData()->InvestigateCity(c);
}

ORDER_RESULT Unit::StealTechnology(Unit &c, sint32 whichAdvance)
{
	return AccessData()->StealTechnology(c, whichAdvance);
}


ORDER_RESULT Unit::InciteRevolution(Unit &c)
{
	return AccessData()->InciteRevolution(c);
}

ORDER_RESULT Unit::AssassinateRuler(Unit &c)
{
	return AccessData()->AssassinateRuler(c);
}

ORDER_RESULT Unit::NullifyWalls(Unit &c)
{
	return AccessData()->NullifyWalls(c);
}

BOOL Unit::HasBeenSpiedUpon() const
{
	return GetData()->HasBeenSpiedUpon();
}

void Unit::SetSpiedUpon()
{
	AccessData()->SetSpiedUpon();
}


void Unit::CityNullifyWalls()
{
	AccessData()->CityNullifyWalls();
}

ORDER_RESULT Unit::EstablishEmbassy(Unit &c)
{
	return AccessData()->EstablishEmbassy(c);
}

BOOL Unit::IsCapitol() const
{
	return GetData()->IsCapitol();
}

void Unit::MakeFranchise(sint32 player)
{
	AccessData()->MakeFranchise(player);
}

sint32 Unit::GetFranchiseOwner() const
{
	return GetData()->GetFranchiseOwner();
}


void Unit::SetFranchiseTurnsRemaining(sint32 turns)
{
	AccessData()->SetFranchiseTurnsRemaining(turns);
}

sint32 Unit::GetFranchiseTurnsRemaining() const
{
	return GetData()->GetFranchiseTurnsRemaining();
}

BOOL Unit::CanSee(Army &al) const
{
	return GetData()->CanSee(al);
}

#ifdef _DEBUG
void Unit::SetIgnoreHappiness(BOOL v)

{
	 AccessData()->SetIgnoreHappiness(v);
}

#endif

void Unit::MakeCitizen(PopDBIndex pi, const MapPoint &pos, sint32 origOwner)
{
	AccessData()->MakeCitizen(pi, pos, origOwner);
}

sint32 Unit::PopCount() const
{
	return GetData()->PopCount();
}


sint32 Unit::CountSlaves() const
{
	return GetData()->CountSlaves();
}

BOOL Unit::IsWatchful() const
{
	return GetData()->IsWatchful();
}

void Unit::SetWatchful()
{
	AccessData()->SetWatchful();
}

void Unit::ModifySpecialAttackChance(UNIT_ORDER_TYPE attack, double &chance)
{
	AccessData()->ModifySpecialAttackChance(attack, chance);
}

void Unit::RemoveOneSlave(PLAYER_INDEX p)
{
	AccessData()->RemoveOneSlave(p);
}

void Unit::DoUprising(UPRISING_CAUSE cause)
{
	AccessData()->DoUprising(cause);
}

void Unit::Place(const MapPoint &point)
{
	AccessData()->Place(point);
}

BOOL Unit::IsTempSlaveUnit() const
{
	return GetData()->IsTempSlaveUnit();
}

void Unit::SetTempSlaveUnit(BOOL b)
{
	AccessData()->SetTempSlaveUnit(b);
}

void Unit::BioInfect( sint32 player )
{
	AccessData()->BioInfect( player );
}

void Unit::NanoInfect( sint32 player )
{
	AccessData()->NanoInfect( player );
}

BOOL Unit::IsBioImmune() const
{
	return GetData()->IsBioImmune();
}

BOOL Unit::IsNanoImmune() const
{
	return GetData()->IsNanoImmune();
}

void Unit::ConvertTo(sint32 player, CONVERTED_BY by)
{
	AccessData()->ConvertTo(player, by);
}

double Unit::TheologicalModifier() const
{
	return GetData()->TheologicalModifier();
}

void Unit::Unconvert()
{
	AccessData()->Unconvert();
}

sint32 Unit::IsConvertedTo() const
{
	return GetData()->IsConvertedTo();
}

BOOL Unit::IsCloaked() const
{
	return GetData()->IsCloaked();
}

void Unit::Cloak()
{
	AccessData()->Cloak();
}

void Unit::Uncloak()
{
	AccessData()->Uncloak();
}

#if 0
BOOL Unit::CanOpenRiftGate() const
{
	if(!GetDBRec()->CreateRift())
		return FALSE;
	return TRUE;
}

BOOL Unit::HasOpenRiftGate(MapPoint &destination) const
{
	if(!GetDBRec()->CreateRift())
		return FALSE;
	return GetData()->HasOpenRiftGate(destination);
}

BOOL Unit::OpenRiftGate(const MapPoint &destination)
{
	if(!GetDBRec()->CreateRift())
		return FALSE;
	return AccessData()->OpenRiftGate(destination);
}

BOOL Unit::IsTravellingRift() const
{
	return GetData()->IsTravellingRift();
}

void Unit::SetTravellingRift()
{
	AccessData()->SetTravellingRift();
}
#endif

BOOL Unit::HasResource(const sint32 resource) const
{
	return GetData()->HasResource(resource);
}

#ifdef CTP1_TRADE
sint32 Unit::GetResourceCount(const sint32 resource) const
{ 
    return GetData()->GetResourceCount(resource);
}

sint32 Unit::GetLocalResourceCount(const sint32 resource) const
{
	return GetData()->GetLocalResourceCount(resource);
}

const Resources *Unit::GetResources() const
{
	return GetData()->GetResources();
}

#endif

sint32 Unit::GetNumTradeRoutes() const
{
	return GetData()->GetNumTradeRoutes();
}

sint32 Unit::GetStoredCityProduction() const
{
    return GetData()->GetStoredCityProduction();
}
sint32 Unit::GetNetCityProduction() const
{
    return GetData()->GetNetCityProduction();
}
sint32 Unit::GetGrossCityProduction() const 
{
    return GetData()->GetGrossCityProduction();
}
sint32 Unit::GetStoredCityFood() const 
{
    return GetData()->GetStoredCityFood();
}
sint32 Unit::GetNetCityFood() const
{
    return GetData()->GetNetCityFood();
}




sint32 Unit::GetGrossCityFood() const 
{
    return GetData()->GetGrossCityFood();
}
sint32 Unit::GetNetCityGold() const
{
    return GetData()->GetNetCityGold();
}
sint32 Unit::GetGrossCityGold() const
{
    return GetData()->GetGrossCityGold();
}

BOOL Unit::IsNanoInfected() const
{
	return GetData()->IsNanoInfected();
}

BOOL Unit::IsBioInfected() const
{
	return GetData()->IsBioInfected();
}

BOOL Unit::IsFranchised() const
{
	return GetData()->IsFranchised();
}

BOOL Unit::IsConverted() const
{
	return GetData()->IsConverted();
}

BOOL Unit::BuyFront()
{
	return AccessData()->BuyFront();
}

void Unit::RemoveFront()
{
	AccessData()->RemoveFront();
}

sint32 Unit::GetOvertimeCost() const
{
	return GetData()->GetOvertimeCost();
}

sint32 Unit::HowMuchLonger() const
{
	return GetData()->HowMuchLonger();
}

void Unit::SetCitySize(sint32 size)
{
	AccessData()->GetCityData()->SetSize(size);
}

void Unit::AddTradedResources(Resources &resources)
{
	AccessData()->GetCityData()->AddTradedResources(resources);
}

#ifdef CTP1_TRADE
void Unit::LocalResources(Resources &resources)
{
	AccessData()->GetCityData()->LocalResources(resources);
}
#endif

sint32 Unit::GetCombatUnits() const
{
	return GetData()->GetCombatUnits();
}

void Unit::GetProjectedHappinessCrime(double &hap, double &crime)
{
	AccessData()->GetProjectedHappinessCrime(hap, crime);
}

BOOL Unit::CanHavePopType(sint32 type) const
{
	return GetData()->CanHavePopType(type);
}

BOOL Unit::CanBuildUnit(sint32 type) const
{
	return GetData()->CanBuildUnit(type);
}

BOOL Unit::CanBuildBuilding(sint32 type) const
{
	return GetData()->CanBuildBuilding(type);
}

BOOL Unit::CanBuildCapitalization() const
{
	return GetData()->CanBuildCapitalization();
}

BOOL Unit::CanBuildWonder(sint32 type) const
{
	return GetData()->CanBuildWonder(type);
}

void Unit::RemoveWonderFromQueue(sint32 type)
{
	AccessData()->RemoveWonderFromQueue(type);
}

void Unit::Injoin(sint32 player)
{
	AccessData()->Injoin(player);
}

BOOL Unit::IsInjoined() const
{
	return GetData()->IsInjoined();
}

sint32 Unit::InjoinedBy() const
{
	return GetData()->InjoinedBy();
}

double Unit::GetDistanceToCapitol() const
{
	return GetData()->GetDistanceToCapitol();
}

BOOL Unit::GetCurrentOrderString(StringId &id) const
{
	return GetData()->GetCurrentOrderString(id);
}

#if 0
void Unit::SetOrders(UNIT_ORDER_TYPE order)
{
	AccessData()->SetOrders(order);
}

UNIT_ORDER_TYPE Unit::GetOrders() const
{
	return GetData()->GetOrders();
}
#endif

double Unit::GetSupportCost() const
{
	return g_player[GetOwner()]->m_readiness->GetSupportCost(*this);
}

double Unit::GetOverseasDistress() const
{
	return GetData()->GetOverseasDistress();
}

void Unit::SupportBuildings()
{
	AccessData()->SupportBuildings();
}

void Unit::CheckRiot()
{
	AccessData()->CheckRiot();
}

BOOL Unit::AiGetCargoMovementPoints(double &min_move_points, 
        BOOL  &first) const
{
    return GetData()->AiGetCargoMovementPoints(min_move_points, 
        first);
}

#if 0
void Unit::SetLastBattle(sint32 turn)
{
	AccessData()->SetLastBattle(turn);
}

sint32 Unit::GetLastBattle() const
{
	return GetData()->GetLastBattle();
}
#endif

BOOL Unit::IsInjured() const
{
	return GetDBRec()->GetMaxHP() > GetHP();
}

void Unit::RecalculateResources()
{
	AccessData()->RecalculateResources();
}
	
BOOL Unit::FightOneLineDanceRangedAttack(Unit &defender)
{
	return AccessData()->FightOneLineDanceRangedAttack(defender);
}

BOOL Unit::FightOneLineDanceAssault(Unit &defender)
{
	return AccessData()->FightOneLineDanceAssault(defender);
}

sint32 Unit::CountTradeWith(PLAYER_INDEX player) const
{
	return GetData()->CountTradeWith(player);
}

BOOL Unit::IsProtectedFromSlavery() const
{
	return GetData()->IsProtectedFromSlavery();
}

void Unit::NotifyAdvance(AdvanceType advance)
{
	AccessData()->NotifyAdvance(advance);
}

void Unit::GiveCommand(UNIT_COMMAND command)
{
	AccessData()->GiveCommand(command);
}

void Unit::ExitWormhole(MapPoint &pos)
{
	AccessData()->ExitWormhole(pos);
}

BOOL Unit::HasLeftMap() const
{
	return GetData()->HasLeftMap();
}

void Unit::ContributeScience(double incomePercent,
							 double &addscience,
							 double &subgold)
{
	AccessData()->ContributeScience(incomePercent,
									addscience,
									subgold);
}

sint32 Unit::FreeSlaves()
{
	return AccessData()->FreeSlaves();
}

void Unit::SetArmy(const Army &army)
{
	AccessData()->SetArmy(army);
}

void Unit::ChangeArmy(const Army &army, CAUSE_NEW_ARMY cause)
{
	AccessData()->ChangeArmy(army, cause);
}

Army Unit::GetArmy() const
{
	return GetData()->GetArmy();
}

BOOL Unit::CanPerformSpaceTransitionNow() const
{
	return GetData()->CanPerformSpaceTransitionNow();
}

void Unit::IndicateSpaceTransition()
{
	AccessData()->IndicateSpaceTransition();
}

void Unit::AddWonderHPBonus(sint32 amt)
{
	AccessData()->AddWonderHPBonus(amt);
}






sint32 Unit::CanPlantNuke(const MapPoint &pos) 
{
	return AccessData()->CanPlantNuke(pos);
}

sint32 Unit::CanMakePark(const MapPoint &pos)
{
	return AccessData()->CanMakePark(pos);
}
sint32 Unit::CanUndergroundRailway(const MapPoint &pos)
{
	return AccessData()->CanUndergroundRailway(pos);
}

sint32 Unit::CanConvert(const MapPoint &pos)
{
	return AccessData()->CanConvert(pos);
}

sint32 Unit::CanEstablishEmbassy(const MapPoint &pos)
{
	return AccessData()->CanEstablishEmbassy(pos);
}

sint32 Unit::CanCreateFranchise(const MapPoint &pos)
{
	return AccessData()->CanCreateFranchise(pos);
}

sint32 Unit::CanAssasinateRuler(const MapPoint &pos)
{
	return AccessData()->CanAssasinateRuler(pos);
}

sint32 Unit::CanStealTechnology(const MapPoint &pos)
{
	return AccessData()->CanStealTechnology(pos);
}

sint32 Unit::CanInjoin(const MapPoint &pos)
{
	return AccessData()->CanInjoin(pos);
}

sint32 Unit::CanInciteRevolution(const MapPoint &pos)
{
	return AccessData()->CanInciteRevolution(pos);
}

sint32 Unit::CanCauseUnhappiness(const MapPoint &pos)
{
	return AccessData()->CanCauseUnhappiness(pos);
}

sint32 Unit::CanExpel(const MapPoint &pos)
{
	return AccessData()->CanExpel(pos);
}

void Unit::AddEndGameObject(sint32 type)
{
	AccessData()->AddEndGameObject(type);
}

BOOL Unit::SendSlaveTo(Unit &dest)
{
	return AccessData()->SendSlaveTo(dest);
}

void Unit::SetFullHappinessTurns(sint32 turns)
{
	AccessData()->SetFullHappinessTurns(turns);
}

sint32 Unit::GetIncomingTrade() const
{
	return GetData()->GetIncomingTrade();
}

sint32 Unit::GetOutgoingTrade() const
{
	return GetData()->GetOutgoingTrade();
}

void Unit::FinishBuilding()
{
	AccessData()->FinishBuilding();
}

void Unit::DestroyRandomBuilding()
{
	AccessData()->DestroyRandomBuilding();
}

void Unit::AddConversionUnhappiness(sint32 who)
{
	AccessData()->AddConversionUnhappiness(who);
}

void Unit::BuildCapitalization()
{
	AccessData()->BuildCapitalization();
}

void Unit::BuildInfrastructure()
{
	AccessData()->BuildInfrastructure();
}

BOOL Unit::Flag(uint32 flag) const
{
	return GetData()->Flag(flag);
}

void Unit::SetFlag(uint32 flag)
{
	AccessData()->SetFlag(flag);
}

void Unit::ClearFlag(uint32 flag)
{
	AccessData()->ClearFlag(flag);
}

const Unit &Unit::GetTransport() const
{
	return GetData()->GetTransport();
}

void Unit::RemoveTransportedUnit(const Unit &u)
{
	AccessData()->RemoveTransportedUnit(u);
}

BOOL Unit::BuildEndGame(sint32 type)
{
	return AccessData()->BuildEndGame(type);
}

void Unit::DisbandCity()
{
	AccessData()->DisbandCity();
}

BOOL Unit::CanPerformSpecialAction() const
{
	return GetData()->CanPerformSpecialAction();
}

void Unit::AddGoods(SlicObject *obj)
{
	AccessData()->AddGoods(obj);
}

void Unit::UpdateZOCForRemoval()
{
	if(!IsNoZoc() && !IsTempSlaveUnit() && !HasLeftMap() && !IsBeingTransported()) {
		MapPoint pos;
		GetPos(pos);
		Cell *cell = g_theWorld->GetCell(pos);
		CellUnitList *units = cell->UnitArmy();
		sint32 i;
		
		
		
		BOOL updateZoc = TRUE;
		if(cell->GetCity().m_id != 0 &&
		   cell->GetCity().IsValid() &&
		   cell->GetCity().GetOwner() == GetOwner()) {
			updateZoc = FALSE;
		} else if(units && units->GetOwner() == GetOwner()) {
			for(i = 0; i < units->Num(); i++) {
				if(!units->Access(i).IsNoZoc()) {
					updateZoc = FALSE;
					break;
				}
			}
		}

		if(updateZoc) {
			g_theWorld->RemoveZOC(pos, GetOwner());
			g_theWorld->AddOtherArmyZOC(pos, GetOwner(), Army(0), Unit(0));
			
			sint32 dd;
			for(dd = 0; dd < (sint32)NOWHERE; dd++) {
				MapPoint npos;
				if(pos.GetNeighborPosition((WORLD_DIRECTION)dd, npos)) {
					g_theWorld->RemoveZOC(npos, GetOwner());
					g_theWorld->AddOtherArmyZOC(npos, GetOwner(), Army(0), Unit(0));
				}
			}
		}
	}
}

void Unit::UpdateZOCForInsertion()
{
	AccessData()->UpdateZOCForInsertion();
}

void Unit::CheckVisionRadius()
{
	AccessData()->CheckVisionRadius();
}

BOOL UnitCanCarry(sint32 dest, sint32 src)
{ 
	const UnitRecord *drec = g_theUnitDB->Get(dest); 
	const UnitRecord *srec = g_theUnitDB->Get(src);
//	const UnitRecord *drec = g_theUnitDB->Get(dest, g_player[GetOwner()]->GetGovernmentType()); // Owner has to be passed
//	const UnitRecord *srec = g_theUnitDB->Get(src, g_player[GetOwner()]->GetGovernmentType());


    if (!drec->GetCanCarry()) {
        return FALSE; 
    } 
    
    if (srec->GetSizeSmall()) { 
        
        if (drec->GetCanCarrySmallLand() && srec->GetMovementTypeLand()) {
            return TRUE; 
        }
        
        if (drec->GetCanCarrySmallAir() && srec->GetMovementTypeAir()) {
            return TRUE; 
        }
        
        if (drec->GetCanCarrySmallWater() && srec->GetMovementTypeSea()) {
            return TRUE; 
        }
        
        if (drec->GetCanCarrySmallSpace() && srec->GetMovementTypeSpace()) {
            return TRUE; 
        }
    }else if (srec->GetSizeMedium()) {
        if (drec->GetCanCarryMedLand() && srec->GetMovementTypeLand()) {
            return TRUE; 
        }
        
        if (drec->GetCanCarryMedAir() && srec->GetMovementTypeAir()) {
            return TRUE; 
        }
        
        if (drec->GetCanCarryMedWater() && srec->GetMovementTypeSea()) {
            return TRUE; 
        }
        
        if (drec->GetCanCarryMedSpace() && srec->GetMovementTypeSpace()) {
            return TRUE; 
        }
    } else if (srec->GetSizeLarge()) { 
        if (drec->GetCanCarryLargeLand() && srec->GetMovementTypeLand()) {
            return TRUE; 
        }
        
        if (drec->GetCanCarryLargeAir() && srec->GetMovementTypeAir()) {
            return TRUE; 
        }
        
        if (drec->GetCanCarryLargeWater() && srec->GetMovementTypeSea()) {
            return TRUE; 
        }
        
        if (drec->GetCanCarryLargeSpace() && srec->GetMovementTypeSpace()) {
            return TRUE; 
        }
    } 
    return FALSE; 
}

CityData *Unit::GetCityData() const
{
	return AccessData()->GetCityData();
}

//----------------------------------------------------------------------------
//
// Name       : Unit::NeedsRefueling
//
// Description: Checks whether the given Unit has to be refueled.
//
// Parameters : -
//
// Globals    : g_theConstDB: The const database
//
// Returns    : bool:         true:  The Unit has to be refueled
//                            false: Otherwise
//
// Remark(s)  : Does not check whether the Unit needs fuel.
//
//----------------------------------------------------------------------------
bool Unit::NeedsRefueling() const
{
	return GetFuel() <= g_theConstDB->NonSpaceFuelCost() * (GetMovementPoints() / 100.0);
}

//----------------------------------------------------------------------------
//
//  Name       : Unit::UnitValidForOrder
//
//  Description: Test if a unit passes an order's UnitPretest
//
//  Parameters : OrderRecord * order_rec : The order's order record 
//
//  Globals    : g_theUnitDB             : The unit database
//
//  Returns    : bool   : true if the unit passes order_rec's UnitPretest
//                      : false otherwise
//
//  Remark(s)  : - 
//
//  Called by  : 
//
//----------------------------------------------------------------------------
bool Unit::UnitValidForOrder(const OrderRecord * order_rec) const
{
	const UnitRecord *unit_rec = g_theUnitDB->Get(GetType(), g_player[GetOwner()]->GetGovernmentType());

	UnitRecord::ChanceEffect *chance_data;
	UnitRecord::InvestigateCityData *investigate_data;
	UnitRecord::StealTechnologyData *steal_data;
	UnitRecord::InciteRevolutionData *incite_data;
	UnitRecord::AssasinateRulerData *assasinate_data;
	UnitRecord::CauseUnhappinessData *unhappiness_data;
	UnitRecord::PlantNukeData *plant_data;
	UnitRecord::SlaveRaidsData *raid_data;
	UnitRecord::SuccessDeathEffect *success_death_data;

	bool order_valid = false;

	
	if(order_rec->GetUnitPretest_CanAttack())
		order_valid = (unit_rec->GetAttack() > 0.0 );
	else if(order_rec->GetUnitPretest_CanEntrench())
		order_valid = unit_rec->GetCanEntrench();
//	else if(order_rec->GetUnitPretest_CanSueFranchise())
//		order_valid = unit_rec->GetCanSueFranchise();
	else if(order_rec->GetUnitPretest_CanSue())
		order_valid = unit_rec->GetCanSue();
	else if(order_rec->GetUnitPretest_CanCreateFranchise())
		order_valid = unit_rec->GetCreateFranchise(chance_data);
	else if(order_rec->GetUnitPretest_CanInvestigateCity())
		order_valid = unit_rec->GetInvestigateCity(investigate_data);
	else if(order_rec->GetUnitPretest_CanBombard())
		order_valid = (unit_rec->GetCanBombard() != 0x0);
	else if(order_rec->GetUnitPretest_CanSettle())
		order_valid = (unit_rec->GetSettleLand() || unit_rec->GetSettleWater());
	else if(order_rec->GetUnitPretest_CanStealTechnology())
		order_valid = unit_rec->GetStealTechnology(steal_data);
	else if(order_rec->GetUnitPretest_CanInciteRevolution())
		order_valid = unit_rec->GetInciteRevolution(incite_data);
	else if(order_rec->GetUnitPretest_CanAssassinateRuler())
		order_valid = unit_rec->GetAssasinateRuler(assasinate_data);
	else if(order_rec->GetUnitPretest_CanExpel())
		order_valid = unit_rec->GetCanExpel();
	else if(order_rec->GetUnitPretest_EstablishEmbassy())
		order_valid = unit_rec->GetEstablishEmbassy();
	else if(order_rec->GetUnitPretest_ThrowParty())
		order_valid = unit_rec->GetThrowParty();
	else if(order_rec->GetUnitPretest_CanCauseUnhappiness())
		order_valid = unit_rec->GetCauseUnhappiness(unhappiness_data);
	else if(order_rec->GetUnitPretest_CanPlantNuke())
		order_valid = unit_rec->GetPlantNuke(plant_data);
	else if(order_rec->GetUnitPretest_CanSlaveRaid())
		order_valid = unit_rec->GetSlaveRaids(raid_data);
	else if(order_rec->GetUnitPretest_CanEnslaveSettler())
		order_valid = unit_rec->GetSettlerSlaveRaids();
	else if(order_rec->GetUnitPretest_CanUndergroundRailway())
		order_valid = unit_rec->GetUndergroundRailway(success_death_data);
	else if(order_rec->GetUnitPretest_CanInciteUprising())
		order_valid = unit_rec->GetSlaveUprising();
	else if(order_rec->GetUnitPretest_CanBioTerror())
		order_valid = unit_rec->GetBioTerror(chance_data);
	else if(order_rec->GetUnitPretest_CanPlague())
		order_valid = unit_rec->GetPlague(chance_data);
	else if(order_rec->GetUnitPretest_CanNanoInfect())
		order_valid = unit_rec->GetNanoTerror(chance_data);
	else if(order_rec->GetUnitPretest_CanConvertCity())
		order_valid = unit_rec->GetConvertCities(success_death_data);
	else if(order_rec->GetUnitPretest_CanReformCity())
		order_valid = unit_rec->GetCanReform();
	else if(order_rec->GetUnitPretest_CanSellIndulgences())
		order_valid = unit_rec->GetIndulgenceSales();
//	else if(order_rec->GetUnitPretest_CanFaithHeal())
//		order_valid = false;
	else if(order_rec->GetUnitPretest_CanSoothsay())
		order_valid = unit_rec->GetCanSoothsay();
	else if(order_rec->GetUnitPretest_CanCreatePark())
		order_valid = unit_rec->GetCreateParks();
	else if(order_rec->GetUnitPretest_CanPillage())
		order_valid = unit_rec->GetCanPillage();
	else if(order_rec->GetUnitPretest_CanInjoin())
		order_valid = unit_rec->GetCanInjoin();
	else if(order_rec->GetUnitPretest_CanInterceptTrade())
		order_valid = unit_rec->GetCanPirate();
	else if(order_rec->GetUnitPretest_CanAdvertise())
		order_valid = unit_rec->GetAdvertise();
	else if(order_rec->GetUnitPretest_CanNukeCity())
		order_valid = unit_rec->GetNuclearAttack();
	else if(order_rec->GetUnitPretest_CanTransport())
		order_valid = unit_rec->GetCargoData();
	else if(order_rec->GetUnitPretest_CanBeTransported())
		order_valid = unit_rec->GetSizeMedium() || 
			unit_rec->GetSizeSmall();
	else if(order_rec->GetUnitPretest_CanLaunch())
		order_valid = unit_rec->GetSpaceLaunch();
	else if(order_rec->GetUnitPretest_CanTarget())
		order_valid = unit_rec->GetNuclearAttack();
	else if(order_rec->GetUnitPretest_NoFuelThenCrash()
	     && unit_rec->GetNoFuelThenCrash()
	     &&!unit_rec->GetSingleUse())
		order_valid = NeedsRefueling();
//	else if(order_rec->GetUnitPretest_CanParadrop())
//		order_valid = unit_rec->GetParatrooperTransport();
	else if(order_rec->GetUnitPretest_None())
		order_valid = true;

	return order_valid;
}
