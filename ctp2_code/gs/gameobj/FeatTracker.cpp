//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Feat tracking
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Memory leak repaired.
// - Propagate feat accomplishments.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "FeatTracker.h"
#include "civarchive.h"
#include "newturncount.h"
#include "pointerlist.h"
#include "FeatRecord.h"
#include "player.h"
#include "AdvanceRecord.h"
#include "WonderTracker.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "BuildingRecord.h"
#include "UnitDynArr.h"
#include "citydata.h"

#include "Events.h"
#include "GameEventUser.h"
#include "GameEventManager.h"
#include "EventTracker.h"
#include "Score.h"

#include "net_info.h"
#include "network.h"

FeatTracker *g_featTracker = NULL;

//----------------------------------------------------------------------------
//
// Name       : Feat::Feat
//
// Description: Constructor for an accomplished feat
//
// Parameters : type			: feat identifier
//				player			: player that accomplished the feat
//				round			: the round in which the feat was accomplished
//
// Globals    : NewTurnCount	: the current round (when not provided)
//
// Returns    : -
//
// Remark(s)  : The special round value USE_CURRENT_ROUND may be used to 
//              indicate that the feat has been accomplished right now.
//
//----------------------------------------------------------------------------

Feat::Feat(sint32 type, sint32 player, sint32 round)
:	m_type		(type),
	m_player	(player)
{
	m_round	= (USE_CURRENT_ROUND == round) 
		      ? NewTurnCount::GetCurrentRound() 
			  : round;
}

Feat::Feat(CivArchive &archive)
{
	Serialize(archive);
}

Feat::~Feat()
{
}

void Feat::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive.StoreChunk((uint8*)this, ((uint8 *)&m_round) + sizeof(m_round));
	} else {
		archive.LoadChunk((uint8*)this, ((uint8 *)&m_round) + sizeof(m_round));
	}
}

FeatTracker::FeatTracker()
{
	m_activeList = new PointerList<Feat>;
	sint32 i;
	for(i = FEAT_EFFECT_NONE + 1; i < FEAT_EFFECT_MAX; i++) {
		
		m_effectList[i] = NULL;
	}

	m_achieved = new bool[g_theFeatDB->NumRecords()];
	memset(m_achieved, 0, sizeof(bool) * g_theFeatDB->NumRecords());

	m_buildingFeat = new bool[g_theBuildingDB->NumRecords()];
	memset(m_buildingFeat, 0, sizeof(bool) * g_theBuildingDB->NumRecords());
	
	FindBuildingFeats();
}

FeatTracker::FeatTracker(CivArchive &archive)
{
	m_activeList = new PointerList<Feat>;

	sint32 i;
	for(i = FEAT_EFFECT_NONE + 1; i < FEAT_EFFECT_MAX; i++) {
		
		m_effectList[i] = NULL;
	}


	Serialize(archive);
}

FeatTracker::~FeatTracker()
{
	m_activeList->DeleteAll();
	delete m_activeList;

	sint32 i;
	for(i = 0; i < FEAT_EFFECT_MAX; i++) {
		if(m_effectList[i]) {
			delete m_effectList[i];
			m_effectList[i] = NULL;
		}
	}

	if(m_achieved) {
		delete [] m_achieved;
		m_achieved = NULL;
	}

	if(m_buildingFeat) {
		delete [] m_buildingFeat;
		m_buildingFeat = NULL;
	}
}

void FeatTracker::Serialize(CivArchive & archive)
{
	sint32 count;

	if(archive.IsStoring()) {
		count = m_activeList->GetCount();
		archive << count;

		PointerList<Feat>::Walker walk(m_activeList);
		while(walk.IsValid()) {
			walk.GetObj()->Serialize(archive);
			walk.Next();
		}

		count = g_theFeatDB->NumRecords();
		archive << count;
		archive.Store((uint8*)m_achieved, count * sizeof(bool));

		count = g_theBuildingDB->NumRecords();
		archive << count;
		archive.Store((uint8*)m_buildingFeat, count * sizeof(bool));

	} else {
		archive >> count;
		sint32 i;
		for(i = 0; i < count; i++) {
			Feat *feat = new Feat(archive);
			m_activeList->AddTail(feat);
		}

		archive >> count;
		m_achieved = new bool[count];
		archive.Load((uint8*)m_achieved, count * sizeof(bool));

		if(count != g_theFeatDB->NumRecords()) {
			delete m_achieved;
			m_achieved = new bool[g_theFeatDB->NumRecords()];
			memset(m_achieved, 0, g_theFeatDB->NumRecords() * sizeof(bool));
		}

		archive >> count;
		m_buildingFeat = new bool[count];
		archive.Load((uint8*)m_buildingFeat, count * sizeof(bool));

		if(count != g_theBuildingDB->NumRecords()) {
			delete [] m_buildingFeat;
			m_buildingFeat = new bool[g_theBuildingDB->NumRecords()];
			memset(m_buildingFeat, 0, g_theBuildingDB->NumRecords() * sizeof(bool));
		}

		
		PointerList<Feat>::Walker walk(m_activeList);
		while(walk.IsValid()) {
			Feat *feat = walk.GetObj();
			AddFeatToEffectLists(feat);
			walk.Next();
		}

		FindBuildingFeats();
	}

}

#define CHECK_FEAT_LIST(func, eff) \
if(rec->func()) { \
    if(!m_effectList[eff]) {\
        m_effectList[eff] = new PointerList<Feat>;\
    }\
    m_effectList[eff]->AddTail(feat);\
}

void FeatTracker::AddFeatToEffectLists(Feat *feat)
{
	const FeatRecord *rec = g_theFeatDB->Get(feat->GetType());

	CHECK_FEAT_LIST(GetEffectBoatMovement,                FEAT_EFFECT_BOAT_MOVEMENT);
	CHECK_FEAT_LIST(GetEffectCityDefenseBonus,		    FEAT_EFFECT_CITY_DEFENSE_BONUS);
	CHECK_FEAT_LIST(GetEffectReduceCityWalls,		        FEAT_EFFECT_REDUCE_CITY_WALLS);
	CHECK_FEAT_LIST(GetEffectIncreaseCityVision,	        FEAT_EFFECT_INCREASE_CITY_VISION);
	CHECK_FEAT_LIST(GetEffectIncreaseProduction,	        FEAT_EFFECT_INCREASE_PRODUCTION);
	CHECK_FEAT_LIST(GetEffectIncreaseCommerce,		    FEAT_EFFECT_INCREASE_COMMERCE);
	CHECK_FEAT_LIST(GetEffectIncreaseHappiness,   FEAT_EFFECT_INCREASE_HAPPINESS);
	CHECK_FEAT_LIST(GetEffectEliminateDistancePenalty,    FEAT_EFFECT_ELIMINATE_DISTANCE_PENALTY);
	CHECK_FEAT_LIST(GetEffectIncreaseBoatVision,	        FEAT_EFFECT_INCREASE_BOAT_VISION);
	CHECK_FEAT_LIST(GetEffectIncreaseScience,		        FEAT_EFFECT_INCREASE_SCIENCE);
	CHECK_FEAT_LIST(GetEffectGiveMaps,				    FEAT_EFFECT_GIVE_MAPS);
	CHECK_FEAT_LIST(GetEffectIncreaseHitPoints,	        FEAT_EFFECT_INCREASE_HIT_POINTS);
	CHECK_FEAT_LIST(GetEffectScriptedTurn,			    FEAT_EFFECT_SCRIPTED_TURN);
	CHECK_FEAT_LIST(GetEffectScriptedCity,			    FEAT_EFFECT_SCRIPTED_CITY);
}

#define REMOVE_FROM_FEAT_LIST(func, eff) \
    if(m_effectList[eff] && rec->func()) {\
	    PointerList<Feat>::PointerListNode *node = m_effectList[eff]->Find(feat);\
        if(node) {\
            m_effectList[eff]->Remove(node); \
        }\
    }

void FeatTracker::RemoveFeatFromEffectLists(Feat *feat)
{
	const FeatRecord *rec = g_theFeatDB->Get(feat->GetType());

	REMOVE_FROM_FEAT_LIST(GetEffectBoatMovement,                FEAT_EFFECT_BOAT_MOVEMENT);
	REMOVE_FROM_FEAT_LIST(GetEffectCityDefenseBonus,		    FEAT_EFFECT_CITY_DEFENSE_BONUS);
	REMOVE_FROM_FEAT_LIST(GetEffectReduceCityWalls,		        FEAT_EFFECT_REDUCE_CITY_WALLS);
	REMOVE_FROM_FEAT_LIST(GetEffectIncreaseCityVision,	        FEAT_EFFECT_INCREASE_CITY_VISION);
	REMOVE_FROM_FEAT_LIST(GetEffectIncreaseProduction,	        FEAT_EFFECT_INCREASE_PRODUCTION);
	REMOVE_FROM_FEAT_LIST(GetEffectIncreaseCommerce,		    FEAT_EFFECT_INCREASE_COMMERCE);
	REMOVE_FROM_FEAT_LIST(GetEffectIncreaseHappiness,   FEAT_EFFECT_INCREASE_HAPPINESS);
	REMOVE_FROM_FEAT_LIST(GetEffectEliminateDistancePenalty,    FEAT_EFFECT_ELIMINATE_DISTANCE_PENALTY);
	REMOVE_FROM_FEAT_LIST(GetEffectIncreaseBoatVision,	        FEAT_EFFECT_INCREASE_BOAT_VISION);
	REMOVE_FROM_FEAT_LIST(GetEffectIncreaseScience,		        FEAT_EFFECT_INCREASE_SCIENCE);
	REMOVE_FROM_FEAT_LIST(GetEffectGiveMaps,				    FEAT_EFFECT_GIVE_MAPS);
	REMOVE_FROM_FEAT_LIST(GetEffectIncreaseHitPoints,	        FEAT_EFFECT_INCREASE_HIT_POINTS);
	REMOVE_FROM_FEAT_LIST(GetEffectScriptedTurn,			    FEAT_EFFECT_SCRIPTED_TURN);
	REMOVE_FROM_FEAT_LIST(GetEffectScriptedCity,			    FEAT_EFFECT_SCRIPTED_CITY);
}

//----------------------------------------------------------------------------
//
// Name       : FeatTracker::AddFeat
//
// Description: Add an accomplished feat to the records
//
// Parameters : type			: feat identifier
//				player			: player that accomplished the feat
//				round			: the round in which the feat was accomplished
//
// Globals    : g_theFeatDB		: database of feat descriptions
//				TODO: add more globals
//
// Returns    : -
//
// Remark(s)  : The special round value USE_CURRENT_ROUND may be used to 
//              indicate that the feat has been accomplished right now.
//
//----------------------------------------------------------------------------

void FeatTracker::AddFeat(sint32 type, sint32 player, sint32 round)
{
	const FeatRecord *rec = g_theFeatDB->Get(type);
	Assert(rec);
	if(!rec) return;
	
	if(m_achieved[type]) {
		
		return;
	}

	if(rec->GetNumExcludeAdvance() > 0) {
		
		sint32 a, p;
		for(a = 0; a < rec->GetNumExcludeAdvance(); a++) {
			for(p = 0; p < k_MAX_PLAYERS; p++) {
				if(g_player[p] && g_player[p]->HasAdvance(rec->GetExcludeAdvanceIndex(a))) {
					return;
				}
			}
		}
	}

	if(rec->GetNumExcludeWonder() > 0) {
		
		sint32 w;
		for(w = 0; w < rec->GetNumExcludeWonder(); w++) {
			if(g_theWonderTracker->HasWonderBeenBuilt(rec->GetExcludeWonderIndex(w)))
				return;
		}
	}

	if(rec->GetNumExcludeFeat() > 0) {
		
		sint32 f;
		for(f = 0; f < rec->GetNumExcludeFeat(); f++) {
			if(m_achieved[rec->GetExcludeFeatIndex(f)])
				return;
		}
	}

	
	const MBCHAR *slicFunc;
	if(rec->GetExcludeFunction(slicFunc)) {
		if(g_slicEngine->CallExcludeFunc(slicFunc, type, player))
			return;
	}


	m_achieved[type] = true;

	Feat * theFeat = new Feat(type, player, round);
	m_activeList->AddTail(theFeat);

	AddFeatToEffectLists(theFeat);

	const MBCHAR *slicMessage;
	SlicObject *so;
	if(rec->GetSlicMessage(slicMessage)) {
		so = new SlicObject((char *)slicMessage);
	} else {
		so = new SlicObject("MGenericFeatAccomplished");
	}
	so->AddPlayer(player);
	so->AddRecipient(player);
	so->AddInt(type);
	g_slicEngine->Execute(so);

	g_player[player]->m_score->AddFeat();

	g_eventTracker->AddEvent(EVENT_TYPE_FEAT, player, theFeat->GetRound(), type);
}

void FeatTracker::AddFeat(const MBCHAR *name, sint32 player)
{
	sint32 featIndex;
	if(!g_theFeatDB->GetNamedItem(name, featIndex)) {
		bool unknown_feat = false;
		Assert(unknown_feat);
		return;
	}

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AccomplishFeat,
						   GEA_Int, featIndex,
						   GEA_Player, player,
						   GEA_End);
	
}

sint32 FeatTracker::GetEffect(FEAT_EFFECT effect, sint32 player, bool getTotal)
{
	if(!m_effectList[effect])
		return 0;

	sint32 result = 0;
	sint32 sub = 0;
	PointerList<Feat>::Walker walk(m_effectList[effect]);
	while(walk.IsValid()) {
		if(walk.GetObj()->GetPlayer() == player) {
			const FeatRecord *rec = g_theFeatDB->Get(walk.GetObj()->GetType());
			switch(effect) {
				case FEAT_EFFECT_BOAT_MOVEMENT:               rec->GetEffectBoatMovement(sub);break;
				case FEAT_EFFECT_CITY_DEFENSE_BONUS:          rec->GetEffectCityDefenseBonus(sub);break;
				case FEAT_EFFECT_REDUCE_CITY_WALLS:           rec->GetEffectReduceCityWalls(sub);break;
				case FEAT_EFFECT_INCREASE_CITY_VISION:        rec->GetEffectIncreaseCityVision(sub);break;
				case FEAT_EFFECT_INCREASE_PRODUCTION:         rec->GetEffectIncreaseProduction(sub);break;
				case FEAT_EFFECT_INCREASE_COMMERCE:           rec->GetEffectIncreaseCommerce(sub);break;
				case FEAT_EFFECT_INCREASE_HAPPINESS:          rec->GetEffectIncreaseHappiness(sub);break;
				case FEAT_EFFECT_ELIMINATE_DISTANCE_PENALTY:  rec->GetEffectEliminateDistancePenalty(sub);break;
				case FEAT_EFFECT_INCREASE_BOAT_VISION:        rec->GetEffectIncreaseBoatVision(sub);break;
				case FEAT_EFFECT_INCREASE_SCIENCE:            rec->GetEffectIncreaseScience(sub);break;
				
				case FEAT_EFFECT_INCREASE_HIT_POINTS:         rec->GetEffectIncreaseHitPoints(sub);break;
				
				
				default:
					Assert(FALSE);
					break;
			}
			if(getTotal)
				result += sub;
			else
				result = max(sub, result);
		}
		walk.Next();
	}
	return result;
}

sint32 FeatTracker::GetAdditiveEffect(FEAT_EFFECT effect, sint32 player)
{
	return GetEffect(effect, player, true);
}

sint32 FeatTracker::GetMaxEffect(FEAT_EFFECT effect, sint32 player)
{
	return GetEffect(effect, player, false);
}

void FeatTracker::BeginTurn(sint32 player)
{
	PointerList<Feat>::Walker walk(m_activeList);
	while(walk.IsValid()) {
		Feat *feat = walk.GetObj();
		if(feat->GetPlayer() == player) {
			
			const FeatRecord *rec = g_theFeatDB->Get(feat->GetType());
			if(rec->GetDuration() + feat->GetRound() <= NewTurnCount::GetCurrentRound()) {
				
				walk.Remove();
				RemoveFeatFromEffectLists(feat);
				delete feat;
				continue; 
			}
		}
		walk.Next();
	}
}

void FeatTracker::FindBuildingFeats()
{
	sint32 f;
	for(f = 0; f < g_theFeatDB->NumRecords(); f++) {
		const FeatRecord *rec = g_theFeatDB->Get(f);
		const FeatRecord::BuildingFeat *bf;
		if(rec->GetBuilding(bf)) {
			m_buildingFeat[bf->GetBuildingIndex()] = true;
		}
	}
}

void FeatTracker::CheckBuildingFeat(Unit &city, sint32 building)
{
	
	if(!m_buildingFeat[building]) return;

	
	sint32 f;
	for(f = 0; f < g_theFeatDB->NumRecords(); f++) {
		const FeatRecord *rec = g_theFeatDB->Get(f);
		const FeatRecord::BuildingFeat *bf;

		
		if(rec->GetBuilding(bf)) {
			
			if(bf->GetBuildingIndex() == building) {
				
				sint32 numCities = 0;
				sint32 c;
				for(c = 0; c < g_player[city.GetOwner()]->m_all_cities->Num(); c++) {
					Unit aCity = g_player[city.GetOwner()]->m_all_cities->Access(c);
					if(aCity.CD()->HaveImprovement(building))
						numCities++;
				}
				
				sint32 num, percent;
				
				if(bf->GetNum(num)) {
					if(numCities >= num) {
						
						g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AccomplishFeat,
											   GEA_Int, f,
											   GEA_Player, city.GetOwner(),
											   GEA_End);
					}
				} else if(bf->GetPercentCities(percent)) {
					sint32 havePercent = (numCities * 100) / g_player[city.GetOwner()]->m_all_cities->Num();
					if(havePercent >= percent) {
						
						AddFeat(f, city.GetOwner());
						g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AccomplishFeat,
											   GEA_Int, f,
											   GEA_Player, city.GetOwner(),
											   GEA_End);
					}
				}
			}
		}
	}	
}

void FeatTracker::CheckConquerFeat(sint32 defeated, sint32 defeatedByWhom)
{
	sint32 featIndex;
	if(g_theFeatDB->GetNamedItem("FEAT_CONQUERED_BY_FORCE", featIndex)) 
	{
		
		sint32 minCityCount;
		if(g_theFeatDB->Get(featIndex)->GetMinimumSizeOfCiv())
		{
			g_theFeatDB->Get(featIndex)->GetMinimumSizeOfCiv(minCityCount);
			if(g_player[defeated]->GetMaxCityCount() >= minCityCount)
			{
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AccomplishFeat,
									   GEA_Int, featIndex,
									   GEA_Player, defeatedByWhom,
									   GEA_End);
			}
		}
	}
	else
	{
		bool unknown_feat = false;
		Assert(unknown_feat);
		return;
	}
}

//----------------------------------------------------------------------------
//
// Name       : AccomplishFeat::GEVHookCallback
//
// Description: Handler for GEV_AccomplishFeat events.
//
// Parameters : gameEventType	: should be GEV_AccomplishFeat
//				args			: list of arguments				
//
// Globals    : -
//
// Returns    : GEV_HD_Continue always
//
// Remark(s)  : The arguments must contain the identifier of the accomplished
//              feat and the player that accomplished it.
//
//----------------------------------------------------------------------------

STDEHANDLER(AccomplishFeat)
{
	
	sint32 player, featIndex;
	if(!args->GetInt(0, featIndex)) return GEV_HD_Continue;
	if(!args->GetPlayer(0, player)) return GEV_HD_Continue;

	g_featTracker->AddFeat(featIndex, player);

	if (g_network.IsHost()) 
	{
		// Propagate the information to the clients.
		// Remark: g_player[player] has been verified in GetPlayer.
		g_network.Block(player);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_ACCOMPLISHED_FEAT,
									  featIndex, 
									  player, 
									  g_player[player]->GetCurRound()
									 )
						 );
		g_network.Unblock(player);
	}

	return GEV_HD_Continue;
}


STDEHANDLER(FeatBeginTurn)
{
	sint32 player;
	if(!args->GetPlayer(0, player)) return GEV_HD_Continue;

	g_featTracker->BeginTurn(player);
	return GEV_HD_Continue;
}

STDEHANDLER(FeatBuildingBuilt)
{
	Unit city;
	sint32 building;

	if(!args->GetInt(0, building)) return GEV_HD_Continue;
	if(!args->GetCity(0, city)) return GEV_HD_Continue;

	g_featTracker->CheckBuildingFeat(city, building);
	return GEV_HD_Continue;
}




void FeatTracker::InitializeEvents()
{
	g_gevManager->AddCallback(GEV_AccomplishFeat, GEV_PRI_Primary, &s_AccomplishFeat);

	g_gevManager->AddCallback(GEV_BeginTurn, GEV_PRI_Post, &s_FeatBeginTurn);
	g_gevManager->AddCallback(GEV_CreateBuilding, GEV_PRI_Post, &s_FeatBuildingBuilt);
	g_gevManager->AddCallback(GEV_CreateBuilding, GEV_PRI_Post, &s_FeatBuildingBuilt);
}

void FeatTracker::CleanupEvents()
{
}
