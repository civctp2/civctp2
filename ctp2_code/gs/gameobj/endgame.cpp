
#include "c3.h"
#include "EndGame.h"
#include "civarchive.h"
#include "EndGameDB.h"
#include "TurnCnt.h"
#include "player.h"

#include "SlicObject.h"
#include "SlicEngine.h"
#include "SlicSegment.h"
#include "RandGen.h"
#include "UnitDynArr.h"
#include "UnitData.h"
#include "citydata.h"
#include "screenutils.h"
#include "SelItem.h"
#include "EndgameWindow.h"
#include "Advances.h"
#include "AdvanceRecord.h"
#include "network.h"
#include "net_endgame.h"
#include "StrDB.h"
#include "net_info.h"
#include "Score.h"
#include "GameSettings.h"
#include "advanceutil.h"
#include "gstypes.h"

extern TurnCount *g_turn;
extern Player **g_player;
extern RandomGenerator *g_rand;
extern SelectedItem *g_selected_item;
extern StringDB *g_theStringDB;

EndGame::EndGame(PLAYER_INDEX owner)
{
	m_owner = owner;
	Init();
}

EndGame::EndGame(CivArchive &archive)
{
	Serialize(archive);
}

EndGame::~EndGame()
{
	if(m_numBuilt)
		delete [] m_numBuilt;
	if (m_savedNumBuilt)
		delete [] m_savedNumBuilt;
}

void EndGame::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive.StoreChunk((uint8*)&m_owner, (uint8*)&m_currentStageBegan + sizeof(m_currentStageBegan));
		archive.Store((uint8*)m_numBuilt, g_theEndGameDB->m_nRec * sizeof(sint32));
		archive.Store((uint8*)m_savedNumBuilt, g_theEndGameDB->m_nRec * sizeof(sint32));
	} else {
		archive.LoadChunk((uint8*)&m_owner, (uint8*)&m_currentStageBegan + sizeof(m_currentStageBegan));

		
		
		if (m_numBuilt)
			delete [] m_numBuilt;
		if (m_savedNumBuilt)
			delete [] m_savedNumBuilt;
		m_numBuilt = new sint32[g_theEndGameDB->m_nRec];
		m_savedNumBuilt = new sint32[g_theEndGameDB->m_nRec];
		archive.Load((uint8*)m_numBuilt, g_theEndGameDB->m_nRec * sizeof(sint32));
		archive.Load((uint8*)m_savedNumBuilt, g_theEndGameDB->m_nRec * sizeof(sint32));
	}
}

void EndGame::Init()
{
	m_currentStage = -1;
	m_currentStageBegan = -1;

	m_numBuilt = new sint32[g_theEndGameDB->m_nRec];
	m_savedNumBuilt = new sint32[g_theEndGameDB->m_nRec];

    sint32 i; 
    for (i=0; i < g_theEndGameDB->m_nRec; i++) { 
        m_numBuilt[i] = 0; 
		m_savedNumBuilt[i] = 0;
    }
}

void EndGame::AddObject(sint32 type)
{
	const EndGameRecord *egrec = g_theEndGameDB->Get(type);
	if(egrec->NotifyLabBuilt()) {
		SlicObject *so = new SlicObject("302EndGameOtherCivBuiltLab");
		so->AddAllRecipientsBut(m_owner);
		so->AddCivilisation(m_owner);
		g_slicEngine->Execute(so);

		if(g_network.IsHost()) {
			g_network.Block(m_owner);
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_OTHER_CIV_LAB_MSG,
										  m_owner));
			g_network.Unblock(m_owner);
		}
			
		so = new SlicObject("308EndGameFinishedXLab");
		so->AddRecipient(m_owner);
		g_slicEngine->Execute(so);
	}

	m_numBuilt[type]++;

	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.QueuePacketToAll(new NetEndGame(m_owner));
		g_network.Unblock(m_owner);
	}

	
	if(m_owner == g_selected_item->GetVisiblePlayer()) {
		
		
		if(g_endgameWindow) g_endgameWindow->Update(this);
		else open_EndGame();
	}
}

void EndGame::ClearAll()
{
	sint32 i;

	m_currentStage = -1;
	m_currentStageBegan = -1;

	for(i = 0; i < g_theEndGameDB->m_nRec;  i++) {
		m_numBuilt[i] = 0;
		m_savedNumBuilt[i] = 0;
	}
}


BOOL EndGame::BeginSequence()
{
	sint32 i;
	for(i = 0; i < g_theEndGameDB->m_nRec; i++) {
		
		if(g_theEndGameDB->Get(i)->ExactlyOneRequired() &&
		   m_numBuilt[i] < 1)
			return FALSE;
	}

	SlicObject *so = new SlicObject("303EndGameOtherCivStartedSequence");
	so->AddAllRecipientsBut(m_owner);
	so->AddCivilisation(m_owner);
	g_slicEngine->Execute(so);
	
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_OTHER_CIV_SEQUENCE_MSG,
									  m_owner));
	}


	m_currentStage = 0;
	m_currentStageBegan = g_turn->GetRound();

	
	if(m_owner == g_selected_item->GetVisiblePlayer()) {
		
		
		if(g_endgameWindow) g_endgameWindow->Update(this);
		else open_EndGame();
	}

	return TRUE;
}

void EndGame::BeginTurn()
{
	SlicObject *so;

	if(m_currentStage < 0 || m_currentStage >= g_theEndGameDB->GetNumStages())
		return;

	
	if(GetCataclysmChance() > 0) {
		double cataclysm_chance = GetCataclysmChance();
		cataclysm_chance /= GetTurnsForNextStage();
		cataclysm_chance *= 10;
		if(g_rand->Next(1000) < sint32(cataclysm_chance)) {
			Cataclysm();
			return;
		}
	}

	sint32 turnsForNextStage = GetTurnsForNextStage();
    if ((turnsForNextStage >= 0) &&	   
        (g_turn->GetRound() > m_currentStageBegan + turnsForNextStage)) {
        if (MetRequirementsForNextStage()) {
            AdvanceStage();
        } else {
			if (!HaveEnoughECDs() &&
				(g_slicEngine->GetSegment("061NeedEcd")->TestLastShown(m_owner, 5))) {
				so = new SlicObject("061NeedEcd");
				so->AddRecipient(m_owner);
				g_slicEngine->Execute(so);
			}

			if(!HaveEnoughFields() &&
			   (g_slicEngine->GetSegment("062NeedField")->TestLastShown(m_owner, 5))) {
				so = new SlicObject("062NeedField");
				so->AddRecipient(m_owner);
				g_slicEngine->Execute(so);
			}

			if(!HaveMaxSplicers() &&
			   (g_slicEngine->GetSegment("063ShouldBuildSplicer")->TestLastShown(m_owner, 5))) {
				so = new SlicObject("063ShouldBuildSplicer");
				so->AddRecipient(m_owner);
				g_slicEngine->Execute(so);
			}
		}
	} else if(m_currentStage == 2 && 
			  turnsForNextStage >= 0 &&
			  (((m_currentStageBegan + turnsForNextStage) - 
				g_turn->GetRound()) < 5)) {
		if(g_slicEngine->GetSegment("053AlienAlmostDone")->TestLastShown(m_owner, 5)) {
			so = new SlicObject("053AlienAlmostDone");
			so->AddRecipient(m_owner);
			g_slicEngine->Execute(so);

			so = new SlicObject("054AlienAlmostDoneOthers");
			so->AddAllRecipientsBut(m_owner);
			so->AddCivilisation(m_owner);
			g_slicEngine->Execute(so);

			if(g_network.IsHost()) {
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_ALIEN_ALMOST_DONE_OTHERS_MSG,
											  m_owner));
			}
		}
	}

	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.QueuePacketToAll(new NetEndGame(m_owner));
		g_network.Unblock(m_owner);
	}
}

void EndGame::AdvanceStage()
{
	SlicObject *so;

	
	

#if 0
	
	
	sint32 cataclysm_chance = GetCataclysmChance();
	if(cataclysm_chance > 0) {
		if(g_rand->Next(100) < cataclysm_chance) {
			Cataclysm();
			return;
		}
	}
#endif

	bool openScreen = true;
	
	m_currentStage++;
	if(m_currentStage >= g_theEndGameDB->GetNumStages()) {
		
		SlicObject *so = new SlicObject("309EndGameWon");
		so->AddAllRecipientsBut(m_owner);
		so->AddCivilisation(m_owner);
		g_slicEngine->Execute(so);
		if(g_network.IsHost()) {
			g_network.Block(m_owner);
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_WON_END_GAME,
										  m_owner));
			g_network.Unblock(m_owner);
		}

		g_player[m_owner]->m_score->SetWonByWonder();
		g_player[m_owner]->GameOver(GAME_OVER_WON_WORMHOLE, -1);
		m_currentStage = g_theEndGameDB->GetNumStages();
		openScreen = false;
		g_theGameSettings->SetAlienEndGameWon(m_owner);

		sint32 i;
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i] && i != m_owner) {
				g_player[i]->GameOver(GAME_OVER_LOST_OVERRUN_BY_SMURFS, m_owner);
			}
		}
	}
	m_currentStageBegan = g_turn->GetRound();

	switch (m_currentStage) {
	  case 1: {
		  so = new SlicObject("059EmbryoStage2");
		  so->AddRecipient(m_owner);
		  g_slicEngine->Execute(so);

		  
		  
		  
		  so = new SlicObject("058AlienScrappedOwner");
		  so->AddAllRecipientsBut(m_owner);
		  so->AddCivilisation(m_owner);
		  g_slicEngine->Execute(so);

		  if(g_network.IsHost()) {
			  g_network.Enqueue(new NetInfo(NET_INFO_CODE_ALIEN_SCRAPPED_OWNER,
											m_owner));
		  }
		  break;
	  }
	  case 2: {
		  so = new SlicObject("060EmbryoStage3");
		  so->AddRecipient(m_owner);
		  g_slicEngine->Execute(so);
		  break;
	  }
	}

    
	if(openScreen && m_owner == g_selected_item->GetVisiblePlayer()) {
		
		
		if(g_endgameWindow) g_endgameWindow->Update(this);
		else open_EndGame();
	}
}

sint32 EndGame::GetCataclysmChance()
{
	
	
	
	sint32 i;
	sint32 maxChance = 0;

	for(i = 0; i < g_theEndGameDB->m_nRec; i++) {
		const EndGameRecord *egrec = g_theEndGameDB->Get(i);
		if(egrec->GetCataclysmNum() > 0) {
			sint32 chance = (egrec->GetCataclysmNum() - m_numBuilt[i]) * egrec->GetCataclysmPercent();
			if(chance > maxChance)
				maxChance = chance;
		}
	}
	return maxChance;
}

sint32 EndGame::GetTurnsForNextStage()
{
	
	
	sint32 i;
	sint32 min = -1;
	for(i = 0; i < g_theEndGameDB->m_nRec; i++) {
		const EndGameRecord *egrec = g_theEndGameDB->Get(i);
		if(egrec->ControlsSpeed()) {
			if(m_numBuilt[i] < 0) {
				
				continue;
			}
			if(min < 0 || egrec->GetTurnsPerStage(m_numBuilt[i]) < min) {
				min = egrec->GetTurnsPerStage(m_numBuilt[i]);
			}
		}
	}
	return min;
}

BOOL EndGame::HaveEnoughECDs()
{
    sint32 i;
    for(i = 0; i < g_theEndGameDB->m_nRec; i++) {
        const EndGameRecord *egrec = g_theEndGameDB->Get(i);
		if(strcmp(g_theStringDB->GetIdStr(egrec->m_name), "ET_COMMUNICATION_DEVICE"))
			continue;

        
        if(egrec->RequiredToAdvanceFromStage(m_currentStage) > m_numBuilt[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL EndGame::HaveEnoughFields()
{
    sint32 i;
    for(i = 0; i < g_theEndGameDB->m_nRec; i++) {
        const EndGameRecord *egrec = g_theEndGameDB->Get(i);
		if(strcmp(g_theStringDB->GetIdStr(egrec->m_name), "CONTAINMENT_FIELD"))
			continue;

        
        if(egrec->RequiredToAdvanceFromStage(m_currentStage) > m_numBuilt[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL EndGame::HaveMaxSplicers()
{
    sint32 i;
    for(i = 0; i < g_theEndGameDB->m_nRec; i++) {
        const EndGameRecord *egrec = g_theEndGameDB->Get(i);
		if(strcmp(g_theStringDB->GetIdStr(egrec->m_name), "GENE_SEQUENCER"))
			continue;

        
        if(m_numBuilt[i] < egrec->GetMaxAllowed()) {
            return FALSE;
        } else {
			return TRUE;
		}
    }
    return TRUE;
}

BOOL EndGame::HaveAllPrerequisites()
{
    sint32 i;
    for(i = 0; i < g_theEndGameDB->m_nRec; i++) {
        const EndGameRecord *egrec = g_theEndGameDB->Get(i);
        
        if(egrec->RequiredToAdvanceFromStage(m_currentStage) > m_numBuilt[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL EndGame::MetRequirementsForNextStage()
{
	
	

	if(m_currentStage >= g_theEndGameDB->GetNumStages()) {
		
		return TRUE;
	}

	if(m_currentStage >= g_theEndGameDB->GetNumStages() - 1) {
		
		
		if(!g_player[m_owner]->m_advances->HasAdvance(advanceutil_GetAlienLifeAdvance())) {
			return FALSE;
		}
	}

	if (!HaveAllPrerequisites()) {
		return FALSE;
	}

	return TRUE;
}

void EndGame::Cataclysm()
{
	
	SlicObject *so = new SlicObject("300EndGameCataclysm");
	so->AddRecipient(m_owner);
	g_slicEngine->Execute(so);

	so = new SlicObject("301EndGameCataclysmOtherCiv");
	so->AddAllRecipientsBut(m_owner);
	so->AddCivilisation(m_owner);
	g_slicEngine->Execute(so);

	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_CATACLYSM_OTHER,
									  m_owner));
	}

	Init();

	sint32 i;
	for(i = 0; i < g_player[m_owner]->m_all_cities->Num(); i++) {
		g_player[m_owner]->m_all_cities->Access(i).AccessData()->GetCityData()->RemoveEndGameObjects();
	}
	

	
	if(m_owner == g_selected_item->GetVisiblePlayer()) {
		close_EndGame();
	}
}

void EndGame::XLabCaptured()
{
	Init();
	sint32 i;
	for(i = 0; i < g_player[m_owner]->m_all_cities->Num(); i++) {
		g_player[m_owner]->m_all_cities->Access(i).AccessData()->GetCityData()->RemoveEndGameObjects();
	}
	

	
	if(m_owner == g_selected_item->GetVisiblePlayer()) {
		close_EndGame();
	}
}


sint32 EndGame::GetTurnsSinceStageBegan()
{
	return(g_turn->GetRound() - m_currentStageBegan);
}


sint32 EndGame::GetStage()
{
	return(m_currentStage);
}

sint32 EndGame::GetNumberBuilt(sint32 type)
{
	return(m_numBuilt[type]);
}


sint32 EndGame::GetDisplayedStage()
{
	return(m_savedCurrentStage);
}

sint32 EndGame::GetNumberShown(sint32 type)
{
	return(m_savedNumBuilt[type]);
}




void EndGame::UpdateDisplayState(void)
{
	sint32 i; 
    
	m_savedCurrentStage = m_currentStage;

	for (i=0; i < g_theEndGameDB->m_nRec; i++) { 
		m_savedNumBuilt[i] = m_numBuilt[i];
    }
}

BOOL EndGame::HasLab()
{
	sint32 i;
	for(i = 0; i < g_theEndGameDB->m_nRec; i++) {
		if(g_theEndGameDB->Get(i)->NotifyLabBuilt()) {
			if(m_numBuilt[i] > 0) {
				return TRUE;
			}
		}
	}
	return FALSE;
}
