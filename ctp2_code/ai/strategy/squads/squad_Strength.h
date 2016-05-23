//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// - Added get and set methods for the bombard members. (30-Apr-2008 Martin Gühmann)
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
//   - Added following methods: NothingNeeded, GetTotalMissingStrength, HasEnough, and Log_Debug_Infor
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __AGENT_STRENGTH_H__
#define __AGENT_STRENGTH_H__

class Squad_Strength;
class Army;
class MapPoint;

#include "ctp2_inttypes.h"      // sintN
#include "scheduler_types.h"

class Squad_Strength
{
public:
	Squad_Strength(sint8 agent_count = 0)
	: m_attack_str       (0.0),
	  m_defense_str      (0.0),
	  m_ranged_str       (0.0),
	  m_value            (0),
	  m_land_bombard_str (0.0),
	  m_water_bombard_str(0.0),
	  m_air_bombard_str  (0.0),
	  m_agent_count      (agent_count),
	  m_transport        (0),
	  m_defenders        (0),
	  m_ranged           (0)
	{ ; };

	Squad_Strength(const Army& army);
	Squad_Strength(const MapPoint& pos);

	// Use compiler generated copy constructor, destructor, and assignment operator.

	bool operator> (const Squad_Strength &squad_strength) const;

	Squad_Strength & operator+=(const Squad_Strength & add_me);
	Squad_Strength & operator-=(const Squad_Strength & remove_me);

	sint32 Get_Agent_Count()                    const { return m_agent_count; };
	void   Set_Agent_Count(const sint32 & count)      { m_agent_count = count; };

	void Add_Agent_Strength(const Agent_ptr & agent);
	void Remove_Agent_Strength(const Agent_ptr & agent);

	void Set_Pos_Strength       (const MapPoint & pos);
	void Set_Army_Strength      (const Army & army, bool noCargo = false);
	void Set_Cargo_Strength     (const Army & army);
	void Set_Enemy_Grid_Strength(const MapPoint & pos, const sint32 & playerId);

	float Get_Attack      () const { return m_attack_str;        };
	float Get_Defense     () const { return m_defense_str;       };
	float Get_Ranged      () const { return m_ranged_str;        };
	float Get_Bombard_Land() const { return m_land_bombard_str;  };
	float Get_Bombard_Sea () const { return m_water_bombard_str; };
	float Get_Bombard_Air () const { return m_air_bombard_str;   };
	float Get_Value       () const { return m_value;             };
	sint8 Get_Transport   () const { return m_transport;         };
	sint8 Get_Defenders   () const { return m_defenders;         };
	sint8 Get_Ranged_Units() const { return m_ranged;            };

	void Set_Attack       (const float & attack)  { m_attack_str        = attack;  };
	void Set_Defense      (const float & defense) { m_defense_str       = defense; };
	void Set_Ranged       (const float & ranged)  { m_ranged_str        = ranged;  };
	void Set_Bombard_Land (const float & land)    { m_land_bombard_str  = land;    };
	void Set_Bombard_Sea  (const float & sea)     { m_water_bombard_str = sea;     };
	void Set_Bombard_Air  (const float & air)     { m_air_bombard_str   = air;     };
	void Set_Value        (const float & value)   { m_value             = value;   };
	void Set_Transport    (const sint8 & slots)   { m_transport         = slots;    Assert(slots >= 0); };
	void Set_Defenders    (const sint8 & units)   { m_defenders         = units;   };
	void Set_Ranged_Units (const sint8 & units)   { m_ranged            = units;   };

	void Set_Force_Matching( const float attack_ratio,
							 const float defense_ratio,
							 const float ranged_ratio,
							 const float bombard_ratio,
							 const float value_ratio );
	void Set_To_The_Maximum(Squad_Strength);

	bool NothingNeeded() const;
	void Log_Debug_Info(const int & log, sint32 playerId, sint32 goalType, char * text) const;
	bool HasEnough(const Squad_Strength & otherStrength, bool ignoreBombard = false) const;
	float GetTotalMissing(const Squad_Strength & otherStrength) const;

protected:

	float m_attack_str;
	float m_defense_str;
	float m_ranged_str;
	float m_land_bombard_str;
	float m_water_bombard_str;
	float m_air_bombard_str;
	float m_value;

	sint8 m_agent_count;
	sint8 m_transport;
	sint8 m_defenders;
	sint8 m_ranged;
};

#endif // __AGENT_STRENGTH_H__
