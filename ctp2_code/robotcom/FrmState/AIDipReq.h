#pragma once
#ifndef __AI_DIP_REQ_H__
#define __AI_DIP_REQ_H__

#include "ic3DipReq.h"

class AiMain;
class Foreigner;
class ForeignCity;

#define k_MAX_DIPLOMATIC_UTILITY 1000

class AIDiplomaticRequest : public C3AIDiplomaticRequest
{
public:
	double m_utility;
	double m_threshold;
	double m_dip_send_coef;
	ForeignCity *m_target_city_ptr;

	sint32 m_isvalid;

	void Clear() {
		m_request = REQUEST_TYPE_NULL;
		m_tone = k_MESSAGE_TONE_NEUTRAL;
		m_advance = -1;
		m_reciprocalAdvance = -1;
		m_gold = 0;
		m_targetCity = 0;
		m_reciprocalCity = 0;
		m_utility = 0;
		m_dip_send_coef = 1;
		m_threshold = k_MAX_DIPLOMATIC_UTILITY + 1;
		m_isvalid = FALSE;
	}
	void WeighOptions(AiMain *ai, Foreigner *foreigner, BOOL firstCall);
};

#endif
