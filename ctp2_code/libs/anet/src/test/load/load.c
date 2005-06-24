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
Load the server.

Usage: load <argument list>

Help Argument:
	-?					Print long help message.
Required Arguments:
	-c=count			The number of players to load the server with.
	-r=list				Command list; join rooms.
	-R=list				Command list; create rooms.
	-a=list ip_address flag    Join any game (ip address and flags is optional;
										default is game server and lobby)
Optional Arguments:
	-d=driver			Name of the driver to use (default = loopback driver).
	-g=species			Game species to use (default = Dark Reign (666)).
	-l=loop             Number of times to loop
	-i=packet_loss		The percentage of packet loss introduced (default = 0%).
	-p=port				Port number to connect from (default = any).
	-s=server_name		The name or address of the server to use (default = none).
	-t=time				sec to spend in each session (default=60).
	-y=time				msec to wait in inner loop (default=100).
	-k					Enable the program to continue on next host if the
						current host is not responding.
	-e=poor@some.com	Whom to e-mail if serious errors happen.
Hidden Arguments:
	-m=player			Player number (used only in loads spawned by load)

Command List:
The command list is a series of commands separated by commas, vertical bars,
or semicolons.  The first character of each command determines the
action taken:
	!		Exit load application immediately.
	$		Disconnect from the server and reconnect to the same server
	$name	Disconnect from the server and connect to server "name".
	+		Create a room named "LoadPPP" (PPP=player number), and join it.
	+name	Create a room named "namePPP" and join it
	@name	Join a room named "name".
Commands that do not begin with one of the special characters are treated
either as if they were a +name (if the command list was invoked with the
-R switch), or as if they were an @name (if the command list was invoked
with the -r switch).

Test Design
-----------
Invokes the requisite number of copies of itself, giving each copy a
sequentially-chosen player number.
When invoked with the -R switch, creates a room; otherwise,
joins a room.  The room name is chosen sequentially from the list of
rooms on the command line.
The test attempts to run with a percentage of packet loss as requested.

Created from PSP.c.
$Log: load.c $
Revision 1.16  1997/09/18 18:10:06  anitalee
1. added period separator.
2. don't sleep between launches for now.
3. try to close nicely when ! command received.
Revision 1.15  1997/09/17 02:21:47  lbennett
Graceful shutdown.
Revision 1.14  1997/09/16 02:08:07  lbennett
Improved logging, removed entry/exit logging.
Revision 1.13  1997/09/15 23:44:01  lbennett
Added quit command, modified printing of results.
Revision 1.12  1997/09/15 23:43:43  lbennett
*** empty log message ***
Revision 1.11  1997/09/14 01:34:00  lbennett
Restored error messages.
Revision 1.10  1997/09/13 22:49:32  lbennett
More detailed reporting.
Revision 1.9  1997/09/13 21:49:23  lbennett
Use new returns from dpSetGameServer().
Revision 1.8  1997/09/13 02:24:17  lbennett
Improved reporting.
Revision 1.7  1997/09/12 01:28:26  lbennett
Improved message logging.
Revision 1.6  1997/09/11 21:27:34  lbennett
dprint's galore
Revision 1.5  1997/09/11 18:08:09  lbennett
No longer crashes on abort.
Revision 1.4  1997/09/11 04:41:53  lbennett
1. Don't let dpEnumSessions() finishing confuse us into thinking
dpOpen() finished!
2. Actually check to see if dpOpen()'s callback indicates success or
failure!
This fixes the "err 18" bug.
Revision 1.3  1997/09/11 01:57:14  lbennett
Removed debug-only switch from log file setting.
Revision 1.2  1997/09/10 01:34:15  lbennett
Testing.
Revision 1.1  1997/09/06 01:51:22  lbennett
Initial revision
--------------------------------------------------------------------------*/

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if (defined _WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#ifdef UNIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <errno.h>
#else
#include <process.h>
#endif
#include <time.h>
#include <assert.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "eclock.h"
#include "anet.h"
#include "../../dp/ptimer.h"


/* Mac replacements for some "standard" DOS stuff. */
#ifdef __MWERKS__
#include "stricmp.h"
#include <console.h>  /* for commandline support */
#endif

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed"
   (Windows *linked* code has six of these ) */
#if (defined _WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* Define this to keep track of the player list */
/* #define PLAYER_LIST_STORED 1 */


/**
* Debugging support
*/

/* Log file macros; disable any not desired. */
#define DPRINTC(a)	DPRINT(a)		/* calls to library */
#define DPRINTD(a)	/*printf a*/		/* minor details */
#define DPRINTE(a)	DPRINT(a)		/* entry and exit */
#define DPRINTP(a)	DPRINT(a)		/* progress in methods */
#define DPRINTR(a)	printf a; DPRINT(a)		/* results */
#define DPRINTS(a)	/*printf a*/		/* status */
#define DPRINTX(a)	printf a		/* errors */

/**
* Constants
*/

#define DEFAULT_SPECIES	666
#define DEFAULT_DRIVER	"..\\..\\..\\win\\dll\\wloopd.dll"
#define DEFAULT_PORT	20000
#define DEFAULT_YIELD	100
#define DEFAULT_SESSIONTIME 60

#define MAX_SESSIONS	1000
#define MAX_YIELD		2000
#define MAX_ARGUMENTS	20

#define RECEIVE_FLAGS	0
#define BUFSIZE			1024

#define test_TIMEOUT_DELAY	60 * ECLOCKS_PER_SEC
#define test_ACK_DELAY		5 * ECLOCKS_PER_SEC

#define load_BADTIME -999

#define NASTYGRAM_MAX_LEN 	16000

/* Result values */
#define load_RES_OK				0	/* everything seems OK */
#define load_RES_BADARGS		1	/* invalid command-line arguments */
#define load_RES_NOMEM			2	/* Could not allocate memory */
#define load_RES_PROCSTART		3	/* Could not start process */
#define load_RES_SIGNAL			4	/* user/system signalled */
#define load_RES_SESSIONLOST	5	/* lost session during test */
#define load_RES_DPERR			6	/* error calling dp library */
#define load_RES_HOSTLOST		7	/* lost host */
#define load_RES_TIMEOUT		8	/* too much time passed */


long cps2ms(clock_t t)
{
	static long hertz = 0;
	if (!hertz)
		hertz = ECLOCKS_PER_SEC;

	if (hertz > 1000) {
		return ((long)(t)) / (ECLOCKS_PER_SEC / 1000);
	} else {
		return ((long)(t)) * (1000 / ECLOCKS_PER_SEC);
	}
/* macro to convert ECLOCKS_PER_SEC to milliseconds */
/*#if ECLOCKS_PER_SEC > 1000
#define cps2ms(t) ((long)(t)) / (ECLOCKS_PER_SEC / 1000)
#else
#define cps2ms(t) ((long)(t)) * (1000 / ECLOCKS_PER_SEC)
#endif*/
}

/**
* Types
*/

/* Structure to hold the ongoing data of the test */
typedef struct data_s {
	dp_t* myDP;
	#if PLAYER_LIST_STORED
		dpid_t* playerID;
	#endif
	char* driver;
	char** cmd;
	char* server;
	char* currentServer;
	dp_session_t sessions[MAX_SESSIONS];
	dpid_t myID;
	time_t yield_time;  /* millisecond */
	time_t sessionTime; /* second */
	clock_t findClock;  /* millisecond */
	clock_t joinClock;  /* millisecond */
	clock_t playClock;  /* millisecond */
	unsigned long portnum;
	int bCallbacksFinished;
	int bOpenSessionCallbackFinished;
	int bCallbackSucceeded;
	int bCreateRoom;
	int curCmd;
	int n_sessions;
	int n_players;
	int n_expectedPlayers;
	int n_maxPlayers;
	int packetLoss;
	int cmdCount;
	int thisPlayer;
	int abort_if_host_not_responding;
	int host_not_responding;
	#if defined(_WIN32)
		int emptyCount;
	#endif
	unsigned short species;
	int bJoinAny;
	char* ipadr_joinany;
	short flag_joinany;
	char room_joinany[256];
	int bWaitingToFreeze;
	int n_loop;
	
	char nastygram_buf[NASTYGRAM_MAX_LEN];
	char *e_mail;
} data_t;


/**
* Global variables
*/

data_t data;		// Must be global for signal handler
FILE * outfile = NULL;

/**
* Methods
*/

void sendMailFromToSubjectBody(const char *from, const char *to, 
	const char *subject, const char *body);

/*-------------------------------------------------------------------------
 Stop the test
-------------------------------------------------------------------------*/
void
abortRun(
	int exitCode,
	char* message)
{
	DPRINTE(("Aborting(%d): %s\n", exitCode, message));
	DPRINTR(("load.abortRun Aborting(%d): %s\n", exitCode, message));
	exit(exitCode);
}


/*-------------------------------------------------------------------------
 Handle a signal (^C or similar)
-------------------------------------------------------------------------*/
void
signalHandler(
	int sigId)
{
	DPRINTE(("load.signalHandler: Entry\n"));
	sigId = sigId;
	#if 0	/* Destroying dp in signal handler causes crashes */
		if(data.myDP != NULL) {
			DPRINTP(("load.signalHandler %d: player %s leaving.\n", thisPlayer, name));
			DPRINTC(("load.signalHandler: calling dpClose\n"));
			dpClose(data.myDP);
			DPRINTC(("load.signalHandler: calling dpDestroy\n"));
			dpDestroy(data.myDP, 0);
			data.myDP = NULL;
		}
	#endif
	DPRINTE(("load.signalHandler: Calling abortRun to exit\n"));
	abortRun(load_RES_SIGNAL, "Terminated at user request.\n");
}

#ifdef UNIX
void
alarmHandler(
	int sigId)
{
	DPRINTE(("load.alarmHandler: Entry\n"));
	if (data.e_mail && data.e_mail[0]) {
		char temp[100];
		
		sprintf(temp, "Cannot set server to %s.\n", data.currentServer);
		sendMailFromToSubjectBody("anet_load", data.e_mail, "Load result", temp);
	}

	DPRINTE(("load.alarmHandler: Calling abortRun to exit\n"));
	abortRun(load_RES_SIGNAL, "Terminated due to alarm timeout.\n");
}
#endif

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
	data_t* res = (data_t *) context;
	DPRINTE(("load.open_cb: Entry\n"));
	DPRINTP(("load.open_cb: ps %p\n", ps));
	res->bOpenSessionCallbackFinished = TRUE;
	res->bCallbackSucceeded = (ps != NULL);
	if (res->bJoinAny && res->bCallbackSucceeded)
		strcpy(res->room_joinany, ps->sessionName);

	(void) pTimeout;
	(void) flags;
	DPRINTE(("load.open_cb: Exit, returning %d\n", (ps!=NULL)));
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
	data_t* res = (data_t *) context;

	DPRINTE(("load.listSessions_cb: Entry\n"));
	if (!sDesc) {
		DPRINTP(("load.listSessions_cb: session list complete.\n"));
		res->bCallbacksFinished = TRUE;
		DPRINTE(("load.listSessions_cb: Exit, returning FALSE\n"));
		return FALSE;
	}
	if (res->n_sessions >= MAX_SESSIONS) {
		DPRINTX(("load.listSessions_cb: session list overflow.\n"));
		res->bCallbacksFinished = TRUE;
		DPRINTE(("load.listSessions_cb: Exit, returning FALSE\n"));
		return FALSE;
	}
	DPRINTP(("load.listSessions_cb: session %d = %s\n",res->n_sessions, sDesc->sessionName));
	res->sessions[res->n_sessions] = *sDesc;
	res->n_sessions++;
	sDesc->szPassword[dp_PASSWORDLEN-1] = 0;

	(void) pTimeout;
	(void) flags;

	DPRINTE(("load.listSessions_cb: Exit, returning TRUE\n"));
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
	data_t* res = (data_t *) context;

	DPRINTE(("load.joinSessionByName: Entry\n"));
	/* Find the given session. */
	for (i = 0; i < res->n_sessions; i++) {
		DPRINTP(("load.joinSessionByName: %s =? %s\n",res->sessions[i].sessionName, name));
		if (strnicmp(res->sessions[i].sessionName, name, strlen(name)) == 0)
			break;
	}
	if (i == res->n_sessions) {
		DPRINTE(("load.joinSessionByName: Exit, returning dp_RES_EMPTY\n"));
		return dp_RES_EMPTY;
	}

	/* Join it. */
	DPRINTC(("load.joinSessionByName: calling dpOpen\n"));
	err = dpOpen(dp, &(res->sessions[i]), open_cb, context);

	DPRINTE(("load.joinSessionByName: Exit, returning %d\n", err));
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
	data_t* res = (data_t *) context;

	DPRINTE(("load.create_player_cb: Entry\n"));
	if (id != dp_ID_NONE)
		res->myID = id;
	res->bCallbacksFinished = TRUE;

	/* Store player ID in our list, if it's not already there */
	#if PLAYER_LIST_STORED
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
	#endif

	(void) name;
	(void) flags;
	DPRINTE(("load.create_player_cb: Exit\n"));
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
	data_t* res = (data_t *) context;

	DPRINTE(("load.listPlayers_cb: Entry\n"));
	if (id != dp_ID_NONE) {
		res->n_players++;
		#if PLAYER_LIST_STORED
		{
			int i;
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
		}
		#endif
	} else {
		res->bCallbacksFinished = TRUE;
	}

	(void) name;
	(void) flags;
	DPRINTE(("load.listPlayers_cb: Exit\n"));
}


/*-------------------------------------------------------------------------
 Handle a received message, if any.
-------------------------------------------------------------------------*/
void
processMessage(dp_t* myDP, data_t* res)
{
	dpid_t idFrom;
	dpid_t idTo;
	char buf[dpio_MAXLEN_UNRELIABLE];
	size_t size = dpio_MAXLEN_UNRELIABLE;
	dp_result_t err;

	DPRINTE(("load.processMessage: Entry\n"));

	/* Get a packet */
	DPRINTC(("load.processMessage: calling dpReceive\n"));
	err = dpReceive(myDP, &idFrom, &idTo, RECEIVE_FLAGS, buf, &size);

	/* Handle empty packets */
	if ((err == dp_RES_EMPTY) || (size <= 0)) {
		#if defined(_WIN32)
			if((res->emptyCount++ > 3) && (res->yield_time > 0))
				Sleep(res->yield_time);
		#endif
		DPRINTE(("load.processMessage: Exit NP\n"));
		return;
	}
	#if defined(_WIN32)
		res->emptyCount = 0;
	#endif

	/* Abort test for loss of host or session */
	if (err == dp_RES_HOST_NOT_RESPONDING && !res->bWaitingToFreeze) {
		char temp[100];

		sprintf(temp, "Host %s is not responding.\n", res->currentServer);
		if ((strlen(res->nastygram_buf) + strlen(temp) + 1) < NASTYGRAM_MAX_LEN) {
			strcat(res->nastygram_buf, temp);
		}
		res->host_not_responding = TRUE;
		if (res->abort_if_host_not_responding)
			abortRun(load_RES_HOSTLOST, temp);
		DPRINTX(("%3d: %s", res->thisPlayer, temp));
		err = dp_RES_OK; 	/* supress further warning */
	}

#ifdef dp_ANET2
	if(*((dp_packetType_t*)buf) == dp_SESSIONLOST_PACKET_ID)
		abortRun(load_RES_SESSIONLOST, "Session lost.\n");
#endif

	/* Handle other error packets */
	if (err != dp_RES_OK) {
		DPRINTP(("load.processMessage: Error:%d in dpReceive.\n",err));
		DPRINTX(("%3d: Got err=%d from dpReceive\n", res->thisPlayer, err));
	}
	DPRINTE(("load.processMessage: Exit\n"));
}


/*-------------------------------------------------------------------------
Wait timeout sec for a host-not-responding message; return when one
found or timeout is over or host is ready.
-------------------------------------------------------------------------*/
int
waitForHost(
	dp_t* myDP,
	clock_t timeout)
{
	dpid_t idFrom;
	dpid_t idTo;
	char buf[dpio_MAXLEN_UNRELIABLE];
	size_t size = dpio_MAXLEN_UNRELIABLE;
	dp_result_t err;
	clock_t endClock;

	endClock = eclock() + timeout * ECLOCKS_PER_SEC;
	while((long)(endClock - eclock()) > 0) {
		size = dpio_MAXLEN_UNRELIABLE;
		err = dpReceive(myDP, &idFrom, &idTo, RECEIVE_FLAGS, buf, &size);
		if (err == dp_RES_HOST_NOT_RESPONDING) {
			DPRINTD(("load.waitForHostFail returning hostlost.\n"));
			return load_RES_HOSTLOST;
		}
		if(err != dp_RES_EMPTY) DPRINTD(("load.waitForHostFail got %d from dpReceive.\n",err));
	}

	if((long)(endClock - eclock()) < 0) {
		DPRINTD(("load.waitForHostFail returning timeout.\n"));
		return load_RES_TIMEOUT;
	}
	return load_RES_OK;
}


/*-------------------------------------------------------------------------
Simulate activity of one session for one user.
First, create or join the session, then send some chat messages.  After a
while, leave the session.
-------------------------------------------------------------------------*/
int
simulateUserGame(
	int thisPlayer,
	char* room,
	int startGame)
{
	char name[64];
	time_t now;
	time_t startTime;
	clock_t startClock;
	clock_t endClock;
	int lastCount = 0;
	int err;
	int i;

	DPRINTE(("load.simulateUserGame: Entry\n"));

	/* Initialize times */
	data.findClock = load_BADTIME ;
	data.joinClock = load_BADTIME ;
	data.playClock = load_BADTIME ;

	/* Host/join session */
	data.bCallbackSucceeded = TRUE;
	startClock = eclock();
	if (startGame) {
		dp_session_t sess;

		/* get list of sessions, so that the dpOpen gets
		   called only after we've really connected. */
		data.bCallbacksFinished = FALSE;
		DPRINTC(("load.simulateUserGame: calling dpEnumSessions\n"));
		memset(&sess, 0, sizeof(sess));
		err = dpEnumSessions(data.myDP, &sess, NULL, 2000L, listSessions_cb, &data);
		if (err != dp_RES_OK) {
			DPRINTX(("%3d: error %d calling dpEnumSessions\n", thisPlayer, err));
			DPRINTE(("load.simulateUserGame: Exit, returning load_RES_DPERR\n"));
			return load_RES_DPERR;
		}
		/* wait for reply */
		endClock = eclock() + test_TIMEOUT_DELAY;
		while (!data.bCallbacksFinished) {
			processMessage(data.myDP, &data); if (data.host_not_responding) return load_RES_DPERR;

			/* Timeout check */
			if((long)(endClock - eclock()) < 0) {
				DPRINTX(("%3d: Timed out waiting for dpEnumSessions/c.\n", thisPlayer));
				DPRINTE(("load.simulateUserGame: Exit, returning load_RES_TIMEOUT\n"));
				return load_RES_TIMEOUT;
			}
		}
		processMessage(data.myDP, &data); if (data.host_not_responding) return load_RES_DPERR;
		data.findClock = cps2ms(eclock() - startClock);

		/* host a session */
		DPRINTS(("%3d: Starting session %s.\n", thisPlayer, room));
		memset(&sess, 0, sizeof(sess));
		sess.sessionType = data.species;
		sess.maxPlayers = dp_MAXREALPLAYERS;
		strncpy(sess.sessionName, room, dp_SNAMELEN);
		sess.sessionName[dp_SNAMELEN-1] = '\0';
		sess.flags = dp_SESSION_FLAGS_CREATESESSION;
		sess.dwUser1 = 0;		/* Not a lobby */
		data.bOpenSessionCallbackFinished = FALSE;
		DPRINTC(("load.simulateUserGame: calling dpOpen\n"));
		err = dpOpen(data.myDP, &sess, open_cb, &data);
		if (err != dp_RES_OK) {
			DPRINTX(("%3d: Unable to create a game, error: %d\n", thisPlayer, err));
			DPRINTE(("load.simulateUserGame: Exit, returning load_RES_DPERR\n"));
			return load_RES_DPERR;
		}
		processMessage(data.myDP, &data); if (data.host_not_responding) return load_RES_DPERR;
	} else {
		dp_session_t sess;

		/* join the session */
		memset(&sess, 0, sizeof(sess));
		/* sess.sessionType = data.species; */
	  if (data.bJoinAny) {
			data.findClock = cps2ms(eclock() - startClock);
			if (data.ipadr_joinany) {
				char newHostAdr[dp_MAX_ADR_LEN];
				err = dpResolveHostname(data.myDP, data.ipadr_joinany, newHostAdr);
				if (err != dp_RES_OK) {
					DPRINTX(("Unable to get address, err:%d\n", err));
					DPRINTE(("load.simulateUserGame: Exit, returning load_RES_DPERR\n"));
					return load_RES_DPERR;
				} else
					strcpy(&sess.adrMaster[0], newHostAdr);
				sess.sessionType = data.species;
				sess.flags = data.flag_joinany;
				err = dpOpen(data.myDP, &sess, open_cb, &data);
			} else
				err = dpOpen(data.myDP, NULL, open_cb, &data);
			data.bOpenSessionCallbackFinished = FALSE;
	  } else {
		DPRINTS(("%3d: Joining session %s.\n", thisPlayer, room));
		err = dp_RES_OK + 1;
		for(i = 0; (i < 10) && (err != dp_RES_OK); i++) {
			data.n_sessions = 0;
			while (data.n_sessions == 0) {
				/* get list of sessions */
				data.bCallbacksFinished = FALSE;
				DPRINTC(("load.simulateUserGame: calling dpEnumSessions\n"));
				err = dpEnumSessions(data.myDP, &sess, NULL, 2000L, listSessions_cb, &data);
				if (err != dp_RES_OK) {
					DPRINTX(("%3d: error %d calling dpEnumSessions\n", thisPlayer, err));
					DPRINTE(("load.simulateUserGame: Exit, returning load_RES_DPERR\n"));
					return load_RES_DPERR;
				}
				/* wait for reply */
				endClock = eclock() + test_TIMEOUT_DELAY;
				while (!data.bCallbacksFinished) {
					processMessage(data.myDP, &data); if (data.host_not_responding) return load_RES_DPERR;

					/* Timeout check */
					if((long)(endClock - eclock()) < 0) {
						DPRINTX(("%3d: Timed out waiting for dpEnumSessions/c.\n", thisPlayer));
						DPRINTE(("load.simulateUserGame: Exit, returning load_RES_TIMEOUT\n"));
						return load_RES_TIMEOUT;
					}
				}
				processMessage(data.myDP, &data); if (data.host_not_responding) return load_RES_DPERR;

				/* Timeout check */
				if((long)(endClock - eclock()) < 0) {
					DPRINTX(("%3d: Timed out waiting for dpEnumSessions.\n", thisPlayer));
					DPRINTE(("load.simulateUserGame: Exit, returning load_RES_TIMEOUT.\n"));
					return load_RES_TIMEOUT;
				}
			}
			data.findClock = cps2ms(eclock() - startClock);
			DPRINT(("load: Took %d msec to find session.\n", data.findClock));

			/* join the session */
			data.bOpenSessionCallbackFinished = FALSE;
			err = joinSessionByName(data.myDP, room, &data);
		}
	  }
		if (err != dp_RES_OK) {
			DPRINTX(("%3d: Unable to join a session, error: %d\n", thisPlayer, err));
			DPRINTE(("load.simulateUserGame: Exit, returning load_RES_DPERR.\n"));
			return load_RES_DPERR;
		}
	}
	endClock = eclock() + test_TIMEOUT_DELAY;
	while (!data.bOpenSessionCallbackFinished) {
		processMessage(data.myDP, &data); if (data.host_not_responding) return load_RES_DPERR;

		/* Timeout check */
		if((long)(endClock - eclock()) < 0) {
			DPRINTX(("%3d: Timed out joining session %s.\n",
					thisPlayer, room));
			DPRINTP(("load.simulateUserGame %d: Timed out joining session %s.\n",
					thisPlayer, room));
			dpClose(data.myDP);
			DPRINTE(("load.simulateUserGame: Exit, returning load_RES_TIMEOUT..\n"));
			return load_RES_TIMEOUT;
		}
	}
	if (!data.bCallbackSucceeded) {
		DPRINTX(("%3d: Can't join session %s; callback had null session pointer\n", thisPlayer, room));
		DPRINTP(("load.simulateUserGame %d: Can't join session %s; callback had null session pointer\n", thisPlayer, room));
		DPRINTC(("load.simulateUserGame: calling dpClose\n"));
		dpClose(data.myDP);
		DPRINTE(("load.simulateUserGame: Exit, returning load_RES_DPERR...\n"));
		return load_RES_DPERR;
	}
	processMessage(data.myDP, &data); if (data.host_not_responding) return load_RES_DPERR;
	data.joinClock = cps2ms(eclock() - startClock);
	DPRINT(("load: Took %d msec to join session.\n", data.joinClock-data.findClock));

	/* Create a new player */
	data.bCallbacksFinished = FALSE;
	sprintf(name, "Loader%d", thisPlayer);
	DPRINTS(("%3d: Creating player %s.\n", thisPlayer, name));
	DPRINTC(("load.simulateUserGame: calling dpCreatePlayer\n"));
	err = dpCreatePlayer(data.myDP, create_player_cb, &data, name);
	if (err != dp_RES_OK) {
		DPRINTX(("%3d: Creating player \"%s\" returns err %d\n", thisPlayer, name, err));
		DPRINTP(("load.simulateUserGame %d: Creating player \"%s\" returns err %d\n", thisPlayer, name, err));
		DPRINTC(("load.simulateUserGame: calling dpClose\n"));
		dpClose(data.myDP);
		DPRINTE(("load.simulateUserGame: Exit, returning load_RES_DPERR..\n"));
		return load_RES_DPERR;
	}
	/* wait for callback */
	endClock = eclock() + test_TIMEOUT_DELAY;
	while (!data.bCallbacksFinished) {
		processMessage(data.myDP, &data); if (data.host_not_responding) return load_RES_DPERR;

		/* Timeout check */
		if((long)(endClock - eclock()) < 0) {
			DPRINTX(("%3d: Timed out creating player.\n", thisPlayer));
			DPRINTP(("load.simulateUserGame %d: Timed out creating player.\n", thisPlayer));
			DPRINTC(("load.simulateUserGame: calling dpClose\n"));
			dpClose(data.myDP);
			DPRINTE(("load.simulateUserGame: Exit, returning load_RES_TIMEOUT...\n"));
			return load_RES_TIMEOUT;
		}
	}
	processMessage(data.myDP, &data); if (data.host_not_responding) return load_RES_DPERR;
	data.playClock = cps2ms(eclock() - startClock);
	DPRINT(("load: Took %d msec to create player.\n", data.playClock-data.joinClock));
	DPRINTE(("load.simulateUserGame: got player; t=%d\n", data.playClock));

	/* Scan the player list for a while */
	data.n_players = 0;
	/* used time() instead of eclock() to avoid midnight problem */
	startTime = time(&startTime);
	while (difftime(time(&now), startTime) < data.sessionTime) {
		data.bCallbacksFinished = FALSE;
		data.n_players = 0;

		DPRINTC(("load.simulateUserGame: calling dpEnumPlayers\n"));
		err = dpEnumPlayers(data.myDP, NULL,
				(dpEnumPlayersCallback_t) listPlayers_cb, &data, 1750L);
		if (err != dp_RES_OK) {
			DPRINTX(("error %d calling dpEnumPlayers\n", err));
			DPRINTP(("load.simulateUserGame error %d calling dpEnumPlayers\n", err));
			DPRINTC(("load.simulateUserGame: calling dpClose\n"));
			dpClose(data.myDP);
			DPRINTE(("load.simulateUserGame: Exit, returning load_RES_DPERR\n"));
			return load_RES_DPERR;
		}

		while (!data.bCallbacksFinished) {
			processMessage(data.myDP, &data); if (data.host_not_responding) return load_RES_DPERR;
		}
		processMessage(data.myDP, &data); if (data.host_not_responding) return load_RES_DPERR;

		/* Keep track of how many players found */
		if(lastCount != data.n_players) {
			lastCount = data.n_players;
			/*DPRINTS(("%3d: %d players in %s.\n",thisPlayer,lastCount,room));*/
			if(data.n_maxPlayers < lastCount)
				data.n_maxPlayers = lastCount;
		}
	}

	/* Leave */
	DPRINTS(("%3d: player %s leaving.\n", thisPlayer, name));
	DPRINTP(("load.simulateUserGame %d: player %s leaving.\n", thisPlayer, name));
	DPRINTC(("load.simulateUserGame: calling dpClose\n"));
	err = dpClose(data.myDP);
	if (err != dp_RES_OK) {
		DPRINTX(("%3d: error %d calling dpClose\n", thisPlayer, err));
		DPRINTP(("load.simulateUserGame %d: error %d calling dpClose\n", thisPlayer, err));
		DPRINTE(("load.simulateUserGame: Exit, returning load_RES_DPERR\n"));
		return load_RES_DPERR;
	}
#ifndef dp_ANET2
	{	long when;
		when = time(0);
		data.bWaitingToFreeze = 1;
		while((dp_RES_BUSY == dpReadyToFreeze(data.myDP)) && ((long)(time(0)-when) < 20))
			processMessage(data.myDP, &data);
		data.bWaitingToFreeze = 0;
	}
#endif

	DPRINTE(("load.simulateUserGame: Exit, returning load_RES_OK\n"));
	return load_RES_OK;
}


/*-------------------------------------------------------------------------
Read a comma, vertical bar or semicolon-separated list from a string.  Adjacent
commas will produce a zero-length string as a list element.
-------------------------------------------------------------------------*/
int					/* Number of items in final list */
scanCommaList(
	char*** list,	/* (output) list */
	char *arg)		/* (input) string to scan */
{
	int count = 1;
	int i;
	char* pc;

	assert(list != NULL);
	assert(arg != NULL);

	/* Find number of items in list */
	for(pc = arg; *pc != '\0'; pc++)
		if((',' == *pc) || (';' == *pc) || ('|' == *pc))
			count++;

	/* Allocate storage */
	*list = (char**)malloc(count * sizeof(char*));
	if(NULL == *list)
		abortRun(load_RES_NOMEM, "Unable to allocate list storage.");

	/* Copy pointers into argument; replace commas with '\0' */
	((*list)[0]) = strtok(arg, ",;|");
	for(i = 1; i < count; i++)
		((*list)[i]) = strtok(NULL, ",;|");

	/* Return number of elements in list */
	return count;
}


/*-------------------------------------------------------------------------
Set the server used.
-------------------------------------------------------------------------*/
int					/* status */
setServer(
	char* server)	/* Server name, or NULL to clear server */
{
	dp_result_t err;

#ifdef UNIX
	alarm(30);
#endif
	DPRINTE(("load.setServer: Entry\n"));

	#if defined DPRINTR
		if(server != NULL) {
			DPRINTR(("%3d: Setting server to %s.\n", data.thisPlayer, server));
			if (outfile)
				fprintf(outfile, "%3d: Setting server to %s.\n", data.thisPlayer, server);
		}
	#endif

	/* If we're going to set the server (if server isn't "-") */
	if((NULL == server) || (server[0] != '-')) {
		char *prevServer;
		err = dp_RES_BUSY;
		prevServer = data.currentServer;
		data.currentServer = server;
		while(err == dp_RES_BUSY) {
			DPRINTC(("load.setServer: calling dpSetGameServer\n"));
			data.host_not_responding = FALSE;
#ifdef dp_MULTISESSTABLE
			err = dpSetGameServerEx(data.myDP, server, data.species);
#else
			err = dpSetGameServer(data.myDP, server);
#endif
			if ((err != dp_RES_OK) && (err != dp_RES_BUSY)) {
				DPRINTE(("load.setServer: Returning %d\n",err));
				return err;
			}
			processMessage(data.myDP, &data); if (data.host_not_responding) return dp_RES_HOST_NOT_RESPONDING;
			processMessage(data.myDP, &data); if (data.host_not_responding) return dp_RES_HOST_NOT_RESPONDING;
			processMessage(data.myDP, &data); if (data.host_not_responding) return dp_RES_HOST_NOT_RESPONDING;
			processMessage(data.myDP, &data); if (data.host_not_responding) return dp_RES_HOST_NOT_RESPONDING;
			DPRINTD(("load.setServer: err=%d\n",err));
		}

		#if 0
			/* Handle ghafla following a server change, if any. */
			if(prevServer != NULL)
				waitForHost(data.myDP, 30);
		#endif

		/* Wait a little while for the game server to notice us */
		/* There should be a callback that tells us when we're connected! */
		/* This block is required if and only if some of the processes
		   need to join a room created by other processes. */
		#if 0
		{
			time_t endClock = eclock() + 20 * ECLOCKS_PER_SEC;
			while ((long)(endClock - eclock()) > 0)
				processMessage(data.myDP, &data);
		}
		#endif
	}
	DPRINTE(("load.setServer: Returning dp_RES_OK\n"));
#ifdef UNIX
	alarm(0);
#endif
	return dp_RES_OK;
}


static char* helpMsg[] = {
"Usage: load <argument list>",
"\nHelp Argument:",
"  -?              Print this message.",
"Required Arguments:",
"  -c=count                    The number of players to load the server with.",
"  -r=list                     Command list; join rooms.",
"  -R=list                     Command list; create rooms.",
"  -a=list ip_addresss flags   Command list; join any session (ip_address, flags is optional;use game server and lobby flag by default",
"Optional Arguments:",
"  -d=driver       Name of the driver to use (default = loopback driver).",
"  -g=species      Game species to use (default = Dark Reign (666)).",
"  -i=packet_loss  The percentage of packet loss introduced (default = 0%).",
"  -p=port         Port number to connect from (default = any).",
"  -s=server_name  The name or address of the server to use (default = none).",
"  -t=time         sec to spend in each session (default=60).",
"  -y=time         msec to wait in inner loop (default=100).",
"  -k              Enable to continue on next host if the current one not responding.",
"  -e=poor@some.com	Whom to e-mail if serious errors happen.",
/*"Hidden Arguments:",
"  -m=player			Player number (used only in loads spawned by load)",*/
"\nCommand List:",
"The command list is a series of commands separated by commas or",
"semicolons.  The first character of each command determines the",
"action taken:",
"  !       Exit load application immediately.",
"  $       Disconnect from the server and reconnect to the same server",
"  $name   Disconnect from the server and connect to server \"name\".",
"  +       Create a room named \"LoadPPP\" (PPP=player number), and join it.",
"  +name   Create a room named \"namePPP\" and join it",
"  @name   Join a room named \"name\".",
"  @any    Join any room",
"Commands that do not begin with one of the special characters are treated",
"either as if they were a +name (if the command list was invoked with the",
"-R switch), or as if they were an @name (if the command list was invoked",
"with the -r switch), or as if the were an @any (if the command list was",
"invoked with the -a switch).",
};

/*-------------------------------------------------------------------------
Write out a verbose help message
-------------------------------------------------------------------------*/
void
printHelp()
{
	int i;
	for(i = 0; i < (sizeof(helpMsg)/sizeof(char*)); i++)
		puts(helpMsg[i]);
}


/*-------------------------------------------------------------------------
 Handle command-line arguments
-------------------------------------------------------------------------*/
int						/* status */
handleArguments(
	int argc,			/* (input) number of arguments */
	char *argv[] )		/* (input) value of arguments */
{
	char buf[BUFSIZE];
	char usage[BUFSIZE];
	int i;
	char* rooms = NULL;

	sprintf(usage, "Usage: %s <argument list>\nType '%s -?' for help.", argv[0], argv[0]);

	/* Initialize arguments */
	data.driver = DEFAULT_DRIVER;
	data.server = NULL;
	data.currentServer = NULL;
	data.cmd = NULL;
	data.cmdCount = 0;
	data.thisPlayer = -1;
	data.n_expectedPlayers = 0;
	data.packetLoss = 0;
#ifdef dp_ANET2
	data.portnum = comm_PORT_ANY;
#else
	data.portnum = 0;
#endif
	data.species = DEFAULT_SPECIES;
	data.sessionTime = DEFAULT_SESSIONTIME;
	data.yield_time = DEFAULT_YIELD;
	data.bCreateRoom = FALSE;
	data.bJoinAny = FALSE;
	data.ipadr_joinany = NULL;
	data.flag_joinany = dp_SESSION_FLAGS_ISLOBBY;
	data.room_joinany[0] = '\0';
	data.abort_if_host_not_responding = TRUE;
	data.n_loop = 0;

	/* Handle each command-line argument */
	for(i = 0; i < argc; i++) {
		if((argv[i][0] == '-') || (argv[i][0] == '/')) {
			if ((argv[i][1] == '\0') || ((argv[i][2] != '=') &&
					(argv[i][2] != ':') && (argv[i][2] != '\0'))) {
				sprintf(buf, "Malformed switch: %s\n%s", argv[i], usage);
				abortRun(load_RES_BADARGS, buf);
			} else if((argv[i][2] != '\0') && (argv[i][3] == '\0')) {
				sprintf(buf, "Value expected: %s\n%s", argv[i], usage);
				abortRun(load_RES_BADARGS, buf);
			}
			switch(argv[i][1]) {
				case 'a': case 'A':	/* join any session*/
					data.bJoinAny = TRUE;
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					} else if ((i + 2 < argc) &&
						!(argv[i+1][0] == '-' || argv[i+1][0] == '/') &&
						(argv[i+2][0] == '-' || argv[i+2][0] == '/')) {
						sprintf(buf, "If specify ip addr, need to specify flags as well\n%s", usage);
						abortRun(load_RES_BADARGS, buf);
					}
					rooms = argv[i]+3;
					if ((i + 2 < argc) &&
						!(argv[i+1][0] == '-' || argv[i+1][0] == '/')) {
						data.ipadr_joinany = argv[i+1];
						data.flag_joinany = (short)atoi(argv[i+2]);
						i += 2;
					}
					break;
				case 'c': case 'C':	/* player count */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					} else if(sscanf(argv[i]+3, "%d", &(data.n_expectedPlayers)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortRun(load_RES_BADARGS, buf);
					}
					break;
				case 'd': case 'D':	/* driver filename */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					}
					data.driver = argv[i]+3;
					break;
				case 'g': case 'G':	/* game species */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					} else if(sscanf(argv[i]+3, "%d", &(data.species)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortRun(load_RES_BADARGS, buf);
					}
					break;
				case 'i': case 'I':	/* packet loss */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					} else if(sscanf(argv[i]+3, "%d", &(data.packetLoss)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortRun(load_RES_BADARGS, buf);
					}
					break;
				case 'l': case 'L':	/* number of times to loop */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					} else if(sscanf(argv[i]+3, "%d", &(data.n_loop)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortRun(load_RES_BADARGS, buf);
					}
					break;
				case 'm': case 'M':	/* player (me) */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					} else if(sscanf(argv[i]+3, "%d", &(data.thisPlayer)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortRun(load_RES_BADARGS, buf);
					}
					break;
				case 'p': case 'P':	/* port number */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					} else if (0 == stricmp(argv[i]+3, "any")) {
						data.portnum = 0;
					} else if(sscanf(argv[i]+3, "%ld", &(data.portnum)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortRun(load_RES_BADARGS, buf);
					}
					break;
				case 'R':	/* create room name */
					data.bCreateRoom = TRUE;
					/* fall through to join room name code */
				case 'r':	/* join room name */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					}
					rooms = argv[i]+3;
					break;
				case 's': case 'S':	/* server address */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					}
					if(argv[i][3] != '-')
						data.server = argv[i]+3;
					else
						data.server = NULL;
					break;
				case 't': case 'T':	/* session time (sec) */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					} else if(sscanf(argv[i]+3, "%d", &(data.sessionTime)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortRun(load_RES_BADARGS, buf);
					}
					break;
				case 'y': case 'Y':	/* yield time (msec) */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					} else if(sscanf(argv[i]+3, "%d", &(data.yield_time)) != 1) {
						sprintf(buf, "Malformed switch: %s", argv[i]);
						abortRun(load_RES_BADARGS, buf);
					}
					break;
				case '?':	/* verbose help */
					printHelp();
					exit(load_RES_OK);
					break;
				/* enable program to continue onto the next host if the 
				 * current one is not responding
				 */
				case 'k': case 'K':	
					if(argv[i][2] == '\0') {
						data.abort_if_host_not_responding = FALSE;
					} else {
						abortRun(load_RES_BADARGS, "Do not supply value to -k option\n");
					}
					break;
				/* Whom to e-mail if serious errors happen. */
				case 'e': case 'E':	
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortRun(load_RES_BADARGS, buf);
					}
					data.e_mail = argv[i]+3;
					break;
				default:
					sprintf(buf,"Illegal switch: %s\nType '%s -?' for help.\n", argv[i], argv[0]);
					abortRun(load_RES_BADARGS, buf);
			}
		}
	}

	if(data.thisPlayer < -1)
		abortRun(load_RES_BADARGS, "Player number must be nonnegative.");
	if(data.n_expectedPlayers < 1)
		abortRun(load_RES_BADARGS, "Must have at least one player.");
	if(data.thisPlayer >= data.n_expectedPlayers)
		abortRun(load_RES_BADARGS, "Player number must smaller than player count.");
	if(MAX_YIELD < data.yield_time)
		data.yield_time = MAX_YIELD;

	/* Only scan the cmd list on child processes */
	if(data.thisPlayer >= 0) 
		data.cmdCount = scanCommaList(&(data.cmd), rooms);

	return load_RES_OK;
}


/*-------------------------------------------------------------------------
 Launch all tests
-------------------------------------------------------------------------*/
void launch (
	int playerCount,	/* Total number to launch */
	int argc,			/* Count of arguments */
	char** argv)		/* Arguments */
{
	int* procs;
	char* childArgs[MAX_ARGUMENTS];
	char buf[BUFSIZE];
	int i;

	/* Copy original arguments into NULL-terminated array of strings */
	assert(MAX_ARGUMENTS >= (argc - 2));
	for(i = 0; i < argc; i++)
		childArgs[i] = argv[i];
	childArgs[argc] = buf;
	childArgs[argc+1] = NULL;

	/* allocate storage */
	if((procs = (int *) malloc(playerCount * sizeof(int))) == NULL)
		abortRun(load_RES_NOMEM, "Unable to allocate process handle storage.\n");

	/* Start tests */
	for(i = 0; i < data.n_expectedPlayers; i++) {
		sprintf(buf, "-m=%d", i);
#ifdef UNIX
		procs[i] = fork();
#else
		procs[i] = _spawnv(_P_NOWAIT, argv[0], childArgs);
#endif
		if(procs[i] == -1) {
			sprintf(buf, "Unable to start process %d: %s\n", i, strerror(errno));
			abortRun(load_RES_PROCSTART, buf);
		}
#ifdef UNIX
		if (procs[i] == 0)   /* child process */
			execv(argv[0], childArgs);
#endif
		#if 0 && (defined _WIN32)
			Sleep(1000);
		#endif
	}

	/* Wait for tests to finish */
	for(i = 0; i < data.n_expectedPlayers; i++) {
#ifdef UNIX
		waitpid(procs[i], NULL, 0);
#else
		_cwait(NULL, procs[i], 0);
#endif
	}
}


/*-------------------------------------------------------------------------
Give this one's figure-of-merit
-------------------------------------------------------------------------*/
void printResults(
	char* room)
{
#if defined(DPRINTR)
	char findRes[30];
	char joinRes[30];
	char playRes[30];
	char totalRes[30];
	char name[30];

	sprintf(name, "Loader%d", data.thisPlayer);

	if(data.findClock >= 0)
		sprintf(findRes, "%3d.%01d", data.findClock/1000, (data.findClock%1000)/100);
	else
		sprintf(findRes, "  ***");

	if(data.joinClock >= 0)
		sprintf(joinRes, "%3d.%01d", (data.joinClock-data.findClock)/1000, ((data.joinClock-data.findClock)%1000)/100);
	else
		sprintf(joinRes, "  ***");

	if(data.playClock >= 0) {
		sprintf(playRes, "%3d.%01d", (data.playClock-data.joinClock)/1000, ((data.playClock-data.joinClock)%1000)/100);
		sprintf(totalRes, "%3d.%01d", data.playClock/1000, (data.playClock%1000)/100);
	} else {
		sprintf(playRes, "  ***");
		sprintf(totalRes, "  ***");
	}

	if (!data.bJoinAny) {
		DPRINTR(("%3d:%9.9s in %-8.8s %5.5s find + %5.5s join + %5.5s play = %5.5s.\n",
			data.thisPlayer, name, room, findRes, joinRes, playRes, totalRes));
		if (outfile)
			fprintf(outfile,"%d %27s %9.9s in %-8.8s %5.5s find + %5.5s join + %5.5s play = %5.5s.\n",
				time(0), data.currentServer, name, room, findRes, joinRes, playRes, totalRes);
	} else {
		DPRINTR(("%3d:%9.9s in %-8.8s %5.5s find + %5.5s join + %5.5s play = %5.5s.\n",
			data.thisPlayer, name, data.room_joinany, findRes, joinRes, playRes, totalRes));
		if (outfile)
			fprintf(outfile,"%d %27s %9.9s in %-8.8s %5.5s find + %5.5s join + %5.5s play = %5.5s.\n",
				time(0), data.currentServer, name, data.room_joinany, findRes, joinRes, playRes, totalRes);
		data.room_joinany[0] = '\0';
	}
#else
	(void) name;
#endif
}

/*-------------------------------------------------------------------------
 Send an e-mail message from the given sender to the given recipient.
 Sender and Recipient must be valid internet email addresses.
-------------------------------------------------------------------------*/
void sendMailFromToSubjectBody(const char *from, const char *to, 
	const char *subject, const char *body) 
{
#ifdef _WIN32
	/*postie -host:internetsmtp.activision.com -file:\build\log-all.log -from:battlezone.build -s:"%rerror%" -to:dkegel@activision.com*/
	char cmdline[1024];
	FILE *fp;

	sprintf(cmdline, "postie -host:internetsmtp.activision.com -from:%s -to:%s -s:\"%s\"", from, to, subject);
	printf("calling popen(%s...)\n", cmdline);
	fp = _popen(cmdline, "wt");
	if (!fp) {
		DPRINTR(("sendMailFromToSubjectBody: error in popen\n"));
		return;
	}
	fputs(body, fp);
	fclose(fp);
#elif defined(UNIX)
	int proc = fork();
	if(proc == -1) {
		DPRINTR(("sendMailFromToSubjectBody: error in fork\n"));
	} else if (proc == 0) {  /* child process */
		FILE *fp;
		if (!(fp = fopen("/tmp/senderr.tmp", "w"))) {
			DPRINTR(("sendMailFromToSubjectBody: error in fopen\n"));
			exit(1);
		}
		fputs(body, fp);
		fputs(".\n", fp);
		fclose(fp);
		if (!freopen("/tmp/senderr.tmp", "r", stdin)) {
			DPRINTR(("sendMailFromToSubjectBody: error in freopen\n"));
			exit(2);
		}
		execl("/usr/sbin/sendmail", "/usr/sbin/sendmail", to, NULL);
		DPRINTR(("sendMailFromToSubjectBody: error in exec, errno %d\n", errno));
		exit(3);
	} else { /* parent; get child's exit code */
		int status;
		pid_t pid;
		pid = waitpid(proc, &status, 0);
		if (WIFEXITED(status) == 0) {
			int retCode = WEXITSTATUS(status);
			DPRINTR(("sendMailFromToSubjectBody: sendmail failed, err %d\n", retCode));
		}
			
	}
#endif
}

/*-------------------------------------------------------------------------
 Run the load for one player
-------------------------------------------------------------------------*/
int testOne (
	int thisPlayer,			/* Which player are we */
	int packetLoss,
	int startGame,
	unsigned long port,
	char* driver)
{
	dp_transport_t CommDLLName;
	commInitReq_t commInitReq;
	dp_result_t err;
	long when;
	int nloop;

	char buf[BUFSIZE];
	char name[64];

	/* Allocate storage */
	#if PLAYER_LIST_STORED
		if((data.playerID = (dpid_t *) malloc(playerCount * sizeof(dpid_t))) == NULL)
			abortRun(load_RES_NOMEM, "Unable to allocate ID storage.\n");
		for(i = 0; i < playerCount; i++)
			data.playerID[i] = dp_ID_NONE;
	#endif

#ifdef dp_ANET2
	/* Set library ini file and dp log file */
	sprintf(name, "load%d.ini", packetLoss);
	dpini_SetFile(name);
	sprintf(name, "load%d.log", thisPlayer);
	dp_setLogFname(name);
	sprintf(name, "load%d.out", thisPlayer);
	outfile = fopen(name, "at");
#endif

	DPRINTE(("load.testOne: Entry (after log set).\n"));

	/* Set up interrupt/alarm handling */
	signal(SIGINT, signalHandler);
	signal(SIGABRT, signalHandler);
	signal(SIGTERM, signalHandler);
#ifdef UNIX
	signal(SIGALRM, alarmHandler);
#endif

	/* Initialize communications */
	memset(&CommDLLName, 0, sizeof(CommDLLName));
	strncpy(CommDLLName.fname, driver, sizeof(CommDLLName.fname));
	CommDLLName.fname[sizeof(CommDLLName.fname)-1] = '\0';
	memset(&commInitReq, 0, sizeof(commInitReq));
	#ifdef _WIN32
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
#ifdef DP_ANET2
	if(comm_PORT_ANY == port)
		commInitReq.portnum = comm_PORT_ANY;
	else
#endif
		commInitReq.portnum = port + thisPlayer;	/* keep port# unique */
	DPRINTS(("%3d: Opening port %ld.\n", thisPlayer, commInitReq.portnum));
	commInitReq.reqLen = sizeof(commInitReq_t);
	DPRINTC(("load.testOne: calling dpCreate\n"));
	err = dpCreate(&(data.myDP), &CommDLLName, &commInitReq, NULL);
	if (err != dp_RES_OK) {
		sprintf(buf, "Error %d creating!", err);
		abortRun(load_RES_DPERR, buf);
	}

	/* Set the game server */
	setServer(data.server);

	/* Loop forever, doing whatever */
	nloop = 0;
	while(data.myDP != NULL) {
		if(NULL != data.cmd[data.curCmd]) {
			if (data.n_loop && (nloop >= data.n_loop))
				data.cmd[data.curCmd][0] = '!';
			switch(data.cmd[data.curCmd][0]) {
				case '!':	/* exit */
					DPRINTR(("%3d: Exit command encountered; complying.\n", thisPlayer));
					/* Terminate communications */
					setServer(NULL);
					when = time(0);
					data.bWaitingToFreeze = 1;
					while((dp_RES_BUSY == dpReadyToFreeze(data.myDP)) && ((long)(time(0)-when) < 20)) {
						processMessage(data.myDP, &data);
					}
					data.bWaitingToFreeze = 0;
					DPRINTR(("%3d: Done waiting for freeze.\n", thisPlayer));
					dpDestroy(data.myDP, 0);
					data.myDP = NULL;
					break;
				case '$':	/* Disconnect and reconnect to server */
					setServer(NULL);
					if('\0' != data.cmd[data.curCmd][1])
						setServer(data.cmd[data.curCmd]+1);
					else
						setServer(data.server);
					break;
				case '+':	/* Create room (unique name) */
					if (data.host_not_responding)
						break;
					if('\0' != data.cmd[data.curCmd][1])
						sprintf(buf, "%s%d", data.cmd[data.curCmd]+1, thisPlayer);
					else
						sprintf(buf, "Load%d", thisPlayer);
					simulateUserGame(thisPlayer, buf, TRUE);
					printResults(buf);
					break;
				case '@':	/* join room */
					if (data.host_not_responding)
						break;
					simulateUserGame(thisPlayer, data.cmd[data.curCmd]+1, FALSE);
					printResults(data.cmd[data.curCmd]+1);
					break;
				case '&':   /* wait 2 sec. */
					{	clock_t start = eclock();
						while ((long)(eclock() - start) < 2 * ECLOCKS_PER_SEC)
							processMessage(data.myDP, &data);
					}
					break;
				default:	/* Create or join room */
					if (data.host_not_responding)
						break;
					simulateUserGame(thisPlayer, data.cmd[data.curCmd], startGame);
					printResults(data.cmd[data.curCmd]);
					break;
			}
		}
		if(data.cmdCount > 1)
			data.curCmd = (data.curCmd + 1) % data.cmdCount;
		if (data.curCmd == 0)
			nloop++;
	}
	if (data.e_mail && data.e_mail[0] && data.nastygram_buf[0]) 
		sendMailFromToSubjectBody("anet_load", data.e_mail, "Load result", data.nastygram_buf);


	DPRINTE(("load.testOne: Exit\n"));
	return load_RES_OK;
}


/*-------------------------------------------------------------------------
 Entry point
-------------------------------------------------------------------------*/
int						/* Success (0) or failure (nonzero) */
main(
	int argc,			/* number of arguments */
	char *argv[] )		/* value of arguments */
{
	memset(&data, 0, sizeof(data_t));
	/* Initialize scratch values */
	data.myID = dp_ID_NONE;
	#if PLAYER_LIST_STORED
		data.playerID = NULL;
	#endif
	data.bCallbacksFinished = FALSE;
	data.n_sessions = 0;
	data.n_players = 0;
	#if defined(_WIN32)
		data.emptyCount = 0;
	#endif
	data.bWaitingToFreeze = 0;

	/* Initialize data to no-values-found */
	data.n_maxPlayers = 0;
	data.curCmd = 0;

	/* Handle arguments */
	handleArguments(argc, argv);

	/* If we're the master, launch the others. */
	if(data.thisPlayer < 0) {
		launch(
			data.n_expectedPlayers,
			argc,
			argv);
	} else {
		if (data.n_loop) {
			ptimer_disableInterrupt();
		}
		testOne(
			data.thisPlayer,
			data.packetLoss,
			data.bCreateRoom,
			data.portnum,
			data.driver);
		if (data.n_loop) {
			ptimer_enableInterrupt();
		}
	}

	return load_RES_OK;
}
