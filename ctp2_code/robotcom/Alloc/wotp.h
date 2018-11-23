#ifndef __WILLOFTHEPEOPLE_H__
#define __WILLOFTHEPEOPLE_H__ 1

class CivArchive;

#include "FzOut.h"

class WillOfThePeopleFlat{
protected:

    sint32 m_current_gov;

    double m_ave_city_size;
    double m_ave_dist_to_capitol;
    double m_units_overseas;
    double m_losses_overseas;
    double m_losses_home;
    double m_ave_city_pollution ;
    double m_ave_city_production;
    double m_desired_science;
    double m_total_science_from_city;
    double m_num_units;
    double m_cities_num;

public:

	WillOfThePeopleFlat();
	void Serialize(CivArchive &archive);
};

class WillOfThePeoplePtr {
protected:
    sint32 m_num_gov;
    BOOL *m_is_enabled;

    double *m_utility;

public:

	WillOfThePeoplePtr();
    ~WillOfThePeoplePtr();
	void Serialize(CivArchive &archive);
	void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
};

class WillOfThePeople : public WillOfThePeopleFlat , public WillOfThePeoplePtr {

public:

	sint8 *m_rank;


    WillOfThePeople(AiMain *ai);
    WillOfThePeople(CivArchive &archive);
    ~WillOfThePeople();
    void Serialize(CivArchive &archive);







	double UnhappyGovWorkday(AiMain *ai,  const sint32 gov);
	double UnhappyGovWages(AiMain *ai, const sint32 gov);
	double UnhappyGovRations(AiMain *ai, const sint32 gov);
	double UnhappyGovEmpSize(AiMain *ai, const sint32 gov);
    double UnhappyGovTooManyCities(AiMain *ai, const sint32 gov);
    void HappyMartialLaw(AiMain *ai, const sint32 gov, double &happy);
	double UnhappyUnitDist(AiMain *ai, const sint32 gov);
	double UnhappyCombatMoral(AiMain *ai, const sint32 gov);
	double UnhappyPolluion(AiMain *ai, const sint32 gov);
	double GovCrime(AiMain *ai, const sint32 gov, const double happy);
	double GovGold(AiMain *ai,  const sint32 gov, const double happy, const double crime);
    double UtilityGovTooManyCities(AiMain *ai, const sint32 gov);
	double UtilityGovProduction(AiMain *ai, const sint32 gov, const double crime);
	double UtilityGovKnowlege(AiMain *ai, const sint32 gov, const double gold);
	double UtilityGovReadiness(AiMain *ai, const sint32 gov, const double crime);
	double UtilityGoverment(AiMain *ai, sint32 idx_gov);
    double UtilityIWantGov(AiMain *ai, sint32 idx_gov);

    void SetStats(AiMain *ai);
	void PollNewGov(AiMain *ai);

    void RegisterLearnedScience(AiMain *ai, const sint32 idx_sci);

    sint32 GetCurrentGov() const { return m_current_gov; }

	sint8 RankGovernment(AiMain *ai, sint32 idx_gov);
};

#endif __WILLOFTHEPEOPLE_H__
