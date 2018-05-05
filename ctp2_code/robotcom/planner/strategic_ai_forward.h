#ifndef __Strategic_AI_Forward_h__
#define __Strategic_AI_Forward_h__










class AIP;


typedef class Planner * Planner_ptr;

class Scheduler;

class Agent;

class ArmyGoal;

class ArmyAgent;

class Goal;

class GameObject;

class AiMain;

class Plan;

typedef class Goal * Goal_ptr;

typedef class Escort_Goal * Escort_Goal_ptr;

typedef class Mapgrid_Goal * Mapgrid_Goal_ptr;

typedef struct tag_strategy_map * strategy_map_ptr;

class Squad;
typedef class Squad * squad_ptr;

typedef class Plan * Plan_ptr;

typedef class Unit * Unit_ptr;

template <class DATA_TYPE> class list_array;
template <class DATA_TYPE, class KEY_TYPE> class sorted_array;
template <class T> class semi_dynamic_array;

#include "robotcom/planner/Squad_And_Goal_Types.h"

#include "robotcom/planner/dr_debug.h"
#include "common.h"
#include "robotcom/planner/linked_list.h"
#include "semi_dynamic_array.h"
#include "robotcom/planner/sorted_array.h"
#include "robotcom/planner/matrix.h"

#include "robotcom/planner/unitflow.h"

#include "robotcom/planner/aip.h"

#include "robotcom/planner/squad.h"

#include "robotcom/planner/goal.h"

#include "Plan.h"

#include "Scheduler.h"

#include "robotcom/planner/planner.h"

#include "robotcom/planner/ailog.h"

#endif
