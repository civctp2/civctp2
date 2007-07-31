









#include "c3.h"
#include "net_rand.h"
#include "net_util.h"
#include "RandGen.h"

extern RandomGenerator *g_rand;

#define k_RAND_ARRAY_SIZE 56
NetRand::NetRand()
{
}

void NetRand::Packetize(uint8* buf, uint16 &size)
{
	buf[0] = k_PACKET_RAND_ID >> 8;
	buf[1] = k_PACKET_RAND_ID & 0xff;
	
	size = 2;
	PUSHLONG(g_rand->m_start_seed);
	for(sint32 i = 0; i < k_RAND_ARRAY_SIZE; i++) {
		PUSHLONG(g_rand->m_buffer[i]);
	}
	PUSHLONG(g_rand->m_firstp - g_rand->m_buffer);
	PUSHLONG(g_rand->m_secondp - g_rand->m_buffer);
	PUSHLONG(g_rand->m_endp - g_rand->m_buffer);
	PUSHLONG(g_rand->m_callCount);
}

void NetRand::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos;

	Assert(MAKE_CIV3_ID(buf[0], buf[1]) == k_PACKET_RAND_ID);
	sint32 firstpindex, secondpindex, endpindex;

	pos = 2;

	PULLLONG(g_rand->m_start_seed);

	for(sint32 i = 0; i < k_RAND_ARRAY_SIZE; i++) {
		PULLLONG(g_rand->m_buffer[i]);
	}

	PULLLONG(firstpindex);
	PULLLONG(secondpindex);
	PULLLONG(endpindex);

	g_rand->m_firstp = &g_rand->m_buffer[firstpindex];
	g_rand->m_secondp = &g_rand->m_buffer[secondpindex];
	g_rand->m_endp = &g_rand->m_buffer[endpindex];
	
	PULLLONG(g_rand->m_callCount);
}
