


#include "c3.h"
#include "globals.h"
#include "MoveFlags.h"
#include "CivArchive.h"
#include "IC3InstDB.h"
#include "IC3Rand.h"


#include "BSet.h"
#include "MapPoint.h"


#include "IC3GameState.h"
#include "IC3Player.h"
#include "IC3World.h"
#include "IC3RobotAstar.h"

#include "AiMain.h"
#include "AiCell.h"
#include "AiMap.h"
#include "RouteUtilityList.h"


#include "ArmyAgent.h"
#include "CityAgent.h"
#include "InstBidList.h"



#include "FSAlloc.h"


#include "IC3GameState.h"
#include "IC3TerrDB.h"
#include "AiMain.h"


#include "ArmyGoal.h"
#include "GoalDefense.h"
#include "GlExpl.h"
#include "GlAttck.h"
#include "GlSettl.h"
#include "GlRoute.h"
#include "GlFreight.h"



#include "common.h"
#include "linked_list.h"
#include "sorted_array.h"



#include "Unitflow.h"
#include "AIP.h"
#include "Squad.h"
#include "Goal.h"

#include "Scheduler.h"
#include "Planner.h"


#include "AllocWGF.h"
#include "CityGrowth.h"
#include "CityProd.h"
#include "CityFP.h"


#include "FzOut.h" 
#include "GoalInst.h"

#include "CityTree.h"
#include "cont.h"
#include "GoalRoad.h"
#include "CityEdge.h"


#include "FlatPtr.h"
#include "ScienceAgent.h"



#include "dr_debug.h"
#include "matrix.h"
#include "Grassfire.h"
#include "strategic_map.h"
#include "AiRndCnt.h"
#include "Pirate_Marker.h"

#include "WallClock.h"
extern Wall_Clock *g_wall_clock;


extern double fz_production_tax; 

extern double fz_city_growth_ratio;
extern double fz_city_growth_min_percent;


extern double fz_city_prod_ratio;
extern double fz_city_prod_min_percent;

extern double fz_city_fp_ratio;
extern double fz_city_fp_min_percent;

extern double fz_num_cities; 

extern double fz_wgf_stupid; 
extern double fz_ave_turns_new_pop; 
extern double fz_min_turns_new_pop; 

extern double fz_inst_threat_threshold; 

#include "OA_Repair_Installation.h"
extern double fz_max_num_dirty_tiles;



FSCompeteForInstallations::FSCompeteForInstallations()
{ 
    return;
} 

FSCompeteForInstallations::~FSCompeteForInstallations()
{ 
    return;
} 

void FSCompeteForInstallations::Serialize (CivArchive &archive)
{
    CHECKSERIALIZE
}


BOOL FSSetTaxes::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{
    ai->m_player->SetMaterialsTax(fz_production_tax); 
    
    ai->m_alloc_WGF->SetRates(ai); 


    return TRUE; 
}

void FSSetTaxes::Serialize (CivArchive &archive)
{
    CHECKSERIALIZE

    return; 
}



FSConnectTradeRoutes::FSConnectTradeRoutes()
{ 
    return; 
} 

BOOL FSConnectTradeRoutes::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{
    BOOL cross_talk = FALSE; 
    BOOL can_do; 
    sint32 idx_freight; 
    
    can_do = ai->m_science_agent->GetBestFreight(idx_freight);

	
	
	can_do &= !ai->GetCaravansReserved();

	
	ai->SetCaravansReserved(FALSE);

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif
 

    if (can_do) { 
        do { 
    Assert(++finite_loop < 100); 
            FindUnconnectedGoods(ai); 
            CollectCityBids(ai);
            ConnectGoods(ai, cross_talk); 

        } while (cross_talk); 
    }


    return can_do; 
}


void FSConnectTradeRoutes::Serialize (CivArchive &archive)
{
    CHECKSERIALIZE

    return; 
}

void FSConnectTradeRoutes::FindUnconnectedGoods(AiMain *ai)
{
    BSetID id; 
    CityAgent *ca=NULL;

    GoalRoute *goal_obj=NULL; 

    
    ai->m_route_utility_list->Clear(); 


    
    
    sint32 in_num;
    sint32 in_max; 
    sint32 out_num; 
    sint32 out_max;
    BOOL is_unknown_id; 
    sint32 good_type; 
    sint32 local_good_count; 
    sint32 total_good_count; 
    sint32 good_num = ai->m_terrainDB->GetNumTypeGoods();

    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) {            
        
        ai->m_player->GetCityNumTradeRoutes (ai->m_my_player_id, id.GetVal(), &is_unknown_id, 
            &in_num, &in_max, &out_num, &out_max);

       if (out_num >= out_max) continue; 




       for (good_type=0; good_type<good_num; good_type++) {
           
           ai->m_player->GetGoodCount(&is_unknown_id, ca->m_id.GetVal(), 
               good_type, &local_good_count, &total_good_count); 

           if ((local_good_count > 0) && (local_good_count == total_good_count)) { 
                 goal_obj = new GoalRoute(ca, good_type); 
				 ai->m_route_utility_list->InsertGoal(goal_obj); 					   
           } 
       } 
    } 
} 

void FSConnectTradeRoutes::CollectCityBids(AiMain *ai)
{ 
    GoalRoute *goal_obj=NULL;
    CityAgent *ca=NULL, *dest_city=NULL;
    BSetID id; 
    double utility, max_utility, cost, min_cost; 
    sint32 n;
    sint32 i; 
    sint32 in_num;
    sint32 in_max; 
    sint32 out_num; 
    sint32 out_max;
    BOOL is_unknown_id; 
    sint32 max_good_type = -1; 
    sint32 good_type; 
    Pirate_Marker *pirate_ptr=NULL; 
    uint32 src_city_id; 

    
    n = ai->m_route_utility_list->GetNumGoals(); 
    for (i=0; i<n; i++) { 
        goal_obj = ai->m_route_utility_list->GetGoal(i); 
        dest_city = NULL; 
        max_utility = 1.0; 
        min_cost = 10000000.0;

        good_type = goal_obj->GetGoodType();
        if (max_good_type < good_type) {
            max_good_type = good_type; 
        } 

        src_city_id = goal_obj->GetCityAgent()->m_id.GetVal(); 

        
        for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) {            

            if (src_city_id == ca->m_id.GetVal()) continue; 

            ai->m_player->GetCityNumTradeRoutes (ai->m_my_player_id, id.GetVal(), &is_unknown_id, 
                &in_num, &in_max, &out_num, &out_max);

            Assert(is_unknown_id == FALSE); 

            if (in_num >= in_max) continue; 

            pirate_ptr = ca->FindPiracyMarker(ai->m_my_player_id, src_city_id);

            if (pirate_ptr) { 
                if (!pirate_ptr->IsSafe(ai->m_round_count->GetRound())) continue; 
            } 


            goal_obj->EstimateRouteBenifit(ai, ca, utility, cost);
            if (max_utility < utility) {
                max_utility = utility; 
                min_cost = cost; 
                dest_city = ca; 
            } else if ((max_utility == utility) && (cost < min_cost)) { 
                max_utility = utility; 
                min_cost = cost; 
                dest_city = ca; 
            }
        }

        if (dest_city) { 
            goal_obj->SetDest(dest_city, max_utility, min_cost);
        }
    }

    
    if (0 < max_good_type) { 

        
        max_good_type++; 
        sint32 *table = new sint32[max_good_type]; 
        for (i=0; i<max_good_type; i++) { 
            table[i] = 0; 
        } 

        
        for(i=0; i<n; i++) { 
            goal_obj = ai->m_route_utility_list->GetGoal(i); 
            if (1.0 <= goal_obj->GetUtility()) { 
                table[goal_obj->GetGoodType()]++; 
            }
        } 

        
        for(i=0; i<n; i++) { 
            goal_obj = ai->m_route_utility_list->GetGoal(i); 
            if (1.0 <= goal_obj->GetUtility()) { 

                goal_obj->AddToUtility(table[goal_obj->GetGoodType()] * 20.0 + 
                    goal_obj->GetDestAgent()->GetNumCitizens()); 
            }
        } 

         
         delete[] table; 

    } 
} 

void FSConnectTradeRoutes::ConnectGoods(AiMain *ai, BOOL &cross_talk)
{
    GoalRoute *goal_obj=NULL, *max_goal=NULL;
    sint32 n, i; 
    sint32 freight; 
    double utility, max_utility; 
    BOOL is_a_there_a_new_route =FALSE; 

    cross_talk = FALSE; 
    n = ai->m_route_utility_list->GetNumGoals(); 

    freight = ai->m_player->GetUnusedFreight();
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    
    while (0 < freight) {    

Assert(++finite_loop < 100000); 
        max_goal = NULL; 
        max_utility = 0.0;


        
        for (i=0; i<n; i++) { 
            goal_obj = ai->m_route_utility_list->GetGoal(i); 
            if (goal_obj->IsSatisfied() == FALSE) { 
                utility = goal_obj->GetUtility(); 
            } else { 
                utility = 0.0; 
            } 
            if (max_utility < utility) { 
                max_utility = utility;
                max_goal = goal_obj;
            } 
        }

        if (max_goal) { 
            max_goal->ConnectRoute(ai, freight);
            max_goal->SetIsSatisfied(TRUE); 
            is_a_there_a_new_route = TRUE;
        } else { 
            break; 
        } 
    
        sint32 con_gt = max_goal->GetGoodType(); 



        
        
        if (is_a_there_a_new_route && (0 < freight)) { 
            for (i=0; i<n; i++) { 

                if ((con_gt == ai->m_route_utility_list->GetGoal(i)->GetGoodType()) &&   
                     (ai->m_route_utility_list->GetGoal(i)->IsSatisfied() == FALSE)) { 
                    cross_talk = TRUE;                                              
                    return;
                } 

            }
        }

    }
}
 


BOOL FSCompeteForInstallations::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{ 
    
    sint32 median; 
    ai->m_city_prod->EstimateProduction(ai);

    sint32 num_city = ai->m_city_prod->GetNumCities(); 

    if (0 < num_city) { 
        ai->m_city_prod->HeapSort(); 
        ai->m_city_prod->FindMedian(median, fz_city_prod_ratio, fz_city_prod_min_percent); 

        ai->m_city_fp->EstimateFP(ai);
        ai->m_city_fp->HeapSort(); 
        ai->m_city_fp->FindMedian(median, fz_city_fp_ratio, fz_city_fp_min_percent); 

        ai->m_city_growth->EstimateGrowthTime(ai);
        ai->m_city_growth->StopUnhappyGrowth(ai);
        ai->m_city_growth->HeapSort(); 
        ai->m_city_growth->FindMedianOnlyHappy(median, fz_city_growth_ratio, fz_city_growth_min_percent); 

        ai->m_city_growth->CalcAverage(); 
        fz_ave_turns_new_pop = ai->m_city_growth->GetAverage(); 
        fz_min_turns_new_pop = ai->m_city_growth->GetMin(); 
    } else { 

        
        ai->m_city_prod->Clear(); 
        ai->m_city_fp->Clear();
        ai->m_city_growth->Clear();

        fz_ave_turns_new_pop = 1000.0; 
        fz_min_turns_new_pop = 1000.0; 
    } 

    MakeCostBenefitList(ai); 
    ai->m_inst_bid_list->CalcDesiredPW(ai);
    SpendProduction(ai); 


    return TRUE; 
}

void FSCompeteForInstallations::MakeCostBenefitList(AiMain *ai)
{
    
    CityAgent *ca=NULL;
    BSetID id; 

    ai->m_inst_bid_list->Clear(); 

    sint32 stored = ai->m_player->GetMaterialsStored (); 
    sint32 nCities = ai->m_city_growth->GetNumCities(); 
    sint32 idx_city; 

    BOOL no_land_improvements = (fz_max_num_dirty_tiles < 
        ((Opportunity_Action_Repair_Installation*)ai->m_opportunity_action[OPPORTUNITY_ACTION_REPAIR_INSTALLATION])
            ->GetDirtyTileCount()); 

    MapPointData pos; 
    sint32 type_terrain;

   
    if ((0 < stored) && (0 < nCities)) {        
        sint32 median = ai->m_city_growth->GetMedian(); 

         
        for (idx_city=0; idx_city<nCities; idx_city++) {
            ca = ai->m_city_growth->GetCity(idx_city)->GetAgent(); 

            if (no_land_improvements) { 
                ca->GetPos(pos); 
                if (GROUND_Z == pos.z) { 
                    if (ai->m_world->GetTerrainType(&pos, &type_terrain)) { 
                        switch (type_terrain) { 
                        case TERRAIN_WATER_SHALLOW:
                        case TERRAIN_WATER_DEEP:
                        case TERRAIN_WATER_VOLCANO:
                        case TERRAIN_WATER_BEACH:
                        case TERRAIN_WATER_SHELF:
                        case TERRAIN_WATER_TRENCH:
                        case TERRAIN_WATER_RIFT:
                        case TERRAIN_SPACE:
                            break; 
                        default:
                            continue; 
                        } 
                    } 
                } 
            }


            ca->FindInstallationBenefits(ai, idx_city, median); 
        }

        
       

        if (!no_land_improvements) { 
            if ((int(ai->m_round_count->GetRound()) % 25) == 0) { 
                LayCellInst(ai, INSTALLATION_AIR_BASES,      100000);
                LayCellInst(ai, INSTALLATION_LISTENING_POSTS, 80000); 
        
                
                
            }
            LayRoads(ai);
        }
    }
}








 
#if 0    
void FSCompeteForInstallations::LayMines(AiMain *ai)
{

    MapPointData *size = ai->m_map->GetSize(); 

    double cell[k_MINECELLX][k_MINECELLY]; 
    double sum; 
    double score;
    double r; 
    BOOL searching;
    sint32 imax = size->x / k_MINECELLX; 
    sint32 jmax = size->y / k_MINECELLY; 
    sint32 i; 
    sint32 j; 
    sint32 x; 
    sint32 y; 
    sint32 prod_cost;
    MapPointData pos; 
    pos.z = 0; 
    sint32 inst_type; 
    ERR_BUILD_INST err; 
    double move_cost;
    GoalInstallation *goal_obj=NULL;
    MapPointData xy_pos; 
    sint32 start_cont; 
    BOOL start_is_land;
    sint32 coef = ai->m_rand->Next(8) + 1; 

    for (i=0; i<imax; i++) { 
        for (j=0; j<jmax; j++) { 

            sum = 0.0;
            for (x = 0; x < k_MINECELLX; x++) { 
                for (y = 0; y < k_MINECELLY; y++) { 
     
                    pos.x = i * k_MINECELLX + x; 
                    pos.y = j * k_MINECELLY + y; 

                    if (SPACE_Z == pos.z) { 
                        inst_type = INSTALLATION_X_RAY_MINES; 
                    } else { 
                        if (ai->m_world->IsWater(&pos)) { 
                            inst_type = INSTALLATION_MAGNETIC_MINES; 
                              if (!ai->m_continents->GetContinent(ai, pos, start_cont, start_is_land)) { 
                                  cell[x][y]=0.0;
                                  continue; 
                              }

                              if (ai->m_world->GetWaterContinentSize(start_cont) < 25) { 
                                  cell[x][y]=0.0;
                                  continue; 
                              } 
         
                        } else { 
                            inst_type = INSTALLATION_LAND_MINES;
                        }
                    }

                    if (ai->m_installationDB->CanBuildHere(inst_type, &pos, &err)) {

                        if (ai->m_world->GetCellOwner(&pos) == ai->m_my_player_id) {
                            score = 1.0;   
                        } else { 
                            score = 6.0; 
                        }
                        

                        if (ai->m_world->IsMyPopThere(&pos)) { 
                            score *= 0.5;
                        } else { 
                            move_cost = ai->m_world->GetMoveCost(&pos); 
                            if (move_cost < 50.0) { 
                                score *= 6.0; 
                            } else if (move_cost < 100.0) { 
                                score *= 2.0; 
                            } 
                        } 

                        ai->m_player->Norm2XY(pos, xy_pos); 
                        score *= (1.0 + ai->m_planner->the_strategic_map->Get_Threat(xy_pos));
                           

                     } else {
                           score = 0.0; 
                     }
                     cell[x][y] = score;
                     sum += score; 
                 }
            }


            if (0.0 < sum) { 
                r = ai->m_rand->Next(1000); 
                r *= sum/1000.0; 
                searching = TRUE; 
                for (x = 0; (x < k_MINECELLX) && searching; x++) { 
                    for (y = 0; (y < k_MINECELLY) && searching; y++) { 
                        if (r < cell[x][y]) { 
                            searching=FALSE; 
                            pos.x = i * k_MINECELLX + x; 
                            pos.y = j * k_MINECELLY + y; 

                            if (SPACE_Z == pos.z) { 
                                inst_type = INSTALLATION_X_RAY_MINES; 
                            } else { 
                                if (ai->m_world->IsWater(&pos)) { 
                                    inst_type = INSTALLATION_MAGNETIC_MINES; 
                                } else { 
                                    inst_type = INSTALLATION_LAND_MINES;
                                }
                            }

                            prod_cost = ai->m_installationDB->GetCost(inst_type, &pos);
                            goal_obj = new GoalInstallation(inst_type, pos, cell[x][y] * coef, NULL, prod_cost);
                            ai->m_inst_bid_list->InsertGoal(goal_obj);
                        } else {
                            r -= cell[x][y];
                        } 
                    }
                }
            }

        }
    }
}

#endif







void FSCompeteForInstallations::LayCellInst(AiMain *ai, sint32 inst_type, 
                                            sint32 utility)
{
    sint32 max_cellx, max_celly;
    ai->m_map->GetCellMax(max_cellx, max_celly);

    MapPointData pos; 
    pos.z = 0; 
    ERR_BUILD_INST err; 
    GoalInstallation *goal_obj=NULL;
    sint32 i; 
    sint32 j; 
    sint32 k; 
    sint32 prod_cost;
    sint32 count; 
    BOOL searching;
    MapPoint XYpos; 
    double threat;
	MapPointData wrap_pos;

    for (i=0; i<max_cellx; i++) { 
        for (j=0; j<max_celly; j++) { 

            pos.x = i * k_MINECELLX;
            pos.y = j * k_MINECELLY;

            switch (inst_type) { 
            case INSTALLATION_FORTIFICATIONS: count = ai->m_map->GetFortCellCount(pos); break;  
            case INSTALLATION_LISTENING_POSTS: count = ai->m_map->GetGuardtowerCellCount(pos); break; 
            case INSTALLATION_AIR_BASES:  count = ai->m_map->GetAirfieldCellCount(pos); break;
            default:
                INSANE(UNKNOWN_INST_TYPE); 
            }

            if (count < 1) { 
                searching = TRUE; 
                for (k=0; k<30 && searching; k++) {
                    pos.x = sint16(i * k_MINECELLX + ai->m_rand->Next(k_MINECELLX-5)); 
                    pos.y = sint16(j * k_MINECELLY + ai->m_rand->Next(k_MINECELLY-5)); 
               
					ai->m_map->Wrap(pos.x, pos.y, pos.z, wrap_pos); 

					ai->m_player->Norm2XY(wrap_pos, XYpos);
                    threat = ai->m_planner->the_strategic_map->Get_Threat(XYpos);    
                    if (fz_inst_threat_threshold < threat) {
                        continue; 
                    } 

                    if (ai->m_installationDB->CanBuildHere(inst_type, 0, &wrap_pos, &err)) {
                         searching = FALSE; 
                         prod_cost = ai->m_installationDB->GetCost(inst_type, &wrap_pos, 0);
                         goal_obj = new GoalInstallation(inst_type, 0, wrap_pos, utility, NULL, prod_cost);
                         ai->m_inst_bid_list->InsertGoal(goal_obj);
                    }
                }

            } 

        }
    }
}


AiMain *g_ai; 

BOOL RoadCallback (BOOL can_enter,  MapPointData *prev,  
   MapPointData *pos, float *cost, BOOL is_zoc, ASTAR_ENTRY_TYPE &entry)
{ 
    if (0 < pos->z) { 
        *cost = k_ASTAR_BIG; 
        return FALSE; 
    } 

    sint32 road_type;
    if (can_enter) { 
        if (g_ai->m_world->IsMoveTypeWater(pos) || 
            g_ai->m_world->IsMoveTypeShallowWater(pos)) { 

            if (g_ai->m_world->CanBuildUnderseaTunnel(pos)) { 
                if (g_ai->m_world->HasUnderseaTunnel(pos)) { 
                    *cost *= 1.1f; 
                } else { 
                    *cost *= 2.1f;  
                }
                return TRUE; 
            } else { 

                
                *cost = k_ASTAR_BIG; 
                return FALSE; 
            }
        } else { 

            if (g_ai->m_world->GetConstructingRoad(pos, &road_type)) { 
                *cost *= 0.3f; 
                return TRUE;
            } else if (g_ai->m_world->GetRoad(pos, &road_type)) { 
                return TRUE;
            } else { 
                *cost *= 1.5f; 
                return TRUE; 
            }
        } 
    } else { 
        *cost = k_ASTAR_BIG; 
		entry = ASTAR_ENTRY_TYPE(0); 
        return FALSE;
    }
}

void FSCompeteForInstallations::LayRoads(AiMain *ai)
{
 
    sint32 best_road;
    
    if (!ai->m_science_agent->GetBestRoadType(best_road)) { 
        return; 
    }


    CityTree *mst=NULL;
    CityEdge *e=NULL; 
    sint32 idx_cont; 
    CityAgent *sa=NULL, *da=NULL; 
    GoalRoad *goal_road=NULL;  
    sint32 num_continents = ai->m_continents->GetNumLandCont(); 

    g_ai = ai; 

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    MapPointData start_pos; 
    MapPointData dest_pos; 
    sint32 bufSize; 
    MapPointData *buffer;
    sint32 nPoints;
    float move_cost;
    uint32 mi;
    sint32 cutoff=200000; 
    sint32 nodes_opened; 

    for (idx_cont=0; idx_cont<num_continents; idx_cont++) { 
        mst = ai->m_continents->GetMst(idx_cont); 

        if (mst == NULL) continue; 

        e = mst->TakeCheapestEdge();
        while (e) { 
Assert(++finite_loop < 100000); 
            sa = e->GetStartCityAgent(); 
            da = e->GetDestCityAgent(); 
            if (!sa->IsConnectedByRoadAtLeast(da, best_road)) { 


                sa->GetPos(start_pos); 
                da->GetPos(dest_pos); 

                if (ai->m_science_agent->HasUnderseaTunnel()) { 
                    mi = k_BIT_MOVEMENT_TYPE_LAND | k_BIT_MOVEMENT_TYPE_MOUNTAIN | 
                        k_BIT_MOVEMENT_TYPE_WATER | k_BIT_MOVEMENT_TYPE_SHALLOW_WATER; 
                } else { 
                    mi = k_BIT_MOVEMENT_TYPE_LAND | k_BIT_MOVEMENT_TYPE_MOUNTAIN;
                }

                bufSize=50; 
                buffer = new MapPointData[bufSize]; 



                if (ai->m_robot_astar->FindPath(RoadCallback, 0, PATH_ARMY_TYPE_MADEUP, 
                    mi, &start_pos, &dest_pos, &bufSize, &buffer, 
                    &nPoints, &move_cost, FALSE, FALSE, FALSE, TRUE, cutoff, nodes_opened, FALSE, 
                    TRUE, FALSE)) {


                    Assert(0 < nPoints); 
                    goal_road = new GoalRoad(ai, best_road, sa, da, 
                        bufSize, buffer, nPoints);

                    ai->m_inst_bid_list->InsertGoal(goal_road);
                } else {

                    delete[] buffer; 
                    buffer = NULL; 
                }
            }
            delete e;
            e = mst->TakeCheapestEdge();
        }
    }
}

void FSCompeteForInstallations::SpendProduction(AiMain *ai)
{
    GoalInstallation *max_goal=NULL;
    sint32 stored = ai->m_player->GetMaterialsStored (); 
    DynamicArray <GoalInstallation *> used_goals; 
    BOOL pos_is_used; 
    sint32 i, n; 

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while ((0 < stored) && ai->m_inst_bid_list->GetHighestProfit(ai, max_goal)) { 
Assert(++finite_loop < 100000); 
        Assert(max_goal); 

        n = used_goals.Num(); 

        pos_is_used = FALSE; 
        for (i=0; i<n && !pos_is_used; i++) { 
            if (used_goals[i]->SamePosition(max_goal)) { 
                pos_is_used = TRUE; 
            } 
        } 

        if (!pos_is_used) { 
            max_goal->Construct(ai, stored); 
            used_goals.InsertFlat(max_goal); 
        } 
    } 
}

