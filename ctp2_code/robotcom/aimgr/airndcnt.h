

#pragma once 

#ifndef __AI_ROUND_CNT_H__
#define __AI_ROUND_CNT_H__ 1

class CivArchive; 
class AiMain;

class AIRoundCount 

{ 

    double m_count; 

public:

    AIRoundCount(); 
    AIRoundCount(CivArchive &archive); 
    void Serialize(CivArchive &archive); 

	void BeginTurn(AiMain *ai); 
	double EstimateRoundsToEnd() const; 
    sint32 GetRound() const { return sint32(m_count); } 
	 
}; 

#endif __AI_ROUND_CNT_H__