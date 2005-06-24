//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Gold
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
// _MSC_VER		
// - When defined, allows Microsoft C++ extensions.
// - When not defined, generates standard C++.
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks 
//       between #else and #endif are modified Apolyton code. The blocks 
//       between #if and #else are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Marked Microsoft C++ extensions for future GNU compilation.
// - Do not trigger a warning when rushbuying causes a negative cashflow.
// - Cleaned up import structure for future GNU compilation.
// - Merged the constructors.
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __GOLD_H__
#define __GOLD_H__ 1


//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class	Gold;

#define	k_GOLD_VERSION_MAJOR	0									
#define k_GOLD_VERSION_MINOR	0

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "c3types.h"			// MBCHAR, sint..., uint...
#include "civarchive.h"			// CivArchive
#include "gstypes.h"			// PLAYER_INDEX

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------


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
	

    PLAYER_INDEX	m_owner;

public:
	Gold(PLAYER_INDEX const owner = PLAYER_INDEX_INVALID);


	void AddGold(const Gold &amount) {
		AddGold(amount.GetLevel()) ;
	}

    void AddGold(const sint32 delta);
    void ClearIncome(); 
    void AddIncome(const sint32 delta); 
    void SubIncome(const sint32 delta); 
    sint32  GetIncome() const { return m_income_this_turn; } 
	sint32 DeltaThisTurn() const
	{
		// Do not count the rushbuy costs (incidental & under full control of
		// the player) when determining whether the economy is sound.
		return m_level + m_lost_to_rushbuy - m_level_last_turn;
	};
    BOOL BankruptcyImminent() const;

	void SubGold(const Gold &amount) {
		SubGold(amount.GetLevel()) ;
	}

    void SubGold (const sint32 delta);
	sint32 GetLevel() const { return m_level; }; 
	
	void SetLevel(sint32 level) { m_level = level;};
	PLAYER_INDEX	GetOwner() const
	{
		return m_owner;
	};

	
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
