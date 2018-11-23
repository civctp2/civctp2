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
 Self test for scorerep functions.
--------------------------------------------------------------------------*/

#include <string.h>
#include <assert.h>
#include "anet.h"
#include "scorerep.h"
#include "assoctab.h"

/* fake a few functions so we don't have to include dp2.lib */
dp_dprintf(
	const char *	__format,	/* printf-style format (or NULL) */
	...)						/* printf-style arguments on stack (if any) */
{
#include <stdarg.h>
	va_list argptr = NULL;
	int len = 0;

	if (__format) {
		va_start(argptr, __format);
		len = vprintf(__format, argptr);
		va_end(argptr);
	}

	return len;
}

dp_result_t dpReportAssertionFailure(int lineno, char *file, char *linetxt)
{
	printf("dpReportAssertionFailure: %s, %d: %s\n", file, lineno, linetxt);
	return dp_RES_OK;
}

int scorerep_test(void)
{
	dp_result_t err;
	dp_uid_t myUID;
	dpid_t myId;
	dp_uid_t hisUID;
	dpid_t hisId;
	scorerep_t *rep;
	scorerep_buf_t repbuf;
	scorerep_player_t *player;
	unsigned short bloblen;
	char blob[scorerep_MAX_BLOBLEN];

	/* create a rep */
	rep = scorerep_create();
	assert(rep);

	/* set my id and uid */
	myId = 1;
	myUID = (long) myId + 10000;
	err = scorerep_setSelf(rep, myId, myUID);
	assert(!err);
	assert(rep->id == myId);
	assert(rep->uid == myUID);

	/* fill my score report with a few blobs */
	for (hisId = myId; hisId < myId + 10; hisId++) {
		hisUID = (long) hisId + 10000;
		blob[0] = dpGETSHORT_FIRSTBYTE(hisId);
		blob[1] = dpGETSHORT_SECONDBYTE(hisId);
		blob[2] = dpGETLONG_FIRSTBYTE(hisUID);
		blob[3] = dpGETLONG_SECONDBYTE(hisUID);
		blob[4] = dpGETLONG_THIRDBYTE(hisUID);
		blob[5] = dpGETLONG_FOURTHBYTE(hisUID);
		bloblen = 6;
		err = scorerep_set(rep, hisId, hisUID, 0, blob, bloblen);
		assert(!err);
	}

	/* convert it to a buffer with everyone's scores */
	err = scorerep_toBuf(rep, scorerep_FLAGS_SELFEXIT, myId, &repbuf);
	assert(!err);

	/* destroy the rep */
	scorerep_destroy(rep);
	rep = NULL;

	/* create a new rep */
	rep = scorerep_create();
	assert(rep);

	/* set my id and uid */
	myId = 1;
	myUID = (long) myId + 10000;
	err = scorerep_setSelf(rep, myId, myUID);
	assert(!err);
	assert(rep->id == myId);
	assert(rep->uid == myUID);

	/* read the buffer into my report table */
	err = scorerep_fromBuf(rep, &repbuf);
	assert(!err);
	assert(rep->players);
	assert(rep->flags == scorerep_FLAGS_SELFEXIT);

	/* check that it contains what I put there and only that */
	assert(rep->players->n_used = 10);
	for (hisId = myId; hisId < myId + 10; hisId++) {
		player = (scorerep_player_t *)assoctab_subscript(rep->players, hisId);
		assert(player);
		hisUID = (long) hisId + 10000;
		assert(player->uid = hisUID);
		assert(player->bloblen == 6);
		assert(hisId == dpMAKESHORT(player->blob[0], player->blob[1]));
		assert(hisUID == dpMAKELONG(player->blob[2], player->blob[3], player->blob[4], player->blob[5]));
	}

	/* convert it to a buffer with only my and his scores */
	hisId = myId + 1;
	hisUID = (long) hisId + 10000;
	memset(&repbuf, 0, sizeof(repbuf));
	err = scorerep_toBuf(rep, 0, hisId, &repbuf);
	assert(!err);

	/* destroy the rep */
	scorerep_destroy(rep);
	rep = NULL;

	/* create a new rep */
	rep = scorerep_create();
	assert(rep);

	/* set my id and uid */
	myId = 1;
	myUID = (long) myId + 10000;
	err = scorerep_setSelf(rep, myId, myUID);
	assert(!err);
	assert(rep->id == myId);
	assert(rep->uid == myUID);

	/* read the buffer into my report table */
	err = scorerep_fromBuf(rep, &repbuf);
	assert(!err);
	assert(rep->players);
	assert(rep->flags == 0);

	/* check that it contains what I put there and only that */
	assert(rep->players->n_used = 2);
	player = (scorerep_player_t *)assoctab_subscript(rep->players, myId);
	assert(player);
	assert(player->uid = myUID);
	assert(player->bloblen == 6);
	assert(myId == dpMAKESHORT(player->blob[0], player->blob[1]));
	assert(myUID == dpMAKELONG(player->blob[2], player->blob[3], player->blob[4], player->blob[5]));

	hisId = myId + 1;
	hisUID = (long) hisId + 10000;
	player = (scorerep_player_t *)assoctab_subscript(rep->players, hisId);
	assert(player);
	assert(player->uid = hisUID);
	assert(player->bloblen == 6);
	assert(hisId == dpMAKESHORT(player->blob[0], player->blob[1]));
	assert(hisUID == dpMAKELONG(player->blob[2], player->blob[3], player->blob[4], player->blob[5]));

	/* destroy the rep */
	scorerep_destroy(rep);
	rep = NULL;

	return 0;
}

int main(void)
{
	if (scorerep_test()) {
		printf("scorerep test failed!\n");
		return 1;
	}
	printf("scorerep no tests failed.\n");
	return 0;
}
