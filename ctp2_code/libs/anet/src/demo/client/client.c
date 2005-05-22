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
 Example text-based client for Active.Net.
 Trivial user interface.  Lets you execute ActiveNet API calls
 interactively.
 Demonstrates basic functionality of connecting to game server.
 Does not demo peer-to-peer lobby functionality.

 Plunks user down in common chat area.
 Launches netsnake or rawtest upon command.

 $Log: client.c $
 Revision 1.2  1997/01/06 21:01:37  dkegel
 Minor changes - error msg, help text.
 Revision 1.1  1997/01/02 19:22:28  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#include <conio.h>
#include <stdio.h>
#include <ctype.h>
#include "dp.h"

#define DEBUG_MODULE TRUE   /*  enable DPRINT in this file */
#include "dprint.h"

static char		adrString[256];
static int		packetLen;
char        	CommDLLName[256];
commInitReq_t	commInitReq;		/*  Parameters needed by some comm drivers. */
char			modeministr[256];
char			phonenum[256];
dpid_t          myGameHost = dp_ID_NONE;
dpid_t          myID = dp_ID_NONE;
dp_t            *myDP;

int pleaseQuit = 0;

#define SPECIES_LOBBY 100		/* unique identifier for our lobby */
#define SPECIES_NETSNAKE 101	/* unique identifier for netsnake */
#define SPECIES_MERCNET 1010	/* unique identifier for mercnet 1.05 */

/* Define client-specific packet types here. */
#define CHAT_PACKET_ID		dppt_MAKE('C','H')

dp_species_t cur_sessType = SPECIES_MERCNET;	/* Start off with Mercnet lobby */

/*-------------------------------------------------------------------------
 Display text in a chat window.
-------------------------------------------------------------------------*/
void stuffChatText(char *from, char *txt)
{
	printf("%s: %s\n", from, txt);
	DPRINT(("sct: %s: %s\n", from, txt));
	fflush(stdout);
}

/*-------------------------------------------------------------------------
 Stuff to support ErasePlayerByName().
-------------------------------------------------------------------------*/
typedef struct {
	dp_result_t err;
	int count;
	char *name;
} erasePlayerContext_t;

void erasePlayerCallback(dpid_t id, char_t *name, long flags, void *context)
{
	erasePlayerContext_t *pc = context;
	char buf[512];

	if ((id != myGameHost)
	&& (strnicmp(pc->name, name, strlen(name)) == 0)) {
		DPRINT(("ErasePlayerCallback: found player %s, id %d\n", name, id));
		sprintf(buf,"Deleting player %s.", name);
		stuffChatText("****", buf);
		pc->err = dpDestroyPlayer(myDP, id);
		pc->count++;
	}
	(void) flags;
}

/*--------------------------------------------------------------------------
 Erase one or more players from the current game.
 Abbreviated names are ok.
 Refuses to kick the game host out of the game
--------------------------------------------------------------------------*/
void ErasePlayerByName(char *name)
{
	dp_result_t err;
	erasePlayerContext_t pc;
	char buf[512];

	pc.err = dp_RES_OK;
	pc.count = 0;
	pc.name = name;

	DPRINT(("ErasePlayerByName: trying to erase %s\n", name));
	err = dpEnumPlayers(myDP, NULL, erasePlayerCallback, &pc, 0);

	if (err != dp_RES_OK) {
		DPRINT(("ErasePlayerByName: can't list players, err %d\n", err));
	} else if (pc.err != dp_RES_OK) {
		DPRINT(("ErasePlayerByName: error destroying player, err %d\n", err));
	}
	if (pc.count == 0) {
		DPRINT(("ErasePlayerByName: no player matching %s\n", name));
		sprintf(buf,"No player named %s.", name);
		stuffChatText("****", buf);
	}
}

/*-------------------------------------------------------------------------
 Stuff to support PingPlayerByName().
-------------------------------------------------------------------------*/
void ping_callback(dp_karma_t karma, time_t avg_ms, int loss_pct)
{
	char buf[256];

	DPRINT(("ping_callback: karma %d, avg_ms %d, loss_pct %d\n",
		karma, avg_ms, loss_pct));
	sprintf(buf, "ping: average delay: %d ms, loss: %d%%", avg_ms, loss_pct);
	stuffChatText("*****", buf);

}

typedef struct {
	dp_result_t err;
	int count;
	char *name;
} pingPlayerContext_t;

void pingPlayerCallback(dpid_t id, char_t *name, long flags, void *context)
{
	pingPlayerContext_t *pc = context;
	char buf[512];
	int karma = time(0);	/* unique identifier for this ping */

	if ((pc->count == 0)
	&&  (strnicmp(pc->name, name, strlen(name)) == 0)) {
		sprintf(buf, "Pinging %s...", name);
		stuffChatText("*****", buf);
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
	erasePlayerContext_t pc;
	char buf[512];

	pc.err = dp_RES_OK;
	pc.count = 0;
	pc.name = name;

	DPRINT(("PingPlayerByName: trying to Ping %s\n", name));
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

/* Called when user gives /list command. */
void enumSessionCallback(dp_session_t *sDesc,void *context)
{
	char buf[300];
	if (!sDesc) {
		stuffChatText("****", "End of session list");
		return;
	}


	sprintf(buf, "(%04x; %d players) %s", sDesc->karma, sDesc->currentPlayers, sDesc->sessionName);
	stuffChatText("****", buf);
	(void) context;
}

/*-------------------------------------------------------------------------
 Callback triggered by dpOpen when creating a session.
-------------------------------------------------------------------------*/
void	create_sess_cb(dp_session_t *ps, void *context)

{
	if (ps)
		stuffChatText("*****", "Created session.");
	else
		stuffChatText("*****", "Failed to create session.");

	(void) context;
}

/*-------------------------------------------------------------------------
 Given a line of text from the user, if it's a slash command, execute it,
 and return 1.  Otherwise, return 0.
-------------------------------------------------------------------------*/
int processChatCommand(const char *szChatText)
{
	char *arg1, *arg2, *c, string[255], buf[255];
	dp_session_t  sess;
	dp_result_t err;

	if (szChatText[0] != '/')
		return FALSE;

	strcpy(string, szChatText+1);
	DPRINT(("processChatCommand: %s\n", szChatText));
	arg1 = string;
	c = strchr(string, ' ');		// look for whitespace
	if (c != NULL) {
		*c = 0;						// null terminate
		while (isspace(*++c)) {};	// seek to end of whitespace
		arg2 = c;
	} else {						// couldn't find whitespace
		arg2 = "";					// point to empty string
	}
	DPRINT(("processChatCommand: arg1 %s, arg2 %s\n", arg1, arg2));

	if (stricmp(arg1, "kick") == 0) {
		// kludge
		if (myDP->s.masterPseudoplayer != myDP->my_players[0].id) {
			stuffChatText("*****", "Sorry, only the host can kick players.");
			return TRUE;
		}
		if (!arg2[0]) {
			stuffChatText("*****", "/kick: you need to specify a callsign.");
			return TRUE;
		}
		sprintf(buf, "Kicking first player matching \"%s\"", arg2);
		stuffChatText("*****", buf);
		ErasePlayerByName(arg2);

	} else if (stricmp(arg1, "ping") == 0) {
		DPRINT(("processChatCommand: request to ping %s\n", arg2));
		if (!arg2[0]) {
			stuffChatText("*****", "/ping: you need to specify a callsign.");
			return TRUE;
		}
		sprintf(buf, "Pinging first player matching \"%s\"", arg2);
		stuffChatText("*****", buf);
		PingPlayerByName(arg2);

	} else if (stricmp(arg1, "list") == 0) {
		/* List game sessions */
		sess.sessionType = cur_sessType;
		stuffChatText("****", "Listing sessions:");
		err = dpEnumSessions(myDP, &sess, NULL, 1750L, enumSessionCallback, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "error %d calling dpEnumSessions", err);
			stuffChatText("*****", buf);
		}

	} else if (stricmp(arg1, "quit") == 0) {
		pleaseQuit = TRUE;

	} else if (stricmp(arg1, "join") == 0) {
#if 0
		/* Join a particular session.  User specifies session name. */
		DPRINT(("processChatCommand: request to join %s\n", arg2));
		if (!arg2[0]) {
			stuffChatText("*****", "/join: you need to specify a room name.");
			return TRUE;
		}
		sprintf(buf, "Joining first room matching \"%s\"", arg2);
		stuffChatText("*****", buf);
		JoinGameByName(arg2);
#endif


	} else if (stricmp(arg1, "host") == 0) {
		/* Create a room */
		sess.sessionType = cur_sessType;
		sess.maxPlayers = dp_MAXREALPLAYERS;
		strncpy(sess.sessionName, "Demo Room", dp_SNAMELEN);
		sess.sessionName[dp_SNAMELEN-1] = 0;
		sess.flags = dp_SESSION_FLAGS_CREATESESSION;
		sess.dwUser1 = 0;		/* Not a lobby */
		err = dpOpen(myDP, &sess, create_sess_cb, NULL);
		if (err != dp_RES_OK) {
			sprintf(buf, "Unable to create a session, error: %d", err);
			stuffChatText("*****", buf);
		}

	} else if (stricmp(arg1, "help") == 0) {
		stuffChatText("*****", "/game GAMENAME -- selects a new game.  Affects /list, /host, and /launch.");
		stuffChatText("*****", "/rooms         -- lists available rooms.");
		stuffChatText("*****", "/host          -- creates a new room (and launches current game).");
		stuffChatText("*****", "/join          -- joins an existing room (and launches current game).");
		stuffChatText("*****", "/players       -- list players in current room.");
		stuffChatText("*****", "/launch        -- take people in current session into the game proper");
		stuffChatText("*****", "/kick callsign -- lets the game host eject a player from game.");
		stuffChatText("*****", "/ping callsign -- measure round trip lag time to a player.");
		stuffChatText("*****", "/quit          -- terminate program");
		stuffChatText("*****", "Text not starting with a slash is sent as a chat message.");
	} else
		stuffChatText("*****", "Unknown command.  Type /help for a list of commands.");
	return TRUE;
}

// Display a chat message and broadcast it to all other players
//
void    SendChatMessage( char *szChatText )
{
	struct {
		dp_packetType_t type;
		char buf[200];
		char skip[6];	/* Caution: six bytes after buf trashed by dpSend! */
	} pkt;
	dp_result_t err;

	stuffChatText("me", szChatText);

	pkt.type = CHAT_PACKET_ID;
	strcpy(pkt.buf, szChatText);

	err = dpSend(myDP, myID, dp_ID_BROADCAST, dp_SEND_RELIABLE, &pkt, sizeof(pkt)-sizeof(pkt.skip));
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

  CommDLLName[0] = 0;
  adrString[0] = 0;
  packetLen=0;

  for (i = 1; i < argc; ++i) {
	if (argv[i][0] == '/' || argv[i][0] == '-') /*  deal with args that start with - or / */
	{
	  if (argv[i][1] == 0) {
		printf("Expected letter after -\n");
		exit(1);
	  }
	  if (argv[i][2] != '=') {
		printf("Expected = after -%c\n", argv[i][1]);
		exit(1);
	  }
	  switch(toupper(argv[i][1])) {
		case 'L':
			printf("Setting packetLen to %s\n", argv[i]+3);
			packetLen = atoi(argv[i]+3);
			break;
		case 'D':
			/*  Turn on some sort of debugging mode */
			break;

		case 'P':   /*  Server IP Address */
			if ((chptr = strchr(argv[i], '=')) == NULL) /*  No comm params specified. */
                          adrString[0] = '\0';
			else {
			  chptr++;
                          strncpy(adrString, chptr, sizeof(adrString));
                          adrString[sizeof(adrString)-1] = '\0';
			}
			break;
		case 'K':	/*  Comm parameters; Kport, Kirq, Kpulse */
			if (stricmp(argv[i], "-Kpulse") == 0) {
				/*  No need to set default case  */
				/*  TOUCH_TONE == 0 */
				/*  PULSE == 1 */
				commInitReq.dialing_method = 1;
				break;
			}
			if (stricmp(argv[i], "-Ktest") == 0) {
				/*  Do loopback test.  Other side must have -Ktest, too. */
				commInitReq.flags |= comm_INIT_FLAGS_TEST;
				break;
			}
			if ((chptr = strchr(argv[i], '=')) == NULL) {
				printf("-K flag requires -Kport=hexnum, -Kirq=intnum, -Kpulse, or -Ktest");
				exit(1);
			}
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
				*chptr = '=';
				printf("unknown parameter %s", argv[i]);
				exit(1);
			}
			break;

		case 'N':   /*  Communications DLL name */
			if ((chptr = strchr(argv[i], '=')) == NULL) /*  No comm DLL name specified. */
				CommDLLName[0] = '\0';
			else {
				chptr++;
				strncpy(CommDLLName, chptr, sizeof(CommDLLName));
				CommDLLName[sizeof(CommDLLName)-1] = '\0';
			}
			break;

		case 'I':   /*  Set modem init string */
			strncpy(modeministr, argv[i]+3, sizeof(modeministr));
			modeministr[sizeof(modeministr)-1] = '\0';
			break;

		case 'W':   /*  Set phone number */
			strncpy(phonenum, argv[i]+3, sizeof(phonenum));
			phonenum[sizeof(phonenum)-1] = '\0';
			break;

		case 'Y':   /*  Set comm port */
			sscanf(argv[i]+3, "%ld", &commInitReq.portnum);
			break;

		case 'Z':   /*  Set baud rate */
			sscanf(argv[i]+3, "%ld", &commInitReq.baud);
			break;

		default:
			printf("Usage: %s cmd [-N=comm.dll][-P=adrString]\n"
				"[-W=phonenum][-Y=commportnum(0 = com1)][-Z=baudrate][-I=modeministr]\n"
				"[-Kirq=irqnum][-Kport=hexportnum][-Kpulse][-Ktest]\n"
				"Unknown arg %s\n", argv[0], argv[i]);
		    exit(1);
	  } /*  end switch */
	}
  }
}

int main( int argc, char *argv[] )
{
	dp_result_t   err;
	commInitReq_t commInitReq;
	char kbuf[512];
	char buf[512];
	kbuf[0] = 0;

	if (argc == 1) {
		printf("Usage: %s [-N=comm.dll][-P=adrString]\n"
			"[-W=phonenum][-Y=commportnum(0 = com1)][-Z=baudrate][-I=modeministr]\n"
			"[-Kirq=irqnum][-Kport=hexportnum][-Kpulse][-Ktest]\n", argv[0]);
		exit(1);
	}

	memset(&commInitReq, 0, sizeof(commInitReq));
	commInitReq.sessionId = rand() | (rand() << 16);
	commInitReq.reqLen = sizeof(commInitReq_t);
	modeministr[0] = 0;
	phonenum[0] = 0;
	memset(&commInitReq, 0, sizeof(commInitReq));
	commInitReq.modeministr = modeministr;
	commInitReq.phonenum = phonenum;
	commInitReq.baud = 19200;
	commInitReq.hwirq = 0;
	commInitReq.portnum = 1;
	commInitReq.sessionId = rand() | (rand() << 16);	
	commInitReq.reqLen = sizeof(commInitReq_t);

	ProcessCommandLine(argc, argv);

#ifndef UNIX
	if (!CommDLLName[0]) {
		printf("No DLL was specified.\n");
		exit(1);
	}
#endif

	enableDebugPrint(TRUE);

	/* Initialize the networking layer */
	err = dpCreate(&myDP, CommDLLName, &commInitReq, NULL);
	if (err != dp_RES_OK) {
		printf("Unable to start communications");
		exit(1);
	}

	/* Specify what server to use to rendezvous with other players. */
	if (adrString[0])
		dpSetGameServer(myDP, adrString);

	srand(time(0));

	/* Sit there waiting for a user command.
	 */
	pleaseQuit = 0;
	while (!pleaseQuit) {
		dpid_t idFrom;
		dpid_t idTo;
		char nameBuf[256];
		struct {
			dp_packetType_t type;
			union {
				dp_user_addPlayer_packet_t addPlayer;
				dp_user_delPlayer_packet_t delPlayer;
				char buf[512];
			} u;
		} pkt;
		size_t size;
		int ch = 0;

		if (kbhit()) {
			/* Accumulate keys into keyboard buffer.  Allow backspace. */
			int len = strlen(kbuf);
			ch = getche();
			if (ch == 8) {
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
			if (!processChatCommand(kbuf))		/* command */
				SendChatMessage(kbuf);			/* chat */
			/* Empty it. */
			kbuf[0] = 0;
		}

		/* Housekeeping.  Notice players coming and going. */
		size = sizeof(pkt);
		err = dpReceive(myDP, &idFrom, &idTo, 0, &pkt, &size);

		if (err == dp_RES_EMPTY)
			continue;

		if (err != dp_RES_OK) {
			DPRINT(("Unable to receive a packet, error: %d\n", err));
			continue;
		}

		switch (pkt.type) {
		case dp_USER_ADDPLAYER_PACKET_ID:
			DPRINT(("Player '%s' (%d) added.  %d in room\n", pkt.u.addPlayer.name, idFrom, dpNumPlayers(myDP)));
			sprintf(buf, "%s enters.", pkt.u.addPlayer.name);
			stuffChatText("*****", buf);
			break;

		case dp_USER_DELPLAYER_PACKET_ID:
			DPRINT(("Player '%s' (%d) deleted.\n", pkt.u.delPlayer.name, idFrom));
			sprintf(buf, "%s leaves.", pkt.u.addPlayer.name);
			stuffChatText("*****", buf);
			break;

		case CHAT_PACKET_ID:
			err = dpGetPlayerName(myDP, idFrom, nameBuf, sizeof(nameBuf));
			stuffChatText(nameBuf, pkt.u.buf);
			break;


		default:
			DPRINT(("That's odd; got a %2.2s packet.\n", &pkt.type));
		}
	}

	dpDestroy(myDP, 0);	/* go ahead and hang up */

    return(0);
}
