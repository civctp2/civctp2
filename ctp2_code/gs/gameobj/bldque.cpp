//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Build queue handling
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
// - Allow infastructure and capitalisation when loading the build queue
//   (modification as posted by Peter Triggs).
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3errors.h"
#include "c3files.h"

#include "GameFile.h"
#include "BldQue.h"
#include "ObjPool.h"
#include "Unit.h"
#include "MapPoint.h"
#include "Player.h"
#include "CityData.h"
#include "CivArchive.h"
#include "BuildingRecord.h"
#include "AiCause.h"
#include "Checksum.h"
#include "CivPaths.h"
#include "WonderRecord.h"
#include "StrDB.h"
#include "network.h"
#include "net_info.h"
#include "net_action.h"
#include "ConstDB.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "QuickSlic.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"
#include "SoundManager.h"
#include "gamesounds.h"
#include "SelItem.h"

#include "UnitData.h"
#include "UnitPool.h"

#include "profiledb.h"

#include "WonderTracker.h"
#include "wondermoviewin.h"
#include "EndGameDB.h"
#include "DiffDB.h"

#include "controlpanelwindow.h"

#include "director.h"

#include "GameSettings.h"
#include "AgeRecord.h"
#include "GovernmentRecord.h"

#include "UnitRecord.h"

#include "GameEventManager.h"


#include "Globals.h"

#include "buildingutil.h"
#include "wonderutil.h"

extern	ControlPanelWindow	*g_controlPanel;
extern	CivPaths	*g_civPaths ;
extern	SoundManager *g_soundManager;
extern DifficultyDB     *g_theDifficultyDB;

#define k_BUILDQUEUE_VERSION_MAJOR	0									
#define k_BUILDQUEUE_VERSION_MINOR	1									

extern void player_ActivateSpaceButton(sint32 pl);

BuildQueue::BuildQueue()
{ 
    m_wonderStarted = 0;
    m_wonderStopped = 0;
	m_wonderComplete = 0;
    m_settler_pending = FALSE;
	m_list = new PointerList<BuildNode>;
	m_frontWhenBuilt = NULL;

	strcpy(m_name,"");
} 

BuildQueue::~BuildQueue()
{ 
	if(m_list) {
		m_list->DeleteAll();
		delete m_list;
		m_list = NULL;
	}
}













void BuildQueue::Serialize(CivArchive &archive)
{
	sint32 c;

	if(archive.IsStoring()) {
		archive.StoreChunk((uint8*)&m_owner, (uint8*)&m_name + sizeof(m_name));
		archive << m_wonderComplete;
		c = m_list->GetCount();
		archive << c;
		PointerList<BuildNode>::Walker walk(m_list);
		while(walk.IsValid()) {
			archive.Store((uint8*)walk.GetObj(), sizeof(BuildNode));
			walk.Next();
		}
	} else {
		archive.LoadChunk((uint8*)&m_owner, (uint8*)&m_name + sizeof(m_name));
		if(g_saveFileVersion >= 62) {
			archive >> m_wonderComplete;
		} else {
			m_wonderComplete = 0;
		}

		m_list->DeleteAll();
		archive >> c;
		sint32 i;
		for(i = 0; i < c; i++) {
			BuildNode *node = new BuildNode;
			archive.Load((uint8*)node, sizeof(BuildNode));
			m_list->AddTail(node);
		}
	}
}





void BuildQueue::SerializeQueue(CivArchive &archive)
{
	uint16 len;
	if(archive.IsStoring()) {
		len = strlen(m_name);
		archive << len;
		archive.Store((uint8*)m_name, len * sizeof(MBCHAR));

		len = m_list->GetCount();
		archive << len;
		PointerList<BuildNode>::Walker walk(m_list);
		while(walk.IsValid()) {
			archive.Store((uint8*)walk.GetObj(), sizeof(BuildNode));
			walk.Next();
		}
	} else {
		archive >> len;
		Assert(len < (sizeof(m_name) / sizeof(MBCHAR)));
		archive.Load((uint8*)m_name, len * sizeof(MBCHAR));
		
		archive >> len;
		for(sint32 i = 0; i < len; i++) {
			BuildNode *newNode = new BuildNode;
			archive.Load((uint8*)newNode, sizeof(BuildNode));
			m_list->AddTail(newNode);
		}
	}
}













uint32 BldQue_BuildQueue_GetVersion(void)
{
	return (k_BUILDQUEUE_VERSION_MAJOR<<16 | k_BUILDQUEUE_VERSION_MINOR) ;
}




















sint32 BuildQueue::Load(const MBCHAR *file)
{
	FILE	*fpQueue ;

	
	if(m_city.CD()->AlreadyBoughtFront()) {
		ClearAllButHead();
	} else {
		Clear();
	}
	fpQueue = c3files_fopen(C3DIR_DIRECT, (char *)file, "r");
	if(!fpQueue) return 0;

	char buf[k_MAX_NAME_LEN];
	sint32 category;
	sint32 type;
	sint32 cost;
	while(!c3files_feof(fpQueue)) {
		
		if(!c3files_fgets(buf, k_MAX_NAME_LEN, fpQueue))
			continue;

		buf[strlen(buf) - 1] = 0; 

		
		switch(buf[0]) {
			case 'U':
				category = k_GAME_OBJ_TYPE_UNIT;
				if(!g_theUnitDB->GetNamedItem(&buf[2], type)) {
					Assert(FALSE);
					continue;
				}
				cost = g_theUnitDB->Get(type)->GetShieldCost();
				break;
			case 'B':
				category = k_GAME_OBJ_TYPE_IMPROVEMENT;
				if(!g_theBuildingDB->GetNamedItem(&buf[2], type)) {
					Assert(FALSE);
					continue;
				}
				cost = g_theBuildingDB->Get(type)->GetProductionCost();
				break;
			case 'W':
				category = k_GAME_OBJ_TYPE_WONDER;
				if(!g_theWonderDB->GetNamedItem(&buf[2], type)) {
					Assert(FALSE);
					continue;
				}
				cost = g_theWonderDB->Get(type)->GetProductionCost();
				break;
			case '#':
				
				continue;
#if !defined(ACTIVISION_ORIGINAL)
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
#endif				
			default:
				Assert(FALSE);
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
	FILE *fpQueue;
	fpQueue = c3files_fopen(C3DIR_DIRECT, (char *)file, "w");
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
	return (1);
}

	









void BuildQueue::EndTurn(void)
{
    
    if (m_wonderStarted != m_wonderStopped) {
        if (m_wonderStopped) {
            SendMsgWonderStopped(m_wonderStopped) ;
            g_theWonderTracker->ClearBuildingWonder(m_wonderStopped, m_owner);
			
        }
        if (m_wonderStarted != m_wonderComplete) {
            SendMsgWonderStarted(m_wonderStarted) ;
            g_theWonderTracker->SetBuildingWonder(m_wonderStarted, m_owner);
            
        }
    }        
    m_wonderStarted = 0;
    m_wonderStopped = 0;
    m_settler_pending = FALSE;
}










void BuildQueue::Clear(BOOL fromServer)

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
										   
    while (m_list->GetHead()) {
		if (m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_WONDER)
		{
            m_wonderStopped = m_list->GetHead()->m_type;
			
			g_theWonderTracker->ClearBuildingWonder(m_wonderStopped, m_owner);
		}

		delete m_list->RemoveHead();
    } 

	Assert(m_list->GetCount() == 0);
}

void BuildQueue::ClearAllButHead(BOOL fromServer)
{
	if(m_list->GetCount() < 2)
		return;

	if(!fromServer && g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_CLEAR_QUEUE_EXCEPT_HEAD));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_CLEAR_QUEUE_EXCEPT_HEAD,
									  (uint32)m_city));
		g_network.Unblock(m_owner);
	}

	
	
	BuildNode *head = m_list->RemoveHead();
	m_list->DeleteAll();
	m_list->AddHead(head);
}

bool BuildQueue::BuildFrontUnit(BOOL forceFinish)
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

		if(g_theUnitDB->Get(m_list->GetHead()->m_type)->GetBuildingRemovesAPop() &&
		   cd->PopCount() < 2 &&
		   !forceFinish) {
			if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
			   (g_network.IsClient() && g_network.IsLocalPlayer(m_owner))) {
				m_settler_pending = TRUE;
				return false;
			}
		}

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
	
	if(wonderutil_IsBuilt(m_list->GetHead()->m_type)) {
		RemoveHead();
		return false;
	}

	if(m_list->GetHead()->m_cost <= m_city.CD()->GetStoredCityProduction()) {
		DPRINTF(k_DBG_GAMESTATE, ("City %lx built wonder: %s\n",
								  (uint32)m_city,
								  g_theStringDB->GetNameStr(wonderutil_Get(m_list->GetHead()->m_type)->GetName())));

		m_list->GetHead()->m_flags |= k_BUILD_NODE_FLAG_ALREADY_BUILT;
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_CreateWonder,
							   GEA_City, m_city.m_id,
							   GEA_Int, m_list->GetHead()->m_type,
							   GEA_End);
	
		wonderutil_AddBuilt(m_list->GetHead()->m_type);

		return true;
	} else {
		
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
	if(m_list->GetHead()->m_cost <= m_city.CD()->GetStoredCityProduction()) {

		
		
		m_list->GetHead()->m_flags |= k_BUILD_NODE_FLAG_ALREADY_BUILT;
		DPRINTF(k_DBG_GAMESTATE, ("City %lx built endgame object type %d\n",
								  m_city, m_list->GetHead()->m_type));
		g_player[m_owner]->AddEndGameObject(m_city, m_list->GetHead()->m_type);
		g_player[m_owner]->RegisterCreateBuilding(m_city, 
														 m_list->GetHead()->m_type); 
		return true;
	} else {
		return false;
	}
}


bool BuildQueue::BuildFront(sint32 &shieldstore, CityData *cd, const MapPoint &pos, 
                           uint64 &built_improvements, uint64 &built_wonders,
						   BOOL forceFinish)
{
    bool is_something_built = FALSE; 

    m_settler_pending = FALSE;

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
			DPRINTF(k_DBG_GAMESTATE, ("Setting shieldstore to 0 for %lx\n", m_city.m_id));
    		shieldstore = 0;
        }
        return false;
    }
  
}

void BuildQueue::FinishBuildFront(Unit &u)
{
	CityData *cd = m_city.CD();
	if(m_list->GetHead() && m_list->GetHead() == m_frontWhenBuilt) {
		
		if (m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_WONDER) {
			SendMsgWonderComplete(cd, m_list->GetHead()->m_type) ;
			m_wonderComplete = m_list->GetHead()->m_type;
			g_theWonderTracker->ClearBuildingWonder(m_list->GetHead()->m_type, m_owner);
		} else if (m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT) {	
			if (g_theUnitDB->Get(m_list->GetHead()->m_type)->GetWormholeProbe()) {	
				SendMsgWormholeProbeComplete() ;
			}
		}
			
		
		BOOL isEmpty = m_list->GetCount() < 2;
		BOOL doRemove = FALSE;

		
		
		SlicObject *so = NULL;

		
		
		
		
		if(m_list->GetHead()->m_category != k_GAME_OBJ_TYPE_UNIT ||
		   (m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT && 
			((m_list->GetCount() >= 2) || g_theUnitDB->Get(m_list->GetHead()->m_type)->GetOnlyBuildOne()))  || 
			Player::IsThisPlayerARobot(m_owner) || cd->GetUseGovernor()) {

			
			doRemove = TRUE;

			
			
			
			if(m_list->GetCount() == 1 && m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT &&
				g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT && 
			   g_network.IsClient() && g_network.IsLocalPlayer(m_owner) &&
			   !g_theUnitDB->Get(m_list->GetHead()->m_type)->GetOnlyBuildOne()) {
				
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
					so->AddAction(g_theStringDB->GetNameStr(g_theEndGameDB->Get(m_list->GetHead()->m_type)->m_name));
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
						so->AddAction(g_theStringDB->GetNameStr(wonderutil_Get(next->m_type)->m_name));
						break;
					case k_GAME_OBJ_TYPE_ENDGAME_OBJECT:
						so->AddAction(g_theStringDB->GetNameStr(g_theEndGameDB->Get(next->m_type)->m_name));
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

		
		if(doRemove)
			RemoveHead();

	}

	if (m_list->GetHead()) { 
		
		if (m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_WONDER) {
			m_wonderStarted = m_list->GetHead()->m_type;
			
			g_theWonderTracker->SetBuildingWonder(m_wonderStarted, m_owner);
		}
		else if (m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT) {		
			SendMsgWormholeProbeStarted() ;
		} else if(m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_INFRASTRUCTURE) {
			m_city.CD()->BuildInfrastructure();
		} else if(m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_CAPITALIZATION) {
			m_city.CD()->BuildCapitalization();
		}
	}
        
	if(g_network.IsHost()) {
		g_network.Block(cd->GetHomeCity().GetOwner());
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILT_FRONT,
									  (uint32)cd->GetHomeCity()));
		g_network.Unblock(cd->GetHomeCity().GetOwner());
	}

	DPRINTF(k_DBG_GAMESTATE, ("Setting shieldstore to 0 for %lx\n", m_city.m_id));
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ZeroProduction,
						   GEA_City, m_city,
						   GEA_End);

	m_frontWhenBuilt = NULL;
}


void BuildQueue::SendMsgWonderComplete(CityData *cd, sint32 wonder)
{
	SendMsgWonderCompleteOwner(cd, wonder) ;
	SendMsgWonderCompleteEveryone(cd, wonder) ;
}


void BuildQueue::SendMsgWonderCompleteOwner(CityData *cd, sint32 wonder)
{
	SlicObject *so ;

	
	so = new SlicObject("46WonderComplete") ;
	so->AddCivilisation(m_owner) ;
	so->AddWonder(wonder) ;
	so->AddCity(cd->GetHomeCity()) ;
	so->AddRecipient(m_owner) ;
	g_slicEngine->Execute(so) ;
}


void BuildQueue::SendMsgWonderCompleteEveryone(CityData *cd, sint32 wonder)
{
	SlicObject *so ;
	
	
	so = new SlicObject("47WonderComplete") ;
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

	SlicObject *so ;

	so = new SlicObject("44aWonderStopped") ;
	so->AddCivilisation(m_owner) ;
	so->AddWonder(type) ;
	so->AddAllRecipientsBut(m_owner);
	g_slicEngine->Execute(so) ;
}


void BuildQueue::SendMsgWonderAlmostComplete(void)
{
	SlicObject *so ;

	so = new SlicObject("45WonderAlmostFinished") ;
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

	SlicObject *so ;

	so = new SlicObject("44WonderStarted") ;
	so->AddCivilisation(m_owner) ;
	so->AddWonder(type) ;
	so->AddAllRecipientsBut(m_owner);
	g_slicEngine->Execute(so) ;
}

				
void BuildQueue::SendMsgWormholeProbeStarted(void)
{
	return; 
	SlicObject *so ;

	
	if (!m_list->GetHead()) return;

	if (m_list->GetHead()->m_category != k_GAME_OBJ_TYPE_UNIT)
		return ;

	if (!g_theUnitDB->Get(m_list->GetHead()->m_type)->GetWormholeProbe())	
		return ;

   	so = new SlicObject("63WormholeProbeStarted") ;
    so->AddCivilisation(m_owner) ;
	so->AddUnit(m_list->GetHead()->m_type) ;
	so->AddAllRecipientsBut(m_owner);
	g_slicEngine->Execute(so) ;
}


void BuildQueue::SendMsgWormholeProbeComplete(void)
{
	return; 
	SlicObject	*so ;

	so = new SlicObject("64WormholeProbeCompleted") ;
	so->AddCivilisation(m_owner) ;
	so->AddUnit(m_list->GetHead()->m_type) ;
	so->AddAllRecipientsBut(m_owner);
    g_slicEngine->Execute(so) ;
}


void BuildQueue::RemoveHead()
{
	Assert(m_list->GetHead());
	if(!m_list->GetHead())
		return;

	if (m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_WONDER) {
		m_wonderStarted = m_list->GetHead()->m_type;
		
		g_theWonderTracker->SetBuildingWonder(m_wonderStarted, m_owner);
	} else if (m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT) {	
		SendMsgWormholeProbeStarted() ;
	}
	delete m_list->RemoveHead();
}

void BuildQueue::RawInsertTail(sint32 cat, sint32 t, sint32 cost)
{
	
	if(g_theUnitPool && g_theUnitPool->IsValid(m_city)) {
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
		if(g_player[o]->GetPlayerType() == PLAYER_TYPE_ROBOT &&
			!(g_network.IsClient() && g_network.IsLocalPlayer(o))) {
			sint32 age = 0; 
			if(age >= k_MAX_AGES)
				age = k_MAX_AGES - 1;
			cost = sint32(double(cost) * 
						   (g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->
							GetAiProductionCostAdjustment(o, age)));
		}
	}

    BuildNode *newNode = new BuildNode;

    newNode->m_category = cat; 
    newNode->m_type = t; 
    newNode->m_cost = cost; 
	newNode->m_flags = 0;

	m_list->AddTail(newNode);
}

void BuildQueue::ReplaceHead(sint32 cat, sint32 t, sint32 cost)
{ 
	if(m_list->GetHead() == NULL) { 
        InsertTail (cat, t, cost); 
    } else { 
		if (m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_WONDER)
		{
            m_wonderStopped = m_list->GetHead()->m_type;
			
			g_theWonderTracker->ClearBuildingWonder(m_wonderStopped, m_owner);
		}

        m_list->GetHead()->m_category = cat;
        m_list->GetHead()->m_type = t; 

		sint32 o = m_city.GetOwner();

		if(g_player[o]->GetPlayerType() == PLAYER_TYPE_ROBOT &&
			!(g_network.IsClient() && g_network.IsLocalPlayer(o))) {
			sint32 age = 0; 
			if(age >= k_MAX_AGES)
				age = k_MAX_AGES - 1;
			cost = sint32(double(cost) * 
						   (g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->
							GetAiProductionCostAdjustment(o,age)));
		}

        m_list->GetHead()->m_cost = cost; 
		if (cat == k_GAME_OBJ_TYPE_WONDER) {
            m_wonderStarted = t;
			
			g_theWonderTracker->SetBuildingWonder(m_wonderStarted, m_owner);
		}
		else if (m_list->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT)		
			{
			SendMsgWormholeProbeStarted() ;
			}

		if (g_soundManager) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if (visiblePlayer == m_owner) {
				g_soundManager->AddSound(SOUNDTYPE_VOICE, (uint32)0, 
										gamesounds_GetGameSoundID(GAMESOUNDS_BUILDING_STARTED),
										0,
										0);
			}
		}

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

BOOL BuildQueue::DoInsertChecks(sint32 cat, sint32 t, sint32 cost)
{
	switch(cat) {
        case k_GAME_OBJ_TYPE_UNIT:
		{
			const UnitRecord* rec = g_theUnitDB->Get(t);
            if(!m_city.CanBuildUnit(t)) { 
				return FALSE;
            }

			if (!m_list->GetHead())
				{
				SendMsgWormholeProbeStarted() ;
				}

			break;
		}

		case k_GAME_OBJ_TYPE_WONDER :
		{
			PointerList<BuildNode>::Walker walk(m_list);
			while(walk.IsValid()) {
				if(walk.GetObj()->m_category == k_GAME_OBJ_TYPE_WONDER &&
				   walk.GetObj()->m_type == t) {
					return FALSE;
				}
				walk.Next();
			}
			if (!m_list->GetHead()) {
                m_wonderStarted = t;
				
				g_theWonderTracker->SetBuildingWonder(m_wonderStarted, m_owner);
			}

			break ;
		}
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
		{
			if(!m_city.CanBuildBuilding(t))
				return FALSE;

			PointerList<BuildNode>::Walker walk(m_list);
			while(walk.IsValid()) {
				if(walk.GetObj()->m_category == k_GAME_OBJ_TYPE_IMPROVEMENT &&
				   walk.GetObj()->m_type == t) {
					return FALSE;
				}
				walk.Next();
			}
			break;
		}
		





		default:
			break;
	}
	return TRUE;
}

BOOL BuildQueue::InsertTail(sint32 cat, sint32 t, sint32 cost)

{
	if(!DoInsertChecks(cat, t, cost))
		return FALSE;

    if (g_soundManager && m_list->GetCount() == 0) {
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		if (visiblePlayer == m_owner) {			
			g_soundManager->AddSound(SOUNDTYPE_VOICE, (uint32)0, 
									gamesounds_GetGameSoundID(GAMESOUNDS_BUILDING_STARTED),
									0,
									0);
		}
	}

    RawInsertTail(cat, t, cost);
    return TRUE; 
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
		case k_GAME_OBJ_TYPE_ENDGAME_OBJECT: return grec->GetEndGameRushModifier();
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
		return -1;
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
	BOOL	firstTime = TRUE ;

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
				strcat(s, g_theStringDB->GetNameStr(g_theEndGameDB->Get(node->m_type)->m_name));
				break;

			default :
				Assert(0) ;
				break ;

			}

		if (firstTime)
			{
			sprintf(s, "%s(%d/%d), ", s, node->m_cost, node->m_cost - shieldstore) ;
			firstTime = FALSE ;
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

sint32 BuildQueue::RemoveNode( BuildNode *node, CAUSE_REMOVE_BUILD_ITEM cause )
{
	if (!node) return FALSE;
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

	return TRUE;
}

sint32 BuildQueue::InsertAfter( BuildNode *targetNode, BuildNode *node )
{
	if (!node) return FALSE;
	if (!targetNode) return FALSE;

	PointerList<BuildNode>::Walker walk(m_list);
	while(walk.IsValid()) {
		if(walk.GetObj() == targetNode) {
			walk.InsertAfter(node);
			return TRUE;
		}
		walk.Next();
	}

	return FALSE;
}

sint32 BuildQueue::InsertIndex( sint32 index, BuildNode *node )
{
	if (!node) return FALSE;
	
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
	return TRUE;
}


sint32 BuildQueue::RemoveNodeByIndex(sint32 index,
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
		return FALSE;
	}
	
	return TRUE;
}


void BuildQueue::SetName(MBCHAR *name)
{
	if (name) strcpy(m_name,name);
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

void BuildQueue::RemoveIllegalItems(BOOL isClientAck)
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
	switch(cat) {
        case k_GAME_OBJ_TYPE_UNIT:
			return g_theUnitDB->Get(t)->GetShieldCost();
		case k_GAME_OBJ_TYPE_WONDER :
			return wonderutil_Get(t)->GetProductionCost();
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
			return g_theBuildingDB->Get(t)->GetProductionCost();
		case k_GAME_OBJ_TYPE_ENDGAME_OBJECT:
			return g_theEndGameDB->Get(t)->GetCost();
		default:
			return 0x7fffffff;
	}
}

BOOL BuildQueue::InsertBefore(BuildNode *old, 
							  sint32 cat, sint32 t)
{
	sint32 cost = GetCost(cat, t);

	if(!m_list->GetHead()) {
		InsertTail(cat, t, cost);
	}

	if(!DoInsertChecks(cat, t, cost))
		return FALSE;

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

	
	if(g_network.IsHost()) {
		g_network.Enqueue(m_city.AccessData()->GetCityData());
	} else if(g_network.IsClient()) {
		g_network.SendBuildQueue(m_city.AccessData()->GetCityData());
	}

	return TRUE;
}

void BuildQueue::FinishCreatingUnit(Unit &u)
{
	CityData *cd = m_city.CD();

	if(u.m_id != (0)) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ZeroProduction,
							   GEA_City, m_city,
							   GEA_End);

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

		if((g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT) ||
		   (g_network.IsClient() && g_network.IsLocalPlayer(m_owner) ||
			(!g_network.IsActive() && !g_theProfileDB->AIPopCheat()))) {
			if(u.GetDBRec()->GetBuildingRemovesAPop()) {
				cd->SubtractAccumulatedFood(g_theConstDB->CityGrowthCoefficient());
				cd->ChangePopulation(-1);
			}
		}
		
		
		
		
		FinishBuildFront(u);
	}
}

sint32 BuildQueue::GetLen() const
{
	return m_list->GetCount();
}

BuildNode *BuildQueue::GetHead()
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

	if(g_network.IsHost()) {
		g_network.Enqueue(m_city.AccessData()->GetCityData());
	} else if(g_network.IsClient()) {
		g_network.SendBuildQueue(m_city.AccessData()->GetCityData());
	}
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

	if(g_network.IsHost()) {
		g_network.Enqueue(m_city.AccessData()->GetCityData());
	} else if(g_network.IsClient()) {
		g_network.SendBuildQueue(m_city.AccessData()->GetCityData());
	}

}

bool BuildQueue::IsItemInQueue(uint32 cat, sint32 type)
{
	PointerList<BuildNode>::Walker walk(m_list);
	for(; walk.IsValid(); walk.Next()) {
		if(walk.GetObj()->m_category == cat &&
		   walk.GetObj()->m_type == type)
			return true;
	}
	return false;
}

