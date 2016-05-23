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
 Player group test for Active.Net.

 Allows you to interactively create, join, leave, and destroy groups.

 $Log: gtest.c $
 Revision 1.115  1997/10/08 18:20:47  lbennett
 Changed dpEnumApps
 Revision 1.114  1997/10/03 00:29:34  anitalee
 fixed bug so launchapp happens before dpDestroy
 Revision 1.113  1997/09/26 18:25:15  dkegel
 Get comm driver and init req from freeze file during dpio_tahw
 Revision 1.112  1997/09/11 21:18:58  dkegel
 1. Print out maxplayers when enumerating sessions
 2. Let user specify flags when creating session
 Revision 1.111  1997/08/26 02:11:12  dkegel
 Whoops - support both old and new versions of dpio_findTimedOutHost
 Revision 1.110  1997/08/26 00:57:19  dkegel
 Adapted to change in dpio.
 Revision 1.109  1997/08/21 21:35:14  dkegel
 Fix crash in delgroups/enumgroups command
 Revision 1.108  1997/08/21 19:51:16  dkegel
 1. Validate arguments on dpOpen better.
 2. Print out dpid's when local messages come in about players getting
 created or deleted.
 Revision 1.107  1997/08/15 19:17:34  anitalee
 redefined _MAX_PATH for Mac and used dpExitFromApp to exit gtest.
 Revision 1.106  1997/08/13 17:10:34  dkegel
 Host command now lets you set dwUser1 at creation time, too.
 Revision 1.105  1997/08/12 23:40:01  lbennett
 Removed uninitialized variable from print statement.
 Revision 1.104  1997/08/12 18:14:04  anitalee
 Fixed another bug.
 Revision 1.103  1997/08/12 18:12:07  anitalee
 Fixed preprocessor bug.
 Revision 1.102  1997/08/11 21:58:36  dkegel
 Callbacks now dp_PASCAL, as required.
 Revision 1.101  1997/08/11 20:41:14  anitalee
 changed file separator to depend on operating system.
 Revision 1.100  1997/08/06 21:43:41  dkegel
 Added demo of dpGetPlayerCaps; now, "players" command shows latency, too.
 Revision 1.99  1997/08/01 01:45:35  lbennett
 Adding support for non-migratable sessions.
 Revision 1.98  1997/07/31 02:07:59  lbennett
 Added debugging statements.
 Revision 1.97  1997/07/28 02:38:20  dkegel
 1. Use dp_transport_t.
 2. When processing commandline, don't screw up on string -n=:dll:
 Revision 1.96  1997/07/27 01:12:47  dkegel
 1. Allow reading from explicit file with -s:fname option (for Mac)
 2. Exit nicely at EOF
 3. Skip whitespace at start of script lines (for Mac reading MS-DOS text)
 4. Set theError properly in session and player creation callbacks
 so scripts can check whether they succeeded (see tests/rejoin.in)
 Revision 1.95  1997/07/26 19:49:46  dkegel
 Adapted to new const in api.
 Revision 1.94  1997/07/25 18:34:30  anitalee
 Changed launch function to dpLaunchApp and added optional exit code argument to quit command.
 Revision 1.93  1997/07/22 18:26:05  anitalee
 Modified to work with changes in rawsun.
 Revision 1.91  1997/07/16 00:08:09  dkegel
 show value of shellOpts for each enumerated app
 Revision 1.90  1997/07/08 21:58:42  dkegel
 1. Added dpio_freeze and dpio_thaw commands.
 2. Changed meaning of thisPlayer to mean a local player, rather than
 a recently created local player, to allow its use after thawing.
 Likewise changed anyPlayer to mean a non-local player.
 3. Init random number generator properly so sessionId's are really
 random; this is important for freeze/thaw with loopback driver.
 (Should use eclock() rather than getTickCount(), though!)
 Revision 1.89  1997/06/26 20:01:07  anitalee
 Display session type as well during enumapp callback.
 Revision 1.88  1997/06/26 17:37:29  dkegel
 Clear session list when changing servers.
 Revision 1.87  1997/06/25 02:32:50  anitalee
 Added chatting function and cleaned up some minor stuff.
 Revision 1.86  1997/06/23 01:10:42  dkegel
 Don't use both upper and lowercase versions of subroutine names,
 e.g. launchApp() was silently being used instead of the missing
      launchapp()!
 Minor improvements to app stuff.
 Revision 1.85  1997/06/23 00:22:57  dkegel
 1. Use 8.3 filename for launchap.h so it will compile under Watcom.
 2. Use _MAX_PATH from stdlib.h instead of defining it here.
 Revision 1.84  1997/06/21 02:18:16  anitalee
 Added functions to list games on this machine and launch one of them.
 Revision 1.83  1997/06/13 22:54:41  geychaner
 Changed dp incoming callbacks to connection callbacks
 Revision 1.82  1997/06/10 23:51:23  geychaner
 Fix problem with openHdl callback by eliminating call to countMyPlayers
 Revision 1.81  1997/06/05 00:09:22  dkegel
 Print out driver info in hex.
 Revision 1.80  1997/06/04 17:12:15  anitalee
 Added timing to the turn system.
 Revision 1.79  1997/06/04 00:54:59  dkegel
 Fixed help msg.
 Revision 1.78  1997/06/04 00:50:58  anitalee
 added dpenumTransport and some turn functions
 Revision 1.77  1997/06/02 01:31:21  dkegel
 Sense ^C (or, on Mac, cmd-.) and call cleanup function before quitting.
 Revision 1.76  1997/05/31 22:46:51  dkegel
 Runs on Mac.  Fixed bad error message and a few syntax errors.
 Revision 1.75  1997/05/30 22:20:24  anitalee
 added some variable casts to make CodeWarrior compile (still doesn't link)
 Revision 1.74  1997/05/28 17:47:56  anitalee
 ******** in sscanf functions, need to use %h when reading short integers
 Revision 1.73  1997/05/23 21:55:18  dkegel
 strip trailing spaces from input lines so batch-file produced scripts
 work properly.
 Revision 1.72  1997/05/23 19:28:25  dkegel
 1. Fixed bug in remotequit handler.
 2. Print out warning in remotequit command if user has no player yet.
 3. Zero out session record before creating session!
 Revision 1.71  1997/05/23 01:30:36  dkegel
 Fixed doofes crash bug.
 Revision 1.70  1997/05/23 00:55:03  dkegel
 1. Added 'gameservers' and 'setgameserver' commands.
 2. Enabled 'remotequit' command.
 3. Help now prompts after every 23 lines or so.
 Revision 1.69  1997/05/22 19:47:15  dkegel
 1. Constants can now be hex if you start them with 0x
 2. 'host' now takes optional room name argument
 3. 'set' can now set dwUser1; its help now lists everything it sets
 4. new cmd 'show' can get value of any variable
 5. list more fields when enumerating sessions
 6. 'assert' help now lists all variables it can test
 Revision 1.68  1997/05/20 19:29:51  dkegel
 Switched to eclock() for portability; see eclock.h.
 Revision 1.67  1997/05/12 22:11:43  anitalee
 runs on Solaris (changed putch to putchar)
 Revision 1.66  1997/05/09 23:30:17  dkegel
 No longer try to jam a four-byte playerHdl_t into a two-byte dpid_t.
 Caught during a port to the SPARC architecture!
 Revision 1.65  1997/04/29 22:33:14  dkegel
 Use new name for dp_enableDebugPrint.
 Revision 1.64  1997/03/28 18:57:15  geychaner
 Removed that last change last night; wasn't a bug after all.
 Revision 1.63  1997/03/28 02:06:18  geychaner
 DON'T clear the session list when calling EnumSessions!!!!!
 Revision 1.62  1997/03/20 04:27:18  geychaner
 Set the dpio clock more often.
 Revision 1.61  1997/03/15 04:49:44  geychaner
 Added the UNIX clock() fix to gtest.
 Revision 1.60  1997/03/15 03:36:07  geychaner
 Added callbacks to test new incoming connection callback functionality.
 Revision 1.59  1997/03/14 23:47:14  geychaner
 Added command to set the maximum number of player handles, for testing
 Revision 1.58  1997/03/14 02:36:19  geychaner
 Changed dprint for failed open handle callback
 Revision 1.57  1997/03/13 22:38:05  dkegel
 Added theError, theVar, and set.
 Revision 1.56  1997/03/13 19:58:16  geychaner
 Minor amendment to the spamming algorithm so the spam timer doesn't
 fall behind the clock too far.
 Revision 1.55  1997/03/13 03:54:00  dkegel
 No longer need to give dpio_create &hMaster.
 Revision 1.54  1997/03/12 22:17:53  dkegel
 1. Added anyPlayer, anyGroup, thisPlayer, thisGroup variables.
 2. Made n_players variable reflect result of gplayers correctly.
 3. Preparse arg2 into argc/argvi[]/argvs[] in hopes it will make it
    easier to let more commands support variables.
 Revision 1.53  1997/03/12 00:10:11  dkegel
 DLL version works.
 Revision 1.52  1997/03/11 22:15:27  geychaner
 Rolled back to r1.50; removed callback from setGameServer
 Revision 1.50  1997/03/11 06:08:52  geychaner
 1. USE_DPIO is TRUE if not DP_DLL, to enable dpio stuff
 2. Added callback to dpio_openHdl for testing purposes.  Works.
 Revision 1.49  1997/03/09 02:41:19  dkegel
 1. Becoming lobby now sets low bit of dwUser1 in session description
 to make netmech happy.
 2. Don't crash in ping if no players match.
 Revision 1.48  1997/03/07 03:54:24  geychaner
 Added an extra fflush just before handling a command
 Revision 1.47  1997/03/07 03:08:20  geychaner
 Changed keyboard I/O for cross-platform compilability using the raw library
 Revision 1.46  1997/03/07 00:53:07  dkegel
 Freeze only destroys, doesn't quit now.  Lets you use freeze as
 a "hasty terminate session" command when checking rejoining.
 Revision 1.45  1997/03/06 00:25:18  geychaner
 Fixed bug in session join and player create callbacks where failure would
 reference a nonexistent variable when printing.
 Revision 1.44  1997/03/05 06:55:12  geychaner
 Changed spamming so it doesn't skip loops
 Revision 1.43  1997/03/05 05:43:49  gceych
 1. Changed so no DLL (DP_DLL not defined) compiles with dpio.h
 2. Added frame rate calculator to the busy (formerly burn) command
 3. Improved spam calculator (I hope)
 4. Improved the calibration of the burn10msec call (somewhat)
 Revision 1.42  1997/03/05 03:37:29  dkegel
 1. Added missing standard libraries and TRUE
 2. No longer need dprint.h now that it's in anet.h...
 Revision 1.41  1997/03/05 03:32:53  gceych
 1. Store player ID when newplayer command is called to create player
 2. Added burn command to simulate frame draw
 3. Added spam command to simulate game data being transmitted
 Revision 1.40  1997/03/04 19:09:42  dkegel
 1. Unless USE_DPIO is defined, gtest will now compile with anet.h
 instead of dp.h, and thus won't have access to the dpio commands.
 2. Assert now sets exit status.  Exit status now documented.
 Revision 1.39  1997/03/03 04:50:33  dkegel
 The dpio_findTimedOutHosts command now calls dpio_closeHdlImmed if
 it finds any timed out hosts.
 Revision 1.38  1997/03/01 23:13:52  dkegel
 Fixed minor doc bug.
 Revision 1.37  1997/03/01 09:01:29  dkegel
 1. Added doc for -c and findTimedOutHost
 2. do normal polling for a second after assertion failure to
 let retransmits happen.
 Revision 1.36  1997/03/01 02:51:08  dkegel
 Assertion failures are now orderly exits - should also set exit val...
 Revision 1.35  1997/03/01 02:33:21  gceych
 1. Added dpio_findtimedOutHost
 2. changed do dpio_update doesn't print RES_EMPTY errors
 Revision 1.34  1997/03/01 01:51:56  dkegel
 Added dpio_closeHdl.
 Revision 1.33  1997/02/28 22:17:01  dkegel
 1. Added dpio_ commands.
 2. Added rplayers command and n_players variable.
 3. Reorganized into sections similar to dp.htm.
 4. Moved help message into MATCH macro, wrote perl script help.pl
    to extract into array, added help function to print out array.
 Revision 1.32  1997/02/27 21:49:12  dkegel
 1. Better usage message.
 2. No more DP_WIN_COMPAT.
 Revision 1.31  1997/02/27 20:53:51  dkegel
 1. Can use : now instead of = in commandline
 2. send now has repeatcount on end
 3. added assert command for regression testing
 Revision 1.30  1997/02/27 03:01:02  dkegel
 No, really.
 Revision 1.28  1997/02/26 18:12:31  dkegel
 1. No longer need slashes on commands.
 2. If -s option, echo commands as they are read.
 3. s is short for sessions, not sleep.
 4. -s no longer needs =1.
 5. Noncommands no longer treated as chat.
 Revision 1.27  1997/02/26 16:57:55  dkegel
 Added /sleep for benefit of scripts.
 Revision 1.26  1997/02/26 00:55:10  dkegel
 Added /wait command to allow scripting.
 Revision 1.25  1997/02/25 23:23:58  dkegel
 For now, specify commdll in thaw just in case it's a cross-platform thaw
 Revision 1.24  1997/02/25 01:44:04  dkegel
 1. Fixed crash-ish bug - was passing garbage value for buf in pv_get
 2. Added -S flag to support forcing line-oriented i/o; fixes problem
 when running regression tests under win32.
 Revision 1.23  1997/02/23 23:34:32  dkegel
 Added -B argument to simulate frame redraw cycle.
 Discovered the "gotta call dpReceive three times more" principle
 of lag avoidance :-) caused by dpReceive returning dp_RES_EMPTY
 when it should really return dp_RES_AGAIN.
 Revision 1.22  1997/02/23 05:47:52  dkegel
 1. Added /remotequit command.
 2. Added /fpset command to make testing big variables easier.
 3. Adapted to new message type for pv.
 Revision 1.21  1997/02/22 23:10:37  dkegel
 Added /lobby command for testing gameserver function.
 Revision 1.20  1997/02/22 22:02:37  dkegel
 Fixed sessionId to actually be different from run to run.
 Revision 1.19  1997/02/22 21:49:47  dkegel
 /send now sends both reliably and unreliably.
 Revision 1.18  1997/02/21 06:51:10  dkegel
 Compatible with Microsoft C.
 Revision 1.17  1997/02/20 00:28:25  dkegel
 Now print out announcement when a player variable changes.
 Revision 1.16  1997/02/20 00:08:24  dkegel
 Added /ping.
 Revision 1.15  1997/02/17 06:10:29  dkegel
 1. Tests dpini_SetFile; reads gtest.ini instead of mw2.ini or dp.ini.
 2. /send now tests max length unreliable packet.
 Revision 1.14  1997/02/16 04:09:15  dkegel
 1. dpCreate doesn't need comm parameters when thawing.
 2. Quit without closing when freezing.
 Revision 1.13  1997/02/16 01:15:51  dkegel
 1. Modified /freeze to not thaw or destroy.
 (There is a trick to using dpFreeze correctly having to do with
 timing and avoiding "player deleted" messages from being sent!
 This probably needs work!)
 2. Added /thaw command and -c (no create) option to allow /thaw to work.
 3. Added in example code showing how to invoke a DLL and how to
 be invoked in case we need to test whether api works well if linked
 in statically by both main .exe and a .dll (like I76).
 Seems to work fine, as long as you freeze and thaw when passing control
 between the copies of the api...
 Revision 1.12  1997/02/13 00:25:23  dkegel
 Added freeze/thaw test.
 Revision 1.11  1997/02/12 22:18:52  dkegel
 1. Added /send command.
 2. Null-terminate chat messages before printing.
 3. Added announcement when becoming HOST.
 4. Announce unrecognized packets.
 Revision 1.10  1997/02/12 02:59:03  dkegel
 1. Use dp's playerdata now instead of addon module.
 2. Close session before quitting.
 Revision 1.9  1997/02/10 07:00:47  dkegel
 1. Print out all interesting add/leave system messages.
 2. Implement delete group.
 3. Adapt to changed API.
 Revision 1.8  1997/02/06 23:11:51  dkegel
 1. Added more group test commands.
 2. Adjusted to match changes in api for win compatibility.
 Revision 1.7  1997/02/04 04:20:30  dkegel
 Added /species command to allow listing or creating other game types.
 Revision 1.6  1997/02/01 00:48:49  dkegel
 Compiles properly under windows now...
 Revision 1.5  1997/01/31 07:49:40  dkegel
 Now supports Windows.
 Revision 1.4  1997/01/30 01:59:41  dkegel
 Added session capabilities command.
 Revision 1.3  1997/01/28 05:33:29  dkegel
 New function pd_addPlayer causes everyone to notify new player of
 their variables.
 Gtest now calls pd_addPlayer and pd_deletePlayer properly.
 Revision 1.2  1997/01/28 03:37:35  dkegel
 1. Converted to dp_RES_OK
 2. Create pd earlier
 3. Call pd_handlePacket
 Seems to work...
 Revision 1.1  1997/01/25 04:58:35  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "eclock.h"

#ifdef UNIX
#include <unistd.h>
#else
#include "unistd.h"
#endif

#ifdef UNIX
#include <sys/param.h>
#define _MAX_PATH MAXPATHLEN
#elif defined(__MWERKS__)
#if defined(_MAX_PATH)
#undef _MAX_PATH
#endif
#define _MAX_PATH FILENAME_MAX
#endif

#ifndef DP_DLL
#define USE_DPIO TRUE
#include "dp.h"
#define DEBUG_MODULE TRUE   /*  enable DPRINT in this file */
#include "dprint.h"
#else
#include "anet.h"
#endif

#ifdef dp_ANET2
#include "avgstat.h"
dp_avgstat_t avgstat;
#endif
#include "raw.h"

#include "dpcrc32.h"

#include "strdup.h"

#ifdef __MWERKS__
/* Mac replacements for some "standard" DOS stuff. */
#include "stricmp.h"
#include <console.h>  /* for commandline support */
#include <StandardFile.h>
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* definitions and variables for dpturn calls */
#define TURN_UNINIT 0
#define TURN_IDLE   1
#define TURN_SEND 2
#define TURN_RECEIVE 4
static int turn_status = TURN_UNINIT;	/* uninitialized state */
static int my_nPlayer = 0;
#if 0
static int round = 0;
static time_t turnstart, turnend;
#endif

static char		adrString[256];
static short	defaultSessType;
dp_transport_t 	CommDLLName;
commInitReq_t	commInitReq;		/* Parameters needed by some comm drivers. */
char			modeministr[256];
char			phonenum[256];
dpid_t          myGameHost = dp_ID_NONE;
dpid_t          myID = dp_ID_NONE;
dpid_t          mylowestId = dp_ID_NONE;
dp_t            *myDP = NULL;
#ifdef USE_DPIO
dpio_t			*dpio = NULL;
#endif

int exitCode = 0;

#define MAX_APPS 50
#define MAX_SESSIONS 100
dp_session_t	sessions[MAX_SESSIONS];
dp_appParam_t	apps[MAX_APPS];		/* Set during callback listApps_cb */
int n_sessions = 0;
int n_players = 0;
int n_apps = 0;						/* Set during callback listApps_cb */
dpid_t thisPlayer = dp_ID_NONE;	/* Most recent player created */
dpid_t thisGroup = dp_ID_NONE;		/* Most recent group created */
dpid_t anyPlayer = dp_ID_NONE;		/* Any non-local player */
dpid_t anyGroup = dp_ID_NONE;		/* Most recent group created or listed */

dp_uid_t myUID = dp_UID_NONE;  /* my UID when I've logged into a server
								* using tserv password authentication.
								*/

int app_pick = -1; 	/* Set by pickApp(); used during freeze command
					   to save to correct directory and used by
					   launchApp to find application to be started*/

dp_result_t theError = dp_RES_OK;	/* most recent dp error */

int pleaseQuit = 0;

int dontCreate = FALSE;
clock_t busyTime = 0;
int busyLoops = 0;

int loops_done = 0;
clock_t firstLoop = 0;

int spam_pktsz = 0;
dpid_t spam_To = dp_ID_NONE;
clock_t nextSpam = 0, spam_time = 0;

int spams_received = 0, spam_recsz = 0;
clock_t frst_recSpam = 0;

char *scriptfname = NULL;
FILE *scriptfp = NULL;

int use_dpio = FALSE;
clock_t dpio_now;

/* A flag used by the /wait and several other commands. */
int callbacksFinished = TRUE;
/* Another flag used by /wait - turns on the wait behavior */
int waiting = FALSE;
/* Flag used by /sleep */
clock_t sleep_until = 0;

/* #define SPECIES_GTEST 55		 unique identifier */
dp_species_t cur_species = 55;

/* Define client-specific packet types here. */
#define CHAT_PACKET_ID		dppt_MAKE('C','H')
#define SPAM_PACKET_ID		dppt_MAKE('S','P')

static void quit(int exit_code)
{
	if (myDP) {
		dpDestroy(myDP, 0);	/* go ahead and hang up */
	}
	raw_end();
	printf("Exiting with status %d.\n", exit_code);
	dpExitFromApp(exit_code);
}

/* Mark time for 10 milliseconds without making any system calls */
/* First call calibrates. */
static void burn10msec(int csec)
{
	static char burnbuf[1024] = "testing testing one two three";
	static int factor = 0;

	if (!factor) {
		int newfactor;
		clock_t x = ECLOCKS_PER_SEC * 2;

		printf("burn10msec: Calibrating busy loop.\n");
		/* Calibrate the busy loop. */
		factor = 1;
		newfactor = 0;
		x += eclock();
		while ((long)(x - eclock()) > 0) {
			burn10msec(20);
			newfactor++;
		}
		factor = newfactor / 10;
		DPRINT(("burn10msec: %d loops = 10 msec.\n", factor));
		printf("burn10msec: %d loops = 10 msec.\n", factor);

#if 0
		/* Test the calibration */
		x = eclock();
		burn10msec(100);
		x = eclock() - x;
		DPRINT(("burn10msec: Asked for 1 second, got %f.\n", (float)x / ECLOCKS_PER_SEC));
		printf("burn10msec: Asked for 1 second, got %f.\n", (float)x / ECLOCKS_PER_SEC);
#endif
		return;
	}

	csec *= factor;	/* convert arg in units of centisec to units of loops */

	while (csec-- > 0)
		dp_crc32((unsigned char *)burnbuf, sizeof(burnbuf));
}

/*--------------------------------------------------------------------------
 Callback function to tell what id a name corresponded to.
 id is dp_ID_NONE on error.
--------------------------------------------------------------------------*/
void (*id_notifier)(dpid_t id);

typedef struct {
	void *context;
	char *name;
	dpEnumPlayersCallback_t cb;
} enumGroupsContext_t;

/*--------------------------------------------------------------------------
 Internal callback for groupNameToID.
 Calls the specified callback upon each match.
--------------------------------------------------------------------------*/
void dp_PASCAL enumGroupsByNameCallback(dpid_t id, char_t *name, long flags, void *context)
{
	enumGroupsContext_t *ctxt = (enumGroupsContext_t *) context;

	if (id == dp_ID_NONE) return;

	if (strnicmp(ctxt->name, name, strlen(ctxt->name)) == 0)
		ctxt->cb(id, name, flags, ctxt->context);
}

/*--------------------------------------------------------------------------
 Enumerate the groups that match a specific name in the given session.
 Call the specified callback upon each match.
--------------------------------------------------------------------------*/
dp_result_t enumGroupsByName(dp_t *dp, char *name, dp_session_t *sp, dpEnumPlayersCallback_t cb, void *context) {
	dp_result_t err;
	enumGroupsContext_t ctxt;

	DPRINT(("groupNameToID(%s, ...):\n", name));
	ctxt.cb = cb;
	ctxt.name = name;
	ctxt.context = context;
	callbacksFinished = FALSE;
	err = dpEnumGroups(dp, sp, enumGroupsByNameCallback, &ctxt, 1750L);

	if (err != dp_RES_OK) {
		DPRINT(("enumGroupsByName: can't list groups, err %d\n", err));
	}
	return err;
}

/*-------------------------------------------------------------------------
 Display text in a chat window.
-------------------------------------------------------------------------*/
void stuffChatText(const char *from, const char *txt)
{
	printf("%s: %s\n", from, txt);
	DPRINT(("sct: %s: %s\n", from, txt));
	fflush(stdout);
}

#if 0
static void dumpBuf(char *buf, int len)
{
	int i;
	DPRINT(("%d bytes: ", len));
	for (i=0; i<len; i++) {
		DPRINT(("%02x ", 0xff & buf[i]));
	}
	DPRINT(("\n"));
}
#else
#define dumpBuf(buf, len)
#endif

/* #ifdef _DEBUG */
#if 1
/* Convert a key to ASCII for debug printing */
static char *key2buf(const char *key, int keylen, char *buf)
{
	int i;

	if (keylen > dp_KEY_MAXLEN)
		return "key too long";

	buf[0] = 0;
	for (i=0; i<keylen; i++) {
		/* DPRINT(("key[%d] %d\n", i, key[i])); */
		sprintf(buf + strlen(buf), "%d%s", 255 & key[i], (i == (keylen-1)) ? "" : ".");
	}
	return buf;
}

/* Same thing but in hex */
static char *key2bufh(const char *key, int keylen, char *buf)
{
	int i;

	if (keylen > dp_KEY_MAXLEN)
		return "key too long";

	buf[0] = 0;
	for (i=0; i<keylen; i++) {
		/* DPRINT(("key[%d] %d\n", i, key[i])); */
		sprintf(buf + strlen(buf), "%02x%s", 255 & key[i], (i == (keylen-1)) ? "" : ".");
	}
	return buf;
}

/* Convert a key to ASCII for debug printing */
static char key2a_buf[256];
static char key2a_buf2[256];
static char key2a_buf3[256];

/* Don't use this twice in one printf! */
#define key2a(key, keylen) key2buf(key, keylen, key2a_buf)
/* You can use this one for the second key in a printf */
#define key2a2(key, keylen) key2buf(key, keylen, key2a_buf2)
/* You can use this one for the third key in a printf */
#define key2a3(key, keylen) key2buf(key, keylen, key2a_buf3)
/* Same thing but in hex */
#define key2a3h(key, keylen) key2bufh(key, keylen, key2a_buf3)
#endif

/*--------------------------------------------------------------------------
 convert a dp_playerId_t to ASCII
--------------------------------------------------------------------------*/
static void dp_playerId_t_toString(const dp_playerId_t *src, char *buf)
{
	int i;

	/* print out dp_playerId_t field */
	sprintf(buf, "playerId: id=%d, karma=%d, name=%s, blob=", src->id, src->karma, src->name);
	for (i=0; i< src->bloblen; i++)
		sprintf(buf + strlen(buf), " %02x", src->blob[i]);
}

/*--------------------------------------------------------------------------
 convert a dp_session_t to ASCII
--------------------------------------------------------------------------*/
static void dp_session_t_toString(const dp_session_t *src, char *buf)
{
	unsigned int i;

	sprintf(buf, "session: sessType %d, name %s, k%d u%x f%x, cur %d, max %d, reserved2 %s, u ",
			src->sessionType, src->sessionName,
			src->karma, (unsigned) src->dwUser1, src->flags,
			src->currentPlayers,
			src->maxPlayers,
			key2a2((char*) src->reserved2, sizeof(src->reserved2))
			);
	for (i=0; i < sizeof(src->szUserField); i++)
		sprintf(buf + strlen(buf), " %02x", src->szUserField[i]);
	sprintf(buf +strlen(buf), ", adrMaster ");
	for (i=0; i < sizeof(src->adrMaster); i++)
		sprintf(buf + strlen(buf), " %02x", (unsigned) src->adrMaster[i]);
}

/*--------------------------------------------------------------------------
 convert a dp_serverInfo_t to ASCII
--------------------------------------------------------------------------*/
static void dp_serverInfo_t_toString(const dp_serverInfo_t *src, char *buf)
{
	sprintf(buf, "serverInfo: host %s, rtt %d, loss %d%% sessType %d, cur_users %d, max_users %d, sessTypeUsers %d",
			src->hostname, src->rtt_ms_avg, src->loss_percent,
			src->sessType, src->cur_users, src->max_users,
			src->cur_sessTypeUsers);
}

/*--------------------------------------------------------------------------
  converts fields in dp_objectDelta_packet_t into ASCII characters and put
  them into a buffer.
--------------------------------------------------------------------------*/
static void dp_objectDelta2string(dp_objectDelta_packet_t *delta, char *buf)
{
	if (delta->key[0] == dp_KEY_PLAYERS)
		sprintf(buf, "objectDelta: status:%d, latency:%dms, loss:%d%%, flags:%lx, key:%s, subkey:%s (%s)\n", delta->status, delta->latency, delta->pktloss, delta->flags, key2a(delta->key, delta->keylen), key2a2(delta->subkey, delta->subkeylen), key2a3h(delta->subkey, delta->subkeylen));
	else
		sprintf(buf, "objectDelta: status:%d, flags:%lx, key:%s, subkey:%s (%s)\n", delta->status, delta->flags, key2a(delta->key, delta->keylen), key2a2(delta->subkey, delta->subkeylen), key2a3h(delta->subkey, delta->subkeylen));

	switch (delta->key[0]) {
	case dp_KEY_PLAYERS:
		dp_playerId_t_toString(&delta->data.p, buf+strlen(buf));
		break;

	case dp_KEY_SESSIONS:
		dp_session_t_toString(&delta->data.sess, buf+strlen(buf));
		break;

	case dp_KEY_SERVERPINGS:
		dp_serverInfo_t_toString(&delta->data.serv, buf+strlen(buf));
		break;

	default:
		break;
	}
}

/*--------------------------------------------------------------------------
 Spam some packets to the network; simulates being a game
--------------------------------------------------------------------------*/
void spam_packets (dpid_t idFrom, dpid_t idTo, int pktsz)
{
	char pktbuf[dpio_MAXLEN_UNRELIABLE] = "SP", buf[255];
	int i;
	dp_result_t err;

    if (!myDP || !pktsz || idFrom == dp_ID_NONE || idTo == dp_ID_NONE)
		return;
	for (i=2; i<pktsz; i++)
		pktbuf[i] = i;
	err = dpSend(myDP, idFrom, idTo, dp_SEND_UNRELIABLE, &pktbuf, pktsz);
	if (err != dp_RES_OK) {
		sprintf(buf, "Error %d sending spam to id %d", err, idTo);
		stuffChatText("*****", buf);
	}
}
#define MAX_PORTNAME_SIZE 10

/*--------------------------------------------------------------------------
 Callback triggered by getting transport info.
--------------------------------------------------------------------------*/
void dp_PASCAL enumTransports_cb(const dp_transport_t *path, const comm_driverInfo_t *info, void *context)
{
	char buf[300];
	DPRINT(("enumTranports_cb(%s, %s, %hx, %hx)\n", path->fname, info->name, info->capabilities, info->needs));
	if (!path) {
		stuffChatText("****", "End of transport list");
		callbacksFinished = TRUE;
	} else {
		sprintf(buf, " %s: name:%s cap:%hx needs:%hx", path->fname, info->name, info->capabilities, info->needs);
		stuffChatText("****", buf);

		/* If it's a modem or serial driver, print out
		 * a list of available ports
		 */
		if (info->needs & comm_INIT_NEEDS_PORTNUM) {
			commPortName_t portName[MAX_PORTNAME_SIZE];
			int nportName;
			int i;
			dp_result_t err;

			err = dpEnumPorts(path, portName, MAX_PORTNAME_SIZE, &nportName);
			if (err != dp_RES_OK) {
				sprintf(buf, "Error %d calling dpEnumPorts\n", err);
				stuffChatText("*****", buf);
			} else {
				for (i=0; i<nportName; i++) {
					sprintf(buf, "portnum %d portname %s\n", portName[i].portnum, portName[i].name);
					stuffChatText("*****", buf);
				}
			}
		}
	}
}

/*-------------------------------------------------------------------------
 Callback triggered by listing game servers.
-------------------------------------------------------------------------*/
int dp_PASCAL listServersEx_cb(const char *hostname, long roundtrip_ms,
		dp_serverInfo_t *server, long *pTimeout, long flags,void *context)
{
	char buf[300];
	DPRINT(("listServers_cb(%s, %d, %p, %d, %p)\n", hostname, roundtrip_ms, pTimeout, flags, context));
	if (!hostname) {
		stuffChatText("****", "End of server list");
		callbacksFinished = TRUE;
		return FALSE;
	}

	if (server)
		sprintf(buf, "Server %s: %ld ms, %d%% pkt loss, species %d\n %d sesstype users, %d tot users, %d max users, %d cur games, %d sesstype games", hostname, roundtrip_ms, server->loss_percent, server->sessType, server->cur_sessTypeUsers, server->cur_users, server->max_users, server->cur_games, server->cur_sessTypeGames);
	else
		sprintf(buf, "Server %s: %ld ms", hostname, roundtrip_ms);
	stuffChatText("****", buf);
	(void) context;
	return TRUE;
}

int dp_PASCAL listServers_cb(const char *hostname, long roundtrip_ms,long *pTimeout,long flags,void *context)
{
	return listServersEx_cb(hostname, roundtrip_ms, NULL, pTimeout,
							flags, context);
}

/*-------------------------------------------------------------------------
 Callback triggered by listing sessions.
-------------------------------------------------------------------------*/
int dp_PASCAL listSessions_cb(dp_session_t *sDesc, long *pTimeout,long flags,void *context)
{
	char buf[300];
	if (!sDesc) {
		stuffChatText("****", "End of session list");
		callbacksFinished = TRUE;
		return FALSE;
	}
	DPRINT(("listSessions_cb: c%p, f%d; ", context, flags));
	dumpBuf((char *)sDesc, sizeof(dp_session_t));
	if (n_sessions >= MAX_SESSIONS) {
		stuffChatText("****", "Too many sessions");
		callbacksFinished = TRUE;
		return FALSE;
	}
	sessions[n_sessions] = *sDesc;
	n_sessions++;
	sDesc->szPassword[dp_PASSWORDLEN-1] = 0;

	sprintf(buf, "(k %d; %d/%d plrs; spec %d; fl %x; U1 %x; pw '%s') name %s",
		sDesc->karma & 0xffff,
		sDesc->currentPlayers,
		sDesc->maxPlayers,
		sDesc->sessionType, sDesc->flags,
		(unsigned) sDesc->dwUser1, sDesc->szPassword,
		sDesc->sessionName
		);
	stuffChatText("****", buf);
	(void) context;
	return TRUE;
}

/*-------------------------------------------------------------------------
 Callback triggered by listing players.
-------------------------------------------------------------------------*/
void dp_PASCAL listPlayers_cb(dpid_t id, char_t *name, long flags, void *context)
{
	char buf[300];
	if (id != dp_ID_NONE) {
		/* Get the player caps for kicks. */
		dp_caps_t info;
		dp_result_t err = dpGetPlayerCaps(myDP, id, &info, dp_SEND_RELIABLE);
		if (err != dp_RES_OK)
			info.dwLatency = -1;

		sprintf(buf, "Found player id %d, name %s, lag %ld", id, name, info.dwLatency);
		stuffChatText("*****", buf);
		n_players++;
		if (flags & dp_EPC_FLAGS_LOCAL) {
			sprintf(buf, "Local player id %d, name %s; setting thisPlayer", id, name);
			stuffChatText("*****", buf);
			thisPlayer = id;
		} else
			anyPlayer = id;

	} else {
		stuffChatText("*****", "End of player listing.");
		callbacksFinished = TRUE;
	}

	(void) context;
	(void) flags;
}

#ifdef dp_ANET2
/*-------------------------------------------------------------------------
 Callback triggered by listing players.
-------------------------------------------------------------------------*/
void dp_PASCAL listPlayersEx_cb(dpid_t id, char_t *name, long flags, void *context, dp_playerId_t *player)
{
	char buf[300];
	if (id != dp_ID_NONE) {
		int i;
		/* Get the player caps for kicks. */
		dp_caps_t info;
		dp_result_t err = dpGetPlayerCaps(myDP, id, &info, dp_SEND_RELIABLE);
		if (err != dp_RES_OK)
			info.dwLatency = -1;

		sprintf(buf, "Found player id %d, name %s, lag %ld, bloblen %d ", id, name, info.dwLatency, player->bloblen);
		for (i=0; i<player->bloblen; i++)
			sprintf(buf + strlen(buf), " %02x", player->blob[i]);
		stuffChatText("*****", buf);
		n_players++;
		if (flags & dp_EPC_FLAGS_LOCAL) {
			sprintf(buf, "Local player id %d, name %s; setting thisPlayer", id, name);
			stuffChatText("*****", buf);
			thisPlayer = id;
		} else
			anyPlayer = id;

	} else {
		stuffChatText("*****", "End of player listing.");
		callbacksFinished = TRUE;
	}

	(void) context;
	(void) flags;
}
#endif

/*-------------------------------------------------------------------------
 Callback triggered by listing groups.
-------------------------------------------------------------------------*/
void dp_PASCAL listGroups_cb(dpid_t id, char_t *name, long flags, void *context)
{
	char buf[300];
	if (id != dp_ID_NONE) {
		sprintf(buf, "Found group id %d, name %s", id, name);
		stuffChatText("*****", buf);
		anyGroup = id;
	} else {
		stuffChatText("*****", "End of group listing.");
		callbacksFinished = TRUE;
	}

	(void) context;
	(void) flags;
}

/*-------------------------------------------------------------------------
 Callback triggered by deleting groups.
-------------------------------------------------------------------------*/
void dp_PASCAL delGroups_cb(dpid_t id, char_t *name, long flags, void *context)
{
	if (id != dp_ID_NONE) {
		dp_result_t err;
		char buf[300];
		err = dpDestroyGroup(myDP, id);
		sprintf(buf, "Deleting group id %d, name %s gives result code %d", id, name, err);
		stuffChatText("*****", buf);
	} else {
		callbacksFinished = TRUE;
	}

	(void) context;
	(void) flags;
}

/*-------------------------------------------------------------------------
 Callback triggered by listing players.
-------------------------------------------------------------------------*/
static void dp_FAR dp_PASCAL my_listPlayers_cb(dpid_t id, char_t * name, long flags, void *context)
{
	(void) context;

	if (id != dp_ID_NONE) {
		DPRINT(("my_listPlayers_cb: Found player id %d, name %s\n", id, name));
		if ((flags & dp_EPC_FLAGS_LOCAL) > 0) {
			DPRINT(("my_listPlayers_cb: ...and it is me....\n"));
			my_nPlayer++;
			if (mylowestId == dp_ID_NONE) {
				mylowestId = id;
			}
		}
	}
}

/*-------------------------------------------------------------------------
 Call to figure out our own ID.
-------------------------------------------------------------------------*/
dp_result_t countMyPlayers(dp_t *dp)
{
	dp_result_t err;

	my_nPlayer = 0;
	mylowestId = dp_ID_NONE;
	err = dpEnumPlayers(dp, NULL, my_listPlayers_cb, dp, 1750L);
	if (err != dp_RES_OK) {
		DPRINT(("countMyPlayers: error %d calling dpEnumPlayers\n", err));
		return err;
	}
	if (mylowestId == dp_ID_NONE) {
		DPRINT(("countMyPlayers: don't know who I am\n"));
		return dp_RES_BUG;
	}
	DPRINT(("mylowestId = %d\n", mylowestId));
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Callback triggered by dpOpen when creating a session.
-------------------------------------------------------------------------*/
int 	dp_PASCAL create_sess_cb(dp_session_t *ps, long *pTimeout,long flags, void *context)

{
	if (ps) {
		stuffChatText("*****", "Created session.");
		theError = dp_RES_OK;
	} else {
		stuffChatText("*****", "Failed to create session.");
		theError = dp_RES_BAD;
	}
	callbacksFinished = TRUE;

	(void) context;
	return (ps != NULL);
}

#ifdef USE_DPIO
/*-------------------------------------------------------------------------
 Callback triggered by openHdl when opening a player handle with dpio.
-------------------------------------------------------------------------*/
void openHdl_cb(playerHdl_t hdl, int nhdls, dp_result_t err, void *context)
{
	char buf[255];

	if (err == dp_RES_OPENED)
		sprintf(buf, "Opened handle h:%x to %s, total %d", (unsigned) hdl, (char*) context, nhdls);
	else if (err == dp_RES_CLOSED)
		sprintf(buf, "Closed handle h:%x to %s, total %d", (unsigned) hdl, (char*) context, nhdls);
	else if (err == dp_RES_OK)
		sprintf(buf, "Called openHdl on open handle h:%x to %s", (unsigned) hdl, (char*) context);
	else if (err == dp_RES_HOST_NOT_RESPONDING)
		sprintf(buf, "Handle h:%x host %s not responding", (unsigned) hdl, (char*) context);
	else
		sprintf(buf, "Handle h:%x host %s, status:%d\n", (unsigned) hdl, (char*) context, err);
    /* if (myDP) countMyPlayers(myDP); */
	stuffChatText("*****", buf);
	if (strcmp ((char *) context, "<INCOMING>"))
		callbacksFinished = TRUE;

	(void) context;
}
#endif

/*-------------------------------------------------------------------------
 Callback triggered by an incoming connection being established in dp.
 Note: Games shouldn't need this.
 This is here so that the programmer running gtest can see the inner guts
 of dpio at work.
-------------------------------------------------------------------------*/
void dp_PASCAL connection_cb(void *adr, int len, int nhdls, dp_result_t err,
	char *user, char *pass, void *context)
{
	char buf[255], print_adr[256];

	if (!adr || !len) {
		strcpy (print_adr, "<UNKNOWN>");
	} else {
		int i;

		for (i = 0; i < len; i++)
			sprintf(print_adr + 3*i, ":%02x", ((unsigned char *)adr)[i]);
	}
	if (err == dp_RES_OPENED)
		sprintf(buf, "Opened handle to host%s, total %d", print_adr, nhdls);
	else if (err == dp_RES_CLOSED)
		sprintf(buf, "Closed handle to host%s, total %d", print_adr, nhdls);
	else if (err == dp_RES_OK)
		sprintf(buf, "Called open on open handle to host%s", print_adr);
	else if (err == dp_RES_HOST_NOT_RESPONDING)
		sprintf(buf, "Host%s not responding", print_adr);
	else
		sprintf(buf, "Host%s, status:%d\n", print_adr, err);
	stuffChatText("*****", buf);

	(void) context;
}

/*-------------------------------------------------------------------------
 Callback triggered by dpOpen when joining a session.
-------------------------------------------------------------------------*/
int dp_PASCAL join_sess_cb(dp_session_t *ps, long *pTimeout, long flags, void *context) {
	char buf[255];
	if (ps) {
		sprintf(buf,  "Joined session %s.", ps->sessionName);
		stuffChatText("*****", buf);
		theError = dp_RES_OK;
	} else {
		if (context)
			sprintf(buf, "Failed to join session %s.", (char*) context);
		else
			sprintf(buf, "Failed to join any session.");
		stuffChatText("*****", buf);
		theError = dp_RES_BAD;
	}
	callbacksFinished = TRUE;

	(void) context;
	return FALSE;
}

/*-------------------------------------------------------------------------
 Join the first session that matches the given name.
 User must have called listSesions() first.
-------------------------------------------------------------------------*/
dp_result_t joinSessionByName(dp_t *dp, char *name)
{
	int i;
	dp_result_t err;
	static char context[255];

	/* Find the given session. */
	for (i=0; i<n_sessions; i++) {
		DPRINT(("jsbn: %s =? %s\n",sessions[i].sessionName, name));
		if (strnicmp(sessions[i].sessionName, name, strlen(name)) == 0)
			break;
	}
	if (i == n_sessions)
		return dp_RES_EMPTY;

	/* Join it. */
	strcpy(context, name);
	err = dpOpen(dp, &sessions[i], join_sess_cb, context);
	if (err != dp_RES_OK) {
		char buf[255];
		sprintf(buf, "Unable to open the session, error: %d", err);
		stuffChatText("*****", buf);
	}

	return err;
}

/*-------------------------------------------------------------------------
 Callback triggered by creating a player.
-------------------------------------------------------------------------*/
void dp_PASCAL create_player_cb(dpid_t id, char_t *name, long flags, void *context)
{
	char buf[300];
	if (id != dp_ID_NONE) {
		sprintf(buf, "Created player id %d, name %s", id, name);
		stuffChatText("*****", buf);
		thisPlayer = id;
		theError = dp_RES_OK;
	} else {
		sprintf(buf, "Failed to create player %s", (char*) context);
		stuffChatText("*****", buf);
		theError = dp_RES_BAD;
	}
	callbacksFinished = TRUE;

	(void) context;
	(void) flags;
}

/*-------------------------------------------------------------------------
 Stuff to support PingPlayerByName().
-------------------------------------------------------------------------*/
void dp_PASCAL ping_callback(dp_karma_t karma, long avg_ms, int loss_pct)
{
	char buf[256];

	DPRINT(("ping_callback: karma %d, avg_ms %d, loss_pct %d\n",
		karma, avg_ms, loss_pct));
	sprintf(buf, "ping: average delay: %ld ms, loss: %d%%", avg_ms, loss_pct);
	stuffChatText("*****", buf);
	callbacksFinished = TRUE;
}

typedef struct {
	dp_result_t err;
	int count;
	char *name;
} pingPlayerContext_t;

void dp_PASCAL pingPlayerCallback(dpid_t id, char_t *name, long flags, void *context)
{
	pingPlayerContext_t *pc = (pingPlayerContext_t *) context;
	char buf[512];
	dp_karma_t karma = (dp_karma_t)time(0);	/* unique identifier for this ping */

	if ((pc->count == 0)
	&&  (id != dp_ID_NONE)
	&&  name
	&&  (strnicmp(pc->name, name, strlen(name)) == 0)) {
		sprintf(buf, "Pinging %s...", name);
		stuffChatText("*****", buf);
		callbacksFinished = FALSE;
		pc->err = dpPingUser(myDP, id, karma, ping_callback);
		pc->count++;
	}
	(void) flags;
}

/*-------------------------------------------------------------------------
 Given the first few letters of a players callsign, ping the first player
 whose name matches.
 Return 1 on success, 0 on failure.
-------------------------------------------------------------------------*/
void PingPlayerByName(char *name) {
	dp_result_t err;
	pingPlayerContext_t pc;
	char buf[512];

	pc.err = dp_RES_OK;
	pc.count = 0;
	pc.name = name;

	DPRINT(("PingPlayerByName: trying to Ping %s\n", name));
	callbacksFinished = FALSE;
	n_players = 0;
	err = dpEnumPlayers(myDP, NULL, pingPlayerCallback, &pc, 0);

	if (err != dp_RES_OK) {
		DPRINT(("PingPlayerByName: can't list players, err %d\n", err));
	} else if (pc.err != dp_RES_OK) {
		DPRINT(("PingPlayerByName: error pinging player, err %d\n", err));
	}
	if (pc.count == 0) {
		DPRINT(("PingPlayerByName: no player matching %s\n", name));
		sprintf(buf,"No player named %s.", name);
		stuffChatText("****", buf);
	}
}

/*-------------------------------------------------------------------------
 Callback triggered by listing applications.
-------------------------------------------------------------------------*/
void dp_PASCAL listApps_cb(dp_appParam_t *app, void *context)
{
	char buf[300];

	if (n_apps < MAX_APPS) {
		apps[n_apps] = *app;
		apps[n_apps].name = strdup(app->name);
		apps[n_apps].path = strdup(app->path);
		apps[n_apps].args = strdup(app->args);
		apps[n_apps].cwd = strdup(app->cwd);
		apps[n_apps].shellOpts = strdup(app->shellOpts);
		sprintf(buf, "%s path:%s args:%s cwd:%s type:%d shellopts:%s",
			app->name,
			app->path, app->args,
			app->cwd,
			app->sessionType,
			app->shellOpts);
#ifdef dp_ANET2
		sprintf(buf+strlen(buf),
				" ver:%u.%u, latest:%u.%u, lang:%d, plat:%d",
			app->current.major, app->current.minor,
			app->latest.major, app->latest.minor,
			app->language,
			app->platform);
#endif
		stuffChatText("****", buf);
		n_apps++;

	}
}

/*-------------------------------------------------------------------------
 Pick the first application that matches the given name and store its
 index in app_pick.
 User must have called listApps() first.
-------------------------------------------------------------------------*/
dp_result_t pickApp(char *name)
{
	int i;

	/* Find the given application. */
	for (i=0; i<n_apps; i++) {
		DPRINT(("labn: %s =? %s\n",apps[i].name, name));
		if (strnicmp(apps[i].name, name, strlen(name)) == 0)
			break;
	}
	if (i == n_apps)
		return dp_RES_EMPTY;

	app_pick = i;
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Broadcast a chat message to all other players.
-------------------------------------------------------------------------*/
dp_result_t SendChatMessage( char *szChatText )
{
#include "dppack1.h"
	struct {
		dp_packetType_t type;
		char buf[200];
		char skip[6];	/* Caution: six bytes after buf trashed by dpSend! */
	} pkt;
#include "dpunpack.h"
	dp_result_t err;

	pkt.type = CHAT_PACKET_ID;
	strcpy(pkt.buf, szChatText);
	err = dpSend(myDP, mylowestId, dp_ID_BROADCAST, dp_SEND_RELIABLE, &pkt, sizeof(pkt)-sizeof(pkt.skip));
	return err;
}

/*--------------------------------------------------------------------------
 Loop for processing a turn.
--------------------------------------------------------------------------*/
void process_turn(void)
{
#if 0
	char buf[300];
	char turnbuf[dp_MAXLEN_UNRELIABLE];
	dpid_t idFrom;
	size_t size;
	dp_result_t err;

	err = dpTurnPoll(myDP);
	if (err == dp_RES_EMPTY) {
		/* Normal - round not done (hmm, shouldn't that be 'dp_RES_BUSY'?) */
		turn_status = TURN_RECEIVE;
	} else if (err == dp_RES_OK) {
		DPRINT(("process_turn: round %d over\n", round));
		idFrom = dp_ID_NONE;
		size = sizeof(turnbuf);
		while (1) {
			int i;
			int errs = 0;
			err = dpTurnGetMove(myDP, &idFrom, turnbuf, &size);
			if (err == dp_RES_EMPTY)
				break;
			else if (err != dp_RES_OK) {
				sprintf(buf, "error: dpTurnGetMove returns e:%d\n", err);
				stuffChatText("****", buf);
				break;
			}

			sprintf(buf, "Got move from id:%d\n", idFrom);
			stuffChatText("****", buf);
			/* Check move */
			if (turnbuf[0] != (idFrom & 255)) {
				sprintf(buf, "\nBad move (id) from id %d, turnbuf[0]=%s\n", idFrom & 255, turnbuf[0]);
				stuffChatText("****", buf);
				errs++;
			}
			for (i=1; i<(int)size; i++)
				if ((turnbuf[i] & 255) != (round & 255)) {
					sprintf(buf, "\nBad move (turnbuf[%d] = %d) from id %d\n", i, turnbuf[i], idFrom);
					stuffChatText("****", buf);
					errs ++;
				}
			if (errs)
				sprintf(buf, "\nBad move (%d errs) from id %d\n", errs, idFrom);
				stuffChatText("****", buf);

			idFrom = dp_ID_NONE;
		}
		sprintf(buf, "Round %d over\n", round);
		stuffChatText("*****", buf);
		round++;
	    turn_status = TURN_IDLE;
		callbacksFinished = TRUE;
	} else {
		sprintf(buf, "error: dpTurnPoll returns e:%d\n", err);
		stuffChatText("*****", buf);
	}
#endif
}

static size_t my_filesize( FILE *fp )
{
	size_t save_pos, size_of_file;

	save_pos = ftell( fp );
	fseek( fp, 0L, SEEK_END );
	size_of_file = ftell( fp );
	fseek( fp, save_pos, SEEK_SET );
	return( size_of_file );
}

#define BADVAL -1

int theVar = 0;

/* Get and/or set value of current session's dwUser1 field */
static int dwUser1(int newval, int set)
{
	dp_session_t s;
	size_t len;
	int oldval;
	dp_result_t err;
	char buf[256];

	len = sizeof(s);
	err = dpGetSessionDesc(myDP, &s, &len);
	if (err != dp_RES_OK) {
		sprintf(buf, "error %d calling dpGetSessionDesc", err);
		stuffChatText("*****", buf);
		return 0;
	}
	if (set) {
		oldval = s.dwUser1;
		s.dwUser1 = newval;
		err = dpSetSessionDesc(myDP, &s, 0);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpSetSessionDesc", err);
			stuffChatText("*****", buf);
			return oldval;
		}
	}
	return s.dwUser1;
}

/* Set a gtest variable from within a script. */
static void setval(const char *s, int val)
{
	if (stricmp(s, "theVar") == 0)
		theVar = val;
	else if (stricmp(s, "dwUser1") == 0)
		dwUser1(val, 1);
	else {
		char buf[256];
		sprintf(buf, "Illegal variable name %s", s);
		stuffChatText("****", buf);
		exit(1);
	}
	DPRINT(("setval(%s, %d)\n", s, val));
}

/* Access a constant or internal gtest variable from within a script.
 * Constants are assumed to be
 *  hex if they have a leading 0x,
 *  octal if they have leading zeroes,
 *  decimal otherwise.
 */
static int getval(const char *s)
{
	int val;

	if (isdigit(*s)) {
		sscanf(s, "%i", &val);		/* %i format allows 0x prefix */
	} else if (stricmp(s, "n_sessions") == 0)
		val = n_sessions;
	else if (stricmp(s, "n_players") == 0)
		val = n_players;
	else if (stricmp(s, "thisPlayer") == 0)	/* Most recent player created */
		val = thisPlayer;
	else if (stricmp(s, "thisGroup") == 0)	/* Most recent group created */
		val = thisGroup;
	else if (stricmp(s, "anyPlayer") == 0)	/* Any non-local player */
		val = anyPlayer;
	else if (stricmp(s, "anyGroup") == 0)	/* Most recent group listed or created */
		val = anyGroup;
	else if (stricmp(s, "theError") == 0)	/* Most recent dp/dpio error code */
		val = theError;
	else if (stricmp(s, "theVar") == 0)
		val = theVar;
	else if (stricmp(s, "dwUser1") == 0)
		val = dwUser1(0, 0);
	else {
		val = BADVAL;		/* lame */
#if 0
		char buf[256];
		sprintf(buf, "Illegal variable name %s", s);
		stuffChatText("****", buf);
		exit(1);
#endif
	}
	DPRINT(("getval(%s) = %d\n", s, val));
	return val;
}

static int doop(int term1, char *op, int term2)
{
	char buf[256];
	int val;

	if (!strcmp(op, ">"))
		val = (term1 > term2);
	else if (!strcmp(op, "<"))
		val = (term1 < term2);
	else if (!strcmp(op, "=="))
		val = (term1 == term2);
	else if (!strcmp(op, "!="))
		val = (term1 != term2);
	else {
		sprintf(buf, "Illegal operator %s", op);
		stuffChatText("****", buf);
		exit(1);
	}
	DPRINT(("doop(%d,%s,%d) = %d\n", term1, op, term2, val));
	return val;
}

static void help()
{
	int i;
	char **p;
	static char *help[] = {
" Basic script language commands ",
"    #                 -- Comment",
"    help              -- Get help",
"    quit              -- terminate program (can give an exit code; default 0)",
"    remotequit        -- tell other players to terminate program",
"    wait              -- Wait until enum or join finishes.",
"    sleep             -- Sleep given number of seconds",
"    busy              -- Be busy given number of 10ms per main loop",
"    set               -- Set var to val; var is theVar or dwUser1",
"    show              -- Print varable value; var is one of anyGroup,anyPlayer,dwUser1,n_players,n_sessions,theError,theVar,thisGroup, or thisPlayer",
"    spam              -- Send data to node at intervals",
"    assert            -- Compare values and abort if unequal; var is one of anyGroup,anyPlayer,dwUser1,n_players,n_sessions,theError,theVar,thisGroup, or thisPlayer",
" dpio commands ",
"    dpio_create       -- Load dpio",
"    dpio_openHdl      -- Open a comm handle to a given node",
"    dpio_closeHdl     -- Close a comm handle",
"    dpio_findTimedOutHost -- Find closed handles",
"    dpio_setMaxPlayerHdls -- Set maximum handles",
"    dpio_setHandleOptions -- Set handle options",
"    dpio_put_reliable -- Send datagram via dpio reliably to given node",
"    dpio_freeze       -- Save dpio to disk [dpio.dat]",
"    dpio_thaw         -- Restore dpio from disk [dpio.dat]",
"    statinit          -- initialize dp statistics",
"    statprint         -- print dp statistics",
" startup/shutdown ",
"    enumtransports    -- list available transports",
"    gameservers       -- list available game servers",
"    setGameServer     -- connects to a game server",
"    login             -- log in to a game server",
"    acctcreate        -- create a new account on a game server",
"    acctchange        -- change an account on a game server",
"    acctactivate      -- activate an account on a game server",
"    requestcode       -- send a request for an activation code email to the game server",
"    scorestart        -- Start a score report",
"    scorereport       -- Add a score to the report",
"    scoreend          -- End and submit a score report",
"    dpCreate          -- load dp",
"    dpDestroy         -- unload dp",
"    freeze            -- freeze the current session and destroy",
"    thaw              -- Restore a dp session from disk",
" session mgmt ",
"    species           -- Set species used for listing and hosting",
"    host              -- create new session; name [maxpl] [flags] [dwuser1].",
"    lobby             -- declare the current session to be a server lobby",
"    join              -- join an existing session",
"    joinany           -- join any existing session",
"    dpClose           -- Disconnect from the current session",
"    sessions          -- list available sessions",
"    setPingIntervals  -- set ping intervals in ms; <piggyback> <forced>",
"    sdeltas           -- request session deltas; [on|off]",
"    gdeltas           -- request gameserver deltas; [on|off]",
"    pdeltas           -- request player deltas; <on|off> [roomname]",
"    ldeltas           -- request player latency deltas; [on|off]",
"    dpGetSessionDesc  -- get current session description",
" player mgmt ",
"    players           -- list players in current session",
"    rplayers          -- list players in named session",
"    xplayers          -- list players in current session",
"    xrplayers         -- list players in named session",
"    enableNewPlayers  -- Enable or disable new players",
"    newplayer         -- create a player in current session",
"    delplayer         -- destroy a player in current session",
"    setpname          -- change the name of the player with the given id",
"    setpblob          -- change the blob of the player with the given id",
" group mgmt ",
"    groups            -- list groups in current session",
"    newgroup          -- create a group in current session",
"    delgroup          -- destroy a group in current session",
"    gaddplayer        -- add a player to a group",
"    gdelplayer        -- delete a player from a group",
"    gplayers          -- list players in given group",
" player variables ",
"    pset              -- Set a variable of a player",
"    fpset             -- Set a variable of a player to a given file",
"    pget              -- Get value of a variable of a player",
" misc ",
"    ping              -- measure round trip lag time to a player",
"    scap              -- Get capabilities of current session.",
"    enumapp           -- list available applications",
"    patch             -- Check whether patch is needed [apply]",
"    pickapp           -- pick an existing application for use with launchapp",
"    launchapp         -- freeze, start the application selected from pickapp, then destroy",
"    crash             -- cause an exception",
" sending/receiving datagrams ",
"    send              -- send a packet to a given player or group",
"    chat              -- broadcast a chat message",
" turn commands ",
"    turninit          -- initialize turn system",
"    turnfinish        -- shut down turn system",
"    turnput           -- send your move; use wait for other players' moves",
NULL
	};	/* see help.pl */

	/* Should probably let them scroll backwards in list, too. */
	i = 0;
	for (p=help; *p; p++) {
		stuffChatText("help", *p);
		if ((i++ == 24) && !scriptfp) {
			int c;
			i = 0;
			stuffChatText("*****", "Press any key to continue");
			while (!raw_kbhit())
				;
			c = raw_getc();
			if (c == 3)
				quit(1);
		}
	}
	if (!scriptfp)
		stuffChatText("*****", "Help finished.");
}

void statprint_fn(const char *buf, void *context)
{
	stuffChatText("stat", buf);
}

#define SUBARGS 4

/*-------------------------------------------------------------------------
 Given a line of text from the user, if it's a slash command, execute it,
 and return 1.  Otherwise, return 0.
-------------------------------------------------------------------------*/
int processChatCommand(const char *szChatText)
{
	char *arg0, *arg2, *c, string[255], buf[255];

	int argc;
	char argvs[SUBARGS][128];
	int  argvi[SUBARGS];

	static char context[255];
	dp_session_t  sess;
	dp_result_t err;
	int i;

	//if (szChatText[0] != '/')
		//return FALSE;

	printf("\n");		// Kludge so user can see what they typed

	strcpy(string, szChatText);
	DPRINT(("processChatCommand: %s\n", szChatText));
	arg0 = string;
	c = strchr(string, ' ');		// look for whitespace
	if (!c) c = strchr(string, '\n');	// look for whitespace
	if (c != NULL) {
		*c = 0;						// null terminate
		while (isspace(*++c)) {};	// seek to end of whitespace
		arg2 = c;
	} else {						// couldn't find whitespace
		arg2 = "";					// point to empty string
	}
	DPRINT(("processChatCommand: arg0 %s, arg2 %s\n", arg0, arg2));

	/* Parse arg2 into words, convert to integers; BADVAL if not an integer */
	argc = sscanf(arg2, "%s %s %s %s", argvs[0], argvs[1], argvs[2], argvs[3]);
	if (argc < 0)
		argc = 0;
	for (i=0; i<argc; i++) {
		argvi[i] = getval(argvs[i]);
		DPRINT(("pcc: subarg %d; %s; %d\n", i, argvs[i], argvi[i]));
	}

#define MATCH(s,h) (strnicmp(arg0, s, strlen(arg0)) == 0)

/*---------------| Basic script language commands |-----------------------*/
	if (MATCH("", "Empty line")) {
		;
	} else if (MATCH("#", "Comment")) {
		;
	} else if (MATCH("help", "Get help")) {
		help();
	} else if (MATCH("quit", "terminate program (can give an exit code; default 0)")) {
		pleaseQuit = TRUE;
		if (argc == 1) {
			exitCode = argvi[0];
		}
		if (myDP)
			dpClose(myDP);
	} else if (MATCH("remotequit", "tell other players to terminate program")) {
		strcpy(buf, "rq");
		if (thisPlayer == dp_ID_NONE) {
			stuffChatText("*****", "You must create a player first.");
			return TRUE;
		}
		err = dpSend(myDP, thisPlayer, 0, dp_SEND_UNRELIABLE, buf, strlen(buf));
		if (err != dp_RES_OK) {
			stuffChatText("*****", "Can't send...");
			return TRUE;
		}
		err = dpSend(myDP, thisPlayer, 0, dp_SEND_RELIABLE, buf, strlen(buf));
		if (err != dp_RES_OK) {
			stuffChatText("*****", "Can't send...");
			return TRUE;
		}
		stuffChatText("*****", "Sent remotequit message to all other players.");
	} else if (MATCH("wait", "Wait until enum or join finishes.")) {
		waiting = TRUE;
		stuffChatText("*****", "Waiting for callback to finish");
	} else if (MATCH("sleep", "Sleep given number of seconds")) {
		float duration;
		if (!arg2[0] || (1 != sscanf(arg2, "%f", &duration))) {
			sprintf(buf, "sleep: must specify a duration in seconds (decimal point ok)");
			stuffChatText("*****", buf);
			return TRUE;
		}
		sleep_until = eclock() + (clock_t)(duration * ECLOCKS_PER_SEC);
		sleep_until = eclock() + (clock_t)(duration * ECLOCKS_PER_SEC);
		stuffChatText("*****", "Sleeping.");
	} else if (MATCH("busy", "Be busy given number of 10ms per main loop")) {
		if (loops_done) {
			float loopsec = (float) (eclock() - firstLoop) / ECLOCKS_PER_SEC;
			float looprate = (float) (loops_done) / loopsec;

			sprintf(buf, "busy: %d loops in %.2f sec; %.2f loops/sec",
				loops_done, loopsec, looprate);
			firstLoop = loops_done = 0;
			stuffChatText("*****", buf);
		}
		if (!arg2[0]) {
			sprintf(buf, "busy: at %d; to change, specify duration in 10ms units (int)", busyTime);
			stuffChatText("*****", buf);
		} else {
			busyTime = atoi(arg2);
			sprintf (buf, "Busy time set to %d", busyTime);
			stuffChatText("*****", buf);
			burn10msec(1);	/* calibrate */
		}
	} else if (MATCH("set", "Set var to val; var is theVar or dwUser1")) {
		if ((argc != 2) || (argvi[1] == BADVAL)) {
			stuffChatText("*****", "set: expected 'set var val'");
			return TRUE;
		}
		setval(argvs[0], argvi[1]);
		sprintf(buf, "set %s to %d", argvs[0], argvi[1]);
		stuffChatText("*****", buf);
	} else if (MATCH("show", "Print varable value; var is one of anyGroup,anyPlayer,dwUser1,n_players,n_sessions,theError,theVar,thisGroup, or thisPlayer")) {
		long x;
		if (argc != 1) {
			stuffChatText("*****", "show: expected 'show var'");
			return TRUE;
		}
		x = getval(argvs[0]);
		sprintf(buf, "the value of %s is %ld (%x)", argvs[0], x, (unsigned) x);
		stuffChatText("*****", buf);
	} else if (MATCH("spam", "Send data to node at intervals")) {
		dpid_t dest;
		int got;
		int pktsz = 100, interval = 200;

		if (spams_received) {
			int spam_avgsz = spam_recsz / spams_received;
			float spamsec = (float) (eclock() - frst_recSpam) / ECLOCKS_PER_SEC;
			float spamrate = (float) (spams_received) / spamsec;

			sprintf(buf, "spam: %d spams in %.2f sec, avg size %d, %.2f spams/sec",
				spams_received, spamsec, spam_avgsz, spamrate);
			frst_recSpam = spam_recsz = spams_received = 0;
			stuffChatText("*****", buf);
		}
		got = 0;
		if (!arg2[0] || (1 > (got = sscanf(arg2, "%hu %d %d", &dest, &pktsz, &interval)))) {
			sprintf(buf, "spam: to spam, specify player id, and opt pkt sz and msec int");
			stuffChatText("*****", buf);
		} else {
			if (pktsz > dpio_MAXLEN_UNRELIABLE) {
				stuffChatText("*****", "Packet size too large; setting to maximum");
				pktsz = dpio_MAXLEN_UNRELIABLE;
			}
			spam_To = dest;
			spam_pktsz = pktsz;
			spam_time = (clock_t) (interval * ECLOCKS_PER_SEC / 1000);
			nextSpam = eclock() + spam_time;
		}
		if (spam_To != dp_ID_NONE) {
			if (thisPlayer == dp_ID_NONE)
				sprintf(buf, "Can't spam %d bytes each %d ticks to id %d; no player created!",
					spam_pktsz, spam_time, spam_To);
			else
				sprintf(buf, "Spamming %d bytes each %d ticks to id %d",
					spam_pktsz, spam_time, spam_To);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("assert", "Compare values and abort if unequal; var is one of anyGroup,anyPlayer,dwUser1,n_players,n_sessions,theError,theVar,thisGroup, or thisPlayer")) {
		int result;
		if (argc != 3) {
			sprintf(buf, "assert: must specify val1 op val2 (%d out of 3 ain't bad)", argc);
			stuffChatText("*****", buf);
			sprintf(buf, "assert: Legal ops are == != < >");
			stuffChatText("*****", buf);
			return TRUE;
		}
		result = doop(argvi[0], argvs[1], argvi[2]);
		sprintf(buf, "assert %s %s %s (%d %s %d) (%d) %s.",
			argvs[0], argvs[1], argvs[2],
			argvi[0], argvs[1], argvi[2],
			result,
			(result ? "OK" : "FAILS"));
		stuffChatText("*****", buf);
		if (!result) {
			/* Terminate, but give the reliable system 1 second to
			 * notify everybody.
			 */
			pleaseQuit = TRUE;
			if (myDP) {
				dpSetGameServer(myDP, NULL);	/* Disconnect */
				dpClose(myDP);
			}
			sleep_until = eclock() + ECLOCKS_PER_SEC;
			exitCode = 3;	/* Assertion failed */
		}
#ifdef USE_DPIO
/*---------------| dpio commands |-----------------------*/
	} else if (MATCH("dpio_create", "Load dpio")) {
		if (myDP) {
			stuffChatText("*****", "dpio_create: DP already created.");
			return TRUE;
		}
		if (dpio) {
			stuffChatText("*****", "dpio_create: dpio already created.");
			return TRUE;
		}
		err = dpio_create(&dpio, &CommDLLName, &commInitReq,
			&dpio_now, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d creating dpio", err);
			stuffChatText("*****", buf);
			pleaseQuit = TRUE;
			return TRUE;
		}
		dpio_setIncomingCallback(dpio, openHdl_cb, "<INCOMING>");
		stuffChatText("*****", "dpio created.");
	} else if (MATCH("dpio_openHdl", "Open a comm handle to a given node")) {
		playerHdl_t h;
		int got;
		unsigned char adrBuf[dp_MAX_ADR_LEN];
		commScanAddrReq_t		scanReq;
		commScanAddrResp_t		scanResp;

		got = 0;
		if (!arg2[0]) {
			sprintf(buf, "dpio_openHdl: must specify an address");
			stuffChatText("*****", buf);
			return TRUE;
		}
		if (!dpio) {
			sprintf(buf, "dpio_openHdl: must dpio_create first");
			stuffChatText("*****", buf);
			return TRUE;
		}

		/*  Convert partner's address to binary for use with commSayHi */
		scanReq.printable = arg2;
		scanReq.address = adrBuf;
		scanReq.size = sizeof(adrBuf);
		if (!commScanAddr(&scanReq, &scanResp, dpio->commPtr)) {
			sprintf(buf, "Unable to scan player address, error: %ld", scanResp.status);
			stuffChatText("*****", buf);
			return TRUE;
		}
		/*  Open a comm handle to the partner's address */
		strcpy (context, arg2);
		callbacksFinished = FALSE;
		h = dpio_openHdl(dpio, adrBuf, openHdl_cb, context);
		if (h == PLAYER_NONE) {
			sprintf(buf, "Unable to connect to address %s", arg2);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Opened handle %lu", h);
		stuffChatText("*****", buf);
	} else if (MATCH("dpio_closeHdl", "Close a comm handle")) {
		playerHdl_t h;
		dp_result_t err;

		if (!arg2[0]) {
			sprintf(buf, "dpio_closeHdl: must specify a handle");
			stuffChatText("*****", buf);
			return TRUE;
		}
		h = atoi(arg2);

		/*  Close the comm handle */
		err = dpio_closeHdl(dpio, h);
		if (err != dp_RES_OK) {
			sprintf(buf, "Unable to close handle %lu; err %d", h, err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Closed handle %lu", h);
		stuffChatText("*****", buf);
	} else if (MATCH("dpio_findTimedOutHost", "Find closed handles")) {
		playerHdl_t h;

		h = dpio_findTimedOutHost(dpio
#ifndef dp_ANET2
				, PLAYER_NONE
#endif
				);
		if (h != PLAYER_NONE) {
			sprintf(buf, "Player %lu has timed out.  Calling closeHdlImmed.", h);
			stuffChatText("*****", buf);
			dpio_closeHdlImmed(dpio, h);
			return TRUE;
		}
		sprintf(buf, "No timed out players.");
		stuffChatText("*****", buf);
	} else if (MATCH("dpio_setMaxPlayerHdls", "Set maximum handles")) {
		int h = atoi(arg2);
		if (!arg2[0] || !h) {
			sprintf(buf, "dpio_setMaxPlayerHdls: must specify a number");
			stuffChatText("*****", buf);
			return TRUE;
		}
		if (dpio)
			dpio_setMaxPlayerHdls(dpio, h);
		if (myDP)
			dpio_setMaxPlayerHdls(myDP->dpio, h);
		sprintf(buf, "Set max player handles to %d.", h);
		stuffChatText("*****", buf);
#ifdef dp_ANET2
	} else if (MATCH("dpio_setHandleOptions", "Set handle options")) {
		int h;
		int options;
		if (!dpio) {
			sprintf(buf, "dpio_setHandleOptions: must dpio_create first");
			stuffChatText("*****", buf);
			return TRUE;
		}
		if (argc != 2) {
			sprintf(buf, "dpio_setHandleOptions: must specify a handle and a value");
			stuffChatText("*****", buf);
			return TRUE;
		}
		h = argvi[0];
		options = argvi[1];
		err = dpio_setHandleOptions(dpio, h, options);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d setting handle options for handle 0x%x to 0x%x.", err, h, options);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Set handle options for handle 0x%x to 0x%x.", h, options);
		stuffChatText("*****", buf);
#endif
	} else if (MATCH("dpio_put_reliable", "Send datagram via dpio reliably to given node")) {
		playerHdl_t dest;
		playerHdl_t errhdl;
		int got;
		char pktbuf[dpio_MAXLEN_UNRELIABLE];
		int i;
		int pkts = 1;

		got = 0;
		if (!arg2[0] || (1 > (got = sscanf(arg2, "%lu %d", &dest, &pkts)))) {
			sprintf(buf, "dpio_put_reliable: must specify a dest player number, and optionally a pkt count");
			stuffChatText("*****", buf);
			return TRUE;
		}
		DPRINT(("dpr: request to send %d pkts to %d\n", pkts, dest));
		for (i=0; i<(signed)sizeof(pktbuf); i++)
			pktbuf[i] = i;
		for (i=0; i<pkts; i++) {
			err = dpio_put_reliable(dpio, &dest, 1, &pktbuf, dpio_MAXLEN_RELIABLE, &errhdl);
			if (err != dp_RES_OK) {
				sprintf(buf, "Error %d sending to id %lu", err, dest);
				stuffChatText("*****", buf);
				return TRUE;
			}
		}
		sprintf(buf, "Sent %d bytes %d times to handle %lu reliably", dpio_MAXLEN_RELIABLE, pkts, dest);
		stuffChatText("*****", buf);
	} else if (MATCH("dpio_freeze", "Save dpio to disk [dpio.dat]")) {
		dp_result_t err;
		char freezefile[_MAX_PATH];
		FILE *fp;
		if (!dpio) {
			stuffChatText("*****", "dpio_freeze: must call dpio_create first");
			return TRUE;
		}
		/* Note: after freezing, call dpio_destroy without
		 * calling dpio_close, dpio_receive, or dpio_send, otherwise
		 * your reliable transport windows will be screwed up!
		 */
		if (!arg2[0]) {
			stuffChatText("*****", "Defaulting to dpio.dat");
			arg2 = "dpio.dat";
		}
		strcpy(freezefile, arg2);
		fp = fopen(freezefile, "wb");
		if (!fp) {
			sprintf(buf, "Errno %d creating freeze file %s", errno, freezefile);
			stuffChatText("*****", buf);
			return TRUE;
		}
		err = dpio_freeze(dpio, fp);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d freezing", err);
			stuffChatText("*****", buf);
			fclose(fp);
			return TRUE;
		}
		stuffChatText("*****", "Frozen.  dpio_thaw to continue.");
		dpio_destroy(dpio, 0);	/* go ahead and hang up */
		dpio = NULL;
	} else if (MATCH("dpio_thaw", "Restore dpio from disk [dpio.dat]")) {
		dp_result_t err;
		char freezefile[_MAX_PATH];
		FILE *fp;
		if (!arg2[0]) {
			stuffChatText("*****", "Defaulting to dpio.dat");
			arg2 = "dpio.dat";
		}
		strcpy(freezefile, arg2);
		if (myDP) {
			stuffChatText("*****", "dpio_thaw: DP already created.");
			return TRUE;
		}
		if (dpio) {
			stuffChatText("*****", "dpio_thaw: dpio already created.");
			return TRUE;
		}
		fp = fopen(freezefile, "rb");
		if (!fp) {
			sprintf(buf, "Errno %d opening freeze file %s", errno, freezefile);
			stuffChatText("*****", buf);
			return TRUE;
		}
		err = dpio_create(&dpio, NULL, NULL, &dpio_now, fp);
		fclose(fp);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d thawing dpio", err);
			stuffChatText("*****", buf);
			pleaseQuit = TRUE;
			return TRUE;
		}
		dpio_setIncomingCallback(dpio, openHdl_cb, "<INCOMING>");
		stuffChatText("*****", "dpio thawed.");
#endif
#ifdef dp_ANET2
	} else if (MATCH("statinit", "initialize dp statistics")) {
		err = dp_avgstat_init(myDP, &avgstat);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dp_avgstat_init", err);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("statprint", "print dp statistics")) {
		dp_stats_t stats1;
		dp_stats_t stats60;
		dp_stats_t max1;
		dp_stats_t max60;
		err = dp_avgstat_poll(myDP, &avgstat, &stats1, &stats60, &max1, &max60);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dp_avgstat_poll", err);
			stuffChatText("*****", buf);
		}
		stuffChatText("*****", "one minute average");
		err = dp_avgstat_print(myDP, &stats1, statprint_fn, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dp_avgstat_print", err);
			stuffChatText("*****", buf);
		}
		stuffChatText("*****", "sixty minute average");
		err = dp_avgstat_print(myDP, &stats60, statprint_fn, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dp_avgstat_print", err);
			stuffChatText("*****", buf);
		}
		stuffChatText("*****", "one minute max");
		err = dp_avgstat_print(myDP, &max1, statprint_fn, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dp_avgstat_print", err);
			stuffChatText("*****", buf);
		}
		stuffChatText("*****", "sixty minute max");
		err = dp_avgstat_print(myDP, &max60, statprint_fn, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dp_avgstat_print", err);
			stuffChatText("*****", buf);
		}
#endif
/*---------------| startup/shutdown |-----------------------*/
	} else if (MATCH("enumtransports", "list available transports")) {
		dp_transport_t dlldir;
		if (!arg2[0]) {
			arg2="";
		}
		memset(&dlldir, 0, sizeof(dlldir));
		strcpy(dlldir.fname, arg2);
		stuffChatText("****", "Printing transport info:");
		callbacksFinished = FALSE;
		err = dpEnumTransports(&dlldir, enumTransports_cb, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpEnumTransports", err);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("gameservers", "list available game servers")) {
		dp_species_t sessType = 0;
		stuffChatText("****", "Listing servers:");
		callbacksFinished = FALSE;
		if (argc == 1) {
			sessType = argvi[0];
			err = dpEnumServersEx(myDP, 3000L, sessType,
									listServersEx_cb, NULL);
		} else
			err = dpEnumServers(myDP, 3000L, listServers_cb, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpEnumServers", err);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("setGameServer", "connects to a game server")) {
#ifdef dp_MULTISESSTABLE
		char *hostname = argvs[0];
		if (argc != 2) {
			stuffChatText("*****", "setGameServer: you must specify a server name and a session type");
			return TRUE;
		}
		if (!strcmp(hostname, "0"))
			hostname = NULL;
		err = dpSetGameServerEx(myDP, hostname, (short)argvi[1]);
		n_sessions = 0;
		if (err != dp_RES_OK) {
			sprintf(buf, "Unable to set game server to %s, type %d, error: %d", argvs[0], argvi[1], err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Game server set to %s, type %d", argvs[0], argvi[1]);
		cur_species = argvi[1];
		stuffChatText("*****", buf);
#else
		if (!arg2[0]) {
			stuffChatText("*****", "setGameServer: you must specify a server name");
			return TRUE;
		}
		err = dpSetGameServer(myDP, arg2);
		n_sessions = 0;
		if (err != dp_RES_OK) {
			sprintf(buf, "Unable to set game server to %s, error: %d", arg2, err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Game server set to %s", arg2);
		stuffChatText("*****", buf);
#endif
	} else if (MATCH("login", "log in to a game server")) {
		char *username = argvs[0];
		char *password = argvs[1];
		if (argc != 2) {
			stuffChatText("*****", "login: you must specify a username and a password");
			return TRUE;
		}
		err = dpAccountLogin(myDP, username, password);
		if (err != dp_RES_OK) {
			sprintf(buf, "Unable to queue login request, error: %d", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Login request queued.");
		stuffChatText("*****", buf);
#if 0
	} else if (MATCH("acctcreate", "create a new account on a game server")) {
		char *username = argvs[0];
		char *password = argvs[1];
		char *email = argvs[2];
		int flags;
		if (argc != 4) {
			stuffChatText("*****", "acctcreate: you must specify a username, password, email, and flags");
			return TRUE;
		}
		flags = atoi(argvs[3]);
		err = dpAccountCreate(myDP, username, password, flags, email);
		if (err != dp_RES_OK) {
			sprintf(buf, "Unable to queue account create request, error: %d", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Account create request queued.");
		stuffChatText("*****", buf);
	} else if (MATCH("acctchange", "change an account on a game server")) {
		char *password = argvs[0];
		char *newpassword = argvs[1];
		char *email = argvs[2];
		int flags;
		if (argc != 4) {
			stuffChatText("*****", "acctchange: you must specify your old password, new password, email, and flags");
			return TRUE;
		}
		flags = atoi(argvs[3]);
		err = dpChangePassword(myDP, password, newpassword, flags, email);
		if (err != dp_RES_OK) {
			sprintf(buf, "Unable to queue account change request, error: %d", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Account change request queued.");
		stuffChatText("*****", buf);
#endif
	} else if (MATCH("acctactivate", "activate an account on a game server")) {
		char *code = argvs[0];
		if (argc != 1) {
			stuffChatText("*****", "acctactivate: you must specify an activation code");
			return TRUE;
		}
		err = dpAccountActivate(myDP, code);
		if (err != dp_RES_OK) {
			sprintf(buf, "Unable to queue account activate request, error: %d", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Account activate request queued.");
		stuffChatText("*****", buf);
	} else if (MATCH("requestcode", "send a request for an activation code email to the game server")) {
		if (argc != 0) {
			stuffChatText("*****", "requestcode takes no parameters");
			return TRUE;
		}
		err = dpRequestEmail(myDP);
		if (err != dp_RES_OK) {
			sprintf(buf, "Unable to queue email request, error: %d", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Activation code email request queued.");
		stuffChatText("*****", buf);
	} else if (MATCH("scorestart", "Start a score report")) {
		err = dpReportScoreStart(myDP, 0);
		if (err != dp_RES_OK) {
			sprintf(buf, "dpReportScoreStart returns err:%d\n", err);
			stuffChatText("*****", buf);
		}
		stuffChatText("*****", "Ready for scorereports.\n");
		return TRUE;
	} else if (MATCH("scorereport", "Add a score to the report")) {
		int scoreId;
		long scoreVal;

		if (argc != 2) {
			stuffChatText("*****", "scorereport: you must specify a scoreId and score value.");
			return TRUE;
		}
		scoreId = argvi[0];
		scoreVal = argvi[1];
		err = dpReportScore2(myDP, thisPlayer, scoreId, scoreVal);
		if (err != dp_RES_OK) {
			DPRINT(("gtest: Error %d calling dpReportScore2\n", err));
			sprintf(buf, "dpReportScore2 returns err:%d\n", err);
			stuffChatText("*****", buf);
		}
		return TRUE;
	} else if (MATCH("scoreend", "End and submit a score report")) {
		err = dpReportScoreEnd(myDP);
		if (err != dp_RES_OK) {
			DPRINT(("gtest: Error %d calling dpReportScoreEnd\n", err));
			sprintf(buf, "dpReportScoreEnd returns err:%d\n", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		stuffChatText("*****", "Score report finished.\n");
		return TRUE;
	} else if (MATCH("dpCreate", "load dp")) {
		if (myDP) {
			stuffChatText("*****", "DP already created.  Can't create again.");
			return TRUE;
		}
		err = dpCreate(&myDP, &CommDLLName, &commInitReq, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d creating!", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Created dp.");
		stuffChatText("*****", buf);
	} else if (MATCH("dpDestroy", "unload dp")) {
		if (myDP) {
			dpDestroy(myDP, 0);
			myDP = NULL;
			stuffChatText("*****", "dp destroyed.");
		} else
			stuffChatText("*****", "no dp to destroy.");
	} else if (MATCH("freeze", "freeze the current session and destroy")) {
		dp_result_t err;
		char freezefile[_MAX_PATH];
		/* Note: after freezing, call dpDestroy without
		 * calling dpClose, otherwise your players will be deleted!
		 */
		if (!arg2[0]) {
			stuffChatText("*****", "Defaulting to freeze.dat");
			arg2 = "freeze.dat";
		}
		strcpy(freezefile, arg2);
		sprintf(buf, "New freeze file %s\n", freezefile);
		stuffChatText("*****", buf);
		err = dpFreeze(myDP, freezefile);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d freezing", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		stuffChatText("*****", "Frozen (ready or not!).  Thaw to continue.");
		dpDestroy(myDP, 0);	/* go ahead and hang up */
		myDP = NULL;
	} else if (MATCH("thaw", "Restore a dp session from disk")) {
		if (myDP) {
			stuffChatText("*****", "DP already created.  Can't thaw.");
			return TRUE;
		}
		if (!arg2[0]) {
			stuffChatText("*****", "Defaulting to freeze.dat");
			arg2 = "freeze.dat";
		}
		/* Specify commdllname in case this is a cross-platform freeze/thaw */
		err = dpCreate(&myDP, &CommDLLName, NULL, arg2);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d thawing file %s; quitting", err, arg2);
			stuffChatText("*****", buf);
			pleaseQuit = TRUE;
			return TRUE;
		}
		sprintf(buf, "Thawed file %s", arg2);
		stuffChatText("*****", buf);
/*---------------| session mgmt |-----------------------*/
	} else if (MATCH("species", "Set species used for listing and hosting")) {
		int got;

		got = 0;
		if (!arg2[0] || (1 != (got = sscanf(arg2, "%hu", &cur_species)))) {
			sprintf(buf, "species: must specify a value for 'species'");
			stuffChatText("*****", buf);
			return TRUE;
		}
		DPRINT(("processChatCommand: set current species to %d\n", cur_species));
		sprintf(buf, "set current species to %d\n", cur_species);
		stuffChatText("*****", buf);
	} else if (MATCH("host", "create new session; name [maxpl] [flags] [dwuser1].")) {
		char *roomname =       argvs[0];
		int maxpl=((argc>1)&&(argvi[1]!=BADVAL)) ? argvi[1]:dp_MAXREALPLAYERS;
		int flags=((argc>2)&&(argvi[2]!=BADVAL)) ? argvi[2]:0;
		int user1=((argc>3)&&(argvi[3]!=BADVAL)) ? argvi[3]:0;

		/* Create a room */
		memset(&sess, 0, sizeof(sess));
		sess.sessionType = cur_species;
		sess.maxPlayers = maxpl;
		if (argc < 2) {
			stuffChatText("*****", "Usage: host Room_Name [maxpl] [flags] [dwuser1]");
		} else {
			strncpy(sess.sessionName, roomname, dp_SNAMELEN);
			sess.sessionName[dp_SNAMELEN-1] = 0;
			sess.flags = dp_SESSION_FLAGS_CREATESESSION | flags;
			sess.dwUser1 = user1;
			err = dpOpen(myDP, &sess, create_sess_cb, NULL);
			if (err != dp_RES_OK) {
				sprintf(buf, "Unable to create a session, error: %d", err);
				stuffChatText("*****", buf);
			} else {
				sprintf(buf, "Called dpOpen to create session; maxpl %d, flags %x, dwUser1 %x", sess.maxPlayers, sess.flags, (unsigned) sess.dwUser1);
				stuffChatText("*****", buf);
			}
		}
	} else if (MATCH("lobby", "declare the current session to be a server lobby")) {
		err = dpDeclareLobby(myDP, 1);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpDeclareLobby", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		{
			dp_session_t s;
			size_t len;
			len = sizeof(s);
			err = dpGetSessionDesc(myDP, &s, &len);
			if (err != dp_RES_OK) {
				sprintf(buf, "error %d calling dpGetSessionDesc", err);
				stuffChatText("*****", buf);
				return TRUE;
			}
			s.dwUser1 |= 1;
			err = dpSetSessionDesc(myDP, &s, 0);
			if (err != dp_RES_OK) {
				sprintf(buf, "error %d calling dpSetSessionDesc", err);
				stuffChatText("*****", buf);
				return TRUE;
			}
		}
		stuffChatText("*****", "Becoming server lobby.");
	} else if (MATCH("join", "join an existing session")) {
		/* Join a particular session.  User specifies session name. */
		DPRINT(("processChatCommand: request to join %s\n", arg2));
		if (!arg2[0]) {
			stuffChatText("*****", "join: you need to specify a room name.");
			return TRUE;
		}
		sprintf(buf, "Joining first room matching \"%s\"", arg2);
		stuffChatText("*****", buf);
		callbacksFinished = FALSE;
		theError = joinSessionByName(myDP, arg2);
		if (theError != dp_RES_OK) {
			sprintf(buf, "Unable to join a session, error: %d", theError);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("joinany", "join any existing session")) {
		/* Join any particular session. User can specify nothing or
		 * address, session type and flags.*/
		dp_session_t sess;
		if (argc != 0 && !(argc == 3 && argvs[0] && argvi[1])) {
			stuffChatText("*****", "joinany: you need to specify either nothing or address, session type and flags.");
			return TRUE;
		}
		memset(&sess, 0, sizeof(sess));
		theError = dp_RES_OK;
		if (argc == 3) {
			char newHostAdr[dp_MAX_ADR_LEN];
			theError = dpResolveHostname(myDP, argvs[0], newHostAdr);
			if (theError != dp_RES_OK)
				DPRINT(("Unable to get address, err:%d\n", theError));
			else {
				strcpy((char*) sess.adrMaster, newHostAdr);
				sess.sessionType = argvi[1];
				sess.flags = argvi[2];
				cur_species = sess.sessionType;
				DPRINT(("processChatCommand: request to join any session on %s\n", argvs[0]));
			}
		} else {
			DPRINT(("processChatCommand: request to join any session on game server\n"));
		}
		callbacksFinished = FALSE;
		if (argc == 3 && theError == dp_RES_OK)
			theError = dpOpen(myDP, &sess, join_sess_cb, NULL);
		else if (argc == 0)
			theError = dpOpen(myDP, NULL, join_sess_cb, NULL);
		if (theError != dp_RES_OK) {
			sprintf(buf, "Unable to join a session, error: %d", theError);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("dpClose", "Disconnect from the current session")) {
		if (myDP) {
			dpClose(myDP);
			stuffChatText("*****", "dp closed.");
		} else
			stuffChatText("*****", "no dp to close.");
	} else if (MATCH("sessions", "list available sessions")) {
		/* List game sessions */
		sess.sessionType = cur_species;
		stuffChatText("****", "Listing sessions:");
		n_sessions = 0;
		callbacksFinished = FALSE;
		err = dpEnumSessions(myDP, &sess, NULL, 1750L, listSessions_cb, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpEnumSessions", err);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("setPingIntervals", "set ping intervals in ms; <piggyback> <forced>")) {
		int piggyback_interval;
		int forced_interval;

		if (argc != 2) {
			stuffChatText("*****", "setPingIntervals: you must specify piggyback and forced ping intervals (or 0 to disable)");
			return TRUE;
		}
		piggyback_interval = argvi[0];
		forced_interval = argvi[1];
		err = dpSetPingIntervals(myDP, piggyback_interval, forced_interval);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpSetPingIntervals", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Set ping intervals to piggyback:%d ms, forced:%d ms\n",
			piggyback_interval, forced_interval);
		stuffChatText("*****", buf);
	} else if (MATCH("sdeltas", "request session deltas; [on|off]")) {
		char key[1];
		int keylen = 1;
		int enable = (argc == 0) || (stricmp(argvs[0], "on")==0);
		key[0] = dp_KEY_SESSIONS;
		err = dpRequestObjectDeltas(myDP, enable, key, keylen);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpRequestObjectDeltas", err);
			stuffChatText("*****", buf);
		} else {
			if (enable)
				stuffChatText("*****", "Session deltas enabled.");
			else
				stuffChatText("*****", "Session deltas disabled.");
		}
	} else if (MATCH("gdeltas", "request gameserver deltas; [on|off]")) {
		int keylen = 3;
		int enable = (argc == 0) || (stricmp(argvs[0], "on")==0);
		char key[3];
		key[0] = dp_KEY_SERVERPINGS;
		key[1] = (char) dpGETSHORT_FIRSTBYTE(cur_species);
		key[2] = (char) dpGETSHORT_SECONDBYTE(cur_species);
		err = dpRequestObjectDeltas(myDP, enable, key, keylen);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpRequestObjectDeltas", err);
			stuffChatText("*****", buf);
		} else {
			if (enable) {
				sprintf(buf, "Gameserver deltas for sessiontype %d enabled.", cur_species);
				stuffChatText("*****", buf);
			} else
				stuffChatText("*****", "Gameserver deltas disabled.");
		}
	} else if (MATCH("pdeltas", "request player deltas; <on|off> [roomname]")) {
		int keylen;
		char key[dp_MAX_ADR_LEN + 3];
		int enable;
		if (((argc != 1) && (argc != 2))
		||  (!stricmp(argvs[0], "on") && !stricmp(argvs[0], "off"))) {
			stuffChatText("*****", "pdeltas: you need to specify on or off and, optionally, one word for room name");
			return TRUE;
		}
		enable = (stricmp(argvs[0], "on") == 0);
		if (argc == 2) {
			for (i = 0; i < n_sessions; i++) {
				DPRINT(("pdeltas: %s =? %s\n", sessions[i].sessionName, argvs[1]));
				if (strnicmp(sessions[i].sessionName, argvs[1], strlen(argvs[1])) == 0)
					break;
			}
			if (i == n_sessions) {
				sprintf(buf, "session \"%s\" not found", argvs[1]);
				stuffChatText("*****", buf);
				return TRUE;
			}
			dpGetSessionId(myDP, &sessions[i], &key[1], &keylen);
		} else {
			dp_session_t sess;
			err = dpGetSessionDesc(myDP, &sess, NULL);
			if (err != dp_RES_OK) {
				sprintf(buf, "error %d getting current session", err);
				stuffChatText("*****", buf);
				return TRUE;
			}
			dpGetSessionId(myDP, &sess, &key[1], &keylen);
		}

		key[0] = dp_KEY_PLAYERS;
		keylen++;
		err = dpRequestObjectDeltas(myDP, enable, key, keylen);
		if (err != dp_RES_OK)
			sprintf(buf, "error %d calling dpRequestObjectDeltas", err);
		else if (enable)
			sprintf(buf, "Player deltas for session \"%s\" enabled.", sessions[i].sessionName);
		else
			sprintf(buf, "Player deltas disabled for session \"%s\".", sessions[i].sessionName);
		stuffChatText("*****", buf);
		buf[0] = 0;
	} else if (MATCH("ldeltas", "request player latency deltas; [on|off]")) {
		int keylen;
		char key[dp_MAX_ADR_LEN + 3];
		int enable;
		if ((argc != 0) && ((argc != 1) ||
			(!stricmp(argvs[0], "on") && !stricmp(argvs[0], "off")))) {
			stuffChatText("*****", "ldeltas: usage: ldeltas [on|off]");
			return TRUE;
		}
		enable = (argc == 0) || (stricmp(argvs[0], "on")==0);
		keylen = 1;
		key[0] = dp_KEY_PLAYER_LATENCIES;
		err = dpRequestObjectDeltas(myDP, enable, key, keylen);
		if (err != dp_RES_OK)
			sprintf(buf, "error %d calling dpRequestObjectDeltas", err);
		else if (enable)
			sprintf(buf, "Player latency deltas for this session enabled.");
		else
			sprintf(buf, "Player latency deltas disabled for this session.");
		stuffChatText("*****", buf);
		buf[0] = 0;
	} else if (MATCH("dpGetSessionDesc", "get current session description")) {
		dp_session_t s;
		size_t len = sizeof(dp_session_t);
		err = dpGetSessionDesc(myDP, &s, &len);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpGetSessionDesc", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "(k %d; %d/%d plrs; spec %d; fl %x; U1 %x; pw '%s') name %s",
			s.karma & 0xffff, s.currentPlayers, s.maxPlayers,
			s.sessionType, s.flags, (unsigned) s.dwUser1, s.szPassword, s.sessionName);
		stuffChatText("****", buf);
/*---------------| player mgmt |-----------------------*/
	} else if (MATCH("players", "list players in current session")) {
		/* List game players */
		stuffChatText("****", "Listing players in current session:");
		callbacksFinished = FALSE;
		n_players = 0;
		thisPlayer = dp_ID_NONE;
		anyPlayer = dp_ID_NONE;
		err = dpEnumPlayers(myDP, NULL, listPlayers_cb, NULL, 1750L);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpEnumPlayers", err);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("rplayers", "list players in named session")) {
		int i;
		/* Find the given session. */
		for (i=0; i<n_sessions; i++) {
			DPRINT(("rplayers: %s =? %s\n",sessions[i].sessionName, arg2));
			if (strnicmp(sessions[i].sessionName, arg2, strlen(arg2)) == 0)
				break;
		}
		if (i == n_sessions) {
			sprintf(buf, "Unable to find session matching %s", arg2);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Listing players in session %s:", sessions[i].sessionName);
		stuffChatText("****", buf);
		callbacksFinished = FALSE;
		n_players = 0;
		err = dpEnumPlayers(myDP, &sessions[i], listPlayers_cb, NULL, 1750L);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpEnumPlayers", err);
			stuffChatText("*****", buf);
		}
#ifdef dp_ANET2
	} else if (MATCH("xplayers", "list players in current session")) {
		/* List game players */
		stuffChatText("****", "Listing players in current session:");
		callbacksFinished = FALSE;
		n_players = 0;
		thisPlayer = dp_ID_NONE;
		anyPlayer = dp_ID_NONE;
		err = dpEnumPlayersEx(myDP, NULL, listPlayersEx_cb, NULL, 1750L);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpEnumPlayers", err);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("xrplayers", "list players in named session")) {
		int i;
		/* Find the given session. */
		for (i=0; i<n_sessions; i++) {
			DPRINT(("rplayers: %s =? %s\n",sessions[i].sessionName, arg2));
			if (strnicmp(sessions[i].sessionName, arg2, strlen(arg2)) == 0)
				break;
		}
		if (i == n_sessions) {
			sprintf(buf, "Unable to find session matching %s", arg2);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Listing players in session %s:", sessions[i].sessionName);
		stuffChatText("****", buf);
		callbacksFinished = FALSE;
		n_players = 0;
		err = dpEnumPlayersEx(myDP, &sessions[i], listPlayersEx_cb, NULL, 1750L);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpEnumPlayers", err);
			stuffChatText("*****", buf);
		}
#endif
	} else if (MATCH("enableNewPlayers", "Enable or disable new players")) {
		int enable;
		if (argc != 1) {
			sprintf(buf, "enableNewPlayers: must specify 0 (disable) or 1 (enable)");
			stuffChatText("*****", buf);
			return TRUE;
		}
		enable = argvi[0];
		DPRINT(("enableNewPlayers: enable=%d\n", enable));
		err = dpEnableNewPlayers(myDP, enable);
		if (err != dp_RES_OK) {
			sprintf(buf, "dpEnableNewPlayers(,%d) returns err %d", enable, err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		if (enable)
			stuffChatText("*****", "Enabled new players");
		else
			stuffChatText("*****", "Disabled new players");
	} else if (MATCH("newplayer", "create a player in current session")) {
		DPRINT(("processChatCommand: request to create player %s\n", arg2));
		if (!arg2[0]) {
			stuffChatText("*****", "newplayer: you need to specify a player name.");
			return TRUE;
		}
		callbacksFinished = FALSE;
		strcpy (context, arg2);
		/* Strip trailing spaces from player name */
		while (*context && isspace(context[strlen(context)-1]))
			context[strlen(context)-1] = 0;
		err = dpCreatePlayer(myDP, create_player_cb, context, context);
		if (err != dp_RES_OK) {
			sprintf(buf, "Creating player \"%s\" returns err %d", arg2, err);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("delplayer", "destroy a player in current session")) {
		dpid_t id;
		char pname[256];
		int got;

		got = 0;
		if (!arg2[0] || (1 != (got = sscanf(arg2, "%hu", &id)))) {
			sprintf(buf, "delplayer: must specify a player number");
			stuffChatText("*****", buf);
			return TRUE;
		}
		DPRINT(("processChatCommand: request to delete player %s\n", arg2));
		err = dpGetPlayerName(myDP, id, pname, sizeof(pname));
		if (err != dp_RES_OK) {
			sprintf(buf, "Getting player %d's name returns err %d", id, err);
			stuffChatText("*****", buf);
			pname[0] = 0;
			/* return TRUE;   fall thru */
		}
		err = dpDestroyPlayer(myDP, id);
		if (err != dp_RES_OK) {
			sprintf(buf, "Deleting player %d returns err %d", id, err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Deleted player %d = %s", id, pname);
		stuffChatText("*****", buf);
#ifdef dp_ANET2
	} else if (MATCH("setpname", "change the name of the player with the given id")) {
		dpid_t id;
		char *pname;

		if (argc != 2) {
			sprintf(buf, "setpname: must specify a player number and name");
			stuffChatText("*****", buf);
			return TRUE;
		}
		id = argvi[0];
		pname = argvs[1];
		DPRINT(("processChatCommand: request to rename player %s\n", arg2));
		err = dpSetPlayerName(myDP, id, pname);
		if (err != dp_RES_OK) {
			sprintf(buf, "Setting player %d's name returns err %d", id, err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Set name of player %d to %s", id, pname);
		stuffChatText("*****", buf);
	} else if (MATCH("setpblob", "change the blob of the player with the given id")) {
		dpid_t id;
		char *pblob;

		if (argc != 2) {
			sprintf(buf, "setpblob: must specify a player number and blob");
			stuffChatText("*****", buf);
			return TRUE;
		}
		id = argvi[0];
		pblob = argvs[1];
		DPRINT(("processChatCommand: request to reblob player %s\n", arg2));
		err = dpSetPlayerBlob(myDP, id, pblob, strlen(pblob));
		if (err != dp_RES_OK) {
			sprintf(buf, "Setting player %d's blob returns err %d", id, err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Set blob of player %d to %s", id, pblob);
		stuffChatText("*****", buf);
#endif
/*---------------| group mgmt |-----------------------*/
	} else if (MATCH("groups", "list groups in current session")) {
		/* List game groups */
		stuffChatText("****", "Listing groups in current session:");
		callbacksFinished = FALSE;
		err = dpEnumGroups(myDP, NULL, listGroups_cb, NULL, 1750L);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpEnumGroups", err);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("newgroup", "create a group in current session")) {
		dpid_t id;
		DPRINT(("processChatCommand: request to create group %s\n", arg2));
		if (!arg2[0]) {
			stuffChatText("*****", "newgroup: you need to specify a group name.");
			return TRUE;
		}
		id = dp_ID_NONE;
		err = dpCreateGroup(myDP, &id, arg2);
		anyGroup = id;
		thisGroup = id;
		sprintf(buf, "Creating group \"%s\" returns id %d, err %d", arg2, id, err);
		stuffChatText("*****", buf);
	} else if (MATCH("delgroup", "destroy a group in current session")) {
		DPRINT(("processChatCommand: request to delete group %s\n", arg2));
		if (!arg2[0]) {
			stuffChatText("*****", "delgroup: you need to specify a group name.");
			return TRUE;
		}
		err = enumGroupsByName(myDP, arg2, NULL, delGroups_cb, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling enumGroupsByName", err);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("gaddplayer", "add a player to a group")) {
		dpid_t idGroup, idPlayer;
		char pname[256], gname[256];
		int got;

		got = 0;
		if (argc != 2) {
			sprintf(buf, "gaddplayer: must specify a group and player number (%d out of 2 ain't bad).", got);
			stuffChatText("*****", buf);
			return TRUE;
		}
		idGroup = argvi[0];
		idPlayer = argvi[1];
		if (idPlayer <= 0 || idGroup <= 0) {
			sprintf(buf, "gaddplayer: unknown group number %s(%d) or player number %s(%d).", argvs[0], argvi[0], argvs[1], argvi[1]);
			stuffChatText("*****", buf);
			return TRUE;
		}
		DPRINT(("processChatCommand: request to add player %d to group %d\n", idPlayer, idGroup));
		err = dpGetPlayerName(myDP, idPlayer, pname, sizeof(pname));
		if (err != dp_RES_OK) {
			sprintf(buf, "Getting player %d's name returns err %d", idPlayer, err);
			stuffChatText("*****", buf);
			pname[0] = 0;
			/* return TRUE;   fall thru */
		}
		err = dpGetPlayerName(myDP, idGroup, gname, sizeof(gname));
		if (err != dp_RES_OK) {
			sprintf(buf, "Getting group %d's name returns err %d", idGroup, err);
			stuffChatText("*****", buf);
			gname[0] = 0;
			/* return TRUE;   fall thru */
		}
		err = dpAddPlayerToGroup(myDP, idGroup, idPlayer);
		if (err != dp_RES_OK) {
			sprintf(buf, "Adding player %d = %s to group %d = %s returns err %d",
				 idPlayer, pname, idGroup, gname, err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Added player %d = %s to group %d = %s",
			 idPlayer, pname, idGroup, gname);
		stuffChatText("*****", buf);
	} else if (MATCH("gdelplayer", "delete a player from a group")) {
		dpid_t idGroup, idPlayer;
		char pname[256], gname[256];
		int got;

		got = 0;
		if (!arg2[0] || (2 != (got = sscanf(arg2, "%hu %hu", &idGroup, &idPlayer)))) {
			sprintf(buf, "gdelplayer: must specify a group and player number (%d out of 2 ain't bad).", got);
			stuffChatText("*****", buf);
			return TRUE;
		}
		DPRINT(("processChatCommand: request to del player %d from group %d\n", idPlayer, idGroup));
		err = dpGetPlayerName(myDP, idPlayer, pname, sizeof(pname));
		if (err != dp_RES_OK) {
			sprintf(buf, "Getting player %d's name returns err %d", idPlayer, err);
			stuffChatText("*****", buf);
			pname[0] = 0;
			/* return TRUE;   fall thru */
		}
		err = dpGetPlayerName(myDP, idGroup, gname, sizeof(gname));
		if (err != dp_RES_OK) {
			sprintf(buf, "Getting group %d's name returns err %d", idGroup, err);
			stuffChatText("*****", buf);
			gname[0] = 0;
			/* return TRUE;   fall thru */
		}
		err = dpDeletePlayerFromGroup(myDP, idGroup, idPlayer);
		if (err != dp_RES_OK) {
			sprintf(buf, "Deleting player %d = %s from group %d = %s returns err %d",
				 idPlayer, pname, idGroup, gname, err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Deleted player %d = %s from group %d = %s",
			 idPlayer, pname, idGroup, gname);
		stuffChatText("*****", buf);
	} else if (MATCH("gplayers", "list players in given group")) {
		/* List group players */
		dpid_t idGroup;
		char gname[256];
		int got;

		got = 0;
		if (!arg2[0]) {
			sprintf(buf, "gplayers: must specify a group number");
			stuffChatText("*****", buf);
			return TRUE;
		}
		idGroup = argvi[0];
		if (idGroup <= 0) {
			sprintf(buf, "gaddplayer: unknown group number %s.", argvs[0]);
			stuffChatText("*****", buf);
			return TRUE;
		}
		err = dpGetPlayerName(myDP, idGroup, gname, sizeof(gname));
		if (err != dp_RES_OK) {
			sprintf(buf, "Getting group %d's name returns err %d", idGroup, err);
			stuffChatText("*****", buf);
			gname[0] = 0;
			/* return TRUE;   fall thru */
		}
		sprintf(buf, "Listing players in group %d = %s:", idGroup, gname);
		stuffChatText("****", buf);
		callbacksFinished = FALSE;
		n_players = 0;
		err = dpEnumGroupPlayers(myDP, idGroup, NULL, listPlayers_cb, NULL, 1750L);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpEnumPlayers", err);
			stuffChatText("*****", buf);
		}
/*---------------| player variables |-----------------------*/
	} else if (MATCH("pset", "Set a variable of a player")) {
		dpid_t id;
		char pname[256], val[256];
		int key;
		int got;

		got = 0;
		if (!arg2[0] || (3 != (got = sscanf(arg2, "%hu %d %s", &id, &key, val)))) {
			sprintf(buf, "pset: must specify a player number, key, and value (%d out of 3 ain't bad).", got);
			stuffChatText("*****", buf);
			return TRUE;
		}
		DPRINT(("processChatCommand: request to set variable %d for player %s to %s\n", key, pname, val));
		err = dpGetPlayerName(myDP, id, pname, sizeof(pname));
		if (err != dp_RES_OK) {
			sprintf(buf, "Getting player %d's name returns err %d", id, err);
			stuffChatText("*****", buf);
			pname[0] = 0;
			/* return TRUE;   fall thru */
		}
		err = dpSetPlayerData(myDP, id, key, val, strlen(val)+1, 0);
		if (err != dp_RES_OK) {
			sprintf(buf, "Setting player %d = %s's var %d returns err %d", id, pname, key, err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Set player %s's var %d to %s, len %zd", pname, key, val, strlen(val)+1);
		stuffChatText("*****", buf);
	} else if (MATCH("fpset", "Set a variable of a player to a given file")) {
		dpid_t id;
		char pname[256], fname[256];
		char *val;
		size_t len;
		int key;
		int got;
		FILE *fp;

		got = 0;
		if (!arg2[0] || (3 != (got = sscanf(arg2, "%hu %d %s", &id, &key, fname)))) {
			sprintf(buf, "fpset: must specify a player number, key, and filename (%d out of 3 ain't bad).", got);
			stuffChatText("*****", buf);
			return TRUE;
		}
		DPRINT(("processChatCommand: request to set variable %d for player %s to contents of %s\n", key, pname, fname));
		err = dpGetPlayerName(myDP, id, pname, sizeof(pname));
		if (err != dp_RES_OK) {
			sprintf(buf, "Getting player %d's name returns err %d", id, err);
			stuffChatText("*****", buf);
			pname[0] = 0;
			/* return TRUE;   fall thru */
		}
		fp = fopen(fname, "rb");
		if (!fp) {
			sprintf(buf, "Can't open file %s", fname);
			stuffChatText("*****", buf);
			return TRUE;
		}
		len = my_filesize(fp);
		if (len == 0 || len > 1000000) {
			fclose(fp);
			sprintf(buf, "Bad size %zd for file %s", len, fname);
			stuffChatText("*****", buf);
			return TRUE;
		}
		val = (char *) malloc(len);
		if (!val) {
			fclose(fp);
			sprintf(buf, "Can't allocate %zd bytes", len);
			stuffChatText("*****", buf);
			return TRUE;
		}
		if (fread(val, sizeof(char), len, fp) != len) {
			fclose(fp);
			sprintf(buf, "Can't read %zd bytes", len);
			stuffChatText("*****", buf);
			return TRUE;
		}
		fclose(fp);
		err = dpSetPlayerData(myDP, id, key, val, len, 0);
		if (err != dp_RES_OK) {
			sprintf(buf, "Setting player %d = %s's var %d returns err %d", id, pname, key, err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Set player %s's var %d to %20.20s, len %zd", pname, key, val, len);
		stuffChatText("*****", buf);
	} else if (MATCH("pget", "Get value of a variable of a player")) {
		dpid_t id;
		char pname[256];
		char *val;
		int key;
		int got;
		size_t len;

		got = 0;
		if (!arg2[0] || (2 != (got = sscanf(arg2, "%hu %d", &id, &key)))) {
			sprintf(buf, "pget: must specify a player number and key (%d out of 2 ain't bad).", got);
			stuffChatText("*****", buf);
			return TRUE;
		}
		DPRINT(("processChatCommand: request to set variable %d for player %s\n", key, pname));
		err = dpGetPlayerName(myDP, id, pname, sizeof(pname));
		if (err != dp_RES_OK) {
			sprintf(buf, "Getting player %d's name returns err %d", id, err);
			stuffChatText("*****", buf);
			pname[0] = 0;
			/* return TRUE;  fall thru */
		}
		len = 0;
		val = NULL;
		err = dpGetPlayerData(myDP, id, key, val, &len, 0);
		if (err != dp_RES_FULL) {
			sprintf(buf, "Getting size of player %d = %s's var %d returns err %d", id, pname, key, err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		val = (char *) malloc(len);
		if (!val) {
			stuffChatText("*****", "can't malloc");
			return TRUE;
		}
		err = dpGetPlayerData(myDP, id, key, val, &len, 0);
		if (err != dp_RES_OK) {
			free(val);
			sprintf(buf, "Getting player %d = %s's var %d returns err %d", id, pname, key, err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Player %s's var %d has len %zd, value %20.20s", pname, key, len, val);
		stuffChatText("*****", buf);
		free(val);
/*---------------| misc |-----------------------*/
	} else if (MATCH("ping", "measure round trip lag time to a player")) {
		DPRINT(("processChatCommand: request to ping %s\n", arg2));
		if (!arg2[0]) {
			stuffChatText("*****", "ping: you need to specify a callsign.");
			return TRUE;
		}
		sprintf(buf, "Pinging first player matching \"%s\"", arg2);
		stuffChatText("*****", buf);
		/* Strip trailing spaces from player name */
		while (*arg2 && isspace(arg2[strlen(arg2)-1]))
			arg2[strlen(arg2)-1] = 0;
		PingPlayerByName(arg2);

	} else if (MATCH("scap", "Get capabilities of current session.")) {
		dp_caps_t caps;
		int flags;
		int got;

		got = 0;
		if (!arg2[0] || (1 != (got = sscanf(arg2, "%d", &flags)))) {
			sprintf(buf, "scap: must specify a value (0 or 1) for 'flags'");
			stuffChatText("*****", buf);
			return TRUE;
		}
		DPRINT(("processChatCommand: request to get session caps\n"));
		err = dpGetCaps(myDP, &caps, flags);
		if (err != dp_RES_OK) {
			sprintf(buf, "Getting session caps returns err %d", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "dpGetCaps(%d): Size %ld, MaxBufferSize %ld, MaxPlayers %ld, MaxLocalPlayers %ld, Flags %x, Latency %ld\n",
			flags,
			caps.dwSize,
			caps.dwMaxBufferSize,
			caps.dwMaxPlayers,
			caps.dwMaxLocalPlayers,
			(unsigned) caps.dwFlags,
			caps.dwLatency);
		stuffChatText("*****", buf);
	} else if (MATCH("enumapp", "list available applications")) {
		/* List game applications */
		int i;
		stuffChatText("****", "Listing applications:");
		for (i=0; i<n_apps; i++) {
			free(apps[i].name);
			free(apps[i].args);
			free(apps[i].path);
			free(apps[i].cwd);
		}
		n_apps = 0;
		err = dpEnumApp(myDP, listApps_cb, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpEnumApp", err);
			stuffChatText("*****", buf);
		}
		if (n_apps == MAX_APPS) {
			stuffChatText("*****", "error: number of apps found > array bounds");
		}
#ifdef dp_ANET2
	} else if (MATCH("patch", "Check whether patch is needed [apply]")) {
		dp_appParam_t app;
		err = dpGetAppVersion(myDP, &app);
		if ((err == dp_RES_EMPTY) || (err == dp_RES_OK)) {
			sprintf(buf, "dpGetAppVersion returns type %d, plat %d, lang %d",
				app.sessionType, app.platform, app.language);
			stuffChatText("*****", buf);
		}
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpGetAppVersion", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "dpGetAppVersion says it's time to update!");
		stuffChatText("*****", buf);
		if (argc == 0)
			return TRUE;
		/* Update it! */
		err = dpDownloadUpdate(myDP, &app);
		if (err == dp_RES_OK) {
			stuffChatText("*****", "Updating!  User must restart app after patch.");
			exit(1);
		}
		sprintf(buf, "error %d calling dpDownloadUpdate", err);
		stuffChatText("*****", buf);
		return TRUE;
#endif
	} else if (MATCH("pickapp", "pick an existing application for use with launchapp")) {
		/* Pick a particular application.  User specifies application name. */
		DPRINT(("processChatCommand: request to start %s\n", arg2));
		if (!arg2[0]) {
			stuffChatText("*****", "pickapp: you need to specify an application name.");
			return TRUE;
		}
		sprintf(buf, "Picking first application matching \"%s\"", arg2);
		stuffChatText("*****", buf);
		theError = pickApp(arg2);
		sprintf(buf, "Picked application index %d", app_pick);
		stuffChatText("*****", buf);
		if (theError != dp_RES_OK) {
			sprintf(buf, "Unable to pick an application, error: %d", theError);
			stuffChatText("*****", buf);
		}
	} else if (MATCH("launchapp", "freeze, start the application selected from pickapp, then destroy")) {
		/* Start the application specified by app_pick.
		 * User must have called listApps() first.
		 */
		char separator;
		char freezefile[_MAX_PATH];
		comm_driverInfo_t info;
		DPRINT(("processChatCommand: request to start\n"));
		if (app_pick == -1) {
			stuffChatText("*****", "You must first pick a game with pickApp");
			return TRUE;
		}
		if (!arg2[0]) {
			stuffChatText("*****", "Defaulting to freeze.dat");
			arg2 = "freeze.dat";
		}
		strcpy(freezefile, apps[app_pick].cwd);
#ifdef UNIX
		separator = '/';
#elif defined(__POWERPC__)
		separator = ':';
#else
		separator = '\\';
#endif
		if (freezefile[strlen(freezefile) - 1] != separator) {
			char buf[2];
			sprintf(buf, "%c", separator);
			strcat(freezefile, buf);
			strcat(freezefile, arg2);
		} else {
			strcat(freezefile, arg2);
		}
		sprintf(buf, "New freeze file %s\n", freezefile);
		stuffChatText("*****", buf);
		err = dpFreeze(myDP, freezefile);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d freezing", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		stuffChatText("*****", "Frozen.  Thaw to continue.");
		/* If IPX or Internet, must destroy before launching!
		 * If Modem, must destroy after lauching!
		 * Serial could be either way...
		 */
		err = dpGetTransportInfo(&CommDLLName, &info);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d getting transport info", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		if (!(info.needs & comm_INIT_NEEDS_PHONENUM)) {
			dpDestroy(myDP, 0);
			myDP = NULL;
		}
		theError = dpLaunchApp(&apps[app_pick]);
		if (theError != dp_RES_OK && theError != dp_RES_EMPTY) {
			sprintf(buf, "Unable to start an application, error: %d", theError);
			stuffChatText("*****", buf);
		} else if (dp_RES_EMPTY == theError) {
			stuffChatText("*****", "Could not find stub");
		}
		if (info.needs & comm_INIT_NEEDS_PHONENUM) {
			sleep(5);	/* hope app does dpCreate in 5 seconds or less */
			dpDestroy(myDP, 1);	/* don't hang up */
			myDP = NULL;
		}
	} else if (MATCH("crash", "cause an exception")) {
		int i = 0, j = argc;
		j /= i;
/*---------------| sending/receiving datagrams |-------------*/
	} else if (MATCH("send", "send a packet to a given player or group")) {
		dpid_t idFrom, idTo;
		int got;
		char pktbuf[dp_MAXLEN_UNRELIABLE + sizeof_dp_envelope_t];
		int i;
		int pkts = 1;

		got = 0;
		if (argc < 2 || argc > 3) {
			sprintf(buf, "send: must specify a source and dest player number, and optionally a pkt count");
			stuffChatText("*****", buf);
			return TRUE;
		}
		idFrom = argvi[0];
		idTo = argvi[1];
		if ((argc == 3) && (argvi[2] != BADVAL))
			pkts = argvi[2];
		DPRINT(("processChatCommand: request to send from %d to %d, %d times\n", idFrom, idTo, pkts));
		for (i=0; i<(signed)sizeof(pktbuf); i++)
			pktbuf[i] = i;
		for (i=0; i<pkts; i++) {
			err = dpSend(myDP, idFrom, idTo, dp_SEND_UNRELIABLE, &pktbuf, dp_MAXLEN_UNRELIABLE);
			if (err != dp_RES_OK) {
				sprintf(buf, "Error %d sending to id %d", err, idTo);
				stuffChatText("*****", buf);
				return TRUE;
			}
		}
		sprintf(buf, "Sent %d bytes %d times to id %d unreliably", dp_MAXLEN_UNRELIABLE, pkts, idTo);
		stuffChatText("*****", buf);
		err = dpSend(myDP, idFrom, idTo, dp_SEND_RELIABLE, &pktbuf, dp_MAXLEN_RELIABLE);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d sending to id %d", err, idTo);
			stuffChatText("*****", buf);
			return TRUE;
		}
		sprintf(buf, "Sent %d bytes to id %d reliably, too", dp_MAXLEN_RELIABLE, idTo);
		stuffChatText("*****", buf);
	} else if (MATCH("chat", "broadcast a chat message")) {
		if (argc < 1) {
			sprintf(buf, "chat: require a chat message");
			stuffChatText("*****", buf);
			return TRUE;
		}
		DPRINT(("processChatCommand: request to broadcast chat message\n"));
		/* sender of chat message is mylowestId */
		if (mylowestId == dp_ID_NONE) {
			err = countMyPlayers(myDP);
			if (err != dp_RES_OK) {
				sprintf(buf, "Error %d finding mylowestId", err);
				stuffChatText("*****", buf);
				return TRUE;
			}
		}
		err = SendChatMessage(arg2);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d broadcasting chat message", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
#if 0
/*---------------| turn commands |-------------*/
	} else if (MATCH("turninit", "initialize turn system")) {
		if(turn_status != TURN_UNINIT) {
			stuffChatText("*****", "Turn already initialized.  Can't reinitialize.");
			return TRUE;
		}
		/* Initialize the turn system. */
		err=dpTurnInit(myDP);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d creating dpturn\n", err);
			stuffChatText("*****", buf);
			exit(2);
		}
		round = 0;
		turn_status = TURN_IDLE;
		turnstart = eclock();
		stuffChatText("*****", "Turn initialized.");
	} else if (MATCH("turnfinish", "shut down turn system")) {
		/* Shut down the turn system. */
		turnend = eclock();
		sprintf(buf, "%d Rounds in %d Seconds\n", round, (turnend - turnstart)/ECLOCKS_PER_SEC);
		stuffChatText("*****", buf);
		sprintf(buf, "%f RPS\n", ECLOCKS_PER_SEC * round / (float)(turnend - turnstart));
		stuffChatText("*****", buf);
		err=dpTurnFinish(myDP);
		if (err != dp_RES_OK) {
			sprintf(buf, "Error %d shutting down dpturn\n", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		turn_status = TURN_UNINIT;
		stuffChatText("*****", "Turn shut down.");
	} else if (MATCH("turnput", "send your move; use wait for other players' moves")) {
		char fakedata[200];
		int gPacketSize = 200;	/* DEFAULT PACKETSIZE */
		dp_result_t err;

		/* can do turnput if only one player has been created locally */
		if (round == 0) {
			countMyPlayers(myDP);
		}
		if (my_nPlayer != 1) {
			sprintf(buf, "Can only send if you have one player; you have %d players\n", my_nPlayer);
			stuffChatText("*****", buf);
			return TRUE;
		}
		/* data is set so that it can be checked by the receiving end */
		memset(fakedata, round & 255, gPacketSize);
		fakedata[0] = mylowestId & 255;
		err = dpTurnPutMove(myDP, fakedata, gPacketSize);
		if (err != dp_RES_OK) {
			sprintf(buf, "turnput failed, e:%d.  Sync lost.\n", err);
			stuffChatText("*****", buf);
			return TRUE;
		}
		callbacksFinished = FALSE;
		turn_status = TURN_SEND;
		sprintf(buf, "Move sent by player id %d.\n", mylowestId);
		stuffChatText("*****", buf);
#endif
	} else {
		stuffChatText("*****", "Unknown command.  Type help for a list of commands.");
	}
	return TRUE;
}

void usage()
{
	printf("\
Usage: gtest [-N:comm.dll][-B:busy10msec][-P:adrString][-f:foo.log][-S][-x]\n\
[-W:phonenum][-Y:commportnum(0 : com1)][-Z:baudrate][-I:modeministr]\n\
[-Kirq:irqnum][-Kport:hexportnum][-Kpulse][-Ktest]\n\
\n\
Option details:\n\
-n:foo.dll Load comm dll foo.dll\n\
-b:10      Burn CPU for 10 centisec, then call dpReceive until empty, repeat\n\
-f:foo.log Write log file to foo.log (if not overwridden by gtest.ini)\n\
-s         Read from stdin, not keyboard (needed for scripts in win32)\n\
-s:foo.txt Read from foo.txt, not keyboard (needed for scripts on Mac)\n\
-x         use dpio, not dp\n\
-c         don't create dp on startup\n\
-p:server  Connect to given game server (need to specify =t also)\n\
-t:species Connect to game server with given species (session type)\n\
\n\
Exit status is 0 for success, 1 for usage error, 2 for DLL load error,\n\
3 for assertion failure.\n\
");
}

/* ======================================================================== *
   Function Name: ProcessCommandLine
   Description  : Processes the command line arguments.  Some arguments are
   handled directly by this routine.  Others simply result in an update to
   the global switches data structure
 * ======================================================================== */
void ProcessCommandLine(int argc, char **argv) {
  char *chptr;
  int   i;

#ifndef __MWERKS__	/* commdll not gotten from commandline on mac */
  memset(&CommDLLName, 0, sizeof(CommDLLName));
#endif
  adrString[0] = 0;
  busyTime = 0;
  defaultSessType = 0;

  for (i = 1; i < argc; ++i) {
	if (argv[i][0] == '/' || argv[i][0] == '-') /*  deal with args that start with - or / */
	{
	  if (argv[i][1] == 0) {
		printf("Expected letter after -\n");
		exit(1);
	  }
	  switch(toupper(argv[i][1])) {
	  case 'C':
		dontCreate = TRUE;
		continue;
	  case 'X':
		use_dpio = TRUE;
		continue;
	  case 'S':
		if ((argv[i][2] == ':') || (argv[i][2] == '=')) {
			scriptfname = argv[i]+3;
			scriptfp = fopen(scriptfname, "r");
			if (!scriptfp) {
				printf("gtest: Can't open script file %s\n", scriptfname);
				perror(scriptfname);
				exit(1);
			}
		} else {
			scriptfname = "stdin";
			scriptfp = stdin;
		}
		continue;
	  case 'K':	/*  Comm parameters; Kport, Kirq, Kpulse */
		if (stricmp(argv[i], "-Kpulse") == 0) {
			commInitReq.dialing_method = comm_INIT_DIALING_METHOD_PULSE;
			continue;
		}
		if (stricmp(argv[i], "-Ktest") == 0) {
			/*  Do loopback test.  Other side must have -Ktest, too. */
			commInitReq.flags |= comm_INIT_FLAGS_TEST;
			continue;
		}
		break;
	  default:
	    ;
	  }
	  /* To separate flag names from flag arguments, we use : or =.
	   * On Mac, : is path separator, so support '='; search for = first!
	   * On PC, = is eaten by batch file argument processor, so support ':' ...
	   * it'll look a little funny with drive letters...
	   */
	  chptr = strchr(argv[i], '=');
	  if (!chptr) chptr = strchr(argv[i], ':');
	  if (!chptr) {
		printf("Expected : or = after switch name in %s\n", argv[i]);
		exit(1);
	  }
	  switch(toupper(argv[i][1])) {

		case 'B':
			printf("Setting busy time to %s ticks\n", argv[i]+3);
			busyTime = atoi(chptr+1);
			break;
		case 'F':
			printf("Log file name : %s\n", chptr+1);
			dp_setLogFname(chptr+1);
			break;
		case 'P':   /*  Server IP Address */
			strncpy(adrString, chptr+1, sizeof(adrString));
			adrString[sizeof(adrString)-1] = '\0';
			break;
		case 'T':   /*  Server session type */
			defaultSessType = atoi(chptr+1);
			break;
		case 'K':	/*  Comm parameters; Kport, Kirq, Kpulse */
			*chptr = 0;

			if (stricmp(argv[i], "-Kport") == 0) {
				sscanf(chptr+1, "%lx", &commInitReq.baseadr);
				/* printf("setting comm port base adr to 0x%x, effect depends on driver.\n", commInitReq.baseadr); */
				/* sleep(1); */
			} else if (stricmp(argv[i], "-Kirq") == 0) {
				sscanf(chptr+1, "%ld", &commInitReq.hwirq);
				/* printf("setting comm port irq to %d, effect depends on driver.\n", commInitReq.hwirq); */
				/* sleep(1); */
			} else {
				*chptr = ':';
				printf("unknown parameter %s", argv[i]);
				exit(1);
			}
			break;

		case 'N':   /*  Communications DLL name */
			strncpy(CommDLLName.fname, chptr+1, sizeof(CommDLLName.fname));
			CommDLLName.fname[sizeof(CommDLLName.fname)-1] = '\0';
			break;

		case 'I':   /*  Set modem init string */
			strncpy(modeministr, chptr+1, sizeof(modeministr));
			modeministr[sizeof(modeministr)-1] = '\0';
			break;

		case 'W':   /*  Set phone number */
			strncpy(phonenum, chptr+1, sizeof(phonenum));
			phonenum[sizeof(phonenum)-1] = '\0';
			break;

		case 'Y':   /*  Set comm port */
			sscanf(chptr+1, "%ld", &commInitReq.portnum);
			break;

		case 'Z':   /*  Set baud rate */
			sscanf(chptr+1, "%ld", &commInitReq.baud);
			break;

		default:
			usage();
			printf("%s: Unknown arg %s\n", argv[0], argv[i]);
		    exit(1);
	  } /*  end switch */
	}
  }
}

#if 0
__declspec(dllexport) int fakemain( int argc, char *argv[] )
#elif defined(_WIN32)
static int gtest( int argc, char *argv[] );

static LONG __cdecl Debug_ExceptionFilter(LPEXCEPTION_POINTERS ep)
{
	dpReportCrashEx(ep, "yech, crash in gtest");

	if (ep->ExceptionRecord->ExceptionCode == aeh_ASSERTION_CODE)
		return (EXCEPTION_CONTINUE_EXECUTION);

	return (EXCEPTION_CONTINUE_SEARCH);
}

int main( int argc, char *argv[] )
{
	__try {
		gtest(argc, argv);
	} __except(Debug_ExceptionFilter(GetExceptionInformation())) {
		;
	}

	quit(exitCode);
	return exitCode;	/* to make compiler happy */
}

#else
static int gtest( int argc, char *argv[] );

int main( int argc, char *argv[] )
{
	gtest(argc, argv);

	quit(exitCode);
	return exitCode;	/* to make compiler happy */
}
#endif

int gtest( int argc, char *argv[] )
{
	dp_result_t   err = 0;
	char kbuf[512];
	char buf[512];
	kbuf[0] = 0;

	#ifdef __MWERKS__
		// get shared library usin StdFile box, so we can debug
		// the shared library easily by launching its .SYM file
		// before launching gtest's .SYM file in the debugger.
        {
			StandardFileReply sfr;
			SFTypeList slType = {'shlb'};
			char *cname;

			InitGraf(&qd.thePort);
			InitFonts();
			InitWindows();
			InitMenus();
			TEInit();
			InitDialogs(0);

			StandardGetFile(0,1,slType,&sfr);

			if (!sfr.sfGood)
				return 0;
			p2cstr(sfr.sfFile.name);
			cname = (char *) sfr.sfFile.name;
			strncpy(CommDLLName.fname, cname, sizeof(CommDLLName.fname));
			CommDLLName.fname[sizeof(CommDLLName.fname)-1] = '\0';
			CommDLLName.vRefNum = sfr.sfFile.vRefNum;
			CommDLLName.dirID = sfr.sfFile.parID;

		}
		argc = ccommand(&argv);
	#else
		/* Is this a Mac thingy?
		argc = ccommand(&argv);*/
		if (argc == 1) {
			usage();
			exit(1);
		}
	#endif

	memset(&commInitReq, 0, sizeof(commInitReq));
	/* SessionId is vital for serial driver.  Must be different from
	 * run to run!
	 */
#ifdef _WIN32
	srand(GetTickCount() ^ GetCurrentProcessId());
#endif
	commInitReq.sessionId = rand() ^ (rand() << 16) ^ time(0) ^ eclock();
	commInitReq.reqLen = sizeof(commInitReq_t);
	modeministr[0] = 0;
	phonenum[0] = 0;
	commInitReq.modeministr = modeministr;
	commInitReq.phonenum = phonenum;
	commInitReq.baud = 19200;
	commInitReq.hwirq = 0;
	commInitReq.portnum = 0;	/* must default to 0 for internet */
	commInitReq.dialing_method = comm_INIT_DIALING_METHOD_TONE;
	commInitReq.reqLen = sizeof(commInitReq_t);

	ProcessCommandLine(argc, argv);

#ifndef UNIX
	if (!CommDLLName.fname[0]) {
		printf("No DLL was specified.\n");
		exit(1);
	}
#endif

	dpini_SetFile("gtest.ini");
	dp_enableDebugPrint(TRUE);

	if (!dontCreate) {
		printf("Creating dp...\n");
		/* Initialize the networking layer */
		DPRINT(("setting sessionId to %x for benefit of serial drivers.\n", commInitReq.sessionId));
		printf("setting sessionId to %x for benefit of serial drivers.\n", (unsigned) commInitReq.sessionId);
		err = dpCreate(&myDP, &CommDLLName, &commInitReq, NULL);
		if (err != dp_RES_OK) {
			printf("Unable to start communications");
			exit(2);
		}

		/* Specify what server to use to rendezvous with other players. */
		/* I usually use the "setgameserver" command to do this in a script. */
        if (adrString[0]) {
			if (defaultSessType)
				cur_species = defaultSessType;
#ifdef dp_MULTISESSTABLE
			err = dpSetGameServerEx(myDP, adrString, defaultSessType);
#else
			err = dpSetGameServer(myDP, adrString);
#endif
            if (err != dp_RES_OK) {
                printf("SetGameServer(%s) failed, error %d\n", adrString, err);
                exit(3);
            }
        }

		/* Games won't need to know about incoming connections,
		 * but this test program likes to print them out in case
		 * the programmer running this test program is curious.
		 */
		dpSetConnCallback(myDP, connection_cb, "<INCOMING>");
	}

	/* The modem driver and loopback drivers use the random number
	 * generator, so initialize it here.  Games should do this, too.
	 */
	srand(time(0));

	if (busyTime)
		burn10msec(1);	/* calibrate */

	/* Assert command will set this if it detects an assertion failure,
	 * so we can pass info about the assertion failure to the calling
	 * program in our exit status.
	 */
	exitCode = 0;
	raw_init();

	/* Sit there waiting for a user command.
	 */
	pleaseQuit = 0;
	while (sleep_until || !pleaseQuit) {
		dpid_t idFrom;
		dpid_t idTo;
		char nameBuf[256];
#include "dppack1.h"
		struct {
			dp_packetType_t type PACK;
			union {
				dp_user_addPlayer_packet_t addPlayer;
				dp_user_delPlayer_packet_t delPlayer;
				dp_user_addGroup_packet_t ag;
				dp_user_delGroup_packet_t dg;
				dp_addPlayerToGroup_packet_t gap;
				dp_delPlayerFromGroup_packet_t gdp;
				dp_user_playerData_packet_t pv;
				dp_objectDelta_packet_t delta;
				dp_account_packet_t acctpkt;
				dp_sessionResult_packet_t sessRes;
				unsigned char buf[512];
			} u PACK;
		} pkt;
#include "dpunpack.h"
		size_t size;
		int ch = 0;

		/* Set the time of the start of the first loop */
		if (!firstLoop)
			firstLoop = eclock();

		/* Handle /sleep  */
		if (sleep_until) {
			if ((long)(sleep_until - eclock()) < 0) {
				sleep_until = 0;
				stuffChatText("*****", "Sleep finished.");
				continue;
			}
		}
		/* Handle /wait  */
		if (waiting && callbacksFinished) {
			waiting = FALSE;
			stuffChatText("*****", "Wait finished.");
		}
		dpio_now = eclock();

		if (sleep_until || waiting) {
			;				/* don't fetch commands while waiting */
		} else if (scriptfp) {
			int i;
			if (!fgets(kbuf, sizeof(kbuf), scriptfp)) {
				printf("Reached end of script file %s\n", scriptfname);
				quit(0);
			}

			/* Strip trailing spaces from input line */
			while (*kbuf && isspace(kbuf[strlen(kbuf)-1]))
				kbuf[strlen(kbuf)-1] = 0;
			/* Strip leading spaces and \r's from input line */
			for (i=0; kbuf[i] && (isspace(kbuf[i]) || (kbuf[i]=='\r')); i++)
				;
			putchar('>'); puts(kbuf+i);		/* echo it */
			processChatCommand(kbuf+i);
		} else if (raw_kbhit()) {
			/* Accumulate keys into keyboard buffer.  Allow backspace. */
			int len = strlen(kbuf);
			ch = raw_getc();
			if (ch == -1) {
				/* guess kbhit was wrong */
				continue;
			}
			if (ch == 3)
				quit(1);
			putchar(ch);
			fflush(stdout);
			if (ch == 8 || ch == 127) {
#ifdef SPARC
				if (ch == 127) putchar('\b');
#endif
				if (len > 0)
					kbuf[len-1] = 0;
				continue;
			}
			if (ch != '\r' && ch != '\n') {
				/* Append to buffer. */
				kbuf[len++] = ch;
				kbuf[len] = 0;
				/* printf("Appending %c to buffer\n", ch); */
				continue;
			}
			/* Process the keyboard buffer. */
			if (!kbuf[0])
				continue;

			processChatCommand(kbuf);

			/* Empty it. */
			kbuf[0] = 0;
		}

		/* Simulate a redraw cycle. */
		if (busyTime) {
			burn10msec(busyTime);
			DPRINT(("---Burn %ld msec done, time %8d---\n", busyTime, eclock()));
		}

		if (!myDP
#ifdef USE_DPIO
		&& !dpio
#endif
		)
			continue;

		dpio_now = eclock();
		/* Housekeeping.  Notice players coming and going. */
		for (;;) {
			int i;

#ifdef _WIN32
			Sleep(10);	/* give other program 10 ms to run */
#endif
			if (myDP) {
				size = sizeof(pkt);
				err = dpReceive(myDP, &idFrom, &idTo, 0, &pkt, &size);
				if (err == dp_RES_EMPTY)
					break;
#ifdef USE_DPIO
			} else if (dpio) {
				int empties;
				for (empties=0; empties<3; empties++) {
					playerHdl_t h;
					size = sizeof(pkt);
					h = dpio_findTimedOutHost(dpio
#ifndef dp_ANET2
						, PLAYER_NONE
#endif
							);
					if (h != PLAYER_NONE) {
						sprintf(buf, "Player %lu has timed out.  Calling closeHdlImmed.", h);
						stuffChatText("*****", buf);
						dpio_closeHdlImmed(dpio, h);
					}
					err = dpio_update(dpio);
					if (err != dp_RES_OK && err != dp_RES_EMPTY) {
						DPRINT(("dpio_update reports error %d\n", err));
					}
					h = (dpid_t) PLAYER_NONE;	/* kludge */
					err = dpio_get(dpio, &h, &pkt, &size, NULL);
					idFrom = (dpid_t) h;		/* double kludge */

					if (err != dp_RES_AGAIN)
						break;
				}
				if (err == dp_RES_EMPTY || err == dp_RES_AGAIN)
					break;
#endif
			}

			if (err != dp_RES_OK) {
				DPRINT(("Unable to receive a packet, error: %d\n", err));
				switch (err) {
				case dp_RES_HOST_NOT_RESPONDING:
					stuffChatText("*****", "Error: host not responding!  Connection to game server lost.");
					break;
				case dp_RES_MODEM_NORESPONSE:
					stuffChatText("*****", "Error: modem not responding!  Connection lost.");
					break;
				default:
					sprintf(buf, "Unable to receive a packet, error: %d", err);
					stuffChatText("*****", buf);
				}
				continue;
			}

			switch (pkt.type) {
			case dp_USER_ADDPLAYER_PACKET_ID:
				DPRINT(("Player '%s' (id = %d = %d) added.  %d in room\n", pkt.u.addPlayer.name, idFrom, pkt.u.delPlayer.id, dpNumPlayers(myDP)));
				sprintf(buf, "%s enters; dpid = %d, karma %d, adr = ", pkt.u.addPlayer.name, pkt.u.addPlayer.id, pkt.u.addPlayer.karma);

				for (i = 0; i < 6; i++)
					sprintf(buf+strlen(buf), ".%d", 0xff&((unsigned char *)pkt.u.addPlayer.adr)[i]);
				stuffChatText("*****", buf);
				countMyPlayers(myDP);		/* see if it was a local player */
				break;

			case dp_USER_DELPLAYER_PACKET_ID:
				DPRINT(("Player '%s' (id = %d = %d) deleted.\n", pkt.u.delPlayer.name, idFrom, pkt.u.delPlayer.id));
				sprintf(buf, "%s leaves; dpid = %d.", pkt.u.delPlayer.name, pkt.u.delPlayer.id);
				stuffChatText("*****", buf);
				countMyPlayers(myDP);		/* see if it was a local player */
				break;

			case dp_USER_ADDGROUP_PACKET_ID:
				sprintf(buf, "Group %s (%d) added.", pkt.u.ag.name, pkt.u.ag.id);
				stuffChatText("*****", buf);
				break;

			case dp_USER_DELGROUP_PACKET_ID:
				sprintf(buf, "Group %s (%d) deleted.", pkt.u.dg.name, pkt.u.dg.id);
				stuffChatText("*****", buf);
				break;

			case dp_USER_ADDPLAYERTOGROUP_PACKET_ID:
				sprintf(buf, "Player %d added to group %d.", pkt.u.gap.dpIdPlayer, pkt.u.gap.dpIdGroup);
				stuffChatText("*****", buf);
				break;

			case dp_USER_DELPLAYERFROMGROUP_PACKET_ID:
				sprintf(buf, "Player %d deleted from group %d.", pkt.u.gdp.dpIdPlayer, pkt.u.gdp.dpIdGroup);
				stuffChatText("*****", buf);
				break;

			case dp_USER_PLAYERDATA_PACKET_ID:
				sprintf(buf, "New value for player %d's variable %d: len %zd, value %-20.20s",
					pkt.u.pv.id, pkt.u.pv.key, pkt.u.pv.len, (char*) pkt.u.pv.data);
				stuffChatText("*****", buf);
				break;

			case dppt_MAKE(0, 1):
#ifdef USE_DPIO
				if (dpio) sprintf(nameBuf, "%d", idFrom);
#endif
				if (myDP) err = dpGetPlayerName(myDP, idFrom, nameBuf, sizeof(nameBuf));
				{
					int i;
					int errs = 0;
					for (i=0; i<((int)size)-2; i++)
						if (pkt.u.buf[i] != i+2) {
							errs++;
							DPRINT(("i %d, buf[i] %d\n", i, pkt.u.buf[i]));
						}
					sprintf(buf, "Got %zd bytes from %s; %d errors.\n", size, nameBuf, errs);
					stuffChatText("*****", buf);
				}
				break;

			case dppt_MAKE('r', 'q'):
				stuffChatText("*****", "Remote quit received.");
				pleaseQuit = TRUE;
				sleep_until = 0;
				waiting = FALSE;
				break;

			case SPAM_PACKET_ID:
				if (myDP) err = dpGetPlayerName(myDP, idFrom, nameBuf, sizeof(nameBuf));
				{
					int i;
					int errs = 0;
					for (i=0; i<((int)size)-2; i++)
						if (pkt.u.buf[i] != i+2) {
							errs++;
							DPRINT(("i %d, buf[i] %d\n", i, pkt.u.buf[i]));
						}
				}
				/* Increment the spam counters, and set the spam time */
				spams_received++;
				spam_recsz += size;
				if (!frst_recSpam)
					frst_recSpam = eclock();
				break;

			case CHAT_PACKET_ID:
				err = dpGetPlayerName(myDP, idFrom, nameBuf, sizeof(nameBuf));
				pkt.u.buf[size-sizeof(dp_packetType_t)] = 0;	/* Null terminate */
				stuffChatText(nameBuf, (char *) pkt.u.buf);
				break;

			case dp_USER_HOST_PACKET_ID:
				stuffChatText("*****", "This machine is now host.");
				break;

			case dp_SESSIONLOST_PACKET_ID:
				stuffChatText("*****", "The session has been closed for lack of a suitable host.");
				break;

			case dp_OBJECTDELTA_PACKET_ID:
				{
					char bigBuf[2048];
					dp_objectDelta2string(&pkt.u.delta, bigBuf);
					stuffChatText("*****", bigBuf);
				}
				break;

			case dp_ACCOUNT_PACKET_ID:
				switch (pkt.u.acctpkt.reason) {
				case dp_RES_OK:       /* logged in, account is activated */
					myUID = pkt.u.acctpkt.uid;
					assert(myUID != dp_UID_NONE);
					sprintf(buf, "Logged as uid %lu. (account activated)\n", myUID);
					stuffChatText("*****", buf);
					break;
				case dp_RES_NOTYET:   /* logged in, account is not activated */
					myUID = pkt.u.acctpkt.uid;
					assert(myUID != dp_UID_NONE);
					sprintf(buf, "Logged as uid %lu. You must activate this account now.\n", myUID);
					stuffChatText("*****", buf);
					break;
				case dp_RES_CHANGED:  /* acctchange succeeded */
					myUID = pkt.u.acctpkt.uid;
					assert(myUID != dp_UID_NONE);
					sprintf(buf, "Account change successful for uid %lu.", myUID);
					stuffChatText("*****", buf);
					break;
				case dp_RES_ACCESS:   /* failed, name/password incorrect */
					/* leave uid alone, since this may be a response to a
					 * password change attempt (failure != logout)
					 */
					stuffChatText("*****", "Access Denied.");
					break;
				case dp_RES_ALREADY:  /* failed, account is in use */
					/* definitely not logged in */
					myUID = dp_UID_NONE;
					stuffChatText("*****", "Access Denied, account is in use.");
					break;
				default:              /* failed, bug */
					/* leave uid alone, since this may be a response to a
					 * password change attempt (failure != logout)
					 */
					sprintf(buf, "Account request failed, reason %d\n", pkt.u.acctpkt.reason);
					stuffChatText("*****", buf);
					break;
				}
				break;

			case dp_SESSIONRESULT_PACKET_ID:
				if (pkt.u.sessRes.reason != dp_RES_OK) {
					sprintf(buf, "Session join failed, reason:%d\n", pkt.u.sessRes.reason);
					stuffChatText("*****", buf);
				} else {
					sprintf(buf, "Joined session %s, type:%d\n", pkt.u.sessRes.sess.sessionName, pkt.u.sessRes.sess.sessionType);
					stuffChatText("*****", buf);
				}
				break;

			default:
				sprintf(buf, "Odd; got unexpected packet %2.2s, size %zd.\n", (char*) &pkt.type, size);
				stuffChatText("*****", buf);
			}
		}

		if(turn_status & (TURN_SEND | TURN_RECEIVE)) {
			process_turn();
		}
		/* Spam any hosts that deserve it */
		if ((long)(nextSpam - eclock()) < 0) {
			spam_packets (thisPlayer, spam_To, spam_pktsz);
			nextSpam += spam_time;
			if ((long)(nextSpam - eclock()) < spam_time / 2)
				nextSpam = eclock() + spam_time / 2;
		}

		/* Increment the loop counter */
		loops_done++;
	}

	return exitCode;	/* to make compiler happy */
}
