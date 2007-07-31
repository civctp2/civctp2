
#include "c3.h"
#include "globals.h"
#include "c3types.h"
#include "c3errors.h"

#include "IMapPointData.h"
#include "IC3Player.h"
#include "IC3Government.h"

#include "CityAgent.h"

#include "CivArchive.h"
#include "FlatPtr.h"


#include "IC3GameState.h"
#include "IC3Population.h"
#include "IC3World.h"

#include "AiMain.h"
#include "AiCell.h"
#include "AiMap.h"
#include "AiRndCnt.h"


#include "ArmyAgent.h"
#include "CityAgent.h"
#include "AiBldQue.h"
#include "BSet.h"
#include "CivArchive.h"
#include "ForeignAgent.h"
#include "ForeignCity.h"
#include "Foreigner.h"


#include "FzOut.h"

extern double fz_min_happiness; 
extern double fz_max_crime; 
extern double fz_min_food_factor;

extern double fz_i_agreed_to_stop_polluting;


#include "InstBidList.h"
#include "GoalInst.h"

#include "BldAgent.h"


#include "ArmyGoal.h"
#include "GlBlg.h"
#include "GlFreight.h"


#include "AllocWGF.h"
#include "Wotp.h"


#include "WallClock.h"
extern Wall_Clock *g_wall_clock;




void CityAgent::GetCityVariables( 
    AiMain *ai, 
    sint32 &num_field_workers, sint32 &num_slaves, sint32 &affect_pop,
    sint32 &happiness, double &cop, double &crime, double &musician_happiness, 
    double &num_musicians, sint32 &total_city_prod, sint32 &total_city_food)
{

    
    BOOL is_unknown_id=TRUE; 
    
    
    Assert(is_unknown_id == FALSE); 

    num_slaves =  ai->m_population->GetCitySlaveCount(m_id.GetVal(), 
        &is_unknown_id) ;
    

    num_musicians = m_num_musicians; 


    ai->m_player->GetCityHappiness(m_id.GetVal(), &is_unknown_id, 
        &m_old_happiness, &m_old_crime); 
    happiness = m_old_happiness; 
    crime = m_old_crime; 
    ai->m_player->GetCityCrimePrevention(m_id.GetVal(), &is_unknown_id, &cop); 
    

    total_city_prod = m_total_field_prod; 
    total_city_food = m_projected_food;

    if (num_field_workers < 1) { 
        m_ave_food_tile = 5.0; 
        m_ave_prod_tile = 10.0; 
    }  else { 
        m_ave_food_tile = m_total_field_food / num_field_workers; 
        m_ave_prod_tile = m_total_field_prod / num_field_workers; 
    }

    Assert(is_unknown_id == FALSE); 
}


sint32 CityAgent::GetMusicianEffect() const
{ 
    return m_num_musicians; 
} 

sint32 CityAgent::GetNumAffectivePop() const
{ 
    return m_total_citizens; 
} 

sint32 CityAgent::GetNumCitizens() const
{ 
    return m_total_citizens; 
} 

void CityAgent::CalcProjectedStats(
       double prod_effect, double wage, double ration_size, 
       double delta_wgf_happiness, 
       double &net_prod, double net_gold, double &net_food)
{
    double food_needed = m_total_citizens * ration_size; 
    double prod_avg, food_avg; 
    double fw = m_num_field_workers + m_num_slaves; 
    double nm = m_num_musicians; 

    if (0 < fw) {
        prod_avg = m_total_field_prod / fw; 
        food_avg = m_total_field_food / fw; 
    } else { 
        prod_avg = 0.0; 
        food_avg = 0.0; 
    } 
    fw -= m_num_slaves; 

    double projected_food = m_total_field_food; 
    double projected_prod = m_total_field_prod; 

    double projected_happy = m_old_happiness + 
        GetNumAffectivePop() * delta_wgf_happiness; 

    double musician_effect = GetMusicianEffect(); 
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while ((fz_min_happiness < projected_happy) && (0 < nm)) { 
Assert(++finite_loop < 1000); 
        fw += 1.0; 
        nm -= 1.0; 
        projected_happy -= musician_effect; 
        projected_food += food_avg * 0.5;
        projected_prod += prod_avg * 0.5; 
    } 

#ifdef _DEBUG
    finite_loop = 0; 
#endif
    while ((projected_happy < fz_min_happiness) && (0 < fw) &&
        (food_needed < projected_food)) { 
Assert(++finite_loop < 1000); 
        fw -= 1.0; 
        nm += 1.0; 
        projected_happy += musician_effect; 
        projected_food -= food_avg; 
        projected_prod -= prod_avg; 
    } 

    net_prod += (projected_prod * prod_effect); 
    net_gold += (m_total_citizens * wage); 
    net_food += (projected_food - food_needed); 
}











BOOL CityAgent::PopNeedsChanging(AiMain *ai, const double food_coef,                              
    const double prod_coef, const double gold_coef, sint32 idx_city )
{
    BOOL is_unknown_id; 




    if (((m_pop_prod > prod_coef) || (m_pop_prod < prod_coef)) || 
        ((m_pop_food > food_coef) || (m_pop_food < food_coef)) || 
        ((m_pop_gold > gold_coef) || (m_pop_gold < gold_coef))) { 
        return TRUE; 
    }

    if (ai->m_alloc_WGF->IsReset()) { 
        return TRUE; 
    } 

    if (m_pop_needs_changing) { 
        return TRUE; 
    }
	
	if (ai->m_population->HasPopGrown(m_id.GetVal(), &is_unknown_id))
		return (TRUE) ;

	if (is_unknown_id)
		return (FALSE) ;

	if (ai->m_population->HasPopStarved(m_id.GetVal(), &is_unknown_id))
		return (TRUE) ;

	if (is_unknown_id)
		return (FALSE) ;

	if (ai->m_population->WasPopExpelled(m_id.GetVal(), &is_unknown_id))
		return (TRUE) ;

	if (is_unknown_id)
		return (FALSE) ;

	if (ai->m_population->WasImprovementBuilt(m_id.GetVal(), &is_unknown_id))
		return (TRUE) ;

	if (is_unknown_id)
		return (FALSE) ;

	if (ai->m_population->WasTerrainImprovementBuilt(m_id.GetVal(), &is_unknown_id))
		return (TRUE) ;

	if (is_unknown_id)
		return (FALSE) ;

	if (ai->m_population->WasHappinessAttacked(m_id.GetVal(), &is_unknown_id))
		return (TRUE) ;

	if (is_unknown_id)
		return (FALSE) ;

	if (ai->m_population->WasTerrainPolluted())
		return (TRUE) ;

    if ((sint32(ai->m_round_count->GetRound()) % 5) == (idx_city % 5)) 
        return TRUE; 

	return (FALSE) ;
}


BOOL CityAgent::PlaceAllPop( AiMain *ai, const double foodCoef, 
    const double productionCoef, const double goldCoef, 
    const double scienceCoef, const double happyCoef, sint32 idx_city, sint32 nCities)	{

	sint32	idx_pop = 0,
            remaining_pop=0,
			num_pop=0, 
            idx_slave = 0, 
            remaining_slaves=0,
            num_slaves=0, 
            total_pop=0;

    m_pop_prod = productionCoef; 
    m_pop_food = foodCoef; 
    m_pop_gold = goldCoef;
    m_i_need_forced_tile_improvements = FALSE;    

    m_pop_needs_changing = FALSE; 

    TileUtility *open_tile[k_NUM_CITY_TILES]; 
    TileUtility open_tile_data[k_NUM_CITY_TILES]; 

	BOOL	is_unknown_id,
			*popUsed=NULL;

    remaining_slaves = num_slaves = ai->m_population->GetCitySlaveCount(m_id.GetVal(), &is_unknown_id) ;
	total_pop = ai->m_population->GetCityPopCount(m_id.GetVal(), &is_unknown_id) ;
 	remaining_pop = num_pop = (total_pop - remaining_slaves);
    m_total_citizens = num_pop;
   
    m_num_slaves = remaining_slaves; 
    m_num_field_workers = 0; 
    m_num_musicians = 0;

	if (is_unknown_id)
		return (FALSE) ;

    
    BOOL slave_search= TRUE;
    for (idx_slave=0; (idx_slave < total_pop) && slave_search; idx_slave++) { 
       if (ai->m_population->GetCityPopIsSlave(m_id.GetVal(), idx_slave, &is_unknown_id)) { 
          slave_search = FALSE; 
          break; 
       }
    }

    
    BOOL pop_search= TRUE;
    for (idx_pop=0; (idx_pop < total_pop) && pop_search; idx_pop++) { 
       if (!ai->m_population->GetCityPopIsSlave(m_id.GetVal(), idx_pop, &is_unknown_id)) { 
          pop_search = FALSE; 
          break; 
       }
    }

	ai->m_player->AiStartMovingPops(m_id.GetVal());

    
    FindOpenTiles(ai, open_tile, open_tile_data); 

    m_total_field_prod=0; 
    m_total_field_food=0; 

	UpdateProductionBonuses(ai);
    
	SatisfyMinimumCityRequirements(ai, idx_slave, remaining_slaves, idx_pop, 
        remaining_pop, total_pop, open_tile,  foodCoef, productionCoef, goldCoef, 
        scienceCoef, happyCoef, double(idx_city), double(nCities));

    if ((0 < remaining_slaves) || (0 < remaining_pop)) { 
        MaximizeCityOutput(ai, idx_slave, remaining_slaves, idx_pop, 
            remaining_pop, total_pop, foodCoef, productionCoef, goldCoef, 
            scienceCoef, happyCoef, open_tile);
    }

	ai->m_player->AiDoneMovingPops(m_id.GetVal());

	return (TRUE) ;
}


void CityAgent::FindOpenTiles(AiMain *ai, TileUtility *open_tile[k_NUM_CITY_TILES], 
        TileUtility open_tile_data[k_NUM_CITY_TILES]) 
{ 
	BOOL is_unknown_id;
    sint32 num_no_enemy; 
	MapPointData no_enemy[CITY_TILE_SIZE] ;
    sint32 idx_tile; 

	SetPos(ai);

	ai->m_world->GetOpenTileValues(ai->my_player_index, m_pos, &num_no_enemy, no_enemy) ;		

    for (idx_tile=0; idx_tile<num_no_enemy; idx_tile++) {	
        open_tile_data[idx_tile].m_utility = 0.0; 
        open_tile_data[idx_tile].m_pos = no_enemy[idx_tile]; 
        open_tile[idx_tile] = &open_tile_data[idx_tile]; 
    }	    

    for (idx_tile = num_no_enemy; idx_tile<k_NUM_CITY_TILES; idx_tile++) { 
        open_tile[idx_tile] = NULL; 
    } 

    ai->m_player->GetAllTileValue (m_id.GetVal(), &is_unknown_id, 
        num_no_enemy, open_tile);
} 

BOOL CityAgent::SatisfyMinimumCityRequirements(AiMain *ai,
   sint32  &idx_slave, sint32 &remaining_slaves, sint32 &idx_pop, 
   sint32 &remaining_pop, const sint32 total_pop, 
   TileUtility *open_tile[k_NUM_CITY_TILES], 
   const double foodCoef, const double productionCoef, const double goldCoef, 
     const double scienceCoef, const double happyCoef, double idx_city, double nCities)
{ 
    BOOL is_unknown_id=FALSE; 
    double accum_production = ai->m_population->GetTileProduction(m_id.GetVal(), m_pos, &is_unknown_id); 
    double accum_food = ai->m_population->GetTileFood(m_id.GetVal(), m_pos, &is_unknown_id); 

    double max_coef = 10 * (foodCoef + productionCoef + goldCoef +  
            + scienceCoef + happyCoef);

    sint32 rev_hap = ai->m_gs->GetConstRevolutionLevel();
    sint32 riot_hap = ai->m_gs->GetConstRiotLevel(); 

	SupplyEntertainers(ai, idx_pop, remaining_pop, total_pop, rev_hap, 1000.0) ;	

    double percent1 = 1.0; 

    double percent2 = fz_min_food_factor - percent1;




    PlacePopMinimumFood(ai, idx_slave, remaining_slaves, idx_pop,  
        remaining_pop, total_pop, open_tile, foodCoef, 
        productionCoef, max_coef, accum_production, accum_food, 
        percent1);

    SupplyEntertainers(ai, idx_pop, remaining_pop, total_pop, riot_hap, 1000.0) ;	

    if (0.0 < percent2)										
	{												
		
		
		
		long city_size_pop_effect; 
		ai->m_player->GetCitySizePopEffect(m_id.GetVal(), &is_unknown_id, &city_size_pop_effect); 

		
		
		
		if(	((this->GetHappiness() >= ai->m_gs->GetConstRiotLevel() ) 
				&&	( this->GetNumMusicians() == 0 ) )	
			||	( city_size_pop_effect == 0 ) )		
		{


			PlacePopMinimumFood(ai, idx_slave, remaining_slaves, idx_pop, 
				remaining_pop, total_pop, open_tile, foodCoef, 
				productionCoef, max_coef, accum_production, accum_food, percent2);
		}
    }

    PlacePopMinimumProduction(ai, idx_slave, remaining_slaves, idx_pop, 
       remaining_pop, total_pop, open_tile, accum_production);

    if (2 < total_pop) { 
        PlacePopMinimumGold(ai, idx_slave, remaining_slaves, idx_pop, 
            remaining_pop, total_pop, open_tile, foodCoef, 
            productionCoef, max_coef, accum_production, accum_food);
    }

	SupplyEntertainers(ai, idx_pop, remaining_pop, total_pop,  fz_min_happiness, fz_max_crime) ;	
	
    
    ai->m_population->GetCityProjectedProduction(m_id.GetVal(), &m_projected_gross_production, &is_unknown_id);
    ai->m_population->GetCityProjectedFood(m_id.GetVal(), &m_projected_food, &is_unknown_id);
    ai->m_population->GetCityProjectedTrade(m_id.GetVal(), 
        &is_unknown_id, &m_projected_gross_gold, &m_projected_net_gold);

    CalcEstimatedCityProduction();


	return (TRUE) ;
}

void CityAgent::PlacePopMinimumGold(AiMain *ai,
   sint32  &idx_slave, sint32 &remaining_slaves, sint32 &idx_pop, 
   sint32 &remaining_pop, const sint32 total_pop, 
   TileUtility *open_tile[k_NUM_CITY_TILES], const double foodCoef, 
   const double productionCoef, const double max_coef, double &accum_production, 
   double &accum_food)
{
    sint32 idx_tile; 
    sint32 idx_max; 
    double max_gold; 
    double max_food; 
    double max_production; 
    double max_irrigation;
    sint32 searching = TRUE; 

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while (searching && ((0 < remaining_pop) || (0 < remaining_pop))) { 
Assert(++finite_loop < 100000); 
        max_gold = 0.0; 
        max_food = 0.0; 
        max_production = 0.0; 
        max_irrigation = 0.0; 
        searching = FALSE; 
        idx_max = -1;
        for (idx_tile=0; idx_tile<k_NUM_CITY_TILES; idx_tile++) { 
            if (open_tile[idx_tile]) { 
                if (ai->m_world->IsTradeGood(&(open_tile[idx_tile]->m_pos))) { 

                    if (max_gold < open_tile[idx_tile]->m_gold) { 
                        max_gold = open_tile[idx_tile]->m_gold;
                        max_food = open_tile[idx_tile]->m_food; 
                        max_production = open_tile[idx_tile]->m_production; 
                        max_irrigation = open_tile[idx_tile]->m_can_be_irrigated; 
                        idx_max = idx_tile; 
                        searching = TRUE; 
                    } else if (max_gold == open_tile[idx_tile]->m_gold) { 
                        if (max_food < open_tile[idx_tile]->m_food) {                     
                            max_gold = open_tile[idx_tile]->m_gold;
                            max_food = open_tile[idx_tile]->m_food; 
                            max_production = open_tile[idx_tile]->m_production; 
                            max_irrigation = open_tile[idx_tile]->m_can_be_irrigated; 
                            idx_max = idx_tile; 
                            searching = TRUE; 
                        } else if (max_food == open_tile[idx_tile]->m_food) {

                           
                            if (max_production < open_tile[idx_tile]->m_production) { 
                                max_gold = open_tile[idx_tile]->m_gold;
                                max_food = open_tile[idx_tile]->m_food; 
                                max_production = open_tile[idx_tile]->m_production; 
                                max_irrigation = open_tile[idx_tile]->m_can_be_irrigated; 
                                idx_max = idx_tile; 
                                searching = TRUE; 
                            } else if (max_production == open_tile[idx_tile]->m_production) { 

                               if (max_irrigation < open_tile[idx_tile]->m_can_be_irrigated) { 
                                    max_gold = open_tile[idx_tile]->m_gold;
                                    max_food = open_tile[idx_tile]->m_food; 
                                    max_production = open_tile[idx_tile]->m_production; 
                                    max_irrigation = open_tile[idx_tile]->m_can_be_irrigated; 
                                    idx_max = idx_tile; 
                                    searching = TRUE; 
                                }
                            }
                        }
                    }
                }
            }
        }

        if (searching) { 
            accum_production += open_tile[idx_max]->m_production; 
            accum_food += open_tile[idx_max]->m_food; 
            PlaceACitizen(ai, idx_max, idx_slave, remaining_slaves,  idx_pop, 
                 remaining_pop, total_pop, open_tile); 
        }
    } 
}

void CityAgent::PlacePopMinimumFood(AiMain *ai,
   sint32  &idx_slave, sint32 &remaining_slaves, sint32 &idx_pop, 
   sint32 &remaining_pop, const sint32 total_pop, 
   TileUtility *open_tile[k_NUM_CITY_TILES], const double foodCoef, 
   const double productionCoef, const double max_coef, double &accum_production, 
   double &accum_food, const double percent)

{
    sint32 idx_tile; 
    sint32 idx_max; 
    double max_food; 
    sint32 searching = FALSE; 
    sint32 needed_food=0; 
    BOOL is_unknown_id; 
    double f; 

    sint32 g = ai->m_will_of_the_people->GetCurrentGov(); 
    double ration_coef = ai->m_governmentDB->GetRationsCoef(g);
    double ration_unit = ai->m_governmentDB->GetRationsUnit(g); 
    double ration_base = ai->m_governmentDB->GetRationsBase(g); 
    double ration_level_expected = ai->m_governmentDB->GetRationsExpectation(g);
    double ration_expected = total_pop * (5.0 + 2.5 * (2.0 + ration_level_expected)); 
    sint32 nf;

    ai->m_population->GetCityRequiredFood(m_id.GetVal(), &nf, &is_unknown_id);

    
    

    
    needed_food = sint32(ceil(percent * double(nf))); 

    searching = FALSE; 

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while (!searching && (accum_food < needed_food) &&
     ((0 < remaining_slaves) || (0 < remaining_pop))) { 

Assert(++finite_loop < 100000); 

        max_food = 0.0; 
        searching = TRUE; 
        idx_max = -1;
        for (idx_tile=0; idx_tile<k_NUM_CITY_TILES; idx_tile++) { 
            if (open_tile[idx_tile]) { 
                f = open_tile[idx_tile]->m_food * max_coef + 
                    open_tile[idx_tile]->m_can_be_irrigated +
                    open_tile[idx_tile]->m_gold +
                    open_tile[idx_tile]->m_production * 0.25 ;

                if (max_food < f) { 
                    max_food = f; 
                    idx_max = idx_tile; 
                    searching = FALSE; 
                } 
            }
        }

        if (!searching) { 
            accum_food += open_tile[idx_max]->m_food; 
            accum_production += open_tile[idx_max]->m_production; 
            PlaceACitizen(ai, idx_max, idx_slave, remaining_slaves,  idx_pop, 
                 remaining_pop, total_pop, open_tile); 
        }
    }
}

void CityAgent::PlacePopMinimumProduction( AiMain *ai,
   sint32  &idx_slave, sint32 &remaining_slaves, sint32 &idx_pop, 
   sint32 &remaining_pop, const sint32 total_pop, 
   TileUtility *open_tile[k_NUM_CITY_TILES], double &accum_production)
{
    if (0 < accum_production) { 
        return; 
    } 

    if ((remaining_slaves < 1) &&  (remaining_pop < 1)) {
        return; 
    } 
   
    double max_production = 0.0; 
    double f; 
    BOOL searching = TRUE; 
    sint32 idx_max = -1;
    sint32 idx_tile; 
    for (idx_tile=0; idx_tile<k_NUM_CITY_TILES; idx_tile++) { 
        if (open_tile[idx_tile]) { 
            f = open_tile[idx_tile]->m_production * 100.0 + 
                open_tile[idx_tile]->m_gold + 
                open_tile[idx_tile]->m_food * 0.5 +
                open_tile[idx_tile]->m_can_be_irrigated;

            if (max_production < f) { 
                max_production = f; 
                idx_max = idx_tile; 
                searching = FALSE; 
            }
        }
    }

    if (!searching) { 
         PlaceACitizen(ai, idx_max, idx_slave, remaining_slaves,  idx_pop, 
             remaining_pop, total_pop, open_tile); 
    }
}

void CityAgent::PlaceACitizen(
    AiMain *ai, const sint32 idx_tile, sint32 &idx_slave, 
    sint32 &remaining_slaves,  sint32 &idx_pop, sint32 &remaining_pop, 
    const sint32 total_pop,  TileUtility *open_tile[k_NUM_CITY_TILES]) 
{
    BOOL slave_search; 
    BOOL pop_search; 
    BOOL is_unknown_id=FALSE; 
    sint32 r; 

    sint32 tmp;

    Assert((0 < remaining_slaves) || (0 < remaining_pop)); 
    if ((idx_slave < total_pop) && (0 < remaining_slaves)) { 
START_MULTI_CLOCK
        r = ai->m_population->TryPlacePop(m_id.GetVal(), idx_slave, 
            ai->my_player_index, &open_tile[idx_tile]->m_pos, &is_unknown_id);
END_MULTI_CLOCK

           ai->m_world->GetProd(&(open_tile[idx_tile]->m_pos), &tmp); 
           m_total_field_prod += tmp; 
           ai->m_world->GetFood(&(open_tile[idx_tile]->m_pos), &tmp); 
           m_total_field_food += tmp; 


        Assert(is_unknown_id == FALSE); 
        Assert(r);  
        open_tile[idx_tile] = NULL; 
        remaining_slaves--;
        slave_search = TRUE; 
        for (idx_slave++ ; (idx_slave < total_pop) && slave_search; idx_slave++) { 
             if (ai->m_population->GetCityPopIsSlave(m_id.GetVal(), idx_slave, &is_unknown_id)) { 
                  slave_search = FALSE; 
                  break; 
             }
        }
    } else { 
START_MULTI_CLOCK
         r = ai->m_population->TryPlacePop(m_id.GetVal(), idx_pop, 
            ai->my_player_index, &open_tile[idx_tile]->m_pos, &is_unknown_id);
END_MULTI_CLOCK

            ai->m_world->GetProd(&(open_tile[idx_tile]->m_pos), &tmp); 
            m_total_field_prod += tmp; 
            ai->m_world->GetFood(&(open_tile[idx_tile]->m_pos), &tmp); 
            m_total_field_food += tmp; 

            m_num_field_workers++; 

         Assert(is_unknown_id == FALSE); 
         Assert(r);
#ifdef _DEBUG 
         if ( !r) {
              r = ai->m_population->TryPlacePop(m_id.GetVal(), idx_pop, 
                ai->my_player_index, &open_tile[idx_tile]->m_pos, &is_unknown_id);
         }
#endif

         open_tile[idx_tile] = NULL; 
         remaining_pop--;
         pop_search=TRUE; 
         for (idx_pop++ ; (idx_pop < total_pop) && pop_search; idx_pop++) { 
             if (!ai->m_population->GetCityPopIsSlave(m_id.GetVal(), idx_pop, 
                  &is_unknown_id)) { 
                  pop_search = FALSE; 
                  break;
              }
         }
    }
}












void CityAgent::SupplyEntertainers(AiMain *ai, sint32 &idx_pop, 
    sint32 &remaining_pop,  const sint32 total_pop, const double min_happiness, 
    const double max_crime)
{
	BOOL is_unknown_id;

    

    double raw_happiness; 
    double happy_per_entertainer;

    ai->m_population->GetRawHappiness(m_id.GetVal(), &is_unknown_id, 
        &raw_happiness, &happy_per_entertainer);

    

    raw_happiness += happy_per_entertainer * m_num_musicians;
	for ( ; (idx_pop<total_pop) && (remaining_pop>0) && ((raw_happiness < min_happiness)); idx_pop++){
		ai->m_population->SetCityPopInCity(m_id.GetVal(), idx_pop, TRUE, &is_unknown_id) ;	
		ai->m_population->SetCityPopType(m_id.GetVal(), idx_pop, POPTYPE_MUSICIAN, &is_unknown_id) ;	
        m_num_musicians++; 
		remaining_pop-- ;										
        raw_happiness += happy_per_entertainer;
	}
}











void CityAgent::MaximizeCityOutput(AiMain *ai,
   sint32  &idx_slave, sint32 &remaining_slaves, sint32 &idx_pop, 
   sint32 &remaining_pop, const sint32 total_pop, 
   double foodCoef, double productionCoef, double goldCoef, 
   double scienceCoef, double happyCoef, 
   TileUtility *open_tile[k_NUM_CITY_TILES])
{
#ifdef CTP1_HAS_RISEN_FROM_THE_GRAVE
    sint32 idx_tile=0; 
    CalcTileUtility(open_tile, foodCoef, productionCoef, goldCoef); 
    SortOpenTiles(open_tile);

	BOOL stop_pollution_test = FALSE;
	if (0.9 < fz_i_agreed_to_stop_polluting && fz_i_agreed_to_stop_polluting < 1.1) 
	{
		stop_pollution_test = TRUE;
	}

	BOOL is_unknown_id=FALSE;
    uint32 u_id;   
    u_id = m_id.GetVal(); 
    for ( ; (idx_slave<total_pop) && remaining_slaves && (idx_tile < k_NUM_CITY_TILES); idx_slave++) { 
        if (ai->m_population->GetCityPopIsSlave(u_id, idx_slave, &is_unknown_id)) { 
            PutCitizenBestTile(ai, idx_slave, open_tile, idx_tile);
            remaining_slaves--;
			








        }
        Assert(is_unknown_id == FALSE); 
    } 

    
     sint32 pop_best_scientist;
     sint32 pop_best_science; 
     sint32 pop_best_grunt;
     sint32 pop_best_production;
     sint32 pop_best_musician; 
     sint32 pop_best_luxury;
     sint32 pop_best_banker;
     sint32 pop_best_gold;

     ai->m_player->GetBestProfessionals(m_id.GetVal(), &is_unknown_id, 
        &pop_best_scientist, &pop_best_science,  &pop_best_grunt, &pop_best_production, 
        &pop_best_musician, &pop_best_luxury, &pop_best_banker,  &pop_best_gold
     );



	 Assert(0 <= pop_best_scientist); 

	

     double prof_utility = -1000000.0; 
     sint32 prof = POPTYPE_SCIENTIST; 

     double scientist_utility;
	 if (0 <= pop_best_scientist) { 
		 scientist_utility = 
			 pop_best_science * scienceCoef * (1 + GetProductionBonus(PRODUCTION_TYPE_KNOWLEDGE)); 
	 } else { 
		 scientist_utility = prof_utility - 10;
	 }

     double grunt_utility;
	 if (0 <= pop_best_grunt) { 
		 grunt_utility = pop_best_production * productionCoef * ( 1 + GetProductionBonus(PRODUCTION_TYPE_PRODUCTION));
	 } else { 
		 grunt_utility = prof_utility - 10;
	 }

     double musician_utility; 
	 if (0 <= pop_best_musician) { 
		 musician_utility = pop_best_luxury * happyCoef;
	 } else { 
		 musician_utility = prof_utility - 10;
	 }

	 double banker_utility;
	 if (0 <= pop_best_banker) { 
		 banker_utility = pop_best_gold * goldCoef * (1 + GetProductionBonus(PRODUCTION_TYPE_GOLD));
	 } else { 
		 banker_utility = prof_utility - 10;
	 }


     
     if (grunt_utility < scientist_utility) 
	 { 
        if (scientist_utility < musician_utility) 
		{  
			if ( musician_utility < banker_utility) 
			{										
				prof_utility = banker_utility;
				prof = POPTYPE_BANKER;
			} else								
			{
				prof_utility = musician_utility;  
				prof = POPTYPE_MUSICIAN;
			}
        } else 
		{
			if (scientist_utility < banker_utility) 
			{											
				prof_utility = banker_utility;			
				prof = POPTYPE_BANKER;
			} else								
			{	
				prof_utility = scientist_utility; 
				prof = POPTYPE_SCIENTIST; 
			}
        } 
    } else 	
	{								
        if (grunt_utility < musician_utility) { 
			if ( musician_utility < banker_utility)	
			{
				prof_utility = banker_utility;			
				prof = POPTYPE_BANKER;
			} else
			{
	            prof_utility = musician_utility;        
			    prof = POPTYPE_MUSICIAN;
			}
        } else								
		{				
			if (grunt_utility < banker_utility) 
			{
				prof_utility = banker_utility;			
				prof = POPTYPE_BANKER;
			} else
            {
				prof_utility = grunt_utility; 
	            prof = POPTYPE_GRUNT; 
			}
        } 
    } 

	if (prof_utility <=  -1000000.0) {  
      prof = POPTYPE_SCIENTIST; 
	} 


    for ( ; (idx_pop<total_pop) && remaining_pop && (idx_tile < k_NUM_CITY_TILES); idx_pop++) { 
        if (!ai->m_population->GetCityPopIsSlave(u_id, idx_pop, &is_unknown_id)) { 
            PutCitizenBestPosition(ai, idx_pop, prof_utility, POPTYPE(prof),  
                open_tile, idx_tile);
            remaining_pop--; 
			








        }
        Assert(is_unknown_id== FALSE); 
    } 
#endif
}


void CityAgent::CalcTileUtility(TileUtility *open_tile[k_NUM_CITY_TILES],                                 
     const double foodCoef, const double productionCoef, const double goldCoef)
{ 
#if 0
	sint32 i; 

   for (i=0; i<k_NUM_CITY_TILES; i++) { 
       if (open_tile[i]) { 
           open_tile[i]->m_utility = foodCoef * open_tile[i]->m_food +
               productionCoef * open_tile[i]->m_production + 
               goldCoef * open_tile[i]->m_gold * (1 + GetProductionBonus(PRODUCTION_TYPE_GOLD)) +
               open_tile[i]->m_can_be_irrigated; 
       } 
   } 
#endif
}

void CityAgent::SortOpenTiles(TileUtility *open_tile[k_NUM_CITY_TILES])
{
    
   sint32 i, j; 
   TileUtility *tmp=NULL; 

   for (i=0; i<(k_NUM_CITY_TILES+1); i++) { 
       for (j=i+1; j<k_NUM_CITY_TILES; j++) { 
           if (open_tile[j]) { 
               if (open_tile[i] == NULL) { 
                   open_tile[i] = open_tile[j]; 
                   open_tile[j] = NULL; 
               } else if (open_tile[i]->m_utility < open_tile[j]->m_utility) { 
                   tmp = open_tile[i]; 
                   open_tile[i] = open_tile[j]; 
                   open_tile[j] = tmp; 
               } 
           }
       } 
   } 
}

double CityAgent::CalcGruntUtility( AiMain *ai, const double productionCoef)
{
    BOOL is_unknown_id; 
    double projectedGrunt;

    projectedGrunt = ai->m_population->GetCityGruntOutput(m_id.GetVal(), 
            &is_unknown_id);
	return  projectedGrunt * productionCoef ;
}


double CityAgent::CalcScientistUtility( AiMain *ai, const double scienceCoef)
{
    BOOL is_unknown_id; 
    double projectedScience;

    projectedScience = ai->m_population->GetCityScientistOutput(m_id.GetVal(), 
            &is_unknown_id);
	return  projectedScience * scienceCoef ;
}


double CityAgent::CalcMusicianUtility( AiMain *ai, const double happyCoef)
{
    BOOL is_unknown_id; 
    double projectedMusic;

    projectedMusic = ai->m_population->GetCityMusicianOutput(m_id.GetVal(), 
            &is_unknown_id);
	return  projectedMusic * happyCoef ;
}

BOOL CityAgent::PutCitizenBestPosition( AiMain *ai, const sint32 idx_pop, 
   const double prof_utility, const POPTYPE prof,
   TileUtility *open_tile[k_NUM_CITY_TILES], sint32 &idx_tile)
                
{
    Assert(idx_tile < k_NUM_CITY_TILES);
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while ((idx_tile < k_NUM_CITY_TILES) && (open_tile[idx_tile] == NULL)) { 
Assert(++finite_loop < 100000); 
      idx_tile++; 
    }

    if ((idx_tile < k_NUM_CITY_TILES) && open_tile[idx_tile]) { 
        if (open_tile[idx_tile]->m_utility < 1) { 
            m_i_need_forced_tile_improvements = TRUE; 
        } 
    }

    BOOL is_unknown_id; 




	
	
	
	
    if ((idx_tile < k_NUM_CITY_TILES && open_tile[idx_tile]) && 
		((prof_utility <= open_tile[idx_tile]->m_utility) || 
        (-1 == prof) ||  (m_pos->z == SPACE_Z))) {

        return PutCitizenBestTile(ai, idx_pop, open_tile, idx_tile);
    } else { 
		ai->m_population->SetCityPopType(m_id.GetVal(), idx_pop, prof, &is_unknown_id) ;	
        Assert(is_unknown_id == FALSE); 

        if (is_unknown_id != FALSE)
            ai->m_population->SetCityPopType(m_id.GetVal(), idx_pop, prof, &is_unknown_id) ;	
        
        return TRUE; 
    }
}
   

BOOL CityAgent::PutCitizenBestTile( AiMain *ai, const sint32 idx_citizen, 
     TileUtility *open_tile[k_NUM_CITY_TILES], sint32 &idx_tile)
{
    BOOL searching=TRUE; 
    BOOL is_unknown_id=FALSE; 
    BOOL r; 
    sint32 tmp; 

    Assert(0 <= idx_tile);
    Assert(idx_tile < k_NUM_CITY_TILES);
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    do {
        if (open_tile[idx_tile]) { 
START_MULTI_CLOCK
            r = ai->m_population->TryPlacePop(m_id.GetVal(), idx_citizen, 
                ai->my_player_index, &open_tile[idx_tile]->m_pos, &is_unknown_id);
END_MULTI_CLOCK
            if (r) { 
                ai->m_world->GetProd(&(open_tile[idx_tile]->m_pos), &tmp); 
                m_total_field_prod += tmp; 
                ai->m_world->GetFood(&(open_tile[idx_tile]->m_pos), &tmp); 
                m_total_field_food += tmp; 

                m_num_field_workers++; 
  
                open_tile[idx_tile] = NULL; 
                searching = FALSE; 
            } else { 
                INSANE(COULD_NOT_PLACE_POP); 
            }
        } 
        idx_tile++; 
Assert(++finite_loop < 100000); 
   } while (searching && (idx_tile < k_NUM_CITY_TILES)); 

   return !searching; 
}


