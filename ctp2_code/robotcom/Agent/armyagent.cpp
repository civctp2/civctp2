




























































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































#include "c3.h"
#include "globals.h"
#include "IMapPointData.h"
#include "IC3RobotAstar.h"
#include "IC3Player.h"
#include "IC3Wonder.h"
#include "IC3UnitDB.h"
#include "IC3World.h"
#include "DynArr.h" 


#include "AiMap.h"
#include "AiCell.h"
#include "AIRndCnt.h"
#include "IC3GameState.h"
#include "AiMain.h"
#include "FrameState.h"
#include "Cont.h"


#include "dr_debug.h"
#include "semi_dynamic_array.h"
#include "matrix.h"
#include "Grassfire.h"
#include "Strategic_Map.h"


#include "BSet.h"
#include "ArmyAgent.h"
#include "CityAgent.h"
#include "AiBldQue.h"
#include "CivArchive.h"


#include "FzOut.h"


#include "ArmyGoal.h"
#include "GoalDefense.h"
#include "GlSettl.h"
#include "GlExpl.h"
#include "GlAttck.h"
#include "GlTrans.h"
#include "GlEnslave.h"
#include "ScienceAgent.h"
#include "Squad_Strength.h"
#include "gold.h"


#include "AstarPnt.h"

#include "WallClock.h"
extern Wall_Clock *g_wall_clock;


#include "Order.h"
#include "AiCause.h"
#include "C3Player.h"
#include "Foreigner.h"
#include "ForeignCity.h"

#include "MoveFlags.h"


#define DISPLAY_ARMY_DETAILS 1














extern double fz_max_attack_strength; 
extern double fz_max_defense_strength; 

extern double fz_city_threat_decay; 

extern double fz_com_city_decay;
extern double fz_com_city_scale; 

extern double fz_explore_near_civ; 

extern double fz_recruit_goal_threshold;  
extern double fz_attack_goal_threshold;  
extern double fz_settle_goal_threshold;  
extern double fz_explore_goal_threshold;  


extern double fz_settler_packing; 

extern double fz_army_foreign_threat_decay; 
extern double fz_army_foreign_threat_falloff; 

extern double fz_reserve_flank_attack ;   
extern double fz_reserve_flank_defense;
extern double fz_reserve_range0_attack;    
extern double fz_reserve_range0_defense;  
extern double fz_reserve_range1_attack;    
extern double fz_reserve_range1_defense;

extern double fz_unexplored_ocean_threshold;

extern double fz_inst_prod; 
extern double fz_inst_food; 
extern double fz_inst_move; 
extern double fz_inst_def; 
extern double fz_inst_vision_range; 
extern double fz_inst_airfield; 
extern double fz_inst_fort; 
extern double fz_inst_attack;
extern double fz_inst_firepower;

extern double fz_freight_coef;
extern double fz_gold_2_production;

extern double fz_get_on_islands_coef; 
extern double fz_coef_fill_area_to_boat_explore; 

extern double fz_time;
extern double fz_settle_patience;






void ArmyAgent::Init(AiMain *ai, const AGENT_STATE the_state)
{
    m_state = the_state;
    Init(ai); 
}

void ArmyAgent::Init(AiMain *ai)
{
    m_path_total_nPoints=10; 
    m_path_cur_nPoints=0; 
    m_path = new MapPointData[m_path_total_nPoints]; 
    m_path_cur=0; 
    
    m_nUnits=0;

    m_nUnits = 0; 







    
    m_agent_type = AGENT_TYPE_ARMY;
    m_cargo = NULL; 
    

    sint32 i;
    for (i=0; i<k_MAX_ARMY_SIZE; i++) { 
        m_unit_type[i]=-1; 
    }

     m_agent_strength->AddArmies(1); 

     m_my_pos = new MapPointData; 
	 m_my_pos->x = -1; 
	 m_my_pos->y = -1; 
	 m_my_pos->z = -1; 

     m_XYpos = new MapPointData; 
	 m_XYpos->x = -1; 
	 m_XYpos->y = -1; 
	 m_XYpos->z = -1; 

     m_my_continent = -1;
     m_my_is_land = FALSE; 

     UpdatePosition(ai); 

     m_current_movepoints = 100.0; 
     m_max_movepoints = 100.0;
     m_army_contains_settler  = FALSE;
     m_uses_fuel = FALSE; 
     m_max_fuel = FALSE; 
     m_current_fuel = 0; 
	 m_pathValid = FALSE;
	 m_pathThreat = 0.0;
	 m_lastGoal = GOAL_TYPE_MAX;
	 
     
     m_is_paratroop=FALSE; 
     m_has_been_paradropped=FALSE; 
     m_is_wounded = FALSE; 
	 m_probed_wormhole = FALSE;
}

ArmyAgent::~ArmyAgent()
{ 

    if (m_path) { 
        delete[] m_path; 
        m_path = NULL; 
    } 

    if (m_cargo) { 
        delete m_cargo; 
        m_cargo = NULL; 
    } 

    if (m_my_pos) { 
        delete m_my_pos; 
        m_my_pos = NULL; 
    } 

    if (m_XYpos) { 
        delete m_XYpos; 
        m_XYpos = NULL; 
    } 
}

ArmyAgent::ArmyAgent(AiMain *ai)
{ 
    m_home_city_agent = NULL; 
    m_home_city_id = 0; 
    Init(ai,  AGENT_STATE_LOOKING_FOR_WORK); 
}

ArmyAgent::ArmyAgent(AiMain *ai, BSetID id, CityAgent *hc, AGENT_STATE the_state)
{
   m_id = id; 
   m_home_city_agent = hc; 
   if (m_home_city_agent) { 
       m_home_city_id = hc->m_id; 
   } else { 
       m_home_city_id = 0; 
   } 

   Init(ai, the_state); 
}


ArmyAgent::ArmyAgent(IC3CivArchive *a) 
{
    Serialize(a); 
} 

void ArmyAgent::Serialize(IC3CivArchive *archive) 

{
   

    sint32 idx_cargo, num_cargo, max_cargo; 

    CivArchive *a  = (CivArchive *) archive; 

    Agent::Serialize(*a); 

    sint32 i; 

    if (a->IsStoring()) { 

        (*a) << m_path_total_nPoints; 
        (*a) << m_path_cur_nPoints;
        (*a) << m_path_cur; 

        for (i=0; i<m_path_cur_nPoints; i++) { 
            a->Store((uint8*)&m_path[i], sizeof(MapPointData)); 
        }

        a->PutSINT32(m_state); 
        a->PutSINT32(m_home_city_id.GetVal()); 
        (*a) << m_nUnits; 
        a->Store((uint8*)m_unit_type, k_MAX_ARMY_SIZE * sizeof(sint16));  
        if (m_cargo) { 
            (*a) << m_cargo->ArraySize(); 
            (*a) << m_cargo->Num(); 
            for (idx_cargo=0; idx_cargo<m_cargo->Num(); idx_cargo++) { 
                (*a) << m_cargo->Get(idx_cargo); 
            } 
        } else { 
            max_cargo = 0 ; 
            (*a) << max_cargo; 
        }

        (*a) << m_my_pos->x; 
        (*a) << m_my_pos->y; 
        (*a) << m_my_pos->z;
        
        (*a) << m_XYpos->x; 
        (*a) << m_XYpos->y; 
        (*a) << m_XYpos->z; 

        (*a) << m_my_continent;
        (*a) << m_current_movepoints;
        (*a) << m_max_movepoints;
        (*a) << m_max_fuel; 
        (*a) << m_current_fuel; 
		

        (*a) << m_pathThreat; 

        a->PutSINT32(m_lastGoal);

		
        a->PutSINT8(m_my_is_land); 
        a->PutSINT8(m_pathValid);
        a->PutSINT8(m_army_contains_settler);
        a->PutSINT8(m_uses_fuel); 
        a->PutSINT8(m_is_paratroop); 
        a->PutSINT8(m_has_been_paradropped); 
        a->PutSINT8(m_is_wounded); 
		a->PutSINT8(m_probed_wormhole);
        
    } else { 

		
        (*a) >> m_path_total_nPoints; 
        (*a) >> m_path_cur_nPoints;
        (*a) >> m_path_cur; 
        m_path = new MapPointData[m_path_total_nPoints]; 

        for (i=0; i<m_path_cur_nPoints; i++) { 
            a->Load((uint8*)&m_path[i], sizeof(MapPointData)); 
        }
        
        m_state = AGENT_STATE(a->GetSINT32()); 
        m_home_city_id = a->GetSINT32(); 
        m_home_city_agent = NULL; 
        (*a) >> m_nUnits; 
        a->Load((uint8*)m_unit_type, k_MAX_ARMY_SIZE * sizeof(sint16));  

        (*a)>> max_cargo; 
        if (0 < max_cargo) { 
            (*a) >> num_cargo; 
            m_cargo = new DynamicArray<sint32>(max_cargo); 
            for (idx_cargo=0; idx_cargo<num_cargo; idx_cargo++) { 
                m_cargo->ExtendByOneFlat(); 
                (*a) >> m_cargo->Access(idx_cargo); 
            }
        } else { 
            m_cargo = NULL; 
        }

        

  
        m_my_pos = new MapPointData; 
        (*a) >> m_my_pos->x; 
        (*a) >> m_my_pos->y; 
        (*a) >> m_my_pos->z;


        m_XYpos = new MapPointData; 
        (*a) >> m_XYpos->x; 
        (*a) >> m_XYpos->y; 
        (*a) >> m_XYpos->z; 

        (*a) >> m_my_continent;
        (*a) >> m_current_movepoints;
        (*a) >> m_max_movepoints;
        (*a) >> m_max_fuel; 
        (*a) >> m_current_fuel; 
		

        (*a) >> m_pathThreat; 

        m_lastGoal = GOAL_TYPE(a->GetSINT32());
		
		
        m_my_is_land = a->GetSINT8();
        m_pathValid = a->GetSINT8();
        m_army_contains_settler = a->GetSINT8();
        m_uses_fuel = a->GetSINT8(); 
        m_is_paratroop = a->GetSINT8();
        m_has_been_paradropped = a->GetSINT8(); 
        m_is_wounded = a->GetSINT8(); 
		m_probed_wormhole = a->GetSINT8();
        
    } 
}

BOOL ArmyAgent::Validate(AiMain *ai)
{ 
    BOOL r = Agent::Validate(ai); 
    return r; 
} 

void ArmyAgent::HookBldQueue(AiMain *ai) 

{ 
    
    if ((m_state == AGENT_STATE_BUILDING) || 
        (m_state == AGENT_STATE_UNBUILT)) { 

        m_home_city_agent = ai->m_city_set->Find(m_home_city_id); 
    }
}




void ArmyAgent::BeginTurn(AiMain *ai)
{ 
    if (AGENT_STATE_MARKED_DEAD == m_state) return; 

    UpdateMovePoints(ai);
    UpdateMaxMovePoints(ai); 

    if (m_state == AGENT_STATE_TRANSPORT_GOTO_BOARD) { 
        m_state = AGENT_STATE_LOOKING_FOR_WORK; 
    } else if (m_state == AGENT_STATE_CARGO_GOTO_BOARD) { 
        m_state = AGENT_STATE_LOOKING_FOR_WORK; 
    } 

    m_is_wounded = FALSE;

    if ((m_state != AGENT_STATE_UNBUILT) &&
        (m_state != AGENT_STATE_BUILDING)) { 
		if (GetUsesFuel()) { 
			UpdateCurrentFuel(ai);  
		}
		
		ResetArmyStrength(ai);
    }

    UpdatePosition(ai); 
}

BOOL ArmyAgent::GetPos(AiMain *ai, MapPointData &pos) const
{
    pos = *m_my_pos; 
    return TRUE; 
}


BOOL ArmyAgent::GetXYPos(AiMain *ai, MapPointData &pos) const
{
    pos = *m_XYpos; 
    return TRUE; 
}

void ArmyAgent::UpdateXYPos(AiMain *ai)
{
    BOOL is_unknown_id;

    switch (m_state) { 
    case AGENT_STATE_UNBUILT:
    case AGENT_STATE_BUILDING: 
        m_home_city_agent->GetXYPos(ai, *m_XYpos); 
        return; 
    default:
        ai->m_player->GetArmyXYPos(ai->my_player_index, m_id.GetVal(), 
            &is_unknown_id, m_XYpos); 

        Assert(is_unknown_id == FALSE); 
    }
}

void ArmyAgent::UpdatePosition(AiMain *ai)
{
    BOOL is_unknown_id;
    MapPointData new_pos;

    switch (m_state) { 
    case AGENT_STATE_MARKED_DEAD:
       return;
    case AGENT_STATE_UNBUILT:
    case AGENT_STATE_BUILDING: 
        Assert(m_home_city_agent); 
        m_home_city_agent->GetPos(new_pos); 
        break; 
    default:
        ai->m_player->GetArmyPos(m_id.GetVal(), &is_unknown_id, &new_pos); 

        Assert(is_unknown_id == FALSE); 
    }

#ifdef _DEBUG
    MapPointData *size=NULL; 
    size = ai->m_map->GetSize();
    
    Assert(0 <= new_pos.x); 
    Assert(new_pos.x < size->x); 
    Assert(0 <= new_pos.y); 
    Assert(new_pos.y < size->y); 
    Assert(0 <= new_pos.z); 
    Assert(new_pos.z < size->z); 
#endif 

    UpdatePosition(ai, &new_pos);
}

void ArmyAgent::UpdatePosition(AiMain *ai, MapPointData *new_pos)
{
	m_pathValid = ((m_path[m_path_cur].x != new_pos->x) &&
				   (m_path[m_path_cur].y != new_pos->y) &&
				   (m_path[m_path_cur].z != new_pos->z));
    
  

    if ((m_my_pos->x == new_pos->x) &&
        (m_my_pos->y == new_pos->y) &&
        (m_my_pos->z == new_pos->z)) {
        
        UpdateContinent(ai); 
        return; 
    } else { 

        *m_my_pos = *new_pos; 

        UpdateXYPos(ai);
        UpdateContinent(ai); 
    }
    
}
   
void ArmyAgent::UpdateContinent(AiMain *ai) 
{ 
    if (m_my_pos->z == GROUND_Z) { 
	    sint32 cont;
	    BOOL is_land;
        ai->m_continents->GetContinent(ai, *m_my_pos, cont, is_land); 
	    m_my_continent = (sint16) cont;
	    m_my_is_land = (is_land == TRUE);
    } else { 
        m_my_continent = -1; 
        m_my_is_land = FALSE; 
    } 
} 

sint32 ArmyAgent::GetContinent() const
{
    return m_my_continent; 
}

void ArmyAgent::SetContinent(const sint32 c)
{
    m_my_continent = (sint16) c;
}

BOOL ArmyAgent::GetMyIsLand() const
{
    return m_my_is_land; 
}

void ArmyAgent::SetMyIsLand(const BOOL l)
{
    m_my_is_land = (l == TRUE); 
}


BOOL ArmyAgent::IsInCity(AiMain *ai, CityAgent *&the_city)
{
    sint32 player_id; 
    uint32 c_id; 
    
    if (ai->m_world->GetCityId (m_my_pos, &player_id, &c_id)) {  
        Assert(ai->m_my_player_id == player_id); 
		
		if (ai->m_my_player_id != player_id)
			return FALSE;
        the_city = ai->m_city_set->Find(c_id);
        return TRUE;
    } else {
        the_city = NULL; 
        return FALSE; 
    } 
}

void ArmyAgent::SetHomeCityAgent(CityAgent *ca)
{
    Assert(ca);
    m_home_city_agent = ca;
    m_home_city_id = ca->m_id; 
}


sint32 ArmyAgent::GetUnbuiltType() const 
{ 
   Assert((m_state == AGENT_STATE_UNBUILT) || (m_state == AGENT_STATE_BUILDING)); 
   return m_unit_type[0]; 
}
   










void ArmyAgent::AddDelUnitTypeStrength(AiMain *ai, sint32 unit_type,
    BOOL add, const float hp)
{
    float as = float(ai->m_unitDB->GetAttack(unit_type));
    float ds = float(ai->m_unitDB->GetDefense(unit_type));
    float fp = float(ai->m_unitDB->GetFirepower(unit_type));
    float rs = float(ai->m_unitDB->GetZBRange(unit_type)); 
   
    sint32 max_hp; 

	if (add) {

        max_hp = ai->m_unitDB->GetHP_WithReadiness(unit_type); 
        if ((hp * 2) < max_hp) { 
            m_is_wounded = TRUE; 
        } 

	    m_agent_strength->AddAttack(as, fp, hp); 
		m_agent_strength->AddDefense(ds, fp, hp); 
        if (0 < rs) { 
            m_agent_strength->AddRangeStrength(rs,fp, hp); 
            m_agent_strength->AddRangeUnitCount(1); 
        } 

		if (the_squad) { 
			the_squad->m_squad_strength.AddAttack(as, fp, hp); 
			the_squad->m_squad_strength.AddDefense(ds, fp, hp); 
            if (0 < rs) { 
                the_squad->m_squad_strength.AddRangeStrength(rs,fp, hp); 
                the_squad->m_squad_strength.AddRangeUnitCount(1); 
            } 
		}
	}
	else
	{
	    m_agent_strength->SubtractAttack(as, fp, hp); 
		m_agent_strength->SubtractDefense(ds, fp, hp); 
        if (0 < rs) { 
            m_agent_strength->AddRangeStrength(-rs,fp, hp); 
            m_agent_strength->AddRangeUnitCount(-1); 
        } 

		if (the_squad) { 
			the_squad->m_squad_strength.SubtractAttack(as, fp, hp); 
			the_squad->m_squad_strength.SubtractDefense(ds, fp, hp); 
            if (0 < rs) { 
                the_squad->m_squad_strength.AddRangeStrength(-rs,fp,hp); 
                the_squad->m_squad_strength.AddRangeUnitCount(-1); 
            } 
		}
	}
}

void ArmyAgent::FlattenSquadStrength()
{ 
    if (the_squad) { 
		
        the_squad->m_squad_strength.Subtract(m_agent_strength); 
        m_agent_strength->Init(); 
    } 
} 




void ArmyAgent::ResetArmyStrength(AiMain *ai)
{
	
    FlattenSquadStrength(); 

	sint32 i, n; 
	sint32 unit_type[100]; 
	sint32 unit_hp[100]; 
	BOOL is_unknown_id; 
	
	ai->m_player->GetArmyCurrentHP(m_id.GetVal(), &is_unknown_id, &n, unit_type, unit_hp);

	sint32 full_slots, empty_slots;
	GetNumCargo(ai, full_slots, empty_slots);

	
	m_agent_strength->AddUnits(n);
	
    m_agent_strength->AddTransports(empty_slots); 
	
	m_agent_strength->AddArmies(1);
	
	if (the_squad)
	{
		
		the_squad->m_squad_strength.AddUnits(n);
		
		the_squad->m_squad_strength.AddTransports(empty_slots);
			
		the_squad->m_squad_strength.AddArmies(1);
	}

	
	if (full_slots <= 0)
	{
		for (i=0; i<n; i++) { 
			AddDelUnitTypeStrength(ai, unit_type[i], TRUE, float(unit_hp[i]));
		}    
	}
	
	else
	{
		sint32 idx_cargo; 
		sint32 unit_type;
		sint32 hp;
		for (idx_cargo=0; idx_cargo<full_slots; idx_cargo++) { 
			unit_type = m_cargo->Get(idx_cargo);
			
			hp = ai->m_unitDB->GetHP(unit_type); 
			AddDelUnitTypeStrength(ai, unit_type, TRUE, float(hp));
		}    
	}
} 

void ArmyAgent::InsertUnitType(AiMain *ai, sint32 add_me, sint32 hp)
{
	Assert(m_nUnits <= 9);

	
	if (m_nUnits <= 9)		{
			m_unit_type[m_nUnits] = (sint16) add_me; 
			m_nUnits++; 
    } else { 
        INSANE(TOO_MANY_UNITS); 
		return;
    }
   
    if (!m_army_contains_settler) 
        UpdateArmyContainsSettler(ai); 

    if (!m_uses_fuel)
        UpdateUsesFuel(ai); 

	

	

    UpdateMovePoints(ai); 
    UpdateMaxMovePoints(ai); 

	#ifdef _DEBUG
	
	
	
    
    

	
	#endif
}

void ArmyAgent::SetSingleUnitType(AiMain *ai,  sint32 unit_type, sint32 hp) 
{
	Assert(m_nUnits <= 9);

	m_unit_type[0] = (sint16) unit_type; 
	m_nUnits = 1; 
   
    UpdateArmyContainsSettler(ai); 

    UpdateUsesFuel(ai); 

	
	FlattenSquadStrength();
	AddDelUnitTypeStrength(ai, unit_type, TRUE, float(hp));

    UpdateMovePoints(ai); 
    UpdateMaxMovePoints(ai); 
}

void ArmyAgent::ChangeBuildingType (AiMain *ai,  sint32 unit_type)
{
    m_nUnits=0; 
    sint32 max_hp = ai->m_unitDB->GetHP(unit_type); 
    SetSingleUnitType(ai, unit_type, max_hp); 
}



BOOL ArmyAgent::IsWaitState()
{ 
    switch (m_state) { 
    case AGENT_STATE_MARKED_DEAD:
    case AGENT_STATE_MARKED_SETTLE:
    case AGENT_STATE_MARKED_GARRISON:
    case AGENT_STATE_MARKED_WAITING_TOBE_CARGO:
    case AGENT_STATE_MARKED_GROUP_ARMY:
    case AGENT_STATE_MARKED_WAITING_TO_RECIEVE:
    case AGENT_STATE_CARGO_GOTO_BOARD:
    case AGENT_STATE_TRANSPORT_GOTO_BOARD:
	case AGENT_STATE_AT_GOAL:
        return TRUE; 
    default:
        return FALSE; 
    }   
}

double ArmyAgent::EstimateUtility (AiMain *ai, ArmyGoal *goal_obj)
{

    if (IsWaitState()) { 
        return BAD_UTILITY; 
    } 

    Assert(0 < m_nUnits); 
    if (m_nUnits <= 0) { 
        return BAD_UTILITY; 
    } 

	double value = 0.0;				

    
	

    switch(goal_obj->GetType()) { 
    case GOAL_TYPE_SETTLE: 
        value = ActionBidToSettle(ai, (GoalSettle*)goal_obj, FALSE); 
		break;
    case GOAL_TYPE_EXPLORE:
        value = ActionBidToExplore(ai, (GoalExplore*)goal_obj); 
		break;
    case GOAL_TYPE_DEFENSE:
        value = ActionBidToDefend(ai, (GoalDefense*)goal_obj); 
		break;
	case GOAL_TYPE_ATTACK_REGION:
    case GOAL_TYPE_ATTACK:
        value = ActionBidToAttack(ai, (GoalAttack*)goal_obj); 
		break;
    case GOAL_TYPE_PERIMETER:
        value = ActionBidToPatrol(ai, goal_obj); 
		break;
    case GOAL_TYPE_PATROL:
        value = ActionBidToPatrol(ai, goal_obj); 
		break;



	case GOAL_TYPE_ENSLAVE:
		value = ActionBidToEnslave(ai, (GoalEnslave*) goal_obj);
		break;
    default: 
        return A_REALLY_BIG_NEGATIVE_REAL_NUMBER; 
    }

	
	return value;
}


double ArmyAgent::ActionBidToSettle(AiMain *ai, GoalSettle *goal_obj, 
    const BOOL will_be_transported) 
{
    MapPointData dest_pos;
    double move_point_cost;
    sint32 rounds;  
	double settle_now_bonus = 0.0;

    if (IsWaitState()) { 
        return BAD_UTILITY; 
    } 
    
    goal_obj->GetPos(dest_pos); 

  
    if (PretestSettle(ai, dest_pos) <= BAD_UTILITY) { 
        return BAD_UTILITY; 
    }



    
    if ((m_state == AGENT_STATE_UNBUILT) || (m_state == AGENT_STATE_BUILDING)) { 
        if (0 < m_home_city_agent->GetQueueLen()) { 
            return BAD_UTILITY; 
        }
    }
 
    if (PretestBid(ai, GOAL_TYPE_SETTLE, dest_pos) < (BAD_UTILITY + 0.5)) { 
        return BAD_UTILITY; 
    } 

    double build_time; 
    if (BidDistance(ai, will_be_transported, dest_pos, move_point_cost, rounds, build_time) < 0.0) { 
        return BAD_UTILITY; 
    } 

	

	sint32 simple_movement_rounds = MovePoints2Rounds(ai,  move_point_cost);

	
	if (fz_time <= 5.0)
	{
		
       
		if (simple_movement_rounds > 5.0)
			return BAD_UTILITY;

		
		settle_now_bonus = (ai->m_planner->the_AIP.max_misc_bid_bonus/(simple_movement_rounds+1));


	} 
	
	
	
	
	
	

    if (50 < rounds) { 
        return BAD_UTILITY; 
    } 

  
   

    double cargo_bonus = 0.0; 

    
    if ((GROUND_Z == m_my_pos->z) && (dest_pos.z == GROUND_Z)) { 


        sint32 dest_cont; 
        BOOL dest_is_land;

        
 	    if (!ai->m_continents->GetContinent(ai, dest_pos, dest_cont, dest_is_land)) { 
            return BAD_UTILITY;
        } 

        
        if ((m_state == AGENT_STATE_UNBUILT) || (m_state == AGENT_STATE_BUILDING)) {     
            if (10 <=  ai->m_continents->GetLandSettlers(m_my_continent)) {  
                    return BAD_UTILITY;
            }
        }

		
		
		if (m_my_is_land)
			{
				if (!ai->m_continents->LandGotSpaceLauncher(m_my_continent) &&
					!ai->m_science_agent->HasWaterMove()) { 

					if (m_my_continent != dest_cont) {
						return BAD_UTILITY;
					}
				}
			}
  	
        if (GotCargo()) { 

            if (!BoatCanReachShore(ai, dest_pos)) { 
                return BAD_UTILITY;
            }

            if (SETTLE_ENVIRONMENT_ISLAND == goal_obj->GetSettleEnvironment()) { 
                cargo_bonus = 1000.0; 
            } 
        }    

    } else if ((m_my_pos->z == GROUND_Z) && (dest_pos.z == SPACE_Z)) { 

        if (!CanSpaceLaunch(ai) &&
            !CanReachLauncher(ai)) { 
            return BAD_UTILITY;
        } 

    } else if ((m_my_pos->z == SPACE_Z) && (dest_pos.z == GROUND_Z)) { 
        if (!CanSpaceLand(ai)) { 
            return BAD_UTILITY; 
        }
    }




	double time_term;
    double cost  = move_point_cost * 0.01 + build_time; 

    
    time_term = (fz_settle_patience- cost * cost) / fz_settle_patience;
    
	if (time_term < 0.5) time_term = 0.5; 

    


    return settle_now_bonus + cargo_bonus +  
          time_term * goal_obj->Compute_Raw_Priority(ai);
	
	
	
	
	
	

    
    
}


BOOL ArmyAgent::CanReachLauncher(AiMain *ai)
{ 
    return IsAtSpaceLauncher(ai);
} 

BOOL ArmyAgent::IsAtSpaceLauncher(AiMain *ai)
{ 
   
    sint32 player_id; 
    uint32 c_id; 
    CityAgent *ca=NULL; 

    if (ai->m_world->GetCityId(m_my_pos, &player_id, &c_id)) { 
        ca = ai->m_city_set->Find(c_id); 
        return ca->HasSpaceLauncher(); 
    } else { 
        return FALSE; 
    } 
} 


double ArmyAgent::PretestSettle(AiMain *ai,  MapPointData &dest_pos) 

{ 
    BOOL is_unknown_id; 

    if (0 == ai->m_my_player_id) { 
        return BAD_UTILITY;
    } 

    switch (m_state) { 
    case AGENT_STATE_MARKED_DEAD:
    case AGENT_STATE_MARKED_SETTLE:
    case AGENT_STATE_MARKED_GARRISON:
        return BAD_UTILITY;     
    case AGENT_STATE_BUILDING: 
    case AGENT_STATE_UNBUILT:
       if (ai->m_player->UnitTypeCanSettle(GetUnbuiltType(), &dest_pos)) {
           return 1.0; 
       } else { 
           return BAD_UTILITY;
       }
    default: 
       if (ai->m_player->ArmyCanSettle(m_id.GetVal(), &is_unknown_id, &dest_pos)) {
           return 1.0; 
       } else { 
           if (GotCargo()) { 
               sint32 idx_cargo; 
               sint32 num_cargo = m_cargo->Num(); 
               for (idx_cargo=0; idx_cargo<num_cargo; idx_cargo++) { 
                   if (ai->m_unitDB->CanSettleHere(m_cargo->Get(idx_cargo), &dest_pos)) { 
                       return 1.0; 
                   }
               }
           }
           return BAD_UTILITY; 
       }
    }
}

double ArmyAgent::PretestIsSameCont(AiMain *ai,  MapPointData &dest_pos, 
      sint32 &dest_cont, BOOL &dest_is_land)
{



    if (!ai->m_continents->GetContinent(ai, dest_pos, dest_cont, dest_is_land)) { 
        return TRUE; 
    } 

    if ((m_my_is_land == dest_is_land) && (m_my_continent == dest_cont)) { 
        return 1.0; 
    } else { 
        return BAD_UTILITY;
    }
}

BOOL ArmyAgent::GetArmyContainsSettler() const
{ 
    return m_army_contains_settler; 
} 

void ArmyAgent::UpdateArmyContainsSettler(AiMain *ai) 
{
    sint16 i; 

    for (i=0; i<m_nUnits; i++) { 
        if (ai->m_unitDB->IsSettler(m_unit_type[i])) { 
            m_army_contains_settler = TRUE;
            return; 
        } 
    } 

    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (i=0; i<n; i++) { 
            if (ai->m_unitDB->IsSettler(m_cargo->Access(i))) { 
                m_army_contains_settler = TRUE; 
                return;
            } 
        }
    } 

    m_army_contains_settler = FALSE; 
}

sint32 ArmyAgent::GetSettlersCost(AiMain *ai)
{
	sint32 cost = 0;
    for (int i=0; i<m_nUnits; i++) { 
        if (ai->m_unitDB->IsSettler(m_unit_type[i])) 
			{ 
				cost += ai->m_unitDB->GetProductionCost(m_unit_type[i]);
        } 
    } 
	return cost;
}

uint32 ArmyAgent::GetActiveDefenseRangeCount(AiMain *ai) 
{
    sint16 i; 
	uint32 count = 0;

    for (i=0; i<m_nUnits; i++) { 
        if (ai->m_unitDB->GetActiveDefenseRange(m_unit_type[i]) > 0) { 
            count++;
        } 
    } 

	

    return count; 
}

BOOL ArmyAgent::ArmyContainsTransport(AiMain *ai) 
{     
    sint16 i; 

    for (i=0; i<m_nUnits; i++) { 
        if (ai->m_unitDB->IsTransport(m_unit_type[i])) { 
            return TRUE; 
        } 
    } 

	

    return FALSE; 
} 

BOOL ArmyAgent::GetNumCargo(AiMain *ai,  sint32 &full_slots, sint32 &empty_slots)
{
	sint32 num_units;
	sint32 unit_type[100]; 
	sint32 unit_hp[100]; 
	BOOL is_unknown_id; 

	
	if (m_state == AGENT_STATE_MARKED_DEAD)
	{
		BOOL GetNumCargo_of_Dead_Army = FALSE;

        

		full_slots = 0;
		empty_slots = 0;
		return FALSE;
	}

	
	ai->m_player->GetArmyCurrentHP(m_id.GetVal(), &is_unknown_id, &num_units, unit_type, unit_hp);

    if (ArmyContainsTransport(ai)) {    
        if ((m_state == AGENT_STATE_UNBUILT) || (m_state == AGENT_STATE_BUILDING)) { 
            empty_slots = ai->m_unitDB->GetMaxCargo(GetUnbuiltType()); 
            full_slots = 0; 
        } else { 
			
			
            
            
			
		    empty_slots = 0;
			for (sint32 unit_idx=0; unit_idx< num_units; unit_idx++) 
			{ 
				empty_slots += ai->m_unitDB->GetMaxCargo(unit_type[unit_idx]); 			
			} 
			
			Assert(empty_slots);
			
            full_slots = m_cargo->Num();
			empty_slots -= full_slots;
			Assert(empty_slots >= 0);
        } 
        return TRUE; 
    } else { 
        full_slots = 0; 
        empty_slots = 0; 
    } 
  
    return FALSE; 
} 

sint32 ArmyAgent::GetTransports(AiMain *ai)
{
	sint16 transports = 0;

	for (sint16 unit_idx=0; unit_idx<m_nUnits; unit_idx++) 
		transports += (ai->m_unitDB->GetMaxCargo(m_unit_type[unit_idx]) > 0); 			
	return transports;
}

BOOL ArmyAgent::IsCellWaterCont(AiMain *ai, sint16 x, sint16 y, sint16 z, 
    sint32 dest_cont)
{

    MapPointData w; 

    sint32 w_cont; 
    BOOL w_is_land;

    ai->m_map->Wrap(x, y, z, w); 
    if (ai->m_continents->GetContinent(ai, w, w_cont, w_is_land)) { 
       if (!w_is_land) { 
          if (w_cont == dest_cont) { 
             return TRUE; 
          } 
       }
    }

    return FALSE; 
}

BOOL ArmyAgent::TouchesThisOcean(AiMain *ai, MapPointData &my_pos, sint32 dest_cont)
{
    if (my_pos.z != 0)
        return FALSE; 

    if (IsCellWaterCont(ai, my_pos.x+1, my_pos.y+1, my_pos.z, dest_cont)) return TRUE; 
    if (IsCellWaterCont(ai, my_pos.x+1, my_pos.y, my_pos.z, dest_cont)) return TRUE; 
    if (IsCellWaterCont(ai, my_pos.x+1, my_pos.y-1, my_pos.z, dest_cont)) return TRUE; 

    if (IsCellWaterCont(ai, my_pos.x, my_pos.y+1, my_pos.z, dest_cont)) return TRUE; 
    if (IsCellWaterCont(ai, my_pos.x, my_pos.y-1, my_pos.z, dest_cont)) return TRUE; 

    if (IsCellWaterCont(ai, my_pos.x-1, my_pos.y+1, my_pos.z, dest_cont)) return TRUE; 
    if (IsCellWaterCont(ai, my_pos.x-1, my_pos.y, my_pos.z, dest_cont)) return TRUE; 
    if (IsCellWaterCont(ai, my_pos.x-1, my_pos.y-1, my_pos.z, dest_cont)) return TRUE; 

    return FALSE; 
}

double ArmyAgent::ActionBidToExplore(AiMain *ai,  GoalExplore *goal_obj)
{
    MapPointData dest_pos;
    double move_point_cost;
    sint32 rounds; 
    
    Assert(goal_obj); 

    goal_obj->GetPos(dest_pos); 

    if (!PretestFuelToTarget(ai, dest_pos)) { 
        return BAD_UTILITY; 
    }

    if (IsWounded()) return BAD_UTILITY; 

	if (CanBeRustled(ai))
		return BAD_UTILITY;

    if (GetArmyContainsSettler()) 
        return BAD_UTILITY;

    
    if (PretestBid(ai, goal_obj->GetType(), dest_pos) < (BAD_UTILITY + 0.5)) { 
        return BAD_UTILITY; 
    } 


    sint32 dest_cont=-1; 
    BOOL dest_is_land=FALSE;

    if (!ai->m_continents->GetContinent(ai, dest_pos, dest_cont, dest_is_land)) { 
        return BAD_UTILITY;
    }

    sint32 s; 
    if ((m_state == AGENT_STATE_UNBUILT) || (m_state == AGENT_STATE_BUILDING)) { 
        if (dest_is_land) { 
            if (ai->m_unitDB->CanMoveLand(GetUnbuiltType())) { 
                s = ai->m_continents->GetLandArmies(m_my_continent); 
                if (8 <= s) { 
                    return BAD_UTILITY;
                } 
            } else if (ai->m_unitDB->CanMoveAir(GetUnbuiltType())){ 
                
            } else { 
                return BAD_UTILITY; 
            } 
        } else { 
            if (ai->m_unitDB->CanMoveWater(GetUnbuiltType()) || 
                ai->m_unitDB->CanMoveShallowWater(GetUnbuiltType())) { 
                s = ai->m_continents->GetWaterArmies(dest_cont); 
                if (4 <= s) { 
                    return BAD_UTILITY;
                } 
            } else { 
                return BAD_UTILITY; 
            } 
        }
    }

    BOOL is_boat=FALSE;
    
    if (dest_is_land == FALSE) { 
        is_boat = ai->m_world->IsMoveTypeWater(&dest_pos) || ai->m_world->IsMoveTypeShallowWater(&dest_pos); 
    }












    
    if (is_boat) { 
       
        Assert(dest_is_land == FALSE); 

        if (AGENT_STATE_UNBUILT == m_state) { 

            if (!TouchesThisOcean(ai, *m_my_pos, dest_cont)) { 
                return BAD_UTILITY;                         
            } 

        } else { 
            if (m_my_is_land) { 

                if (!TouchesThisOcean(ai, *m_my_pos, dest_cont)) { 
                    return BAD_UTILITY;          
                } 

            } else { 

                if (m_my_continent != dest_cont) { 
                    return BAD_UTILITY; 
                }
            }
        }

    } else { 
        if (PretestIsSameCont(ai, dest_pos, dest_cont, dest_is_land) < 0.0) { 
            return BAD_UTILITY; 
        }       
    }

    if (PretestExploreBid() < 0.0) { 
          return BAD_UTILITY; 
    }

    double unit_type_bonus; 

	
    if (AGENT_STATE_UNBUILT == m_state) 
	{
		
		if (ai->m_unitDB->IsSettler(GetUnbuiltType()) )
		{
			
			unit_type_bonus = -ai->m_planner->the_AIP.better_unit_type_modifier/2; 
		}
		
		else
		{
			
			unit_type_bonus = -ai->m_planner->the_AIP.better_unit_type_modifier; 
		}
    } 
	
	else
	{
		
		unit_type_bonus = 0.0; 

	} 

    
	double build_time; 

    if (BidDistance(ai, FALSE, dest_pos, move_point_cost, rounds, build_time) < 0.0) { 
        return BAD_UTILITY; 
    } 

	

	
	
	
	double time_term;

	double modifier;

	double bonus = 0.0;

	

	
	
	double tile_count = ai->m_map->WrappedDistance(*m_my_pos, dest_pos);
	time_term = ( rounds * rounds 
		* ai->m_planner->the_AIP.distance_from_unit_priority_modifier )
		- tile_count;

    double boat_bonus;
    if (is_boat) { 

        boat_bonus = 100000.0; 
    } else { 
        boat_bonus = 0.0; 
    } 
	

    double fast_unit_bonus = (GetMaxMovePoints() - 200.0) * 0.01  * ai->m_planner->the_AIP.max_misc_bid_bonus * 0.1; 
	bonus = boat_bonus + unit_type_bonus + fast_unit_bonus;

	
	if (bonus > ai->m_planner->the_AIP.max_misc_bid_bonus)
		bonus = ai->m_planner->the_AIP.max_misc_bid_bonus;

	modifier = time_term + bonus;

	
	return modifier + goal_obj->Compute_Raw_Priority(ai);
}

double ArmyAgent::PretestExploreBid() const

{ 
    switch (m_state) { 
    case AGENT_STATE_GOTO_SETTLE_DEST:
    case AGENT_STATE_GOTO_RECRUITMENT_DEST:
    case AGENT_STATE_GOTO_ATTACK_DEST:
    case AGENT_STATE_MARKED_DEAD:
    case AGENT_STATE_MARKED_SETTLE:
    case AGENT_STATE_MARKED_GARRISON: 
	case AGENT_STATE_AT_GOAL:
        return BAD_UTILITY; 
    default: 
        return 1.0; 
    }
} 

double ArmyAgent::ActionBidToDefend(AiMain *ai,  GoalDefense *goal_obj)
{
    double move_point_cost;
    sint32 rounds; 

	double time_term;			
   
	if (!goal_obj->PretestUnitTypes(ai, this, goal_obj->GetType()))
		return BAD_UTILITY;

	Assert(m_nUnits);
	
	if (m_nUnits <= 0)
		return BAD_UTILITY;
    
    MapPointData dest_pos; 
    goal_obj->GetPos(dest_pos); 
    if (PretestBid(ai, goal_obj->GetType(), dest_pos) < (BAD_UTILITY + 0.5)) { 
        return BAD_UTILITY; 
    } 


    BOOL is_same_cont;
    BOOL is_space_dest;
    if (!PretestContinentReachable(ai, dest_pos, is_same_cont, is_space_dest)) { 
        return BAD_UTILITY; 
    } 
    
    if (GetArmyContainsSettler()) {
        return BAD_UTILITY; 
    }

    if (!PretestFuelToTarget(ai, dest_pos)) { 
        return BAD_UTILITY; 
    }

    if (GetUsesFuel()) { 
		
        
        
        
    }

    double build_time; 

    if (BidDistance(ai, FALSE, dest_pos, move_point_cost, rounds, build_time) < 0.0) { 
        return BAD_UTILITY; 
    } 

	

    GoalDefense *goal_rd = ((GoalDefense *) goal_obj); 
    Assert(goal_rd); 

	
    
	double tile_count = ai->m_map->WrappedDistance(*m_my_pos, dest_pos);

	time_term = ( rounds * rounds 
		* ai->m_planner->the_AIP.distance_from_unit_priority_modifier )
		- tile_count;

	
	

    Assert(0 < m_nUnits); 
    if (m_nUnits <= 0) { 
        return BAD_UTILITY; 
    } 

	double bonus;
	bonus = (GetActiveDefenseRangeCount(ai) / m_nUnits) * 
		ai->m_planner->the_AIP.max_misc_bid_bonus;

	
	return bonus + time_term + goal_obj->Compute_Raw_Priority(ai);
}
 

double ArmyAgent::ActionBidToPatrol(AiMain *ai,  ArmyGoal *goal_obj)
{
    double move_point_cost;
    sint32 rounds; 

    MapPointData dest_pos; 
    goal_obj->GetPos(dest_pos); 
    if (!PretestFuelToTarget(ai, dest_pos)) { 
        return BAD_UTILITY; 
    }

    double value = goal_obj->GetValue(); 

    if (IsWounded()) return BAD_UTILITY; 

    if (PretestAttack(ai, dest_pos, value) < 0.0) { 
        return BAD_UTILITY; 
    } 

    if (PretestBid(ai, GOAL_TYPE_ATTACK, dest_pos) < (BAD_UTILITY + 0.5)) { 
        return BAD_UTILITY; 
    } 

    double build_time; 

    if (BidDistance(ai, FALSE, dest_pos, move_point_cost, rounds, build_time) < 0.0) { 
        return BAD_UTILITY; 
    } 

	
	if (ai->m_my_player_id == PLAYER_INDEX_VANDALS &&
		rounds > ai->m_planner->the_AIP.wander_rounds_from_target)
		return BAD_UTILITY;

    sint32 dest_cont=-1; 
    BOOL dest_is_land=FALSE;
    BOOL check_continents = (!CanMoveSpace(ai)) && (!CanMoveAir(ai));

    if (check_continents) { 
        if (PretestIsSameCont(ai, dest_pos, dest_cont, dest_is_land) < 0.0) { 
            return BAD_UTILITY; 
        }       
    }

	
	double time_term;
   
	double tile_count = ai->m_map->WrappedDistance(*m_my_pos, dest_pos);
	time_term = ( rounds * rounds 
		* ai->m_planner->the_AIP.distance_from_unit_priority_modifier )
		- tile_count;

	return time_term  + goal_obj->Compute_Raw_Priority(ai);;
}


double ArmyAgent::ActionBidToAttack(AiMain *ai,  GoalAttack *goal_obj)
{
    double move_point_cost;
    sint32 rounds; 

    double value = goal_obj->GetValue(); 

    MapPointData dest_pos; 
    goal_obj->GetPos(dest_pos); 

    if (PretestAttack(ai, dest_pos, value) < 0.0) { 
        return BAD_UTILITY; 
    } 

    if (!PretestFuelToTarget(ai, dest_pos)) { 
        return BAD_UTILITY; 
    }

    if (PretestBid(ai, GOAL_TYPE_ATTACK, dest_pos) < (BAD_UTILITY + 0.5)) { 
        return BAD_UTILITY; 
    } 

    double build_time;
    if (BidDistance(ai, FALSE, dest_pos, move_point_cost, rounds, build_time) < 0.0) { 
        return BAD_UTILITY; 
    } 

	
	if (ai->m_my_player_id == PLAYER_INDEX_VANDALS &&
		rounds > ai->m_planner->the_AIP.wander_rounds_from_target)
		return BAD_UTILITY;

    BOOL is_same_cont;
    BOOL is_space_dest;
    if (!PretestContinentReachable(ai, dest_pos, is_same_cont, is_space_dest)) { 
        return BAD_UTILITY; 
    }
    
    
    double bonuses = ComputeTargetCombatBonus(ai, dest_pos, goal_obj, rounds);

	
	double time_term;   
	double tile_count = ai->m_map->WrappedDistance(*m_my_pos, dest_pos);
	time_term = ( rounds * rounds 
		* ai->m_planner->the_AIP.distance_from_unit_priority_modifier )
		- tile_count;

	double u =  bonuses + time_term  + goal_obj->Compute_Raw_Priority(ai);
    if (1000000.0 < u) { 
        goal_obj->Compute_Raw_Priority(ai);
    } 
    return u; 

}

double ArmyAgent::ComputeTargetCombatBonus(AiMain *ai, MapPointData &dest_pos, 
    ArmyGoal *goal_obj, const sint32 rounds)
{ 
    
    double m = ai->m_planner->the_AIP.max_misc_bid_bonus;

    double raw_goal_defense = goal_obj->GetDefenseStrength(); 
    double goal_defense = raw_goal_defense; 
    if (goal_defense < 1.0) { 
        goal_defense = 1.0; 
    } 

    double raw_goal_attack = goal_obj->GetAttackStrength(); 
    double goal_attack = raw_goal_attack; 
    if (goal_attack < 1.0) {
        goal_attack = 1.0; 
    } 
  
    double my_attack = m_agent_strength->GetAttack();
    double my_defense = m_agent_strength->GetDefense();

    
    double attack_power_ratio = 0.33 * (my_attack - goal_defense) / goal_defense; 
    if (1.0 < attack_power_ratio) { 
        attack_power_ratio = 1.0; 
    } else if (attack_power_ratio < -1.0) { 
        attack_power_ratio = -1.0;
    }
    double defense_power_ratio = 0.33 * (my_defense - goal_attack) / goal_attack; 
    if (1.0 < attack_power_ratio) { 
        attack_power_ratio = 1.0; 
    } else if (attack_power_ratio < -1.0) { 
        attack_power_ratio = -1.0;
    }

    double bonuses = 0; 

    if ((my_attack >= goal_defense) && (my_defense >= goal_attack)) { 


        bonuses +=  m * 0.5 * attack_power_ratio; 

    } else if ((my_attack >= goal_defense) && (my_defense < goal_attack)) { 



        if (rounds <= 1) { 
            bonuses +=  m * 0.5 * attack_power_ratio; 
        } else { 
            bonuses += m * 0.5 * defense_power_ratio; 
        }

    } else if ((my_attack < goal_defense) && (my_defense >= goal_attack)) { 


        bonuses +=  m * 0.5 * attack_power_ratio; 

    } else { 

        bonuses +=  m * 0.5 * min(attack_power_ratio, defense_power_ratio); 
    } 

    return bonuses; 
}

double ArmyAgent::PretestAttack(AiMain *ai,  const MapPointData &pos, const double value) 
{ 
  
  
  

    if (GetArmyContainsSettler()) {
        return BAD_UTILITY; 
    }

	
	MapPointData toughpos = pos;
	if (ai->m_world->IsCityHere(&toughpos))
		return BAD_UTILITY;













	
    if ((NULL == m_cargo) && (m_agent_strength->GetAttack() <= 0.0)) { 
        return BAD_UTILITY;
    } else { 
        return 1.0;
    }

} 










































































































double ArmyAgent::PretestBid(AiMain *ai, GOAL_TYPE goal, MapPointData &pos)
{    
    double give_up, give_up_threshold = 0.0; 
	BOOL army_can_enter = FALSE;
	BOOL cargo_can_enter = FALSE;

    
    

	
    if (GOAL_TYPE_RETREAT == goal) { 
        if (!IsMigratingParatroop()) {             
                return BAD_UTILITY; 
        } 
    } 

    switch (m_state) { 
    case AGENT_STATE_MARKED_DEAD:
    case AGENT_STATE_MARKED_SETTLE:
    case AGENT_STATE_MARKED_GARRISON:
	case AGENT_STATE_AT_GOAL:
        return BAD_UTILITY;     
		break;
    
	case AGENT_STATE_BUILDING: 
    case AGENT_STATE_UNBUILT:

       if (m_home_city_agent->GetProjectedNetProduction() < 1) { 
            return BAD_UTILITY; 
       }
        
       give_up = m_home_city_agent->EstimateCostToGiveUpCurrentBuild(ai, goal); 
       if (give_up_threshold <= give_up)  {      
            return BAD_UTILITY;                              
       }

	   if (ai->m_player->UnitTypeCanEnter(GetUnbuiltType(), &pos)) 
		   return 1.0;
	   else
		   return BAD_UTILITY; 

	   break;
    
    case AGENT_STATE_LOOKING_FOR_WORK: 
	case AGENT_STATE_GOTO_PERIMETER_DEST:
	case AGENT_STATE_GOTO_PATROL_DEST:
	case AGENT_STATE_GOTO_CHOKEPOINT_DEST:
    case AGENT_STATE_GOTO_EXPLORE_DEST:     
    case AGENT_STATE_GOTO_SETTLE_DEST:
    case AGENT_STATE_GOTO_ATTACK_DEST:
    case AGENT_STATE_GOTO_ENSLAVE_DEST:
    case AGENT_STATE_GOTO_RECRUITMENT_ATTACK_DEST:
    case AGENT_STATE_GOTO_RECRUITMENT_DEST:
		
		
		if (goal == GOAL_TYPE_BOMBARD)
		{
			uint32 target_move_type;
			BOOL r = target_move_type = ai->m_world->GetMoveType(
				&pos, 
				&target_move_type);
			Assert(r);
			return CanBombard(ai, target_move_type);
		}

		
		
		if (GotCargo()) 
		{ 
			
			if (CargoCanEnter(ai, pos))
				return 1.0;
			else
				return BAD_UTILITY;
		}
		else 
			if (ai->m_player->MoveTypeCanEnter(GetMoveIntersection(ai), &pos))
				return 1.0;

		return BAD_UTILITY;
    default: 
        INSANE(PRETEST_BID_UNKNOWN); 
        return BAD_UTILITY; 
    }
}  


BOOL ArmyAgent::PretestContinentReachable(AiMain *ai, MapPointData &dest_pos, 
    BOOL &is_same_cont, BOOL &is_space_dest) 
{ 
      
    

    sint32 dest_cont=-3; 
    BOOL dest_is_land=FALSE;

    is_same_cont= FALSE; 
    is_space_dest = FALSE; 
    
    if ((m_my_pos->z == GROUND_Z) && (dest_pos.z == GROUND_Z)) { 

        
 	    if (!ai->m_continents->GetContinent(ai, dest_pos, dest_cont, dest_is_land)) { 
            return FALSE;
        } 

        if ((dest_is_land == m_my_is_land) && (m_my_continent == dest_cont)) { 
            is_same_cont = TRUE; 
            return TRUE; 
        } 


        if (CanMoveAir(ai)) { 
            return TRUE; 
        } 

        
        if (m_my_is_land && ai->m_continents->LandGotSpaceLauncher(m_my_continent)) { 
            return TRUE; 
        } 

        if (GotCargo()) { 

            if (BoatCanReachShore(ai, dest_pos)) { 
                return TRUE;
            } else { 
				return FALSE; 
			} 
        }  else { 

            if (m_my_is_land != dest_is_land) { 
                return FALSE;   
            } else { 
                return TRUE; 
            } 
        }

    } else if ((m_my_pos->z == GROUND_Z) && (dest_pos.z == SPACE_Z)) { 

        

        if (CanSpaceLaunch(ai)) { 
            is_space_dest = TRUE; 
            return TRUE; 
        } else { 
            if (m_my_is_land && ai->m_continents->LandGotSpaceLauncher(m_my_continent)) { 
                is_space_dest = TRUE; 
                return TRUE; 
            } else { 
                return FALSE; 
            }
        } 

    } else if ((m_my_pos->z == SPACE_Z) && (dest_pos.z == GROUND_Z)) { 
        if (CanSpaceLand(ai)) { 
            return TRUE; 
        } else if (!CanMoveSpace(ai)) { 
            return TRUE; 
        } else { 
            return FALSE; 
        }

    } else if ((SPACE_Z == m_my_pos->z) && (SPACE_Z == dest_pos.z)) { 
        is_space_dest = TRUE; 
        return TRUE; 
    } 

    INSANE(Z_ILLIGAL); 
    return FALSE; 
} 




double ArmyAgent::ActionBidToEnslave(AiMain *ai, GoalEnslave* goal_enslave)
{ 
    MapPointData dest_pos; 
    goal_enslave->GetPos(dest_pos); 
    if (dest_pos.z != GROUND_Z) { 
        return BAD_UTILITY; 
    } 

    if (!PretestFuelToTarget(ai, dest_pos)) { 
        return BAD_UTILITY; 
    }

    if (IsWounded()) return BAD_UTILITY; 

    BOOL is_same_cont;
    BOOL is_space_dest;
    if (!PretestContinentReachable(ai, dest_pos, is_same_cont, is_space_dest)) { 
        return BAD_UTILITY; 
    } 

    if (PretestBid(ai, GOAL_TYPE_ENSLAVE, dest_pos) < (BAD_UTILITY + 0.5)) { 
        return BAD_UTILITY; 
    } 
    sint32 dest_cont; 
    BOOL dest_is_land; 
    if (PretestIsSameCont(ai, dest_pos, dest_cont, dest_is_land) < 0.0) { 
        return BAD_UTILITY; 
    }       

    if (AGENT_STATE_UNBUILT == m_state) { 
        if (0 < m_home_city_agent->GetQueueLen())  { 
            return BAD_UTILITY; 
        } 
    }

    if (!CanEnslave(ai)) { 
        return BAD_UTILITY; 
    } 

    if (GotCargo()) { 
        return BAD_UTILITY; 
    } 

    if (GetArmyContainsSettler()) {
        return BAD_UTILITY; 
    }

    double move_point_cost;
    sint32 rounds; 
    double build_time;
    if (BidDistance(ai, FALSE, dest_pos, move_point_cost, rounds, build_time) < 0.0) { 
        return BAD_UTILITY; 
    }

	
	if (ai->m_my_player_id == PLAYER_INDEX_VANDALS &&
		rounds > ai->m_planner->the_AIP.wander_rounds_from_target)
		return BAD_UTILITY;

	
	double high_movement_weight = .25;
	double high_attack_weight = .75;
	double bonuses = 0.0;

	
	sint32 best_attack;
	sint32 my_attack  = GetBestAttack(ai);
	if (ai->m_science_agent->GetBestLandAttack(best_attack))
		bonuses += (my_attack / best_attack)
			* ai->m_planner->the_AIP.max_misc_bid_bonus
			* high_attack_weight;
	else
		; 

    
	sint32 best_move;
	double my_move = GetBestMove(ai);
	if (ai->m_science_agent->GetBestLandMove(best_move))
		bonuses += (my_move / best_move)
			* ai->m_planner->the_AIP.max_misc_bid_bonus
			* high_movement_weight;
	else
		; 

	
	double time_term;
    
	double tile_count = ai->m_map->WrappedDistance(*m_my_pos, dest_pos);
	time_term = ( rounds * rounds 
		* ai->m_planner->the_AIP.distance_from_unit_priority_modifier )
		- tile_count;

	return bonuses + time_term + goal_enslave->Compute_Raw_Priority(ai); 
}	

BOOL ArmyAgent::CanEnslave(AiMain *ai) const
{
    sint16 unit_idx;

    for (unit_idx=0; unit_idx<m_nUnits; unit_idx++) { 
        if (ai->m_unitDB->CanConductSlaveRaids(m_unit_type[unit_idx])) { 
            return TRUE; 
        } 
    } 

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx =0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanConductSlaveRaids(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 


    return FALSE; 
}

BOOL ArmyAgent::CanConvertCity(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanConvertCity(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanConvertCity(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanBioterrorCity(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanBioterrorCity(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanBioterrorCity(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanNanoattackCity(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanNanoattackCity(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanNanoattackCity(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}



















BOOL ArmyAgent::CanBonusFood(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->GetBonusFood(m_unit_type[unit_idx])>0)
				return TRUE;
		}

	

	return FALSE;
}

BOOL ArmyAgent::CanMakePark(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanCreateParks(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanCreateParks(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanCauseUnhappiness(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanCauseUnhappiness(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanCauseUnhappiness(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanEstablishEmbassy(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanEstablishEmbassy(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanEstablishEmbassy(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 


	return FALSE;
}

BOOL ArmyAgent::CanCreateFranchise(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanCreateFranchise(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanCreateFranchise(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanStealTechnology(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanStealTechnology(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanStealTechnology(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanInciteRevolution(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanInciteRevolution(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanInciteRevolution(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanInjoin(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanInjoin(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanInjoin(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanSueFranchise(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanSue(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanSue(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanAssasinateRuler(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanAssasinateRuler(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanAssasinateRuler(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanUndergroundRailway(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanConductUndergroundRailway(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanConductUndergroundRailway(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanSoothsay(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanSoothsay(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanSoothsay(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanConductHits(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanConductHits(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanConductHits(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanAdvertise(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanAdvertise(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanAdvertise(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;

}


BOOL ArmyAgent::CanPlantNuke(AiMain *ai) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanPlantNuke(m_unit_type[unit_idx]))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanPlantNuke(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}


BOOL ArmyAgent::CanAttackFromSpaceShip(AiMain *ai)
{
    sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++) {
        if (ai->m_unitDB->CanAttackFromSpaceShip(m_unit_type[unit_idx])) { 
			return TRUE;
        }
	}

    sint32 n = m_cargo->Num(); 
    for (unit_idx=0; unit_idx<n; unit_idx++) { 
        if (ai->m_unitDB->CanAttackFromSpaceShip(m_cargo->Access(unit_idx))) { 
            return TRUE;
        } 
    }

    return FALSE; 

}

BOOL ArmyAgent::CanBeachAssaultAtAll(AiMain *ai)
{
    sint32 unit_idx; 
    for (unit_idx=0; unit_idx<m_nUnits; unit_idx++) { 
        if (ai->m_unitDB->CanBeachAssaultAtAll(m_unit_type[unit_idx])) { 
			return TRUE;
        }
	}

    sint32 n = m_cargo->Num(); 
    for (unit_idx=0; unit_idx<n; unit_idx++) { 
        if (ai->m_unitDB->CanBeachAssaultAtAll(m_cargo->Access(unit_idx))) { 
            return TRUE;
        } 
    }

    return FALSE;
}


BOOL ArmyAgent::CanBeachAssault(AiMain *ai, const MapPointData &pos) const
{
	sint16 unit_idx;
	MapPointData tough_pos = pos;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanBeachAssault(m_unit_type[unit_idx], &tough_pos))
				return TRUE;
		}

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->CanBeachAssault(m_cargo->Access(unit_idx), &tough_pos)) { 
                return TRUE;
            } 
        }
    } 

	return FALSE;
}

BOOL ArmyAgent::CanProbeWormhole(AiMain *ai) const
{
    sint16 unit_idx;

	
    for (unit_idx=0; unit_idx<m_nUnits; unit_idx++) { 
        if (ai->m_unitDB->IsWormholeProbe(m_unit_type[unit_idx])) 
			{ 
				return TRUE; 
			} 
    } 
	

    return FALSE; 
}


BOOL ArmyAgent::IsStealthy(AiMain *ai) const
{
    sint16 unit_idx;

	
    for (unit_idx=0; unit_idx<m_nUnits; unit_idx++) { 
        if (!ai->m_unitDB->IsStealthy(m_unit_type[unit_idx])) 
			{ 
				return FALSE; 
			} 
    } 

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (!ai->m_unitDB->IsStealthy(m_cargo->Access(unit_idx))) { 
                return FALSE;
            } 
        }
    } 

    return TRUE; 
}


BOOL ArmyAgent::CanBeRustled(AiMain *ai) const
{
    sint16 unit_idx;

	
    for (unit_idx=0; unit_idx<m_nUnits; unit_idx++) { 
        if (ai->m_unitDB->CanBeRustled(m_unit_type[unit_idx]))
			{ 
				return TRUE; 
			} 
    } 

	

    return FALSE; 
}

BOOL ArmyAgent::CanEntrench(AiMain *ai) const
{
    sint16 unit_idx;

	
    for (unit_idx=0; unit_idx<m_nUnits; unit_idx++) { 
        if (!ai->m_unitDB->CanEntrench(m_unit_type[unit_idx])) 
			{ 
				return FALSE; 
			} 
    } 

	

    return TRUE; 
}

BOOL ArmyAgent::CanBombard(AiMain *ai, const uint32 move_type) const
{
	sint16 unit_idx;

	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			
			if (move_type == 0xffffffff &&
				ai->m_unitDB->IsBombard(unit_idx))
				return TRUE;
							
			
			if (((move_type & k_BIT_MOVEMENT_TYPE_LAND) &&
				 ai->m_unitDB->IsLandBombard(m_unit_type[unit_idx])) ||
				((move_type & k_BIT_MOVEMENT_TYPE_MOUNTAIN) &&
				 ai->m_unitDB->IsMountainBombard(m_unit_type[unit_idx])) ||
				((move_type & k_BIT_MOVEMENT_TYPE_AIR) &&
				 ai->m_unitDB->IsAirBombard(m_unit_type[unit_idx])) ||
				((move_type & k_BIT_MOVEMENT_TYPE_WATER) &&
				 ai->m_unitDB->IsWaterBombard(m_unit_type[unit_idx])) ||
				((move_type & k_BIT_MOVEMENT_TYPE_SPACE) &&
				ai->m_unitDB->IsSpaceBombard(m_unit_type[unit_idx])))
				return TRUE;
		}

	

	return FALSE;
}


BOOL ArmyAgent::CanBombardTarget(AiMain *ai, const MapPointData &pos ) const
{
	MapPointData tough_pos = pos;
	BOOL is_unknown_id;

 	if (ai->m_player->CanBombard (m_id.GetVal(), &is_unknown_id, 
								  &tough_pos))
		return TRUE;

	return FALSE;
}

BOOL ArmyAgent::CanSue(AiMain *ai) const
{
	sint16 unit_idx;
	for (unit_idx=0; unit_idx<m_nUnits; unit_idx++)
		{
			if (ai->m_unitDB->CanSue(m_unit_type[unit_idx]))
				return TRUE;
		}

	return FALSE;
}


BOOL ArmyAgent::ActionTryToEntrench(AiMain *ai)
{
	
    BOOL is_unknown_id; 

	
	if (CanEntrench(ai))
		{
			
			return ai->m_player->ExecuteAIOrder(m_id.GetVal(),
										 &is_unknown_id, 
										 m_my_pos, 
										 UNIT_ORDER_ENTRENCH);
		}
	return FALSE;
}


BOOL ArmyAgent::ActionTryToEnslave(AiMain *ai, MapPointData &dummy_pos, 
    GoalEnslave * goal_obj)
{    
    BOOL is_unknown_id; 
    MapPointData target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_SLAVE_RAID);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    goal_obj->GetPos(target_pos); 
    m_state = AGENT_STATE_LOOKING_FOR_WORK; 
    if (ai->m_player->IsSlaveRaidPossible (m_id.GetVal(), &is_unknown_id, 
        &target_pos)) { 

        return ai->m_player->AttemptSlaveRaid (m_id.GetVal(), &is_unknown_id, 
          &target_pos);
    } else { 
        return FALSE; 
    } 
}

BOOL ArmyAgent::ActionTryToRustle(AiMain *ai, MapPointData &dummy_pos, 
    ArmyGoal * goal_obj)
{    
	Assert(FALSE);
	return FALSE; 
} 

BOOL ArmyAgent::ActionTryToPillage(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_PILLAGE);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (ai->m_player->CanPillage (m_id.GetVal(), &is_unknown_id, 
								  &tough_pos)) 
		{ 
			return ai->m_player->Pillage( m_id.GetVal(), &is_unknown_id, 
										 &tough_pos);
		}
	else
        return FALSE; 
} 

BOOL ArmyAgent::ActionTryToPirate(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_INTERCEPT_TRADE);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;


    if (ai->m_player->CanPirate (m_id.GetVal(), &is_unknown_id, 
								  &tough_pos)) 
		{ 
			return ai->m_player->Pirate( m_id.GetVal(), &is_unknown_id, 
										 &tough_pos);
		}
	else
        return FALSE; 
} 

BOOL ArmyAgent::ActionTryToSueFranchise(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
	MapPointData tough_pos = target_pos;

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_SUE_FRANCHISE);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (ai->m_player->CanSueFranchise (m_id.GetVal(), &is_unknown_id, 
										   &tough_pos)) 
		{ 
			return ai->m_player->SueFranchise (m_id.GetVal(), &is_unknown_id, 
											   &tough_pos);
		}
	else
        return FALSE; 
} 

BOOL ArmyAgent::ActionTryToReformCity(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
	MapPointData tough_pos = target_pos;

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_REFORM);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (ai->m_player->CanReformCity (m_id.GetVal(), &is_unknown_id, 
										   &tough_pos)) 
		{ 
			return ai->m_player->ReformCity(m_id.GetVal(), &is_unknown_id, 
											&tough_pos);
		}
	else
        return FALSE; 
} 

BOOL ArmyAgent::ActionTryToBombard(AiMain *ai, MapPointData &dummy_pos, 
    ArmyGoal * goal_obj)
{    
    BOOL is_unknown_id; 
    MapPointData target_pos; 

    goal_obj->GetPos(target_pos); 
    if (ai->m_player->CanBombard (m_id.GetVal(), &is_unknown_id, 
								  &target_pos)) 
		{ 
			return ai->m_player->Bombard( m_id.GetVal(), &is_unknown_id, 
										  &target_pos);
		}
	else
        return FALSE; 
} 

BOOL ArmyAgent::ActionTryToConvert(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
	MapPointData tough_pos = target_pos;

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_CONVERT);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (ai->m_player->CanConvertCity (m_id.GetVal(), &is_unknown_id, 
								  &tough_pos)) 
		{ 
			return ai->m_player->ConvertCity (m_id.GetVal(), &is_unknown_id, 
										  &tough_pos);
		}
	else
        return FALSE; 
} 

















BOOL ArmyAgent::ActionTryToPlantNuke(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost( UNIT_ORDER_PLANT_NUKE);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (ai->m_player->AbleToPlantNukeTarget (m_id.GetVal(), &is_unknown_id, 
											   &tough_pos)) 
		{ 
			return ai->m_player->ExecuteAIOrder(m_id.GetVal(), &is_unknown_id, 
										  &tough_pos, UNIT_ORDER_PLANT_NUKE);
		}
	else
        return FALSE; 
} 

BOOL ArmyAgent::ActionTryToMakePark(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost( UNIT_ORDER_CREATE_PARK);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (ai->m_player->AbleToMakeParkTarget (m_id.GetVal(), &is_unknown_id, 
											&tough_pos))
		{ 
			return ai->m_player->ExecuteAIOrder(m_id.GetVal(), &is_unknown_id, 
										  &tough_pos, UNIT_ORDER_CREATE_PARK);
		}
	else
        return FALSE; 
} 

BOOL ArmyAgent::ActionTryToUndergroundRailway(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 
	uint32 order;

	
	sint32 cost = ai->m_gs->GetOrderGoldCost( UNIT_ORDER_UNDERGROUND_RAILWAY);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (ai->m_player->AbleToUndergroundRailwayTarget (m_id.GetVal(), &is_unknown_id, 
													  &tough_pos)) 
		{ 
			
			if (ai->m_world->GetCitySlaveCount(&tough_pos) > 3)
				order = UNIT_ORDER_INCITE_UPRISING;
			else
				order = UNIT_ORDER_UNDERGROUND_RAILWAY;

			return ai->m_player->ExecuteAIOrder(m_id.GetVal(), 
												&is_unknown_id, 
												&tough_pos, 
												order);	
		}
	else
        return FALSE; 
}

BOOL ArmyAgent::ActionTryToEstablishEmbassy(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_ESTABLISH_EMBASSY);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (ai->m_player->AbleToEstablishEmbassyTarget (m_id.GetVal(), &is_unknown_id, 
													&tough_pos)) 
		{ 
			return ai->m_player->ExecuteAIOrder(m_id.GetVal(), &is_unknown_id, 
										  &tough_pos, UNIT_ORDER_ESTABLISH_EMBASSY);
		}
	else
        return FALSE; 
}

BOOL ArmyAgent::ActionTryToCreateFranchise(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_FRANCHISE);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (ai->m_player->AbleToCreateFranchiseTarget (m_id.GetVal(), &is_unknown_id, 
												   &tough_pos)) 
		{ 
			return ai->m_player->ExecuteAIOrder(m_id.GetVal(), &is_unknown_id, 
										  &tough_pos, UNIT_ORDER_FRANCHISE);
		}
	else
        return FALSE; 
}

BOOL ArmyAgent::ActionTryToAssasinateRuler(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_ASSASSINATE);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (ai->m_player->AbleToAssasinateRulerTarget (m_id.GetVal(), &is_unknown_id, 
												   &tough_pos)) 
		{ 
			return ai->m_player->ExecuteAIOrder(m_id.GetVal(), &is_unknown_id, 
												&tough_pos, UNIT_ORDER_ASSASSINATE);
		}
	else
        return FALSE; 
}

BOOL ArmyAgent::ActionTryToStealTechnology(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_STEAL_TECHNOLOGY);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (ai->m_player->AbleToStealTechnologyTarget(m_id.GetVal(), &is_unknown_id, 
												   &tough_pos)) 
		{ 
			return ai->m_player->ExecuteAIOrder(m_id.GetVal(), &is_unknown_id, 
												&tough_pos, UNIT_ORDER_STEAL_TECHNOLOGY);
		}
	else
        return FALSE; 
}

BOOL ArmyAgent::ActionTryToInjoin(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_INJOIN);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (ai->m_player->AbleToInjoinTarget (m_id.GetVal(), &is_unknown_id, 
								  &tough_pos)) 
		{ 
			return ai->m_player->ExecuteAIOrder(m_id.GetVal(), &is_unknown_id, 
												&tough_pos, UNIT_ORDER_INJOIN);
		}
	else
        return FALSE; 
}

BOOL ArmyAgent::ActionTryToInciteRevolution(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_INCITE_REVOLUTION);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;


    if (ai->m_player->AbleToInciteRevolutionTarget (m_id.GetVal(), &is_unknown_id, 
													&tough_pos)) 
		{ 
			return ai->m_player->ExecuteAIOrder(m_id.GetVal(), &is_unknown_id, 
												&tough_pos, UNIT_ORDER_INCITE_REVOLUTION);
		}
	else
        return FALSE; 
} 

BOOL ArmyAgent::ActionTryToCauseUnhappiness(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 
	sint32 cost;

    if (ai->m_player->AbleToCauseUnhappinessTarget (m_id.GetVal(), 
													&is_unknown_id, 
													&tough_pos)) 
		{ 
			
			
			
			
			

			if (CanSoothsay(ai))
				{
					
					cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_SOOTHSAY);
					if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
						return FALSE;

					return ai->m_player->
						ExecuteAIOrder(m_id.GetVal(), 
									   &is_unknown_id,
									   &tough_pos, 
									   UNIT_ORDER_SOOTHSAY);
				}

#if 0
			
			if (CanConductHits(ai))
				{
					
					cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_CONDUCT_HIT);
					if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
						return FALSE;

					return ai->m_player->
						ExecuteAIOrder(m_id.GetVal(), 
									   &is_unknown_id,
									   &tough_pos, 
									   UNIT_ORDER_CONDUCT_HIT);
				}
#endif

			if (CanAdvertise(ai))
				{
					
					cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_ADVERTISE);
					if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
						return FALSE;

					return ai->m_player->
						ExecuteAIOrder(m_id.GetVal(), 
									   &is_unknown_id,
									   &tough_pos, 
									   UNIT_ORDER_ADVERTISE);
				}

			BOOL CAUSE_UNHAPPINESS_ORDER_NOT_FOUND = FALSE;
			Assert(CAUSE_UNHAPPINESS_ORDER_NOT_FOUND);
		}

	return FALSE; 
} 

BOOL ArmyAgent::ActionTryToExpel(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	if (ai->m_player->AbleToExpelTarget (m_id.GetVal(),
		&is_unknown_id, 
		&tough_pos))
	{
		
		sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_EXPEL);
		if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
			return FALSE;

		return ai->m_player->ExecuteAIOrder(m_id.GetVal(), &is_unknown_id, 
			&tough_pos, UNIT_ORDER_EXPEL);
	}
	else if (ai->m_player->AbleToSueTarget (m_id.GetVal(),
		&is_unknown_id, 
		&tough_pos))
	{
		
		sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_SUE);
		if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
			return FALSE;

	    return ai->m_player->ExecuteAIOrder(m_id.GetVal(), &is_unknown_id, 
													&tough_pos, UNIT_ORDER_SUE);
	}
    return FALSE; 
} 

BOOL ArmyAgent::ActionTryToBioterrorCity(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_BIO_INFECT);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (CanBioterrorCity(ai)) { 

        sint32 defense_owner;
        uint32 c_id; 
        BOOL get_city_ok = ai->m_world->GetCityId (&tough_pos, &defense_owner, &c_id); 
        Assert(get_city_ok); 
		if (get_city_ok) {
			Assert(ai->m_foreigner[defense_owner]);
			BSet<ForeignCity> *known_cities = ai->m_foreigner[defense_owner]->GetKnownCities(); 
			Assert(known_cities); 
			ForeignCity *the_city = known_cities->Find(c_id);
			Assert(the_city); 
			if (the_city) {
				the_city->SetBeenNanoAttacked(); 
			}
			return ai->m_player->ExecuteAIOrder(m_id.GetVal(), &is_unknown_id, 
													&tough_pos, UNIT_ORDER_BIO_INFECT);
		} else {
			return FALSE;
		}
    } else {
        return FALSE; 
    }
} 

BOOL ArmyAgent::ActionTryToNanoattackCity(AiMain *ai, const MapPointData &target_pos)
{    
    BOOL is_unknown_id; 
    MapPointData tough_pos = target_pos; 

	
	sint32 cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_NANO_INFECT);
	if (!ai->m_gold->CanSpendSpecialAction(ai, cost))
		return FALSE;

    if (CanNanoattackCity(ai)) { 

        sint32 defense_owner;
        uint32 c_id; 
        BOOL get_city_ok = ai->m_world->GetCityId (&tough_pos, &defense_owner, &c_id); 
        Assert(get_city_ok); 
		if (get_city_ok) {
			Assert(ai->m_foreigner[defense_owner]);
			BSet<ForeignCity> *known_cities = ai->m_foreigner[defense_owner]->GetKnownCities(); 
			Assert(known_cities); 
			ForeignCity *the_city = known_cities->Find(c_id);
			Assert(the_city); 
			if (the_city) {
				the_city->SetBeenNanoAttacked(); 
			}
			return ai->m_player->ExecuteAIOrder(m_id.GetVal(), &is_unknown_id, 
						&tough_pos, UNIT_ORDER_NANO_INFECT);
		} else {
			return FALSE;
		}
	}
	else
        return FALSE; 
} 



BOOL ArmyAgent::ActionTryToBackAway(AiMain *ai)
{
	
	BOOL did_move = FALSE;
	MapPointData prev;
	if (m_path_cur_nPoints > 1 &&
		m_path_cur > 0)
		{
			
			prev = m_path[m_path_cur-1];
			m_path[0] = m_path[m_path_cur];   
			m_path[1] = prev; 
			m_path_cur = 0;
			m_path_cur_nPoints = 2; 
			m_pathValid = FALSE;    
			GotoNextPathPoint(ai, did_move);
		}

	return did_move;
}


void ArmyAgent::RemoveLastPathPoint()
{
	
	
    if (1 < m_path_cur_nPoints) { 
        m_path_cur_nPoints--;
    }
}


sint32 ArmyAgent::GetBestAttack(AiMain *ai)
{
	sint32 best_attack = 0;
	sint32 attack;
	sint32 unit_type;
    for (int i=0; i < GetUnitCount(); i++)
		{ 
			unit_type = GetUnitTypeByNumber(i); 
			attack = ai->m_unitDB->GetAttack(unit_type);
			if ( attack > best_attack )
				best_attack = attack;
		} 
	return best_attack;
}

sint32 ArmyAgent::GetBestDefense(AiMain *ai)
{
	sint32 best_defense = 0;
	sint32 defense;
	sint32 unit_type;
    for (int i=0; i < GetUnitCount(); i++)
		{ 
			unit_type = GetUnitTypeByNumber(i); 
			defense = ai->m_unitDB->GetDefense(unit_type);
			if ( defense > best_defense )
				best_defense = defense;
		} 
	return best_defense;
}

double ArmyAgent::GetBestMove(AiMain *ai)
{
	double best_move = 0.0;
	double move;
	sint32 unit_type;
    for (int i=0; i < GetUnitCount(); i++)
		{ 
			unit_type = GetUnitTypeByNumber(i); 
			move = ai->m_unitDB->UnitTypeMovementPoints(unit_type);
			if ( move > best_move )
				best_move = move;
		} 
	return best_move;	
}



















double ArmyAgent::BidDistance(AiMain *ai, BOOL will_be_transported, 
     MapPointData &pos, double &move_point_cost, sint32 &rounds, 
     double &build_time)
{
    sint32 is_unbuilt; 

    rounds = 0;
    move_point_cost = 0.0; 

    if (!will_be_transported) { 
        if (m_my_pos->z != pos.z) { 
            if (m_my_pos->z == GROUND_Z) { 
                if (!CanSpaceLaunch(ai) &&
                    !CanReachLauncher(ai)) { 
                    return BAD_UTILITY; 
                } 
            } else { 
                if (!CanSpaceLand(ai) && CanMoveSpace(ai)) { 

                    return BAD_UTILITY; 
                }
            }
        }
    }
              

    if (m_state == AGENT_STATE_BUILDING) { 
        rounds += m_home_city_agent->EstimateRoundsToFinished(ai, m_id); 
    }
    
    if (m_state == AGENT_STATE_UNBUILT) { 
        rounds += m_home_city_agent->EstimateRoundsToEmptyQueue(ai); 
        rounds += m_home_city_agent->EstimateRoundsToBuildUnit(ai, GetUnbuiltType()); 
    } 
    build_time = rounds; 

    
    if (IsEqual(*m_my_pos, pos)) { 
        move_point_cost = 0.0; 
        return 0.0; 
    } else { 
        
		
        
        
        is_unbuilt = ((m_state == AGENT_STATE_BUILDING) || 
            (m_state == AGENT_STATE_UNBUILT)); 
        
        
       
        move_point_cost =  100 * ai->m_map->WrappedDistance(*m_my_pos, pos);    
    }
    
    rounds +=  MovePoints2Rounds(ai,  move_point_cost); 
   
    return 1.0; 
}    


AiMain *g_trans_ai; 
double g_trans_max_r; 
sint32 g_trans_dest_cont;
BOOL g_trans_can_space_assault;
BOOL g_trans_can_beach_assault;
BOOL g_trans_can_capture_city;

BOOL TransportPathCallback (BOOL can_enter,  MapPointData *prev,  
   MapPointData *pos, float *cost, BOOL is_zoc, ASTAR_ENTRY_TYPE &entry)
{ 
	
	static const float incursion_cost = 
		g_trans_ai->m_science_agent->ComputeMaxMoveCost(g_trans_ai) * 2;

    if (can_enter) { 

	    
	    

        sint32 cont; 
        BOOL is_land; 
	    BOOL r; 
	    BOOL wrong_cont;
    
        if (GROUND_Z == pos->z) { 
            r = g_trans_ai->m_continents->GetContinent(g_trans_ai, *pos, cont, is_land);
            Assert(r); 
		    
		    
            wrong_cont = ( cont != g_trans_dest_cont) &&
			    !g_trans_ai->m_world->IsCityHere(pos);
        } else {
            wrong_cont = FALSE; 
        } 

	    
        
        sint32 player_id;
        uint32 a_id;
        uint32 c_id; 
        sint32 top_unit_type;
        sint32 unit_num;

        BOOL occupied = FALSE; 

        if (g_trans_ai->m_world->GetArmyId(pos, &player_id, &a_id, &top_unit_type, &unit_num)) { 

            if (prev->z != pos->z) {  
                *cost = k_ASTAR_BIG;
			    entry = ASTAR_RETRY_DIRECTION; 
		        return FALSE;
            } 

            if (g_trans_ai->m_my_player_id != player_id) { 
                if (pos->z == SPACE_Z) { 
                   occupied = !g_trans_can_space_assault; 
                } else { 
                   occupied = !(g_trans_can_space_assault || g_trans_can_beach_assault); 
                }
            } 
        } else if (g_trans_ai->m_world->GetCityId (pos, &player_id, &c_id)) { 

            if (prev->z != pos->z) { 
                *cost = k_ASTAR_BIG;
			    entry = ASTAR_RETRY_DIRECTION; 
		        return FALSE;
            } 

            if (g_trans_ai->m_my_player_id != player_id) { 
                occupied = !(g_trans_can_space_assault || 
                    g_trans_can_beach_assault || 
                    g_trans_can_capture_city); 
            }             
        }  

		if (occupied || wrong_cont) {
	        
            if (is_land && 
				(g_trans_ai->m_world->IsMoveTypeWater(prev) ||
				 g_trans_ai->m_world->IsMoveTypeShallowWater(prev) ||
				 g_trans_ai->m_world->IsCityHere(prev))) { 
                *cost = k_ASTAR_BIG;
			    entry = ASTAR_RETRY_DIRECTION; 
		        return FALSE;
            } else if (pos->z == SPACE_Z) { 
                *cost = k_ASTAR_BIG;
			    entry = ASTAR_RETRY_DIRECTION; 
		        return FALSE;
            } 
        }

        if (g_trans_ai->m_world->IsMoveTypeWater(pos) || g_trans_ai->m_world->IsMoveTypeShallowWater(pos)) { 
            *cost *= float(g_trans_max_r);
        } 
        
		
		if (g_trans_ai->GetLeaveOurLands())	{
			sint32 cell_owner = 
				g_trans_ai->m_world->GetCellOwner(pos);
			if ((cell_owner > 0) && (cell_owner < k_MAX_PLAYERS)) { 
				if (g_trans_ai->m_foreigner[cell_owner]) { 
					
					
					if (g_trans_ai->m_foreigner[cell_owner]->IsLeaveOurLands() && 
						k_REGARD_HOTWAR <= g_trans_ai->m_foreigner[cell_owner]->GetUnitRegard()) { 
						*cost += incursion_cost;
					} 
				}
			}
		}
		






        return TRUE; 
    } else { 
        *cost = k_ASTAR_BIG; 
		entry = ASTAR_BLOCKED; 
        return FALSE;
    }
}

#define MAX_TILE_DEFENSE 3.0
BOOL DefensivePathCallback (BOOL can_enter,  MapPointData *prev,  
   MapPointData *pos, float *cost, BOOL is_zoc, ASTAR_ENTRY_TYPE &entry)
{ 
	
	static const float incursion_cost = 
		g_trans_ai->m_science_agent->ComputeMaxMoveCost(g_trans_ai)* 2;

    if (can_enter) { 
        
		
		if (g_trans_ai->GetLeaveOurLands())	{
			sint32 cell_owner = 
				g_trans_ai->m_world->GetCellOwner(pos);
			if ((cell_owner > 0) && (cell_owner < k_MAX_PLAYERS)) { 
	            if (g_trans_ai->m_foreigner[cell_owner]) { 
					
					
					if (g_trans_ai->m_foreigner[cell_owner]->IsLeaveOurLands() && 
						k_REGARD_HOTWAR <= g_trans_ai->m_foreigner[cell_owner]->GetUnitRegard()) { 
						*cost += incursion_cost;
					} 
				}
			}
		}

























        return TRUE; 
    } else { 
        *cost = k_ASTAR_BIG; 
		entry = ASTAR_ENTRY_TYPE(0); 
        return FALSE;
    }
}


















BOOL ArmyAgent::FindPathRoundTrip(AiMain *ai, MapPointData &pos, 
     sint32 &pathtime, const sint32 & max_rounds, BOOL check_rail_launcher, 
     sint32 cutoff, sint32 &nodes_opened)
{
   
    float total_cost; 
	sint32 movement_rounds;

    pathtime = GetTickCount(); 
    
    if ((m_my_pos->x == pos.x) &&
        (m_my_pos->y == pos.y) && 
        (m_my_pos->z == pos.z)) { 
        m_path_cur_nPoints = 1;
        m_path_cur = 0; 
        m_path[0].x = m_my_pos->x; 
        m_path[0].y = m_my_pos->y; 
        m_path[0].z = m_my_pos->z; 
        return TRUE; 
    }
    
    BOOL is_unbuilt = (m_state == AGENT_STATE_UNBUILT) || (m_state == AGENT_STATE_BUILDING); 
    
    BOOL is_unknown_id=FALSE;

	
    if (GotCargo() || is_unbuilt)
		return false;


	if (ai->m_robot_astar->FindPath(DefensivePathCallback, 
									m_id.GetVal(), 
									PATH_ARMY_TYPE_EXISTS, 
									0, 
									m_my_pos, 
									&pos, 
									&m_path_total_nPoints , 
									&m_path, 
									&m_path_cur_nPoints , 
									&total_cost, 
									CanSpaceLaunch(ai), 
									CanSpaceLand(ai), 
									check_rail_launcher, 
                                    FALSE, 
                                    cutoff, 
                                    nodes_opened,
									TRUE, 
                                    FALSE, 
                                    TRUE))
		{




            Assert(0 < m_path_cur_nPoints); 
#ifdef _DEBUG
	ValidatePath(ai, GOAL_TYPE_COUNTER_STEALTH);
#endif

			movement_rounds = 
				MovePoints2Rounds(ai,  total_cost*2);

			if (movement_rounds > max_rounds)
				return FALSE;

			MakePathRoundTrip();			
			m_path_cur = 0; 
			m_pathValid = TRUE;

			return TRUE; 
		} 
	else 
		{ 

			m_pathValid = FALSE;
			return FALSE; 
		}

}












BOOL ArmyAgent::UseOldPath(AiMain *ai, MapPointData &pos, const GOAL_TYPE what_goal)
{ 
    MapPointData xy_pos;
	ai->m_player->Norm2XY(pos, xy_pos); 
	double cur_threat =
		ai->m_planner->the_strategic_map->Get_Threat(xy_pos);

    
	
	if (!(m_pathValid && 
		m_path[m_path_cur_nPoints-1] == pos &&
		cur_threat >= m_pathThreat * 0.75  &&
		cur_threat <= m_pathThreat * 1.25 )) {

  			
			m_pathThreat = cur_threat; 

			
			return FALSE;
		}

    sint32 player_id;
	uint32 a_id;
    sint32 top_unit_type; 
	sint32 unit_num;
    uint32 c_id;
    BOOL unitVis;
    BOOL cityVis;
    BOOL mineVis;
    sint32 last_point; 
    BOOL can_be_expelled; 

    switch (what_goal) { 
    case GOAL_TYPE_SEIGE:  
    case GOAL_TYPE_ATTACK: 
    case GOAL_TYPE_SALLY:
    case GOAL_TYPE_BOMBARD:

    case GOAL_TYPE_ENSLAVE: 
    case GOAL_TYPE_CONVERT_CITY:
    case GOAL_TYPE_BIOTERROR_CITY:
    case GOAL_TYPE_NANOATTACK_CITY:
    case GOAL_TYPE_HARASS_CITY:
    case GOAL_TYPE_EXPEL:
    case GOAL_TYPE_HURT_ARMY:
    case GOAL_TYPE_COUNTER_STEALTH:
    case GOAL_TYPE_PLANT_NUKE:
    case GOAL_TYPE_MAKE_PARK:
    case GOAL_TYPE_CAUSE_UNHAPPINESS:
    case GOAL_TYPE_ESTABLISH_EMBASSY:
    case GOAL_TYPE_FRANCHISING:
    case GOAL_TYPE_STEAL_TECHNOLOGY:
    case GOAL_TYPE_INCITE_REVOLUTION:
    case GOAL_TYPE_INJOIN:
    case GOAL_TYPE_ASSASINATE_RULER:
    case GOAL_TYPE_UNDERGROUND_RAILWAY:
    case GOAL_TYPE_NUKE_CITY:
  
    case GOAL_TYPE_REFORM_CITY:
    case GOAL_TYPE_SUE_FRANCHISE:
        last_point = m_path_cur_nPoints-1; 
		break;
    default:
        last_point = m_path_cur_nPoints; 
    }

    
    sint32 idx_point; 
    for (idx_point = m_path_cur+1; idx_point<last_point; idx_point++) { 

        ai->m_world->GetCellContents(&m_path[idx_point], &player_id, 
			&a_id, &top_unit_type, &unit_num, &c_id, &unitVis, &cityVis, &mineVis, 
            &can_be_expelled);

        if ((a_id != 0) && !can_be_expelled) {  
            if (ai->m_my_player_id != player_id) { 
                return FALSE; 
            } 
        } else if (c_id != 0) { 
            if (ai->m_my_player_id != player_id) { 
                return FALSE; 
            } 
        }
    }

    return TRUE;
}





void ArmyAgent::ValidatePath(AiMain *ai, GOAL_TYPE what_goal)
{ 
	sint32 player_id;
	uint32 a_id;
    sint32 top_unit_type; 
	sint32 unit_num;
    uint32 c_id;
    BOOL unitVis;
    BOOL cityVis;
    BOOL mineVis;
    sint32 last_point; 
    BOOL can_be_expelled; 

    if (GOAL_TYPE_WANDER == what_goal) { 
        return; 
    } 

    switch (what_goal) { 
    case GOAL_TYPE_SEIGE:  
    case GOAL_TYPE_ATTACK: 
    case GOAL_TYPE_SALLY:
    case GOAL_TYPE_BOMBARD:

    case GOAL_TYPE_ENSLAVE: 
    case GOAL_TYPE_CONVERT_CITY:
    case GOAL_TYPE_BIOTERROR_CITY:
    case GOAL_TYPE_NANOATTACK_CITY:
    case GOAL_TYPE_HARASS_CITY:
    case GOAL_TYPE_EXPEL:
    case GOAL_TYPE_HURT_ARMY:
    case GOAL_TYPE_COUNTER_STEALTH:
    case GOAL_TYPE_PLANT_NUKE:
    case GOAL_TYPE_MAKE_PARK:
    case GOAL_TYPE_CAUSE_UNHAPPINESS:
    case GOAL_TYPE_ESTABLISH_EMBASSY:
    case GOAL_TYPE_FRANCHISING:
    case GOAL_TYPE_STEAL_TECHNOLOGY:
    case GOAL_TYPE_INCITE_REVOLUTION:
    case GOAL_TYPE_INJOIN:
    case GOAL_TYPE_ASSASINATE_RULER:
    case GOAL_TYPE_UNDERGROUND_RAILWAY:
    case GOAL_TYPE_NUKE_CITY:
  
    case GOAL_TYPE_REFORM_CITY:
    case GOAL_TYPE_SUE_FRANCHISE:
        last_point = m_path_cur_nPoints-1; 
		break;
    default:
        last_point = m_path_cur_nPoints; 
    }

	sint32 hey_theres_is_an_army_here=0; 
	sint32 idx_point; 
    for (idx_point = m_path_cur; idx_point<last_point; idx_point++) { 

        ai->m_world->GetCellContents(&m_path[idx_point], &player_id, 
			&a_id, &top_unit_type, &unit_num, &c_id, &unitVis, &cityVis, &mineVis, 
            &can_be_expelled);

        if ((a_id != 0) && !can_be_expelled) {  
            if (ai->m_my_player_id != player_id) { 
                Assert(hey_theres_is_an_army_here); 

                
                 ai->m_world->GetCellContents(&m_path[idx_point], &player_id, 
        			&a_id, &top_unit_type, &unit_num, &c_id, &unitVis, &cityVis, &mineVis, 
                    &can_be_expelled);
            } 
        } else if (c_id != 0) { 
            if (ai->m_my_player_id != player_id) { 
				Assert(hey_theres_is_an_army_here);

                 
                 ai->m_world->GetCellContents(&m_path[idx_point], &player_id, 
        			&a_id, &top_unit_type, &unit_num, &c_id, &unitVis, &cityVis, &mineVis, 
                    &can_be_expelled);

            } 
        }

    }
}


BOOL ArmyAgent::FindPath(AiMain *ai, MapPointData &pos, 
     sint32 &pathtime, BOOL check_rail_launcher, sint32 cutoff, 
     sint32 &nodes_opened, BOOL check_dest, const GOAL_TYPE what_goal)
{
         
    float total_cost; 
    
    pathtime = GetTickCount(); 

    
    if ((m_my_pos->x == pos.x) &&
        (m_my_pos->y == pos.y) && 
        (m_my_pos->z == pos.z)) { 
        m_path_cur_nPoints = 1;
        m_path_cur = 0; 
        m_path[0].x = m_my_pos->x; 
        m_path[0].y = m_my_pos->y; 
        m_path[0].z = m_my_pos->z; 
		m_pathValid = TRUE;
    
        return TRUE; 
    }

    
	if (UseOldPath(ai, pos, what_goal))
		return TRUE;

    BOOL is_unbuilt = (m_state == AGENT_STATE_UNBUILT) || (m_state == AGENT_STATE_BUILDING); 
    
    BOOL is_unknown_id=FALSE;

    
    g_trans_ai = ai; 
    g_trans_max_r = 0.8; 
	sint32 cont;
	BOOL is_land;
	BOOL r = g_trans_ai->m_continents->GetContinent(g_trans_ai, pos, cont, is_land);
	g_trans_dest_cont = cont;

    sint32 notest = 1; 
    if (!GotCargo()) { 
        if (is_unbuilt) { 


            if (ai->m_robot_astar->FindPath(NULL, m_id.GetVal(), PATH_ARMY_TYPE_DB, 
                GetUnbuiltType(), m_my_pos, &pos, &m_path_total_nPoints, &m_path, 
                &m_path_cur_nPoints, &total_cost, CanSpaceLaunch(ai), 
                CanSpaceLand(ai), check_rail_launcher, FALSE, cutoff, nodes_opened,
				check_dest, FALSE, TRUE)) { 

				
                Assert(0 < m_path_cur_nPoints); 
#ifdef _DEBUG
	ValidatePath(ai, what_goal);
#endif
                m_path_cur = 0; 
				m_pathValid = TRUE;
                return TRUE; 
	           } else { 
				m_pathValid = FALSE;

                return FALSE; 
            }
        } else { 
START_MULTI_CLOCK 

            BOOL can_go_down = CanSpaceLand(ai) || ((m_my_pos->z==SPACE_Z) && !CanMoveSpace(ai)); 
            if (ai->m_robot_astar->FindPath(DefensivePathCallback, m_id.GetVal(), 
                PATH_ARMY_TYPE_EXISTS, 0, 
                m_my_pos, &pos, &m_path_total_nPoints, &m_path, &m_path_cur_nPoints, 
                &total_cost, CanSpaceLaunch(ai), can_go_down, 
                check_rail_launcher, FALSE, cutoff, nodes_opened, check_dest, FALSE, TRUE)) { 
                m_path_cur = 0; 
END_MULTI_CLOCK 

                Assert(0 < m_path_cur_nPoints); 
#ifdef _DEBUG
	ValidatePath(ai, what_goal);
#endif

                m_path_cur = 0; 
				m_pathValid = TRUE;
                return TRUE; 
            } else { 

				m_pathValid = FALSE;
                return FALSE; 
            }
        }
    } else { 
        sint32 idx_unit; 
        sint32 idx_cargo; 
        uint32 move_intersection1 = 0xFFFFFFFF;
        for (idx_unit=0; idx_unit<m_nUnits; idx_unit++) { 
            move_intersection1 &= ai->m_unitDB->GetMovementType(m_unit_type[idx_unit]); 
        }
        uint32 move_intersection2 = 0xFFFFFFFF;
        for (idx_cargo=0; idx_cargo<m_cargo->Num(); idx_cargo++) { 
            move_intersection2 &= ai->m_unitDB->GetMovementType(m_cargo->Get(idx_cargo)); 
        } 
        
        uint32 move_union = move_intersection1 | move_intersection2; 
        

		
		

        check_dest = FALSE; 
        g_trans_can_space_assault = CanAttackFromSpaceShip(ai);
        g_trans_can_beach_assault = CanBeachAssaultAtAll(ai);
        g_trans_can_capture_city = !CantCaptureCity(ai);

        if (ai->m_robot_astar->FindPath(TransportPathCallback, m_id.GetVal(), 
            PATH_ARMY_TYPE_MADEUP,   move_union, m_my_pos, 
            &pos, &m_path_total_nPoints, &m_path, &m_path_cur_nPoints, 
            &total_cost, CanSpaceLaunch(ai), CanSpaceLand(ai), 
            check_rail_launcher, FALSE, cutoff, nodes_opened, check_dest, FALSE, TRUE)) { 


#ifdef _DEBUG
	ValidatePath(ai, what_goal);
#endif
            Assert(0 < m_path_cur_nPoints); 
			m_path_cur = 0; 
			m_pathValid = TRUE;
            return TRUE; 
        } else { 

			m_pathValid = FALSE;
            return FALSE; 
        }
    }    

    Assert(0 < m_path_cur_nPoints); 
}

sint32 ArmyAgent::MovePoints2Rounds(AiMain *ai, double total_cost) 
{
    double cur, min_move; 
    BOOL is_unknown_id=FALSE; 
    
    switch (m_state) { 
    case AGENT_STATE_BUILDING: 
    case AGENT_STATE_UNBUILT: 
        break; 
    case AGENT_STATE_LOOKING_FOR_WORK: 
    case AGENT_STATE_GOTO_SETTLE_DEST:
	case AGENT_STATE_GOTO_PERIMETER_DEST:
	case AGENT_STATE_GOTO_PATROL_DEST:
	case AGENT_STATE_GOTO_CHOKEPOINT_DEST:
    case AGENT_STATE_GOTO_EXPLORE_DEST:
    case AGENT_STATE_GOTO_ATTACK_DEST:
    case AGENT_STATE_GOTO_RECRUITMENT_DEST:
	case AGENT_STATE_GOTO_ENSLAVE_DEST:
    case AGENT_STATE_GOTO_RECRUITMENT_ATTACK_DEST:
    case AGENT_STATE_MARKED_GARRISON:
    case AGENT_STATE_TRANSPORT_GOTO_BOARD: 
    case AGENT_STATE_CARGO_GOTO_BOARD:
	case AGENT_STATE_AT_GOAL:
        ai->m_player->GetArmyCurMinMovementPoints(m_id.GetVal(), &is_unknown_id, &cur); 
        Assert(is_unknown_id == FALSE);
  
        if (total_cost <= cur) { 
            return 0; 
        } else { 
            total_cost -= cur; 
        } 
        break; 
    case AGENT_STATE_MARKED_DEAD:
    case AGENT_STATE_MARKED_SETTLE:
        INSANE(MOVE_2_ROUNDS_MARKED_STATE);
        break;

    default: 
        INSANE(MOVE_2_ROUNDS_UNKNOWN); 
        break;
    }

    sint32 rnd = 0; 

    min_move = GetMaxMovePoints();
    
    rnd  = sint32(total_cost/min_move); 

    if (0.01 < (total_cost - rnd * min_move)) { 
        rnd++; 
    } 
    return rnd; 
}

void ArmyAgent::UpdateMaxMovePoints(AiMain *ai) 
{ 
    BOOL is_unknown_id; 

    switch (m_state) { 
    case AGENT_STATE_BUILDING: 
    case AGENT_STATE_UNBUILT: 
        m_max_movepoints = ai->m_unitDB->UnitTypeMovementPoints(GetUnbuiltType()); 
        break; 
    case AGENT_STATE_LOOKING_FOR_WORK: 
    case AGENT_STATE_GOTO_SETTLE_DEST:
	case AGENT_STATE_GOTO_PERIMETER_DEST:
	case AGENT_STATE_GOTO_PATROL_DEST:
	case AGENT_STATE_GOTO_CHOKEPOINT_DEST:
    case AGENT_STATE_GOTO_EXPLORE_DEST:
    case AGENT_STATE_GOTO_ATTACK_DEST:
	case AGENT_STATE_GOTO_ENSLAVE_DEST:	
    case AGENT_STATE_GOTO_RECRUITMENT_DEST:
    case AGENT_STATE_GOTO_RECRUITMENT_ATTACK_DEST:
    case AGENT_STATE_MARKED_GARRISON:
	case AGENT_STATE_AT_GOAL:
    case AGENT_STATE_TRANSPORT_GOTO_BOARD:
    case AGENT_STATE_CARGO_GOTO_BOARD:
        ai->m_player->GetArmyMinMovementPoints(m_id.GetVal(), &is_unknown_id, &m_max_movepoints); 
        Assert(is_unknown_id == FALSE);
        break; 
    case AGENT_STATE_MARKED_DEAD:
    case AGENT_STATE_MARKED_SETTLE:
        m_max_movepoints = 0.0;
        break;
    default: 
        INSANE(GET_MAX_MOVE_UNKNOWN); 
        m_max_movepoints = 0.0;
        break;
    }
}
 
void ArmyAgent::UpdateMovePoints(AiMain *ai) 
{ 
    BOOL is_unknown_id; 

    switch (m_state) { 
    case AGENT_STATE_BUILDING: 
    case AGENT_STATE_UNBUILT: 
        m_current_movepoints = ai->m_unitDB->UnitTypeMovementPoints(GetUnbuiltType()); 
        return; 
    case AGENT_STATE_LOOKING_FOR_WORK: 
    case AGENT_STATE_GOTO_SETTLE_DEST:
	case AGENT_STATE_GOTO_PERIMETER_DEST:
	case AGENT_STATE_GOTO_PATROL_DEST:
	case AGENT_STATE_GOTO_CHOKEPOINT_DEST:
    case AGENT_STATE_GOTO_EXPLORE_DEST:
    case AGENT_STATE_GOTO_ATTACK_DEST: 
    case AGENT_STATE_GOTO_RECRUITMENT_DEST:
    case AGENT_STATE_GOTO_RECRUITMENT_ATTACK_DEST:
	case AGENT_STATE_GOTO_ENSLAVE_DEST:
    case AGENT_STATE_MARKED_GARRISON:
	case AGENT_STATE_AT_GOAL:
    case AGENT_STATE_WAITING_FOR_TRANSPORT:
    case AGENT_STATE_TRANSPORT_GOTO_BOARD:
    case AGENT_STATE_CARGO_GOTO_BOARD:
        ai->m_player->GetArmyCurMinMovementPoints(m_id.GetVal(), &is_unknown_id, &m_current_movepoints);
        return;
    case AGENT_STATE_MARKED_DEAD:
    case AGENT_STATE_MARKED_SETTLE:
        m_current_movepoints = 0.0;
        return;
    default: 
        INSANE(GET_MOVE_POINTS_AGENT_STATE_UNKNOWN); 
        m_current_movepoints = 0.0; 
        return; 
    }
}

BOOL ArmyAgent::AtEndOfPath(AiMain *ai)
{
        
    Assert(0 < m_path_cur_nPoints); 

    if (IsEqual(m_path[m_path_cur_nPoints-1], *m_my_pos)) { 
        return TRUE; 
    } else if ((m_path_cur_nPoints-1) <= m_path_cur) {   
        INSANE (PAST_END_OF_PATH);
        m_state = AGENT_STATE_LOOKING_FOR_WORK; 
        return FALSE; 
    } else { 
        return FALSE; 
    } 
}

sint32 ArmyAgent::GetRemainingPathPoints(AiMain *ai)
{
	return  m_path_cur_nPoints - m_path_cur - 1;
}

BOOL ArmyAgent::CanEnter(AiMain *ai, MapPointData &pos)
{
    BOOL is_unknown_id;
    BOOL move_to_many_units_dest;
    BOOL move_violated_zoc;
    BOOL move_violated_movetype;
    BOOL move_out_of_fuel;

	
    BOOL b =  ai->m_player->ArmyCanEnter(m_id.GetVal(), &is_unknown_id, 
        &pos, &move_to_many_units_dest, &move_violated_zoc, 
        &move_violated_movetype, &move_out_of_fuel);

    Assert(!is_unknown_id); 
    return b; 
}

BOOL ArmyAgent::CanEnterNextPathPoint(AiMain *ai)
{
	
	if (m_path_cur+1 > m_path_cur_nPoints)
		return FALSE;

	Assert(m_path_cur_nPoints - m_path_cur >= 1);
	Assert(m_path[m_path_cur+1].z == GROUND_Z || 
		   m_path[m_path_cur+1].z == SPACE_Z);
    return (CanEnter(ai, m_path[m_path_cur+1]));
}

void ArmyAgent::GetNextPathPoint(MapPointData &pos)
{
	Assert(m_path_cur_nPoints - m_path_cur >= 1);
    pos = m_path[m_path_cur+1];
	Assert(pos.z == GROUND_Z || pos.z == SPACE_Z);
}

void ArmyAgent::GotoNextPathPoint(AiMain *ai, BOOL &did_move)
{
                
    BOOL is_unknown_id;
    BOOL i_died;  
    BOOL move_violated_zoc;
    BOOL revealed_foreign_units; 
    BOOL revealed_unexplored;
    BOOL is_transported; 
    BOOL out_of_fuel;


START_MULTI_CLOCK
    ai->m_player->ArmyMoveTo(m_id.GetVal(), &is_unknown_id, 
                &m_path[m_path_cur+1], &did_move, &i_died, 
                &move_violated_zoc, &revealed_foreign_units, 
                &revealed_unexplored, 
                &is_transported, &out_of_fuel);
END_MULTI_CLOCK    
        
    Assert(!is_unknown_id); 
    if (did_move) { 
        m_path_cur++; 
    }        

    if (m_state == AGENT_STATE_MARKED_DEAD)
        return; 

    UpdateMovePoints(ai); 

 
    if (GetUsesFuel()) { 
        UpdateCurrentFuel(ai);  
    }

}



BOOL ArmyAgent::ActionSettle(AiMain *ai) 
{ 
    BOOL is_unknown_id;
    return ai->m_player->ArmySettle(m_id.GetVal(), &is_unknown_id); 
}





















void ArmyAgent::ActionUngroupArmy(AiMain *ai)
{ 
    
    BOOL is_unknown_id;
    sint32 r; 

    Assert(1 < m_nUnits); 
    r  = ai->m_player->ArmyUngroup(m_id.GetVal(), &is_unknown_id); 
	
	
    
    
    
    Assert(r); 
} 



void ArmyAgent::ClearArmy(AiMain *ai)
{

    m_nUnits = 0; 
	m_state = AGENT_STATE_LOOKING_FOR_WORK;  
}

























































































































void ArmyAgent::EventMarkDead() 
{ 
    m_state = AGENT_STATE_MARKED_DEAD;
}

void ArmyAgent::EventMarkTransported()
{
    m_state = AGENT_STATE_MARKED_WAITING_TOBE_CARGO;
} 

double ArmyAgent::GetDefenseStrength()

{
    return m_agent_strength->GetDefense(); 
} 

double ArmyAgent::GetAttackStrength()
{
    return m_agent_strength->GetAttack(); 
} 


void ArmyAgent::EventUngarrison() 

{
    if (m_state == AGENT_STATE_MARKED_GARRISON) { 
        m_state = AGENT_STATE_LOOKING_FOR_WORK; 
    } 
}

BOOL ArmyAgent::BoatCanReachShore(AiMain *ai,  MapPointData &dest_pos)
{ 
   
    sint32 dest_cont; 
    BOOL dest_is_land; 

    if (m_my_is_land) { 
        if (!ai->m_continents->GetContinent(ai, dest_pos, dest_cont, dest_is_land)) { 
            return FALSE; 
        }
        if (!ai->m_continents->LandShareWater(ai, m_my_continent, dest_cont)) { 
            return FALSE;
        }

		
		return TRUE;
    }
    
    
    if (!ai->m_continents->GetContinent(ai, dest_pos, dest_cont, dest_is_land)) { 
        return FALSE; 
    }
    
    if (!dest_is_land) { 
        return FALSE; 
    } 
    
    if (!ai->m_world->IsLandNextTooWater(dest_cont, m_my_continent)) { 
        return FALSE; 
    }

    return TRUE; 
}


BOOL ArmyAgent::PretestTransportSomewhere(AiMain *ai,  ArmyGoal *goal_obj)
{
    double give_up, give_up_threshold =0.0; 

    
    switch (m_state) { 
    case AGENT_STATE_MARKED_DEAD:
    case AGENT_STATE_MARKED_SETTLE:
    case AGENT_STATE_MARKED_GARRISON:
        return FALSE;     

    case AGENT_STATE_BUILDING: 
    case AGENT_STATE_UNBUILT:
       if (m_home_city_agent->GetProjectedNetProduction() < 1) { 
            return FALSE; 
       }
       give_up = m_home_city_agent->EstimateCostToGiveUpCurrentBuild(ai, goal_obj->GetType()); 
       if (give_up_threshold <= give_up)  {      
            return FALSE;                              
       }
    default:
        INSANE(PRETEST_TRANSPORT_UNKNOWN_STATE);
    }

    
    sint32 full_slots;
    sint32 empty_slots;
    if (!GetNumCargo(ai, full_slots, empty_slots)) { 
        return FALSE; 
    }

    

    if (0 < full_slots) { 
        return FALSE; 
    } 

    if (empty_slots < 1) { 
        return FALSE; 
    }

    if (GetMovePoints() < MINIMUM_MOVEMENT_POINTS_TO_MOVE ||
		IsWaitState()) { 
        return FALSE; 
    } 

   

    MapPointData dest_pos;
    sint32       dest_cont; 
    BOOL         dest_is_land; 
    goal_obj->GetPos(dest_pos); 

    
    if ((m_state == AGENT_STATE_BUILDING) || (m_state == AGENT_STATE_UNBUILT)) { 
        if (!CanMoveSpace(ai) && !CanMoveAir(ai)) { 
                       
            if (!ai->m_continents->GetContinent(ai, dest_pos, dest_cont, dest_is_land)) { 
                return FALSE; 
            }
            
            if (!dest_is_land) { 
                return FALSE; 
            } 
            
            if (m_my_is_land) { 
                if (!ai->m_continents->LandShareWater(ai, m_my_continent, dest_cont)) { 
                    return FALSE;
                }
            } else { 
                if (!ai->m_world->IsLandNextTooWater(m_my_continent, dest_cont)) { 
                    return FALSE; 
                }
            }
        }
    } else { 
    }

    return TRUE; 
}

























































































































double ArmyAgent::EstimateTransportUtility(AiMain *ai, ArmyAgent *transport) 
{
    
    Assert(transport); 

    if (!transport->CanTransport(ai, this)) { 
        return BAD_UTILITY; 
    } 

    if (transport->GetMovePoints() < MINIMUM_MOVEMENT_POINTS_TO_MOVE ||
		transport->IsWaitState()) { 
        return BAD_UTILITY; 
    } 

    BOOL check_continents = !transport->CanMoveSpace(ai) && 
        !transport->CanMoveAir(ai) && 
        (m_my_pos->z == GROUND_Z);

	
    
    
    
    
    

    MapPointData trans_pos; 
    transport->GetPos(ai, trans_pos); 
    sint32 trans_cont; 
    BOOL trans_is_land; 
    if (check_continents) { 
        if (!ai->m_continents->GetContinent(ai, trans_pos, trans_cont, trans_is_land)) { 
            return BAD_UTILITY; 
        }
        
       if (trans_is_land) { 

           
           
           

           if (!ai->m_continents->LandShareWater(ai, trans_cont, m_my_continent)) { 
               return BAD_UTILITY; 
           } 

           
           
           

       } else { 
        
            if (!ai->m_world->IsLandNextTooWater(m_my_continent, trans_cont)) { 
                return BAD_UTILITY; 
            } 
            
            
            
       }
    }

    double move_point_cost_trans; 
    sint32 rounds_trans; 
    double build_time;
    if (transport->BidDistance(ai,  FALSE, *m_my_pos, move_point_cost_trans, rounds_trans, build_time) < 0.0) { 
        return BAD_UTILITY; 
    } 
    
    if (transport->m_state == AGENT_STATE_BUILDING) { 
        rounds_trans += transport->m_home_city_agent->
            EstimateRoundsToFinished(ai, m_id); 
    } else if (transport->m_state == AGENT_STATE_UNBUILT) { 
        rounds_trans += transport->m_home_city_agent->
			EstimateRoundsToEmptyQueue(ai); 
        rounds_trans += transport->m_home_city_agent->
            EstimateRoundsToBuildUnit(ai, transport->GetUnbuiltType()); 
    } 

	double tile_count = ai->m_map->WrappedDistance(*m_my_pos, trans_pos);

	double move_type_bonus = 0.0;

	
	
	if (transport->CanMoveDeepWater(ai))
		move_type_bonus += 1000.0; 

	return move_type_bonus + 100.0 * (ai->m_round_count->EstimateRoundsToEnd() - rounds_trans) - tile_count; 
}

void ArmyAgent::RegisterClearCargo()
{
	if (NULL != m_cargo)
		m_cargo->Clear();
}

void ArmyAgent::RegisterInsertCargo(AiMain *ai, sint32 unit_type, sint32 hp) 
{

    if (NULL ==  m_cargo) { 
        m_cargo = new DynamicArray<sint32>; 
    } 

    m_cargo->InsertFlat(unit_type); 

    if (!m_army_contains_settler) 
        UpdateArmyContainsSettler(ai); 

	
	

	
	if (the_squad)
	{
		SetLastGoal(the_squad->last_goal_type);

		
		
	}

	#ifdef _DEBUG
	
	
	
    
    
	
	#endif

}

void ArmyAgent::RegisterUnloadCargo(AiMain *ai, sint32 unit_type, sint32 hp,
									const CAUSE_REMOVE_ARMY & cause)
{
    sint32 cargo_idx;
    sint32 cargo_num;

	
	if (cause == CAUSE_REMOVE_ARMY_TRANSPORT_DIED && 
		m_state == AGENT_STATE_MARKED_DEAD)
		return;

    Assert(m_cargo); 
    cargo_num = m_cargo->Num(); 
    for (cargo_idx=0; cargo_idx<cargo_num; cargo_idx++) { 
        if (m_cargo->Access(cargo_idx) == unit_type) { 
            m_cargo->DelIndexFlat(cargo_idx);
			break;
        } 
    }
	Assert(cargo_idx < cargo_num); 
    if (m_army_contains_settler) 
        UpdateArmyContainsSettler(ai); 

	
	

	
	if (GotCargo() <= 0)
		{
			SetLastGoal(GOAL_TYPE_MAX);

			
			

			
			
			if (the_squad)
			{
				
				
				the_squad->Recompute_Squad_Class(ai);
				
			}
		}

	
	ResetArmyStrength(ai);
	
	#ifdef _DEBUG
	if (cause != CAUSE_REMOVE_ARMY_TRANSPORT_DIED)
	{
		
		sint32 full_slots, empty_slots;
		BOOL is_unknown_id;
		ai->m_player->ArmyGetNumCargo(&is_unknown_id, m_id.GetVal(), 
			&full_slots, &empty_slots);
		
		GetNumCargo(ai, full_slots, empty_slots);
		Assert(full_slots == m_cargo->Num());
	}
	#endif
}


void ArmyAgent::RegisterDeadUnit(AiMain *ai, sint32 unit_type)
{
    Assert(0 < m_nUnits); 
    if (m_nUnits <= 0) { 
        m_state = AGENT_STATE_MARKED_DEAD; 
        return; 
    } 

	sint16 unit_idx; 

	
    for (unit_idx=0; unit_idx < m_nUnits; unit_idx++) {
        if (m_unit_type[unit_idx] == unit_type) { 
			break; 
        }
    }

	
	Assert(unit_idx < m_nUnits || m_cargo); 
	if (unit_idx >= m_nUnits) {
		return; 
	} 

	Assert(m_unit_type[unit_idx] == unit_type); 

	for ( ; unit_idx < (m_nUnits-1); unit_idx++) { 
		m_unit_type[unit_idx] = m_unit_type[unit_idx+1]; 
	} 
	Assert(unit_idx == (m_nUnits-1)); 
	m_unit_type[unit_idx] = -1; 
	m_nUnits--; 

	Assert(0 <= m_nUnits); 

    if (m_nUnits <= 0) { 
        m_state = AGENT_STATE_MARKED_DEAD; 
    } 
	
	else
	{
		ResetArmyStrength(ai);
	
	    if (m_army_contains_settler) 
			UpdateArmyContainsSettler(ai); 

		if (m_uses_fuel)
	        UpdateUsesFuel(ai); 

		
		

		UpdateMovePoints(ai); 
	    UpdateMaxMovePoints(ai); 
	}

}


BOOL ArmyAgent::CanTransport(AiMain *ai,  ArmyAgent *cargo)
{ 
    BOOL c = (cargo->GetState() == AGENT_STATE_BUILDING) || (cargo->GetState() == AGENT_STATE_UNBUILT);
    BOOL t = (m_state == AGENT_STATE_BUILDING) || (m_state == AGENT_STATE_UNBUILT);
    sint32 idx_unit; 

    if (c && t) { 
        return ai->m_unitDB->CanTransport(GetUnbuiltType(), cargo->GetUnbuiltType()); 
    } else if (c && (t == FALSE)) { 
        for (idx_unit=0; (idx_unit < m_nUnits); idx_unit++) { 
            if (ai->m_unitDB->CanTransport(m_unit_type[idx_unit], cargo->GetUnbuiltType())) { 
                return TRUE;
            }
        } 
        return FALSE; 

    } else if ((c == FALSE) && t) { 
        if (ai->m_unitDB->GetMaxCargo(GetUnbuiltType()) < cargo->m_nUnits) { 
            return FALSE; 
        } 

        for (idx_unit=0; (idx_unit < cargo->m_nUnits); idx_unit++) { 
            if (ai->m_unitDB->CanTransport(GetUnbuiltType(), cargo->m_unit_type[idx_unit]) == FALSE) { 
                return FALSE; 
            }
        } 
        return TRUE; 

    } else { 
        BOOL is_unknown_id; 
		





		
        if (ai->m_player->AiNumUnitsCanMoveIntoThisTranportEver(&is_unknown_id,  
            cargo->GetID().GetVal(), m_id.GetVal()) < 1) { 
            return FALSE; 
        }
        return TRUE; 
    }
    return TRUE; 
}

uint32 ArmyAgent::GetMoveIntersection(AiMain* ai)
{
    sint32 idx_unit; 

    uint32 move_intersection = 0xFFFFFFFF; 

    for (idx_unit=0; idx_unit<m_nUnits; idx_unit++) { 
        move_intersection &= ai->m_unitDB->GetMovementType(m_unit_type[idx_unit]); 
    } 

    return move_intersection; 
}


BOOL ArmyAgent::FindPathToBoard(AiMain *ai, uint32 move_intersection, 
                                 MapPointData &pos, sint32 &pathtime, 
                                 sint32 cutoff, sint32 &nodes_opened,
								 BOOL check_dest, const GOAL_TYPE what_goal)
{

  float total_cost; 

  pathtime = GetTickCount(); 

  g_trans_ai = ai; 
  g_trans_max_r = 100.0 / GetMaxMovePoints(); 

  if ((m_my_pos->x == pos.x) &&
      (m_my_pos->y == pos.y) && 
      (m_my_pos->z == pos.z)) { 
      m_path_cur_nPoints = 1;
      m_path_cur = 0; 
      m_path[0].x = m_my_pos->x; 
      m_path[0].y = m_my_pos->y; 
      m_path[0].z = m_my_pos->z; 
      return TRUE; 
  }
   


    
	if (UseOldPath(ai, pos, what_goal))
		return TRUE;

	
	
    sint32 cont; 
    BOOL is_land; 
	BOOL r = g_trans_ai->m_continents->GetContinent(g_trans_ai, pos, cont, is_land);
	g_trans_dest_cont = cont;

    g_trans_can_space_assault = CanAttackFromSpaceShip(ai);
    g_trans_can_beach_assault = CanBeachAssaultAtAll(ai);
    g_trans_can_capture_city = !CantCaptureCity(ai);

    if (ai->m_robot_astar->FindPath(TransportPathCallback, m_id.GetVal(), 
       PATH_ARMY_TYPE_MADEUP,   move_intersection, m_my_pos, 
          &pos, &m_path_total_nPoints, &m_path, &m_path_cur_nPoints, 
          &total_cost, CanSpaceLaunch(ai), CanSpaceLand(ai), FALSE, 
          FALSE, cutoff, nodes_opened, check_dest, FALSE, TRUE)) { 



          Assert(0 < m_path_cur_nPoints);
#ifdef _DEBUG
	ValidatePath(ai, what_goal);
#endif

          m_path_cur = 0; 

		  m_pathValid = TRUE;
         return TRUE; 
      } else { 
		 m_pathValid = FALSE;
         return FALSE; 
      }
}

BOOL ArmyAgent::GotCargo() const 
{ 
    if (m_cargo) { 
        return 0 < m_cargo->Num(); 
    } else { 
        return FALSE; 
    } 
} 

BOOL ArmyAgent::TryToBoardTransport(AiMain *ai,  ArmyAgent *transport) 
{ 
    MapPointData trans_pos; 

    transport->GetPos(ai, trans_pos); 

    sint32 r; 
    BOOL is_unknown_id;
    BOOL is_transported; 

    if (MINIMUM_MOVEMENT_POINTS_TO_MOVE <= GetMovePoints()) 
	{
		
		if (IsEqual(*m_my_pos, trans_pos)) 
		{ 
			r = ai->m_player->AiArmyMoveIntoTranport(&is_unknown_id,  m_id.GetVal(), 
                transport->GetID().GetVal(), &is_transported);
			

			Assert(is_unknown_id == FALSE); 

			m_pathValid = FALSE;
			return r;
		} 
		
		m_pathValid = FALSE;
		
		return FALSE; 
    } 
    return FALSE; 
}

























































void ArmyAgent::FlipCopyPath(ArmyAgent *cargo) 
{
    MapPointData* tmp=NULL; 
    if (m_path_total_nPoints < cargo->m_path_total_nPoints) { 
        tmp = new MapPointData[cargo->m_path_total_nPoints]; 
        delete[] m_path;
        m_path = tmp;
        m_path_total_nPoints = cargo->m_path_total_nPoints; 
    } 

    m_path_cur_nPoints = cargo->m_path_cur_nPoints; 
    m_path_cur = 0; 

    sint32 idx_path; 
    for (idx_path=0; idx_path<m_path_cur_nPoints; idx_path++) { 
        m_path[idx_path] = cargo->m_path[(m_path_cur_nPoints-1) - idx_path]; 
    } 

} 


void ArmyAgent::MakePathRoundTrip()
{
    sint32 idx_path; 
	sint32 idx_append;
	
	if (m_path_total_nPoints < m_path_cur_nPoints * 2)
		{
			
			MapPointData* tmp = new MapPointData[m_path_cur_nPoints * 2];
			for (int idx_path=0; idx_path < m_path_cur_nPoints; idx_path++)
				
				tmp[idx_path] = m_path[idx_path];
			m_path_total_nPoints = m_path_cur_nPoints * 2;
			delete[] m_path;
			m_path = tmp;
		}
			
	
	idx_append = m_path_cur_nPoints;
    for (idx_path=m_path_cur_nPoints-2; 
		 idx_path >= 0; 
		 idx_path--, idx_append++) 
		{ 
			m_path[idx_append] = m_path[idx_path]; 
		} 

	
	m_path_cur_nPoints = idx_append;
} 



BOOL ArmyAgent::MoveToTransportBoard(AiMain *ai, GoalTransport* goal_obj, 
     ArmyAgent* cargo, BOOL& did_move, BOOL& revealed_unexplored, BOOL& use_marked,
     BOOL& revealed_forign_units, double utility)
{

    sint32 r; 
    sint32 pathtime;
    MapPointData cargo_pos; 
    sint32 cutoff=200000;
    sint32 nodes_opened; 

    BSetID tmpid=NULL;
    ArmyAgent *aa=NULL; 
    BSetNode<ArmyAgent> *an=NULL; 

    switch (m_state) { 
    case AGENT_STATE_UNBUILT:
		
		if (m_home_city_agent->PretestContructUnit(ai, m_unit_type[0]))
		{
			m_home_city_agent->EnqueueArmy(ai, m_unit_type[0], utility); 
			return TRUE;				
		}
        return FALSE;
        break; 
    case AGENT_STATE_BUILDING: 
        return TRUE;
    default:
        
        cargo->GetPos(ai, cargo_pos); 

        g_trans_ai = ai; 
        g_trans_max_r = 100.0 / GetMaxMovePoints(); 

        uint32 move_intersection = GetMoveIntersection(ai);
        move_intersection |= cargo->GetMoveIntersection(ai); 
 
        r = FindPathToBoard(ai, move_intersection, *m_my_pos, pathtime, 
            cutoff, nodes_opened, FALSE, GOAL_TYPE_NULL);
    
        if (r) { 
            m_state = AGENT_STATE_TRANSPORT_GOTO_BOARD;

  
            m_state = AGENT_STATE_TRANSPORT_GOTO_BOARD; 
        } else { 
            return FALSE;
        }
    }

    return TRUE; 
}

void ArmyAgent::InsertCargo(AiMain *ai, ArmyAgent *cargo_agent)
{
	
	Assert(0);
    if ( m_cargo == NULL) { 
        m_cargo = new DynamicArray<sint32>; 
    } 

    sint32 idx_cargo; 
    for (idx_cargo=0; idx_cargo<cargo_agent->m_nUnits; idx_cargo++) { 
        m_cargo->InsertFlat(cargo_agent->m_unit_type[idx_cargo]); 
    }
}


BOOL ArmyAgent::CargoCanEnter(AiMain *ai,  MapPointData &dest_pos) 
{ 
	sint32 idx_cargo, num_cargo; 
	num_cargo = m_cargo->Num(); 
	
	
	if (num_cargo <= 0)
		return FALSE;
	   
	
   
   

   
   
   
   
   
   
   
   
   
   

  
    
      
       
   

   for (idx_cargo=0; idx_cargo<num_cargo; idx_cargo++) { 
      if (!ai->m_player->UnitTypeCanEnter(m_cargo->Get(idx_cargo), &dest_pos)) {
          return FALSE; 
      }
   }
   return TRUE; 
} 


BOOL ArmyAgent::DebarkCargoOntoPath(AiMain *ai)
{
    MapPointData dest_pos; 
    dest_pos = m_path[m_path_cur+1]; 

    BOOL is_unknown_id              = FALSE;
    BOOL did_move                   = FALSE; 
    BOOL i_died                     = FALSE; 
    BOOL revealed_foreign_units     = FALSE;
    BOOL revealed_unexplored        = FALSE; 
    BOOL zocViolation               = FALSE; 
    BOOL is_transported             = FALSE;
    BOOL r                          = FALSE; 
    sint32 dest_cont; 
    BOOL dest_is_land;
	
	
	ai->m_continents->GetContinent(ai, dest_pos, dest_cont, dest_is_land); 
    
    Assert(GotCargo()); 

    if (!CargoCanEnter(ai, dest_pos)) 
        return FALSE; 

	
	
	
	
	
	r = ai->m_player->AiUnloadAllTransportsInArmy(&is_unknown_id, 
        m_id.GetVal(), &dest_pos, &did_move, &i_died, 
		&revealed_foreign_units, &revealed_unexplored, 
		&zocViolation, &is_transported);

	
	MapPointData wrap_pos;
	sint16 dx, dy;
	for (dx=-1; dx < 2 && !r; dx++)
		for (dy = -1; dy < 2 && !r; dy++)
	{
			   
			   ai->m_map->Wrap(m_my_pos->x + dx, 
							   m_my_pos->y + dy,
							   m_my_pos->z, 
							   wrap_pos); 

   			
			if (wrap_pos.x == dest_pos.x && 
				wrap_pos.y == dest_pos.y)
				continue;

			r = ai->m_player->AiUnloadAllTransportsInArmy(&is_unknown_id, 
		        m_id.GetVal(), &wrap_pos, &did_move, &i_died, 
				&revealed_foreign_units, &revealed_unexplored, 
				&zocViolation, &is_transported);
	}
	
	if (r)
	{
	   m_state = AGENT_STATE_LOOKING_FOR_WORK; 
	   
	}
    return r; 
}

void ArmyAgent::UpdateUsesFuel(AiMain *ai)
{ 
    sint32 idx_unit; 

    for (idx_unit=0; idx_unit<m_nUnits; idx_unit++) { 
        if (ai->m_unitDB->UsesFuel(m_unit_type[idx_unit])) { 
            m_uses_fuel = TRUE;
            UpdateMaxFuel(ai); 
            return;
        } 
    }
    m_uses_fuel = FALSE; 
} 

void ArmyAgent::UpdateCurrentFuel(AiMain *ai)
{
    BOOL is_unknown_id; 
	sint32 fuel;
    sint32 r = ai->m_player->GetCurrentFuel(&is_unknown_id, m_id.GetVal(), &fuel); 

    if (m_state == AGENT_STATE_MARKED_DEAD)
        return; 

    Assert(r); 
	m_current_fuel = (sint16) fuel;
    Assert(is_unknown_id == FALSE); 
    return ; 
}


void  ArmyAgent::UpdateMaxFuel(AiMain *ai)
{ 
    sint16 idx_unit; 
    m_max_fuel = SHRT_MAX; 
    sint16 m; 

    for (idx_unit=0; idx_unit<m_nUnits; idx_unit++) { 
        if (ai->m_unitDB->UsesFuel(m_unit_type[idx_unit])) { 
            m = (sint16) ai->m_unitDB->GetMaxFuel(m_unit_type[idx_unit]); 
            if (m < m_max_fuel) { 
                m_max_fuel = m; 
            } 
        } 
    }
} 

BOOL ArmyAgent::OverFuelSafety(AiMain *ai)
{
    if (GetUsesFuel()) { 
        if (IsSingleUse(ai)) { 
            return FALSE; 
        } else {
			return double(GetCurrentFuel()-1) < (double(GetMaxFuel()) *0.5);
        }
    } else { 
        return FALSE; 
    } 
}








BOOL ArmyAgent::IsInFuelRange(AiMain * ai, MapPointData &dest_pos)
{
    sint32 current_fuel = GetCurrentFuel(); 

    double d = ai->m_map->WrappedDistance(*m_my_pos, dest_pos);

    return d <= current_fuel; 
}


BOOL ArmyAgent::PretestFuelToTarget(AiMain *ai, MapPointData &dest_pos)
{
     double f; 
     double m;

     if (GetUsesFuel()) { 
         if ((AGENT_STATE_UNBUILT == m_state) || (AGENT_STATE_BUILDING == m_state)) {
             m = f = GetMaxFuel();                            
         } else { 
             f = GetCurrentFuel(); 
             m = GetMaxFuel(); 
         }

         double d = ai->m_map->WrappedDistance(*m_my_pos, dest_pos); 

		 if (IsSingleUse(ai))
			return (d <= f); 
		 else
			return (d <= f-(m*0.5)); 
     } else { 
         return TRUE; 
     } 
}

BOOL ArmyAgent::IsParatroop() const 
{ 
    return m_is_paratroop; 
}

BOOL ArmyAgent::IsMigratingParatroop() const
{
    return m_is_paratroop && !m_has_been_paradropped; 
}

void ArmyAgent::UpdateParadropStatus(AiMain *ai, const CAUSE_NEW_ARMY cause)
{
    
    sint16 unit_idx; 
    for (unit_idx=0; unit_idx<m_nUnits; unit_idx++) { 
        if (ai->m_unitDB->IsParatrooper(m_unit_type[unit_idx])) { 
            m_is_paratroop = TRUE; 
        } 
    }

    m_has_been_paradropped = (cause == CAUSE_NEW_ARMY_PARADROP); 
}

BOOL ArmyAgent::CanParadropNow(AiMain *ai) 
{ 

    BOOL is_unknown_id; 

    return ai->m_player->CanParadropNow(&is_unknown_id, m_id.GetVal()); 
    Assert(FALSE == is_unknown_id); 



















}


BOOL ArmyAgent::IsSingleUse(AiMain *ai)
{
	
    sint16 unit_idx; 
    for (unit_idx=0; unit_idx<m_nUnits; unit_idx++) { 
        if (ai->m_unitDB->IsSingleUse(m_unit_type[unit_idx]) == TRUE) {
            return TRUE; 
        } 
    }

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->IsSingleUse(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

    return FALSE; 
}

BOOL ArmyAgent::IsNuclearAttack(AiMain *ai)
{
	
    sint16 unit_idx; 
    for (unit_idx=0; unit_idx<m_nUnits; unit_idx++) { 
        if (ai->m_unitDB->IsNuclearAttack(m_unit_type[unit_idx]) == FALSE) {
            return FALSE; 
        } 
    }

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (ai->m_unitDB->IsNuclearAttack(m_cargo->Access(unit_idx))) { 
                return TRUE;
            } 
        }
    } 

    return TRUE; 
}

void ArmyAgent::RegisterNukeDeath(AiMain *ai)
{
	if (IsNuclearAttack(ai) &&
		ai->m_world->IsSafeFromNukes(*m_my_pos))
	{
		ai->m_map->RegisterNukeSafeCity(*m_my_pos);
	}
}

BOOL ArmyAgent::CanSpaceLaunch(AiMain *ai)
{
    sint32 idx_unit; 
    for (idx_unit=0; idx_unit<m_nUnits; idx_unit++) { 
        if (ai->m_unitDB->CanSpaceLaunch(m_unit_type[idx_unit]) == FALSE) {
            return FALSE; 
        } 
    }

	

    return TRUE; 
}

BOOL ArmyAgent::CanSpaceLand(AiMain *ai)
{
    sint32 idx_unit; 
    for (idx_unit=0; idx_unit<m_nUnits; idx_unit++) { 
        if (ai->m_unitDB->CanSpaceLand(m_unit_type[idx_unit]) == FALSE) {
            return FALSE; 
        } 
    }

	

    return TRUE; 
}

BOOL ArmyAgent::CanMoveSpace(AiMain *ai) 
{
    sint32 idx_unit; 

    for (idx_unit=0; idx_unit<m_nUnits; idx_unit++) {
        if (!ai->m_unitDB->CanMoveSpace(m_unit_type[idx_unit])) { 
            return FALSE; 
        } 
    } 

	

    return TRUE; 
}

BOOL ArmyAgent::CanMoveAir(AiMain *ai)
{
     sint32 idx_unit; 

    for (idx_unit=0; idx_unit<m_nUnits; idx_unit++) {
        if (!ai->m_unitDB->CanMoveAir(m_unit_type[idx_unit])) { 
            return FALSE; 
        } 
    } 

	

    return TRUE; 
}

BOOL ArmyAgent::CanMoveWater(AiMain *ai)
{
     sint32 idx_unit; 

    for (idx_unit=0; idx_unit<m_nUnits; idx_unit++) {
        if (!ai->m_unitDB->CanMoveWater(m_unit_type[idx_unit]) &&
			!ai->m_unitDB->CanMoveShallowWater(m_unit_type[idx_unit]))
			{ 
				return FALSE; 
			} 
    } 

	

    return TRUE; 
}

BOOL ArmyAgent::CanMoveDeepWater(AiMain *ai)
{
     sint32 idx_unit; 

    for (idx_unit=0; idx_unit<m_nUnits; idx_unit++) {
        if (!ai->m_unitDB->CanMoveWater(m_unit_type[idx_unit]))
			return FALSE; 
    } 

	

    return TRUE; 
}

BOOL ArmyAgent::HasNoZOC(AiMain *ai)
{
	sint16 unit_idx; 

    for (unit_idx=0; unit_idx<m_nUnits; unit_idx++) {
        if (!ai->m_unitDB->IsNoZoc(m_unit_type[unit_idx])) { 
            return FALSE; 
        } 
    } 

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (!ai->m_unitDB->IsNoZoc(m_cargo->Access(unit_idx))) { 
                return FALSE;
            } 
        }
    } 

    return TRUE; 
}

BOOL ArmyAgent::CantCaptureCity(AiMain *ai)
{
	sint16 unit_idx; 

    for (unit_idx=0; unit_idx<m_nUnits; unit_idx++) {
        if (!ai->m_unitDB->IsCantCaptureCity(m_unit_type[unit_idx])) { 
            return FALSE; 
        } 
    } 

	
    if (m_cargo) { 
        sint32 n = m_cargo->Num(); 
        for (unit_idx=0; unit_idx<n; unit_idx++) { 
            if (!ai->m_unitDB->IsCantCaptureCity(m_cargo->Access(unit_idx))) { 
                return FALSE;
            } 
        }
    } 

    return TRUE; 
}


sint32 ArmyAgent::GetActualCost(AiMain *ai) 

{ 
    sint32 c = 0; 

    if ((m_state == AGENT_STATE_UNBUILT) || (m_state == AGENT_STATE_BUILDING)) { 
        return 0;
    } 

    sint32 i;
    for (i=0; i<m_nUnits; i++) { 
        c += ai->m_unitDB->GetProductionCost(m_unit_type[i]); 
    } 

    return c; 
}

void ArmyAgent::DumpStr(AiMain *ai, char *str)
{
    uint32 u_id = m_id.GetVal(); 
    BOOL is_unknown_id;

    ai->m_player->SetArmyDBGString (u_id, &is_unknown_id, str);
}

void ArmyAgent::CountUnitTypes(semi_dynamic_array <int> * actual_units) 
{
    sint16 i; 

    for (i=0; i<m_nUnits; i++) 
	{ 
		
		(*actual_units)[m_unit_type[i]]++;

    } 

}


sint32 ArmyAgent::GetUnitCount() const
{
	return m_nUnits;
}	  


sint32 ArmyAgent::GetUnitTypeByNumber(const int &i) const
{
	Assert(i < m_nUnits && i >= 0);
	return m_unit_type[i];
}

sint32 ArmyAgent::GetCargoUnitTypeByNumber(AiMain *ai, const int &i) const
{
	#ifdef _DEBUG
	
	sint32 full_slots, empty_slots;
	BOOL is_unknown_id;
    ai->m_player->ArmyGetNumCargo(&is_unknown_id, m_id.GetVal(), 
         &full_slots, &empty_slots);

	Assert(full_slots == m_cargo->Num());
	#endif

	Assert(i < m_cargo->Num() && i >= 0);
	#ifdef _DEBUG
		
		if (i >= m_cargo->Num() || i < 0)
			return 0;
	#endif
	return m_cargo->Get(i);
}







double ArmyAgent::Estimate_Army_Strength(AiMain *ai)
{
	
	double total_strength = 0;			
	


	
	total_strength = GetAttackStrength() + GetDefenseStrength();

	
	total_strength += m_agent_strength->GetRangedStrength();

	
	if (total_strength < 1.0) total_strength = 1.0;

	
	return total_strength;
}














double ArmyAgent::Get_Army_Attack_Strength(AiMain *ai)
{
	
	sint16 i;							
	double total_strength = 0;			
	double type_hitpoints;				
	double attack_value;				
	double fire_power;
	


	
	for (i = 0; i < m_nUnits; i++)
	{
		
        attack_value = ai->m_unitDB->GetAttack(m_unit_type[i]); 
		type_hitpoints = ai->m_unitDB->GetHP_WithReadiness(m_unit_type[i]);
		fire_power = ai->m_unitDB->GetFirepower(m_unit_type[i]);

		
		total_strength += attack_value * type_hitpoints * fire_power;

	} 

	
	return total_strength;
}













void ArmyAgent::Get_Army_Attack_And_Defense_Strengths
(
	AiMain *ai, 
	double & attack, 
	double & defend
)
{
    attack = m_agent_strength->GetAttack(); 
    defend = m_agent_strength->GetDefense(); 

}













void ArmyAgent::Display_Army(AiMain *ai)
{
#ifdef DISPLAY_ARMY_DETAILS
	
	sint16 i;							
	char * army_type_name;				
	


	
	for (i = 0; i < m_nUnits; i++)
	{
		
		army_type_name = ai->m_unitDB->Unittype_Int_To_String(m_unit_type[i]);

#ifdef DBGAILOG
		if (army_type_name != NULL)
		{
			if (ai->AI_VERBOSITY >= LOG_MEDIUM)
				AILOG((wstr, "        %s\n", army_type_name));
		}
#endif DBGAILOG

	} 

#ifdef DBGAILOG
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG((wstr, "    \n"));
#endif DBGAILOG

#endif
}












GOAL_TYPE ArmyAgent::GetLastGoal()
{
	return m_lastGoal;
}












void ArmyAgent::SetLastGoal(const GOAL_TYPE &type)
{
	
	m_lastGoal = type;
}


























































































BSetID ArmyAgent::GetHomeCityId()
{ 
	Assert(m_home_city_id != BSetID(0));
	return m_home_city_id; 
}  












void ArmyAgent::UnloadRefueledCargo(AiMain *ai)
{
   sint32 idx_cargo, num_cargo; 
   sint32 player_id; 
   uint32 a_id; 
   sint32 top_unit_type; 
   sint32 num_units; 
   uint32 c_id;
   BOOL unitVis; 
   BOOL cityVis; 
   BOOL mineVis;
   BOOL i_died; 
   BOOL revealed_foreign_units;
   BOOL revealed_unexplored; 
   BOOL zocViolation; 
   BOOL is_transported;
   BOOL is_unknown_id;
   sint16 dx,dy,x,y;
   MapPointData wrap_pos;
   BOOL r;
   BOOL can_be_expelled; 

   
   num_cargo = m_cargo->Num();
   BOOL did_move = FALSE; 
   sint32 unload_count=0;

   
   if (m_cargo->Num() <= 0)
	   return;

   
   for (idx_cargo=0; idx_cargo<num_cargo; idx_cargo++) 
	   
	   
	   if (ai->m_unitDB->UsesFuel(m_cargo->Get(idx_cargo)) &&
		   ai->m_player->UnitTypeCanEnter(m_cargo->Get(idx_cargo), m_my_pos))
		   unload_count++;
		   
   
   if (unload_count <= 0)
	   return;

   
   for (x = 0; x < 3 && !did_move;  x++)
	   for (y = 0; y < 3 && !did_move;  y++)
		   {
			   
			   dx = ( x <= 1 ? x :-1);
			   dy = ( y <= 1 ? y :-1);

			   
			   ai->m_map->Wrap(m_my_pos->x + dx, 
							   m_my_pos->y + dy,
							   m_my_pos->z, 
							   wrap_pos); 

			   
			   ai->m_world->GetCellContents( &wrap_pos, 
											  &player_id, 
											  &a_id, 
											  &top_unit_type, 
											  &num_units, 
											  &c_id,
											  &unitVis, 
											  &cityVis, 
											  &mineVis, 
                                              &can_be_expelled);

			   
			   
			   if (player_id != ai->my_player_index ||
				   unload_count > k_MAX_ARMY_SIZE - num_units)
				   continue;

			   
			   r = ai->m_player->
				   AiUnloadAllTransportsInArmy(&is_unknown_id, 
											   m_id.GetVal(), 
											   &wrap_pos, 
											   &did_move, 
											   &i_died, 
											   &revealed_foreign_units,
											   &revealed_unexplored, 
											   &zocViolation, 
											   &is_transported);
			   
		   }
}













sint32 ArmyAgent::DisbandObsolete(AiMain *ai)
{
	sint32 obsolete_units = 0;
	BOOL is_unknown_id; 
	CityAgent *the_city;
	MapPointData xy_pos;

	
	if (m_nUnits <= 0)
		return 0;

	
	if (m_state == AGENT_STATE_MARKED_DEAD ||
		m_state == AGENT_STATE_UNBUILT ||
		m_state == AGENT_STATE_BUILDING)
		return 0;

	
	for (int i = 0; i < m_nUnits; i++)
	{
		
		if (!ai->m_unitDB->IsUnitObsolete(m_unit_type[i]))
			return 0;
	}

	

	
	ai->m_player->Norm2XY(*m_my_pos, xy_pos); 
	if (ai->m_planner->the_strategic_map->Get_Threat(xy_pos) > 1.0)
		return 0;

	

	
	

	
	
	
	if (!(IsInCity(ai, the_city)))
		return 0;

	sint32 disbanded_units = GetNUnits();
	
	
	
	
		Assert(the_city);
		sint32 min_garrison = 
			the_city->GetMinSlaveGarrison();
		
		
		sint32 remaining_defenders = 
			the_city->GetNumUnitsInCity(ai) -
			disbanded_units;

		
		if (remaining_defenders < min_garrison)
			return 0;
	

	

	#ifdef DBGAILOG
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
	{
		AILOG((wstr, "Disbanding Army  @ %x (%d,%d)\n", 
			(int) this,
			m_my_pos->x,
			m_my_pos->y));
		Display_Army(ai);
	}
	#endif DBGAILOG

	
	ai->m_player->ExecuteAIOrder(m_id.GetVal(),
		&is_unknown_id, 
		m_my_pos, 
		UNIT_ORDER_DISBAND);

	Assert(m_state == AGENT_STATE_MARKED_DEAD);
	return disbanded_units;
}

void ArmyAgent::SetProbedWormhole(const BOOL &val)
{
	m_probed_wormhole = val;
}

BOOL ArmyAgent::HasProbedWormhole()
{
	return m_probed_wormhole;
}

sint16 ArmyAgent::LandWaterTransitionCount(AiMain *ai)
{
	BOOL prev_is_land;
	BOOL next_is_land;
	uint32 move_type;
	sint16 count = 0;
	
	ai->m_world->GetMoveType(&m_path[m_path_cur], &move_type);
	prev_is_land = (move_type & k_BIT_MOVEMENT_TYPE_LAND ||
		move_type & k_BIT_MOVEMENT_TYPE_MOUNTAIN);
	for (sint32 i = m_path_cur+1; i < m_path_cur_nPoints; i++)
	{
		ai->m_world->GetMoveType(&m_path[i], &move_type);
		next_is_land =  (move_type & k_BIT_MOVEMENT_TYPE_LAND ||
			move_type & k_BIT_MOVEMENT_TYPE_MOUNTAIN);	
		if (prev_is_land != next_is_land)
		{
			count++;
			prev_is_land = next_is_land;
		}
	}
	return count;
}
