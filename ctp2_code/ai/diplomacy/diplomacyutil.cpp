#include "c3.h"
#include "diplomacyutil.h"
#include "DiplomacyProposalRecord.h"
#include "DiplomacyThreatRecord.h"

sint32 s_proposal_to_db_map[PROPOSAL_MAX];
PROPOSAL_TYPE s_db_to_proposal_map[PROPOSAL_MAX];

sint32 s_threat_to_db_map[THREAT_MAX];
THREAT_TYPE s_db_to_threat_map[THREAT_MAX];

void diplomacyutil_Initialize()
{

	Assert(g_theDiplomacyProposalDB->NumRecords() == PROPOSAL_MAX);

	sint32 i, j;

	for(i = 0; i < sint32(PROPOSAL_MAX); i++) {
		s_proposal_to_db_map[i] = -1;
		s_db_to_proposal_map[i] = PROPOSAL_NONE;
	}

	for(i = 0; i < g_theDiplomacyProposalDB->NumRecords(); i++) {
		const DiplomacyProposalRecord *rec = g_theDiplomacyProposalDB->Get(i);
		for(j = sint32(PROPOSAL_NONE); j < sint32(PROPOSAL_MAX); j++) {
			if(stricmp(rec->GetType(), s_proposalNames[j].c_str()) == 0) {
				s_proposal_to_db_map[j] = i;
				s_db_to_proposal_map[i] = (PROPOSAL_TYPE)j;
				break;
			}
		}
	}

	Assert(g_theDiplomacyThreatDB->NumRecords() == THREAT_MAX);
	for(i = 0; i < sint32(THREAT_MAX); i++) {
		s_threat_to_db_map[i] = -1;
		s_db_to_threat_map[i] = THREAT_NONE;
	}

	for(i = 0; i < g_theDiplomacyThreatDB->NumRecords(); i++) {
		const DiplomacyThreatRecord *rec = g_theDiplomacyThreatDB->Get(i);
		for(j = sint32(THREAT_NONE); j < sint32(THREAT_MAX); j++) {
			if(stricmp(rec->GetType(), s_threatNames[j].c_str()) == 0) {
				s_threat_to_db_map[j] = i;
				s_db_to_threat_map[i] = (THREAT_TYPE)j;
				break;
			}
		}
	}
}

const DiplomacyProposalRecord *diplomacyutil_GetRecord(PROPOSAL_TYPE type)
{
	return g_theDiplomacyProposalDB->Get(s_proposal_to_db_map[type]);
}

sint32 diplomacyutil_GetDBIndex(PROPOSAL_TYPE type)
{
	return s_proposal_to_db_map[type];
}

PROPOSAL_TYPE diplomacyutil_GetProposalType(sint32 dbIndex)
{
	if(dbIndex < 0 || dbIndex >= PROPOSAL_MAX) {

		return PROPOSAL_NONE;
	}
	return s_db_to_proposal_map[dbIndex];
}

const DiplomacyThreatRecord *diplomacyutil_GetRecord(THREAT_TYPE type)
{
	return g_theDiplomacyThreatDB->Get(s_threat_to_db_map[type]);
}

sint32 diplomacyutil_GetDBIndex(THREAT_TYPE type)
{
	return s_threat_to_db_map[type];
}

THREAT_TYPE diplomacyutil_GetThreatType(sint32 dbIndex)
{
	if(dbIndex < 0 || dbIndex >= THREAT_MAX) {
		return THREAT_NONE;
	}
	return s_db_to_threat_map[dbIndex];
}
