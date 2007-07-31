
#pragma once
#ifndef __FS_DIPLOMAT_H__
#define __FS_DIPLOMAT_H__

interface IC3DiplomaticRequest;

#include "FrameState.h"
#include "fliif.h"

interface IC3TradeOffer;
class Foreigner;
class AIDiplomaticRequest;
template <class T> class PointerList;
class CityAgent;

class FSDiplomat : public FrameState {
private:
	PointerList<IC3TradeOffer> *m_tradeOffers;
	BOOL m_registered_foreigner_vars[FLI_SECT_MAX];
	BOOL m_registered_incoming_vars;
	BOOL m_registered_pre_outgoing;
	BOOL m_registered_received_vars;

	sint32 FSDiplomat::AreHumansOccupied(AiMain *ai);
	void ComputeNormalizedStr(AiMain *ai);

    friend AiMain; 
public:
	FSDiplomat();
	~FSDiplomat();
	
	BOOL Execute(AiMain *ai, sint32 &branch, FILE *fout);
	void Serialize(CivArchive &archive);
	
	void SetupIncomingMessageVars(AiMain *ai,
								  IC3DiplomaticRequest *request);
	BOOL HandleIncomingMessage(AiMain *ai, IC3DiplomaticRequest *request);
	BOOL HandleIncomingResponse(AiMain *ai, IC3DiplomaticRequest *request);
	BOOL HandleAcceptedMessage(AiMain *ai, IC3DiplomaticRequest *request);
	BOOL HandleRejectedMessage(AiMain *ai, IC3DiplomaticRequest *request);
	void SetupForeignerVars(AiMain *ai, Foreigner *foreigner,
							BOOL isBeginDiplomacy, sint32 section);
    
    void BuildFriendTable(AiMain *ai);

	double GetUtility(AiMain *ai, IC3DiplomaticRequest *request);

	double GreetingUtility(AiMain *ai, IC3DiplomaticRequest *request);
	
	
	double DemandAdvanceUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double DemandCityUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double DemandMapUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double DemandGoldUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double DemandStopTradeUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double DemandAttackEnemyUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double DemandLeaveOurLandsUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double DemandReducePollutionUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double DemandLowerTariffsUtility(AiMain *ai, IC3DiplomaticRequest *request);
	
	
	double OfferAdvanceUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double OfferCityUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double OfferMapUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double OfferGoldUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double OfferCeaseFireUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double OfferPermanentAllianceUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double OfferPactCaptureCityUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double OfferPactEndPollutionUtility(AiMain *ai, IC3DiplomaticRequest *request);

	
	
	double ExchangeAdvanceUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double ExchangeCityUtility(AiMain *ai, IC3DiplomaticRequest *request);
	double ExchangeMapUtility(AiMain *ai, IC3DiplomaticRequest *request);

	void GetSendUtility(AiMain *ai,
						Foreigner *foreigner,
						AIDiplomaticRequest *request);
	double GreetingSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	
	
	double DemandAdvanceSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double DemandCitySendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double DemandMapSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double DemandGoldSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double DemandStopTradeSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double DemandAttackEnemySendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double DemandLeaveOurLandsSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double DemandReducePollutionSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double DemandLowerTariffsSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	
	
	double OfferAdvanceSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double OfferCitySendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double OfferMapSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double OfferGoldSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double OfferCeaseFireSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double OfferPermanentAllianceSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double OfferPactCaptureCitySendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double OfferPactEndPollutionSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);

	
	
	double ExchangeAdvanceSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double ExchangeCitySendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);
	double ExchangeMapSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request);


	
	
	void ReceiveTradeBid(AiMain *ai,
						 uint32 fromCity,
						 sint32 resource,
						 sint32 toCityOwner,
						 uint32 toCity,
						 sint32 price,
						 uint32 bidId);

	void RegisterTradeOffer(AiMain *ai, IC3TradeOffer *offer);
	void RemoveTradeOffer(AiMain *ai, uint32 id);
	IC3TradeOffer *AlreadyOffering(AiMain *ai, CityAgent *city, sint32 resource);
	void ConsiderTradeOffers(AiMain *ai);
	void ConsiderMakingTradeOffers(AiMain *ai);
	BOOL ConfirmAcceptTradeOffer(AiMain *ai,
								 uint32 offer, uint32 destCity,
								 PLAYER_INDEX player);
	void ConsiderMakingTradeBids(AiMain *ai);
	void ClearFuzzyMessageReceived(AiMain *ai);

	void RegisterYouAreViolatingAgreement(AiMain *ai,
										  AGREEMENT_TYPE agreement,
										  PLAYER_INDEX against,
										  sint32 rounds);
	void RegisterOtherCivIsViolatingAgreement(AiMain *ai,
											  AGREEMENT_TYPE agreement,
											  PLAYER_INDEX violator,
											  sint32 rounds);
	void RegisterAttack(AiMain *ai,
						PLAYER_INDEX attacker, PLAYER_INDEX defender);
	void RegisterBrokeTreaty(AiMain *ai,
							 PLAYER_INDEX breaker,
							 PLAYER_INDEX breakee);
};

#endif
