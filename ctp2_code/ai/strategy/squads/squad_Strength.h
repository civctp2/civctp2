







#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __AGENT_STRENGTH_H__
#define __AGENT_STRENGTH_H__

class Squad_Strength;

#include "ctp2_inttypes.h"      // sintN
#include "MapPoint.h"           // MapPoint
#include "scheduler_types.h"

class Squad_Strength 
{ 
public:
    Squad_Strength(sint16 agent_count = 0)
    : m_agent_count      (agent_count),
	  m_attack_str       (0.0),
      m_defense_str      (0.0),
      m_ranged_str       (0.0),
	  m_value            (0),
	  m_transport        (0),
	  m_defenders        (0),
	  m_ranged           (0),
	  m_land_bombard_str (0.0),
	  m_water_bombard_str(0.0),
	  m_air_bombard_str  (0.0)
    { ; }; 

	// Use compiler generated copy constructor, destructor, and assignment operator.
	
	bool operator> (const Squad_Strength &squad_strength) const;

	
    Squad_Strength & operator+=(const Squad_Strength & add_me); 

	
    Squad_Strength & operator-=(const Squad_Strength & remove_me);

	
    sint32 Get_Agent_Count() const; 

	
    void Set_Agent_Count(const sint32 & count); 

	
    void Add_Agent_Strength(const Agent_ptr & agent);

	
	void Remove_Agent_Strength(const Agent_ptr & agent);

	
    void Set_Pos_Strength(const MapPoint & pos);

	
    double Get_Attack() const; 

	
    void Set_Attack(const double & attack); 

	
    double Get_Defense() const; 

	
    void Set_Defense(const double & defense); 

	
    double Get_Ranged() const; 

	
    void Set_Ranged(const double & defense); 

	
    double Get_Value() const; 

	
    void Set_Value(const double & value); 

	
    sint16 Get_Transport() const; 

	
    void Set_Transport(const sint16 & slots); 

	
    sint16 Get_Defenders() const; 

	
    void Set_Defenders(const sint16 & units); 

	
    sint16 Get_Ranged_Units() const; 

	
    void Set_Ranged_Units(const sint16 & units); 

	
	void Set_Force_Matching( const double attack_ratio,
							 const double defense_ratio,
							 const double ranged_ratio,
							 const double bombard_ratio,
							 const double value_ratio );
	void Set_To_The_Maximum(Squad_Strength);

protected:

    sint16 m_agent_count;  

	
    double m_attack_str;  

	
    double m_defense_str;  

	
    double m_ranged_str;
	
	
	double m_value;

	
	sint16 m_transport;

	
	sint16 m_defenders;
	
	
    sint16 m_ranged; 

	double m_land_bombard_str;
	double m_water_bombard_str;
	double m_air_bombard_str;
}; 

#endif // __AGENT_STRENGTH_H__
