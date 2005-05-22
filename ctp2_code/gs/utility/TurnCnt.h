#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TURN_COUNT_H__
#define __TURN_COUNT_H__


#include "newturncount.h"

class CivArchive ;
template <class T> class SimpleDynamicArray;

typedef sint32 PLAYER_INDEX;

class TurnCount {
    sint32 m_round; 
    sint32 m_turn; 
	sint32 m_year;
	sint32 m_activePlayers;
	BOOL m_simultaneousMode;
	SimpleDynamicArray<sint32> *m_sliceList;
	sint32 m_lastBeginTurn;
	BOOL m_isHotSeat;
	BOOL m_isEmail;
	uint8 m_sentGameAlmostOverMessage;
	uint8 m_sentGameOverMessage;
	sint32 m_happinessPlayer;

	
    friend class NetInfo;

	private :
		void RunNewYearMessages(void) ;
		void SendMsgEndOfGameEarlyWarning(void) ;
		void SendMsgToAllPlayers(MBCHAR *s) ;
		static sint32 sm_the_stop_player;

public:
    
    static sint32 GetStopPlayer() { return sm_the_stop_player; }
    static void SetStopPlayer(const sint32 &player_index) { sm_the_stop_player = player_index; } 

    TurnCount(); 
    TurnCount(CivArchive &archive);
	~TurnCount();

    void Init();
    void Init(CivArchive &archive);
    
    
	void ChooseNextActivePlayer();
    void EndThisTurn();
	void BeginNewRound();
    void InformNetwork();
	void SliceInformNetwork();
	void InformMessages();
    void BeginNewTurn(BOOL clientVerification);
    void EndThisTurnBeginNewTurn (BOOL clientRequest = FALSE);
	void EndThisSlice();
    BOOL BeginNewSlice();
    void EndThisSliceBeginNewSlice();

    sint32 GetRound() const { return NewTurnCount::GetCurrentRound(); }
    sint32 GetTurn() const { return m_turn; }
    sint32 GetYear() const { return NewTurnCount::GetCurrentYear(); }

	void SkipToRound(sint32 round);

	void SetSliceTo(sint32 player);
	void QueueSliceFor(sint32 player);

	BOOL SimultaneousMode() const;
	void SetSimultaneousMode(BOOL on);
	void NetworkEndTurn(BOOL force = FALSE);
	
    void Serialize(CivArchive &archive) ;
    
	void CountActivePlayers();
    void PlayerDead(PLAYER_INDEX player);
	void RegisterNewPlayer(PLAYER_INDEX player);

	void SetHotSeat(BOOL on);
	void SetEmail(BOOL on);
	BOOL IsHotSeat();
	BOOL IsEmail();

	void NextRound(BOOL fromDirector = FALSE, BOOL force = FALSE);

    BOOL VerifyEndTurn(BOOL force);

	void ProfileNextRound();
#ifdef _DEBUG
	void LogPlayerStats(void);
#endif

	void ChooseHappinessPlayer();
	sint32 GetHappinessPlayer() { return m_happinessPlayer; }

	void NotifyBecameHost();
	void SendNextPlayerMessage();
};

extern TurnCount *g_turn;
#endif
