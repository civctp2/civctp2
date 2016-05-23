#ifndef DIPLOMACY_UTIL_H__
#define DIPLOMACY_UTIL_H__

#include "diplomattypes.h"

class DiplomacyProposalRecord;
class DiplomacyThreatRecord;

void diplomacyutil_Initialize();
const DiplomacyProposalRecord *diplomacyutil_GetRecord(PROPOSAL_TYPE type);
sint32 diplomacyutil_GetDBIndex(PROPOSAL_TYPE type);
PROPOSAL_TYPE diplomacyutil_GetProposalType(sint32 dbIndex);

const DiplomacyThreatRecord *diplomacyutil_GetRecord(THREAT_TYPE type);
sint32 diplomacyutil_GetDBIndex(THREAT_TYPE type);
THREAT_TYPE diplomacyutil_GetThreatType(sint32 dbIndex);

#endif
