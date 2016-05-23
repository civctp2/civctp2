#include "c3.h"


#include "civarchive.h"
#include "IMapPointData.h"

#include "ic3GameState.h"
#include "ic3player.h"

#include "aimain.h"
#include "bset.h"

#include "gold.h"
#include "FlatPtr.h"
#include "civarchive.h"
#include "FzOut.h"
#include "CityAgent.h"

extern double fz_budget_income_new_blg;
extern double fz_budget_income_wages;
extern double fz_budget_income_science;
extern double fz_budget_income_savings;
extern double fz_budget_savings_diplomacy;
extern double fz_budget_savings_overtime;
extern double fz_budget_savings_reserve;

extern double fz_percent_wages_needed_content;

extern double fz_income_expense_cleric;
extern double fz_income_expense_crime;
extern double fz_income_expense_maintenance;
extern double fz_income_expense_wages;
extern double fz_income_expense_science;
extern double fz_income_expense_savings;

extern double fz_save_expense_overtime;
extern double fz_save_expense_diplomacy;


extern double fz_base_gold_reserve_multiple;


extern double fz_special_action_gold_reserve_multiple;


ZEROMEM(GoldFlat);
FLATSERIALIZE(GoldFlat);









AiGold::AiGold()
{
}







AiGold::AiGold(
  CivArchive &archive
) {
    Serialize(archive);
}







AiGold::~AiGold()
{
}







void AiGold::Serialize(
   CivArchive &archive
)
{
    CHECKSERIALIZE

    GoldFlat::Serialize(archive);
}








void AiGold::Deduct(
   const sint32 g
) {
    Assert(0 < g);

    m_current_savings -= g;

    Assert(0 <= m_current_savings);
}








void AiGold::Add(const sint32 g)
{
    Assert(0 < g);

    m_current_savings += g;

    Assert(0 <= m_current_savings);
}































void AiGold::CalcBudget()
{

    m_percent_income_wages = fz_budget_income_wages;
    m_percent_income_new_blg = fz_budget_income_new_blg;

    m_allowed_expense_new_blg = (m_old_wages +
        m_old_science +  m_old_savings) *fz_budget_income_new_blg;

    double rsum = 1.0 / (fz_budget_income_science + fz_budget_income_savings +
        0.00000001);

    m_percent_income_science = rsum * fz_budget_income_science;

    m_percent_income_savings = rsum * fz_budget_income_savings;

    rsum = 1.0 / (fz_budget_savings_diplomacy +
        fz_budget_savings_overtime +
        fz_budget_savings_reserve);

    m_percent_savings_diplomacy = rsum * fz_budget_savings_diplomacy;
    m_percent_savings_overtime = rsum * fz_budget_savings_overtime;
    m_percent_savings_reserve = rsum * fz_budget_savings_reserve;

}

double AiGold::GetPercentInNewBlg() const
{
    return m_percent_income_new_blg;
}

double AiGold::GetPercentInWages() const
{
    return m_percent_income_wages;
}

double AiGold::GetPercentInScience() const
{
    return m_percent_income_science;
}

double AiGold::GetPercentSaveOvertime() const
{
    return m_percent_savings_overtime;
}

double AiGold::GetPercentSaveDiplomacy() const
{
    return m_percent_savings_diplomacy;
}

void AiGold::SetGoldLevels(AiMain *ai)
{
    ai->m_player->GetGoldLevels(&m_old_gross_income, &m_old_lost_to_cleric,
        &m_old_lost_to_crime, &m_old_maintenance, &m_old_wages,
        &m_old_science, &m_old_savings, &m_current_savings);
}

void AiGold::StoreProjectedIncome(AiMain *ai)
{
    CityAgent *agent=NULL;
    BSetID id;

    m_projected_gross_income = 0;
    for (agent = ai->m_city_set->First(id); ai->m_city_set->Last(); agent = ai->m_city_set->Next(id)) {
        m_projected_gross_income += agent->GetProjectedGrossIncome();
    }
}

void AiGold::RegisterOvertime(const sint32 g)
{
    Assert(0<=g);
    spent_overtime += g;
}

void AiGold::RegisterDiplomacy(const sint32 g)
{
    Assert(0<=g);
    spent_diplomacy += g;
}

void AiGold::GetExpenses(double &cleric, double &crime, double &maintenance,
        double &wage, double &science,  double &savings,
        double &overtime, double &diplomacy)
{
    if (m_old_gross_income < 1) {

        cleric = 0.0;
        crime = 0.0;
        maintenance = 0.0;
        wage = 0.0;
        science = 0.0;
        savings = 0.0;

    } else {

        double r = 1.0/double(m_old_gross_income);

        cleric = r * double (m_old_lost_to_cleric);
        crime = r * double(m_old_lost_to_crime);
        maintenance = r * double(m_old_maintenance);
        wage = r * double(m_old_wages);
        science = r * double(m_old_science);

        double sum = m_old_lost_to_cleric + m_old_lost_to_crime +
                m_old_maintenance +  m_old_wages +  m_old_science;

        if (m_old_gross_income > sum) {
            savings = r * (m_old_gross_income - sum);
        } else {
            savings = 0.0;
        }
    }

    if (m_old_savings < 1) {
        overtime = 0.0;
        diplomacy = 0.0;
    } else {

        overtime = double(spent_overtime) / double(m_old_savings);
        spent_overtime = 0;

        diplomacy = double(spent_diplomacy) / double(m_old_savings);
        spent_diplomacy = 0;
    }
}

BOOL AiGold::CanSpendRushBuy(AiMain* ai, const sint32 g)
{





	double base_reserve = ( m_old_maintenance +
							m_old_wages +
							m_old_lost_to_cleric +
							m_old_lost_to_crime ) *
		fz_base_gold_reserve_multiple;





	double special_action_reserve =  ai->m_gs->GetOrderMaxGoldCost() *
		fz_special_action_gold_reserve_multiple;

	if (m_current_savings - g > (base_reserve + special_action_reserve))
		return TRUE;
	return FALSE;
}

BOOL AiGold::CanSpendSpecialAction(AiMain* ai, const sint32 g)
{




	double base_reserve = ( m_old_maintenance +
						    m_old_wages +
						    m_old_lost_to_cleric +
						    m_old_lost_to_crime ) *
		fz_base_gold_reserve_multiple;

	if (m_current_savings - g > base_reserve)
		return TRUE;
	return FALSE;
}
