

#ifndef __Gold_H__
#define __Gold_H__ 1

class CivArchive; 
class AiMain;


class GoldFlat{ 
protected:

    sint32 m_projected_gross_income; 
    sint32 m_old_gross_income; 
    sint32 m_old_lost_to_cleric;     
    sint32 m_old_lost_to_crime;
    sint32 m_old_maintenance; 
    sint32 m_old_wages; 
    sint32 m_old_science; 
    sint32 m_old_savings; 
    sint32 m_current_savings; 

    sint32 spent_overtime;
    sint32 spent_diplomacy; 

    double m_percent_income_new_blg; 
    double m_allowed_expense_new_blg;

    double m_percent_income_wages; 
    double m_percent_income_science;
    double m_percent_income_savings; 
    
    double m_percent_savings_diplomacy;
    double m_percent_savings_overtime;
    double m_percent_savings_reserve;

public:
	GoldFlat(); 
	void Serialize(CivArchive &archive); 
};



class AiGold : public GoldFlat { 


public:


    AiGold(); 
    AiGold(CivArchive &archive); 
    ~AiGold(); 
    void Serialize(CivArchive &archive); 


    void Deduct(const sint32 g); 
    void Add(const sint32 g); 

    void CalcBudget();

    double GetPercentInNewBlg() const; 
    double GetAllowedExpenseNewBlg() const { return m_allowed_expense_new_blg; }
    double GetPercentInWages() const;
    double GetPercentInScience() const;
    double GetPercentSaveOvertime() const;
    double GetPercentSaveDiplomacy() const; 

    sint32 GetProjectedGrossIncome() const { return m_projected_gross_income; } 
    sint32 GetGrossIncome() const { return m_old_gross_income; } 
    sint32 GetLostToCleric() const { return m_old_lost_to_cleric; }
    sint32 GetLostToCrime() const { return m_old_lost_to_crime; }
    sint32 GetOldMaintenance() const { return m_old_maintenance; }
    sint32 GetOldWages() const { return m_old_wages; }
    sint32 GetOldScience() const { return m_old_science; }
    sint32 GetOldSavings() const { return m_old_savings; }
    sint32 GetCurrentSavings() const { return m_current_savings; }

    void SetGoldLevels(AiMain *ai);
    void StoreProjectedIncome(AiMain *ai); 

    void GetExpenses(double &cleric, double &crime, double &maintenance,  
        double &wage, double &science,  double &savings, 
        double &overtime, double &diplomacy);

    void RegisterOvertime(const sint32 g);
    void RegisterDiplomacy(const sint32 d);

	BOOL CanSpendRushBuy(AiMain* ai, const sint32 g);
	BOOL CanSpendSpecialAction(AiMain* ai, const sint32 g);
};

#endif __Gold_H__
