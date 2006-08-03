//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Readiness modifiers
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
// - Fixed gold support cost so that it does not break the save game format. (April 29th 2006 Martin Gühmann)
// - Added difficulty dependent AI keep units over budget cheat. (April 29th 2006 Martin Gühmann)
// - Replaced old difficulty database by new one. (April 29th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

// EMOD TODO outcommented but in place to do GoldHunger for Units 1-17-2006
//Also need to add in Player and Citydata??

#include "c3.h"
#include "Readiness.h"
#include "civarchive.h"
#include "DB.h"
#include "GovernmentRecord.h"
#include "Unit.h"
#include "network.h"
#include "WonderRecord.h"
#include "player.h"
#include "AICause.h"
#include "UnitRec.h"
#include "Army.h"
#include "dynarr.h"
#include "TurnCnt.h"
#include "SlicSegment.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "UnitData.h"
#include "wonderutil.h"
#include "DifficultyRecord.h"
#include "GameSettings.h"

MilitaryReadiness::MilitaryReadiness(sint32 a_Owner)
:
    m_delta             (0.0),  
    m_hp_modifier       (1.0), 
    m_cost              (0.0),
    m_percent_last_turn (0.0), 
    m_readinessLevel    (READINESS_LEVEL_WAR),
    m_ignore_unsupport  (FALSE), 
    m_owner             (a_Owner),
    m_turnStarted       (-1),
	m_costGold          (0)
{
}
    
double MilitaryReadiness::GetSupportModifier(sint32 gov) const
{
    const GovernmentRecord *rec = g_theGovernmentDB->Get(gov); 

    switch (m_readinessLevel) { 
    case READINESS_LEVEL_PEACE: 
        return rec->GetReadyPeaceCoef(); 
    case READINESS_LEVEL_ALERT: 
        return rec->GetReadyAlertCoef(); 
    case READINESS_LEVEL_WAR: 
        return rec->GetReadyWarCoef(); 
    default:
        Assert(0); 
        return 0.0;
    }
}

double MilitaryReadiness::GetSpecialForcesSupportModifier(sint32 gov) const
{
	const GovernmentRecord *rec = g_theGovernmentDB->Get(gov); 

	return rec->GetReadyWarCoef();
}

void MilitaryReadiness::BeginTurn(sint32 gov)
{
	if(m_delta < 0) {
		if(m_hp_modifier > GetReadyHP(gov, m_readinessLevel)) {
			m_hp_modifier = GetReadyHP(gov, m_readinessLevel);
			m_turnStarted = -1;
		}
	} else {
		if(m_hp_modifier >= GetReadyHP(gov, m_readinessLevel) - 0.0001) {
			m_hp_modifier = GetReadyHP(gov, m_readinessLevel);
			m_turnStarted = -1;
		} else {
			m_hp_modifier += m_delta;
		}
	}

	g_network.Block(m_owner);
	ENQUEUE();
	g_network.Unblock(m_owner);
}

double MilitaryReadiness::GetHPModifier() const
{
    Assert(0.00001 < m_hp_modifier); 
    return m_hp_modifier; 
}

double MilitaryReadiness::GetReadyHP(sint32 gov, READINESS_LEVEL level) 
{
    const GovernmentRecord *rec = g_theGovernmentDB->Get(gov); 

    switch (level) { 
    case READINESS_LEVEL_PEACE: 
        return rec->GetReadyPeaceHP(); 
    case READINESS_LEVEL_ALERT: 
        return rec->GetReadyAlertHP(); 
    case READINESS_LEVEL_WAR: 
        return rec->GetReadyWarHP(); 
    default:
        Assert(0);
        return 0.0; 
    }
}

void MilitaryReadiness::SupportUnit(const Unit &u, sint32 gov)
{
	m_cost += GetSupportCost(u);

	g_network.Block(m_owner);
	ENQUEUE();
	g_network.Unblock(m_owner);
}

#if 0
//EMOD
void MilitaryReadiness::SupportUnitGold(const Unit &u, sint32 gov)
{
	double unitCostGold = GetSupportCostGold(u);

	m_costGold += unitCostGold;

	g_network.Block(m_owner);
	ENQUEUE();
	g_network.Unblock(m_owner);
}
#endif

double MilitaryReadiness::GetSupportCost(const Unit &u)
{
	if(u.GetIsProfessional())
		return 0.0;

	if(u.GetNeedsNoSupport())
		return 0.0;

	double unitCost;
	if(u.GetDBRec()->GetIsSpecialForces()) {
		unitCost = u.GetShieldHunger() * GetSpecialForcesSupportModifier(g_player[m_owner]->GetGovernmentType());
	} else {
		unitCost = u.GetShieldHunger() * GetSupportModifier(g_player[m_owner]->GetGovernmentType());
	}
	unitCost -= unitCost * 
		double((double)wonderutil_GetReadinessCostReduction(
			g_player[m_owner]->GetBuiltWonders()) / 100.0);

	unitCost *= g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetSupportCoef();
//EMOD for AI
	if(g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetAINoShieldHunger()
		&& g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
			unitCost -= unitCost;
	}

	return unitCost;
}


//EMOD
sint32 MilitaryReadiness::GetSupportCostGold(const Unit &u)
{
	if(u.GetIsProfessional())
		return 0;

	if(u.GetNeedsNoSupport())
		return 0;

	double unitCostGold = 0.0;
	sint32 goldHunger = u.GetDBRec()->GetGoldHunger();
	if(goldHunger > 0) {

		if(u.GetDBRec()->GetIsSpecialForces()) {
			unitCostGold = static_cast<double>(goldHunger) * GetSpecialForcesSupportModifier(g_player[m_owner]->GetGovernmentType());
		} else {
			unitCostGold = static_cast<double>(goldHunger) * GetSupportModifier(g_player[m_owner]->GetGovernmentType());
		}

		unitCostGold -= unitCostGold * 
			(static_cast<double>(wonderutil_GetReadinessCostReduction(
				g_player[m_owner]->GetBuiltWonders())) / 100.0);

		unitCostGold *= g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetSupportCoef();
	}

	//EMOD for AI
	if(g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetAINoGoldHunger()
		&& g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
			unitCostGold -= unitCostGold;
	}

	return static_cast<sint32>(unitCostGold);
}


void MilitaryReadiness::UnsupportUnit(const Unit &u, sint32 gov)
{
   if (m_ignore_unsupport) 
       return; 

   m_cost -= GetSupportCost(u);

   g_network.Block(m_owner);
   ENQUEUE();
   g_network.Unblock(m_owner);
}

void MilitaryReadiness::SetLevel(sint32 gov, DynamicArray<Army> &all_armies, 
              READINESS_LEVEL level, BOOL immediate)
{
	if (level == m_readinessLevel)
		return;

    m_cost = 0.0; 

	sint32 turns;

	if (level < m_readinessLevel || immediate) {
		
		turns = 1;
		m_turnStarted = -1;
	} else {
		turns = g_theGovernmentDB->Get(gov)->GetTurnsToNewReadiness();
		m_turnStarted = g_turn->GetRound();
	}

	READINESS_LEVEL oldLevel = m_readinessLevel;
    m_readinessLevel = level; 

    m_delta = (GetReadyHP(gov, m_readinessLevel) - GetReadyHP(gov, oldLevel))/turns;
	RecalcCost();

	g_slicEngine->RunTrigger(TRIGGER_LIST_READINESS,
							 ST_PLAYER, m_owner,
							 ST_END);
}

void MilitaryReadiness::RecalcCost()
{
	m_cost = 0.0;
	DynamicArray<Army> *all_armies = g_player[m_owner]->m_all_armies;
    int i, j; 
    int const   n = all_armies->Num(); 
    for (i=0; i<n; i++) { 
        int const m =all_armies->Access(i).Num(); 
        for (j=0; j<m; j++) { 
			m_cost += GetSupportCost(all_armies->Access(i)[j]);
        }
    }
}

struct UnitCost { 
    Unit u; 
    double cost; 
};


//EMOD
sint32 MilitaryReadiness::TotalUnitGoldSupport()
{
//based on RecalcCost, this coade (renamed from RecalcCostGold) gets all support gold hunger from units * govt coefficient * readiness 

	m_costGold = 0;
	DynamicArray<Army> *all_armies = g_player[m_owner]->m_all_armies;
	sint32 i, j; 
	sint32 const n = all_armies->Num(); 
	for (i=0; i<n; i++) 
    { 
    	sint32 const m = all_armies->Access(i).Num(); 
    	for (j=0; j<m; j++) 
        { 
		    m_costGold += GetSupportCostGold(all_armies->Access(i)[j]);
    	}
    }	
    return m_costGold;
}





void MilitaryReadiness::KillUnitsOverBudget(sint32 gov, DynamicArray<Army> &m_all_armies, sint32 mil_total)
// notgoing to do a gold one because it will cost economy not units, tougher for human and AI
{
    if (sint32(m_cost) <= mil_total) 
        return; 

//EMOD  AI can run deficit? but cant build production?
	if(g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetNoAIProductionDeficit()
	&& g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT)
        return; 



    sint32 n = m_all_armies.Num(); 

    if (n < 1) { 
        return; 
    } 

    sint32 i, j, m;
    sint32 n_units = 0;
    for (i=0; i<n; i++) { 
        n_units += m_all_armies[i].Num(); 
    }
    
    Assert(0 < n_units); 
    UnitCost *all_units = new UnitCost[n_units]; 
	UnitCost *prof_units = new UnitCost[n_units];

    n_units = 0;
	sint32 n_prof_units = 0;
    for (i=0; i<n; i++) { 
        m=m_all_armies[i].Num(); 
        for (j=0; j<m; j++) { 
            if (m_all_armies[i][j].GetNeedsNoSupport())
                continue;

			if(m_all_armies[i][j].Flag(k_UDF_IN_WORMHOLE))
				continue;

            if (m_all_armies[i][j].GetIsProfessional()) {
				prof_units[n_prof_units].u = m_all_armies[i][j];
				prof_units[n_prof_units].cost = GetSupportCost(m_all_armies[i][j]);
				n_prof_units++;
			}
			else{
				all_units[n_units].u = m_all_armies[i][j]; 
				all_units[n_units].cost = GetSupportCost(m_all_armies[i][j]);
				n_units++; 
			}
        }
    }


    double tmpc; 
    Unit tmpu; 
    
    for (i=0; i<(n_units-1); i++) { 
        for (j=i+1; j<n_units; j++) { 
            if (all_units[i].cost > all_units[j].cost) { 
                tmpu = all_units[i].u; 
                tmpc = all_units[i].cost; 

                all_units[i].u = all_units[j].u; 
                all_units[i].cost = all_units[j].cost; 

                all_units[j].u = tmpu; 
                all_units[j].cost = tmpc; 
            }
        } 
    } 


    m_ignore_unsupport = TRUE; 
    for (i = n_units-1; 0 <= i; i--) { 
        if (sint32(m_cost) <= mil_total) 
            break; 

        m_cost -= all_units[i].cost; 

        if (0 != m_owner) { 
            if (g_slicEngine->GetSegment("120NoSupport")->TestLastShown(m_owner, 1)) {
                SlicObject *so = new SlicObject("120NoSupport") ;
                so->AddRecipient(m_owner) ;
                g_slicEngine->Execute(so) ;
            }

            all_units[i].u.Kill(CAUSE_REMOVE_ARMY_NO_MAT_SUPPORT, -1);
        }
    }

	
	
	if(sint32(m_cost) > mil_total) {
		for (i=0; i<(n_prof_units-1); i++) { 
			for (j=i+1; j<n_prof_units; j++) { 
				if (prof_units[i].cost < prof_units[j].cost) { 
					tmpu = prof_units[i].u; 
					tmpc = prof_units[i].cost; 

					prof_units[i].u = prof_units[j].u; 
					prof_units[i].cost = prof_units[j].cost; 

					prof_units[j].u = tmpu; 
					prof_units[j].cost = tmpc; 
				}
			} 
		} 
		for (i = n_prof_units-1; 0 <= i; i--) { 
			if (sint32(m_cost) <= mil_total) 
				break; 
			
			m_cost -= prof_units[i].cost; 

            if (g_slicEngine->GetSegment("120NoSupport")->TestLastShown(m_owner, 1)) {
                SlicObject *so = new SlicObject("120NoSupport") ;
                so->AddRecipient(m_owner) ;
                g_slicEngine->Execute(so) ;
            }

			prof_units[i].u.Kill(CAUSE_REMOVE_ARMY_NO_MAT_SUPPORT, -1); 
		}
	}
    m_ignore_unsupport = FALSE; 

    delete [] all_units; 
	delete [] prof_units;
}

sint32 MilitaryReadiness::GetTurnsToNewReadiness()
{
	if(m_turnStarted < 0)
		return 0;
	sint32 turns = g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->
		GetTurnsToNewReadiness();
	sint32 finish = m_turnStarted + turns;

	if(finish <= g_turn->GetRound())
		return 0;

	return finish - g_turn->GetRound();
}

//----------------------------------------------------------------------------
//
// Name       : Serialize
//
// Description: Store or Load a block of data to or from a data stream
//
// Parameters : archive : the data stream
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : The original code used a block dump of 56 bytes.
//              (sizeof(MilitaryReadiness) == 56)
//              So we still have space for an additional 4 bytes of data.
//
//----------------------------------------------------------------------------
void MilitaryReadiness::Serialize(CivArchive &archive)
{
	if (archive.IsStoring())
    {
        archive << m_delta; 
        archive << m_hp_modifier; 
        archive << m_cost; 
        archive << m_percent_last_turn; 
        archive << static_cast<sint32>(m_readinessLevel);
        archive << static_cast<sint32>(m_ignore_unsupport); 
        archive << m_owner;
        archive << m_turnStarted;
	    archive << m_costGold; 
        // unused padding, to fill to an 8 byte boundary 
        archive << static_cast<sint32>(0);  
    }
    else
    {
        sint32  l_FourBytes;

        archive >> m_delta; 
        archive >> m_hp_modifier; 
        archive >> m_cost; 
        archive >> m_percent_last_turn;
        archive >> l_FourBytes;
        m_readinessLevel    = static_cast<READINESS_LEVEL>(l_FourBytes);
        archive >> l_FourBytes;
        m_ignore_unsupport  = static_cast<BOOL>(l_FourBytes); 
        archive >> m_owner;
        archive >> m_turnStarted;
	    archive >> m_costGold;  
        // unused padding, to fill to an 8 byte boundary 
        archive >> l_FourBytes;
    }
}

