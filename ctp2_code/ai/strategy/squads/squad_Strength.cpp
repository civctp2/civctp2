//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Squad strength object 
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
// _MSC_VER		
// - Compiler version (for the Microsoft C++ compiler only)
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks
//       between #else and #endif are modified Apolyton code. The blocks that
//       are active for _MSC_VER value 1200 are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Changed the > operator (used by Goal::IsSatisfied method)
//   Original method only test attack or defense param.
//   New methode take the sum of all strength (attack, defense, ranged,...)
//   by Calvitix.
// - Handled problem with invalid units.
// - Added a test in > operator based on number of agent (to permit 
//   neversatisfied goals) - Calvitix
// - Disabled Calvitix last change of the > operator as it slows down the
//   game on maps with more than one continent massively. Nevertheless this
//   operator needs to be reworked. - Feb. 21st 2005 Martin Gühmann
// 
//----------------------------------------------------------------------------

#include "c3.h"

#include "Agent.h"
#include "Squad_Strength.h"
#include "CellUnitList.h"
#include "World.h"
#include "UnitRecord.h"
#include "DebugAssert.h"

Squad_Strength::Squad_Strength()
{
	Init();
} 


Squad_Strength::Squad_Strength(const Squad_Strength &squad_strength)
{
	*this = squad_strength;
}


Squad_Strength::~Squad_Strength()
{
} 


Squad_Strength & Squad_Strength::operator= (const Squad_Strength &squad_strength)
{
	
    m_agent_count = squad_strength.m_agent_count;  

	
    m_attack_str = squad_strength.m_attack_str;

	
    m_defense_str = squad_strength.m_defense_str;

	m_ranged_str = squad_strength.m_ranged_str;

	
	m_value = squad_strength.m_value;

	
	m_transport = squad_strength.m_transport;

	m_defenders = squad_strength.m_defenders;

    m_ranged = squad_strength.m_ranged; 

	m_land_bombard_str = squad_strength.m_land_bombard_str;
	m_water_bombard_str = squad_strength.m_water_bombard_str;
	m_air_bombard_str = squad_strength.m_air_bombard_str;

    return *this;
}


bool Squad_Strength::operator> (const Squad_Strength &squad_strength) const
{
	
	bool greater = 
		(m_attack_str > squad_strength.m_attack_str) ||
		(m_defense_str > squad_strength.m_defense_str) ||
		(m_transport > squad_strength.m_transport);
	
	
	bool greater_defenders = (m_defenders  > squad_strength.m_defenders );
	bool greater_ranged = (m_ranged > squad_strength.m_ranged);
	bool greater_ranged_str = (m_ranged_str  > squad_strength.m_ranged_str );

	
	bool greater_value = (m_value > squad_strength.m_value);
	bool greater_agents = (m_agent_count > squad_strength.m_agent_count);

#if 0 && !defined (ACTIVISION_ORIGINAL) // Calvitix 
// Removed by Martin Gühmann
// Causes a severe slow down on maps with more than one continent.
// Needs therefore be reconsidered. That is also true for the
// original code, only the greater variable is used.
    greater = false;
	
    // Attack difference
    sint16 attack_cpr = (m_attack_str - squad_strength.m_attack_str);
    // Defense difference
    sint16 defense_cpr = (m_defense_str - squad_strength.m_defense_str);
    // ranged difference
    sint16 ranged_cpr = (m_ranged_str - squad_strength.m_ranged_str);
    // value difference
    sint16 value_cpr = (m_value - squad_strength.m_value);

    sint16 ranged_nb = (m_ranged - squad_strength.m_ranged);
    sint16 defenders_nb = (m_defenders - squad_strength.m_defenders);
    sint16 transport_nb = (m_transport - squad_strength.m_transport);

    // the addition of all differences has to be greater than 0
    // it is certainly better than only testing attack or defense
    // but can be improved (for exemple, by applying a ratio on the greater score
    if ((attack_cpr + defense_cpr + ranged_cpr + value_cpr) > 0)
        greater = true;

    // To be compatible with the original operator, test the
    // nb of transport too
    if (m_transport > 0 && transport_nb > 0)
        greater = true;

    // test the nb of agent too
    if (m_agent_count > 0 && squad_strength.m_agent_count > 0)
	{
        greater = greater || greater_agents;

		//when only agent count is as criterion : (for special units for example)
		if (m_attack_str + m_defense_str + m_ranged_str + m_value  == 0)
		{
			greater = greater_agents;
		}

	}


#endif
	return greater;
}


Squad_Strength & Squad_Strength::operator+=(const Squad_Strength & add_me)
{
	m_agent_count += add_me.m_agent_count;
	m_attack_str += add_me.m_attack_str;
	m_defense_str += add_me.m_defense_str;
	m_ranged_str += add_me.m_ranged_str;
	m_value += add_me.m_value;
	m_transport += add_me.m_transport;
	m_defenders += add_me.m_defenders;
    m_ranged += add_me.m_ranged; 

	m_land_bombard_str += add_me.m_land_bombard_str;
	m_water_bombard_str += add_me.m_water_bombard_str;
	m_air_bombard_str += add_me.m_air_bombard_str;

    return *this;
} 


Squad_Strength & Squad_Strength::operator-=(const Squad_Strength & remove_me)
{
	m_agent_count -= remove_me.m_agent_count;
	m_attack_str -= remove_me.m_attack_str;
	m_defense_str -= remove_me.m_defense_str;
	m_ranged_str -= remove_me.m_ranged_str;
	m_value -= remove_me.m_value;
	m_transport -= remove_me.m_transport;
	m_defenders -= remove_me.m_defenders;
    m_ranged -= remove_me.m_ranged; 
	m_land_bombard_str -= remove_me.m_land_bombard_str;
	m_water_bombard_str -= remove_me.m_water_bombard_str;
	m_air_bombard_str -= remove_me.m_air_bombard_str;

    return *this;
}


sint32 Squad_Strength::Get_Agent_Count() const
{
	return m_agent_count;
} 


void Squad_Strength::Set_Agent_Count(const sint32 & count)
{
	m_agent_count = count;
}


void Squad_Strength::Add_Agent_Strength(const Agent_ptr & agent)
{
	(*this) += agent->Get_Squad_Strength();
}


void Squad_Strength::Remove_Agent_Strength(const Agent_ptr & agent)
{
	(*this) -= agent->Get_Squad_Strength();
}


void Squad_Strength::Set_Pos_Strength(const MapPoint & pos)
{
	CellUnitList *army = g_theWorld->GetArmyPtr(pos);

	
	if (army == NULL)
	{
		Init();
		m_agent_count = 1;	// why not 0 ??? 
		return;
	}

	m_agent_count = army->Num();

	
	army->ComputeStrength(m_defense_str,	
						  m_attack_str,		
						  m_ranged_str,		
						  m_defenders,		
						  m_ranged,
						  m_land_bombard_str,
						  m_water_bombard_str,
						  m_air_bombard_str);		
   
	
	m_value = 0.0;
	m_transport = 0;
#if defined(ACTIVISION_ORIGINAL)	// crash on invalid unit
	for (int i = 0; i < army->Num(); i++)
	{
		
        m_value += army->Get(i).GetDBRec()->GetShieldCost();
		m_transport += ( army->Get(i).GetCargoCapacity() - army->Get(i).GetNumCarried() );
	} 
#else
	for (int i = m_agent_count; i > 0; --i)
	{
		Unit const &	unit	= army->Get(i - 1);
		
		if (unit.IsValid())
		{
			m_value		+= unit.GetDBRec()->GetShieldCost();
			m_transport	+= (unit.GetCargoCapacity() - unit.GetNumCarried());
		}
		else
		{
			// Remove the invalid unit
			army->DelIndex(i - 1);
			--m_agent_count;
		}
	}
#endif
}


double Squad_Strength::Get_Attack() const
{
	return m_attack_str;
} 


void Squad_Strength::Set_Attack(const double & attack)
{
	m_attack_str = attack;
} 


double Squad_Strength::Get_Defense() const
{
	return m_defense_str;
} 


void Squad_Strength::Set_Defense(const double & defense)
{
	m_defense_str = defense;
} 


double Squad_Strength::Get_Ranged() const
{
	return m_ranged_str;
}


void Squad_Strength::Set_Ranged(const double & ranged)
{
	m_ranged_str = ranged;
}


double Squad_Strength::Get_Value() const
{
	return m_value;
} 


void Squad_Strength::Set_Value(const double & value)
{
	m_value = value;
} 


sint16 Squad_Strength::Get_Transport() const
{
	return m_transport;
}
	

void Squad_Strength::Set_Transport(const sint16 & slots)
{
	m_transport = slots;
}


sint16 Squad_Strength::Get_Defenders() const
{
	return m_defenders;
}


void Squad_Strength::Set_Defenders(const sint16 & units)
{
	m_defenders = units;
}


sint16 Squad_Strength::Get_Ranged_Units() const
{
	return m_ranged;
}


void Squad_Strength::Set_Ranged_Units(const sint16 & units)
{
	m_ranged = units;
}


void Squad_Strength::Set_Force_Matching( const double attack_ratio,
										const double defense_ratio,
										const double ranged_ratio,
										const double bombard_ratio,
										const double value_ratio )
{
	m_attack_str *= attack_ratio;
	m_defense_str *= defense_ratio;
	m_ranged_str *= ranged_ratio;
	m_land_bombard_str *= bombard_ratio;
	m_water_bombard_str *= bombard_ratio;
	m_air_bombard_str *= bombard_ratio;
	m_value *= value_ratio;
}
