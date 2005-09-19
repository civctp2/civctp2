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
 Score reports are blobs of data created by game specific code.
 They are initially created in a messy in-memory representation that
 allows easy access to some interesting parameters, then are rolled up
 into an efficient-to-send representation.
--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef _WIN32
#include <crtdbg.h>
#define ASSERTMEM() assert(_CrtCheckMemory())
#else
#define ASSERTMEM()
#endif

#include "dp2.h"
#include "scorerep.h"
#include "contract.h"

#define scorerep_ASSERT_VALID(rep) assert(!rep->players || \
	(rep->players->buf && \
	 rep->players->n_used <= rep->players->n_alloced && \
	 rep->players->n_alloced <= 1000 && rep->players->unit < 4000));


#if defined(DPRNT) || defined(DEBUG) || defined(_DEBUG)
/* Convert a binary buffer to hex notation.  Don't use twice in one DPRINT! */
static const char *hexstring(const unsigned char *binstr, int len)
{
	static char buf[768];
	int i;
	if (len < 1) return "";
	for (i = 0; i < len && i < 256; i++)
		sprintf(buf + 3*i, "%02x ", binstr[i]);
	buf[3*i-1] = '\0';
	return buf;
}

/* DPRINT a scorerep's contents for debugging */
static void scorerep_print(const scorerep_t *rep)
{
	int i;
	DPRINT(("*** flags:0x%x id:%d uid:%d leaverid:%d leaveruid:%d\n",
		rep->flags, rep->id, rep->uid, rep->leaverid, rep->leaveruid));
	if (!rep->players) {
		DPRINT(("*** players:NULL\n"));
		return;
	}
	DPRINT(("*** nplayers:%d (alloced:%d, unit:%d) players ->\n",
		rep->players->n_used, rep->players->n_alloced, rep->players->unit));
	for (i = 0; i < rep->players->n_used; i++) {
		assoctab_item_t *pi = assoctab_getkey(rep->players, i);
		scorerep_player_t *pp = (scorerep_player_t *)pi->value;
		DPRINT(("*** %d: id:%d -> uid:%d blob:%s\n", i, pi->key, pp->uid,
			hexstring(pp->blob, pp->bloblen)));
	}
}
#else
#define scorerep_print(rep)
#endif

/*-------------------------------------------------------------------------
 Create a scorerep_t.
-------------------------------------------------------------------------*/
scorerep_t *scorerep_create(void)
{
	scorerep_t *rep = malloc(sizeof(scorerep_t));
	ASSERTMEM();
	if (!rep) return NULL;
	memset(rep, 0, sizeof(*rep));
	ASSERTMEM();
	return rep;
}

/*-------------------------------------------------------------------------
 Free a scorerep_t after use.
-------------------------------------------------------------------------*/
void scorerep_destroy(scorerep_t *rep)
{
	ASSERTMEM();
	if (!rep)
		return;
	if (rep->players) {
		scorerep_ASSERT_VALID(rep);
		assoctab_destroy(rep->players);
		rep->players = NULL;
	}
	ASSERTMEM();
	memset(rep, 0, sizeof(*rep));
	free(rep);
	ASSERTMEM();
}

/*--------------------------------------------------------------------------
 Call once to report your own dpid and dpuid.
--------------------------------------------------------------------------*/
dp_result_t scorerep_setSelf(scorerep_t *rep, dpid_t dpId, dp_uid_t uid)
{
	rep->id = dpId;
	rep->uid = uid;
	ASSERTMEM();
	scorerep_ASSERT_VALID(rep);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Call once to report the leaver's dpid and dpuid.
--------------------------------------------------------------------------*/
dp_result_t scorerep_setLeaver(scorerep_t *rep, dpid_t dpId, dp_uid_t uid)
{
	rep->leaverid = dpId;
	rep->leaveruid = uid;
	ASSERTMEM();
	scorerep_ASSERT_VALID(rep);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Append the given score from the given player/uid to the internal
 representation in the given scorerep_t.
 Overwrites the previous value of this score for the given player.
--------------------------------------------------------------------------*/
dp_result_t scorerep_set(scorerep_t *rep, dpid_t dpId, dp_uid_t uid, int scoreId, const unsigned char *blob, unsigned short bloblen)
{
	scorerep_player_t *player;

	ASSERTMEM();
	DPRINT(("scorerep_set: setting id:%d uid:%d scoreid:%x bloblen:%d blob:%s\n", dpId, uid, scoreId, bloblen, hexstring(blob, bloblen)));
	if (scoreId != 0)
		return dp_RES_UNIMPLEMENTED;

	if (bloblen > scorerep_MAX_BLOBLEN) {
		DPRINT(("scorerep_set: bloblen %d is out of range (0-%d)\n", bloblen, scorerep_MAX_BLOBLEN));
		return dp_RES_BAD;
	}
	
	/* Create the table of tables, if needed... */
	if (!rep->players) {
		rep->players = assoctab_create(sizeof(scorerep_player_t));
		ASSERTMEM();
		if (!rep->players)
			return dp_RES_NOMEM;
	}

	/* Create an entry for this player, if needed... */
	player = assoctab_subscript_grow(rep->players, dpId);
	ASSERTMEM();
	scorerep_ASSERT_VALID(rep);
	if (!player)
		return dp_RES_NOMEM;

	/* Save the score blob */
	player->uid = uid;
	memcpy(player->blob, blob, bloblen);
	player->bloblen = bloblen;	
	ASSERTMEM();
	scorerep_ASSERT_VALID(rep);
	
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Convert a scorerep_t to a scorerep_buf_t suitable for writing to disk or
 network.

 Flags is 0 or a bitwise combination of scorerep_FLAGS_*.
 If scorerep_FLAGS_SELFEXIT, everyone's score is output; else 
 only reports on score for us (i.e. the id we reported via setSelf) and the
 given id.
 Other flags are simply stored in the report for interpretation by the 
 receiver.
--------------------------------------------------------------------------*/
dp_result_t scorerep_toBuf(const scorerep_t *rep, long flags, dpid_t id, scorerep_buf_t *repbuf)
{
	int i;
	short nPlayers;
	unsigned char *pbuf;
	scorerep_player_t *player;
	
	ASSERTMEM();
	scorerep_ASSERT_VALID(rep);
	scorerep_print(rep);

	/* save information from dpReportScore2 in the following format:
	 * struct {
	 * 	short score_flag;	    // last argument of dpReportScoreStart
	 *  short sessType;         // Session type of game
	 *  short submitter_id;     // id of submitting player
	 *  long  submitter_uid;    // uid of submitting player
	 *  short leaver_id;		// id of leaving player
	 *  short leaver_uid;		// uid of leaving player
	 * 	short nPlayers;
	 *	short ids[nPlayers];	// rows; player id's 
	 *	long uids[nPlayers];	// user id's associated with those players
	 *  short bloblen[nPlayers];  // lengths of the players' score blobs
	 *	char blobs[nPlayers][bloblen[nPlayers]];  // score blobs
	 * }
	 */
	precondition(rep);
	precondition(repbuf);
	precondition(rep->players); 

	if (flags & scorerep_FLAGS_SELFEXIT) {
		assert(id == rep->id);
		nPlayers = rep->players->n_used;
	} else {
		assert(id != rep->id);
		nPlayers = 2;
	}

	player = assoctab_subscript(rep->players, id);
	ASSERTMEM();
	scorerep_ASSERT_VALID(rep);
	if (!player) {
		DPRINT(("scorerep_toBuf: could not find id:%d in rep->players\n",
			id));
		return dp_RES_BAD;
	}

	DPRINT(("scorerep_toBuf: writing nPlayers:%d flag:0x%x, sesstype:%d myid:%d myuid:%d id:%d uid:%d\n", nPlayers, flags, rep->sessType, rep->id, rep->uid, id, player->uid));
	pbuf = repbuf->buf;
	*pbuf++ = dpGETSHORT_FIRSTBYTE((short)flags);
	*pbuf++ = dpGETSHORT_SECONDBYTE((short)flags);
	*pbuf++ = dpGETSHORT_FIRSTBYTE(rep->sessType);
	*pbuf++ = dpGETSHORT_SECONDBYTE(rep->sessType);
	*pbuf++ = dpGETSHORT_FIRSTBYTE(rep->id);
	*pbuf++ = dpGETSHORT_SECONDBYTE(rep->id);
	*pbuf++ = (char)dpGETLONG_FIRSTBYTE(rep->uid);
	*pbuf++ = (char)dpGETLONG_SECONDBYTE(rep->uid);
	*pbuf++ = (char)dpGETLONG_THIRDBYTE(rep->uid);
	*pbuf++ = (char)dpGETLONG_FOURTHBYTE(rep->uid);
	*pbuf++ = dpGETSHORT_FIRSTBYTE(id);
	*pbuf++ = dpGETSHORT_SECONDBYTE(id);
	*pbuf++ = (char)dpGETLONG_FIRSTBYTE(player->uid);
	*pbuf++ = (char)dpGETLONG_SECONDBYTE(player->uid);
	*pbuf++ = (char)dpGETLONG_THIRDBYTE(player->uid);
	*pbuf++ = (char)dpGETLONG_FOURTHBYTE(player->uid);
	*pbuf++ = dpGETSHORT_FIRSTBYTE(nPlayers);
	*pbuf++ = dpGETSHORT_SECONDBYTE(nPlayers);
	ASSERTMEM();
	scorerep_ASSERT_VALID(rep);
	
	DPRINT(("scorerep_toBuf: players:"));
	if (!(flags & scorerep_FLAGS_SELFEXIT)) {
		scorerep_player_t *myplayer = assoctab_subscript(rep->players, rep->id);
		if (!myplayer) {
			DPRINT(("scorerep_toBuf: could not find myid:%d in rep->players\n",
				rep->id));
			return dp_RES_BAD;
		}
		assert(rep->uid == myplayer->uid);
		DPRINT((" id:%d uid:%d\n", id, player->uid));
		DPRINT((" id:%d uid:%d\n", rep->id, rep->uid));
		/* Fill dpid array */
		*pbuf++ = dpGETSHORT_FIRSTBYTE(id);
		*pbuf++ = dpGETSHORT_SECONDBYTE(id);
		*pbuf++ = dpGETSHORT_FIRSTBYTE(rep->id);
		*pbuf++ = dpGETSHORT_SECONDBYTE(rep->id);
		/* Fill uid array */
		*pbuf++ = (char) dpGETLONG_FIRSTBYTE(player->uid);
		*pbuf++ = (char) dpGETLONG_SECONDBYTE(player->uid);
		*pbuf++ = (char) dpGETLONG_THIRDBYTE(player->uid);
		*pbuf++ = (char) dpGETLONG_FOURTHBYTE(player->uid);
		*pbuf++ = (char) dpGETLONG_FIRSTBYTE(rep->uid);
		*pbuf++ = (char) dpGETLONG_SECONDBYTE(rep->uid);
		*pbuf++ = (char) dpGETLONG_THIRDBYTE(rep->uid);
		*pbuf++ = (char) dpGETLONG_FOURTHBYTE(rep->uid);
		/* Fill bloblen array */
		*pbuf++ = dpGETSHORT_FIRSTBYTE(player->bloblen);
		*pbuf++ = dpGETSHORT_SECONDBYTE(player->bloblen);
		*pbuf++ = dpGETSHORT_FIRSTBYTE(myplayer->bloblen);
		*pbuf++ = dpGETSHORT_SECONDBYTE(myplayer->bloblen);

		if (player->bloblen > scorerep_MAX_BLOBLEN) {
			DPRINT(("scorerep_toBuf: bloblen:%d of id:%d is out of range\n",
				id, player->bloblen));
			return dp_RES_BUG;
		}
		if (myplayer->bloblen > scorerep_MAX_BLOBLEN) {
			DPRINT(("scorerep_toBuf: bloblen:%d of id:%d is out of range\n",
				rep->id, myplayer->bloblen));
			return dp_RES_BUG;
		}
		if ((pbuf - repbuf->buf) + player->bloblen + myplayer->bloblen > scorerep_MAX_BUFLEN) {
			DPRINT(("scorerep_toBuf: rep {nP:%d; bloblen:%d} is too large for repbuf\n", nPlayers, player->bloblen));
			dpReportAssertionFailure(__LINE__, __FILE__, "overflow");
			return dp_RES_FULL;
		}
		/* Fill score score array */
		memcpy(pbuf, player->blob, player->bloblen);
		pbuf += player->bloblen;
		memcpy(pbuf, myplayer->blob, myplayer->bloblen);
		pbuf += myplayer->bloblen;
		ASSERTMEM();
		scorerep_ASSERT_VALID(rep);
	} else {
		size_t len = (size_t)(pbuf - repbuf->buf) + nPlayers * (sizeof(long) + 2*sizeof(short));  /* length so far, without blobs */
		
		if (len > scorerep_MAX_BUFLEN) {
			DPRINT(("scorerep_toBuf: rep (nP:%d) is too large for repbuf\n", nPlayers));
			dpReportAssertionFailure(__LINE__, __FILE__, "overflow");
			return dp_RES_FULL;
		}
		/* Fill dpid array */
		for (i=0; i < nPlayers; i++) {
			assoctab_item_t *pe = assoctab_getkey(rep->players, i);
			
			assert(pe);
			DPRINT((" id:%d", pe->key));
			*pbuf++ = dpGETSHORT_FIRSTBYTE(pe->key);
			*pbuf++ = dpGETSHORT_SECONDBYTE(pe->key);
		}
		DPRINT(("\n"));
		ASSERTMEM();
		scorerep_ASSERT_VALID(rep);

		/* Fill uid array */
		for (i=0; i < nPlayers; i++) {
			assoctab_item_t *pe = assoctab_getkey(rep->players, i);
			
			assert(pe);
			player = (scorerep_player_t *)pe->value;
			DPRINT((" uid:%d", player->uid));
			*pbuf++ = (char) dpGETLONG_FIRSTBYTE(player->uid);
			*pbuf++ = (char) dpGETLONG_SECONDBYTE(player->uid);
			*pbuf++ = (char) dpGETLONG_THIRDBYTE(player->uid);
			*pbuf++ = (char) dpGETLONG_FOURTHBYTE(player->uid);
		}
		DPRINT(("\n"));
		ASSERTMEM();
		scorerep_ASSERT_VALID(rep);

		/* Fill bloblen array */		
		for (i=0; i < nPlayers; i++) {
			assoctab_item_t *pe = assoctab_getkey(rep->players, i);
			
			assert(pe);
			player = (scorerep_player_t *)pe->value;
			DPRINT((" bloblen:%d", player->bloblen));
			if (player->bloblen > scorerep_MAX_BLOBLEN) {
				DPRINT(("scorerep_toBuf: bloblen:%d is out of range\n", player->bloblen));
				return dp_RES_BUG;
			}
			len += player->bloblen;
			if (len > scorerep_MAX_BUFLEN) {
				DPRINT(("scorerep_toBuf: rep {nP:%d; last_bloblen:%d} is too large for repbuf\n", nPlayers, player->bloblen));
				dpReportAssertionFailure(__LINE__, __FILE__, "overflow");
				return dp_RES_FULL;
			}
			*pbuf++ = dpGETSHORT_FIRSTBYTE(player->bloblen);
			*pbuf++ = dpGETSHORT_SECONDBYTE(player->bloblen);
		}
		DPRINT(("\n"));
		ASSERTMEM();
		scorerep_ASSERT_VALID(rep);

		/* Fill score blob array */
		for (i=0; i < nPlayers; i++) {
			assoctab_item_t *pe = assoctab_getkey(rep->players, i);
			
			assert(pe);
			player = (scorerep_player_t *)pe->value;
			memcpy(pbuf, player->blob, player->bloblen);
			pbuf += player->bloblen;
			ASSERTMEM();
			scorerep_ASSERT_VALID(rep);
		}
	}
	repbuf->len = (pbuf - repbuf->buf);
	if (repbuf->len > scorerep_MAX_BUFLEN) {
		DPRINT(("scorerep_toBuf: bug: very wierd unexpected overflow\n"));
		return dp_RES_BUG;
	}
	DPRINT(("scorerep_toBuf: returning buf:%s\n", hexstring(repbuf->buf, repbuf->len)));
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Convert a scorerep_buf_t to a scorerep_t.  You can then probe the innards
 of the scorerep_t yourself.
 On entry, rep points to a scorerep_buf_t created by scorerep_create().
--------------------------------------------------------------------------*/
dp_result_t scorerep_fromBuf(scorerep_t *rep, const scorerep_buf_t *repbuf)
{
	dp_result_t err;
	const unsigned char *pbuf;
	int i;
	short nPlayers;
	dpid_t *ids;
	dp_uid_t *uids;
	unsigned short *bloblens;

	precondition(rep);
	precondition(repbuf);
	precondition(/* repbuf->len >= 0 && (Expression always true) */ repbuf->len <= scorerep_MAX_BUFLEN);
	ASSERTMEM();
	
	/* Iterate through the elements of the buffer, merrily calling
	 * scorerep_set() as you go.
	 */
	pbuf = repbuf->buf;
	rep->flags = dpMAKESHORT(pbuf[0], pbuf[1]);
	pbuf += sizeof(short);
	rep->sessType = dpMAKESHORT(pbuf[0], pbuf[1]);
	pbuf += sizeof(short);
	rep->id = dpMAKESHORT(pbuf[0], pbuf[1]);
	pbuf += sizeof(short);
	rep->uid = dpMAKELONG(pbuf[0], pbuf[1], pbuf[2], pbuf[3]);
	pbuf += sizeof(long);
	rep->leaverid = dpMAKESHORT(pbuf[0], pbuf[1]);
	pbuf += sizeof(short);
	rep->leaveruid = dpMAKELONG(pbuf[0], pbuf[1], pbuf[2], pbuf[3]);
	pbuf += sizeof(long);
	nPlayers = dpMAKESHORT(pbuf[0], pbuf[1]);
	pbuf += sizeof(short);
	ASSERTMEM();
	
	DPRINT(("scorerep_fromBuf: reading %d Players from buf:%s\n", nPlayers, hexstring(repbuf->buf, repbuf->len)));
	if (nPlayers <= 0)
		return dp_RES_EMPTY;
	if (repbuf->len < (size_t)(pbuf - repbuf->buf) + nPlayers*(sizeof(long) + 2*sizeof(short))) {
		DPRINT(("scorerep_fromBuf: repbuf->len:%d is too small for %d players.\n", repbuf->len, nPlayers));
		return dp_RES_BAD;
	}
	if (repbuf->len > scorerep_MAX_BUFLEN) {
		DPRINT(("scorerep_fromBuf: repbuf->len:%d > scorerep_MAX_BUFLEN:%d.\n", repbuf->len, scorerep_MAX_BUFLEN));
		return dp_RES_BAD;
	}
	
	ids = (dpid_t *)pbuf;
	pbuf += sizeof(short) * nPlayers;
	uids = (dp_uid_t *)pbuf;
	pbuf += sizeof(long) * nPlayers;
	bloblens = (unsigned short *)pbuf;
	pbuf += sizeof(short) * nPlayers;

	for (i = 0; i < nPlayers; i++) {
		char *c;
		dpid_t id;
		dp_uid_t uid;
		unsigned short bloblen;
		
		c = (char *)(ids + i);
		id = dpMAKESHORT(c[0], c[1]);
		c = (char *)(uids + i);
		uid = dpMAKELONG(c[0], c[1], c[2], c[3]);
		c = (char *)(bloblens + i);
		bloblen = dpMAKESHORT(c[0], c[1]);
		
		err = scorerep_set(rep, id, uid, 0, pbuf, bloblen);
		pbuf += bloblen;
		ASSERTMEM();
		if (err != dp_RES_OK) {
			DPRINT(("scorerep_fromBuf: scorerep_set(rep, %d, %d, 0, buf, %d) returns err:%d\n", id, uid, bloblen, err));
			return err;
		}
		if ((size_t)(pbuf - repbuf->buf) > repbuf->len) {
			DPRINT(("scorerep_fromBuf: repbuf->len:%d is too small for %d players + blobs.\n", repbuf->len, nPlayers));
			return dp_RES_BAD;
		}
	}
	if ((size_t)(pbuf - repbuf->buf) != repbuf->len) {
		DPRINT(("scorerep_fromBuf: repbuf->len:%d does not match contents of repbuf->buf (%d)\n", repbuf->len, (pbuf - repbuf->buf)));
		return dp_RES_BAD;
	}
	scorerep_ASSERT_VALID(rep);
	scorerep_print(rep);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Convert a scorerep_player_t to a scorerep_buf_t suitable for writing to disk.
--------------------------------------------------------------------------*/
dp_result_t scorerep_player_toBuf(const scorerep_player_t *player, scorerep_buf_t *repbuf)
{
	unsigned char *pbuf;
	
	/* save in the following format:
	 * struct {
	 *	short bloblen;
	 * 	char blob[bloblen];
	 * }
	 */
	precondition(player);
	precondition(player->bloblen <= scorerep_MAX_BUFLEN);
	ASSERTMEM();
	
	if (sizeof(player->bloblen) + player->bloblen > scorerep_MAX_BUFLEN) {
		DPRINT(("scorerep_player_toBuf: player is too large for repbuf\n"));
		dpReportAssertionFailure(__LINE__, __FILE__, "overflow");
		return dp_RES_FULL;
	}

	DPRINT(("scorerep_player_toBuf: writing blob of length %d\n", player->bloblen));
	pbuf = repbuf->buf;
	*pbuf++ = dpGETSHORT_FIRSTBYTE(player->bloblen);
	*pbuf++ = dpGETSHORT_SECONDBYTE(player->bloblen);
	memcpy(pbuf, player->blob, player->bloblen);	
	repbuf->len = (pbuf - repbuf->buf);
	ASSERTMEM();
	if (repbuf->len > sizeof(repbuf->buf)) {
		DPRINT(("scorerep_toBuf: bug: very wierd unexpected overflow\n"));
		return dp_RES_BUG;
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Convert a scorerep_buf_t to a scorerep_player_t.
 You can then probe the innards of the scorerep_player_t yourself.
 On entry, player points to an uninitialized scorerep_player_t;
 when you're done with the scorerep_player_t, call assoctab_destroy on
 its scores array.
 OK, so this is lame.  We'll fix the interface later...
--------------------------------------------------------------------------*/
dp_result_t scorerep_player_fromBuf(scorerep_player_t *player, const scorerep_buf_t *repbuf)
{
	const unsigned char *pbuf;

	precondition(player);
	ASSERTMEM();
	
	pbuf = repbuf->buf;
	player->bloblen = dpMAKESHORT(pbuf[0], pbuf[1]);
	pbuf += sizeof(short);

	if (player->bloblen > scorerep_MAX_BLOBLEN) {
		DPRINT(("scorerep_player_fromBuf: bloblen %d is out of range (0-%d)\n", player->bloblen, scorerep_MAX_BLOBLEN));
		return dp_RES_BUG;
	}
	
	DPRINT(("scorerep_player_fromBuf: reading blob of length %d\n", player->bloblen));
	memcpy(player->blob, pbuf, player->bloblen);
	ASSERTMEM();
	return dp_RES_OK;
}

