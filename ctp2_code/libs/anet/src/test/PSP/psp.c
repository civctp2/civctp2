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
 Test of player list propagation (Player-Sees-Player)

 Usage: PSP <argument list>

 Required Arguments:
	-c=count			The number of players to run the test with.
 Optional Arguments:
	-b=player			Start spawning at this player number (default = 0).
	-d=driver			Name of the driver to use (default = loopback driver).
	-e=player			End spawning at this player number (default = count).
	-g=group_name		Create group with name group_name (has a default).
	-i=packet_loss		The percentage of packet loss introduced (default = 0%).
	-l=logfile			The filename for the log (default = stdout).
	-n=times			Number of times to run test (default = 1)
	-p=port				Port number to connect to (default = any).
	-r=room				Room name to connect to (has a default).
	-s=server_name		The name or address of the server to use (default = none).
	-t=timeout			msec to wait after completion (default = 0).
	-v=variable count	The number of player variables to run the test with.
	-y=time				msec to wait in inner loop (default=100).
 Hidden Arguments:
	-m=player			Player number (used only in PSPs spawned by PSP)
	(Note that arguments -b and -e are ignored if -m is present).

 Test Design
 -----------
 Invokes the requisite number of copies of itself, giving each copy a
 sequentially-chosen player number.
 When invoked as player 0, creates a game; otherwise, joins a game.
 The test records the elapsed time between these five events:
 1. Call to dpOpen().
 2. Callback from dpOpen().
 3. Call to dpCreatePlayer().
 4. Callback from dpCreatePlayer().
 5. Receipt of the full player list.
 Times are recorded as incomplete if the event has not occurred.
 Receipt of the full player list is determined by receiving a list
 containting a player count equal to total_players.
 The results are written to a log file, or stdout if no log file is requested.
 The test attempts to run with a percentage of packet loss as requested.

 $Log: psp.c $
 Revision 1.31  1997/10/03 18:47:11  lbennett
 Reduced number of lines printed on end-of-loop.
 Revision 1.30  1997/10/01 00:08:43  lbennett
 Fixed some group handling bugs in freeze/thaw's spawn code.
 Revision 1.29  1997/09/30 06:39:58  lbennett
 Handle timing for looped freeze/thaw test.
 Revision 1.28  1997/09/27 05:33:04  lbennett
 Added looping, freeze/thaw.
 Revision 1.27  1997/09/20 04:53:22  anitalee
 added option for creating groups
 Revision 1.26  1997/09/03 23:39:57  lbennett
 Added load compile switch.
 Revision 1.25  1997/09/03 22:39:54  lbennett
 Added better join-session handling.
 Revision 1.24  1997/09/03 01:37:38  lbennett
 Added room name parameter
 Revision 1.23  1997/09/02 23:12:27  lbennett
 Handle session lost and host lost.
 Revision 1.22  1997/09/02 21:39:27  lbennett
 Added yield-time parameter.
 Revision 1.21  1997/09/01 18:42:26  dkegel
 Don't wait for gameserver if there is none...
 Revision 1.20  1997/09/01 03:43:14  lbennett
 Wait a while for gameserver.
 Revision 1.19  1997/08/31 22:16:00  lbennett
 Added wait after dpSetGameServer.
 Revision 1.18  1997/08/31 00:27:13  lbennett
 Added support for multiple machine tests.
 Revision 1.17  1997/08/30 22:55:13  lbennett
 Modified to run a complete test from this executable.
 Revision 1.15  1997/08/25 03:41:50  dkegel
 Don't forget dpClose().
 Revision 1.14  1997/08/23 02:11:25  lbennett
 Added too many extra command-line parameters.
 Revision 1.13  1997/08/19 21:51:50  dkegel
 1. Synchronize by sending message to master, then having master
 send packet to everyone; that's 2N rather than N^2 packets,
 and it avoids a nasty race condition in dp2.
 2. Check for the race condition.
 Revision 1.12  1997/08/16 01:12:03  lbennett
 *** empty log message ***
 Revision 1.11  1997/08/16 01:10:06  lbennett
 *** empty log message ***
 Revision 1.10  1997/08/15 01:51:23  lbennett
 Improved termination conditions.
 Revision 1.9  1997/08/14 21:05:06  lbennett
 Fixed communications of end-of-test.
 Revision 1.8  1997/08/14 18:55:36  lbennett
 Added packet loss, end-of-run communications.
 Revision 1.7  1997/08/07 19:58:18  lbennett
 Added timeouts to all loops.
 Revision 1.6  1997/08/07 00:49:24  lbennett
 Removed randomness from session ID (guarantee uniqueness)
 Revision 1.5  1997/08/06 22:57:59  lbennett
 Better reporting, timeout for incomplete test.
 Revision 1.4  1997/08/06 02:04:51  lbennett
 Longer delay, better formatting of results.
 Revision 1.3  1997/08/06 01:10:27  lbennett
 Debugged to work in non-start mode.
 Revision 1.2  1997/08/05 22:50:02  lbennett
 Debugged somewhat.
 Revision 1.1  1997/08/01 01:08:43  lbennett
 Initial revision
--------------------------------------------------------------------------*/

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if (defined WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <assert.h>
#include <math.h>
#include <process.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "eclock.h"
#include "anet.h"

/* Mac replacements for some "standard" DOS stuff. */
#ifdef __MWERKS__
#include "stricmp.h"
#include <console.h>  /* for commandline support */
#endif

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed"
   (Windows *linked* code has six of these ) */
#if (defined WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/**
* Constants
*/

#define TEST_SPECIES	666
#define DEFAULT_ROOM	"PSP303."
#define DEFAULT_DRIVER	"..\\..\\..\\win\\dll\\wloopd.dll"
#define FREEZENAME	"dp%d.frz"
#define FREEZENAME2	"dpi%d.frz"

#define DEFAULT_PORT	20000
#define YIELD_DEFAULT	100

#define MAX_SESSIONS	100
#define test_YIELD_MAX	2000

#define RECEIVE_FLAGS	0
#define BUFSIZE			1024

#define test_TIMEOUT_DELAY	60 * ECLOCKS_PER_SEC
#define test_ACK_DELAY		5 * ECLOCKS_PER_SEC

#define test_ALL_RECEIVED	dppt_MAKE('a','r')

/* Test result values */
#define psp_RES_OK				0	/* passed */
#define psp_RES_TOOFEWPLAYERS	1	/* not enough players found */
#define psp_RES_BADACK			3	/* got invalid ack */
#define psp_RES_SIGNAL			4	/* user/system signalled */
#define psp_RES_SESSIONLOST		5	/* lost session during test */
#define psp_RES_IDZERO			6	/* */
#define psp_RES_HOSTLOST		7	/* lost host during test */

#define SwapBytes(v)	((unsigned short)((v)<<8)|(unsigned char)((v)>>8))
/**
* Types
*/

/* Structure to hold arguments to the test */
typedef struct pspArgs_s {
	char *server;
	char *group;
	int packetLoss;
	unsigned long portnum;
	time_t extra_timeout;
	int endPlayer;
} pspArgs_t;

/* Structure to hold the ongoing results of the test */
typedef struct pspResult_s {
	// Scratch
	dp_session_t sessions[MAX_SESSIONS];
	int* procs;
	char* room;
	char* exe;
	boolean callbacksFinished;
	boolean allAcks;
	boolean* acks;
	int n_sessions;
	int n_players;
	int n_groups;
	int n_gplayers;   // number of players found in group
	int n_pvplayers;  // number of players found with player variables set
	int *npv;		  // # of player variables received indexed by player
	int freezeLoop;
	int in_session;
	time_t yield_time;
	#if defined(WIN32)
		int emptyCount;
	#endif

	// Results
	dpid_t* playerID;
	dpid_t* gplayerID;   // list of players in group
	dpid_t* pvplayerID;  // list of players with player variables set
	dpid_t myID;
	dpid_t groupID;
	time_t dpOpenTime;
	time_t dpOpenCallbackTime;
	time_t dpCreatePlayerTime;
	time_t dpCreatePlayerCallbackTime;
	time_t allPlayerTime;
	time_t allGPlayerTime;
	time_t allPVPlayerTime;
	int thisPlayer;
	int n_expectedPlayers;
	int n_localPlayers;
	int n_maxPlayers;
	int n_maxGPlayers;
	int n_maxPVPlayers;
	int beginPlayer;
	int n_expectedpVars;
	int loops;
	char *driver;
} pspResult_t;

/**
* Global variables
*/

pspResult_t results;	// Must be global for signal handler
FILE* logFile;			// Must be global for signal handler

/**
* Methods
*/

/*-------------------------------------------------------------------------
 Write the results to the log file
-------------------------------------------------------------------------*/
void
writeReport(
	FILE* log,
	pspResult_t* res)
{
	if(res->thisPlayer == res->beginPlayer) {
		fprintf(log, "PSP Test\n%d Players\n%d Players on Local Machine\nDriver: %s\n",
			res->n_expectedPlayers, res->n_localPlayers, res->driver);
	}
	fprintf(log, "\nPlayer %d res:\n", res->thisPlayer);
	if(res->allPlayerTime == -1) {
		int diff = eclock() - res->dpOpenTime;
		fprintf(log, "INCOMPLETE: %d.%03d seconds until %d of %d players found.\n",
				diff/ECLOCKS_PER_SEC, diff%ECLOCKS_PER_SEC, res->n_maxPlayers, res->n_expectedPlayers);
	} else {
		int diff = res->allPlayerTime - res->dpOpenTime;
		fprintf(log, "%d.%03d seconds until player list completed.\n",
				diff/ECLOCKS_PER_SEC, diff%ECLOCKS_PER_SEC);
	}
	if (res->groupID != dp_ID_NONE) {
		if(res->allGPlayerTime == -1) {
			int diff = eclock() - res->dpOpenTime;
			fprintf(log, "INCOMPLETE: %d.%03d seconds until %d of %d group players found.\n",
					diff/ECLOCKS_PER_SEC, diff%ECLOCKS_PER_SEC, res->n_maxGPlayers, res->n_expectedPlayers);
		} else {
			int diff = res->allGPlayerTime - res->dpOpenTime;
			fprintf(log, "%d.%03d seconds until group player list completed.\n",
					diff/ECLOCKS_PER_SEC, diff%ECLOCKS_PER_SEC);
		}
	}
	if (res->n_expectedpVars != 0) {
		if(res->allPVPlayerTime == -1) {
			int diff = eclock() - res->dpOpenTime;
			fprintf(log, "INCOMPLETE: %d.%03d seconds until all %d player variables for %d of %d players found.\n",
					diff/ECLOCKS_PER_SEC, diff%ECLOCKS_PER_SEC, res->n_expectedpVars, res->n_maxPVPlayers, res->n_expectedPlayers);
		} else {
			int diff = res->allPVPlayerTime - res->dpOpenTime;
			fprintf(log, "%d.%03d seconds until all %d player variables for all players completed.\n",
					diff/ECLOCKS_PER_SEC, diff%ECLOCKS_PER_SEC, res->n_expectedpVars);
		}
	}
	if(res->dpCreatePlayerCallbackTime == -1) {
		fprintf(log, "Create player callback never completed.\n");
	} else {
		int diff = res->dpCreatePlayerCallbackTime - res->dpOpenTime;
		fprintf(log, "%d.%03d seconds until player creation callback.\n",
				diff/ECLOCKS_PER_SEC, diff%ECLOCKS_PER_SEC);
	}
	if(res->dpCreatePlayerTime == -1) {
		fprintf(log, "Player create never completed.\n");
	} else {
		int diff = res->dpCreatePlayerTime - res->dpOpenTime;
		fprintf(log, "%d.%03d seconds until player creation returned.\n",
				diff/ECLOCKS_PER_SEC, diff%ECLOCKS_PER_SEC);
	}
	if(res->dpOpenCallbackTime == -1) {
		fprintf(log, "Session open callback never completed.\n");
	} else {
		int diff = res->dpOpenCallbackTime - res->dpOpenTime;
		fprintf(log, "%d.%03d seconds until session open callback.\n",
				diff/ECLOCKS_PER_SEC, diff%ECLOCKS_PER_SEC);
	}
}

/*-------------------------------------------------------------------------
 Stop the test
-------------------------------------------------------------------------*/
void
abortTest(
	FILE* log,
	int exitCode,
	char* message,
	pspResult_t* res)
{
	DPRINT(("abortTest: %s\n", message));
	if(res != NULL) {
		fprintf(log, "%d: abortTest--%s\n", res->thisPlayer, message);
		printf("%d: abortTest--%s\n", res->thisPlayer, message);
		writeReport(log, res);
	} else {
		fprintf(log, "abortTest--%s\n", message);
		printf("abortTest--%s\n", message);
	}
	fclose(log);
	exit(exitCode);
}

/*-------------------------------------------------------------------------
 Handle a signal (^C or similar)
-------------------------------------------------------------------------*/
void
signalHandler(
	int sigId)
{
	sigId = sigId;
	abortTest(logFile, psp_RES_SIGNAL, "Test terminated at user request.\n", &results);
}

/*-------------------------------------------------------------------------
 Set the player as having acknowledged.
-------------------------------------------------------------------------*/
void
setAck(
	boolean value,
	dpid_t idFrom,		/* The player */
	pspResult_t* res)
{
	int i;
	char buf[BUFSIZE];
	boolean allAck = TRUE;
	int found = FALSE;

	for(i = 0; i < res->n_expectedPlayers; i++) {
		if(idFrom == res->playerID[i]) {
			DPRINT(("PSP: acks[%d](%d) = %d\n", i, idFrom, value));
			res->acks[i] = value;
			found = TRUE;
		}
		allAck = (boolean) (allAck && res->acks[i]);
	}
	if (!found) {
		sprintf(buf, "setAck: err: got ack from id %d not in res array; got record after we timed out?", idFrom);
		abortTest(logFile, 2, buf, &results);
	}
	res->allAcks = allAck;
}

/*-------------------------------------------------------------------------
 Get whether the player has acknowledged.
-------------------------------------------------------------------------*/
boolean
getAck(
	dpid_t idFrom,		/* The player */
	pspResult_t* res)
{
	int i;

	for(i = 0; i < res->n_expectedPlayers; i++) {
		if(idFrom == res->playerID[i])
			return res->acks[i];
	}
	abortTest(logFile, psp_RES_BADACK, "getAck: bad arg\n", &results);

}

/*-------------------------------------------------------------------------
 Callback triggered by dpOpen.
-------------------------------------------------------------------------*/
int dp_PASCAL
open_cb(
	dp_session_t *ps,
	long *pTimeout,
	long flags,
	void *context)
{
	pspResult_t* res = (pspResult_t *) context;
	res->dpOpenCallbackTime = eclock();

	res->callbacksFinished = TRUE;
	if (ps == NULL)
		abortTest(logFile, 3, "open_cb: unable to join session.\n", &results);

	(void) pTimeout;
	(void) flags;
	return (ps != NULL);
}

/*-------------------------------------------------------------------------
 Callback triggered by listing sessions.
-------------------------------------------------------------------------*/
int dp_PASCAL
listSessions_cb(
	dp_session_t *sDesc,
	long *pTimeout,
	long flags,
	void *context)
{
	pspResult_t* res = (pspResult_t *) context;

	if (!sDesc) {
		DPRINT(("listSessions_cb: session list complete.\n"));
		res->callbacksFinished = TRUE;
		return FALSE;
	}
	if (res->n_sessions >= MAX_SESSIONS) {
		DPRINT(("listSessions_cb: session list overflow.\n"));
		res->callbacksFinished = TRUE;
		return FALSE;
	}
	DPRINT(("listSessions_cb: session %d = %s\n",res->n_sessions, sDesc->sessionName));
	res->sessions[res->n_sessions] = *sDesc;
	res->n_sessions++;
	sDesc->szPassword[dp_PASSWORDLEN-1] = 0;

	(void) pTimeout;
	(void) flags;

	return TRUE;
}

/*-------------------------------------------------------------------------
 Join the first session that matches the given name.
 User must have called listSesions() first.
-------------------------------------------------------------------------*/
dp_result_t
joinSessionByName(
	dp_t *dp,
	char *name,
	void* context)
{
	int i;
	dp_result_t err;
	pspResult_t* res = (pspResult_t *) context;

	/* Find the given session. */
	for (i = 0; i < res->n_sessions; i++) {
		DPRINT(("jsbn: %s =? %s\n",res->sessions[i].sessionName, name));
		if (strnicmp(res->sessions[i].sessionName, name, strlen(name)) == 0)
			break;
	}
	if (i == res->n_sessions)
		return dp_RES_EMPTY;

	/* Join it. */
	res->dpOpenTime = eclock();
	err = dpOpen(dp, &(res->sessions[i]), open_cb, context);

	return err;
}

/*-------------------------------------------------------------------------
 Callback triggered by creating a player.
-------------------------------------------------------------------------*/
void dp_PASCAL
create_player_cb(
	dpid_t id,
	char_t *name,
	long flags,
	void *context)
{
	int i = 0;
	pspResult_t* res = (pspResult_t *) context;
	res->dpCreatePlayerCallbackTime = eclock();

	if (id != dp_ID_NONE)
		res->myID = id;
	res->callbacksFinished = TRUE;

	/* Store player ID in our list, if it's not already there */
	for(i = 0; i < res->n_expectedPlayers; i++) {
		if (res->playerID[i] == id) {
			break;
		}
	}
	if(i >= res->n_expectedPlayers) {
		for(i = 0; i < res->n_expectedPlayers; i++) {
			if (res->playerID[i] == dp_ID_NONE) {
				res->playerID[i] = id;
				break;
			}
		}
	}
	setAck(TRUE, id, res);

	(void) name;
	(void) flags;
}

/*-------------------------------------------------------------------------
 Callback triggered by listing players.
-------------------------------------------------------------------------*/
void dp_PASCAL
listPlayers_cb(
	dpid_t id,
	char_t *name,
	long flags,
	void *context)
{
	pspResult_t* res = (pspResult_t *) context;
	int i;

	if (id != dp_ID_NONE) {
		res->n_players++;
		/* Store player ID in our list, if it's not already there */
		for(i = 0; i < res->n_expectedPlayers; i++) {
			if (res->playerID[i] == id) {
				break;
			}
		}
		if(i >= res->n_expectedPlayers) {
			for(i = 0; i < res->n_expectedPlayers; i++) {
				if (res->playerID[i] == dp_ID_NONE) {
					res->playerID[i] = id;
					break;
				}
			}
		}
	} else {
		res->callbacksFinished = TRUE;
		if(res->n_players >= res->n_expectedPlayers)
			res->allPlayerTime = eclock();
	}

	(void) name;
	(void) flags;
}

/*-------------------------------------------------------------------------
 Callback triggered by listing groups.
-------------------------------------------------------------------------*/
void dp_PASCAL
listGroups_cb(
	dpid_t id,
	char_t *name,
	long flags,
	void *context)
{
	pspResult_t* res = (pspResult_t *) context;

	if (id != dp_ID_NONE) {
		res->groupID = id;
		res->n_groups++;
	} else {
		res->callbacksFinished = TRUE;
	}

	(void) name;
	(void) flags;
}

/*-------------------------------------------------------------------------
 Callback triggered by listing group players.
-------------------------------------------------------------------------*/
void dp_PASCAL
listGroupPlayers_cb(
	dpid_t id,
	char_t *name,
	long flags,
	void *context)
{
	pspResult_t* res = (pspResult_t *) context;
	int i;

	if (id != dp_ID_NONE) {
		res->n_gplayers++;
		/* Store player ID in our list, if it's not already there */
		for(i = 0; i < res->n_expectedPlayers; i++) {
			if (res->gplayerID[i] == id) {
				break;
			}
		}
		if(i >= res->n_expectedPlayers) {
			for(i = 0; i < res->n_expectedPlayers; i++) {
				if (res->gplayerID[i] == dp_ID_NONE) {
					res->gplayerID[i] = id;
					break;
				}
			}
		}
	} else {
		res->callbacksFinished = TRUE;
		if(res->n_gplayers >= res->n_expectedPlayers)
			res->allGPlayerTime = eclock();
	}

	(void) name;
	(void) flags;
}

/*-------------------------------------------------------------------------
 Handle a received message, if any.
-------------------------------------------------------------------------*/
void
processMessage(dp_t* myDP, pspResult_t* res)
{
	dpid_t idFrom;
	dpid_t idTo;
	char buf[dpio_MAXLEN_UNRELIABLE];
	void *pbuf = &buf[0];
	unsigned int size = dpio_MAXLEN_UNRELIABLE;
	dp_result_t err;

	/* Get a packet */
	err = dpReceive(myDP, &idFrom, &idTo, RECEIVE_FLAGS, buf, &size);

	/* Handle empty packets */
	if (err == dp_RES_EMPTY) {
		#if defined(WIN32)
			if((res->emptyCount++ > 3) && (res->yield_time > 0))
				Sleep(res->yield_time);
		#endif
		return;
	}
	#if defined(WIN32)
		res->emptyCount = 0;
	#endif

	/* Abort test for loss of host or session */
	if (err == dp_RES_HOST_NOT_RESPONDING) {
		abortTest(logFile, psp_RES_HOSTLOST, "Host not responding.", res);
	}

	/* Handle real packets */
	if ((err == dp_RES_OK) && (size > 0)) {
		if(res->in_session && (*((dp_packetType_t*)buf) == dp_SESSIONLOST_PACKET_ID)) {
			abortTest(logFile, psp_RES_SESSIONLOST, "Session lost.", res);
		}

		switch ((*((dp_packetType_t *)pbuf))) {
			case test_ALL_RECEIVED:
				if (res->myID == 0) {
					abortTest(logFile, psp_RES_IDZERO, "Hey!  My id is still zero when I get my first message!", NULL);
				}

				if(idTo == res->myID) {
					setAck(TRUE, idFrom, res);
				}
				break;
			case dp_USER_ADDPLAYER_PACKET_ID: {
			/* detect only as check if player record or player variable record
			 * arrives first; add player to res array using dpEnumPlayers */
				int i;
				char printbuf[BUFSIZE];
				struct {
					dp_packetType_t tag PACK;
					dp_user_addPlayer_packet_t pdat PACK;
				} *pkt = pbuf;
				if (res->n_expectedpVars != 0) {
					for (i = 0; i < res->n_expectedPlayers; i++) {
						if (res->pvplayerID[i] == pkt->pdat.id) {
							sprintf(printbuf, "processMessage: err: either got player variable before player record or duplicate player record for id %d\n", pkt->pdat.id);
							abortTest(logFile, 2, printbuf, res);
						}
					}
					if (i >= res->n_expectedPlayers) {
						for(i = 0; i < res->n_expectedPlayers; i++) {
							if (res->pvplayerID[i] == dp_ID_NONE) {
								res->pvplayerID[i] = pkt->pdat.id;
								break;
							}
						}
					}
				}
				break;
			}
			case dp_USER_PLAYERDATA_PACKET_ID: {
				int i;
				char *ipvbuf;
				char printbuf[BUFSIZE];
				long base;
				struct {
					dp_packetType_t tag PACK;
					dp_user_playerData_packet_t pdat PACK;
				} *pkt = pbuf;
				// find player record for player variable
				for (i = 0; i < res->n_expectedPlayers; i++)
					if (res->pvplayerID[i] == pkt->pdat.id) break;
				if (i >= res->n_expectedPlayers) {
					sprintf(printbuf, "processMessage: err: got player variable before player record for id %d\n", pkt->pdat.id);
					abortTest(logFile, 2, printbuf, res);
				}
				// check if value of pvbuf is correct
				base = ((unsigned long)(pkt->pdat.key << 16) | (unsigned short)SwapBytes(pkt->pdat.id)) + pkt->pdat.key;
				if(pkt->pdat.len != (pkt->pdat.key * 25 + 1) * sizeof(base)) {
					sprintf(printbuf, "processMessage: err: incorrect buf size %d for player index %d; should be %d", pkt->pdat.len, pkt->pdat.key, (pkt->pdat.key * 25 + 1) * sizeof(base));
					abortTest(logFile, 2, printbuf, res);
				}
				for (ipvbuf = (char *)(pkt->pdat.data); ipvbuf < (char *)(pkt->pdat.data) + pkt->pdat.len; ipvbuf += sizeof(base)) {
					if(*((long*)ipvbuf) != base) {
						sprintf(printbuf, "processMessage: err: got incorrect pvbuf value at %d byte position; base, val, playerId, index are %ld, %ld, %d, %d", ipvbuf - (char *)(pkt->pdat.data), base, *((long*)ipvbuf), pkt->pdat.id, pkt->pdat.key);
						abortTest(logFile, 2, printbuf, res);
					}
				}
				res->npv[i]++;
				break;
			}
			default:
				break;
		}
	} else {
		DPRINT(("Error in dpReceive.\n"));
		printf("%d: Got err=%d from dpReceive\n", res->thisPlayer, err);
	}
}

/*-------------------------------------------------------------------------
 Tell other tests we're done, and wait for them to be done too.
-------------------------------------------------------------------------*/
void synchronize_done_simple(
	dp_t *myDP,
	pspResult_t *res,
	boolean timedOut)
{
	char buf[BUFSIZE];
	int i;
	int size;
	time_t endtime;

	buf[0] = test_ALL_RECEIVED & 0xff;
	buf[1] = ((test_ALL_RECEIVED) >> 8) & 0xff;
	size = 2;

	/* Tell everyone else we're done. */
	for(i = 0; i < res->n_expectedPlayers; i++) {
		if (res->playerID[i] == res->myID)
			continue;
		if((res->myID != dp_ID_NONE) && (res->playerID[i] != dp_ID_NONE))
			dpSend(myDP, res->myID, res->playerID[i],
						dp_SEND_RELIABLE, buf, size);
	}

	/* Wait a while to allow other tests to complete */
	if(!timedOut) {
		DPRINT(("Test complete; waiting for other tests.\n", res->thisPlayer));
	} else {
		DPRINT(("Test failed to find other players or group or player variables.\n"));
		printf("%d: Didn't find other players or group or player variables.\n", res->thisPlayer);
	}
	printf("%d: Complete; waiting for other tests.\n", res->thisPlayer);
	endtime = eclock() + test_TIMEOUT_DELAY;
	while (((long)(eclock() - endtime) < 0) && (!res->allAcks))
		processMessage(myDP, &results);
}

/*-------------------------------------------------------------------------
 Tell the master we're done, and wait for it to tell us everyone else is
 done.
 Assuming that everyone finds out the master's address before their own,
 this avoids the race condition at the start of a session where
 some players don't know the player id yet that appears as the source or
 destination address of a message.
-------------------------------------------------------------------------*/
void synchronize_done_via_host(
	dp_t *myDP,
	pspResult_t *res,
	int iteration,
	boolean timedOut)
{
	dpid_t master;
	char buf[BUFSIZE];
	int i;
	int size;
	time_t endtime;

	/* Tell other tests we're done */
	buf[0] = test_ALL_RECEIVED & 0xff;
	buf[1] = ((test_ALL_RECEIVED) >> 8) & 0xff;
	size = 2;

	/* Figure out who's the master.  Master has lowest dpid. */
	master = dp_ID_NONE;
	for(i = 0; i < res->n_expectedPlayers; i++)
		if (res->playerID[i] < master)
				master = res->playerID[i];

	if (master == res->myID) {
		/* If we're the master, wait for everyone to ack. */
		if (timedOut) {
			printf("%d.%d: Master didn't find other players or group or player variables.\n", res->thisPlayer, iteration);
		} else {
			printf("%d.%d: Complete; master waiting for others.\n", res->thisPlayer, iteration);
		}
		endtime = eclock() + test_TIMEOUT_DELAY;
		while (((long)(eclock() - endtime) < 0) && (!res->allAcks))
			processMessage(myDP, &results);
		if ((long)(eclock() - endtime) >= 0)
			printf("%d.%d: Master timed out waiting for other players to ack.\n", res->thisPlayer, iteration);
		printf("%d.%d: Master really complete.\n", res->thisPlayer, iteration);

		/* Then tell everyone else we're done. */
		for(i = 0; i < res->n_expectedPlayers; i++) {
			if (res->playerID[i] == res->myID)
				continue;
			if((res->myID != dp_ID_NONE) && (res->playerID[i] != dp_ID_NONE))
				dpSend(myDP, res->myID, res->playerID[i],
							dp_SEND_RELIABLE, buf, size);
		}
	} else {
		/* Else tell the master we're done, and wait for master to ack. */
		if((res->myID != dp_ID_NONE) && (master != dp_ID_NONE))
			dpSend(myDP, res->myID, master, dp_SEND_RELIABLE, buf, size);

		if (timedOut) {
			printf("%d.%d: Didn't find other players or group or player variables.\n", res->thisPlayer, iteration);
		}
		printf("%d.%d: Complete; waiting for other hosts.\n", res->thisPlayer, iteration);
		endtime = eclock() + test_TIMEOUT_DELAY;
		while (((long)(eclock() - endtime) < 0) && !getAck(master, res))
			processMessage(myDP, &results);
		printf("%d.%d: Really complete.\n", res->thisPlayer, iteration);
	}
}

/*-------------------------------------------------------------------------
 Handle command-line arguments
-------------------------------------------------------------------------*/
int						/* status */
handleArguments(
	pspArgs_t *arg,		/* (output) values */
	FILE** logfile,		/* (output) log file */
	int argc,			/* (input) number of arguments */
	char *argv[] )		/* (input) value of arguments */
{
	char buf[BUFSIZE];
	char usage[BUFSIZE];
	int i;

	sprintf(usage,
"Usage: %s -c=player_count [-m=player] [-i=packet_loss] [-t=timeout]\n\
  [-p=port] [-d=driver] [-s=server] [-n=times] [-l=log_file] [-g=group_name]\n\
  [-v=player_variable_count]", argv[0]);

	/* Initialize arguments */
	results.driver = DEFAULT_DRIVER;
	arg->server = NULL;
	results.room = DEFAULT_ROOM;
	results.thisPlayer = -1;
	results.n_expectedPlayers = 0;
	arg->packetLoss = 0;
	arg->portnum = comm_PORT_ANY;
	arg->extra_timeout = 0;
	results.yield_time = YIELD_DEFAULT;
	results.beginPlayer = 0;
	arg->endPlayer = -1;
	*logfile = stdout;
	arg->group = NULL;
	results.n_expectedpVars = 0;

	/* Handle each command-line argument */
	for(i = 0; i < argc; i++) {
		if((argv[i][0] == '-') || (argv[i][0] == '/')) {
			if ((argv[i][1] == '\0') || ((argv[i][2] != '=') &&
					(argv[i][2] != ':') && (argv[i][2] != '\0'))) {
				sprintf(buf, "Malformed switch: %s\n%s", argv[i], usage);
				abortTest(*logfile, 3, buf, NULL);
			} else if((argv[i][2] != '\0') && (argv[i][3] == '\0')) {
				sprintf(buf, "Value expected: %s\n%s", argv[i], usage);
				abortTest(*logfile, 3, buf, NULL);
			}
			switch(argv[i][1]) {
				case 'b': case 'B':	/* begin spawning at this player */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(results.beginPlayer)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortTest(*logfile, 3, buf, NULL);
					}
					break;
				case 'c': case 'C':	/* player count */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(results.n_expectedPlayers)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortTest(*logfile, 3, buf, NULL);
					}
					break;
				case 'd': case 'D':	/* driver filename */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					}
					results.driver = argv[i]+3;
					break;
				case 'e': case 'E':	/* end spawning at this player */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(arg->endPlayer)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortTest(*logfile, 3, buf, NULL);
					}
					break;
				case 'f': case 'F': /* freeze on this iteration */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(results.freezeLoop)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortTest(*logfile, 3, buf, NULL);
					}
					break;
				case 'g': case 'G': /* create group with this name */
					if(argv[i][2] == '\0') {
						arg->group = "g1";
					} else {
						arg->group = argv[i]+3;
					}
					break;
				case 'i': case 'I':	/* packet loss */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(arg->packetLoss)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortTest(*logfile, 3, buf, NULL);
					}
					break;
				case 'l': case 'L':	/* log filename */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					} else if(results.loops == -1) {
						if((*logfile = fopen(argv[i]+3, "a")) == NULL) {
							sprintf(buf, "Error opening log file %s\n%s", argv[i]+3, usage);
							abortTest(stdout, 3, buf, NULL);
						}
					} else if((*logfile = fopen(argv[i]+3, "w")) == NULL) {
						sprintf(buf, "Error opening log file %s\n%s", argv[i]+3, usage);
						abortTest(stdout, 3, buf, NULL);
					}
					break;
				case 'm': case 'M':	/* player (me) */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(results.thisPlayer)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortTest(*logfile, 3, buf, NULL);
					}
					break;
				case 'n': case 'N':	/* loop */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(results.loops)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortTest(*logfile, 3, buf, NULL);
					}
					break;
				case 'p': case 'P':	/* port number */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					} else if (0 == stricmp(argv[i]+3, "any")) {
						arg->portnum = 0;
					} else if(sscanf(argv[i]+3, "%ld", &(arg->portnum)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortTest(*logfile, 3, buf, NULL);
					}
					break;
				case 'r': case 'R':	/* room name */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					}
					results.room = argv[i]+3;
					break;
				case 's': case 'S':	/* server address */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					}
					if(argv[i][3] != '-')
						arg->server = argv[i]+3;
					else
						arg->server = NULL;
					break;
				case 't': case 'T':	/* timeout (msec) */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%ld", &(arg->extra_timeout)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortTest(*logfile, 3, buf, NULL);
					}
					break;
				case 'v': case 'V':	/* player variable count */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(results.n_expectedpVars)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortTest(*logfile, 3, buf, NULL);
					}
					break;
				case 'y': case 'Y':	/* yield time (msec) */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(*logfile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(results.yield_time)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortTest(*logfile, 3, buf, NULL);
					}
					break;
				default:
					sprintf(buf,"Illegal switch: %s", argv[i]);
					abortTest(*logfile, 3, buf, NULL);
			}
		}
	}

	if(results.thisPlayer < -1)
		abortTest(*logfile, 3, "Player number must be nonnegative.", NULL);
	if(results.n_expectedPlayers < 2)
		abortTest(*logfile, 3, "Must have at least two players.", NULL);
	if(results.thisPlayer >= results.n_expectedPlayers)
		abortTest(*logfile, 3, "Player number must smaller than player count.", NULL);
	if(test_YIELD_MAX < results.yield_time)
		results.yield_time = test_YIELD_MAX;
	if((arg->endPlayer < 0) || (arg->endPlayer > results.n_expectedPlayers))
		arg->endPlayer = results.n_expectedPlayers;
	if(arg->endPlayer <= results.beginPlayer)
		abortTest(*logfile, 3, "Player range must include one or more players.", NULL);
	results.n_localPlayers = arg->endPlayer - results.beginPlayer;

	return 0;
}

/*-------------------------------------------------------------------------
 Launch all tests
-------------------------------------------------------------------------*/
int launch (
	int playerCount,		/* Total number to launch */
	int beginPlayer,
	int endPlayer,
	int packetLoss,
	int pVarCount,
	unsigned long port,
	unsigned long timeout,
	char* test,
	char* driver,
	char* server,
	char* room,
	char* group,
	int** procs)
{
	char * childArgs[15] = {
		NULL,
		"-m=whichPlyr",
		"-c=countPlyr",
		"-i=packetLoss",
		"-t=timeout#####",
		"-p=port########",
		"-d=driver_name_complete_path_specified_xxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"-s=server.network.address.specification",
		"-l=logfile.xxxxxxxxxxxxxx",
		"-r=name_of_the_room_to_join_on_the_server",
		"-g=name_of_the_group_to_join_in_the_session",
		"-n=loopCount",
		"-f=freezeLoop",
		"-v=countPlayerVariables",
		NULL
	};

	int i;
	char buf[BUFSIZE];
	int rVal;

	/* allocate storage */
	if((*procs = (int *) malloc(playerCount * sizeof(int))) == NULL)
		abortTest(logFile, 3, "Unable to allocate process handle storage.\n", NULL);

	/* Start tests */
	childArgs[0] = test;
	sprintf(childArgs[2], "-c=%d", playerCount);
	sprintf(childArgs[3], "-i=%d", packetLoss);
	sprintf(childArgs[4], "-t=%ld", timeout);
	sprintf(childArgs[13],"-v=%d", pVarCount);
	if(driver != NULL)
		sprintf(childArgs[6], "-d=%s", driver);
	else
		sprintf(childArgs[6], "-d=%s", DEFAULT_DRIVER);
	if(server != NULL)
		sprintf(childArgs[7], "-s=%s", server);
	else
		sprintf(childArgs[7], "-s=-");
	if(room != NULL)
		sprintf(childArgs[9], "-r=%s", room);
	else
		sprintf(childArgs[9], "-r=%s", DEFAULT_ROOM);
	if(group != NULL)
		sprintf(childArgs[10], "-g=%s", group);
	else
		sprintf(childArgs[10], "");
	sprintf(childArgs[11], "-n=%d", results.loops);
	for(i = beginPlayer; i < endPlayer; i++) {
		sprintf(childArgs[1], "-m=%d", i);
		if((0 == port) || (comm_PORT_ANY == port))
			sprintf(childArgs[5], "-p=any");
		else
			sprintf(childArgs[5], "-p=%d", port+i);
		sprintf(childArgs[8], "-l=RawPSP%d.%d.log", packetLoss, i);
		sprintf(childArgs[12], "-f=%d", 2*i+1);
		(*procs)[i] = _spawnv(_P_NOWAIT, childArgs[0], childArgs);
		if((*procs)[i] == -1) {
			sprintf(buf, "Unable to start process %d: %s\n", i, strerror(errno));
			abortTest(logFile, 4, buf, NULL);
		}
	}

	/* Wait for results */
	rVal = 0;
	for(i = beginPlayer; i < endPlayer; i++) {
		int result;
		_cwait(&result, (*procs)[i], 0);
		if(result != 0)
			rVal = result;
	}
	return rVal;
}

/*-------------------------------------------------------------------------
 Collate results from all tests
-------------------------------------------------------------------------*/
int collate (
	int playerCount,		/* Total number launched */
	int beginPlayer,
	int endPlayer,
	int packetLoss,
	int pVarCount,
	char *group,
	int* procs)
{
	int* result = NULL;
	int* found = NULL;
	float* endTime = NULL;
	int* Gresult = NULL;
	int* Gfound = NULL;
	float* GendTime = NULL;
	int* PVresult = NULL;
	int* PVfound = NULL;
	float* PVendTime = NULL;

	float foundMean = 0.0f;
	float endMean = 0.0f;
	float endSD = 0.0f;
	int completionCount;
	float GfoundMean = 0.0f;
	float GendMean = 0.0f;
	float GendSD = 0.0f;
	int GcompletionCount;
	float PVfoundMean = 0.0f;
	float PVendMean = 0.0f;
	float PVendSD = 0.0f;
	int PVcompletionCount;
	int rVal;

	char buf[BUFSIZE];
	int i;

	int loopCount = results.loops;

	/* Allocate storage */
	if((result = (int *) malloc(playerCount * loopCount * sizeof(int))) == NULL)
		abortTest(logFile, 3, "Unable to allocate result storage.\n", NULL);
	if((found = (int *) malloc(playerCount * loopCount * sizeof(int))) == NULL)
		abortTest(logFile, 3, "Unable to allocate found storage.\n", NULL);
	if((endTime = (float *) malloc(playerCount * loopCount * sizeof(float))) == NULL)
		abortTest(logFile, 3, "Unable to allocate time storage.\n", NULL);
	if (group) {
		if((Gresult = (int *) malloc(playerCount * loopCount * sizeof(int))) == NULL)
			abortTest(logFile, 3, "Unable to allocate result storage.\n", NULL);
		if((Gfound = (int *) malloc(playerCount * loopCount * sizeof(int))) == NULL)
			abortTest(logFile, 3, "Unable to allocate found storage.\n", NULL);
		if((GendTime = (float *) malloc(playerCount * loopCount * sizeof(float))) == NULL)
			abortTest(logFile, 3, "Unable to allocate time storage.\n", NULL);
	}
	if (pVarCount) {
		if((PVresult = (int *) malloc(playerCount * loopCount * sizeof(int))) == NULL)
			abortTest(logFile, 3, "Unable to allocate result storage.\n", NULL);
		if((PVfound = (int *) malloc(playerCount * loopCount * sizeof(int))) == NULL)
			abortTest(logFile, 3, "Unable to allocate found storage.\n", NULL);
		if((PVendTime = (float *) malloc(playerCount * loopCount * sizeof(float))) == NULL)
			abortTest(logFile, 3, "Unable to allocate time storage.\n", NULL);
	}

	/* Wait for tests to finish */
	for(i = beginPlayer; i < endPlayer; i++) {
		rVal = _cwait(&(result[i]), procs[i], 0);
	}

	/* Get test results */
	for(i = beginPlayer; i < endPlayer; i++) {
		FILE* fp;
		int cnt;
		float val;
		char test[BUFSIZE];
		sprintf(buf, "RawPSP%d.%d.log", packetLoss, i);
		if((fp = fopen(buf, "r")) != NULL) {
			int j = (i-beginPlayer)*loopCount;
			found[j] = playerCount;
			endTime[j] = -1.0f;
			if (group) {
				Gfound[j] = playerCount;
				GendTime[j] = -1.0f;
			}
			if (pVarCount) {
				PVfound[j] = playerCount;
				PVendTime[j] = -1.0f;
			}
			while(fgets(buf, BUFSIZE, fp) != NULL) {
				if(sscanf(buf, "%s", &test) == 1) {
					if(strcmpi(test, "looping") == 0) {
						j++;
						found[j] = playerCount;
						endTime[j] = -1.0f;
						if (group) {
							Gfound[j] = playerCount;
							GendTime[j] = -1.0f;
						}
						if (pVarCount) {
							PVfound[j] = playerCount;
							PVendTime[j] = -1.0f;
						}
					}
				}
				if(sscanf(buf, "%f%*s%*s%*s%s", &val, &test) == 2) {
					if(strcmpi(test, "list") == 0)
						endTime[j] = val;
				}
				if(sscanf(buf, "%*s%*f%*s%*s%d%*s%*d%s", &cnt, &test) == 2) {
					if(strcmpi(test, "players") == 0)
						found[j] = cnt;
				}
				if (group) {
					if(sscanf(buf, "%f%*s%*s%*s%*s%s", &val, &test) == 2) {
						if(strcmpi(test, "list") == 0)
							GendTime[j] = val;
					}
					if(sscanf(buf, "%*s%*f%*s%*s%d%*s%*d%s", &cnt, &test) == 2) {
						if(strcmpi(test, "group") == 0)
							Gfound[j] = cnt;
					}
				}
				if (pVarCount) {
					int chkVarCount = 0;
					if(sscanf(buf, "%f%*s%*s%*s%d%*s%s", &val, &chkVarCount, &test) == 3) {
						if(strcmpi(test, "variables") == 0)
							PVendTime[j] = val;
						if (chkVarCount != pVarCount)
							printf("error: chkVarCount %d, pVarCount %d different for player %d\n", chkVarCount, pVarCount, i);
					}
					if(sscanf(buf, "%*s%*f%*s%*s%*s%d%*s%s%*s%d", &chkVarCount, &test, &cnt) == 3) {
						if(strcmpi(test, "variables") == 0)
							PVfound[j] = cnt;
						if (chkVarCount != pVarCount)
							printf("error: chkVarCount %d, pVarCount %d different for player %d\n", chkVarCount, pVarCount, i);
					}
				}
			}
		}
	}

	/* Compute average and standard deviation */
	completionCount = 0;
	GcompletionCount = 0;
	PVcompletionCount = 0;
	for(i = 0; i < (endPlayer-beginPlayer)*loopCount; i++) {
		foundMean += found[i];
		if (endTime[i] >= 0.0) {
			endMean += endTime[i];
			completionCount++;
		}
		if (group) {
			GfoundMean += Gfound[i];
			if(GendTime[i] >= 0.0) {
				GendMean += GendTime[i];
				GcompletionCount++;
			}
		}
		if (pVarCount) {
			PVfoundMean += PVfound[i];
			if(PVendTime[i] >= 0.0) {
				PVendMean += PVendTime[i];
				PVcompletionCount++;
			}
		}
	}
	foundMean /= results.n_localPlayers * loopCount;
	if (completionCount > 0)
		endMean /= completionCount;
	if (completionCount > 2) {
		for(i = 0; i < (endPlayer-beginPlayer)*loopCount; i++) {
			if (endTime[i] >= 0.0)
				endSD += (endTime[i] - endMean) * (endTime[i] - endMean);
		}
		endSD = (float) sqrt(endSD / (completionCount-1));
	} else
		endSD = -1.0f;
	if (group) {
		GfoundMean /= results.n_localPlayers * loopCount;
		if (GcompletionCount > 0)
			GendMean /= GcompletionCount;
		if (GcompletionCount > 2) {
			for(i = 0; i < (endPlayer-beginPlayer)*loopCount; i++) {
				if (GendTime[i] >= 0.0)
					GendSD += (GendTime[i] - GendMean) * (GendTime[i] - GendMean);
			}
			GendSD = (float) sqrt(GendSD / (GcompletionCount-1));
		} else
			GendSD = -1.0f;
	}
	if (pVarCount) {
		PVfoundMean /= results.n_localPlayers * loopCount;
		if (PVcompletionCount > 0)
			PVendMean /= PVcompletionCount;
		if (PVcompletionCount > 2) {
			for(i = 0; i < (endPlayer-beginPlayer)*loopCount; i++) {
				if (PVendTime[i] >= 0.0)
					PVendSD += (PVendTime[i] - PVendMean) * (PVendTime[i] - PVendMean);
			}
			PVendSD = (float) sqrt(PVendSD / (PVcompletionCount-1));
		} else
			PVendSD = -1.0f;
	}

	/* Report results */
	fprintf(logFile, "\n%d%% packet loss.\n", packetLoss);
	fprintf(logFile, "%d of %d local player lists completed; average of %.2f of %d players found.\n",
			completionCount / loopCount, results.n_localPlayers, foundMean, results.n_expectedPlayers);
	if (completionCount > 0) {
		if (endSD >= 0.0f)
			fprintf(logFile, "Average completion time was %0.2f +/- %0.2f seconds.\n",
					endMean, endSD);
		else
			fprintf(logFile, "Average completion time was %0.2f seconds.\n", endMean);
	}
	if (group) {
		fprintf(logFile, "%d of %d local group player lists completed; average of %.2f of %d group players found.\n",
				GcompletionCount / loopCount, results.n_localPlayers, GfoundMean, results.n_expectedPlayers);
		if (GcompletionCount > 0) {
			if (GendSD >= 0.0f)
				fprintf(logFile, "Average completion time was %0.2f +/- %0.2f seconds.\n",
						GendMean, GendSD);
			else
				fprintf(logFile, "Average completion time was %0.2f seconds.\n", GendMean);
		}
	}
	if (pVarCount) {
		fprintf(logFile, "all %d player variables found for %d of %d players; found all player variables for average of %.2f of %d players.\n", pVarCount,
				PVcompletionCount / loopCount, results.n_localPlayers, PVfoundMean, results.n_expectedPlayers);
		if (PVcompletionCount > 0) {
			if (PVendSD >= 0.0f)
				fprintf(logFile, "Average completion time was %0.2f +/- %0.2f seconds.\n",
						PVendMean, PVendSD);
			else
				fprintf(logFile, "Average completion time was %0.2f seconds.\n", PVendMean);
		}
	}

	/* Return success or failure */
	if (completionCount == results.n_localPlayers * loopCount) {
		if ((!group || GcompletionCount == results.n_localPlayers * loopCount)
			&& (!pVarCount || PVcompletionCount == results.n_localPlayers * loopCount))
			return 0;
		else
			return 1;
	}
	return 1;
}

/*-------------------------------------------------------------------------
 Run the test for one player
-------------------------------------------------------------------------*/
int testOne (
	int thisPlayer,			/* Which player are we */
	int packetLoss,
	int pVarCount,
	unsigned long port,
	unsigned long timeout,
	char* driver,
	char* server,
	char *group)
{
	dp_t* myDP = NULL;
	dp_transport_t CommDLLName;
	commInitReq_t commInitReq;
	dp_session_t sess;
	//dpid_t idFrom;
	//dpid_t idTo;
	dp_result_t err;

	char buf[BUFSIZE];
	char name[64];
	char fname[128];
	time_t startTime;
	time_t endtime;
	int lastCount = 0;
	int lastGCount = 0;
	int lastPVCount = 0;
	boolean timedOut;
	boolean *pvdone;
	int i;
	int iLoop;
	int startLoopAt = 0;
	int thawed = FALSE;
	int hostNum = -1;

	printf("%d: Opening port %ld.\n", thisPlayer, port);

	/* Set library ini file */
	sprintf(name, "PSP%d.ini", packetLoss);
	dpini_SetFile(name);

	/* Set up interrupt handling */
	signal(SIGINT, signalHandler);
	signal(SIGABRT, signalHandler);
	signal(SIGTERM, signalHandler);

	results.in_session = FALSE;
	if(results.loops < 1) {	/* thawing */
		/* Find our file */
		FILE* thawFile;
		sprintf(fname, FREEZENAME2, thisPlayer);
		thawFile = fopen(fname, "rb");
		assert(thawFile != NULL);
		thawed = TRUE;

		/* Read first part from our file */
		fread(&startLoopAt,sizeof(int),1,thawFile);
		fread(&(results.loops),sizeof(int),1,thawFile);
		fread(&(results.n_expectedPlayers),sizeof(int),1,thawFile);
		fread(&(results.n_expectedpVars),sizeof(int),1,thawFile);

		/* Allocate storage */
		if((results.room = (char *) malloc(120 * sizeof(char))) == NULL)
			abortTest(logFile, 3, "Unable to allocate room name.\n", NULL);
		if((results.acks = (boolean *) malloc(results.n_expectedPlayers * sizeof(boolean))) == NULL)
			abortTest(logFile, 3, "Unable to allocate results storage.\n", NULL);
		if((results.playerID = (dpid_t *) malloc(results.n_expectedPlayers * sizeof(dpid_t))) == NULL)
			abortTest(logFile, 3, "Unable to allocate ID storage.\n", NULL);
		for(i = 0; i < results.n_expectedPlayers; i++) {
			results.acks[i] = FALSE;
			results.playerID[i] = dp_ID_NONE;
		}
		if (group) {
			if((results.gplayerID = (dpid_t *) malloc(results.n_expectedPlayers * sizeof(dpid_t))) == NULL)
				abortTest(logFile, 3, "Unable to allocate GID storage.\n", NULL);
			for(i = 0; i < results.n_expectedPlayers; i++) {
				results.gplayerID[i] = dp_ID_NONE;
			}
		}
		if (pVarCount) {
			if((results.pvplayerID = (dpid_t *) malloc(results.n_expectedPlayers * sizeof(dpid_t))) == NULL)
				abortTest(logFile, 3, "Unable to allocate PVID storage.\n", NULL);
			for(i = 0; i < results.n_expectedPlayers; i++) {
				results.pvplayerID[i] = dp_ID_NONE;
			}
			if((results.npv = (int *) malloc(results.n_expectedPlayers * sizeof(int))) == NULL)
				abortTest(logFile, 3, "Unable to allocate npv storage.\n", NULL);
			for(i = 0; i < results.n_expectedPlayers; i++) {
				results.npv[i] = 0;
			}
			if((pvdone = (boolean *) malloc(results.n_expectedPlayers * sizeof(boolean))) == NULL)
				abortTest(logFile, 3, "Unable to allocate pvdone storage.\n", NULL);
			for(i = 0; i < results.n_expectedPlayers; i++) {
				pvdone[i] = FALSE;
			}
		}

		/* Read the rest of our file */
		fread(&(results.n_localPlayers),sizeof(int),1,thawFile);
		fread(&(results.beginPlayer),sizeof(int),1,thawFile);
		fread(&(results.myID),sizeof(dpid_t),1,thawFile);
		fread(&(results.groupID),sizeof(dpid_t),1,thawFile);
		fread(&startTime,sizeof(clock_t),1,thawFile);
		fread(&(results.dpOpenTime),sizeof(clock_t),1,thawFile);
		fread(&(results.dpCreatePlayerTime),sizeof(clock_t),1,thawFile);
		fread(&i,sizeof(int),1,thawFile);
		fread(results.room,sizeof(char),i,thawFile);
		for(i = 0; i < results.n_expectedPlayers; i++) {
			fread(&(results.playerID[i]),sizeof(dpid_t),1,thawFile);
			fread(&(results.acks[i]),sizeof(dpid_t),1,thawFile);
			if (group)
				fread(&(results.gplayerID[i]),sizeof(dpid_t),1,thawFile);
			if (pVarCount) {
				fread(&(results.pvplayerID[i]),sizeof(dpid_t),1,thawFile);
				fread(&(results.npv[i]),sizeof(int),1,thawFile);
			}
		}
		fclose(thawFile);
		sprintf(name, "dpt%d.%d.log", thisPlayer, startLoopAt);
		dp_setLogFname(name);

		/* Read the DP from its file */
		sprintf(fname, FREEZENAME, thisPlayer);
		printf("%d.%d: Thawing from file %s.\n", thisPlayer, startLoopAt, fname);
		err = dpCreate(&myDP, NULL, NULL, fname);
		assert(err == dp_RES_OK);

		/* Re-set any callbacks */
	} else {	/* not thawing */
		sprintf(name, "dpt%d.%d.log", thisPlayer, startLoopAt);
		#if !defined(NDEBUG)
			dp_setLogFname(name);
		#endif

		/* Allocate storage */
		if((results.acks = (boolean *) malloc(results.n_expectedPlayers * sizeof(boolean))) == NULL)
			abortTest(logFile, 3, "Unable to allocate results storage.\n", NULL);
		if((results.playerID = (dpid_t *) malloc(results.n_expectedPlayers * sizeof(dpid_t))) == NULL)
			abortTest(logFile, 3, "Unable to allocate ID storage.\n", NULL);
		for(i = 0; i < results.n_expectedPlayers; i++) {
			results.acks[i] = FALSE;
			results.playerID[i] = dp_ID_NONE;
		}
		if (group) {
			if((results.gplayerID = (dpid_t *) malloc(results.n_expectedPlayers * sizeof(dpid_t))) == NULL)
				abortTest(logFile, 3, "Unable to allocate GID storage.\n", NULL);
			for(i = 0; i < results.n_expectedPlayers; i++) {
				results.gplayerID[i] = dp_ID_NONE;
			}
		}
		if (pVarCount) {
			if((results.pvplayerID = (dpid_t *) malloc(results.n_expectedPlayers * sizeof(dpid_t))) == NULL)
				abortTest(logFile, 3, "Unable to allocate PVID storage.\n", NULL);
			for(i = 0; i < results.n_expectedPlayers; i++) {
				results.pvplayerID[i] = dp_ID_NONE;
			}
			if((results.npv = (int *) malloc(results.n_expectedPlayers * sizeof(int))) == NULL)
				abortTest(logFile, 3, "Unable to allocate npv storage.\n", NULL);
			for(i = 0; i < results.n_expectedPlayers; i++) {
				results.npv[i] = 0;
			}
			if((pvdone = (boolean *) malloc(results.n_expectedPlayers * sizeof(boolean))) == NULL)
				abortTest(logFile, 3, "Unable to allocate pvdone storage.\n", NULL);
			for(i = 0; i < results.n_expectedPlayers; i++) {
				pvdone[i] = FALSE;
			}
		}

		/* Initialize communications */
		memset(&CommDLLName, 0, sizeof(CommDLLName));
		strncpy(CommDLLName.fname, driver, sizeof(CommDLLName.fname));
		CommDLLName.fname[sizeof(CommDLLName.fname)-1] = '\0';
		memset(&commInitReq, 0, sizeof(commInitReq));
		#ifdef WIN32
			srand(GetTickCount());
		#endif
		/*
			WARNING: Using a randomly generated or time-based session ID
			does not work for this test; the individual runs are started
			at nearly the same time.
			Avoid this type of code:
			commInitReq.sessionId = rand() | (rand() << 16) | time(0) | eclock();
		*/
		commInitReq.sessionId = thisPlayer + 5;	/* avoid special numbers */
		commInitReq.reqLen = sizeof(commInitReq_t);
		commInitReq.modeministr = NULL;
		commInitReq.phonenum = NULL;
		commInitReq.baud = 19200;
		commInitReq.hwirq = 0;
		commInitReq.portnum = port;
		commInitReq.reqLen = sizeof(commInitReq_t);
		err = dpCreate(&myDP, &CommDLLName, &commInitReq, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d creating!", err);
			abortTest(logFile, 2, buf, NULL);
		}

		/* Set the game server */
		if((NULL != server) && (server[0] != '-')) {
			err = dpSetGameServer(myDP, server);
			if (err != dp_RES_OK) {
				sprintf(buf, "Unable to set the game server, error: %d", err);
				abortTest(logFile, 2, buf, NULL);
			}

			/* Wait a little while for the game server to notice us */
			/* There should be a callback that tells us when we're connected! */
			endtime = eclock() + 20 * ECLOCKS_PER_SEC;
			while ((long)(eclock() - endtime) < 0)
				processMessage(myDP, &results);
		}
	}

	/* Run the test itself the requested number of times */
	for(iLoop = startLoopAt; iLoop < results.loops; iLoop++) {
		hostNum = (iLoop/*/2*/) % results.n_expectedPlayers;
		if (thawed) {
			thawed = FALSE;
			results.in_session = TRUE;
		} else { /* not thawed */
			int pvar;
			/* Host/join session */
			startTime = eclock();
			/* Let each one be master ?twice?, in turn */
			if (thisPlayer == hostNum) {
				/* host a session */
				memset(&sess, 0, sizeof(sess));
				sess.sessionType = TEST_SPECIES;
				sess.maxPlayers = (short) results.n_expectedPlayers;
				sprintf(sess.sessionName, "%s%d", results.room, iLoop);
				sess.sessionName[dp_SNAMELEN-1] = '\0';
				printf("%d.%d: Starting session %s.\n", thisPlayer, iLoop, sess.sessionName);
				sess.flags = dp_SESSION_FLAGS_CREATESESSION;
				if (pVarCount)
					sess.flags |= dp_SESSION_FLAGS_ENABLE_PLAYERVARS;
				sess.dwUser1 = 0;		/* Not a lobby */
				results.callbacksFinished = FALSE;
				results.dpOpenTime = eclock();
				err = dpOpen(myDP, &sess, open_cb, &results);
				if (err != dp_RES_OK) {
					sprintf(buf, "Unable to create a session, error: %d", err);
					abortTest(logFile, 2, buf, NULL);
				}
				results.in_session = TRUE;
				processMessage(myDP, &results);
				if (group) {
					err = dpCreateGroup(myDP, &results.groupID, group);
					if (err != dp_RES_OK) {
						sprintf(buf, "Unable to create a group, error: %d", err);
						abortTest(logFile, 2, buf, NULL);
					}
					processMessage(myDP, &results);
				}
			} else {
				/* wait for session list to show up */
				printf("%d.%d: Finding sessions.\n", thisPlayer, iLoop);
				sess.sessionType = TEST_SPECIES;
				err = dp_RES_OK+1;
				for(i = 0; (i < 30) && (err != dp_RES_OK); i++) {
					results.n_sessions = 0;
					while (results.n_sessions == 0) {
						/* sess */
						results.callbacksFinished = FALSE;
						err = dpEnumSessions(myDP, &sess, NULL, 2000L, listSessions_cb, &results);
						if (err != dp_RES_OK) {
							sprintf(buf, "error %d calling dpEnumSessions", err);
							abortTest(logFile, 2, buf, &results);
						}
						/* wait for reply */
						while (!results.callbacksFinished) {
							processMessage(myDP, &results);

							/* Timeout check */
							if((eclock() - startTime) > test_TIMEOUT_DELAY)
								abortTest(logFile, 2, "Timed out waiting for dpEnumSessions/c.\n", &results);
						}
						processMessage(myDP, &results);

						/* Timeout check */
						if((eclock() - startTime) > test_TIMEOUT_DELAY)
							abortTest(logFile, 2, "Timed out waiting for dpEnumSessions.\n", &results);
					}

					/* join the session */
					results.callbacksFinished = FALSE;
					sprintf(buf, "%s%d", results.room, iLoop);
					printf("%d.%d: Joining session %s.\n", thisPlayer, iLoop, buf);
					err = joinSessionByName(myDP, buf, &results);
				}
				if (err != dp_RES_OK) {
					sprintf(buf, "Unable to join a session, error: %d", err);
					abortTest(logFile, 2, buf, &results);
				}
				results.in_session = TRUE;
			}

			/* Check that we've got a time */
			if(results.dpOpenTime == -1) {
				abortTest(logFile, 1, "Unable to read starting time.", &results);
			}
			while (!results.callbacksFinished) {
				processMessage(myDP, &results);

				/* Timeout check */
				if((eclock() - results.dpOpenTime) > test_TIMEOUT_DELAY)
					abortTest(logFile, 2, "Timed out waiting for session.\n", &results);
			}
			processMessage(myDP, &results);

			/* Create a new player */
			results.callbacksFinished = FALSE;
			sprintf(name, "PSPuser%d", thisPlayer);
			printf("%d.%d: Creating player %s.\n", thisPlayer, iLoop, name);
			results.dpCreatePlayerTime = eclock();
			err = dpCreatePlayer(myDP, create_player_cb, &results, name);
			if (err != dp_RES_OK) {
				sprintf(buf, "Creating player \"%s\" returns err %d", name, err);
				abortTest(logFile, 2, buf, &results);
			}
			/* wait for callback */
			while (!results.callbacksFinished) {
				processMessage(myDP, &results);

				/* Timeout check */
				if((eclock() - results.dpOpenTime) > test_TIMEOUT_DELAY)
					abortTest(logFile, 2, "Timed out waiting for player.\n", &results);
			}
			processMessage(myDP, &results);
			if (group) {
				if (thisPlayer != 0) {
					/* wait for group list to appear */
					results.n_groups = 0;
					while (results.n_groups == 0) {
						/* groups */
						results.callbacksFinished = FALSE;
						err = dpEnumGroups(myDP, NULL, listGroups_cb, &results, 1750L);
						if (err != dp_RES_OK) {
							sprintf(buf, "error %d calling dpEnumGroups", err);
							abortTest(logFile, 2, buf, &results);
						}
						/* wait for reply */
						while (!results.callbacksFinished) {
							processMessage(myDP, &results);

							/* Timeout check */
							if((eclock() - startTime) > test_TIMEOUT_DELAY)
								abortTest(logFile, 2, "Timed out waiting for dpEnumGroups/c.\n", &results);
						}
						processMessage(myDP, &results);

						/* Timeout check */
						if((eclock() - startTime) > test_TIMEOUT_DELAY)
							abortTest(logFile, 2, "Timed out waiting for dpEnumGroups.\n", &results);
					}
					if (results.n_groups != 1)
						printf("%d.%d: weird, got number of groups = %d\n", thisPlayer, iLoop, results.n_groups);
				}

				/* add player to the group */
				results.callbacksFinished = FALSE;
				err = dpAddPlayerToGroup(myDP, results.groupID, results.myID);
				if (err != dp_RES_OK) {
					sprintf(buf, "Unable to add player to group, error: %d", err);
					abortTest(logFile, 2, buf, &results);
				}
			}

			for (pvar = 0; pvar < pVarCount; pvar++) {
				int j;
				char val[5 * 1024];
				char *ptr;
				long base;
				base = ((unsigned long)(pvar << 16) | (unsigned short)SwapBytes(results.myID)) + pvar;
				ptr = &val[0];
				for (j = 0; j < pvar * 25 + 1; j++) { /*increment by 100 bytes*/
					ptr = &val[0] + j * sizeof(base);
					memcpy(ptr, &base, sizeof(base));
				}
				dpSetPlayerData(myDP, results.myID, pvar, val, (ptr - val) + sizeof(base), 0);
			}

			/* Freeze and spawn if it's the right iteration. */
			if(iLoop == results.freezeLoop) {
				int result;
				int proc;
				char buf1[BUFSIZE];
				char buf2[BUFSIZE];
				char buf3[BUFSIZE];
				char buf4[BUFSIZE];
				char buf5[BUFSIZE];
				char buf6[BUFSIZE];
				char buf7[BUFSIZE];
				char buf8[BUFSIZE];
				FILE* freezeFile;

				/* Wait for a safe freeze time */
				while(dpReadyToFreeze(myDP) == dp_RES_BUSY)
					processMessage(myDP, &results);

				/* Find our file */
				sprintf(fname, FREEZENAME2, thisPlayer);
				printf("%d.%d: Freezing info to file %s.\n", thisPlayer, iLoop, fname);
				freezeFile = fopen(fname, "wb");
				assert(freezeFile != NULL);

				/* Write everything to our file */
				fwrite(&iLoop,sizeof(int),1,freezeFile);
				fwrite(&(results.loops),sizeof(int),1,freezeFile);
				fwrite(&(results.n_expectedPlayers),sizeof(int),1,freezeFile);
				fwrite(&(results.n_expectedpVars),sizeof(int),1,freezeFile);
				fwrite(&(results.n_localPlayers),sizeof(int),1,freezeFile);
				fwrite(&(results.beginPlayer),sizeof(int),1,freezeFile);
				fwrite(&(results.myID),sizeof(dpid_t),1,freezeFile);
				fwrite(&(results.groupID),sizeof(dpid_t),1,freezeFile);
				fwrite(&startTime,sizeof(clock_t),1,freezeFile);
				fwrite(&(results.dpOpenTime),sizeof(clock_t),1,freezeFile);
				fwrite(&(results.dpCreatePlayerTime),sizeof(clock_t),1,freezeFile);
				i = strlen(results.room) + 1;
				fwrite(&i,sizeof(int),1,freezeFile);
				fwrite(results.room,sizeof(char),i,freezeFile);
				for(i = 0; i < results.n_expectedPlayers; i++) {
					fwrite(&(results.playerID[i]),sizeof(dpid_t),1,freezeFile);
					fwrite(&(results.acks[i]),sizeof(dpid_t),1,freezeFile);
					if (group)
						fwrite(&(results.gplayerID[i]),sizeof(dpid_t),1,freezeFile);
					if (pVarCount) {
						fwrite(&(results.pvplayerID[i]),sizeof(dpid_t),1,freezeFile);
						fwrite(&(results.npv[i]),sizeof(int),1,freezeFile);
					}
				}
				fclose(freezeFile);

				/* Save the dp in another file */
				sprintf(fname, FREEZENAME, thisPlayer);
				printf("%d.%d: Freezing to file %s.\n", thisPlayer, iLoop, fname);
				err = dpFreeze(myDP, fname);
				assert(err == dp_RES_OK);
				dpDestroy(myDP, 1);

				/* Spawn and return child's return code */
				sprintf(buf1, "-m=%d", thisPlayer);
				sprintf(buf2, "-c=%d", results.n_expectedPlayers);
				sprintf(buf3, "-r=%s", results.room);
				sprintf(buf4, "-f=%d", iLoop + 2*(results.n_expectedPlayers + 2));
				sprintf(buf5, "-n=-1");
				sprintf(buf6, "-v=%d", pVarCount);
				/* kludge: logfile arg must come after loop arg */
				sprintf(buf7, "-l=RawPSP%d.%d.log", packetLoss, thisPlayer);
				if(group) {
					sprintf(buf8, "-g=%s", group);
				} else {
					buf8[0] = '\0';
				}

				fclose(logFile);
				proc = _spawnl(_P_NOWAIT, results.exe, results.exe, buf1, buf2, buf3, buf4, buf5, buf6, buf7, buf8, fname, NULL);
				assert(proc != -1);
				_cwait(&result, proc, 0);
				printf("Spawned proc %d.%d returned %d.\n", thisPlayer, iLoop, result);
				return result;
			}
		} /* not thawed */

		/* Wait for player, group player, player variable list */
		printf("%d.%d: Waiting for %d players.\n", thisPlayer, iLoop, results.n_expectedPlayers);
		results.n_players = 0;
		results.n_gplayers = 0;
		results.n_pvplayers = 0;
		timedOut = FALSE;
		while(!timedOut &&
			 ((results.n_players < results.n_expectedPlayers) ||
			 (group && results.n_gplayers < results.n_expectedPlayers) ||
			 (pVarCount && results.n_pvplayers < results.n_expectedPlayers))) {

			processMessage(myDP, &results);
			if (results.n_players < results.n_expectedPlayers) {
				results.callbacksFinished = FALSE;
				results.n_players = 0;
				err = dpEnumPlayers(myDP, NULL,
						(dpEnumPlayersCallback_t) listPlayers_cb, &results, 1750L);
				if (err != dp_RES_OK) {
					sprintf(buf, "error %d calling dpEnumPlayers", err);
					abortTest(logFile, 2, buf, &results);
				}
				while (!results.callbacksFinished)
					processMessage(myDP, &results);
				processMessage(myDP, &results);

				/* Keep track of how many players found */
				if(lastCount != results.n_players) {
					lastCount = results.n_players;
					printf("%d.%d: Currently have %d players.\n", thisPlayer, iLoop, lastCount);
					if(results.n_maxPlayers < lastCount)
						results.n_maxPlayers = lastCount;
				}

				/* Timeout check */
				if((eclock() - results.dpCreatePlayerTime) > test_TIMEOUT_DELAY)
					timedOut = TRUE;
			}

			if (group && (results.n_gplayers < results.n_expectedPlayers)) {
				results.callbacksFinished = FALSE;
				results.n_gplayers = 0;
				err = dpEnumGroupPlayers(myDP, results.groupID, NULL, listGroupPlayers_cb, &results, 1750L);
				if (err != dp_RES_OK) {
					sprintf(buf, "error %d calling dpEnumGroupPlayers", err);
					abortTest(logFile, 2, buf, &results);
				}
				while (!results.callbacksFinished)
					processMessage(myDP, &results);
				processMessage(myDP, &results);

				/* Keep track of how many players found */
				if(lastGCount != results.n_gplayers) {
					lastGCount = results.n_gplayers;
					printf("%d.%d: Currently have %d players in group.\n", thisPlayer, iLoop, lastGCount);
					if(results.n_maxGPlayers < lastGCount)
						results.n_maxGPlayers = lastGCount;
				}

				/* Timeout check */
				if((eclock() - results.dpCreatePlayerTime) > test_TIMEOUT_DELAY)
					timedOut = TRUE;
			}

			if (pVarCount && results.n_pvplayers < results.n_expectedPlayers) {
				int i, j;
				for (i = 0; i < results.n_expectedPlayers; i++) {
					processMessage(myDP, &results);
					if (results.pvplayerID[i] == dp_ID_NONE) continue;
					if (results.npv[i] == results.n_expectedpVars) {
						if (pvdone[i] == TRUE) continue;
						for (j = 0; j < results.n_expectedPlayers; j++) {
							if (results.playerID[j] == results.pvplayerID[i])
								break;
						}
						/* OK if not found since results array may not contain
						 * record yet.  Check if player variable really arrives
						 * before player record during processMessage(). */
						if (j >= results.n_expectedPlayers) continue;
						for (j = 0; j < results.n_expectedpVars; j++) {
							char pvbuf[10 * 1024];
							size_t spvbuf;
							spvbuf = sizeof(pvbuf);
							err = dpGetPlayerData(myDP, results.pvplayerID[i], j, pvbuf, &spvbuf, 0);
							if (err != dp_RES_OK) {
								sprintf(pvbuf, "error %d calling dpGetPlayerData when should have all data", err);
								abortTest(logFile, 2, pvbuf, &results);
							} else {
								// check if value of pvbuf is correct
								long base;
								char *ipvbuf;
								base = ((unsigned long)(j << 16) | (unsigned short)SwapBytes(results.pvplayerID[i])) + j;
								if(spvbuf != (j * 25 + 1) * sizeof(base)) {
									sprintf(buf, "error: incorrect buf size %d for player index %d; should be %d", spvbuf, j);
									abortTest(logFile, 2, buf, &results);
								}
								for (ipvbuf = &pvbuf[0]; ipvbuf < &pvbuf[0] + spvbuf; ipvbuf += sizeof(base)) {
									if(*((long*)ipvbuf) != base) {
										sprintf(buf, "error: got incorrect pvbuf value at %d byte position; base, val, playerId, index are %ld, %ld, %d, %d", ipvbuf - &pvbuf[0], base, *((long*)ipvbuf), results.pvplayerID[i], j);
										abortTest(logFile, 2, buf, &results);
									}
								}
							}
						}
						results.n_pvplayers++;
						if (results.n_pvplayers == results.n_expectedPlayers)
							results.allPVPlayerTime = eclock();
						pvdone[i] = TRUE;
					}
				}

				/* Keep track of how many players found */
				if(lastPVCount != results.n_pvplayers) {
					lastPVCount = results.n_pvplayers;
					printf("%d.%d: Currently found all player variables for %d of %d players.\n", thisPlayer, iLoop, lastPVCount, results.n_expectedPlayers);
					if(results.n_maxPVPlayers < lastPVCount)
						results.n_maxPVPlayers = lastPVCount;
				}

				/* Timeout check */
				if((eclock() - results.dpCreatePlayerTime) > test_TIMEOUT_DELAY)
					timedOut = TRUE;
			}
		}
		/* if didn't find all player variables, print out how many found */
		if (pVarCount && results.n_pvplayers != results.n_expectedPlayers) {
			int i;
			for (i = 0; i < results.n_expectedPlayers; i++) {
				if (results.pvplayerID[i] != dp_ID_NONE && results.npv[i] != 0)
					printf("%d.%d: timedOut: found %d out of %d player vars for player id %d\n", thisPlayer, iLoop, results.npv[i], results.n_expectedpVars, results.pvplayerID[i]);
			}
		}

		/* Tell others we're done */
		#if 0
			synchronize_done_simple(myDP, &results, iLoop, timedOut);
		#else
			synchronize_done_via_host(myDP, &results, iLoop, timedOut);
		#endif
		/*if(thisPlayer == hostNum) {*/
			endtime = eclock() + 3 * ECLOCKS_PER_SEC;
			while((long)(eclock() - endtime) < 0)
				processMessage(myDP, &results);
		/*}*/

		/* Get rid of my player */
		printf("%d.%d: Destroying player.\n", thisPlayer, iLoop);
		err = dpDestroyPlayer(myDP, results.myID);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpDestroyPlayer (host)", err);
			abortTest(logFile, 2, buf, &results);
		}
		processMessage(myDP, &results);

		/* Wait for everyone to leave */
		timedOut = FALSE;
		printf("%d.%d: %d players; waiting for 0.\n", thisPlayer, iLoop, results.n_players);
		while(!timedOut && (results.n_players > 0)) {
			if (results.n_players > 0) {
				results.callbacksFinished = FALSE;
				results.n_players = 0;
				err = dpEnumPlayers(myDP, NULL,
						(dpEnumPlayersCallback_t) listPlayers_cb, &results, 1750L);
				if (err != dp_RES_OK) {
					sprintf(buf, "error %d calling dpEnumPlayers (host)", err);
					abortTest(logFile, 2, buf, &results);
				}
				while (!results.callbacksFinished) {
					processMessage(myDP, &results);
				}
				processMessage(myDP, &results);

				/* Timeout check */
				if((eclock() - results.dpCreatePlayerTime) > test_TIMEOUT_DELAY)
					timedOut = TRUE;

				if(lastCount != results.n_players) {
					lastCount = results.n_players;
					printf("%d.%d: %d players; waiting for 0.\n", thisPlayer, iLoop, results.n_players);
				}
			}
		}
		/*if(thisPlayer == hostNum) {*/
			endtime = eclock() + 3 * ECLOCKS_PER_SEC;
			while((long)(eclock() - endtime) < 0)
				processMessage(myDP, &results);
		/*}*/
		printf("%d.%d: Session appears empty.\n", thisPlayer, iLoop);

		/* Hang around until all packets have been sent and acked */
		endtime = eclock() + test_TIMEOUT_DELAY;
		while((dpReadyToFreeze(myDP) == dp_RES_BUSY) && ((long)(eclock() - endtime) < 0))
			processMessage(myDP, &results);

		/* Close the session, then hang around a little longer, to re-ack
		 * any dropped packets.
		 */
		printf("%d.%d: Leaving session.\n", thisPlayer, iLoop);
		dpClose(myDP);
		results.in_session = FALSE;
		endtime = eclock() + test_ACK_DELAY;
		while ((long)(eclock() - endtime) < 0)
			processMessage(myDP, &results);

		/* Hang around until all packets have been sent and acked */
		endtime = eclock() + test_TIMEOUT_DELAY;
		while((dpReadyToFreeze(myDP) == dp_RES_BUSY) && ((long)(eclock() - endtime) < 0))
			processMessage(myDP, &results);

		/* Clear scratch variables and write results*/
		for (i = 0; i < results.n_expectedPlayers; i++) {
			results.acks[i] = FALSE;
			results.playerID[i] = dp_ID_NONE;
			if (group)
				results.gplayerID[i] = dp_ID_NONE;
			if (pVarCount) {
				results.npv[i] = 0;
				results.pvplayerID[i] = dp_ID_NONE;
				pvdone[i] = FALSE;
			}
		}
		lastPVCount = 0;
		lastGCount = 0;
		writeReport(logFile, &results);
		if (iLoop < results.loops - 1)
			fprintf(logFile, "looping\n");
	}

	/* Tell others we're done */
	#if 0
		synchronize_done_simple(myDP, &results, -1, timedOut);
	//#else
		synchronize_done_via_host(myDP, &results, -1, timedOut);
	#endif

	/* Write results */
	fclose(logFile);

	/* Hang around until all packets have been sent and acked */
	endtime = eclock() + test_TIMEOUT_DELAY;
	while((dpReadyToFreeze(myDP) == dp_RES_BUSY) && ((long)(eclock() - endtime) < 0))
		processMessage(myDP, &results);
	printf("%d: Ready to freeze.\n", thisPlayer);

	/* Hang around a little longer if desired. */
	endtime = eclock() + timeout;
	while ((long)(eclock() - endtime) < 0)
		processMessage(myDP, &results);

	/* Clean up nicely */
	dpDestroy(myDP, 0);

	/* Return status */
	printf("%d: Test over.\n", thisPlayer);
	if(results.allPlayerTime != -1)
		return psp_RES_OK;
	return psp_RES_TOOFEWPLAYERS;
}

/*-------------------------------------------------------------------------
 Entry point
-------------------------------------------------------------------------*/
int						/* Success (0) or failure (nonzero) */
main(
	int argc,			/* number of arguments */
	char *argv[] )		/* value of arguments */
{
	pspArgs_t args;

	/* Initialize scratch values */
	results.myID = dp_ID_NONE;
	results.playerID = NULL;
	results.groupID = dp_ID_NONE;
	results.gplayerID = NULL;
	results.pvplayerID = NULL;
	results.callbacksFinished = FALSE;
	results.allAcks = FALSE;
	results.acks = NULL;
	results.npv = NULL;
	results.n_sessions = 0;
	results.n_players = 0;
	results.n_groups = 0;
	results.n_gplayers = 0;
	results.n_pvplayers = 0;
	results.loops = 1;
	#if defined(WIN32)
		results.emptyCount = 0;
	#endif

	/* Initialize results to no-values-found */
	results.dpOpenTime = -1;
	results.dpOpenCallbackTime = -1;
	results.dpCreatePlayerTime = -1;
	results.dpCreatePlayerCallbackTime = -1;
	results.allPlayerTime = -1;
	results.allGPlayerTime = -1;
	results.allPVPlayerTime = -1;
	results.n_maxPlayers = 0;
	results.n_maxGPlayers = 0;
	results.n_maxPVPlayers = 0;
	results.beginPlayer = -1;
	results.exe = argv[0];

	/* Handle arguments */
	handleArguments(&args, &logFile, argc, argv);

	/* If we're the master, launch the others. */
	if(results.thisPlayer < 0) {
		launch(
			results.n_expectedPlayers,
			results.beginPlayer,
			args.endPlayer,
			args.packetLoss,
			results.n_expectedpVars,
			args.portnum,
			args.extra_timeout,
			argv[0],
			results.driver,
			args.server,
			results.room,
			args.group,
			&(results.procs));
		return collate(
			results.n_expectedPlayers,
			results.beginPlayer,
			args.endPlayer,
			args.packetLoss,
			results.n_expectedpVars,
			args.group,
			results.procs);

	} else {
		return testOne(
			results.thisPlayer,
			args.packetLoss,
			results.n_expectedpVars,
			args.portnum,
			args.extra_timeout,
			results.driver,
			args.server,
			args.group);
	}
}
