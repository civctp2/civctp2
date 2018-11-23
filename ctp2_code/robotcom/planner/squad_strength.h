#pragma once

#ifndef __AGENT_STRENGTH_H__
#define __AGENT_STRENGTH_H__

class CivArchive;
class Squad_Strength {

    sint16 m_army_count;
    sint16 m_unit_count;
    float m_attack_str;
    float m_defense_str;
    sint16 m_transport_capacity;
    sint16 m_ranged_unit_count;
    float m_ranged_str;




public:

    Squad_Strength();
    ~Squad_Strength();

	void Init();

    void Serialize(CivArchive &archive);

    sint32 GetArmyCount() const;
    void AddArmies(const sint32 num);

    sint32 GetUnitCount() const;
    void AddUnits(const sint32 num);
    void DelUnits(const sint32 num);
	void SetUnitCount(sint32 units);

    float GetAttack() const;
    void AddNetAttack(const float attack);
    void AddAttack(const float attack, const float fire_power, const float hp);
	void SubtractAttack(const float attack, const float fire_power, const float hp);
    void SetAttack(const float attack);

    float GetDefense() const;
    void AddNetDefense(const float defense);
    void AddDefense(const float defense, const float fire_power, const float hp);
	void SubtractDefense(const float defense, const float fire_power, const float hp);
    void SetDefense(const float defense);

    sint32 GetTransport() const;
    void AddTransports(const sint32 number_units_can_be_carried);
    void SetTransports(const sint32 number_units_can_be_carried);

    void AddRangeUnitCount(const sint32 n);
    void SetRangeUnitCount(const sint32 n);
    void AddRangeStrength(const float str, const float fire_power, const float hp);
    void SetRangeStrength(const float str);
	float GetRangedStrength() const;
	float GetRangedUnitCount() const;

	sint32 EffectiveRangedStrength() const;
    BOOL IsEnough(const Squad_Strength *test_me, sint32 test_dist) const;
    BOOL IsLessThenEqZero() const;

    void Add(const Squad_Strength *add_me);
    void Subtract(const Squad_Strength *add_me);
};

class AiMain;
struct MapPointData;
extern  BOOL ComputeEnemyStrengthAt(AiMain *ai, MapPointData &target_pos,
        float &attack, sint32 &defend_unit_count, float &defense,
        sint32 &ranged_unit_count, float &range);

#endif __AGENT_STRENGTH_H__
