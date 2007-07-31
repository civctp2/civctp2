
#include "c3.h"
#include "AIRndCnt.h"
#include "CivArchive.h"
#include "AiMain.h"








AIRoundCount::AIRoundCount()
{
	m_count = 0.0; 
}







AIRoundCount::AIRoundCount(CivArchive &archive)
{ 
    Serialize(archive); 
} 







void AIRoundCount::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive.Store((uint8*)this, sizeof(*this)); 
    } else { 
        archive.Load((uint8*)this, sizeof(*this)); 
    }
}








void AIRoundCount::BeginTurn(AiMain *ai)
{
	m_count = ai->m_gs->GetTurn();
}








double AIRoundCount::EstimateRoundsToEnd() const
{ 

	return max(500 - m_count, 2.0); 
} 

