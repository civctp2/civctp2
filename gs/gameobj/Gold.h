
#pragma once
#ifndef __GOLD_H__
#define __GOLD_H__ 1

class CivArchive;

#define k_GOLD_VERSION_MAJOR	0									
#define k_GOLD_VERSION_MINOR	0									


class Gold { 

	sint32 m_level; 
    sint32 m_income_this_turn; 
    sint32 m_gross_income; 
    sint32 m_lost_to_cleric; 
    sint32 m_lost_to_crime; 
    sint32 m_lost_to_rushbuy; 

    sint32 m_wages_this_turn; 
    sint32 m_maintenance_this_turn; 
    sint32 m_science_this_turn;
    sint32 m_level_last_turn; 
    sint32 m_delta_last_turn; 

	double m_consider_for_science; 
	
	sint32 m_owner;

public:

	Gold(sint32 owner); 
	Gold(void) ;

	void AddGold(const Gold &amount) {
		AddGold(amount.GetLevel()) ;
	}

    void AddGold(const sint32 delta);
    void ClearIncome(); 
    void AddIncome(const sint32 delta); 
    void SubIncome(const sint32 delta); 
    sint32  GetIncome() const { return m_income_this_turn; } 
    sint32 DeltaThisTurn() const { return(m_level - m_level_last_turn); }
    BOOL BankruptcyImminent() const;

	void SubGold(const Gold &amount) {
		SubGold(amount.GetLevel()) ;
	}

    void SubGold (const sint32 delta);
	sint32 GetLevel() const { return m_level; }; 
	
	void SetLevel(sint32 level) { m_level = level;};
	sint32 GetOwner() { return m_owner; }

	
	BOOL GiveGold(const Gold &amount) {
		return (GiveGold(amount.GetLevel())) ;
	}

	BOOL GiveGold(const sint32 amount);

	Gold & operator= (const Gold &copyMe) ;

	void Serialize(CivArchive &archive) ;

    void GetGoldLevels(sint32 *income, sint32 *lost_to_cleric,
        sint32 *lost_to_crime, sint32 *maintenance, sint32 *wages, 
        sint32 *science, sint32 *old_savings, sint32 *current_savings); 

    void ClearStats();

    void SetSavings();
    void AddLostToCleric(const sint32 g);
    void AddLostToCrime(const sint32 g); 
    void AddLostToRushBuy(const sint32 g); 
    void SetGrossIncome();
	sint32 GetGrossIncome() const { return m_gross_income; }
    void AddMaintenance(const sint32 g);
    void AddWages(const sint32 g);
    void SetScience(const sint32 g);
	sint32 GetScience() const { return m_science_this_turn; }

	double GetConsiderForScience() const { return m_consider_for_science; }
	void SetConsiderForScience(double cfs) { m_consider_for_science = cfs; }
	
};

uint32 Gold_Gold_GetVersion(void) ;
#endif
