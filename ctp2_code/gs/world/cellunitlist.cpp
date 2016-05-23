//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling of a list of Units in the same cell
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
// USE_STOP_ZERO_MOVEMENT
// - When defined, prevents unit without movement points from moving.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Ships no longer get an underwater tunnel movement bonus, based on
//   suggestions by NelsonAndBronte.
// - Make unit types with 0 movement stand still (compiler option).
// - Handled crashes with invalid units.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "cellunitlist.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "player.h"
#include "UnitRec.h"
#include "DB.h"
#include "WonderRecord.h"
#include "SelItem.h"
#include "Army.h"
#include "network.h"
#include "SlicEngine.h"
#include "Cell.h"
#include "UnitDynArr.h"
#include "UnitPool.h"
#include "UnitData.h"
#include "UnitRecord.h"
#include "MoveFlags.h"
#include "wonderutil.h"
#include "GameEventManager.h"
#include "TerrainRecord.h"	// TerrainRecord

extern sint32 g_fog_toggle;

sint32 CellUnitList::Insert(Unit id)
{
	Assert(m_nElements < k_MAX_ARMY_SIZE);
	if(m_nElements < k_MAX_ARMY_SIZE) {
		m_array[m_nElements].m_id = id.m_id;
		m_nElements++;
		UpdateMoveIntersection();
		return 1;
	}
	return 0;
}

BOOL CellUnitList::CanEnter(const MapPoint &point) const
{
	if(m_nElements < 1) {

		return TRUE;
	}

	Cell *cell = g_theWorld->GetCell(point);
	if(cell->GetCity().IsValid() &&
	   cell->GetCity().GetOwner() == m_array[0].GetOwner()) {
		return TRUE;
	}

	uint32 env = cell->GetEnv();

	if(m_moveIntersection) {
		if((env & m_moveIntersection))
			return TRUE;

		if((m_moveIntersection & k_BIT_MOVEMENT_TYPE_AIR) &&
		   !(env & k_BIT_MOVEMENT_TYPE_SPACE)) {
			return TRUE;
		}

		if((m_moveIntersection & k_BIT_MOVEMENT_TYPE_LAND) &&
		   (env & k_MASK_ENV_ROAD)) {
			return TRUE;
		}

		if((m_moveIntersection & k_BIT_MOVEMENT_TYPE_SHALLOW_WATER) &&
		   !(m_moveIntersection & k_BIT_MOVEMENT_TYPE_WATER) &&
		   (env & k_BIT_MOVEMENT_TYPE_WATER) &&
		   wonderutil_GetAllBoatsDeepWater(g_player[m_array[0].GetOwner()]->m_builtWonders)) {
			return TRUE;
		}

		return FALSE;
	} else {

		for(sint32 i = 0; i < m_nElements; i++) {
			uint32 moveType = m_array[i].GetMovementType();

			if((env & (moveType << k_SHIFT_ENV_MOVEMENT_TYPE)) ||
			   ((moveType & k_BIT_MOVEMENT_TYPE_AIR)) ||
			   ((moveType & k_BIT_MOVEMENT_TYPE_LAND) && (env & k_MASK_ENV_ROAD))) {

				continue;
			} else {
				if((moveType & k_BIT_MOVEMENT_TYPE_SHALLOW_WATER) &&
				   !(moveType & k_BIT_MOVEMENT_TYPE_WATER) &&
				   (env & k_BIT_MOVEMENT_TYPE_WATER) &&
				   wonderutil_GetAllBoatsDeepWater(g_player[GetOwner()]->m_builtWonders)) {
					continue;
				}
				return FALSE;
			}
		}
		return TRUE;
	}
}

//----------------------------------------------------------------------------
//
// Name       : CellUnitList::GetMovementTypeLand
//
// Description: Determines whether all units can move on land.
//
// Parameters : -
//
// Globals	  : -
//
// Returns    : bool			: all units can move on land
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

bool CellUnitList::GetMovementTypeLand() const
{
    for (int i = 0; i < m_nElements; ++i)
	{
        if (!m_array[i].GetMovementTypeAir())
		{
            return false;
        }
    }

    return true;
}

BOOL CellUnitList::HasWormholeProbe() const
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetWormholeProbe()) {
			return TRUE;
		}
	}
	return FALSE;
}

PLAYER_INDEX CellUnitList::GetOwner() const
{
    Assert (m_nElements > 0);
    if(m_nElements <= 0)
		return 0;

    return m_array[0].GetOwner();
}

void CellUnitList::GetPos(MapPoint &pos) const
{
    Assert(0 < m_nElements);
    if(m_nElements < 1) {
		pos.x = -1;
		pos.y = -1;
		return;
	}
    m_array[0].GetPos(pos);
}

BOOL CellUnitList::IsAtLeastOneMoveLand() const

{

    sint32 i;

    for (i=0; i<m_nElements; i++) {
        if (m_array[i].GetMovementTypeLand()){
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CellUnitList::IsAtLeastOneMoveWater() const
{

    sint32 i;

    for (i=0; i<m_nElements; i++) {
        if (m_array[i].GetMovementTypeSea()){
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CellUnitList::IsAtLeastOneMoveShallowWater() const
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].GetMovementTypeShallowWater()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CellUnitList::IsAtLeastOneMoveAir() const

{

    sint32 i;

    for (i=0; i<m_nElements; i++) {
        if (m_array[i].GetMovementTypeAir()) {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL CellUnitList::IsAtLeastOneMoveSpace() const

{

    sint32 i;

    for (i=0; i<m_nElements; i++) {
        if (m_array[i].GetMovementTypeSpace()) {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL CellUnitList::IsAtLeastOneMoveMountain() const

{

    sint32 i;

    for (i=0; i<m_nElements; i++) {
        if (m_array[i].GetMovementTypeMountain()) {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL CellUnitList::IsEnemy(PLAYER_INDEX owner) const
{
	sint32 myOwner = GetOwner();
	if(g_network.IsActive() && g_network.TeamsEnabled() &&
	   g_player[myOwner] && g_player[owner] &&
	   g_player[myOwner]->m_networkGroup == g_player[owner]->m_networkGroup) {
		return FALSE;
	}

    uint32 a, b;
    a = ~(g_player[m_array[0].GetOwner()]->GetMaskAlliance());
    b = (0x00000001 << owner);

    return a & b;
}

BOOL CellUnitList::IsEnemy( Unit defender) const
{
    return player_isEnemy(GetOwner(), defender.GetOwner());
}

BOOL CellUnitList::IsEnemy(CellUnitList &defender) const
{
    if (defender.Num() < 1)
        return FALSE;
    else
        return IsEnemy(defender[0]);
}

BOOL CellUnitList::CanAtLeastOneCaptureCity() const
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(!m_array[i].GetDBRec()->GetCantCaptureCity()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CellUnitList::IsVisible(sint32 player) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetVisibility() & (1 << player))
			return TRUE;
	}
	return FALSE;
}

BOOL CellUnitList::CanBeExpelled()
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(!m_array[i].CanBeExpelled())
			return FALSE;
	}
	return TRUE;
}

extern sint32 g_god;

//----------------------------------------------------------------------------
//
// Name       : CellUnitList::GetTopVisibleUnitOfMoveType
//
// Description: Determine which unit of a stack to display.
//
// Parameters : looker				: the observing player
//              moveType			: a bit set of movement types
//				isResyncReported	: invalid units do not have to be reported
//
// Globals    : g_selected_item		: currently selected item on screen
//				g_player			: players
//				g_theWorld			: map information
//				g_network			: network handler (for multiplayer)
//				g_god				: when set, everything is visible
//				g_fog_toggle		: when set, everything is visible
//
// Returns    : bool				: some unit is visible
//				maxi				: index of the unit to display
//				isResyncReported	: updated when a (new) missing unit has
//                                    triggered a resync request
//
// Remark(s)  : - When returning false, the value of maxi has not been updated.
//              - Units in a city are only visible when awake or currently
//                selected.
//              - When there are multiple units in a stack, the top unit is
//				  determined on either movement points (own unit) or
//                "visibility class" + strength (enemy unit).
//
//----------------------------------------------------------------------------

bool CellUnitList::GetTopVisibleUnitOfMoveType
(
	PLAYER_INDEX const	looker,
	uint32 const		moveTypes,
	sint32 &			maxi,
	bool &				isResyncReported
) const
{
    bool	is_found_unit	= false;
	double	maxStrength		= -100000.0;
    uint32	min_vis			= 0xffffffff;
	double	minmove			= -1;

	Army	selectedArmy(0);
	g_selected_item->GetSelectedArmy(selectedArmy);

	for (sint32 i = 0; i < m_nElements; ++i)
	{
		Unit const &	u	= m_array[i];

		if (u.IsValid())
		{
			if (u.IsSameMovementType(moveTypes)	&&	// move types match
				// cell visible
				((u.GetVisibility() & (0x01 << looker))						||
				 (g_player[looker] && g_player[looker]->m_hasGlobalRadar)	||
				 g_god || g_fog_toggle
			    )								&&
			    // selected, awake, or out in the open
			    ((u.GetArmy().m_id == selectedArmy.m_id)					||
			     !(u.IsAsleep() || u.IsEntrenched() || u.IsEntrenching())	||
				 !g_theWorld->HasCity(u.RetPos())
			    )
			   )
			{
				if (u.GetOwner() == looker)
				{
					// Looking at own units: order on movement points
					if (u.GetMovementPoints() > minmove)
					{
						maxi			= i;
						minmove			= u.GetMovementPoints();
						is_found_unit	= true;
					}
				}
				else
				{
					// Looking at "enemy" units: order on visibility class,
					// with strength as tie-breaker.
					uint32 const	vis = u.GetVisibilityClass();

					if (vis <= min_vis)
					{
						double const	strength	=
							u.GetAttack() + u.GetDefense();

						if (strength > maxStrength)
						{
							maxStrength		= strength;
							maxi			= i;
							min_vis			= vis;
							is_found_unit	= true;
						}
					}
				}
			}
		}
		else if (!isResyncReported)
		{
			g_network.RequestResync(RESYNC_INVALID_UNIT);
			isResyncReported = true;
		}
    }

	return is_found_unit;
}










Unit CellUnitList::GetTopVisibleUnit(PLAYER_INDEX const looker) const
{
    uint32	move_union			= 0x0000;
	bool	stopResyncReport	= !g_network.IsClient();

    for (sint32 i = 0; i < m_nElements; ++i)
	{
		if (m_array[i].IsValid())
		{
			move_union |= m_array[i].GetMovementType();
		}
		else
		{
			// Would like to do DelIndex(i), but this is a const method.

			if (!stopResyncReport)
			{
				g_network.RequestResync(RESYNC_INVALID_UNIT);
				stopResyncReport = false;
			}
		}
    }

    sint32	maxi;

    if (move_union & k_Unit_MovementType_Space_Bit)
	{
        if (GetTopVisibleUnitOfMoveType
				(looker, k_Unit_MovementType_Space_Bit, maxi, stopResyncReport)
		   )
		{
            return m_array[maxi];
        }
    }
    if (move_union & k_Unit_MovementType_Air_Bit)
	{
        if (GetTopVisibleUnitOfMoveType
				(looker, k_Unit_MovementType_Air_Bit, maxi, stopResyncReport)
		   )
		{
            return m_array[maxi];
        }
    }
    if (move_union & k_Unit_MovementType_Sea_Bit)
	{
        if (GetTopVisibleUnitOfMoveType
				(looker, k_Unit_MovementType_Sea_Bit, maxi, stopResyncReport)
		   )
		{
            return m_array[maxi];
        }
    }
    if (move_union & k_Unit_MovementType_ShallowWater_Bit)
	{
        if (GetTopVisibleUnitOfMoveType
				(looker, k_Unit_MovementType_ShallowWater_Bit, maxi, stopResyncReport)
		   )
		{
            return m_array[maxi];
        }
    }
    if (move_union & k_Unit_MovementType_Land_Bit)
	{
        if (GetTopVisibleUnitOfMoveType
				(looker, k_Unit_MovementType_Land_Bit, maxi, stopResyncReport)
		   )
		{
            return m_array[maxi];
        }
    }
    if (move_union & k_Unit_MovementType_Mountain_Bit)
	{
        if (GetTopVisibleUnitOfMoveType
				(looker,k_Unit_MovementType_Mountain_Bit, maxi, stopResyncReport)
		   )
		{
            return m_array[maxi];
        }
    }
    if (move_union & k_Unit_MovementType_Trade_Bit)
	{
        if (GetTopVisibleUnitOfMoveType
				(looker, k_Unit_MovementType_Trade_Bit, maxi, stopResyncReport)
		   )
		{
            return m_array[maxi];
        }
    }

    return Unit(0);
}

BOOL CellUnitList::CanBeSued() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanBeSued())
			return TRUE;
	}
	return FALSE;
}

void CellUnitList::ForceVisibleThisTurn(const PLAYER_INDEX to_me)
{
    sint32 i;
    for (i=0; i<m_nElements; i++) {
        m_array[i].ForceVisibleThisTurn(to_me);
    }
}

double CellUnitList::GetHPModifier()

{
    return m_array[0].GetHPModifier();
}

void CellUnitList::DoVictoryEnslavement(sint32 origOwner)
{
	sint32 i;

	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].GetHP() > 0 &&
		   m_array[i].GetDBRec()->GetVictoryEnslavement()) {


			Unit hc;
			static MapPoint slpos;
			GetPos(slpos);







			sint32 r = g_player[m_array[0].GetOwner()]->
				GetSlaveCity(slpos, hc);

			Assert(hc.m_id != (0));
			if(hc.m_id == (0))
				break;

			static MapPoint cpos;
			hc.GetPos(cpos);
			if(g_network.IsHost()) {
				g_network.Block(hc.GetOwner());
			}

			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MakePop,
								   GEA_City, hc.m_id,
								   GEA_Player, origOwner,
								   GEA_End);

			g_slicEngine->RunVictoryEnslavementTriggers(m_array[i],
														origOwner, hc);
			if(g_network.IsHost()) {
				g_network.Unblock(hc.GetOwner());
			}
			break;
		}
	}
}

BOOL CellUnitList::ExertsZOC() const
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(!m_array[i].IsNoZoc())
			return TRUE;
	}
	return FALSE;
}


BOOL CellUnitList::CanMoveIntoCell(const MapPoint &pos,
								   BOOL &zocViolation,
								   BOOL ignoreZoc,
								   BOOL &alliedCity)
{
	Cell *cell = g_theWorld->GetCell(pos);
	CellUnitList *ul = cell->UnitArmy();

	zocViolation = FALSE;
	alliedCity = FALSE;

	if(ul && ul->Num() + m_nElements > k_MAX_ARMY_SIZE)
		return FALSE;

	static MapPoint myPos;
	GetPos(myPos);
	sint32 myOwner;
	myOwner = GetOwner();

	if(!CanEnter(pos)) {
		return FALSE;
	}

	if((!(m_flags & k_CULF_IGNORES_ZOC)) && !ignoreZoc) {
		if(g_theWorld->IsMoveZOC(myOwner, myPos, pos, FALSE)) {
			zocViolation = TRUE;
			return FALSE;
		}
	}

	if(cell->GetCity().m_id != (0)) {
		if(myOwner != cell->GetCity().GetOwner()) {
			if(m_flags & k_CULF_CANT_CAPTURE_CITY)
				return FALSE;
			sint32 cityOwner = cell->GetCity().GetOwner();

			if(myOwner == PLAYER_INDEX_VANDALS && wonderutil_GetProtectFromBarbarians(g_player[cityOwner]->m_builtWonders)) {
				return FALSE;
			}

			if(!IsEnemy(cell->GetCity().GetOwner()) ||
			   g_player[myOwner]->WillViolateCeaseFire(cityOwner) ||
			   g_player[myOwner]->WillViolatePact(cityOwner)) {
				alliedCity = TRUE;
				return FALSE;
			}
		}
	}

    return TRUE;
}

BOOL CellUnitList::IsMovePointsEnough(const double cost) const
{
#if defined(USE_STOP_ZERO_MOVEMENT)
	for (sint32 i = 0; i < m_nElements; ++i)
	{
		double const mp	= m_array[i].GetMovementPoints();

		if ((mp == 0.0) ||
		    ((mp < cost) && !m_array[i].GetFirstMoveThisTurn())
		   )
		{
			return FALSE;
		}
		// else: unit has enough movement points
	}
#else
    sint32 i;
    double mp;

    for (i=0; i<m_nElements; i++) {

		if(m_array[i].GetFirstMoveThisTurn())
			continue;

        mp = m_array[i].GetMovementPoints();
        if (mp < cost) {
            return FALSE;
        }
    }
#endif

    return TRUE;
}

BOOL CellUnitList::IsMovePointsEnough(const MapPoint &pos)
{

    double cost;

    if (GetMovementTypeAir()) {
        cost = k_MOVE_AIR_COST;
	// Prevent ships from diving under and using tunnels.
	} else if (g_theWorld->IsTunnel(pos) && !GetMovementTypeLand()) {
		sint32 icost;
		(void) g_theWorld->GetTerrain(pos)->GetEnvBase()->GetMovement(icost);
		cost = icost;
    } else {
        cost = g_theWorld->GetMoveCost(pos);
    }

    return IsMovePointsEnough(cost);
}

BOOL CellUnitList::GetMovementTypeAir() const
{
    sint32 i;

    for (i=0; i<m_nElements; i++) {
        if (!m_array[i].GetMovementTypeAir()) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CellUnitList::CanBeCargoPodded() const
{
	sint32 i;
	sint32 cargoPodType = -1;
	for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
		if(g_theUnitDB->Get(i)->GetCargoPod()) {
			cargoPodType = i;
			break;
		}
	}
	Assert(i < g_theUnitDB->NumRecords());
	if(i >= g_theUnitDB->NumRecords())
		return FALSE;

	const UnitRecord *cargoRec = g_theUnitDB->Get(cargoPodType);
	extern BOOL UnitCanCarry(sint32 src, sint32 dest);
	if(!cargoRec->GetCargoDataPtr())
		return FALSE;

	for(i = 0; i < m_nElements; i++) {
		if(cargoRec->GetCargoDataPtr()->GetMaxCargo() < m_nElements ||
		   !UnitCanCarry(cargoPodType, m_array[i].GetType())) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CellUnitList::CanSpaceLand() const
{
    sint32 i;
    const UnitRecord *rec;
    for (i=0; i<m_nElements; i++) {
		rec = g_theUnitDB->Get(m_array[i].GetType());
        if (!rec->GetSpaceLand()) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CellUnitList::CanSpaceLaunch() const
{
    sint32 i;
    const UnitRecord *rec;
    for (i=0; i<m_nElements; i++) {
		rec = g_theUnitDB->Get(m_array[i].GetType());
        if (!rec->GetSpaceLaunch()) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CellUnitList::IsIgnoresZOC() const
{

    sint32 i;

    for (i=0; i<m_nElements; i++) {
        if (!m_array[i].IsIgnoresZOC())
            return FALSE;
    }
    return TRUE;
}

void CellUnitList::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive.StoreChunk((uint8*)&m_array[0], (uint8*)&m_nElements + sizeof(m_nElements));
	} else {
		archive.LoadChunk((uint8*)&m_array[0], (uint8*)&m_nElements + sizeof(m_nElements));
	}
}

CellUnitList::CellUnitList(const DynamicArray<Unit> &copyme)
{
	Assert(copyme.Num() < k_MAX_ARMY_SIZE);
	if(copyme.Num() < k_MAX_ARMY_SIZE) {
		memcpy(m_array, copyme.m_array, sizeof(Unit) * copyme.m_nElements);
		m_nElements = copyme.m_nElements;
		UpdateMoveIntersection();
	}
}

sint32 CellUnitList::Del(const Unit &id)
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].m_id == id.m_id) {
			memmove(&m_array[i], &m_array[i+1], (m_nElements - i - 1) * sizeof(Unit));
			m_nElements--;
			UpdateMoveIntersection();
			return 1;
		}
	}
	return 0;
}

sint32 CellUnitList::DelIndex(const sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_nElements);
	if(index >= 0 && index < m_nElements) {
		memmove(&m_array[index], &m_array[index+1], (m_nElements - index - 1) * sizeof(Unit));
		m_nElements--;
		UpdateMoveIntersection();
		return 1;
	}
	return 0;
}

void CellUnitList::KillList(CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy)
{
	sint32 i;

	Unit tmpArray[k_MAX_ARMY_SIZE];
	for(i = m_nElements - 1; i >= 0; i--) {
		tmpArray[i].m_id = m_array[i].m_id;
	}
	sint32 n = m_nElements;
	for(i = n - 1; i >= 0; i--) {
		tmpArray[i].Kill(cause, killedBy);
	}
}

sint32 CellUnitList::IsPresent(const Unit &u)
{
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].m_id == u.m_id)
			return 1;
	}
	return 0;
}

void CellUnitList::UpdateMoveIntersection()
{
	sint32 i;
	m_moveIntersection = 0xffffffff;
	m_flags = k_CULF_IGNORES_ZOC |
		k_CULF_CANT_CAPTURE_CITY |
		k_CULF_CAN_SPACE_LAUNCH |
		k_CULF_CAN_SPACE_LAND;
	uint32 oldMoveBits;
	uint32 newMoveBits;

	for(i = 0; i < m_nElements; i++) {

		oldMoveBits = 0;
		if(!m_array[i].IsValid()) {
			continue;
		}
		newMoveBits = m_array[i].GetMovementType();

		if(newMoveBits & k_Unit_MovementType_Land_Bit)
			oldMoveBits |= k_BIT_MOVEMENT_TYPE_LAND;
		if(newMoveBits & k_Unit_MovementType_Sea_Bit)
			oldMoveBits |= k_BIT_MOVEMENT_TYPE_WATER;
		if(newMoveBits & k_Unit_MovementType_Air_Bit)
			oldMoveBits |= k_BIT_MOVEMENT_TYPE_AIR;
		if(newMoveBits & k_Unit_MovementType_Mountain_Bit)
			oldMoveBits |= k_BIT_MOVEMENT_TYPE_MOUNTAIN;
		if(newMoveBits & k_Unit_MovementType_Trade_Bit)
			oldMoveBits |= k_BIT_MOVEMENT_TYPE_TRADE;
		if(newMoveBits & k_Unit_MovementType_ShallowWater_Bit)
			oldMoveBits |= k_BIT_MOVEMENT_TYPE_SHALLOW_WATER;
		if(newMoveBits & k_Unit_MovementType_Space_Bit)
			oldMoveBits |= k_BIT_MOVEMENT_TYPE_SPACE;







		if(g_theUnitPool->IsValid(m_array[i])) {
			m_moveIntersection &= oldMoveBits;
			if(!m_array[i].IsIgnoresZOC()) {
				m_flags &= ~(k_CULF_IGNORES_ZOC);
			}
			if(!m_array[i].IsCantCaptureCity()) {
				m_flags &= ~(k_CULF_CANT_CAPTURE_CITY);
			}
		}
	}
}

void CellUnitList::ComputeStrength(double & attack,
								   double & defense,
								   double & ranged,
								   sint16 & defend_unit_count,
								   sint16 & ranged_unit_count,
								   double & land_bombard,
								   double & water_bombard,
								   double & air_bombard) const
{
	attack = 0.0;
	defense = 0.0;
	ranged = 0.0;
	defend_unit_count = 0;
	ranged_unit_count = 0;

	land_bombard = 0.0;
	water_bombard = 0.0;
	air_bombard = 0.0;

	double firepower;
	double hitpoints;

	double r;
	sint32 unit_type;
	UnitDynamicArray* cargo_list;
	int j;
	const UnitRecord *rec;

	for(int i = 0; i < m_nElements; i++) {
		if (!g_theUnitPool->IsValid(m_array[i]))
			continue;
		unit_type = m_array[i].GetType();
		rec = g_theUnitDB->Get(unit_type);
		firepower = double(g_theUnitDB->Get(unit_type)->GetFirepower());
        attack += (double) (rec->GetAttack()
			* m_array[i].GetHP()
			* firepower);
        defense += (double) (m_array[i]->GetPositionDefense(Unit(0))
			* m_array[i].GetHP()
			* firepower);
        r = rec->GetZBRangeAttack()
			* firepower;
        if ( r > 0.0 ) {
            ranged +=  r;
            ranged_unit_count++;
        }

		if (rec->GetCanBombardLand() ||
			rec->GetCanBombardMountain()) {
			land_bombard += (double) (rec->GetAttack()
				* double(m_array[i].GetHP())
				* firepower);
		}

		if (rec->GetCanBombardWater()) {
			water_bombard += (double) (rec->GetAttack()
				* double(m_array[i].GetHP())
				* firepower);
		}

		if (rec->GetCanBombardAir()) {
			air_bombard += (double) (rec->GetAttack()
				* double(m_array[i].GetHP())
				* firepower);
		}

		cargo_list = m_array[i]->GetCargoList();

		if (cargo_list != NULL)
		{
			for (j = 0; j < cargo_list->Num(); j++) {
				unit_type = cargo_list->Access(j).GetType();
				rec = g_theUnitDB->Get(unit_type);

				firepower = double(rec->GetFirepower());
				hitpoints = double(rec->GetMaxHP());
				attack += (double) (rec->GetAttack()
					* hitpoints	* firepower);
				defense += (double) (rec->GetDefense()
					* hitpoints	* firepower);
				r = rec->GetZBRangeAttack()
					* firepower;
				if ( r > 0.0 ) {
					ranged +=  r;
					ranged_unit_count++;
				}

				if (rec->GetCanBombardLand() ||
					rec->GetCanBombardMountain()) {
					land_bombard += (double) (rec->GetAttack()
						* hitpoints	* firepower);
				}

				if (rec->GetCanBombardWater()) {
					water_bombard += (double) (rec->GetAttack()
						* hitpoints	* firepower);
				}

				if (rec->GetCanBombardAir()) {
					air_bombard += (double) (rec->GetAttack()
						* hitpoints	* firepower);
				}
			}
		}
    }
}

double CellUnitList::GetAverageHealthPercentage()
{
	if(m_nElements < 1)
		return 0.0;

	double totalRatio = 0;
	sint32 i;
	for(i = 0; i < m_nElements; i++) {
		if(m_array[i].IsValid()) {
			double maxHp = m_array[i].GetDBRec()->GetMaxHP();
			if(maxHp == 0) {
				maxHp = 1;
			}
			totalRatio += m_array[i].GetHP() / maxHp;
		}
	}
	return totalRatio / double(m_nElements);
}
