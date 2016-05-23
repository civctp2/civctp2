//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : World continent handling
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Added note about my continent error. Having terrain that was Air only caused it.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "World.h"

#include "bset.h"
#include "Cell.h"
#include "directions.h"
#include "dynarr.h"
#include "mapanalysis.h"
#include "MoveFlags.h"
#include "Unit.h"

#define INVALID_CONTINENT -2
#define SEARCHING_CONTINENT -1
#define WATER_CONTINENT_START 0
#define LAND_CONTINENT_START 16000

sint16 World::GetContinent(const MapPoint & pos) const
{
    return GetCell(pos)->GetContinent();
}

bool World::IsOnSameContinent(const MapPoint & pos1, const MapPoint & pos2) const
{
    return GetContinent(pos1) == GetContinent(pos2);
}

struct MapPointNode
{
    MapPointNode
    (
        MapPoint const &    a_Pos,
        MapPointNode *      a_Next = NULL
    )
    :
        pos         (a_Pos),
        next        (a_Next)
    { };

    MapPoint        pos;
    MapPointNode *  next;
};

struct Old_Cont_Node
{
    Old_Cont_Node
    (
        MapPoint const &     a_Pos,
        Old_Cont_Node *      a_Next      = NULL,
        sint32               a_Continent = INVALID_CONTINENT
    )
    :
        m_pos       (a_Pos),
        m_old_cont  (a_Continent),
        m_next      (a_Next)
    { };

    MapPoint        m_pos;
    sint32          m_old_cont;
    Old_Cont_Node * m_next;
};

Old_Cont_Node * g_tunnel_list   = NULL;

void World::NumberContinents()
{
    InitContinent();
    GrowOceans();
    ResetCanalsTunnels();
    GrowContinents();

    m_continents_are_numbered=TRUE;

    while (g_tunnel_list)
    {
        Old_Cont_Node * del_me = g_tunnel_list;
        g_tunnel_list = g_tunnel_list->m_next;
        GetCell(del_me->m_pos)->SetContinent((sint16)del_me->m_old_cont);
        delete del_me;
    }

    FindContinentNeighbors();
    FindContinentSize();
    MapAnalysis::GetMapAnalysis().ResizeContinents();
}

sint16 World::GetMinWaterContinent() const { return WATER_CONTINENT_START; }
sint16 World::GetMaxWaterContinent() const { return m_water_continent_max; }
sint16 World::GetMinLandContinent() const { return LAND_CONTINENT_START; }
sint16 World::GetMaxLandContinent() const { return m_land_continent_max; }

void World::InitContinent()
{
    m_water_continent_max = WATER_CONTINENT_START;
    m_land_continent_max = LAND_CONTINENT_START;

    MapPoint p;
    for (p.x = 0; p.x <m_size.x; p.x++) {
        for (p.y = 0; p.y <m_size.y; p.y++) {
            GetCell(p)->SetContinent(INVALID_CONTINENT);
        }
    }
}

void World::GrowOceans()
{
    MapPoint p;

    for (p.x = 0; p.x <m_size.x; p.x++) {
        for (p.y = 0; p.y <m_size.y; p.y++) {

            if (IsNewWater(p)) {
                GrowWater(p);
            }
        }
    }
}

void World::GrowWater(MapPoint const & start)
{
    MapPointNode * search_list   = NULL;
    AddToWaterSearch(search_list, start);

    MapPoint       center;
    MapPointNode * finished_list = NULL;
#ifdef _DEBUG
    sint32         finite_loop   = 0;
#endif
    MapPoint       test_pos;

    while (NextPoint(search_list, finished_list, center))
    {
		Assert(++finite_loop < 100000);

		if(center.GetNeighborPosition(NORTH, test_pos))
			AddToWaterSearch(search_list, test_pos);
		if(center.GetNeighborPosition(NORTHEAST, test_pos))
			AddToWaterSearch(search_list, test_pos);
		if(center.GetNeighborPosition(NORTHWEST, test_pos))
			AddToWaterSearch(search_list, test_pos);

		if(center.GetNeighborPosition(EAST, test_pos))
			AddToWaterSearch(search_list, test_pos);
		if(center.GetNeighborPosition(WEST, test_pos))
			AddToWaterSearch(search_list, test_pos);

		if(center.GetNeighborPosition(SOUTH, test_pos))
			AddToWaterSearch(search_list, test_pos);
		if(center.GetNeighborPosition(SOUTHEAST, test_pos))
			AddToWaterSearch(search_list, test_pos);
		if(center.GetNeighborPosition(SOUTHWEST, test_pos))
			AddToWaterSearch(search_list, test_pos);
    }

#ifdef _DEBUG
    finite_loop=0;
#endif

    while (finished_list)
    {
        Assert(++finite_loop < 100000);

        MapPointNode * ptr = finished_list;
        finished_list = finished_list->next;
        GetCell(ptr->pos)->SetContinent(m_water_continent_max);
        delete ptr;
    }

    m_water_continent_max++;
    Assert(WATER_CONTINENT_START < m_water_continent_max);
    Assert(m_water_continent_max < LAND_CONTINENT_START);
}

void World::AddToWaterSearch
(
    MapPointNode *&  search_list,
    MapPoint const & pos
)
{
    if (IsNewWater(pos))
    {
        search_list = new MapPointNode(pos, search_list);
        GetCell(pos)->SetContinent(SEARCHING_CONTINENT);
    }
}

bool World::IsNewWater(MapPoint const & p) const
{
    Cell *  c = GetCell(p);
    sint32  v = c->GetContinent();

    if (INVALID_CONTINENT == v)
    {
        uint32 e = GetCell(p)->GetEnv();
        return (e & k_BIT_MOVEMENT_TYPE_WATER)
            || (e & k_BIT_MOVEMENT_TYPE_SHALLOW_WATER)
            || (c->GetCity().m_id != 0);
    }

    return false;
}

bool World::NextPoint
(
    MapPointNode *& search_list,
    MapPointNode *& finished_list,
    MapPoint &      p
)
{
    if (search_list == NULL)
    {
        return false;
    }
    else
    {
        MapPointNode *  ptr = search_list;
        search_list     = search_list->next;

        ptr->next       = finished_list;
        finished_list   = ptr;
        p               = ptr->pos;
        return true;
    }
}


void World::ResetCanalsTunnels()
{
    while (g_tunnel_list)
    {
        Old_Cont_Node * oldFirst    = g_tunnel_list;
        g_tunnel_list               = g_tunnel_list->m_next;
        delete oldFirst;
    }

    MapPoint pos;

    for (pos.x=0; pos.x<m_size.x; pos.x++) {
       for (pos.y=0; pos.y<m_size.y; pos.y++) {

           Cell * c = GetCell(pos);
           uint32 e = c->GetEnv();

           if ((e & k_BIT_MOVEMENT_TYPE_WATER) || (e & k_BIT_MOVEMENT_TYPE_SHALLOW_WATER))
           {
               if (e & k_MASK_ENV_CANAL_TUNNEL)
               {

                   sint32  old_cont_val = c->GetContinent();
                   if ((0 <= old_cont_val) && IsWater(pos))
                   {
                       g_tunnel_list = new Old_Cont_Node(pos, g_tunnel_list, old_cont_val);
                   }

                   c->SetContinent(INVALID_CONTINENT);
               }
           }
           else if ((e & k_BIT_MOVEMENT_TYPE_LAND) || (e & k_BIT_MOVEMENT_TYPE_MOUNTAIN))
           {
               c->SetContinent(INVALID_CONTINENT);
           }
       }
    }
}





void World::GrowContinents()
{
    MapPoint p;

    for (p.x = 0; p.x <m_size.x; p.x++) {
        for (p.y = 0; p.y <m_size.y; p.y++) {
            if (IsNewLand(p)) {
                GrowLand(p);
            }
        }
    }
}

bool World::IsNewLand(MapPoint const &p) const
{
    if (INVALID_CONTINENT == GetCell(p)->GetContinent())
    {
        uint32 e = GetCell(p)->GetEnv();

        return (e & k_BIT_MOVEMENT_TYPE_LAND)
            || (e & k_BIT_MOVEMENT_TYPE_MOUNTAIN)
            || (e & k_MASK_ENV_CANAL_TUNNEL);
    }

    return false;
}


void World::GrowLand(MapPoint const & start)
{
    MapPointNode * search_list   = NULL;
    AddToLandSearch(search_list, start);

    MapPoint       test_pos;
    MapPoint       center;
    MapPointNode * finished_list = NULL;
#ifdef _DEBUG
    sint32         finite_loop   = 0;
#endif

    while (NextPoint(search_list, finished_list, center))
    {
		Assert(++finite_loop < 100000);

		if(center.GetNeighborPosition(NORTH, test_pos))
			AddToLandSearch(search_list, test_pos);
		if(center.GetNeighborPosition(NORTHEAST, test_pos))
			AddToLandSearch(search_list, test_pos);
		if(center.GetNeighborPosition(NORTHWEST, test_pos))
			AddToLandSearch(search_list, test_pos);

		if(center.GetNeighborPosition(EAST, test_pos))
			AddToLandSearch(search_list, test_pos);
		if(center.GetNeighborPosition(WEST, test_pos))
			AddToLandSearch(search_list, test_pos);

		if(center.GetNeighborPosition(SOUTH, test_pos))
			AddToLandSearch(search_list, test_pos);
		if(center.GetNeighborPosition(SOUTHEAST, test_pos))
			AddToLandSearch(search_list, test_pos);
		if(center.GetNeighborPosition(SOUTHWEST, test_pos))
			AddToLandSearch(search_list, test_pos);
    }

#ifdef _DEBUG
    finite_loop=0;
#endif

    while (finished_list)
    {
        Assert(++finite_loop < 100000);

        MapPointNode * ptr = finished_list;
        finished_list = finished_list->next;

        GetCell(ptr->pos)->SetContinent(m_land_continent_max);
        delete ptr;
    }

    m_land_continent_max++;
    Assert(LAND_CONTINENT_START < m_land_continent_max);
}

void World::AddToLandSearch
(
    MapPointNode *&  search_list,
    MapPoint const & pos
)
{
    if (IsNewLand(pos))
    {
        search_list = new MapPointNode(pos, search_list);
        GetCell(pos)->SetContinent(SEARCHING_CONTINENT);
    }
}


void World::FindContinentSize()
{
    m_water_size->ExtendNFlat(m_water_continent_max);

    sint32 i;
    for (i=0; i<m_water_continent_max; i++) {
        m_water_size->Access(i) = 0;
    }

    sint32 diff =  m_land_continent_max - LAND_CONTINENT_START;

    m_land_size->ExtendNFlat(diff);
    for (i=0; i<diff; i++) {
        m_land_size->Access(i) = 0;
    }

    bool is_land;
    sint16 cont;
    MapPoint pos;
    for (pos.x = 0; pos.x<m_size.x; pos.x++) {
        for (pos.y=0; pos.y<m_size.y; pos.y++) {
            GetContinent(pos, cont, is_land);
            if (is_land) {
                m_land_size->Access(cont)++;
            } else {
                m_water_size->Access(cont)++;
            }
        }
    }
}

sint32 World::GetLandContinentSize(sint32 cont_num) const
{
    return m_land_size->Access(cont_num);
}

sint32 World::GetWaterContinentSize(sint32 cont_num) const
{
    return m_water_size->Access(cont_num);
}




void World::GetContinent(const MapPoint &pos, sint16 &cont_number, bool &is_land) const
{
	// E note: I got this error when I made Muntains air only.
	// So this method only checks land and water
	static bool REPORTED_MAP_CONTINENT_NUMBERING_INCORRECT  = false; // Made static so it shows only once
	is_land = !IsWater(pos);

	cont_number = GetCell(pos)->m_continent_number;
	if (is_land)
	{
		cont_number -= LAND_CONTINENT_START;
	}

	if (    (cont_number < 0)
	     || (!is_land && cont_number >= LAND_CONTINENT_START)
	   )
	{
		Assert(REPORTED_MAP_CONTINENT_NUMBERING_INCORRECT);
		REPORTED_MAP_CONTINENT_NUMBERING_INCORRECT = true;
		is_land = false;
		cont_number = 0;
	}
}

void World::AllocateNeighborMem()
{
    sint32 n = m_water_next_too_land->Num();
    sint32 i;

    for (i=0; i<n; i++) {
        m_water_next_too_land->Access(i).Clear();
    }

    for (i=n; i<m_water_continent_max; i++) {
        m_water_next_too_land->ExtendByOne();
    }

    sint32 diff = m_land_continent_max - LAND_CONTINENT_START;
    n = m_land_next_too_water->Num();

    for (i=0; i<n; i++) {
        m_land_next_too_water->Access(i).Clear();
    }

    for (i=n; i<diff; i++) {
        m_land_next_too_water->ExtendByOne();
    }
}

void World::FindContinentNeighbors()
{
    sint16 center_cont;
    MapPoint center;
    MapPoint test_cont;
    bool is_land = false;

    AllocateNeighborMem();

    for(center.x=0; center.x<m_size.x; center.x++) {
        for(center.y=0; center.y<m_size.y; center.y++) {

            GetContinent(center, center_cont, is_land);

            Assert(0 <= center_cont);
            if (center_cont < 0) continue;

            if(center.GetNeighborPosition(NORTHWEST, test_cont))
				FindAContinentNeighbor(center_cont, test_cont, is_land);

            if(center.GetNeighborPosition(WEST, test_cont))
				FindAContinentNeighbor(center_cont, test_cont, is_land);

            if(center.GetNeighborPosition(SOUTHWEST, test_cont))
				FindAContinentNeighbor(center_cont, test_cont, is_land);

            if(center.GetNeighborPosition(SOUTH, test_cont))
				FindAContinentNeighbor(center_cont, test_cont, is_land);
        }
    }
}

void World::FindAContinentNeighbor
(
    sint32     center_cont,
    MapPoint & test_point,
    bool       is_land
)
{
    bool is_neighbor_land = false;
    sint16 test_cont;

    GetContinent(test_point, test_cont, is_neighbor_land);

    if (is_land && !is_neighbor_land) {
        InsertLandNextToWater(center_cont, test_cont);
        InsertWaterNextToLand(test_cont, center_cont);
    } else if (!is_land && is_neighbor_land) {
        InsertWaterNextToLand(center_cont, test_cont);
        InsertLandNextToWater(test_cont, center_cont);
    }
}

void World::InsertWaterNextToLand(const sint32 waterc, const sint32 landc)
{
    if ((waterc < 0) || (m_water_next_too_land->Num() <= waterc)) {
        sint32 water_cont_out_of_bounds=0;
        Assert(water_cont_out_of_bounds);
        return;
    }

    DynamicArray<sint32> * next = &(m_water_next_too_land->Access(waterc));
    sint32 num_land = next->Num();

    for (sint32 idx_next_too = 0; idx_next_too < num_land; idx_next_too++) {
        if (next->Get(idx_next_too) == landc) {
            return;
        }
    }

   next->InsertFlat(landc);

}

void World::InsertLandNextToWater(const sint32 landc, const sint32 waterc)
{
    if ((landc < 0) || (m_land_next_too_water->Num()  <= landc)) {
        sint32 land_cont_out_of_bounds=0;
        Assert(land_cont_out_of_bounds);
        return;
    }

    DynamicArray<sint32> * next = &(m_land_next_too_water->Access(landc));
    sint32 num_water = next->Num();

    for (sint32 idx_next_too=0; idx_next_too<num_water; idx_next_too++) {
        if (next->Get(idx_next_too) == waterc) {
            return;
        }
    }

   next->InsertFlat(waterc);

}

bool World::IsWaterNextTooLand(const sint32 waterc, const sint32 landc) const
{
    DynamicArray<sint32> * next = &(m_water_next_too_land->Access(waterc));
    sint32 num_water = next->Num();

    for (sint32 idx_next_too=0; idx_next_too<num_water; idx_next_too++) {
        if (next->Get(idx_next_too) == landc) {
            return true;
        }
    }

    return false;
}


bool World::IsLandNextTooWater(const sint32 landc, const sint32 waterc) const
{
    DynamicArray<sint32> * next = &(m_land_next_too_water->Access(landc));
    sint32 num_land = next->Num();

    for (sint32 idx_next_too=0; idx_next_too<num_land; idx_next_too++) {
        if (next->Get(idx_next_too) == waterc) {
            return true;
        }
    }

    return false;
}




bool World::GetIsChokePoint(MapPoint const & pos) const
{
    return GetCell(pos)->GetIsChokePoint();
}

void World::CalcChokePoints()
{
    sint16 ** water_map = new sint16*[m_size.x];
    sint32 x;
    for (x=0; x<m_size.x; x++) {
        water_map[x] = new sint16[m_size.y];
    }

    sint16 ** land_map = new sint16*[m_size.x];
    for (x=0; x<m_size.x; x++) {
        land_map[x] = new sint16[m_size.y];
    }

    Grassfire8(FALSE, water_map);
    ClipGF(water_map);

    Grassfire8(TRUE, land_map);
    ClipGF(land_map);

    SaveGF(water_map, land_map);

    for (x=0; x<m_size.x; x++) {
        delete[] land_map[x];
        delete[] water_map[x];
    }
    delete[] land_map;
    delete[] water_map;
}

bool World::IsGFComputed(bool is_choke_land, MapPoint const & pos) const
{
    return is_choke_land == (IsLand (pos) || IsMountain (pos));
}

void World::Grassfire8(bool is_choke_land, sint16 **tmp_map)
{
    MapPoint pos;
    for (pos.x=0; pos.x < m_size.x; pos.x++) {
        for (pos.y=0; pos.y < m_size.y; pos.y++) {
            if (IsGFComputed(is_choke_land, pos)) {
                tmp_map[pos.x][pos.y] = 9999;
            } else {
                tmp_map[pos.x][pos.y] =0;
            }
        }
    }

    sint16 v, mv = 0; // Reconsider!
    MapPoint w;
    for (pos.x=0; pos.x < m_size.x; pos.x++) {
        for (pos.y=0; pos.y < m_size.y; pos.y++) {

            if (IsGFComputed(is_choke_land, pos)) {

                if(pos.GetNeighborPosition(NORTHWEST, w)) {
					v = tmp_map[w.x][w.y];
					if (v < mv) mv = v;
				}

                if(pos.GetNeighborPosition(WEST, w)) {
					v = tmp_map[w.x][w.y];
					mv = v;
				}

                if(pos.GetNeighborPosition(SOUTHWEST, w)) {
					v = tmp_map[w.x][w.y];
					if (v < mv) mv = v;
				}

                if(pos.GetNeighborPosition(SOUTH, w)) {
					v = tmp_map[w.x][w.y];
					if (v < mv) mv = v;
				}

                mv++;
                tmp_map[pos.x][pos.y] = mv;
            } else {
                tmp_map[pos.x][pos.y] = 0;
            }
        }
    }

    for (pos.x=(m_size.x-1); 0<=pos.x; pos.x--) {
        for (pos.y=(m_size.y-1); 0<=pos.y; pos.y--) {

            if (IsGFComputed(is_choke_land, pos)) {

                if(pos.GetNeighborPosition(SOUTHEAST, w)) {
					v = tmp_map[w.x][w.y];
					mv = v;
				}

                if(pos.GetNeighborPosition(EAST, w)) {
					v = tmp_map[w.x][w.y];
					if (v < mv) mv = v;
				}

                if(pos.GetNeighborPosition(NORTHEAST, w)) {
					v = tmp_map[w.x][w.y];
					if (v < mv) mv = v;
				}

                if(pos.GetNeighborPosition(NORTH, w)) {
					v = tmp_map[w.x][w.y];
					if (v < mv) mv = v;
				}

                v = tmp_map[pos.x][pos.y];
                if (mv < v) {
                    mv++;
                    tmp_map[pos.x][pos.y] = mv;
                }
            } else {
                tmp_map[pos.x][pos.y] = 0;
            }
        }
    }
}

void World::ClipGF(sint16 **tmp_map)
{
    MapPoint pos;
    MapPoint w;

    sint16 v;
    sint16 threshold_min = 1;
    sint16 threshold_max = 1;
    sint32 count = 0;

     for (pos.x=0; pos.x<m_size.x; pos.x++) {
        for (pos.y=0; pos.y<m_size.y; pos.y++) {

            sint16 curr = tmp_map[pos.x][pos.y];

            if ((1 == curr)) {

                count=0;
                if(pos.GetNeighborPosition(NORTHEAST, w)) {
					v = sint16(abs(tmp_map[w.x][w.y]));
					if (0 == v) count++;
				}

                if(pos.GetNeighborPosition(NORTHWEST, w)) {
					v = sint16(abs(tmp_map[w.x][w.y]));
					if (0 == v) count++;
				}

                if(pos.GetNeighborPosition(SOUTHWEST, w)) {
					v = sint16(abs(tmp_map[w.x][w.y]));
					if (0 == v) count++;
				}

                if(pos.GetNeighborPosition(SOUTHEAST, w)) {
					v = sint16(abs(tmp_map[w.x][w.y]));
					if (0 == v) count++;
				}

                if (3 == count) {
                    curr = -curr;
                    tmp_map[pos.x][pos.y] = curr;
                }
            }
        }
    }

     for (pos.x=0; pos.x<m_size.x; pos.x++) {
        for (pos.y=0; pos.y<m_size.y; pos.y++) {
            if (tmp_map[pos.x][pos.y] < 0) {
               tmp_map[pos.x][pos.y] = 0;
            }
        }
     }

    bool going_up = false;
    for (pos.x=0; pos.x<m_size.x; pos.x++) {
        for (pos.y=0; pos.y<m_size.y; pos.y++) {

            sint16 curr = tmp_map[pos.x][pos.y];
            if ((threshold_min <= curr) && (curr <= threshold_max)) {

                if(pos.GetNeighborPosition(SOUTHEAST, w)) {
					count = 0;
					v = tmp_map[w.x][w.y];
					going_up = v < threshold_min;
				}

                if(pos.GetNeighborPosition(EAST, w)) {
					v = sint16(abs(tmp_map[w.x][w.y]));
					if (going_up) {
						if (threshold_max <= v) {
							going_up = false;
							count++;
						}
					} else {
						if (v < threshold_min) {
							going_up = true;
							count++;
						}
					}
				}

                if(pos.GetNeighborPosition(NORTHEAST, w)) {
					v = sint16(abs(tmp_map[w.x][w.y]));
					if (going_up) {
						if (threshold_max <= v) {
							going_up = false;
							count++;
						}
					} else {
						if (v < threshold_min) {
							going_up = true;
							count++;
						}
					}
				}

                if(pos.GetNeighborPosition(NORTH, w)) {
					v = sint16(abs(tmp_map[w.x][w.y]));
					if (going_up) {
						if (threshold_max <= v) {
							going_up = false;
							count++;
						}
					} else {
						if (v < threshold_min) {
							going_up = true;
							count++;
						}
					}
				}

                if(pos.GetNeighborPosition(NORTHWEST, w)) {
					v = sint16(abs(tmp_map[w.x][w.y]));
					if (going_up) {
						if (threshold_max <= v) {
							going_up = false;
							count++;
						}
					} else {
						if (v < threshold_min) {
							going_up = true;
							count++;
						}
					}
				}

                if(pos.GetNeighborPosition(WEST, w)) {
					v = sint16(abs(tmp_map[w.x][w.y]));
					if (going_up) {
						if (threshold_max <= v) {
							going_up = false;
							count++;
						}
					} else {
						if (v < threshold_min) {
							going_up = true;
							count++;
						}
					}
				}

                if(pos.GetNeighborPosition(SOUTHWEST, w)) {
					v = sint16(abs(tmp_map[w.x][w.y]));
					if (going_up) {
						if (threshold_max <= v) {
							going_up = false;
							count++;
						}
					} else {
						if (v < threshold_min) {
							going_up = true;
							count++;
						}
					}
				}

                if(pos.GetNeighborPosition(SOUTH, w)) {
					v = sint16(abs(tmp_map[w.x][w.y]));
					if (going_up) {
						if (threshold_max <= v) {
							going_up = false;
							count++;
						}
					} else {
						if (v < threshold_min) {
							going_up = true;
							count++;
						}
					}
				}

                if (count < 3) {
                    curr = -curr;
                    tmp_map[pos.x][pos.y] = curr;
                }
            } else {
                curr = -curr;
                tmp_map[pos.x][pos.y] = curr;
            }
        }
    }
}

void World::DumpGF()
{
    MapPoint pos;
    for (pos.x=0; pos.x<m_size.x; pos.x++) {
        for (pos.y=0; pos.y<m_size.y; pos.y++) {
            sint32 gf = GetCell(pos)->GF();
            if (1 == gf)
            {
                SetColor(pos, gf);
            }
        }
    }
}

void World::SaveGF(sint16 **water_map, sint16 **land_map)
{
    for (sint32 x=0; x<m_size.x; x++) {
        for (sint32 y=0; y<m_size.y; y++) {
            if ((water_map[x][y] == 1) || (land_map[x][y] == 1)) {
                AccessCell(x, y)->m_gf = 1;
            } else {
                AccessCell(x, y)->m_gf = 0;
            }
        }
    }
}

bool World::LandShareWater(const sint32 land1, const sint32 land2) const
{
    for
    (
        sint32 water_idx = GetMinWaterContinent();
        water_idx < GetMaxWaterContinent();
        ++water_idx
    )
    {
        if (IsLandNextTooWater(land1, water_idx) &&
            IsLandNextTooWater(land2, water_idx)
           )
        {
            return true;
        }
    }

    return false;
}
