









#include "c3.h"
#include "SlicConst.h"
#include "SlicEngine.h"
#include "diplomattypes.h"

std::string s_regardEventNames[REGARD_EVENT_MAX] = {
	"SCENARIO",
	"MILITARY_POWER",
	"MILITARY_SAFETY",
	"DIPLOMACY",
	"GOLD",
	"KNOWLEDGE",
	"PRODUCTION",
	"ALL",
};

std::string s_proposalNames[PROPOSAL_MAX] = {
	"NONE",
	"OFFER_GIVE_CITY",
	"REQUEST_GIVE_CITY",
	"OFFER_WITHDRAW_TROOPS",
	"REQUEST_WITHDRAW_TROOPS",
	"OFFER_STOP_PIRACY",
	"REQUEST_STOP_PIRACY",
	"OFFER_BREAK_AGREEMENT",
	"REQUEST_BREAK_AGREEMENT",
	"OFFER_STOP_RESEARCH",
	"REQUEST_STOP_RESEARCH",
	"OFFER_REDUCE_NUCLEAR_WEAPONS",
	"REQUEST_REDUCE_NUCLEAR_WEAPONS",
	"OFFER_REDUCE_BIO_WEAPONS",
	"REQUEST_REDUCE_BIO_WEAPONS",
	"OFFER_REDUCE_NANO_WEAPONS",
	"REQUEST_REDUCE_NANO_WEAPONS",
	"OFFER_GIVE_ADVANCE",
	"REQUEST_GIVE_ADVANCE",
	"OFFER_GIVE_GOLD",
	"REQUEST_GIVE_GOLD",
	"OFFER_REDUCE_POLLUTION",
	"REQUEST_REDUCE_POLLUTION",
	"OFFER_MAP",
	"REQUEST_MAP",
	"OFFER_HONOR_MILITARY_AGREEMENT",
	"REQUEST_HONOR_MILITARY_AGREEMENT",
	"OFFER_HONOR_POLLUTION_AGREEMENT",
	"REQUEST_HONOR_POLLUTION_AGREEMENT",
	"OFFER_END_EMBARGO",
	"REQUEST_END_EMBARGO",
	"TREATY_DECLARE_WAR",
	"TREATY_CEASEFIRE",
	"TREATY_PEACE",
	"TREATY_TRADE_PACT",
	"TREATY_RESEARCH_PACT",
	"TREATY_MILITARY_PACT",
	"TREATY_POLLUTION_PACT",
	"TREATY_ALLIANCE",
};

std::string s_threatNames[THREAT_MAX] = { 
	"NONE",
	
	
	
	"DESTROY_CITY",
	
	"TRADE_EMBARGO",
	"DECLARE_WAR"
};

std::string s_responseNames[RESPONSE_MAX] = {
	"INVALID",
	"REJECT",
	"ACCEPT",
	"COUNTER",
	"THREATEN",	
};


std::string s_motivationNames[MOTIVATION_MAX] = {
	"NONE",
	
	"FEAR_SCENARIO",
	"FEAR_INVASION",
	"FEAR_CITY_DEFENSE",
	"FEAR_PIRACY",
	"FEAR_SPECIAL_ATTACKS",
	"FEAR_ADVANCE",
	"FEAR_SPYING",
	"FEAR_SCIENCE_RANK",
	"FEAR_MILITARY_RANK",
	"FEAR_TRADE_RANK",
	"FEAR_POLLUTION_RANK",
	
	"DESIRE_SCENARIO",
	"DESIRE_ATTACK",
	"DESIRE_ATTACK_CITY",
	"DESIRE_ADVANCE",
	"DESIRE_TRADE",
	"DESIRE_GROWTH",
	"DESIRE_GOLD",
	"DESIRE_INTIMIDATE",
	"DESIRE_MAKE_FRIEND",
	"DESIRE_ENLIST_FRIEND",
};


void DiplomatTypes::InitializeSlicConsts() {

	
	g_slicEngine->AddConst("MIN_REGARD", MIN_REGARD);
	g_slicEngine->AddConst("HOTWAR_REGARD", HOTWAR_REGARD);
	g_slicEngine->AddConst("COLDWAR_REGARD", COLDWAR_REGARD);
	g_slicEngine->AddConst("NEUTRAL_REGARD",NEUTRAL_REGARD);
	g_slicEngine->AddConst("FRIEND_REGARD", FRIEND_REGARD);
	g_slicEngine->AddConst("ALLIED_REGARD", ALLIED_REGARD);
	g_slicEngine->AddConst("MAX_REGARD", MAX_REGARD);
	
	sint32 i;
	std::string fullname;

	
	for ( i = 0; i < REGARD_EVENT_MAX; i++) {
		fullname = std::string("REGARD_EVENT_") + s_regardEventNames[i];
		g_slicEngine->AddConst(fullname.c_str(), i);
	}

	
	for ( i = PROPOSAL_NONE; i < PROPOSAL_MAX; i++) {
		fullname = std::string("PROPOSAL_") + s_proposalNames[i];
		g_slicEngine->AddConst(fullname.c_str(),i);
	}

	
	for ( i = THREAT_NONE; i < THREAT_MAX; i++) {
		fullname = std::string("THREAT_") + s_threatNames[i];
		g_slicEngine->AddConst(fullname.c_str(),i);
	}

	
	for ( i = RESPONSE_INVALID; i < RESPONSE_MAX; i++) {
		fullname = std::string("RESPONSE_") + s_responseNames[i];
		g_slicEngine->AddConst(fullname.c_str(),i);
	}

	
	for ( i = MOTIVATION_NONE; i < MOTIVATION_MAX; i++) {
		fullname = std::string("MOTIVATION_") +  s_motivationNames[i];
		g_slicEngine->AddConst(fullname.c_str(),i);
	}
}


