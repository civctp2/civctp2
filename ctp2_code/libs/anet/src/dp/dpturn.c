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

 A round goes like this:

 1. Master sends out his move.
 2. Slaves get master's move, and send their own moves to all other slaves.
 3. When each slave has received all moves, it sends its move to master.
 4. Master receives all moves.

 From the point of view of a node, here's how to step thru a round:

 1. Wait for master's move.
 2. Send move to everyone but master.
 3. Wait for everyone else's move.
 4. If slave, Send move to master.

 When these steps are complete, signal the upper layer code that the round
 is complete.

 Those wait states are implemented by waiting for two booleans to go true:
  gotMasterMove
  gotAllSlaveMoves

 All moves are sent reliably; the underlying transport will retry until they
 get through.

 $Log: dpturn.c $
 Revision 1.5  1997/06/27 03:19:41  dkegel
 Ignore player creation/deletion if not in dpturn mode.
 Revision 1.4  1997/05/31 22:49:27  dkegel
 Moved pragma pack into dp*pack*.h for portability.
 Revision 1.3  1997/05/29 19:38:57  dkegel
 Fixed minor type mismatches caught by Codewarrior.
 Revision 1.2  1997/04/29 18:56:46  dkegel
 1. Finished converting to return dp_result_t's instead of booleans.
 2. Add check for more 'uninitialized' conditions.
 Revision 1.1  1997/04/25 18:23:06  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <time.h>
#include "dp.h"
#include "dpprivy.h"

/*--------------------------------------------------------------------------
 Add a peer.
 Returns TRUE on success.
 Not sure what happens if a player is added after rounds start!
--------------------------------------------------------------------------*/
DPBOOL dpturn_addPlayer(dp_t *dp, dpid_t id)
{
	int loop;
	dpturn_peer_t *p;

	if (!dp || !dp->dpt.initialized) return FALSE;

	/* Is it me? */
	if (id == dp->dpt.id) {
		DPRINT(("dpturn_addplayer(%d): no need to add myself.\n", id));
		return FALSE;	/* yes.  ignore. */
	}

	/* Does peer already exist? */
	for (loop = 0, p = dp->dpt.peers; loop < dp->dpt.n_peers; loop++, p++)
		if (p->id == id) {
			DPRINT(("dpturn_addplayer(%d): already exists.\n", id));
			return FALSE;	/* yes.  ignore. */
		}

	/* Is peer table full? */
	if (dp->dpt.n_peers >= dpturn_MAX_PEERS) {
		DPRINT(("dpturn_addplayer(%d): full.\n", id));
		return FALSE;
	}

	/* Allocate new element. */
	memset(p, 0, sizeof(dpturn_peer_t));
	p->id = id;
	dp->dpt.n_peers++;
	DPRINT(("dpturn_addplayer(%d): success.\n", id));
	return TRUE;
}

/*--------------------------------------------------------------------------
 Delete a peer.
 Returns TRUE on success.
 Probably works OK if a player is deleted after rounds start, unless
 it's the master.  Need to think about that case more.
--------------------------------------------------------------------------*/
DPBOOL dpturn_delPlayer(dp_t *dp, dpid_t id)
{
	int loop;
	dpturn_peer_t *p;

	if (!dp || !dp->dpt.initialized) return FALSE;

	/* Does peer already exist? */
	for (loop = 0, p = dp->dpt.peers; loop < dp->dpt.n_peers; loop++, p++)
		if (p->id == id)
			break;
	if (loop == dp->dpt.n_peers)
		return FALSE;

	/* Delete old element. */
	/* If there was 1, and he was deleted, move zero bytes, eh? */
	dp->dpt.n_peers--;
	memmove(p, p+1, (dp->dpt.n_peers - loop) * sizeof(dpturn_peer_t));

	/* If master was deleted, woe unto us.  FIX */
	if (id == dp->dpt.master) {
		DPRINT(("dpturn_delPlayer: bug: can't yet handle master deletion\n"));
		return FALSE;
	}

	/* Recalculate affected flags. */
	dp->dpt.gotAllSlaveMoves = TRUE;
	for (loop = 0, p = dp->dpt.peers; loop < dp->dpt.n_peers; loop++, p++)
		if (dp->dpt.master != p->id)
			dp->dpt.gotAllSlaveMoves &= p->received;

	return TRUE;
}

/*-------------------------------------------------------------------------
 Callback triggered by listing players.
-------------------------------------------------------------------------*/
static void dp_FAR dp_PASCAL dpturn_listPlayers_cb(dpid_t id, char_t * name, long flags, void *context)
{
	dp_t *dp = (dp_t *) context;

	if (id != dp_ID_NONE) {
		DPRINT(("dpturn_listPlayers_cb: Found player id %d, name %s\n", id, name));
		if ((flags & dp_EPC_FLAGS_LOCAL) > 0) {
			DPRINT(("dpturn_listPlayers_cb: ...and it is me....\n"));
			dp->dpt.id = id;
		} else {
			/* Don't include self in peer table.  Don't overflow it, either. */
			dpturn_addPlayer(dp, id);
		}

		/* Check if this could be the master - lowest id wins */
		if ((dp->dpt.master == dp_ID_NONE) || (id < dp->dpt.master)) {
			dp->dpt.master = id;
		}
	} else
		DPRINT(("dpturn_listPlayers_cb: done.  n_peers %d\n", dp->dpt.n_peers));
}

/*-------------------------------------------------------------------------
 Call to initialize dpturn.
 Must have already called dpCreate(), dpOpen, and created a player.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpTurnInit(dp_t *dp)
{
	dp_result_t err;

	memset(&dp->dpt, 0, sizeof(dp->dpt));
	dp->dpt.initialized = TRUE;
	dp->dpt.master = dp_ID_NONE;

	DPRINT(("dpTurncreate: List Players\n"));
	err = dpEnumPlayers(dp, NULL, dpturn_listPlayers_cb, dp, 1750L);
	if (err != dp_RES_OK) {
		DPRINT(("dpTurncreate: error %d calling dpEnumPlayers\n", err));
		return err;
	}
	if (dp->dpt.id == dp_ID_NONE) {
		DPRINT(("dpTurncreate: don't know who I am\n"));
		return dp_RES_BUG;
	}
	if (dp->dpt.master == dp_ID_NONE) {
		DPRINT(("dpTurncreate: don't know who's master\n"));
		return dp_RES_BUG;
	}

	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Call to shut down dpturn.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpTurnFinish(dp_t *dp)
{
	memset(&dp->dpt, 0, sizeof(dp->dpt));
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Call to send a move to all players.
 Move is queued for transmission by dpTurnPoll.
 Returns dp_RES_OK if ok, dp_RES_FULL if caller neglected to pick up all
 moves before calling.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpTurnPutMove(dp_t *dp, char *buf, size_t buflen)
{
	int loop;
	dpturn_peer_t *p;

	if (!dp || !dp->dpt.initialized) return dp_RES_BUG;

	dp->dpt.n_sent++;
	DPRINT(("dpTurnPutMove: round %d\n", dp->dpt.n_sent));
	memcpy(dp->dpt.out_data, buf, buflen);
	dp->dpt.out_data_size = buflen;
	for (loop=0, p=dp->dpt.peers; loop < dp->dpt.n_peers; loop++, p++) {
		p->outReady = TRUE;
		if (p->received && (p->n_received != dp->dpt.n_sent)) {
			DPRINT(("dpTurnPutMove: bug: haven't picked up id %d's move\n", p->id));
			return dp_RES_FULL;
		}
	}

	/* If we're the master, don't need for our move to send! */
	if (dp->dpt.id == dp->dpt.master) {
		dp->dpt.gotMasterMove = TRUE;
		DPRINT(("dpTurnPutMove: I'm the master, so don't wait.\n"));
	} else if (dp->dpt.n_peers == 1) {
		/* If we're a slave in a two-player game, don't need to wait for others. */
		dp->dpt.gotAllSlaveMoves = TRUE;
	}
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Check to see if a move has been received from the given player during this
 round.  If so, put its length in *data_len.
 If *id is dp_ID_NONE, then any player's move is retrieved, and
 the player's id is placed in *id.

 If it will fit in *data_len bytes, copy it to buf, and return DP_RES_OK.
 Otherwise, return dp_RES_FULL.

 Caller may wish to wait until dpTurnPoll() returns TRUE before calling
 this.  In that case, he should then call this function for all players
 and call dpTurnPutMove() before calling dpturn_poll() again to avoid
 losing a player's move.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpTurnGetMove(dp_t *dp, dpid_t *id, void *buf, size_t *data_len)
{
	int loop;
	dpturn_peer_t *p;
	size_t maxlen;

	if (!dp || !dp->dpt.initialized || !id || !data_len || !buf) return dp_RES_BUG;

	maxlen = *data_len;

	*data_len = 0;

	if (*id != dp_ID_NONE) {
		/* Locate given player's record. */
		dpid_t the_id = *id;
#if 1   // ultrakludge: the first game to use this call only uses the
		// 'definite id' entry, and only calls when orders are actually ready
		// and it needs to see its own moves echoed back to itself.
		if (the_id == dp->dpt.id) {
			// copy our own orders back to ourselves.  odd but needed for dr.
			*data_len = dp->dpt.out_data_size;
			if (dp->dpt.out_data_size > maxlen)
				return dp_RES_FULL;
			memcpy(buf, dp->dpt.out_data, dp->dpt.out_data_size);
			//dp->dpt.self_received = FALSE;
			DPRINT(("dpTurnGetMove: got move for self, how cheezy\n"));
			return dp_RES_OK;
		}
#endif
		for (loop = 0, p = dp->dpt.peers; loop < dp->dpt.n_peers; loop++, p++)
			if (the_id == p->id)
				break;

	} else {
		/* No player specified?  Accept any player with a packet ready. */
		for (loop = 0, p = dp->dpt.peers; loop < dp->dpt.n_peers; loop++, p++)
			if (p->received) {
				/* Return player id to caller. */
				*id = p->id;
				break;
			}
	}
	if (loop == dp->dpt.n_peers)
		return dp_RES_EMPTY;
	if (!p->received)
		return dp_RES_EMPTY;

	*data_len = p->in_data_size;

	/* Make sure caller's buffer big enough. */
	if (p->in_data_size > (size_t)maxlen)
		return dp_RES_FULL;

	/* Fetch packet. */
	memcpy(buf, p->in_data, p->in_data_size);
	p->received = FALSE;
	DPRINT(("dpTurnGetMove: got move for id %d\n", *id));
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Examine a packet to see if it is of interest to dpturn, and maybe process it.
 Returns dp_RES_EMPTY if the packet is private to dp->dpt,
 dp_RES_OK if the packet is some other kind of packet.
-------------------------------------------------------------------------*/
dp_result_t dpturn_handlePacket(dp_t *dp, dpid_t idFrom, char *buf, size_t buflen)
{
	int loop;
	dpturn_peer_t *p;
#include "dppack1.h"
	struct dp_turn_handlePacket_pkt_s {
		dp_packetType_t type PACK;
		union {
			/*dp_user_addPlayer_packet_t addPlayer;*/
			/*dp_user_delPlayer_packet_t delPlayer;*/
			unsigned char buf[dpio_MAXLEN_UNRELIABLE];
		} u PACK;
	} *pkt = (struct dp_turn_handlePacket_pkt_s *)buf;
#include "dpunpack.h"
	buflen -= sizeof(dp_packetType_t);

	DPRINT(("dpturn_handlePacket: type %2.2s\n", &pkt->type));
	if (!dp || !dp->dpt.initialized) return dp_RES_BUG;
	switch (pkt->type) {
	case dpturn_ROUND_PKT_ID:
		if (idFrom == dp->dpt.master) {
			dp_result_t err;
			dp_packetType_t tag = dpturn_ROUND_PKT_ID;
			/* Now that I have the master's move, I know
			 * that all move packets got thru, no more acks need be sent.
			 */
			err = dpHigherLevelBlanketAck(dp, dp_ID_BROADCAST, (char *)&tag, sizeof(tag));
			/* Also, I know I can send my move now. */
			dp->dpt.gotMasterMove = TRUE;
			DPRINT(("dpturn_handlePacket: Got Pkt from %d; ackall returns %d; t:%d\n", idFrom, err, clock()));
		} else
			DPRINT(("dpturn_handlePacket: Got Pkt from %d; t:%d\n", idFrom, clock()));

		/* Set the received flag for this peer, and the global one for all peers. */
		dp->dpt.gotAllSlaveMoves = TRUE;
		for (loop = 0, p = dp->dpt.peers; loop < dp->dpt.n_peers; loop++, p++) {
			if (idFrom == p->id) {
				if (p->received) {
					DPRINT(("dpturn_handlePacket: bug: received before cleared\n"));
					/* Hmm, guess we need an error return value, don't we? */
				}
				p->received = TRUE;
				p->n_received++;
				p->in_data_size = buflen;
				memcpy(p->in_data, pkt->u.buf, buflen);
			}
			if (dp->dpt.master != p->id)
				dp->dpt.gotAllSlaveMoves &= p->received;
		}

		return dp_RES_EMPTY;
	default:
		;
	}
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Send any packets appropriate at this point in the round.
 If not in dpturn mode, or if some player's move has not yet been
 received, or our move has not yet finished sending, returns
 dp_RES_EMPTY.
 If the round just finished, and packets are ready to picked up via
 dpTurnGetMove, returns dp_RES_OK.
 If dp is NULL, returns dp_RES_BUG.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpTurnPoll(dp_t *dp)
{
#include "dppack1.h"
	struct {
		dp_packetType_t type PACK;
		char data[dp_MAXLEN_RELIABLE - sizeof(dp_packetType_t)] PACK;
		char skip[6] PACK;      /* Caution: six bytes after buf trashed by dpSend! */
	} pkt;
#include "dpunpack.h"
	int copyIt = TRUE;
	dp_result_t err;
	int loop;
	dpturn_peer_t *p;
	int numUnsentMoves;

	if (!dp) return dp_RES_BUG;
	if (!dp->dpt.initialized) return dp_RES_EMPTY;

	DPRINT(("dpTurnPoll: gotMasterMove %d, gotAllSlaveMoves %d; peers %d; t:%d\n",
			dp->dpt.gotMasterMove, dp->dpt.gotAllSlaveMoves, dp->dpt.n_peers, clock()));

	/* 1. Wait for master's move. */
	if (!dp->dpt.gotMasterMove)
		return dp_RES_EMPTY;

	/* If we haven't sent our move to everyone, try to send it again. */
	numUnsentMoves = 0;
	for (loop = 0, p = dp->dpt.peers; loop < dp->dpt.n_peers; loop++, p++) {
		playerHdl_t dest;
		if (!p->outReady) {
			DPRINT(("dpTurnPoll: peer id:%d already sent\n", p->id));
			continue;
		}

		numUnsentMoves++;

		/* 2. Send move to everyone but master. */
		/* 3. Wait for everyone else's move. */
		/* 4. If slave, Send move to master. */
		if (!dp->dpt.gotAllSlaveMoves && (p->id == dp->dpt.master))
			continue;

		if (copyIt) {
			pkt.type = dpturn_ROUND_PKT_ID;
			memcpy(&(pkt.data), dp->dpt.out_data, dp->dpt.out_data_size);
			copyIt = FALSE;
		}
		dest = dpid2commHdl(dp, p->id);
		err = dpio_put_reliable(dp->dpio, &dest, 1, &pkt, sizeof(dp_packetType_t) + dp->dpt.out_data_size, NULL);
		if (err != dp_RES_OK) {
			DPRINT(("dpTurnPoll: Error %d sending from %d\n", err, dp->dpt.id));
		} else {
			DPRINT(("dpTurnPoll: Sent round To %d; t:%d\n", p->id, clock()));
			p->outReady = FALSE;
			numUnsentMoves--;
		}
	}
	/* If we finished sending and receiving, prepare for next round,
	 * and notify caller, who should call dpTurnGetMove() to get all
	 * moves if he hasn't already.
	 */
	DPRINT(("dpTurnPoll: numUnsent %d\n",numUnsentMoves));
	if ((numUnsentMoves == 0) && dp->dpt.gotAllSlaveMoves) {
		if (dp->dpt.master == dp->dpt.id) {
			/* I am the master; now that I have everyone's move, I know
			 * that all move packets got thru, no more acks need be sent.
			 */
			dp_result_t err;
			dp_packetType_t tag = dpturn_ROUND_PKT_ID;
			err = dpHigherLevelBlanketAck(dp, dp_ID_BROADCAST, (char *)&tag, sizeof(tag));
		}
		dp->dpt.gotMasterMove = FALSE;
		dp->dpt.gotAllSlaveMoves = FALSE;
		return dp_RES_OK;
	}
	return dp_RES_EMPTY;
}
