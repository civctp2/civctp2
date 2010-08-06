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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Resolved ambiguous sqrt call.
// - Added an isStealth parameter in CharacterizeArmy method - Calvitix
// - Changed the rounds calculation method (not based on default mouvement cost (100),
//   but based on the minimum cost between start and destination points) - Calvitix
// - Made FindPath and GetRounds methods more flexible. (25-Jan-2008 Martin Gühmann)
// - Disband army text is also shown in the optimized version. (26-Jan-2008 Martin Gühmann)
// - Changed rounds calculation back to original method. (30-Jan-2008 Martin Gühmann)
// - Agents used in now exclusively set here. (8-Feb-2008 Martin Gühmann)
// - Standartized army strength computation. (30-Apr-2008 Martin Gühmann)
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
// - For boats with cargo the squad class is now computed from the cargo. (06-Sep-2008 Martin Gühmann)
// - Empty transporters cannot defend. (06-Sep-2008 Martin Gühmann)
// - Fixed unit garrison assignment. (23-Jan-2009 Martin Gühmann)
// - Merged in CTPAgent, removed virtual functions, for design and speed
//   improvement. (29-Jan-2009 Martin Gühmann)
// - Merged in Squad, no need for an additional class, just wastes space. (29-Jan-2009 Martin Gühmann)
// - Removed last Squad remainings, no need to allocate all that memory. (14-Feb-2009 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "agent.h"

#include "gfx_options.h"
#include "robotastar2.h"
#include "CityAstar.h"
#include "Cell.h"
#include "Unit.h"
#include "player.h"
#include "Events.h"
#include "Order.h"
#include "GameEventUser.h"
#include "CTPDatabase.h"
#include "OrderRecord.h"    // g_theOrderDB
#include "GoalRecord.h"
#include "UnitRecord.h"
#include "mapanalysis.h"
#include "ctpaidebug.h"
#include "ctpai.h"
#include "GoalRecord.h"
#include "Goal.h"

extern MapPoint  g_mp_size;
extern CityAstar g_city_astar;

Agent::Agent()
:
    m_squad_class       (SQUAD_CLASS_DEFAULT),
    m_agent_type        (-1),
    m_goal              (NULL),
    m_squad_strength    (0),
    m_can_be_executed   (true),
    m_detached          (false),
    m_neededForGarrison (false),
    m_army              (),
    m_playerId          (PLAYER_UNASSIGNED),
    m_targetOrder       (OrderRecord::INDEX_INVALID),
    m_targetPos         ()
{
}

Agent::Agent(const Army & army)
:
    m_squad_strength    (0),
    m_goal              (NULL),
    m_army              (army),
    m_playerId          (army->GetOwner()),
    m_targetOrder       (OrderRecord::INDEX_INVALID),
    m_targetPos         (),
    m_squad_class       (SQUAD_CLASS_DEFAULT),
    m_agent_type        (-1),
    m_can_be_executed   (true),
    m_detached          (false),
    m_neededForGarrison (false)
{
	Compute_Squad_Strength();
}

Agent::Agent(const Agent & an_Original)
:
    m_squad_strength    (an_Original.m_squad_strength),
    m_goal              (NULL),                            // Pointer reference, different after a scheduler copy
    m_army              (an_Original.m_army),
    m_playerId          (an_Original.m_playerId),
    m_targetOrder       (an_Original.m_targetOrder),
    m_targetPos         (an_Original.m_targetPos),
    m_squad_class       (an_Original.m_squad_class),
    m_agent_type        (an_Original.m_agent_type),
    m_can_be_executed   (true),                            // Bound to a goal, which is just a reference
    m_detached          (an_Original.m_detached),
    m_neededForGarrison (an_Original.m_neededForGarrison)  // Depends on location
{
}

Agent::~Agent()
{
// Nothing to delete, references only
}

Agent & Agent::operator = (const Agent & an_Original)
{
	Assert(false); // Hopefully not used
	if (this != &an_Original)
	{
		m_squad_class       = an_Original.m_squad_class;
		m_agent_type        = an_Original.m_agent_type;
		m_goal              = an_Original.m_goal;
		m_squad_strength    = an_Original.m_squad_strength;
		m_can_be_executed   = an_Original.m_can_be_executed;
		m_detached          = an_Original.m_detached;
		m_neededForGarrison = an_Original.m_neededForGarrison;
		m_army              = an_Original.m_army;
		m_playerId          = an_Original.m_playerId;
		m_targetOrder       = an_Original.m_targetOrder;
		m_targetPos         = an_Original.m_targetPos;
	}

	return *this;
}

SQUAD_CLASS Agent::Compute_Squad_Class()
{
	m_squad_class = SQUAD_CLASS_DEFAULT;

	if(Get_Is_Dead())
	{
		return m_squad_class;
	}

	bool   isspecial;
	bool   isstealth;
	sint32 maxattack;
	sint32 maxdefense;
	bool   cancapture;
	bool   haszoc;
	bool   canbombard;
	sint32 transports;
	sint32 max;
	sint32 empty;

	bool   canTransport = m_army->GetCargo(transports, max, empty);
	bool   isEmpty      = max > 0 && max == empty;

	if(!m_army->HasCargo())
	{
		m_army->CharacterizeArmy(
		    isspecial,
		    isstealth,
		    maxattack,
		    maxdefense,
		    cancapture,
		    haszoc,
		    canbombard);
	}
	else
	{
		m_army->CharacterizeCargo(
		    isspecial,
		    isstealth,
		    maxattack,
		    maxdefense,
		    cancapture,
		    haszoc,
		    canbombard);
	}

	m_squad_class |= k_Goal_SquadClass_CanExplore_Bit;

	if(isspecial)
		m_squad_class |= k_Goal_SquadClass_Special_Bit;

	if(isstealth)
		m_squad_class |= k_Goal_SquadClass_Stealth_Bit;

	if(maxattack > 0)
		m_squad_class |= k_Goal_SquadClass_CanAttack_Bit;

	if(maxdefense > 0 && !isEmpty && !isspecial)
		m_squad_class |= k_Goal_SquadClass_CanDefend_Bit;

	if(cancapture)
		m_squad_class |= k_Goal_SquadClass_CanCaptureCity_Bit;

	if(haszoc)
		m_squad_class |= k_Goal_SquadClass_HasZoc_Bit;

	if(canbombard)
		m_squad_class |= k_Goal_SquadClass_CanBombard_Bit;

	if(canTransport)
		m_squad_class |= k_Goal_SquadClass_CanTransport_Bit;

	return m_squad_class;
}

const Squad_Strength & Agent::Compute_Squad_Strength()
{
	Assert(m_army.IsValid());

	sint32 transports, max_slots, empty_slots;
	m_army->GetCargo(transports, max_slots, empty_slots);

	sint16 defense_count;
	sint16 ranged_count;
	float  attack_strength;
	float  defense_strength;
	float  ranged_strength;
	float  bombard_land_strength;
	float  bombard_sea_strength;
	float  bombard_air_strength;
	float  total_value;
	m_army->ComputeStrength(attack_strength,
	                        defense_strength,
	                        ranged_strength,
	                        defense_count,
	                        ranged_count,
	                        bombard_land_strength,
	                        bombard_sea_strength,
	                        bombard_air_strength,
	                        total_value,
	                        true
	                       );

	m_squad_strength.Set_Agent_Count (m_army.Num()                    );
	m_squad_strength.Set_Attack      (attack_strength                 );
	m_squad_strength.Set_Defense     (defense_strength                );
	m_squad_strength.Set_Defenders   (defense_count                   );
	m_squad_strength.Set_Ranged      (ranged_strength                 );
	m_squad_strength.Set_Ranged_Units(ranged_count                    );
	m_squad_strength.Set_Bombard_Land(bombard_land_strength           );
	m_squad_strength.Set_Bombard_Sea (bombard_sea_strength            );
	m_squad_strength.Set_Bombard_Air (bombard_air_strength            );
	m_squad_strength.Set_Value       (total_value                     );
	m_squad_strength.Set_Transport   (static_cast<sint16>(empty_slots));

	return m_squad_strength;
}

void Agent::Log_Debug_Info(const int & log, const Goal * const goal) const
{
	if (!m_army.IsValid())
	{
		AI_DPRINTF(log, m_playerId, -1, m_army.m_id, ("   Object Destroyed"));
		return;
	}

	MapPoint pos;
	m_army->GetPos(pos);

	AI_DPRINTF(log,
	           m_playerId,
	           -1,
	           -1,
	           ("\t\t   Agent: handle=%x,\tclass=%x,\t(x=%d,y=%d),\t (is_used=%d) \t (by_this=%d) \t (in %s)\n",
	            m_army.m_id,
	            m_squad_class,
	            pos.x,
	            pos.y,
	            (m_goal ? 1 : 0),
	            ((goal == m_goal) ? 1 : 0),
	            (g_theWorld->HasCity(pos) ? g_theWorld->GetCity(pos).GetName() : "field")
	           )
	          );
	
	AI_DPRINTF(log, -99, -1, m_army.m_id,
		("\t\t   -------\n")); 
}

bool Agent::FindPathToBoard(const uint32 & move_intersection, const MapPoint & dest_pos, const bool & check_dest, Path & found_path)
{
	MapPoint start_pos;
	m_army->GetPos(start_pos);

	if(start_pos == dest_pos)
	{
		found_path.Clear();
		found_path.JustSetStart( start_pos );
		found_path.Restart( start_pos );
		return true;
	}

	if(m_army->CheckValidDestination(dest_pos))
	{
		found_path = Path(m_army->GetOrder(0)->m_path);
		return true;
	}

	float total_cost;
	double move_points;
	m_army->MinMovementPoints(move_points);

	double trans_max_r = 100.0 / move_points;
	sint32 cont = g_theWorld->GetContinent(dest_pos);

	if (RobotAstar2::s_aiPathing.FindPath( RobotAstar2::PATH_TYPE_TRANSPORT,
										   m_army,
										   move_intersection,
										   start_pos,
										   dest_pos,
										   check_dest,
										   cont,
										   static_cast<float>(trans_max_r),
										   found_path,
										   total_cost ))
	{
		Assert(0 < found_path.Num());

		found_path.Start(start_pos);
		return true;
	}

	return false;
}

bool Agent::FindPath(const Army & army, const MapPoint & target_pos, const bool & check_dest, Path & found_path)
{
	if(army->CheckValidDestination(target_pos))
	{
		found_path = Path(army->GetOrder(0)->m_path);
		return true;
	}

	float total_cost = 0.0f;
	return FindPath(army, target_pos, check_dest, found_path, total_cost);
}

bool Agent::FindPath(const Army & army, const MapPoint & target_pos, const bool & check_dest, Path & found_path, float & total_cost)
{
	MapPoint start_pos;
	army->GetPos(start_pos);

	if ( start_pos == target_pos)
	{
		found_path.Clear();
		found_path.JustSetStart( start_pos );
		found_path.Restart( start_pos ); 
		return true;
	}

	sint32 cont = g_theWorld->GetContinent(target_pos);

	double trans_max_r = 0.8;
	bool tmp_check_dest = check_dest;
	RobotAstar2::PathType path_type;
	uint32 move_union;

	if(army->HasCargo()
	&&!army->GetMovementTypeAir()
	){
		uint32 move_intersection1 = army->GetMovementType();
		uint32 move_intersection2 = army->GetCargoMovementType();

		move_union = move_intersection1 | move_intersection2;

		tmp_check_dest = false;
		path_type = RobotAstar2::PATH_TYPE_TRANSPORT;
	}
	else
	{
		move_union = 0x0;
		path_type = RobotAstar2::PATH_TYPE_DEFENSIVE;
	}
	
	if (RobotAstar2::s_aiPathing.FindPath( path_type,
										   army,
										   move_union,
										   start_pos,
										   target_pos,
										   tmp_check_dest,
										   cont,
										   static_cast<float>(trans_max_r),
										   found_path,
										   total_cost ))
	{
		Assert(0 < found_path.Num());

		found_path.Start(start_pos);
		return true;
	}

	return false;
}

double Agent::GetRoundsPrecise(const MapPoint & pos, sint32 & cells) const
{
	double move_point_cost = 0.0;
	double min_move;

	if (Get_Pos() == pos)
	{
		cells = 0;
		return 0.0;
	}

#if 0
	// This would care about shallow water and the like but it is too slow.
	uint32 move_union = m_army->GetMovementType();
	if(m_army->HasCargo()
	&&!m_army->GetMovementTypeAir()
	){
		move_union |= m_army->GetCargoMovementType();
	}

	float cost = 0.0f;
	g_city_astar.FindCantEnterPenaltyDistance(m_playerId, Get_Pos(), pos, cost, cells, move_union);
	
	move_point_cost = static_cast<double>(cost);
	cells *= cells;
#endif

	cells = MapPoint::GetSquaredDistance(Get_Pos(), pos);
	if (cells > 0)
	{
		///Improvement of rounds evaluation (based on minimum cost point between
		///start and destination mappoints. - Calvitix
		Cell *          myCell      = g_theWorld->GetCell(pos);
		Cell *          otherCell   = g_theWorld->GetCell(Get_Pos());
		double const    movement    = 100.0;
		// This does not do the trick, better avaerage 
		// over all tiles from pos to target, unfortunately this
		// is slow.
		//  std::min(myCell->GetMoveCost(), otherCell->GetMoveCost());

		//ToDo : instead of 100.0, compute the min of terrain costs (with implementation)
		move_point_cost = movement * sqrt(static_cast<double>(cells)); //original : 100.0
	}

	m_army->MinMovementPoints(min_move);

	// That are bee line move cost not necessarily the real path cost around an island for a shallow water unit.
	return move_point_cost/min_move;
}

sint32 Agent::GetRounds(const MapPoint & pos, sint32 & cells) const
{
	return static_cast<sint32>(ceil(GetRoundsPrecise(pos, cells)));
}

bool Agent::CanReachTargetContinent(const MapPoint & pos) const
{
	if(m_army.GetMovementTypeAir())
		return true;

	sint16 target_cont;
	sint16 my_continent;
	bool   origin_is_land;
	bool   target_is_land;

	g_theWorld->GetContinent( Get_Pos(), my_continent, origin_is_land );
	g_theWorld->GetContinent( pos, target_cont, target_is_land );

	if(origin_is_land && target_is_land)
	{
		return g_theWorld->LandShareWater(my_continent, target_cont);
	}
	else if(!origin_is_land && !target_is_land)
	{
		return my_continent == target_cont;
	}
	else if(origin_is_land && !target_is_land)
	{
		return g_theWorld->IsLandNextTooWater(my_continent, target_cont);
	}
	else // if(!origin_is_land && target_is_land) // Last possibility
	{
		return g_theWorld->IsLandNextTooWater(target_cont, my_continent);
	}
}

bool Agent::EstimateTransportUtility(const Agent_ptr transport, Utility & utility) const
{
	Assert(transport);
	utility = 0;

	if (m_army->NumUnitsCanMoveIntoThisTransport(*transport->m_army.GetData()) <= 0)
		return false;

	bool check_continents = !transport->m_army.GetMovementTypeAir();
	bool is_land;


	MapPoint trans_pos = transport->Get_Pos();
	if (check_continents)
	{
		sint16 trans_cont;
		sint16 my_continent;

		g_theWorld->GetContinent( Get_Pos(), my_continent, is_land );
		g_theWorld->GetContinent( trans_pos, trans_cont, is_land );

		if ( is_land )
		{
			if (!g_theWorld->LandShareWater( trans_cont, my_continent ))
				return false;
		}
		else
		{
			if (!g_theWorld->IsLandNextTooWater(my_continent, trans_cont))
				return false;
		}
	}

	sint32 tile_count;
	sint32 trans_rounds = GetRounds(trans_pos, tile_count);

	size_t move_type_bonus = transport->m_army->CountMovementTypeSea() * 1000;

	utility = move_type_bonus + (trans_rounds * -100) - tile_count;

	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_army->GetOwner(), -1, -1,
	("\t %9x (%3d,%3d),\t%9x (%3d,%3d),\t%8d,\t%8d,\t%8d,\t%8d\n",
	this,                                          // This agent
	this->Get_Pos().x,                             // Agent pos.x
	this->Get_Pos().y,                             // Agent pos.y
	transport,                                     // Found transport
	transport->Get_Pos().x,                        // Transport pos.x
	transport->Get_Pos().y,                        // Transport pos.y
	utility,                                       // Transport utility
	move_type_bonus,                               // Movement bonus of transporter type
	trans_rounds,                                  // Distance to transporter (Square rooted quare distance), not identical with path distance
	tile_count));                                  // Rounds to target

	return true;
}

#if 0
void Agent::Follow_Path(const Path & found_path, const sint32 & order_type)
{
	Assert(Get_Can_Be_Executed());

	Path *tmpPath = new Path(found_path);
	MapPoint target_pos = tmpPath->GetEnd();

	sint32 range = 0;
	if (order_type >= 0)
	{
		(void) g_theOrderDB->Get(order_type)->GetRange(range);
	}

	Assert(range < 10);

	while (range > 0)
	{
		tmpPath->SnipEnd();
		range--;
	}

	g_gevManager->AddEvent(GEV_INSERT_Tail,
	                       GEV_MoveOrder,
	                       GEA_Army, m_army,
	                       GEA_Path, tmpPath,
	                       GEA_MapPoint, target_pos,
	                       GEA_Int, (order_type == -1),
	                       GEA_End
	                      );

	Set_Target_Pos(target_pos);
	Set_Target_Order(order_type);
	Set_Can_Be_Executed(false);
}
#endif

bool Agent::Can_Execute_Order(const sint32 & order_type) const
{
	const OrderRecord *order = g_theOrderDB->Get(order_type);

	bool can_execute =  m_army->TestOrderAll(order);

	return can_execute;
}

void Agent::Execute_Order(const sint32 & order_type, const MapPoint & target_pos)
{
	Assert(Get_Can_Be_Executed());

	if (order_type < 0)
	{
		Set_Target_Order(-1);
		return;
	}

	Assert(order_type < g_theOrderDB->NumRecords());

	sint32 game_event = ArmyData::OrderToEvent(order_type);

	sint32 range = 0;
	(void) g_theOrderDB->Get(order_type)->GetRange(range);

	if (range > 0)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail,
		                       static_cast<GAME_EVENT>(game_event),
		                       GEA_Army, m_army,
		                       GEA_MapPoint, target_pos,
		                       GEA_End
		                      );

		Set_Target_Pos(target_pos);
	}
	else
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail,
		                       static_cast<GAME_EVENT>(game_event),
		                       GEA_Army, m_army,
		                       GEA_End
		                      );

		MapPoint pos;
		m_army->GetPos(pos);
		Set_Target_Pos(pos);
	}

	Set_Target_Order(order_type);
	Set_Can_Be_Executed(false);
}

void Agent::Group_Order()
{
	Assert(Get_Can_Be_Executed());

	g_gevManager->AddEvent(GEV_INSERT_Tail,
	                       GEV_GroupOrder,
	                       GEA_Army, m_army,
	                       GEA_End
	                      );

	MapPoint pos;
	m_army->GetPos(pos);
	Set_Target_Pos(pos);
	Set_Can_Be_Executed(false);
}

void Agent::Group_With( Agent_ptr second_army )
{
	Assert(second_army->Get_Can_Be_Executed());

	Army army = second_army->m_army;

	for (sint32 unit_num = army.Num() - 1; unit_num >= 0; unit_num--)
	{
		const Unit & unit = army.Access(unit_num);

		g_gevManager->AddEvent(GEV_INSERT_Tail,
		                       GEV_GroupUnitOrder,
		                       GEA_Army, m_army,
		                       GEA_Unit, unit,
		                       GEA_End
		                      );
	}

	MapPoint pos;
	m_army->GetPos(pos);

	Set_Target_Pos(pos);
	Set_Can_Be_Executed(false);

	second_army->Set_Target_Pos(pos);
	second_army->Set_Can_Be_Executed(false);

	const GoalRecord* rec = g_theGoalDB->Get(m_goal->Get_Goal_Type());

	const char * myText = rec->GetNameText();
	MBCHAR * myString   = new MBCHAR[strlen(myText) + 80];
	MBCHAR * goalString = new MBCHAR[strlen(myText) + 40];
	memset(goalString, 0, strlen(myText) + 40);
	memset(myString,   0, strlen(myText) + 80);

	for (uint8 myComp = 0; myComp < strlen(myText) - 5; myComp++)
	{
		goalString[myComp] = myText[myComp + 5];
	}

	MapPoint dest_pos = m_goal->Get_Target_Pos();

	sprintf(myString, "Grouping at (%d,%d) to %s (%d,%d)", pos.x, pos.y, goalString, dest_pos.x, dest_pos.y);
	g_graphicsOptions->AddTextToArmy(m_army, myString, 220, m_goal->Get_Goal_Type());

	delete[] goalString;
	delete[] myString;
}

void Agent::Ungroup_Order()
{
	Assert(Get_Can_Be_Executed());

	g_gevManager->AddEvent(GEV_INSERT_Tail,
	                       GEV_UngroupOrder,
	                       GEA_Army, m_army,
	                       GEA_End
	                      );

	MapPoint pos;
	m_army->GetPos(pos);
	Set_Target_Pos(pos);
	Set_Can_Be_Executed(false);

	MBCHAR * myString = new MBCHAR[256];
	sprintf(myString, "Ungrouping at (%d,%d)", pos.x, pos.y);
	g_graphicsOptions->AddTextToArmy(m_army, myString, 220, m_goal->Get_Goal_Type());
	delete[] myString;
}

void Agent::MoveIntoTransport()
{
	Assert(Get_Can_Be_Executed());

	g_gevManager->AddEvent(GEV_INSERT_Tail,
	                       GEV_BoardTransportOrder,
	                       GEA_Army, m_army,
	                       GEA_End
	                      );

	MapPoint pos;
	m_army->GetPos(pos);
	Set_Target_Pos(pos);
	Set_Can_Be_Executed(false);
}

void Agent::UnloadCargo()
{
	Assert(Get_Can_Be_Executed());

	MapPoint pos;
	MapPoint pos2;
	m_army->GetPos(pos);
	sint32 cargoNum = m_army->GetCargoNum();

	if(cargoNum + g_theWorld->GetCell(pos)->GetNumUnits() > k_MAX_ARMY_SIZE)
	{
		for(
		    WORLD_DIRECTION d = NORTH;
		                    d < NOWHERE;
		                    d = (WORLD_DIRECTION)((sint32)d + 1)
		   )
		{
			if
			  (
			       pos.GetNeighborPosition(d, pos2)
			    && m_army->CargoCanEnter(pos2)
			    && cargoNum + g_theWorld->GetCell(pos2)->GetNumUnits() <= k_MAX_ARMY_SIZE
			  )
			{
				pos = pos2;
				break;
			}
		}
	}

	g_gevManager->AddEvent(GEV_INSERT_Tail,
	                       GEV_UnloadOrder,
	                       GEA_Army, m_army,
	                       GEA_MapPoint, pos,
	                       GEA_End
	                      );

	Set_Target_Pos(pos);
	Set_Can_Be_Executed(false);
}

sint32 Agent::DisbandObsoleteArmies()
{
	sint32 count = DisbandObsoleteUnits();

	if (count > 0)
	{
		AI_DPRINTF(k_DBG_SCHEDULER, m_army.GetOwner(), -1, -1, ("*** Disbanding Army:\n"));
		Log_Debug_Info(k_DBG_SCHEDULER, Get_Goal());
	}

	return count;
}

sint32 Agent::DisbandObsoleteUnits()
{
	sint32 unit_count = m_army.IsValid() ? m_army.Num() : 0;

	if (unit_count <= 0)
		return 0;

	if (!Get_Can_Be_Executed())
		return 0;
	
	if (!m_army->IsObsolete())
		return 0;

	if (m_army->HasCargo())
		return 0;

	if (m_army->CanSettle())
		return 0;

	MapPoint    pos         = Get_Pos();
	sint32      power       = 
	    MapAnalysis::GetMapAnalysis().GetPower(m_playerId, pos);
	sint32      threat      = 
	    MapAnalysis::GetMapAnalysis().GetThreat(m_playerId, pos);

	if ( (power > 0) && ((threat/(double)power) > 1.0))
		return 0;

	Unit        city_unit   = g_theWorld->GetCity(pos);

	if (city_unit.m_id == 0)
	{
		MapPoint nearest_city_pos;
		sint32 nearest_distance = g_mp_size.x + g_mp_size.y; 
		sint32 distance;
		bool found = false;

		// Get nearest own city
		/// @ToDo move this into its own method
		Assert(g_player[m_playerId]);
		UnitDynamicArray *city_list = g_player[m_playerId]->GetAllCitiesList();
		for (sint16 i = 0; i < city_list->Num(); i++)
		{
			city_unit = city_list->Access(i);
			distance = MapPoint::GetSquaredDistance(city_unit.RetPos(), pos);
			if (distance < nearest_distance)
			{
				nearest_city_pos = city_unit.RetPos();
				nearest_distance = distance;
				found = true;
			}
		}

		Path found_path;
		if (found && FindPath(m_army, nearest_city_pos, true, found_path))
		{
			const OrderRecord *order_rec = CtpAi::GetDisbandArmyOrder();

			PerformOrderHere(order_rec, &found_path);
			g_graphicsOptions->AddTextToArmy(m_army, "DISBAND", 255, m_goal ? m_goal->Get_Goal_Type() : -1);
		}
		return 0;
	}

	CityData *  city    = city_unit.GetCityData();
	Assert(city);

	CellUnitList city_units;
	g_theWorld->GetArmy(pos, city_units); 
	sint32 remaining_defenders = city_units.Num() - unit_count;

	if (remaining_defenders < city->GetNeededGarrison())
		return 0;

	const OrderRecord *order_rec = CtpAi::GetDisbandArmyOrder();
	if(order_rec)
	{
		PerformOrder(order_rec);
		g_graphicsOptions->AddTextToArmy(m_army, "DISBAND", 255, m_goal ? m_goal->Get_Goal_Type() : -1);
	}

	return unit_count;
}

void Agent::PerformOrderHere(const OrderRecord * order_rec, const Path * path, GAME_EVENT_INSERT priority)
{
	Assert(Get_Can_Be_Executed());

	m_army->PerformOrderHere(order_rec, path, priority);
	Set_Target_Order(order_rec->GetIndex());
	Set_Target_Pos(path->GetEnd());
	Set_Can_Be_Executed(false);
}

void Agent::PerformOrder(const OrderRecord * order_rec)
{
	Assert(Get_Can_Be_Executed());

	MapPoint pos;
	m_army->GetPos(pos);
	m_army->PerformOrder(order_rec);
	Set_Target_Order(order_rec->GetIndex());
	Set_Target_Pos(pos);
	Set_Can_Be_Executed(false);
}

void Agent::WaitHere(const MapPoint & goal_pos)
{
	if(Get_Can_Be_Executed())
	{
		Set_Can_Be_Executed(false);
		m_army->ClearOrders();

		MapPoint pos;
		m_army->GetPos(pos);
		MBCHAR * myString = new MBCHAR[255];
		sprintf(myString, "Waiting GROUP @ (%d,%d) to GO (%d,%d)", pos.x, pos.y, goal_pos.x, goal_pos.y);
		g_graphicsOptions->AddTextToArmy(m_army, myString, 220, m_goal->Get_Goal_Type());
		delete[] myString;
	}
}

void Agent::ClearOrders()
{
	if(Has_Any_Goal())
	{
		m_army->ClearOrders();

		const GoalRecord* rec = g_theGoalDB->Get(m_goal->Get_Goal_Type());

		const char * myText = rec->GetNameText();
		MBCHAR * myString   = new MBCHAR[strlen(myText) + 80];
		MBCHAR * goalString = new MBCHAR[strlen(myText) + 40];
		memset(goalString, 0, strlen(myText) + 40);
		memset(myString,   0, strlen(myText) + 80);

		for(uint8 myComp = 0; myComp < strlen(myText) - 5; myComp++)
		{
			goalString[myComp] = myText[myComp + 5];
		}

		MapPoint pos;
		m_army->GetPos(pos);
		if(!m_goal->Get_Invalid())
		{
			MapPoint dest_pos = m_goal->Get_Target_Pos();

			sprintf(myString, "Clearing oders at (%d,%d) for %s (%d,%d)", pos.x, pos.y, goalString, dest_pos.x, dest_pos.y);
		}
		else
		{
			sprintf(myString, "Clearing oders at (%d,%d) for %s (%d,%d)", pos.x, pos.y, goalString, m_targetPos.x, m_targetPos.y);
		}

		g_graphicsOptions->AddTextToArmy(m_army, myString, 220, m_goal->Get_Goal_Type());

		delete[] goalString;
		delete[] myString;
	}
}

bool Agent::HasMovePoints() const
{
	double movePoints;
	m_army->CurMinMovementPoints(movePoints);
	return movePoints >= 1.0;
}
