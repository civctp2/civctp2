










#include "c3.h"
#include "Globals.h"
#include "C3GameState.h"
#include "C3UnitDB.h"
#include "C3BlgDB.h"
#include "C3InstDB.h"
#include "CivPaths.h"
#include "C3TerrDB.h"
#include "C3ErrorReport.h"
#include "C3String.h"
#include "C3Rand.h"
#include "C3Player.h"
#include "C3World.h"
#include "C3Science.h"
#include "C3Government.h"
#include "C3Wonder.h"
#include "C3Population.h"
#include "C3EndGameDB.h"

#include "IC3DipReq.h"
#include "BSet.h"
#include "UnitPool.h"
#include "Gold.h"
#include "Strengths.h"
#include "Pollution.h"
#include "UnitDynArr.h"
#include "ProfileDB.h"
#include "TradePool.h"
#include "TradeDynArr.h"
#include "Order.h"

#include "RobotAstar.h"

#include "ConstDB.h"

#include "Player.h"



#include "GameSettings.h"

#include "network.h"



extern Player **g_player; 
extern ConstDB *g_theConstDB ;
extern CivPaths *g_civPaths;
extern UnitPool *g_theUnitPool;
extern Pollution *g_thePollution ;
extern ProfileDB *g_theProfileDB;

STDMETHODIMP C3GameState::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3GameState)) {
		*obj = (IC3GameState*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3GameState::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3GameState::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}




C3GameState::C3GameState(sint32 idx)

{
    m_refCount = 0; 
    Assert(g_player[idx]); 
    m_owner = PLAYER_INDEX(idx); 

  	m_player = new C3Player(idx);
	m_world = new C3World(g_player[idx]);
    m_astar = new RobotAstar(g_player[idx]); 
    m_unitDB = new C3UnitDB(g_player[idx]); 
    m_buildingDB = new C3BuildingDB(g_player[idx]); 
    m_population = new C3Population(idx); 
	m_science = new C3Science(idx) ;
	m_government = new C3Government(idx) ;
	m_wonder = new C3Wonder(idx) ;
    m_installationDB = new C3InstDB(g_player[idx]); 
    m_terrainDB = new C3TerrainDB(g_player[idx]); 
	m_errorReport = new C3ErrorReport;
	m_stringDB = new C3String;
	m_endGameDB = new C3EndGameDB(g_player[idx]) ;

	if(g_network.IsClient()) {
		
		m_rand = new C3Rand(TRUE);
	} else {
		m_rand = new C3Rand(FALSE);
	}
}


C3GameState::C3GameState(CivArchive &archive)

{ 
    Serialize(archive); 
} 

void C3GameState::Serialize(CivArchive &archive)

{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 

        archive << m_refCount; 
        archive.PutSINT32(m_owner); 

        m_player->Serialize(archive); 
        m_world->Serialize(archive); 
        m_astar->Serialize(archive); 
        m_unitDB->Serialize(archive); 

        m_buildingDB->Serialize(archive);      
        m_population->Serialize(archive);
		m_science->Serialize(archive) ;
		m_government->Serialize(archive) ;
		m_wonder->Serialize(archive) ;
        m_installationDB->Serialize(archive); 
        m_terrainDB->Serialize(archive); 
        m_rand->Serialize(archive); 
        m_errorReport->Serialize(archive); 
        m_stringDB->Serialize(archive); 
		m_endGameDB->Serialize(archive); 
    } else { 
        archive >> m_refCount; 
        m_owner = PLAYER_INDEX(archive.GetSINT32()); 

        m_player = new C3Player(archive);
	    m_world = new C3World(g_player[m_owner], archive);
        m_astar = new RobotAstar(g_player[m_owner], archive); 
        m_unitDB = new C3UnitDB(g_player[m_owner], archive);
		
        m_buildingDB = new C3BuildingDB(g_player[m_owner], archive); 
        m_population = new C3Population(archive) ;
		m_science = new C3Science(archive) ;
		m_government = new C3Government(archive) ;
		m_wonder = new C3Wonder(archive) ;
        m_installationDB = new C3InstDB(g_player[m_owner], archive); 
        m_terrainDB = new C3TerrainDB(g_player[m_owner], archive);
        m_rand = new C3Rand(archive); 
        m_errorReport = new C3ErrorReport(archive);
	    m_stringDB = new C3String(archive);
        m_endGameDB = new C3EndGameDB(g_player[m_owner], archive);
    }

    CHECKSERIALIZE
} 

C3GameState::~C3GameState()

{
    delete (C3Player*) m_player;
	delete (C3World *) m_world;
    delete (RobotAstar *) m_astar; 
    delete (C3UnitDB *) m_unitDB;
	delete (C3BuildingDB *) m_buildingDB; 
    delete (C3InstDB *) m_installationDB; 
    delete (C3Population *) m_population; 
    delete (C3Science *) m_science ;
	delete (C3Government *) m_government ;
	delete (C3Wonder *) m_wonder ;
    delete (C3TerrainDB *) m_terrainDB; 
	if(m_errorReport) delete m_errorReport;
	if(m_stringDB) delete m_stringDB;
	delete (C3EndGameDB *) m_endGameDB;

	
	if (m_rand)
		delete m_rand;
}

sint32 C3GameState::GetNumPlayersAtStart()
{
    return g_theProfileDB->GetNPlayers();
}

sint32 C3GameState::GetNumPlayers()
{
	sint32 i;
	sint32 count = 0;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i])
			count++;
	}
	return count;
}

sint32 C3GameState::GetMyPlayerId()
{
    return m_owner; 
}










sint32 C3GameState::GetConstRevolutionLevel(void)
	{
	return (g_theConstDB->GetRevolutionLevel()) ;
	}

sint32 C3GameState::GetConstRiotLevel(void) 
{ 
    return g_theConstDB->GetRiotLevel(); 
} 


const MBCHAR *C3GameState::GetDataPath(void)
{
	static MBCHAR path[MAX_PATH];
	g_civPaths->GetSpecificPath(C3DIR_AIDATA, path, FALSE);
	return path;
}

STDMETHODIMP_(void)C3GameState::SendDiplomaticRequest(C3AIDiplomaticRequest *request)
{
	Player *p = g_player[m_owner];
	Assert(p);
	if(!p)
		return;
	PLAYER_INDEX recip = request->m_recipient;

	switch(request->m_tone) {
		case k_MESSAGE_TONE_HOSTILE:
			if(request->m_strength == k_MESSAGE_STRENGTH_STRONG) {
				p->SetAttitude(recip, ATTITUDE_TYPE_STRONG_HOSTILE);
			} else {
				p->SetAttitude(recip, ATTITUDE_TYPE_WEAK_HOSTILE);
			}
			break;
		case k_MESSAGE_TONE_NEUTRAL:
			p->SetAttitude(recip, ATTITUDE_TYPE_NEUTRAL);
			break;
		case k_MESSAGE_TONE_FRIENDLY:
			if(request->m_strength == k_MESSAGE_STRENGTH_STRONG) {
				p->SetAttitude(recip, ATTITUDE_TYPE_STRONG_FRIENDLY);
			} else {
				p->SetAttitude(recip, ATTITUDE_TYPE_WEAK_FRIENDLY);
			}
			break;
		default:
			

            break; 
	}
	switch(request->m_request) {
		case REQUEST_TYPE_GREETING: 
			p->RequestGreeting(recip);
			break;

		
		case REQUEST_TYPE_DEMAND_ADVANCE: 
			p->RequestDemandAdvance(recip, request->m_advance);
			break;
		case REQUEST_TYPE_DEMAND_CITY: 
		{
			BSetID *idx = g_player[recip]->m_bset_cities_index->Find(request->m_targetCity);
			if(idx) {
				Unit city = g_player[recip]->m_all_cities->Access(idx->GetVal());
				Assert(g_theUnitPool->IsValid(city));
				if(g_theUnitPool->IsValid(city)) {
					p->RequestDemandCity(recip, city);
				}
			}
			break;
		}

		case REQUEST_TYPE_DEMAND_MAP: 
			p->RequestDemandMap(recip);
			break;
		case REQUEST_TYPE_DEMAND_GOLD: 
		{
			Gold gold;
			gold.SetLevel(request->m_gold);
			p->RequestDemandGold(recip, gold);
			break;
		}
		case REQUEST_TYPE_DEMAND_STOP_TRADE: 
			p->RequestDemandStopTrade(recip, request->m_thirdParty);
			break;
		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY: 
			p->RequestDemandAttackEnemy(recip, request->m_thirdParty);
			break;
		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS: 
			p->RequestDemandLeaveOurLands(recip);
			break;
		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION: 
			p->RequestDemandReducePollution(recip);
			break;
		case REQUEST_TYPE_DEMAND_NO_PIRACY:
			p->RequestDemandNoPiracy(recip);
			break;
		
		case REQUEST_TYPE_OFFER_ADVANCE: 
			p->RequestOfferAdvance(recip, request->m_advance);
			break;
		case REQUEST_TYPE_OFFER_CITY: 
		{
			BSetID *idx = p->m_bset_cities_index->Find(request->m_targetCity);
			Assert(idx);
			if(idx) {
				Unit city = p->m_all_cities->Access(idx->GetVal());
				Assert(g_theUnitPool->IsValid(city));
				if(g_theUnitPool->IsValid(city)) {
					p->RequestOfferCity(recip, city);
				}
			}
			break;
		}
		case REQUEST_TYPE_OFFER_MAP: 
			p->RequestOfferMap(recip);
			break;
		case REQUEST_TYPE_OFFER_GOLD: 
		{
			Gold gold;
			gold.SetLevel(request->m_gold);
			p->RequestOfferGold(recip, gold);
			break;
		}
		case REQUEST_TYPE_OFFER_CEASE_FIRE: 
			p->RequestOfferCeaseFire(recip);
			break;
		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE: 
			p->RequestOfferPermanentAlliance(recip);
			break;
		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY: 
		{
			BSetID *idx = g_player[request->m_thirdParty]->m_bset_cities_index->Find(request->m_targetCity);
			Assert(idx);
			if(idx) {
				Unit city = g_player[recip]->m_all_cities->Access(idx->GetVal());
				Assert(g_theUnitPool->IsValid(city));
				if(g_theUnitPool->IsValid(city)) {
					p->RequestOfferPactCaptureCity(recip, city);
				}
			}
			break;
		}
		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION: 
			p->RequestOfferPactEndPollution(recip);
			break;
			
		case REQUEST_TYPE_EXCHANGE_ADVANCE: 
			p->RequestExchangeAdvance(recip, request->m_advance, request->m_reciprocalAdvance);
			break;
		case REQUEST_TYPE_EXCHANGE_CITY: 
		{
			BSetID *idx = g_player[request->m_thirdParty]->m_bset_cities_index->Find(request->m_targetCity);
			Assert(idx);
			if(idx) {
				Unit city = g_player[recip]->m_all_cities->Access(idx->GetVal());
				Assert(g_theUnitPool->IsValid(city));
				if(g_theUnitPool->IsValid(city)) {
					BSetID *rewardidx = p->m_bset_cities_index->Find(request->m_reciprocalCity);
					Assert(rewardidx);
					if(rewardidx) {
						Unit rewardCity = p->m_all_cities->Access(idx->GetVal());
						Assert(g_theUnitPool->IsValid(rewardCity));
						if(g_theUnitPool->IsValid(rewardCity)) {
							p->RequestExchangeCity(recip, city, rewardCity);
						}
					}
				}
			}
			break;
		}
		case REQUEST_TYPE_EXCHANGE_MAP: 
			p->RequestExchangeMap(recip);
			break;
	}
}

STDMETHODIMP_(sint32) C3GameState::GetTradeWith(PLAYER_INDEX second, 
												PLAYER_INDEX third)
{
	Assert(g_player[second]);
	if(g_player[second]) {
		return g_player[second]->GetTradeWith(third);
	}
	return 0;
}

void C3GameState::GetMinMaxStrength(double *min, double *max)
{ 
    sint32 player_idx; 
    double s; 

    *min = 1000000000.0; 
    *max = -1.0; 

    for (player_idx=0; player_idx < k_MAX_PLAYERS; player_idx++) { 
        if(g_player[player_idx]) {
		    s = (double)g_player[player_idx]->m_strengths->GetStrength(STRENGTH_CAT_MILITARY);
            if (s < *min) { 
                *min = s; 
            } 

            if (*max < s) { 
                *max = s; 
            } 
	    }
    }
} 

STDMETHODIMP_(double) C3GameState::GetStrength(PLAYER_INDEX player)
{
    
	Assert(g_player[player]);
	double strValue = 0.0;
	if(g_player[player]) {
		
		

		
		strValue += g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_UNITS);
		strValue += g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_GOLD);
		strValue += g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_BUILDINGS);
		strValue += g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_WONDERS);
		strValue += g_player[player]->m_strengths->GetStrength(STRENGTH_CAT_PRODUCTION);
	}
	return strValue;
}
STDMETHODIMP_(sint32) C3GameState::GetGold (PLAYER_INDEX player)
{
	Assert(g_player[player]);
	if(g_player[player] && g_player[m_owner] &&
	   g_player[m_owner]->HasEmbassyWith(player)) {
		return g_player[player]->GetGold();
	}
	return 0;
}

STDMETHODIMP_(double) C3GameState::GetPollutionLevel (PLAYER_INDEX player)
{
	Assert(g_player[player]);
	if(g_player[player]) {
		return g_player[player]->GetPollutionLevel();
	}
	return 0.0;
}

STDMETHODIMP_(sint32) C3GameState::GetGlobalPollutionLevel()
{
	return g_thePollution->GetGlobalPollutionLevel();
}

STDMETHODIMP_(sint32) C3GameState::GetRoundsToNextDisaster()
{
	return g_thePollution->GetRoundsToNextDisaster();
}

STDMETHODIMP_(sint32) C3GameState::GetPollutionTriggerLevel()
{
	return g_thePollution->GetNextTrigger();
}

STDMETHODIMP_(sint32) C3GameState::GetHisTradeRoutes(PLAYER_INDEX player)
{
	Assert(g_player[player]);
	if(g_player[player]) {
		return g_player[player]->GetNumTradeRoutes();
	}
	return 0;
}

STDMETHODIMP_(sint32) C3GameState::IsPlayerAlive(PLAYER_INDEX player)
{
	return g_player[player] != NULL;
}

STDMETHODIMP_(sint32) C3GameState::GetGoodCount(PLAYER_INDEX player,
												uint32 cityId,
												sint32 resource)
{
	BOOL isUnknown;
	Unit city;

	Assert(g_player[player]);
	if(g_player[player]) {
		g_player[player]->AiGetCity(isUnknown, cityId, city);
		if(!isUnknown) {
			return city.GetResourceCount(resource);
		}
	}

	return 0;
}

STDMETHODIMP_(void) C3GameState::MakeTradeBid(PLAYER_INDEX foreigner,
											  uint32 foreignCity,
											  sint32 resource,
											  sint32 goldOffered,
											  uint32 myCity)
{
	Unit fromCity;
	Unit toCity;
	BOOL isUnknown;

	g_player[foreigner]->AiGetCity(isUnknown, foreignCity, fromCity);
	Assert(!isUnknown);
	if(!isUnknown) {
		g_player[m_owner]->AiGetCity(isUnknown, myCity, toCity);
		Assert(!isUnknown);
		if(!isUnknown) {
			
			g_player[m_owner]->SendTradeBid(fromCity, resource, toCity,
											goldOffered);
		}
	}
}

STDMETHODIMP_(sint32) C3GameState::GetTotalTrade(PLAYER_INDEX foreigner)
{
	Assert(g_player[foreigner]);
	if(g_player[foreigner]) {
		return g_player[foreigner]->GetNumTradeRoutes();
	} else {
		return 0;
	}
}

STDMETHODIMP_(sint32) C3GameState::GetGlobalTradeRoutes()
{
	sint32 num = 0;

	return g_theTradePool->GetAllRoutes().Num();
}

STDMETHODIMP_(sint32) C3GameState::GetDifficulty()
{
	Assert(g_theGameSettings);
	return g_theGameSettings->GetDifficulty();
}

STDMETHODIMP_(sint32) C3GameState::GetOrderGoldCost(sint32 order)
{
	
	sint32 index = g_orderInfoMap[order];
	OrderInfo *oi = NULL;
	if(index >= 0 && index < g_numOrderInfo) {
		oi = &g_orderInfo[index];
	} 

	if (oi)
		{
			return oi->m_goldCost;
		}
	return 0;
}


STDMETHODIMP_(sint32) C3GameState::GetOrderMaxGoldCost()
{
	sint32 max = 0;
	OrderInfo *oi = NULL;
	for (sint32 index=0; index<g_numOrderInfo; index++)
		{
			oi = &g_orderInfo[index];
			if (oi && oi->m_goldCost > max )
				max = oi->m_goldCost;
		}
	return max;
}

#include "CivApp.h"

STDMETHODIMP_(void) C3GameState::ProcessGraphicsCallback()
{
	extern CivApp	*g_civApp;

	if (g_civApp)
		g_civApp->ProcessGraphicsCallback();
}


#include "TurnCnt.h"

STDMETHODIMP_(sint32) C3GameState::GetTurn()
{
	extern TurnCount *g_turn; 

	if (g_turn)
		return g_turn->GetRound();
	return -1;
}
