











#pragma once
#ifndef __SLICFUNCAI_H__
#define __SLICFUNCAI_H__


struct ProposalData;
struct Response;
struct NewProposal;

#include "slicfunc.h"








bool ParseProposalDataSlicArgs(SlicArgList *args, sint32 &argNum, ProposalData &data);


bool ParseResponseSlicArgs(SlicArgList *args, sint32 &argNum, Response &data);


bool ParseNewProposalSlicArgs(SlicArgList *args, sint32 &argNum, NewProposal &data);






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


#endif __SLICFUNCAI_H__
