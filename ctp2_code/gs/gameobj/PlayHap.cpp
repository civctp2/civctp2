#include "c3.h"
#include "Globals.h"

#include "PlayHap.h"
#include "player.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "ConstDB.h"
#include "aui.h"
#include "aui_surface.h"
#include "primitives.h"
#include "aui_window.h"
#include "debugwindow.h"
#include "Unit.h"
#include "dynarr.h"
#include "UnitDynArr.h"
#include "Cell.h"
#include "installationtree.h"

#include "GovernmentRecord.h"
#include "terrainutil.h"

#include "civarchive.h"
#include "network.h"
#include "GovernmentRecord.h"

#include "Army.h"

extern DebugWindow *g_debugWindow;

PlayerHappiness::PlayerHappiness()

{
    m_pad = 0; 

    m_workday_per_person = g_theConstDB->GetBaseWorkday(); 
    m_unitless_workday = 0;

    m_rations_per_person = g_theConstDB->GetBaseRations(); 
    m_unitless_rations = 0; 

    m_wages_per_person = g_theConstDB->GetBaseWages(); 
    m_unitless_wages = 0; 

    m_raw_overseas_defeat = 0.0;
    m_raw_home_defeat =0.0;

    m_overseas_defeat = 0.0;
    m_home_defeat = 0.0;

    m_dist_overseas = 0.0;
    m_overseas = 0.0; 

	m_warDiscontent = 0;
}

void PlayerHappiness::CalcPeaceMovement(Player *p, 
    DynamicArray<Army> &all_armies, UnitDynamicArray &all_cities)

{
    m_raw_overseas_defeat =  ((1.0 - p->GetOverseasDefeatDecay()) * m_raw_overseas_defeat + p->GetOverseasDefeatCoef() * p->GetUnitsLostOverseas()); 
    m_raw_home_defeat = ((1.0 - p->GetHomeDefeatDecay()) * m_raw_home_defeat + p->GetHomeDefeatCoef() * p->GetUnitsLostHome()); 

    m_overseas_defeat = m_raw_overseas_defeat;
    m_home_defeat = m_raw_home_defeat;
    
    Unit nearest; 
    sint32 n, m, i, j; 
    MapPoint apos, cpos, diff; 
    double distance; 
    double d; 
    double radius = p->GetAtHomeRadius();

    n = all_armies.Num(); 
    m = all_cities.Num(); 
    m_dist_overseas = 0;
    for (i=0; i<n; i++) { 

        all_armies[i].GetPos(apos); 

       
  
        if (p->GetNearestCity(apos, nearest, distance) == FALSE) { 
            m_overseas = 0.0; 
            m_overseas_defeat = 0.0; 
            m_home_defeat = 0.0; 
            return;
        }        

        if (distance <= radius) { 
            continue; 
        }

        d = distance - radius; 

        Assert (0 < d);
        m = all_armies[i].Num(); 
        for (j=0; j<m; j++) { 
             if (!all_armies[i][j].IsContentOverseas()) { 
                  m_dist_overseas += double(d); 
             }                   
        }
    }

    m_overseas = p->GetOverseasCoef() * m_dist_overseas; 

	sint32 militaryUnitsOutOfCities = 0;
	n = all_armies.Num();
	for(i = 0; i < n; i++) {
		MapPoint pos;
		all_armies[i].GetPos(pos);
		Cell *cell = g_theWorld->GetCell(pos);
		if(cell->GetCity().m_id != (0))
			continue;
		if (terrainutil_HasFort(pos) && cell->GetOwner() == p->m_owner)
			continue;				
		
		for(j = 0; j < all_armies[i].Num(); j++) {
			if(all_armies[i][j].GetAttack() > 0) {
				militaryUnitsOutOfCities++;
			}
		}
	}


	if(p->m_playerType == PLAYER_TYPE_ROBOT &&
	   !(g_network.IsClient() && g_network.IsLocalPlayer(p->m_owner))) {
		militaryUnitsOutOfCities = 0;
	}

	m_warDiscontent = 0;
	const GovernmentRecord *grec = g_theGovernmentDB->Get(p->m_government_type);
	if(militaryUnitsOutOfCities > grec->GetWarDiscontentMaxUnits()) {
		sint32 diff = militaryUnitsOutOfCities - grec->GetWarDiscontentMaxUnits();
		double total = diff * grec->GetWarDiscontentPerUnit();
		double perCity;
		if(all_cities.Num() < 1)
			perCity = 0;
		else
			perCity = total / all_cities.Num();

		m_warDiscontent = perCity;
	}
}


void PlayerHappiness::SetWorkdayLevel (sint32 h)
{
   
    
    

    m_workday_per_person = - h * g_theConstDB->GetUnitWorkday() + g_theConstDB->GetBaseWorkday() ;

    
    m_unitless_workday = h;
}

double PlayerHappiness::GetWorkdayPerPerson ()
{   return m_workday_per_person; 
}

sint32 PlayerHappiness::GetUnitlessWorkday ()
{
    return m_unitless_workday; 
}


void PlayerHappiness::SetWagesLevel (sint32 w)
{
    
    m_wages_per_person = w * g_theConstDB->GetUnitWages() + g_theConstDB->GetBaseWages();

    
    m_unitless_wages = w;
}

double PlayerHappiness::GetWagesPerPerson ()
{   return m_wages_per_person; 
}

sint32 PlayerHappiness::GetUnitlessWages ()
{
    return m_unitless_wages; 
}


void PlayerHappiness::SetRationsLevel (sint32 r)
{
    
    m_rations_per_person = r * g_theConstDB->GetUnitRations() + g_theConstDB->GetBaseRations();

    
    m_unitless_rations = r;
}

double PlayerHappiness::GetRationsPerPerson () const
{   return m_rations_per_person; 
}

sint32 PlayerHappiness::GetUnitlessRations ()
{
    return m_unitless_rations; 
}

void PlayerHappiness::DisplayWWR()
{
	char str[80];   
	
	sprintf(str, "work    %d work per person:%f  ",  m_unitless_workday, m_workday_per_person); 
	g_debugWindow->AddText(str);
	sprintf(str, "wage    %d wage per person:%f  ",  m_unitless_wages, m_wages_per_person); 
	g_debugWindow->AddText(str);
	
	
	sprintf(str, "rations %d  rations per person:%f ", m_unitless_rations, m_rations_per_person * g_theConstDB->GetBaseRations()); 
	g_debugWindow->AddText(str);
	
	















	
}

void PlayerHappiness::Serialize (CivArchive &archive)

{
    CHECKSERIALIZE

    if (archive.IsStoring()) {
		archive.Store((uint8 *)this, sizeof(PlayerHappiness)) ;
    } else {
		archive.Load((uint8 *)this, sizeof(PlayerHappiness)) ;
    }
}
