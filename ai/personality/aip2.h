











#pragma once
#ifndef __AIP2_H__
#define __AIP2_H__

#include "player.h"


#include <string>
#include <vector>
#include "c3debugstl.h"

#include "DiplomatTypes.h"

class DipAIP {
public:
	DipAIP() {
	};
	bool SwitchAIP(const std::string &name) {
		m_name = name;
		return true;
	}

	
	std::string m_name;
	sint8 m_offerPeace;   
	sint8 m_acceptPeace;  
	sint32 m_warCrimesRegardCost; 
	float m_GoodEventDecay[REGARD_EVENT_ALL];
	float m_BadEventDecay[REGARD_EVENT_ALL];
};

#ifdef _DEBUG
typedef std::vector<DipAIP, std::dbgallocator<DipAIP> > DipAIPVector;
#else
typedef std::vector<DipAIP> DipAIPVector;
#endif

class AIP {

public:
	AIP() {
		m_dipAIP.resize(k_MAX_PLAYERS);
	}

	
	static AIP & GetAIP(const PLAYER_INDEX playerId)	
		{ return s_AIP[playerId]; }

	
	DipAIP & GetDipAIP(const PLAYER_INDEX foreignerId) 
		{ return m_dipAIP[foreignerId]; }

	const std::string & GetPersonality() const 
		{return m_personality;}

	void SetPersonality(const std::string & personality) 
		{m_personality = personality;}


	bool SwitchAIP(const std::string &name) {
		m_strategicStateName = name;
		return true;
	}

	
	sint32 BuildListCompletion(const std::string & buildListName) const {
		return 20;
	};

private:

	static std::vector<AIP> s_AIP;

	DipAIPVector m_dipAIP;

	
	std::string m_personality;
	std::string m_strategicStateName;
};

#endif __AIP2_H__
