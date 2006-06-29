//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Unit data
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
// _DEBUG
// - Generate debug version
//
// CTP1_TRADE
// - Creates an executable with trade like in CTP1. Currently broken.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Healing in fort handled as in city.
// - Movement point handling for ships at tunnels corrected.
// - Handle unit types that have CanCarry property, but have MaxCargo 0.
// - Corrected movement type check for active defenders.
// - Added second message after investigation of a city.
// - Standardised min/max usage.
// - Modified UDUnitTypeCanSettle so that it checks for a CanSettleOn 
//   unit property so that modders can make  settling terrain-specific. - E
// - Fixed a broken comparision in UDUnitTypeCanSettle so that it now allows 
//   settling again. - Mar. 1st 2005 Martin Gühmann
// - When a city is conquered vision is now removed afterwards the city data
//   has changed hand. That allows the creation of UnseenCell's with the 
//   current owner. If you loose a city to someone else you know who the 
//   b*st*rd is. When changing hands the city isn't anymore removed and
//   then added back to the wolrd. - Mar. 4th 2005 Martin Gühmann
// - Added GetTurnsToNextPop(sint32 &p)const; PFT 29 mar 05, to help show 
//   # turns until city grows
// - Implemented immobile units (set MaxMovePoints equal to 0 in units.txt) 
//   PFT 17 Mar 05
// - Replaced a comma by a semicolon in the Serialize method. - May 19th 2005 Martin Gühmann
// - Removed some unsused method to removed some unused in methods in
//   CityData. - Aug 6th 2005 Martin Gühmann
// - Removed another unused and unecessary function. (Aug 12th 2005 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Added city data to "settle too close"-report.
// - NonLethalBombard implemented in UnitData::Bombard 15-FEB-2006 
// - Added MoveBonus to DeductMove so we can AllTerrainAsRoad-like units 3-31-2006 by E
// - Old settle terrain flags are ignored if new CanSettleOn terrain flags are 
//   defined. (April 22nd 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "UnitData.h"

#include "ConstDB.h"
#include "StrDB.h"
#include "WonderRecord.h"

#include "GameObj.h"

#include "UnitDynArr.h"

#include "citydata.h"
#include "World.h"
#include "Cell.h"

#include "SpriteState.h"
#include "UnitActor.h"
#include "player.h"
#include "RandGen.h"
#include "QuadTree.h"
#include "installationtree.h"
#include "installation.h"

#include "UnitDynArr.h"
#include "Advances.h"
#include "Readiness.h"

#include "network.h"
#include "net_action.h"
#include "net_info.h"
#include "net_unit.h"

#include "c3math.h"
#include "Vision.h"
#include "CivilisationPool.h"
#include "SelItem.h"
#include "tiledmap.h"
#include "director.h"
#include "SlicEngine.h"
#include "SlicObject.h"

#include "AICause.h"
#include "HappyTracker.h"
#include "AchievementTracker.h"

#include "TurnCnt.h"

#include "Agreement.h"
#include "Wormhole.h"
#include "victorymoviewin.h"
#include "cellunitlist.h"
#include "Order.h"

#include "Diffcly.h"

#include "ArmyPool.h"
#include "TradeOfferPool.h"
#include "soundmanager.h"
#include "gamesounds.h"
#include "TerrainRecord.h"

#include "UnitPool.h"
#include "AdvanceRecord.h"

#include "TradeBids.h"
#include "TradePool.h"

#include "profileDB.h"

#include "bset.h"

#include "MaterialPool.h"
#include "SpecialAttackInfoRecord.h"
#include "SpecialEffectRecord.h"
#include "SpriteRecord.h"
#include "UnitRecord.h"
#include "unitutil.h"

#include "MoveFlags.h"

#include "GameEventManager.h"

#include "wonderutil.h"

#include "AgreementMatrix.h"

#include "BitMask.h"

#include "FeatTracker.h"

#include "TerrainImprovementRecord.h"
#include "terrainutil.h"
#include "buildingutil.h"
#include "BuildingRecord.h"
#include "Gold.h"
#include "ArmyData.h"
#include "WonderTracker.h"

#include "citywindow.h"

#ifdef _DEBUG
#include "aui.h"
#include "controlpanelwindow.h"
extern ControlPanelWindow   *g_controlPanel;
#endif

extern QuadTree<Unit>       *g_theUnitTree;

extern SoundManager         *g_soundManager;

extern bool UnitCanCarry(sint32 dest, sint32 src, sint32 government);









UnitData::UnitData(
    const sint32 t,
    const sint32 trans_t,
    const Unit &i,
    const PLAYER_INDEX o,
    const MapPoint &center_pos,
    const Unit hc,
    UnitActor *actor
    ) : GameObj(i.m_id)
                   
{







	m_pos = center_pos;

	Create(t, trans_t, i, o);
	Place(center_pos);

	if(actor) {
		m_actor = actor;
		Unit actorId = actor->GetUnitID();
		
		g_director->AddMorphUnit(m_actor, m_sprite_state, t, Unit(m_id));
		if(g_network.IsHost()) {
			g_network.Block(m_owner);
			g_network.Enqueue(this, actorId);
			g_network.Unblock(m_owner);
		}
		m_actor->SetNewUnitVisionRange((GetVisionRange()));
	} else {
		m_actor = new UnitActor(m_sprite_state, Unit(m_id), m_type, center_pos,
								m_owner, FALSE, (GetVisionRange()),
								m_city_data ? m_city_data->GetDesiredSpriteIndex() : 0);
		m_actor->SetUnitVisionRange((GetVisionRange()));
		m_actor->SetUnitVisibility(m_visibility);

		g_network.Enqueue(this);
	}
}

UnitData::UnitData(const sint32 t,
				   const sint32 trans_t,
				   const Unit &i,
				   const PLAYER_INDEX o,
				   const MapPoint &actor_pos) : GameObj(i.m_id)
{

	Create(t, trans_t, i, o);

	m_visibility = 0xffffffff;
	m_temp_visibility = 0xffffffff;
	m_radar_visibility = 0xffffffff;
	m_actor = new UnitActor(m_sprite_state, Unit(m_id), m_type, actor_pos,
							m_owner, FALSE, (GetVisionRange()), 0);
	m_actor->SetUnitVisionRange((GetVisionRange()));

	m_pos = actor_pos;	
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::Place
//
// Description: Place this unit at MapPoint &center_pos
//
// Parameters : MapPoint &center_pos
// 
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Also places ZOC around the unit.
//
//----------------------------------------------------------------------------
void UnitData::Place(const MapPoint &center_pos)
{
	m_pos = center_pos;

	if(!g_theUnitDB->Get(m_type)->GetIsTrader()) {
		bool revealedUnexplored = false;
		SetVisible(m_owner);
		AddUnitVision(revealedUnexplored);

		if(g_theWorld->GetCell(center_pos)->GetGoodyHut()) {
			g_theWorld->GetCell(center_pos)->DeleteGoodyHut();
		}

		if(!g_theUnitDB->Get(m_type)->GetNoZoc() &&
		   (!g_theWorld->HasCity(m_pos) || g_theUnitDB->Get(m_type)->GetHasPopAndCanBuild())) {
			sint32 dd;
			MapPoint chk;
			g_theWorld->AddZOC(center_pos, m_owner);
			uint32 moveType = g_theUnitDB->Get(m_type)->GetMovementType();
			for(dd = (sint32)NORTH; dd < (sint32)NOWHERE; dd++) {
				if(center_pos.GetNeighborPosition((WORLD_DIRECTION)dd, chk)) {
					if(g_theWorld->CanEnter(chk, moveType)) {
						g_theWorld->AddZOC(chk, m_owner);
					}
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::Create
//
// Description: Create a unit of type t with id i.m_id and owner o (at m_pos)
//
// Parameters : sint32       t  : the unit's type
//              sint32 trans_t  : ? g_theUnitDB->Get(t)->GetTransType(), see UnitPool::Create
//              Unit        &i  :
//              PLAYER_INDEX o  : the unit's owner
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void UnitData::Create(const sint32 t,
				 const sint32 trans_t,
				 const Unit &i,
				 const PLAYER_INDEX o)
{
	DPRINTF(k_DBG_GAMESTATE, ("Creating unit: id: %d, type: %d, owner: %d\n",
							  i.m_id, t, o));
	const UnitRecord *rec = g_theUnitDB->Get(t);

	m_flags = 0;
	m_actor = 0;
	m_type = t;

#ifdef _DEBUG
	m_text[0] = 0; 
#endif

	m_owner = o;

//EMOD add civ bonus

	sint32 wonderHPBonus = wonderutil_GetIncreaseHP(g_player[m_owner]->m_builtWonders);
 //   sint32 buildingHPBonus = buildingutil_GetIncreaseHP(m_city_data->GetEffectiveBuildings()); //EMOD 4-27-2006 m_city_data or cityData?
//	m_hp = g_theUnitDB->Get(t)->GetMaxHP() + wonderHPBonus + buildingHPBonus;
		m_hp = g_theUnitDB->Get(t)->GetMaxHP() + wonderHPBonus;
	m_movement_points = g_theUnitDB->Get(t)->GetMaxMovePoints();

	//PFT 17 Mar 05, don't FIRST_MOVE flag immobile units
	if( !IsImmobile() )
		SetFlag(k_UDF_FIRST_MOVE);

	m_fuel = g_theUnitDB->Get(t)->GetMaxFuel(); 

	sint32 amt;
	if((rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater())) {
		if((amt = wonderutil_GetIncreaseBoatMovement(g_player[m_owner]->m_builtWonders)) > 0) {
			m_movement_points += amt;
		}
		if((amt = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_BOAT_MOVEMENT, m_owner)) > 0) {
			m_movement_points += amt;
		}
	}
// EMOD Crashes because the first unit doesn't have a city 
//	sint32 buildingHPBonus = 0;
//	if((buildingHPBonus = buildingutil_GetIncreaseHP(m_city_data->GetEffectiveBuildings())) > 0) {
//			m_hp += buildingHPBonus;
//	}

	if(rec->GetCanCarry()
	&& rec->GetCargoDataPtr()
	&&(0 < rec->GetCargoDataPtr()->GetMaxCargo())
	){
		m_cargo_list = new UnitDynamicArray(rec->GetCargoDataPtr()->GetMaxCargo());
	} else {
		m_cargo_list = NULL; 
	}

	if (g_theUnitDB->Get(m_type)->GetHasPopAndCanBuild())
	{
		m_city_data = new CityData(m_owner, i, m_pos);
	}
	else
		m_city_data = NULL;

	m_sprite_state = new SpriteState(g_theUnitDB->Get(t)->GetDefaultSprite()->GetValue());

	m_visibility = 0;
	m_temp_visibility = 0;
	m_radar_visibility = 0;
	m_ever_visible = (1 << 0);

	m_transport.m_id = (0);

	m_roundTheWorldMask = new BitMask(g_theWorld->GetXWidth());
	m_roundTheWorldMask->SetBit(m_pos.x);
}

UnitData::UnitData(CivArchive &archive) : GameObj(0)
{
#ifdef _DEBUG
	m_text[0] = 0;
#endif

	m_cargo_list = NULL;
	m_city_data = NULL;
	m_actor = NULL;
	m_sprite_state = NULL;
	m_lesser = NULL;
	m_greater = NULL;
	
	Serialize(archive);
}

UnitData::~UnitData()
{
	delete m_cargo_list;
	delete m_city_data;
	delete m_actor;
	delete m_roundTheWorldMask;
	delete m_lesser;
	delete m_greater;
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::SetPosAndNothingElse
//
// Description: Set this unit's position to MapPoint &p
//
// Parameters : MapPoint &p
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : 
//
//----------------------------------------------------------------------------
void UnitData::SetPosAndNothingElse(const MapPoint &p)
{
	m_pos = p; 

	Assert(g_player[m_owner]); 
	g_player[m_owner]->RegisterYourArmyWasMoved(m_army, m_pos);
}

void UnitData::SetPos(const MapPoint &p, bool &revealed_unexplored,
                      bool &left_map)
{
	
	if(Flag(k_UDF_IS_ENTRENCHED)) {
		ClearFlag(k_UDF_IS_ENTRENCHED);
	}

	
	RemoveUnitVision();
	
	UndoVision();
	m_pos = p;

	
	m_roundTheWorldMask->SetBit(m_pos.x);
	const UnitRecord *rec = g_theUnitDB->Get(m_type);
	if(m_roundTheWorldMask->AllBitsSet() && (rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater())) {
		g_featTracker->AddFeat("FEAT_SAILED_AROUND_WORLD", m_owner);
	}

	if(g_wormhole && g_wormhole->CheckEnter(Unit(m_id))) {
		left_map = TRUE;
		SetFlag(k_UDF_HAS_LEFT_MAP);
		SetFlag(k_UDF_IN_WORMHOLE);
		g_player[m_owner]->m_readiness->UnsupportUnit(Unit(m_id),
													  g_player[m_owner]->m_government_type);
		g_player[m_owner]->RecoveredProbe(Unit());
	} else {
		left_map = false;
		AddUnitVision(revealed_unexplored);
		
		

		Cell *cell = g_theWorld->GetCell(p);
		if(g_wormhole) {
			if(g_theUnitDB->Get(m_type)->GetWormholeProbe() && Flag(k_UDF_RETURNED_FROM_WORMHOLE)) {
				if(cell->GetCity().m_id != 0) {
					g_player[m_owner]->RecoveredProbe(cell->GetCity());
				}
			}
		}
	}
	Assert(g_player[m_owner]); 
	g_player[m_owner]->RegisterYourArmyWasMoved(m_army, m_pos);
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::DeductMoveCost
//
// Description: Deducts cost from this unit's movement points.
//
//              If the unit doesn't use fuel, returns FALSE and sets out_of_fuel = FALSE
//
//              If the unit uses fuel, it trys to refuel it:
//                 if successful, returns FALSE and sets out_of_fuel = FALSE
//                 if unsuccessful, returns TRUE, sets out_of_fuel = TRUE, and kills human owned unit
//
// Parameters : Unit &me          : ?
//              double cost       : the cost to be deducted
//              BOOL &out_of_fuel : TRUE if deduction causes unit to run out of fuel
//
// Globals    : -
//
// Returns    : sint32 (TRUE or FALSE)
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 UnitData::DeductMoveCost(const Unit &me, const double cost, bool &out_of_fuel) 
{

	const UnitRecord *rec = g_theUnitDB->Get(m_type);

//EMOD
	sint32 bonus;
	if(rec->GetMoveBonus(bonus)) {  //EMOD
		m_movement_points -= bonus;
//	} else {
//		m_movement_points -= cost; //outcomment here so air isn't deducted twice
	}
	if(!Flag(k_UDF_PACMAN) && (rec->GetMoveBonus(bonus) == 0)) {  //needed to add this flag because it seemed to affect movebonus too

		m_movement_points -= cost;
		m_movement_points = std::max(m_movement_points, 0.0);
		ClearFlag(k_UDF_FIRST_MOVE);
	}




	out_of_fuel = false;
	if (!rec->GetNoFuelThenCrash()){
		return FALSE;
	} else {
		m_fuel -= g_theConstDB->NonSpaceFuelCost();

		if (m_fuel <= 0) {
			CheckForRefuel();//refuel the unit if it's in a city, airbase, or being transported

			if(m_fuel <= 0) { //refuelling failed, it's NoFuelThenCrash
				out_of_fuel = true;
			
				if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
				  (g_network.IsClient() && g_network.IsLocalPlayer(m_owner))) {
					m_army.AddDeath(Unit(m_id), CAUSE_REMOVE_ARMY_OUTOFFUEL, -1);
				}

				m_movement_points = 0;
				return TRUE;
			}
		}
	}
	return FALSE;
}

bool UnitData::IsImmobile()const {

	const UnitRecord *rec = g_theUnitDB->Get(m_type);

//	if (rec->GetCantmove())
//		return true;

	if (rec->GetMaxMovePoints() < 1.0 )
		return true;

	return false;
}

bool UnitData::CantGroup()const {

	const UnitRecord *rec = g_theUnitDB->Get(m_type);

	if (rec->GetCantGroup())
		return true;

	return false;
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::ResetMovement
//
// Description: Reset this unit's movement points to it's unitDB record's MaxMovePoints.
//
//              If the unit uses fuel, try to refuel it.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : sint32 (TRUE or FALSE)
//
// Remark(s)  : LossMoveToDmg not used but might be implemented by editing Units.txt
//
//----------------------------------------------------------------------------
sint32 UnitData::ResetMovement()
{
	const UnitRecord *rec = g_theUnitDB->Get(m_type);    

	if (rec->GetLossMoveToDmgNone()) { //all entries in Units.txt have this
		m_movement_points = rec->GetMaxMovePoints(); 
	} else if (rec->GetLossMoveToDmgTwo()) { 
		m_movement_points = 
			std::max(2.0, rec->GetMaxMovePoints() * m_hp * rec->GetMaxHPr());
	} else {
		m_movement_points = rec->GetMaxMovePoints() * m_hp * rec->GetMaxHPr();
	}

	sint32 amt;
	if((rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater()) &&
	   ((amt = wonderutil_GetIncreaseBoatMovement(g_player[m_owner]->m_builtWonders)) > 0))
	{
		m_movement_points += amt;
	}
	if((rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater()) &&
	   ((amt = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_BOAT_MOVEMENT, m_owner)) > 0))
	{
		m_movement_points += amt;
	}

	if(Flag(k_UDF_IS_ENTRENCHING)) {
		if(Flag(k_UDF_FIRST_MOVE)) {
			SetFlag(k_UDF_IS_ENTRENCHED);
			ClearFlag(k_UDF_IS_ENTRENCHING);
		}
	}

	if(IsImmobile()){//17 Mar 05, PFT, don't FIRST_MOVE flag immobile units
		m_movement_points = 0.0;
		ClearFlag(k_UDF_FIRST_MOVE);//speeds up debugging
	}
	else{
		SetFlag(k_UDF_FIRST_MOVE);
	}



	ClearFlag(k_UDF_ALREADY_PERFORMED_SPACE_TRANSITION);

	if (g_theUnitDB->Get(m_type)->GetNoFuelThenCrash()) {
		double origMovePoints = m_movement_points;
		CheckForRefuel();//this may zero out m_movement_points
		m_movement_points = origMovePoints;//so restore them
		ENQUEUE();
		return (0 < m_fuel);
	} else {

		ENQUEUE();
		return TRUE;
	}
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::GetInserted
//
// Description: This unit gets inserted into the transport Unit &transport
//
// Parameters : -
//
// Globals    : g_theWorld
//
// Returns    : -
//
// Remark(s)  : see next method
//
//----------------------------------------------------------------------------
void UnitData::GetInserted(const Unit &transport)
{
	Unit me(m_id);
	SetIsInTransport(transport);
	g_theWorld->RemoveUnitReference(m_pos, me);
	UndoVision();
	RemoveUnitVision();
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::GetInserted
//
// Description: Try to insert the Unit &addme into the m_cargo_list of this unit.
//
// Parameters : -
//
// Globals    : g_theUnitDB
//
// Returns    : -
//
// Remark(s)  : TRUE if successful.
//
//----------------------------------------------------------------------------
sint32 UnitData::InsertCargo(const Unit &addme)

{
    Assert (m_cargo_list); 
    
    if (m_cargo_list->Num() < g_theUnitDB->Get(m_type)->GetCargoDataPtr()->GetMaxCargo()) { 
        m_cargo_list->Insert(addme); 

		g_slicEngine->RunTrigger(TRIGGER_LIST_EMBARKED,
								 ST_UNIT, addme,
								 ST_PLAYER, m_owner,
								 ST_END);
        return TRUE; 
    } else {
        return FALSE; 
    }
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::GetCargoHP
//
// Description: Maps each unit in this transport's m_cargo_list onto it's (current) 
//              hit points and counts the total number of units carried.
//
// Parameters : sint32 &count          :the total number of units being carried.
//              sint32 unit_type[100]  :the unit_type of each unit
//              sint32 unit_hp[100]    :the corresponding current hit points
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : 100 is the max size of m_cargo_list.
//              Actually, not used.
//
//----------------------------------------------------------------------------
void UnitData::GetCargoHP(sint32 &count, sint32 unit_type[100], sint32 unit_hp[100]) const
{
    sint32 n = m_cargo_list->Num(); 
    sint32 j; 
	count = 0;
    for (j=0; j<n; j++) { 
        Assert(j < 100); 
        unit_type[j] = m_cargo_list->Get(j).GetType(); 
        unit_hp[j] = sint32(m_cargo_list->Get(j).GetHP()); 
        count++;
    } 
}

sint32 UnitData::PayWages(sint32 w)

{
	Assert(FALSE);
	return 0;
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::GetWagesNeeded
//
// Description: Calculate the wages that this city needs to pay it's citizens.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : 
//
//----------------------------------------------------------------------------
sint32 UnitData::GetWagesNeeded()
{

// EMOD - Add something like unit wages? each unit has a citizen wage but can be multiplied?

	Assert(m_city_data);
	if(m_city_data) {
		return m_city_data->GetWagesNeeded();
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::DelFromCargo
//
// Description: Delete the Unit delme from this transport's m_cargo_list.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : 
//
//----------------------------------------------------------------------------
void UnitData::DelFromCargo(const Unit & delme)
{ 
    Assert (m_cargo_list); 
    
    sint32 r = m_cargo_list->Del(delme); 
    Assert(r);
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::CanCarry
//
// Description: Check if this unit is a transport that can carry units whose type is sint32 src.
//
// Parameters : sint32 src : the unit type of the unit being checked.
//
// Globals    : -
//
// Returns    : sint32 (TRUE or FALSE)
//
// Remark(s)  : src must be the right size so that this unit can carry it.
//              See BOOL UnitCanCarry in Unit.cpp.
//
//----------------------------------------------------------------------------
sint32 UnitData::CanCarry(
                          const sint32 src 
                          ) const 
                          
{ 
    return UnitCanCarry(m_type, src, g_player[m_owner]->GetGovernmentType()); 
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::GetCargoCapacity
//
// Description: Get the number of currently free slots in this unit's m_cargo_list.
//
// Parameters : -
//
// Globals    : g_theUnitDB
//
// Returns    : sint32 the number of currently free slots in this unit's m_cargo_list.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 UnitData::GetCargoCapacity() const

{
	if (m_cargo_list) { 
		
		if (g_theUnitDB->Get(m_type)->GetCargoDataPtr())
			return g_theUnitDB->Get(m_type)->GetCargoDataPtr()->GetMaxCargo() - m_cargo_list->Num();
	}

	return 0; 
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::GetNumCarried
//
// Description: Get the number of units currently being carried by this unit.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : sint32 the number of units currently being carried by this unit.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 UnitData::GetNumCarried() const
{
	if(m_cargo_list) {
		return m_cargo_list->Num();
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::CargoHasLandUnits
//
// Description: Test if this unit is carrying land units in it's cargo.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : sint32 TRUE if this unit is carrying units that can move on land.
//
// Remark(s)  : Obsolete; it was used to warn a human that he was trying to disembark
//              from a cargo pod into water.
//
//----------------------------------------------------------------------------
BOOL UnitData::CargoHasLandUnits() const
{
    if (!m_cargo_list)
        return(FALSE);

	int n = m_cargo_list->Num();
	for (int i=0; i<n; i++) { 
        Unit *unit = &(m_cargo_list->Access(i));
        if (unit->GetMovementTypeLand() || unit->GetMovementTypeMountain()) {
            return(TRUE);
        }
    }

    return(FALSE);
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::IsMovePointsEnough
//
// Description: Test if this unit has enough move points to move into a cell whose
//              type is that of pos.
//
// Parameters : MapPoint &pos : the position whose type we're interested in.
//
// Globals    : -
//
// Returns    : BOOL
//
// Remark(s)  :
//
//----------------------------------------------------------------------------
BOOL UnitData::IsMovePointsEnough(const MapPoint &pos) const
{
	if (Flag(k_UDF_FIRST_MOVE)) {
		return TRUE;
	} else {
		double cost;
		sint32 fixMoveCosts;

		if (g_theUnitDB->Get(GetType())->GetMovementTypeAir() ) {
			cost = k_MOVE_AIR_COST;
		} else if(g_theUnitDB->Get(GetType())->GetMoveBonus(fixMoveCosts)) {
			cost = static_cast<double>(fixMoveCosts);

		// Prevent ships from diving under and using tunnels.
		} else if (g_theWorld->IsTunnel(pos) && 
		          !g_theUnitDB->Get(m_type)->GetMovementTypeLand()
		          )
		{
			sint32 icost;
			g_theWorld->GetTerrain(pos)->GetEnvBase()->GetMovement(icost);
			cost = icost;
		} else {
			cost = g_theWorld->GetMoveCost(pos);
		}

		return (cost <= m_movement_points );
	}
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::CanAtLeastOneCargoUnloadAt
//
// Description: Test if at least one of this unit's cargo list can unload into MapPoint dest_pos
//
// Parameters : MapPoint &old_pos   : ?
//              MapPoint &dest_pos  :
//              BOOL & use_vision   : if TRUE then use Vision::IsVisible(dest_pos)
//
// Globals    : -
//				
// Returns    : BOOL
//
// Remark(s)  :
//
//---------------------------------------------------------------------------- 
BOOL UnitData::CanAtLeastOneCargoUnloadAt(const MapPoint &old_pos, const MapPoint &dest_pos, const BOOL & use_vision) const
{
	if(!m_cargo_list)
		return FALSE;

    sint32 cargo_idx;
    sint32 cargo_num; 
    cargo_num = m_cargo_list->Num(); 

    for (cargo_idx=0; cargo_idx < cargo_num; cargo_idx++) { 
        if (CanThisCargoUnloadAt((*m_cargo_list)[cargo_idx], old_pos, dest_pos, use_vision)) {
           return TRUE; 
        }
    }
    return FALSE; 
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::CanThisCargoUnloadAt
//
// Description: Returns TRUE if Unit the_cargo can unload into MapPoint new_pos 
//
// Parameters : Unit & the_cargo
//              MapPoint & old_pos    : not used 
//              MapPoint & new_pos
//              BOOL & use_vision     : if TRUE then use Vision::IsVisible(new_pos)
//
// Globals    : -
//				
// Returns    : BOOL
//
// Remark(s)  : 
//
//---------------------------------------------------------------------------- 
BOOL UnitData::CanThisCargoUnloadAt(const Unit & the_cargo, const MapPoint & old_pos, 
                                    const MapPoint & new_pos, const BOOL & use_vision) const
{ 
    Assert(m_cargo_list); 

    const UnitData *the_cargo_data = the_cargo.AccessData(); 
    Assert(the_cargo_data);
    
    if (!the_cargo_data->IsMovePointsEnough(new_pos)) 
        return FALSE; 

    Cell *the_dest = g_theWorld->GetCell(new_pos); 
    CellUnitList *the_dest_army = the_dest->UnitArmy();

    if (0 < the_dest_army->Num())
        if (the_dest_army->Access(0).GetOwner() == m_owner)
            if (k_MAX_ARMY_SIZE <= the_dest_army->Num()) 
                return FALSE; 

    BOOL check_baddies; 
    if (use_vision) { 
        check_baddies = g_player[m_owner]->IsVisible(new_pos); 
    } else { 
        check_baddies = TRUE; 
    } 

    if (check_baddies) { 

        if (0 < the_dest_army->Num())
            if (the_dest_army->Access(0).GetOwner() != m_owner)
                if (!the_cargo->CanBeachAssaultRightNow()) 
                    return FALSE;

        if(m_pos != new_pos) { 
            if (!the_cargo.IsIgnoresZOC() && 
                g_theWorld->IsMoveZOC(m_owner, m_pos, new_pos, FALSE)) 
                return FALSE; 
        }
    }

    if (the_dest->GetCity().m_id != 0) {  

        if (the_dest->GetCity().GetOwner() != m_owner) { 
            if (the_cargo.GetDBRec()->GetCantCaptureCity())
                return FALSE; 
        }

    } else { 

        if (!g_theWorld->CanEnter(new_pos, the_cargo.GetMovementType())) 
            return FALSE; 
    }   
    return TRUE; 
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::UnloadCargo
//
// Description: Try to unload (maybe one of) this unit's m_cargo_list onto MapPoint &new_pos. 
//
// Parameters : MapPoint &new_pos     : the position to unload to
//              Army     &debark      : a new army formed from the debarked units
//              BOOL     justOneUnit  : if TRUE then only unload &theUnit
//              Unit     &theUnit     : the unit determined by justOneUnit
//
// Globals    : -
//				
// Returns    : BOOL TRUE when done (not success/failure)
//
// Remark(s)  : Better to check max_debark > 0 before returning TRUE ? 
//
//----------------------------------------------------------------------------
BOOL UnitData::UnloadCargo(const MapPoint &new_pos, Army &debark,
						   BOOL justOneUnit, const Unit &theUnit)

{
	if(!m_cargo_list)
		return FALSE;

	Cell *cell = g_theWorld->GetCell(new_pos);

	sint32 max_debark;
	if(cell->UnitArmy() && cell->UnitArmy()->GetOwner() != m_owner) {
	//compute the max number of units we can unload	
		max_debark = k_MAX_ARMY_SIZE - g_theWorld->GetCell(m_pos)->GetNumUnits();
	} else {
        max_debark = std::min
            (k_MAX_ARMY_SIZE - cell->GetNumUnits(),
			 k_MAX_ARMY_SIZE - g_theWorld->GetCell(m_pos)->GetNumUnits()
            );
	}
	sint32 const        n       = m_cargo_list->Num();
	sint32 count = 0;
    Unit passenger; 

	for (sint32 i=n-1; 0 <=i ; i--) { 

		if(justOneUnit && theUnit != m_cargo_list->Access(i))
			continue;

        if (max_debark <= count) 
            break; 

        if (CanThisCargoUnloadAt(m_cargo_list->Access(i), m_pos, new_pos, FALSE)) { 

            count++;
            
            passenger = (*m_cargo_list)[i];
            m_cargo_list->DelIndex(i); 
            passenger .SetPosAndNothingElse(m_pos);     
 		    passenger .UnsetIsInTransport(); 
  
			UnitDynamicArray revealedUnits;
			g_theWorld->InsertUnit(m_pos, passenger, revealedUnits);

			bool revealedUnexplored;
			passenger.AddUnitVision(revealedUnexplored);
			
			debark.Insert(passenger );  
        }
	} 	
	return TRUE;
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::UnloadSelectedCargo
//
// Description: Try to unload this unit's m_cargo_list onto MapPoint &new_pos. 
//
// Parameters : MapPoint &new_pos     : the position to unload to
//              Army     &debark      : a new army formed from the debarked units
//
// Globals    : -
//				
// Returns    : BOOL TRUE when done (not success/failure)
//
// Remark(s)  : 
//
//----------------------------------------------------------------------------
BOOL UnitData::UnloadSelectedCargo(const MapPoint &new_pos, Army &debark)
{	
	if(!m_cargo_list)
		return FALSE;

	Cell *cell = g_theWorld->GetCell(new_pos);
    //how many units can we unload
    sint32 max_debark = 
        std::min(k_MAX_ARMY_SIZE - cell->GetNumUnits(),
		         k_MAX_ARMY_SIZE - g_theWorld->GetCell(m_pos)->GetNumUnits()
                );// not m_cargo_list ?

	sint32 const        n       = m_cargo_list->Num();
	sint32 count = 0;
    Unit passenger; 

	for (sint32 i=n-1; 0 <=i ; i--) { 
		if(!m_cargo_list->Access(i).Flag(k_UDF_TEMP_TRANSPORT_SELECT))
			continue;

		m_cargo_list->Access(i).ClearFlag(k_UDF_TEMP_TRANSPORT_SELECT);

        if (max_debark <= count) 
            break; 

        if (CanThisCargoUnloadAt(m_cargo_list->Access(i), m_pos, new_pos, FALSE)) { 

            count++;
            
            passenger = (*m_cargo_list)[i];
            m_cargo_list->DelIndex(i); 
            passenger .SetPosAndNothingElse(m_pos);     
 		    passenger .UnsetIsInTransport(); 
  
			UnitDynamicArray revealedUnits;
			g_theWorld->InsertUnit(m_pos, passenger, revealedUnits);
            g_player[m_owner]->RegisterUnloadCargo(m_army.m_id, passenger.GetType(), (sint32)passenger.GetHP()); 

			bool revealedUnexplored;
			passenger.AddUnitVision(revealedUnexplored);
			
			debark.Insert(passenger );  
        }
	}	
	return TRUE;
}

// this is pre-CTP1 release:
sint32 UnitData::CanRustle(CellUnitList &defender) const 
{
	if (defender.GetOwner() == GetOwner())
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;

	const UnitRecord *attack_rec, *defend_rec; 
	attack_rec = g_theUnitDB->Get(m_type);

	if(!attack_rec->GetCanRustle())
		return false;

	if (attack_rec->GetMovementTypeAir())
		return false;

	sint32 defenders = defender.Num();
	sint32 i;
	for (i=0; i< defenders; i++)
		{
			defend_rec = defender[i].GetDBRec();
			if (!defend_rec->GetCanBeRustled())
				return false;
		}
	return true;
}

sint32 UnitData::CanConvertCity(Unit &city) const 
{
	if(city.GetOwner() == GetOwner()) {
		return false;
	}
	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;
	return true;
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::CanBombard
//
// Description: Check if this unit is in position to, and able to, bombard 
//              at least one member of the CellUnitList &defender 
//
// Parameters : CellUnitList &defender	: the defending army (list of units)
//
// Globals    : g_theUnitDB	: unit properties database
//
// Returns    : sint32		: TRUE or FALSE
//
// Remark(s)  : 
//
//----------------------------------------------------------------------------
sint32 UnitData::CanBombard(CellUnitList &defender) const 

{
   MapPoint dpos;
   defender.GetPos(dpos);

   const UnitRecord *rec = g_theUnitDB->Get(m_type);

   sint32 rge;
   rec->GetBombardRange(rge);
   sint32 dist = m_pos.NormalizedDistance(dpos);

   if(dist > rge)
	   return FALSE;

   if (rec->GetCanBombardLand()) 
      if (defender.IsAtLeastOneMoveLand())
         return TRUE; 

   if (rec->GetCanBombardWater()) 
      if (defender.IsAtLeastOneMoveWater() || defender.IsAtLeastOneMoveShallowWater())
         return TRUE; 

   if (rec->GetCanBombardAir()) 
      if (defender.IsAtLeastOneMoveAir())
         return TRUE; 

   if (rec->GetCanBombardSpace()) 
      if (defender.IsAtLeastOneMoveSpace())
         return TRUE; 

   if (rec->GetCanBombardMountain()) 
      if (defender.IsAtLeastOneMoveMountain())
         return TRUE; 

   return FALSE; 
}

sint32 UnitData::CanCounterBombard(CellUnitList &defender) const 
{
   if (g_theUnitDB->Get(m_type)->GetCanCounterBombard()) { 
      return CanBombard(defender); 
   } else { 
      return FALSE; 
   }
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::CanActivelyDefend
//
// Description: Check how many times this unit can actively defend against a
//              passing army.
//
// Parameters : attacker	: the army (list of units) to defend against
//
// Globals    : g_theUnitDB	: unit properties database
//
// Returns    : sint32		: number of active defense strikes that can be
//                            executed by this unit.
//
// Remark(s)  : The movement types of attacker and defender have to match to 
//              be able to actively defend.
//
//----------------------------------------------------------------------------
sint32 UnitData::CanActivelyDefend(CellUnitList &attacker) const
{
	const UnitRecord *rec = g_theUnitDB->Get(m_type);
	if(rec->GetActiveDefenseRange() <= 0)
		return 0;

	bool const	movesMatch	=
		(attacker.IsAtLeastOneMoveLand()     && rec->GetDefendLand())	||
		(attacker.IsAtLeastOneMoveWater()    && rec->GetDefendWater())	||
		(attacker.IsAtLeastOneMoveAir()      && rec->GetDefendAir())	||
		(attacker.IsAtLeastOneMoveSpace()    && rec->GetDefendSpace())	||
		(attacker.IsAtLeastOneMoveMountain() && rec->GetDefendMountain());

	if(movesMatch) {
		if(rec->GetActiveDefenseOnlyWhenCarryingEnablers()) {
			sint32 numEnablers = 0;
			sint32 i;
			for(i = 0; i < m_cargo_list->Num(); i++) {
				if(m_cargo_list->Access(i).GetDBRec()->GetEnableCarrierDefenses())
					numEnablers++;
			}
			return numEnablers;
		}
		return 1;
	}
	return 0;
}

double UnitData::GetAttack(const UnitRecord *rec, const Unit defender) const
{
	UnitRecord const *	defrec		= defender.GetDBRec();

	if(!(rec->GetCanAttack() & defrec->GetMovementType())) 
	{
		return 0;
	}

	double const		baseattack	= rec->GetAttack();
	double				attack		= baseattack;

	if (defrec->GetWoodenShip()) 
    {
		attack += baseattack * rec->GetAttackWoodenShipBonus();
	}

	MapPoint	dpos;
	defender.GetPos(dpos);
    double		cityAttack;
	if ((g_theWorld->GetCell(dpos)->GetCity().m_id != (0)) &&
        rec->GetAttackCityBonus(cityAttack)
       ) 
    {
		attack += baseattack * cityAttack;
	}

    double submarineAttack;
	if (defrec->GetIsSubmarine() &&
        rec->GetAttackBonusSubmarine(submarineAttack)
       ) 
    {
		attack += baseattack * submarineAttack;
	}

	// EMOD
	double	bonus;
	if (defrec->GetIsFoot() && rec->GetFootBonus(bonus))
	{
		attack += baseattack * bonus;
	}

	if (defrec->GetIsMounted() && rec->GetMountedBonus(bonus)) 
	{
		attack += baseattack * bonus;
	}

	if (defrec->GetIsSiege() && rec->GetSiegeBonus(bonus)) 
	{
		attack += baseattack * bonus;
	}

	if (defrec->GetIsWheeled() && rec->GetWheeledBonus(bonus)) 
	{
		attack += baseattack * bonus;
	}

	if (defrec->GetIsMechanized() && rec->GetMechanizedBonus(bonus)) 
	{
		attack += baseattack * bonus;
	}

    if (defrec->GetIsSpecialForces() && rec->GetSpecialForcesBonus(bonus)) 
    {
		attack += baseattack * bonus;
	}

    if (defrec->GetIsHelicopter() && rec->GetHelicopterBonus(bonus)) 
    {
		attack += baseattack * bonus;
	}

	if (defrec->GetCivilian() && rec->GetCivilianBonus(bonus)) 
	{
		attack += baseattack * bonus;
	}

	if (defrec->GetIsGuerrilla() && rec->GetGuerrillaBonus(bonus)) 
	{
		attack += baseattack * bonus;
	}
	
	//end EMOD

	return attack;
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::Bombard
//
// Description: Do bombard damage to (i.e., subtract hit points from) Unit defender.
//
// Parameters : UnitRecord *rec           : this unit's UnitRecord data
//            : Unit defender	          : the defending unit
//            : BOOL isCounterBombardment : toggles Slic Bombardment/CounterBombardmentTriggers
//
// Globals    : g_theUnitDB	  : unit properties database
//            : g_slicEngine  : 
//            : g_theConstDB  : 
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void UnitData::Bombard(const UnitRecord *rec, Unit defender,
					   BOOL isCounterBombardment)
{
	sint32 f = (sint32)(rec->GetFirepower() /  
		g_theUnitDB->Get(defender.GetType())->GetArmor()); 
	sint32 n;
	bool canBombard = rec->GetBombRounds(n);			
	Assert(canBombard);
	if(!canBombard)
		n = 0;

	double hp = defender.GetHP(); 

	g_slicEngine->RunCounterBombardmentTriggers(defender, Unit(m_id));

	double defenseStrength = defender.GetDefense(Unit(m_id));
	double attack = rec->GetZBRangeAttack();

	double prob = attack / (attack + defenseStrength);
	sint32 p = sint32(prob * 100);

	double dmr = 1.0/defender.GetHPModifier(); 
	if (IsVeteran()) 
		p += sint32(double(p) * g_theConstDB->GetVetCoef()); 
	
	for (sint32 i = 0; i < n; ++i) 
    { 
		if (g_rand->Next(100) < p) 
        { 
			hp -= f * dmr; 
		} 
	}

	if (rec->GetNonLethalBombard() && (hp < 0.999))
	{
		hp = 1.0;	// Damage, but do not kill
	}

	defender.SetHP(hp); 
	if(isCounterBombardment) {
		g_slicEngine->RunCounterBombardmentTriggers(Unit(m_id), defender);
	} else {
		g_slicEngine->RunBombardmentTriggers(Unit(m_id), defender);
	}
}

// not used 
void UnitData::BombardOneRound(const UnitRecord *rec, Unit &defender, 
                               double dbonus, 
                               double dmr)
{
    sint32 p = 0;
    (void) rec->GetProbOfBombHit(p);

    sint32 f = rec->GetZBRangeAttack(); 
    double hp = defender.GetHP(); 

    if (IsVeteran()) 
    {
        p += sint32(double(p) * g_theConstDB->GetVetCoef()); 
    }


    p  = int (p *(1.0 + dbonus)); 

    if (g_rand->Next(100) < p) { 
         hp -= f * dmr; 
    } 
    defender.SetHP(hp); 
}

// returns TRUE if this unit can bombard defender-type units
BOOL UnitData::CanBombardType(const Unit & defender) const
{
	const UnitRecord *rec = g_theUnitDB->Get(m_type); 

	if (rec->GetCanBombardLand() || rec->GetDefendLand()) {
		if (defender.GetMovementTypeLand()) {
			return TRUE;
		}
	}
	
	if (rec->GetCanBombardWater() || rec->GetDefendWater()) {
		if (defender.GetMovementTypeSea() || defender.GetMovementTypeShallowWater()) {
			return TRUE;
		}
	}
	
	if (rec->GetCanBombardAir() || rec->GetDefendAir()) {
		if (defender.GetMovementTypeAir()) {
			return TRUE; 
		}
	}
	
	if (rec->GetCanBombardSpace() || rec->GetDefendSpace()) {
		if (defender.GetMovementTypeSpace()){
			return TRUE;
		}
	}
	
	if (rec->GetCanBombardMountain() || rec->GetDefendMountain()) {
		if (defender.GetMovementTypeMountain()) {
			return TRUE;
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::Bombard
//
// Description: Try to bombard a random member of CellUnitList &defender
//
// Parameters : CellUnitList &defender	  : the defending army (list of units)
//            : BOOL isCounterBombardment : toggles Slic Bombardment/CounterBombardmentTriggers
//                                          later (when doing damage to defender)
//
// Globals    : g_theUnitDB	: unit properties database
//            : g_theWorld  : the map
//
// Returns    : BOOL		: TRUE if successful
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
BOOL UnitData::Bombard(CellUnitList &defender, BOOL isCounterBombardment) 
{ 
	const UnitRecord *rec = g_theUnitDB->Get(m_type); 
	sint32 i, j, r; 

	Assert(0 < defender.Num()); 
	r = g_rand->Next(defender.Num()); 

	for (i=r, j=0; 
		 j < defender.Num(); 
		 i = ((i + 1) % defender.Num()), j++) { 

		if(!(defender[i].GetVisibility() & (1 << m_owner)) && !g_theWorld->GetCity(defender[i].RetPos()).IsValid())
			continue;

// EMOD - add something like bombarding reduces city defenses here?


		if (CanBombardType(defender[i]) || rec->GetSingleUse())
		{
			Bombard(rec, defender[i], isCounterBombardment);
			return TRUE;
		}
	}
	return FALSE;
}

//not used
void UnitData::FightOneRound(Unit did, double defenders_bonus, 
                             double amr, double dmr)
{	
	double d = did.GetDefense(Unit(this->m_id)); 
	double a = GetAttack(g_theUnitDB->Get(m_type), did);
	
	if (IsVeteran()) { 
		a += a * g_theConstDB->GetVetCoef(); 
	} 
	
	Assert(0.00001f < a+d); 
	
	sint32 p = sint32(double (0x0fff) * a/(a+d));
	
	if ((g_rand->Next() & 0x0fff) < p) { 
		did.DeductHP(amr * g_theUnitDB->Get(m_type)->GetFirepower()); 
	} else {
		DeductHP(dmr * did.GetFirepower()); 
	}
}

void UnitData::DeductHP(double fp)
{
	if(g_theUnitDB->Get(m_type)->GetIsSpecialForces() || Flag(k_UDF_IS_PROFESSIONAL)) {
		m_hp -= fp;
	} else {
		m_hp -= fp * (1 / g_player[m_owner]->m_readiness->GetHPModifier()); //military readiness 
	}


	return;
}

//----------------------------------------------------------------------------
//
// Name       : UDUnitTypeCanSettle
//
// Description: Checks unit properties to see if it can settle a city on 
//              a tile
//
// Parameters : sint32 unit_type       : Variable for the type of unit 
//              const MapPoint &pos    : Variable for tile on map
//
// Globals    : g_theWorld             : The game world properties
//              g_theUnitDB            : Unit properties 
//
// Returns    : BOOL                   : Returns TRUE if an unit-type can 
//                                       settle on a tile.
//                                       Returns FALSE if the unit cannot 
//                                       settle a city there.
//
// Remark(s)  : A new unit attribute CanSettleOn was added by E.
//              Modders will define this in Unit.txt as CanSettleOn: X.  
//              The flag adds an additional option in order to restrict 
//              where cities can be built. 
//
//----------------------------------------------------------------------------
BOOL UDUnitTypeCanSettle(sint32 unit_type, const MapPoint &pos) 
{
	sint32 i;
	const UnitRecord *rec = g_theUnitDB->Get(unit_type);
	sint32 t = rec->GetSettleCityTypeIndex();
	if (t < 0) {
		return FALSE;
	}
	if (g_theUnitDB->Get(t)->GetHasPopAndCanBuild() == FALSE) {
		return FALSE;
	}
	if (g_theWorld->HasCity(pos)) 
		return FALSE;

	if (rec->GetNumCanSettleOn() > 0){
		for(i = 0; i < rec->GetNumCanSettleOn(); i++) {
			if(rec->GetCanSettleOnIndex(i) == g_theWorld->GetCell(pos)->GetTerrain()) {
				return TRUE;
			}
		}
		return FALSE;
	}

	if (rec->GetSettleLand() && g_theWorld->IsLand(pos))
		return TRUE; 
	else if (rec->GetSettleMountain() && g_theWorld->IsMountain(pos))
		return TRUE; 
	else if (rec->GetSettleWater() && g_theWorld->IsWater(pos))
		return TRUE; 
	else if (rec->GetSettleSpace() && g_theWorld->IsSpace(pos))
		return TRUE;
	
	return FALSE;
}



BOOL UnitData::CanSettle(const MapPoint &pos) const 
{
	return  UDUnitTypeCanSettle(m_type, pos);
}


void BringCityIntoAge(sint32 age, Unit c) 
{
	
	BOOL ThisFunctionUsesHardwiredAgesAndDoesntUseEvents = FALSE;
	Assert(ThisFunctionUsesHardwiredAgesAndDoesntUseEvents);

    sint32 owner = c.GetOwner(); 
    Gold gold; 
    MapPoint pos; 
    c.GetPos(pos);
    CityData *cd = c.GetData()->GetCityData(); 

    switch (age) { 
    case 5:
        if (545 < g_turn->GetRound()) return; 
        break; 
    case 4:
        if (470 < g_turn->GetRound()) return; 
        break; 
    case 3:
        if (400 < g_turn->GetRound()) return; 
        break;
    case 2:
        if (270 < g_turn->GetRound()) return; 
        break; 
    case 1:
        if (145 < g_turn->GetRound()) return; 
        break; 
    default:
        break; 
    } 



    switch (age) {
    case 5:
      c.BuildImprovement(33); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(34); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(35); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(36); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(37); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(38); 
      cd->CheatBuildFirstItem();
      
     
      c.BuildImprovement(40); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(41); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(42); 
      cd->CheatBuildFirstItem();
      g_player[owner]->m_materialPool->AddMaterials(8000);
	  gold.SetLevel(1000) ;
	  g_player[owner]->BequeathGold(gold) ;
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);

    case 4: 
      c.BuildImprovement(25); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(26); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(27); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(28); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(29); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(30); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(31); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(32); 
      cd->CheatBuildFirstItem();
      g_player[owner]->m_materialPool->AddMaterials(5000);
	  gold.SetLevel(3000) ;
	  g_player[owner]->BequeathGold(gold) ;
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);

    case 3:

      c.BuildImprovement(17); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(18); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(19); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(20); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(21); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(22); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(23); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(24); 
      cd->CheatBuildFirstItem();
      g_player[owner]->m_materialPool->AddMaterials(4000);
	  gold.SetLevel(1000) ;
	  g_player[owner]->BequeathGold(gold) ;
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);

    case 2:

      c.BuildImprovement(8); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(9); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(10); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(11); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(12); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(13); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(14); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(15); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(16); 
      cd->CheatBuildFirstItem();
      g_player[owner]->m_materialPool->AddMaterials(2000);
	  gold.SetLevel(1000) ;
	  g_player[owner]->BequeathGold(gold) ;
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);

    case 1:

      c.BuildImprovement(0); 
      cd->CheatBuildFirstItem();
    
      
      c.BuildImprovement(2); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(3); 
      cd->CheatBuildFirstItem();

	  c.BuildImprovement(4); 
	  cd->CheatBuildFirstItem();

      c.BuildImprovement(5); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(6); 
      cd->CheatBuildFirstItem();
      c.BuildImprovement(7); 
      cd->CheatBuildFirstItem();
      g_player[owner]->m_materialPool->AddMaterials(2000);
	  gold.SetLevel(1000) ;
	  g_player[owner]->BequeathGold(gold);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);
      c.MakeCitizen(0, pos);

    }
}

BOOL UnitData::Settle()
{
	DPRINTF(k_DBG_GAMESTATE, ("Unit %lx settling\n", m_id));

    if (CanSettle(m_pos) == FALSE) {
		DPRINTF(k_DBG_GAMESTATE, ("CanSettle false!\n"));
        return FALSE; 
	}


    Unit    nearbyCity  = g_theWorld->GetCell(m_pos)->GetCityOwner();

   
	if (nearbyCity.IsValid()) 
    {
		SlicObject *so = new SlicObject("29IASettlingTooClose") ;
		so->AddRecipient(m_owner);
		so->AddCity(nearbyCity);
		g_slicEngine->Execute(so);
			DPRINTF(k_DBG_GAMESTATE, ("Tile already owned!\n"));
		return FALSE;
    }

	if(m_movement_points < g_theConstDB->SpecialActionMoveCost() &&
	   !Flag(k_UDF_FIRST_MOVE)) {
		DPRINTF(k_DBG_GAMESTATE, ("No movement (%lf < %lf), FirstMove: %d\n", m_movement_points, g_theConstDB->SpecialActionMoveCost(),
								  Flag(k_UDF_FIRST_MOVE)));

		if(!g_theProfileDB->AllowAISettleMoveCheat()) {
			g_slicEngine->RunCantSettleMovementTriggers(Unit(m_id));
			return FALSE;
		}

		if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT) {
			g_slicEngine->RunCantSettleMovementTriggers(Unit(m_id));
			return FALSE;
		} else if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
			g_slicEngine->RunCantSettleMovementTriggers(Unit(m_id));
			return FALSE;
        } else { 
            return FALSE; 
        } 
	}




  	const UnitRecord *rec = g_theUnitDB->Get(m_type);   
    sint32 t = rec->GetSettleCityTypeIndex();


	if((g_theWorld->IsWater(m_pos) || g_theWorld->IsShallowWater(m_pos)) && 
	   !g_theUnitDB->Get(t)->GetMovementTypeSea()) {
		DPRINTF(k_DBG_GAMESTATE, ("Wrong terrain type\n"));
		return FALSE;
	}

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
						   GEV_KillUnit,
						   GEA_Unit, m_id,
						   GEA_Int, CAUSE_REMOVE_ARMY_SETTLE,
						   GEA_Player, -1,
						   GEA_End);

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
						   GEV_CreateCity,
						   GEA_Player, m_owner,
						   GEA_MapPoint, m_pos,
						   GEA_Int, CAUSE_NEW_CITY_SETTLE,
						   GEA_Int, m_type,
						   GEA_End);

	return TRUE; 
}




//if this city is not supposed to begin it's turn, kill it
//see the callback CityBeginTurnEvent
void UnitData::BeginTurnCity(const Unit &me, UnitDynamicArray &dead)

{
    Assert(m_city_data); 

    if (m_city_data->BeginTurn() == FALSE) {
        dead.Insert(me); 
    }
}

//set this unit visible to PLAYER_INDEX player 
void UnitData::SetVisible(PLAYER_INDEX player) 
{ 
	m_visibility |= (1 << player); 
	m_temp_visibility |= (1 << player);
	m_ever_visible |= (1 << player);
	
	if (m_actor) {
		g_director->AddSetVisibility(m_actor, GetVisibility());
	}
}

//set this unit invisible to PLAYER_INDEX player 
void UnitData::UnsetVisible(PLAYER_INDEX player) 
{
	m_visibility &= ~(1 << player);

	if (m_actor) {
		g_director->AddSetVisibility(m_actor, GetVisibility());
	}
}

void UnitData::BeginTurnVision(PLAYER_INDEX player)
{
	if(m_visibility & (1 << player)) {
		m_temp_visibility |= (1 << player);
	} else {
		m_temp_visibility &= ~(1 << player);
		m_temp_visibility_array.NextTurn(player);
		m_temp_visibility |= m_temp_visibility_array.GetCurrentVisibility(player);
		if(g_player[player]->m_hasGlobalRadar && g_theUnitDB->Get(m_type)->GetVisionClassStandard()) {
			m_temp_visibility |= (1 << player);
			m_visibility |= (1 << player);
		}
	}

	
	
	if (m_actor) {
		g_director->AddSetVisibility(m_actor, GetVisibility());
	}


	
	
	m_radar_visibility &= ~(1 << player);
	
}

void UnitData::ResetCityOwner(const Unit &me, const PLAYER_INDEX newo, 
                              sint32 is_conquest, const CAUSE_REMOVE_CITY cause)
{
	DPRINTF(k_DBG_GAMESTATE, ("ResetCityOwner: %lx, new: %d, old: %d, conq: %d, cause: %d\n",
							  me.m_id, newo, g_theUnitPool->IsValid(me) ? me.GetOwner() : -1, is_conquest, cause));
	PLAYER_INDEX	player ;

	CityWindow::NotifyCityCaptured(me);

	ID	item ;

	SELECT_TYPE	state ;

	Unit u = me;
	g_theTradeOfferPool->RemoveTradeOffersFromCity(u);
	g_theTradeBids->CancelBidsWithCity(u);

    Assert (0 <= newo); 
    Assert (newo < k_MAX_PLAYERS); 
	Assert(m_city_data);
	
	g_selected_item->GetTopCurItem(player, item, state) ;
	if (item == me)
		g_selected_item->Deselect(m_owner) ;							

	if(!u.IsNoZoc()) {
		
		g_theWorld->RemoveZOC(m_pos, m_owner);
		g_theWorld->AddOtherArmyZOC(m_pos, m_owner, Army(), me);
		sint32 dd;
		for(dd = 0; dd < (sint32)NOWHERE; dd++) {
			MapPoint npos;
			if(m_pos.GetNeighborPosition((WORLD_DIRECTION)dd, npos)) {
				g_theWorld->RemoveZOC(npos, m_owner);
				g_theWorld->AddOtherArmyZOC(npos, m_owner, Army(), me);
				g_theWorld->AddZOC(npos, newo);
			}
		}
	}
   
	sint32 killedBy = newo;

	g_player[m_owner]->RemoveCityReferenceFromPlayer(Unit(m_id), cause, killedBy); 

   CAUSE_NEW_CITY nc_cause; 
   switch (cause) { 
   case CAUSE_REMOVE_CITY_HAPPINESS_REVOLT: nc_cause = CAUSE_NEW_CITY_REVOLT; break; 
   case CAUSE_REMOVE_CITY_ATTACK: nc_cause = CAUSE_NEW_CITY_CONQUEST; break; 
   case CAUSE_REMOVE_CITY_DIPLOMACY: nc_cause = CAUSE_NEW_CITY_DIPLOMACY; break;
   case CAUSE_REMOVE_CITY_SLAVE_UPRISING: nc_cause = CAUSE_NEW_CITY_SLAVE_UPRISING; break;
   case CAUSE_REMOVE_CITY_AI_ENTITY_REVOLT: nc_cause = CAUSE_NEW_CITY_REVOLT; break;
   default: 
       Assert(0); 
       nc_cause = CAUSE_NEW_CITY_UNKNOWN; 
   } 

   
   
   
   
   m_city_data->DestroyCapitol();
 
#if 0
	double oldVisionRange = (g_theUnitDB->Get(m_type)->m_vision_range);
   m_vision_range = g_theUnitDB->Get(m_type)->m_vision_range;
   
   
   
   if (g_player[newo]->GetPlayerType() == PLAYER_TYPE_ROBOT &&
	   (!g_network.IsClient() || !g_network.IsLocalPlayer(newo)))
   {
	   
	   
	   Difficulty *diff = g_player[newo]->m_difficulty;
	   m_vision_range += diff->GetVisionBonus();
	   
   } 
#endif

   
    
   if (is_conquest) { 
       m_city_data->ResetConquestDistress(g_player[newo]->GetConquestDistress()); 
   } 

   m_visibility |= (1 << newo);
   m_temp_visibility |= (1 << newo);
   m_ever_visible |= m_visibility;

   


   g_director->AddSetOwner(m_actor, newo);
   g_director->AddSetVisibility(m_actor, m_visibility);

   








   uint64 wonders = m_city_data->GetBuiltWonders();
   sint32 w;
   for(w = 0; w < 64; w++) {
	   if(wonders & ((uint64)1 << w)) {
			Unit me(m_id);
		   g_player[m_owner]->RemoveWonder(w, FALSE);
		   g_player[newo]->AddWonder(w, me);
	   }
   }

   m_city_data->ResetCityOwner(newo);

   Assert(CAUSE_NEW_CITY_SETTLE != nc_cause);
   g_player[newo]->AddCityReferenceToPlayer(me, nc_cause);

   RemoveUnitVision(); // Now remove unit vision, the old owner knows what happend.
   m_owner = newo; // Now change owner
   bool revealedUnexplored;
   AddUnitVision(revealedUnexplored);

   UnitDynamicArray revealed_units;
   DoVision(revealed_units);


#if 0
	
	

	
	
   sint32 i;
   Cell *cell = g_theWorld->GetCell(m_pos);
   for(i = 0; i < cell->GetNumUnits(); i++) {
	   if(cell->AccessUnit(i).GetOwner() != m_owner) {
		   g_gevManager->Pause();
		   g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillUnit,
								  GEA_Unit, cell->AccessUnit(i),
								  GEA_Int, CAUSE_REMOVE_ARMY_DIPLOMACY,
								  GEA_Player, m_owner,
								  GEA_End);
		   g_gevManager->Resume();
	   }
   }
#endif
}










void UnitData::ResetUnitOwner(const Unit &me, const PLAYER_INDEX new_owner,
                              CAUSE_REMOVE_ARMY rem_cause)
{
	
	if(m_cargo_list) {
		sint32 i;
		for(i = m_cargo_list->Num() - 1; i >= 0; i--) {
			if(g_theUnitPool->IsValid(m_cargo_list->Access(i))) {
				m_cargo_list->Access(i).Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
			}
		}
	}

	g_player[m_owner]->RemoveUnitReference(Unit(m_id), rem_cause, -1) ;

	if(m_army.IsValid()) {
		m_army.SetRemoveCause(rem_cause);
		m_army.Del(me);
	}
	m_army.m_id = 0; 

	RemoveUnitVision();
	

    CAUSE_NEW_ARMY new_cause; 
    switch (rem_cause) { 
    case CAUSE_REMOVE_ARMY_SLAVE_UPRISING:
    case CAUSE_REMOVE_ARMY_UPRISING: 
        new_cause = CAUSE_NEW_ARMY_UPRISING; 
        break; 
    case CAUSE_REMOVE_ARMY_RUSTLE: 
        new_cause = CAUSE_NEW_ARMY_RUSTLE; 
        break; 
    case CAUSE_REMOVE_ARMY_DIPLOMACY:
        new_cause = CAUSE_NEW_ARMY_DIPLOMACY;
        break; 
    default:
        Assert(FALSE); 
        new_cause = CAUSE_NEW_ARMY_UNKNOWN; 
    } 

	m_owner = new_owner;
#if 0
	m_vision_range = g_theUnitDB->Get(m_type)->m_vision_range;

	
	
	if (g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT &&
		(!g_network.IsClient() || !g_network.IsLocalPlayer(m_owner)))
	{
		
		
	    Difficulty *diff = g_player[m_owner]->m_difficulty;
		m_vision_range += diff->GetVisionBonus();

	} 
#endif

	g_player[new_owner]->InsertUnitReference(me, new_cause, Unit()) ;

	bool revealedUnexplored;
	AddUnitVision(revealedUnexplored);
	
	m_visibility |= (1 << new_owner);
	m_temp_visibility |= (1 << new_owner);
	m_ever_visible |= m_visibility;
	if(m_actor) {
		g_director->AddSetOwner(m_actor, new_owner);
		g_director->AddSetVisibility(m_actor, m_visibility);
		g_director->AddSetVisionRange(m_actor, (GetVisionRange()));

	}

	UnitDynamicArray revealed_units;
	DoVision(revealed_units);

    ENQUEUE() ;
}

BOOL UnitData::BuildUnit(sint32 type)
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->BuildUnit(type);
}

BOOL UnitData::BuildImprovement(sint32 type)
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->BuildImprovement(type);
}

BOOL UnitData::BuildWonder(sint32 wonder)
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->BuildWonder(wonder);
}

void UnitData::AddWonder(sint32 type)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->AddWonder(type);
}

BOOL UnitData::ChangeCurrentlyBuildingItem(sint32 category, sint32 item_type)

{
    Assert(m_city_data);
    if(!m_city_data) return FALSE;

    return m_city_data->ChangeCurrentlyBuildingItem(category, item_type);
}


BOOL UnitData::HaveImprovement( const sint32 type) const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->HaveImprovement(type); 
}

uint64 UnitData::GetImprovements() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return (m_city_data->GetImprovements());
}


void UnitData::DestroyCapitol()

{
    Assert(m_city_data); 
	if(!m_city_data)
		return;

    m_city_data->DestroyCapitol(); 
}

void UnitData::DestroyImprovement(sint32 type)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	m_city_data->DestroyImprovement(type);
}


void UnitData::NewGovernment(sint32 type)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	m_city_data->NewGovernment(type);
}


sint32 UnitData::ImprovementCanRefuel(const Unit &u) const

{ 
    Assert(m_city_data);
    Assert(u);
    return m_city_data->ImprovementCanRefuel(u);
}


double UnitData::GetDefendersBonus() const

{
    Assert(m_city_data); 

    return m_city_data->GetDefendersBonus(); 
}







#ifdef _DEBUG

char const * UnitData::GetText() const

{ 
    return m_text; 
} 

void UnitData::SetText(char *str) 
{ 
    strcpy(m_text, str); 
} 


void UnitData::GamestateDebug()

{
    strcpy(m_text, g_theStringDB->GetIdStr(g_theUnitDB->Get(m_type)->m_name));
}

#endif

void UnitData::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

	if (archive.IsStoring())
	{
		
		archive<<m_id;
		archive.PutSINT32(m_owner);
		archive<<m_fuel;
		archive<<m_hp;
		archive<<m_movement_points;
		
		archive<<m_type; 
		archive<<m_visibility;
		archive<<m_temp_visibility;
		archive<<m_radar_visibility;
		archive<<m_ever_visible;
		
		
		archive << m_flags;

		m_army.Serialize(archive);
		m_pos.Serialize(archive);
		
		
		if (m_cargo_list)
		{
			archive<<(uint8)(TRUE);								
			m_cargo_list->Serialize(archive);						
		}
		else
			archive<<(uint8)(FALSE);								

		
		if (m_city_data)
		{
			archive<<(uint8)(TRUE);								
			m_city_data->Serialize(archive);						
		}
		else
			archive<<(uint8)(FALSE);								

		m_actor->Serialize(archive);

		m_temp_visibility_array.Serialize(archive);
		m_transport.Serialize(archive);

		m_roundTheWorldMask->Serialize(archive);

		m_target_city.Serialize(archive);

		archive << (uint32)(m_lesser != NULL);

		if (m_lesser)
			((UnitData *)(m_lesser))->Serialize(archive);

		archive << (uint32)(m_greater != NULL);

		if (m_greater)
			((UnitData *)(m_greater))->Serialize(archive);

	}
	else
	{
		uint8	tmp;

		archive>>m_id;
		m_owner = (PLAYER_INDEX)archive.GetSINT32();
		archive>>m_fuel;
		archive>>m_hp;
		archive>>m_movement_points;
		
		archive>>m_type;
		archive>>m_visibility;
		archive>>m_temp_visibility;
		archive>>m_radar_visibility;
		archive>>m_ever_visible;
		
		
		archive >> m_flags;
		
		m_army.Serialize(archive);
		m_pos.Serialize(archive);
		
#ifdef _DEBUG
		m_text[0] = 0;
#endif
		
		if (m_cargo_list)
			delete m_cargo_list;

		archive>>tmp;
		if (tmp)
		{
			m_cargo_list = new UnitDynamicArray();
			m_cargo_list->Serialize(archive);
		}
		else
			m_cargo_list=NULL;

		
		if (m_city_data)
			delete m_city_data;

		archive>>tmp ;
		if (tmp)
			m_city_data = new CityData(archive);
		else
			m_city_data = NULL ;

		
		if (m_actor)
		{
			delete m_actor;
			m_actor = NULL;
		}

		m_actor = new UnitActor(archive);

		m_sprite_state = m_actor->GetSpriteState();

		m_temp_visibility_array.Serialize(archive);
		m_transport.Serialize(archive);

		m_roundTheWorldMask = new BitMask(archive);

		m_target_city.Serialize(archive);

		uint32 hasOld;

		
		if (m_lesser)
		{
			delete m_lesser;
			m_lesser = NULL;
		}

		
		if (m_greater)
		{
			delete m_greater;
			m_greater = NULL;
		}

		archive >> hasOld;
		if (hasOld) {
			m_lesser = new UnitData(archive);
		} else {
			m_lesser = NULL;
		}

		archive >> hasOld;
		if (hasOld) {
			m_greater = new UnitData(archive);
		} else {
			m_greater = NULL;
		}

	}
}


uint32 UnitData_UnitData_GetVersion(void)
	{
	return (k_UNITDATA_VERSION_MAJOR<<16 | k_UNITDATA_VERSION_MINOR) ;
	}

void UnitData::GetConsumptionStats(sint32 &foodConsumed) const 
{
	Assert(m_city_data) ;
	m_city_data->GetConsumptionStats(foodConsumed) ;
}
//----------------------------------------------------------------------------
//
// Name       : UnitData::GetPop
//
// Description: Get this city's population
//
// Parameters : sint32 &p   : the filled in number
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void UnitData::GetPop(sint32 &p)const
{
    Assert(m_city_data);
    m_city_data->GetPop(p); 
}
//----------------------------------------------------------------------------
//
// Name       : UnitData::GetTurnsToNextPop
//
// Description: Get the number of turns until this city adds a pop
//
// Parameters : sint32 &p   : the filled in number
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : The city's m_turnsNextPop is not archived.
//
//----------------------------------------------------------------------------
void UnitData::GetTurnsToNextPop(sint32 &p)const//PFT 29 mar 05

{
	Assert(m_city_data);
	p = m_city_data->TurnsToNextPop();
}

void UnitData::AddTradeRoute(TradeRoute &route, BOOL fromNetwork)
{
	Assert(m_city_data);
	Assert(route.m_id != 0);
	m_city_data->AddTradeRoute(route, fromNetwork);
}

void UnitData::DelTradeRoute(TradeRoute route)
{
	Assert(m_city_data);
	Assert(route.m_id != 0);
	m_city_data->DelTradeRoute(route);
}

BOOL UnitData::CanInterceptTrade() const
{
	Cell* cell = g_theWorld->GetCell(m_pos);
	sint32 i;

	
	if(!cell->GetNumTradeRoutes())
		return false;

	
	if (!g_theUnitDB->Get(GetType())->GetCanPirate())
		return false;

	
	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
			return false;

	
	for(i = cell->GetNumTradeRoutes() - 1; i >= 0; i--) {
		TradeRoute route = cell->GetTradeRoute(i);
		PLAYER_INDEX source_owner = route.GetOwner();
//		PLAYER_INDEX dest_owner = route.GetDestination().GetOwner();
		if ( source_owner == m_owner )
			continue;

		if(g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT &&
		   !(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))) {
		   
			if(AgreementMatrix::s_agreements.HasAgreement(
				m_owner, 
				source_owner,  
				PROPOSAL_OFFER_STOP_PIRACY))
				continue;
		}

		
		return true;
	}
	
	return false;
}


ORDER_RESULT UnitData::InterceptTrade()
{
	Cell* cell = g_theWorld->GetCell(m_pos);
	sint32 i;
	sint32 numPirated = 0;

	Assert(cell);
	if(!cell) return ORDER_RESULT_ILLEGAL;

	
	
	for(i = cell->GetNumTradeRoutes() - 1; i >= 0; i--) 
	{
		TradeRoute route = cell->GetTradeRoute(i);
		if(!g_theTradePool->IsValid(route)) {
			if(g_network.IsClient()) {
				g_network.RequestResync(RESYNC_BAD_TRADE_ROUTE);
				return ORDER_RESULT_ILLEGAL;
			}
			continue;
		}

		PLAYER_INDEX source_owner = route.GetOwner();
//		PLAYER_INDEX dest_owner = route.GetDestination().GetOwner();
		
		if ( source_owner == m_owner )
			continue;
		
		g_gevManager->AddEvent
            (GEV_INSERT_AfterCurrent,   GEV_SetPiratingArmy,
			 GEA_TradeRoute,            route,
			 GEA_Army,                  m_army,
			 GEA_End
            );

		numPirated++;


#ifdef CTP1_TRADE
		
		
		sint32 index; 
		index = g_player[source_owner]->FindCityIndex(route.GetSource());
		index = g_player[dest_owner]->FindCityIndex(route.GetDestination());
		ROUTE_TYPE type; 
		sint32 resource;             
		route.GetSourceResource(type, resource);
		



		
		BOOL sourceOwnerCaught = FALSE;
		BOOL destOwnerCaught = FALSE;
		
		if(g_rand->Next(100) < g_theConstDB->PiracyKillsTraderChance()) {
			g_player[route.GetPayingFor()]->KillATrader();
		} else {
			sourceOwnerCaught = TRUE;
			destOwnerCaught = TRUE;
		}
		Unit fromCity, toCity;
		fromCity = route.GetSource();
		toCity = route.GetDestination();

		if(!g_theUnitPool->IsValid(fromCity))
			fromCity.m_id = (0);

		if(!g_theUnitPool->IsValid(toCity))
			toCity.m_id = (0);

		if(!sourceOwnerCaught && fromCity.m_id != (0) &&
		   g_player[fromCity.GetOwner()] &&
		   g_player[fromCity.GetOwner()]->IsVisible(m_pos)) {
			sourceOwnerCaught = TRUE;
		}
		if(!destOwnerCaught && toCity.m_id != (0) &&
		   (fromCity.m_id == (0) || toCity.GetOwner() != fromCity.GetOwner()) &&
		   g_player[toCity.GetOwner()] &&
		   g_player[toCity.GetOwner()]->IsVisible(m_pos)) {
			destOwnerCaught = TRUE;
		}

		SlicObject *so;
		if(fromCity.m_id != (0)) {
			if(sourceOwnerCaught) {
				
				
				so = new SlicObject("045TradePirated");
				so->AddCivilisation(m_owner);
			} else {
				so = new SlicObject("045aTradePiratedUnknown");
			}

			so->AddRecipient(fromCity.GetOwner());
			so->AddCity(fromCity);
			so->AddCity(toCity);
			ROUTE_TYPE type;
			sint32 resource;
			route.GetSourceResource(type, resource);
			so->AddGood(resource);
			g_slicEngine->Execute(so);
		}
		if(toCity.m_id != (0)) {
			if(fromCity.m_id == (0) || toCity.GetOwner() != fromCity.GetOwner()) {
				if(destOwnerCaught) {
					
					
					so = new SlicObject("045TradePirated");
					so->AddCivilisation(m_owner);
				} else {
					so = new SlicObject("045aTradePiratedUnknown");
				}
				so->AddRecipient(toCity.GetOwner());
				so->AddCity(fromCity);
				so->AddCity(toCity);
				ROUTE_TYPE type;
				sint32 resource;
				route.GetSourceResource(type, resource);
				so->AddGood(resource);
				g_slicEngine->Execute(so);
			}
		}
		g_player[m_owner]->AddGold(g_theConstDB->GoldFromPiracy());
		
		Unit me(m_id);
		g_slicEngine->RunPiracyTriggers(route, me);
		
		route.Kill(CAUSE_KILL_TRADE_ROUTE_PIRATED);
		numPirated++;
#endif
	}
	if(numPirated > 0) {
		return ORDER_RESULT_SUCCEEDED;
	} else {
		return ORDER_RESULT_FAILED;
	}
}

void UnitData::DoVision(UnitDynamicArray &revealedUnits)
{
	DynamicArray<Unit> array;
	MapPoint topleft = m_pos;
	double maxVisionRange = unitutil_GetMaxVisionRange();

	
	
	
	topleft.x -= (sint16)maxVisionRange;

	
	g_theUnitTree->SearchRect(array, topleft, 
							  static_cast<sint16>(maxVisionRange) * 2 + 1,
							  static_cast<sint16>(maxVisionRange) * 2 + 1,
							  ~(1 << m_owner));
	sint32 i, n = array.Num();

	if(!IsCity()) {
		
		
		m_visibility = (1 << m_owner) | g_theWonderTracker->GlobeSatFlags();
		m_radar_visibility = 1 << m_owner;
		m_ever_visible |= m_visibility;
	}
	m_temp_visibility = m_visibility;

	
	sint32 maxrsq = (sint32)((maxVisionRange + 0.5) * (maxVisionRange + 0.5));
	sint32 myrsq = (sint32)(((GetVisionRange()) + 0.5) * ((GetVisionRange()) + 0.5));
	const UnitRecord *myrec = g_theUnitDB->Get(m_type);

	for(i = n-1; i >= 0; i--) {
		UnitData* him = array[i].AccessData();

		double hisr = him->GetVisionRange() + 0.5;
		sint32 hisrsq = (sint32)(hisr * hisr);
		sint32 ls = GetDistance(him, this, (sint32)maxVisionRange);

		
		if(ls > maxrsq) {
			continue;
		}
		
		BOOL runContactHim = FALSE;
		BOOL runContactMe = FALSE;
		
		if(!Flag(k_UDF_IS_CLOAKED) && 
		   ls <= hisrsq && !(m_visibility & (1 << him->m_owner))) {
			if(g_theUnitDB->Get(him->m_type)->GetCanSee() &
			   myrec->GetVisionClass()) {
				m_visibility |= 1 << him->m_owner;
				
				m_ever_visible |= m_visibility;
				m_temp_visibility |= 1 << him->m_owner;
				if(him->Flag(k_UDF_IS_ASLEEP)) {
					sint32 i;
					for(i = 0; i < him->m_army.Num(); i++) {
						him->m_army[i].WakeUp();
					}
				}

				runContactHim = TRUE;
			}
		}

		if(!him->Flag(k_UDF_IS_CLOAKED) &&
		   ls <= myrsq && !(him->m_visibility & (1 << m_owner))) {
			if(g_theUnitDB->Get(him->m_type)->GetVisionClass() &
			   myrec->GetCanSee()) {
				him->m_visibility |= 1 << m_owner;
				him->m_ever_visible |= him->m_visibility;
				
				if(!(him->m_temp_visibility & (1 << m_owner))) {
					revealedUnits.Insert(Unit(him->m_id));
					him->m_actor->SetPos(him->m_pos);
					him->m_temp_visibility |= 1 << m_owner;

					
					runContactMe = TRUE;
				}
				
				
				if (him->m_actor) {
					g_director->AddSetVisibility(him->m_actor, him->GetVisibility());
				}
			}
		}
		if(runContactMe || runContactHim) {
			g_slicEngine->RunContactTriggers(Unit(him->m_id), Unit(m_id));

		}

		if(runContactMe) {
			Assert(g_player[m_owner]);
			if(g_player[m_owner]) {
				g_player[m_owner]->ContactMade(him->m_owner);
				g_slicEngine->RunTrigger(TRIGGER_LIST_SIGHTED_UNIT,
										 ST_UNIT, him->m_id,
										 ST_UNIT, m_id,
										 ST_PLAYER, m_owner,
										 ST_PLAYER, him->m_owner,
										 ST_END);
			}
		}

		if(runContactHim) {
			Assert(g_player[him->m_owner]);
			if(g_player[him->m_owner]) {
				g_player[him->m_owner]->ContactMade(m_owner);
				g_slicEngine->RunTrigger(TRIGGER_LIST_SIGHTED_UNIT,
										 ST_UNIT, m_id,
										 ST_UNIT, him->m_id,
										 ST_PLAYER, him->m_owner,
										 ST_PLAYER, m_owner,
										 ST_END);
			}
		}
	}

	
	
	
	DynamicArray<Installation> instArray;

	maxVisionRange = MAX(terrainutil_GetMaxVisionRange(),(GetVisionRange()));
	maxrsq = (sint32)((maxVisionRange + 0.5) * (maxVisionRange + 0.5));
	topleft = m_pos;
	topleft.x -= sint16(maxVisionRange);

	g_theInstallationTree->SearchRect(
		instArray, topleft,
		static_cast<sint16>(maxVisionRange) * 2 + 1,
		static_cast<sint16>(maxVisionRange) * 2 + 1);
	                    
	n = instArray.Num();

	for(i = n-1; i >=0; i--) {
		Installation inst = instArray[i];
		sint32 ls = GetDistance(inst, this, (sint32)maxVisionRange);
		if(ls > maxrsq) {
			continue;
		}

		
		if(ls <= myrsq && !(inst.GetVisibility() & (1 << m_owner))) {
			if(myrec->GetCanSee() &
			   g_theTerrainImprovementDB->Get(inst.GetType())->GetCanSee()) {
				inst.SetVisible(m_owner);
			}
		}

		
		if(!(m_visibility & (1 << inst.GetOwner()))) {
			double hisr = terrainutil_GetVisionRange(inst.GetType(),inst.RetPos());
			if(hisr > 0) {
				sint32 hisrsq = sint32((hisr+0.5) * (hisr+0.5));
				if(ls <= hisrsq) {
					if(myrec->GetVisionClass() &
					   g_theTerrainImprovementDB->Get(inst.GetType())->GetCanSee()) {
						m_visibility |= (1 << inst.GetOwner());
						m_ever_visible |= m_visibility;
						m_temp_visibility |= (1 << inst.GetOwner());
					}
				}
			}
		}
	}

	
	
	if (m_actor) {
		g_director->AddSetVisibility(m_actor, GetVisibility());
	}
}

extern sint32 g_fog_toggle; 


extern sint32 g_god;

uint32 UnitData::GetVisibility() const 
{ 
    if (g_fog_toggle) { 
        return 0xffffffff; 
    } 
	
	if (g_god) return 0xFFFFFFFF;

    return m_visibility | m_temp_visibility; 
}
	
void UnitData::UndoVision()
{
	DynamicArray<Unit> enemyArray;
	DynamicArray<Unit> friendArray;
	double maxVisionRange = unitutil_GetMaxVisionRange();
	sint32 maxrsq = (sint32)((maxVisionRange+0.5) * (maxVisionRange + 0.5));

	MapPoint topleft = m_pos;
	topleft.x -= sint16((GetVisionRange()));

	
	g_theUnitTree->SearchRect(enemyArray, topleft,
							  static_cast<sint16>(GetVisionRange()) * 2 + 1,
							  static_cast<sint16>(GetVisionRange()) * 2 + 1,
							  ~(1 << m_owner));
	sint32 en = enemyArray.Num();
	sint32 i;
	for(i = 0; i < en; i++) {
		
		if(!(enemyArray[i].GetRealVisibility() & (1 << m_owner)))
			continue;

		
		
		
		if(enemyArray[i].IsCity()) {
			continue;
		}

		UnitData* hostile = enemyArray[i].AccessData();
		topleft = hostile->m_pos;
		topleft.x -= sint16(maxVisionRange);
		
		
		g_theUnitTree->SearchRect(friendArray, topleft,
								  static_cast<sint16>(maxVisionRange) * 2 + 1,
								  static_cast<sint16>(maxVisionRange) * 2 + 1,
								  1 << m_owner);
		sint32 fn = friendArray.Num();
		BOOL canBeSeen = FALSE;
		for(sint32 j = 0; j < fn; j++) {
			if(friendArray[j] == Unit(m_id))
				continue;
			UnitData* friendly = friendArray[j].AccessData();
			
			
			sint32 ls = GetDistance(friendly, hostile, (sint32)maxVisionRange);
			if(ls > maxrsq)
				continue;
			
			double fr = friendly->GetVisionRange() + 0.5;
			sint32 frsq = sint32(fr * fr);
			
			if(ls <= frsq) {
				if(g_theUnitDB->Get(hostile->m_type)->GetVisionClass() &
				   g_theUnitDB->Get(friendly->m_type)->GetCanSee()) {
					
					
					canBeSeen = TRUE;
					break;
				}
			}
		}
		if(!canBeSeen) {
			hostile->m_visibility &= ~(1 << m_owner);
		}
	}

	
	DynamicArray<Installation> instArray;

	maxVisionRange = MAX(terrainutil_GetMaxVisionRange(), (GetVisionRange()));
	maxrsq = (sint32)((maxVisionRange + 0.5) * (maxVisionRange + 0.5));
	topleft = m_pos;
	topleft.x -= sint16((GetVisionRange()));

	g_theInstallationTree->SearchRect(
		instArray, topleft,
		static_cast<sint16>(maxVisionRange) * 2 + 1,
		static_cast<sint16>(maxVisionRange) * 2 + 1,
		~(1 << m_owner));
	                    
	en = instArray.Num();
	for(i = 0; i < en; i++) {
		if(!instArray[i].GetVisibility() & (1 << m_owner))
			continue;

		Installation inst = instArray[i];
		inst.GetPos(topleft);
		topleft.x -= sint16(maxVisionRange);
		inst.CheckVision(m_owner);
	}
}

void UnitData::KillVision()
{
	if(!Flag(k_UDF_IS_IN_TRANSPORT) && !Flag(k_UDF_HAS_LEFT_MAP) && !IsTempSlaveUnit()) {
		RemoveUnitVision();
		UndoVision();
	}
}

// returns squared distance
sint32 UnitData::GetDistance(UnitData* unit1, UnitData* unit2,
							 sint32 wrapRange)
{
	MapPoint u2pos = unit2->m_pos;
	return GetDistance(unit1, u2pos, wrapRange);
}

sint32 UnitData::GetDistance(const Installation &inst, UnitData* unit2,
							 sint32 wrapRange)
{
	MapPoint iPos;

	inst.GetPos(iPos);
	return GetDistance(unit2, iPos, wrapRange);
}


sint32 UnitData::GetDistance(const UnitData* unit, const MapPoint &pos,
							 sint32 wrapRange)
{
	MapPoint uPos;

	unit->GetPos(uPos);
	return GetDistance(uPos, pos, wrapRange);
}

sint32 UnitData::GetDistance(const MapPoint &uPos, const MapPoint &pos,
							 sint32 wrapRange)
{
	return MapPoint::GetSquaredDistance(uPos, pos);
}

sint32 UnitData::CollectProduction(sint32 &mil, sint32 &mat)

{
	Assert(FALSE);
	return 0;
}

void UnitData::PayFederalProduction (double percent_military,
									 sint32 &mil_paid, 
									 double percent_terrain, 
									 sint32 &mat_paid)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->PayFederalProduction(percent_military, mil_paid, percent_terrain, 
        mat_paid); 

}

void UnitData::PayFederalProductionAbs (sint32 mil_paid, 
										double percent_mat, 
										sint32 &mat_paid)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->PayFederalProductionAbs (mil_paid, percent_mat, mat_paid);

}


void UnitData::SetMaterialContribution(BOOL on)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->SetMaterialContribution(on);
}

BOOL UnitData::GetMaterialContribution() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;
	return m_city_data->GetMaterialContribution();
}

BOOL UnitData::GetMilitaryContribution() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;
	return m_city_data->GetMilitaryContribution();
}

void UnitData::SetMilitaryContribution(BOOL on)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->SetMilitaryContribution(on);
}

    void SetMilitaryContribution(BOOL on);
    BOOL GetMilitaryContribution();


BOOL UnitData::IsEntrenched() const
{
	return Flag(k_UDF_IS_ENTRENCHED);
}

BOOL UnitData::IsEntrenching() const
{
	return Flag(k_UDF_IS_ENTRENCHING);
}

BOOL UnitData::CanEntrench() const
{
	return g_theUnitDB->Get(m_type)->GetCanEntrench();
}

void UnitData::Entrench()
{
	if(Flag(k_UDF_IS_ENTRENCHED))
		return;

	if(!CanEntrench())
		return;

	SetFlag(k_UDF_IS_ENTRENCHING);
	
		g_slicEngine->RunTrigger(TRIGGER_LIST_FORTIFY,
								 ST_UNIT, Unit(m_id),
								 ST_PLAYER, m_owner,
								 ST_END);
}

void UnitData::Detrench()
{
	ClearFlag(k_UDF_IS_ENTRENCHED);
	ClearFlag(k_UDF_IS_ENTRENCHING);
	
}

BOOL UnitData::IsAsleep() const
{
	return Flag(k_UDF_IS_ASLEEP);
}

void UnitData::Sleep()
{
	SetFlag(k_UDF_IS_ASLEEP);
	
}

void UnitData::WakeUp()
{
	ClearFlag(k_UDF_IS_ASLEEP);
	
}

void UnitData::CityRadiusFunc(const MapPoint &pos)
{
	Cell *cell = g_theWorld->GetCell(pos);
	if(cell->GetCity().m_id != (0) &&
	   cell->GetCity().GetOwner() != m_owner &&
	   cell->GetCity().IsCapitol() &&
	   g_rand->Next(100) < sint32(g_theConstDB->HearGossipChance() * 100.0)) {
	   	Unit unit = cell->GetCity();
		HearGossip(unit);
	}
}

void UnitData::BeginTurn()
{
	BOOL needsEnqueue = FALSE;
	const UnitRecord *rec = g_theUnitDB->Get(m_type);
	if(rec->GetHearGossip()) {
		AdjacentIterator(m_pos, this);
	}
	   
#if 0
	if(Flag(k_UDF_IS_TRAVELLING_RIFT)) {
		ClearFlag(k_UDF_IS_TRAVELLING_RIFT);
		needsEnqueue = TRUE;
	}

	if(Flag(k_UDF_OPEN_RIFT_GATE)) {
		ClearFlag(k_UDF_OPEN_RIFT_GATE);
	}
#endif

	

	
	
	
	
	

	if(Flag(k_UDF_USED_ACTIVE_DEFENSE)) {
		ClearFlag(k_UDF_USED_ACTIVE_DEFENSE);
		needsEnqueue = TRUE;
	}

	if(Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN)) {
		ClearFlag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN);
		needsEnqueue = TRUE;
	}

	
	Assert(!Flag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW));
	if(Flag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW)) {
		ClearFlag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW);
		needsEnqueue = TRUE;
	}

	if(Flag(k_UDF_TURN_OVER)) {
		ClearFlag(k_UDF_TURN_OVER);
		needsEnqueue = TRUE;
	}

	if(Flag(k_UDF_FOUGHT_THIS_TURN)) {
		ClearFlag(k_UDF_FOUGHT_THIS_TURN);
		needsEnqueue = TRUE;
	}

	if(g_theWorld->IsInstallation(m_pos)) {

		if (rec->GetNoFuelThenCrash() && 
			terrainutil_HasAirfield(m_pos) &&
			g_theWorld->GetOwner(m_pos) == m_owner &&    //EMOD TODO add treaty?
			m_fuel < rec->GetMaxFuel()) {
			m_fuel = rec->GetMaxFuel();
			needsEnqueue = TRUE;
		}
	}


	g_slicEngine->RunUnitBeginTurnTriggers(Unit(m_id));


	if(Flag(k_UDF_ALREADY_PERFORMED_SPACE_TRANSITION)) {
		ClearFlag(k_UDF_ALREADY_PERFORMED_SPACE_TRANSITION);
		needsEnqueue = TRUE;
	}

	if(needsEnqueue) {
		g_network.Block(m_owner);
		ENQUEUE();
		g_network.Unblock(m_owner);
	}
}

void UnitData::EndTurn()
{
	const UnitRecord *rec = g_theUnitDB->Get(m_type);

//EMOD add civbonus

	sint32     wonderBonus  = wonderutil_GetIncreaseHP(g_player[m_owner]->m_builtWonders);
	double          origHP  = m_hp;
	double const	maxHp	= rec->GetMaxHP() + wonderBonus;

    // PFT added: heal immobile units also, 17 Mar 05
	if ((Flag(k_UDF_FIRST_MOVE) || IsImmobile() ) && (m_hp < maxHp))
	{
		if (g_theWorld->HasCity(m_pos) || 
			(g_theWorld->IsInstallation(m_pos) && terrainutil_HasFort(m_pos))
		   ) 
		{
			m_hp += maxHp * g_theConstDB->CityHealRate();
		} 
		else 
		{
			m_hp += maxHp * g_theConstDB->NormalHealRate();
		}

        m_hp = std::min(m_hp, maxHp);
	}

	if(rec->GetNoFuelThenCrash()) {
		if(!CheckForRefuel() && !Flag(k_UDF_IN_SPACE)) {
			m_fuel -= g_theConstDB->NonSpaceFuelCost() * sint32(m_movement_points / 100.0);

			
			if(m_fuel <= 0) {
				Unit me(m_id);
				if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
				   (g_network.IsClient() && g_network.IsLocalPlayer(m_owner))) {
					me.Kill(CAUSE_REMOVE_ARMY_OUTOFFUEL, -1);
					return;
				}
			}
		}
	}

	
	if(g_network.IsActive() && m_hp != origHP) {
		if(g_network.IsHost()) {
			g_network.Block(m_owner);
			g_network.QueuePacketToAll(new NetUnitHP((uint32)m_id, m_hp));
			g_network.Unblock(m_owner);
		}
	}
}

void UnitData::InitializeCityData(sint32 settlerType)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	m_city_data->Initialize(settlerType);
}

void UnitData::AddHappyTimer(sint32 turns, double adjust, HAPPY_REASON reason)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	m_city_data->AddHappyTimer(turns, adjust, reason);
}

void UnitData::EndTurnCity()
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->EndTurn();
}

sint32 UnitData::GetPollution() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return 0;
	return m_city_data->GetPollution();
}

void UnitData::CityToPark(sint32 agressor)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	m_city_data->CityToPark(agressor);
}



void UnitData::RebuildQuadTree()
{
    UnitRecord const * rec = g_theUnitDB->Get(m_type);
    if (    Flag(k_UDF_IS_IN_TRANSPORT) 
         || Flag(k_UDF_HAS_LEFT_MAP) 
         || !rec
         || rec->GetIsTrader()
       )
    {
	    // No action: unit is not at the map
    }
    else
    {
        g_theUnitTree->Insert(Unit(m_id));
    }

	if(m_lesser)
		((UnitData*)(m_lesser))->RebuildQuadTree();

	if(m_greater)
		((UnitData*)(m_greater))->RebuildQuadTree();
}

void UnitData::GetNuked(UnitDynamicArray &killList)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->GetNuked(killList);
}

BOOL UnitData::SafeFromNukes() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;
	return m_city_data->SafeFromNukes();
}

// returns this unit's defense points when attacked (in m_pos) by Unit attacker.
// see also UnitData::GetDefense below, which builds on this.
double UnitData::GetPositionDefense(const Unit &attacker) const
{
	const UnitRecord *myRec = g_theUnitDB->Get(m_type);

	if (g_theWorld->IsWater(m_pos) && 
        !(myRec->GetMovementTypeSea() || myRec->GetMovementTypeShallowWater())
       ) 
    {
		return 1.0;
	}

	Cell *          cell    = g_theWorld->GetCell(m_pos);
    double const    basedef = myRec->GetDefense();
    double          def     = basedef;

	if(cell->GetCity().m_id != (0)) 
	{
		const CityData *cityData = 
			cell->GetCity().GetData()->m_city_data;
		Assert(cityData);
		def += cityData->GetDefendersBonus();

		double wallval,walldef;
		if(cityData->HasCityWalls() && g_theUnitPool->IsValid(attacker))
		{
			wallval=g_featTracker->GetAdditiveEffect(FEAT_EFFECT_REDUCE_CITY_WALLS, attacker.GetOwner());
			if(wallval)
			{
				walldef=buildingutil_GetCityWallsDefense(cityData->GetEffectiveBuildings());
				def=(def+wallval);
			}
//			double deductwall;
//			deductwall = myRec->GetReducesDefensesBonus ();
//			if(deductwall)
//			{
//				def -= deductwall;
//			}
		}
	} 

	if(Flag(k_UDF_IS_ENTRENCHED)) {
		def += basedef * g_theConstDB->GetEntrenchmentBonus();
	}

	double terrain_bonus = 0.0;
	double fort_bonus = 0.0;
	terrainutil_GetDefenseBonus(m_pos, terrain_bonus, fort_bonus);

	def += (basedef * fort_bonus);

	
	if(terrain_bonus > 0 && 
		(myRec->GetMovementTypeLand() && g_theWorld->IsLand(m_pos)) ||
		(myRec->GetMovementTypeMountain() && g_theWorld->IsMountain(m_pos)) ||
		(myRec->GetMovementTypeSea() && g_theWorld->IsWater(m_pos)) ||
		(myRec->GetMovementTypeSpace() && g_theWorld->IsSpace(m_pos))) 
	{
		def += basedef * terrain_bonus;
	}

	return def;
}

// returns this unit's attack points when attacking Unit defender
double UnitData::GetOffense(const Unit &defender) const
{
	const UnitRecord *  myRec   = g_theUnitDB->Get(m_type);
	Unit                city    = g_theWorld->GetCity(m_pos);
	double              base    = myRec->GetAttack();

	if(city.IsValid()) {
		base += city.CD()->GetOffenseBonus(defender);
	}

	sint32 intAttack = (sint32)base;
	sint32 modAttack = g_slicEngine->CallMod(mod_UnitAttack, intAttack, m_id, defender.m_id, intAttack);
	if(modAttack != intAttack)
		base = modAttack;
	return base;
}

// returns this units defense points when defending against Unit attacker 	
double UnitData::GetDefense(const Unit &attacker) const
{
	const UnitRecord *attackRec, *myRec;
	Cell *cell = g_theWorld->GetCell(m_pos);

	attackRec = attacker.GetDBRec();
	myRec = g_theUnitDB->Get(m_type);
	
	double def = GetPositionDefense(attacker);

	if(cell->GetCity().m_id != (0)) {
		const CityData *cityData = cell->GetCity().GetData()->m_city_data;
		Assert(cityData);

		if(attackRec->GetIgnoreCityWalls()) {
			
			def -= cityData->GetDefendersBonus();
			
			def += cityData->GetDefendersBonusNoWalls();
		}
	}




	if(attackRec->GetIsMounted()) {
		def += myRec->GetDefense() * myRec->GetBonusAgainstMounted();
	}

	if(attackRec->GetMovementTypeAir()) {
		def += myRec->GetDefense() * myRec->GetBonusAirDefense();
	}
	//Additional Battle Modifiers by E 12-05-2005
    /// @todo Replace the bool returning parameterless Get functions.
	if(attackRec->GetIsFoot()) {
		def += myRec->GetDefense() * myRec->GetFootBonus();
	}

	if(attackRec->GetIsSiege()) {
		def += myRec->GetDefense() * myRec->GetSiegeBonus();
	}

	if(attackRec->GetIsWheeled()) {
		def += myRec->GetDefense() * myRec->GetWheeledBonus();
	}

	if(attackRec->GetIsMechanized()) {
		def += myRec->GetDefense() * myRec->GetMechanizedBonus();
	}

	if(attackRec->GetIsHelicopter()) {
		def += myRec->GetDefense() * myRec->GetHelicopterBonus();
	}

		if(attackRec->GetIsGuerrilla()) {
		def += myRec->GetDefense() * myRec->GetGuerrillaBonus();
	}

	if(attackRec->GetCivilian()) {
		def += myRec->GetDefense() * myRec->GetCivilianBonus();
	}

		if(attackRec->GetIsSpecialForces()) {
		def += myRec->GetDefense() * myRec->GetSpecialForcesBonus();
	}


	// end EMOD

	sint32 intDef = (sint32)def;
	sint32 modDef = g_slicEngine->CallMod(mod_UnitDefense, intDef, m_id, attacker.m_id, intDef);
	if(modDef != intDef)
		def = modDef;

	return def;
}

// not used
BOOL UnitData::HasAirport() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->HasAirport();
}

// not used
void UnitData::UseAirport()
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	m_city_data->UseAirport();
}

sint32 UnitData::AirportLastUsed() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return -1;

	return m_city_data->AirportLastUsed();
}

BOOL UnitData::HasCityWalls() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->HasCityWalls();
}


BOOL UnitData::HasForceField() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->HasForceField();
}

BOOL UnitData::StoppedBySpies(const Unit &c)
{
	SlicObject	*so ;
	MapPoint pos;
	c.GetPos(pos);
	Cell *cell = g_theWorld->GetCell(pos);
	
	sint32 n = cell->GetNumUnits();

	for (sint32 i = 0; i < n; i++) {
		double chance = 0.0;
        (void) g_theUnitDB->Get(cell->AccessUnit(i).GetType())->GetDefendAgainstSpies(chance);
		
		if (wonderutil_GetSpiesEverywhere(g_player[c.GetOwner()]->GetBuiltWonders()))
        {
            chance = std::max(0.5, chance);
        }

		if(chance > 0.001) {
			if(g_rand->Next(100) <= sint32(chance * 100.0)) {
				DPRINTF(k_DBG_GAMESTATE, ("Spy was stopped by spies stationed in city\n"));
				so = new SlicObject("10zStoppedBySpies") ;
				so->AddRecipient(c.GetOwner()) ;
				so->AddCivilisation(m_owner) ;
				so->AddCity(c) ;
				so->AddUnitRecord(m_type);
				g_slicEngine->Execute(so) ;

				so = new SlicObject("11zStoppedBySpies") ;
				so->AddRecipient(m_owner) ;
				so->AddCivilisation(c.GetOwner()) ;
				so->AddCity(c) ;
				so->AddUnitRecord(m_type);
				g_slicEngine->Execute(so) ;
				Unit me(m_id);
				me.Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);



				return TRUE;
			}
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::InvestigateCity
//
// Description: Order this unit to investigate the city Unit &c.
//
// Parameters : Unit &c       : a city
//
// Globals    : g_theUnitDB
//            : g_slicEngine
//            : g_rand		  :
//				
// Returns    : ORDER_RESULT  : attempt success/failure indication
//
// Remark(s)  :	 
//
//----------------------------------------------------------------------------
ORDER_RESULT UnitData::InvestigateCity(Unit c)
{
	SlicObject	*so ;

	if(StoppedBySpies(c)) {
		return ORDER_RESULT_FAILED;
	}

	double chance, eliteChance, deathChance;
	const UnitRecord::InvestigateCityData *data;
	g_theUnitDB->Get(m_type)->GetInvestigateCity(data);
	chance = data->GetChance();
	eliteChance = data->GetEliteChance();
	deathChance = data->GetDeathChance();

	if(Flag(k_UDF_IS_VET)) {
		chance = eliteChance;
	}

	c.ModifySpecialAttackChance(UNIT_ORDER_INVESTIGATE_CITY, chance);
	c.SetWatchful();

	if(g_rand->Next(100) >= sint32(chance * 100.0)) {
		DPRINTF(k_DBG_GAMESTATE, ("Spy failed\n"));

		if(g_rand->Next(100) < sint32(deathChance * 100.0)) {
			Unit me(m_id);
			so = new SlicObject("10aInvestigateCityFailed") ;
			so->AddRecipient(c.GetOwner()) ;
			so->AddCivilisation(m_owner) ;
			so->AddCity(c) ;
			so->AddUnitRecord(m_type);
			g_slicEngine->Execute(so) ;
	
			so = new SlicObject("11aInvestigateCityFailed") ;
			so->AddRecipient(m_owner) ;
			so->AddCity(c) ;
			so->AddUnitRecord(m_type);
			g_slicEngine->Execute(so) ;

			me.Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);
		} 
		  else {
			so = new SlicObject("10aInvestigateCityFailedEsc") ;
			so->AddRecipient(c.GetOwner()) ;
			so->AddCivilisation(m_owner) ;
			so->AddCity(c) ;
			so->AddUnitRecord(m_type);
			g_slicEngine->Execute(so) ;
	
			so = new SlicObject("11aInvestigateCityFailedEsc") ;
			so->AddRecipient(m_owner) ;
			so->AddCity(c) ;
			so->AddUnitRecord(m_type);
			g_slicEngine->Execute(so) ;
		}


		return ORDER_RESULT_FAILED;
	}
	
	
	ActionSuccessful(SPECATTACK_SPY, c);

	if(m_owner == g_selected_item->GetVisiblePlayer()) {
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_DisplayInvestigationWindow,
							   GEA_Unit, m_id,
							   GEA_City, c.m_id,
							   GEA_End);
	}
	return ORDER_RESULT_SUCCEEDED;
}

ORDER_RESULT UnitData::StealTechnology(Unit c, sint32 whichAdvance)
{
	SlicObject	*so ;

	





	MapPoint pos;
	c.GetPos(pos);

	m_army.InformAI(UNIT_ORDER_STEAL_TECHNOLOGY, pos);
	if(StoppedBySpies(c)) {
		return ORDER_RESULT_FAILED;
	}

	sint32 num;
	uint8 *canSteal = g_player[m_owner]->m_advances->CanAskFor(g_player[c.GetOwner()]->m_advances,
															  num);
	double randChance, specChance, deathChance;
	BOOL r;
	const UnitRecord::StealTechnologyData *data;
	r = g_theUnitDB->Get(m_type)->GetStealTechnology(data);
	randChance = data->GetRandomChance();
	specChance = data->GetSpecificChance();
	deathChance = data->GetDeathChance();
	Assert(r);
	if(!r)
		return ORDER_RESULT_ILLEGAL;

	
	
	if(whichAdvance < 0) {
		if(Flag(k_UDF_IS_VET)) {
			randChance += g_theConstDB->EliteSpyBonus();
		}

		c.ModifySpecialAttackChance(UNIT_ORDER_STEAL_TECHNOLOGY, randChance);
		c.SetWatchful();

		if(g_rand->Next(100) >= sint32(randChance * 100.0)) {
			so = new SlicObject("10bStealTechnologyFailed") ;
			so->AddRecipient(c.GetOwner()) ;
			so->AddCivilisation(m_owner) ;
			so->AddCity(c) ;
			so->AddUnitRecord(m_type);
			g_slicEngine->Execute(so) ;

			so = new SlicObject("11bStealTechnologyFailed") ;
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(c.GetOwner()) ;
			so->AddCity(c) ;
			so->AddUnitRecord(m_type);
			g_slicEngine->Execute(so) ;
			Unit me(m_id);

			if(g_rand->Next(100) < sint32(deathChance * 100.0))
				me.Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);

			goto failed;
		}

		
		sint32 i, n = num;
		if(num > 0) {
			sint32 count = 0;
			sint32 which = g_rand->Next(num);

			for(i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
				if(canSteal[i]) {
					if(which == count) {
						g_player[m_owner]->m_advances->GiveAdvance(i, CAUSE_SCI_COMBAT);
						break;
					}
					count++;
				}
			}
			Assert(i < g_theAdvanceDB->NumRecords());
			
			so = new SlicObject("11bStoleTechnology") ;
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(c.GetOwner()) ;
			so->AddAdvance(i) ;
			so->AddCity(c) ;
			so->AddUnitRecord(m_type);
			g_slicEngine->Execute(so);

			so = new SlicObject("186StealTechnologyVictim");
			so->AddRecipient(c.GetOwner()) ;
			so->AddCivilisation(m_owner) ;
			so->AddAdvance(i) ;
			so->AddCity(c) ;
			so->AddUnitRecord(m_type);
			g_slicEngine->Execute(so);
		} else {
			so = new SlicObject("11bNothingToSteal") ;
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(c.GetOwner()) ;
			so->AddCity(c) ;
			so->AddUnitRecord(m_type);
			g_slicEngine->Execute(so) ;
		}
		
	} else {
		if(Flag(k_UDF_IS_VET)) {
			specChance += g_theConstDB->EliteSpyBonus();
		}

		c.ModifySpecialAttackChance(UNIT_ORDER_STEAL_TECHNOLOGY, specChance);
		c.SetWatchful();

		if(g_rand->Next(100) > sint32(specChance * 100.0)) {
			
			so = new SlicObject("10bStealTechnologyFailed") ;
			so->AddRecipient(c.GetOwner()) ;
			so->AddCivilisation(m_owner) ;
			so->AddCity(c) ;
			so->AddUnitRecord(m_type);
			g_slicEngine->Execute(so) ;

			so = new SlicObject("11bStealTechnologyFailed") ;
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(c.GetOwner()) ;
			so->AddCity(c) ;
			so->AddUnitRecord(m_type);
			g_slicEngine->Execute(so) ;
			Unit me(m_id);
			if(g_rand->Next(100) < sint32(deathChance * 100.0))
				me.Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);
			
			goto failed;
		}
		
		
		g_player[m_owner]->m_advances->GiveAdvance(whichAdvance, CAUSE_SCI_COMBAT);
		
		so = new SlicObject("11bStoleTechnology") ;
		so->AddRecipient(m_owner) ;
		so->AddCivilisation(c.GetOwner()) ;
		so->AddAdvance(whichAdvance) ;
		so->AddCity(c) ;
		so->AddUnitRecord(m_type);
		g_slicEngine->Execute(so) ;

		so = new SlicObject("186StealTechnologyVictim");
		so->AddRecipient(c.GetOwner()) ;
		so->AddCivilisation(m_owner) ;
		so->AddAdvance(whichAdvance) ;
		so->AddCity(c) ;
		so->AddUnitRecord(m_type);
		g_slicEngine->Execute(so);
	}

	
	ActionSuccessful(SPECATTACK_STEALTECH, c);
	return ORDER_RESULT_SUCCEEDED;

failed:
	

	return ORDER_RESULT_FAILED;
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::InciteRevolution
//
// Description: Order this unit to incite a revolution in the city Unit &c.
//
// Parameters : Unit &c             : a city
//
// Globals    : g_theUnitDB
//            : g_slicEngine
//            : g_rand		  :
//				
// Returns    : ORDER_RESULT  : attempt success/failure indication
//
// Remark(s)  :	
//
//----------------------------------------------------------------------------
ORDER_RESULT UnitData::InciteRevolution(Unit c)
{
	if (StoppedBySpies(c)) {
		return ORDER_RESULT_FAILED;
	}

	const UnitRecord::InciteRevolutionData *data;
	g_theUnitDB->Get(m_type)->GetInciteRevolution(data);
	double chance = data->GetChance();
	double eliteChance = data->GetEliteChance();
	double deathChance = data->GetDeathChance();

	if (Flag(k_UDF_IS_VET)) {
		chance = eliteChance;
	}
	
	c.ModifySpecialAttackChance(UNIT_ORDER_INCITE_REVOLUTION, chance);
	c.SetWatchful();

	SlicObject	*so;
	if(g_rand->Next(100) >= sint32(chance * 100.0)) {
		DPRINTF(k_DBG_GAMESTATE, ("Spy failed\n"));

		so = new SlicObject("10cInciteRevolutionFailed") ;
		so->AddRecipient(c.GetOwner()) ;
		so->AddCivilisation(m_owner) ;
		so->AddCity(c) ;
		so->AddUnitRecord(m_type);
		g_slicEngine->Execute(so) ;

		so = new SlicObject("11cInciteRevolutionFailed") ;
		so->AddRecipient(m_owner) ;
		so->AddCivilisation(c.GetOwner()) ;
		so->AddCity(c) ;
		so->AddUnitRecord(m_type);
		g_slicEngine->Execute(so) ;

		if(g_rand->Next(100) < sint32(deathChance * 100.0)) {
			Unit me(m_id);
			me.Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);
		}
		return ORDER_RESULT_FAILED;
	}
	
	

    PLAYER_INDEX city_owner = c.GetOwner();
	c.AccessData()->m_city_data->Revolt(g_player[c.GetOwner()]->m_civRevoltingCitiesShouldJoin, TRUE);
	c.SetSpiedUpon();

	
	ActionSuccessful(SPECATTACK_INCITEREVOLUTION, c);

    so = new SlicObject("171InciteRevolutionCompleteVictim") ;
    so->AddRecipient(city_owner) ;
    so->AddCity(c) ;
	so->AddUnitRecord(m_type);
    g_slicEngine->Execute(so) ;

	return ORDER_RESULT_SUCCEEDED;
}

ORDER_RESULT UnitData::AssassinateRuler(Unit c)
{
	SlicObject	*so ;

	if(StoppedBySpies(c)) {
		return ORDER_RESULT_FAILED;
	}

	double chance, eliteChance, deathChance;
	const UnitRecord::AssasinateRulerData *data;
	g_theUnitDB->Get(m_type)->GetAssasinateRuler(data);
	chance = data->GetChance();
	eliteChance = data->GetEliteChance();
	deathChance = data->GetDeathChance();
	
	if(Flag(k_UDF_IS_VET))
		chance = eliteChance;

	c.ModifySpecialAttackChance(UNIT_ORDER_ASSASSINATE, chance);
	c.SetWatchful();

	if(g_rand->Next(100) >= sint32(chance * 100.0)) {
		DPRINTF(k_DBG_GAMESTATE, ("Assassination failed."));
		so = new SlicObject("10dAssassinationFailed") ;
		so->AddRecipient(c.GetOwner()) ;
		so->AddCivilisation(m_owner) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		so = new SlicObject("11dAssassinationFailed") ;
		so->AddRecipient(m_owner) ;
		so->AddCivilisation(c.GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;
		if(g_rand->Next(100) < sint32(deathChance * 100.0)) {
			Unit me(m_id);
			me.Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);
		}

		


		return ORDER_RESULT_FAILED;
	}

	DPRINTF(k_DBG_GAMESTATE, ("assasination succeeded\n"));
	g_player[c.GetOwner()]->SetGovernmentType(0); 
	g_player[c.GetOwner()]->AssasinateRuler();

	
	ActionSuccessful(SPECATTACK_BOMBCABINET, c);
	return ORDER_RESULT_SUCCEEDED;
}

//leftover from CTP1 ?
ORDER_RESULT UnitData::NullifyWalls(Unit c)
{
#if 0
	if(StoppedBySpies(c)) {
		return ORDER_RESULT_FAILED;
	}

	double chance, eliteChance, deathChance;
	g_theUnitDB->Get(m_type)->AssasinateRuler(chance, eliteChance, deathChance);
	
	if(Flag(k_UDF_IS_VET))
		chance = eliteChance;

	c.ModifySpecialAttackChance(UNIT_ORDER_NULLIFY_WALLS, chance);

	if(g_rand->Next(100) >= sint32(chance * 100.0)) {
		DPRINTF(k_DBG_GAMESTATE, ("City wall nullification failed."));

		if(g_rand->Next(100) < sint32(deathChance * 100.0)) {
			Unit me(m_id);
			me.Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);
		}
		return ORDER_RESULT_FAILED;
	}

	DPRINTF(k_DBG_GAMESTATE, ("City wall nullification succeeded\n"));
	c.CityNullifyWalls();
	c.SetWatchful();
	return ORDER_RESULT_SUCCEEDED;
#endif
	return ORDER_RESULT_FAILED;
}

BOOL UnitData::HasBeenSpiedUpon() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->HasBeenSpiedUpon();
}

void UnitData::SetSpiedUpon()
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	m_city_data->SetSpiedUpon();
}

void UnitData::CityNullifyWalls()
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->CityNullifyWalls();
}

ORDER_RESULT UnitData::EstablishEmbassy(Unit c)
{
	Assert(g_theUnitDB->Get(m_type)->GetEstablishEmbassy());
	if(!g_theUnitDB->Get(m_type)->GetEstablishEmbassy())
		return ORDER_RESULT_ILLEGAL;

	if(g_player[m_owner]->HasEmbassyWith(c.GetOwner())) {
		return ORDER_RESULT_ILLEGAL;
	}

	if(wonderutil_GetCloseEmbassies(g_player[c.GetOwner()]->GetBuiltWonders())) {
		
		return ORDER_RESULT_FAILED;
	}


	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_EstablishEmbassy,
						   GEA_Player, m_owner,
						   GEA_Player, c.GetOwner(),
						   GEA_End);
	

	ActionSuccessful(SPECATTACK_ESTABLISHEMBASSY, c);
	return ORDER_RESULT_SUCCEEDED;
}

ORDER_RESULT UnitData::ThrowParty(Unit c, sint32 gold)
{
	Assert(g_theUnitDB->Get(m_type)->GetThrowParty());
	if(!g_theUnitDB->Get(m_type)->GetThrowParty())
		return ORDER_RESULT_ILLEGAL;

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ThrowParty,
						   GEA_Player, m_owner,
						   GEA_Player, c.GetOwner(),
						   GEA_End);
	

	ActionSuccessful(SPECATTACK_THROWPARTY, c);
	return ORDER_RESULT_SUCCEEDED;
}

void UnitData::HearGossip(Unit c)
{
	MapPoint center;
	sint32 oplayer = c.GetOwner();
	sint32 cost;
	sint32 maxCost = 0;
	UnitDynamicArray maxCostUnits;
	SlicObject *so = NULL;

	switch(g_rand->Next(3)) {
        case 0: {
            

            sint32 i, num;
            uint8 *canSteal = g_player[m_owner]->m_advances->
                CanAskFor(g_player[c.GetOwner()]->m_advances, num);
            
            for(i=0; i<num; i++) {
                if (canSteal[i]) {
                    g_player[m_owner]->m_advances->GiveAdvance(i, CAUSE_SCI_COMBAT);

                    so = new SlicObject("146GossipCompleteAttacker") ;
                    so->AddRecipient(m_owner) ;
                    so->AddAdvance(i) ;
                    g_slicEngine->Execute(so);

                    break;
                }                    
            }
            break;
        }

		case 2:
			if(0) {
				
				
				
			} else {
				sint32 i, n = g_player[oplayer]->m_all_armies->Num();
				for(i = 0; i < n; i++) {
					cost = g_player[oplayer]->m_all_armies->Access(i).GetCost();
					if(cost > maxCost) {
						cost = maxCost;
						maxCostUnits.Clear();
						maxCostUnits.Insert(g_player[oplayer]->m_all_armies->
											Access(i).Access(0));
					} else if (cost == maxCost) {
						maxCostUnits.Insert(g_player[oplayer]->m_all_armies->
											Access(i).Access(0));
					}
				}
				if(maxCostUnits.Num() <= 0) {
					
					
					so = new SlicObject("97GossipBoring");
					so->AddCivilisation(oplayer);
					so->AddRecipient(m_owner);
					g_slicEngine->Execute(so);
					return;
				}
				n = g_rand->Next(maxCostUnits.Num());
				maxCostUnits[n].GetPos(center);
			}
			so = new SlicObject("98GossipMap");
			so->AddCivilisation(oplayer);
			so->AddRecipient(m_owner);
			so->AddLocation(center);
			g_slicEngine->Execute(so);

			g_player[m_owner]->m_vision->CopyCircle(g_player[oplayer]->m_vision,
													center,
													g_theConstDB->GossipMapRadius());
				
			break;
		case 1:
		{
			DPRINTF(k_DBG_GAMESTATE, ("Readiness: %d\n",
									  g_player[c.GetOwner()]->m_readiness->GetLevel()));
			switch(g_player[c.GetOwner()]->m_readiness->GetLevel()) {
				case 0:
					so = new SlicObject("96GossipPeacetimeReadiness");
					break;
				case 1:
					so = new SlicObject("95GossipAlertReadiness");
					break;
				case 2:
					so = new SlicObject("94GossipFullReadiness");
					break;
			}
			so->AddRecipient(m_owner);
			so->AddCivilisation(c.GetOwner());
			g_slicEngine->Execute(so);
			
			break;
		}
	}

	ActionSuccessful(SPECATTACK_HEARGOSSIP, c);
}

BOOL UnitData::IsCapitol() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->IsCapitol();
}

void UnitData::MakeFranchise(sint32 player)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	m_city_data->MakeFranchise(player);

}

sint32 UnitData::GetFranchiseOwner() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->GetFranchiseOwner();
}

void UnitData::SetFranchiseTurnsRemaining(sint32 turns)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	m_city_data->SetFranchiseTurnsRemaining(turns);
}

sint32 UnitData::GetFranchiseTurnsRemaining() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return -1;

	return m_city_data->GetFranchiseTurnsRemaining();
}

BOOL UnitData::CanSee(const Army &al) const
{
	sint32 i, n = al.Num();
	for(i = 0; i < n; i++) {
		if(al[i].GetVisibility() & (1 << m_owner))
			return TRUE;
	}
	return FALSE;
}

#ifdef _DEBUG
void UnitData::SetIgnoreHappiness(BOOL v) 

{ 
    m_city_data->SetIgnoreHappiness(v); 
}
#endif

void UnitData::MakeCitizen(PopDBIndex pi, const MapPoint &point, sint32 origOwner)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	
	Assert(g_gevManager->IsProcessing());

	m_city_data->MakeCitizen(pi, point, origOwner);
}

sint32 UnitData::PopCount() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return m_city_data->PopCount();
}

sint32 UnitData::CountSlaves() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return m_city_data->SlaveCount();
}

BOOL UnitData::IsWatchful() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->IsWatchful();
}

void UnitData::SetWatchful()
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->SetWatchful();
}

void UnitData::ModifySpecialAttackChance(UNIT_ORDER_TYPE attack,
										 double &chance)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->ModifySpecialAttackChance(attack, chance);
}

void UnitData::RemoveOneSlave(PLAYER_INDEX p)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->RemoveOneSlave(p);
}

void UnitData::DoUprising(UPRISING_CAUSE cause)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->DoUprising(cause);

}

BOOL UnitData::IsTempSlaveUnit() const
{
	return Flag(k_UDF_TEMP_SLAVE_UNIT);
}

void UnitData::SetTempSlaveUnit(BOOL b)
{
	if(b)
		SetFlag(k_UDF_TEMP_SLAVE_UNIT);
	else
		ClearFlag(k_UDF_TEMP_SLAVE_UNIT);
}

void UnitData::BioInfect( sint32 player )
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	m_city_data->BioInfect( player );
}

void UnitData::NanoInfect( sint32 player )
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->NanoInfect( player );
}

BOOL UnitData::IsBioImmune() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;
	return m_city_data->IsBioImmune();
}

BOOL UnitData::IsNanoImmune() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;
	return m_city_data->IsNanoImmune();
}

void UnitData::ConvertTo(sint32 player, CONVERTED_BY by)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	m_city_data->ConvertTo(player, by);
}

double UnitData::TheologicalModifier() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return 0.0;
	return m_city_data->TheologicalModifier();
}

void UnitData::Unconvert()
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->Unconvert();
}

sint32 UnitData::IsConvertedTo() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return -1;
	return m_city_data->IsConvertedTo();
}

BOOL UnitData::IsCloaked() const
{
	return Flag(k_UDF_IS_CLOAKED);
}

void UnitData::Cloak()
{
    bool out_of_fuel; 
	DeductMoveCost(Unit(m_id), g_theConstDB->SpecialActionMoveCost(), out_of_fuel);
	SetFlag(k_UDF_IS_CLOAKED);
}

void UnitData::Uncloak()
{
    bool out_of_fuel; 
	DeductMoveCost(Unit(m_id), g_theConstDB->SpecialActionMoveCost(), out_of_fuel);
	ClearFlag(k_UDF_IS_CLOAKED);
}

#if 0
BOOL UnitData::HasOpenRiftGate(MapPoint &destination) const
{
	if(!Flag(k_UDF_OPEN_RIFT_GATE))
		return FALSE;

	destination = m_riftDestination;
	return TRUE;
}

BOOL UnitData::OpenRiftGate(const MapPoint &destination)
{
	Assert(g_player[m_owner]->IsVisible(destination));
	if(!g_player[m_owner]->IsVisible(destination))
		return FALSE;

	SetFlag(k_UDF_OPEN_RIFT_GATE);
	m_riftDestination = destination;

	m_movement_points = 0;
	ClearFlag(k_UDF_FIRST_MOVE);
	
	return TRUE;
}

BOOL UnitData::IsTravellingRift() const
{
	return Flag(k_UDF_IS_TRAVELLING_RIFT);
}

void UnitData::SetTravellingRift()
{
	SetFlag(k_UDF_IS_TRAVELLING_RIFT);
}
#endif

BOOL UnitData::HasResource(const sint32 resource) const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->HasResource(resource);
}

#ifdef CTP1_TRADE
sint32 UnitData::GetResourceCount(const sint32 resource) const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->GetResourceCount(resource);
}

sint32 UnitData::GetLocalResourceCount(const sint32 resource) const
{
	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return m_city_data->GetLocalResourceCount(resource);
}

const Resources *UnitData::GetResources() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return NULL;
	return m_city_data->GetResources();
}
#endif

sint32 UnitData::GetNumTradeRoutes() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return 0;
	return m_city_data->GetNumTradeRoutes();
}

sint32 UnitData::GetStoredCityProduction() const 
{
  	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return m_city_data->GetStoredCityProduction();
} 

sint32 UnitData::GetNetCityProduction() const 
{
  	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return m_city_data->GetNetCityProduction();
} 

sint32 UnitData::GetGrossCityProduction() const 
{
  	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return m_city_data->GetGrossCityProduction();
} 


sint32 UnitData::GetStoredCityFood() const 
{
  	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return m_city_data->GetStoredCityFood();
} 

sint32 UnitData::GetNetCityFood() const 
{
  	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return m_city_data->GetNetCityFood();
} 










sint32 UnitData::GetGrossCityFood() const 
{
  	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return m_city_data->GetGrossCityFood();
} 


sint32 UnitData::GetNetCityGold() const 
{
  	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return m_city_data->GetNetCityGold();
} 

sint32 UnitData::GetGrossCityGold() const 
{
  	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return m_city_data->GetGrossCityGold();
} 

void UnitData::ForceVisibleDuration(const PLAYER_INDEX to_me, sint32 duration)
{
	Assert(duration >= 0);
	m_temp_visibility_array.SetVisibleDuration(to_me, duration);
	m_temp_visibility |= (1 << to_me);
}

void VisibilityDurationArray::Serialize(CivArchive &archive)
{

	if(archive.IsStoring()) {
		archive << m_array_index;
		archive.Store((uint8*)m_array, k_DEFAULT_VIS_DURATION_SIZE * sizeof(uint32));
	} else {
		archive >> m_array_index;
		archive.Load((uint8*)m_array, k_DEFAULT_VIS_DURATION_SIZE * sizeof(uint32));
	}
}

BOOL UnitData::IsNanoInfected() const
{
    Assert(m_city_data);
    if(!m_city_data) return FALSE;

    return m_city_data->IsNanoInfected();
}

BOOL UnitData::IsBioInfected() const
{
    Assert(m_city_data);
    if(!m_city_data) return FALSE;

    return m_city_data->IsBioInfected();
}

BOOL UnitData::IsFranchised() const
{
    Assert(m_city_data);
    if(!m_city_data) return FALSE;

    return m_city_data->IsFranchised();
}

BOOL UnitData::IsConverted() const
{
    Assert(m_city_data);
    if(!m_city_data) return FALSE;

    return m_city_data->IsConverted();
}

BOOL UnitData::BuyFront()
{
   Assert(m_city_data);
   if(!m_city_data) return FALSE;
   return m_city_data->BuyFront();
}

sint32 UnitData::GetOvertimeCost() const
{
	Assert(m_city_data);
	if(!m_city_data) return 0x7fffffff;
	return m_city_data->GetOvertimeCost();
}

sint32 UnitData::HowMuchLonger() const
{
	Assert(m_city_data);
	if(!m_city_data) return 0;
	return m_city_data->HowMuchLonger();
}

// not used
sint32 UnitData::GetCombatUnits() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return 0;
	return m_city_data->GetCombatUnits();
}

BOOL UnitData::CanBuildUnit(sint32 type) const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;
	return m_city_data->CanBuildUnit(type);
}

BOOL UnitData::CanBuildBuilding(sint32 type) const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;
	return m_city_data->CanBuildBuilding(type);
}





//CanBuildInfrastructure ? 





BOOL UnitData::CanBuildCapitalization() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;
	return m_city_data->CanBuildCapitalization();
}

BOOL UnitData::CanBuildWonder(sint32 type) const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;
	return m_city_data->CanBuildWonder(type);
}

void UnitData::RemoveWonderFromQueue(sint32 type)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->RemoveWonderFromQueue(type);
}

void UnitData::RemoveFront()
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->RemoveFront();
}

void UnitData::Injoin(sint32 player)
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->Injoin(player);


}

BOOL UnitData::IsInjoined() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;
	return m_city_data->IsInjoined();
}

sint32 UnitData::InjoinedBy() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return -1;
	return m_city_data->InjoinedBy();
}

// not used
double UnitData::GetDistanceToCapitol() const
{
	MapPoint cappos;
	g_player[m_owner]->GetCapitolPos(cappos);
	return double(m_pos.NormalizedDistance(cappos));
}

BOOL UnitData::GetCurrentOrderString(StringId &id) const
{
	char *name = NULL;

	if(Flag(k_UDF_IS_ENTRENCHED) || Flag(k_UDF_IS_ENTRENCHING)) {
		name = "UNIT_ORDER_ENTRENCH";
	} else if(Flag(k_UDF_IS_ASLEEP)) {
		name = "UNIT_ORDER_SLEEP";
	} else if(m_army.m_id == (0)) {
		Assert(IsBeingTransported());
		name = "UNIT_ORDER_TRANSPORTED";
	} else if(m_army.NumOrders() > 0 && m_army.GetOrder(0)) {
		switch(m_army.GetOrder(0)->m_order) {
			case UNIT_ORDER_MOVE:
			case UNIT_ORDER_MOVE_TO:
				name = "UNIT_ORDER_MOVE";
				break;
			case UNIT_ORDER_PATROL:
			case UNIT_ORDER_CIRCULAR_PATROL:
				name = "UNIT_ORDER_PATROL";
				break;
			default:
				name = "UNIT_ORDER_NONE";
				break;
		}
	} else {
		name = "UNIT_ORDER_NONE";
	}
				
	Assert(name != NULL);
	if(name == NULL)
		return FALSE;

	return g_theStringDB->GetStringID(name, id);
}

#if 0
void UnitData::SetOrders(UNIT_ORDER_TYPE order)
{
	
	if(m_currentOrders != UNIT_ORDER_SLEEP) {
		ClearFlag(k_UDF_IS_ASLEEP);
	}
	if(m_currentOrders != UNIT_ORDER_ENTRENCH) {
		ClearFlag(k_UDF_IS_ENTRENCHED);
		ClearFlag(k_UDF_IS_ENTRENCHING);
	}
}
#endif

// not used !
double UnitData::GetOverseasDistress() const
{
	Unit nearCity;
	double distance;
	double radius = g_player[m_owner]->GetAtHomeRadius();

	if(Unit(m_id).IsContentOverseas())
		return 0.0;

	if(!g_player[m_owner]->GetNearestCity(m_pos, nearCity, distance)) {
		return 0.0;
	}

	if(distance < radius) {
		return 0.0;
	}

	return (distance - radius) * g_player[m_owner]->GetOverseasCoef();
}

void UnitData::SupportBuildings()
{
	Assert(FALSE);
	return;	
}

void UnitData::CheckRiot()
{
	Assert(m_city_data);
	if(!m_city_data)
		return;
	m_city_data->CheckRiot();
}

BOOL UnitData::AiGetCargoMovementPoints(double &min_move_points, 
   BOOL  &first) const 
{ 

    if (m_cargo_list) { 
       
        
       sint32 idx_cargo, num_cargo; 
       num_cargo = m_cargo_list->Num(); 
       if (num_cargo < 1) { 
           return FALSE; 
       } 
       double mp; 
       for (idx_cargo=0; idx_cargo<num_cargo; idx_cargo++) { 

           mp = m_cargo_list->Get(idx_cargo).GetMovementPoints();
           if (mp < min_move_points) { 
               min_move_points = mp; 
           } 
           if (Flag(k_UDF_FIRST_MOVE)) { 
               first = FALSE; 
           }
       }
       return TRUE; 
    } else {
        return FALSE; 
    }
}

#if 0
void UnitData::SetLastBattle(sint32 turn)
{
	m_last_battle_turn = turn;
}

sint32 UnitData::GetLastBattle() const
{
	return m_last_battle_turn;
}
#endif
	
void UnitData::SetIsInTransport(const Unit &transport)
{ 
	SetFlag(k_UDF_IS_IN_TRANSPORT);
	m_army.Del(Unit(m_id));
	m_army.m_id = (0);
	m_transport = transport;
	
}

void UnitData::UnsetIsInTransport()
{ 
	if(CanBeachAssaultRightNow()) {
		SetFlag(k_UDF_BEACH_ASSAULT_LEGAL);
	} else {
		ClearFlag(k_UDF_BEACH_ASSAULT_LEGAL);
	}
	ClearFlag(k_UDF_IS_IN_TRANSPORT);
	m_transport.m_id = (0);
	
}

void UnitData::SetIsProfessional(BOOL on)
{
	SetFlag(k_UDF_IS_PROFESSIONAL);
	ENQUEUE(); 
}

void UnitData::SetOwner(PLAYER_INDEX newo)
{ 
	m_owner = newo; 
	ENQUEUE(); 
}

void UnitData::SetType(sint32 type)
{ 
	m_type = type; 
	//ENQUEUE(); 
}


void UnitData::SetHP(const double hp)
{
	m_hp = hp;
	
}

void UnitData::SetMovementPoints(double mp)
{ 
	if(!Flag(k_UDF_PACMAN))
		m_movement_points = mp; 
	
	
}

BOOL UnitData::PacMan() const { return Flag(k_UDF_PACMAN); }
void UnitData::SetPacMan() 
{ 
	SetFlag(k_UDF_PACMAN);
	
	
}

//Not used
void UnitData::SetFirstMoveThisTurn(sint32 fm)
{
	if(fm)
		SetFlag(k_UDF_FIRST_MOVE);
	else
		ClearFlag(k_UDF_FIRST_MOVE);
}

// not used
void UnitData::SetFuel(sint32 fuel)
{
	m_fuel = fuel;
	
}

BOOL UnitData::GetUsedFuel (sint32 &fuel_remaining, sint32 &max_fuel) const
{
    if (!g_theUnitDB->Get(m_type)->GetNoFuelThenCrash()) return FALSE;

    fuel_remaining = m_fuel; 

    max_fuel = g_theUnitDB->Get(m_type)->GetMaxFuel(); 

    return TRUE; 
}

void UnitData::SetVeteran()
{ 
    if (!Flag(k_UDF_IS_VET)) { 
        SetFlag(k_UDF_IS_VET);
        SlicObject *so = new SlicObject("250UnitGainedVeteranStatus");
        so->AddUnit(Unit(m_id));
        so->AddRecipient(m_owner);
        g_slicEngine->Execute(so);
    }
}

void UnitData::UnVeteran()
{ 
    if (Flag(k_UDF_IS_VET)) 
        ClearFlag(k_UDF_IS_VET);
}

void UnitData::SetPatrolling(BOOL patrolling)
{ 
	if(patrolling)
		SetFlag(k_UDF_IS_PATROLLING);
	else
		ClearFlag(k_UDF_IS_PATROLLING);
	
}

double UnitData::GetHP() const
{
	if(g_theUnitDB->Get(m_type)->GetIsSpecialForces() || Flag(k_UDF_IS_PROFESSIONAL)) {
		return m_hp;
	} else {
		return m_hp * g_player[m_owner]->m_readiness->GetHPModifier();
	}
}

void UnitData::RecalculateResources()
{
	Assert(m_city_data);
	if(!m_city_data)
		return;

	m_city_data->RecalculateResources();
}

BOOL UnitData::FightOneLineDanceRangedAttack(Unit &defender)
{
	double d = defender.GetDefense(Unit(m_id));
	const UnitRecord *rec = g_theUnitDB->Get(m_type);
	double r = rec->GetZBRangeAttack();
	sint32 firepower = rec->GetFirepower();

	double chance = r / (r + d);

	
	if(!(rec->GetCanAttack() & defender.GetDBRec()->GetMovementType())) {
		return FALSE;
	}

	if(g_rand->Next(1000) < sint32(chance * 1000)) {
		if(firepower > 1) {
			defender.DeductHP(g_rand->Next(rec->GetFirepower() - 1) + 1);
		} else {
			defender.DeductHP(1);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL UnitData::FightOneLineDanceAssault(Unit &defender)
{
	double d = defender.GetDefense(Unit(m_id));
	double a = GetAttack(g_theUnitDB->Get(m_type), defender);

	double chance = a / (a + d);

	if(g_rand->Next(1000) < sint32(chance * 1000)) {
		defender.DeductHP(1);
		return TRUE;
	} else {
		DeductHP(1);
	}

	return FALSE;
}

sint32 UnitData::CountTradeWith(PLAYER_INDEX player) const
{
	Assert(m_city_data);
	if(!m_city_data)
		return 0;

	return m_city_data->CountTradeWith(player);
}	

BOOL UnitData::IsProtectedFromSlavery() const
{
	Assert(m_city_data);
	if(!m_city_data)
		return FALSE;

	return m_city_data->IsProtectedFromSlavery() > 0.0;
}


void UnitData::NotifyAdvance(AdvanceType advance)
{
	if(m_city_data)
		m_city_data->NotifyAdvance(advance);
	
}

sint32 UnitData::CreateOwnArmy()
{
	if(m_army.m_id != (0) && m_army.Num() < 2)
		return 1;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_CREATE_OWN_ARMY,
										   (uint32)m_id));
	}

	Army newArmy = g_player[m_owner]->GetNewArmy(CAUSE_NEW_ARMY_UNKNOWN);
	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(g_theArmyPool->AccessArmy(newArmy));
		g_network.Unblock(m_owner);
	}
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddUnitToArmy,
						   GEA_Unit, m_id,
						   GEA_Army, newArmy,
						   GEA_Int, CAUSE_NEW_ARMY_UNKNOWN,
						   GEA_End);
	

	
	
	
	return 1;
}

void UnitData::GiveCommand(UNIT_COMMAND command)
{
	if(Flag(k_UDF_IS_IN_TRANSPORT))
		return;

	
	
	
	if(m_army.m_id == (0) || m_army.Num() != 1) {
		CreateOwnArmy();
	}
	g_player[m_owner]->GiveArmyCommand(m_army, command);
}

void UnitData::ExitWormhole(MapPoint &pos)
{
	ClearFlag(k_UDF_HAS_LEFT_MAP);
	SetFlag(k_UDF_RETURNED_FROM_WORMHOLE);
	ClearFlag(k_UDF_IN_WORMHOLE);
	g_player[m_owner]->RecoveredProbe(Unit());

#if 0
	Cell *cell = g_theWorld->GetCell(pos);
    UnitDynamicArray revealedUnits;
	Unit me(m_id);
	

	
	if(cell->UnitArmy()) {
		if(cell->UnitArmy()->GetOwner() == m_owner &&
		   cell->UnitArmy()->Num() < k_MAX_ARMY_SIZE) {
			SetPosAndNothingElse(pos);
			m_army.ResetPos();
			g_theWorld->InsertUnit(pos, Unit(m_id), revealedUnits);
			AddUnitVision(revealed);

			SlicObject *so = new SlicObject("307EndGameProbeReturned");
			so->AddUnit(Unit(m_id));
			so->AddRecipient(m_owner);
			so->AddCivilisation(m_owner);
			g_slicEngine->Execute(so);

			if(cell->GetCity().m_id != 0) {
				g_player[m_owner]->RecoveredProbe(cell->GetCity());
			}

			
			
			
		} else {
			WORLD_DIRECTION d = (WORLD_DIRECTION)g_rand->Next(UP);
			MapPoint npos;			
			do {
				d = (WORLD_DIRECTION)((sint32)d + 1);
				if((sint32)d >= (sint32)UP) {
					d = (WORLD_DIRECTION)0;
				}
			} while(!pos.GetNeighborPosition(d, npos));

			ExitWormhole(npos);
		}
	} else {
		SetPosAndNothingElse(pos);
		m_army.ResetPos();
		g_theWorld->InsertUnit(pos, Unit(m_id), revealedUnits);
		AddUnitVision(revealed);

		SlicObject *so = new SlicObject("307EndGameProbeReturned");
		so->AddUnit(Unit(m_id));
		so->AddRecipient(m_owner);
		so->AddCivilisation(m_owner);
		g_slicEngine->Execute(so);

		if(cell->GetCity().m_id != 0) {
			g_player[m_owner]->RecoveredProbe(cell->GetCity());
		}
		
	}
#endif
}

BOOL UnitData::HasLeftMap() const
{
	return Flag(k_UDF_HAS_LEFT_MAP);
}

//not used
void UnitData::ContributeScience(double incomePercent,
								 double &addscience,
								 double &subgold)
{
	Assert(m_city_data);
	if(m_city_data) {
		m_city_data->ContributeScience(incomePercent,
									   addscience, subgold);
	}
}

sint32 UnitData::FreeSlaves()
{
	Assert(m_city_data);
	if(m_city_data) {
		return m_city_data->FreeSlaves();
	}
	return 0;
}

void UnitData::ChangeArmy(const Army &army, CAUSE_NEW_ARMY cause)
{
	if(army == m_army)
		return;

	if(g_network.IsClient()) {
		if(!army.IsValid()) {
			g_network.RequestResync(RESYNC_INVALID_ARMY_OTHER);
			return;
		}
	}

	
	Assert(!Flag(k_UDF_IS_IN_TRANSPORT));
	if(Flag(k_UDF_IS_IN_TRANSPORT))
		return;

	if(g_network.IsHost()) {
		if(cause != CAUSE_NEW_ARMY_INITIAL && cause != CAUSE_NEW_ARMY_REMOTE_UNGROUPING)
			g_network.Block(m_owner);

		g_network.Enqueue(new NetInfo(NET_INFO_CODE_CHANGE_ARMY,
									  m_id, m_army, army));

		if(cause != CAUSE_NEW_ARMY_INITIAL && cause != CAUSE_NEW_ARMY_REMOTE_UNGROUPING)
			g_network.Unblock(m_owner);
	} else if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner) && cause != CAUSE_NEW_ARMY_NETWORK) {
		g_network.SendAction(new NetAction(NET_ACTION_GROUP_ARMY,
										   army.m_id, m_id));
	}

	if(g_theArmyPool->IsValid(m_army)) {
		m_army.Del(Unit(m_id));
	}
	m_army = army;
	m_army.Insert(Unit(m_id));

}

void UnitData::SetArmy(const Army &army)
{
	Assert(m_army == army || !m_army.IsValid() || (m_army.IsValid() && !m_army->IsPresent(Unit(m_id))));

	m_army = army;

	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_ARMY,
									  m_id, (uint32)army));
		g_network.Unblock(m_owner);
	}
}

void UnitData::KillTransportedUnits()
{
	if(m_cargo_list) {
		sint32 i;
		for(i = m_cargo_list->Num() - 1; i >= 0; i--) {
			m_cargo_list->Access(i).Kill(CAUSE_REMOVE_ARMY_TRANSPORT_DIED, -1);
			
			
		}
	}
}




BOOL UnitData::CanPerformSpaceTransitionNow() const
{
	if(g_theUnitDB->Get(m_type)->GetCargoPod()) {
		if(Flag(k_UDF_ALREADY_PERFORMED_SPACE_TRANSITION))
			return FALSE;
		else
			return TRUE;
	}

	return TRUE;
}

void UnitData::IndicateSpaceTransition()
{
	SetFlag(k_UDF_ALREADY_PERFORMED_SPACE_TRANSITION);
}

void UnitData::AddWonderHPBonus(sint32 amt)
{
	m_hp += amt;
}

sint32 UnitData::CanPlantNuke(const MapPoint &pos) const 
{
	if (!g_theUnitDB->Get(m_type)->GetPlantNuke())
		return false;

	if (!m_pos.IsNextTo(pos))
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;
	return true;
}

sint32 UnitData::CanMakePark(const MapPoint &pos) const 
{
	if (!g_theUnitDB->Get(m_type)->GetCreateParks())
		return false;

	if (!m_pos.IsNextTo(pos))
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;
	return true;
}

sint32 UnitData::CanUndergroundRailway(const MapPoint &pos) const 
{
	if (!g_theUnitDB->Get(m_type)->GetUndergroundRailway())
		return false;

	if (!m_pos.IsNextTo(pos))
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;
	return true;
}

// Returns true if this unit is next to pos and can convert a city there
sint32 UnitData::CanConvert(const MapPoint &pos) const 
{
	if (!g_theUnitDB->Get(m_type)->GetConvertCities())
		return false;

	if (!m_pos.IsNextTo(pos))
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;
	return true;
}


sint32 UnitData::CanEstablishEmbassy(const MapPoint &pos) const 
{
	if (!g_theUnitDB->Get(m_type)->GetEstablishEmbassy())
		return false;

	if (!m_pos.IsNextTo(pos))
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;
	return true;
}

sint32 UnitData::CanCreateFranchise(const MapPoint &pos) const 
{
	if (!g_theUnitDB->Get(m_type)->GetCreateFranchise())
		return false;

	if (!m_pos.IsNextTo(pos))
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;
	return true;
}

sint32 UnitData::CanAssasinateRuler(const MapPoint &pos) const 
{
	if (!g_theUnitDB->Get(m_type)->GetAssasinateRuler())
		return false;

	if (!m_pos.IsNextTo(pos))
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;
	return true;
}

sint32 UnitData::CanStealTechnology(const MapPoint &pos) const 
{
	if (!g_theUnitDB->Get(m_type)->GetStealTechnology())
		return false;

	if (!m_pos.IsNextTo(pos))
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;

	return true;
}

sint32 UnitData::CanInjoin(const MapPoint &pos) const 
{
	if (!g_theUnitDB->Get(m_type)->GetCanInjoin())
		return false;

	if (!m_pos.IsNextTo(pos))
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;
	return true;
}

sint32 UnitData::CanInciteRevolution(const MapPoint &pos) const 
{
	if (!g_theUnitDB->Get(m_type)->GetInciteRevolution())
		return false;

	if (!m_pos.IsNextTo(pos))
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;
	return true;
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::CanCauseUnhappiness
//
// Description: Test if this unit can cause unhappiness.
//
// Parameters : MapPoint &pos
// 
//
// Globals    : -
//
// Returns    : true if this unit is in position to cause unhappiness at MapPoint &pos right now.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 UnitData::CanCauseUnhappiness(const MapPoint &pos) const 
{
	if (!g_theUnitDB->Get(m_type)->GetCauseUnhappiness())
		return false;

	if (!m_pos.IsNextTo(pos))
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;
	return true;
}

sint32 UnitData::CanExpel(const MapPoint &pos) const 
{
	if (g_theUnitDB->Get(m_type)->GetAttack() <= 0)
		return false;

	if (!m_pos.IsNextTo(pos))
		return false;

	if (GetMovementPoints() < g_theConstDB->SpecialActionMoveCost())
		return false;
	return true;
}

void UnitData::AddEndGameObject(sint32 type)
{
	Assert(m_city_data);
	if(m_city_data) {
		m_city_data->AddEndGameObject(type);
	}
}

BOOL UnitData::SendSlaveTo(Unit &dest)
{
	Assert(m_city_data);
	if(m_city_data) {
		return m_city_data->SendSlaveTo(dest);
	}
	return FALSE;
}

void UnitData::SetFullHappinessTurns(sint32 turns)
{
	Assert(m_city_data);
	if(m_city_data) {
		m_city_data->SetFullHappinessTurns(turns);
	}
}

void UnitData::RemoveUnitVision()
{
	Assert(Flag(k_UDF_VISION_ADDED));
	if(Flag(k_UDF_VISION_ADDED)) {
		g_player[m_owner]->RemoveUnitVision(m_pos, (GetVisionRange()));
		ClearFlag(k_UDF_VISION_ADDED);

		if (m_owner == g_selected_item->GetVisiblePlayer())
			g_director->AddRemoveVision(m_pos, (GetVisionRange()));
	}
}

void UnitData::RemoveOldUnitVision(double oldRadius)
{
	Assert(Flag(k_UDF_VISION_ADDED));
	if(Flag(k_UDF_VISION_ADDED)) {
		g_player[m_owner]->RemoveUnitVision(m_pos, oldRadius);
		ClearFlag(k_UDF_VISION_ADDED);

		if (m_owner == g_selected_item->GetVisiblePlayer())
			g_director->AddRemoveVision(m_pos, oldRadius);
	}
}

void UnitData::AddUnitVision(bool &revealed)
{
	Assert(!Flag(k_UDF_VISION_ADDED));
	if(!Flag(k_UDF_VISION_ADDED)) {
		double radius;
		if(m_city_data) {
			radius = m_city_data->GetVisionRadius();
		} else {
			radius = GetVisionRange();
		}
		g_player[m_owner]->AddUnitVision(m_pos, radius, revealed);
		SetFlag(k_UDF_VISION_ADDED);
		
		if (m_owner == g_selected_item->GetVisiblePlayer())
			g_director->AddAddVision(m_pos, radius);
	}
}

sint32 UnitData::GetIncomingTrade() const
{
	Assert(m_city_data);
	if(m_city_data) {
		return m_city_data->GetIncomingTrade();
	}
	return 0;
}

sint32 UnitData::GetOutgoingTrade() const
{
	Assert(m_city_data);
	if(m_city_data) {
		return m_city_data->GetOutgoingTrade();
	}
	return 0;
}

//----------------------------------------------------------------------------
//
// Name       : UnitData::CheckForRefuel
//
// Description: Test if this unit is in position to be refueled. If so, refuel it
//              and zero it's movement points.
//
// Parameters : -
// 
//
// Globals    : -
//
// Returns    : TRUE if unit can be and has been refueled.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
BOOL UnitData::CheckForRefuel()
{

	//	Cell *cell = g_theWorld->GetCell(m_pos);
	//sint32 CellOwner = cell->GetOwner();

	const UnitRecord *rec = g_theUnitDB->Get(m_type);
	if (IsBeingTransported()) { 
		m_fuel = rec->GetMaxFuel();
		m_movement_points = 0;
		return TRUE;
	}
	
	Unit c = g_theWorld->GetCity(m_pos);	
	if (c.m_id != (0)) { 
		m_fuel = rec->GetMaxFuel();
		m_movement_points = 0;
		return TRUE;
	}
	
	if(g_theWorld->IsInstallation(m_pos)) {
		if( terrainutil_HasAirfield(m_pos) &&
			g_theWorld->GetOwner(m_pos) == m_owner) { //add (!IsEnemy(CellOwner) || if((!AgreementMatrix::s_agreements.HasAgreement(defense_owner, m_owner, PROPOSAL_TREATY_DECLARE_WAR))
			m_fuel = rec->GetMaxFuel();
			m_movement_points = 0;
			return TRUE;
		}
	}
	return FALSE;
}

void UnitData::FinishBuilding()
{
	Assert(m_city_data);
	if(m_city_data) {
		m_city_data->FinishBuilding();
	}
}

void UnitData::DestroyRandomBuilding()
{
	Assert(m_city_data);
	if(m_city_data) {
		m_city_data->DestroyRandomBuilding();
	}
}

void UnitData::AddConversionUnhappiness(sint32 who)
{
	Assert(m_city_data);
	if(m_city_data) {
		m_city_data->AddConversionUnhappiness(who);
	}
}

void UnitData::BuildInfrastructure()
{
	Assert(m_city_data);
	if(m_city_data) {
		m_city_data->BuildInfrastructure();
	}
}

void UnitData::BuildCapitalization()
{
	Assert(m_city_data);
	if(m_city_data) {
		m_city_data->BuildCapitalization();
	}
}

void UnitData::ActionSuccessful(SPECATTACK attack, const Unit &c)
{
	sint32		soundID, spriteID;

	const SpecialAttackInfoRecord *rec = unitutil_GetSpecialAttack(attack);
	soundID = rec->GetSoundIDIndex();
	spriteID = rec->GetSpriteID()->GetValue();

	if (spriteID != -1 && soundID != -1) {
		g_director->AddSpecialAttack(m_actor->GetUnitID(), c, attack);
	} else {
		if (soundID != -1) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == m_owner) || 
				(m_visibility & (1 << visiblePlayer))) {

				g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 	soundID, m_pos.x, m_pos.y);
			}
		}
	}
}

void UnitData::ActionUnsuccessful(void)
{
	sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
	if ((visiblePlayer == m_owner) || 
		(m_visibility & (1 << visiblePlayer))) {

		g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
							gamesounds_GetGameSoundID(GAMESOUNDS_DEFAULT_FAIL),
							m_pos.x,
							m_pos.y);
	}
}


const Unit &UnitData::GetTransport() const
{
	return m_transport;
}

void UnitData::RemoveTransportedUnit(const Unit &u)
{
	Assert(m_cargo_list);
	if(m_cargo_list) {
		sint32 r = m_cargo_list->Del(u);
		Assert(r); 
	}
}

BOOL UnitData::BuildEndGame(sint32 type)
{
	





	return FALSE;
}

void UnitData::DisbandCity()
{
	Assert(m_city_data);
	if(m_city_data) {
		m_city_data->Disband();
	}
}

BOOL UnitData::CanPerformSpecialAction() const
{
	if(GetFirstMoveThisTurn())
		return TRUE;

	if(m_movement_points < g_theConstDB->SpecialActionMoveCost())
		return FALSE;

	if(Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN))
		return FALSE;

	if(Flag(k_UDF_FOUGHT_THIS_TURN))
		return FALSE;
										
	return TRUE;
}

void UnitData::AddGoods(SlicObject *obj)
{
	Assert(m_city_data);
	if(m_city_data) {
		m_city_data->AddGoods(obj);
	}
}

void UnitData::UpdateZOCForInsertion()
{
	Unit me(m_id);
	if(!me.IsNoZoc() && !IsTempSlaveUnit()) {
		MapPoint chk;

		g_theWorld->AddZOC(m_pos, m_owner);
		uint32 moveType = g_theUnitDB->Get(m_type)->GetMovementType();
//		BOOL isCity = g_theUnitDB->Get(m_type)->GetHasPopAndCanBuild();
		for (sint32 dd = (sint32)NORTH; dd < (sint32)NOWHERE; dd++) {
			if(m_pos.GetNeighborPosition((WORLD_DIRECTION)dd, chk)) {
				if(g_theWorld->CanEnter(chk, moveType)) {
					g_theWorld->AddZOC(chk, m_owner);
				}
			}
		}
	}
}

double UnitData::GetVisionRange() const
{
	if(m_city_data) {
		return m_city_data->GetVisionRadius();
	} else {
		return g_theUnitDB->Get(m_type)->GetVisionRange();
	}
}

void UnitData::CheckVisionRadius()
{
#if 0
	if(m_vision_range != g_theUnitDB->Get(m_type)->m_vision_range) {
		BOOL add = FALSE;
		if(Flag(k_UDF_VISION_ADDED)) {
			RemoveUnitVision();
			add = TRUE;

		}
		m_vision_range = g_theUnitDB->Get(m_type)->m_vision_range;

		if(add) {
			AddUnitVision();
		}
	}
#endif
}

void UnitData::SetTargetCity(const Unit &city)
{
	m_target_city = city;
	if(m_target_city.IsValid()) {
		SetFlag(k_UDF_HAS_TARGET);
	} else {
		ClearFlag(k_UDF_HAS_TARGET);
	}
}

const Unit &UnitData::GetTargetCity()
{
	return m_target_city;
}

bool UnitData::CanBeachAssaultRightNow()
{
	if(!m_transport.IsValid()) {
		
		return false;
	}

	if(g_theUnitDB->Get(m_type)->GetParatrooper()) {
		if(m_transport.GetDBRec()->GetParatrooperTransport())
			return true;
		return false;
	}

	if(g_theUnitDB->Get(m_type)->GetCanBeachAssault()) {
		if(m_transport.GetDBRec()->GetMovementTypeSea() ||
		   m_transport.GetDBRec()->GetMovementTypeShallowWater())
			return true;
		return false;
	}

	return false;
}


