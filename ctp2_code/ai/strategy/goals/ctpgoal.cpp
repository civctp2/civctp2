//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Goal handling
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
// _DEBUG_SCHEDULER
// _DEBUG
// - Generate debug version
// 
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Resolved ambiguous sqrt calls.
// - Added the Target Position (X,Y) to The ArmyText (the text that is
//   shown on the screen in Debug mode, with the command 'armytext') - calvitix
//   update : it now show if the Army is grouping or going to board...
// - Added an attribute for the goal subtask (used by armytext display)
// - Changed the task forces for goals (based on objectif's threat)
// - Added a Debug Log (activated with k_dbg_scheduler_all) to see the goal priority computing
//   (raw priority, plus value of each modifier)
// - Added an Ungroup condition that can be associated to goals (as it exists RallyFirst) - Calvitix
// - Added consitions for Treaspassing units : (to favorise their Retreat) - Calvitix
// - Added conditions for wounded units : IsWoundedbonus (see goals.txt) - Calvitix
// - Added conditions for territory owner and IsVisible (see goals.txt) - Calvitix
// - Correct the determination of Empire Center and foreign Empire center - Calvitix
// - Invalid goals if in territory with nontrespassing treaty - Calvitix
// - Allow incursion permission to stealth units
// - Rollback the determination of check_dest (problem with special units that couldn't find 
//   their path to goal anymore
// - Added other conditions to determine the RETREAT goals (and set max of 2 units per city 
//   for that goals, hardcoded (to not depend on threat)
// - Forbid to army with settlers to perform ATTACK or SEIGE goals
// - Allow Units that are grouping to both move (if they are far enough)
// - Corrected condition to give unowned bonus in Compute_Raw_Priority, now it is 
//   given if the according territory has owner -1 instead of 0 
//   - Feb. 21st 2005 Martin Gühmann
// - Removed unnecessary reinitialization in Compute_Needed_Troop_Flow for trade
//   routes and tileimps. - Feb. 21st 2005 Martin Gühmann
// - Started to reimplement the refuel order. - May 7th 2005 Martin Gühmann
// - Removed .NET warnings - May 7th 2005 Martin Gühmann
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Added checks and Asserts to try to prevent crashes.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "gfx_options.h"
#include "CTPDatabase.h"
#include "GoalRecord.h"
#include "StrategyRecord.h"
#include "OrderRecord.h"
#include "Cell.h"
#include "World.h"
#include "player.h"				// g_player, PLAYER_UNASSIGNED
#include "Order.h"
#include "ArmyData.h"
#include "UnitData.h"
#include "UnitPool.h"
#include "ArmyPool.h"
#include "MoveFlags.h"
#include "ctpgoal.h"
#include "Diplomat.h"
#include "ctpagent.h"
#include "mapanalysis.h"
#include "settlemap.h"
#include "UnitRecord.h"
#include "AgreementMatrix.h"
#include "ctpaidebug.h"
#include "CityInfluenceIterator.h"
#include "ctpai.h"
#include "terrainutil.h"
#include "WonderRecord.h"
#include "advanceutil.h"







extern CTPDatabase<GoalRecord> *g_theGoalDB;
extern MapPoint g_mp_size;
extern UnitPool         *g_theUnitPool;
extern ArmyPool			*g_theArmyPool;


CTPGoal::CTPGoal()
:
    m_target_pos    (),
    m_target_city   (),
    m_target_army   (),
    m_sub_task      (SUB_TASK_GOAL)
{ ; }

CTPGoal::~CTPGoal()
{ ; }


bool CTPGoal::operator == (const CTPGoal & rval) const
{
	return (m_playerId == rval.m_playerId)          &&
		   (m_goal_type == rval.m_goal_type)        &&
		   (m_target_army == rval.m_target_army)    &&
  		   (m_target_city == rval.m_target_city)    &&
		   (m_target_pos == rval.m_target_pos);
}


// TODO: find out what this operator if used for. The relation between this 
// definition and the one in Goal is completely unclear.
bool CTPGoal::operator < (const CTPGoal & rval) const
{
	return (m_playerId < rval.m_playerId)           &&
           (m_goal_type < rval.m_goal_type)         &&
		   (m_target_army < rval.m_target_army)     &&
		   (m_target_city < rval.m_target_city)     &&
		   (m_target_pos.x < rval.m_target_pos.x)   && 
		   (m_target_pos.y < rval.m_target_pos.y);
}


void CTPGoal::Set_Target_Pos(const MapPoint & pos)
{
	m_target_pos = pos;
}


void CTPGoal::Set_Target_Army(const Army & army)
{
	m_target_army = army;
}


void CTPGoal::Set_Target_City(const Unit & city)
{
	m_target_city = city;
}


const MapPoint CTPGoal::Get_Target_Pos(const Army & army) const
{
	const GoalRecord *rec = g_theGoalDB->Get(m_goal_type);
	sint32 i,j;
	sint32 max_squared_dist = (g_theWorld->GetWidth() * g_theWorld->GetHeight());
	max_squared_dist *= max_squared_dist;
	sint32 best_squared_dist = max_squared_dist;
	sint32 tmp_squared_dist;
	MapPoint best_target_pos;
	MapPoint army_pos = army->RetPos();

	
	if (rec->GetTargetTypeTradeRoute())
	{
		Assert( m_target_city.m_id != 0);

		const TradeDynamicArray* trade_routes = 
			m_target_city.GetCityData()->GetTradeSourceList();
		for(i = 0; i < trade_routes->Num(); i++) 
		{
			const DynamicArray<MapPoint>* path = (*trade_routes)[i].GetPath();

			for (j = 1; j < path->Num()-1; j++)
			{
				Cell *cell = g_theWorld->GetCell(path->Get(j));


				if (cell->HasCity())
					continue;

				if (cell->CanEnter(army->GetMovementType()))
				{
					tmp_squared_dist = 
					MapPoint::GetSquaredDistance(path->Get(j), army_pos);
					if (tmp_squared_dist < best_squared_dist)
					{
						best_squared_dist = tmp_squared_dist;
						best_target_pos = path->Get(j);
					}
				}
			}

			if (best_squared_dist < max_squared_dist)
				return best_target_pos;
			else
			{
				bool NO_TRADE_ROUTE_TARGET_POS_FOUND = true;
				Assert(NO_TRADE_ROUTE_TARGET_POS_FOUND);
			}
		}
	}
	else if (rec->GetTargetTypeImprovement())
	{
		Assert( m_target_city.m_id != 0);

		if (m_target_city->GetCityData()->WasTerrainImprovementBuilt())
		{
			CityInfluenceIterator it(m_target_city.RetPos(), m_target_city.GetCityData()->GetSizeIndex());
			for(it.Start(); !it.End(); it.Next()) 
			{
				Cell *cell = g_theWorld->GetCell(it.Pos());


				if (!(cell->GetCityOwner() == m_target_city))
					continue;


				if (cell->GetNumDBImprovements() <= 0)
					continue;


				if (m_target_city.RetPos() == it.Pos())
					continue;

				if (cell->CanEnter(army->GetMovementType()))
				{
					tmp_squared_dist = MapPoint::GetSquaredDistance(it.Pos(), army_pos);
					if (tmp_squared_dist < best_squared_dist)
					{
						best_squared_dist = tmp_squared_dist;
						best_target_pos = it.Pos();
					}
				}
			}


			if (best_squared_dist < max_squared_dist)
				return best_target_pos;
			else
			{
				bool NO_TILE_IMPROVEMENT_TARGET_POS_FOUND = true;
				Assert(NO_TILE_IMPROVEMENT_TARGET_POS_FOUND);
			}
		}
	}
	else if (rec->GetTargetTypePetrolStation()){
		sint32 distance_to_refuel;
		MapPoint refuel_pos;
		CtpAi::GetNearestRefuel(army, army_pos, refuel_pos, distance_to_refuel);
		return refuel_pos;
	}


	return Get_Target_Pos();
}


const MapPoint & CTPGoal::Get_Target_Pos() const
{
	static MapPoint pos;    // ugly life-time extension

	if (m_target_army != ID()) 
	{
		if (m_target_army.IsValid())
        {
			m_target_army->GetPos(pos);
        }
		else
		{
			pos.x = -1;
			pos.y = -1;
		}
	}
	else if (m_target_city != ID())
	{
		
		if (m_target_city.IsValid())
		{
			m_target_city.GetPos(pos);
		}
		else
		{
			pos.x = -1;
			pos.y = -1;
		}
	}
	else
    {
		return m_target_pos;
    }
	
	if (pos.x == -1 || pos.y == -1)
	{
		
		Assert(0);
		
		pos.x = 0;
		pos.y = 0;
	}

	return pos;
}


const Army & CTPGoal::Get_Target_Army() const
{
	return m_target_army;
}


void CTPGoal::Set_Sub_Task(const SUB_TASK_TYPE & sub_task)
{
    m_sub_task = sub_task;
}


const SUB_TASK_TYPE & CTPGoal::Get_Sub_Task() const
{
    return m_sub_task;
}




const Unit & CTPGoal::Get_Target_City() const
{
	return m_target_city;
}


sint32 CTPGoal::Get_Target_Value() const
{
	sint32              value   = 0;
	const GoalRecord *  rec     = g_theGoalDB->Get(m_goal_type);
    Assert(rec);

	if ( rec->GetTargetTypeAttackUnit() || 
		rec->GetTargetTypeSpecialUnit() )
	{
		const Army &    army = Get_Target_Army();
	    sint32          tmp;
		army->GetArmyStrength(tmp,tmp,tmp,tmp,tmp,tmp,value);
	}
	else if (rec->GetTargetTypeCity())
	{
		if (m_target_city.IsValid())
        {
			value = m_target_city->GetCityData()->GetValue();
        }
	}
	else if ( rec->GetTargetTypeTradeRoute() )
	{
		const Unit & city = Get_Target_City();
		value = city->GetCityData()->GetGoldFromTradeRoutes();
	}

	return value;
}


PLAYER_INDEX CTPGoal::Get_Target_Owner() const
{
	PLAYER_INDEX       target_owner = PLAYER_UNASSIGNED;
	GoalRecord const * goal_record  = g_theGoalDB->Get(m_goal_type);
	Assert(goal_record);
	
	if(goal_record->GetTargetTypeAttackUnit()
	|| goal_record->GetTargetTypeSpecialUnit()
	)
    {
		target_owner = m_target_army.GetOwner();
	}
	else if(goal_record->GetTargetTypePetrolStation()){
		if(m_target_city != ID() || g_theWorld->IsAirfield(Get_Target_Pos())){
			target_owner = g_theWorld->GetOwner(Get_Target_Pos());
		}
		else if(m_target_army != ID()){
			target_owner = m_target_army.GetOwner();
		}
	}
	else{
		MapPoint pos(Get_Target_Pos());
		if(pos.x >= 0){
			target_owner = g_theWorld->GetOwner(Get_Target_Pos());
		}
	}

	return target_owner;
}



Agent_ptr CTPGoal::Rollback_Agent(Agent_List::const_iterator & agent_iter)
{
	Assert(agent_iter != m_agents.end());
	CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr) (*agent_iter);
    Assert(ctpagent_ptr);

#ifdef _DEBUG_SCHEDULER
	if (ctpagent_ptr->Get_Army().IsValid())
	{
		Assert(ctpagent_ptr->Get_Army()->m_theAgent == ctpagent_ptr);
		Assert(ctpagent_ptr->Get_Army()->m_theGoal == this);
	}
#endif // _DEBUG_SCHEDULER

	
	
	
	if (ctpagent_ptr->Get_Is_Dead() == false &&
		g_player[m_playerId] && 
		g_player[m_playerId]->m_playerType == PLAYER_TYPE_ROBOT)
    {
		ctpagent_ptr->Get_Army()->ClearOrders();
    }

	return Goal::Rollback_Agent(agent_iter);
}




bool CTPGoal::Is_Execute_Incrementally() const
{
	return g_theGoalDB->Get(m_goal_type)->GetExecuteIncrementally();
}



void CTPGoal::Compute_Needed_Troop_Flow()
{

	const GoalRecord *goal_record = g_theGoalDB->Get(m_goal_type);
	sint32 threat = MapAnalysis::GetMapAnalysis().GetThreat(m_playerId, Get_Target_Pos());

	m_current_needed_strength   = Squad_Strength(1);
		// why only one unit ? Why then zero units? - Martin Gühmann
		// by bringing a real army to pirate or pillage, it can be ready for seige or attack
		// a single unit is quite defenseless - Calvitix

	if(goal_record->GetTargetTypeChokePoint()){

		// Need also attack and ranged strength - Calvitix
		m_current_needed_strength.Set_Attack(threat / 2);
		m_current_needed_strength.Set_Ranged(threat / 2);
		m_current_needed_strength.Set_Value(threat);
	}

	else if(goal_record->GetTargetTypeImprovement()
	     || goal_record->GetTargetTypeTradeRoute()
	     ){

		m_current_needed_strength.Set_Attack(threat / 2);
		m_current_needed_strength.Set_Ranged(threat / 2);
		m_current_needed_strength.Set_Value(threat);
	}

	else if(goal_record->GetTargetTypeEndgame()){

		if(goal_record->GetTargetOwnerSelf())
			m_current_needed_strength.Set_Defense(threat);
		
		else
			m_current_needed_strength.Set_Attack(threat);

		//to be sure that the global force of the army will be enough
		// (not only a wounded unit for example)
		m_current_needed_strength.Set_Value(threat);

	}
	
	else if(goal_record->GetTargetTypeCity()
	     && goal_record->GetTargetOwnerSelf()
	     ){

		// tweak to obtain RETREAT Goal definition : TO DO - 'cleaner' method - Calvitix
		// Set to 2 to so that units with no goals will retreat to the nearest city
		// (I Prefer that method than the GARRISON troops, that are not able to leave the city)
		// cities will be better defended if their is enough units, otherwise units will be 
		// affected to relevant goals
		if(g_theGoalDB->Get(m_goal_type)->GetTargetTypeCity()
		&& g_theGoalDB->Get(m_goal_type)->GetTargetOwnerSelf()
		&& goal_record->GetTreaspassingArmyBonus() > 0
		){
			m_current_needed_strength.Set_Agent_Count(2);
		}
		else{

			const StrategyRecord & strategy = 
				Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

			sint32 offensive_garrison;
			sint32 defensive_garrison;
			sint32 ranged_garrison;
			strategy.GetOffensiveGarrisonCount(offensive_garrison);
			strategy.GetDefensiveGarrisonCount(defensive_garrison);
			strategy.GetRangedGarrisonCount(ranged_garrison);

			// why only defensive units ?
			// added ranged units - Calvitix
			m_current_needed_strength.Set_Defense(threat * 2 / 3);
			m_current_needed_strength.Set_Ranged(threat / 3);
			m_current_needed_strength.Set_Value(threat);

			//not used for the moment (only attack or defense strength is considerated
			//(see army_strength > operator) - Calvitix
			m_current_needed_strength.Set_Defenders(static_cast<sint16>(defensive_garrison + offensive_garrison));
			m_current_needed_strength.Set_Ranged_Units(static_cast<sint16>(ranged_garrison));
		}
	}
	else if((goal_record->GetTargetTypeAttackUnit()
	     ||  goal_record->GetTargetTypeCity())
	     && !goal_record->GetTargetOwnerSelf()
	     && !goal_record->GetTargetTypeSpecialUnit()
	     ){

		// a real Attack force, depending on threat
		m_current_needed_strength.Set_Attack(threat); 
		m_current_needed_strength.Set_Defense(threat);
		m_current_needed_strength.Set_Ranged(threat);
		m_current_needed_strength.Set_Value(threat);
	}
	
	else if(goal_record->GetTargetTypeBorder()){

		// assuming threat is the global strength to use (to be coherent with other changes) - Calvitix
		m_current_needed_strength.Set_Defense(threat / 2);
		m_current_needed_strength.Set_Attack(threat / 2);
		m_current_needed_strength.Set_Value(threat);
	}
	else if (   goal_record->GetTargetTypeSettleLand()
	        ||  goal_record->GetTargetTypeSettleSea()
	        ||  goal_record->GetTargetTypePetrolStation()
            )
    {
		// No strength is needed
	}
	else{
		m_current_needed_strength.Set_Pos_Strength(Get_Target_Pos());
	}





	const StrategyRecord & strategy =
		Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	const StrategyRecord::ForceMatch *force_match = NULL;

	
	switch (goal_record->GetForceMatch())
	{
	case k_Goal_ForceMatch_Offensive_Bit:
		force_match = strategy.GetOffensivePtr();
		break;
	case k_Goal_ForceMatch_Defensive_Bit:
		force_match = strategy.GetDefensivePtr();
		break;
	case k_Goal_ForceMatch_StealthAttack_Bit:
		force_match = strategy.GetStealthAttackPtr();
		break;
	case k_Goal_ForceMatch_Bombard_Bit:
		force_match = strategy.GetBombardPtr();
		break;
	case k_Goal_ForceMatch_Special_Bit:
		force_match = strategy.GetSpecialPtr();
		break;
	case k_Goal_ForceMatch_Harass_Bit:
		force_match = strategy.GetHarassPtr();
		break;
	default:

		Assert(0);
	}


	Assert(force_match);
	m_current_needed_strength.Set_Force_Matching(
		force_match->GetAttackMatch(),
		force_match->GetDefenseMatch(),
		force_match->GetRangedMatch(),
		force_match->GetBombardMatch(),
		force_match->GetValueMatch());

	if (goal_record->GetNeverSatisfied())
	{
		//Dangerous if not used with execute incrementally - Calvitix
		//my changes in Sort_Matches will give for the moment to a neverSatisfied
		//goal with high priority all the ressources available !!
		// for the moment, remove from the only one goal that use it (GOAL_RETREAT)
		m_current_needed_strength.Set_Agent_Count(9999);
	}

	
	if (m_current_needed_strength.Get_Transport() > 0)
	{
		sint32 const    dest_cont       = 
            g_theWorld->GetContinent(Get_Target_Pos());
		bool            need_transport  = false;

		for 
        (
		    Agent_List::iterator agent_iter = m_agents.begin(); 
			agent_iter != m_agents.end() && !need_transport;
			++agent_iter
        ) 
		{
			CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr) *agent_iter;
            Assert(ctpagent_ptr);

			need_transport = 
                dest_cont != g_theWorld->GetContinent(ctpagent_ptr->Get_Pos());
		}

		if (!need_transport)
		{
			m_current_needed_strength.Set_Transport(0);
		}
	}
}




Utility CTPGoal::Compute_Matching_Value( const Agent_ptr agent ) const
{
	
	CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr) agent;

	Player *player_ptr = g_player[ m_playerId ];
	Assert(player_ptr && ctpagent_ptr);

	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	if (ctpagent_ptr->Get_Is_Dead())
	{
		return BAD_UTILITY;
	}

	MapPoint dest_pos = Get_Target_Pos(ctpagent_ptr->Get_Army());

	if (!Pretest_Bid(ctpagent_ptr, dest_pos))
	{
		return BAD_UTILITY;
	}

	PLAYER_INDEX target_owner = Get_Target_Owner();
	const Diplomat & diplomat = Diplomat::GetDiplomat(m_playerId);


	if( target_owner > 0 &&
		(!diplomat.IncursionPermission(target_owner)))
	{
		
		bool isspecial, cancapture, haszoc, canbombard;
		bool isstealth;
		sint32 maxattack, maxdefense;
		ctpagent_ptr->Get_Army()->CharacterizeArmy( isspecial, 
			isstealth, 
			maxattack, 
			maxdefense, 
			cancapture,
			haszoc,
			canbombard);
		if (!isspecial || maxattack > 0 || haszoc)
		{
			return BAD_UTILITY;
		}
	}


	sint8 new_garrison = 0;
	double new_garrison_strength = 0.0;
	if (NeededForGarrison(ctpagent_ptr, dest_pos, new_garrison, new_garrison_strength))
	{
		return BAD_UTILITY;
	}




	sint32 transports, max,empty;
	if (m_current_needed_strength.Get_Transport() > 0 &&
		ctpagent_ptr->Get_Army()->GetCargo(transports, max, empty) &&
		empty > 0 && 
		m_agents.size() > 1)
	{
		CTPAgent const *target_agent_ptr = NULL;
		
		for (Agent_List::const_iterator agent_iter = m_agents.begin(); agent_iter != m_agents.end(); ++agent_iter)
		{
			target_agent_ptr = (CTPAgent const *) (*agent_iter);
			
			if (ctpagent_ptr != target_agent_ptr)
				break;
		}
		
		Assert(ctpagent_ptr != target_agent_ptr);

		dest_pos = target_agent_ptr->Get_Pos();
	}
	
	
	Utility bonus = 0;

//Set if unit is wounded and it is a retreat of defense goal, add bonus 
//to goalpriority + matching

	MapPoint armyPos = ctpagent_ptr->Get_Pos();	
	PLAYER_INDEX PosOwner = g_theWorld->GetOwner(armyPos);

	if(g_theGoalDB->Get(m_goal_type)->GetTargetTypeCity() 
	&& g_theGoalDB->Get(m_goal_type)->GetTargetOwnerSelf()
	){
		// For Defend or Retreat goals
		if(ctpagent_ptr->Get_Army()->IsWounded()
		&&!ctpagent_ptr->Get_Army()->IsObsolete()
		){
			bonus+= g_theGoalDB->Get(m_goal_type)->GetWoundedArmyBonus();
		}
		if(PosOwner != m_playerId
		&&!diplomat.IncursionPermission(PosOwner)
		){
			bonus+= g_theGoalDB->Get(m_goal_type)->GetTreaspassingArmyBonus();
		}
		
	}
	else if((g_theGoalDB->Get(m_goal_type)->GetTargetOwnerColdEnemy()
	     ||  g_theGoalDB->Get(m_goal_type)->GetTargetOwnerHotEnemy())
	     && (g_theGoalDB->Get(m_goal_type)->GetTargetTypeAttackUnit()
	     ||  g_theGoalDB->Get(m_goal_type)->GetTargetTypeCity())
	     ){  //For Attack goals (unit or city)

		if(ctpagent_ptr->Get_Army()->IsWounded() && !ctpagent_ptr->Get_Army()->IsObsolete()){
				bonus+= g_theGoalDB->Get(m_goal_type)->GetWoundedArmyBonus();
		}
		if(PosOwner != m_playerId && !diplomat.IncursionPermission(PosOwner)){
			bonus += g_theGoalDB->Get(m_goal_type)->GetTreaspassingArmyBonus();
		}
		if(ctpagent_ptr->Get_Army()->CanSettle()){
			// If there is a settler in the army... 
			bonus = BAD_UTILITY;
		}
	}


#if defined(_DEBUG)  // Add a debug report of goal computing (raw priority and all modifiers)
	double report_wounded = bonus;
#endif //_DEBUG
	
	if ( ctpagent_ptr->Get_Army()->IsObsolete() )
		bonus += g_theGoalDB->Get(m_goal_type)->GetObsoleteArmyBonus();

#if defined(_DEBUG)  // Add a debug report of goal computing (raw priority and all modifiers)
	double report_obsolete = bonus - report_wounded;
#endif //_DEBUG
	
	double min_move;
	double squared_distance = MapPoint::GetSquaredDistance(ctpagent_ptr->Get_Pos(), dest_pos);
	ctpagent_ptr->Get_Army().MinMovementPoints(min_move);
	double cell_dist = sqrt(squared_distance);
	double eta = (cell_dist * 100) / min_move;


	Utility raw_priority = Get_Raw_Priority();

	
	if (g_theGoalDB->Get(m_goal_type)->GetTreaspassingArmyBonus() > 0)
	{
		PLAYER_INDEX pos_owner = g_theWorld->GetCell(ctpagent_ptr->Get_Pos())->GetOwner();
// Added by Calvitix
		bool incursion_permissin = Diplomat::GetDiplomat(m_playerId).IncursionPermission(pos_owner);
		if (pos_owner >= 0 && !(incursion_permissin))
		{
			bonus += g_theGoalDB->Get(m_goal_type)->GetTreaspassingArmyBonus();

		}
	}
#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers)
            double report_Treaspassing = bonus - report_obsolete;
#endif //_DEBUG

    
    
	sint32 distance_modifier;
	strategy.GetDistanceModifierFactor(distance_modifier);
	Utility time_term = static_cast<Utility>( (eta * distance_modifier) + cell_dist);
#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers)
            double report_time_term = time_term;
#endif //_DEBUG
	
	if (!ctpagent_ptr->Get_Army()->HasCargo())
	{
		
		
		sint32 army_cont, goal_cont;
		BOOL is_land;
		g_theWorld->GetContinent(dest_pos, goal_cont, is_land);
		g_theWorld->GetContinent(ctpagent_ptr->Get_Pos(), army_cont, is_land);
		if ( (goal_cont != army_cont) && 
			(ctpagent_ptr->Get_Army()->GetMovementTypeAir() == FALSE) &&
			g_player[m_playerId]->GetCargoCapacity() <= 0)
		{
			
			
			
			
			return Goal::BAD_UTILITY;
		}
	}

	Utility match = bonus + time_term + raw_priority;
	
#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, this->Get_Player_Index(), m_goal_type, -1,
	("\t %9x,\t%9x (%3d,%3d),\t%s (%3d,%3d),\t%8d,\t%8d,\t%8f,\t%8f,\t%8f,\t%8f,\t%8f,\t%8f,\t%8d \n",
	this,
	ctpagent_ptr,
	ctpagent_ptr->Get_Pos().x,
	ctpagent_ptr->Get_Pos().y,
	g_theGoalDB->Get(m_goal_type)->GetNameText(),
	this->Get_Target_Pos().x,
	this->Get_Target_Pos().y,
	match,
	raw_priority,
	cell_dist,
	eta,
	bonus,
	report_wounded,
	report_obsolete,
	report_Treaspassing,
	time_term));
#endif //_DEBUG

	return match;
}




Utility CTPGoal::Compute_Raw_Priority()
{
	Player *    player_ptr = g_player[m_playerId];
	Assert(player_ptr);

	if (!player_ptr || Get_Totally_Complete()) 
	{
		m_raw_priority = Goal::BAD_UTILITY;
		return m_raw_priority;
	}
	
	MapPoint target_pos = Get_Target_Pos();


	const Diplomat & diplomat = Diplomat::GetDiplomat(m_playerId);


	const StrategyRecord & strategy = diplomat.GetCurrentStrategy();

	
	if ((player_ptr->CanUseSeaTab() == FALSE ) && 
		(g_theWorld->IsWater(target_pos) || g_theWorld->IsShallowWater(target_pos)))
	{
		m_raw_priority = Goal::BAD_UTILITY;
		return m_raw_priority;
	}

	const GoalRecord * goal_rec = g_theGoalDB->Get(m_goal_type);
	const MapAnalysis & map = MapAnalysis::GetMapAnalysis();
	PLAYER_INDEX target_owner = Get_Target_Owner();

	//alway compute a foreign center (even if the target if owned by the player
	// otherwise it compute with coords (0,0) !!
	MapPoint empire_center;
	MapPoint foreign_empire_center;
	empire_center = map.GetEmpireCenter(m_playerId);
	if (target_owner > 0 && m_playerId != target_owner)
		foreign_empire_center = map.GetEmpireCenter(target_owner);
	else
		foreign_empire_center = map.GetNearestForeigner(m_playerId, target_pos);

	Assert(m_goal_type < strategy.GetNumGoalElement());
	Assert(strategy.GetGoalElement(m_goal_type) != NULL);
	
	double cell_value = (m_goal_type < strategy.GetNumGoalElement())
                        ? strategy.GetGoalElement(m_goal_type)->GetPriority()
                        : 0.0;

#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	double report_cell_value            = cell_value;
	double report_cell_lastvalue        = cell_value;
	double report_cell_threat           = 0.0;
	double report_cell_EnemyValue       = 0.0;
	double report_cell_AlliedValue      = 0.0;
	double report_cell_HomeDistance     = 0.0;
	double report_cell_EnemyDistance    = 0.0;
	double report_cell_Settle           = 0.0;
	double report_cell_Chokepoint       = 0.0;
	double report_cell_MaxPower         = 0.0;
	double report_cell_Unexplored       = 0.0;
	double report_cell_NotVisible       = 0.0;
	double report_cell_NoOwnerTerritory = 0.0;
	double report_cell_InHomeTerritory  = 0.0;
	double report_cell_InEnemyTerritory = 0.0;
#endif //_DEBUG
	
	double maxThreat = static_cast<double>(map.GetMaxThreat(m_playerId));
	if ( maxThreat > 0 )
	{
		cell_value += 
		( ( static_cast<double>(map.GetThreat(m_playerId, target_pos)) /
		     maxThreat) * 
		  g_theGoalDB->Get(m_goal_type)->GetThreatBonus() );
	
#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
		report_cell_threat = cell_value - report_cell_lastvalue;
		report_cell_lastvalue = cell_value;
#endif //_DEBUG
	}
	
	double maxEnemyValue = static_cast<double>(map.GetMaxEnemyValue(m_playerId));
	if (maxEnemyValue)
	{
		cell_value += 
		( ( static_cast<double>(map.GetEnemyValue( m_playerId, target_pos)) /
		    maxEnemyValue ) * 
		  goal_rec->GetEnemyValueBonus() );
	
#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
		report_cell_EnemyValue = cell_value - report_cell_lastvalue;
		report_cell_lastvalue = cell_value;
#endif //_DEBUG
	}
	
	double maxAlliedValue = static_cast<double>(map.GetMaxAlliedValue(m_playerId));
	if ( maxAlliedValue > 0)
	{
		cell_value += 
		( ( static_cast<double>(map.GetAlliedValue(m_playerId, target_pos)) /
		    maxAlliedValue ) * 
		  goal_rec->GetAlliedValueBonus() );
	
#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
		report_cell_AlliedValue = cell_value - report_cell_lastvalue;
		report_cell_lastvalue = cell_value;
#endif //_DEBUG
	}
	double maxPower = static_cast<double>(map.GetMaxPower(m_playerId));
	if (maxPower > 0)
	{
		cell_value += 
		( ( static_cast<double>(map.GetPower( m_playerId, target_pos)) /
		    maxPower ) * 
		goal_rec->GetPowerBonus() );

#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
		report_cell_MaxPower = cell_value - report_cell_lastvalue;
		report_cell_lastvalue = cell_value;
#endif //_DEBUG
	}
	
	cell_value += sqrt(static_cast<double>
	(MapPoint::GetSquaredDistance(target_pos, empire_center))) * goal_rec->GetDistanceToHomeBonus();

#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_HomeDistance = cell_value - report_cell_lastvalue;
	report_cell_lastvalue = cell_value;
#endif //_DEBUG
	
	if (foreign_empire_center.x != 0 && foreign_empire_center.y != 0)//Dangerious if the empire center has coords (0,0)
	{
		cell_value += sqrt(static_cast<double>
		(MapPoint::GetSquaredDistance(target_pos, foreign_empire_center))) * goal_rec->GetDistanceToEnemyBonus();
	}

#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_EnemyDistance = cell_value - report_cell_lastvalue;
	report_cell_lastvalue = cell_value;
#endif //_DEBUG
	
	if (g_theWorld->GetCell( target_pos )->GetIsChokePoint())
	{
		cell_value += goal_rec->GetChokePointBonus();
	}
	
#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_Chokepoint = cell_value - report_cell_lastvalue;
	report_cell_lastvalue = cell_value;
#endif //_DEBUG


	if (!player_ptr->IsExplored(target_pos))
	{
		cell_value += goal_rec->GetUnexploredBonus();
	}

#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_Unexplored = cell_value - report_cell_lastvalue;
	report_cell_lastvalue = cell_value;
#endif //_DEBUG

	if (!player_ptr->IsVisible(target_pos))
	{
		cell_value += goal_rec->GetNotVisibleBonus();
	}

#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_NotVisible = cell_value - report_cell_lastvalue;
	report_cell_lastvalue = cell_value;
#endif //_DEBUG


	PLAYER_INDEX territoryOwner = g_theWorld->GetCell( target_pos )->GetOwner();
	if (m_playerId == territoryOwner)
	{
		cell_value += goal_rec->GetInHomeTerritoryBonus();
	}

#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_InHomeTerritory = cell_value - report_cell_lastvalue;
	report_cell_lastvalue = cell_value;
#endif //_DEBUG

	if (m_playerId != territoryOwner && territoryOwner >= 0) // 0: Barbarian player
	{
		cell_value += goal_rec->GetInEnemyTerritoryBonus();
	}

#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_InEnemyTerritory = cell_value - report_cell_lastvalue;
	report_cell_lastvalue = cell_value;
#endif //_DEBUG

	if (territoryOwner == PLAYER_UNASSIGNED)
	{
		cell_value += goal_rec->GetNoOwnerTerritoryBonus();
	}

#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_NoOwnerTerritory = cell_value - report_cell_lastvalue;
	report_cell_lastvalue = cell_value;
#endif //_DEBUG

	
	if ( goal_rec->GetTargetTypeSettleLand() ||
		 goal_rec->GetTargetTypeSettleSea() )
	{
		cell_value += SettleMap::s_settleMap.GetValue(target_pos);

#if defined(_DEBUG)
		report_cell_Settle = cell_value - report_cell_lastvalue;
		report_cell_lastvalue = cell_value;
#endif //_DEBUG


	}

	
	sint32 threaten_bonus = GetThreatenBonus();

	m_raw_priority = (Utility) cell_value + threaten_bonus;

	
	Assert(m_raw_priority < Goal::MAX_UTILITY);
	Assert(m_raw_priority >= Goal::BAD_UTILITY);
	if (m_raw_priority > Goal::MAX_UTILITY)
		m_raw_priority = Goal::MAX_UTILITY-1;
	else if (m_raw_priority < Goal::BAD_UTILITY)
		m_raw_priority = Goal::BAD_UTILITY;


#if defined(_DEBUG) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, this->Get_Player_Index(), this->Get_Goal_Type(), -1,
	("\t %9x,\t%s, rc(%3d,%3d),\t%8f,\t%8f,\t%8f,\t%8f,\t%8f,\t%8f,\t%8f, rc(%3d,%3d),\t%8f, rc(%3d,%3d), \t%8f,\t%8f,\t%8f,\t%8f,\t%8f \n",
	this,
	goal_rec->GetNameText(),
	target_pos.x,
	target_pos.y,
	report_cell_value,
	report_cell_lastvalue,
	report_cell_threat,
	report_cell_EnemyValue,
	report_cell_AlliedValue,
	report_cell_MaxPower,
	report_cell_HomeDistance,
	empire_center.x,
	empire_center.y,
	report_cell_EnemyDistance,
	foreign_empire_center.x,
	foreign_empire_center.y,
	report_cell_Settle,
	report_cell_Chokepoint,
	report_cell_Unexplored,
	report_cell_NotVisible,
	threaten_bonus));
#endif //_DEBUG

	return m_raw_priority;
}






GOAL_RESULT CTPGoal::Execute_Task()
{
	

	
	if (Get_Totally_Complete())
		return GOAL_COMPLETE;

	
	if (Can_Be_Executed() == false)
		return GOAL_ALREADY_MOVED;

    Assert(m_agents.begin() != m_agents.end());
	
	CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr) *m_agents.begin();
	MapPoint goto_pos = Get_Target_Pos(ctpagent_ptr->Get_Army());
	MapPoint my_pos;
	bool rally_complete = true;
	Set_Sub_Task(SUB_TASK_GOAL);
	const GoalRecord *goal_record = g_theGoalDB->Get(m_goal_type);
	sint32 cells;

	// Add this condition to avoid that a 12 units army with SEIGE goal retreat 
	// at 1 tile near the city, because it has left 1 unit and has to group with 
	// another one. I Think it is better to go on an seige the city (if there is 
	// more than 2/3 left, if more than 8 units).
	bool hastogowithoutgrouping = (goal_record->GetNeverSatisfied() && ctpagent_ptr->GetRounds(goto_pos,cells) <= 1)
		                          && m_current_attacking_strength.Get_Agent_Count() > (2*k_MAX_ARMY_SIZE/3);
    if (Is_Satisfied() || Is_Execute_Incrementally() || hastogowithoutgrouping) 
    {
	
		if (goal_record->GetRallyFirst()) 
		{
			
			if ( !RallyComplete() )
            {
                Set_Sub_Task(SUB_TASK_RALLY);
            }
			
			if (Goal_Too_Expensive() == true)
				return GOAL_FAILED;

			
			if ( Ok_To_Rally() )
			{
				if ( RallyTroops() == false)
					return GOAL_FAILED;

				// If hastogowithoutgrouping is true, execute the goal 
				// even if the rally is not complete
				if (RallyComplete() == false)
					return GOAL_IN_PROGRESS;
				else
				{
					Set_Sub_Task(SUB_TASK_GOAL);
				}
			}
		}
	
	

//Added an Ungroup method (sometimes, for example to explore, it is more interessant
//to have a lot of small units rather than a huge army	
		else if (goal_record->GetUnGroupFirst()) 
		{

				Set_Sub_Task(SUB_TASK_UNGROUP);
				if ( UnGroupTroops() == false)
					return GOAL_FAILED;

				
				if (UnGroupComplete() == false)
					return GOAL_IN_PROGRESS;
				else
				{
					Set_Sub_Task(SUB_TASK_GOAL);
				}
		}
/*        else
        {
            Set_Sub_Task(SUB_TASK_GOAL);
        }*/
        Agent_List::iterator agent_iter;
        for (agent_iter = m_agents.begin(); agent_iter != m_agents.end();agent_iter++) 
		{
			
			
			ctpagent_ptr = (CTPAgent_ptr) *agent_iter;

			
			if (ctpagent_ptr->Get_Is_Dead() == true)
				continue;

			
			if (ctpagent_ptr->Get_Can_Be_Executed() == false)
				continue;

			if (GotoGoalTaskSolution(ctpagent_ptr, goto_pos, m_sub_task) == false)
			{
					
					
					
					if (Needs_Transport())
						return GOAL_NEEDS_TRANSPORT;
					else
						return GOAL_FAILED;
				}
		} 
		
    } 
	
    return GOAL_IN_PROGRESS;
} 


bool CTPGoal::Get_Totally_Complete() const
{
	
	if (Get_Invalid())
		return true;

	const GoalRecord *goal_record = g_theGoalDB->Get(m_goal_type);
    Diplomat & diplomat = Diplomat::GetDiplomat(m_playerId);
	PLAYER_INDEX target_owner = Get_Target_Owner();
	MapPoint target_pos = Get_Target_Pos();

	Player *player_ptr = g_player[ m_playerId ];
	Assert(player_ptr != NULL);

	
	
	if (m_playerId > 0 && target_owner > 0 && goal_record->GetTargetProtectionWonder())
	{
		const WonderRecord *wonder_rec = goal_record->GetTargetProtectionWonderPtr();
		
		
		if ((AgreementMatrix::s_agreements.TurnsAtWar(m_playerId, target_owner) < 0) &&
			g_player[target_owner] &&
			(g_player[target_owner]->GetBuiltWonders() & ((uint64)1 << (uint64)(wonder_rec->GetIndex()))))
			return true;
	}

	
	if (MapAnalysis::GetMapAnalysis().PlayerCanEnter(m_playerId, target_pos) == false)
		return true;

	
	bool isspecial; 
	bool isstealth = false;
	sint32 maxattack = 0;
	bool iscivilian = false;
	if ( goal_record->GetTargetTypeAttackUnit() ||
		goal_record->GetTargetTypeSpecialUnit() ) 
	{

		
		if (g_theWorld->GetCity(m_target_army->RetPos()).m_id != 0)
			return true;

		sint32 maxdefense; 
		bool cancapture;
		bool haszoc;
		bool canbombard;

		m_target_army->CharacterizeArmy(isspecial, 
			    isstealth,
			maxattack, 
			maxdefense, 
			cancapture,
			haszoc,
			canbombard);

		iscivilian = (m_target_army->IsCivilian()==TRUE);

		
		if (isspecial && !m_target_army->IsVisible(m_playerId))
			return true;

		
	}

	
	if ( goal_record->GetTargetTypeSpecialUnit() == true && 
		goal_record->GetTargetTypeAttackUnit() == false &&
		(maxattack > 0) )
	{
		return true;
	}

	
	if ( goal_record->GetTargetTypeUnexplored() )
	{
		Unit city = g_theWorld->GetCity(target_pos);
		CellUnitList army;
		g_theWorld->GetArmy(target_pos,army);
		if (army.Num() > 0 || city.m_id != 0x0)
			return true;
	}

	
	
	
	if ( m_playerId != 0 && target_owner > 0  && target_owner != m_playerId)
	{
		
		bool regard_checked = false;
		bool diplomacy_match = true;
		
		
		if ( player_ptr->HasContactWith(target_owner))
		{
			
			if (iscivilian &&
				goal_record->GetTargetOwnerHotEnemy() &&
				(diplomat.GetPersonality()->GetAlignmentGood() ||
				 diplomat.GetPersonality()->GetAlignmentNeutral()))
				 return true;

			if (goal_record->GetTargetOwnerNeutral())
			{
				
				diplomacy_match = 
					diplomat.TestEffectiveRegard(target_owner, NEUTRAL_REGARD);
				
				
				regard_checked = true;
			}
			
			
			if ( (!regard_checked || !diplomacy_match) &&
				goal_record->GetTargetOwnerColdEnemy() )
			{
				
				diplomacy_match = 
					diplomat.TestEffectiveRegard(target_owner, COLDWAR_REGARD);
				
				
				regard_checked = true;
			}
			
			
			if ( (!regard_checked || !diplomacy_match) &&
				goal_record->GetTargetOwnerHotEnemy() )
			{
				
				diplomacy_match = 
					diplomat.TestEffectiveRegard(target_owner, HOTWAR_REGARD);
				
				
				regard_checked = true;
			}
			
			
			if ( (!regard_checked || !diplomacy_match) &&
				goal_record->GetTargetOwnerAlly() )
			{
				
				diplomacy_match = 
					diplomat.TestEffectiveRegard(target_owner, ALLIED_REGARD);
				
				
				regard_checked = true;
			}
// If the goal is not executed by stealth units, forbid to execute it if their is no 
// incursion permission	(depending on alignement) - Calvitix
			if ((!diplomat.IncursionPermission(target_owner) &&
				(diplomat.GetPersonality()->GetAlignmentGood() ||
				 diplomat.GetPersonality()->GetAlignmentNeutral()))
				 && !goal_record->GetSquadClassStealth())
			{
                AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, 0,
                ("GOAL %x (%s) (%3d,%3d): Diplomacy match failed : No permission to enter territory\n", this, g_theGoalDB->Get(m_goal_type)->GetNameText(),target_pos.x,target_pos.y));
				return true;
			}


			if ( diplomacy_match == false )
			{
                          AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, 0,
                ("GOAL %x (%s) (%3d,%3d): Diplomacy match failed.\n", this, g_theGoalDB->Get(m_goal_type)->GetNameText(),target_pos.x,target_pos.y));
                
				return true;
			}
		}
		else if ( goal_record->GetTargetOwnerNoContact() == FALSE )
		{
			
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, 0,
            ("GOAL %x (%s) (%3d,%3d): Target owner not contacted.\n", this, g_theGoalDB->Get(m_goal_type)->GetNameText(),target_pos.x,target_pos.y));
            

			
			return true;
		}
	}

	
	const OrderRecord * order_record = goal_record->GetExecute();

	switch ( order_record->GetTargetPretest() ) {
	case k_Order_TargetPretest_EnemyArmy_Bit:
	case k_Order_TargetPretest_EnemySpecialUnit_Bit:
	case k_Order_TargetPretest_EnemySettler_Bit:
	case k_Order_TargetPretest_EnemyTradeUnit_Bit:
	case k_Order_TargetPretest_AttackPosition_Bit:
		if ((m_target_army.m_id != 0) && (m_target_army.GetOwner() == m_playerId)) 
		{
			
			Assert(0);
			return true;
		}
		break;
	case k_Order_TargetPretest_TradeRoute_Bit:
		
		Assert( m_target_city.m_id != 0);
		if ((m_target_city.m_id == 0) ||
			 (m_target_city.GetCityData()->GetTradeSourceList() == NULL) ||
			 (m_target_city.GetCityData()->GetTradeSourceList()->Num() <= 0))
			return true;
		break;
	case k_Order_TargetPretest_TerrainImprovement_Bit:
		
		if (goal_record->GetTargetTypeImprovement())
		{
			Assert( m_target_city.m_id != 0);
			if (m_target_city.m_id == 0 ||
				!m_target_city->GetCityData()->WasTerrainImprovementBuilt())
				return true;
		}
		else
		{
			
			if (g_theWorld->GetCell(target_pos)->GetNumDBImprovements() <= 0)
				return true;
		}
		break;
	};

	
	if (g_player[m_playerId]->GetGold() < order_record->GetGold())
	{
            AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, 0,
        ("GOAL %x (%s): Not enough gold to perform goal.\n", this, g_theGoalDB->Get(m_goal_type)->GetNameText()));
        
		return true;
	}

	
	if ( goal_record->GetAvoidWatchfulCity() )
	{
		Assert(m_target_city.m_id != 0);
		if ( m_target_city->IsWatchful() == TRUE )
			return true;
	}

	
	if ( goal_record->GetTargetTypeChokePoint() )
	{
		ArmyData army	= Army();
		
		g_theWorld->GetArmy(target_pos, army);
		if ( army.GetOwner() != m_playerId )
			return false;
		
		
		if ( Is_Satisfied() && ArmiesAtGoal() )
			return true;
	}

	
	if(order_record->GetUnitPretest_CanPlantNuke())
	{
		if (!g_player[m_playerId]->m_advances->HasAdvance(advanceutil_GetNukeAdvance()))
			return true;
	}

	
	if ( order_record->GetTargetPretestEnemySpecialUnit() == true)
	{
		if (g_theWorld->GetOwner(target_pos) != m_playerId)
			return true;
	}
	
	
	if ( order_record->GetTargetPretestTerrainImprovement() == false &&
		 order_record->GetTargetPretestTradeRoute() == false) 
	{
		
		if (ArmyData::TargetValidForOrder(order_record, target_pos) == false)	
			return true;
	}

	
	
	if ( order_record->GetTargetPretestEnemySpecialUnit() &&
		 m_target_army.m_id != 0 &&
		 m_target_army->CanBeExpelled() )
	{
		return true;
	}

	
	
	

	if (order_record->GetUnitPretest_CanReformCity())
	{
		
		if  (!m_target_city.GetCityData()->IsConverted())
			return true;
	}

	if (order_record->GetUnitPretest_CanSueFranchise())
	{
		
		if  (!m_target_city.GetCityData()->IsFranchised())
			return true;
	}

	if (order_record->GetUnitPretest_EstablishEmbassy())
	{
		
		if  (g_player[m_playerId]->HasEmbassyWith(m_target_city->GetOwner()))
			return true;
	}
	
	if (order_record->GetUnitPretest_CanCreateFranchise())
	{
		if (m_target_city.GetCityData()->GetFranchiseTurnsRemaining() <= 0)
				return true;
	}
	
	if (order_record->GetUnitPretest_CanBioTerror())
	{
		
		if (m_target_city.GetCityData()->IsBioInfected())
			return true;
	}
	
	if (order_record->GetUnitPretest_CanNanoInfect())
	{
			
			if (m_target_city.GetCityData()->IsNanoInfected())
				return true;
	}
	
	if (order_record->GetUnitPretest_CanConvertCity())
	{
			
			if (m_target_city.GetCityData()->GetConvertedTo() == m_playerId)
				return true;
	}
	
	if (order_record->GetUnitPretest_CanInterceptTrade())
	{
			
			Assert( m_target_city.m_id != 0);
			if ((m_target_city.m_id == 0) ||
				((m_target_city.GetCityData()->GetTradeSourceList()) &&
				(m_target_city.GetCityData()->GetTradeSourceList()->Num() <= 0)))
				return true;
	}

	
	if (order_record->GetUnitPretest_CanInciteRevolution())
	{
		sint32 cost;
		if (ArmyData::GetInciteRevolutionCost(target_pos, cost) &&
			( cost > g_player[m_playerId]->GetGold()))
			return true;
	}

	
	if (order_record->GetUnitPretest_CanNukeCity())
	{
		if (diplomat.GetNuclearLaunchTarget() == target_owner)
			return true;
	}


	//Try to Steal Technology only if the other civ has more advances than player
	//Otherwise, spy can do anything else
	if (order_record->GetUnitPretest_CanStealTechnology())
	{
		if (g_player[m_playerId]->NumAdvances() > g_player[target_owner]->NumAdvances())
			return true;
	}

	//Abolisionist has to go to cities with slaves
	if(order_record->GetUnitPretest_CanInciteUprising()
	|| order_record->GetUnitPretest_CanUndergroundRailway ()
	){
		if (m_target_city.GetCityData()->SlaveCount() == 0)
			return true;
	}

	if(order_record->GetUnitPretest_NoFuelThenCrash()){

		if(g_theUnitPool->IsValid(m_target_city)
		&& target_owner != m_playerId
		){
			return true;
		}
		else if(g_theArmyPool->IsValid(m_target_army)
		     && target_owner != m_playerId
		     ){
			return true;
		}
		else if(!g_theWorld->IsAirfield(target_pos)){
			return true;
		}
	}
	
	return false;
}


bool CTPGoal::Get_Invalid() const
{
	const GoalRecord *goal_record = g_theGoalDB->Get(m_goal_type);

	if(goal_record->GetTargetTypeAttackUnit()
	|| goal_record->GetTargetTypeSpecialUnit()
	){
		if(g_theArmyPool->IsValid(m_target_army) == FALSE)
			return true;
		else if(m_target_army->Num() == 0)
			return true;
		else
			return false;
	}
	
	
	if(goal_record->GetTargetTypeCity()
	|| goal_record->GetTargetTypeTradeRoute()
	|| goal_record->GetTargetTypeImprovement()
	){
		if(g_theUnitPool->IsValid(m_target_city) == FALSE)
			return true;

		CityData *city = m_target_city->GetCityData();
		if(city == NULL)
		{
			Assert(0);
			return true;
		}

		
		if(goal_record->GetTargetOwnerSelf()
		&& city->GetOwner() != m_playerId)
			return true;
		else if(!goal_record->GetTargetOwnerSelf()
		     &&  city->GetOwner() == m_playerId)
			return true;
	}

	
	if(goal_record->GetTargetTypeEndgame())
	{
		return(terrainutil_HasEndgame( Get_Target_Pos() ) == false);
	}

	if (goal_record->GetTargetTypeUnexplored() )
		return(g_player[m_playerId]->IsExplored(Get_Target_Pos()) == TRUE);
	
	
	if(goal_record->GetTargetTypeSettleLand()
	|| goal_record->GetTargetTypeSettleSea()
	){
		if(SettleMap::s_settleMap.CanSettlePos(Get_Target_Pos()) == false)
			return true;

		if(g_theWorld->HasCity( Get_Target_Pos()))
			return true;
	}

	if(goal_record->GetTargetTypeGoodyHut())
		return(g_theWorld->GetGoodyHut(Get_Target_Pos()) == NULL);

	// Check whether the target can refuel the given army
	if(goal_record->GetTargetTypePetrolStation()){

		if(g_theUnitPool->IsValid(m_target_city)){

			CityData *city = m_target_city->GetCityData();
			if(city == NULL){
				Assert(0);
				return true;
			}

			if(goal_record->GetTargetOwnerSelf()
			&& city->GetOwner() != m_playerId)
				return true;
			else if(!goal_record->GetTargetOwnerSelf()
				 &&  city->GetOwner() == m_playerId)
				return true;
		}
//		else if(g_theArmyPool->IsValid(m_target_army)){ // Aircraft caries are missing
//			return Cannot carry aircrafts;
//		}
		return !g_theWorld->IsAirfield(Get_Target_Pos());
	}

	return false;
}


bool CTPGoal::Get_Removal_Time() const
{ 
	if ( Get_Invalid() ||
		 (g_theGoalDB->Get(m_goal_type)->GetRemoveWhenComplete() && 
		  Get_Totally_Complete() ) )
		 return true;
	return false;
}



bool CTPGoal::Pretest_Bid(const Agent_ptr agent_ptr) const
{
	return Pretest_Bid(agent_ptr, MapPoint(-1,-1));
}


bool CTPGoal::Pretest_Bid(const Agent_ptr agent_ptr, const MapPoint & cache_pos) const
{
    
	CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr) agent_ptr;
	const Army & army = ctpagent_ptr->Get_Army();
	
	
	sint32 transports;
	sint32 needed_transports;
	sint32 max_cargo_slots;
	sint32 empty_cargo_slots;
	MapPoint target_pos;
	
	if (cache_pos == MapPoint(-1,-1))
		target_pos = Get_Target_Pos(army);
	else
		target_pos = cache_pos;

	const GoalRecord *goal_rec = g_theGoalDB->Get(m_goal_type);

	
	const OrderRecord *order_rec = goal_rec->GetExecute();

	
	
	if (army->GetMinFuel() != 0x7fffffff)
	{
		sint32 num_tiles_to_half;
		sint32 num_tiles_to_empty;
		army->CalcRemainingFuel(num_tiles_to_half, num_tiles_to_empty);

		
		num_tiles_to_empty = static_cast<sint32>(num_tiles_to_empty / k_MOVE_AIR_COST);
		num_tiles_to_half = static_cast<sint32>(num_tiles_to_half / k_MOVE_AIR_COST);

		sint32 distance_to_refuel;
		sint32 distance_to_target;
		MapPoint refuel_pos;
		CtpAi::GetNearestRefuel(army, target_pos, refuel_pos, distance_to_refuel);
		
		distance_to_target = 
		    static_cast<sint32>(sqrt(static_cast<double>
		    (MapPoint::GetSquaredDistance(army->RetPos(), target_pos))
		    ));

		if (num_tiles_to_empty < distance_to_target + distance_to_refuel)
			return false;
	}

	
	if	(goal_rec->GetSquadClassCanBombard())
		{
			static CellUnitList defenders;
			defenders.Clear();
			g_theWorld->GetArmy(target_pos, defenders);
			if (ctpagent_ptr->Get_Army()->CanBombardTargetType(defenders) == FALSE)
				return false;
		}

	
	army->GetCargo(transports, max_cargo_slots, empty_cargo_slots);
	needed_transports = (m_current_needed_strength.Get_Transport() - 
						 m_current_attacking_strength.Get_Transport());


	
	
	if (transports > 0 )
	{ 
		
		if ((empty_cargo_slots < max_cargo_slots) || (army->HasCargo()))
		{
			
			ORDER_TEST cargo_test = army->CargoTestOrderHere(order_rec, target_pos);
			if ( cargo_test	== ORDER_TEST_ILLEGAL ||
				 cargo_test == ORDER_TEST_INVALID_TARGET )
				return false;
			else
				return true;
		}
		
		else if (needed_transports > 0)
			return true;
	}

	
	
	bool needs_land_unit = (goal_rec->GetTargetTypeCity() && 
		goal_rec->GetTargetOwnerSelf() );
	needs_land_unit |= goal_rec->GetTargetTypeGoodyHut();
	
	uint32 movement_type = army->GetMovementType();
	if (army->HasCargo())
		movement_type |= army->GetCargoMovementType();
	bool land_movement = ((movement_type & 
		   (k_Unit_MovementType_Land_Bit | k_Unit_MovementType_Mountain_Bit)) != 0x0);
	land_movement &= ((movement_type & k_Unit_MovementType_Air_Bit) == 0x0);

	if (needs_land_unit && !land_movement)
	{
		return false;
	}

	
	if ( army->CanNukeCity() && 
		 (g_theWorld->GetCell(target_pos)->GetOwner() != m_playerId ||
		  g_theWorld->GetCity(target_pos).m_id != 0x0) )
		return false;

	
	if ( army->TestOrder(order_rec) == ORDER_TEST_ILLEGAL )
		return false;

	
	
	if ( !army->CanEnter(target_pos) )
		return false;

    return true;
}


void CTPGoal::Log_Debug_Info(const int &log) const
{
#ifdef _DEBUG
    
    int row = 0;
	int column = 0;

	





	const char *name = g_theGoalDB->Get(m_goal_type)->GetNameText();

    Agent_List::const_iterator agent_iter;
	CTPAgent_ptr stagent_ptr;
	bool log_army = true;
	bool log_goal = CtpAiDebug::DebugLogCheck(m_playerId, m_goal_type, -1);
	MapPoint pos = Get_Target_Pos();
	for (agent_iter = m_agents.begin(); agent_iter != m_agents.end(); agent_iter++) {
		    log_army = false;
			stagent_ptr = (CTPAgent_ptr) *agent_iter;
			if (CtpAiDebug::DebugLogCheck(m_playerId, m_goal_type, stagent_ptr->Get_Army().m_id))
			{
				log_army = true;
				
				
				pos = Get_Target_Pos(stagent_ptr->Get_Army());
				break;
			}
    } 

	
	if (log_army == false || log_goal == false)
	{
		

		return;
	}

	if (m_raw_priority > BAD_UTILITY)
	{
		DPRINTF(log, 
        ("\t %9x,\t%s,\t%8d,\t(%3d,%3d)\n",
			this,
        name,
			m_raw_priority, 
	        pos.x,
        pos.y));
	}
	else 
	{
		MapPoint pos = Get_Target_Pos();
        DPRINTF(log, ("\t %9x,\t%s,\tBAD_UTILITY,\t(%d,%d)\n",
			this,
			name,
			pos.x,
        pos.y));
	}
	if (m_agents.size() > 0)
		DPRINTF(log,("\t\t\tCommitted Agents:\n"));

	
	for (agent_iter = m_agents.begin();
		 agent_iter != m_agents.end();
		 agent_iter++) {
			
			stagent_ptr = (CTPAgent_ptr) *agent_iter;
            stagent_ptr->Log_Debug_Info(log);
    } 
#endif // _DEBUG
}


bool CTPGoal::ReferencesAgent(const CTPAgent *ctp_agent) const
{
#ifdef _DEBUG
    Agent_List::const_iterator agent_iter;
	CTPAgent_ptr tmp_ctp_agent;

	
	for (agent_iter = m_agents.begin(); agent_iter != m_agents.end(); agent_iter++) 
	{
		
		tmp_ctp_agent = (CTPAgent_ptr) *agent_iter;
		if (ctp_agent == tmp_ctp_agent)
			return true;
    } 
#endif // _DEBUG

	return false;
}







bool CTPGoal::NeededForGarrison(CTPAgent_ptr army, 
								const MapPoint &dest_pos,
								sint8 & new_garrison,
								double & new_garrison_strength) const
{
	Unit city = g_theWorld->GetCity( army->Get_Pos() );

	
	if (city.m_id == 0)
		return false;

	
	if (( army->Get_Army()->GetMovementType() & 
		  (k_Unit_MovementType_Land_Bit | k_Unit_MovementType_Mountain_Bit)) == 0)
	{
		return false;
	}


	
	new_garrison = city->GetCityData()->GetCurrentGarrison();
	new_garrison_strength = city->GetCityData()->GetCurrentGarrisonStrength();

	
	
	if ( dest_pos == army->Get_Pos() )
		return false;

	sint32 hp;
	sint32 defense_count;
	sint32 ranged_count;
	sint32 attack_strength;
	sint32 defense_strength;
	sint32 ranged_strength;
	sint32 total_value;
	
	
	if (((army->Get_Squad_Class() & k_Goal_SquadClass_CanDefend_Bit) != 0x0 ) &&
		((army->Get_Squad_Class() & k_Goal_SquadClass_Special_Bit) == 0x0 ) &&
		((army->Get_Squad_Class() & k_Goal_SquadClass_CanTransport_Bit) == 0x0 ) )
	{
		army->Get_Army()->GetArmyStrength(hp,
			defense_count,
			ranged_count,
			attack_strength,
			defense_strength,
			ranged_strength,
			total_value);
		
		
		defense_strength += static_cast<sint32>(city->GetCityData()->GetDefendersBonus() * defense_count);
		
		
		new_garrison -= static_cast<sint8>(defense_count);
		new_garrison_strength -= static_cast<double>(defense_strength);
		
		
		
		if ( new_garrison >= city->GetCityData()->GetNeededGarrison() &&
			new_garrison_strength >= city->GetCityData()->GetNeededGarrisonStrength() )
		{
			return false;
		}
		return true;
	}
	return false;
}

bool CTPGoal::FollowPathToTask( CTPAgent_ptr first_army, 
							    CTPAgent_ptr second_army, 
							    const SUB_TASK_TYPE sub_task, 
							    const MapPoint &dest_pos,
								const Path &found_path)
{ 
    bool did_move = false; 

	
	Unit city = g_theWorld->GetCity( first_army->Get_Pos() );
	if (city.m_id != 0)
	{
		sint8 new_garrison;
		double new_garrison_strength;
		if (NeededForGarrison(first_army, dest_pos, new_garrison, new_garrison_strength))
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, first_army->Get_Army().m_id,
            ("GOAL %x (%d): FollowPathToTask::Can not send army needed for garrison to destination (x=%d,y=%d):\n", this,
            m_goal_type, dest_pos.x, dest_pos.y));
			first_army->Log_Debug_Info(k_DBG_SCHEDULER);
            uint8 magnitude = 220;
            g_graphicsOptions->AddTextToArmy(first_army->Get_Army(), "GARRISON", magnitude);
			return false;
		}
		else 
		{
			
			city->GetCityData()->SetCurrentGarrison(new_garrison);
			city->GetCityData()->SetCurrentGarrisonStrength(new_garrison_strength);
		}
	}

	
	const GoalRecord *goal_rec = g_theGoalDB->Get(m_goal_type);
	const OrderRecord *order_rec;
	ORDER_TEST test = ORDER_TEST_OK;
	
	if (m_sub_task == SUB_TASK_GOAL)
	{
		order_rec = goal_rec->GetExecute();
		if (first_army->Get_Army()->HasCargo() )
		{
			test = first_army->Get_Army()->CargoTestOrderHere(order_rec, dest_pos );

			
			if (first_army->Get_Army()->GetMovementTypeAir())
			{
				order_rec = CtpAi::GetUnloadOrder();
			}
			
			if (first_army->Get_Pos() == dest_pos &&
				city.m_id != 0x0 )
			{
				order_rec = CtpAi::GetUnloadOrder();
			}
		}
		else
		{
			test = first_army->Get_Army()->TestOrderHere(order_rec, dest_pos );
		}
	}
	else
	{
		
		order_rec = CtpAi::GetMoveOrder();
	}

	
	if ( test == ORDER_TEST_OK || 
		 test == ORDER_TEST_NO_MOVEMENT )
	{

    //I want to see armytext even in optimized test version - Calvitix
	Utility val = Compute_Matching_Value(first_army);
	uint8 magnitude = (uint8) (((5000000 - val)* 255.0) / 5000000);
	g_graphicsOptions->AddTextToArmy(first_army->Get_Army(), goal_rec->GetNameText(), magnitude);
    const MapPoint & my_target_loc = dest_pos;
    const char * myText = goal_rec->GetNameText();
    MBCHAR * myString = new MBCHAR[strlen(myText) + 40];
    MBCHAR * goalString = new MBCHAR[strlen(myText) + 20];
    memset(goalString, 0, strlen(myText) + 20);
    memset(myString, 0, strlen(myText) + 40);
    for (uint8 myComp = 0; myComp < strlen(myText) - 5; myComp++)
    {
        goalString[myComp] = myText[myComp + 5];
    }
    switch (m_sub_task)
    {

	
        case SUB_TASK_RALLY:
            sprintf(myString, "Group to %s (%d,%d)", goalString, my_target_loc.x, my_target_loc.y);
            break;
        case SUB_TASK_TRANSPORT_TO_BOARD:
            sprintf(myString, "Transp. to %s (%d,%d)", goalString, my_target_loc.x, my_target_loc.y);
            break;
        case SUB_TASK_CARGO_TO_BOARD:
            sprintf(myString, "Cargo. to %s (%d,%d)", goalString, my_target_loc.x, my_target_loc.y);
            break;
        case SUB_TASK_AIRLIFT:
            sprintf(myString, "Airlift to %s (%d,%d)", goalString, my_target_loc.x, my_target_loc.y);
            break;
		case SUB_TASK_GOAL:
		default :
            sprintf(myString, "%s (%d,%d)", goalString, my_target_loc.x, my_target_loc.y);
            break;
    }
    g_graphicsOptions->AddTextToArmy(first_army->Get_Army(), myString, magnitude);
    delete[] myString;
    delete[] goalString;

		if (first_army->Get_Can_Be_Executed() == true)
		{
			Assert(order_rec);
			if (order_rec)
				first_army->Get_Army()->PerformOrderHere(order_rec, (Path *) &found_path);
		}
		else 
		{
			
		}

		return true;
	}
	else
    {

		g_graphicsOptions->AddTextToArmy(first_army->Get_Army(), "NULL", 0);

	if ( test != ORDER_TEST_OK && test == ORDER_TEST_NO_MOVEMENT )
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, first_army->Get_Army().m_id,
            ("GOAL %x (%d): FollowPathToTask:: failed TestOrderHere( %s, (%d,%d))\n", this, m_goal_type,
				order_rec->GetNameText(),dest_pos.x,dest_pos.y));
		}

		
		return false;
	}


	return true;
}



bool CTPGoal::GotoTransportTaskSolution(CTPAgent_ptr the_army, CTPAgent_ptr the_transport, const SUB_TASK_TYPE & sub_task)
{
	MapPoint dest_pos;
	Path found_path;
	bool check_dest;
	uint32 move_intersection;
	
	bool transport_at_rendezvous;
	MapPoint next_pos;
	MapPoint start_pos;
	bool found = false;
	BOOL airfield_found;
	BOOL city_found;
	MapPoint nearest_airfield;
	Unit nearest_city;
	double city_distance = 0.0;
	sint32 cargo_cont;

	switch (sub_task)
	{

	case SUB_TASK_AIRLIFT:
	{	
		
		
		start_pos = the_army->Get_Pos(); 

		cargo_cont = g_theWorld->GetContinent(start_pos);
		
		
		airfield_found = g_player[m_playerId]->
			GetNearestAirfield(start_pos, nearest_airfield, cargo_cont);
		double airfield_distance = 0.0;
		city_found = g_player[m_playerId]->
			GetNearestCity(start_pos, nearest_city, city_distance, FALSE);
		if (airfield_found)
		{
			airfield_distance = MapPoint::GetSquaredDistance(start_pos, nearest_airfield);
		}

		if (airfield_found && city_found && (airfield_distance < city_distance))
			dest_pos = nearest_airfield;
		else if (city_found)
		{
			dest_pos = nearest_city.RetPos();
		}
		else
		{
			
			return false;
		}

		
		if (dest_pos == start_pos &&
			dest_pos == the_transport->Get_Army()->RetPos())
		{
			
			the_army->MoveIntoTransport();

			
			the_transport->Set_Can_Be_Executed(false);

			return true;
		}

		check_dest = true;
		
		
		
		
		
		found = CTPAgent::FindPath(the_army->Get_Army(), dest_pos, check_dest, found_path); 

		
		if (found && FollowPathToTask(the_army, the_transport, sub_task, dest_pos, found_path) )
		{
			
			move_intersection = the_transport->Get_Army().GetMovementType();

			
			found = CTPAgent::FindPath(the_transport->Get_Army(), dest_pos, check_dest, found_path); 

			
			if (found && FollowPathToTask(the_transport, the_army, sub_task, dest_pos, found_path) )
				return true;
		}
	
		break;
	}							
	case SUB_TASK_TRANSPORT_TO_BOARD:
		
		check_dest = false;
        dest_pos = the_army->Get_Pos(); 

		
		if (the_transport->Get_Army()->CheckValidDestination(dest_pos))
			return true;

		
		
        move_intersection = 
			the_transport->Get_Army().GetMovementType() | the_army->Get_Army().GetMovementType();
		
        
        found = the_transport->FindPathToBoard(move_intersection, dest_pos, check_dest, found_path);

		if (!found)
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, the_transport->Get_Army().m_id,
                    ("GOAL %x (%d):GotoTransportTaskSolution:: No path found from army to destination (x=%d,y=%d) (SUB_TASK_TRANSPORT_TO_BOARD):",
                    this, m_goal_type, dest_pos.x, dest_pos.y));
			the_transport->Log_Debug_Info(k_DBG_SCHEDULER);
                    uint8 magnitude = 220;
                    MBCHAR * myString = new MBCHAR[40];
                    sprintf(myString, "NO PATH -> BOARD (%d,%d)", dest_pos.x, dest_pos.y);
                    g_graphicsOptions->AddTextToArmy(the_army->Get_Army(), myString, magnitude);
                    delete[] myString;
		}
		
		
		else 
		{
			found_path.SnipEndUntilCanEnter(the_transport->Get_Army()->GetMovementType());

			
			if ( (found_path.GetMovesRemaining() > 0) &&
				 !FollowPathToTask(the_transport, the_army, sub_task, dest_pos, found_path) )
				return false;
			
			return true;
		}

		break;
		
    case SUB_TASK_CARGO_TO_BOARD:
		
        dest_pos = the_transport->Get_Pos();
		start_pos = the_army->Get_Pos();
		
		
		

		
		if (dest_pos == start_pos)
		{
			the_army->MoveIntoTransport();

			return true;
		}
		
		if (! the_transport->Get_Army()->AtEndOfPath() )
		{
			
			transport_at_rendezvous = the_transport->Get_Army()->GetNextPathPoint(next_pos);
			
			
			
			transport_at_rendezvous &= 
				( the_transport->Get_Army()->CanEnter(next_pos) == false);
		}
		else
		{
			
			transport_at_rendezvous = true;
			
		}
		
		
		if (transport_at_rendezvous )
			
		{
			
			move_intersection = the_army->Get_Army().GetMovementType();
			
			check_dest = true;
			
			
			found =  the_army->FindPathToBoard(move_intersection, dest_pos, check_dest, found_path);
		}
		else
		{
			
			check_dest =  false; 

			
			found = CTPAgent::FindPath(the_army->Get_Army(), dest_pos, check_dest, found_path); 

			if (!found)
			{
				
				move_intersection = the_transport->Get_Army().GetMovementType() |
					the_army->Get_Army().GetMovementType(); 
			
				
				found =  the_army->FindPathToBoard(move_intersection, dest_pos, check_dest, found_path);
			}
		}

		if (!found)
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, the_army->Get_Army().m_id,
                    ("GOAL %x (%d): GotoTransportTaskSolution: No path found from army to destination (x=%d,y=%d) (SUB_TASK_CARGO_TO_BOARD):",
                    this, m_goal_type, dest_pos.x, dest_pos.y));
			the_army->Log_Debug_Info(k_DBG_SCHEDULER);
                    uint8 magnitude = 220;
                    MBCHAR * myString = new MBCHAR[40];
                    sprintf(myString, "NO PATH -> BOARD (%d,%d)", dest_pos.x, dest_pos.y);
                    g_graphicsOptions->AddTextToArmy(the_army->Get_Army(), myString, magnitude);
                    delete[] myString;
		}

		if (found && FollowPathToTask(the_army, the_transport, sub_task, dest_pos, found_path) )
			return true;

		break;
		
    default:
		
        Assert(false); 
	}

	return false;
}


bool CTPGoal::GotoGoalTaskSolution(CTPAgent_ptr the_army, const MapPoint & goal_pos, const SUB_TASK_TYPE & sub_task)
{

	const OrderRecord * unload_order_rec = CtpAi::GetUnloadOrder();
	const OrderRecord * space_launch_order_rec = CtpAi::GetSpaceLaunchOrder();

	
	if (the_army->Get_Army()->CheckValidDestination(goal_pos))
		return true; 

	Path found_path;

    MapPoint start_pos; 
	MapPoint dest_pos;
	sint32 dest_cont; 
	BOOL dest_is_land;
	sint32 army_cont;
	sint32 army_is_land;
	uint32 move_intersection;
	BOOL city_found;
	Unit nearest_city;
	sint32 target_cont;
	double city_distance;

	sint32 range = 0;
	(void) g_theGoalDB->Get(m_goal_type)->GetExecute()->GetRange(range);
	
	
	bool check_dest;
	const GoalRecord * goal_rec = g_theGoalDB->Get(m_goal_type);
	if (range > 0  || 
		goal_rec->GetExecute()->GetTargetPretestAttackPosition() ||
		(goal_rec->GetTargetTypeCity() && goal_rec->GetTargetOwnerSelf()))
		check_dest = false; 
	else
		check_dest = true;

    bool found = false;

	start_pos = the_army->Get_Pos();

	
	
	bool ok_to_rally = Ok_To_Rally();
	dest_cont = g_theWorld->GetContinent(goal_pos);
	dest_is_land = g_theWorld->IsLand(goal_pos);
	army_cont = g_theWorld->GetContinent(the_army->Get_Pos());
	army_is_land = g_theWorld->IsLand(the_army->Get_Pos());

	
	
	bool waiting_for_buddies = (!ok_to_rally && (sub_task == SUB_TASK_RALLY) && 
		(army_cont == dest_cont) && dest_is_land && army_is_land);
	
	if ( the_army->Get_Army()->HasCargo() && 
		the_army->Get_Army()->GetMovementTypeAir() &&
		the_army->Get_Army()->CanSpaceLaunch())
	{
		
		
		target_cont = g_theWorld->GetContinent(goal_pos);
		
		
		city_found = g_player[m_playerId]->
			GetNearestCity(goal_pos, nearest_city, city_distance, FALSE, target_cont);
		
		if (city_found)
		{

			found = CTPAgent::FindPath(the_army->Get_Army(), nearest_city.RetPos(), true, found_path); 
            if (found) Set_Sub_Task(SUB_TASK_AIRLIFT);
			
			if (start_pos == nearest_city.RetPos())
			{
				
				the_army->Get_Army()->
					PerformOrderHere(unload_order_rec, (Path *) &found_path);
			}
			else
			{
				
				if (the_army->Get_Army()->HasLeftMap() == FALSE)
				{
					the_army->Get_Army()->
						PerformOrderHere(space_launch_order_rec, (Path *) &found_path);
				}
			}
			
			
			the_army->Set_Can_Be_Executed(false);
			return true;
		}
	}
	else if ( the_army->Get_Army()->HasCargo() && 
		!the_army->Get_Army()->GetMovementTypeAir())
	{
		
		move_intersection = 
			the_army->Get_Army()->GetMovementType() | the_army->Get_Army()->GetCargoMovementType();
		
		
		found = the_army->FindPathToBoard(move_intersection, goal_pos, false, found_path);
            if (found) Set_Sub_Task(SUB_TASK_TRANSPORT_TO_BOARD);
	}
	else
	{
		
		if (!waiting_for_buddies)
		{
			
			found = CTPAgent::FindPath(the_army->Get_Army(),goal_pos, check_dest, found_path); 
		}
	}

    switch (sub_task) { 
    case SUB_TASK_GOAL: 
		if (!found)
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, the_army->Get_Army().m_id,
                    ("GOAL %x (%d): GotoGoalTaskSolution: No path found from army to goal (x=%d,y=%d) (SUB_TASK_GOAL):",
                    this, m_goal_type, goal_pos.x, goal_pos.y));
			the_army->Log_Debug_Info(k_DBG_SCHEDULER);
                    uint8 magnitude = 220;
                    MBCHAR * myString = new MBCHAR[40];
                    sprintf(myString, "NO PATH to (%d,%d) - %d", goal_pos.x, goal_pos.y,m_goal_type);
                    g_graphicsOptions->AddTextToArmy(the_army->Get_Army(), myString, magnitude);
                    delete[] myString;

		}

		break;

	case SUB_TASK_RALLY: 
		
		

		
		if (waiting_for_buddies)
		{
                    Utility val = Compute_Matching_Value(the_army);
                    uint8 magnitude = (uint8)(((5000000 - val) * 255.0) / 5000000);
                    MBCHAR * myString = new MBCHAR[40];
                    sprintf(myString, "Waiting GROUP to GO (%d,%d)", goal_pos.x, goal_pos.y);
                    g_graphicsOptions->AddTextToArmy(the_army->Get_Army(), myString, magnitude);
                    delete[] myString;
			return true;
		}

		
		if (!found)
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, the_army->Get_Army().m_id,
                    ("GOAL %x (%d):GotoGoalTaskSolution: No path found from army to goal (x=%d,y=%d) (SUB_TASK_RALLY):",
                    this, m_goal_type, goal_pos.x, goal_pos.y));
			the_army->Log_Debug_Info(k_DBG_SCHEDULER);
                    uint8 magnitude = 220;
                    g_graphicsOptions->AddTextToArmy(the_army->Get_Army(), "NO PATH (GROUP)", magnitude);
		}

		break;

    default:
                {
                    if (!found)
                    {
                        AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, the_army->Get_Army().m_id,
                        ("GOAL %x (%d):GotoGoalTaskSolution: No path found from army to goal (x=%d,y=%d) (SUB_TASK_TRANSPORT:",
                        this, m_goal_type, goal_pos.x, goal_pos.y));
                        the_army->Log_Debug_Info(k_DBG_SCHEDULER);
                        uint8 magnitude = 220;
                        g_graphicsOptions->AddTextToArmy(the_army->Get_Army(), "NO PATH (TRANSP.)", magnitude);
                    }
                }
    } 

	bool move_success = false;
	if ( found )
	{
		move_success = FollowPathToTask(the_army, NULL, sub_task, goal_pos, found_path);
	}
	else 
	{
		move_success = TryTransport(the_army, goal_pos);
	}

	
	if (move_success)
	{
		
		the_army->Set_Can_Be_Executed(false);
	}

	return move_success;
}


bool CTPGoal::Ok_To_Rally() const
{
	if (m_agents.size() == 1)
		return false;

	if (!g_theWorld->IsLand(Get_Target_Pos()))
		return true;

	sint32 const    dest_cont       = g_theWorld->GetContinent(Get_Target_Pos());

	sint32          num_in_water    = 0;
	sint32          num_at_dest     = 0;
	sint32          count           = 0;
	
	for 
    (
        Agent_List::const_iterator  agent_iter = m_agents.begin(); 
        agent_iter != m_agents.end();
        ++agent_iter
    ) 
    {
		const CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr) *agent_iter;
		Assert(ctpagent_ptr);
		
		if (!ctpagent_ptr->Get_Is_Dead())
		{
	        MapPoint const  army_pos        = ctpagent_ptr->Get_Pos();
			
			if (g_theWorld->IsLand(army_pos))
            {
                if (dest_cont == g_theWorld->GetContinent(army_pos))
                {
                    ++num_at_dest;
                }
                else
                {
                    return false;
                }
            }
            else
            {
				++num_in_water;
            }
			
			++count;
		}
	} 
	
	return (num_in_water + num_at_dest) == count;
}


bool CTPGoal::RallyComplete() const
{
	if (m_agents.size() == 1) 
		return true;

	bool incompleteStackFound   = false;

	for 
    (
        Agent_List::const_iterator agent_iter = m_agents.begin(); 
        agent_iter != m_agents.end(); 
        ++agent_iter
    )
	{
		
	    CTPAgent_ptr ctpagent_ptr   = (CTPAgent_ptr) *agent_iter;
        Assert(ctpagent_ptr);
		
		if (ctpagent_ptr->Get_Is_Dead())
			continue;

		
	    MapPoint const  army_pos = ctpagent_ptr->Get_Pos();

		if (!ctpagent_ptr->IsArmyPosFilled())
		{
			if (incompleteStackFound)
            {
				return false;
            }
            
			incompleteStackFound = true;
		}
		
	    sint8 adjacent_count        = 0;
	    sint8 total_count           = 0;

		for 
        (	
            Agent_List::const_iterator adjacent_agent_iter = m_agents.begin(); 
			adjacent_agent_iter != m_agents.end() && adjacent_count < 3;
			++adjacent_agent_iter
        )
		{ 
			CTPAgent_ptr ctpadjacent_ptr = (CTPAgent_ptr) *adjacent_agent_iter;
            Assert(ctpadjacent_ptr);

			if ( ctpadjacent_ptr->Get_Is_Dead())
				continue;
			
	        MapPoint const  adjacent_pos = ctpagent_ptr->Get_Pos();
			
			if (army_pos.IsNextTo(adjacent_pos))
            {
				++adjacent_count;
            }

			++total_count;
		}
		
		
		
		if ((adjacent_count < total_count - 1) &&
			(adjacent_count < 3 ) 
           )
        {
			 return false;
        }
	}
	
	
	return true;
}



bool CTPGoal::RallyTroops() 
{
	bool agent1_is_partial;
	bool agent2_is_partial;
	bool partiality_found = false;
	
    Set_Sub_Task(SUB_TASK_RALLY);
	Agent_List tmp_agents   = m_agents;
	
	CTPAgent_ptr ctpagent2_ptr;
	CTPAgent_ptr closest_agent_ptr;
	
	
	
	Agent_List::iterator agent2_iter;
	Agent_List::iterator closest_agent_iter;
	Agent_List::iterator agent1_iter = tmp_agents.begin(); 
	while (agent1_iter != tmp_agents.end()) 
	{
	    CTPAgent_ptr ctpagent1_ptr = (CTPAgent_ptr) *agent1_iter;
        Assert(ctpagent1_ptr);
		
		if (ctpagent1_ptr->Get_Is_Dead())
		{
			++agent1_iter;
			continue;
		}
		
		
		agent1_is_partial = (ctpagent1_ptr->Get_Army().Num() < k_MAX_ARMY_SIZE );
		
		
		sint32 min_distance = (g_mp_size.x + g_mp_size.y);
		min_distance *= min_distance;
		closest_agent_iter = tmp_agents.end();
		partiality_found = false;
		
		
		for (agent2_iter = tmp_agents.begin(); agent2_iter != tmp_agents.end(); agent2_iter++) 
		{
			
			ctpagent2_ptr = (CTPAgent_ptr) *agent2_iter;
			
			
			if (ctpagent1_ptr == ctpagent2_ptr)
				continue;

			
			if (ctpagent2_ptr->Get_Is_Dead())
				continue;
			
			
			
			agent2_is_partial = (ctpagent2_ptr->Get_Army().Num() < k_MAX_ARMY_SIZE);
			if ( (partiality_found) &&
				(agent2_is_partial != agent1_is_partial) )
				continue;
			
			
			sint32 const distance = 
                MapPoint::GetSquaredDistance(ctpagent1_ptr->Get_Pos(), ctpagent2_ptr->Get_Pos());

			if (distance < min_distance)
			{
				min_distance        = distance;
				closest_agent_iter  = agent2_iter;
			}
			
			
			if ( agent1_is_partial == agent2_is_partial && 
				partiality_found == false ) 
			{
				partiality_found = true;
				min_distance = distance;
				closest_agent_iter = agent2_iter;
			}
		} 
		
		if (min_distance < 1)
		{
			
			
			ctpagent1_ptr->Group_With((CTPAgent_ptr) *closest_agent_iter);
		}
		else if ( closest_agent_iter != tmp_agents.end() )
		{
			MapPoint closest_agent_pos;
			
			closest_agent_ptr = (CTPAgent_ptr) *closest_agent_iter;
			
			closest_agent_pos = closest_agent_ptr->Get_Pos();

// To avoid Groups to be blocked when an unit is in a city 
// (problem with garrison -> not enough room)
			sint32 cells;
			if(!g_theWorld->GetCity(closest_agent_pos).IsValid() 
			||  ctpagent1_ptr->GetRounds(closest_agent_pos, cells) > 2
			){
       			if (GotoGoalTaskSolution(ctpagent1_ptr, closest_agent_pos, SUB_TASK_RALLY) == false)
	       			return false;
			}
			else
			{
                    uint8 magnitude = 220;
                    MBCHAR * myString = new MBCHAR[40];
					MapPoint goal_pos;
					goal_pos = Get_Target_Pos(ctpagent1_ptr->Get_Army());
                    sprintf(myString, "Waiting GROUP to GO (%d,%d)", goal_pos.x, goal_pos.y);
				    g_graphicsOptions->AddTextToArmy(ctpagent1_ptr->Get_Army(), myString, magnitude);
                    delete[] myString;	
			}
				MapPoint agent1_pos;
				agent1_pos = ctpagent1_ptr->Get_Pos();
			if ( g_theWorld->GetCity(closest_agent_pos).IsValid() || closest_agent_ptr->GetRounds(agent1_pos, cells) > 2)
			{
				
				if (g_theWorld->GetCity(agent1_pos).IsValid() && g_theWorld->GetCity(closest_agent_pos).IsValid()) //two units are in another town
				{
					MapPoint tempPos;
					sint32 i;
     				sint32 result;
					for (i = 0 ; i < SOUTH ; i++)
					{
						result = closest_agent_pos.GetNeighborPosition(WORLD_DIRECTION(i),tempPos);
					    if(result)
						{
						   CellUnitList *the_army=NULL;
						   the_army = g_theWorld->GetArmyPtr(tempPos);
						   if (!the_army)
						   {  //search for cell without army
								agent1_pos = tempPos;
								break;
						   }						   
						}
					}
				}

				if (GotoGoalTaskSolution(closest_agent_ptr, agent1_pos, SUB_TASK_RALLY ) == false)
				return false;

			}
			else
			{
				CellUnitList *the_army=NULL;
				the_army = g_theWorld->GetArmyPtr(closest_agent_pos);
				if (static_cast<uint32>(the_army->Num()) >= m_agents.size() && m_agents.size() > k_MAX_ARMY_SIZE/2)
				{
					MapPoint tempPos;
					sint32 i;
     				sint32 result;
					for (i = 0 ; i < SOUTH ; i++)
					{
						result = closest_agent_pos.GetNeighborPosition(WORLD_DIRECTION(i),tempPos);
					    if(result)
						{
							CellUnitList *the_army=NULL;
							the_army = g_theWorld->GetArmyPtr(tempPos);
							if (!the_army)
							{  //search for cell without army
								if (GotoGoalTaskSolution(closest_agent_ptr, tempPos, SUB_TASK_RALLY ) == false)
									return false;
								break;
							}						   
						}
					}
				}
			}
			
			
			tmp_agents.insert(tmp_agents.begin(), *closest_agent_iter);
			tmp_agents.erase(closest_agent_iter);
		}
		else
		{
			
			Assert(false);
		}

		++agent1_iter;
	} 
	return true;
}


bool CTPGoal::UnGroupTroops() 
{
	Agent_List      tmp_agents  = m_agents;
	bool            breturn     = false;
	
	for
    (
	    Agent_List::iterator    agent1_iter = tmp_agents.begin();
        agent1_iter != tmp_agents.end();
        ++agent1_iter
    ) 
	{
	    CTPAgent_ptr    ctpagent1_ptr   = (CTPAgent_ptr) *agent1_iter;
		Assert(ctpagent1_ptr);

		if (!ctpagent1_ptr->Get_Is_Dead())
		{
		    if (ctpagent1_ptr->Get_Army().Num() > 1)
		    {
			    ctpagent1_ptr->Ungroup_Order();
		    }

		    breturn = true;
		}
	}	
	
	return breturn;
}


bool CTPGoal::UnGroupComplete() const
{
	for 
    (
        Agent_List::const_iterator  agent_iter = m_agents.begin(); 
        agent_iter != m_agents.end(); 
        ++agent_iter
    )
	{
        CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr) *agent_iter;
        Assert(ctpagent_ptr);

		if (!ctpagent_ptr->Get_Is_Dead() &&
            (ctpagent_ptr->Get_Army().Num() > 1)
           )
        {
		    return false;
        }
	}

	return true;
}




bool CTPGoal::TryTransport(CTPAgent_ptr agent_ptr, const MapPoint & goal_pos) 
{
	
	if (g_theGoalDB->Get(m_goal_type)->GetNoTransport())
		return false; 

	
	if ( g_theWorld->GetContinent( goal_pos ) == 
		 g_theWorld->GetContinent(agent_ptr->Get_Pos()) )
		return false;
	
	CTPAgent_ptr transport_ptr;

	
    if ( FindTransport(agent_ptr, transport_ptr) ) { 

        
        if (LoadTransport(agent_ptr, transport_ptr))
			return true;        
    } 

	return false; 
}



bool CTPGoal::FindTransport(const CTPAgent_ptr & agent_ptr, CTPAgent_ptr & transport_ptr) 
{
	transport_ptr = NULL;

	double          max_utility         = Goal::BAD_UTILITY;
	sint32          needed_transport    = 0;
	sint32 const    dest_cont           = g_theWorld->GetContinent(Get_Target_Pos());

	for 
    (
        Agent_List::iterator    agent_iter  = m_agents.begin(); 
        agent_iter != m_agents.end(); 
        ++agent_iter
    )
	{
		CTPAgent_ptr    possible_transport  = (CTPAgent_ptr) *agent_iter;

        sint32          transports          = 0;
        sint32          max_slots           = 0;
        sint32          empty_slots         = 0;
		possible_transport->Get_Army()->GetCargo(transports, max_slots, empty_slots);

		if (dest_cont != g_theWorld->GetContinent(possible_transport->Get_Army()->RetPos()))
		{
			needed_transport += agent_ptr->Get_Army()->Num();
			needed_transport -= transports;
		}
		needed_transport -= empty_slots;

		if (max_slots <= 0)
			continue;

		if (possible_transport == agent_ptr)
			continue;

		if ( possible_transport->Get_Can_Be_Executed() == false)
		{
			MapPoint target_pos = possible_transport->Get_Target_Pos();
			MapPoint army_pos = agent_ptr->Get_Army()->RetPos();
			if (!target_pos.IsNextTo(army_pos))
				continue;
		}

	    double  utility         = Goal::BAD_UTILITY; 
		if (agent_ptr->EstimateTransportUtility(possible_transport, utility) )  
		{
			if (max_utility < utility) 
            { 
				max_utility     = utility; 
				transport_ptr   = possible_transport;
			} 
		}
	} 

	m_current_needed_strength.Set_Transport(static_cast<sint16>(needed_transport));

	return (transport_ptr != NULL); 
}



bool CTPGoal::LoadTransport(CTPAgent_ptr agent_ptr, CTPAgent_ptr transport_ptr)
{
	bool success = false; 

	if (transport_ptr->Get_Army()->GetMovementTypeAir() &&
		transport_ptr->Get_Army()->CanSpaceLaunch())
	{
		success = GotoTransportTaskSolution(agent_ptr, transport_ptr, SUB_TASK_AIRLIFT);
		if (success)
			{
				Set_Sub_Task(SUB_TASK_CARGO_TO_BOARD);
			}

	}
	else
	{

		success = GotoTransportTaskSolution(agent_ptr, transport_ptr, SUB_TASK_TRANSPORT_TO_BOARD);


		if (success) { 
			success = GotoTransportTaskSolution(agent_ptr, transport_ptr, SUB_TASK_CARGO_TO_BOARD);

			if (success)
			{
				Set_Sub_Task(SUB_TASK_CARGO_TO_BOARD);
				agent_ptr->Set_Can_Be_Executed(false);
				transport_ptr->Set_Can_Be_Executed(false);
			}
		}
	}


	if (success)
	{
		g_player[m_playerId]->
			AddCargoCapacity(static_cast<sint16>(-1 * agent_ptr->Get_Army().Num()));
	}

	return success; 
}






bool CTPGoal::ArmiesAtGoal() const
{
	MapPoint pos = Get_Target_Pos();
	CTPAgent_ptr ctpagent_ptr;

	Agent_List::const_iterator agent_iter;
	for (agent_iter = m_agents.begin();
		 agent_iter != m_agents.end();
		 ++agent_iter) {
			
			ctpagent_ptr = (CTPAgent_ptr) *agent_iter;
            if (ctpagent_ptr->Get_Pos() != pos)
				return false;
    } 

	return true;
}


sint32 CTPGoal::GetThreatenBonus() const
{
	Threat tmp_threat;
	PLAYER_INDEX target_owner = Get_Target_Owner();
	const GoalRecord *goal_record = g_theGoalDB->Get(m_goal_type);
	const Diplomat & diplomat = Diplomat::GetDiplomat(m_playerId);
	
	
	if (goal_record->GetThreatenTypeNone())
		return 0;

	switch (goal_record->GetThreatenType())
	{
	
	
    case k_Goal_ThreatenType_DestroyCity_Bit:
		if (diplomat.HasThreat(target_owner, THREAT_DESTROY_CITY, tmp_threat))
		{
			
			if (tmp_threat.detail.arg.cityId == m_target_city.m_id)
				return goal_record->GetThreatenBonus();
		}
		break;
	}
	return 0;
}


bool CTPGoal::Goal_Too_Expensive() const
{
	return (m_current_attacking_strength.Get_Agent_Count() > k_MAX_ARMY_SIZE) 
        && (m_current_attacking_strength.Get_Value() > 
                m_current_needed_strength.Get_Value() * 3
           );
}
