









#include "c3.h"
#include "c3errors.h"

#include "globals.h"
#include "player.h"
#include "Unit.h"
#include "gold.h"
#include "Advances.h"
#include "AgreementData.h"
#include "TurnCnt.h"
#include "Civilisation.h"
#include "DiplomaticRequestData.h"
#include "AdvanceRecord.h"
#include "UnitPool.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "UnitData.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"
#include "ConstDB.h"

#include "Agreement.h"

#include "network.h"

#include "aicause.h"
#include "AgreementPool.h"

extern	Player	**g_player ;
extern	TurnCount	*g_turn ;
extern UnitPool *g_theUnitPool;
extern World *g_theWorld; 
extern ConstDB *g_theConstDB;

extern AgreementPool *g_theAgreementPool;

#include "Diplomacy_Log.h"
extern Diplomacy_Log *g_theDiplomacyLog; 








AgreementData::AgreementData(const ID id) : GAMEOBJ(id.m_id)
{
    Init(); 

	m_agreement = AGREEMENT_TYPE_NULL ;

	m_round = 0 ;
	m_expires = 0 ;
	m_owner = PLAYER_INDEX_INVALID ;
	m_recipient = PLAYER_INDEX_INVALID ;
	m_isBroken = FALSE;
	m_ownerPollution = 0;
	m_recipientPollution = 0;
}









AgreementData::AgreementData(const ID id, const PLAYER_INDEX owner, 
   const PLAYER_INDEX recipient, const AGREEMENT_TYPE agreement) : GAMEOBJ(id.m_id)
{
    Init(); 

    m_id = id; 
	m_agreement = agreement ;

	m_round = g_turn->GetRound() ;
	m_expires = k_EXPIRATION_NEVER ;
	m_owner = owner ;
	m_recipient = recipient ;
	m_isBroken = FALSE;

	if(g_player[owner]) {
		m_ownerPollution = g_player[owner]->GetCurrentPollution();
	} else {
		m_ownerPollution = 0;
	}

	if(g_player[recipient]) {
		m_recipientPollution = g_player[m_recipient]->GetCurrentPollution();
	} else {
		m_recipientPollution = 0;
	}

	ENQUEUE();
}

AgreementData::AgreementData(CivArchive &archive) : GAMEOBJ(0)
{
    Init(); 
    Serialize(archive); 
} 


void AgreementData::Init()
{ 
    m_owner = -1; 
	m_recipient = -1; 	
    m_thirdParty = -1;
	m_agreement = AGREEMENT_TYPE_NULL;		
  
	m_round = -1;
    m_expires = -1;	
	m_isBroken = FALSE;
    m_targetCity = Unit(0); 
}












void AgreementData::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE 

    GAMEOBJ::Serialize(archive); 
	uint8 hasChild;

	if (archive.IsStoring())
		{
		archive.PutSINT32(m_owner) ;
		archive.PutSINT32(m_recipient) ;
		archive.PutSINT32(m_thirdParty) ;

		archive.PutSINT32(m_agreement) ;


		archive<<m_round ;
		archive<<m_expires ;
		archive.PutSINT32(m_isBroken);

		m_targetCity.Serialize(archive) ;



		hasChild = m_lesser != NULL;
		archive << hasChild;
		if (m_lesser)
			((AgreementData *)(m_lesser))->Serialize(archive) ;

		hasChild = m_greater != NULL;
		archive << hasChild;
		if (m_greater)
			((AgreementData *)(m_greater))->Serialize(archive) ;

		}
	else
		{
		m_owner = (PLAYER_INDEX)(archive.GetSINT32()) ;
		m_recipient = (PLAYER_INDEX)(archive.GetSINT32()) ;
		m_thirdParty = (PLAYER_INDEX)(archive.GetSINT32()) ;

		m_agreement = (AGREEMENT_TYPE)(archive.GetSINT32()) ;

		archive>>m_round ;
		archive>>m_expires ;
		m_isBroken = (BOOL)(archive.GetSINT32());

		m_targetCity.Serialize(archive) ;


		archive >> hasChild;
		if(hasChild)
			m_lesser = new AgreementData(archive);
		else
			m_greater = NULL;
		archive >> hasChild;
		if(hasChild)
			m_greater = new AgreementData(archive);
		else
			m_greater = NULL;
		}
}

	







void AgreementData::MakeAgreement(PLAYER_INDEX owner, PLAYER_INDEX recipient, AGREEMENT_TYPE agreement)
	{
	m_owner = owner ;
	m_recipient = recipient ;
	m_agreement = agreement ;

	m_round = g_turn->GetRound() ;
	m_expires = k_EXPIRATION_NEVER ;



	m_thirdParty = PLAYER_INDEX_INVALID ;

#ifdef _DEBUG
        if (g_theDiplomacyLog) { 
            g_theDiplomacyLog->LogMakeAgreement(m_owner, m_recipient, m_thirdParty,
                m_agreement); 
        } 
#endif _DEBUG

	ENQUEUE();
}























































































































void AgreementData::SetTarget(const Unit &city)
	{
	m_targetCity = city ;
	}



























































void AgreementData::Dump(const sint32 i)
	{
	MBCHAR	s[_MAX_PATH] ;

	sprintf(s, "%d (%d) - P%d & P%d agree to ", i, m_expires, m_owner, m_recipient) ;
	switch (m_agreement)
		{
		case AGREEMENT_TYPE_DEMAND_STOP_TRADE :
			sprintf(s, "%s stop trade with P%d", s, m_thirdParty) ;
			break ;

		case AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS :
			sprintf(s, "%s leave the lands", s) ;
			break ;

		case AGREEMENT_TYPE_REDUCE_POLLUTION :
			sprintf(s, "%s reduce pollution", s) ;
			break ;

		case AGREEMENT_TYPE_CEASE_FIRE :
			sprintf(s, "%s cease fire", s) ;
			break ;

		case AGREEMENT_TYPE_PACT_CAPTURE_CITY :
			sprintf(s, "%s capture city %d", s, m_targetCity) ;
			break ;

		case AGREEMENT_TYPE_PACT_END_POLLUTION :
			sprintf(s, "%s end pollution", s) ;
			break ;

		default :
			sprintf(s, "%s \"Unknown diplomatic agreement type\"", s) ;
			break ;

		}

	DPRINTF(k_DBG_INFO, ("%s\n", s)) ;
	}



















































void AgreementData::ExtractPlayer(sint32 indexId, sint32 memberId, MBCHAR *sExpanded)
	{
	Civilisation	*civ ;

	if (indexId >= 2)
		{
		if (m_agreement != AGREEMENT_TYPE_DEMAND_STOP_TRADE)
			
			{
			c3errors_ErrorDialogFromDB("AGREEMENT_ERROR", "AGREEMENT_ERROR_NOT_DEMAND_STOP_TRADE") ;
			return;
			}

		}

	switch (indexId)
		{
		case 0 :
			civ = g_player[m_owner]->GetCivilisation() ;
			break ;

		case 1 :
			civ = g_player[m_recipient]->GetCivilisation() ;
			break ;

		case 2 :
			civ = g_player[m_thirdParty]->GetCivilisation() ;
			break ;

		default :
			c3errors_ErrorDialogFromDB("AGREEMENT_ERROR", "AGREEMENT_ERROR_INDEX_OUT_OF_BOUNDS") ;
			break ;

		}

	switch (memberId)
		{
		case 0 :													
			strcpy(sExpanded, civ->GetLeaderName()) ;
			break ;

		case 1 :													
			civ->GetSingularCivName(sExpanded) ;
			break ;

		case 2 :													
			civ->GetPluralCivName(sExpanded) ;
			break ;

		case 3 :													
			civ->GetCountryName(sExpanded) ;
			break ;

		case 4 :													
			sprintf(sExpanded, "%ld", g_player[civ->GetOwner()]->GetGold()) ;
			break ;

		default :
			c3errors_ErrorDialogFromDB("AGREEMENT_ERROR", "AGREEMENT_ERROR_UNKNOWN_PLAYER_MEMBER") ;	
			return;

		}

	}











































































































































































































void AgreementData::Interpret(MBCHAR *msg, MBCHAR *sInterpreted)
	{
#define MAX_MEMBERS	15
	struct
		{
		MBCHAR	*sClass ;											
		
		MBCHAR	*sMember[MAX_MEMBERS] ;								
		} varList[]={	{ "city",		{ "name", "population", "happiness", "production", "food", "pos", "leader_name", "civ_name_singular", "civ_name_plural", "country_name", "slaves", NULL } },
						{ "gold",		{ "amount", NULL } },
						{ "player",		{ "leader_name", "civ_name_singular", "civ_name_plural", "country_name", "gold", NULL } },
						{ "advance",	{ "name", "index", "prerequisites", "leadsto", NULL } },
						{ NULL } } ;

	MBCHAR	*pInput,
			*pOutput,
			*pToken,
			*p,
			sClass[_MAX_PATH],
			sNum[_MAX_PATH],
			sMember[_MAX_PATH],
			sExpanded[_MAX_PATH],
			sToken[_MAX_PATH] ;

	sint32	classId,
			memberId,
			indexId ;


	
	pInput = msg ;
	pOutput = sInterpreted ;
	while (*pInput)
		{
		
		if (*pInput == '[')											
			{
			pToken = sToken ;										
			pInput++ ;												
			while ((*pInput != ']') && *pInput)						
				*pToken++ = *pInput++ ;								

			if (!pInput) {
				c3errors_ErrorDialog("error", "malformed class string") ;
				strcpy(sInterpreted, msg);
				return;
			}

			pInput++ ;												
			*pToken = NULL ;										
			pToken = sToken;

			
			p = sClass ;											
			while (*pToken && (*pToken!='.'))						
				*p++=*pToken++ ;

			*p = NULL ;												
			pToken++ ;												

			
			if (strchr(pToken, '.'))								
				{
				p = sNum ;											
				while (*pToken && (*pToken!='.'))					
					*p++=*pToken++ ;

				*p = NULL ;											
				if (!pToken) {										
					c3errors_ErrorDialog("error", "malformed class string") ;
					strcpy(sInterpreted, msg);
					return;
				}

				pToken++ ;											
				}
			else
				sNum[0] = NULL ;

			
			strcpy(sMember, pToken) ;								

			
			for (classId=0; varList[classId].sClass && stricmp(varList[classId].sClass, sClass); classId++)
				;

			Assert(varList[classId].sClass) ;
			if (!varList[classId].sClass) {							
				c3errors_ErrorDialogFromDB("AGREEMENT_ERROR", "MESSAGE_UNKNOWN_VARIABLE") ;	
				strcpy(sInterpreted, msg);
				return;
			}

			
			if (sNum[0])
				{
				for (p=sNum; *p && isdigit(*p); p++) ;					
			
				if (*p) {
					c3errors_ErrorDialogFromDB("AGREEMENT_ERROR", "AGREEMENT_ERROR_INDEX_OUT_OF_BOUNDS") ;	
					strcpy(sInterpreted, msg);
					return;
				}

				indexId = atoi(sNum) ;									
				Assert(indexId>0) ;
				indexId-- ;
				if (indexId<0) {
					c3errors_ErrorDialogFromDB("AGREEMENT_ERROR", "AGREEMENT_ERROR_INDEX_OUT_OF_BOUNDS") ;	
					strcpy(sInterpreted, msg);
					return;
				}

				}
			else
				indexId = 0 ;

			
			for (memberId=0; varList[memberId].sMember 
								&& varList[classId].sMember[memberId] 
								&& stricmp(varList[classId].sMember[memberId], sMember); memberId++) ;

			if (!varList[memberId].sMember)	{						
				c3errors_ErrorDialogFromDB("AGREEMENT_ERROR", "MESSAGE_UNKNOWN_VARIABLE") ;	
				strcpy(sInterpreted, msg);
				return;
			}

			
			

			switch (classId)
				{
				case 0 :											
					
					Assert(FALSE) ;

					break ;

				case 1 :											
					
					Assert(FALSE) ;

					break ;

				case 2 :											
					ExtractPlayer(indexId, memberId, sExpanded) ;
					break ;

				case 3 :											
					
					Assert(FALSE) ;

					break ;

				default :											
					c3errors_ErrorDialogFromDB("AGREEMENT_ERROR", "AGREEMENT_ERROR_UNKNOWN_VARIABLE") ;	
					strcpy(sInterpreted, msg);
					return;
				}

			strcpy(pOutput, sExpanded) ;							

			pOutput += strlen(sExpanded) ;
			}
		else
			*pOutput++ = *pInput++ ;								

		}

	*pOutput = NULL ;												
	}











void AgreementData::ToString(MBCHAR *s)
	{
	MBCHAR	msg[_MAX_PATH] ;

	switch (m_agreement)
		{
		case AGREEMENT_TYPE_DEMAND_ADVANCE :
			strcpy(msg, "DIPxxx3_AGREEMENT_DEMAND_ADVANCE") ;
			break ;

		case AGREEMENT_TYPE_DEMAND_CITY :
			strcpy(msg, "DIPxxx3_AGREEMENT_DEMAND_CITY") ;
			break ;

		case AGREEMENT_TYPE_DEMAND_MAP :
			strcpy(msg, "DIPxxx3_AGREEMENT_DEMAND_MAP") ;
			break ;

		case AGREEMENT_TYPE_DEMAND_GOLD :
			strcpy(msg, "DIPxxx3_AGREEMENT_DEMAND_GOLD") ;
			break ;

		case AGREEMENT_TYPE_DEMAND_ATTACK_ENEMY :
			strcpy(msg, "DIPxxx3_AGREEMENT_DEMAND_ATTACK_ENEMY") ;
			break ;

		case AGREEMENT_TYPE_OFFER_ADVANCE :
			strcpy(msg, "DIPxxx3_AGREEMENT_OFFER_ADVANCE") ;
			break ;

		case AGREEMENT_TYPE_OFFER_CITY :
			strcpy(msg, "DIPxxx3_AGREEMENT_OFFER_CITY") ;
			break ;

		case AGREEMENT_TYPE_OFFER_MAP :
			strcpy(msg, "DIPxxx3_AGREEMENT_OFFER_MAP") ;
			break ;

		case AGREEMENT_TYPE_OFFER_GOLD :
			strcpy(msg, "DIPxxx3_AGREEMENT_OFFER_GOLD") ;
			break ;

		case AGREEMENT_TYPE_EXCHANGE_ADVANCE :
			strcpy(msg, "DIPxxx3_AGREEMENT_EXCHANGE_ADVANCE") ;
			break ;

		case AGREEMENT_TYPE_EXCHANGE_CITY :
			strcpy(msg, "DIPxxx3_AGREEMENT_EXCHANGE_CITY") ;
			break ;

		case AGREEMENT_TYPE_EXCHANGE_MAP :
			strcpy(msg, "DIPxxx3_AGREEMENT_EXCHANGE_MAP") ;
			break ;

		case AGREEMENT_TYPE_NO_PIRACY :
			strcpy(msg, "DIPxxx3_AGREEMENT_NO_PIRACY") ;
			break ;

		case AGREEMENT_TYPE_DEMAND_STOP_TRADE :
			strcpy(msg, "DIPxxx1_AGREEMENT_STOP_TRADING_WITH") ;
			break ;

		case AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS :
			strcpy(msg, "DIPxxx2_AGREEMENT_LEAVE_OUR_LANDS") ;
			break ;

		case AGREEMENT_TYPE_REDUCE_POLLUTION :
			strcpy(msg, "DIPxxx3_AGREEMENT_REDUCE_POLLUTION") ;
			break ;

		case AGREEMENT_TYPE_CEASE_FIRE :
			strcpy(msg, "DIPxxx3_AGREEMENT_CEASE_FIRE") ;
			break ;

		case AGREEMENT_TYPE_PACT_CAPTURE_CITY :
			strcpy(msg, "DIPxxx3_AGREEMENT_CAPTURE_CITY") ;
			break ;

		case AGREEMENT_TYPE_PACT_END_POLLUTION :
			strcpy(msg, "DIPxxx3_AGREEMENT_END_POLLUTION") ;
			break ;

		default :
			Assert(FALSE) ;
			
			c3errors_ErrorDialogFromDB("AGREEMENT_ERROR", "AGREEMENT_ERROR_UNKNOWN_AGREEMENT") ;	
			sprintf(s, "%s \"Unknown diplomatic agreement type\"", s) ;
			break ;

		}

	Interpret(msg, s) ;
	}













sint32 AgreementData::DecrementTurns(void)
	{
	if (m_expires == k_EXPIRATION_NEVER)
		return (k_EXPIRATION_NEVER) ;

	if (m_expires>0)
		m_expires-- ;

	ENQUEUE();
	return (m_expires) ;
	}











void AgreementData::EndTurn(void)
	{
	DecrementTurns() ;
	ENQUEUE();
	}



void AgreementData::RecipientIsViolating(PLAYER_INDEX curPlayer, BOOL force)
{
	
	char objName[256];
	BOOL sendMessage = FALSE;
	sint32 now = g_turn->GetRound();
	sint32 rounds = now - m_round;
	BOOL tellAi = FALSE;
	sint32 otherCiv = -1;
	BOOL addCity = FALSE;

	switch(m_agreement) {
		case AGREEMENT_TYPE_DEMAND_STOP_TRADE:
			tellAi = TRUE;
			if(rounds > g_theConstDB->StopTradeRounds()) {
				sendMessage = TRUE;
				sprintf(objName, "008StopTradeWithBroken");
				otherCiv = m_thirdParty;
			}
			break;
		case AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS:
			tellAi = TRUE;
			if(rounds > g_theConstDB->LeaveOurLandsRounds() || force) {
				sendMessage = TRUE;
				sprintf(objName, "260LeaveOurLandsBroken");
			}
			break;
		case AGREEMENT_TYPE_REDUCE_POLLUTION:			
			tellAi = TRUE;
			if(rounds > g_theConstDB->ReducePollutionRounds()) {
				sendMessage = TRUE;
				sprintf(objName, "261ReducePollutionBroken");
			}
			break;
#if 0
		case AGREEMENT_TYPE_PACT_CAPTURE_CITY:
			tellAi = TRUE;
			if(rounds > g_theConstDB->CaptureCityRounds()) {
				sendMessage = TRUE;
				sprintf(objName, "262CaptureCityBroken");
				addCity = TRUE;
			}
			break;
#endif
		case AGREEMENT_TYPE_PACT_END_POLLUTION:
			tellAi = TRUE;
			if(rounds > g_theConstDB->EndPollutionRounds()) {
				sendMessage = TRUE;
				sprintf(objName, "263EndPollutionBroken");
			}
			break;
		case AGREEMENT_TYPE_DEMAND_ATTACK_ENEMY:
			tellAi = TRUE;
			if(rounds > g_theConstDB->AttackEnemyRounds()) {
				sendMessage = TRUE;
				sprintf(objName, "264AttackEnemyBroken");
				otherCiv = m_thirdParty;
			}
			break;
		case AGREEMENT_TYPE_NO_PIRACY:
			tellAi = TRUE;
			sendMessage = TRUE;
			sprintf(objName, "004NoPiracyBroken");
			break;
		default:
			break;
	}

	if(tellAi) {
		
		
		
		
		
	}

	if(sendMessage) {
		SlicObject *so1 = new SlicObject(objName);
		strcat(objName, "ByYou");
		SlicObject *so2 = new SlicObject(objName);

		so1->AddCivilisation(m_recipient);
		so1->AddRecipient(m_owner);

		so2->AddCivilisation(m_owner);
		so2->AddRecipient(m_recipient);
		if(otherCiv >= 0) {
			so1->AddCivilisation(otherCiv);
			so2->AddCivilisation(otherCiv);
		}
		if(addCity) {
			so1->AddCity(m_targetCity);
			so2->AddCity(m_targetCity);
		}
		g_slicEngine->Execute(so1);
		if(g_slicEngine->GetSegment(objName)) {
			g_slicEngine->Execute(so2);
		} else {
			delete so2;
		}

		

		Agreement me(m_id);
		if(g_theAgreementPool->IsValid(me)) {
			me.Kill();
		}
	}
}

void AgreementData::OwnerIsViolating(PLAYER_INDEX curPlayer)
{
	
	char objName[256];
	BOOL sendMessage = FALSE;
	sint32 now = g_turn->GetRound();
	sint32 rounds = now - m_round;
	BOOL tellAi = FALSE;
	sint32 otherCiv = -1;
	BOOL addCity = FALSE;

	
	switch(m_agreement) {
		case AGREEMENT_TYPE_PACT_END_POLLUTION:
			
			tellAi = TRUE;
			if(rounds > g_theConstDB->EndPollutionRounds()) {
				sendMessage = TRUE;
				sprintf(objName, "263EndPollutionBroken");
			}
			break;
		default:
			break;
	}

	if(tellAi) {
		
		
		
		
		
	}

	if(sendMessage) {
		SlicObject *so1 = new SlicObject(objName);
		strcat(objName, "ByYou");
		SlicObject *so2 = new SlicObject(objName);

		so1->AddCivilisation(m_owner);
		so1->AddRecipient(m_recipient);

		so2->AddCivilisation(m_recipient);
		so2->AddRecipient(m_owner);
		if(otherCiv >= 0) {
			so1->AddCivilisation(otherCiv);
			so2->AddCivilisation(otherCiv);
		}
		if(addCity) {
			so1->AddCity(m_targetCity);
			so2->AddCity(m_targetCity);
		}
		g_slicEngine->Execute(so1);
		g_slicEngine->Execute(so2);

		

		Agreement me(m_id);
		if(g_theAgreementPool->IsValid(me)) {
			me.Kill();
		}
	}
}

void AgreementData::BeginTurnOwner()
{
	
	if(!g_player[m_recipient] || g_player[m_recipient]->m_isDead)
		return;

	switch(m_agreement) {
		case AGREEMENT_TYPE_DEMAND_STOP_TRADE:
		{
			if(g_player[m_recipient]) {
				
				sint32 trade = g_player[m_recipient]->GetTradeWith(m_thirdParty);
				if(trade > 0) {
					RecipientIsViolating(m_owner);
				}
			}
			break;
		}
		case AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS:
		{
			if(g_player[m_recipient]) {
				
				DynamicArray<Army> *armies = g_player[m_recipient]->m_all_armies;
				sint32 i, n = armies->Num();
				for(i = 0; i < n; i++) {
					MapPoint pos;
					sint32 j;
					BOOL hasAttackUnits = FALSE;
					for(j = 0; j < armies->Access(i).Num(); j++) {
						if(armies->Access(i).Access(j).GetAttack() > 0.00001) {
							hasAttackUnits = TRUE;
							break;
						}
					}
					if(!hasAttackUnits)
						continue;

					armies->Access(i).GetPos(pos);
					if(g_theWorld->GetCell(pos)->GetOwner() == m_owner) {
						RecipientIsViolating(m_owner);
						return;
					}
				}
			}
			break;
		}
		case AGREEMENT_TYPE_REDUCE_POLLUTION:
		{
			if(g_player[m_recipient]) {
				if(g_player[m_recipient]->GetCurrentPollution() >= m_recipientPollution) {
					RecipientIsViolating(m_owner);
				}
			}
			break;
		}
#if 0
		case AGREEMENT_TYPE_PACT_CAPTURE_CITY:
		{
			if(g_player[m_recipient]) {
				if(g_theUnitPool->IsValid(m_targetCity)) {
					if(m_targetCity.GetOwner() == m_owner ||
					   m_targetCity.GetOwner() == m_recipient) {
						
					}
				}
			}
			break;
		}
#endif
		case AGREEMENT_TYPE_PACT_END_POLLUTION:
		{
			if(g_player[m_owner]) {
				sint32 now = g_turn->GetRound();
				sint32 rounds = now - m_round;
				Agreement me(m_id);
				if(g_player[m_owner]->GetCurrentPollution() > m_ownerPollution &&
				   g_player[m_owner]->GetCurrentPollution() > uint32(g_theConstDB->MinEcoPactViolationLevel())) {
					OwnerIsViolating(m_owner);
				}

				
				
				if(g_theAgreementPool->IsValid(me)) {
					if(rounds > g_theConstDB->EndPollutionRounds()) {
						m_ownerPollution = g_player[m_owner]->GetCurrentPollution();
						if(g_player[m_recipient]) {
							m_recipientPollution = g_player[m_recipient]->GetCurrentPollution();
						}
						
						m_round = now;
						if(g_network.IsHost())
							g_network.Enqueue(this);
					}
				}
			}
			break;
		}
		case AGREEMENT_TYPE_DEMAND_ATTACK_ENEMY:
		{
			if(g_player[m_recipient]) {
				if(g_player[m_recipient]->GetLastAttacked(m_thirdParty) < m_round) {
					RecipientIsViolating(m_owner);
				}
			}
			break;
		}
		default:
			
			
			return;
	}
}

void AgreementData::Break()

{ 
    m_isBroken = TRUE;


    switch(m_agreement) {
		case AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS:
            
            
            
            break; 
        default:
            break;
    }

#ifdef _DEBUG
        if (g_theDiplomacyLog) { 
            g_theDiplomacyLog->LogBrokenAgreement(m_owner, m_recipient, m_thirdParty,
                m_agreement); 
        } 
#endif _DEBUG

} 

void AgreementData::BeginTurnRecipient()
{
	
	return;

	
	if(!g_player[m_owner] || g_player[m_owner]->m_isDead)
		return;

	switch(m_agreement) {
		case AGREEMENT_TYPE_DEMAND_STOP_TRADE:
		{
			if(g_player[m_recipient]) {
				
				sint32 trade = g_player[m_recipient]->GetTradeWith(m_thirdParty);
				if(trade > 0) {
					RecipientIsViolating(m_recipient);
				}
			}
			break;
		}
		case AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS:
		{
			if(g_player[m_recipient]) {
				
				DynamicArray<Army> *armies = g_player[m_recipient]->m_all_armies;
				sint32 i, n = armies->Num();
				for(i = 0; i < n; i++) {
					MapPoint pos;
					sint32 j;
					BOOL hasAttackUnits = FALSE;
					for(j = 0; j < armies->Access(i).Num(); j++) {
						if(armies->Access(i).Access(j).GetAttack() > 0.00001) {
							hasAttackUnits = TRUE;
							break;
						}
					}
					if(!hasAttackUnits)
						continue;

					armies->Access(i).GetPos(pos);
					if(g_theWorld->GetCell(pos)->GetOwner() == m_owner) {
						RecipientIsViolating(m_recipient);
						return;
					}
				}
			}
			break;
		}
		case AGREEMENT_TYPE_REDUCE_POLLUTION:
		{
			if(g_player[m_recipient]) {
				if(g_player[m_recipient]->GetCurrentPollution() >= m_recipientPollution) {
					RecipientIsViolating(m_recipient);
				}
			}
			break;
		}
#if 0
		case AGREEMENT_TYPE_PACT_CAPTURE_CITY:
		{
			if(g_player[m_recipient]) {
				if(g_theUnitPool->IsValid(m_targetCity)) {
					if(m_targetCity.GetOwner() == m_owner ||
					   m_targetCity.GetOwner() == m_recipient) {
						
					}
				}
			}
			break;
		}
#endif
		case AGREEMENT_TYPE_PACT_END_POLLUTION:
		{
			if(g_player[m_recipient]) {
				sint32 now = g_turn->GetRound();
				sint32 rounds = now - m_round;
				Agreement me(m_id);

				if(g_player[m_recipient]->GetCurrentPollution() > m_recipientPollution &&
				   g_player[m_recipient]->GetCurrentPollution() > uint32(g_theConstDB->MinEcoPactViolationLevel())) {
					RecipientIsViolating(m_recipient);
				}

				
				
				if(g_theAgreementPool->IsValid(me)) {
					if(rounds > g_theConstDB->EndPollutionRounds()) {
						m_ownerPollution = g_player[m_owner]->GetCurrentPollution();
						if(g_player[m_recipient]) {
							m_recipientPollution = g_player[m_recipient]->GetCurrentPollution();
						}
						
						m_round = now;
						if(g_network.IsHost())
							g_network.Enqueue(this);
					}
				}
			}
			break;
		}
		case AGREEMENT_TYPE_DEMAND_ATTACK_ENEMY:
		{
			if(g_player[m_recipient]) {
				if(g_player[m_recipient]->GetLastAttacked(m_thirdParty) < m_round) {
					RecipientIsViolating(m_recipient);
				}
			}
			break;
		}
		default:
			
			
			
			
			return;
	}
}


void AgreementData::SetExpires(sint32 turns)
{ 
	Assert((turns == k_EXPIRATION_NEVER) || (turns > 0)) ; 
	m_expires = turns ; 
	ENQUEUE();
}
