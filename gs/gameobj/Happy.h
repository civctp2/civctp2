









#pragma once
#ifndef __HAPPY_H__
#define __HAPPY_H__

class CityData; 
class CivArchive ;
typedef sint32 PLAYER_INDEX ;
template <class T> class DynamicArray;

#define k_HAPPY_VERSION_MAJOR	0									
#define k_HAPPY_VERSION_MINOR	0									

class Player;
enum HAPPY_REASON;
class HappyTracker;

class HappyTimer {
public:
	sint32 m_turnsRemaining;
	double m_adjustment;
	HAPPY_REASON m_reason;

	HappyTimer(sint32 turns, double adjust,
			   HAPPY_REASON reason)
	{
		m_turnsRemaining = turns;
		m_adjustment = adjust;
		m_reason = reason;
	}
	
	HappyTimer()
	{
		m_turnsRemaining = 0;
		m_adjustment = 0.0;
		m_reason = (HAPPY_REASON)-1; 
	}

	void Serialize(CivArchive &archive);

	void Castrate() {}
	void DelPointers() {}
};

class Happy
	{
	private:
		
        
		double m_happiness;


        double m_last_captured;

        double m_base; 
        double m_size; 
        double m_pollution; 
        double m_conquest_distress;
        double m_empire_dist;
        double m_enemy_action; 
        double m_peace; 
        double m_starvation; 

        double m_workday; 
        double m_wages; 
        double m_rations; 

        double m_martial_law;

        double m_pop_ent; 
        double m_improvement; 
        double m_wonders; 

        double m_dist_to_capitol; 
		sint32 m_cost_to_capitol;
		sint32 m_fullHappinessTurns;
        sint32 m_pad; 
		double m_too_many_cities;
		double m_timed;
        double m_crime; 
		

		
		DynamicArray<HappyTimer> *m_timedChanges;
		

		HappyTracker *m_tracker;

		friend class NetHappy;

	public:
		Happy() ;
		Happy(const Happy *copyme);
		~Happy() ;
		void CopyFrom(const Happy *copyme);
		void SaveTracker();
		void RestoreTracker();

		void Castrate();
        void DelPointers();

        void DecayConquestDistress(); 

		sint32 ShouldRevolt(const sint32 incite_bonus) const ;
        double CalcBase(Player *p);
        double CalcSize(CityData &cd, Player *p);
        double CalcTooManyCities(Player *p); 
        double CalcPollution(CityData &cd, Player *p);
        void ResetConquestDistress(double new_distress);
        double CalcConquestDistress(CityData &cd, Player *p);
        double CalcDistanceFromCapitol(CityData &cd, Player *p);
        double CalcEnemyAction(); 

		
		static double CalcCityIndependentWorkday(Player *player);
		static double CalcCityIndependentWages(Player *player);
		static double CalcCityIndependentRations(Player *player);

        double CalcWorkday(CityData &cd, Player *p);                
        double CalcWages(CityData &cd, Player *p);
        double CalcRations(CityData &cd, Player *p);

        double CalcPeaceMovement(CityData &cd, Player *p);

        double CalcMartialLaw(CityData &cd, Player *p);
        double CalcPopEntertain(CityData &cd, Player *p);
        double CalcImprovementContentment(CityData &cd, Player *p);
        double CalcWonders(CityData &cd, Player *p);
		double CalcTimedChanges(CityData &cd, Player *p, BOOL projectedOnly,
								BOOL isFirstPass);
		double CalcStarvation(CityData &cd, Player *p);
		
        void CalcHappiness(CityData &cd, BOOL projectedOnly,  
						   sint32 &delta_martial_law,
						   BOOL isFirstPass);

        double CalcCrime(CityData &cd, Player *p);

		double CalcFeats(Player *p);

		void AddTimer(sint32 turns, double adjust, 
					  HAPPY_REASON reason);
		void RemoveTimerReason(HAPPY_REASON reason);
		void ClearTimedChanges();

        double GetHappiness() const { return m_happiness; } 
        double GetBase() const { return m_base; }
        double GetSize() const { return m_size; }
        double GetPollution() const { return m_pollution; }
        double GetConquestDistress() const { return m_conquest_distress; }
        double GetEmpireDist() const { return m_empire_dist; }
        double GetDistToCapitol() const { return m_dist_to_capitol; }

		sint32 GetCostToCapitol() const {return m_cost_to_capitol; }
		void SetCostToCapitol(sint32 cost) { m_cost_to_capitol = cost; }
        double GetEnemyAction() const { return m_enemy_action;  }
        double GetPeace() const { return m_peace; }
        double GetWorkday() const { return m_workday; }
        double GetWages() const { return m_wages; }
        double GetRations() const { return m_rations; }
        double GetMartialLaw() const { return m_martial_law; } 
        double GetPopEntertainment() const { return m_pop_ent; }
        double GetImprovement() const { return m_improvement; }
        double GetWonders() const { return m_wonders; }
        double GetCrime() const { return m_crime; }  
		double GetTooManyCities() const { return m_too_many_cities; }
		BOOL IsVeryHappy(void) const ;

        void ResetCrime(CityData *cd, double target_happiness);
        double GetGreedyPopHappiness(CityData &cd); 

		void SetFullHappinessTurns(sint32 turns);
		void Serialize(CivArchive &archive) ;						

		
		void ForceRevolt(void) ;									
        void CountAffectivePop(CityData &cd);
		HappyTracker *GetHappyTracker() { return m_tracker; }
	} ;

uint32 Happy_Happy_GetVersion(void) ;
#else
class Happy ;
#endif
