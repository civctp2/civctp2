#include "c3.h"

#include "Sci.h"
#include "CivArchive.h"
#include "CtpAi.h"
#include "Player.h"
#include "AgreementMatrix.h"
#include "Unit.h"
#include "UnitData.h"
#include "UnitPool.h"

Science::Science()

{ 
	m_level = 0; 
}











void Science::Serialize(CivArchive &archive)
{
	if (archive.IsStoring())
		archive<<m_level ;
	else
		archive>>m_level ;

}














uint32 Sci_Science_GetVersion(void)
	{
	return (k_SCIENCE_VERSION_MAJOR<<16 | k_SCIENCE_VERSION_MINOR) ;
	}



sint32 Science::ComputeScienceFromResearchPacts(const sint32 playerId)
{
	sint32 total_pact_science = 0;
	for (PLAYER_INDEX foreignerId = 1; foreignerId < CtpAi::s_maxPlayers; foreignerId++)
	{
		if (g_player[foreignerId])
		{
			total_pact_science += ComputeScienceFromResearchPact(playerId, foreignerId);
		}
	}
	return total_pact_science;
}


sint32 Science::ComputeScienceFromResearchPact(const sint32 playerId, const PLAYER_INDEX foreignerId)
{
	Assert(g_player[foreignerId] != NULL);
	if (g_player[foreignerId] == NULL)
		return 0;

	ai::Agreement research_pact =
		AgreementMatrix::s_agreements.GetAgreement(playerId, foreignerId, PROPOSAL_TREATY_RESEARCH_PACT);
	sint32 research_pact_science = 0;

	
	if (research_pact.start != -1 && research_pact.end == -1) 
	{
		Unit city;
		sint32 num_cities = g_player[foreignerId]->m_all_cities->Num();
		for (sint32 i = 0; i < num_cities; i++)
		{
			city = g_player[foreignerId]->m_all_cities->Access(i);
			Assert( g_theUnitPool->IsValid(city) );
			Assert( city->GetCityData() != NULL );
			
			research_pact_science += city.CD()->GetScience();
		}
		
		research_pact_science = (research_pact_science * 1.05);
	}
	return research_pact_science;
}
