
#include "c3.h"
#include "MapPoint.h"

#include "civarchive.h"
#include "FlatPtr.h"


#include "ic3UnitDB.h"
#include "ic3world.h"
#include "aimain.h"

#include "squad_strength.h"









Squad_Strength::Squad_Strength()
{
	Init(); 
}








void Squad_Strength::Init()
{
	memset (this, 0, sizeof(Squad_Strength)); 
}








Squad_Strength::~Squad_Strength()
{
	
}







void Squad_Strength::Serialize(CivArchive &archive)
{
	if (archive.IsStoring()) { 
        archive.Store((uint8*)this, sizeof(Squad_Strength)); 
	} else { 
        archive.Load((uint8*)this, sizeof(Squad_Strength)); 
	} 
}







sint32 Squad_Strength::GetArmyCount() const
{ 
    return m_army_count; 
} 







void Squad_Strength::AddArmies(const sint32 num)
{ 
    Assert(0 < num);
    m_army_count += sint16(num); 
} 







sint32 Squad_Strength::GetUnitCount() const
{
    return m_unit_count; 
}







void Squad_Strength::AddUnits(const sint32 num)
{
    Assert(0 < num); 
    m_unit_count += sint16(num); 
}








void Squad_Strength::DelUnits(const sint32 num)
{
    Assert(0 < num); 
    m_unit_count -= sint16(num); 
}








void Squad_Strength::SetUnitCount(sint32 units)
{
    m_unit_count = sint16(units); 
}








float Squad_Strength::GetAttack() const
{
    return m_attack_str; 
}

void Squad_Strength::SetAttack(const float attack)
{ 
    m_attack_str = attack; 
} 

void Squad_Strength::AddNetAttack(const float attack)
{
    Assert(0.0 <= attack); 

    m_attack_str += attack; 

    Assert(0.0 <= m_attack_str);
}







void Squad_Strength::AddAttack(
    const float attack, 
    const float fire_power, 
    const float hp 
    )
{

    Assert(0.0 <= attack); 
    Assert(0.0 <= fire_power); 
    

    m_attack_str += attack * fire_power * hp; 
	Assert(m_attack_str >= 0);
}

void Squad_Strength::SubtractAttack(
    const float attack, 
    const float fire_power, 
    const float hp 
    )
{

    Assert(0.0 <= attack); 
    Assert(0.0 <= fire_power); 
    Assert(0.0 < hp); 

    m_attack_str -= attack * fire_power * hp; 
	Assert(m_attack_str >= 0);
}








float Squad_Strength::GetDefense() const
{ 
    return m_defense_str; 
} 


void Squad_Strength::AddNetDefense(const float defense)
{
    Assert(0.0 <= defense); 

    m_defense_str += defense; 

    Assert(0.0 <= m_defense_str);
	Assert(m_defense_str >= 0);
}







void Squad_Strength::AddDefense(
    const float defense, 
    const float fire_power, 
    const float hp 
    )
{
    Assert(0.0 <= defense); 
    Assert(0.0 <= fire_power); 
    
    
    m_defense_str += defense * fire_power * hp; 
	Assert(m_defense_str >= 0);
}

void Squad_Strength::SubtractDefense(
    const float defense, 
    const float fire_power, 
    const float hp 
    )
{
    Assert(0.0 <= defense); 
    Assert(0.0 <= fire_power); 
    Assert(0.0 < hp);
    
    m_defense_str -= defense * fire_power * hp; 
	Assert(m_defense_str >= 0);
}


void Squad_Strength::SetDefense(const float defense) 
{ 
    m_defense_str = defense; 
	Assert(m_defense_str >= 0);
} 









sint32 Squad_Strength::GetTransport() const
{
    return m_transport_capacity; 
}







void Squad_Strength::AddTransports(const sint32 number_units_can_be_carried)
{
    
    m_transport_capacity += sint16 (number_units_can_be_carried); 
	Assert(m_transport_capacity >= 0);
}

void Squad_Strength::SetTransports(const sint32 number_units_can_be_carried)
{ 
    m_transport_capacity = sint16(number_units_can_be_carried); 
	Assert(m_transport_capacity >= 0);
}




sint32 Squad_Strength::EffectiveRangedStrength() const
{
    sint32 effective_units;
    sint32 range_str; 
    if (0 < m_ranged_unit_count) { 
        effective_units = int(min(m_unit_count * 0.5, m_ranged_unit_count));
		
        range_str = sint32(effective_units * m_ranged_str/m_ranged_unit_count);
    }else { 
        range_str = 0; 
    } 

	
	

	return range_str;
}








BOOL Squad_Strength::IsEnough(const Squad_Strength *test_me, sint32 test_dist) const
{

 



 
	sint32 test_ranged_str;
    sint32 my_ranged_str; 

	test_ranged_str = test_me->EffectiveRangedStrength();
	my_ranged_str = EffectiveRangedStrength();

    if ((test_me->m_attack_str + test_ranged_str) <
		(m_attack_str + my_ranged_str))
        return FALSE; 

    if (1 < test_dist) { 
		if ((test_me->m_defense_str + test_ranged_str) < (m_defense_str + my_ranged_str))
            return FALSE; 
    }

    if (test_me->m_transport_capacity < m_transport_capacity)
        return FALSE; 

    return TRUE; 
}







void Squad_Strength::Add(const Squad_Strength *add_me)
{
    m_army_count += add_me->m_army_count;
    Assert(0 <= m_army_count);
    m_unit_count += add_me->m_unit_count; 
    Assert(0 <= m_unit_count); 
    m_attack_str += add_me->m_attack_str; 
    Assert(0 <= m_attack_str); 
    m_defense_str += add_me->m_defense_str;
    Assert(0 <= m_defense_str); 
    m_transport_capacity += add_me->m_transport_capacity; 
    Assert(0 <= m_transport_capacity); 
    m_ranged_unit_count += add_me->m_ranged_unit_count; 
    m_ranged_str += add_me->m_ranged_str; 

}








void Squad_Strength::Subtract(const Squad_Strength *add_me)
{
    Assert(0 <= m_army_count);
    Assert(0 <= m_unit_count); 
    Assert(0 <= m_attack_str); 
    Assert(0 <= m_defense_str); 
    Assert(0 <= m_transport_capacity); 

    m_army_count -= add_me->m_army_count;
    m_unit_count -= add_me->m_unit_count; 
    m_attack_str -= add_me->m_attack_str; 
    m_defense_str -= add_me->m_defense_str;
    m_transport_capacity -= add_me->m_transport_capacity; 
    m_ranged_unit_count -= add_me->m_ranged_unit_count; 
    m_ranged_str -= add_me->m_ranged_str; 

    Assert(0 <= m_army_count);
    Assert(0 <= m_unit_count); 
    Assert(0 <= m_attack_str); 
    Assert(0 <= m_defense_str); 
    Assert(0 <= m_transport_capacity); 
}








BOOL Squad_Strength::IsLessThenEqZero() const
{
    if (0 < m_army_count)
        return FALSE; 

    if (0 < m_unit_count)
        return FALSE; 

    if (0 < m_attack_str)
        return FALSE; 

    if (0 < m_defense_str)
        return FALSE; 

    if (0 < m_transport_capacity)
        return FALSE; 

    return TRUE; 
}


void Squad_Strength::AddRangeUnitCount(const sint32 n)
{
    m_ranged_unit_count += sint16(n); 

}

void Squad_Strength::SetRangeUnitCount(const sint32 n)
{
     m_ranged_unit_count = sint16(n); 
}

void Squad_Strength::AddRangeStrength(const float str, 
									  const float fire_power, 
									  const float hp)
{
     m_ranged_str += str * fire_power * hp;
	Assert(m_ranged_str >= 0);
}

void Squad_Strength::SetRangeStrength(const float str)
{
    m_ranged_str = str; 
	Assert(m_ranged_str >= 0);
}

float Squad_Strength::GetRangedStrength() const
{
	return m_ranged_str;
}

float Squad_Strength::GetRangedUnitCount() const
{
	return m_ranged_unit_count;
}


BOOL ComputeEnemyStrengthAt(AiMain *ai, MapPointData &target_pos, 
    float &attack, sint32 &defend_unit_count, float &defense, 
    sint32 &ranged_unit_count, float &ranged)
{
    attack=0.0;						
	defense=0.0;					
    ranged=0.0; 
    defend_unit_count=0; 
    ranged_unit_count=0; 


    PLAYER_INDEX enemy_owner; 
    sint32 unit_type[k_MAX_ARMY_SIZE];
    sint32 unit_hp[k_MAX_ARMY_SIZE];
	float firepower;
    BOOL is_entrenched; 
    
    BOOL get_unit_data_ok = ai->m_world->GetUnitData(&target_pos, &enemy_owner, &defend_unit_count, 
        unit_type, unit_hp, &is_entrenched);

    if (!get_unit_data_ok) return FALSE; 

    if (ai->m_my_player_id == enemy_owner) return FALSE;  

  
    float r; 
	
    sint32 ranged_count=0; 
	

    sint32 unit_idx; 
    for (unit_idx=0; unit_idx<defend_unit_count; unit_idx++) { 
		firepower = float(ai->m_unitDB->GetFirepower(unit_type[unit_idx]));
        attack += ai->m_unitDB->GetAttack(unit_type[unit_idx]) 
			* unit_hp[unit_idx] 
			* firepower; 
        defense += ai->m_unitDB->GetDefense(unit_type[unit_idx]) 
			* unit_hp[unit_idx]
			* firepower;
        r = ai->m_unitDB->GetZBRange(unit_type[unit_idx])
			* firepower;
        if (0.0 < r) { 
            ranged +=  r; 
            ranged_unit_count++; 
        } 
    } 

    double terrain_bonus;
    double fort_bonus;
    double wall_bonus;

    ai->m_world->GetTerrainCombatBonus (&target_pos, &terrain_bonus, 
        &fort_bonus, &wall_bonus);

    
    float entrenchment_bonus; 
    if (is_entrenched) { 
        entrenchment_bonus =  float(ai->m_unitDB->GetEntrenchmentBonus()); 
    } else { 
        entrenchment_bonus = 0.0; 
    } 

    defense = float(defense + 
            defense * entrenchment_bonus + 
            defense * terrain_bonus + 
            defense * fort_bonus +
            + wall_bonus * defend_unit_count); 

	Assert(defense < 99999.0);
	Assert(defense >= 0);

	Assert(attack < 99999.0);
	Assert(attack >= 0);

	Assert(ranged < 99999.0);
	Assert(ranged >= 0);
   
    return TRUE; 
}


