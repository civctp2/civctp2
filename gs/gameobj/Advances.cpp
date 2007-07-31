









#include "c3.h"      

#include "Advances.h"
#include "AdvanceRecord.h"

#include "CivArchive.h"
#include "Player.h"
#include "WonderRecord.h"
#include "AgeRecord.h"
#include "StrDB.h"

#include "network.h"
#include "net_info.h"
#include "net_action.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "QuickSlic.h"
#include "Civilisation.h"


#include "AiCause.h"
#include "SelItem.h"

#include "UnitActor.h"
#include "TiledMap.h"

#include "Unit.h"
#include "Sci.h"
#include "Gold.h"

#include "DiffDB.h"
#include "ProfileDB.h"
#include "ConstDB.h"
#include "RandGen.h"
#include "statswindow.h"
#include "CivilisationPool.h"

#include "GameSettings.h"
#include "AgeRecord.h"
#include "BuildingRecord.h"
#include "UnitDynArr.h"
#include "UnitRecord.h"
#include "UnitData.h"

#include "buildingutil.h"
#include "wonderutil.h"
#include "MainControlPanel.h"

extern Player** g_player;

extern SelectedItem *g_selected_item;
extern TiledMap		*g_tiledMap;
extern DifficultyDB *g_theDifficultyDB;
extern ProfileDB    *g_theProfileDB;
extern ConstDB      *g_theConstDB;
extern RandomGenerator *g_rand;
extern StringDB     *g_theStringDB ;
extern CivilisationPool *g_theCivilisationPool;

#define k_MAX_ADVANCE_TURNS 1000

Advances::Advances()
{
	m_size = g_theAdvanceDB->NumRecords();
    Assert(m_size);
	m_hasAdvance = new uint8[m_size];
	m_canResearch = new uint8[m_size];
	m_turnsSinceOffered = new uint16[m_size];
	memset(m_hasAdvance, 0, m_size * sizeof(*m_hasAdvance));
	memset(m_turnsSinceOffered, 0, m_size * sizeof(uint16));
	m_total_cost = 0;
	m_discovered = 0;
	m_researching = -1;
	m_theLastAdvanceEnabledThisManyAdvances = 0;
}

Advances::Advances(sint32 num)
{
	m_size = num;
    Assert(m_size);
	m_hasAdvance = new uint8[m_size];
	memset(m_hasAdvance, 0, m_size * sizeof(uint8));
	m_canResearch = new uint8[m_size];
	m_turnsSinceOffered = new uint16[m_size];
	memset(m_turnsSinceOffered, 0, m_size * sizeof(uint16));
	m_total_cost = 0;
	m_discovered = 0;
	m_researching = -1;
	m_theLastAdvanceEnabledThisManyAdvances = 0;
}

Advances::~Advances()
{
	Assert(m_hasAdvance != NULL);
	if(m_hasAdvance) {
		delete [] m_hasAdvance;
		m_hasAdvance = NULL;
	}

	
	if (m_canResearch) {
		delete [] m_canResearch;
		m_canResearch = NULL;
	}

	if(m_turnsSinceOffered) {
		delete [] m_turnsSinceOffered;
		m_turnsSinceOffered = NULL;
	}
}

void Advances::SetOwner(PLAYER_INDEX o)
{
	m_owner = o; 

	m_age = 0;
}

void Advances::Copy(Advances *copy)
{
	*this = *copy;
	if(m_hasAdvance) {
		m_hasAdvance = new uint8[m_size];
		memcpy(m_hasAdvance, copy->m_hasAdvance, m_size * sizeof(uint8));
	}

	if(m_canResearch) {
		m_canResearch = new uint8[m_size];
		memcpy(m_canResearch, copy->m_canResearch, m_size * sizeof(uint8));
	}

	if(m_turnsSinceOffered) {
		m_turnsSinceOffered = new uint16[m_size];
		memcpy(m_turnsSinceOffered, copy->m_turnsSinceOffered, m_size * sizeof(uint16));
	}
}

uint8
Advances::HasAdvance(sint32 index)
{
	if (index >= 1000000)
		return FALSE;

	if(index < 0)
		return TRUE;

	return m_hasAdvance[index];
}

void Advances::UpdateCitySprites(BOOL forceUpdate)
{
	
	if (g_player[m_owner] != NULL) {
		sint32 newAge = 0; 
		if (newAge != m_age || forceUpdate) {
			if(newAge != m_age) {
				g_slicEngine->RunAgeChangeTriggers(m_owner);
			}
			
			m_age = newAge;

			UnitDynamicArray	*allCities =  g_player[m_owner]->GetAllCitiesList();
			
			for (sint32 i=0; i<allCities->Num(); i++) {
				Unit		city = allCities->Access(i);
				UnitActor	*cityActor = city.GetActor();
				if (cityActor) {
					cityActor->ChangeImage(city.GetSpriteState(), city.GetType(), city);
				}
			}

			
			
			
			
            if (!forceUpdate && g_player[m_owner] && g_theCivilisationPool->IsValid(*g_player[m_owner]->m_civilisation)) {
                sint32 i;
                SlicObject *so;

                
                for(i = 1; i < k_MAX_PLAYERS; i++) {
                    if (g_player[i] && (i != m_owner) &&
                        (!g_player[i]->m_isDead)) {
                        if (0  >= m_age)
                            break;
                    }
                }
            
                
                if (i == k_MAX_PLAYERS) {
                    so = new SlicObject("131NewAgeFirst") ;
                    so->AddAge(m_age);
                    so->AddRecipient(m_owner) ;
                    g_slicEngine->Execute(so) ;

                    so = new SlicObject("132NewAgeOthers") ;
                    so->AddCivilisation(m_owner) ;
                    so->AddAge(m_age);
                    so->AddAllRecipientsBut(m_owner) ;
                    g_slicEngine->Execute(so) ;
                }
            }
		}
	}
}

void Advances::SetHasAdvance(AdvanceType advance)
{
	if(m_hasAdvance[advance]) {
		
		
		return;
	}
	if(!g_slicEngine->CallMod(mod_CanPlayerHaveAdvance, TRUE, m_owner, advance))
		return;

	m_hasAdvance[advance] = TRUE;

	const AdvanceRecord	*rec;
	rec = g_theAdvanceDB->Get(advance);
	
	m_canResearch[advance] = FALSE;

	
	if (rec->GetDeepOcean()) {
		if (g_player[m_owner]) {
			g_player[m_owner]->SetDeepOceanVisible(TRUE);
		}

		
		if (g_tiledMap) {
			g_tiledMap->Refresh();
			g_tiledMap->InvalidateMix();
		}
	}

	if(rec->GetCapitalization()) {
		g_player[m_owner]->m_can_build_capitalization = TRUE;
	}

	if(rec->GetInfrastructure()) {
		g_player[m_owner]->m_can_build_infrastructure = TRUE;
	}

	
	if ( rec->GetTransform()) {
		g_player[m_owner]->m_can_use_terra_tab = TRUE;
	}

	













    if (strcmp(g_theStringDB->GetIdStr(g_theAdvanceDB->Get(advance)->m_name),
               "ADVANCE_AERODYNAMICS") == 0) {
        QuickSlic("42IAAirportTip", m_owner, TRUE);
    }

	UpdateCitySprites(FALSE);

	g_player[m_owner]->SetHasAdvance(advance);
}



void 
Advances::GrantAdvance()
{
	Assert(m_researching >= 0 && m_researching < m_size);
	if(m_researching < 0 || m_researching >= m_size)
		return;

	
	
	
	
	DPRINTF(k_DBG_GAMESTATE, ("Advance: Player %d discovered %s\n", m_owner,
							  g_theStringDB->GetNameStr(g_theAdvanceDB->Get(m_researching)->GetName())));
	

	m_discovered++;

	
	m_total_cost = m_total_cost + g_theAdvanceDB->Get(m_researching)->GetCost();


	if(g_network.IsActive() && g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_ADVANCE,
									  m_owner, m_researching, m_discovered,
									  g_player[m_owner]->m_science->GetLevel()));
	}

	sint32 gotAdvance = m_researching;
	SetHasAdvance(m_researching);

	g_player[m_owner]->SetCityRoads();

    
    
	
	
}


void Advances::GiveAdvance(AdvanceType adv, CAUSE_SCI cause, BOOL fromClient) 
{






	DPRINTF(k_DBG_GAMESTATE, ("Advance: Player %d was given %s\n", m_owner,
			g_theAdvanceDB->GetNameStr(adv)));

	if(m_hasAdvance[adv]) {
		
		return;
	}

	if(!g_slicEngine->CallMod(mod_CanPlayerHaveAdvance, TRUE, m_owner, adv))
		
		return;

	sint32 pointCost = g_theAdvanceDB->Get(adv)->GetPowerPoints();
	if(g_network.IsActive() && g_network.SetupMode()) {
		if(g_player[m_owner]->GetPoints() < pointCost)
			
			return;

		
		
		
		if(g_network.IsClient() && m_owner != g_selected_item->GetVisiblePlayer())
			return;

		if(g_network.IsHost()) {
			if(!fromClient && m_owner != g_selected_item->GetVisiblePlayer())
				return;
		}

		g_player[m_owner]->DeductPoints(pointCost);
		if(g_network.IsClient()) {
			g_network.SendAction(new NetAction(NET_ACTION_ADVANCE_CHEAT,
											   adv));
		}
	}


	SetHasAdvance(adv);

	m_discovered++;

	
	m_total_cost = m_total_cost + g_theAdvanceDB->Get(adv)->GetCost();

	if(g_network.IsActive()) {
		if(g_network.IsHost()) {
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_ADVANCE,
										  m_owner, adv, m_discovered,
										  g_player[m_owner]->m_science->GetLevel()));
		}
	}
}

void Advances::GiveAdvancePlusPrerequisites(AdvanceType adv)
{
	if(!g_slicEngine->CallMod(mod_CanPlayerHaveAdvance, TRUE, m_owner, adv))
		return;

	GiveAdvance(adv, CAUSE_SCI_INITIAL, FALSE);
	sint32 i;
	for(i = 0; i < g_theAdvanceDB->Get(adv)->GetNumPrerequisites(); i++) {
		if(adv == g_theAdvanceDB->Get(adv)->GetPrerequisitesIndex(i))
			continue;
		if(!HasAdvance(g_theAdvanceDB->Get(adv)->GetPrerequisitesIndex(i))) {
			GiveAdvancePlusPrerequisites(g_theAdvanceDB->Get(adv)->GetPrerequisitesIndex(i));
		}
	}
}


void Advances::TakeAdvance(AdvanceType adv) 
{






	DPRINTF(k_DBG_GAMESTATE, ("Advance: %s was taken from Player %d", g_theAdvanceDB->GetNameStr(adv), 
		m_owner));

	if(!m_hasAdvance[adv])
		return;

	sint32 pointCost = g_theAdvanceDB->Get(adv)->GetPowerPoints();
	if(g_network.IsActive() && g_network.SetupMode()) {
		g_player[m_owner]->AddPoints(pointCost);
		if(g_network.IsClient()) {
			g_network.SendAction(new NetAction(NET_ACTION_TAKE_ADVANCE_CHEAT,
											   adv));
		}
	}

	m_hasAdvance[adv] = FALSE;
	

	
	m_discovered--;

	g_player[m_owner]->SetCityRoads();
}


void Advances::InitialAdvance(AdvanceType adv) 
{
	DPRINTF(k_DBG_GAMESTATE, ("Advance: Player %d was given %s as an initial advance\n", m_owner,
			g_theAdvanceDB->GetNameStr(adv)));

	if(!g_slicEngine->CallMod(mod_CanPlayerHaveAdvance, TRUE, m_owner, adv))
		return;

	SetHasAdvance(adv);

	
	
	m_total_cost = m_total_cost + g_theAdvanceDB->Get(adv)->GetCost();

	if(g_network.IsActive() && g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_ADVANCE,
									  m_owner, adv, m_discovered));
	}
}


uint8*
Advances::CanResearch()
{
	
    Assert(0 < m_size);
	uint8* research = new uint8[m_size];
	
	memcpy(research, m_canResearch, m_size * sizeof(uint8));
	return research;
}

BOOL Advances::CanResearch(sint32 advance)
{
	return (BOOL)m_canResearch[advance];
}

void Advances::ResetCanResearch(sint32 justGot)
{
	sint32 num = 0;
	sint32 i;
	sint32 numEnabled = 0;
	
	
	for(sint32 adv = 0; adv < m_size; adv++) {
		if(m_hasAdvance[adv]) {
			
			m_canResearch[adv] = FALSE;
		} else {
			
			const AdvanceRecord* rec = g_theAdvanceDB->Get(adv);
			uint8 canResearch = TRUE;
			BOOL  justEnabled = FALSE;
			
			
			
			
			if(g_network.IsActive() && rec->GetAgeIndex() > g_theGameSettings->GetEndingAge()) {
				
				
				canResearch = FALSE;
			} else if(!g_slicEngine->CallMod(mod_CanPlayerHaveAdvance, TRUE, m_owner, rec->GetIndex())) {
				canResearch = FALSE;
			} else {
				
				for(sint32 prereq = 0; prereq < rec->GetNumPrerequisites(); prereq++) {
					if(rec->GetIndex() == rec->GetPrerequisitesIndex(prereq)) {
						canResearch = FALSE;
						continue;
					}

					if(!m_hasAdvance[rec->GetPrerequisitesIndex(prereq)]) {
						
						canResearch = FALSE;
					}
					if(rec->GetPrerequisitesIndex(prereq) == justGot)
						justEnabled = TRUE;
				}
			}
			m_canResearch[adv] = canResearch;
			if(canResearch) {
				num++;
				if(justEnabled) {
					numEnabled++;
					m_turnsSinceOffered[adv] = k_MAX_ADVANCE_TURNS;
				} else if(m_turnsSinceOffered[adv] < k_MAX_ADVANCE_TURNS) {
					m_turnsSinceOffered[adv]++;
				}
			}
		}
	}

	sint32 minChoices = g_theConstDB->AdvanceChoicesMin();
	sint32 numOffered = 0;
	sint32 turnCutoff = 0xffff;
	sint32 total = 0;
	
	
	if(num <= minChoices) {
		
		
		goto done;
	}

	
	
	sint32 j, numChosen;
	uint16 histogram[k_MAX_ADVANCE_TURNS + 1];
	memset(histogram, 0, sizeof(uint16) * (k_MAX_ADVANCE_TURNS + 1));

	for(i = 0; i < m_size; i++) {
		if(!m_canResearch[i])
			continue;
		histogram[m_turnsSinceOffered[i]]++;
	}

	
	for(i = k_MAX_ADVANCE_TURNS; i >= 0; i--) {
		total += histogram[i];
		if(total >= minChoices) {
			
			turnCutoff = i;
			break;
		}
	}

	numChosen = 0;
	for(i = 0; i < m_size; i++) {
		if(m_canResearch[i] && m_turnsSinceOffered[i] < turnCutoff) {
			if(m_turnsSinceOffered[i] < k_MAX_ADVANCE_TURNS)
				m_turnsSinceOffered[i]++;
			m_canResearch[i] = 0;
		} else if(m_canResearch[i]) {
			numChosen++;
		}
	}

	if(numChosen == minChoices) {
		goto done;
	}

	for(i = k_MAX_ADVANCE_TURNS; i >= turnCutoff; i--) {
		if(histogram[i] > 0) {
			if(numOffered + histogram[i] > minChoices) {
				
				sint32 howMany = minChoices - numOffered;
				uint32 *possible = new uint32[m_size];
				sint32 p = 0;
				
				for(j = 0; j < m_size; j++) {
					if(m_canResearch[j] && m_turnsSinceOffered[j] == i) {
						possible[p] = j;
						p++;
					}
				}
				Assert(p >= howMany);
				sint32 h;

				
				
				for(h = p; h > howMany; h--) {
					sint32 which = g_rand->Next(p);
					m_canResearch[possible[which]] = 0;
					memmove(&possible[which], &possible[which + 1], p - which - 1);
					p--;
				}
				delete [] possible;
				goto done;
			} else {
				numOffered += histogram[i];
			}
		}
	}
		
#if 0
	if(num > g_theConstDB->AdvanceChoicesMin()) {
		sint32 howMany = g_rand->Next(g_theConstDB->AdvanceChoicesMax() - 
									  g_theConstDB->AdvanceChoicesMin()) +
			g_theConstDB->AdvanceChoicesMin();
		while(num > howMany) {
			sint32 origNum = num;
			sint32 which = g_rand->Next(num);
			sint32 found = 0;
			for(i = 0; i < m_size; i++) {
				if(m_canResearch[i]) {
					if(found == which) {
						m_canResearch[i] = FALSE;
						num--;
						break;
					}
					found++;
				}
			}
			Assert(num != origNum);
			if(num == origNum)
				break;
		}
	}

	

	
	if(m_theLastAdvanceEnabledThisManyAdvances <= 4) {
		for(i = 0; i < m_size; i++) {
			if(!m_hasAdvance[i] && !m_canResearch[i]) {

				
				if(g_theAdvanceDB->RequiresProbeRecovery(i) &&
				   !g_player[m_owner]->m_hasRecoveredProbe) {
					continue;
				}

				const AdvanceRecord *rec = g_theAdvanceDB->Get(i);
				BOOL justEnabled = FALSE;
				BOOL canResearch = TRUE;
				for(sint32 prereq = 0; prereq < rec->m_nPrereq; prereq++) {
					if(!m_hasAdvance[rec->m_prereq[prereq]]) {
						canResearch = FALSE;
						break;
					}
					if(rec->m_prereq[prereq] == justGot) {
						justEnabled = TRUE;
					}
				}
				if(justEnabled && canResearch) {
					m_canResearch[i] = TRUE;
				}
			}
		}
	}
#endif
 done:
	m_theLastAdvanceEnabledThisManyAdvances = numEnabled;

	AddAlienLifeAdvance();

	for(i = 0; i < m_size; i++) {
		if(m_canResearch[i]) {
			m_turnsSinceOffered[i] = 0;
		}
	}

}

void Advances::AddAlienLifeAdvance()
{
	
	

	
	
	
	
	
	
	
}


uint8*
Advances::CanAskFor(Advances* otherCivAdvances, sint32 &num) 
{
	
    Assert(0<m_size);
	uint8* askFor = new uint8[m_size];

	num = 0;
	for(sint32 adv = 0; adv < m_size; adv++) {
		if(!m_hasAdvance[adv] && otherCivAdvances->m_hasAdvance[adv] &&
		   g_slicEngine->CallMod(mod_CanPlayerHaveAdvance, TRUE, m_owner, adv)) {
			
			num++;
			askFor[adv] = TRUE;
		} else {
			askFor[adv] = FALSE;
		}
	}
	
	return askFor;
}


uint8*
Advances::CanOffer(Advances* otherCivAdvances, sint32 &num) 
{
	
    Assert(0<m_size);
	uint8* offer = new uint8[m_size];

	num = 0;
	for(sint32 adv = 0; adv < m_size; adv++) {
		if(m_hasAdvance[adv] && !otherCivAdvances->m_hasAdvance[adv] &&
		   g_slicEngine->CallMod(mod_CanPlayerHaveAdvance, TRUE, otherCivAdvances->m_owner, adv)) {
			
			num++;
			offer[adv] = TRUE;
		} else {
			offer[adv] = FALSE;
		}
	}
	
	return offer;
}













sint32 Advances::GetPollutionSizeModifier(void) const
	{
	sint32	i,
			bonus = 0 ;

	for (i=0; i<m_size; i++)
		{
		if (m_hasAdvance[i])
			bonus += (sint32) g_theAdvanceDB->Get(i)->GetPollutionSizeModifier() ;

		}

	return (bonus) ;
	}













sint32 Advances::GetPollutionProductionModifier(void) const
	{
	sint32	i,
			bonus = 0 ;

	for (i=0; i<m_size; i++)
		{
		if (m_hasAdvance[i])
			bonus += (sint32)g_theAdvanceDB->Get(i)->GetPollutionProductionModifier() ;

		}

	return (bonus) ;
	}


sint32
Advances::GetCost()
{
	return GetCost(m_researching);
}

sint32 Advances::GetCost(const AdvanceType adv)
{
	if(!g_player[m_owner])
		return 0x7fffffff;

	sint32 cost = g_player[m_owner]->GetScienceHandicap() * m_discovered + 
		g_theAdvanceDB->Get(adv)->GetCost();

	if(g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT &&
	   !(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))) {
		sint32 age = 0; 
		if(age >= k_MAX_AGES)
			age = k_MAX_AGES - 1;

		cost = sint32(ceil(double(cost) * 
							g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetAiTechnologyCost(m_owner,age)));
	} else {
		cost += sint32(ceil((double(cost) * g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->m_human_science_bonus)));
	}

	return cost;
}



sint32 Advances::FindLevel(AdvanceRecord* rec)
{
	if(rec->GetNumPrerequisites() == 0)
		return 0;
	else {
		sint32 maxLevel = -1;
		for(sint32 prereq = 0 ; prereq < rec->GetNumPrerequisites(); prereq++) {			
			AdvanceRecord* prereqRecord = 
				g_theAdvanceDB->Access(rec->GetPrerequisitesIndex(prereq));
			if(rec == prereqRecord)
				continue;

			if(prereqRecord != rec) {
				sint32 level = FindLevel(prereqRecord);
				if(level > maxLevel)
					maxLevel = level;
			}
		}
		return 1 + maxLevel;
	}
}

#ifdef _DEBUG

void
Advances::DebugDumpTree()
{
    Assert(0<m_size);
	sint32* level = new sint32[m_size];
	sint32 numLevels = 0;

	for(sint32 i = 0; i < m_size; i++) {
		AdvanceRecord* rec = g_theAdvanceDB->Access(i);
		level[i] = FindLevel(rec);
		if(level[i] > numLevels - 1)
			numLevels = level[i] + 1;
	}
	for(sint32 l = 0; l < numLevels; l++) {
		DPRINTF(k_DBG_INFO, ("Advance: level %d:\n", l));
		for(i = 0; i < m_size; i++) {
			if(level[i] == l) {
				DPRINTF(k_DBG_INFO, ("Advance: %s\n", 
									 g_theAdvanceDB->GetNameStr(i)));
			}
		}
		DPRINTF(k_DBG_INFO, ("\n"));
	}

	delete [] level;
}
#endif

void
Advances::Serialize(CivArchive& archive)
{
    CHECKSERIALIZE

	if(archive.IsStoring()) {
		archive.StoreChunk((uint8 *)&m_owner, ((uint8 *)&m_discovered)+sizeof(m_discovered));

		archive.Store((uint8*)m_hasAdvance, m_size * sizeof(uint8));
		archive.Store((uint8*)m_canResearch, m_size * sizeof(uint8));
		archive.Store((uint8*)m_turnsSinceOffered, m_size * sizeof(uint16));
	} else {
		archive.LoadChunk((uint8 *)&m_owner, ((uint8 *)&m_discovered)+sizeof(m_discovered));

		if(m_hasAdvance != NULL) {
			delete [] m_hasAdvance;
		}
        Assert(m_size);
		m_hasAdvance = new uint8[m_size];
		archive.Load((uint8*)m_hasAdvance, m_size * sizeof(uint8));
		archive.Load((uint8*)m_canResearch, m_size * sizeof(uint8));
		archive.Load((uint8*)m_turnsSinceOffered, m_size * sizeof(uint16));
	}
}














uint32 Advances_Advances_GetVersion(void)
	{
	return (k_ADVANCES_VERSION_MAJOR<<16 | k_ADVANCES_VERSION_MINOR) ;
	}

sint32 Advances::GetMinPrerequisites(sint32 adv) const
{
	if(m_hasAdvance[adv])
		return 0;

	const AdvanceRecord *rec = g_theAdvanceDB->Get(adv);
	sint32 prereq;
	sint32 totalneeded = 0;
	for(prereq = 0; prereq < rec->GetNumPrerequisites(); prereq++) {
		if(rec->GetIndex() == rec->GetPrerequisitesIndex(prereq)) {
			continue;
		}

		if(!m_hasAdvance[rec->GetPrerequisitesIndex(prereq)]) {
			
			totalneeded += GetMinPrerequisites(rec->GetPrerequisitesIndex(prereq));
		}
	}
	return totalneeded + 1; 
}

sint32 Advances::GetProjectedScience()
{
	UnitDynamicArray *cities = g_player[m_owner]->m_all_cities;
	sint32 s = 0, i;
	for(i = 0; i < cities->Num(); i++) {
		
			
			
		
		
		
		

		
		
		s += cities->Access(i).CD()->GetProjectedScience();
		
	}

	if(m_owner == g_selected_item->GetVisiblePlayer() && !g_network.IsClient()) {
		MainControlPanel::SelectedCity();
	}

	return s;
#if 0
	sint32 i;
	
	
	
	
	sint32 wages = 0;
	UnitDynamicArray *cities = g_player[m_owner]->m_all_cities;
	for(i = 0; i < cities->Num(); i++) {
		wages += cities->Access(i).AccessData()->GetCityData()->CalcWages((sint32)g_player[m_owner]->GetWagesPerPerson());
	}

	sint32 totalTrade = 0;	
	sint32 totalGrossGold = 0;
	sint32 totalScience = 0, totalTradeScience = 0, totalPopScience = 0;
	sint32 projScience, projPopScience, projGrossGold, projNetGold;
	sint32 projTradeBeforeCrime, projWages;
	sint32 totalUpkeep = 0;
	sint32 wonderLevel = wonderutil_GetDecreaseMaintenance(g_player[m_owner]->m_builtWonders);

	for(i = g_player[m_owner]->m_all_cities->Num() - 1; i >= 0; i--) {
		Unit city = g_player[m_owner]->m_all_cities->Access(i);
		city.AccessData()->GetCityData()->
			GetDetailedProjectedTradeScience(projGrossGold, projNetGold, projScience,
											 projPopScience, projTradeBeforeCrime,
											 projWages);
		totalTrade += projNetGold;
		totalGrossGold += projGrossGold;

		totalUpkeep += buildingutil_GetTotalUpkeep(city.GetData()->GetCityData()->GetImprovements(), wonderLevel);
		totalPopScience += projPopScience;
	}

	totalTrade += g_player[m_owner]->CalcWonderGold();

	if(totalTrade - wages - totalUpkeep > 0) {
		g_player[m_owner]->m_gold->SetConsiderForScience(double(totalTrade - wages - totalUpkeep) / double(totalGrossGold));

		for(i = g_player[m_owner]->m_all_cities->Num() - 1; i >= 0; i--) {
			g_player[m_owner]->m_all_cities->Access(i).AccessData()->GetCityData()->
				GetDetailedProjectedTradeScience(projGrossGold, projNetGold, projScience,
												 projPopScience, projTradeBeforeCrime,
												 projWages);
			totalTradeScience += projScience;
		}
		totalTradeScience = sint32(double(totalTradeScience) *  g_player[m_owner]->GetKnowledgeCoef());
		double w = 0.01 * double(wonderutil_GetIncreaseKnowledgePercentage(g_player[m_owner]->GetBuiltWonders()));
		totalTradeScience += sint32(totalTradeScience * w);
	} else {
		totalTradeScience = 0;
	}
	totalScience = totalTradeScience + totalPopScience;
	return totalScience;
#endif
}

sint32 Advances::TurnsToNextAdvance(AdvanceType adv)
{
	if(adv < 0)
		adv = m_researching;

	sint32 totalScience = GetProjectedScience();
	sint32 scienceNeeded = GetCost(adv) - g_player[m_owner]->m_science->GetLevel();

	if(!totalScience)
		return -1;

	return scienceNeeded / totalScience;
}

void Advances::SetResearching(AdvanceType adv)
{ 
	if(!g_slicEngine->CallMod(mod_CanPlayerHaveAdvance, TRUE, m_owner, adv))
		return;

	m_researching = adv; 

	g_slicEngine->RunTrigger(TRIGGER_LIST_PLAYER_RESEARCHING,
							 ST_ADVANCE, adv,
							 ST_PLAYER, m_owner,
							 ST_END);
}
