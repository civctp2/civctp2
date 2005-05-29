#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __PLAYER_HAPPINESS_H__ 
#define __PLAYER_HAPPINESS_H__ 1

class Player; 
template <class T> class DynamicArray;

class Army; 

class CivArchive; 
class UnitDynamicArray;

class PlayerHappiness { 

    double m_workday_per_person; 


    double m_rations_per_person; 

    double m_wages_per_person; 

    double m_raw_overseas_defeat;
    double m_raw_home_defeat;

    double m_overseas_defeat;
    double m_home_defeat;
    double m_dist_overseas;
    double m_overseas; 

    sint32 m_unitless_rations; 
    sint32 m_unitless_wages; 
    sint32 m_unitless_workday; 
	double m_warDiscontent;
    sint32 m_pad; 
	friend class NetPlayerHappy;

public:

    PlayerHappiness(); 

    void CalcPeaceMovement(Player *p, DynamicArray<Army> &all_armies, 
        UnitDynamicArray &m_all_cities);

    void SetWorkdayLevel (sint32 w); 
    double GetWorkdayPerPerson (); 
    sint32 GetUnitlessWorkday ();

    void SetWagesLevel (sint32 w); 
    double GetWagesPerPerson (); 
    sint32 GetUnitlessWages ();

    void SetRationsLevel (sint32 w); 
    double GetRationsPerPerson () const; 
    sint32 GetUnitlessRations ();

    void GetPeaceMovement(double &overseas_defeat, 
        double &home_defeat, double &overseas) { 
        overseas_defeat = m_overseas_defeat; 
        home_defeat = m_home_defeat; 
        overseas = m_overseas; 
    }

    double GetWarDiscontent() { return m_warDiscontent; }

    void DisplayWWR();

    void Serialize (CivArchive &archive); 
}; 

#endif
