//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Army data handling
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
// - Generate debug version when set.
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
// - Made some methods const.
// - Improved handling of space launched units.
// - Added a isstealth paramater in characterizeArmy method
// - Prevented leak report from unused static variables.
// - Added modification for increased bombard range in ::Bombard & ::PerformOrderHere
// - Improved destructor to clean up lists.
// - Implemented Immobile units.
// - Moved UnitValidForOrder to Unit.cpp to be able to access the Unit
//   properties. - April 24th 2005 Martin Gühmann
// - Teleevangelist unit does not need to have index 66 in unit database,
//   so that the soothsay message is replaced by the faith heal message.
//   Actual this should be replaced by new order. - April 30th 2005 Martin Gühmann
// - Implemented GovernmentModified for UnitDB - April 30th 2005 Martin Gühmann
// - Added unit record and civilisation information to some messageboxes.
// - Repaired crash when inciting an uprising succeeds.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Positions of units on transports are now updated. (Sep 9th 2005 Martin Gühmann)
// - Improved grouping checks.
// - SneakBomard check added to BombardCity and Bombard  by E 17-JAN-2006
// - SneakAttack added to ::Fight because it works here   by E 17-JAN-2006
// - Immobile (and CantGroup) don't work     by E 17-JAN-2006
// - CantPillage Implemented   by E 20-JAN-2006
// - CanBombardTiles implemented - tiles can be bombarded by E 20-JAN-2006
// - CollateralTileDamage Implemented by E 20-JAN-2006
// - AllTerrainAsImprovement NOT impelemented in DeductMoveCost
// - NonLethalBombard removed and put in UnitData::Bombard 15-FEB-2006
// - VerifyAttack modified to prevent accidental wars - 24-MAR-2006
// - verifyttack removed from bombard because the player should know where the point it 25-MAR-2006
// - CanCaptureTile added to Pillage and work by E 4-12-2006
// - CanBeGifted added to disband, allow humans to give units to AI by E 4-12-2006
// - DeniedtoEnemy added to DeductMove if an imp has this then when at war you dont
//   the tileimps move bonus only the base terrain (intent for railroads not roads)
//   by E 4-12-2006
// - Updated Multiple Attack flag by E 4-25-2006
// - Upgrade code added to Sleep by E 4-25-2006 (outcommented)
// - PrecisionStrike flag enabled by E; units with flag will only hit
//   buildings (bomb everywhere else though) in cities 4-26-2006
// - TargetsCivilians flag enabled by E; units with flag will only kill
//   civilians in cities (bomb everywhere else though) 4-26-2006
// - Added check in VerifyAttack to exclude barbarians 5-2-2006 by E
// - Added CanHarvest to BeginTurn for Units that have the flag CanHarvest and
//   are Entrenched by E 5-15-2006
// - Added Gold Cost to CanCaptureTile 5-16-2006 by E
// - Added SettleImprovement to beginturn 5-27-2006 by E
// - Armies with a CanSettleOn only unit are now recognized as settler armies. (May 21st 2006 Martin Gühmann)
// - Standardized code (May 21st 2006 Martin Gühmann)
// - Added HostileTerrain so units in terrain that has this value lose 5-27-2006 by E
// - Added Great Merchant Gold 5-27-2006 by E
// - Added barbariancamps to entrenched barbs  5-27-2006 by E
// - Added SettleBuilding check to convertcity, this way a religious unit can add
//   a building to the city converted. A possibility for using religion in Ctp2 by E 5-27-2006
// - Added tileimps that can upgrade by E 5-30-2006
// - Added minefield tileimps that deduct HP by E 5-30-2006
// - changed barbariancamps to barbariancities (E 6-07-2006)
// - Added new barbarian camps as a settle imp code (E 6-07-2006)
// - Added barbarianspawsnbarbarian (E 6-07-2006)
// - Changed barbariancamps to barbariancities (E 6-07-2006)
// - Made barbarians immune to hstile terrain (E 6-10-2006)
// - Implemented SpawnBarbairans units (E 6-10-2006)
// - Fixed CanCloak (25th 2006 Martin Gühmann)
// - Repaired some crashes.
// - Implemented EnablesPunativeAirstrikes Wonder flag
// - Implemented ConstDB ChanceLostAtSea
// - Added difficulty to make ai immune to sinking
// - Added slic message for ship sinking
// - Added CanUpgrade. Upgrade, CanUpgradeNoGold, and UpgradeNoGold methods.
// - Added UpgradeUnit to Begin Turn
// - Added a return true for AI in verifyattack (for testing)
// - Added random number for beginturn barbarian code
// - Moved the upgrade stuff into its own methods, however more work is needed.
//   (Dec 24th 2006 Martin Gühmann)
// - Added message for hostile terrain and guerrilla spawn
// - Readded message for ship sinking (removed later)
// - Added new limits for BarbarianSpawnBarbarian
// - Changed HostileTerain to only be in affect if you are at war with the owner
//   or it is unassigned
// - Added Slic execute commands
// - Modified sink to send the unit type 5-24-2007
// - Cleaned up beginturn moving stuff to ArmyData
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - Changed settlebuilding for convert to estabvlish building since it isn't settling
// - Added establishbuilding check to advertise, makes it more worthwhile - by E 27 Aug 2007
// - Units that are grouped into previously empty armies now show up on the map. (5-Aug-2007 Martin Gühmann)
// - Improved Ungroup and transport capacity methods. (5-Aug-2007 Martin Gühmann)
// - Bombard order is not given twice anymore. (30-Jan-2008 Martin Gühmann)
// - The player's cargo capacity is now calculated before the AI uses its
//   units and not afterwards. (3-Feb-2008 Martin Gühmann)
// - Added check move points option to CanAtLeastOneCargoUnloadAt. (8-Feb-2008 Martin Gühmann)
// - Throwing diplomatic parties costs now the displayed price. (9-Feb-2008 Martin Gühmann)
// - Added messages for failed slave raids, one for an alive slave and another
//   for a killed slaver. (9-Feb-2008 Martin Gühmann)
// - Expelling costs now move points. (9-Feb-2008 Martin Gühmann)
// - Separated the Settle event drom the Settle in City event. (19-Feb-2008 Martin Gühmann)
// - Special attack centers only if the auto center option on units and
//   cities is set. (23-Feb-2008 Martin Gühmann)
// - Merged finish move. (13-Aug-2008 Martin Gühmann)
// - Modified CheckActiveDefenders to only fire when a valid target is alive.
//   Also removed counterbombarding and returning active defense fire after
//   an active defender has fired, to remove the cheesy effect. (09-Mar-2009 Maq)
//	 Fixed expelling if at least one unit on a tile cannot be expelled, then
//	 none from that tile can be. (12-Apr-2009 Maq)
// - Changed occurances of UnitRecord::GetMaxHP to
//   UnitData::CalculateTotalHP. (Aug 3rd 2009 Maq)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "ArmyData.h"

#include "Agreement.h"
#include "AgreementMatrix.h"
#include "AgreementPool.h"
#include "AICause.h"
#include <algorithm>                    // std::min
#include "AdvanceRecord.h"
#include "Advances.h"
#include "ArmyPool.h"
#include "Barbarians.h"
#include "BuildingRecord.h"
#include "buildingutil.h"
#include "Cell.h"
#include "cellunitlist.h"
#include "citywindow.h"
#include "ConstRecord.h"
#include "CTP2Combat.h"
#include "DifficultyRecord.h"
#include "Diplomacy_Log.h"
#include "Diplomat.h"
#include "director.h"
#include <functional>                   // std::mem_fun_ref
#include "GameEventArgList.h"
#include "GameEventArgument.h"
#include "GameEventManager.h"
#include "GameSettings.h"
#include "gamesounds.h"
#include "Gold.h"
#include "GoodyHuts.h"
#include "GovernmentRecord.h"
#include "HappyTracker.h"
#include "mapanalysis.h"
#include "MaterialPool.h"
#include "MoveFlags.h"
#include "net_action.h"
#include "net_info.h"
#include "net_order.h"
#include "network.h"
#include "Order.h"
#include "OrderRecord.h"
#include "player.h"
#include "pollution.h"
#include "QuadTree.h"                   // g_theUnitTree
#include "QuickSlic.h"
#include "profileDB.h"
#include "radarmap.h"
#include "RandGen.h"                    // g_rand
#include "ResourceRecord.h"             // g_theResourceDB
#include "RiskRecord.h"
#include "Scheduler.h"
#include "SelItem.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "SlicSegment.h"
#include "soundmanager.h"               // g_soundManager
#include "SoundRecord.h"
#include "SpecialAttackInfoRecord.h"
#include "SpecialEffectRecord.h"
#include "StrDB.h"
#include "TerrainImprovementRecord.h"
#include "TerrainRecord.h"
#include "terrainutil.h"
#include "TerrImprovePool.h"
#include "tiledmap.h"
#include "TradePool.h"
#include "TradeRouteData.h"
#include "TurnCnt.h"
#include "UnitActor.h"
#include "UnitAstar.h"
#include "UnitData.h"
#include "UnitRecord.h"
#include "unitutil.h"
#include "WonderRecord.h"
#include "wonderutil.h"
#include "World.h"

extern AgreementPool *  g_theAgreementPool;
extern Diplomacy_Log *  g_theDiplomacyLog;
extern Pollution *      g_thePollution;
extern UnitAstar *      g_theUnitAstar;

BOOL    g_smokingCrack      = TRUE;
BOOL    g_useOrderQueues    = TRUE;

#define N_F  (1 << NORTH)
#define NE_F (1 << NORTHEAST)
#define E_F  (1 << EAST)
#define SE_F (1 << SOUTHEAST)
#define S_F  (1 << SOUTH)
#define SW_F (1 << SOUTHWEST)
#define W_F  (1 << WEST)
#define NW_F (1 << NORTHWEST)

namespace
{

uint32 const    MOVEMENT_ANYWHERE   = 0xffffffffu;

//----------------------------------------------------------------------------
//
// Name       : CityReport
//
// Description: Slic object for something happening at a city
//
// Parameters : what            : what happened (messagebox identifier)
//              city            : where it happened
//
//----------------------------------------------------------------------------
class CityReport : public SlicObject
{
public:
    CityReport
    (
        char const *    what,
        Unit const &    city
    )
    :   SlicObject      (what)
    {
        AddRecipient(city.GetOwner());
        AddCity(city);
    };
};

//----------------------------------------------------------------------------
//
// Name       : AggressorReport
//
// Description: Slic object for something a player did
//
// Parameters : what			: what happened (messagebox identifier)
//              actor           : the unit that did it
//              city            : where it happened
//
//----------------------------------------------------------------------------
class AggressorReport : public SlicObject
{
public:
    AggressorReport
    (
        char const *    what,
        Unit const &    actor,
        Unit const &    city
    )
    :   SlicObject      (what)
    {
        AddRecipient(actor.GetOwner());
        AddUnitRecord(actor.GetType());
        AddCity(city);
    };
};

//----------------------------------------------------------------------------
//
// Name       : VictimReport
//
// Description: Slic object for something that was done to a player
//
// Parameters : what            : what happened (messagebox identifier)
//              actor           : the unit that did it
//              city            : where it happened
//
//----------------------------------------------------------------------------
class VictimReport : public CityReport
{
public:
    VictimReport
    (
        char const *    what,
        Unit const &    actor,
        Unit const &    city
    )
    :   CityReport  (what, city)
    {
        AddUnitRecord(actor.GetType());
    };
};

//----------------------------------------------------------------------------
//
// Name       : IsSolist
//
// Description: Determine whether unit should not be grouped with others
//
// Parameters : u       : the unit
//
// Globals    : -
//
// Returns    : bool    : unit is immobile or otherwise forbidden to group
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool IsSolist(Unit const & u)
{
	return u.IsImmobile() || u.CantGroup();
}

} // namespace

sint32 * ArmyData::s_orderDBToEventMap = NULL;

ArmyData::ArmyData(const Army &army, const UnitDynamicArray &units)
:   GameObj                 (army.m_id),
    m_tempKillList          (NULL),
    m_attackedByDefenders   (new UnitDynamicArray),
    m_orders                (new PointerList<Order>),
    m_owner                 (-1),
    m_pos                   (),
    m_hasBeenAdded          (false),
    m_isPirating            (false),
    m_name                  (NULL),
    m_reentryTurn           (-1),
    m_reentryPos            (),
    m_debugStringColor      (0),
    m_killMeSoon            (new PointerList<KillRecord>),
    m_dontKillCount         (0),
    m_needToKill            (false),
    m_addOrdersAI           (true),
    m_debugString           (NULL)
{
    for (sint32 i = 0; i < units.Num(); ++i)
    {
        Insert(units.Get(i));
    }
}

ArmyData::ArmyData(const Army &army, const CellUnitList &units)
:   GameObj                 (army.m_id),
    m_tempKillList          (NULL),
    m_attackedByDefenders   (new UnitDynamicArray),
    m_orders                (new PointerList<Order>),
    m_owner                 (-1),
    m_pos                   (),
    m_hasBeenAdded          (false),
    m_isPirating            (false),
    m_name                  (NULL),
    m_reentryTurn           (-1),
    m_reentryPos            (),
    m_debugStringColor      (0),
    m_killMeSoon            (new PointerList<KillRecord>),
    m_dontKillCount         (0),
    m_needToKill            (false),
    m_addOrdersAI           (true),
    m_debugString           (NULL)
{
    for (sint32 i = 0; i < units.Num(); ++i)
    {
        Insert(units.Get(i));
    }
}

ArmyData::ArmyData(const Army &army, Unit &u)
:   GameObj                 (army.m_id),
    m_tempKillList          (NULL),
    m_attackedByDefenders   (new UnitDynamicArray),
    m_orders                (new PointerList<Order>),
    m_owner                 (-1),
    m_pos                   (),
    m_hasBeenAdded          (false),
    m_isPirating            (false),
    m_name                  (NULL),
    m_reentryTurn           (-1),
    m_reentryPos            (),
    m_debugStringColor      (0),
    m_killMeSoon            (new PointerList<KillRecord>),
    m_dontKillCount         (0),
    m_needToKill            (false),
    m_addOrdersAI           (true),
    m_debugString           (NULL)
{
    Insert(u);
}

ArmyData::ArmyData(const Army &army)
:   GameObj                 (army.m_id),
    m_tempKillList          (NULL),
    m_attackedByDefenders   (new UnitDynamicArray),
    m_orders                (new PointerList<Order>),
    m_owner                 (-1),
    m_pos                   (),
    m_hasBeenAdded          (false),
    m_isPirating            (false),
    m_name                  (NULL),
    m_reentryTurn           (-1),
    m_reentryPos            (),
    m_debugStringColor      (0),
    m_killMeSoon            (new PointerList<KillRecord>),
    m_dontKillCount         (0),
    m_needToKill            (false),
    m_addOrdersAI           (true),
    m_debugString           (NULL)
{
}

ArmyData::ArmyData(CivArchive &archive)
:   GameObj                 (0),
    m_tempKillList          (NULL),
    m_attackedByDefenders   (new UnitDynamicArray),
    m_orders                (new PointerList<Order>),
    m_owner                 (-1),
    m_pos                   (),
    m_hasBeenAdded          (false),
    m_isPirating            (false),
    m_name                  (NULL),
    m_reentryTurn           (-1),
    m_reentryPos            (),
    m_debugStringColor      (0),
    m_killMeSoon            (new PointerList<KillRecord>),
    m_dontKillCount         (0),
    m_needToKill            (false),
    m_addOrdersAI           (true),
    m_debugString           (NULL)
{
    Serialize(archive);
}

ArmyData::~ArmyData()
{
    if (m_orders)
    {
        m_orders->DeleteAll();
        delete m_orders;
    }
    if (m_killMeSoon)
    {
        m_killMeSoon->DeleteAll();
        delete m_killMeSoon;
    }

    delete m_attackedByDefenders;
    delete m_debugString;
    delete m_name;
    delete m_tempKillList;
}

void ArmyData::Serialize(CivArchive &archive)
{
    GameObj::Serialize(archive);
    CellUnitList::Serialize(archive);
    m_attackedByDefenders->Serialize(archive);
    m_pos.Serialize(archive);

    sint32 ocount, i;
    uint8 hasChild;

    if(archive.IsStoring()) {
        archive << m_owner;
        archive << m_reentryTurn;
	m_reentryPos.Serialize(archive);
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
        archive >> m_reentryTurn;
	m_reentryPos.Serialize(archive);
        m_dontKillCount = archive.GetUINT8();
        m_needToKill    = archive.GetUINT8() != 0;
        m_hasBeenAdded  = archive.GetUINT8() != 0;
        m_isPirating    = archive.GetUINT8() != 0;

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

//----------------------------------------------------------------------------
//
// Name       : ArmyData::SetOwner
//
// Description: Set owner for an empty army
//
// Parameters : PLAYER_INDEX p
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::SetOwner(PLAYER_INDEX p)
{
    Assert(m_nElements <= 0);
    m_owner = p;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::Insert
//
// Description: Insert Unit id into this army
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool true when done.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::Insert(const Unit &id)
{
    Assert(m_nElements < k_MAX_ARMY_SIZE);
    if(m_nElements >= k_MAX_ARMY_SIZE)
        return false;

    if(m_nElements > 0) {
        MapPoint hisPos;
        id.GetPos(hisPos);
        Assert(m_pos == hisPos);
        if(m_pos != hisPos)
            return false;

        Assert(m_array[0].GetOwner() == id.GetOwner());
        if(m_array[0].GetOwner() != id.GetOwner())
            return false;

    } else {
        m_owner = id.GetOwner();
        id.GetPos(m_pos);
    }

    CellUnitList::Insert(id);
    Unit u(id);
    u.SetArmy(Army(m_id));

    if(m_nElements > 0) {
        g_director->AddShow(id);
    }

    return true;
}

uint32 ArmyData::GetMovementType() const
{
    uint32 tmp = MOVEMENT_ANYWHERE;

    for(sint32 i = 0; i < m_nElements; i++) {
        tmp &= m_array[i].GetMovementType();
    }

    return tmp;
}

uint32 ArmyData::GetCargoMovementType() const
{
    uint32 tmp = MOVEMENT_ANYWHERE;

    for(sint32 i = 0; i < m_nElements; i++)
    {
        const UnitDynamicArray * cargo =
            m_array[i].AccessData()->GetCargoList();

        if (cargo)
        {
            for(sint32 j = 0; j < cargo->Num(); j++) {
                tmp &= cargo->Access(j).GetMovementType();
            }
        }
    }

    return tmp;
}

sint32 ArmyData::GetCargoNum() const
{
    sint32 num = 0;

    for(sint32 i = 0; i < m_nElements; ++i)
    {
        UnitDynamicArray const * cargo =
            m_array[i].AccessData()->GetCargoList();

        if(cargo)
            num += cargo->Num();
    }

    return num;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::HasCargo
//
// Description: Test if this army is carrying cargo.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool true if at least one unit in the CellUnitList has cargo.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::HasCargo() const
{
    for(sint32 i = 0; i < m_nElements; ++i)
    {
        UnitDynamicArray const * cargo =
            m_array[i].AccessData()->GetCargoList();

        if (cargo && cargo->Num() > 0)
            return true;
    }

    return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::HasCargoOnlyStealth
//
// Description: Test if cargo from all units in CellUnitList are only stealth
//				units.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool true if all the cargo is only is stealth units.
//
// Remark(s)  : Assumes there is at least one unit with at least one cargo.
//
//----------------------------------------------------------------------------
bool ArmyData::HasCargoOnlyStealth() const
{
    for(sint32 i = 0; i < m_nElements; ++i)
    {
        UnitDynamicArray const * cargo =
            m_array[i].AccessData()->GetCargoList();

		if (cargo && cargo->Num() > 0)
		{
			sint32	cargoCount = cargo->Num();

			for(sint32 j = 0; j < cargoCount; j++)
			{
				UnitRecord const * rec = m_array[i]->GetDBRec();
				rec = cargo->Access(j)->GetDBRec();
				if (!rec) continue;

				if (!rec->GetVisionClassStealth())
					return false;
			}
		}
    }
    return true;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::GetCargo
//
// Description: Test if this army is carrying cargo and fill in the function's parameters.
//
// Parameters : sint32 &transports  : the number of transports
//            : sint32 &max         : the max cargo space available
//            : sint32 &empty       : the current cargo space available (max - used)
//
// Globals    : g_player            :
//
// Returns    : bool true if at least one unit in the CellUnitList can transport.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::GetCargo(sint32 &transports, sint32 &max, sint32 &empty) const
{
    transports      = 0;
    max             = 0;
    empty           = 0;
    sint32  used    = 0;

    for (sint32 i = 0; i < m_nElements; ++i)
    {
        UnitDynamicArray const *    cargo = m_array[i].AccessData()->GetCargoList();

        if (cargo)
        {
            used += cargo->Num();
        }

        if (m_array[i].GetDBRec()->HasCargoData())
        {
            UnitRecord::CargoData const *   cargoData   =
                m_array[i].GetDBRec()->GetCargoDataPtr();
            sint32 const                    tmp         =
                cargoData ? cargoData->GetMaxCargo() : 0;

            max += tmp;
            if (tmp > 0)
            {
                ++transports;
            }
        }
    }

    Assert( max >= used );
    Assert( max >= transports);
    empty = max - used;

    return (transports > 0);
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CargoCanEnter
//
// Description: Test if some unit that this army is carrying as cargo can unload into pos.
//
// Parameters : MapPoint &pos      : the landing position
//
// Globals    : g_theWorld
//
// Returns    : bool true if at least one of the cargo members can unload into pos.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CargoCanEnter(const MapPoint &pos) const
{
    Cell *cell = g_theWorld->GetCell(pos);

    for(sint32 i = 0; i < m_nElements; i++)
    {
        const UnitDynamicArray * cargo =
            m_array[i].AccessData()->GetCargoList();

        if (cargo)
        {
            for(sint32 j = 0; j < cargo->Num(); j++)
            {
                if (cell->CanEnter(cargo->Access(j).GetMovementType()))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CountMovementTypeSea
//
// Description: Count how many ships are in this army.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : sint16             : the number of ships in this army.
//
// Remark(s)  : "ships" means any unit with MovementType: Sea in it's DB rec
//
//----------------------------------------------------------------------------
size_t ArmyData::CountMovementTypeSea() const
{
    size_t count = 0;

    for(sint32 i = 0; i < m_nElements; ++i)
    {
        if (m_array[i].GetMovementTypeSea())
        {
            ++count;
        }
    }

    return count;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanSettle
//
// Description: Test if some unit in this army can settle in pos.
//
// Parameters : MapPoint &pos
//
// Globals    : -
//
// Returns    : bool true if at least one member of the CellUnitList can settle in pos.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanSettle(const MapPoint &pos) const
{
    for(sint32 i = 0; i < m_nElements; i++)
    {
        if (m_array[i].CanSettle(pos))
            return true;
    }
    return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanSettle
//
// Description: Test if some unit in this army can settle.
//
// Parameters : -
//
// Globals    : g_theUnitPool : (instantiated) units in the game
//
// Returns    : bool true if at least one member of the CellUnitList can
//              settle anywhere.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanSettle() const
{
	for(sint32 i = 0; i < m_nElements; i++)
	{
		Assert(m_array[i].IsValid());
		if (    m_array[i].IsValid()
		     && (   m_array[i].GetDBRec()->GetSettle()
		         || m_array[i].GetDBRec()->GetNumCanSettleOn() > 0
		        )
		   )
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanTransport
//
// Description: Test if some unit in this army can carry cargo.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanTransport() const
{
    for(sint32 i = 0; i < m_nElements; ++i)
    {
        sint32 const	cargo = m_array[i].GetData()->GetCargoCapacity();
        if (cargo > 0)
            return true;
    }

    return false;
}

// not used
bool ArmyData::CanPatrol() const
{
    for(sint32 i = 0; i < m_nElements; i++)
    {
        if(m_array[i].GetDBRec()->GetCanPatrol())
            return true;
    }
    return false;
}

// Returns true if this army is a sentinal.
bool ArmyData::IsAsleep() const
{
	return m_array[0].IsAsleep();
}

// Put this army on sentinal duty.
void ArmyData::Sleep()
{
	for(sint32 i = m_nElements - 1; i >= 0; i--)
	{
		if(!m_array[i]->IsAsleep())
		{
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SleepUnit,
			                       GEA_Unit, m_array[i],
			                       GEA_End);
		}
	}
}

// Activate this army from sentinal duty.
void ArmyData::WakeUp()
{
	for(sint32 i = 0; i < m_nElements; i++)
	{
		if(m_array[i]->IsAsleep()){
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_WakeUnit,
			                       GEA_Unit, m_array[i],
			                       GEA_End);
		}
	}
}

// Returns true if this army is entrenched (fortified)
bool ArmyData::IsEntrenched() const
{
	return m_array[0].IsEntrenched();
}

// Returns true if this army is entrenching
bool ArmyData::IsEntrenching() const
{
	return m_array[0].IsEntrenching();
}

// Returns true if each member of this army can entrench
bool ArmyData::CanEntrench() const
{
    for(sint32 i = 0; i < m_nElements; i++)
    {
        if (!m_array[i].CanEntrench())
            return false;
    }
    return true;
}

// Entrench (fortify) this army
void ArmyData::Entrench()
{
    for(sint32 i = 0; i < m_nElements; i++)
    {
        g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_EntrenchUnit,
                               GEA_Unit, m_array[i],
                               GEA_End);

    }
}

// Detrench (unfortify) this army
void ArmyData::Detrench()
{
    for(sint32 i = 0; i < m_nElements; i++)
    {
        g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_DetrenchUnit,
                               GEA_Unit, m_array[i],
                               GEA_End);

    }
}

// not used
bool ArmyData::IsPatrolling() const
{
    return false;
}


void ArmyData::GetActors(Unit &excludeMe, UnitActor **restOfStack)
{
    sint32			n = 0;

    for(sint32 i = 0; i < m_nElements; i++)
    {
        UnitActor * a = m_array[i].GetActor();
        if (a != excludeMe.GetActor()) {
            restOfStack[n++] = a;
        }
    }
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::GroupArmy
//
// Description: Group the Army army with this army
//
// Parameters : Army &army      : the incoming army
//
// Globals    : g_gevManager    :
//              g_player        : player array
//
// Returns    : -
//
// Remark(s)  : Does not allow any grouping of immobile units
//
//----------------------------------------------------------------------------
void ArmyData::GroupArmy(Army &army)
{
    sint32 i;
    DPRINTF(k_DBG_GAMESTATE, ("Army 0x%lx grouping army 0x%lx", m_id, army.m_id));

    // PFT 17 Mar 05, E 18-Oct-2005:
    // Prevent some categories of units from grouping.

    // Check incoming army for restrictions
    for (i = 0; i < army.Num(); ++i)
    {
        if (IsSolist(army[i]))
        {
             return;
        }
    }

    // Check this army for restrictions
    for (i = 0; i < m_nElements; ++i)
    {
        if (IsSolist(m_array[i]))
        {
             return;
        }
    }

    // both armies ok
    bool atLeastOneAsleep = false;

    for(i = army.Num() - 1; i >= 0; i--) {
        DPRINTF(k_DBG_GAMESTATE, ("Inserting unit 0x%lx\n", army[i].m_id));
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
    //fixed, PFT 07 apr 05. If somebody is entrenched, then entrench everyone.
    for(i = 0; i < m_nElements; i++) {
        if(m_array[i].IsEntrenching() || m_array[i].IsEntrenched()) {
            g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_EntrenchOrder,
                                   GEA_Army, m_id,
                                   GEA_End);

            break;
        }
    }
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::GroupAllUnits
//
// Description: Group all the (mobile) units in the cell's CellUnitList *m_unit_army into this army
//
// Parameters : -
//
// Globals    : g_theUnitPool       :
//              g_network           : multiplayer manager
//              g_theWorld          : the map
//              g_gevManager        :
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::GroupAllUnits()
{
    DPRINTF(k_DBG_GAMESTATE, ("Army 0x%lx grouping everyone\n", m_id));
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
                //exclude immobile units, PFT 17 Mar 05
                if (IsSolist(ul->Access(i)))
                    continue;

                ul->Access(i).ChangeArmy(Army(m_id), CAUSE_NEW_ARMY_GROUPING);
                DPRINTF(k_DBG_GAMESTATE, ("Grouped unit 0x%lx\n",
                                          ul->Access(i).m_id));
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

//----------------------------------------------------------------------------
//
// Name       : ArmyData::GroupUnit
//
// Description: Group the Unit unit with this army
//
// Parameters : -
//
// Globals    : g_theUnitPool   :
//            : g_network       : multiplayer manager
//            : g_player        : player array [see Player::InitPlayer for initialized player data]
//
// Returns    : -
//
// Remark(s)  : But not if it's immobile
//
//----------------------------------------------------------------------------
void ArmyData::GroupUnit(Unit unit)
{
    DPRINTF(k_DBG_GAMESTATE, ("Army 0x%lx grouping unit 0x%lx\n", m_id, unit.m_id));

    Assert(unit.IsValid());
    if (!unit.IsValid())
    {
        if (g_network.IsHost() && !g_network.IsLocalPlayer(m_owner))
        {
            g_network.Resync(m_owner);
        }
        else if (g_network.IsClient())
        {
            g_network.RequestResync(RESYNC_INVALID_UNIT);
        }
        return;
    }

    // Exclude some categories of units
    if (IsSolist(unit))
    {
        return;
    }
    for(sint32 j = 0; j < m_nElements; ++j)
    {
        if (IsSolist(m_array[j]))
        {
            return;
        }
    }

    MapPoint upos;
    unit.GetPos(upos);
    if(m_nElements == 0) {
        m_pos = upos;
    }

    if(upos == m_pos) {

        if (unit.GetArmy().IsValid())
        {
	    // SetRemoveCause used to be called here which had no effect and was removed
        }
        else
        {
            Assert(unit.GetArmy().IsValid());
        }

        g_player[m_owner]->
            RemoveUnitReferenceFromPlayer(unit, CAUSE_REMOVE_ARMY_GROUPING, m_owner);

        unit.ChangeArmy(Army(m_id), CAUSE_NEW_ARMY_GROUPING);
    }

    if (IsAsleep() || unit.Flag(k_UDF_IS_ASLEEP)) {
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

// Ungroup this army into it's constituent units
void ArmyData::UngroupUnits()
{
	RemainNumUnits(0);
}

void ArmyData::RemainNumUnits(sint32 remain)
{
	remain = std::max(remain - 1, 0);
	for(sint32 i = m_nElements - 1; i > remain; i--)
	{
		Army newArmy = g_player[m_owner]->GetNewArmy(CAUSE_NEW_ARMY_UNGROUPING_ORDER);
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddUnitToArmy,
		                       GEA_Unit, m_array[i],
		                       GEA_Army, newArmy,
		                       GEA_Int, CAUSE_NEW_ARMY_UNGROUPING_ORDER,
		                       GEA_End);
	}
}

void ArmyData::Split(sint32 remain)
{
	remain = std::max(remain - 1, 0);
	Army newArmy = g_player[m_owner]->GetNewArmy(CAUSE_NEW_ARMY_UNGROUPING_ORDER);
	for(sint32 i = m_nElements - 1; i > remain; i--)
	{
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddUnitToArmy,
							   GEA_Unit, m_array[i],
							   GEA_Army, newArmy,
							   GEA_Int, CAUSE_NEW_ARMY_UNGROUPING_ORDER,
							   GEA_End);
	}
}

// If this army has a m_tempKillList, insert all the units at MapPoint &pos into it.
// Then cut all the improvements at pos.

// this is unused and may be left over from some previous nuke code
void ArmyData::CityRadiusFunc(const MapPoint &pos)
{
    if(m_tempKillList) {
        Cell *cell = g_theWorld->GetCell(pos);
        if (cell->UnitArmy())
        {
            for (sint32 i = 0; i < cell->UnitArmy()->Num(); i++)
            {
                m_tempKillList->Insert(cell->UnitArmy()->Access(i));
            }
        }
    }
    g_theWorld->CutImprovements(pos);
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::GetActiveDefenders
//
// Description:
//
// Parameters : UnitDynamicArray &input  : possibleDefenders
//              UnitDynamicArray &output : activeDefenders
//              bool isCargoPodCheck     : true if CargoPods are to be included in input array
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : See CheckActiveDefenders. The defenders must be at war with this army.
//
//----------------------------------------------------------------------------
void ArmyData::GetActiveDefenders(UnitDynamicArray &input,
                                  UnitDynamicArray &output,
                                  bool isCargoPodCheck)
{
    sint32 j;
    sint32 n = input.Num();
    sint32 numDefenders = 0;
    sint32 owner = m_array[0].GetOwner();
    output.Clear();

    for (sint32 i = 0; i < n; i++) {

        if(input[i].GetOwner() == owner)
            continue;
        if(!input[i].CanActivelyDefend(Army(m_id))) {
            if(!(isCargoPodCheck &&
                 input[i].GetDBRec()->GetDefendAir()))
                continue;
        }

        if (!g_player[owner]->HasWarWith(input[i].GetOwner()))
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

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CheckActiveDefenders
//
// Description: .
//
// Parameters : MapPoint &pos        :
//              bool isCargoPodCheck : true if CargoPods are to be included in the check
//
//            : g_theUnitTree
//
// Returns    : true if pos contains units who can actively defend against this army
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CheckActiveDefenders(MapPoint &pos, bool cargoPodCheck)
{
    if(m_nElements <= 0)
        return false;

    // NuclearAttack is the sound and effect specific to vanilla Nuke units
    if (m_nElements == 1 &&
        m_array[0].GetDBRec()->HasNuclearAttack())
        return false;

    sint32 maxActiveDefenseRange = unitutil_MaxActiveDefenseRange();
    MapPoint topleft(pos.x - maxActiveDefenseRange, pos.y);

    UnitDynamicArray possibleDefenders;
    g_theUnitTree->SearchRect(possibleDefenders, topleft,
                              static_cast<sint16>(maxActiveDefenseRange * 2 + 1),
                              static_cast<sint16>(maxActiveDefenseRange * 2 + 1),
                              ~(1 << m_array[0].GetOwner()));
    if(possibleDefenders.Num() <= 0)
        return false;

    UnitDynamicArray activeDefenders;
    GetActiveDefenders(possibleDefenders, activeDefenders, cargoPodCheck);
    sint32 defenderOwner    = PLAYER_UNASSIGNED;
    sint32 owner            = GetOwner();

    UnitDynamicArray deadDefenders;
    sint32 i, j, r, n = activeDefenders.Num();
    for (i = 0; i < n; i++)
    {
        Unit        ta      = activeDefenders[i];
        if (!ta.CanSee(Army(m_id)))
            continue;
        Unit        td      =
            GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());
        MapPoint    dpos    = ta.RetPos();

		UnitRecord const *	rec	= ta.GetDBRec();
		sint32 validtargets = 0;
		if (rec->GetDefendLand() && validtargets == 0) {
			for(r = 0; r < m_nElements; r++) {
				if(!(m_array[r].GetHP() < 1.0)
				&& m_array[r].GetMovementTypeLand()) {
					validtargets++;
					continue;
				}
			}
		}
		if (rec->GetDefendMountain() && validtargets == 0) {
			for(r = 0; r < m_nElements; r++) {
				if(!(m_array[r].GetHP() < 1.0)
				&& m_array[r].GetMovementTypeMountain()) {
					validtargets++;
					continue;
				}
			}
		}
		if (rec->GetDefendWater() && validtargets == 0) {
			for(r = 0; r < m_nElements; r++) {
				if(!(m_array[r].GetHP() < 1.0)
				&& (m_array[r].GetMovementTypeSea()
				 || m_array[r].GetMovementTypeShallowWater())) {
					validtargets++;
					continue;
				}
			}
		}
		if (rec->GetDefendAir() && validtargets == 0) {
			for(r = 0; r < m_nElements; r++) {
				if(!(m_array[r].GetHP() < 1.0)
				&& m_array[r].GetMovementTypeAir()) {
					validtargets++;
					continue;
				}
			}
		}
		if (rec->GetDefendSpace() && validtargets == 0) {
			for(r = 0; r < m_nElements; r++) {
				if(!(m_array[r].GetHP() < 1.0)
				&& m_array[r].GetMovementTypeSpace()) {
					validtargets++;
					continue;
				}
			}
		}
		if (validtargets <= 0) break;

        g_director->AddAttackPos(ta, m_pos);
        g_slicEngine->RunActiveDefenseTriggers(ta, td);

		activeDefenders[i].Bombard(*this, false);
		activeDefenders[i].SetFlag(k_UDF_USED_ACTIVE_DEFENSE);

		defenderOwner = activeDefenders[i].GetOwner();

		Cell *dcell = g_theWorld->GetCell(dpos);

		for(j = 0; j < m_nElements; j++) {
			if(!(m_array[j].GetHP() < 1.0)) {
				for(sint32 k = 0; k < dcell->GetNumUnits(); k++) {
					if(dcell->AccessUnit(k).GetHP() < 1.0 &&
                       !deadDefenders.IsPresent(dcell->AccessUnit(k)))
                        deadDefenders.Insert(dcell->AccessUnit(k));
                }
			}
		}
	}

    if(defenderOwner < 0)
        return false;

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
    return true;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::BeginTurn
//
// Description: begin this army's turn.
//
// Parameters : -
//
// Globals    : g_theWorld:             World properties
//              g_player:               List of players
//              g_theConstDB:           The const database
//              g_network
//              g_gevManager
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::BeginTurn()
{
//EMOD to add Harvesting Units 5-15-2006

/// @bug  This will cause problems when units are being airlifted
///       (k_CULF_IN_SPACE), and do not have a valid location on the map.
/// @todo Convert the loops to procedures, to reduce the length of the BeginTurn
///       method. 250 lines is way too long. //moved some
///       Move stuff to UnitData, when it only concerns units, not armies.
///       Efficiency note: do not compute cell/terrain stuff inside the loop - all
///       units of the army are supposed to be at the same location.
///       Define a better cause than CAUSE_REMOVE_ARMY_DISBANDED when the army is
///       not disbanded at all.

	// EMODs
	if(CheckSink())
	{
		return;
	}

	CheckHostileTerrain();
	CheckMineField();
	BarbarianSpawning();
	//Upgrade AI
	if(g_player[m_owner]->IsRobot()) // Added so human units don't automatically upgrade 3-27-2007
	{
		if(g_theProfileDB->GetValueByName("Upgrade"))
		{
			Upgrade();
		}
	}

	/// @todo remove this or implement this
//If diff->chokeunit and unit is surrounded
//	RadiusIterator it(cityPos, m_sizeIndex);
//	for(it.Start(); !it.End(); it.Next()) {
//		if(g_theWorld->GetArmy(it.Pos()), owner != m_owner
//			m_array[i].DeductHP(1);

//END EMOD

    m_flags &= ~(k_CULF_EXECUTED_THIS_TURN);
    if(m_isPirating) {//then collect gold from trade routes that this army is pirating
        Cell *cell = g_theWorld->GetCell(m_pos);
        Assert(cell);
        if(cell) {
            sint32 i;
            sint32 piratedByMe = 0;
            for(i = 0; i < cell->GetNumTradeRoutes(); i++) {
                TradeRoute route = cell->GetTradeRoute(i);
		if(!route.IsActive()) // skip deactivated routes (only exist for drawing until revisited, see #256)
		    continue;

                if(route->GetPiratingArmy().m_id == m_id) {
		    sint32 pgold = static_cast<sint32>(route->GetValue() * g_theConstDB->Get(0)->GetPiracyWasteCoefficient());
                    g_player[m_owner]->AddGold(pgold);
					//added pirated strategic good?
                    piratedByMe++;

		    Unit fromCity, toCity;
		    fromCity = route.GetSource();
		    toCity = route.GetDestination();

		    ROUTE_TYPE type;
		    sint32 good;
		    route.GetSourceResource(type, good);
					
		    SlicObject * so = new SlicObject("043TradePiratedBonus");
		    so->AddRecipient(toCity.GetOwner());
		    so->AddGood(good);
		    so->AddCity(fromCity); // sender
		    so->AddCity(toCity); // recipent, this city is the destination
		    so->AddCivilisation(GetOwner());
		    so->AddGold(g_theResourceDB->Get(good)->GetFood()); // missuse to pass integer to msg
		    so->AddGold(g_theResourceDB->Get(good)->GetProduction()); // missuse to pass integer to msg
		    so->AddGold(g_theResourceDB->Get(good)->GetGold()); // missuse to pass integer to msg
		    g_slicEngine->Execute(so);
		
		    SlicObject * so1 = new SlicObject("044TradePirateGold");
		    so1->AddRecipient(GetOwner());
		    so1->AddGold(pgold) ;
		    so1->AddGood(good);
		    so1->AddCity(fromCity);
		    so1->AddCity(toCity);
		    so1->AddCivilisation(fromCity.GetOwner());
		    g_slicEngine->Execute(so1);

		    SlicObject * so2 = new SlicObject("045TradePirated");
		    so2->AddRecipient(fromCity.GetOwner());
		    so2->AddGold(route->GetValue()) ;
		    so2->AddGood(good);
		    so2->AddCity(fromCity);
		    so2->AddCity(toCity);
		    so2->AddCivilisation(GetOwner());
		    g_slicEngine->Execute(so2);
                }
            }
            if(piratedByMe < 1) {
                StopPirating(); // if no route was successfully pirated by me, which happens if routes got removed; moving the army/unit along a route also calls StopPirating probably from AddOrders or AutoAddOrders
            }
        }
    }

    if (!g_player[m_owner]->m_can_use_sea_tab)
    {
        g_player[m_owner]->m_can_use_sea_tab = (CountMovementTypeSea() > 0);
    }

    if(m_flags & k_CULF_IN_SPACE) {
        if(NewTurnCount::GetCurrentRound() >= m_reentryTurn) { // > ensures that space plan can reenter later in case city cannot hold any more units at that moment
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

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanFight
//
// Description: .
//
// Parameters : CellUnitList &defender :
//
// Globals    : g_theWorld             : The Game's world
//
// Returns    : true if this army and the defending units in CellUnitList &defender can fight
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanFight(CellUnitList &defender)
{
	sint32 i;

	bool haveCombatUnits = false;
    for(i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetAttack() > 0) {
			haveCombatUnits = true;
            break;
        }
    }
    if(!haveCombatUnits)
		return false;

    MapPoint pos;
    defender.GetPos(pos);
    if(g_theWorld->IsCity(pos)) {
		return true;
    }

    for(i = 0; i < m_nElements; i++) {
        for(sint32 j = 0; j < defender.Num(); j++) {
			const UnitRecord *rec = m_array[i].GetDBRec();
            if(defender[j].IsSubmarine() && !rec->GetCanAttackUnderwater())
                continue;
            if(rec->GetCanAttack() & defender[j].GetDBRec()->GetMovementType())
				return true;

			// EMOD add CantBeAttacked / Peacekeeper here??
			// Add Peacekeeper wonder flag where if you are
			// at peace the enemy cant declare war and cant
			// attack your troops?

			if(!defender[j].GetDBRec()->GetMovementTypeSea()
			&& !defender[j].GetDBRec()->GetMovementTypeAir()
			&& g_theWorld->IsWater(pos)
			&&(rec->GetCanAttack() &
			  (k_BIT_MOVEMENT_TYPE_SHALLOW_WATER | k_BIT_MOVEMENT_TYPE_WATER))
			){
				return true;
            }
        }
    }
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::InvestigateCity
//
// Description: Order this army to investigate a city at MapPoint point.
//
// Parameters : MapPoint      point
//
// Globals    : -
//
// Returns    : ORDER_RESULT          : attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::InvestigateCity(const MapPoint &point)
{
    Unit c = GetAdjacentCity(point);
    if(c.m_id == 0)
        return ORDER_RESULT_ILLEGAL;

    if(c.GetOwner() == m_owner) {
        return ORDER_RESULT_ILLEGAL;
    }

    for(sint32 i = 0; i < m_nElements; i++) {
        if(m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN))
            continue;

        if(m_array[i].GetDBRec()->HasInvestigateCity()) {
            AddSpecialActionUsed(m_array[i]);
            //InformAI(UNIT_ORDER_INVESTIGATE_CITY, point); //does nothing here but could be implemented

            return m_array[i].InvestigateCity(c);
        }
    }
    return ORDER_RESULT_ILLEGAL;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::NullifyWalls
//
// Description: Order this army to nullify the city walls at MapPoint point.
//
// Parameters : MapPoint
//
// Globals    : -
//
// Returns    : ORDER_RESULT            : attempt success/failure indication
//
// Remark(s)  : calls UnitData::NullifyWalls, which is zeroed out
//
//----------------------------------------------------------------------------
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
        if(m_array[i].GetDBRec()->GetNullifyCityWalls()) {
            AddSpecialActionUsed(m_array[i]);
            InformAI(UNIT_ORDER_NULLIFY_WALLS, point);

            return m_array[i].NullifyWalls(c);
        }
    }
    return ORDER_RESULT_ILLEGAL;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::StealTechnology
//
// Description: Order this army to steal technology from a city at MapPoint point.
//
// Parameters : MapPoint
//
// Globals    : g_player                : player array
//            : g_network               :
//            : g_theAdvanceDB
//            : g_slicEngine
//
// Returns    : ORDER_RESULT            : attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::StealTechnology(const MapPoint &point)
{
    Unit c = GetAdjacentCity(point);
    if(c.m_id == 0)
        return ORDER_RESULT_ILLEGAL;

    if(c.GetOwner() == m_owner)
        return ORDER_RESULT_ILLEGAL;

    for(sint32 i = 0; i < m_nElements; i++) {
        if(m_array[i].GetDBRec()->HasStealTechnology()) {
            sint32 num;
            uint8 *canSteal = g_player[m_owner]->m_advances->CanAskFor(g_player[c.GetOwner()]->m_advances,
                                                                       num);
            if(num > 0)
            {
                if(g_player[m_owner]->IsRobot()
                &&(!g_network.IsClient()
                || !g_network.IsLocalPlayer(m_owner))
                ){
                    delete [] canSteal;
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

//----------------------------------------------------------------------------
//
// Name       : ArmyData::InciteRevolution
//
// Description: Order this army to incite a revolution in a city at MapPoint point.
//
// Parameters : MapPoint
//
// Globals    : g_gevManager            :
//
// Returns    : ORDER_RESULT            : attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
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

		if(m_array[i].GetDBRec()->HasInciteRevolution()) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_InciteRevolutionUnit,
								   GEA_Unit, m_array[i].m_id,
								   GEA_City, c.m_id,
								   GEA_End);


			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SubGold,
								   GEA_Player, m_owner,
								   GEA_Int, cost,
								   GEA_End);

			AddSpecialActionUsed(m_array[i]);

			//InformAI(UNIT_ORDER_INCITE_REVOLUTION, point);//does nothing here but could be implemented

			return ORDER_RESULT_INCOMPLETE;
		}
	}
	return ORDER_RESULT_ILLEGAL;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::AssassinateRuler
//
// Description: Order this army to assassinate a ruler at MapPoint point.
//
// Parameters : MapPoint
//
// Globals    : g_gevManager
//            : g_slicEngine
//
// Returns    : ORDER_RESULT            : attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::AssassinateRuler(const MapPoint &point)
{
	Unit c = GetAdjacentCity(point);
	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN))
			continue;
		if(m_array[i].GetDBRec()->HasAssasinateRuler()) {
			AddSpecialActionUsed(m_array[i]);

			//InformAI(UNIT_ORDER_ASSASSINATE, point); //does nothing here but could be implemented
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AssassinateRulerUnit,
								   GEA_Unit, m_array[i].m_id,
								   GEA_City, c.m_id,
								   GEA_End);
			Unit u = m_array[i];

			g_slicEngine->Execute
			    (new CityReport("911ConductHitCompleteVictim", c));
			g_slicEngine->Execute
			    (new AggressorReport("911ConductHitCompleteAttacker", u, c));

			return ORDER_RESULT_INCOMPLETE;
		}
	}

	g_slicEngine->Execute(new CityReport("911ConductHitFailedVictim", c));
	SlicObject *so = new SlicObject("911ConductHitFailedAttack");
	so->AddRecipient(m_owner);
	so->AddCity(c);
	g_slicEngine->Execute(so);

	return ORDER_RESULT_ILLEGAL;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::GetAdjacentCity
//
// Description: Get the city that occupies MapPoint point, if it exists and this army is next to it.
//
// Parameters : MapPoint
//
// Globals    : g_theWorld
//
// Returns    : Unit            : a city
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
Unit ArmyData::GetAdjacentCity(const MapPoint &point) const
{
	Cell *  cell = g_theWorld->GetCell(point);

	if (cell && ((point == m_pos) || point.IsNextTo(m_pos)))
	{
		return cell->GetCity();
	}
	else
	{
		return Unit();
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::GetCost
//
// Description: Returns the shield cost of this army.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : sint32
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 ArmyData::GetCost()
{
	sint32 cost = 0;
	for(sint32 i = 0; i < m_nElements; i++) {
		cost += m_array[i].GetDBRec()->GetShieldCost();
	}
	return cost;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanFranchise
//
// Description: Returns true if some member of this army can create a franchise.
//              In this case, fills in uindex with the units index in the army's
//              CellUnitList and chance with the unitDB chance of success.
//
// Parameters : double &chance  :
//              sint32 &uindex
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanFranchise(double &chance, sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		const UnitRecord::ChanceEffect *data;
		if(m_array[i].GetDBRec()->GetCreateFranchise(data) &&
		   !m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			uindex = i;
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::Franchise
//
// Description: Order this army to create a franchise at MapPoint point.
//
// Parameters : MapPoint
//
// Globals    : g_theWorld
//            : g_gevManager
//            : g_slicEngine
//            : g_rand
//            : g_player                : player array
//
// Returns    : ORDER_RESULT            : Attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::Franchise(const MapPoint &point)
{
	double chance;
	sint32 uindex;

	if(!CanFranchise(chance, uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit u      = m_array[uindex];
	Unit city   = GetAdjacentCity(point);

	if(city.m_id == 0) {
		return ORDER_RESULT_ILLEGAL;
	}

	if(city.GetOwner() == m_owner) {
		return ORDER_RESULT_ILLEGAL;
	}

	if(city.GetData()->GetCityData()->GetFranchiseOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

	//InformAI(UNIT_ORDER_FRANCHISE, point);//does nothing here but could be implemented

	AddSpecialActionUsed(u);

	city.ModifySpecialAttackChance(UNIT_ORDER_FRANCHISE, chance);
	city.SetWatchful();

	if(g_rand->Next(100) < sint32(chance * 100.0)) {

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MakeFranchise,
							   GEA_Unit, u,
							   GEA_City, city,
							   GEA_Player, m_owner,
							   GEA_End);

		DPRINTF(k_DBG_GAMESTATE, ("Franchise established\n"));
		ActionSuccessful(SPECATTACK_CREATEFRANCHISE, u, city);
		g_slicEngine->Execute(new CityReport("193BranchCompleteVictim", city));
		g_player[city.GetOwner()]->ContactMade(m_owner);
		return ORDER_RESULT_SUCCEEDED;
	}
	else
	{
		DPRINTF(k_DBG_GAMESTATE, ("Franchise attempt failed\n"));
		g_slicEngine->Execute(new CityReport("195BranchFailedVictim", city));
		return ORDER_RESULT_FAILED;
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanSue
//
// Description: Returns true if some unit in this army can sue.
//              In this case fills in index with that unit's index in this army's CellUnitList
//
// Parameters : sint32 &index
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanSue(sint32 &index) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanSue() &&
		   !m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN) &&
		   m_array[i].CanPerformSpecialAction()) {
			index = i;
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanSue
//
// Description: Returns true if some unit in this army can sue.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanSue() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanSue() &&
		   m_array[i].CanPerformSpecialAction() ) {
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanBeSued
//
// Description: Returns true if some unit in this army can be sued.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanBeSued() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanBeSued())
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::Sue
//
// Description: Order this army to sue a unit at MapPoint point.
//
// Parameters : MapPoint
//
// Globals    : g_theWorld
//            : g_gevManager
//            : g_slicEngine
//
// Returns    : ORDER_RESULT            : attempt success/failure indication
//
// Remark(s)  : Calls SueFranchise if the unit is a franchise
//
//----------------------------------------------------------------------------
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
	    /* SueFranchise now handled by SueFranchiseOrder directly
		if(cell->GetCity().m_id != 0 &&
		   cell->GetCity().GetOwner() == m_owner) {
			Assert(m_orders->GetHead() && m_orders->GetHead()->m_order == UNIT_ORDER_SUE);
			if(m_orders->GetHead() && m_orders->GetHead()->m_order == UNIT_ORDER_SUE) {
				m_orders->GetHead()->m_order = UNIT_ORDER_SUE_FRANCHISE;
			}
			return SueFranchise(point);
		}
	    */
		return ORDER_RESULT_ILLEGAL;
	}

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_Lawsuit,
						   GEA_Army, m_id,
						   GEA_Unit, m_array[uindex].m_id,
						   GEA_MapPoint, point,
						   GEA_End);

	return ORDER_RESULT_SUCCEEDED;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::SueFranchise
//
// Description: Order this army to sue a franchise at MapPoint point.
//
// Parameters : MapPoint
//
// Globals    : g_theWorld
//            : g_gevManager
//            : g_slicEngine
//
// Returns    : ORDER_RESULT            : attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::SueFranchise(const MapPoint &point)
{
	sint32 uindex;
	if(!CanSue(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit &	u		= m_array[uindex];
	Unit c = GetAdjacentCity(point);



	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() != m_owner) { // only remove franchises from your own cities

		return ORDER_RESULT_ILLEGAL;
	}

	if(c.GetFranchiseOwner() < 0) {

		return ORDER_RESULT_ILLEGAL;
	}

	if(c.GetFranchiseTurnsRemaining() == 0) {

		return ORDER_RESULT_ILLEGAL;
	}

	if(c.GetFranchiseTurnsRemaining() > 0 && c.GetFranchiseTurnsRemaining() <= g_theConstDB->Get(0)->GetTurnsToSueFranchise()) {

		return ORDER_RESULT_ILLEGAL;
	}

	AddSpecialActionUsed(u);

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_RemoveFranchise,
						   GEA_Army, m_id,
						   GEA_Unit, u,
						   GEA_City, c,
						   GEA_End);

	return ORDER_RESULT_SUCCEEDED;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::Expel
//
// Description: Order this army to expel whatever units are at MapPoint &point.
//
// Parameters : MapPoint
//
// Globals    : g_theWorld
//            : g_gevManager
//            : g_player                : player array
//
// Returns    : ORDER_RESULT            : attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::Expel(const MapPoint &point)
{
	Cell *cell = g_theWorld->GetCell(point);

	if (!cell || !point.IsNextTo(m_pos))
	{
		return ORDER_RESULT_ILLEGAL;
	}

	sint32 i, n = cell->GetNumUnits();
	if(n > 0)
	{
		// If at least one unit on a tile cannot be expelled, then none can be. -Maq
		for(i = 0; i < n; i++) {
			if(!cell->AccessUnit(i).CanBeExpelled())
				return ORDER_RESULT_ILLEGAL;
		}

		if(cell->UnitArmy()->GetOwner() == m_owner)
			return ORDER_RESULT_ILLEGAL;
	}
	else
	{
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

	//InformAI(UNIT_ORDER_EXPEL, point);//does nothing here but could be implemented

	bool 			foundCity	= false;
	MapPoint 		cpos;
	CellUnitList 	expelled;
	for(i = 0; i < n; i++) {
		if(cell->AccessUnit(i).CanBeExpelled() &&
		   cell->AccessUnit(i).GetVisibility() & (1 << m_owner)) {
			Unit u = cell->AccessUnit(i);

			foundCity = u.NearestFriendlyCityWithRoom(cpos, numToExpel,
													  u.GetArmy());

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

		for(i = 0; i < Num(); ++i)
		{
			AddSpecialActionUsed(m_array[i]);
		}

		return ORDER_RESULT_SUCCEEDED;
	}
	return ORDER_RESULT_FAILED;
}

bool ArmyData::CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amt,
								   sint32 &uindex) const
{
	const UnitRecord::CauseUnhappinessData *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCauseUnhappiness(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			timer = data->GetTimer();
			amt = data->GetAmount();
			uindex = i;
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CauseUnhappiness
//
// Description: Order the uindex-th member of this army to cause unhappiness
//              in the city that occupies MapPoint &point.
//
// Parameters : MapPoint point  : a point adjacent to this army's location (which should contain a city).
//            : sint32 uindex   : the index in this army's CellUnitList of the unit that executes the order.
//
// Globals    : g_slicEngine
//            : g_network
//
// Returns    : ORDER_RESULT    : attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanCauseUnhappiness(double &chance, sint32 &timer, sint32 &amt) const
{
	const UnitRecord::CauseUnhappinessData *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCauseUnhappiness(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			timer = data->GetTimer();
			amt = data->GetAmount();
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CauseUnhappiness
//
// Description: Order the uindex-th member of this army to cause unhappiness
//              in the city that occupies MapPoint &point.
//
// Parameters : MapPoint point  : a point adjacent to this army's location (which should contain a city).
//            : sint32 uindex   : the index in this army's CellUnitList of the unit that executes the order.
//
// Globals    : g_slicEngine
//            : g_network
//            : g_theUnitDB     : The unit database
//
// Returns    : ORDER_RESULT	: attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::CauseUnhappiness(const MapPoint &point, sint32 uindex)
{
	Assert(uindex >= 0);
	Assert(uindex < m_nElements);
	const UnitRecord::CauseUnhappinessData *cuData;
	if(uindex < 0 || uindex >= m_nElements)
		return ORDER_RESULT_ILLEGAL;

	Unit &	u	= m_array[uindex];
	if(!u.GetDBRec()->GetCauseUnhappiness(cuData) ||
	   !u.CanPerformSpecialAction()
	  )
	{
		return ORDER_RESULT_ILLEGAL;
	}

	double	chance	= cuData->GetChance();
	sint32	timer	= cuData->GetTimer();
	sint32	amount	= cuData->GetAmount();

	Unit c = GetAdjacentCity(point);

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner) {
		DPRINTF(k_DBG_GAMESTATE, ("Don't try to make your own people unhappy, bozo!\n"));
		return ORDER_RESULT_ILLEGAL;
	}

    //InformAI(UNIT_ORDER_CAUSE_UNHAPPINESS, point); //does nothing here but could be implemented

	AddSpecialActionUsed(u);

	c.ModifySpecialAttackChance(UNIT_ORDER_CAUSE_UNHAPPINESS, chance);
	c.SetWatchful();

	char unitName[256];
	strcpy(unitName, g_theStringDB->GetIdStr(g_theUnitDB->GetName(u.GetData()->GetType())));

	if(g_rand->Next(100) >= sint32(chance * 100.0)) {

	    if (strcmp(unitName, "UNIT_CYBER_NINJA") == 0) {
		g_slicEngine->Execute
		    (new CityReport("230TerrorhackFailedVictim", c));
		g_slicEngine->Execute
		    (new AggressorReport("229TerrorhackFailedAttacker", u, c));
		}

	    DPRINTF(k_DBG_GAMESTATE, ("Cause unhappiness failed\n"));
	    return ORDER_RESULT_FAILED;
	}

	DPRINTF(k_DBG_GAMESTATE, ("City 0x%lx will be %d less happy for %d turns\n",
							  uint32(c), amount, timer));

	if(g_network.IsHost()) {

		g_network.Enqueue(new NetInfo(NET_INFO_CODE_HAPPINESS_ATTACK,
									  (uint32)c,
									  timer,
									  amount));
	}

	ActionSuccessful(SPECATTACK_CAUSEUNHAPPINESS, u, c);
	c.AddHappyTimer(timer, double(-amount), HAPPY_REASON_HAPPINESS_ATTACK);
	c.AccessData()->GetCityData()->HappinessAttackedBy(m_owner) ;

	if (strcmp(unitName, "UNIT_CYBER_NINJA") == 0) {
	    g_slicEngine->Execute
		(new CityReport("228TerrorhackCompleteVictim", c));
	    g_slicEngine->Execute
		(new AggressorReport("227TerrorhackCompleteAttacker", u, c)) ;
	    }
	else if (strcmp(unitName, "UNIT_SUBNEURAL_ADS") == 0 ||
	    strcmp(unitName, "UNIT_CORPORATE_BRANCH") == 0
	    ) {
	    SlicObject * so = new CityReport("197AdvertiseCompleteVictim", c);
	    so->AddCivilisation(GetOwner());
	    g_slicEngine->Execute(so);
	    /* disabled because success of action is obvious any way (special effect and city icon) and therefore message spamming can be avoided
	    g_slicEngine->Execute
		(new AggressorReport("911AdvertiseCompleteAttacker", u, c)) ;
	    */
	    }

	return ORDER_RESULT_SUCCEEDED;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanPlantNuke
//
// Description: Returns true if the unit in this army whose CellUnitList index
//              is uindex can plant a nuke.
//              In this case, double &chance, double &escape_chance are filled
//              in from the unit's UnitRecord.
//
// Parameters : double &chance,
//              double &escape_chance,
//              sint32 &uindex
//
// Globals    : g_player         : player array
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanPlantNuke(double &chance, double &escape_chance,
							sint32 &uindex) const
{
	if ( !g_player[m_owner]->CanUseNukes())
		return false;

	const UnitRecord::PlantNukeData *data;

	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetPlantNuke(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			chance = data->GetChance();
			escape_chance = data->GetEscapeChance();
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanPlantNuke
//
// Description: Returns true if some unit in this army can plant a nuke.
//              In this case, double &chance, double &escape_chance are filled
//              in from that unit's UnitRecord.
//
// Parameters : double &chance,
//              double &escape_chance,
// Globals    : g_player                : player array
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanPlantNuke(double &chance, double &escape_chance) const
{
	if(!g_player[m_owner]->CanUseNukes())
		return false;

	const UnitRecord::PlantNukeData *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetPlantNuke(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			escape_chance = data->GetEscapeChance();
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::PlantNuke
//
// Description: Order this army to plant a nuke in the city that occupies MapPoint &point.
//
// Parameters : MapPoint point  : a point adjacent to this army's location (which should contain a city).
//
// Globals    : g_slicEngine
//            : g_gevManager
//
// Returns    : ORDER_RESULT    : attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::PlantNuke(const MapPoint &point)
{
	double chance, escape_chance;
	sint32 uindex;
	if(!CanPlantNuke(chance, escape_chance, uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit &	u	= m_array[uindex];
	Unit 	c	= GetAdjacentCity(point);
	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

    PLAYER_INDEX defense_owner = c.GetOwner();
	if(defense_owner == m_owner) {
		DPRINTF(k_DBG_GAMESTATE, ("I don't think you want to do that.\n"));
		return ORDER_RESULT_ILLEGAL;
	}

	Diplomat::GetDiplomat(c.GetOwner()).LogViolationEvent(m_owner, PROPOSAL_TREATY_CEASEFIRE);

    //InformAI(UNIT_ORDER_PLANT_NUKE, point); //does nothing here but could be implemented

	AddSpecialActionUsed(u);

	g_slicEngine->RunTerrorismTriggers(u, c);

	c.ModifySpecialAttackChance(UNIT_ORDER_PLANT_NUKE, chance);
	c.SetWatchful();

	if(g_rand->Next(100) < sint32(chance * 100.0)) {
		MapPoint pos;
		MapPoint cpos;
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_PlantNukeUnit,
							   GEA_Unit, u,
							   GEA_City, c,
							   GEA_End);

		u.GetPos(pos);
		if(g_rand->Next(100) < sint32(escape_chance)) {

			u.NearestFriendlyCity(cpos);
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_Teleport,
								   GEA_Army, m_id,
								   GEA_MapPoint, cpos,
								   GEA_End);

		}

		ActionSuccessful(SPECATTACK_PLANTNUKE, u, c);

        g_slicEngine->Execute(new CityReport("178NukeCompleteVictim", c));

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_NukeCity,
							   GEA_City,    c,
							   GEA_Player,  m_owner,
							   GEA_End);

		g_slicEngine->Execute
            (new AggressorReport("911NukeCompleteAggressor", u, c));

		return ORDER_RESULT_SUCCEEDED;
	}
    else
    {
        g_slicEngine->Execute(new CityReport("10gNukeFailed", c));
        g_slicEngine->Execute(new AggressorReport("11gNukeFailed", u, c));
    }
	return ORDER_RESULT_FAILED;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::SetPositionAndFixActors
//
// Description: Teleport this army from it's old position to MapPoint &p.
//
// Parameters : MapPoint p  :
//
// Globals    : g_theWorld
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::SetPositionAndFixActors(const MapPoint &p)
{
	MapPoint opos;
	GetPos(opos);

	for(sint32 i = 0; i < m_nElements; i++)
	{
		g_theWorld->RemoveUnitReference(opos, m_array[i]);
		m_array[i].SetPosition(p);
	}

	FixActors(opos, p);
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::FixActors
//
// Description: Finish off a teleport event.
//
// Parameters : MapPoint opos                   : the army's old position
//            : MapPoint npos                   : the army's new position
//
// Globals    : g_director                      : display manager
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::FixActors(MapPoint &opos, const MapPoint &npos)
{
	Unit top_src = GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());
	if(!top_src.IsValid())
		top_src = m_array[0];

	UnitActor **restOfStack = NULL;
	sint32 numRest = m_nElements - 1;

	if (numRest > 0) {
		restOfStack = new (UnitActor* [numRest]);
		GetActors(top_src, restOfStack);
	}

	MapPoint newPos(npos);
	g_director->AddMove(top_src, opos, newPos, numRest, restOfStack, top_src.GetMoveSoundID());

}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanSlaveRaid
//
// Description: Returns true if the unit in this army whose CellUnitList index
//              is uindex can do a slave raid.
//              In this case, double &success, double &death, sint32 &timer, and
//              sint32 &amount, are filled in from the unit's UnitRecord.
//
// Parameters : double &success,
//              double &death,
//              sint32 &timer
//              sint32 &amount
//              sint32 &uindex
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanSlaveRaid(double &success, double &death,
							sint32 &timer, sint32 &amount,
							sint32 &uindex) const
{
	const UnitRecord::SlaveRaidsData *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetSlaveRaids(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			success = data->GetChance();
			death = data->GetDeathChance();
			timer = data->GetTimer();
			amount = data->GetAmount();
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanSlaveRaid
//
// Description: Returns false if the first (top visible?) unit in this army can't slave raid.
//              Otherwise, double &success, double &death, sint32 &timer, and
//              sint32 &amount, are filled in from the unit's UnitRecord.
//
// Parameters : double &success,
//              double &death,
//              sint32 &timer
//              sint32 &amount
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : This is used in determining what buttons to show in the control panel window.
//
//----------------------------------------------------------------------------
bool ArmyData::CanSlaveRaid(double &success, double &death,
							sint32 &timer, sint32 &amount) const
{
	const UnitRecord::SlaveRaidsData *data = NULL;
	for(sint32 i = 0; i < m_nElements; i++) {
		if( !m_array[i].GetDBRec()->GetSlaveRaids(data)) {

			return false;
		}
	}

	success = data->GetChance();
	death = data->GetDeathChance();
	timer = data->GetTimer();
	amount = data->GetAmount();
	return true;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::SlaveRaid
//
// Description: Order this army to do a slave raid on the city or settler that
//              occupies MapPoint &point.
//
// Parameters : MapPoint point  : A point adjacent to this army's location
//                                (which should contain a city).
//
// Globals    : g_slicEngine
//            : g_rand
//            : g_gevManager
//            : g_player        : Player array [see Player::InitPlayer for
//                                initialized player data]
//
// Returns    : ORDER_RESULT    : Attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::SlaveRaid(const MapPoint &point)
{
	double success, death;
	sint32 timer, amount;
	sint32 uindex;
	bool target_is_city;
	Unit target_city; // city that is attaced by slaver
	Unit home_city; // city where slaves are put to work

	if (!IsSlaveRaidPossible(point, success, death, timer, amount, uindex,
	    target_is_city, target_city, home_city))
	{
		DPRINTF(k_DBG_GAMESTATE, ("!IsSlaveRaidPossible()\n"));
		return ORDER_RESULT_ILLEGAL;
	}

	MapPoint mypos;
	GetPos(mypos);
	if(!point.IsNextTo(mypos))
	{
		DPRINTF(k_DBG_GAMESTATE, ("!IsNextTo in SlaveRaid\n"));
		return ORDER_RESULT_ILLEGAL;
	}

	if (target_is_city)
	{
		// InformAI(UNIT_ORDER_SLAVE_RAID, point);//does nothing here but could be implemented
	}
	else
	{
		// InformAI(UNIT_ORDER_ENSLAVE_SETTLER, point); //does nothing here but could be implemented

		DPRINTF(k_DBG_GAMESTATE, ("Doing EnslaveSettler instead of SlaveRaid\n"));
		return EnslaveSettler(point, uindex, home_city); //redetermines home_city?
	}

	double slaveryReduction = target_city.IsProtectedFromSlavery();
	success *= slaveryReduction;

	target_city.SetWatchful();

	AddSpecialActionUsed(m_array[uindex]);

	g_slicEngine->RunEnslavementTriggers(m_array[uindex], target_city);

	if(g_rand->Next(100) < sint32(success * 100.0))
	{
		if(target_city.PopCount() <= 1)
		{
			return ORDER_RESULT_FAILED;
		}

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SlaveRaidCity,
							   GEA_Unit, m_array[uindex],
							   GEA_City, target_city.m_id,
							   GEA_End);

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MakePop,
							   GEA_City, home_city,
							   GEA_Player, target_city.GetOwner(),
							   GEA_End);

		if (target_city.IsValid())
		{
			target_city.AddHappyTimer(timer, -amount, HAPPY_REASON_SLAVES_TAKEN);
		}

		g_slicEngine->Execute
		    (new CityReport("137SlaveCompleteVictim", target_city));

		SlicObject * so = new SlicObject("137SlaveryCompleteAttacker");
		so->AddRecipient(GetOwner());
		so->AddCivilisation(GetOwner());
		so->AddCity(home_city);
		g_slicEngine->Execute(so);

		ActionSuccessful(SPECATTACK_SLAVERAID, m_array[uindex], target_city);
		return ORDER_RESULT_SUCCEEDED;
	}
	else
	{
		g_slicEngine->Execute
		    (new CityReport("138SlaveFailedVictim", target_city));

		if(g_rand->Next(100) < sint32(death * 100.0))
		{
			MapPoint pos;
			m_array[uindex].GetPos(pos);
			g_player[m_array[uindex].GetOwner()]->
				RegisterLostUnits(1, pos, DEATH_EFFECT_CALC);
			m_array[uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_SLAVERAID, -1);

			SlicObject * so = new SlicObject("138SlaveryKilledAttacker");
			so->AddRecipient(GetOwner());
			so->AddCivilisation(GetOwner());
			so->AddCity(target_city);
			g_slicEngine->Execute(so);
		}
		else
		{
			SlicObject * so = new SlicObject("138SlaveryFailedAttacker");
			so->AddRecipient(GetOwner());
			so->AddCivilisation(GetOwner());
			so->AddCity(target_city);
			g_slicEngine->Execute(so);

		}

		if(slaveryReduction < 1.0)
		{
			g_slicEngine->Execute
			    (new CityReport("140ProtectedFromSlaveryVictim", target_city));
			SlicObject * so = new SlicObject("141ProtectedFromSlaveryAttacker");
			so->AddRecipient(GetOwner());
			so->AddCity(target_city);
			g_slicEngine->Execute(so);

		}

		return ORDER_RESULT_FAILED;
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::IsSlaveRaidPossible
//
// Description: Returns true if the unit in this army whose CellUnitList index
//              is uindex can do a slave raid on the city or settler that occupies MapPoint &point.
//
//              In this case, double &success, double &death, sint32 &timer, and
//              sint32 &amount, are filled in from the unit's UnitRecord while
//              bool &target_is_city, Unit &target_city, and Unit &home_city are filled
//              in depending on whether the target is a city or settler.
//
// Parameters : const MapPoint &point   : the target location for the raid
//              double &success         : Database chance of success
//              double &death           : Database chance of death in attempt
//              sint32 &timer           : Database timer number
//              sint32 &amount          : Database amount of unhappiness to cause
//              sint32 &uindex          : CellUnitList index of raiding unit
//              bool &target_is_city    : true if target_is_city, false if target is settler
//              Unit &target_city       : the target city, if it exists
//              Unit &home_city         : destination city for slave
//
// Globals    : g_theWorld
//            : g_player                : player array [see Player::InitPlayer for initialized player data]
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::IsSlaveRaidPossible(const MapPoint &point,
                                   double &success,
                                   double &death,
                                   sint32 &timer,
                                   sint32 &amount,
                                   sint32 &uindex,
                                   bool &target_is_city,
                                   Unit &target_city,
                                   Unit &home_city)
{
	target_city.m_id = 0;
	  home_city.m_id = 0;

	if(!CanSlaveRaid(success, death, timer, amount, uindex))
	{
		return false;
	}

	target_city = GetAdjacentCity(point);
	Cell *cell = g_theWorld->GetCell(point);
	target_city = cell->GetCity();

	if(target_city.m_id != 0)
	{
		target_is_city = true;

		if(target_city.GetOwner() == m_array[0].GetOwner())
		{
			return false;
		}

		if(target_city.PopCount() < 2)
		{
			return false;
		}

		if(m_array[uindex].IsVeteran())
		{
			success += g_theConstDB->Get(0)->GetEliteSlaverBonus();
		}

		target_city.CD()->ModifySpecialAttackChance(UNIT_ORDER_SLAVE_RAID, success);

		if(target_city.IsWatchful())
		{
			death *= g_theConstDB->Get(0)->GetWatchfulCityDeathModifier();
		}
	}
	else
	{
		target_is_city = false;

		sint32 uindex;
		if(!CanEnslaveSettler(uindex))
		{
			return false;
		}

		if(cell->GetNumUnits() != 1)
		{
			return false;
		}

		if(!cell->AccessUnit(0).GetDBRec()->GetSettle())
		{
			return false;
		}

		if(cell->AccessUnit(0).GetOwner() == m_array[0].GetOwner())
		{
			return false;
		}
	}

	MapPoint slpos;
	GetPos(slpos);

	return g_player[m_array[0].GetOwner()]->GetSlaveCity(slpos, home_city);
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanEnslaveSettler
//
// Description: Returns true if some unit in this army whose can enslave settlers on this turn.
//              In this case, fills in uindex with this army's CellUnitList index of that unit.
//
// Parameters : sint32 &uindex          : CellUnitList index of raiding unit
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanEnslaveSettler(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasSettlerSlaveRaids() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::EnslaveSettler
//
// Description: Order the uindex-th member of this army to enslave a settler that occupies MapPoint &point
//              and send him to Unit home_city.
//
// Parameters : MapPoint point  : a point adjacent to this army's location (which should contain exactly one settler).
//              Unit home_city  : the destination city for the captured slave.
//
// Globals    : g_player        : player array
//            : g_theWorld
//            : g_gevManager
//            : g_slicEngine
//
// Returns    : ORDER_RESULT    : attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::EnslaveSettler(const MapPoint &point, const sint32 uindex,
                              Unit home_city)
{
	sint32 r = g_player[m_owner]->GetSlaveCity(m_pos, home_city);
	if(!r)
		return ORDER_RESULT_ILLEGAL;

	Cell *cell = g_theWorld->GetCell(point);

	sint32 n = cell->GetNumUnits();
	Assert(n > 0);
	if(n != 1)
	{
		return ORDER_RESULT_ILLEGAL;
	}

	if(!cell->AccessUnit(0).GetDBRec()->GetSettle())
	{
		return ORDER_RESULT_ILLEGAL;
	}

	ActionSuccessful(SPECATTACK_ENSLAVESETTLER, m_array[uindex], cell->AccessUnit(0));

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_EnslaveSettler, // also sends messages
						   GEA_Army, m_id,
						   GEA_Unit, m_array[uindex],
						   GEA_Unit, cell->AccessUnit(0).m_id,
						   GEA_End);

	AddSpecialActionUsed(m_array[uindex]);

	return ORDER_RESULT_SUCCEEDED;
}

bool ArmyData::CanBeEnslaved() const
{
	return m_nElements == 1 && m_array[0].GetDBRec()->GetSettle();
}

bool ArmyData::CanUndergroundRailway(double &success, double &death,
						   sint32 &uindex) const
{
	const UnitRecord::SuccessDeathEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetUndergroundRailway(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			success = data->GetChance();
			death = data->GetDeathChance();

			return true;
		}
	}
	return false;
}

bool ArmyData::CanUndergroundRailway(double &success, double &death) const
{
	const UnitRecord::SuccessDeathEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetUndergroundRailway(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			success = data->GetChance();
			death = data->GetDeathChance();
			return true;
		}
	}
	return false;
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

	if(c.GetOwner() == m_array[0].GetOwner())
	{
		DPRINTF(k_DBG_GAMESTATE, ("You can't free your own slaves with an abolitionist.  In fact, you shouldn't even be able to have both slaves and an abolitionist.  Stop cheating!\n"));
		return ORDER_RESULT_ILLEGAL;
	}

	if(c.CountSlaves() <= 0)
	{
		DPRINTF(k_DBG_GAMESTATE, ("Dammit, there aren't any slaves there."));

		so = new SlicObject("167FreeslaveNoSlavesToFree");
		so->AddRecipient(GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);

		return ORDER_RESULT_ILLEGAL;
	}

	//InformAI(UNIT_ORDER_UNDERGROUND_RAILWAY, point); //does nothing here but could be implemented

	if(m_array[uindex].IsVeteran()) {
		success += g_theConstDB->Get(0)->GetEliteAbolitionistBonus();
	}

	if(c.IsWatchful()) {
		success *= g_theConstDB->Get(0)->GetWatchfulCitySuccessModifier();
		death *= g_theConstDB->Get(0)->GetWatchfulCityDeathModifier();
	}

	c.SetWatchful();

	if(g_rand->Next(100) < sint32(success * 100.0))
	{
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_UndergroundRailwayUnit,
							   GEA_Unit, m_array[uindex].m_id,
							   GEA_City, c,
							   GEA_End);
		return ORDER_RESULT_SUCCEEDED;
	}
	else
	{
		AddSpecialActionUsed(m_array[uindex]);

		if(g_rand->Next(100) < sint32(death * 100.0))
		{
			m_array[uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_UNDERGROUND_RR_RAID, -1);
		}

		g_slicEngine->Execute(new CityReport("164FreeslaveFailedVictim", c));

		so = new SlicObject("166FreeslaveFailedAgressor");
		so->AddRecipient(GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);

		return ORDER_RESULT_FAILED;
	}
}

bool ArmyData::CanInciteUprising(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasSlaveUprising() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return true;
		}
	}
	return false;
}

ORDER_RESULT ArmyData::InciteUprising(const MapPoint &point)
{
	SlicObject *so;
	sint32 uindex;
	if(!CanInciteUprising(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);
	if(!c.IsValid())
		return ORDER_RESULT_ILLEGAL;

	PLAYER_INDEX const  cityOwner = c.GetOwner();

	if(cityOwner == m_owner)
	{
		return ORDER_RESULT_ILLEGAL;
	}

	if(c.CountSlaves() <= 0)
	{
		so = new SlicObject("167FreeslaveNoSlavesToFree");
		so->AddRecipient(GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);

		return ORDER_RESULT_ILLEGAL;
	}

	Player *    p               = g_player[cityOwner];
	double      distanceCost    = 100.0;
	MapPoint    start;

	if(p->GetCapitolPos(start))
	{
		MapPoint    dest;
		c.GetPos(dest);
		distanceCost += 100.0 * start.NormalizedDistance(dest);
	}
	else
	{
		distanceCost += p->GetMaxEmpireDistance();
	}

	double const    baseCost        =
	    (g_player[c.GetOwner()]->m_gold->GetLevel() + 5000) *
	    static_cast<double>(c.PopCount()) *
	    (1 / distanceCost) *
	    g_theConstDB->Get(0)->GetInciteUprisingGoldCoefficient();
	double const    capitolPenalty  = c.IsCapitol() ? g_theConstDB->Get(0)->GetInciteUprisingCapitolPenalty() : 0.0;

	sint32 const    cost            = static_cast<sint32>(baseCost + capitolPenalty);

	DPRINTF(k_DBG_GAMESTATE, ("Cost to incite uprising: %ld\n", cost));

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
	so->AddRecipient(cityOwner) ;
	so->AddCity(c) ;
	g_slicEngine->Execute(so) ;

	ActionSuccessful(SPECATTACK_SLAVEUPRISING, m_array[uindex], c);

	return ORDER_RESULT_SUCCEEDED;
}

bool ArmyData::CanEstablishEmbassy(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetEstablishEmbassy()) {
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::CanEstablishEmbassy() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetEstablishEmbassy() &&
		   m_array[i].CanPerformSpecialAction()) {
			return true;
		}
	}
	return false;
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

	if(g_player[m_owner]->HasWarWith(c.GetOwner()))
		return ORDER_RESULT_ILLEGAL;

	if(wonderutil_GetCloseEmbassies(g_player[c.GetOwner()]->m_builtWonders)) {
		SlicObject *so = new SlicObject("145NoEmbassiesWonder");
		so->AddCivilisation(c.GetOwner());
		so->AddCity(c);
		sint32 w;
		for(w = 0; w < g_theWonderDB->NumRecords(); w++)
		{
			if( g_player[c.GetOwner()]->HasWonder(w)
			&&  wonderutil_Get(w, c.GetOwner())->GetCloseEmbassies()
			&& !wonderutil_IsObsolete(w)
			){
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

bool ArmyData::CanThrowParty(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++)
	{
		if(m_array[i].GetDBRec()->GetThrowParty())
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::CanThrowParty() const
{
	for(sint32 i = 0; i < m_nElements; i++)
	{
		if(m_array[i].GetDBRec()->GetThrowParty()
		&& m_array[i].CanPerformSpecialAction()
		){
			return true;
		}
	}
	return false;
}

ORDER_RESULT ArmyData::ThrowParty(const MapPoint &point)
{
	sint32 uindex;
	if(!CanThrowParty(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);
	Unit u = m_array[uindex];

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

	if(g_player[m_owner]->HasWarWith(c.GetOwner()))
		return ORDER_RESULT_ILLEGAL;

	if(!Diplomat::GetDiplomat(c.GetOwner()).ReadyToParty())
		return ORDER_RESULT_ILLEGAL;

	AddSpecialActionUsed(m_array[uindex]);

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
	                       GEV_ThrowPartyUnit,
	                       GEA_Unit, m_array[uindex].m_id,
	                       GEA_City, c,
	                       GEA_End
	                      );

	return ORDER_RESULT_SUCCEEDED;
}

bool ArmyData::CanBioInfect(double &chance, sint32 &uindex) const
{
	const UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetBioTerror(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::CanBioInfect(double &chance) const
{
	const UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetBioTerror(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			return true;
		}
	}
	return false;
}

ORDER_RESULT ArmyData::BioInfect(const MapPoint &point)
{
	double chance;
	sint32 uindex;

	if(!CanBioInfect(chance, uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_array[0].GetOwner())
	{
		return ORDER_RESULT_ILLEGAL;
	}

	if(c.IsBioInfected())
	{
		return ORDER_RESULT_ILLEGAL;
	}

	//InformAI(UNIT_ORDER_BIO_INFECT, point); //does nothing here but could be implemented

	if(m_array[uindex].IsVeteran())
	{
		chance += g_theConstDB->Get(0)->GetEliteTerroristBonus();
	}

	SlicObject * so = NULL;

	if(c.IsBioImmune())
	{
		DPRINTF(k_DBG_GAMESTATE, ("Bio infection failed because city immune\n"));
		so = new CityReport("10iImmuneToBioInfect", c);
		so->AddCivilisation(c.GetOwner());
		g_slicEngine->Execute(so);

		so = new SlicObject("11iImmuneToBioInfect");
		so->AddRecipient(GetOwner());
		so->AddCivilisation(c.GetOwner());
		so->AddCity(c) ;
		g_slicEngine->Execute(so);

		return ORDER_RESULT_FAILED;
	}

	AddSpecialActionUsed(m_array[uindex]);

	c.ModifySpecialAttackChance(UNIT_ORDER_BIO_INFECT, chance);
	c.SetWatchful();

	if(g_rand->Next(100) < sint32(chance * 100.0))
	{
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_BioInfectCityUnit,
							   GEA_Unit, m_array[uindex].m_id,
							   GEA_City, c.m_id,
							   GEA_End);
		return ORDER_RESULT_SUCCEEDED;
	}
	else
	{
		DPRINTF(k_DBG_GAMESTATE, ("Bio infection failed because I said so.\n"));
		so = new CityReport("10iBioInfectFailed", c);
		so->AddCivilisation(GetOwner());
		g_slicEngine->Execute(so);

		so = new SlicObject("11iBioInfectFailed");
		so->AddRecipient(GetOwner());
		so->AddCivilisation(c.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);

		if(g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetBioInfectionTerroristDeathChance() * 100.0))
			m_array[uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);

		return ORDER_RESULT_FAILED;
	}
}

bool ArmyData::CanPlague(double &chance, sint32 &uindex) const
{
	const UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetPlague(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::CanPlague(double &chance) const
{
	const UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetPlague(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			return true;
		}
	}
	return false;
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

    //InformAI(UNIT_ORDER_NANO_INFECT, point); //does nothing here but could be implemented

	if(m_array[uindex].IsVeteran()) {
		chance += g_theConstDB->Get(0)->GetEliteTerroristBonus();
	}

	if(c.IsBioImmune()) {
		DPRINTF(k_DBG_GAMESTATE, ("Plague failed because city immune\n"));
		so = new CityReport("10jImmuneToPlague", c);
		so->AddCivilisation(GetOwner());
		g_slicEngine->Execute(so);

		so = new SlicObject("11jImmuneToPlague");
		so->AddRecipient(GetOwner());
		so->AddCivilisation(c.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);

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
		so = new CityReport("10jPlagueFailed", c);
		so->AddCivilisation(GetOwner());
		g_slicEngine->Execute(so);

		so = new SlicObject("11jPlagueFailed");
		so->AddRecipient(GetOwner());
		so->AddCivilisation(c.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);

		if(g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetBioInfectionTerroristDeathChance() * 100.0))
			m_array[uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);

		return ORDER_RESULT_FAILED;
	}
}

bool ArmyData::CanNanoInfect(double &chance, sint32 &uindex) const
{
	const UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetNanoTerror(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::CanNanoInfect(double &chance) const
{
	const UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetNanoTerror(data) &&
		   m_array[i].CanPerformSpecialAction()) {
			chance = data->GetChance();
			return true;
		}
	}
	return false;
}

ORDER_RESULT ArmyData::NanoInfect(const MapPoint &point)
{
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
		chance += g_theConstDB->Get(0)->GetEliteTerroristBonus();
	}

	AddSpecialActionUsed(m_array[uindex]);

	SlicObject * so = NULL;

	if(c.IsNanoImmune()) {
		so = new CityReport("10hImmuneToNanoTerror", c);
		so->AddCivilisation(GetOwner());
		g_slicEngine->Execute(so);

		so = new SlicObject("11hImmuneToNanoTerror");
		so->AddRecipient(GetOwner());
		so->AddCivilisation(c.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);

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

		so = new CityReport("911CrisisCityIsNanoInfected", c);
		so->AddCivilisation(c->GetOwner()); // ToDo: Move this into CityReport if possible
		g_slicEngine->Execute(so);

		return ORDER_RESULT_SUCCEEDED;
	} else {
		DPRINTF(k_DBG_GAMESTATE, ("Nano infection failed: The world is a safer place.\n"));

		so = new CityReport("10hNanoTerrorFailed", c);
		so->AddCivilisation(GetOwner());
		g_slicEngine->Execute(so);

		so = new SlicObject("11hNanoTerrorFailed");
		so->AddRecipient(GetOwner());
		so->AddCivilisation(c.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);

		if(g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetNanoInfectionTerroristDeathChance() * 100.0))
			m_array[uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_SPYING, -1);

		return ORDER_RESULT_FAILED;
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanConvertCity
//
// Description: Returns true if some member of this army can convert a city.
//              In this case, fills in double &best_chance and double &best_death_chance
//              with the UnitRecord values of the unit that has the best chance of doing it.
//              best_uindex is the index into this army's m_array CellUnitList
//
// Parameters : double &best_chance
//              double &best_death_chance
//              sint32 &best_uindex
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanConvertCity(double &best_chance, double &best_death_chance,
							  sint32 &best_uindex) const
{
	double chance = 0.0;
	double death_chance = 0.0;
	best_chance = -1.0;

	UnitRecord::SuccessDeathEffect const * data;
    for (sint32 i = m_nElements - 1; i>= 0; i--) {
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

bool ArmyData::CanConvertCity(double &best_chance, double &best_death_chance) const
{
	double chance = 0.0;
	double death_chance = 0.0;
	best_chance = -1.0;

	UnitRecord::SuccessDeathEffect const * data;
    for (sint32 i = m_nElements - 1; i>= 0; i--) {

        if ( m_array[i].GetDBRec()->GetConvertCities(data) &&
			m_array[i].CanPerformSpecialAction() ) {
			chance = data->GetChance();
			death_chance = data->GetDeathChance();

			if(chance > best_chance) {
				best_chance = chance;
				best_death_chance = death_chance;
			}
			if ( best_chance <= 0.0 ) {
				return false;
			}
		}
		else {
			return false;
		}
	}
	return true;
}

bool ArmyData::AbleToConvertTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++)
	{
		if(m_array[i].CanConvert(point))
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanConvertCity
//
// Description: Returns true if this army is next to MapPoint point, which contains a city,
//              and at least one of it's members can convert cities.
//
// Parameters : MapPoint &point
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanConvertCity(const MapPoint &point) const
{
	Unit city = GetAdjacentCity(point);
	if(city.m_id == 0)
//EMOD add unit check like in bombard
//units then can be converted
		return false;

	for(sint32 i = 0; i < m_nElements; i++) {
		if (m_array[i].CanConvertCity(city))
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::ConvertCity
//
// Description: Order this army to convert a city that occupies MapPoint &point.
//
// Parameters : MapPoint point  : a point adjacent to this army's location.
//
// Globals    : g_rand
//            : g_gevManager
//            : g_slicEngine
//            : g_theWonderDB
//
// Returns    : ORDER_RESULT    : attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::ConvertCity(const MapPoint &point)
{
	double chance = 0.0, death_chance = 0.0;
	double best_chance = 0.0;
	double best_death_chance = 0.0;
	sint32 best_uindex = -1;

	Unit city = GetAdjacentCity(point);

	if(city.m_id  == 0)
		return ORDER_RESULT_ILLEGAL;

	if(city.GetCityData()->IsProtectedFromConversion()) {
		SlicObject *so = new SlicObject("361IAProtectedFromConversionByWonder");
		so->AddCity(city);
		sint32 i;
		for(i = 0; i < g_theWonderDB->NumRecords(); i++) {
			if(!g_player[city.GetOwner()]->HasWonder(i))
				continue;

			if(wonderutil_Get(i, m_owner)->GetPreventConversion()) {
				so->AddWonder(i);
				break;
			}
		}
		Assert(i < g_theWonderDB->NumRecords());
		if(i >= g_theWonderDB->NumRecords()) {
			delete so;
		} else {
			so->AddCivilisation(city.GetOwner());
			so->AddRecipient(m_owner);
			g_slicEngine->Execute(so);
		}
		return ORDER_RESULT_ILLEGAL;
	}

	const UnitRecord::SuccessDeathEffect *data;

	for (sint32 i = m_nElements - 1; i>= 0; i--) {
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

	Unit &  u   = m_array[best_uindex];
	chance = chance - (double(chance) * (city.TheologicalModifier() / 100.0));

	AddSpecialActionUsed(u);

	//InformAI(UNIT_ORDER_CONVERT, point);//does nothing here but could be implemented

	city.ModifySpecialAttackChance(UNIT_ORDER_CONVERT, best_chance);
	city.SetWatchful();

	if(g_rand->Next(100) < sint32(chance * 100.0)) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ConvertCityUnit,
							   GEA_Unit, u,
							   GEA_City, city,
							   GEA_End);

		// EMOD - if city can convert building & establishbuilding it builds
		// that building there. Used to spread religions
		for (sint32 i = m_nElements - 1; i>= 0; i--)
		{
			const UnitRecord *urec = m_array[i].GetDBRec();
			if(m_array[i].GetDBRec()->GetNumEstablishBuilding())
			{
				for(sint32 b = 0; b < urec->GetNumEstablishBuilding(); b++)
				{
					sint32 bi = urec->GetEstablishBuildingIndex(b);
					Assert(bi >= 0);
					Assert(bi < g_theBuildingDB->NumRecords());

					if(bi >= 0 && bi < g_theBuildingDB->NumRecords())
					{
						g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CreateBuilding,
						                       GEA_City, city,
						                       GEA_Int, bi,
						                       GEA_End);
					}
				}
			}
		}
		// end EMOD

		return ORDER_RESULT_SUCCEEDED;
	}
	else {
		DPRINTF(k_DBG_GAMESTATE, ("Conversion failed\n"));

		g_slicEngine->Execute
		    (new VictimReport("152ConvertFailedVictim", u, city));
		g_slicEngine->Execute
		    (new AggressorReport("153ConvertFailedAttacker", u, city));

		if(g_rand->Next(100) < sint32(best_death_chance * 100.0)) {
			DPRINTF(k_DBG_GAMESTATE, ("And cleric died\n"));
			u.Kill(CAUSE_REMOVE_ARMY_DIED_IN_CONVERT_RAID, -1);
		}

		return ORDER_RESULT_FAILED;
	}
}

bool ArmyData::CanReformCity(sint32 &uindex, const MapPoint &point) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		const UnitRecord *rec = m_array[i].GetDBRec();
		if(rec->HasCanReform() &&
		   m_array[i].CanPerformSpecialAction()) {
			if(!rec->GetMovementTypeLand() && !rec->GetMovementTypeMountain())
				continue;
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::CanReformCity() const
{
	MapPoint point;
	GetPos( point );

	for(sint32 i = 0; i < m_nElements; i++) {
		const UnitRecord *rec = m_array[i].GetDBRec();
		if(!rec->HasCanReform() ||
		   !m_array[i].CanPerformSpecialAction()) {
			return false;
		}
	}
	return true;
}

ORDER_RESULT ArmyData::ReformCity(const MapPoint &point)
{

	sint32 uindex;

	if(!CanReformCity(uindex, point))
		return ORDER_RESULT_ILLEGAL;

	Unit c = GetAdjacentCity(point);

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() != m_array[uindex].GetOwner()) // only reform (remove conversion) your own cities 
		return ORDER_RESULT_ILLEGAL;

	if(c.IsConvertedTo() < 0)
		return ORDER_RESULT_ILLEGAL;

	AddSpecialActionUsed(m_array[uindex]);

	if(g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetReformationChance() * 100.0))
	{
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ReformCityUnit,
							   GEA_Unit, m_array[uindex].m_id,
							   GEA_City, c.m_id,
							   GEA_End);
		
		SlicObject *so = new SlicObject("135ReformCityVictim");
		so->AddRecipient(c.IsConvertedTo());
		so->AddCity(c);
		so->AddGold(c.GetConvertedGold());
		g_slicEngine->Execute(so);


		// EMOD added for reforming units to destroy
		// religious buildings, if they can build there own
		for (sint32 i = m_nElements - 1; i >= 0; i--)
		{
			const UnitRecord *urec = m_array[i].GetDBRec();
			if(m_array[i].GetDBRec()->GetNumEstablishBuilding())
			{
				for(sint32 b = 0; b < urec->GetNumEstablishBuilding(); b++)
				{
					const BuildingRecord *brec = buildingutil_Get(urec->GetEstablishBuildingIndex(b), m_owner);
					if(brec->GetNumConflictsWithBuilding())
					{
						for(sint32 conflictb = 0; conflictb < brec->GetNumConflictsWithBuilding(); conflictb++)
						{
							if(c.CD()->HasBuilding(brec->GetConflictsWithBuildingIndex(conflictb))) {
								c.CD()->DestroyImprovement(brec->GetConflictsWithBuildingIndex(conflictb));
							}
						}
					}
				}
			}
		}
		return ORDER_RESULT_SUCCEEDED;
	} else {
		DPRINTF(k_DBG_GAMESTATE, ("Reformation failed\n"));

		if(g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetReformationDeathChance() * 100.0)) {
			DPRINTF(k_DBG_GAMESTATE, ("And inquisitor died.\n"));

			m_array[uindex].Kill(CAUSE_REMOVE_ARMY_DIED_IN_REFORMATION, -1);
		}

		return ORDER_RESULT_FAILED;
	}
}

bool ArmyData::CanSellIndulgences(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasIndulgenceSales() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::CanSellIndulgences() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasIndulgenceSales() &&
		   m_array[i].CanPerformSpecialAction()) {
			return true;
		}
	}
	return false;
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
							   GEA_Int, g_theConstDB->Get(0)->GetUnconvertedIndulgenceGold(),
							   GEA_End);

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddGold,
							   GEA_Player, m_owner,
							   GEA_Int, g_theConstDB->Get(0)->GetUnconvertedIndulgenceGold(),
							   GEA_End);

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddHappyTimer,
							   GEA_City, c.m_id,
							   GEA_Int, 1,
							   GEA_Int, g_theConstDB->Get(0)->GetUnconvertedIndulgenceHappiness(),
							   GEA_Int, HAPPY_REASON_INDULGENCES,
							   GEA_End);

	} else if(c.IsConvertedTo() == m_array[uindex].GetOwner()) {

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SubGold,
							   GEA_Player, c.GetOwner(),
							   GEA_Int, g_theConstDB->Get(0)->GetConvertedIndulgenceGold(),
							   GEA_End);
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddGold,
							   GEA_Player, m_owner,
							   GEA_Int, g_theConstDB->Get(0)->GetConvertedIndulgenceGold(),
							   GEA_End);
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddHappyTimer,
							   GEA_City, c.m_id,
							   GEA_Int, 1,
							   GEA_Int, g_theConstDB->Get(0)->GetConvertedIndulgenceHappiness(),
							   GEA_Int, HAPPY_REASON_INDULGENCES,
							   GEA_End);

	} else {

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SubGold,
							   GEA_Player, c.GetOwner(),
							   GEA_Int, g_theConstDB->Get(0)->GetOtherFaithIndulgenceGold(),
							   GEA_End);
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddGold,
							   GEA_Player, m_owner,
							   GEA_Int, g_theConstDB->Get(0)->GetOtherFaithIndulgenceGold(),
							   GEA_End);
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddHappyTimer,
							   GEA_City, c.m_id,
							   GEA_Int, 1,
							   GEA_Int, g_theConstDB->Get(0)->GetOtherFaithIndulgenceHappiness(),
							   GEA_Int, HAPPY_REASON_INDULGENCES,
							   GEA_End);
	}

	// Teleevangelist unit may not have index 66 in unit database
	if(u.GetDBRec()->GetIsTelevangelist()) {
		g_slicEngine->Execute(new CityReport("911FaithHealVictim", c));

		/* disabled because action never fails and gold increase already tells the success and therefore message spamming can be avoided
		SlicObject * so  = new SlicObject("911FaithHealAttacker");
		so->AddRecipient(u.GetOwner());
		so->AddCivilisation(c.GetOwner()); // Televangelist message differs that of clerics and in addition needs the civ
		so->AddCity(c);
		g_slicEngine->Execute(so);
		*/
	} else {
		g_slicEngine->Execute(new CityReport("911IndulgenceCompleteVictim", c));

		/* disabled because action never fails and gold increase already tells the success and therefore message spamming can be avoided
		SlicObject * so = new SlicObject("911IndulgenceCompleteAttacker");
		so->AddRecipient(u.GetOwner());
		so->AddCity(c);
		g_slicEngine->Execute(so);
		*/
	}

	ActionSuccessful(SPECATTACK_SELLINDULGENCE, m_array[uindex], c);
	return ORDER_RESULT_SUCCEEDED;
}

bool ArmyData::CanSoothsay(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasCanSoothsay() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return true;
		}
	}

	return false;
}

bool ArmyData::CanSoothsay() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasCanSoothsay() &&
		   m_array[i].CanPerformSpecialAction()) {
			return true;
		}
	}

	return false;
}

ORDER_RESULT ArmyData::Soothsay(const MapPoint &point)
{
	sint32 uindex;

	if(!CanSoothsay(uindex))
		return ORDER_RESULT_ILLEGAL;

	ORDER_RESULT res = CauseUnhappiness(point, uindex);
	if (res == ORDER_RESULT_SUCCEEDED || res == ORDER_RESULT_SUCCEEDED_INCOMPLETE)
    {
	    Unit u = m_array[uindex];
		Unit c = GetAdjacentCity(point);
		if (c.IsValid())
        {
			g_slicEngine->Execute
                (new VictimReport("911SoothsayCompleteVictim", u, c));
			g_slicEngine->Execute
                (new AggressorReport("911SoothsayCompleteAttacker", u, c));
		}
	}
	return res;
}

bool ArmyData::CanAdvertise(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetAdvertise() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::CanAdvertise() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetAdvertise())
			return true;
	}
	return false;
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

	// establish that building there. Used to spread corporations
	for (sint32 i = m_nElements - 1; i>= 0; i--) {
		const UnitRecord *urec = m_array[i].GetDBRec();
		if(m_array[i].GetDBRec()->GetNumEstablishBuilding()) {
			for(sint32 b = 0; b < urec->GetNumEstablishBuilding(); b++) {
				sint32 bi = urec->GetEstablishBuildingIndex(b);
				Assert(bi >= 0);
				Assert(bi < g_theBuildingDB->NumRecords());
				if(bi >= 0 && bi < g_theBuildingDB->NumRecords()) {
					g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CreateBuilding,
			                       GEA_City, c,
			                       GEA_Int, bi,
			                       GEA_End);
				}
			}
		}
	}
// end EMOD

	return CauseUnhappiness(point, uindex);
}

bool ArmyData::CanLaunch(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasSpaceLaunch() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::CanLaunch() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasSpaceLaunch())
			return true;
	}
	return false;
}

ORDER_RESULT ArmyData::Launch(Order *order)
{
	sint32 uindex;

	if(!CanLaunch(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit destCity = g_theWorld->GetCity(order->m_point);
	if(!destCity.IsValid() || destCity.GetOwner() != m_owner)
		return ORDER_RESULT_ILLEGAL;

	if(m_nElements > 1)
	{
		m_array[uindex]->CreateOwnArmy();
	}

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_LaunchUnit,
						   GEA_Unit, m_array[uindex],
						   GEA_MapPoint, order->m_point,
						   GEA_End);
	return ORDER_RESULT_SUCCEEDED;
}

bool ArmyData::CanTarget(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasNuclearAttack() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return true;
		}
	}

	return false;
}

bool ArmyData::CanTarget() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasNuclearAttack())
			return true;
	}
	return false;
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
		if(m_array[i].GetDBRec()->HasNuclearAttack()) {
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
		if(m_array[i].GetDBRec()->HasNuclearAttack()) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ClearTarget,
								   GEA_Unit, m_array[i],
								   GEA_End);
		}
	}
	return ORDER_RESULT_SUCCEEDED;
}

void ArmyData::SetReentry(sint32 turns, MapPoint &pos)
{
	Assert(g_theWorld->HasCity(pos));
	m_reentryTurn = NewTurnCount::GetCurrentRound() + turns;
	m_reentryPos = pos;
	m_flags |= k_CULF_IN_SPACE;
}

void ArmyData::Reenter()
{
	Assert(m_flags & k_CULF_IN_SPACE);
	if(!(m_flags & k_CULF_IN_SPACE))
	{
		return;
	}

	Unit city = g_theWorld->GetCity(m_reentryPos);
	if(!city.IsValid() || city.GetOwner() != m_owner) // target city is gone
	{
		for (int i = 0; i < m_nElements; i++)
		{
			// ToDo: add message (with eye) that the space plane and its cargo was lost because the target city is gone
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
								   GEA_Unit, m_array[i].m_id,
								   GEA_Int, 0,
								   GEA_Player, -1,
								   GEA_End);
		}
	}
	else if(g_theWorld->GetCell(m_reentryPos)->GetNumUnits() > (k_MAX_ARMY_SIZE - m_nElements)) // target city cannot hold any more units
	{
		// ToDo: add message (with eye) that the space plan cannot reenter as long as the target city cannot hold more units
		return;
	}
	else
	{
		m_flags &= ~(k_CULF_IN_SPACE);
		MapPoint oldPos = m_pos;

		for (int i = 0; i < m_nElements; i++)
		{
			m_array[i]->ClearFlag(k_UDF_HAS_LEFT_MAP);
			m_array[i]->ClearFlag(k_UDF_IN_SPACE);

			m_array[i]->SetPosAndNothingElse(m_reentryPos);
			g_theWorld->InsertUnit(m_reentryPos, m_array[i]);
			m_array[i]->AddUnitVision();
		}

		ResetPos();
		FixActors(oldPos, m_pos);
		for (int i = 0; i < m_nElements; i++) {
			g_director->AddShow(m_array[i]);
		}
	}
}

/////////////////////////////////////////////////////////////////////
//
// None of these AbleTo... functions appear to be currently used
//
//////////////////////////////////////////////////////////////////////
bool ArmyData::AbleToPlantNukeTarget(const MapPoint &point, sint32 &uindex)
{
	for (sint32 i = 0; i < m_nElements ; i++)
	{
		if (m_array[i].CanPlantNuke(point))
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::AbleToMakeParkTarget(const MapPoint &point, sint32 &uindex)
{
	for (sint32 i = 0; i < m_nElements ; i++)
	{
		if (m_array[i].CanMakePark(point))
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::AbleToUndergroundRailwayTarget(const MapPoint &point, sint32 &uindex)
{
	for (sint32 i = 0; i < m_nElements ; i++)
	{
		if (m_array[i].CanUndergroundRailway(point))
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::AbleToEstablishEmbassyTarget(const MapPoint &point, sint32 &uindex)
{
	for (sint32 i = 0; i < m_nElements ; i++)
	{
		if (m_array[i].CanEstablishEmbassy(point))
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::AbleToCreateFranchiseTarget(const MapPoint &point, sint32 &uindex)
{
	for (sint32 i = 0; i < m_nElements ; i++)
	{
		if (m_array[i].CanCreateFranchise(point))
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::AbleToAssasinateRulerTarget(const MapPoint &point, sint32 &uindex)
{
	for (sint32 i = 0; i < m_nElements ; i++)
	{
		if (m_array[i].CanAssasinateRuler(point))
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::AbleToStealTechnologyTarget(const MapPoint &point, sint32 &uindex)
{
	for (sint32 i = 0; i < m_nElements ; i++)
	{
		if (m_array[i].CanStealTechnology(point))
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::AbleToInjoinTarget(const MapPoint &point, sint32 &uindex)
{
	for(sint32 i = 0; i < m_nElements ; i++)
	{
		if(m_array[i].CanInjoin(point))
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::AbleToInciteRevolutionTarget(const MapPoint &point, sint32 &uindex)
{
	for (sint32 i = 0; i < m_nElements ; i++)
	{
		if (m_array[i].CanInciteRevolution(point))
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::AbleToCauseUnhappinessTarget(const MapPoint &point, sint32 &uindex)
{
	for (sint32 i = 0; i < m_nElements ; i++)
	{
		if (m_array[i].CanCauseUnhappiness(point))
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::AbleToExpelTarget(const MapPoint &point, sint32 &uindex)
{
	for (sint32 i = 0; i < m_nElements ; i++)
	{
		if (m_array[i].CanExpel(point))
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::CanExpel() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCanExpel() &&
		   m_array[i].CanPerformSpecialAction())
			return true;
	}
	return false;
}

bool ArmyData::CanCreatePark(sint32 &uindex) const
{
	for (sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasCreateParks() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::CanCreatePark() const
{
	for (sint32 i = 0; i < m_nElements; i++) {
		if(!m_array[i].GetDBRec()->HasCreateParks() ||
		   !m_array[i].CanPerformSpecialAction()) {
			return false;
		}
	}
	return true;
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

	if(!g_player[m_owner]->IsRobot()
	||  g_network.IsClient()
	&& g_network.IsLocalPlayer(m_owner))
		if(!VerifyAttack(UNIT_ORDER_CREATE_PARK, point,
						 c.GetOwner()))
			return ORDER_RESULT_ILLEGAL;

	//InformAI(UNIT_ORDER_CREATE_PARK, point); //does nothing here but could be implemented

	ActionSuccessful(SPECATTACK_CREATEPARK, m_array[uindex], c);
	AddSpecialActionUsed(m_array[uindex]);

	SlicObject * so = new CityReport("911NaniteCleanseCompleteVictim", c);
	so->AddCivilisation(c->GetOwner());
	g_slicEngine->Execute(so);

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_CreateParkUnit,
						   GEA_Unit, m_array[uindex].m_id,
						   GEA_City, c.m_id,
						   GEA_End);

	return ORDER_RESULT_SUCCEEDED;
}

bool ArmyData::CanPillage(uint32 & uindex) const
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

bool ArmyData::CanPillage() const
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

	for(sint32 i = 0; i < m_nElements; i++)
	{
		if (!m_array[i].GetDBRec()->GetCanPillage() ||
		    !m_array[i].CanPerformSpecialAction()
		   )
		{
			return false;
		}
	}

	return true;
}

//not used
void ArmyData::ThisMeansWAR(PLAYER_INDEX defense_owner)
{
	Assert(g_player[m_owner]);
	if(g_player[m_owner]) {
		g_player[m_owner]->ThisMeansWAR(defense_owner);
	}
}

ORDER_RESULT ArmyData::Pillage(bool test_ownership)
{
	uint32 uindex;
	if (!CanPillage(uindex))
		return ORDER_RESULT_ILLEGAL;

	MapPoint pos;
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


	if(!g_player[m_owner]->IsRobot()
	|| (g_network.IsClient()
	&&  g_network.IsLocalPlayer(m_owner))
	)
		if(test_ownership
		&& !VerifyAttack(UNIT_ORDER_PILLAGE_UNCONDITIONALLY, m_pos, cellOwner)
		)
			return ORDER_RESULT_ILLEGAL;

	//InformAI(UNIT_ORDER_PILLAGE, m_pos); //does nothing here but could be implemented

	AddSpecialActionUsed(m_array[uindex]);

//	bool CanPillageTileImp = true;

	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *trec = g_theTerrainImprovementDB->Get(imp);
		if(trec->GetCantPillage()){
			return ORDER_RESULT_ILLEGAL;
			//CanPillageTileImp = false;
			//break;
		}
	}

	// EMOD to allow units to take a tile if they have a flag instead of pillging,
	// sometimes it good to take a fortress use will probably go to lawyers
	// or diplomats this us temporary until i can make it an order that costs gold
	//			sint32 rushmod = g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetUnitRushModifier();
	//			sint32 impgold = cell->AccessImprovement(i).GetMaterialCost();
	for(sint32 j = 0; j < m_nElements; j++) {
		sint32 rushmod = static_cast<sint32>(g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetWonderRushModifier());
		sint32 goldcost = (cell->GetGoldProduced() * cell->GetNumDBImprovements()) * rushmod;
		if((m_array[j].GetDBRec()->GetCanCaptureTile()) && (g_player[m_owner]->m_gold->GetLevel() > goldcost)){ // && (terrainutil_CanBeCaptured(m_pos))) {
			if (cellOwner != m_owner) {
				cell->SetOwner(m_owner);
				g_theWorld->ChangeOwner(pos, cellOwner, m_owner);
				g_player[cellOwner]->m_gold->AddGold(goldcost); //may crash because of barbs?
				g_player[m_owner]->m_gold->SubGold(goldcost);
				return ORDER_RESULT_SUCCEEDED;
			}
		}
	}


//	if(!CanPillageTileImp){
//		return ORDER_RESULT_ILLEGAL;
//	}

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_PillageUnit,
						   GEA_Unit, m_array[uindex],
						   GEA_End);

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_CutImprovements,
						   GEA_MapPoint, pos,
						   GEA_End);

	return ORDER_RESULT_SUCCEEDED;
}

bool ArmyData::CanInjoin(sint32 &uindex) const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasCanInjoin() &&
		   m_array[i].CanPerformSpecialAction()) {
			uindex = i;
			return true;
		}
	}
	return false;
}

bool ArmyData::CanInjoin() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasCanInjoin() &&
		   m_array[i].CanPerformSpecialAction()) {
			return true;
		}
	}
	return false;
}

ORDER_RESULT ArmyData::Injoin(const MapPoint &point)
{
	sint32 uindex;
	if(!CanInjoin(uindex))
		return ORDER_RESULT_ILLEGAL;

	Unit    c = GetAdjacentCity(point);
	Unit &  u = m_array[uindex];

	if(c.m_id == 0)
		return ORDER_RESULT_ILLEGAL;

	if(c.IsInjoined())
		return ORDER_RESULT_ILLEGAL;

	if(c.GetOwner() == m_owner)
		return ORDER_RESULT_ILLEGAL;

	//InformAI(UNIT_ORDER_INJOIN, point); //does nothing here but could be implemented

	AddSpecialActionUsed(u);
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_InjoinUnit,
						   GEA_Unit, u,
						   GEA_City, c.m_id,
						   GEA_End);
	ActionSuccessful(SPECATTACK_INJOIN, u, c);
	/* disabled because injunction icon of city already tells the success and therefore message spamming can be avoided
	g_slicEngine->Execute
	    (new AggressorReport("911InjunctionCompleteAttack", u, c));
	*/

	return ORDER_RESULT_SUCCEEDED;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::GetFirstMoveThisTurn
//
// Description: Returns true if each unit in this army is flagged as k_UDF_FIRST_MOVE
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : k_UDF_FIRST_MOVE is reset every turn by GEV_ResetAllMovement(GEA_Player).
//              This is the flag that ensures that units can make one move.
//
//----------------------------------------------------------------------------
bool ArmyData::GetFirstMoveThisTurn() const
{
	for(sint32 i = 0; i < m_nElements; i++)
	{
		if(!m_array[i].GetFirstMoveThisTurn())
			return false;
	}
	return true;
}

bool ArmyData::HasLeftMap() const
{
	for(sint32 i = 0; i < m_nElements; i++)
	{
		if (!m_array[i].HasLeftMap())
			return false;
	}
	return true;
}

bool ArmyData::CanNukeCity() const
{
	for(sint32 i = 0; i < m_nElements; i++)
	{
		if (m_array[i].GetDBRec()->HasNuclearAttack())
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CurMinMovementPoints
//
// Description: Get the min of the m_movement_points of this army's units and
//              put it in double &cur.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::CurMinMovementPoints(double &cur) const
{
	cur = 10000000.0;

	for(sint32 i = 0; i < m_nElements; i++)
	{
		cur = std::min<double>(cur, m_array[i].GetMovementPoints());
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::MinMovementPoints
//
// Description: Get the min of the m_MaxMovePoints of this army's units (from their UnitRecords)
//              and put it in double &cur.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::MinMovementPoints(double &cur) const
{
	cur = 10000000.0;

	for(sint32 i = 0; i < m_nElements; i++)
	{
		cur = std::min<double>(cur, m_array[i].GetMaxMovePoints());
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::GetBombardRange
//
// Description: Test if this army can bombard. Fill in min_rge and max_rge.
//
// Parameters : sint32 & min_rge : the min of the bombard ranges of this army's units
//            : sint32 & max_rge : the max of the bombard ranges of this army's units
//
// Globals    : -
//
// Returns    : bool true if this army contains at least one unit which can bombard.
//
// Remark(s)  : Used to allow bombarding from a distance.
//
// Additions by PFT Dec 18, 2004. And modified by Martin Gühmann.
//
//----------------------------------------------------------------------------
bool ArmyData::GetBombardRange(sint32 & min_rge, sint32 & max_rge)
{
	min_rge = 0x7fffffff;
	max_rge = 0;

	for(sint32 i = 0; i < m_nElements; i++)
	{
		const UnitRecord *rec = m_array[i].GetDBRec();
		sint32 rge;
		if(rec->GetBombardRange(rge)) // Check if it has a range (else rge contains garbage below)
		{
			if(rge > max_rge)
				max_rge = rge;
			if(rge < min_rge)
				min_rge = rge;
		}
	}

	if(max_rge > 0)
		return true;

	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanBombard
//
// Description: Returns true if some member of this army can bombard some
//              unit or tile improvement at a map location.
//
// Parameters : MapPoint & point
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : Called by CtpAi::BombardAdjacentEnemies
//
//----------------------------------------------------------------------------
bool ArmyData::CanBombard(const MapPoint &point) const
{
	// Check for bombardable units
	CellUnitList defender;
	g_theWorld->GetArmy(point, defender);

	if(defender.Num() > 0)
	{
		for(sint32 i = 0; i < m_nElements; ++i)
		{
			if(m_array[i].CanBombard(defender))
			{
				return true;
			}
		}
	}

#if 0
	// This prevents the AI from figuring out wheather a unit can be bombarded
	// Fix this first, before you enable it again.
	// In case of doubt check the method's callers

	// The actual bombard event is guarded by the CanBombardType method
	// so it doesn't work anyway.

	// EMOD Bombard tile Imps by E 20-JAN-2006
	// This could harm the AI by bombarding every tileimp on its way

	// Check for bombardable (= pillagable) tile improvements
	Cell *  cell = g_theWorld->GetCell(point);

	if(m_owner != cell->GetOwner()) // Don't bombard your own terrain improvements
	{
		for(sint32 ti = 0; ti < cell->GetNumDBImprovements(); ++ti)
		{
			TerrainImprovementRecord const * trec =
			    g_theTerrainImprovementDB->Get(cell->GetDBImprovement(ti));

			if(!trec->GetCantPillage())
			{
				/// @todo Check m_array to find a suitable bombarder for
				///       the improvement type (land, sea, etc.)
				return CanBombard();
			}
		}
	}
#endif

	// Nothing bombardable at this point
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanBombard
//
// Description: Returns true if if some member of this army can bombard right now
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanBombard() const
{
	for(sint32 i = m_nElements - 1; i>= 0; i--)
	{
		if(m_array[i].GetDBRec()->GetCanBombard()
		&& m_array[i].CanPerformSpecialAction()
		){
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::BombardCity
//
// Description: Returns true if one unit in this army has bombarded a city
//              at MapPoint &point
//
// Parameters : MapPoint &point
//              bool doAnimations   : If true show
//
// Globals    : g_player            : Player array [see Player::InitPlayer for
//                                    initialized player data]
//
// Returns    : bool
//
// Remark(s)  : true means that the bombarding unit has tried to destroy a
//              building and kill a pop
//
//----------------------------------------------------------------------------
bool ArmyData::BombardCity(const MapPoint &point, bool doAnimations)
{
	Unit c = g_theWorld->GetCell(point)->GetCity();
	sint32 prob;
	bool atLeastOneBombarded = false;
	if(c.m_id != 0) {
		if(c.GetOwner() == m_owner)
			return false;

		if(c.Flag(k_UDF_CANT_BE_ATTACKED))
			return false;

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

			// Removed Verify because the player shouldn't accidentally
			// bombard since its a button and aim
			// Do not remove this, there are always people who do not
			// know what there are doing.
			if(!g_player[m_owner]->IsRobot()
			|| (g_network.IsClient()
			&&  g_network.IsLocalPlayer(m_owner))
			){
				if(!VerifyAttack(UNIT_ORDER_BOMBARD, point, c.GetOwner())
				){
					return false;
				}
			}

			AddSpecialActionUsed(m_array[i]);
			MapPoint nonConstPos = point;

			if(doAnimations)
			{
				// * Added auto-center for bombardment
				if (g_selected_item->IsAutoCenterOn()
					&& (c.GetOwner() == g_selected_item->GetVisiblePlayer()
					|| m_owner == g_selected_item->GetVisiblePlayer()
					))
				{
					g_director->AddCenterMap(point);
				}
				g_director->AddAttackPos(m_array[i], nonConstPos);

				bool out_of_fuel;

				//EMOD Multiple Attacks/Blitz removed it from only Air to a separate flag - 2-24-2006
				//if(!m_array[i].GetDBRec()->GetMovementTypeAir()) {  //this allowed for multiple air bombard
				//4-28-2006 EMOD changed to allow fighters to move after attack but not attack again
				if(m_array[i].GetDBRec()->GetMultipleAttacks())
				{
					m_array[i].DeductMoveCost(g_theConstDB->Get(0)->GetSpecialActionMoveCost(), out_of_fuel);
				}
				else if(m_array[i].GetDBRec()->GetMovementTypeAir())
				{
					m_array[i].DeductMoveCost(k_MOVE_COMBAT_COST, out_of_fuel);
				}
				else
				{
					m_array[i].SetMovementPoints(0.0);
				}
			}

			atLeastOneBombarded = true;

			sint32 r = g_rand->Next(100);
			DPRINTF(k_DBG_GAMESTATE, ("Bombarding 0x%lx: r1 = %d\n", c.m_id, r));

			prob = static_cast<sint32>(rec->GetZBRangeAttack() - buildingutil_GetCityWallsDefense(c.GetCityData()->GetImprovements(), m_owner));
			if(prob < 0)
			{
				prob = 0;
			}

			// Add precision strike flag

			if( (m_array[i].GetDBRec()->GetPrecisionStrike()) && (r < prob))
			{
				c.DestroyRandomBuilding();
			}
			else
			{
				if(r < g_theConstDB->Get(0)->GetBombardDestroyBuildingChance() * prob)
				{
					c.DestroyRandomBuilding();
				}
			}

			//regard impacts of killing civilians
			Diplomat &  cell_diplomat   = Diplomat::GetDiplomat
			    ((PLAYER_UNASSIGNED == c.GetOwner()) ? PLAYER_INDEX_VANDALS : c.GetOwner());
			sint32 regardcost   = static_cast<sint32>( ((m_array[i].GetDBRec()->GetAttack()) / 5.0) * -1.0);
			// if (player has advance->IsMassMedia()){
			// Add Targets Civilian Flag
			if(m_array[i].GetDBRec()->GetTargetsCivilians()&& (r < prob))
			{
				c.CD()->ChangePopulation(-1);
				StringId strId;
				g_theStringDB->GetStringID("REGARD_EVENT_ATTACKED_CIVILIANS", strId);
				cell_diplomat.LogRegardEvent
				    (m_owner, regardcost, REGARD_EVENT_MILITARY_POWER, strId);
			}
			else
			{
				if(c.PopCount() > 1
				&&(g_rand->Next(100) < g_theConstDB->Get(0)->GetBombardKillPopChance() * prob)
				){
					DPRINTF(k_DBG_GAMESTATE, ("Removing one pop from 0x%lx\n", c.m_id));
					c.CD()->ChangePopulation(-1);
				}
			}

			// EMOD
			// Add wondercheck for EnablesPunativeAirstrikes	5 JUL 2006
			////////////////////////
			bool Punstrike = true;
			if((!wonderutil_GetEnablesPunativeAirstrikes(g_player[m_owner]->m_builtWonders)) && (!m_array[i].GetMovementTypeAir())){ //(!m_array[i].GetDBRec()->GetMovementTypeAir())){
				Punstrike = false;
			}
			// END EMOD

			if(!m_array[i].GetDBRec()->GetSneakBombard()
			&& !Punstrike
			){ //EMOD added by E for sneak bombarding
				Diplomat & defending_diplomat = Diplomat::GetDiplomat(c.GetOwner());
				defending_diplomat.LogViolationEvent(m_owner, PROPOSAL_TREATY_CEASEFIRE);
			}

		}
		return atLeastOneBombarded;
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::Bombard
//
// Description: Attempt to bombard a MapPoint (location).
//
// Parameters : MapPoint
//
// Globals    : g_director              : display manager
//              g_network               : multiplayer manager
//              g_player                : list of active players
//              g_selectedItem          : selected unit or city
//              g_theWorld              : the map
//
// Returns    : ORDER_RESULT            : attempt success/failure indication
//
// Remark(s)  : No special effects.
//
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::Bombard(const MapPoint &orderPoint)
{
	MapPoint        point   = orderPoint;
	CellUnitList    defender;
	g_theWorld->GetArmy(point, defender);
	sint32 i;

	if(point == m_pos)
	{
		return ORDER_RESULT_ILLEGAL;
	}
	else
	{
		//PFT 30 mar 05, bombard from distance
		sint32 dist = m_pos.NormalizedDistance(point);
		sint32 min_rge, max_rge;
		if(GetBombardRange(min_rge,max_rge))
		{

			DPRINTF(k_DBG_GAMESTATE, ("Getting BombardRange max_rge %d, dist %d\n", max_rge, dist));

			if(dist > max_rge)
			{//the target is out of this army's bombarding range
				return ORDER_RESULT_ILLEGAL;
			}
		}
		else //army can't bombard
			return ORDER_RESULT_ILLEGAL;
	}

	if (defender.Num() < 1)
	{ // No defenders
		if(BombardCity(point, true))
		{//so if there's a city, bombard it
			return ORDER_RESULT_SUCCEEDED;
		}
		else
		{
			// EMOD Bombard tile Imps  by E	20-JAN-2006
			for (i = 0; i< m_nElements; i++)
			{
				if (m_array[i].GetDBRec()->GetCanBombardTiles())
				{
					Cell *cell = g_theWorld->GetCell(point);
					sint32 cellOwner = cell->GetOwner();
					for(sint32 ti = 0; ti < cell->GetNumDBImprovements(); ti++)
					{
						sint32 imp = cell->GetDBImprovement(ti);
						const TerrainImprovementRecord *trec = g_theTerrainImprovementDB->Get(imp);
						if(trec->GetCantPillage() == 0)
						{
							for(i = 0; i < m_nElements; i++)
							{
								if(!m_array[i].GetDBRec()->GetSneakBombard())
								{
									Diplomat & defending_diplomat = Diplomat::GetDiplomat(cellOwner);
									defending_diplomat.LogViolationEvent(m_owner, PROPOSAL_TREATY_CEASEFIRE);
									g_theWorld->CutImprovements(point);

									return ORDER_RESULT_SUCCEEDED;
								}
							}
						}
					}
				}
			}

			return ORDER_RESULT_ILLEGAL; // Or allow bombarding tile improvements?  NOW ADDED by E
			// end EMOD
		}
	}

	// Defenders present code starts here
	for(i = 0; i < defender.Num(); i++) {//return illegal if at least one can't be attacked
		if(defender[i].Flag(k_UDF_CANT_BE_ATTACKED))
			return ORDER_RESULT_ILLEGAL;
	}

	if(defender.GetOwner() == m_owner) {//return illegal if they're our own troops
		return ORDER_RESULT_ILLEGAL;
	}

	// Removed Verify because the player shouldn't
	// accidentally bombard since its a button and aim
	// Do not remove: There are always people who do not
	// know what they are doing.
	if(!g_player[m_owner]->IsRobot()
	|| (g_network.IsClient()
	&&  g_network.IsLocalPlayer(m_owner)))
		if(!VerifyAttack(UNIT_ORDER_BOMBARD, point, defender.GetOwner()))
			return ORDER_RESULT_ILLEGAL;

	sint32 numAttacks = 0;
	sint32 numAlive = m_nElements;
	bool out_of_fuel;

	for (i = m_nElements - 1; i>= 0; i--) {
		if(!m_array[i].CanPerformSpecialAction())
			continue;

		// EMOD SpecialBombardments PrecisionStrike and
		// TargetsCivilians check can only attack cities
		if(m_array[i].GetDBRec()->GetPrecisionStrike() || m_array[i].GetDBRec()->GetTargetsCivilians()){
			if(BombardCity(point, true)) {//so if there's a city, bombard it
				return ORDER_RESULT_SUCCEEDED;
			}
			return ORDER_RESULT_ILLEGAL;
		}


		if (m_array[i].CanBombard(defender)) {

			DPRINTF(k_DBG_GAMESTATE, ("unit i=%d, CanBombard(defender)=%d\n", i, m_array[i].CanBombard(defender)));

			if(m_array[i].Bombard(defender, false)) {//this unit tries to bombard a random member of defender
				numAttacks++;
				if(numAttacks == 1)
				{
					// Inform defender
				}

				// * Added auto-center for bombardment
				if (g_selected_item->IsAutoCenterOn()
					&& (defender.GetOwner() == g_selected_item->GetVisiblePlayer()
					|| m_owner == g_selected_item->GetVisiblePlayer()
					))
				{
					g_director->AddCenterMap(point);
				}

				g_director->AddAttackPos(m_array[i], point);

				AddSpecialActionUsed(m_array[i]);

				// EMOD Multiple Attacks/Blitz removed it from
				// only Air to a separate flag - 2-24-2006

				//if(!m_array[i].GetDBRec()->GetMovementTypeAir()) {  //this allowed for multiple air bombard

				if(m_array[i].GetDBRec()->GetMultipleAttacks()) {
					m_array[i].DeductMoveCost(g_theConstDB->Get(0)->GetSpecialActionMoveCost(), out_of_fuel);
				} else if(m_array[i].GetDBRec()->GetMovementTypeAir()) {
					m_array[i].DeductMoveCost(k_MOVE_COMBAT_COST, out_of_fuel);
				} else {
					m_array[i].SetMovementPoints(0.0);
				}

//				if(!m_array[i].GetDBRec()->GetMultipleAttacks()) {
//					m_array[i].SetMovementPoints(0.0);
//				} else {
//					m_array[i].DeductMoveCost(g_theConstDB->SpecialActionMoveCost(), out_of_fuel); //k_MOVE_COMBAT_COST
//				}
			}
		}
	}
	//emod

	PLAYER_INDEX  defense_owner;
	defense_owner = defender.GetOwner();

	bool AlltaSneakBombard = true;

	for (i = m_nElements - 1; i>= 0; i--)
	{
		if(!m_array[i].GetDBRec()->GetSneakBombard())
		{
			AlltaSneakBombard = false;
			break;
		}
	}

	if(!m_array[0].GetDBRec()->GetSneakBombard())
	{
		AlltaSneakBombard = false;
	}

	//EMOD
	// add WonderCheck for EnablesPunativeAirstrikes
	bool Punstrike = true;
	if(!wonderutil_GetEnablesPunativeAirstrikes(g_player[m_owner]->m_builtWonders)
	&& !m_array[i].GetMovementTypeAir()
//	&& !m_array[i].GetDBRec()->GetMovementTypeAir()
	){
		Punstrike = false;
	}

	bool AllDefSneakAttack = true;
	for(i = 0; i < defender.Num(); i++)
	{
		if(!defender[i].GetDBRec()->GetSneakAttack())
		{
			AllDefSneakAttack = false;
			break;
		}
	}

	if(!defender[0].GetDBRec()->GetSneakAttack())
	{
		AllDefSneakAttack = false;
	}

	if(!AlltaSneakBombard && !AllDefSneakAttack && !Punstrike){
		Diplomat & defending_diplomat = Diplomat::GetDiplomat(defense_owner);
		defending_diplomat.LogViolationEvent(m_owner, PROPOSAL_TREATY_CEASEFIRE);
	}

	//end EMOD


	if(numAttacks <= 0)
		return ORDER_RESULT_ILLEGAL;

	if(numAlive < 1)
		return ORDER_RESULT_SUCCEEDED;//a Pyrrhic victory, these were our guys
	//do counterbombarding and kill off attacking units
	for (i = 0; i<defender.Num(); i++)
	{
		if (defender[i].CanCounterBombard(*this))
		{
			defender[i].Bombard(*this, true);
		}
	}
////////////////ORIGINAL CODE
///////////////Used in Unitdata::bombard is it necessary here? m_array and defender both us .bombard so why is it killing?
	for (i = m_nElements - 1; 0 <= i; i--)
	{
		if (m_array[i].GetHP() < 0.999)
		{
			m_array[i].Kill(CAUSE_REMOVE_ARMY_COUNTERBOMBARD, defender.GetOwner());
		}
	}

	//kill off defending units that were newly damaged
	for (i = defender.Num() - 1; 0 <= i; i--)
	{
		if (defender[i].GetHP() < 0.999)
		{
			defender[i].Kill(CAUSE_REMOVE_ARMY_BOMBARD, GetOwner());
		}
	}
	////////////////

	//emod

	for (i = 0; i< m_nElements; i++)
	{
		if (m_array[i].GetDBRec()->GetCollateralTileDamage())
		{
			Cell *cell = g_theWorld->GetCell(point);
//			sint32 cellOwner = cell->GetOwner();
			for(sint32 ti = 0; ti < cell->GetNumDBImprovements(); ti++)
			{
				sint32 imp = cell->GetDBImprovement(ti);
				const TerrainImprovementRecord *trec = g_theTerrainImprovementDB->Get(imp);
				if(!trec->GetCantPillage())
				{
					for(i = 0; i < m_nElements; i++)
					{
						g_theWorld->CutImprovements(point);
					}
				}
			}
		}
	}

	//if there's a city at point, try to destroy a building and kill a pop
	BombardCity(point, false);
	return ORDER_RESULT_SUCCEEDED;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanInterceptTrade
//
// Description: Returns true if some unit in this army can intercept trade.
//              In this case, fills in uindex with the army's CellUnitList index
//              of the first such unit.
//
// Parameters : uint32 &uindex   :
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanInterceptTrade(uint32 &uindex) const
{
	for (sint32 i = m_nElements - 1; i>= 0; i--)
	{
		if (m_array[i].CanInterceptTrade())
		{
			uindex = i;
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::InterceptTrade
//
// Description: Attempt to pirate a trade route.
//
// Parameters : -
//
// Globals    : g_director              : display manager
//              g_network               : multiplayer manager
//              g_player                : list of active players
//              g_selectedItem          : selected unit or city
//              g_theSoundDB            : sound database
//              g_theSpecialEffectDB    : special effect database
//              g_theWorld              : the map
//
// Returns    : ORDER_RESULT            : attempt success/failure indication
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_RESULT ArmyData::InterceptTrade()
{
	sint32 typeIndex = g_theSpecialEffectDB->FindTypeIndex("SPECEFFECT_PIRATE");
	sint32 effectId = g_theSpecialEffectDB->Get(typeIndex)->GetValue();
	sint32 soundId = g_theSoundDB->FindTypeIndex("SOUND_ID_SLAVE_RAIDS");

	for (sint32 i = m_nElements - 1; i>= 0; i--)
	{
		if (m_array[i].CanInterceptTrade() &&
			m_array[i].CanPerformSpecialAction())
		{
		        // code apparently a left-over from CTP1, see: https://github.com/civctp2/civctp2/pull/154
			if(!g_player[m_owner]->IsRobot()
			||(g_network.IsClient()
			&& g_network.IsLocalPlayer(m_owner))
			){
				Cell *cell = g_theWorld->GetCell(m_pos);
				for (sint32 j = 0; j < cell->GetNumTradeRoutes(); j++)
				{
					TradeRoute route = cell->GetTradeRoute(j);
					sint32 route_owner = route.GetOwner();
					if(!route.IsActive()) // skip deactivated routes (only exist for drawing until revisited, see #256)
					    continue;
					if (AgreementMatrix::s_agreements.HasAgreement(
						route_owner,
						m_owner,
						PROPOSAL_REQUEST_STOP_PIRACY))
					{
						SlicObject *so = new SlicObject("12IABreakNoPiracy");
						so->AddRecipient(m_owner);
						so->AddCivilisation(m_owner);
						so->AddCivilisation(route_owner);
						so->AddUnit(m_array[i]);
						so->AddLocation(m_pos);
						so->AddOrder(UNIT_ORDER_INTERCEPT_TRADE);
						g_slicEngine->Execute(so);

						g_selected_item->ForceDirectorSelect(Army(m_id));
						return ORDER_RESULT_ILLEGAL;
					}
					if (AgreementMatrix::s_agreements.HasAgreement(
						route_owner,
						m_owner,
						PROPOSAL_TREATY_TRADE_PACT))
					{
						SlicObject *so = new SlicObject("12IABreakTradePact");
						so->AddRecipient(m_owner);
						so->AddCivilisation(m_owner);
						so->AddCivilisation(route_owner);
						so->AddUnit(m_array[i]);
						so->AddLocation(m_pos);
						so->AddOrder(UNIT_ORDER_INTERCEPT_TRADE);
						g_slicEngine->Execute(so);

						g_selected_item->ForceDirectorSelect(Army(m_id));
						return ORDER_RESULT_ILLEGAL;
					}
				}
			}

			ORDER_RESULT const	res	= m_array[i].InterceptTrade(); // call UnitData::InterceptTrade() which adds GEV_SetPiratingArmy

			if (res == ORDER_RESULT_ILLEGAL)
			{
				// No action: nothing has happened yet.
			}
			else
			{
				//InformAI(UNIT_ORDER_INTERCEPT_TRADE, m_pos); //does nothing here but could be implemented
				if (g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(m_pos))
				{
					if(g_selected_item->IsAutoCenterOn())
					{
						g_director->AddCenterMap(m_pos);
					}

					g_director->AddSpecialEffect(m_pos, effectId, soundId);
				}
				AddSpecialActionUsed(m_array[i]);
				m_isPirating = true;
			}

			return res;
		}
	}
	return ORDER_RESULT_ILLEGAL;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::NumOrders
//
// Description: Returns the number of orders in this army's m_orders list
//
// Parameters : -
//
// Globals    : -
//
// Returns    : sint32
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 ArmyData::NumOrders() const
{
	return m_orders->GetCount();
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::GetOrder
//
// Description: Returns the index-th order in this army's m_orders list.
//
// Parameters : sint32 index
//
// Globals    : -
//
// Returns    : Order
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
//
// Name       : ArmyData::AutoAddOrders
//
// Description: Add an order to this army's m_orders list and if
//              the army's owner is valid, call ExecuteOrders.
//
// Parameters : UNIT_ORDER_TYPE order   :
//              Path *path              :
//              MapPoint &point         :
//              sint32 argument         :
//
// Globals    : g_useOrderQueues
//            : g_player                : player array
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::AutoAddOrders(UNIT_ORDER_TYPE order, Path *path,
							 const MapPoint &point, sint32 argument)
{
	if(g_player[m_owner]->IsRobot() && !m_addOrdersAI)
	{
		return;
	}

	if(!g_useOrderQueues)
	{
		ClearOrders();
	}

	m_orders->AddTail(new Order(order, path, point, argument));

	if(point != m_pos || order != UNIT_ORDER_INTERCEPT_TRADE){
	    StopPirating(); // to ensure trade route is not regarded as pirated when moving off (which is not possible any more on the new position, see https://github.com/civctp2/civctp2/issues/75
	    }

	if(m_owner >= 0 && m_owner < k_MAX_PLAYERS && g_player[m_owner])
	{
		ExecuteOrders(false);
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::AutoAddOrdersWrongTurn
//
// Description: Clear this army's orders and add an order to this army's m_orders
//              list. If the army's owner is valid call ExecuteOrders.
//
// Parameters : UNIT_ORDER_TYPE order   :
//              Path *path              :
//              MapPoint &point         :
//              sint32 argument         :
//
// Globals    : g_player                : player array
//
// Returns    : -
//
// Remark(s)  : used when expelling units
//
//----------------------------------------------------------------------------
void ArmyData::AutoAddOrdersWrongTurn(UNIT_ORDER_TYPE order, Path *path,
									  const MapPoint &point, sint32 argument)
{
	if(g_player[m_owner]->IsRobot() && !m_addOrdersAI)
	{
		return;
	}

	ClearOrders();

	m_orders->AddTail(new Order(order, path, point, argument));

	if(point != m_pos || order != UNIT_ORDER_INTERCEPT_TRADE){
	    StopPirating(); // to ensure trade route is not regarded as pirated when moving off (which is not possible any more on the new position, see https://github.com/civctp2/civctp2/issues/75
	    }

	if(m_owner >= 0 && m_owner < k_MAX_PLAYERS && g_player[m_owner]) {
		ExecuteOrders(false);
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::AddOrders
//
// Description: Add a UNIT_ORDER_TYPE order to this army's m_orders list.
//
// Parameters : UNIT_ORDER_TYPE order   :
//              Path *path              :
//              MapPoint &point         :
//              sint32 argument         :
//              GAME_EVENT passedEvent  :
//
// Globals    : g_network               :
//            : g_selected_item         :
//            : g_player                : player array [see Player::InitPlayer for initialized player data]
//
// Returns    : -
//
// Remark(s)  : the m_orders list is what gets deleted when you call ArmyData::ClearOrders()
//
//----------------------------------------------------------------------------
void ArmyData::AddOrders(UNIT_ORDER_TYPE order, Path *path, const MapPoint &point,
						 sint32 argument, GAME_EVENT passedEvent)
{
	if(g_player[m_owner]->IsRobot() && !m_addOrdersAI)
	{
		delete path;

		return;
	}

	bool execute = true;

	if(g_network.IsActive() && g_network.IsLocalPlayer(m_owner) &&
	   !g_network.IsMyTurn() && g_selected_item->GetCurPlayer() == m_owner &&
	   g_selected_item->GetVisiblePlayer() == m_owner) {
		if(path)
			delete path;
		Assert(false);
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
		DPRINTF(k_DBG_GAMESTATE, ("Adding event order for army 0x%lx, event = %s, targetPos = (%i, %i)\n", m_id, g_gevManager->GetEventName(attackOrder->m_eventType), point.x, point.y));
		attackOrder->m_gameEventArgs = args;

		m_orders->AddTail(attackOrder);
		execute = false;

		Assert(path == NULL); // We do not do anything with the path, so we should not have one, otherwise we have a memory leak
	}
	else
	{
		if(g_network.IsActive() && order == UNIT_ORDER_ADD_EVENT) {

		}
		else
		{
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

	if(point != m_pos || order != UNIT_ORDER_INTERCEPT_TRADE){
	    StopPirating(); // to ensure trade route is not regarded as pirated when moving off (which is not possible any more on the new position, see https://github.com/civctp2/civctp2/issues/75
	    }

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
					if(!g_network.IsActive() || g_network.IsLocalPlayer(m_owner))
						ExecuteOrders();
			}
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::ClearOrders
//
// Description: Delete all orders from this army's m_orders list
//
// Parameters : -
//
// Globals    : g_network
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::ClearOrders()
{
	if(m_orders->GetHead())
	{
		DPRINTF(k_DBG_GAMESTATE, ("Army 0x%lx clearing orders\n", m_id));
		if(g_network.IsHost())
		{
			g_network.Block(m_owner);
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_CLEAR_ORDERS,
										  (uint32)m_id));
			g_network.Unblock(m_owner);
		}
		else if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))
		{
			g_network.SendAction(new NetAction(NET_ACTION_CLEAR_ORDERS,
											   (uint32)m_id));
		}
		m_orders->DeleteAll();
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::ExecuteOrders
//
// Description: Execute the orders in this army's m_orders list.
//
// Parameters : bool propagate
//
// Globals    : g_theUnitPool
//            : g_theArmyPool
//            : g_theDiplomacyLog
//            : g_player             : player array
//            : g_selected_item
//            : g_network
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::ExecuteOrders(bool propagate)
{
	Assert(m_hasBeenAdded);
	if(!m_hasBeenAdded)
		return false;

	m_dontKillCount++;
	CheckAddEventOrder();

	Assert(m_dontKillCount);
	if(m_dontKillCount) {
		m_dontKillCount--;

		if(m_needToKill && !m_dontKillCount) {
			Army me(m_id);
			me.Kill();

			return false;
		}
	}

	if(!m_orders->GetHead()) {
		return false;
	}
	m_flags |= k_CULF_EXECUTED_THIS_TURN;

	Order *order;
	bool completedOrder = false;
	bool keepGoing = true;
	sint32 owner = m_owner;
	Army me(m_id);
	if(m_orders->GetHead()->m_order != UNIT_ORDER_EXPEL_TO) {

		if(g_network.IsActive() && m_owner == g_selected_item->GetVisiblePlayer() &&
		   !g_network.IsMyTurn()) {
		   return false;
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
//		UNIT_ORDER_TYPE type = m_orders->GetHead()->m_order;

		if(propagate) {
			if(m_orders->GetHead()->m_order != UNIT_ORDER_EXPEL_TO) {
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_EXECUTE_ORDERS,
											  (uint32)m_id));
			}
		}
	}

	DPRINTF(k_DBG_GAMESTATE, ("Army 0x%lx Executing order %s @ (%d,%d), turn=%d\n", m_id, g_orderInfo[m_orders->GetHead()->m_order].m_name, m_pos.x, m_pos.y, g_player[m_owner]->m_current_round));

	while(keepGoing && m_nElements > 0 &&
		  (order = m_orders->GetHead()) != NULL) {

		switch(order->m_order) {
			case UNIT_ORDER_MOVE:
			case UNIT_ORDER_MOVE_TO:
			case UNIT_ORDER_MOVE_THEN_UNLOAD:
			case UNIT_ORDER_VICTORY_MOVE:
				completedOrder = ExecuteMoveOrder(order);
				if(!completedOrder)
					keepGoing = false;
				break;
			case UNIT_ORDER_ENTRENCH:
				Entrench();
				completedOrder = true;
				break;
			case UNIT_ORDER_SLEEP:
				Sleep();
				completedOrder = true;
				break;
			case UNIT_ORDER_DETRENCH:
				Detrench();
				completedOrder = true;
				break;
			case UNIT_ORDER_UNLOAD:
			case UNIT_ORDER_UNLOAD_SELECTED_STACK:
				completedOrder = ExecuteUnloadOrder(order);
				if(!completedOrder)
					keepGoing = false;
				break;
			case UNIT_ORDER_TELEPORT_TO:
			case UNIT_ORDER_EXPEL_TO:
				order->m_order = UNIT_ORDER_TELEPORT_TO;
				completedOrder = ExecuteTeleportOrder(order);
				break;
			case UNIT_ORDER_GROUP:
				GroupAllUnits();
				completedOrder = true;
				break;
			case UNIT_ORDER_UNGROUP:
				UngroupUnits();
				completedOrder = true;
				break;
			case UNIT_ORDER_GROUP_UNIT:
				GroupUnit(Unit(order->m_argument));
				completedOrder = true;
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
				completedOrder = true;
				SetUnloadMovementPoints();
				break;
			case UNIT_ORDER_DISBAND:
				completedOrder = true;
				Disband();
				break;
			case UNIT_ORDER_SETTLE:
				completedOrder = true;
				Settle();
				break;
			case UNIT_ORDER_FINISH_ATTACK:
				completedOrder = true;
				FinishAttack(order);
				break;
			case UNIT_ORDER_BOARD_TRANSPORT:
				completedOrder = true;
				DoBoardTransport(order);
				break;
			case UNIT_ORDER_WAKE_UP:
				completedOrder = true;
				WakeUp();
				break;
			case UNIT_ORDER_ADD_EVENT:
				completedOrder = false;
				keepGoing = false;
				break;
			case UNIT_ORDER_SETTLE_IN_CITY:
				completedOrder = true;
				SettleInCity();
				break;
			case UNIT_ORDER_UPGRADE:
				completedOrder = true;
				Upgrade();
				break;
			default:
				Assert(false);
		}

#ifdef _DEBUG
			if (g_theDiplomacyLog) {
				g_theDiplomacyLog->PopRegardRequest();
			}
#endif _DEBUG

		if (me.IsValid())
		{
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
			completedOrder = false;
		}
		else
		{
			keepGoing = false;
		}
	}
	if(g_network.IsHost()) {
		g_network.Unblock(owner);

	}

	KillRecord *kill;
	while((kill = m_killMeSoon->RemoveHead()) != NULL)
	{
		if (kill->m_unit.IsValid())
		{
			kill->m_unit.Kill(kill->m_cause, kill->m_who);
		}
		delete kill;
	}

	if(m_dontKillCount)
	{
		m_dontKillCount--;
		if(m_needToKill && !m_dontKillCount)
		{
			me.Kill();
		}
	}

	if (g_selected_item)
	{
		PLAYER_INDEX    player;
		ID              id;
		SELECT_TYPE     type;

		g_selected_item->GetTopCurItem(player, id, type);
		if (id == me)
		{
			g_selected_item->Refresh();
		}
	}

	return true;
}

//---------------------------------------------------------------------------------------------------------
//
// Name       : ArmyData::InformAI
//
// Description: If a unit is ordered to steal technology log the regard cost
//
// Parameters : UNIT_ORDER_TYPE order_type
//              MapPoint &pos
//
// Globals    : g_theStringDB
//
// Returns    : -
//
// Remark(s)  : Only defined for UNIT_ORDER_STEAL_TECHNOLOGY, but not called for it.
//              All other calls do nothing. Probably did more originally, there are
//              lots of dud calls.
//
//---------------------------------------------------------------------------------------------------------
void ArmyData::InformAI(const UNIT_ORDER_TYPE order_type, const MapPoint &pos)
{
	const Unit city = g_theWorld->GetCity(pos);

	if (!city.IsValid())
		return;

	sint32 cost;
	Diplomat & city_diplomat = Diplomat::GetDiplomat(city.GetOwner());
	switch (order_type)
	{
	case UNIT_ORDER_STEAL_TECHNOLOGY:
		city_diplomat.GetCurrentDiplomacy(m_owner).GetCreateParkRegardCost(cost);
		// CreateParkRegardCost since there's no 'StealTechologyRegardCost'

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

//----------------------------------------------------------------------------
//
// Name       : ArmyData::ExecuteMoveOrder
//
// Description: Returns true if army is at or (if appropriate) next to it's destination. (?)
//              Otherwise it inserts a MoveArmy event after the current event in the event queue.
//
// Parameters : Order *order
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : Called by ExecuteOrders (whose switch defines the move orders).
//              If this returns false then completed_order = false in the above switch.
//
//----------------------------------------------------------------------------
bool ArmyData::ExecuteMoveOrder(Order *order)
{
	WORLD_DIRECTION d = NOWHERE;

	if ((order->m_order == UNIT_ORDER_MOVE) ||
	    (order->m_order == UNIT_ORDER_MOVE_THEN_UNLOAD)
	   )
	{
		if( order->m_path->IsEndDir() ||
		   (order->m_order == UNIT_ORDER_MOVE_THEN_UNLOAD && order->m_point.IsNextTo(m_pos))
		  )
		{
			if (order->m_order == UNIT_ORDER_MOVE_THEN_UNLOAD && order->m_point.IsNextTo(m_pos))
			{
				return ExecuteUnloadOrder(order);
			}
			return true;
		}

		// Keep moving along the assigned path
		order->m_path->GetCurrentDir(d);
	}
	else // UNIT_ORDER_MOVE_TO or UNIT_ORDER_VICTORY_MOVE
	{
		if (m_pos == order->m_point)
			return true;

		// Move from m_pos in the direction of the target point
		d = m_pos.GetNeighborDirection(order->m_point);
	}

	if(NOWHERE == d)
	{
		// Something is not right
		if (g_network.IsClient())
		{
			g_network.RequestResync(RESYNC_ARMY_POS);
		}
		else if (g_network.IsHost() && !g_network.IsLocalPlayer(m_owner))
		{
			g_network.Resync(m_owner);
		}
	}
	else
	{
		if(DoLeaveOurLandsCheck(order->m_point, UNIT_ORDER_MOVE_TO)){
		    return false;
		    }

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
		                       GEV_MoveArmy,
		                       GEA_Army, m_id,
		                       GEA_Direction, d,
		                       GEA_Int, order->m_order,
		                       GEA_Int, order->m_argument,
		                       GEA_MapPoint, order->m_point,
		                       GEA_End);
	}

	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::IsOccupiedByForeigner
//
// Description: Returns true if there is at least one foreign unit in MapPoint &pos
//
// Parameters : MapPoint &point
//
// Globals    : g_theWorld
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::IsOccupiedByForeigner(const MapPoint &pos)
{
	Cell * cell = g_theWorld->GetCell(pos);

	return (cell->GetNumUnits() > 0)
	    && (cell->UnitArmy()->GetOwner() != m_owner);
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CheckLoadSleepingCargoFromCity
//
// Description: Attempt to load the sleeping units at this army's position
//              (city or airfield) into whatever transports may be there.
//
// Parameters : Order *order            : why?
//
// Globals    : g_player                : player array
//              g_theWorld              : the map
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::CheckLoadSleepingCargoFromCity(Order *order)
{
	Cell *cell = g_theWorld->GetCell(m_pos);
	//if neither in a city nor in an airfield
	if( cell->GetCity().m_id == 0
	&& !terrainutil_HasAirfield(m_pos))
		return;

	if(cell->GetNumUnits() < 1)
		return;

	for(sint32 i = 0; i < m_nElements; i++)
	{
		if(m_array[i].GetCargoCapacity() > 0)
		{
			for (int j = cell->UnitArmy()->Num() - 1; j >= 0; j--)
			{
				Unit u = cell->UnitArmy()->Access(j);
				if(!u.IsAsleep())
					continue;

				if(m_array[i].CanCarry(u))
				{
					bool out_of_fuel;
					u.SetIsInTransport(m_array[i]);
					u.DeductMoveCost(k_MOVE_ENTER_TRANSPORT_COST, out_of_fuel);
					g_theWorld->RemoveUnitReference(m_pos, u);
					u.UndoVision();
					u.RemoveUnitVision();
					m_array[i].InsertCargo(u);
				}

				if(m_array[i].GetCargoCapacity() < 1)
					break;
			}
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::Move
//
// Description:
//
// Parameters : WORLD_DIRECTION d  :
//              Order *order       :
//
// Globals    : g_player           : player array
//
// Returns    : bool
//
// Remark(s)  : Actually, not used. Must be left over from CTP1
//
//----------------------------------------------------------------------------
/* commented, since not used any more
bool ArmyData::Move(WORLD_DIRECTION d, Order *order)
{
	MapPoint oldPos = m_pos;

	MapPoint newPos;
	sint32 r = oldPos.GetNeighborPosition(d, newPos);

	if(!r)
		return true;

	if(CheckSpecialUnitMove(newPos)) {
		return true;
	}

	CheckLoadSleepingCargoFromCity(order);

	if(IsMovePointsEnough(newPos)) {
		if(IsOccupiedByForeigner(newPos)) {

			CellUnitList *defender = g_theWorld->GetCell(newPos)->UnitArmy();


			sint32 i;
			for(i = 0; i < defender->Num(); i++) {
				if(defender->Access(i).Flag(k_UDF_CANT_BE_ATTACKED))
					return false;
			}

			if(!CheckWasEnemyVisible(newPos))
			{
				g_player[m_owner]->ContactMade(defender->GetOwner());

				if(g_player[m_owner]->IsRobot())
				{
					if(g_network.IsClient()
					&& g_network.IsLocalPlayer(m_owner)
					){
						return false;
					}
				}
				else
				{
					g_selected_item->ForceDirectorSelect(Army(m_id));
					return false;
				}
			}

			if(order->m_order == UNIT_ORDER_MOVE) {

				while(!order->m_path->IsEndDir())
					order->m_path->IncDir();




				if(!order->m_argument)
					return false;




				if(newPos != order->m_point)
					return false;
			}

			if(order->m_order == UNIT_ORDER_MOVE_THEN_UNLOAD)
				return false;

			for(i = 0; i < m_nElements; i++) {
				if(m_array[i].Flag(k_UDF_FOUGHT_THIS_TURN) ||
				   m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN)) {

					return false;
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
			if(g_player[m_owner]->IsRobot())
			{
				sint32 defOwner = defender->GetOwner();
				if(g_player[m_owner]->WillViolateCeaseFire(defOwner) ||
				   g_player[m_owner]->WillViolatePact(defOwner)) {
					sint32 oldAlliance = g_player[m_owner]->m_broken_alliances_and_cease_fires;
					g_player[m_owner]->BreakAlliance(defOwner);
					g_player[m_owner]->BreakCeaseFire(defOwner, oldAlliance == g_player[m_owner]->m_broken_alliances_and_cease_fires);
				}
			}

			AutoAddOrders(UNIT_ORDER_FINISH_ATTACK, NULL, newPos, 0);
			return false;
		}

		if(DoLeaveOurLandsCheck(newPos, UNIT_ORDER_MOVE_TO))
		{
			return false;
		}

		return FinishMove(d, newPos, order->m_order);

	}
	return false;
}
*/

bool ArmyData::FinishMove(WORLD_DIRECTION d, MapPoint &newPos, UNIT_ORDER_TYPE order)
{
	CellUnitList transports;
	sint32 canMoveIntoTransport = NumUnitsCanMoveIntoTransport(newPos, transports);

	// Problem with AI: It load units into transport helis, even so they are not the target.
	// However these things don't seem to fix it, properly.
	if
	  (
	      g_theWorld->GetCity(newPos).m_id == 0
	   && canMoveIntoTransport > 0
//	   && order == UNIT_ORDER_BOARD_TRANSPORT
//	   && !g_player[m_owner]->IsRobot()
//	   && !g_theWorld->IsOnSameContinent(newPos, m_pos)
	  )
	{
		Army army(m_id);

		g_gevManager->AddEvent
		                      (
		                       GEV_INSERT_AfterCurrent,
		                       GEV_MoveIntoTransport,
		                       GEA_Army, army,
		                       GEA_MapPoint, newPos,
		                       GEA_End
		                      );

		return true;
	}
	else
	{
		bool const  didMove = MoveIntoCell(newPos, order, d);

		if(didMove)
		{
			for(sint32 i = m_nElements - 1; i >= 0; i--)
			{
				uint32 moveType = m_array[i].GetMovementType();
				if(!g_theWorld->CanEnter(newPos, moveType))
				{
					// That's clearly wrong
					if((moveType & k_BIT_MOVEMENT_TYPE_SHALLOW_WATER)
					&&!(moveType & k_BIT_MOVEMENT_TYPE_WATER)
					&&  g_theWorld->GetCell(newPos)->GetEnv() & k_BIT_MOVEMENT_TYPE_WATER
					&&  wonderutil_GetAllBoatsDeepWater(g_player[GetOwner()]->m_builtWonders)
					){

					}
					else
					{
						if(g_theWorld->GetCity(newPos).m_id == 0)
						{
							g_gevManager->AddEvent
							                      (
							                       GEV_INSERT_AfterCurrent,
							                       GEV_KillUnit,
							                       GEA_Unit,      m_array[i].m_id,
							                       GEA_Int,       CAUSE_REMOVE_ARMY_ILLEGAL_CELL,
							                       GEA_Player,    -1,
							                       GEA_End
							                      );
						}
					}
				}
			}
		}

		return didMove;
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::FinishAttack
//
// Description:
//
// Parameters : Order *order       :
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::FinishAttack(Order *order)
{
	bool res = MoveIntoForeigner(order->m_point);
	if(!res)
		return true;

	if(m_nElements <= 0)
		return true;

	for (sint32 i = 0; i < m_nElements; i++)
	{
		if(!m_array[i].GetDBRec()->GetMovementTypeAir()) {
			m_array[i].SetMovementPoints(0.0);
		}
	}

	WORLD_DIRECTION d = m_pos.GetNeighborDirection(order->m_point);
	return FinishMove(d, order->m_point, order->m_order);
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CheckSpecialUnitMove
//
// Description:
//
// Parameters : MapPoint &pos       :
//
// Globals    : g_player            : player array
//
// Returns    : bool
//
// Remark(s)  : Deals with Nuke attacks.
//
//----------------------------------------------------------------------------
bool ArmyData::CheckSpecialUnitMove(const MapPoint &pos)
{
	SlicObject	*so ;

	Unit city = g_theWorld->GetCell(pos)->GetCity();

	if(city.IsValid()) {
		if(city.GetOwner() != m_owner) {
			for(sint32 i = 0; i < m_nElements; i++) {

				if(m_array[i].GetDBRec()->HasNuclearAttack()) {

					if(!g_player[m_owner]->IsRobot()
					||(g_network.IsClient()
					&& g_network.IsLocalPlayer(m_owner)))
						if(!VerifyAttack(UNIT_ORDER_MOVE_TO, pos,
										 city.GetOwner()))
							return true;

					if(city.SafeFromNukes()) {
						so = new CityReport("10gSafeFromNukes", city);
						so->AddCivilisation(city.GetOwner());
						g_slicEngine->Execute(so);

						so = new SlicObject("11gSafeFromNukes");
						so->AddRecipient(GetOwner());
						so->AddCivilisation(city.GetOwner());
						so->AddCity(city);
						g_slicEngine->Execute(so);

						g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
											   GEA_Unit, m_array[i],
											   GEA_Int, CAUSE_REMOVE_ARMY_NUKE,
											   GEA_Player, -1,
											   GEA_End);

						return true;
					}

					if (g_slicEngine->GetSegment("49WorldPollutionNuclearWar")->TestLastShown(m_owner, 10)) {
						so = new SlicObject("49WorldPollutionNuclearWar") ;
						so->AddCity(city);
						so->AddAllRecipients();
						g_slicEngine->Execute(so) ;
					}

					sint32 pollution;
					if (m_array[i].GetDBRec()->GetDeathPollution(pollution))
					{
						g_player[m_owner]->AdjustEventPollution(pollution);
					}

					g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_NukeCityUnit,
										   GEA_Unit, m_array[i],
										   GEA_City, city.m_id,
										   GEA_End);

#if 0
					UnitDynamicArray killList;
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
					return true;
				}


			}
			return false;
		} else {
			return false;
		}
	}

	CellUnitList *defender = g_theWorld->GetCell(pos)->UnitArmy();
	if(defender && defender->Num() > 0) {
		Assert(defender->Access(0).IsValid());
		if(defender->Access(0).IsValid() && defender->Access(0).GetOwner() != m_owner) {
			for(sint32 i = 0; i < m_nElements; i++) {


				if(m_array[i].GetDBRec()->HasNuclearAttack()) {

					if(!CheckWasEnemyVisible(pos)) {
						Order *order = m_orders->GetHead();

						if(order && order->m_order == UNIT_ORDER_MOVE) {

							while(!order->m_path->IsEndDir())
								order->m_path->IncDir();
						}

						return true;
					}

					if(!g_player[m_owner]->IsRobot()
					||(g_network.IsClient()
					&& g_network.IsLocalPlayer(m_owner)))
						if(!VerifyAttack(UNIT_ORDER_MOVE_TO, pos,
										 defender->GetOwner()))
							return true;






                    if (g_slicEngine->GetSegment("49WorldPollutionNuclearWar")->TestLastShown(m_owner, 10)) {
                        so = new SlicObject("49WorldPollutionNuclearWar") ;
                        so->AddAllRecipients() ;
                        g_slicEngine->Execute(so) ;
                    }

                    sint32 pollution;
                    if (m_array[i].GetDBRec()->GetDeathPollution(pollution))
                    {
					    g_player[m_owner]->AdjustEventPollution(pollution);
                    }

					g_thePollution->AddNukePollution(pos);

					g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_NukeLocationUnit,
										   GEA_Unit, m_array[i],
										   GEA_MapPoint, pos,
										   GEA_End);
					return true;
				}


			}
		}
	}

	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::MoveIntoForeigner
//
// Description: Returns true if
//
// Parameters : MapPoint &point
//
// Globals    : g_player    : player array
//
// Returns    : bool
//
// Remark(s)  : Added check for at war instead of other agreements
//              Returns allways false
//
//----------------------------------------------------------------------------
bool ArmyData::MoveIntoForeigner(const MapPoint &pos)
{
	CellUnitList defender;
	g_theWorld->GetArmy(pos, defender);

	if(defender.Num() <= 0)
		return false;

	PLAYER_INDEX    defense_owner   = defender.GetOwner();
	PLAYER_INDEX    attack_owner    = GetOwner();

	if (defense_owner == attack_owner)
	{
		// No use attacking own troops
		return false;
	}

	if(!CanFight(defender))
	{
		return false;
	}

	Diplomat & diplomat = Diplomat::GetDiplomat(attack_owner);
//outcommented because not called? What a shitty argumentation
//	if (IsEnemy(defense_owner) &&
//		!g_player[m_owner]->WillViolateCeaseFire(defense_owner) &&
//		!g_player[m_owner]->WillViolatePact(defense_owner) &&
	if(
	           g_player[attack_owner]->HasWarWith(defense_owner)
	   ||
	      (
	           g_player[attack_owner]->IsRobot()
	        && diplomat.HasWarOrDesiresPreemptivelyWith(defense_owner)
	      )
	  )
	{
		Battle(pos, defender);
	}
	else
	{
		//EMOD to allow for sneak attacks without popup
		Unit ta = GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());

		//Army me(m_id);
		if (ta.m_id == 0)
		{
			ta = m_array[0];
		}

		Unit td = defender.GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());

		if (td.m_id == 0)
		{
			td = defender[0];
		}

		sint32 i;
		bool AlltaSneakAttack = true;
		for (i = m_nElements - 1; i>= 0; i--)
		{
			if(!m_array[i].GetDBRec()->GetSneakAttack())
			{
				AlltaSneakAttack = false;
				break;
			}
		}

		if (ta.m_id == 0)
		{
			if(!m_array[0].GetDBRec()->GetSneakAttack())
			{
				AlltaSneakAttack = false;
			}
		}

		bool AllDefSneakAttack = true;
		for(i = 0; i < defender.Num(); i++)
		{
			if(!defender[i].GetDBRec()->GetSneakAttack())
			{
				AllDefSneakAttack = false;
				break;
			}
		}

		if (td.m_id == 0)
		{
			if(!defender[0].GetDBRec()->GetSneakAttack())
			{
				AllDefSneakAttack = false;
			}
		}

		if(!AlltaSneakAttack && !AllDefSneakAttack)
		{
			// Have to check wehther we desire war
			if(VerifyAttack(UNIT_ORDER_MOVE_TO, pos, defense_owner))
			{
				Battle(pos, defender);
			}

			return false;
			// And check whether the attacker should plan this attack
			// The following would allow units of differents civilisations to share the same tile if we had not Battle before.
	//		return VerifyAttack(UNIT_ORDER_MOVE_TO, pos, defense_owner);
		}
		else
		{ //EMOD
			Fight(defender);
		} //EMOD
	}
	return false;
}

void ArmyData::Battle(const MapPoint &pos, CellUnitList & defender)
{
	InformAI(UNIT_ORDER_FINISH_ATTACK, pos);

	sint32 numCloaked = 0;
	for (sint32 i = 0; i < defender.Num(); i++)
	{
		if (defender[i].IsCloaked())
			numCloaked++;
	}
	if(numCloaked >= defender.Num() || m_array[0].AccessData()->PacMan())
	{
		defender.KillList(CAUSE_REMOVE_ARMY_UNKNOWN, GetOwner());
	}
	else
	{
		bool allSingleUse = true;
		sint32 i;
		for(i = 0; i < m_nElements; i++)
		{
			if(!m_array[i].GetDBRec()->GetSingleUse())
			{
				allSingleUse = false;
				break;
			}
		}

		Army me(m_id);
		if(!allSingleUse)
		{
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
								   GEV_Battle,
								   GEA_Army, me,
								   GEA_MapPoint, pos,
								   GEA_End);
		}

		for(i = 0; i < m_nElements; i++)
		{
			if(m_array[i].GetDBRec()->GetSingleUse())
			{
				static CellUnitList defender;
				defender.Clear();
				g_theWorld->GetArmy(pos, defender);
					MapPoint nonConstPos = pos;
				g_director->AddAttackPos(m_array[i], nonConstPos);
				m_array[i].Bombard(defender, false);

				for(sint32 j = 0; j < defender.Num(); j++)
				{
					if(defender[j].GetHP() < 0.5)
					{
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
	//	return false;
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::VerifyAttack
//
// Description: Returns true if this army's owner (g_player[m_owner])
//              and defense_owner are neither allied nor have a ceasefire.
//
// Parameters : UNIT_ORDER_TYPE order
//            : MapPoint &point
//            : sint32 defense_owner
//
// Globals    : g_player    : player array
//            : g_network
//
// Returns    : bool
//
// Remarks    :
//
//----------------------------------------------------------------------------
bool ArmyData::VerifyAttack(UNIT_ORDER_TYPE order, const MapPoint &pos,
							sint32 defense_owner)
{
	if (g_player[m_owner]->HasWarWith(defense_owner))
	{
		return true;
	}

	// Modified to catch accidental attacks
	SlicObject *so;
	if (    g_network.IsActive()
	     && g_network.TeamsEnabled()
	     && (g_player[m_owner]->m_networkGroup ==
	            g_player[defense_owner]->m_networkGroup
	        )
	   )
	{
		so = new SlicObject("110aCantAttackTeammates");
	}
	else if(!IsEnemy(defense_owner))
	{
		so = new SlicObject("110CantAttackAllies");
	}
	// EMOD - Added Civ2 style Dove Party that prevents war
	// if you have ParliamentaryVoteChance govt
	else if (g_rand->Next(100) <
	         g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)
	            ->GetParliamentaryVoteChance()
	        )
	{
		so = new SlicObject("Civ2Doves");
	}
	else if (!g_player[m_owner]->IsRobot())
	{
		so = new SlicObject("999AttackWarning");
	}
	else
	{
		return true;
	}

	so->AddRecipient(m_owner);
	so->AddCivilisation(defense_owner);
	so->AddUnit(m_array[0]);
	so->AddLocation(pos);
	so->AddOrder(order);
	g_slicEngine->Execute(so);
	g_selected_item->ForceDirectorSelect(Army(m_id));

	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::ExertsZOC
//
// Description: Returns true if some unit in this army exerts a zone of control
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::ExertsZOC() const
{
	for(sint32 i = 0; i < m_nElements; i++)
	{
		if (!m_array[i].IsNoZoc())
			return true;
	}

	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::UpdateZOCForMove
//
// Description: -
//
// Parameters : MapPoint &point
//            : WORLD_DIRECTION d
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::UpdateZOCForMove(const MapPoint &pos, WORLD_DIRECTION d)
{
	Cell *cell = g_theWorld->GetCell(m_pos);
	sint32 i;
	Army me(m_id);
	bool doneRemoving = false;

	if(cell->GetCity().IsValid()) {
		doneRemoving = true;
	} else {
		for(i = cell->GetNumUnits() - 1; i >= 0; i--) {

			if(cell->AccessUnit(i).GetArmy() != me &&
			   !cell->AccessUnit(i).IsNoZoc()) {
				doneRemoving = true;
				break;
			}
		}
	}

	uint8 dirs = 0;
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

	    DynamicArray<MapPoint> points;
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
			g_theWorld->AddOtherArmyZOC(points[mp], m_owner, me, Unit());
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
	UnitDynamicArray units;
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

//----------------------------------------------------------------------------
//
// Name       : ArmyData::MoveIntoCell
//
// Description: Test if this army can move into MapPoint &pos
//
// Parameters : MapPoint &pos
//            : UNIT_ORDER_TYPE order
//            : WORLD_DIRECTION d
//
// Globals    : g_player                : player array
//              g_selectedItem          : selected unit or city
//              g_theWorld              : the map
//              g_gevManager
//
// Returns    : bool
//
// Remark(s)  : if true, inserts GEV_MoveUnits
//
//----------------------------------------------------------------------------
bool ArmyData::MoveIntoCell(const MapPoint &pos, UNIT_ORDER_TYPE order, WORLD_DIRECTION d)
{
	bool zocViolation;
	bool alliedCity;

	bool ignoreZoc = (order == UNIT_ORDER_FINISH_ATTACK || order == UNIT_ORDER_VICTORY_MOVE);

	if(!CanMoveIntoCell(pos, zocViolation, ignoreZoc, alliedCity))
	{
		DPRINTF(k_DBG_GAMESTATE, ("Move Failure: Army 0x%lx Executing order %s @ (%d,%d) to (%d,%d), turn=%d\n", m_id, g_orderInfo[m_orders->GetHead()->m_order].m_name, m_pos.x, m_pos.y, pos.x, pos.y, g_player[m_owner]->m_current_round));

		if(zocViolation)
		{
			RevealZOCUnits(pos);
			if(m_owner == g_selected_item->GetVisiblePlayer())
			{
				g_selected_item->ForceDirectorSelect(Army(m_id));
			}
		}

		if(alliedCity)
		{
			Unit city = g_theWorld->GetCity(pos);
			//EMOD to allow for sneak attacks without popup
			Unit ta = GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());

			if (ta.m_id == 0)
			{
				ta = m_array[0];
			}

			bool AlltaSneakAttack = true;
			for(sint32 i = m_nElements - 1; i>= 0; i--)
			{
				if(!m_array[i].GetDBRec()->GetSneakAttack())
				{
					AlltaSneakAttack = false;
					break;
				}
			}

			if (ta.m_id == 0)
			{
				if(!m_array[0].GetDBRec()->GetSneakAttack()){
					AlltaSneakAttack = false;
				}
			}

			if(!AlltaSneakAttack){
				/// @todo Explain: What is the purpose of this?
				//        Or fix it.
				// emod - this is to prevent sneakattack units that walk into an occupied cell from starting a war
				VerifyAttack(UNIT_ORDER_MOVE_TO, pos, city.GetOwner());
				return false;
		//		return VerifyAttack(UNIT_ORDER_MOVE_TO, pos, city.GetOwner());
			}

			return true;
		}

		if(g_player[m_owner]->IsRobot())
		{
			if(CanAtLeastOneCargoUnloadAt(pos, false))
			{
				g_gevManager->AddEvent
				                      (
				                       GEV_INSERT_AfterCurrent,
				                       GEV_UnloadOrder,
				                       GEA_Army, m_id,
				                       GEA_MapPoint, pos,
				                       GEA_End
				                      );
			}
		}

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ClearOrders,
							   GEA_Army, m_id,
							   GEA_End);
		return false;
	}

	if(ExertsZOC())
	{
		UpdateZOCForMove(pos, d);
	}

	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MoveUnits,
						   GEA_Army, m_id,
						   GEA_MapPoint, m_pos,
						   GEA_MapPoint, pos,
						   GEA_End);

	return true;
}

void ArmyData::MoveActors(const MapPoint &pos, bool teleport)
{
	if(g_selected_item->GetPlayerOnScreen() < 0
	&& g_selected_item->GetPlayerOnScreen() != g_selected_item->GetVisiblePlayer())
		return;

	Unit top_src = GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());
	if (top_src.m_id == 0)
		top_src = m_array[0];

	UnitActor **restOfStack = NULL;
	sint32 numRest = 0;

	Unit nonDead;

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

	if (teleport || !(top_src.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()))) {
		g_director->AddTeleport(top_src, m_pos, pos, numRest, restOfStack);
	} else {
		g_director->AddMove(top_src, m_pos, pos, numRest, restOfStack, top_src.GetMoveSoundID());
	}

	if (top_src.GetData()->HasLeftMap()) {
		g_director->AddHide(top_src);
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::MoveUnits
//
// Description: Move this army from m_pos to pos.
//
// Parameters : MapPoint &pos
//
// Globals    : g_network               : multiplayer manager
//              g_selectedItem          : selected unit or city
//              g_gevManager            :
//              g_radarMap              :
//              g_theWorld              : the map
//              g_player	            : player array
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::MoveUnits(const MapPoint &pos)
{
	if (m_flags & k_CULF_IN_SPACE)
	{
		return;
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	bool notReported = true;
#endif

	MapPoint oldPos = m_pos;
	bool anyVisible = false;
	for(sint32 i = 0; i < m_nElements; i++)
	{
		anyVisible = anyVisible || (m_array[i].GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()));
		// EMOD - Rebasing of units, especially aircraft - code removed trying to create a code that automatically moves a unit from a
		//city to another city anywhere in the world and costing that unit 1 move.

		if(m_array[i].GetDBRec()->GetCanRebase())
		{
			if (!IsOccupiedByForeigner(pos))
			{
				if(g_theWorld->HasCity(pos)
				|| terrainutil_HasAirfield(pos)
				){  //add unit later?
					m_array[i].SetPosition(pos);
				}
			}
		}
		//end EMOD

		if(g_theWorld->GetCell(pos)->GetNumUnits() >= k_MAX_ARMY_SIZE)
		{
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
			                       GEA_Unit, m_array[i],
			                       GEA_Int, CAUSE_REMOVE_ARMY_EXPELLED_NO_CITIES,
			                       GEA_Player, -1,
			                       GEA_End);
			m_array[i].SetFlag(k_UDF_TELEPORT_DEATH);

			Assert(false);
		}
		else
		{
			if(m_array[i].IsEntrenching() || m_array[i].IsEntrenched())
			{
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_DetrenchUnit,
				                       GEA_Unit, m_array[i],
				                       GEA_End);
			}

			if(m_array[i].IsAsleep())
			{
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_WakeUnit,
				                       GEA_Unit, m_array[i],
				                       GEA_End);
			}

			g_theWorld->RemoveUnitReference(m_pos, m_array[i]);

			UnitDynamicArray revealedUnits;
			m_array[i].MoveToPosition(pos, &revealedUnits);

			if(m_array[i].GetNumCarried() > 0)
			{
				for(sint32 j = 0; j < m_array[i].GetNumCarried(); ++j)
				{
					m_array[i].GetData()->GetCargoList()->Access(j).SetPosAndNothingElse(pos);
				}
			}

			if( revealedUnits.Num() > 0
			&& !m_array[i].GetDBRec()->GetSingleUse()
			){
#if defined(_DEBUG) || defined(USE_LOGGING)
				if(notReported)
				{
					DPRINTF(k_DBG_GAMESTATE, ("Army 0x%lx revealed %d units during move:\n", m_id, revealedUnits.Num()));
					for(sint32 uu = 0; uu < revealedUnits.Num(); uu++)
					{
						DPRINTF(k_DBG_GAMESTATE, ("  Unit 0x%lx\n", revealedUnits[uu].m_id));
					}
					notReported = false;
				}
#endif
				if( !g_network.IsActive()
				&&  g_player[m_owner]->IsHuman()
				){
					ClearOrders();
				}
				else if(g_network.IsHost())
				{
					if( !g_network.IsLocalPlayer(m_owner)
					||  m_owner == g_selected_item->GetVisiblePlayer()
					){
						ClearOrders();
					}
				}
				else if(g_network.IsClient())
				{
					if(g_network.IsLocalPlayer(m_owner))
					{
						ClearOrders();
					}
				}
				else if(g_player[m_owner]->IsRobot())
				{
					ClearOrders();
					m_addOrdersAI = false;
					Scheduler::s_needAnotherCycle = true;
				}
			}

			if(m_array[i].HasLeftMap())
			{
				Assert(false);
			}
		}
	}

	if(anyVisible && g_radarMap)
	{
		g_radarMap->RedrawTile(oldPos); // oldPos only used here
		g_radarMap->RedrawTile(m_pos);  // m_pos hasn't been modified so oldPos and m_pos are still identical
	}

	if(HasLeftMap())
	{
		g_selected_item->RegisterRemovedArmy(m_owner, Army(m_id));
	}

	//original
	DeductMoveCost(pos);

	m_pos = pos;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CheckTerrainEvents
//
// Description: If this army's m_pos contains a GoodyHut, open it. Then look around
//              and redraw all the immediately neighbouring tiles.
//
// Parameters : -
//
// Globals    : g_network               : multiplayer manager
//              g_tiledMap              :
//              g_theWorld              : the map
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::CheckTerrainEvents()
{
	Cell *cell = g_theWorld->GetCell(m_pos);
	if(cell->GetGoodyHut())
	{
		cell->GetGoodyHut()->OpenGoody(m_owner, m_pos);

		if(cell->GetGoodyHut())
		{
			cell->DeleteGoodyHut();
		}

		MapPoint pos2;
		g_tiledMap->RedrawTile(&m_pos);
		for(
		    WORLD_DIRECTION d = NORTH;
		                    d < NOWHERE;
		                    d = (WORLD_DIRECTION)((sint32)d + 1)
		   )
		{
			if(m_pos.GetNeighborPosition(d, pos2))
			{
				g_tiledMap->RedrawTile(&pos2);
			}
		}

		if(g_network.IsHost())
		{
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_REMOVE_HUT,
										  m_pos.x, m_pos.y));
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanMoveIntoTransport
//
// Description: Returns true if this army can move into some transports at MapPoint &pos
//
// Parameters : MapPoint &pos
//              CellUnitList &transports
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : May involve ungrouping the army.
//
//----------------------------------------------------------------------------
bool ArmyData::CanMoveIntoTransport(const MapPoint &pos,
									CellUnitList &transports) const
{
	if(!g_theWorld->GetEmptyTransports(pos, transports)) // Make a list of the transports with empty slots at pos
		return false;

	return CanMoveIntoThisTransport(transports); // true if this army can be loaded onto those transports
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanMoveIntoTransport
//
// Description: Returns the number of units that will go into CellUnitList &transports
//
// Parameters : MapPoint &pos
//              CellUnitList &transports
//
// Globals    : -
//
// Returns    : sint32
//
// Remark(s)  : May involve ungrouping the army.
//
//----------------------------------------------------------------------------
sint32 ArmyData::NumUnitsCanMoveIntoTransport(const MapPoint &pos,
											  CellUnitList &transports) const
{
	if(!g_theWorld->GetEmptyTransports(pos, transports)) // Make a list of the transports with empty slots at pos
		return 0;

	return NumUnitsCanMoveIntoThisTransport(transports); // true if this army can be loaded onto those transports
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::NumUnitsCanMoveIntoThisTransport
//
// Description: Returns the number of units that will go into CellUnitList &transports
//
// Parameters : CellUnitList &transports
//
// Globals    : -
//
// Returns    : sint32
//
// Remark(s)  : May involve ungrouping the army.
//
//----------------------------------------------------------------------------
sint32 ArmyData::NumUnitsCanMoveIntoThisTransport(const CellUnitList &transports) const
{
	sint32 i, j;
	sint32 count[k_MAX_ARMY_SIZE];
	memset(count, 0, k_MAX_ARMY_SIZE * sizeof(sint32));
	for(i = 0; i < transports.Num(); i++)
	{
		count[i] = transports.Get(i).GetCargoCapacity();
	}

	sint32 cargo_fits = 0;
	bool searching;
	for (i = 0; i<m_nElements; i++)
	{
		searching = true;
		for (j = 0; searching && (j < transports.Num()); j++)
		{
			if ((0 < count[j]) && transports.Get(j).CanCarry(m_array[i]))
			{
				count[j]--;
				searching = false;
			}
		}

		if (!searching)
		{
			cargo_fits++;
		}
	}
	return cargo_fits;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanMoveIntoThisTransport
//
// Description: Test if this army can move into CellUnitList &transports
//
// Parameters : CellUnitList &transports
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : May involve ungrouping the army.
//
//----------------------------------------------------------------------------
bool ArmyData::CanMoveIntoThisTransport(const CellUnitList &transports) const
{
	sint32 i, j;
	sint32 count[k_MAX_ARMY_SIZE];
	memset(count, 0, k_MAX_ARMY_SIZE * sizeof(sint32));
	for(i = 0; i < transports.Num(); i++) {
		count[i] = transports.Get(i).GetCargoCapacity();
	}

	bool searching;
	for (i = 0; i<m_nElements; i++)
	{
		searching = true;
		for (j=0; searching && (j<transports.Num()); j++)
		{
			if ((0 < count[j]) && transports.Get(j).CanCarry(m_array[i]))
			{
				count[j]--;
				searching = false;
			}
		}
		if (searching)
		{
			return false;
		}
	}
	return true;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::MoveIntoTransport
//
// Description: Move this army into the members of CellUnitList &transports
//
// Parameters : MapPoint &pos
//              CellUnitList &transports
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::MoveIntoTransport(const MapPoint &pos, CellUnitList &transports)
{
	m_dontKillCount++;

	Unit top_src = GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());
	if (!top_src.IsValid())
	{
		Assert(m_array[0].IsValid());
		top_src = m_array[0];
	}

	sint32 count = 0;
	for (sint32 i = m_nElements - 1; i>= 0; i--)
	{
		for (sint32 j=0; j < transports.Num(); j++)
		{
			if (transports[j].CanCarry(m_array[i]) &&
				transports[j].GetCargoCapacity() > 0)
			{
				count++;

				m_array[i].SetIsInTransport(transports[j]);
				bool dummy_out_of_fuel;
				m_array[i].DeductMoveCost
				    (k_MOVE_ENTER_TRANSPORT_COST, dummy_out_of_fuel);

				g_theWorld->RemoveUnitReference(m_pos, m_array[i]);
				m_array[i].UndoVision();
				m_array[i].RemoveUnitVision();

				transports[j].InsertCargo(m_array[i]);

				transports[j].GetArmy()->WakeUp();

				if (g_soundManager && top_src.IsValid())
				{
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

				if (g_network.IsHost())
				{
					g_network.Block(m_owner);
					g_network.Enqueue(transports[j].AccessData());
					g_network.Unblock(m_owner);
				}

				break;
			}
		}
	}

	if(!count)
	{
		m_dontKillCount--;
		return true;
	}

	UnitActor **restOfStack = NULL;
	sint32 numRest = m_nElements - 1;

	if (numRest > 0)
	{
		restOfStack = new (UnitActor* [numRest]);
		GetActors(top_src, restOfStack);
	}

	MapPoint directorDoesntLikeConsts = pos;
	g_director->AddMove(top_src, m_pos, directorDoesntLikeConsts, numRest, restOfStack, top_src.GetMoveSoundID());

	UpdateZOCForRemoval();

	if(m_dontKillCount)
	{
		m_dontKillCount--;
		if(m_needToKill && !m_dontKillCount)
		{
			Army me(m_id);
			me.Kill();
		}
	}

	return true;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::DoBoardTransport
//
// Description: Try to board transports at m_pos.
//
// Parameters : Order *order
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::DoBoardTransport(Order *order)
{
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
						   GEV_MoveIntoTransport,
						   GEA_Army, m_id,
						   GEA_MapPoint, m_pos,
						   GEA_End);
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanAtLeastOneCargoUnloadAt
//
// Description: Returns true if at least one member of this army is carrying some cargo (unit)
//              that can unload into MapPoint & dest_pos
//
// Parameters : MapPoint & old_pos   : ?
//              MapPoint & dest_pos
//              bool & used_vision
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CanAtLeastOneCargoUnloadAt(const MapPoint & unload_pos,
                                          const bool & used_vision,
                                          const bool check_move_points) const
{
	for(sint32 i = 0; i < m_nElements; i++)
	{
		if(m_array[i].CanAtLeastOneCargoUnloadAt(unload_pos,
		                                         used_vision,
		                                         check_move_points)
		){
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::ExecuteUnloadOrder
//
// Description: Try to unload some troops at order->m_point
//
// Parameters : Order *order
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::ExecuteUnloadOrder(Order *order)
{
	MapPoint    from_pt = m_pos;
	MapPoint    to_pt   = order->m_point;

	if(from_pt.x == to_pt.x
	&& from_pt.y == to_pt.y
	){
		// do nothing here, we're on top of the unloading point
	}
	else
	{
		if(!from_pt.IsNextTo(to_pt))
		{
			return true;
		}
	}

	if(DoLeaveOurLandsCheck(to_pt, UNIT_ORDER_UNLOAD))
	{
		return true;
	}

	//if we already have a max stack at to_pt
	sint32 max_debark = k_MAX_ARMY_SIZE;
	CellUnitList *a = g_theWorld->GetArmyPtr(to_pt);
	if(a && (0 < a->Num()))
	{
		if(m_owner == a->GetOwner())
		{
			max_debark = k_MAX_ARMY_SIZE - a->Num();
			if(max_debark == 0)
			{
				return true; //no space to land troops
			}
		}
	}

	Army debark;

	bool unitUnloadDone = false;
	sint32 count = 0; // Count the units to debark between the transporters so that we do not try to debark more units than fit at the unload position.
	for(sint32 i = 0; i < m_nElements; i++)
	{
		if(order->m_order == UNIT_ORDER_UNLOAD
		|| order->m_order == UNIT_ORDER_MOVE_THEN_UNLOAD)
		{
			unitUnloadDone |= m_array[i].UnloadCargo(to_pt, debark, count);
		}
		else if(order->m_order == UNIT_ORDER_UNLOAD_SELECTED_STACK)
		{
			unitUnloadDone |= m_array[i].UnloadCargo(to_pt, debark, count, true);
		}
		else
		{
			Assert(false);
		}
	}

	if(debark.m_id != 0)
	{
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
		                       GEV_FinishUnload,
		                       GEA_Army, m_id,
		                       GEA_Army, debark,
		                       GEA_MapPoint, to_pt,
		                       GEA_End
		                      );
	}
	else
	{
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		if(visiblePlayer == m_array[0].GetOwner()
		|| (m_array[0].GetVisibility() & (1 << visiblePlayer)))
		{
			g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0,
								m_array[0].GetCantMoveSoundID(),
								to_pt.x,
								to_pt.y);
		}
	}

	return unitUnloadDone;
}

void ArmyData::FinishUnloadOrder(Army &debark, MapPoint &to_pt)
{
	if(debark.Num() <= 0)
	{
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		if ((visiblePlayer == m_array[0].GetOwner()) ||
			(m_array[0].GetVisibility() & (1 << visiblePlayer)))
		{
			g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0,
								m_array[0].GetCantMoveSoundID(),
								to_pt.x,
								to_pt.y);
		}

		debark.Kill();
	}
	else
	{
		sint32 i;
//		sint32 n = debark.Num();

		if(IsOccupiedByForeigner(to_pt))
		{
			for(i = 0; i < debark.Num(); i++)
			{
				if((!debark[i]->Flag(k_UDF_BEACH_ASSAULT_LEGAL)))
				{
					CheckWasEnemyVisible(to_pt);

					bool inserted = false;
					for(sint32 j = 0; j < m_nElements && !inserted; j++)
					{
						inserted = m_array[j].InsertCargo(debark[i]);
						if(inserted)
						{
							Unit u = debark[i];
							u.GetInserted(m_array[j]);
							break;
						}
					}

					Assert(inserted);
					debark.DelIndex(i);
					i--;
				}
			}
		}
		else if(g_theWorld->GetCity(to_pt).m_id != 0 &&
		        g_theWorld->GetCity(to_pt).GetOwner() != m_owner)
		{
			if(!g_theArmyPool->AccessArmy(debark)->CanAtLeastOneCaptureCity())
			{
				for(i = debark.Num() - 1; i >= 0; i--) {
					bool inserted = false;
					for(sint32 j = 0; j < m_nElements && !inserted; j++)
					{
						inserted = m_array[j].InsertCargo(debark[i]);
						if(inserted)
						{
							debark[i].GetInserted(m_array[j]);
							break;
						}
					}

					Assert(inserted);
					debark.DelIndex(i);
				}
			}
		}

		if(debark.Num() > 0)
		{
			if (g_soundManager)
			{
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if ((visiblePlayer == debark[0].GetOwner()) ||
				    (debark[0].GetVisibility() & (1 << visiblePlayer)))
				{
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0,
									    m_array[0].GetUnloadSoundID(),
									    to_pt.x,
									    to_pt.y);
				}
			}

			if (debark.IsValid())
			{
				for(sint32 i = 0; i < debark.Num(); i++)
				{
					if(!debark[i]->Flag(k_UDF_BEACH_ASSAULT_LEGAL) &&
					   !debark[i].GetMovementTypeAir())
					{
						g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_SetUnloadMovementUnit,
											   GEA_Unit, debark[i].m_id,
											   GEA_End);
					}
				}
			}

			if(m_pos != to_pt)
			{
				for(sint32 i = 0; i < debark.Num(); i++)
				{
					if (debark[i].GetActor())
					{
						debark[i].GetActor()->Hide();
						debark[i].GetActor()->PositionActor(m_pos);
						g_director->AddShow(debark[i]);
					}
				}
				debark.AutoAddOrders(UNIT_ORDER_MOVE_TO, NULL, to_pt, 0);
			}
			else
			{
				for(sint32 i = 0; i < debark.Num(); i++)
				{
					g_director->AddShow(debark[i]);
				}
				debark.AutoAddOrders(UNIT_ORDER_TELEPORT_TO, NULL, m_pos, 0);
			}
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::DeductMoveCost
//
// Description: Deduct the cost of moving into pos from each of this army's units
//
// Parameters : MapPoint &pos
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : - Attempted MoveBonus here, but it appears this is only for terrain
//                Unit data is for Units
//              - Added Denied to enemy check 4-11-2006
//
//----------------------------------------------------------------------------
void ArmyData::DeductMoveCost(const MapPoint &pos)
{
	sint32 i;
	double cost = g_theWorld->GetMoveCost(pos);   //GetEnvBase()->GetMovement() non imps for Denyenemy
	sint32  movebonus; // = g_theUnitDB->Get()->GetMoveBonus();
	double c;
	// EMOD
	Cell *cell = g_theWorld->GetCell(pos);
	sint32 CellOwner = cell->GetOwner();
	sint32 j = 0;
	sint32 imp = cell->GetDBImprovement(j);

	// End EMOD
	for(i = m_nElements - 1; i >= 0; i--)
	{
		if(m_array[i].GetMovementTypeAir())
		{
			c = k_MOVE_AIR_COST;
		// EMOD - this code may no longer be necessay since I
		// think this code only gets the cost of the pos and
		// unitdata is used for the unit deduct cost
		}
		else if(m_array[i].GetDBRec()->GetMoveBonus(movebonus))
		{
			c = movebonus;
		//end EMOD
		}
		else if(g_theWorld->IsTunnel(pos))
		{
			if(!m_array[i].GetMovementTypeLand())
			{
				sint32 icost;
				g_theWorld->GetTerrain(pos)->GetEnvBase()->GetMovement(icost);
				c = icost;
			} else {
				c = cost;
			}
		}
		else if(m_array[i].Flag(k_UDF_FOUGHT_THIS_TURN))
		{  // Add Blitz/multiple attacks here?
			if(!m_array[i].GetDBRec()->GetMultipleAttacks())
			{
				c = m_array[i].GetMovementPoints();
			}
			else
			{
				c = cost;
			}
		}
		// EMOD for having some tileimps not allowed for enemy
		// movement like railroads 4-12-2006
		else if(m_array[i].GetOwner() != CellOwner
		     && CellOwner > 0
		     && cell->GetDBImprovement(j) > 0
		){  //this denies all?
			const TerrainImprovementRecord *trec = g_theTerrainImprovementDB->Get(imp);
			if(g_player[m_owner]->HasWarWith(CellOwner)
			&& trec->GetDeniedToEnemy())
			{  //i.e. RailRoads see Cell::CalcTerrainMoveCost?
				sint32 enemycost;
				g_theWorld->GetTerrain(pos)->GetEnvBase()->GetMovement(enemycost);
				c = enemycost;
			}
			else
			{
				c = cost;
			}
			//end EMOD
		}
		else
		{
			c = cost;
		}
//end EMOD
//original
//			c = m_array[i].GetMovementPoints();
//		}else {
//			c = cost;
//		}

		bool out_of_fuel = false;
		m_array[i].DeductMoveCost(c, out_of_fuel);
	}
}

sint32 ArmyData::Fight(CellUnitList &defender)
{
	Assert(defender.Num() > 0);
	Assert(m_nElements > 0);
	if(defender.Num() < 1 || m_nElements < 1)
	{
		return false;
	}
	m_dontKillCount++;

	sint32 i;

	for(i = 0; i < m_nElements; i++)
	{
		//EMOD add multiple attack flag here? 4-25-2006
		if(!m_array[i].GetDBRec()->GetMultipleAttacks())
		{
			m_array[i].SetFlag(k_UDF_FOUGHT_THIS_TURN);
		}
	}

	for(i = 0; i < defender.Num(); i++)
	{
		defender[i].SetFlag(k_UDF_FOUGHT_THIS_TURN);
	}
//	sint32 n_start_attackers = m_nElements;

	PLAYER_INDEX attack_owner, defense_owner;
	attack_owner = GetOwner();
	defense_owner = defender.GetOwner();
	MapPoint attack_pos, defense_pos;
	GetPos(attack_pos);
	defender.GetPos(defense_pos);

	ForceVisibleThisTurn(defense_owner);
	defender.ForceVisibleThisTurn(attack_owner);

	Unit ta = GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());

	if(ta.m_id == 0)
	{
		ta = m_array[0];
	}

	Unit td = defender.GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());

	if(td.m_id == 0)
	{
		td = defender[0];
	}

	if((ta.m_id != 0) && (td.m_id != 0))
	{
		if(ta.GetActor())
		{
			UnitActor *actor = ta.GetActor();

			if(!actor->HasAnim(UNITACTION_ATTACK))
			{
				for(i = 0; i < m_nElements; i++)
				{
					if(m_array[i].GetActor() &&
					   m_array[i].GetActor()->HasAnim(UNITACTION_ATTACK)) {
						ta = m_array[i];
						break;
					}
				}
			}
		}
		g_slicEngine->RunAttackTriggers(ta, td);
	}
	else
	{
		if(ta.IsValid() && defender[0].IsValid())
		{
			g_director->AddAttackPos(ta, defender[0].RetPos());
		}
	}

	if (ta.m_id != 0)
		ta.SetFlag(k_UDF_WAS_TOP_UNIT_BEFORE_BATTLE);

	if (td.m_id != 0)
		td.SetFlag(k_UDF_WAS_TOP_UNIT_BEFORE_BATTLE);

	MapPoint pos;

	g_player[attack_owner]->MakeConvertedCitiesUnhappy(defense_owner);

	defender.GetPos(pos);

	if(g_selected_item->IsAutoCenterOn()
	&& defender.GetOwner() == g_selected_item->GetVisiblePlayer())
		g_director->AddCenterMap(pos);

	Unit c = g_theWorld->GetCity(pos);

	if( (g_turn->IsHotSeat()
	||   g_turn->IsEmail()
	||   g_theProfileDB->GetUseAttackMessages()
	    )
	&&   g_player[defender.GetOwner()]->IsHuman()
	){
		SlicObject *so = NULL;
		if (c.IsValid())
		{
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

	if (c.m_id != (0))
	{
		defenders_bonus = c.GetDefendersBonus();
	}

//	double amr = 1.0 / GetHPModifier();
//	double dmr = 1.0 / defender.GetHPModifier();

	// EMOD Sneak Attack now works...1-28-2006

	bool AlltaSneakAttack = true;
	if(ta.IsValid()) {
		for (i = m_nElements - 1; i>= 0; i--) {   //for(i = 0; i < m_nElements; i++) {
			if(!m_array[i].GetDBRec()->GetSneakAttack()){
				AlltaSneakAttack = false;
				break;
			}
		}
	}

	if (ta.m_id == 0) {
		if(!m_array[0].GetDBRec()->GetSneakAttack()){
				AlltaSneakAttack = false;
		}
	}

	bool AllDefSneakAttack = true;
	for(i = 0; i < defender.Num(); i++) {
		if(!defender[i].GetDBRec()->GetSneakAttack()){
			AllDefSneakAttack = false;
			break;
		}
	}

	if (td.m_id == 0) {
		if(!defender[0].GetDBRec()->GetSneakAttack()){
			AllDefSneakAttack = false;
		}
	}

	if(!AlltaSneakAttack && !AllDefSneakAttack){
		Diplomat & defending_diplomat = Diplomat::GetDiplomat(defense_owner);
		defending_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_CEASEFIRE);
	}

	// end EMOD

	bool defenderSucks = true;
	for(i = 0; i < defender.Num(); i++) {
		if(defender[i].GetAttack() > 0
		|| defender[i].GetDefense() > 0) {
			defenderSucks = false;
			break;
		}
	}

	if(defenderSucks) {
		if(ta.IsValid()) {
			g_director->AddAttack(ta, td);
			for(i = 0; i < defender.Num(); i++) { // insert unit kill event such that it is exectued after BattleAftermath event
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
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, // BattleAftermath event has to be added after unit kill to be executed before
								   GEV_BattleAftermath,
								   GEA_Army, m_id,
								   GEA_MapPoint, pos,
								   GEA_Unit, ta,
								   GEA_Unit, td,
								   GEA_Player, m_owner,
								   GEA_Player, defense_owner,
								   GEA_Int, 0,
								   GEA_End);
		}
	}
	else
	{
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
			return false;
		}
	}

	return false;
}

void ArmyData::UpdateZOCForRemoval()
{
	CellUnitList *units = g_theWorld->GetCell(m_pos)->UnitArmy();

	if(units && units->GetOwner() == m_owner) {
		for (sint32 i = 0; i < units->Num(); i++) {
			if(units->Access(i).GetArmy().m_id != m_id &&
			   !units->Access(i).IsNoZoc()) {
				return;
			}
		}
	}

	g_theWorld->RemoveZOC(m_pos, m_owner);
	g_theWorld->AddOtherArmyZOC(m_pos, m_owner, Army(m_id), Unit());

	for (sint32 dd = 0; dd < (sint32)NOWHERE; dd++) {
		MapPoint npos;
		if(m_pos.GetNeighborPosition((WORLD_DIRECTION)dd, npos)) {
			g_theWorld->RemoveZOC(npos, m_owner);
			g_theWorld->AddOtherArmyZOC(npos, m_owner, Army(m_id), Unit());
		}
	}
}

bool ArmyData::ExecuteTeleportOrder(Order *order)
{
	UpdateZOCForRemoval();

	MoveUnits(order->m_point);

	for (sint32 i = 0; i < m_nElements; i++) {
		m_array[i].UpdateZOCForInsertion();
	}

	MoveActors(order->m_point, true);
	m_pos = order->m_point;

	CheckTerrainEvents();


	return true;
}

void ArmyData::ResetPos()
{
	if(m_nElements <= 0)
		return;
	m_array[0].GetPos(m_pos);
#ifdef _DEBUG
	for (sint32 i = 1; i < m_nElements; i++) {
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

		for (sint32 i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
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
	m_hasBeenAdded = true;
}

void ArmyData::AddDeath(const Unit &unit, CAUSE_REMOVE_ARMY cause,
						PLAYER_INDEX who)
{
	m_killMeSoon->AddTail(new KillRecord(unit, cause, who));
}

void ArmyData::SetUnloadMovementPoints()
{
	for (sint32 i = 0; i < m_nElements; i++) {
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

//Disband an army. Recover 1/2 it's ShieldCost if it's in a city.
//Gift in friendly territory until a new order is made
//out comment notes for possible future Great Leaders
void ArmyData::Disband()
{
	if (g_player[m_owner]->m_all_armies->Num() < 2 &&
	    g_player[m_owner]->m_all_cities->Num() < 1)
		return;

	Unit        city            = g_theWorld->GetCity(m_pos);
	Cell *      cell            = g_theWorld->GetCell(m_pos);
	sint32      cellOwner       = cell->GetOwner();
	Diplomat &  cell_diplomat   = Diplomat::GetDiplomat
	    ((PLAYER_UNASSIGNED == cellOwner) ? PLAYER_INDEX_VANDALS : cellOwner);

	// Usually, "for (int i = 0; i < n; ++i)" expresses more clearly that you
	// are going through all items of an array. But when killing/removing items,
	// it is safer to start from the end, otherwise indices may get shifted
	// while you are busy, and half of the items will not be removed.
	for(sint32 i = m_nElements - 1; i >= 0; i--)
	{
		if (city.IsValid()) {
			// Shield cost should be difficulty dependent
			city.GetCityData()->AddShields(m_array[i].GetDBRec()->GetShieldCost() / 2);
		}

		// Should be moved into own method and own event
		// EMOD Gift Units for Human Player 4-12-2006
		if (    (cellOwner != m_owner)
		     && (cellOwner != PLAYER_UNASSIGNED)
		     && !g_player[m_owner]->HasWarWith(cellOwner)
		     && (m_array[i].GetDBRec()->GetCanBeGifted())
		     && (!g_player[m_owner]->IsRobot())
		   )
		{
			sint32 newunit      = m_array[i].GetType();
			sint32 regardcost   = static_cast<sint32>((m_array[i].GetDBRec()->GetAttack()) / 5.0);

			m_array[i].Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
			/// @todo Check if this works when disbanding an army consisting
			///       of multiple units. Does the game engine allow creation of
			///       units of 2 players at the same location?
			///       Indeed this does support the game engine, you can have
			///       two unit at the same location and even group them into
			///       one army, but then you should not try to move it,
			///       otherwise the game will crash. Actually it would be
			///       cleaner to have the position empty before adding any
			///       foreign units.
			g_player[cellOwner]->CreateUnit
			    (newunit, m_pos, Unit(), false, CAUSE_NEW_ARMY_INITIAL);

			StringId strId;
			g_theStringDB->GetStringID("REGARD_EVENT_UNITS_GIFTED", strId);
			cell_diplomat.LogRegardEvent
			    (m_owner, regardcost, REGARD_EVENT_GOLD, strId);
		}
		else
		{
			m_array[i].Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::GetMinFuel
//
// Description: Returns the lowest fuel level of all the planes in the CellUnitList,
//              or 0x7fffffff if there are none.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : sint32
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 ArmyData::GetMinFuel()
{
    sint32 minFuel = 0x7fffffff;

    for (sint32 i = 0; i < m_nElements; i++ )
    {
        if (m_array[i].GetDBRec()->GetNoFuelThenCrash())
        {
            minFuel = std::min<sint32>(minFuel, m_array[i].GetFuel());
        }
    }

    return minFuel;
}

//-------------------------------------------------------------------------------------------
//
// Name       : ArmyData::CalcRemainingFuel
//
// Description: fills in the following parameters:
//
// Parameters : num_tiles_to_half   :the distance (in 100's) that the lowest fueled plane can move
//                                   before reaching half it's fuel level,
//
//              num_tiles_to_empty  :the distance (in 100's) that the lowest fueled plane can move
//                                   before running out of fuel.
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//--------------------------------------------------------------------------------------------
void ArmyData::CalcRemainingFuel(sint32 &num_tiles_to_half, sint32 &num_tiles_to_empty) const
{

	num_tiles_to_half = 1000000;
	num_tiles_to_empty = 1000000;

	sint32 fuel_remaining;
	sint32 max_fuel;
	sint32 fuel_to_half;
	for (sint32 i = 0; i < m_nElements; i++ )
	{
		// if the unit is a plane, get how much fuel it has left and (from unitDB) it's max_fuel
		if (!m_array[i].GetUsedFuel (fuel_remaining, max_fuel)) continue;

		fuel_to_half = fuel_remaining - max_fuel / 2;

		if (fuel_to_half < num_tiles_to_half)
		{
			num_tiles_to_half = std::max<sint32>(fuel_to_half, 0);
		}

		if (fuel_remaining < num_tiles_to_empty)
		{
			num_tiles_to_empty = fuel_remaining;
		}
	}
}

bool ArmyData::CanMove()
{
	bool noneMoved = true;
	bool allOverSpecialCost = true;

	for(sint32 i = 0; i < m_nElements; i++)
	{
		if(!m_array[i].GetFirstMoveThisTurn())
			noneMoved = false;
		if(!m_array[i].CanPerformSpecialAction())
			allOverSpecialCost = false;
	}

	if(noneMoved)
	{
		return true;
	}

	if(allOverSpecialCost)
	{
		return true;
	}

	MapPoint neighbor;
	for(sint32 d = 0; d < (sint32)NOWHERE; d++)
	{
		if(m_pos.GetNeighborPosition((WORLD_DIRECTION)d, neighbor))
		{
			if(IsMovePointsEnough(neighbor))
			{
				bool zocViolation;
				bool alliedCity;
				if(CanMoveIntoCell(neighbor,
								   zocViolation,
								   false,
								   alliedCity))
					return true;
				else if(IsOccupiedByForeigner(neighbor))
					return true;
			}
		}
	}
	return false;
}

void ArmyData::ActionSuccessful(SPECATTACK attack, Unit &unit, Unit const & c)
{
	SpecialAttackInfoRecord const * rec = unitutil_GetSpecialAttack(attack);
	sint32  soundID  = rec ? rec->GetSoundIDIndex() : -1;
	sint32  spriteID = rec ? rec->GetSpriteID()->GetValue() : -1;

	if(spriteID != -1 && soundID != -1)
	{
		if(g_selected_item->IsAutoCenterOn())
		{
			if(
			     (
			       (    m_owner == g_selected_item->GetVisiblePlayer()
			         || (unit.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()))
			       )
			    || c.IsValid()
			    && (    c.GetOwner() == g_selected_item->GetVisiblePlayer()
			         || (c.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()))
			       )
			     )
			){
				g_director->AddCenterMap(m_pos);
			}
		}

		g_director->AddSpecialAttack(unit, c, attack);
	}
	else
	{
		if(soundID != -1)
		{
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();

			if(visiblePlayer == m_owner
			|| unit.GetVisibility() & (1 << visiblePlayer))
			{
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

bool ArmyData::CanSomeCargoBeachAssault() const
{
	for(sint32 i = 0; i < m_nElements; i++)
	{
		const UnitDynamicArray * cargo =
			m_array[i].AccessData()->GetCargoList();

		if(cargo)
		{
			for(sint32 j = 0; j < cargo->Num(); j++)
			{
				if(cargo->Access(j).CanBeachAssault())
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool ArmyData::CanBeachAssault() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(!m_array[i].CanBeachAssault())
			return false;
	}
	return true;
}

bool ArmyData::CanHearGossip() const
{
	for (sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetHearGossip())
			return true;
	}
	return false;
}

bool ArmyData::CanSlaveUprising() const
{
	for (sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->HasSlaveUprising()
		&& m_array[i].CanPerformSpecialAction())
			return true;
	}
	return false;
}

bool ArmyData::CanInciteRevolution( double &chance, double &eliteChance ) const
{
	const UnitRecord::InciteRevolutionData *data;
	for (sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetInciteRevolution(data)
		&& m_array[i].CanPerformSpecialAction()
		){
			chance = data->GetChance();
			eliteChance = data->GetEliteChance();
			return true;
		}
	}
	return false;
}

bool ArmyData::CanCloak() const
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(!m_array[i].GetDBRec()->GetCanCloak()
		&& !m_array[i].CanPerformSpecialAction())
			return false;
	}
	return true;
}

bool ArmyData::CanCreateFranchise( double &chance ) const
{
	const UnitRecord::ChanceEffect *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetCreateFranchise(data)
		&& m_array[i].CanPerformSpecialAction()
		){
			chance = data->GetChance();
			return true;
		}
	}
	return false;
}

bool ArmyData::CanAssasinateRuler( double &chance, double &eliteChance) const
{
	const UnitRecord::AssasinateRulerData *data;

	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetAssasinateRuler(data)
		&& m_array[i].CanPerformSpecialAction()
		){
			chance = data->GetChance();
			eliteChance = data->GetEliteChance();
			return true;
		}
	}
	return false;
}

bool ArmyData::CanStealTechnology( double &randChance, double &chance) const
{
	const UnitRecord::StealTechnologyData *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetStealTechnology(data)
		&& m_array[i].CanPerformSpecialAction()
		){
			randChance = data->GetRandomChance();
			chance = data->GetSpecificChance();
			return true;
		}
	}
	return false;
}

bool ArmyData::CanInvestigateCity( double &chance, double &eliteChance) const
{
	const UnitRecord::InvestigateCityData *data;
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i].GetDBRec()->GetInvestigateCity(data)
		&& m_array[i].CanPerformSpecialAction()
		){
			chance = data->GetChance();
			eliteChance = data->GetEliteChance();
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ExecuteSpecialOrder
//
// Description: Try to execute a 'special' order (see the switch below for a list of them).
//
// Parameters : Order *order       :
//              bool &keepGoing    :
//
// Globals    :
//
// Returns    : bool
//
// Remark(s)  : called by ExecuteOrders, this is effectively a sub-switch.
//
//----------------------------------------------------------------------------
bool ArmyData::ExecuteSpecialOrder(Order *order, bool &keepGoing)
{
	keepGoing = true;

	Assert(order->m_order < UNIT_ORDER_MAX);
	Assert(order->m_order >= (UNIT_ORDER_TYPE) 0);
	if(order->m_order < (UNIT_ORDER_TYPE)0 || order->m_order >= UNIT_ORDER_MAX)
		return true;

	if(!CanPerformSpecialAction())
	{
		keepGoing = false;
		return false;
	}

	if (s_orderDBToEventMap == NULL)
		AssociateEventsWithOrdersDB();
	Assert(s_orderDBToEventMap);

	GAME_EVENT gev = Order::OrderToEvent(order->m_order);
	sint32 odb;
	for(odb = 0; odb < g_theOrderDB->NumRecords(); odb++)
	{
		if(s_orderDBToEventMap[odb] == gev)
		{
			break;
		}
	}
	Assert(odb < g_theOrderDB->NumRecords());
	if(odb >= g_theOrderDB->NumRecords())
		odb = 0;

	const OrderRecord *order_rec = g_theOrderDB->Get(odb);
	// If not enough gold, just do failure special effects and return
	if(order_rec && order_rec->GetGold() > 0)
	{
		if(g_player[m_owner]->m_gold->GetLevel() < order_rec->GetGold())
		{
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if
			  (
			       visiblePlayer == m_owner
			    || (m_array[0].GetVisibility() & (1 << visiblePlayer))
			  )
			{
				sint32	spriteID = g_theSpecialEffectDB->Get(g_theSpecialEffectDB->FindTypeIndex("SPECEFFECT_GENERAL_CANT"))->GetValue();
				sint32	soundID  = gamesounds_GetGameSoundID(GAMESOUNDS_TOOEXPENSIVE);

				g_director->AddSpecialEffect(order->m_point, spriteID, soundID);
			}
			return true;
		}
	}

	bool useDefaultSuccessSound = false;

	uint32 origVisibility = 0;
	for(sint32 i = 0; i < m_nElements; i++)
	{
		origVisibility |= m_array[i].GetVisibility();
	}

	ORDER_RESULT result;
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
			useDefaultSuccessSound = true;
			break;
		case UNIT_ORDER_FRANCHISE:
			result = Franchise(order->m_point);
			break;
		case UNIT_ORDER_SUE_FRANCHISE:
			result = SueFranchise(order->m_point);
			useDefaultSuccessSound = true;
			break;
		case UNIT_ORDER_EXPEL:
			result = Expel(order->m_point);
			useDefaultSuccessSound = true;
			break;
		case UNIT_ORDER_ESTABLISH_EMBASSY:
			result = EstablishEmbassy(order->m_point);
			break;
		case UNIT_ORDER_THROW_PARTY:
			result = ThrowParty(order->m_point);
			break;
		case UNIT_ORDER_CAUSE_UNHAPPINESS:
			Assert(false);
			result = ORDER_RESULT_ILLEGAL;
			break;
		case UNIT_ORDER_PLANT_NUKE:
			result = PlantNuke(order->m_point);
			break;
		case UNIT_ORDER_SLAVE_RAID:
			result = SlaveRaid(order->m_point);
			break;
		case UNIT_ORDER_ENSLAVE_SETTLER:
			result = EnslaveSettler(order->m_point, 0, Unit());
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
			result = Pillage(true);
			useDefaultSuccessSound = true;
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
			result = Pillage(false);
			useDefaultSuccessSound = true;
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
			Assert(false);
			break;
	}

#ifdef _DEBUG
		if (g_theDiplomacyLog)
		{
			g_theDiplomacyLog->PopRegardRequest();
		}
#endif _DEBUG

	if(result == ORDER_RESULT_ILLEGAL)
	{
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		if ((visiblePlayer == m_owner) ||
			(m_array[0].GetVisibility() & (1 << visiblePlayer))) {

			sint32	spriteID = g_theSpecialEffectDB->Get(g_theSpecialEffectDB->FindTypeIndex("SPECEFFECT_GENERAL_CANT"))->GetValue();
			sint32	soundID = gamesounds_GetGameSoundID(GAMESOUNDS_ILLEGAL_SPECIAL);

			g_director->AddSpecialEffect(order->m_point, spriteID, soundID);
		}

		for(sint32 i = m_nElements - 1; i >= 0; i--)
		{
			Assert(!m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW));
			m_array[i].ClearFlag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW);
		}
		return true;
	}

	bool deduct = false;

	switch(result)
	{
		case ORDER_RESULT_FAILED:
		{
			deduct = true;

			sint32	spriteID = g_theSpecialEffectDB->Get(g_theSpecialEffectDB->FindTypeIndex("SPECEFFECT_GENERAL_FAIL"))->GetValue();
			sint32 soundID = order_rec->GetFailSoundIndex();
			if(soundID < 0) {
				soundID = gamesounds_GetGameSoundID(GAMESOUNDS_GENERALFAIL);
			}

			if(g_selected_item->IsAutoCenterOn()
			&&!g_director->TileWillBeCompletelyVisible(order->m_point.x, order->m_point.y)
			&& g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(order->m_point)
			){
				g_director->AddCenterMap(m_pos);
			}

			g_director->AddSpecialEffect(order->m_point, spriteID, soundID);
		}
			break;
		case ORDER_RESULT_SUCCEEDED:
		case ORDER_RESULT_SUCCEEDED_INCOMPLETE:
			deduct = true;

			if (useDefaultSuccessSound)
			{
				if(g_selected_item->IsAutoCenterOn()
				&&!g_director->TileWillBeCompletelyVisible(order->m_point.x, order->m_point.y)
				&& g_player[g_selected_item->GetVisiblePlayer()]->m_vision->IsVisible(order->m_point)
				){
					g_director->AddCenterMap(m_pos);
				}

				sint32	spriteID = g_theSpecialEffectDB->Get(g_theSpecialEffectDB->FindTypeIndex("SPECEFFECT_GENERAL_SUCCESS"))->GetValue();
				sint32 soundID = order_rec->GetSoundIndex();
				if(soundID < 0) {
					soundID = gamesounds_GetGameSoundID(GAMESOUNDS_GENERALSUCCEED);
				}

				if(g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(order->m_point))
				{
					g_director->AddSpecialEffect(order->m_point, spriteID, soundID);
				}
			}
			else
			{
			}
			break;
		case ORDER_RESULT_INCOMPLETE:
			deduct = true;
			break;
	}

	if(result == ORDER_RESULT_SUCCEEDED
	|| result == ORDER_RESULT_SUCCEEDED_INCOMPLETE
	){
		char const * sText = NULL;
		switch(order->m_order)
		{
			case UNIT_ORDER_STEAL_TECHNOLOGY:  sText = "186StealTechnologyVictim"; break;
			case UNIT_ORDER_ASSASSINATE:       sText = "176AssassinationCompleteVictim"; break;
			case UNIT_ORDER_INDULGENCE:        sText = "155IndulgenceCompleteVictim"; break;
			case UNIT_ORDER_INJOIN:            sText = "159InjunctionCompleteVictim"; break;
		}

		if (sText)
		{
			Unit c = GetAdjacentCity(order->m_point);
			if (c.IsValid())
			{
				g_slicEngine->Execute(new CityReport(sText, c));
			}
		}
	}

	if(deduct)
	{
		if(order_rec)
		{
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
			                       GEV_SubGold,
			                       GEA_Player, m_owner,
			                       GEA_Int, order_rec->GetGold(),
			                       GEA_End
			                      );
		}

		for (sint32 i = m_nElements - 1; i >= 0; i--)
		{
			if(m_array[i].Flag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW))
			{
				m_array[i].ClearFlag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW);
				if(order_rec)
				{
					bool out_of_fuel;
					m_array[i].DeductMoveCost(order_rec->GetMove(), out_of_fuel);
				}
				m_array[i].ClearFlag(k_UDF_FIRST_MOVE);
			}
		}
	}

	return (result != ORDER_RESULT_SUCCEEDED_INCOMPLETE);
}

void ArmyData::AddSpecialActionUsed(Unit &who)
{
	who.SetFlag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN);
	who.SetFlag(k_UDF_USED_SPECIAL_ACTION_JUST_NOW);
}

void ArmyData::SetTurnOver()
{
	for (sint32 i = 0; i < m_nElements; i++) {
		m_array[i].SetFlag(k_UDF_TURN_OVER);
	}
}

bool ArmyData::TurnOver()
{
	for (sint32 i = 0; i < m_nElements; i++) {
		if(!m_array[i].Flag(k_UDF_TURN_OVER))
			return false;
	}
	return true;
}

void ArmyData::GetCurrentHP
(
	sint32 &	count,
	sint32		unit_type[MAX_UNIT_COUNT],
	sint32		unit_hp[MAX_UNIT_COUNT]
) const
{
	count = Num();

	for(sint32 i = 0; i < count; ++i)
	{
		Assert(i < MAX_UNIT_COUNT);

		unit_type[i]    = m_array[i].GetType();
		unit_hp  [i]    = std::max<sint32>(static_cast<sint32>(m_array[i].GetHP()), 0);
	}
}

bool ArmyData::IsWounded() const
{
	sint32 totalCurrentHP = 0;
	sint32 totalHP        = 0;

	for(sint32 i = 0 ; i < m_nElements ; i ++)
	{
		totalCurrentHP += std::max<sint32>(static_cast<sint32>(m_array[i].GetHP()), 0);
		totalHP += m_array[i]->CalculateTotalHP();
	}

	return (totalCurrentHP < totalHP/2);
	//criterion can be changed, but seems relevant. Even if support isn't
	//determined as full, it will be wise not to attack with a unit that has
	//half than its normal HPs.
}

bool ArmyData::IsCargoWounded() const
{
	sint32 totalCurrentHP = 0;
	sint32 totalHP        = 0;

	for(sint32 i = 0 ; i < m_nElements ; i++)
	{
		UnitDynamicArray* cargoList = m_array[i]->GetCargoList();
		sint32 n = cargoList != NULL ? cargoList->Num() : 0;

		for(sint32 j = 0; j < n; j++)
		{
			totalCurrentHP += std::max<sint32>(static_cast<sint32>((*cargoList)[j].GetHP()), 0);
			totalHP        += (*cargoList)[j]->CalculateTotalHP();
		}
	}

	return (totalCurrentHP < totalHP/2);
	//criterion can be changed, but seems relevant. Even if support isn't
	//determined as full, it will be wise not to attack with a unit that has
	//half than its normal HPs.
}

bool ArmyData::CheckWasEnemyVisible(const MapPoint &pos, bool justCheck)
{
	Cell *cell = g_theWorld->GetCell(pos);

	if (cell->GetCity().IsValid())
	{
		bool visible = (cell->GetCity().GetVisibility() & (1 << m_owner)) != 0;
		if(!visible && !justCheck)
		{
			cell->GetCity().SetVisible(m_owner);
		}

		return visible;
	}

	bool visible = false;

	CellUnitList * defender = cell->UnitArmy();
	if (defender)
	{
		sint32 i;
		for (i = 0; i < defender->Num(); i++)
		{
			if (defender->Access(i).GetVisibility() & (1 << m_owner))
			{
				visible = true;
				break;
			}
		}

		if (!justCheck)
		{
			for (i = 0; i < defender->Num(); i++)
			{
				defender->Access(i).SetVisible(m_owner);
			}
		}
	}

	if (!justCheck && (m_owner == g_selected_item->GetVisiblePlayer()))
	{
		g_selected_item->ForceDirectorSelect(Army(m_id));
	}

	return visible;
}

bool ArmyData::GetInciteRevolutionCost( const MapPoint &point, sint32 &attackCost )
{
	Unit c = g_theWorld->GetCity(point);
	if (!c.IsValid())
		return false;

	Player * p = g_player[c.GetOwner()];
	if(!p)
		return false;

	double      distcost = g_theConstDB->Get(0)->GetInciteRevolutionCapitolPenalty();
	MapPoint    start;
	if (p->GetCapitolPos(start) && p->GetMaxEmpireDistance())
	{
		MapPoint    dest;
		c.GetPos(dest);
		double const distanceFromCapitol =
			sqrt(static_cast<double>(MapPoint::GetSquaredDistance(start, dest)));
		distcost *= (1.0 - (distanceFromCapitol / p->GetMaxEmpireDistance()));
	}

	distcost = std::max<double>(1.0, distcost);

	double capitolPenalty = c.IsCapitol() ? g_theConstDB->Get(0)->GetInciteRevolutionCapitolPenalty() : 0.0;
	double goldcost = p->m_gold->GetLevel();
	double popcost = c.PopCount() * g_theConstDB->Get(0)->GetInciteRevolutionGoldCoefficient();

	attackCost = static_cast<sint32>(goldcost + popcost + distcost + capitolPenalty);

	return true;
}

bool ArmyData::GetInciteUprisingCost( const MapPoint &point, sint32 &attackCost )
{
	Unit c = g_theWorld->GetCity(point);

	if (!c.IsValid())
		return false;

	if(c.CountSlaves() <= 0) {//not really relevent anymore
		return false;
	}

	MapPoint start, dest;
	PLAYER_INDEX city_owner = c.GetOwner();
	Player *p = g_player[city_owner];
	double distcost;

	if(p->GetCapitolPos(start)) {
		c.GetPos(dest);

		double const distanceFromCapitol =
			sqrt(static_cast<double>(MapPoint::GetSquaredDistance(start, dest)));
		distcost = 100.0 * distanceFromCapitol;

	} else {
		distcost = p->GetMaxEmpireDistance();
	}

	distcost += 100.0;

	if(distcost < 1.0)
		distcost = 1.0;

	sint32 capitolPenalty = 0;
	if(c.IsCapitol()) {
		capitolPenalty = sint32(g_theConstDB->Get(0)->GetInciteUprisingCapitolPenalty());
	}
	double cost = (g_player[c.GetOwner()]->m_gold->GetLevel() + 5000) *
		static_cast<double>(c.PopCount()) * (1.0 / distcost) *
		 g_theConstDB->Get(0)->GetInciteUprisingGoldCoefficient() +
		 capitolPenalty;

	attackCost = static_cast<sint32>(cost);

	return true;
}

////Possibly left over from CTP1, but now used in CTP2
bool ArmyData::DoLeaveOurLandsCheck(const MapPoint &newPos,
									UNIT_ORDER_TYPE order_type)
{

	Cell *cell = g_theWorld->GetCell(newPos);

	if(cell->GetOwner() >= 0 && cell->GetOwner() != m_owner &&
	   cell->GetOwner() != g_theWorld->GetCell(m_pos)->GetOwner()) {


		bool atLeastOneNonSpecialUnit = false;
		sint32 i;
		for(i = 0; i < m_nElements; i++) {
			if(m_array[i].GetAttack() > 0 ||
			   m_array[i].GetDBRec()->GetVisionClassStandard()) {
				atLeastOneNonSpecialUnit = true;
			}
		}
		if(atLeastOneNonSpecialUnit) {
		        //// similar to ArmyData::InterceptTrade()
			if(AgreementMatrix::s_agreements.HasAgreement(cell->GetOwner(), m_owner, PROPOSAL_REQUEST_WITHDRAW_TROOPS)){
				if(!g_player[m_owner]->IsRobot()
				|| (g_network.IsClient()
				&&  g_network.IsLocalPlayer(m_owner))
				){
					SlicObject *so = new SlicObject("13IAEnteringLands");
					so->AddCivilisation(m_owner);
					so->AddCivilisation(cell->GetOwner());
					so->AddLocation(newPos);
					so->AddOrder(order_type);
					so->AddRecipient(m_owner);
					so->AddUnit(m_array[0]);
					g_selected_item->ForceDirectorSelect(Army(m_id));
					g_slicEngine->Execute(so);
					return true;
				}
				else
				{
					Diplomat::GetDiplomat(cell->GetOwner()).LogViolationEvent(m_owner, PROPOSAL_REQUEST_WITHDRAW_TROOPS);
				}
			}
		}
	}
	return false;
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

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CharacterizeArmy
//
// Description: Get data for army and fill in given parameters.
//
// Parameters : bool & isspecial     : true if each unit is either not standard vision or has no attack
//              bool & isstealth,    : true if all units are stealth
//              sint32 & maxattack   : attack value of strongest attacking unit
//              sint32 & maxdefense  : defense value of strongest defending unit
//              bool & cancapture    : true if some unit can capture cities
//              bool & haszoc        : true if some unit has a zone of control
//              bool & canbombard    : true if some unit can bombard
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ArmyData::CharacterizeArmy
(
    bool & isspecial,
    bool & isstealth,
    sint32 & maxattack,
    sint32 & maxdefense,
    bool & cancapture,
    bool & haszoc,
    bool & canbombard
) const
{
	isspecial  = false;
	isstealth  = true;
	maxattack  = 0;
	maxdefense = 0;
	cancapture = false;
	haszoc     = false;
	canbombard = false;

	for(sint32 i = 0; i < m_nElements; i++)
	{
		const UnitRecord *rec = m_array[i].GetDBRec();

		if(rec->HasNuclearAttack())
		{
			isspecial  = true;
			maxattack  = 0;
			maxdefense = 0;
			cancapture = false;
			haszoc     = false;
			canbombard = false;
			break;
		}

		isspecial  |= !rec->GetVisionClassStandard();
		isspecial  |= (rec->GetAttack() <= 0.0);
		isstealth  &=  rec->GetVisionClassStealth();
		cancapture |= !rec->GetCantCaptureCity();
		haszoc     |= !rec->GetNoZoc();
		canbombard |= (rec->GetCanBombard() != 0x0);

		maxattack   = std::max(maxattack,  static_cast<sint32>(rec->GetAttack()));
		maxdefense  = std::max(maxdefense, static_cast<sint32>(rec->GetDefense()));
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::IsStealth
//
// Description: True if all units are stealth
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::IsStealth() const
{
	for(sint32 i = 0; i < m_nElements; i++)
	{
		if (!m_array[i].GetDBRec()->GetVisionClassStealth())
			return false;
	}
	return true;
}

void ArmyData::CharacterizeCargo
(
    bool & isspecial,
    bool & isstealth,
    sint32 & maxattack,
    sint32 & maxdefense,
    bool & cancapture,
    bool & haszoc,
    bool & canbombard
) const
{
	isspecial  = false;
	isstealth  = true;
	maxattack  = 0;
	maxdefense = 0;
	cancapture = false;
	haszoc     = false;
	canbombard = false;

	for(sint32 i = 0; i < m_nElements; i++)
	{
		const UnitDynamicArray * cargo =
		    m_array[i]->GetCargoList();

		if(cargo != NULL)
		{
			for(sint32 j = 0; j < cargo->Num(); j++)
			{
				const UnitRecord *rec = cargo->Access(j)->GetDBRec();

				isspecial  |= !rec->GetVisionClassStandard();
				isspecial  |= (rec->GetAttack() <= 0.0);
				isstealth  &=  rec->GetVisionClassStealth();
				cancapture |= !rec->GetCantCaptureCity();
				haszoc     |= !rec->GetNoZoc();
				canbombard |= (rec->GetCanBombard() != 0x0);

				maxattack   = std::max(maxattack,  static_cast<sint32>(rec->GetAttack()));
				maxdefense  = std::max(maxdefense, static_cast<sint32>(rec->GetDefense()));
			}
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CanPerformSpecialAction
//
// Description: Returns true if each unit in this army can perform, but has not
//              yet performed, a special action this turn.
//
// Parameters : MapPoint &point
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : special actions are typically performed by IsSpecialForces units (plus pirating and bombarding)
//
//----------------------------------------------------------------------------
bool ArmyData::CanPerformSpecialAction() const
{
	for (sint32 i = 0; i < m_nElements; i++) {
		if(!m_array[i].CanPerformSpecialAction())
			return false;
	}
	return true;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CheckAddEventOrder
//
// Description:
//
//
// Parameters : -
//
// Globals    : g_network       : multiplayer manager
//              g_gevManager
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
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

// moves one direction (tile) down a path, or removes order if at end of path
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

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CheckValidDestination
//
// Description: Returns true if this army has a pathed move order whose end point is dest (?)
//
// Parameters : MapPoint &dest
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::CheckValidDestination(const MapPoint &dest) const
{
	Order *order = m_orders->GetHead();
	// not clear on what's going on in this part
	if(order && order->m_order == UNIT_ORDER_MOVE && order->m_path)
	{
		MapPoint pos;
		if(order->m_path->IsEnd())
		{
			pos = order->m_path->GetEnd();
		}
		else
		{
			order->m_path->GetCurrentPoint(pos);
		}

		if(pos != m_pos)
		{
			order->m_path->RestoreIndexAndCurrentPos(order->m_path->GetNextIndex());
		}
	}

	return (order) && (order->m_order == UNIT_ORDER_MOVE)
	               && (order->m_path)
	               && (order->m_path->GetEnd() == dest);
}

// returns true if this army's current order is UNIT_ORDER_MOVE and the army already is where it was ordered to go
bool ArmyData::AtEndOfPath() const
{
	Order *order = m_orders->GetHead();

	return (order) && (order->m_order == UNIT_ORDER_MOVE)
	               && (order->m_path)
	               && (order->m_path->GetEnd() == m_pos);
}

// Returns true if this army's current order is UNIT_ORDER_MOVE and it hasn't reached the end of its path.
// In this case fills in next_pos with the current point of the path.
bool ArmyData::GetNextPathPoint(MapPoint & next_pos) const
{
	Order *order = m_orders->GetHead();
	if ((order) &&
		(order->m_order == UNIT_ORDER_MOVE) &&
		(order->m_path) &&
		(!order->m_path->IsEndDir()))
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
// Parameters : order_rec   : the order to test
//
// Globals    : -
//
// Returns    : bool        : all units in the army are capable of
//                            performing the order.
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
		if (!g_player[m_owner]->CanUseNukes())
			return false;
	}

	for(sint32 i = 0; i < m_nElements; i++) {

		const UnitRecord *unit_rec = m_array[i].GetDBRec();

		if(order_rec->GetUnitPretest_NoFuelThenCrash()
		&& unit_rec->GetNoFuelThenCrash()
		&&(m_array[i].NeedsRefueling())
		){
			return true;
		}

		orderValid &= m_array[i].UnitValidForOrder(order_rec);
	}

	return orderValid;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::TestOrderAny
//
// Description: Test whether some unit is capable of performing an order.
//
// Parameters : order_rec   : the order to test
//
// Globals    : g_player    : player (capabilities)
//
// Returns    : bool        : at least one unit in the army is capable of
//                            performing the order.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::TestOrderAny(OrderRecord const * order_rec) const
{
	if (order_rec->GetUnitPretest_CanPlantNuke() &&
	    !g_player[m_owner]->CanUseNukes()
	   )
	{
		return false;
	}

	bool		orderValid	= false;

	for
	(
		sint32 i = 0;
		!orderValid && (i < m_nElements);
		++i
	)
	{
		orderValid = m_array[i].UnitValidForOrder(order_rec);
	}

	return orderValid;
}

bool ArmyData::TestCargoOrderAny(OrderRecord const * order_rec) const
{
	if (order_rec->GetUnitPretest_CanPlantNuke() &&
	    !g_player[m_owner]->CanUseNukes()
	   )
	{
		return false;
	}

	bool		orderValid	= false;

	for
	(
		sint32 i = 0;
		!orderValid && (i < m_nElements);
		++i
	)
	{
		if(m_array[i]->GetNumCarried() > 0)
		{
			for(sint32 j = 0; !orderValid && (j < m_array[i]->GetNumCarried()); ++j)
			{
				orderValid = m_array[i]->GetCargoList()->Get(j).UnitValidForOrder(order_rec);
			}
		}
	}

	return orderValid;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::TestOrderUnit
//
// Description: Test whether a unit is capable of performing an order.
//
// Parameters : order_rec   : the order to test
//              unit_index  : the unit to test
//
// Globals    : g_player	: player array [see Player::InitPlayer for initialized player data]
//
// Returns    : bool        : true if the unit is capable of
//                            performing the order.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool ArmyData::TestOrderUnit(const OrderRecord *order_rec, uint32 unit_index) const
{

	if((unit_index < 0) || (unit_index >= static_cast<uint32>(m_nElements))) // Should be fixed in CellUnitList
		return(false);

	return(m_array[unit_index].UnitValidForOrder(order_rec));
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::TestOrder
//
// Description: Test whether this army is capable of performing an order.
//
// Parameters : order_rec   : the order to test
//
// Globals    : -
//
// Returns    : ORDER_TEST  : legality result
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ORDER_TEST ArmyData::TestOrder(const OrderRecord * order_rec) const
{

	return TestOrderHere( order_rec, m_pos );
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::TestOrderHere
//
// Description: Test whether this army is capable of performing an order at a position.
//
// Parameters : order_rec   : the order to test
//              pos         : the position to test
//
// Globals    : g_player    : player array [see Player::InitPlayer for initialized player data]
//
// Returns    : ORDER_TEST  : legality result
//
// Remark(s)  : used mainly in ControlPanelWindow
//
//----------------------------------------------------------------------------
ORDER_TEST ArmyData::TestOrderHere(const OrderRecord * order_rec, const MapPoint & pos) const
{
	Assert(g_player[m_owner]);
	bool        can_afford      = (g_player[m_owner]->GetGold() >= order_rec->GetGold());

	// order_rec:: range = 0 (army must be on top of tile) or range = 1 (can execute order from adjacent tile)
	sint32      range           = 0;
	(void) order_rec->GetRange(range);

	bool        target_valid    = (order_rec->GetIsTeleport() && pos == m_pos)
	                              ? false
	                              : TargetValidForOrder(order_rec, pos);

	//true if order is to be executed at this army's m_pos
	bool        cur_pos_valid   = (range == 0) && TargetValidForOrder(order_rec, m_pos);

	if(order_rec->GetTargetPretestAttackPosition() && (target_valid || cur_pos_valid))
	{
		CellUnitList targetArmy;
		g_theWorld->GetArmy(pos,targetArmy);

		if(g_theWorld->GetCell(pos)->GetCity().IsValid()
		&& g_theWorld->GetCell(pos)->GetCity().GetOwner() != m_owner
		){
			// Nothing
		}
		else if(!targetArmy.Num() || targetArmy.GetOwner() == m_owner)
		{
			target_valid  = false;
			cur_pos_valid = false;
		}
	}

	if (order_rec->GetTargetPretestEnemySpecialUnit())
	{
		if (g_theWorld->GetOwner(pos) != m_owner)
		{
			target_valid  = false;
			cur_pos_valid = false;
		}
	}

	if (order_rec->GetTargetPretestTransport())
	{
		CellUnitList transports;
		cur_pos_valid = NumUnitsCanMoveIntoTransport(m_pos, transports) > 0;
		target_valid  = NumUnitsCanMoveIntoTransport(pos, transports) > 0;
	}
	else if ((range <= 0) && !CanEnter(pos))
	{
		target_valid = false;
	}

	ORDER_TEST  best_result     = ORDER_TEST_ILLEGAL;

	for (sint32 army_index = 0; army_index < m_nElements; ++army_index)
	{
		if (m_array[army_index].UnitValidForOrder(order_rec))
		{
			// Order is valid

			if (!target_valid && !cur_pos_valid)
			{
				return (range > 0) ? ORDER_TEST_NEEDS_TARGET : ORDER_TEST_INVALID_TARGET;
			}
			else if (order_rec->GetTargetPretestAdjacentPosition() &&
					!m_pos.IsNextTo(pos) && (m_pos != pos))
			{
				return ORDER_TEST_INVALID_TARGET;
			}
			else if (!can_afford)
			{
				return ORDER_TEST_LACKS_GOLD;
			}
			else if (m_array[army_index].CanPerformSpecialAction())
			{
				return ORDER_TEST_OK;
			}
			else
			{
				best_result = ORDER_TEST_NO_MOVEMENT;
			}
		}
	}

	return best_result;
}

//----------------------------------------------------------------------------
//
//  Name       : CargoTestOrderHere
//
//  Description: Test whether the cargo (units) being carried by this army's transports
//               are capable of executing an order at a location.
//
//  Parameters : const OrderRecord * order_rec : the order
//               const MapPoint & pos          : the location
//
//  Globals    : g_player     : player array
//
//  Returns    : ORDER_TEST   : legality result
//
//  Remark(s)  : -
//
//  Called by  :
//
//----------------------------------------------------------------------------
ORDER_TEST ArmyData::CargoTestOrderHere(const OrderRecord * order_rec, const MapPoint & pos) const
{
	bool order_valid;

	ORDER_TEST best_result = ORDER_TEST_ILLEGAL;
	//test if the player can afford the order
	Assert(g_player[m_owner]);
	bool can_afford = (g_player[m_owner]->GetGold() >= order_rec->GetGold());

	//order_rec: range = 0 (army must be on top of tile) or range = 1 (can execute order from adjacent tile)
	sint32 range = 0;
	(void) order_rec->GetRange(range);

	//test if whatever is at pos passes order_rec's TargetPretest
	bool target_valid = TargetValidForOrder(order_rec, pos);

	if(range <= 0 && !CargoCanEnter(pos))
	{
		target_valid = false;
	}

	for(sint32 army_index = 0; army_index < m_nElements; army_index++)
	{
		const UnitDynamicArray* cargo = m_array[army_index].AccessData()->GetCargoList();
		if (cargo)
		{
			for(sint32 cargo_index = 0; cargo_index < cargo->Num(); cargo_index++)
			{
				order_valid = cargo->Access(cargo_index).UnitValidForOrder(order_rec);
				if(order_valid)
				{
					if(range > 0 && pos == m_pos)
					{
						return ORDER_TEST_NEEDS_TARGET;
					}
					else if(!target_valid)
					{
						return ORDER_TEST_INVALID_TARGET;
					}
					else if(!can_afford)
					{
						return ORDER_TEST_LACKS_GOLD;
					}
					else if(m_array[army_index].CanPerformSpecialAction())
					{
						return ORDER_TEST_OK;
					}
					else
					{
						best_result = ORDER_TEST_NO_MOVEMENT;
					}
				}
			}
		}
	}
	return best_result;
}

//----------------------------------------------------------------------------
//
//  Name       : ArmyData::TargetValidForOrder
//
//  Description: test if whatever is at a target position passes an order's TargetPretest
//
//  Parameters : const OrderRecord * order_rec,
//               const MapPoint & pos,
//
//  Globals    : g_theWorld
//
//  Returns    : bool   : true if whatever is at MapPoint &pos passes OrderRecord * order_rec's TargetPretest
//
//  Remark(s)  : -
//
//  Called by  :
//
//----------------------------------------------------------------------------
bool ArmyData::TargetValidForOrder(const OrderRecord * order_rec, const MapPoint &pos)
{
	bool target_valid = false;

	Unit target_city = g_theWorld->GetCity(pos);
	if (order_rec->GetTargetPretestEnemyCity() ||
		order_rec->GetTargetPretestOwnCity())
	{
		target_valid = (target_city.IsValid());
	}

	CellUnitList target_army;
	g_theWorld->GetArmy(pos, target_army);
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
		target_valid = true;
		break;
	case k_Order_TargetPretest_AttackPosition_Bit:
		target_valid |= ((target_city.IsValid()) || (target_army.Num() > 0));
		break;
	};
	return target_valid;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::PerformOrder
//
// Description: Start performing an order
//
// Parameters : OrderRecord * order_rec
//
// Returns    : -
//
// Remark(s)  : funnels orders both from the user interface (controlpanelwindow) and from AI
//
//----------------------------------------------------------------------------
void ArmyData::PerformOrder(const OrderRecord * order_rec)
{
	Path tmp_path;
	tmp_path.SetStart(m_pos);//start a path from this army's current MapPoint

	PerformOrderHere(order_rec, &tmp_path);
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::PerformOrderHere
//
// Description: Perform an order
//
// Parameters : OrderRecord * order_rec : the order's DB record
//              Path * path             : a path starting from m_pos
//
// Globals    : g_gevManager
//
// Returns    : -
//
// Remark(s)  : actually perform the order
//
//----------------------------------------------------------------------------
void ArmyData::PerformOrderHere(const OrderRecord * order_rec, const Path * path, GAME_EVENT_INSERT priority)
{
	Assert(path != NULL);
	if (path == NULL)
		return;

	if (m_flags & k_CULF_IN_SPACE)
		return;

	Path *      tmp_path            = new Path(path);
	MapPoint    target_pos;

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
	sint32 moves = tmp_path->GetMovesRemaining();
	//order_rec: range = 0 (army must be on top of tile) or range = 1 (can execute order from adjacent tile)
	sint32 range = 0;
	if (order_rec->GetRange(range))
	{
		Assert(range <= moves || order_rec->GetTargetPretestAdjacentPosition());
	}

	sint32 min_rge, max_rge=0;
	MapPoint move_pos = m_pos; // move_pos will become a position to move to if trying to bombard out of range
	if (strcmp (order_rec->GetEventName(),"BombardOrder") == 0) // Should it really be like this? Actually, this belongs into the BombardEvent, which probably should just be given when we are in range.
	{
		if(GetBombardRange(min_rge, max_rge))
		{
			sint32 dist = m_pos.NormalizedDistance(target_pos);
			if(dist > max_rge) // The target is out of range
			{
				for(sint32 i = 0; i < moves; i++) // Find a position in tmp_path to move to
				{
					DPRINTF(k_DBG_FILE, ("move_pos (%d,%d), target_pos (%d,%d), NormalizedDistance %d, i %d\n",
										 move_pos.x, move_pos.y, target_pos.x, target_pos.y, dist, i));
					tmp_path->IncDir();
					tmp_path->GetCurrentPoint(move_pos);
					dist = move_pos.NormalizedDistance(target_pos);

					if(dist <= max_rge) // We're now within range
					{
						DPRINTF(k_DBG_FILE, ("\n found move_pos: we're now within range\n"));
						tmp_path->Start(m_pos); // Reset tmp_path

						for(sint32 j = 1; j < moves - i; j++) // Shorten move_path to end at move_pos
						{
							tmp_path->SnipEnd();
						}

						break; // Exit the loop and continue
					}
				}
			}
			else // Bombard now?
			{
				g_gevManager->AddEvent(priority,
				                       static_cast<GAME_EVENT>(game_event),
				                       GEA_Army, Army(m_id),
				                       GEA_MapPoint, target_pos,
				                       GEA_End
				                      );
			}
		}
	}
	else
	{
		for (sint32 i = 0; moves > 0 && i < range; i++)
		{
			tmp_path->SnipEnd();
			moves--;
		}
	}

	g_gevManager->Pause();
	//insert order's game_event here
	if (game_event > 0)
	{
		if (range > 0 || order_rec->GetIsTeleport() || order_rec->GetIsTarget())//event needs target pos
		{
			g_gevManager->AddEvent(priority,
			                       static_cast<GAME_EVENT>(game_event),
			                       GEA_Army, Army(m_id),
			                       GEA_MapPoint, target_pos,
			                       GEA_End
			                      );
		}
		else
		{
			g_gevManager->AddEvent(priority,
			                       static_cast<GAME_EVENT>(game_event),
			                       GEA_Army, Army(m_id),
			                       GEA_End
			                      );
		}
	}

	//insert GEV_MoveOrder here, i.e., move adjacent to target pos or within bombarding range of target pos
	if(tmp_path->GetMovesRemaining() > 0
	&& !order_rec->GetIsTeleport()
	&& !order_rec->GetIsTarget()
	){
		if(max_rge > 0) // max_rge > 0 implys BombardOrder and army has bombarding units
		{
			double cur_move_pts;
			CurMinMovementPoints(cur_move_pts);

			DPRINTF(k_DBG_FILE, ("army 0x%lx, cur_move_pts=%f, move_pos=<%d,%d>\n",m_id,cur_move_pts,move_pos.x,move_pos.y));

			if(move_pos != m_pos) // Then first move army to move_pos
			{

				g_gevManager->AddEvent(priority,
				                       GEV_MoveOrder,
				                       GEA_Army, Army(m_id),
				                       GEA_Path, tmp_path,
				                       GEA_MapPoint, move_pos,
				                       GEA_Int, (game_event == -1),
				                       GEA_End
				                      );
			}
			else
			{
				delete tmp_path;
			}
			// Bombard target_pos
			// Nothing to do here since the order has been already given.

		}
		else // Not a bombard order
		{
			g_gevManager->AddEvent(priority,
			                       GEV_MoveOrder,
			                       GEA_Army, Army(m_id),
			                       GEA_Path, tmp_path,
			                       GEA_MapPoint, target_pos,
			                       GEA_Int, (game_event == -1),
			                       GEA_End
			                      );
		}
	}
	else
	{
		delete tmp_path;
	}

	g_gevManager->AddEvent(priority,
	                       GEV_ClearOrders,
	                       GEA_Army, Army(m_id),
	                       GEA_End
	                      );

	g_gevManager->Resume();
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::AssociateEventsWithOrdersDB
//
// Description: Produces a map from order database records to the game events they use
//
// Parameters : -
//
// Globals    : g_gevManager
//            : g_theOrderDB
//
// Returns    : -
//
// Remark(s)  : s_orderDBToEventMap[order_index] returns the event name you find in
//              the orderDB record whose index is order_index
//
//----------------------------------------------------------------------------
void ArmyData::AssociateEventsWithOrdersDB()
{
	delete [] s_orderDBToEventMap;
	s_orderDBToEventMap = new sint32[g_theOrderDB->NumRecords()];

	for (sint32 order_index = 0; order_index < g_theOrderDB->NumRecords(); order_index++)
	{
		char const * event_name = g_theOrderDB->Get(order_index)->GetEventName();
		if (strlen(event_name) > 0)
		{
			s_orderDBToEventMap[order_index] = g_gevManager->GetEventIndex(event_name);
		}
		else
		{
			s_orderDBToEventMap[order_index] = -1;
			DPRINTF(k_DBG_SCHEDULER, ("AssociateEventsWithOrdersDB: No matching event found for	order DB index %d.\n",
				order_index));
		}
	}
}

void ArmyData::Settle()
{
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
	                       GEV_Settle,
	                       GEA_Army, m_id,
	                       GEA_End
	                      );
}

void ArmyData::SettleInCity()
{
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
	                       GEV_SettleInCity,
	                       GEA_Army, m_id,
	                       GEA_End
	                      );
}

bool ArmyData::IsObsolete() const
{
	Assert(g_player[m_owner] != NULL);

	for (sint32 i = 0; i < m_nElements; i++)
	{
		const UnitRecord * rec = m_array[i].GetDBRec();

		for (sint32 a = 0; a < rec->GetNumObsoleteAdvance(); a++)
		{
			if (g_player[m_owner]->HasAdvance(rec->GetObsoleteAdvance(a)->GetIndex()))
			{
				return true;
			}
		}
	}

	return false;
}

bool ArmyData::IsCargoObsolete() const
{
	Assert(g_player[m_owner] != NULL);

	for(sint32 i = 0; i < m_nElements; i++)
	{
		UnitDynamicArray* cargoList = m_array[i]->GetCargoList();
		sint32 n = cargoList != NULL ? cargoList->Num() : 0;

		for(sint32 j = 0; j < n; j++)
		{
			const UnitRecord * rec = cargoList->Get(j).GetDBRec();

			for(sint32 a = 0; a < rec->GetNumObsoleteAdvance(); a++)
			{
				if(g_player[m_owner]->HasAdvance(rec->GetObsoleteAdvance(a)->GetIndex()))
				{
					return true;
				}
			}
		}
	}

	return false;
}

void ArmyData::StopPirating()
{
	if(!m_isPirating) return;

	Cell *cell = g_theWorld->GetCell(m_pos);
	Assert(cell);
	if (cell)
	{
		for(sint32 i = 0; i < cell->GetNumTradeRoutes(); ++i)
		{
			TradeRoute route = cell->GetTradeRoute(i);
			if (route->GetPiratingArmy().m_id == m_id)
			{
				g_gevManager->AddEvent
				    (GEV_INSERT_AfterCurrent, GEV_SetPiratingArmy, // executes StopPiracyRegardEvent
				     GEA_TradeRoute, route,
				     GEA_Army, 0, // setting the pirating army to ID= 0 stops pirating this route
				     GEA_End
				    );
			}
		}
	}

	m_isPirating = false;
}

const MBCHAR * ArmyData::GetName() const
{
	if (m_name)
	{
		return m_name;
	}
	else
	{
		/// @todo Check possible reentrancy problems
		static MBCHAR buf[40];
		sprintf(buf, "%s%d", g_theStringDB->GetNameStr("ARMY_NAME_PREFIX"), m_id & (0x0fffffff));
		return buf;
	}
}

void ArmyData::SetName(const MBCHAR *name)
{
	delete [] m_name;
	if (name)
	{
		m_name = new MBCHAR[strlen(name) + 1];
		strcpy(m_name, name);
	}
	else
	{
		m_name = NULL;
	}
}

const MBCHAR * ArmyData::GetDebugString() const
{
	return m_debugString;
}

void ArmyData::SetDebugString(const MBCHAR * name)
{
	delete [] m_debugString;
	if (name)
	{
		m_debugString = new MBCHAR[strlen(name) + 1];
		strcpy(m_debugString, name);
	}
	else
	{
		m_debugString = NULL;
	}
}
bool ArmyData::PlayerCanSee(const PLAYER_INDEX playerId) const
{
	for (sint32 i = 0; i < Num() ; i++)
	{
		if (m_array[i].GetVisibility() & (1 << playerId))
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : ArmyData::CargoCountIf
//
// Description: Count the number of units (including cargo) having a property.
//
// Parameters : a_HasProperty   Property determining function
//
// Globals    : -
//
// Returns    : size_t          The number of units having the property
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
size_t ArmyData::CargoCountIf(UnitRecord::BoolAccessor a_HasProperty) const
{
	size_t          propertyCount   = 0;
	size_t const    unitCount       = static_cast<size_t>(Num());

	for (size_t i = 0; i < unitCount; ++i)
	{
		// Check army unit itself
		if ((m_array[i].GetDBRec()->*a_HasProperty)())
		{
			++propertyCount;
		}

		// Check cargo of army unit
		UnitDynamicArray const *    cargo       =
		    m_array[i].AccessData()->GetCargoList();
		size_t const                cargoCount  =
		    cargo ? static_cast<size_t>(cargo->Num()) : 0;

		for (size_t j = 0; j < cargoCount; ++j)
		{
			if ((cargo->Access(j).GetDBRec()->*a_HasProperty)())
			{
				++propertyCount;
			}
		}
	}

	return propertyCount;
}

sint16 ArmyData::CountNuclearUnits() const
{
	return static_cast<sint16>(CargoCountIf(&UnitRecord::HasNuclearAttack));
}

sint16 ArmyData::CountBioUnits() const
{
	return static_cast<sint16>(CargoCountIf(&UnitRecord::HasBioTerror));
}

sint16 ArmyData::CountNanoUnits() const
{
	return static_cast<sint16>(CargoCountIf(&UnitRecord::HasCreateParks));
}

sint16 ArmyData::DisbandNuclearUnits(const sint16 count)
{
	const UnitRecord *rec;
	const UnitDynamicArray *cargo;
	sint16 disbanded = 0;
	sint32 j = 0;
	for (sint32 i = (m_nElements - 1); i >= 0 && (disbanded < count); i--)
	{
		rec = m_array[i].GetDBRec();
		cargo = m_array[i].AccessData()->GetCargoList();

		if (rec->HasNuclearAttack())
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
				rec = cargo->Access(j).GetDBRec();
				if (rec->HasNuclearAttack())
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
	sint16 disbanded = 0;
	sint32 j = 0;
	for (sint32 i = (m_nElements - 1); i >= 0 && (disbanded < count); i--)
	{
		rec = m_array[i].GetDBRec();
		cargo = m_array[i].AccessData()->GetCargoList();

		if (rec->HasBioTerror())
		{
			if (cargo && cargo->Num() > 0)
			{
				Assert(false);
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
				rec = cargo->Access(j).GetDBRec();
				if (rec->HasBioTerror())
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
	sint16 disbanded = 0;
	sint32 j = 0;
	for (sint32 i = (m_nElements - 1); i >= 0 && (disbanded < count); i--)
	{
		rec = m_array[i].GetDBRec();
		cargo = m_array[i].AccessData()->GetCargoList();

		if (rec->HasCreateParks())
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
				rec = cargo->Access(j).GetDBRec();
				if (rec->HasCreateParks())
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
	for(sint32 i = 0; i < m_nElements; i++)
	{
		if (m_array[i]->IsVeteran())
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

bool ArmyData::IsInVisionRangeAndCanEnter(MapPoint &pos) const
{
	if(!CanEnter(pos))
		return false;

	MapPoint here;
	GetPos(here);

	if(!g_theWorld->IsOnSameContinent(pos, here))
		return false;

	// !CargoCanEnter(pos) add this

	sint32 visionRange = 0;
	sint32 index = 0;

	for (sint32 i = 0; i < m_nElements; i++)
	{
		if (m_array[i]->GetVisionRange() > visionRange)
		{
			visionRange = static_cast<sint32>(m_array[i]->GetVisionRange());
			index = i;
		}
	}

	return m_array[index].IsInVisionRange(pos);
}

bool ArmyData::NearestUnexplored(MapPoint &pos) const
{
	sint32 visionRange = 0;
	sint32 index = 0;

	for (sint32 i = 0; i < m_nElements; i++)
	{
		if (m_array[i]->GetVisionRange() > visionRange)
		{
			visionRange = static_cast<sint32>(m_array[i]->GetVisionRange());
			index = i;
		}
	}

	sint32 searchRadius = visionRange + 7; // Should be exposed
	return m_array[index].NearestUnexplored(searchRadius, pos);
}

bool ArmyData::Upgrade()
{
	sint32 count = 0;

	sint32 type  = 0;
	sint32 costs = 0;

	for(sint32 i = m_nElements - 1; i >= 0; i--)
	{
		if(m_array[i]->CanUpgrade(type, costs))
		{
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_UpgradeUnit,
			                       GEA_Unit, m_array[i],
			                       GEA_End);

			++count;
		}
	}

	return count != 0;
}

bool ArmyData::UpgradeTypeAndCosts(bool & full, sint32 & costs, sint32 & fullCosts, sint8 & numUpgrade, sint8 & numUpgradeAll) const
{
	numUpgrade    = 0;
	numUpgradeAll = 0;
	costs         = 0;
	fullCosts     = 0;

	for(sint32 i = 0; i < m_nElements; ++i)
	{
		sint32 type = m_array[i]->GetBestUpgradeUnitType();

		if(type >= 0)
		{
			++numUpgradeAll;

			sint32 unit_costs = m_array[i]->GetUpgradeCosts(type);

			fullCosts += unit_costs;

			if(costs + unit_costs <= g_player[m_owner]->m_gold->GetLevel())
			{
				costs += unit_costs;
				++numUpgrade;
			}
		}
	}

	full = (numUpgrade == numUpgradeAll);

	return numUpgradeAll != 0;
}

void ArmyData::CheckHostileTerrain()
{
	const RiskRecord *risk = g_theRiskDB->Get(g_theGameSettings->GetRisk());
	Cell *cell = g_theWorld->GetCell(m_pos);
	sint32 cellowner = cell->GetOwner();
	// EMOD: If Hostileterrain and not fort than deduct HP from the unit
	TerrainRecord const * trec = g_theTerrainDB->Get(g_theWorld->GetTerrainType(m_pos));
	sint32 hpcost;
	if(trec->GetHostileTerrainCost(hpcost) && m_owner > 0) // Add AI immunity? No cheats, E!
	{
		if(g_rand->Next(10000) < risk->GetBarbarianChance() * 10000)
		{

			if( !terrainutil_HasFort(m_pos)
			&& !terrainutil_HasAirfield(m_pos) // Added by E 5-28-2006
			&& g_player[m_owner]->HasWarWith(cellowner)  //added 5-24-2007 so your and friendly territory isn't hostile
			|| cellowner == PLAYER_UNASSIGNED //added 5-24-2007 wastelands should be hostile
			){
				for(sint32 i = 0; i < m_nElements; i++) {
					const UnitRecord *urec = m_array[i].GetDBRec();
					if(!urec->GetImmuneToHostileTerrain()) {
						m_array[i].DeductHP(hpcost);
						SlicObject *so = new SlicObject("999HostileTerrain");
						so->AddRecipient(m_owner);
						so->AddUnitRecord(m_array[i].GetType());
						g_slicEngine->Execute(so);

						if (m_array[i].GetHP() < 0.999) {
							m_array[i].Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
						}
					}
				}
			}
		}
	}
}

void ArmyData::CheckMineField()
{
//	const RiskRecord *risk = g_theRiskDB->Get(g_theGameSettings->GetRisk());
//	Cell *cell = g_theWorld->GetCell(m_pos);
//	sint32 CellOwner = cell->GetOwner();
		//EMOD If tile has tileimp that is a minefield then deduct HP
	if(terrainutil_HasMinefield(m_pos)
	){
		//TerrainRecord const * tirec = g_theTerrainImprovementDB->Get(g_theWorld->GetCell(m_pos)->GetDBImprovement());
		double hpcost2;
		/// @todo use standard identifiers for index variables like i and j
		for(sint32 ti = 0; ti < g_theWorld->GetCell(m_pos)->GetNumDBImprovements(); ++ti)
		{ /// @todo use i instead of ti
			const TerrainImprovementRecord * tirec = g_theTerrainImprovementDB->Get(g_theWorld->GetCell(m_pos)->GetDBImprovement(ti));
			const TerrainImprovementRecord::Effect *effect = terrainutil_GetTerrainEffect(tirec, m_pos);
			for(sint32 u = 0; u < m_nElements; u++)
			{ /// @todo use j instead of u
				if(effect->GetMinefield(hpcost2) && !m_array[u].GetMovementTypeAir())
				{ //EMOD
					m_array[u].DeductHP(hpcost2); // This should go into the MoveUnits event
				}
				if(m_array[u].GetHP() < 0.999)
				{
					m_array[u].Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
				}
			}
		}
	}
}

bool ArmyData::CheckSink()
{
	sint32 numSunken = 0;

	// Lost at sea random chance
	// Maybe move this code into an event
	sint32 chance = g_theConstDB->Get(0)->GetChanceLostAtSea();
	if( chance > 0
	&&(!g_player[m_owner]->IsRobot()
	|| !g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetAINoSinking())
	){
		for(sint32 i = 0; i < m_nElements; i++)
		{
			if(m_array[i].Sink(chance)){
				numSunken++;
			}
		}
	}

	return numSunken == m_nElements;
}

void ArmyData::BarbarianSpawning()
{
	const RiskRecord *risk = g_theRiskDB->Get(g_theGameSettings->GetRisk());
	sint32 i;
	//const MapAnalysis & map = MapAnalysis::GetMapAnalysis();
	//MapPoint epos = map.GetNearestForeigner(PLAYER_INDEX_VANDALS, m_pos);
	//Cell *ecell = g_theWorld->GetCell(epos);
	sint32 meat = PLAYER_UNASSIGNED;
	//PLAYER_INDEX meat = CellOwner;

	// EMOD Barbarian Camps
	// This should be risk level depending  //EMOD added Risk 10-05-2006
	if(g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetNumBarbarianCamps())
	{
		if(g_rand->Next(10000) < risk->GetBarbarianChance() * 10000) {
			for(i = 0; i < m_nElements; i++) {
//				Cell *cell = g_theWorld->GetCell(m_pos);
				const DifficultyRecord *drec = g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty());
				//sint32 j;
				//sint32 newimp = drec->GetBarbarianCampsIndex(j);
				if(m_array[i].IsEntrenched()
				&& drec->GetNumBarbarianCamps()
				&& m_owner == PLAYER_INDEX_VANDALS
				){
					for(sint32 j = 0; j < drec->GetNumBarbarianCamps(); j++) {
						g_player[m_owner]->CreateSpecialImprovement(drec->GetBarbarianCampsIndex(j), m_pos, 0);
					}
				}
			}
		}
	}

	// This should be risk level depending //EMOD added Risk 10-25-2006
	if(g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetBarbarianCities())
	{
		if(g_rand->Next(10000) < risk->GetBarbarianChance() * 10000) {
			for(i = 0; i < m_nElements; i++) {
				if(m_array[i].IsEntrenched()
				&&!g_theWorld->GetCity(m_pos)
				&& m_owner == PLAYER_INDEX_VANDALS
				){
					g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_CreateCity,
						GEA_Player, PLAYER_INDEX_VANDALS,
						GEA_MapPoint, m_pos,
						GEA_Int, CAUSE_NEW_CITY_GOODY_HUT,
						GEA_Int, -1,
						GEA_End);
				}
			}
		}
	}
	// Barbarian leader spawn
	// This should be risk level depending ADDED EMOD 10-05-2006
	if(m_owner == PLAYER_INDEX_VANDALS) {
		sint32 barbmax = g_theRiskDB->Get(g_theGameSettings->GetRisk())->GetMaxSpontaniousBarbarians();
		sint32 barbhorde = g_player[PLAYER_INDEX_VANDALS]->m_all_units->Num();

		if(
			(g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetBarbarianSpawnsBarbarian())
		||  (g_theProfileDB->IsBarbarianSpawnsBarbarian())
		){

			if ( (barbhorde) >= (barbmax^2) ) {
				return;
			}

			if (//new limits to prevent barbarian spam
				(barbhorde) <= (barbmax^2) // create some kind of max
			){
				if(g_rand->Next(10000) < risk->GetBarbarianChance() * 10000) {
					Barbarians::AddBarbarians(m_pos, meat, true);
				}
			}
		}
	}

}

bool ArmyData::HasElite() const
{
	for(sint32 i = 0; i < m_nElements; i++)
	{
		if (m_array[i]->IsElite())
			return true;
	}

	return false;
}
