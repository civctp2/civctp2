









#include "c3.h"
#include "c3errors.h"

#include "globals.h"
#include "AdvanceRecord.h"
#include "player.h"
#include "Unit.h"
#include "TurnCnt.h"
#include "Agreement.h"
#include "message.h"
#include "DiplomaticRequest.h"
#include "MessageData.h"
#include "DiplomaticRequestData.h"
#include "MessagePool.h"
#include "StrDB.h"
#include "UnitData.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "aicause.h"

#include "network.h"
#include "net_info.h"
#include "net_action.h"
#include "SelItem.h"
extern SelectedItem *g_selected_item;


	extern	StringDB	*g_theStringDB ;

	extern	TurnCount	*g_turn ;

	extern	Player	**g_player ;



 
#include "Diplomacy_Log.h"
extern Diplomacy_Log *g_theDiplomacyLog; 

#include "TradePool.h"
extern TradePool *g_theTradePool;








DiplomaticRequestData::DiplomaticRequestData(const ID id) : GAMEOBJ(id.m_id)
	{
	
    m_round = g_turn->GetRound() ;

	m_owner = PLAYER_INDEX_INVALID ;
	m_recipient = PLAYER_INDEX_INVALID ;
	m_thirdParty = PLAYER_INDEX_INVALID ;
	m_request = REQUEST_TYPE_NULL ;
	m_response = REQUEST_RESPONSE_TYPE_NULL ;

	m_advance = NULL ;
	m_reciprocalAdvance = NULL ;
	m_targetCity = NULL ;
	m_reciprocalCity = NULL ;
	m_amount = 0 ;
	m_tone = k_MESSAGE_TONE_NEUTRAL;


	}








DiplomaticRequestData::DiplomaticRequestData(const ID id, const PLAYER_INDEX owner, const PLAYER_INDEX recipient, const REQUEST_TYPE request) : GAMEOBJ(id.m_id)
	{
	m_owner = owner ;
	m_recipient = recipient ;
	m_request = request ;

	
	m_round = g_turn->GetRound() ;
	m_thirdParty = PLAYER_INDEX_INVALID ;
	m_response = REQUEST_RESPONSE_TYPE_NULL ;

	m_advance = NULL ;
	m_reciprocalAdvance = NULL ;
	m_targetCity = NULL ;
	m_reciprocalCity = NULL ;
	m_amount = 0 ;
	m_tone = k_MESSAGE_TONE_NEUTRAL;


	}












void DiplomaticRequestData::Serialize(CivArchive &archive)
	{

    CHECKSERIALIZE

    GAMEOBJ::Serialize(archive); 
	uint8 hasChild;

	if (archive.IsStoring())
		{
		archive<<m_round ;
		archive.PutSINT32(m_owner) ;
		archive.PutSINT32(m_recipient) ;
		archive.PutSINT32(m_thirdParty) ;
		archive.PutSINT32(m_request) ;
		archive.PutSINT32(m_response) ;

		archive.PutSINT32(m_advance) ;
		archive.PutSINT32(m_reciprocalAdvance) ;
		archive.PutSINT32(m_tone);
		m_targetCity.Serialize(archive) ;
		m_reciprocalCity.Serialize(archive) ;
		m_amount.Serialize(archive) ;


		hasChild = m_lesser != NULL;
		archive << hasChild;
		if (m_lesser)
			((DiplomaticRequestData *)(m_lesser))->Serialize(archive) ;

		hasChild = m_greater != NULL;
		archive << hasChild;
		if (m_greater)
			((DiplomaticRequestData *)(m_greater))->Serialize(archive) ;

		}
	else
		{
		archive>>m_round ;

		m_owner = (PLAYER_INDEX)(archive.GetSINT32()) ;
		m_recipient = (PLAYER_INDEX)(archive.GetSINT32()) ;
		m_thirdParty = (PLAYER_INDEX)(archive.GetSINT32()) ;
		m_request = (REQUEST_TYPE)(archive.GetSINT32()) ;
		m_response = (REQUEST_RESPONSE_TYPE)(archive.GetSINT32()) ;

		m_advance = archive.GetSINT32() ;
		m_reciprocalAdvance = archive.GetSINT32() ;
		m_tone = archive.GetSINT32();
		m_targetCity.Serialize(archive) ;
		m_reciprocalCity.Serialize(archive) ;
		m_amount.Serialize(archive) ;


		archive >> hasChild;
		if(hasChild)
			m_lesser = new DiplomaticRequestData(archive);
		else
			m_lesser = NULL;
		archive >> hasChild;
		if(hasChild)
			m_greater = new DiplomaticRequestData(archive);
		else
			m_greater = NULL;
		}

	}












void DiplomaticRequestData::SetThirdParty(const PLAYER_INDEX thirdParty)
	{
	Assert(thirdParty >=0 && thirdParty<k_MAX_PLAYERS) ;
	m_thirdParty = thirdParty ;
	}












void DiplomaticRequestData::SetResponse(const REQUEST_RESPONSE_TYPE response)
	{
	m_response = response ;
	}













void DiplomaticRequestData::SetTarget(const Unit &city)
	{
	m_targetCity = city ;
	}












void DiplomaticRequestData::SetGold(const Gold &amount)
	{
	m_amount = amount ;
#if 0
	if(g_network.IsClient()) 
		{
		g_network.SendAction(new NetAction(NET_ACTION_SET_REQUEST_GOLD,
										   (uint32)m_id,
										   (uint32)m_amount.GetLevel()));
		} 
	else 
		{
		g_network.Enqueue(this);
	    }
#endif
	}











void DiplomaticRequestData::Dump(const sint32 i)
	{
	switch (m_request)
		{
		case REQUEST_TYPE_GREETING :								
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : Greetings\n", i, m_owner, m_recipient)) ;
			break ;

		case REQUEST_TYPE_DEMAND_ADVANCE :
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : Demand Advance #%d\n", i, m_owner, m_recipient, m_advance)) ;
			break ;

		case REQUEST_TYPE_DEMAND_CITY :
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : demand City #%d\n", i, m_owner, m_recipient, m_targetCity)) ;
			break ;

		case REQUEST_TYPE_DEMAND_MAP :
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : demand map\n", i, m_owner, m_recipient)) ;
			break ;

		case REQUEST_TYPE_DEMAND_GOLD :
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : demand %d Gold\n", i, m_owner, m_recipient, m_amount)) ;
			break ;

		case REQUEST_TYPE_DEMAND_STOP_TRADE :
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : demand stop trading with P%d\n", i, m_owner, m_recipient, m_thirdParty)) ;
			break ;

		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY :						
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : demand attack enemy P%d\n", i, m_owner, m_recipient, m_thirdParty)) ;
			break ;

		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS :					
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : vacate lands\n", i, m_owner, m_recipient)) ;
			break ;

		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION :					
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : reduce pollution\n", i, m_owner, m_recipient)) ;
			break ;

		case REQUEST_TYPE_OFFER_ADVANCE :							
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : offer of Advance #%d\n", i, m_owner, m_recipient, m_advance)) ;
			break ;

		case REQUEST_TYPE_OFFER_CITY :								
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : offer of City #%d\n", i, m_owner, m_recipient, m_targetCity)) ;
			break ;

		case REQUEST_TYPE_OFFER_MAP :								
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : offer of map\n", i, m_owner, m_recipient)) ;
			break ;

		case REQUEST_TYPE_OFFER_GOLD :								
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : offer of %d Gold\n", i, m_owner, m_recipient, m_amount)) ;
			break ;

		case REQUEST_TYPE_OFFER_CEASE_FIRE :						
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : offers cease fire\n", i, m_owner, m_recipient)) ;
			break ;

		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE :				
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : offer permanent alliance\n", i, m_owner, m_recipient)) ;
			break ;

		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY :					
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : offer pact to capture City #%d\n", i, m_owner, m_recipient, m_targetCity)) ;
			break ;

		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION :			
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : offer pact to end pollution\n", i, m_owner, m_recipient)) ;
			break ;

		case REQUEST_TYPE_EXCHANGE_ADVANCE :						
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : exchange Advance #%d for Advance #%d\n", i, m_owner, m_recipient, m_advance, m_reciprocalAdvance)) ;
			break ;

		case REQUEST_TYPE_EXCHANGE_CITY :							
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : exchange City #%d for City #%d\n", i, m_owner, m_recipient, m_reciprocalCity, m_targetCity)) ;
			break ;

		case REQUEST_TYPE_EXCHANGE_MAP :
			DPRINTF(k_DBG_INFO, ("%d - From P%d to P%d : exchange maps\n", i, m_owner, m_recipient)) ;
			break ;

		default :
			Assert(0) ;
			
			break ;

		}

	}













void DiplomaticRequestData::Enact(BOOL fromCurPlayer)
	{
	SlicObject *so = NULL;

#ifdef _DEBUG
    if (g_theDiplomacyLog) {
        g_theDiplomacyLog->LogEnact(m_owner, m_recipient, m_request);	
    }
#endif _DEBUG

	if(g_network.IsClient() && !fromCurPlayer) {
		g_network.SendAction(new NetAction(NET_ACTION_ENACT_REQUEST,
										   (uint32)m_id));
		DiplomaticRequest me(m_id);
		if(!g_network.IsLocalPlayer(g_selected_item->GetCurPlayer())) 
			g_network.AddEnact(me);
		
		
		if(!g_network.IsMyTurn())
			return;
	} else if(g_network.IsHost()) {
		if(!fromCurPlayer && !g_network.IsLocalPlayer(g_selected_item->GetCurPlayer())) {
			g_network.QueuePacket(g_network.IndexToId(g_selected_item->GetCurPlayer()),
								  new NetInfo(NET_INFO_CODE_ENACT_REQUEST_NEED_ACK,
											  (uint32)m_id));
			DiplomaticRequest me(m_id);
			g_network.AddEnact(me);
			return;
		} else {
			g_network.Block(g_selected_item->GetCurPlayer());
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_ENACT_REQUEST,
										  (uint32)m_id));
			g_network.Unblock(g_selected_item->GetCurPlayer());
		}
	}
	
	if(!g_player[m_owner] || !g_player[m_recipient]) {
		
		DiplomaticRequest me(m_id);
		me.Kill();
		return;
	}

	m_response = REQUEST_RESPONSE_TYPE_ACCEPTED ;
	switch (m_request)
		{
		case REQUEST_TYPE_DEMAND_ADVANCE :
			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_DEMAND_ADVANCE) ;
			g_player[m_recipient]->GiveAdvance(m_owner, m_advance, CAUSE_SCI_DIPLOMACY) ;
			g_slicEngine->RunDiscoveryTradedTriggers(m_recipient, m_owner, m_advance);

			so = new SlicObject("01dipAcceptDemandAdvance");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddAdvance(m_advance) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_CITY :
#ifdef _DIPLOMATIC_CITY_EXCHANGE
			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_DEMAND_CITY) ;
			g_player[m_recipient]->GiveCity(m_owner, m_targetCity) ;
			so = new SlicObject("01dipAcceptDemandCity");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddCity(m_targetCity) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
#endif
			break ;

		case REQUEST_TYPE_DEMAND_MAP :
			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_DEMAND_MAP) ;
			g_player[m_recipient]->GiveMap(m_owner) ;
			so = new SlicObject("01dipAcceptDemandMaps");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_GOLD :
			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_DEMAND_GOLD) ;
			if (g_player[m_recipient]->GiveGold(m_owner, m_amount))
                so = new SlicObject("01dipAcceptDemandGold");
			else
                so = new SlicObject("01dipRejectDemandGold");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner);
			so->AddCivilisation(m_recipient) ;
			so->AddGold(m_amount.GetLevel()) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_STOP_TRADE :
			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_DEMAND_STOP_TRADE, m_thirdParty) ;
			g_player[m_recipient]->StopTradingWith(m_thirdParty) ;		
            g_theTradePool->BreakOffTrade(m_owner, m_thirdParty);
			so = new SlicObject("01dipAcceptDemandStoptrade");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddCivilisation(m_thirdParty) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));

            
            


			break ;
		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY:
			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_DEMAND_ATTACK_ENEMY) ;
			so = new SlicObject("01dipAcceptDemandAttack");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddCivilisation(m_thirdParty) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
            
            

            g_theTradePool->BreakOffTrade(m_owner, m_thirdParty);
            g_theTradePool->BreakOffTrade(m_recipient, m_thirdParty);
			break ;

		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS :					
			g_player[m_owner]->MakeLeaveOurLands(m_recipient) ;
			so = new SlicObject("01dipAcceptDemandLeave");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION :					
			g_player[m_owner]->MakeReducePollution(m_recipient) ;
			so = new SlicObject("01dipAcceptDemandPollution");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_ADVANCE :							
			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_OFFER_ADVANCE) ;
			g_player[m_owner]->GiveAdvance(m_recipient, m_advance, CAUSE_SCI_DIPLOMACY) ;
			g_slicEngine->RunDiscoveryTradedTriggers(m_owner, m_recipient, m_advance);

			so = new SlicObject("01dipAcceptOfferAdvance");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddAdvance(m_advance) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_CITY :								
#ifdef _DIPLOMATIC_CITY_EXCHANGE
			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_OFFER_CITY) ;
			g_player[m_owner]->GiveCity(m_recipient, m_targetCity) ;
			so = new SlicObject("01dipAcceptOfferCity");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddCity(m_targetCity) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
#endif
			break ;

		case REQUEST_TYPE_OFFER_MAP :								
			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_OFFER_MAP) ;
			g_player[m_owner]->GiveMap(m_recipient) ;
			so = new SlicObject("01dipAcceptOfferMaps");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_GOLD :								
			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_OFFER_GOLD) ;
			g_player[m_owner]->GiveGold(m_recipient, m_amount) ;
			so = new SlicObject("01dipAcceptOfferGold");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddGold(m_amount.GetLevel()) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_CEASE_FIRE :						
			g_player[m_owner]->MakeCeaseFire(m_recipient) ;
			so = new SlicObject("01dipAcceptTreatyCease");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE :				
			g_player[m_owner]->FormAlliance(m_recipient) ;
			so = new SlicObject("01dipAcceptTreatyAlliance");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY :					
			g_player[m_owner]->MakeCaptureCityPact(m_recipient, m_targetCity) ;
			break ;

		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION :			
		{
			g_player[m_owner]->MakeEndPollutionPact(m_recipient) ;
			so = new SlicObject("01dipAcceptTreatyEco");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));

			SlicObject *so2 = new SlicObject("01dipAcceptTreatyEco");
			
			
			
			so2->AddRecipient(m_recipient);	
			so2->AddCivilisation(m_recipient) ;
			so2->AddCivilisation(m_owner) ;
			so2->AddAttitude(ATTITUDE_TYPE_NEUTRAL);
			g_slicEngine->Execute(so2) ;
			break ;
		}

		case REQUEST_TYPE_EXCHANGE_ADVANCE :						
			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_EXCHANGE_ADVANCE) ;
			g_player[m_owner]->GiveAdvance(m_recipient, m_advance, CAUSE_SCI_DIPLOMACY) ;	
			g_player[m_recipient]->GiveAdvance(m_owner, m_reciprocalAdvance, CAUSE_SCI_DIPLOMACY) ;	

			g_slicEngine->RunDiscoveryTradedTriggers(m_owner, m_recipient, m_advance);
			g_slicEngine->RunDiscoveryTradedTriggers(m_recipient, m_owner, m_reciprocalAdvance);

			so = new SlicObject("01dipAcceptExchangeAdvance");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddAdvance(m_advance) ;
			so->AddAdvance(m_reciprocalAdvance) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_EXCHANGE_CITY :							
#ifdef _DIPLOMATIC_CITY_EXCHANGE
			so = new SlicObject("01dipAcceptExchangeCity");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddCity(m_targetCity) ;
			so->AddCity(m_reciprocalCity) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));

			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_EXCHANGE_CITY) ;
			g_player[m_recipient]->ExchangeCity(m_owner, m_reciprocalCity, m_targetCity) ;	
#endif
			break ;

		case REQUEST_TYPE_EXCHANGE_MAP :
			g_player[m_owner]->MakeShortCeaseFire(m_recipient, AGREEMENT_TYPE_EXCHANGE_MAP) ;
			g_player[m_recipient]->ExchangeMap(m_owner) ;			
			so = new SlicObject("01dipAcceptExchangeMaps");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_NO_PIRACY :
			g_player[m_owner]->MakeNoPiracyPact(m_recipient) ;			
			so = new SlicObject("01dipAcceptDemandPiracy");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;


		default :
			
			break ;

		}

	    if(so) {
			so->SetIsDiplomaticResponse();
			g_slicEngine->Execute(so);
		}
		g_player[m_owner]->RegisterDiplomaticResponse(DiplomaticRequest(m_id));

#ifdef _DEBUG
    if (g_theDiplomacyLog) { 
        g_theDiplomacyLog->LogRegard(m_owner, m_recipient); 
        g_theDiplomacyLog->LogRegard(m_recipient, m_owner); 
    } 
#endif _DEBUG

		DiplomaticRequest me(m_id);
		me.Kill();



	}












ATTITUDE_TYPE DiplomaticRequestData::GetAttitude(PLAYER_INDEX p1, PLAYER_INDEX p2)
	{
	Assert((p1>=0) && (p1<k_MAX_PLAYERS)) ;
	Assert((g_player[p1]) && (!g_player[p1]->IsDead())) ;
	Assert((p2>=0) && (p2<k_MAX_PLAYERS)) ;
	Assert((g_player[p2]) && (!g_player[p2]->IsDead())) ;

	return (g_player[p1]->GetAttitude(p2)) ;
	}













void DiplomaticRequestData::Reject(BOOL fromServer)
	{
	SlicObject	*so = NULL;

	if(g_network.IsClient() && !fromServer) {
		g_network.SendAction(new NetAction(NET_ACTION_REJECT_REQUEST,
										   (uint32)m_id));
	} else if(g_network.IsHost()) {
		g_network.Block(m_recipient);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_REJECT_REQUEST,
									  (uint32)m_id));
		g_network.Unblock(m_recipient);
	}
	
	if(!g_player[m_owner] || g_player[m_owner]->m_isDead ||
	   !g_player[m_recipient] || g_player[m_recipient]->m_isDead) {
		return;
	}

#ifdef _DEBUG
    if (g_theDiplomacyLog) { 
        g_theDiplomacyLog->LogReject(m_owner, m_recipient, m_request); 
    } 
#endif 

	m_response = REQUEST_RESPONSE_TYPE_REJECTED ;
	switch (m_request)
		{
		case REQUEST_TYPE_GREETING :
			break ;

		case REQUEST_TYPE_DEMAND_ADVANCE :
			so = new SlicObject("01dipRejectDemandAdvance");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddAdvance(m_advance) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_CITY :
			so = new SlicObject("01dipRejectDemandCity");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddCity(m_targetCity) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_MAP :
			so = new SlicObject("01dipRejectDemandMaps");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_GOLD :
			so = new SlicObject("01dipRejectDemandGold");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddGold(m_amount.GetLevel()) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_STOP_TRADE :
			so = new SlicObject("01dipRejectDemandStoptrade");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			if(!g_player[m_thirdParty] || g_player[m_thirdParty]->IsDead()) {
				delete so;
				return;
			}
			so->AddCivilisation(m_thirdParty) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY :
			so = new SlicObject("01dipRejectDemandAttack");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			if(!g_player[m_thirdParty] || g_player[m_thirdParty]->IsDead()) {
				delete so;
				return;
			}
			so->AddCivilisation(m_thirdParty) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS :
			so = new SlicObject("01dipRejectDemandLeave");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION :
			so = new SlicObject("01dipRejectDemandPollution");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_ADVANCE :
			so = new SlicObject("01dipRejectOfferAdvance");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddAdvance(m_advance) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_CITY :
			so = new SlicObject("01dipRejectOfferCity");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddCity(m_targetCity) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_MAP :
			so = new SlicObject("01dipRejectOfferMaps");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_GOLD :
			so = new SlicObject("01dipRejectOfferGold");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddGold(m_amount.GetLevel()) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_CEASE_FIRE :
			so = new SlicObject("01dipRejectTreatyCease");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE :
			so = new SlicObject("01dipRejectTreatyAlliance");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY :
			break ;

		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION :
			so = new SlicObject("01dipRejectTreatyEco");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_EXCHANGE_CITY :
			so = new SlicObject("01dipRejectExchangeCity");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddCity(m_targetCity) ;
			so->AddCity(m_reciprocalCity) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;
		
		case REQUEST_TYPE_EXCHANGE_ADVANCE :
			so = new SlicObject("01dipRejectExchangeAdvance");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
			so->AddAdvance(m_advance) ;
			so->AddAdvance(m_reciprocalAdvance) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_EXCHANGE_MAP :
			so = new SlicObject("01dipRejectExchangeMaps");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		case REQUEST_TYPE_DEMAND_NO_PIRACY :
			so = new SlicObject("01dipRejectDemandPiracy");
			so->AddRecipient(m_owner) ;
			so->AddCivilisation(m_owner) ;
			so->AddCivilisation(m_recipient) ;
            so->AddAttitude(GetAttitude(m_recipient, m_owner));
			break ;

		default :
			c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_FOUND") ;
			break ;

		}

	if(so) {
		so->SetIsDiplomaticResponse();
		g_slicEngine->Execute(so);
	}

	g_player[m_owner]->RegisterDiplomaticResponse(DiplomaticRequest(m_id));

#ifdef _DEBUG
    if (g_theDiplomacyLog) { 
        g_theDiplomacyLog->LogRegard(m_owner, m_recipient); 
        g_theDiplomacyLog->LogRegard(m_recipient, m_owner); 
    } 
#endif _DEBUG

}

MBCHAR *DiplomaticRequestData::GetRequestString(void)
{
	switch (m_request) {
		case REQUEST_TYPE_GREETING :
            return("01dipRequestGreetings");
            break;
		case REQUEST_TYPE_DEMAND_ADVANCE :
            return("01dipDemandAdvance");
            break;
		case REQUEST_TYPE_DEMAND_CITY :
            return("01dipDemandCity");
            break;
		case REQUEST_TYPE_DEMAND_MAP :
            return("01dipDemandMaps");
            break;
		case REQUEST_TYPE_DEMAND_GOLD :
            return("01dipDemandGold");
            break;
		case REQUEST_TYPE_DEMAND_STOP_TRADE :
            return("01dipDemandStoptrade");
            break;
		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY :
            return("01dipDemandAttack");
            break;
		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS :
            return("01dipDemandLeave");
            break;
		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION :
            return("01dipDemandPollution");
            break;
		case REQUEST_TYPE_OFFER_ADVANCE :
            return("01dipOfferAdvance");
            break;
		case REQUEST_TYPE_OFFER_CITY :
            return("01dipOfferCity");
            break;
		case REQUEST_TYPE_OFFER_MAP :
            return("01dipOfferMaps");
            break;
		case REQUEST_TYPE_OFFER_GOLD :
            return("01dipOfferGold");
            break;
		case REQUEST_TYPE_OFFER_CEASE_FIRE :
            return("01dipTreatyCease");
            break;
		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE :
            return("01dipTreatyAlliance");
            break;
		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY :
            return("01dipTreatyCapture");
            break;
		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION :
            return("01dipTreatyEco");
            break;
		case REQUEST_TYPE_EXCHANGE_CITY :
            return("01dipExchangeCity");
            break;
		case REQUEST_TYPE_EXCHANGE_ADVANCE :
            return("01dipExchangeAdvance");
            break;
		case REQUEST_TYPE_EXCHANGE_MAP :
            return("01dipExchangeMaps");
            break;
		case REQUEST_TYPE_DEMAND_NO_PIRACY :
            return("01dipDemandPiracy");
            break;
		default :
			c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_FOUND") ;
			break ;

    }
    return(NULL);
}


void DiplomaticRequestData::SetAdvance(const AdvanceType &advance)
{
	m_advance = advance;
}

void DiplomaticRequestData::SetWanted(const AdvanceType &advance)
{
	m_reciprocalAdvance = advance;
}

void DiplomaticRequestData::SetWanted(const Unit &city)
{
	m_reciprocalCity = city;
}

void DiplomaticRequestData::Complete()
{
	switch(GetAttitude(m_owner, m_recipient)) {
		case ATTITUDE_TYPE_STRONG_HOSTILE: 
			m_tone = k_MESSAGE_TONE_HOSTILE; 
			break;
		case ATTITUDE_TYPE_WEAK_HOSTILE: 
			m_tone = k_MESSAGE_TONE_HOSTILE; 
			break;
		case ATTITUDE_TYPE_NEUTRAL:
			m_tone = k_MESSAGE_TONE_NEUTRAL;
			break;
		case ATTITUDE_TYPE_WEAK_FRIENDLY:
			m_tone = k_MESSAGE_TONE_FRIENDLY;
			break;
		case ATTITUDE_TYPE_STRONG_FRIENDLY:
			m_tone = k_MESSAGE_TONE_FRIENDLY;
			break;
		default:
			Assert(FALSE);
			break;
	}

#ifdef _DEBUG
    if (g_theDiplomacyLog) { 
        g_theDiplomacyLog->LogTone(m_owner, m_recipient, GetAttitude(m_owner, m_recipient)); 
    }
#endif _DEBUG

	if(g_network.IsHost()) {
		g_network.Enqueue(this);
	} else if(g_network.IsClient()) {
		g_network.AddCreatedObject(this);
		g_network.SendDiplomaticRequest(this);
	}
}

sint32 DiplomaticRequestData::GetTone() const
{
	return m_tone;
}
