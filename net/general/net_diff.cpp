









#include "c3.h"
#include "net_diff.h"
#include "net_util.h"

#include "Diffcly.h"
#include "Player.h"

extern Player** g_player;

NetDifficulty::NetDifficulty(Difficulty* diff)
{
	m_difficulty = diff;
}

void 
NetDifficulty::Packetize(uint8* buf, uint16& size)
{
	buf[0] = 'G';
	buf[1] = 'D';

	putshort(&buf[2], (uint16)m_difficulty->m_owner);
	memcpy(&buf[4], m_difficulty, sizeof(*m_difficulty));
	size = 4 + sizeof(*m_difficulty);
}

void
NetDifficulty::Unpacketize(uint16 id, uint8* buf, uint16 size)
{
	Assert(buf[0] == 'G' && buf[1] == 'D');
	PLAYER_INDEX pidx = (PLAYER_INDEX)getshort(&buf[2]);
	
	Assert(pidx < k_MAX_PLAYERS);
	Assert(g_player[pidx] != NULL);

	*g_player[pidx]->m_difficulty = *(Difficulty*)&buf[4];
}
