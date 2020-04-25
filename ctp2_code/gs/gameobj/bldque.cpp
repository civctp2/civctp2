//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Build queue handling
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
// CTP1_HAS_RISEN_FROM_THE_GRAVE
// Activate CTP1 wormhole probe production reporting
//
//----------------------------------------------------------------------------
//
// Note:
// The build queue start/stop reports are considering local modifications only.
// When swapping the wonder production of 2 cities, you may get 4 reports.
// To improve reporting further, the wonder start/stop changes would have to go
// through a "clearinghouse" for the entire civilisation.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Allow infastructure and capitalisation when loading the build queue
//   (modification as posted by Peter Triggs).
// - Players are now notified if someone starts to build the first wonder
//   from the database. - Feb. 23rd 2005 Martin Gühmann
// - Added assignment operator. - Jul 16th 2005 Martin Gühmann
// - Removed old endgame database, doesn't seem to be used. (Aug 29th 2005 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Report (wonder) start of second item when deleting the first item.
// - Improved handling of multiple build queue actions in a turn.
// - Add PopCostsToBuild to allow for Units with higher than 1 (BuildingRemovesaPop)
//   population cost to be removed from a city.
// - Replaced old difficulty database by new one. (April 29th 2006 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - Reimplmented the Serialize method to shrink the size of instances. (13-Jan-2019 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "BldQue.h"

#include "c3errors.h"
#include "c3files.h"
#include "gamefile.h"
#include "ObjPool.h"
#include "Unit.h"
#include "MapPoint.h"
#include "player.h"
#include "citydata.h"
#include "civarchive.h"
#include "BuildingRecord.h"
#include "AICause.h"
#include "Checksum.h"
#include "CivPaths.h"               // g_civPaths
#include "WonderRecord.h"
#include "StrDB.h"
#include "network.h"
#include "net_info.h"
#include "net_action.h"
#include "ConstRecord.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "QuickSlic.h"
#include "World.h"
#include "Cell.h"
#include "soundmanager.h"           // g_soundManager
#include "gamesounds.h"
#include "SelItem.h"

#include "UnitData.h"
#include "UnitPool.h"
#include "profileDB.h"
#include "WonderTracker.h"
#include "wondermoviewin.h"
#include "Diffcly.h"
#include "controlpanelwindow.h"     // g_controlPanel
#include "director.h"
#include "GameSettings.h"
#include "AgeRecord.h"
#include "GovernmentRecord.h"

#include "UnitRecord.h"
#include "GameEventManager.h"
#include "Globals.h"
#include "buildingutil.h"
#include "wonderutil.h"

#define k_BUILDQUEUE_VERSION_MAJOR	0
#define k_BUILDQUEUE_VERSION_MINOR	1

namespace
{

    sint32 const        NOTHING_THIS_TURN   = -1;

} // namespace

BuildQueue::BuildQueue()
:
	m_list                      (new PointerList<BuildNode>),
	m_owner                     (PLAYER_UNASSIGNED),
	m_city                      (),
	m_wonderStarted             (NOTHING_THIS_TURN),
	m_wonderStopped             (NOTHING_THIS_TURN),
	m_wonderComplete            (NOTHING_THIS_TURN),
	m_frontWhenBuilt            (NULL),
	m_settler_pending           (false),
	m_popcoststobuild_pending   (false),          // EMOD
	m_name                      (NULL)
{
}
/* Doesn't seem to copy m_list properly
BuildQueue::BuildQueue(BuildQueue & copy)
:
	m_list(copy.m_list),
	m_owner(copy.m_owner),
	m_city(copy.m_owner),
	m_wonderStarted(copy.m_owner),
	m_wonderStopped(copy.m_owner),
	m_wonderComplete(copy.m_owner),
	m_frontWhenBuilt(NULL),
	m_settler_pending(copy.m_owner),
	m_popcoststobuild_pending(copy.m_owner),          // EMOD
	m_name(NULL)
{
	SetName(copy.m_name);
	if(copy.m_frontWhenBuilt != NULL)
	{
		m_frontWhenBuilt = m_list->GetHead();
	}
}*/


BuildQueue::~BuildQueue()
{
	// m_frontWhenBuilt not deleted: reference only

	if (m_list)
	{
		m_list->DeleteAll();
		delete m_list;
	}

	delete[] m_name;
}

void BuildQueue::Serialize(CivArchive &archive)
{
	if (archive.IsStoring())
	{
		archive.PutSINT32(m_owner);
		m_city.Serialize(archive);
		archive.PutSINT32(m_wonderStarted);
		archive.PutSINT32(m_wonderStopped);

		MBCHAR name[256]; // Replace an unused but saved member
		memset(name, '\0', sizeof(name));
		if(m_name != NULL)
		{
			strcpy(name, m_name);
		}

		archive.StoreChunk((uint8*)&name, (uint8*)&name + sizeof(name));

		archive << m_wonderComplete;
		sint32 c = m_list->GetCount();
		Assert(c >= 0);
		archive << c;
		PointerList<BuildNode>::Walker walk(m_list);
		while(walk.IsValid())
		{
			archive.Store((uint8*)walk.GetObj(), sizeof(BuildNode));
			walk.Next();
		}
	}
	else
	{
		m_owner = archive.GetSINT32();
		m_city.Serialize(archive);
		m_wonderStarted = archive.GetSINT32();
		m_wonderStopped = archive.GetSINT32();

		MBCHAR name[256]; // Replace an unused but saved member
		archive.LoadChunk((uint8*)&name, (uint8*)&name + sizeof(name));
		m_name = new MBCHAR[strlen(name) + 1];
		strcpy(m_name, name);

		if(g_saveFileVersion >= 62)
		{
			archive >> m_wonderComplete;
		}
		else
		{
			m_wonderComplete = NOTHING_THIS_TURN;
		}

		uint32 c;
		archive >> c;

		m_list->DeleteAll();
		for (uint32 i = 0; i < c; ++i)
		{
			BuildNode *node = new BuildNode;
			archive.Load((uint8*)node, sizeof(BuildNode));
			m_list->AddTail(node);
		}
	}
}

uint32 BldQue_BuildQueue_GetVersion(void)
{
	return (k_BUILDQUEUE_VERSION_MAJOR<<16 | k_BUILDQUEUE_VERSION_MINOR) ;
}

// Loads a custom queue into a city.
sint32 BuildQueue::Load(const MBCHAR *file)
{

	FILE * fpQueue = c3files_fopen(C3DIR_DIRECT, file, "r");
	if (!fpQueue) return 0;

	if (m_city.CD()->AlreadyBoughtFront())
	{
		ClearAllButHead();
	}
	else
	{
		Clear();
	}

	char buf[k_MAX_NAME_LEN];
	sint32 category = 0;
	sint32 type = 0;
	sint32 cost = 0;
	while(!c3files_feof(fpQueue))
	{

		if(!c3files_fgets(buf, k_MAX_NAME_LEN, fpQueue))
			continue;

		buf[strlen(buf) - 1] = 0;

		switch(buf[0])
		{
			case 'U':
				category = k_GAME_OBJ_TYPE_UNIT;
				if(!g_theUnitDB->GetNamedItem(&buf[2], type))
				{
					Assert(FALSE);
					continue;
				}
				cost = g_theUnitDB->Get(type)->GetShieldCost();
				break;
			case 'B':
				category = k_GAME_OBJ_TYPE_IMPROVEMENT;
				if(!g_theBuildingDB->GetNamedItem(&buf[2], type))
				{
					Assert(FALSE);
					continue;
				}
				//ProductionCostPopModifier m_city.CD()->PopCount()
				//EMOD ProductionCostPopModifier  10-10-2006

				sint32 bcost;

				if(buildingutil_Get(type, m_owner)->GetProductionCostPopModifier())
				{
					bcost = buildingutil_Get(type, m_owner)->GetProductionCost() * m_city.CD()->PopCount();
				}
				else
				{
					bcost = buildingutil_Get(type, m_owner)->GetProductionCost();
				}

				cost = bcost;
				//end EMOD 10-10-2006
				//cost = buildingutil_Get(type, m_owner)->GetProductionCost(); //original
				break;
			case 'W':
				category = k_GAME_OBJ_TYPE_WONDER;
				if(!g_theWonderDB->GetNamedItem(&buf[2], type))
				{
					Assert(FALSE);
					continue;
				}

				cost = wonderutil_Get(type, m_owner)->GetProductionCost();
				break;
			case '#':

				continue;
			case 'C':
				Assert(buf[1] == 'A' && buf[2] == 'P');
				if(buf[1] == 'A' && buf[2] == 'P') {
					category = k_GAME_OBJ_TYPE_CAPITALIZATION;
					type = 0;
				} else {
					continue;
				}
				break;
			case 'I':
				Assert(buf[1] == 'N' && buf[2] == 'F');
				if(buf[1] == 'N' && buf[2] == 'F') {
					category = k_GAME_OBJ_TYPE_INFRASTRUCTURE;
					type = 0;
				} else {
					continue;
				}
				break;
			default:
				Assert(false);
				continue;
		}
		InsertTail(category, type, cost);
	}

	c3files_fclose(fpQueue);

	RemoveIllegalItems();

	return 1;
}

sint32 BuildQueue::Save(const MBCHAR *file)
{
	FILE * fpQueue = c3files_fopen(C3DIR_DIRECT, file, "w");
	if(!fpQueue) return 0;

	PointerList<BuildNode>::Walker walk(m_list);
	while(walk.IsValid()) {
		switch(walk.GetObj()->m_category) {
			case k_GAME_OBJ_TYPE_UNIT:
				c3files_fprintf(fpQueue, "U %s\n", g_theUnitDB->Get(walk.GetObj()->m_type)->GetIDText());
				break;
			case k_GAME_OBJ_TYPE_IMPROVEMENT:
				c3files_fprintf(fpQueue, "B %s\n", g_theBuildingDB->Get(walk.GetObj()->m_type)->GetIDText());
				break;
			case k_GAME_OBJ_TYPE_WONDER:
				c3files_fprintf(fpQueue, "W %s\n", g_theWonderDB->Get(walk.GetObj()->m_type)->GetIDText());
				break;
		}
		walk.Next();
	}
	c3files_fclose(fpQueue);
	return 1;
}

void BuildQueue::EndTurn(void)
{
	if (m_wonderStarted != m_wonderStopped)
	{
// Correct the condition with the index shift
        if (m_wonderStopped >= 0) {
            SendMsgWonderStopped(m_wonderStopped) ;
            g_theWonderTracker->ClearBuildingWonder(m_wonderStopped, m_owner);
        }
        if (m_wonderStarted != m_wonderComplete && m_wonderStarted >= 0) {
            SendMsgWonderStarted(m_wonderStarted) ;
            g_theWonderTracker->SetBuildingWonder(m_wonderStarted, m_owner);
        }
    }

	m_wonderComplete           = NOTHING_THIS_TURN;
	m_wonderStarted            = NOTHING_THIS_TURN;
	m_wonderStopped            = NOTHING_THIS_TURN;
	m_settler_pending          = false;
	m_popcoststobuild_pending  = false;              // EMOD
}

void BuildQueue::Clear(bool fromServer)
{
	if(!fromServer && g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_CLEAR_QUEUE,
										   (uint32)m_city));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_CLEAR_QUEUE,
									  (uint32)m_city));
		g_network.Unblock(m_owner);
	}

    HandleProductionStop();
    m_list->DeleteAll();
}

void BuildQueue::ClearAllButHead(bool fromServer)
{
	if (m_list->GetCount() > 1)
    {
	    if(!fromServer && g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		    g_network.SendAction(new NetAction(NET_ACTION_CLEAR_QUEUE_EXCEPT_HEAD));
	    } else if(g_network.IsHost()) {
		    g_network.Block(m_owner);
		    g_network.Enqueue(new NetInfo(NET_INFO_CODE_CLEAR_QUEUE_EXCEPT_HEAD,
									      (uint32)m_city));
		    g_network.Unblock(m_owner);
	    }

	    BuildNode * head = m_list->RemoveHead();
	    m_list->DeleteAll();
	    m_list->AddHead(head);
    }
}

bool BuildQueue::BuildFrontUnit(bool forceFinish)
{
	Unit u;
	CityData *cd = m_city.CD();
	if (m_list->GetHead()->m_cost <= m_city.CD()->GetStoredCityProduction()) {
		MapPoint cpos;
		m_city.GetPos(cpos);
		if(g_theWorld->GetCell(cpos)->GetNumUnits() >= k_MAX_ARMY_SIZE &&
		   !g_theUnitDB->Get(m_list->GetHead()->m_type)->GetIsTrader()) {
			SlicObject *so = new SlicObject("106CantBuildUnitCellIsFull");
			so->AddCity(m_city);
			so->AddRecipient(m_owner);
			so->AddAction(g_theStringDB->GetNameStr(g_theUnitDB->Get(m_list->GetHead()->m_type)->m_name));
			g_slicEngine->Execute(so);
			return false;
		}

		if(g_theUnitDB->Get(m_list->GetHead()->m_type)->GetBuildingRemovesAPop()
		&& cd->PopCount() < 2
		&&!forceFinish
		){
			if(!g_player[m_owner]->IsRobot()
			|| (g_network.IsClient()
			&&  g_network.IsLocalPlayer(m_owner))
			){
				m_settler_pending = true;   // This gets "sent" to CityEvent::CityBuildFrontEvent and all it does is trigger a slic object, why doesn't just work like above?
				return false;
			}
		}
		// EMOD to prevent units with POP > 1 from disbanding city
		// with popcoststobuild pending
		sint32 unitpop;
		if(g_theUnitDB->Get(m_list->GetHead()->m_type)->GetPopCostsToBuild(unitpop)
		&& cd->PopCount() <= unitpop
		&&!forceFinish
		){
			if(!g_player[m_owner]->IsRobot()
			|| (g_network.IsClient()
			&&  g_network.IsLocalPlayer(m_owner))
			){
				m_popcoststobuild_pending  = true;
				return false;
			}
		}
		// End EMOD

		DPRINTF(k_DBG_GAMESTATE, ("City %lx building unit: %s\n",
								  (uint32)cd->GetHomeCity(),
								  g_theStringDB->GetNameStr(g_theUnitDB->Get(m_list->GetHead()->m_type)->m_name)));

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
							   GEV_CreateUnit,
							   GEA_Player, m_owner,
							   GEA_MapPoint, cpos,
							   GEA_City, m_city,
							   GEA_Int, m_list->GetHead()->m_type,
							   GEA_Int, CAUSE_NEW_ARMY_BUILT,
							   GEA_End);
	} else {
		return false;
	}
	return true;
}

bool BuildQueue::BuildFrontBuilding()
{
	if (m_list->GetHead()->m_cost <= m_city.CD()->GetStoredCityProduction()) {
		DPRINTF(k_DBG_GAMESTATE, (
				  "City %lx built improvement: %s\n",
				  (uint32)m_city,
				  g_theStringDB->GetNameStr(g_theBuildingDB->Get(m_list->GetHead()->m_type)->m_name)));

		m_list->GetHead()->m_flags |= k_BUILD_NODE_FLAG_ALREADY_BUILT;
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_CreateBuilding,
							   GEA_City, m_city.m_id,
							   GEA_Int, m_list->GetHead()->m_type,
							   GEA_End);
		return true;
	} else {
		return false;
	}
}

bool BuildQueue::BuildFrontWonder()
{
	if(wonderutil_IsBuilt(m_list->GetHead()->m_type))
	{
		RemoveHead();
		return false;
	}

	if(m_list->GetHead()->m_cost <= m_city.CD()->GetStoredCityProduction()) {
		DPRINTF(k_DBG_GAMESTATE, ("City %lx built wonder: %s\n",
								  (uint32)m_city,
								  g_theStringDB->GetNameStr(g_theWonderDB->Get(m_list->GetHead()->m_type)->GetName())));

		m_list->GetHead()->m_flags |= k_BUILD_NODE_FLAG_ALREADY_BUILT;
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_CreateWonder,
							   GEA_City, m_city.m_id,
							   GEA_Int, m_list->GetHead()->m_type,
							   GEA_End);

		wonderutil_AddBuilt(m_list->GetHead()->m_type);

		return true;
	}
	else
	{
		if(m_city.CD()->HowMuchLonger() <= 3 &&
		   !(m_list->GetHead()->m_flags & k_BUILD_NODE_FLAG_ALMOST_DONE)) {
			m_list->GetHead()->m_flags |= k_BUILD_NODE_FLAG_ALMOST_DONE;
			if(g_network.IsHost()) {
				g_network.Block(m_owner);
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_WONDER_ALMOST_DONE,
											  m_owner, m_list->GetHead()->m_type,
											  m_city.m_id));
				g_network.Unblock(m_owner);
			}

			SlicObject *so = new SlicObject("45WonderAlmostFinished");
			so->AddAllRecipientsBut(m_owner);
			so->AddWonder(m_list->GetHead()->m_type);
			so->AddCivilisation(m_owner);
			so->AddCity(m_city);
			g_slicEngine->Execute(so);
		}
		return false;
	}
}

bool BuildQueue::BuildFrontEndgame()
{
	if(m_list->GetHead()->m_cost <= m_city.CD()->GetStoredCityProduction())
	{
		m_list->GetHead()->m_flags |= k_BUILD_NODE_FLAG_ALREADY_BUILT;
		DPRINTF(k_DBG_GAMESTATE, ("City %lx built endgame object type %d\n",
								  m_city.m_id, m_list->GetHead()->m_type));
		g_player[m_owner]->AddEndGameObject(m_city, m_list->GetHead()->m_type);
		return true;
	}
	else
	{
		return false;
	}
}

bool BuildQueue::BuildFront(sint32 &shieldstore, CityData *cd, const MapPoint &pos,
                           uint64 &built_improvements, uint64 &built_wonders,
                            bool forceFinish)
{
	bool is_something_built = false;

	m_settler_pending = false;
	m_popcoststobuild_pending = false; // EMOD

	if (m_list->GetHead()) {
		DPRINTF(k_DBG_GAMESTATE, ("BuildFront: City %lx building %d,%d\n",
								  m_city.m_id, m_list->GetHead()->m_category,
								  m_list->GetHead()->m_type));

		switch (m_list->GetHead()->m_category) {
		case k_GAME_OBJ_TYPE_UNIT:
			is_something_built = BuildFrontUnit(forceFinish);
			break;
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
			is_something_built = BuildFrontBuilding();
			break;
		case k_GAME_OBJ_TYPE_WONDER:
			is_something_built = BuildFrontWonder();
			break;
		case k_GAME_OBJ_TYPE_ENDGAME_OBJECT:
			is_something_built = BuildFrontEndgame();
			break;
		case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
			m_city.CD()->BuildInfrastructure();
			break;
		case k_GAME_OBJ_TYPE_CAPITALIZATION:
			m_city.CD()->BuildCapitalization();
			break;
		default:
			Assert(0);
		}

		if (is_something_built) {

			m_frontWhenBuilt = m_list->GetHead();

			if (g_soundManager) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if (visiblePlayer == m_owner) {
					g_soundManager->AddSound(SOUNDTYPE_VOICE, (uint32)0,
											gamesounds_GetGameSoundID(GAMESOUNDS_BUILDING_COMPLETE),
											pos.x,
											pos.y);
				}
			}
		}

		return true;
	} else {

		DPRINTF(k_DBG_GAMESTATE, ("BuildFront: City %lx building nothing\n",
								  m_city.m_id));

		if (!Player::IsThisPlayerARobot(m_owner) ||
			(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))) {
			// Cities with empty queues lose the same switch penalty % of their shield store
			// each turn they are empty.

			if (shieldstore > 0)
			{
				sint32 s = 0;
				double penalty =
					static_cast<double>(g_theConstDB->Get(0)->GetChangeCurrentlyBuildingItemPenalty());

				penalty = std::min<double>
								  (
								   1.0,
								   std::max<double>(0.0, 1.0 - penalty * 0.01)
								  );

				s = static_cast<sint32>(static_cast<double>(shieldstore) * penalty);
				DPRINTF(k_DBG_GAMESTATE, ("Deducting %i shields for empty queue in city of %lx\n", s, m_city.m_id));
				shieldstore = s;
			}
			assert(shieldstore >= 0);
		}
		return false;
	}
}

void BuildQueue::FinishBuildFront(Unit &u)
{
	CityData *cd = m_city.CD();
	sint32 rollOverShields = 0;
	if (m_list->GetHead() && m_list->GetHead() == m_frontWhenBuilt)
	{

		if (cd->GetStoredCityProduction() > m_list->GetHead()->m_cost)
			rollOverShields = (cd->GetStoredCityProduction() - m_list->GetHead()->m_cost);

		HandleProductionComplete();

		bool            isEmpty     = m_list->GetCount() <= 1;
		bool            doRemove    = false;
		SlicObject *    so          = NULL;

		if ((m_list->GetHead()->m_category != k_GAME_OBJ_TYPE_UNIT)         ||
		    !isEmpty                                                        ||
		    g_theUnitDB->Get(m_list->GetHead()->m_type)->GetOnlyBuildOne()  ||
		    Player::IsThisPlayerARobot(m_owner)                             ||
		    cd->GetUseGovernor()
		   )
		{
			doRemove = true;

			if (isEmpty                                                 &&
			    (m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT) &&
			    Player::IsThisPlayerARobot(m_owner)                     &&
			    g_network.IsClient()                                    &&
			    g_network.IsLocalPlayer(m_owner)                        &&
			    !g_theUnitDB->Get(m_list->GetHead()->m_type)->GetOnlyBuildOne()
			   )
			{
				g_network.SendAction(new NetAction(NET_ACTION_REMOVE_BUILD_ITEM,
												   (uint32)m_city, 0,
												   CAUSE_REMOVE_BUILD_ITEM_BUILT));
			}

			switch(m_list->GetHead()->m_category) {
				case k_GAME_OBJ_TYPE_UNIT:
					if(g_theProfileDB->IsAllUnitCompleteMessages() ||
					   (g_theProfileDB->IsNonContinuousUnitCompleteMessages() &&
						u.GetDBRec()->GetOnlyBuildOne()) || isEmpty) {
						if(isEmpty) {
							so = new SlicObject("38UnitCompletedQueueEmpty");
						} else {
							so = new SlicObject("38UnitCompleted");
						}

						so->AddUnit(u);
					}
					break;
				case k_GAME_OBJ_TYPE_IMPROVEMENT:
					if(isEmpty) {
						so = new SlicObject("38BuildingBuiltQueueEmpty");
					} else {
						so = new SlicObject("38BuildingBuilt");
					}
					so->AddBuilding(m_list->GetHead()->m_type);
					break;
				case k_GAME_OBJ_TYPE_WONDER:
					if(isEmpty) {
						so = new SlicObject("38WonderBuiltQueueEmpty");
					} else {
						so = new SlicObject("38WonderBuilt");
					}
					so->AddWonder(m_list->GetHead()->m_type);
					break;
				case k_GAME_OBJ_TYPE_ENDGAME_OBJECT:
					if(isEmpty) {
						so = new SlicObject("38EndgameBuiltQueueEmpty");
					} else {
						so = new SlicObject("38EndgameBuilt");
					}
	//				so->AddAction(g_theStringDB->GetNameStr(g_theEndGameDB->Get(m_list->GetHead()->m_type)->m_name));
					break;
				default:
					Assert(FALSE);
					break;
			}
		} else {

			if(g_theProfileDB->IsAllUnitCompleteMessages() ||
			   (g_theProfileDB->IsNonContinuousUnitCompleteMessages() &&
				g_theUnitDB->Get(m_list->GetHead()->m_type)->GetOnlyBuildOne())) {
				so = new SlicObject("38UnitCompleted");

				so->AddUnit(u);
			}
			m_list->GetHead()->m_flags = 0;
		}

		if(so) {

			so->AddCity(m_city);
			so->AddRecipient(m_owner);

			BuildNode *next = NULL;
			if(m_list->GetHeadNode()->GetNext()) {
				next = m_list->GetHeadNode()->GetNext()->GetObj();
			}
			if(next) {

				switch(next->m_category) {
					case k_GAME_OBJ_TYPE_UNIT:
						so->AddAction(g_theStringDB->GetNameStr(g_theUnitDB->Get(next->m_type)->m_name));
						break;
					case k_GAME_OBJ_TYPE_IMPROVEMENT:
						so->AddAction(g_theStringDB->GetNameStr(g_theBuildingDB->Get(next->m_type)->m_name));
						break;
					case k_GAME_OBJ_TYPE_WONDER:
						so->AddAction(g_theStringDB->GetNameStr(g_theWonderDB->Get(next->m_type)->m_name));
						break;
					case k_GAME_OBJ_TYPE_ENDGAME_OBJECT:
//						so->AddAction(g_theStringDB->GetNameStr(g_theEndGameDB->Get(next->m_type)->m_name));
						break;
					case k_GAME_OBJ_TYPE_CAPITALIZATION:
						so->AddAction(g_theStringDB->GetNameStr("CAPITALIZATION"));
						break;
					case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
						so->AddAction(g_theStringDB->GetNameStr("INFRASTRUCTURE"));
						break;
				}
			} else if(m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT &&
					  !g_theUnitDB->Get(m_list->GetHead()->m_type)->GetOnlyBuildOne()) {
				so->AddAction(g_theStringDB->GetNameStr(g_theUnitDB->Get(m_list->GetHead()->m_type)->m_name));
			}

			if(u.IsValid())
				so->AddUnit(u);
			g_slicEngine->Execute(so);
			m_city.AccessData()->GetCityData()->SetSentInefficientMessage();
		}

		if (isEmpty)
			// Reset build category if something was built and there is nothing
			// next in the queue. This is so a switch penalty is not added when a
			// new item is added to an empty queue with production roll over on.
			cd->SetBuildCategoryAtBeginTurn(-5);// set no penalty type after
		                                        // completing an item in an empty
		                                        // queue.

		if(doRemove)
			RemoveHead();
	}

	// Check special actions for the (new) first item
	HandleProductionStart();

	if(g_network.IsHost()) {
		g_network.Block(cd->GetHomeCity().GetOwner());
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILT_FRONT,
									  (uint32)cd->GetHomeCity()));
		g_network.Unblock(cd->GetHomeCity().GetOwner());
	}

	//DPRINTF(k_DBG_GAMESTATE, ("Setting shieldstore to 0 for %lx\n", m_city.m_id));
	//g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ZeroProduction,
	//					   GEA_City, m_city,
	//					   GEA_End);

	DPRINTF(k_DBG_GAMESTATE, ("Setting rollover shieldstore to %d for %lx\n", rollOverShields, m_city.m_id));
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_RollOverProduction,
						   GEA_City, m_city,
						   GEA_Int, rollOverShields,
						   GEA_End);

	m_frontWhenBuilt = NULL;
}

//----------------------------------------------------------------------------
//
// Name       : HandleProductionComplete
//
// Description: Check whether to do anything special when completing a
//              production item.
//
// Parameters : -
//
// Globals    : g_theWonderTracker  : updated when a wonder is completed
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void BuildQueue::HandleProductionComplete(void)
{
	BuildNode * head	= m_list->GetHead();

	if (head)
	{
		switch (head->m_category)
		{
		default:
			// No special actions
			break;

		case k_GAME_OBJ_TYPE_WONDER:
			Assert(m_wonderComplete == NOTHING_THIS_TURN);
			m_wonderComplete = head->m_type;
			// Completion overrides start reporting
			if (m_wonderStarted == m_wonderComplete)
			{
				m_wonderStarted = NOTHING_THIS_TURN;
			}
			SendMsgWonderComplete(m_city.CD(), m_wonderComplete);
			g_theWonderTracker->ClearBuildingWonder(m_wonderComplete, m_owner);
			break;

#if defined(CTP1_HAS_RISEN_FROM_THE_GRAVE)
		case k_GAME_OBJ_TYPE_UNIT:
			if (g_theUnitDB->Get(head->m_type)->GetWormholeProbe())
			{
				SendMsgWormholeProbeComplete() ;
			}
			break;
#endif
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : HandleProductionStart
//
// Description: Check whether to do anything special when switching to a new
//              production item.
//
// Parameters : -
//
// Globals    : g_theWonderTracker  : updated when a wonder is started
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void BuildQueue::HandleProductionStart(void)
{
	BuildNode * newHead = m_list->GetHead();

	if (newHead)
	{
		switch (newHead->m_category)
		{
		default:
			// No special actions
			break;

		case k_GAME_OBJ_TYPE_WONDER:
			if (m_wonderStopped == newHead->m_type)
			{
				m_wonderStopped = NOTHING_THIS_TURN;	// resume stopped
			}
			else
			{
				m_wonderStarted = newHead->m_type;		// start new
			}
			g_theWonderTracker->SetBuildingWonder(newHead->m_type, m_owner);
			break;

#if defined(CTP1_HAS_RISEN_FROM_THE_GRAVE)
		case k_GAME_OBJ_TYPE_UNIT:
			SendMsgWormholeProbeStarted();
			break;
#endif

		case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
			m_city.CD()->BuildInfrastructure();
			break;

		case k_GAME_OBJ_TYPE_CAPITALIZATION:
			m_city.CD()->BuildCapitalization();
			break;
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : HandleProductionStop
//
// Description: Check whether to do anything special when switching to a new
//              production item.
//
// Parameters : -
//
// Globals    : g_theWonderTracker  : updated when a wonder is stopped
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void BuildQueue::HandleProductionStop(void)
{
	BuildNode * oldHead = m_list->GetHead();

	if (oldHead)
	{
		switch (oldHead->m_category)
		{
		default:
			// No special actions
			break;

		case k_GAME_OBJ_TYPE_WONDER:
			if (m_wonderStarted == oldHead->m_type)
			{
				m_wonderStarted = NOTHING_THIS_TURN;	// undo start
			}
			else
			{
				m_wonderStopped = oldHead->m_type;		// stop
			}
			g_theWonderTracker->ClearBuildingWonder(oldHead->m_type, m_owner);
			break;

#if defined(CTP1_HAS_RISEN_FROM_THE_GRAVE)
		case k_GAME_OBJ_TYPE_UNIT:
			// SendMsgWormholeProbeStopped() does not exist
			break;
#endif
		}
	}
}

void BuildQueue::SendMsgWonderComplete(CityData *cd, sint32 wonder)
{
	SendMsgWonderCompleteOwner(cd, wonder) ;
	SendMsgWonderCompleteEveryone(cd, wonder) ;
}

void BuildQueue::SendMsgWonderCompleteOwner(CityData *cd, sint32 wonder)
{
	SlicObject *so = new SlicObject("46WonderComplete") ;
	so->AddCivilisation(m_owner) ;
	so->AddWonder(wonder) ;
	so->AddCity(cd->GetHomeCity()) ;
	so->AddRecipient(m_owner) ;
	g_slicEngine->Execute(so) ;
}

void BuildQueue::SendMsgWonderCompleteEveryone(CityData *cd, sint32 wonder)
{
	SlicObject *so = new SlicObject("47WonderComplete") ;
	so->AddCivilisation(m_owner) ;
	so->AddWonder(wonder) ;
	so->AddCity(cd->GetHomeCity()) ;
	so->AddAllRecipientsBut(m_owner);
	g_slicEngine->Execute(so) ;
}

void BuildQueue::SendMsgWonderStopped(sint32 type)
{
	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_WONDER_STOPPED,
									  m_owner, type));
		g_network.Unblock(m_owner);
	}

	SlicObject *    so = new SlicObject("44aWonderStopped") ;
	so->AddCivilisation(m_owner) ;
	so->AddWonder(type) ;
	so->AddAllRecipientsBut(m_owner);
	g_slicEngine->Execute(so) ;
}

void BuildQueue::SendMsgWonderAlmostComplete(void)
{
	SlicObject *    so = new SlicObject("45WonderAlmostFinished") ;
	so->AddCivilisation(m_owner) ;
	so->AddWonder(m_list->GetHead()->m_type) ;
	so->AddAllRecipientsBut(m_owner);
	g_slicEngine->Execute(so) ;
}

void BuildQueue::SendMsgWonderStarted(sint32 type)
{
	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_WONDER_STARTED,
									  m_owner, type));
		g_network.Unblock(m_owner);
	}

	SlicObject *    so = new SlicObject("44WonderStarted") ;
	so->AddCivilisation(m_owner) ;
	so->AddWonder(type) ;
	so->AddAllRecipientsBut(m_owner);
	g_slicEngine->Execute(so) ;
}

#if defined(CTP1_HAS_RISEN_FROM_THE_GRAVE)

void BuildQueue::SendMsgWormholeProbeStarted(void)
{
	if (!m_list->GetHead()) return;

	if (m_list->GetHead()->m_category != k_GAME_OBJ_TYPE_UNIT)
		return ;

	if (!g_theUnitDB->Get(m_list->GetHead()->m_type)->GetWormholeProbe())
		return ;

	SlicObject * so = new SlicObject("63WormholeProbeStarted") ;
	so->AddCivilisation(m_owner) ;
	so->AddUnit(m_list->GetHead()->m_type) ;
	so->AddAllRecipientsBut(m_owner);
	g_slicEngine->Execute(so) ;
}

void BuildQueue::SendMsgWormholeProbeComplete(void)
{
	SlicObject	*so = new SlicObject("64WormholeProbeCompleted") ;
	so->AddCivilisation(m_owner) ;
	so->AddUnit(m_list->GetHead()->m_type) ;
	so->AddAllRecipientsBut(m_owner);
	g_slicEngine->Execute(so) ;
}

#endif // CTP1_HAS_RISEN_FROM_THE_GRAVE

void BuildQueue::RemoveHead()
{
	Assert(m_list->GetHead());
	if (m_list->GetHead())
	{
		delete m_list->RemoveHead();

		// Check the new head, if any
		HandleProductionStart();
	}
	// else: No action: queue was empty
}

void BuildQueue::RawInsertTail(sint32 cat, sint32 t, sint32 cost)
{
	if (m_city.IsValid())
	{
		switch(cat) {
			case k_GAME_OBJ_TYPE_UNIT:
				if(!m_city.CanBuildUnit(t))
					return;
				break;
			case k_GAME_OBJ_TYPE_IMPROVEMENT:
				if(!m_city.CanBuildBuilding(t))
					return;
				break;
			case k_GAME_OBJ_TYPE_WONDER:
				if(!wonderutil_IsAvailable(t, m_owner))
					return;
				break;
			case k_GAME_OBJ_TYPE_CAPITALIZATION:
				if(!m_city.CanBuildCapitalization())
					return;
				break;
			case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
				if(!m_city.CD()->CanBuildInfrastructure())
					return;
				break;
			default:
				Assert(FALSE);
				return;
		}
		sint32 o = m_city.GetOwner();
		if(g_player[o]->IsRobot() &&
			!(g_network.IsClient() && g_network.IsLocalPlayer(o))) {
			sint32 age = 0;
			cost = static_cast<sint32>(static_cast<double>(cost) *
				diffutil_GetAiProductionCostAdjustment(g_theGameSettings->GetDifficulty(), o, age));
		}
	}

	BuildNode *newNode = new BuildNode;
	newNode->m_category = cat;
	newNode->m_type = t;
	newNode->m_cost = cost;
	newNode->m_flags = 0;
	m_list->AddTail(newNode);
}

// This is not used.
void BuildQueue::ReplaceHead(sint32 cat, sint32 t, sint32 cost)
{
	BuildNode * oldHead = m_list->GetHead();
	if (oldHead)
		{
		HandleProductionStop();

		oldHead->m_category = cat;
		oldHead->m_type     = t;

		sint32 o = m_city.GetOwner();
		if(g_player[o]->IsRobot() &&
			!(g_network.IsClient() && g_network.IsLocalPlayer(o))) {
			sint32 age = 0;
			cost = static_cast<sint32>(static_cast<double>(cost) *
				diffutil_GetAiProductionCostAdjustment(g_theGameSettings->GetDifficulty(), o, age));
		}
		oldHead->m_cost     = cost;

		HandleProductionStart();

		if (g_soundManager && (g_selected_item->GetVisiblePlayer() == m_owner))
		{
			g_soundManager->AddSound(
			                         SOUNDTYPE_VOICE,
			                         (uint32)0,
			                         gamesounds_GetGameSoundID(GAMESOUNDS_BUILDING_STARTED),
			                         0,
			                         0
			                        );
			}
		}
	else
	{
		InsertTail(cat, t, cost);
	}

	if(cat == k_GAME_OBJ_TYPE_WONDER ||
	   cat == k_GAME_OBJ_TYPE_IMPROVEMENT) {
		PointerList<BuildNode>::Walker walk(m_list);
		if(walk.IsValid()) {

			walk.Next();
		}
		while(walk.IsValid()) {
			BuildNode *node = walk.GetObj();
			if(node->m_category == m_list->GetHead()->m_category &&
			   node->m_type == m_list->GetHead()->m_type) {
				RemoveNode(node, CAUSE_REMOVE_BUILD_ITEM_ILLEGAL);
				break;
			} else {
				walk.Next();
			}
		}
	}
}

bool BuildQueue::DoInsertChecks(sint32 cat, sint32 t, sint32 cost)
{
	switch(cat)
	{
		default:
			// No insert checks
			break;

		case k_GAME_OBJ_TYPE_UNIT:
		{
			if(!m_city.CanBuildUnit(t))
			{
				return false;
			}

#if defined(CTP1_HAS_RISEN_FROM_THE_GRAVE)
			if (!m_list->GetHead())
			{
				SendMsgWormholeProbeStarted();
			}
#endif // CTP1_HAS_RISEN_FROM_THE_GRAVE

		}
			break;

		case k_GAME_OBJ_TYPE_WONDER :
		{
			PointerList<BuildNode>::Walker walk(m_list);
			while(walk.IsValid()) {
				if(walk.GetObj()->m_category == k_GAME_OBJ_TYPE_WONDER &&
				   walk.GetObj()->m_type == t) {
					return false;
				}
				walk.Next();
			}

			// This is the only way the game makes a wonder start message to appear.
			// TODO: Make sure that the standart start handling is used.
			//       This looks rather like a head.
			if (!m_list->GetHead()) {
			       m_wonderStarted = t;

				g_theWonderTracker->SetBuildingWonder(m_wonderStarted, m_owner);
			}
		}
		break ;

		case k_GAME_OBJ_TYPE_IMPROVEMENT:
		{
			if(!m_city.CanBuildBuilding(t))
				return false;

			PointerList<BuildNode>::Walker walk(m_list);
			while(walk.IsValid()) {
				if(walk.GetObj()->m_category == k_GAME_OBJ_TYPE_IMPROVEMENT &&
				   walk.GetObj()->m_type == t) {
					return false;
				}
				walk.Next();
			}
		}
			break;
		}

	return true;
}

bool BuildQueue::InsertTail(sint32 cat, sint32 t, sint32 cost)
{
	if(!DoInsertChecks(cat, t, cost))
		return false;

	if (m_list->GetCount() == 0)
	{
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		if ((visiblePlayer == m_owner) && g_soundManager)
		{
			g_soundManager->AddSound(SOUNDTYPE_VOICE, (uint32)0,
									gamesounds_GetGameSoundID(GAMESOUNDS_BUILDING_STARTED),
									0,
									0);
		}

		HandleProductionStart();
	}

	RawInsertTail(cat, t, cost);
	return true;
}

void BuildQueue::ResetOwner(const sint32 new_owner)
{
	Clear();
	m_owner = new_owner;
}

double BuildQueue::GetTypeCoeff() const
{
	if(!m_list->GetHead()) {
		return 1.0;
	}

	const GovernmentRecord *grec = g_theGovernmentDB->Get(g_player[m_owner]->m_government_type);
	switch(m_list->GetHead()->m_category) {
		case k_GAME_OBJ_TYPE_UNIT: return grec->GetUnitRushModifier();
		case k_GAME_OBJ_TYPE_IMPROVEMENT: return grec->GetBuildingRushModifier();
		case k_GAME_OBJ_TYPE_WONDER: return grec->GetWonderRushModifier();
//		case k_GAME_OBJ_TYPE_ENDGAME_OBJECT: return grec->GetEndGameRushModifier();
		default:
			return 1.0;
	}
}

double BuildQueue::GetPercentRemaining(sint32 shieldstore) const
{
	if(!m_list->GetHead())
		return 1.0;

	if (m_list->GetHead()->m_cost == 0)
		return 0.0;

	return ((double) GetProductionRemaining(shieldstore) / (double) m_list->GetHead()->m_cost);
}

sint32 BuildQueue::GetPercentCompleted(sint32 shieldstore) const
{
	if (!m_list->GetHead())
		return 0;

	if (m_list->GetHead()->m_cost)
		return 100 - ((GetProductionRemaining(shieldstore) * 100) / m_list->GetHead()->m_cost);
	else
		return 100;
}

sint32 BuildQueue::GetProductionRemaining(sint32 shieldstore) const
{
	if(!m_list->GetHead())
		return 0x7fffffff;

	return m_list->GetHead()->m_cost - shieldstore;
}

sint32 BuildQueue::GetFrontCost()
{
	if(!m_list->GetHead())
		return 0x7fffffff;

	return m_list->GetHead()->m_cost;
}

void BuildQueue::Dump(const sint32 shieldstore, MBCHAR *s)
{
#if 0
	bool    firstTime = true;

	BuildNode	*node = m_list->GetHead() ;

	*s = NULL ;
	while (node)
		{
		switch (node->m_category)
			{
			case k_GAME_OBJ_TYPE_UNIT :
				strcat(s, g_theStringDB->GetNameStr(g_theUnitDB->Get(node->m_type)->m_name)) ;
				break ;

			case k_GAME_OBJ_TYPE_POP :
				strcat(s, "Pop") ;
				break ;

			case k_GAME_OBJ_TYPE_IMPROVEMENT :
				strcat(s, g_theStringDB->GetNameStr(g_theBuildingDB->Get(node->m_type)->m_name)) ;
				break ;

			case k_GAME_OBJ_TYPE_WONDER :
				strcat(s, g_theStringDB->GetNameStr(wonderutil_Get(node->m_type)->m_name)) ;
				break ;
			case k_GAME_OBJ_TYPE_ENDGAME_OBJECT:
//				strcat(s, g_theStringDB->GetNameStr(g_theEndGameDB->Get(node->m_type)->m_name));
//				break;

			default :
				Assert(false);
				break ;

			}

		if (firstTime)
			{
			sprintf(s, "%s(%d/%d), ", s, node->m_cost, node->m_cost - shieldstore) ;
			firstTime = false;
			}
		else
			sprintf(s, "%s(%d), ", s, node->m_cost) ;

		node = node->m_next ;
		}

	s[strlen(s) - 2] = NULL ;
#endif
}

void BuildQueue::SetCity(Unit &city)
{
	m_city = city;
}

bool BuildQueue::RemoveNode( BuildNode *node, CAUSE_REMOVE_BUILD_ITEM cause )
{
	if (!node) return false;
	bool found = false;
	sint32 index = 0;

	PointerList<BuildNode>::Walker walk(m_list);

	while(walk.IsValid()) {
		if(walk.GetObj() == node) {

			if(walk.GetObj() == m_list->GetHead()) {
				if (m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_WONDER) {
					if (cause == CAUSE_REMOVE_BUILD_ITEM_ILLEGAL) {


						g_theWonderTracker->ClearBuildingWonder(m_list->GetHead()->m_type, m_owner);
					} else {
						m_wonderStopped = m_list->GetHead()->m_type;

						g_theWonderTracker->ClearBuildingWonder(m_wonderStopped, m_owner);
					}
				}
			}

			found = true;
			delete walk.Remove();
			break;
		} else {
			walk.Next();
			index++;
		}
	}

	if(found && cause == CAUSE_REMOVE_BUILD_ITEM_MANUAL) {
		if(g_network.IsClient()) {
			if(g_network.IsLocalPlayer(m_city.GetOwner())) {
				g_network.SendAction(new NetAction(NET_ACTION_REMOVE_BUILD_ITEM,
												   (uint32)m_city, index,
												   cause));
			}
		} else if(g_network.IsHost()) {
			g_network.Block(m_city.GetOwner());
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_REMOVE_BUILD_ITEM,
										  (uint32)m_city, index));
			g_network.Unblock(m_city.GetOwner());
		}
	}

	return true;
}

bool BuildQueue::InsertAfter( BuildNode *targetNode, BuildNode *node )
{
	if(!node) return false;
	if(!targetNode) return false;

	PointerList<BuildNode>::Walker walk(m_list);
	while(walk.IsValid()) {
		if(walk.GetObj() == targetNode) {
			walk.InsertAfter(node);
			return true;
		}
		walk.Next();
	}

	return false;
}

bool BuildQueue::InsertIndex(sint32 index, BuildNode *node)
{
	if (!node) return false;

	sint32 count = 0;

	if(index >= m_list->GetCount()) {
		m_list->AddTail(node);
	} else {
		PointerList<BuildNode>::Walker walk(m_list);
		while(walk.IsValid()) {
			if(count == index) {
				walk.Insert(node);
				break;
			}
			walk.Next();
			count++;
		}
	}
	return true;
}

bool BuildQueue::RemoveNodeByIndex(sint32 index,
									 CAUSE_REMOVE_BUILD_ITEM cause)
{
	sint32 count = 0;
	PointerList<BuildNode>::Walker walk(m_list);
	bool found = false;

	if (index == 0 && m_list->GetHead()) {
		RemoveHead();
		found = true;
	}
	else {
		while(walk.IsValid()) {
			if(count == index) {
				delete walk.Remove();
				found = true;
				break;
			}
			count++;
			walk.Next();
		}
	}

	if(!found) {
		if(g_network.IsHost() && !g_network.IsLocalPlayer(m_owner)) {
			g_network.Resync(m_owner);
		} else if(g_network.IsClient()) {
			g_network.RequestResync(RESYNC_BAD_BUILD_NODE);
		}
		return false;
	}

	return true;
}

void BuildQueue::SetName(MBCHAR *name)
{
	delete[] m_name;

	if(name != NULL)
	{
		m_name = new MBCHAR[strlen(name) + 1];
		strcpy(m_name, name);
	}
	else
	{
		m_name = NULL;
	}
}

void BuildQueue::RemoveObjectsOfType(sint32 cat, sint32 type,
									 CAUSE_REMOVE_BUILD_ITEM cause)
{
	BuildNode *bn;
	PointerList<BuildNode>::Walker walk(m_list);
	while(walk.IsValid()) {
		bn = walk.GetObj();
		if(bn->m_category == cat &&
		   bn->m_type == type) {
			delete walk.Remove();
		} else {
			walk.Next();
		}
	}
}

void BuildQueue::RemoveIllegalItems(bool isClientAck)
{
	if(g_network.IsHost() && !g_network.IsLocalPlayer(m_owner) &&
	   !isClientAck) {
		g_network.QueuePacket(g_network.IndexToId(m_owner),
							  new NetInfo(NET_INFO_CODE_REMOVE_ILLEGAL_ITEMS,
										  m_city.m_id));
		return;
	}

	PointerList<BuildNode>::Walker walk(m_list);
	BuildNode *bn;
	while(walk.IsValid()) {
		bn = walk.GetObj();


		walk.Next();

		if(!(bn->m_flags & k_BUILD_NODE_FLAG_ALREADY_BUILT)) {
			switch(bn->m_category) {
				case k_GAME_OBJ_TYPE_UNIT:
					if(!m_city.CanBuildUnit(bn->m_type)) {
						RemoveNode(bn, CAUSE_REMOVE_BUILD_ITEM_ILLEGAL);
					}
					break;
				case k_GAME_OBJ_TYPE_WONDER:

					if(!m_city.CanBuildWonder(bn->m_type)) {
						RemoveNode(bn, CAUSE_REMOVE_BUILD_ITEM_ILLEGAL);
					}
					break;
				case k_GAME_OBJ_TYPE_IMPROVEMENT:
					if(!m_city.CanBuildBuilding(bn->m_type)) {
						RemoveNode(bn, CAUSE_REMOVE_BUILD_ITEM_ILLEGAL);
					}
					break;
				case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
					if(!m_city.CD()->CanBuildInfrastructure()) {
						RemoveNode(bn, CAUSE_REMOVE_BUILD_ITEM_ILLEGAL);
					}
					break;
				case k_GAME_OBJ_TYPE_CAPITALIZATION:
					if(!m_city.CD()->CanBuildCapitalization()) {
						RemoveNode(bn, CAUSE_REMOVE_BUILD_ITEM_ILLEGAL);
					}
					break;







				default:
					Assert(FALSE);
					break;
			}
		}
	}
}

sint32 BuildQueue::GetCost(sint32 cat, sint32 t)
{
	switch(cat)
	{
		case k_GAME_OBJ_TYPE_UNIT:
			return g_theUnitDB->Get(t)->GetShieldCost();
		case k_GAME_OBJ_TYPE_WONDER :
			return wonderutil_Get(t, m_owner)->GetProductionCost();
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
			//ProductionCostPopModifier m_city.CD()->PopCount()
			//EMOD ProductionCostPopModifier  10-10-2006

				sint32 bcost;

				if(buildingutil_Get(t, m_owner)->GetProductionCostPopModifier())
				{
					bcost = buildingutil_Get(t, m_owner)->GetProductionCost() * m_city.CD()->PopCount();
				}
				else
				{
					bcost = buildingutil_Get(t, m_owner)->GetProductionCost();
				}

				return bcost;
			//return g_theBuildingDB->Get(t, m_owner)->GetProductionCost();
		case k_GAME_OBJ_TYPE_ENDGAME_OBJECT:
//			return g_theEndGameDB->Get(t)->GetCost();
		default:
			return 0x7fffffff;
	}
}

bool BuildQueue::InsertBefore(BuildNode *old,
							  sint32 cat, sint32 t)
{
	sint32 cost = GetCost(cat, t);

	if(!m_list->GetHead()) {
		InsertTail(cat, t, cost);
	}

	if(!DoInsertChecks(cat, t, cost))
		return false;

	PointerList<BuildNode>::Walker walk(m_list);
	while(walk.IsValid()) {
		if(walk.GetObj() == old) {
			BuildNode *ins = new BuildNode;
			ins->m_cost = cost;
			ins->m_category = cat;
			ins->m_type = t;
			ins->m_flags = 0;
			walk.Insert(ins);
			break;
		}
		walk.Next();
	}

	SynchroniseNetworkData();
	return true;
}

void BuildQueue::FinishCreatingUnit(Unit &u)
{
	if(u.m_id != (0)) {
		//g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ZeroProduction,
		//					   GEA_City, m_city,
		//					   GEA_End);

		if(m_frontWhenBuilt && m_list->GetHead() == m_frontWhenBuilt) {
			Assert(m_list->GetHead());
			if(m_list->GetHead()) {
				Assert(m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT);
				if(m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT) {
					Assert(m_list->GetHead()->m_type == u.GetType());
					if(m_list->GetHead()->m_type == u.GetType()) {
						m_list->GetHead()->m_flags |= k_BUILD_NODE_FLAG_ALREADY_BUILT;
					}
				}
			}
		}

		CityData *cd = m_city.CD();
		const UnitRecord *rec = u.GetDBRec();

		if(  !g_player[m_owner]->IsRobot() // The AI is cheating
		|| (  g_network.IsClient()
		&&    g_network.IsLocalPlayer(m_owner)
		||  (!g_network.IsActive()
		&&   !g_theProfileDB->AIPopCheat()
		    )
		   )
		  )
		{
			if(rec->GetBuildingRemovesAPop())
			{
				cd->SubtractAccumulatedFood(static_cast<sint32>(g_theConstDB->Get(0)->GetCityGrowthCoefficient()));
				cd->ChangePopulation(-1);
			}
			// EMOD
			sint32 pop;
			if(rec->GetPopCostsToBuild(pop))
			{
				cd->SubtractAccumulatedFood(static_cast<sint32>(g_theConstDB->Get(0)->GetCityGrowthCoefficient()));
				cd->ChangePopulation(-pop);
			}
		}

		// If cd had building enables veterans
		// then u.SetVeteran();
		uint64 buildings = cd->GetEffectiveBuildings();
		for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++) {
			if(buildingutil_Get(i, m_owner)->GetEnablesAllVeterans()) {
				if((buildings & ((uint64)1 << uint64(i)))) {
					u.SetVeteran();
				}
			}
			if(
			  (buildingutil_Get(i, m_owner)->GetEnablesLandVeterans())
			&&(rec->GetMovementTypeLand() || rec->GetMovementTypeMountain())
			){
				if((buildings & ((uint64)1 << uint64(i)))) {
					u.SetVeteran();
				}
			}

			if(
			  (buildingutil_Get(i, m_owner)->GetEnablesSeaVeterans())
			&&(rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater())
			){
				if((buildings & ((uint64)1 << uint64(i)))) {
					u.SetVeteran();
				}
			}
			//Air Veterans
			if(
			  (buildingutil_Get(i, m_owner)->GetEnablesAirVeterans())
			&&(rec->GetMovementTypeAir())
			){
				if((buildings & ((uint64)1 << uint64(i)))) {
					u.SetVeteran();
				}
			}
			//end special veterans
		}
		//add sea and air veterans

		//end EMOD
		FinishBuildFront(u);
	}
}

sint32 BuildQueue::GetLen() const
{
	return m_list->GetCount();
}

BuildNode *BuildQueue::GetHead() const
{
	return m_list->GetHead();
}

BuildNode *BuildQueue::GetNodeByIndex(sint32 index)
{
	PointerList<BuildNode>::Walker walk(m_list);
	sint32 i = 0;
	while(walk.IsValid() && i < index) {
		walk.Next();
		i++;
	}
	if(!walk.IsValid()) return NULL;
	return walk.GetObj();
}

void BuildQueue::MoveNodeUp(sint32 index)
{
	if(index < 1 || index >= m_list->GetCount())
		return;

	PointerList<BuildNode>::Walker walk(m_list);
	sint32 i = 0;
	while(walk.IsValid() && i < index) {
		walk.Next();
		i++;
	}

	if(walk.IsValid()) {
		walk.MoveUp();
	}

	SynchroniseNetworkData();
}

void BuildQueue::MoveNodeDown(sint32 index)
{
	if(index < 0 || index >= m_list->GetCount() - 1)
		return;

	PointerList<BuildNode>::Walker walk(m_list);
	sint32 i = 0;
	while(walk.IsValid() && i < index) {
		walk.Next();
		i++;
	}

	if(walk.IsValid()) {
		walk.MoveDown();
	}

	SynchroniseNetworkData();
}

bool BuildQueue::IsItemInQueue(uint32 cat, sint32 type)
{
	for
	(
	    PointerList<BuildNode>::Walker walk(m_list);
	    walk.IsValid();
	    walk.Next()
	)
	{
		if(walk.GetObj()->m_category == static_cast<sint32>(cat) &&
		   walk.GetObj()->m_type == type)
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : BuildQueue::operator=
//
// Description: Assignment operator copies an instance of BuildQueue on the
//              right side to the instance on the left side.
//
// Parameters : BuildQueue &copy    : Instance to be copied.
//
// Globals    : -
//
// Returns    : BuildQueue &        : Reference to modified object
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
BuildQueue & BuildQueue::operator = (BuildQueue const & copy)
{
	if (this != &copy)
	{
		m_owner          = copy.m_owner;
		m_city           = copy.m_city;
		m_wonderStarted  = copy.m_wonderStarted;
		m_wonderStopped  = copy.m_wonderStopped;
		m_list->DeleteAll();

		for
		(
		    PointerList<BuildNode>::Walker walk(copy.m_list);
		                                   walk.IsValid();
		                                   walk.Next()
		)
		{
			BuildNode* destBN = new BuildNode();
			BuildNode* sourBN = walk.GetObj();
			memcpy(destBN, sourBN, sizeof(BuildNode));
			m_list->AddTail(destBN);
		}
	}

	return *this;
}

//----------------------------------------------------------------------------
//
// Name       : BuildQueue::SynchroniseNetworkData
//
// Description: Update the (multiplayer) network data with the latest changes.
//
// Parameters : -
//
// Globals    : g_network           : network settings
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void BuildQueue::SynchroniseNetworkData(void) const
{
	if (g_network.IsHost())
	{
		g_network.Enqueue(m_city.AccessData()->GetCityData());
	}
	else if(g_network.IsClient())
	{
		g_network.SendBuildQueue(m_city.AccessData()->GetCityData());
	}
	// else: No action: Single player game.
}

bool BuildQueue::IsHeadUnit() const
{
	return m_list->GetHead() != NULL ? m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT : false;
}

sint32 BuildQueue::GetHeadType() const
{
	return m_list->GetHead()->m_type;
}
