//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Happiness handling 
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
// - Standardised min/max usage.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "Happy.h"
#include "CivArchive.h"
#include "Player.h"
#include "ConstDB.h"
#include "UnitPool.h"
#include "CityData.h"
#include "SelItem.h"
#include "TurnCnt.h"
#include "MapPoint.h"

#include "XY_Coordinates.h"
#include "World.h"
#include "BuildingRecord.h"
#include "CellUnitList.h"
#include "WonderRecord.h"
#include "HappyTracker.h"
#include "Diffcly.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "SlicSegment.h"
#include "PlayHap.h"
#include "gamefile.h"
#include "buildingutil.h"

#include "UnitData.h"
#include "Cell.h"
#include "GovernmentRecord.h"

#include "wonderutil.h"
#include "FeatTracker.h"

extern SelectedItem g_theSelectedItem; 
extern Player**	g_player ;
extern ConstDB		*g_theConstDB ;
extern TurnCount   *g_turn; 

extern World    *g_theWorld; 
extern UnitPool	*g_theUnitPool ;










Happy::Happy()	{

   m_last_captured = 1000000000.0;
   m_happiness = 0.0 ;
   m_conquest_distress=0.0;
   m_base = 0.0; 
   m_size = 0.0; 
   m_pollution = 0.0; 
   m_starvation = 0.0; 
   m_empire_dist = 0.0;
   m_enemy_action = 0.0; 
   m_peace = 0.0; 
   m_workday=0.0; 
   m_wages=0.0; 
   m_rations=0.0; 
   m_martial_law=0.0;
   m_pop_ent=0.0; 
   m_improvement=0.0; 
   m_wonders=0.0; 
   m_crime=0.0; 
   m_too_many_cities = 0.0; 
   m_dist_to_capitol = 0.0; 
   m_cost_to_capitol = 0;
   m_fullHappinessTurns = 0;
   m_pad = 0; 
   m_timed = 0;

   m_timedChanges = new DynamicArray<HappyTimer>;
   m_tracker = new HappyTracker();
}

Happy::Happy(const Happy *copyme)
{
	CopyFrom(copyme);

	m_timedChanges = new DynamicArray<HappyTimer>;
	m_tracker = new HappyTracker();
}

Happy::~Happy()	{
	if(m_timedChanges) {
		delete m_timedChanges;
		m_timedChanges = NULL;
	}

	if(m_tracker) {
		delete m_tracker;
	}
}


void Happy::Serialize (CivArchive &archive)

{
	sint32 i;

    CHECKSERIALIZE

    if (archive.IsStoring()) {
		archive.StoreChunk((uint8 *)&m_happiness, ((uint8 *)&m_crime)+sizeof(m_crime));

		archive << m_timedChanges->Num();
		for(i = 0; i < m_timedChanges->Num(); i++) {
			m_timedChanges->Access(i).Serialize(archive);
		}
		m_tracker->Serialize(archive);
    } else {
		archive.LoadChunk((uint8 *)&m_happiness, ((uint8 *)&m_crime)+sizeof(m_crime));

		sint32 num;
		archive >> num;
		for(i = 0; i < num; i++) {
			m_timedChanges->ExtendByOne();
			m_timedChanges->Access(i).Serialize(archive);
		}

		
		if (m_tracker)
			delete m_tracker;
		m_tracker = new HappyTracker(archive);
    }
}

void Happy::CopyFrom(const Happy *copyme)
{
	m_last_captured = copyme->m_last_captured;
	m_happiness = copyme->m_happiness;
	m_conquest_distress = copyme->m_conquest_distress;
	m_base = copyme->m_base;
	m_size = copyme->m_size;
	m_pollution = copyme->m_pollution;
    m_starvation = copyme->m_starvation; 
	m_empire_dist = copyme->m_empire_dist;
	m_enemy_action = copyme->m_enemy_action;
	m_peace = copyme->m_peace;
	m_workday = copyme->m_workday;
	m_wages = copyme->m_wages;
	m_rations = copyme->m_rations;
	m_martial_law = copyme->m_martial_law;
	m_pop_ent = copyme->m_pop_ent;
	m_improvement = copyme->m_improvement;
	m_wonders = copyme->m_wonders;
	m_crime = copyme->m_crime;
	m_fullHappinessTurns = copyme->m_fullHappinessTurns;
	m_dist_to_capitol = copyme->m_dist_to_capitol;
}

void Happy::Castrate()
{
	m_timedChanges = NULL;
	m_tracker = NULL;
}

void Happy::DelPointers()
{ 
    if (m_timedChanges) { 
        delete m_timedChanges; 
    } 

	if(m_tracker)
		delete m_tracker;
    m_timedChanges=NULL; 
	m_tracker = NULL;
}

double Happy::CalcBase(Player *p)

{ 
    m_base = p->GetBaseContentment(); 
    return m_base;
}



double Happy::CalcSize(CityData &cd, Player *p)

{  
	m_size	= p->GetBigCityScale() * std::min(p->GetBigCityOffset(), 0.0);
	m_tracker->SetHappiness(HAPPY_REASON_CITY_SIZE, m_size);
    return m_size; 
}

double Happy::CalcTooManyCities(Player *p)
{
    sint32 i;
    sint32 gov_type = p->GetGovernmentType();
   
    double t = double (g_theGovernmentDB->Get(gov_type)->GetTooManyCitiesThreshold()) ; 
    double s = g_theGovernmentDB->Get(gov_type)->GetTooManyCitiesCoefficient(); 
    double n = p->m_all_cities->Num(); 

    
    sint32 num_cities = 0;
    for (i=0; i<n; i++) { 
        Unit *city = &(p->m_all_cities->Access(i));
        sint32 pop;
        if (! g_theUnitPool->IsValid(*city))
            continue;
        
        city->GetPop(pop);
        if (pop < 1)
            continue;

        num_cities++;
    }

	if(num_cities < 1)
		num_cities = 1;

	double res;
    if (num_cities <= t) { 
        res = 0.0; 
    } else { 
        res = -s * (num_cities - t); 

        
        if (g_slicEngine->GetSegment("28IAMaxCitiesReached")->TestLastShown(p->m_owner, 10)) {
            SlicObject *so = new SlicObject("28IAMaxCitiesReached") ;
            so->AddRecipient(p->m_owner) ;
            g_slicEngine->Execute(so) ;
        }
    }
	m_tracker->SetHappiness(HAPPY_REASON_NUM_CITIES, res);
	m_too_many_cities = res;

	return m_too_many_cities;
}

void Happy::ResetConquestDistress(double new_distress)

{
    m_conquest_distress = new_distress; 
}


double Happy::CalcConquestDistress(CityData &cd, Player *p)
{

    m_conquest_distress -= m_conquest_distress * p->GetConquestDistressDecay();
	m_tracker->SetHappiness(HAPPY_REASON_CONQUEST_DISTRESS, m_conquest_distress);
    return m_conquest_distress; 
}

double Happy::CalcDistanceFromCapitol(CityData &cd, Player *p)

{

    double cost = 0.0; 
    MapPoint start, dest; 
    BOOL is_cap = cd.IsCapitol();

    if (is_cap || p->GetCapitolPos(start) || p->m_first_city) { 
        cd.m_home_city.GetPos(dest); 
		if(is_cap || p->m_first_city) {
			m_empire_dist = 0.0;
            m_dist_to_capitol = 0.0; 
			m_tracker->SetHappiness(HAPPY_REASON_DISTANCE_FROM_CAPITOL,
				m_empire_dist);
			return 0;
		}
		
		cost = m_cost_to_capitol;

        
		cost += p->m_difficulty->GetDistanceFromCapitolAdjustment();

        cost = min(cost, p->GetMaxEmpireDistance()); 

    } else { 
         cost = p->GetMaxEmpireDistance();
    }

	
    m_dist_to_capitol = max(0.0, cost - g_theGovernmentDB->Get(p->GetGovernmentType())->GetMinEmpireDistance()); 


    m_empire_dist = -p->GetEmpireDistanceScale() * m_dist_to_capitol;
	sint32 wonderDistanceModifier = wonderutil_GetDecreaseEmpireSize(p->m_builtWonders);
	if(wonderDistanceModifier > 0) {
		m_empire_dist -= m_empire_dist * double(double(wonderDistanceModifier) / 100.0);
	}

	m_tracker->SetHappiness(HAPPY_REASON_DISTANCE_FROM_CAPITOL,
							m_empire_dist);
    return m_empire_dist;
}

double Happy::CalcEnemyAction() { 
 
    m_enemy_action = 0.0; 
	m_tracker->SetHappiness(HAPPY_REASON_ENEMY_ACTION,
							m_enemy_action);
    return m_enemy_action; 
}

double Happy::CalcPeaceMovement(CityData &cd, Player *p)

{ 
    double overseas_defeat, home_defeat, overseas; 
    double prev_peace = m_peace;

    p->GetPeaceMovement(overseas_defeat, home_defeat, overseas);
	
	sint32 numCities = p->m_all_cities->Num();
	if(numCities < 1) 
		numCities = 1;
	
	overseas_defeat /= numCities;
	home_defeat /= numCities;
	overseas /= numCities;

	m_peace = -overseas_defeat + home_defeat - overseas;
	

    
    if (((m_peace <= -1.0) && (prev_peace > -1.0)) || (m_peace <= -2.0)) {
        if (g_slicEngine->GetSegment("27IAWarDiscontentRising")->TestLastShown(p->m_owner, 10)) {
            SlicObject *so = new SlicObject("27IAWarDiscontentRising") ;
            so->AddRecipient(p->m_owner) ;
            so->AddCivilisation(p->m_owner) ;
            g_slicEngine->Execute(so) ;
        }
    }

	m_peace -= p->m_global_happiness->GetWarDiscontent();

	m_peace += m_peace * buildingutil_GetLowerPeaceMovement(cd.GetEffectiveBuildings());

	m_tracker->SetHappiness(HAPPY_REASON_PEACE_MOVEMENT,
							m_peace);
	return m_peace;
}

double Happy::CalcPollution(CityData &cd, Player *p)
{
	if(wonderutil_GetNoPollutionUnhappiness(p->m_builtWonders)) {
		m_pollution = 0;
	} else {
		m_pollution = - p->GetPollutionUnhappyCoef() * cd.m_total_pollution; 
	}
	m_tracker->SetHappiness(HAPPY_REASON_POLLUTION,
							m_pollution);
    return m_pollution;
}


double Happy::CalcCityIndependentWorkday(Player *player)
{
	sint32 delta = (player->GetUnitlessWorkday() - 
					player->GetWorkdayExpectation());
	if(delta > 0)
		return(delta * player->GetPositiveWorkdayCoef());
	else
		return(delta * player->GetNegativeWorkdayCoef());
}

double Happy::CalcCityIndependentWages(Player *player)
{
	sint32 delta = player->GetUnitlessWages() - player->GetWagesExpectation();
	if(delta > 0)
		return(player->GetPositiveWagesCoef() * delta);
	else
		return(player->GetNegativeWagesCoef() * delta);
}

double Happy::CalcCityIndependentRations(Player *player)
{
	sint32 delta = player->GetUnitlessRations() - player->GetRationsExpectation();
	if(delta > 0)
		return(player->GetPositiveRationsCoef() * delta);
	else
		return(player->GetNegativeRationsCoef() * delta);
}

double Happy::CalcWorkday(CityData &cd, Player *p)
{
	m_workday = CalcCityIndependentWorkday(p);

	m_tracker->SetHappiness(HAPPY_REASON_WORKDAY, m_workday); 
    return m_workday; 
}


double Happy::CalcWages(CityData &cd, Player *p)
{
	m_wages = CalcCityIndependentWages(p);

	m_tracker->SetHappiness(HAPPY_REASON_WAGES, m_wages); 
    return m_wages; 
}

double Happy::CalcRations(CityData &cd, Player *p)
{
	m_rations = CalcCityIndependentRations(p);

	m_tracker->SetHappiness(HAPPY_REASON_RATIONS, m_rations); 
    return m_rations;
}

double Happy::CalcMartialLaw(CityData &cd, Player *p)

{
	
    sint32 mu = p->GetMaxMartialLawUnits();

    if (mu < 1) { 
        m_martial_law = 0.0; 
    } else { 
        double me = p->GetMartialLawEffect();
        MapPoint pos; 

        cd.m_home_city.GetPos(pos); 
        CellUnitList *a = g_theWorld->GetArmyPtr(pos); 

		
		
		sint32 n; 
		sint32 i;
		sint32 count = 0; 
		
		if(a) {
			n = a->Num(); 
			for (i=0; i<n; i++) { 
				if ((*a)[i].ExertsMartialLaw()) { 
					count++; 
				} 
			}
		}        
        m_martial_law = min (mu, count) * me; 
    }
    m_tracker->SetHappiness(HAPPY_REASON_MARTIAL_LAW, m_martial_law);
    return m_martial_law; 
}




double Happy::CalcPopEntertain(CityData &cd, Player *p)
{

    sint32 n = cd.m_population;

	double increaseSpecialists = double(wonderutil_GetIncreaseSpecialists(p->m_builtWonders));
	
    m_pop_ent = cd.GetHappinessFromPops(); 
	m_tracker->SetHappiness(HAPPY_REASON_ENTERTAINERS, m_pop_ent);
    return m_pop_ent; 
}
    

double Happy::CalcImprovementContentment(CityData &cd, Player *p)
{
    sint32 b; 
    
    m_improvement = 0.0; 

    buildingutil_GetHappinessIncrement(cd.GetEffectiveBuildings(), b, cd.m_owner); 
    m_improvement += b;

    
    

	m_tracker->SetHappiness(HAPPY_REASON_BUILDINGS, m_improvement);
    return m_improvement;
}

double Happy::CalcWonders(CityData &cd, Player *p)

{
	m_wonders = 0.0;

	
	
	m_wonders += wonderutil_GetIncreaseHappinessEmpire(p->GetBuiltWonders());

	m_tracker->SetHappiness(HAPPY_REASON_WONDERS, m_wonders);
	return m_wonders;
} 


double Happy::CalcCrime(CityData &cd, Player *p)

{ 
    double threshold = p->GetCrimeOffset();

    if (m_happiness > threshold) { 
        m_crime = 0.0; 
    } else { 
        double base_crime = threshold - m_happiness; 
        double cops = cd.GetImprovementCrimeMod() - 
					  (double)(wonderutil_GetDecreaseCrimePercentage(p->GetBuiltWonders()) / 100.0);
		double total_crime = 0.01 * base_crime;
		total_crime += cops * total_crime;
		total_crime *= p->GetCrimeCoef();
		m_crime = min(1.0, total_crime);
		if(m_crime < 0)
			m_crime = 0;
    }

	
	m_tracker->SetHappiness(HAPPY_REASON_CRIME, 0); 
    return m_crime;
}
 
double Happy::CalcTimedChanges(CityData &cd, Player *p, BOOL projectedOnly,
							   BOOL isFirstPass)
{
	sint32 i, n = m_timedChanges->Num();
	DynamicArray<HappyTimer> dead;
	double ret = 0.0;

	if(isFirstPass && !projectedOnly) {
		m_tracker->ClearEntries(HAPPY_REASON_HAPPINESS_ATTACK,
								HAPPY_REASON_MAX);
		for(i = n-1; i >= 0; i--) {
			double adj = (*m_timedChanges)[i].m_adjustment;
			ret += adj;
			
			if(isFirstPass && !projectedOnly) {
				m_tracker->AddTimedChange((*m_timedChanges)[i].m_reason, adj);
				if(--((*m_timedChanges)[i].m_turnsRemaining) <= 0) {
					m_timedChanges->DelIndex(i);
				}
			}
		}
		m_timed = ret;
	}
	return m_timed;
}

double Happy::CalcStarvation(CityData &cd, Player *p)
{
	
	
	
	
    
	    m_tracker->SetHappiness(HAPPY_REASON_STARVATION, 0.0);
	    m_starvation = 0.0;
    

    return m_starvation; 
}


void Happy::CountAffectivePop(CityData &cd)
{ 
} 

void Happy::CalcHappiness(CityData &cd, BOOL projectedOnly, 
						  sint32 &delta_martial_law, 
						  BOOL isFirstPass)

{
    
    Player *p = g_player[cd.m_owner]; 

    CountAffectivePop(cd);

    
    m_happiness = CalcBase(p); 

	
    if(cd.m_owner == PLAYER_INDEX_VANDALS) {
		return;
	}

	
	

	CalcTimedChanges(cd, p, projectedOnly, isFirstPass);

	if(m_fullHappinessTurns > 0) {
		m_happiness = 100;
		if(isFirstPass)
			m_fullHappinessTurns--;
		m_tracker->SetHappiness(HAPPY_REASON_WONDER_TURNS_REMAINING, m_fullHappinessTurns);
		return;
	}

	if(wonderutil_GetAllCitizensContent(p->m_builtWonders)) {
		
		return;
	}

	if(buildingutil_GetNoUnhappyPeople(cd.GetEffectiveBuildings())) {
		
		return;
	}
    m_happiness += CalcSize(cd, p);
	if(isFirstPass) {
		m_happiness += CalcTooManyCities(p); 
	} else {
		m_happiness += m_too_many_cities;
	}

	if(isFirstPass) {
		m_happiness += CalcConquestDistress(cd, p);
	} else {
		m_happiness += m_conquest_distress;
	}


	if(isFirstPass) {		
		m_happiness += CalcDistanceFromCapitol(cd, p);
	} else {
		m_happiness += m_empire_dist;
	}

    m_happiness += CalcEnemyAction();       
    m_happiness += CalcPollution(cd, p); 

	m_happiness -= CalcStarvation(cd, p);

    
	m_happiness += CalcPeaceMovement(cd, p);

	CalcWorkday(cd, p);
	CalcWages(cd, p);
	CalcRations(cd, p);

    if(m_workday + m_wages + m_rations > 0) {
		m_happiness += 2;
		sint32 howManyContribute = 0;
		if(m_workday > 0)
			howManyContribute++;
		if(m_wages > 0)
			howManyContribute++;
		if(m_rations > 0)
			howManyContribute++;

		double amountPer = 2 * (1 / howManyContribute);
		if(m_workday > 0) {
			m_tracker->SetHappiness(HAPPY_REASON_WORKDAY, amountPer);
		} else {
			m_tracker->SetHappiness(HAPPY_REASON_WORKDAY, 0);
		}

		if(m_wages > 0) {
			m_tracker->SetHappiness(HAPPY_REASON_WAGES, amountPer);
		} else {
			m_tracker->SetHappiness(HAPPY_REASON_WAGES, 0);
		}

		if(m_rations > 0) {
			m_tracker->SetHappiness(HAPPY_REASON_RATIONS, amountPer);
		} else {
			m_tracker->SetHappiness(HAPPY_REASON_RATIONS, 0);
		}
	} else {
		
		m_happiness += m_workday;
		m_happiness += m_wages;
		m_happiness += m_rations;
	}

    
	m_happiness += p->GetTimedHappiness();

	
	
	m_happiness += m_timed;
 
    m_tracker->SetHappiness(HAPPY_REASON_ASSASSINATION, p->GetTimedHappiness());

    
    
    
    
    double const		mlt = static_cast<double>(p->GetMartialLawThreshold());
    if (m_happiness < mlt) 
	{ 
        double const	new_happiness = 
			std::min(CalcMartialLaw(cd, p) + m_happiness,  mlt); 
        delta_martial_law = sint32 (new_happiness - m_happiness + 0.5); 
        m_happiness = new_happiness; 

    } else { 
        delta_martial_law = 0; 
    } 
	
    
    m_happiness += CalcPopEntertain(cd, p) + 
        CalcImprovementContentment(cd, p) +  
        CalcWonders(cd, p);

	m_happiness += CalcFeats(p);

    CalcCrime(cd, p); 

	sint32 intHap = (sint32)m_happiness;
	sint32 newHappiness = g_slicEngine->CallMod(mod_CityHappiness, intHap, cd.GetHomeCity(), intHap);
	if(intHap != newHappiness)
		m_happiness = newHappiness;



  
    
       
         
       
   
	
}

void Happy::ResetCrime(CityData *cd, double target_happiness) 
{ 
    Player *p = g_player[cd->m_owner]; 
    m_happiness = target_happiness; 
    CalcCrime(*cd, p); 
}



double Happy::GetGreedyPopHappiness(CityData &cd)
{
    
    Player *p = g_player[cd.m_owner]; 

    
    double local_happiness = m_base; 



















	
    if(cd.m_owner == PLAYER_INDEX_VANDALS) {
		return local_happiness;
	}

    local_happiness += m_size;
	local_happiness += m_too_many_cities;
	local_happiness += m_empire_dist;
    local_happiness += m_pollution; 
	local_happiness -= m_starvation;

    
	local_happiness += m_peace;


    CalcWorkday(cd, p);                
    CalcWages(cd, p);
    CalcRations(cd, p);

    if(m_workday + m_wages + m_rations > 0) {
		local_happiness += 2;
	} else {
	}

    
    
    
    
    
	double const	mlt	= static_cast<double>(p->GetMartialLawThreshold());
	if (local_happiness < mlt)
	{
		local_happiness	= 
			std::min(CalcMartialLaw(cd, p) + local_happiness, mlt);
	}
    
    local_happiness += 
        CalcImprovementContentment(cd, p) +  
        CalcWonders(cd, p);

    

    return local_happiness; 

}



void Happy::AddTimer(sint32 turns, double adjust, HAPPY_REASON reason)
{
	HappyTimer timer(turns, adjust, reason);
	
	m_timedChanges->Insert(timer);
}

void Happy::RemoveTimerReason(HAPPY_REASON reason)
{
	sint32 i;
	for(i = m_timedChanges->Num() - 1; i >= 0; i--) {
		if(m_timedChanges->Access(i).m_reason == reason) {
			m_timedChanges->DelIndex(i);
		}
	}
}

void Happy::ClearTimedChanges()
{
	while(m_timedChanges->Num() > 0) {
		m_timedChanges->DelIndex(0);
	}

	m_tracker->ClearEntries(HAPPY_REASON_HAPPINESS_ATTACK,
							HAPPY_REASON_MAX);

	m_timed = 0;
}










void HappyTimer::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive << m_turnsRemaining;
		archive << m_adjustment;
		
		
		archive.PutSINT32(m_reason);
	} else {
		archive >> m_turnsRemaining;
		archive >> m_adjustment;

		m_reason = (HAPPY_REASON)archive.GetSINT32();

	}
}





















sint32 Happy::ShouldRevolt(const sint32 incite_bonus) const
	{
	if (m_happiness<(g_theConstDB->GetRevolutionLevel()+incite_bonus))
		return (TRUE) ;

	return (FALSE) ;
	}













BOOL Happy::IsVeryHappy(void) const
	{
	return (m_happiness >= g_theConstDB->GetVeryHappyThreshold()) ;
	}












void Happy::ForceRevolt(void)
	{
	m_happiness = g_theConstDB->GetRevolutionLevel()-10 ;
	}


void Happy::SetFullHappinessTurns(sint32 turns)
{
	m_fullHappinessTurns = turns;
	m_happiness = 100;
	m_tracker->ClearEntries(HAPPY_REASON_SMOKING_CRACK,
							HAPPY_REASON_MAX);
}













uint32 Happy_Happy_GetVersion(void)
	{
	return (k_HAPPY_VERSION_MAJOR<<16 | k_HAPPY_VERSION_MINOR) ;
	}

void Happy::SaveTracker()
{
	m_tracker->SaveHappiness();
}

void Happy::RestoreTracker()
{
	m_tracker->RestoreHappiness();
}

double Happy::CalcFeats(Player *p)
{
	double res = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_HAPPINESS, p->m_owner);
	m_tracker->SetHappiness(HAPPY_REASON_FEATS, res);
	return res;
}
