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
// _DEBUG
// - Generate debug version when set.
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
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "ctpagent.h"

#include "gfx_options.h"
#include "robotastar2.h"
#include "World.h"          // g_theWorld
#include "Cell.h"
#include "Unit.h"
#include "Army.h"
#include "ArmyData.h"
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

extern MapPoint g_mp_size;


CTPAgent::CTPAgent()
:
    Agent               (),
    m_neededForGarrison (false),
    m_army              (),
    m_playerId          (PLAYER_UNASSIGNED),
    m_targetOrder       (OrderRecord::INDEX_INVALID),
    m_targetPos         ()
{
}

CTPAgent::CTPAgent(const Army & army)
:
    Agent               (),
    m_neededForGarrison (false),
    m_army              (army),
    m_playerId          (army->GetOwner()),
    m_targetOrder       (OrderRecord::INDEX_INVALID),
    m_targetPos         ()
{
	Compute_Squad_Strength();
}

CTPAgent::CTPAgent(CTPAgent const & an_Original)
:
    Agent               (an_Original),
    m_neededForGarrison (an_Original.m_neededForGarrison),
    m_army              (an_Original.m_army),
    m_playerId          (an_Original.m_playerId),
    m_targetOrder       (an_Original.m_targetOrder),
    m_targetPos         (an_Original.m_targetPos)
{
}

CTPAgent::~CTPAgent()
{
	// No action: nothing to delete   
}

CTPAgent & CTPAgent::operator = (CTPAgent const & an_Original)
{
	if (this != &an_Original)
	{
		Agent::operator = (an_Original);
		m_army          = an_Original.m_army;
		m_playerId      = an_Original.m_playerId;
		m_targetOrder   = an_Original.m_targetOrder;
		m_targetPos     = an_Original.m_targetPos;
	}

	return *this;
}

const Army & CTPAgent::Get_Army() const
{
	return m_army;
}

bool CTPAgent::Get_Is_Dead() const
{
	if (!m_army.IsValid())
		return true;

	return m_army->GetOwner() != m_playerId;
}

SQUAD_CLASS CTPAgent::Compute_Squad_Class()
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

bool CTPAgent::IsArmyPosFilled() const
{
	return g_theWorld->GetCell(Get_Pos())->GetNumUnits() >= k_MAX_ARMY_SIZE;
}

bool CTPAgent::IsOneArmyAtPos() const
{
	return g_theWorld->GetCell(Get_Pos())->GetNumUnits() == Get_Army()->Num();
}

sint32 CTPAgent::GetUnitsAtPos() const
{
	return g_theWorld->GetCell(Get_Pos())->GetNumUnits();
}

bool CTPAgent::CanMove() const
{
	return m_army.IsValid() && m_army->CanMove();
}

MapPoint CTPAgent::Get_Pos() const
{
	return m_army.IsValid() ? m_army->RetPos() : MapPoint();
}

const Squad_Strength & CTPAgent::Compute_Squad_Strength()
{
	Assert(m_army.IsValid());

	sint32 transports, max_slots, empty_slots;
	m_army->GetCargo(transports, max_slots, empty_slots);

	sint16 defense_count;
	sint16 ranged_count;
	double attack_strength;
	double defense_strength;
	double ranged_strength;
	double bombard_land_strength;
	double bombard_sea_strength;
	double bombard_air_strength;
	double total_value;
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

void CTPAgent::Log_Debug_Info(const int & log, const Goal * const goal) const
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

bool CTPAgent::FindPathToBoard(const uint32 & move_intersection, const MapPoint & dest_pos, const bool & check_dest, Path & found_path)
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

bool CTPAgent::FindPath(const Army & army, const MapPoint & target_pos, const bool & check_dest, Path & found_path)
{
	if(army->CheckValidDestination(target_pos))
	{
		found_path = Path(army->GetOrder(0)->m_path);
		return true;
	}

	float total_cost = 0.0f;
	return FindPath(army, target_pos, check_dest, found_path, total_cost);
}

bool CTPAgent::FindPath(const Army & army, const MapPoint & target_pos, const bool & check_dest, Path & found_path, float & total_cost)
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

double CTPAgent::GetRoundsPrecise(const MapPoint & pos, sint32 & cells) const
{
	double move_point_cost = 0.0;
	double min_move;

	if (Get_Pos() == pos)
	{
		cells = 0;
		return 0.0;
	}

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

		//To DO : instead of 100.0, compute the min of terraint cost (with implementation)
		move_point_cost = movement * sqrt(static_cast<double>(cells)); //original : 100.0
	}

	Get_Army()->MinMovementPoints(min_move);

	// That are bee line move cost not necessarily the real path cost around an island for a shallow water unit.
	return move_point_cost/min_move;
}

sint32 CTPAgent::GetRounds(const MapPoint & pos, sint32 & cells) const
{
	return static_cast<sint32>(ceil(GetRoundsPrecise(pos, cells)));
}

bool CTPAgent::EstimateTransportUtility(const CTPAgent_ptr transport, Utility & utility) const
{
	Assert(transport);
	utility = 0.0;

	if (m_army->NumUnitsCanMoveIntoThisTransport(*transport->Get_Army().GetData()) <= 0)
		return false;

	bool check_continents = !transport->Get_Army().GetMovementTypeAir();
	bool is_land;

	sint16 my_continent;
	g_theWorld->GetContinent( Get_Pos(), my_continent, is_land );

	MapPoint trans_pos = transport->Get_Pos();
	if (check_continents)
	{
		sint16 trans_cont;

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

	size_t move_type_bonus = transport->Get_Army()->CountMovementTypeSea() * 1000;

	utility = move_type_bonus + (trans_rounds * -100) - tile_count;

	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, Get_Army()->GetOwner(), -1, -1,
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

void CTPAgent::Set_Target_Order(const sint32 target_order)
{
	m_targetOrder = target_order;
}

void CTPAgent::Set_Target_Pos(const MapPoint &target_pos)
{
	m_targetPos = target_pos;
}

sint32 CTPAgent::Get_Target_Order() const
{
	return m_targetOrder;
}

const MapPoint & CTPAgent::Get_Target_Pos() const
{
	return m_targetPos;
}

#if 0
void CTPAgent::Follow_Path(const Path & found_path, const sint32 & order_type)
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

bool CTPAgent::Can_Execute_Order(const sint32 & order_type) const
{
	const OrderRecord *order = g_theOrderDB->Get(order_type);

	bool can_execute =  m_army->TestOrderAll(order);

	return can_execute;
}

void CTPAgent::Execute_Order(const sint32 & order_type, const MapPoint & target_pos)
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
		Get_Army()->GetPos(pos);
		Set_Target_Pos(pos);
	}

	Set_Target_Order(order_type);
	Set_Can_Be_Executed(false);
}

void CTPAgent::Group_Order()
{
	Assert(Get_Can_Be_Executed());

	g_gevManager->AddEvent(GEV_INSERT_Tail,
	                       GEV_GroupOrder,
	                       GEA_Army, m_army,
	                       GEA_End
	                      );

	MapPoint pos;
	Get_Army()->GetPos(pos);
	Set_Target_Pos(pos);
	Set_Can_Be_Executed(false);
}

void CTPAgent::Group_With( CTPAgent_ptr second_army )
{
	Assert(second_army->Get_Can_Be_Executed());

	Army army = second_army->Get_Army();

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
	Get_Army()->GetPos(pos);

	Set_Target_Pos(pos);
	Set_Can_Be_Executed(false);

	second_army->Set_Target_Pos(pos);
	second_army->Set_Can_Be_Executed(false);

	const GoalRecord* rec = g_theGoalDB->Get(m_goal->Get_Goal_Type());

	const char * myText = rec->GetNameText();
	MBCHAR * myString   = new MBCHAR[strlen(myText) + 40];
	MBCHAR * goalString = new MBCHAR[strlen(myText) + 20];
	memset(goalString, 0, strlen(myText) + 20);
	memset(myString,   0, strlen(myText) + 40);

	for (uint8 myComp = 0; myComp < strlen(myText) - 5; myComp++)
	{
		goalString[myComp] = myText[myComp + 5];
	}

	MapPoint dest_pos = m_goal->Get_Target_Pos();

	sprintf(myString, "Grouping at (%d,%d) to %s (%d,%d)", pos.x, pos.y, goalString, dest_pos.x, dest_pos.y);
	g_graphicsOptions->AddTextToArmy(Get_Army(), myString, 220, m_goal->Get_Goal_Type());

	delete[] goalString;
	delete[] myString;
}

void CTPAgent::Ungroup_Order()
{
	Assert(Get_Can_Be_Executed());

	g_gevManager->AddEvent(GEV_INSERT_Tail,
	                       GEV_UngroupOrder,
	                       GEA_Army, m_army,
	                       GEA_End
	                      );

	MapPoint pos;
	Get_Army()->GetPos(pos);
	Set_Target_Pos(pos);
	Set_Can_Be_Executed(false);

	MBCHAR * myString = new MBCHAR[40];
	sprintf(myString, "Ungrouping at (%d,%d)", pos.x, pos.y);
	g_graphicsOptions->AddTextToArmy(Get_Army(), myString, 220, m_goal->Get_Goal_Type());
	delete[] myString;
}

void CTPAgent::MoveIntoTransport()
{
	Assert(Get_Can_Be_Executed());

	g_gevManager->AddEvent(GEV_INSERT_Tail,
	                       GEV_BoardTransportOrder,
	                       GEA_Army, Get_Army(),
	                       GEA_End
	                      );

	MapPoint pos;
	Get_Army()->GetPos(pos);
	Set_Target_Pos(pos);
	Set_Can_Be_Executed(false);
}

sint32 CTPAgent::DisbandObsoleteUnits()
{
	sint32 unit_count = m_army.IsValid() ? m_army.Num() : 0;

	if (unit_count <= 0)
		return 0;

	if (!Get_Can_Be_Executed())
		return 0;
	
	if (!Get_Army()->IsObsolete())
		return 0;

	if (!Get_Army()->IsEntrenched())
		return 0;

	if (Get_Army()->CanSettle())
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
		if (found && FindPath(Get_Army(), nearest_city_pos, true, found_path))
		{
			const OrderRecord *order_rec = CtpAi::GetDisbandArmyOrder();

			PerformOrderHere(order_rec, &found_path);
			g_graphicsOptions->AddTextToArmy(Get_Army(), "DISBAND", 255, m_goal ? m_goal->Get_Goal_Type() : -1);
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
		g_graphicsOptions->AddTextToArmy(Get_Army(), "DISBAND", 255, m_goal ? m_goal->Get_Goal_Type() : -1);
	}

	return unit_count;
}

void CTPAgent::PerformOrderHere(const OrderRecord * order_rec, const Path * path, GAME_EVENT_INSERT priority)
{
	Assert(Get_Can_Be_Executed());

	Get_Army()->PerformOrderHere(order_rec, path, priority);
	Set_Target_Order(order_rec->GetIndex());
	Set_Target_Pos(path->GetEnd());
	Set_Can_Be_Executed(false);
}

void CTPAgent::PerformOrder(const OrderRecord * order_rec)
{
	Assert(Get_Can_Be_Executed());

	MapPoint pos;
	Get_Army()->GetPos(pos);
	Get_Army()->PerformOrder(order_rec);
	Set_Target_Order(order_rec->GetIndex());
	Set_Target_Pos(pos);
	Set_Can_Be_Executed(false);
}

void CTPAgent::WaitHere(const MapPoint & goal_pos)
{
	if(Get_Can_Be_Executed())
	{
		Set_Can_Be_Executed(false);
		Get_Army()->ClearOrders();

		MapPoint pos;
		Get_Army()->GetPos(pos);
		MBCHAR * myString = new MBCHAR[40];
		sprintf(myString, "Waiting GROUP @ (%d,%d) to GO (%d,%d)", pos.x, pos.y, goal_pos.x, goal_pos.y);
		g_graphicsOptions->AddTextToArmy(Get_Army(), myString, 220, m_goal->Get_Goal_Type());
		delete[] myString;
	}
}

void CTPAgent::ClearOrders()
{
	if(Has_Goal())
	{
		Get_Army()->ClearOrders();

		const GoalRecord* rec = g_theGoalDB->Get(m_goal->Get_Goal_Type());

		const char * myText = rec->GetNameText();
		MBCHAR * myString   = new MBCHAR[strlen(myText) + 40];
		MBCHAR * goalString = new MBCHAR[strlen(myText) + 20];
		memset(goalString, 0, strlen(myText) + 20);
		memset(myString,   0, strlen(myText) + 40);

		for(uint8 myComp = 0; myComp < strlen(myText) - 5; myComp++)
		{
			goalString[myComp] = myText[myComp + 5];
		}

		MapPoint pos;
		Get_Army()->GetPos(pos);
		if(!m_goal->Get_Invalid())
		{
			MapPoint dest_pos = m_goal->Get_Target_Pos();

			sprintf(myString, "Clearing oders at (%d,%d) for %s (%d,%d)", pos.x, pos.y, goalString, dest_pos.x, dest_pos.y);
		}
		else
		{
			sprintf(myString, "Clearing oders at (%d,%d) for %s (%d,%d)", pos.x, pos.y, goalString, m_targetPos.x, m_targetPos.y);
		}

		g_graphicsOptions->AddTextToArmy(Get_Army(), myString, 220, m_goal->Get_Goal_Type());

		delete[] goalString;
		delete[] myString;
	}
}

bool CTPAgent::HasMovePoints() const
{
	double movePoints;
	Get_Army()->CurMinMovementPoints(movePoints);
	return movePoints >= 1.0;
}
