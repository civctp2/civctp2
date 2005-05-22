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

#ifndef scorerep_h
#define scorerep_h

#include "aneterr.h"
#include "assoctab.h"

/* An internal representation of the uid and scores for a given player */
#define scorerep_MAX_BLOBLEN 256
typedef struct {
	dp_uid_t uid;			 /* This player's uid */
	unsigned short bloblen;  /* This player's score blob length */
	char blob[scorerep_MAX_BLOBLEN];  /* This player's score blob */
} scorerep_player_t;

#define scorerep_FLAGS_SELFEXIT 1	/* I'm leaving and reporting all scores */
#define scorerep_FLAGS_LAUNCHED 2	/* This game has launched into combat */

/* An internal representation of a score report. */
typedef struct {
	dp_species_t sessType;	/* what kind of game this is a report for */
	dpid_t id;				/* dpid of reporting player */
	dp_uid_t uid;			/* uid of reporting player */
	dpid_t leaverid;		/* dpid of leaving player */
	dp_uid_t leaveruid;		/* uid of leaving player */
	short flags;			/* bitwise union of scorerep_FLAGS_* */
	assoctab_t *players;	/* scorerep_player_t players[dpid_t] */
} scorerep_t;

/* The external representation */
#define scorerep_MAX_BUFLEN 1024
typedef struct {
	size_t len;				/* how many bytes of buf are significant */
	char buf[scorerep_MAX_BUFLEN];
} scorerep_buf_t;

/*-------------------------------------------------------------------------
 Create a scorerep_t.
-------------------------------------------------------------------------*/
scorerep_t *scorerep_create(void);

/*-------------------------------------------------------------------------
 Free a scorerep_t after use.
-------------------------------------------------------------------------*/
void scorerep_destroy(scorerep_t *rep);

/*--------------------------------------------------------------------------
 Call once to report your own dpid and dpuid.
--------------------------------------------------------------------------*/
dp_result_t scorerep_setSelf(scorerep_t *rep, dpid_t dpId, dp_uid_t uid);

/*--------------------------------------------------------------------------
 Call once to report the leaver's dpid and dpuid.
--------------------------------------------------------------------------*/
dp_result_t scorerep_setLeaver(scorerep_t *rep, dpid_t dpId, dp_uid_t uid);

/*--------------------------------------------------------------------------
 Append the given score from the given player/uid to the internal
 representation in the given scorerep_t.
 Overwrites the previous value of this score for the given player.
--------------------------------------------------------------------------*/
dp_result_t scorerep_set(scorerep_t *rep, dpid_t dpId, dp_uid_t uid, int scoreId, const char *blob, unsigned short bloblen);

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
dp_result_t scorerep_toBuf(const scorerep_t *rep, long flags, dpid_t id, scorerep_buf_t *repbuf);

/*--------------------------------------------------------------------------
 Convert a scorerep_buf_t to a scorerep_t.  You can then probe the innards
 of the scorerep_t yourself.
 On entry, rep points to a scorerep_buf_t created by scorerep_create().
--------------------------------------------------------------------------*/
dp_result_t scorerep_fromBuf(scorerep_t *rep, const scorerep_buf_t *repbuf);

/*--------------------------------------------------------------------------
 Convert a scorerep_player_t to a scorerep_buf_t suitable for writing to disk.
--------------------------------------------------------------------------*/
dp_result_t scorerep_player_toBuf(const scorerep_player_t *player, scorerep_buf_t *repbuf);

/*--------------------------------------------------------------------------
 Convert a scorerep_buf_t to a scorerep_player_t.
 You can then probe the innards of the scorerep_player_t yourself.
 On entry, rep points to an uninitialized scorerep_player_t;
 when you're done with the scorerep_player_t, call assoctab_destroy on
 its scores array.
 OK, so this is lame.  We'll fix the interface later...
--------------------------------------------------------------------------*/
dp_result_t scorerep_player_fromBuf(scorerep_player_t *player, const scorerep_buf_t *repbuf);

#endif
