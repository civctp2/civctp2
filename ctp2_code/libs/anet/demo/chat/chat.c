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

/*-------------------------------------------------------------------------
 Chat Program started using dp session from a freeze file.
-------------------------------------------------------------------------*/
#include <stdio.h>
#include <crtdbg.h>
#include <direct.h>
#include "anet.h"
#include "dppack1.h"
#include "raw.h"
#include "eclock.h"

#define CHAT_PACKET_ID dppt_MAKE('C', 'H')
#define MAX_MESSAGE 200

dpid_t my_id = dp_ID_NONE;
int quitState = 0; /* set to 1 to start quit sequence */

/* Any program that gets compressed by pecomp must import these two function,
 * but doesn't need really to call them.
 * If you use the /MDd option with visual C, it won't for some reason
 * import them by default, so you have to explicitly.
 * Must export this to avoid dead code elimination.
 */
DP_API void foo()
{
	LoadLibrary(NULL);
	GetProcAddress(NULL, NULL);
}

/*-------------------------------------------------------------------------
 Call to quit chat.
-------------------------------------------------------------------------*/
static void quit(dp_t *myDP, int exit_code)
{
	if (myDP) {
		dpDestroy(myDP, 0);	/* go ahead and hang up */
	}
	raw_end();
	printf("chat: Exiting with status %d.\n", exit_code);

	if (exit_code) {
		printf("Press any key to continue\n");
		getch();
	}

	exit(exit_code);
}

/*-------------------------------------------------------------------------
 Call to send chat message.
-------------------------------------------------------------------------*/
dp_result_t chat_broadcast(dp_t *myDP, char *message)
{
	struct {
		dp_packetType_t type;
		char buf[MAX_MESSAGE];
		char skip[6];
	} pkt;
	dp_result_t err;

	pkt.type = CHAT_PACKET_ID;
	strcpy(pkt.buf, message);
	err = dpSend(myDP, my_id, dp_ID_BROADCAST, dp_SEND_RELIABLE, &pkt, sizeof(pkt) - sizeof(pkt.skip));
	return err;
}

/*-------------------------------------------------------------------------
 Show command line usage
 ------------------------------------------------------------------------*/
void usage()
{
	printf("\
Usage: chat(d) [filename]\n\
Starts chat session according to parameters in given file\n\
created by anetdrop (default: freeze.dat)\n");
	exit(1);
}

/*--------------------------------------------------------------------------
 Convert a object delta status to human readable form.
--------------------------------------------------------------------------*/
static const char *status2a(dp_result_t status)
{
	switch (status) {
	case dp_RES_CREATED:
		return "created";
	case dp_RES_CHANGED:
		return "changed";
	case dp_RES_DELETED:
		return "deleted";
	default:
		printf("chat: bad status\n");
		quit(NULL, 1);
	}
}

/*-------------------------------------------------------------------------
 cd to the directory of the executable.  Web browsers start
 helper apps in a random directory, which makes the helper app very unhappy
 if it expected to find its own data files in the current working
 directory.  Call this function at the beginning of a helper app to fix
 this.
 Returns 0 on error
 		 1 on success
-------------------------------------------------------------------------*/
static int cdToAppDir(void)
{
	char dir[MAX_PATH];
	char *chop;

	if (!GetModuleFileName(NULL, dir, sizeof(dir)))
			return 0;

	chop = strrchr(dir, '\\');
	if (chop)
		*chop = 0;
	else
		return 0;

	return (SetCurrentDirectory(dir));
}

/*--------------------------------------------------------------------------
 The standard argv under Windows was computed without regard to quotes.
 In order to support filenames with spaces, ignore the standard argv,
 and compute one that obeys double quotes.
--------------------------------------------------------------------------*/
static int getargv(char argv[][MAX_PATH], int maxargc)
{
	char	*p, *q;
	int		argc;

	p = GetCommandLine();
	printf("Command line:%s\n", p);
	for (argc = 0; (argc < maxargc) && *p; argc++) {
		// Skip to beginning of next argument
		while (*p && isspace(*p))
			p++;
		if (!*p)
			break;
		// Copy next argument; handle quoted string correctly
		if (*p == '"') {
			p++;		// skip quote
			for (q=argv[argc]; *p && (*p != '"'); )
				*q++ = *p++;
			*q = 0;
			p++; 		// skip quote
		} else {
			for (q=argv[argc]; *p && !isspace(*p); )
				*q++ = *p++;
			*q = 0;
		}
	}

	return argc;
}

#define MAX_ARGS 2

/*-------------------------------------------------------------------------
 Initialize session and run main chat loop
 ------------------------------------------------------------------------*/
int chat(int sysargc, char **sysargv)
{
	char	argv[MAX_ARGS][MAX_PATH];
	int		argc;
	int exitCode = 0;
	dp_t *myDP;
	dp_result_t err = dp_RES_OK;
	char freezefile[100];
	char kbuf[MAX_MESSAGE] = "";
	char masterHostName[64];
	int keylen;
	char key[dp_MAX_ADR_LEN + 3];
	dp_session_t sess;
	char cwdbuf[MAX_PATH];

	argc = getargv(argv, MAX_ARGS);
	raw_init();
	if (argc == 1)
		strcpy(freezefile, "freeze.dat");
	else if ((argc == 2) && (argv[1][0] != '-'))
		strcpy(freezefile, argv[1]);
	else
		usage();

	printf("cwd is %s\n", getcwd(cwdbuf, MAX_PATH));
	cdToAppDir();
	printf("cwd is %s\n", getcwd(cwdbuf, MAX_PATH));

	err = dpCreate(&myDP, NULL, NULL, freezefile);
	if (err != dp_RES_OK) {
		printf("chat: error %d thawing %s.\n", err, freezefile);
		quit(myDP, 1);
	}
#if 0
	keylen = 1;
	key[0] = dp_KEY_PLAYER_LATENCIES;
	err = dpRequestObjectDeltas(myDP, TRUE, key, keylen);
	if (err != dp_RES_OK) {
		printf("error %d requesting latency deltas\n", err);
		quit(myDP, 1);
	}
#endif
	keylen = 1;
	key[0] = dp_KEY_SESSIONS;
	err = dpRequestObjectDeltas(myDP, TRUE, key, keylen);
	if (err != dp_RES_OK) {
		printf("error %d requesting session deltas\n", err);
		quit(myDP, 1);
	}

	err = dpGetSessionDesc(myDP, &sess, NULL);
	if (err != dp_RES_OK) {
		printf("error %d getting current session\n", err);
		quit(myDP, 1);
	}

	err = dpGetSessionId(myDP, &sess, &key[1], &keylen);
	if (err != dp_RES_OK) {
		printf("error %d getting current session ID\n", err);
		quit(myDP, 1);
	}

	key[0] = dp_KEY_PLAYERS;
	keylen++;
	err = dpRequestObjectDeltas(myDP, TRUE, key, keylen);
	if (err != dp_RES_OK) {
		printf("error %d requesting player deltas\n", err);
		quit(myDP, 1);
	}

	printf("You're in session %s", sess.sessionName);
#if 0
	{
		char	key[dp_KEY_MAXLEN];
		int		keylen;
		int		i;

		printf(" (id=");
		dpGetSessionId(myDP, &sess, key, &keylen);
		for (i = 0; i < keylen; i++) {
			printf("%x.", key[i] & 0xFF);
		}
		printf(")");
	}
#endif
	if (dpGetGameServerEx(myDP, masterHostName, sizeof(masterHostName), NULL) ==dp_RES_OK) {
		printf(", on server %s", masterHostName);
	}
	printf("\n");
	printf("To leave this session, press ^D.\n");
	printf("To force a divide-by-zero crash, press ^X.\n");

	while(1) {			/* input and receive loop */
		dpid_t idFrom;
		dpid_t idTo;
		struct {
			dp_packetType_t type PACK;
			union {
				dp_objectDelta_packet_t	delta;
				unsigned char buf[dpio_MAXLEN_UNRELIABLE];
			} u PACK;
		} pkt;
		size_t pktsize;

		if (quitState == 1) {
			/* Shutdown and quit out of chat */
			err = dpShutdown(myDP, 3000, 3000, 0);
			if (dp_RES_OK == err) {
				return 0;
			}
		}

		if (raw_kbhit()) {
			int len = strlen(kbuf);
			int ch = 0;
			if (len >= MAX_MESSAGE) {
				printf("\nchat: Message too long. Truncating and sending.\n");
				err = chat_broadcast(myDP, kbuf);
				if (err != dp_RES_OK) {
					printf("chat: error %d sending message.\n", err);
				}
				kbuf[0] = '\0';
				continue;
			}
			ch = raw_getc();
			if (ch == -1) {
				/* guess kbhit was wrong */
				continue;
			}
			if (ch == 4) {		/* ^D */
				printf("\nquitting ....\n");

				quitState = 1;
				if (ch == 17) {
					exitCode = 1;			/* if stub exists, tell it to die */
				}
				continue;
			}
			if (ch == 24) {			/* ^X  - force divide-by-zero crash */
				int i, j;
				j = 10;
				i = (int)strchr("a", 'b');		/* result is zero, but compiler doesn't know that */
				j /= i;
				printf("Result of division by zero is %d\n", j);
			}
			putchar(ch);
			fflush(stdout);
			if (ch == '\r') {					/* ^M */
				putchar('\n');
			}
			if (ch == 8) {						/* BSp */
				if (len > 0)
					kbuf[len-1] = 0;
				continue;
			}
			if (ch != '\r' && ch != '\n') {
				/* Append to buffer. */
				kbuf[len++] = ch;
				kbuf[len] = 0;
				continue;
			}
			/* Process the keyboard buffer. */
			if (!kbuf[0])
				continue;

			err = chat_broadcast(myDP, kbuf);
			if (err != dp_RES_OK) {
				printf("chat: error %d sending message.\n", err);
			}

			/* Empty it. */
			kbuf[0] = 0;
		}

	  	/* Receive and process packets */
		pktsize = sizeof(pkt);
		err = dpReceive(myDP, &idFrom, &idTo, 0, &pkt, &pktsize);
		assert(_CrtCheckMemory());
		switch (err) {
		case dp_RES_MODEM_NORESPONSE:
			printf("chat: modem reports NO CARRIER!\n");
			continue;
		case dp_RES_HOST_NOT_RESPONDING:
			printf("chat: connection to game server lost!\n");
			continue;
		case dp_RES_EMPTY:
			continue;
		case dp_RES_OK:
			break;
		default :
			printf("chat: error %d receiving packet.\n", err);
			continue;
		}
		switch (pkt.type) {
		case dp_OBJECTDELTA_PACKET_ID:
			if (pkt.u.delta.key[0] == dp_KEY_PLAYERS) {
				dp_objectDelta_packet_t *delta = &pkt.u.delta;
				const char *op;

				op = status2a(delta->status);
				if (dp_OBJECTDELTA_FLAG_LOCAL & delta->flags) {
					char *msg = "";
					if (dp_OBJECTDELTA_FLAG_ISHOST & delta->flags)
						msg = " -- and you're the host";
					printf("***: Player id:%d %s %s -- and it's you%s!\n",
							delta->data.p.id, delta->data.p.name, op, msg);
					my_id = delta->data.p.id;
				} else {
					printf("***: Player id:%d %s %s: latency:%dms, loss:%d%%, flags:%lx\n",
						delta->data.p.id, delta->data.p.name, op,
						delta->latency, delta->pktloss, delta->flags);
				}
			} else if (pkt.u.delta.key[0] == dp_KEY_SESSIONS) {
				dp_objectDelta_packet_t *delta = &pkt.u.delta;
				const char *op;

				op = status2a(delta->status);
				printf("session %s (sessType %d, k%d u%x f%x, cur %d, max %d) %s\n",
					delta->data.sess.sessionName, delta->data.sess.sessionType,
					delta->data.sess.karma, delta->data.sess.dwUser1,
					delta->data.sess.flags,
					delta->data.sess.currentPlayers,
					delta->data.sess.maxPlayers, op);
			}
			break;
		case dp_USER_HOST_PACKET_ID:
			printf("chat: Host is dead. You are now host.\n");
			break;
		case CHAT_PACKET_ID:
			{
				char nameBuf[256];
				char chatBuf[dpio_MAXLEN_UNRELIABLE];	/* overkill */
				size_t chatLen;
				err = dpGetPlayerName(myDP, idFrom, nameBuf, sizeof(nameBuf));
				if (err != dp_RES_OK) {
					printf("chat: chat packet from unknown id %d.\n", idFrom);
			}
			chatLen = pktsize-sizeof(dp_packetType_t);
			assert(chatLen < sizeof(chatBuf));
			memcpy(chatBuf, pkt.u.buf, chatLen);
			chatBuf[chatLen] = 0;					/* Nul terminate */
			DPRINT(("%s: %s\n", nameBuf, chatBuf));
			printf("%s: %s\n", nameBuf, chatBuf);
			break;
			}
		case dp_SESSIONLOST_PACKET_ID:
			printf("Sorry session lost, you might as well quit\n");
			break;
		default:
			break;
		}
	}
	quit(myDP, exitCode);
	return 1;
}

static LONG __cdecl Debug_ExceptionFilter(LPEXCEPTION_POINTERS ep)
{
	dpReportCrash(ep);

	if (ep->ExceptionRecord->ExceptionCode == aeh_ASSERTION_CODE)
		return EXCEPTION_CONTINUE_EXECUTION;

	return (EXCEPTION_CONTINUE_SEARCH);
}

int _cdecl main( int argc, char *argv[] )
{
	int retVal = 0;
	__try {
		retVal = chat(argc, argv);
	} __except(Debug_ExceptionFilter(GetExceptionInformation())) {
		;
	}

	return retVal;	/* to make compiler happy */
}
