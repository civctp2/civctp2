#include "c3.h"
#include "net_strengths.h"
#include "net_util.h"
#include "Strengths.h"
#include "player.h"
#include "TurnCnt.h"
#include "Score.h"

extern TurnCount *g_turn;
extern Player **g_player;

NetStrengths::NetStrengths(sint32 player)
{
	m_player = player;
}

void NetStrengths::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_STRENGTH_ID);
	Strengths *str = g_player[m_player]->m_strengths;

	PUSHLONG(m_player);
	PUSHLONG(g_player[m_player]->m_current_round);

	sint32 i;
	for(i = sint32(STRENGTH_CAT_NONE) + 1; i < sint32(STRENGTH_CAT_MAX); i++) {
		PUSHLONG(str->GetStrength((STRENGTH_CAT)i));
	}
}

void NetStrengths::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_STRENGTH_ID);

	PULLLONG(m_player);
	Assert(g_player[m_player]);
	if(!g_player[m_player])
		return;

	sint32 round;
	PULLLONG(round);

	Strengths *str = g_player[m_player]->m_strengths;

	sint32 i;
	for(i = sint32(STRENGTH_CAT_NONE) + 1; i < sint32(STRENGTH_CAT_MAX); i++) {
		sint32 strength;
		PULLLONG(strength);
		if(str->m_strengthRecords[i].Num() < round + 1) {
			for(sint32 j = str->m_strengthRecords[i].Num(); j < round + 1; j++) {
				str->m_strengthRecords[i].Insert(0);
			}
		}
		str->m_strengthRecords[i][round] = strength;
	}
	Assert(pos == size);
}

void NetFullStrengths::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_FULL_STRENGTHS_ID);
	Strengths *str = g_player[m_player]->m_strengths;
	PUSHLONG(m_player);
	PUSHLONG(m_startRound);
	PUSHLONG(m_endRound);

	sint32 n = str->m_strengthRecords[0].Num();
	PUSHLONG(n);
	sint32 r, i;
	for(r = m_startRound; r <= m_endRound; r++) {
		for(i = sint32(STRENGTH_CAT_NONE) + 1; i < sint32(STRENGTH_CAT_MAX); i++) {
			PUSHLONG(str->m_strengthRecords[i][r]);
		}
	}
}

void NetFullStrengths::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_FULL_STRENGTHS_ID);

	PULLLONG(m_player);
	Assert(g_player[m_player]);
	if(!g_player[m_player])
		return;

	PULLLONG(m_startRound);
	PULLLONG(m_endRound);

	Strengths *str = g_player[m_player]->m_strengths;

	sint32 n;
	PULLLONG(n);
	sint32 r, i;
	if(m_startRound == 0) {
		for(i = sint32(STRENGTH_CAT_NONE) + 1; i < sint32(STRENGTH_CAT_MAX); i++) {
			str->m_strengthRecords[i].Clear();
		}
	}

	for(r = m_startRound; r < m_endRound; r++) {
		for(i = sint32(STRENGTH_CAT_NONE) + 1; i < sint32(STRENGTH_CAT_MAX); i++) {
			sint32 s;
			PULLLONG(s);
			str->m_strengthRecords[i].Insert(s);
		}
	}
}

void NetScores::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_SCORES_ID);

	Score *sc = g_player[m_player]->m_score;
	Assert(sc);

	PUSHLONG(m_player);
	PUSHLONG(sc->m_cities_recaptured);
	PUSHLONG(sc->m_opponents_conquered);
	PUSHLONG(sc->m_finalScore);
	PUSHLONG(sc->m_victory_type);
	PUSHLONG(sc->m_feats);
}

void NetScores::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_SCORES_ID);

	PULLLONG(m_player);
	Assert(m_player >= 0);
	Assert(m_player < k_MAX_PLAYERS);
	if(m_player < 0 || m_player >= k_MAX_PLAYERS)
		return;

	if(!g_player[m_player])
		return;

	Score *sc = g_player[m_player]->m_score;
	Assert(sc);

	PULLLONG(sc->m_cities_recaptured);
	PULLLONG(sc->m_opponents_conquered);
	PULLLONG(sc->m_finalScore);
	PULLLONG(sc->m_victory_type);
	PULLLONG(sc->m_feats);
}
