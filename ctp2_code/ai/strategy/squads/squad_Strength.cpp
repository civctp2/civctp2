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
#include "MapPoint.h"           // MapPoint

Squad_Strength::Squad_Strength(const Army& army)
{
	Set_Army_Strength(army);
}

Squad_Strength::Squad_Strength(const MapPoint& pos)
{
	Set_Pos_Strength(pos);
}

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
	float const attack_cpr  = (m_attack_str  - squad_strength.m_attack_str );
	// Defense difference
	float const defense_cpr = (m_defense_str - squad_strength.m_defense_str);
	// ranged difference
	float const ranged_cpr  = (m_ranged_str  - squad_strength.m_ranged_str );
	// value difference
	float const value_cpr   = (m_value       - squad_strength.m_value      );

	float const battle_cpr  = attack_cpr + defense_cpr + ranged_cpr + value_cpr;
	if (battle_cpr > 0.0f)
	{
		return true;
	}
	else if (battle_cpr < 0.0f)
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

void Squad_Strength::Set_Army_Strength(const Army & army, bool noCargo)
{
	Assert(army.IsValid());

	if(!army.IsValid())
	{
		*this           = Squad_Strength(1);
		return;
	}

	army->ComputeStrength(m_attack_str,
	                      m_defense_str,
	                      m_ranged_str,
	                      m_defenders,
	                      m_ranged,
	                      m_land_bombard_str,
	                      m_water_bombard_str,
	                      m_air_bombard_str,
	                      m_value,
	                      true,
	                      noCargo);

	m_agent_count       = static_cast<sint8>(army.Num());

	m_transport = 0;
	for(sint8 i = m_agent_count; i > 0; --i)
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

void Squad_Strength::Set_Cargo_Strength(const Army & army)
{
	Assert(army.IsValid());

	if(!army.IsValid())
	{
		*this           = Squad_Strength(1);
		return;
	}
	else if(!army->HasCargo())
	{
		*this           = Squad_Strength(0);
		return;
	}

	army->ComputeCargoStrength(m_attack_str,
	                           m_defense_str,
	                           m_ranged_str,
	                           m_defenders,
	                           m_ranged,
	                           m_land_bombard_str,
	                           m_water_bombard_str,
	                           m_air_bombard_str,
	                           m_value,
	                           true);

	m_agent_count       = static_cast<sint8>(army->GetCargoNum());
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
	for(sint8 i = m_agent_count; i > 0; --i)
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

void Squad_Strength::Set_Force_Matching( const float attack_ratio,
                                         const float defense_ratio,
                                         const float ranged_ratio,
                                         const float bombard_ratio,
                                         const float value_ratio )
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
	return m_air_bombard_str   >= 0.0f
	    && m_land_bombard_str  >= 0.0f
	    && m_water_bombard_str >= 0.0f
	    && m_attack_str        >= 0.0f
	    && m_defense_str       >= 0.0f
	    && m_ranged_str        >= 0.0f
	    && m_value             >= 0.0f
	    && m_agent_count       >= 0
	    && m_transport         >= 0
	    && m_defenders         >= 0
	    && m_ranged            >= 0;
}

float Squad_Strength::GetTotalMissing(const Squad_Strength & otherStrength) const
{
	float attack        = m_attack_str        - otherStrength.m_attack_str;
	float defense       = m_defense_str       - otherStrength.m_defense_str;
	float ranged        = m_ranged_str        - otherStrength.m_ranged_str;
	float value         = m_value             - otherStrength.m_value;
	float land_bombard  = m_land_bombard_str  - otherStrength.m_land_bombard_str;
	float water_bombard = m_water_bombard_str - otherStrength.m_water_bombard_str;
	float air_bombard   = m_air_bombard_str   - otherStrength.m_air_bombard_str;

	sint8 count         = m_agent_count       - otherStrength.m_agent_count;

	return std::max(0.0f, attack)
	     + std::max(0.0f, defense)
	     + std::max(0.0f, ranged)
	     + std::max(0.0f, value)
	     + std::max(0.0f, land_bombard)
	     + std::max(0.0f, water_bombard)
	     + std::max(0.0f, air_bombard)
	     + std::max(0.0f, static_cast<float>(count));
}

bool Squad_Strength::HasEnough(const Squad_Strength & otherStrength, bool ignoreBombard) const
{
	return (m_air_bombard_str   >= otherStrength.m_air_bombard_str   || ignoreBombard)
	    && (m_land_bombard_str  >= otherStrength.m_land_bombard_str  || ignoreBombard)
	    && (m_water_bombard_str >= otherStrength.m_water_bombard_str || ignoreBombard)
	    && m_attack_str        >= otherStrength.m_attack_str
	    && m_defense_str       >= otherStrength.m_defense_str
	    && m_ranged_str        >= otherStrength.m_ranged_str
	    && m_value             >= otherStrength.m_value
	    && m_agent_count       >= otherStrength.m_agent_count;
}

void Squad_Strength::Log_Debug_Info(const int & log, sint32 playerId, sint32 goalType, char * text) const
{
	AI_DPRINTF
	          (
	           k_DBG_SCHEDULER_ALL, playerId, goalType,
	                                                -1, ("\t\t%s"
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
