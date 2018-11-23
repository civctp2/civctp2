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

#include "dp.h"
#include "eclock.h"

#include <stdio.h>
#include <assert.h>

/*-------------------------------------------------------*/
/* Convert a key to ASCII for debug printing */
static char *key2buf(char *key, int keylen, char *buf)
{
	int i;

	buf[0] = 0;
	for (i=0; i<keylen; i++) {
		//DPRINT(("key[%d] %d\n", i, key[i]));
		sprintf(buf + strlen(buf), "%d%s", 255 & key[i], (i == (keylen-1)) ? "" : ".");
	}
	return buf;
}
/* Convert a key to ASCII for debug printing */
static char key2a_buf[256];
static char key2a_buf2[256];
/* Don't use this twice in one printf! */
#define key2a(key, keylen) key2buf(key, keylen, key2a_buf)
/* You can use this one for the second key in a printf */
#define key2a2(key, keylen) key2buf(key, keylen, key2a_buf2)
/*-------------------------------------------------------*/

/* Print out incoming variable adds or deletes. */
int dp_PASCAL print_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t err)
{
	dptab_table_t *sessions = (dptab_table_t *)context;

	printf("print_cb: src h:%x, dest h:%x, table %s, subkey %s, err %d\n",
			src, dest,
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen),
			err);

	return TRUE;
}

/*-------------------------------------------------------------------------
 Callback triggered by listing sessions.
-------------------------------------------------------------------------*/
int dp_PASCAL listSessions_cb(dp_session_t *sDesc, long *pTimeout,long flags,void *context)
{
	char buf[300];
	if (!sDesc) {
		puts("End of session list");
		return FALSE;
	}

	sprintf(buf, "(k %d; %d plrs; spec %d; fl %x; U1 %x; pw '%s') name %s",
		sDesc->karma & 0xffff, sDesc->currentPlayers,
		sDesc->sessionType, sDesc->flags,
		sDesc->dwUser1, sDesc->szPassword,
		sDesc->sessionName
		);
	puts(buf);
	(void) context;
	return TRUE;
}

void dp2_poll(dp_t *mydp, int secs)
{
	clock_t started;

	/* Sit for a few seconds, and wait for session to be propagated to
	 * server
	 */
	started = eclock();
	while ((long)(eclock() - started) < (secs * ECLOCKS_PER_SEC)) {
		dpid_t idFrom, idTo;
		char pkt[512];
		size_t size;
		dp_result_t err;

		size = sizeof(pkt);
		err = dpReceive(mydp, &idFrom, &idTo, 0, &pkt, &size);
		assert ((err == dp_RES_EMPTY) || (err == dp_RES_OK));

		/* List sessions once per second */
		{
			static long oldtime = 0;
			if (oldtime != time(NULL)) {
				dp_session_t sess;
				/* Enumerate sessions and see if our session came back from server */
				memset(&sess, 0, sizeof(sess));
				sess.sessionType = 1234;

				err = dpEnumSessions(mydp, &sess, NULL, 0, listSessions_cb, NULL);
				oldtime = time(NULL);
			}
		}
		if (kbhit()) {
			getch();
			printf("Disconnecting from server at user request\n");
			DPRINT(("Disconnecting from server at user request\n"));
			/* disconnect from the game server abruptly. */
			dpSetGameServer(mydp, NULL);
		}
	}
}

dp2_test(char *servername, char *sessionname)
{
	dp_t *mydp;
	dp_transport_t theTransport;
	dp_result_t err;
	dp_session_t sess;

	memset(&theTransport, 0, sizeof(theTransport));
	strcpy(theTransport.fname, "wloopd.dll");
	err = dpCreate(&mydp, &theTransport, NULL, NULL);
	assert(err == dp_RES_OK);

	/* Whenever a variable comes in, print it. */
	dptab_setTableCallback(mydp->mysessions, print_cb, NULL);
	dptab_setTableCallback(mydp->sessions, print_cb, NULL);

	/* Now log in to the game server. */
	err = dpSetGameServer(mydp, servername);
	assert(err == dp_RES_OK);

	/* Create a session */
	memset(&sess, 0, sizeof(sess));
	strcpy(sess.sessionName, sessionname);
	sess.sessionType = 1234;
	sess.maxPlayers = 8;
	sess.dwUser1 = 0;
	sess.flags = dp_SESSION_FLAGS_CREATESESSION;
	err = dpOpen(mydp, &sess, NULL, NULL);
	assert(err == dp_RES_OK);
	printf("Created session %s.\n", sessionname);

	/* Sit for a few seconds, and wait for session to be propagated to
	 * server
	 */
	dp2_poll(mydp, 40);

	/* Kill our session - either by deleting it nicely, or by disconnecting
	 * from the game server abruptly.
	 */
#if 1
	err = dpClose(mydp);
	assert(err == dp_RES_OK);
#else
	dpSetGameServer(mydp, NULL);
#endif

	/* Sit for a few seconds, and wait for session deletion to be propagated to
	 * server
	 */
	dp2_poll(mydp, 5);

	err = dpDestroy(mydp, 0);
	assert(err == dp_RES_OK);

	return 0;
}

main(int argc, char **argv)
{
	int i;
	char *logfname;
	char *servername;

	if (argc != 3) {
		printf("Usage: dp2t1 logfile server\n");
		exit(1);
	}
	logfname = argv[1];
	servername = argv[2];

	dp_setLogFname(logfname);
	printf("Set log filename to %s\n", logfname);

	for (i=0; i<1; i++) {
		printf("Test iteration %d.\n", i);
		dp2_test(servername, logfname);
	}
	printf("No test failed\n");
	return 0;
}
