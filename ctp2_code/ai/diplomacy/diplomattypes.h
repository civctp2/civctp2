//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Types of diplomacy objects
// Id           : $Id$
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
// - Compiler version (for the Microsoft C++ compiler only)
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       holds: the (_MSC_VER) preprocessor directive lines, and the blocks 
//       that are inactive for _MSC_VER value 1200 are modified Apolyton code. 
//       The blocks that are inactiThe blocks that are active for _MSC_VER 
//       value 1200 are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Marked Microsoft extensions.
// - Corrected compiler warnings.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __DIPLOMAT_TYPES_H__
#define __DIPLOMAT_TYPES_H__

#include <string>

typedef sint32 PLAYER_INDEX;
typedef sint32 StringId;

namespace DiplomatTypes
{
	void InitializeSlicConsts();
};

enum
{
	MIN_REGARD     =    0,
	HOTWAR_REGARD  =  100,
	COLDWAR_REGARD =  300,
	NEUTRAL_REGARD =  500,
	FRIEND_REGARD  =  700,
	ALLIED_REGARD  =  900,
	MAX_REGARD     = 1000,
};

enum DIPLOMATIC_TONE
{
	DIPLOMATIC_TONE_NOT_CHOSEN = -1,
	DIPLOMATIC_TONE_KIND, 
	DIPLOMATIC_TONE_EQUAL, 
	DIPLOMATIC_TONE_MEEK, 
	DIPLOMATIC_TONE_INDIGNANT, 
	DIPLOMATIC_TONE_ANGRY, 
	DIPLOMATIC_TONE_MAX
};

enum REGARD_EVENT_TYPE
{
		REGARD_EVENT_SCENARIO,
		REGARD_EVENT_MILITARY_POWER,
		REGARD_EVENT_MILITARY_SAFETY,
		REGARD_EVENT_DIPLOMACY,
		REGARD_EVENT_GOLD,
		REGARD_EVENT_KNOWLEDGE,
		REGARD_EVENT_PRODUCTION,
		REGARD_EVENT_ALL,
		REGARD_EVENT_MAX
};

extern std::string s_regardEventNames[REGARD_EVENT_MAX];

enum PROPOSAL_TYPE
{
	PROPOSAL_NONE,
	
	PROPOSAL_OFFER_GIVE_CITY,			
	PROPOSAL_REQUEST_GIVE_CITY,			
	
	PROPOSAL_OFFER_WITHDRAW_TROOPS,			
	PROPOSAL_REQUEST_WITHDRAW_TROOPS,		
	
	PROPOSAL_OFFER_STOP_PIRACY,			
	PROPOSAL_REQUEST_STOP_PIRACY,		
	
	PROPOSAL_OFFER_BREAK_AGREEMENT,			
	PROPOSAL_REQUEST_BREAK_AGREEMENT,		
	
	PROPOSAL_OFFER_STOP_RESEARCH,			
	PROPOSAL_REQUEST_STOP_RESEARCH,			
	
	PROPOSAL_OFFER_REDUCE_NUCLEAR_WEAPONS,	
	PROPOSAL_REQUEST_REDUCE_NUCLEAR_WEAPONS,
	
	PROPOSAL_OFFER_REDUCE_BIO_WEAPONS,		
	PROPOSAL_REQUEST_REDUCE_BIO_WEAPONS,	
	
	PROPOSAL_OFFER_REDUCE_NANO_WEAPONS,		
	PROPOSAL_REQUEST_REDUCE_NANO_WEAPONS,	
	
	PROPOSAL_OFFER_GIVE_ADVANCE,			
	PROPOSAL_REQUEST_GIVE_ADVANCE,			
	
	PROPOSAL_OFFER_GIVE_GOLD,				
	PROPOSAL_REQUEST_GIVE_GOLD,				
	
	PROPOSAL_OFFER_REDUCE_POLLUTION,		
	PROPOSAL_REQUEST_REDUCE_POLLUTION,		
	
	PROPOSAL_OFFER_MAP,						
	PROPOSAL_REQUEST_MAP,					
	
	PROPOSAL_OFFER_HONOR_MILITARY_AGREEMENT,	
	PROPOSAL_REQUEST_HONOR_MILITARY_AGREEMENT,	
	
	PROPOSAL_OFFER_HONOR_POLLUTION_AGREEMENT,	
	PROPOSAL_REQUEST_HONOR_POLLUTION_AGREEMENT,	
	
	PROPOSAL_OFFER_END_EMBARGO,				
	PROPOSAL_REQUEST_END_EMBARGO,			
	
	
	PROPOSAL_TREATY_DECLARE_WAR,			
	PROPOSAL_TREATY_CEASEFIRE,				
	PROPOSAL_TREATY_PEACE,					
	PROPOSAL_TREATY_TRADE_PACT,				
	PROPOSAL_TREATY_RESEARCH_PACT,			
	PROPOSAL_TREATY_MILITARY_PACT,			
	PROPOSAL_TREATY_POLLUTION_PACT,			
	PROPOSAL_TREATY_ALLIANCE,				
	PROPOSAL_MAX
};

extern std::string s_proposalNames[PROPOSAL_MAX];

enum THREAT_TYPE
{
	THREAT_NONE,
	//Maybe there were some more kind of threads
	//
	//
	THREAT_DESTROY_CITY,
	//
	THREAT_TRADE_EMBARGO,
	THREAT_DECLARE_WAR,
	THREAT_MAX
};

extern std::string s_threatNames[THREAT_MAX];

enum RESPONSE_TYPE
{
	RESPONSE_INVALID,
	RESPONSE_REJECT,
	RESPONSE_ACCEPT,
	RESPONSE_COUNTER,
	RESPONSE_THREATEN,
	RESPONSE_MAX
};

extern std::string s_responseNames[RESPONSE_MAX];

struct DiplomacyResult
{
	DiplomacyResult()
	:	science    (0),
		gold       (0),
		production (0),
		regard     (0)
	{}

	sint32 science;
	sint32 gold;
	sint32 production;
	sint32 regard;
};

struct DiplomacyArg
{
	DiplomacyArg()
	:	playerId     (-1),
		cityId       (-1),
		armyId       (-1),
		agreementId  (-1),
		advanceType  (-1),
		unitType     (-1),
		pollution    (-1),
		gold         (-1),
		percent      (-1.0)
	{}

	bool operator==(const DiplomacyArg &a) const
	{
		return playerId     == a.playerId
		    && cityId       == a.cityId
		    && armyId       == a.armyId
		    && agreementId  == a.agreementId
		    && advanceType  == a.advanceType
		    && unitType     == a.unitType
		    && pollution    == a.pollution
		    && gold         == a.gold
		    && percent      == a.percent;
	}
	
	bool operator<(const DiplomacyArg &a) const
	{
		return playerId     <= a.playerId
		    && cityId       <= a.cityId
		    && armyId       <= a.armyId
			&& agreementId  <= a.agreementId
			&& advanceType  <= a.advanceType
			&& unitType     <= a.unitType
			&& pollution    <= a.pollution
			&& gold         <= a.gold
			&& percent      <= a.percent;
	}

	const DiplomacyArg & operator=(const DiplomacyArg & rval)
	{
		playerId    = rval.playerId;
		cityId      = rval.cityId;
		armyId      = rval.armyId;
		agreementId = rval.agreementId;
		advanceType = rval.advanceType;
		unitType    = rval.unitType;
		pollution   = rval.pollution;
		gold        = rval.gold;
		percent     = rval.percent;
		return *this;
	}

	const DiplomacyArg & operator=(const sint32 value)
	{
		playerId    = value;
		cityId      = value;
		armyId      = value;
		agreementId = value;
		advanceType = value;
		unitType    = value;
		pollution   = value;
		gold        = value;
		percent     = ((double) value / 100.0);

		return *this;
	}

	PLAYER_INDEX playerId;
	sint32       cityId;
	sint32       armyId;
	sint32       agreementId;
	sint32       advanceType;
	sint32       unitType;
	sint32       pollution;
	sint32       gold;
	double       percent;
};

struct ProposalData
{
	ProposalData()
	:	first_type (PROPOSAL_NONE),
		second_type(PROPOSAL_NONE),
		tone       (DIPLOMATIC_TONE_NOT_CHOSEN)
	{};

	bool operator==(const ProposalData & a) const
	{
		return first_type  == a.first_type
		    && second_type == a.second_type;
	}

	const ProposalData & operator=(const ProposalData & rval)
	{
		first_type  = rval.first_type;
		first_arg   = rval.first_arg;
		second_type = rval.second_type;
		second_arg  = rval.second_arg;
		tone        = rval.tone;

		return *this;
	}

	PROPOSAL_TYPE   first_type;
	DiplomacyArg    first_arg;
	PROPOSAL_TYPE   second_type;
	DiplomacyArg    second_arg;
	DIPLOMATIC_TONE tone;
};

struct ThreatData
{
	ThreatData()
	:	type(THREAT_NONE),
		arg()
	{};

	bool operator==(const ThreatData & a) const
	{
		return type == a.type;
	}

	const ThreatData & operator=(const ThreatData & rval)
	{
		type  = rval.type;
		arg   = rval.arg;

		return *this;
	}

	THREAT_TYPE  type;
	DiplomacyArg arg;
};

struct NewProposal {
	NewProposal()
	:	id            (-1),
		priority      (-1),
		senderId      (-1),
		receiverId    (-1),
		detail          (),
		explainStrId  (-1),
		adviceStrId   (-1),
		newsStrId     (-1)
	{ };

	bool operator==(const NewProposal & a) const
	{
		return senderId   == a.senderId
		    && receiverId == a.receiverId
		    && detail     == a.detail;
	}

	bool operator!=(const NewProposal & a) const
	{
		return !(*this == a);
	}

	const NewProposal & operator=(const NewProposal & rval)
	{
		id           = rval.id;
		priority     = rval.priority;
		senderId     = rval.senderId;
		receiverId   = rval.receiverId;
		detail       = rval.detail;
		explainStrId = rval.explainStrId;
		adviceStrId  = rval.adviceStrId;
		newsStrId    = rval.newsStrId;

		return *this;
	}

	sint32       id;
	sint16       priority;
	PLAYER_INDEX senderId;
	PLAYER_INDEX receiverId;
	ProposalData detail;
	StringId     explainStrId;
	StringId     adviceStrId;
	StringId     newsStrId;
};

struct Response
{
	Response()
	:	id            (-1),
		priority      (-1),
		senderId      (-1),
		receiverId    (-1),
		type          (RESPONSE_INVALID),
		counter       (),
		threat        (),
		explainStrId  (-1),
		adviceStrId   (-1),
		newsStrId     (-1)
	{ };

	bool operator==(const Response & a) const
	{
		return id         == a.id
		    && type       == a.type
		    && senderId   == a.senderId
		    && receiverId == a.receiverId;
	}

	bool operator!=(const Response & a) const
	{
		return !(*this == a);
	}

	const Response & operator=(const Response & rval)
	{
		id           = rval.id;
		priority     = rval.priority;
		senderId     = rval.senderId;
		receiverId   = rval.receiverId;
		type         = rval.type;
		counter      = rval.counter;
		threat       = rval.threat;
		explainStrId = rval.explainStrId;
		adviceStrId  = rval.adviceStrId;
		newsStrId    = rval.newsStrId;

		return *this;
	}

	sint32        id;
	sint16        priority;
	PLAYER_INDEX  senderId;
	PLAYER_INDEX  receiverId;
	RESPONSE_TYPE type;
	
	ProposalData  counter;
	ThreatData    threat;
	
	StringId      explainStrId;
	StringId      adviceStrId;
	StringId      newsStrId;
};

struct Threat
{
	Threat()
	:	id            (-1),
		senderId      (-1),
		receiverId    (-1),
		start         (-1),
		end           (-1),
		explainStrId  (-1),
		newsStrId     (-1)
	{}
	
	bool operator==(const sint32 &tmpId) const
	{
		return tmpId == id;
	}

	bool operator!=(const sint32 &tmpId) const
	{
		return tmpId != id;
	}
	
	bool operator==(const Threat &threat) const
	{
		return detail == threat.detail;
	}

	const Threat & operator=(const Threat & rval)
	{
		id           = rval.id;
		senderId     = rval.senderId;
		receiverId   = rval.receiverId;
		start        = rval.start;
		end          = rval.end;
		detail       = rval.detail;
		explainStrId = rval.explainStrId;
		newsStrId    = rval.newsStrId;

		return *this;
	}

	sint32       id;
	PLAYER_INDEX senderId;
	PLAYER_INDEX receiverId;
	sint16       start;
	sint16       end;
	ThreatData   detail;
	StringId     explainStrId;
	StringId     newsStrId;
};

namespace ai
{
	typedef sint16 Regard;
	
	struct Agreement
	{
		Agreement()
		:	id            (-1),
			senderId      (-1),
			receiverId    (-1),
			start         (-1),
			end           (-1),
			explainStrId  (-1),
			newsStrId     (-1)
		{}
		
		bool operator==(const sint32 &tmpId) const
		{
			return (tmpId == id); 
		}
		
		bool operator==(const ai::Agreement & rval) const
		{
			return  senderId             == rval.senderId
			    &&  receiverId           == rval.receiverId
			    && (proposal.first_type  == rval.proposal.first_type
			    ||  proposal.second_type == rval.proposal.second_type);
		}
		bool operator!=(const ai::Agreement & rval) const
		{
			return ( !(*this == rval));
		}

		const ai::Agreement & operator=(const ai::Agreement & rval)
		{
			id            = rval.id;
			senderId      = rval.senderId;
			receiverId    = rval.receiverId;
			start         = rval.start;
			end           = rval.end;
			proposal      = rval.proposal;
			explainStrId  = rval.explainStrId;
			newsStrId     = rval.newsStrId;

			return *this;
		}

		sint32       id;
		PLAYER_INDEX senderId;
		PLAYER_INDEX receiverId;
		sint16       start;
		sint16       end;
		ProposalData proposal;
		StringId     explainStrId;
		StringId     newsStrId;
	};
};

enum MOTIVATION_TYPE {
	MOTIVATION_NONE,
	
	MOTIVATION_FEAR_SCENARIO,
	MOTIVATION_FEAR_INVASION,
	MOTIVATION_FEAR_CITY_DEFENSE,
	MOTIVATION_FEAR_PIRACY,
	MOTIVATION_FEAR_SPECIAL_ATTACKS,
	MOTIVATION_FEAR_ADVANCE,
	MOTIVATION_FEAR_SPYING,
	MOTIVATION_FEAR_SCIENCE_RANK,
	MOTIVATION_FEAR_MILITARY_RANK,
	MOTIVATION_FEAR_TRADE_RANK,
	MOTIVATION_FEAR_POLLUTION,
	
	MOTIVATION_DESIRE_SCENARIO,
	MOTIVATION_DESIRE_ATTACK,
	MOTIVATION_DESIRE_ATTACK_CITY,
	MOTIVATION_DESIRE_ADVANCE,
	MOTIVATION_DESIRE_TRADE,
	MOTIVATION_DESIRE_GROWTH,
	MOTIVATION_DESIRE_GOLD,
	MOTIVATION_DESIRE_INTIMIDATE,
	MOTIVATION_DESIRE_MAKE_FRIEND,
	MOTIVATION_DESIRE_ENLIST_FRIEND,
	MOTIVATION_MAX
};

extern std::string s_motivationNames[MOTIVATION_MAX];

struct Motivation
{
	Motivation()
	:	priority    (-1),
		type        (MOTIVATION_NONE),
		adviceStrId (-1)
	{}

	bool operator==(const Motivation &motivation) const
	{
		return priority == motivation.priority
		    && type     == motivation.type
		    && arg      == motivation.arg;
	}

	bool operator!=(const Motivation & motivation) const
	{
		return !operator==(motivation);
	}

	bool operator<(const Motivation & motivation) const
	{
		return priority < motivation.priority
		    && type     < motivation.type
		    && arg      < motivation.arg;
	}

	sint16          priority;
	MOTIVATION_TYPE type;
	DiplomacyArg    arg;
	StringId        adviceStrId;
};

struct AiState {
	AiState()
	:	priority    (-1),
		dbIndex     (-1),
		spyStrId    (-1),
		adviceStrId (-1),
		newsStrId   (-1)
	{}

	bool operator==(const AiState &a) const 
	{
		return (dbIndex==a.dbIndex);
	}

	bool operator!=(const AiState &a) const
	{
		return (dbIndex!=a.dbIndex);
	}

	sint32   priority;
	sint32   dbIndex;
	StringId spyStrId;
	StringId adviceStrId;
	StringId newsStrId;
};

struct OldNegotiationEvent
{
	NewProposal   proposal;
	Response      response;
	ai::Agreement agreement;
	Threat        threat;
};

struct NegotiationEvent
{
	NegotiationEvent()
	: proposal   (),
	  response   (),
	  agreement  (),
	  threat     (),
	  round      (-1)
	{}

	NewProposal   proposal;
	Response      response;
	ai::Agreement agreement;
	Threat        threat;
	sint16        round;
};

#endif // __DIPLOMAT_TYPES_H__
