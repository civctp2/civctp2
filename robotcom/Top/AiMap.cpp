














#include "c3.h"
#include "globals.h"
#include "IC3GameState.h"
#include "IC3World.h"
#include "IC3Player.h"
#include "IC3Rand.h"
#include "IC3Government.h"


#include "AiMap.h"
#include "AiCell.h"
#include "IC3GameState.h"
#include "IC3GameState.h"
#include "AiMain.h"
#include "Foreigner.h"
#include "Cont.h"


#include "dr_debug.h"
#include "matrix.h"
#include "Grassfire.h"
#include "Strategic_Map.h"


#include "BSet.h"
#include "CityAgent.h"
#include "CivArchive.h"
#include "ForeignAgent.h"
#include "ForeignCity.h"


#include "FzOut.h"


#include "ArmyGoal.h"
#include "GlSettl.h"
#include "ScienceAgent.h"
#include "FzOut.h"
#include "Cont.h"
#include "Bit_Table.h"
#include "WotP.h"




extern double fz_settler_packing; 
extern double fz_explore_near_civ; 
extern double fz_max_attack_strength; 
extern double fz_max_defense_strength; 
extern double fz_no_settle; 


extern Bit_Table *g_can_settle_ground;
extern Bit_Table *g_can_settle_space;

#define DRAW_SETTLE_GOALS 1

AiMap::AiMap(sint16 x, sint16 y, sint16 z, IC3GameState *gs)

{
    sint32 i, j; 

    m_choke_land = 0; 
    m_size = new MapPointData; 
    m_size->x = x; 
    m_size->y = y; 
    m_size->z = z; 
    
    m_can_settle_ground = new Bit_Table(x, y, TRUE); 
    m_can_settle_space = new Bit_Table(x, y, TRUE); 

    m_cellx = m_size->x / k_MINECELLX; 
    m_celly = m_size->y / k_MINECELLY; 
  









    m_fort_count = new sint32*[m_cellx];
    for(i=0; i<m_cellx; i++) { 
        m_fort_count[i] = new sint32[m_celly]; 
        for (j=0; j<m_celly; j++) { 
            m_fort_count[i][j] = 0; 
        } 
    } 

    m_airfield_count = new sint32*[m_cellx];
    for(i=0; i<m_cellx; i++) { 
        m_airfield_count[i] = new sint32[m_celly]; 
        for (j=0; j<m_celly; j++) { 
            m_airfield_count[i][j] = 0; 
        } 
    } 

    m_guardtower_count = new sint32*[m_cellx];
    for(i=0; i<m_cellx; i++) { 
        m_guardtower_count[i] = new sint32[m_celly]; 
        for (j=0; j<m_celly; j++) { 
            m_guardtower_count[i][j] = 0; 
        } 
    } 

    
    MapValueStruct ***raw=NULL; 













    m_settler_xy_range = 1; 
}

AiMap::~AiMap()
{
   sint32 i; 

 











    delete m_can_settle_ground; 
    delete m_can_settle_space; 

	delete m_size;
    m_size = NULL; 







    for (i=0; i<m_cellx; i++) { 
        delete[] m_fort_count[i]; 
    } 
    delete[] m_fort_count; 
    m_fort_count=NULL; 

    for (i=0; i<m_cellx; i++) { 
        delete[] m_airfield_count[i]; 
    } 
    delete[] m_airfield_count; 
    m_airfield_count=NULL; 

    for (i=0; i<m_cellx; i++) { 
        delete[] m_guardtower_count[i]; 
    } 
    delete[] m_guardtower_count; 
    m_guardtower_count=NULL; 

}

AiMap::AiMap(CivArchive &archive)
{
    Serialize(archive); 
}

void AiMap::Serialize(CivArchive &archive)
{
    sint32 i, j; 

    CHECKSERIALIZE

    
    if (archive.IsStoring()) { 

        m_can_settle_ground->Serialize(archive); 
        m_can_settle_space->Serialize(archive); 

        archive << m_choke_land; 

        archive << m_size->x; 
        archive << m_size->y; 
        archive << m_size->z; 











        archive << m_cellx; 
        archive << m_celly; 
        








        for (i=0; i<m_cellx; i++) { 
            for (j=0; j<m_celly; j++) { 
                archive << m_fort_count[i][j]; 
            }
        }

        for (i=0; i<m_cellx; i++) { 
            for (j=0; j<m_celly; j++) { 
                archive << m_airfield_count[i][j]; 
            }
        }

        for (i=0; i<m_cellx; i++) { 
            for (j=0; j<m_celly; j++) { 
                archive << m_guardtower_count[i][j]; 
            }
        }

        archive << m_settler_xy_range; 


    } else {
        m_can_settle_ground = new Bit_Table(archive); 
        m_can_settle_space = new Bit_Table(archive); 

        archive >> m_choke_land; 

        m_size = new MapPointData; 
        archive >> m_size->x; 
        archive >> m_size->y; 
        archive >> m_size->z; 

















        archive >> m_cellx; 
        archive >> m_celly; 








        m_fort_count = new sint32*[m_cellx];
        for(i=0; i<m_cellx; i++) { 
            m_fort_count[i] = new sint32[m_celly]; 
        } 

        m_airfield_count = new sint32*[m_cellx];
        for(i=0; i<m_cellx; i++) { 
            m_airfield_count[i] = new sint32[m_celly]; 
        } 

        m_guardtower_count = new sint32*[m_cellx];
        for(i=0; i<m_cellx; i++) { 
            m_guardtower_count[i] = new sint32[m_celly]; 
        } 

        








        for (i=0; i<m_cellx; i++) { 
            for (j=0; j<m_celly; j++) { 
                archive >> m_fort_count[i][j]; 
            }
        }

        for (i=0; i<m_cellx; i++) { 
            for (j=0; j<m_celly; j++) { 
                archive >> m_airfield_count[i][j]; 
            }
        }


        for (i=0; i<m_cellx; i++) { 
            for (j=0; j<m_celly; j++) { 
                archive >> m_guardtower_count[i][j]; 
            }
        }
 
        archive >> m_settler_xy_range; 

    }
}
















































void AiMap::Wrap(const sint16 x, const sint16 y, const sint16 z, MapPointData &w) const
{ 

    if (y < 0) { 
        w.y = m_size->y + y; 
    } else if (m_size->y <= y) { 
        w.y = y - m_size->y; 
    } else { 
        w.y = y; 
    }  

    if (x < 0) { 
        w.x = m_size->x + x; 
        w.y = w.y - m_size->y/2;
        if (w.y < 0) {
            w.y += m_size->y; 
        } 
    } else if (m_size->x <= x) { 
        w.x = x - m_size->x; 
        w.y = (w.y + m_size->y/2) % m_size->y; 
    } else { 
        w.x = x; 
    } 


    w.z = z; 
    Assert(0 <= z); 
    Assert(z < 2); 
}

double AiMap::WrappedDistance(const MapPointData &a, const MapPointData &b)

{
    MapPointData top, bottom, left, right;    
    double mind = 10000.0, maxd;
    double dx, dy; 

    dx = fabs(a.x - b.x); 
    dy = fabs(a.y - b.y); 

    if (dx < dy) { 
        maxd = dy; 
    } else { 
        maxd = dx; 
    } 
    if (maxd < mind) { 
        mind = maxd; 
    } 

    if (a.x <= b.x) { 
        top = b; 
        bottom = a; 
    } else { 
        top = a; 
        bottom = b; 
    } 


    bottom.y = (bottom.y + m_size->y/2) % m_size->y; 
    dy = fabs(top.y - bottom.y); 
    dx = fabs((m_size->x - top.x)  + bottom.x); 

    if (dx < dy) { 
        maxd = dy; 
    } else { 
        maxd = dx; 
    } 
    if (maxd < mind) { 
        mind = maxd; 
    } 

    if (bottom.y < top.y) { 
       bottom.y += m_size->y; 
    } else { 
       top.y += m_size->y; 
    }
    dy = fabs(top.y - bottom.y); 

    if (dx < dy) { 
        maxd = dy; 
    } else { 
        maxd = dx; 
    } 
    if (maxd < mind) { 
        mind = maxd; 
    } 

    if (a.y <= b.y) { 
        right = b; 
        left = a; 
    } else { 
        right = a; 
        left = b; 
    } 

    dx = fabs(right.x - left.x); 
    dy = fabs((m_size->y - right.y) + left.y); 
    if (dx < dy) { 
        maxd = dy; 
    } else { 
        maxd = dx; 
    } 
    if (maxd < mind) { 
        mind = maxd; 
    } 

    return mind; 
}

BOOL IsEqual(const MapPointData &start, const MapPointData &pos)
{
    return (start.x == pos.x) & 
        (start.y == pos.y) & 
        (start.z == pos.z);
}

BOOL AiMap::CanSeeForeigner(AiMain *ai)
{

    MapPointData pos; 
    sint32 maxz = 1; 


    for (pos.z=0; pos.z<maxz; pos.z++) { 
        for (pos.x=0; pos.x<m_size->x; pos.x++) { 
            for (pos.y=0; pos.y<m_size->y; pos.y++) { 
                if (ai->m_world->IsVisibleEnemyArmyThere (&pos)) { 
                    return TRUE; 
                }
            }
        }
    }
    return FALSE; 
}



void AiMap::GetVisibleForeigners(AiMain *ai)
{
    sint32 i;
    MapPointData pos; 
    sint32 f_id; 
    uint32 a_id, c_id;
    sint32 maxz; 
    sint32	top_unit_type; 
    sint32	unit_num;
	BOOL	unitVis, cityVis, mineVis;
	sint32 player;
	
    for(i=0; i<k_MAX_PLAYERS; i++) { 
        if (ai->m_foreigner[i]) { 
            ai->m_foreigner[i]->ClearVision(ai); 

			
			if (!ai->m_player->IsRobotPlayer(i))
			{
				if (ai->m_player->GetCapitolPos(i, &pos))
					{
						ai->m_world->GetCityId( &pos, 
												&player,
												&c_id);
						ai->m_foreigner[i]->CanSeeCity(ai, pos, c_id);
					}
			}
		}
	}

	
	maxz = m_size->z; 
	
	
	
	
    for (pos.z=0; pos.z<maxz; pos.z++) { 
        for (pos.x=0; pos.x<m_size->x; pos.x++) { 
			for (pos.y=0; pos.y<m_size->y; pos.y++) { 
				
				ai->m_world->GetCityAndArmyId( &pos, &f_id,
					&a_id,
					&top_unit_type, &unit_num, &c_id,
					&unitVis, &cityVis,
					&mineVis );
				
				if ( mineVis ) {
					RegisterMinedCell( pos );
				}

				
				if (unitVis && !cityVis) {                    
					if (ai->m_my_player_id != f_id) { 
						Assert(a_id != 0); 
						Assert(ai->m_foreigner[f_id]); 
						ai->m_foreigner[f_id]->CanSeeArmy(ai, pos, a_id, 
							top_unit_type, unit_num); 
					}
				} 
				
				if (cityVis) { 
					if (ai->m_my_player_id != f_id) { 
						ai->m_foreigner[f_id]->CanSeeCity(ai, pos, c_id);
						
					}
				}
			}
		}
	}

}

BOOL AiMap::HasPopFromThisCity(AiMain *ai, MapPointData &w, uint32 city_id) const

{
    uint32 u_id; 
    
    
    
    
    return FALSE; 
    
}

sint32 AiMap::ExploredArea(AiMain *ai)
{
	












	
	return ai->m_planner->the_strategic_map->rough_count_of_explored_cells;
}

void AiMap::RegisterStealthAttack(const MapPointData &pos)
{
	sint16 dist = 1; 
	MapPointData tmp;

	
	
	

	
	
	Wrap(pos.x+dist,pos.y, pos.z, tmp);
	m_stealthAttacks.InsertFlat(tmp);
	
	Wrap(pos.x+dist,pos.y+dist,pos.z,tmp);
	m_stealthAttacks.InsertFlat(tmp);
	
	Wrap(pos.x,pos.y+dist,pos.z,tmp);
	m_stealthAttacks.InsertFlat(tmp);
	
	Wrap(pos.x-dist,pos.y+dist, pos.z,tmp);
	m_stealthAttacks.InsertFlat(tmp);
	
	Wrap(pos.x-dist,pos.y, pos.z,tmp);
	m_stealthAttacks.InsertFlat(tmp);
	
	Wrap(pos.x-dist,pos.y-dist, pos.z,tmp);
	m_stealthAttacks.InsertFlat(tmp);
	
	Wrap(pos.x,pos.y-dist, pos.z,tmp);
	m_stealthAttacks.InsertFlat(tmp);
	
	Wrap(pos.x+dist, pos.y-dist, pos.z,tmp);
	m_stealthAttacks.InsertFlat(tmp);
}

void AiMap::ClearStealthAttacks() 
{
	m_stealthAttacks.Clear();
}


DynamicArray<MapPointData> & AiMap::GetStealthAttacks() 
{
	return m_stealthAttacks;
}

void AiMap::RegisterMinedCell(const MapPointData &pos)
{
	m_minedCells.InsertFlat(pos);
}

void AiMap::ClearMinedCells() 
{
	m_minedCells.Clear();
}

void AiMap::RemoveMinedCell(const MapPointData &pos)
{
	sint32 index = m_minedCells.Find(pos);
	
	if (index >= 0)
		m_minedCells.DelIndexFlat(index);
}


DynamicArray<MapPointData> & AiMap::GetMinedCells() 
{
	return m_minedCells;
}

void AiMap::RegisterNukeSafeCity(const MapPointData &pos)
{
	
	if (!IsNukeSafeCity(pos))
		m_nukeSafeCities.InsertFlat(pos);
}

BOOL AiMap::IsNukeSafeCity(const MapPointData &pos)
{
	return (m_nukeSafeCities.Find(pos) >= 0);
}


void AiMap::IncMineCellCount(const MapPointData &pos)
{ 
    sint32 x = pos.x / k_MINECELLX;
    sint32 y = pos.y / k_MINECELLY; 

    m_mine_count[x][y]++;
}

sint32 AiMap::GetMineCellCount(const MapPointData &pos)
{ 
    sint32 x = pos.x / k_MINECELLX;
    sint32 y = pos.y / k_MINECELLY; 

    return m_mine_count[x][y];
} 

void AiMap::IncFortCellCount(const MapPointData &pos)
{ 
    sint32 x = pos.x / k_MINECELLX;
    sint32 y = pos.y / k_MINECELLY; 

    m_fort_count[x][y]++;
}

sint32 AiMap::GetFortCellCount(const MapPointData &pos)
{ 
    sint32 x = pos.x / k_MINECELLX;
    sint32 y = pos.y / k_MINECELLY; 

    return m_fort_count[x][y];
} 

void AiMap::IncAirfieldCellCount(const MapPointData &pos)
{ 
    sint32 x = pos.x / k_MINECELLX;
    sint32 y = pos.y / k_MINECELLY; 

    m_airfield_count[x][y]++;
}

sint32 AiMap::GetAirfieldCellCount(const MapPointData &pos)
{ 
    sint32 x = pos.x / k_MINECELLX;
    sint32 y = pos.y / k_MINECELLY; 

    return m_airfield_count[x][y];
} 


void AiMap::IncGuardtowerCellCount(const MapPointData &pos)
{ 
    sint32 x = pos.x / k_MINECELLX;
    sint32 y = pos.y / k_MINECELLY; 

    m_guardtower_count[x][y]++;
}

sint32 AiMap::GetGuardtowerCellCount(const MapPointData &pos)
{ 
    sint32 x = pos.x / k_MINECELLX;
    sint32 y = pos.y / k_MINECELLY; 

    return m_guardtower_count[x][y];
} 
















void AiMap::GroundBuildSettleTree(AiMain *ai)
{ 
    m_settle_queue.Clear(); 
    AiCellPtr *add_me=NULL; 

    MapPointData pos; 
    BOOL land_only = !ai->m_science_agent->HasWaterSettle(); 
    MapPointData xy_pos; 

    pos.z = GROUND_Z; 
    sint32 insert_count=0; 
    for (pos.x=0; pos.x<m_size->x; pos.x++) { 
        for (pos.y=0; pos.y<m_size->y; pos.y++) { 

            
            if (!m_can_settle_ground->Get(pos.x, pos.y)) continue;

            if (land_only && ai->m_world->IsWater(&pos)) continue; 
               
            ai->m_player->Norm2XY(pos, xy_pos); 
            if (m_settler_xy_range < ai->m_planner->the_strategic_map->
                Get_Distance_To_Our_Empire(xy_pos)) continue; 

            
            insert_count++; 
            if ((SHRT_MAX-2) <= insert_count) break;
            

            add_me = new AiCellPtr; 
            add_me->x = sint16(pos.x); 
            add_me->y = sint16(pos.y); 

            m_settle_queue.Insert(add_me);
                     
        }
    }
}

void AiMap::SpaceBuildSettleTree(AiMain *ai)
{
    MapPointData pos; 
    AiCellPtr *add_me; 
    MapPointData xy_pos; 

    sint32 insert_count=0; 
    pos.z = SPACE_Z; 
    for (pos.x=0; pos.x<m_size->x; pos.x++) { 
        for (pos.y=0; pos.y<m_size->y; pos.y++) { 

            if (!m_can_settle_space->Get(pos.x,pos.y)) continue; 

            ai->m_player->Norm2XY(pos, xy_pos); 
            if (m_settler_xy_range < ai->m_planner->the_strategic_map->
                Get_Distance_To_Our_Empire(xy_pos)) continue; 

            
            insert_count++; 
            if ((SHRT_MAX-2) <= insert_count) break;

            

            add_me = new AiCellPtr; 

            add_me->x = sint16(pos.x); 
            add_me->y = sint16(pos.y); 
            m_settle_queue.Insert(add_me);
        }
    }

}










void AiMap::FlattenCanSettle(Bit_Table *can_settle, MapPointData &center, 
    sint16 radius)
{
    MapPointData p;
    MapPointData w; 
    p.z = center.z; 

    for (p.x=(center.x-radius); p.x<=(center.x+radius); p.x++) { 
        for (p.y=(center.y-radius); p.y<=(center.y+radius); p.y++) { 
            Wrap(p.x, p.y, p.z, w); 
            can_settle->Set(w.x, w.y, 0); 
        }
    }
}

void AiMap::UnflattenSettle(MapPointData &center, sint16 radius)
{
    MapPointData p;
    MapPointData w; 
    p.z = center.z; 

    if (center.z == GROUND_Z) { 
        for (p.x=(center.x-radius); p.x<=(center.x+radius); p.x++) { 
            for (p.y=(center.y-radius); p.y<=(center.y+radius); p.y++) { 
                Wrap(p.x, p.y, p.z, w); 
                m_can_settle_ground->Set(w.x, w.y, 1); 
            }
        }
    } else { 
        for (p.x=(center.x-radius); p.x<=(center.x+radius); p.x++) { 
            for (p.y=(center.y-radius); p.y<=(center.y+radius); p.y++) { 
                Wrap(p.x, p.y, p.z, w); 
                m_can_settle_space->Set(w.x, w.y, 1); 
            }
        }
    }
}


void AiMap::FlattenStaticSettle(MapPointData &center, sint16 radius)
{
    if (center.z == GROUND_Z) { 
        FlattenCanSettle(m_can_settle_ground, center, radius);
    } else { 
        FlattenCanSettle(m_can_settle_space, center, radius);
    }
}


void AiMap::InsertBestSettleGoal(AiMain *ai, ERROR_INSERT_SETTLE &err, 
    Bit_Table *can_settle, sint32 *land_cont_count, 
    sint32 &total_land_count, sint32 land_eval_num, 
    sint32 *water_cont_count,
    sint32 &total_water_count, sint32 water_eval_num, 
    sint16 z_value)
{ 

    AiCellPtr *biggest=NULL; 

    
    BOOL searching=TRUE; 
    while (searching && (1 < m_settle_queue.Len())) { 
        biggest = m_settle_queue.Remove(1); 
        if (can_settle->Get(biggest->x, biggest->y)) { 
            searching = FALSE; 
        } else { 
            delete biggest;
        }
    }

    if (searching) { 
        err = ERROR_INSERT_SETTLE_EMPTY; 
        return; 
    } 

    Assert(biggest); 

    MapPointData center; 
    center.x = biggest->x;
    center.y = biggest->y;
    center.z = z_value;
    
    BOOL make_goal = TRUE; 

    sint32 player_id; 
    uint32 a_id;
    uint32 c_id;
    sint32 top_unit_type;
    sint32 unit_num;
    BOOL unitVis;
    BOOL cityVis;
    BOOL mineVis;
	BOOL can_be_expelled;

    
    
    
    
    

	
	
	ai->m_world->GetCellContents( &center,
								  &player_id,
								  &a_id,
								  &top_unit_type,
								  &unit_num,
								  &c_id,
								  &unitVis,
								  &cityVis,
								  &mineVis, 
								  &can_be_expelled);
	
	
	if (c_id != 0 && player_id != ai->my_player_index)
		make_goal = FALSE;
					
	
	if (a_id != 0 && player_id != ai->my_player_index)
		make_goal = FALSE;

    if (z_value == GROUND_Z) { 
        sint32 dest_cont;
        BOOL dest_is_land; 
        if (!ai->m_continents->GetContinent(ai, center, dest_cont, dest_is_land)) { 
            return ;
        } 

        if (dest_is_land) { 
            Assert(0<=dest_cont); 
            Assert(dest_cont<ai->m_continents->GetNumLandCont()); 
            land_cont_count[dest_cont]++;
            if (ai->m_continents->GetLandArmies(dest_cont) < 1) { 
                if (1 < land_cont_count[dest_cont]) { 
                    make_goal = FALSE; 
                } 
            } 

            if (land_eval_num < total_land_count) { 
                make_goal = FALSE; 
            } 

            if (make_goal) { 
                total_land_count++; 
            } 

        } else {
            Assert(0<=dest_cont); 
            Assert(dest_cont<ai->m_continents->GetNumWaterCont()); 
            water_cont_count[dest_cont]++;
            if (ai->m_continents->GetWaterArmies(dest_cont) < 1) { 
                if (1 < water_cont_count[dest_cont]) { 
                    make_goal = FALSE; 
                } 
            } 

            if (water_eval_num < total_water_count) { 
                make_goal = FALSE; 
            } 

            if (make_goal) { 
                total_water_count++; 
            } 

        }
    }

     
    if (make_goal) { 

        sint16 settle_value = ai->m_world->GetSettleValue(center.x, center.y, center.z);

#ifdef DRAW_SETTLE_GOALS

        if (settle_value < 1) { 
            ai->m_world->SetColor(&center, 999);           
        } else { 
            ai->m_world->SetColor(&center, 666);           
        }
#endif
        if (0 < settle_value) { 

            GoalSettle *goal_obj=NULL;
            goal_obj = new GoalSettle(ai, settle_value, center); 
            ai->m_planner->Add_Goal(goal_obj);
            
            FlattenCanSettle(can_settle, center, 1);
        }
    }

    delete biggest;
    err = ERROR_INSERT_SETTLE_OK; 
}


Bit_Table *g_static_can_settle;
AiMain *g_settle_ai;
sint16 g_settle_z; 

void AiMap::InsertSettleGoals(AiMain *ai, const sint32 eval_num)
{
    sint32 foo; 

if (6 == ai->m_my_player_id) { 
    foo = 0 ; 
} 

    
    if ((0.99 < fz_no_settle) ||
        (ai->m_governmentDB->GetTooManyCitiesThreshold(
            ai->m_will_of_the_people->GetCurrentGov()) <= ai->m_city_set->GetNum())) {
            return; 
    }
                

#ifdef DRAW_SETTLE_GOALS
    ai->m_world->ClearColor();  
#endif

    *g_can_settle_ground = *m_can_settle_ground; 
    *g_can_settle_space = *m_can_settle_space; 


    sint32 total_land_count, land_eval_num, total_water_count, water_eval_num; 
    
    
    g_static_can_settle = m_can_settle_ground;
    g_settle_ai = ai;
    g_settle_z = GROUND_Z; 

    GroundBuildSettleTree(ai); 

    if ((m_settle_queue.Len() < 10) && (m_settler_xy_range<50)){ 
        m_settler_xy_range++; 
    } 
    
    sint32 i; 
    ERROR_INSERT_SETTLE err; 

    if (0 < m_settle_queue.Len()) { 

        sint32 cont_idx; 
        sint32 *land_cont_count;
        land_cont_count = new sint32[ai->m_continents->GetNumLandCont()];
        for (cont_idx=0; cont_idx<ai->m_continents->GetNumLandCont(); cont_idx++) { 
            land_cont_count[cont_idx] = 0; 
        }
        sint32 *water_cont_count;
        water_cont_count = new sint32[ai->m_continents->GetNumWaterCont()];
        for (cont_idx=0; cont_idx<ai->m_continents->GetNumWaterCont(); cont_idx++) { 
            water_cont_count[cont_idx] = 0; 
        }

        total_land_count=0; 
        total_water_count=0; 
        if (ai->m_science_agent->HasWaterSettle()) { 
            land_eval_num = sint32(eval_num * 0.5); 
            water_eval_num = sint32(eval_num * 0.25); 
        } else { 
            land_eval_num = eval_num; 
            water_eval_num = 0; 
        } 

        err = ERROR_INSERT_SETTLE_OK ;
        for (i=0; 
            (0 < m_settle_queue.Len()) && ((total_land_count < land_eval_num) ||
            (total_water_count < water_eval_num)) && (ERROR_INSERT_SETTLE_OK == err); 
                    i++) { 
            InsertBestSettleGoal(ai, err, g_can_settle_ground, 
                land_cont_count, total_land_count, land_eval_num,
                water_cont_count, total_water_count, water_eval_num, 
                GROUND_Z); 
        }

        delete land_cont_count; 
        land_cont_count = NULL; 
        delete water_cont_count; 
        water_cont_count = NULL; 
    }

    if ((total_land_count <  (land_eval_num*0.8)) &&
        (total_water_count < (water_eval_num*0.8))) { 
        m_settler_xy_range++; 
    }

    CleanSettleQueue();
    if (!ai->m_science_agent->HasSpaceSettle()) return; 

    
    
    g_settle_z = SPACE_Z; 
    err=ERROR_INSERT_SETTLE_OK; 
    SpaceBuildSettleTree(ai); 

    for (i=0; (0 < m_settle_queue.Len()) && (i < (eval_num *0.25)) && (ERROR_INSERT_SETTLE_OK == err); i++) { 
        InsertBestSettleGoal(ai, err, g_can_settle_space, 
            NULL, total_land_count, land_eval_num, 
            NULL, total_water_count, water_eval_num, 
            SPACE_Z); 
    }
    
    if (i < 10) { 
        m_settler_xy_range++; 
    } 

    
    CleanSettleQueue();
} 


void AiMap::DumpSettleScore(AiMain *ai)
{
return;
}

void AiMap::CleanSettleQueue()
{ 
    sint32 i; 
    AiCellPtr *ptr;

    for (i= m_settle_queue.Len(); 0 <i; i--) { 
        ptr = m_settle_queue.Remove(i); 
        delete ptr; 
    }
} 


