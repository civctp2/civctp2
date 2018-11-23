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

/*#define WEBLAUNCH*/
/*--------------------------------------------------------------------------
 $Log: server.c $
 Revision 1.26  1997/09/05 18:04:53  dkegel
 No longer default to servers.txt; require commandline parameter to
 set server list filename.
 Revision 1.25  1997/09/04 20:09:40  lbennett
 Fixed L1 build, removed warnings.
 Revision 1.24  1997/09/04 06:47:07  dkegel
 1. Support dptab_requestSubscription().
 2. Use it to transfer a table of game servers from the server we're
 connected to, if any.  Crude, but it's better than nothing.
 Revision 1.23  1997/09/03 20:06:08  dkegel
 Fixed -p parsing.
 Don't set socket option under win32 (not in dll).
 Revision 1.22  1997/09/02 23:50:24  dkegel
 Sets keepalive on incoming connections.
 Revision 1.21  1997/09/02 23:00:17  dkegel
 Let port number be set from the command line.
 Revision 1.20  1997/08/28 22:40:45  dkegel
 Oops- broke v1 build
 Revision 1.19  1997/08/28 18:08:58  dkegel
 Now support reading a list of rooms in from a file named by -r
 if -s is not given.
 Revision 1.18  1997/08/22 17:07:03  dkegel
 Initial v2 support.
 Revision 1.17  1997/08/07 01:06:01  lbennett
 Added no-migrate flag to server lobby, corrected warnings.
 Revision 1.16  1997/07/25 21:29:55  lbennett
 Fixed naming convention violation.
 Revision 1.15  1997/07/25 21:17:12  lbennett
 Added command-line option for session type (-S)
 Revision 1.14  1997/06/26 17:59:15  dkegel
 Initialize the session record, stupid!
 Revision 1.13  1997/06/13 23:20:38  geychaner
 Changed incoming callback to generic callback in dp
 Revision 1.12  1997/05/28 17:40:49  anitalee
 switched to eclock() for portability; see eclock.h.
 Revision 1.11  1997/04/17 20:25:10  geychaner
 Really incredibly obviously dumb malloc bug in reading HTML file fixed.
 Revision 1.10  1997/04/16 01:03:05  geychaner
 BIG CHANGES since last version:
 1. Greatly nexpanded HTML file capabilities.  Now reads template file
    into memory at startup.  More user-defined tags, for current time,
    server start time, games played.
 2. Server now counts number of games played, which required an overhaul
    of the session enumerator.
 3. Server now freezes at shutdown and thaws at startup, allowing for
    cleaner server performance from user perspective.
 Revision 1.9  1997/04/03 00:29:10  geychaner
 Minor HTML formatting changes to the tables of games
 Revision 1.8  1997/03/19 03:08:20  geychaner
 1. Added some prints to the stdout server log file in addition to DPRINTS
 2. Server now can create a web page of active sessions!
 Revision 1.7  1997/03/15 05:06:49  geychaner
 Fixed a bunch of dprints in the server
 Revision 1.6  1997/03/15 04:50:36  geychaner
 Added the incoming connections callback and ability to specify dp.ini file.
 Revision 1.5  1997/03/07 23:00:26  geychaner
 Added definitions of TRUE/FALSE
 Revision 1.4  1997/03/07 22:49:21  geychaner
 Updated .h file includes and joinsess_cb function args
 Revision 1.3  1997/02/04 04:21:15  dkegel
 Compiles on Linux now...
 Revision 1.2  1997/01/02 19:24:00  dkegel
 Use new dpDeclareLobby() call to turn on internet game server code in dp.
 Revision 1.1  1996/12/16 05:12:57  dkegel
 Initial revision

 Demo server for ActiveNet.
--------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <io.h>
#endif

#ifdef UNIX
#include <unistd.h>
#endif

#ifdef dp_ANET2
#include "dp.h"
#include "portable.h"
#include "dpexcept.h"
#else
#include "commapi.h"
#include "anet.h"
#endif
#include "eclock.h"

#include "dpprivy.h"

#ifdef UNIX
#define HTMLEXT ".html"
#else
#define HTMLEXT ".html"
#endif

#ifdef dp_STATS
#include "avgstat.h"
dp_avgstat_t avgstat;
#endif
#define GS_SLICE_PERHOUR 4 /* update GamesServedSlice every 15 min. */
#define GSMAX GS_SLICE_PERHOUR * 24

long GS_BEACON_INTERVAL()
{
	static long hertz = 0;

	if(!hertz)
		hertz = ECLOCKS_PER_SEC;

	if(hertz > GS_SLICE_PERHOUR)
		return (3600 * (ECLOCKS_PER_SEC / GS_SLICE_PERHOUR));
	else
		return (3600 / (GS_SLICE_PERHOUR / ECLOCKS_PER_SEC));

/*
#if ECLOCKS_PER_SEC > GS_SLICE_PERHOUR
#define GS_BEACON_INTERVAL 3600 * (ECLOCKS_PER_SEC / GS_SLICE_PERHOUR)
#else
#define GS_BEACON_INTERVAL 3600 / (GS_SLICE_PERHOUR / ECLOCKS_PER_SEC)
#endif
*/
}

/* number of games currently active indexed by session type*/
assoctab_t *GamesActive = NULL;
/* number of games served since uptime indexed by session type*/
assoctab_t *GamesServed = NULL;
/* assoctab indexed by session type of integer array containing number of games
 * served each GS_BEACON_INTERVAL for past 24 hrs. */
assoctab_t *GamesServedSlice;
static int gsindex = 0;
time_t StartTime;

#define UUDPS_SOCKET_DEFAULT (0x52A1+commapi_NET_PORTOFFSET+1)
#define UUDP_SOCKET_DEFAULT	(0x52A1+commapi_NET_PORTOFFSET)

#define SBDSERV_PORT 21157

/* Stuff passed in command line options */
char CommDLLName[128] = "";
char RoomName[128] = "";
char IniFile[128] = "";
char FreezeFile[128] = "";
char FreezeFile2[128] = "";
char HostsFile[128] = "";
char AppsFile[128] = "";
char AtvilogFile[128] = "";
char WMQDirectory[128] = "";
dp_species_t SessionType = DP_ILLEGAL_SPECIES;
int Portnum = UUDPS_SOCKET_DEFAULT;
int MaxPlayerHdls = 400;

#define MERCNET_SESSIONTYPE ((dp_species_t) 1005+5)	/* must be same as in sstate.h */

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* We want to enumerate the sessions every 60 seconds for a web page */
#define HTTP_UPDATE_SECS	 60
#define ENUM_BEACON_INTERVAL HTTP_UPDATE_SECS * ECLOCKS_PER_SEC

/* sdbserv likes to be polled every 10 secs */
#define SBDSERV_BEACON_INTERVAL	(10 * ECLOCKS_PER_SEC)

#define SESSION_DELETE 2	/* Number of enumSessions after which to delete a session */

/* Kludge: this should be dynamic e.g. an hkeytab */
#define SERVER_MAXSESSIONS 1024
struct {
	dp_session_t s;			/* The session structure */
	short notseen;			/* Number of enumSessions which this session hasn't been seen */
	int max_activePlayers;	/* The max players seen in this session */
} sessions[SERVER_MAXSESSIONS];	/* Extra room for dead sessions */
int n_sessions;

/* info sent to pinging hosts */
static commSessInfo_t sessinfo;

/* Type to represent a template /output file pair */
typedef struct {
	char ofname[128]; 		/* name of output html file */
	char *template;			/* body of input template file, nul terminated */
} page_t;

/* Type to save room options read from room file */
typedef struct {
	int bEnablePlayerVars;
	int maxPlayers;
} ROOM_OPTS;

int LobbyMaxPlayers = dp_MAXREALPLAYERS;  /* Size of lobbies, default = max */

#define MAX_PAGES 90
page_t pages[MAX_PAGES];	/* status page we periodically refresh */
int npages = 0;				/* number of elements of pages[] used so far */

int dpiofd = -1;			/* dpio socket file descriptor */

int pleaseQuit = FALSE;	/* Signal the main loop to quit */
int pleaseUpdate = FALSE;	/* Signal the main loop to update */

char Hostname[128];		/* Hostname of this host */

#include "servscor.h"
#include "sbdserv.h"
#ifdef WEBLAUNCH
#include "bhttp.h"
#endif

/*----------------------------------------------------------------------
 Debugging routine.
----------------------------------------------------------------------*/
#if 1
static void printAdr(
	int adrLen,
	const unsigned char adr[dp_MAX_ADR_LEN])
{
	char buf[256];
	int i;

	buf[0] = 0;
	for (i=0; i<adrLen-1; i++)
		sprintf(buf + 3*i, "%02x:", adr[i]);
	sprintf(buf + 3*i, "%02x", adr[i]);
	DPRINT(("%s", buf));
	(void) adr;
}
#else
#define printAdr(adrLen, adr)
#endif

#if 1
#if 0 /* key2* are defined in dpprivy.h */
/* Convert a key to ASCII for debug printing */
static char *key2buf(const char *key, int keylen, char *buf)
{
	int i;

	if (keylen > hkeytab_MAXLEN)
		return "key too long";

	buf[0] = 0;
	for (i=0; i<keylen; i++) {
		/* DPRINT(("key[%d] %d\n", i, key[i])); */
		sprintf(buf + strlen(buf), "%d%s", 255 & key[i], (i == (keylen-1)) ? "" : ".");
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
#endif

static void dprint_sess(dp_t *dp, const dp_session_t *sess, char *msg)
{
	DPRINT(("%s: sessType %d, name %s, k%d u%x f%x, table %s, key %s, adr ",
			msg,
			sess->sessionType, sess->sessionName,
			sess->karma, sess->dwUser1, sess->flags,
			key2a(dp->sessions->key, dp->sessions->keylen),
			key2a2(sess->reserved2, dp->dpio->myAdrLen+2)
			));
	printAdr(dp->dpio->myAdrLen, sess->adrMaster);
	DPRINT(("\n"));

}

#else
#if 0 /* key2* are defined in dpprivy.h */
#define key2a(key, keylen) "ndbug"
#define key2a2(key, keylen) "ndbug"
#define key2a3(key, keylen) "ndbug"
#endif
#define dprint_sess(dp, sess, msg)
#endif

/*-------------------------------------------------------------------------
 Callback function for status dump routines.
 We use it to output to a webpage.
-------------------------------------------------------------------------*/
void statprint_fn(const char *buf, void *context)
{
	FILE *fp = (FILE *)context;
	fputs(buf, fp);
	putc('\n', fp);
}

/*-------------------------------------------------------------------------
 Tell the main program to quit; used as a signal handler
-------------------------------------------------------------------------*/
void exitNow (int signal)
{
	pleaseQuit = TRUE;
}

/*-------------------------------------------------------------------------
 Tell the main program to update tables; used as a signal handler
-------------------------------------------------------------------------*/
void updateNow (int signal)
{
	pleaseUpdate = TRUE;
}

/*-------------------------------------------------------------------------
 Search for special HTML tags
 Returns the remainder of the input string (if any);
 places a pointer to the tag name in tagname and the tag value in arg.
-------------------------------------------------------------------------*/
char *FindTag(const char *inp, FILE *out, char **tagname, int *arg)
{
	char *mark;

	/* Search for the secret mark to go on to the bonus round */
	mark = strchr(inp, '{');
	if (mark) {
		int wrote, leftlen;

		/* Output up to the mark */
		leftlen = mark - inp;
		if (out) {
			wrote = fwrite(inp, 1, leftlen, out);
			if (wrote != leftlen)
				printf("Warning: output %d of %d bytes to HTML file\n", wrote, leftlen);
		}

		/* Move up to the beginning of the tag name */
		*tagname = mark + 1;
		/* Find the end of the tag name */
		mark = strpbrk(*tagname, "} ");

		if (mark[0] == ' ') {
			/* This tag has a value; scan it */
			if ((arg != NULL)
			&&  (1 != sscanf(mark, " %d", arg)))
				printf("Warning: could not find value for tag %8.8s.\n", *tagname);
			/* Find the end of tag */
			mark = strchr(mark, '}');
		}
		/* Return the unused portion for reexamination */
		return (mark ? ++mark : NULL);
	}
	/* Could not find the secret word! */
	return NULL;
}

#define tagcompare(x, y) !strnicmp(x, y, strlen(y))

/* get the next line from input string */
static char *sgets(char *s, int n, const char *inp)
{
	int ncpy = 0;
	char *ptr;
	memset(s, 0, n);
	if ((ptr = strchr(inp, '\n')))
		ncpy = (ptr - inp) + 1 < n - 1 ? (ptr - inp) + 1 : n - 1;
	else
		ncpy = strlen(inp) < n - 1 ? strlen(inp) : n - 1;
	if (ncpy <= 0) return NULL;
	memcpy(s, inp, ncpy);
	s[ncpy] = '\0';
	return s;
}

void makeSessionTableCell(dp_session_t *s, FILE *out)
{
	fprintf(out, "<TD ALIGN=CENTER>\n");
#ifdef WEBLAUNCH
	{
		unsigned char *id = s->reserved2;
		char idstr[64];

		sprintf(idstr, "%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x",
			id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);

		fprintf(out, "\
<A HREF=\"http://%s:%d/s/%s.anet%d\"><IMG ALIGN=LEFT SRC=/join.gif  ALT=Join></A>\
<A HREF=\"http://%s:%d/d/%s.anet%d\"><IMG ALIGN=LEFT SRC=/djoin.gif ALT=DJoin></A>\n",
		Hostname, 8080, idstr, s->sessionType,
		Hostname, 8080, idstr, s->sessionType);
	}
#endif
	fprintf(out, "%s</TD><TD ALIGN=CENTER>%d/%d</TD>\n",
		s->sessionName, s->currentPlayers,
		s->maxPlayers);
}

#define MAX_LINE 1024
#define OUTPUT_MIN_GAMES 1

/*-------------------------------------------------------------------------
 Function for creating the session web page
-------------------------------------------------------------------------*/
void MakeSessionWebPage(dp_t *dp, const char *HTML, FILE *out)
{
	const char	*wday[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"},
				*mmm[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
							"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	char *scan, *tagname, *line;
	char linebuf[MAX_LINE];
	int bSkipLine;
	int value;

    /* get the next line */
	while ((sgets(linebuf, MAX_LINE, HTML))) {
	  int *gameserv = NULL;
	  int gameservcount = 0;
	  HTML += strlen(linebuf);
	  /* Ignore this line if printing GAMESDAY and there are no current
	   *  games, or games served in past 24 hrs. is too few.
	   *  ***WARNING*** assumption is made that the GAMESDAY keyword
	   *  occurs at most once per line of the template file.
	   * Note: you can force print of games served in past 24 hrs.
	   *  regardless of its value by using tag PRINTGAMESDAY.
	   */
	  bSkipLine = 0;
	  line = linebuf;
	  while (scan = FindTag(line, NULL, &tagname, &value)) {
		if (tagcompare(tagname, "GAMESDAY")) {
			int *count = (GamesActive ? assoctab_subscript(
					GamesActive, value) : NULL);
			/*DPRINT(("SERVER: MakeSessionWebPage: GamesActive %p count %p *count %d sessType %d\n", GamesActive, count, (count ? *count : 0), value));*/
			if (!count || !*count) {
				int i;
				gameserv = (int *)assoctab_subscript(GamesServedSlice, value);
				if (gameserv) {
					for (gameservcount = 0, i = 0; i < GSMAX; i++)
						gameservcount += gameserv[i];
					if (gameservcount < OUTPUT_MIN_GAMES)
						bSkipLine = 1;
					if (gameservcount == 0)
						assoctab_subscript_delete(GamesServedSlice, value);
				} else
					bSkipLine = 1;
			}
			break;
		}
		line = scan;
	  }
	  if (bSkipLine)
		continue;
	  /* Search for the {tagname value} special tags and output line to file */
	  line = linebuf;
	  while (scan = FindTag(line, out, &tagname, &value)) {
		/* Found a tag; figure out what it is */
		if (tagcompare(tagname, "SPECIES")) {
			/* Found SPECIES n, so output a table of matching sessions */
			int i, nsessions = 0;
			short ngames = 0;
			if (value > 0 && value < 65536)
				ngames = sessinfo.games[value];
			for (i = 0; i < n_sessions; i++) {
				if (!sessions[i].notseen
				&& (value == sessions[i].s.sessionType))
					nsessions++;
			}

			if (ngames) {
				int trow = 1;

				/* Create a 2-column table of current games */
				fputs("<TABLE BORDER=1 WIDTH=100%>\n", out);
				fprintf(out, "<CAPTION ALIGN=TOP>%d games currently in progress</CAPTION>\n", ngames);
				fputs("<TR><TH WIDTH=50%>Game Name</TH><TH>Players/Max</TH><TH WIDTH=50%>Game Name</TH><TH>Players/Max</TH></TR>\n", out);
				for (i = 0; i < n_sessions; i++) {
					if (!sessions[i].notseen
					&& (value == sessions[i].s.sessionType)) {
						if (sessions[i].s.flags & dp_SESSION_FLAGS_ISLOBBY)
							continue;
						/* Matching game; output it */
						if (trow) fputs("<TR>", out);
						makeSessionTableCell(&sessions[i].s, out);
						if (!trow) fputs("</TR>", out);
						trow = !trow;
					}
				}
				if (!trow) fputs("</TR>", out);
				fputs("</TABLE>\n", out);
			} else {
				fputs("<BR>No games currently in progress.<P>", out);
			}
			if (nsessions > ngames) {
				int trow = 1;

				/* Create a 2-column table of current lobbies */
				if (ngames)
					fputs("<BR><BR>\n", out);
				fputs("<TABLE BORDER=1 WIDTH=100%>\n", out);
				fputs("<TR><TH WIDTH=50%>Lobby Name</TH><TH>Players/Max</TH><TH WIDTH=50%>Lobby Name</TH><TH>Players/Max</TH></TR>\n", out);
				for (i = 0; i < n_sessions; i++) {
					if (!sessions[i].notseen
					&&  (value == sessions[i].s.sessionType)) {
						if (!(sessions[i].s.flags & dp_SESSION_FLAGS_ISLOBBY))
							continue;
						/* Matching lobby; output it */
						if (trow) fputs("<TR>", out);
						makeSessionTableCell(&sessions[i].s, out);
						if (!trow) fputs("</TR>", out);
						trow = !trow;
					}
				}
				if (!trow) fputs("</TR>", out);
				fputs("</TABLE>\n", out);
			}
		} else if (tagcompare(tagname, "TIME")) {
			/* Output the current time in local time */
			time_t now = time(NULL);
			struct tm *btime = localtime(&now);
			int hour = btime->tm_hour % 12;
			if (!hour) hour = 12;

			fprintf(out, "%s, %d %s %4d %d:%02d:%02d %s %s",
				wday[btime->tm_wday], btime->tm_mday, mmm[btime->tm_mon],
				btime->tm_year+1900, hour, btime->tm_min, btime->tm_sec,
				((btime->tm_hour / 12) ? "pm" : "am"), tzname[btime->tm_isdst > 0]);
		} else if (tagcompare(tagname, "UPTIME")) {
			/* Output the gameserver start time in local time */
			time_t now = StartTime;
			struct tm *btime = localtime(&now);
			int hour = btime->tm_hour % 12;
			if (!hour) hour = 12;

			fprintf(out, "%s, %d %s %4d %d:%02d:%02d %s %s",
				wday[btime->tm_wday], btime->tm_mday, mmm[btime->tm_mon],
				btime->tm_year+1900, hour, btime->tm_min, btime->tm_sec,
				((btime->tm_hour / 12) ? "pm" : "am"), tzname[btime->tm_isdst > 0]);
		} else if (tagcompare(tagname, "EXPTIME")) {
			/* Output the HTTP page expire time in GMT */
			time_t now = time(NULL) + HTTP_UPDATE_SECS;
			struct tm *btime = gmtime(&now);

			fprintf(out, "%s, %02d %s %04d %02d:%02d:%02d GMT",
				wday[btime->tm_wday], btime->tm_mday, mmm[btime->tm_mon],
				btime->tm_year+1900, btime->tm_hour, btime->tm_min, btime->tm_sec);
		} else if (tagcompare(tagname, "GAMESDAY")) {
			/* Use the value calculated when figuring whether to skip line */
			if (gameservcount == 0) {
				printf("bug: gameservcount = 0 in GAMESDAY\n");
			}
			fprintf(out, "%d", gameservcount);
		} else if (tagcompare(tagname, "PRINTGAMESDAY")) {
			int i, count = 0;
			int *gameserv = (int *)assoctab_subscript(GamesServedSlice, value);
			if (gameserv) {
				for (i = 0; i < GSMAX; i++)
				 	count += gameserv[i];
				if (count == 0)
					assoctab_subscript_delete(GamesServedSlice, value);
			}
			fprintf(out, "%d", count);
		} else if (tagcompare(tagname, "GAMES")) {
			/* Output the gameserver games served counter */
			int *pcount = assoctab_subscript(GamesServed, value);
			int count = pcount ? *pcount : 0;

			fprintf(out, "%d", count);
		} else if (tagcompare(tagname, "CURGAMES")) {
			short n_games = 0;
			if (value > 0 && value < 65536)
				n_games = sessinfo.games[value];
			fprintf(out, "%d", n_games);
		} else if (tagcompare(tagname, "CURPLAYERS")) {
			short n_players = 0;
			if (value > 0 && value < 65536)
				n_players = sessinfo.players[value];
			fprintf(out, "%hd", n_players);
		} else if (tagcompare(tagname, "UPDATE")) {
			/* Output the time between HTML updates */
			fprintf(out, "%d", HTTP_UPDATE_SECS);
		} else if (tagcompare(tagname, "HOSTNAME")) {
			/* Output the hostname */
			fputs(Hostname, out);
#ifdef dp_STATS
		} else if (tagcompare(tagname, "STATS")) {
			/* Output the network statistics */
			dp_stats_t stats1;
			dp_stats_t stats60;
			dp_stats_t max1;
			dp_stats_t max60;

			dp_avgstat_poll(dp, &avgstat, &stats1, &stats60, &max1, &max60);

			/* kludge: output to stdout, too */
#if 0
			statprint_fn("average for last minute", stdout);
			dp_avgstat_print(dp, &stats1, statprint_fn, stdout);
			statprint_fn("worst minute so far", stdout);
			dp_avgstat_print(dp, &max1, statprint_fn, stdout);
#endif

			/* output to the html file */
			statprint_fn("average for last minute", out);
			dp_avgstat_print(dp, &stats1, statprint_fn, out);
			statprint_fn("average for last hour (units per minute)", out);
			dp_avgstat_print(dp, &stats60, statprint_fn, out);
			statprint_fn("worst minute so far", out);
			dp_avgstat_print(dp, &max1, statprint_fn, out);
			statprint_fn("worst hour so far (units per minute)", out);
			dp_avgstat_print(dp, &max60, statprint_fn, out);
		}
#endif
		/* Wrap around and search again */
		line = scan;
	  }
	  /* Couldn't find the tag; output the leftover line */
	  fputs(line, out);
	}
}

/*-------------------------------------------------------------------------
 Callback triggered by listing sessions.
-------------------------------------------------------------------------*/
int dp_PASCAL listSessions_cb(dp_session_t *sDesc, long *pTimeout, long flags, void *context)
{
	dp_t *dp = (dp_t *) context;
	int i;

	if (!sDesc) {
		FILE *outHtml;
		int act_sessions = 0;

		/* First, delete any session that have timed out for too long */
		for (i=n_sessions-1; i>=0; i--) {
			if (sessions[i].notseen > SESSION_DELETE) {
				printf("Deleted session %s, k:%d, maxPlay %d, notseen %d\n",
					sessions[i].s.sessionName, sessions[i].s.karma,
					sessions[i].max_activePlayers, sessions[i].notseen);
				DPRINT(("SERVER: Deleted session %s, k:%d, maxPlay %d, notseen %d\n",
					sessions[i].s.sessionName, sessions[i].s.karma,
					sessions[i].max_activePlayers, sessions[i].notseen));
				/* See if this was a completed game */
				if (sessions[i].max_activePlayers > 1) {
					/* FIXME: Shouldn't be including chat rooms in game count */
					int *count = assoctab_subscript_grow(GamesServed,
							sessions[i].s.sessionType);
					if (count) {
						(*count)++;
						DPRINT(("SERVER: Count for type %d now %d\n",
							sessions[i].s.sessionType, *count));
					} else {
						DPRINT(("SERVER: bug: can't grow GamesServed\n"));
					}
					if (!(sessions[i].s.flags & dp_SESSION_FLAGS_ISLOBBY)) {
						int *countslice = assoctab_subscript_grow(
								GamesServedSlice, sessions[i].s.sessionType);
						if (countslice) {
							countslice[gsindex]++;
						} else {
							DPRINT(("SERVER: bug: can't grow GamesServedSlice\n"));
						}
					}
				}
				/* Remove the session from the list */
				memmove(sessions+i, sessions+i+1,
						(n_sessions-1-i) * sizeof(sessions[0]));
				n_sessions--;
			} else if (!sessions[i].notseen) {
				act_sessions++;

				/* Increment counts for ping info */
				sessinfo.players[sessions[i].s.sessionType] += sessions[i].s.currentPlayers;
				sessinfo.cur_players += sessions[i].s.currentPlayers;
				if (!(sessions[i].s.flags & dp_SESSION_FLAGS_ISLOBBY)) {
					sessinfo.cur_games++;
					sessinfo.games[sessions[i].s.sessionType]++;
				}
			}
		}
		/* End of session list; we want to create the web page */
#if 0
		printf("End of session list; %d active, %d in list.\n", act_sessions, n_sessions);
#endif
		DPRINT(("SERVER: End of session list; %d active, %d in list.\n", act_sessions, n_sessions));

		for (i = 0; i < npages; i++) {
			outHtml = fopen(pages[i].ofname, "w");
			if (!outHtml) {
				printf("Error %d opening web page output file %s\n", errno, pages[i].ofname);
				DPRINT(("SERVER: Error %d opening web page output file %s\n", errno, pages[i].ofname));
				return FALSE;
			}
			MakeSessionWebPage(dp, pages[i].template, outHtml);
			fclose(outHtml);
		}
		return FALSE;
	}

#if 0
	printf("Session (%04x; %d/%d players; species %d (%x)) name %s\n",
		sDesc->karma, sDesc->currentPlayers, sDesc->maxPlayers,
		sDesc->sessionType, sDesc->sessionType, sDesc->sessionName);
#endif
	DPRINT(("SERVER: Session (%04x; %d/%d players; species %d (%x)) name %s\n",
		sDesc->karma, sDesc->currentPlayers, sDesc->maxPlayers,
		sDesc->sessionType, sDesc->sessionType, sDesc->sessionName));

	/* Update instantaneous count of games of this type */
	if (!(sDesc->flags & dp_SESSION_FLAGS_ISLOBBY)) {
		int *count = assoctab_subscript_grow(
				GamesActive, sDesc->sessionType);
		if (count)
			count[0]++;
		/*DPRINT(("SERVER: listSessions_cb: GamesActive{%d}=%d\n", sDesc->sessionType, count[0]));*/
	}

	/* Check to see if this is a known session */
	for (i=0; i<n_sessions; i++) {
		if (sessions[i].s.karma == sDesc->karma) {
			sessions[i].s = *sDesc;
			/* Reset the server session data */
			sessions[i].notseen = 0;
			if (sessions[i].max_activePlayers < sDesc->currentPlayers)
				sessions[i].max_activePlayers = sDesc->currentPlayers;
			return TRUE;
		}
	}

	/* New session; add it to the list */
	if (n_sessions < SERVER_MAXSESSIONS) {
		sessions[n_sessions].notseen = 0;
		sessions[n_sessions].max_activePlayers = sDesc->currentPlayers;
		sessions[n_sessions++].s = *sDesc;
	}
	return TRUE;
}

#ifdef dp_ANET2

#ifdef dp_MULTISESSTABLE
/*----------------------------------------------------------------------
 Callback for addition/deletion of session entries.
 Copies session entries into session-specific session tables.
 Should also repack them in new, more efficient format.
----------------------------------------------------------------------*/
int dp_PASCAL sessions_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t status)
{
	dp_t *dp = (dp_t *)context;
	dptab_table_t *tab;
	char key[dptab_KEY_MAXLEN];
	dp_session_t sess;
	dp_result_t err;

	DPRINT(("sessions_cb: status %d total %d\n", status, total));
	if (!dp || !buf || ((status != dp_RES_CREATED) &&
		(status != dp_RES_CHANGED) && (status != dp_RES_DELETED)))
		return 0;

	/* Must unpack when retrieving records from dptab */
	dp_unpack_session(dp, subkey, subkeylen, buf, total, &sess);
	dprint_sess(dp, &sess, "SERVER: sessions_cb");

	/* Locate (and possibly create) the session-specific table */
	key[0] = dp_KEY_SESSIONS;
	key[1] = dpGETSHORT_FIRSTBYTE(sess.sessionType);
	key[2] = dpGETSHORT_SECONDBYTE(sess.sessionType);
	err = dptab_createTable(dp->dt, &tab, key, 3, 0, NULL,NULL,NULL,NULL);
	if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
		printf("Can't locate session table\n");
		exit(1);
	}

	/* Propagate the change to the subtable */
	if (status == dp_RES_CREATED || status == dp_RES_CHANGED) {
		size_t sessbuflen;
		char sessbuf[2*sizeof(dp_session_t)];
		/* Repack the entry.  Note that sessionType is now part of the
		 * table key, so it doesn't need to be stored in the entry anymore.
		 */
		sessbuflen = dp_pack_session(dp, sess.sessionType, &sess, sessbuf);
		dptab_set(dptab, tab, subkey, subkeylen, sessbuf, sessbuflen, 1, PLAYER_ME);
	} else if (status == dp_RES_DELETED)
		dptab_delete(dptab, tab, subkey, subkeylen);

	return 0;
}

#endif

/*----------------------------------------------------------------------
 Callback for addition/deletion of mysession entries.
 Use this on a standalone server to copy the mysession table into the
 session table.
 Eventually, we might use this opportunity to modify the session record
 on the fly...
----------------------------------------------------------------------*/
int dp_PASCAL mysessions_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t err)
{
	dp_t *dp = (dp_t *)context;

	DPRINT(("mysessions_cb: err %d\n", err));
	if (!dp)
		return 0;

	if (err == dp_RES_CREATED || err == dp_RES_CHANGED)
		dptab_set(dptab, dp->sessions, subkey, subkeylen, buf, total, 2, PLAYER_ME);
	else if (err == dp_RES_DELETED)
		dptab_delete(dptab, dp->sessions, subkey, subkeylen);

	return 0;
}

#if 0	/* now in dp */
/*-------------------------------------------------------------------------
 When a new client comes online,
 send it our SESSIONS table,
 and when it sends us its MYSESSIONS table, put it in our MYSESSIONS table.
 (If we're the master gameserver, dump it into the sessions table
 instead.)
-------------------------------------------------------------------------*/
void addClient(dp_t *dp, playerHdl_t src, char *body, size_t len)
{
	dp_result_t err;
	dptab_table_t *target;

	target = dp->sessions; /* dp->sessions on slave srvr */

	DPRINT(("addClient: h:%x; sucking his MYSESSIONS into my sessions table\n",
			src));

	err = dptab_addPeer(dp->dt, src);
	if (err != dp_RES_OK) {
		DPRINT(("addClient: can't addPeer, err:%d\n", err));
		return;
	}

	/* Send our session table to the new client. */
	if (len == 0) {
		/* Original case: subscribe to master session table */
		err = dptab_addSubscriber(dp->dt, dp->sessions, src);
		if (err != dp_RES_OK) {
			DPRINT(("addClient: can't addSubscriber, err:%d\n", err));
			return;
		}
	} else if (len == 2) {
		/* New case: subscribe to a particular session table */
		char key[3];
		dptab_table_t *tab;
		dp_species_t sessionType = dpMAKESHORT(body[0], body[1]);
		/* Locate (and possibly create) the session-specific table */
		key[0] = dp_KEY_SESSIONS;
		key[1] = dpGETSHORT_FIRSTBYTE(sessionType);
		key[2] = dpGETSHORT_SECONDBYTE(sessionType);
		err = dptab_createTable(dp->dt, &tab, key, 3, 0, NULL,NULL,NULL,NULL);
		if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
			printf("Can't locate session table\n");
			exit(1);
		}
		err = dptab_addSubscriber(dp->dt, tab, src);
		if (err != dp_RES_OK) {
			DPRINT(("addClient: can't addSubscriber, err:%d\n", err));
			return;
		}
	} else {
		printf("addClient: bad login packet len %d\n", len);
		DPRINT(("addClient: bad login packet len %d\n", len));
	}
	/* Accept the new client's mysessions table, dump it in to
	 * the 'target' table (different for master and slave servers)
	 */
	err = dptab_addPublisher(dp->dt, target,
			dp->mysessions->key, dp->mysessions->keylen, src);
	if (err != dp_RES_OK) {
		DPRINT(("addClient: can't addPublisher, err:%d\n", err));
		return;
	}
	err = dp_subscribeExceptions(dp->dt, src);
	if (err != dp_RES_OK) {
		DPRINT(("addClient: can't subscribeExceptions, err:%d\n", err));
		return;
	}
}
#endif

/*--------------------------------------------------------------------------
 Parse the string from room file, putting the options into roomopts
  and returning the actual room name.
--------------------------------------------------------------------------*/
char *parseRoomStr(char *c, ROOM_OPTS *roomopts)
{
	char *p, buf[256];
	memset(roomopts, 0, sizeof(*roomopts));
	for ( ; *c != '\0'; c++) {
		if ((*c != ' ') && (*c != '\t')) {
			if (*c == '-') {
				switch (*(c + 1)) {
					case 'v':
						if ((p = strchr(c, ' '))) {
							memcpy(&buf[0], c + 3, (p - c) - 3);
							buf[(p - c) - 3] = '\0';
							roomopts->bEnablePlayerVars = atoi(buf);
							c = p;
						} else
							return NULL;
						break;
					case 'l':
						if ((p = strchr(c, ' '))) {
							memcpy(&buf[0], c + 3, (p - c) - 3);
							buf[(p - c) - 3] = '\0';
							roomopts->maxPlayers = atoi(buf);
							if ((roomopts->maxPlayers < 1)
							||  (roomopts->maxPlayers > dp_MAXREALPLAYERS))
								roomopts->maxPlayers = 0;  /* use default */
							c = p;
						} else
							return NULL;
					default:
						printf("Unknown room option -%c\n", *(c+1));
						if ((p = strchr(c, ' ')) == NULL)
							return NULL;
						break;
				}
			} else
				break;
		}
	}
	return c;
}
#endif

/*-------------------------------------------------------------------------
 Read in the HTML template file and get ready for HTML output
-------------------------------------------------------------------------*/
int SetHTMLFile(char *tplFile)
{
	FILE *inTpl;
	struct stat statbuf;
	size_t fsize;
	char *fext;

	if (npages >= MAX_PAGES) {
		printf("Too many template files (%s)\n", tplFile);
		return FALSE;
	}

	/* Allocate memory for the HTML template */
	if (stat(tplFile, &statbuf)) {
		printf("Error %d accessing HTML template file %s\n", errno, tplFile);
		return FALSE;
	}
	if (!statbuf.st_size) {
		printf("HTML template file %s has no size.\n", tplFile);
		return FALSE;
	}
    pages[npages].template = malloc(statbuf.st_size+1);
	if (!pages[npages].template) {
		printf("Error allocating memory for HTML template.\n");
		return FALSE;
	}
	/* Open and read the template file */
	inTpl = fopen(tplFile, "r");
	if (!inTpl) {
		printf("Error %d opening HTML template file %s.\n", errno, tplFile);
		return FALSE;
	}
	fsize = fread(pages[npages].template, 1, statbuf.st_size, inTpl);
	if (statbuf.st_size != (int)(fsize)) {
#ifndef WIN32
		printf("Warning: read %d of %d bytes from file %s\n", fsize, statbuf.st_size, tplFile);
#endif
		if (!fsize) return FALSE;
	}
	pages[npages].template[fsize] = '\0';
	fclose(inTpl);

	/* Create the output file name */
	strcpy(pages[npages].ofname, tplFile);
	/* Replace the file extension with .html, or append it.
	 * NOTE - this code is imperfect! It could overrun the string. */
	fext = strrchr(pages[npages].ofname, '.');
	if (!fext) fext = pages[npages].ofname+sizeof(pages[npages].ofname);
	strcpy(fext, HTMLEXT);
	if (!strcmp(pages[npages].ofname, tplFile)) {
		printf("error: HTML template file %s must not end in .htm or .html!\n", tplFile);
		fprintf(stderr, "error: HTML template file %s must not end in .htm or .html!\n", tplFile);
		exit(1);
	}
	npages++;

	return TRUE;
}

/* uses UPDATE_HTML_FILE to find out which template files to load;
 * generated by servfil script */
#define UPDATE_HTML_FILE "webpg.new"
static void updateHTMLFile()
{
	int i;
	FILE *fp;
	char buf[1024];
	if (!(fp = fopen(UPDATE_HTML_FILE, "r")))
		return;
	for (i = 0; i < npages; i++) {
		free(pages[i].template);
		remove(pages[i].ofname);
	}
	npages = 0;
	while (fgets(buf, 1024, fp)) {
		if (buf[strlen(buf) - 1] = '\n')
			buf[strlen(buf) - 1] = '\0';
		SetHTMLFile(buf);
	}
	fclose(fp);
}

/*-------------------------------------------------------------------------
 Read commandline arguments into globals.  Return 0 on success.
-------------------------------------------------------------------------*/
int ProcessCommandLine(int argc, char **argv) {
	int   i;

	CommDLLName[0] = 0;
	RoomName[0] = 0;
	IniFile[0] = 0;
	FreezeFile[0] = 0;
	FreezeFile2[0] = 0;
	HostsFile[0] = 0;
	SessionType = DP_ILLEGAL_SPECIES;

	for (i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			char *val = strchr(argv[i], '=');

			if (val) val++;
			if (argv[i][1] == 0) {
				printf("Expected letter after %c\n",argv[i][0]);
				return -1;
			}

			switch(toupper(argv[i][1])) {
			case 'A':   /*  apps file name */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1]);
					return -1;
				}
				strncpy(AppsFile, val, sizeof(AppsFile));
				AppsFile[sizeof(AppsFile)-1] = '\0';
                DPRINT(("Loading application info from %s\n",AppsFile));
				break;

			case 'C':   /* Crashlog (atvilog.bin) name */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1]);
					return -1;
				}
				strncpy(AtvilogFile, val, sizeof(AtvilogFile));
				AtvilogFile[sizeof(AtvilogFile)-1] = '\0';
                DPRINT(("Logging crashes to %s\n", AtvilogFile));
				break;

			case 'F':   /*  Freeze/thaw communications File */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1], argv[i]);
					return -1;
				}
				strncpy(FreezeFile, val, sizeof(FreezeFile));
				FreezeFile[sizeof(FreezeFile)-1] = '\0';
				strcpy(FreezeFile2, FreezeFile);
				strcat(FreezeFile2, ".2");
				break;

			case 'H':   /*  hosts file name */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1], argv[i]);
					return -1;
				}
				strncpy(HostsFile, val, sizeof(HostsFile));
				HostsFile[sizeof(HostsFile)-1] = '\0';
				break;

			case 'I':   /*  Initialization File */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1], argv[i]);
					return -1;
				}
				strncpy(IniFile, val, sizeof(IniFile));
				IniFile[sizeof(IniFile)-1] = '\0';
				break;

			case 'L':	/* Default lobby size (max players per) */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1], argv[i]);
					return -1;
				}
				if (sscanf(val, "%d", &LobbyMaxPlayers) != 1) {
					printf("LobbyMaxPlayers must be numeric.\n");
					return -1;
				}
				if (LobbyMaxPlayers < 1 || LobbyMaxPlayers > dp_MAXREALPLAYERS) {
					printf("LobbyMaxPlayers must be between 1 and %d.\n", dp_MAXREALPLAYERS);
					return -1;
				}
				break;

			case 'M':	/* MaxPlayerHdls */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1], argv[i]);
					return -1;
				}
				if (sscanf(val, "%d", &MaxPlayerHdls) != 1) {
					printf("MaxPlayerHdls must be numeric.\n");
					return -1;
				}
				if (MaxPlayerHdls < 1) {
					printf("MaxPlayerHdls must be positive.\n");
					return -1;
				}
				break;

			case 'N':   /*  Communications DLL name */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1], argv[i]);
					return -1;
				}
				strncpy(CommDLLName, val, sizeof(CommDLLName));
				CommDLLName[sizeof(CommDLLName)-1] = '\0';
				break;

			case 'P':   /*  UDP Portnumber */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1], argv[i]);
					return -1;
				}
				Portnum = atoi(argv[i]+3);
				printf("Setting portnum to %d (%s)\n", Portnum, argv[i]+3);
				break;

			case 'Q':   /*  Web message queue directory */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1], argv[i]);
					return -1;
				}
				strncpy(WMQDirectory, val, sizeof(WMQDirectory));
				HostsFile[sizeof(WMQDirectory)-1] = '\0';
				break;

			case 'R':   /*  Room name */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1], argv[i]);
					return -1;
				}
				strncpy(RoomName, val, sizeof(RoomName));
				RoomName[sizeof(RoomName)-1] = '\0';
				break;

			case 'S':	/* Session type */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1], argv[i]);
					return -1;
				}
				if(sscanf(val, "%d", &SessionType) != 1) {
					printf("Session type must be numeric.\n");
					return -1;
				}
				if(SessionType == DP_ILLEGAL_SPECIES) {
					printf("Session type must be positive.\n");
					return -1;
				}
				break;

			case 'W':   /*  HTML web page template of sessions */
				if (!val) {
					printf("Expected = after -%c in %s\n", argv[i][1], argv[i]);
					return -1;
				}
				SetHTMLFile(val);
				break;

			default:
				printf("Unknown arg %s\n", argv[i]);
				return -1;
			}
		} else {
			/* plain file arguments are assumed to be templates */
			SetHTMLFile(argv[i]);
		}
	}
	return 0;
}

static int dp_PASCAL joinsess_cb (dp_session_t *sDesc, long *timeout, long flags, void *context)
{
	if (!sDesc) {
		printf("Error creating session.\n");
		exit(1);
	}
	return FALSE;
}

static int createRoom(dp_t *dp, char *roomName, int sessionType)
{
	char *name;
	dp_session_t  sess;
	dp_result_t err;
	ROOM_OPTS roomopts;

	if (((name = parseRoomStr(roomName, &roomopts)) == NULL) || (*name=='\0')){
		printf("Bad line %s in room file; wanted 'type name'\n", roomName);
		return 1;
	}
	memset(&sess, 0, sizeof(sess));
	sess.sessionType = sessionType;
	if (roomopts.maxPlayers)
		sess.maxPlayers = roomopts.maxPlayers;
	else
		sess.maxPlayers = LobbyMaxPlayers;
	strncpy(sess.sessionName, name, dp_SNAMELEN);
	sess.sessionName[dp_SNAMELEN-1] = 0;
	sess.flags = dp_SESSION_FLAGS_CREATESESSION |
				dp_SESSION_FLAGS_ISLOBBY |		/* Lobby */
				dp_SESSION_FLAGS_MIGRATE_FORBIDDEN;
	if (roomopts.bEnablePlayerVars)
		sess.flags |= dp_SESSION_FLAGS_ENABLE_PLAYERVARS;
	sess.dwUser1 =1; /* Lobby */
	err = dpOpen(dp, &sess, joinsess_cb, NULL);
	if (err != dp_RES_OK) {
		printf("Unable to open a session, error: %d\n", err);
		return 1;
	}
	dpDeclareLobby(dp, 1);
#ifdef dp_ANET2
	dpCastoffSession(dp);
#endif
	return 0;
}

dp_t *dp;

/*-------------------------------------------------------------------------
 Callback triggered by an incoming connection being established in dp.
-------------------------------------------------------------------------*/
void dp_PASCAL incoming_cb(void *adr, int len, int nhdls, dp_result_t err,
	char *user, char *pass, void *context)
{
	char print_adr[256];
	clock_t now = eclock();

	if (!adr || !len) {
		strcpy (print_adr, context);
	} else {
		int i;

		for (i = 0; i < len; i++)
			sprintf(print_adr + 3*i, ":%02x", ((unsigned char *)adr)[i]);
	}
	if (err == dp_RES_OPENED) {
		printf("t:%d, %d conns, Opened host%s, h:%x\n", now, nhdls, print_adr, (playerHdl_t) pass);
		DPRINT(("SERVER: t:%d, %d conns, Opened host%s\n", now, nhdls, print_adr));
#ifdef dp_ANET2
#ifndef WIN32		/* not in dll */
		/* We don't need to send - client will send to us, and
		 * we have a max retry count now
		 */
		dpio_setHandleOptions(dp->dpio, (playerHdl_t)pass, dpio_OPTION_KEEPALIVE);

		servscor_addClient(dp, (playerHdl_t)pass);

#endif
#endif
		err = tserv_client_add(dp->tserv, (playerHdl_t)pass);
		if (err != dp_RES_OK) {
			DPRINT(("SERVER: can't tserv_client_add, err %d?\n", err));
		}

	} else if (err == dp_RES_PEER_CLOSED) {
		printf("t:%d, %d conns, Closing host%s h:%x\n", now, nhdls, print_adr, pass);
		DPRINT(("SERVER: t:%d, %d conns, Closing host%s h:%x\n", now, nhdls, print_adr, pass));
		err = tserv_client_delete(dp->tserv, (playerHdl_t)pass);
		if (err != dp_RES_OK) {
			DPRINT(("SERVER: can't tserv_client_delete, err %d?\n", err));
		}
	} else if (err == dp_RES_CLOSED) {
		printf("t:%d, %d conns, Closed host%s h:%x\n", now, nhdls, print_adr, pass);
		DPRINT(("SERVER: t:%d, %d conns, Closed host%s h:%x\n", now, nhdls, print_adr, pass));
		err = tserv_client_delete(dp->tserv, (playerHdl_t)pass);
		if (err != dp_RES_OK) {
			DPRINT(("SERVER: can't tserv_client_delete, err %d?\n", err));
		}
	} else if (err == dp_RES_OK) {
		printf("t:%d, %d conns, Opened host%s, already open\n", now, nhdls, print_adr);
		DPRINT(("SERVER: t:%d, %d conns, Opened host%s, already open\n", now, nhdls, print_adr));
	} else if (err == dp_RES_HOST_NOT_RESPONDING) {
		printf("t:%d, %d conns, Host%s not responding\n", now, nhdls, print_adr);
		DPRINT(("SERVER: t:%d, %d conns, Host%s not responding\n", now, nhdls, print_adr));
	}
	fflush(stdout);
	(void) context;
}

#ifdef dp_ANET2
/*--------------------------------------------------------------------------
 Callback triggered by an incoming exception record.
--------------------------------------------------------------------------*/
int dp_PASCAL exception_cb(
	dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest,
	char *subkey, int subkeylen, void *buf, size_t sent, size_t total,
	int seconds_left, void *context, dp_result_t status)
{
	if (status != dp_RES_CREATED) return 0;  /* ignore all send/delete messages */
	dp_handleExceptionRecords(buf, sent, aehlog_SERVER_MAXLEN, (aehlog_t *)context);
	dptab_delete(dptab, table, subkey, subkeylen);
	return 0;
}

#define MAX_LINES 512
/*--------------------------------------------------------------------------
 Read in the given text file, and create/update room entries.
--------------------------------------------------------------------------*/
static int updateRoom(
    dp_t *dp,               /* DP to store table in */
    const char *fname)      /* Name of file to read from */
{
	FILE *fp;
	int i, nsess;
	int subkeylen;
	char buf[256];
	char key[dptab_KEY_MAXLEN];
	char subkey[dptab_KEY_MAXLEN];
	dptab_table_t *mysessions;
	dp_result_t err;

	dp_session_t sess[MAX_LINES];
	unsigned short utype[MAX_LINES];
	char uname[MAX_LINES][256];
	char ufullname[MAX_LINES][256];

	assert(dp != NULL);
	assert(fname != NULL);

	/* Get the mysessions table */
	key[0] = dp_KEY_MYSESSIONS;
	if (!(mysessions = dptab_getTable(dp->dt, key, 1))) {
		printf("Serious error: can't find mysessions table\n");
		return 1;
	}
	for (i = 0; i < dptab_tableSize(mysessions); i++) {
		size_t sesslen;
		char *sessbuf;
		dptab_get_byindex(mysessions, i, (void**)&sessbuf, &sesslen, subkey, &subkeylen);
		dp_unpack_session(dp, subkey, subkeylen, sessbuf, sesslen, &sess[i]);
	}

	/* Open the file */
	fp = fopen(fname, "r");
	if (fp == NULL) {
		printf("Can't open rooms file %s\n", fname);
		return 1;
	}
	nsess = 0;
	while (fgets(buf, sizeof(buf), fp)) {
		ROOM_OPTS roomopts;
		char namebuf[128], *name;
		if (sscanf(buf, "%d %[^\n\r\f]", &utype[nsess], namebuf) != 2) {
			printf("Bad line %s in room file; wanted 'type name'\n", buf);
			fclose(fp);
			break;
		} else {
			name = parseRoomStr(namebuf, &roomopts);
			if (name) {
				strcpy(uname[nsess], name);
				strcpy(ufullname[nsess], namebuf);
				nsess++;
			}
		}
	}
	fclose(fp);

	/* FIXME: For room entry, should delete those that no longer exist;
	 *   but what to do about players in those rooms? */
	/* For each line in the file, add new table entries */
	err = dp_RES_OK;
	for (i = 0; i < nsess; i++) {
		int j, err1;
		for (j = 0; j < dptab_tableSize(mysessions); j++) {
			if ((sess[j].sessionType == utype[i]) &&
				!strcmp(sess[j].sessionName, uname[i]))
				break;
		}
		if (j >= dptab_tableSize(mysessions)) {
			err1 = createRoom(dp, ufullname[i], utype[i]);
			if (err1)
				err = err1;
		}
	}
	return (err == dp_RES_OK) ? 0 : 1;
}

/*--------------------------------------------------------------------------
 Read in the given text file, and create/update server entries.
 Each entry has its ip address and port number as its key,
 and its hostname (possibly followed by a blank and a description).
--------------------------------------------------------------------------*/
static int updateServerTable(dp_t *dp, const char *fname)
{
	FILE *fp;
	int i, nserver;
	int subkeylen;
	char buf[256];
	char key[dptab_KEY_MAXLEN];
	char subkey[dptab_KEY_MAXLEN];
	dptab_table_t *servers;
	dp_result_t err;

	char uhostname[MAX_LINES][200], uadr[MAX_LINES][dp_MAX_ADR_LEN];

	assert(dp != NULL);
	assert(fname != NULL);

	/* Open the file */
	fp = fopen(fname, "r");
	if (fp == NULL) {
		printf("Can't open servers file %s\n", fname);
		return 1;
	}

	key[0] = dp_KEY_SERVERS;
	err = dptab_createTable(dp->dt, &servers, key, 1,0, NULL,NULL,NULL,NULL);
	if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
		printf("Can't create/get servers table\n");
		return 1;
	}

	nserver = 0;
	err = dp_RES_OK;
	while (fgets(buf, sizeof(buf), fp)) {
		char ipadr[128], hostname[200];
		if (sscanf(buf, "%s %s", ipadr, hostname) != 2) {
			printf("Bad line %s in server file; wanted 'ipadr hostname ...'\n", buf);
			err = dp_RES_BAD;
		} else {
			int len = dpio_scanAdr(dp->dpio, ipadr, uadr[nserver], dp_MAX_ADR_LEN);
			printf("len %d, myAdrLen %d\n", len, dp->dpio->myAdrLen);
			if (len != dp->dpio->myAdrLen) {
				printf("Can't resolve address %s\n", ipadr);
				continue;
			}
			strcpy(uhostname[nserver], hostname);
			nserver++;
		}
	}
	fclose(fp);

	/*For server table entry, delete those that no longer exist*/
	for (i = dptab_tableSize(servers) - 1; i >= 0; i--) {
		size_t hlen, j;
		char *hbuf;
		if (dptab_get_byindex(servers, i, (void**)&hbuf, &hlen, subkey, &subkeylen) != dp_RES_OK)
			continue;
		for (j = 0; j < nserver; j++) {
			if (!strncmp(subkey, uadr[j], subkeylen))
				break;
		}
		if (j >= nserver)
			dptab_delete(dp->dt, servers, subkey, subkeylen);
	}
	/* For each line in the file, add/update table entries */
	for (i = 0; i < nserver; i++) {
		int err1 = dptab_set(dp->dt, servers, uadr[i], dp->dpio->myAdrLen, uhostname[i], strlen(uhostname[i]), 4, PLAYER_ME);
		if (err1 != dp_RES_OK) {
			printf("Can't update servers entry.\n");
			err = err1;
		}
	}
	return (err == dp_RES_OK) ? 0 : 1;
}

/*--------------------------------------------------------------------------
 Read in the given text file, and create/update app table entries.
--------------------------------------------------------------------------*/
static int updateAppTable(
    dp_t *dp,               /* DP to store table in */
    const char *fname)      /* Name of file to read from */
{
	FILE *fp;
	int i, napp;
	int subkeylen;
	char buf[256];
	char key[dptab_KEY_MAXLEN];
	char subkey[dptab_KEY_MAXLEN];
	dptab_table_t *apps;
	dp_result_t err;

	unsigned short utype[MAX_LINES];
	unsigned short uplat[MAX_LINES];
	unsigned char ulang[MAX_LINES];
	dp_version_t uver[MAX_LINES];

	assert(dp != NULL);
	assert(fname != NULL);

	/* Open the file */
	fp = fopen(fname, "r");
	if (fp == NULL) {
		printf("Can't open apps file %s\n", fname);
		return 1;
	}

    /* Get or create the app table */
	key[0] = dp_KEY_APPLICATIONS;
	err = dptab_createTable(dp->dt, &apps, key, 1, 0, NULL,NULL,NULL,NULL);
	if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
		printf("Can't create/get apps table\n");
		return 1;
	}

	napp = 0;
	err = dp_RES_OK;
	while (fgets(buf, sizeof(buf), fp)) {
		unsigned int vmaj, vmin;
		DPRINT(("loadAppTable: line is %s\n",buf));
		if (sscanf(buf, "%u %u %u %u%*c%u\n",&utype[napp],&uplat[napp],&ulang[napp], &vmaj, &vmin) != 5) {
			printf("Bad line %s in apps file; wanted 'type platform language ...'\n", buf);
			err = dp_RES_BAD;
		} else {
			uver[napp].major = vmaj&0xffff;
			uver[napp].minor = vmin&0xffff;
			napp++;
		}
	}
	fclose(fp);

	/*For app table entry, delete those that no longer exist*/
	for (i = dptab_tableSize(apps) - 1; i >= 0; i--) {
		unsigned short type;
		unsigned short plat;
		unsigned char lang;
		size_t vlen, j;
		dp_version_t *vbuf;
		if (dptab_get_byindex(apps, i, (void**)&vbuf, &vlen, subkey, &subkeylen) != dp_RES_OK)
			continue;
		type = dpMAKESHORT(subkey[0], subkey[1]);
		plat = dpMAKESHORT(subkey[2], subkey[3]);
		lang = subkey[4];
		for (j = 0; j < napp; j++) {
			if ((utype[j] == type) && (uplat[j] = plat) && (ulang[j] = lang))
				break;
		}
		if (j >= napp)
			dptab_delete(dp->dt, apps, subkey, subkeylen);
	}
	/* For each line in the file, add/update table entries */
	for (i = 0; i < napp; i++) {
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen = 0;
		int err1;
		subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(utype[i]);
		subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(utype[i]);
		subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(uplat[i]);
		subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(uplat[i]);
		subkey[subkeylen++] = (char) (ulang[i] & 0xFF);

		err1 = dptab_set(dp->dt, apps, subkey, subkeylen, &uver[i], sizeof(uver[i]), 4, PLAYER_ME);
		if (err != dp_RES_OK) {
			printf("Can't create/update apps entry.\n");
			err = err1;
		}
	}
	return (err == dp_RES_OK) ? 0 : 1;
}

int thawFreezeFile(FILE *fp, clock_t *gsBeacon)
{
	int j, gsmax, bufGsInd, gsTimeLeft;
	char magic[9];
	time_t bufStartTime;
	fread(magic, 9, 1, fp);
	if (memcmp(magic, "serverdat", 9)) {
		printf("Bad freeze header\n");
		fclose(fp);
		return 1;
	}
	fread(&bufStartTime, sizeof(bufStartTime), 1, fp);
	if (!assoctab_thaw(GamesServed, fp)) {
		printf("Can't thaw GamesServed table\n");
		fclose(fp);
		return 1;
	}
	fread(magic, 9, 1, fp);
	if (memcmp(magic, "serverend", 9)) {
		printf("Bad freeze trailer\n");
		fclose(fp);
 		assoctab_destroy(GamesServed);
		GamesServed = assoctab_create(sizeof(int));
		return 1;
	}
	StartTime = bufStartTime;
	if (fread(&bufGsInd, sizeof(bufGsInd), 1, fp) <= 0) { /* old freeze format */
		printf("old freeze format\n");
		fclose(fp);
		return 0;
	}
	fread(&gsmax, sizeof(gsmax), 1, fp);
	if (gsmax != GSMAX) {
		printf("err: thaw GSMAX %d; curr GSMAX %d\n", gsmax, GSMAX);
		return 1;
	}
	fread(&gsTimeLeft, sizeof(gsTimeLeft), 1, fp);
	if (!assoctab_thaw(GamesServedSlice, fp)) {
		printf("Can't thaw GamesServedSlice table\n");
		fclose(fp);
 		assoctab_destroy(GamesServedSlice);
		GamesServedSlice = assoctab_create(GSMAX * sizeof(int));
		return 1;
	}
	fread(magic, 10, 1, fp);
	if (memcmp(magic, "serverend2", 9)) {
		printf("Bad freeze trailer2\n");
		fclose(fp);
 		assoctab_destroy(GamesServedSlice);
		GamesServedSlice = assoctab_create(GSMAX * sizeof(int));
		return 1;
	}
	gsindex = bufGsInd;
	*gsBeacon = eclock() + gsTimeLeft;
	fclose(fp);
	return 0;
}
#endif

static void sendMailToSubjBody(const char *to,
							   const char *subject,
							   const char *body)
{
	FILE *fp = popen("/usr/sbin/sendmail -t", "w");
	fprintf(fp, "To: %s\n", to);
	fprintf(fp, "Subject: %s\n", subject);
	fprintf(fp, "\n%s\n", body);
	fclose(fp);
}

#ifdef WEBLAUNCH
/*--------------------------------------------------------------------------
 User is trying to get a document from the embedded web server.
 The only documents we serve are launch files.
 There are three kinds of launch files
 (where TTT is a session type number, etc.):
 host, with URLs /h.anetTTT?sessname=NNN&playername=PPP&maxplayers=MMM
 server join, with URLs /s/XX.XX.XX.XX.XX.XX.XX.XX.anetTTT where XX=sessId
 direct join, with URLs /d/XX.XX.XX.XX.XX.XX.XX.XX.anetTTT where XX=sessId
 We use a special mime type, application/anetTTT, to tell the web browser
 to launch the game as a helper app.
 The URLs are chosen so that when the game is launched as a helper app,
 the file it is passed on the command line ends in .anetTTT, which should
 make it easier for the game to recognize that it is being launched from
 a third party shell (in this case, a web browser).
--------------------------------------------------------------------------*/
int url2buf(char *context, const char *url, char *buf, int buflen, bhttp_url2buf_result_t *urlResult)
{
	char reftype;
	char sessid[12];
	char playername[64];
	char sessname[64];
	int n;
	int maxplayers;
	dp_session_t sess;
	dp_result_t err;
	char serverAddress[64];

	urlResult->httpResultCode = 404;
	strcpy(urlResult->mimeType, "text/plain");

	err = dpio_printAdr(dp->dpio, dp->dpio->myAdr, dp->dpio->myAdrLen, serverAddress, sizeof(serverAddress));
	if (err != dp_RES_OK)
		return sprintf(buf, "Error %d getting address of server", err);

	n = sscanf(url, "/%c", &reftype);
	if (n != 1) {
		return sprintf(buf, "Error: bad URL %s", url);
	}
	if (reftype == 'h') {
		int sesstype;
		/* Host session via server */
		n = sscanf(url+2, ".anet%d?sessname=%63[^&]&playername=%63[^&]&maxplayers=%d", &sesstype, sessname, playername, &maxplayers);
		if (n < 1) {
			return sprintf(buf, "Error: bad URL %s", url);
		}
		if (!strlen(sessname)) {
			urlResult->httpResultCode = 400;
			return sprintf(buf, "Error: no session name specified");
		}
		if (!strlen(playername)) {
			urlResult->httpResultCode = 400;
			return sprintf(buf, "Error: no player name specified");
		}
		urlResult->httpResultCode = 200;
		sprintf(urlResult->mimeType, "application/anet%d", sesstype);
		return sprintf(buf, "%s\nn=anet/winets2.dll\ng=%s\nh\ns=%s\np=%s\nm=%d",
						dp_LAUNCHPARAMS_T_SIG, serverAddress, sessname, playername, maxplayers);
	}
	n = sscanf(url, "/%c/%2x.%2x.%2x.%2x.%2x.%2x.%2x.%2x",
				&reftype,
				sessid+0,
				sessid+1,
				sessid+2,
				sessid+3,
				sessid+4,
				sessid+5,
				sessid+6,
				sessid+7);

	if (n != 9) {
		return sprintf(buf, "Error: bad URL %s", url);
	}
	err = dpGetSessionDescById(dp, sessid, &sess, NULL);
	if (err != dp_RES_OK) {
		return sprintf(buf, "Error %d getting session description for %s", err, url);
	}
	if (reftype == 'd') {
		char adrMasterAscii[64];
		err = dpio_printAdr(dp->dpio, sess.adrMaster, dp->dpio->myAdrLen, adrMasterAscii, sizeof(adrMasterAscii));
		if (err != dp_RES_OK)
			return sprintf(buf, "Error %d getting address of master", err);

		/* Direct join using ip addresses */
		urlResult->httpResultCode = 200;
		sprintf(urlResult->mimeType, "application/anet%d", sess.sessionType);
		return sprintf(buf, "%s\nn=anet/winets2.dll\na=%s\nJ=0\n",
						dp_LAUNCHPARAMS_T_SIG, adrMasterAscii);
	} else if (reftype == 's') {
		/* Indirect join using server */
		urlResult->httpResultCode = 200;
		sprintf(urlResult->mimeType, "application/anet%d", sess.sessionType);
		return sprintf(buf, "%s\nn=anet/winets2.dll\ng=%s\nj\nl=%s",
						dp_LAUNCHPARAMS_T_SIG, serverAddress, url+3);
	}

	return sprintf(buf, "Bad URL : %s", url);
}
#endif

int main( int argc, char *argv[] )
{
	dp_result_t   err;
	commInitReq_t commInitReq;
	clock_t enumBeacon = eclock();
	clock_t gsBeacon = eclock();
	char *thawMe;
	sbdserv_t *sbdserv;
#ifdef WEBLAUNCH
	bhttp_t *bhttp;
#endif
	long temp;
	FILE *fp;
#ifdef dp_ANET2
	aehlog_t aehlog;
#endif
	dptab_table_t *scoretab;

	memset(&pages, 0, sizeof(pages));

	memset(&commInitReq, 0, sizeof(commInitReq));
	commInitReq.sessionId = rand() | (rand() << 16);
	commInitReq.reqLen = sizeof(commInitReq_t);
	commInitReq.portnum = Portnum;

	if ((argc == 1) || ProcessCommandLine(argc, argv)) {
		printf("Usage: %s [-N=comm.dll] [-R=roomFile] [-R=roomName -S=sessionType]\n", argv[0]);
		printf("            [-I=iniFile][-F=freezeFile][-p=port]\n");
		printf("            [-W=htmlFile1][-W=htmlFile2][-W=...]\n");
		printf("            [-h=serversFile][-a=appsFile][-L=lobbysize]\n");
		exit(1);
	}
	commInitReq.portnum = Portnum;

	aehlog_Create(AtvilogFile, &aehlog);

#ifndef UNIX
	if (!CommDLLName[0]) {
		printf("No DLL was specified.\n");
		exit(1);
	}
#else
	strcpy(CommDLLName, "unused.dll");
#endif

	if (!RoomName[0]) {
		printf("No room was specified.\n");
		exit(1);
	}

	dp_enableDebugPrint(TRUE);

	GamesServed = assoctab_create(sizeof(int));
	if (!GamesServed)
		exit(1);
	GamesServedSlice = assoctab_create(GSMAX * sizeof(int));
	if (!GamesServedSlice)
		exit(1);
	gsindex = 0;
	StartTime = time(0);
	if (!FreezeFile2[0]) {
		/* no freeze file specified */
		;
	} else if ((fp = fopen(FreezeFile2, "rb")) != NULL) {
		/* if can't thaw file can't be read, remove it */
		if (thawFreezeFile(fp, &gsBeacon) != 0)
			remove(FreezeFile2);
	} else {
		printf("No server freeze file %s found\n", FreezeFile2);
		DPRINT(("SERVER: No server freeze file %s found\n", FreezeFile2));
	}
	/* Freeze file detector */
	thawMe = (FreezeFile[0] && !access(FreezeFile, 04)) ? FreezeFile : NULL;

	/* Initialize the networking layer */
	if (IniFile[0])
		dpini_SetFile(IniFile);
	/* Thaw from a freeze file if we specified one */
	err = dpCreate(&dp, (dp_transport_t *)CommDLLName, &commInitReq, thawMe);

	if (err != dp_RES_OK && thawMe) {
		printf("dpCreate failed; Unable to start communications\n");
		printf("retry dpCreate without thawing freeze file\n");
		err = dpCreate(&dp, (dp_transport_t *)CommDLLName, &commInitReq, NULL);
		if (err != dp_RES_OK) {
			printf("Fatal error: dpCreate failed without thawing freeze file, exiting\n");
			DPRINT(("Fatal error: dpCreate failed without thawing freeze file, exiting\n"));
			exit(1);
		}
		thawMe = NULL;  /* Continue as if there hadn't been a freeze file */
		remove(FreezeFile);
	}
	if (!dp) {
		printf("Fatal error: can't start communications.\n");
		DPRINT(("Fatal error: can't start communications.\n"));
		exit(1);
	}
	/* Set a callback to log new incoming connections and process exception
	 * records */
	err = dpSetConnCallback(dp, incoming_cb, "<UNKNOWN>");
	if (err != dp_RES_OK) {
		printf("Fatal error: Unable to set callback, aborting.\n");
		DPRINT(("Fatal error: Unable to set callback, aborting.\n"));
		exit(1);
	}
#ifdef dp_ANET2
	err = dp_setExceptionCb(dp->dt, exception_cb, &aehlog);
	if (err != dp_RES_OK) {
		printf("Unable to set callback for exception records\n");
		exit(1);
	}
#endif
	dpGetParameterLong(dp, dp_PARAMID_FILEDESC, &temp);
	dpiofd = (int)temp;
	printf("dpGetParameterLong returns dpio filedesc:%d\n", dpiofd);

#ifdef dp_STATS
	dp_avgstat_init(dp, &avgstat);
#endif

#ifdef dp_ANET2

#ifdef dp_MULTISESSTABLE
	/* Set a callback to loop sessions into species-specific table */
	dptab_setTableCallback(dp->sessions, sessions_cb, dp);
#endif

	/* Set a callback to loop mysessions into sessions */
	/* (Only needed on standalone or master servers) */
	dptab_setTableCallback(dp->mysessions, mysessions_cb, dp);

	/* Load a server table for those clients who just gotta have a newer
	 * version of the table of game servers.
	 * Eventually, we, too, will subscribe to this table from the
	 * master server.
	 */
	if (!HostsFile[0]) {
		printf("Must specify servers file with -h\n");
		exit(1);
	}
	if (updateServerTable(dp, HostsFile) != 0)
		exit(1);

    /* Load a table of applications */
	if (updateAppTable(dp, AppsFile) != 0)
		exit(1);
#endif

	/* Create the room */
	if (!thawMe) {
		printf("SessionType is %d\n", SessionType);
		if (SessionType != DP_ILLEGAL_SPECIES) {
			/* if -S given, -R arg was literal roomname */
			createRoom(dp, RoomName, SessionType);
		} else {
			/* if no -S given, -R arg must be a filename */
			if (updateRoom(dp, RoomName) != 0)
				exit(1);
		}
	}

	/* Set the sessinfo pointer in uudps for pinging */
	{	commSetParamReq_t req;
		commSetParamResp_t resp;
		memset(&req, 0, sizeof(commSetParamReq_t));
		req.reqLen = sizeof(commSetParamReq_t);
		req.param_num = comm_PARAM_SESSIONINFO;
		req.param_value = (int)&sessinfo;
		if (commSetParam(&req, &resp, dp->dpio->commPtr) != 1) {
			printf("Couldn't commSetParam on sessinfo, status %d\n", resp.status);
			exit(1);
		}
	}

	/* Set the signal actions for quitting */
	signal(SIGTERM, exitNow);
	signal(SIGINT, exitNow);
	signal(SIGHUP, updateNow);

	/* Retreive the host name for future use */
	if (gethostname(Hostname, 128))
		strcpy(Hostname, "[unknown host]");
	printf("Got host name %s\n", Hostname);

#ifdef UNIX	/* not in win32 dll yet */
	/* Set the max number of connections way up */
	dpio_setMaxPlayerHdls(dp->dpio, MaxPlayerHdls);
#endif

	if (!thawMe) {
		/* Initialize the user login stuff, if we didn't thaw it. */
		dp->tca = tca_create();
		if (!dp->tca) {
			printf("Can't init tca.\n");
			exit(1);
		}
		err = tca_openpw(dp->tca, "prefs.dat");
		if ((err != dp_RES_OK) && (err != dp_RES_EMPTY)) {
			printf("Can't open prefs.dat database.\n");
			exit(1);
		}
		dp->tserv = tserv_create(dp->dpio, dp->tca);
		if (!dp->tserv) {
			printf("Can't init tserv.\n");
			exit(1);
		}
	}

	servscor_init(dp, WMQDirectory);

	sbdserv = sbdserv_create(SBDSERV_PORT);
	if (!sbdserv) {
		printf("Can't init sbdserv!\n");
		exit(1);
	}
#ifdef WEBLAUNCH
	bhttp = bhttp_create(8080, url2buf, NULL);
	if (!bhttp) {
		printf("Can't init bhttp!\n");
		exit(1);
	}
#endif

	/* Sit there until asked to quit */
	while (!pleaseQuit) {
#ifdef dp_ANET2
		playerHdl_t hFrom;
#else
		dpid_t idFrom;
#endif
		dpid_t idTo;

#include "dppack1.h"
		struct {
			dp_packetType_t type PACK;
			union {
				dp_user_addPlayer_packet_t addPlayer;
				dp_user_delPlayer_packet_t delPlayer;
				dp_account_packet_t account;
				dp_sessionResult_packet_t sessionRes;
				char buf[1024];
			} u PACK;
		} pkt;
#include "dpunpack.h"

		size_t size;
		fd_set rfds;
		fd_set wfds;
		struct timeval tv;
		int nsocks;
		int sockmax;
		int socktemp;
		int sock;
		clock_t now = eclock();

		tv.tv_sec = 0;
		tv.tv_usec = 50000;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		sockmax = sbdserv_getfds(sbdserv, &rfds);
#ifdef WEBLAUNCH
		socktemp = bhttp_getfds(bhttp, &rfds, &wfds);
		if (socktemp > sockmax)
			sockmax = socktemp;
#endif
		FD_SET(dpiofd, &rfds);
		if (dpiofd > sockmax)
			sockmax = dpiofd;
		nsocks = select(sockmax + 1, &rfds, &wfds, NULL, &tv);
		if (nsocks < 0) {
			DPRINT(("SERVER: bug: select error:%d\n", errno));
			break;
		}
#ifdef WEBLAUNCH
		bhttp_poll(bhttp, &rfds, &wfds, nsocks);
#endif

		if (pleaseUpdate) {
			enumBeacon = eclock(); /* force quick write/flush of web pages */
			updateAppTable(dp, AppsFile);
			updateServerTable(dp, HostsFile);
			if (SessionType == DP_ILLEGAL_SPECIES)
				updateRoom(dp, RoomName);
			updateHTMLFile(); /* please see comment before updateHTMLFile() */
			pleaseUpdate = FALSE;
		}

#ifdef WIN32
		Sleep(10); /* be a good citizen */
#endif
		if ((long)(now - enumBeacon) > 0) {
			dp_session_t  sess;
			int i;

			/* Reset the beacon */
			enumBeacon = now + ENUM_BEACON_INTERVAL;

			/* Increment the notseen flag on all the sessions */
			for (i=0; i<n_sessions; i++) sessions[i].notseen++;

			/* update games served list */
			if (now - gsBeacon > 0) {
				int j;

				/* Reset the beacon */
				gsBeacon = now + GS_BEACON_INTERVAL();

				/* get GamesServedSlice's and zero its gsindex'th member */
				gsindex = (gsindex == GSMAX - 1) ? 0 : gsindex + 1;
				for (j = 0; j < GamesServedSlice->n_used; j++) {
					int *gameslice = (int *)(((assoctab_item_t *)assoctab_getkey(GamesServedSlice, j))->value);
					gameslice[gsindex] = 0;
				}
			}

			/* Enumerate the sessions */
			sess.sessionType = 0;
			memset(&sessinfo, 0, sizeof(sessinfo));
			sessinfo.max_players = dp->dpio->max_playerHdls;
#if 0
			printf("Listing sessions using dpEnumSessions\n", err);
#endif
			if (GamesActive)
				assoctab_destroy(GamesActive);
			GamesActive = assoctab_create(sizeof(int));

			DPRINT(("SERVER: Listing sessions using dpEnumSessions\n", err));
			err = dpEnumSessions(dp, &sess, NULL, 0L, listSessions_cb, dp);
			if (err != dp_RES_OK) {
				printf("Error %d calling dpEnumSessions\n", err);
				DPRINT(("SERVER: Error %d calling dpEnumSessions\n", err));
			}

#ifdef dp_ANET2
			/* flush and close the exception file */
			aehlog_close(&aehlog);
#endif

			/* Check for score reports finished being cross-checked */
			servscor_poll(dp);
		}

		/* Check for incoming sbdclnt exception reports */
		sbdserv_startRead(sbdserv, nsocks);
		while (1) {
			char buf[sbd_MAXLEN];
			int len;
			int i;
			len = sbdserv_poll(sbdserv, &rfds, buf);
			if (len < 0) {
				DPRINT(("SERVER: sbdserv_poll error!\n"));
				break;
			} else if (0 == len) {
				break;
			}
#if 0
			printf("sbdserv got exception record:");
			for (i = 0; i < len; i++)
				printf("%02f", buf[i]);
			printf("\n");
#endif
			/* got an exception record via sbd */
			dp_handleExceptionRecords(buf, len, aehlog_SERVER_MAXLEN, &aehlog);
		}

		/* Check dpio if select timed out or dpiofd has something to read */
		if (nsocks && !FD_ISSET(dpiofd, &rfds))
			continue;

		while (1) {
			size = sizeof(pkt);
			err = dpReceive(dp, (dpid_t *)&hFrom, &idTo, dpReceive_FLAGS_DPIO, &pkt, &size);
			if (err == dp_RES_EMPTY) {
				dpFlush(dp);
				break;
			}
			if (err != dp_RES_OK) {
				printf("Unable to receive a packet, error: %d\n", err);
				DPRINT(("SERVER: Unable to receive a packet, err: %d\n", err));
				break;
			}

			/* printf("."); */
			fflush(stdout);
			switch (pkt.type) {
			case dppt_MAKE('e','1'):
				printf("server: Got add request from h:%x.\n", hFrom);
				dp_addClient(dp, hFrom, pkt.u.buf, size-sizeof(pkt.type));
				/* Note: dp will call deletePeer automatically if hdl closes */
				break;

			case dp_ACCOUNT_PACKET_ID:
				printf("server: got account message reason:%d uid:%d h:%x\n",
						pkt.u.account.reason,
						pkt.u.account.uid,
						pkt.u.account.reserved);
				if (pkt.u.account.reason == dp_RES_CREATED ||
					pkt.u.account.reason == dp_RES_HELP) {
					tcapw_entry_t entry;
					dp_result_t res;

					/* send secret code email on account creation or request */
					res = tserv_uid2info(dp->tserv, pkt.u.account.uid, &entry);
					if (res == dp_RES_OK) {
						char body[256];
						dp_netchar_t secretcode[tcapw_LEN_PW];
						wchar_t wsecretcode[tcapw_LEN_PW];
						char asecretcode[tcapw_LEN_PW];

						res = tcapw_get_secretcode(&entry, secretcode);
						if (res == dp_RES_OK) {
							mywcs_netchar2wchar(wsecretcode, secretcode, tcapw_LEN_PW);
							mywcs_tombs(asecretcode, tcapw_LEN_PW, wsecretcode);
							sprintf(body, "Welcome to ActiveLink!\nYour activation code is %s\n", asecretcode);
							sendMailToSubjBody(entry.email,
											   "ActiveLink Account Activation",
											   body);
							DPRINT(("SERVER: emailed secretcode:%s to %s, uid:%d uname:%s\n", asecretcode, entry.email, pkt.u.account.uid, tcapw_u2ascii(entry.uname.uname, tcapw_LEN_USERNAME)));
						} else {
							DPRINT(("SERVER: tcapw_get_secretcode returns err:%d, uid:%d uname:%s\n", res, pkt.u.account.uid, tcapw_u2ascii(entry.uname.uname, tcapw_LEN_USERNAME)));
						}
					} else {
						DPRINT(("SERVER: tserv_uid2info returns err:%d, uid:%d\n", res, pkt.u.account.uid));
					}
				}
				break;

			case dp_SESSIONRESULT_PACKET_ID:
				if (pkt.u.sessionRes.reason == dp_RES_OK) {
					printf("Created sessType:%d session:%s\n", pkt.u.sessionRes.sess.sessionType, pkt.u.sessionRes.sess.sessionName);
					DPRINT(("SERVER: created sessType:%d session:%s\n", pkt.u.sessionRes.sess.sessionType, pkt.u.sessionRes.sess.sessionName));
				} else {
					printf("Failed to create sessType:%d session:%s, reason:%d\n", pkt.u.sessionRes.sess.sessionType, pkt.u.sessionRes.sess.sessionName, pkt.u.sessionRes.reason);
					DPRINT(("SERVER: failed to create sessType:%d session:%s, reason:%d\n", pkt.u.sessionRes.sess.sessionType, pkt.u.sessionRes.sess.sessionName, pkt.u.sessionRes.reason));
				}
				break;

			default:
				printf("That's odd; got a %2.2s packet.\n", &pkt.type);
				DPRINT(("SERVER: That's odd; got a %2.2s packet\n", &pkt.type));
				break;
			}
		}
	}
	DPRINT(("Received signal; cleaning up.\n"));
	sbdserv_destroy(sbdserv);
#ifdef WEBLAUNCH
	bhttp_destroy(bhttp);
#endif

	/* If there is a freeze file specified, freeze before exiting */
	if (FreezeFile[0] && dp) {
		FILE *fp;

		err = dpFreeze(dp, FreezeFile);
		if (err != dp_RES_OK) {
			printf("Error %d freezing communications.\n", err);
			DPRINT(("SERVER: Error %d freezing communications.\n", err));
		}
		if ((fp = fopen(FreezeFile2, "wb")) != NULL) {
			int j, gsmax, gsTimeLeft;

			fwrite("serverdat", 9, 1, fp);
			fwrite(&StartTime, sizeof(StartTime), 1, fp);
			assoctab_freeze(GamesServed, fp);
			fwrite("serverend", 9, 1, fp);
			fwrite(&gsindex, sizeof(gsindex), 1, fp);
			gsmax = GSMAX;
			fwrite(&gsmax, sizeof(gsmax), 1, fp);
			gsTimeLeft = gsBeacon - eclock();
			fwrite(&gsTimeLeft, sizeof(gsTimeLeft), 1, fp);
			assoctab_freeze(GamesServedSlice, fp);
			fwrite("serverend2", 10, 1, fp);
			fclose(fp);
		} else {
			printf("Error: fail to write to %s\n", FreezeFile2);
			DPRINT(("SERVER: Error: fail to write to %s\n", FreezeFile2));
		}
	}
	if (dp) dpDestroy(dp, 0);	/* go ahead and hang up */
    return(0);
}
