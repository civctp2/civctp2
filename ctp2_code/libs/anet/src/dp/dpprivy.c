/*
Copyright (C) 1995-2001 Activision, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*------------------------------------------------------------------------
 Private functions that don't fit anywhere else in dp...
 $Log: dpprivy.c $
 Revision 1.4  1997/08/29 19:38:09  dkegel
 Disabled stuff not needed in dp_ANET2 case
 Use dp_MUSTSWAP instead of sensing platform directly when possible
 Revision 1.3  1997/06/01 00:52:46  dkegel
 Use right ifdef for power pc.
 Revision 1.2  1997/05/28 17:34:02  anitalee
 Added functions to swap 16 and 32 bit numbers on sent and received packets
 from the network for big-endian machines.
 Revision 1.1  1997/05/15 22:05:36  anitalee
 Initial revision
------------------------------------------------------------------------*/

#include <string.h>

#ifndef NO_NETWORK
#include "dp.h"
#endif
#include "dpprivy.h"

#ifdef SPARC
/*------------------------------------------------------------------------
 A function that simulates clock() as it is implemented on PC's,
 i.e. it returns elapsed time in CLOCKS_PER_SEC units.
------------------------------------------------------------------------*/

clock_t dp_Sparc_clock(void)
{
	static struct tms time_buffer;
	static clock_t clk_convert = 0;

	if (!clk_convert) clk_convert = CLOCKS_PER_SEC / CLK_TCK;
	return times(&time_buffer) * clk_convert;
}
#endif

/*------------------------------------------------------------------------
 Returns system temporary directory.
------------------------------------------------------------------------*/
#ifdef WIN32
#include <windows.h>
int getTempDir(char *name, int maxnamelen)
{
	int namelen = GetTempPath(maxnamelen, name);
	if (namelen == 0) {
		name[0] = '\0';
	} else if (name[namelen - 1] != '\\') {
		name[namelen] = '\\';
		name[namelen++] = '\0';
	}
	return namelen;
}

#elif defined(UNIX)
int getTempDir(char *name, int namelen)
{
	strcpy(name, "/var/tmp/");
	return 5;
}

#elif defined(__POWERPC__)
int getTempDir(char *name, int namelen)
{
	name[0] = '\0';
	return 0;	/* not yet implemented */
}

#endif

/*------------------------------------------------------------------------
 Write/read size bytes to/from dat from/to *bp, incrementing *bp by size.
 Byteswaps if necessary.
------------------------------------------------------------------------*/
#if defined(SPARC) || defined(POWERPC)
void writeSwap(void **bp, const void *dat, unsigned int size)
{
	unsigned char *p0 = (unsigned char *)dat + (size - 1);
	unsigned char *p1 = (unsigned char *)(*bp);
	while (p0 >= dat)
		*p1++ = *p0--;
	*bp = (void *)p1;
}
void readSwap(const void **bp, void *dat, unsigned int size)
{
	unsigned char *p0 = (unsigned char *)(*bp);
	unsigned char *p1 = (unsigned char *)dat + (size - 1);
	while (p1 >= dat)
		*p1-- = *p0++;
	*bp = (void *)p0;
}

#else
void writeSwap(void **bp, const void *dat, unsigned int size)
{
	memcpy(*bp, dat, size);
	unsigned char *ucp = (unsigned char *) *bp;
	ucp += size;
	*bp = (void *) ucp;
}
void readSwap(const void **bp, void *dat, unsigned int size)
{
	memcpy(dat, *bp, size);
	unsigned char *ucp = (unsigned char *) *bp;
	ucp += size;
	*bp = (void *) ucp;
}

#endif

#ifndef NO_NETWORK
#if defined(dp_MUSTSWAP) /* need to swap bytes */
/*------------------------------------------------------------------------
 Functions that swap bytes in dp while receiving and sending packets.
------------------------------------------------------------------------*/

#ifndef dp_ANET2
void dpSwapEnumSessions(dp_enumSessions_packet_t *buffer)
{
	buffer->sessionType = SwapBytes2(buffer->sessionType);
}
#endif

void dpSwapErrorPacket(dp_error_packet_t *buffer)
{
	buffer->request = SwapBytes2(buffer->request);
	buffer->karma = SwapBytes2(buffer->karma);
}

void dpSwapSessionPacket(dp_session_packet_t *buffer)
{
	buffer->masterPseudoplayer = SwapBytes2(buffer->masterPseudoplayer);
	buffer->sessionType = SwapBytes2(buffer->sessionType);
	buffer->karma = SwapBytes2(buffer->karma);
	buffer->maxPlayers = SwapBytes2(buffer->maxPlayers);
	buffer->currentPlayers = SwapBytes2(buffer->currentPlayers);
	buffer->flags = SwapBytes2(buffer->flags);
	buffer->dwReserved1 = SwapBytes4(buffer->dwReserved1);
	buffer->dwUser1 = SwapBytes4(buffer->dwUser1);
}

#ifndef dp_ANET2
void dpSwapEnumPlayers(dp_enumPlayers_packet_t *buffer)
{
	buffer->sessionType = SwapBytes2(buffer->sessionType);
	buffer->karma = SwapBytes2(buffer->karma);
	buffer->start = SwapBytes2(buffer->start);
}

void dpSwapPlayerList(dp_playerList_packet_t *buffer, int nPlayers)
{
	int i;

	buffer->sessionType = SwapBytes2(buffer->sessionType);
	buffer->karma = SwapBytes2(buffer->karma);
	buffer->highest = SwapBytes2(buffer->highest);
	buffer->nPlayers = SwapBytes2(buffer->nPlayers);
	for (i = 0; i < nPlayers; i++) {
		buffer->players[i].id = SwapBytes2(buffer->players[i].id);
		buffer->players[i].karma = SwapBytes2(buffer->players[i].karma);
	}
}

void dpSwapPlayerRoster(dp_playerRoster_packet_t *buffer, int nPlayers)
{
	int i;

	buffer->sessionType = SwapBytes2(buffer->sessionType);
	buffer->karma = SwapBytes2(buffer->karma);
	buffer->flags = SwapBytes2(buffer->flags);
	buffer->nPlayers = SwapBytes2(buffer->nPlayers);
	buffer->maxPlayers = SwapBytes2(buffer->maxPlayers);
	for (i = 0; i < nPlayers; i++) {
		buffer->players[i].id = SwapBytes2(buffer->players[i].id);
		buffer->players[i].karma = SwapBytes2(buffer->players[i].karma);
	}
}

void dpSwapAddPlayer(dp_addPlayer_packet_t *buffer)
{
	buffer->newPlayer.id = SwapBytes2(buffer->newPlayer.id);
	buffer->newPlayer.karma = SwapBytes2(buffer->newPlayer.karma);
	buffer->sessionType = SwapBytes2(buffer->sessionType);
	buffer->sessionKarma = SwapBytes2(buffer->sessionKarma);
}

void dpSwapDelPlayer(dp_delPlayer_packet_t *buffer)
{
	buffer->id = SwapBytes2(buffer->id);
	buffer->karma = SwapBytes2(buffer->karma);
}

void dpSwapUserAddPlayer(dp_user_addPlayer_packet_t *buffer)
{
	buffer->id = SwapBytes2(buffer->id);
	buffer->karma = SwapBytes2(buffer->karma);
}
#endif

void dpSwapPingPacket(dp_ping_packet_t *buffer)
{
	buffer->karma = SwapBytes2(buffer->karma);
	buffer->pktnum = SwapBytes2(buffer->pktnum);
}

#ifndef dp_ANET2
void dpSwapUserAddGroup(dp_group_t *buffer, int n)
{
	int i;

	buffer->id = SwapBytes2(buffer->id);
	buffer->karma = SwapBytes2(buffer->karma);
	buffer->sessionKarma = SwapBytes2(buffer->sessionKarma);
	buffer->n = SwapBytes2(buffer->n);
	for (i = 0; i < n; i++) {
		buffer->members[i] = SwapBytes2(buffer->members[i]);
	}
}

void dpSwapUserDelGroup(dp_user_delGroup_packet_t *buffer)
{
	buffer->id = SwapBytes2(buffer->id);
	buffer->karma = SwapBytes2(buffer->karma);
	buffer->sessionKarma = SwapBytes2(buffer->sessionKarma);
}

void dpSwapPlayerToFromGroup(dp_addPlayerToGroup_packet_t *buffer)
{
	buffer->dpIdGroup = SwapBytes2(buffer->dpIdGroup);
	buffer->dpIdPlayer = SwapBytes2(buffer->dpIdPlayer);
	buffer->sessionKarma = SwapBytes2(buffer->sessionKarma);
}

void dpSwapUserHost(dp_packetType_t *buffer)
{
	*buffer = SwapBytes2(*buffer);
}
#endif

void dpSwapEnv(dp_envelope_t *buffer)
{
	buffer->src = SwapBytes2(buffer->src);
	buffer->dest = SwapBytes2(buffer->dest);
	buffer->sessKarma = SwapBytes2(buffer->sessKarma);
}

#ifndef dp_ANET2
void dpSwapPvUpdateInitial(pv_playerData_initial_packet_t *buffer)
{
	buffer->key = SwapBytes2(buffer->key);
	buffer->flags = SwapBytes2(buffer->flags);
	buffer->crc = SwapBytes4(buffer->crc);
	buffer->id = SwapBytes2(buffer->id);
}

void dpSwapPvUpdate(pv_playerData_body_packet_t *buffer)
{
	buffer->id = SwapBytes2(buffer->id);
}
#endif
#endif /* NO_NETWORK */

#if defined(_DEBUG) || defined(DPRNT)
/* Convert a key to ASCII for debug printing */
char *key2buf(const char *key, int keylen, char *buf)
{
	int i;

	if (keylen > hkeytab_MAXLEN)
		return "key too long";

	buf[0] = 0;
	for (i=0; i<keylen; i++) {
		/* DPRINT(("key[%d] %d\n", i, key[i])); */
		sprintf(buf + strlen(buf), "%d%s", 255 & key[i], (i == (keylen-1)) ? "" : ".");
	}
	return buf;
}

/* Buffers used by key2a macros in dpprivy.h */
char key2a_buf[256];
char key2a_buf2[256];
char key2a_buf3[256];
char key2a_buf4[256];
#endif

#endif
