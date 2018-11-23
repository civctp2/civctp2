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

/*--------------------------------------------------------------------------
 Turn-taking system.

 $Log: dpturn.h $
 Revision 1.2  1997/04/29 18:57:50  dkegel
 Added boolean to note whether dpTurnInit called.  Removed parent pointer.
 Revision 1.1  1997/04/25 18:31:36  dkegel
 Initial revision
--------------------------------------------------------------------------*/
#ifndef dpturn_h
#define dpturn_h

#include <stddef.h>

/* Note: includer must include dp.h first, or this be included by dp.h
 * to get dp_MAXLEN_RELIABLE and dp_packetType_t
 */
#define dpturn_MAX_DATA_LEN	(dp_MAXLEN_RELIABLE - sizeof(dp_packetType_t))

#ifndef DPBOOL_DEFINED
#define DPBOOL_DEFINED
typedef int DPBOOL;
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define dpturn_MAX_PEERS                7

/* Structure to track the status of each of your peers in the game */
/* "Peer" is defined as all the players in the game not including yourself */
typedef struct dpturn_peer_s {
	dpid_t id;                  /* actual DPID of this player */
	DPBOOL outReady;           /* My data is ready to be sent to this player */
	DPBOOL received;           /* Received a packet from this player this round */
	int n_received;			   /* How many rounds we've gotten from him ever */
	char in_data[dp_MAXLEN_RELIABLE];   /* the packet we received */
	size_t in_data_size;        /* its length */
} dpturn_peer_t;

/* The class struct.  Private.  Never touched except via dpturn_functions. */
typedef struct dpturn_s {
	DPBOOL initialized;

	DPBOOL gotMasterMove;
	DPBOOL gotAllSlaveMoves;

	dpid_t id;                  /* our address */
	dpid_t master;              /* master's address */

	int n_peers;
	dpturn_peer_t peers[dpturn_MAX_PEERS];

	/* Outgoing packet */
	char out_data[dp_MAXLEN_RELIABLE];
	size_t out_data_size;
	int n_sent;
} dpturn_t;

/* for packet types, see dppkt.h */
/*#define dpturn_ROUND_PKT_ID dppt_MAKE('d','o')*/

/*-------------------------------------------------------------------------
 Examine a packet to see if it is of interest to dpturn, and maybe process it.
 Returns TRUE if the packet is private to dpturn.
-------------------------------------------------------------------------*/
dp_result_t dpturn_handlePacket(struct dp_s *dp, dpid_t idFrom, char *buf, size_t buflen);

/*--------------------------------------------------------------------------
 Add a peer.
 Returns TRUE on success.
 Not sure what happens if a player is added after rounds start!
--------------------------------------------------------------------------*/
dp_result_t dpturn_addPlayer(struct dp_s *dp, dpid_t id);

/*--------------------------------------------------------------------------
 Delete a peer.
 Returns TRUE on success.
 Probably works OK if a player is deleted after rounds start, unless
 it's the master.  Need to think about that case more.
--------------------------------------------------------------------------*/
dp_result_t dpturn_delPlayer(struct dp_s *dp, dpid_t id);

#endif
