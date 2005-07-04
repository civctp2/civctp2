//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Advance (tech) handling
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
// _DEBUG
// Generate extra debug output
// 
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Microsoft specifics marked.
// - Safeguard FindLevel against infinite recursion.
// - Speeded up goody hut advance and unit selection.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef _ADVANCES_H_
#define _ADVANCES_H_

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// #include <>      

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class           Advances;
typedef sint32  AdvanceType;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "AdvanceRecord.h"  // AdvanceRecord
#include "AICause.h"	    // CAUSE_SCI
#include "civarchive.h"     // CivArchive
#include "ctp2_inttypes.h"  // uint8, uint16, sint32
#include "player.h"         // PLAYER_INDEX

//----------------------------------------------------------------------------
// Declarations
//----------------------------------------------------------------------------

uint32 Advances_Advances_GetVersion(void);


class Advances 
{

private:
//----------------------------------------------------------------------------
// Do not change anything in the types or order of the following variable 
// declarations. Doing so will break reading in of save files.
// See the Serialize implementation for more details.
//----------------------------------------------------------------------------

	PLAYER_INDEX m_owner;

	sint32 m_size;
	AdvanceType m_researching;

	sint32	m_age;	

	sint32 m_theLastAdvanceEnabledThisManyAdvances; 
	
	sint32 m_total_cost;
	sint32 m_discovered; 

//----------------------------------------------------------------------------
// End of fixed variable list. Changing below this line is less dangerous.
//----------------------------------------------------------------------------
	
	uint8* m_hasAdvance;
	uint8 *m_canResearch;
	uint16 *m_turnsSinceOffered;
	
	
	friend class NetInfo;
	friend class NetPlayer;
	friend class NetResearch;

public:

	Advances(sint32 num);
	Advances();
	virtual ~Advances();

	void SetOwner(PLAYER_INDEX o);
	void Copy(Advances *oldCiv);

	sint32 GetNum() const { return m_size; }

	
	void UpdateCitySprites(BOOL forceUpdate);
	uint8 HasAdvance(sint32 index) const;
	void SetHasAdvance(AdvanceType advance);

	void GrantAdvance();  
	void GiveAdvance(AdvanceType adv, CAUSE_SCI cause, BOOL fromClient = FALSE);
	void GiveAdvancePlusPrerequisites(AdvanceType adv);
	void TakeAdvance(AdvanceType adv);
	void InitialAdvance(AdvanceType adv); 
	sint32 GetPollutionSizeModifier(void) const ;
	sint32 GetPollutionProductionModifier(void) const ;

	AdvanceType GetResearching() const { return m_researching; }
	void SetResearching(AdvanceType adv);

	sint32 Advances::GetCostOfWhatHeKnows() const { return m_total_cost; }	

	sint32 GetCost() const;
	sint32 GetCost(const AdvanceType adv) const;

	
	uint8* CanResearch() const;
	BOOL CanResearch(sint32 advance) const;
	void ResetCanResearch(sint32 justGot);
	uint8* CanAskFor(Advances* otherCivAdvances, sint32 &num) const;
	uint8* CanOffer(Advances* otherCivAdvances, sint32 &num) const;

	sint32 GetDiscovered() const { return m_discovered; }

	sint32 GetMinPrerequisites(sint32 adv) const;
	sint32 GetMinPrerequisites(sint32 adv, sint32 limit) const;
	sint32 FindLevel(AdvanceRecord const * const rec, sint32 const fromLevel = 0) const;
	
#ifdef _DEBUG
	void DebugDumpTree();
#endif

	sint32 GetProjectedScience() const;
	sint32 TurnsToNextAdvance(AdvanceType adv = -1) const;

	void AddAlienLifeAdvance();
    
	void Serialize(CivArchive &archive);
};

#endif
