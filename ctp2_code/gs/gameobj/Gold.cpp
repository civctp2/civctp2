//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Gold
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Do not trigger a warning when rushbuying causes a negative cashflow.
// - Merged the constructors.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "gold.h"
#include "civarchive.h"
#include "network.h"

#include "ConstDB.h"

#include "gstypes.h"


Gold::Gold(PLAYER_INDEX const owner)
:	m_level(0),
	m_income_this_turn(0),
	m_gross_income(0),
	m_lost_to_cleric(0), 
    m_lost_to_crime(0), 
    m_lost_to_rushbuy(0),
    m_wages_this_turn(0), 
    m_maintenance_this_turn(0), 
	m_science_this_turn(0),
    m_level_last_turn(0), 
    m_delta_last_turn(0), 
	m_consider_for_science(0.0), 
	m_owner(owner)
{
}



Gold & Gold::operator= (const Gold &copyMe)
{
	m_level = copyMe.GetLevel() ;
	
    m_income_this_turn = copyMe.m_income_this_turn; 
    m_gross_income = copyMe.m_gross_income; 
    m_lost_to_cleric = copyMe.m_lost_to_cleric; 
    m_wages_this_turn = copyMe.m_wages_this_turn; 
    m_maintenance_this_turn = copyMe.m_maintenance_this_turn; 
    m_science_this_turn = copyMe.m_science_this_turn;
    m_level_last_turn = copyMe.m_level_last_turn; 
    m_delta_last_turn = copyMe.m_delta_last_turn;
    m_lost_to_crime = copyMe.m_lost_to_crime; 
    m_lost_to_rushbuy = copyMe.m_lost_to_rushbuy; 

	return (*this) ;
}












void Gold::Serialize(CivArchive &archive)
	{

    CHECKSERIALIZE

	if (archive.IsStoring()) 
		{
		archive<<m_level ;
		archive<<m_owner;
        
        archive << m_income_this_turn; 
        archive << m_gross_income; 
        archive << m_lost_to_cleric; 
        archive << m_wages_this_turn; 
        archive << m_maintenance_this_turn; 
        archive << m_science_this_turn;
        archive << m_level_last_turn; 
        archive << m_delta_last_turn; 
        archive << m_lost_to_crime; 
        archive << m_lost_to_rushbuy; 
		} 
	else 
		{
		archive>>m_level ;
		archive>>m_owner;

        
        archive >> m_income_this_turn; 
        archive >> m_gross_income; 
        archive >> m_lost_to_cleric; 
        archive >> m_wages_this_turn; 
        archive >> m_maintenance_this_turn; 
        archive >> m_science_this_turn;
        archive >> m_level_last_turn; 
        archive >> m_delta_last_turn; 
        archive >> m_lost_to_crime; 
        archive >> m_lost_to_rushbuy; 
		}
	}













uint32 Gold_Gold_GetVersion(void)
	{
	return (k_GOLD_VERSION_MAJOR<<16 | k_GOLD_VERSION_MINOR) ;
	}

void Gold::AddGold(const sint32 delta)
{ 
	
	int newval = m_level + delta; 
	
	
	if ((0 < delta) && (newval < m_level)) { 
#ifdef _DEBUG
		m_level = 1000000; 
#else 
        m_level = 0;
#endif
	} if (newval < 0) { 
#ifdef _DEBUG
		m_level = 1000000; 
#else 
        m_level = 0;
#endif
	} else { 
		m_level = newval;
	}
	
	if (m_owner != PLAYER_INDEX_INVALID && g_network.IsHost()) {
		g_network.Block(m_owner);
		ENQUEUE() ;
		g_network.Unblock(m_owner);
	}
}

void Gold::SubGold(const sint32 delta) 
{

	if (delta <= m_level) { 
		m_level -= delta; 
	} else { 
		Assert(0); 
		m_level = 0; 
	} 
	if(m_owner != PLAYER_INDEX_INVALID && g_network.IsHost()) {
		g_network.Block(m_owner);
		ENQUEUE();
		g_network.Unblock(m_owner);
	}
}

BOOL Gold::GiveGold(const sint32 amount)
{
	Assert(amount>0) ;
	if ((m_level - amount) < 0)
		return (FALSE) ;
	
	m_level -= amount ;
	
	if (m_owner != PLAYER_INDEX_INVALID && g_network.IsHost()) {
		g_network.Block(m_owner);
		ENQUEUE() ;
		g_network.Unblock(m_owner);
	}
	
	return (TRUE) ;
}

void Gold::ClearIncome()
{
    m_income_this_turn = 0; 
} 

void Gold::AddIncome(const sint32 delta)

{
    Assert(0 <= delta); 
    m_income_this_turn += delta; 
    m_level += delta; 
}

void Gold::SubIncome(const sint32 delta)
{
    
    m_income_this_turn -= delta; 
    if (m_income_this_turn < 0) { 
        
        
        m_income_this_turn = 0; 
    } 

    
    m_level -= delta; 

    Assert(0 <= m_level); 

    if (m_level < 0) { 
        m_level = 0; 
    }
}


void Gold::GetGoldLevels(sint32 *gross_income, sint32 *lost_to_cleric,
   sint32 *lost_to_crime,  sint32 *maintenance, sint32 *wages, 
   sint32 *science, sint32 *old_savings, sint32 *current_savings)
{
   *gross_income = m_gross_income; 
   *lost_to_cleric = m_lost_to_cleric; 
   *lost_to_crime = m_lost_to_crime; 
   *maintenance = m_maintenance_this_turn; 
   *wages = m_wages_this_turn; 
   *science = m_science_this_turn; 
   *old_savings = m_level_last_turn; 
   *current_savings = m_level; 
}

void Gold::SetSavings()
{ 
    m_level_last_turn = m_level; 
} 

BOOL Gold::BankruptcyImminent() const
{
    sint32 delta = DeltaThisTurn();
    if (delta >= 0)
        return(FALSE);

    
    
    if (delta < (m_delta_last_turn * 3))
        return(FALSE);

    
    delta += m_wages_this_turn;

	// Not needed: m_lost_to_rushbuy is already included through DeltaThisTurn.

    double fudge = (double)(g_theConstDB->MaintenanceWarningFudgeFactor()) / 100.0;
    sint32 f_level = (sint32)((double)m_level * fudge);

    if (-delta >= f_level)
        return(TRUE);

    return(FALSE);
}

void Gold::AddLostToCleric(const sint32 g)
{
    Assert(0<= g); 
    m_lost_to_cleric += g;
}

void Gold::AddLostToCrime(const sint32 g)
{
    Assert(0 <= g); 
    m_lost_to_crime += g; 
} 

void Gold::AddLostToRushBuy(const sint32 g)
{
    Assert(0 <= g); 
    m_lost_to_rushbuy += g; 
} 

void Gold::SetGrossIncome()
{ 
    m_gross_income = m_income_this_turn + m_lost_to_cleric + m_lost_to_crime; 
} 

void Gold::AddMaintenance(const sint32 g)
{
    m_maintenance_this_turn += g; 
} 

void Gold::AddWages(const sint32 g)
{ 
   m_wages_this_turn += g; 
}

void Gold::SetScience(const sint32 g)
{
   m_science_this_turn = g; 
}

void Gold::ClearStats()
{
	m_delta_last_turn	= DeltaThisTurn();	// Compute before clearing the data
    m_income_this_turn = 0; 
    m_gross_income = 0; 
    m_lost_to_cleric = 0; 
    m_lost_to_crime = 0; 
    m_lost_to_rushbuy = 0;
    m_maintenance_this_turn = 0; 
    m_wages_this_turn = 0; 
    m_science_this_turn = 0; 
    m_level_last_turn = 0; 
}
