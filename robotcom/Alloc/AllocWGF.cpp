#include "c3.h"


  
#include "AllocWGF.h"
#include "FlatPtr.h"
#include "CivArchive.h"

#include "IC3GameState.h"
#include "IC3Player.h"
#include "IC3Government.h"
#include "IC3Population.h"

#include "AiMain.h"
#include "BSet.h"


#include "CityAgent.h"
#include "WotP.h"


#include "FzOut.h"
#include "Gold.h"



extern double fz_wgf_prod;
extern double fz_wgf_food;
extern double fz_wgf_gold;
        
extern double fz_likely_available_food;

extern double fz_food_max; 
extern double fz_food_min; 
extern double fz_work_min; 
extern double fz_use_expectations;

extern double fz_desired_wages;
extern double fz_wage_setting_bonus;




ZEROMEM(AllocWGFFlat); 

AllocWGFPtr::AllocWGFPtr()
{
}

ZERODEL(AllocWGFPtr); 
FLATSERIALIZE(AllocWGFFlat); 
PTRSERIALIZE(AllocWGFPtr); 

AllocWGF::AllocWGF()
{ 
    m_gold_unit = 2.0; 
}

AllocWGF::AllocWGF(CivArchive &archive)
{ 
    Serialize(archive); 
} 

AllocWGF::~AllocWGF()
{ 
} 

void AllocWGFPtr::Store (CivArchive &archive)
{
}

void AllocWGFPtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
}

void AllocWGF::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE 

    AllocWGFFlat::Serialize(archive); 
    
}



void AllocWGF::SetRates(AiMain *ai) 
{ 

    InitSystemVariables(ai);

    if (m_num_cities < 1 || (fz_use_expectations > .99) ) { 
       SetLevel(ai, m_work_expected, m_gold_expected, m_food_expected); 
       m_old_crime = 0.95; 
       return; 
    } 

    sint32 w, g, f; 
    sint32 w_best=2, g_best=2, f_best=2; 
    double u, u_best = -10000000000000.0;
    double new_crime;   
	sint32 max_work = 0, max_food = 0, min_food = -2;

    if (1.5 < fz_work_min) { 
        max_work = 2; 
    } else if (0.5 < fz_work_min) { 
        max_work = 1; 
    } else if (-0.5 < fz_work_min) { 
        max_work = 0;
    } else if (-1.5 < fz_work_min) { 
        max_work = -1;
    } else { 
        max_work = -2;
    }
        
    if (1.5 < fz_food_max) { 
        max_food = 2; 
    } else if (0.5 < fz_food_max) { 
        max_food = 1; 
    } else if (-0.5 < fz_food_max) { 
        max_food = 0;
    } else if (-1.5 < fz_food_max) { 
        max_food = -1;
    } else { 
        max_food = -2;
    }

    if (1.5 < fz_food_min) { 
        min_food = 2; 
    } else if (0.5 < fz_food_min) { 
        min_food = 1; 
    } else if (-0.5 < fz_food_min) { 
        min_food = 0;
    } else if (-1.5 < fz_food_min) { 
        min_food = -1;
    } else { 
        min_food = -2;
    }

	if(min_food > max_food)
		min_food = max_food;

	if(min_food < m_food_min ) min_food = m_food_min ;
	if(max_food > m_food_max ) max_food = m_food_max ;
	if(max_work > m_work_max ) max_work = m_work_max ;

	
    
    for (w = m_work_min ; w <= max_work; w++) { 
        for (g = m_gold_min; g <= m_gold_max; g++) { 
            for (f = min_food; f <= max_food; f++) { 
				
				
                if (((w - m_work_expected) + (g - m_gold_expected) + (f - m_food_expected)) > 1) 
                    continue; 

				
				
				if (((w - m_work_expected) + (g - m_gold_expected) + (f - m_food_expected)) < 0)
                    continue; 

			   u = CalcWGFUtility(ai, w, g, f, new_crime, FALSE); 

			   m_utility[w + 2][g + 2][f + 2] = u; 
                if (u_best < u) { 
                    u_best = u; 
                    w_best = w; 
                    g_best = g; 
                    f_best = f; 
                    m_old_crime = new_crime; 
                } 
            } 
        } 
    } 
	
	
    



































































	if ((((w_best - m_work_expected) + 
		  (g_best - m_gold_expected) + 
		  (f_best - m_food_expected)) < 0) ||
		(((w_best - m_work_expected) + 
		  (g_best - m_gold_expected) + 
		  (f_best - m_food_expected)) > 1))
		{
			SetLevel(ai, m_work_expected, m_gold_expected, m_food_expected); 
		} 
	else
		{
			SetLevel(ai, w_best, g_best, f_best);    
		}
} 

void AllocWGF::SetLevel(AiMain *ai, const sint32 w, const sint32 g, const sint32 f)
{ 
    Assert(m_work_min <= w);
    Assert(w <= m_work_max);
    Assert(m_food_min <= f);
    Assert(f <= m_food_max);
    Assert(m_gold_min <= g);
    Assert(g <= m_gold_max);


    m_work_level = w; 
    m_gold_level = g; 
    m_food_level = f; 
     
    m_is_reset = ((m_old_work_level != m_work_level) ||
                (m_old_gold_level != m_gold_level) ||
                (m_old_food_level != m_food_level));
    
    ai->m_player->SetWGF(w, g, f); 
}

void AllocWGF::RegisterNewWGF (sint32 w, sint32 g, sint32 f)
{
    m_work_level = w; 
    m_gold_level = g; 
    m_food_level = f; 

    m_is_reset = TRUE; 
}

void AllocWGF::GetLevel(double &w, double &g, double &f)
{    
    w = m_work_level; 
    g = m_gold_level; 
    f = m_food_level; 
}

void AllocWGF::CalcWorkRange()
{
    m_work_min = -2; 
    m_work_max = 2; 
}

void AllocWGF::CalcGoldRange() 
{
    m_gold_min = -2; 
    m_gold_max = 2; 
}

void AllocWGF::CalcFoodRange(AiMain *ai)
{
    m_food_min = -2; 
    m_food_max = 2; 
   

























}

void AllocWGF::InitCityVariables(AiMain *ai)
{
    CityAgent *ca=NULL; 
    BSetID id; 

    m_num_cities=0; 
    sint32 num_field_workers;
    sint32 num_slaves; 
    sint32 happy;
    m_num_slaves = 0; 
    double cop, total_cop = 0.0; 
    double crime, total_crime = 0.0; 
    double musician_happiness, total_musician_happiness = 0.0;
    double num_musician, total_num_musician = 0.0; 
    sint32 total_city_prod, total_prod_tile = 0; 
    sint32 total_city_food, total_food_tile = 0; 
    sint32 num_affect_pop; 
    double total_base_happy = 0.0; 
    double size, wgf; 

    m_total_affect_pop = 0.0; 
    m_total_field_workers = 0.0; 

    m_ave_affect_pop = 0; 
    m_max_affect_pop = 0; 
    double raw_wgf;

    m_city_pop.Clear();
    m_city_slaves.Clear();
    m_food_avail.Clear();

    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) { 

        ca->GetCityVariables(ai, num_field_workers, 
            num_slaves, num_affect_pop, 
            happy, cop, crime, musician_happiness, 
            num_musician, total_city_prod, total_city_food); 

        m_city_pop.InsertFlat(num_affect_pop);
        m_city_slaves.InsertFlat(num_slaves); 
        m_food_avail.InsertFlat((float)total_city_food); 

        m_num_cities += 1.0; 
        m_total_affect_pop += num_affect_pop; 
        if (m_max_affect_pop < num_affect_pop) {
            m_max_affect_pop = num_affect_pop; 
        } 


         size =  m_big_city_coef * min (m_big_city_offset - num_affect_pop, 0);
         raw_wgf = ( 
            m_work_coef * (m_work_level - m_work_expected) + 
            m_gold_coef * (m_gold_level - m_gold_expected) + 
            m_food_coef * (m_food_level - m_food_expected));

        if (raw_wgf <= 0) { 
            wgf = num_affect_pop * raw_wgf; 
        } else { 
            if (num_affect_pop == 1) { 
                wgf = 1; 
            } else { 
                wgf = min(2.0, raw_wgf); 
            } 
        } 

		 
		 
		 
		 total_base_happy += happy - (wgf); 

        m_total_field_workers += num_field_workers; 
        m_num_slaves += num_slaves; 
        total_cop += cop; 
        total_crime += crime; 
        total_musician_happiness  += musician_happiness; 
        total_num_musician += num_musician; 
        total_prod_tile += total_city_prod; 
        total_food_tile += total_city_food; 
    } 

    
    
    double r; 
    double p; 
    if (0 < m_num_cities ) { 
        r = 1.0 / double(m_num_cities); 
        m_ave_cop = total_cop * r; 
        m_ave_affect_pop = m_total_affect_pop * r; 
        m_ave_field_workers = m_total_field_workers * r; 

        m_old_happy = total_base_happy * r; 
        m_ave_musician_happiness = total_musician_happiness * r; 
        m_ave_num_musicians = total_num_musician * r; 

        p = m_total_field_workers + m_num_slaves; 
        if (0 < p) { 
            m_raw_prod_tile = max(5.0, total_prod_tile / p); 
            m_raw_food_tile = total_food_tile / p;
        } else { 
            m_raw_prod_tile = 5.0; 
                                    
            m_raw_food_tile = 0; 
        } 
    }
}

void AllocWGF::InitGovernmentVariables(AiMain *ai)
{
    sint32 g = ai->m_will_of_the_people->GetCurrentGov(); 
    m_crime_coef =  ai->m_governmentDB->GetCrimeCoef(g);
	m_crime_threshold =  ai->m_governmentDB->GetCrimeOffset(g);
    m_revolution = ai->m_gs->GetConstRevolutionLevel();
    m_riot = ai->m_gs->GetConstRiotLevel(); 

    m_work_coef = ai->m_governmentDB->GetWorkdayCoef(g); 
    m_work_unit = ai->m_governmentDB->GetWorkdayUnit(g); 
    m_work_base = ai->m_governmentDB->GetWorkdayBase(g); 
	m_work_expected = sint32(ai->m_governmentDB->GetWorkdayExpectation(g));

	m_gold_coef = ai->m_governmentDB->GetWagesCoef(g);
    m_gold_unit = ai->m_governmentDB->GetWagesUnit(g); 
    m_gold_base = ai->m_governmentDB->GetWagesBase(g); 
	m_gold_expected = sint32(ai->m_governmentDB->GetWagesExpectation(g));

    m_food_coef = ai->m_governmentDB->GetRationsCoef(g);
    m_food_unit = ai->m_governmentDB->GetRationsUnit(g); 
    m_food_base = ai->m_governmentDB->GetRationsBase(g); 
	m_food_expected = sint32(ai->m_governmentDB->GetRationsExpectation(g));
    
    m_big_city_coef = ai->m_governmentDB->GetBigCityCoef();
    m_big_city_offset = ai->m_governmentDB->GetBigCityOffset(); 

    m_citizen_hunger = ai->m_population->GetCitizensHunger();
    m_slave_hunger = ai->m_population->GetSlaveHunger(); 

}

void AllocWGF::InitSystemVariables(AiMain *ai)
{
    
    CalcWorkRange();
    CalcGoldRange(); 
    CalcFoodRange(ai); 

    m_old_work_level = m_work_level; 
    m_old_gold_level = m_gold_level; 
    m_old_food_level = m_food_level; 
    m_is_reset = FALSE; 

    InitGovernmentVariables(ai);
    InitCityVariables(ai); 

    if (m_num_cities < 1) { 
        return; 
    }

    m_ave_base_happy = m_old_happy;        
    
   
}

double AllocWGF::CalcNetHap(const double nm, const double ap, 
                            const sint32 w, const sint32 g, const sint32 f)
{ 
    
    double music = nm * m_ave_musician_happiness;
    double raw_wgf = (m_work_coef *(w - m_work_expected) + 
                m_gold_coef *(g - m_gold_expected) + 
                m_food_coef *(f - m_food_expected));

    double wgf;
    if (raw_wgf <= 0) { 
        wgf = ap * raw_wgf; 
    } else { 
        if (ap == 1) { 
            wgf = 1; 
        } else { 
            wgf = min(2.0, raw_wgf); 
        } 
    } 

    wgf = ap * raw_wgf; 

    return m_ave_base_happy + music + wgf; 
}

void AllocWGF::CalcHappy (const sint32 w, const sint32 g, const sint32 f, 
     double &field_workers, double &net_happy, double &new_crime) 
{ 
        
    double fw = m_ave_field_workers;
    double nm = m_ave_num_musicians; 
    
    
	net_happy = CalcNetHap(nm, m_max_affect_pop, w, g, f); 
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while ((0.0 < nm)  && (m_riot < net_happy))  { 
Assert(++finite_loop < 100000); 
        nm -= 1.0; 
        fw += 1.0; 

        net_happy = CalcNetHap(nm, m_max_affect_pop, w, g, f); 
    }

    while ((1.0 <= fw) && (net_happy < (m_riot+1)))  { 
Assert(++finite_loop < 100000); 
        nm += 1.0; 
        fw -= 1.0; 

        net_happy = CalcNetHap(nm, m_max_affect_pop, w, g, f); 
            
    }

    field_workers = m_num_slaves + fw * m_num_cities; 

    double base_crime; 

    
    
    
    if (net_happy > m_crime_threshold) { 
        new_crime = 1.0; 
    } else { 
        base_crime = m_crime_threshold - net_happy; 
        new_crime = max(0.0, 1.0 - 0.01 * base_crime * m_ave_cop * m_crime_coef) ;
    }

    new_crime = 1.0; 
    m_ave_crime = new_crime; 
}


void AllocWGF::CalcProduction(const sint32 w, const double field_workers, 
                              
                              double &prod_net)
{ 
    double tile_prod = m_raw_prod_tile + m_raw_prod_tile * (- w * m_work_unit + m_work_base);

    prod_net = tile_prod * field_workers * m_ave_crime; 
}

void AllocWGF::CalcFood(const sint32 f, const sint32 w, const double field_workers, double &food_net)
{ 
    double food_citizen = max(1.0,  m_citizen_hunger * (f * m_food_unit + m_food_base)); 
    double food_slave = m_slave_hunger * (f * m_food_unit + m_food_base); 
    double food_eaten;
    double leftovers; 
    double min_leftover=10000.0; 

    sint32 city_idx; 
    for (city_idx=0; city_idx<m_num_cities; city_idx++) { 
        food_eaten = m_city_pop[city_idx] * food_citizen + m_city_slaves[city_idx] * food_slave;
        leftovers = ( m_food_avail[city_idx] * fz_likely_available_food)  - food_eaten; 
        if (leftovers < min_leftover) { 
            min_leftover = leftovers; 
        } 
    }

    
    
    double prod_new_citizen = m_raw_prod_tile * (min_leftover / food_citizen);
    if (0.0 < prod_new_citizen) { 
        food_net = (-w * m_work_unit + m_work_base) * prod_new_citizen  * m_ave_crime;  
    } else { 
        food_net = -(prod_new_citizen * prod_new_citizen) -100.0; 
    } 
}



















double AllocWGF::GetCurrentRation()
{
    Assert(0.0 <m_citizen_hunger);
    Assert(0.0 < (m_food_level * m_food_unit + m_food_base)); 

    return m_citizen_hunger * (m_food_level * m_food_unit + m_food_base); 
}


double AllocWGF::ProdOneCitizen()
{
	
    double p = m_work_unit * m_raw_prod_tile * m_ave_crime;
    
    
    return max(p, 2.5); 
}

double AllocWGF::WageOneCitizen()
{
    return m_gold_unit;
}




double AllocWGF::Gold2Prod(const double gold)
{
    double p = ProdOneCitizen(); 
    double g = WageOneCitizen(); 

    Assert (0.0 < g); 
    if (g < 1.0) { 
        g = 1.0; 
    } 

    return gold * (p/g); 
}

void AllocWGF::ContentWages(AiMain *ai, double &wages, double &percent_income) 
{
    
    double i = ai->m_gold->GetProjectedGrossIncome(); 
    i -= ai->m_gold->GetLostToCleric();     
    i = i * m_old_crime;  
    i -= ai->m_gold->GetOldMaintenance(); 

    wages = m_total_affect_pop * (m_gold_expected * m_gold_unit + m_gold_base);     
    if (wages < 1) { 
        percent_income = 0.0;
    } else if (i < 1.0) { 
        percent_income = 0.0; 
    } else { 
        percent_income = wages/i; 
    }
}

void AllocWGF::CalcGold(AiMain *ai, const sint32 g, 
  const double field_workers, double &gold_net) 
{
    
    double i = ai->m_gold->GetProjectedGrossIncome(); 
    i -= ai->m_gold->GetLostToCleric();     
    i = i * m_ave_crime;  
    i -= ai->m_gold->GetOldMaintenance(); 

    double target_spending ;
    if (0 <= i) {     
        target_spending = i * ai->m_gold->GetPercentInWages();
    } else { 
        target_spending = 0.0; 
    }

     double wages_spent = m_total_affect_pop * (g * m_gold_unit + m_gold_base);     
     double diff = target_spending - wages_spent; 

     double gold_prod = Gold2Prod(diff); 

     if (0 <= gold_prod ) { 
                    
                    
        gold_net = gold_prod ; 
    } else { 
        if (wages_spent < (i + ai->m_gold->GetCurrentSavings())) { 
            gold_net = -(gold_prod  * gold_prod); 
        } else {  
            gold_net = -100 - 10.0 * (gold_prod * gold_prod); 
        } 
    } 
 
}

sint32 g_dump; 

double AllocWGF::CalcWGFUtility(AiMain *ai, sint32 w, sint32 g, 
    sint32 f, double &new_crime, const BOOL is_dump) 
{ 
    Assert(m_work_min <= w);
    Assert(w <= m_work_max);
    Assert(m_food_min <= f);
    Assert(f <= m_food_max);
    Assert(m_gold_min <= g);
    Assert(g <= m_gold_max);

    double new_happy; 
    double prod_net, food_net, gold_net; 
    double field_workers; 

    CalcHappy (w, g, f, field_workers, new_happy, new_crime);

    CalcProduction(w, field_workers, prod_net);
    CalcGold(ai, g, field_workers, gold_net);
    CalcFood(f, w, field_workers, food_net);

    
    if (is_dump) { 
        FILE *fout=NULL;
        
        if (g_dump) { 
            fout = fopen("logs\\wgf.txt", "a");
            Assert (fout); 
        } else { 
            fout = fopen("logs\\wgf.txt", "w");
            Assert (fout); 
            fprintf (fout, "w, g, f, new_happy, field_workers, prod_net, gold_net, food_net\n");
            g_dump = 1; 
        }

        fprintf (fout, " %d, %d, %d, %f, %f, %f, %f, %f\n", w, g, 
            f, new_happy, field_workers, prod_net, gold_net, food_net); 

        fclose(fout); 
    }

	







	if( 2.5 > fz_desired_wages && fz_desired_wages < 1.5) fz_desired_wages = 2;
	if( 1.5 > fz_desired_wages && fz_desired_wages < 0.5) fz_desired_wages = 1;
	if( 0.5 > fz_desired_wages && fz_desired_wages < -0.5) fz_desired_wages = 0;
	if( -0.5 > fz_desired_wages && fz_desired_wages < -1.5) fz_desired_wages = -1;
	if( -1.5 > fz_desired_wages && fz_desired_wages < -2.5) fz_desired_wages = -2;

	if ( gold_net > 0 && g == fz_desired_wages) gold_net = gold_net * fz_wage_setting_bonus;

    return fz_wgf_prod * prod_net + 
        fz_wgf_food * food_net + 
        fz_wgf_gold * gold_net;
} 



























































