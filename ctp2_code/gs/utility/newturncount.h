













#ifndef NEW_TURN_COUNT_H__
#define NEW_TURN_COUNT_H__

class NewTurnCount 
{
public:
	NewTurnCount();
	static sint32 GetStopPlayer();
	static void SetStopPlayer(const sint32 &player_index);

	
	static void StartNextPlayer(bool stop);

	static sint32 GetCurrentYear(sint32 player = -1);
	static sint32 GetCurrentRound();
	static void RunNewYearMessages();
	static void SendMsgEndOfGameEarlyWarning();
	static void SendMsgToAllPlayers(MBCHAR *s);
	static BOOL VerifyEndTurn(BOOL force);

	static void ClientStartNewYear(); 
 private:
	
	static void ChooseNextActivePlayer();

	
	static void StartNewYear();

	
	static sint32 sm_the_stop_player;

	static bool m_sentGameAlmostOverMessage;
	static bool m_sentGameOverMessage;

	friend class TurnCount;
};

#endif
