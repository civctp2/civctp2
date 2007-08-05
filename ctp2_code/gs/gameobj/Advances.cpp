//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Advance (tech) handling
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
// -None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Safeguard FindLevel against infinite recursion.
// - Speeded up goody hut advance and unit selection.
// - Replaced old civilisation database by new one. (Aug 22nd 2005 Martin Gühmann)
// - Fixed GetPollutionProductionModifier (June 11st 2005 Martin Gühmann)
// - Fixed GetPollutionSizeModifier (June 11st 2005 Martin Gühmann)
// - Added checks for advances requiring goods, cultureonly, govt only
// - Added EitherPreRequisite to allow flexible tech tree like civ4
// - Added FractionComplete methods. (Feb 4th 2007 Martin Gühmann)
// - Added commodityGold and GoldSupport to projected science
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"                 // pre-compiled header
#include "Advances.h"           // own declarations

#include "AdvanceRecord.h"
#include "civarchive.h"
#include "player.h"             // g_player
#include "WonderRecord.h"
#include "AgeRecord.h"
#include "StrDB.h"              // g_theStringDB
#include "network.h"
#include "net_info.h"
#include "net_action.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "QuickSlic.h"
#include "Civilisation.h"
#include "AICause.h"
#include "SelItem.h"            // g_selected_item
#include "UnitActor.h"
#include "tiledmap.h"           // g_tiledMap
#include "Unit.h"
#include "Sci.h"
#include "Gold.h"
#include "DifficultyRecord.h"   // g_theDifficultyDB
#include "Diffcly.h"
#include "profileDB.h"          // g_theProfileDB
#include "ConstRecord.h"        // g_theConstDB
#include "RandGen.h"            // g_rand
#include "statswindow.h"
#include "CivilisationPool.h"   // g_theCivilisationPool
#include "GameSettings.h"
#include "AgeRecord.h"
#include "BuildingRecord.h"
#include "UnitDynArr.h"
#include "UnitRecord.h"
#include "UnitData.h"
#include "buildingutil.h"
#include "wonderutil.h"
#include "MainControlPanel.h"
#include <stdexcept>            // overflow_error

namespace
{
    char const  REPORT_ADVANCE_LOOP[]   = "Advance loop detected";
    char const  REPORT_ADVANCE_SELF[]   = "Advance undiscoverable";
}


#define k_MAX_ADVANCE_TURNS 1000

#define k_ADVANCES_VERSION_MAJOR	0
#define k_ADVANCES_VERSION_MINOR	0


Advances::Advances(size_t a_Count)
:
    m_owner                                 (PLAYER_UNASSIGNED),
    m_size                                  (a_Count),
    m_researching                           (-1),
    m_age                                   (0),
    m_theLastAdvanceEnabledThisManyAdvances (0), 
    m_total_cost                            (0),
    m_discovered                            (0), 
    m_hasAdvance                            (NULL),
    m_canResearch                           (NULL),
    m_turnsSinceOffered                     (NULL)
{
    Assert(m_size);
	m_hasAdvance        = new uint8[m_size];
	m_canResearch       = new uint8[m_size];
	m_turnsSinceOffered = new uint16[m_size];

    std::fill(m_hasAdvance, m_hasAdvance + m_size, 0);
    std::fill(m_canResearch, m_canResearch + m_size, 0);
    std::fill(m_turnsSinceOffered, m_turnsSinceOffered + m_size, 0);
}

Advances::Advances(Advances const & a_Original)
:
    m_owner                                 (a_Original.m_owner),
    m_size                                  (a_Original.m_size),    
	m_researching                           (a_Original.m_researching),
    m_age                                   (a_Original.m_age),
    m_theLastAdvanceEnabledThisManyAdvances (a_Original.m_theLastAdvanceEnabledThisManyAdvances), 
	m_total_cost                            (a_Original.m_total_cost),
	m_discovered                            (a_Original.m_discovered), 
	m_hasAdvance                            (NULL),
	m_canResearch                           (NULL),
	m_turnsSinceOffered                     (NULL)
{
	m_hasAdvance        = new uint8[m_size];
	m_canResearch       = new uint8[m_size];
	m_turnsSinceOffered = new uint16[m_size];

    std::copy(a_Original.m_hasAdvance, 
              a_Original.m_hasAdvance + m_size, 
              m_hasAdvance
             );
    std::copy(a_Original.m_canResearch, 
              a_Original.m_canResearch + m_size, 
              m_canResearch
             );
    std::copy(a_Original.m_turnsSinceOffered, 
              a_Original.m_turnsSinceOffered + m_size, 
              m_turnsSinceOffered
             );
}

Advances::~Advances()
{
	delete [] m_hasAdvance;
	delete [] m_canResearch;
	delete [] m_turnsSinceOffered;
}

Advances & Advances::operator = (Advances const & a_Original)
{
    if (this != &a_Original)
    {
        m_owner                                 = a_Original.m_owner;
        m_size                                  = a_Original.m_size;   
	    m_researching                           = a_Original.m_researching;
        m_age                                   = a_Original.m_age;
        m_theLastAdvanceEnabledThisManyAdvances = 
            a_Original.m_theLastAdvanceEnabledThisManyAdvances;
	    m_total_cost                            = a_Original.m_total_cost;
	    m_discovered                            = a_Original.m_discovered; 

        delete [] m_hasAdvance;
	    delete [] m_canResearch;
	    delete [] m_turnsSinceOffered;

	    m_hasAdvance        = new uint8[m_size];
	    m_canResearch       = new uint8[m_size];
	    m_turnsSinceOffered = new uint16[m_size];

        std::copy(a_Original.m_hasAdvance, 
                  a_Original.m_hasAdvance + m_size, 
                  m_hasAdvance
                 );
        std::copy(a_Original.m_canResearch, 
                  a_Original.m_canResearch + m_size, 
                  m_canResearch
                 );
        std::copy(a_Original.m_turnsSinceOffered, 
                  a_Original.m_turnsSinceOffered + m_size, 
                  m_turnsSinceOffered
                 );
    }

    return *this;
}

void Advances::SetOwner(PLAYER_INDEX o)
{
	m_owner = o; 
}

bool
Advances::HasAdvance(sint32 index) const
{
	if (index >= m_size)
		return false;

	if(index < 0)
		return true;

	return m_hasAdvance[index] != 0;
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
                        if (0 >= m_age)
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
	if (   !g_player[m_owner]       // non-existing player
        || m_hasAdvance[advance]    // advance already known
        || !g_slicEngine->CallMod   // forbidden by game settings
                (mod_CanPlayerHaveAdvance, TRUE, m_owner, advance)
       )  
    {
		return;
	}

	m_hasAdvance[advance]   = TRUE;
	m_canResearch[advance]  = FALSE;

    const AdvanceRecord	* rec = g_theAdvanceDB->Get(advance);
	
	if (rec->GetDeepOcean()) 
    {
		g_player[m_owner]->SetDeepOceanVisible(TRUE);

		if (g_tiledMap) 
        {
			g_tiledMap->Refresh();
			g_tiledMap->InvalidateMix();
		}
	}

	if (rec->GetCapitalization()) 
    {
		g_player[m_owner]->m_can_build_capitalization = TRUE;
	}

	if (rec->GetInfrastructure()) 
    {
		g_player[m_owner]->m_can_build_infrastructure = TRUE;
	}

	if (rec->GetTransform()) 
    {
		g_player[m_owner]->m_can_use_terra_tab = TRUE;
	}

    if (strcmp(g_theStringDB->GetIdStr(g_theAdvanceDB->Get(advance)->m_name),
               "ADVANCE_AERODYNAMICS") == 0) 
    {
        QuickSlic("42IAAirportTip", m_owner);
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

	SetHasAdvance(m_researching);

	g_player[m_owner]->SetCityRoads();

    
    
	
	
}


void Advances::GiveAdvance(AdvanceType adv, CAUSE_SCI cause, BOOL fromClient) 
{
	DPRINTF(k_DBG_GAMESTATE, ("Advance: Player %d was given %s\n", m_owner,
			g_theAdvanceDB->GetNameStr(adv)));

	if (    m_hasAdvance[adv]           // already known
         || !g_slicEngine->CallMod      // forbidden by game settings
                (mod_CanPlayerHaveAdvance, TRUE, m_owner, adv) 
       ) 
    {
		return;
	}

	if (g_network.IsActive() && g_network.SetupMode()) 
    {
        // Multiplayer game setup

	    sint32 const pointCost = g_theAdvanceDB->Get(adv)->GetPowerPoints();

		if (g_player[m_owner]->GetPoints() < pointCost)
			return; // Too expensive

		if (g_network.IsClient() && m_owner != g_selected_item->GetVisiblePlayer())
			return; // Not for me

		if (g_network.IsHost()) 
        {
			if (!fromClient && m_owner != g_selected_item->GetVisiblePlayer())
				return; // Not for me
		}

		g_player[m_owner]->DeductPoints(pointCost);
		
        if (g_network.IsClient()) 
        {
			g_network.SendAction(new NetAction(NET_ACTION_ADVANCE_CHEAT, adv));
		}
	}


	SetHasAdvance(adv);

	++m_discovered;
	m_total_cost += g_theAdvanceDB->Get(adv)->GetCost();

	if (g_network.IsActive() && g_network.IsHost()) 
    {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_ADVANCE,
									  m_owner, adv, m_discovered,
									  g_player[m_owner]->m_science->GetLevel()
                                     )
                         );
	}
}

void Advances::GiveAdvancePlusPrerequisites(AdvanceType adv)
{
	GiveAdvance(adv, CAUSE_SCI_INITIAL, FALSE);

	for (sint32 i = 0; i < g_theAdvanceDB->Get(adv)->GetNumPrerequisites(); i++) 
    {
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

	if(g_network.IsActive() && g_network.SetupMode()) {
	    sint32 pointCost = g_theAdvanceDB->Get(adv)->GetPowerPoints();
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

	
	
	m_total_cost += g_theAdvanceDB->Get(adv)->GetCost();

	if(g_network.IsActive() && g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_ADVANCE,
									  m_owner, adv, m_discovered));
	}
}


uint8 * Advances::CanResearch() const
{
	uint8 *  research = new uint8[m_size];
    std::copy(m_canResearch, m_canResearch + m_size, research);
	return research;
}

BOOL Advances::CanResearch(sint32 advance) const
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

/////////////////EitherPrerequisite
				if(rec->GetNumEitherPrerequisites() > 0) {
					bool    found   = false;

                    for (sint32 either = 0; either < rec->GetNumEitherPrerequisites(); either++) 
                    {
						if (rec->GetIndex() == rec->GetEitherPrerequisitesIndex(either)) 
                        {
							found = true;
					        if (rec->GetEitherPrerequisitesIndex(either) == justGot)
                            {
						        justEnabled = TRUE;
                            }
							break;
						}
					}

					if(!found)
						canResearch = FALSE;
				}
////////////// PreReq govt techs (but how do you remove them?)
				if(rec->GetNumGovernmentType() > 0) {
					sint32 g;
					bool found = false;
					for(g = 0; g < rec->GetNumGovernmentType(); g++) {
						if(rec->GetGovernmentTypeIndex(g) == g_player[m_owner]->GetGovernmentType()) {
						found = TRUE; //fixed to found
						break;
						}
					}
					if(found){
						justEnabled = TRUE;
					}
					
					if(!found)
						canResearch = FALSE;
					}	
////////////// CultureOnly Techs	

				if(rec->GetNumCultureOnly() > 0) {
					sint32 s;
					bool found = false;
					for(s = 0; s < rec->GetNumCultureOnly(); s++) {
						if(rec->GetCultureOnlyIndex(s) == g_player[m_owner]->GetCivilisation()->GetCityStyle()) {
							found = TRUE; //fixed to found
							break;
						}
					}
					if(found){
						justEnabled = TRUE;
					}
					
					if(!found)
						canResearch = FALSE;
					}
/////////////////EMOD for Advances requiring Goods
				if(rec->GetNumNeedsCityGoodAnyCity()) {
		
					sint32 i, g;
					sint32 n = g_player[m_owner]->m_all_cities->Num();
					bool goodavail = false;

					for(i = 0; i < n; i++) {
						for(g = 0; g < rec->GetNumNeedsCityGoodAnyCity(); g++) {
							if(g_player[m_owner]->m_all_cities->Access(i).AccessData()->GetCityData()->HasNeededGood(rec->GetNeedsCityGoodAnyCityIndex(g))){ 
								goodavail = true;
								break;
							}
						}
					}  //moved goodavail out of for loop
					if(goodavail){
						justEnabled = TRUE;
					}
					if(!goodavail)
						canResearch = FALSE;
				}
//////////////////////END EMOD


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

	sint32 minChoices = g_theConstDB->Get(0)->GetAdvanceChoicesMin();
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

//	if(rec->GetNumNeedsCityGood() > 0) {  //EMOD use as template for EitherPreReq, for more fluid tech tree
//		sint32 g;
//		bool found = false;
//		for(g = 0; g < rec->GetNumEitherPrerequisites(); g++) {
////				for(sint32 prereq = 0; prereq < rec->GetNumPrerequisites(); prereq++) {
//					if(rec->GetIndex() == rec->GetPrerequisitesIndex(prereq)) {
//						canResearch = FALSE;
//						continue;
//					}
//
//					if(!m_hasAdvance[rec->GetPrerequisitesIndex(prereq)]) {
//						
//						canResearch = FALSE;
//					}
//					if(rec->GetPrerequisitesIndex(prereq) == justGot)
////						justEnabled = TRUE;
//			if(m_hasAdvance[rec->GetEitherPrerequisitesIndex(g)] > 0){
//				canResearch = true;
//				break;
//			}
//		}
//		if(!canResearch)
//			return FALSE;
//	}

// Added by E - checks all cities for buying or collecting a good, but its either/or not AND; this is to have tech's require resources
//	if(rec->GetNumNeedsCityGoodAnyCity()) {
//		sint32 i, g;
//		bool goodavail = false;
//
//			for(i = 0; i < m_all_cities->Num(); i++) {
//				for(g = 0; g < rec->GetNumNeedsCityGoodAnyCity(); g++) {
//					if(m_all_cities->Access(i).AccessData()->GetCityData()->HasEitherGood(rec->GetNeedsCityGoodAnyCityIndex(g))){ 
//						goodavail = true;
//						break;
//					}
//				}
//					if(goodavail){
//					break;
//					}
//			}
//			if(!goodavail)
//			return FALSE;
//	}
//

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
Advances::CanAskFor(Advances* otherCivAdvances, sint32 &num) const
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
Advances::CanOffer(Advances* otherCivAdvances, sint32 &num) const 
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












double Advances::GetPollutionSizeModifier(void) const
{
	double bonus = 0.0;
	double value = 0.0;

	for (sint32 i = 0; i < m_size; ++i)
	{
		if(m_hasAdvance[i]
		&& g_theAdvanceDB->Get(i)->GetPollutionSizeModifier(value)
		){
			bonus += value;
		}
    }

	return bonus;
}

double Advances::GetPollutionProductionModifier(void) const
{
	double bonus = 0.0;
	double value = 0.0;

	for (sint32 i = 0; i < m_size; ++i)
	{
		if(m_hasAdvance[i]
		&& g_theAdvanceDB->Get(i)->GetPollutionProductionModifier(value)
		){
			bonus += value;
		}
	}

	return bonus;
}


sint32
Advances::GetCost() const
{
	return GetCost(m_researching);
}

sint32 Advances::GetCost(const AdvanceType adv) const
{
	if(!g_player[m_owner])
		return 0x7fffffff;

	sint32 cost = g_player[m_owner]->GetScienceHandicap() * m_discovered + 
		g_theAdvanceDB->Get(adv)->GetCost();

	if(g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT &&
	   !(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))) {
		sint32 age = 0; 
		cost = static_cast<sint32>(ceil(static_cast<double>(cost) * 
		                           diffutil_GetAiTechnologyCost(g_theGameSettings->GetDifficulty(), m_owner, age)));
	} else {
		cost += static_cast<sint32>(ceil(static_cast<double>(cost) * 
		          g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetHumanScienceBonus()));
	}

	return cost;
}

//----------------------------------------------------------------------------
//
// Name       : Advances::FindLevel
//
// Description: Find level (length of prerequisite chain) of an advance.
//
// Parameters : rec				: advance description in database
//				fromLevel		: length of chain reaching this advance
//
// Globals    : g_theAdvanceDB	: advance database
//
// Returns    : sint32			: length of largest chain of prerequisites
//
// Remark(s)  : throws an overflow_error when a loop is detected in the chain.
//
//----------------------------------------------------------------------------
sint32 Advances::FindLevel
(
	AdvanceRecord const * const	rec, 
	sint32 const				fromLevel
) const
{
	if (fromLevel > g_theAdvanceDB->NumRecords())
	{
		throw std::overflow_error(REPORT_ADVANCE_LOOP);
	}

	sint32	maxLevel = fromLevel;

	for (sint32 prereq = 0; prereq < rec->GetNumPrerequisites(); prereq++) 
	{			
		AdvanceRecord const * prereqRecord = 
			g_theAdvanceDB->Access(rec->GetPrerequisitesIndex(prereq));

        if (rec == prereqRecord)
        {
            throw std::overflow_error(REPORT_ADVANCE_SELF);
        }
        		
		sint32 const level	= FindLevel(prereqRecord, 1 + fromLevel);
		if (level > maxLevel)
			maxLevel = level;
	}

	return maxLevel;
}

	


#ifdef _DEBUG

void
Advances::DebugDumpTree()
{
    Assert(0<m_size);
	sint32* level = new sint32[m_size];
	sint32 numLevels = 0;
	sint32 const	LEVEL_LOOPED	= -1;
	bool			isLoopDetected	= false;
	for(sint32 i = 0; i < m_size; i++) {
		AdvanceRecord* rec = g_theAdvanceDB->Access(i);
		try
		{
			level[i] = FindLevel(rec);
		}
		catch (std::overflow_error &)
		{
			level[i]		= LEVEL_LOOPED;
			isLoopDetected	= true;
		}
		if(level[i] > numLevels - 1)
			numLevels = level[i] + 1;
	}
	for(sint32 l = 0; l < numLevels; l++) {
		DPRINTF(k_DBG_INFO, ("Advance: level %d:\n", l));
		for (sint32 j = 0; j < m_size; j++) 
		{
			if(level[j] == l) {
				DPRINTF(k_DBG_INFO, ("Advance: %s\n", 
									 g_theAdvanceDB->GetNameStr(j)));
			}
		}
		DPRINTF(k_DBG_INFO, ("\n"));
	}

	if (isLoopDetected)
	{
		// Report loops
		DPRINTF(k_DBG_INFO, ("%s\n", REPORT_ADVANCE_LOOP));
		for (sint32 n = 0; n < m_size; ++n)
		{
			if (LEVEL_LOOPED == level[n])
			{
				DPRINTF(k_DBG_INFO, 
					    ("Advance: %s\n", g_theAdvanceDB->GetNameStr(n))
					   );
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

        Assert(m_size);
        delete [] m_hasAdvance;
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

//----------------------------------------------------------------------------
//
// Name       : Advances::GetMinPrerequisites
//
// Description: Get the number of missing steps to get to an advance
//
// Parameters : adv     : the advance to get to
//              limit   : when to stop counting and report it as too advanced
//
// Globals    : g_theAdvanceDB
//
// Returns    : sint32  : the number of missing prerequisite steps
//
// Remark(s)  : - If you already have the advance, the returned value is 0.
//              - If you have all prerequisites for the advance, but do not
//                have the advance itself, the returned value is 1.
//              - If you are missing prerequisites for the advance, the 
//                returned value is the sum of the recursive application of 
//                this function to the missing prerequisites.
//              - If the advance cannot be researched, because it is the 
//                prerequisite of itsself, limit + 1 is returned.
//              - If the limit is reached, the returned value is unspecified,
//                but always larger than the limit.
//
//----------------------------------------------------------------------------
sint32 Advances::GetMinPrerequisites(sint32 adv, sint32 limit) const
{
	if(m_hasAdvance[adv])
	{
		return 0;
	}

	AdvanceRecord const *   rec         = g_theAdvanceDB->Get(adv);
	sint32                  totalneeded = 0;

	for(sint32 i = 0; i < rec->GetNumPrerequisites(); ++i)
	{
		sint32 const prereqDbIndex = rec->GetPrerequisitesIndex(i);
		
		if(rec->GetIndex() != prereqDbIndex)
		{
			if(!m_hasAdvance[prereqDbIndex])
			{
				totalneeded += GetMinPrerequisites(prereqDbIndex, limit - 1);

				if(totalneeded > limit)
				{
					return totalneeded;
				}
			}
		}
		else
		{
			return limit + 1;
		}
	}

	return totalneeded + 1;
}

sint32 Advances::GetMinPrerequisites(sint32 adv) const
{
	return GetMinPrerequisites(adv, m_size * k_MAX_Prerequisites);
}

sint32 Advances::GetProjectedScience() const
{
	UnitDynamicArray *  cities  = g_player[m_owner]->m_all_cities;
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
	//sint32 owner = g_player[m_owner]; //EMOD for modified GetTotalUpkeep

	for(i = g_player[m_owner]->m_all_cities->Num() - 1; i >= 0; i--) {
		Unit city = g_player[m_owner]->m_all_cities->Access(i);
		city.AccessData()->GetCityData()->
			GetDetailedProjectedTradeScience(projGrossGold, projNetGold, projScience,
											 projPopScience, projTradeBeforeCrime,
											 projWages);
		totalTrade += projNetGold;
		totalGrossGold += projGrossGold;

		totalUpkeep += buildingutil_GetTotalUpkeep(city.GetData()->GetCityData()->GetImprovements(), wonderLevel, m_owner);
		totalPopScience += projPopScience;
	}

	totalTrade += g_player[m_owner]->CalcWonderGold();
//added these two from player to be more accurate
/*
	totalTrade += g_player[m_owner]->CommodityMarket();  //emod for calculating commodities should it be in profileDB?
	totalTrade -= g_player[m_owner]->	CalcUnitSupportGold();
	totalTrade -= g_player[m_owner]->	CalcCitySupportGold();
*/
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

sint32 Advances::TurnsToNextAdvance(AdvanceType adv) const
{
	if(adv < 0)
		adv = m_researching;

	sint32 totalScience = GetProjectedScience();
    if (0 == totalScience)
    {
        return -1;
    }

	sint32 scienceNeeded = GetCost(adv) - g_player[m_owner]->m_science->GetLevel();
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

double Advances::FractionComplete(AdvanceType adv) const
{
	if(adv < 0)
		adv = m_researching;

	return static_cast<double>(g_player[m_owner]->m_science->GetLevel()) / static_cast<double>(GetCost(adv));
}

double Advances::FractionComplete() const
{
	return FractionComplete(m_researching);
}