#ifndef __AllocWGF_H__
#define __AllocWGF_H__ 1

#include "dynarr.h"

class CivArchive;

#define k_NUM_WGF 5

class AllocWGFFlat{
protected:

    double m_num_slaves;
    double m_num_cities;
    double m_ave_affect_pop;
    double m_max_affect_pop;
    double m_ave_field_workers;
    double m_total_affect_pop;

    double m_citizen_hunger;
    double m_slave_hunger;

    double m_ave_musician_happiness;
    double m_ave_num_musicians;
    double m_total_field_workers;

    double m_old_happy;
    double m_ave_base_happy;
    double m_riot;
    double m_revolution;
    double m_big_city_coef;
    double m_big_city_offset;

    double m_ave_cop;
    double m_crime_coef;
    double m_ave_crime;
    double m_old_crime;
    double m_crime_threshold;

    double m_raw_prod_tile;
    double m_raw_food_tile;

    sint32 m_work_min;
    sint32 m_work_max;
    sint32 m_work_level;
    sint32 m_work_expected;
    double m_work_base;
    double m_work_unit;
    double m_work_coef;

    sint32 m_food_min;
    sint32 m_food_max;
    sint32 m_food_level;
    sint32 m_food_expected;
    double m_food_base;
    double m_food_unit;
    double m_food_coef;

    sint32 m_gold_min;
    sint32 m_gold_max;
    sint32 m_gold_level;
    sint32 m_gold_expected;
    double m_gold_base;
    double m_gold_unit;
    double m_gold_coef;

    sint32 m_old_work_level;
    sint32 m_old_gold_level;
    sint32 m_old_food_level;
    BOOL m_is_reset;

    double m_utility[k_NUM_WGF][k_NUM_WGF][k_NUM_WGF];

public:
	AllocWGFFlat();
	void Serialize(CivArchive &archive);
};


class AiMain;
class AllocWGFPtr {
protected:

    DynamicArray<sint32> m_city_pop;
    DynamicArray<sint32> m_city_slaves;
    DynamicArray<float> m_food_avail;

public:

	AllocWGFPtr();
    ~AllocWGFPtr();
	void Serialize(CivArchive &archive);
	void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
};

class AllocWGF : public AllocWGFFlat , public AllocWGFPtr {




    void CalcWorkRange();
    void CalcGoldRange();
    void CalcFoodRange(AiMain *ai);

    void InitGovernmentVariables(AiMain *ai);
    void InitCityVariables(AiMain *ai);

    double CalcNetHap(const double nm, const double ap,
        const sint32 w, const sint32 g, const sint32 f);
    void CalcHappy (const sint32 w, const sint32 g, const sint32 f,
        double &avail_pop, double &net_happy, double &net_crime);

    void CalcProduction(const sint32 w, const double avail_pop, double &prod_made);
    void CalcFood(const sint32 f, const sint32 w, const double avail_pop, double &food_net);
    void CalcGold(AiMain *ai, const sint32 g, const double avail_pop, double &gold_net);

    double CalcWGFUtility(AiMain *ai, sint32 w, sint32 g, sint32 f,
        double &new_crime, const BOOL is_dump);

    void SetLevel(AiMain *ai, const sint32 w, const sint32 g, const sint32 f);


public:

    AllocWGF();
    AllocWGF(CivArchive &archive);
    ~AllocWGF();
    void Serialize(CivArchive &archive);

    void InitSystemVariables(AiMain *ai);

    void SetRates(AiMain *ai);
    void GetLevel(double &w, double &g, double &f);
    BOOL IsReset () const { return m_is_reset; }

    double GetCitizenHunger() const { return m_citizen_hunger; }
    double GetAveAffectPop() const { return m_ave_affect_pop; }
    double GetTotalAffectivePop() const { return m_total_affect_pop; }
    double GetAveTileProd() const { return m_raw_prod_tile; }

    double GetCurrentRation();

    double ProdOneCitizen();
    double WageOneCitizen();
    double Gold2Prod(const double gold);
    void ContentWages(AiMain *ai, double &wages, double &percent_income);

    void RegisterNewWGF (sint32 w, sint32 g, sint32 f);
};

#endif __AllocWGF_H__
