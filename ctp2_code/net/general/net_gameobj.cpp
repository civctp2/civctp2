









#include "c3.h"
#include "globals.h"
#include "net_gameobj.h"
#include "GameObj.h"
#include "ObjPool.h"

#include "Unit.h"
#include "TradeRoute.h"
#include "TradeOffer.h"
#include "TerrImprove.h"
#include "installation.h"
#include "DiplomaticRequest.h"
#include "message.h"

#include "UnitPool.h"
#include "TradePool.h"
#include "TradeOfferPool.h"
#include "TerrImprovePool.h"
#include "installationpool.h"
#include "CivilisationPool.h"
#include "DiplomaticRequestPool.h"
#include "MessagePool.h"
#include "ArmyPool.h"

#include "aicause.h"
#include "pointerlist.h"

extern UnitPool *g_theUnitPool;

NetGameObj::GameObjRecord::GameObjRecord(GAMEOBJ *obj)
{
	m_id = obj->m_id;
}

NetGameObj::NetGameObj()
{
	m_created = new PointerList<GameObjRecord>;
	m_limbo = new PointerList<LimboID>;
}

NetGameObj::~NetGameObj()
{
	if(m_created) {
		m_created->DeleteAll();
		delete m_created;
	}

	if(m_limbo)
		delete m_limbo;
}




void NetGameObj::AddCreated(GAMEOBJ *obj)
{
	if(!m_limbo->IsEmpty()) {
		DPRINTF(k_DBG_NET, ("AddCreated: Sending object %lx straight to limbo\n",
							obj->m_id));
		m_limbo->AddTail(new LimboID(obj->m_id));
		
	} else {
		DPRINTF(k_DBG_NET, ("AddCreated: %lx\n", obj->m_id));
		m_created->AddTail(new GameObjRecord(obj));
		m_createdHash.Add(obj->m_id);
	}
}







void NetGameObj::ACKObject(uint32 id)
{
	GameObjRecord *obj = m_created->GetHead();
	FixKey(id);

	if(!obj) {
		LimboID *lID = m_limbo->GetHead();
		if(lID && lID->m_id == id) {
			DPRINTF(k_DBG_NET, ("ACKObject: Found object id %lx in limbo\n", 
								id));
			
			m_limbo->RemoveHead();
			delete lID;
		} else {
			
			Assert(FALSE);
		}

		return;
	}

	Assert(obj->m_id == id); 
	if(obj->m_id == id) {
		m_created->RemoveHead();


		delete obj;
		m_createdHash.Remove(id);
	}
}

void NetGameObj::NAKObject(uint32 myId, uint32 realId)
{
	TheReaper();

	LimboID *lID = m_limbo->RemoveHead();
	uint32 id = lID->m_id;
	delete lID;
	Assert(id == myId); 
	Assert(((realId & k_ID_KEY_MASK) >> k_ID_MASK_SHIFT) ==
		   ((id & k_ID_KEY_MASK) >> k_ID_MASK_SHIFT)); 
	KillObject(id);

	if(realId != 0)
		FixKey(realId);
}

void NetGameObj::TheReaper()
{
	
	while(!m_created->IsEmpty()) {
		GameObjRecord *obj = m_created->RemoveHead();
		
		DPRINTF(k_DBG_NET, ("TheReaper: Sending object %lx to limbo\n",
							obj->m_id));
		m_limbo->AddTail(new LimboID(obj->m_id));
		delete obj;
		
	}
	m_createdHash.Clear();
}

void NetGameObj::CheckReceived(uint32 id)
{
	BOOL reap = FALSE;

#ifdef _DEBUG
	if(!m_created->IsEmpty()) {
		GameObjRecord *obj = m_created->GetHead();
		DPRINTF(k_DBG_NET, ("CheckReceived: %lx (expecting %lx at some point)\n", id, obj->m_id));
	} else {
		DPRINTF(k_DBG_NET, ("CheckReceived: %lx (not expecting anything in particular)\n", id));
	}
#endif

	DPRINTF(k_DBG_NET, ("CheckReceived: %lx\n", id));

	switch(id  & k_ID_TYPE_MASK) {
		case k_BIT_GAME_OBJ_TYPE_UNIT:
		{
			
			
			
			Unit u(id);
			if(g_theUnitPool->IsValid(u)) {
				if(m_createdHash.IsPresent(id)) {
					reap = TRUE;
				}
			} else {
				reap = TRUE;
			}
			break;
		}
		case k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT:
		{
			TerrainImprovement imp(id);
			if(g_theTerrainImprovementPool->IsValid(imp)) {
				if(m_createdHash.IsPresent(id)) {
					reap = TRUE;
				}
			} else {
				reap = TRUE;
			}
			break;
		}
		case k_BIT_GAME_OBJ_TYPE_ARMY:
		{
			Army army(id);
			if(g_theArmyPool->IsValid(army)) {
				if(m_createdHash.IsPresent(id)) {
					reap = TRUE;
				}
			} else {
				reap = TRUE;
			}
			break;
		}
		
		
		
		

		default:
			if(!m_created->IsEmpty()) {
				DPRINTF(k_DBG_NET, ("NetGameObj: Received object id %lx, but have unACKed objects\n", id));
				
				reap = TRUE;
			}
			break;
	}
	if(reap) {
		TheReaper();
		return;
	}
}

void NetGameObj::KillObject(uint32 id)
{
	DPRINTF(k_DBG_NET, ("NetGameObj: Killing object %lx\n", id));
	switch(id & k_ID_TYPE_MASK) {
		case k_BIT_GAME_OBJ_TYPE_UNIT:
		{
			Unit unit(id);
			if(unit.IsValid()) {
				unit.KillUnit(CAUSE_REMOVE_ARMY_UNKNOWN, -1);
			}
			break;
		}
		case k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE:
		{			
			if(g_theTradePool->IsValid(id)) {
				TradeRoute route(id);
				route.KillRoute(CAUSE_KILL_TRADE_ROUTE_UNKNOWN);
			}
			break;
		}
		case k_BIT_GAME_OBJ_TYPE_TRADE_OFFER:
		{
			
			Assert(FALSE);
		}
		case k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT:
		{
			TerrainImprovement imp(id);
			if(g_theTerrainImprovementPool->IsValid(imp)) {
				imp.KillImprovement();
			}
			break;
		}
		case k_BIT_GAME_OBJ_TYPE_INSTALLATION:
		{
			Installation inst(id);
			if(g_theInstallationPool->IsValid(inst)) {
				inst.KillInstallation();
			}
			break;
		}
		case k_BIT_GAME_OBJ_TYPE_CIVILISATION:
		{
			Civilisation cid(id);
			if(g_theCivilisationPool->IsValid(cid)) {
				cid.KillCivilisation();
			}
			break;
		}
		case k_BIT_GAME_OBJ_TYPE_DIPLOMATIC_REQUEST:
		{
			DiplomaticRequest rid(id);

			
			if(g_theDiplomaticRequestPool->IsValid(rid))
				rid.Kill();
			break;
		}
		case k_BIT_GAME_OBJ_TYPE_MESSAGE:
		{
			Message mid(id);
			if(g_theMessagePool->IsValid(mid)) {
				mid.Kill();
			}
			break;
		}
		case k_BIT_GAME_OBJ_TYPE_ARMY:
		{
			Army army(id);
			if(army.IsValid())
				army.Kill();
			break;
		}

		default:
			Assert(FALSE);
	}
}

void NetGameObj::FixKey(uint32 id)
{
	DPRINTF(k_DBG_NET, ("NetGameObj: Fixing key %lx\n", id));
	switch(id & k_ID_TYPE_MASK) {
		case k_BIT_GAME_OBJ_TYPE_UNIT:
			g_theUnitPool->HackSetKey((id & k_ID_KEY_MASK) + 1);
			break;
		case k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE:
			g_theTradePool->HackSetKey((id & k_ID_KEY_MASK) + 1);
			break;
		case k_BIT_GAME_OBJ_TYPE_TRADE_OFFER:
			g_theTradeOfferPool->HackSetKey((id & k_ID_KEY_MASK) + 1);
			break;
		case k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT:
			g_theTerrainImprovementPool->HackSetKey((id & k_ID_KEY_MASK) + 1);
			break;
		case k_BIT_GAME_OBJ_TYPE_INSTALLATION:
			g_theInstallationPool->HackSetKey((id & k_ID_KEY_MASK) + 1);
			break;
		case k_BIT_GAME_OBJ_TYPE_CIVILISATION:
			g_theCivilisationPool->HackSetKey((id & k_ID_KEY_MASK) + 1);
			break;
		case k_BIT_GAME_OBJ_TYPE_DIPLOMATIC_REQUEST:
			g_theDiplomaticRequestPool->HackSetKey((id & k_ID_KEY_MASK) + 1);
			break;
		case k_BIT_GAME_OBJ_TYPE_MESSAGE:
			g_theMessagePool->HackSetKey((id & k_ID_KEY_MASK) + 1);
			break;
		case k_BIT_GAME_OBJ_TYPE_ARMY:
			g_theArmyPool->HackSetKey((id & k_ID_KEY_MASK) + 1);
			break;
		default:
			Assert(FALSE);
	}
}
