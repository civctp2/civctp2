
#include "c3.h"

#include "GoodyHuts.h"
#include "Player.h"
#include "RandGen.h"
#include "AICause.h"
#include "ConstDB.h"
#include "Unit.h"
#include "CivArchive.h"
#include "Advances.h"
#include "CivPaths.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "UnitDynArr.h"
#include "UnitData.h"
#include "SoundManager.h"
#include "gamesounds.h"
#include "network.h"
#include "TiledMap.h"
#include "Barbarians.h"
#include "ProfileDB.h"
#include "RiskDB.h"
#include "AdvanceRecord.h"
#include "SelItem.h"
#include "GameSettings.h"
#include "Exclusions.h"
#include "UnitRecord.h"
#include "GameEventManager.h"

extern Player **g_player;
extern RandomGenerator *g_rand;
extern ConstDB *g_theConstDB;
extern CivPaths *g_civPaths; 
extern TiledMap		*g_tiledMap;
extern ProfileDB *g_theProfileDB;

extern SelectedItem	*g_selected_item;
extern SoundManager *g_soundManager;
extern RiskDatabase *g_theRiskDB;

GoodyHut::GoodyHut()
{
	
	
	m_typeValue = g_rand->Next(100); 
	m_value = g_rand->Next(k_VALUE_RANGE);
}

GoodyHut::GoodyHut(CivArchive &archive)
{
	Serialize(archive);
}

GOODY GoodyHut::ChooseType(sint32 owner)
{
	DPRINTF(k_DBG_GAMESTATE, ("Choose ruin type for typeValue %d\n", m_typeValue));
	if(owner == PLAYER_INDEX_VANDALS) {
		
		return GOODY_BOGUS;
	}

	uint32 accum = 0;
	sint32 riskLevel = g_theGameSettings->GetRisk();
	while(riskLevel >= g_theRiskDB->m_nRec) {
		riskLevel--;
	}
	const RiskRecord *risk = g_theRiskDB->Get(riskLevel);

	accum += (uint32)ceil(risk->m_barbarianHutChance * 100);
	if(m_typeValue < accum) {
		return GOODY_BARBARIANS;
	}
	accum += (uint32)ceil(risk->m_goldChance * 100);
	if(m_typeValue < accum) {
		m_value = (((risk->m_maxGold - risk->m_minGold) * m_value) / k_VALUE_RANGE) + risk->m_minGold;
		return GOODY_GOLD;
	}

	accum += (uint32)ceil(risk->m_advanceChance * 100);
	if(m_typeValue < accum) {
		sint32 *possible; 
		possible = new sint32[g_theAdvanceDB->NumRecords()];
		sint32 nextPossible = 0;
		sint32 i;
		sint32 legalCount = 0;
		sint32 prereq;

		for(i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
			if(g_player[owner]->HasAdvance(i)) {
				continue;
			}
			if((g_theAdvanceDB->Get(i)->GetNumPrerequisites() > 0) &&
			   (g_theAdvanceDB->Get(i)->GetPrerequisitesIndex(0) == i)) {
				continue;
			}

			prereq = g_player[owner]->m_advances->GetMinPrerequisites(i);
			if(prereq <= risk->m_maxAdvancePrerequisites) {
				possible[nextPossible++] = i;
			}
		}
		if(!nextPossible) {
			delete [] possible;
			return GOODY_BOGUS;
		}

		m_value = possible[(nextPossible * m_value) / k_VALUE_RANGE];
		delete [] possible;
		return GOODY_ADVANCE;
	}

	accum += (uint32)ceil(risk->m_unitChance * 100);
	if(m_typeValue < accum) {
		sint32 *possible;
		possible = new sint32[g_theAdvanceDB->NumRecords()];
		sint32 nextPossible = 0;
		sint32 i;
		sint32 legalCount = 0;

		for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
			const UnitRecord *rec = g_theUnitDB->Get(i);
			if(!rec->GetMovementTypeLand())
				continue;
			if(rec->GetAttack() < 1)
				continue;
			if(rec->GetHasPopAndCanBuild())
				continue;
			if(g_exclusions->IsUnitExcluded(i))
				continue;
			if(rec->GetNumGovernmentType() > 0)
				continue;

			if(g_player[owner]->m_advances->GetMinPrerequisites(rec->GetEnableAdvanceIndex()) <= risk->m_maxUnitPrerequisites) {
				possible[nextPossible++] = i;
			}
		}
		if(!nextPossible) {
			delete [] possible;
			return GOODY_BOGUS;
		}
		m_value = possible[(nextPossible * m_value) / k_VALUE_RANGE];
		delete [] possible;
		return GOODY_UNIT;
	}

	accum += (uint32)ceil(risk->m_cityChance * 100);
	if(m_typeValue < accum) {
		return GOODY_CITY;
	}

	accum += (uint32)ceil(risk->m_settlerChance * 100);
	if(m_typeValue < accum) {
		sint32 i;
		for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
			if(g_theUnitDB->Get(i)->GetSettleLand()) {
				m_value = i;
				return GOODY_UNIT;
			}
		}
		return GOODY_BOGUS;
	}
	return GOODY_BOGUS;
}

void GoodyHut::OpenGoody(sint32 owner, const MapPoint &point)
{
	SlicObject *so ;
	
	switch(ChooseType(owner)) {
		case GOODY_BOGUS:
		{
			so = new SlicObject("93BesetByNothing");
			so->AddRecipient(owner) ;
			g_slicEngine->Execute(so) ;
			DPRINTF(k_DBG_GAMESTATE, ("No soup for you!\n"));
            
			
			if (g_soundManager) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if (visiblePlayer == owner) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
											gamesounds_GetGameSoundID(GAMESOUNDS_DRAGDROP_FAIL),
											point.x,
											point.y);
				}
			}
			
			break;
		}
		case GOODY_CITY:
		{
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_CreateCity,
								   GEA_Player, owner,
								   GEA_MapPoint, point,
								   GEA_Int, CAUSE_NEW_CITY_GOODY_HUT,
								   GEA_Int, -1,
								   GEA_End);
#if 0
			Unit city = g_player[owner]->CreateCity(0,
										point,
										CAUSE_NEW_CITY_GOODY_HUT,
										NULL);
			if(city.m_id == 0) {
				so = new SlicObject("93BesetByNothing");
				so->AddRecipient(owner);
				g_slicEngine->Execute(so);
				break;
			}
			so = new SlicObject("80RuinBecomesCity") ;
			so->AddRecipient(owner) ;
			so->AddCity(city);
			g_slicEngine->Execute(so) ;
			DPRINTF(k_DBG_GAMESTATE, ("You get a city!\n"));

			
			
			


			if (g_soundManager) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if (visiblePlayer == owner) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
											gamesounds_GetGameSoundID(GAMESOUNDS_GOODY_CITY),
											point.x,
											point.y);
				}
			}
#endif
			break;
		}
		case GOODY_GOLD:
			so = new SlicObject("83DiscoveredAncientTreasure") ;
			so->AddRecipient(owner) ;
			so->AddGold(m_value) ;
			g_slicEngine->Execute(so) ;
			DPRINTF(k_DBG_GAMESTATE, ("You get %d gold!\n", m_value));
			g_player[owner]->AddGold(m_value);
			if (g_soundManager) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if (visiblePlayer == owner) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
											gamesounds_GetGameSoundID(GAMESOUNDS_TOOEXPENSIVE),
											point.x,
											point.y);
				}
			}
			break;
		case GOODY_ADVANCE:
			so = new SlicObject("79DiscoveredRemnantsOfAncientCivilisation") ;
			DPRINTF(k_DBG_GAMESTATE, ("You find advance %d\n", m_value));
			g_player[owner]->m_advances->GiveAdvance(m_value, CAUSE_SCI_GOODY);
			so->AddRecipient(owner);
			so->AddAdvance(m_value);
			g_slicEngine->Execute(so);

			if (g_soundManager) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if (visiblePlayer == owner) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
											gamesounds_GetGameSoundID(GAMESOUNDS_ADVANCE),
											point.x,
											point.y);
				}
			}
			break;
		case GOODY_UNIT:
		{
			if (g_theUnitDB->Get(m_value)->GetSettle())
				so = new SlicObject("81NomadsHaveJoinedYourCivilisation") ;
			else
				so = new SlicObject("82MercenariesHaveJoinedYourCivilisation") ;

			so->AddRecipient(owner) ;
			g_slicEngine->Execute(so) ;
			DPRINTF(k_DBG_GAMESTATE, ("You get unit %d\n", m_value));
			Unit u = g_player[owner]->CreateUnit(m_value,
												 point,
												 Unit(0),
												 FALSE,
												 CAUSE_NEW_ARMY_GOODY_HUT);








			if (g_soundManager) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if (visiblePlayer == owner) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
											gamesounds_GetGameSoundID(GAMESOUNDS_GOODY_UNIT),
											point.x,
											point.y);
				}
			}
			break;
		}	
		case GOODY_BARBARIANS:
			if(Barbarians::AddBarbarians(point, owner, TRUE)) {
				so = new SlicObject("84BesetByVandals") ;
				so->AddRecipient(owner);
				g_slicEngine->Execute(so);

				if (g_soundManager) {
					sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
					if (visiblePlayer == owner) {
						g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
												 gamesounds_GetGameSoundID(GAMESOUNDS_BOO),
												 point.x,
												 point.y);
					}	
				}
			} else {
				so = new SlicObject("93BesetByNothing");
				so->AddRecipient(owner) ;
				g_slicEngine->Execute(so) ;
				DPRINTF(k_DBG_GAMESTATE, ("No soup for you!\n"));
				
				
				if (g_soundManager) {
					sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
					if (visiblePlayer == owner) {
						g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
												 gamesounds_GetGameSoundID(GAMESOUNDS_DRAGDROP_FAIL),
												 point.x,
												 point.y);
					}
				}
			}
			break;
		default:
			Assert(FALSE);
			break;
	}

}

void GoodyHut::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

	if(archive.IsStoring()) {
		archive.StoreChunk((uint8 *)&m_value, ((uint8 *)&m_typeValue)+sizeof(m_typeValue));

	} else {
		archive.LoadChunk((uint8 *)&m_value, ((uint8 *)&m_typeValue)+sizeof(m_typeValue));

	}
}
