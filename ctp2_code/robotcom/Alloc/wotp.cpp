
#include "c3.h"

#include "MapPoint.h"
#include "CivArchive.h"
#include "FlatPtr.h"


#include "IC3GameState.h"
#include "IC3Government.h"
#include "IC3Science.h"
#include "IC3Player.h"

#include "AiMain.h"
#include "gold.h"
#include "AllocWGF.h"
#include "CityAgent.h"


#include "ScienceAgent.h"


#include "FzOut.h"
#include "WotP.h"
#include "ArmyAgent.h"

#define k_MIN_UTILITY -1000000.0


#define k_AVE_CITY_SIZE 10.0




extern double fz_gov_happy_workday; 
extern double fz_gov_happy_wage; 
extern double fz_gov_happy_rations; 
extern double fz_gov_happy_size;
extern double fz_gov_happy_martial;        
extern double fz_gov_happy_unitdist;
extern double fz_gov_happy_combat;
extern double fz_gov_pollution; 
extern double fz_gov_crime;
extern double fz_gov_gold; 
extern double fz_gov_production;  
extern double fz_gov_science;
extern double fz_gov_readiness; 
extern double fz_gov_number_city_growth;
extern double fz_i_want_this_gov[FZ_GOVERNMENT_MAX]; 
extern double fz_i_want_this_gov_uscale;
extern double fz_new_gov_discount;








ZEROMEM(WillOfThePeopleFlat); 
ZEROMEM(WillOfThePeoplePtr); 
ZERODEL(WillOfThePeoplePtr); 
FLATSERIALIZE(WillOfThePeopleFlat); 
PTRSERIALIZE(WillOfThePeoplePtr); 

WillOfThePeople::WillOfThePeople(AiMain *ai)
{ 
    m_num_gov = ai->m_governmentDB->GetNumGovs(); 
    
    m_is_enabled = new BOOL[m_num_gov];
    m_utility = new double[m_num_gov]; 
    sint32 gov_idx; 
    sint32 enabling_sci; 
    for (gov_idx=0; gov_idx <m_num_gov; gov_idx++) { 
        enabling_sci = ai->m_governmentDB->EnablingDiscovery(gov_idx);
        if (ai->m_science->HasAdvance(enabling_sci)) { 
            m_is_enabled[gov_idx] = TRUE; 
        } else { 
            m_is_enabled[gov_idx] = FALSE; 
        }
        m_utility[gov_idx] = 0.0; 
    } 
 
    m_current_gov = ai->m_player->GetGovernmentType(); 
    Assert(TRUE == m_is_enabled[m_current_gov]); 
    Assert(0 <= m_current_gov); 
    Assert(m_current_gov <m_num_gov); 
    m_utility[m_current_gov] = 1.0; 
	m_rank = new sint8 [m_num_gov];
}

WillOfThePeople::WillOfThePeople(CivArchive &archive)
{ 
    Serialize(archive); 
} 

WillOfThePeople::~WillOfThePeople()
{ 
    delete[] m_is_enabled; 
    m_is_enabled = NULL;
    delete[] m_utility; 
    m_utility=NULL; 
	delete [] m_rank;
	m_rank=NULL;
} 

void WillOfThePeoplePtr::Store (CivArchive &archive)
{
    archive << m_num_gov; 
    archive.Store((uint8*)m_is_enabled, sizeof(BOOL) * m_num_gov); 
    archive.Store((uint8*)m_utility, sizeof(double) * m_num_gov); 
}


void WillOfThePeoplePtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
    archive >> m_num_gov; 
    m_is_enabled = new BOOL[m_num_gov];
    archive.Load((uint8*)m_is_enabled, sizeof(BOOL) * m_num_gov); 

    m_utility = new double[m_num_gov]; 
    archive.Load((uint8*)m_utility, sizeof(double) * m_num_gov); 
}

void WillOfThePeople::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE 

    WillOfThePeopleFlat::Serialize(archive); 
    WillOfThePeoplePtr::Serialize(archive); 

	
    m_rank = new sint8[m_num_gov]; 
}



double WillOfThePeople::UnhappyGovWorkday(AiMain *ai, const sint32 gov)
{
    double work_coef = ai->m_governmentDB->GetWorkdayCoef(gov);
    double work_expt = ai->m_governmentDB->GetWorkdayExpectation(gov); 

    
    return fz_gov_happy_workday * m_ave_city_size * work_coef * (work_expt + 2.0); 
}

double WillOfThePeople::UnhappyGovWages(AiMain *ai, const sint32 gov) 
{
    double wage_coef = ai->m_governmentDB->GetWagesCoef(gov);
    double wage_expt = ai->m_governmentDB->GetWagesExpectation(gov); 

    
    return fz_gov_happy_wage * m_ave_city_size * wage_coef * (wage_expt + 2.0); 
}

double WillOfThePeople::UnhappyGovRations(AiMain *ai, const sint32 gov)  
{
    double ration_coef = ai->m_governmentDB->GetRationsCoef(gov);
    double ration_expt = ai->m_governmentDB->GetRationsExpectation(gov); 

    
    return fz_gov_happy_rations * m_ave_city_size * ration_coef * (ration_expt + 2.0); 
}


double WillOfThePeople::UnhappyGovEmpSize(AiMain *ai, const sint32 gov)  
{
   double scale = ai->m_governmentDB->GetEmpireDistanceScale(gov);
   double dist_cost; 

   if (ai->m_capitol_id == NULL) { 
       dist_cost = ai->m_governmentDB->GetMaxEmpireDistance(gov);
   } else { 
       dist_cost = m_ave_dist_to_capitol; 
   } 
     
   dist_cost = max(0.0, dist_cost - ai->m_governmentDB->GetMinEmpireDistance(gov));

   return fz_gov_happy_size * scale * dist_cost; 
}

double WillOfThePeople::UnhappyGovTooManyCities(AiMain *ai, const sint32 gov)  
{
    sint32 t = ai->m_governmentDB->GetTooManyCitiesThreshold(gov);
    double want_this_many = m_cities_num + m_cities_num * fz_gov_number_city_growth; 
    double diff = want_this_many - t; 

    if (diff < 0.0) { 
        return 0.0; 
    } else { 
        return diff * ai->m_governmentDB->GetTooManyCitiesCoefficient(gov);
    }
}

double WillOfThePeople::UtilityGovTooManyCities(AiMain *ai, const sint32 gov)  
{
    sint32 t = ai->m_governmentDB->GetTooManyCitiesThreshold(gov);
    double diff = t - m_cities_num; 

    if (diff < 0.0) { 
        return 2000.0 * diff; 
    } else { 
        return 0.0;
    }
}

void WillOfThePeople::HappyMartialLaw(AiMain *ai, const sint32 gov, 
   double &happy)
{ 
    
    double t = ai->m_governmentDB->GetMartialLawThreshold(gov); 

    if (happy < t) { 
        double e = ai->m_governmentDB->GetMartialLawEffect(gov);
        double u = ai->m_governmentDB->GetMaxMartialUnits(gov);

        min (t, fz_gov_happy_martial * e * u + happy);        
    }
} 

double WillOfThePeople::UnhappyUnitDist(AiMain *ai,const sint32 gov)
{
    double d = ai->m_governmentDB->GetAtHomeRadius(gov); 
    double o = ai->m_governmentDB->GetOverseasCoef(gov); 

    return fz_gov_happy_unitdist * o * m_units_overseas;
}

double WillOfThePeople::UnhappyCombatMoral(AiMain *ai,const sint32 gov)
{
    double o = ai->m_governmentDB->GetOverseasDefeatCoef(gov); 
    double h = ai->m_governmentDB->GetHomeDefeatCoef(gov);

    return fz_gov_happy_combat * (m_losses_overseas * o - m_losses_home * h);
} 

double WillOfThePeople::UnhappyPolluion(AiMain *ai,const sint32 gov)
{
    double p = ai->m_governmentDB->GetPollutionCoef(gov); 

    return fz_gov_pollution * p * m_ave_city_pollution; 
}

double WillOfThePeople::GovCrime(AiMain *ai,
        const sint32 gov, const double happy)
{
    double o = ai->m_governmentDB->GetCrimeOffset(gov);
    double m = ai->m_governmentDB->GetCrimeCoef(gov); 

    double c = max(0.0, (o - happy));
    return fz_gov_crime * c * m * 0.01;
}

double WillOfThePeople::GovGold(AiMain *ai,
    const sint32 gov, const double happy, const double crime)
{ 
    
    double i = ai->m_gold->GetGrossIncome(); 
    double current_coef = ai->m_governmentDB->GetGoldCoefficient(m_current_gov);
    Assert(0.0 < current_coef) ; 
    double raw_income = i / current_coef; 

    raw_income -= raw_income * crime; 
    raw_income *= ai->m_governmentDB->GetGoldCoefficient(gov);

    return fz_gov_gold * raw_income; 
} 

double WillOfThePeople::UtilityGovProduction(AiMain *ai,
   const sint32 gov, const double crime) 
{
    double p = m_ave_city_production; 
    double current_coef = ai->m_governmentDB->GetProductionCoefficient(m_current_gov); 
    Assert(0.0 < current_coef); 
    double raw_prod = p / current_coef; 

    raw_prod -= raw_prod * crime; 
    raw_prod *= ai->m_governmentDB->GetProductionCoefficient(gov); 

    return fz_gov_production * raw_prod; 
}

double WillOfThePeople::UtilityGovKnowlege(AiMain *ai,
   const sint32 gov, const double gold) 
{
    double s = ai->m_governmentDB->GetKnowledgeCoef(gov); 
    if (s <= 0.0) { 
        s = 0.0001; 
    } 

    

    
    

    double spend_at_least_this =  ai->m_governmentDB->GetMaxScienceRate(gov) * ai->m_alloc_WGF->Gold2Prod(m_desired_science); 
    double gold_cost = (m_desired_science - m_total_science_from_city * s) / s; 
    double prod_cost = ai->m_alloc_WGF->Gold2Prod(gold_cost); 

    return fz_gov_science * (spend_at_least_this - prod_cost);
}


double WillOfThePeople::UtilityGovReadiness(AiMain *ai,
   const sint32 gov, const double crime) 
{









    double np = ai->m_governmentDB->GetProfessionalUnits(gov); 
    double r = ai->m_governmentDB->GetReadyAlertCoef(gov);
    
    double net_units = max(0.0, m_num_units - np); 
    return fz_gov_readiness * (-r * net_units); 
}

double WillOfThePeople::UtilityIWantGov(AiMain *ai, sint32 idx_gov)
{
	

	
    

	
	
	if (idx_gov >= FZ_GOVERNMENT_MAX ||
		fz_i_want_this_gov[idx_gov] > 0.0)
	{ 
        return fz_i_want_this_gov_uscale;
    } else { 
        return 1.0; 
    }
}








double WillOfThePeople::UtilityGoverment(AiMain *ai, sint32 idx_gov)
{
	sint32	numGovs;

    double happy; 
    double crime; 
    double gold; 
	double utility;

	numGovs = ai->m_governmentDB->GetNumGovs();
    
    utility = 0.0;

        
	
    happy = 0.0; 
    crime = 0.0; 
    gold = 0.0; 

    
    happy = 100.0;
    
    happy -= UnhappyGovWorkday(ai, idx_gov); 
    happy -= UnhappyGovWages(ai, idx_gov);  
    happy -= UnhappyGovRations(ai, idx_gov); 
    happy -= UnhappyGovEmpSize(ai, idx_gov);  
    happy -= UnhappyUnitDist(ai, idx_gov);
    happy -= UnhappyCombatMoral(ai, idx_gov);
    happy -= UnhappyPolluion(ai, idx_gov);
    happy -= UnhappyGovTooManyCities(ai, idx_gov);  

    if (happy < 50.0) 
        happy = 50.0; 

    
    HappyMartialLaw(ai, idx_gov, happy);

    
    crime = GovCrime(ai, idx_gov, happy);
    gold =  GovGold(ai, idx_gov, happy, crime);

	utility = 0.0;

    if (idx_gov==0) { 
        utility -= 10000.0;
    } 
    
    utility += UtilityGovTooManyCities(ai, idx_gov);
    utility += UtilityGovProduction(ai, idx_gov, crime); 
	utility += UtilityGovKnowlege(ai, idx_gov, gold);    
	utility += UtilityGovReadiness(ai, idx_gov, crime);

    utility += UtilityIWantGov(ai, idx_gov); 

    return utility; 
}

void WillOfThePeople::SetStats(AiMain *ai)
{

  
    CityAgent *ca; 
    BSetID id; 

    if (ai->m_capitol_id == NULL) { 
        m_ave_dist_to_capitol = 1000.0;
    } 
    
    BOOL is_unknown_id=FALSE; 
    double poll_sum=0.0;
    double prod_sum=0.0;
    double dist_sum = 0.0; 
    m_cities_num = 0.0; 
    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) { 
        m_cities_num += 1.0; 
        poll_sum += ca->GetTotalPollution(); 
        prod_sum += ca->GetGrossProduction(); 
        dist_sum += ai->m_player->GetCityDistFromCapitol(id.GetVal(), &is_unknown_id); 
        Assert(is_unknown_id == FALSE); 
    } 

    if (m_cities_num < 1.0) { 
        m_ave_dist_to_capitol = 0.0; 
        m_ave_city_size = 1.0; 
        m_units_overseas = 0.0;
        m_losses_overseas = 0.0;
        m_losses_home = 0.0;
        m_ave_city_pollution = 1.0;
        m_ave_city_production = 5.0;
        m_total_science_from_city = 5.0;

    } else { 

        m_ave_dist_to_capitol = dist_sum / m_cities_num;
        m_ave_city_size = max(1.0, ai->m_alloc_WGF->GetAveAffectPop());
        
        m_ave_city_pollution = poll_sum / m_cities_num; 
        m_ave_city_production = max(5.0, prod_sum / m_cities_num);  

        
        ai->m_player->GetPeaceMovement (&m_losses_overseas, &m_losses_home,
            &m_units_overseas);

       
        
        double c;
        c = ai->m_governmentDB->GetOverseasDefeatCoef(m_current_gov); 
        if (0.0 < c) { 
            m_losses_overseas =  m_losses_overseas/c;
        }
        c = ai->m_governmentDB->GetHomeDefeatCoef(m_current_gov); 
        if (0.0 < c) { 
            m_losses_home = m_losses_home/c; 
        } 

        c = ai->m_governmentDB->GetOverseasCoef(m_current_gov); 
        if (0.0 < c) { 
            m_units_overseas = m_units_overseas / c; 
        }         
    }

    ArmyAgent *aa=NULL;
    m_num_units = 0.0;
    for (aa = ai->m_army_set->First(id); ai->m_army_set->Last(); aa = ai->m_army_set->Next(id)) { 
        m_num_units += aa->GetNUnits();         
    } 
    
    m_desired_science = max(10.0, ai->m_gold->GetPercentInScience() *
        ai->m_gold->GetProjectedGrossIncome()); 

}

#define k_MAX_GOVERNMENT_RANK 99; 

void WillOfThePeople::PollNewGov(AiMain *ai)
{ 
    sint32 idx_gov; 
    sint32 util_g_max = -1; 
    sint32 rank_g_max = -1;
    double util_max = -1;
	sint8 rank_max = k_MAX_GOVERNMENT_RANK; 
	sint8 rank;
    BOOL searching = TRUE; 
    SetStats(ai);

     
     
    m_current_gov = ai->m_player->GetGovernmentType();

    for (idx_gov=1; idx_gov<m_num_gov; idx_gov++) { 
		rank = RankGovernment(ai, idx_gov);
        if (m_is_enabled[idx_gov] && rank > 0) { 
            m_utility[idx_gov] = UtilityGoverment(ai, idx_gov);
			m_rank[idx_gov] = rank;
        } else  { 
            m_utility[idx_gov] = k_MIN_UTILITY;  
			m_rank[idx_gov] = k_MAX_GOVERNMENT_RANK ;
        } 
        
        
        
        

		
		
		if (m_rank[idx_gov] < rank_max ||
			(idx_gov >= FZ_GOVERNMENT_MAX &&
			 m_utility[idx_gov] > util_max))
			{
				searching = FALSE; 

				
				rank_max = m_rank[idx_gov];
				rank_g_max = idx_gov; 

				
				util_max = m_utility[idx_gov]; 
				util_g_max = idx_gov;
			}
    } 

    if (searching) { 
        

        #ifdef _DEBUG
        
		for (idx_gov=1; idx_gov<m_num_gov; idx_gov++) { 
			rank = RankGovernment(ai, idx_gov);
			if (m_is_enabled[idx_gov] && rank > 0) { 
				m_utility[idx_gov] = UtilityGoverment(ai, idx_gov);
				m_rank[idx_gov] = rank;
			} else  { 
				m_utility[idx_gov] = k_MIN_UTILITY;  
				m_rank[idx_gov] = k_MAX_GOVERNMENT_RANK;
			} 

			if (util_max < m_utility[idx_gov]) { 
				util_max = m_utility[idx_gov]; 
				util_g_max = idx_gov; 
			}

			if (m_rank[idx_gov] < rank_max)
			{
				searching = FALSE; 
				rank_max = m_rank[idx_gov];
				rank_g_max = idx_gov; 
			}
		}
        #endif

		return; 
    }

    sint32 r;

    
    
    
    if (rank_g_max != m_current_gov)
		{
			Assert(rank_g_max != 0); 
			r = ai->m_player->SetGovernmentType(rank_g_max);
			Assert(r); 
			m_current_gov = ai->m_player->GetGovernmentType(); 
		}
}

void WillOfThePeople::RegisterLearnedScience(AiMain *ai, const sint32 idx_sci) 
{ 
    sint32 idx_gov; 
    sint32 enabling;

    for (idx_gov=0; idx_gov<m_num_gov; idx_gov++) { 
        enabling = ai->m_governmentDB->EnablingDiscovery(idx_gov);
        if ((enabling == idx_sci) || (enabling == -1)){ 
            m_is_enabled[idx_gov] = TRUE;                 
        } 
    }
} 

sint8 WillOfThePeople::RankGovernment(AiMain *ai, sint32 idx_gov)
{

	sint8 rank; 

	
	if (idx_gov >= FZ_GOVERNMENT_MAX)
		rank = 1;
	else
		
		rank = (sint8) FZ_ROUND(fz_i_want_this_gov[idx_gov]);

	if (rank > 0)
		{
			
			if (m_cities_num  >=
				ai->m_governmentDB->GetTooManyCitiesThreshold(idx_gov))
				return k_MAX_GOVERNMENT_RANK;

			
			double new_gold_coef = 
				ai->m_governmentDB->GetGoldCoefficient(idx_gov);

			double old_gold_coef = 
				ai->m_governmentDB->GetGoldCoefficient(m_current_gov);
			
			
			double raw_income = 
				ai->m_gold->GetGrossIncome() / old_gold_coef;

			
			double obligations = 
				ai->m_gold->GetLostToCleric() +
				ai->m_gold->GetLostToCrime() +
				ai->m_gold->GetOldMaintenance() +
				ai->m_gold->GetOldWages();

			
			
			if ((new_gold_coef < old_gold_coef) &&
				(new_gold_coef * raw_income < obligations))
				return k_MAX_GOVERNMENT_RANK;
		}

	
	return rank;
}

