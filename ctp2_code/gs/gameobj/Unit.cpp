//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Unit
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
// CTP1_TRADE
// - Creates an executable with trade like in CTP1. Currently broken.
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
// - Removed some unsused method to removed some unused in methods in
//   CityData.. - Aug 6th 2005 Martin Gühmann
// - Removed another unused and unecessary function. (Aug 12th 2005 Martin Gühmann)
// - Added GetAllTerrainAsImp by E 2-24-2006
// - Corrected pollution handling.
// - Moved sinking and upgrade functionality from ArmyData. (Dec 24th 2006 Martin Gühmann)
// - Added IsReligion bools 1-23-2007
// - Added IsHiddenNationality bool 2-7-2007
// - The upgrade function now selects the best unit type for upgrading
//   according to the unit transport capacity or the unit attack, defense and
//   range statitics. (19-May-2007 Martin Gühmann)
// - modified sink to display the sink message and the unit type
// - If a unit dies it now uses the value of LaunchPollution if present
//   to pollute the environment, instead of using a value of 1. (9-Jun-2007 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - Added an IsInVisionRange test. (25-Jan-2008 Martin Gühmann)
// - Added check move points option to CanAtLeastOneCargoUnloadAt (8-Feb-2008 Martin Gühmann).
// - Separated the Settle event drom the Settle in City event. (19-Feb-2008 Martin Gühmann)
// - Changed occurances of UnitRecord::GetMaxHP to
//   UnitData::CalculateTotalHP. (Aug 3rd 2009 Maq)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Unit.h"

#include "Globals.h"
#include "ConstRecord.h"        // g_theConstDB
#include "StrDB.h"              // g_theStringDB
#include "DB.h"
#include "dynarr.h"
#include "UnitDynArr.h"
#include "citydata.h"
#include "cellunitlist.h"
#include "player.h"             // g_player
#include "pollution.h"
#include "World.h"              // g_theWorld
#include "UnitData.h"
#include "network.h"
#include "net_info.h"
#include "UnitPool.h"           // g_theUnitPool
#include "RandGen.h"            // g_rand
#include "aui.h"
#include "primitives.h"
#include "director.h"           // g_director
#include "tech_wllist.h"
#include "Readiness.h"
#include "AICause.h"
#include "SlicEngine.h"
#include "HappyTracker.h"
#include "Cell.h"
#include "TradeOfferPool.h"
#include "ArmyPool.h"
#include "ArmyData.h"
#include "TradeBids.h"
#include "SelItem.h"            // g_selected_item
#include "WonderRecord.h"
#include "gamesounds.h"
#include "SpecialAttackInfoRecord.h"
#include "SpecialEffectRecord.h"
#include "GameEventManager.h"
#include "unitutil.h"
#include "ctpai.h"
#include "gamefile.h"
#include "TerrainRecord.h"
#include "SlicObject.h"
#include "profileDB.h"

extern Pollution *  g_thePollution;

namespace
{
	size_t const    DISTANCE_NOT_ON_MAP = 0xffffffffu;
}

void Unit::KillUnit(const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy)
{
	sint32  pollution;
	if(GetDBRec()->GetDeathPollution(pollution))
	{
		g_player[GetOwner()]->AdjustEventPollution(pollution);
	}

	if(GetDBRec()->GetLaunchPollution(pollution))
	{
		g_player[GetOwner()]->AdjustEventPollution(pollution);
	}

	Unit tmp(m_id);
	tmp.RemoveAllReferences(cause, killedBy);
}

void Unit::RemoveAllReferences(const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy)
{
	bool is_renumber_cont = false;

	if (IsCity())
	{
		AccessData()->GetCityData()->PrepareToRemove(cause, killedBy);
	}

	if(GetArmy().IsValid())
	{
		GetArmy().SetRemoveCause(cause);
	}

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

	DPRINTF(k_DBG_GAMESTATE, ("Unit::RemoveAllReferences: id: 0x%lx cause: %d\n",
	                          m_id, cause));
	if(g_network.IsActive())
	{
		if(g_network.IsHost())
		{
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_KILL_UNIT, m_id));
		}
		else
		{
			g_network.AddDeadUnit(m_id);
		}
	}

	if (GetActor())
	{
		if (cause == CAUSE_REMOVE_ARMY_PARADROP_COMPLETE
			|| IsBeingTransported()
			|| cause == CAUSE_REMOVE_ARMY_DIED_IN_ATTACK
			|| cause == CAUSE_REMOVE_ARMY_ATTACKED
			|| cause == CAUSE_REMOVE_ARMY_SETTLE
			|| cause == CAUSE_REMOVE_ARMY_DISBANDED
			|| cause == CAUSE_REMOVE_ARMY_GOVERNMENT_CHANGE
			|| cause == CAUSE_REMOVE_ARMY_NUKE
			|| cause == CAUSE_REMOVE_ARMY_PARKRANGER) {
			g_director->AddFastKill(GetActor());
		}
		else
		{
			g_director->AddDeath(GetActor(), RetPos(), GetDeathSoundID());
		}
		AccessData()->SetActor(NULL);
	}

	if(!GetDBRec()->GetIsTrader())
	{
		AccessData()->KillVision();
	}

	MapPoint        pos;
	GetPos(pos);
	PLAYER_INDEX    owner   = GetOwner();
	sint32          r       = TRUE;

	if(!GetDBRec()->GetIsTrader()
	&& !IsTempSlaveUnit()
	&& !IsBeingTransported()
	&& !HasLeftMap()
	){
		r = g_theWorld->RemoveUnitReference(pos, *this);
		Assert(r);
	}
	else
	{
		if(GetArmy().IsValid() && !HasLeftMap())
		{
			Assert(false);
			r = g_theWorld->RemoveUnitReference(pos, *this);
			Assert(r);
		}
	}

	if(!IsTempSlaveUnit())
	{
		r = g_player[owner]->RemoveUnitReference(*this, cause, killedBy);
		Assert(r);
	}

	Unit transport = GetTransport();
	if(transport.IsValid())
	{
		transport.RemoveTransportedUnit(*this);
	}

	if(GetDBRec()->GetHasPopAndCanBuild())
	{
		if(cause != CAUSE_REMOVE_ARMY_POLLUTION
		&& cause != CAUSE_REMOVE_ARMY_FLOOD
		){
			g_theWorld->SetCanalTunnel(pos, 0);
			is_renumber_cont = true;
		}

		Cell *cell = g_theWorld->GetCell(pos);

		g_theWorld->CutImprovements(pos);

		if(g_network.IsHost())
		{
			g_network.Enqueue(cell, pos.x, pos.y);
		}

		g_theTradeOfferPool->RemoveTradeOffersFromCity(*this);
		g_theTradeBids->CancelBidsWithCity(*this);

		if (cell->UnitArmy())
		{
			if (cause != CAUSE_REMOVE_ARMY_NUKE)
			{
				for (sint32 i = cell->UnitArmy()->Num() - 1; i >= 0; --i)
				{
					Unit    u = cell->UnitArmy()->Access(i);
					if (u.IsValid() && !g_theWorld->CanEnter(pos, u.GetMovementType()))
					{
						u.Kill(CAUSE_REMOVE_ARMY_ILLEGAL_CELL, -1);
					}
				}
			}
		}

		CD()->RemoveBorders();

		g_selected_item->RegisterRemovedCity(GetOwner(), *this);
	}

	AccessData()->KillTransportedUnits();

	if(GetArmy().IsValid())
	{
		GetArmy().SetKiller(killedBy);
		GetArmy().Del(*this);
	}

	if(!GetDBRec()->GetIsTrader())
	{
		UpdateZOCForRemoval();
	}

	g_theUnitPool->Del(m_id);

	if(is_renumber_cont)
	{
		g_theWorld->NumberContinents();
	}
}


void Unit::FastKill()
{
	if(GetActor()) {
		if(g_director)
			g_director->FastKill(GetActor());
		AccessData()->SetActor(NULL);
	} else {
		Assert(false);
	}

	g_theUnitPool->Del(m_id);
}

bool Unit::IsValid() const
{
	return g_theUnitPool->IsValid(*this);
}

sint32 Unit::GetGoldHunger() const  //EMOD
{
	return GetDBRec()->GetGoldHunger();
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
	return GetData()->GetDBRec();
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
	return GetData()->GetOwner();
}

sint32 Unit::GetType() const
{
	return GetData()->GetType();
}

void Unit::GetPos(MapPoint &pos) const
{
	GetData()->GetPos(pos);
}

MapPoint Unit::RetPos() const
{
	return AccessData()->GetPos();
}

double Unit::GetHPModifier() const
{
	return g_player[GetOwner()]->GetHPModifier();
}

double Unit::GetHP() const
{
	return GetData()->GetHP();
}

uint32 Unit::GetOwnerBit() const
{
	return GetData()->GetOwnerBit();
}

bool Unit::IsIgnoresZOC() const
{
	return GetDBRec()->GetIgnoreZOC();
}

bool Unit::IsInVisionRange(MapPoint &pos) const
{
	MapPoint here;
	GetPos(here);
	double distance    = MapPoint::GetSquaredDistance(here, pos);
	double visionRange = GetVisionRange();

	return distance <= visionRange* visionRange;
}

bool Unit::NearestUnexplored(sint32 searchRadius, MapPoint &pos) const
{
	size_t  distance    = DISTANCE_NOT_ON_MAP;
	MapPoint center;
	GetPos(center);
	CircleIterator it(center, searchRadius, static_cast<sint32>(GetVisionRange()));
	for (it.Start(); !it.End(); it.Next())
	{
		if (    !g_player[GetOwner()]->IsExplored(it.Pos())
		     && GetArmy()->CanEnter(it.Pos())
		     && g_theWorld->IsOnSameContinent(it.Pos(), center)
		   )
		{
			size_t  target_distance = static_cast<size_t>
			    (MapPoint::GetSquaredDistance(center, it.Pos()));

			if (target_distance < distance)
			{
				distance    = target_distance;
				pos         = it.Pos();
			}
		}
	}

	return distance < DISTANCE_NOT_ON_MAP;
}

bool Unit::NearestFriendlyCity(MapPoint &p) const
{
	size_t	    closest_distance = DISTANCE_NOT_ON_MAP;
	MapPoint    unit_pos;
	GetPos(unit_pos);
	p = unit_pos;

	MapPoint    city_pos;
	int const   n = g_player[GetOwner()]->m_all_cities->Num();

	for (int i = 0; i < n; i++)
	{
		g_player[GetOwner()]->m_all_cities->Get(i).GetPos(city_pos);

		size_t d = std::max(abs(city_pos.x - unit_pos.x),
		                    abs(city_pos.y - unit_pos.y)
		                   );
		if (d < closest_distance)
		{
			closest_distance = d;
			p = city_pos;
		}
	}

	return closest_distance < DISTANCE_NOT_ON_MAP;
}

bool Unit::NearestFriendlyCityWithRoom(MapPoint &p, sint32 needRoom,
                                       Army army) const
{
	size_t      closest_distance = DISTANCE_NOT_ON_MAP;

	MapPoint    unit_pos;
	GetPos(unit_pos);
	p = unit_pos;

	MapPoint    city_pos;
	int const   n = g_player[GetOwner()]->m_all_cities->Num();
	bool const  testWaterMove   =
	    army.IsValid() &&
	   (army->IsAtLeastOneMoveWater() || army->IsAtLeastOneMoveShallowWater());

	for (int i = 0; i < n; i++)
	{
		g_player[GetOwner()]->m_all_cities->Get(i).GetPos(city_pos);

		if (g_theWorld->GetCell(city_pos)->GetNumUnits() + needRoom > k_MAX_ARMY_SIZE)
			continue;

		if (testWaterMove &&
			(!g_theWorld->IsNextToWater(city_pos.x, city_pos.y) &&
			 !g_theWorld->IsWater(city_pos)
            )
           )
			continue;

        size_t	d = std::max(abs(city_pos.x - unit_pos.x),
                             abs(city_pos.y - unit_pos.y)
                            );
		if (d < closest_distance)
        {
			closest_distance = d;
			p = city_pos;
		}
	}

	return closest_distance < DISTANCE_NOT_ON_MAP;
}











bool Unit::NearestFriendlyCity(Unit &c) const
{
	size_t	closest_distance = DISTANCE_NOT_ON_MAP;
	c = Unit();

	MapPoint    unit_pos;
	GetPos(unit_pos);

    MapPoint    city_pos;
	int const   n = g_player[GetOwner()]->m_all_cities->Num();
	for (int i = 0; i < n; i++)
	{
		g_player[GetOwner()]->m_all_cities->Get(i).GetPos(city_pos);

        size_t	d = std::max(abs(city_pos.x - unit_pos.x),
                             abs(city_pos.y - unit_pos.y)
                            );

		if (d < closest_distance)
		{
			closest_distance = d;
			c = g_player[GetOwner()]->m_all_cities->Get(i).m_id;
		}

	}

	return closest_distance < DISTANCE_NOT_ON_MAP;
}

void Unit::Launch()
{
    sint32 pollution;
    if (GetDBRec()->GetLaunchPollution(pollution))
    {
		g_player[GetOwner()]->AdjustEventPollution(pollution);
    }
}

bool Unit::MoveToPosition(const MapPoint &p, UnitDynamicArray &revealed)
{
	return SetPosition(p, revealed);
}

bool Unit::SetPosition(const MapPoint &p, UnitDynamicArray &revealed)
{
	bool left_map = false;
	AccessData()->SetPos(p, left_map);

	return left_map || g_theWorld->InsertUnit(p, *this, revealed);
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
	return GetData()->GetMovementPoints();
}

void Unit::SetMovementPoints(double m)
{
	AccessData()->SetMovementPoints(m);
}

bool Unit::CanBeExpelled() const
{
	return GetDBRec()->GetCanBeExpelled() && g_theWorld->GetOwner(RetPos()) != GetOwner();
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

sint32 Unit::GetWagesNeeded() const
{
	return GetData()->GetWagesNeeded();
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

bool Unit::GetMovementTypeLand() const
{
	return GetDBRec()->GetMovementTypeLand();
}

bool Unit::GetMovementTypeSea() const
{
	return GetDBRec()->GetMovementTypeSea();
}

bool Unit::GetMovementTypeShallowWater() const
{
	return GetDBRec()->GetMovementTypeShallowWater();
}

bool Unit::GetMovementTypeAir() const
{
	return GetDBRec()->GetMovementTypeAir();
}

//bool Unit::GetAllTerrainAsImprovement() const  //EMOD
//{
//	return GetDBRec()->GetAllTerrainAsImprovementIndex();  //EMOD
//}

bool Unit::GetMovementTypeSpace() const
{
	return GetDBRec()->GetMovementTypeSpace();
}

bool Unit::GetMovementTypeMountain() const
{
	return GetDBRec()->GetMovementTypeMountain();
}

bool Unit::IsSubmarine() const
{
	return GetDBRec()->GetIsSubmarine();
}

bool Unit::IsNoZoc() const
{
	return GetDBRec()->GetNoZoc();
}

bool Unit::DeductMoveCost(const double cost, bool &out_of_fuel)
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

bool Unit::IsBeingTransported() const
{
	return GetData()->IsBeingTransported();
}

bool Unit::CanAtLeastOneCargoUnloadAt(const MapPoint & unload_pos, const bool & use_vision, bool check_move_points) const
{
	return GetData()->CanAtLeastOneCargoUnloadAt(unload_pos, use_vision, check_move_points);
}

bool Unit::UnloadCargo(const MapPoint &unload_pos, Army &debark, sint32 &count, bool unloadSelected)
{
	return AccessData()->UnloadCargo(unload_pos, debark, count, unloadSelected);
}

bool Unit::IsMovePointsEnough(const MapPoint &pos) const
{
	return GetData()->IsMovePointsEnough(pos);
}

void Unit::GetInserted(const Unit &transport)
{
	AccessData()->GetInserted(transport);
}

bool Unit::InsertCargo(const Unit &addme)
{
	return AccessData()->InsertCargo(addme);
}

bool Unit::CanBeachAssault() const
{
	return GetDBRec()->GetCanBeachAssault();
}

//PFT
bool Unit::IsImmobile()const
{
	return GetData()->IsImmobile();
}

bool Unit::CantGroup()const
{
	return GetData()->CantGroup();
}

sint32 Unit::ResetMovement()
{
	return AccessData()->ResetMovement();
}

void Unit::SetHP(const double newval)
{
	AccessData()->SetHP(newval);
}

void Unit::SetType(const sint32 type)
{
	AccessData()->SetType(type);
}

void Unit::SetHPToMax()
{
   SetHP(AccessData()->CalculateTotalHP());
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

bool Unit::GetUsedFuel (sint32 &fuel_remaining, sint32 &max_fuel) const
{
	return GetData()->GetUsedFuel(fuel_remaining, max_fuel);
}

bool Unit::IsVeteran() const
{
	return GetData()->IsVeteran();
}

void Unit::SetVeteran()  //copy and make for elite units
{
	AccessData()->SetVeteran();  //copy and make for elite units
}

void Unit::UnVeteran()
{
	AccessData()->UnVeteran();
}

bool Unit::CanInterceptTrade() const
{
	return GetData()->CanInterceptTrade();
}

bool Unit::CanRustle(CellUnitList &defender) const
{
	return GetData()->CanRustle(defender);
}

bool Unit::CanConvertCity(Unit &city) const
{
	return GetData()->CanConvertCity(city);
}

bool Unit::CanBombard(CellUnitList &defender) const
{
	return GetData()->CanBombard(defender);
}

bool Unit::CanCounterBombard(CellUnitList &defender) const
{
	return GetData()->CanCounterBombard(defender);
}

bool Unit::CanActivelyDefend(const Army &attacker) const
{
	CellUnitList * list = g_theArmyPool->AccessArmy(attacker);
	return GetData()->CanActivelyDefend(*list);
}

bool Unit::CanActivelyDefend(CellUnitList &attacker) const
{
	return GetData()->CanActivelyDefend(attacker);
}

void Unit::Bombard(const UnitRecord *rec, Unit defender, bool isCounterBombardment)
{
	AccessData()->Bombard(rec, defender, isCounterBombardment);
}

bool Unit::Bombard(CellUnitList &defender, bool isCounterBombardment)
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

bool Unit::IsFlanker() const
{
	return GetDBRec()->GetIsFlanker();
}

sint32 Unit::GetZBRange() const
{
	return GetDBRec()->GetZBRangeAttack();
}

sint32 Unit::GetCargoCapacity() const
{
	return GetData()->GetCargoCapacity();
}

void Unit::GetCargoHP(sint32 &i, sint32 unit_type[100],
                      sint32 unit_hp[100]) const
{
	GetData()->GetCargoHP(i, unit_type, unit_hp);

}

sint32 Unit::GetNumCarried() const
{
	return GetData()->GetNumCarried();
}

bool Unit::DeathEffectsHappy() const
{
	return GetDBRec()->GetDeathEffectsHappy();
}

sint32 Unit::GetSelect1SoundID(void)
{
	return GetDBRec()->GetSoundSelect1Index();
}

sint32 Unit::GetSelect2SoundID(void)
{
	return GetDBRec()->GetSoundSelect2Index();
}

sint32 Unit::GetMoveSoundID(void)
{
	if(!g_theUnitPool->IsValid(m_id))
		return 0;   // TODO: check whether this should be -1
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

bool Unit::GetSpecialAttackInfo(SPECATTACK attack, sint32 *soundID, sint32 *spriteID)
{
	const SpecialAttackInfoRecord *rec = unitutil_GetSpecialAttack(attack);
	if(!rec)
		return false;
	*soundID = rec->GetSoundIDIndex();
	*spriteID = rec->GetSpriteID()->GetValue();
	return true;
}

bool Unit::CanCarry(Unit u) const
{
	return GetData()->CanCarry(u.GetType());
}

bool Unit::CanSettle(const MapPoint &pos, const bool settleOnCity) const
{
	return GetData()->CanSettle(pos, settleOnCity);
}

bool Unit::Settle()
{
	return AccessData()->Settle();
}

#ifdef _DEBUG

char const * Unit::GetText() const
{
    return (g_isScenario) ? GetName() : GetData()->GetText();
}

void Unit::SetText(char *str)
{
	AccessData()->SetText(str);
}

#endif

bool Unit::IsCantCaptureCity()
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
	AccessData()->ResetUnitOwner(*this, newo, rem_cause);

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

bool Unit::BuildUnit(sint32 type)
{
	return AccessData()->BuildUnit(type);
}

bool Unit::BuildImprovement(sint32 type)
{
	return AccessData()->BuildImprovement(type);
}

bool Unit::BuildWonder(sint32 wonder)
{
	return AccessData()->BuildWonder(wonder);
}

void Unit::AddWonder(sint32 type)
{
	AccessData()->AddWonder(type);
}

bool Unit::ChangeCurrentlyBuildingItem(sint32 category, sint32 item_type)
{
	return AccessData()->ChangeCurrentlyBuildingItem(category, item_type);
}

bool Unit::HaveImprovement(sint32 type) const
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

SpriteState * Unit::GetSpriteState() const
{
	return GetData()->GetSpriteState();
}

UnitActor * Unit::GetActor() const
{
	return GetData()->GetActor();
}

bool Unit::IsSameMovementType(uint32 bit_field) const
{
	return (GetDBRec()->GetMovementType() & bit_field) != 0;
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
	return (k_UNIT_VERSION_MAJOR<<16 | k_UNIT_VERSION_MINOR);
}

bool Unit::IsCity() const
{
	return GetData()->IsCity();
}

void Unit::GetPop(sint32 &p) const
{
	GetData()->GetPop(p);
}

//PFT 29 mar 05, show # turns until city next grows a pop
void Unit::GetTurnsToNextPop(sint32 &p) const
{
	GetData()->GetTurnsToNextPop(p);
}

void Unit::DrawCityStats(aui_Surface *surf, sint32 x, sint32 y)
{
}

void Unit::AddTradeRoute(TradeRoute &route, bool fromNetwork)
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

void Unit::AddUnitVision()
{
	AccessData()->AddUnitVision();
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

uint32 Unit::IsVisibilityClass(const uint32 bit) const
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

void Unit::PayFederalProduction(double percent_military,
                                sint32 &mil_paid,
                                double percent_terrain,
                                sint32 &mat_paid)

{
	AccessData()->PayFederalProduction(percent_military, mil_paid,
	                                   percent_terrain, mat_paid);
}

void Unit::PayFederalProductionAbs(sint32 mil_paid,
                                   double percent_mat,
                                   sint32 &mat_paid)

{
	AccessData()->PayFederalProductionAbs (mil_paid, percent_mat, mat_paid);
}

void Unit::SetMaterialContribution(bool on)
{
	AccessData()->SetMaterialContribution(on);
}

bool Unit::GetMaterialContribution() const
{
	return GetData()->GetMaterialContribution();
}

bool Unit::GetMilitaryContribution() const
{
	return GetData()->GetMilitaryContribution();
}

void Unit::SetMilitaryContribution(bool on)

{
	AccessData()->SetMilitaryContribution(on);
}

void Unit::SetIsProfessional(bool on)
{
	if (GetIsProfessional() != on) {
		g_player[GetOwner()]->RegisterProfessionalChange(on, *this);
		AccessData()->SetIsProfessional(on);
	}
}

bool Unit::GetIsProfessional() const
{
	return GetData()->GetIsProfessional();
}

bool Unit::GetNeedsNoSupport() const
{
	return GetDBRec()->GetNeedsNoSupport();
}

sint32 Unit::GetShieldCost() const
{
	return GetDBRec()->GetShieldCost();
}

bool Unit::IsEntrenched() const
{
	return GetData()->IsEntrenched();
}

bool Unit::IsEntrenching() const
{
	return GetData()->IsEntrenching();
}

bool Unit::CanEntrench() const
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

bool Unit::IsAsleep() const
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

void Unit::CalcHappiness(sint32 &virtualGoldSpent, bool firstPass)

{
	AccessData()->CalcHappiness(virtualGoldSpent, firstPass);
}

bool Unit::IsPatrolling() const
{
	return GetData()->IsPatrolling();
}

void Unit::SetPatrolling(bool patrolling)
{
	AccessData()->SetPatrolling(patrolling);
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

bool Unit::SafeFromNukes() const
{
	return GetData()->SafeFromNukes();
}

bool Unit::AttackFromSpaceship() const
{
	return GetDBRec()->GetAttackFromSpaceship();
}

bool Unit::HasAirport() const
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

bool Unit::HasCityWalls() const
{
	return GetData()->HasCityWalls();
}

bool Unit::HasForceField() const
{
	return GetData()->HasForceField();
}

ORDER_RESULT Unit::InvestigateCity(Unit c)
{
	return AccessData()->InvestigateCity(c);
}

ORDER_RESULT Unit::StealTechnology(Unit c, sint32 whichAdvance)
{
	return AccessData()->StealTechnology(c, whichAdvance);
}

ORDER_RESULT Unit::InciteRevolution(Unit c)
{
	return AccessData()->InciteRevolution(c);
}

ORDER_RESULT Unit::AssassinateRuler(Unit c)
{
	return AccessData()->AssassinateRuler(c);
}

ORDER_RESULT Unit::NullifyWalls(Unit c)
{
	return AccessData()->NullifyWalls(c);
}

bool Unit::HasBeenSpiedUpon() const
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

bool Unit::IsCapitol() const
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

sint32 Unit::GetProductionLostToFranchise() const
{
	return GetData()->GetProductionLostToFranchise();
}

void Unit::SetFranchiseTurnsRemaining(sint32 turns)
{
	AccessData()->SetFranchiseTurnsRemaining(turns);
}

sint32 Unit::GetFranchiseTurnsRemaining() const
{
	return GetData()->GetFranchiseTurnsRemaining();
}

bool Unit::CanSee(const Army &al) const
{
	return GetData()->CanSee(al);
}

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

bool Unit::IsWatchful() const
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

bool Unit::IsTempSlaveUnit() const
{
	return GetData()->IsTempSlaveUnit();
}

void Unit::SetTempSlaveUnit(bool b)
{
	AccessData()->SetTempSlaveUnit(b);
}

void Unit::BioInfect(sint32 player)
{
	AccessData()->BioInfect(player);
}

void Unit::NanoInfect(sint32 player)
{
	AccessData()->NanoInfect(player);
}

bool Unit::IsBioImmune() const
{
	return GetData()->IsBioImmune();
}

bool Unit::IsNanoImmune() const
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

sint32 Unit::GetConvertedGold() const
{
	return GetData()->GetConvertedGold();
}

bool Unit::IsCloaked() const
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
bool Unit::CanOpenRiftGate() const
{
	return GetDBRec()->CreateRift();
}

bool Unit::HasOpenRiftGate(MapPoint &destination) const
{
	return GetDBRec()->CreateRift() && GetData()->HasOpenRiftGate(destination);
}

bool Unit::OpenRiftGate(const MapPoint &destination)
{
	return GetDBRec()->CreateRift() && AccessData()->OpenRiftGate(destination);
}

bool Unit::IsTravellingRift() const
{
	return GetData()->IsTravellingRift();
}

void Unit::SetTravellingRift()
{
	AccessData()->SetTravellingRift();
}
#endif

bool Unit::HasResource(const sint32 resource) const
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

bool Unit::IsNanoInfected() const
{
	return GetData()->IsNanoInfected();
}

bool Unit::IsBioInfected() const
{
	return GetData()->IsBioInfected();
}

bool Unit::IsFranchised() const
{
	return GetData()->IsFranchised();
}

bool Unit::IsConverted() const
{
	return GetData()->IsConverted();
}

bool Unit::BuyFront()
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

bool Unit::CanBuildUnit(sint32 type) const
{
	return GetData()->CanBuildUnit(type);
}

bool Unit::CanBuildBuilding(sint32 type) const
{
	return GetData()->CanBuildBuilding(type);
}

bool Unit::CanBuildCapitalization() const
{
	return GetData()->CanBuildCapitalization();
}

bool Unit::CanBuildWonder(sint32 type) const
{
	return GetData()->CanBuildWonder(type);
}

void Unit::Injoin(sint32 player)
{
	AccessData()->Injoin(player);
}

bool Unit::IsInjoined() const
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

bool Unit::GetCurrentOrderString(StringId &id) const
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

bool Unit::AiGetCargoMovementPoints(double &min_move_points,
                                    bool  &first) const
{
	return GetData()->AiGetCargoMovementPoints(min_move_points, first);
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

bool Unit::IsInjured() const
{
	return AccessData()->CalculateTotalHP() > GetHP();
}

void Unit::RecalculateResources()
{
	AccessData()->RecalculateResources();
}

bool Unit::FightOneLineDanceRangedAttack(Unit &defender)
{
	return AccessData()->FightOneLineDanceRangedAttack(defender);
}

bool Unit::FightOneLineDanceAssault(Unit &defender)
{
	return AccessData()->FightOneLineDanceAssault(defender);
}

sint32 Unit::CountTradeWith(PLAYER_INDEX player) const
{
	return GetData()->CountTradeWith(player);
}

double Unit::IsProtectedFromSlavery() const
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

bool Unit::HasLeftMap() const
{
	return GetData()->HasLeftMap();
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

bool Unit::CanPerformSpaceTransitionNow() const
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






bool Unit::CanPlantNuke(const MapPoint &pos)
{
	return AccessData()->CanPlantNuke(pos);
}

bool Unit::CanMakePark(const MapPoint &pos)
{
	return AccessData()->CanMakePark(pos);
}
bool Unit::CanUndergroundRailway(const MapPoint &pos)
{
	return AccessData()->CanUndergroundRailway(pos);
}

bool Unit::CanConvert(const MapPoint &pos)
{
	return AccessData()->CanConvert(pos);
}

bool Unit::CanEstablishEmbassy(const MapPoint &pos)
{
	return AccessData()->CanEstablishEmbassy(pos);
}

bool Unit::CanCreateFranchise(const MapPoint &pos)
{
	return AccessData()->CanCreateFranchise(pos);
}

bool Unit::CanAssasinateRuler(const MapPoint &pos)
{
	return AccessData()->CanAssasinateRuler(pos);
}

bool Unit::CanStealTechnology(const MapPoint &pos)
{
	return AccessData()->CanStealTechnology(pos);
}

bool Unit::CanInjoin(const MapPoint &pos)
{
	return AccessData()->CanInjoin(pos);
}

bool Unit::CanInciteRevolution(const MapPoint &pos)
{
	return AccessData()->CanInciteRevolution(pos);
}

bool Unit::CanCauseUnhappiness(const MapPoint &pos)
{
	return AccessData()->CanCauseUnhappiness(pos);
}

bool Unit::CanExpel(const MapPoint &pos)
{
	return AccessData()->CanExpel(pos);
}

void Unit::AddEndGameObject(sint32 type)
{
	AccessData()->AddEndGameObject(type);
}

bool Unit::SendSlaveTo(Unit dest)
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

bool Unit::Flag(uint32 flag) const
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

bool Unit::BuildEndGame(sint32 type)
{
	return AccessData()->BuildEndGame(type);
}

void Unit::DisbandCity()
{
	AccessData()->DisbandCity();
}

bool Unit::CanPerformSpecialAction() const
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


		bool updateZoc = true;
		if(cell->GetCity().IsValid() &&
		   cell->GetCity().GetOwner() == GetOwner())
        {
			updateZoc = false;
		}
        else if(units && units->GetOwner() == GetOwner())
        {
			for(i = 0; i < units->Num(); i++) {
				if(!units->Access(i).IsNoZoc()) {
					updateZoc = false;
					break;
				}
			}
		}

		if(updateZoc) {
			g_theWorld->RemoveZOC(pos, GetOwner());
			g_theWorld->AddOtherArmyZOC(pos, GetOwner(), Army(), Unit());

			sint32 dd;
			for(dd = 0; dd < (sint32)NOWHERE; dd++) {
				MapPoint npos;
				if(pos.GetNeighborPosition((WORLD_DIRECTION)dd, npos)) {
					g_theWorld->RemoveZOC(npos, GetOwner());
					g_theWorld->AddOtherArmyZOC(npos, GetOwner(), Army(), Unit());
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

bool UnitCanCarry(sint32 dest, sint32 src, sint32 government)
{
	const UnitRecord *drec = g_theUnitDB->Get(dest, government);
	const UnitRecord *srec = g_theUnitDB->Get(src, government);

	if (!drec->GetCanCarry()) {
		return false;
	}

	if (srec->GetSizeSmall()) {

		if (drec->GetCanCarrySmallLand() && srec->GetMovementTypeLand()) {
			return true;
		}

		if (drec->GetCanCarrySmallAir() && srec->GetMovementTypeAir()) {
			return true;
		}

		if (drec->GetCanCarrySmallWater() && srec->GetMovementTypeSea()) {
			return true;
		}

		if (drec->GetCanCarrySmallSpace() && srec->GetMovementTypeSpace()) {
			return true;
		}
	}else if (srec->GetSizeMedium()) {
		if (drec->GetCanCarryMedLand() && srec->GetMovementTypeLand()) {
			return true;
		}

		if (drec->GetCanCarryMedAir() && srec->GetMovementTypeAir()) {
			return true;
		}

		if (drec->GetCanCarryMedWater() && srec->GetMovementTypeSea()) {
			return true;
		}

		if (drec->GetCanCarryMedSpace() && srec->GetMovementTypeSpace()) {
			return true;
		}
	} else if (srec->GetSizeLarge()) {
		if (drec->GetCanCarryLargeLand() && srec->GetMovementTypeLand()) {
			return true;
		}

		if (drec->GetCanCarryLargeAir() && srec->GetMovementTypeAir()) {
			return true;
		}

		if (drec->GetCanCarryLargeWater() && srec->GetMovementTypeSea()) {
			return true;
		}

		if (drec->GetCanCarryLargeSpace() && srec->GetMovementTypeSpace()) {
			return true;
		}
	}
	return false;
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
	return GetFuel() <= g_theConstDB->Get(0)->GetNonSpaceFuelCost() * (GetMovementPoints() / 100.0);
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
//  Called by  : -
//
//----------------------------------------------------------------------------
bool Unit::UnitValidForOrder(const OrderRecord * order_rec) const
{
	const UnitRecord *unit_rec = GetDBRec();

	const UnitRecord::ChanceEffect *chance_data;
	const UnitRecord::InvestigateCityData *investigate_data;
	const UnitRecord::StealTechnologyData *steal_data;
	const UnitRecord::InciteRevolutionData *incite_data;
	const UnitRecord::AssasinateRulerData *assasinate_data;
	const UnitRecord::CauseUnhappinessData *unhappiness_data;
	const UnitRecord::PlantNukeData *plant_data;
	const UnitRecord::SlaveRaidsData *raid_data;
	const UnitRecord::SuccessDeathEffect *success_death_data;

	bool order_valid = false;

	if(order_rec->GetUnitPretest_CanAttack())
		order_valid = (unit_rec->GetAttack() > 0.0 );
	else if(order_rec->GetUnitPretest_CanEntrench())
		order_valid = unit_rec->GetCanEntrench();
	else if(order_rec->GetUnitPretest_CanSueFranchise())
		order_valid = unit_rec->GetCanSue();
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
		order_valid = unit_rec->HasSettlerSlaveRaids();
	else if(order_rec->GetUnitPretest_CanUndergroundRailway())
		order_valid = unit_rec->GetUndergroundRailway(success_death_data);
	else if(order_rec->GetUnitPretest_CanInciteUprising())
		order_valid = unit_rec->HasSlaveUprising();
	else if(order_rec->GetUnitPretest_CanBioTerror())
		order_valid = unit_rec->GetBioTerror(chance_data);
	else if(order_rec->GetUnitPretest_CanPlague())
		order_valid = unit_rec->GetPlague(chance_data);
	else if(order_rec->GetUnitPretest_CanNanoInfect())
		order_valid = unit_rec->GetNanoTerror(chance_data);
	else if(order_rec->GetUnitPretest_CanConvertCity())
		order_valid = unit_rec->GetConvertCities(success_death_data);
	else if(order_rec->GetUnitPretest_CanReformCity())
		order_valid = unit_rec->HasCanReform();
	else if(order_rec->GetUnitPretest_CanSellIndulgences())
		order_valid = unit_rec->HasIndulgenceSales();
//	else if(order_rec->GetUnitPretest_CanFaithHeal())
//		order_valid = false; // Is sell indulgence
	else if(order_rec->GetUnitPretest_CanSoothsay())
		order_valid = unit_rec->HasCanSoothsay();
	else if(order_rec->GetUnitPretest_CanCreatePark())
		order_valid = unit_rec->HasCreateParks();
	else if(order_rec->GetUnitPretest_CanPillage())
		order_valid = unit_rec->GetCanPillage();
	else if(order_rec->GetUnitPretest_CanInjoin())
		order_valid = unit_rec->HasCanInjoin();
	else if(order_rec->GetUnitPretest_CanInterceptTrade())
		order_valid = unit_rec->GetCanPirate();
	else if(order_rec->GetUnitPretest_CanAdvertise())
		order_valid = unit_rec->GetAdvertise();
	else if(order_rec->GetUnitPretest_CanNukeCity())
		order_valid = unit_rec->HasNuclearAttack();
	else if(order_rec->GetUnitPretest_CanTransport())
		order_valid = unit_rec->HasCargoData();
	else if(order_rec->GetUnitPretest_CanBeTransported())
		order_valid = unit_rec->GetSizeMedium() ||
		              unit_rec->GetSizeSmall();
	else if(order_rec->GetUnitPretest_CanLaunch())
		order_valid = unit_rec->HasSpaceLaunch();
	else if(order_rec->GetUnitPretest_CanTarget())
		order_valid = unit_rec->HasNuclearAttack();
	else if(order_rec->GetUnitPretest_NoFuelThenCrash()
	     && unit_rec->GetNoFuelThenCrash()
	     &&!unit_rec->GetSingleUse())
		order_valid = NeedsRefueling();
//	else if(order_rec->GetUnitPretest_CanParadrop())
//		order_valid = unit_rec->GetParatrooperTransport();
	else if(order_rec->GetUnitPretest_CanUpgrade())
	{
		if(g_theProfileDB->IsUpgrade())
		{
			order_valid = (GetData()->GetBestUpgradeUnitType() >= 0);
		}
		else
		{
			order_valid = false;
		}
	}
	else if(order_rec->GetUnitPretest_None())
		order_valid = true;

	return order_valid;
}

bool Unit::Sink(sint32 chance)
{
	const UnitRecord *urec = GetDBRec();
	const TerrainRecord * trec = g_theTerrainDB->Get(g_theWorld->GetTerrainType(RetPos()));

	if(urec->GetCanSinkInSea()
	&& trec->GetMovementTypeSea()
	){
		if(g_rand->Next(100) < chance)
		{
			// Maybe something else than CAUSE_REMOVE_ARMY_DISBANDED
			// or it least call it diferently

			SlicObject *so = new SlicObject("999LostAtSea");
			so->AddRecipient(GetOwner());
			so->AddUnitRecord(GetType());
			g_slicEngine->Execute(so);
			KillUnit(CAUSE_REMOVE_ARMY_DISBANDED, -1);

			return true;
		}
	}

	return false;
}

bool Unit::IsHiddenNationality() const //emod to map to unit actor andmake color same as barbarians
{
	return GetDBRec()->GetHiddenNationality();
}
//elite units 6-5-2007
sint32 Unit::IsElite() const
{
	return GetData()->IsElite();
}

void Unit::SetElite()
{
	AccessData()->SetElite();
}

void Unit::UnElite()
{
	AccessData()->UnElite();
}
