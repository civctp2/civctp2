//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Army data handling
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Fix movement cost of ships above tunnels.
// - Center on pirating (originally by Ahenobarb, slightly modified).
// - Center on bombarding.
// - Fix sueing of franchises.
// - #01 Inform AI only about bombard if it was really possible (L. Hirth 6/2004).
// - #02 TestOrderAny added.
// - Ambiguous sqrt calls resolved.
// - Standardised min/max usage.
// - Add CanTransport method - Calvitix
// - Added IsWounded method - Calvitix
//----------------------------------------------------------------------------

#include "c3.h"
#include "ArmyData.h"

#include "UnitPool.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Player.h"
#include "Director.h"
#include "CivPaths.h"
#include "SelItem.h"
#include "installationtree.h"

#include "terrainutil.h"
#include "installation.h"
#include "RandGen.h"
#include "Cell.h"
#include "Agreement.h"
#include "QuadTree.h"
#include "UnitData.h"
#include "ConstDB.h"
#include "UnitAstar.h"
#include "AiCause.h"

#include "GoodyHuts.h"
#include "SlicSegment.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "QuickSlic.h"
#include "TiledMap.h"
#include "TurnCnt.h"
#include "HappyTracker.h"

#include "CTP2Combat.h"

#include "TradePool.h"
#include "UnitActor.h"
#include "ArmyPool.h"
#include "UnitPool.h"
#include "Advances.h"
#include "AdvanceRecord.h"
#include "StrDB.h"
#include "WonderRecord.h"
#include "TerrainRecord.h"
#include "SpriteStateDB.h"
#include "GameSettings.h"

#include "UnitRecord.h"
#include "SpecialAttackInfoRecord.h"

#include "network.h"
#include "net_order.h"
#include "net_action.h"
#include "net_info.h"

#include "SoundManager.h"
#include "GameSounds.h"
#include "Order.h"
#include "CellUnitList.h"

#include "CityAstar.h"
extern CityAstar g_city_astar;

#include "AgreementPool.h"
extern AgreementPool *g_theAgreementPool;

#include "Pollution.h"
extern Pollution *g_thePollution;

#include "GameEventManager.h"
#include "GameEventArgList.h"
#include "GameEventArgument.h"

#include "unitutil.h"
#include "advanceutil.h"

#include "MoveFlags.h"

#include "SpecialEffectRecord.h"
#include "wonderutil.h"

#include "CtpAi.h"
#include "OrderRecord.h"
#include "AgreementMatrix.h"

#include "SoundRecord.h"

#include "Squad.h"
#include "CTPAgent.h"
#include "CTPGoal.h"

#include "TradeRouteData.h"
#include "Gold.h"
#include "BuildingUtil.h"
#include "Diplomat.h"
#include "Radarmap.h"

BOOL g_smokingCrack = TRUE;
BOOL g_useOrderQueues = TRUE;

extern CivPaths *g_civPaths; 
extern RandomGenerator *g_rand ;
extern QuadTree<Unit> *g_theUnitTree;
extern UnitAstar *g_theUnitAstar;

extern SoundManager *g_soundManager;

#define N_F  (1 << NORTH)
#define NE_F (1 << NORTHEAST)
#define E_F  (1 << EAST)
#define SE_F (1 << SOUTHEAST)
#define S_F  (1 << SOUTH)
#define SW_F (1 << SOUTHWEST)
#define W_F  (1 << WEST)
#define NW_F (1 << NORTHWEST)


#include "ProfileDB.h"
#include "Diplomacy_Log.h"
extern Diplomacy_Log *g_theDiplomacyLog; 

#include "c3ui.h"
extern C3UI *g_c3ui;


sint32 *ArmyData::s_orderDBToEventMap = NULL;






ArmyData::ArmyData(const Army &army, const UnitDynamicArray &units) :
	GameObj(army.m_id)
{
    m_attackedByDefenders = NULL; 
	m_orders = NULL;
    m_killMeSoon = NULL; 
    m_tempKillList = NULL; 
    m_isTransported = FALSE; 
    m_revealedForeignUnits = FALSE; 
    m_zocViolation = FALSE; 
    m_didBattle = FALSE; 
    m_hasBeenAdded = FALSE; 
    m_dontKillCount = 0;
	m_needToKill = 0 ;
    m_removeCause = CAUSE_REMOVE_ARMY(0);
	m_isPirating = false;
	m_name = NULL;

	m_owner = -1;
	m_killer = -1;
	sint32 i;
	for(i = 0; i < units.Num(); i++) {
		Insert(units.Get(i));
	}

	m_attackedByDefenders = new UnitDynamicArray;
	m_orders = new PointerList<Order>;

	m_dontKillCount = 0;
	m_needToKill = FALSE;
	m_hasBeenAdded = FALSE;

	m_killMeSoon = new PointerList<KillRecord>;


	
	m_debugString = NULL;
	m_debugStringColor = 0;

	
	
	
	

}

ArmyData::ArmyData(const Army &army, const CellUnitList &units) :
	GameObj(army.m_id)
{
    m_attackedByDefenders = NULL; 
	m_orders = NULL;
    m_killMeSoon = NULL; 
    m_tempKillList = NULL; 
    m_isTransported = FALSE; 
    m_revealedForeignUnits = FALSE; 
    m_zocViolation = FALSE; 
    m_didBattle = FALSE; 
    m_hasBeenAdded = FALSE; 
    m_dontKillCount = 0;
	m_needToKill = 0 ;
    m_removeCause = CAUSE_REMOVE_ARMY(0);
	m_isPirating = false;
	m_name = NULL;

	m_owner = -1;
	m_killer = -1;
	sint32 i;
	for(i = 0; i < units.Num(); i++) {
		Insert(units.Get(i));
	}

	m_attackedByDefenders = new UnitDynamicArray;
	m_orders = new PointerList<Order>;

	m_dontKillCount = 0;
	m_needToKill = FALSE;
	m_hasBeenAdded = FALSE;

	m_killMeSoon = new PointerList<KillRecord>;

	
	m_debugString = NULL;
	m_debugStringColor = 0;

	
	
	
	

}

ArmyData::ArmyData(const Army &army, Unit &u) :
	GameObj(army.m_id)
{
    m_attackedByDefenders = NULL; 
	m_orders = NULL;
    m_killMeSoon = NULL; 
    m_tempKillList = NULL; 
    m_isTransported = FALSE; 
    m_revealedForeignUnits = FALSE; 
    m_zocViolation = FALSE; 
    m_didBattle = FALSE; 
    m_hasBeenAdded = FALSE; 
    m_dontKillCount = 0;
	m_needToKill = 0 ;
    m_removeCause = CAUSE_REMOVE_ARMY(0);
	m_isPirating = false;
	m_name = NULL;

	m_owner = -1;
	m_killer = -1;
	Insert(u);
	m_attackedByDefenders = new UnitDynamicArray;
	m_orders = new PointerList<Order>;
	m_dontKillCount = 0;
	m_needToKill = FALSE;
	m_hasBeenAdded = FALSE;
	m_killMeSoon = new PointerList<KillRecord>;

   	
	m_debugString = NULL;
	m_debugStringColor = 0;

	
	
	
	

}

ArmyData::ArmyData(const Army &army) :
	GameObj(army.m_id)
{
    m_attackedByDefenders = NULL; 
	m_orders = NULL;
    m_killMeSoon = NULL; 
    m_tempKillList = NULL; 
    m_isTransported = FALSE; 
    m_revealedForeignUnits = FALSE; 
    m_zocViolation = FALSE; 
    m_didBattle = FALSE; 
    m_hasBeenAdded = FALSE; 
    m_dontKillCount = 0;
	m_needToKill = 0 ;
    m_removeCause = CAUSE_REMOVE_ARMY(0);
	m_isPirating = false;
	m_name = NULL;

	m_owner = -1;
	m_killer = -1;
	m_attackedByDefenders = new UnitDynamicArray;
	m_orders = new PointerList<Order>;
	m_dontKillCount = 0;
	m_needToKill = FALSE;
	m_hasBeenAdded = FALSE;
	m_killMeSoon = new PointerList<KillRecord>;

	
	m_debugString = NULL;
	m_debugStringColor = 0;
	
	

	
	
	
	
}

ArmyData::ArmyData(CivArchive &archive) :
	GameObj(0)
{
    m_attackedByDefenders = NULL; 
	m_orders = NULL;
    m_killMeSoon = NULL; 
    m_tempKillList = NULL; 
    m_isTransported = FALSE; 
    m_revealedForeignUnits = FALSE; 
    m_zocViolation = FALSE; 
    m_didBattle = FALSE; 
    m_hasBeenAdded = FALSE; 
    m_dontKillCount = 0;
	m_needToKill = 0 ;
    m_removeCause = CAUSE_REMOVE_ARMY(0);
	m_isPirating = false;
	m_name = NULL;

	m_attackedByDefenders = new UnitDynamicArray;
	m_orders = new PointerList<Order>;
	m_killMeSoon = new PointerList<KillRecord>;

	
	m_debugString = NULL;
	m_debugStringColor = 0;

	Serialize(archive);
}

ArmyData::~ArmyData()
{
	delete m_orders;
	delete m_attackedByDefenders;
	delete m_killMeSoon;

	
	if (m_debugString) delete m_debugString;
	if (m_name)	delete m_name;
}

void ArmyData::Serialize(CivArchive &archive)
{
	GAMEOBJ::Serialize(archive);
	CellUnitList::Serialize(archive);
	m_attackedByDefenders->Serialize(archive);
	m_pos.Serialize(archive);

	sint32 ocount, i;
	uint8 hasChild;

	if(archive.IsStoring()) {
		archive << m_owner;
		archive << m_killer;
		archive.PutSINT32(m_removeCause);
		archive.PutUINT8(m_dontKillCount);
		archive.PutUINT8(m_needToKill);
		archive.PutUINT8(m_hasBeenAdded);
		archive.PutUINT8(m_isPirating);

		ocount = m_orders->GetCount();
		archive << ocount;
		PointerList<Order>::Walker walk(m_orders);
		while(walk.IsValid()) {
			walk.GetObj()->Serialize(archive);
			walk.Next();
		}

		sint32 len = m_name ? strlen(m_name) : 0;
		archive << len;
		if(len > 0) {
			archive.Store((uint8*)m_name, len);
		}

		hasChild = m_lesser != NULL;
		archive << hasChild;
		if(hasChild) {
			m_lesser->Serialize(archive);
		}

		hasChild = m_greater != NULL;
		archive << hasChild;
		if(hasChild) {
			m_greater->Serialize(archive);
		}

	} else {
		archive >> m_owner;
		archive >> m_killer;
		m_removeCause = (CAUSE_REMOVE_ARMY)archive.GetSINT32();
		m_dontKillCount = (BOOL)archive.GetUINT8();
		m_needToKill = (BOOL)archive.GetUINT8();
		m_hasBeenAdded = (BOOL)archive.GetUINT8();
		m_isPirating = archive.GetUINT8() != 0;

		archive >> ocount;
		for(i = 0; i < ocount; i++) {
			Order *newOrder = new Order(archive);
			m_orders->AddTail(newOrder);
		}

		sint32 len;
		archive >> len;
		if(len <= 0)
			m_name = NULL;
		else {
			m_name = new MBCHAR[(len + 1) * sizeof(MBCHAR)];
			archive.Load((uint8*)m_name, len);
			m_name[len] = 0;
		}

		archive >> hasChild;
		if(hasChild) {
			m_lesser = new ArmyData(archive);
		} else {
			m_lesser = NULL;
		}

		archive >> hasChild;
		if(hasChild) {
			m_greater = new ArmyData(archive);
		} else {
			m_greater = NULL;
		}
	}
}

void ArmyData::SetOwner(PLAYER_INDEX p)
{
	Assert(m_nElements <= 0);
	m_owner = p;
}

void ArmyData::SetRemoveCause(CAUSE_REMOVE_ARMY cause)
{
	m_removeCause = cause;
}

CAUSE_REMOVE_ARMY ArmyData::GetRemoveCause() const
{
	return m_removeCause;
}




BOOL ArmyData::Insert(const Unit &id)
{
	Assert(m_nElements < k_MAX_ARMY_SIZE);
	if(m_nElements >= k_MAX_ARMY_SIZE)
		return FALSE;

	if(m_nElements > 0) {
		MapPoint hisPos;
		id.GetPos(hisPos);
		Assert(m_pos == hisPos);
		if(m_pos != hisPos)
			return FALSE;
		
		Assert(m_array[0].GetOwner() == id.GetOwner());
		if(m_array[0].GetOwner() != id.GetOwner())
			return FALSE;

	} else {
		m_owner = id.GetOwner();
		id.GetPos(m_pos);
	}

	CellUnitList::Insert(id);
	Unit u(id);
	u.SetArmy(Army(m_id));

	
	

















	return TRUE;
}

uint32 ArmyData::GetMovementType() const
{
    int i; 
    uint32 tmp = 0xffffffff; 
    
    for (i=0; i<m_nElements; i++) { 
        tmp &= m_array[i].GetMovementType(); 
    }
    
    return tmp; 
}

uint32 ArmyData::GetCargoMovementType() const
{
    int i; 
	int j;
    uint32 tmp = 0xffffffff; 
	const UnitDynamicArray *cargo;
    
    for (i=0; i<m_nElements; i++) 
	{ 
		cargo = m_array[i].AccessData()->GetCargoList();
		for(j = 0; j < cargo->Num(); j++) {
			tmp &= cargo->Access(j).GetMovementType();
		}
	}
   
    return tmp; 
}

BOOL ArmyData::HasCargo() const
{
    int i; 
	const UnitDynamicArray *cargo;
    
    for (i=0; i<m_nElements; i++) 
	{ 
		cargo = m_array[i].AccessData()->GetCargoList();
		if (cargo->Num() > 0)
			return TRUE;
	}
   
    return FALSE; 
}

BOOL ArmyData::GetCargo(sint32 &transports, sint32 &max, sint32 &empty) const
{
    int i; 
	const UnitDynamicArray *cargo;
	transports = 0;
	max = 0;
	empty = 0;
	sint32 used = 0;
	sint32 tmp;
    
    for (i=0; i<m_nElements; i++) 
	{ 
		cargo = m_array[i].AccessData()->GetCargoList();
		used += cargo->Num();
		
		if (g_theUnitDB->Get(m_array[i].GetType())->GetCargoData())
			tmp = g_theUnitDB->Get(m_array[i].GetType())->GetCargoDataPtr()->GetMaxCargo();
		else
			tmp = 0;
		max += tmp;
		if (tmp > 0)
			transports++;
	}
	Assert( max >= used );
	Assert( max >= transports);
	empty = max - used;
	
	return (transports > 0);
}	
	
BOOL ArmyData::CargoCanEnter(const MapPoint &pos) const
{
    int i; 
	int j;
	const UnitDynamicArray *cargo;
	Cell *cell = g_theWorld->GetCell(pos);
    
    for (i=0; i<m_nElements; i++) 
	{ 
		cargo = m_array[i].AccessData()->GetCargoList();
		for(j = 0; j < cargo->Num(); j++) {
			if (cell->CanEnter(cargo->Access(j).GetMovementType())) {
				return TRUE;
			}
		}
	}
   
    return FALSE; 
}

sint16 ArmyData::CountMovementTypeSea() const
{
	sint16 i;
	sint16 count = 0;
	for(i = 0; i < m_nElements; i++)
		if(m_array[i].GetMovementTypeSea())
			count++;
	return count;
}

BOOL ArmyData::CanSettle(const MapPoint &pos) const
{
    sint32 i; 
    
    for (i=0; i<m_nElements; i++) { 
        if (m_array[i].CanSettle(pos) == TRUE)
            return TRUE; 
    } 
    return FALSE; 
}

BOOL ArmyData::CanSettle() const
{
    for(sint32 i = 0; i < m_nElements; i++) {
		Assert(g_theUnitPool->IsValid(m_array[i]));
        if( g_theUnitPool->IsValid(m_array[i]) && g_theUnitDB->Get(m_array[i].GetType())->GetSettle() &&
			m_array[i].CanPerformSpecialAction())
            return TRUE;
    }
    return FALSE;
}

#if !defined (ACTIVISION_ORIGINAL)
BOOL ArmyData::CanTransport() const
{
    int i;
    long cargo;

    for (i = 0; i < m_nElements; i++)
    {
        cargo = m_array[i].AccessData()->GetCargoCapacity();
        if (cargo > 0)
            return TRUE;
    }

    return FALSE;
}
#endif

BOOL ArmyData::CanPatrol() const
{
    for(sint32 i = 0; i < m_nElements; i++) {
        if(g_theUnitDB->Get(m_array[i].GetType())->GetCanPatrol())
            return TRUE;
    }
    return FALSE;
}

BOOL ArmyData::IsAsleep() const
{
	return m_array[0].IsAsleep();
}

void ArmyData::Sleep()
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SleepUnit,
							   GEA_Unit, m_array[i],
							   GEA_End);
		
	}
}

void ArmyData::WakeUp()
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_WakeUnit,
							   GEA_Unit, m_array[i],
							   GEA_End);
		
	}
}

BOOL ArmyData::IsEntrenched() const
{
	return m_array[0].IsEntrenched();
}

BOOL ArmyData::IsEntrenching() const
{
	return m_array[0].IsEntrenching();
}

BOOL ArmyData::CanEntrench() const
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(!m_array[i].CanEntrench())
			return FALSE;
	}
	return TRUE;
}

void ArmyData::Entrench()
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_EntrenchUnit,
							   GEA_Unit, m_array[i],
							   GEA_End);
		
	}
}

void ArmyData::Detrench()
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_DetrenchUnit,
							   GEA_Unit, m_array[i],
							   GEA_End);
		
	}
}


BOOL ArmyData::IsPatrolling() const
{
	return FALSE;
}



void ArmyData::GetActors(Unit &excludeMe, UnitActor **restOfStack)
{
    sint32			i; 
    UnitActor		*a; 
    sint32			n = 0;

	for (i=0; i<m_nElements; i++) { 
		a = m_array[i].GetActor();
		if (a != excludeMe.GetActor()) {
			restOfStack[n++] =  a;
		}
	}
}

void ArmyData::GroupArmy(Army &army)
{
	sint32 i;
	DPRINTF(k_DBG_GAMESTATE, ("Army %lx grouping army %lx",
							  m_id, army));

	army.SetRemoveCause(CAUSE_REMOVE_ARMY_GROUPING);
	bool atLeastOneAsleep = false;
	bool atLeastOneNotEntrenched = false;

	for(i = army.Num() - 1; i >= 0; i--) {
		DPRINTF(k_DBG_GAMESTATE, ("Inserting unit %lx\n", army[i]));       
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddUnitToArmy,
							   GEA_Unit, army[i],
							   GEA_Army, m_id,
							   GEA_End);
		if(army[i].Flag(k_UDF_IS_ASLEEP)) {
			atLeastOneAsleep = true;
		}
		
		
	}

	
	
	
	
	

	if(IsAsleep() || atLeastOneAsleep) {
		WakeUp();
	}

	for(i = 0; i < m_nElements; i++) {
		if((m_array[i].IsEntrenching() || m_array[i].IsEntrenched()) && atLeastOneNotEntrenched) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_EntrenchOrder,
								   GEA_Army, m_id,
								   GEA_End);
			
			break;
		}
	}

	
	g_player[m_owner]->RefreshAIArmyReference(Army(m_id));
}


void ArmyData::GroupAllUnits()
{
	DPRINTF(k_DBG_GAMESTATE, ("Army %lx grouping everyone\n", m_id));
	Cell *cell = g_theWorld->GetCell(m_pos);

	CellUnitList *ul = cell->UnitArmy();

	sint32 i;
	if(ul) {
		for(i = 0; i < ul->Num(); i++) {
			if(ul->Access(i).GetArmy() != Army(m_id)) {
				if(!ul->Access(i).GetArmy().IsValid()) {
					if(g_network.IsClient()) {
						g_network.RequestResync(RESYNC_INVALID_ARMY_OTHER);
						return;
					}
				}
				ul->Access(i).GetArmy().SetRemoveCause(CAUSE_REMOVE_ARMY_GROUPING);
				ul->Access(i).ChangeArmy(Army(m_id), CAUSE_NEW_ARMY_GROUPING);
				DPRINTF(k_DBG_GAMESTATE, ("Grouped unit %lx\n",
										  ul->Access(i)));
			}
		}
	}

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_WakeArmy,
						   GEA_Army, m_id,
						   GEA_End);
	WakeUp();
	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].IsEntrenching() || m_array[i].IsEntrenched()) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_EntrenchOrder,
								   GEA_Army, m_id,
								   GEA_End);
			
			break;
		}
	}

	
	
}
	
void ArmyData::GroupUnit(Unit &unit)
{
	DPRINTF(k_DBG_GAMESTATE, ("Army %lx grouping unit %lx\n", m_id, unit));

	
	if(!g_theUnitPool->IsValid(unit)) {
		if(g_network.IsHost()) {
			Assert(!g_network.IsLocalPlayer(m_owner));
			if(!g_network.IsLocalPlayer(m_owner)) {
				g_network.Resync(m_owner);
				return;
			}
		} else if(g_network.IsClient()) {
			g_network.RequestResync(RESYNC_INVALID_UNIT);
			return;
		}
		Assert(g_theUnitPool->IsValid(unit));
		return;
	}

	MapPoint upos;
	unit.GetPos(upos);
	if(m_nElements == 0) {
		m_pos = upos;
	}
	

	if(upos == m_pos) {
		
		
		
		if(unit.GetArmy().IsValid())
		{
			unit.GetArmy().SetRemoveCause(CAUSE_REMOVE_ARMY_GROUPING);
		}
		else
		{
			Assert(unit.GetArmy().IsValid());
		}

		
		g_player[m_owner]->
			RemoveUnitReferenceFromPlayer(unit, CAUSE_REMOVE_ARMY_GROUPING, m_owner);

		unit.ChangeArmy(Army(m_id), CAUSE_NEW_ARMY_GROUPING);

	}

	
	
	
	
	if(IsAsleep() || unit.Flag(k_UDF_IS_ASLEEP)) {
		WakeUp();
	}

	if(!unit.IsEntrenched() && !unit.IsEntrenching()) {
		sint32 i;
		for(i = 0; i < m_nElements; i++) {
			if(m_array[i].IsEntrenching() || m_array[i].IsEntrenched()) {
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_EntrenchOrder,
					GEA_Army, m_id,
					GEA_End);
				
				break;
			}
		}
	}

	
	
}

void ArmyData::UngroupUnits()
{
	sint32 i;
	for(i = m_nElements - 1; i > 0; i--) {
		
		

		Army newArmy = g_player[m_owner]->GetNewArmy(CAUSE_NEW_ARMY_UNGROUPING_ORDER);
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddUnitToArmy,
							   GEA_Unit, m_array[i],
							   GEA_Army, newArmy,
							   GEA_Int, CAUSE_NEW_ARMY_UNGROUPING_ORDER,
							   GEA_End);
	}
}
		
void ArmyData::CityRadiusFunc(const MapPoint &pos)
{
	if(m_tempKillList) {
		Cell *cell = g_theWorld->GetCell(pos);
		if(cell->UnitArmy()) {
			sint32 i;
			for(i = 0; i < cell->UnitArmy()->Num(); i++) {
				m_tempKillList->Insert(cell->UnitArmy()->Access(i));
			}
		}
	}
	g_theWorld->CutImprovements(pos);
}


void ArmyData::GetActiveDefenders(UnitDynamicArray &input, 
								  UnitDynamicArray &output,
								  BOOL isCargoPodCheck)
{
	sint32 i, j, n = input.Num();
	sint32 numDefenders = 0;
	sint32 owner = m_array[0].GetOwner();
	output.Clear();
	
	for(i = 0; i < n; i++) {
		
		if(input[i].GetOwner() == owner)
			continue;
		if(!input[i].CanActivelyDefend(Army(m_id))) {
			if(!(isCargoPodCheck &&
				 input[i].GetDBRec()->GetDefendAir()))
				continue;
		}
		
		
		
		if (!AgreementMatrix::s_agreements.HasAgreement(input[i].GetOwner(), owner, PROPOSAL_TREATY_DECLARE_WAR))
			continue;

		if(input[i].Flag(k_UDF_USED_ACTIVE_DEFENSE))
			continue;

		double hisr = input[i].GetActiveDefenseRange();
		sint32 hisrsq = sint32((hisr + 0.5) * (hisr + 0.5));

		if(UnitData::GetDistance(input[i].AccessData(), m_array[0].AccessData(),
								 sint32(2 )) > hisrsq) {
			continue;
		}

		for(j = 0; j < numDefenders; j++) {
			if(input[i].GetAttack() > output[j].GetAttack()) {
				output.InsertBefore(input[i], j);
				numDefenders ++;
				break;
			}
		}
		if(j >= numDefenders) {
			output.Insert(input[i]);
			numDefenders++;
		}
	}
}

BOOL
ArmyData::CheckActiveDefenders(MapPoint &pos, BOOL cargoPodCheck)
{
	if(m_nElements <= 0)
		return FALSE;

	
	if (m_nElements == 1 && 
		g_theUnitDB->Get(m_array[0].GetType())->GetNuclearAttack())
		return FALSE;

	MapPoint topleft = pos;
	double maxActiveDefenseRange = unitutil_MaxActiveDefenseRange();
	topleft.x -= sint16(maxActiveDefenseRange);
	
	static UnitDynamicArray possibleDefenders;
	static UnitDynamicArray activeDefenders;
	static UnitDynamicArray deadDefenders;
	
	possibleDefenders.Clear();
	deadDefenders.Clear();
	
	g_theUnitTree->SearchRect(possibleDefenders, topleft,
							  (sint32)maxActiveDefenseRange * 2 + 1,
							  (sint32)maxActiveDefenseRange * 2 + 1,
							  ~(1 << m_array[0].GetOwner()));
	if(possibleDefenders.Num() <= 0)
		return FALSE;
	
	GetActiveDefenders(possibleDefenders, activeDefenders, cargoPodCheck);
	sint32 defenderOwner = -1;
	sint32 owner = GetOwner();
	
	sint32 i, j, n = activeDefenders.Num();
	for(i = 0; i < n; i++) {
		if(!activeDefenders[i].CanSee(Army(m_id)))
			continue;
		
		MapPoint dpos;
		activeDefenders[i].GetPos(dpos);

		Unit ta = activeDefenders[i];
		Unit td = GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());
		
		

		g_director->AddAttackPos(ta, m_pos);
		
		g_slicEngine->RunActiveDefenseTriggers(ta, td);

		sint32 numAttacks = activeDefenders[i].CanActivelyDefend(Army(m_id));
		for(j = 0; j < numAttacks; j++) {
			activeDefenders[i].Bombard(*this, FALSE);
		}
		activeDefenders[i].SetFlag(k_UDF_USED_ACTIVE_DEFENSE);

		defenderOwner = activeDefenders[i].GetOwner();
		
		Cell *dcell = g_theWorld->GetCell(dpos);

		for(j = 0; j < m_nElements; j++) {
			if(!(m_array[j].GetHP() < 1.0) && 
			   (m_array[j].CanCounterBombard(*dcell->UnitArmy()) ||
				m_array[j].CanActivelyDefend(*dcell->UnitArmy()))) {
				m_array[j].Bombard(*dcell->UnitArmy(), TRUE);
				for(sint32 k = 0; k < dcell->GetNumUnits(); k++) {
					if(dcell->AccessUnit(k).GetHP() < 1.0 &&
					   !deadDefenders.IsPresent(dcell->AccessUnit(k)))
						deadDefenders.Insert(dcell->AccessUnit(k));
				}
			}
		}
	}

	if(defenderOwner < 0)
		return FALSE;

	sint32 numDead = 0;
	for(i = m_nElements - 1; i >= 0; i--) {
		if(m_array[i].GetHP() < 1.0) {
            Assert(0 <= defenderOwner); 
            Assert(defenderOwner < k_MAX_PLAYERS); 
			m_array[i].Kill(CAUSE_REMOVE_ARMY_ACTIVE_DEFENSE, defenderOwner);
		}
	}

    Assert(0 <= owner); 
    Assert(owner < k_MAX_PLAYERS); 
	deadDefenders.KillList(CAUSE_REMOVE_ARMY_DIED_IN_ATTACK, owner);
	return TRUE;
}
	
void ArmyData::BeginTurn()
{
	m_flags &= ~(k_CULF_EXECUTED_THIS_TURN);
	if(m_isPirating) {
		Cell *cell = g_theWorld->GetCell(m_pos);
		Assert(cell);
		if(cell) {
			sint32 i;
			sint32 piratedByMe = 0;
			for(i = 0; i < cell->GetNumTradeRoutes(); i++) {
				TradeRoute route = cell->GetTradeRoute(i);
				if(route->GetPiratingArmy().m_id == m_id) {
					g_player[m_owner]->AddGold(route->GetValue() * g_theConstDB->GetPiracyWasteCoefficient());
					piratedByMe++;
				}
			}
			if(piratedByMe < 1) {
				StopPirating();
			}
		}
	}

	
	if (!g_player[m_owner]->m_can_use_sea_tab)
	{
		g_player[m_owner]->m_can_use_sea_tab |= (CountMovementTypeSea() > 0);
	}

	
	sint32 transports, max_slots, empty_slots;
	GetCargo(transports, max_slots, empty_slots);
	g_player[m_owner]->AddCargoCapacity(empty_slots);

	
	if(m_flags & k_CULF_IN_SPACE) {
		if(NewTurnCount::GetCurrentRound() >= m_reentryTurn) {
			if(g_network.IsHost()) {
				g_network.Block(m_owner);
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_REENTER, m_id));
				g_network.Unblock(m_owner);
			}
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_Reentry,
								   GEA_Army, m_id,
								   GEA_End);
		}
	}
}

BOOL ArmyData::CanFight(CellUnitList &defender)
{
	sint32 i, j;

	BOOL haveCombatUnits = FALSE;
	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetAttack() > 0) {
			haveCombatUnits = TRUE;
			break;
		}
	}
	if(!haveCombatUnits)
		return FALSE;

	MapPoint pos;
	defender.GetPos(pos);
	if(g_theWorld->IsCity(pos)) {
		return TRUE;
	}

	for(i = 0; i < m_nElements; i++) {
		for(j = 0; j < defender.Num(); j++) {
			const UnitRecord *rec = g_theUnitDB->Get(m_array[i].GetType());
			if(defender[j].IsSubmarine() && !rec->GetCanAttackUnderwater())
				continue;
			if(rec->GetCanAttack() & defender[j].GetDBRec()->GetMovementType())
				return TRUE;
			
			if(!defender[j].GetDBRec()->GetMovementTypeSea() && !defender[j].GetDBRec()->GetMovementTypeAir() &&
			   g_theWorld->IsWater(pos) && 
			   (rec->GetCanAttack() & 
				(k_BIT_MOVEMENT_TYPE_SHALLOW_WATER | k_BIT_MOVEMENT_TYPE_WATER))) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::InvestigateCity(const MapPoint &point)
{
	Unit c = GetAdjacentCity(point);
	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner) {
		return ORDER_RESULT_ILLEGAL;
	}

	UnitRecord::InvestigateCityData *values;

	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN))
			continue;

		if(g_theUnitDB->Get(m_array[i].GetType())->
		   GetInvestigateCity(values)) {
			AddSpecialActionUsed(m_array[i]);
			InformAI(UNIT_ORDER_INVESTIGATE_CITY, point); 

			return m_array[i].InvestigateCity(c);
		}
	}
	return ORDER_RESULT_ILLEGAL;
}

ORDER_RESULT ArmyData::NullifyWalls(const MapPoint &point)
{
	Unit c = GetAdjacentCity(point);
	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN))
			continue;
		if(g_theUnitDB->Get(m_array[i].GetType())->
		   GetNullifyCityWalls()) {
			AddSpecialActionUsed(m_array[i]);
			InformAI(UNIT_ORDER_NULLIFY_WALLS, point); 

			return m_array[i].NullifyWalls(c);
		}
	}
	return ORDER_RESULT_ILLEGAL;
}

ORDER_RESULT ArmyData::StealTechnology(const MapPoint &point)
{
	
	sint32 whichAdvance = -1;

	Unit c = GetAdjacentCity(point);
	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

	for(sint32 i = 0; i < m_nElements; i++) {
		if(g_theUnitDB->Get(m_array[i].GetType())->
		   GetStealTechnology()) {
			sint32 num;
			uint8 *canSteal = g_player[m_owner]->m_advances->CanAskFor(g_player[c.GetOwner()]->m_advances,
																	   num);
			if(num > 0) {
				
				if(g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT &&
				   (!g_network.IsClient() || !g_network.IsLocalPlayer(m_owner))) {
					return m_array[i].StealTechnology(c, -1);
				}

				
				SlicObject *so = new SlicObject("101StealWhichAdvance");
				so->AddRecipient(m_owner);
				so->AddUnit(m_array[i]);
				so->AddCity(c);
				so->AddCivilisation(c.GetOwner());
				sint32 j;
				for(j = 0; j < g_theAdvanceDB->NumRecords(); j++) {
					if(canSteal[j]) {
						so->AddAdvance(j);
					}
				}
				g_slicEngine->Execute(so);
				delete [] canSteal;
				return ORDER_RESULT_INCOMPLETE;
			} else {
				SlicObject *so = new SlicObject("102NoAdvancesToSteal");
				so->AddRecipient(m_owner);
				so->AddCivilisation(c.GetOwner());
				g_slicEngine->Execute(so);
				delete [] canSteal;
				return ORDER_RESULT_ILLEGAL;
			}
		}
	}
	return ORDER_RESULT_INCOMPLETE; 
}

ORDER_RESULT ArmyData::InciteRevolution(const MapPoint &point)
{
	Unit c = GetAdjacentCity(point);
	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

	sint32 cost;
	GetInciteRevolutionCost(point, cost);

	if(g_player[m_owner]->m_gold->GetLevel() < cost) {
		return ORDER_RESULT_ILLEGAL;
	}

	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN))
			continue;

		if(g_theUnitDB->Get(m_array[i].GetType())->
		   GetInciteRevolution()) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_InciteRevolutionUnit,
								   GEA_Unit, m_array[i].m_id,
								   GEA_City, c.m_id,
								   GEA_End);
			
			

			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SubGold,
								   GEA_Player, m_owner,
								   GEA_Int, cost,
								   GEA_End);

			AddSpecialActionUsed(m_array[i]);

			InformAI(UNIT_ORDER_INCITE_REVOLUTION, point); 

			
			

			
			
			return ORDER_RESULT_INCOMPLETE;
		}
	}
	return ORDER_RESULT_ILLEGAL;
}

ORDER_RESULT ArmyData::AssassinateRuler(const MapPoint &point)
{
	Unit u;
	Unit c = GetAdjacentCity(point);
	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;






	if(c.GetOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN))
			continue;
		if(g_theUnitDB->Get(m_array[i].GetType())->GetAssasinateRuler()) {
			AddSpecialActionUsed(m_array[i]);

			InformAI(UNIT_ORDER_ASSASSINATE, point); 
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AssassinateRulerUnit,
								   GEA_Unit, m_array[i].m_id,
								   GEA_City, c.m_id,
								   GEA_End);
			u = m_array[i];

			

            
            
            
            
            
            
			SlicObject *so = new SlicObject("911ConductHitCompleteVictim");
			so->AddRecipient(c.GetOwner());
			so->AddCity(c);
			g_slicEngine->Execute(so);

			so = new SlicObject("911ConductHitCompleteAttacker");
			so->AddRecipient(u.GetOwner());	
			so->AddCity(c);
			g_slicEngine->Execute(so);

			return ORDER_RESULT_INCOMPLETE;
		}
	}

	SlicObject *so = new SlicObject("911ConductHitFailedAttack");
	so->AddRecipient(u.GetOwner());
	so->AddCity(c);
	g_slicEngine->Execute(so);

	so = new SlicObject("911ConductHitFailedVictim");
	so->AddRecipient(c.GetOwner());
	so->AddCity(c);
	g_slicEngine->Execute(so);

	return ORDER_RESULT_ILLEGAL;
}

Unit ArmyData::GetAdjacentCity(const MapPoint &point) const
{
	Cell *cell = g_theWorld->GetCell(point);
	

	if(cell->GetCity().m_id == 0)
		return Unit(0);

	MapPoint mypos;
	GetPos(mypos);

	
	if(!point.IsNextTo(mypos) && point != mypos) {
	
		return Unit(0);
	}

	return cell->GetCity();
}

sint32 ArmyData::GetCost()
{
	sint32 cost = 0;
	for(sint32 i = 0; i < m_nElements; i++) {
		cost += m_array[i].GetDBRec()->GetShieldCost();
	}
	return cost;
}

BOOL ArmyData::CanFranchise(double &chance, sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		UnitRecord::ChanceEffect *data;
		if(m_array[i].GetDBRec()->GetCreateFranchise(data) &&
		   !m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::Franchise(const MapPoint &point)
{
    SlicObject *so;
	double chance;
	sint32 uindex;
	
	if(!CanFranchise(chance, uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit city = g_theWorld->GetCity(point);
	Cell *cell = g_theWorld->GetCell(point);
	
	if(city.m_id == 0) {
		return ORDER_RESULT_ILLEGAL;
	}

	
	if(city.GetOwner() == m_array[0].GetOwner()) {
		return ORDER_RESULT_ILLEGAL;
	}

	if(city.GetData()->GetCityData()->GetFranchiseOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

    InformAI(UNIT_ORDER_FRANCHISE, point); 

	AddSpecialActionUsed(m_array[uindex]);

	cell->GetCity().ModifySpecialAttackChance(UNIT_ORDER_FRANCHISE, chance);
	cell->GetCity().SetWatchful();

	if(g_rand->Next(100) < sint32(chance * 100.0)) {
		
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MakeFranchise,
							   GEA_Unit, m_array[uindex],
							   GEA_City, cell->GetCity(),
							   GEA_Player, m_owner,
							   GEA_End);
		
		DPRINTF(k_DBG_GAMESTATE, ("Franchise established\n"));
		ActionSuccessful(SPECATTACK_CREATEFRANCHISE, m_array[uindex], cell->GetCity());

        so = new SlicObject("193BranchCompleteVictim") ;
        so->AddRecipient(city.GetOwner()) ;
        so->AddCity(city) ;
        g_slicEngine->Execute(so) ;

		g_player[city.GetOwner()]->ContactMade(m_owner);
		return ORDER_RESULT_SUCCEEDED;
	} else {

		
		DPRINTF(k_DBG_GAMESTATE, ("Franchise attempt failed\n"));

        so = new SlicObject("195BranchFailedVictim") ;
        so->AddRecipient(city.GetOwner()) ;
        so->AddCity(city) ;
        g_slicEngine->Execute(so) ;

		return ORDER_RESULT_FAILED;
	}
}

BOOL ArmyData::CanSue(sint32 &index) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanSue() &&
		   !m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN) &&
		   m_array[i].CanPerformSpecialAction()) {
			index = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanSue() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanSue() &&
		   m_array[i].CanPerformSpecialAction() ) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanBeSued() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanBeSued())
			return TRUE;
	}
	return FALSE;
}

ORDER_RESULT ArmyData::Sue(const MapPoint &point)
{
	sint32 uindex;

	
	if(!CanSue(uindex))
		return ORDER_RESULT_ILLEGAL;

	if(!point.IsNextTo(m_pos) && point != m_pos) {
		return ORDER_RESULT_ILLEGAL;
	}
	Cell *cell = g_theWorld->GetCell(point);
	if(!cell->UnitArmy() || !cell->UnitArmy()->CanBeSued() || cell->UnitArmy()->GetOwner() == m_owner) {
		if(cell->GetCity().m_id != 0 &&
		   cell->GetCity().GetOwner() == m_owner) {
			Assert(m_orders->GetHead() && m_orders->GetHead()->m_order == UNIT_ORDER_SUE);
			if(m_orders->GetHead() && m_orders->GetHead()->m_order == UNIT_ORDER_SUE) {
				m_orders->GetHead()->m_order = UNIT_ORDER_SUE_FRANCHISE;
			}
			return SueFranchise(point);
		}
		
		return ORDER_RESULT_ILLEGAL;
	}

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_Lawsuit,
						   GEA_Army, m_id,
						   GEA_Unit, m_array[uindex].m_id,
						   GEA_MapPoint, point,
						   GEA_End);

	Unit attacking_unit = m_array[uindex];	

	SlicObject *so = new SlicObject("911SueCompleteVictim");
	so->AddRecipient(cell->UnitArmy()->GetOwner());
	so->AddUnitRecord(m_array[uindex].GetType());
	g_slicEngine->Execute(so);

	so = new SlicObject("911SueCompleteAttacker");
	so->AddRecipient(attacking_unit.GetOwner());
	so->AddUnitRecord(m_array[uindex].GetType());
	g_slicEngine->Execute(so);

	return ORDER_RESULT_SUCCEEDED;
}

ORDER_RESULT ArmyData::SueFranchise(const MapPoint &point)
{
	sint32 uindex;

	
	if(!CanSue(uindex))
		return ORDER_RESULT_ILLEGAL;

	Cell *cell = g_theWorld->GetCell(point);
	
	if(cell->GetCity().m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(cell->GetCity().GetOwner() != m_array[0].GetOwner()) {
		
		return ORDER_RESULT_ILLEGAL;
	}

	if(cell->GetCity().GetFranchiseOwner() < 0) {
		
		return ORDER_RESULT_ILLEGAL;
	}

#if defined(ACTIVISION_ORIGINAL) // Franchises can not be sued until expired
	if(cell->GetCity().GetFranchiseTurnsRemaining() >= 0) {
#else
	if(cell->GetCity().GetFranchiseTurnsRemaining() <= 0) {
#endif
		
		return ORDER_RESULT_ILLEGAL;
	}

	AddSpecialActionUsed(m_array[uindex]);
	
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_RemoveFranchise,
						   GEA_Army, m_id,
						   GEA_Unit, m_array[uindex],
						   GEA_City, cell->GetCity(),
						   GEA_End);

	Unit u = m_array[uindex];

	SlicObject *so = new SlicObject("911SueFranchiseCompleteVictim");
	so->AddRecipient(cell->GetCity().GetFranchiseOwner());
	so->AddCity(cell->GetCity());
	g_slicEngine->Execute(so);

	so = new SlicObject("911SueFranchiseCompleteAttacker");
	so->AddRecipient(u.GetOwner());
	so->AddCity(cell->GetCity());
	g_slicEngine->Execute(so);
	
	return ORDER_RESULT_SUCCEEDED;
}

ORDER_RESULT ArmyData::Expel(const MapPoint &point)
{
	
    PLAYER_INDEX victim;
	Cell *cell = g_theWorld->GetCell(point);
	static CellUnitList expelled;
	MapPoint upos, cpos;
	expelled.Clear();

	if(!point.IsNextTo(m_pos)) {
		return ORDER_RESULT_ILLEGAL;
	}

	sint32 i, n = cell->GetNumUnits();
	if(n > 0) {
		if(cell->UnitArmy()->GetOwner() == m_owner)
			return ORDER_RESULT_ILLEGAL;
    } else { 
        return ORDER_RESULT_ILLEGAL; 
    } 

	sint32 numToExpel = 0;
	for(i = 0; i < n; i++) {
		if(cell->AccessUnit(i).CanBeExpelled() &&
		   cell->AccessUnit(i).GetVisibility() & (1 << m_owner)) {
			numToExpel++;
		}
	}

	if(numToExpel < 1)
		return ORDER_RESULT_ILLEGAL;

    InformAI(UNIT_ORDER_EXPEL, point); 

	BOOL foundCity = FALSE;
	for(i = 0; i < n; i++) {
		if(cell->AccessUnit(i).CanBeExpelled() &&
		   cell->AccessUnit(i).GetVisibility() & (1 << m_owner)) {
			Unit u = cell->AccessUnit(i);

            victim = u.GetOwner();
			foundCity = u.NearestFriendlyCityWithRoom(cpos, numToExpel,
													  &u.GetArmy());
		
			expelled.Insert(u);
		}
	}

	n = expelled.Num();
	if(n > 0) {
		for(i = 0; i < n; i++) {
			if(foundCity) {
				Army newArmy;
				if(expelled[i].GetArmy().Num() > 1) {
					newArmy = g_player[expelled[i].GetOwner()]->GetNewArmy(CAUSE_NEW_ARMY_EXPELLED);
				} else {
					newArmy = expelled[i].GetArmy();
				}

				
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_GetExpelledOrder,
									   GEA_Army, newArmy,
									   GEA_MapPoint, cpos,
									   GEA_Player, GetOwner(),
									   GEA_End);

				if(expelled[i].GetArmy().Num() > 1) {
				
					g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddUnitToArmy,
										   GEA_Unit, expelled[i],
										   GEA_Army, newArmy,
										   GEA_Int, CAUSE_NEW_ARMY_EXPELLED,
										   GEA_End);
					
					
					
					
				}
			} else {
				
				expelled[i].Kill(CAUSE_REMOVE_ARMY_EXPELLED_NO_CITIES, m_owner);
			}
		}

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ExpelUnits,
							   GEA_Army, m_id,
							   GEA_MapPoint, point,
							   GEA_End);

		return ORDER_RESULT_SUCCEEDED;
	}
	return ORDER_RESULT_FAILED;
}

BOOL ArmyData::CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amt,
								   sint32 &uindex) const
{
	UnitRecord::CauseUnhappinessData *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCauseUnhappiness(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			timer = data->GetTimer();
			amt = data->GetAmount();
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amt) const
{
	UnitRecord::CauseUnhappinessData *data;	
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCauseUnhappiness(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			timer = data->GetTimer();
			amt = data->GetAmount();
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::CauseUnhappiness(const MapPoint &point, 
										sint32 uindex)
{
    SlicObject *so;
	double chance;
	sint32 timer, amount;
	Assert(uindex >= 0);
	Assert(uindex < m_nElements);
	UnitRecord::CauseUnhappinessData *cuData;	
	if(uindex < 0 || uindex >= m_nElements)
		return ORDER_RESULT_ILLEGAL;

	if(!m_array[uindex].GetDBRec()->GetCauseUnhappiness(cuData) ||
	   !m_array[uindex].CanPerformSpecialAction()) {
		return ORDER_RESULT_ILLEGAL;
	}
	chance = cuData->GetChance();
	timer = cuData->GetTimer();
	amount = cuData->GetAmount();

	Unit c = GetAdjacentCity(point);
	
	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_array[0].GetOwner()) {
		DPRINTF(k_DBG_GAMESTATE, ("Don't try to make your own people unhappy, bozo!\n"));
		return ORDER_RESULT_ILLEGAL;
	}

    InformAI(UNIT_ORDER_CAUSE_UNHAPPINESS, point); 

	AddSpecialActionUsed(m_array[uindex]);

	ActionSuccessful(SPECATTACK_CAUSEUNHAPPINESS, m_array[uindex], c);

	
	c.ModifySpecialAttackChance(UNIT_ORDER_CAUSE_UNHAPPINESS, chance);
	c.SetWatchful();

    
    char unitName[256];
    strcpy(unitName, g_theStringDB->
           GetIdStr(g_theUnitDB->GetName(m_array[0].GetData()->GetType())));

	if(g_rand->Next(100) >= sint32(chance * 100.0)) {
  
        if (strcmp(unitName, "UNIT_CYBER_NINJA") == 0) {
            so = new SlicObject("230TerrorhackFailedVictim") ;
            so->AddRecipient(c.GetOwner()) ;
            so->AddCity(c) ;
            g_slicEngine->Execute(so) ;

            so = new SlicObject("229TerrorhackFailedAttacker") ;
            so->AddRecipient(GetOwner()) ;
            g_slicEngine->Execute(so) ;
        }

        DPRINTF(k_DBG_GAMESTATE, ("Cause unhappiness failed\n"));
		return ORDER_RESULT_FAILED;
	}

	DPRINTF(k_DBG_GAMESTATE, ("City %lx will be %d less happy for %d turns\n",
							  uint32(c), amount, timer));

	if(g_network.IsHost()) {
		
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_HAPPINESS_ATTACK,
									  (uint32)c, 
									  timer,
									  amount));
	}
	c.AddHappyTimer(timer, double(-amount), HAPPY_REASON_HAPPINESS_ATTACK);
	c.AccessData()->GetCityData()->IndicateHappinessAttacked() ;

    if (strcmp(unitName, "UNIT_CYBER_NINJA") == 0) {
        so = new SlicObject("228TerrorhackCompleteVictim") ;
        so->AddRecipient(c.GetOwner()) ;
        so->AddCity(c) ;
        g_slicEngine->Execute(so) ;

        so = new SlicObject("227TerrorhackCompleteAttacker") ;
        so->AddRecipient(GetOwner()) ;
        so->AddCity(c) ;
        g_slicEngine->Execute(so) ;

    } else if (strcmp(unitName, "UNIT_SUBNEURAL_ADS") == 0) {
        so = new SlicObject("197AdvertiseCompleteVictim") ;
        so->AddRecipient(c.GetOwner()) ;
		so->AddCivilisation(GetOwner());
        so->AddCity(c) ;
        g_slicEngine->Execute(so) ;

    }
        
	return ORDER_RESULT_SUCCEEDED;
}

BOOL ArmyData::CanPlantNuke(double &chance, double &escape_chance,
							sint32 &uindex) const
{
	
	if ( !g_player[m_owner]->m_advances->HasAdvance(advanceutil_GetNukeAdvance()))
		return FALSE;

	UnitRecord::PlantNukeData *data;

	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetPlantNuke(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			chance = data->GetChance();
			escape_chance = data->GetEscapeChance();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanPlantNuke(double &chance, double &escape_chance) const
{
	
	if ( !g_player[m_owner]->m_advances->HasAdvance(advanceutil_GetNukeAdvance()))
		return FALSE;

	UnitRecord::PlantNukeData *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetPlantNuke(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			escape_chance = data->GetEscapeChance();
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::PlantNuke(const MapPoint &point)
{
    SlicObject *so;
	double chance, escape_chance;
	sint32 uindex;
	if(!CanPlantNuke(chance, escape_chance, uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);
	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

    PLAYER_INDEX defense_owner = c.GetOwner();
	if(defense_owner == m_array[0].GetOwner()) {
		DPRINTF(k_DBG_GAMESTATE, ("I don't think you want to do that.\n"));
		return ORDER_RESULT_ILLEGAL;
	}

    

	
	Diplomat::GetDiplomat(c.GetOwner()).LogViolationEvent(m_owner, PROPOSAL_TREATY_CEASEFIRE);

    InformAI(UNIT_ORDER_PLANT_NUKE, point); 

	AddSpecialActionUsed(m_array[uindex]);

	g_slicEngine->RunTerrorismTriggers(m_array[uindex], c);
		
	c.ModifySpecialAttackChance(UNIT_ORDER_PLANT_NUKE, chance);
	c.SetWatchful();

	if(g_rand->Next(100) < sint32(chance * 100.0)) {
		MapPoint pos;
		MapPoint cpos;
		Unit u = m_array[uindex];
		
         


		
		
		
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_PlantNukeUnit,
							   GEA_Unit, m_array[uindex],
							   GEA_City, c.m_id,
							   GEA_End);

		u.GetPos(pos);
		if(g_rand->Next(100) < sint32(escape_chance)) {
			
			u.NearestFriendlyCity(cpos);
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_Teleport,
								   GEA_Army, m_id,
								   GEA_MapPoint, cpos,
								   GEA_End);
			
		}


		ActionSuccessful(SPECATTACK_PLANTNUKE, m_array[uindex], c);

		sint32 oldowner = GetOwner();

		static UnitDynamicArray killList;
		killList.Clear();

		
		
        so = new SlicObject("178NukeCompleteVictim") ;
        so->AddRecipient(c.GetOwner()) ;
        so->AddCity(c) ;
        g_slicEngine->Execute(so);

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_NukeCity,
							   GEA_City, c.m_id,
							   GEA_Player, oldowner,
							   GEA_End);
		
		
		
		
		
		
		
		
		SlicObject *so = new SlicObject("911NukeCompleteAggressor");
		so->AddRecipient(u.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);

		return ORDER_RESULT_SUCCEEDED;
	} else {

        so = new SlicObject("10gNukeFailed") ;
        so->AddRecipient(c.GetOwner()) ;
        so->AddCity(c) ;
        g_slicEngine->Execute(so) ;

        so = new SlicObject("11gNukeFailed") ;
        so->AddRecipient(GetOwner()) ;
        so->AddCity(c) ;
		so->AddUnitRecord(m_array[uindex].GetType()) ;
        g_slicEngine->Execute(so) ;

    }
	
	
	
	return ORDER_RESULT_FAILED;
}

void ArmyData::SetPositionAndFixActors(const MapPoint &p)
{
	sint32 i;
	static UnitDynamicArray revealedUnits;
	revealedUnits.Clear();

	MapPoint opos;
	GetPos(opos);
	BOOL revealed_unexplored;

	for(i = 0; i < m_nElements; i++) {
		g_theWorld->RemoveUnitReference(opos, m_array[i]);
		m_array[i].SetPosition(p, revealedUnits, revealed_unexplored);
	}

	FixActors(opos, p, revealedUnits);
}

void ArmyData::FixActors(MapPoint &opos, const MapPoint &npos, UnitDynamicArray &revealedUnits)
{
	
	UnitActor	**revealedActors;

	sint32 i;
	sint32 numRevealed = revealedUnits.Num();
	sint32 numActors = 0;
	if (numRevealed > 0) {
		revealedActors = new UnitActor*[numRevealed];
		for (i=0; i<numRevealed; i++) {
			if(revealedUnits[i].IsValid()) {
				revealedActors[numActors++] = revealedUnits[i].GetActor();
			}
		}
	} else {
		revealedActors = NULL;
	}

	Unit top_src = GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());
	if(!top_src.IsValid())
		top_src = m_array[0];

	
	UnitActor **restOfStack; 
	sint32 numRest = m_nElements - 1;

	if (numRest > 0) {
		restOfStack = new (UnitActor* [numRest]);  
		GetActors(top_src, restOfStack);
	}

	MapPoint newPos(npos);
	g_director->AddMove(top_src, opos, newPos, numActors, revealedActors, 
						numRest, restOfStack, FALSE, top_src.GetMoveSoundID());

}

BOOL ArmyData::CanSlaveRaid(double &success, double &death, 
							sint32 &timer, sint32 &amount,
							sint32 &uindex) const
{
	UnitRecord::SlaveRaidsData *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetSlaveRaids(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			success = data->GetChance();
			death = data->GetDeathChance();
			timer = data->GetTimer();
			amount = data->GetAmount();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanSlaveRaid(double &success, double &death, 
							sint32 &timer, sint32 &amount) const
{
	UnitRecord::SlaveRaidsData *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if( !m_array[i].GetDBRec()->GetSlaveRaids(data)) {
			
			return FALSE;
		}
	}
	success = data->GetChance();
	death = data->GetDeathChance();
	timer = data->GetTimer();
	amount = data->GetAmount();
	return TRUE;
}

ORDER_RESULT ArmyData::SlaveRaid(const MapPoint &point)
{
	double success, death;
	sint32 timer, amount;
	sint32 uindex;
    BOOL target_is_city;
    Unit target_city;
    Unit home_city;

    
    
    if (!IsSlaveRaidPossible(point, success, death, timer, amount, uindex, 
        target_is_city, target_city, home_city)) { 
		DPRINTF(k_DBG_GAMESTATE, ("!IsSlaveRaidPossible()\n"));
        ERROR_SOUND
        return ORDER_RESULT_ILLEGAL; 
    } 

    
    MapPoint mypos; 
    GetPos(mypos); 
    if(!point.IsNextTo(mypos)) { 
        ERROR_SOUND
		DPRINTF(k_DBG_GAMESTATE, ("!IsNextTo in SlaveRaid\n"));
		return ORDER_RESULT_ILLEGAL;
    }

    if (target_is_city) { 
        InformAI(UNIT_ORDER_SLAVE_RAID, point); 
    } else { 
        InformAI(UNIT_ORDER_ENSLAVE_SETTLER, point); 

		DPRINTF(k_DBG_GAMESTATE, ("Doing EnslaveSettler instead of SlaveRaid\n"));
        return EnslaveSettler(point, uindex, home_city); 
    }  

    
	if(target_city.IsProtectedFromSlavery() > 0) {
        {
            SlicObject *so = new SlicObject("140ProtectedFromSlaveryVictim") ;
            so->AddRecipient(target_city.GetOwner()) ;
            so->AddCity(target_city) ;
            g_slicEngine->Execute(so) ;

            so = new SlicObject("141ProtectedFromSlaveryAttacker");
            so->AddRecipient(GetOwner()) ;
            so->AddCity(target_city) ;
            g_slicEngine->Execute(so) ;
        }

		return ORDER_RESULT_FAILED;
	}

	if(m_array[uindex].IsVeteran()) {
		success += g_theConstDB->EliteSlaverBonus();
	}

	target_city.CD()->ModifySpecialAttackChance(UNIT_ORDER_SLAVE_RAID, success);

	if(target_city.IsWatchful()) {
		
		death *= g_theConstDB->WatchfulCityDeathModifier();
	}

	target_city.SetWatchful();

	AddSpecialActionUsed(m_array[uindex]);

	g_slicEngine->RunEnslavementTriggers(m_array[uindex], target_city);

	if(g_rand->Next(100) < sint32(success * 100.0)) {
        
		sint32 pi = 0;

        
		if(target_city.PopCount() <= 1) {
			return ORDER_RESULT_FAILED;
		}

        
		MapPoint cpos;
		home_city.GetPos(cpos);
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SlaveRaidCity,
							   GEA_Unit, m_array[uindex],
							   GEA_City, target_city.m_id,
							   GEA_End);

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MakePop,
							   GEA_City, home_city,
							   GEA_Player, target_city.GetOwner(),
							   GEA_End);


		
		BOOL cityDead = FALSE; 
		if(g_theUnitPool->IsValid(target_city)) {
			target_city.AddHappyTimer(timer, -amount, HAPPY_REASON_SLAVES_TAKEN);
		}

		SlicObject *so = new SlicObject("137SlaveCompleteVictim") ;
		so->AddRecipient(target_city.GetOwner()) ;
		so->AddCity(target_city) ;
		g_slicEngine->Execute(so) ;
		
		so = new SlicObject("137SlaveryCompleteAttacker");
		so->AddRecipient(GetOwner()) ;
		so->AddCivilisation(GetOwner()) ;
		so->AddCity(home_city) ;
		g_slicEngine->Execute(so) ;

		ActionSuccessful(SPECATTACK_SLAVERAID, m_array[uindex], target_city);
        return ORDER_RESULT_SUCCEEDED; 
    } else {
		

        {
            SlicObject *so = new SlicObject("138SlaveFailedVictim") ;
            so->AddRecipient(target_city.GetOwner()) ;
            so->AddCity(target_city) ;
            g_slicEngine->Execute(so) ;
        }

		if(g_rand->Next(100) < sint32(death * 100.0)) {
			MapPoint pos;
			m_array[uindex].GetPos(pos);
			g_player[m_array[uindex].GetOwner()]->
				RegisterLostUnits(1, pos, DEATH_EFFECT_CALC);
			m_array[uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_SLAVERAID, -1);
		}
        return ORDER_RESULT_FAILED;
	}
}

BOOL ArmyData::IsSlaveRaidPossible(const MapPoint &point, 
								   double &success, 
								   double &death, 
								   sint32 &timer, 
								   sint32 &amount,
								   sint32 &uindex, 
								   BOOL &target_is_city, 
								   Unit &target_city, 
								   Unit &home_city)
{
    target_city.m_id = 0; 
    home_city.m_id = 0; 

    if(!CanSlaveRaid(success, death, timer, amount, uindex)) { 
		return FALSE; 
    }

    target_city = GetAdjacentCity(point);
    Cell *cell = g_theWorld->GetCell(point);
	target_city = cell->GetCity();

    if(target_city.m_id != 0) { 

        target_is_city = TRUE; 

	    if(target_city.GetOwner() == m_array[0].GetOwner()) {
		    return FALSE; 
	    }

        if(target_city.PopCount() < 2) {
		    return FALSE;
	    }

    } else { 
        target_is_city = FALSE; 

        sint32 uindex;
        if(!CanEnslaveSettler(uindex)) { 
		    return FALSE; 
        }

	    
	    sint32 n = cell->GetNumUnits();
	    if(n != 1) {
		    return FALSE; 
	    }
	    
        if(!cell->AccessUnit(0).GetDBRec()->GetSettle()) { 
		    return FALSE; 
	    }

        if(cell->AccessUnit(0).GetOwner() == m_array[0].GetOwner()) { 
		    return FALSE; 
        }
    }

    
	MapPoint slpos;
	GetPos(slpos);
	sint32 r = g_player[m_array[0].GetOwner()]->GetSlaveCity(slpos, home_city);

     if(!r) { 
		return FALSE; 
     }

    return TRUE; 
}

BOOL ArmyData::CanEnslaveSettler(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetSettlerSlaveRaids() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::EnslaveSettler(const MapPoint &point, const sint32 uindex, 
                              Unit home_city)
{
	sint32 r = g_player[m_owner]->GetSlaveCity(m_pos, home_city);
	if(!r)
		return ORDER_RESULT_ILLEGAL; 

    Cell *cell = g_theWorld->GetCell(point);

	sint32 n = cell->GetNumUnits();
	Assert(n > 0);
	if(n != 1) {
        ERROR_SOUND
		return ORDER_RESULT_ILLEGAL;
	}
	
	if(!cell->AccessUnit(0).GetDBRec()->GetSettle()) {
        ERROR_SOUND
		return ORDER_RESULT_ILLEGAL;
	}


	ActionSuccessful(SPECATTACK_ENSLAVESETTLER, m_array[uindex], cell->AccessUnit(0));

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_EnslaveSettler,
						   GEA_Army, m_id,
						   GEA_Unit, m_array[uindex],
						   GEA_Unit, cell->AccessUnit(0).m_id,
						   GEA_End);
	sint32 settlerOwner = cell->AccessUnit(0).GetOwner();
	

	
    
    
	sint32 pi = 0;

	
	

    
	
	
    SlicObject *so;

    so = new SlicObject("139SettlerSlavedVictim");
    so->AddRecipient(settlerOwner) ;
    g_slicEngine->Execute(so) ;

    so = new SlicObject("137SlaveryCompleteAttacker");
    so->AddRecipient(m_owner) ;
	so->AddCivilisation(m_owner) ;
    so->AddCity(home_city) ;
    g_slicEngine->Execute(so) ;

	AddSpecialActionUsed(m_array[uindex]);

    return ORDER_RESULT_SUCCEEDED;
}

BOOL ArmyData::CanUndergroundRailway(double &success, double &death,
						   sint32 &uindex) const
{
	UnitRecord::SuccessDeathEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetUndergroundRailway(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			success = data->GetChance();
			death = data->GetDeathChance();

			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanUndergroundRailway(double &success, double &death) const
{
	UnitRecord::SuccessDeathEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetUndergroundRailway(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			success = data->GetChance();
			death = data->GetDeathChance();
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::UndergroundRailway(const MapPoint &point)
{
    SlicObject *so;
	double success, death;
	sint32 uindex;

	if(!CanUndergroundRailway(success, death, uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);
	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_array[0].GetOwner()) {
		DPRINTF(k_DBG_GAMESTATE, ("You can't free your own slaves with an abolitionist.  In fact, you shouldn't even be able to have both slaves and an abolitionist.  Stop cheating!\n"));
		return ORDER_RESULT_ILLEGAL;
	}

	if(c.CountSlaves() <= 0) {
		DPRINTF(k_DBG_GAMESTATE, ("Dammit, there aren't any slaves there."));

        so = new SlicObject("167FreeslaveNoSlavesToFree") ;
        so->AddRecipient(GetOwner()) ;
        so->AddCity(c) ;
        g_slicEngine->Execute(so) ;

		return ORDER_RESULT_ILLEGAL;
	}

    InformAI(UNIT_ORDER_UNDERGROUND_RAILWAY, point); 

	if(m_array[uindex].IsVeteran()) {
		success += g_theConstDB->EliteAbolitionistBonus();
	}

	if(c.IsWatchful()) {
		success *= g_theConstDB->WatchfulCitySuccessModifier();
		death *= g_theConstDB->WatchfulCityDeathModifier();
	}

	c.SetWatchful();

	if(g_rand->Next(100) < sint32(success * 100.0)) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_UndergroundRailwayUnit,
							   GEA_Unit, m_array[uindex].m_id,
							   GEA_City, c,
							   GEA_End);
		return ORDER_RESULT_SUCCEEDED;
	} else {
		AddSpecialActionUsed(m_array[uindex]);

		if(g_rand->Next(100) < sint32(death * 100.0)) {
			m_array[uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_UNDERGROUND_RR_RAID, -1);
		}

        so = new SlicObject("164FreeslaveFailedVictim") ;
        so->AddRecipient(c.GetOwner()) ;
        so->AddCity(c) ;
        g_slicEngine->Execute(so) ;

        so = new SlicObject("166FreeslaveFailedAgressor") ;
        so->AddRecipient(GetOwner()) ;
        so->AddCity(c) ;
        g_slicEngine->Execute(so) ;

		return ORDER_RESULT_FAILED;
	}
}

BOOL ArmyData::CanInciteUprising(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetSlaveUprising() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::InciteUprising(const MapPoint &point)
{
    SlicObject *so;
	sint32 uindex;
	if(!CanInciteUprising(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner) {
		
		
		return ORDER_RESULT_ILLEGAL;
	}

	if(c.CountSlaves() <= 0) {
        so = new SlicObject("167FreeslaveNoSlavesToFree") ;
        so->AddRecipient(GetOwner()) ;
        so->AddCity(c) ;
        g_slicEngine->Execute(so) ;

		return ORDER_RESULT_ILLEGAL;
	}

    InformAI(UNIT_ORDER_INCITE_UPRISING, point); 

	MapPoint start, dest;
    PLAYER_INDEX city_owner = c.GetOwner();
	Player *p = g_player[city_owner];
	float distcost;

	if(p->GetCapitolPos(start)) {
		c.GetPos(dest);
        distcost = 100.0f * start.NormalizedDistance(dest);
	} else {
		distcost = float(p->GetMaxEmpireDistance()); 
	}
	
	distcost += 100;

	if(distcost < 1)
		distcost = 1;


	
#define k_SOME_DISTANCE_NUMBER 1.0

	double numSlaves = c.CountSlaves();
	sint32 capitolPenalty = 0;
	if(c.IsCapitol()) {
		capitolPenalty = sint32(g_theConstDB->InciteUprisingCapitolPenalty());
	}
	double cost = (g_player[c.GetOwner()]->m_gold->GetLevel() + 5000) *
		double(c.PopCount()) * (1 / distcost) *
		 g_theConstDB->InciteUprisingGoldCoefficient() +
		 capitolPenalty;

	DPRINTF(k_DBG_GAMESTATE, ("Cost to incite uprising: %lf\n", cost));

	if(g_player[m_owner]->m_gold->GetLevel() < cost)
		return ORDER_RESULT_FAILED;

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SubGold,
						   GEA_Player, m_owner,
						   GEA_Int, cost,
						   GEA_End);
	
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_InciteUprisingUnit,
						   GEA_Unit, m_array[uindex].m_id,
						   GEA_City, c,
						   GEA_End);
	AddSpecialActionUsed(m_array[uindex]);
	

    so = new SlicObject("208UprisingCompleteVictim") ;
    so->AddRecipient(city_owner) ;
    so->AddCity(c) ;
    g_slicEngine->Execute(so) ;

	ActionSuccessful(SPECATTACK_SLAVEUPRISING, m_array[uindex], c);

	return ORDER_RESULT_SUCCEEDED;
}

	
BOOL ArmyData::CanEstablishEmbassy(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetEstablishEmbassy()) {
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanEstablishEmbassy() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetEstablishEmbassy() &&
		   m_array[i].CanPerformSpecialAction()) {
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::EstablishEmbassy(const MapPoint &point)
{
	sint32 uindex;

	if(!CanEstablishEmbassy(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

	
	
	
	if (AgreementMatrix::s_agreements.HasAgreement(m_owner, c.GetOwner(), PROPOSAL_TREATY_DECLARE_WAR))
		return ORDER_RESULT_ILLEGAL;

	if(wonderutil_GetCloseEmbassies(g_player[c.GetOwner()]->m_builtWonders)) {
		SlicObject *so = new SlicObject("145NoEmbassiesWonder");
		so->AddCivilisation(c.GetOwner());
		so->AddCity(c);
		sint32 w;
		for(w = 0; w < g_theWonderDB->NumRecords(); w++) {
			if((g_player[c.GetOwner()]->m_builtWonders & ((uint64)1 << w)) &&
			   !wonderutil_IsObsolete(w)) {
				so->AddWonder(w);
				break;
			}
		}
		Assert(w < g_theWonderDB->NumRecords());
		if(w >= g_theWonderDB->NumRecords()) {
			delete so;
		} else {
			so->AddRecipient(m_owner);
			g_slicEngine->Execute(so);
		}
		return ORDER_RESULT_FAILED;
	}

	
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_EstablishEmbassyUnit,
						   GEA_Unit, m_array[uindex].m_id,
						   GEA_City, c,
						   GEA_End);
	

	return ORDER_RESULT_INCOMPLETE;
}

BOOL ArmyData::CanThrowParty(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetThrowParty()) {
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanThrowParty() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetThrowParty() &&
		   m_array[i].CanPerformSpecialAction()) {
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::ThrowParty(const MapPoint &point)
{
	sint32 uindex;

	sint32 cost = 100;
	cost = g_theConstDB->GetMaximumPartyCost();

	if(g_player[m_owner]->m_gold->GetLevel() < cost) {
		return ORDER_RESULT_ILLEGAL;
	}

	if(!CanThrowParty(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);
	Unit u = m_array[uindex];

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

	if (AgreementMatrix::s_agreements.HasAgreement(m_owner, c.GetOwner(), PROPOSAL_TREATY_DECLARE_WAR))
		return ORDER_RESULT_ILLEGAL;

	
	if (!Diplomat::GetDiplomat(c.GetOwner()).ReadyToParty())
		return ORDER_RESULT_ILLEGAL;

	AddSpecialActionUsed(m_array[uindex]);

	
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ThrowPartyUnit,
		GEA_Unit, m_array[uindex].m_id,
		GEA_City, c,
		GEA_End);
	
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SubGold,
		GEA_Player, m_owner,
		GEA_Int, cost,
		GEA_End);

	













		
	return ORDER_RESULT_SUCCEEDED;
}

BOOL ArmyData::CanBioInfect(double &chance, sint32 &uindex) const
{
	UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetBioTerror(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanBioInfect(double &chance) const
{
	UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetBioTerror(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::BioInfect(const MapPoint &point)
{
	SlicObject	*so ;

	double chance;
	sint32 uindex;

	if(!CanBioInfect(chance, uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;


	if(c.GetOwner() == m_array[0].GetOwner()) {
		return ORDER_RESULT_ILLEGAL;
	}

    if(c.IsBioInfected()) {
        return ORDER_RESULT_ILLEGAL;
    }        

    InformAI(UNIT_ORDER_BIO_INFECT, point); 

	if(m_array[uindex].IsVeteran()) {
		chance += g_theConstDB->EliteTerroristBonus();
	}

	if(c.IsBioImmune()) {
		DPRINTF(k_DBG_GAMESTATE, ("Bio infection failed because city immune\n"));
		so = new SlicObject("10iImmuneToBioInfect") ;
		so->AddRecipient(c.GetOwner()) ;
		so->AddCivilisation(c.GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		so = new SlicObject("11iImmuneToBioInfect") ;
		so->AddRecipient(GetOwner()) ;
		so->AddCivilisation(c.GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		return ORDER_RESULT_FAILED;
	}

	AddSpecialActionUsed(m_array[uindex]);

	c.ModifySpecialAttackChance(UNIT_ORDER_BIO_INFECT, chance);
	c.SetWatchful();

	if(g_rand->Next(100) < sint32(chance * 100.0)) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_BioInfectCityUnit,
							   GEA_Unit, m_array[uindex].m_id,
							   GEA_City, c.m_id,
							   GEA_End);
		return ORDER_RESULT_SUCCEEDED;
	} else {
		DPRINTF(k_DBG_GAMESTATE, ("Bio infection failed because I said so.\n"));
		so = new SlicObject("10iBioInfectFailed") ;
		so->AddRecipient(c.GetOwner()) ;
		so->AddCivilisation(GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		so = new SlicObject("11iBioInfectFailed") ;
		so->AddRecipient(GetOwner()) ;
		so->AddCivilisation(c.GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		if(g_rand->Next(100) < sint32(g_theConstDB->BioInfectionTerroristDeathChance() * 100.0))
			m_array[uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);

		
		return ORDER_RESULT_FAILED;
	}
}

BOOL ArmyData::CanPlague(double &chance, sint32 &uindex) const
{
	UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetPlague(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanPlague(double &chance) const
{
	UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetPlague(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::Plague(const MapPoint &point)
{
	SlicObject	*so ;

	double chance;
	sint32 uindex;

	if(!CanPlague(chance, uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;


	if(c.GetOwner() == m_array[0].GetOwner()) {
		return ORDER_RESULT_ILLEGAL;
	}

    InformAI(UNIT_ORDER_BIO_INFECT, point); 

	if(m_array[uindex].IsVeteran()) {
		chance += g_theConstDB->EliteTerroristBonus();
	}

	if(c.IsBioImmune()) {
		DPRINTF(k_DBG_GAMESTATE, ("Plague failed because city immune\n"));
		so = new SlicObject("10jImmuneToPlague") ;
		so->AddRecipient(c.GetOwner()) ;
		so->AddCivilisation(GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		so = new SlicObject("11jImmuneToPlague") ;
		so->AddRecipient(GetOwner()) ;
		so->AddCivilisation(c.GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		return ORDER_RESULT_FAILED;
	}

	AddSpecialActionUsed(m_array[uindex]);

	c.ModifySpecialAttackChance(UNIT_ORDER_PLAGUE, chance);
	c.SetWatchful();

	if(g_rand->Next(100) < sint32(chance * 100.0)) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_PlagueCityUnit,
							   GEA_Unit, m_array[uindex].m_id,
							   GEA_City, c.m_id,
							   GEA_End);
		return ORDER_RESULT_SUCCEEDED;
	} else {
		DPRINTF(k_DBG_GAMESTATE, ("Plague failed because I said so.\n"));
		so = new SlicObject("10jPlagueFailed") ;
		so->AddRecipient(c.GetOwner()) ;
		so->AddCivilisation(GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		so = new SlicObject("11jPlagueFailed") ;
		so->AddRecipient(GetOwner()) ;
		so->AddCivilisation(c.GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		if(g_rand->Next(100) < sint32(g_theConstDB->BioInfectionTerroristDeathChance() * 100.0))
			m_array[uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);

		
		return ORDER_RESULT_FAILED;
	}
}

BOOL ArmyData::CanNanoInfect(double &chance, sint32 &uindex) const
{
	UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetNanoTerror(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanNanoInfect(double &chance) const
{
	UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetNanoTerror(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::NanoInfect(const MapPoint &point)
{
	SlicObject	*so ;

	double chance;
	sint32 uindex;

	if(!CanNanoInfect(chance, uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;


	if(c.GetOwner() == m_array[0].GetOwner()) {
		return ORDER_RESULT_ILLEGAL;
	}

    if(c.IsNanoInfected()) {
        return ORDER_RESULT_ILLEGAL;
    }        

    InformAI(UNIT_ORDER_NANO_INFECT, point); 

	if(m_array[uindex].IsVeteran()) {
		chance += g_theConstDB->EliteTerroristBonus();
	}

	AddSpecialActionUsed(m_array[uindex]);

	if(c.IsNanoImmune()) {
		so = new SlicObject("10hImmuneToNanoTerror") ;
		so->AddRecipient(c.GetOwner()) ;
		so->AddCivilisation(GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		so = new SlicObject("11hImmuneToNanoTerror") ;
		so->AddRecipient(GetOwner()) ;
		so->AddCivilisation(c.GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		DPRINTF(k_DBG_GAMESTATE, ("Nano infection failed: City immune\n"));

		return ORDER_RESULT_FAILED;
	}

	c.ModifySpecialAttackChance(UNIT_ORDER_NANO_INFECT, chance);
	c.SetWatchful();

	if(g_rand->Next(100) < sint32(chance * 100.0)) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_NanoInfectCityUnit,
							   GEA_Unit, m_array[uindex],
							   GEA_City, c,
							   GEA_End);
		DPRINTF(k_DBG_GAMESTATE, ("Nano Infection: Success\n"));
		so = new SlicObject("911CrisisCityIsNanoInfected");
		so->AddRecipient(c.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);

		return ORDER_RESULT_SUCCEEDED;
	} else {
		DPRINTF(k_DBG_GAMESTATE, ("Nano infection failed: The world is a safer place.\n"));
		
		so = new SlicObject("10hNanoTerrorFailed") ;
		so->AddRecipient(c.GetOwner()) ;
		so->AddCivilisation(GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		so = new SlicObject("11hNanoTerrorFailed") ;
		so->AddRecipient(GetOwner()) ;
		so->AddCivilisation(c.GetOwner()) ;
		so->AddCity(c) ;
		g_slicEngine->Execute(so) ;

		if(g_rand->Next(100) < sint32(g_theConstDB->NanoInfectionTerroristDeathChance() * 100.0))
			m_array[uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);

		return ORDER_RESULT_FAILED;
	}
}




BOOL ArmyData::CanConvertCity(double &best_chance, double &best_death_chance, 
							  sint32 &best_uindex) const
{
	double chance = 0.0;
	double death_chance = 0.0;
	sint32 i;
	
	best_chance = -1.0;

	UnitRecord::SuccessDeathEffect *data;
    for (i = m_nElements - 1; i>= 0; i--) { 
		
		
        if (m_array[i].GetDBRec()->GetConvertCities(data) &&
			m_array[i].CanPerformSpecialAction()) {
			
			
			chance = data->GetChance();
			death_chance = data->GetDeathChance();
			if(chance > best_chance) {
				best_chance = chance;
				best_death_chance = death_chance;
				best_uindex= i;
			}
		}
	}
	return (best_chance > 0.0);	
}

BOOL ArmyData::CanConvertCity(double &best_chance, double &best_death_chance) const
{
	double chance = 0.0;
	double death_chance = 0.0;
	sint32 i;
	
	best_chance = -1.0;

	UnitRecord::SuccessDeathEffect *data;
    for (i = m_nElements - 1; i>= 0; i--) { 
		
		
        if ( m_array[i].GetDBRec()->GetConvertCities(data) &&
			m_array[i].CanPerformSpecialAction() ) {
			chance = data->GetChance();
			death_chance = data->GetDeathChance();
			
			
			if(chance > best_chance) {
				best_chance = chance;
				best_death_chance = death_chance;
			}
			if ( best_chance <= 0.0 ) {
				return FALSE;
			}
		}
		else {
			return FALSE;
		}
	}
	return TRUE;	
}

BOOL ArmyData::AbleToConvertTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++) 
	{
		if(m_array[i].CanConvert(point))
		{
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanConvertCity(const MapPoint &point) const
{
	Unit city = GetAdjacentCity(point);
	if(city.m_id == 0)
		return false;

	for(sint32 i = 0; i < m_nElements; i++) {
		const UnitRecord *rec = m_array[i].GetDBRec();
		if (m_array[i].CanConvertCity(city))
			return TRUE;
	}
	return FALSE;
}

ORDER_RESULT ArmyData::ConvertCity(const MapPoint &point)
{
	double chance, death_chance;
	double best_chance = 0.0;
	double best_death_chance = 0.0;
	sint32 best_uindex = -1;
	sint32 i;
	Unit u;

	
	Unit city = GetAdjacentCity(point);

	if(city.m_id  == 0)
		return ORDER_RESULT_ILLEGAL;

	if(city.GetData()->GetCityData()->IsProtectedFromConversion()) {
		SlicObject *so = new SlicObject("361IAProtectedFromConversionByWonder");
		so->AddCity(city);
		sint32 i;
		for(i = 0; i < g_theWonderDB->NumRecords(); i++) {
			if(!g_player[city.GetOwner()]->m_builtWonders & ((uint64)1 << (uint64)i))
				continue;

			if(wonderutil_Get(i)->GetPreventConversion()) {
				so->AddWonder(i);
				break;
			}
		}
		Assert(i < g_theWonderDB->NumRecords());
		if(i >= g_theWonderDB->NumRecords()) {
			so->AddCivilisation(city.GetOwner());
			so->AddRecipient(m_owner);
			g_slicEngine->Execute(so);
		} else {
			delete so;
		}
		return ORDER_RESULT_ILLEGAL;
	}

	UnitRecord::SuccessDeathEffect *data;
	
    for (i = m_nElements - 1; i>= 0; i--) { 
		if(m_array[i].CanPerformSpecialAction()&&
		   m_array[i].CanConvertCity(city)) {
			m_array[i].GetDBRec()->GetConvertCities(data);
			chance = data->GetChance();
			death_chance = data->GetDeathChance();
			if (chance > best_chance) {
				best_chance = chance;
				best_death_chance = death_chance;
				best_uindex = i;
			}
		}
	}
					

	if(best_uindex < 0)
		return ORDER_RESULT_ILLEGAL;

	chance = chance - (double(chance) * (city.TheologicalModifier() / 100.0));
	
	AddSpecialActionUsed(m_array[best_uindex]);

    InformAI(UNIT_ORDER_CONVERT, point); 

	city.ModifySpecialAttackChance(UNIT_ORDER_CONVERT, best_chance);
	city.SetWatchful();
	
	u = m_array[best_uindex];

	if(g_rand->Next(100) < sint32(chance * 100.0)) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ConvertCityUnit,
							   GEA_Unit, m_array[best_uindex],
							   GEA_City, city,
							   GEA_End);
		
		return ORDER_RESULT_SUCCEEDED;
	} else {
		
		DPRINTF(k_DBG_GAMESTATE, ("Conversion failed\n"));
        {
            SlicObject *so = new SlicObject("152ConvertFailedVictim") ;
            so->AddRecipient(city.GetOwner()) ;
            so->AddCity(city) ;
			so->AddUnitRecord(m_array[best_uindex].GetType());
            g_slicEngine->Execute(so) ;

            so = new SlicObject("153ConvertFailedAttacker") ;
            so->AddRecipient(GetOwner()) ;
            so->AddCity(city) ;
			so->AddUnitRecord(m_array[best_uindex].GetType());
            g_slicEngine->Execute(so) ;
        }


		if(g_rand->Next(100) < sint32(best_death_chance * 100.0)) {
			DPRINTF(k_DBG_GAMESTATE, ("And cleric died\n"));
			m_array[best_uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_CONVERT_RAID, -1);
		}

		return ORDER_RESULT_FAILED;
	}
}

	
BOOL ArmyData::CanReformCity(sint32 &uindex, const MapPoint &point) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		const UnitRecord *rec = m_array[i].GetDBRec();
		if(rec->GetCanReform() &&
		   m_array[i].CanPerformSpecialAction()) {
			if(!rec->GetMovementTypeLand() && !rec->GetMovementTypeMountain())
				continue;
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanReformCity() const
{
	MapPoint point;
	GetPos( point );

	for(sint32 i = 0; i < m_nElements; i++) {
		const UnitRecord *rec = m_array[i].GetDBRec();
		if(!rec->GetCanReform() ||
		   !m_array[i].CanPerformSpecialAction()) {
			return FALSE;
		}
	}
	return TRUE;
}

ORDER_RESULT ArmyData::ReformCity(const MapPoint &point)
{

	sint32 uindex;

	if(!CanReformCity(uindex, point))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;


	if(c.GetOwner() != m_array[uindex].GetOwner())
		return ORDER_RESULT_ILLEGAL;


	if(c.IsConvertedTo() < 0)
		return ORDER_RESULT_ILLEGAL;

	AddSpecialActionUsed(m_array[uindex]);

	if(g_rand->Next(100) < sint32(g_theConstDB->ReformationChance() * 100.0)) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ReformCityUnit,
							   GEA_Unit, m_array[uindex].m_id,
							   GEA_City, c.m_id,
							   GEA_End);
		return ORDER_RESULT_SUCCEEDED;
	} else {
		DPRINTF(k_DBG_GAMESTATE, ("Reformation failed\n"));
		
		if(g_rand->Next(100) < sint32(g_theConstDB->ReformationDeathChance() * 100.0)) {
			DPRINTF(k_DBG_GAMESTATE, ("And inquisitor died.\n"));
			
			m_array[uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_REFORMATION, -1);
		}

		return ORDER_RESULT_FAILED;
	}
}

BOOL ArmyData::CanSellIndulgences(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetIndulgenceSales() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanSellIndulgences() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetIndulgenceSales() &&
		   m_array[i].CanPerformSpecialAction()) {
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::IndulgenceSale(const MapPoint &point)
{
	sint32 uindex;

	if(!CanSellIndulgences(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);
	Unit u = m_array[uindex];

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

    InformAI(UNIT_ORDER_INDULGENCE, point); 

	AddSpecialActionUsed(m_array[uindex]);

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_IndulgenceSaleMade,
						   GEA_Unit, m_array[uindex],
						   GEA_City, c,
						   GEA_End);

	if(c.IsConvertedTo() < 0) {
		
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SubGold,
							   GEA_Player, c.GetOwner(),
							   GEA_Int, g_theConstDB->UnconvertedIndulgenceGold(),
							   GEA_End);
		

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddGold,
							   GEA_Player, m_owner,
							   GEA_Int, g_theConstDB->UnconvertedIndulgenceGold(),
							   GEA_End);

		

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddHappyTimer,
							   GEA_City, c.m_id,
							   GEA_Int, 1,
							   GEA_Int, g_theConstDB->UnconvertedIndulgenceHappiness(),
							   GEA_Int, HAPPY_REASON_INDULGENCES,
							   GEA_End);
		
		
	} else if(c.IsConvertedTo() == m_array[uindex].GetOwner()) {
		
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SubGold,
							   GEA_Player, c.GetOwner(),
							   GEA_Int, g_theConstDB->ConvertedIndulgenceGold(),
							   GEA_End);
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddGold,
							   GEA_Player, m_owner,
							   GEA_Int, g_theConstDB->ConvertedIndulgenceGold(),
							   GEA_End);
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddHappyTimer,
							   GEA_City, c.m_id,
							   GEA_Int, 1,
							   GEA_Int, g_theConstDB->ConvertedIndulgenceHappiness(),
							   GEA_Int, HAPPY_REASON_INDULGENCES,
							   GEA_End);
		
		
		
		
	} else {
		
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SubGold,
							   GEA_Player, c.GetOwner(),
							   GEA_Int, g_theConstDB->OtherFaithIndulgenceGold(),
							   GEA_End);
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddGold,
							   GEA_Player, m_owner,
							   GEA_Int, g_theConstDB->OtherFaithIndulgenceGold(),
							   GEA_End);
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddHappyTimer,
							   GEA_City, c.m_id,
							   GEA_Int, 1,
							   GEA_Int, g_theConstDB->OtherFaithIndulgenceHappiness(),
							   GEA_Int, HAPPY_REASON_INDULGENCES,
							   GEA_End);
		
		
		
		
	}

	if(uindex == 66) { 
		SlicObject *so = new SlicObject("911FaithHealVictim");
		so->AddRecipient(c.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);

		so = new SlicObject("911FaithHealAttacker");
		so->AddRecipient(u.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);
	} else { 
		SlicObject *so = new SlicObject("911IndulgenceCompleteVictim");
		so->AddRecipient(c.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);

		so = new SlicObject("911IndulgenceCompleteAttacker");
		so->AddRecipient(u.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);
	}

	ActionSuccessful(SPECATTACK_SELLINDULGENCE, m_array[uindex], c);
	return ORDER_RESULT_SUCCEEDED;
}

BOOL ArmyData::CanSoothsay(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanSoothsay() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL ArmyData::CanSoothsay() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanSoothsay() &&
		   m_array[i].CanPerformSpecialAction()) {
			return TRUE;
		}
	}

	return FALSE;
}

ORDER_RESULT ArmyData::Soothsay(const MapPoint &point)
{
	sint32 uindex;

	if(!CanSoothsay(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit u = m_array[uindex];

	ORDER_RESULT res = CauseUnhappiness(point, uindex);
	if(res == ORDER_RESULT_SUCCEEDED || res == ORDER_RESULT_SUCCEEDED_INCOMPLETE) {
		Unit c = GetAdjacentCity(point);
		if(c.IsValid()) {
			SlicObject *so = new SlicObject("911SoothsayCompleteVictim");
			so->AddRecipient(c.GetOwner());
			so->AddCity(c);
			so->AddUnitRecord(m_array[uindex].GetType());
			g_slicEngine->Execute(so);

			so = new SlicObject("911SoothsayCompleteAttacker");
			so->AddRecipient(u.GetOwner());
			so->AddUnitRecord(m_array[uindex].GetType());
			so->AddCity(c);
			g_slicEngine->Execute(so);
		}
	}
	return res;
}

BOOL ArmyData::CanAdvertise(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetAdvertise() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanAdvertise() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetAdvertise())
			return TRUE;
	}
	return FALSE;
}

ORDER_RESULT ArmyData::Advertise(const MapPoint &point)
{
	sint32 uindex;
	Unit c = GetAdjacentCity(point);

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

	if(!CanAdvertise(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit u = m_array[uindex];

	SlicObject *so = new SlicObject("911AdvertiseCompleteAttacker");
	so->AddRecipient(u.GetOwner());
	so->AddCity(c);
	g_slicEngine->Execute(so);

	return CauseUnhappiness(point, uindex);
}

BOOL ArmyData::CanLaunch(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetSpaceLaunch() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanLaunch() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetSpaceLaunch())
			return TRUE;
	}
	return FALSE;
}

ORDER_RESULT ArmyData::Launch(Order *order)
{
	sint32 uindex;

	if(!CanLaunch(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit destCity = g_theWorld->GetCity(order->m_point);
	if(!destCity.IsValid() || destCity.GetOwner() != m_owner)
		return ORDER_RESULT_ILLEGAL;

	if(m_nElements > 1) {
		
		
		
		m_array[uindex]->CreateOwnArmy();
	}

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_LaunchUnit,
						   GEA_Unit, m_array[uindex],
						   GEA_MapPoint, order->m_point,
						   GEA_End);
	return ORDER_RESULT_SUCCEEDED;
}

BOOL ArmyData::CanTarget(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetNuclearAttack() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL ArmyData::CanTarget() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetNuclearAttack())
			return TRUE;
	}
	return FALSE;
}

ORDER_RESULT ArmyData::Target(Order *order)
{
	if(!CanTarget()) return ORDER_RESULT_ILLEGAL;

	Unit targetCity = g_theWorld->GetCity(order->m_point);
	if(!targetCity.IsValid() || targetCity.GetOwner() == m_owner) {
		
		return ORDER_RESULT_ILLEGAL;
	}

	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetNuclearAttack()) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SetTarget,
								   GEA_Unit, m_array[i],
								   GEA_City, targetCity,
								   GEA_End);
		}
	}
	return ORDER_RESULT_SUCCEEDED;
}

ORDER_RESULT ArmyData::ClearTarget()
{
	if(!CanTarget()) return ORDER_RESULT_ILLEGAL;

	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetNuclearAttack()) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ClearTarget,
								   GEA_Unit, m_array[i],
								   GEA_End);
		}
	}
	return ORDER_RESULT_SUCCEEDED;
}

void ArmyData::SetReentry(sint32 turns, MapPoint &pos)
{
	m_reentryTurn = NewTurnCount::GetCurrentRound() + turns;
	m_reentryPos = pos;
	m_flags |= k_CULF_IN_SPACE;
}

void ArmyData::Reenter()
{
	Assert(m_flags & k_CULF_IN_SPACE);
	if(!(m_flags & k_CULF_IN_SPACE)) {
		return;
	}

	UnitDynamicArray revealedUnits;
	revealedUnits.Clear();

	Unit city;
	city = g_theWorld->GetCity(m_reentryPos);
	sint32 i;

	if(!city.IsValid() || city.GetOwner() != m_owner) {
		
		for(i = 0; i < m_nElements; i++) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
								   GEA_Unit, m_array[i].m_id,
								   GEA_Int, 0,
								   GEA_Player, -1,
								   GEA_End);
		}
	} else if(g_theWorld->GetCell(m_reentryPos)->GetNumUnits() > (k_MAX_ARMY_SIZE - m_nElements)) {
		
		return;
	} else {
		m_flags &= ~(k_CULF_IN_SPACE);
		MapPoint oldPos = m_pos;

		for(i = 0; i < m_nElements; i++) {
			m_array[i]->ClearFlag(k_UDF_HAS_LEFT_MAP);
			m_array[i]->ClearFlag(k_UDF_IN_SPACE);
			
			m_array[i]->SetPosAndNothingElse(m_reentryPos);
			g_theWorld->InsertUnit(m_reentryPos, m_array[i], revealedUnits);
			BOOL revealed;
			m_array[i]->AddUnitVision(revealed);
		}

		
		ResetPos();
		FixActors(oldPos, m_pos, revealedUnits);
		for(i = 0; i < m_nElements; i++) {
			g_director->AddShow(m_array[i]);
		}
	}
}

BOOL ArmyData::AbleToPlantNukeTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++) 
	{
		if(m_array[i].CanPlantNuke(point))
		{
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}


BOOL ArmyData::AbleToMakeParkTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++) 
	{
		if(m_array[i].CanMakePark(point))
		{
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}


BOOL ArmyData::AbleToUndergroundRailwayTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++) 
	{
		if(m_array[i].CanUndergroundRailway(point))
		{
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::AbleToEstablishEmbassyTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++) 
	{
		if(m_array[i].CanEstablishEmbassy(point))
		{
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::AbleToCreateFranchiseTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++) 
	{
		if(m_array[i].CanCreateFranchise(point))
		{
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::AbleToAssasinateRulerTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++) 
	{
		if(m_array[i].CanAssasinateRuler(point))
		{
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::AbleToStealTechnologyTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++) 
	{
		if(m_array[i].CanStealTechnology(point))
		{
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::AbleToInjoinTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++) 
	{
		if(m_array[i].CanInjoin(point))
		{
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::AbleToInciteRevolutionTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++) 
	{
		if(m_array[i].CanInciteRevolution(point))
		{
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::AbleToCauseUnhappinessTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++) 
	{
		if(m_array[i].CanCauseUnhappiness(point))
		{
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::AbleToExpelTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++) 
	{
		if(m_array[i].CanExpel(point))
		{
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanExpel() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanExpel() &&
		   m_array[i].CanPerformSpecialAction())
			return TRUE;
	}
	return FALSE;
}

BOOL ArmyData::CanCreatePark(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCreateParks() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanCreatePark() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(!m_array[i].GetDBRec()->GetCreateParks() ||
		   !m_array[i].CanPerformSpecialAction()) {
			return FALSE;
		}
	}
	return TRUE;
}

ORDER_RESULT ArmyData::CreatePark(const MapPoint &point)
{
	sint32 uindex;

	if(!CanCreatePark(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);

	if(!c.IsValid())
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

	if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
	   g_network.IsClient() && g_network.IsLocalPlayer(m_owner))
		if(!VerifyAttack(UNIT_ORDER_CREATE_PARK, point,
						 c.GetOwner()))
			return ORDER_RESULT_ILLEGAL;
	
	
    
    

    InformAI(UNIT_ORDER_CREATE_PARK, point); 

	ActionSuccessful(SPECATTACK_CREATEPARK, m_array[uindex], c);

	
	AddSpecialActionUsed(m_array[uindex]);

	SlicObject *so = new SlicObject("911NaniteCleanseCompleteVictim");
	so->AddRecipient(c.GetOwner());
	so->AddCity(c);
	g_slicEngine->Execute(so);

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_CreateParkUnit,
						   GEA_Unit, m_array[uindex].m_id,
						   GEA_City, c.m_id,
						   GEA_End);
    

	return ORDER_RESULT_SUCCEEDED;
}

BOOL ArmyData::CanPillage(uint32 & uindex) const
{
	MapPoint pos;
	GetPos(pos);
	Cell *cell = g_theWorld->GetCell(pos);

	
	sint32 num_improvements = cell->GetNumDBImprovements();
	if (num_improvements <= 0) {
		
		
		
		
		
		
			return false;
		
	}

	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		const UnitRecord *rec = m_array[i].GetDBRec();
		if(rec->GetMovementTypeAir()) {
			
			continue;
		}

		if(m_array[i].GetDBRec()->GetCanPillage() &&
		   m_array[i].CanPerformSpecialAction())
		{
			uindex = i;
			return true;
		}
	}

	return false;
}

BOOL ArmyData::CanPillage() const
{
	MapPoint pos;
	GetPos(pos);
	Cell *cell = g_theWorld->GetCell(pos);

	sint32 num_improvements = cell->GetNumImprovements();
	if (num_improvements <= 0) {
		uint32 env = cell->GetEnv();
		if(!(env & (k_MASK_ENV_IRRIGATION |
				    k_MASK_ENV_ROAD |
				    k_MASK_ENV_MINE |
					k_MASK_ENV_CANAL_TUNNEL |
					k_MASK_ENV_INSTALLATION))) {
			return false;
		}
	}

	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		const UnitRecord *rec = m_array[i].GetDBRec();
		if(!m_array[i].GetDBRec()->GetCanPillage() ||
		   !m_array[i].CanPerformSpecialAction())
		{
			return false;
		}
	}

	return true;
}

void ArmyData::ThisMeansWAR(PLAYER_INDEX defense_owner)
{
	Assert(g_player[m_owner]);
	if(g_player[m_owner]) {
		g_player[m_owner]->ThisMeansWAR(defense_owner);
	}

	
	
}

ORDER_RESULT ArmyData::Pillage(BOOL test_ownership)
{
	uint32 uindex;
	if (!CanPillage(uindex))
		return ORDER_RESULT_ILLEGAL;

	MapPoint pos;
	MapPoint npos;
	GetPos(pos);
	Cell *cell = g_theWorld->GetCell(pos);
	sint32 cellOwner = cell->GetOwner();

    if (test_ownership && (cellOwner == m_owner)) {
        SlicObject *so = new SlicObject("17IAPillageOwnLand") ;
		so->AddRecipient(m_owner) ;
		so->AddUnit(m_array[0]);
		so->AddCivilisation(m_owner);
		g_slicEngine->Execute(so) ;
        return ORDER_RESULT_ILLEGAL;
    }        
    
	if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
	   (g_network.IsClient() && g_network.IsLocalPlayer(m_owner)))
		if(test_ownership && !VerifyAttack(UNIT_ORDER_PILLAGE_UNCONDITIONALLY, m_pos, cellOwner))
			return ORDER_RESULT_ILLEGAL;



	
    
    
    
    

	InformAI(UNIT_ORDER_PILLAGE, m_pos); 

	AddSpecialActionUsed(m_array[uindex]);

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_PillageUnit,
						   GEA_Unit, m_array[uindex],
						   GEA_End);

    g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_CutImprovements,
						   GEA_MapPoint, pos,
						   GEA_End);

	return ORDER_RESULT_SUCCEEDED;
}


BOOL ArmyData::CanInjoin(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanInjoin() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::CanInjoin() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanInjoin() &&
		   m_array[i].CanPerformSpecialAction()) {
			return TRUE;
		}
	}
	return FALSE;
}

ORDER_RESULT ArmyData::Injoin(const MapPoint &point)
{
	sint32 uindex;

	if(!CanInjoin(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);
	Unit u = m_array[uindex];

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.IsInjoined())
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_array[0].GetOwner())
		return ORDER_RESULT_ILLEGAL;

    InformAI(UNIT_ORDER_INJOIN, point); 

	AddSpecialActionUsed(m_array[uindex]);
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_InjoinUnit,
						   GEA_Unit, m_array[uindex].m_id,
						   GEA_City, c.m_id,
						   GEA_End);
	

	
	ActionSuccessful(SPECATTACK_INJOIN, m_array[uindex], c);
	
	SlicObject *so = new SlicObject("911InjunctionCompleteAttack");
	so->AddRecipient(u.GetOwner());
	so->AddCity(c);
	g_slicEngine->Execute(so);

	return ORDER_RESULT_SUCCEEDED;
}

BOOL ArmyData::GetFirstMoveThisTurn() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(!m_array[i].GetFirstMoveThisTurn())
			return FALSE;
	}
	return TRUE;
}

BOOL ArmyData::HasLeftMap() const
{
	sint32 i;
	
	for(i = 0; i < m_nElements; i++) {
		if(!m_array[i].HasLeftMap())
			return FALSE;
	}
	return TRUE;
}

BOOL ArmyData::CanNukeCity() const
{
	sint32 i;
	
	for(i = 0; i < m_nElements; i++) {
		if( g_theUnitDB->Get(m_array[i].GetType())->GetNuclearAttack() )
			return TRUE;
	}
	return FALSE;
}


void ArmyData::CurMinMovementPoints(double &cur) const
{
    sint32 i;
    double m;
    
    
    cur=10000000.0; 

    for (i=0; i<m_nElements; i++) { 
        m = m_array[i].GetMovementPoints();
        if (m < cur) { 
            cur = m; 
        } 
    } 
}

void ArmyData::MinMovementPoints(double &cur) const
{
    sint32 i;
    double m; 
    
    cur=10000000.0; 

    for (i=0; i<m_nElements; i++) { 
        m = m_array[i].GetMaxMovePoints();
        if (m < cur) { 
            cur = m; 
        } 
    } 
}



BOOL ArmyData::CanBombardTargetType(const CellUnitList & units) const
{
	sint32 i,j;
	
    for (i = m_nElements - 1; i>= 0; i--) 
	{
		for (j = 0; j < units.Num(); j++)
		{
			if ( m_array[i]->CanBombardType(units[j]) == TRUE)
				return TRUE;
		}
	}
	return FALSE;
}



BOOL ArmyData::CanBombard(const MapPoint &point) const
{
	static CellUnitList defender;
	defender.Clear();
	g_theWorld->GetArmy(point, defender);
	sint32 i;
	
	if (defender.Num() < 1)
		return false;

    for (i = m_nElements - 1; i>= 0; i--) { 
		
        if (m_array[i].CanBombard(defender)) 
			return TRUE;
	}
	return FALSE;
}

BOOL ArmyData::CanBombard() const
{
	sint32 i;
	
    for (i = m_nElements - 1; i>= 0; i--) { 
        if( m_array[i].GetDBRec()->GetCanBombard() &&
			m_array[i].CanPerformSpecialAction()) {
            return TRUE;
		}
	}
	return FALSE;
}

BOOL ArmyData::BombardCity(const MapPoint &point, BOOL doAnimations)
{
	Unit c = g_theWorld->GetCell(point)->GetCity();
	sint32 prob;
	bool atLeastOneBombarded = false;
	if(c.m_id != 0) {
		if(c.GetOwner() == m_owner)
			return FALSE;

		if(c.Flag(k_UDF_CANT_BE_ATTACKED))
			return FALSE;

		sint32 i;
		for(i = 0; i < m_nElements; i++) {
			bool ok = false;
			if(!m_array[i].CanPerformSpecialAction()) {
				continue;
			}
			const UnitRecord *rec = m_array[i].GetDBRec();
			if((rec->GetCanBombardLand() || rec->GetCanBombardMountain()) && 
			   c.GetDBRec()->GetMovementTypeLand()) {
				ok = true;
			} else if(rec->GetCanBombardWater() && c.GetDBRec()->GetMovementTypeSea()) {
				ok = true;
			} else if(rec->GetCanBombardSpace() && c.GetDBRec()->GetMovementTypeSpace()) {
				ok = true;
			}

			if(!ok)
				continue;

			if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
			   (g_network.IsClient() && g_network.IsLocalPlayer(m_owner))) {
				if(!VerifyAttack(UNIT_ORDER_BOMBARD, point, c.GetOwner())) {
					return FALSE;
				}
			}

			AddSpecialActionUsed(m_array[i]);
			MapPoint nonConstPos = point;
			
			if(doAnimations) {
				
				g_director->AddAttackPos(m_array[i], nonConstPos);

				
				
				
				BOOL out_of_fuel;
				if(!m_array[i].GetDBRec()->GetMovementTypeAir()) {
					m_array[i].SetMovementPoints(0.0);
				} else {
					m_array[i].DeductMoveCost(k_MOVE_COMBAT_COST, out_of_fuel);
				}
			}
			atLeastOneBombarded = true;

			sint32 r = g_rand->Next(100);
			DPRINTF(k_DBG_GAMESTATE, ("Bombarding 0x%lx: r1 = %d\n", c.m_id, r));

			prob = rec->GetZBRangeAttack() - buildingutil_GetCityWallsDefense(c.GetCityData()->GetImprovements());
			if(prob<0) 
			{
				prob=0;
			}

			if(r < g_theConstDB->BombardDestroyBuildingChance() * prob) {
				c.DestroyRandomBuilding();
			}
			
			if(c.PopCount() > 1 && (g_rand->Next(100) < g_theConstDB->BombardKillPopChance() * prob)) {
				DPRINTF(k_DBG_GAMESTATE, ("Removing one pop from 0x%lx\n", c.m_id));
				c.CD()->ChangePopulation(-1);
			}
			
			
			Diplomat & defending_diplomat = Diplomat::GetDiplomat(c.GetOwner());
			
			
			defending_diplomat.LogViolationEvent(m_owner, PROPOSAL_TREATY_CEASEFIRE);

		}
		if(atLeastOneBombarded)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::Bombard
//
// Description: Attempt to bombard a MapPoint (location).
//
// Parameters : MapPoint
//
// Globals    : g_director				: display manager
//				g_network				: multiplayer manager
//				g_player				: list of active players
//				g_selectedItem			: selected unit or city
//				g_theWorld				: the map
//				
// Returns    : ORDER_RESULT			: attempt success/failure indication
//
// Remark(s)  :	No special effects. 
//
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::Bombard(const MapPoint &orderPoint)
{

	MapPoint point = orderPoint;

	static CellUnitList defender;
	defender.Clear();
	g_theWorld->GetArmy(point, defender);
	sint32 i;
	
	BOOL isSpaceBombard = FALSE;
	if(point == m_pos) {
		
		return ORDER_RESULT_ILLEGAL;
	} else {
		if(!point.IsNextTo(m_pos)) {//rem: this is why you can't bombard from range as in Civ:CTP
			return ORDER_RESULT_ILLEGAL;
		}
	}

    if (defender.Num() < 1) {		
		if(BombardCity(point, TRUE)) {
			return ORDER_RESULT_SUCCEEDED;
		}

		return ORDER_RESULT_ILLEGAL;
	}

	for(i = 0; i < defender.Num(); i++) {
		if(defender[i].Flag(k_UDF_CANT_BE_ATTACKED))
			return ORDER_RESULT_ILLEGAL;
	}

	if(defender.GetOwner() == m_owner) {
		
		return ORDER_RESULT_ILLEGAL;
	}

	if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
	   (g_network.IsClient() && g_network.IsLocalPlayer(m_owner)))
		if(!VerifyAttack(UNIT_ORDER_BOMBARD, point, defender.GetOwner()))
			return ORDER_RESULT_ILLEGAL;

#if defined(ACTIVISION_ORIGINAL) // #01 Inform AI about bombard only if really possible
	PLAYER_INDEX defense_owner = defender.GetOwner();

    
	Diplomat & defending_diplomat = Diplomat::GetDiplomat(defense_owner);

	
	defending_diplomat.LogViolationEvent(m_owner, PROPOSAL_TREATY_CEASEFIRE);

    InformAI(UNIT_ORDER_BOMBARD, point); 
#endif   

    sint32 numAttacks = 0;
	sint32 numAlive = m_nElements;
	BOOL out_of_fuel;
#if !defined(ACTIVISION_ORIGINAL) // #01 Inform AI about bombard only if really possible
    sint32 numPossibleAttacks = 0;
#endif

    for (i = m_nElements - 1; i>= 0; i--) { 
		if(!m_array[i].CanPerformSpecialAction())
			continue;
		
        if (m_array[i].CanBombard(defender)) { 
#if !defined(ACTIVISION_ORIGINAL) // #01 Inform AI about bombard only if really possible
			numPossibleAttacks++;
			if (numPossibleAttacks == 1) {
				// Log attack and inform defender 
				PLAYER_INDEX defense_owner = defender.GetOwner();
				Diplomat & defending_diplomat = Diplomat::GetDiplomat(defense_owner);
				defending_diplomat.LogViolationEvent(m_owner, PROPOSAL_TREATY_CEASEFIRE);
				InformAI(UNIT_ORDER_BOMBARD, point); 
			}
#endif
			if(m_array[i].Bombard(defender, FALSE)) {
				numAttacks++;
#if !defined(ACTIVISION_ORIGINAL)
				// * Added auto-center for bombardment
                if (defender.GetOwner() == g_selected_item->GetVisiblePlayer())
                     g_director->AddCenterMap(point);
#endif				
				g_director->AddAttackPos(m_array[i], point);
				
				AddSpecialActionUsed(m_array[i]);
				
				if(!m_array[i].GetDBRec()->GetMovementTypeAir()) {
					m_array[i].SetMovementPoints(0.0);
				} else {
					m_array[i].DeductMoveCost(k_MOVE_COMBAT_COST, out_of_fuel);
				} 
			}
		}
    }
    
	if(numAttacks <= 0)
		return ORDER_RESULT_ILLEGAL;

	if(numAlive < 1)
		return ORDER_RESULT_SUCCEEDED;

    for (i = 0; i<defender.Num(); i++) { 
        if (defender[i].CanCounterBombard(*this)) { 
            defender[i].Bombard(*this, TRUE); 
        } 
    }
    
    for (i = m_nElements - 1; 0 <= i; i--) { 
        if (m_array[i].GetHP() < 0.999) {
            m_array[i].KillUnit(CAUSE_REMOVE_ARMY_COUNTERBOMBARD, defender.GetOwner());  
        } 
    }
    
    
    for (i = defender.Num() - 1; 0 <= i; i--) { 
        if (defender[i].GetHP() < 0.999) {
            defender[i].KillUnit(CAUSE_REMOVE_ARMY_BOMBARD, GetOwner());  
        } 
    }

	BombardCity(point, FALSE);
	return ORDER_RESULT_SUCCEEDED;
}


BOOL ArmyData::CanInterceptTrade(uint32 &uindex) const
{
	sint32 i;
    for (i = m_nElements - 1; i>= 0; i--) { 
		
        if (m_array[i].CanInterceptTrade())
		{
			uindex = i;

			return TRUE;
		}
	}
	return FALSE;
}


//----------------------------------------------------------------------------
//
// Name       : ArmyData::InterceptTrade
//
// Description: Attempt to pirate a trade route.
//
// Parameters : -
//
// Globals    : g_director				: display manager
//				g_network				: multiplayer manager
//				g_player				: list of active players
//				g_selectedItem			: selected unit or city
//				g_theSoundDB			: sound database
//				g_theSpecialEffectDB	: special effect database
//				g_theWorld				: the map
//				
// Returns    : ORDER_RESULT			: attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

ORDER_RESULT ArmyData::InterceptTrade()
{
	sint32 i;
	sint32 j;
	sint32 typeIndex = g_theSpecialEffectDB->FindTypeIndex("SPECEFFECT_PIRATE");
	sint32 effectId = g_theSpecialEffectDB->Get(typeIndex)->GetValue();
	sint32 soundId = g_theSoundDB->FindTypeIndex("SOUND_ID_SLAVE_RAIDS");

    for (i = m_nElements - 1; i>= 0; i--) { 
		
        if (m_array[i].CanInterceptTrade() &&
			m_array[i].CanPerformSpecialAction())
			{
				if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
				   (g_network.IsClient() && g_network.IsLocalPlayer(m_owner))) {
					Cell *cell = g_theWorld->GetCell(m_pos);
					for(j = 0; j < cell->GetNumTradeRoutes(); j++) {
						sint32 route_owner = cell->GetTradeRoute(j).GetOwner();

						














						if (AgreementMatrix::s_agreements.HasAgreement(
							route_owner, 
							m_owner, 
							PROPOSAL_OFFER_STOP_PIRACY))
						{
							char turnBuf[32];
							
							SlicObject *so = new SlicObject("12IABreakNoPiracy");
							so->AddRecipient(m_owner);
							so->AddCivilisation(m_owner);
							so->AddCivilisation(route_owner );
							so->AddUnit(m_array[i]);
							so->AddLocation(m_pos);
							so->AddOrder(UNIT_ORDER_INTERCEPT_TRADE);
							so->AddAction(turnBuf);
							g_slicEngine->Execute(so);
							g_selected_item->ForceDirectorSelect(Army(m_id));
							return ORDER_RESULT_ILLEGAL;
						}
					}
				}

#if defined(ACTIVISION_ORIGINAL)
				InformAI(UNIT_ORDER_INTERCEPT_TRADE, m_pos); 
				g_director->AddSpecialEffect(m_pos, effectId, soundId);
				ORDER_RESULT res = m_array[i].InterceptTrade();
				if(res != ORDER_RESULT_ILLEGAL) {
					AddSpecialActionUsed(m_array[i]);
					m_isPirating = true;
				}
#else
				ORDER_RESULT const	res	= m_array[i].InterceptTrade();

				if (res == ORDER_RESULT_ILLEGAL)
				{
					// No action: nothing has happened yet.
				}
				else
				{
					InformAI(UNIT_ORDER_INTERCEPT_TRADE, m_pos); 
					if (g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(m_pos)) 
					{
						g_director->AddCenterMap(m_pos);
                        g_director->AddSpecialEffect(m_pos, effectId, soundId);
					}	
					AddSpecialActionUsed(m_array[i]);
					m_isPirating = true;
				}
#endif

				return res;
			}
	}
	return ORDER_RESULT_ILLEGAL;
}






sint32 ArmyData::NumOrders() const
{
	return m_orders->GetCount();
}

const Order *ArmyData::GetOrder(sint32 index) const
{

	if(index < 0 || index >= m_orders->GetCount())
		return NULL;

	sint32 c = 0;
	PointerList<Order>::Walker walk(m_orders);

	for(; walk.IsValid() && c < index; walk.Next()) {
		c++;
	}
	Assert(c == index);
	if(c == index) {
		return walk.GetObj();
	}
	return NULL;
}

void ArmyData::AddOrders(UNIT_ORDER_TYPE order, Path *path)
{
	AddOrders(order, path, m_pos, 0);
}

void ArmyData::AddOrders(UNIT_ORDER_TYPE order, const MapPoint &point)
{
	AddOrders(order, NULL, point, 0);
}

void ArmyData::AddOrders(UNIT_ORDER_TYPE order)
{
	AddOrders(order, NULL, m_pos, 0);
}

void ArmyData::AutoAddOrders(UNIT_ORDER_TYPE order, Path *path, 
							 const MapPoint &point, sint32 argument)
{
	
	
	
	
	
	

	if(!g_useOrderQueues)
		ClearOrders();

	m_orders->AddTail(new Order(order, path, point, argument));
	StopPirating();

	if(m_owner >= 0 && m_owner < k_MAX_PLAYERS && g_player[m_owner]) {
		ExecuteOrders(false);
	}
}
	
void ArmyData::AutoAddOrdersWrongTurn(UNIT_ORDER_TYPE order, Path *path,
									  const MapPoint &point, sint32 argument)
{
	
	
	
	
	ClearOrders();

	m_orders->AddTail(new Order(order, path, point, argument));
	StopPirating();

	if(m_owner >= 0 && m_owner < k_MAX_PLAYERS && g_player[m_owner]) {
		ExecuteOrders(false);
	}
}

void ArmyData::AddOrders(UNIT_ORDER_TYPE order, Path *path, const MapPoint &point,
						 sint32 argument, GAME_EVENT passedEvent)
{
	bool execute = true;

	
	
	
	
	

	
	
	
	if(g_network.IsActive() && g_network.IsLocalPlayer(m_owner) &&
	   !g_network.IsMyTurn() && g_selected_item->GetCurPlayer() == m_owner &&
	   g_selected_item->GetVisiblePlayer() == m_owner) {
		if(path)
			delete path;
		Assert(FALSE);
		return;
	}

	
	
	
	
	Order *curOrder = m_orders->GetHead();

#ifdef _DEBUG
	if(path && !path->IsEnd()) {
		MapPoint p;
		path->GetCurrentPoint(p);
		Assert(p.x >= 0);
	}
#endif

	if((Order::IsSpecialAttack(order) || order == UNIT_ORDER_ADD_EVENT) &&
	   curOrder && 
	   (curOrder->m_order == UNIT_ORDER_MOVE) &&
	   (curOrder->m_path) &&
	   ((point == m_pos) ||
	    (point.IsNextTo(curOrder->m_path->GetEnd())) ||
		(point == curOrder->m_path->GetEnd()))) {

		Order *attackOrder = new Order(UNIT_ORDER_ADD_EVENT, NULL, point, argument);
		GameEventArgList *args = new GameEventArgList();
		
		args->Add(new GameEventArgument(GEA_Army, m_id));
		args->Add(new GameEventArgument(GEA_MapPoint, point));
		args->Add(new GameEventArgument(GEA_Int, argument));
		
		if(order == UNIT_ORDER_ADD_EVENT) {
			attackOrder->m_eventType = passedEvent;
		} else {
			attackOrder->m_eventType = Order::OrderToEvent(order);
		}
		Assert(attackOrder->m_eventType < GEV_MAX && attackOrder->m_eventType >= 0);
		DPRINTF(k_DBG_GAMESTATE, ("Adding event order for army 0x%lx, event=%d\n", m_id, attackOrder->m_eventType));
		attackOrder->m_gameEventArgs = args;
		
		m_orders->AddTail(attackOrder);
		execute = false;
	} else {
		if(g_network.IsActive() && order == UNIT_ORDER_ADD_EVENT) {
			
		} else {
			ClearOrders();
		}


		
		
		
		
		
		m_orders->AddTail(new Order(order, path, point, argument));
		if(order == UNIT_ORDER_ADD_EVENT) {
			Order *o = m_orders->GetTail();
			o->m_eventType = passedEvent;
			GameEventArgList *args = new GameEventArgList();
		
			args->Add(new GameEventArgument(GEA_Army, m_id));
			args->Add(new GameEventArgument(GEA_MapPoint, point));
			args->Add(new GameEventArgument(GEA_Int, argument));
			o->m_gameEventArgs = args;
			
			DPRINTF(k_DBG_GAMESTATE, ("Added explicit event order for army 0x%lx, event=%d\n", m_id, o->m_eventType));
		}
	}

	
	StopPirating();

	Order *o = m_orders->GetTail();
	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetOrder(m_owner, Army(m_id),
									   o->m_order, 
									   o->m_path, 
									   o->m_point, 
									   o->m_argument, 
									   o->m_eventType));
		g_network.Unblock(m_owner);
	} else if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendOrder(m_owner, 
							Army(m_id),
							o->m_order, 
							o->m_path, 
							o->m_point, 
							o->m_argument,
							o->m_eventType);
	}

	if(order == UNIT_ORDER_VICTORY_MOVE) {
		if((!g_network.IsActive() || g_network.IsLocalPlayer(m_owner))) {
			
			
			
			ExecuteOrders();
		}
	} else {
		
		if(execute && 
		   m_owner >= 0 && 
		   m_owner < k_MAX_PLAYERS && 
		   g_player[m_owner]) {
			
			
			if(m_owner == g_selected_item->GetCurPlayer() || 
			   (m_nElements == 1 && m_array[0].Flag(k_UDF_MAD_LAUNCHED))) {
				if(1 ) {
					
					if(!g_network.IsActive() || g_network.IsLocalPlayer(m_owner)) {
						ExecuteOrders();
					}
				} else {
					
					if(!g_network.IsActive() || 
					   (g_network.IsLocalPlayer(m_owner) && g_player[m_owner]->m_playerType != PLAYER_TYPE_ROBOT)) {
						ExecuteOrders();
					}
				}
			}
		}
	}
}

void ArmyData::ClearOrders()
{
	if(m_orders->GetHead()) {
		DPRINTF(k_DBG_GAMESTATE, ("Clearing orders for army %lx\n", m_id));
		
		if(g_network.IsHost()) {
			g_network.Block(m_owner);
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_CLEAR_ORDERS,
										  (uint32)m_id));
			g_network.Unblock(m_owner);
		} else if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
			g_network.SendAction(new NetAction(NET_ACTION_CLEAR_ORDERS,
											   (uint32)m_id));
		}
		m_orders->DeleteAll();
	}
}

BOOL ArmyData::ExecuteOrders(bool propagate)
{
	
	
	Assert(m_hasBeenAdded);
	if(!m_hasBeenAdded)
		return FALSE;

	m_dontKillCount++;
	CheckAddEventOrder();

	
	Assert(m_dontKillCount);
	if(m_dontKillCount) {
		m_dontKillCount--;

		
		if(m_needToKill && !m_dontKillCount) {
			Army me(m_id);
			me.Kill();

			
			return FALSE;
		}
	}

	if(!m_orders->GetHead()) {
		return FALSE;
	}
	m_flags |= k_CULF_EXECUTED_THIS_TURN;

	Order *order;
	BOOL completedOrder = FALSE;
	BOOL keepGoing = TRUE;
	sint32 owner = m_owner;
	Army me(m_id);
	if(m_orders->GetHead()->m_order != UNIT_ORDER_EXPEL_TO) {
		
		
		
		
		if(g_network.IsActive() && m_owner == g_selected_item->GetVisiblePlayer() &&
		   !g_network.IsMyTurn()) {
		   return FALSE;
		}
	}

	m_dontKillCount++;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		if(propagate) {
			g_network.SendAction(new NetAction(NET_ACTION_EXECUTE_ORDERS,
											   (uint32)m_id));
		}
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		UNIT_ORDER_TYPE type = m_orders->GetHead()->m_order;
		
		
			
		
		
		
		if(propagate) {
			if(m_orders->GetHead()->m_order != UNIT_ORDER_EXPEL_TO) {
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_EXECUTE_ORDERS,
											  (uint32)m_id));
			}
		}
	}

	DPRINTF(k_DBG_GAMESTATE, ("Army 0x%lx Executing order %d @ (%d,%d), turn=%d\n", m_id, m_orders->GetHead()->m_order, m_pos.x, m_pos.y, g_player[m_owner]->m_current_round));

	while(keepGoing && m_nElements > 0 &&
		  (order = m_orders->GetHead()) != NULL) {

        

		switch(order->m_order) {
			case UNIT_ORDER_MOVE:
			case UNIT_ORDER_MOVE_TO:
			case UNIT_ORDER_MOVE_THEN_UNLOAD:
			case UNIT_ORDER_VICTORY_MOVE:
				completedOrder = ExecuteMoveOrder(order);
				if(!completedOrder)
					keepGoing = FALSE;
				break;
			case UNIT_ORDER_ENTRENCH:
				Entrench();
				completedOrder = TRUE;
				break;
			case UNIT_ORDER_SLEEP:
				Sleep();
				completedOrder = TRUE;
				break;
			case UNIT_ORDER_DETRENCH:
				Detrench();
				completedOrder = TRUE;
				break;
			case UNIT_ORDER_UNLOAD:
			case UNIT_ORDER_UNLOAD_ONE_UNIT:
			case UNIT_ORDER_UNLOAD_SELECTED_STACK:
				completedOrder = ExecuteUnloadOrder(order);
				if(!completedOrder)
					keepGoing = FALSE;
				break;
			case UNIT_ORDER_TELEPORT_TO:
			case UNIT_ORDER_EXPEL_TO:
				
				
				
				order->m_order = UNIT_ORDER_TELEPORT_TO;
				completedOrder = ExecuteTeleportOrder(order);
				break;
			case UNIT_ORDER_GROUP:
				GroupAllUnits();
				completedOrder = TRUE;
				break;
			case UNIT_ORDER_UNGROUP:
				UngroupUnits();
				completedOrder = TRUE;
				break;
			case UNIT_ORDER_GROUP_UNIT:
				GroupUnit(Unit(order->m_argument));
				completedOrder = TRUE;
				break;
			case UNIT_ORDER_INVESTIGATE_CITY:
			case UNIT_ORDER_NULLIFY_WALLS:
			case UNIT_ORDER_STEAL_TECHNOLOGY:
			case UNIT_ORDER_ASSASSINATE:
			case UNIT_ORDER_INVESTIGATE_READINESS:
			case UNIT_ORDER_BOMBARD:
			case UNIT_ORDER_SUE:
			case UNIT_ORDER_FRANCHISE:
			case UNIT_ORDER_SUE_FRANCHISE:
			case UNIT_ORDER_EXPEL:
			case UNIT_ORDER_ESTABLISH_EMBASSY:
			case UNIT_ORDER_THROW_PARTY:
			case UNIT_ORDER_CAUSE_UNHAPPINESS:
			case UNIT_ORDER_PLANT_NUKE:
			case UNIT_ORDER_SLAVE_RAID:
			case UNIT_ORDER_ENSLAVE_SETTLER:
			case UNIT_ORDER_UNDERGROUND_RAILWAY:
			case UNIT_ORDER_INCITE_UPRISING:
			case UNIT_ORDER_BIO_INFECT:
			case UNIT_ORDER_PLAGUE:
			case UNIT_ORDER_NANO_INFECT:
			case UNIT_ORDER_CONVERT:
			case UNIT_ORDER_REFORM:
			case UNIT_ORDER_INDULGENCE:
			case UNIT_ORDER_SOOTHSAY:
			case UNIT_ORDER_CREATE_PARK:
			case UNIT_ORDER_PILLAGE:
			case UNIT_ORDER_INJOIN:
			case UNIT_ORDER_INTERCEPT_TRADE:
			case UNIT_ORDER_PARADROP_MOVE:
			case UNIT_ORDER_INCITE_REVOLUTION:
			case UNIT_ORDER_PILLAGE_UNCONDITIONALLY:
			case UNIT_ORDER_ADVERTISE:
			case UNIT_ORDER_LAUNCH:
			case UNIT_ORDER_TARGET:
				completedOrder = ExecuteSpecialOrder(order, keepGoing);
				break;
			case UNIT_ORDER_SET_UNLOAD_MOVEMENT_POINTS:
				completedOrder = TRUE;
				SetUnloadMovementPoints();
				break;
			case UNIT_ORDER_DISBAND:
				completedOrder = TRUE;
				Disband();
				break;
			case UNIT_ORDER_SETTLE:
				completedOrder = TRUE;
				Settle();
				break;
			case UNIT_ORDER_FINISH_ATTACK:
				completedOrder = TRUE;
				FinishAttack(order);
				break;
			case UNIT_ORDER_BOARD_TRANSPORT:
				completedOrder = TRUE;
				DoBoardTransport(order);
				break;
			case UNIT_ORDER_WAKE_UP:
				completedOrder = TRUE;
				WakeUp();
				break;
			case UNIT_ORDER_ADD_EVENT:
				completedOrder = FALSE;
				keepGoing = FALSE;
				
				
				
				
				break;
			default:
				Assert(FALSE);
		}

        #ifdef _DEBUG
            if (g_theDiplomacyLog) { 
                g_theDiplomacyLog->PopRegardRequest(); 
            } 
        #endif _DEBUG

		if(!g_theArmyPool->IsValid(me)) {
			keepGoing = FALSE;
		} else {
			if(completedOrder) {
				delete m_orders->RemoveHead();
#ifdef NETWORK_PARANOID
				if(g_network.IsClient() && m_nElements > 0 && !m_needToKill) {
					g_network.SendAction(new NetAction(NET_ACTION_VERIFY_POS,
													   m_id, 
													   (sint32)m_pos.x,
													   (sint32)m_pos.y));
				}
#endif
			}
			completedOrder = FALSE;
		}
	}
	if(g_network.IsHost()) {
		g_network.Unblock(owner);
		
	}
	
	KillRecord *kill;
	while((kill = m_killMeSoon->RemoveHead()) != NULL) {
		if(g_theUnitPool->IsValid(kill->m_unit)) {
			kill->m_unit.Kill(kill->m_cause, kill->m_who);
		}
		delete kill;
	}

	if(m_dontKillCount) {
		m_dontKillCount--;
		if(m_needToKill && !m_dontKillCount) {
			me.Kill();
		}
	}

	
	PLAYER_INDEX player;
	ID id;
	SELECT_TYPE type;

	if ( g_selected_item ) {
		g_selected_item->GetTopCurItem(player, id, type);
		if ( id == me ) {
			g_selected_item->Refresh();
		}
	}

	return TRUE;

	
	
#if 0
	if(g_theArmyPool->IsValid(me)) {
		return m_orders->GetHead() != NULL;
	} else {
		return FALSE;
	}
#endif

}

void ArmyData::InformAI(const UNIT_ORDER_TYPE order_type, const MapPoint &pos)
{
	
	const Unit city = g_theWorld->GetCity(pos);

	if (city.m_id == 0x0 || !g_theUnitPool->IsValid(city))
		return;

	sint32 cost;
	Diplomat & city_diplomat = Diplomat::GetDiplomat(city.GetOwner());
	switch (order_type)
	{
	case UNIT_ORDER_STEAL_TECHNOLOGY:
		city_diplomat.GetCurrentDiplomacy(m_owner).GetCreateParkRegardCost(cost);

		
		StringId strId;
		g_theStringDB->GetStringID("REGARD_EVENT_STEAL_TECHNOLOGY", strId);
		city_diplomat.LogRegardEvent( m_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	
	city_diplomat.LogViolationEvent(m_owner, PROPOSAL_TREATY_RESEARCH_PACT);
	break;
	}
}

void ArmyData::ResumePatrol()
{
}

void ArmyData::ForgetPatrol()
{
}

BOOL ArmyData::ExecuteMoveOrder(Order *order)
{
	WORLD_DIRECTION d;

	BOOL isParatrooperMove = FALSE;
	if(m_nElements == 1 && m_array[0].GetDBRec()->GetParatrooperTransport()) {
		isParatrooperMove = TRUE;
	}

	bool did_move_at_all = false;
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(!m_array[i].GetFirstMoveThisTurn()) {
			did_move_at_all = true;
			break;
		}
	}

	if ((order->m_order == UNIT_ORDER_MOVE) ||
        (order->m_order == UNIT_ORDER_MOVE_THEN_UNLOAD)) {
		
		if(order->m_path->IsEndDir() ||
			(order->m_order == UNIT_ORDER_MOVE_THEN_UNLOAD && order->m_point.IsNextTo(m_pos))) {
			if(order->m_order == UNIT_ORDER_MOVE_THEN_UNLOAD && order->m_point.IsNextTo(m_pos)) {
				return ExecuteUnloadOrder(order);
			}
			return TRUE;
		}

		order->m_path->GetCurrentDir(d);
			
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
							   GEV_MoveArmy,
							   GEA_Army, Army(m_id),
							   GEA_Direction, d,
							   GEA_Int, order->m_order,
							   GEA_Int, order->m_argument,
							   GEA_MapPoint, order->m_point,
							   GEA_End);
		return FALSE;
	} else {
		if(m_pos == order->m_point)
			return TRUE;

		d = m_pos.GetNeighborDirection(order->m_point);
		if(d == NOWHERE) {
			if(g_network.IsClient()) {
				g_network.RequestResync(RESYNC_ARMY_POS);
			} else if(g_network.IsHost() && !g_network.IsLocalPlayer(m_owner)) {
				g_network.Resync(m_owner);
			}
			return FALSE;
		}
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
							   GEV_MoveArmy,
							   GEA_Army, m_id,
							   GEA_Direction, d,
							   GEA_Int, order->m_order,
							   GEA_Int, order->m_argument,
							   GEA_MapPoint, order->m_point,
							   GEA_End);
		

		
		
		
		

		return FALSE;
	}
}

BOOL ArmyData::IsOccupiedByForeigner(const MapPoint &pos)
{
	Cell *cell = g_theWorld->GetCell(pos);

	if(cell->GetNumUnits() < 1)
		return FALSE;

	if(cell->UnitArmy()->GetOwner() != m_owner)
		return TRUE;

	return FALSE;
}

void ArmyData::CheckLoadSleepingCargoFromCity(Order *order)
{
	sint32 i,j;
	Cell *cell = g_theWorld->GetCell(m_pos);
	if(	cell->GetCity().m_id == 0 &&
		terrainutil_HasAirfield(m_pos) == false)
		return;
	
	if(cell->UnitArmy()->Num() < 1)
		
		return;

	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].GetCargoCapacity() > 0) {
			for(j = cell->UnitArmy()->Num() - 1; j >= 0; j--) {
				Unit u = cell->UnitArmy()->Access(j);
				if(!u.IsAsleep())
					continue;
				if(m_array[i].CanCarry(u)) {
					BOOL out_of_fuel;
					u.SetIsInTransport(m_array[i]);
					u.DeductMoveCost(k_MOVE_ENTER_TRANSPORT_COST, out_of_fuel);
					g_theWorld->RemoveUnitReference(m_pos, u);
					u.UndoVision();
					u.RemoveUnitVision();
					m_array[i].InsertCargo(u);
					ID *tmp_id = (ID*)(&m_array[i].GetArmy());
					g_player[m_owner]->RegisterInsertCargo(tmp_id, u.GetType(),
														   (sint32)u.GetHP());
				}
				if(m_array[i].GetCargoCapacity() < 1)
					break;
			}
		}
	}
}

					
BOOL ArmyData::Move(WORLD_DIRECTION d, Order *order)
{
	MapPoint newPos, oldPos;
	oldPos = m_pos;

	sint32 r = oldPos.GetNeighborPosition(d, newPos);

	if(!r)
		return TRUE;

	if(CheckSpecialUnitMove(newPos)) {
		return TRUE;
	}

	CheckLoadSleepingCargoFromCity(order);

	if(IsMovePointsEnough(newPos)) {
		if(IsOccupiedByForeigner(newPos)) {
			
			CellUnitList *defender = g_theWorld->GetCell(newPos)->UnitArmy();

			
			
			sint32 i;
			for(i = 0; i < defender->Num(); i++) {
				if(defender->Access(i).Flag(k_UDF_CANT_BE_ATTACKED))
					return FALSE;
			}

			if(!CheckWasEnemyVisible(newPos)) {

				g_player[m_owner]->ContactMade(defender->GetOwner());

				if(g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
					if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
						
						return FALSE;
					}

					






















				} else {
					g_selected_item->ForceDirectorSelect(Army(m_id));
					return FALSE;
				}
			}
			



























			if(order->m_order == UNIT_ORDER_MOVE) {
				
				
				while(!order->m_path->IsEndDir())
					order->m_path->IncDir();

				
				
				
				if(!order->m_argument)
					return FALSE;

				
				
				
				if(newPos != order->m_point)
					return FALSE;
			}

			if(order->m_order == UNIT_ORDER_MOVE_THEN_UNLOAD)
				return FALSE;

			for(i = 0; i < m_nElements; i++) {
				if(m_array[i].Flag(k_UDF_FOUGHT_THIS_TURN) ||
				   m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN)) {
					
					return FALSE;
				}
			}

			if(m_orders->GetHead() == order) {
				m_orders->RemoveHead();
			}
			if(g_network.IsHost()) {
				g_network.Block(m_owner);
				g_network.SyncRand();
				g_network.Unblock(m_owner);
			}
			if(g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
				
				
				
				sint32 defOwner = defender->GetOwner();
				if(g_player[m_owner]->WillViolateCeaseFire(defOwner) ||
				   g_player[m_owner]->WillViolatePact(defOwner)) {
					sint32 oldAlliance = g_player[m_owner]->m_broken_alliances_and_cease_fires;
					g_player[m_owner]->BreakAlliance(defOwner);
					g_player[m_owner]->BreakCeaseFire(defOwner, oldAlliance == g_player[m_owner]->m_broken_alliances_and_cease_fires);
				}
			}

			AutoAddOrders(UNIT_ORDER_FINISH_ATTACK, NULL, newPos, 0);
			return FALSE;
		}

		if(DoLeaveOurLandsCheck(newPos, UNIT_ORDER_MOVE_TO)) {
			
			
			return FALSE;
		}

		BOOL finished = FinishMove(d, newPos, order);

		return finished;
	}
	return FALSE;
}

BOOL ArmyData::FinishMove(WORLD_DIRECTION d, MapPoint &newPos, Order *order)
{
	static CellUnitList transports;
	
	if(g_theWorld->GetCity(newPos).m_id == 0 && CanMoveIntoTransport(newPos, transports)) {
		m_didMove = MoveIntoTransport(newPos, transports);
		return m_didMove;
	} else {
		m_didMove = MoveIntoCell(newPos, order->m_order, d);

		if(m_didMove) {
			sint32 i;
			for(i = 0; i < m_nElements; i++) {
				g_slicEngine->RunUnitMovedTriggers(m_array[i]);
			}
		}

		if(m_didMove) {
			sint32 i;
			for(i = m_nElements - 1; i >= 0; i--) {
				uint32 moveType = m_array[i].GetMovementType();
				if(!g_theWorld->CanEnter(newPos, moveType)) {
					if((moveType & k_BIT_MOVEMENT_TYPE_SHALLOW_WATER) &&
					   !(moveType & k_BIT_MOVEMENT_TYPE_WATER) &&
					   g_theWorld->GetCell(newPos)->GetEnv() & k_BIT_MOVEMENT_TYPE_WATER &&
					   wonderutil_GetAllBoatsDeepWater(g_player[m_owner]->m_builtWonders)) {
						
					} else {
						if(g_theWorld->GetCity(newPos).m_id == 0 ||
						   g_theWorld->GetCity(newPos).GetOwner() != m_owner) {
							m_array[i].Kill(CAUSE_REMOVE_ARMY_ILLEGAL_CELL, -1);
						}
					}
				}
			}
		}

		return m_didMove;
	}
}

BOOL ArmyData::FinishAttack(Order *order)
{
	sint32 i;

	
    
	BOOL res = MoveIntoForeigner(order->m_point);
	if(!res)
		return TRUE;
	
	if(m_nElements <= 0)
		return TRUE;
	

	
	
	

    

	for(i = 0; i < m_nElements; i++) {
		if(!m_array[i].GetDBRec()->GetMovementTypeAir()) {
			m_array[i].SetMovementPoints(0.0);
		}
	}
	
	

	WORLD_DIRECTION d = m_pos.GetNeighborDirection(order->m_point);
	return FinishMove(d, order->m_point, order);
}

BOOL ArmyData::CheckSpecialUnitMove(const MapPoint &pos)
{
    sint32 j;
	SlicObject	*so ;

	Unit city = g_theWorld->GetCell(pos)->GetCity();

	if(city.IsValid()) {
		if(city.GetOwner() != m_owner) {
			for(sint32 i = 0; i < m_nElements; i++) {
				
				
				
				if(g_theUnitDB->Get(m_array[i].GetType())->GetNuclearAttack()) {

					if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
					   (g_network.IsClient() && g_network.IsLocalPlayer(m_owner)))
						if(!VerifyAttack(UNIT_ORDER_MOVE_TO, pos,
										 city.GetOwner()))
							return TRUE;

					
					
					

					if(city.SafeFromNukes()) {
						so = new SlicObject("10gSafeFromNukes") ;
						so->AddRecipient(city.GetOwner()) ;
						so->AddCivilisation(city.GetOwner()) ;
						so->AddCity(city) ;
						g_slicEngine->Execute(so) ;

						so = new SlicObject("11gSafeFromNukes") ;
						so->AddRecipient(GetOwner()) ;
						so->AddCivilisation(city.GetOwner()) ;
						so->AddCity(city) ;
						g_slicEngine->Execute(so) ;

						
						g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
											   GEA_Unit, m_array[i],
											   GEA_Int, CAUSE_REMOVE_ARMY_NUKE,
											   GEA_Player, -1,
											   GEA_End);
						
						return TRUE;
					}
					
					
                    if (g_slicEngine->GetSegment("49WorldPollutionNuclearWar")->TestLastShown(m_owner, 10)) {
                        so = new SlicObject("49WorldPollutionNuclearWar") ;
						so->AddCity(city);
                        for (j=1; j<k_MAX_PLAYERS; j++)
                            if (g_player[j] && !g_player[j]->m_isDead)
                                so->AddRecipient(j) ;
                        g_slicEngine->Execute(so) ;
                    }

					g_player[m_owner]->AdjustEventPollution(m_array[i].GetDBRec()->GetDeathPollution());

					

					g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_NukeCityUnit,
										   GEA_Unit, m_array[i],
										   GEA_City, city.m_id,
										   GEA_End);
										   
#if 0
					static UnitDynamicArray killList;
					killList.Clear();

                    
                    Assert(0 != city.m_id);



					
					city.GetNuked(killList);
					
					for(j = 0; j < killList.Num(); j++) {
						if(killList[j] != m_array[i]) { 
							if(killList[j].DeathEffectsHappy()) {
								g_player[killList[j].GetOwner()]->RegisterLostUnits(1, pos, DEATH_EFFECT_CALC);
							}
						}
						killList[j].Kill(CAUSE_REMOVE_ARMY_NUKE, GetOwner());
					}
#endif
					return TRUE;
				}
				
				
				
			}
			return FALSE;
		} else {
			return FALSE;
		}
	}

	CellUnitList *defender = g_theWorld->GetCell(pos)->UnitArmy();
	if(defender && defender->Num() > 0) {
		Assert(defender->Access(0).IsValid());
		if(defender->Access(0).IsValid() && defender->Access(0).GetOwner() != m_owner) {
			for(sint32 i = 0; i < m_nElements; i++) {
				
				
				
				if(g_theUnitDB->Get(m_array[i].GetType())->GetNuclearAttack()) {

					if(!CheckWasEnemyVisible(pos)) {
						Order *order = m_orders->GetHead();

						if(order && order->m_order == UNIT_ORDER_MOVE) {
							
							
							while(!order->m_path->IsEndDir())
								order->m_path->IncDir();
						}

						return TRUE;
					}

					if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
					   (g_network.IsClient() && g_network.IsLocalPlayer(m_owner)))
						if(!VerifyAttack(UNIT_ORDER_MOVE_TO, pos,
										 defender->GetOwner()))
							return TRUE;

					
					
					

					
                    if (g_slicEngine->GetSegment("49WorldPollutionNuclearWar")->TestLastShown(m_owner, 10)) {
                        so = new SlicObject("49WorldPollutionNuclearWar") ;
                        for (j=1; j<k_MAX_PLAYERS; j++)
                            if (g_player[j] && !g_player[j]->m_isDead)
                                so->AddRecipient(j) ;
                        g_slicEngine->Execute(so) ;
                    }

					g_player[m_owner]->AdjustEventPollution(m_array[i].GetDBRec()->GetDeathPollution());

					




					g_thePollution->AddNukePollution(pos);

					g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_NukeLocationUnit,
										   GEA_Unit, m_array[i],
										   GEA_MapPoint, pos,
										   GEA_End);
					return TRUE;
				}
				
				
				
			}
		}
	}

	return FALSE;
}

BOOL ArmyData::MoveIntoForeigner(const MapPoint &pos)
{
	static CellUnitList defender;
	defender.Clear();
	g_theWorld->GetArmy(pos, defender);

	if(defender.Num() <= 0)
		return FALSE;

	BOOL i_died = FALSE;

	PLAYER_INDEX defense_owner = defender.GetOwner();
	if (IsEnemy(defense_owner) && 
		!g_player[m_owner]->WillViolateCeaseFire(defense_owner) &&
		!g_player[m_owner]->WillViolatePact(defense_owner) &&
		CanFight(defender)) { 

		
		
		InformAI(UNIT_ORDER_FINISH_ATTACK, pos); 
		
		
		
		
		
		sint32 numCloaked = 0;
		for(sint32 i = 0; i < defender.Num(); i++) {
			if(defender.m_array[i].IsCloaked())
				numCloaked++;
		}
		if(numCloaked >= defender.Num() || m_array[0].AccessData()->PacMan()) {
			defender.KillList(CAUSE_REMOVE_ARMY_UNKNOWN, GetOwner());
		} else {

			bool allSingleUse = true;
			sint32 i;
			for(i = 0; i < m_nElements; i++) {
				if(!m_array[i].GetDBRec()->GetSingleUse()) {
					allSingleUse = false;
					break;
				}
			}

			
			Army me(m_id);
			if(!allSingleUse) {
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
									   GEV_Battle,
									   GEA_Army, me,
									   GEA_MapPoint, pos,
									   GEA_End);
			}

			
			for(i = 0; i < m_nElements; i++) {
				if(m_array[i].GetDBRec()->GetSingleUse()) {
					static CellUnitList defender;
					defender.Clear();
					g_theWorld->GetArmy(pos, defender);					

					MapPoint nonConstPos = pos;
					g_director->AddAttackPos(m_array[i], nonConstPos);
					m_array[i].Bombard(defender, FALSE);

					for(sint32 j = 0; j < defender.Num(); j++) {
						if(defender[j].GetHP() < 0.5) {
							g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
												   GEV_KillUnit,
												   GEA_Unit, defender[j].m_id,
												   GEA_Int, CAUSE_REMOVE_ARMY_BOMBARD,
												   GEA_Player, m_owner,
												   GEA_End);
						}
					}

					g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
										   GEV_KillUnit,
										   GEA_Unit, m_array[i].m_id,
										   GEA_Int, CAUSE_REMOVE_ARMY_SINGLE_USE,
										   GEA_Player, -1,
										   GEA_End);										   
				}
			}

			
			return FALSE;
		}
	} else {
		VerifyAttack(UNIT_ORDER_MOVE_TO, pos, defense_owner);
		return FALSE;
	}
	return TRUE;
}

BOOL ArmyData::VerifyAttack(UNIT_ORDER_TYPE order, const MapPoint &pos,
							sint32 defense_owner)
{
	if(IsEnemy(defense_owner) &&
	   !g_player[m_owner]->WillViolateCeaseFire(defense_owner) &&
	   !g_player[m_owner]->WillViolatePact(defense_owner))
		return TRUE;
	   
	SlicObject *so;
	if(g_network.IsActive() && g_network.TeamsEnabled() &&
	   g_player[m_owner]->m_networkGroup == g_player[defense_owner]->m_networkGroup) {
		so = new SlicObject("110aCantAttackTeammates");
	} else if(!IsEnemy(defense_owner)) {
		so = new SlicObject("110CantAttackAllies");
	} else {
		so = new SlicObject("110bCantAttackHaveTreaty");
	}

	so->AddRecipient(m_owner);
	so->AddCivilisation(defense_owner);
	so->AddUnit(m_array[0]);
	so->AddLocation(pos);
	so->AddOrder(order);
	g_slicEngine->Execute(so);
	g_selected_item->ForceDirectorSelect(Army(m_id));
	return FALSE;
}

BOOL ArmyData::ExertsZOC() const
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(!m_array[i].IsNoZoc())
			return TRUE;
	}
	return FALSE;
}

void ArmyData::UpdateZOCForMove(const MapPoint &pos, WORLD_DIRECTION d)
{
	Cell *cell = g_theWorld->GetCell(m_pos);
	sint32 i;
	Army me(m_id);
	BOOL doneRemoving = FALSE;

	if(cell->GetCity().m_id != 0) {
		doneRemoving = TRUE;
	} else {
		for(i = cell->UnitArmy()->Num() - 1; i >= 0; i--) {
			
			
			if(cell->AccessUnit(i).GetArmy() != me &&
			   !cell->AccessUnit(i).IsNoZoc()) {
				doneRemoving = TRUE;
				break;
			}
		}
	}

	uint8 dirs;
	static DynamicArray<MapPoint> points;
	sint32 dd;
	MapPoint chk;
	if(!doneRemoving) {
		
		switch(d) {
			case NORTH:      dirs = W_F | SW_F | S_F | SE_F | E_F; break;
			case NORTHEAST:  dirs = W_F | SW_F | S_F; break;
			case EAST:       dirs = N_F | NW_F | W_F | SW_F | S_F; break;
			case SOUTHEAST:  dirs = N_F | NW_F | W_F; break;
			case SOUTH:      dirs = W_F | NW_F | N_F | NE_F | E_F; break;
			case SOUTHWEST:  dirs = N_F | NE_F | E_F; break;
			case WEST:       dirs = N_F | NE_F | E_F | SE_F | S_F; break;
			case NORTHWEST:  dirs = S_F | SE_F | E_F; break;
		}

		
		points.Clear();
		for(dd = 0; dd < (sint32)NOWHERE; dd++) {
			if(dirs & (1 << dd)) {
				if(m_pos.GetNeighborPosition((WORLD_DIRECTION)dd, chk)) {
					g_theWorld->RemoveZOC(chk, m_owner);
					points.Insert(chk);
				}
			}
		}

		
		
		sint32 mp;
		for(mp = points.Num() - 1; mp >= 0; mp--) {
			g_theWorld->AddOtherArmyZOC(points[mp], m_owner, me, Unit(0));
		}
	}

	if(!g_theWorld->HasCity(pos) || g_theWorld->GetCity(pos).GetOwner() != m_owner) {
		
		switch(d) {
		case SOUTH:      dirs = W_F | SW_F | S_F | SE_F | E_F; break;
		case SOUTHWEST:  dirs = W_F | SW_F | S_F; break;
		case WEST:       dirs = N_F | NW_F | W_F | SW_F | S_F; break;
		case NORTHWEST:  dirs = N_F | NW_F | W_F; break;
		case NORTH:      dirs = W_F | NW_F | N_F | NE_F | E_F; break;
		case NORTHEAST:  dirs = N_F | NE_F | E_F; break;
		case EAST:       dirs = N_F | NE_F | E_F | SE_F | S_F; break;
		case SOUTHEAST:  dirs = S_F | SE_F | E_F; break;
		}
		
		for(dd = 0; dd < (sint32)NOWHERE; dd++) {
			if(dirs & (1 << dd)) {
				if(pos.GetNeighborPosition((WORLD_DIRECTION)dd, chk)) {
					if(CanEnter(chk)) {
						g_theWorld->AddZOC(chk, m_owner);
					}
				}
			}
		}
	}
}

void ArmyData::RevealZOCUnits(const MapPoint &pos)
{
	static UnitDynamicArray units;
	units.Clear();
	MapPoint left;
	g_theWorld->GetAdjacentUnits(units, pos);
	MapPoint hisPos;
	for(sint32 i = units.Num() - 1; i >= 0; i--) {
		if(!units[i].IsNoZoc() && units[i].GetOwner() != m_owner) {
			if(units[i].GetVisibility() & (1 << m_owner))
				return;
			units[i].GetPos(hisPos);
			units[i].ForceVisibleThisTurn(m_owner);
			units[i].GetActor()->SetUnitVisibility(units[i].GetVisibility());
			return;
		}
	}
}

BOOL ArmyData::MoveIntoCell(const MapPoint &pos, UNIT_ORDER_TYPE order, WORLD_DIRECTION d)
{
	BOOL zocViolation;
	BOOL alliedCity;

	BOOL ignoreZoc = (order == UNIT_ORDER_FINISH_ATTACK || order == UNIT_ORDER_VICTORY_MOVE);

	if(!CanMoveIntoCell(pos, zocViolation, ignoreZoc, alliedCity)) {
		if(zocViolation && 
		   g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_HUMAN) {
			RevealZOCUnits(pos);
			if(m_owner == g_selected_item->GetVisiblePlayer()) {
				g_selected_item->ForceDirectorSelect(Army(m_id));
			}
		}

		m_zocViolation = zocViolation;

		if(alliedCity) {
			
			Unit city = g_theWorld->GetCity(pos);
			VerifyAttack(UNIT_ORDER_MOVE_TO, pos, city.GetOwner());
			return FALSE;
		}

		
		if (g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
			
			
			CtpAi::HandleMoveFailure(Army(m_id), pos); 
		}

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ClearOrders,
							   GEA_Army, m_id,
							   GEA_End);
		return FALSE;
	}

	static UnitDynamicArray revealedUnits;
	BOOL revealedUnexplored = FALSE;
	revealedUnits.Clear();

	static UnitDynamicArray diedInMove;
	diedInMove.Clear();

	if(ExertsZOC()) {
		UpdateZOCForMove(pos, d);
	}

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MoveUnits,
						   GEA_Army, m_id,
						   GEA_MapPoint, m_pos,
						   GEA_MapPoint, pos,
						   GEA_End);

	
	
	
	return TRUE;
}

void ArmyData::MoveActors(const MapPoint &pos, 
						  UnitDynamicArray &revealedUnits,
						  BOOL teleport)
{
	BOOL theTileIsVisible = g_tiledMap->TileIsCompletelyVisible(pos.x, pos.y);

	
	if (g_selected_item->GetPlayerOnScreen() != -1 &&
		g_selected_item->GetPlayerOnScreen() != g_selected_item->GetVisiblePlayer())
			return;

	Unit top_src = GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());    
	if (top_src.m_id == 0)
		top_src = m_array[0];

	sint32 numRevealed = revealedUnits.Num();

	
	UnitActor **restOfStack; 
	sint32 numRest = 0;

	Unit nonDead(0);

	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(!m_array[i].Flag(k_UDF_TELEPORT_DEATH)) {
			if(nonDead.m_id == 0) {
				nonDead.m_id = m_array[i].m_id;
			}
			if(m_array[i].m_id != top_src.m_id) {
				numRest++;
			}
		}
	}
	if(!top_src.IsValid() || top_src.Flag(k_UDF_TELEPORT_DEATH)) {
		top_src = nonDead;
		numRest--;
	}

	if(numRest < 0) {
		
		return;
	}

	if (numRest > 0) {
		sint32 n = 0;
		restOfStack = new (UnitActor* [numRest]);  
		for(i = 0; i < m_nElements; i++) {
			if(!m_array[i].Flag(k_UDF_TELEPORT_DEATH) && m_array[i].m_id != top_src.m_id) {
				restOfStack[n++] = m_array[i].GetActor();
			}
		}
		
	}

	
	UnitActor	**revealedActors;
	
	if (numRevealed > 0) {
		revealedActors = new UnitActor*[numRevealed];
		for (sint32 i=0; i<numRevealed; i++) {
			revealedActors[i] = revealedUnits[i].GetActor();
		}
	} else {
		revealedActors = NULL;
	}
	
	MapPoint newPos = pos; 
	
	if (teleport || !(top_src.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()))) {
		g_director->AddTeleport(top_src, m_pos, newPos, numRevealed, revealedActors, 
								numRest, restOfStack);
	} else {
		g_director->AddMove(top_src, m_pos, newPos, numRevealed, revealedActors, 
							numRest, restOfStack, FALSE, top_src.GetMoveSoundID());  
		
	}
	
	
	if (top_src.GetData()->HasLeftMap())
		g_director->AddHide(top_src);
}

void ArmyData::MoveUnits(const MapPoint &pos)
{
	sint32 i, r; 

	WakeUp();

	
	static UnitDynamicArray revealedUnits;
	BOOL revealedUnexplored = FALSE;
	revealedUnits.Clear();

	MapPoint oldPos = m_pos;
	bool anyVisible = false;
	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].IsEntrenching() || m_array[i].IsEntrenched()) {
			Detrench();
		}

		anyVisible = anyVisible || (m_array[i].GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()));

		if(g_theWorld->GetCell(pos)->GetNumUnits() >= k_MAX_ARMY_SIZE) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
								   GEA_Unit, m_array[i],
								   GEA_Int, CAUSE_REMOVE_ARMY_EXPELLED_NO_CITIES,
								   GEA_Player, -1,
								   GEA_End);
			m_array[i].SetFlag(k_UDF_TELEPORT_DEATH);
		} else {
			g_theWorld->RemoveUnitReference(m_pos, m_array[i]);
			r = m_array[i].MoveToPosition(pos, revealedUnits, revealedUnexplored);

			
			if(revealedUnits.Num() > 0 && !m_array[i].GetDBRec()->GetSingleUse()) {
#ifdef _DEBUG
				DPRINTF(k_DBG_GAMESTATE, ("Army %lx revealed %d units during move:\n", m_id, revealedUnits.Num()));
				sint32 uu;
				for(uu = 0; uu < revealedUnits.Num(); uu++) {
					DPRINTF(k_DBG_GAMESTATE, ("  Unit %lx\n", revealedUnits[uu].m_id));
				}
#endif
				if((!g_network.IsActive() && g_player[m_owner]->m_playerType == PLAYER_TYPE_HUMAN)) {
					ClearOrders();
				} else if(g_network.IsHost()) {
					
					
					if(!g_network.IsLocalPlayer(m_owner) || m_owner == g_selected_item->GetVisiblePlayer()) {
						ClearOrders();
					}
				} else if(g_network.IsClient()) {
					
					
					if(g_network.IsLocalPlayer(m_owner)) {
						ClearOrders();
					}
				}
			}
			if(m_array[i].HasLeftMap()) {
				
				Assert(FALSE);
				
				
				
				
				
				
				
				
				
			}
		}
	}
	if(anyVisible && g_radarMap) {
		g_radarMap->RedrawTile(&oldPos);
		g_radarMap->RedrawTile(&m_pos);
	}

	if(HasLeftMap()) {
		g_selected_item->RegisterRemovedArmy(m_owner, Army(m_id));
	}
	DeductMoveCost(pos);

	m_pos = pos;
}

void ArmyData::CheckTerrainEvents()
{
	
	Cell *cell = g_theWorld->GetCell(m_pos);
	if(cell->GetGoodyHut()) {
		cell->GetGoodyHut()->OpenGoody(m_owner, m_pos);
	
        if (cell->GetGoodyHut()) { 
            cell->DeleteGoodyHut();
        }

		
		MapPoint pos2;
		g_tiledMap->RedrawTile(&m_pos);
		for(WORLD_DIRECTION d = NORTH; d < NOWHERE; d = (WORLD_DIRECTION)((sint32)d + 1)) {
			if(m_pos.GetNeighborPosition(d, pos2)) {
				g_tiledMap->RedrawTile(&pos2);
			}
		}

		if(g_network.IsHost()) {
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_REMOVE_HUT,
										  m_pos.x, m_pos.y));
		}
	}
}

BOOL ArmyData::CanMoveIntoTransport(const MapPoint &pos,
									CellUnitList &transports) const
{
	if(!g_theWorld->GetEmptyTransports(pos, transports))
		return FALSE;

	return CanMoveIntoThisTransport(transports);
}


sint32 ArmyData::NumUnitsCanMoveIntoThisTransport(const CellUnitList &transports) const
{
	sint32 i, j;
	sint32 count[k_MAX_ARMY_SIZE];
	memset(count, 0, k_MAX_ARMY_SIZE * sizeof(sint32));
	for(i = 0; i < transports.Num(); i++) {
		count[i] = transports.Get(i).GetCargoCapacity();
	}

	sint32 cargo_fits = 0;
	sint32 searching;
    for (i = 0; i<m_nElements; i++) {       
        searching = TRUE; 
        for (j=0; searching && (j<transports.Num()); j++) { 
            if ((0 < count[j]) && transports.Get(j).CanCarry(m_array[i])) {
                count[j]--; 
                searching = FALSE; 
            }
        }
        
        if (!searching) { 
            cargo_fits++; 
        } 
    } 
    
    return cargo_fits; 
}

BOOL ArmyData::CanMoveIntoThisTransport(const CellUnitList &transports) const
{
	sint32 i, j;
	sint32 count[k_MAX_ARMY_SIZE];
	memset(count, 0, k_MAX_ARMY_SIZE * sizeof(sint32));
	for(i = 0; i < transports.Num(); i++) {
		count[i] = transports.Get(i).GetCargoCapacity();
	}

	sint32 searching;
    for (i = 0; i<m_nElements; i++) {       
        searching = TRUE; 
        for (j=0; searching && (j<transports.Num()); j++) { 
            if ((0 < count[j]) && transports.Get(j).CanCarry(m_array[i])) {
                count[j]--; 
                searching = FALSE; 
            }
        }
        
        if (searching) { 
            return FALSE; 
        } 
    } 
    
    return TRUE; 
}

BOOL ArmyData::MoveIntoTransport(const MapPoint &pos, CellUnitList &transports)
{
	

	
	
	m_dontKillCount++;

    sint32 i, j;
    sint32 n=0; 
    UnitActor **move_actor; 
    move_actor = new (UnitActor* [m_nElements + 1]);  
    UnitActor *top_src_ptr = NULL;
    UnitActor *act = NULL;
    Unit top_src; 
    BOOL found_top_src = FALSE; 
    
    top_src = GetTopVisibleUnit(g_selected_item->GetVisiblePlayer()); 
    if(top_src.m_id != 0) { 
        top_src_ptr = top_src.GetActor(); 
    } else {
		top_src = m_array[0];
		top_src_ptr = m_array[0].GetActor();
	}
    
	BOOL out_of_fuel;

    
    sint32 count = 0; 
    for (i = m_nElements - 1; i>= 0; i--) {       
        for (j=0; j < transports.Num(); j++) { 
            if (transports[j].CanCarry(m_array[i]) &&
				transports[j].GetCargoCapacity() > 0) { 
                count++; 
                
                act = m_array[i].GetActor(); 
                if (act == top_src_ptr) {                     
                    found_top_src = TRUE;
                } else { 
                    move_actor[n++] = act;
                }
                
                m_array[i].SetIsInTransport(transports[j]);
				m_array[i].DeductMoveCost(k_MOVE_ENTER_TRANSPORT_COST, out_of_fuel);

                g_theWorld->RemoveUnitReference(m_pos, m_array[i]);
				m_array[i].UndoVision();
				m_array[i].RemoveUnitVision();
				
				
                transports[j].InsertCargo(m_array[i]); 
				
				g_theArmyPool->AccessArmy(transports[j].GetArmy())->WakeUp();

				if (g_soundManager && top_src.m_id != 0) {
						sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
						if ((visiblePlayer == top_src.GetOwner()) ||
							(top_src.GetVisibility() & (1 << visiblePlayer))) {
							if(transports[j].GetLoadSoundID() >= 0) {
								g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
												transports[j].GetLoadSoundID(),
												top_src.RetPos().x,
												top_src.RetPos().y);
							}
						}
				}

				if(g_network.IsHost()) {
					g_network.Block(m_owner);
					g_network.Enqueue(transports[j].AccessData());
					g_network.Unblock(m_owner);
				}

                ID *tmp_id = (ID*)(&transports[j].GetArmy());
				
                
				break;
            }
        }
    }

	if(!count) {
		m_dontKillCount--;
		return TRUE;
	}
    
	
	UnitActor	**revealedActors = NULL;
	sint32		numRevealed = 0;

	
	UnitActor **restOfStack; 
	sint32 numRest = m_nElements - 1;

	if (numRest > 0) {
		restOfStack = new (UnitActor* [numRest]);  
		GetActors(top_src, restOfStack);
	}

	MapPoint directorDoesntLikeConsts = pos;
	g_director->AddMove(top_src, m_pos, directorDoesntLikeConsts, numRevealed, revealedActors,
							numRest, restOfStack, TRUE, top_src.GetMoveSoundID());

    m_isTransported = TRUE;

	UpdateZOCForRemoval();

	if(m_dontKillCount) {
		m_dontKillCount--;
		if(m_needToKill && !m_dontKillCount) {
			Army me(m_id);
			me.Kill();
		}
	}


	return TRUE;
}

void ArmyData::DoBoardTransport(Order *order)
{
	static CellUnitList transports;
	transports.Clear();
	if(CanMoveIntoTransport(m_pos, transports)) {
		MoveIntoTransport(m_pos, transports);
	}
}




BOOL ArmyData::CanAtLeastOneCargoUnloadAt(const MapPoint & old_pos, const MapPoint & dest_pos, const BOOL & used_vision) const
{
    sint32 unit_idx; 
    sint32 unit_num = m_nElements;

    for (unit_idx=0; unit_idx<unit_num; unit_idx++) { 
        if (m_array[unit_idx].CanAtLeastOneCargoUnloadAt(old_pos, dest_pos,  used_vision)) {
            return TRUE; 
        }
    }

    return FALSE; 
}

BOOL ArmyData::ExecuteUnloadOrder(Order *order)
{
	MapPoint from_pt, to_pt;

    
    if (order->m_order == UNIT_ORDER_MOVE_THEN_UNLOAD) {
        from_pt = m_pos;
		to_pt = order->m_point;
        
        
    } else {
        from_pt = m_pos;
        to_pt = order->m_point;
    }

   if ((from_pt.x == to_pt.x) &&
       (from_pt.y == to_pt.y)) {
		
		
	    
       
       
       
		
   } else  { 

       if(!from_pt.IsNextTo(to_pt)) { 
	        return TRUE;                    
       }
   }

   
   
   if(DoLeaveOurLandsCheck(to_pt, UNIT_ORDER_UNLOAD)) {
	   return TRUE;
   }

    sint32 i; 
    
    sint32 max_debark = k_MAX_ARMY_SIZE; 
    CellUnitList *a = g_theWorld->GetArmyPtr(to_pt); 
    if (0 < a->Num()) { 
        if (m_owner == a->GetOwner()) { 
            max_debark = k_MAX_ARMY_SIZE - a->Num(); 
            if (max_debark == 0) { 
                return TRUE; 
            }
        }
    }
    
	Army debark = g_player[m_owner]->GetNewArmy(CAUSE_NEW_ARMY_TRANSPORTED);

	BOOL unitUnloadDone = FALSE;
    for (i=0; i<m_nElements; i++) { 
		if(order->m_order == UNIT_ORDER_UNLOAD_ONE_UNIT) {
			unitUnloadDone = m_array[i].UnloadCargo(to_pt, debark, TRUE, Unit(order->m_argument)) || unitUnloadDone;
		} else if(order->m_order == UNIT_ORDER_UNLOAD ||
			      order->m_order == UNIT_ORDER_MOVE_THEN_UNLOAD) {
			unitUnloadDone = m_array[i].UnloadCargo(to_pt, debark, FALSE, Unit(0)) || unitUnloadDone;
		} else if(order->m_order == UNIT_ORDER_UNLOAD_SELECTED_STACK) {
			unitUnloadDone = m_array[i].UnloadSelectedCargo(to_pt, debark) || unitUnloadDone;
		} else {
			Assert(FALSE);
		}
    }
	
	
	
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_FinishUnload,
						   GEA_Army, m_id,
						   GEA_Army, debark,
						   GEA_MapPoint, to_pt,
						   GEA_End);
						   
	return unitUnloadDone;
}
    
void ArmyData::FinishUnloadOrder(Army &debark, MapPoint &to_pt)
{
	if(debark.Num() <= 0) {
		
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		if ((visiblePlayer == m_array[0].GetOwner()) || 
			(m_array[0].GetVisibility() & (1 << visiblePlayer))) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
								m_array[0].GetCantMoveSoundID(),
								to_pt.x,
								to_pt.y);
		}

		debark.Kill();
	} else {
	    
        sint32 i, n = debark.Num(); 

	    if(IsOccupiedByForeigner(to_pt)) {
		    for(i = 0; i < debark.Num(); i++) {
			    if((!debark[i]->Flag(k_UDF_BEACH_ASSAULT_LEGAL))) {
					CheckWasEnemyVisible(to_pt);

				    
				    BOOL inserted = FALSE;
				    for(sint32 j = 0; j < m_nElements && !inserted; j++) {
					    inserted = m_array[j].InsertCargo(debark[i]);
						if(inserted) {
							Unit u = debark[i];
							u.GetInserted(m_array[j]);
							ID *tmp_id = (ID*)(&m_id);
							g_player[m_owner]->RegisterInsertCargo(tmp_id, u.GetType(),
																   (sint32)u.GetHP());
							break;
						}
				    }
				    
				    
				    Assert(inserted);
				    debark.SetRemoveCause(CAUSE_REMOVE_ARMY_UNKNOWN);
				    debark.DelIndex(i);
				    i--;
			    }
		    }
		} else if(g_theWorld->GetCity(to_pt).m_id != 0 && 
			      g_theWorld->GetCity(to_pt).GetOwner() != m_owner) {
			if(!g_theArmyPool->AccessArmy(debark)->CanAtLeastOneCaptureCity()) {
				for(i = debark.Num() - 1; i >= 0; i--) {
					BOOL inserted = FALSE;
					for(sint32 j = 0; j < m_nElements && !inserted; j++) {
					    inserted = m_array[j].InsertCargo(debark[i]);
						if(inserted) {
							debark[i].GetInserted(m_array[j]);
							ID *tmp_id = (ID*)(&m_id);
							g_player[m_owner]->RegisterInsertCargo(tmp_id, debark[i].GetType(),
																   (sint32)debark[i].GetHP());
							break;
						}
				    }
				    
				    
				    Assert(inserted);
				    debark.SetRemoveCause(CAUSE_REMOVE_ARMY_UNKNOWN);
				    debark.DelIndex(i);
				}
			}
		}

	    if(0 < debark.Num()) {
		    if (g_soundManager) {
			    sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			    if ((visiblePlayer == debark[0].GetOwner()) || 
				    (debark[0].GetVisibility() & (1 << visiblePlayer))) {
				    g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
									    m_array[0].GetUnloadSoundID(),
									    to_pt.x,
									    to_pt.y);
			    }
		    }
	    }
		if (0 < debark.Num()) { 
			
			
			if(g_theArmyPool->IsValid(debark)) {
				
				
				
				sint32 i;
				for(i = 0; i < debark.Num(); i++) {
					if(!debark[i]->Flag(k_UDF_BEACH_ASSAULT_LEGAL) &&
					   !debark[i].GetMovementTypeAir()) {
						g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SetUnloadMovementUnit,
											   GEA_Unit, debark[i].m_id,
											   GEA_End);
					}
				}
				
				
				
				m_debarked = debark;
				
			}

            if(m_pos != to_pt) {
				sint32 i;
				for(i = 0; i < debark.Num(); i++) {
					if (debark[i].GetActor()) {
						debark[i].GetActor()->Hide();
						debark[i].GetActor()->PositionActor(m_pos);
						g_director->AddShow(debark[i]);
					}
				}
				debark.AutoAddOrders(UNIT_ORDER_MOVE_TO, NULL, to_pt, 0);
			} else {
				sint32 i;
				for(i = 0; i < debark.Num(); i++) {
					g_director->AddShow(debark[i]);
				}
				debark.AutoAddOrders(UNIT_ORDER_TELEPORT_TO, NULL, m_pos, 0);
			}
			
		}
	}
}

void ArmyData::DeductMoveCost(const MapPoint &pos)
{
	sint32 i;
	double cost = g_theWorld->GetMoveCost(pos);
	double c;
	BOOL out_of_fuel;

	for(i = m_nElements - 1; i >= 0; i--) {
		if(m_array[i].GetMovementTypeAir()) {
			c = k_MOVE_AIR_COST;
		} else if(g_theWorld->IsTunnel(pos)) {
			if(!m_array[i].GetMovementTypeLand()) {
#if defined(ACTIVISION_ORIGINAL)
				c = g_theWorld->GetTerrain(pos)->GetEnvBase()->GetMovement();
#else
				sint32	icost;
				(void) g_theWorld->GetTerrain(pos)->GetEnvBase()->GetMovement(icost);
				c = icost;
#endif
			} else {
				c = cost;
			}
		}else if(m_array[i].Flag(k_UDF_FOUGHT_THIS_TURN)) {
			c = m_array[i].GetMovementPoints();
		}else {
			c = cost;
		}

		out_of_fuel = FALSE;
		m_array[i].DeductMoveCost(c, out_of_fuel);
	}
}

sint32 ArmyData::Fight(CellUnitList &defender)
{  
	Assert(defender.Num() > 0);
	Assert(m_nElements > 0);
	if(defender.Num() < 1 || m_nElements < 1) {
		return FALSE;
	}

	
	
	
	m_dontKillCount++;

	m_didBattle = TRUE;
	sint32 i;

	for(i = 0; i < m_nElements; i++) {
		m_array[i].SetFlag(k_UDF_FOUGHT_THIS_TURN);
	}

	for(i = 0; i < defender.Num(); i++) {
		defender[i].SetFlag(k_UDF_FOUGHT_THIS_TURN);
	}

    
    
	

    sint32 n_start_attackers = m_nElements;
    
    PLAYER_INDEX attack_owner, defense_owner; 
    attack_owner = GetOwner(); 
    defense_owner = defender.GetOwner(); 
    MapPoint attack_pos, defense_pos; 
    GetPos(attack_pos); 
    defender.GetPos(defense_pos); 
    
    ForceVisibleThisTurn(defense_owner); 
    defender.ForceVisibleThisTurn(attack_owner); 
    
    Unit ta = GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());

	
	if (ta.m_id == 0) {
		ta = m_array[0];
	}

    Unit td = defender.GetTopVisibleUnit(g_selected_item->GetVisiblePlayer()); 
    
	
	if (td.m_id == 0) {
		td = defender[0];
	}

    if ((ta.m_id != 0) && (td.m_id != 0)) { 
		if (ta.GetActor()) {
			UnitActor *actor = ta.GetActor();


			if (!actor->HasThisAnim(UNITACTION_ATTACK)) {
				
				
				for (i=0; i<m_nElements; i++) {
					if (m_array[i].GetActor() && 
						m_array[i].GetActor()->HasThisAnim(UNITACTION_ATTACK)) {
						ta = m_array[i];
						break;
					}
				}
			}
		}

		
        
		


		g_slicEngine->RunAttackTriggers(ta, td);
    } else
		if (ta.m_id != 0) {
			if (g_theUnitPool->IsValid(defender[0]))
				g_director->AddAttackPos(ta, defender[0].RetPos());
		}

    if (ta.m_id != 0)
		ta.SetFlag(k_UDF_WAS_TOP_UNIT_BEFORE_BATTLE);

	if (td.m_id != 0)
		td.SetFlag(k_UDF_WAS_TOP_UNIT_BEFORE_BATTLE);

    

    MapPoint pos; 

	
    

	g_player[attack_owner]->MakeConvertedCitiesUnhappy(defense_owner);

    defender.GetPos(pos); 

	
	if (defender.GetOwner() == g_selected_item->GetVisiblePlayer())
		g_director->AddCenterMap(pos);

    Unit c = g_theWorld->GetCity(pos);
        
	if((g_turn->IsHotSeat() || g_turn->IsEmail() || g_theProfileDB->GetUseAttackMessages()) &&
	   g_player[defender.GetOwner()]->GetPlayerType() == PLAYER_TYPE_HUMAN) {
		SlicObject *so = NULL;
		if(g_theUnitPool->IsValid(c)) {
			so = new SlicObject("410HotseatCityAttacked");
			so->AddCity(c);
		} else {
			so = new SlicObject("411HotseatArmyAttacked");
		}

		so->AddRecipient(defender.GetOwner());
		so->AddCivilisation(m_owner);
		so->AddLocation(pos);
		so->AddUnit(ta);
		so->AddUnit(td);
		g_slicEngine->Execute(so);
	}

    double defenders_bonus = 0.0;
    
    if (c.m_id != (0)) { 
        defenders_bonus = c.GetDefendersBonus(); 
    }
    
    double amr = 1.0 / GetHPModifier(); 
    double dmr = 1.0 / defender.GetHPModifier(); 

	bool defenderSucks = true;
	for(i = 0; i < defender.Num(); i++) {
		if(defender[i].GetAttack() > 0) {
			defenderSucks = false;
			break;
		}
	}

	if(defenderSucks) {
		
		if(ta.IsValid()) {
			g_director->AddAttack(ta, td);
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
								   GEV_BattleAftermath,
								   GEA_Army, m_id,
								   GEA_MapPoint, pos,
								   GEA_Unit, ta,
								   GEA_Unit, td,
								   GEA_Player, m_owner,
								   GEA_Player, defense_owner,
								   GEA_Int, 0, 
								   GEA_End);
			for(i = 0; i < defender.Num(); i++) {				
				CAUSE_REMOVE_ARMY cause = CAUSE_REMOVE_ARMY_DIED_IN_ATTACK;
				if(defender[i].m_id == td.m_id) {					
					cause  = CAUSE_REMOVE_ARMY_DIED_IN_ATTACK_ON_TOP;
				}
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
									   GEV_KillUnit,
									   GEA_Unit, defender[i].m_id,
									   GEA_Int, cause,
									   GEA_Player, m_owner,
									   GEA_End);
			}
		}
	} else {
		
		g_director->IncrementPendingGameActions();

		
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
							   GEV_RunCombat,
							   GEA_Army, m_id,
							   GEA_MapPoint, pos,
							   GEA_Player, attack_owner,
							   GEA_Player, defense_owner,
							   GEA_End);

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
							   GEV_StartCombat,
							   GEA_Army, m_id,
							   GEA_MapPoint, pos,
							   GEA_End);
		
	}

	Assert(m_dontKillCount);
	if(m_dontKillCount) {
		m_dontKillCount--;
		if(m_needToKill && !m_dontKillCount) {
			Army me(m_id);
			me.Kill();
			return FALSE;
		}
	}

	return FALSE;
}

void ArmyData::UpdateZOCForRemoval()
{
	CellUnitList *units = g_theWorld->GetCell(m_pos)->UnitArmy();
	sint32 i;
	
	
	
	if(units && units->GetOwner() == m_owner) {
		for(i = 0; i < units->Num(); i++) {
			if(units->Access(i).GetArmy().m_id != m_id &&
			   !units->Access(i).IsNoZoc()) {
				return;
			}
		}
	}

	g_theWorld->RemoveZOC(m_pos, m_owner);
	g_theWorld->AddOtherArmyZOC(m_pos, m_owner, Army(m_id), Unit(0));
	
	sint32 dd;
	for(dd = 0; dd < (sint32)NOWHERE; dd++) {
		MapPoint npos;
		if(m_pos.GetNeighborPosition((WORLD_DIRECTION)dd, npos)) {
			g_theWorld->RemoveZOC(npos, m_owner);
			g_theWorld->AddOtherArmyZOC(npos, m_owner, Army(m_id), Unit(0));
		}
	}
}

BOOL ArmyData::ExecuteTeleportOrder(Order *order)
{
	static UnitDynamicArray revealedUnits;
	BOOL revealedUnexplored = FALSE;
	revealedUnits.Clear();

	
	UpdateZOCForRemoval();

	MoveUnits(order->m_point);

	
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		m_array[i].UpdateZOCForInsertion();
	}

	m_revealedForeignUnits = revealedUnits.Num() > 0;
	m_revealedUnexplored = revealedUnexplored;
	MoveActors(order->m_point, revealedUnits, TRUE);	
	m_pos = order->m_point;

	CheckTerrainEvents();  

    
    
    MapPoint norm_pos; 
    norm_pos.Iso2Norm(order->m_point); 






	return TRUE;
}

void ArmyData::ResetPos()
{
	if(m_nElements <= 0)
		return;
	m_array[0].GetPos(m_pos);
#ifdef _DEBUG
	sint32 i;
	for(i = 1; i < m_nElements; i++) {
		MapPoint pos;
		m_array[i].GetPos(pos);
		Assert(pos == m_pos);
	}
#endif
}

void ArmyData::GetAdvanceFromCityAssault(const Unit &c, 
										 PLAYER_INDEX otherPlayer)
{
	sint32 num;
	uint8 *canAskFor = g_player[m_owner]->m_advances->
		CanAskFor(g_player[otherPlayer]->m_advances, num);

	if(num > 0) {
		sint32 which = g_rand->Next(num);
		sint32 checked = 0;
		sint32 i;
		for(i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
			if(canAskFor[i]) {
				if(checked == which) {
					g_player[m_owner]->m_advances->GiveAdvance(i, CAUSE_SCI_COMBAT);
					SlicObject *so = new SlicObject("99AdvanceFromCapturingCity");
					so->AddCivilisation(otherPlayer);
					so->AddRecipient(m_owner);
					so->AddCity(c);
					so->AddAdvance(i);
					g_slicEngine->Execute(so);
					break;
				}
				checked++;
			}
		}
	}

	delete [] canAskFor;
}

void ArmyData::IndicateAdded()
{
	m_hasBeenAdded = TRUE;
}

PLAYER_INDEX ArmyData::GetKiller() const
{
	return m_killer;
}

void ArmyData::SetKiller(PLAYER_INDEX who)
{
	m_killer = who;
}

void ArmyData::AddDeath(const Unit &unit, CAUSE_REMOVE_ARMY cause,
						PLAYER_INDEX who)
{
	m_killMeSoon->AddTail(new KillRecord(unit, cause, who));
}

void ArmyData::SetUnloadMovementPoints()
{
	sint32 i;
	for (i=0; i<m_nElements; i++) { 
		if (!m_array[i].CanBeachAssault() &&
			!m_array[i].GetMovementTypeAir()) { 
			m_array[i].SetMovementPoints(0.0); 
			if(g_network.IsHost()) {
				g_network.Block(m_owner);
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_MOVEMENT_TO_ZERO, (uint32)m_array[i]));
				g_network.Unblock(m_owner);
			}
		}
	}
}

void ArmyData::Disband()
{
	
	if(g_player[m_owner]->m_all_armies->Num() < 2 &&
	   g_player[m_owner]->m_all_cities->Num() < 1)
		return;

	sint32 i;
	Unit city = g_theWorld->GetCity(m_pos);
	for(i = m_nElements - 1; i >= 0; i--) {
		
		if(city.m_id != (0)) {
			city.AccessData()->GetCityData()->AddShields(m_array[i].GetDBRec()->GetShieldCost() / 2);
		}
		m_array[i].Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
	}

	
	if ( g_selected_item->GetSelectedCity(city) ) {
		
	}
}


sint32 ArmyData::GetMinFuel()
{
    sint32 i, minFuel, currFuel; 
    
	minFuel = currFuel = 0x7fffffff; 

    for ( i = 0; i < m_nElements; i++ ) { 
		if(!m_array[i].GetDBRec()->GetNoFuelThenCrash())
			continue;
		currFuel = m_array[i].GetFuel();
        if (currFuel < minFuel) {
			minFuel = currFuel;
        }
    }
    
    return minFuel;
}

void ArmyData::CalcRemainingFuel(sint32 &num_tiles_to_half, sint32 &num_tiles_to_empty) const
{

    num_tiles_to_half = 1000000; 
    num_tiles_to_empty = 1000000; 

    sint32 unit_idx; 
    sint32 fuel_remaining; 
    sint32 max_fuel; 
    sint32 fuel_to_half; 
    for (unit_idx = 0; unit_idx < m_nElements; unit_idx++ ) { 

        if (!m_array[unit_idx].GetUsedFuel (fuel_remaining, max_fuel)) continue; 

        fuel_to_half = fuel_remaining - max_fuel / 2; 
        
        if (fuel_to_half < num_tiles_to_half) { 
#if defined(ACTIVISION_ORIGINAL)
            num_tiles_to_half = max (0, fuel_to_half); 
#else
			num_tiles_to_half = std::max<sint32>(fuel_to_half, 0);
#endif
        }

        if (fuel_remaining < num_tiles_to_empty) { 
            num_tiles_to_empty = fuel_remaining; 
        }
    }
}




BOOL ArmyData::CanMove() 
{
	sint32 i;
	BOOL noneMoved = TRUE;
	BOOL allOverSpecialCost = TRUE;
	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].GetMovementPoints() < 1) {
			
			return FALSE;
		}
		if(!m_array[i].GetFirstMoveThisTurn())
			noneMoved = FALSE;
		if(!m_array[i].CanPerformSpecialAction())
			allOverSpecialCost = FALSE;
	}
	if(noneMoved) {
		
		return TRUE;
	}
	if(allOverSpecialCost) {
		
		
		
		
		return TRUE;
	}
	
	
	
	sint32 d;
	MapPoint neighbor;
	for(d = 0; d < (sint32)NOWHERE; d++) {
		if(m_pos.GetNeighborPosition((WORLD_DIRECTION)d, neighbor)) {
			if(IsMovePointsEnough(neighbor)) {
				BOOL zocViolation;
				BOOL alliedCity;
				if(CanMoveIntoCell(neighbor,
								   zocViolation,
								   FALSE,
								   alliedCity))
					return TRUE;
				else if(IsOccupiedByForeigner(neighbor))
					return TRUE;
			}
		}
	}
	return FALSE;
}

void ArmyData::ActionSuccessful(SPECATTACK attack, Unit &unit, Unit &c)
{
	sint32		soundID, spriteID;

	const SpecialAttackInfoRecord *rec;
	rec = unitutil_GetSpecialAttack(attack);
	soundID = rec->GetSoundIDIndex();
	spriteID = rec->GetSpriteID()->GetValue();

	if (spriteID != -1 && soundID != -1) {
		if(
		   (((m_owner == g_selected_item->GetVisiblePlayer()) ||
			 (unit.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()))) ||
			g_theUnitPool->IsValid(c) && 
			((c.GetOwner() == g_selected_item->GetVisiblePlayer()) ||
			 (c.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()))))) {
			
			
			g_director->AddCenterMap(m_pos);
		}

		g_director->AddSpecialAttack(unit, c, attack);
	} else {
		if (soundID != -1) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == m_owner) || 
				(unit.GetVisibility() & (1 << visiblePlayer))) {

				g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 	soundID, m_pos.x, m_pos.y);
			}
		}
	}
}

void ArmyData::ActionUnsuccessful(const MapPoint &point)
{
	sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
	if ((visiblePlayer == m_owner) || 
		(m_array[0].GetVisibility() & (1 << visiblePlayer))) {

		g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
							gamesounds_GetGameSoundID(GAMESOUNDS_DEFAULT_FAIL),
							point.x,
							point.y);
	}
}

BOOL ArmyData::CanBeachAssault() const
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(!m_array[i].CanBeachAssault())
			return FALSE;
	}
	return TRUE;
}

BOOL ArmyData::CanHearGossip() const
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
        if(g_theUnitDB->Get(m_array[i].GetType())->GetHearGossip())
			return TRUE;
	}
	return FALSE;
}

BOOL ArmyData::CanSlaveUprising() const
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
        if(g_theUnitDB->Get(m_array[i].GetType())->GetSlaveUprising() &&
		   m_array[i].CanPerformSpecialAction())
			return TRUE;
	}
	return FALSE;
}

BOOL ArmyData::CanInciteRevolution( double &chance, double &eliteChance ) const
{
	UnitRecord::InciteRevolutionData *data;
    for(sint32 i = 0; i < m_nElements; i++) {
        if(g_theUnitDB->Get(m_array[i].GetType())->GetInciteRevolution(data) &&
			m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			eliteChance = data->GetEliteChance();
            return TRUE;
		}
    }
    return FALSE;
}

BOOL ArmyData::CanCloak() const
{
    for(sint32 i = 0; i < m_nElements; i++) {
        if(!g_theUnitDB->Get(m_array[i].GetType())->GetCanCloak() ||
		   !m_array[i].CanPerformSpecialAction())
            return FALSE;
    }
    return TRUE;
}

BOOL ArmyData::CanCreateFranchise( double &chance ) const
{
	UnitRecord::ChanceEffect *data;
    for(sint32 i = 0; i < m_nElements; i++) {
        if(g_theUnitDB->Get(m_array[i].GetType())->GetCreateFranchise(data) &&
			m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			return TRUE;
		}
    }
    return FALSE;
}

BOOL ArmyData::CanAssasinateRuler( double &chance, double &eliteChance) const
{
	UnitRecord::AssasinateRulerData *data;

    for(sint32 i = 0; i < m_nElements; i++) {
        if(g_theUnitDB->Get(m_array[i].GetType())->GetAssasinateRuler(data) &&
			m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			eliteChance = data->GetEliteChance();
            return TRUE;
		}
    }
    return FALSE;
}

BOOL ArmyData::CanStealTechnology( double &randChance, double &chance) const
{
	UnitRecord::StealTechnologyData *data;
    for(sint32 i = 0; i < m_nElements; i++) {
        if(g_theUnitDB->Get(m_array[i].GetType())->GetStealTechnology(data) &&
			m_array[i].CanPerformSpecialAction()) {
			randChance = data->GetRandomChance();
			chance = data->GetSpecificChance();
            return TRUE;
		}
    }
    return FALSE;
}

BOOL ArmyData::CanInvestigateCity( double &chance, double &eliteChance) const
{
	UnitRecord::InvestigateCityData *data;
    for(sint32 i = 0; i < m_nElements; i++) {
        if(m_array[i].GetDBRec()->GetInvestigateCity(data) &&
			m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			eliteChance = data->GetEliteChance();
            return TRUE;
		}
    }
    return FALSE;
}


BOOL ArmyData::ExecuteSpecialOrder(Order *order, BOOL &keepGoing)
{
	
	
	
	keepGoing = TRUE;

	Assert(order->m_order < UNIT_ORDER_MAX);
	Assert(order->m_order >= (UNIT_ORDER_TYPE) 0);
	if(order->m_order < (UNIT_ORDER_TYPE)0 || order->m_order >= UNIT_ORDER_MAX)
		return TRUE;

	if(!CanPerformSpecialAction()) {
		
		keepGoing = FALSE;
		return FALSE;
	}

	if (s_orderDBToEventMap == NULL)
		AssociateEventsWithOrdersDB();
	Assert(s_orderDBToEventMap);

	GAME_EVENT gev = Order::OrderToEvent(order->m_order);
	sint32 odb;
	for(odb = 0; odb < g_theOrderDB->NumRecords(); odb++) {
		if(s_orderDBToEventMap[odb] == gev) {
			break;
		}
	}
	Assert(odb < g_theOrderDB->NumRecords());
	if(odb >= g_theOrderDB->NumRecords())
		odb = 0;

	ORDER_RESULT result;

	const OrderRecord *order_rec = g_theOrderDB->Get(odb);







	
	if(order_rec && order_rec->GetGold() > 0) {
		if(g_player[m_owner]->m_gold->GetLevel() < order_rec->GetGold()) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == m_owner) || 
				(m_array[0].GetVisibility() & (1 << visiblePlayer))) {

				sint32	spriteID = g_theSpecialEffectDB->Get(g_theSpecialEffectDB->FindTypeIndex("SPECEFFECT_GENERAL_CANT"))->GetValue();
				sint32	soundID = gamesounds_GetGameSoundID(GAMESOUNDS_TOOEXPENSIVE);

				g_director->AddSpecialEffect(order->m_point, spriteID, soundID);
			}
			return TRUE;
		}
	}

	BOOL useDefaultSuccessSound = FALSE;

	sint32 i;
	uint32 origVisibility = 0;
	for(i = 0; i < m_nElements; i++) {
		origVisibility |= m_array[i].GetVisibility();
	}

	switch(order->m_order) {
		case UNIT_ORDER_INVESTIGATE_CITY:
			result = InvestigateCity(order->m_point);
			break;
		case UNIT_ORDER_NULLIFY_WALLS:
			result = NullifyWalls(order->m_point);
			break;
		case UNIT_ORDER_STEAL_TECHNOLOGY:
			result = StealTechnology(order->m_point);
			break;
		case UNIT_ORDER_ASSASSINATE:
			result = AssassinateRuler(order->m_point);
			break;
		case UNIT_ORDER_BOMBARD:
			result = Bombard(order->m_point);
			break;
		case UNIT_ORDER_SUE:
			result = Sue(order->m_point);
			useDefaultSuccessSound = TRUE;
			break;
		case UNIT_ORDER_FRANCHISE:
			result = Franchise(order->m_point);
			break;
		case UNIT_ORDER_SUE_FRANCHISE:
			result = SueFranchise(order->m_point);
			useDefaultSuccessSound = TRUE;
			break;
		case UNIT_ORDER_EXPEL:
			result = Expel(order->m_point);
			useDefaultSuccessSound = TRUE;
			break;
		case UNIT_ORDER_ESTABLISH_EMBASSY:
			result = EstablishEmbassy(order->m_point);
			break;
		case UNIT_ORDER_THROW_PARTY:
			result = ThrowParty(order->m_point);
			break;
		case UNIT_ORDER_CAUSE_UNHAPPINESS:
			Assert(FALSE);
			result = ORDER_RESULT_ILLEGAL;
			break;
		case UNIT_ORDER_PLANT_NUKE:
			result = PlantNuke(order->m_point);
			break;
		case UNIT_ORDER_SLAVE_RAID:
			result = SlaveRaid(order->m_point);
			break;
		case UNIT_ORDER_ENSLAVE_SETTLER:
			result = EnslaveSettler(order->m_point, 0, Unit(0));
			break;
		case UNIT_ORDER_UNDERGROUND_RAILWAY:
			result = UndergroundRailway(order->m_point);
			break;
		case UNIT_ORDER_INCITE_UPRISING:
			result = InciteUprising(order->m_point);
			break;
		case UNIT_ORDER_BIO_INFECT:
			result = BioInfect(order->m_point);
			break;
		case UNIT_ORDER_PLAGUE:
			result = Plague(order->m_point);
			break;
		case UNIT_ORDER_NANO_INFECT:
			result = NanoInfect(order->m_point);
			break;
		case UNIT_ORDER_CONVERT:
			result = ConvertCity(order->m_point);
			break;
		case UNIT_ORDER_REFORM:
			result = ReformCity(order->m_point);
			break;
		case UNIT_ORDER_INDULGENCE:
			result = IndulgenceSale(order->m_point);
			break;
		case UNIT_ORDER_SOOTHSAY:
			result = Soothsay(order->m_point);
			break;
		case UNIT_ORDER_CREATE_PARK:
			result = CreatePark(order->m_point);
			break;
		case UNIT_ORDER_PILLAGE:
			result = Pillage(TRUE);
			useDefaultSuccessSound = TRUE;
			break;
		case UNIT_ORDER_INJOIN:
			result = Injoin(order->m_point);
			break;
		case UNIT_ORDER_INTERCEPT_TRADE:
			result = InterceptTrade();

			break;
		case UNIT_ORDER_INCITE_REVOLUTION:
			result = InciteRevolution(order->m_point);
			break;
		case UNIT_ORDER_PILLAGE_UNCONDITIONALLY:
			result = Pillage(FALSE);
			useDefaultSuccessSound = TRUE;
			break;
		case UNIT_ORDER_ADVERTISE:
			result = Advertise(order->m_point);
			break;
		case UNIT_ORDER_LAUNCH:
			result = Launch(order);
			break;
		case UNIT_ORDER_TARGET:
			result = Target(order);
			break;
		case UNIT_ORDER_CLEAR_TARGET:
			result = ClearTarget();
			break;
		default:
			result = ORDER_RESULT_ILLEGAL;
			Assert(FALSE);
			break;
	}

#ifdef _DEBUG
        if (g_theDiplomacyLog) { 
            g_theDiplomacyLog->PopRegardRequest(); 
        } 
#endif _DEBUG

	if(result == ORDER_RESULT_ILLEGAL) { 
		
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		if ((visiblePlayer == m_owner) || 
			(m_array[0].GetVisibility() & (1 << visiblePlayer))) {

			sint32	spriteID = g_theSpecialEffectDB->Get(g_theSpecialEffectDB->FindTypeIndex("SPECEFFECT_GENERAL_CANT"))->GetValue();
			sint32	soundID = gamesounds_GetGameSoundID(GAMESOUNDS_DEFAULT_FAIL);

			g_director->AddSpecialEffect(order->m_point, spriteID, soundID);
		}

		sint32 i;
		for(i = m_nElements - 1; i >= 0; i--) {
			Assert(!m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW));
			m_array[i].ClearFlag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW);
		}
		return TRUE;
	}


	BOOL deduct = FALSE;
	sint32 sound = -1;

	switch(result) {
		case ORDER_RESULT_FAILED: {
			deduct = TRUE;

			sint32	spriteID = g_theSpecialEffectDB->Get(g_theSpecialEffectDB->FindTypeIndex("SPECEFFECT_GENERAL_FAIL"))->GetValue();
			sint32 soundID;
			if(!order_rec->GetFailSound(soundID))
				soundID = 0;
			

			if(!g_director->TileWillBeCompletelyVisible(order->m_point.x, order->m_point.y) &&
			   g_player[g_selected_item->GetVisiblePlayer()]->m_vision->IsVisible(order->m_point)) {
				
				g_director->AddCenterMap(m_pos);
			}

			g_director->AddSpecialEffect(order->m_point, spriteID, soundID);
		}
			break;
		case ORDER_RESULT_SUCCEEDED:
		case ORDER_RESULT_SUCCEEDED_INCOMPLETE:
			deduct = TRUE;

			if (useDefaultSuccessSound) {
				if(!g_director->TileWillBeCompletelyVisible(order->m_point.x, order->m_point.y) &&
				   g_player[g_selected_item->GetVisiblePlayer()]->m_vision->IsVisible(order->m_point)) {
					
					g_director->AddCenterMap(m_pos);
				}

				sint32	spriteID = g_theSpecialEffectDB->Get(g_theSpecialEffectDB->FindTypeIndex("SPECEFFECT_GENERAL_SUCCESS"))->GetValue();
				sint32 soundID;
				if(!order_rec->GetSound(soundID)) {
					soundID = 0;
				}
				

				g_director->AddSpecialEffect(order->m_point, spriteID, soundID);
			} else {
			}
			break;
		case ORDER_RESULT_INCOMPLETE:
			deduct = true;
			break;
	}

    if (result == ORDER_RESULT_SUCCEEDED || 
		result == ORDER_RESULT_SUCCEEDED_INCOMPLETE) {
        char *sText = NULL;
		sint32 uindex = -1;
        switch(order->m_order) {
          case UNIT_ORDER_STEAL_TECHNOLOGY:  sText = "186StealTechnologyVictim"; break;
          case UNIT_ORDER_ASSASSINATE:       sText = "176AssassinationCompleteVictim"; break;
          case UNIT_ORDER_INDULGENCE:        sText = "155IndulgenceCompleteVictim"; break;
		  
          case UNIT_ORDER_INJOIN:            sText = "159InjunctionCompleteVictim"; break;
        }
        if (sText) {
            
            Unit c = GetAdjacentCity(order->m_point);
            
            SlicObject *so = new SlicObject(sText) ;
			if(g_theUnitPool->IsValid(c))
				so->AddCity(c) ;
            so->AddRecipient(c.GetOwner()) ;
            g_slicEngine->Execute(so) ;
        }
    }

	if(deduct) {
		if(order_rec) {
			g_player[m_owner]->m_gold->SubGold(order_rec->GetGold());
		}
		sint32 i;		
		for(i = m_nElements - 1; i >= 0; i--) {
			if(m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW)) {
				m_array[i].ClearFlag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW);
				if(order_rec) {
					BOOL out_of_fuel;
					m_array[i].DeductMoveCost(order_rec->GetMove(), out_of_fuel);
				}
				m_array[i].ClearFlag(k_UDF_FIRST_MOVE);
			}
		}
	}
	if(result == ORDER_RESULT_SUCCEEDED_INCOMPLETE)
		return FALSE;

	return TRUE;
}

void ArmyData::AddSpecialActionUsed(Unit &who)
{
	who.SetFlag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN);
	who.SetFlag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW);
}

void ArmyData::SetTurnOver()
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		m_array[i].SetFlag(k_UDF_TURN_OVER);
	}
}

BOOL ArmyData::TurnOver()
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(!m_array[i].Flag(k_UDF_TURN_OVER))
			return FALSE;
	}
	return TRUE;
}



void ArmyData::GetCurrentHP(sint32 &count, sint32 unit_type[100], 
    sint32 unit_hp[100])
{
    sint32 n = Num(); 
    sint32 unit_idx;
    
    count=0; 
    for (unit_idx=0; unit_idx<n; unit_idx++) { 
        Assert(count < 100); 
        unit_type[count] = m_array[unit_idx].GetType(); 
        unit_hp[count] = sint32(m_array[unit_idx].GetHP()); 
		if (unit_hp[count] <= 0)
		{
			#ifdef _DEBUG
			
			unit_hp[count] = sint32(m_array[unit_idx].GetHP());
			#endif
			
			
			unit_hp[count] = 0;
		}
		Assert(unit_hp[count] >= 0);

        count++; 
		
        
        
        
    } 
}

#if !defined (ACTIVISION_ORIGINAL)
BOOL ArmyData::IsWounded()
{
		sint32 nb;
		sint32  unittypes[100];
		sint32  unithp[100];
		sint32 totalcurrentHP = 0;
		sint32 totalHP = 0;

		GetCurrentHP(nb,unittypes,unithp);
		for (int i = 0 ; i < nb ; i ++)
		{
			totalcurrentHP += unithp[i];
			totalHP+= g_theUnitDB->Get(unittypes[i])->GetMaxHP();
		}
		return (totalcurrentHP < totalHP/2); 
		//criterion can be changed, but seems relevant. Even if support isn't
		//determined as full, it will be wise not to attack with unit that has 
		//half than is normal HP.
}
#endif


BOOL ArmyData::CheckWasEnemyVisible(const MapPoint &pos, bool justCheck)
{
	
	sint32 i;
	BOOL wasVisible = FALSE;
	Cell *cell = g_theWorld->GetCell(pos);

	
	if(cell->GetCity().m_id != 0) {
		if(cell->GetCity().GetVisibility() & (1 << m_owner))
			return TRUE;
		else
			return FALSE;
	}

	CellUnitList *defender = cell->UnitArmy();
	for(i = 0; i < defender->Num(); i++) {
		if(defender->Access(i).GetVisibility() & (1 << m_owner)) {
			wasVisible = TRUE;
			break;
		}
	}
	
	if(!wasVisible && !justCheck) {
		for(i = 0; i < defender->Num(); i++) {
			defender->Access(i).SetVisible(m_owner);
		}
		
		if(m_owner == g_selected_item->GetVisiblePlayer()) {
			g_selected_item->ForceDirectorSelect(Army(m_id));
		}
	}
	return wasVisible;
}

BOOL ArmyData::GetInciteRevolutionCost( const MapPoint &point, sint32 &attackCost )
{
	Unit c = g_theWorld->GetCity(point);
	if(c.m_id == 0)
		return FALSE;

	
	
	

	MapPoint start, dest;
	Player *p = g_player[c.GetOwner()];
#if defined(ACTIVISION_ORIGINAL)	// moved 
	double distcost;
#endif
	if(!p)
		return FALSE;

#if defined(ACTIVISION_ORIGINAL)	// ambiguous sqrt, possible division by 0
	if(p->GetCapitolPos(start)) {
		c.GetPos(dest);
		sint32 dist = MapPoint::GetSquaredDistance(start,dest);
		
		
		distcost = g_theConstDB->InciteRevolutionCapitolPenalty() *
			(1.0 - (sqrt(dist) / p->GetMaxEmpireDistance()));
        
        
        
	} else {
		distcost = g_theConstDB->InciteRevolutionCapitolPenalty(); 
	}
#else
	double distcost = g_theConstDB->InciteRevolutionCapitolPenalty();

	if (p->GetCapitolPos(start) && p->GetMaxEmpireDistance()) 
	{
		c.GetPos(dest);
		double const distanceFromCapitol = 
			sqrt(static_cast<double>(MapPoint::GetSquaredDistance(start, dest)));
		distcost *= (1.0 - (distanceFromCapitol / p->GetMaxEmpireDistance()));
	}
#endif#endif

	if(distcost < 1)
		distcost = 1;

	sint32 capitolPenalty = 0;
	if(c.IsCapitol()) {
		capitolPenalty = sint32(g_theConstDB->InciteRevolutionCapitolPenalty());
	}

	
	double goldcost = p->m_gold->GetLevel();
	double popcost = c.PopCount() * g_theConstDB->InciteRevolutionGoldCoefficient();

	double cost = goldcost + popcost + distcost + capitolPenalty;

	
	
	

	attackCost = sint32(cost);

	return TRUE;
}

BOOL ArmyData::GetInciteUprisingCost( const MapPoint &point, sint32 &attackCost )
{
	Unit c = g_theWorld->GetCity(point);

	
	if(c.m_id == 0)
		return FALSE;

	
	
		
		
	
	

	if(c.CountSlaves() <= 0) {
		return FALSE;
	}

	MapPoint start, dest;
    PLAYER_INDEX city_owner = c.GetOwner();
	Player *p = g_player[city_owner];
	float distcost;

	if(p->GetCapitolPos(start)) {
		c.GetPos(dest);

#if defined(ACTIVISION_ORIGINAL)
		sint32 dist = MapPoint::GetSquaredDistance(start,dest);
		distcost = 100.0 * sqrt(dist); 
#else
		float const distanceFromCapitol =
			sqrt(static_cast<float>(MapPoint::GetSquaredDistance(start, dest)));
		distcost = 100.0f * distanceFromCapitol;
#endif        
        
        
	} else {
		distcost = float(p->GetMaxEmpireDistance()); 
	}
	
	distcost += 100;

	if(distcost < 1)
		distcost = 1;


	
#define k_SOME_DISTANCE_NUMBER 1.0

	double numSlaves = c.CountSlaves();
	





	sint32 capitolPenalty = 0;
	if(c.IsCapitol()) {
		capitolPenalty = sint32(g_theConstDB->InciteUprisingCapitolPenalty());
	}
	double cost = (g_player[c.GetOwner()]->m_gold->GetLevel() + 5000) *
		double(c.PopCount()) * (1 / distcost) *
		 g_theConstDB->InciteUprisingGoldCoefficient() +
		 capitolPenalty;

	attackCost = sint32(cost);

	return TRUE;
}

BOOL ArmyData::DoLeaveOurLandsCheck(const MapPoint &newPos, 
									UNIT_ORDER_TYPE order_type)
{
	
	Cell *cell = g_theWorld->GetCell(newPos);
	
	if(cell->GetOwner() >= 0 && cell->GetOwner() != m_owner &&
	   cell->GetOwner() != g_theWorld->GetCell(m_pos)->GetOwner()) {

		
		
		BOOL atLeastOneNonSpecialUnit = FALSE;
		sint32 i;
		for(i = 0; i < m_nElements; i++) {
			if(m_array[i].GetAttack() > 0 || 
			   m_array[i].GetDBRec()->GetVisionClassStandard()) {
				atLeastOneNonSpecialUnit = TRUE;
			}
		}
		if(atLeastOneNonSpecialUnit) {
			Agreement ag = g_player[cell->GetOwner()]->FindAgreement(AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS, m_owner);
			if(g_theAgreementPool->IsValid(ag) && ag.GetRecipient() == m_owner) {
				
				
				if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
				   (g_network.IsClient() && g_network.IsLocalPlayer(m_owner))) {
					char turnBuf[32];
					sprintf(turnBuf, "%d", ag.GetTurns() + 1);
					SlicObject *so = new SlicObject("13IAEnteringLands");
					so->AddCivilisation(m_owner);
					so->AddCivilisation(cell->GetOwner());
					so->AddAction(turnBuf);
					so->AddLocation(newPos);
					so->AddOrder(order_type);
					so->AddRecipient(m_owner);
					so->AddUnit(m_array[0]);
					g_selected_item->ForceDirectorSelect(Army(m_id));
					g_slicEngine->Execute(so);
					return TRUE;
				} else {
					
					ag.AccessData()->RecipientIsViolating(cell->GetOwner(), TRUE);
				}
			}
		}
	}
	return FALSE;
}

Path *ArmyData::RemovePathedOrder()
{
	Order *order = m_orders->GetHead();
	if(!order)
		return NULL;

	if(!order->m_path)
		return NULL;

	m_orders->RemoveHead();
	Path *ret = order->m_path;
	order->m_path = NULL;
	delete order;
	return ret;
}


void ArmyData::CharacterizeArmy( bool & isspecial, 
								 sint32 & maxattack, 
								 sint32 & maxdefense, 
								 bool & cancapture,
								 bool & haszoc,
								 bool & canbombard) const
{
	isspecial = false;
	maxattack = 0;
	maxdefense = 0;
	cancapture = false;
	haszoc = false;
	canbombard = false;

	for(sint32 i = 0; i < m_nElements; i++) {
		const UnitRecord *rec = g_theUnitDB->Get(m_array[i].GetType());

		
		
		if (rec->GetNuclearAttack()) 
		{
			isspecial = true;
			maxattack = 0;
			maxdefense = 0;
			cancapture = false;
			haszoc = false;
			canbombard = false;
			break;
		}

		isspecial |= ((rec->GetVisionClassStandard() == false) ||
			(rec->GetAttack() <= 0));
		if (m_array[i].GetAttack() > maxattack)
			maxattack = sint32(rec->GetAttack());
		if (m_array[i].GetDefense() > maxdefense)
			maxdefense = sint32(rec->GetDefense());
		cancapture |= (rec->GetCantCaptureCity() == false);
		haszoc |= (rec->GetNoZoc() == false);
		canbombard |= (rec->GetCanBombard() != 0x0);
	}
}


BOOL ArmyData::IsCivilian() const
{
	const UnitRecord *rec;
	for(sint32 i = 0; i < m_nElements; i++) 
	{
		rec = g_theUnitDB->Get(m_array[i]->GetType());
		if (!rec->GetCivilian())
			return FALSE;
	}
	return TRUE;
}


void ArmyData::GetArmyStrength( sint32 & hitpoints, 
							    sint32 & defense_count, 
							    sint32 & ranged_count, 
							    sint32 & attack_strength,
							    sint32 & defense_strength,
							    sint32 & ranged_strength,
						        sint32 & total_value ) const
{
	double fire_power;

	hitpoints = 0;
	defense_count = 0;
	ranged_count = 0;
	attack_strength = 0;
	defense_strength = 0;
	ranged_strength = 0;
	total_value = 0;

	UnitDynamicArray* cargo_list;
	int j;

	for(sint32 i = 0; i < m_nElements; i++) 
	{
		
		cargo_list = m_array[i]->GetCargoList();
		
		
		
		if (cargo_list->Num() > 0)
		{
			for (j = 0; j < cargo_list->Num(); j++)
			{
				const UnitRecord *rec = g_theUnitDB->Get(cargo_list->Access(j).GetType());
				
				
				hitpoints = (sint16)m_array[i].GetHP();
				fire_power = rec->GetFirepower();
				
				
				if (rec->GetDefense() > 0)
					defense_count++;
				
				
				if (rec->GetZBRangeAttack() > 0)
					ranged_count++;
				
				
				attack_strength += (sint32) (rec->GetAttack() * hitpoints * fire_power);
				defense_strength += (sint32) (rec->GetDefense() * hitpoints * fire_power);
				ranged_strength += (sint32) (rec->GetZBRangeAttack() * hitpoints * fire_power);
				total_value += (sint32) (rec->GetShieldCost()); 
			}
			
		}
		
		else 
		{
			const UnitRecord *rec = g_theUnitDB->Get(m_array[i].GetType());
			
			
			hitpoints = (sint32)m_array[i].GetHP();
			fire_power = rec->GetFirepower();
			
			
			if (m_array[i].GetDefense() > 0)
				defense_count++;
			
			
			if (rec->GetZBRangeAttack() > 0)
				ranged_count++;
			
			
			attack_strength += (sint32) (m_array[i].GetAttack() * hitpoints * fire_power);
			defense_strength += (sint32) (m_array[i].GetDefense() * hitpoints * fire_power);
			ranged_strength += (sint32) (rec->GetZBRangeAttack() * hitpoints * fire_power);
			total_value += m_array[i].GetShieldCost(); 
		}
	}
}

bool ArmyData::CanPerformSpecialAction() const
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(!m_array[i].CanPerformSpecialAction())
			return false;
	}
	return true;
}

void ArmyData::CheckAddEventOrder()
{
	if(g_network.IsClient() && !g_network.IsLocalPlayer(m_owner)) {
		
		
		return;
	}

	Order *order = m_orders->GetHead();
	if(order && order->m_order == UNIT_ORDER_ADD_EVENT && CanPerformSpecialAction()) {
		m_orders->RemoveHead();
		g_gevManager->ArglistAddEvent(GEV_INSERT_AfterCurrent,
									  order->m_eventType,
									  order->m_gameEventArgs);
		order->m_gameEventArgs = NULL;
		delete order;
	}
}

void ArmyData::IncrementOrderPath()
{
	Order *order = m_orders->GetHead();
	if(order && order->m_path) {
		order->m_path->IncDir();
		if(order->m_path->IsEndDir()) {
			m_orders->RemoveHead();
			delete order;
		}
	}
}


bool ArmyData::CheckValidDestination(const MapPoint &dest) const
{
	Order *order = m_orders->GetHead();

	if (order && order->m_order== UNIT_ORDER_MOVE && order->m_path)
	{
		
		MapPoint pos;
		if (order->m_path->IsEnd())
			pos = order->m_path->GetEnd();
		else
			order->m_path->GetCurrentPoint(pos);
		
		if (pos != m_pos)
			
			order->m_path->RestoreIndexAndCurrentPos(order->m_path->GetNextIndex());
	}

	
	if ((order) &&
		(order->m_order == UNIT_ORDER_MOVE) &&
		(order->m_path) &&
		(order->m_path->GetEnd() == dest))
		return true;

	
	
	return false;
}


bool ArmyData::AtEndOfPath() const
{
	Order *order = m_orders->GetHead();

	
	if ((order) &&
		(order->m_order == UNIT_ORDER_MOVE) &&
		(order->m_path) &&
		(order->m_path->GetEnd() == m_pos))
		return true;

	
	
	return false;
}


bool ArmyData::GetNextPathPoint(MapPoint & next_pos) const
{
	Order *order = m_orders->GetHead();
	if ((order) &&
		(order->m_order == UNIT_ORDER_MOVE) &&
		(order->m_path) && 
		(order->m_path->IsEndDir() == FALSE))
	{
		order->m_path->GetCurrentPoint(next_pos);
		return true;
	}

	
	
	return false;
}


//----------------------------------------------------------------------------
//
// Name       : ArmyData::TestOrderAll
//
// Description: Test whether all units are capable of performing an order.
//
// Parameters : order_rec	: the order to test
//
// Globals    : g_player	: player (capabilities)
//				g_theUnitDB	: unit (capabilities)
//
// Returns    : bool		: all units in the army are capable of 
//							  performing the order.
//
// Remark(s)  : In contrast to mathematical logic, an army without units is
//              incapable of doing anything.
//
//----------------------------------------------------------------------------
bool ArmyData::TestOrderAll(const OrderRecord *order_rec) const
{
	
	if(!m_nElements)
		return(false);

	
	bool orderValid = true;

	
	if(order_rec->GetUnitPretest_CanPlantNuke())
	{
		if (!g_player[m_owner]->m_advances->HasAdvance(advanceutil_GetNukeAdvance()))
			return false;
	}

	
	for(sint32 army_index = 0; army_index < m_nElements; army_index++) {
		
		const UnitRecord *unit_rec = g_theUnitDB->Get(m_array[army_index].GetType());

		
		orderValid = orderValid && UnitValidForOrder(order_rec, unit_rec);
	}

	
	return(orderValid);
}


#if !defined(ACTIVISION_ORIGINAL) // #02
//----------------------------------------------------------------------------
//
// Name       : ArmyData::TestOrderAny
//
// Description: Test whether some unit is capable of performing an order.
//
// Parameters : order_rec	: the order to test
//
// Globals    : g_player	: player (capabilities)
//				g_theUnitDB	: unit (capabilities)
//
// Returns    : bool		: at least one unit in the army is capable of 
//							  performing the order.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::TestOrderAny(OrderRecord const * order_rec) const
{
	if (order_rec->GetUnitPretest_CanPlantNuke() &&
		!g_player[m_owner]->m_advances->HasAdvance(advanceutil_GetNukeAdvance())
	   )
	{
		return false;
	}

	bool		orderValid	= false;

	for 
	(
		sint32	army_index	= 0; 
		!orderValid && (army_index < m_nElements); 
		++army_index
	) 
	{
		
		UnitRecord const *	unit_rec	= 
			g_theUnitDB->Get(m_array[army_index].GetType());

		orderValid = UnitValidForOrder(order_rec, unit_rec);
	}

	return orderValid;
}

#endif	// ACTIVISION_ORIGINAL

bool ArmyData::TestOrderUnit(const OrderRecord *order_rec, uint32 unit_index) const
{
	
	if((unit_index < 0) || (unit_index >= m_nElements))
		return(false);

	
	const UnitRecord *unit_rec = g_theUnitDB->Get(m_array[unit_index].GetType());

	
	return(UnitValidForOrder(order_rec, unit_rec));
}


ORDER_TEST ArmyData::TestOrder(const OrderRecord * order_rec) const
{
	
	return TestOrderHere( order_rec, m_pos );
}


ORDER_TEST ArmyData::TestOrderHere(const OrderRecord * order_rec, const MapPoint & pos) const
{
	bool target_valid;
	bool cur_pos_valid;
	bool order_valid;
	bool can_afford;
	sint32 range;

	ORDER_TEST best_result = 	ORDER_TEST_ILLEGAL;

	
	Assert(g_player[m_owner]);
	can_afford = ( g_player[m_owner]->GetGold() >= order_rec->GetGold() );

	
	if (order_rec->GetRange()) {
		order_rec->GetRange(range);
	}
	else {
		range = 0;
	}

	
	if(order_rec->GetIsTeleport() && pos == m_pos) {
		target_valid = false; 
	} else {
		target_valid = TargetValidForOrder(order_rec, pos);
	}

	cur_pos_valid = ( range == 0) && TargetValidForOrder(order_rec, m_pos);

	if(order_rec->GetTargetPretestAttackPosition() && (target_valid || cur_pos_valid))
	{
		CellUnitList targetArmy;
		g_theWorld->GetArmy(pos,targetArmy);
		if(g_theWorld->GetCell(pos)->GetCity().IsValid() && g_theWorld->GetCell(pos)->GetCity().GetOwner() != m_owner) {
			
		} else if(!targetArmy.Num() || targetArmy.GetOwner() == m_owner)
		{
			target_valid = false;
			cur_pos_valid = false;
		}
	}

	
	if ( order_rec->GetTargetPretestEnemySpecialUnit() == true)
	{
		if (g_theWorld->GetOwner(pos) != m_owner)
		{
			target_valid = false;
			cur_pos_valid = false;
		}	
	}

	if(order_rec->GetTargetPretestTransport())
	{
		static CellUnitList transports;
		cur_pos_valid = CanMoveIntoTransport(m_pos, transports) != 0;
		target_valid = CanMoveIntoTransport(pos, transports) != 0;
	} else if (range <= 0) {
		target_valid &= (CanEnter(pos) == TRUE);
	}

	
	const UnitRecord *unit_rec;
	for(sint32 army_index = 0; army_index < m_nElements; army_index++) {
		unit_rec = g_theUnitDB->Get(m_array[army_index].GetType());
		order_valid = UnitValidForOrder(order_rec, unit_rec);
			
		
		if ( order_valid ) {
			
			if  (( range > 0 ) && (cur_pos_valid == false) && (target_valid == false))
			{
				
				return ORDER_TEST_NEEDS_TARGET;
			}
			else if ( target_valid == false && cur_pos_valid == false) {
				
				return ORDER_TEST_INVALID_TARGET;
			}
			else if (order_rec->GetTargetPretestAdjacentPosition() && 
					!m_pos.IsNextTo(pos) && (m_pos != pos))
			{
				
				return ORDER_TEST_INVALID_TARGET;
			}
			else if ( can_afford == false) {
				
				return ORDER_TEST_LACKS_GOLD;
			}
			else if ( m_array[army_index].CanPerformSpecialAction() == TRUE ) {
				
				return ORDER_TEST_OK;
			}
			else {
				
				best_result = ORDER_TEST_NO_MOVEMENT;
			}
		} 
	}

	return best_result;
}


ORDER_TEST ArmyData::CargoTestOrderHere(const OrderRecord * order_rec, const MapPoint & pos) const
{
	bool target_valid;
	bool order_valid;
	bool can_afford;
	sint32 range;

	ORDER_TEST best_result = 	ORDER_TEST_ILLEGAL;

	
	Assert(g_player[m_owner]);
	can_afford = ( g_player[m_owner]->GetGold() >= order_rec->GetGold() );

	
	if (order_rec->GetRange()) {
		order_rec->GetRange(range);
	}
	else {
		range = 0;
	}

	
	target_valid = TargetValidForOrder(order_rec, pos);

	if (range <= 0)
	{
		target_valid &= (CargoCanEnter(pos) == TRUE);
	}

	
	sint32 cargo_index;
	const UnitRecord *unit_rec;
	const UnitDynamicArray *cargo;
	for(sint32 army_index = 0; army_index < m_nElements; army_index++) {
		cargo = m_array[army_index].AccessData()->GetCargoList();
		for (cargo_index = 0; cargo_index < cargo->Num(); cargo_index++) {
			unit_rec = g_theUnitDB->Get(cargo->Access(cargo_index)->GetType());
			order_valid = UnitValidForOrder(order_rec, unit_rec);

			
			if ( order_valid ) {
				
				if  (( range > 0 ) && ( pos == m_pos))
				{
					
					return ORDER_TEST_NEEDS_TARGET;
				}
				else if ( target_valid == false ) {
					
					return ORDER_TEST_INVALID_TARGET;
				}
				else if ( can_afford == false) {
					
					return ORDER_TEST_LACKS_GOLD;
				}
				else if ( m_array[army_index].CanPerformSpecialAction() == TRUE ) {
					
					return ORDER_TEST_OK;
				}
				else {
					
					best_result = ORDER_TEST_NO_MOVEMENT;
				}
			} 

		} 
	} 
	
	return best_result;
}


bool ArmyData::TargetValidForOrder(const OrderRecord * order_rec, const MapPoint &pos)
{
	Unit target_city;
	CellUnitList target_army;
	bool target_valid = false;

	target_city = g_theWorld->GetCity(pos);
	g_theWorld->GetArmy(pos,target_army);

	if (order_rec->GetTargetPretestEnemyCity() ||
		order_rec->GetTargetPretestOwnCity())
	{
		target_valid = (target_city.m_id != 0);
	}

	if (order_rec->GetTargetPretestEnemyArmy())
	{
		target_valid |= (target_army.Num() > 0);
	}

	switch ( order_rec->GetTargetPretest() ) {
	case k_Order_TargetPretest_None_Bit:
		target_valid = true;
		break;
	case k_Order_TargetPretest_EnemySpecialUnit_Bit:
		target_valid |= (target_army.Num() > 0) && ( target_army[0].GetDBRec()->GetIsSpecialForces());
		break;
	case k_Order_TargetPretest_EnemySettler_Bit:
		target_valid |= (target_army.Num() > 0) && ( target_army[0].GetDBRec()->GetSettle());
		break;
	case k_Order_TargetPretest_EnemyTradeUnit_Bit:
		target_valid |= (target_army.Num() > 0) && ( target_army[0].GetDBRec()->GetCanBeSued());
		break;
	case k_Order_TargetPretest_TradeRoute_Bit:
		target_valid |= ( g_theWorld->GetCell(pos)->GetNumTradeRoutes() > 0);
		break;
	case k_Order_TargetPretest_TerrainImprovement_Bit:
		target_valid |= ( g_theWorld->GetCell(pos)->GetNumDBImprovements() > 0);
		break;
	case k_Order_TargetPretest_AdjacentPosition_Bit:
	case k_Order_TargetPretest_MovePosition_Bit:
		target_valid |= true;
		break;
	case k_Order_TargetPretest_AttackPosition_Bit:
		target_valid |= ((target_city.m_id != 0) || (target_army.Num() > 0));
		break;
	};

	return target_valid;
}


bool ArmyData::UnitValidForOrder(const OrderRecord * order_rec, const UnitRecord *unit_rec)
{
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

	
	if (order_rec->GetUnitPretest_CanAttack())
		order_valid = (unit_rec->GetAttack() > 0.0 );
	else if (order_rec->GetUnitPretest_CanEntrench())
		order_valid = unit_rec->GetCanEntrench();
	else if (order_rec->GetUnitPretest_CanSue())
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
	else if(order_rec->GetUnitPretest_None())
		order_valid = true;

	return order_valid;
}


void ArmyData::PerformOrder(const OrderRecord * order_rec)
{
	Path tmp_path;
	tmp_path.SetStart(m_pos);

	PerformOrderHere( order_rec, &tmp_path );
}


void ArmyData::PerformOrderHere(const OrderRecord * order_rec, const Path * path)
{
	Assert(path != NULL);

	if (path == NULL)
		return ;

	
	if (m_flags & k_CULF_IN_SPACE)
		return;

	
	Path *tmp_path = new Path((Path *) path);
	MapPoint target_pos;

	if (tmp_path->GetMovesRemaining() > 0)
	{
		
		target_pos = tmp_path->GetEnd();
	}
	else
	{
		
		target_pos = m_pos;
	}

	
	

	
	
		
	

		
	
	

	
	if (s_orderDBToEventMap == NULL)
		AssociateEventsWithOrdersDB();
	Assert(s_orderDBToEventMap != NULL);

	
	sint32 game_event = s_orderDBToEventMap[order_rec->GetIndex()];

	
	sint32 range = 0;
	sint32 moves = tmp_path->GetMovesRemaining();
	if (order_rec->GetRange()) {
		order_rec->GetRange(range);
		
		Assert(range <= moves || order_rec->GetTargetPretestAdjacentPosition());
	}

	
	for (sint32 i = 0; moves > 0 && i < range; i++)	{
		tmp_path->SnipEnd();
		moves--;
	}

	
	
	g_gevManager->Pause();

	

	
	if (game_event > 0)	{
		if (range > 0 || order_rec->GetIsTeleport() || order_rec->GetIsTarget())
			{
				
				g_gevManager->AddEvent( GEV_INSERT_AfterCurrent, 
										static_cast<GAME_EVENT>(game_event), 
										GEA_Army, Army(m_id), 
										GEA_MapPoint, target_pos, 
										GEA_End);
			}
		else {
			
			g_gevManager->AddEvent( GEV_INSERT_AfterCurrent, 
									static_cast<GAME_EVENT>(game_event), 
									GEA_Army, Army(m_id), 
									GEA_End);
		}
	}

	
	if (tmp_path->GetMovesRemaining() > 0 && !order_rec->GetIsTeleport() && !order_rec->GetIsTarget()) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MoveOrder, 
							   GEA_Army, Army(m_id),			
							   GEA_Path, tmp_path,		 
							   GEA_MapPoint, target_pos, 
							   GEA_Int, (game_event == -1), 
							   GEA_End);
	}
	else {
		
		delete tmp_path;
	}

	
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ClearOrders,
						   GEA_Army, Army(m_id),	
						   GEA_End);

	
	g_gevManager->Resume();
	
}


void ArmyData::AssociateEventsWithOrdersDB()
{
	
	const char *event_name;

#if defined(ACTIVISION_ORIGINAL)	// wrong delete	
	if (s_orderDBToEventMap != NULL)
		delete s_orderDBToEventMap;
#else
	delete [] s_orderDBToEventMap;
#endif
	
	s_orderDBToEventMap = new sint32 [g_theOrderDB->NumRecords()];
	
	for (int order_index = 0; order_index < g_theOrderDB->NumRecords(); order_index++)
	{
		event_name = g_theOrderDB->Get(order_index)->GetEventName();
		if (strlen(event_name) > 0)
			s_orderDBToEventMap[order_index] = g_gevManager->GetEventIndex(event_name);
		else
		{
			s_orderDBToEventMap[order_index] = -1;
			DPRINTF(k_DBG_SCHEDULER, ("AssociateEventsWithOrdersDB: No matching event found for %s.\n",
				event_name, g_theOrderDB->Get(order_index)->GetNameText()));
		}
	}
}

void ArmyData::Settle()
{
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
						   GEV_Settle,
						   GEA_Army, m_id,
						   GEA_End);
}


bool ArmyData::IsObsolete() const
{
	sint32 unit_index;
	sint32 adv_index;
	sint32 adv_type;

	Assert(g_player[m_owner] != NULL);

	
    for (unit_index = 0; unit_index < m_nElements; unit_index++) { 
		const UnitRecord * rec = g_theUnitDB->Get(m_array[unit_index].GetType());
		
		for (adv_index = 0; adv_index < rec->GetNumObsoleteAdvance(); adv_index++) {
			adv_type = rec->GetObsoleteAdvance(adv_index)->GetIndex();
			if (g_player[m_owner]->HasAdvance(adv_type) == TRUE)
				return true;
		}
	}
	return false;
}

void ArmyData::StopPirating()
{
	if(!m_isPirating) return;

	Cell *cell = g_theWorld->GetCell(m_pos);
	Assert(cell);
	if(cell) {
		sint32 i;
		sint32 piratedByMe = 0;
		for(i = 0; i < cell->GetNumTradeRoutes(); i++) {
			TradeRoute route = cell->GetTradeRoute(i);
			if(route->GetPiratingArmy().m_id == m_id) {
				
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SetPiratingArmy,
					   GEA_TradeRoute, m_array[i],
					   GEA_Army, Army(0),
					   GEA_End);
			}
		}
	}
	
	m_isPirating = false;
}

const MBCHAR *ArmyData::GetName() const
{
	if(!m_name) {
		static MBCHAR buf[40];
		sprintf(buf, "%s%d", g_theStringDB->GetNameStr("ARMY_NAME_PREFIX"), m_id & (0x0fffffff));
		return buf;
	} else {
		return m_name;
	}
}

void ArmyData::SetName(const MBCHAR *name)
{
	
	if(m_name) {
		delete [] m_name;
		m_name = NULL;
	}
	if(name) {
		m_name = new MBCHAR[strlen(name) + 1];
		strcpy(m_name, name);
	}
}


bool ArmyData::PlayerCanSee(const PLAYER_INDEX playerId) const
{
    for (sint32 i = 0; i < Num() ; i++) { 
        if (m_array[i].GetVisibility() & (1 << playerId))
			return true;
	}
	return false;
}


sint16 ArmyData::CountNuclearUnits() const
{
	sint16 count = 0;
	sint32 j;
	const UnitRecord *rec;
	const UnitDynamicArray *cargo;
    for (sint32 i = 0; i < Num() ; i++) { 
		rec = g_theUnitDB->Get(m_array[i].GetType());
        count += rec->GetNuclearAttack();
		cargo = m_array[i].AccessData()->GetCargoList();
		for(j = 0; cargo && j < cargo->Num(); j++) 
		{
			rec = g_theUnitDB->Get(cargo->Access(j).GetType());
		    count += rec->GetNuclearAttack();
		}
	}
	return count;
}


sint16 ArmyData::CountBioUnits() const
{
	sint16 count = 0;
	sint32 j;
	const UnitRecord *rec;
	const UnitDynamicArray *cargo;
    for (sint32 i = 0; i < Num() ; i++) { 
		rec = g_theUnitDB->Get(m_array[i].GetType());
        count += rec->GetBioTerror();
		cargo = m_array[i].AccessData()->GetCargoList();
		for(j = 0; cargo && j < cargo->Num(); j++) 
		{
			rec = g_theUnitDB->Get(cargo->Access(j).GetType());
		    count += rec->GetBioTerror();
		}
	}
	return count;
}


sint16 ArmyData::CountNanoUnits() const
{
	sint16 count = 0;
	sint32 j;
	const UnitRecord *rec;
	const UnitDynamicArray *cargo;
    for (sint32 i = 0; i < Num() ; i++) { 
		rec = g_theUnitDB->Get(m_array[i].GetType());
        count += rec->GetCreateParks();
		cargo = m_array[i].AccessData()->GetCargoList();
		for(j = 0; cargo && j < cargo->Num(); j++) 
		{
			rec = g_theUnitDB->Get(cargo->Access(j).GetType());
		    count += rec->GetCreateParks();
		}
	}
	return count;
}


sint16 ArmyData::DisbandNuclearUnits(const sint16 count)
{
	const UnitRecord *rec;
	const UnitDynamicArray *cargo;
	sint32 disbanded = 0;
	sint32 i,j = 0;
	for(i = (m_nElements - 1); i >= 0 && (disbanded < count); i--)
	{ 
		rec = g_theUnitDB->Get(m_array[i].GetType());
		cargo = m_array[i].AccessData()->GetCargoList();

        if (rec->GetNuclearAttack())
		{
			if (cargo && cargo->Num() > 0)
			{
				
				Assert(0);
			}
			else
			{
				m_array[i].Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
				disbanded++;
			}
		}
		else if (cargo)
		{
			
			for(j = (cargo->Num() - 1); j >= 0 && (disbanded < count); j--)
			{
				rec = g_theUnitDB->Get(cargo->Access(j).GetType());
				if (rec->GetNuclearAttack())
				{
					cargo->Access(j).Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
					disbanded++;
					continue;
				}
				j++;
			}
		}
	}
	return disbanded;
}


sint16 ArmyData::DisbandBioUnits(const sint16 count)
{
	const UnitRecord *rec;
	const UnitDynamicArray *cargo;
	sint32 disbanded = 0;
	sint32 i,j = 0;
   	for(i = (m_nElements - 1); i >= 0 && (disbanded < count); i--) 
	{ 
		rec = g_theUnitDB->Get(m_array[i].GetType());
		cargo = m_array[i].AccessData()->GetCargoList();

        if (rec->GetBioTerror())
		{
			if (cargo && cargo->Num() > 0)
			{
				
				Assert(0);
			}
			else
			{
				m_array[i].Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
				disbanded++;
			}
		}
		else if (cargo)
		{
			
			for(j = (cargo->Num() - 1); j >= 0 && (disbanded < count); j--)
			{
				rec = g_theUnitDB->Get(cargo->Access(j).GetType());
				if (rec->GetBioTerror())
				{
					cargo->Access(j).Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
					disbanded++;
					continue;
				}
				j++;
			}
		}
	}
	return disbanded;
}


sint16 ArmyData::DisbandNanoUnits(const sint16 count)
{
	const UnitRecord *rec;
	const UnitDynamicArray *cargo;
	sint32 disbanded = 0;
	sint32 i,j = 0;
   	for(i = (m_nElements - 1); i >= 0 && (disbanded < count); i--) 
	{ 
		rec = g_theUnitDB->Get(m_array[i].GetType());
		cargo = m_array[i].AccessData()->GetCargoList();

        if (rec->GetCreateParks())
		{
			if (cargo && cargo->Num() > 0)
			{
				
				Assert(0);
			}
			else
			{
				m_array[i].Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
				disbanded++;
			}
		}
		else if (cargo)
		{
			
			for(j = (cargo->Num() - 1); j >= 0 && (disbanded < count); j--)
			{
				rec = g_theUnitDB->Get(cargo->Access(j).GetType());
				if (rec->GetCreateParks())
				{
					cargo->Access(j).Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
					disbanded++;
					continue;
				}
				j++;
			}
		}
	}
	return disbanded;
}

bool ArmyData::HasVeterans() const
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(m_array[i]->IsVeteran())
			return true;
	}
	return false;
}

void ArmyData::DecrementDontKillCount()
{
	m_dontKillCount--;
	Assert(m_dontKillCount >= 0);
	if(m_dontKillCount <= 0) {
		if(m_needToKill) {
			Army me(m_id);
			me.Kill();
		}			
	}
}
