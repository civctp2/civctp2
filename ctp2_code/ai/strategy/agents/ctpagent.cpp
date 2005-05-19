//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Goal handling
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Resolved ambiguous sqrt call.
// - Added an isStealth parameter in CharacterizeArmy method - Calvitix
// - Changed the rounds calculation method (not based on default mouvement cost (100),
//   but based on the minimum cost between start and destination points) - Calvitix
//----------------------------------------------------------------------------

#include "c3.h"


#include "gfx_options.h"

#include "ctpagent.h"
#include "robotastar2.h"

#include "ArmyPool.h"
#include "World.h"
#include "Cell.h"
#include "Unit.h"
#include "Army.h"
#include "ArmyData.h"
#include "player.h"
#include "Events.h"
#include "order.h"
#include "GameEventUser.h"
#include "CTPDatabase.h"
#include "OrderRecord.h"
#include "GoalRecord.h"
#include "UnitRecord.h"
#include "mapanalysis.h"
#include "ctpaidebug.h"
#include "ctpai.h"

using namespace std;

extern CTPDatabase<OrderRecord> *g_theOrderDB;
extern ArmyPool		*g_theArmyPool;
extern World		*g_theWorld;
extern MapPoint g_mp_size;














CTPAgent::CTPAgent()
{
    Init();
}


CTPAgent::CTPAgent(const CTPAgent &agent)
{
    *this = agent;
}


CTPAgent::~CTPAgent()
{
    
}



void CTPAgent::Init()
{
    
    Agent::Init();
    
    
    m_army = 0; 

	
	m_squad_class = SQUAD_CLASS_DEFAULT;
}


CTPAgent & CTPAgent::operator= (const CTPAgent & agent)
{
    
    Agent::operator=(agent);
    
    
    
    

    
    m_army = agent.m_army;
   
    return *this;
}


const Army & CTPAgent::Get_Army() const
{
    return m_army;
}


void CTPAgent::Set_Army(const Army & army)
{
    m_army = army;

    
    Assert(g_theArmyPool->IsValid(army));

#ifdef _DEBUG_SCHEDULER
	Assert(army->m_theAgent == NULL);
	army->m_theAgent = this;
#endif _DEBUG_SCHEDULER
}


PLAYER_INDEX CTPAgent::Get_Player_Number() const
{
    return m_playerId;
}


void CTPAgent::Set_Player_Number(const PLAYER_INDEX & player_num)
{
    m_playerId = player_num;
}


bool CTPAgent::Get_Is_Dead() const
{
    
    if (!g_theArmyPool->IsValid(m_army))
        return true;

#ifdef _DEBUG_SCHEDULER
	Assert(m_army->m_theAgent == this);
#endif _DEBUG_SCHEDULER

    
    if (m_army->GetOwner() != m_playerId)
        return true;

    return false;
}


SQUAD_CLASS CTPAgent::Compute_Squad_Class() 
{

	if (Get_Is_Dead())
		return 0x0;

	bool isspecial; 
	bool isstealth;
	sint32 maxattack; 
	sint32 maxdefense; 
	bool cancapture;
	bool haszoc;
	bool canbombard;
	sint32 transports;
	sint32 max;
	sint32 empty;

#ifdef _DEBUG_SCHEDULER
	Assert(m_army->m_theAgent == this);
#endif _DEBUG_SCHEDULER

	m_army->CharacterizeArmy(
		isspecial, 
	    isstealth,
		maxattack, 
		maxdefense, 
		cancapture,
		haszoc,
		canbombard);

	m_squad_class = 0x0;
	
	
	m_squad_class |= k_Goal_SquadClass_CanExplore_Bit;

	if ( isspecial )
		m_squad_class |= k_Goal_SquadClass_Special_Bit;

	if (isstealth)
		m_squad_class |= k_Goal_SquadClass_Stealth_Bit;

	if ( maxattack > 0 )
		m_squad_class |= k_Goal_SquadClass_CanAttack_Bit;

	if ( maxdefense > 0 )
		m_squad_class |= k_Goal_SquadClass_CanDefend_Bit;

	if ( cancapture )
		m_squad_class |= k_Goal_SquadClass_CanCaptureCity_Bit;

	if ( haszoc )
		m_squad_class |= k_Goal_SquadClass_HasZoc_Bit;

	if ( canbombard )
		m_squad_class |= k_Goal_SquadClass_CanBombard_Bit;

	if ( m_army->GetCargo(transports,max,empty) == TRUE )
		m_squad_class |= k_Goal_SquadClass_CanTransport_Bit;

	
	if (( m_army->CountMovementTypeSea() > 0) || isspecial)
		m_squad_class &= ~k_Goal_SquadClass_CanDefend_Bit;

	return m_squad_class;
}


bool CTPAgent::IsArmyPosFilled() const
{
	return ( g_theWorld->GetCell(Get_Pos())->GetNumUnits() >= k_MAX_ARMY_SIZE );
}


bool CTPAgent::CanMove() const
{
	
	if (m_army->GetFirstMoveThisTurn())
		return true;

	return false; 
}



MapPoint CTPAgent::Get_Pos() const
{
	static MapPoint pos(0,0);

    
    if (!g_theArmyPool->IsValid(m_army))
        return pos;

	return m_army->RetPos();
}


const Squad_Strength & CTPAgent::Compute_Squad_Strength()
{
	
	sint32 hitpoints;				
	sint32 defense_strength;		
	sint32 attack_strength;			
	sint32 ranged_strength;			
	sint32 total_value;			    
	sint32 defense_count;
	sint32 ranged_count;
	

    Assert( g_theArmyPool->IsValid(m_army) );

	sint32 transports, max_slots, empty_slots;
	m_army->GetCargo(transports, max_slots, empty_slots);

	m_army->GetArmyStrength( hitpoints, 
		 			         defense_count, 
							 ranged_count, 
						     attack_strength,
						     defense_strength,
						     ranged_strength,
						     total_value );

	
	m_squad_strength.Set_Agent_Count(m_army.Num());
	m_squad_strength.Set_Attack( attack_strength );
	m_squad_strength.Set_Defense( defense_strength );
	m_squad_strength.Set_Defenders( defense_count );
	m_squad_strength.Set_Ranged( ranged_strength );
	m_squad_strength.Set_Ranged_Units( ranged_count );
	m_squad_strength.Set_Value( total_value );
	m_squad_strength.Set_Transport( empty_slots );

	return m_squad_strength;
}






void CTPAgent::Log_Debug_Info(const int & log) const
{
    
    int row = 0;
	int column = 0;
   
    
    if(!g_theArmyPool->IsValid(m_army))
    {
        AI_DPRINTF(log, m_playerId, -1, m_army.m_id, ("   Object Destroyed"));
        return;
    }

    MapPoint pos;
	m_army->GetPos(pos);
    
    
    
    
    AI_DPRINTF(log, m_playerId, -1, -1,
		("\t\t   Agent: handle=%x,\tclass=%x,\t(x=%d,y=%d),\t (is_used=%d)\n",
		m_army.m_id,
		m_squad_class,
		pos.x, 
		pos.y,
		(m_is_used ? 1 : 0)));
	
	AI_DPRINTF(log, -99, -1, m_army.m_id,
		("\t\t   -------\n")); 
}






bool CTPAgent::FindPathToBoard( const uint32 & move_intersection, const MapPoint & dest_pos, const bool & check_dest, Path & found_path ) 
{

	MapPoint start_pos;
    m_army->GetPos(start_pos); 

    float total_cost; 
    
    
    if ( start_pos == dest_pos)
	{ 
        found_path.Clear();
		found_path.JustSetStart( start_pos );
		found_path.Restart( start_pos ); 
        return true; 
    }

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
										   trans_max_r,
										   found_path,      
										   total_cost ))	
		{
			Assert(0 < found_path.Num()); 
			
			found_path.Start(start_pos);
			return true; 
		} 

    return false; 
}



bool CTPAgent::FindPath(const Army & army, const MapPoint & target_pos, const bool & check_dest, Path & found_path )
{
	MapPoint start_pos;
    army->GetPos(start_pos); 

    float total_cost; 
	double trans_max_r = 0.8;
	bool tmp_check_dest = check_dest;
	RobotAstar2::PathType path_type;
	uint32 move_union;
    
    
    if ( start_pos == target_pos)
	{ 
        found_path.Clear();
		found_path.JustSetStart( start_pos );
		found_path.Restart( start_pos ); 
        return true; 
    }

	sint32 cont = g_theWorld->GetContinent(target_pos);
	
	
	

    if (army->HasCargo() &&
		!army->GetMovementTypeAir()) 
		{ 
			
			uint32 move_intersection1 = army->GetMovementType();
			uint32 move_intersection2 = army->GetCargoMovementType();
			uint32 move_union = move_intersection1 | move_intersection2; 
			
			
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
										   trans_max_r,
										   found_path,     
										   total_cost ))   
		{
            Assert(0 < found_path.Num());
			
			found_path.Start(start_pos);
            return true; 
        } 

    return false; 
}


sint32 CTPAgent::GetRounds(const MapPoint & pos, sint32 & cells) const
{
    sint32 rounds = 0;
    double move_point_cost = 0.0; 
	double min_move;

	cells = 0;

	
    if (Get_Pos() == pos) 
        return 0; 

	cells = MapPoint::GetSquaredDistance(Get_Pos(), pos);
	if (cells > 0)
	{
	   ///Improvement of rounds evaluation (based on minimum cost point between
	   ///start and destination mappoints. - Calvitix
		Cell * myCell = g_theWorld->GetCell(pos);
		sint32 movement = myCell->GetMoveCost();
		myCell = g_theWorld->GetCell(Get_Pos());
		movement = std::min((long)movement,(long)myCell->GetMoveCost());

		//To DO : instead of 100.0, compute the min of terraint cost (with implementation)
		move_point_cost = movement * sqrt(static_cast<double>(cells)); //original : 100.0
	}

    Get_Army()->MinMovementPoints(min_move);
	
	
	if (move_point_cost <= min_move)
		return 1;
	else
		return sint32(ceil(move_point_cost/min_move)); 
}    



bool CTPAgent::EstimateTransportUtility(const CTPAgent_ptr transport, double & utility) const
{
    Assert(transport); 
	utility = 0.0;

    if (m_army->NumUnitsCanMoveIntoThisTransport(*transport->Get_Army().GetData()) <= 0)
		return false; 

    BOOL check_continents = !transport->Get_Army().GetMovementTypeAir();
	BOOL is_land;

	sint32 my_continent;
	g_theWorld->GetContinent( Get_Pos(), my_continent, is_land );
	
    MapPoint trans_pos = transport->Get_Pos(); 
    if (check_continents) 
	{ 
		sint32 trans_cont;
		
		g_theWorld->GetContinent( trans_pos, trans_cont, is_land ); 
        
		
		if ( is_land ) 
		{
			
			if ( g_theWorld->LandShareWater( trans_cont, my_continent ) == FALSE ) 
				return false; 
		}
		else 
		{ 
			
			if ( g_theWorld->IsLandNextTooWater(my_continent, trans_cont) == FALSE) 
				return false;	
		} 
	}

	sint32 tile_count;
	sint32 trans_rounds = GetRounds(trans_pos, tile_count);

	
	
	double move_type_bonus = 0.0;
	move_type_bonus += 
		transport->Get_Army()->CountMovementTypeSea() * 1000.0;

	utility = move_type_bonus + (trans_rounds * -100.0) - tile_count;

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


bool CTPAgent::Follow_Path(const Path & found_path, const sint32 & order_type)
{
    
    
    
    
    
    

	
	Path *tmpPath = new Path(found_path);
	MapPoint target_pos = tmpPath->GetEnd();

	
	sint32 range = 0;
	if (order_type >= 0 && g_theOrderDB->Get(order_type)->GetRange())
	{
		g_theOrderDB->Get(order_type)->GetRange(range);
	}

	
	Assert(range < 10);

	
	while (range > 0)
	{
		tmpPath->SnipEnd();
		range--;
	}
	
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MoveOrder, 
		GEA_Army, m_army,		 
		GEA_Path, tmpPath,		 
		GEA_MapPoint, target_pos, 
		GEA_Int, (order_type == -1), 
		GEA_End);

	
	Set_Can_Be_Executed(false);

	return true;
}


bool CTPAgent::Can_Execute_Order(const sint32 & order_type) const
{
	const OrderRecord *order = g_theOrderDB->Get(order_type);

	bool can_execute =  m_army->TestOrderAll(order);

	return can_execute;
}


void CTPAgent::Execute_Order(const sint32 & order_type, const MapPoint & target_pos)
{
	m_targetPos = target_pos;
	m_targetOrder = order_type;

	
	if (order_type < 0)
		return;

	Assert(order_type < g_theOrderDB->NumRecords());

	
	sint32 game_event = s_orderDBToEventMap[order_type];

	
	sint32 range = 0;
	if (g_theOrderDB->Get(order_type)->GetRange())
	{
		g_theOrderDB->Get(order_type)->GetRange(range);
	}

	if (range > 0)
	{
		
		g_gevManager->AddEvent( GEV_INSERT_Tail, 
							static_cast<GAME_EVENT>(game_event), 
							GEA_Army, m_army, 
							GEA_MapPoint, target_pos, 
							GEA_End);
	}
	else
	{
	
		
		g_gevManager->AddEvent( GEV_INSERT_Tail, 
			static_cast<GAME_EVENT>(game_event), 
			GEA_Army, m_army, 
			GEA_End);

	}

	
	Set_Can_Be_Executed(false);

}


void CTPAgent::Group_Order()
{
	
	g_gevManager->AddEvent( GEV_INSERT_Tail, 
		GEV_GroupOrder, 
		GEA_Army, m_army, 
		GEA_End);

	
	Set_Can_Be_Executed(false);
}


void CTPAgent::Group_With( CTPAgent_ptr second_army )
{
	Army army = second_army->Get_Army();

	for (sint32 unit_num = army.Num() - 1; unit_num >= 0; unit_num--)
	{
		const Unit & unit = army.Access(unit_num);

		
		g_gevManager->AddEvent( GEV_INSERT_Tail, 
			GEV_GroupUnitOrder, 
			GEA_Army, m_army, 
			GEA_Unit, unit, 
			GEA_End);
	}

	
	second_army->Set_Can_Be_Executed(false);
}


void CTPAgent::Ungroup_Order()
{
	
	g_gevManager->AddEvent( GEV_INSERT_Tail, 
		GEV_UngroupOrder, 
		GEA_Army, m_army, 
		GEA_End);

	
	Set_Can_Be_Executed(false);
}


void CTPAgent::MoveIntoTransport()
{
	
	g_gevManager->AddEvent( GEV_INSERT_Tail, 
					   GEV_BoardTransportOrder, 
					   GEA_Army, Get_Army(), 
					   GEA_End);
	
	Set_Can_Be_Executed(false);
}


std::vector<sint32, dbgallocator<sint32> > CTPAgent::s_orderDBToEventMap;
void CTPAgent::AssociateEventsWithOrdersDB()
{
	
	const char *event_name;
	s_orderDBToEventMap.resize(g_theOrderDB->NumRecords());
	for (int order_index = 0; order_index < g_theOrderDB->NumRecords(); order_index++)
	{
		event_name = g_theOrderDB->Get(order_index)->GetEventName();
		if (strlen(event_name) > 0)
			s_orderDBToEventMap[order_index] = g_gevManager->GetEventIndex(event_name);
		else
		{
			s_orderDBToEventMap[order_index] = -1;
			DPRINTF(k_DBG_SCHEDULER, ("AssociateEventsWithOrdersDB: Event %s not found\n",
				event_name));
		}
	}
}


sint32 CTPAgent::DisbandObsoleteUnits()
{
	Unit city_unit;
	CityData *city;
	MapPoint pos;

	
	if (!g_theArmyPool->IsValid(Get_Army()))
	{
		
		
		return 0;
	}

	sint32 unit_count = Get_Army().Num();

	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];

	
	if (unit_count <= 0)
		return 0;

	
	if (Get_Can_Be_Executed() == false)
		return 0;

	
	if (Get_Army()->IsObsolete() == false)
		return 0;

	
	if (Get_Army()->CanSettle() == TRUE)
		return 0;

	
	pos = Get_Pos();
	sint32 power = MapAnalysis::GetMapAnalysis().GetPower(m_playerId, pos);
	sint32 threat = MapAnalysis::GetMapAnalysis().GetThreat(m_playerId, pos);
	if ( (power > 0) && ((threat/(double)power) > 1.0))
		return 0;

	
	city_unit = g_theWorld->GetCity(pos);

	
	if (city_unit.m_id == 0)
		{
			
			MapPoint nearest_city_pos;
			sint32 nearest_distance = g_mp_size.x + g_mp_size.y; 
			sint32 distance;
			bool found = false;

			UnitDynamicArray *city_list = player_ptr->GetAllCitiesList();
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

					
					
					Get_Army()->PerformOrderHere(order_rec, (Path *) &found_path);
					Set_Can_Be_Executed(false);
					Set_Target_Order(order_rec->GetIndex());
					Set_Target_Pos(found_path.GetEnd());
#ifdef _DEBUG
					g_graphicsOptions->AddTextToArmy(Get_Army(), "DISBAND", 255);
#endif
				}
			return 0;
		}
	else 
		{
			
			city = city_unit.GetCityData();
			Assert(city);
		}

	sint32 min_garrison = city->GetNeededGarrison();

	CellUnitList city_units;
	g_theWorld->GetArmy(pos, city_units); 
	sint32 remaining_defenders = city_units.Num() - unit_count;

	
	if (remaining_defenders < min_garrison)
		return 0;

	
	const OrderRecord *order_rec = CtpAi::GetDisbandArmyOrder();
	if (order_rec)
		Get_Army()->PerformOrder(order_rec);

	return unit_count;
}
