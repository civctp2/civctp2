//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : SLIC AI functions
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
//----------------------------------------------------------------------------
//
//
// Modifications from the original Activision code:
//
// - Slic AI functions of the patch added, by Martin Gühmann.
// - DeclareWar slic function added, by Peter Triggs.
// - Marked Microsoft specific pragma, cleaned up export, voided DeclareWar.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __SLICFUNCAI_H__
#define __SLICFUNCAI_H__



#include "SlicFunc.h"	// SLICFUNC, SFR






SLICFUNC(SFR_VOID, LogRegardEvent)


SLICFUNC(SFR_INT, GetPublicRegard)


SLICFUNC(SFR_INT, GetEffectiveRegard)


SLICFUNC(SFR_INT, GetTrust)


SLICFUNC(SFR_VOID, SetTrust)


SLICFUNC(SFR_VOID, RecomputeRegard)






SLICFUNC(SFR_INT, HasAgreementWithAnyone);


SLICFUNC(SFR_INT, HasAgreement);


SLICFUNC(SFR_VOID, CancelAgreement);


SLICFUNC(SFR_INT, TurnsSinceLastWar);


SLICFUNC(SFR_INT, TurnsAtWar);



SLICFUNC(SFR_INT, GetLastHotwarAttack);



SLICFUNC(SFR_INT, GetLastColdwarAttack);










SLICFUNC(SFR_VOID, ConsiderResponse);


SLICFUNC(SFR_VOID, SetResponse);






SLICFUNC(SFR_VOID, ConsiderMotivation)






SLICFUNC(SFR_VOID, ConsiderNewProposal)


SLICFUNC(SFR_VOID, SetNewProposal);






SLICFUNC(SFR_VOID, ConsiderStrategicState);


SLICFUNC(SFR_VOID, ComputeCurrentStrategy);






SLICFUNC(SFR_VOID, ConsiderDiplomaticState);


SLICFUNC(SFR_VOID, ChangeDiplomaticState);






SLICFUNC(SFR_INT, GetTradeFrom);


SLICFUNC(SFR_INT, GetTributeFrom);


SLICFUNC(SFR_INT, GetGoldSurplusPercent)


SLICFUNC(SFR_INT, CanBuySurplus)


SLICFUNC(SFR_INT, GetAdvanceLevelPercent)


SLICFUNC(SFR_INT, AtWarCount)


SLICFUNC(SFR_INT, EffectiveAtWarCount)


SLICFUNC(SFR_INT, AtWarWith)


SLICFUNC(SFR_INT, EffectiveWarWith)







SLICFUNC(SFR_INT, GetNuclearLaunchTarget)



SLICFUNC(SFR_VOID, TargetNuclearAttack)





SLICFUNC(SFR_VOID, SetArmyDetachState)

SLICFUNC(SFR_INT, GetBorderIncursionBy)
SLICFUNC(SFR_INT, GetLastNewProposalType)
SLICFUNC(SFR_INT, GetLastNewProposalArg) //Not implemented
SLICFUNC(SFR_INT, GetLastNewProposalTone)
SLICFUNC(SFR_INT, GetLastResponseType)
SLICFUNC(SFR_INT, GetLastCounterResponseType)
SLICFUNC(SFR_INT, GetLastCounterResponseArg)//Not implemented
SLICFUNC(SFR_INT, GetLastThreatResponseType)
SLICFUNC(SFR_INT, GetLastThreatResponseArg)//Not implemented
SLICFUNC(SFR_INT, GetAgreementDuration)
SLICFUNC(SFR_INT, GetNewProposalPriority)
SLICFUNC(SFR_INT, GetNextAdvance)
SLICFUNC(SFR_INT, GetDesiredAdvanceFrom)
SLICFUNC(SFR_INT, GetLastBorderIncursion)
SLICFUNC(SFR_INT, GetPersonalityType)
SLICFUNC(SFR_INT, GetAtRiskCitiesValue)
SLICFUNC(SFR_INT, GetRelativeStrength)
SLICFUNC(SFR_INT, GetDesireWarWith)
SLICFUNC(SFR_INT, RoundPercentReduction)
SLICFUNC(SFR_INT, RoundGold)
SLICFUNC(SFR_INT, GetPollutionLevelPromisedTo)//Not implemented
SLICFUNC(SFR_INT, GetPiracyIncomeFrom)
SLICFUNC(SFR_INT, GetProjectedScience)//Not implemented
SLICFUNC(SFR_INT, CanFormAlliance)
SLICFUNC(SFR_INT, GetStopResearchingAdvance)//Not implemented
SLICFUNC(SFR_INT, GetNanoWeaponsCount)
SLICFUNC(SFR_INT, GetBioWeaponsCount)
SLICFUNC(SFR_INT, GetNuclearWeaponsCount)
SLICFUNC(SFR_INT, FindCityToExtortFrom)//Not implemented
SLICFUNC(SFR_INT, GetEmbargo)
SLICFUNC(SFR_INT, SetEmbargo)
SLICFUNC(SFR_INT, GetTotalValue)
SLICFUNC(SFR_INT, GetNewProposalResult)
SLICFUNC(SFR_INT, GetCounterProposalResult)
SLICFUNC(SFR_INT, GetMostAtRiskCity)//Not implemented
SLICFUNC(SFR_VOID, DeclareWar)


#endif __SLICFUNCAI_H__
