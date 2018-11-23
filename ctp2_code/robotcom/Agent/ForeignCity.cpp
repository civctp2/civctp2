#include "c3.h"
#include "globals.h"

#include "bset.h"

#include "ic3GameState.h"
#include "ic3player.h"
#include "ic3world.h"
#include "ic3UnitDB.h"
#include "aimain.h"
#include "scienceagent.h"

#include "civarchive.h"
#include "MapPoint.h"

#include "ArmyGoal.h"
#include "GlEnslave.h"

#include "ForeignCity.h"
#include "squad_strength.h"

ForeignCity::ForeignCity() : Agent()

{
    Init();
}

ForeignCity::ForeignCity(AiMain *ai, BSetID &id, PLAYER_INDEX pi)
{
    Init();

	m_ai = ai;

    m_player_index = pi;
    m_has_enslave_goal = FALSE;

	Set_Seige_Status(false);
    m_pos = new MapPointData();
    m_id = id;
}

ForeignCity::~ForeignCity()
{







	if (m_ai)
		Remove_Enslave_Goal(m_ai);




    NDELETE (m_pos);
}

ForeignCity::ForeignCity(IC3CivArchive *archive)
{
    Init();

    m_pos = new MapPointData;
    Serialize(archive);
}

void ForeignCity::Init()
{
    m_agent_type = AGENT_TYPE_FOREIGN_CITY;
    m_pos = NULL;
    m_player_index = -1;
	seige_goal_set = FALSE;
    m_has_enslave_goal = FALSE;
    m_ai = NULL;

    m_been_nano_attacked = FALSE;

	enslave_me_goal_ID = UNASSIGNED_ID;
	m_convertMeGoalID = UNASSIGNED_ID;
	m_harassMeGoalID = UNASSIGNED_ID;
}

void ForeignCity::Serialize(IC3CivArchive *archive)
{
    CivArchive *a = (CivArchive *) archive;

    Agent::Serialize(*a);

    if (a->IsStoring()) {

        *a << m_pos->x;
        *a << m_pos->y;
        *a << m_pos->z;

        *a << m_player_index;
        a->PutSINT32(seige_goal_set);

        a->PutSINT32(m_been_nano_attacked);

        a->PutSINT32(m_has_enslave_goal);
	    *a << enslave_me_goal_ID;
		*a << m_convertMeGoalID;
		*a << m_harassMeGoalID;
    } else {
        *a >> m_pos->x;
        *a >> m_pos->y;
        *a >> m_pos->z;

        *a >> m_player_index;
        seige_goal_set = BOOL (a->GetSINT32());

        m_been_nano_attacked = BOOL(a->GetSINT32());

        m_has_enslave_goal = a->GetSINT32();
	    *a >> enslave_me_goal_ID;
		*a >> m_convertMeGoalID;
		*a >> m_harassMeGoalID;
    }

}

void ForeignCity::Hook(AiMain *ai)
{
    m_ai = ai;
}

void ForeignCity::SetPos (const MapPointData &p)

{
    *m_pos = p;
}

void ForeignCity::GetPos(MapPointData &my_pos)
{
    my_pos = *m_pos;
}

void ForeignCity::GetXYPos(AiMain *ai, PLAYER_INDEX owner, MapPointData &my_pos)
{
    BOOL is_unknown_id;

    ai->m_player->GetCityXYPos(owner, m_id.GetVal(), &is_unknown_id, &my_pos);

    Assert(is_unknown_id == FALSE);
}

double ForeignCity::GetAttackStrength() const
{

	Assert(FALSE);
    return 220.0;
}

void ForeignCity::GetDefenseStrength
(
	AiMain *ai,
	float & attack,
	float & defend
) const
{

	sint32 types_in_army[k_MAX_ARMY_SIZE];
	sint32 i;
	sint32 num_in_army;
    float total_strength = 0;
	float type_hitpoints;
	float attack_value;
	float firepower;
	float my_defense;
	Squad_Strength my_strength;


	ai->m_world->Get_Army_Types(m_pos, types_in_army, num_in_army);

	for (i = 0; i < num_in_army; i++)
	{

        attack_value = float(ai->m_unitDB->GetAttack(types_in_army[i]));
        type_hitpoints = float(ai->m_unitDB->GetHP(types_in_army[i]));
		firepower = float(ai->m_unitDB->GetFirepower(types_in_army[i]));
		my_defense = float(ai->m_unitDB->GetDefense(types_in_army[i]));

		my_strength.AddAttack(attack_value, firepower, type_hitpoints);
		my_strength.AddDefense(my_defense, firepower, type_hitpoints);

	}

    attack = my_strength.GetAttack();
    defend = my_strength.GetDefense();
}

sint32 ForeignCity::GetNumDefenders(AiMain *ai)
{
  	sint32 types_in_army[k_MAX_ARMY_SIZE];
	sint32 num_in_army;

	ai->m_world->Get_Army_Types(m_pos, types_in_army, num_in_army);

    return num_in_army;
}

double ForeignCity::GetTargetValue() const
{
    return 4000000.0;
}

double ForeignCity::ProjectedHappiness(AiMain *ai) const
{
	return 75.0;
}

double ForeignCity::ProjectedProduction(AiMain *ai) const
{
	return 50.0;
}

double ForeignCity::ProjectedFood(AiMain *ai) const
{
	return 50.0;
}









void ForeignCity::Set_Seige_Status(BOOL new_seige_status)
{

	seige_goal_set = new_seige_status;
}














BOOL ForeignCity::Get_Seige_Status()
{
	return seige_goal_set;
}






















































































void ForeignCity::Remove_Enslave_Goal(AiMain *ai)
{
	GoalEnslave *goal_obj;


	if (m_has_enslave_goal == TRUE)
	{
		goal_obj = (GoalEnslave *)
			ai->m_planner->Return_Goal_By_ID(enslave_me_goal_ID);

		Assert(goal_obj != NULL);
		if (goal_obj != NULL)
		{

            goal_obj->Set_Totally_Complete(TRUE);

			if (goal_obj->attacking_squad != NULL)
			{

				goal_obj->attacking_squad->Detach_Goal();

			}

			ai->m_planner->the_scheduler->Remove_Goal_And_Add_To_Delete_List(goal_obj);

		}

		RemoveEnslaveGoal();

	}
}





























































































































































double ForeignCity::PretestEnslave()
{
    if (m_has_enslave_goal) {
        return BAD_UTILITY;
    } else {
        return 1.0;
    }
}

void ForeignCity::SetHasEnslaveGoal(AiMain *ai, const BOOL b)
{

	m_ai = ai;

    m_has_enslave_goal = b;
}

void ForeignCity::SetEnslaveGoalID(AiMain *ai, sint32 goal_ID)
{

	m_ai = ai;

    enslave_me_goal_ID = goal_ID;
}

void ForeignCity::RemoveEnslaveGoal()
{
    m_has_enslave_goal = FALSE;
}

double ForeignCity::EstimateUtility(AiMain *ai, ArmyGoal *goal_obj)
{
    INSANE(FOREIGN_CITY_NO_UTILITY_FUNCTION);
    return BAD_UTILITY;
}

double ForeignCity::Estimate_Attack_Strength(AiMain *ai)
{
	sint32 attack;
	sint32 hp;
	sint32 fp;

	if (ai->m_science_agent->GetBestLandAttack(attack) &&
		ai->m_science_agent->GetBestHP(hp) &&
		ai->m_science_agent->GetBestFP(fp))
		return attack * hp * fp;
	else
		return 1.0;
}

sint32 ForeignCity::GetGoodCount(AiMain *ai, sint32 resource)
{
	return ai->m_gs->GetGoodCount(m_player_index, m_id.GetVal(), resource);
}

void ForeignCity::GetCityNumTradeRoutes(AiMain *ai, sint32 &in_num, sint32 &in_max,
    sint32 &out_num, sint32 &out_max)
{
    BOOL is_unknown_id;

    ai->m_player->GetCityNumTradeRoutes (m_player_index, m_id.GetVal(), &is_unknown_id,
        &in_num, &in_max, &out_num, &out_max);
}

sint32 ForeignCity::GetPopulation(AiMain *ai)
{
    BOOL is_unknown_id;

    sint32 r = ai->m_player->GetCityPopulation(m_player_index, m_id.GetVal(), &is_unknown_id);

    Assert(FALSE == is_unknown_id);

    return r;

}

sint32 ForeignCity::GetProduction(AiMain *ai)
{
    BOOL is_unknown_id;

    sint32 r = ai->m_player->GetCityProduction(m_player_index, m_id.GetVal(), &is_unknown_id);

    Assert(FALSE == is_unknown_id);
    return r;
}

sint32 ForeignCity::GetFood(AiMain *ai)
{
    BOOL is_unknown_id;

    sint32 r = ai->m_player->GetCityFood(m_player_index, m_id.GetVal(), &is_unknown_id);

    Assert(FALSE == is_unknown_id);

    return r;
}

sint32 ForeignCity::GetGold(AiMain *ai)
{
    BOOL is_unknown_id;
    sint32 r = ai->m_player->GetCityGold(m_player_index, m_id.GetVal(), &is_unknown_id);

    Assert(FALSE == is_unknown_id);

    return r;
}
