









#pragma once
#ifndef _ADVANCES_H_
#define _ADVANCES_H_

typedef sint32 AdvanceType;
class CivArchive;
class AdvanceRecord;

typedef sint32 PLAYER_INDEX;

enum CAUSE_SCI;

#define k_ADVANCES_VERSION_MAJOR	0								
#define k_ADVANCES_VERSION_MINOR	0								

class Advances {
private:
	
	PLAYER_INDEX m_owner;

	sint32 m_size;
	AdvanceType m_researching;

	sint32	m_age;	

	sint32 m_theLastAdvanceEnabledThisManyAdvances; 
	
	sint32 m_total_cost;
	sint32 m_discovered; 

	

	
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
	sint32 FindLevel(AdvanceRecord* rec);
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
