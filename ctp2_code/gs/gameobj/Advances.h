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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
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
// - Microsoft specifics marked.
// - Safeguard FindLevel against infinite recursion.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _ADVANCES_H_
#define _ADVANCES_H_

typedef sint32 AdvanceType;
class CivArchive;
class AdvanceRecord;

typedef sint32 PLAYER_INDEX;

#if defined(_MSC_VER)
enum CAUSE_SCI;
#else
#include "AICause.h"	// CAUSE_SCI
#endif

#define k_ADVANCES_VERSION_MAJOR	0								
#define k_ADVANCES_VERSION_MINOR	0								

class Advances {
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
	~Advances();

	void SetOwner(PLAYER_INDEX o);
	void Copy(Advances *oldCiv);

	sint32 GetNum() { return m_size; }

	
	void UpdateCitySprites(BOOL forceUpdate);
	uint8 HasAdvance(sint32 index);
	void SetHasAdvance(AdvanceType advance);

	void GrantAdvance();  
	void GiveAdvance(AdvanceType adv, CAUSE_SCI cause, BOOL fromClient = FALSE);
	void GiveAdvancePlusPrerequisites(AdvanceType adv);
	void TakeAdvance(AdvanceType adv);
	void InitialAdvance(AdvanceType adv); 
	sint32 GetPollutionSizeModifier(void) const ;
	sint32 GetPollutionProductionModifier(void) const ;

	AdvanceType GetResearching() { return m_researching; }
	void SetResearching(AdvanceType adv);

	sint32 Advances::GetCostOfWhatHeKnows() { return m_total_cost; }	

	sint32 GetCost();
	sint32 GetCost(const AdvanceType adv);

	
	uint8* CanResearch();
	BOOL CanResearch(sint32 advance);
	void ResetCanResearch(sint32 justGot);
	uint8* CanAskFor(Advances* otherCivAdvances, sint32 &num);
	uint8* CanOffer(Advances* otherCivAdvances, sint32 &num);

	sint32 GetDiscovered() const { return m_discovered; }

	sint32 GetMinPrerequisites(sint32 adv) const;
	sint32 FindLevel(AdvanceRecord const * const rec, sint32 const fromLevel = 0) const;

#ifdef _DEBUG
	void DebugDumpTree();
#endif

	sint32 GetProjectedScience();
	sint32 TurnsToNextAdvance(AdvanceType adv = -1);

	void AddAlienLifeAdvance();

	
	void Serialize(CivArchive &archive);
};

uint32 Advances_Advances_GetVersion(void) ;
#else
class Advances;
#endif
