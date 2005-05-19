

















































































































































































































#include "c3.h"
#include "c3types.h"
#include "c3errors.h"
#include "FlatPtr.h"
#include "civarchive.h"
#include "ic3world.h"
#include "ic3player.h"
#include "ic3Rand.h"

#include "Fuzzy_Variables.h"
#include "aimain.h"
#include "AiMap.h"


#include "common.h"
#include "dr_debug.h"
#include "linked_list.h"
#include "matrix.h"
#include "grassfire.h"
#include "ailog.h"


#include "Agent.h"
#include "CityAgent.h"
#include "ArmyAgent.h"
#include "ForeignAgent.h"
#include "ForeignCity.h"
#include "Foreigner.h"


#include "iparser.h"


#include "strategic_map.h"


#define LOG_STRATEGIC_MAP true











matrix<threat_type> * threat_map=NULL;	
matrix<sint8> * empire_map=NULL;		
matrix<int> * distance_to_empire=NULL;	
matrix<sint8> * enemy_empire_map=NULL;	
matrix<int> * distance_to_enemy=NULL;	
matrix<int> * distance_to_explored=NULL;


matrix<threat_type> * power_map=NULL;	


matrix<threat_type> * value_map=NULL;


matrix<GoalMapTarget * > * m_pillageGoals=NULL;
matrix<GoalMapTarget * > * m_goodyHutGoals=NULL;
matrix<GoalMapTarget * > * m_pirateGoals=NULL;


matrix<threat_type> * scratch_map=NULL;	
										
										
										

Grassfire<sint8> * empire_grassfire=NULL;	
											

Grassfire<sint8> * explored_grassfire=NULL;
											


int map_user_count = 0;





























Strategic_Map::Strategic_Map
(
	AiMain *init_ai,					
	char * strategic_AI_config_file_name
)
{
	
	rough_count_of_explored_cells = 0;
	death_map_resolution = 7;			

	
	ai = init_ai;

	
	if (strategic_AI_config_file_name != NULL)
	{
		
		Load_Map_Config(strategic_AI_config_file_name);

	} 
	else
	{
		
		Load_Map_Config(DEFAULT_AI_MAP_CONFIG_FILE);

	} 

	
	death_map = NULL;
	exploration_map = NULL;

	
	Init_Maps();

	
	our_city_count = 0;
	free_city_count = 0;
}














Strategic_Map::~Strategic_Map()
{
	
	Clean_Up();
}














Strategic_Map::Strategic_Map(AiMain *init_ai, CivArchive &archive)
{
  















	
	death_map = NULL;
	exploration_map = NULL;

    
	rough_count_of_explored_cells = 0;
	m_max_value = 1.0;
	m_max_power = 1.0;
	m_space_threat = 0.0;

	death_map = NULL;

	ai = init_ai;

    Serialize(archive);

 	
	Init_Maps();



















}







ZEROMEM(Strategic_Map_Flat_Data); 
FLATSERIALIZE(Strategic_Map_Flat_Data); 


void Strategic_Map::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    Strategic_Map_Flat_Data::Serialize(archive);

    sint32 rows, columns, x, y;

	
	if (archive.IsStoring())
	{
        if (death_map) { 
            archive.PutSINT32(1); 

            rows = death_map->Get_Rows();
		    columns = death_map->Get_Columns();
            archive.PutSINT32(rows);
            archive.PutSINT32(columns);            
            for (x=0; x<rows; x++) { 
                for (y=0; y<columns; y++) { 
                    archive << ((*death_map)(x,y));
                }
            }
        } else { 
            archive.PutSINT32(0); 
        } 

        if (exploration_map) { 
            archive.PutSINT32(1); 
            rows = exploration_map->Get_Rows();
		    columns = exploration_map->Get_Columns();
            archive.PutSINT32(rows);
            archive.PutSINT32(columns);            
            for (x=0; x<rows; x++) { 
                for (y=0; y<columns; y++) { 
                    archive.PutSINT32((*exploration_map)(x,y));
                }
            }
        } else { 
            archive.PutSINT32(0); 
        }

	}
	else
	{
        sint32 is_death_map;
        is_death_map = archive.GetSINT32(); 
        if (is_death_map) { 
            rows = archive.GetSINT32(); 
            columns = archive.GetSINT32(); 
            death_map = new matrix<double>(rows, columns); 
            for (x=0; x<rows; x++) { 
                for (y=0; y<columns; y++) { 
                    archive >> (*death_map)(x,y);
                }
            }

        } else { 
            death_map = NULL; 
        } 

        sint32 is_exploration_map;
        is_exploration_map = archive.GetSINT32(); 
        if (is_exploration_map) { 
            rows = archive.GetSINT32(); 
            columns = archive.GetSINT32(); 
            exploration_map = new matrix<sint8>(rows, columns); 
            for (x=0; x<rows; x++) { 
                for (y=0; y<columns; y++) { 
                    (*exploration_map)(x,y) = (sint8) archive.GetSINT32();
                }
            }
        } else { 
            exploration_map = NULL; 
        } 

	}



























































































































































































































































































}







sint32 Strategic_Map::Get_Our_City_Count()
{
	return our_city_count;
}


















void Strategic_Map::Update_Death_Differential()
{
	
	sint32 row, column;					
	double the_death_differential;		
	double the_diminishment;			
	

	
	the_diminishment = ai->m_planner->the_AIP.danger_diminishment;

	
	for (row = 0; row < death_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < death_map->Get_Columns(); column++)
		{
			
			the_death_differential = (*death_map)(row, column);

			
			if (fabs (the_death_differential) <= the_diminishment)
			{
				the_death_differential = 0.0;
			}

			
			else
			{
				if (the_death_differential > 0.0)
				{
					the_death_differential -= the_diminishment;
				}
				else
				{
					the_death_differential += the_diminishment;
				}
			}

			
			(*death_map)(row, column) =	the_death_differential;

		} 

	} 

	
	m_max_deaths = Find_Map_Max(death_map);
	m_min_deaths = Find_Map_Min(death_map);
	
#ifdef LOG_STRATEGIC_MAP
	
	Log_Death_Differential();
#endif 

}













void Strategic_Map::Log_Death_Differential()
{
#ifdef DBGAILOG

#ifdef LOG_STRATEGIC_MAP

	
	sint32 row, column;					
	double the_death_differential;		
	

	if (ai->AI_VERBOSITY < LOG_MEDIUM) return;

	AILOG((wstr, "\n\n DEATH DIFFERENTIAL \n\n"));

	
	for(row = 0; row < death_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < death_map->Get_Columns(); column++)
		{
			
			the_death_differential = (*death_map)(row, column);

			AILOG((wstr, "%4.2f ", the_death_differential));

		} 

		
		AILOG((wstr, "\n"));

	} 

#endif 
#endif DBGAILOG

}















void Strategic_Map::Update_Threats()
{
	
	sint32 i;
    ForeignAgent *him= NULL;			
    ForeignCity *hisCity= NULL;			
    BSet<ForeignAgent> *vu=NULL;		
    BSet<ForeignCity> *cu=NULL;			
    BSetID h_id;						
	double this_unit_threat;			
	sint32 row, column;					
	MapPointData his_pos;				
	bool log_threats;					
	

#ifdef LOG_STRATEGIC_MAP
	
	log_threats = false;
#endif 

	
	Clear_Threats();
    BOOL compute_enemy_strength_success; 
    MapPointData norm_pos; 
    float attack; 
    sint32 defend_unit_count;
    float defend; 
    sint32 ranged_unit_count;
    float ranged;

	
    for (i=0; i<k_MAX_PLAYERS; i++) 
	{ 
		
        if (ai->m_foreigner[i]) 
		{ 
			
			
			double unit_reguard = ai->m_foreigner[i]->GetUnitRegard();
			
			if(unit_reguard > 100.0) { unit_reguard = 100.0; }
			double AtWarModifier;
			
			
			if (ai->m_foreigner[i]->IsAtHotWar())
				AtWarModifier = 1.0;
			else
				AtWarModifier = (101.0 - unit_reguard)*0.01;

			
            cu = ai->m_foreigner[i]->GetKnownCities();
			
            for (hisCity=cu->First(h_id); cu->Last(); hisCity=cu->Next(h_id)) 
			{ 
				
				hisCity->GetXYPos(ai, i, his_pos);

				
				XY_to_MapgridRC(his_pos.x, his_pos.y, 
								row, column, threat_map_resolution);
			
				
				
				

                hisCity->GetPos(norm_pos); 
                
                compute_enemy_strength_success = ComputeEnemyStrengthAt(ai, norm_pos, attack, defend_unit_count, defend, 
                    ranged_unit_count, ranged);

				
			    Assert( attack < 99999.0); 
				Assert( defend < 99999.0); 
				Assert( ranged < 99999.0); 

				
				if (defend > 99999.0)
					defend = 100.0;

                if (attack < 1.0) { 
                    attack = 100.0; 
                } 
            
				
				
			    
                this_unit_threat = attack + ranged;

				
				if (his_pos.z == SPACE_Z)
					{
						m_space_threat += (float)(this_unit_threat*AtWarModifier);
					}
            
				
				

                Assert(0 <= (this_unit_threat*AtWarModifier));
				(*threat_map)(row, column) = (threat_type) ((*threat_map)(row, column)
					+ (this_unit_threat*AtWarModifier));

#ifdef LOG_STRATEGIC_MAP
				
				log_threats = true;
#endif 
            } 

			
            vu = ai->m_foreigner[i]->GetVisibleUnits();

			
            for (him = vu->First(h_id); vu->Last(); him =  vu->Next(h_id)) 
			{ 
				
				him->GetXYPos(his_pos);

				
				XY_to_MapgridRC(his_pos.x, his_pos.y, 
								row, column, threat_map_resolution);
			

				
                
				
                him->GetPos(norm_pos); 
                compute_enemy_strength_success = ComputeEnemyStrengthAt(ai, norm_pos, attack, defend_unit_count, defend, 
                    ranged_unit_count, ranged);
                Assert(compute_enemy_strength_success); 

				
			    Assert( attack < 99999.0); 
				Assert( defend < 99999.0); 
				Assert( ranged < 99999.0); 

				
				if (defend > 99999.0)
					defend = 100.0;

				
                
                
                
                
				
                
				this_unit_threat = attack + ranged;

				
				
				
				
                 Assert(0 <= (this_unit_threat*AtWarModifier));

				 
				if (him->IsSpaceAttack(ai))
					{
						
						m_space_threat += (float)(this_unit_threat*AtWarModifier);
						if (him->IsSpaceLand(ai))
							this_unit_threat = 0.0;
					}

				(*threat_map)(row, column) = (float) ((*threat_map)(row, column)
					+ (this_unit_threat*AtWarModifier));


#ifdef LOG_STRATEGIC_MAP
				
				log_threats = true;
#endif 
            } 

		} 
	
    }

	
	m_max_threat = Find_Map_Max(threat_map);

#ifdef LOG_STRATEGIC_MAP
	
	if (log_threats) Log_Threats();
#endif 

	
	
	
	Relax_Threats();

	
	Update_Overall_Threat();

	
	Update_Power_Ratio();

#ifdef LOG_STRATEGIC_MAP
	
	if (log_threats) Log_Threats();
#endif 

}













void Strategic_Map::Update_Overall_Threat()
{
	
	sint32 row, column;					
	sint32 dist_row, dist_col;			
	sint32 x, y;						
	double cell_threat;					
	double cell_distance;				
	

	
	if (ai->fuzzy_variables)
		ai->fuzzy_variables->overall_threat = 0.0;

	
	summed_threat = 0.0;

	
	if (our_city_count < 1) return;

	
	for(row = 0; row < threat_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < threat_map->Get_Columns(); column++)
		{
			
			MapgridRC_to_XY(row, column, x, y, threat_map_resolution);

			
			XY_to_MapgridRC(x, y, dist_row, dist_col, empire_map_resolution);

			
			cell_distance = (double) (*empire_map)(dist_row, dist_col);

			
			
			
			
			if (cell_distance < 0.999) 
				cell_distance = INSIDE_EMPIRE_DISTANCE_DIVISOR;

			
			cell_threat = (*threat_map)(row, column);

			
			summed_threat += cell_threat;

			
			cell_threat /= cell_distance;

			
			Assert(ai->fuzzy_variables);
			ai->fuzzy_variables->overall_threat += cell_threat;

		} 

	} 

	
	Assert(ai->fuzzy_variables);
	ai->fuzzy_variables->overall_threat /= our_city_count;
}











float Strategic_Map::Find_Map_Max( matrix<float> *map)
{
	
	sint32 row, column;
	float max;
	
	
	max = 1.0;
	
	
	for(row = 0; row < map->Get_Rows(); row++)
	{
		
		for (column = 0; column < map->Get_Columns(); column++)
		{
			
			if ((*map)(row, column) > max)
				max = (*map)(row, column);
		} 
	} 
	return max;
}










float Strategic_Map::Find_Map_Min( matrix<float> *map)
{
	
	sint32 row, column;
	float min;
	
	
	min = 999999.0;
	
	
	for(row = 0; row < map->Get_Rows(); row++)
	{
		
		for (column = 0; column < map->Get_Columns(); column++)
		{
			
			if ((*map)(row, column) < min)
				min = (*map)(row, column);
		} 
	} 
	return min;
}











double Strategic_Map::Find_Map_Max( matrix<double> *map)
{
	
	sint32 row, column;
	double max;
	
	
	max = 1.0;
	
	
	for(row = 0; row < map->Get_Rows(); row++)
	{
		
		for (column = 0; column < map->Get_Columns(); column++)
		{
			
			if ((*map)(row, column) > max)
				max = (*map)(row, column);
		} 
	} 
	return max;
}










double Strategic_Map::Find_Map_Min( matrix<double> *map)
{
	
	sint32 row, column;
	double min;
	
	
	min = 999999.0;
	
	
	for(row = 0; row < map->Get_Rows(); row++)
	{
		
		for (column = 0; column < map->Get_Columns(); column++)
		{
			
			if ((*map)(row, column) < min)
				min = (*map)(row, column);
		} 
	} 
	return min;
}















void Strategic_Map::Update_Power_Map()
{
	
    ArmyAgent *army_agent= NULL;		
	CityAgent *city_agent= NULL;		
    BSetID army_id;						
	double this_unit_threat;			
	sint32 row, column;					
	MapPointData pos;					
	double city_threat;					
	

	
	Clear_Power_Map();

	
	
	for (city_agent=ai->m_city_set->First(army_id); 
		ai->m_city_set->Last(); 
		city_agent=ai->m_city_set->Next(army_id)) 
		{ 
			
			city_agent->GetXYPos(ai, pos);

			
			XY_to_MapgridRC(pos.x, pos.y, 
							row, column, threat_map_resolution);
			
			
			city_threat = city_agent->Estimate_City_Strength(ai);

			
			
			(*power_map)(row, column) = (threat_type) ((*power_map)(row, column)
					+ city_threat);
            } 

			
            for (army_agent = ai->m_army_set->First(army_id); 
				 ai->m_army_set->Last(); 
				 army_agent =  ai->m_army_set->Next(army_id)) 
				{ 
					
					army_agent->GetXYPos(ai, pos);

					
					XY_to_MapgridRC(pos.x, pos.y, 
									row, column, threat_map_resolution);
			
					
					this_unit_threat = 
						army_agent->Estimate_Army_Strength(ai);

				(*power_map)(row, column) = (threat_type) ((*power_map)(row, column)
					+ this_unit_threat);
				}

	
	m_max_power = Find_Map_Max(power_map);

	
	
	

	
	
}











void Strategic_Map::Clear_Power_Map()
{
	
	sint32 row, column;					
	

    

    Assert(power_map); 
	
	for(row = 0; row < power_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < power_map->Get_Columns(); column++)
		{
			
			(*power_map)(row, column) = 0.0;

		} 

	} 
}











void Strategic_Map::Relax_Power_Map()
{
	
	sint32 row, column;					
	sint32 relax_cycle_count = 0;		
	threat_type power;						
	matrix<threat_type> *temp_map;
	

    Assert(power_map);

	
	scratch_map->Fast_Copy(*power_map);
	
















		
		
		for(row = 0; row < power_map->Get_Rows(); row++)
		{
			
			for (column = 0; column < power_map->Get_Columns(); column++)
			{
				
				power = (*power_map)(row, column);

				
				if (power > MIN_THREAT_VALUE)
				{

					
					Relax_Two_Cells(row, column, row-1, column, 
									RELAX_SHARED_SIDE, power_map, scratch_map, power, relax_cycle_count);
					Relax_Two_Cells(row, column, row+1, column, 
									RELAX_SHARED_SIDE, power_map, scratch_map, power, relax_cycle_count);
					Relax_Two_Cells(row, column, row, column-1, 
									RELAX_SHARED_SIDE, power_map, scratch_map, power, relax_cycle_count);
					Relax_Two_Cells(row, column, row, column+1, 
									RELAX_SHARED_SIDE, power_map, scratch_map, power, relax_cycle_count);

					
					Relax_Two_Cells(row, column, row-1, column-1, 
									RELAX_DIAGONAL, power_map, scratch_map, power, relax_cycle_count);
					Relax_Two_Cells(row, column, row-1, column+1, 
									RELAX_DIAGONAL, power_map, scratch_map, power, relax_cycle_count);
					Relax_Two_Cells(row, column, row+1, column-1, 
									RELAX_DIAGONAL, power_map, scratch_map, power, relax_cycle_count);
					Relax_Two_Cells(row, column, row+1, column+1, 
									RELAX_DIAGONAL, power_map, scratch_map, power, relax_cycle_count);

				} 

			} 
		} 













	
	temp_map = power_map;
	power_map = scratch_map;
	scratch_map = temp_map;


}















void Strategic_Map::Update_Value_Map()
{
	
    ArmyAgent *army_agent= NULL;		
	CityAgent *city_agent= NULL;		
    BSetID army_id;						
	double value;						
	sint32 row, column;					
	MapPointData pos;					
	

	
	Clear_Value_Map();

	
	for (city_agent=ai->m_city_set->First(army_id); 
		 ai->m_city_set->Last(); 
		 city_agent=ai->m_city_set->Next(army_id)) 
		{ 
			
			city_agent->GetXYPos(ai, pos);

			
			XY_to_MapgridRC(pos.x, pos.y, 
							row, column, threat_map_resolution);
			
			
			value = city_agent->Estimate_City_Value(ai);

			
			

			(*value_map)(row, column) = (threat_type) ((*value_map)(row, column)
					+ value);
		} 

	
	for (army_agent = ai->m_army_set->First(army_id); 
		 ai->m_army_set->Last(); 
		 army_agent =  ai->m_army_set->Next(army_id)) 
		{ 
			if (!army_agent->GetArmyContainsSettler())
				continue;

			
			army_agent->GetXYPos(ai, pos);

			
			XY_to_MapgridRC(pos.x, pos.y, 
							row, column, threat_map_resolution);
			
			
			value = (double) army_agent->GetSettlersCost(ai);

			
			

			(*value_map)(row, column) = (threat_type) ((*value_map)(row, column)
				+ value);
		}

	
	MapPointData *map_size = ai->m_map->GetSize();
	uint32 route_idx;
	uint32 num_routes;
	PLAYER_INDEX src_owner;
	PLAYER_INDEX dst_owner;
	pos.z = GROUND_Z;
	for (pos.y = 0; pos.y < map_size->y; pos.y++)
	{
		
		for (pos.x = 0; pos.x < map_size->x; pos.x++)
		{
            
            
            if (ai->m_world->IsCityHere(&pos))
                continue;

			
			XY_to_MapgridRC(pos.x, pos.y, 
							row, column, threat_map_resolution);
			            
			
			num_routes =
				ai->m_world->GetNumTradeRoutes(pos);
			
			
			for (route_idx = 0; route_idx < num_routes; route_idx++)
				{
					src_owner = ai->m_world->
						GetTradeRouteSource(pos, route_idx);
					dst_owner = ai->m_world->
						GetTradeRouteDestination(pos, route_idx);
					
					if (dst_owner != ai->my_player_index && 
						src_owner != ai->my_player_index)
						continue;
					
					value = ai->m_world->GetTradeRouteGoldInReturn(
						pos,
						route_idx);

					
					
					if (value == 0.0)
						value += 10.0;

					
					
					(*value_map)(row, column) = (threat_type) ((*value_map)(row, column)
						+ value);
				}
		} 
	} 

	
	m_max_value = Find_Map_Max(value_map);

	
	
	
	
	
	
}











void Strategic_Map::Clear_Value_Map()
{
	
	sint32 row, column;					
	

    Assert(value_map); 
	
	for(row = 0; row < value_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < value_map->Get_Columns(); column++)
		{
			
			(*value_map)(row, column) = 0.0;

		} 

	} 
}











void Strategic_Map::Relax_Value_Map()
{
	
	sint32 row, column;					
	sint32 relax_cycle_count = 0;			
	threat_type value;						
	matrix<threat_type> *temp_map;
	












	
	scratch_map->Fast_Copy(*value_map);





		
		
		for(row = 0; row < value_map->Get_Rows(); row++)
		{
			
			for (column = 0; column < value_map->Get_Columns(); column++)
			{
				
				value = (*value_map)(row, column);

				
				if (value > MIN_THREAT_VALUE)
				{
					
					Relax_Two_Cells(row, column, row-1, column, 
									RELAX_SHARED_SIDE, value_map, scratch_map, value, relax_cycle_count);
					Relax_Two_Cells(row, column, row+1, column, 
									RELAX_SHARED_SIDE, value_map, scratch_map, value, relax_cycle_count);
					Relax_Two_Cells(row, column, row, column-1, 
									RELAX_SHARED_SIDE, value_map, scratch_map, value, relax_cycle_count);
					Relax_Two_Cells(row, column, row, column+1, 
									RELAX_SHARED_SIDE, value_map, scratch_map, value, relax_cycle_count);

					
					Relax_Two_Cells(row, column, row-1, column-1, 
									RELAX_DIAGONAL, value_map, scratch_map, value, relax_cycle_count);
					Relax_Two_Cells(row, column, row-1, column+1, 
									RELAX_DIAGONAL, value_map, scratch_map, value, relax_cycle_count);
					Relax_Two_Cells(row, column, row+1, column-1, 
									RELAX_DIAGONAL, value_map, scratch_map, value, relax_cycle_count);
					Relax_Two_Cells(row, column, row+1, column+1, 
									RELAX_DIAGONAL, value_map, scratch_map, value, relax_cycle_count);

				} 

			} 
		} 

		













	
	
	temp_map = power_map;
	power_map = scratch_map;
	scratch_map = power_map;


}










double Strategic_Map::Get_Power_Bonus(AiMain *ai, MapPointData xyPos)
{
	return ((Get_Power(xyPos) / Get_Max_Power()) *
			ai->m_planner->the_AIP.max_power_raw_bonus);
}

double Strategic_Map::Get_Value_Bonus(AiMain *ai, MapPointData xyPos)
{
	return ((Get_Value(xyPos) / Get_Max_Value ()) *
			ai->m_planner->the_AIP.max_value_raw_bonus);	
}

double Strategic_Map::Get_Danger_Bonus(AiMain *ai, MapPointData xyPos)
{
	
	return ((Get_Death_Differential(xyPos) - Get_Min_Danger()) / 
			(Get_Max_Danger () - Get_Min_Danger()) *
			ai->m_planner->the_AIP.max_danger_raw_bonus);	
}

double Strategic_Map::Get_Threat_Bonus(AiMain *ai, MapPointData xyPos)
{
	return ((Get_Threat(xyPos) / Get_Max_Threat ()) *
			ai->m_planner->the_AIP.max_threat_raw_bonus);	
}








void Strategic_Map::Update_Power_Ratio()
{
	
    ArmyAgent *agent=NULL; 
    BSetID id; 
	double capped_power_ratio;			
	

	
	our_summed_power = 0.0;

    
    for (agent = ai->m_army_set->First(id); ai->m_army_set->Last(); agent = ai->m_army_set->Next(id)) 
	{ 
		
		our_summed_power += agent->Get_Army_Attack_Strength(ai);

	} 

	
	
	if (our_summed_power < 0.01)
	{
		
		if (summed_threat < 0.01)
			capped_power_ratio = (MAX_POWER_RATIO/2);
		else
			capped_power_ratio = MAX_POWER_RATIO;

	} 

	else 
	{
		
		capped_power_ratio = summed_threat/our_summed_power;

		
		if (capped_power_ratio > MAX_POWER_RATIO)
			capped_power_ratio = MAX_POWER_RATIO;

	} 
  
	
	Assert(ai->fuzzy_variables);
	ai->fuzzy_variables->power_ratio = capped_power_ratio;
}













void Strategic_Map::Update_Our_Empire(AiMain *ai)
{
	
    CityAgent *ca = NULL; 
    BSetID id; 
	sint32 row, column;					
	MapPointData city_pos;				
	

	
	Clear_Our_Empire();

	
	for ( ca = ai->m_city_set->First(id); 
		  ai->m_city_set->Last(); 
		  ca = ai->m_city_set->Next(id) )
	{
		
		ca->GetXYPos(ai, city_pos);

		
		XY_to_MapgridRC(city_pos.x, city_pos.y, 
						row, column, empire_map_resolution);

		
		(*empire_map)(row, column) = (*empire_map)(row, column) + 1;

		
		our_city_count++;

		
        if (ca->GetQueueLen() < 1) 
			free_city_count++;

	} 

	
	empire_grassfire->Xform(empire_map, distance_to_empire,
						   0, DISTANCE_INSIDE, BORDER_IS_TARGET, 
						   EIGHT_CONNECTED,
							ai->m_world->IsXwrap(), ai->m_world->IsYwrap());

#ifdef LOG_STRATEGIC_MAP
	
	Log_Our_Empire();

#endif 
}














void Strategic_Map::Update_Enemy_Empire(AiMain *ai)
{
	
    sint32 i; 
    BSet<ForeignCity> *vc=NULL; 
    BSetID c_id; 
    ForeignCity *his_city=NULL; 
	MapPointData city_pos;				
	sint32 row, column;					
#ifdef LOG_STRATEGIC_MAP
	bool log_enemy_empire;				
#endif 
	


#ifdef LOG_STRATEGIC_MAP
	
	log_enemy_empire = false;
#endif 

    MapPointData *max_size =  ai->m_map->GetSize();
 
    
	Clear_Enemy_Empire();

	
    for (i=0; i<k_MAX_PLAYERS; i++) 
	{ 
		
        if (ai->m_foreigner[i]) 
		{ 
			
 			
            if (ai->m_foreigner[i]->IsAtHotWar() ||
                ai->m_foreigner[i]->IsAtColdWar() ||
				!ai->m_foreigner[i]->IsRobot()) 
			{ 
				
                vc = ai->m_foreigner[i]->GetKnownCities();

				
                for (his_city = vc->First(c_id); vc->Last(); his_city =  vc->Next(c_id)) 
				{
					
					his_city->GetXYPos(ai, i, city_pos);

					

                    Assert((0 <= city_pos.x) && (city_pos.x < (2 * max_size->x)));
                    Assert((0 <= city_pos.y) && (city_pos.y < max_size->y));

					
					if ((city_pos.x < 0 ) || 
						(city_pos.x >= (2 * max_size->x)) ||
						(city_pos.y < 0 ) || 
						(city_pos.y >= max_size->y))
						continue;

					XY_to_MapgridRC(city_pos.x, city_pos.y, 
									row, column, empire_map_resolution);

					
					
					(*enemy_empire_map)(row, column) = 
						(*enemy_empire_map)(row, column) + 1;

#ifdef LOG_STRATEGIC_MAP
					
					log_enemy_empire = true;
#endif 

                } 

            } 

        }  

    }  

	
	empire_grassfire->Xform(enemy_empire_map, distance_to_enemy,
						   0, DISTANCE_INSIDE, BORDER_IS_TARGET, 
						   EIGHT_CONNECTED,
							ai->m_world->IsXwrap(), ai->m_world->IsYwrap());

#ifdef LOG_STRATEGIC_MAP
	
	if (log_enemy_empire) Log_Enemy_Empire();
#endif 

}













void Strategic_Map::Update_Explored_Territory(AiMain *ai)
{
	
	sint32 row, column;					
	sint32 left, top;					
	int i, j;
	MapPointData norm_pos;				
	MapPointData XY_pos;				
	bool explored;						
	int explored_mapgrid_count = 0;		
    MapPointData *map_size =  ai->m_map->GetSize();
	

	
	XY_pos.z = norm_pos.z = 0;

	
	
	
	for(row = 0; row < exploration_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < exploration_map->Get_Columns(); column++)
		{
			
			if (not (*exploration_map)(row, column))
			{
				
				explored = false;

				
				left = column * exploration_map_resolution;
				top = row *exploration_map_resolution;

				
				for (i = 0; 
					(i < exploration_map_resolution) and (not explored); 
					i++)
				{
					
					XY_pos.y = top + i;

					
					for (j = 0; 
						(j < exploration_map_resolution) and (not explored); 
						j++)
					{
						
						XY_pos.x = left + j;

						
						if ((XY_pos.y > map_size->y) or
							(XY_pos.x > 2*map_size->x))
							continue;


						
						ai->m_player->XY2Norm(XY_pos, norm_pos);

						
						if (ai->m_world->IsExplored(&norm_pos))
						{
							
							(*exploration_map)(row, column) = true;
							explored = true;

						} 


					} 

				} 

			} 

			
			if ((*exploration_map)(row, column))
				explored_mapgrid_count++;

		} 

	} 

	
	rough_count_of_explored_cells = explored_mapgrid_count * 
		exploration_map_resolution * exploration_map_resolution;






























	
	
	
	



	explored_grassfire->Xform(exploration_map, distance_to_explored,
						   true, DISTANCE_OUTSIDE, BORDER_IS_NOT_TARGET, 
						   EIGHT_CONNECTED, ai->m_world->IsXwrap(), ai->m_world->IsYwrap());


#ifdef LOG_STRATEGIC_MAP
	
	Log_Exploration_Map();
#endif 

}













































































void Strategic_Map::Adjust_Death_Differential
(
	double adjustment,					
	MapPointData &query_pos				
)
{
	
	sint32 row, column;					
	

	
	XY_to_MapgridRC(query_pos.x, query_pos.y, row, column, 
					death_map_resolution);

	if ((row >= 0) && (row < death_map->Get_Rows()) &&
		(column >= 0) && (column < death_map->Get_Columns()))
	{
		(*death_map)(row, column) += adjustment;
	}
}













double Strategic_Map::Get_Death_Differential
(
	MapPointData &query_pos				
)
{
	
	sint32 row, column;					
	

	
	XY_to_MapgridRC(query_pos.x, query_pos.y, row, column, 
					death_map_resolution);		

	
	return (*death_map)(row, column);
}













double Strategic_Map::Get_Threat
(
	MapPointData &query_pos				
)
{
	
	sint32 row, column;					
	double value;
	

	
	if (query_pos.z == SPACE_Z)
		return m_space_threat;

	
	XY_to_MapgridRC(query_pos.x, query_pos.y, row, column, 
					threat_map_resolution);

	value = (*threat_map)(row, column);
	

	
	return value;
}







double Strategic_Map::Get_Power(MapPointData &query_pos)
{
	
	sint32 row, column;					
	

	
	XY_to_MapgridRC(query_pos.x, query_pos.y, row, column, 
					threat_map_resolution);

	
	return (*power_map)(row, column);
}

double Strategic_Map::Get_Value(MapPointData &query_pos)
{
	
	sint32 row, column;					
	

	
	XY_to_MapgridRC(query_pos.x, query_pos.y, row, column, 
					threat_map_resolution);

	
	return (*value_map)(row, column);
}








int Strategic_Map::Get_Distance_To_Our_Empire
(
	MapPointData &query_pos				
)
{
	
	sint32 row, column;					
	

	
	if (our_city_count == 0) return 0;

	
	XY_to_MapgridRC(query_pos.x, query_pos.y, row, column, 
					empire_map_resolution);

    Assert(0 <= (*distance_to_empire)(row, column));
	
	return (*distance_to_empire)(row, column);
}













int Strategic_Map::Get_Distance_To_Enemy_Empire
(
	MapPointData &query_pos				
)
{
	
	sint32 row, column;					
	

	
	XY_to_MapgridRC(query_pos.x, query_pos.y, row, column, 
					empire_map_resolution);

	
	return (*distance_to_enemy)(row, column);
}






	






int Strategic_Map::Get_Distance_To_Enemy_Empire
(
	sint32 row,							
	sint32 column						
)
{

	
	return (*distance_to_enemy)(row, column);
}






	






void Strategic_Map::Get_Empire_Map_Dimensions
(
	sint32 & rows,						
	sint32 & columns					
)
{
	
	rows = empire_map->Get_Rows();
	columns = empire_map->Get_Columns();
}













sint32 Strategic_Map::Get_Distance_To_Empire_Mapgrid_Value
(
	sint32 row,							
	sint32 column						
)
{
	
	return (*distance_to_empire)(row, column);
}













void Strategic_Map::Get_Empire_Mapgrid_Center_Tile
(
	sint32 row,							
	sint32 column,						
	MapPointData &norm_pos				
)
{
	
	MapPointData xy_pos;				
	

	
	xy_pos.x = sint16(column * empire_map_resolution);
	xy_pos.y = sint16(row * empire_map_resolution);

	
	xy_pos.z = GROUND_Z;

	
	xy_pos.x += sint16(empire_map_resolution/2);
	xy_pos.y += sint16(empire_map_resolution/2);

	
	ai->m_player->XY2Norm(xy_pos, norm_pos);

}













void Strategic_Map::Get_Empire_Mapgrid_Random_Tile
(
	sint32 row,							
	sint32 column,						
	MapPointData &norm_pos				
)
{
	
	MapPointData xy_pos;				
	

	
	xy_pos.x = sint16(column * empire_map_resolution);
	xy_pos.y = sint16(row * empire_map_resolution);

	
	xy_pos.z = GROUND_Z;

	
	xy_pos.x += sint16(ai->m_rand->Next(empire_map_resolution));
	xy_pos.y += sint16(ai->m_rand->Next(empire_map_resolution));

	
	ai->m_player->XY2Norm(xy_pos, norm_pos);

}













void Strategic_Map::Get_Explore_Map_Dimensions
(
	sint32 & rows,						
	sint32 & columns					
)
{
	
	rows = exploration_map->Get_Rows();
	columns = exploration_map->Get_Columns();
}













sint32 Strategic_Map::Must_We_Explored_Here
(
	sint32 row,							
	sint32 column						
)
{
	
	if ((*distance_to_explored)(row, column) == 1)
		return TRUE;

	
	return FALSE;
}













void Strategic_Map::Get_Explore_Mapgrid_Center_Tile
(
	sint32 row,							
	sint32 column,						
	MapPointData &norm_pos				
)
{
	
	MapPointData xy_pos;				
	

	
	xy_pos.x = sint16(column * exploration_map_resolution);
	xy_pos.y = sint16(row * exploration_map_resolution);

	
	xy_pos.z = GROUND_Z;

	
	xy_pos.x += exploration_map_resolution/2;
	xy_pos.y += exploration_map_resolution/2;

	
	ai->m_player->XY2Norm(xy_pos, norm_pos);
}













BOOL Strategic_Map::Get_Unexplore_Mapgrid_Neighbor_Tile
(
	sint32 row,							
	sint32 column,						
	MapPointData &norm_pos,				
	uint32 corner,						
	BOOL x_wrap,						
	BOOL y_wrap							
)
{
	
	MapPointData xy_pos;				
	

	



	
	xy_pos.x = sint16(column * exploration_map_resolution);
	xy_pos.y = sint16(row * exploration_map_resolution);

	switch (corner)
	{
		case 0:
			row = row -1;
			column = column -1;
			break;
		case 1:
			row = row +1;
			column = column -1;
			break;
		case 2:
			row = row +1;
			column = column +1;
			break;
		case 3:
			row = row -1;
			column = column +1;
			break;
		case 4:
			row = row;
			column = column -1;
			break;
		case 5:
			row = row;
			column = column +1;
			break;
		case 6:
			row = row +1;
			column = column;
			break;
		case 7:
			row = row -1;
			column = column;
			break;

	}	

	if (!y_wrap &&
		(row < 0 || row >= distance_to_explored->Get_Rows()))
		return FALSE;

	if (!x_wrap &&
		(column < 0 || column >= distance_to_explored->Get_Columns()))
		return FALSE;

	if (row < 0) row = distance_to_explored->Get_Rows() - 1;
	if (column < 0) column = distance_to_explored->Get_Columns() - 1;
	if (row >= distance_to_explored->Get_Rows()) row = 0;
	if (column >= distance_to_explored->Get_Columns()) column = 0;

	Get_Explore_Mapgrid_Center_Tile( row, column, norm_pos);

	return ((*exploration_map)(row, column) == FALSE);
}





















void Strategic_Map::Load_Map_Config
(
	char * strategic_AI_config_file_name
)
{
	
	iparser * map_parameters;           
	int error = IPARSE_NO_ERR;			
	char AI_directory_path[500];		
	

	
	map_parameters = new iparser(ai);

#ifdef DBGAILOG

#ifdef LOG_STRATEGIC_MAP
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
		AILOG((wstr, "Reading AI Map Config File '%s'.\n", 
				strategic_AI_config_file_name));
#endif
#endif DBGAILOG

	
	sprintf(AI_directory_path, "%s", ai->m_gs->GetDataPath());

	
	map_parameters->Append_Path(AI_directory_path);

	
	error = map_parameters->Parse_File( strategic_AI_config_file_name );

	
	if (error is NO_FILE_ERROR)
	{
		
		AILOG((wstr, "ERROR Reading AI Map Config File '%s'.\n", 
				strategic_AI_config_file_name));

		
		Assert(error != NO_FILE_ERROR);

	} 

	

	
	threat_map_resolution = 
		map_parameters->Get_Runtime_Int("threat_map_resolution", &error);
	
	
	relaxation_coefficient = 
		map_parameters->Get_Runtime_Double("relaxation_coefficient", &error);
	
	
	relaxation_cycles = 
		map_parameters->Get_Runtime_Int("relaxation_cycles", &error);
	
	
	empire_map_resolution = 
		map_parameters->Get_Runtime_Int("empire_map_resolution", &error);

	
	death_map_resolution = 
		map_parameters->Get_Runtime_Int("death_map_resolution", &error);
		
	
	exploration_map_resolution = 
		map_parameters->Get_Runtime_Int("exploration_map_resolution", &error);

	
	goal_map_resolution = 
		map_parameters->Get_Runtime_Int("goal_map_resolution", &error);

		
	
	delete map_parameters;
    map_parameters = NULL; 
}













void Strategic_Map::Init_Maps()
{
	
	sint32 rows, columns;				
										
	

	
	map_user_count++;

	

	
	rows = (sint32) ceil(ai->m_world->GetYHeight()/(double)threat_map_resolution);
	columns = (sint32) ceil((2*ai->m_world->GetXWidth())/(double)threat_map_resolution);

	
	if (threat_map is NULL)
		threat_map = new matrix<threat_type> (rows, columns);	

	
	if (scratch_map is NULL)
		scratch_map = new matrix<threat_type> (rows, columns);
	
	
	if (power_map is NULL)
		power_map = new matrix<threat_type> (rows, columns);

	
	if (value_map is NULL)
		value_map = new matrix<threat_type> (rows, columns);

	

	
	rows = (sint32) ceil(ai->m_world->GetYHeight()/(double)death_map_resolution);
	columns = (sint32) ceil((2*ai->m_world->GetXWidth())/(double)death_map_resolution);

	
	if (death_map is NULL)
	{
		death_map = new matrix<double> (rows, columns);
		Clear_Death_Differential();
	}

	
	rows = (sint32) ceil(ai->m_world->GetYHeight()/(double)empire_map_resolution);
	columns = (sint32) ceil((2*ai->m_world->GetXWidth())/(double)empire_map_resolution);

	
	if (empire_map is NULL)
		empire_map = new matrix<sint8> (rows, columns);
	if (distance_to_empire is NULL)
		distance_to_empire = new matrix<int> (rows, columns);
    sint32 x, y; 
    for (x=0; x<rows; x++) { 
        for (y=0; y<columns; y++) { 
            (*distance_to_empire)(x,y) = 0; 
        } 
    } 

	if (empire_grassfire is NULL)
		empire_grassfire = new Grassfire<sint8> (rows, columns);

	
	
	
	if (enemy_empire_map is NULL)
		enemy_empire_map = new matrix<sint8> (rows, columns);	
	if (distance_to_enemy is NULL)
		distance_to_enemy = new matrix<int> (rows, columns);

	

	
	rows = (sint32) ceil(ai->m_world->GetYHeight()
		/(double) exploration_map_resolution);
	columns = (sint32) ceil((2*ai->m_world->GetXWidth())
		/(double) exploration_map_resolution);

	if (exploration_map is NULL)
	{
		exploration_map = new matrix<sint8> (rows, columns);
		Clear_Exploration_Map();
	}
	if (distance_to_explored is NULL)
		distance_to_explored = new matrix<int> (rows, columns);
	if (explored_grassfire is NULL)
		explored_grassfire = new Grassfire<sint8> (rows, columns);

	
	rows = (sint32) ceil(ai->m_world->GetYHeight()
		/(double) goal_map_resolution);
	columns = (sint32) ceil((2*ai->m_world->GetXWidth())
		/(double) goal_map_resolution);


	if (m_pillageGoals is NULL)
		m_pillageGoals = new matrix<GoalMapTarget *>(rows,columns);
	if (m_goodyHutGoals is NULL)
		m_goodyHutGoals = new matrix<GoalMapTarget *>(rows,columns);
	if (m_pirateGoals is NULL)
		m_pirateGoals = new matrix<GoalMapTarget *>(rows,columns);

	
	Clear_Threats();
	Clear_Our_Empire();
	Clear_Enemy_Empire();
	Clear_Goal_Map();

}













void Strategic_Map::Clean_Up()
{
	
	map_user_count--;

	Assert(map_user_count >= 0);

	delete death_map;
    death_map = NULL; 

	
	if (exploration_map) { 
		delete exploration_map;
		exploration_map = NULL; 
	} 

	
	if (map_user_count is 0)
	{
		
		
		delete threat_map;
		threat_map = NULL; 
		delete scratch_map;
		scratch_map = NULL; 

		delete power_map;
		power_map = NULL; 
		delete value_map;
		value_map = NULL; 

		
		delete empire_map;
		empire_map = NULL; 
		delete distance_to_empire;
		distance_to_empire = NULL; 
		delete empire_grassfire;
		empire_grassfire = NULL; 
		
		
		delete enemy_empire_map;
		enemy_empire_map = NULL; 
		delete distance_to_enemy;
		distance_to_enemy = NULL; 

		if (distance_to_explored) { 
			delete distance_to_explored;
			distance_to_explored = NULL; 
		} 

		if (explored_grassfire) { 
			delete explored_grassfire;
			explored_grassfire = NULL; 
		} 

		
	
	
	
	
	
		if (m_pillageGoals) {
			delete m_pillageGoals;
			m_pillageGoals = NULL;
		}

		if (m_goodyHutGoals) {
			delete m_goodyHutGoals;
			m_goodyHutGoals = NULL;
		}

		if (m_pirateGoals) {
			delete m_pirateGoals;
			m_pirateGoals = NULL;
		}
	} 
}













void Strategic_Map::Clear_Death_Differential()
{
	
	sint32 row, column;					
	

    Assert(death_map); 
	
	for(row = 0; row < death_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < death_map->Get_Columns(); column++)
		{
			
			(*death_map)(row, column) = 0.0;

		} 

	} 
}













void Strategic_Map::Clear_Threats()
{
	
	sint32 row, column;					
	

	
	if (ai->fuzzy_variables)
		ai->fuzzy_variables->overall_threat = 0.0;

    Assert(threat_map); 
	
	for(row = 0; row < threat_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < threat_map->Get_Columns(); column++)
		{
			
			(*threat_map)(row, column) = 0.0;

		} 

	} 

	
	m_space_threat = 0.0;
}




















































































void Strategic_Map::Relax_Threats()
{
	
	sint32 row, column;					
	sint32 relax_cycle_count;			
	threat_type threat;						
	matrix<threat_type> *map_from;			
	matrix<threat_type> *map_to;
	matrix<threat_type> *temp_map;
	

	
	if (relaxation_cycles <= 0) return;

	
	map_from = threat_map;
	map_to = scratch_map;

	
	scratch_map->Fast_Copy(*threat_map);

	
















	
	for (relax_cycle_count = 0; relax_cycle_count < relaxation_cycles; 
		 relax_cycle_count++)
	{
		
		
		for(row = 0; row < threat_map->Get_Rows(); row++)
		{
			
			for (column = 0; column < threat_map->Get_Columns(); column++)
			{
				
				threat = (*map_from)(row, column);

				
				if (threat > MIN_THREAT_VALUE)
				{
					
					Relax_Two_Cells(row, column, row-1, column, RELAX_SHARED_SIDE, map_from, map_to, threat, relax_cycle_count);
					Relax_Two_Cells(row, column, row+1, column, RELAX_SHARED_SIDE, map_from, map_to, threat, relax_cycle_count);
					Relax_Two_Cells(row, column, row, column-1, RELAX_SHARED_SIDE, map_from, map_to, threat, relax_cycle_count);
					Relax_Two_Cells(row, column, row, column+1, RELAX_SHARED_SIDE, map_from, map_to, threat, relax_cycle_count);

					
					Relax_Two_Cells(row, column, row-1, column-1, RELAX_DIAGONAL, map_from, map_to, threat, relax_cycle_count);
					Relax_Two_Cells(row, column, row-1, column+1, RELAX_DIAGONAL, map_from, map_to, threat, relax_cycle_count);
					Relax_Two_Cells(row, column, row+1, column-1, RELAX_DIAGONAL, map_from, map_to, threat, relax_cycle_count);
					Relax_Two_Cells(row, column, row+1, column+1, RELAX_DIAGONAL, map_from, map_to, threat, relax_cycle_count);

				} 

			} 

		} 

		
		temp_map = map_from;
		map_from = map_to;
		map_to = temp_map;

	} 


















	
	
	
	if (map_from isnt threat_map)
	{
		
		threat_map = map_from;
		scratch_map = map_to;

	} 

}













void Strategic_Map::Log_Threats()
{
#ifdef DBGAILOG

#ifdef LOG_STRATEGIC_MAP

	
	sint32 row, column;					
	threat_type the_threat;					
	

	if (ai->AI_VERBOSITY < LOG_MEDIUM) return;

	AILOG((wstr, "\n\n THREAT MAP \n\n"));
	AILOG((wstr, "Overall Threat: %f\n\n", ai->fuzzy_variables->overall_threat));

	
	for(row = 0; row < threat_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < threat_map->Get_Columns(); column++)
		{
			
			the_threat = (*threat_map)(row, column);

			AILOG((wstr, "%6.2f ", the_threat));

		} 

		
		AILOG((wstr, "\n"));

	} 

#endif 
#endif DBGAILOG

}













void Strategic_Map::Clear_Our_Empire()
{
	
	sint32 row, column;					
	

	
	our_city_count = 0;
	free_city_count = 0;

    Assert(empire_map); 
	
	for(row = 0; row < empire_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < empire_map->Get_Columns(); column++)
		{
			
			(*empire_map)(row, column) = 0;

		} 

	} 
}













void Strategic_Map::Log_Our_Empire()
{
#ifdef DBGAILOG

#ifdef LOG_STRATEGIC_MAP

	
	sint32 row, column;					
	int the_distance;					
	sint32 empire_value;				
	

	if (ai->AI_VERBOSITY < LOG_MEDIUM) return;

	AILOG((wstr, "\n\n DISTANCE TO OUR EMPIRE\n\n"));

	
	for(row = 0; row < empire_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < empire_map->Get_Columns(); column++)
		{
			
			the_distance = (*distance_to_empire)(row, column);

			
			if (the_distance == 0)
			{
				
				empire_value = (*empire_map)(row, column);
				AILOG((wstr, "[%2d]", empire_value));
			}
			else
			{

				AILOG((wstr, " %2d ", the_distance));
			}

		} 

		
		AILOG((wstr, "\n"));

	} 

#endif 
#endif DBGAILOG

}













void Strategic_Map::Clear_Enemy_Empire()
{
	
	sint32 row, column;					
	

    Assert(enemy_empire_map); 

	
	for(row = 0; row < enemy_empire_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < enemy_empire_map->Get_Columns(); column++)
		{
			
			(*enemy_empire_map)(row, column) = 0;

		} 

	} 
}













void Strategic_Map::Log_Enemy_Empire()
{
#ifdef DBGAILOG

#ifdef LOG_STRATEGIC_MAP

	
	sint32 row, column;					
	int the_distance;					
	sint32 empire_value;				
	

	if (ai->AI_VERBOSITY < LOG_MEDIUM) return;

	AILOG((wstr, "\n\n DISTANCE TO ENEMY EMPIRE \n\n"));

	
	for(row = 0; row < empire_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < empire_map->Get_Columns(); column++)
		{
			
			the_distance = (*distance_to_enemy)(row, column);

			
			if (the_distance == 0)
			{
				
				empire_value = (*enemy_empire_map)(row, column);
				AILOG((wstr, "[%4d] ", empire_value));
			}
			else
			{

				AILOG((wstr, " %4d  ", the_distance));
			}

		} 

		
		AILOG((wstr, "\n"));

	} 

#endif 
#endif DBGAILOG

}













void Strategic_Map::Clear_Exploration_Map()
{
	
	sint32 row, column;					
	

    Assert(exploration_map); 

	
	for(row = 0; row < exploration_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < exploration_map->Get_Columns(); column++)
		{
			
			(*exploration_map)(row, column) = false;

		} 

	} 
}













void Strategic_Map::Clear_Goal_Map()
{
	
	sint32 row, column;					
	

    Assert(m_pillageGoals); 
	
	for(row = 0; row < m_pillageGoals->Get_Rows(); row++)
	{
		
		for (column = 0; column < m_pillageGoals->Get_Columns(); column++)
		{
			

			(*m_pillageGoals)(row,column) = NULL;
			(*m_goodyHutGoals)(row,column) = NULL;
			(*m_pirateGoals)(row,column) = NULL;
		} 

	} 
}













void Strategic_Map::Log_Exploration_Map()
{
#ifdef DBGAILOG

#ifdef LOG_STRATEGIC_MAP

	
	sint32 row, column;					
	sint32 explored;					
	

	if (ai->AI_VERBOSITY < LOG_MEDIUM) return;

	AILOG((wstr, "\n\n EXPLORED \n\n"));

	
	for(row = 0; row < exploration_map->Get_Rows(); row++)
	{
		
		for (column = 0; column < exploration_map->Get_Columns(); column++)
		{
			
			explored = (*exploration_map)(row, column);

			
			if (explored)
			{









					AILOG((wstr, "%d", (*distance_to_explored)(row, column)));
			}
			else
			{
				if ((*distance_to_explored)(row, column) == 1)
				{
					AILOG((wstr, "X"));
				}
				else
				{
					AILOG((wstr, "?"));
				}
			}

		} 

		
		AILOG((wstr, "\n"));

	} 

#endif 
#endif DBGAILOG

}













GoalMapTarget *Strategic_Map::GetAreaGoal
(
	const MapPointData &norm_pos,
	const GOAL_TYPE &type
)
{
	
	MapPointData xy_pos;				
	GoalMapTarget *tmp_goal;
	uint32 column, row;
	

	
	if (norm_pos.z != GROUND_Z)
		return NULL;

	
	ai->m_player->Norm2XY(norm_pos, xy_pos);

	
	column = (uint32) floor(xy_pos.x / goal_map_resolution);
	row = (uint32) floor(xy_pos.y / goal_map_resolution);

	switch (type)
		{



			case GOAL_TYPE_PILLAGE:
				tmp_goal = (*m_pillageGoals)(row,column);
				break;
			case GOAL_TYPE_GOODY_HUT:
				tmp_goal = (*m_goodyHutGoals)(row,column);
				break;
			case GOAL_TYPE_PIRATE:
				tmp_goal = (*m_pirateGoals)(row,column);
				break;
			default:
				
				tmp_goal = NULL;
		}

	return tmp_goal;
}













void Strategic_Map::SetAreaGoal
(
	const MapPointData &norm_pos,
	const GOAL_TYPE &type,
	GoalMapTarget *tmp_goal	
)
{
	
	MapPointData xy_pos;				
	uint32 column, row;
	

	
	if (norm_pos.z != GROUND_Z)
		return;

	
	ai->m_player->Norm2XY(norm_pos, xy_pos);

	
	column = (uint32) floor(xy_pos.x / goal_map_resolution);
	row = (uint32) floor(xy_pos.y / goal_map_resolution);

	#ifdef _DEBUG
	if (tmp_goal != NULL)
	{
		switch (type)
		{



		case GOAL_TYPE_PILLAGE:
			Assert((*m_pillageGoals)(row,column) == NULL);
			break;
		case GOAL_TYPE_GOODY_HUT:
			Assert((*m_goodyHutGoals)(row,column) == NULL);
			break;
		case GOAL_TYPE_PIRATE:
			Assert((*m_pirateGoals)(row,column) == NULL);
			break;
		}
	}
	#endif
	
	switch (type)
		{



			case GOAL_TYPE_PILLAGE:
				(*m_pillageGoals)(row,column) = tmp_goal;
				break;
			case GOAL_TYPE_GOODY_HUT:
				(*m_goodyHutGoals)(row,column) = tmp_goal;
				break;
			case GOAL_TYPE_PIRATE:
				(*m_pirateGoals)(row,column) = tmp_goal;
				break;
			default:
				
				break;
		}
}














