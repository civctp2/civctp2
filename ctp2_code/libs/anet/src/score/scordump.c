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
 Program to connect to a server and dump the contents of it's score
 tables to the screen.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "dp2.h"
#include "scorerep.h"
#include "eclock.h"

#define PRINT(a) printf a

#define MSG_CONNECTING "Connecting %d"
#define MSG_LAUNCHING  "Launching"
#define MSG_ERR_LAUNCH "Error %d launching game at line %d"
#define MSG_WIND_TITLE "Game Launcher"
#define MSG_ERROR_TITLE "Error"

#define MAXARGS 10
#define DQUOTECHAR '"'
#define SPACECHAR (char)0x20
#define TABCHAR   (char)0x08

/*--------------------------------------------------------------------------
 Specific application code goes below here.
--------------------------------------------------------------------------*/

static int callbacksFinished;
static int openFinished;
static dpid_t my_id;
static dp_session_t theSession;

static void usage()
{
	static char *szUsage = "\
scordump: dump the contents of a server's score database\n\
 Options:\n\
-n=path            	# which transport to use, e.g. -n=anet/winets2.dll\n\
-h                 	# host a session\n\
-m=maxplayers      	# Max number of players in session\n\
-j                 	# join a session\n\
-J=16              	# join any internet session (16=lobby, 0=game)\n\
-s=mygame          	# session name\n\
-p=myname          	# player name\n\
-a=206.43.1.0      	# address (of host; used when joining if no broadcast)\n\
-g=server_name  	# Name of the server to connect to \n\
-u=username passwd  # username and password for login\n\
-v               	# Allow player variables\n\
-w                 	# Don't exit until command does; return its exit code.\n\
-x                 	# Let launched program init comm system\n\
-b=9600            	# Baud rate\n\
-c=0               	# COM port to use (0=COM1)\n\
-f=5555555        	# Phone number to dial when using modem, omit to answer\n\
-i=ATZ             	# Modem initialization string\n\
-d=1               	# Dial method (1=Tone, 0=Pulse)\n\
-o=206.41.1.1;206.41.1.2 # open connections to the list of addresses\n\
-o=@ADDRESSFILE          # open connections to the address in the file ADDRESSFILE\n\
\n\
 Example:\n\
   scordump -n=anet/winets2d.dll -h -g=california4 -u=myname mypassword\n\
";
	PRINT((szUsage));
	exit(1);
}

int dp_PASCAL myCommThaw_cb(int status, void *context)
{
	static int status_old = -1;

	if (status != status_old) {
		printf("Connecting status:%d\n", status);
		status_old = status;
	}
	return TRUE;  /* don't cancel */
}

int main(int argc, char **argv)
{
	int argi;
	dp_t *dp = NULL;
	dp_result_t err;
	dp_appParam_t app;
	dp_launchParams_t params;
	int let_game_init_comm = FALSE;

	char	GameName[MAX_PATH]="";
	char	GamePath[MAX_PATH]="";
	char	GameArgs[MAX_PATH]="";
	char	GameCwd[MAX_PATH]="";
	char	GameShellOpts[MAX_PATH]="";

	FILE *fp;

	app.name = GameName;
	app.path = GamePath;
	app.args = GameArgs;
	app.cwd = GameCwd;
	app.shellOpts = GameShellOpts;

	memset(&params, 0, sizeof(params));
	params.Maxplayers=4;

	for (argi=1; argi<argc; argi++) {
		int c = argv[argi][0];
		int c1 = argv[argi][1];
		int c2 = argv[argi][2];
		char *arg;

		if (c != '-') continue;

		/* Parse argument, if present */
		/* Allow either = or : for argument separator */
		arg = NULL;
		if ((c2 == '=') || (c2 == ':'))
			arg = argv[argi]+3;

		/* Verify argument for those options that need it */
		if (!arg && (strchr("nspamgJu", c1))) {
			PRINT(("Bad argument '%s'; -%c requires parameter\n", argv[argi], c1));
			usage();
		}
		if (c1 == 'u' && argc-argi < 2) {
			PRINT(("Bad argument '%s'; -u requires 2 parameters\n", argv[argi])); 
		}

		switch (c1) {
		case 'b': params.commInitReq.baud=atoi(arg); break;
		case 'c': params.commInitReq.portnum=atoi(arg); break;
		case 'f': strcpy(params.Phonenum, arg); break;
		case 'i': strcpy(params.Modeministr, arg); break;
		case 'd': params.commInitReq.dialing_method=atoi(arg);break;
		case 'h': params.Host=1; break;
		case 'j': if (params.Join != 0) {
					PRINT(("Must specify only one of -j or -z.\n"));
				  	usage();
				  }
				  params.Join=1; break;
		case 'J': if (params.Join != 0) {
					PRINT(("Must specify only one of -j or -z.\n"));
				  	usage();
				  }
				  params.JoinAnyFlags=atoi(arg);
				  params.Join=dp_LAUNCHPARAMS_JOIN_ANY | 0x01; break;
		case 'w': params.Wait=1; break;
		case 'v': params.EnablePlayervars=1; break;
		case 'x': let_game_init_comm=TRUE; break;
		case 'n': strcpy(params.Driver,arg); break;
		case 's': strcpy(params.Sessname, arg); break;
		case 'p': strcpy(params.Playname, arg); break;
		case 'a': strcpy(params.Adr,arg); break;
		case 'm': params.Maxplayers = atoi(arg); break;
		case 'g': strcpy(params.GameServer,arg); break;
		case 'u': strncpy(params.Username, arg, 16);
			      argi++;
				  strncpy(params.Password, argv[argi], 16);
				  break;
		case 'o':
		{
			/*
			 * Addresses are separated by ';'
			 * We convert this to the following
			 *
			 * "text address1"\0
			 * "text address2"\0
			 * "text address3"\0
			 * \0
			 *
			 */
			char	*ptr;

			params.commInitReq.flags |= comm_INIT_FLAGS_CONN_ADDR;
			strcpy(params.OpenAddresses, arg);

			/*
			 * If the first character of the arg is '@' then the argument is a file
			 */
			if (*arg == '@')
			{
				/*
				 * The file MUST be in the following format
				 *
				 * "text address1";"text address2";"text address3"EOF
				 */
				FILE *fp;

				fp = fopen(arg + 1, "rt");

				if (fp == NULL)
				{
					PRINT(("Could not open address file \"%s\"", arg + 1));
					usage();
				}

				if (fread(params.OpenAddresses, 1, 1024, fp) == 0)
				{
					PRINT(("File \"%s\" contains no data!", arg + 1));
					usage();
				}

				fclose(fp);
			}

			/*
			 * Add \0 to end
			 */
			ptr = params.OpenAddresses + strlen(params.OpenAddresses) + 1;
			*ptr = '\0';

			/*
			 * Convert ';' to \0
			 */
			ptr = params.OpenAddresses;
			while (ptr = strchr(ptr, ';'))
			{
				*ptr = '\0';
				ptr++;
			}
			break;
		}

		default:
			PRINT(("bad argument '%s'\n", argv[argi]));
			usage();
		}
	}

	/* Got arguments. */
	/* Check arguments. */
	if (params.Adr[0] && !params.Host && params.Join)
		params.Join |= dp_LAUNCHPARAMS_JOIN_ANY;
	if (params.Host + (params.Join & 0x01) != 1) {
		PRINT(("Must specify either host or join with -h or -j or -z.\n"));
		usage();
	}
	if (!params.Driver[0]) {
		PRINT(("Must specify a driver with -n.\n"));
		usage();
	}
	if ((params.Maxplayers < 2) || (params.Maxplayers > dp_MAXREALPLAYERS)) {
		PRINT(("params.Maxplayers must be between 2 and %d.\n", dp_MAXREALPLAYERS));
		usage();
	}
	if (params.GameServer[0] && params.Adr[0]) {
		PRINT(("cannot specify both address and game server.\n"));
		usage();
	}
	if ((params.Join & dp_LAUNCHPARAMS_JOIN_ANY) &&
		!(params.GameServer[0] || params.Adr[0])) {
		PRINT(("cannot specify joinany without specifying either address or game server.\n"));
		usage();
	}
	if ((params.Join & dp_LAUNCHPARAMS_JOIN_ANY) &&
		(params.JoinAnyFlags != 16) && params.GameServer[0]) {
		PRINT(("on game server, can specify joinany only with lobby session.\n"));
		usage();
	}

	if (!params.Playname[0]) {
		if (params.Host)
			strcpy(params.Playname, "host");
		else
			strcpy(params.Playname, "player");
	}

    params.commInitReq.sessionId = rand() ^ (rand() << 16) ^ time(0) ^ clock();
	params.commInitReq.reqLen = sizeof(commInitReq_t);

	fp = fopen("freeze.dat", "wb");
	if (!fp) {
		PRINT((MSG_ERR_LAUNCH , dp_RES_BAD, __LINE__));
		exit(1);
	}
	if ((fwrite(dp_LAUNCHPARAMS_T_SIG, sizeof(dp_LAUNCHPARAMS_T_SIG), 1, fp) != 1) ||
		(fwrite(&params, sizeof(params), 1, fp) != 1) ||
		(fwrite(dp_LAUNCHPARAMS_T_SIG, sizeof(dp_LAUNCHPARAMS_T_SIG), 1, fp) != 1)) {
		PRINT((MSG_ERR_LAUNCH , dp_RES_BAD, __LINE__));
		exit(1);
	}
	fclose(fp);

	if (!let_game_init_comm) {
		fp = fopen("freeze.dat", "rb");
		if (!fp) {
			PRINT((MSG_ERR_LAUNCH, dp_RES_BAD, __LINE__));
			exit(1);
		}
		err = dpCommThaw(&dp, fp, myCommThaw_cb, NULL);
		if (err != dp_RES_OK) {
			PRINT((MSG_ERR_LAUNCH, err, __LINE__));
			exit(1);
		}
		fclose(fp);

		assert(dp);
		dp->now = eclock();
	}

	if (dp) {
		int quitState = 0;
		clock_t tfinish;
		clock_t deadline = dp->now + 5 * ECLOCKS_PER_SEC;
		char key[dp_KEY_MAXLEN];
		key[0] = dp_KEY_SCORES;
		
		err = dpRequestObjectDeltas(dp, TRUE, key, 1);
		assert(!err);
		
		while (1) {
			dpid_t idFrom, idTo;
			char pktbuf[dpio_MAXLEN_UNRELIABLE];
			size_t rxsize = dpio_MAXLEN_UNRELIABLE;

			err = dpReceive(dp, &idFrom, &idTo, 0, &pktbuf, &rxsize);
			switch (err) {
			case dp_RES_MODEM_NORESPONSE:
				printf("scordump: modem reports NO CARRIER!\n");
				break;
			case dp_RES_HOST_NOT_RESPONDING:
				printf("scordump: connection to game server lost!\n");
				break;
			case dp_RES_EMPTY:
				break;
			case dp_RES_OK:
				break;
			default :
				printf("scordump: error %d receiving packet.\n", err);
				break;
			}
			if (err == dp_RES_OK) {
				dp_objectDelta_packet_t *delta;
				
				switch (*((dp_packetType_t *)pktbuf)) {	
				case dp_OBJECTDELTA_PACKET_ID:
					delta = (dp_objectDelta_packet_t *)(pktbuf + sizeof(dp_packetType_t));
					if (delta->key[0] == dp_KEY_SCORES && delta->keylen == 3) {
						scorerep_player_t player;
						dp_scoreInfo_t *psi;
						scorerep_buf_t repbuf;
						dp_species_t gameType;
						dp_uid_t uid;
						int i;

						gameType = dpMAKESHORT(delta->key[1], delta->key[2]);
						uid = dpMAKELONG(delta->subkey[0], delta->subkey[1],
										 delta->subkey[2], delta->subkey[3]);
						psi = &(delta->data.score);
						PRINT(("Got SCORE delta: flags:%d status:%d species:%d uid:%d nScoreTypes %d\n", delta->flags, delta->status, gameType, uid, psi->nScoreTypes));
						repbuf.len = sizeof(psi->nScoreTypes) + psi->nScoreTypes *
							(sizeof(psi->scoreIds[0]) + sizeof(psi->scores[0]));
						memcpy(repbuf.buf, psi, repbuf.len);
						
						err = scorerep_player_fromBuf(&player, &repbuf);
						assert(!err);
						assert(player.scores);
						for (i = 0; i < player.scores->n_used; i++) {
							int scoreId;
							long scoreVal;
							assoctab_item_t *pe = assoctab_getkey(player.scores, i);
							assert(pe);
							scoreId = pe->key;
							scoreVal = *((long *)pe->value);
							PRINT((" %x:%d", scoreId, scoreVal));
						}
						PRINT(("\n"));
						deadline = dp->now + 5 * ECLOCKS_PER_SEC;
					}
				}
			} else if (err != dp_RES_EMPTY) {
				printf("scorsend: dpReceive returns err:%d\n", err);
				break;
			}

#if 0			
			if (!quitState && ((long)(eclock() - deadline) > 0))
				quitState = 3;
#endif
			
			/* no player to delete, so just close. */
			if (quitState == 3) {
				printf("scorsend: dpClose() closing session...\n");
				dpClose(dp);
				quitState++;
				tfinish = eclock() + ECLOCKS_PER_SEC / 4;
			} else if (quitState == 4) {
				/* Wait a little longer to give dpClose a time to send packets */
				if ((long)(eclock() - tfinish) > 0)
					quitState++;
			} else if (quitState == 5) {
				/* Have table transfers and network traffic finished yet? */
				if (dpReadyToFreeze(dp) != dp_RES_BUSY)
					quitState++;
			} else if (quitState == 6) {
				printf("scorsend: dpSetGameServer(NULL) closing connection...\n");
				dpSetGameServerEx(dp, NULL, 0);
				quitState++;
			} else if (quitState == 7) {
				/* Has closing associated network traffic finished yet? */
				if (dpReadyToFreeze(dp) != dp_RES_BUSY) {
					quitState++;
					tfinish = eclock() + ECLOCKS_PER_SEC / 4;
				}
			} else if (quitState != 0) {
				/* Wait a little longer in case we must retransmit anything */
				if ((long)(eclock() - tfinish) > 0) {
					printf("scorsend: done (quitState:%d)\n", quitState);
					return 0; /* break out of the main loop, and call dpDestroy */
				}
			}
		}
	}

	return 0;
}
