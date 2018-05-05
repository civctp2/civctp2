#include "ctp/c3.h"
#include "user-robotcom/shared/globals.h"
#include "user-robotcom/outcom/ic3player.h"
#include "user-robotcom/outcom/ic3Rand.h"
#include "user-robotcom/outcom/ic3world.h"
#include "user-robotcom/outcom/ic3UnitDB.h"
#include "gs/gameobj/AgreementTypes.h"

#include "robot/aibackdoor/dynarr.h"

#include "robotcom/planner/dr_debug.h"

#include "common.h"
#include "robotcom/planner/linked_list.h"
#include "semi_dynamic_array.h"
#include "robotcom/planner/sorted_array.h"
#include "robotcom/planner/matrix.h"
#include "robotcom/planner/ailog.h"

#include "robot/aibackdoor/bset.h"
#include "gs/world/MapPoint.h"

#include "user-robotcom/outcom/ic3GameState.h"
#include "aimain.h"
#include "robotcom/Top/aicell.h"
#include "robotcom/Top/AiMap.h"
#include "robotcom/Alloc/RouteUtilityList.h"
#include "robotcom/aimgr/WonderTbl.h"

#include "robotcom/Agent/ArmyAgent.h"
#include "robotcom/Agent/CityAgent.h"
#include "robotcom/Alloc/InstBidList.h"

#include "robotcom/Agent/ForeignAgent.h"
#include "robotcom/Agent/ForeignCity.h"

#include "Foreigner.h"


#include "robotcom/FrmState/fscompete.h"

#include "robotcom/goals/ArmyGoal.h"
#include "robotcom/goals/GoalDefense.h"
#include "robotcom/goals/GlExpl.h"
#include "robotcom/goals/GlAttck.h"
#include "robotcom/goals/GlSettl.h"
#include "robotcom/goals/GlRoute.h"
#include "robotcom/goals/glfreight.h"
#include "robotcom/goals/GlTrans.h"
#include "robotcom/goals/GlEnslave.h"
#include "robotcom/goals/Goal_Perimeter.h"
#include "robotcom/goals/Goal_Patrol.h"
#include "robotcom/goals/GoalMapTarget.h"
#include "robotcom/goals/GoalCityTarget.h"
#include "robotcom/goals/Goal_Attack_Region.h"
#include "robotcom/goals/GoalRetreat.h"
#include "robotcom/goals/GoalSally.h"

#include "robotcom/fuzzy/FzOut.h"
#include "robotcom/goals/GoalInst.h"

#include "Plan.h"






#include "robotcom/planner/unitflow.h"

#include "robotcom/planner/aip.h"

#include "robotcom/planner/squad.h"

#include "robotcom/planner/goal.h"

#include "Plan.h"

#include "Scheduler.h"

#include "robotcom/planner/planner.h"

#include "robotcom/planner/grassfire.h"
#include "robotcom/Top/strategic_map.h"

#include "robotcom/backdoor/Wallclock.h"
extern Wall_Clock *g_wall_clock;


extern double fz_settle_goal_threshold;
extern double fz_explore_goal_threshold;
extern double fz_time;
extern double fz_freight_reserve;




FSCompeteForAllMovement::FSCompeteForAllMovement()

{
    return;
}
void FSCompeteForAllMovement::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    return;
}











BOOL FSCompeteForAllMovement::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{

	return TRUE;

    if (ai->m_i_am_dead) {
       return FALSE;
    }

	if (ai->m_army_set->GetNum() <= 0 &&
		ai->m_city_set->GetNum() <= 0)
		return true;

CHECKCLOCK

	#ifdef DBGAILOG

	if (ai->AI_VERBOSITY >= LOG_HEADERS)
	{
		AILOG(( wstr,  "\n\n\n****\n****\n****\n**** NEW TURN.  TIME: %f\n****\n****\n****\n\n\n",
					(fz_time - 1.0)));

		AILOG(( wstr, "**** Current AIP: %s\n****\n\n",
			ai->m_planner->the_AIP.name));

	}
	#endif DBGAILOG

	#ifdef VALIDATE_ON
	{   BOOL r = ai->m_planner->Validate(ai);
		Assert(r);
	}
	#endif _DEBUG


	ai->UnloadRefueledCargo();

	ai->m_planner->the_scheduler->Update_Squads();
CHECKCLOCK


	ai->m_planner->the_scheduler->Clean_Up();
CHECKCLOCK

	ai->m_planner->the_scheduler->Planning_Status_Reset();
CHECKCLOCK


	ai->m_planner->Update_Map(ai);
CHECKCLOCK

	ai->m_planner->Update_Goals();
CHECKCLOCK

	FindAllGoalPos(ai, fout);
CHECKCLOCK

	ai->m_planner->Update_Squads(ai);
CHECKCLOCK

	ai->m_planner->the_AIP.Update();
CHECKCLOCK

	ai->m_planner->the_scheduler->Allocate_Troops_And_Execute_Tasks(ai);
CHECKCLOCK








	ai->BombardAdjacentEnemies();

	ai->DisbandObsoleteArmies();

	#ifdef DBGAILOG
		if (ai->AI_VERBOSITY >= LOG_HEADERS)
			AILOG(( wstr,  "\n\n\n****\n****\n****\n**** END OF TURN: %f\n****\n****\n****\n\n\n",
				(fz_time - 1.0)));
	#endif DBGAILOG

	#ifdef VALIDATE_ON
	{
		BOOL r = ai->m_planner->Validate(ai);
		Assert(r);
	}
	#endif _DEBUG

CHECKCLOCK
    return TRUE;
}














void FSCompeteForAllMovement::FindAllGoalPos(AiMain *ai, FILE *fout)
{




#ifdef DBGAILOG
if (ai->AI_VERBOSITY >= LOG_HEADERS){
	AILOG(( wstr,  "FSCompeteForAllMovement: Finding Goals\n"));
}
#endif DBGAILOG


    InsertCityBuildGoal(ai);






    if (PretestFreight(ai)) {
        InsertFreightGoal(ai);
    }







	if (!ai->m_wonder_table->GetEmancipation())

        InsertEnslaveGoals(ai);

	Insert_Attack_Goals(ai);






	Insert_Seige_Goals(ai);








	Insert_Sally_Goals(ai);






	if (ai->my_player_index != 0)
		{


			Insert_Perimeter_And_Patrol_Goals(ai);

			Insert_Explore_Goals(ai);


		}

	GoalMapTarget::SetupMapTargetGoals(ai);









    if (PretestSettle(ai)) {

        sint32 settle_num =  ai->m_planner->the_AIP.max_eval_goals_of_type[GOAL_TYPE_SETTLE];
        if (settle_num < 0) {
            settle_num = 200;
        }
        ai->m_map->InsertSettleGoals(ai, settle_num);





    }

}





void FSCompeteForAllMovement::InsertCityBuildGoal(AiMain *ai)
{
    CityAgent *ca;
    BSetID id;

    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) {
        if (ca->GetQueueLen() < 1) {
            ca->InsertBuildingGoals(ai);
        }
    }
}





BOOL FSCompeteForAllMovement::PretestFreight(AiMain *ai)
{
    CityAgent *ca=NULL;
    BSetID id;

    if (ai->m_route_utility_list->GetNumGoals() < 1)
        return FALSE;

    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) {
        if (ca->GetQueueLen() < 1) {
            return TRUE;
        }
    }

    return FALSE;
}


void FSCompeteForAllMovement::InsertFreightGoal(AiMain *ai)
{
    sint32 i, n;
    double cost=0.0;
    double utility=0.0;
    double u;
    GoalFreight *goal_obj=NULL;

    n = ai->m_route_utility_list->GetNumGoals();
    for (i=0; i<n; i++) {
        if (ai->m_route_utility_list->GetGoal(i)->IsSatisfied() == FALSE) {
             u = ai->m_route_utility_list->GetGoal(i)->GetUtility();
             if (0.0 < u) {
                cost += 1.0;
                utility += u;
             }
        }
    }

    cost += fz_freight_reserve;

    cost -= ai->m_player->GetUnusedFreight();

    if (cost <= 0) {
        return;
    }

    BSetID id;
    CityAgent *ca=NULL;
    sint32 freight_size;

    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) {
        if (ca->IsConstructingFreight(ai, freight_size)) {
            cost -= freight_size;
        }
    }

    if (0 < cost) {
        goal_obj = new GoalFreight(sint32(cost), utility);

		ai->m_planner->Add_Goal(goal_obj);
    }
}

BOOL FSCompeteForAllMovement::PretestSettle(AiMain *ai) const

{
    BSetID id;
    CityAgent *ca;

    if (0 == ai->m_my_player_id) {
        return FALSE;
    }

    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) {
        if (ca->GetQueueLen() < 1) {
            return TRUE;
        }
    }

    ArmyAgent *agent=NULL;

    for (agent = ai->m_army_set->First(id); ai->m_army_set->Last(); agent = ai->m_army_set->Next(id)) {
           switch (agent->GetState()) {
            case AGENT_STATE_BUILDING:
            case AGENT_STATE_UNBUILT:
            case AGENT_STATE_MARKED_DEAD:
            case AGENT_STATE_MARKED_SETTLE:
                continue;
            default:
                if (agent->GetArmyContainsSettler()) {
                    return TRUE;
                }
             }

    }

    return FALSE;
}

BOOL FSCompeteForAllMovement::PretestExplore(AiMain *ai) const
{
    BSetID id;
    CityAgent *ca;

    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) {
        if (ca->GetQueueLen() < 1) {
            return TRUE;
        }
    }

    ArmyAgent *agent;

    for (agent = ai->m_army_set->First(id); ai->m_army_set->Last(); agent = ai->m_army_set->Next(id)) {
        if (agent->GetState() == AGENT_STATE_LOOKING_FOR_WORK) {
            if (16 <= agent->GetMovePoints()) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

void FSCompeteForAllMovement::InsertEnslaveGoals(AiMain *ai)
{

	int i;
    BSetID h_id;
    BSetID c_id;
    ForeignAgent *him= NULL;
    ForeignCity  *his_city=NULL;
    BSet<ForeignAgent> *vu=NULL;
    BSet<ForeignCity> *vc=NULL;
	GoalEnslave *goal_obj=NULL;




    for (i=0; i<k_MAX_PLAYERS; i++)
	{

        if (i == ai->m_my_player_id) {
            continue;
        }

        if (ai->m_foreigner[i])
		{

            if (ai->m_foreigner[i]->IsAtColdWar())
			{

                vu = ai->m_foreigner[i]->GetVisibleUnits();

                for (him = vu->First(h_id); vu->Last(); him =  vu->Next(h_id))
				{
                    if (BAD_UTILITY < him->PretestEnslave()) {

                        if (him->IsLoneSettler(ai)) {

   					        goal_obj = new GoalEnslave(ai, h_id, him);

					        ai->m_planner->Add_Goal(goal_obj);

                            him->SetHasEnslaveGoal(TRUE);

                            him->SetEnslaveGoalID(goal_obj->goal_ID);

                        }
                    }
                }

                vc = ai->m_foreigner[i]->GetKnownCities();

				MapPointData pos;
				PLAYER_INDEX converted_to;
				BOOL is_unknown_id;
                for (his_city = vc->First(c_id); vc->Last(); his_city =  vc->Next(c_id))
				{


					his_city->GetPos(pos);
					converted_to =
						ai->m_world->GetConvertedTo(c_id.GetVal(),
													&is_unknown_id,
													&pos);
					if (ai->m_world->GetHasCityWalls(&pos) ||
						ai->m_world->GetHasForceField(&pos) ||
						converted_to == ai->my_player_index ||
						his_city->GetPopulation(ai) < 2)
						continue;

                    if (BAD_UTILITY < his_city->PretestEnslave()) {

 						goal_obj = new GoalEnslave(ai, c_id, his_city);
						ai->m_planner->Add_Goal(goal_obj);

						his_city->SetHasEnslaveGoal(ai, TRUE);

						his_city->SetEnslaveGoalID(ai, goal_obj->goal_ID);

                    }
                }
            }

        }

    }
}








void FSCompeteForAllMovement::Insert_Attack_Goals(AiMain *ai)
{

	int i;
    ForeignAgent *him= NULL;
    BSet<ForeignAgent> *vu=NULL;
    BSetID h_id;
	GoalAttack *attack_the_bastard;
	MapPointData pos;


    for (i=0; i<k_MAX_PLAYERS; i++)
	{

        if (ai->m_foreigner[i])
		{

            if (ai->m_foreigner[i]->IsAtHotWar() ||
				ai->m_foreigner[i]->IsAtColdWar())
			{

                vu = ai->m_foreigner[i]->GetVisibleUnits();

                for (him = vu->First(h_id); vu->Last(); him =  vu->Next(h_id))
				{

					him->SetupSpecialGoals(ai);








					if (him->attack_me_goal == NULL &&
						ai->m_foreigner[i]->IsAtHotWar())
					{

						him->GetPos(pos);

						attack_the_bastard = new GoalAttack(ai, him, pos);

						ai->m_planner->Add_Goal(attack_the_bastard);

						him->attack_me_goal = attack_the_bastard;
					}

                }

            }

        }

    }
}











void FSCompeteForAllMovement::Insert_Seige_Goals(AiMain *ai)
{

    BSet<ForeignCity> *vc=NULL;
    BSetID c_id;
    ForeignCity *his_city=NULL;


	GoalCityTarget::SetupCityTargetGoals(ai);
}










































































void FSCompeteForAllMovement::Insert_Perimeter_And_Patrol_Goals(AiMain *ai)
{

	sint32 map_rows, map_columns;
	sint32 row, column;
	sint16 height;
	MapPointData new_goal_pos;
    ArmyGoal *new_goal = NULL;
	sint32 distance_to_empire;
	sint32 to_patrol_or_not_to_patrol;
	PLAYER_INDEX owner;
	uint32 army_id;
	sint32 unit_type;
	sint32 num_units;
	uint32 city_id;
	BOOL unit_visible;
	BOOL city_visible;
	BOOL mine_visible;
    BOOL can_be_expelled;


	ai->m_planner->the_strategic_map->Get_Empire_Map_Dimensions(
		map_rows, map_columns);









	for (height = GROUND_Z; height <= SPACE_Z; height++)
	{

		for (row = 0; row < map_rows; row++)
		{

			for (column = 0; column < map_columns; column++)
			{

				distance_to_empire =
					ai->m_planner->the_strategic_map->Get_Distance_To_Empire_Mapgrid_Value(
						row, column);

				if (distance_to_empire == 1)
				{

					ai->m_planner->the_strategic_map->Get_Empire_Mapgrid_Random_Tile(
						row, column, new_goal_pos);

					new_goal_pos.z = height;




					if (ai->m_world->IsCellOwned(&new_goal_pos))
						{
							PLAYER_INDEX dst_owner =
								ai->m_world->GetCellOwner(&new_goal_pos);
							if (ai->m_player->HaveActiveAgreement(dst_owner,
									 AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS))

								continue;

							if	(ai->m_foreigner[dst_owner] &&
								 !ai->m_foreigner[dst_owner]->IsAtColdWar())

								continue;
						}




					ai->m_world->GetCellContents( &new_goal_pos,
						&owner,
						&army_id,
						&unit_type,
						&num_units,
						&city_id,
						&unit_visible,
						&city_visible,
						&mine_visible,
                        &can_be_expelled);

					if (city_id != 0 && owner != ai->my_player_index)
						continue;

					if (army_id != 0 && owner != ai->my_player_index)
						if (!can_be_expelled)
							continue;


					new_goal = new Goal_Perimeter(ai, 0.0, new_goal_pos);

					ai->m_planner->Add_Goal(new_goal);

				}

				else if ((distance_to_empire == 0) &&
						 (ai->m_planner->the_AIP.patrol_one_in_n >= 1))
				{

					to_patrol_or_not_to_patrol = 1 +
						ai->m_rand->Next(ai->m_planner->the_AIP.patrol_one_in_n);

					if (to_patrol_or_not_to_patrol == 1)
					{


						ai->m_planner->the_strategic_map->Get_Empire_Mapgrid_Random_Tile(
							row, column, new_goal_pos);
						new_goal_pos.z = height;




						ai->m_world->GetCellContents( &new_goal_pos,
							&owner,
							&army_id,
							&unit_type,
							&num_units,
							&city_id,
							&unit_visible,
							&city_visible,
							&mine_visible,
                            &can_be_expelled);

						if (city_id != 0 && owner != ai->my_player_index)
							continue;

						if (army_id != 0 && owner != ai->my_player_index)
							if (!can_be_expelled)
								continue;


						new_goal = new Goal_Patrol(ai, 0.0, new_goal_pos);

						ai->m_planner->Add_Goal(new_goal);

					}

				}

			}

		}

	}
}










void FSCompeteForAllMovement::Insert_Explore_Goals(AiMain *ai)
{

	sint32 map_rows, map_columns;
	sint32 row, column;
	MapPointData new_goal_pos;
    ArmyGoal *new_goal = NULL;
	sint32 explore_this;

	BOOL x_wrap;
	BOOL y_wrap;
	PLAYER_INDEX cell_owner;
	PLAYER_INDEX owner;
	uint32 army_id;
	sint32 unit_type;
	sint32 num_units;
	uint32 city_id;
	BOOL unit_visible;
	BOOL city_visible;
	BOOL mine_visible;
    BOOL can_be_expelled;

	x_wrap = ai->m_world->IsXwrap();
	y_wrap = ai->m_world->IsYwrap();

	ai->m_planner->the_strategic_map->Get_Explore_Map_Dimensions(
		map_rows, map_columns);





	for (row = 0; row < map_rows; row++)
	{

		for (column = 0; column < map_columns; column++)
		{

			explore_this =
				ai->m_planner->the_strategic_map->Must_We_Explored_Here(row, column);

			if (explore_this)
			{

				ai->m_planner->the_strategic_map->Get_Explore_Mapgrid_Center_Tile(
						row, column, new_goal_pos);


				if (ai->m_world->IsCellOwned(&new_goal_pos))
					{
						cell_owner = ai->m_world->GetCellOwner(&new_goal_pos);
						if (ai->m_player->HaveActiveAgreement(cell_owner,
							  AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS ))

							continue;
					}




				ai->m_world->GetCellContents( &new_goal_pos,
					&owner,
					&army_id,
					&unit_type,
					&num_units,
					&city_id,
					&unit_visible,
					&city_visible,
					&mine_visible,
                    &can_be_expelled);

				if (city_id != 0 && owner != ai->my_player_index)
					continue;

				if (army_id != 0 && owner != ai->my_player_index)
					if (!can_be_expelled)
						continue;


				new_goal = new GoalExplore(ai, 0.0, new_goal_pos);

				ai->m_planner->Add_Goal(new_goal);













			}

		}

	}
}




















void FSCompeteForAllMovement::Insert_Sally_Goals(AiMain *ai)
{

	int i;
    BSetID h_id;
    ForeignAgent *him= NULL;
    BSet<ForeignAgent> *vu=NULL;
    GoalSally *new_goal = NULL;


    for (i=0; i<k_MAX_PLAYERS; i++)
		{

			if (i == ai->m_my_player_id)
				{
					continue;
				}

			if (ai->m_foreigner[i])
				{




					if (ai->m_foreigner[i]->IsAtHotWar())
						{

							vu = ai->m_foreigner[i]->GetVisibleUnits();

							for (him = vu->First(h_id);
								 vu->Last();
								 him =  vu->Next(h_id))
								{
									him->SetupSallyGoal(ai);
								}
						}
				}
		}
}
