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

/*------- Player Variables -------------------------------------------------
 Each player (or group) can have "player variables" associated with it.
 These are available automatically to everyone in the session.
 If desired, they can be made available outside the session as well.

 The system reserves the first few tags for its own use, so
 games should only use tags dp_PLAYERDATA_KEY_USERDEF through
 dp_PLAYERDATA_KEY_MAX.

 Example uses:
 * Player's long name (set dp_PLAYERDATA_FLAG_PUBLIC to give lobby access)
 * Custom vehicle or logos
 * Gross position and damage data
 * Game secrets (the password for my fortress is SPAM)
 * Voice taunts
 * Downloadable mission filename and checksum
 * Downloadable missions (set dp_PLAYERDATA_FLAG_NOFLOOD); first call
   to dpGetPlayerData for that key starts the retrieval.

 If you need to know whether all data has been propagated, call
 dpReadyToFreeze(); if it returns dp_RES_OK on all machines,
 the data has finished propagating.

 $Log: dppv.c $
 Revision 1.17  1997/06/28 00:59:44  dkegel
 Fixed possible crash bug if pv NULL
 Revision 1.16  1997/05/31 22:49:26  dkegel
 Moved pragma pack into dp*pack*.h for portability.
 Revision 1.15  1997/05/29 19:38:57  dkegel
 Fixed minor type mismatches caught by Codewarrior.
 Revision 1.14  1997/05/28 17:28:30  anitalee
 Made portable to big-endian machines - calls SwapBytes2 now when reading
 or writing 16-bit integers to or from the net.  See comments in dpio.c.
 Revision 1.13  1997/03/12 05:34:49  dkegel
 Minor dprint improvements.
 Revision 1.12  1997/03/07 19:38:58  dkegel
 use dp_MALLOC/FREE/REALLOC instead of malloc/free/realloc
 so the Windows DLL can use GlobalAlloc or whatever to avoid
 losing its mind when HeapDestroy is called.
 Revision 1.11  1997/03/06 19:09:20  dkegel
 Fixed two dprints.
 Revision 1.10  1997/03/05 02:39:23  dkegel
 ini.h and dprint.h now integrated into anet.h;
 to compile or use DP as a DLL, define DP_DLL;
 win\build.bat now creates debugging and nondebugging versions
 of both the static dp.lib and the dynamic anetdll.dll.
 Revision 1.9  1997/02/27 21:51:13  dkegel
 Oops.  Fix crash on double free.
 Revision 1.8  1997/02/27 01:53:23  dkegel
 THE BIG REARRANGEMENT: includes now done carefully to avoid
 conflicts between anet.h and dp.h
 Revision 1.7  1997/02/26 18:16:48  dkegel
 Protect better against pv=null, since that happens if dpPoll is
 called after dpClose.
 Revision 1.6  1997/02/25 23:28:24  dkegel
 Don't scribble over dp pointer when thawing.  Oops.
 Revision 1.5  1997/02/25 01:43:29  dkegel
 1. Player variables now preserved across freeze/thaw cycles.
 2. pv_get now supports getting just the length by passing NULL buffer.
 Revision 1.4  1997/02/24 01:23:32  dkegel
 Fewer DPRINT's.
 Revision 1.3  1997/02/23 05:36:42  dkegel
 Rewrote to replace immediate pv_sendPacket with continuous pv_update.
 Much better behaved.  Can handle arbitrarily long variables.
 Protects variables with CRC - but only weakly notifies anyone if
 there's a CRC error; should probably be an assert.
 Revision 1.2  1997/02/20 00:30:23  dkegel
 Reorg to get ready for large variables.
 Revision 1.1  1997/02/12 03:06:32  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dp.h"
#include "dpmem.h"
#include "dpprivy.h"
#include "dpcrc32.h"

/*#undef DPRINT
 *#define DPRINT(s)
 */

/* #define DPRINTBUFS */
#ifdef DPRINTBUFS
static void dprint_buf(char *buf, size_t len)
{
	int i;
	if (len > 50) len = 50;
	for (i=0; i<len; i++) {
		DPRINT(("%02x ", buf[i]));
	}
	DPRINT(("\n"));
}
#endif

/*--------------------------------------------------------------------------
 Create a pv_t.
--------------------------------------------------------------------------*/
pv_t *pv_create(struct dp_s *dp)
{
	pv_t *pv;

	pv = (pv_t *)dp_MALLOC(sizeof(pv_t));
	if (!pv) return NULL;
	memset(pv, 0, sizeof(pv_t));

	pv->peers = assoctab_create(sizeof(pv_peer_t));

	if (!pv->peers) {
		assoctab_destroy(pv->peers);
		dp_FREE(pv);
		return NULL;
	}

	pv->next_send = dp->now;
	pv->cur_key_index = -1;
	pv->cur_ndests = 0;
	pv->new_ndests = 0;
	pv->cur_nkeys = 0;
	pv->cur_offset = 0;

	pv->dp = dp;
	return pv;
}

/*--------------------------------------------------------------------------
 Destroy a pv_t.
--------------------------------------------------------------------------*/
void pv_destroy(pv_t *pv)
{
	int i;
	if (!pv) return;

	/* For all players.  Step backwards to avoid having to move in delete. */
	for (i=pv->peers->n_used-1; i>=0; i--) {
		assoctab_item_t *pe;
		pe = assoctab_getkey(pv->peers, i);
		if (!pe)
			break;	/* horrible error */

		/* Delete this player */
		pv_deletePlayer(pv, pe->key);
	}
	/* Destroy the main table */
	assoctab_destroy(pv->peers);
}

/*--------------------------------------------------------------------------
 Delete a particular player from the table.
--------------------------------------------------------------------------*/
dp_result_t pv_deletePlayer(pv_t *pv, int player)
{
	int i;
	pv_peer_t *peer;
	if (!pv) return dp_RES_BUG;

	DPRINT(("pv_deletePlayer(dp, %d)\n", player));

	/* Get this player's table. */
	peer = (pv_peer_t *)assoctab_subscript(pv->peers, player);
	if (!peer) {
		DPRINT(("pv_deletePlayer: no table for player %d\n", player));
		return dp_RES_EMPTY;
	}

	/* Free all memory pointed to by the variables in peer->vars */
	for (i=0; i < peer->vars->n_used; i++) {
		pv_var_t *pvar;
		assoctab_item_t *pe;
		pe = assoctab_getkey(peer->vars, i);
		if (!pe)
			break;	/* horrible error */
		pvar = (pv_var_t *) &pe->value;

		/* Destroy their values */
		if (pvar->buf) dp_FREE(pvar->buf);
	}
	if (peer->incoming.buf) dp_FREE(peer->incoming.buf);

	/* Destroy it, and remove it from the table. */
	assoctab_destroy(peer->vars);
	if (assoctab_subscript_delete(pv->peers, player))
		return dp_RES_BUG;
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Add a particular player to the table.
--------------------------------------------------------------------------*/
static pv_peer_t *pv_addPlayer(pv_t *pv, int player)
{
	/* No peer yet.  Create one. */
	pv_peer_t *peer = (pv_peer_t *)assoctab_subscript_grow(pv->peers, player);
	if (!peer) {
		DPRINT(("pv_addPlayer(%d): can't grow peers!\n", player));
		return NULL;
	}
	if (peer->vars) return peer;
	peer->vars = assoctab_create(sizeof(pv_var_t));
	peer->incoming.key = dp_PLAYERDATA_KEY_NONE;
	peer->allocated = dpio_MAXLEN_RELIABLE;
	peer->incoming.buf = dp_MALLOC(peer->allocated);
	if (!peer->incoming.buf) return NULL;	/* bug */
	DPRINT(("pv_addPlayer: created variable table for player %d\n", player));
	return peer;
}

/*--------------------------------------------------------------------------
 Notify pv that a new node has entered the session,
 so pv can send him the variable table.
--------------------------------------------------------------------------*/
dp_result_t pv_addNode(pv_t *pv, playerHdl_t node)
{
	int i;

	DPRINT(("pv_addNode(pv, %d)\n", node));
	if (!pv) return dp_RES_BUG;
	/* Make sure not already in new_dests. */
	for (i=0; i<pv->new_ndests; i++)
		if (pv->new_dests[i] == node) return dp_RES_ALREADY;

	/* Make sure not too many. */
	if (pv->new_ndests >= dp_MAXPLAYERS)
		return dp_RES_BUG;

	/* Add to update list. */
	pv->new_dests[pv->new_ndests++] = node;
	DPRINT(("pv_addNode: added to list.  Now new_ndests = %d\n", pv->new_ndests));
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Set variable 'key' for a player.
 Value is copied onto the heap, and must be freed later with
 pv_set (overwriting frees), pv_deletePlayer or pv_destroy.
--------------------------------------------------------------------------*/
dp_result_t pv_set(pv_t *pv, int player, int key, size_t len, void *buf, int flags)
{
	pv_peer_t *peer;
	pv_var_t *pvar;

	if (!pv) return dp_RES_BUG;

	DPRINT(("pv_set(pv, player %d, key %d, len %d, buf %p, flags %x):\n", player, key, len, buf, flags));
#ifdef DPRINTBUFS
	DPRINT(("pv_set: buf = "));
	dprint_buf(buf, len);
#endif
	if (len > dp_PLAYERDATA_LEN_MAX)
		return dp_RES_BADSIZE;

	/* Get this player's table. */
	peer = (pv_peer_t *)assoctab_subscript(pv->peers, player);
	if (!peer) {
		/* No peer yet.  Create one. */
		peer = pv_addPlayer(pv, player);
		if (!peer) return dp_RES_NOMEM;
	}

	/* Add a key/value pair to it */
	pvar = (pv_var_t *)assoctab_subscript_grow(peer->vars, key);
	if (!pvar) {
		DPRINT(("pv_set: can't grow player %d's vars[%d]!\n", player, key));
		return dp_RES_NOMEM;
	}

	/* Delete old value from heap if present and not big enough */
	if (!pvar->buf || pvar->len < len) {
		if (pvar->buf) dp_FREE(pvar->buf);
		pvar->buf = dp_MALLOC(len);
	}
	if (!pvar->buf) return dp_RES_NOMEM;

	/* Fill the key/value pair with the given data */
	pvar->key = key;
	pvar->flags = flags;
	pvar->len = len;
	memcpy(pvar->buf, buf, len);
	pvar->crc = dp_crc32((unsigned char *)buf, len);
	DPRINT(("pv_set: player %d key %d len %d crc is %x\n", player, key, len, pvar->crc));

	/* Inform other players of the change unless told not to. */
	if (!(flags & dp_PLAYERDATA_FLAG_NOFLOOD)) {
		DPRINT(("Setting peer->dirty TRUE for player %d\n", player));
		peer->dirty = TRUE;
		pvar->flags |= dp_PLAYERDATA_FLAG_DIRTY;
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Get a variable for a player.
 Caller must set *len before calling to the size of buf.

 If the player or variable does not exist, dp_RES_EMPTY is returned.
 If *len is not big enough, *len is set to the current size,
 and dp_res_FULL is returned.
 buf may be NULL if you only want to find the variable's size.
--------------------------------------------------------------------------*/
dp_result_t pv_get(pv_t *pv, int player, int key, size_t *len, void *buf, int flags)
{
	pv_peer_t *peer;
	pv_var_t *pvar;
	size_t buflen;

	if (!pv) return dp_RES_BUG;
	if (!len) return dp_RES_BAD;

	/* Get this player's table. */
	peer = (pv_peer_t *)assoctab_subscript(pv->peers, player);
	if (!peer) return dp_RES_EMPTY;

	/* Get the desired key/value pair */
	pvar = (pv_var_t *)assoctab_subscript(peer->vars, key);
	if (!pvar) return dp_RES_EMPTY;		/* Hmm, it'd be nice to have another code */

	buflen = *len;
	*len = pvar->len;
	if (buflen < pvar->len) return dp_RES_FULL;
	if (buf) memcpy(buf, pvar->buf, pvar->len);

	(void) flags;
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Process a network message generated by this module on another computer.

 Note: player deletion should be noticed by some other handler, and
 should cause a call to pv_deletePlayer().

 Large variables will be handled as follows:
 The buffer is broken into pieces.
 The first piece is sent with tag pv_PLAYERDATA_INITIAL_PACKET_ID
 and has a header structure describing the length, owner, and key of the
 incoming variables.
 Subsequent pieces are sent with tag pv_PLAYERDATA_BODY_PACKET_ID,
 and lack headers.

 When a first piece comes in, it goes into a holding area for that source;
 When middle or last pieces come in, they are appended to the holding area;
 When a last piece comes in, the holding area is copied to the real variable
 area, and the user code is informed.

 On success,
	if not the final packet,
		Returns dp_RES_EMPTY.
	else,
		Returns dp_RES_OK, and places a tagged dp_user_playerData_packet_t
		in buffer.
--------------------------------------------------------------------------*/
dp_result_t pv_handlePacket(pv_t *pv, size_t len, void *buf)
{
	dp_packetType_t *tag = (dp_packetType_t *)buf;
	pv_peer_t *peer;
	pv_var_t *pvar;
	dp_result_t err;
	dpid_t id;

#ifdef DPRINTBUFS
	DPRINT(("pv_handlePacket(pv, %d, ", len));
	dprint_buf(buf, len);
#endif
	if (!pv)
		return dp_RES_BUG;

	if (*tag == pv_PLAYERDATA_INITIAL_PACKET_ID) {
		pv_playerData_initial_packet_t *body = (pv_playerData_initial_packet_t *)((char *)buf + sizeof(dp_packetType_t));
		void *payload = ((char *)body) + sizeof(pv_playerData_initial_packet_t);
		size_t datalen = len - sizeof(dp_packetType_t) - sizeof(pv_playerData_initial_packet_t);

		if (datalen < 1 || datalen > pv_PLAYERDATA_INITIAL_MAXLEN)
			return dp_RES_BUG;
		/* call dpSwapPvUpdateInitial to byte swap body */
		dpSwapPvUpdateInitial(body);
		id = body->id;

		/* Locate the sender's holding area and check for sanity. */
		peer = (pv_peer_t *)assoctab_subscript(pv->peers, id);
		if (!peer) {
			/* No peer yet.  Create one.  Hope we can trust id. */
			peer = pv_addPlayer(pv, id);
			if (!peer) {
				DPRINT(("pv_handlePacket: pv_addPlayer returns NULL\n"));
				return dp_RES_NOMEM;
			}
		}
		pvar = &peer->incoming;
		if (peer->allocated < body->len) {
			void *p = dp_REALLOC(pvar->buf, body->len);
			if (!p) return dp_RES_NOMEM;
			pvar->buf = p;
			peer->allocated = body->len;
		}
		/* Clear the holding area & copy this in. */
		pvar->key = body->key;
		pvar->flags = body->flags;
		pvar->len = body->len;
		pvar->crc = body->crc;
		memcpy(pvar->type, body->type, sizeof(pvar->type));
		memcpy(pvar->buf, payload, datalen);
		pvar->offset = datalen;
		DPRINT(("pv_handlePacket: after 1st packet, offset is %d, len is %d\n", pvar->offset, pvar->len));

	} else if (*tag == pv_PLAYERDATA_BODY_PACKET_ID) {
		pv_playerData_body_packet_t *body = (pv_playerData_body_packet_t *)((char *)buf + sizeof(dp_packetType_t));
		void *payload = ((char *)body) + sizeof(pv_playerData_body_packet_t);
		size_t datalen = len - sizeof(dp_packetType_t) - sizeof(pv_playerData_body_packet_t);

		if (datalen < 1 || datalen > pv_PLAYERDATA_BODY_MAXLEN) {
			DPRINT(("pv_handlePacket: datalen\n"));
			return dp_RES_BUG;
		}
		/* call dpSwapPvUpdate to byte swap body */
		dpSwapPvUpdate(body);
		id = body->id;
		/* Locate the sender's holding area and check for sanity. */
		peer = (pv_peer_t *)assoctab_subscript(pv->peers, id);
		if (!peer) {
			DPRINT(("pv_handlePacket: no variables for player %d\n", id));
			return dp_RES_BAD;
		}
		pvar = &peer->incoming;
		if (peer->allocated < pvar->offset + datalen) {
			DPRINT(("pv_handlePacket: allocated %d need %d + %d\n", peer->allocated, pvar->offset, datalen));
			return dp_RES_BUG;
		}

		/* Append to holding area. */
		memcpy((char *)pvar->buf + pvar->offset, payload, datalen);
		pvar->offset += datalen;
		DPRINT(("pv_handlePacket: after: id %d, key %d, offset %d, len %d\n", id, pvar->key, pvar->offset, pvar->len));

	} else
		return dp_RES_EMPTY;	/* no error - but no pv packet recognized */

	if (pvar->offset == pvar->len) {
		/* The variable has arrived! Obey the value change it carries.
		 * Don't echo to other machines!
		 */
		long newcrc = dp_crc32((unsigned char *)pvar->buf, pvar->len);
		DPRINT(("pv_handlePacket: got crc %x\n", newcrc));
		if (newcrc != pvar->crc) {
			DPRINT(("pv_handlePacket: bad crc %x, expected %x!\n", newcrc, pvar->crc));
			return dp_RES_BAD;
		}

		err = pv_set(pv, id, pvar->key, pvar->len, pvar->buf, dp_PLAYERDATA_FLAG_NOFLOOD);
		DPRINT(("pv_handlePacket: called pv_set; returning err %d\n", err));
		if (err == dp_RES_OK) {
			dp_user_playerData_packet_t *body = (dp_user_playerData_packet_t *)((char *)buf + sizeof(dp_packetType_t));
			/* Notify local players - overwrite buffer with user packet. */
			*tag = dp_USER_PLAYERDATA_PACKET_ID;
			body->len = pvar->len;
			body->id = id;
			body->key = pvar->key;
			body->data = pvar->buf;
			return dp_RES_OK;
		}
		return err;
	}
	return dp_RES_EMPTY;
}

/*--------------------------------------------------------------------------
 Call pv_update() periodically to propagate player variables to our peers.
 Each call results in sending at most one packet to one or more destinations,
 so it must be called frequently (>= 5 times/sec) to keep the data flowing.
 Only propagate the variables owned by the local player!  Since there can be
 several local players, punt and make the variable owner a parameter
 rather than propagating all local player's variables.  So I'm lame.

 pv_InformNewHost() triggers updates by adding to new_dests;
 pv_set triggers updates by setting the DIRTY bit on changed variables.
 pv_DeleteHost() avoids no-such-destination errors by deleting from
 both new_dests and cur_dests.

 Here's the algorithm:
 If you're in the middle of an update cycle, finish it before
 looking for dirty variables or new_dests.  That is,
 if cur_key_index != -1, just continue sending cur_keys[cur_key_index]
 to cur_dests[]; increment to the next key index when done; if
 no more keys, set to -1.

 Else if newhosts is not empty,
 set cur_dests to new_dests, cur_keys to everykey, clear new_dests,
 and start an update cycle.

 Else if new_keys is not empty,
 set cur_dests to everyone, cur_keys to new_keys, clear new_keys,
 and start an update cycle.

 If a variable is changed while we are in the middle of sending it,
 abort that particular variable transmission; it will be fully sent
 in the next cycle.

 This method makes sure everyone receives new values exactly once.
--------------------------------------------------------------------------*/

dp_result_t pv_update(pv_t *pv, dpid_t owner)
{
	size_t len, hdrlen;
	pv_peer_t *peer;
	assoctab_item_t *pe;
	pv_var_t *pvar;
	dp_result_t err;
	playerHdl_t errHdl;
	time_t interval;
	int i, j;
	char buf[dpio_MAXLEN_RELIABLE];

	if (!pv) {
		DPRINT(("pv_update: pv null\n"));
		return dp_RES_BUG;
	}

	/* Wait 'til previous transmission has had time to get sent. */
	if ((long)(pv->dp->now - pv->next_send) < 0) return dp_RES_OK;
	/* Set default next-check-time if no transmission this time. */
	pv->next_send = pv->dp->now + pv->dp->clocksPerSec / 8;

	/* To propagate the given id's variables, need to get access to them */
	peer = (pv_peer_t *)assoctab_subscript(pv->peers, owner);
	if (!peer) {
		/*DPRINT(("pv_update: no variables for player %d\n", owner)); */
		return dp_RES_OK;
	}
	/*DPRINT(("pv_update: peer->dirty is %d for player %d\n", peer->dirty, owner));*/

	/* If it's time to start a new cycle, do it. */
	if (pv->cur_key_index == -1) {
		if (pv->new_ndests > 0) {
			/* New hosts have been added since last cycle. */
			/* Start a host update cycle. */

			/* Copy new host list. */
			pv->cur_ndests = pv->new_ndests;
			memcpy(pv->cur_dests, pv->new_dests, pv->new_ndests * sizeof(pv->new_dests[0]));
			/* Reset new host list. */
			pv->new_ndests = 0;

			/* Fill key list with all public keys for this player. */
			for (i=j=0; i<peer->vars->n_used && j<dp_PLAYERDATA_NKEYS_MAX; i++) {
				pe = assoctab_getkey(peer->vars, i);
				if (!pe) break;	/* horrible error */
				pvar = (pv_var_t *) &pe->value;
				if (!(pvar->flags & dp_PLAYERDATA_FLAG_NOFLOOD)) {
					pv->cur_keys[j++] = pe->key;
				}
			}
			/* Early exit if no public variables. */
			if (j == 0) return dp_RES_OK;
			pv->cur_nkeys = j;
			DPRINT(("pv_update: starting new host update cycle. nkeys %d, ndests %d\n", pv->cur_nkeys, pv->cur_ndests));
			/* Trigger start. */
			pv->cur_key_index = 0;
			pv->cur_offset = 0;
		} else if (peer->dirty > 0) {
			/* Varible values have changed since last cycle. */
			/* Start a variable update cycle. */
			peer->dirty = FALSE;
			DPRINT(("pv_update: Clearing peer->dirty for player %d\n", owner));

			/* Set host list to 'all other hosts in game'. */
			pv->cur_ndests = dp_getBroadcastHdls(pv->dp, pv->cur_dests);

			/* Fill key list with all dirty variables.  Mark them clean. */
			for (i=j=0; i<peer->vars->n_used && j<dp_PLAYERDATA_NKEYS_MAX; i++) {
				pe = assoctab_getkey(peer->vars, i);
				if (!pe) break;	/* horrible error */
				pvar = (pv_var_t *) &pe->value;
				if ((pvar->flags & dp_PLAYERDATA_FLAG_DIRTY)
				&&  !(pvar->flags & dp_PLAYERDATA_FLAG_NOFLOOD)) {
					pv->cur_keys[j++] = pe->key;
					pvar->flags &= ~dp_PLAYERDATA_FLAG_DIRTY;
				}
			}
			/* Early exit if no other hosts in game (after clearing dirty!). */
			if (pv->cur_ndests <= 0) return dp_RES_OK;
			/* Early exit if no dirty public variables. */
			if (j == 0) return dp_RES_OK;
			pv->cur_nkeys = j;
			DPRINT(("pv_update: starting new variable update cycle. nkeys %d, ndests %d\n", pv->cur_nkeys, pv->cur_ndests));
			/* Trigger start. */
			pv->cur_key_index = 0;
			pv->cur_offset = 0;
		}
	}

	/* Are we in the middle of an update? */
	if (pv->cur_key_index == -1) return dp_RES_OK;	/* No. */

	/* Get a pointer to the variable we're working on.  Make sure it's
	 * still there, still clean, and still needs data transferred.
	 */
	do {
		pvar = (pv_var_t *)assoctab_subscript(peer->vars, pv->cur_keys[pv->cur_key_index]);
		/* Has it been deleted or changed or finished? */
		if (!pvar
		|| (pvar->flags & dp_PLAYERDATA_FLAG_DIRTY)
		|| (pvar->len <= pv->cur_offset)) {
			DPRINT(("pv_update: pvar %p, flags %x; advancing to next var\n", pvar, pvar?pvar->flags:0x666));
			/* Yes.  Just advance to the next variable. */
			pv->cur_key_index++;
			pv->cur_offset = 0;
			if (pv->cur_key_index >= pv->cur_nkeys) {
				/* That was the last variable.  The cycle is over. */
				pv->cur_key_index = -1;
				DPRINT(("pv_update: cycle over\n"));
				return dp_RES_OK;
			}
			pvar = NULL;
		}
	} while (!pvar);

	/* Send the current chunk of the current variable. */
	if (pv->cur_offset == 0) {
		dp_packetType_t *tag = (dp_packetType_t *)buf;
		pv_playerData_initial_packet_t *body = (pv_playerData_initial_packet_t *)(buf + sizeof(dp_packetType_t));
		void *payload = ((char *)body) + sizeof(pv_playerData_initial_packet_t);

		/* Send an initial chunk. */
		len = pvar->len;
		if (len > pv_PLAYERDATA_INITIAL_MAXLEN)
			len = pv_PLAYERDATA_INITIAL_MAXLEN;

		*tag = pv_PLAYERDATA_INITIAL_PACKET_ID;
		body->len = pvar->len;
		body->id = owner;
		body->flags = (short) pvar->flags;
		body->key = pvar->key;
		body->crc = pvar->crc;
		/* call dpSwapPvUpdateInitial to byte swap body */
		dpSwapPvUpdateInitial(body);
		memcpy(payload, pvar->buf, len);

		hdrlen = sizeof(dp_packetType_t)+sizeof(pv_playerData_initial_packet_t);
	} else {
		dp_packetType_t *tag = (dp_packetType_t *)buf;
		char *pastTag = buf + sizeof(dp_packetType_t);
		pv_playerData_body_packet_t *body = (pv_playerData_body_packet_t *)pastTag;
		void *payload = ((char *)body) + sizeof(pv_playerData_body_packet_t);
		len = pvar->len - pv->cur_offset;
		if (len > pv_PLAYERDATA_BODY_MAXLEN)
			len = pv_PLAYERDATA_BODY_MAXLEN;
#ifndef __MWERKS__
		assert(len>0);
#endif
		*tag = pv_PLAYERDATA_BODY_PACKET_ID;
		body->id = owner;
		/* call dpSwapPvUpdate to byte swap body */
		dpSwapPvUpdate(body);
		memcpy(payload, (char *)(pvar->buf)+pv->cur_offset, len);
		hdrlen = sizeof(dp_packetType_t)+sizeof(pv_playerData_body_packet_t);
	}

	err = dpio_put_reliable(pv->dp->dpio, pv->cur_dests, pv->cur_ndests,
		buf, len + hdrlen, &errHdl);

	/* Estimate the bandwidth used by this send. */
	interval = (pv->dp->clocksPerSec * (len + hdrlen)) / pv_BYTES_PER_SEC;
	pv->next_send = pv->dp->now + interval;
	DPRINT(("pv_update: Sent key %d off %d len %d to %d hosts; result %d, errHdl %d; next send in %d tix.\n",
		pv->cur_keys[pv->cur_key_index],
		pv->cur_offset, len, pv->cur_ndests, err, errHdl, interval));

	if (err == dp_RES_OK)
		pv->cur_offset += len;
	else if (err != dp_RES_FULL)
		DPRINT(("pv_update: serious send problem: dpio_put_reliable returned %d\n", err));

	return err;
}

/*-----------------------------------------------------------------------
 Save the state of a pv_var_t to disk.
-----------------------------------------------------------------------*/
static dp_result_t pv_var_freeze(pv_var_t *pvar, FILE *fp)
{
	pv_var_t temp = *pvar;
	DPRINT(("pv_var_freeze: key %d len %d offset %d type %3.3s\n",
		pvar->key, pvar->len, pvar->offset, pvar->type));
	temp.buf = (char *)0x12345678;		/* use as signature */
	if ((fwrite(&temp, sizeof(pv_var_t), 1, fp) != 1)
	||  (fwrite(pvar->buf, pvar->len, 1, fp) != 1))
		return dp_RES_FULL;
	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
 Restore the state of a pv_var_t from disk.
 On entry, buf is not allocated.
 On exit, buf is set to a buffer of just the right size from malloc.
-----------------------------------------------------------------------*/
static dp_result_t pv_var_thaw(pv_var_t *pvar, FILE *fp)
{
	/* Read in the variable */
	if (fread(pvar, sizeof(pv_var_t), 1, fp) != 1) {
		DPRINT(("pv_var_thaw: can't read var\n"));
		return dp_RES_EMPTY;
	}
	if (pvar->buf != (char *)0x12345678) {	/* check signature */
		DPRINT(("pv_var_thaw: bad sig %p\n", pvar->buf));
		return dp_RES_BAD;
	}
	if (pvar->len > dp_PLAYERDATA_LEN_MAX) {
		DPRINT(("pv_var_thaw: bad len %d\n", pvar->len));
		return dp_RES_BADSIZE;
	}

	DPRINT(("pv_var_thaw: key %d len %d offset %d type %3.3s\n",
		pvar->key, pvar->len, pvar->offset, pvar->type));
	pvar->buf = dp_MALLOC(pvar->len + 1);		/* kludge: avoid zero length */
	if (!pvar->buf) {
		DPRINT(("pv_var_thaw: can't allocate %d bytes\n", pvar->len+1));
		return dp_RES_NOMEM;
	}
	if (pvar->len && (fread(pvar->buf, pvar->len, 1, fp) != 1)) {
		DPRINT(("pv_var_thaw: can't read %d bytes\n", pvar->len));
		return dp_RES_EMPTY;
	}
	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
 Save the state of a pv_peer_t to disk along with who it's for.
-----------------------------------------------------------------------*/
static dp_result_t pv_peer_freeze(pv_peer_t *peer, dpid_t id, FILE *fp)
{
	int i;
	short dirty = peer->dirty;
	unsigned long n_vars;
	long sig = 0x12340009;
	dp_result_t err;

	DPRINT(("pv_peer_freeze:\n"));
	n_vars = (unsigned long) peer->vars->n_used;
	/* Header */
	if ((fwrite(&sig, sizeof(sig), 1, fp) != 1)
	||  (fwrite(&dirty, sizeof(dirty), 1, fp) != 1)
	||  (fwrite(&id, sizeof(id), 1, fp) != 1)
	||  (fwrite(&n_vars, sizeof(n_vars), 1, fp) != 1))
		return dp_RES_FULL;

	DPRINT(("pv_peer_freeze: peer %d, n_vars %d\n", id, n_vars));

	/* Body */
	for (i=0; i < peer->vars->n_used; i++) {
		assoctab_item_t *pe;
		pv_var_t *pvar;
		pe = assoctab_getkey(peer->vars, i);
		if (!pe)
			break;	/* horrible error */
		pvar = (pv_var_t *)&pe->value;
		DPRINT(("pv_peer_freeze: peer %d, var %d = %d, len %d\n",
			id, pe->key, pvar->key, pvar->len));
		err = pv_var_freeze(pvar, fp);
		if (err != dp_RES_OK)
			return err;
	}
	DPRINT(("pv_peer_freeze: freezing incoming\n"));
	err = pv_var_freeze(&peer->incoming, fp);
	if (err != dp_RES_OK)
		return err;

	/* No trailer */
	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
 Restore the state of a pv_peer_t from disk and add it to the pv_t.
-----------------------------------------------------------------------*/
static dp_result_t pv_peer_thaw(pv_t *pv, FILE *fp)
{
	pv_peer_t *peer;
	int i;
	short dirty;
	unsigned long n_vars;
	long sig;
	dpid_t id;
	dp_result_t err;

	/* Header */
	if ((fread(&sig, sizeof(sig), 1, fp) != 1)
	||  (fread(&dirty, sizeof(dirty), 1, fp) != 1)
	||  (fread(&id, sizeof(id), 1, fp) != 1)
	||  (fread(&n_vars, sizeof(n_vars), 1, fp) != 1)) {
		DPRINT(("pv_peer_thaw: can't read header\n"));
		return dp_RES_EMPTY;
	}

	if (sig != 0x12340009) {
		DPRINT(("pv_peer_thaw: bad sig\n"));
		return dp_RES_BAD;
	}
	if (n_vars > dp_PLAYERDATA_NKEYS_MAX) {
		DPRINT(("pv_peer_thaw: bad n_vars %d\n", n_vars));
		return dp_RES_BAD;
	}
	if ((dirty != FALSE) && (dirty != TRUE)) {
		DPRINT(("pv_peer_thaw: bad dirty\n"));
		return dp_RES_BAD;
	}
	DPRINT(("pv_peer_thaw: id %d, pv %p, n_vars %d\n", id, pv, n_vars));
	peer = pv_addPlayer(pv, id);
	if (!peer)
		return dp_RES_NOMEM;

	/* Body */
	for (i=0; i < n_vars; i++) {
		/* Grab the variable */
		pv_var_t var, *pvar;
		err = pv_var_thaw(&var, fp);
		if (err != dp_RES_OK) {
			DPRINT(("pv_peer_thaw: pv_var_thaw returns %d\n", err));
			return err;
		}
		DPRINT(("pv_peer_thaw: peer %d, var %d, len %d\n",
			id, var.key, var.len));
		/* Add it to the peer's var table. */
		pvar = (pv_var_t *)assoctab_subscript_grow(peer->vars, var.key);
		if (!pvar) {
			DPRINT(("pv_peer_thaw: can't grow player %d's vars[%d]!\n", id, var.key));
			return dp_RES_NOMEM;
		}

		/* Delete old value from heap if present */
		if (pvar->buf) dp_FREE(pvar->buf);

		/* Copy variable at last. */
		*pvar = var;
	}
	/* Ditch the incoming.buf allocated by pv_addPlayer, as pv_var_thaw
	 * allocates its own.
	 */
	if (peer->incoming.buf)
		dp_FREE(peer->incoming.buf);
	DPRINT(("pv_peer_thaw: thawing incoming\n"));
	err = pv_var_thaw(&peer->incoming, fp);
	if (err != dp_RES_OK)
		return err;
	peer->allocated = peer->incoming.len;
	peer->dirty = dirty;

	/* No trailer */
	return dp_RES_OK;
}

#define pv_SIG_START "pv 1"
#define pv_SIG_END   "pv 2"

/*-----------------------------------------------------------------------
 Save the state of a pv_t to disk.
-----------------------------------------------------------------------*/
dp_result_t pv_freeze(pv_t *pv, FILE *fp)
{
	int i;
	unsigned long n_peers;
	pv_t temp;
	DPRINT(("pv_freeze:\n"));

	if (!pv) {
		DPRINT(("pv_freeze: pv null\n"));
		return dp_RES_BUG;
	}

	temp = *pv;

	/* Header */
	fwrite(pv_SIG_START, sizeof(pv_SIG_START), 1, fp);
	temp.peers = (assoctab_t *)0x12345679;		/* use as signature */
	temp.dp = NULL;
	n_peers = pv->peers->n_used;
	if ((fwrite(&temp, sizeof(pv_t), 1, fp) != 1)
	||  (fwrite(&n_peers, sizeof(n_peers), 1, fp) != 1))
		return dp_RES_FULL;

	/* Body */
	for (i=0; i<pv->peers->n_used; i++) {
		assoctab_item_t *pe;
		pe = assoctab_getkey(pv->peers, i);
		if (!pe)
			break;	/* horrible error */

		/* save this player */
		pv_peer_freeze((pv_peer_t *)&pe->value, pe->key, fp);
	}

	/* Trailer */
	fwrite(pv_SIG_END, sizeof(pv_SIG_END), 1, fp);

	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
 Restore the state of a pv_t from disk.
 Call after creating a virgin pv_t.
-----------------------------------------------------------------------*/
dp_result_t pv_thaw(pv_t *pv, FILE *fp)
{
	char buf[sizeof(pv_SIG_END)];
	unsigned long n_peers;
	int i;
	dp_result_t err;
	dynatab_t *peers;
	dp_t *dp;

	if (!pv) {
		DPRINT(("pv_thaw: pv null\n"));
		return dp_RES_BUG;
	}

	/* Header */
	if ((fread(buf, sizeof(pv_SIG_START), 1, fp) != 1)
	||  memcmp(buf, pv_SIG_START, sizeof(pv_SIG_START))) {
		DPRINT(("pv_thaw: no start signature\n"));
		return dp_RES_BAD;
	}
    peers = pv->peers;
	dp = pv->dp;
	if ((fread(pv, sizeof(pv_t), 1, fp) != 1)
	||  (fread(&n_peers, sizeof(n_peers), 1, fp) != 1))
		return dp_RES_EMPTY;
	if ((int) pv->peers != 0x12345679)
		return dp_RES_BAD;
    pv->peers = peers;
	pv->dp = dp;
	if (n_peers > dp_MAXPLAYERS) {
		DPRINT(("pv_thaw: bad n_peers\n"));
		return dp_RES_BAD;
	}

	/* Body */
	for (i=0; i<n_peers; i++) {
		err = pv_peer_thaw(pv, fp);
		DPRINT(("pv_thaw: peer %d of %d; pv_peer_thaw returns %d\n", i, n_peers, err));
		if (err != dp_RES_OK)
			return err;
	}

	/* Trailer */
	if ((fread(buf, sizeof(pv_SIG_END), 1, fp) != 1)
	||  memcmp(buf, pv_SIG_END, sizeof(pv_SIG_END))) {
		DPRINT(("pv_thaw: no end signature\n"));
		return dp_RES_BAD;
	}
	return dp_RES_OK;
}
