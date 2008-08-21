//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Squad strength object 
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
// - Changed the > operator (used by Goal::IsSatisfied method)
//   Original method only test attack or defense param.
//   New methode take the sum of all strength (attack, defense, ranged,...)
//   by Calvitix and Martin Gühmann.
// - Handled problem with invalid units.
// - Added get and set methods for the bombard members. (30-Apr-2008 Martin Gühmann)
// - Position strength is now calculated terrain independently. (13-Aug-2008 Martin Gühmann)
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
//   - Added following methods: NothingNeeded, GetTotalMissingStrength, HasEnough, and Log_Debug_Infor
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "squad_Strength.h"

#include "agent.h"
#include "cellunitlist.h"
#include "UnitRecord.h"
#include "World.h"
#include "ctpaidebug.h"
#include "mapanalysis.h"

class Agent;

bool Squad_Strength::operator> (const Squad_Strength &squad_strength) const
{
	// Transport squads should always be bigger:
	if (m_transport > squad_strength.m_transport)
	{
		return true;
	}
	else if (m_transport < squad_strength.m_transport)
	{
		return false;
	}

	// Equal transport strength: test the battle strength

	// Attack difference
	double const attack_cpr  = (m_attack_str  - squad_strength.m_attack_str );
	// Defense difference
	double const defense_cpr = (m_defense_str - squad_strength.m_defense_str);
	// ranged difference
	double const ranged_cpr  = (m_ranged_str  - squad_strength.m_ranged_str );
	// value difference
	double const value_cpr   = (m_value       - squad_strength.m_value      );

	double const battle_cpr  = attack_cpr + defense_cpr + ranged_cpr + value_cpr;
	if (battle_cpr > 0)
	{
		return true;
	}
	else if (battle_cpr < 0)
	{
		return false;
	}

	// Equal battle strength: test the agent count

	if (m_agent_count > 0 && squad_strength.m_agent_count > 0){

		//If only agent count is a criterion : (for special units for example)
		if(m_attack_str + m_defense_str + m_ranged_str + m_value  == 0
		&& squad_strength.m_attack_str + squad_strength.m_defense_str + squad_strength.m_ranged_str + squad_strength.m_value  == 0
		){
			return (m_agent_count > squad_strength.m_agent_count);
		}
	}
	return false;
}

Squad_Strength & Squad_Strength::operator+=(const Squad_Strength & add_me)
{
	m_agent_count       += add_me.m_agent_count;
	m_attack_str        += add_me.m_attack_str;
	m_defense_str       += add_me.m_defense_str;
	m_ranged_str        += add_me.m_ranged_str;
	m_value             += add_me.m_value;
	m_transport         += add_me.m_transport;
	m_defenders         += add_me.m_defenders;
	m_ranged            += add_me.m_ranged; 

	m_land_bombard_str  += add_me.m_land_bombard_str;
	m_water_bombard_str += add_me.m_water_bombard_str;
	m_air_bombard_str   += add_me.m_air_bombard_str;

	return *this;
}

Squad_Strength & Squad_Strength::operator-=(const Squad_Strength & remove_me)
{
	m_agent_count       -= remove_me.m_agent_count;
	m_attack_str        -= remove_me.m_attack_str;
	m_defense_str       -= remove_me.m_defense_str;
	m_ranged_str        -= remove_me.m_ranged_str;
	m_value             -= remove_me.m_value;
	m_transport         -= remove_me.m_transport;
	m_defenders         -= remove_me.m_defenders;
	m_ranged            -= remove_me.m_ranged; 
	m_land_bombard_str  -= remove_me.m_land_bombard_str;
	m_water_bombard_str -= remove_me.m_water_bombard_str;
	m_air_bombard_str   -= remove_me.m_air_bombard_str;

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
	Assert(m_transport >= 0);
}

void Squad_Strength::Remove_Agent_Strength(const Agent_ptr & agent)
{
	Assert(Get_Agent_Count() > 0);
	(*this) -= agent->Get_Squad_Strength();
	Assert(m_transport >= 0);
}

void Squad_Strength::Set_Pos_Strength(const MapPoint & pos)
{
	CellUnitList *army = g_theWorld->GetArmyPtr(pos);

	if (army == NULL)
	{
		*this           = Squad_Strength(1);
		return;
	}

	m_agent_count = army->Num();

	army->ComputeStrength(m_defense_str, // defense and attack reversed
						  m_attack_str,
						  m_ranged_str,
						  m_defenders,
						  m_ranged,
						  m_land_bombard_str,
						  m_water_bombard_str,
						  m_air_bombard_str,
						  m_value,
						  false
						  );

	m_value = 0.0;
	m_transport = 0;
	for(sint32 i = m_agent_count; i > 0; --i)
	{
		Unit const &	unit	= army->Get(i - 1);
		
		if(unit.IsValid())
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
}

void Squad_Strength::Set_Enemy_Grid_Strength(const MapPoint & pos, const sint32 & playerId)
{
	m_attack_str        = MapAnalysis::GetMapAnalysis().GetEnemyAttack     (playerId, pos);
	m_defense_str       = MapAnalysis::GetMapAnalysis().GetEnemyDefense    (playerId, pos);
	m_ranged_str        = MapAnalysis::GetMapAnalysis().GetEnemyRanged     (playerId, pos);
	m_land_bombard_str  = MapAnalysis::GetMapAnalysis().GetEnemyBombardLand(playerId, pos);
	m_water_bombard_str = MapAnalysis::GetMapAnalysis().GetEnemyBombardSea (playerId, pos);
	m_air_bombard_str   = MapAnalysis::GetMapAnalysis().GetEnemyBombardAir (playerId, pos);
	m_value             = MapAnalysis::GetMapAnalysis().GetEnemyValue      (playerId, pos);
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

double Squad_Strength::Get_Bombard_Land() const
{
	return m_land_bombard_str;
}

void Squad_Strength::Set_Bombard_Land(const double & land)
{
	m_land_bombard_str = land;
}

double Squad_Strength::Get_Bombard_Sea() const
{
	return m_water_bombard_str;
}

void Squad_Strength::Set_Bombard_Sea(const double & sea)
{
	m_water_bombard_str = sea;
}

double Squad_Strength::Get_Bombard_Air() const
{
	return m_air_bombard_str;
}

void Squad_Strength::Set_Bombard_Air(const double & air)
{
	m_air_bombard_str = air;
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
	Assert(slots >= 0);
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
	m_attack_str        *= attack_ratio;
	m_defense_str       *= defense_ratio;
	m_ranged_str        *= ranged_ratio;
	m_land_bombard_str  *= bombard_ratio;
	m_water_bombard_str *= bombard_ratio;
	m_air_bombard_str   *= bombard_ratio;
	m_value             *= value_ratio;
}

void Squad_Strength::Set_To_The_Maximum(Squad_Strength otherStrength)
{
	m_agent_count       = std::max(m_agent_count      , otherStrength.m_agent_count      );
	m_air_bombard_str   = std::max(m_air_bombard_str  , otherStrength.m_air_bombard_str  );
	m_attack_str        = std::max(m_attack_str       , otherStrength.m_attack_str       );
	m_defenders         = std::max(m_defenders        , otherStrength.m_defenders        );
	m_defense_str       = std::max(m_defense_str      , otherStrength.m_defense_str      );
	m_land_bombard_str  = std::max(m_land_bombard_str , otherStrength.m_land_bombard_str );
	m_ranged            = std::max(m_ranged           , otherStrength.m_ranged           );
	m_ranged_str        = std::max(m_ranged_str       , otherStrength.m_ranged_str       );
	m_transport         = std::max(m_transport        , otherStrength.m_transport        );
	m_value             = std::max(m_value            , otherStrength.m_value            );
	m_water_bombard_str = std::max(m_water_bombard_str, otherStrength.m_water_bombard_str);
}

bool Squad_Strength::NothingNeeded() const
{
	return m_air_bombard_str   >= 0
	    && m_land_bombard_str  >= 0
	    && m_water_bombard_str >= 0
	    && m_attack_str        >= 0
	    && m_defense_str       >= 0
	    && m_ranged_str        >= 0
	    && m_value             >= 0
	    && m_agent_count       >= 0
	    && m_transport         >= 0
	    && m_defenders         >= 0
	    && m_ranged            >= 0;
}

double Squad_Strength::GetTotalMissing(const Squad_Strength & otherStrength) const
{
	double attack        = m_attack_str        - otherStrength.m_attack_str;
	double defense       = m_defense_str       - otherStrength.m_defense_str;
	double ranged        = m_ranged_str        - otherStrength.m_ranged_str;
	double value         = m_value             - otherStrength.m_value;
	double land_bombard  = m_land_bombard_str  - otherStrength.m_land_bombard_str;
	double water_bombard = m_water_bombard_str - otherStrength.m_water_bombard_str;
	double air_bombard   = m_air_bombard_str   - otherStrength.m_air_bombard_str;

	sint16 count         = m_agent_count       - otherStrength.m_agent_count;

	return std::max(0.0, attack)
	     + std::max(0.0, defense)
	     + std::max(0.0, ranged)
	     + std::max(0.0, value)
	     + std::max(0.0, land_bombard)
	     + std::max(0.0, water_bombard)
	     + std::max(0.0, air_bombard)
	     + std::max(0.0, static_cast<double>(count));
}

bool Squad_Strength::HasEnough(const Squad_Strength & otherStrength) const
{
	return m_air_bombard_str   >= otherStrength.m_air_bombard_str
	    && m_land_bombard_str  >= otherStrength.m_land_bombard_str
	    && m_water_bombard_str >= otherStrength.m_water_bombard_str
	    && m_attack_str        >= otherStrength.m_attack_str
	    && m_defense_str       >= otherStrength.m_defense_str
	    && m_ranged_str        >= otherStrength.m_ranged_str
	    && m_value             >= otherStrength.m_value
	    && m_agent_count       >= otherStrength.m_agent_count;
}

void Squad_Strength::Log_Debug_Info(const int & log, sint32 playerId, char * text) const
{
	AI_DPRINTF
	          (
	           k_DBG_SCHEDULER_ALL, playerId, -1, -1, ("\t\t%s"
	                                                     "\tm_air_bombard_str   %f"
	                                                     "\tm_land_bombard_str  %f"
	                                                     "\tm_water_bombard_str %f"
	                                                     "\tm_attack_str        %f"
	                                                     "\tm_defense_str       %f"
	                                                     "\tm_ranged_str        %f"
	                                                     "\tm_value             %f"
	                                                     "\tm_agent_count       %d"
	                                                     "\tm_transport         %d"
	                                                     "\tm_defenders         %d"
	                                                     "\tm_ranged            %d\n"
	                                                   , text
	                                                   , m_air_bombard_str
	                                                   , m_land_bombard_str
	                                                   , m_water_bombard_str
	                                                   , m_attack_str
	                                                   , m_defense_str
	                                                   , m_ranged_str
	                                                   , m_value
	                                                   , m_agent_count
	                                                   , m_transport
	                                                   , m_defenders
	                                                   , m_ranged
	                                                  )
	          );
}
